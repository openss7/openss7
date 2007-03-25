/*****************************************************************************

 @(#) $RCSfile: mg.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2006/05/08 11:00:57 $

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

 Last Modified $Date: 2006/05/08 11:00:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mg.c,v $
 Revision 0.9.2.14  2006/05/08 11:00:57  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.13  2006/03/07 01:10:27  brian
 - binary compatible callouts

 Revision 0.9.2.12  2006/03/04 13:00:11  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: mg.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2006/05/08 11:00:57 $"

static char const ident[] =
    "$RCSfile: mg.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2006/05/08 11:00:57 $";

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>
#include <ss7/mgi.h>
#include <ss7/mgi_ioctl.h>

#define MG_DESCRIP	"SS7 MEDIA GATEWAY (MG) STREAMS MULTIPLEXING DRIVER."
#define MG_REVISION	"LfS $RCSfile: mg.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2006/05/08 11:00:57 $"
#define MG_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define MG_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MG_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MG_LICENSE	"GPL"
#define MG_BANNER	MG_DESCRIP	"\n" \
			MG_REVISION	"\n" \
			MG_COPYRIGHT	"\n" \
			MG_DEVICE	"\n" \
			MG_CONTACT
#define MG_SPLASH	MG_DESCRIP	"\n" \
			MG_REVISION

#ifdef LINUX
MODULE_AUTHOR(MG_CONTACT);
MODULE_DESCRIPTION(MG_DESCRIP);
MODULE_SUPPORTED_DEVICE(MG_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MG_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mg");
#endif
#endif				/* LINUX */

#ifdef LFS
#define MG_DRV_ID		CONFIG_STREAMS_MG_MODID
#define MG_DRV_NAME		CONFIG_STREAMS_MG_NAME
#define MG_CMAJORS		CONFIG_STREAMS_MG_NMAJORS
#define MG_CMAJOR_0		CONFIG_STREAMS_MG_MAJOR
#define MG_UNITS		CONFIG_STREAMS_MG_NMINORS
#endif				/* LFS */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		MG_DRV_ID
#define DRV_NAME	MG_DRV_NAME
#define CMAJORS		MG_CMAJORS
#define CMAJOR_0	MG_CMAJOR_0
#define UNITS		MG_UNITS
#ifdef MODULE
#define DRV_BANNER	MG_BANNER
#else				/* MODULE */
#define DRV_BANNER	MG_SPLASH
#endif				/* MODULE */

STATIC struct module_info mg_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC struct module_info mg_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC struct module_info mux_winfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC struct module_info mux_rinfo = {
	mi_idnum:DRV_ID,		/* Module ID number */
	mi_idname:DRV_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC streamscall int mg_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int mg_close(queue_t *, int, cred_t *);

STATIC struct qinit mg_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:mg_open,		/* Each open */
	qi_qclose:mg_close,		/* Last close */
	qi_minfo:&mg_rinfo,		/* Information */
};

STATIC struct qinit mg_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&mg_winfo,		/* Information */
};

STATIC struct qinit mux_rinit = {
	qi_putp:ss7_iput,		/* Read put (message from below) */
	qi_srvp:ss7_isrv,		/* Read queue service */
	qi_minfo:&mux_rinfo,		/* Information */
};

STATIC struct qinit mux_winit = {
	qi_putp:ss7_oput,		/* Write put (message from above) */
	qi_srvp:ss7_osrv,		/* Write queue service */
	qi_minfo:&mux_winfo,		/* Information */
};

MODULE_STATIC struct streamtab mginfo = {
	st_rdinit:&mg_rinit,		/* Upper read queue */
	st_wrinit:&mg_winit,		/* Upper write queue */
	st_muxrinit:&mux_rinit,		/* Lower read queue */
	st_muxwinit:&mux_winit,		/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Data Structures
 *
 *  =========================================================================
 */

/*
   generic linkage 
 */
#define SLIST_CONN(__s1, __s2) \
	struct { \
		size_t numb; \
		struct __s1 *list; \
		size_t conn; \
		size_t ready; \
	} __s2 \


struct df;				/* default */
struct mg;				/* media gateway upper stream */
struct se;				/* communications session */
struct lg;				/* connection leg */
struct cn;				/* connection */
struct ac;				/* action */
struct ch;				/* channel */
struct mx;				/* multiplexer lower stream */

typedef struct df {
	spinlock_t lock;		/* master lock */
	SLIST_HEAD (mg, mg);		/* list of MG structures */
	SLIST_HEAD (se, se);		/* list of SE structures */
	SLIST_HEAD (lg, lg);		/* list of LG structures */
	SLIST_HEAD (ch, ch);		/* list of CH structures */
	SLIST_HEAD (ac, ac);		/* list of AC structures */
	SLIST_HEAD (mx, mx);		/* list of MX structures */
	struct mg_timers_df timers;	/* default timers */
	struct mg_opt_conf_df config;	/* default configuration */
	struct mg_stats_df statsp;	/* default statistics periods */
	struct mg_stats_df stats;	/* default statistics */
	struct mg_notify_df notify;	/* default notificiations */
} df_t;

STATIC struct df master;

STATIC inline struct df *
df_lookup(ulong id)
{
	if (id)
		return (NULL);
	return (&master);
}

#define MAX_CH_NUMB 16
/*
   media gateway structure 
 */
typedef struct mg {
	STR_DECLARATION (struct mg);	/* stream declaration */
	SLIST_HEAD (ch, ch);		/* channel list */
	SLIST_HEAD (se, se);		/* session list */
	struct mg_timers_mg timers;	/* media gateway timers */
	struct mg_opt_conf_mg config;	/* media gateway configuration */
	struct mg_stats_mg statsp;	/* media gateway statistics periods */
	struct mg_stats_mg stats;	/* media gateway statistics */
	struct mg_notify_mg notify;	/* media gateway notificiations */
} mg_t;
#define MG_PRIV(__q) ((struct mg *)(__q)->q_ptr)

STATIC struct mg *mg_alloc_priv(queue_t *, struct mg **, dev_t *, cred_t *);
STATIC void mg_free_priv(struct mg *);
STATIC struct mg *mg_lookup(ulong);
#if 0
STATIC ulong mg_get_id(ulong);
#endif
STATIC struct mg *mg_get(struct mg *);
STATIC void mg_put(struct mg *);

/*
   communications session 
 */
typedef struct se {
	HEAD_DECLARATION (struct se);	/* head declaration */
	SLIST_HEAD (lg, lg);		/* list of legs joined with this SE */
	SLIST_LINKAGE (mg, se, mg);	/* MG this session belongs to */
	struct mg_timers_se timers;	/* session timers */
	struct mg_opt_conf_se config;	/* session configuration */
	struct mg_stats_se statsp;	/* session statistics periods */
	struct mg_stats_se stats;	/* session statistics */
	struct mg_notify_se notify;	/* session notificiations */
} se_t;

STATIC struct se *se_alloc_priv(ulong, struct mg *);
STATIC void se_free_priv(struct se *);
STATIC struct se *se_lookup(ulong, struct mg *);
STATIC ulong se_get_id(ulong);
STATIC struct se *se_get(struct se *);
STATIC void se_put(struct se *);

/*
   connection leg structure 
 */
typedef struct lg {
	HEAD_DECLARATION (struct lg);	/* head declaration */
	SLIST_HEAD (ch, ch);		/* channels */
	SLIST_LINKAGE (se, lg, se);	/* session this termination belongs to */
	struct mg_timers_lg timers;	/* leg timers */
	struct mg_opt_conf_lg config;	/* leg configuration */
	struct mg_stats_lg statsp;	/* leg statistics periods */
	struct mg_stats_lg stats;	/* leg statistics */
	struct mg_notify_lg notify;	/* leg notificiations */
} lg_t;

STATIC struct lg *lg_alloc_priv(ulong id, struct se *se);
STATIC void lg_free_priv(struct lg *);
STATIC struct lg *lg_lookup(ulong, struct se *);
STATIC ulong lg_get_id(ulong);
STATIC struct lg *lg_get(struct lg *);
STATIC void lg_put(struct lg *);

/*
   connection structure 
 */
typedef struct cn {
	HEAD_DECLARATION (struct cn);	/* head declaration */
	size_t samples;			/* samples buffered */
	mblk_t *buf;			/* buffered bytes */
	ulong pad;			/* connection padding */
	SLIST_LINKAGE (ch, cn, ic);	/* I/C channel linkage */
	SLIST_LINKAGE (ch, cn, og);	/* O/G channel linkage */
} cn_t;

STATIC struct cn *cn_alloc_priv(struct ch *ic, struct ch *og, ulong pad);
STATIC void cn_free_priv(struct cn *);

#define MGF_PULL_PENDING	0x00000001
#define MGF_PUSH_PENDING	0x00000002
#define MGF_OG_READY		0x00000004
#define MGF_IC_READY		0x00000008

/*
   action structure 
 */
typedef struct ac {
	HEAD_DECLARATION (struct ac);	/* head declaration */
	size_t samples;			/* pattern samples */
	mblk_t *buf;			/* pattern buffer */
	ulong pad;			/* pattern padding */
	size_t bytes;			/* pattern bytes */
	ulong pos;			/* pattern position */
	ulong duration;			/* pattern duration */
	SLIST_LINKAGE (ch, ac, ch);	/* O/G channel linkage */
} ac_t;

STATIC struct ac *ac_alloc_priv(ulong, ulong, ulong, ulong, struct ch *, mblk_t *);
STATIC void ac_free_priv(struct ac *);
STATIC ulong ac_get_id(ulong);

/*
   channel structure 
 */
typedef struct ch {
	HEAD_DECLARATION (struct ch);	/* head declaration */
	ulong mx_slot;			/* slot in multiplex */
	SLIST_LINKAGE (mx, ch, mx);	/* multiplex we belong to */
	ulong lg_slot;			/* slot in connection leg */
	SLIST_LINKAGE (lg, ch, lg);	/* connection leg we belong to */
	SLIST_CONN (cn, og);		/* O/G connection list */
	SLIST_CONN (cn, ic);		/* I/C connection list */
	SLIST_CONN (ac, ac);		/* Action list */
	struct mg_timers_ch timers;	/* channel timers */
	struct mg_opt_conf_ch config;	/* channel configuration */
	struct mg_stats_ch statsp;	/* channel statistics periods */
	struct mg_stats_ch stats;	/* channel statistics */
	struct mg_notify_ch notify;	/* channel notificiations */
} ch_t;

STATIC struct ch *ch_alloc_priv(ulong, struct mx *, ulong, ulong);
STATIC void ch_free_priv(struct ch *);
STATIC struct ch *ch_lookup(ulong);
STATIC ulong ch_get_id(ulong);
STATIC struct ch *ch_get(struct ch *);
STATIC void ch_put(struct ch *);

/*
   multiplex structure 
 */
typedef struct mx {
	STR_DECLARATION (struct mx);	/* stream declaration */
	SLIST_CONN (ch, ch);		/* channel list */
	struct mg_timers_mx timers;	/* multiplex timers */
	struct mg_opt_conf_mx config;	/* multiplex configuration */
	struct mg_stats_mx statsp;	/* multiplex statistics periods */
	struct mg_stats_mx stats;	/* multiplex statistics */
	struct mg_notify_mx notify;	/* multiplex notificiations */
} mx_t;
#define MX_PRIV(__q) ((struct mx *)(__q)->q_ptr)

STATIC struct mx *mx_alloc_link(queue_t *, struct mx **, ulong, cred_t *);
STATIC void mx_free_link(struct mx *);
STATIC struct mx *mx_lookup(ulong);
STATIC ulong mx_get_id(ulong);
STATIC struct mx *mx_get(struct mx *);
STATIC void mx_put(struct mx *);

STATIC struct mg_opt_conf_mx mg_default = {
	flags:0,
	encoding:MX_ENCODING_G711_PCM_A,
	block_size:64,
	sample_size:8,
	samples:8,
	rate:8000,
	tx_channels:1,
	rx_channels:1,
};

/*
 *  =========================================================================
 *
 *  UTILITY FUNCTIONS
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  CONVERSION Functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  16-bit magnitude        A-law
 *  ----------------        --------
 *  .0000000wxyz....        s000wxyz
 *  .0000001wxyz....        s001wxyz
 *  .000001wxyz.....        s010wxyz
 *  .00001wxyz......        s011wxyz
 *  .0001wxyz.......        s100wxyz
 *  .001wxyz........        s101wxyz
 *  .01wxyz.........        s110wxyz
 *  .1wxyz..........        s111wxyz
 */
STATIC inline int16_t
pcma2pcml(uint8_t s)
{
	int seg, lval;
	s ^= 0x55;
	seg = (s >> 4) & 0x07;
	lval = ((seg ? 0x084 : 0x004) | ((uint16_t) (s & 0x0f) << 3)) << (seg ? seg : 1);
	return ((s & 0x80) ? lval : -lval);
}
STATIC inline uint8_t
pcml2pcma(int16_t s)
{
	int seg, shft;
	uint8_t aval, sign;
	uint16_t tst, mag;
	if (s < 0) {
		mag = -s - 8;
		sign = 0x00;
	} else {
		mag = s;
		sign = 0x80;
	}
	for (seg = 8, tst = 0x8000; !(tst & mag) && seg; tst >>= 1, seg--) ;
	if (seg < 8) {
		shft = seg ? seg + 3 : 4;
		aval = (seg << 4) | ((mag >> shft) & 0x0f);
	} else
		aval = 0x7f;
	return ((aval | sign) ^ 0x55);
}
STATIC inline uint8_t
pcmu2pcma(uint8_t s)
{
	static uint8_t utoa[] = {
		0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04,
		0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x08, 0x08,
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
		0x19, 0x1b, 0x1d, 0x1f, 0x21, 0x22, 0x23, 0x24,
		0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
		0x2e, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
		0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
		0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
		0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
		0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
		0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
		0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
		0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
		0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80
	};
	return ((s & 0x80) ? (0xd5 ^ (utoa[s ^ 0xff] - 1)) : (0x55 ^ (utoa[s ^ 0x7f] - 1)));
}

/*
 *  16-bit biased mag       u-law
 *  ----------------        --------
 *  .000000010000100 bias   s0000000
 *  .00000001wxyz...        s000wxyz
 *  .0000001wxyz....        s001wxyz
 *  .000001wxyz.....        s010wxyz
 *  .00001wxyz......        s011wxyz
 *  .0001wxyz.......        s100wxyz
 *  .001wxyz........        s101wxyz
 *  .01wxyz.........        s110wxyz
 *  .1wxyz..........        s111wxyz
 *  1000000010000100        s1111111
 */
STATIC inline int16_t
pcmu2pcml(uint8_t s)
{
	int seg, lval;
	s ^= 0xff;
	seg = (s >> 4) & 0x07;
	lval = ((0x084 | ((uint16_t) (s & 0x0f) << 3)) << seg) - (33 << 2);
	return ((s & 0x80) ? -lval : lval);
}
STATIC inline uint8_t
pcml2pcmu(int16_t s)
{
	int seg, shft;
	uint8_t uval, sign;
	uint16_t tst, mag;
	if (s < 0) {
		mag = -s;
		sign = 0x00;
	} else {
		mag = s;
		sign = 0x80;
	}
	mag += (33 << 2);
	for (seg = 8, tst = 0x8000; !(tst & mag) && seg; tst >>= 1, seg--) ;
	if (seg < 8) {
		shft = seg + 3;
		uval = (seg << 4) | ((mag >> shft) & 0x0f);
	} else
		uval = 0x7f;
	return ((uval | sign) ^ 0xff);
}
STATIC inline uint8_t
pcma2pcmu(uint8_t s)
{
	static uint8_t atou[] = {
		0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0e,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
		0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
		0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
		0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x30, 0x31, 0x31,
		0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
		0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x40,
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
		0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x4f,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
		0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f
	};
	return ((s & 0x80) ? (0xff ^ atou[s ^ 0xd5]) : (0x7f ^ atou[s ^ 0x55]));
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DATA Handling Functions
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  CONVERT MSG
 *  -----------------------------------
 *  Convert a message of samples from the incoming format to the outgoing format.  We don't handle a whole lot of
 *  formats here, and PCM_L is actually 16-bit clamped signed native endian rather than 12-bit G711 linear.
 */
STATIC int
mg_convert_msg(queue_t *q, mblk_t *dp, struct cn *cn, ulong ic_type, ulong og_type)
{
	if (pullupmsg(dp, -1)) {
		mblk_t *mp;
		size_t bytes, samples;
		uint pad = cn->pad;
		switch (og_type) {
		default:
			swerr();
		case MX_ENCODING_NONE:
			samples = msgsize(dp);
			bytes = samples;
			if (cn->ic.ch->ic.conn == 1)
				mp = dp;
			else
				mp = ss7_dupmsg(q, dp);
			break;
		case MX_ENCODING_G711_PCM_L:
		{
			int16_t *op;
			switch (ic_type) {
			default:
				swerr();
			case MX_ENCODING_NONE:
				bytes = msgsize(dp);
				samples = bytes >> 1;
				if (cn->ic.ch->ic.conn == 1)
					mp = dp;
				else
					mp = ss7_dupmsg(q, dp);
				break;
			case MX_ENCODING_G711_PCM_L:
				bytes = msgsize(dp);
				samples = bytes >> 1;
				if (!pad) {
					if (cn->ic.ch->ic.conn == 1)
						mp = dp;
					else
						mp = ss7_dupmsg(q, dp);
				} else if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					int16_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					while (samples--)
						*op++ = (*ip++ >> pad);
				}
				break;
			case MX_ENCODING_G711_PCM_A:
				samples = msgdsize(dp);
				bytes = samples << 1;
				if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					uint8_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					while (samples--)
						*op++ = (int16_t) pcma2pcml(*ip++) >> pad;
				}
				break;
			case MX_ENCODING_G711_PCM_U:
				samples = msgdsize(dp);
				bytes = samples << 1;
				if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					uint8_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					while (samples--)
						*op++ = (int16_t) pcmu2pcml(*ip++) >> pad;
				}
				break;
			}
			break;
		}
		case MX_ENCODING_G711_PCM_A:
		{
			uint8_t *op;
			switch (ic_type) {
			default:
				swerr();
			case MX_ENCODING_NONE:
				samples = msgsize(dp);
				bytes = samples;
				if (cn->ic.ch->ic.conn == 1)
					mp = dp;
				else
					mp = ss7_dupmsg(q, dp);
				break;
			case MX_ENCODING_G711_PCM_L:
				samples = msgsize(dp) >> 1;
				bytes = samples;
				if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					int16_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					while (samples--)
						*op++ = pcml2pcma(*ip++ >> pad);
				}
				break;
			case MX_ENCODING_G711_PCM_A:
				samples = msgsize(dp);
				bytes = samples;
				if (!pad) {
					if (cn->ic.ch->ic.conn == 1)
						mp = dp;
					else
						mp = ss7_dupmsg(q, dp);
				} else if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					uint8_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					while (samples--)
						*op++ = pcml2pcma(pcma2pcml(*ip++) >> pad);
				}
				break;
			case MX_ENCODING_G711_PCM_U:
				samples = msgsize(dp);
				bytes = samples;
				if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					uint8_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					if (pad)
						while (samples--)
							*op++ = pcml2pcma(pcmu2pcml(*ip++) >> pad);
					else
						while (samples--)
							*op++ = pcmu2pcma(*ip++);
				}
				break;
			}
			break;
		}
		case MX_ENCODING_G711_PCM_U:
		{
			uint8_t *op;
			switch (ic_type) {
			default:
				swerr();
			case MX_ENCODING_NONE:
				samples = msgsize(dp);
				bytes = samples;
				if (cn->ic.ch->ic.conn == 1)
					mp = dp;
				else
					mp = ss7_dupmsg(q, dp);
				break;
			case MX_ENCODING_G711_PCM_L:
				samples = msgsize(dp) >> 1;
				bytes = samples;
				if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					int16_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					while (samples--)
						*op++ = pcml2pcmu(*ip++ >> pad);
				}
				break;
			case MX_ENCODING_G711_PCM_A:
				samples = msgsize(dp);
				bytes = samples;
				if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					uint8_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					if (pad)
						while (samples--)
							*op++ = pcml2pcmu(pcma2pcml(*ip++) >> pad);
					else
						while (samples--)
							*op++ = pcma2pcmu(*ip++);
				}
				break;
			case MX_ENCODING_G711_PCM_U:
				samples = msgsize(dp);
				bytes = samples;
				if (!pad) {
					if (cn->ic.ch->ic.conn == 1)
						mp = dp;
					else
						mp = ss7_dupmsg(q, dp);
				} else if ((mp = ss7_allocb(q, bytes, BPRI_MED))) {
					uint8_t *ip;
					mp->b_wptr += bytes;
					ip = (typeof(ip)) dp->b_rptr;
					op = (typeof(op)) mp->b_rptr;
					while (samples--)
						*op++ = pcml2pcmu(pcmu2pcml(*ip++) >> pad);
				}
				break;
			}
			break;
		}
		}
		if (mp) {
			if (cn->buf) {
				linkb(cn->buf, mp);
				cn->samples += samples;
			} else {
				cn->buf = mp;
				cn->samples = samples;
			}
			if (mp == dp)
				return (QR_ABSORBED);
			return (QR_DONE);
		}
	}
	return (-ENOBUFS);
}

/*
 *  CONVERT INPUT
 *  -----------------------------------
 *  Convert input format to output or intermediate format if required.
 */
STATIC int
mg_convert_input(queue_t *q, mblk_t *dp, struct cn *cn)
{
	ulong ic_type = cn->ic.ch->config.encoding;
	ulong og_type = cn->og.ch->config.encoding;
	if (cn->og.ch->og.conn + cn->og.ch->ac.conn > 1) {	/* conferencing */
		/* 
		   need to perform conferencing, convert and pad to 16-bit signed native */
		og_type = MX_ENCODING_G711_PCM_L;
	}
	if (ic_type == MX_ENCODING_NONE)
		ic_type = cn->og.ch->lg.lg->se.se->config.encoding;
	return mg_convert_msg(q, dp, cn, ic_type, og_type);
}

/*
 *  PUSH FRAMES
 *  -----------------------------------
 *  A channel's incoming connections each have a blocksize of data ready to be output to a channel, and the
 *  associated multiplex is ready for data.  We perform a push on the channel and write a block.
 */
STATIC int
mg_push_frames(queue_t *q, struct mx *mx, struct ch *ch)
{
	int err;
	struct cn *cn;
	struct ac *ac;
	mblk_t *mp = NULL, *dp = NULL;
	int pad;
	uint cons;
	size_t samples = ch->config.samples;
	if (mx->ch.conn > 1) {
		/* 
		   we need an MX_DATA_REQ on the front of the data */
		struct MX_data_req *p;
		if (!(mp = ss7_allocb(q, sizeof(*p), BPRI_MED)))
			goto enobufs;
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_DATA_REQ;
		p->mx_slot = ch->mx_slot;
	}
	/* 
	   calculate padding for confrence */
	for (pad = -1, cons = ch->og.conn + ch->ac.conn; cons; pad++, cons >>= 1) ;
	if (pad < 0)
		goto efault;
	if (!pad && !ch->ac.conn) {
		/* 
		   just one connection active */
		size_t bytes = ch->config.block_size >> 3;
		for (cn = ch->og.list; cn; cn = cn->og.next)
			if (!(cn->flags & MGF_PULL_PENDING) && (cn->flags & MGF_OG_READY))
				break;
		if (!cn)
			goto efault;
		/* 
		   pull up samples on active connection */
		if (!pullupmsg(cn->buf, bytes))
			goto enobufs;
		if (!(dp = ss7_dupb(q, cn->buf)))
			goto enobufs;
		dp->b_wptr = dp->b_rptr + bytes;
		cn->buf->b_rptr += bytes;
		if ((cn->samples -= samples) < samples) {
			cn->og.ch->og.ready--;
			cn->flags &= ~MGF_OG_READY;
			cn->ic.ch->ic.ready++;
			cn->flags |= MGF_IC_READY;
		}
	} else {
		ulong og_type;
		/* 
		   need to perform conferencing of connections and actions */
		size_t bytes = samples << 1;
		ensure(dp, return (-EFAULT));
		/* 
		   we need a working/result buffer */
		if (!(dp = ss7_allocb(q, bytes, BPRI_MED)))
			goto enobufs;
		dp->b_datap->db_type = M_DATA;
		bzero(dp->b_rptr, bytes);
		dp->b_wptr += bytes;
		/* 
		   conference in connections and actions */
		for (cn = ch->og.list; cn; cn = cn->og.next) {
			if (!(cn->flags & MGF_PULL_PENDING) && (cn->flags & MGF_OG_READY)) {
				int16_t *ip, *op;
				size_t count = samples;
				if (!pullupmsg(cn->buf, bytes))
					goto enobufs;
				ip = (typeof(ip)) cn->buf->b_rptr;
				op = (typeof(op)) dp->b_rptr;
				while (count--)
					*op++ += (*ip++) >> pad;
				cn->buf->b_rptr += bytes;
				if ((cn->samples -= samples) < samples) {
					cn->og.ch->og.ready--;
					cn->flags &= ~MGF_OG_READY;
					cn->ic.ch->ic.ready++;
					cn->flags |= MGF_IC_READY;
				}
				cn->flags |= MGF_PULL_PENDING;
			}
		}
		/* 
		   actions are always PCM_L coding */
		for (ac = ch->ac.list; ac; ac = ac->ch.next) {
			if (!(ac->flags & MGF_PULL_PENDING) && (ac->flags & MGF_OG_READY)) {
				switch (ac->type) {
				case MG_ACTION_SEND_PATTERN:
				{
					int16_t *ip, *op;
					size_t count = samples;
					if (!pullupmsg(ac->buf, bytes))
						goto enobufs;
					ip = (typeof(ip)) ac->buf->b_rptr;
					op = (typeof(op)) dp->b_rptr;
					while (count--)
						*op++ += (*ip++) >> pad;
					ac->buf->b_rptr += bytes;
					if ((ac->samples -= samples) < samples) {
						ch->ac.conn--;
						ac->flags &= ~MGF_OG_READY;
					}
					break;
				}
				case MG_ACTION_REPEAT_PATTERN:
				{
					int16_t *ip, *op;
					size_t count = samples;
					if (!pullupmsg(ac->buf, -1))
						goto enobufs;
					ip = (typeof(ip)) (ac->buf->b_rptr + ac->pos);
					op = (typeof(op)) dp->b_rptr;
					while (count--)
						*op++ += (*ip++) >> pad;
					ac->pos += bytes;
					if (ac->bytes < ac->pos + bytes)
						ac->pos = 0;	/* replay */
					break;
				}
				}
				ac->flags |= MGF_PULL_PENDING;
			}
		}
		for (cn = ch->og.list; cn; cn = cn->og.next)
			cn->flags &= ~MGF_PULL_PENDING;
		for (ac = ch->ac.list; ac; ac = ac->ch.next)
			ac->flags &= ~MGF_PULL_PENDING;
		/* 
		   convert summation buffer to results buffer in place */
		if ((og_type = ch->config.encoding) == MX_ENCODING_NONE)
			og_type = ch->lg.lg->se.se->config.encoding;
		/* 
		   when we convert conference info, it is always in PCM_L format */
		switch (og_type) {
		default:
		case MX_ENCODING_NONE:
			swerr();
		case MX_ENCODING_G711_PCM_L:
			break;
		case MX_ENCODING_G711_PCM_A:
		{
			uint8_t *op = (typeof(op)) dp->b_rptr;
			int16_t *ip = (typeof(ip)) dp->b_rptr;
			size_t count = (dp->b_wptr - dp->b_rptr) >> 1;
			dp->b_wptr = dp->b_rptr + count;
			while (count--)
				*op++ = pcml2pcma(*ip++);
			break;
		}
		case MX_ENCODING_G711_PCM_U:
		{
			uint8_t *op = (typeof(op)) dp->b_rptr;
			int16_t *ip = (typeof(ip)) dp->b_rptr;
			size_t count = (dp->b_wptr - dp->b_rptr) >> 1;
			dp->b_wptr = dp->b_rptr + count;
			while (count--)
				*op++ = pcml2pcmu(*ip++);
			break;
		}
		}
	}
	if (mp)
		mp->b_cont = dp;
	else
		mp = dp;
	ss7_oput(mx->oq, mp);
	return (QR_DONE);
      error:
	if (mp)
		freeb(mp);
	if (dp)
		freeb(dp);
	return (err);
      enobufs:
	err = -ENOBUFS;
	goto error;
      efault:
	swerr();
	err = -EFAULT;
	goto error;
};

/*
 *  WAKEUP MULTIPLEX
 *  -----------------------------------
 *  A multiplex has all of its outgoing channels ready.  Push the data down to the multiplex or wait for backenable,
 *  or bufcall.  This push function assumes that the lower multiplex needs all connected channels written for proper
 *  flow control.
 */
STATIC INLINE void
mg_wakeup_multiplex(struct mx *mx)
{
	while (mx->ch.ready >= mx->ch.conn && canput(mx->oq)) {
		struct ch *ch;
		for (ch = mx->ch.list; ch; ch = ch->mx.next)
			if (!(ch->flags & MGF_PUSH_PENDING) && (ch->flags & MGF_OG_READY)) {
				if (mg_push_frames(mx->oq, mx, ch) < 0)
					return;	/* backenable or bufcall will re-enter */
				ch->flags |= MGF_PUSH_PENDING;
			}
		for (ch = mx->ch.list; ch; ch = ch->mx.next)
			ch->flags &= ~MGF_PUSH_PENDING;
	}
	/* 
	   wait for more data, or a backenable to pull frames */
	return;
}

STATIC void
mg_r_wakeup(queue_t *q)
{
	mg_wakeup_multiplex(MX_PRIV(q));
}
STATIC void
mx_w_wakeup(queue_t *q)
{
	mg_wakeup_multiplex(MX_PRIV(q));
}

/*
 *  WAKEUP CHANNEL
 *  -----------------------------------
 */
STATIC INLINE void
mg_wakeup_channel(struct ch *ch)
{
	if (ch->og.ready >= ch->og.conn && !(ch->flags & MGF_OG_READY)) {
		struct mx *mx = ch->mx.mx;
		ch->flags |= MGF_OG_READY;
		mx->ch.ready++;
		mg_wakeup_multiplex(mx);
	}
	return;
}

/*
 *  WAKEUP CONNECTION
 *  -----------------------------------
 */
STATIC INLINE void
mg_wakeup_connection(struct cn *cn)
{
	struct ch *ch = cn->og.ch;
	if (cn->samples >= ch->config.samples && !(cn->flags & MGF_OG_READY)) {
		cn->flags |= MGF_OG_READY;
		ch->og.ready++;
		mg_wakeup_channel(ch);
	}
	return;
}

/*
 *  PUSH CONNECT
 *  -----------------------------------
 *  A connection has data.  Convert the data and push it to the outgoing channel.  Note that mg_convert_input (and
 *  this function) will return QR_ABSORBED if the data has been moved (an possibly converted in-place) rather than
 *  coverted to a new buffer.  Otherwise it returns QR_DONE.
 */
#if 0
STATIC int
mg_push_connect(queue_t *q, mblk_t *mp, struct cn *cn)
{
	int err;
	if ((err = mg_convert_input(q, mp, cn)) >= 0)
		mg_wakeup_connection(cn);
	return (err);
}
#endif
STATIC int
ch_write(queue_t *q, mblk_t *dp, struct ch *ch)
{
	if (ch->ic.ready >= ch->ic.conn) {
		int err;
		struct cn *cn;
		for (cn = ch->ic.list; cn; cn = cn->next)
			if (!(cn->flags & MGF_PUSH_PENDING) && (cn->flags & MGF_IC_READY)) {
				if ((err = mg_convert_input(q, dp, cn)) < 0)
					return (err);
				mg_wakeup_connection(cn);
				if (err)
					return (err);
				cn->flags |= MGF_PUSH_PENDING;
			}
		for (cn = ch->ic.list; cn; cn = cn->next)
			cn->flags &= ~MGF_PUSH_PENDING;
		return (QR_DONE);
	}
	rare();
	return (-EBUSY);
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
 *  Primitives sent upstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_FLUSH
 *  -----------------------------------
 */
#if 0
STATIC int
m_flush(queue_t *q, struct mg *mg, int band, int flags, int what)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_FLUSH;
		*(mp->b_wptr)++ = flags | band ? FLUSHBAND : 0;
		*(mp->b_wptr)++ = band;
		printd(("%s: %p: <- M_FLUSH\n", DRV_NAME, mg));
		ss7_oput(mg->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, struct mg *mg, int error)
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
			printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			*(mp->b_wptr)++ = error < 0 ? -error : error;
			mg->i_state = MGS_UNUSABLE;
			printd(("%s: %p: <- M_ERROR\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_DATA
 *  -----------------------------------
 *  The preferred method for sending data to the media gateway user is to just
 *  send M_DATA blocks for efficiency and speed.
 */
STATIC inline int
mg_read(queue_t *q, struct mg *mg, mblk_t *dp)
{
	if (mg && mg->oq) {
		if (canput(mg->oq)) {
			printd(("%s: %p: <- M_DATA\n", DRV_NAME, mg));
			ss7_oput(mg->oq, dp);
			return (QR_DONE);
		}
		rare();
		return (-EBUSY);
	}
	rare();
	freemsg(dp);
	return (QR_ABSORBED);
}

/*
 *  MG_INFO_ACK
 *  -----------------------------------
 *  Acknowledges and information requets and provides information about the connection points
 *  in which the MG is bound.
 */
STATIC inline int
mg_info_ack(queue_t *q)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	mblk_t *mp;
	struct MG_info_ack *p;
	struct MG_channel_opt *o;
	size_t opt_len = sizeof(*o);
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mg_primitive = MG_INFO_ACK;
		p->mg_se_id = se ? se->id : 0;
		p->mg_opt_length = opt_len;
		p->mg_opt_offset = opt_len ? sizeof(*p) : 0;
		p->mg_prov_flags = 0;
		p->mg_style = MG_STYLE2;
		p->mg_version = MG_VERSION;
		o = (typeof(o)) mp->b_wptr;
		mp->b_wptr += sizeof(*o);
		o->mg_obj_type = MG_OBJ_TYPE_CH;
		o->ch_block_size = mg->config.block_size;
		o->ch_encoding = mg->config.encoding;
		o->ch_sample_size = mg->config.sample_size;
		o->ch_rate = mg->config.rate;
		o->ch_tx_channels = mg->config.tx_channels;
		o->ch_rx_channels = mg->config.rx_channels;
		o->ch_flags = mg->config.flags;
		/* 
		   state does not change */
		printd(("%s: %p: <- MG_INFO_ACK\n", DRV_NAME, mg));
		ss7_oput(mg->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MG_OPTMGMT_ACK
 *  -----------------------------------
 *  Acknowledges and reports success of failure of the requested operation on provider or
 *  channel options associated with the requesting stream.
 */
STATIC inline int
mg_optmgmt_ack(queue_t *q, struct mg *mg, uchar *opt_ptr, size_t opt_len, ulong flags)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_optmgmt_ack *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_OPTMGMT_ACK;
			p->mg_opt_length = opt_len;
			p->mg_opt_offset = opt_len ? sizeof(*p) : 0;
			p->mg_mgmt_flags = flags;
			if (opt_len) {
				bcopy(opt_ptr, mp->b_wptr, opt_len);
				mp->b_wptr += opt_len;
			}
			printd(("%s: %p: <- MG_OPTMGMT_ACK\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_OK_ACK
 *  -----------------------------------
 *  Positively acknowledges the last operation requiring acknowledgement.
 */
STATIC inline int
mg_ok_ack(queue_t *q, struct mg *mg, long prim)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_ok_ack *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_OK_ACK;
			p->mg_correct_prim = prim;
			printd(("%s: %p: <- MG_OK_ACK\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_ERROR_ACK
 *  -----------------------------------
 *  Negatively acknowledges the last operation requiring negative acknolwedgement and provides
 *  the error type and unix error.
 */
STATIC inline int
mg_error_ack(queue_t *q, struct mg *mg, long prim, long error)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_error_ack *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_ERROR_ACK;
			p->mg_error_primitive = prim;
			p->mg_error_type = error < 0 ? MGSYSERR : error;
			p->mg_unix_error = error < 0 ? -error : 0;
			printd(("%s: %p: <- MG_ERROR_ACK\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_ATTACH_ACK
 *  -----------------------------------
 *  Acknowledges the attachment of ther requesting stream to a newly created or existing
 *  communications session (context).
 */
STATIC inline int
mg_attach_ack(queue_t *q, struct mg *mg, struct ch *ch)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_attach_ack *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_ATTACH_ACK;
			p->mg_mx_id = ch->mx.mx ? ch->mx.mx->id : 0;
			p->mg_mx_slot = ch->mx_slot;
			p->mg_ch_id = ch->id;
			printd(("%s: %p: <- MG_ATTACH_ACK\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_JOIN_CON
 *  -----------------------------------
 *  Confirms that the requested channel or requesting stream has joined the communications
 *  session.
 */
STATIC inline int
mg_join_con(queue_t *q, struct mg *mg, struct se *se, struct lg *lg)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_join_con *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_JOIN_CON;
			p->mg_se_id = se->id;
			p->mg_tp_id = lg->id;
			printd(("%s: %p: <- MG_JOIN_CON\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_ACTION_CON
 *  -----------------------------------
 *  Confirms that the requested action has begun.  MG_ACTION_REQ which have the MG_MORE_DATA flags set will not be
 *  confirmed until the last MG_ACTION_REQ has been issued by the MG user.  The end of restricted duration actions
 *  will be indicated with the MG_ACTION_IND primitive.
 */
STATIC inline int
mg_action_con(queue_t *q, struct mg *mg, struct se *se, struct lg *lg, struct ac *ac)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_action_con *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_ACTION_CON;
			p->mg_action = ac->type;
			p->mg_se_id = se->id;
			p->mg_tp_id = lg->id;
			p->mg_action_id = ac->id;
			printd(("%s: %p: <- MG_ACTION_CON\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_ACTION_IND
 *  -----------------------------------
 *  Indicates that the action identified by the indicated action identifier has completed.
 */
STATIC inline int
mg_action_ind(queue_t *q, struct mg *mg, struct se *se, struct lg *lg, struct ac *ac)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_action_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_ACTION_IND;
			p->mg_action = ac->type;
			p->mg_se_id = se->id;
			p->mg_tp_id = lg->id;
			p->mg_action_id = ac->id;
			printd(("%s: %p: <- MG_ACTION_IND\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_CONN_CON
 *  -----------------------------------
 *  Confirms that the indicated channel has been connected as requested.
 */
STATIC inline int
mg_conn_con(queue_t *q, struct mg *mg, struct se *se, struct lg *lg)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_conn_con *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_CONN_CON;
			p->mg_se_id = se ? se->id : 0;
			p->mg_tp_id = lg ? lg->id : 0;
			if (lg)
				lg->state = MGS_CONNECTED;
			if (se)
				se->state = MGS_CONNECTED;
			printd(("%s: %p: <- MG_CONN_CON\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_DATA_IND
 *  -----------------------------------
 *  Inidicates that data was received on a connected channel.
 */
STATIC inline int
mg_data_ind(queue_t *q, struct mg *mg, ulong flags, mblk_t *dp)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_data_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_DATA_IND;
			p->mg_flags = flags;
			mp->b_cont = dp;
			printd(("%s: %p: <- MG_DATA_IND\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_DISCON_IND
 *  -----------------------------------
 *  Indicates that a channel has been disconnected as a result of a lower level failure and the
 *  cause of the disconnection.
 */
STATIC inline int
mg_discon_ind(queue_t *q, struct mg *mg, struct se *se, struct lg *lg, ulong flags, ulong cause)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_discon_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_DISCON_IND;
			p->mg_se_id = se ? se->id : 0;
			p->mg_tp_id = lg ? lg->id : 0;
			p->mg_conn_flags = flags;
			p->mg_cause = cause;
			if (lg)
				lg->state = MGS_JOINED;
			if (se)
				for (lg = se->lg.list; lg; lg = lg->se.next) {
					switch (lg->state) {
					case MGS_JOINED:
						continue;
					case MGS_WCON_CREQ:
						se->state = MGS_WCON_CREQ;
						break;
					case MGS_WCON_DREQ:
						se->state = MGS_WCON_DREQ;
						break;
					case MGS_CONNECTED:
						se->state = MGS_CONNECTED;
						break;
					}
					break;
				}
			printd(("%s: %p: <- MG_DISCON_IND\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_DISCON_CON
 *  -----------------------------------
 *  Confirms that the indicated channel has been disconnected as requested.
 */
STATIC inline int
mg_discon_con(queue_t *q, struct mg *mg, struct se *se, struct lg *lg, ulong flags)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_discon_con *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_DISCON_CON;
			p->mg_se_id = se ? se->id : 0;
			p->mg_tp_id = lg ? lg->id : 0;
			if (lg) {
				lg->flags &= ~flags;
				if (lg->flags & (MGF_IC_DIR | MGF_OG_DIR))
					lg->state = MGS_CONNECTED;
				else
					lg->state = MGS_JOINED;
			}
			if (se) {
				for (lg = se->lg.list; lg; lg = lg->se.next) {
					switch (lg->state) {
					case MGS_JOINED:
						continue;
					case MGS_WCON_CREQ:
						se->state = MGS_WCON_CREQ;
						break;
					case MGS_WCON_DREQ:
						se->state = MGS_WCON_DREQ;
						break;
					case MGS_CONNECTED:
						se->state = MGS_CONNECTED;
						break;
					}
					break;
				}
			}
			printd(("%s: %p: <- MG_DISCON_CON\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_LEAVE_IND
 *  -----------------------------------
 *  Indicates that a channel has left a communication session as a result of a lower level
 *  failure and the cause of the deletion.
 */
STATIC inline int
mg_leave_ind(queue_t *q, struct mg *mg, struct se *se, struct lg *lg, ulong cause)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_leave_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_LEAVE_IND;
			p->mg_se_id = se ? se->id : 0;
			p->mg_tp_id = lg ? lg->id : 0;
			p->mg_cause = cause;
			fixme(("Adjust lg and se states\n"));
			if (lg) {
			}
			if (se) {
			}
			printd(("%s: %p: <- MG_LEAVE_IND\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_LEAVE_CON
 *  -----------------------------------
 *  Confirms that the indicated channel or requesting stream has left the communications
 *  session as requested.
 */
STATIC inline int
mg_leave_con(queue_t *q, struct mg *mg, struct se *se, struct lg *lg)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_leave_con *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_LEAVE_CON;
			p->mg_se_id = se ? se->id : 0;
			p->mg_tp_id = lg ? lg->id : 0;
			fixme(("Adjust lg and se states\n"));
			if (lg) {
			}
			if (se) {
			}
			printd(("%s: %p: <- MG_LEAVE_CON\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  MG_NOTIFY_IND
 *  -----------------------------------
 *  Indicates to the media gateway user that an event has occured on in the session.
 */
STATIC inline int
mg_notify_ind(queue_t *q, struct mg *mg, ulong event, uchar *dia_ptr, size_t dia_len)
{
	if (mg && mg->oq) {
		mblk_t *mp;
		struct MG_notify_ind *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dia_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mg_primitive = MG_NOTIFY_IND;
			p->mg_event = event;
			p->mg_diag_length = dia_len;
			p->mg_diag_offset = dia_len ? sizeof(*p) : 0;
			if (dia_len) {
				bcopy(dia_ptr, mp->b_wptr, dia_len);
				mp->b_wptr += dia_len;
			}
			printd(("%s: %p: <- MG_NOTIFY_IND\n", DRV_NAME, mg));
			ss7_oput(mg->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 *  The preferred method for sending data to the multiplex provider is to just send M_DATA blocks for efficiency and
 *  speed.
 */
STATIC inline int
mx_write(queue_t *q, struct mx *mx, mblk_t *dp)
{
	if (mx && mx->oq) {
		if (canput(mx->oq)) {
			printd(("%s: %p: M_DATA ->\n", DRV_NAME, mx));
			ss7_oput(mx->oq, dp);
			return (QR_ABSORBED);
		}
		rare();
		return (-EBUSY);
	}
	rare();
	freemsg(dp);
	return (QR_ABSORBED);
}

/*
 *  MX_INFO_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider return information concerning the multiplex provider and any attached
 *  slots.
 */
STATIC inline int
mx_info_req(queue_t *q, struct mx *mx)
{
	if (mx && mx->oq) {
		mblk_t *mp;
		struct MX_info_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mx_primitive = MX_INFO_REQ;
			printd(("%s: %p: MX_INFO_REQ ->\n", DRV_NAME, mx));
			ss7_oput(mx->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (QR_DONE);
}

/*
 *  MX_ATTACH_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider attach the requesting stream to the specified multiplex (address) with the
 *  specified options.
 */
STATIC inline int
mx_attach_req(queue_t *q, struct mx *mx, uchar *add_ptr, size_t add_len, ulong flags)
{
	if (mx && mx->oq) {
		mblk_t *mp;
		struct MX_attach_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
			mp->b_datap->db_type = MX_ATTACH_REQ;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mx_primitive = MX_ATTACH_REQ;
			p->mx_addr_length = add_len;
			p->mx_addr_offset = add_len ? sizeof(*p) : 0;
			p->mx_flags = flags;
			if (add_len) {
				bcopy(add_ptr, mp->b_wptr, add_len);
				mp->b_wptr += add_len;
			}
			printd(("%s: %p: MX_ATTACH_REQ ->\n", DRV_NAME, mx));
			mx->state = MGS_WACK_AREQ;
			ss7_oput(mx->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (QR_DONE);
}

/*
 *  MX_ENABLE_REQ
 *  -----------------------------------
 *  Requests that the attached multiplex be enabled and confirmed enabled with a MX_ENABLE_CON.
 */
STATIC inline int
mx_enable_req(queue_t *q, struct mx *mx)
{
	if (mx && mx->oq) {
		mblk_t *mp;
		struct MX_enable_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = MX_ENABLE_REQ;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mx_primitive = MX_ENABLE_REQ;
			mx->state = MGS_WCON_JREQ;
			printd(("%s: %p: MX_ENABLE_REQ ->\n", DRV_NAME, mx));
			ss7_oput(mx->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EFAULT);
}

/*
 *  MX_CONNECT_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider connect the multiplex's transmit or receive direction on the specified slot
 *  to the requesing stream.  When successful, subsequent data sent as MX_DATA_REQ will be transmitted on the
 *  multiplex and subsequent data received on the multiplex will be delivered as MX_DATA_IND.  Any M_DATA blocks
 *  provided with the connect request will be transmitted immediately following the successful connection.
 */
STATIC inline int
mx_connect_req(queue_t *q, struct mx *mx, struct ch *ch)
{
	if (mx && mx->oq) {
		mblk_t *mp;
		struct MX_connect_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mx_primitive = MX_CONNECT_REQ;
			p->mx_conn_flags = ch->flags & (MXF_RX_DIR | MXF_TX_DIR);
			p->mx_slot = ch->mx_slot;
			ch->state = MGS_WCON_CREQ;
			printd(("%s; %p: MX_CONNECT_REQ ->\n", DRV_NAME, mx));
			ss7_oput(mx->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EFAULT);
}

/*
 *  MX_DATA_REQ
 *  -----------------------------------
 *  The non-preferred method for sending data to the multiplex provider is to send MX_DATA_REQ blocks.  We prefer
 *  just send M_DATA blocks for efficiency and speed.  See mx_write() above.  If slot is specified as -1UL, then the
 *  each byte of the data corresponds to one slot in the multiplex.  If slot is specified 0 <= slot < n, where n is
 *  the number of slots in the multiplex, then the data consists of data only for the specified slot.
 */
STATIC inline int
mx_data_req(queue_t *q, struct mx *mx, ulong slot, mblk_t *dp)
{
	if (mx && mx->oq) {
		if (canput(mx->oq)) {
			mblk_t *mp;
			struct MX_data_req *p;
			if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = (typeof(p)) mp->b_wptr;
				mp->b_wptr += sizeof(*p);
				p->mx_primitive = MX_DATA_REQ;
				p->mx_slot = slot;
				mp->b_cont = dp;
				printd(("%s; %p: MX_DATA_REQ ->\n", DRV_NAME, mx));
				ss7_oput(mx->oq, mp);
				return (QR_ABSORBED);
			}
			rare();
			return (-ENOBUFS);
		}
		rare();
		return (-EBUSY);
	}
	rare();
	freemsg(dp);
	return (QR_DONE);
}

/*
 *  MX_DISCONNECT_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider disconnect the multiplex's transmit or receive direction for the specified
 *  slot from the requesting stream.  When successful, subsequent received data and MX_DATA_REQ for the disconnected
 *  slot will be discarded.
 */
STATIC inline int
mx_disconnect_req(queue_t *q, struct mx *mx, struct ch *ch, ulong flags)
{
	if (mx && mx->oq) {
		mblk_t *mp;
		struct MX_disconnect_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mx_primitive = MX_DISCONNECT_REQ;
			p->mx_conn_flags = flags;
			p->mx_slot = ch->mx_slot;
			ch->state = MGS_WCON_DREQ;
			ch->flags &= ~flags;
			printd(("%s: %p: MX_DISCONNECT_REQ ->\n", DRV_NAME, mx));
			ss7_oput(mx->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (QR_DONE);
}

/*
 *  MX_DISABLE_REQ
 *  -----------------------------------
 *  Requests that the attached multiplex be disabled and the disabling confirmed with a MX_DISABLE_CON primitive.
 */
STATIC inline int
mx_disable_req(queue_t *q, struct mx *mx)
{
	if (mx && mx->oq) {
		mblk_t *mp;
		struct MX_disable_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mx_primitive = MX_DISABLE_REQ;
			mx->state = MGS_WCON_LREQ;
			printd(("%s: %p: MX_DISABLE_REQ ->\n", DRV_NAME, mx));
			ss7_oput(mx->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	swerr();
	return (QR_DONE);
}

/*
 *  MX_DETACH_REQ
 *  -----------------------------------
 *  Request that the multiplex provider detach the requesting stream from the currently attached multiplex.
 */
STATIC inline int
mx_detach_req(queue_t *q, struct mx *mx)
{
	if (mx && mx->oq) {
		mblk_t *mp;
		struct MX_detach_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = MX_DETACH_REQ;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->mx_primitive = MX_DETACH_REQ;
			printd(("%s: %p: MX_DETACH_REQ ->\n", DRV_NAME, mx));
			mx->state = MGS_WACK_UREQ;
			ss7_oput(mx->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (QR_DONE);
}

/*
 *  =========================================================================
 *
 *  Timers
 *
 *  =========================================================================
 */

/*
   no timers are currently defined 
 */

/*
 *  =========================================================================
 *
 *  STATE MACHINES
 *
 *  =========================================================================
 */

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
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  M_DATA
 *  -----------------------------------
 *  Preferred way of receiving data from multiplex provider.  The multiplex provider should always
 *  give us data as M_DATA blocks.
 */
STATIC int
mx_read(queue_t *q, mblk_t *dp)
{
	struct mx *mx = MX_PRIV(q);
	struct ch *ch;
	if (mx->state != MGS_CONNECTED || !(mx->flags & MGF_IC_DIR) || !(ch = mx->ch.list))
		goto discard;
	if (!msgsize(dp))
		goto efault;
	return ch_write(q, dp, ch);
      efault:
	swerr();
	return (-EFAULT);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  MX_INFO_ACK
 *  -----------------------------------
 *  Indicates information about the multiplex provider and any attached multiplex.
 */
STATIC int
mx_info_ack(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	int err;
	struct MX_info_ack *p = (typeof(p)) mp->b_rptr;
	(void) mx;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	fixme(("Unimplemented primitive\n"));
	goto efault;
      efault:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MX_OK_ACK
 *  -----------------------------------
 *  Indicates that the last operation requiring acknowledgement was successful.
 */
STATIC int
mx_ok_ack(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	int err;
	struct MX_ok_ack *p = (typeof(p)) mp->b_rptr;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (p->mx_correct_prim) {
	case MX_ATTACH_REQ:
		mx->state = MGS_ATTACHED;
		return (QR_DONE);
	case MX_DETACH_REQ:
		mx->state = MGS_DETACHED;
		return (QR_DONE);
	}
	goto outstate;
      outstate:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MX_ERROR_ACK
 *  -----------------------------------
 *  Indicates that the last operation requiring negative acknowledgement was unsuccessful.
 */
STATIC int
mx_error_ack(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	long error;
	ulong prim;
	struct MX_error_ack *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	prim = p->mx_error_primitive;
	error =
	    (p->mx_error_type == MXSYSERR) ? -p->mx_unix_error : p->mx_error_type & 0x00008000UL;
	return mg_error_ack(q, mg, prim, error);
      emsgsize:
	fixme(("Do more here, error out the stream\n"));
	return (-EMSGSIZE);
}

/*
 *  MX_ENABLE_CON
 *  -----------------------------------
 *  Confirms that the attached multiplex was enabled.  When we receive confirmation that the multiplex was enabled,
 *  we check any legs associated with the multiplex channels to see if the legs are waiting for enable (join)
 *  confirmation.  If they are, and this is the last channel to be enabled in the leg, we confirm the leg and
 *  session joined to any MG user.
 */
STATIC int
mx_enable_con(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct mg *mg;
	struct se *se;
	struct lg *lg;
	struct ch *ch;
	int err;
	struct MX_enable_con *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state != MXS_WCON_EREQ)
		goto outstate;
	for (ch = mx->ch.list; ch; ch = ch->mx.next) {
		if (ch->state == MGS_WCON_JREQ) {
			if ((lg = ch->lg.lg)) {
				struct ch *c2 = ch;
				if (lg->state == MGS_WCON_JREQ) {
					for (c2 = lg->ch.list; c2 && c2 != ch; c2 = c2->lg.next)
						if (c2->state == MGS_WCON_JREQ)
							break;
				}
				if (c2) {
					ch->state = MGS_JOINED;
					continue;
				}
				if ((se = lg->se.se) && se->state == MGS_WCON_JREQ) {
					if ((mg = se->mg.mg)) {
						if ((err = mg_join_con(q, mg, se, lg)) < 0)
							return (err);
						se->state = MGS_JOINED;
					}
				}
				lg->state = MGS_JOINED;
				ch->state = MGS_JOINED;
			}
		}
	}
	mx->state = MXS_ENABLED;
	return (QR_DONE);
      outstate:
	swerr();
	return (-EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MX_CONNECT_CON
 *  -----------------------------------
 *  Confirms a connection and also includes any data which has arrived on the attached and connected multiplex as a
 *  result of the connection.  When we receive a connection confirmation that a specific slot in the multiplex was
 *  connected, we check any legs associated with the multiple channel to see if the leg is awaiting connection.  If
 *  the leg is awaiting connection, we check to see if any other channels in the leg are awaiting connection, or if
 *  any of the channels at the other end of a connection to these channels are awaiting connection at their end.  If
 *  we are the last channel to connect, we confirm the leg and the session connected to any MG user.
 */
STATIC int
mx_connect_con(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct se *se;
	struct lg *lg;
	struct ch *ch, *c2, *c3;
	struct cn *cn;
	int err;
	struct MX_connect_con *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state != MXS_ENABLED && mx->state != MXS_WCON_DREQ)
		goto outstate;
	for (ch = mx->ch.list; ch && ch->mx_slot != p->mx_slot; ch = ch->mx.next) ;
	if (!ch)
		goto enxio;
	if (ch->state != MGS_WCON_CREQ)
		goto ignore;
	if (!(lg = ch->lg.lg) || lg->state != MGS_WCON_CREQ)
		goto efault;
	for (c2 = lg->ch.list; c2; c2 = c2->lg.next) {
		if (c2 == ch)
			continue;
		/* 
		   if there are any other channels in the multirate group waiting for confirmed
		   connection, just wait for them. */
		if (c2->state == MGS_WCON_CREQ)
			goto done;
		for (cn = c2->ic.list; cn; cn = cn->ic.next) {
			if ((cn->state == MGS_WCON_CREQ) && (c3 = cn->og.ch)) {
				/* 
				   if the outgoing end of the connection is waiting for confirmed
				   connection, just wait for them */
				if (c3->state == MGS_WCON_CREQ)
					goto done;
				if (c3->state == MGS_CONNECTED)
					cn->state = MGS_CONNECTED;
			}
		}
		for (cn = c2->og.list; cn; cn = cn->og.next) {
			if ((cn->state == MGS_WCON_CREQ) && (c3 = cn->ic.ch)) {
				/* 
				   if the incoming end of the connection is waiting for confirmed
				   connection, just wait for them */
				if (c3->state == MGS_WCON_CREQ)
					goto done;
				if (c3->state == MGS_CONNECTED)
					cn->state = MGS_CONNECTED;
			}
		}
	}
	if ((se = lg->se.se) && se->state == MGS_WCON_CREQ)
		if ((err = mg_conn_con(q, se->mg.mg, se, lg)) < 0)
			return (err);
	lg->state = MGS_CONNECTED;
      done:
	ch->state = MGS_CONNECTED;
	return (QR_DONE);
      efault:
	pswerr(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	ch->state = MGS_CONNECTED;
	return (-EFAULT);
      ignore:
	ptrace(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	return (QR_DONE);	/* ignore */
      enxio:
	pswerr(("%s: No such channel %lu for multiplex %lu\n", DRV_NAME, p->mx_slot, mx->id));
	return (-ENXIO);
      outstate:
	pswerr(("%s: Receiving messages for disabled multiplex %lu\n", DRV_NAME, mx->id));
	return (-EPROTO);
      emsgsize:
	pswerr(("%s: Bad message message size on multiplex %lu\n", DRV_NAME, mx->id));
	return (-EMSGSIZE);
}

/*
 *  MX_DATA_IND
 *  -----------------------------------
 *  Indicates that data has arrived for the attached multiplex.  This is the non-prefered way of receiving data from
 *  a simple multiplex (one channel); the simple multiplex provider should only send us raw M_DATA blocks where
 *  possible.
 */
STATIC int
mx_data_ind(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct lg *lg;
	struct ch *ch;
	struct MX_data_ind *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) || !mp->b_cont || !msgsize(mp->b_cont))
		goto emsgsize;
	if (mx->state != MXS_ENABLED && mx->state != MXS_WCON_DREQ)
		goto outstate;
	for (ch = mx->ch.list; ch && ch->mx_slot != p->mx_slot; ch = ch->mx.next) ;
	if (!ch)
		goto enxio;
	if (ch->state != MGS_CONNECTED)
		goto ignore;
	if (!(lg = ch->lg.lg) || lg->state != MGS_CONNECTED)
		goto efault;
	if ((err = ch_write(q, mp->b_cont, ch)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      efault:
	pswerr(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	ch->state = MGS_JOINED;
	return (-EFAULT);
      ignore:
	ptrace(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	return (QR_DONE);
      enxio:
	pswerr(("%s: No such channel %lu for multiplex %lu\n", DRV_NAME, p->mx_slot, mx->id));
	return (-ENXIO);
      outstate:
	pswerr(("%s: Receiving messages for disabled multiplex %lu\n", DRV_NAME, mx->id));
	return (-EPROTO);
      emsgsize:
	pswerr(("%s: Bad message message size on multiplex %lu\n", DRV_NAME, mx->id));
	return (-EMSGSIZE);
}

/*
 *  MX_DISCONNECT_IND
 *  -----------------------------------
 *  Indicates that an autonomous disconnection of the multiplex occured, the cause of the
 *  disconnection (normaly multiplex provider lower layer fault), and also includes any data
 *  which has arrived on the attached and disconnected multiplex before it was disconnected.
 */
STATIC int
mx_disconnect_ind(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct se *se;
	struct lg *lg;
	struct ch *ch, *c2, *c3;
	struct cn *cn;
	int err;
	struct MX_disconnect_ind *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state != MXS_ENABLED && mx->state != MXS_WCON_DREQ)
		goto outstate;
	for (ch = mx->ch.list; ch && ch->mx_slot != p->mx_slot; ch = ch->mx.next) ;
	if (!ch)
		goto enxio;
	if (ch->state == MGS_JOINED)
		goto ignore;
	if (!(lg = ch->lg.lg) || lg->state == MGS_JOINED)
		goto efault;
	for (c2 = lg->ch.list; c2; c2 = c2->lg.next) {
		/* 
		   When one channel in a multirate group is lost, they must all be disconnected */
		if (c2->state != MXS_CONNECTED && c2->state != MXS_WCON_CREQ)
			continue;
		if (p->mx_conn_flags & MXF_RX_DIR) {
			while ((cn = c2->ic.list)) {
				/* 
				   check if we are only connection to other end */
				if ((c3 = cn->og.ch) && c3->state == MXS_CONNECTED
				    && c3->flags & MXF_TX_DIR && c3->og.numb == 1
				    && (err = mx_disconnect_req(q, c3->mx.mx, c3, MXF_TX_DIR)))
					return (err);
				cn_free_priv(c2->ic.list);
			}
		}
		if (p->mx_conn_flags & MXF_TX_DIR) {
			while ((cn = c2->og.list)) {
				/* 
				   check if we are only connection to other end */
				if ((c3 = cn->ic.ch) && c3->state == MGS_CONNECTED
				    && c3->flags & MXF_RX_DIR && c3->ic.numb == 1
				    && (err = mx_disconnect_req(q, c3->mx.mx, c3, MXF_RX_DIR)))
					return (err);
				cn_free_priv(c2->og.list);
			}
		}
		/* 
		   disconnect other channels in this leg */
		if ((c2 != ch) && (err = mx_disconnect_req(q, c2->mx.mx, c2, p->mx_conn_flags)))
			return (err);
	}
	if ((se = lg->se.se) && ((se->state == MGS_CONNECTED || se->state == MGS_WCON_CREQ)))
		if ((err = mg_discon_ind(q, se->mg.mg, se, lg, p->mx_conn_flags, p->mx_cause)) < 0)
			return (err);
	lg->state = MGS_JOINED;
	ch->state = MGS_JOINED;
	return (QR_DONE);
      efault:
	pswerr(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	ch->state = MGS_JOINED;
	return (-EFAULT);
      ignore:
	ptrace(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	return (QR_DONE);	/* ignore */
      enxio:
	pswerr(("%s: No such channel %lu for multiplex %lu\n", DRV_NAME, p->mx_slot, mx->id));
	return (-ENXIO);
      outstate:
	pswerr(("%s: Receiving messages for disabled multiplex %lu\n", DRV_NAME, mx->id));
	return (-EPROTO);
      emsgsize:
	pswerr(("%s: Bad message size on multiplex %lu\n", DRV_NAME, mx->id));
	return (-EMSGSIZE);
}

/*
 *  MX_DISCONNECT_CON
 *  -----------------------------------
 *  Confirms the disconnection and also includes any data which arrived before the
 *  disconnection was completed.  We must also effect and confirm any pending
 *  topology requests.
 */
STATIC int
mx_disconnect_con(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct mg *mg;
	struct se *se;
	struct lg *lg;
	struct ch *ch, *c2;
	int err;
	struct MX_disconnect_con *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state != MXS_ENABLED && mx->state != MXS_WCON_DREQ)
		goto outstate;
	for (ch = mx->ch.list; ch && ch->mx_slot != p->mx_slot; ch = ch->mx.next) ;
	if (!ch)
		goto enxio;
	if (ch->state != MGS_WCON_DREQ)
		goto done_ch;
	if (!(lg = ch->lg.lg))
		goto efault_ch;
	if (lg->state != MGS_WCON_DREQ)
		goto done_ch;
	for (c2 = lg->ch.list; c2; c2 = c2->lg.next) {
		if (c2 == ch)
			continue;
		/* 
		   if there are any other channels in the multirate group waiting for confirmed
		   disconnection, just wait for them */
		if (c2->state == MGS_WCON_DREQ)
			goto done_ch;
	}
	if (!(se = lg->se.se))
		goto efault_lg;
	if (se->state != MGS_WCON_DREQ)
		goto done_lg;
	if (!(mg = se->mg.mg))
		goto efault_lg;
	if ((err = mg_discon_con(q, mg, se, lg, p->mx_conn_flags)) < 0)
		return (err);
      done_lg:
	lg->state = MGS_JOINED;
      done_ch:
	ch->state = MGS_JOINED;
	return (QR_DONE);
      efault_lg:
	pswerr(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	goto done_lg;
      efault_ch:
	pswerr(("%s: Ignoring unexpected message on channel %lu\n", DRV_NAME, ch->id));
	goto done_ch;
      enxio:
	pswerr(("%s: No such channel %lu for multiplex %lu\n", DRV_NAME, p->mx_slot, mx->id));
	return (-ENXIO);
      outstate:
	pswerr(("%s: Receiving messages for disabled multiplex %lu\n", DRV_NAME, mx->id));
	return (-EPROTO);
      emsgsize:
	pswerr(("%s: Bad message size on multiplex %lu\n", DRV_NAME, mx->id));
	return (-EMSGSIZE);
}

/*
 *  MX_DISABLE_IND
 *  -----------------------------------
 *  Indicates that the attached multiplex was disabled (and disconnected if connected) due to a
 *  lower layer fault.  The cause is also indicated.  For common circuits, this indicates the
 *  abortive release of the circuit.
 */
STATIC int
mx_disable_ind(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct mg *mg;
	struct se *se;
	struct lg *lg;
	struct ch *ch;
	int err;
	struct MX_disable_ind *p = (typeof(p)) mp->b_rptr;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	for (ch = mx->ch.list; ch; ch = ch->mx.next) {
		if (ch->state != MGS_DETACHED) {
			if ((lg = ch->lg.lg)) {
				struct ch *c2 = ch;
				if (lg->state != MGS_DETACHED) {
					for (c2 = lg->ch.list; c2 && c2 != ch; c2 = c2->lg.next)
						if (c2->state != MGS_DETACHED)
							break;
				}
				if (c2) {
					ch->state = MGS_DETACHED;
					continue;
				}
				if ((se = lg->se.se) && se->state != MGS_DETACHED) {
					if ((mg = se->mg.mg)) {
						if ((err =
						     mg_leave_ind(q, mg, se, lg, p->mx_cause)) < 0)
							return (err);
						se->state = MGS_DETACHED;
					}
				}
				lg->state = MGS_DETACHED;
				ch->state = MGS_DETACHED;
			}
		}
	}
	mx->state = MXS_DETACHED;
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  MX_DISABLE_CON
 *  -----------------------------------
 *  Confirms that the attached multiplex was disabled as requested.  For common circuits this is
 *  the same as confirming the release of the circuit.
 */
STATIC int
mx_disable_con(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct mg *mg;
	struct se *se;
	struct lg *lg;
	struct ch *ch;
	int err;
	struct MX_disable_con *p = (typeof(p)) mp->b_rptr;
	(void) err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state != MGS_WCON_LREQ)
		goto ignore;
	for (ch = mx->ch.list; ch; ch = ch->mx.next) {
		if (ch->state == MGS_WCON_LREQ) {
			if ((lg = ch->lg.lg)) {
				struct ch *c2 = ch;
				if (lg->state == MGS_WCON_LREQ) {
					for (c2 = lg->ch.list; c2 && c2 != ch; c2 = c2->lg.next)
						if (c2->state == MGS_WCON_LREQ)
							break;
				}
				if (c2) {
					ch->state = MGS_DETACHED;
					continue;
				}
				if ((se = lg->se.se) && se->state == MGS_WCON_LREQ) {
					if ((mg = se->mg.mg)) {
						if ((err = mg_leave_con(q, mg, se, lg)) < 0)
							return (err);
						se->state = MGS_DETACHED;
					}
				}
				lg->state = MGS_DETACHED;
				ch->state = MGS_DETACHED;
			}
		}
	}
	mx->state = MXS_DETACHED;
	return (QR_DONE);
      ignore:
	rare();
	return (QR_DONE);
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
 *  M_DATA
 *  -----------------------------------
 *  This is the preferred way for an MG stream to provide data to the connection point, but it is only valid for the
 *  first channel.
 */
STATIC int
mg_write(queue_t *q, mblk_t *dp)
{
	struct mg *mg = MG_PRIV(q);
	struct ch *ch;
	if (mg->state != MGS_CONNECTED || !(mg->flags & MGF_IC_DIR) || !(ch = mg->ch.list))
		goto discard;
	if (!msgsize(dp))
		goto eproto;
	return ch_write(q, dp, ch);
      eproto:
	return m_error(q, mg, -EPROTO);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  MG_INFO_REQ
 *  -----------------------------------
 *  Requests that the provider return in an MG_INFO_ACK information about the provider and any
 *  attached communications sessions (contexts).
 */
STATIC int
mg_info_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct MG_info_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mg->state == MGS_UNINIT)
		goto retry;
	if (mg->state == MGS_UNUSABLE)
		goto outstate;
	return mg_info_ack(q);
      retry:
	return (QR_DISABLE);
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
}

/*
 *  MG_OPTMGMT_REQ
 *  -----------------------------------
 *  Requests that the provider set, negotiate, or get option information and return
 *  acknowledgement using an MG_OPTMGMT_ACK.
 */
STATIC int
mg_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct MG_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	union MG_options *o;
	uchar *opt_ptr;
	size_t opt_len;
	ulong flags;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr > mp->b_rptr + p->mg_opt_offset + p->mg_opt_length)
		goto emsgsize;
	if (!((1 << mg->state) & (MGSF_ATTACHED | MGSF_JOINED | MGSF_CONNECTED)))
		goto outstate;
	if (p->mg_opt_length) {
		if (p->mg_opt_length < sizeof(o->obj.mg_obj_type))
			goto badopt;
		o = (typeof(o)) (mp->b_rptr + p->mg_opt_offset);
		if (o->obj.mg_obj_type != MG_OBJ_TYPE_CH)
			goto badopttype;
		switch (o->obj.mg_obj_type) {
		case MG_OBJ_TYPE_CH:
			if (p->mg_opt_length < sizeof(o->ch))
				goto badopt;
			switch (p->mg_mgmt_flags) {
			case 0:
			case MG_NEGOTIATE:
				/* 
				   negotiate current */
			case MG_SET_OPT:
				/* 
				   set current */
				mg->config.block_size = o->ch.ch_block_size;
				mg->config.encoding = o->ch.ch_encoding;
				mg->config.sample_size = o->ch.ch_sample_size;
				mg->config.rate = o->ch.ch_rate;
				mg->config.tx_channels = o->ch.ch_tx_channels;
				mg->config.rx_channels = o->ch.ch_rx_channels;
				mg->config.flags = o->ch.ch_flags;
				opt_ptr = (uchar *) o;
				opt_len = sizeof(o->ch);
				flags = p->mg_mgmt_flags;
				return mg_optmgmt_ack(q, mg, opt_ptr, opt_len, flags);
			case MG_DEFAULT:
				mg_default.block_size = o->ch.ch_block_size;
				mg_default.encoding = o->ch.ch_encoding;
				mg_default.sample_size = o->ch.ch_sample_size;
				mg_default.rate = o->ch.ch_rate;
				mg_default.tx_channels = o->ch.ch_tx_channels;
				mg_default.rx_channels = o->ch.ch_rx_channels;
				mg_default.flags = o->ch.ch_flags;
				opt_ptr = (uchar *) o;
				opt_len = sizeof(o->ch);
				flags = p->mg_mgmt_flags;
				return mg_optmgmt_ack(q, mg, opt_ptr, opt_len, flags);
			}
			break;
		default:
			goto badopttype;
		}
	} else {
		union MG_options opts;
		o = &opts;
		/* 
		   no options */
		switch (p->mg_mgmt_flags) {
		case MX_GET_OPT:
			o->ch.mg_obj_type = MG_OBJ_TYPE_CH;
			o->ch.ch_block_size = mg->config.block_size;
			o->ch.ch_encoding = mg->config.encoding;
			o->ch.ch_sample_size = mg->config.sample_size;
			o->ch.ch_rate = mg->config.rate;
			o->ch.ch_tx_channels = mg->config.tx_channels;
			o->ch.ch_rx_channels = mg->config.rx_channels;
			o->ch.ch_flags = mg->config.flags;
			opt_ptr = (uchar *) o;
			opt_len = sizeof(o->ch);
			flags = MX_GET_OPT;
			return mg_optmgmt_ack(q, mg, opt_ptr, opt_len, flags);
		case 0:
		case MX_DEFAULT:
			o->ch.mg_obj_type = MG_OBJ_TYPE_CH;
			o->ch.ch_block_size = mg_default.block_size;
			o->ch.ch_encoding = mg_default.encoding;
			o->ch.ch_sample_size = mg_default.sample_size;
			o->ch.ch_rate = mg_default.rate;
			o->ch.ch_tx_channels = mg_default.tx_channels;
			o->ch.ch_rx_channels = mg_default.rx_channels;
			o->ch.ch_flags = mg_default.flags;
			opt_ptr = (uchar *) o;
			opt_len = sizeof(o->ch);
			flags = MX_DEFAULT;
			return mg_optmgmt_ack(q, mg, opt_ptr, opt_len, flags);
		}
	}
	goto badflag;
      badflag:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADFLAG);
      badopttype:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADOPTTYPE);
      badopt:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADOPT);
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
}

/*
 *  MG_ATTACH_REQ
 *  -----------------------------------
 *  Requests that the provider attach the specified multiplex and slot to a specified or newly created channel id.
 *  The primitive is acknowledged wtih the MG_ATTACH_ACK.
 */
STATIC int
mg_attach_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct mx *mx;
	struct ch *ch;
	struct MG_attach_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (mg->i_state) {
	case MGS_DETACHED:
	case MGS_ATTACHED:
		if (!p->mg_mx_id) {
			/* 
			   requesting stream */
			mx = (struct mx *) mg;
		} else {
			if (!(mx = mx_lookup(p->mg_mx_id)))
				goto badid;
			if (p->mg_mx_slot >= mx->config.tx_channels
			    && p->mg_mx_slot >= mx->config.rx_channels)
				goto einval;
		}
		for (ch = mx->ch.list; ch && ch->mx_slot != p->mg_mx_slot; ch = ch->mx.next) ;
		if (ch)
			goto badid;
		if (!(ch = ch_alloc_priv(0, mx, p->mg_mx_slot, p->mg_ch_type)))
			goto enomem;
		mg->i_state = MGS_WACK_AREQ;
		goto channel_known;
		/* 
		   fall through */
	case MGS_WACK_AREQ:
		if (!p->mg_mx_id) {
			/* 
			   requesting stream */
			mx = (struct mx *) mg;
		} else {
			if (!(mx = mx_lookup(p->mg_mx_id)))
				goto efault;
			if (p->mg_mx_slot >= mx->config.tx_channels
			    && p->mg_mx_slot >= mx->config.rx_channels)
				goto efault;
		}
		for (ch = mx->ch.list; ch && ch->mx_slot != p->mg_mx_slot; ch = ch->mx.next) ;
		if (!ch)
			goto efault;
	      channel_known:
		if ((err = mg_attach_ack(q, mg, ch)) < 0)
			return (err);
		mg->i_state = MGS_ATTACHED;
		return (QR_DONE);
	}
	goto outstate;
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      badid:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      enomem:
	return mg_error_ack(q, mg, p->mg_primitive, -ENOMEM);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
      einval:
	return mg_error_ack(q, mg, p->mg_primitive, -EINVAL);
      efault:
	return m_error(q, mg, -EFAULT);
}

/*
 *  MG_DETACH_REQ
 *  -----------------------------------
 *  Requests that the provider detach the requesting stream from an attached channel and acknowledge success with an
 *  MG_OK_ACK or MG_ERROR_ACK.
 */
STATIC int
mg_detach_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct ch *ch;
	struct MG_detach_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (mg->i_state) {
	case MGS_ATTACHED:
		if (!(ch = ch_lookup(p->mg_ch_id)))
			goto badid;
		ch_free_priv(ch);
		mg->i_state = MGS_WACK_DREQ;
		/* 
		   fall through */
	case MGS_WACK_DREQ:
		if ((err = mg_ok_ack(q, mg, p->mg_primitive)) < 0)
			return (err);
		if (mg->ch.list)
			mg->i_state = MGS_ATTACHED;
		else
			mg->i_state = MGS_DETACHED;
		return (QR_DONE);
	}
	goto outstate;
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      badid:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
}

/*
 *  MG_JOIN_REQ
 *  -----------------------------------
 *  Requests that the specified channels be joines to the specified session with newly created termination point
 *  with id specified by the caller (mg_tp_id non-zero) or assigned by the provider (mg_tp_id zero).  If the
 *  specified session id is zero then a new session will be created.  If the specified session id exists, the
 *  existing session will be joined.  If the specified session id is non-zero but does not exist, one will be
 *  created with the specified id.  All channels must be successfully enabled before this primitive will be
 *  confirmed.  This primitive is confirmed with the MG_JOIN_CON primitive.  It is refused with the MG_ERROR_ACK or
 *  MG_LEAVE_IND primitive.
 */
STATIC int
mg_join_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se;
	struct lg *lg;
	struct ch *ch, **chp;
	struct mx *mx;
	ulong *idp, *edp;
	ulong slot;
	int waiting;
	struct MG_join_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr > mp->b_rptr + p->mg_channel_length + p->mg_channel_offset)
		goto emsgsize;
	if (p->mg_channel_length < sizeof(ulong))
		goto badopt;
	if (!((1 << mg->i_state) & (MGSF_DETACHED | MGSF_ATTACHED)))
		goto outstate;
	if (!(se = se_lookup(p->mg_se_id, mg)) && !(se = se_alloc_priv(p->mg_se_id, mg)))
		goto enomem;
	switch (se->state) {
	case MGS_DETACHED:
	case MGS_ATTACHED:
	case MGS_JOINED:
		for (lg = se->lg.list; lg && lg->id != p->mg_tp_id; lg = lg->se.next) ;
		if (lg)
			goto idbusy;
		idp = (ulong *) (mp->b_rptr + p->mg_channel_offset);
		edp = idp + (p->mg_channel_length / sizeof(*idp));
		if (idp >= edp)
			goto efault;
		for (; idp < edp; idp++) {
			if (!(ch = ch_lookup(*idp)))
				goto badopt;
			if (ch->lg.lg)
				goto idbusy;
			if (!(mx = ch->mx.mx))
				goto efault;
			if (!((1 << mx->state) & (MXSF_DETACHED | MXSF_WCON_EREQ | MXSF_ENABLED)))
				goto idbusy;
		}
		if (!(lg = lg_alloc_priv(p->mg_tp_id, se))) {
			if (!se->lg.list)
				se_free_priv(se);
			goto enomem;
		}
		for (slot = 0, chp = &lg->ch.list; idp < edp; idp++, slot++) {
			/* 
			   link channels to leg */
			if ((ch->lg.next = *chp))
				ch->lg.next->lg.prev = &ch->lg.next;
			ch->lg.next = *chp;
			ch->lg.prev = chp;
			*chp = ch_get(ch);
			lg->ch.numb++;
			ch->lg_slot = slot;
			ch->lg.lg = lg_get(lg);
		}
		se->state = MGS_WACK_JREQ;
		p->mg_tp_id = lg->id;
		goto lg_known;
	case MGS_WACK_JREQ:
		for (lg = se->lg.list; lg && lg->id != p->mg_tp_id; lg = lg->se.next) ;
		if (!lg)
			goto efault;
	      lg_known:
		waiting = 0;
		/* 
		   make sure all the muxltiplex are enabled */
		for (ch = lg->ch.list; ch; ch = ch->lg.next) {
			if ((mx = ch->mx.mx)) {
				if (mx == (struct mx *) mg)
					continue;
				switch (mx->state) {
				case MXS_DETACHED:
					if ((err = mx_enable_req(q, mx)) < 0)
						return (err);
					mx->state = MXS_WCON_EREQ;
				case MXS_WCON_EREQ:
					waiting = 1;
				case MXS_ENABLED:
					continue;
				}
			}
			swerr();
		}
		if (waiting) {
			se->state = MGS_WCON_JREQ;
			return (QR_DONE);
		}
		if ((err = mg_join_con(q, mg, se, lg)) < 0)
			return (err);
		se->state = MGS_JOINED;
		return (QR_DONE);
	}
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
	// badid:
	// return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      idbusy:
	return mg_error_ack(q, mg, p->mg_primitive, MGIDBUSY);
      badopt:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADOPT);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
      enomem:
	ss7_bufcall(q, sizeof(*se), BPRI_MED);
	return (-ENOMEM);
      efault:
	return m_error(q, mg, -EFAULT);
}

/*
 *  MG_ACTION_REQ
 *  -----------------------------------
 *  Requests that the action of the requested type be performed against the specified session and termination point
 *  for the requested duration and with the requested options.  When more data is indicated using the MG_MORE_DATA
 *  flag, it indicates that subsequence MG_ACTION_REQ primitives contain more data for the associated pattern.  The
 *  data is encoded according to the media format of the requesting stream.  When the requested duration is zero,
 *  the action will continue until its normal termination, or until subsequently aborted.  Actions on terminations
 *  which are currently connected in a communications session will be mixed with the media received from the
 *  communications session and any other actions which are currently being performed on the termination.  Actions on
 *  terminations which are currently disconnected from a communications session will be mixed with the media from
 *  other actions on the termination point.  Some actions can only be performed on disconnected termination points
 *  (e.g., MG_ACTION_LOOPBACK, MG_ACTION_TEST_SILENT).  Some actions replace other actions on the termination point
 *  (e.g., MG_ACTION_LOOPBACK).  Some actions performed on a termination point will be performed on individual
 *  channels that make up the termination point (e.g., MG_ACTION_LOOPBACK).
 */
STATIC int
mg_action_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se;
	struct lg *lg;
	struct ch *ch;
	struct ac *ac;
	struct MG_action_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!((1 << mg->i_state) && (MGSF_DETACHED | MGSF_ATTACHED)))
		goto outstate;
	if (!(se = se_lookup(p->mg_se_id, mg)) || !(lg = lg_lookup(p->mg_tp_id, se)))
		goto badid;
	if (p->mg_action < MG_ACTION_FIRST || p->mg_action > MG_ACTION_LAST)
		goto badact;
	if (!(ch = lg->ch.list))
		goto efault;	/* legs should not exist without channels */
	for (ac = ch->ac.list; ac && (ac->type != p->mg_action || !(ac->flags & MG_MORE_DATA));
	     ac = ac->ch.next) ;
	if (!ac) {
		if (!
		    (ac =
		     ac_alloc_priv(0, p->mg_action, p->mg_flags, p->mg_duration, ch, mp->b_cont)))
			goto enomem;
	} else {
		if (mp->b_cont) {
			if (ac->buf)
				linkb(ac->buf, mp->b_cont);
			else {
				ac->buf = mp->b_cont;
				ac->pos = 0;
			}
		}
		if (!(p->mg_flags & MG_MORE_DATA))
			ac->flags &= ~MG_MORE_DATA;
	}
	if (!(ac->flags & MG_MORE_DATA))
		if ((err = mg_action_con(q, mg, se, lg, ac)) < 0)
			return (err);
	return (QR_TRIMMED);
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      badact:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADACT);
      badid:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
      enomem:
	ss7_bufcall(q, sizeof(*ac), BPRI_MED);
	return (-ENOMEM);
      efault:
	return m_error(q, mg, -EFAULT);
}

/*
 *  MG_ABORT_REQ
 *  -----------------------------------
 *  Requests that the specified action be aborted.  This primitive is confirmed with the MG_OK_ACK primitive.  If
 *  the action identifier is MG_ACTION_PREVIOUS, this primitive requests that the previously initiated (unconfirmed)
 *  action be aborted.  If the action identifier is zero, this primitive requests that all actions ont the specified
 *  termination point be aborted.
 */
STATIC int
mg_abort_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se;
	struct lg *lg;
	struct ch *ch;
	struct ac *ac;
	struct MG_abort_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!((1 << mg->i_state) && (MGSF_DETACHED | MGSF_ATTACHED)))
		goto outstate;
	if (!(se = se_lookup(p->mg_se_id, mg)))
		goto badid;
	if (!((1 << se->state) && (MGSF_JOINED | MGSF_CONNECTED)))
		goto outstate;
	if (!(lg = lg_lookup(p->mg_tp_id, se)))
		goto badid;
	if (!((1 << lg->state) && (MGSF_JOINED | MGSF_CONNECTED)))
		goto outstate;
	if (p->mg_action_id) {
		/* 
		   abort specified action on termination point */
		for (ch = lg->ch.list; ch; ch = ch->lg.next)
			for (ac = ch->ac.list; ac; ac = ac->ch.next)
				if (ac->id == p->mg_action_id)
					goto found;
		goto badid;
	      found:
		if ((err = mg_ok_ack(q, mg, p->mg_primitive)) < 0)
			return (err);
		ac_free_priv(ac);	/* this will destroy the action */
		return (QR_DONE);
	} else {
		/* 
		   abort all actions on termination point */
		for (ch = lg->ch.list; ch; ch = ch->lg.next)
			while ((ac = ch->ac.list))
				ac_free_priv(ac);
		return mg_ok_ack(q, mg, p->mg_primitive);
	}
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      badid:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
}

/*
 *  MG_CONN_REQ
 *  -----------------------------------
 *  Requests that the requested termination point be connected into the communications session in the directions
 *  indicated by the mg_conn_flags, with the digital padding specified and the optional topology description.  If
 *  the optional topology description is not included, it is assumed that the termination point is requested to be
 *  connected to all other participants in the communications session in the directions requested.  If the optional
 *  topology description is included, it contains the list of other termination points in the session to which the
 *  specified termination point is to be connected in the directions requested.  If the termination point is not
 *  specified (mg_tp_id is zero) it is assumed that all participants (joined termination points) in the
 *  communications session are to be connected to all other participants in the communication session in full mesh.
 */
STATIC int
mg_conn_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se;
	struct lg *lg, *l2;
	struct ch *ch, *c2;
	struct cn *cn;
	struct MG_conn_req *p = (typeof(p)) mp->b_rptr;
	int err, single, waiting;
	ulong flags;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mg_topology_offset + p->mg_topology_length)
		goto emsgsize;
	if (!(p->mg_conn_flags & (MGF_IC_DIR | MGF_OG_DIR)))
		p->mg_conn_flags |= (MGF_IC_DIR | MGF_OG_DIR);
	if (!((1 << mg->i_state) && (MGSF_DETACHED | MGSF_ATTACHED)))
		goto outstate;
	if (!(se = se_lookup(p->mg_se_id, mg)))
		goto badid;
	if (se->state != MGS_JOINED)
		goto outstate;
	if (p->mg_tp_id) {
		if (!(lg = lg_lookup(p->mg_tp_id, se)))
			goto badid;
		if (lg->state != MGS_JOINED && lg->state != MGS_CONNECTED)
			goto outstate;
		if (p->mg_topology_length)	/* not supported yet */
			goto notsupp;
		single = 1;
		waiting = 0;
		goto lg_known;
	} else {
		lg = se->lg.list;
		if (p->mg_topology_length)
			goto badopt;
		single = 0;
		waiting = 0;
	}
	for (; lg && !single; lg = lg->se.next) {
	      lg_known:
		for (ch = lg->ch.list; ch; ch = ch->lg.next) {
			struct mx *mx, *m2;
			if (!(mx = ch->mx.mx))
				continue;
			switch (ch->state) {
			case MGS_CONNECTED:
			case MGS_JOINED:
				for (l2 = se->lg.list; l2 && l2 != lg; l2 = l2->se.next) {
					for (c2 = l2->ch.list; c2; c2 = c2->lg.next) {
						if (c2->lg_slot == ch->lg_slot) {
							flags = 0;
							m2 = c2->mx.mx;
							if (p->mg_conn_flags & MGF_IC_DIR) {
								for (cn = ch->ic.list;
								     cn && cn->og.ch != c2;
								     cn = cn->ic.next) ;
								if (!cn
								    && !(cn =
									 cn_alloc_priv(ch, c2,
										       p->
										       mg_padding)))
									return (-ENOMEM);
								c2->flags |= MXF_RX_DIR;
							}
							if (p->mg_conn_flags & MGF_OG_DIR) {
								for (cn = ch->og.list;
								     cn && cn->ic.ch != c2;
								     cn = cn->og.next) ;
								if (!cn
								    && !(cn =
									 cn_alloc_priv(c2, ch,
										       p->
										       mg_padding)))
									return (-ENOMEM);
								c2->flags |= MXF_TX_DIR;
							}
							switch (c2->state) {
							case MGS_JOINED:
								if ((err =
								     mx_connect_req(q, m2, c2)) < 0)
									return (err);
								c2->state = MGS_WCON_CREQ;
							case MGS_WCON_CREQ:
								break;
							}
						}
					}
				}
				ch->state = MGS_WACK_CREQ;
			case MGS_WACK_CREQ:
				if (mx == (struct mx *) mg) {
					ch->state = MGS_CONNECTED;
					lg->state = MGS_CONNECTED;
					continue;
				}
				ch->flags |=
				    ((p->mg_conn_flags & MGF_IC_DIR) ? MXF_TX_DIR : 0) | ((p->
											   mg_conn_flags
											   &
											   MGF_OG_DIR)
											  ?
											  MXF_RX_DIR
											  : 0);
				if ((err = mx_connect_req(q, mx, ch)) < 0)
					return (err);
				ch->state = MGS_WCON_CREQ;
			case MGS_WCON_CREQ:
				waiting = 1;
				break;
			}
		}
	}
	if (waiting) {
		se->state = MGS_WCON_DREQ;
		return (QR_DONE);
	}
	if ((err = mg_conn_con(q, se->mg.mg, se, lg)) < 0)	/* careful, lg can be NULL */
		return (err);
	return (QR_DONE);
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      notsupp:
	return mg_error_ack(q, mg, p->mg_primitive, MGNOTSUPP);
      badid:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      badopt:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADOPT);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
}

/*
 *  MG_DATA_REQ
 *  -----------------------------------
 *  Requests that the provider send data to the communications session.
 */
STATIC int
mg_data_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct ch *ch;
	struct MG_data_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!mp->b_cont || !msgsize(mp->b_cont))
		goto eproto;
	for (ch = mg->ch.list; ch && ch->mx_slot != p->mg_mx_slot; ch = ch->mx.next) ;
	if (!ch)
		goto discard;
	if ((err = ch_write(q, mp->b_cont, ch)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
      eproto:
	return m_error(q, mg, -EPROTO);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  MG_DISCON_REQ
 *  -----------------------------------
 *  Requests that the termination point be disconnected from the communications session in the directions indicated
 *  by the mg_conn_flags, and the optional topology description (not yet supported).  If the optional topology
 *  description is not specified, it is assumed that the termination point is to be disconnected from all other
 *  participants in the communications session for the directions requested.  If the optional topology description
 *  is specified, it contains the list of other termination points in the session from which the specified
 *  termination point is to be disconnected in the direction requested.
 */
STATIC int
mg_discon_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se;
	struct lg *lg;
	struct ch *ch, *c2;
	struct cn *cn;
	struct MG_discon_req *p = (typeof(p)) mp->b_rptr;
	int err, single, waiting;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr < mp->b_rptr + p->mg_topology_offset + p->mg_topology_length)
		goto emsgsize;
	if (!((1 << mg->i_state) && (MGSF_DETACHED | MGSF_ATTACHED)))
		goto outstate;
	if (!(se = se_lookup(p->mg_se_id, mg)))
		goto badid;
	if (se->state != MGS_JOINED)
		goto outstate;
	if (p->mg_tp_id) {
		if (!(lg = lg_lookup(p->mg_tp_id, se)))
			goto badid;
		if (lg->state != MGS_CONNECTED)
			goto outstate;
		if (p->mg_topology_length)
			goto eopnotsupp;	/* not supported yet */
		single = 1;
		waiting = 0;
		goto lg_known;
	} else {
		if (p->mg_topology_length)
			goto badopt;
		lg = se->lg.list;
		single = 0;
		waiting = 0;
	}
	for (; lg && !single; lg = lg->se.next) {
	      lg_known:
		for (ch = lg->ch.list; ch; ch = ch->lg.next) {
			struct mx *mx;
			if (!ch->og.list && !ch->ic.list)
				continue;
			if (!(mx = ch->mx.mx))
				continue;
			switch (ch->state) {
			case MGS_CONNECTED:
				if (p->mg_conn_flags & MGF_IC_DIR) {
					while ((cn = ch->ic.list)) {
						/* 
						   check if we are only connection to other end */
						if ((c2 = cn->og.ch) && c2->state == MXS_CONNECTED
						    && c2->flags & MXF_TX_DIR && c2->og.numb == 1
						    && (err =
							mx_disconnect_req(q, c2->mx.mx, c2,
									  MXF_TX_DIR)))
							return (err);
						cn_free_priv(ch->ic.list);
					}
				}
				if (p->mg_conn_flags & MGF_OG_DIR) {
					while ((cn = ch->og.list)) {
						/* 
						   check if we are only connection to other end */
						if ((c2 = cn->ic.ch) && c2->state == MXS_CONNECTED
						    && c2->flags & MXF_RX_DIR && c2->ic.numb == 1
						    && (err =
							mx_disconnect_req(q, c2->mx.mx, c2,
									  MXF_RX_DIR)))
							return (err);
						cn_free_priv(ch->og.list);
					}
				}
				ch->state = MGS_WACK_DREQ;
			case MGS_WACK_DREQ:
				if ((err = mx_disconnect_req(q, mx, ch, p->mg_conn_flags)) < 0)
					return (err);
			case MGS_WCON_DREQ:
				waiting = 1;
				break;
			}
		}
		if (!single)
			lg->state = MGS_WCON_DREQ;
	}
	if (waiting) {
		se->state = MGS_WCON_DREQ;
		if (single)
			lg->state = MGS_WCON_DREQ;
		return (QR_DONE);
	}
	if ((err = mg_discon_con(q, mg, se, lg, p->mg_conn_flags)) < 0)	/* careful, lg can be NULL */
		return (err);
	return (QR_DONE);
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      badid:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      badopt:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADOPT);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
      eopnotsupp:
	return mg_error_ack(q, mg, p->mg_primitive, -EOPNOTSUPP);
}

/*
 *  MG_LEAVE_REQ
 *  -----------------------------------
 *  Requests that the specified termination point (mg_tp_id non-zero) or all termination points (mg_tp_id zero)
 *  leave the specified communication session.  Once all termination points leave a communications session, the
 *  communications session is destroyed.
 */
STATIC int
mg_leave_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se;
	struct lg *lg = NULL;
	struct ch *ch, *c2;
	struct mx *mx;
	int disable, single, waiting = 0;
	struct MG_leave_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (!((1 << mg->i_state) && (MGSF_DETACHED | MGSF_ATTACHED)))
		goto outstate;
	if (!(se = se_lookup(p->mg_se_id, mg)))
		goto badid;
	switch (se->state) {
	case MGS_JOINED:
		if (!p->mg_tp_id) {
			for (lg = se->lg.list; lg && lg->id != p->mg_tp_id; lg = lg->se.next) ;
			if (!lg)
				goto badid;
			for (ch = lg->ch.list; ch; ch = ch->lg.next)
				if (ch->og.list || ch->ic.list)
					goto outstate;
			single = 1;
			waiting = 0;
			goto lg_known;
		}
		/* 
		   fall through */
	case MGS_WACK_LREQ:
		break;
	default:
		goto outstate;
	}
	if (se) {
		if (!p->mg_tp_id) {
			for (lg = se->lg.list; lg && lg->id != p->mg_tp_id; lg = lg->se.next) ;
			if (!lg)
				goto efault;
			single = 1;
			waiting = 0;
		} else {
			lg = se->lg.list;
			single = 0;
			waiting = 0;
		}
		for (; lg && !single; lg = lg->se.next) {
		      lg_known:
			for (ch = lg->ch.list; ch && ch->mx.mx != (struct mx *) mg;
			     ch = ch->lg.next) {
				if ((mx = ch->mx.mx)) {
					if (mx == (struct mx *) mg)
						continue;
					switch (mx->state) {
					case MXS_ENABLED:
						disable = 1;
						for (c2 = mx->ch.list; c2 && c2 != ch;
						     c2 = c2->mx.next) {
							if (c2->lg.lg)
								disable = 0;
						}
						if (!disable)
							continue;
						if ((err = mx_disable_req(q, mx)) < 0)
							return (err);
						mx->state = MXS_WCON_DREQ;
						/* 
						   fall through */
					case MXS_WCON_DREQ:
						waiting = 1;
						continue;
					}
				}
				swerr();
			}
			lg_free_priv(lg);
		}
		if (!single)
			se_free_priv(se);
	}
	if (waiting) {
		se->state = MGS_WCON_LREQ;
		return (QR_DONE);
	}
	if ((err = mg_leave_con(q, mg, se, lg)) < 0)	/* careful, lg can be NULL */
		return (err);
	return (QR_DONE);
      outstate:
	return mg_error_ack(q, mg, p->mg_primitive, MGOUTSTATE);
      badid:
	return mg_error_ack(q, mg, p->mg_primitive, MGBADID);
      emsgsize:
	return mg_error_ack(q, mg, p->mg_primitive, -EMSGSIZE);
      efault:
	return m_error(q, mg, -EFAULT);
}

/*
 *  MG_NOTIFY_REQ
 *  -----------------------------------
 *  Requests that the provider provide event notifications to the requesting stream as
 *  specified in the request.
 */
STATIC int
mg_notify_req(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	int err;
	(void) mg;
	err = -EFAULT;
	fixme(("Write this function\n"));
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IO Controls
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  GET OPTIONS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_opt_get_mg(mg_option_t * arg, struct mg *mg, int size)
{
	mg_opt_conf_mg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!mg)
		return (-EINVAL);
	*opt = mg->config;
	return (QR_DONE);
}
STATIC int
mg_opt_get_se(mg_option_t * arg, struct se *se, int size)
{
	mg_opt_conf_se_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!se)
		return (-EINVAL);
	*opt = se->config;
	return (QR_DONE);
}
STATIC int
mg_opt_get_lg(mg_option_t * arg, struct lg *lg, int size)
{
	mg_opt_conf_lg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!lg)
		return (-EINVAL);
	*opt = lg->config;
	return (QR_DONE);
}
STATIC int
mg_opt_get_ch(mg_option_t * arg, struct ch *ch, int size)
{
	mg_opt_conf_ch_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ch)
		return (-EINVAL);
	*opt = ch->config;
	return (QR_DONE);
}
STATIC int
mg_opt_get_mx(mg_option_t * arg, struct mx *mx, int size)
{
	mg_opt_conf_mx_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!mx)
		return (-EINVAL);
	*opt = mx->config;
	return (QR_DONE);
}
STATIC int
mg_opt_get_df(mg_option_t * arg, struct df *df, int size)
{
	mg_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
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
mg_opt_set_mg(mg_option_t * arg, struct mg *mg, int size)
{
	mg_opt_conf_mg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!mg)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	mg->config = *opt;
	return (QR_DONE);
}
STATIC int
mg_opt_set_se(mg_option_t * arg, struct se *se, int size)
{
	mg_opt_conf_se_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!se)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	se->config = *opt;
	return (QR_DONE);
}
STATIC int
mg_opt_set_lg(mg_option_t * arg, struct lg *lg, int size)
{
	mg_opt_conf_lg_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!lg)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	lg->config = *opt;
	return (QR_DONE);
}
STATIC int
mg_opt_set_ch(mg_option_t * arg, struct ch *ch, int size)
{
	mg_opt_conf_ch_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!ch)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	ch->config = *opt;
	return (QR_DONE);
}
STATIC int
mg_opt_set_mx(mg_option_t * arg, struct mx *mx, int size)
{
	mg_opt_conf_mx_t *opt = (typeof(opt)) (arg + 1);
	if ((size -= sizeof(*opt)) < 0)
		return (-EINVAL);
	if (!mx)
		return (-EINVAL);
	todo(("Check options before setting them\n"));
	mx->config = *opt;
	return (QR_DONE);
}
STATIC int
mg_opt_set_df(mg_option_t * arg, struct df *df, int size)
{
	mg_opt_conf_df_t *opt = (typeof(opt)) (arg + 1);
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
mg_get_mg(mg_config_t * arg, struct mg *mg, int size)
{
	struct se *se;
	mg_conf_mg_t *cnf = (typeof(cnf)) (arg + 1);
	mg_conf_se_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!mg || size < sizeof(*arg))
		return (-EINVAL);
	/* 
	   write out list of session configuration following media gateway configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (se = mg->se.list; se && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     se = se->mg.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MG_OBJ_TYPE_SE;
		arg->id = se->id;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mg_get_se(mg_config_t * arg, struct se *se, int size)
{
	struct lg *lg;
	mg_conf_se_t *cnf = (typeof(cnf)) (arg + 1);
	mg_conf_lg_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!se || size < sizeof(*arg))
		return (-EINVAL);
	/* 
	   write out list of session configuration following media gateway configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (lg = se->lg.list; lg && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     lg = lg->se.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MG_OBJ_TYPE_LG;
		arg->id = lg->id;
		chd->seid = lg->se.se ? lg->se.se->id : 0;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mg_get_lg(mg_config_t * arg, struct lg *lg, int size)
{
	struct ch *ch;
	mg_conf_lg_t *cnf = (typeof(cnf)) (arg + 1);
	mg_conf_ch_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!lg || size < sizeof(*arg))
		return (-EINVAL);
	cnf->seid = lg->se.se ? lg->se.se->id : 0;
	/* 
	   write out list of session configuration following media gateway configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (ch = lg->ch.list; ch && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     ch = ch->lg.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MG_OBJ_TYPE_CH;
		arg->id = ch->id;
		chd->tpid = ch->lg.lg ? ch->lg.lg->id : 0;
		chd->mxid = ch->mx.mx ? ch->mx.mx->id : 0;
		chd->slot = ch->mx_slot;
		chd->encoding = ch->config.encoding;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mg_get_ch(mg_config_t * arg, struct ch *ch, int size)
{
	mg_conf_ch_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!ch || size < sizeof(*arg))
		return (-EINVAL);
	cnf->tpid = ch->lg.lg ? ch->lg.lg->id : 0;
	cnf->mxid = ch->mx.mx ? ch->mx.mx->id : 0;
	cnf->slot = ch->mx_slot;
	cnf->encoding = ch->config.encoding;
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mg_get_mx(mg_config_t * arg, struct mx *mx, int size)
{
	struct ch *ch;
	mg_conf_mx_t *cnf = (typeof(cnf)) (arg + 1);
	mg_conf_ch_t *chd;
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!mx || size < sizeof(*arg))
		return (-EINVAL);
	cnf->muxid = mx->u.mux.index;
	/* 
	   write out list of session configuration following media gateway configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (ch = mx->ch.list; ch && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     ch = ch->mx.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MG_OBJ_TYPE_CH;
		arg->id = ch->id;
		chd->tpid = ch->lg.lg ? ch->lg.lg->id : 0;
		chd->mxid = ch->mx.mx ? ch->mx.mx->id : 0;
		chd->slot = ch->mx_slot;
		chd->encoding = ch->config.encoding;
	}
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}
STATIC int
mg_get_df(mg_config_t * arg, struct df *df, int size)
{
	mg_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if ((size -= sizeof(*cnf)) < sizeof(*arg))
		return (-EINVAL);
	if (!df || size < sizeof(*arg))
		return (-EINVAL);
	arg = (typeof(arg)) (cnf + 1);
	/* 
	   terminate list with zero object type */
	arg->type = 0;
	arg->id = 0;
	return (QR_DONE);
}

/*
 *  ADD Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_add_mg(mg_config_t * arg, struct mg *mg, int size, int force, int test)
{
	mg_conf_mg_t *cnf = (typeof(cnf)) (arg + 1);
	if (mg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   MG objects are not added statically */
	return (-EINVAL);
}
STATIC int
mg_add_se(mg_config_t * arg, struct se *se, int size, int force, int test)
{
	mg_conf_se_t *cnf = (typeof(cnf)) (arg + 1);
	if (se || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   SE objects are not added statically */
	return (-EINVAL);
}
STATIC int
mg_add_lg(mg_config_t * arg, struct lg *lg, int size, int force, int test)
{
	mg_conf_lg_t *cnf = (typeof(cnf)) (arg + 1);
	if (lg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   LG objects are not added statically */
	return (-EINVAL);
}
STATIC int
mg_add_ch(mg_config_t * arg, struct ch *ch, int size, int force, int test)
{
	/* 
	   Channels for lower MX streams are allocated statically. */
	/* 
	   Channels for upper MG streams are allocated dynamically. */
	struct mx *mx;
	mg_conf_ch_t *cnf = (typeof(cnf)) (arg + 1);
	if (ch || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	for (ch = master.ch.list; ch; ch = ch->next)
		if (ch->id == arg->id)
			return (-EBUSY);
	for (mx = master.mx.list; mx && mx->id != cnf->mxid; mx = mx->next) ;
	if (!mx)
		return (-EINVAL);
	for (ch = mx->ch.list; ch; ch = ch->mx.next)
		if (ch->mx_slot == cnf->slot)
			return (-EBUSY);
	if (!force) {
		todo(("Check if active first\n"));
	}
	if (!test) {
		if (!(ch = ch_alloc_priv(ch_get_id(arg->id), mx, cnf->slot, cnf->encoding)))
			return (-ENOMEM);
		arg->id = ch->id;
	}
	return (QR_DONE);
}
STATIC int
mg_add_mx(mg_config_t * arg, struct mx *mx, int size, int force, int test)
{
	mg_conf_mx_t *cnf = (typeof(cnf)) (arg + 1);
	if (mx || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	for (mx = master.mx.list; mx; mx = mx->next)
		if (mx->u.mux.index == cnf->muxid) {
			if (mx->id)
				return (-EINVAL);
			break;
		}
	if (!mx)
		return (-EINVAL);
	if (!force) {
		todo(("Check if mx is active first\n"));
	}
	if (!test) {
		mx->id = mx_get_id(arg->id);
		arg->id = mx->id;
	}
	return (QR_DONE);
}
STATIC int
mg_add_df(mg_config_t * arg, struct df *df, int size, int force, int test)
{
	mg_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	return (-EINVAL);
}

/*
 *  CHA Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_cha_mg(mg_config_t * arg, struct mg *mg, int size, int force, int test)
{
	mg_conf_mg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!mg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   nothing to change */
	return (QR_DONE);
}
STATIC int
mg_cha_se(mg_config_t * arg, struct se *se, int size, int force, int test)
{
	mg_conf_se_t *cnf = (typeof(cnf)) (arg + 1);
	if (!se || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   nothing to change */
	return (QR_DONE);
}
STATIC int
mg_cha_lg(mg_config_t * arg, struct lg *lg, int size, int force, int test)
{
	mg_conf_lg_t *cnf = (typeof(cnf)) (arg + 1);
	if (!lg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   nothing to change */
	return (QR_DONE);
}
STATIC int
mg_cha_ch(mg_config_t * arg, struct ch *ch, int size, int force, int test)
{
	mg_conf_ch_t *cnf = (typeof(cnf)) (arg + 1);
	if (!ch || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   nothing to change */
	return (QR_DONE);
}
STATIC int
mg_cha_mx(mg_config_t * arg, struct mx *mx, int size, int force, int test)
{
	mg_conf_mx_t *cnf = (typeof(cnf)) (arg + 1);
	if (!mx || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   nothing to change */
	return (QR_DONE);
}
STATIC int
mg_cha_df(mg_config_t * arg, struct df *df, int size, int force, int test)
{
	mg_conf_df_t *cnf = (typeof(cnf)) (arg + 1);
	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* 
	   nothing to change */
	return (QR_DONE);
}

/*
 *  DEL Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_del_mg(mg_config_t * arg, struct mg *mg, int size, int force, int test)
{
	if (!mg)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// mg_free_priv(mg);
	}
	return (QR_DONE);
}
STATIC int
mg_del_se(mg_config_t * arg, struct se *se, int size, int force, int test)
{
	if (!se)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// se_free_priv(mg);
	}
	return (-EINVAL);
}
STATIC int
mg_del_lg(mg_config_t * arg, struct lg *lg, int size, int force, int test)
{
	if (!lg)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// lg_free_priv(mg);
	}
	return (-EINVAL);
}
STATIC int
mg_del_ch(mg_config_t * arg, struct ch *ch, int size, int force, int test)
{
	if (!ch)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// ch_free_priv(mg);
	}
	return (-EINVAL);
}
STATIC int
mg_del_mx(mg_config_t * arg, struct mx *mx, int size, int force, int test)
{
	if (!mx)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// mx_free_priv(mg);
	}
	return (QR_DONE);
}
STATIC int
mg_del_df(mg_config_t * arg, struct df *df, int size, int force, int test)
{
	if (!df)
		return (-EINVAL);
	if (!force) {
	}
	if (!test) {
		// df_free_priv(mg);
	}
	return (QR_DONE);
}

/*
 *  GET State
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_sta_mg(mg_statem_t * arg, struct mg *mg, int size)
{
	mg_statem_mg_t *sta = (typeof(sta)) (arg + 1);
	if (!mg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = mg->flags;
	arg->state = mg->state;
	sta->timers = mg->timers;
	return (QR_DONE);
}
STATIC int
mg_sta_se(mg_statem_t * arg, struct se *se, int size)
{
	mg_statem_se_t *sta = (typeof(sta)) (arg + 1);
	if (!se || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = se->flags;
	arg->state = se->state;
	sta->timers = se->timers;
	return (QR_DONE);
}
STATIC int
mg_sta_lg(mg_statem_t * arg, struct lg *lg, int size)
{
	mg_statem_lg_t *sta = (typeof(sta)) (arg + 1);
	if (!lg || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = lg->flags;
	arg->state = lg->state;
	sta->timers = lg->timers;
	return (QR_DONE);
}
STATIC int
mg_sta_ch(mg_statem_t * arg, struct ch *ch, int size)
{
	mg_statem_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = ch->flags;
	arg->state = ch->state;
	sta->timers = ch->timers;
	return (QR_DONE);
}
STATIC int
mg_sta_mx(mg_statem_t * arg, struct mx *mx, int size)
{
	mg_statem_mx_t *sta = (typeof(sta)) (arg + 1);
	if (!mx || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = mx->flags;
	arg->state = mx->state;
	sta->timers = mx->timers;
	return (QR_DONE);
}
STATIC int
mg_sta_df(mg_statem_t * arg, struct df *df, int size)
{
	mg_statem_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < sizeof(ulong))
		return (-EINVAL);
	arg->flags = 0;
	arg->state = 0;
	return (QR_DONE);
}

/*
 *  GET Statistics periods
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_statp_get_mg(mg_stats_t * arg, struct mg *mg, int size)
{
	mg_stats_mg_t *sta = (typeof(sta)) (arg + 1);
	if (!mg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = mg->statsp;
	return (QR_DONE);
}
STATIC int
mg_statp_get_se(mg_stats_t * arg, struct se *se, int size)
{
	mg_stats_se_t *sta = (typeof(sta)) (arg + 1);
	if (!se || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = se->statsp;
	return (QR_DONE);
}
STATIC int
mg_statp_get_lg(mg_stats_t * arg, struct lg *lg, int size)
{
	mg_stats_lg_t *sta = (typeof(sta)) (arg + 1);
	if (!lg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = lg->statsp;
	return (QR_DONE);
}
STATIC int
mg_statp_get_ch(mg_stats_t * arg, struct ch *ch, int size)
{
	mg_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = ch->statsp;
	return (QR_DONE);
}
STATIC int
mg_statp_get_mx(mg_stats_t * arg, struct mx *mx, int size)
{
	mg_stats_mx_t *sta = (typeof(sta)) (arg + 1);
	if (!mx || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = mx->statsp;
	return (QR_DONE);
}
STATIC int
mg_statp_get_df(mg_stats_t * arg, struct df *df, int size)
{
	mg_stats_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = df->statsp;
	return (QR_DONE);
}

/*
 *  SET Statistics periods
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_statp_set_mg(mg_stats_t * arg, struct mg *mg, int size)
{
	mg_stats_mg_t *sta = (typeof(sta)) (arg + 1);
	if (!mg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	mg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
mg_statp_set_se(mg_stats_t * arg, struct se *se, int size)
{
	mg_stats_se_t *sta = (typeof(sta)) (arg + 1);
	if (!se || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	se->statsp = *sta;
	return (QR_DONE);
}
STATIC int
mg_statp_set_lg(mg_stats_t * arg, struct lg *lg, int size)
{
	mg_stats_lg_t *sta = (typeof(sta)) (arg + 1);
	if (!lg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	lg->statsp = *sta;
	return (QR_DONE);
}
STATIC int
mg_statp_set_ch(mg_stats_t * arg, struct ch *ch, int size)
{
	mg_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	ch->statsp = *sta;
	return (QR_DONE);
}
STATIC int
mg_statp_set_mx(mg_stats_t * arg, struct mx *mx, int size)
{
	mg_stats_mx_t *sta = (typeof(sta)) (arg + 1);
	if (!mx || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	mx->statsp = *sta;
	return (QR_DONE);
}
STATIC int
mg_statp_set_df(mg_stats_t * arg, struct df *df, int size)
{
	mg_stats_df_t *sta = (typeof(sta)) (arg + 1);
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
mg_stat_get_mg(mg_stats_t * arg, struct mg *mg, int size)
{
	mg_stats_mg_t *sta = (typeof(sta)) (arg + 1);
	if (!mg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = mg->stats;
	return (QR_DONE);
}
STATIC int
mg_stat_get_se(mg_stats_t * arg, struct se *se, int size)
{
	mg_stats_se_t *sta = (typeof(sta)) (arg + 1);
	if (!se || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = se->stats;
	return (QR_DONE);
}
STATIC int
mg_stat_get_lg(mg_stats_t * arg, struct lg *lg, int size)
{
	mg_stats_lg_t *sta = (typeof(sta)) (arg + 1);
	if (!lg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = lg->stats;
	return (QR_DONE);
}
STATIC int
mg_stat_get_ch(mg_stats_t * arg, struct ch *ch, int size)
{
	mg_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = ch->stats;
	return (QR_DONE);
}
STATIC int
mg_stat_get_mx(mg_stats_t * arg, struct mx *mx, int size)
{
	mg_stats_mx_t *sta = (typeof(sta)) (arg + 1);
	if (!mx || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	*sta = mx->stats;
	return (QR_DONE);
}
STATIC int
mg_stat_get_df(mg_stats_t * arg, struct df *df, int size)
{
	mg_stats_df_t *sta = (typeof(sta)) (arg + 1);
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
mg_stat_clr_mg(mg_stats_t * arg, struct mg *mg, int size)
{
	uchar *s, *d;
	mg_stats_mg_t *sta = (typeof(sta)) (arg + 1);
	if (!mg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & mg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
mg_stat_clr_se(mg_stats_t * arg, struct se *se, int size)
{
	uchar *s, *d;
	mg_stats_se_t *sta = (typeof(sta)) (arg + 1);
	if (!se || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & se->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
mg_stat_clr_lg(mg_stats_t * arg, struct lg *lg, int size)
{
	uchar *s, *d;
	mg_stats_lg_t *sta = (typeof(sta)) (arg + 1);
	if (!lg || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & lg->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
mg_stat_clr_ch(mg_stats_t * arg, struct ch *ch, int size)
{
	uchar *s, *d;
	mg_stats_ch_t *sta = (typeof(sta)) (arg + 1);
	if (!ch || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & ch->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
mg_stat_clr_mx(mg_stats_t * arg, struct mx *mx, int size)
{
	uchar *s, *d;
	mg_stats_mx_t *sta = (typeof(sta)) (arg + 1);
	if (!mx || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & mx->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}
STATIC int
mg_stat_clr_df(mg_stats_t * arg, struct df *df, int size)
{
	uchar *s, *d;
	mg_stats_df_t *sta = (typeof(sta)) (arg + 1);
	if (!df || (size -= sizeof(*sta)) < 0)
		return (-EINVAL);
	s = (typeof(s)) (sta + 1);
	d = (typeof(d)) & df->stats;
	for (size = sizeof(*sta); size--; *d &= *s) ;
	return (QR_DONE);
}

/*
 *  GET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_not_get_mg(mg_notify_t * arg, struct mg *mg, int size)
{
	mg_notify_mg_t *not = (typeof(not)) (arg + 1);
	if (!mg || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	not->events = mg->notify.events;
	return (QR_DONE);
}
STATIC int
mg_not_get_se(mg_notify_t * arg, struct se *se, int size)
{
	mg_notify_se_t *not = (typeof(not)) (arg + 1);
	if (!se || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	not->events = se->notify.events;
	return (QR_DONE);
}
STATIC int
mg_not_get_lg(mg_notify_t * arg, struct lg *lg, int size)
{
	mg_notify_lg_t *not = (typeof(not)) (arg + 1);
	if (!lg || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	not->events = lg->notify.events;
	return (QR_DONE);
}
STATIC int
mg_not_get_ch(mg_notify_t * arg, struct ch *ch, int size)
{
	mg_notify_ch_t *not = (typeof(not)) (arg + 1);
	if (!ch || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	not->events = ch->notify.events;
	return (QR_DONE);
}
STATIC int
mg_not_get_mx(mg_notify_t * arg, struct mx *mx, int size)
{
	mg_notify_mx_t *not = (typeof(not)) (arg + 1);
	if (!mx || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	not->events = mx->notify.events;
	return (QR_DONE);
}
STATIC int
mg_not_get_df(mg_notify_t * arg, struct df *df, int size)
{
	mg_notify_df_t *not = (typeof(not)) (arg + 1);
	if (!df || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	not->events = df->notify.events;
	return (QR_DONE);
}

/*
 *  SET Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_not_set_mg(mg_notify_t * arg, struct mg *mg, int size)
{
	mg_notify_mg_t *not = (typeof(not)) (arg + 1);
	if (!mg || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	mg->notify.events |= not->events;
	return (QR_DONE);
}
STATIC int
mg_not_set_se(mg_notify_t * arg, struct se *se, int size)
{
	mg_notify_se_t *not = (typeof(not)) (arg + 1);
	if (!se || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	se->notify.events |= not->events;
	return (QR_DONE);
}
STATIC int
mg_not_set_lg(mg_notify_t * arg, struct lg *lg, int size)
{
	mg_notify_lg_t *not = (typeof(not)) (arg + 1);
	if (!lg || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	lg->notify.events |= not->events;
	return (QR_DONE);
}
STATIC int
mg_not_set_ch(mg_notify_t * arg, struct ch *ch, int size)
{
	mg_notify_ch_t *not = (typeof(not)) (arg + 1);
	if (!ch || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	ch->notify.events |= not->events;
	return (QR_DONE);
}
STATIC int
mg_not_set_mx(mg_notify_t * arg, struct mx *mx, int size)
{
	mg_notify_mx_t *not = (typeof(not)) (arg + 1);
	if (!mx || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	mx->notify.events |= not->events;
	return (QR_DONE);
}
STATIC int
mg_not_set_df(mg_notify_t * arg, struct df *df, int size)
{
	mg_notify_df_t *not = (typeof(not)) (arg + 1);
	if (!df || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	df->notify.events |= not->events;
	return (QR_DONE);
}

/*
 *  CLR Notifications
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_not_clr_mg(mg_notify_t * arg, struct mg *mg, int size)
{
	mg_notify_mg_t *not = (typeof(not)) (arg + 1);
	if (!mg || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	mg->notify.events &= ~not->events;
	return (QR_DONE);
}
STATIC int
mg_not_clr_se(mg_notify_t * arg, struct se *se, int size)
{
	mg_notify_se_t *not = (typeof(not)) (arg + 1);
	if (!se || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	se->notify.events &= ~not->events;
	return (QR_DONE);
}
STATIC int
mg_not_clr_lg(mg_notify_t * arg, struct lg *lg, int size)
{
	mg_notify_lg_t *not = (typeof(not)) (arg + 1);
	if (!lg || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	lg->notify.events &= ~not->events;
	return (QR_DONE);
}
STATIC int
mg_not_clr_ch(mg_notify_t * arg, struct ch *ch, int size)
{
	mg_notify_ch_t *not = (typeof(not)) (arg + 1);
	if (!ch || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	ch->notify.events &= ~not->events;
	return (QR_DONE);
}
STATIC int
mg_not_clr_mx(mg_notify_t * arg, struct mx *mx, int size)
{
	mg_notify_mx_t *not = (typeof(not)) (arg + 1);
	if (!mx || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	mx->notify.events &= ~not->events;
	return (QR_DONE);
}
STATIC int
mg_not_clr_df(mg_notify_t * arg, struct df *df, int size)
{
	mg_notify_df_t *not = (typeof(not)) (arg + 1);
	if (!df || (size -= sizeof(*not)) < 0)
		return (-EINVAL);
	df->notify.events &= ~not->events;
	return (QR_DONE);
}

/*
 *  BLO Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_mgmt_blo_mg(mg_mgmt_t * arg, struct mg *mg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_blo_se(mg_mgmt_t * arg, struct se *se)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_blo_lg(mg_mgmt_t * arg, struct lg *lg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_blo_ch(mg_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_blo_mx(mg_mgmt_t * arg, struct mx *mx)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_blo_df(mg_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  UBL Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_mgmt_ubl_mg(mg_mgmt_t * arg, struct mg *mg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_ubl_se(mg_mgmt_t * arg, struct se *se)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_ubl_lg(mg_mgmt_t * arg, struct lg *lg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_ubl_ch(mg_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_ubl_mx(mg_mgmt_t * arg, struct mx *mx)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_ubl_df(mg_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  RES Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_mgmt_res_mg(mg_mgmt_t * arg, struct mg *mg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_res_se(mg_mgmt_t * arg, struct se *se)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_res_lg(mg_mgmt_t * arg, struct lg *lg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_res_ch(mg_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_res_mx(mg_mgmt_t * arg, struct mx *mx)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_res_df(mg_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  QRY Management
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
mg_mgmt_qry_mg(mg_mgmt_t * arg, struct mg *mg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_qry_se(mg_mgmt_t * arg, struct se *se)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_qry_lg(mg_mgmt_t * arg, struct lg *lg)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_qry_ch(mg_mgmt_t * arg, struct ch *ch)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_qry_mx(mg_mgmt_t * arg, struct mx *mx)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}
STATIC int
mg_mgmt_qry_df(mg_mgmt_t * arg, struct df *df)
{
	todo(("Write this function\n"));
	return (-EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IOCTL Interface
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MG_IOCGOPTIONS
 *  -----------------------------------
 *  Get configuration otpions by object type and id.
 */
STATIC int
mg_iocgoptions(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mg_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case MG_OBJ_TYPE_MG:
				return mg_opt_get_mg(arg, mg_lookup(arg->id), size);
			case MG_OBJ_TYPE_SE:
				return mg_opt_get_se(arg, se_lookup(arg->id, mg), size);
			case MG_OBJ_TYPE_LG:
				return mg_opt_get_lg(arg, lg_lookup(arg->id, se), size);
			case MG_OBJ_TYPE_CH:
				return mg_opt_get_ch(arg, ch_lookup(arg->id), size);
			case MG_OBJ_TYPE_MX:
				return mg_opt_get_mx(arg, mx_lookup(arg->id), size);
			case MG_OBJ_TYPE_DF:
				return mg_opt_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCSOPTIONS
 *  -----------------------------------
 *  Set configuration otpions by object type and id.
 */
STATIC int
mg_iocsoptions(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mg_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->type) {
			case MG_OBJ_TYPE_MG:
				return mg_opt_set_mg(arg, mg_lookup(arg->id), size);
			case MG_OBJ_TYPE_SE:
				return mg_opt_set_se(arg, se_lookup(arg->id, mg), size);
			case MG_OBJ_TYPE_LG:
				return mg_opt_set_lg(arg, lg_lookup(arg->id, se), size);
			case MG_OBJ_TYPE_CH:
				return mg_opt_set_ch(arg, ch_lookup(arg->id), size);
			case MG_OBJ_TYPE_MX:
				return mg_opt_set_mx(arg, mx_lookup(arg->id), size);
			case MG_OBJ_TYPE_DF:
				return mg_opt_set_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCGCONFIG
 *  -----------------------------------
 *  Get configuration by object type and id.  Also gets the configuration of
 *  as many children of the object as will fit in the provided buffer.
 */
STATIC int
mg_iocgconfig(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mg_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			switch (arg->type) {
			case MG_OBJ_TYPE_MG:
				return mg_get_mg(arg, mg_lookup(arg->id), size);
			case MG_OBJ_TYPE_SE:
				return mg_get_se(arg, se_lookup(arg->id, mg), size);
			case MG_OBJ_TYPE_LG:
				return mg_get_lg(arg, lg_lookup(arg->id, se), size);
			case MG_OBJ_TYPE_CH:
				return mg_get_ch(arg, ch_lookup(arg->id), size);
			case MG_OBJ_TYPE_MX:
				return mg_get_mx(arg, mx_lookup(arg->id), size);
			case MG_OBJ_TYPE_DF:
				return mg_get_df(arg, &master, size);
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCSCONFIG
 *  -----------------------------------
 *  Sets the configuration by object type, object id, and command.
 */
STATIC int
mg_iocsconfig(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mg_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case MG_ADD:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_add_mg(arg, mg_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_SE:
					return mg_add_se(arg, se_lookup(arg->id, mg), size, 0, 0);
				case MG_OBJ_TYPE_LG:
					return mg_add_lg(arg, lg_lookup(arg->id, se), size, 0, 0);
				case MG_OBJ_TYPE_CH:
					return mg_add_ch(arg, ch_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_MX:
					return mg_add_mx(arg, mx_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_DF:
					return mg_add_df(arg, &master, size, 0, 0);
				}
				break;
			case MG_CHA:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_cha_mg(arg, mg_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_SE:
					return mg_cha_se(arg, se_lookup(arg->id, mg), size, 0, 0);
				case MG_OBJ_TYPE_LG:
					return mg_cha_lg(arg, lg_lookup(arg->id, se), size, 0, 0);
				case MG_OBJ_TYPE_CH:
					return mg_cha_ch(arg, ch_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_MX:
					return mg_cha_mx(arg, mx_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_DF:
					return mg_cha_df(arg, &master, size, 0, 0);
				}
				break;
			case MG_DEL:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_del_mg(arg, mg_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_SE:
					return mg_del_se(arg, se_lookup(arg->id, mg), size, 0, 0);
				case MG_OBJ_TYPE_LG:
					return mg_del_lg(arg, lg_lookup(arg->id, se), size, 0, 0);
				case MG_OBJ_TYPE_CH:
					return mg_del_ch(arg, ch_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_MX:
					return mg_del_mx(arg, mx_lookup(arg->id), size, 0, 0);
				case MG_OBJ_TYPE_DF:
					return mg_del_df(arg, &master, size, 0, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCTCONFIG
 *  -----------------------------------
 *  Test configuraiton by object type and id.
 */
STATIC int
mg_ioctconfig(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mg_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case MG_ADD:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_add_mg(arg, mg_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_SE:
					return mg_add_se(arg, se_lookup(arg->id, mg), size, 0, 1);
				case MG_OBJ_TYPE_LG:
					return mg_add_lg(arg, lg_lookup(arg->id, se), size, 0, 1);
				case MG_OBJ_TYPE_CH:
					return mg_add_ch(arg, ch_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_MX:
					return mg_add_mx(arg, mx_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_DF:
					return mg_add_df(arg, &master, size, 0, 1);
				}
				break;
			case MG_CHA:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_cha_mg(arg, mg_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_SE:
					return mg_cha_se(arg, se_lookup(arg->id, mg), size, 0, 1);
				case MG_OBJ_TYPE_LG:
					return mg_cha_lg(arg, lg_lookup(arg->id, se), size, 0, 1);
				case MG_OBJ_TYPE_CH:
					return mg_cha_ch(arg, ch_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_MX:
					return mg_cha_mx(arg, mx_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_DF:
					return mg_cha_df(arg, &master, size, 0, 1);
				}
				break;
			case MG_DEL:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_del_mg(arg, mg_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_SE:
					return mg_del_se(arg, se_lookup(arg->id, mg), size, 0, 1);
				case MG_OBJ_TYPE_LG:
					return mg_del_lg(arg, lg_lookup(arg->id, se), size, 0, 1);
				case MG_OBJ_TYPE_CH:
					return mg_del_ch(arg, ch_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_MX:
					return mg_del_mx(arg, mx_lookup(arg->id), size, 0, 1);
				case MG_OBJ_TYPE_DF:
					return mg_del_df(arg, &master, size, 0, 1);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCCCONFIG
 *  -----------------------------------
 *  Commit configuration by object type and id.
 */
STATIC int
mg_ioccconfig(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		int size = msgdsize(mp);
		mg_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) >= 0)
			switch (arg->cmd) {
			case MG_ADD:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_add_mg(arg, mg_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_SE:
					return mg_add_se(arg, se_lookup(arg->id, mg), size, 1, 0);
				case MG_OBJ_TYPE_LG:
					return mg_add_lg(arg, lg_lookup(arg->id, se), size, 1, 0);
				case MG_OBJ_TYPE_CH:
					return mg_add_ch(arg, ch_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_MX:
					return mg_add_mx(arg, mx_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_DF:
					return mg_add_df(arg, &master, size, 1, 0);
				}
				break;
			case MG_CHA:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_cha_mg(arg, mg_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_SE:
					return mg_cha_se(arg, se_lookup(arg->id, mg), size, 1, 0);
				case MG_OBJ_TYPE_LG:
					return mg_cha_lg(arg, lg_lookup(arg->id, se), size, 1, 0);
				case MG_OBJ_TYPE_CH:
					return mg_cha_ch(arg, ch_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_MX:
					return mg_cha_mx(arg, mx_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_DF:
					return mg_cha_df(arg, &master, size, 1, 0);
				}
				break;
			case MG_DEL:
				switch (arg->type) {
				case MG_OBJ_TYPE_MG:
					return mg_del_mg(arg, mg_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_SE:
					return mg_del_se(arg, se_lookup(arg->id, mg), size, 1, 0);
				case MG_OBJ_TYPE_LG:
					return mg_del_lg(arg, lg_lookup(arg->id, se), size, 1, 0);
				case MG_OBJ_TYPE_CH:
					return mg_del_ch(arg, ch_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_MX:
					return mg_del_mx(arg, mx_lookup(arg->id), size, 1, 0);
				case MG_OBJ_TYPE_DF:
					return mg_del_df(arg, &master, size, 1, 0);
				}
				break;
			}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCGSTATEM
 *  -----------------------------------
 */
STATIC int
mg_iocgstatem(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_sta_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_sta_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_sta_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_sta_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_sta_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_sta_df(arg, &master, size);
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
 *  MG_IOCCMRESET
 *  -----------------------------------
 */
STATIC int
mg_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		mg_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		(void) arg;
		return (-EOPNOTSUPP);
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCGSTATSP
 *  -----------------------------------
 */
STATIC int
mg_iocgstatsp(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_statp_get_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_statp_get_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_statp_get_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_statp_get_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_statp_get_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_statp_get_df(arg, df_lookup(arg->id), size);
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
 *  MG_IOCSSTATSP
 *  -----------------------------------
 */
STATIC int
mg_iocsstatsp(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_statp_set_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_statp_set_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_statp_set_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_statp_set_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_statp_set_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_statp_set_df(arg, df_lookup(arg->id), size);
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
 *  MG_IOCGSTATS
 *  -----------------------------------
 */
STATIC int
mg_iocgstats(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_stat_get_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_stat_get_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_stat_get_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_stat_get_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_stat_get_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_stat_get_df(arg, df_lookup(arg->id), size);
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
 *  MG_IOCCSTATS
 *  -----------------------------------
 */
STATIC int
mg_ioccstats(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_stat_clr_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_stat_clr_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_stat_clr_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_stat_clr_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_stat_clr_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_stat_clr_df(arg, df_lookup(arg->id), size);
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
 *  MG_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC int
mg_iocgnotify(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_not_get_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_not_get_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_not_get_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_not_get_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_not_get_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_not_get_df(arg, df_lookup(arg->id), size);
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
 *  MG_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC int
mg_iocsnotify(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_not_set_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_not_set_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_not_set_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_not_set_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_not_set_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_not_set_df(arg, df_lookup(arg->id), size);
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
 *  MG_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC int
mg_ioccnotify(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		psw_t flags;
		int ret = QR_DONE;
		int size = msgdsize(mp);
		mg_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		if ((size -= sizeof(*arg)) < 0)
			return (-EMSGSIZE);
		spin_lock_irqsave(&master.lock, flags);
		switch (arg->type) {
		case MG_OBJ_TYPE_MG:
			ret = mg_not_clr_mg(arg, mg_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_SE:
			ret = mg_not_clr_se(arg, se_lookup(arg->id, mg), size);
			break;
		case MG_OBJ_TYPE_LG:
			ret = mg_not_clr_lg(arg, lg_lookup(arg->id, se), size);
			break;
		case MG_OBJ_TYPE_CH:
			ret = mg_not_clr_ch(arg, ch_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_MX:
			ret = mg_not_clr_mx(arg, mx_lookup(arg->id), size);
			break;
		case MG_OBJ_TYPE_DF:
			ret = mg_not_clr_df(arg, df_lookup(arg->id), size);
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
 *  MG_IOCCMGMT
 *  -----------------------------------
 *  Manage objects.
 */
STATIC int
mg_ioccmgmt(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct se *se = mg->se.list;
	if (mp->b_cont) {
		mg_mgmt_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		switch (arg->cmd) {
		case MG_MGMT_BLOCK:
			switch (arg->type) {
			case MG_OBJ_TYPE_MG:
				return mg_mgmt_blo_mg(arg, mg_lookup(arg->id));
			case MG_OBJ_TYPE_SE:
				return mg_mgmt_blo_se(arg, se_lookup(arg->id, mg));
			case MG_OBJ_TYPE_LG:
				return mg_mgmt_blo_lg(arg, lg_lookup(arg->id, se));
			case MG_OBJ_TYPE_CH:
				return mg_mgmt_blo_ch(arg, ch_lookup(arg->id));
			case MG_OBJ_TYPE_MX:
				return mg_mgmt_blo_mx(arg, mx_lookup(arg->id));
			case MG_OBJ_TYPE_DF:
				return mg_mgmt_blo_df(arg, df_lookup(arg->id));
			}
			break;
		case MG_MGMT_UNBLOCK:
			switch (arg->type) {
			case MG_OBJ_TYPE_MG:
				return mg_mgmt_ubl_mg(arg, mg_lookup(arg->id));
			case MG_OBJ_TYPE_SE:
				return mg_mgmt_ubl_se(arg, se_lookup(arg->id, mg));
			case MG_OBJ_TYPE_LG:
				return mg_mgmt_ubl_lg(arg, lg_lookup(arg->id, se));
			case MG_OBJ_TYPE_CH:
				return mg_mgmt_ubl_ch(arg, ch_lookup(arg->id));
			case MG_OBJ_TYPE_MX:
				return mg_mgmt_ubl_mx(arg, mx_lookup(arg->id));
			case MG_OBJ_TYPE_DF:
				return mg_mgmt_ubl_df(arg, df_lookup(arg->id));
			}
			break;
		case MG_MGMT_RESET:
			switch (arg->type) {
			case MG_OBJ_TYPE_MG:
				return mg_mgmt_res_mg(arg, mg_lookup(arg->id));
			case MG_OBJ_TYPE_SE:
				return mg_mgmt_res_se(arg, se_lookup(arg->id, mg));
			case MG_OBJ_TYPE_LG:
				return mg_mgmt_res_lg(arg, lg_lookup(arg->id, se));
			case MG_OBJ_TYPE_CH:
				return mg_mgmt_res_ch(arg, ch_lookup(arg->id));
			case MG_OBJ_TYPE_MX:
				return mg_mgmt_res_mx(arg, mx_lookup(arg->id));
			case MG_OBJ_TYPE_DF:
				return mg_mgmt_res_df(arg, df_lookup(arg->id));
			}
			break;
		case MG_MGMT_QUERY:
			switch (arg->type) {
			case MG_OBJ_TYPE_MG:
				return mg_mgmt_qry_mg(arg, mg_lookup(arg->id));
			case MG_OBJ_TYPE_SE:
				return mg_mgmt_qry_se(arg, se_lookup(arg->id, mg));
			case MG_OBJ_TYPE_LG:
				return mg_mgmt_qry_lg(arg, lg_lookup(arg->id, se));
			case MG_OBJ_TYPE_CH:
				return mg_mgmt_qry_ch(arg, ch_lookup(arg->id));
			case MG_OBJ_TYPE_MX:
				return mg_mgmt_qry_mx(arg, mx_lookup(arg->id));
			case MG_OBJ_TYPE_DF:
				return mg_mgmt_qry_df(arg, df_lookup(arg->id));
			}
			break;
		}
	}
	rare();
	return (-EINVAL);
}

/*
 *  MG_IOCCPASS
 *  -----------------------------------
 *  Pass a message to the lower layer.
 */
STATIC int
mg_ioccpass(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		mg_pass_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		mblk_t *bp, *dp;
		struct mx *mx;
		for (mx = master.mx.list; mx && mx->u.mux.index != arg->muxid; mx = mx->next) ;
		if (!mx || !mx->oq)
			return (-EINVAL);
		if (arg->type < QPCTL && !canput(mx->oq))
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
		ss7_oput(mx->oq, bp);
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
mg_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mg *mg = MG_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	switch (type) {
	case __SID:
	{
		struct mx *mx;
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
					mg));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			if ((mx =
			     mx_alloc_link(lb->l_qbot, &master.mx.list, lb->l_index, iocp->ioc_cr)))
				break;
			ret = -ENOMEM;
			break;
		case _IOC_NR(I_PUNLINK):
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					mg));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			for (mx = master.mx.list; mx; mx = mx->next)
				if (mx->u.mux.index == lb->l_index)
					break;
			if (!mx) {
				ret = -EINVAL;
				ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n", DRV_NAME,
					mg));
				break;
			}
			mx_free_link(mx);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", DRV_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case MG_IOC_MAGIC:
	{
		/* 
		   These are MG IOCTLs. */
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(MG_IOCGOPTIONS):
			printd(("%s: %p: -> MG_IOCGOPTIONS\n", DRV_NAME, mg));
			ret = mg_iocgoptions(q, mp);
			break;
		case _IOC_NR(MG_IOCSOPTIONS):
			printd(("%s: %p: -> MG_IOCSOPTIONS\n", DRV_NAME, mg));
			ret = mg_iocsoptions(q, mp);
			break;
		case _IOC_NR(MG_IOCGCONFIG):
			printd(("%s: %p: -> MG_IOCGCONFIG\n", DRV_NAME, mg));
			ret = mg_iocgconfig(q, mp);
			break;
		case _IOC_NR(MG_IOCSCONFIG):
			printd(("%s: %p: -> MG_IOCSCONFIG\n", DRV_NAME, mg));
			ret = mg_iocsconfig(q, mp);
			break;
		case _IOC_NR(MG_IOCTCONFIG):
			printd(("%s: %p: -> MG_IOCTCONFIG\n", DRV_NAME, mg));
			ret = mg_ioctconfig(q, mp);
			break;
		case _IOC_NR(MG_IOCCCONFIG):
			printd(("%s: %p: -> MG_IOCCCONFIG\n", DRV_NAME, mg));
			ret = mg_ioccconfig(q, mp);
			break;
		case _IOC_NR(MG_IOCGSTATEM):
			printd(("%s: %p: -> MG_IOCGSTATEM\n", DRV_NAME, mg));
			ret = mg_iocgstatem(q, mp);
			break;
		case _IOC_NR(MG_IOCCMRESET):
			printd(("%s: %p: -> MG_IOCCMRESET\n", DRV_NAME, mg));
			ret = mg_ioccmreset(q, mp);
			break;
		case _IOC_NR(MG_IOCGSTATSP):
			printd(("%s: %p: -> MG_IOCGSTATSP\n", DRV_NAME, mg));
			ret = mg_iocgstatsp(q, mp);
			break;
		case _IOC_NR(MG_IOCSSTATSP):
			printd(("%s: %p: -> MG_IOCSSTATSP\n", DRV_NAME, mg));
			ret = mg_iocsstatsp(q, mp);
			break;
		case _IOC_NR(MG_IOCGSTATS):
			printd(("%s: %p: -> MG_IOCGSTATS\n", DRV_NAME, mg));
			ret = mg_iocgstats(q, mp);
			break;
		case _IOC_NR(MG_IOCCSTATS):
			printd(("%s: %p: -> MG_IOCCSTATS\n", DRV_NAME, mg));
			ret = mg_ioccstats(q, mp);
			break;
		case _IOC_NR(MG_IOCGNOTIFY):
			printd(("%s: %p: -> MG_IOCGNOTIFY\n", DRV_NAME, mg));
			ret = mg_iocgnotify(q, mp);
			break;
		case _IOC_NR(MG_IOCSNOTIFY):
			printd(("%s: %p: -> MG_IOCSNOTIFY\n", DRV_NAME, mg));
			ret = mg_iocsnotify(q, mp);
			break;
		case _IOC_NR(MG_IOCCNOTIFY):
			printd(("%s: %p: -> MG_IOCCNOTIFY\n", DRV_NAME, mg));
			ret = mg_ioccnotify(q, mp);
			break;
		case _IOC_NR(MG_IOCCMGMT):
			printd(("%s: %p: -> MG_IOCCMGMT\n", DRV_NAME, mg));
			ret = mg_ioccmgmt(q, mp);
			break;
		case _IOC_NR(MG_IOCCPASS):
			printd(("%s: %p: -> MG_IOCCPASS\n", DRV_NAME, mg));
			ret = mg_ioccpass(q, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported MG ioctl %c, %d\n", DRV_NAME, mg,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case MX_IOC_MAGIC:
	{
		struct mx *mx;
		ulong mxid;
		/* 
		   These are MX IOCTLs. They differ from normal MX IOCTLs in the fact that they
		   have a Multiplex Identifier as a discriminator in the first long in the
		   argument. This is used to find the lower stream and is stripped and then the
		   IOCTL is passed to the lower stream.  We intercept the ACK or NAK from the lower 
		   stream and place the discrimintor back on the argument before passing it back
		   up. */
		ret = -EINVAL;
		if (count < size)
			break;
		mxid = *((ulong *) mp->b_cont->b_rptr);
		for (mx = master.mx.list; mx && mx->id != mxid; mx = mx->next) ;
		if (!mx || !mx->oq)
			break;
		*(ulong *) mp->b_cont->b_rptr++ = mg->u.mux.index;
		iocp->ioc_count -= sizeof(ulong);
		ss7_oput(mx->oq, mp);
		ret = QR_ABSORBED;
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
 *  M_IOCACK, M_IOCNAK Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mx_r_iocack(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	struct mg *mg;
	ulong mgid;
	(void) iocp;
	if (!mp->b_cont)
		goto efault;
	mgid = *(ulong *) --mp->b_cont->b_rptr;
	*((ulong *) mp->b_cont->b_rptr) = mx->id;
	for (mg = master.mg.list; mg && mg->u.mux.index != mgid; mg = mg->next) ;
	if (!mg)
		goto efault;
	if (!mg->oq)
		goto discard;
	ss7_oput(mg->oq, mp);
	return (QR_ABSORBED);
      efault:
	swerr();
	return (-EFAULT);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Primitives from MGC to MG.
 *  -----------------------------------
 */
STATIC int
mg_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct mg *mg = MG_PRIV(q);
	ulong prim;
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case MG_INFO_REQ:
		printd(("%s: %p: -> MG_INFO_REQ\n", DRV_NAME, mg));
		rtn = mg_info_req(q, mp);
		break;
	case MG_OPTMGMT_REQ:
		printd(("%s: %p: -> MG_OPTMGMT_REQ\n", DRV_NAME, mg));
		rtn = mg_optmgmt_req(q, mp);
		break;
	case MG_ATTACH_REQ:
		printd(("%s: %p: -> MG_ATTACH_REQ\n", DRV_NAME, mg));
		rtn = mg_attach_req(q, mp);
		break;
	case MG_DETACH_REQ:
		printd(("%s: %p: -> MG_DETACH_REQ\n", DRV_NAME, mg));
		rtn = mg_detach_req(q, mp);
		break;
	case MG_JOIN_REQ:
		printd(("%s: %p: -> MG_JOIN_REQ\n", DRV_NAME, mg));
		rtn = mg_join_req(q, mp);
		break;
	case MG_ACTION_REQ:
		printd(("%s: %p: -> MG_ACTION_REQ\n", DRV_NAME, mg));
		rtn = mg_action_req(q, mp);
		break;
	case MG_ABORT_REQ:
		printd(("%s: %p: -> MG_ABORT_REQ\n", DRV_NAME, mg));
		rtn = mg_abort_req(q, mp);
		break;
	case MG_CONN_REQ:
		printd(("%s: %p: -> MG_CONN_REQ\n", DRV_NAME, mg));
		rtn = mg_conn_req(q, mp);
		break;
	case MG_DATA_REQ:
		printd(("%s: %p: -> MG_DATA_REQ\n", DRV_NAME, mg));
		rtn = mg_data_req(q, mp);
		break;
	case MG_DISCON_REQ:
		printd(("%s: %p: -> MG_DISCON_REQ\n", DRV_NAME, mg));
		rtn = mg_discon_req(q, mp);
		break;
	case MG_LEAVE_REQ:
		printd(("%s: %p: -> MG_LEAVE_REQ\n", DRV_NAME, mg));
		rtn = mg_leave_req(q, mp);
		break;
	case MG_NOTIFY_REQ:
		printd(("%s: %p: -> MG_NOTIFY_REQ\n", DRV_NAME, mg));
		rtn = mg_notify_req(q, mp);
		break;
	default:
		printd(("%s: %p: -> MG_????\n", DRV_NAME, mg));
		rtn = mg_error_ack(q, mg, prim, MGNOTSUPP);
		break;
	}
	return (rtn);
}

/*
 *  Primitives from MX to MG.
 *  -----------------------------------
 */
STATIC int
mx_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct mx *mx = MX_PRIV(q);
	(void) mx;
	switch (*((ulong *) mp->b_rptr)) {
	case MX_INFO_ACK:
		printd(("%s: %p: MX_INFO_ACK <-\n", DRV_NAME, mx));
		rtn = mx_info_ack(q, mp);
		break;
	case MX_OK_ACK:
		printd(("%s: %p: MX_OK_ACK <-\n", DRV_NAME, mx));
		rtn = mx_ok_ack(q, mp);
		break;
	case MX_ERROR_ACK:
		printd(("%s: %p: MX_ERROR_ACK <-\n", DRV_NAME, mx));
		rtn = mx_error_ack(q, mp);
		break;
	case MX_ENABLE_CON:
		printd(("%s: %p: MX_ENABLE_CON <-\n", DRV_NAME, mx));
		rtn = mx_enable_con(q, mp);
		break;
	case MX_CONNECT_CON:
		printd(("%s: %p: MX_CONNECT_CON <-\n", DRV_NAME, mx));
		rtn = mx_connect_con(q, mp);
		break;
	case MX_DATA_IND:
		printd(("%s: %p: MX_DATA_IND <-\n", DRV_NAME, mx));
		rtn = mx_data_ind(q, mp);
		break;
	case MX_DISCONNECT_IND:
		printd(("%s: %p: MX_DISCONNECT_IND <-\n", DRV_NAME, mx));
		rtn = mx_disconnect_ind(q, mp);
		break;
	case MX_DISCONNECT_CON:
		printd(("%s: %p: MX_DISCONNECT_CON <-\n", DRV_NAME, mx));
		rtn = mx_disconnect_con(q, mp);
		break;
	case MX_DISABLE_IND:
		printd(("%s: %p: MX_DISABLE_IND <-\n", DRV_NAME, mx));
		rtn = mx_disable_ind(q, mp);
		break;
	case MX_DISABLE_CON:
		printd(("%s: %p: MX_DISABLE_CON <-\n", DRV_NAME, mx));
		rtn = mx_disable_con(q, mp);
		break;
	default:
		printd(("%s: %p: ???? %lu <-\n", DRV_NAME, mx, *(ulong *) mp->b_rptr));
		rtn = -EFAULT;
		break;
	}
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
mg_w_data(queue_t *q, mblk_t *mp)
{
	/* 
	   data from above */
	printd(("%s: %p: -> M_DATA\n", DRV_NAME, MG_PRIV(q)));
	return mg_write(q, mp);
}
STATIC int
mx_r_data(queue_t *q, mblk_t *mp)
{
	/* 
	   data from below */
	printd(("%s: %p: M_DATA <-\n", DRV_NAME, MX_PRIV(q)));
	return mx_read(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mx_r_error(queue_t *q, mblk_t *mp)
{
	fixme(("Unimmplemented function\n"));
	return (-EFAULT);
}
STATIC int
mx_r_hangup(queue_t *q, mblk_t *mp)
{
	fixme(("Unimmplemented function\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC inline int
mg_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC inline int
mg_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mg_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mg_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mg_w_proto(q, mp);
	case M_IOCTL:
		return mg_w_ioctl(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC inline int
mx_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mx_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mx_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_r_proto(q, mp);
	case M_ERROR:
		return mx_r_error(q, mp);
	case M_HANGUP:
		return mx_r_hangup(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return mx_r_iocack(q, mp);
	}
	seldom();
	return (QR_PASSFLOW);
}
STATIC inline int
mx_w_prim(queue_t *q, mblk_t *mp)
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
STATIC major_t mg_majors[MG_CMAJORS] = { MG_CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
mg_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct mg *mg, **mgp = &master.mg.list;
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
	if (cmajor != MG_CMAJOR_0 || cminor >= 2) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	spin_lock_irqsave(&master.lock, flags);
	for (; *mgp; mgp = (typeof(mgp)) & (*mgp)->next) {
		major_t dmajor = (*mgp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*mgp)->u.dev.cminor;
			if (cminor < dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= MG_CMAJORS || !(cmajor = mg_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= MG_CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(mg = mg_alloc_priv(q, mgp, devp, crp))) {
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
STATIC streamscall int
mg_close(queue_t *q, int flag, cred_t *crp)
{
	struct mg *mg = MG_PRIV(q);
	(void) flag;
	(void) crp;
	(void) mg;
	printd(("%s: %p: closing character device %d:%d\n", DRV_NAME, mg, mg->u.dev.cmajor,
		mg->u.dev.cminor));
	mg_free_priv(mg);
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
STATIC kmem_cache_t *mg_priv_cachep = NULL;
STATIC kmem_cache_t *se_priv_cachep = NULL;
STATIC kmem_cache_t *lg_priv_cachep = NULL;
STATIC kmem_cache_t *cn_priv_cachep = NULL;
STATIC kmem_cache_t *ac_priv_cachep = NULL;
STATIC kmem_cache_t *ch_priv_cachep = NULL;
STATIC kmem_cache_t *mx_priv_cachep = NULL;

STATIC int
mg_init_caches(void)
{
	if (!mg_priv_cachep
	    && !(mg_priv_cachep =
		 kmem_cache_create("mg_priv_cachep", sizeof(struct mg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mg_priv_cachep", DRV_NAME);
		goto no_mg;
	} else
		printd(("%s: initialized mg private structure cache\n", DRV_NAME));
	if (!se_priv_cachep
	    && !(se_priv_cachep =
		 kmem_cache_create("se_priv_cachep", sizeof(struct se), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate se_priv_cachep", DRV_NAME);
		goto no_se;
	} else
		printd(("%s: initialized se private structure cache\n", DRV_NAME));
	if (!lg_priv_cachep
	    && !(lg_priv_cachep =
		 kmem_cache_create("lg_priv_cachep", sizeof(struct lg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate lg_priv_cachep", DRV_NAME);
		goto no_lg;
	} else
		printd(("%s: initialized lg private structure cache\n", DRV_NAME));
	if (!cn_priv_cachep
	    && !(cn_priv_cachep =
		 kmem_cache_create("cn_priv_cachep", sizeof(struct cn), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate cn_priv_cachep", DRV_NAME);
		goto no_cn;
	} else
		printd(("%s: initialized cn private structure cache\n", DRV_NAME));
	if (!ac_priv_cachep
	    && !(ac_priv_cachep =
		 kmem_cache_create("ac_priv_cachep", sizeof(struct ac), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ac_priv_cachep", DRV_NAME);
		goto no_ac;
	} else
		printd(("%s: initialized ac private structure cache\n", DRV_NAME));
	if (!ch_priv_cachep
	    && !(ch_priv_cachep =
		 kmem_cache_create("ch_priv_cachep", sizeof(struct ch), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ch_priv_cachep", DRV_NAME);
		goto no_ch;
	} else
		printd(("%s: initialized ch private structure cache\n", DRV_NAME));
	if (!mx_priv_cachep
	    && !(mx_priv_cachep =
		 kmem_cache_create("mx_priv_cachep", sizeof(struct mx), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mx_priv_cachep", DRV_NAME);
		goto no_ch;
	} else
		printd(("%s: initialized mx private structure cache\n", DRV_NAME));
	return (0);
	// no_mx:
	// kmem_cache_destroy(ch_priv_cachep);
      no_ch:
	kmem_cache_destroy(ac_priv_cachep);
      no_ac:
	kmem_cache_destroy(cn_priv_cachep);
      no_cn:
	kmem_cache_destroy(lg_priv_cachep);
      no_lg:
	kmem_cache_destroy(se_priv_cachep);
      no_se:
	kmem_cache_destroy(mg_priv_cachep);
      no_mg:
	return (-ENOMEM);
}

STATIC int
mg_term_caches(void)
{
	int err = 0;
	if (mg_priv_cachep) {
		if (kmem_cache_destroy(mg_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mg_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mg_priv_cachep\n", DRV_NAME));
	}
	if (se_priv_cachep) {
		if (kmem_cache_destroy(se_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy se_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed se_priv_cachep\n", DRV_NAME));
	}
	if (lg_priv_cachep) {
		if (kmem_cache_destroy(lg_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy lg_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed lg_priv_cachep\n", DRV_NAME));
	}
	if (cn_priv_cachep) {
		if (kmem_cache_destroy(cn_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy cn_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed cn_priv_cachep\n", DRV_NAME));
	}
	if (ac_priv_cachep) {
		if (kmem_cache_destroy(ac_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ac_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ac_priv_cachep\n", DRV_NAME));
	}
	if (ch_priv_cachep) {
		if (kmem_cache_destroy(ch_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ch_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ch_priv_cachep\n", DRV_NAME));
	}
	if (mx_priv_cachep) {
		if (kmem_cache_destroy(mx_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mx_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mx_priv_cachep\n", DRV_NAME));
	}
	return (err);
}

/*
 *  MG allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct mg *
mg_alloc_priv(queue_t *q, struct mg **mgp, dev_t *devp, cred_t *crp)
{
	struct mg *mg;
	if ((mg = kmem_cache_alloc(mg_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated mg private structure %d:%d\n", DRV_NAME, mg,
			getmajor(*devp), getminor(*devp)));
		bzero(mg, sizeof(*mg));
		mg->u.dev.cmajor = getmajor(*devp);
		mg->u.dev.cminor = getminor(*devp);
		mg->cred = *crp;
		spin_lock_init(&mg->qlock);	/* "mg-queue-lock" */
		(mg->iq = WR(q))->q_ptr = mg_get(mg);
		(mg->oq = RD(q))->q_ptr = mg_get(mg);
		mg->o_prim = mg_r_prim;
		mg->i_prim = mg_w_prim;
		mg->o_wakeup = mg_r_wakeup;
		mg->i_wakeup = NULL;
		mg->i_state = MGS_DETACHED;
		mg->i_style = LMI_STYLE1;
		mg->i_version = 1;
		spin_lock_init(&mg->lock);	/* "mg-priv-lock" */
		/* 
		   place in master list */
		if ((mg->next = *mgp))
			mg->next->prev = &mg->next;
		mg->prev = mgp;
		*mgp = mg_get(mg);
		master.mg.numb++;
	} else
		ptrace(("%s: ERROR: Could not allocate mg private structure %d:%d\n", DRV_NAME,
			getmajor(*devp), getminor(*devp)));
	return (mg);
}
STATIC void
mg_free_priv(struct mg *mg)
{
	psw_t flags;
	ensure(mg, return);
	spin_lock_irqsave(&mg->lock, flags);
	{
		struct ch *ch;
		struct se *se;
		ss7_unbufcall((str_t *) mg);
		flushq(mg->oq, FLUSHALL);
		flushq(mg->iq, FLUSHALL);
		// mg_timer_stop(mg, tall);
		/* 
		   free all channels */
		while ((ch = mg->ch.list))
			ch_free_priv(ch);
		/* 
		   free all sessions */
		while ((se = mg->se.list))
			se_free_priv(se);
		/* 
		   remove from master list */
		if ((*mg->prev = mg->next))
			mg->next->prev = mg->prev;
		mg->next = NULL;
		mg->prev = &mg->next;
		ensure(atomic_read(&mg->refcnt) > 1, mg_get(mg));
		mg_put(mg);
		ensure(master.mg.numb > 0, master.mg.numb++);
		master.mg.numb--;
		/* 
		   remove from queues */
		ensure(atomic_read(&mg->refcnt) > 1, mg_get(mg));
		mg_put((mg_t *) xchg(&(mg->iq->q_ptr), NULL));
		ensure(atomic_read(&mg->refcnt) > 1, mg_get(mg));
		mg_put((mg_t *) xchg(&(mg->oq->q_ptr), NULL));
		/* 
		   done, check final count */
		if (atomic_read(&mg->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: mg lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, mg, atomic_read(&mg->refcnt)));
			atomic_set(&mg->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&mg->lock, flags);
	mg_put(mg);		/* final put */
	return;
}
STATIC struct mg *
mg_lookup(ulong id)
{
	struct mg *mg = NULL;
	if (id)
		for (mg = master.mg.list; mg && mg->id != id; mg = mg->next) ;
	return (mg);
}
#if 0
STATIC ulong
mg_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		for (id = ++sequence; mg_lookup(id); id = ++sequence) ;
	return (id);
}
#endif
STATIC struct mg *
mg_get(struct mg *mg)
{
	atomic_inc(&mg->refcnt);
	return (mg);
}
STATIC void
mg_put(struct mg *mg)
{
	if (atomic_dec_and_test(&mg->refcnt)) {
		kmem_cache_free(mg_priv_cachep, mg);
		printd(("%s: %p: freed mg private structure\n", DRV_NAME, mg));
	}
}

/*
 *  SE allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct se *
se_alloc_priv(ulong id, struct mg *mg)
{
	struct se *se, **sep;
	if ((se = kmem_cache_alloc(se_priv_cachep, SLAB_ATOMIC))) {
		bzero(se, sizeof(*se));
		spin_lock_init(&se->lock);	/* "se-lock" */
		se_get(se);	/* first get */
		/* 
		   add to master list */
		if ((se->next = master.se.list))
			se->next->prev = &se->next;
		se->prev = &master.se.list;
		master.se.list = se_get(se);
		master.se.numb++;
		/* 
		   link to mg in id order */
		se->id = se_get_id(id);
		se->type = MG_OBJ_TYPE_SE;
		se->state = MGS_ATTACHED;
		for (sep = &mg->se.list; (*sep) && (*sep)->id < se->id; (*sep) = (*sep)->next) ;
		if ((se->mg.next = *sep))
			se->mg.next->mg.prev = &se->mg.next;
		se->mg.next = *sep;
		se->mg.prev = sep;
		se->mg.mg = mg_get(mg);
		*sep = se_get(se);
		mg->se.numb++;
		printd(("%s: %p: linked se structure %lu\n", DRV_NAME, se, se->id));
	} else
		ptrace(("%s: ERROR: Could not allocate se structure %lu\n", DRV_NAME, id));
	return (se);
}
STATIC void
se_free_priv(struct se *se)
{
	psw_t flags;
	ensure(se, return);
	spin_lock_irqsave(&se->lock, flags);
	{
		struct mg *mg;
		struct lg *lg;
		while ((lg = se->lg.list))
			lg_free_priv(lg);
		if ((mg = se->mg.mg)) {
			mg_put(xchg(&se->mg.mg, NULL));
			ensure(atomic_read(&se->refcnt) > 1, se_get(se));
			fixme(("Do more than this!\n"));
			se_put(xchg(&mg->se.list, NULL));
		}
		/* 
		   unlink from mg */
		if ((*se->mg.prev = se->mg.next))
			se->mg.next->mg.prev = se->mg.prev;
		se->mg.next = NULL;
		se->mg.prev = &se->mg.next;
		ensure(atomic_read(&se->refcnt) > 1, se_get(se));
		se_put(se);
		mg_put(xchg(&se->mg.mg, NULL));
		assure(mg->se.numb > 0);
		mg->se.numb--;
		/* 
		   remove from master list */
		if ((*se->prev = se->next))
			se->next->prev = se->prev;
		se->next = NULL;
		se->prev = &se->next;
		ensure(atomic_read(&se->refcnt) > 1, se_get(se));
		se_put(se);
		assure(master.se.numb > 0);
		master.se.numb--;
		/* 
		   done, check final count */
		if (atomic_read(&se->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: se lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, se, atomic_read(&se->refcnt)));
			atomic_set(&se->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&se->lock, flags);
	se_put(se);		/* final put */
	return;
}
STATIC struct se *
se_lookup(ulong id, struct mg *mg)
{
	struct se *se = NULL;
	if (id)
		for (se = mg->se.list; se && se->id != id; se = se->next) ;
	return (se);
}
STATIC ulong
se_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct se *
se_get(struct se *se)
{
	atomic_inc(&se->refcnt);
	return (se);
}
STATIC void
se_put(struct se *se)
{
	if (atomic_dec_and_test(&se->refcnt)) {
		kmem_cache_free(se_priv_cachep, se);
		printd(("%s: %p: freed se structure\n", DRV_NAME, se));
	}
}

/*
 *  LG allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct lg *
lg_alloc_priv(ulong id, struct se *se)
{
	struct lg *lg;
	if ((lg = kmem_cache_alloc(lg_priv_cachep, SLAB_ATOMIC))) {
		bzero(lg, sizeof(*lg));
		spin_lock_init(&lg->lock);	/* "lg-lock" */
		lg_get(lg);	/* first get */
		/* 
		   add to master list */
		if ((lg->next = master.lg.list))
			lg->next->prev = &lg->next;
		lg->prev = &master.lg.list;
		master.lg.list = lg_get(lg);
		master.lg.numb++;
		/* 
		   add to se list */
		lg->id = lg_get_id(id);
		lg->type = MG_OBJ_TYPE_LG;
		lg->state = MGS_JOINED;
		if ((lg->se.next = se->lg.list))
			lg->se.next->se.prev = &lg->se.next;
		lg->se.next = se->lg.list;
		lg->se.prev = &se->lg.list;
		lg->se.se = se_get(se);
		se->lg.list = lg_get(lg);
		se->lg.numb++;
		printd(("%s: %p: linked lg structure %lu\n", DRV_NAME, lg, lg->id));
	} else
		ptrace(("%s: ERROR: Could not allocate lg structure %lu\n", DRV_NAME, id));
	return (lg);
}
STATIC void
lg_free_priv(struct lg *lg)
{
	psw_t flags;
	ensure(lg, return);
	spin_lock_irqsave(&lg->lock, flags);
	{
		struct se *se;
		struct ch *ch;
		/* 
		   unlink channels */
		while ((ch = lg->ch.list)) {
			/* 
			   disconnect channel */
			while (ch->og.list)
				cn_free_priv(ch->og.list);
			while (ch->ic.list)
				cn_free_priv(ch->ic.list);
			while (ch->ac.list)
				ac_free_priv(ch->ac.list);
			/* 
			   unlink channel from lg */
			ch->lg_slot = -1UL;
			if ((*ch->lg.prev = ch->lg.next))
				ch->lg.next->lg.prev = ch->lg.prev;
			ch->lg.next = NULL;
			ch->lg.prev = &ch->lg.next;
			ch_put(ch);
			lg_put(xchg(&ch->lg.lg, NULL));
			lg->ch.numb--;
		}
		/* 
		   unlink from se */
		if ((se = lg->se.se)) {
			if ((*lg->se.prev = lg->se.next))
				lg->se.next->se.prev = lg->se.prev;
			lg->se.next = NULL;
			lg->se.prev = &lg->se.next;
			ensure(atomic_read(&lg->refcnt) > 1, lg_get(lg));
			lg_put(lg);
			assure(se->lg.numb > 0);
			se->lg.numb--;
			se_put(xchg(&lg->se.se, NULL));
			/* 
			   destroy se if we are the last leg */
			if (!(se->lg.list))
				se_free_priv(se);
		}
		/* 
		   remove from master list */
		if ((*lg->prev = lg->next))
			lg->next->prev = lg->prev;
		lg->next = NULL;
		lg->prev = &lg->next;
		lg->id = 0;
		/* 
		   done, check final count */
		if (atomic_read(&lg->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: lg lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, lg, atomic_read(&lg->refcnt)));
			atomic_set(&lg->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&lg->lock, flags);
	lg_put(lg);		/* final put */
}
STATIC struct lg *
lg_lookup(ulong id, struct se *se)
{
	struct lg *lg = NULL;
	if (id)
		for (lg = se->lg.list; lg && lg->id != id; lg = lg->next) ;
	return (lg);
}
STATIC ulong
lg_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct lg *
lg_get(struct lg *lg)
{
	atomic_inc(&lg->refcnt);
	return (lg);
}
STATIC void
lg_put(struct lg *lg)
{
	if (atomic_dec_and_test(&lg->refcnt)) {
		kmem_cache_free(lg_priv_cachep, lg);
		printd(("%s: %p: freed lg structure\n", DRV_NAME, lg));
	}
}

/*
 *  CN allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct cn *
cn_alloc_priv(struct ch *ic, struct ch *og, ulong pad)
{
	/* 
	   Form a connection between in an incoming and outgoing leg */
	struct cn *cn;
	if ((cn = kmem_cache_alloc(cn_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated cn private structure\n", DRV_NAME, cn));
		bzero(cn, sizeof(*cn));
		cn->state = MGS_WCON_CREQ;
		cn->flags = 0;
		cn->pad = pad;
		if (ic) {
			/* 
			   link to incoming ch */
			cn->ic.ch = ch_get(ic);
			if ((cn->ic.next = ic->ic.list))
				cn->ic.next->ic.prev = &cn->ic.next;
			cn->ic.prev = &ic->ic.list;
			ic->ic.list = cn;
			ic->ic.numb++;
		}
		if (og) {
			/* 
			   link to outgoing ch */
			cn->og.ch = ch_get(og);
			if ((cn->og.next = og->og.list))
				cn->og.next->og.prev = &cn->og.next;
			cn->og.prev = &og->og.list;
			og->og.list = cn;
			og->og.numb++;
		}
	} else
		ptrace(("%s: ERROR: Could not allocate cn structure\n", DRV_NAME));
	return (cn);
}
STATIC void
cn_free_priv(struct cn *cn)
{
	struct ch *ch;
	ensure(cn, return);
	/* 
	   free buffers */
	if (cn->buf) {
		cn->samples = 0;
		freemsg(xchg(&cn->buf, NULL));
	}
	/* 
	   unlink from ic */
	if ((ch = cn->ic.ch)) {
		if ((*cn->ic.prev = cn->ic.next))
			cn->ic.next->ic.prev = cn->ic.prev;
		cn->ic.next = NULL;
		cn->ic.prev = &cn->ic.next;
		ch->ic.numb--;
		ch_put(xchg(&cn->ic.ch, NULL));
	}
	/* 
	   unlink from og */
	if ((ch = cn->og.ch)) {
		if ((*cn->og.prev = cn->og.next))
			cn->og.next->og.prev = cn->og.prev;
		cn->og.next = NULL;
		cn->og.prev = &cn->og.next;
		ch->og.numb--;
		ch_put(xchg(&cn->og.ch, NULL));
	}
	kmem_cache_free(cn_priv_cachep, cn);
	printd(("%s: %p: freed cn private structure\n", DRV_NAME, cn));
}

/*
 *  AC allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct ac *
ac_alloc_priv(ulong id, ulong type, ulong flags, ulong duration, struct ch *ch, mblk_t *dp)
{
	struct ac *ac;
	if ((ac = kmem_cache_alloc(ac_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated ac private structure\n", DRV_NAME, ac));
		bzero(ac, sizeof(*ac));
		ac->id = ac_get_id(id);
		ac->type = type;
		ac->flags = flags;
		ac->duration = duration;
		if (dp) {
			ac->pos = 0;
			ac->buf = dp;
		} else {
			ac->pos = -1UL;
			ac->buf = NULL;
		}
		/* 
		   link to ch */
		if ((ac->ch.next = ch->ac.list))
			ac->ch.next->ch.prev = &ac->ch.next;
		ac->ch.prev = &ch->ac.list;
		ac->ch.ch = ch_get(ch);
		ch->ac.list = ac;
		ch->ac.numb++;
	} else
		ptrace(("%s: ERROR: Could not allocate ac private structure\n", DRV_NAME));
	return (ac);
}
STATIC void
ac_free_priv(struct ac *ac)
{
	struct ch *ch;
	ensure(ac, return);
	if ((ch = ac->ch.ch)) {
		if (ac->buf)
			freemsg(xchg(&ac->buf, NULL));
		/* 
		   unlink from ch */
		if ((*ac->ch.prev = ac->ch.next))
			ac->ch.next->ch.prev = ac->ch.prev;
		ac->ch.next = NULL;
		ac->ch.prev = &ac->ch.next;
		ch_put(xchg(&ac->ch.ch, NULL));
		ch->ac.numb--;
	}
	if (ac->buf)
		freemsg(xchg(&ac->buf, NULL));
	kmem_cache_free(ac_priv_cachep, ac);
	printd(("%s: %p: freed ac private structure\n", DRV_NAME, ac));
}
STATIC ulong
ac_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}

/*
 *  CH allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct ch *
ch_alloc_priv(ulong id, struct mx *mx, ulong slot, ulong type)
{
	struct ch *ch;
	if ((ch = kmem_cache_alloc(ch_priv_cachep, SLAB_ATOMIC))) {
		bzero(ch, sizeof(*ch));
		spin_lock_init(&ch->lock);	/* "ch-lock" */
		ch_get(ch);	/* first get */
		/* 
		   add to master list */
		if ((ch->next == master.ch.list))
			ch->next->prev = &ch->next;
		ch->prev = &master.ch.list;
		master.ch.list = ch_get(ch);
		master.ch.numb++;
		if (mx) {
			struct ch **chp;
			for (chp = &mx->ch.list; (*chp) && (*chp)->mx_slot < slot;
			     (*chp) = (*chp)->mx.next) ;
			/* 
			   add to mx list at slot position */
			ch->mx_slot = slot;
			if ((ch->mx.next = *chp))
				ch->mx.next->mx.prev = &ch->mx.next;
			ch->mx.prev = chp;
			ch->mx.mx = mx_get(mx);
			*chp = ch_get(ch);
			mx->ch.numb++;
		} else {
			ch->mx_slot = -1UL;
			swerr();
		}
		ch->id = ch_get_id(id);
		ch->type = MG_OBJ_TYPE_CH;
		ch->state = MGS_JOINED;
		ch->config.samples = 8;
		ch->config.rate = 8000;
		ch->config.tx_channels = 1;
		ch->config.rx_channels = 1;
		switch (type) {
		default:
			swerr();
		case MX_ENCODING_NONE:
			ch->config.flags = 0;
			ch->config.encoding = MX_ENCODING_NONE;
			ch->config.block_size = 64;
			ch->config.sample_size = 8;
			break;
		case MX_ENCODING_G711_PCM_L:
			ch->config.flags = 0;
			ch->config.encoding = MX_ENCODING_G711_PCM_L;
			ch->config.block_size = 128;
			ch->config.sample_size = 16;
			break;
		case MX_ENCODING_G711_PCM_A:
			ch->config.flags = 0;
			ch->config.encoding = MX_ENCODING_G711_PCM_A;
			ch->config.block_size = 64;
			ch->config.sample_size = 8;
			break;
		case MX_ENCODING_G711_PCM_U:
			ch->config.flags = 0;
			ch->config.encoding = MX_ENCODING_G711_PCM_U;
			ch->config.block_size = 64;
			ch->config.sample_size = 8;
			break;
		}
		printd(("%s: %p: linked ch structure %lu\n", DRV_NAME, ch, ch->id));
	} else
		ptrace(("%s: ERROR: Could not allocate ch structure %lu\n", DRV_NAME, id));
	return (ch);
}
STATIC void
ch_free_priv(struct ch *ch)
{
	psw_t flags;
	ensure(ch, return);
	spin_lock_irqsave(&ch->lock, flags);
	{
		struct mx *mx;
		struct lg *lg;
		struct cn *cn;
		struct ac *ac;
		/* 
		   clear action list */
		while ((ac = ch->ac.list))
			ac_free_priv(ac);
		/* 
		   clear outgoing connection list */
		while ((cn = ch->og.list))
			cn_free_priv(cn);
		/* 
		   clear incoming ocnnection list */
		while ((cn = ch->ic.list))
			cn_free_priv(cn);
		if ((mx = ch->mx.mx)) {
			/* 
			   unlink from mx */
			if ((*ch->mx.prev = ch->mx.next))
				ch->mx.next->mx.prev = ch->mx.prev;
			ch->mx.next = NULL;
			ch->mx.prev = &ch->mx.next;
			ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
			ch_put(ch);
			assure(mx->ch.numb > 0);
			mx->ch.numb--;
			mx_put(xchg(&ch->mx.mx, NULL));
		}
		if ((lg = ch->lg.lg)) {
			/* 
			   unlink from lg */
			if ((*ch->lg.prev = ch->lg.next))
				ch->lg.next->lg.prev = ch->lg.prev;
			ch->lg.next = NULL;
			ch->lg.prev = &ch->lg.next;
			ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
			ch_put(ch);
			assure(lg->ch.numb > 0);
			lg->ch.numb--;
			lg_put(xchg(&ch->lg.lg, NULL));
			/* 
			   we must destroy leg now (this may destroy se too) */
			lg_free_priv(lg);
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
		ch->id = 0;
		if (atomic_read(&ch->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: ch lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, ch, atomic_read(&ch->refcnt)));
			atomic_set(&ch->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&ch->lock, flags);
	ch_put(ch);		/* final put */
}
STATIC struct ch *
ch_lookup(ulong id)
{
	struct ch *ch = NULL;
	if (id)
		for (ch = master.ch.list; ch && ch->id != id; ch = ch->next) ;
	return (ch);
}
STATIC ulong
ch_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		id = ++sequence;
	return (id);
}
STATIC struct ch *
ch_get(struct ch *ch)
{
	atomic_inc(&ch->refcnt);
	return (ch);
}
STATIC void
ch_put(struct ch *ch)
{
	if (atomic_dec_and_test(&ch->refcnt)) {
		kmem_cache_free(ch_priv_cachep, ch);
		printd(("%s: %p: freed ch structure\n", DRV_NAME, ch));
	}
}

/*
 *  MX allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct mx *
mx_alloc_link(queue_t *q, struct mx **mxp, ulong index, cred_t *crp)
{
	struct mx *mx;
	if ((mx = kmem_cache_alloc(mx_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated mx private structure %lu\n", DRV_NAME, mx, index));
		bzero(mx, sizeof(*mx));
		mx_get(mx);	/* first get */
		mx->u.mux.index = index;
		mx->cred = *crp;
		spin_lock_init(&mx->qlock);	/* "mx-queue-lock" */
		(mx->iq = RD(q))->q_ptr = mx_get(mx);
		(mx->oq = WR(q))->q_ptr = mx_get(mx);
		mx->o_prim = mx_w_prim;
		mx->i_prim = mx_r_prim;
		mx->o_wakeup = mx_w_wakeup;
		mx->i_wakeup = NULL;
		mx->i_state = LMI_UNUSABLE;
		mx->i_style = LMI_STYLE1;
		mx->i_version = 1;
		mx->state = MXS_ATTACHED;	/* must be linked in attached state */
		spin_lock_init(&mx->lock);	/* "mx-priv-lock" */
		/* 
		   place in master list */
		if ((mx->next = *mxp))
			mx->next->prev = &mx->next;
		mx->prev = mxp;
		*mxp = mx_get(mx);
		master.mx.numb++;
		mx->id = index;	/* just use mux index for id */
		mx->type = MG_OBJ_TYPE_MX;
	} else
		ptrace(("%s: ERROR: Could not allocate mx private structure %lu\n", DRV_NAME,
			index));
	return (mx);
}
STATIC void
mx_free_link(struct mx *mx)
{
	psw_t flags;
	ensure(mx, return);
	spin_lock_irqsave(&mx->lock, flags);
	{
		struct ch *ch;
		ss7_unbufcall((str_t *) mx);
		flushq(mx->oq, FLUSHALL);
		flushq(mx->iq, FLUSHALL);
		// mx_timer_stop(mx, tall);
		/* 
		   free all channels */
		while ((ch = mx->ch.list))
			ch_free_priv(ch);
		/* 
		   remove from master list */
		if ((*mx->prev = mx->next))
			mx->next->prev = mx->prev;
		mx->next = NULL;
		mx->prev = &mx->next;
		ensure(atomic_read(&mx->refcnt) > 1, mx_get(mx));
		mx_put(mx);
		ensure(master.mx.numb > 0, master.mx.numb++);
		master.mx.numb--;
		/* 
		   remove from queues */
		ensure(atomic_read(&mx->refcnt) > 1, mx_get(mx));
		mx_put(xchg(&mx->iq->q_ptr, NULL));
		ensure(atomic_read(&mx->refcnt) > 1, mx_get(mx));
		mx_put(xchg(&mx->oq->q_ptr, NULL));
		/* 
		   done, check final count */
		if (atomic_read(&mx->refcnt) != 1) {
			__printd(("%s: %s: %p: ERROR: mx lingering reference count = %d\n",
				  DRV_NAME, __FUNCTION__, mx, atomic_read(&mx->refcnt)));
			atomic_set(&mx->refcnt, 1);
		}
	}
	spin_unlock_irqrestore(&mx->lock, flags);
	mx_put(mx);		/* final put */
	return;
}
STATIC struct mx *
mx_lookup(ulong id)
{
	struct mx *mx = NULL;
	if (id)
		for (mx = master.mx.list; mx && mx->id != id; mx = mx->next) ;
	return (mx);
}
STATIC ulong
mx_get_id(ulong id)
{
	static ulong sequence = 0;
	if (!id)
		for (id = ++sequence; mx_lookup(id); id = ++sequence) ;
	return (id);
}
STATIC struct mx *
mx_get(struct mx *mx)
{
	assure(mx);
	if (mx)
		atomic_inc(&mx->refcnt);
	return (mx);
}
STATIC void
mx_put(struct mx *mx)
{
	assure(mx);
	if (mx) {
		assure(atomic_read(&mx->refcnt) > 1);
		if (atomic_dec_and_test(&mx->refcnt)) {
			kmem_cache_free(mx_priv_cachep, mx);
			printd(("%s: %s: %p: freed mx private structure\n", DRV_NAME, __FUNCTION__,
				mx));
		}
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

STATIC struct cdevsw mg_cdev = {
	.d_name = DRV_NAME,
	.d_str = &mginfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
mg_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&mg_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
mg_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&mg_cdev, major)) < 0)
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
mg_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &mginfo, UNITS, DRV_NAME)) < 0)
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
mg_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
mgterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (mg_majors[mindex]) {
			if ((err = mg_unregister_strdev(mg_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					mg_majors[mindex]);
			if (mindex)
				mg_majors[mindex] = 0;
		}
	}
	if ((err = mg_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
mginit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = mg_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		mgterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = mg_register_strdev(mg_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					mg_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				mgterminate();
				return (err);
			}
		}
		if (mg_majors[mindex] == 0)
			mg_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(mg_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = mg_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mginit);
module_exit(mgterminate);

#endif				/* LINUX */
