/*****************************************************************************

 @(#) File: src/drivers/mg.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 *****************************************************************************/

/*
 * This a Media Gateway (MG) multiplexing driver.  The purpose of the MG driver
 * is to manage the sessions of various media streams attached to MX or CH
 * Streams linked under the multiplexing driver, internal canned or emulated
 * audio sources.
 *
 * There are two ways of processing media streams: synchronous and asynchronous.
 * In synchronous processing, processing occurs on one sample in the media
 * stream to completion, before the next sample is processed.  In asynchronous
 * processing, a function is performed on a group of samples: typically a
 * fixed-sized block of samples.  Synchronous processing is suitable for DSP,
 * but inefficient for memory cache processors.  Asynchronous processing is
 * efficient for memory cache processors.  STREAMS and general purpose
 * processors are better suited to block processing.  Data cache efficiency is
 * increased by block processing samples.  Instruction cache efficiency is
 * increased by processing groups of blocks with related processing needs (same
 * execution paths).  Both are facilitated by the STREAMS framework.
 *
 * There are two architectural approaches to processing blocked asynchronous
 * media sessions: a push or pull architecture.
 *
 * Under the push architecture, when data samples are available at a source
 * channel (i.e. input queue), converted samples are "pushed" towards the output
 * channels, where they are queued for transmission.  For complex media sessions
 * requiring broadcast across a number of channels (such as multi-party
 * conferencing), the push architecture results in a significant increase in the
 * amount of memory consumed storing output channel samples in the output
 * queues; however, execution scheduling latency can be reduced because the data
 * is prepared before it is needed for transmission.
 *
 * Under a pull architecture, the data samples are needed (i.e. output queues
 * are enabled), samples are drawn from the input channels.  For complex media
 * sessions requiring broadcast across a number of output channels (again,
 * multi-party conferencing as an example), the pull architecture results in a
 * decrease in the amount of memory consumed because only input samples are
 * stored until needed for transmission.  On the flip side, channels that need
 * to be mixed require more memory than the push architecture.
 *
 * For plain old telephone calls, there is little difference between approaches.
 */

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>
#include <ss7/mgi.h>
#include <ss7/mgi_ioctl.h>

#define MG_DESCRIP	"SS7 Media Gateway (MG) STREAMS Multiplexing Driver"
#define MG_EXTRA	"Part of the OpenSS7 VoIP Stack for Linux Fast-STREAMS"
#define MG_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define MG_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define MG_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MG_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MG_LICENSE	"GPL"
#define MG_BANNER	MG_DESCRIP	"\n" \
			MG_EXTRA	"\n" \
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
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif				/* LINUX */

#define MG_DRV_ID		CONFIG_STREAMS_MG_MODID
#define MG_DRV_NAME		CONFIG_STREAMS_MG_NAME
#define MG_CMAJORS		CONFIG_STREAMS_MG_NMAJORS
#define MG_CMAJOR_0		CONFIG_STREAMS_MG_MAJOR
#define MG_UNITS		CONFIG_STREAMS_MG_NMINORS

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
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size accepted */
	.mi_hiwat = 1,		/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

STATIC struct module_info mg_rinfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size accepted */
	.mi_hiwat = 1,		/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

STATIC struct module_info mux_winfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size accepted */
	.mi_hiwat = 1,		/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

STATIC struct module_info mux_rinfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module ID name */
	.mi_minpsz = 1,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size accepted */
	.mi_hiwat = 1,		/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

STATIC streamscall int mg_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int mg_qclose(queue_t *, int, cred_t *);

STATIC struct qinit mg_rinit = {
	.qi_putp = ss7_oput,	/* Read put (message from below) */
	.qi_srvp = ss7_osrv,	/* Read queue service */
	.qi_qopen = mg_qopen,	/* Each open */
	.qi_qclose = mg_qclose,	/* Last close */
	.qi_minfo = &mg_rinfo,	/* Information */
};

STATIC struct qinit mg_winit = {
	.qi_putp = ss7_iput,	/* Write put (message from above) */
	.qi_srvp = ss7_isrv,	/* Write queue service */
	.qi_minfo = &mg_winfo,	/* Information */
};

STATIC struct qinit mux_rinit = {
	.qi_putp = ss7_iput,	/* Read put (message from below) */
	.qi_srvp = ss7_isrv,	/* Read queue service */
	.qi_minfo = &mux_rinfo,	/* Information */
};

STATIC struct qinit mux_winit = {
	.qi_putp = ss7_oput,	/* Write put (message from above) */
	.qi_srvp = ss7_osrv,	/* Write queue service */
	.qi_minfo = &mux_winfo,	/* Information */
};

MODULE_STATIC struct streamtab mginfo = {
	.st_rdinit = &mg_rinit,	/* Upper read queue */
	.st_wrinit = &mg_winit,	/* Upper write queue */
	.st_muxrinit = &mux_rinit,	/* Lower read queue */
	.st_muxwinit = &mux_winit,	/* Lower write queue */
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

STATIC struct df master = {
#if	defined __SPIN_LOCK_UNLOCKED
	.lock = __SPIN_LOCK_UNLOCKED(master.lock),
#elif	defined SPIN_LOCK_UNLOCKED
	.lock = SPIN_LOCK_UNLOCKED,
#else
#error	cannot initialize spin locks
#endif
};

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
/** mg_convert_msg: - convert a message of samples from the incoming format to
  * the outgoing format.  We don't handle a whole lot of formats here, and PCM_L
  * is actually 16-bit clamped signed native endian rather than 12-bit G711
  * linear.
  *
  * @q: the active queue
  * @dp: data pointer
  * @cn: connection structure
  * @ic_type: incoming media stream type
  * @og_type: outgoing media stream type
  *
  * The @ic_type and @og_type can be one of:
  *
  * - MX_ENCODING_NONE: no encoding.
  *
  * - MX_ENCODING_G711_PCM_L: PCM linear.  This is actually 16-bit clamped
  *   signed native endian rather thean 12-bit G711 linear.
  *
  * - MX_ENCODING_G711_PCM_A: PCM A-law.
  *
  * - MX_ENCODING_G711_PCM_U: PCM mu-law.
  */
STATIC int
mg_convert_msg(queue_t *q, mblk_t *dp, struct cn *cn, ulong ic_type, ulong og_type)
{
	mblk_t *mp;
	size_t bytes, samples;
	uint pad = cn->pad;

	if (unlikely(!pullupmsg(dp, -1)))
		return (-ENOBUFS);

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
			} else if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			} else if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
			} else if (likely(! !(mp = mi_allocb(q, bytes, BPRI_MED)))) {
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
	return (-ENOBUFS);
}

/** mg_convert_input: - convert input format to output or intermediate format if
  * required.
  *
  * @q: active queue
  * @dp: data pointer
  * @cn: connection structure pointer
  */
STATIC int
mg_convert_input(queue_t *q, mblk_t *dp, struct cn *cn)
{
	ulong ic_type = cn->ic.ch->config.encoding;
	ulong og_type = cn->og.ch->config.encoding;

	if (cn->og.ch->og.conn + cn->og.ch->ac.conn > 1) {	/* conferencing */
		/* need to perform conferencing, convert and pad to 16-bit signed native */
		og_type = MX_ENCODING_G711_PCM_L;
	}
	if (ic_type == MX_ENCODING_NONE)
		ic_type = cn->og.ch->lg.lg->se.se->config.encoding;
	return mg_convert_msg(q, dp, cn, ic_type, og_type);
}

/** mg_push_frames: - a channel's incoming connections each have a blocksize of
  * data ready to be output to a channel, and the associated multiplex is ready
  * for data.  We perform a push on the channel and write a block.
  *
  * @q: active queue
  * @mx: multiplex structure pointer
  * @ch: channel structure pointer
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
		/* we need an MX_DATA_REQ on the front of the data */
		struct MX_data_req *p;

		if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
			goto enobufs;
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_DATA_REQ;
		p->mx_slot = ch->mx_slot;
	}
	/* calculate padding for confrence */
	for (pad = -1, cons = ch->og.conn + ch->ac.conn; cons; pad++, cons >>= 1) ;
	if (pad < 0)
		goto efault;
	if (!pad && !ch->ac.conn) {
		/* just one connection active */
		size_t bytes = ch->config.block_size >> 3;

		for (cn = ch->og.list; cn; cn = cn->og.next)
			if (!(cn->flags & MGF_PULL_PENDING) && (cn->flags & MGF_OG_READY))
				break;
		if (!cn)
			goto efault;
		/* pull up samples on active connection */
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

		/* need to perform conferencing of connections and actions */
		size_t bytes = samples << 1;

		ensure(dp, return (-EFAULT));
		/* we need a working/result buffer */
		if (unlikely(!(dp = mi_allocb(q, bytes, BPRI_MED))))
			goto enobufs;
		dp->b_datap->db_type = M_DATA;
		bzero(dp->b_rptr, bytes);
		dp->b_wptr += bytes;
		/* conference in connections and actions */
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
		/* actions are always PCM_L coding */
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
		/* convert summation buffer to results buffer in place */
		if ((og_type = ch->config.encoding) == MX_ENCODING_NONE)
			og_type = ch->lg.lg->se.se->config.encoding;
		/* when we convert conference info, it is always in PCM_L format */
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

/** mg_wakeup_multiplex: - a multiplex has all of its outgoing channels ready.
  * Push the data down to the multiplex or wait for backenable, or bufcall.
  * This push function assumes that the lower multiplex needs all connected
  * channels written for proper flow control.
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
	/* wait for more data, or a backenable to pull frames */
	return;
}

/** mg_r_wakeup: - MG stream read wakeup procedure.  A queue wakeup procedure is
  * executed once for each time that the queue's service procedure is run.
  * These wakeup function are used to defer actions until the queue service
  * procedure runs.
  *
  * @q: the queue being woken
  */
STATIC void
mg_r_wakeup(queue_t *q)
{
	mg_wakeup_multiplex(MX_PRIV(q));
}

/** mx_w_wakeup: - MX stream write wakeup procedure.  A queue wakeup procedure
  * is executed once for each time that the queue's service procedure is run.
  * These wakeup function are used to defer actions until the queue service
  * procedure runs.
  *
  * @q: the queue being woken
  */
STATIC void
mx_w_wakeup(queue_t *q)
{
	mg_wakeup_multiplex(MX_PRIV(q));
}

/** mg_wakeup_channel: - wakes a channel process
  * @ch: the channel to wake
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

/** mg_wakeup_connection: - wakes a connection process
  * @cn: the connection to wake
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

#if 0
/** mg_push_connect: - a connection has data.  Convert the data and push it to
  * the outgoing channel.
  *
  * return: - Note that mg_convert_input (and this function) will return
  * QR_ABSORBED if the data has been moved (an possibly converted in-place)
  * rather than coverted to a new buffer.  Otherwise it returns QR_DONE.
  */
STATIC int
mg_push_connect(queue_t *q, mblk_t *mp, struct cn *cn)
{
	int err;

	if ((err = mg_convert_input(q, mp, cn)) >= 0)
		mg_wakeup_connection(cn);
	return (err);
}
#endif

/** ch_write: - write to a channel
  *
  * @q: the active queue
  * @dp: the data to write
  * @ch: the channel to which to write
  *
  * This function is called whenever there is input data from a source to a
  * connected sesssion.  For each connection in a session that is accepting
  * data, the data is converted from the source format to the connection format,
  * and data is queued to the connection.  If any of the outgoing channels have
  * sufficient data to output a block, they are marked for output and a wakeup
  * is scheduled for the channel.  A wakeup consists of marking the channel's
  * output queue for processing (a STREAMS qenable()).  When the service
  * procedure of the output queue runs, blocks will be collected and
  * transmitted.
  */
STATIC int
ch_write(queue_t *q, mblk_t *dp, struct ch *ch)
{
	if (ch->ic.ready >= ch->ic.conn) {
		int err;
		struct cn *cn;

		/* The purpose of the MGF_PUSH_PENDING flags here is in case we have a buffer
		   allocation failure during mg_convert_input() that results in an -ENOBUFS error
		   return, which will result in placing the data primitive back on the input queue
		   and awaiting a bufcall.  When we come back, we will not write again the samples
		   that were already written. */
		for (cn = ch->ic.list; cn; cn = cn->next) {
			if (!(cn->flags & MGF_PUSH_PENDING) && (cn->flags & MGF_IC_READY)) {
				if ((err = mg_convert_input(q, dp, cn)) < 0)
					return (err);
				mg_wakeup_connection(cn);
				if (err)
					return (err);
				cn->flags |= MGF_PUSH_PENDING;
			}
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

	if (likely(! !(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_FLUSH;
		*(mp->b_wptr)++ = flags | band ? FLUSHBAND : 0;
		*(mp->b_wptr)++ = band;
		printd(("%s: %p: <- M_FLUSH\n", DRV_NAME, mg));
		ss7_oput(mg->oq, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
#endif

/** m_error: - generate error indication upstream when required.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @error: error code
  */
STATIC int
m_error(queue_t *q, mblk_t *msg, struct mg *mg, int error)
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
	if (unlikely(!(mp = mi_allocb(q, 2, BPRI_MED))))
		goto enobufs;

	freemsg(msg);

	if (hangup) {
		DB_TYPE(mp) = M_HANGUP;
		mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- M_HANGUP");
	} else {
		DB_TYPE(mp) = M_ERROR;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		*(mp->b_wptr)++ = error < 0 ? -error : error;
		mg->i_state = MGS_UNUSABLE;
		mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- M_ERROR");
	}
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** m_data: - the preferred method for sending data to the media gateway user is
  * to just send M_DATA blocks for efficiency and speed.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @dp: data payload
  */
static int
mg_read(queue_t *q, mblk_t *msg, struct mg *mg, mblk_t *dp)
{
	if (unlikely(!canput(mg->oq)))
		goto ebusy;

	if (msg && msg != dp) {
		if (msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
	}
	mi_strlog(mg->oq, STRLOGDA, SL_TRACE, "<- M_DATA");
	putnext(mg->oq, dp);
	return (0);

      ebusy:
	return (-EBUSY);
}

/** mg_info_ack: - acknowledges an information request and provides information
  * about the connection points in which the MG is bound.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  */
static int
mg_info_ack(queue_t *q, mblk_t *msg, struct mg *mg)
{
	struct se *se = mg->se.list;
	mblk_t *mp;
	MG_info_ack_t *p;
	MG_channel_opt_t *o;
	size_t opt_len = sizeof(*o);

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_INFO_ACK;
	p->mg_se_id = se ? se->id : 0;
	p->mg_opt_length = opt_len;
	p->mg_opt_offset = opt_len ? sizeof(*p) : 0;
	p->mg_prov_flags = 0;
	p->mg_style = MG_STYLE2;
	p->mg_version = MG_VERSION;
	mp->b_wptr += sizeof(*p);
	o = (typeof(o)) mp->b_wptr;
	o->mg_obj_type = MG_OBJ_TYPE_CH;
	o->ch_block_size = mg->config.block_size;
	o->ch_encoding = mg->config.encoding;
	o->ch_sample_size = mg->config.sample_size;
	o->ch_rate = mg->config.rate;
	o->ch_tx_channels = mg->config.tx_channels;
	o->ch_rx_channels = mg->config.rx_channels;
	o->ch_flags = mg->config.flags;
	mp->b_wptr += sizeof(*o);
	/* state does not change */
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_INFO_ACK");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_optmgmt_ack: acknowledges and reports success of failure of the requested
  * operation on provider or channel options associated with the requesting
  * stream.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @opt_ptr: options pointer
  * @opt_len: options length
  * @flags: flags
  */
static int
mg_optmgmt_ack(queue_t *q, mblk_t *msg, struct mg *mg, uchar *opt_ptr, size_t opt_len, ulong flags)
{
	MG_optmgmt_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_OPTMGMT_ACK;
	p->mg_opt_length = opt_len;
	p->mg_opt_offset = opt_len ? sizeof(*p) : 0;
	p->mg_mgmt_flags = flags;
	mp->b_wptr += sizeof(*p);
	if (opt_len) {
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
	}
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- M_OPTMGMT_ACK");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_ok_ack: - positively acknowledges the last operation requiring
  * acknowledgement.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @prim: correct primitive
  */
static int
mg_ok_ack(queue_t *q, mblk_t *msg, struct mg *mg, long prim)
{
	MG_ok_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_OK_ACK;
	p->mg_correct_prim = prim;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	switch (mg_get_state(mg)) {
	case MGS_WACK_DREQ:
		mg_set_state(mg, mg->ch.list ? MGS_ATTACHED : MGS_DETACHED);
		break;
	}
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_OK_ACK");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_error_ack: - negatively acknowledges the last operation requiring
  * negative acknolwedgement and provides the error type and unix error.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @prim: primitive in error
  * @error: error code
  */
static int
mg_error_ack(queue_t *q, mblk_t *msg, struct mg *mg, long prim, long error)
{
	MG_error_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_ERROR_ACK;
	p->mg_error_primitive = prim;
	p->mg_error_type = error < 0 ? MGSYSERR : error;
	p->mg_unix_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_ERROR_ACK");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_attach_ack: - acknowledges the attachment of ther requesting stream to a
  * newly created or existing communications session (context).
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @ch: channel structure
  */
static int
mg_attach_ack(queue_t *q, mblk_t *msg, struct mg *mg, struct ch *ch)
{
	MG_attach_ack_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_ATTACH_ACK;
	p->mg_mx_id = ch->mx.mx ? ch->mx.mx->id : 0;
	p->mg_mx_slot = ch->mx_slot;
	p->mg_ch_id = ch->id;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mg_set_state(mg, MGS_ATTACHED);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_ATTACH_ACK");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_join_con: - confirms that the requested channel or requesting stream has
  * joined the communications session.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  */
static int
mg_join_con(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg)
{
	MG_join_con_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_JOIN_CON;
	p->mg_se_id = se->id;
	p->mg_tp_id = lg->id;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	se_set_state(se, MGS_JOINED);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_JOIN_CON");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_action_con: - confirms that the requested action has begun.
  * MG_ACTION_REQ which have the MG_MORE_DATA flags set will not be confirmed
  * until the last MG_ACTION_REQ has been issued by the MG user.  The end of
  * restricted duration actions will be indicated with the MG_ACTION_IND
  * primitive.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  * @ac: action structure
  */
static int
mg_action_con(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg, struct ac *ac)
{
	MG_action_con_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_ACTION_CON;
	p->mg_action = ac->type;
	p->mg_se_id = se->id;
	p->mg_tp_id = lg->id;
	p->mg_action_id = ac->id;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_ACTION_CON");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_action_ind: - indicates that the action identified by the indicated
  * action identifier has completed.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  * @ac: action structure
 */
static int
mg_action_ind(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg, struct ac *ac)
{
	MG_action_ind_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_ACTION_IND;
	p->mg_action = ac->type;
	p->mg_se_id = se->id;
	p->mg_tp_id = lg->id;
	p->mg_action_id = ac->id;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_ACTION_IND");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_conn_con: - confirms that the indicated channel has been connected as
  * requested.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  */
static int
mg_conn_con(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg)
{
	MG_conn_con_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_CONN_CON;
	p->mg_se_id = se ? se->id : 0;
	p->mg_tp_id = lg ? lg->id : 0;
	mp->b_wptr += sizeof(*p);
	if (lg)
		lg->state = MGS_CONNECTED;
	if (se)
		se->state = MGS_CONNECTED;
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_CONN_CON");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_data_ind: - indicates that data was received on a connected channel.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @flags: data flags
  * @dp: data payload
 */
static int
mg_data_ind(queue_t *q, mblk_t *msg, struct mg *mg, ulong flags, mblk_t *dp)
{
	MG_data_ind_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(mg->oq)))
		goto ebusy;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_DATA_IND;
	p->mg_flags = flags;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (msg && msg != dp)
		freemsg(msg);
	mi_strlog(mg->oq, STRLOGDA, SL_TRACE, "<- MG_DATA_IND");
	putnext(mg->oq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** mg_discon_ind: - indicates that a channel has been disconnected as a result
  * of a lower level failure and the cause of the disconnection.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  * @flags: disconnect flags
  * @cause: disconnect cause
  */
static int
mg_discon_ind(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg, ulong flags,
	      ulong cause)
{
	MG_discon_ind_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_DISCON_IND;
	p->mg_se_id = se ? se->id : 0;
	p->mg_tp_id = lg ? lg->id : 0;
	p->mg_conn_flags = flags;
	p->mg_cause = cause;
	mp->b_wptr += sizeof(*p);

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
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_DISCON_IND");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_discon_con: - confirms that the indicated channel has been disconnected
  * as requested.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  * @flags: disconnect flags
 */
static int
mg_discon_con(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg, ulong flags)
{
	MG_discon_con_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_DISCON_CON;
	p->mg_se_id = se ? se->id : 0;
	p->mg_tp_id = lg ? lg->id : 0;
	mp->b_wptr += sizeof(*p);

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
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_DISCON_CON");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_leave_ind: - indicates that a channel has left a communication session as
  * a result of a lower level failure and the cause of the deletion.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  * @cause: leave cause
  */
static int
mg_leave_ind(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg, ulong cause)
{
	MG_leave_ind_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_LEAVE_IND;
	p->mg_se_id = se ? se->id : 0;
	p->mg_tp_id = lg ? lg->id : 0;
	p->mg_cause = cause;
	mp->b_wptr += sizeof(*p);

	fixme(("Adjust lg and se states\n"));
	if (lg) {
	}
	if (se) {
	}
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_LEAVE_IND");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_leave_con: - confirms that the indicated channel or requesting stream has
  * left the communications session as requested.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @se: session structure
  * @lg: leg structure
  */
static int
mg_leave_con(queue_t *q, mblk_t *msg, struct mg *mg, struct se *se, struct lg *lg)
{
	mblk_t *mp;
	MG_leave_con_t *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_LEAVE_CON;
	p->mg_se_id = se ? se->id : 0;
	p->mg_tp_id = lg ? lg->id : 0;
	mp->b_wptr += sizeof(*p);

	fixme(("Adjust lg and se states\n"));
	if (lg) {
	}
	if (se) {
	}
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_LEAVE_CON");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mg_notify_ind: - indicates to the media gateway user that an event has
  * occured on in the session.
  * @q: active queue
  * @msg: message to free upon success
  * @mg: private structure
  * @event: event numnber
  * @dia_ptr: diagnostic pointer
  * @dia_len: diagnostic length
  */
static int
mg_notify_ind(queue_t *q, mblk_t *msg, struct mg *mg, ulong event, uchar *dia_ptr, size_t dia_len)
{
	mblk_t *mp;
	MG_notify_ind_t *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + dia_len, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mg_primitive = MG_NOTIFY_IND;
	p->mg_event = event;
	p->mg_diag_length = dia_len;
	p->mg_diag_offset = dia_len ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dia_len) {
		bcopy(dia_ptr, mp->b_wptr, dia_len);
		mp->b_wptr += dia_len;
	}
	freemsg(msg);
	mi_strlog(mg->oq, STRLOGTX, SL_TRACE, "<- MG_NOTIFY_IND");
	putnext(mg->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  -------------------------------------------------------------------------
 */
/** mx_write: - the preferred method for sending data to the multiplex provider
  * is to just send M_DATA blocks for efficiency and speed.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
  * @dp: data
  */
static int
mx_write(queue_t *q, mblk_t *msg, struct mx *mx, mblk_t *dp)
{
	if (unlikely(!canputnext(mx->oq)))
		goto ebusy;

	if (msg && msg != dp) {
		if (msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
	}
	mi_strlog(mx->oq, STRLOGDA, SL_TRACE, "M_DATA ->");
	putnext(mx->oq, dp);
	return (0);

      ebusy:
	return (-EBUSY);
}

/** mx_info_req: - requests that the multiplex provider return information
  * concerning the multiplex provider and any attached slots.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
  */
static int
mx_info_req(queue_t *q, mblk_t *msg, struct mx *mx)
{
	MX_info_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "MX_INFO_REQ ->");
	putnext(mx->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mx_attach_req: - requests that the multiplex provider attach the requesting
  * stream to the specified multiplex (address) with the specified options.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
  * @add_ptr: address pointer
  * @add_len: address length
  * @flags: attach flags
  */
static int
mx_attach_req(queue_t *q, mblk_t *msg, struct mx *mx, uchar *add_ptr, size_t add_len, ulong flags)
{
	MX_attach_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = MX_ATTACH_REQ;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_ATTACH_REQ;
	p->mx_addr_length = add_len;
	p->mx_addr_offset = add_len ? sizeof(*p) : 0;
	p->mx_flags = flags;
	mp->b_wptr += sizeof(*p);
	if (add_len) {
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
	}
	mx->state = MGS_WACK_AREQ;
	freemsg(msg);
	mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "MX_ATTACH_REQ ->");
	putnext(mx->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mx_enable_req: - requests that the attached multiplex be enabled and
  * confirmed enabled with a MX_ENABLE_CON.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
  */
static int
mx_enable_req(queue_t *q, mblk_t *msg, struct mx *mx)
{
	MX_enable_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = MX_ENABLE_REQ;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_ENABLE_REQ;
	mp->b_wptr += sizeof(*p);
	mx->state = MGS_WCON_JREQ;
	freemsg(msg);
	mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "MX_ENABLE_REQ ->");
	putnext(mx->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mx_connect_req: - requests that the multiplex provider connect the
  * multiplex's transmit or receive direction on the specified slot to the
  * requesing stream.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
  * @ch: channel structure
  *
  * When successful, subsequent data sent as MX_DATA_REQ will be transmitted on
  * the multiplex and subsequent data received on the multiplex will be
  * delivered as MX_DATA_IND.  Any M_DATA blocks provided with the connect
  * request will be transmitted immediately following the successful connection.
 */
static int
mx_connect_req(queue_t *q, mblk_t *msg, struct mx *mx, struct ch *ch)
{
	mblk_t *mp;
	MX_connect_req_t *p;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_CONNECT_REQ;
	p->mx_conn_flags = ch->flags & (MXF_RX_DIR | MXF_TX_DIR);
	p->mx_slot = ch->mx_slot;
	mp->b_wptr += sizeof(*p);
	ch->state = MGS_WCON_CREQ;
	freemsg(msg);
	mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "MX_CONNECT_REQ ->");
	putnext(mx->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mx_data_req: - the non-preferred method for sending data to the multiplex
  * provider is to send MX_DATA_REQ blocks.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
  * @slot: multiplex slot
  * @dp: data payload
  *
  * We prefer just send M_DATA blocks for efficiency and speed.  See mx_write()
  * above.  If slot is specified as -1UL, then the each byte of the data
  * corresponds to one slot in the multiplex.  If slot is specified 0 <= slot <
  * n, where n is the number of slots in the multiplex, then the data consists
  * of data only for the specified slot.
  */
static int
mx_data_req(queue_t *q, mblk_t *msg, struct mx *mx, ulong slot, mblk_t *dp)
{
	MX_data_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(mx->oq)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_DATA_REQ;
	p->mx_slot = slot;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (msg && msg != dp)
		freemsg(msg);
	mi_strlog(mx->oq, STRLOGDA, SL_TRACE, "MX_DATA_REQ ->");
	putnext(mx->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
      ebusy:
	freeb(mp);
	return (-EBUSY);
}

/** mx_disconnect_req: - requests that the multiplex provider disconnect the
  * multiplex's transmit or receive direction for the specified slot from the
  * requesting stream.
  *
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
  * @ch: channel structure
  * @flags: disconnect flags
  *
  * When successful, subsequent received data and MX_DATA_REQ for the
  * disconnected slot will be discarded.
  */
static int
mx_disconnect_req(queue_t *q, mblk_t *msg, struct mx *mx, struct ch *ch, ulong flags)
{
	MX_disconnect_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_DISCONNECT_REQ;
	p->mx_conn_flags = flags;
	p->mx_slot = ch->mx_slot;
	mp->b_wptr += sizeof(*p);
	ch->state = MGS_WCON_DREQ;
	ch->flags &= ~flags;
	freemsg(msg);
	mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "MX_DISCONNECT_REQ ->");
	putnext(mx->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/** mx_disable_req: - requests that the attached multiplex be disabled and the
  * disabling confirmed with a MX_DISABLE_CON primitive.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: private structure
 */
static int
mx_disable_req(queue_t *q, mblk_t *msg, struct mx *mx)
{
	MX_disable_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(mx->oq)))
		goto ebusy;

	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_DISABLE_REQ;
	mp->b_wptr += sizeof(*p);
	mg_set_state(mx, MGS_WCON_LREQ);
	freemsg(msg);
	mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "MX_DISABLE_REQ ->");
	puntext(mx->oq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** mx_detach_req: - request that the multiplex provider detach the requesting
  * stream from the currently attached multiplex.
  * @q: active queue
  * @msg: message to free upon success
  * @mx: multiplex stream
  */
static int
mx_detach_req(queue_t *q, mblk_t *msg, struct mx *mx)
{
	MX_detach_req_t *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	if (unlikely(!canputnext(mx->oq)))
		goto ebusy;

	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->mx_primitive = MX_DETACH_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mg_set_state(mx, MGS_WACK_UREQ);
	mi_strlog(mx->oq, STRLOGTX, SL_TRACE, "MX_DETACH_REQ ->");
	putnext(mx->oq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
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
 *  Preferred way of receiving data from multiplex provider.  The multiplex
 *  provider should always give us data as M_DATA blocks.
 */
STATIC int
mx_read(queue_t *q, mblk_t *dp, struct mx *mx)
{
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
 *  Indicates information about the multiplex provider and any attached
 *  multiplex.
 */
STATIC int
mx_info_ack(queue_t *q, mblk_t *mp, struct mx *mx)
{
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
mx_ok_ack(queue_t *q, mblk_t *mp, struct mx *mx)
{
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
 *  Indicates that the last operation requiring negative acknowledgement was
 *  unsuccessful.
 */
STATIC int
mx_error_ack(queue_t *q, mblk_t *mp, struct mx *mx)
{
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
 *  Confirms that the attached multiplex was enabled.  When we receive
 *  confirmation that the multiplex was enabled, we check any legs associated
 *  with the multiplex channels to see if the legs are waiting for enable (join)
 *  confirmation.  If they are, and this is the last channel to be enabled in
 *  the leg, we confirm the leg and session joined to any MG user.
 */
STATIC int
mx_enable_con(queue_t *q, mblk_t *mp, struct mx *mx)
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
 *  Confirms a connection and also includes any data which has arrived on the
 *  attached and connected multiplex as a result of the connection.  When we
 *  receive a connection confirmation that a specific slot in the multiplex was
 *  connected, we check any legs associated with the multiple channel to see if
 *  the leg is awaiting connection.  If the leg is awaiting connection, we check
 *  to see if any other channels in the leg are awaiting connection, or if any
 *  of the channels at the other end of a connection to these channels are
 *  awaiting connection at their end.  If we are the last channel to connect, we
 *  confirm the leg and the session connected to any MG user.
 */
STATIC int
mx_connect_con(queue_t *q, mblk_t *mp, struct mx *mx)
{
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
		/* if there are any other channels in the multirate group waiting for confirmed
		   connection, just wait for them. */
		if (c2->state == MGS_WCON_CREQ)
			goto done;
		for (cn = c2->ic.list; cn; cn = cn->ic.next) {
			if ((cn->state == MGS_WCON_CREQ) && (c3 = cn->og.ch)) {
				/* if the outgoing end of the connection is waiting for confirmed
				   connection, just wait for them */
				if (c3->state == MGS_WCON_CREQ)
					goto done;
				if (c3->state == MGS_CONNECTED)
					cn->state = MGS_CONNECTED;
			}
		}
		for (cn = c2->og.list; cn; cn = cn->og.next) {
			if ((cn->state == MGS_WCON_CREQ) && (c3 = cn->ic.ch)) {
				/* if the incoming end of the connection is waiting for confirmed
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
 *  Indicates that data has arrived for the attached multiplex.  This is the
 *  non-prefered way of receiving data from a simple multiplex (one channel);
 *  the simple multiplex provider should only send us raw M_DATA blocks where
 *  possible.
 */
STATIC int
mx_data_ind(queue_t *q, mblk_t *mp, struct mx *mx)
{
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
 *  Indicates that an autonomous disconnection of the multiplex occured, the
 *  cause of the disconnection (normaly multiplex provider lower layer fault),
 *  and also includes any data which has arrived on the attached and
 *  disconnected multiplex before it was disconnected.
 */
STATIC int
mx_disconnect_ind(queue_t *q, mblk_t *mp, struct mx *mx)
{
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
		/* When one channel in a multirate group is lost, they must all be disconnected */
		if (c2->state != MXS_CONNECTED && c2->state != MXS_WCON_CREQ)
			continue;
		if (p->mx_conn_flags & MXF_RX_DIR) {
			while ((cn = c2->ic.list)) {
				/* check if we are only connection to other end */
				if ((c3 = cn->og.ch) && c3->state == MXS_CONNECTED
				    && c3->flags & MXF_TX_DIR && c3->og.numb == 1
				    && (err = mx_disconnect_req(q, c3->mx.mx, c3, MXF_TX_DIR)))
					return (err);
				cn_free_priv(c2->ic.list);
			}
		}
		if (p->mx_conn_flags & MXF_TX_DIR) {
			while ((cn = c2->og.list)) {
				/* check if we are only connection to other end */
				if ((c3 = cn->ic.ch) && c3->state == MGS_CONNECTED
				    && c3->flags & MXF_RX_DIR && c3->ic.numb == 1
				    && (err = mx_disconnect_req(q, c3->mx.mx, c3, MXF_RX_DIR)))
					return (err);
				cn_free_priv(c2->og.list);
			}
		}
		/* disconnect other channels in this leg */
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
 *  Confirms the disconnection and also includes any data which arrived before
 *  the disconnection was completed.  We must also effect and confirm any
 *  pending topology requests.
 */
STATIC int
mx_disconnect_con(queue_t *q, mblk_t *mp, struct mx *mx)
{
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
		/* if there are any other channels in the multirate group waiting for confirmed
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
 *  Indicates that the attached multiplex was disabled (and disconnected if
 *  connected) due to a lower layer fault.  The cause is also indicated.  For
 *  common circuits, this indicates the abortive release of the circuit.
 */
STATIC int
mx_disable_ind(queue_t *q, mblk_t *mp, struct mx *mx)
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
 *  Confirms that the attached multiplex was disabled as requested.  For common
 *  circuits this is the same as confirming the release of the circuit.
 */
STATIC int
mx_disable_con(queue_t *q, mblk_t *mp, struct mx *mx)
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
 *  This is the preferred way for an MG stream to provide data to the connection
 *  point, but it is only valid for the first channel.
 */
STATIC int
mg_write(queue_t *q, mblk_t *dp, struct mg *mg)
{
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

/** mg_info_req: - requests that the provider return in an MG_INFO_ACK
  * information about the provider and any attached communications sessions
  * (contexts).
  * @q: active queue (upper write queue)
  * @mp: the MG_INFO_REQ primitive
  * @mg: private structure (locked)
  */
STATIC int
mg_info_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	MG_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	switch (mg_get_state(mg)) {
	case MGS_UNINIT:
		goto retry;
	case MGS_UNUSABLE:
		goto outstate;
	default:
		break;
	}
	return mg_info_ack(q, mp, mg);
      retry:
	err = -EAGAIN;
	goto error;
      outstate:
	err = MGOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_INFO_REQ, err);
}

/** mg_optmgmt_req: - requests that the provider set, negotiate, or get option
  * information and return acknowledgement using an MG_OPTMGMT_ACK.
  * @q: active queue (upper write queue)
  * @mp: the MG_OPTMGMT_REQ primitive
  * @mg: private structure (locked)
  */
STATIC int
mg_optmgmt_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct MG_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	union MG_options *o;
	uchar *opt_ptr;
	size_t opt_len;
	ulong flags;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (!MBLKIN(mp, p->mg_opt_offset, p->mg_opt_length))
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
				/* negotiate current */
			case MG_SET_OPT:
				/* set current */
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
				return mg_optmgmt_ack(q, mp, mg, opt_ptr, opt_len, flags);
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
				return mg_optmgmt_ack(q, mp, mg, opt_ptr, opt_len, flags);
			}
			break;
		default:
			goto badopttype;
		}
	} else {
		union MG_options opts;

		o = &opts;
		/* no options */
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
			return mg_optmgmt_ack(q, mp, mg, opt_ptr, opt_len, flags);
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
			return mg_optmgmt_ack(q, mp, mg, opt_ptr, opt_len, flags);
		}
	}
	goto badflag;
      badflag:
	err = MGBADFLAG;
	goto error;
      badopttype:
	err = MGBADOPTTYPE;
	goto error;
      badopt:
	err = MGBADOPT;
	goto error;
      outstate:
	err = MGOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_OPTMGMT_REQ, err);
}

/** mg_attach_req: - requests that the provider attach the specified multiplex
  * and slot to a specified or newly created channel id.  The primitive is
  * acknowledged wtih the MG_ATTACH_ACK.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_ATTACH_REQ primitive
  * @mg: private structure (locked)
  */
STATIC int
mg_attach_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct mx *mx;
	struct ch *ch;
	MG_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	switch (mg_get_state(mg)) {
	case MGS_DETACHED:
	case MGS_ATTACHED:
		if (!p->mg_mx_id) {
			/* requesting stream */
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
		mg_set_state(mg, MGS_WACK_AREQ);
		goto channel_known;
		/* fall through */
	case MGS_WACK_AREQ:
		if (!p->mg_mx_id) {
			/* requesting stream */
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
		return mg_attach_ack(q, mp, mg, ch);
	}
	goto outstate;
      outstate:
	err = MGOUTSTATE;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      enomem:
	err = -ENOMEM;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_ATTACH_REQ, err);
}

/** mg_detach_req: - requests that the provider detach the requesting stream
  * from an attached channel and acknowledge success with an MG_OK_ACK or
  * MG_ERROR_ACK.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_DETACH_REQ primitive
  * @mg: private structure (locked)
  */
STATIC int
mg_detach_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct ch *ch;
	MG_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	switch (mg_get_state(mg)) {
	case MGS_ATTACHED:
		if (!(ch = ch_lookup(p->mg_ch_id)))
			goto badid;
		ch_free_priv(ch);
		mg_set_state(mg, MGS_WACK_DREQ);
		/* fall through */
	case MGS_WACK_DREQ:
		return mg_ok_ack(q, mp, mg, MG_DETACH_REQ);
	default:
		goto outstate;
	}
      outstate:
	err = MGOUTSTATE;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_DETACH_REQ, err);
}

/** mg_join_req: - requests that the specified channels be joines to the
  * specified session with newly created termination point with id specified by
  * the caller (mg_tp_id non-zero) or assigned by the provider (mg_tp_id zero).
  * If the specified session id is zero then a new session will be created.  If
  * the specified session id exists, the existing session will be joined.  If
  * the specified session id is non-zero but does not exist, one will be created
  * with the specified id.  All channels must be successfully enabled before
  * this primitive will be confirmed.  This primitive is confirmed with the
  * MG_JOIN_CON primitive.  It is refused with the MG_ERROR_ACK or MG_LEAVE_IND
  * primitive.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_JOIN_REQ primitive
  * @mg: private structure (locked)
  */
STATIC int
mg_join_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct se *se;
	struct lg *lg;
	struct ch *ch, **chp;
	struct mx *mx;
	ulong *idp, *edp;
	ulong slot;
	int waiting;

	MG_join_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (!MBLKIN(mp, p->mg_channel_length, p->mg_channel_offset))
		goto emsgsize;
	if (p->mg_channel_length < sizeof(ulong))
		goto badopt;
	if (!((1 << mg->i_state) & (MGSF_DETACHED | MGSF_ATTACHED)))
		goto outstate;
	if (!(se = se_lookup(p->mg_se_id, mg)) && !(se = se_alloc_priv(p->mg_se_id, mg)))
		goto enomem;
	switch (se_get_state(se)) {
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
			/* link channels to leg */
			if ((ch->lg.next = *chp))
				ch->lg.next->lg.prev = &ch->lg.next;
			ch->lg.next = *chp;
			ch->lg.prev = chp;
			*chp = ch_get(ch);
			lg->ch.numb++;
			ch->lg_slot = slot;
			ch->lg.lg = lg_get(lg);
		}
		se_set_state(se, MGS_WACK_JREQ);
		p->mg_tp_id = lg->id;
		goto lg_known;
	case MGS_WACK_JREQ:
		for (lg = se->lg.list; lg && lg->id != p->mg_tp_id; lg = lg->se.next) ;
		if (!lg)
			goto efault;
	      lg_known:
		waiting = 0;
		/* make sure all the muxltiplex are enabled */
		for (ch = lg->ch.list; ch; ch = ch->lg.next) {
			if ((mx = ch->mx.mx)) {
				if (mx == (struct mx *) mg)
					continue;
				switch (mx->state) {
				case MXS_DETACHED:
					if ((err = mx_enable_req(q, NULL, mx)) < 0)
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
		return mg_join_con(q, mp, mg, se, lg);
	}
      outstate:
	err = MGOUTSTATE;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      idbusy:
	err = MGIDBUSY;
	goto error;
      badopt:
	err = MGBADOPT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      enomem:
	mi_bufcall(q, sizeof(*se), BPRI_MED);
	err = -ENOMEM;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_JOIN_REQ, err);
}

/** mg_action_req: - requests that the action of the requested type be performed
  * against the specified session and termination point for the requested
  * duration and with the requested options.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_ACTION_REQ primitive
  * @mg: private structure (locked)
  *
  * When more data is indicated using the MG_MORE_DATA flag, it indicates that
  * subsequence MG_ACTION_REQ primitives contain more data for the associated
  * pattern.  The data is encoded according to the media format of the
  * requesting stream.  When the requested duration is zero, the action will
  * continue until its normal termination, or until subsequently aborted.
  * Actions on terminations which are currently connected in a communications
  * session will be mixed with the media received from the communications
  * session and any other actions which are currently being performed on the
  * termination.  Actions on terminations which are currently disconnected from
  * a communications session will be mixed with the media from other actions on
  * the termination point.  Some actions can only be performed on disconnected
  * termination points (e.g., MG_ACTION_LOOPBACK, MG_ACTION_TEST_SILENT).  Some
  * actions replace other actions on the termination point (e.g.,
  * MG_ACTION_LOOPBACK).  Some actions performed on a termination point will be
  * performed on individual channels that make up the termination point (e.g.,
  * MG_ACTION_LOOPBACK).
  */
STATIC int
mg_action_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct se *se;
	struct lg *lg;
	struct ch *ch;
	struct ac *ac;
	MG_action_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
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
		if (!(ac = ac_alloc_priv(0, p->mg_action, p->mg_flags, p->mg_duration,
					 ch, mp->b_cont)))
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
		return mg_action_con(q, mp, mg, se, lg, ac);
	return (QR_TRIMMED);
      outstate:
	err = MGOUTSTATE;
	goto error;
      badact:
	err = MGBADACT;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      enomem:
	mi_bufcall(q, sizeof(*ac), BPRI_MED);
	err = -ENOMEM;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_ACTION_REQ, err);
}

/** mg_abort_req: - requests that the specified action be aborted.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_ABORT_REQ primitive
  * @mg: private structure (locked)
  *
  * This primitive is confirmed with the MG_OK_ACK primitive.  If the action
  * identifier is MG_ACTION_PREVIOUS, this primitive requests that the
  * previously initiated (unconfirmed) action be aborted.  If the action
  * identifier is zero, this primitive requests that all actions ont the
  * specified termination point be aborted.
  */
STATIC int
mg_abort_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct se *se;
	struct lg *lg;
	struct ch *ch;
	struct ac *ac;
	MG_abort_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
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
		/* abort specified action on termination point */
		for (ch = lg->ch.list; ch; ch = ch->lg.next)
			for (ac = ch->ac.list; ac; ac = ac->ch.next)
				if (ac->id == p->mg_action_id)
					goto found;
		goto badid;
	      found:
		/* FIXME: convert this */
		if ((err = mg_ok_ack(q, mg, p->mg_primitive)) < 0)
			return (err);
		ac_free_priv(ac);	/* this will destroy the action */
		return (QR_DONE);
	} else {
		/* abort all actions on termination point */
		for (ch = lg->ch.list; ch; ch = ch->lg.next)
			while ((ac = ch->ac.list))
				ac_free_priv(ac);
		return mg_ok_ack(q, mg, p->mg_primitive);
	}
      outstate:
	err = MGOUTSTATE;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_ABORT_REQ, err);
}

/** mg_conn_req: - requests that the requested termination point be connected
  * into the communications session in the directions indicated by the
  * mg_conn_flags, with the digital padding specified and the optional topology
  * description.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_CONN_REQ primitive
  * @mg: private structure (locked)
  *
  * If the optional topology description is not included, it is assumed that the
  * termination point is requested to be connected to all other participants in
  * the communications session in the directions requested.  If the optional
  * topology description is included, it contains the list of other termination
  * points in the session to which the specified termination point is to be
  * connected in the directions requested.  If the termination point is not
  * specified (mg_tp_id is zero) it is assumed that all participants (joined
  * termination points) in the communications session are to be connected to all
  * other participants in the communication session in full mesh.
  */
STATIC int
mg_conn_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct se *se;
	struct lg *lg, *l2;
	struct ch *ch, *c2;
	struct cn *cn;
	MG_conn_req_t *p = (typeof(p)) mp->b_rptr;
	int err, single, waiting;
	ulong flags;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (!MBLKIN(mp, p->mg_topology_offset, p->m_topology_length))
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
										       p->mg_padding)))
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
										       p->mg_padding)))
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
				    ((p->mg_conn_flags & MGF_IC_DIR) ? MXF_TX_DIR : 0) |
				    ((p->mg_conn_flags & MGF_OG_DIR)
				     ? MXF_RX_DIR : 0);
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
	return mg_conn_con(q, se->mg.mg, se, lg);	/* careful, lg can be NULL */
      outstate:
	err = MGOUTSTATE;
	goto error;
      notsupp:
	err = MGNOTSUPP;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      badopt:
	err = MGBADOPT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_CONN_REQ, err);
}

/** mg_data_req: - requests that the provider send data to the communications
  * session.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_DATA_REQ primitive
  * @mg: private structure (locked)
  */
STATIC int
mg_data_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct ch *ch;
	MG_data_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (!mp->b_cont || !msgsize(mp->b_cont))
		goto eproto;
	for (ch = mg->ch.list; ch && ch->mx_slot != p->mg_mx_slot; ch = ch->mx.next) ;
	if (!ch)
		goto discard;
	return ch_write(q, mp, mp->b_cont, ch);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      eproto:
	err = -EPROTO;
	goto error;
      discard:
	freemsg(mp);
	return (0);
      error:
	return mg_error_reply(q, mp, mg, MG_DATA_REQ, err);
}

/** mg_discon_req: - requests that the termination point be disconnected from
  * the communications session in the directions indicated by the mg_conn_flags,
  * and the optional topology description (not yet supported).
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_DISCON_REQ primitive
  * @mg: private structure (locked)
  *
  * If the optional topology description is not specified, it is assumed that
  * the termination point is to be disconnected from all other participants in
  * the communications session for the directions requested.  If the optional
  * topology description is specified, it contains the list of other termination
  * points in the session from which the specified termination point is to be
  * disconnected in the direction requested.
  */
STATIC int
mg_discon_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct se *se;
	struct lg *lg;
	struct ch *ch, *c2;
	struct cn *cn;
	MG_discon_req_t *p = (typeof(p)) mp->b_rptr;
	int err, single, waiting;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (!MBLKIN(mp, p->mg_topology_offset, p->mg_topology_length))
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
						/* check if we are only connection to other end */
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
						/* check if we are only connection to other end */
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
	return mg_discon_con(q, mp, mg, se, lg, p->mg_conn_flags);	/* careful, lg can be NULL */
      outstate:
	err = MGOUTSTATE;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      badopt:
	err = MGBADOPT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      eopnotsupp:
	err = -EOPNOTSUPP;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_DISCON_REQ, err);
}

/** mg_leave_req: - requests that the specified termination point (mg_tp_id
  * non-zero) or all termination points (mg_tp_id zero) leave the specified
  * communication session.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_LEAVE_REQ primitive
  * @mg: private structure (locked)
  *
  * Once all termination points leave a communications session, the
  * communications session is destroyed.
  */
STATIC int
mg_leave_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct se *se;
	struct lg *lg = NULL;
	struct ch *ch, *c2;
	struct mx *mx;
	int disable, single, waiting = 0;
	MG_leave_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
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
		/* fall through */
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
						if ((err = mx_disable_req(q, mp, mx)) < 0)
							return (err);
						mx->state = MXS_WCON_DREQ;
						/* fall through */
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
	return mg_leave_con(q, mp, mg, se, lg);	/* careful, lg can be NULL */
      outstate:
	err = MGOUTSTATE;
	goto error;
      badid:
	err = MGBADID;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      error:
	return mg_error_reply(q, mp, mg, MG_LEAVE_REQ, err);
}

/** mg_notify_req: - requests that the provider provide event notifications to
  * the requesting stream as specified in the request.
  *
  * @q: active queue (upper write queue)
  * @mp: the MG_NOTIFY_REQ primitive
  * @mg: private structure (locked)
  */
STATIC int
mg_notify_req(queue_t *q, mblk_t *mp, struct mg *mg)
{
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
	/* write out list of session configuration following media gateway configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (se = mg->se.list; se && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     se = se->mg.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MG_OBJ_TYPE_SE;
		arg->id = se->id;
	}
	/* terminate list with zero object type */
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
	/* write out list of session configuration following media gateway configuration */
	arg = (typeof(arg)) (cnf + 1);
	chd = (typeof(chd)) (arg + 1);
	for (lg = se->lg.list; lg && size >= sizeof(*arg) + sizeof(*chd) + sizeof(*arg);
	     lg = lg->se.next, size -= sizeof(*arg) + sizeof(*chd), arg =
	     (typeof(arg)) (chd + 1), chd = (typeof(chd)) (arg + 1)) {
		arg->type = MG_OBJ_TYPE_LG;
		arg->id = lg->id;
		chd->seid = lg->se.se ? lg->se.se->id : 0;
	}
	/* terminate list with zero object type */
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
	/* write out list of session configuration following media gateway configuration */
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
	/* terminate list with zero object type */
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
	/* terminate list with zero object type */
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
	/* write out list of session configuration following media gateway configuration */
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
	/* terminate list with zero object type */
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
mg_add_mg(mg_config_t * arg, struct mg *mg, int size, int force, int test)
{
	mg_conf_mg_t *cnf = (typeof(cnf)) (arg + 1);

	if (mg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* MG objects are not added statically */
	return (-EINVAL);
}

STATIC int
mg_add_se(mg_config_t * arg, struct se *se, int size, int force, int test)
{
	mg_conf_se_t *cnf = (typeof(cnf)) (arg + 1);

	if (se || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* SE objects are not added statically */
	return (-EINVAL);
}

STATIC int
mg_add_lg(mg_config_t * arg, struct lg *lg, int size, int force, int test)
{
	mg_conf_lg_t *cnf = (typeof(cnf)) (arg + 1);

	if (lg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* LG objects are not added statically */
	return (-EINVAL);
}

STATIC int
mg_add_ch(mg_config_t * arg, struct ch *ch, int size, int force, int test)
{
	/* Channels for lower MX streams are allocated statically. */
	/* Channels for upper MG streams are allocated dynamically. */
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
	/* nothing to change */
	return (QR_DONE);
}

STATIC int
mg_cha_se(mg_config_t * arg, struct se *se, int size, int force, int test)
{
	mg_conf_se_t *cnf = (typeof(cnf)) (arg + 1);

	if (!se || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* nothing to change */
	return (QR_DONE);
}

STATIC int
mg_cha_lg(mg_config_t * arg, struct lg *lg, int size, int force, int test)
{
	mg_conf_lg_t *cnf = (typeof(cnf)) (arg + 1);

	if (!lg || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* nothing to change */
	return (QR_DONE);
}

STATIC int
mg_cha_ch(mg_config_t * arg, struct ch *ch, int size, int force, int test)
{
	mg_conf_ch_t *cnf = (typeof(cnf)) (arg + 1);

	if (!ch || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* nothing to change */
	return (QR_DONE);
}

STATIC int
mg_cha_mx(mg_config_t * arg, struct mx *mx, int size, int force, int test)
{
	mg_conf_mx_t *cnf = (typeof(cnf)) (arg + 1);

	if (!mx || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* nothing to change */
	return (QR_DONE);
}

STATIC int
mg_cha_df(mg_config_t * arg, struct df *df, int size, int force, int test)
{
	mg_conf_df_t *cnf = (typeof(cnf)) (arg + 1);

	if (!df || (size -= sizeof(*cnf)) < 0)
		return (-EINVAL);
	/* nothing to change */
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
/** __mg_ioctl: - process M_IOCTL for MG input-output control
  * @q: active queue (upper write queue)
  * @mp: the M_IOCTL message
  * @mg: private structure (locked)
  *
  * This is step 1 of the input-output contorl operation.  Step 1 consists of
  * copying in the necessary prefix structure that identifies the object type
  * and id being managed.
  */
static int
__mg_ioctl(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;
	int err = 0;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", mg_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(MG_IOCGOPTIONS):
	case _IOC_NR(MG_IOCSOPTIONS):
		size = sizeof(struct mg_option);
		break;
	case _IOC_NR(MG_IOCSCONFIG):
	case _IOC_NR(MG_IOCGCONFIG):
	case _IOC_NR(MG_IOCTCONFIG):
	case _IOC_NR(MG_IOCCCONFIG):
	case _IOC_NR(MG_IOCLCONFIG):
		size = sizeof(struct mg_config);
		break;
	case _IOC_NR(MG_IOCGSTATEM):
	case _IOC_NR(MG_IOCCMRESET):
		size = sizeof(struct mg_statem);
		break;
	case _IOC_NR(MG_IOCGSTATSP):
	case _IOC_NR(MG_IOCSSTATSP):
		size = sizeof(struct mg_statsp);
		break;
	case _IOC_NR(MG_IOCGSTATS):
	case _IOC_NR(MG_IOCCSTATS):
		size = sizeof(struct mg_stats);
		break;
	case _IOC_NR(MG_IOCGNOTIFY):
	case _IOC_NR(MG_IOCSNOTIFY):
	case _IOC_NR(MG_IOCCNOTIFY):
		size = sizeof(struct mg_notify);
		break;
	case _IOC_NR(MG_IOCCMGMT):
		size = sizeof(struct mg_mgmt);
		break;
	case _IOC_NR(MG_IOCCPASS):
		size = sizeof(struct mg_pass);
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

/** mg_copyin: - process M_IOCDATA message from copyin
  * @q: active queue (upper write queue)
  * @mp: the M_IOCDATA message
  * @mg: private structure (locked)
  * @dp: data pointer
  *
  * This is step 2 of the MG input-output control operation.  Step 2 consists of
  * copying out for GET operations, and processing an additional copy in
  * operation for SET operations.
  */
static int
mg_copyin(queue_t *q, mblk_t *mp, struct mg *mg, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp;
	void *obj;
	int size = -1;
	int err = 0;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", mg_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MG_IOCGOPTIONS):
	{
		struct mg_option *o, *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_opts(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, 0, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL)))
				*(struct mg_opt_conf_df *) o = ((struct df *) obj)->options;
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL)))
				*(struct mg_opt_conf_mg *) o = ((struct mg *) obj)->options;
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL)))
				*(struct mg_opt_conf_se *) o = ((struct se *) obj)->options;
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL)))
				*(struct mg_opt_conf_lg *) o = ((struct lg *) obj)->options;
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL)))
				*(struct mg_opt_conf_ch *) o = ((struct ch *) obj)->options;
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL)))
				*(struct mg_opt_conf_mx *) o = ((struct mx *) obj)->options;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCSOPTIONS):
	{
		struct mg_option *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_opts(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(MG_IOCSCONFIG):
	{
		struct mg_config *p = (typeof(p)) dp->b_rptr;

		if ((size = mg - size_conf(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(MG_IOCGCONFIG):
	{
		struct mg_config *o, *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_conf(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL)))
				*(struct mg_conf_df *) o = ((struct df *) obj)->config;
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL)))
				*(struct mg_conf_mg *) o = ((struct mg *) obj)->config;
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL)))
				*(struct mg_conf_se *) o = ((struct se *) obj)->config;
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL)))
				*(struct mg_conf_lg *) o = ((struct lg *) obj)->config;
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL)))
				*(struct mg_conf_ch *) o = ((struct ch *) obj)->config;
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL)))
				*(struct mg_conf_mx *) o = ((struct mx *) obj)->config;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCTCONFIG):
	{
		struct mg_config *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_conf(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(MG_IOCCCONFIG):
	{
		struct mg_config *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_conf(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(MG_IOCLCONFIG):
		/* not supported just yet */
		goto eopnotsupp;
	case _IOC_NR(MG_IOCGSTATEM):
	{
		struct mg_statem *o, *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_statem(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o = *p;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL))) {
				o->state = 0;
				*(struct mg_statem_df *) (o + 1) = ((struct df *) obj)->statem;
			}
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL))) {
				o->state = 0;
				*(struct mg_statem_mg *) (o + 1) = ((struct mg *) obj)->statem;
			}
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL))) {
				o->state = 0;
				*(struct mg_statem_se *) (o + 1) = ((struct se *) obj)->statem;
			}
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL))) {
				o->state = 0;
				*(struct mg_statem_lg *) (o + 1) = ((struct lg *) obj)->statem;
			}
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL))) {
				o->state = 0;
				*(struct mg_statem_ch *) (o + 1) = ((struct ch *) obj)->statem;
			}
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL))) {
				o->state = 0;
				*(struct mg_statem_mx *) (o + 1) = ((struct mx *) obj)->statem;
			}
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCCMRESET):
		goto eopnotsupp;
	case _IOC_NR(MG_IOCGSTATSP):
	{
		struct mg_stats *o, *p = (typeof(p)) dp->b_rptr;

		if ((size = ua_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL)))
				*(struct mg_stats_df *) o = ((struct df *) obj)->statsp;
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL)))
				*(struct mg_stats_mg *) o = ((struct mg *) obj)->statsp;
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL)))
				*(struct mg_stats_se *) o = ((struct se *) obj)->statsp;
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL)))
				*(struct mg_stats_lg *) o = ((struct lg *) obj)->statsp;
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL)))
				*(struct mg_stats_ch *) o = ((struct ch *) obj)->statsp;
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL)))
				*(struct mg_stats_mx *) o = ((struct mx *) obj)->statsp;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCSSTATSP):
	{
		struct mg_stats *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_stats(p->type, sizeof(*p))) < 0)
			goto efault;
		break;
	}
	case _IOC_NR(MG_IOCGSTATS):
	{
		struct mg_stats *o, *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL)))
				*(struct mg_stats_df *) o = ((struct df *) obj)->stats;
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL)))
				*(struct mg_stats_mg *) o = ((struct mg *) obj)->stats;
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL)))
				*(struct mg_stats_se *) o = ((struct se *) obj)->stats;
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL)))
				*(struct mg_stats_lg *) o = ((struct lg *) obj)->stats;
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL)))
				*(struct mg_stats_ch *) o = ((struct ch *) obj)->stats;
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL)))
				*(struct mg_stats_mx *) o = ((struct mx *) obj)->stats;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCCSTATS):
	{
		struct mg_stats *o, *p = (typeof(p)) dp->b_rptr;

		if ((size = mg_size_stats(p->type, sizeof(*p))) < 0)
			break;
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o++ = *p;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL))) {
				*(struct mg_stats_df *) o = ((struct df *) obj)->stats;
				bzero(&((struct df *) obj)->stats,
				      sizeof(((struct df *) obj)->stats));
			}
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL))) {
				*(struct mg_stats_mg *) o = ((struct mg *) obj)->stats;
				bzero(&((struct mg *) obj)->stats,
				      sizeof(((struct mg *) obj)->stats));
			}
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL))) {
				*(struct mg_stats_se *) o = ((struct se *) obj)->stats;
				bzero(&((struct se *) obj)->stats,
				      sizeof(((struct se *) obj)->stats));
			}
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL))) {
				*(struct mg_stats_lg *) o = ((struct lg *) obj)->stats;
				bzero(&((struct lg *) obj)->stats,
				      sizeof(((struct lg *) obj)->stats));
			}
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL))) {
				*(struct mg_stats_ch *) o = ((struct ch *) obj)->stats;
				bzero(&((struct ch *) obj)->stats,
				      sizeof(((struct ch *) obj)->stats));
			}
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL))) {
				*(struct mg_stats_mx *) o = ((struct mx *) obj)->stats;
				bzero(&((struct mx *) obj)->stats,
				      sizeof(((struct mx *) obj)->stats));
			}
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCGNOTIFY):
	{
		struct mg_notify *o, *p = (typeof(p)) dp->b_rptr;
		int size;

		size = sizeof(lmi_notify_t);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, size, false)))
			goto enobufs;
		o = (typeof(o)) bp->b_rptr;
		*o = *p;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL)))
				o->notify = ((struct df *) obj)->events;
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL)))
				o->notify = ((struct mg *) obj)->events;
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL)))
				o->notify = ((struct se *) obj)->events;
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL)))
				o->notify = ((struct lg *) obj)->events;
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL)))
				o->notify = ((struct ch *) obj)->events;
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL)))
				o->notify = ((struct mx *) obj)->events;
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCSNOTIFY):
		goto eopnotsupp;
	case _IOC_NR(MG_IOCCNOTIFY):
		goto eopnotsupp;
	case _IOC_NR(MG_IOCCMGMT):
		goto eopnotsupp;
	case _IOC_NR(MG_IOCCPASS):
	{
		struct mg_pass *p = (typeof(p)) dp->b_rptr;

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

/** mg_copyin2: - process M_IOCDATA from second copyin
  * @q: active queue (upper write queue)
  * @mp: the M_IOCDATA message
  * @mg: private structure (locked)
  *
  * This is step 3, and the final step, of the MG input-output control SET
  * operation.  Step 3 consists of processing the copied in data for a SET
  * operation and completing the input-output control.
  */
static noinline fastcall __unlikely int
mg_copyin2(queue_t *q, mblk_t *mp, struct mg *mg, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	void *obj;
	int err = 0;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", mg_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(MG_IOCSOPTIONS):
	{
		struct mg_option *p = (typeof(p)) dp->b_rptr;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL)))
				((struct df *) obj)->options = *(struct mg_opt_conf_df *) (p + 1);
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL)))
				((struct mg *) obj)->options = *(struct mg_opt_conf_mg *) (p + 1);
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL)))
				((struct se *) obj)->options = *(struct mg_opt_conf_se *) (p + 1);
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL)))
				((struct lg *) obj)->options = *(struct mg_opt_conf_lg *) (p + 1);
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL)))
				((struct ch *) obj)->options = *(struct mg_opt_conf_ch *) (p + 1);
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL)))
				((struct mx *) obj)->options = *(struct mg_opt_conf_mx *) (p + 1);
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCSCONFIG):
	{
		goto sconfig;
	}
	case _IOC_NR(MG_IOCCCONFIG):
	{
		struct mg_config *p;

	      sconfig:
		p = (typeof(p)) dp->b_rptr;
		switch (p->cmd) {
		case MG_ADD:
			switch (p->type) {
			case MG_OBJ_TYPE_DF:
				err = mg_add_object_df(mg, q, p->id, (struct mg_conf_df *) (p + 1));
				break;
			case MG_OBJ_TYPE_MG:
				err = mg_add_object_mg(mg, q, p->id, (struct mg_conf_mg *) (p + 1));
				break;
			case MG_OBJ_TYPE_SE:
				err = mg_add_object_se(mg, q, p->id, (struct mg_conf_se *) (p + 1));
				break;
			case MG_OBJ_TYPE_LG:
				err = mg_add_object_lg(mg, q, p->id, (struct mg_conf_lg *) (p + 1));
				break;
			case MG_OBJ_TYPE_CH:
				err = mg_add_object_ch(mg, q, p->id, (struct mg_conf_ch *) (p + 1));
				break;
			case MG_OBJ_TYPE_MX:
				err = mg_add_object_mx(mg, q, p->id, (struct mg_conf_mx *) (p + 1));
				break;
			default:
				err = EINVAL;
				break;
			}
			break;
		case MG_CHA:
			switch (p->type) {
			case MG_OBJ_TYPE_DF:
				err = mg_cha_object_df(mg, q, p->id, (struct mg_conf_df *) (p + 1));
				break;
			case MG_OBJ_TYPE_MG:
				err = mg_cha_object_mg(mg, q, p->id, (struct mg_conf_mg *) (p + 1));
				break;
			case MG_OBJ_TYPE_SE:
				err = mg_cha_object_se(mg, q, p->id, (struct mg_conf_se *) (p + 1));
				break;
			case MG_OBJ_TYPE_LG:
				err = mg_cha_object_lg(mg, q, p->id, (struct mg_conf_lg *) (p + 1));
				break;
			case MG_OBJ_TYPE_CH:
				err = mg_cha_object_ch(mg, q, p->id, (struct mg_conf_ch *) (p + 1));
				break;
			case MG_OBJ_TYPE_MX:
				err = mg_cha_object_mx(mg, q, p->id, (struct mg_conf_mx *) (p + 1));
				break;
			default:
				err = EINVAL;
				break;
			}
			break;
		case MG_DEL:
			switch (p->type) {
			case MG_OBJ_TYPE_DF:
				err = mg_del_object_df(mg, q, p->id, (struct mg_conf_df *) (p + 1));
				break;
			case MG_OBJ_TYPE_MG:
				err = mg_del_object_mg(mg, q, p->id, (struct mg_conf_mg *) (p + 1));
				break;
			case MG_OBJ_TYPE_SE:
				err = mg_del_object_se(mg, q, p->id, (struct mg_conf_se *) (p + 1));
				break;
			case MG_OBJ_TYPE_LG:
				err = mg_del_object_lg(mg, q, p->id, (struct mg_conf_lg *) (p + 1));
				break;
			case MG_OBJ_TYPE_CH:
				err = mg_del_object_ch(mg, q, p->id, (struct mg_conf_ch *) (p + 1));
				break;
			case MG_OBJ_TYPE_MX:
				err = mg_del_object_mx(mg, q, p->id, (struct mg_conf_mx *) (p + 1));
				break;
			default:
				err = EINVAL;
				break;
			}
			break;
		case MG_MOD:
			err = EOPNOTSUPP;
			break;
		default:
			err = EINVAL;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCTCONFIG):
	{
		struct mg_config *p = (typeof(p)) dp->b_rptr;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL))) ;
			/* FIXME: do something */
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL))) ;
			/* FIXME: do something */
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL))) ;
			/* FIXME: do something */
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL))) ;
			/* FIXME: do something */
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL))) ;
			/* FIXME: do something */
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL))) ;
			/* FIXME: do something */
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCSSTATSP):
	{
		struct mg_stats *p = (typeof(p)) dp->b_rptr;

		switch (p->type) {
		case MG_OBJ_TYPE_DF:
			if ((obj = mg_find_object_df(&err, mg, p->id, NULL)))
				((struct df *) obj)->statsp = *(struct mg_stats_df *) (p + 1);
			break;
		case MG_OBJ_TYPE_MG:
			if ((obj = mg_find_object_mg(&err, mg, p->id, NULL)))
				((struct mg *) obj)->statsp = *(struct mg_stats_mg *) (p + 1);
			break;
		case MG_OBJ_TYPE_SE:
			if ((obj = mg_find_object_se(&err, mg, p->id, NULL)))
				((struct se *) obj)->statsp = *(struct mg_stats_se *) (p + 1);
			break;
		case MG_OBJ_TYPE_LG:
			if ((obj = mg_find_object_lg(&err, mg, p->id, NULL)))
				((struct lg *) obj)->statsp = *(struct mg_stats_lg *) (p + 1);
			break;
		case MG_OBJ_TYPE_CH:
			if ((obj = mg_find_object_ch(&err, mg, p->id, NULL)))
				((struct ch *) obj)->statsp = *(struct mg_stats_ch *) (p + 1);
			break;
		case MG_OBJ_TYPE_MX:
			if ((obj = mg_find_object_mx(&err, mg, p->id, NULL)))
				((struct mx *) obj)->statsp = *(struct mg_stats_mx *) (p + 1);
			break;
		default:
			err = EFAULT;
			break;
		}
		break;
	}
	case _IOC_NR(MG_IOCCPASS):
	{
		struct mg_pass *p = (typeof(p)) dp->b_rptr;

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

/** mg_copyout: - process M_IOCDATA from copyout
  * @q: active queue (upper write queue)
  * @mp: the M_IOCDATA message
  * @mg: private structure (locked)
  * @dp: M_IOCDATA data block
  *
  * This is step3, and the final step, for the TRANSPARENT MG input-output
  * control GET operation.  Step 3 consists of acknolwedging the input-output
  * control operation with a return value.
  */
static noinline fastcall __unlikely int
mg_copyout(queue_t *q, mblk_t *mp, struct mg *mg, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

/** mg_w_iocdata: - process M_IOCDATA message
  * @q: active queue (upper write queue)
  * @mp: the M_IOCDATA message
  * @mg: private structure (locked)
  */
static noinline fastcall int
mg_w_iocdata(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *dp;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		return (0);
	case MI_COPY_CASE(MI_COPY_IN, 1):
		switch (__builtin_expect(_IOC_TYPE(cp->cp_cmd), MG_IOC_MAGIC)) {
		case MG_IOC_MAGIC:
			return mg_copyin(q, mp, mg, dp);
		default:
			mi_copy_done(q, mp, EINVAL);
			return (0);
		}
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		switch (__builtin_expect(_IOC_TYPE(cp->cp_cmd), MG_IOC_MAGIC)) {
		case MG_IOC_MAGIC:
			return mg_copyin2(q, mp, mg, dp);
		default:
			mi_copy_done(q, mp, EINVAL);
			return (0);
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (__builtin_expect(_IOC_TYPE(cp->cp_cmd), MG_IOC_MAGIC)) {
		case MG_IOC_MAGIC:
			return mg_copyout(q, mp, mg, dp);
		default:
			mi_copy_done(q, mp, EINVAL);
			return (0);
		}
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		return (0);
	}
}

/** __mg_i_ioctl: - perform I_LINK, I_UNLINK, I_PLINK or I_PUNLINK operation
  * @q: active queue (upper write queue)
  * @mp: the M_IOCTL message
  * @mg: private structure (locked)
  *
  * The driver supports I_LINK operations; however, any MX/CH Stream that is
  * linked with an I_LINK operation can only be managed by the control Stream
  * linking the lower Stream and cannot be shared across other upper Stream
  * unless configured against an MGID.
  *
  * Note that if this is not the first MX/CH Stream linked and there are running
  * users, this MX/CH Stream will not be available to them until it is
  * configured and brought into the active state.  If this is the first MX/CH
  * Stream, there cannot be running users.
  */
static noinline fastcall __unlikely int
__mg_i_ioctl(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int err;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_PLINK):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> I_PLINK");
		err = EPERM;
		if (!drv_priv(ioc->ioc_cr))
			break;
		/* fall through */
	case _IOC_NR(I_LINK):
	{
		struct linkblk *l;
		struct mx *mx;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> I_LINK");
		err = EINVAL;
		if ((l = (typeof(l)) mp->b_cont->b_rptr) == NULL)
			break;
		err = ENOMEM;
		if (!(mx = mi_open_alloc(sizeof(*mx))))
			break;
		if ((err =
		     mi_open_link(&mg_links, (caddr_t) mx, (dev_t *) &l->l_index, 0, DRVOPEN,
				  NULL)))
			break;
		err = 0;
		mi_attach(RD(l->l_qbot), (caddr_t) mx);
		mx_link(mx);
		mi_copy_done(q, mp, 0);
		break;
	}
	case _IOC_NR(I_PUNLINK):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> I_PUNLINK");
		err = EPERM;
		if (!drv_priv(ioc->ioc_cr))
			break;
		/* fall through */
	case _IOC_NR(I_UNLINK):
	{
		struct linkblk *l;
		struct mx *mx;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> I_UNLINK");
		err = EINVAL;
		if ((l = (typeof(l)) mp->b_cont->b_rptr) == NULL)
			break;
		for (mx = mi_first_ptr(&mg_links); mx && mx->sid != l->l_index;
		     mx = mi_next_ptr(&mg_links), (caddr_t) mx) ;
		err = EINVAL;
		if (!mx)
			break;
		err = 0;
		mx_unlink(mx);
		mi_detach(RD(mx->oq), (caddr_t) mx);
		mi_close_unlink(&mg_links, (caddr_t) mx);
		mi_close_free((caddr_t) mx);
		/* Should probably flush queues in case a Stream head is reattached */
		flushq(RD(l->l_qtop), FLUSHALL);
		mi_copy_done(q, mp, 0);
		break;
	}
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

/** __mg_w_ioctl: - process M_IOCTL message
  * @q: active queue (upper write queue)
  * @mp: M_IOCTL message
  * @mg: private structure (locked)
  */
static noinline fastcall int
__mg_w_ioctl(queue_t *q, mblk_t *mp, struct mg *mg)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (unlikely(!mp->b_cont)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (__builtin_expect(_IOC_TYPE(ioc->ioc_cmd), MG_IOC_MAGIC)) {
	case __SID:
		return __mg_i_ioctl(q, mp, mg);
	case MG_IOC_MAGIC:
		return __mg_ioctl(q, mp, mg);
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCACK, M_IOCNAK Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
__mx_r_iocack(queue_t *q, mblk_t *mp, struct mx *mx)
{
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

/** mg_w_other: - process upper write other message
  * @q: active queue (upper write queue)
  * @mp: the other message
  */
static noinline fastcall __unlikely int
mg_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "unhandled STREAMS message %d on upper write queue",
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

/** mg_w_ctl: - process upper write M_CTL/M_PCCTL message
  * @q: active queue (upper write queue)
  * @mp: the M_CTL/M_PCCTL message
  * @mg: private structure (locked)
  */
static noinline fastcall int
mg_w_ctl(queue_t *q, mblk_t *mp, struct mg *mg)
{
	mi_strlog(q, 0, SL_ERROR, "M_CTL/M_PCCTL message on upper write queue");
	freemsg(mp);
	return (0);
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
__mg_w_proto(queue_t *q, mblk_t *mp, struct mg *mg)
{
	int rtn;
	ulong prim = *(ulong *) mp->b_rptr;

	switch (prim) {
	case MG_DATA_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> %s", mg_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", mg_primname(prim));
		break;
	}

	switch (prim) {
	case MG_INFO_REQ:
		rtn = mg_info_req(q, mp, mg);
		break;
	case MG_OPTMGMT_REQ:
		rtn = mg_optmgmt_req(q, mp, mg);
		break;
	case MG_ATTACH_REQ:
		rtn = mg_attach_req(q, mp, mg);
		break;
	case MG_DETACH_REQ:
		rtn = mg_detach_req(q, mp, mg);
		break;
	case MG_JOIN_REQ:
		rtn = mg_join_req(q, mp, mg);
		break;
	case MG_ACTION_REQ:
		rtn = mg_action_req(q, mp, mg);
		break;
	case MG_ABORT_REQ:
		rtn = mg_abort_req(q, mp, mg);
		break;
	case MG_CONN_REQ:
		rtn = mg_conn_req(q, mp, mg);
		break;
	case MG_DATA_REQ:
		rtn = mg_data_req(q, mp, mg);
		break;
	case MG_DISCON_REQ:
		rtn = mg_discon_req(q, mp, mg);
		break;
	case MG_LEAVE_REQ:
		rtn = mg_leave_req(q, mp, mg);
		break;
	case MG_NOTIFY_REQ:
		rtn = mg_notify_req(q, mp, mg);
		break;
	default:
		rtn = mg_error_ack(q, mp, mg, prim, MGNOTSUPP);
		break;
	}
	return (rtn);
}

/*
 *  Primitives from MX to MG.
 *  -----------------------------------
 */
STATIC int
__mx_r_proto(queue_t *q, mblk_t *mp, struct mx *mx)
{
	int rtn;
	ulong prim = *(ulong *) mp->b_rptr;

	switch (prim) {
	case MX_DATA_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "MX_DATA_IND <-");
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", mx_primname(prim));
		break;
	}
	switch (prim) {
	case MX_INFO_ACK:
		rtn = mx_info_ack(q, mp, mx);
		break;
	case MX_OK_ACK:
		rtn = mx_ok_ack(q, mp, mx);
		break;
	case MX_ERROR_ACK:
		rtn = mx_error_ack(q, mp, mx);
		break;
	case MX_ENABLE_CON:
		rtn = mx_enable_con(q, mp, mx);
		break;
	case MX_CONNECT_CON:
		rtn = mx_connect_con(q, mp, mx);
		break;
	case MX_DATA_IND:
		rtn = mx_data_ind(q, mp, mx);
		break;
	case MX_DISCONNECT_IND:
		rtn = mx_disconnect_ind(q, mp, mx);
		break;
	case MX_DISCONNECT_CON:
		rtn = mx_disconnect_con(q, mp, mx);
		break;
	case MX_DISABLE_IND:
		rtn = mx_disable_ind(q, mp, mx);
		break;
	case MX_DISABLE_CON:
		rtn = mx_disable_con(q, mp, mx);
		break;
	default:
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
__mg_w_data(queue_t *q, mblk_t *mp, struct mg *mg)
{
	/* data from above */
	mi_strlog(q, STRLOGDA, SL_TRACE, "-> M_DATA");
	return mg_write(q, mp, mg);
}

STATIC int
__mx_r_data(queue_t *q, mblk_t *mp, struct mx *mx)
{
	/* data from below */
	mi_strlog(q, STRLOGDA, SL_TRACE, "M_DATA <-");
	return mx_read(q, mp, mx);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
__mx_r_error(queue_t *q, mblk_t *mp, struct mx *mx)
{
	fixme(("Unimmplemented function\n"));
	return (-EFAULT);
}

STATIC int
__mx_r_hangup(queue_t *q, mblk_t *mp, struct mx *mx)
{
	fixme(("Unimmplemented function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
/** mg_w_flush: - process upper write M_FLUSH message
  * @q: active queue (upper write queue)
  * @mp: the M_FLUSH message
  *
  * Note that messages are never queued to the upper read side queue of the
  * queue pair.  Therefore, the upper read queue does not have to be flushed.
  */
static noinline fastcall __unlikely int
mg_w_flush(queue_t *q, mblk_t *mp)
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

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
static inline fastcall int
mg_w_prim_srv(queue_t *q, mblk_t *mp, struct mg *mg)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return __mg_w_data(q, mp, mg);
	case M_PROTO:
	case M_PCPROTO:
		return __mg_w_proto(q, mp, mg);
	case M_CTL:
	case M_PCCTL:
		return __mg_w_ctl(q, mp, mg);
	case M_IOCTL:
	case M_IOCDATA:
		return __mg_w_ioctl(q, mp, mg);
	case M_FLUSH:
		return mg_w_flush(q, mp);
	default:
		return __mg_w_other(q, mp, mg);
	}
}

/** mg_w_prim_slow: - process a message requiring locking
  * @q: the active queue (upper write queue)
  * @mp: the STREAMS message
  *
  * The difference between mg_w_prim_slow() and mg_w_prim_lock() is that the
  * former only takes a read lock on the master lock structure.  This means that
  * other processors can be executing other procedures and routines, but none
  * can change the "plumbing" of the multiplexing driver.  As a result, the
  * routines invoked by this function cannot change the plumbing of the
  * multiplexing driver.
  */
static noinline fastcall int
mg_w_prim_slow(queue_t *q, mblk_t *mp)
{
	struct mg *mg;
	int rtn;

	read_lock(&master.lock);
	{
		if ((mg = (struct mg *) mi_trylock(q)) != NULL) {
			rtn = mg_w_prim_srv(q, mp, mg);
			mi_unlock((caddr_t) mg);
		} else
			rtn = -EDEADLK;
	}
	read_unlock(&master.lock);
	return (rtn);
}

/** mg_w_prim_lock: - process a message requiring locking
  * @q: the active queue (upper write queue)
  * @mp: the STREAMS message
  *
  * The difference between mg_w_prim_lock() and mg_w_prim_slow() is that the
  * former takes a full write lock on the master lock structure.  This means
  * that other processors can be executing other procedures and routines, but
  * only on other queue pairs, and only the current processor can affect changes
  * to the private structure and the plumbing of the multiplexing driver.
  */
static noinline fastcall int
mg_w_prim_lock(queue_t *q, mblk_t *mp)
{
	struct mg *mp;
	psw_t psw;
	int rtn;

	write_lock_irqsave(&master.lock, psw);
	{
		if (likely((mg = (struct mg *) mi_trylock(q)) != NULL)) {
			rtn = mg_w_prim_srv(q, mp, mg);
			mi_unlock((caddr_t) mg);
		} else {
			/* This should never happen because we are holding an exclusive lock on
			   master.lock, so nobody can be holding a lock on the queue pair. */
			rtn = -EDEADLK;
		}
	}
	write_unlock_irqrestore(&master.lock, psw);
	return (rtn);
}

static inline fastcall int
mg_w_prim_put(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
		return (-EAGAIN);
	case M_IOCTL:
	case M_IOCDATA:
		return mg_w_prim_lock(q, mp);
	case M_PCPROTO:
	case M_PCCTL:
	case M_HPDATA:
	default:
		return mg_w_prim_slow(q, mp);
	case M_FLUSH:
		return mg_w_flush(q, mp);
	}
}

static inline fastcall int
mx_r_prim_srv(queue_t *q, mblk_t *mp, struct mx *mx)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return __mx_r_data(q, mp, mx);
	case M_PROTO:
	case M_PCPROTO:
		return __mx_r_proto(q, mp, mx);
	case M_CTL:
	case M_PCCTL:
		return __mx_r_ctl(q, mp, mx);
	case M_ERROR:
		return __mx_r_error(q, mp, mx);
	case M_HANGUP:
		return __mx_r_hangup(q, mp, mx);
	case M_IOCACK:
	case M_IOCNAK:
		return __mx_r_iocack(q, mp, mx);
	default:
		return (-EFAULT);
	}
}

static noinline fastcall int
mx_r_prim_slow(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	int rtn;

	read_lock(&master.lock);
	if ((mx = (struct mx *) mi_trylock(q)) != NULL) {
		rtn = mx_r_prim_srv(q, mp, mx);
		mi_unlock((caddr_t) mx);
	} else
		rtn = -EDEADLK;
	read_unlock(&master.lock);
	return (rtn);
}

static inline fastcall int
mx_r_prim_put(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
		return (-EAGAIN);
	case M_IOCTL:
	case M_IOCDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_HPDATA:
		return mx_r_prim_slow(q, mp);
	case M_FLUSH:
		return mx_r_flush(q, mp);
	default:
		return (-EFAULT);
	}
}

/** mg_wput - upper write put procedure
  * @q: active queue
  * @mp: message to put on queue
  *
  * This is a cannonical put procedure.  Note that the best performance is
  * acheived by simply queing all non-priority messages.
  */
streamscall __hot_in int
mg_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || q->q_flag & QSVCBUSY))
	    || mg_w_prim_put(q, mp)) {
		mg_wstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;	/* must succeed */
			putq(q, mp);
		}
	}
	return (0);
}

/** mx_wsrv - upper write queue service procedure
  * @q: active queue (upper write queue)
  *
  * This is a cannonical service procedure with multiplex read locking.  Lock
  * the queue structure once, and then process multiple messages.
  */
streamscall __hot_in int
mg_wsrv(queue_t *q)
{
	struct mg *mg;
	mblk_t *mp;

	read_lock(&master.lock);
	if (likely((mg = (struct mg *) mi_trylock(q)) != NULL)) {
		while (likely((mp = getq(q)) != NULL)) {
			if (unlikely(mg_w_prim_srv(q, mp, mg))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) mg);
	}
	read_unlock(&master.lock);
	return (0);
}

/** mg_rput - upper read put procedure
  * @q: active queue (upper read queue)
  * @mp: the message to put
  *
  * The upper read queue never has messages placed on it.  Invoking the upper
  * read queue put procedure is an error.  Messages should always be placed on
  * the queue past the upper read queue.
  */
streamscall __hot_out int
mg_rput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_ERROR, "software error: %s", __FUNCTION__);
	putnext(q, mp);
	return (0);
}

/** mg_rsrv - upper read queue service procedure
  * @q: the queue to service (upper read queue)
  *
  * The upper read queue service procedure is invoked when the read queue of the
  * module above backenables the queue.  The action taken here is to simply
  * determine the queue(s) feeding this queue and explicitly enable them.
  * Taking a read lock on the master.lock protects the multiplexer linkage and also
  * protects the service procedure from the unlink procedure.
  */
streamscall __hot_out int
mg_rsrv(queue_t *q)
{
	struct mg *mg;

	read_lock(&master.lock);
	if ((mg = MG_PRIV(q)) && mg->other && mg->other->iq)
		qenable(mg->other->iq);
	mg_r_wakeup(q);
	read_unlock(&master.lock);
	return (0);
}

/** mx_wput - lower write put procedure
  * @q: active queue (lower write queue)
  * @mp: the message to put
  *
  * The lower write queue never has messages placed on it.  Invoking the lower
  * write put procedure is an error.  Messages should always be placed on the
  * queue past the lower write queue.
  */
streamscall __hot_out int
mx_wput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGERR, SL_ERROR, "software error: %s", __FUNCTION__);
	putnext(q, mp);
	return (0);
}

/** mx_wsrv - lower write queue service procedure
  * @q: the queue to service (lower write queue)
  *
  * The lower write queue service procedure is invoked when the write queue of
  * the module below backenables the queue.  The action taken here is to simply
  * determine the queue(s) feeding this queue and explicitly enable them.
  * Taking a read lock on the master.lock protects multiplexer linkage and also
  * protects the service procedure from the unlink procedure.
  */
streamscall __hot_out int
mx_wsrv(queue_t *q)
{
	struct mx *mx;

	read_lock(&master.lock);
	if ((mx = MX_PRIV(q)) && mx->other && mx->other->iq)
		qenable(mx->other->iq);
	mx_w_wakeup(q);
	read_unlock(&master.lock);
	return (0);
}

/** mx_rput - lower read put procedure
  * @q: active queue
  * @mp: message to put on queue
  *
  * This is a cannonical put procedure.
  */
static streamscall __hot_in int
mx_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || q->q_flag & QSVCBUSY))
	    || mx_r_prim_put(q, mp)) {
		mx_rstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;	/* must succeed */
			putq(q, mp);
		}
	}
	return (0);
}

/** mx_rsrv - lower read service procedure
  * @q: active queue (lower read queue)
  *
  * This is a canonical service procedure with multiplex read locking.  Lock the
  * queue structure once, and then process multiple messages.
  */
static streamscall __hot_in int
mx_rsrv(queue_t *q)
{
	struct mx *mx;
	mblk_t *mp;

	read_lock(&master.lock);
	if (likely((mx = (struct mx *) mi_trylock(q)) != NULL)) {
		while (likely((mp = getq(q)) != NULL)) {
			if (unlikely(mx_r_prim_srv(q, mp) != 0)) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				break;
			}
		}
		mi_unlock((caddr_t) mx);
	}
	read_unlock(&master.lock);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC major_t mg_majors[MG_CMAJORS] = { MG_CMAJOR_0, };

/** mg_qopen: - MG driver STREAMS open routine
  * @q: read queue of opened Stream
  * @devp: pointer to device number opened
  * @oflag: flags to the open call
  * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
  * @crp: pointer to opener's credentials
  */
STATIC streamscall int
mg_qopen(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct mg *mg, **mgp = &master.mg.list;
	psw_t psw;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		return (EIO);
	}
	/* Linux Fast-STREAMS always passes internal major device number (module id). Note also,
	   however, that strconf-sh attempts to allocate module ids that are identical to the base
	   major device number anyway. */
	/* Linux Fast-STREAMS always passes internal major device numbers (modules ids) */
	if (cmajor != DRV_ID) {
		return (ENXIO);
	}
	/* sorry, you can't open by minor device */
	if (cminor >= 2) {
		return (ENXIO);
	}
	cminor = 2;
	spin_lock_irqsave(&master.lock, psw);
	for (; *mgp; mgp = (typeof(mgp)) & (*mgp)->next) {
		major_t dmajor = (*mgp)->u.dev.cmajor;

		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*mgp)->u.dev.cminor;

			if (cminor < dminor)
				break;
			if (cminor == dminor) {
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
		spin_unlock_irqrestore(&master.lock, psw);
		return (ENXIO);
	}
	_printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(mg = mg_alloc_priv(q, mgp, devp, crp))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, psw);
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&master.lock, psw);
	return (0);
}

/** mg_qclose: - MG driver STREAMS close routine
  * @q: read queue of closing Stream
  * @oflag: flags to open call
  * @crp: pointer to closer's credentials
  */
STATIC streamscall int
mg_qclose(queue_t *q, int flag, cred_t *crp)
{
	struct mg *mg = MG_PRIV(q);

	(void) flag;
	(void) crp;
	(void) mg;
	_printd(("%s: %p: closing character device %d:%d\n", DRV_NAME, mg, mg->u.dev.cmajor,
		 mg->u.dev.cminor));
	/* make sure procedures are off */
	flushq(WR(q), FLUSHALL);
	flushq(RD(q), FLUSHALL);
	qprocsoff(q);
	mg_free_priv(mg);
	goto quit;
      quit:
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cachep_t mg_priv_cachep = NULL;
STATIC kmem_cachep_t se_priv_cachep = NULL;
STATIC kmem_cachep_t lg_priv_cachep = NULL;
STATIC kmem_cachep_t cn_priv_cachep = NULL;
STATIC kmem_cachep_t ac_priv_cachep = NULL;
STATIC kmem_cachep_t ch_priv_cachep = NULL;
STATIC kmem_cachep_t mx_priv_cachep = NULL;

STATIC int
mg_init_caches(void)
{
	if (!mg_priv_cachep
	    && !(mg_priv_cachep =
		 kmem_create_cache("mg_priv_cachep", sizeof(struct mg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mg_priv_cachep", DRV_NAME);
		goto no_mg;
	} else
		printd(("%s: initialized mg private structure cache\n", DRV_NAME));
	if (!se_priv_cachep
	    && !(se_priv_cachep =
		 kmem_create_cache("se_priv_cachep", sizeof(struct se), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate se_priv_cachep", DRV_NAME);
		goto no_se;
	} else
		printd(("%s: initialized se private structure cache\n", DRV_NAME));
	if (!lg_priv_cachep
	    && !(lg_priv_cachep =
		 kmem_create_cache("lg_priv_cachep", sizeof(struct lg), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate lg_priv_cachep", DRV_NAME);
		goto no_lg;
	} else
		printd(("%s: initialized lg private structure cache\n", DRV_NAME));
	if (!cn_priv_cachep
	    && !(cn_priv_cachep =
		 kmem_create_cache("cn_priv_cachep", sizeof(struct cn), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate cn_priv_cachep", DRV_NAME);
		goto no_cn;
	} else
		printd(("%s: initialized cn private structure cache\n", DRV_NAME));
	if (!ac_priv_cachep
	    && !(ac_priv_cachep =
		 kmem_create_cache("ac_priv_cachep", sizeof(struct ac), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ac_priv_cachep", DRV_NAME);
		goto no_ac;
	} else
		printd(("%s: initialized ac private structure cache\n", DRV_NAME));
	if (!ch_priv_cachep
	    && !(ch_priv_cachep =
		 kmem_create_cache("ch_priv_cachep", sizeof(struct ch), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ch_priv_cachep", DRV_NAME);
		goto no_ch;
	} else
		printd(("%s: initialized ch private structure cache\n", DRV_NAME));
	if (!mx_priv_cachep
	    && !(mx_priv_cachep =
		 kmem_create_cache("mx_priv_cachep", sizeof(struct mx), 0, SLAB_HWCACHE_ALIGN, NULL,
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
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mg_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mg_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mg_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mg_priv_cachep);
#endif
	}
	if (se_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(se_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy se_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed se_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(se_priv_cachep);
#endif
	}
	if (lg_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(lg_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy lg_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed lg_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(lg_priv_cachep);
#endif
	}
	if (cn_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(cn_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy cn_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed cn_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(cn_priv_cachep);
#endif
	}
	if (ac_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(ac_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ac_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ac_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(ac_priv_cachep);
#endif
	}
	if (ch_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(ch_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ch_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed ch_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(ch_priv_cachep);
#endif
	}
	if (mx_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mx_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mx_priv_cachep", __FUNCTION__);
			err = -EBUSY;
		} else
			printd(("%s: destroyed mx_priv_cachep\n", DRV_NAME));
#else
		kmem_cache_destroy(mx_priv_cachep);
#endif
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

	if ((mg = kmem_cache_alloc(mg_priv_cachep, GFP_ATOMIC))) {
		printd(("%s: %p: allocated mg private structure %d:%d\n", DRV_NAME, mg,
			getmajor(*devp), getminor(*devp)));
		bzero(mg, sizeof(*mg));
		mg->u.dev.cmajor = getmajor(*devp);
		mg->u.dev.cminor = getminor(*devp);
		mg->cred = *crp;
		spin_lock_init(&mg->qlock);	/* "mg-queue-lock" */
		(mg->iq = WR(q))->q_ptr = mg_get(mg);
		(mg->oq = RD(q))->q_ptr = mg_get(mg);
		mg->o_wakeup = mg_r_wakeup;
		mg->i_wakeup = NULL;
		mg->i_state = MGS_DETACHED;
		mg->i_style = LMI_STYLE1;
		mg->i_version = 1;
		spin_lock_init(&mg->lock);	/* "mg-priv-lock" */
		/* place in master list */
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
		/* free all channels */
		while ((ch = mg->ch.list))
			ch_free_priv(ch);
		/* free all sessions */
		while ((se = mg->se.list))
			se_free_priv(se);
		/* remove from master list */
		if ((*mg->prev = mg->next))
			mg->next->prev = mg->prev;
		mg->next = NULL;
		mg->prev = &mg->next;
		ensure(atomic_read(&mg->refcnt) > 1, mg_get(mg));
		mg_put(mg);
		ensure(master.mg.numb > 0, master.mg.numb++);
		master.mg.numb--;
		/* remove from queues */
		ensure(atomic_read(&mg->refcnt) > 1, mg_get(mg));
		mg_put((mg_t *) xchg(&(mg->iq->q_ptr), NULL));
		ensure(atomic_read(&mg->refcnt) > 1, mg_get(mg));
		mg_put((mg_t *) xchg(&(mg->oq->q_ptr), NULL));
		/* done, check final count */
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

	if ((se = kmem_cache_alloc(se_priv_cachep, GFP_ATOMIC))) {
		bzero(se, sizeof(*se));
		spin_lock_init(&se->lock);	/* "se-lock" */
		se_get(se);	/* first get */
		/* add to master list */
		if ((se->next = master.se.list))
			se->next->prev = &se->next;
		se->prev = &master.se.list;
		master.se.list = se_get(se);
		master.se.numb++;
		/* link to mg in id order */
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
		/* unlink from mg */
		if ((*se->mg.prev = se->mg.next))
			se->mg.next->mg.prev = se->mg.prev;
		se->mg.next = NULL;
		se->mg.prev = &se->mg.next;
		ensure(atomic_read(&se->refcnt) > 1, se_get(se));
		se_put(se);
		mg_put(xchg(&se->mg.mg, NULL));
		assure(mg->se.numb > 0);
		mg->se.numb--;
		/* remove from master list */
		if ((*se->prev = se->next))
			se->next->prev = se->prev;
		se->next = NULL;
		se->prev = &se->next;
		ensure(atomic_read(&se->refcnt) > 1, se_get(se));
		se_put(se);
		assure(master.se.numb > 0);
		master.se.numb--;
		/* done, check final count */
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

	if ((lg = kmem_cache_alloc(lg_priv_cachep, GFP_ATOMIC))) {
		bzero(lg, sizeof(*lg));
		spin_lock_init(&lg->lock);	/* "lg-lock" */
		lg_get(lg);	/* first get */
		/* add to master list */
		if ((lg->next = master.lg.list))
			lg->next->prev = &lg->next;
		lg->prev = &master.lg.list;
		master.lg.list = lg_get(lg);
		master.lg.numb++;
		/* add to se list */
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

		/* unlink channels */
		while ((ch = lg->ch.list)) {
			/* disconnect channel */
			while (ch->og.list)
				cn_free_priv(ch->og.list);
			while (ch->ic.list)
				cn_free_priv(ch->ic.list);
			while (ch->ac.list)
				ac_free_priv(ch->ac.list);
			/* unlink channel from lg */
			ch->lg_slot = -1UL;
			if ((*ch->lg.prev = ch->lg.next))
				ch->lg.next->lg.prev = ch->lg.prev;
			ch->lg.next = NULL;
			ch->lg.prev = &ch->lg.next;
			ch_put(ch);
			lg_put(xchg(&ch->lg.lg, NULL));
			lg->ch.numb--;
		}
		/* unlink from se */
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
			/* destroy se if we are the last leg */
			if (!(se->lg.list))
				se_free_priv(se);
		}
		/* remove from master list */
		if ((*lg->prev = lg->next))
			lg->next->prev = lg->prev;
		lg->next = NULL;
		lg->prev = &lg->next;
		lg->id = 0;
		/* done, check final count */
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
	/* Form a connection between in an incoming and outgoing leg */
	struct cn *cn;

	if ((cn = kmem_cache_alloc(cn_priv_cachep, GFP_ATOMIC))) {
		printd(("%s: %p: allocated cn private structure\n", DRV_NAME, cn));
		bzero(cn, sizeof(*cn));
		cn->state = MGS_WCON_CREQ;
		cn->flags = 0;
		cn->pad = pad;
		if (ic) {
			/* link to incoming ch */
			cn->ic.ch = ch_get(ic);
			if ((cn->ic.next = ic->ic.list))
				cn->ic.next->ic.prev = &cn->ic.next;
			cn->ic.prev = &ic->ic.list;
			ic->ic.list = cn;
			ic->ic.numb++;
		}
		if (og) {
			/* link to outgoing ch */
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
	/* free buffers */
	if (cn->buf) {
		cn->samples = 0;
		freemsg(xchg(&cn->buf, NULL));
	}
	/* unlink from ic */
	if ((ch = cn->ic.ch)) {
		if ((*cn->ic.prev = cn->ic.next))
			cn->ic.next->ic.prev = cn->ic.prev;
		cn->ic.next = NULL;
		cn->ic.prev = &cn->ic.next;
		ch->ic.numb--;
		ch_put(xchg(&cn->ic.ch, NULL));
	}
	/* unlink from og */
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

	if ((ac = kmem_cache_alloc(ac_priv_cachep, GFP_ATOMIC))) {
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
		/* link to ch */
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
		/* unlink from ch */
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

	if ((ch = kmem_cache_alloc(ch_priv_cachep, GFP_ATOMIC))) {
		bzero(ch, sizeof(*ch));
		spin_lock_init(&ch->lock);	/* "ch-lock" */
		ch_get(ch);	/* first get */
		/* add to master list */
		if ((ch->next == master.ch.list))
			ch->next->prev = &ch->next;
		ch->prev = &master.ch.list;
		master.ch.list = ch_get(ch);
		master.ch.numb++;
		if (mx) {
			struct ch **chp;

			for (chp = &mx->ch.list; (*chp) && (*chp)->mx_slot < slot;
			     (*chp) = (*chp)->mx.next) ;
			/* add to mx list at slot position */
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

		/* clear action list */
		while ((ac = ch->ac.list))
			ac_free_priv(ac);
		/* clear outgoing connection list */
		while ((cn = ch->og.list))
			cn_free_priv(cn);
		/* clear incoming ocnnection list */
		while ((cn = ch->ic.list))
			cn_free_priv(cn);
		if ((mx = ch->mx.mx)) {
			/* unlink from mx */
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
			/* unlink from lg */
			if ((*ch->lg.prev = ch->lg.next))
				ch->lg.next->lg.prev = ch->lg.prev;
			ch->lg.next = NULL;
			ch->lg.prev = &ch->lg.next;
			ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
			ch_put(ch);
			assure(lg->ch.numb > 0);
			lg->ch.numb--;
			lg_put(xchg(&ch->lg.lg, NULL));
			/* we must destroy leg now (this may destroy se too) */
			lg_free_priv(lg);
		}
		/* remove from master list */
		if ((*ch->prev = ch->next))
			ch->next->prev = ch->prev;
		ch->next = NULL;
		ch->prev = &ch->next;
		ensure(atomic_read(&ch->refcnt) > 1, ch_get(ch));
		ch_put(ch);
		assure(master.ch.numb > 0);
		master.ch.numb--;
		/* done, check final count */
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

	if ((mx = kmem_cache_alloc(mx_priv_cachep, GFP_ATOMIC))) {
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
		/* place in master list */
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
		/* free all channels */
		while ((ch = mx->ch.list))
			ch_free_priv(ch);
		/* remove from master list */
		if ((*mx->prev = mx->next))
			mx->next->prev = mx->prev;
		mx->next = NULL;
		mx->prev = &mx->next;
		ensure(atomic_read(&mx->refcnt) > 1, mx_get(mx));
		mx_put(mx);
		ensure(master.mx.numb > 0, master.mx.numb++);
		master.mx.numb--;
		/* remove from queues */
		ensure(atomic_read(&mx->refcnt) > 1, mx_get(mx));
		mx_put(xchg(&mx->iq->q_ptr, NULL));
		ensure(atomic_read(&mx->refcnt) > 1, mx_get(mx));
		mx_put(xchg(&mx->oq->q_ptr, NULL));
		/* done, check final count */
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
STATIC struct cdevsw mg_cdev = {
	.d_name = DRV_NAME,
	.d_str = &mginfo,
	.d_flag = D_MP,
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
