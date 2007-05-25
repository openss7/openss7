/*****************************************************************************

 @(#) $RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9.2.69 $) $Date: 2007/05/25 12:04:56 $

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

 Last Modified $Date: 2007/05/25 12:04:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp2.c,v $
 Revision 0.9.2.69  2007/05/25 12:04:56  brian
 - final performance tweaks

 Revision 0.9.2.68  2007/05/24 23:51:48  brian
 - nice stable performance test runs

 Revision 0.9.2.67  2007/05/22 02:10:20  brian
 - SCTP performance testing updates

 Revision 0.9.2.66  2007/05/18 12:05:25  brian
 - wrap up of sctp testing

 Revision 0.9.2.65  2007/05/18 05:02:01  brian
 - final sctp performance rework

 Revision 0.9.2.64  2007/04/12 20:06:41  brian
 - changes from performance testing and misc bug fixes

 Revision 0.9.2.63  2007/03/25 19:01:57  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.62  2007/03/25 06:00:36  brian
 - flush corrections

 Revision 0.9.2.61  2007/03/25 00:53:16  brian
 - synchronization updates

 Revision 0.9.2.60  2007/03/15 02:01:49  brian
 - last known bug fixes, report failed expectations

 Revision 0.9.2.59  2007/03/10 13:53:04  brian
 - checking in latest corrections for release

 Revision 0.9.2.58  2006/12/15 00:22:00  brian
 - bufq locking changes and test suite upgrade

 Revision 0.9.2.57  2006/12/08 05:28:08  brian
 - bufq locking change and debian init script name correction

 Revision 0.9.2.56  2006/10/30 20:26:18  brian
 - bug hunting

 Revision 0.9.2.55  2006/10/21 19:55:24  brian
 - a couple more test case corrections

 Revision 0.9.2.54  2006/10/21 12:00:18  brian
 - missing checkins

 Revision 0.9.2.53  2006/10/21 00:22:06  brian
 - corrections for zero-length messages, QR_TRIMMED tweaks

 Revision 0.9.2.52  2006/10/19 12:48:16  brian
 - corrections to ETSI SACK frequency

 Revision 0.9.2.51  2006/10/19 11:52:30  brian
 - added support for ETSI SACK frequency

 Revision 0.9.2.50  2006/10/17 12:11:28  brian
 - printf statement correction for debug compile

 Revision 0.9.2.49  2006/10/16 00:14:38  brian
 - updates for release and test case passes on UP

 Revision 0.9.2.48  2006/08/23 11:20:59  brian
 - rationalized sctp2.c to sctp package

 Revision 0.9.2.47  2006/08/07 22:17:02  brian
 - changes from SCTP Interop

 Revision 0.9.2.46  2006/07/29 07:44:07  brian
 - CVS checkin of changes before leaving for SCTP interop

 Revision 0.9.2.45  2006/07/24 09:01:40  brian
 - results of udp2 optimizations

 Revision 0.9.2.44  2006/07/16 12:46:37  brian
 - handle skb_linearize with 1 arg on recent kernels

 Revision 0.9.2.43  2006/07/15 13:06:13  brian
 - rationalized np_ip.c and rawip.c to upd.c drivers

 Revision 0.9.2.42  2006/04/22 01:10:38  brian
 - locking correction

 Revision 0.9.2.41  2006/03/27 01:25:38  brian
 - working up IP driver and SCTP testing

 Revision 0.9.2.40  2006/03/03 11:46:59  brian
 - 32/64-bit compatibility

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9.2.69 $) $Date: 2007/05/25 12:04:56 $"

static char const ident[] =
    "$RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9.2.69 $) $Date: 2007/05/25 12:04:56 $";

#define _LFS_SOURCE
#define _SVR4_SOURCE
#define _SUN_SOURCE

#include "sctp_compat.h"

#define t_tst_bit(nr,addr)  sctp_tst_bit(nr,addr)
#define t_set_bit(nr,addr)  sctp_set_bit(nr,addr)
#define t_clr_bit(nr,addr)  sctp_clr_bit(nr,addr)

#undef ETSI

#include "sctp_hooks.h"

#define SCTP_DESCRIP	"SCTP/IP STREAMS (NPI/TPI) DRIVER."
#define SCTP_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SCTP_REVISION	"OpenSS7 $RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9.2.69 $) $Date: 2007/05/25 12:04:56 $"
#define SCTP_COPYRIGHT	"Copyright (c) 1997-2007  OpenSS7 Corporation.  All Rights Reserved."
#define SCTP_DEVICE	"Supports Linux Fast-STREAMS and Linux NET4."
#define SCTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCTP_LICENSE	"GPL"
#define SCTP_BANNER	SCTP_DESCRIP	"\n" \
			SCTP_EXTRA	"\n" \
			SCTP_REVISION	"\n" \
			SCTP_COPYRIGHT	"\n" \
			SCTP_DEVICE	"\n" \
			SCTP_CONTACT
#define SCTP_SPLASH	SCTP_DESCRIP	"\n" \
			SCTP_REVISION

#ifdef LINUX
#ifdef SCTP_CONFIG_MODULE
MODULE_AUTHOR(SCTP_CONTACT);
MODULE_DESCRIPTION(SCTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCTP_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sctp");
#ifdef LFS
MODULE_ALIAS("streams-driver-sctp");
#endif
#endif
#endif				/* SCTP_CONFIG_MODULE */
#endif				/* LINUX */

#define SCTPLOGST	1	/* log SCTP state transitions */
#define SCTPLOGTO	2	/* log SCTP timeouts */
#define SCTPLOGRX	3	/* log SCTP primitives received */
#define SCTPLOGTX	4	/* log SCTP primitives issued */
#define SCTPLOGTE	5	/* log SCTP timer events */
#define SCTPLOGNO	6	/* log SCTP additional data */
#define SCTPLOGDA	7	/* log SCTP data */

#ifdef _OPTIMIZE_SPEED
#define sctplog(sp, level, flags, fmt, ...) \
	do { } while (0)
#else
#define sctplog(sp, level, flags, fmt, ...) \
	strlog(sp->cmajor, sp->cminor, level, flags, fmt, ##__VA_ARGS__)
#endif

#define sctplogerr(sp, fmt, ...) sctplog(sp, 0, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define sctplogst(sp, fmt, ...)  sctplog(sp, SCTPLOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define sctplogto(sp, fmt, ...)  sctplog(sp, SCTPLOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define sctplogrx(sp, fmt, ...)  sctplog(sp, SCTPLOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define sctplogtx(sp, fmt, ...)  sctplog(sp, SCTPLOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define sctplogte(sp, fmt, ...)  sctplog(sp, SCTPLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define sctplogno(sp, fmt, ...)  sctplog(sp, SCTPLOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define sctplogda(sp, fmt, ...)  sctplog(sp, SCTPLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)

/*
 *  =========================================================================
 *
 *  DEFAULT Protocol Values
 *
 *  =========================================================================
 */
/* ip defaults */
#define SCTP_DEFAULT_IP_TOS			(0x0)
#define SCTP_DEFAULT_IP_TTL			(64)
#define SCTP_DEFAULT_IP_PROTO			(132)
#define SCTP_DEFAULT_IP_DONTROUTE		(0)
#define SCTP_DEFAULT_IP_BROADCAST		(0)
#define SCTP_DEFAULT_IP_PRIORITY		(0)
/* per association defaults */
#define SCTP_DEFAULT_RMEM			(1<<15)	/* XXX */
#define SCTP_DEFAULT_MAX_ISTREAMS		(33)
#define SCTP_DEFAULT_REQ_OSTREAMS		(1)
#define SCTP_DEFAULT_MAX_INIT_RETRIES		(8)
#define SCTP_DEFAULT_ASSOC_MAX_RETRANS		(10)
#define SCTP_DEFAULT_VALID_COOKIE_LIFE		(60*HZ)
#define SCTP_DEFAULT_COOKIE_INC			(1*HZ)
#define SCTP_DEFAULT_MAC_TYPE			(0)
#define SCTP_DEFAULT_CSUM_TYPE			(0)
#define SCTP_DEFAULT_THROTTLE_ITVL		(50*HZ/1000)
#define SCTP_DEFAULT_SID			(0)
#define SCTP_DEFAULT_PPI			(0)
#define SCTP_DEFAULT_MAX_SACK_DELAY		(200*HZ/1000)
#ifdef ETSI
#define SCTP_DEFAULT_SACK_FREQUENCY		(2)
#endif
#define SCTP_DEFAULT_MAX_BURST			(4)
#define SCTP_DEFAULT_ADAPTATION_LAYER_INFO	(0)
#define SCTP_DEFAULT_PARTIAL_RELIABILITY	(0)
/* per destination defaults */
#define SCTP_DEFAULT_PATH_MAX_RETRANS		(5)
#define SCTP_DEFAULT_RTO_INITIAL		(3*HZ)
#define SCTP_DEFAULT_RTO_MIN			(1*HZ)
#define SCTP_DEFAULT_RTO_MAX			(60*HZ)
#define SCTP_DEFAULT_HEARTBEAT_ITVL		(30*HZ)

#ifndef SK_RMEM_MAX
#define SK_RMEM_MAX 65535
#endif
#ifndef SK_WMEM_MAX
#define SK_WMEM_MAX 65535
#endif
#define xti_default_debug		{ 0, }
#define xti_default_linger		(struct t_linger){T_YES, 120}
#define xti_default_rcvbuf		SK_RMEM_MAX
#define xti_default_rcvlowat		1
#define xti_default_sndbuf		SK_WMEM_MAX
#define xti_default_sndlowat		1

#define ip_default_options		{ 0, }
#define ip_default_tos			0
#define ip_default_ttl			64
#define ip_default_reuseaddr		T_NO
#define ip_default_dontroute		T_NO
#define ip_default_broadcast		T_NO

#define sctp_default_nodelay		T_NO
#define sctp_default_maxseg		536
#define sctp_default_cork		T_NO
#define sctp_default_ppi		0
#define sctp_default_sid		0
#define sctp_default_ssn		T_UNSPEC
#define sctp_default_tsn		T_UNSPEC
#define sctp_default_recvopt		T_YES
#define sctp_default_cookie_life	60	/* seconds */
#define sctp_default_sack_delay		200	/* milliseconds */
#ifdef ETSI
#define sctp_default_sack_frequency	2
#endif
#define sctp_default_path_max_retrans	5
#define sctp_default_assoc_max_retrans	10
#define sctp_default_max_init_retries	8
#define sctp_default_heartbeat_itvl	30	/* seconds */
#define sctp_default_rto_initial	3000	/* milliseconds */
#define sctp_default_rto_min		1000	/* milliseconds */
#define sctp_default_rto_max		60000	/* milliseconds */
#define sctp_default_ostreams		1
#define sctp_default_istreams		33
#define sctp_default_cookie_inc		1000	/* milliseconds */
#define sctp_default_throttle_itvl	50	/* milliseconds */
#ifdef SCTP_CONFIG_HMAC_MD5
#define sctp_default_mac_type		T_SCTP_HMAC_MD5
#else
#ifdef SCTP_CONFIG_HMAC_SHA1
#define sctp_default_mac_type		T_SCTP_HMAC_SHA1
#else
#define sctp_default_mac_type		T_SCTP_HMAC_NONE
#endif
#endif
#if defined SCTP_CONFIG_CRC_32C
#define sctp_default_cksum_type		T_SCTP_CSUM_CRC32C
#else
#define sctp_default_cksum_type		T_SCTP_CSUM_ADLER32
#endif
#define sctp_default_hb			(struct t_sctp_hb){ INADDR_ANY, T_YES, sctp_default_heartbeat_itvl }
#define sctp_default_rto		(struct t_sctp_rto){ INADDR_ANY, sctp_default_rto_initial, sctp_default_rto_min, sctp_default_rto_max, sctp_default_path_max_retrans }
#define sctp_default_sctp_status	(struct t_sctp_status){ 0, }
#define sctp_default_dest_status	(struct t_sctp_dest_status){ INADDR_ANY, }
#define sctp_default_status		{ { 0, }, { INADDR_ANY, } }
#define sctp_default_debug		0
#if defined SCTP_CONFIG_ECN
#define sctp_default_ecn		T_YES
#else
#define sctp_default_ecn		T_NO
#endif
#define sctp_default_ali		0
#define sctp_default_add		T_NO
#define sctp_default_set		T_NO
#define sctp_default_add_ip		(struct sockaddr_in){AF_INET, 0, {INADDR_ANY}}
#define sctp_default_del_ip		(struct sockaddr_in){AF_INET, 0, {INADDR_ANY}}
#define sctp_default_set_ip		(struct sockaddr_in){AF_INET, 0, {INADDR_ANY}}
#define sctp_default_pr			T_NO
#define sctp_default_lifetime		T_UNSPEC
#define sctp_default_disposition	T_UNSPEC
#define sctp_default_max_burst		4

enum {
	_T_BIT_XTI_DEBUG = 0,
	_T_BIT_XTI_LINGER,
	_T_BIT_XTI_RCVBUF,
	_T_BIT_XTI_RCVLOWAT,
	_T_BIT_XTI_SNDBUF,
	_T_BIT_XTI_SNDLOWAT,
	_T_BIT_IP_OPTIONS,
	_T_BIT_IP_TOS,
	_T_BIT_IP_TTL,
	_T_BIT_IP_REUSEADDR,
	_T_BIT_IP_DONTROUTE,
	_T_BIT_IP_BROADCAST,
	_T_BIT_IP_ADDR,
	_T_BIT_SCTP_NODELAY,
	_T_BIT_SCTP_MAXSEG,
	_T_BIT_SCTP_CORK,
	_T_BIT_SCTP_PPI,
	_T_BIT_SCTP_SID,
	_T_BIT_SCTP_SSN,
	_T_BIT_SCTP_TSN,
	_T_BIT_SCTP_RECVOPT,
	_T_BIT_SCTP_COOKIE_LIFE,
	_T_BIT_SCTP_SACK_DELAY,
#ifdef ETSI
	_T_BIT_SCTP_SACK_FREQUENCY,
#endif
	_T_BIT_SCTP_PATH_MAX_RETRANS,
	_T_BIT_SCTP_ASSOC_MAX_RETRANS,
	_T_BIT_SCTP_MAX_INIT_RETRIES,
	_T_BIT_SCTP_HEARTBEAT_ITVL,
	_T_BIT_SCTP_RTO_INITIAL,
	_T_BIT_SCTP_RTO_MIN,
	_T_BIT_SCTP_RTO_MAX,
	_T_BIT_SCTP_OSTREAMS,
	_T_BIT_SCTP_ISTREAMS,
	_T_BIT_SCTP_COOKIE_INC,
	_T_BIT_SCTP_THROTTLE_ITVL,
	_T_BIT_SCTP_MAC_TYPE,
	_T_BIT_SCTP_CKSUM_TYPE,
	_T_BIT_SCTP_HB,
	_T_BIT_SCTP_RTO,
	_T_BIT_SCTP_STATUS,
	_T_BIT_SCTP_DEBUG,
	_T_BIT_SCTP_ECN,
	_T_BIT_SCTP_ALI,
	_T_BIT_SCTP_ADD,
	_T_BIT_SCTP_SET,
	_T_BIT_SCTP_ADD_IP,
	_T_BIT_SCTP_DEL_IP,
	_T_BIT_SCTP_SET_IP,
	_T_BIT_SCTP_PR,
	_T_BIT_SCTP_LIFETIME,
	_T_BIT_SCTP_DISPOSITION,
	_T_BIT_SCTP_MAX_BURST,
};

#if 0
/* ip defaults */
STATIC uint sctp_default_ip_tos = SCTP_DEFAULT_IP_TOS;
STATIC uint sctp_default_ip_ttl = SCTP_DEFAULT_IP_TTL;
STATIC uint sctp_default_ip_proto = SCTP_DEFAULT_IP_PROTO;
STATIC uint sctp_default_ip_dontroute = SCTP_DEFAULT_IP_DONTROUTE;
STATIC uint sctp_default_ip_broadcast = SCTP_DEFAULT_IP_BROADCAST;
STATIC uint sctp_default_ip_priority = SCTP_DEFAULT_IP_PRIORITY;

/* per association defaults */
STATIC size_t sctp_default_rmem = SCTP_DEFAULT_RMEM;
STATIC size_t sctp_default_max_istreams = SCTP_DEFAULT_MAX_ISTREAMS;
STATIC size_t sctp_default_req_ostreams = SCTP_DEFAULT_REQ_OSTREAMS;
STATIC size_t sctp_default_max_init_retries = SCTP_DEFAULT_MAX_INIT_RETRIES;
STATIC size_t sctp_default_assoc_max_retrans = SCTP_DEFAULT_ASSOC_MAX_RETRANS;
STATIC size_t sctp_default_valid_cookie_life = SCTP_DEFAULT_VALID_COOKIE_LIFE;
STATIC size_t sctp_default_cookie_inc = SCTP_DEFAULT_COOKIE_INC;
STATIC size_t sctp_default_mac_type = SCTP_DEFAULT_MAC_TYPE;
STATIC size_t sctp_default_csum_type = SCTP_DEFAULT_CSUM_TYPE;
STATIC size_t sctp_default_throttle_itvl = SCTP_DEFAULT_THROTTLE_ITVL;
STATIC size_t sctp_default_default_sid = SCTP_DEFAULT_SID;
STATIC size_t sctp_default_default_ppi = SCTP_DEFAULT_PPI;
STATIC size_t sctp_default_max_sack_delay = SCTP_DEFAULT_MAX_SACK_DELAY;

#ifdef ETSI
STATIC size_t sctp_default_sack_frequency = SCTP_DEFAULT_SACK_FREQUENCY;
#endif
STATIC size_t sctp_default_max_burst = SCTP_DEFAULT_MAX_BURST;
STATIC size_t sctp_default_adaptation_layer_info = SCTP_DEFAULT_ADAPTATION_LAYER_INFO;
STATIC size_t sctp_default_partial_reliability = SCTP_DEFAULT_PARTIAL_RELIABILITY;

/* per destination defaults */
STATIC size_t sctp_default_rto_initial = SCTP_DEFAULT_RTO_INITIAL;
STATIC size_t sctp_default_rto_min = SCTP_DEFAULT_RTO_MIN;
STATIC size_t sctp_default_rto_max = SCTP_DEFAULT_RTO_MAX;
STATIC size_t sctp_default_path_max_retrans = SCTP_DEFAULT_PATH_MAX_RETRANS;
STATIC size_t sctp_default_heartbeat_itvl = SCTP_DEFAULT_HEARTBEAT_ITVL;
#endif

/*
 *  =========================================================================
 *
 *  DATA STRUCTURES
 *
 *  =========================================================================
 */
struct sctp;
typedef struct sctp sctp_t;
struct sctp_tcb;
typedef struct sctp_tcb sctp_tcb_t;
struct sctp_bind_bucket {
	unsigned short port;
	unsigned short fastreuse;
	struct sctp_bind_bucket *next;
	sctp_t *owners;
	struct sctp_bind_bucket **prev;
};
struct sctp_dup {
	struct sctp_dup *next;
	struct sctp_dup **prev;
	uint32_t tsn;
};
struct sctp_strm {
	struct sctp_strm *next;		/* linkage to stream list */
	struct sctp_strm **prev;	/* linkage to stream list */
	sctp_t *sp;
	uint16_t sid;			/* stream identifier */
	uint16_t ssn;			/* stream sequence number */
	struct {
		uint32_t ppi;		/* payload protocol id */
		mblk_t *head;		/* head pointer */
		uint more;		/* more data in (E)TSDU */
	} x, n;				/* expedited (x) and normal (n) */
};

/*
 *  Stream flags.
 */
#define SCTP_STRMF_MORE		0x00000001	/* STRM more data in (E)SDU */
#define SCTP_STRMF_DROP		0x00008000	/* STRM dropping */
struct sctp_saddr {
	struct sctp_saddr *next;	/* linkage for srce address list */
	struct sctp_saddr **prev;	/* linkage for srce address list */
	sctp_t *sp;			/* linkage for srce address list */
	uint32_t saddr;			/* srce address (network order) */
	uint flags;			/* flags for this source */
};

/*
 * Source flags.
 */
#define SCTP_SRCEF_ADD_REQUEST	0x0001	/* SRCE add requested */
#define SCTP_SRCEF_DEL_REQUEST	0x0002	/* SRCE del requested */
#define SCTP_SRCEF_SET_REQUEST	0x0004	/* SRCE set requested */
#define SCTP_SRCEF_ADD_PENDING	0x0008	/* SRCE add pending */
#define SCTP_SRCEF_DEL_PENDING	0x0010	/* SRCE del pending */
#define SCTP_SRCEF_SET_PENDING	0x0020	/* SRCE set pending */
#define SCTP_SRCEM_ASCONF	(SCTP_SRCEF_ADD_REQUEST| \
				 SCTP_SRCEF_DEL_REQUEST| \
				 SCTP_SRCEF_ADD_PENDING| \
				 SCTP_SRCEF_DEL_PENDING)
struct sctp_daddr {
	struct sctp_daddr *next;	/* linkage for dest address list */
	struct sctp_daddr **prev;	/* linkage for dest address list */
	sctp_t *sp;			/* linkage for dest address list */
	uint32_t daddr;			/* dest address (network order) */
	uint32_t saddr;			/* srce address (network order) */
	atomic_t refcnt;		/* reference count */
	uint dif;			/* device interface */
	uint flags;			/* flags for this destination */
	size_t header_len;		/* header length */
	size_t mtu;			/* mtu */
	size_t dmps;			/* dest max payload size */
	uint hb_onoff;			/* activation of heartbeats */
	uint hb_fill;			/* fill for heartbeat (PMTUDISC) */
	size_t in_flight;		/* bytes in flight */
	size_t retransmits;		/* retransmits this dest */
	size_t max_retrans;		/* max path retransmits */
	size_t dups;			/* number of duplicates */
	size_t cwnd;			/* congestion window */
	size_t ssthresh;		/* slow start threshold */
	long timers;			/* deferred timer flags */
	volatile toid_t timer_heartbeat;/* heartbeat timer (for acks) */
	volatile toid_t timer_retrans;	/* retrans (RTO) timer */
	volatile toid_t timer_idle;	/* idle timer */
	ulong when;			/* last time transmitting */
	size_t partial_ack;		/* partial window bytes acked */
	size_t ack_accum;		/* accumulator for acks */
	ulong hb_time;			/* time of last heartbeat sent */
	ulong hb_itvl;			/* interval between heartbeats */
	ulong hb_act;			/* heartbeat activation */
	ulong rto_ini;			/* initial RTO value */
	ulong rto_max;			/* maximum RTO value */
	ulong rto_min;			/* minimum RTO value */
	ulong rto;			/* current RTO value */
	ulong rttvar;			/* current RTT variance */
	ulong srtt;			/* current smoothed RTT */
	int route_caps;			/* route capabilities */
	struct dst_entry *dst_cache;	/* destination cache */
	size_t packets;			/* packet count */
};

/*
 *  Destination flags.
 */
#define SCTP_DESTF_INACTIVE	0x0001	/* DEST is inactive */
#define SCTP_DESTF_HBACTIVE	0x0002	/* DEST heartbeats */
#define SCTP_DESTF_TIMEDOUT	0x0004	/* DEST timeout occured */
#define SCTP_DESTF_PMTUDISC	0x0008	/* DEST pmtu discovered */
#define SCTP_DESTF_ROUTFAIL	0x0010	/* DEST routing failure */
#define SCTP_DESTF_UNUSABLE	0x0020	/* DEST unusable */
#define SCTP_DESTF_CONGESTD	0x0040	/* DEST congested */
#define SCTP_DESTF_DROPPING	0x0080	/* DEST is dropping packets */
#define SCTP_DESTF_FORWDTSN	0x0100	/* DEST has forward tsn outstanding */
#define SCTP_DESTF_NEEDVRFY	0x0200	/* DEST needs to be verified SCTP IG 2.36 */
#define SCTP_DESTF_ELIGIBLE	0x0400	/* DEST is eligible for FR */
#define SCTP_DESTM_DONT_USE	(SCTP_DESTF_INACTIVE| \
				 SCTP_DESTF_UNUSABLE| \
				 SCTP_DESTF_ROUTFAIL| \
				 SCTP_DESTF_CONGESTD| \
				 SCTP_DESTF_TIMEDOUT)
#define SCTP_DESTM_CANT_USE	(SCTP_DESTF_UNUSABLE| \
				 SCTP_DESTF_ROUTFAIL)
struct t_sctp_status_pair {
	struct t_sctp_status status;
	struct t_sctp_dest_status dest_status;
};

struct sctp_options {
	unsigned long flags[2];		/* at least 2 long words of flags for 64 flags */
	struct {
		t_uscalar_t debug[4];	/* XTI_DEBUG */
		struct t_linger linger;	/* XTI_LINGER */
		t_uscalar_t rcvbuf;	/* XTI_RCVBUF */
		t_uscalar_t rcvlowat;	/* XTI_RCVLOWAT */
		t_uscalar_t sndbuf;	/* XTI_SNDBUF */
		t_uscalar_t sndlowat;	/* XTI_SNDLOWAT */
	} xti;
	struct {
		unsigned char options[40];	/* T_IP_OPTIONS */
		unsigned char tos;	/* T_IP_TOS */
		unsigned char ttl;	/* T_IP_TTL */
		unsigned int reuseaddr;	/* T_IP_REUSEADDR */
		unsigned int dontroute;	/* T_IP_DONTROUTE */
		unsigned int broadcast;	/* T_IP_BROADCAST */
		uint32_t addr;		/* T_IP_ADDR */
	} ip;
	struct {
		t_uscalar_t nodelay;	/* T_SCTP_NODELAY */
		t_uscalar_t maxseg;	/* T_SCTP_MAXSEG */
		t_uscalar_t cork;	/* T_SCTP_CORK */
		t_uscalar_t ppi;	/* T_SCTP_PPI */
		t_uscalar_t sid;	/* T_SCTP_SID */
		t_uscalar_t ssn;	/* T_SCTP_SSN */
		t_uscalar_t tsn;	/* T_SCTP_TSN */
		t_uscalar_t recvopt;	/* T_SCTP_RECVOPT */
		t_uscalar_t cookie_life;	/* T_SCTP_COOKIE_LIFE */
		t_uscalar_t sack_delay;	/* T_SCTP_SACK_DELAY */
#ifdef ETSI
		t_uscalar_t sack_freq;	/* T_SCTP_SACK_FREQUENCY */
#endif
		t_uscalar_t path_max_retrans;	/* T_SCTP_PATH_MAX_RETRANS */
		t_uscalar_t assoc_max_retrans;	/* T_SCTP_ASSOC_MAX_RETRANS */
		t_uscalar_t max_init_retries;	/* T_SCTP_MAX_INIT_RETRIES */
		t_uscalar_t heartbeat_itvl;	/* T_SCTP_HEARTBEAT_ITVL */
		t_uscalar_t rto_initial;	/* T_SCTP_RTO_INITIAL */
		t_uscalar_t rto_min;	/* T_SCTP_RTO_MIN */
		t_uscalar_t rto_max;	/* T_SCTP_RTO_MAX */
		t_uscalar_t ostreams;	/* T_SCTP_OSTREAMS */
		t_uscalar_t istreams;	/* T_SCTP_ISTREAMS */
		t_uscalar_t cookie_inc;	/* T_SCTP_COOKIE_INC */
		t_uscalar_t throttle_itvl;	/* T_SCTP_THROTTLE_ITVL */
		t_uscalar_t mac_type;	/* T_SCTP_MAC_TYPE */
		t_uscalar_t cksum_type;	/* T_SCTP_CKSUM_TYPE */
		struct t_sctp_hb hb;	/* T_SCTP_HB */
		struct t_sctp_rto rto;	/* T_SCTP_RTO */
		struct t_sctp_status_pair status;	/* T_SCTP_STATUS */
		t_uscalar_t debug;	/* T_SCTP_DEBUG */
#if defined SCTP_CONFIG_ECN
		t_uscalar_t ecn;	/* T_SCTP_ECN */
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
		t_uscalar_t ali;	/* T_SCTP_ALI */
#endif
#if defined SCTP_CONFIG_ADD_IP
		t_uscalar_t add;	/* T_SCTP_ADD */
		t_uscalar_t set;	/* T_SCTP_SET */
		struct sockaddr_in add_ip;	/* T_SCTP_ADD_IP */
		struct sockaddr_in del_ip;	/* T_SCTP_DEL_IP */
		struct sockaddr_in set_ip;	/* T_SCTP_SET_IP */
#endif
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
		t_uscalar_t pr;		/* T_SCTP_PR */
#endif
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
		t_uscalar_t lifetime;	/* T_SCTP_LIFETIME */
#endif
		t_uscalar_t disposition;	/* T_SCTP_DISPOSITION */
		t_uscalar_t max_burst;	/* T_SCTP_MAX_BURST */
	} sctp;
};

STATIC struct sctp_options sctp_defaults = {
	{0,}
	,
	{
	 xti_default_debug,
	 xti_default_linger,
	 xti_default_rcvbuf,
	 xti_default_rcvlowat,
	 xti_default_sndbuf,
	 xti_default_sndlowat,
	 }
	,
	{
	 ip_default_options,
	 ip_default_tos,
	 ip_default_ttl,
	 ip_default_reuseaddr,
	 ip_default_dontroute,
	 ip_default_broadcast,
	 }
	,
	{
	 sctp_default_nodelay,
	 sctp_default_maxseg,
	 sctp_default_cork,
	 sctp_default_ppi,
	 sctp_default_sid,
	 sctp_default_ssn,
	 sctp_default_tsn,
	 sctp_default_recvopt,
	 (sctp_default_cookie_life * HZ),	/* XXX */
	 (sctp_default_sack_delay * HZ / 1000),	/* XXX */
#ifdef ETSI
	 sctp_default_sack_frequency,
#endif
	 sctp_default_path_max_retrans,
	 sctp_default_assoc_max_retrans,
	 sctp_default_max_init_retries,
	 (sctp_default_heartbeat_itvl * HZ),	/* XXX */
	 (sctp_default_rto_initial * HZ / 1000),	/* XXX */
	 (sctp_default_rto_min * HZ / 1000),	/* XXX */
	 (sctp_default_rto_max * HZ / 1000),	/* XXX */
	 sctp_default_ostreams,
	 sctp_default_istreams,
	 (sctp_default_cookie_inc * HZ / 1000),	/* XXX */
	 (sctp_default_throttle_itvl * HZ / 1000),	/* XXX */
	 sctp_default_mac_type,
	 sctp_default_cksum_type,
	 sctp_default_hb,
	 sctp_default_rto,
	 sctp_default_status,
	 sctp_default_debug,
#if defined SCTP_CONFIG_ECN
	 sctp_default_ecn,
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	 sctp_default_ali,
#endif
#if defined SCTP_CONFIG_ADD_IP
	 sctp_default_add,
	 sctp_default_set,
	 sctp_default_add_ip,
	 sctp_default_del_ip,
	 sctp_default_set_ip,
#endif
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
	 sctp_default_pr,
#endif
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
	 sctp_default_lifetime,
#endif
	 sctp_default_disposition,
	 sctp_default_max_burst,
	 }
};

#define t_defaults sctp_defaults

/*
 *  Foolish me, I thought that this structure was stable...
 *  Define the transitional (Linux 2.6.10) version
 */
#undef inet_opt
#define inet_opt    my_inet_opt

struct inet_opt {
	int uc_ttl;
	int tos;
	__u16 id;
};

struct sctp_ifops;			/* interface operations */

/*
 *  Primary data structure.
 */
struct sctp {
	sctp_t *next;			/* linkage for master list */
	sctp_t **prev;			/* linkage for master list */
	major_t cmajor;			/* major device number */
	minor_t cminor;			/* minor device number */
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	cred_t cred;			/* credentials of opener */
	atomic_t refcnt;		/* reference count */
	spinlock_t qlock;		/* queue lock */
	long users;			/* lock holders */
	wait_queue_head_t waitq;	/* sleepers on lock */
	int bcid;			/* bufcall id */
//      t_uscalar_t i_flags;            /* Interface flags */
	volatile t_uscalar_t i_state;	/* Interface state */
	volatile t_uscalar_t state;	/* SCTP state */
	sctp_t *bnext;			/* linkage for bind hash */
	sctp_t **bprev;			/* linkage for bind hash */
	struct sctp_bind_bucket *bindb;	/* linkage for bind hash */
	sctp_t *lnext;			/* linkage for list hash */
	sctp_t **lprev;			/* linkage for list hash */
	sctp_t *pnext;			/* linkage for ptag hash */
	sctp_t **pprev;			/* linkage for ptag hash */
	sctp_t *vnext;			/* linkage for vtag hash */
	sctp_t **vprev;			/* linkage for vtag hash */
	sctp_t *tnext;			/* linkage for tcb hash */
	sctp_t **tprev;			/* linkage for tcb hash */
	uint8_t hashent;		/* vtag cache entry */
	uint8_t nonagle;		/* Nagle setting */
	struct sctp_ifops *ops;		/* interface operations */
	bufq_t rcvq;			/* read queue */
	bufq_t sndq;			/* write queue */
	uint route_caps;		/* routing capabilities */
	uint protocol;			/* IP protocol number */
	uint reuse;			/* IP reuseaddr */
	uint localroute;		/* IP localroute */
	uint broadcast;			/* IP broadcast */
	uint priority;			/* IP priority */
	unsigned short num;		/* port number (host byte order) */
	uint16_t sport;			/* loc port (network byte order) */
	uint16_t dport;			/* rem port (network byte order) */
	uint userlocks;			/* user specification locks */
	struct sctp_saddr *saddr;	/* list of loc addresses */
	struct sctp_daddr *daddr;	/* list of rem addresses */
	uint16_t sanum;			/* number of srce addresses in list */
	uint16_t danum;			/* number of dest addresses in list */
	struct sctp_daddr *taddr;	/* primary transmit dest address */
	struct sctp_daddr *raddr;	/* retransmission dest address */
	struct sctp_daddr *caddr;	/* last received dest address */
	struct sctp_strm *ostrm;	/* list of outbound streams */
	struct sctp_strm *istrm;	/* list if inbound streams */
	uint16_t osnum;			/* number of outbound stream struct */
	uint16_t isnum;			/* number of inbound stream struct */
	struct sctp_strm *ostr;		/* current output stream */
	struct sctp_strm *istr;		/* current input stream */
	ulong max_sack;			/* maximum sack delay */
#ifdef ETSI
	ulong sack_freq;		/* sack frequency */
#endif
	ulong throttle;			/* throttle init interval */
	ulong life;			/* data lifetime */
	uint8_t disp;			/* data disposition */
	uint8_t prel;			/* partial reliability */
	ulong hb_rcvd;			/* hb received timestamp */
	mblk_t *retry;			/* msg to retry on timer expiry */
	bufq_t conq;			/* connect queue */
	size_t conind;			/* max number outstanding conn_inds */
	bufq_t expq;			/* expedited queue */
	bufq_t urgq;			/* urgent queue */
	bufq_t errq;			/* error queue */
	bufq_t oooq;			/* out of order queue */
	sctp_tcb_t *gaps;		/* gaps acks for this stream */
	size_t ngaps;			/* number of gap reports in list */
	size_t nunds;			/* number of undelivered in list */
	bufq_t dupq;			/* duplicate queue */
	sctp_tcb_t *dups;		/* dup tsns for this stream */
	size_t ndups;			/* number of dup reports in list */
	bufq_t rtxq;			/* retransmit queue */
	size_t nrtxs;			/* number of retransmits in list */
	size_t nsack;			/* number of sacked in list */
	bufq_t ackq;			/* pending acknowledgement queue */
	uint16_t n_ostr;		/* number of outbound streams */
	uint16_t n_istr;		/* number of inbound streams */
	uint32_t v_tag;			/* locl verification tag */
	uint32_t p_tag;			/* peer verification tag */
	uint32_t p_rwnd;		/* peer receive window */
	uint32_t p_rbuf;		/* send receive buffer */
	uint32_t a_rwnd;		/* advertized receive window */
	uint32_t m_tsn;			/* manshall TSN */
	uint32_t t_tsn;			/* transmit TSN */
	uint32_t t_ack;			/* transmit TSN cum acked */
	uint32_t r_ack;			/* received TSN cum acked */
	uint32_t l_fsn;			/* local advanced TSN */
	uint32_t p_fsn;			/* peer advanced TSN */
	uint32_t l_lsn;			/* local lowest ECN TSN */
	uint32_t p_lsn;			/* peer lowest ECN TSN */
	uint sackcnt;			/* count of SACKS delayed */
	uint sackf;			/* sack flags for association */
	uint flags;			/* flags */
	uint pmtu;			/* path MTU for association */
	uint amps;			/* assoc max payload size */
	int ext_header_len;		/* extra header length for options */
	size_t in_flight;		/* number of bytes in flight */
	size_t retransmits;		/* number of retransmits this assoc */
	size_t max_retrans;		/* max association retransmits */
	size_t max_burst;		/* max number of burst packets */
	/* ------------------------------------------------------------------ */

	/* ------------------------------------------------------------------ */
	struct inet_opt inet;		/* inet options */
	uint cmsg_flags;		/* flags */
	long timers;			/* deferred timer flags */
	volatile toid_t timer_init;	/* init timer */
	volatile toid_t timer_cookie;	/* cookie timer */
	volatile toid_t timer_shutdown;	/* shutdown timer */
	volatile toid_t timer_guard;	/* shutdown guard timer */
	volatile toid_t timer_sack;	/* sack timer */
	volatile toid_t timer_asconf;	/* asconf timer */
	volatile toid_t timer_life;	/* lifetime timer */
	uint8_t hmac;			/* hmac type */
	uint8_t cksum;			/* checksum type */
	ulong hb_tint;			/* hb throttle interval */
	uint16_t sid;			/* default sid */
	uint32_t ppi;			/* default ppi */
	ulong ck_life;			/* valid cookie life */
	ulong ck_inc;			/* cookie increment */
	uint8_t l_caps;			/* local capabilities */
	uint8_t p_caps;			/* peer capabilities */
	mblk_t *reply;			/* saved reply to ASCONF chunk */
	uint32_t l_asn;			/* local ASCONF sequence number */
	uint32_t p_asn;			/* peer ASCONF sequence number */
	uint32_t l_ali;			/* local adaptation layer info */
	uint32_t p_ali;			/* peer adaptation layer info */
	uint16_t req_ostr;		/* requested outbound streams */
	uint16_t max_istr;		/* maximum inbound streams */
	size_t max_inits;		/* max init retransmits */
	ulong rto_ini;			/* default rto initial */
	ulong rto_min;			/* default rto minimum */
	ulong rto_max;			/* default rto maximum */
	size_t rtx_path;		/* default path max retrans */
	ulong hb_itvl;			/* default hb interval */
	uint8_t debug;			/* debug flags */
	int user_amps;			/* user max assoc payload size */
	t_uscalar_t origin;		/* origin for disconnect (if any) */
	t_scalar_t reason;		/* reason for disconnect (if any) */
	struct sctp_options options;	/* options structure */
	int linger;			/* linger option */
	ulong lingertime;		/* time to linger */
	size_t rcvbuf;			/* receive buffer size */
	size_t rcvlowat;		/* receive buffer low water mark */
	size_t sndbuf;			/* send buffer size */
	size_t sndlowat;		/* send buffer low water mark */
};

#define SCTP_PRIV(__q) ((sctp_t *)(__q)->q_ptr)

STATIC struct sctp *sctp_protolist = NULL;
STATIC atomic_t sctp_stream_count = ATOMIC_INIT(0);

/*
 *  User locks.
 */
#define SCTP_SNDBUF_LOCK	1
#define SCTP_RCVBUF_LOCK	2
#define SCTP_BINDADDR_LOCK	4
#define SCTP_BINDPORT_LOCK	8
/*
 *  SCTP Origin Codes (Match with NPI).
 */
#define SCTP_ORIG_PROVIDER	0x0100	/* provider originated reset/disconnect */
#define SCTP_ORIG_USER		0x0101	/* user originated reset/disconnect */
#define SCTP_ORIG_UNDEFINED	0x0102	/* reset/disconnect originator undefined */
/*
 *  Sack flags.
 */
#define SCTP_SACKF_NEW		0x0001	/* SACK for new data RFC 2960 6.2 */
#define SCTP_SACKF_WUP		0x0002	/* SACK for wakeup RFC 2960 6.2 */
#define SCTP_SACKF_GAP		0x0004	/* SACK for lost data RFC 2960 7.2.4 */
#define SCTP_SACKF_DUP		0x0008	/* SACK for duplic data RFC 2960 6.2 */
#define SCTP_SACKF_URG		0x0010	/* SACK for urgent data RFC 2960 ??? */
#define SCTP_SACKF_WND		0x0020	/* SACK for new a_rwnd RFC 2960 6.2 */
#define SCTP_SACKF_TIM		0x0040	/* SACK for timeout RFC 2960 6.2 */
#define SCTP_SACKF_NOD		0x0080	/* SACK no delay RFC 2960 9.2 */
#define SCTP_SACKF_ECN		0x0100	/* SACK for ECN RFC 2960 Adx A */
#define SCTP_SACKF_FSN		0x0200	/* SACK for FORWARD TSN PR-SCTP 3.5 F2 */
#define SCTP_SACKF_CWR		0x1000	/* SACK for CWR RFC 2960 Adx A */
#define SCTP_SACKF_ASC		0x2000	/* SACK for ASCONF */
#define SCTP_SACKF_NOW		0x01fe	/* SACK forced mask */
#define SCTP_SACKF_ANY		0x01ff	/* SACK for anything */
/*
 *  SCTP private flags
 */
#define SCTP_FLAG_REC_CONF_OPT		0x00000001
#define SCTP_FLAG_EX_DATA_OPT		0x00000002
#define SCTP_FLAG_DEFAULT_RC_SEL	0x00000004
#define SCTP_FLAG_NEED_CLEANUP		0x00000008
/*
 *  HMAC types.
 */
#define SCTP_HMAC_NONE		0	/* no hmac (all one's) */
#define SCTP_HMAC_SHA_1		1	/* SHA-1 coded hmac */
#define SCTP_HMAC_MD5		2	/* MD5 coded hmac */
/*
 *  Checksum types.
 */
#define SCTP_CSUM_CRC32C	0	/* CRC-32c checksum output */
#define SCTP_CSUM_ADLER32	1	/* Adler32 checksum output */
/*
 *  Partial Reliability flags.
 */
#define SCTP_PR_NONE		0x0	/* no partial reliability */
#define SCTP_PR_PREFERRED	0x1	/* partial reliability preferred */
#define SCTP_PR_REQUIRED	0x2	/* partial reliability required */
/*
 *  Message disposition flags.
 */
#define SCTP_DISPOSITION_NONE		0x0
#define SCTP_DISPOSITION_UNSENT		0x1
#define SCTP_DISPOSITION_SENT		0x2
#define SCTP_DISPOSITION_GAP_ACKED	0x3
#define SCTP_DISPOSITION_ACKED		0x4
/*
 *  Some options flags.
 */
#define SCTP_OPTION_DROPPING	0x01	/* stream will drop packets */
#define SCTP_OPTION_BREAK	0x02	/* stream will break dest #1 and 2 one way */
#define SCTP_OPTION_DBREAK	0x04	/* stream will break dest both ways */
#define SCTP_OPTION_RANDOM	0x08	/* stream will drop packets at random */

/*
 *  Capabilities
 */
#define SCTP_CAPS_ECN		0x01	/* ECN capable */
#define SCTP_CAPS_ADD_IP	0x02	/* ADD-IP capable */
#define SCTP_CAPS_SET_IP	0x04	/* ADD-IP capable */
#define SCTP_CAPS_ALI		0x08	/* Adaptation Layer Info capable */
#define SCTP_CAPS_PR		0x10	/* PR-SCTP capable */

/*
 *  Message control block flags
 */
#define SCTPCB_FLAG_LAST_FRAG	    0x0001	/* aligned with SCTP DATA chunk flags */
#define SCTPCB_FLAG_FIRST_FRAG	    0x0002	/* aligned with SCTP DATA chunk flags */
#define SCTPCB_FLAG_URG		    0x0004	/* aligned with SCTP DATA chunk flags */
#define SCTPCB_FLAG_CKSUMMED	    0x0100
#define SCTPCB_FLAG_DELIV	    0x0200	/* delivered on receive */
#define SCTPCB_FLAG_DROPPED	    0x0200	/* also dropped on transmit */
#define SCTPCB_FLAG_ACK		    0x0400
#define SCTPCB_FLAG_NACK	    0x0800
#define SCTPCB_FLAG_CONF	    0x1000
#define SCTPCB_FLAG_SENT	    0x2000
#define SCTPCB_FLAG_RETRANS	    0x4000
#define SCTPCB_FLAG_SACKED	    0x8000

#if !defined HAVE_OPENSS7_SCTP && !defined HAVE_LKSCTP_SCTP
/*
 *  In accordance with draft-ietf-sigtran-sctp-mib-07
 */
struct sctp_mib {
	unsigned long SctpRtoAlgorithm;
	unsigned long SctpRtoMin;
	unsigned long SctpRtoMax;
	unsigned long SctpRtoInitial;
	unsigned long SctpMaxAssoc;
	unsigned long SctpValCookieLife;
	unsigned long SctpMaxInitRetr;
	unsigned long SctpCurrEstab;
	unsigned long SctpActiveEstabs;
	unsigned long SctpPassiveEstabs;
	unsigned long SctpAborteds;
	unsigned long SctpShutdowns;
	unsigned long SctpOutOfBlues;
	unsigned long SctpChecksumErrors;
	unsigned long SctpOutCtrlChunks;
	unsigned long SctpOutOrderChunks;
	unsigned long SctpOutUnorderChunks;
	unsigned long SctpInCtrlChunks;
	unsigned long SctpInOrderChunks;
	unsigned long SctpInUnorderChunks;
	unsigned long SctpFragUsrMsgs;
	unsigned long SctpReasmUsrMsgs;
	unsigned long SctpOutSCTPPacks;
	unsigned long SctpInSCTPPacks;
	unsigned long SctpDiscontinuityTime;
	unsigned long __pad[0];
} ____cacheline_aligned;

#ifdef DEFINE_SNMP_STAT
DEFINE_SNMP_STAT(struct sctp_mib, sctp_statistics);
#else
struct sctp_mib sctp_statistics[NR_CPUS * 2];
#endif				/* DEFINE_SNMP_STAT */
#else				/* HAVE_OPENSS7_SCTP || HAVE_LKSCTP_SCTP */
#ifdef DEFINE_SNMP_STAT
DEFINE_SNMP_STAT(struct sctp_mib, sctp_statistics);
#endif				/* DEFINE_SNMP_STAT */
#endif				/* HAVE_OPENSS7_SCTP || HAVE_LKSCTP_SCTP */

#define SCTP_INC_STATS(field)		SNMP_INC_STATS(sctp_statistics, field)
#define SCTP_INC_STATS_BH(field)	SNMP_INC_STATS(sctp_statistics, field)
#define SCTP_INC_STATS_USER(field)	SNMP_INC_STATS(sctp_statistics, field)

#ifdef HAVE_KINC_LINUX_SNMP_H
#define SctpRtoAlgorithm	SCTP_MIB_RTOALGORITHM
#define SctpRtoMin		SCTP_MIB_RTOMIN
#define SctpRtoMax		SCTP_MIB_RTOMAX
#define SctpRtoInitial		SCTP_MIB_RTOINITIAL
/* #define SctpMaxAssoc		SCTP_MIB_MAXASSOC */
#define SctpValCookieLife	SCTP_MIB_VALCOOKIELIFE
#define SctpMaxInitRetr		SCTP_MIB_MAXINITRETR
#define SctpCurrEstab		SCTP_MIB_CURRESTAB
#define SctpActiveEstabs	SCTP_MIB_ACTIVEESTABS
#define SctpPassiveEstabs	SCTP_MIB_PASSIVEESTABS
#define SctpAborteds		SCTP_MIB_ABORTEDS
#define SctpShutdowns		SCTP_MIB_SHUTDOWNS
#define SctpOutOfBlues		SCTP_MIB_OUTOFBLUES
#define SctpChecksumErrors	SCTP_MIB_CHECKSUMERRORS
#define SctpOutCtrlChunks	SCTP_MIB_OUTCTRLCHUNKS
#define SctpOutOrderChunks	SCTP_MIB_OUTORDERCHUNKS
#define SctpOutUnorderChunks	SCTP_MIB_OUTUNORDERCHUNKS
#define SctpInCtrlChunks	SCTP_MIB_INCTRLCHUNKS
#define SctpInOrderChunks	SCTP_MIB_INORDERCHUNKS
#define SctpInUnorderChunks	SCTP_MIB_INUNORDERCHUNKS
#define SctpFragUsrMsgs		SCTP_MIB_FRAGUSRMSGS
#define SctpReasmUsrMsgs	SCTP_MIB_REASMUSRMSGS
#define SctpOutSCTPPacks	SCTP_MIB_OUTSCTPPACKS
#define SctpInSCTPPacks		SCTP_MIB_INSCTPPACKS
/* #define SctpDiscontinuityTime	SCTP_MIB_DISCONTINUITYTIME */
#ifndef SCTP_MIB_MAX
enum {
	SCTP_MIB_NUM = 0,
	SCTP_MIB_CURRESTAB,		/* CurrEstab */
	SCTP_MIB_ACTIVEESTABS,		/* ActiveEstabs */
	SCTP_MIB_PASSIVEESTABS,		/* PassiveEstabs */
	SCTP_MIB_ABORTEDS,		/* Aborteds */
	SCTP_MIB_SHUTDOWNS,		/* Shutdowns */
	SCTP_MIB_OUTOFBLUES,		/* OutOfBlues */
	SCTP_MIB_CHECKSUMERRORS,	/* ChecksumErrors */
	SCTP_MIB_OUTCTRLCHUNKS,		/* OutCtrlChunks */
	SCTP_MIB_OUTORDERCHUNKS,	/* OutOrderChunks */
	SCTP_MIB_OUTUNORDERCHUNKS,	/* OutUnorderChunks */
	SCTP_MIB_INCTRLCHUNKS,		/* InCtrlChunks */
	SCTP_MIB_INORDERCHUNKS,		/* InOrderChunks */
	SCTP_MIB_INUNORDERCHUNKS,	/* InUnorderChunks */
	SCTP_MIB_FRAGUSRMSGS,		/* FragUsrMsgs */
	SCTP_MIB_REASMUSRMSGS,		/* ReasmUsrMsgs */
	SCTP_MIB_OUTSCTPPACKS,		/* OutSCTPPacks */
	SCTP_MIB_INSCTPPACKS,		/* InSCTPPacks */
	SCTP_MIB_T1_INIT_EXPIREDS,
	SCTP_MIB_T1_COOKIE_EXPIREDS,
	SCTP_MIB_T2_SHUTDOWN_EXPIREDS,
	SCTP_MIB_T3_RTX_EXPIREDS,
	SCTP_MIB_T4_RTO_EXPIREDS,
	SCTP_MIB_T5_SHUTDOWN_GUARD_EXPIREDS,
	SCTP_MIB_DELAY_SACK_EXPIREDS,
	SCTP_MIB_AUTOCLOSE_EXPIREDS,
	SCTP_MIB_T3_RETRANSMITS,
	SCTP_MIB_PMTUD_RETRANSMITS,
	SCTP_MIB_FAST_RETRANSMITS,
	SCTP_MIB_IN_PKT_SOFTIRQ,
	SCTP_MIB_IN_PKT_BACKLOG,
	SCTP_MIB_IN_PKT_DISCARDS,
	SCTP_MIB_IN_DATA_CHUNK_DISCARDS,
	__SCTP_MIB_MAX
};

#define SCTP_MIB_MAX    __SCTP_MIB_MAX
struct sctp_mib {
	unsigned long mibs[SCTP_MIB_MAX];
} __SNMP_MIB_ALIGN__;
#endif
#endif

/*
 *  =========================================================================
 *
 *  SCTP STATES
 *
 *  =========================================================================
 *
 *  For sockets, base SCTP states off of TCP states to avoid rewriting a lot of TCP code at the
 *  inet_socket level.
 */
#define SCTP_ESTABLISHED	 1	/* ESTABLISHED */
#define SCTP_COOKIE_WAIT	 2	/* COOKIE-WAIT */
#define SCTP_COOKIE_ECHOED	 3	/* COOKIE-ECHOED */
#define SCTP_SHUTDOWN_PENDING	 4	/* SHUTDOWN-PENDING */
#define SCTP_SHUTDOWN_SENT	 5	/* SHUTDOWN-SENT */
#define SCTP_UNREACHABLE	 6	/* (not used) */
#define SCTP_CLOSED		 7	/* CLOSED */
#define SCTP_SHUTDOWN_RECEIVED	 8	/* SHUTDOWN-RECEIVED */
#define SCTP_SHUTDOWN_RECVWAIT	 9	/* SHUTDOWN-RECEIVED */
#define SCTP_LISTEN		10	/* LISTEN */
#define SCTP_SHUTDOWN_ACK_SENT	11	/* SHUTDOWN-ACK-SENT */
#define SCTP_MAX_STATES		12
#define SCTPF_CLOSED             (1<<SCTP_CLOSED		)
#define SCTPF_LISTEN             (1<<SCTP_LISTEN		)
#define SCTPF_COOKIE_WAIT        (1<<SCTP_COOKIE_WAIT		)
#define SCTPF_COOKIE_ECHOED      (1<<SCTP_COOKIE_ECHOED		)
#define SCTPF_ESTABLISHED        (1<<SCTP_ESTABLISHED		)
#define SCTPF_SHUTDOWN_PENDING   (1<<SCTP_SHUTDOWN_PENDING	)
#define SCTPF_SHUTDOWN_SENT      (1<<SCTP_SHUTDOWN_SENT		)
#define SCTPF_SHUTDOWN_RECEIVED  (1<<SCTP_SHUTDOWN_RECEIVED	)
#define SCTPF_SHUTDOWN_RECVWAIT  (1<<SCTP_SHUTDOWN_RECVWAIT	)
#define SCTPF_SHUTDOWN_ACK_SENT  (1<<SCTP_SHUTDOWN_ACK_SENT	)
#define SCTPF_UNREACHABLE	 (1<<SCTP_UNREACHABLE		)
#define SCTPF_OPENING		 (SCTPF_COOKIE_WAIT| \
				  SCTPF_COOKIE_ECHOED)
#define SCTPF_CLOSING		 (SCTPF_SHUTDOWN_SENT| \
				  SCTPF_SHUTDOWN_ACK_SENT)
#define SCTPF_CONNECTED		 (SCTPF_ESTABLISHED| \
				  SCTPF_SHUTDOWN_PENDING| \
				  SCTPF_SHUTDOWN_RECEIVED| \
				  SCTPF_SHUTDOWN_RECVWAIT)
#define	SCTPF_DISCONNECTED	 (SCTPF_CLOSED| \
				  SCTPF_LISTEN| \
				  SCTPF_UNREACHABLE)
#define SCTPF_SENDING		 (SCTPF_COOKIE_ECHOED| \
				  SCTPF_ESTABLISHED| \
				  SCTPF_SHUTDOWN_PENDING| \
				  SCTPF_SHUTDOWN_RECEIVED| \
				  SCTPF_SHUTDOWN_RECVWAIT)
#define SCTPF_RECEIVING		 (SCTPF_COOKIE_ECHOED| \
				  SCTPF_ESTABLISHED| \
				  SCTPF_SHUTDOWN_PENDING| \
				  SCTPF_SHUTDOWN_SENT)
#define SCTPF_NEEDABORT		 (SCTPF_COOKIE_ECHOED| \
				  SCTPF_ESTABLISHED| \
				  SCTPF_SHUTDOWN_PENDING| \
				  SCTPF_SHUTDOWN_RECEIVED| \
				  SCTPF_SHUTDOWN_RECVWAIT| \
				  SCTPF_SHUTDOWN_SENT)
#define SCTPF_HAVEUSER		 (SCTPF_COOKIE_WAIT| \
				  SCTPF_COOKIE_ECHOED| \
				  SCTPF_ESTABLISHED| \
				  SCTPF_SHUTDOWN_PENDING| \
				  SCTPF_SHUTDOWN_RECEIVED| \
				  SCTPF_SHUTDOWN_SENT)

#ifndef _OPTIMIZE_SPEED
STATIC char *sctp_state_name[] = {
	"(Uninitialized)",
	"ESTABLISHED",
	"COOKIE-WAIT",
	"COOKIE-ECHOED",
	"SHUTDOWN-PENDING",
	"SHUTDOWN-SENT",
	"(Unreachable)",
	"CLOSED",
	"SHUTDOWN-RECEIVED",
	"SHUTDOWN-RECVWAIT",
	"LISTEN",
	"SHUTDOWN-ACK-SENT",
	"(Max-State)"
};

STATIC const char *
sctp_statename(int state)
{
	switch (state) {
	case 0:
	case SCTP_ESTABLISHED:
	case SCTP_COOKIE_WAIT:
	case SCTP_COOKIE_ECHOED:
	case SCTP_SHUTDOWN_PENDING:
	case SCTP_SHUTDOWN_SENT:
	case SCTP_CLOSED:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
	case SCTP_LISTEN:
	case SCTP_SHUTDOWN_ACK_SENT:
	case SCTP_UNREACHABLE:
	case SCTP_MAX_STATES:
		return (sctp_state_name[state]);
	default:
		return ("(Invalid)");

	}
}

STATIC INLINE int
sctp_change_state(struct sctp *sp, int newstate, const char *file, int line)
{
	int oldstate;

	if ((oldstate = xchg(&sp->state, newstate)) != newstate)
		sctplogst(sp, "%s <- %s (%s +%d)", sctp_statename(newstate), sctp_statename(oldstate), file, line);
	return (oldstate);
}

#define sctp_change_state(sp, newstate) \
	sctp_change_state(sp, newstate, __FILE__, __LINE__)
#else
STATIC INLINE int
sctp_change_state(struct sctp *sp, int newstate)
{
	return xchg(&sp->state, newstate);
}
#endif

/*
 *  =========================================================================
 *
 *  SCTP Message Structures
 *
 *  =========================================================================
 */
#ifndef HAVE_OPENSS7_SCTP
#define PADC(__len) (((__len)+3)&~0x3)
#define SCTP_PADC(__len) (((__len)+3)&~0x3)

#ifdef LINUX
#undef bzero
#define bzero(__ptr,__len) memset((__ptr), 0, (__len))
#undef bcopy
#define bcopy(__from,__to,__len) memcpy((__to), (__from), (__len))
#endif

/* basic headers */
struct sctphdr {
	uint16_t srce;
	uint16_t dest;
	uint32_t v_tag;
	uint32_t check;
};
struct sctpchdr {
	uint8_t type;
	uint8_t flags;
	uint16_t len;
};
#endif				/* HAVE_OPENSS7_SCTP */

#define SCTP_IPH(__mp) ((struct iphdr *)((__mp)->b_datap->db_base))
#define SCTP_SH(__mp) ((struct sctphdr *)((__mp)->b_datap->db_base + (SCTP_IPH(__mp)->ihl << 2)))
#define SCTP_CH(__mp) ((struct sctpchdr *)((__mp)->b_rptr))

#ifdef HAVE_KMEMB_STRUCT_SK_BUFF_H_SH
#define SCTP_SKB_SH(__skb)	((__skb)->h.sh)
#else
#define SCTP_SKB_SH(__skb)	((struct sctphdr *)((__skb)->h.raw))
#endif
#define SCTP_SKB_SH_SRCE(__skb)	(SCTP_SKB_SH(__skb)->srce)
#define SCTP_SKB_SH_DEST(__skb)	(SCTP_SKB_SH(__skb)->dest)
#define SCTP_SKB_SH_VTAG(__skb)	(SCTP_SKB_SH(__skb)->v_tag)
#define SCTP_SKB_SH_CSUM(__skb)	(SCTP_SKB_SH(__skb)->check)
#define SCTP_SKB_CH(__skb)	((struct sctpchdr *)(__skb)->data)
#define SCTP_SKB_CH_TYPE(__skb)	(SCTP_SKB_CH(__skb)->type)
#define SCTP_SKB_CH_LEN(__skb)	(ntohs(SCTP_SKB_CH(__skb)->len))

/*
 *  SCTP Cause Codes
 */
#define SCTP_CAUSE_INVALID_STR		   1	/* Invalid Stream Identifier */
#define SCTP_CAUSE_MISSING_PARM		   2	/* Missing Mandatory Parameter */
#define SCTP_CAUSE_STALE_COOKIE		   3	/* Stale Cookie Error */
#define SCTP_CAUSE_NO_RESOURCE		   4	/* Out of Resource */
#define SCTP_CAUSE_BAD_ADDRESS		   5	/* Unresolvable Address */
#define SCTP_CAUSE_BAD_CHUNK_TYPE	   6	/* Unrecognized Chunk Type */
#define SCTP_CAUSE_INVALID_PARM		   7	/* Invalid Mandatory Parameter */
#define SCTP_CAUSE_BAD_PARM		   8	/* Unrecognized Parameters */
#define SCTP_CAUSE_NO_DATA		   9	/* No User Data */
#define SCTP_CAUSE_SHUTDOWN		  10	/* Cookie Received While Shutting Down */
#define SCTP_CAUSE_NEW_ADDR		  11	/* Restart of assoc w/ new address */
#define SCTP_CAUSE_USER_INITIATED	  12	/* User initiated abort */
#define SCTP_CAUSE_PROTO_VIOLATION	  13	/* Protocol violation */
#define SCTP_CAUSE_LAST_ADDR		0x100	/* Request to delete last ip address */
#define SCTP_CAUSE_RES_SHORTAGE		0x101	/* Operation refused resource shortage */
#define SCTP_CAUSE_SOURCE_ADDR		0x102	/* Request to delete source ip address */
#define SCTP_CAUSE_ILLEGAL_ASCONF	0x103	/* Assoc. aborted illegal ASCONF-ACK */

#define SCTP_CF_INVALID_STR	(1<< SCTP_CAUSE_INVALID_STR	)
#define SCTP_CF_MISSING_PARM	(1<< SCTP_CAUSE_MISSING_PARM	)
#define SCTP_CF_STALE_COOKIE	(1<< SCTP_CAUSE_STALE_COOKIE	)
#define SCTP_CF_NO_RESOURCE	(1<< SCTP_CAUSE_NO_RESOURCE	)
#define SCTP_CF_BAD_ADDRESS	(1<< SCTP_CAUSE_BAD_ADDRESS	)
#define SCTP_CF_BAD_CHUNK_TYPE	(1<< SCTP_CAUSE_BAD_CHUNK_TYPE	)
#define SCTP_CF_INVALID_PARM	(1<< SCTP_CAUSE_INVALID_PARM	)
#define SCTP_CF_BAD_PARM	(1<< SCTP_CAUSE_BAD_PARM	)
#define SCTP_CF_NO_DATA		(1<< SCTP_CAUSE_NO_DATA		)
#define SCTP_CF_SHUTDOWN	(1<< SCTP_CAUSE_SHUTDOWN	)

#define SCTP_CF_MASK_ANY	(SCTP_CF_INVALID_STR	\
				|SCTP_CF_MISSING_PARM	\
				|SCTP_CF_STALE_COOKIE	\
				|SCTP_CF_NO_RESOURCE	\
				|SCTP_CF_BAD_ADDRESS	\
				|SCTP_CF_BAD_CHUNK_TYPE	\
				|SCTP_CF_INVALID_PARM	\
				|SCTP_CF_BAD_PARM	\
				|SCTP_CF_NO_DATA	\
				|SCTP_CF_SHUTDOWN	)
#ifndef HAVE_OPENSS7_SCTP
/*
 *  SCTP Cause Code Structures
 */
struct sctpehdr {
	uint16_t code;
	uint16_t len;
};
struct sctp_bad_stri {			/* Invalid Stream Identifier */
	struct sctpehdr eh;
	uint16_t sid;			/* stream identifier */
	uint16_t res;			/* reserved */
};
struct sctp_no_mparm {			/* Missing Mandatory Parameter */
	struct sctpehdr eh;
	uint32_t num;			/* number of missing parms */
	uint16_t mp[0];			/* array of missing parms */
};
struct sctp_stale_ck {			/* Stale Cookie Error */
	struct sctpehdr eh;
	uint32_t staleness;		/* measure of staleness (usec) */
};
struct sctp_no_rsrce {			/* Out of Resource */
	struct sctpehdr eh;
};
struct sctp_bad_addr {			/* Unresolvable Address */
	struct sctpehdr eh;
	/* followed by */
	/* address parameter */
};
struct sctp_bad_chnk {			/* Unrecognized Chunk Type */
	struct sctpehdr eh;
	struct sctpchdr ch;		/* header of unrecognized chunk */
};
struct sctp_bad_parm {			/* Invalid Mandatory Parameter */
	struct sctpehdr eh;
};
struct sctp_unk_parm {			/* Unrecognized Parameters */
	struct sctpehdr eh;
	unsigned char parm[0];		/* unrecognized parameters */
};
struct sctp_no_udata {			/* No user Data */
	struct sctpehdr eh;
	uint32_t tsn;			/* tsn of data chunk */
};
struct sctp_ck_shutd {			/* Cookie Received While Shutting Down */
	struct sctpehdr eh;
};
struct sctp_last_add {			/* Request to delete last address */
	struct sctpehdr eh;
	uint32_t cid;
	/* followed by ASCONF TLV */
};
struct sctp_op_short {			/* Operation refused resource shortage */
	struct sctpehdr eh;
	uint32_t cid;
	/* followed by ASCONF TLV */
};
struct sctp_srce_add {			/* Request to delete source address */
	struct sctpehdr eh;
	uint32_t cid;
	/* followed by ASCONF TLV */
};
struct sctp_bad_conf {			/* Illegal ASCONF-ACK */
	struct sctpehdr eh;
};
union sctp_cause {
	struct sctpehdr eh;
	struct sctp_bad_stri bad_stri;	/* Invalid Stream Identifier */
	struct sctp_no_mparm no_mparm;	/* Missing Mandatory Parameter */
	struct sctp_stale_ck stale_ck;	/* Stale Cookie Error */
	struct sctp_no_rsrce no_rsrce;	/* Out of Resource */
	struct sctp_bad_addr bad_addr;	/* Unresolvable Address */
	struct sctp_bad_chnk bad_chnk;	/* Unrecognized Chunk Type */
	struct sctp_bad_parm bad_parm;	/* Invalid Mandatory Parameter */
	struct sctp_unk_parm unk_parm;	/* Unrecognized Parameters */
	struct sctp_no_udata no_udata;	/* No user Data */
	struct sctp_ck_shutd ck_shutd;	/* Cookie Received While Shutting Down */
};

/*
 *  COOKIE parameter
 *
 *  TODO: Put IP OPTIONS (struct ip_options + __data) from the INIT message
 *  into the cookie and pull them back out of the COOKIE ECHO.  As it stands
 *  now we are only supporting IP OPTIONS with our own INIT.
 */
struct sctp_cookie {
	unsigned long timestamp;	/* timestamp of the cookie */
	unsigned long lifespan;		/* lifespan of the cookie */
	uint32_t v_tag;			/* local verification tag */
	uint32_t daddr;			/* dest address */
	uint32_t saddr;			/* srce address */
	uint16_t dport;			/* dest port */
	uint16_t sport;			/* dest port */
	uint32_t p_tag;			/* peer's verification tag */
	uint32_t p_tsn;			/* peer initial TSN */
	uint32_t p_rwnd;		/* perr a_rwnd */
	uint16_t n_istr;		/* number of inbound streams */
	uint16_t n_ostr;		/* number of outbound streams */
	uint32_t l_caps;		/* local capabilities */
	uint32_t p_caps;		/* peer capabilities */
	uint32_t l_ali;			/* local adaptation layer information */
	uint32_t p_ali;			/* peer adaptation layer information */
	uint32_t l_ttag;		/* local tie tag */
	uint32_t p_ttag;		/* peer tie tag */
	uint16_t danum;			/* number of dest transport addresses */
	uint16_t sanum;			/* number of srce transport addresses */
	uint16_t key_tag;		/* sender's tag for key */
	uint16_t opt_len;		/* length of included ip options */
	/* followed by opt.__data */
	/* followed by dtas */
	/* uint32_t dtas[0]; dest transport addresses */
	/* followed by stas */
	/* uint32_t stas[0]; srce transport addresses */
	/* followed by mac */
	/* uint8_t mac[160]; message authentication code */
};

/*
 *  Our HEARTBEAT INFO structure:
 */
struct sctp_hb_info {
	unsigned long timestamp;	/* jiffies timestamp of when it was sent */
	uint32_t daddr;			/* destination address sent to */
	int mtu;			/* Destingation MTU being tested */
	unsigned char fill[0];		/* Fill for Destination MTU testing */
};
#endif				/* HAVE_OPENSS7_SCTP */
/*
 *  SCTP Parameter types
 */
#define SCTP_PTYPE_MASK			(__constant_htons(0x3fff))
#define SCTP_PTYPE_MASK_REPORT		(__constant_htons(0x4000))
#define SCTP_PTYPE_MASK_CONTINUE	(__constant_htons(0x8000))
#define SCTP_PTYPE_MASK_REP_CONT	(__constant_htons(0xC000))

#define SCTP_PTYPE_HEARTBEAT_INFO	(__constant_htons(1))
#define SCTP_PTYPE_IPV4_ADDR		(__constant_htons(5))
#define SCTP_PTYPE_IPV6_ADDR		(__constant_htons(6))
#define SCTP_PTYPE_STATE_COOKIE		(__constant_htons(7))
#define SCTP_PTYPE_UNREC_PARMS		(__constant_htons(8))
#define SCTP_PTYPE_COOKIE_PSRV		(__constant_htons(9))
#define SCTP_PTYPE_HOST_NAME		(__constant_htons(11))
#define SCTP_PTYPE_ADDR_TYPE		(__constant_htons(12))
#define SCTP_PTYPE_ECN_CAPABLE		(__constant_htons(0)|SCTP_PTYPE_MASK_CONTINUE)
#define SCTP_PTYPE_PR_SCTP		(__constant_htons(0)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_ADD_IP		(__constant_htons(1)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_DEL_IP		(__constant_htons(2)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_ERROR_CAUSE		(__constant_htons(3)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_SET_IP		(__constant_htons(4)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_SUCCESS_REPORT	(__constant_htons(5)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_ALI			(__constant_htons(6)|SCTP_PTYPE_MASK_REP_CONT)

#ifndef HAVE_OPENSS7_SCTP
struct sctpphdr {
	uint16_t type;
	uint16_t len;
};
struct sctp_heartbeat_info {
	struct sctpphdr ph;
	struct sctp_hb_info hb_info;
};
struct sctp_ipv4_addr {
	struct sctpphdr ph;
	uint32_t addr;
};
struct sctp_ipv6_addr {
	struct sctpphdr ph;
};
struct sctp_state_cookie {
	struct sctpphdr ph;
	struct sctp_cookie cookie[0];
};
struct sctp_unrec_parms {
	struct sctpphdr ph;
};
struct sctp_cookie_psrv {
	struct sctpphdr ph;
	uint32_t ck_inc;
};
struct sctp_host_name {
	struct sctpphdr ph;
};
struct sctp_addr_type {
	struct sctpphdr ph;
	uint16_t type[0];
};
struct sctp_ecn_capable {
	struct sctpphdr ph;
};
struct sctp_pr_sctp {
	struct sctpphdr ph;
};
struct sctp_ali {
	struct sctpphdr ph;
	uint32_t ali;
};
struct sctp_add_ip {
	struct sctpphdr ph;
	uint32_t id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct sctp_del_ip {
	struct sctpphdr ph;
	uint32_t id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct sctp_set_ip {
	struct sctpphdr ph;
	uint32_t id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct sctp_error_cause {
	struct sctpphdr ph;
	uint32_t cid;
	/* followed by error causes */
};
struct sctp_success_report {
	struct sctpphdr ph;
	uint32_t cid;
};
union sctp_parm {
	struct sctpphdr ph;
	struct sctp_heartbeat_info heartbeat_info;
	struct sctp_ipv4_addr ipv4_addr;
	struct sctp_ipv6_addr ipv6_addr;
	struct sctp_state_cookie state_cookie;
	struct sctp_unrec_parms unrec_parms;
	struct sctp_cookie_psrv cookie_prsv;
	struct sctp_host_name host_name;
	struct sctp_addr_type addr_type;
	struct sctp_ecn_capable ecn_capable;
	struct sctp_pr_sctp pr_sctp;
	struct sctp_ali ali;
	struct sctp_add_ip add_ip;
	struct sctp_del_ip del_ip;
	struct sctp_set_ip set_ip;
	struct sctp_error_cause error_cause;
	struct sctp_success_report success_report;
};
#endif				/* HAVE_OPENSS7_SCTP */
/*
 *  SCTP Chunk Types
 */
#define SCTP_CTYPE_MASK			0x3f
#define SCTP_CTYPE_MASK_CONTINUE	0x80
#define SCTP_CTYPE_MASK_REPORT		0x40
#define SCTP_CTYPE_MASK_REP_CONT	0xC0

#define SCTP_CTYPE_DATA			0
#define SCTP_CTYPE_INIT			1
#define SCTP_CTYPE_INIT_ACK		2
#define SCTP_CTYPE_SACK			3
#define SCTP_CTYPE_HEARTBEAT		4
#define SCTP_CTYPE_HEARTBEAT_ACK	5
#define SCTP_CTYPE_ABORT		6
#define SCTP_CTYPE_SHUTDOWN		7
#define SCTP_CTYPE_SHUTDOWN_ACK		8
#define SCTP_CTYPE_ERROR		9
#define SCTP_CTYPE_COOKIE_ECHO		10
#define SCTP_CTYPE_COOKIE_ACK		11
#define SCTP_CTYPE_ECNE			12
#define SCTP_CTYPE_CWR			13
#define SCTP_CTYPE_SHUTDOWN_COMPLETE	14
#define SCTP_CTYPE_ASCONF_ACK		(0|SCTP_CTYPE_MASK_CONTINUE)
#define SCTP_CTYPE_ASCONF		(1|SCTP_CTYPE_MASK_REP_CONT)
#define SCTP_CTYPE_FORWARD_TSN		(0|SCTP_CTYPE_MASK_REP_CONT)
#ifndef HAVE_OPENSS7_SCTP
/*
 *  SCTP Chunk Structures
 */
struct sctp_data {
	struct sctpchdr ch;
	uint32_t tsn;			/* Transmit Sequence Number */
	uint16_t sid;			/* Stream Identifier */
	uint16_t ssn;			/* Stream Sequence Number */
	uint32_t ppi;			/* Payload Protocol Identifier */
	uint8_t udat[0];		/* User data */
};
struct sctp_init {
	struct sctpchdr ch;
	uint32_t i_tag;			/* Initiate Tag */
	uint32_t a_rwnd;		/* Advertised Received Window Credit */
	uint16_t n_ostr;		/* Number of Outbound Streams */
	uint16_t n_istr;		/* Number of Inbound Streams */
	uint32_t i_tsn;			/* Initial TSN */
};
struct sctp_init_ack {
	struct sctpchdr ch;
	uint32_t i_tag;			/* Initiate Tag */
	uint32_t a_rwnd;		/* Advertised Received Window Credit */
	uint16_t n_ostr;		/* Number of Outbound Streams */
	uint16_t n_istr;		/* Number of Inbound Streams */
	uint32_t i_tsn;			/* Initial TSN */
};
struct sctp_sack {
	struct sctpchdr ch;
	uint32_t c_tsn;			/* Cumulative TSN Ack */
	uint32_t a_rwnd;		/* Advertized Receiver Window Credit */
	uint16_t ngaps;			/* Number of Gap Blocks */
	uint16_t ndups;			/* Number of Duplicate TSNs */
	uint16_t gaps[0];		/* Gap blocks */
	uint32_t dups[0];		/* Duplicate TSNs */
};
struct sctp_heartbeat {
	struct sctpchdr ch;
};
struct sctp_heartbeat_ack {
	struct sctpchdr ch;
};
struct sctp_abort {
	struct sctpchdr ch;
	union sctp_cause cause[0];
};
struct sctp_shutdown {
	struct sctpchdr ch;
	uint32_t c_tsn;			/* Cummulative TSN Ack */
};
struct sctp_shutdown_ack {
	struct sctpchdr ch;
};
struct sctp_error {
	struct sctpchdr ch;
	union sctp_cause cause[0];
};
struct sctp_cookie_echo {
	struct sctpchdr ch;
	unsigned char cookie[0];
};
struct sctp_cookie_ack {
	struct sctpchdr ch;
};
struct sctp_ecne {
	struct sctpchdr ch;
	uint32_t l_tsn;
};
struct sctp_cwr {
	struct sctpchdr ch;
	uint32_t l_tsn;
};
struct sctp_shutdown_comp {
	struct sctpchdr ch;
};
struct sctp_asconf {
	struct sctpchdr ch;
	uint32_t asn;
};
struct sctp_asconf_ack {
	struct sctpchdr ch;
	uint32_t asn;
};
struct sctp_forward_tsn {
	struct sctpchdr ch;
	uint32_t f_tsn;
};
union sctp_chunk {
	struct sctpchdr ch;
	struct sctp_data data;
	struct sctp_init init;
	struct sctp_init_ack init_ack;
	struct sctp_sack sack;
	struct sctp_heartbeat heartbeat;
	struct sctp_heartbeat_ack heartbeat_ack;
	struct sctp_abort abort;
	struct sctp_shutdown shutdown;
	struct sctp_shutdown_ack shutdown_ack;
	struct sctp_error error;
	struct sctp_cookie_echo cookie_echo;
	struct sctp_cookie_ack cookie_ack;
	struct sctp_ecne ecne;
	struct sctp_cwr cwr;
	struct sctp_shutdown_comp shutdown_comp;
	struct sctp_asconf asconf;
	struct sctp_asconf_ack asconf_ack;
	struct sctp_forward_tsn forward_tsn;
};

/*
 *  SCTP Message Structures
 */
struct sctp_msg {
	struct sctphdr mh;
	union sctp_chunk chunk;
};
#endif				/* HAVE_OPENSS7_SCTP */

/*
 *  Message control block datastructures:
 *  -------------------------------------------------------------------------
 *  This is what the send packet queueing engine uses to pass SCTP per-packet control information to
 *  the transmission code.  The receive packet queueing engine also uses this.  If this grows,
 *  please adjust skbuff.h:skbuff->cb[xxx] size appropriately.  (We currently have 48 bytes to work
 *  with.) We are bang on the 48 byte limit.
 */
/* message control block */
struct sctp_tcb {
	/* for gaps, dups and acks on receive, frag on transmit */
	sctp_tcb_t *next;		/* message linkage */
	sctp_tcb_t *tail;		/* message linkage */
	mblk_t *mp;			/* message linkage */
	uint32_t tsn;			/* why do I need these?, they are in the chunk header */
	uint16_t sid;
	uint16_t ssn;
	uint32_t ppi;
	uint16_t dlen;			/* data length */
	uint16_t flags;			/* general flags inc. data chunk flags */
	uint16_t sacks;			/* number of times gap acked */
	uint16_t trans;			/* number of times retransmitted */
	unsigned long expires;		/* when this packet expires */
	unsigned long when;		/* time of tx/rx/ack */
	struct sctp_daddr *daddr;	/* daddr tx to or rx from */
	struct sctp_strm *st;		/* strm tx to or rx from */
};

#define SCTP_TCB(__mp) ((sctp_tcb_t *)((__mp)->b_datap->db_base))

/*
 * TLI interface state flags
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
#define TSM_LISTEN	(TSF_IDLE \
			|TSF_WRES_CIND)
#define TSM_CONNECTED	(TSF_WCON_CREQ\
			|TSF_WRES_CIND\
			|TSF_DATA_XFER\
			|TSF_WIND_ORDREL\
			|TSF_WREQ_ORDREL)
#define TSM_DISCONN	(TSF_IDLE\
			|TSF_UNBND)
#define TSM_INDATA	(TSF_DATA_XFER\
			|TSF_WIND_ORDREL)
#define TSM_OUTDATA	(TSF_DATA_XFER\
			|TSF_WREQ_ORDREL)
#define TF_IP_BROADCAST			(1<< 0)
#define TF_IP_DONTROUTE			(1<< 1)
#define TF_IP_REUSEADDR			(1<< 2)
#define TF_IP_OPTIONS			(1<< 3)
#define TF_IP_TOS			(1<< 4)
#define TF_IP_TTL			(1<< 5)
#define TF_SCTP_NODELAY			(1<< 6)
#define TF_SCTP_CORK			(1<< 7)
#define TF_SCTP_PPI			(1<< 8)
#define TF_SCTP_SID			(1<< 9)
#define TF_SCTP_SSN			(1<<10)
#define TF_SCTP_TSN			(1<<11)
#define TF_SCTP_RECVOPT			(1<<12)
#define TF_SCTP_COOKIE_LIFE		(1<<13)
#define TF_SCTP_SACK_DELAY		(1<<14)
#define TF_SCTP_PATH_MAX_RETRANS	(1<<15)
#define TF_SCTP_ASSOC_MAX_RETRANS	(1<<16)
#define TF_SCTP_MAX_INIT_RETRIES	(1<<17)
#define TF_SCTP_HEARTBEAT_ITVL		(1<<18)
#define TF_SCTP_RTO_INITIAL		(1<<19)
#define TF_SCTP_RTO_MIN			(1<<20)
#define TF_SCTP_RTO_MAX			(1<<21)
#define TF_SCTP_OSTREAMS		(1<<22)
#define TF_SCTP_ISTREAMS		(1<<23)
#define TF_SCTP_COOKIE_INC		(1<<24)
#define TF_SCTP_THROTTLE_ITVL		(1<<25)
#define TF_SCTP_MAC_TYPE		(1<<26)
#define TF_SCTP_CSUM_TYPE		(1<<27)
#define TF_SCTP_HB			(1<<28)
#define TF_SCTP_RTO			(1<<29)
#define TF_SCTP_MAXSEG			(1<<30)
#define TF_SCTP_STATUS			(1<<31)
#define TF_SCTP_DEBUG			(1<<32)
#define TF_SCTP_ALLOPS			0xffffffff

#define NSM_ALLSTATES	(NSF_NOSTATES - 1)
#define NSM_WACK_DREQ	(NSF_WACK_DREQ6 \
			|NSF_WACK_DREQ7 \
			|NSF_WACK_DREQ9 \
			|NSF_WACK_DREQ10 \
			|NSF_WACK_DREQ11)
#define NSM_LISTEN	(NSF_IDLE \
			|NSF_WRES_CIND)
#define NSM_CONNECTED	(NSF_WCON_CREQ\
			|NSF_WRES_CIND\
			|NSF_DATA_XFER\
			|NSF_WCON_RREQ\
			|NSF_WRES_RIND)
#define NSM_DISCONN	(NSF_IDLE\
			|NSF_UNBND)
#define NSM_INDATA	(NSF_DATA_XFER\
			|NSF_WCON_RREQ)
#define NSM_OUTDATA	(NSF_DATA_XFER\
			|NSF_WRES_RIND)

#ifndef N_PROVIDER
#define N_PROVIDER  0
#define N_USER	    1
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Locking
 *
 *  -------------------------------------------------------------------------
 */
#define sctp_init_lock(__sp) spin_lock_init(&((__sp)->qlock))

/*
 *  These two are used by timeout functions to lock normal queue functions
 *  from entering put and srv routines.
 */
#define bh_lock_sctp(__sp) spin_lock_bh(&((__sp)->qlock))
#define bh_unlock_sctp(__sp) spin_unlock_bh(&((__sp)->qlock))

STATIC spinlock_t sctp_protolock = SPIN_LOCK_UNLOCKED;

/**
 * sctp_trylockq: - try to lock a private structure
 * @q: queue pair associated with private structure
 *
 * Returns a pointer to the locked private structure or NULL if the private structure could not be
 * locked immediately.  This form is used by message handling procedures to gain exclusive access to
 * the queue pair private structure.
 */
STATIC inline fastcall struct sctp *
sctp_trylockq(queue_t *q)
{
	struct sctp *sp = SCTP_PRIV(q), *rp = sp;

	if (likely(sp != NULL) && unlikely(test_and_set_bit(0, &sp->users))) {
		set_bit((q->q_flag & QREADR) ? 1 : 2, &sp->users);
		if (likely(test_and_set_bit(0, &sp->users)))
			rp = NULL;
	}
	return (rp);
}

/**
 * sctp_trylock_timer: - try to lock a private structure for a timer
 * @sp: the private structure to lock
 * @sd: the address structure (or NULL)
 * @bit: timer bit to set upon failure
 *
 * Returns true (1) if the lock is held and false (0) when it is not.  If the function fails to
 * acquire the lock, the specified bit is set in the private structure (and possibly address
 * structure) indicating that a timeout is deferred and the read queue service procedure is will be
 * scheduled when the structure is unlocked.  When the read queue service procedure runs, it
 * acquires structure locks, checks for deferred timeouts and runs them.  Otherwise, the timeout can
 * be executed immediately because we use qtimeout()/quntimeout() that runs like a put() procedure
 * against the queue pair.
 */

STATIC inline fastcall int
sctp_trylock_timer(struct sctp *sp, struct sctp_daddr *sd, uint bit)
{
	if (unlikely(test_and_set_bit(0, &sp->users))) {
		set_bit(1, &sp->users);	/* mark read queue service */
		set_bit(bit, &sp->timers);	/* mark deferred timeout */
		if (sd != NULL)
			set_bit(bit, &sd->timers);	/* mark deferred timeout */
		if (likely(test_and_set_bit(0, &sp->users)))
			return (0);
	}
	clear_bit(bit, sd ? &sd->timers : &sp->timers);
	return (1);
}

/**
 * sctp_sleeplock: - try to lock a private structure with blocking
 * @q: queue pair associated with private structure
 *
 * Returns a pointer to the locked private structure or NULL if the private structure could not be
 * locked and the wait was interrupted by a signal.  The reason for this function is to allow the
 * stream close procedure to sleep while waiting for a listening stream to complete accepting a
 * connection on the closing stream before proceeding.
 */
STATIC struct sctp *
sctp_sleeplock(queue_t *q)
{
	struct sctp *sp = SCTP_PRIV(q);

	if (unlikely(test_and_set_bit(0, &sp->users))) {
		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue(&sp->waitq, &wait);
		for (;;) {
			set_current_state(TASK_UNINTERRUPTIBLE);
			set_bit(3, &sp->users);
			if (!test_and_set_bit(0, &sp->users))
				break;
			schedule();
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sp->waitq, &wait);
	}
	return (sp);
}

/**
 * sctp_unlockq: - unlock a private structure
 * @sp: pointer to locked private structure
 *
 * Waiting queues will be enabled and waiting processes will be woken up.  Not normally used by the
 * close procedure, just the queue procedures when unlocking a locked private structure.
 */
STATIC inline fastcall void
sctp_unlockq(struct sctp *sp)
{
	long users;

	users = xchg(&sp->users, 0x00);

	if (users & 0x02)
		enableq(sp->rq);
	if (users & 0x04)
		enableq(sp->wq);
	if (users & 0x08)
		wake_up_all(&sp->waitq);
}

/*
 *  =========================================================================
 *
 *  SCTP Provider --> SCTP User Interface Primitives
 *
 *  =========================================================================
 *
 *  There must be defined by the specific SCTP User and linked.
 *
 *  CONN_IND:   provides indication of a incoming connection from the peer and provides a pointer to
 *		the connection indication structure which contains information from the verified
 *		cookie in the COOKIE-ECHO message.  The interface should indicate the connection to
 *		its user and buffer the indication as required.  A return value of non-zero
 *		indicates that the interface could not buffer the connection indication.  Any data
 *		received in the COOKIE-ECHO message will be separately indicated with the DATA_IND
 *		primitive.
 *
 *  CONN_CON:   provides confirmation that the connection has been established.This is called when a
 *		COOKIE-ACK has been successfully received.  Any data received with the SCTP message
 *		that contained the COOKIE-ACK will be separately indicated with the data indication
 *		primitive.
 *
 *  DATA_IND:   provides indication of data that was received in sequence for the specified message
 *		control block parameters.  The message control block parameters of interest are PPI,
 *		SID, and SSN.  Also, the MORE argument indicates that there are further data
 *		indications that make up the same data record.  The M_DATA block provided contains a
 *		message control block and the data payload.
 *
 *  DATACK_IND: provides indication that the message with the given message control block parameters
 *		was received in entirety and positively acknowledged by the peer.  Message control
 *		block parameters of interest include SID and SSN.  Also of interest might be the per
 *		chunk statistics present in the control block (how many times sent, delay) This only
 *		applies to ordered data unless a token was provided on write.  The M_DATA block
 *		provided contains only a message control block and no data.
 *
 *  RESET_IND:  provids indication that the association has been reset as a result of an association
 *		restart condition.  It also means that any unacknowledged transmit data will be
 *		discarded and that stream sequence numbers are being reset.
 *
 *  DISCON_IND: provides indication that either an outstanding connection indication or an existing
 *		connection has been abotively disconnected for the provided reason.  When the CP
 *		argument is included, it identifies the connection indication being disconnected.
 *		When the CP argument is NULL, the socket or stream to which the DISCON_IND is sent
 *		is the one being disconnected.  In SCTP no data is ever associated with an
 *		DISCON_IND.
 *
 *  ORDREL_IND: provides indication that an existing connection has received a SHUTDOWN or
 *		SHUTDOWN-ACK from the peer.
 *
 *  RETR_IND:   provides indication of a retrieved message with the given message control block
 *		parameters.  Message control block parameters of interest are SID and SSN, STATUS
 *		and statistics associated with the data.  The M_DATA block provided contains a
 *		message control block and the data payload.
 *
 *  RETR_CON:   provides confirmation that all unsent and unacked data has been retrieved.
 *
 */
struct sctp_ifops {
	int (*conn_ind) (struct sctp * sp, mblk_t *cp);
	int (*conn_con) (struct sctp * sp);
	int (*data_ind) (struct sctp * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn,
			 uint ord, uint more, mblk_t *dp);
	int (*datack_ind) (struct sctp * sp, uint32_t ppi, uint16_t sid, uint16_t ssn,
			   uint32_t tsn);
	int (*reset_ind) (struct sctp * sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *cp);
	int (*reset_con) (struct sctp * sp);
	int (*discon_ind) (struct sctp * sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *cp);
	int (*ordrel_ind) (struct sctp * sp);
	int (*retr_ind) (struct sctp * sp, mblk_t *dp);
	int (*retr_con) (struct sctp * sp);
};

/*
 *  =========================================================================
 *
 *  SCTP User Interface --> SCTP Provider Primitives
 *
 *  =========================================================================
 *
 *  BIND_REQ:   requests that the provider bind the socket or stream to an SCTP port and address
 *		list and set the requested number of connection indications.  The provider allocates
 *		any necessary resources for binding the socket or stream.
 *
 *  CONN_REQ:   requests that the provider initiate an association with the provided data (if any).
 *		This will initiate an association by starting the
 *		INIT/INIT-ACK/COOKIE-ECHO/COOKIE-ACK procedure.  The user will either receive and
 *		error number in return, or will receive indication via the DISCON_IND or CONN_CON
 *		callbacks that the connection has failed or was successful.
 *
 *  CONN_RES:   responds to a previous connection indication from the SCTP provider.  The connection
 *		response includes the conndication indication that was earlier provided and a
 *		pointer to the accepting socket or stream  the accepting stream may be the same as
 *		the responding stream.  Any data provided will be bundled with the COOKIE-ACK.
 *
 *  DATA_REQ:   requests that the provided data with the provide message control block parameters be
 *		sent on the connection.  Message control block parameters of interest are PPI, SID,
 *		ordered flag, receipt confirmation flag.  The M_DATA block provided contains a
 *		message control block and the data payload.
 *
 *  DATACK_REQ: requests that the provider acknowledge receipt of a previous DATA_IND with the given
 *		transmit sequence number.  (This will also acknowledge any previous data fragments
 *		that made up the data record that ended with this TSN.)
 *
 *  RESET_CON:  used to acknowledge a previous reset indication.  This sends a COOKIE ACK in an SCTP
 *		restart scenario.
 *
 *  DISCON_REQ: requests that either the the requesting socket or stream be aborted or that the
 *		connection indication provided by rejected.  This results in an ABORT being sent.
 *
 *  ORDREL_REQ: request that the SCTP provider accept no more data for transmission on the
 *		connection and inform the other side that it is finished sending data.  This results
 *		in a SHUTDOWN or SHUTDOWN-ACK being sent.
 *
 *  UNBIND_REQ: requests that the provider unbind the socket or stream in preparation for having the
 *		socket or stream closed.  The provider frees any and all resources associated with
 *		the socket or stream regardless of what state the socket or stream is in.
 *
 */
STATIC fastcall int sctp_bind_req(struct sctp *sp, uint16_t sport, struct sockaddr_in *ssin,
				  size_t snum, t_uscalar_t cons);
STATIC fastcall int sctp_conn_req(struct sctp *sp, uint16_t dport, struct sockaddr_in *dsin,
				  size_t dnum, mblk_t *dp);
STATIC fastcall int sctp_conn_res(struct sctp *sp, mblk_t *cp, struct sctp *ap, mblk_t *dp);

STATIC fastcall int sctp_data_req(struct sctp *sp, uint32_t ppi, uint16_t sid, uint ord, uint more,
				  uint rctp, mblk_t *dp);
STATIC fastcall int sctp_reset_req(struct sctp *sp);
STATIC fastcall int sctp_reset_res(struct sctp *sp);
STATIC fastcall int sctp_discon_req(struct sctp *sp, mblk_t *cp);
STATIC fastcall int sctp_ordrel_req(struct sctp *sp);
STATIC fastcall int sctp_unbind_req(struct sctp *sp);

/*
 *  =========================================================================
 *
 *  SCTP Peer --> SCTP Primitives (Receive Messages)
 *
 *  =========================================================================
 */
noinline fastcall int sctp_recv_msg_slow(struct sctp *sp, mblk_t *mp);
noinline fastcall int sctp_recv_err(struct sctp *sp, mblk_t *mp);

/*
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */
#define SCTP_FR_COUNT 4

/* STREAMS needs to redefine these for streams structure instead of socket. */
#ifdef SCTP_CONFIG_ECN
#undef INET_ECN_xmit
#define	INET_ECN_xmit(sp) do { (sp)->inet.tos |= 2; } while (0)
#undef INET_ECN_dontxmit
#define	INET_ECN_dontxmit(sp) do { (sp)->inet.tos &= ~3; } while (0)
#endif				/* SCTP_CONFIG_ECN */

/*
 *  =========================================================================
 *
 *  CHECKSUMS
 *
 *  =========================================================================
 */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
/*
 *  -------------------------------------------------------------------------
 *
 *  Adler-32 Checksum
 *
 *  -------------------------------------------------------------------------
 */
#include "sctp_adler32.h"
/*
 *  This relies on a characteristic of the adler32 checksum.  The checksum is two sums:
 *
 *      s1[i] = s1[i-1] + (s1 - 1)
 *      s2[i] = s2[i-1] + s2 + k*(s1[i-1] - 1)
 *
 *  This works as long as the two buffers have a total length < NMAX above.  When the partial sums
 *  are complete, it is necessary to apply the %=BASE to the result before combining it as in the
 *  calculation above.
 */
STATIC INLINE uint32_t
__add_adler32(uint32_t csum1, uint32_t csum2, uint16_t l2)
{
	uint16_t s1 = (csum1);
	uint16_t s2 = (csum1 >> 16);
	uint16_t p1 = (csum2);
	uint16_t p2 = (csum2 >> 16);

	return ((s2 + p2 + (l2 * (s1 - 1))) << 16) | (s1 + p1);
}
STATIC INLINE uint32_t
__final_adler32(uint32_t csum)
{
	uint16_t s1 = (csum);
	uint16_t s2 = (csum >> 16);

	s1 %= BASE;
	s2 %= BASE;
	return (s2 << 16) | s1;
}

#define CP1(i) { s1 += (dst[i]=src[i]); s2 += s1; }
#define CP2(i)  CP1(i); CP1(i+1);
#define CP4(i)  CP2(i); CP2(i+2);
#define CP8(i)  CP4(i); CP4(i+4);
#define CP16    CP8(0); CP8(8);
STATIC INLINE uint32_t
__adler32_partial_copy_from_user(register const unsigned char *src, register unsigned char *dst,
				 int len, uint32_t sum, int *errp)
{
	register uint16_t s1 = sum;
	register uint16_t s2 = sum >> 16;

	(void) errp;
	if (!src)
		return 1L;
	while (len > 0) {
		while (len >= 16) {
			CP16;
			src += 16;
			dst += 16;
			len -= 16;
		}
		if (len != 0)
			do {
				s1 += (*dst++ = *src++);
				s2 += s1;
			} while (--len);
	}
	return (s2 << 16) | s1;
}
STATIC INLINE uint32_t
adler32_and_copy_from_user(const char *src, char *dst, int len, int sum, int *errp)
{
	if (access_ok(VERIFY_READ, src, len))
		return __adler32_partial_copy_from_user(src, dst, len, sum, errp);
	if (len)
		*errp = -EFAULT;
	return sum;
}
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
/*
 *  -------------------------------------------------------------------------
 *
 *  CRC-32C Checksum
 *
 *  -------------------------------------------------------------------------
 */
#ifdef SCTP_CONFIG_CRC_32C
#include "sctp_crc32c.h"
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  CHECKSUM
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE uint32_t
cksum(struct sctp *sp, void *buf, size_t len)
{
	switch (sp->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		return crc32c(~0, buf, len);
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return adler32(1, buf, len);
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
	}
}

#if 0				/* never used */
STATIC INLINE uint32_t
__add_cksum(struct sctp *sp, uint32_t csum1, uint32_t csum2, uint16_t l2)
{
	switch (sp->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		return __add_crc32c(csum1, csum2, l2);
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return __add_adler32(csum1, csum2, l2);
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
	}
}
STATIC INLINE uint32_t
__final_cksum(struct sctp *sp, uint32_t csum)
{
	switch (sp->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		return __final_crc32c(csum);
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return __final_adler32(csum);
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
	}
}
#endif
STATIC INLINE int
cksum_verify(uint32_t csum, void *buf, size_t len)
{
#if defined(SCTP_CONFIG_CRC_32C)||!defined(SCTP_CONFIG_ADLER_32)
	if (csum != crc32c(~0, buf, len))
#endif				/* defined(SCTP_CONFIG_CRC_32C)||!defined(SCTP_CONFIG_ADLER_32) */
#ifdef SCTP_CONFIG_ADLER_32
		if (csum != adler32(1, buf, len))
#endif				/* SCTP_CONFIG_ADLER_32 */
			return (0);
	return (1);
}
STATIC INLINE int
cksum_sp_verify(struct sctp *sp, uint32_t csum, void *buf, size_t len)
{
	switch (sp->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		if (csum != crc32c(~0, buf, len))
			return (0);
		return (1);
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		if (csum != adler32(1, buf, len))
			return (0);
		return (1);
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
	}
}
STATIC INLINE uint32_t
cksum_generate(void *buf, size_t len)
{
	uint32_t csum = 0;

#if defined(SCTP_CONFIG_CRC_32C)||!defined(SCTP_CONFIG_ADLER_32)
	csum = crc32c(~0, buf, len);
#else
#ifdef SCTP_CONFIG_ADLER_32
	csum = adler32(1, buf, len);
#endif
#endif				/* defined(SCTP_CONFIG_ADLER_32) */
	return (csum);
}

/*
 *  =========================================================================
 *
 *  CACHES
 *
 *  =========================================================================
 *  Note that the STREAMS version has an extra cache for the SCTP private structure.  This is
 *  contained inside the sock structure on the Linux Natvie (Sockets) version.
 */
STATIC kmem_cachep_t sctp_sctp_cachep = NULL;
STATIC kmem_cachep_t sctp_bind_cachep = NULL;
STATIC kmem_cachep_t sctp_dest_cachep = NULL;
STATIC kmem_cachep_t sctp_srce_cachep = NULL;
STATIC kmem_cachep_t sctp_strm_cachep = NULL;
STATIC void
sctp_init_caches(void)
{
	if (!sctp_sctp_cachep
	    && !(sctp_sctp_cachep =
		 kmem_cache_create("sctp_sctp_cachep", sizeof(struct sctp), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s:Cannot alloc sctp_sctp_cachep.\n", __FUNCTION__);
	if (!sctp_bind_cachep
	    && !(sctp_bind_cachep =
		 kmem_cache_create("sctp_bind_bucket", sizeof(struct sctp_bind_bucket), 0,
				   SLAB_HWCACHE_ALIGN, NULL, NULL)))
		panic("%s: Cannot alloc sctp_bind cache.\n", __FUNCTION__);
	if (!sctp_dest_cachep
	    && !(sctp_dest_cachep =
		 kmem_cache_create("sctp_daddr", sizeof(struct sctp_daddr), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s: Cannot alloc sctp_daddr cache.\n", __FUNCTION__);
	if (!sctp_srce_cachep
	    && !(sctp_srce_cachep =
		 kmem_cache_create("sctp_saddr", sizeof(struct sctp_saddr), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s: Cannot alloc sctp_saddr cache.\n", __FUNCTION__);
	if (!sctp_strm_cachep
	    && !(sctp_strm_cachep =
		 kmem_cache_create("sctp_strm", sizeof(struct sctp_strm), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s: Cannot alloc sctp_strm cache.\n", __FUNCTION__);
	return;
}

#if defined SCTP_CONFIG_MODULE
STATIC void
sctp_term_caches(void)
{
	if (sctp_sctp_cachep)
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sctp_sctp_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_sctp_cachep", __FUNCTION__);
#else
		kmem_cache_destroy(sctp_sctp_cachep);
#endif
	if (sctp_bind_cachep)
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sctp_bind_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_bind_cachep", __FUNCTION__);
#else
		kmem_cache_destroy(sctp_bind_cachep);
#endif
	if (sctp_dest_cachep)
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sctp_dest_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_dest_cachep", __FUNCTION__);
#else
		kmem_cache_destroy(sctp_dest_cachep);
#endif
	if (sctp_srce_cachep)
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sctp_srce_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_srce_cachep", __FUNCTION__);
#else
		kmem_cache_destroy(sctp_srce_cachep);
#endif
	if (sctp_strm_cachep)
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sctp_strm_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_strm_cachep", __FUNCTION__);
#else
		kmem_cache_destroy(sctp_strm_cachep);
#endif
	return;
}
#endif				/* defined SCTP_CONFIG_MODULE */
STATIC INLINE sctp_t *
sctp_get(void)
{
	sctp_t *sp;

	if ((sp = kmem_cache_alloc(sctp_sctp_cachep, GFP_ATOMIC))) {
		bzero(sp, sizeof(*sp));
		atomic_set(&sp->refcnt, 1);
	}
	return (sp);
}
STATIC INLINE void
sctp_hold(sctp_t * sp)
{
	if (sp)
		atomic_inc(&sp->refcnt);
}
STATIC INLINE void
sctp_put(sctp_t * sp)
{
	if (sp)
		if (atomic_dec_and_test(&sp->refcnt)) {
			_ctrace(kmem_cache_free(sctp_sctp_cachep, sp));
		}
}

STATIC INLINE struct sctp_daddr *
sctp_dget(void)
{
	struct sctp_daddr *sd;

	if ((sd = kmem_cache_alloc(sctp_dest_cachep, GFP_ATOMIC))) {
		bzero(sd, sizeof(*sd));
		atomic_set(&sd->refcnt, 1);
	}
	return (sd);
}
STATIC INLINE void
sctp_dhold(struct sctp_daddr *sd)
{
	if (sd)
		atomic_inc(&sd->refcnt);
}
STATIC INLINE void
sctp_dput(struct sctp_daddr *sd)
{
	if (sd)
		if (atomic_dec_and_test(&sd->refcnt)) {
			sctp_t *sp = xchg(&sd->sp, NULL);

			sctplogno(sp,
				  "Deallocating destination address %d.%d.%d.%d",
				  (sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
				  (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff);
			_ctrace(kmem_cache_free(sctp_dest_cachep, sd));
			sctp_put(sp);
		}
}

/*
 *  =========================================================================
 *
 *  SAFE BUFCALLS
 *
 *  =========================================================================
 *  Whereas sockets allocates most buffers in user mode, where it is possible to simply retrun an
 *  error to a system call to the user, the STREAMS driver runs at soft interrupt and there is no
 *  user present.  Therefore it is necessary to defer STREAMS scheduling to wait for the
 *  availability of a buffer.  This is done with bufcalls.  However, for LiS and a number of other
 *  STREAMS implementations, bufcalls are unsafe.  Here we do reference counting agains the STREAMS
 *  private structure when buffer calls are issued and when they complete.  STREAMS private
 *  structures will not be deallocated until the buffer calls are complete.
 */

STATIC streamscall __unlikely void
sctp_bufsrv(long data)
{
	struct sctp *sp = (struct sctp *) data;

	if (xchg(&sp->bcid, 0)) {
		if (sp->rq)
			qenable(sp->rq);
		if (sp->wq)
			qenable(sp->wq);
		sctp_put(sp);
	}
}
noinline fastcall __unlikely void
sctp_unbufcall(struct sctp *sp)
{
	int bcid;

	if ((bcid = xchg(&sp->bcid, 0))) {
		unbufcall(bcid);
		sctp_put(sp);
	}
}
noinline fastcall __unlikely void
sctp_bufcall(struct sctp *sp, size_t size, int prior)
{
	int bcid;

	sctp_hold(sp);
	if ((bcid = xchg(&sp->bcid, bufcall(size, prior, &sctp_bufsrv, (long) sp)))) {
		unbufcall(bcid);
		sctp_put(sp);
	}
}
STATIC INLINE fastcall __unlikely mblk_t *
sctp_allocb(struct sctp *sp, size_t size, int prior)
{
	mblk_t *mp;

	if (!(mp = allocb(size, prior)) && sp)
		sctp_bufcall(sp, size, prior);
	return (mp);
}
STATIC INLINE fastcall __unlikely mblk_t *
sctp_dupb(struct sctp *sp, mblk_t *bp, size_t size)
{
	mblk_t *mp = NULL;

#if 1
	if (bp->b_datap->db_ref >= 247)
		goto trycopy;
	if (!(mp = dupb(bp)) && sp) {
#if 0
		if (bp->b_datap->db_ref == 255)
			sctplogerr(sp, "%s() db_ref is 255", __FUNCTION__);
#endif
#if 0
		if (bp->b_datap->db_ref == 255) {
			// dump_stack();
			return (NULL);
		}
#endif
		if (bp->b_datap->db_ref >= 247) {
		      trycopy:
			/* do not do a deep copy, just the requested length */
			if (size > 0 && (mp = sctp_allocb(sp, size, BPRI_MED))) {
				mp->b_datap->db_type = bp->b_datap->db_type;
				mp->b_band = bp->b_band;
				mp->b_flag = bp->b_flag;
				mp->b_csum = bp->b_csum;
				bcopy(bp->b_rptr, mp->b_rptr, size);
				mp->b_wptr = mp->b_rptr + size;
				return (mp);
			}
		}
#if 0
		if (bp->b_datap->db_ref == 255) {
			int size = bp->b_datap->db_lim - bp->b_datap->db_base;

			/* failed because of reference wrap, try a deep copy */
			if ((mp = sctp_allocb(sp, size, BPRI_MED))) {
				mp->b_datap->db_type = bp->b_datap->db_type;
				mp->b_band = bp->b_band;
				mp->b_flag = bp->b_flag;
				mp->b_csum = bp->b_csum;
				mp->b_rptr = bp->b_rptr +
				    (mp->b_datap->db_base - bp->b_datap->db_base);
				mp->b_wptr = bp->b_wptr +
				    (mp->b_datap->db_base - bp->b_datap->db_base);
				return (mp);
			}
		}
#endif
		sctp_bufcall(sp, FASTBUF, BPRI_MED);
	}
	return (mp);
#else
	/* try a deep copy instead */
	int size = bp->b_datap->db_lim - bp->b_datap->db_base;

	if ((mp = sctp_allocb(sp, size, BPRI_MED))) {
		mp->b_datap->db_type = bp->b_datap->db_type;
		mp->b_band = bp->b_band;
		mp->b_flag = bp->b_flag;
		mp->b_csum = bp->b_csum;
		mp->b_rptr = bp->b_rptr + (mp->b_datap->db_base - bp->b_datap->db_base);
		mp->b_wptr = bp->b_wptr + (mp->b_datap->db_base - bp->b_datap->db_base);
		return (mp);
	}
	if (sp)
		sctp_bufcall(sp, size, BPRI_MED);
	return (mp);
#endif
}
STATIC INLINE fastcall __unlikely mblk_t *
sctp_dupmsg(struct sctp *sp, mblk_t *bp)
{
	mblk_t *mp;

	if (!(mp = dupmsg(bp)) && sp)
		sctp_bufcall(sp, FASTBUF, BPRI_MED);
	return (mp);
}

/*
 *  =========================================================================
 *
 *  SAFE TIMEOUTS
 *
 *  =========================================================================
 */
/*
 *  TODO: Under LiS, STREAMS timers are horribly inefficient.  We should provide a option here on
 *  the STREAMS version to just use the Linux timers now that we have this aligned with the Linux
 *  Native Sockets version.
 */

#define SCTP_TBIT_INIT		0
#define SCTP_TBIT_COOKIE	1
#define SCTP_TBIT_SHUTDOWN	2
#define SCTP_TBIT_GUARD		3
#define SCTP_TBIT_SACK		4
#define SCTP_TBIT_ASCONF	5
#define SCTP_TBIT_LIFE		6
#define SCTP_TBIT_HEARTBEAT	7
#define SCTP_TBIT_RETRANS	8
#define SCTP_TBIT_IDLE		9

/*
 * Under this approach, if the timeout fails to acquire the structure lock, a bit is set in the
 * private structure (and possibly address structure) indicating that a timeout is deferred and the
 * queue's read service procedure will be scheduled when the structure is unlocked.  When the read
 * queue service procedure runs, it acquires structure locks, checks for deferred timeouts and runs
 * them.  Otherwise, the timeout can be executed immediately because we use qtimeout()/quntimeout()
 * that runs like a put() procedure.
 */
STATIC inline fastcall void
sp_timeout(struct sctp *sp, void fastcall (*fcn)(struct sctp *), uint bit)
{
	if (sctp_trylock_timer(sp, NULL, bit)) {
		(*fcn)(sp);
		sctp_unlockq(sp);
	}
}
STATIC inline fastcall void
sd_timeout(struct sctp_daddr *sd, void fastcall (*fcn)(struct sctp_daddr *), uint bit)
{
	if (sctp_trylock_timer(sd->sp, sd, bit)) {
		(*fcn)(sd);
		sctp_unlockq(sd->sp);
	}
}

STATIC streamscall void sctp_init_timeout(void *arg);
STATIC streamscall void sctp_cookie_timeout(void *arg);
STATIC streamscall void sctp_shutdown_timeout(void *arg);
STATIC streamscall void sctp_guard_timeout(void *arg);
STATIC streamscall void sctp_sack_timeout(void *arg);
STATIC streamscall void sctp_asconf_timeout(void *arg);
STATIC streamscall void sctp_life_timeout(void *arg);
STATIC streamscall void sctp_heartbeat_timeout(void *arg);
STATIC streamscall void sctp_retrans_timeout(void *arg);
STATIC streamscall void sctp_idle_timeout(void *arg);

STATIC inline int
sctp_timeout_pending(volatile toid_t *tidp)
{
	return (*tidp != (toid_t) 0);
}

STATIC inline toid_t
sp_timer(struct sctp *sp, volatile toid_t *tidp, void streamscall (*fcn)(void *), clock_t ticks)
{
	toid_t tid_new, tid_old;

	tid_new = qtimeout(sp->rq, fcn, sp, ticks);
	if ((tid_old = xchg(tidp, tid_new)))
		quntimeout(sp->rq, tid_old);

	return (tid_new);
}
STATIC inline toid_t
sd_timer(struct sctp_daddr *sd, volatile toid_t *tidp, void streamscall (*fcn)(void *), clock_t ticks)
{
	toid_t tid_new, tid_old;

	if (unlikely(ticks < 2)) {
		sctplogerr(sd->sp, "%s() ticks is %d, setting to 2", __FUNCTION__, (int) ticks);
		ticks = 2;
	}
	tid_new = qtimeout(sd->sp->rq, fcn, sd, ticks);
	if ((tid_old = xchg(tidp, tid_new)))
		quntimeout(sd->sp->rq, tid_old);

	return (tid_new);
}

STATIC inline toid_t
sp_timer_init(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "starting timer T1-init, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer(sp, &sp->timer_init, &sctp_init_timeout, ticks);
}
STATIC inline toid_t
sp_timer_cookie(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "starting timer T1-cookie, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer(sp, &sp->timer_cookie, &sctp_cookie_timeout, ticks);
}
STATIC inline toid_t
sp_timer_shutdown(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "starting timer T4-shutdown, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer(sp, &sp->timer_shutdown, &sctp_shutdown_timeout, ticks);
}
STATIC inline toid_t
sp_timer_guard(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "starting timer T5-guard, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer(sp, &sp->timer_guard, &sctp_guard_timeout, ticks);
}
STATIC inline toid_t
sp_timer_sack(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "starting timer T2-sack, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer(sp, &sp->timer_sack, &sctp_sack_timeout, ticks);
}
STATIC inline toid_t
sp_timer_asconf(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "starting timer T-asconf, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer(sp, &sp->timer_asconf, &sctp_asconf_timeout, ticks);
}
STATIC inline toid_t
sp_timer_life(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "starting timer T-life, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer(sp, &sp->timer_life, &sctp_life_timeout, ticks);
}
STATIC inline toid_t
sd_timer_heartbeat(struct sctp_daddr *sd, clock_t ticks)
{
	sctplogte(sd->sp, "starting timer T-heartbeat, %d ticks, %p", (int) ticks, sd);
	if (unlikely(ticks < 1)) {
		sctplogte(sd->sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sd_timer(sd, &sd->timer_heartbeat, &sctp_heartbeat_timeout, ticks);
}
STATIC inline toid_t
sd_timer_retrans(struct sctp_daddr *sd, clock_t ticks)
{
	sctplogte(sd->sp, "starting timer T3-retrans, %d ticks, %p", (int) ticks, sd);
	if (unlikely(ticks < 1)) {
		sctplogte(sd->sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sd_timer(sd, &sd->timer_retrans, &sctp_retrans_timeout, ticks);
}
STATIC inline toid_t
sd_timer_idle(struct sctp_daddr *sd, clock_t ticks)
{
	sctplogte(sd->sp, "starting timer T-idle, %d ticks, %p", (int) ticks, sd);
	if (unlikely(ticks < 1)) {
		sctplogte(sd->sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sd_timer(sd, &sd->timer_idle, &sctp_idle_timeout, ticks);
}

STATIC inline toid_t
sp_timer_cond(struct sctp *sp, volatile toid_t *tidp, void streamscall (*fcn)(void *), clock_t ticks)
{
	return (*tidp ? 0 : sp_timer(sp, tidp, fcn, ticks));
}
STATIC inline toid_t
sd_timer_cond(struct sctp_daddr *sd, volatile toid_t *tidp, void streamscall (*fcn)(void *), clock_t ticks)
{
	return (*tidp ? 0 : sd_timer(sd, tidp, fcn, ticks));
}

STATIC inline toid_t
sp_timer_cond_init(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "conditional start timer T1-init, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer_cond(sp, &sp->timer_init, &sctp_init_timeout, ticks);
}
STATIC inline toid_t
sp_timer_cond_cookie(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "conditional start timer T1-cookie, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer_cond(sp, &sp->timer_cookie, &sctp_cookie_timeout, ticks);
}
STATIC inline toid_t
sp_timer_cond_shutdown(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "conditional start timer T4-shutdown, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer_cond(sp, &sp->timer_shutdown, &sctp_shutdown_timeout, ticks);
}
STATIC inline toid_t
sp_timer_cond_guard(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "conditional start timer T5-guard, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer_cond(sp, &sp->timer_guard, &sctp_guard_timeout, ticks);
}
STATIC inline toid_t
sp_timer_cond_sack(struct sctp *sp, clock_t ticks)
{
	sctplogda(sp, "conditional start timer T2-sack, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogda(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer_cond(sp, &sp->timer_sack, &sctp_sack_timeout, ticks);
}
STATIC inline toid_t
sp_timer_cond_asconf(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "conditional start timer T-asconf, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer_cond(sp, &sp->timer_asconf, &sctp_asconf_timeout, ticks);
}
STATIC inline toid_t
sp_timer_cond_life(struct sctp *sp, clock_t ticks)
{
	sctplogte(sp, "conditional start timer T-life, %d ticks", (int) ticks);
	if (unlikely(ticks < 1)) {
		sctplogte(sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sp_timer_cond(sp, &sp->timer_life, &sctp_life_timeout, ticks);
}
STATIC inline toid_t
sd_timer_cond_heartbeat(struct sctp_daddr *sd, clock_t ticks)
{
	sctplogte(sd->sp, "conditional start timer T-heartbeat, %d ticks, %p", (int) ticks, sd);
	if (unlikely(ticks < 1)) {
		sctplogte(sd->sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sd_timer_cond(sd, &sd->timer_heartbeat, &sctp_heartbeat_timeout, ticks);
}
STATIC inline toid_t
sd_timer_cond_retrans(struct sctp_daddr *sd, clock_t ticks)
{
	sctplogda(sd->sp, "conditional start timer T3-retrans, %d ticks, %p", (int) ticks, sd);
	if (unlikely(ticks < 1)) {
		sctplogda(sd->sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sd_timer_cond(sd, &sd->timer_retrans, &sctp_retrans_timeout, ticks);
}
STATIC inline toid_t
sd_timer_cond_idle(struct sctp_daddr *sd, clock_t ticks)
{
	sctplogte(sd->sp, "conditional start timer T-idle, %d ticks, %p", (int) ticks, sd);
	if (unlikely(ticks < 1)) {
		sctplogte(sd->sp, "%s() ticks is %d, setting to 1", __FUNCTION__, (int) ticks);
		ticks = 1;
	}
	return sd_timer_cond(sd, &sd->timer_idle, &sctp_idle_timeout, ticks);
}

STATIC inline toid_t
sp_timer_cancel(struct sctp *sp, volatile toid_t *tidp)
{
	toid_t tid;

	if ((tid = xchg(tidp, (toid_t)0)) != (toid_t) 0)
		quntimeout(sp->rq, tid);
	return (tid);
}

STATIC inline toid_t
sp_timer_cancel_init(struct sctp *sp)
{
	sctplogte(sp, "stopping timer T1-init");
	return sp_timer_cancel(sp, &sp->timer_init);
}
STATIC inline toid_t
sp_timer_cancel_cookie(struct sctp *sp)
{
	sctplogte(sp, "stopping timer T1-cookie");
	return sp_timer_cancel(sp, &sp->timer_cookie);
}
STATIC inline toid_t
sp_timer_cancel_shutdown(struct sctp *sp)
{
	sctplogte(sp, "stopping timer T4-shutdown");
	return sp_timer_cancel(sp, &sp->timer_shutdown);
}
STATIC inline toid_t
sp_timer_cancel_guard(struct sctp *sp)
{
	sctplogte(sp, "stopping timer T5-guard");
	return sp_timer_cancel(sp, &sp->timer_guard);
}
STATIC inline toid_t
sp_timer_cancel_sack(struct sctp *sp)
{
	sctplogte(sp, "stopping timer T2-sack");
	return sp_timer_cancel(sp, &sp->timer_sack);
}
STATIC inline toid_t
sp_timer_cancel_asconf(struct sctp *sp)
{
	sctplogte(sp, "stopping timer T-asconf");
	return sp_timer_cancel(sp, &sp->timer_asconf);
}
STATIC inline toid_t
sp_timer_cancel_life(struct sctp *sp)
{
	sctplogte(sp, "stopping timer T-life");
	return sp_timer_cancel(sp, &sp->timer_life);
}
STATIC inline toid_t
sd_timer_cancel_heartbeat(struct sctp_daddr *sd)
{
	sctplogte(sd->sp, "stopping timer T-heartbeat, %p", sd);
	return sp_timer_cancel(sd->sp, &sd->timer_heartbeat);
}
STATIC inline toid_t
sd_timer_cancel_retrans(struct sctp_daddr *sd)
{
	sctplogte(sd->sp, "stopping timer T3-retrans, %p", sd);
	return sp_timer_cancel(sd->sp, &sd->timer_retrans);
}
STATIC inline toid_t
sd_timer_cancel_idle(struct sctp_daddr *sd)
{
	sctplogte(sd->sp, "stopping timer T-idle, %p", sd);
	return sp_timer_cancel(sd->sp, &sd->timer_idle);
}

/*
 *  Find a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE struct sctp_daddr *
sctp_find_daddr(sctp_t * sp, uint32_t daddr)
{
	struct sctp_daddr *sd;

#ifndef SCTP_CONFIG_ADD_IP
	for (sd = sp->daddr; sd && sd->daddr != daddr; sd = sd->next) ;
#else				/* SCTP_CONFIG_ADD_IP */
	for (sd = sp->daddr; sd && !(sd->flags & SCTP_DESTF_UNUSABLE) && sd->daddr != daddr;
	     sd = sd->next) ;
#endif				/* SCTP_CONFIG_ADD_IP */
	return (sd);
}

/*
 *  Find a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE struct sctp_saddr *
sctp_find_saddr(sctp_t * sp, uint32_t saddr)
{
	struct sctp_saddr *ss;

	for (ss = sp->saddr; ss && ss->saddr != saddr; ss = ss->next) ;
	return (ss);
}

STATIC struct sctp_strm *sctp_strm_alloc(struct sctp_strm **stp, uint16_t sid, int *erp);

/*
 *  Find or Add an Inbound or Outbound Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE struct sctp_strm *
sctp_istrm_find(sctp_t * sp, uint16_t sid, int *errp)
{
	struct sctp_strm *st;

	if (!(st = sp->istr) || st->sid != sid)
		for (st = sp->istrm; st && st->sid != sid; st = st->next) ;
	if (st)
		sp->istr = st;	/* cache */
	else
		st = sctp_strm_alloc(&sp->istrm, sid, errp);
	return (st);
}
STATIC INLINE struct sctp_strm *
sctp_ostrm_find(sctp_t * sp, uint16_t sid, int *errp)
{
	struct sctp_strm *st;

	if (!(st = sp->ostr) || st->sid != sid)
		for (st = sp->ostrm; st && st->sid != sid; st = st->next) ;
	if (st)
		sp->ostr = st;	/* cache */
	else
		st = sctp_strm_alloc(&sp->ostrm, sid, errp);
	return (st);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DESTINATION ADDRESS HANDLING
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_daddr *
__sctp_daddr_alloc(sctp_t * sp, uint32_t daddr, int *errp)
{
	struct sctp_daddr *sd;
	int dat = inet_addr_type(daddr);

	assert(errp);
	__ensure(sp, *errp = -EFAULT; return (NULL));
	if (daddr == INADDR_ANY) {
		*errp = -EDESTADDRREQ;
		sctplogerr(sp, "cannot connect to INADDR_ANY");
		return (NULL);
	}
#if 0
	if (!(daddr & 0xff000000)) {	/* zeronet is illegal */
		assure(daddr & 0xff000000);
		*errp = -EADDRNOTAVAIL;
		seldom();
		return (NULL);
	}
#endif
	if (sp->daddr && inet_addr_type(sp->daddr->daddr) != dat) {
		*errp = -EADDRNOTAVAIL;
		sctplogno(sp, "skipping incompatible %d.%d.%d.%d",
			  (daddr >> 0) & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff,
			  (daddr >> 24) & 0xff);
		return (NULL);
	}
	if (dat != RTN_UNICAST && dat != RTN_LOCAL && dat != RTN_ANYCAST) {
		*errp = -EADDRNOTAVAIL;
		sctplogno(sp, "skipping non-unicast %d.%d.%d.%d",
			  (daddr >> 0) & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff,
			  (daddr >> 24) & 0xff);
		return (NULL);
	}
	/* TODO: need to check permissions (TACCES) for broadcast or multicast addresses and
	   whether host addresses are valid (TBADADDR). */
	if ((sd = sctp_dget())) {
		sctplogno(sp,
			  "allocating destination address %d.%d.%d.%d", (daddr >> 0) & 0xff,
			  (daddr >> 8) & 0xff, (daddr >> 16) & 0xff, (daddr >> 24) & 0xff);
		sctp_hold(sp);
		if ((sd->next = sp->daddr))
			sd->next->prev = &sd->next;
		sd->prev = &sp->daddr;
		sp->daddr = sd;
		sp->danum++;
		sd->sp = sp;
		sd->daddr = daddr;
		sd->mtu = 576;	/* fix up after routing */
		sd->dmps =
		    sd->mtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
		sd->ssthresh = 2 * sd->mtu;	/* fix up after routing */
		sd->cwnd = sd->mtu;	/* fix up after routing */
		/* per destination defaults */
		sd->hb_itvl = sp->hb_itvl;	/* heartbeat interval */
		sd->rto_max = sp->rto_max;	/* maximum RTO */
		sd->rto_min = sp->rto_min;	/* minimum RTO */
		sd->rto = sp->rto_ini;	/* initial RTO */
		sctplogno(sp, "sd->hb_itvl = %d", (int) sd->hb_itvl);
		sctplogno(sp, "sd->rto_max = %d", (int) sd->rto_max);
		sctplogno(sp, "sd->rto_min = %d", (int) sd->rto_min);
		sctplogno(sp, "sd->rto = %d", (int) sd->rto);
		sd->max_retrans = sp->rtx_path;	/* max path retrans */
		/* init timers */
		sd->timers = 0;
		sd->timer_heartbeat = 0;
		sd->timer_retrans = 0;
		sd->timer_idle = 0;
		return (sd);
	}
	*errp = -ENOMEM;
	rare();
	return (NULL);
}

/*
 *  Find or Add a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_daddr *
sctp_daddr_include(sctp_t * sp, uint32_t daddr, int *errp)
{
	struct sctp_daddr *sd;

	assert(errp);
	__ensure(sp, *errp = -EFAULT; return (NULL));
	if (!(sd = sctp_find_daddr(sp, daddr)))
		sd = __sctp_daddr_alloc(sp, daddr, errp);
	usual(sd);
	return (sd);
}

/*
 *  Free a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_daddr_free(struct sctp_daddr *sd)
{
	ensure(sd, return);
	sd->timers = 0;
	sd_timer_cancel_retrans(sd);
	sd_timer_cancel_heartbeat(sd);
	sd_timer_cancel_idle(sd);
	/* Need to free any cached IP routes.  */
	if (sd->dst_cache)
		dst_release(xchg(&sd->dst_cache, NULL));
	if (sd->sp)
		sd->sp->danum--;
	else
		swerr();
	if ((*sd->prev = sd->next))
		sd->next->prev = sd->prev;
	sctplogno(sd->sp,
		  "deallocating destination address %d.%d.%d.%d", (sd->daddr >> 0) & 0xff,
		  (sd->daddr >> 8) & 0xff, (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff);
	sd->next = NULL;
	sd->prev = &sd->next;
	sctp_dput(sd);
}

/*
 *  Free all Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_free_daddrs(sctp_t * sp)
{
	struct sctp_daddr *sd, *sd_next;

	ensure(sp, return);
	sd_next = sp->daddr;
	usual(sd_next);
	while ((sd = sd_next)) {
		sd_next = sd->next;
		__sctp_daddr_free(sd);
	}
	unless(sp->danum, sp->danum = 0);
	sp->taddr = NULL;
	sp->raddr = NULL;
	sp->caddr = NULL;
	sp->dport = 0;
}

#ifdef SCTP_CONFIG_ADD_IP
/*
 *  Add a Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_daddr *
sctp_add_daddr(sctp_t * sp, uint32_t daddr)
{
	int err = 0;

	/* TODO: Check that adding the address does not cause a transport endpoint conflict with an 
	   existing association. */
	return __sctp_daddr_alloc(sp, daddr, &err);
}

/*
 *  Delete a Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_del_daddr(struct sctp_daddr *sd)
{
	sctp_t *sp;
	mblk_t *mp;
	pl_t pl;

	ensure(sd, return);
	ensure(sd->sp, return);
	sp = sd->sp;
	if (!(sd->flags & SCTP_DESTF_UNUSABLE)) {
		/* IMPLEMENTATION NOTE:- This is the lazy way to do this, if we maintained
		   reference counts on sctp_daddr structures, then we could at least check if
		   walking all these queues was necessary. */
		pl = bufq_lock(&sp->rcvq);
		for (mp = bufq_head(&sp->rcvq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->rcvq, pl);
		pl = bufq_lock(&sp->expq);
		for (mp = bufq_head(&sp->expq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->expq, pl);
		pl = bufq_lock(&sp->sndq);
		for (mp = bufq_head(&sp->sndq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->sndq, pl);
		pl = bufq_lock(&sp->urgq);
		for (mp = bufq_head(&sp->urgq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->urgq, pl);
		pl = bufq_lock(&sp->oooq);
		for (mp = bufq_head(&sp->oooq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->oooq, pl);
		pl = bufq_lock(&sp->dupq);
		for (mp = bufq_head(&sp->dupq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->dupq, pl);
#if 1
		pl = bufq_lock(&sp->ackq);
		for (mp = bufq_head(&sp->ackq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->ackq, pl);
#endif
		sd_timer_cancel_heartbeat(sd);
		sd_timer_cancel_idle(sd);
		sd->flags |= SCTP_DESTF_UNUSABLE;
		if (sd->dst_cache)
			dst_release(xchg(&sd->dst_cache, NULL));
	}
	if (!sctp_timeout_pending(&sd->timer_retrans) && !sd->in_flight) {
		pl = bufq_lock(&sp->rtxq);
		for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		bufq_unlock(&sp->rtxq, pl);
		__sctp_daddr_free(sd);
	}
}
#endif				/* SCTP_CONFIG_ADD_IP */

/*
 *  Allocate a group of Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_alloc_daddrs(sctp_t * sp, uint16_t dport, uint32_t *daddrs, size_t dnum)
{
	int err = 0, allocated = 0;

	__ensure(sp, return (-EFAULT));
	__ensure(daddrs || !dnum, return (-EFAULT));
	sp->dport = dport;
	if (dnum) {
		while (dnum--)
			if (!sctp_daddr_include(sp, daddrs[dnum], &err) && err) {
				rare();
				break;
			} else
				allocated++;
	} else
		usual(dnum);
	if (err) {
		abnormal(err);
		sctplogerr(sp, "returning error %d", err);
		__sctp_free_daddrs(sp);
		return (err);
	}
	return (allocated);
}

/*
 *  Allocate a group of Destination Socket Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_alloc_sock_daddrs(sctp_t * sp, uint16_t dport, struct sockaddr_in *daddrs, size_t dnum)
{
	int err = 0, allocated = 0;

	__ensure(sp, return (-EFAULT));
	__ensure(daddrs || !dnum, return (-EFAULT));
	unless(sp->daddr || sp->danum, __sctp_free_daddrs(sp));	/* start clean */
	if (dnum) {
		while (dnum--) {
			if (daddrs[dnum].sin_family != AF_INET || daddrs[dnum].sin_port != dport) {
				err = -EINVAL;
				break;
			}
			if (sctp_daddr_include(sp, daddrs[dnum].sin_addr.s_addr, &err))
				allocated++;
			else if (err) {
				rare();
				break;
			}
		}
	} else
		usual(dnum);
	if (err) {
		abnormal(err);
		__sctp_free_daddrs(sp);
		return (err);
	}
	return (allocated);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SOURCE ADDRESS HANDLING
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_saddr *
__sctp_saddr_alloc(sctp_t * sp, uint32_t saddr, int *errp)
{
	struct sctp_saddr *ss;

	assert(errp);
	__ensure(sp, *errp = -EFAULT;
	       return (NULL));
	if (saddr == INADDR_ANY) {
		sctplogno(sp, "skipping INADDR_ANY");
		return (NULL);
	}
	if (sp->saddr && LOOPBACK(sp->saddr->saddr) != LOOPBACK(saddr)) {
		*errp = -EADDRNOTAVAIL;
		sctplogno(sp, "skipping incompatible %d.%d.%d.%d",
			  (saddr >> 0) & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff,
			  (saddr >> 24) & 0xff);
		return (NULL);
	}
#ifdef sysctl_ip_nonlocal_bind
	if (!sysctl_ip_nonlocal_bind && inet_addr_type(saddr) != RTN_LOCAL) {
		*errp = -EADDRNOTAVAIL;
		sctplogno(sp, "skipping non-local %d.%d.%d.%d",
			  (saddr >> 0) & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff,
			  (saddr >> 24) & 0xff);
		return (NULL);
	}
#endif				/* sysctl_ip_nonlocal_bind */
	if ((ss = kmem_cache_alloc(sctp_srce_cachep, GFP_ATOMIC))) {
		sctplogno(sp, "allocating source address %d.%d.%d.%d",
			  (saddr >> 0) & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff,
			  (saddr >> 24) & 0xff);
		bzero(ss, sizeof(*ss));
		if ((ss->next = sp->saddr))
			ss->next->prev = &ss->next;
		ss->prev = &sp->saddr;
		sp->saddr = ss;
		sp->sanum++;
		ss->sp = sp;
		ss->saddr = saddr;
		return (ss);
	}
	*errp = -ENOMEM;
	return (ss);
}

/*
 *  Find or Add a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_saddr *
sctp_saddr_include(sctp_t * sp, uint32_t saddr, int *errp)
{
	struct sctp_saddr *ss;

	assert(errp);
	__ensure(sp, *errp = -EFAULT; return (NULL));
	if (!(ss = sctp_find_saddr(sp, saddr)))
		ss = __sctp_saddr_alloc(sp, saddr, errp);
	usual(ss);
	return (ss);
}

/*
 *  Free a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_saddr_free(struct sctp_saddr *ss)
{
	assert(ss);
	if (ss->sp)
		ss->sp->sanum--;
	else
		swerr();
	if ((*ss->prev = ss->next))
		ss->next->prev = ss->prev;
	sctplogno(ss->sp,
		  "deallocating source address %d.%d.%d.%d", (ss->saddr >> 0) & 0xff,
		  (ss->saddr >> 8) & 0xff, (ss->saddr >> 16) & 0xff, (ss->saddr >> 24) & 0xff);
	bzero(ss, sizeof(*ss));	/* debug */
	ss->prev = &ss->next;
	ss->next = NULL;
	_ctrace(kmem_cache_free(sctp_srce_cachep, ss));
}

/*
 *  Free all Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_free_saddrs(sctp_t * sp)
{
	struct sctp_saddr *ss, *ss_next;

	assert(sp);
	ss_next = sp->saddr;
	usual(ss_next);
	while ((ss = ss_next)) {
		ss_next = ss->next;
		__sctp_saddr_free(ss);
	}
	unless(sp->sanum, sp->sanum = 0);
	unless(sp->saddr, sp->saddr = NULL);
}

/*
 *  Allocate a group of Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_alloc_saddrs(sctp_t * sp, uint16_t sport, uint32_t *saddrs, size_t snum)
{
	int err = 0;

	__ensure(sp, return (-EFAULT));
	__ensure(saddrs || !snum, return (-EFAULT));
	sp->sport = sport;
	if (snum) {
		while (snum--)
			if (!sctp_saddr_include(sp, saddrs[snum], &err) && err) {
				rare();
				break;
			}
	} else
		usual(snum);
	if (err) {
		rare();
		__sctp_free_saddrs(sp);
	}
	return (err);
}

/*
 *  Allocate a group of Source Socket Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_alloc_sock_saddrs(sctp_t * sp, uint16_t sport, struct sockaddr_in *saddrs, size_t snum)
{
	int err = 0, allocated = 0;

	__ensure(sp, return (-EFAULT));
	__ensure(saddrs || !snum, return (-EFAULT));
	unless(sp->saddr || sp->sanum, __sctp_free_saddrs(sp));
	if (snum) {
		while (snum--) {
			if (saddrs[snum].sin_family != AF_INET || saddrs[snum].sin_port != sport) {
				err = -EINVAL;
				break;
			}
			if (sctp_saddr_include(sp, saddrs[snum].sin_addr.s_addr, &err))
				allocated++;
			else if (err) {
				rare();
				break;
			}
		}
	} else
		usual(snum);
	if (err) {
		abnormal(err);
		__sctp_free_saddrs(sp);
		return (err);
	}
	return (allocated);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP Stream handling
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate an Inbound or Outbound Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_strm *
sctp_strm_alloc(struct sctp_strm **stp, uint16_t sid, int *errp)
{
	struct sctp_strm *st;

	if ((st = kmem_cache_alloc(sctp_strm_cachep, GFP_ATOMIC))) {
		bzero(st, sizeof(*st));
		if ((st->next = (*stp)))
			st->next->prev = &st->next;
		st->prev = stp;
		(*stp) = st;
		st->sid = sid;
		st->ssn = -1;	/* SCTP IG 2.24 */
		return (st);
	}
	*errp = -ENOMEM;
	rare();
	return (NULL);
}

/*
 *  Free a Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_strm_free(struct sctp_strm *st)
{
	assert(st);
	if ((*st->prev = st->next))
		st->next->prev = st->prev;
	bzero(st, sizeof(*st));	/* debug */
	_ctrace(kmem_cache_free(sctp_strm_cachep, st));
}

/*
 *  Free all Streams
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_free_strms(sctp_t * sp)
{
	struct sctp_strm *st, *st_next;

	assert(sp);
	st_next = sp->ostrm;
	usual(st_next);
	while ((st = st_next)) {
		st_next = st->next;
		sctp_strm_free(st);
	}
	sp->ostr = NULL;
	st_next = sp->istrm;
	usual(st_next);
	while ((st = st_next)) {
		st_next = st->next;
		sctp_strm_free(st);
	}
	sp->istr = NULL;
}

/*
 *  =========================================================================
 *
 *  HASHES
 *
 *  =========================================================================
 */
struct sctp_bhash_bucket {
	rwlock_t lock;
	struct sctp_bind_bucket *list;
};
struct sctp_hash_bucket {
	rwlock_t lock;
	sctp_t *list;
};

STATIC unsigned int sctp_bhash_size = 0;

#define sctp_lhash_size (1<<6)
#define sctp_phash_size (1<<6)
#define sctp_cache_size (1<<6)
STATIC unsigned int sctp_vhash_size = 0;

#define sctp_thash_size (1<<6)

STATIC struct sctp_bhash_bucket *sctp_bhash;	/* bind */
STATIC struct sctp_hash_bucket sctp_lhash[sctp_lhash_size];	/* listen */
STATIC struct sctp_hash_bucket sctp_phash[sctp_phash_size];	/* p_tag */
STATIC struct sctp_hash_bucket sctp_cache[sctp_cache_size];	/* v_tag level 1 */
STATIC struct sctp_hash_bucket *sctp_vhash;	/* v_tag level 2 */
STATIC struct sctp_hash_bucket sctp_thash[sctp_thash_size];	/* tcb level 4 */

STATIC unsigned int sctp_bhash_order = 0;
STATIC unsigned int sctp_vhash_order = 0;

STATIC void
sctp_init_hashes(void)
{
	int order, i;
	unsigned long goal;

	/* size and allocate vtag hash table */
	goal = num_physpages >> (20 - PAGE_SHIFT);
	for (order = 0; (1 << order) < goal; order++) ;
	do {
		sctp_vhash_order = order;
		sctp_vhash_size = (1 << order) * PAGE_SIZE / sizeof(struct sctp_hash_bucket);
		sctp_vhash = (struct sctp_hash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (sctp_vhash == NULL && --order >= 0);
	if (!sctp_vhash)
		panic("%s: Failed to allocate SCTP established hash table\n", __FUNCTION__);
	/* size and allocate bind hash table */
	goal = (((1 << order) * PAGE_SIZE) / sizeof(struct sctp_bhash_bucket));
	if (goal > (64 * 1024)) {
		goal = (((64 * 1024) * sizeof(struct sctp_bhash_bucket)) / PAGE_SIZE);
		for (order = 0; (1 << order) < goal; order++) ;
	}
	do {
		sctp_bhash_order = order;
		sctp_bhash_size = (1 << order) * PAGE_SIZE / sizeof(struct sctp_bhash_bucket);
		sctp_bhash = (struct sctp_bhash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (sctp_bhash == NULL && --order >= 0);
	if (!sctp_bhash)
		panic("%s: Failed to allocate SCTP bind hash table\n", __FUNCTION__);
	bzero(sctp_bhash, sctp_bhash_size * sizeof(struct sctp_bhash_bucket));
	bzero(sctp_lhash, sctp_lhash_size * sizeof(struct sctp_hash_bucket));
	bzero(sctp_phash, sctp_phash_size * sizeof(struct sctp_hash_bucket));
	bzero(sctp_cache, sctp_cache_size * sizeof(struct sctp_hash_bucket));
	bzero(sctp_vhash, sctp_vhash_size * sizeof(struct sctp_hash_bucket));
	bzero(sctp_thash, sctp_thash_size * sizeof(struct sctp_hash_bucket));
	for (i = 0; i < sctp_bhash_size; i++)
		rwlock_init(&sctp_bhash[i].lock);
	for (i = 0; i < sctp_lhash_size; i++)
		rwlock_init(&sctp_lhash[i].lock);
	for (i = 0; i < sctp_phash_size; i++)
		rwlock_init(&sctp_phash[i].lock);
	for (i = 0; i < sctp_cache_size; i++)
		rwlock_init(&sctp_cache[i].lock);
	for (i = 0; i < sctp_vhash_size; i++)
		rwlock_init(&sctp_vhash[i].lock);
	for (i = 0; i < sctp_thash_size; i++)
		rwlock_init(&sctp_thash[i].lock);
#if 0
	printd(("INFO: bind hash table configured size = %d\n", sctp_bhash_size));
	printd(("INFO: list hash table configured size = %d\n", sctp_lhash_size));
	printd(("INFO: ptag hash table configured size = %d\n", sctp_phash_size));
	printd(("INFO: vtag cach table configured size = %d\n", sctp_cache_size));
	printd(("INFO: vtag hash table configured size = %d\n", sctp_vhash_size));
	printd(("INFO: tcb  hash table configured size = %d\n", sctp_thash_size));
#else
	cmn_err(CE_NOTE, "INFO: bind hash table configured size = %d", sctp_bhash_size);
	cmn_err(CE_NOTE, "INFO: list hash table configured size = %d", sctp_lhash_size);
	cmn_err(CE_NOTE, "INFO: ptag hash table configured size = %d", sctp_phash_size);
	cmn_err(CE_NOTE, "INFO: vtag cach table configured size = %d", sctp_cache_size);
	cmn_err(CE_NOTE, "INFO: vtag hash table configured size = %d", sctp_vhash_size);
	cmn_err(CE_NOTE, "INFO: tcb  hash table configured size = %d", sctp_thash_size);
#endif
}

#if defined SCTP_CONFIG_MODULE
STATIC void
sctp_term_hashes(void)
{
	/* free hashes */
	if (sctp_vhash) {
		free_pages((unsigned long) sctp_vhash, sctp_vhash_order);
		sctp_vhash = NULL;
		sctp_vhash_order = 0;
	}
	if (sctp_bhash) {
		free_pages((unsigned long) sctp_bhash, sctp_bhash_order);
		sctp_bhash = NULL;
		sctp_bhash_order = 0;
	}
}
#endif				/* defined SCTP_CONFIG_MODULE */

/*
 *  Hashing Functions
 *  -------------------------------------------------------------------------
 */
STATIC INLINE uint
sctp_bhashfn(uint16_t num)
{
	return ((sctp_bhash_size - 1) & num);
}
STATIC INLINE uint
sctp_lhashfn(uint16_t port)
{
	return ((sctp_lhash_size - 1) & port);
}
STATIC INLINE uint
sctp_phashfn(uint32_t ptag)
{
	return ((sctp_phash_size - 1) & ptag);
}
STATIC INLINE uint
sctp_cachefn(uint32_t vtag)
{
	return ((sctp_cache_size - 1) & vtag);
}
STATIC INLINE uint
sctp_vhashfn(uint32_t vtag)
{
	return ((sctp_vhash_size - 1) & vtag);
}
STATIC INLINE uint
sctp_thashfn(uint16_t sport, uint16_t dport)
{
	return ((sctp_thash_size - 1) & (sport + (dport << 4)));
}
STATIC INLINE uint
sctp_sp_bhashfn(sctp_t * sp)
{
	return sctp_bhashfn(sp->num);
}
STATIC INLINE uint
sctp_sp_lhashfn(sctp_t * sp)
{
	return sctp_lhashfn(sp->sport);
}
STATIC INLINE uint
sctp_sp_phashfn(sctp_t * sp)
{
	return sctp_phashfn(sp->p_tag);
}
STATIC INLINE uint
sctp_sp_cachefn(sctp_t * sp)
{
	return (sp->hashent = sctp_cachefn(sp->v_tag));
}
STATIC INLINE uint
sctp_sp_vhashfn(sctp_t * sp)
{
	return sctp_vhashfn(sp->v_tag);
}
STATIC INLINE uint
sctp_sp_thashfn(sctp_t * sp)
{
	return sctp_thashfn(sp->sport, sp->dport);
}
STATIC void
__sctp_lhash_insert(sctp_t * sp)
{
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_sp_lhashfn(sp)];

	sctplogno(sp, "adding to listen hashes");
	write_lock(&hp->lock);
	if (!sp->lprev) {
		if ((sp->lnext = hp->list))
			sp->lnext->lprev = &sp->lnext;
		sp->lprev = &hp->list;
		hp->list = sp;
	} else {
		sctplogerr(sp, "%s() already in hashes", __FUNCTION__);
	}
	write_unlock(&hp->lock);
}
STATIC void
__sctp_phash_insert(sctp_t * sp)
{
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_sp_phashfn(sp)];

	sctplogno(sp, "adding to peer tag hashes");
	write_lock(&hp->lock);
	if (!sp->pprev) {
		if ((sp->pnext = hp->list))
			sp->pnext->pprev = &sp->pnext;
		sp->pprev = &hp->list;
		hp->list = sp;
	} else {
		sctplogerr(sp, "%s() already in hashes", __FUNCTION__);
	}
	write_unlock(&hp->lock);
}
STATIC void
__sctp_vhash_insert(sctp_t * sp)
{
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_sp_vhashfn(sp)];
	struct sctp_hash_bucket *cp = &sctp_cache[sctp_sp_cachefn(sp)];

	sctplogno(sp, "adding to verification tag hashes");
	write_lock(&hp->lock);
	if (!sp->vprev) {
		if ((sp->vnext = hp->list))
			sp->vnext->vprev = &sp->vnext;
		sp->vprev = &hp->list;
		hp->list = sp;
		cp->list = sp;
	} else {
		sctplogerr(sp, "%s() already in hashes", __FUNCTION__);
	}
	write_unlock(&hp->lock);
}
STATIC void
___sctp_thash_insert(sctp_t * sp, struct sctp_hash_bucket *hp)
{
	sctplogno(sp, "adding to TCB hashes");
	if (!sp->tprev) {
		if ((sp->tnext = hp->list))
			sp->tnext->tprev = &sp->tnext;
		sp->tprev = &hp->list;
		hp->list = sp;
	} else {
		sctplogerr(sp, "%s() already in hashes", __FUNCTION__);
	}
}

STATIC void
__sctp_lhash_unhash(sctp_t * sp)
{
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_sp_lhashfn(sp)];

	sctplogno(sp, "removing from listen hashes");
	write_lock(&hp->lock);
	if (sp->lprev) {
		if ((*(sp->lprev) = sp->lnext))
			sp->lnext->lprev = sp->lprev;
		sp->lnext = NULL;
		sp->lprev = NULL;
	} else {
		sctplogerr(sp, "%s() not in hashes", __FUNCTION__);
	}
	write_unlock(&hp->lock);
}
STATIC void
__sctp_phash_unhash(sctp_t * sp)
{
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_sp_phashfn(sp)];

	sctplogno(sp, "removing from peer tag hashes");
	write_lock(&hp->lock);
	if (sp->pprev) {
		if ((*(sp->pprev) = sp->pnext))
			sp->pnext->pprev = sp->pprev;
		sp->pnext = NULL;
		sp->pprev = NULL;
	} else {
		sctplogerr(sp, "%s() not in hashes", __FUNCTION__);
	}
	write_unlock(&hp->lock);
}
STATIC void
__sctp_vhash_unhash(sctp_t * sp)
{
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_sp_vhashfn(sp)];

	sctplogno(sp, "removing from verification tag hashes");
	write_lock(&hp->lock);
	if (sp->vprev) {
		if ((*(sp->vprev) = sp->vnext))
			sp->vnext->vprev = sp->vprev;
		sp->vnext = NULL;
		sp->vprev = NULL;
		if (sctp_cache[sp->hashent].list == sp)
			sctp_cache[sp->hashent].list = NULL;
		sp->hashent = 0;
	} else {
		sctplogerr(sp, "%s() not in hashes", __FUNCTION__);
	}
	write_unlock(&hp->lock);
}
STATIC void
__sctp_thash_unhash(sctp_t * sp)
{
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_sp_thashfn(sp)];

	sctplogno(sp, "removing from TCB hashes");
	write_lock(&hp->lock);
	if (sp->tprev) {
		if ((*(sp->tprev) = sp->tnext))
			sp->tnext->tprev = sp->tprev;
		sp->tnext = NULL;
		sp->tprev = NULL;
	} else {
		sctplogerr(sp, "%s() not in hashes", __FUNCTION__);
	}
	write_unlock(&hp->lock);
}
STATIC void
__sctp_bindb_get(unsigned short snum)
{
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];

	(void) hp;
	write_lock(&hp->lock);
}
STATIC void
__sctp_bindb_put(unsigned short snum)
{
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];

	(void) hp;
	write_unlock(&hp->lock);
}
STATIC struct sctp_bind_bucket *
sctp_bindb_get(unsigned short snum, unsigned long *flagsp)
{
	struct sctp_bind_bucket *sb;
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	unsigned long flags;

	write_lock_irqsave(&hp->lock, flags);
	printd(("INFO: Getting bind bucket for port = %d\n", snum));
	for (sb = hp->list; sb && sb->port != snum; sb = sb->next) ;
	*flagsp = flags;
	return (sb);
}
STATIC void
sctp_bindb_put(unsigned short snum, unsigned long *flagsp)
{
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	unsigned long flags = *flagsp;

	(void) hp;
	printd(("INFO: Putting bind bucket for port = %d\n", snum));
	write_unlock_irqrestore(&hp->lock, flags);
}
STATIC INLINE struct sctp_bind_bucket *
__sctp_bindb_create(unsigned short snum)
{
	struct sctp_bind_bucket *sb;

	if ((sb = kmem_cache_alloc(sctp_bind_cachep, GFP_ATOMIC))) {
		struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];

		printd(("INFO: Allocating bind bucket for port = %d\n", snum));
		bzero(sb, sizeof(*sb));
		sb->port = snum;
		sb->fastreuse = 1;
		if ((sb->next = hp->list))
			sb->next->prev = &sb->next;
		sb->prev = &hp->list;
		hp->list = sb;
	}
	return (sb);
}
STATIC void
___sctp_bhash_insert(sctp_t * sp, struct sctp_bind_bucket *sb)
{
	if (!sp->bprev) {
		sctplogno(sp, "adding to bind bucket at port %d", sb->port);
		if ((sp->bnext = sb->owners))
			sp->bnext->bprev = &sp->bnext;
		sp->bprev = &sb->owners;
		sb->owners = sp;
		sp->bindb = sb;
		sp->num = sb->port;
		sp->sport = htons(sb->port);
	} else if (sp->state == SCTP_LISTEN) {
		sctplogno(sp, "re-adding listener to bind bucket at port %d", sb->port);
	} else {
		sctplogerr(sp, "%s() already in hashes", __FUNCTION__);
	}
	if (!sp->reuse || sp->state == SCTP_LISTEN)
		sb->fastreuse = 0;
}

STATIC void
__sctp_inherit_port(struct sctp *sp, struct sctp *lsp)
{
	struct sctp_bind_bucket *sb;

	sp->num = lsp->num;
	sp->sport = lsp->sport;
	__sctp_bindb_get(sp->num);
	if ((sb = lsp->bindb))
		___sctp_bhash_insert(sp, sb);
	else {
		sctplogerr(sp, "%s() no port to inherit", __FUNCTION__);
	}
	__sctp_bindb_put(sp->num);
}

STATIC void
__sctp_bhash_unhash(struct sctp *sp)
{
	struct sctp_bhash_bucket *bp = &sctp_bhash[sctp_sp_bhashfn(sp)];

	write_lock(&bp->lock);
	if (sp->bprev) {
		sctplogno(sp,
			  "removing from bind bucket at port %d, num %d",
			  sp->bindb ? sp->bindb->port : -1U, sp->num);
		if ((*(sp->bprev) = sp->bnext))
			sp->bnext->bprev = sp->bprev;
		sp->bnext = NULL;
		sp->bprev = NULL;
		if (sp->bindb) {
			if (sp->bindb->owners == NULL) {
				struct sctp_bind_bucket *sb = sp->bindb;

				sctplogno(sp, "deallocating bind bucket for port %d", sb->port);
				if (sb->prev) {
					if ((*(sb->prev) = sb->next))
						sb->next->prev = sb->prev;
					sb->next = NULL;
					sb->prev = NULL;
					bzero(sb, sizeof(*sb));	/* debug */
					_ctrace(kmem_cache_free(sctp_bind_cachep, sb));
				} else {
					sctplogerr(sp, "%s() not in hashes", __FUNCTION__);
				}
			}
			sp->bindb = NULL;
		} else {
			sctplogerr(sp, "%s() no bind bucket", __FUNCTION__);
		}
	} else {
		sctplogerr(sp, "%s() not bound", __FUNCTION__);
	}
	sp->num = 0;
	sp->sport = 0;
	write_unlock(&bp->lock);
}

/*
 *  Add to listening Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This also checks for conflicts.  This uses the rule that no two sockets can be listening on the
 *  same primary transport address (IP/port).  It is a question whether whether a socket which is
 *  bound to multiple addresses using bind(2) and then calls listen(2) should be viewed as listening
 *  on the primary address or should be viewed as listening on all addresses.  The xinetd(8) will
 *  probably want to listen on INADDR_ANY for specific port numbers.
 */

/*
 *  Get Local Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Get all the available local addresses and build the source address list from those that are
 *  available.
 */
STATIC int
__sctp_get_addrs(sctp_t * sp, uint32_t daddr)
{
	int allocated = 0;
	struct net_device *dev;

	read_lock(&dev_base_lock);
#if ! ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
	read_lock(&inetdev_lock);
#endif
	for (dev = dev_base; dev; dev = dev->next) {
		struct in_device *in_dev;
		struct in_ifaddr *ifa;

#if ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
		rcu_read_lock();
#endif
#ifdef HAVE_KFUNC___IN_DEV_GET_RCU
		if (!(in_dev = __in_dev_get_rcu(dev))) {
#else
		if (!(in_dev = __in_dev_get(dev))) {
#endif
#if ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
			rcu_read_unlock();
#endif
			continue;
		}
#if !( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
		read_lock(&in_dev->lock);
#endif
		/* get primary or secondary addresses for each interface */
		for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
			int err = 0;

			if (LOOPBACK(ifa->ifa_local) != LOOPBACK(daddr))
				continue;
#if 0
			if (ifa->ifa_scope != RT_SCOPE_HOST)
				continue;
#endif
			if (sctp_saddr_include(sp, ifa->ifa_local, &err))
				allocated++;
		}
#if ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
		rcu_read_unlock();
#else
		read_unlock(&in_dev->lock);
#endif
	}
#if ! ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
	read_unlock(&inetdev_lock);
#endif
	read_unlock(&dev_base_lock);
	return (allocated == 0);
}
STATIC int
sctp_get_addrs(sctp_t * sp, uint32_t daddr)
{
	unsigned long flags;
	int ret;

	local_irq_save(flags);
	ret = __sctp_get_addrs(sp, daddr);
	local_irq_restore(flags);
	return (ret);
}

/*
 *  Add to Connection Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This also checks for conflicts.  This uses the rule that there cannot be more than one SCTP
 *  association between an given pair of local and remote transport addresses (IP/port).
 */
STATIC int
sctp_conn_hash(struct sctp *sp)
{
	sctp_t *sp2;
	struct sctp_hash_bucket *hp;
	struct sctp_saddr *ss, *ss2, *ss_next;
	struct sctp_daddr *sd, *sd2;

	assert(sp);
	local_bh_disable();
#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (sp->tprev) {
		sctplogerr(sp, "%s() should not be t hashed", __FUNCTION__);
		__sctp_thash_unhash(sp);
	}
	if (sp->vprev) {
		sctplogerr(sp, "%s() should not be v hashed", __FUNCTION__);
		__sctp_vhash_unhash(sp);
	}
	if (sp->pprev) {
		sctplogerr(sp, "%s() should not be p hashed", __FUNCTION__);
		__sctp_phash_unhash(sp);
	}
#endif
	/* Check for conflicts: we search the TCB hash list for other streams with the same port
	   pair and with the same pairing of src and dst addresses.  We do this with write locks on 
	   for the hash list so that we can add our stream if we succeed.  This is the TCB hash, so
	   even if this is a long search, we are only blocking other connection hash calls and TCB
	   lookups (for ootb packets) for this hash bucket. */
	hp = &sctp_thash[sctp_sp_thashfn(sp)];
	write_lock(&hp->lock);
	for (sp2 = hp->list; sp2; sp2 = sp2->tnext) {
		if (sp2->sport != sp->sport || sp2->dport != sp->dport)
			continue;
		ss_next = sp->saddr;
		while ((ss = ss_next)) {
			ss_next = ss->next;
			for (ss2 = sp2->saddr; ss2; ss2 = ss2->next) {
				if (ss->saddr != ss2->saddr)
					continue;
				for (sd = sp->daddr; sd; sd = sd->next) {
					for (sd2 = sp2->daddr; sd2; sd2 = sd2->next) {
						if (sd->daddr != sd2->daddr)
							continue;
						/* matching daddr */
						/* Now, if we have a matching
						   saddr/sport/daddr/dport n-tuple, then we should
						   check if we are SCTP_BINDADDR_LOCK'd.  If we are 
						   not, then we were bound to INADDR_ANY and (for
						   connect) we are permitted to kick addresses out
						   of the source address list to resolve conflicts. 
						 */
						if (!(sp->userlocks & SCTP_BINDADDR_LOCK)) {
							__sctp_saddr_free(ss);
							goto next_saddr;
						}
						goto eaddrinuse;
					}
				}
			}
		      next_saddr:
			continue;
		}
	}
	/* Of course, if we kick all the source addresses out of the list, then we still have a
	   conflict.  */
	if (!(sp->userlocks & SCTP_BINDADDR_LOCK) && !sp->saddr) {
		rare();
		goto eaddrinuse;
	}
	___sctp_thash_insert(sp, hp);
	write_unlock(&hp->lock);
	__sctp_vhash_insert(sp);
	__sctp_phash_insert(sp);
	local_bh_enable();
	return (0);
      eaddrinuse:
	write_unlock(&hp->lock);
	local_bh_enable();
	return (-EADDRINUSE);	/* conflict */
}

/*
 *  =========================================================================
 *
 *  LOOKUP Functions
 *
 *  =========================================================================
 *
 *  A fast caching hashing lookup function for SCTP.
 *
 *  IMPLEMENTATION NOTES:- All but a few SCTP messages carry our Verification Tag.  If the message
 *  requires our Verification Tag and we cannot lookup the stream on the Verification Tag we treat
 *  the packet similar to an OOTB packet.  The only restriction that this approach imposes is in the
 *  selection of our Verification Tag, which cannot be identical to any other Verification Tag which
 *  we have chosen so far.  We, therefore, check the Verification Tag selected at initialization
 *  against the cache for uniqueness.  This also allows us to acquire the Verification Tag to
 *  minimize collisions on the hash table.  This allows us to simplify the hashing function because
 *  we are guaranteeing equal hash coverage using selection.
 */
/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP LISTEN - LISTEN hash (sport)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Listener stream lookup with wildcards.  This will find any bound listener stream from the
 *  destination address, destination port and device index.  This is only for INIT and COOKIE ECHO.
 */
STATIC struct sctp *
sctp_lookup_listen(uint16_t dport, uint32_t daddr)
{
	sctp_t *sp, *result = NULL;
	int hiscore = 0;
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_lhashfn(dport)];

	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->lnext) {
		int score = 0;

		if (sp->sport) {
			if (sp->sport != dport)
				continue;
			score++;
			if (sp->saddr) {
				struct sctp_saddr *ss;

				if (!(ss = sctp_find_saddr(sp, daddr)))
					continue;
				if (ss->flags & SCTP_SRCEM_ASCONF)
					continue;
				score++;
			}
		} else {
			sctplogerr(sp, "unassigned port number");
		}
		if (score == 2) {
			result = sp;
			break;
		}
		if (score > hiscore) {
			hiscore = score;
			result = sp;
		}
	}
	if (result)
		sctp_hold(result);
	read_unlock(&hp->lock);
	usual(result);
	if (result)
		return (result);
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP BIND - BIND hash (port)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Like the listen lookup, but done on the bind hashes.  This is to find potential conflicts for
 *  INIT.  We are just looking for a non-closed scoket bound to the port with a compatible
 *  destination address considering wildcards.  This is used for exceptional INIT cases when no
 *  listening socket is found.  Port locks must have been taken on the bind (the port must have been
 *  specified by the user and not dynamically assigned otherwise the socket will not accept INITs.
 */
STATIC struct sctp *
sctp_lookup_bind(uint16_t dport, uint32_t daddr)
{
	sctp_t *result = NULL;
	int snum = ntohs(dport);
	struct sctp_bind_bucket *sb;
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];

	read_lock(&hp->lock);
	for (sb = hp->list; sb && sb->port != snum; sb = sb->next) ;
	if (sb) {
		sctp_t *sp;
		int hiscore = 0;

		for (sp = sb->owners; sp; sp = sp->bnext) {
			int score = 0;

			if ((1 << sp->state) & (SCTPF_DISCONNECTED))
				continue;
			if (!(sp->userlocks & SCTP_BINDPORT_LOCK))
				continue;
			if (sp->sport) {
				if (sp->sport != dport)
					continue;
				score++;
				if (sp->saddr) {
					struct sctp_saddr *ss;

					if (!(ss = sctp_find_saddr(sp, daddr)))
						continue;
					if (ss->flags & SCTP_SRCEM_ASCONF)
						continue;
					score++;
				}
			} else {
				sctplogerr(sp, "unassigned port number");
			}
			if (score > 1) {
				result = sp;
				break;
			}
			if (score > hiscore) {
				hiscore = score;
				result = sp;
			}
		}
	}
	if (result)
		sctp_hold(result);
	read_unlock(&hp->lock);
	usual(result);
	if (result)
		return (result);
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP - TCB hash (port pair)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We do the hard match here, because we don't have valid v_tags or p_tags.  We don't have the
 *  information anyways.  We are just looking for an established stream which can accept the packet
 *  based on port numbers and source and destination addresses.  This is used for INIT, OOTB
 *  determination as well as ICMP lookups for failed sent INIT messages.
 *
 */
#define sctp_match_tcb(__sp, __saddr, __daddr, __sport, __dport) \
	( ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__saddr))) && \
	  (sctp_find_daddr((__sp),(__daddr))) )

STATIC struct sctp *
sctp_lookup_tcb(uint16_t sport, uint16_t dport, uint32_t saddr, uint32_t daddr)
{
	sctp_t *sp;
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_thashfn(sport, dport)];

	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->tnext)
		if (sctp_match_tcb(sp, saddr, daddr, sport, dport))
			break;
	if (sp)
		sctp_hold(sp);
	read_unlock(&hp->lock);
	if (sp)
		return (sp);
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP PTAG - Peer hash (peer tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We can either match fast and loose or slow and sure.  We have a peer tag which is validation
 *  enough without walking the address lists most of the time.  The INIT and COOKIE ECHO stuff needs
 *  it for checking peer tags and peer tie tags.  ICMP needs this for looking up all packets which
 *  were returned that we sent out with the peer's tag (excepts INIT which has no tag, we use TCB
 *  ICMP lookup for that).  ICMP lookups match with reversed source and destination addresses.
 *
 *  ADD-IP must ignore addresses at times (when retransmitting an ASCONF which successfully deleted
 *  an IP address but for which the ASCONF ACK was lost).  Therefore, we always use just
 *  verification tag and port for ADD-IP regardless of the slow verification setting.
 */
#if !defined(SCTP_CONFIG_SLOW_VERIFICATION) || defined(SCTP_CONFIG_ADD_IP)
#define	sctp_match_ptag(__sp, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == (__sp)->p_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) )
#else				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */
#define	sctp_match_ptag(__sp, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == (__sp)->p_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__saddr))) && \
	  (sctp_find_daddr((__sp),(__daddr))) )
#endif				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */

STATIC struct sctp *
sctp_lookup_ptag(uint32_t p_tag, uint16_t sport, uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	sctp_t *sp;
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_phashfn(p_tag)];

	(void) saddr;
	(void) daddr;
	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->pnext)
		if (sctp_match_ptag(sp, saddr, daddr, p_tag, sport, dport))
			break;
	if (sp)
		sctp_hold(sp);
	read_unlock(&hp->lock);
	if (sp)
		return (sp);
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP VTAG - Established hash (local tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  This is the main lookup for data transfer on established streams.  This should run as fast and
 *  furious as possible.  We run fast and loose and rely on the verification tag only.  We cache and
 *  hash so a stream of back-to-back packets to the same destination (bursty traffic) will whirl.
 *  Because we MD4 hash verification tags when we generate them, the hash should get good random
 *  distribution with minimum collisions.
 *
 *  ADD-IP must ignore addresses at times (when retransmitting an ASCONF which successfully deleted
 *  an IP address but for which the ASCONF ACK was lost).  Therefore, we always use just
 *  verification tag for ADD-IP regardless of the slow verification setting.
 *
 */
#if !defined(SCTP_CONFIG_SLOW_VERIFICATION) || defined(SCTP_CONFIG_ADD_IP)
#define sctp_match_vtag(__sp, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == (__sp)->v_tag) )
#else				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */
#define sctp_match_vtag(__sp, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == (__sp)->v_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__daddr))) )
#endif				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */

STATIC struct sctp *
sctp_lookup_vtag(uint32_t v_tag, uint16_t sport, uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	sctp_t *sp;
	unsigned int hash = sctp_cachefn(v_tag);
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_vhashfn(v_tag)];

	(void) saddr;
	(void) daddr;
	read_lock(&hp->lock);
	if (!(sp = sctp_cache[hash].list)
	    || !sctp_match_vtag(sp, saddr, daddr, v_tag, sport, dport)) {
		for (sp = hp->list; sp; sp = sp->vnext)
			if (sctp_match_vtag(sp, saddr, daddr, v_tag, sport, dport)) {
				sctp_cache[hash].list = sp;
				break;
			}
	}
	if (sp)
		sctp_hold(sp);
	read_unlock(&hp->lock);
	if (sp)
		return (sp);
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP COOKIE ECHO - Special lookup rules for cookie echo chunks
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp *
sctp_lookup_cookie_echo(struct sctp_cookie *ck, uint32_t v_tag, uint16_t sport, uint16_t dport,
			uint32_t daddr, uint32_t saddr)
{
	struct sctp *sp = NULL;

	/* quick sanity checks on cookie */
	if (ck->v_tag == v_tag && ck->sport == sport && ck->dport == dport) {
		if (		/* RFC 2960 5.2.4 (A) */
			   (ck->l_ttag && ck->p_ttag
			    && (sp = sctp_lookup_vtag(ck->l_ttag, sport, dport, saddr, daddr))
			    && sp == sctp_lookup_ptag(ck->p_ttag, sport, dport, saddr, daddr))
			   /* RFC 2960 5.2.4 (B) or (D) */
			   || ((sp = sctp_lookup_vtag(v_tag, sport, dport, saddr, daddr)))
			   /* RFC 2960 5.2.4 (C) or IG 2.6 replacement */
			   || (!ck->l_ttag && !ck->p_ttag
			       && ((sp = sctp_lookup_ptag(ck->p_tag, sport, dport, saddr, daddr))
				   || (sp = sctp_lookup_tcb(sport, dport, saddr, ck->daddr))))
			   /* RFC 2960 5.1 */
			   || ((sp = sctp_lookup_listen(sport, daddr))))
			if (sp->state == SCTP_LISTEN || (sp->userlocks & SCTP_BINDPORT_LOCK))
				return (sp);
	} else {
		rare();
		if (ck->v_tag != v_tag)
			printd(("INFO: cookie v_tag = %08X, header v_tag = %08X\n", ck->v_tag,
				v_tag));
		if (ck->sport != sport)
			printd(("INFO: cookie sport = %08X, header sport = %08X\n", ck->sport,
				sport));
		if (ck->dport != dport)
			printd(("INFO: cookie dport = %08X, header dport = %08X\n", ck->dport,
				dport));
	}
	seldom();
	return (NULL);
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP - Established hash (local verification tag with fallbacks)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Stream hash lookup with fast path for data.  This uses verification tag when it is available.
 *  Source address and port are checked after the verification tag matches.  When called for INIT
 *  and COOKIE ECHO messages, the function returns a listening (bind) hash lookup.  For SHUTDOWN
 *  COMPLETE and ABORT messages with the T-bit set, an icmp (peer tag) lookup is performed instead.
 *
 *  Note: SCTP IG 2.18 is unnecessary.
 */
STATIC struct sctp *
sctp_lookup(struct sctphdr *sh, uint32_t daddr, uint32_t saddr)
{
	struct sctp *sp = NULL;
	struct sctpchdr *ch = (struct sctpchdr *) (sh + 1);
	int ctype = ch->type;
	uint32_t v_tag = sh->v_tag;
	uint16_t dport = sh->dest;
	uint16_t sport = sh->srce;

	if (v_tag) {
		/* fast path */
		if (ctype == SCTP_CTYPE_SACK || ctype == SCTP_CTYPE_DATA)
			return sctp_lookup_vtag(v_tag, dport, sport, daddr, saddr);
		switch (ctype) {
			/* See RFC 2960 Section 8.5.1 */
		case SCTP_CTYPE_ABORT:
		case SCTP_CTYPE_SHUTDOWN_COMPLETE:
			/* SCTP IG 2.13 */
			if (ch->flags & 0x1)	/* T bit set */
				return sctp_lookup_ptag(v_tag, dport, sport, daddr, saddr);
		default:
			if ((sp = sctp_lookup_vtag(v_tag, dport, sport, daddr, saddr)))
				return (sp);
			if (ctype == SCTP_CTYPE_ABORT)
				/* check abort for conn ind */
				if ((sp = sctp_lookup_listen(dport, daddr)))
					return (sp);
		case SCTP_CTYPE_INIT:
			break;
		case SCTP_CTYPE_COOKIE_ECHO:{
			struct sctp_cookie *ck = (struct sctp_cookie *)
			    ((struct sctp_cookie_echo *) ch)->cookie;

			return sctp_lookup_cookie_echo(ck, v_tag, dport, sport, daddr, saddr);
		}
		}
	} else if (ctype == SCTP_CTYPE_INIT)
		if ((sp = sctp_lookup_listen(dport, daddr))
		    || (sp = sctp_lookup_bind(dport, daddr)))
			return (sp);
	seldom();
	return (NULL);
}

/*
 *  =========================================================================
 *
 *  COOKIE HANDLING and SECURITY
 *
 *  =========================================================================
 */
extern uint32_t secure_tcp_sequence_number(uint32_t, uint32_t, uint16_t, uint16_t);
STATIC uint32_t
sctp_get_vtag(uint32_t daddr, uint32_t saddr, uint16_t dport, uint16_t sport)
{
	uint32_t ret;

	/* protected against zeros and repeats: our vtags are always unique */
	for (;;) {
		struct sctp *sp;

		if (!(ret = secure_tcp_sequence_number(daddr, saddr, dport, sport)))
			continue;
		if ((sp = sctp_lookup_vtag(ret, sport, dport, saddr, daddr))) {
			sctp_put(sp);
			continue;
		}
		break;
	}
	return (ret);
}

#define HMAC_SIZE (160/8)
#define raw_cookie_size(__ck) \
		( sizeof(struct sctp_cookie) \
		+ ((__ck)->opt_len) \
		+ ((__ck)->danum * sizeof(uint32_t)) \
		+ ((__ck)->sanum * sizeof(uint32_t)) )

#ifdef SCTP_CONFIG_HMAC_SHA1
#include "sctp_hmac_sha1.h"
#endif				/* SCTP_CONFIG_HMAC_SHA1 */

#ifdef SCTP_CONFIG_HMAC_MD5
#include "sctp_hmac_md5.h"
#endif				/* SCTP_CONFIG_HMAC_MD5 */

/*
 *  =========================================================================
 *
 *  SCTP Key handling
 *
 *  =========================================================================
 *
 *  This algorithm uses 2^n keys (NUM_KEYS) which are recycled quickly and evenly.  Each key has a
 *  key tag which is placed in the SCTP cookie.  Each key is valid for the cookie lifespan plus a
 *  key life.  The key life should be set to whatever cookie life increment has been permitted.
 *  When a cookie is checked for validity, its MAC is verified using the key with the key tag in the
 *  cookie.  If the key has already been recycled, the tagged key will not fit the lock anymore.
 *  Note that the keys are cycled only as quickly as the requests for signatures come in.  This adds
 *  another degree of variability to the key selection.
 */
struct sctp_key {
	union {
		uint32_t seq[16];
		uint8_t key[64];
	} u;
	unsigned int last;
	unsigned long created;
};

#define NUM_KEYS 4
STATIC struct sctp_key sctp_keys[NUM_KEYS];
STATIC int sctp_current_key = 0;
extern uint32_t secure_tcp_sequence_number(uint32_t, uint32_t, uint16_t, uint16_t);

/*
 *  TODO:  This rekeying is too predicatable.  There are several things bad about it: (1) the key
 *  has a historic component, which is bad; (2) initial keys have zeros in alot of places which
 *  makes it no stronger than if only 32 bit keys were used.
 */
STATIC void
sctp_rekey(int k)
{
	uint32_t *seq;
	int n = (sctp_keys[k].last + 1) & 0xf;

	sctp_keys[k].last = n;
	seq = &sctp_keys[k].u.seq[n];
	*seq = secure_tcp_sequence_number(*(seq + 1), *(seq + 2), *(seq + 3), *(seq + 4));
}
STATIC int
sctp_get_key(sctp_t * sp)
{
	int k = sctp_current_key;
	unsigned long duration = ((sp->ck_life + NUM_KEYS - 1) / NUM_KEYS) + sp->ck_inc;
	unsigned long created = sctp_keys[k].created;

	if (!created) {
		sctp_keys[k].created = jiffies;
		return k;
	}
	if (created + duration < jiffies) {
		k = (k + 1) % NUM_KEYS;
		sctp_rekey(k);
		sctp_current_key = k;
	}
	return k;
}
STATIC void
sctp_hmac(sctp_t * sp, uint8_t *text, int tlen, uint8_t *key, int klen, uint8_t *hmac)
{
	memset(hmac, 0xff, HMAC_SIZE);
	switch (sp->hmac) {
#ifdef SCTP_CONFIG_HMAC_SHA1
	case SCTP_HMAC_SHA_1:
		hmac_sha1(text, tlen, key, klen, hmac);
		break;
#endif				/* SCTP_CONFIG_HMAC_SHA1 */
#ifdef SCTP_CONFIG_HMAC_MD5
	case SCTP_HMAC_MD5:
		hmac_md5(text, tlen, key, klen, hmac);
		break;
#endif				/* SCTP_CONFIG_HMAC_MD5 */
	default:
	case SCTP_HMAC_NONE:
		break;
	}
}
STATIC void
sctp_sign_cookie(sctp_t * sp, struct sctp_cookie *ck)
{
	uint8_t *text = (uint8_t *) ck;
	int tlen = raw_cookie_size(ck);
	int ktag = sctp_get_key(sp);
	uint8_t *key = sctp_keys[ktag].u.key;
	int klen = sizeof(sctp_keys[0].u.key);
	uint8_t *hmacp = ((uint8_t *) ck) + tlen;

	ck->key_tag = ktag;
	sctp_hmac(sp, text, tlen, key, klen, hmacp);
}

/* Note: caller must verify length of cookie */
STATIC int
sctp_verify_cookie(sctp_t * sp, struct sctp_cookie *ck)
{
	uint8_t hmac[HMAC_SIZE];
	uint8_t *text = (uint8_t *) ck;
	int tlen = raw_cookie_size(ck);
	int ktag = (ck->key_tag) % NUM_KEYS;
	uint8_t *key = sctp_keys[ktag].u.key;
	int klen = sizeof(sctp_keys[0].u.key);
	uint8_t *hmacp = ((uint8_t *) ck) + tlen;

	sctp_hmac(sp, text, tlen, key, klen, hmac);
	return memcmp(hmacp, hmac, HMAC_SIZE);
}

/*
 *  CONGESTION/RECEIVE WINDOW AVAILABILITY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Calculate of the remaining space in the current packet, how much is available for use by data
 *  according to the current peer receive window, the current destination congestion window, and the
 *  current outstanding data bytes in flight.
 *
 *  This is called iteratively as each data chunk is tested for bundling into the current message.
 *  The usable length returned does not include the data chunk header.
 */
STATIC INLINE size_t
sctp_avail(sctp_t * sp, struct sctp_daddr *sd)
{
	size_t cwnd, rwnd, swnd, awnd;

	cwnd = sd->cwnd + sd->mtu + 1;
	cwnd = (cwnd > sd->in_flight) ? cwnd - sd->in_flight : 0;
	rwnd = sp->p_rwnd;
	rwnd = (rwnd > sp->in_flight) ? rwnd - sp->in_flight : 0;
	swnd = (cwnd < rwnd) ? cwnd : rwnd;
	awnd = (sp->in_flight) ? swnd : cwnd;
	return awnd;
}

/*
 *  =========================================================================
 *
 *  IP OUTPUT: ROUTING FUNCTIONS
 *
 *  =========================================================================
 *
 *  MULTI-HOMED IP ROUTING FUNCTIONS
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  BREAK TIE
 *  -------------------------------------------------------------------------
 *  Break a tie between two equally rated routes.
 */
STATIC INLINE struct sctp_daddr *
sctp_break_tie(sctp_t * sp, struct sctp_daddr *sd1, struct sctp_daddr *sd2)
{
#if 0
	/* choose usable routes over unusable routes */
	if ((sd1->flags & SCTP_DESTF_UNUSABLE) != (sd2->flags & SCTP_DESTF_UNUSABLE)) {
		if (sd2->flags & SCTP_DESTF_UNUSABLE)
			goto use1;
		goto use2;
	}
	/* choose routes over non-routes */
	if ((sd1->dst_cache == NULL) != (sd2->dst_cache == NULL)) {
		if (sd2->dst_cache == NULL)
			goto use1;
		goto use2;
	}
#endif
	/* choose routes with the least excessive timeouts */
	if ((sd1->retransmits > sd1->max_retrans) && (sd2->retransmits > sd2->max_retrans)) {
		if (sd1->retransmits + sd2->max_retrans != sd2->retransmits + sd1->max_retrans) {
			if (sd1->retransmits + sd2->max_retrans <
			    sd2->retransmits + sd1->max_retrans)
				goto use1;
			goto use2;
		}
	}
	/* choose routes with the least duplicates */
	if (sd1->dups && sd2->dups) {
		if (sd1->dups != sd2->dups) {
			if (sd1->dups < sd2->dups)
				goto use1;
			goto use2;
		}
	}
	/* choose routes with the lowest srtt */
	if (sd1->srtt != sd2->srtt) {
		if (sd1->srtt < sd2->srtt)
			goto use1;
		goto use2;
	}
	/* choose not to slow start */
	if ((sd1->cwnd > sd1->ssthresh) != (sd2->cwnd > sd2->ssthresh)) {
		if (sd1->cwnd > sd1->ssthresh)
			goto use1;
		goto use2;
	}
	/* choose same route */
	if (sd1 == sp->taddr || sd2 == sp->taddr) {
		if (sd1 == sp->taddr)
			goto use1;
		goto use2;
	}
#if 0
	/* choose routes with lowest congestion window */
	if (sd1->cwnd != sd2->cwnd) {
		if (sd1->cwnd < sd2->cwnd)
			goto use1;
		goto use2;
	}
#endif
	/* choose routes with best congestion window */
	if (sd1->cwnd + sd2->in_flight != sd2->cwnd + sd1->in_flight) {
		if (sd1->cwnd + sd2->in_flight > sd2->cwnd + sd1->in_flight)
			goto use1;
		goto use2;
	}
	/* choose routes with the lowest rto */
	if (sd1->rto != sd2->rto) {
		if (sd1->rto < sd2->rto)
			goto use1;
		goto use2;
	}
	/* can't break tie, just go with sd1 */
      use1:
	return (sd1);
      use2:
	return (sd2);
}

/*
 *  RATE ROUTE
 *  -------------------------------------------------------------------------
 *  Rate the provided route according to a weighting function.
 */
STATIC INLINE int
sctp_rate_route(sctp_t * sp, struct sctp_daddr *sd)
{
	int value = 0;

#if 0
#ifdef SCTP_CONFIG_ADD_IP
	if (sd->flags & SCTP_DESTF_UNUSABLE)
		goto done;
	value += 10;
#endif
#endif
	if (sd->dst_cache == NULL)
		goto done;
#if 0
	if (sd->dst_cache->obsolete) {
		dst_release(xchg(&sd->dst_cache, NULL));
		goto done;
	}
#endif
	value += 10;
	if (sd->retransmits > sd->max_retrans)
		goto done;
	value++;
	if (sctp_avail(sp, sd) == 0)
		goto done;
	value++;
	if (!sd->retransmits)
		value += 1;
	if (!sd->dups)
		value += 1;
	if (sd->cwnd >= sd->in_flight + sd->dmps)
		value += 2;
	if (sd->cwnd <= sd->ssthresh)
		value += 1;
	if (sd == sp->taddr)
		value -= 1;
      done:
	return (value);
}

/*
 *  CHOSE BEST
 *  -------------------------------------------------------------------------
 *  This function is called by sctp_update_routes to choose the best primary address.  We alway
 *  return a usable address if possible.  This is called only when a route fails or a destination
 *  goes inactive due to too many timeouts, so destinations that are timing out are not so large a
 *  factor.
 */
STATIC INLINE struct sctp_daddr *
sctp_choose_best(sctp_t * sp)
{
	struct sctp_daddr *best = NULL, *sd;
	int best_value = -1, value;

	for (sd = sp->daddr; sd; sd = sd->next) {
		if (best_value != (value = sctp_rate_route(sp, sd)) || !best) {
			if (best_value < value) {
				best = sd;
				best_value = value;
			}
		} else
			best = sctp_break_tie(sp, best, sd);
	}
	return (best);
}

/*
 *  UPDATE ROUTES
 *  -------------------------------------------------------------------------
 *  This function is called whenever there is a routing problem or whenever a timeout occurs on a
 *  destination.  It's purpose is to perform a complete re-analysis of the available destination
 *  addresses and IP routing and establish new routes as required and set the (primary) and
 *  (secondary) destination addresses.
 */
#if (defined(SCTP_CONFIG_DEBUG) || defined(SCTP_CONFIG_TEST)) && defined(SCTP_CONFIG_ERROR_GENERATOR)
#define SCTP_CONFIG_ERROR_GENERATOR_LEVEL  8
#define SCTP_CONFIG_ERROR_GENERATOR_LIMIT 13
#define SCTP_CONFIG_BREAK_GENERATOR_LEVEL 50
#define SCTP_CONFIG_BREAK_GENERATOR_LIMIT 200
#endif				/* defined(SCTP_CONFIG_DEBUG) &&
				   defined(SCTP_CONFIG_ERROR_GENERATOR) */
STATIC INLINE int
my_dst_check(struct dst_entry **dstp)
{
	struct dst_entry *dst;

	if (dstp) {
		dst = *dstp;
		if (dst->ops && dst->ops->check)
			return (!(*dstp = dst->ops->check(dst, 0)));
		dst_release(xchg(dstp, NULL));
		return (0);
	} else {
		swerr();
		return (0);
	}
}

STATIC void
sp_dst_reset(struct sctp *sp)
{
	struct sctp_daddr *sd;

	for (sd = sp->daddr; sd; sd = sd->next)
		if (sd->dst_cache)
			dst_release(xchg(&sd->dst_cache, NULL));
}

#ifdef HAVE_KFUNC_DST_MTU
/* Why do stupid people rename things like this? */
#undef dst_pmtu
#define dst_pmtu dst_mtu
#endif

#undef RT_CONN_FLAGS
#define RT_CONN_FLAGS(__sp) (RT_TOS(sp->inet.tos) | sp->localroute)

STATIC int
sctp_update_routes(struct sctp *sp, int force_reselect)
{
	int err = -EHOSTUNREACH;
	int mtu_changed = 0;
	int viable_route = 0;
	int route_changed = 0;
	struct sctp_daddr *sd;
	struct sctp_daddr *taddr;
	int old_pmtu;

	assert(sp);
	__ensure(sp->daddr, return (-EFAULT));
	old_pmtu = xchg(&sp->pmtu, INT_MAX);	/* big enough? */
	sp->route_caps = -1L;
	taddr = sp->taddr;
	/* First we check our cached routes..  */
	for (sd = sp->daddr; sd; sd = sd->next) {
		struct rtable *rt = (struct rtable *) sd->dst_cache;

#ifdef SCTP_CONFIG_ADD_IP
		if (sd->flags & SCTP_DESTF_UNUSABLE) {
			if (sd->dst_cache)
				dst_release(xchg(&sd->dst_cache, NULL));
			continue;
		}
#endif				/* SCTP_CONFIG_ADD_IP */
		sctplogda(sp, "checking route %d.%d.%d.%d",
			  (sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
			  (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff);
		if (!sd->dst_cache || (sd->dst_cache->obsolete && !my_dst_check(&sd->dst_cache))) {
			rt = NULL;
			sd->saddr = 0;
			route_changed = 1;
			/* try wildcard saddr and dif routing */
#if defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
			err = ip_route_connect(&rt, sd->daddr, 0, RT_CONN_FLAGS(sp), 0);
#else
			err = ip_route_connect(&rt, sd->daddr, 0, RT_CONN_FLAGS(sp), 0,
					       IPPROTO_SCTP, sp->sport, sp->dport, NULL);
#endif
			if (err < 0 || !rt || rt->u.dst.obsolete) {
				rare();
				sctplogerr(sp, "%s() no route", __FUNCTION__);
				if (rt)
					ip_rt_put(rt);
				if (err == 0)
					err = -EHOSTUNREACH;
				continue;
			}
			if (rt->rt_flags & (RTCF_MULTICAST | RTCF_BROADCAST)
			    && !sp->broadcast) {
				rare();
				sctplogerr(sp, "%s() no unicast route", __FUNCTION__);
				ip_rt_put(rt);
				err = -ENETUNREACH;
				continue;
			}
			sd->saddr = rt->rt_src;
			if (!sctp_find_saddr(sp, sd->saddr)) {
				rare();
				sctplogerr(sp, "%s() no route from source", __FUNCTION__);
#ifdef SCTP_CONFIG_ADD_IP
				/* Candidate for ADD-IP but we can't use it yet */
				if (sp->p_caps & SCTP_CAPS_ADD_IP
				    && !(sp->userlocks & SCTP_BINDADDR_LOCK)) {
					int err = 0;
					struct sctp_saddr *ss;

					if ((ss = __sctp_saddr_alloc(sp, rt->rt_src, &err))) {
						ss->flags |= SCTP_SRCEF_ADD_REQUEST;
						sp->sackf |= SCTP_SACKF_ASC;
					}
				}
#endif				/* SCTP_CONFIG_ADD_IP */
				if (!sp->saddr) {
					rare();
					sctplogerr(sp, "%s() no source for route", __FUNCTION__);
					ip_rt_put(rt);
					assure(sp->saddr);
					err = -EADDRNOTAVAIL;
					continue;
				}
				sd->saddr = sp->saddr->saddr;
			}
			sd->dif = rt->rt_iif;
			/* always revert to initial settings when rerouting */
			sd->rto = sp->rto_ini;
			sd->rttvar = 0;
			sd->srtt = 0;
			sd->mtu = dst_pmtu(&rt->u.dst);
			sd->dmps =
			    sd->mtu - sp->ext_header_len - sizeof(struct iphdr) -
			    sizeof(struct sctphdr);
			sd->ssthresh = 2 * dst_pmtu(&rt->u.dst);
			sd->cwnd = dst_pmtu(&rt->u.dst);
			/* SCTP IG Section 2.9 */
			sd->partial_ack = 0;
			sd->dst_cache = &rt->u.dst;
			sd->route_caps = rt->u.dst.dev->features;
			route_changed = 1;
		}
		/* You're welcome diald! */
		if (sysctl_ip_dynaddr && sp->state == SCTP_COOKIE_WAIT && sd == sp->daddr) {
			/* see if route changed on primary as result of INIT that was discarded */
			struct rtable *rt2 = NULL;

#if defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
			if (!ip_route_connect(&rt2, rt->rt_dst, 0, RT_CONN_FLAGS(sp), sd->dif))
#else
			if (!ip_route_connect(&rt2, rt->rt_dst, 0, RT_CONN_FLAGS(sp), sd->dif,
					      IPPROTO_SCTP, sp->sport, sp->dport, NULL))
#endif
			{
				if (rt2->rt_src != rt->rt_src) {
					rare();
					sctplogerr(sp, "%s() wrong source for route", __FUNCTION__);
					rt2 = xchg(&rt, rt2);
					sd->rto = sp->rto_ini;
					sd->rttvar = 0;
					sd->srtt = 0;
					sd->mtu = dst_pmtu(&rt->u.dst);
					sd->dmps =
					    sd->mtu - sp->ext_header_len - sizeof(struct iphdr) -
					    sizeof(struct sctphdr);
					sd->ssthresh = 2 * dst_pmtu(&rt->u.dst);
					sd->cwnd = dst_pmtu(&rt->u.dst);
					/* SCTP IG Section 2.9 */
					sd->partial_ack = 0;
					sd->dst_cache = &rt->u.dst;
					sd->route_caps = rt->u.dst.dev->features;
					route_changed = 1;
				}
				ip_rt_put(rt2);
			}
		}
		viable_route = 1;
		/* always update MTU if we have a viable route */
		sd->route_caps &= rt->u.dst.dev->features;	/* XXX */
		if (sd->mtu != dst_pmtu(&rt->u.dst)) {
			sd->mtu = dst_pmtu(&rt->u.dst);
			sd->dmps =
			    sd->mtu - sp->ext_header_len - sizeof(struct iphdr) -
			    sizeof(struct sctphdr);
			mtu_changed = 1;
			rare();
			sctplogerr(sp, "%s() mtu changed", __FUNCTION__);
		}
		if (sp->pmtu > sd->mtu) {
			sp->pmtu = sd->mtu;
			sp->amps = sd->dmps;
		}
	}
	if (!viable_route) {
		rare();
		sctplogerr(sp, "%s() no viable route", __FUNCTION__);
		/* set defaults */
		sp->taddr = sp->daddr;
		sp->pmtu = ip_rt_min_pmtu;
		sp->amps =
		    sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
		sctplogerr(sp, "no viable route");
		return (err);
	}
	/* if we have made or need changes then we want to reanalyze routes */
	if (force_reselect || route_changed || mtu_changed || sp->pmtu != old_pmtu || !sp->taddr) {
#if (defined(SCTP_CONFIG_DEBUG) || defined(SCTP_CONFIG_TEST)) && defined(SCTP_CONFIG_ERROR_GENERATOR)
		int bad_choice = 0;
#endif				/* defined(SCTP_CONFIG_DEBUG) &&
				   defined(SCTP_CONFIG_ERROR_GENERATOR) */
		sp->taddr = sctp_choose_best(sp);
		usual(sp->taddr);
#if (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) && defined SCTP_CONFIG_ERROR_GENERATOR
		if ((sp->debug & SCTP_OPTION_BREAK)
		    && (sp->taddr == sp->daddr || sp->taddr == sp->daddr->next)
		    && sp->taddr->packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL) {
			ptrace(("Primary sp->taddr %03d.%03d.%03d.%03d chosen poorly on %p\n",
				(sp->taddr->daddr >> 0) & 0xff, (sp->taddr->daddr >> 8) & 0xff,
				(sp->taddr->daddr >> 16) & 0xff, (sp->taddr->daddr >> 24) & 0xff,
				sp));
			bad_choice = 1;
		}
#endif				/* (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) &&
				   defined SCTP_CONFIG_ERROR_GENERATOR */
		if (sp->taddr)
			ptrace(("sp = %p, taddr = %p, Primary route: %d.%d.%d.%d -> %d.%d.%d.%d\n",
				sp, sp->taddr, (sp->taddr->saddr >> 0) & 0xff,
				(sp->taddr->saddr >> 8) & 0xff, (sp->taddr->saddr >> 16) & 0xff,
				(sp->taddr->saddr >> 24) & 0xff, (sp->taddr->daddr >> 0) & 0xff,
				(sp->taddr->daddr >> 8) & 0xff, (sp->taddr->daddr >> 16) & 0xff,
				(sp->taddr->daddr >> 24) & 0xff));
	}
	return (0);
}

/*
 *  ==========================================================================
 *
 *  MESSAGE OUTPUT
 *
 *  ==========================================================================
 */
/*
 *  QUEUE XMIT (Queue for transmission)
 *  -------------------------------------------------------------------------
 *  We need this broken out so that we can use the netfilter hooks.
 */
#ifdef HAVE_KFUNC_DST_OUTPUT
STATIC INLINE int
sctp_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;

#ifdef NETIF_F_TSO
	ip_select_ident_more(iph, &rt->u.dst, NULL, 0);
#else
	ip_select_ident(iph, &rt->u.dst, NULL);
#endif
	ip_send_check(iph);
#ifdef HAVE_KFUNC_IP_DST_OUTPUT
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, ip_dst_output);
#else
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev, dst_output);
#endif
}
#else
STATIC INLINE int
sctp_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;

	if (skb->len > dst_pmtu(&rt->u.dst)) {
		rare();
		return ip_fragment(skb, skb->dst->output);
	} else {
		iph->frag_off |= __constant_htons(IP_DF);
		ip_send_check(iph);
		return skb->dst->output(skb);
	}
}
#endif

/*
 *  XMIT OOTB (Disconnect Send with no Listening Socket or STREAM).
 *  -------------------------------------------------------------------------
 *  This sends disconnected without a socket or stream.  All that is needed is a destination address
 *  and a socket buffer or message block.  The only time that we use this is for responding to OOTB
 *  packets with ABORT or SHUTDOWN COMPLETE.
 */
STATIC void
sctp_xmit_ootb(uint32_t daddr, uint32_t saddr, mblk_t *mp)
{
	struct rtable *rt = NULL;

	ensure(mp, return);
	if (!ip_route_output(&rt, daddr, 0, 0, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = msgdsize(mp);
		size_t tlen = plen + sizeof(struct iphdr);

		usual(hlen);
		usual(plen);
		/* IMPLEMENTATION NOTE:- Sockets ensures that there is sufficient headroom in the
		   passed in sk_buff, fills out the IP header, calculates the checksum and sends
		   the buffer.  For STREAMS, no socket buffer is passed in, just a message block,
		   so a new socket buffer is allocated with the necessary headroom and the message
		   blocks are copied to the socket buffer. */
		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			struct sctphdr *sh;
			unsigned char *data;

			skb_reserve(skb, hlen);
			/* find headers */
			iph = (typeof(iph)) __skb_put(skb, tlen);
			sh = (typeof(sh)) (iph + 1);
			data = (unsigned char *) (sh);
			skb->dst = &rt->u.dst;
			skb->priority = 0;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = sctp_defaults.ip.tos;
			iph->frag_off = 0;
			iph->ttl = sysctl_ip_default_ttl;
			if (iph->ttl < 64)
				iph->ttl = 64;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = 132;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
#ifndef HAVE_KFUNC_DST_OUTPUT
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS
			__ip_select_ident(iph, &rt->u.dst);
#else
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS
			__ip_select_ident(iph, &rt->u.dst, 0);
#else
#error HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS or HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS must be defined.
#endif
#endif
#endif
			/* For sockets the passed in sk_buff represents a single packet.  For
			   STREAMS, the passed in mblk_t pointer is possibly a message buffer chain
			   and we must iterate along the b_cont pointer. */
			for (bp = mp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;

				if (blen > 0) {
					bcopy(bp->b_rptr, data, blen);
					data += blen;
				} else
					rare();
			}
			/* TODO:- For STREAMS it would be better to combine copying the buffer
			   segments above with performing the checking below.  */
			sh->check = 0;
			if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				sh->check = htonl(cksum_generate(sh, plen));
			SCTP_INC_STATS(SctpOutSCTPPacks);
#ifdef HAVE_KFUNC_DST_OUTPUT
			sctp_queue_xmit(skb);
#else
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
#endif
		} else
			rare();
	} else
		rare();
	/* sending OOTB reponses are one time events, if we can't send the message we just drop it, 
	   the peer will probably come back again later */
	freemsg(mp);
	return;
}

/*
 *
 *  XMIT MSG  (Disconnected Send)
 *  -------------------------------------------------------------------------
 *  This sends disconnected sends.  All that is needed is a destination address and a message block.
 *  The only time that we use this is for sending INIT ACKs (because we have not built a complete
 *  socket or stream yet, however we do at least have a Listening socket or stream or possibly an
 *  established socket or stream that is partially qualitfied and can provide some TOS and other IP
 *  option information for the packet.
 *
 *  Note that RFC 2960 request that we use the destination address to which the original INIT was
 *  sent as the source address for the INIT-ACK.  When we receive INIT-ACKs we really don't care
 *  whether they come from any of the addresses to which the association is bound.
 *
 *  This is also now used for sending ABORT or ABORT w/ERROR in response to INIT chunks.
 */
STATIC INLINE void freechunks(mblk_t *mp);
STATIC void
sctp_xmit_msg(uint32_t saddr, uint32_t daddr, mblk_t *mp, struct sctp *sp)
{
	struct inet_opt *ip = &sp->inet;
	struct rtable *rt = NULL;

	ensure(mp, return);
	if (!ip_route_output(&rt, daddr, saddr, RT_TOS(ip->tos) | sp->localroute, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = msgdsize(mp);
		size_t tlen = plen + sizeof(struct iphdr);

		usual(hlen);
		usual(plen);
		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			struct sctphdr *sh;
			unsigned char *data;

			sctplogda(sp,
				  "sending message %d.%d.%d.%d -> %d.%d.%d.%d", (saddr >> 0) & 0xff,
				  (saddr >> 8) & 0xff, (saddr >> 16) & 0xff, (saddr >> 24) & 0xff,
				  (daddr >> 0) & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff,
				  (daddr >> 24) & 0xff);
			skb_reserve(skb, hlen);
			iph = (struct iphdr *) __skb_put(skb, tlen);
			sh = (struct sctphdr *) (iph + 1);
			data = (unsigned char *) (sh);
			skb->dst = &rt->u.dst;
			skb->priority = sp->priority;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = ip->tos;
			iph->frag_off = 0;
#if 1
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
			iph->ttl = ip->ttl;
#else
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
			iph->ttl = ip->uc_ttl;
#endif
#endif				/* HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
#else
			iph->ttl = ip->uc_ttl;
#endif
			if (iph->ttl < 64)
				iph->ttl = 64;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = sp->protocol;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
#ifndef HAVE_KFUNC_DST_OUTPUT
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS
			__ip_select_ident(iph, &rt->u.dst);
#else
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS
			__ip_select_ident(iph, &rt->u.dst, 0);
#else
#error HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS or HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS must be defined.
#endif
#endif
#endif
			/* For sockets the passed in sk_buff represents a single packet.  For
			   STREAMS, the passed in mblk_t pointer is possibly a message buffer chain
			   and we must iterate along the b_cont pointer. */
			for (bp = mp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;

				if (blen > 0) {
					bcopy(bp->b_rptr, data, blen);
					data += blen;
				} else
					rare();
			}
			/* TODO:- For STREAMS it would be better to combine copying the buffer
			   segments above with performing the checking below.  */
			sh->check = 0;
			if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				sh->check = htonl(cksum(sp, sh, plen));
			SCTP_INC_STATS(SctpOutSCTPPacks);
#ifdef HAVE_KFUNC_DST_OUTPUT
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
#else
			sctp_queue_xmit(skb);
#endif
		} else
			rare();
	} else
		rare();
	/* sending INIT ACKs are one time events, if we can't get the response off, we just drop
	   the INIT ACK: the peer should send us another INIT * in a short while... */
	freemsg(mp);
	return;
}

/*
 *  SEND MSG  (Connected Send)
 *  -------------------------------------------------------------------------
 *  This sends connected sends.  It requires a socket or stream, a desination address structure and
 *  a socket buffer or message block.  This function does not free the socket buffer or message
 *  block.  The caller is responsible for the socket buffer or message block.
 */
#if (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) && defined SCTP_CONFIG_ERROR_GENERATOR
STATIC int break_packets = 0;
STATIC int seed = 152;
STATIC unsigned char
random(void)
{
	return (unsigned char) (seed = seed * 60691 + 1);
}
#endif				/* (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) &&
				   defined SCTP_CONFIG_ERROR_GENERATOR */
STATIC void
sctp_send_msg(struct sctp *sp, struct sctp_daddr *sd, mblk_t *mp)
{
	struct inet_opt *ip = &sp->inet;
	struct sk_buff *skb;
	struct net_device *dev;
	size_t plen, hlen, tlen;

	ensure(sp, return);
	sctplogda(sp, "%s() sending message", __FUNCTION__);
	ensure(sd, return);
	ensure(mp, return);
	ensure(sd->dst_cache, return);
	ensure(!sd->dst_cache->obsolete, return);
	dev = sd->dst_cache->dev;
	plen = SCTP_TCB(mp)->dlen;
	hlen = (dev->hard_header_len + 15) & ~15;
	tlen = sizeof(struct iphdr) + plen;
#if (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) && defined SCTP_CONFIG_ERROR_GENERATOR
	if ((sp->debug & SCTP_OPTION_DBREAK) && sd->daddr == 0x010000ff
	    && ++break_packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL) {
		if (break_packets > SCTP_CONFIG_BREAK_GENERATOR_LIMIT)
			break_packets = 0;
		return;
	}
	if ((sp->debug & SCTP_OPTION_BREAK)
	    && (sd == sp->daddr || sd == sp->daddr->next)
	    && ++sd->packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL) {
		return;
	}
	if ((sp->debug & SCTP_OPTION_DROPPING)
	    && ++sd->packets > SCTP_CONFIG_ERROR_GENERATOR_LEVEL) {
		if (sd->packets > SCTP_CONFIG_ERROR_GENERATOR_LIMIT)
			sd->packets = 0;
		return;
	}
	if ((sp->debug & SCTP_OPTION_RANDOM)
	    && ++sd->packets > 2 * SCTP_CONFIG_ERROR_GENERATOR_LEVEL) {
		if (!(random() & 0x7f)) {
			sctplogerr(sp, "dropping packet");
			return;
		}
	}
#endif				/* (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) &&
				   defined SCTP_CONFIG_ERROR_GENERATOR */
	sctplogda(sp,
		  "preparing message hlen %u, plen %u, tlen %u", (uint) hlen, (uint) plen,
		  (uint) tlen);
	unusual(plen == 0 || plen > 1 << 15);
	/* IMPLEMENTATION NOTE:- We could clone these sk_buffs or dup these mblks and put them into 
	   a fraglist, however, this would require copying the sk_buff header on each data chunk
	   (just to get a new next pointer). Unfortunately, for the most part in SCTP, data chunks
	   are very small: even smaller than the sk_buff header so it is probably not worth cloning 
	   or duping versus copying messages.  */
	/* A workable sendpages might be a better approach to larger data chunks.  */
	if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
		mblk_t *bp;
		struct iphdr *iph;
		struct sctphdr *sh;
		unsigned char *head, *data;
		size_t alen = 0;

		sctplogda(sp,
			  "sending messsage %d.%d.%d.%d -> %d.%d.%d.%d", (sd->saddr >> 0) & 0xff,
			  (sd->saddr >> 8) & 0xff, (sd->saddr >> 16) & 0xff,
			  (sd->saddr >> 24) & 0xff, (sd->daddr >> 0) & 0xff,
			  (sd->daddr >> 8) & 0xff, (sd->daddr >> 16) & 0xff,
			  (sd->daddr >> 24) & 0xff);
		skb_reserve(skb, hlen);
		iph = (struct iphdr *) __skb_put(skb, tlen);	/* XXX */
		sh = (struct sctphdr *) (iph + 1);
		head = data = (unsigned char *) sh;
		skb->dst = dst_clone(sd->dst_cache);
		skb->priority = sp->priority;
		iph->version = 4;
		iph->ihl = 5;
		iph->tos = ip->tos;
		iph->frag_off = 0;
#if 1
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
		iph->ttl = ip->ttl;
#else
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
		iph->ttl = ip->uc_ttl;
#endif
#endif				/* HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
#else
		iph->ttl = ip->uc_ttl;
#endif
		if (iph->ttl < 64)
			iph->ttl = 64;
		iph->daddr = sd->daddr;	/* XXX */
		iph->saddr = sd->saddr;
		iph->protocol = sp->protocol;
		iph->tot_len = htons(tlen);
		skb->nh.iph = iph;
#ifndef HAVE_KFUNC_DST_OUTPUT
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS
		__ip_select_ident(iph, sd->dst_cache);
#else
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS
		__ip_select_ident(iph, sd->dst_cache, 0);
#else
#error HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS or HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS must be defined.
#endif
#endif
#endif
		/* For sockets, socket buffers representing chunks are chained together by control
		   block pointers, however, each socket buffer in the chain is a complete chunk.
		   For message blocks, blocks representing chunks are chained together with the
		   b_next pointer, but each chunk can consist of one or more segments chained
		   together by the b_cont pointer. */
		for (bp = mp; bp; bp = bp->b_next) {
			mblk_t *db;
			size_t pad, blen, clen = 0;
			sctp_tcb_t *cb = SCTP_TCB(bp);

			/* for destination RTT calculation */
			cb->daddr = sd;
			cb->when = jiffies;
			cb->flags |= SCTPCB_FLAG_SENT;	/* should have been set by caller */
			cb->trans++;
			/* For sockets the passed in sk_buff represents a single packet.  For
			   STREAMS, the passed in mblk_t pointer is possibly a message buffer chain
			   and we must iterate along the b_cont pointer. */
			for (db = bp; db; db = db->b_cont) {
				blen = db->b_wptr - db->b_rptr;
				normal(db->b_datap->db_type == M_DATA);
				if (db->b_datap->db_type == M_DATA) {
					normal(blen > 0);
					if (blen > 0) {
						ensure(head + plen >= data + blen, kfree_skb(skb);
						       return);
						bcopy(db->b_rptr, data, blen);
						data += blen;
						clen += blen;
					}
				}
			}
			/* pad each chunk if not padded already */
			pad = PADC(clen) - clen;
			ensure(head + plen >= data + pad, kfree_skb(skb); return);
			bzero(data, pad);
			data += pad;
			alen += clen + pad;
		}
		if (alen != plen) {
			usual(alen == plen);
			ptrace(("alen = %u, plen = %u discarding\n", (uint) alen, (uint) plen));
			kfree_skb(skb);
			return;
		}
		/* IMPLEMENTATION NOTE:- Although the functional support is included for partial
		   checksum and copy from user, we don't use it.  The only time that partial
		   checksum is going to save us is when chunks are bundled repeatedly (for
		   retransmission).  But then we are network bound rather than processor bound, so
		   there is probably little use there either.  It might be better, however, to
		   combine checksumming below with copying above for efficiency. */
		sh->check = 0;
		if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
			sh->check = htonl(cksum(sp, sh, plen));
		SCTP_INC_STATS(SctpOutSCTPPacks);
#ifdef HAVE_KFUNC_DST_OUTPUT
		NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
#else
		sctp_queue_xmit(skb);
#endif
		/* Whenever we transmit something, we expect a reply to our v_tag, so we put
		   ourselves in the 1st level vtag caches expecting a quick reply. */
		if (!((1 << sp->state) & (SCTPF_DISCONNECTED)))
			sctp_cache[sp->hashent].list = sp;
	} else {
		ptrace(("ERROR: couldn't allocate skbuf len %u\n", (uint) (hlen + tlen)));
	}
}

/*
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */

/* is s2<=s1<=s3 ? */
#define between(__s1,__s2,__s3)((uint32_t)(__s3)-(uint32_t)(__s2)>=(uint32_t)(__s1)-(uint32_t)(__s2))
#define before(__s1,__s2) (((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))<0))
#define after(__s1,__s2) (((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))<0))

#define between_s(__s1,__s2,__s3)((uint16_t)(__s3)-(uint16_t)(__s2)>=(uint16_t)(__s1)-(uint16_t)(__s2))
#define before_s(__s1,__s2) (((int16_t)((uint16_t)(__s1)-(uint16_t)(__s2))<0))
#define after_s(__s1,__s2) (((int16_t)((uint16_t)(__s2)-(uint16_t)(__s1))<0))

/*
 *  IMPLEMENTATION NOTE:-  This sctp_bundle_cookie is used to avoid passing 7 arguments on the stack
 *  with each call to a bundle function.
 */
struct sctp_bundle_cookie {
	mblk_t **dpp;			/* place to link buffer */
	size_t mrem;			/* reminaing payload in packet */
	size_t mlen;			/* current length of message */
	size_t swnd;			/* remaining send window */
	size_t pbuf;			/* peer buffer */
};

STATIC mblk_t *sctp_alloc_chk(struct sctp *sp, size_t clen, size_t dlen);

/*
 *  BUNDLING FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  BUNDLE SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_sack(struct sctp *sp,	/* association */
		 struct sctp_daddr *sd,	/* destination */
		 struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	mblk_t *mp;
	struct sctp_sack *m;
	size_t ngaps = sp->ngaps;
	size_t ndups = sp->ndups;
	size_t glen = ngaps * sizeof(uint32_t);
	size_t dlen = ndups * sizeof(uint32_t);
	size_t clen = sizeof(*m) + glen + dlen;
	size_t plen = PADC(clen);

#ifdef SCTP_CONFIG_ECN
	struct sctp_ecne *e;
	size_t elen;
#endif				/* SCTP_CONFIG_ECN */

	assert(sp);
#ifdef SCTP_CONFIG_ECN
	elen = ((sp->sackf & SCTP_SACKF_ECN) ? sizeof(*e) : 0);
	plen += PADC(elen);
#endif				/* SCTP_CONFIG_ECN */
	if ((1 << sp->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen > sd->dmps) {
		size_t too_many_dups;

		rare();		/* trim down sack */
		sctplogtx(sp, "%s() trimming sack", __FUNCTION__);
		too_many_dups = (clen - ckp->mrem + 3) / sizeof(uint32_t);
		ndups = (ndups > too_many_dups) ? ndups - too_many_dups : 0;
		clen = sizeof(*m) + glen + ndups * sizeof(uint32_t);
		if (ckp->mrem < clen) {
			size_t too_many_gaps;

			rare();	/* trim some more */
			sctplogtx(sp, "%s() trimming sack some more", __FUNCTION__);
			too_many_gaps = (clen - ckp->mrem + 3) / sizeof(uint32_t);
			ngaps = (ngaps > too_many_gaps) ? ngaps - too_many_gaps : 0;
			clen = sizeof(*m) + ngaps * sizeof(uint32_t);
		}
	}
	if (plen > ckp->mrem && plen <= sd->dmps)
		goto wait_for_next_packet;
	if (!(mp = sctp_alloc_chk(sp, clen, plen)))
		goto enobufs;
	{
		size_t arwnd = sp->a_rwnd;
		sctp_tcb_t *gap = sp->gaps;
		sctp_tcb_t *dup = sp->dups;

		/* For sockets, socket buffer management maintaines the sp->a_rwnd value at the
		   current available receive window. For STREAMS, sp->a_rwnd is the maximum
		   available receive window and we subtract the queued bytes in each of the receive 
		   buffers. For STREAMS we are also including the duplicate queue which protects us 
		   more from possible attacks on the duplicate queue. */
		size_t count =
		    bufq_size(&sp->oooq) + bufq_size(&sp->dupq) + bufq_size(&sp->rcvq) +
		    bufq_size(&sp->expq);
		/* TODO: double check the a_rwnd calculation for STREAMS as it does not seem to
		   take into account SWS. */
		arwnd = (count < arwnd) ? arwnd - count : 0;
		ptrace(("INFO: arwnd = %u, oooq = %u:%u, dupq = %u:%u, rcvq = %u:%u, expq = %u:%u\n", (uint) arwnd, (uint) bufq_size(&sp->oooq), (uint) bufq_length(&sp->oooq), (uint) bufq_size(&sp->dupq), (uint) bufq_length(&sp->dupq), (uint) bufq_size(&sp->rcvq), (uint) bufq_length(&sp->rcvq), (uint) bufq_size(&sp->expq), (uint) bufq_length(&sp->expq)));
		/* fill out sack message information */
		m = (typeof(m)) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_SACK;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->c_tsn = htonl(sp->r_ack);
		m->a_rwnd = htonl(arwnd);
		m->ngaps = htons(ngaps);
		m->ndups = htons(ndups);
		mp->b_wptr += sizeof(*m);
		for (; gap && ngaps; gap = gap->next, ngaps--) {
			*(uint16_t *) mp->b_wptr = htons(gap->tsn - sp->r_ack);
			mp->b_wptr += sizeof(uint16_t);
			gap = gap->tail;
			*(uint16_t *) mp->b_wptr = htons(gap->tsn - sp->r_ack);
			mp->b_wptr += sizeof(uint16_t);
		}
		for (; dup && ndups; dup = dup->next, ndups--) {
			*(uint32_t *) mp->b_wptr = htonl(dup->tsn);
			mp->b_wptr += sizeof(uint32_t);
		}
		bufq_purge(&sp->dupq);
		sp->ndups = 0;
		sp->dups = NULL;
#ifdef SCTP_CONFIG_ECN
		if (sp->sackf & SCTP_SACKF_ECN) {
			e = (typeof(e)) mp->b_wptr;
			e->ch.type = SCTP_CTYPE_ECNE;
			e->ch.flags = 0;
			e->ch.len = __constant_htons(sizeof(*e));
			e->l_tsn = htonl(sp->l_lsn);
			mp->b_wptr += sizeof(*e);
			SCTP_INC_STATS(SctpOutCtrlChunks);
			sctplogtx(sp, "bundling ECNE chunk");
		}
#endif				/* SCTP_CONFIG_ECN */
		sp->sackf &= ~SCTP_SACKF_ANY;
#ifdef ETSI
		sp->sackcnt = 0;
#endif
		sp_timer_cancel_sack(sp);
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = mp;
		ckp->dpp = &(mp->b_next);
		mp->b_next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		sctplogtx(sp, "bundled SACK chunk");
		return (0);
	}
      enobufs:
	return (0);
      wait_for_next_packet:
	return (1);
      outstate:
	sctplogerr(sp, "%s() in wrong SCTP state %s", __FUNCTION__, sctp_statename(sp->state));
	sp->sackf &= ~SCTP_SACKF_ANY;
#ifdef ETSI
	sp->sackcnt = 0;
#endif
	return (0);
}

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
/*
 *  BUNDLE FORWARD SEQUENCE NUMBER
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_fsn(struct sctp *sp,	/* association */
		struct sctp_daddr *sd,	/* destination */
		struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	mblk_t *mp, *db;
	struct sctp_forward_tsn *m;
	size_t clen = sizeof(*m);
	size_t plen;
	size_t nstrs = 0;
	size_t mstrs = (sd->dmps - sizeof(*m)) / sizeof(uint32_t);
	uint32_t l_fsn = sp->t_ack;
	pl_t pl;

	if ((1 << sp->state) & ~(SCTPF_SENDING))
		goto outstate;
	if (!after(sp->l_fsn, sp->t_ack))
		goto outstate;
	pl = bufq_lock(&sp->rtxq);
	for (db = bufq_head(&sp->rtxq); db; db = db->b_next) {
		sctp_tcb_t *cb = SCTP_TCB(db);

		if (!after(cb->tsn, sp->t_ack))
			continue;
		if (after(cb->tsn, sp->l_fsn))
			break;
		l_fsn = cb->tsn;
		if (cb->st && !(cb->flags & SCTPCB_FLAG_URG)
		    && !(cb->st->n.more & SCTP_STRMF_DROP)) {
			cb->st->n.more |= SCTP_STRMF_DROP;
			nstrs++;
			if (nstrs >= mstrs)
				break;
		}
	}
	bufq_unlock(&sp->rtxq, pl);
	clen += nstrs * sizeof(uint32_t);
	plen = PADC(clen);
	if (plen >= ckp->mrem && plen <= sd->dmps)
		goto wait_for_next_packet;
	if (!(mp = sctp_alloc_chk(sp, clen, plen)))
		goto enobufs;
	{
		/* fill out fsn message headers */
		m = (typeof(m)) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_FORWARD_TSN;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->f_tsn = htonl(l_fsn);
		mp->b_wptr += sizeof(*m);
		/* run backwards to build stream number list */
		pl = bufq_lock(&sp->rtxq);
		for (db = bufq_tail(&sp->rtxq); db; db = db->b_prev) {
			sctp_tcb_t *cb = SCTP_TCB(db);

			if (after(cb->tsn, l_fsn))
				continue;
			if (!after(cb->tsn, sp->t_ack))
				break;
			if (cb->st && !(cb->flags & SCTPCB_FLAG_URG)
			    && (cb->st->n.more & SCTP_STRMF_DROP)) {
				*(uint16_t *) mp->b_wptr = cb->st->sid;
				mp->b_wptr += sizeof(uint16_t);
				*(uint16_t *) mp->b_wptr = cb->ssn;
				mp->b_wptr += sizeof(uint16_t);
				cb->st->n.more &= ~SCTP_STRMF_DROP;
				assure(nstrs);
				nstrs--;
			}
		}
		bufq_unlock(&sp->rtxq, pl);
		assure(!nstrs);
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_timer_cancel_sack(sp);
		sd->flags |= SCTP_DESTF_FORWDTSN;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = mp;
		ckp->dpp = &(mp->b_next);
		mp->b_next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		sctplogtx(sp, "bundled FORWARD-TSN chunk");
		return (0);
	}
      enobufs:
	return (1);
      wait_for_next_packet:
	return (1);
      outstate:
	sctplogerr(sp, "%s() in wrong SCTP state %s", __FUNCTION__, sctp_statename(sp->state));
	sp->sackf &= ~SCTP_SACKF_FSN;
	if (!(sp->sackf & SCTP_SACKF_ANY))
		sp_timer_cancel_sack(sp);
	return (0);
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

#ifdef SCTP_CONFIG_ECN
/*
 *  BUNDLE CONGESTION WINDOW REDUCTION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_cwr(struct sctp *sp,	/* association */
		struct sctp_daddr *sd,	/* destination */
		struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	mblk_t *mp;
	struct sctp_cwr *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);

	if ((1 << sp->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen >= ckp->mrem && plen <= sd->dmps)
		goto wait_for_next_packet;
	if (!(mp = sctp_alloc_chk(sp, clen, plen)))
		goto enobufs;
	{
		/* fill out cwr message headers */
		m = (typeof(m)) mp->b_wptr;
		m->ch.type = SCTP_CTYPE_CWR;
		m->ch.flags = 0;
		m->ch.len = __constant_htons(sizeof(*m));
		m->l_tsn = htonl(sp->p_lsn);
		mp->b_wptr += plen;
		sp->sackf &= ~SCTP_SACKF_CWR;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = mp;
		ckp->dpp = &(mp->b_next);
		mp->b_next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		sctplogtx(sp, "bundled CWR chunk");
		return (0);
	}
      enobufs:
	return (0);
      wait_for_next_packet:
	return (1);
      outstate:
	sctplogerr(sp, "%s() in wrong SCTP state %s", __FUNCTION__, sctp_statename(sp->state));
	sp->sackf &= ~SCTP_SACKF_CWR;
	return (0);
}
#endif				/* SCTP_CONFIG_ECN */

/*
 *  BUNDLE ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_error(struct sctp *sp,	/* association */
		  struct sctp_daddr *sd,	/* destination */
		  struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	mblk_t *mp;
	pl_t pl;

	/* SCTP IG 2.27 - Bundle errors with COOKIE-ECHO or after receiving COOKIE-WAIT */
	if ((1 << sp->state) & (SCTPF_COOKIE_ECHOED | SCTPF_DISCONNECTED))
		goto outstate;
	pl = bufq_lock(&sp->errq);
	while (ckp->mrem && (mp = bufq_head(&sp->errq))) {
		size_t clen = mp->b_wptr - mp->b_rptr;
		size_t plen = PADC(clen);

		if (plen > ckp->mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = __bufq_unlink(&sp->errq, mp);
		ckp->dpp = &(mp->b_next);
		mp->b_next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		sctplogtx(sp, "bundled ERROR chunk");
	}
	bufq_unlock(&sp->errq, pl);
	return (0);
      wait_for_next_packet:
	rare();
	sctplogtx(sp, "%s() cannot bundle error", __FUNCTION__);
	bufq_unlock(&sp->errq, pl);
	return (1);
      outstate:
	sctplogerr(sp, "%s() in wrong SCTP state %s", __FUNCTION__, sctp_statename(sp->state));
	return (0);
}

/*
 *  BUNDLE DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
/*
 *  IMPLEMENTATION NOTES:-  Some information about fragmentation:  Fragmentation and nagling are
 *  intertwined.  When there are no unacked packets in flight (also considering minshall
 *  modifications), packets are dispatched as soon as they are half full.  When there are unacked
 *  packets in flight, we fill packets fully.  However, we are aware of the association path MTU
 *  when filling packets and never fill packets larger than the association path MTU.  This is all
 *  done in the send data or sendmsg routines.  This means that when we go to transmit a queued
 *  packet, if it is too large it is only because the current MTU is less than the association path
 *  MTU at the time that the packet was queued.  So the only time that packets are overfilled is
 *  when PMTU is falling.
 */
STATIC INLINE int
trimhead(mblk_t *mp, int len)
{
	/* NOTE: Trimming the head is more difficult for STREAMS as the message block can be
	   chained together.  For sockets, there is only one socket buffer to trim. */
	for (; len && mp; mp = mp->b_cont) {
		if (mp->b_datap->db_type == M_DATA) {
			int size = mp->b_wptr - mp->b_rptr;

			if (size > len) {
				mp->b_rptr += len;
				len = 0;
			} else if (size >= 0) {
				mp->b_rptr = mp->b_wptr;
				len -= size;
			}
		}
	}
	assure(!len);
	return (len);
}
STATIC INLINE int
trimtail(mblk_t *mp, int len)
{
	/* NOTE: Trimming the tail is more difficult for STREAMS as the message block can be
	   chained together.  For sockets, there is only one socket buffer to trim. */
	for (; len && mp; mp = mp->b_cont) {
		if (mp->b_datap->db_type == M_DATA) {
			int size = mp->b_wptr - mp->b_rptr;

			if (size > len) {
				mp->b_wptr = mp->b_rptr + len;
				len = 0;
			} else if (size >= 0) {
				mp->b_wptr = mp->b_rptr;
				len -= size;
			}
		}
	}
	for (; mp; mp = mp->b_cont)
		if (mp->b_datap->db_type == M_DATA)
			if (mp->b_wptr > mp->b_rptr)
				mp->b_wptr = mp->b_rptr;
	assure(!len);
	return (len);
}

/*
 *  FRAGMENT DATA CHUNKS
 *
 *  Try to fragment a DATA chunk which has not been transmitted yet into two chunks, the first small
 *  enough to fit into the pmtu and the second one containing the remainder of the data in a chunk.
 *  This is called iteratively, so the reminaing data may also be further fragmented according to
 *  the pmtu experienced at the time that it is further fragmented.
 */
#if 0
#if defined(SCTP_CONFIG_DEBUG)||defined(SCTP_CONFIG_TEST)||defined(SCTP_CONFIG_SAFE)
STATIC void
sctp_frag_chunk(bufq_t * bq, mblk_t *mp, size_t mps)
{
	mblk_t *dp;

	assert(bq);
	assert(mp);
	rare();
	/* This should be extremely rare, now that we are fragmenting in sctp_send_data.  This
	   fragmentation only occurs if the path MTU has dropped since we buffered data for
	   transmission.  It is probably not necessary any more. */
	{
		mblk_t *bp;

		/* copy the transmission control block and data header */
		if (!(dp = copyb(mp))) {
			rare();
			return;
		}
		/* skb_copy and copyb do not copy the hidden control block */
		bcopy(mp->b_datap->db_base, dp->b_datap->db_base,
		      mp->b_datap->db_lim - mp->b_datap->db_base);
		/* For socket buffers, when we copy the buffer we copy all of the data associated
		   with the buffer.  For message blocks, we must separately duplicate the message
		   blocks that form the data. */
		if (!(bp = dupmsg(mp->b_cont))) {
			rare();
			freeb(dp);
			return;
		}
		dp->b_cont = bp;
	}
	{
		sctp_tcb_t *cb1;
		sctp_tcb_t *cb2;
		struct sctp_data *m1;
		struct sctp_data *m2;
		size_t dlen;

		cb1 = SCTP_TCB(mp);
		cb2 = SCTP_TCB(dp);
		m1 = (struct sctp_data *) mp->b_rptr;
		m2 = (struct sctp_data *) dp->b_rptr;
		dlen = (mps - sizeof(struct sctp_data));
		cb1->dlen = dlen;
		cb1->flags &= ~SCTPCB_FLAG_LAST_FRAG;
		m1->ch.len = htons(cb1->dlen + sizeof(*m1));
		m1->ch.flags = cb1->flags & 0x7;
		cb2->dlen -= dlen;
		cb2->flags &= ~SCTPCB_FLAG_FIRST_FRAG;
		m2->ch.len = htons(cb2->dlen + sizeof(*m2));
		m2->ch.flags = cb2->flags & 0x7;
#if 1
		{
			int ret;

			ret = trimhead(mp, dlen);	/* trim originai */
			ensure(ret, freemsg(dp); return;);
			ret = trimtail(dp, dlen);	/* trim fragment */
			ensure(ret, freemsg(dp); return;);
		}
#else
		fixme(("Should consider multiple mblks\n"));
		mp->b_cont->b_wptr = mp->b_cont->b_rptr + dlen;	/* trim original */
		dp->b_cont->b_rptr = dp->b_cont->b_rptr + dlen;	/* trim fragment */
#endif
	}
	/* insert the fresh copy after the existing copy in the buffer queue */
	bufq_append(bq, mp, dp);
}
#endif				/* defined(SCTP_CONFIG_DEBUG)||defined(SCTP_CONFIG_TEST)||defined(SCTP_CONFIG_SAFE) 
				 */
#endif

/*
 *  BUNDLE DATA for RETRANSMISSION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

STATIC INLINE int
sctp_bundle_data_retrans(struct sctp *sp,	/* association */
			 struct sctp_daddr *sd,	/* destination */
			 struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	mblk_t *mp;
	pl_t pl;

	pl = bufq_lock(&sp->rtxq);
	for (mp = bufq_head(&sp->rtxq); mp && ckp->mrem && ckp->swnd; mp = mp->b_next) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		if (cb->flags & SCTPCB_FLAG_RETRANS) {
			mblk_t *db;
			size_t dlen = cb->dlen;
			size_t plen = PADC(sizeof(struct sctp_data) + dlen);

			if (plen > ckp->mrem && plen <= sd->dmps)
				goto wait_for_next_packet;
			if (dlen > ckp->swnd && cb->sacks != SCTP_FR_COUNT)
				goto congested;
			if (!(db = sctp_dupmsg(sp, mp)))
				goto enobufs;
			cb->flags &= ~SCTPCB_FLAG_RETRANS;
			ensure(sp->nrtxs > 0, sp->nrtxs = 1);
			sp->nrtxs--;
			cb->when = jiffies;
			cb->daddr = sd;
			ckp->swnd -= dlen;
			sd->in_flight += dlen;
			sp->in_flight += dlen;
			ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
			ckp->mlen += plen;
			*ckp->dpp = db;
			ckp->dpp = &(db->b_next);
			db->b_next = NULL;
		}
	}
	bufq_unlock(&sp->rtxq, pl);
	return (0);
      congested:
	bufq_unlock(&sp->rtxq, pl);
	return (0);
      wait_for_next_packet:
	rare();
	sctplogtx(sp, "%s() cannot bundle retrans", __FUNCTION__);
	bufq_unlock(&sp->rtxq, pl);
	return (1);
      enobufs:
	bufq_unlock(&sp->rtxq, pl);
	return (1);
}

/*
 *  BUNDLE NEW EXPEDITED (OUT OF ORDER) DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_data_urgent(struct sctp *sp,	/* association */
			struct sctp_daddr *sd,	/* destination */
			struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	mblk_t *mp;
	pl_t pl;

	pl = bufq_lock(&sp->urgq);
	while (ckp->mrem && ckp->swnd && (mp = bufq_head(&sp->urgq))) {
		mblk_t *db;
		struct sctp_data *m = (typeof(m)) mp->b_rptr;
		sctp_tcb_t *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(*m) + dlen);

		ensure(cb->st, continue);
#if 0
#if defined(SCTP_CONFIG_DEBUG)||defined(SCTP_CONFIG_TEST)||defined(SCTP_CONFIG_SAFE)
		/* this should only occur if the pmtu is falling */
		if (sp->amps <= ckp->mrem && plen > sp->amps) {
			rare();
			sctp_frag_chunk(&sp->urgq, mp, sp->amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
#endif
		if (plen > ckp->mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		if (dlen > ckp->swnd && sd->in_flight)
			goto congested;
		if ((mp == cb->st->x.head)) {
			sctplogda(sp, "stole partial");
			cb->st->x.head = NULL;	/* steal partial */
		}
		if (!(db = sctp_dupmsg(sp, mp)))
			goto enobufs;
		cb->tsn = sp->t_tsn++;
		cb->flags |= SCTPCB_FLAG_SENT;
		cb->when = jiffies;
		cb->daddr = sd;
		m->tsn = htonl(cb->tsn);
		ckp->swnd -= dlen;
		sd->in_flight += dlen;
		sp->in_flight += dlen;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = db;
		ckp->dpp = &(db->b_next);
		db->b_next = NULL;
		bufq_queue(&sp->rtxq, __bufq_unlink(&sp->urgq, mp));
		SCTP_INC_STATS(SctpOutUnorderChunks);
		sctplogda(sp, "bundling DATA chunk (unordered)");
	}
	bufq_unlock(&sp->urgq, pl);
	return (0);
      congested:
	bufq_unlock(&sp->urgq, pl);
	return (0);
      wait_for_next_packet:
	rare();
	sctplogtx(sp, "%s() cannot bundle urgent", __FUNCTION__);
	bufq_unlock(&sp->urgq, pl);
	return (1);
      enobufs:
	bufq_unlock(&sp->urgq, pl);
	return (1);
}

/*
 *  BUNDLE NEW NORMAL (ORDERED) DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_data_normal(struct sctp *sp,	/* association */
			struct sctp_daddr *sd,	/* destination */
			struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	mblk_t *mp;
	pl_t pl;

	pl = bufq_lock(&sp->sndq);
	while (ckp->mrem && ckp->swnd && (mp = bufq_head(&sp->sndq))) {
		mblk_t *db;
		struct sctp_data *m = (typeof(m)) mp->b_rptr;
		sctp_tcb_t *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(*m) + dlen);

		ensure(cb->st, continue);
#if 0
#if defined(SCTP_CONFIG_DEBUG)||defined(SCTP_CONFIG_TEST)||defined(SCTP_CONFIG_SAFE)
		/* this should only occur if the pmtu is falling */
		if (sp->amps <= ckp->mrem && plen > sp->amps) {
			rare();
			sctp_frag_chunk(&sp->sndq, mp, sp->amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
#endif
		if (plen > ckp->mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		if (dlen > ckp->swnd && sd->in_flight)
			goto congested;
		if ((mp == cb->st->n.head)) {
			sctplogda(sp, "stole partial");
			cb->st->n.head = NULL;	/* steal partial */
		}
		if (!(db = sctp_dupmsg(sp, mp)))
			goto enobufs;
		cb->tsn = sp->t_tsn++;
		cb->flags |= SCTPCB_FLAG_SENT;	/* this is data */
		cb->when = jiffies;
		cb->daddr = sd;
		m->tsn = htonl(cb->tsn);
		ckp->swnd -= dlen;
		sd->in_flight += dlen;
		sp->in_flight += dlen;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = db;
		ckp->dpp = &(db->b_next);
		db->b_next = NULL;
		bufq_queue(&sp->rtxq, __bufq_unlink(&sp->sndq, mp));
		SCTP_INC_STATS(SctpOutOrderChunks);
		sctplogda(sp, "bundling DATA chunk (ordered)");
	}
	bufq_unlock(&sp->sndq, pl);
	return (0);
      congested:
	bufq_unlock(&sp->sndq, pl);
	return (0);
      wait_for_next_packet:
	seldom();
	bufq_unlock(&sp->sndq, pl);
	return (1);
      enobufs:
	bufq_unlock(&sp->sndq, pl);
	return (1);
}

/*
 *  BUNDLE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Tack on SACK, ERROR, and DATA chunks up to the destination MTU considering
 *  congestion windows and fragmentation sizes.
 */
STATIC int
sctp_bundle_more(struct sctp *sp, struct sctp_daddr *sd, mblk_t *mp, int nonagle)
{
	struct sctp_bundle_cookie cookie;
	static const size_t hlen = sizeof(struct sctphdr);
	int rtn = 0;

	/* find transmission control block */
	sctp_tcb_t *cb = SCTP_TCB(mp);

	/* initialize argument cookie */
	cookie.dpp = &(mp->b_next);
	cookie.mlen = msgdsize(mp);
	cookie.mrem = (cookie.mlen < sd->dmps + hlen) ? sd->dmps + hlen - cookie.mlen : 0;
	cookie.swnd = sctp_avail(sp, sd);
	cookie.pbuf = sp->p_rbuf >> 1;
	if (((sp->sackf & SCTP_SACKF_NOW) && !sp->rq->q_count)
	    || (sp->nrtxs)
	    || (bufq_head(&sp->urgq))
	    || (bufq_head(&sp->errq))
	    || ((!sp->in_flight || nonagle == 1) && bufq_head(&sp->sndq))
	    /* SCTP IG 2.15.2, SWS avoidance sending RFC 1122 4.2.3.4 */
	    || (bufq_size(&sp->sndq) >= cookie.mrem && cookie.swnd >= cookie.mrem)
	    || (bufq_size(&sp->sndq) >= cookie.pbuf && cookie.swnd >= cookie.pbuf)
	    /* Manshall modification */
	    || !before(sp->m_tsn, sp->t_ack)
	    ) {
		uint32_t t_tsn = sp->t_tsn;
		int res, max = 0, was_in_flight = sp->in_flight;

		*cookie.dpp = NULL;
		if (cookie.mrem && (sp->sackf & SCTP_SACKF_ANY)) {
			res = sctp_bundle_sack(sp, sd, &cookie);
			max |= res;
		}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		if (cookie.mrem && (sp->sackf & SCTP_SACKF_FSN)) {
			res = sctp_bundle_fsn(sp, sd, &cookie);
			max |= res;
		}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
		if (cookie.mrem && bufq_head(&sp->errq)) {
			res = sctp_bundle_error(sp, sd, &cookie);
			max |= res;
		}
#ifdef SCTP_CONFIG_ECN
		/* really only supposed to be bundled with new data */
		if (cookie.mrem && (sp->sackf & SCTP_SACKF_CWR) && bufq_head(&sp->sndq)) {
			res = sctp_bundle_cwr(sp, sd, &cookie);
			max |= res;
		}
#endif				/* SCTP_CONFIG_ECN */
		if (cookie.swnd) {	/* subject to flow control */
			if (cookie.mrem && bufq_head(&sp->urgq)) {
				res = sctp_bundle_data_urgent(sp, sd, &cookie);
				max |= res;
				rtn += res;
			}
			if (cookie.mrem && sp->nrtxs) {
				res = sctp_bundle_data_retrans(sp, sd, &cookie);
				max |= res;
			}
			if (cookie.mrem && bufq_head(&sp->sndq)) {
				res = sctp_bundle_data_normal(sp, sd, &cookie);
				max |= res;
				rtn += res;
			}
			cb->dlen = cookie.mlen;
			if ((sd->in_flight || (sd->flags & SCTP_DESTF_FORWDTSN)))
				sd_timer_cond_retrans(sd, sd->rto);
			if (!max && bufq_head(&sp->sndq) && after(sp->t_tsn, t_tsn))
				sp->m_tsn = t_tsn;	/* sent a short packet */
			else if (!was_in_flight)
				sp->m_tsn = sp->t_ack;
		}
	}
	return (rtn);
}

/*
 *  ALLOC CHK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocate an mblk or sk_buff for a chunk without hard-header, IP header or SCTP header headroom.
 *  This is used for allocating mblks or sk_buffs that are used to contain a chunk for bundling.
 *  This is only used for bundling control chunks and not for bundling data chunks, therefore, we do
 *  not allocate write space for these chunks.
 */
STATIC mblk_t *
sctp_alloc_chk(struct sctp *sp, size_t clen, size_t dlen)
{
	mblk_t *mp;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);

	assert(sp);
	if ((mp = sctp_allocb(sp, sizeof(*cb) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* initialized control block */
		cb = (typeof(cb)) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = dlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);
		/* pre-zero padding */
		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/*
 *  ALLOC CTL
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocate a mblk for a chunk without hard-header, IP header or SCTP header headroom.  This is
 *  used for allocating mblks which are used to track a received chunk.  This is really only to
 *  create a transmission control block for STREAMS which does not have a control block in the
 *  message block structure.  Sockets has a transmission control block in each socket buffer.
 */
STATIC mblk_t *
sctp_alloc_ctl(struct sctp *sp, size_t clen, size_t dlen)
{
	mblk_t *mp;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);

	assert(sp);
	if ((mp = sctp_allocb(sp, sizeof(*cb) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_CTL;
		/* initialized control block */
		cb = (typeof(cb)) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = dlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);
		/* pre-zero padding */
		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/*
 *  ALLOC MSG
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocates a new message block with a control block, an SCTP message header, (for sockets, room
 *  for an IP header and hard header) and, optionally, the first chunk of a message.  This is used
 *  for allocating messages that contain a single control chunk and for allocating the header for a
 *  message that will have additional chunks bundled.
 */
STATIC mblk_t *
sctp_alloc_msg(struct sctp *sp, size_t clen)
{
	mblk_t *mp;
	struct sctphdr *sh;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);
	size_t mlen = sizeof(*sh) + plen;
	size_t hlen = 0;

	assert(sp);
	if ((mp = sctp_allocb(sp, sizeof(*cb) + hlen + mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* initialize control block */
		cb = (sctp_tcb_t *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = mlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);
		/* pull the hard and IP header headroom */
		mp->b_wptr += hlen;
		sh = (struct sctphdr *) mp->b_wptr;
		sh->srce = sp->sport;
		sh->dest = sp->dport;
		sh->v_tag = sp->p_tag;
		sh->check = 0;
		mp->b_wptr += sizeof(*sh);
		/* pre-zero padding */
		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/*
 *  ALLOC REPLY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for allocating a message block with a hidden transmission control block for a message
 *  (with IP header room for sockets) and an STCP header.  This is for out of the blue replies and
 *  all that is required is the SCTP header of the message to which this is a reply.
 */
STATIC mblk_t *
sctp_alloc_reply(struct sctp *sp, struct sctphdr *rh, size_t clen)
{
	mblk_t *mp;
	struct sctphdr *sh;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);
	size_t mlen = sizeof(*sh) + plen;
	size_t hlen = 0;

	ensure(rh, return NULL);
	if ((mp = sctp_allocb(sp, sizeof(*cb) + hlen + mlen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		/* initialize control block */
		cb = (sctp_tcb_t *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = mlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);
		/* reserve IP and hard header room */
		mp->b_wptr += hlen;
		sh = (struct sctphdr *) mp->b_wptr;
		sh->srce = rh->dest;
		sh->dest = rh->srce;
		sh->v_tag = rh->v_tag;
		sh->check = 0;
		mp->b_wptr += sizeof(*sh);
		/* pre-zero padding */
		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/*
 *  FREE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for freeing a chain list of mblks or sk_buffs that represent a packet built out of
 *  separate chunks.  Chunks in mblks are chained together with the mp->b_next pointer, chunks in
 *  sk_buffs are chained together with the cb->next pointer.
 */
STATIC INLINE void
freechunks(mblk_t *mp)
{
	if (mp) {
		mblk_t *mp_next = mp;

		while ((mp = mp_next)) {
			mp_next = mp->b_next;
			freemsg(mp);
		}
	}
}

/*
 *  ROUTE SELECTION
 *  -------------------------------------------------------------------------
 *  
 *  ROUTE NORMAL
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Send to the normal transmit (primary) address.  If that has timedout, we use the retransmit
 *  (secondary) address.  We always use the secondary address if we have retransmit chunks to bundle
 *  or if we have be receiving duplicates (our SACKs are not getting through on the primary
 *  address).
 *
 */
STATIC void sctp_abort(struct sctp *sp, t_uscalar_t origin, t_scalar_t reason);

/*
 *  This version for use by bottom halves that have the socket or stream locked so that it will not
 *  attempt to lock the socket or stream on disconnect.
 */
STATIC struct sctp_daddr *
sctp_route_normal(struct sctp *sp)
{
	int err;
	struct sctp_daddr *sd;

	assert(sp);
	if ((err = sctp_update_routes(sp, 1))) {
		rare();
		sctplogst(sp, "%s() no viable route", __FUNCTION__);
		/* we have no viable route */
		if ((1 << sp->state) & (SCTPF_OPENING)) {
			/* Only abort the association if we are opening and have no viable route.
			   If we have an established or closing association, wait for timers to
			   fire and retransmissions to peg before aborting.  Then associations will
			   not abort simply due to temporary routing changes. */
			sctp_abort(sp, SCTP_ORIG_PROVIDER, -EHOSTUNREACH);
		}
		return (NULL);
	}
	sd = sp->taddr;
	normal(sd);
	return (sd);
}

/*
 *  ROUTE RESPONSE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  In response to control chunks we normally send back to the address that the control chunk came
 *  from.  If that address is unusable or wasn't provided or is suspect we send as normal.
 */
STATIC struct sctp_daddr *
sctp_route_response(struct sctp *sp)
{
	struct sctp_daddr *sd;

	assert(sp);
	sd = sp->caddr;
	if (!sd || !sd->dst_cache || sd->dst_cache->obsolete || sd->retransmits || sd->dups)
		sd = sctp_route_normal(sp);
	normal(sd);
	return (sd);
}

#ifdef SCTP_CONFIG_ADD_IP
STATIC void sctp_send_asconf(struct sctp *sp);
#endif				/* SCTP_CONFIG_ADD_IP */
/**
 * ___sctp_transmit_wakeup: - wake up transmitters (send SACK, ERROR, DATA)
 * @sp: private structure (locked)
 *
 * TODO: When the user has specified a destination address to t_sndopt() or sctp_sendmsg() we want
 * to suppress nagle and do a destination-specific bundling instead of our normal route selection.
 * We could do this by peeking the urgq and the sndq to check for user-specified destinations before
 * selecting the destination and calling sctp_bundle_more.  This looks like it will have a
 * performance impact so we will leave it for now.
 */
STATIC inline streamscall __hot_out void
___sctp_transmit_wakeup(struct sctp *sp)
{
	int i, n, reroute = 0;
	mblk_t *mp;
	struct sctp_daddr *sd;
	int loop_max = 1000;

	sctplogda(sp, "performing transmitter wakeup");
	ensure(sp, return);
	if ((1 << sp->state) & ~(SCTPF_CONNECTED))
		goto skip;
#ifdef SCTP_CONFIG_ADD_IP
	if (sp->state == SCTP_ESTABLISHED && sp->sackf & SCTP_SACKF_ASC)
		sctp_send_asconf(sp);
#endif				/* SCTP_CONFIG_ADD_IP */
	/* SCTP IG 2.8, 2.14 */
//      for (i = 0, n = 0; i < loop_max && n <= sp->max_burst; i++) {
	for (i = 0, n = 0; i < loop_max; i++) {
		/* placed in order of probability */
		if (!bufq_head(&sp->sndq) && (!(sp->sackf & SCTP_SACKF_NOW) || sp->rq->q_count)
		    && !sp->nrtxs && !bufq_head(&sp->urgq)
		    && !bufq_head(&sp->errq))
			goto done;
		if (!(sd = sctp_route_normal(sp)))
			goto noroute;
		if (!(mp = sctp_alloc_msg(sp, 0)))
			goto enobufs;
		n += sctp_bundle_more(sp, sd, mp, sp->nonagle);
		if (mp->b_next) {
			reroute = 0;
			sctp_send_msg(sp, sd, mp);
			freechunks(mp);
			continue;
		}
		if (!reroute) {
			reroute = 1;
			freechunks(mp);
			continue;
		}
		goto discard;
	}
	assure(i < loop_max);
	return;
      discard:
	freechunks(mp);
	goto done;
      noroute:
	ptrace(("ERROR: no route to peer\n"));
	goto done;
      enobufs:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto done;
      done:
	assure(i > 0 || !(sp->sackf & SCTP_SACKF_NOW) || sp->rq->q_count);
      skip:
	// sctplogerr(sp, "skipping wakeup in incorrect state");
	return;
}

STATIC void
sctp_transmit_wakeup(struct sctp *sp)
{
	/* The STREAMS version defers transmitter wakeup by scheduling the write queue of the
	   stream.  This eventually calls ___sctp_transmit_wakeup. On the sockets version, we have
	   to call ___sctp_transmit_wakeup directly. */
	if (sp->wq) {
		qenable(sp->wq);
		return;
	}
	sctplogerr(sp, "%s() with no wq", __FUNCTION__);
}

/*
 *  =========================================================================
 *
 *  SCTP State Machine Indications
 *
 *  =========================================================================
 */

/*
 * The following are SCTP state machine indications and confirmations generated by the core SCTP
 * service interface independent state machine.  The specific service primitive interface operation
 * translates these into TPI or NPI specific actions.
 */

/**
 * sctp_conn_ind: - generate a connection indication
 * @sp: private structure
 * @cp: connection indication
 */
STATIC INLINE int
sctp_conn_ind(struct sctp *sp, mblk_t *cp)
{
	/* connection indication */
	sctplogtx(sp, "X_CONN_IND <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->conn_ind)
			return sp->ops->conn_ind(sp, cp);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}

/**
 * sctp_conn_con: - generate a connection confirmation
 * @sp: private structure
 */
STATIC INLINE int
sctp_conn_con(struct sctp *sp)
{
	sctplogtx(sp, "X_CONN_CON <-");
	assert(sp);
	if (sp->rq) {
		int err = 0;
		int oldstate = sctp_change_state(sp, SCTP_ESTABLISHED);

		if (sp->ops->conn_con)
			err = sp->ops->conn_con(sp);
		if (err < 0)
			sctp_change_state(sp, oldstate);
		return (err);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}

/**
 * sctp_data_ind: - generate a data indication
 * @sp: private structure
 * @ppi: Payload Protocol Identifier
 * @sid: stream identifier
 * @ssn: stream sequence number
 * @tsn: transmit sequence number
 * @ord: true when ordered
 * @more: true when partial segment
 * @dp: user data
 *
 * Indicates to the SCTP user that data has arrived, the ancilliary information associated with the
 * data, and the data itself.
 */
STATIC INLINE int
sctp_data_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
	      uint more, mblk_t *dp)
{
	sctplogda(sp, "X_DATA_IND <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->data_ind)
			return sp->ops->data_ind(sp, ppi, sid, ssn, tsn, ord, more, dp);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}

/**
 * sctp_datack_ind: - data acknowledgement indication
 * @sp: private structure
 * @ppi: payload protocol identifier
 * @sid: stream identifier
 * @ssn: stream sequence number
 * @tsn: transmit sequence number
 *
 * Indicates to the SCTP user that data has been acknowledged received by the peer and the
 * ancilliary information associated with the data.  Note that not all service interfaces supports
 * data acknowledgements.
 */
STATIC INLINE int
sctp_datack_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn)
{
	sctplogda(sp, "X_DATACK_IND <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->datack_ind)
			return sp->ops->datack_ind(sp, ppi, sid, ssn, tsn);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}

/**
 * sctp_discon_ind: - disconnect indication
 * @sp: private structure
 * @origin: origin of disconnect
 * @reason: reason for disconnect
 * @cp: connection indication being disconnected
 *
 * When passed an active connection indication, the interface specific disconnect indication
 * function must unlink and free the connection indication or return an error.  Other than that, the
 * interface specific disconnect indication function merely informs the interface of the disconnect
 * and takes no other action.
 */
STATIC INLINE int
sctp_discon_ind(struct sctp *sp, t_uscalar_t origin, t_scalar_t reason, mblk_t *cp)
{
	sctplogtx(sp, "X_DISCON_IND <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->discon_ind)
			return sp->ops->discon_ind(sp, origin, reason, cp);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	if (cp)
		freemsg(cp);
	return (0);
}

/**
 * sctp_ordrel_ind: - orderly release indication
 * @sp: private structure
 *
 * Inidicates to the SCTP user that an orderly release has been initiated by the peer and not to
 * expect further data from the peer.  Not all service interfaces support orderly release.  If the
 * service interface does not support orderly release, abortive release (disconnect) will be used
 * instead.
 */
STATIC INLINE int
sctp_ordrel_ind(struct sctp *sp)
{
	sctplogtx(sp, "X_ORDREL_IND <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->ordrel_ind)
			return sp->ops->ordrel_ind(sp);
		sctp_abort(sp, SCTP_ORIG_USER, SCTP_CAUSE_USER_INITIATED);
		return (0);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}

/**
 * sctp_reset_ind: - reset indication
 * @sp: private structure
 * @origin: origin of reset
 * @reason: reason for reset
 * @cp: connection indication
 *
 * Indicates to the SCTP user that the connection has been reset.  There is currently only one
 * reason for reset: association restart.  Not all service interfaces support reset indications. If
 * the service interface does not support reset indications, abortive release (disconnect) will be
 * performed instead.
 */
STATIC INLINE int
sctp_reset_ind(struct sctp *sp, t_uscalar_t origin, t_scalar_t reason, mblk_t *cp)
{
	sctplogtx(sp, "X_RESET_IND <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->reset_ind)
			return sp->ops->reset_ind(sp, origin, reason, cp);
		sctp_abort(sp, origin, reason);
		return (0);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}

/**
 * sctp_reset_con:- reset confirmation
 * @sp: private structure
 *
 * Indicates to the SCTP user that the previous SCTP user initiated reset has completed.  There is
 * currently only one reason for reset: SCTP association restart.
 */
STATIC INLINE int
sctp_reset_con(struct sctp *sp)
{
	sctplogtx(sp, "X_RESET_CON <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->reset_con)
			return sp->ops->reset_con(sp);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}

#if 0
/**
 * sctp_retr_ind: - retrieval indication
 * @sp: private structure
 * @dp: user data
 *
 * Indicates to the SCTP user that user data has been retrieved.  Not all service interfaces support
 * retrieval (only the SCTPI and AALI do at the moment).
 */
STATIC INLINE int
sctp_retr_ind(struct sctp *sp, mblk_t *dp)
{
	sctplogtx(sp, "X_RETRV_IND <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->retr_ind)
			return sp->ops->retr_ind(sp, dp);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}
#endif

#if 0
/**
 * sctp_retr_con: - retrieval confirmation
 * @sp: private structure
 * 
 * Confirms to the SCTP user that user data retrieval is complete.  Not all service interface
 * support retrieval (only the SCTPI and AALI do at the moment).
 */
STATIC INLINE int
sctp_retr_con(struct sctp *sp)
{
	sctplogtx(sp, "X_RETRV_CON <-");
	assert(sp);
	if (sp->rq) {
		if (sp->ops->retr_con)
			return sp->ops->retr_con(sp);
	}
	sctplogerr(sp, "%s() with no rq", __FUNCTION__);
	return (-EFAULT);
}
#endif

#ifdef SCTP_CONFIG_ECN
/**
 * send_ecne: - send ECNE (Explicit Congestion Notification Echo)
 * @sp: private structure
 *
 * Just marks an ECNE chunk to be sent with the next SACK (and don't delay SACKs).
 */
STATIC inline fastcall void
sctp_send_ecne(struct sctp *sp)
{
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
		sctplogda(sp, "marking ECNE");
		sp->sackf |= SCTP_SACKF_ECN;
		sctp_transmit_wakeup(sp);
	}
}

/**
 * sctp_send_cwr: - send CWR (Congestion Window Reduction)
 * @sp: private structure
 *
 * Just mark a CWR chunk to be bundled with the next DATA.
 */
STATIC inline fastcall void
sctp_send_cwr(struct sctp *sp)
{
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
		sctplogda(sp, "marking CWR");
		sp->sackf |= SCTP_SACKF_CWR;
		sctp_transmit_wakeup(sp);
	}
}
#endif				/* SCTP_CONFIG_ECN */
/*
 *  =========================================================================
 *
 *  SCTP State Machine TIMEOUTS
 *
 *  =========================================================================
 */
STATIC void sctp_send_heartbeat(struct sctp *sp, struct sctp_daddr *sd);

/*
 *  ASSOCIATION TIMEOUT FUNCTION
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_assoc_timedout(struct sctp *sp,	/* association */
		    struct sctp_daddr *sd,	/* destination */
		    size_t rmax,	/* retry maximum */
		    int stroke /* whether to stroke counts */ )
{
	__ensure(sp, return (-EFAULT));
	__ensure(sd, return (-EFAULT));
	/* RFC 2960 6.3.3 E1 and 7.2.3, E2, E3 and 8.3 */
	sd->ssthresh = ((sd->cwnd >> 1) > (sd->mtu << 1)) ? sd->cwnd >> 1 : sd->mtu << 1;
	sd->cwnd = sd->mtu;
	/* SCTP IG Section 2.9 */
	sd->partial_ack = 0;
	sd->rto = (sd->rto) ? sd->rto << 1 : 1;
	sd->rto = (sd->rto_min > sd->rto) ? sd->rto_min : sd->rto;
	sd->rto = (sd->rto_max < sd->rto) ? sd->rto_max : sd->rto;
	ptrace(("Timeout calculation:\n  sd->retransmits = %d\n  sp->retransmits = %d\n  ssthresh = %d\n  cwnd = %d\n  rto = %d\n", (uint) sd->retransmits, (uint) sp->retransmits, (uint) sd->ssthresh, (uint) sd->cwnd, (int) sd->rto));
#ifdef SCTP_CONFIG_ECN
	sctp_send_cwr(sp);
#endif				/* SCTP_CONFIG_ECN */
	/* SCTP IG 2.15: don't stroke counts on zero window probes, or ECN */
	if (!stroke && sp->state != SCTP_COOKIE_WAIT)
		return (0);
	/* See RFC 2960 Section 8.3 */
	if (++sd->retransmits > sd->max_retrans) {
		if (sd->retransmits > sd->max_retrans + 1) {
			if ((1 << sp->state) & (SCTPF_CONNECTED))
				return (0);
		} else {
			if (sd->dst_cache)
				dst_negative_advice(&sd->dst_cache);
#if 0
			if (sd->dst_cache)
				dst_release(xchg(&sd->dst_cache, NULL));
#endif
			if (!sd->dst_cache)
				sd->saddr = 0;
#if 1
			/* IMPLEMENTATION NOTE:- When max_retrans and rto_max are set to zero, we
			   are cruel on destinations that drop a single packet due to noise.  This
			   forces an immediate heartbeat on the destination so that it can be made
			   available again quickly if it passes the heartbeat. */
			if (sd->rto_max < 2 && sd->max_retrans == 0
			    && (1 << sp->state) & (SCTPF_CONNECTED | SCTPF_CLOSING)) {
				sd_timer_cancel_idle(sd);
				sctp_send_heartbeat(sp, sd);
			}
#endif
		}
	}
	/* See RFC 2960 Section 8.2 */
	if (rmax && sp->retransmits++ >= rmax) {
		int err;

		seldom();
		if ((err = sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -ETIMEDOUT, NULL)) >= 0)
			err = -ETIMEDOUT;
		sctp_abort(sp, SCTP_ORIG_PROVIDER, -ETIMEDOUT);
		return (err);
	}
	return (0);
}

/**
 * __sctp_init_timeout: - init timeout (T1-init)
 * @data: private structure pointer
 *
 * The init timer has expired indicating that we have not received an INIT ACK within timer T1-init.
 * This means that we should attempt to retransmit the INIT until we have attempted Max.Init.Retrans
 * times.
 */
STATIC fastcall void
__sctp_init_timeout(struct sctp *sp)
{
	struct sctp_daddr *sd = sp->taddr;

	sctplogto(sp, "INIT Timeout");
	if (sp->state != SCTP_COOKIE_WAIT)
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_inits ? sp->max_inits : 1, sp->p_rwnd))
		goto timedout;
	sd = sp->taddr;		/* might have new primary */
	sp_timer_init(sp, sd->rto);
	sctp_send_msg(sp, sd, sp->retry);
      done:
	return;
      timedout:
	sctplogto(sp, "%s() association timed out", __FUNCTION__);
	goto done;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_init_timeout(void *arg)
{
	struct sctp *sp = (typeof(sp)) arg;

	if (xchg(&sp->timer_init, 0) != (toid_t) 0)
		sp_timeout(sp, &__sctp_init_timeout, SCTP_TBIT_INIT);
}

/**
 * __sctp_cookie_timeout: cookie timeout (T1-cookie)
 * @data: private structure pointer
 * 
 * The cookie timer has expired indicating that we have not yet received a COOKIE ACK within timer
 * T1-cookie.  This means that we should attempt to retransmit the COOKIE ECHO until we have
 * attempted Path.Max.Retrans times.
 */
STATIC fastcall void
__sctp_cookie_timeout(struct sctp *sp)
{
	struct sctp_daddr *sd = sp->taddr;
	mblk_t *mp;
	pl_t pl;

	sctplogto(sp, "COOKIE Timeout");
	if (sp->state != SCTP_COOKIE_ECHOED)
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	/* See RFC 2960 6.3.3 E3 */
	pl = bufq_lock(&sp->rtxq);
	for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
		    && !(cb->flags & (SCTPCB_FLAG_RETRANS | SCTPCB_FLAG_DROPPED))) {
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			cb->sacks = SCTP_FR_COUNT;	/* not eligible for FR now */
		}
	}
	bufq_unlock(&sp->rtxq, pl);
	if ((sd = sctp_route_normal(sp))) {
		sp_timer_cookie(sp, sd->rto);
		sctp_send_msg(sp, sd, sp->retry);
	}
      done:
	return;
      timedout:
	sctplogto(sp, "%s() association timed out", __FUNCTION__);
	goto done;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_cookie_timeout(void *arg)
{
	struct sctp *sp = (typeof(sp)) arg;

	if (xchg(&sp->timer_cookie, 0) != (toid_t) 0)
		sp_timeout(sp, &__sctp_cookie_timeout, SCTP_TBIT_COOKIE);
}

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
/*
 *  SEND FORWARD TSN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE void
sctp_send_forward_tsn(struct sctp *sp)
{
	struct sctp_daddr *sd;

	/* PR-SCTP 3.5 (F2) */
	sctplogto(sp, "FORWARD-TSN Timeout");
	sp->sackf |= SCTP_SACKF_FSN;
	for (sd = sp->daddr; sd; sd = sd->next) {
		if (sd->flags & SCTP_DESTF_FORWDTSN) {
			if (!sd->in_flight)
				sd_timer_cancel_retrans(sd);
			sd->flags &= ~SCTP_DESTF_FORWDTSN;
		}
	}
	sctp_transmit_wakeup(sp);
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

/**
 * __sctp_retrans_timeout: retrans timeout (T3-rtx)
 * @data: destination structure pointer
 * 
 * This means that we have not received an ack for a DATA chunk within timer T3-rtx.  This means
 * that we should mark all outstanding DATA chunks for retransmission and start a retransmission
 * cycle.
 */
STATIC fastcall void
__sctp_retrans_timeout(struct sctp_daddr *sd)
{
	sctp_t *sp = sd->sp;
	int retransmits = 0;
	mblk_t *mp;
	pl_t pl;

	sctplogto(sp, "RETRANS Timeout");
	if ((1 << sp->state) & ~(SCTPF_CONNECTED))
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	/* See RFC 2960 6.3.3 E3 */
	pl = bufq_lock(&sp->rtxq);
	for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
		sctp_tcb_t *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;

		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
		    && !(cb->flags & (SCTPCB_FLAG_RETRANS
				      | SCTPCB_FLAG_SACKED | SCTPCB_FLAG_DROPPED))) {
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			cb->sacks = SCTP_FR_COUNT;	/* not eligible for FR now */
			normal(sd->in_flight >= dlen);
			normal(sp->in_flight >= dlen);
			/* credit dest and assoc */
			sd->in_flight = (sd->in_flight > dlen) ? sd->in_flight - dlen : 0;
			sp->in_flight = (sp->in_flight > dlen) ? sp->in_flight - dlen : 0;
			retransmits++;
		}
	}
	bufq_unlock(&sp->rtxq, pl);
	normal(retransmits);
#ifdef SCTP_CONFIG_ADD_IP
	if (sd->flags & SCTP_DESTF_UNUSABLE)
		sctp_del_daddr(sd);
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sp);
		sp_timer_cond_sack(sp, sp->max_sack);
		sp->sackf |= SCTP_SACKF_NOD;	/* don't delay retransmitted FWD TSN */
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_timer_cancel_sack(sp);
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	sctp_transmit_wakeup(sp);
      done:
	return;
      timedout:
	sctplogto(sp, "%s() association timed out", __FUNCTION__);
	goto done;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_retrans_timeout(void *arg)
{
	struct sctp_daddr *sd = (typeof(sd)) arg;

	if (xchg(&sd->timer_retrans, 0) != (toid_t) 0)
		sd_timeout(sd, &__sctp_retrans_timeout, SCTP_TBIT_RETRANS);
}

/**
 * sctp_ack_timeout: - SACK timeout
 * @data: private structure pointer
 *
 * This timer is the 200ms timer which ensures that a SACK is sent within 200ms of the receipt of an
 * unacknoweldged DATA chunk.  When an unacknowledged DATA chunks i receive and the timer is not
 * running, the timer is set.  Whenever a DATA chunks(s) are acknowledged, the timer is stopped.
 */
STATIC fastcall void
__sctp_sack_timeout(struct sctp *sp)
{
	sctplogto(sp, "SACK Timeout");
	if ((1 << sp->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	sp->sackf |= SCTP_SACKF_TIM;	/* RFC 2960 6.2 */
	sctp_transmit_wakeup(sp);
      done:
	return;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_sack_timeout(void *arg)
{
	struct sctp *sp = (typeof(sp)) arg;

	if (xchg(&sp->timer_sack, 0) != (toid_t) 0)
		sp_timeout(sp, &__sctp_sack_timeout, SCTP_TBIT_SACK);
}

/**
 * __sctp_idle_timeout: idle timeout
 * @data: destination structure pointer
 *
 * This means that a destination has been idle for longer than the hb.itvl or the interval for which
 * we must send heartbeats.  This timer is reset every time we do an RTT calculation for a
 * destination.  It is stopped while sending heartbeats and started again whenever an RTT
 * calculation is done.  While this timer is stopped, heartbeats will be sent until they are
 * acknowledged.
 */
STATIC fastcall void
__sctp_idle_timeout(struct sctp_daddr *sd)
{
	sctp_t *sp = sd->sp;

	sctplogto(sp, "IDLE Timeout");
	/* SCTP IG 2.10 but we continue through closing states */
	if ((1 << sp->state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	sctp_send_heartbeat(sp, sd);
      done:
	return;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_idle_timeout(void *arg)
{
	struct sctp_daddr *sd = (typeof(sd)) arg;

	if (xchg(&sd->timer_idle, 0) != (toid_t) 0)
		sd_timeout(sd, &__sctp_idle_timeout, SCTP_TBIT_IDLE);
}

/**
 * __sctp_heartbeat_timeout: - HEARTBEAT timeout
 * @data: destination structure pointer
 *
 * If we get a heartbeat timeout we adjust RTO the same as we do for retransmit (and the congestion
 * window) and resend the heartbeat.  Once we have sent Path.Max.Retrans heartbeats unsuccessfully,
 * we mark the destination as unusable, but continue heartbeating until they get acknowledged.
 * (Well!  That's not really true, is it?)
 */
STATIC void sctp_reset_idle(struct sctp_daddr *sd);
STATIC fastcall void
__sctp_heartbeat_timeout(struct sctp_daddr *sd)
{
	struct sctp *sp = sd->sp;

	sctplogto(sp, "HEARTBEAT Timeout");
	/* SCTP IG 2.10 but we continue through closing states */
	if ((1 << sp->state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	/* SCTP IG 2.10 */
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, 1))
		goto timedout;
	sctp_reset_idle(sd);
#if 0
	sctp_send_heartbeat(sp, sd);
#endif
      done:
	return;
      timedout:
	sctplogto(sp, "%s() association timed out", __FUNCTION__);
	goto done;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_heartbeat_timeout(void *arg)
{
	struct sctp_daddr *sd = (typeof(sd)) arg;

	if (xchg(&sd->timer_heartbeat, 0) != (toid_t) 0)
		sd_timeout(sd, &__sctp_heartbeat_timeout, SCTP_TBIT_HEARTBEAT);
}

/**
 * __sctp_shutdown_timeout: - SHUTDOWN timeout
 * @data: private structure pointer
 *
 * This means that we have timedout on sending a SHUTDOWN or a SHUTDOWN ACK message.  We simply
 * resend the message.
 */
STATIC fastcall void
__sctp_shutdown_timeout(struct sctp *sp)
{
	struct sctp_daddr *sd = sp->taddr;

	sctplogto(sp, "SHUTDOWN Timeout");
	if (!((1 << sp->state) & (SCTPF_CLOSING)))
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	if (!(sd = sctp_route_normal(sp)))
		goto done;
	sp_timer_shutdown(sp, sd->rto);
	normal(sp->retry);
	sctp_send_msg(sp, sd, sp->retry);
      done:
	return;
      timedout:
	sctplogto(sp, "%s() association timed out", __FUNCTION__);
	goto done;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_shutdown_timeout(void *arg)
{
	struct sctp *sp = (typeof(sp)) arg;

	if (xchg(&sp->timer_shutdown, 0) != (toid_t) 0)
		sp_timeout(sp, &__sctp_shutdown_timeout, SCTP_TBIT_SHUTDOWN);
}

/**
 * __sctp_guard_timeout: - guard timeout
 * @data: private structure pointer
 *
 * This means that we have timed out in the over SHUTDOWN process and need to ABORT the association
 * per SCTP IG 2.12.
 */
STATIC void sctp_send_abort(struct sctp *sp);
STATIC fastcall void
__sctp_guard_timeout(struct sctp *sp)
{
	sctplogto(sp, "GUARD Timeout");
	if ((1 << sp->state) & ~(SCTPF_CLOSING))
		goto outstate;
	sctp_send_abort(sp);
	sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -ETIMEDOUT, NULL);
	sctp_abort(sp, SCTP_ORIG_PROVIDER, -ETIMEDOUT);
      done:
	return;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_guard_timeout(void *arg)
{
	struct sctp *sp = (typeof(sp)) arg;

	if (xchg(&sp->timer_guard, 0) != (toid_t) 0)
		sp_timeout(sp, &__sctp_guard_timeout, SCTP_TBIT_GUARD);
}

#ifdef SCTP_CONFIG_ADD_IP
/**
 * __sctp_asconf_timeout: - ASCONF timeout
 * @data: private structure pointer
 *
 * This means that we have timedout on sending a ASCONF message.  We simply resend the message.
 */
STATIC fastcall void
__sctp_asconf_timeout(struct sctp *sp)
{
	struct sctp_daddr *sd = sp->taddr;

	sctplogto(sp, "ASCONF Timeout");
	if ((1 << sp->state) & ~(SCTPF_ESTABLISHED))
		goto outstate;
	/* ADD-IP (B1), (B2) and (B3) */
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	if (!(sd = sctp_route_normal(sp)))
		goto noroute;
	/* ADD-IP (B5) */
	sp_timer_asconf(sp, sd->rto);
	/* ADD-IP (B4) */
	normal(sp->retry);
	sctp_send_msg(sp, sd, sp->retry);
      done:
	return;
      noroute:
	sctplogte(sp, "no route");
	goto done;
      timedout:
	sctplogto(sp, "%s() association timed out", __FUNCTION__);
	goto done;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_asconf_timeout(void *arg)
{
	struct sctp *sp = (typeof(sp)) arg;

	if (xchg(&sp->timer_asconf, 0) != (toid_t) 0)
		sp_timeout(sp, &__sctp_asconf_timeout, SCTP_TBIT_ASCONF);
}
#endif				/* SCTP_CONFIG_ADD_IP */

#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
/*
 *  LIFETIME TIMEOUT
 *  -------------------------------------------------------------------------
 */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
STATIC INLINE void sctp_send_forward_tsn(struct sctp *sp);
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
STATIC fastcall void
__sctp_life_timeout(struct sctp *sp)
{
	mblk_t *mp, *mp_next;
	unsigned long expires = -1;
	pl_t pl;

	sctplogto(sp, "LIFETIME Timeout");
	if ((1 << sp->state) & ~(SCTPF_SENDING))
		goto outstate;
	pl = bufq_lock(&sp->sndq);
	mp_next = bufq_head(&sp->sndq);
	while ((mp = mp_next)) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		mp_next = mp->b_next;
		if (!cb->expires)
			continue;
		if (cb->expires > jiffies) {
			if (expires > cb->expires)
				expires = cb->expires;
			continue;
		}
		cb->flags |= (SCTPCB_FLAG_DROPPED | SCTPCB_FLAG_NACK);
		cb->when = jiffies;
		cb->next = NULL;
		__bufq_unlink(&sp->sndq, mp);
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG)
			bufq_queue(&sp->ackq, mp);
		else
			freemsg(mp);
	}
	bufq_unlock(&sp->sndq, pl);
	pl = bufq_lock(&sp->urgq);
	mp_next = bufq_head(&sp->urgq);
	while ((mp = mp_next)) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		mp_next = mp->b_next;
		if (!cb->expires)
			continue;
		if (cb->expires > jiffies) {
			if (expires > cb->expires)
				expires = cb->expires;
			continue;
		}
		cb->flags |= (SCTPCB_FLAG_DROPPED | SCTPCB_FLAG_NACK);
		cb->when = jiffies;
		cb->next = NULL;
		__bufq_unlink(&sp->urgq, mp);
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG)
			bufq_queue(&sp->ackq, mp);
		else
			freemsg(mp);
	}
	bufq_unlock(&sp->urgq, pl);
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	/* need to walk through retransmit queue as well */
	if (!(sp->p_caps & SCTP_CAPS_PR))
		goto nocaps;
	pl = bufq_lock(&sp->rtxq);
	for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		if ((cb->flags & SCTPCB_FLAG_DROPPED))
			goto push;
		if (!cb->expires)
			continue;
		if (cb->expires > jiffies) {
			if (expires > cb->expires)
				expires = cb->expires;
			continue;
		}
		/* PR-SCTP 3.5 A2 */
		if (cb->flags & SCTPCB_FLAG_RETRANS) {
			cb->flags &= ~SCTPCB_FLAG_RETRANS;
			ensure(sp->nrtxs > 0, sp->nrtxs = 1);
			sp->nrtxs--;
		} else {
			struct sctp_daddr *sd = cb->daddr;
			size_t dlen = cb->dlen;

			if (sd) {
				/* credit destination (now) */
				sd->in_flight = (sd->in_flight > dlen) ? sd->in_flight - dlen : 0;
				if (!sd->in_flight) {
					sd_timer_cancel_retrans(sd);
#ifdef SCTP_CONFIG_ADD_IP
					if (sd->flags & SCTP_DESTF_UNUSABLE)
						sctp_del_daddr(sd);
#endif				/* SCTP_CONFIG_ADD_IP */
				}
			}
			/* credit association (now) */
			normal(sp->in_flight >= dlen);
			sp->in_flight = (sp->in_flight > dlen) ? sp->in_flight - dlen : 0;
		}
		cb->flags |= SCTPCB_FLAG_DROPPED;
	      push:
		/* push the forward ack point */
		if (cb->tsn == sp->l_fsn + 1) {
			sp->l_fsn++;
		}
	}
	bufq_unlock(&sp->rtxq, pl);
	if (expires != -1)
		sp_timer_life(sp, expires - jiffies);
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sp);
		sp_timer_cond_sack(sp, sp->max_sack);
		sctp_transmit_wakeup(sp);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_timer_cancel_sack(sp);
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
      done:
	return;
      nocaps:
	sctplogte(sp, "no capabilities");
	goto done;
      outstate:
	sctplogte(sp, "%s() timeout in incorrect state", __FUNCTION__);
	goto done;
}
STATIC streamscall void
sctp_life_timeout(void *arg)
{
	struct sctp *sp = (typeof(sp)) arg;

	if (xchg(&sp->timer_life, 0) != (toid_t) 0)
		sp_timeout(sp, &__sctp_life_timeout, SCTP_TBIT_LIFE);
}
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */

/**
 * sctp_send_data: - send data
 * @sp: private structure
 * @st: stream pointer
 * @flags: send flags
 * @dp: user data
 *
 * This function slaps a chunk header onto the M_DATA blocks which form the data and places it onto
 * the stream's write queue.  The message blocks input to this function already have a chunk control
 * block prepended.  (Under sockets, this function is replaced by the sockets sendmsg function.)
 *
 * This function returns zero (0) on success or a negative error number usable by m_error_reply().
 */
STATIC int
sctp_send_data(struct sctp *sp, struct sctp_strm *st, t_uscalar_t flags, mblk_t *dp)
{
	uint *more;
	mblk_t **head;
	uint32_t ppi;
	size_t mlen, dlen;
	bufq_t *sndq;
	t_uscalar_t dflags = flags;
	uint urg = (dflags & SCTPCB_FLAG_URG);

	assert(sp);
	assert(st);
	assert(dp);
	if (urg) {
		sndq = &sp->urgq;
		more = &st->x.more;
		head = &st->x.head;
		ppi = st->x.ppi;
	} else {
		sndq = &sp->sndq;
		more = &st->n.more;
		head = &st->n.head;
		ppi = st->n.ppi;
	}
	for (mlen = msgdsize(dp); mlen; mlen -= dlen, dflags &= ~SCTPCB_FLAG_FIRST_FRAG) {
		mblk_t *bp;
		struct sctp_daddr *sd;
		size_t awnd, plen, amps, used, swnd;

		if (!(sd = sctp_route_normal(sp))) {
			sctplogda(sp, "EHOSTUNREACH: error path taken");
			return (-EHOSTUNREACH);
		}
		/* If there is not enough room in the current send window to handle all or at least 
		   1/2 MTU of the data and the current send backlog then return (-EBUSY) and put
		   the message back on the queue so that backpressure will result.  We only do this 
		   separately for normal data and urgent data (urgent data will be expedited ahead
		   of even retransmissions). */
		plen = PADC(sizeof(struct sctp_data) + mlen);
		amps = sp->amps;
		used = sndq->q_count;
		swnd = sctp_avail(sp, sd);
		awnd = (swnd > used) ? swnd - used : 0;
		if (plen > awnd || plen > amps) {
			if (plen <= amps || awnd < amps >> 1) {	/* SWS avoidance */
				sctplogda(sp,
					  "EBUSY: plen = %u, amps = %u, awnd = %u, swnd = %u, used = %u",
					  (uint) plen, (uint) amps, (uint) awnd, (uint) swnd,
					  (uint) used);
				sctplogda(sp,
					  "EBUSY: sd->cwnd = %u, sd->in_flight = %u, sp->p_rwnd = %u, sp->in_flight = %u",
					  (uint) sd->cwnd, (uint) sd->in_flight, (uint) sp->p_rwnd,
					  (uint) sp->in_flight);
				goto ebusy;
			}
			if (!(bp = sctp_dupmsg(sp, dp)))
				goto enobufs;
			dlen = (awnd < amps) ? amps >> 1 : amps;
			__ensure(dlen > sizeof(struct sctp_data), freemsg(bp); return (-EFAULT));
			dlen -= sizeof(struct sctp_data);
			__ensure(dlen < mlen, freemsg(bp); return (-EFAULT));
#if 1
			{
				int ret;

				ret = trimhead(dp, dlen);	/* trim original */
				__unless(ret, freemsg(bp); return (-EFAULT));
				ret = trimtail(bp, dlen);	/* trim fragment */
				__unless(ret, freemsg(bp); return (-EFAULT));
			}
#else
			fixme(("Should consider multiple mblks\n"));
			dp->b_rptr = dp->b_rptr + dlen;	/* trim original */
			bp->b_wptr = bp->b_rptr + dlen;	/* trim fragment */
#endif
			dflags &= ~SCTPCB_FLAG_LAST_FRAG;
			SCTP_INC_STATS_USER(SctpFragUsrMsgs);
		} else {
			bp = dp;
			dlen = mlen;	/* use entire */
			dflags |= flags & SCTPCB_FLAG_LAST_FRAG;
			/* If we have an existing SDU being built that hasn't been transmitted yet, 
			   we just tack data onto it.  We concatenate only to an MTU. */
			if (*more && *head && plen + SCTP_TCB(*head)->dlen <= amps) {
				struct sctp_data *m;
				sctp_tcb_t *cb;
				pl_t pl;

				pl = bufq_lock(sndq);
				cb = SCTP_TCB(*head);
				cb->flags |= (dflags & 0x7);
				cb->dlen += dlen;
				sndq->q_count += dlen;
				m = (struct sctp_data *) (*head)->b_rptr;
				m->ch.flags = cb->flags & 0x7;
				m->ch.len = htons(sizeof(*m) + cb->dlen);
				linkb(*head, bp);
				normal(cb->dlen == msgdsize((*head)->b_cont));
				if (dflags & SCTPCB_FLAG_LAST_FRAG) {
					*head = NULL;
					*more = 0;
				}
				bufq_unlock(sndq, pl);
				return (0);
			}
		}
		{
			mblk_t *mp;
			struct sctp_data *m;
			sctp_tcb_t *cb;

			if (!(mp = sctp_alloc_chk(sp, sizeof(*m), dlen)))
				goto enobufs;
			sctplogda(sp, "preparing data chunk for transmission");
			cb = SCTP_TCB(mp);
			cb->flags = dflags;
			cb->st = st;
			cb->when = jiffies;
			cb->daddr = NULL;	/* set when transmitted */
			cb->ppi = ppi;
			cb->sid = st->sid;
			cb->ssn = urg ? 0 : (st->ssn + 1) & 0xffff;
			m = (struct sctp_data *) mp->b_wptr;
			m->ch.type = SCTP_CTYPE_DATA;
			m->ch.flags = dflags;
			m->ch.len = htons(sizeof(*m) + dlen);
			m->tsn = 0;	/* assign before transmission */
			m->sid = htons(cb->sid);
			m->ssn = htons(cb->ssn);
			m->ppi = htonl(cb->ppi);
			mp->b_wptr += sizeof(*m);
			mp->b_cont = bp;
			normal(cb->dlen == msgdsize(mp->b_cont));
			/* Remember where we can add more data in case data completing a SDU comes
			   before we are forced to bundle the DATA. */
			*more = (dflags & SCTPCB_FLAG_LAST_FRAG) ? 0 : 1;
			*head = (dflags & SCTPCB_FLAG_LAST_FRAG) ? NULL : mp;
			if (!urg && (dflags & SCTPCB_FLAG_LAST_FRAG))
				st->ssn = cb->ssn;
			sctplogda(sp, "queueing data chunk");
			bufq_queue(sndq, mp);
		}
	}
	sctp_transmit_wakeup(sp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogda(sp, "flow controlled");
	return (-EBUSY);
}

/*
 *  SEND SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE void
sctp_send_sack(struct sctp *sp)
{
	sp->sackf |= SCTP_SACKF_NOD;
	sctplogda(sp, "marking SACK");
	sctp_transmit_wakeup(sp);
}

/*
 *  SEND ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We just queue the error, we don't send it out...  It gets bundled with other things.  This also
 *  provides for SCTP IG 2.27.
 */
STATIC int
sctp_send_error(struct sctp *sp, uint ecode, caddr_t eptr, size_t elen)
{
	mblk_t *mp;
	struct sctp_error *m;
	struct sctpehdr *eh;
	size_t clen = sizeof(*m) + sizeof(*eh) + elen;
	size_t plen = PADC(clen);

	if (!(mp = sctp_alloc_chk(sp, clen, plen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ERROR;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) (m + 1);
	eh->code = htons(ecode);
	eh->len = htons(sizeof(*eh) + elen);
	bcopy(eptr, (eh + 1), elen);
	mp->b_wptr += plen;
	bufq_queue(&sp->errq, mp);
	sctp_transmit_wakeup(sp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  SEND INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the INIT and get timers started, we must return an error to the user
 *  interface calling this function.
 */
STATIC int
sctp_send_init(struct sctp *sp)
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_saddr *ss;
	struct sctp_init *m;
	struct sctp_addr_type *at;
	struct sctp_ipv4_addr *ap;
	struct sctp_cookie_psrv *cp;

#ifdef SCTP_CONFIG_ECN
	struct sctp_ecn_capable *ec;
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	struct sctp_ali *ai;
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	struct sctp_pr_sctp *pr;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	size_t sanum = sp->sanum;
	size_t clen = sizeof(*m)
	    + (sanum * PADC(sizeof(*ap)))
	    + (sp->ck_inc ? PADC(sizeof(*cp)) : 0)
	    + (sizeof(*at) + sizeof(at->type[0]))
#ifdef SCTP_CONFIG_ECN
	    + ((sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) ? PADC(sizeof(*ec)) : 0)
#endif					/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	    + ((sp->l_caps & SCTP_CAPS_ALI) ? PADC(sizeof(*ai)) : 0)
#endif					/* defined(SCTP_CONFIG_ADD_IP) ||
					   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    + ((sp->l_caps & SCTP_CAPS_PR) ? PADC(sizeof(*pr)) : 0)
#endif					/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	;

	if (!(sd = sp->daddr))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_INIT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = sp->v_tag;
	m->a_rwnd = htonl(sp->a_rwnd);
	m->n_istr = htons(sp->max_istr);
	m->n_ostr = htons(sp->req_ostr);
	m->i_tsn = htonl(sp->v_tag);
	mp->b_wptr += sizeof(*m);
	for (ss = sp->saddr; ss && sanum; ss = ss->next, sanum--) {
		ap = (typeof(ap)) mp->b_wptr;
		ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
		ap->ph.len = htons(sizeof(*ap));
		ap->addr = ss->saddr;
		mp->b_wptr += PADC(sizeof(*ap));
	}
	unusual(ss);
	unusual(sanum);
	if (sp->ck_inc) {
		cp = (typeof(cp)) mp->b_wptr;
		cp->ph.type = SCTP_PTYPE_COOKIE_PSRV;
		cp->ph.len = htons(sizeof(*cp));;
		cp->ck_inc = htonl(sp->ck_inc);
		mp->b_wptr += PADC(sizeof(*cp));
	}
	at = (typeof(at)) mp->b_wptr;
	at->ph.type = SCTP_PTYPE_ADDR_TYPE;
	at->ph.len = htons(sizeof(*at) + sizeof(at->type[0]));
	at->type[0] = SCTP_PTYPE_IPV4_ADDR;
	mp->b_wptr += PADC(sizeof(*at) + sizeof(at->type[0]));
#ifdef SCTP_CONFIG_ECN
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
		ec = (typeof(ec)) mp->b_wptr;
		ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
		ec->ph.len = __constant_htons(sizeof(*ec));
		mp->b_wptr += PADC(sizeof(*ec));
	}
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) mp->b_wptr;
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htons(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
		mp->b_wptr += PADC(sizeof(*ai));
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) mp->b_wptr;
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
		mp->b_wptr += PADC(sizeof(*pr));
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sp_timer_init(sp, sd->rto);
	abnormal(sp->retry);
	sctp_change_state(sp, SCTP_COOKIE_WAIT);
	sctplogtx(sp, "sending INIT");
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
	return (0);
      enobufs:
	// ptrace(("ERROR: no buffers\n"));
	return (-ENOBUFS);
      noroute:
	sctplogerr(sp, "%s() no route", __FUNCTION__);
	return (-EFAULT);
}

/*
 *  SEND INIT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  No state change results from replying to an INIT.  INIT ACKs are sent without a TCB but a socket
 *  or stream is referenced.  INIT ACK chunks cannot have any other chunks bundled with them. (RFC
 *  2960 6.10).
 */
STATIC void
sctp_send_init_ack(struct sctp *sp, uint32_t saddr, uint32_t daddr, struct sctphdr *sh,
		   struct sctp_cookie *ck, mblk_t *unrec)
{
	mblk_t *mp;
	struct sctp_saddr *ss;
	struct sctphdr *rh;
	struct sctp_init_ack *m;
	struct sctpphdr *ph;

#ifdef SCTP_CONFIG_ECN
	struct sctp_ecn_capable *ec;
#endif				/* SCTP_CONFIG_ECN */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	struct sctp_pr_sctp *pr;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	struct sctp_ali *ai;
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
	struct sctp_unrec_parms *up;
	struct sctp_cookie *cp;
	struct sctp_ipv4_addr *ap;
	struct sctp_init *im = (struct sctp_init *) (sh + 1);
	unsigned char *init = (unsigned char *) im;
	int anum = ck->danum;
	int snum = ck->sanum;
	size_t klen = sizeof(*ph) + raw_cookie_size(ck) + HMAC_SIZE;
	size_t dlen = sp->sanum * PADC(sizeof(*ap));
	size_t ulen = (ulen = msgdsize(unrec)) ? PADC(sizeof(*up) + ulen) : 0;
	size_t clen = sizeof(*m) + dlen + klen + ulen
#ifdef SCTP_CONFIG_ECN
	    + PADC(sizeof(*ec))
#endif					/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	    + ((sp->l_caps & SCTP_CAPS_ALI) ? PADC(sizeof(*ai)) : 0)
#endif					/* defined(SCTP_CONFIG_ADD_IP) ||
					   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    + ((sp->l_caps & SCTP_CAPS_PR) ? PADC(sizeof(*pr)) : 0)
#endif					/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	;
	int arem, alen;

	assert(sp);
	if (!(mp = sctp_alloc_reply(sp, sh, clen)))
		goto enobufs;
	rh = ((typeof(rh)) mp->b_wptr) - 1;
	rh->v_tag = im->i_tag;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_INIT_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = ck->v_tag;
	m->a_rwnd = htonl(sp->a_rwnd);
	m->n_istr = htons(ck->n_istr);
	m->n_ostr = htons(ck->n_ostr);
	m->i_tsn = htonl(ck->v_tag);
	mp->b_wptr += sizeof(*m);
	for (ss = sp->saddr; ss; ss = ss->next) {
		ap = (struct sctp_ipv4_addr *) mp->b_wptr;
		ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
		ap->ph.len = __constant_htons(sizeof(*ap));
		ap->addr = ss->saddr;
		mp->b_wptr += PADC(sizeof(*ap));
	}
	ph = (typeof(ph)) mp->b_wptr;
	ph->type = SCTP_PTYPE_STATE_COOKIE;
	ph->len = htons(klen);
	mp->b_wptr += sizeof(*ph);
	cp = (typeof(cp)) mp->b_wptr;
	bcopy(ck, cp, sizeof(*cp));
	mp->b_wptr += sizeof(*cp);
#if 0
	/* copy in IP reply options */
	if (ck->opt_len) {
		assure(opt);
		bcopy(opt, mp->b_wptr, optlength(opt));
		kfree_s(opt, optlength(opt));
		sp->opt = (struct ip_options *) mp->b_wptr;
		mp->b_wptr += ck->opt_len;
	}
#endif
	for (ap = (typeof(ap)) (init + sizeof(struct sctp_init)), arem =
	     PADC(htons(((struct sctpchdr *) init)->len)) - sizeof(struct sctp_init);
	     anum && arem >= sizeof(struct sctpphdr);
	     arem -= PADC(alen), ap = (typeof(ap)) (((uint8_t *) ap) + PADC(alen))) {
		if ((alen = ntohs(ap->ph.len)) > arem) {
			assure(alen <= arem);
			sctplogerr(sp, "alen = %d, arem = %d", (int) alen, (int) arem);
			freemsg(mp);
			rare();
			return;
		}
		if (ap->ph.type == SCTP_PTYPE_IPV4_ADDR) {
			/* skip primary */
			if (ap->addr != ck->daddr) {
				*(uint32_t *) mp->b_wptr = ap->addr;
				mp->b_wptr += sizeof(uint32_t);
				anum--;
			}
		}
	}
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (ss->saddr != ck->saddr) {
			*(uint32_t *) mp->b_wptr = ss->saddr;
			mp->b_wptr += sizeof(uint32_t);
			snum--;
		}
	}
	assure(!anum);
	assure(!snum);
	sctp_sign_cookie(sp, cp);
	mp->b_wptr += HMAC_SIZE;
#ifdef SCTP_CONFIG_ECN
	ec = (typeof(ec)) mp->b_wptr;
	ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
	ec->ph.len = __constant_htons(sizeof(*ec));
	mp->b_wptr += PADC(sizeof(*ec));
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) mp->b_wptr;
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htons(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
		mp->b_wptr += PADC(sizeof(*ai));
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) mp->b_wptr;
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
		mp->b_wptr += PADC(sizeof(*pr));
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	/* SCTP IG 2.27 add unrecognized parameters parameter */
	if (ulen) {
		size_t blen = unrec->b_wptr - unrec->b_rptr;

		up = (typeof(up)) mp->b_wptr;
		up->ph.type = SCTP_PTYPE_UNREC_PARMS;
		up->ph.len = htons(ulen);
		mp->b_wptr += sizeof(*up);
		bcopy(unrec->b_rptr, mp->b_wptr, blen);
		mp->b_wptr += (PADC(blen));
	}
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending INIT-ACK");
	sctp_xmit_msg(saddr, daddr, mp, sp);
	return;
      enobufs:
	return;
}

/*
 *  SEND COOKIE ECHO
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the COOKIE ECHO and get timers started, we must return an error to the user
 *  interface calling this function.
 */
STATIC int
sctp_send_cookie_echo(struct sctp *sp, caddr_t kptr, size_t klen)
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_cookie_echo *m;
	size_t clen = sizeof(*m) + klen;

	assert(sp);
	if (!(sd = sctp_route_normal(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_cookie_echo *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_COOKIE_ECHO;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	bcopy(kptr, (m + 1), klen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctp_change_state(sp, SCTP_COOKIE_ECHOED);
	sp_timer_cookie(sp, sd->rto);
	sctplogtx(sp, "sending COOKIE-ECHO");
	sctp_bundle_more(sp, sd, mp, 1);	/* don't nagle */
	sctp_send_msg(sp, sd, mp);
	abnormal(sp->retry);
	freechunks(xchg(&sp->retry, mp));
	return (0);
      enobufs:
	return (-ENOBUFS);
      noroute:
	sctplogerr(sp, "%s() no route", __FUNCTION__);
	return (-EFAULT);
}

/*
 *  SEND COOKIE ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  SACK and DATA can be bundled with the COOKIE ACK.
 */
STATIC int sctp_return_more(mblk_t *mp);
STATIC void
sctp_send_cookie_ack(struct sctp *sp)
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_cookie_ack *m;
	size_t clen = sizeof(*m);

	assert(sp);
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	sctp_change_state(sp, SCTP_ESTABLISHED);
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_cookie_ack *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_COOKIE_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending COOKIE-ACK");
	sctp_bundle_more(sp, sd, mp, 1);	/* don't nagle */
	sctp_send_msg(sp, sd, mp);
	goto done;
      done:
	freechunks(mp);
      enobufs:
	/* start idle timers */
	for (sd = sp->daddr; sd; sd = sd->next)
		sctp_reset_idle(sd);
      noroute:
	return;
}

/*
 *  SEND HEARTBEAT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We don't send heartbeats when idle timers expire if we are in the wrong state, we just reset the
 *  idle timer.
 *
 *  TODO: SCTP IG 2.36  We need to generate initiate heartbeats and generate a nonce to all
 *  unverified destinations.
 */
STATIC void
sctp_send_heartbeat(struct sctp *sp, struct sctp_daddr *sd)
{
	mblk_t *mp;
	struct sctp_heartbeat *m;
	struct sctp_heartbeat_info *h;
	size_t fill, clen, hlen, plen;

	assert(sp);
	ensure(sd, return);
	fill = sd->hb_fill;
	hlen = sizeof(*h) + fill;
	clen = sizeof(*m) + hlen;
	plen = PADC(clen);
	sd->hb_time = jiffies;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_HEARTBEAT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	h = (typeof(h)) (m + 1);
	h->ph.type = SCTP_PTYPE_HEARTBEAT_INFO;
	h->ph.len = htons(hlen);
	h->hb_info.timestamp = sd->hb_time;
	h->hb_info.daddr = sd->daddr;
	h->hb_info.mtu = sd->mtu;
	bzero(h->hb_info.fill, fill);
	mp->b_wptr += plen;
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending HEARTBEAT");
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
      enobufs:
	sd_timer_heartbeat(sd, sd->rto);
}

/*
 *  SEND HEARTBEAT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Take the incoming HEARTBEAT message and turn it back around as a HEARTBEAT ACK message.  Note
 *  that if the incoming chunk parameters are invalid, so are the outgoing parameters, this is
 *  because the hb_info parameter is opaque to us.  This is consistent with
 *  draft-stewart-ong-sctpbakeoff-sigtran-01.
 */
STATIC void
sctp_send_heartbeat_ack(struct sctp *sp, caddr_t hptr, size_t hlen)
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_heartbeat_ack *m;
	size_t clen = sizeof(*m) + hlen;

	assert(sp);
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_heartbeat_ack *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_HEARTBEAT_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	bcopy(hptr, (m + 1), hlen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending HEARTBEAT-ACK");
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
	return;
      noroute:
	sctplogtx(sp, "%s(): no route", __FUNCTION__);
      enobufs:
	return;
}

/*
 *  SEND ABORT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  There is no point in bundling control chunks after an ABORT chunk.  Also, DATA chunks are not to
 *  be bundled with ABORT chunks.
 */
STATIC void
sctp_send_abort(struct sctp *sp)
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);

	if ((1 << sp->state) & ~(SCTPF_NEEDABORT))
		goto notneeded;
	if ((1 << sp->state) & SCTPF_CONNECTED)
		sd = sctp_route_normal(sp);
	else
		sd = sp->daddr;
	if (!sd)
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_abort *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending ABORT");
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
      enobufs:
      noroute:
	if ((1 << sp->state) & ~(SCTPF_DISCONNECTED)) {
		if (sp->state != SCTP_CLOSED)
			SCTP_INC_STATS(SctpAborteds);
		sctp_change_state(sp, sp->conind ? SCTP_LISTEN : SCTP_CLOSED);
	}
      notneeded:
	return;
}

/*
 *  SEND ABORT (w/ERROR CAUSE)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Errors (beyond the error header) must be formatted by the called and indicated by are and len.
 *  There is no point in bundling data or control chunks after and abort chunk.
 */
STATIC void
sctp_send_abort_error(struct sctp *sp, int errn, void *aptr,	/* argument ptr */
		      size_t alen /* argument len */ )
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;

	if ((1 << sp->state) & ~(SCTPF_NEEDABORT))
		goto notneeded;
	if (!(sd = sctp_route_normal(sp)))
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_abort *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (struct sctpehdr *) (m + 1);
	eh->code = htons(errn);
	eh->len = htons(elen);
	bcopy(aptr, (eh + 1), alen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending ABORT w/Cause");
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
      enobufs:
      noroute:
	if ((1 << sp->state) & ~(SCTPF_DISCONNECTED)) {
		if (sp->state != SCTP_CLOSED)
			SCTP_INC_STATS(SctpAborteds);
		sctp_change_state(sp, sp->conind ? SCTP_LISTEN : SCTP_CLOSED);
	}
	return;
      noerror:
	sctp_send_abort(sp);
      notneeded:
	return;
}

/*
 *  SEND SHUTDOWN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  RFC 2960 6.2 "... DATA chunks cannot be bundled with SHUTDOWN or SHUTDOWN ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN and get timers started, we must inform the user interface
 *  calling this function.
 */
STATIC int
sctp_send_shutdown(struct sctp *sp)
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_shutdown *m;
	size_t clen = sizeof(*m);

	assert(sp);
#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (bufq_head(&sp->sndq) || bufq_head(&sp->urgq) || bufq_head(&sp->rtxq))
		sctplogerr(sp, "%s() with queued chunks", __FUNCTION__);
#endif				/* defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST */
	if (!(sd = sctp_route_normal(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_shutdown *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	m->c_tsn = htonl(sp->r_ack);
	mp->b_wptr += PADC(clen);
	/* shutdown acks everything but dups and gaps */
	sp->sackf &= (SCTP_SACKF_DUP | SCTP_SACKF_GAP);
	sp_timer_cancel_sack(sp);
#ifdef SCTP_CONFIG_ADD_IP
	sp_timer_cancel_asconf(sp);
#endif				/* SCTP_CONFIG_ADD_IP */
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sp_timer_shutdown(sp, sd->rto);
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sp->state != SCTP_SHUTDOWN_SENT)
		sp_timer_guard(sp, 5 * sd->rto);
	sctp_change_state(sp, SCTP_SHUTDOWN_SENT);
	sctplogtx(sp, "sending SHUTDOWN");
	sctp_bundle_more(sp, sd, mp, 1);	/* not DATA */
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
	return (0);
      enobufs:
	return (-ENOBUFS);
      noroute:
	sctplogerr(sp, "%s() no route", __FUNCTION__);
	return (-EFAULT);
}

/*
 *  SEND SHUTDOWN ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  SHUTDOWN ACK is sent in response to a SHUTDOWN message after all data has cleared or in reponse
 *  to a COOKIE ECHO during the SHUTDOWN_ACK_SENT state.  If the error flag is set, we want to
 *  bundle and ERROR chunk with the SHUTDOWN ACK indicating "cookie received while shutting down."
 *
 *  RFC 2960 6.2. "...  DATA chunks cannot be bundled with SHUTDOWN or SHUTDOWN ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN ACK and get timers started, we must return an error to the
 *  user interface calling this function.
 */
STATIC int
sctp_send_shutdown_ack(struct sctp *sp)
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_shutdown_ack *m;
	size_t clen = sizeof(*m);

	assert(sp);
#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (bufq_head(&sp->sndq) || bufq_head(&sp->urgq) || bufq_head(&sp->rtxq))
		sctplogerr(sp, "%s() with queued chunks", __FUNCTION__);
#endif				/* defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST */
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_shutdown_ack *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sp_timer_cancel_sack(sp);
#ifdef SCTP_CONFIG_ADD_IP
	sp_timer_cancel_asconf(sp);
#endif				/* SCTP_CONFIG_ADD_IP */
	sp_timer_shutdown(sp, sd->rto);
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sp->state != SCTP_SHUTDOWN_ACK_SENT)
		sp_timer_guard(sp, 5 * sd->rto);
	sctp_change_state(sp, SCTP_SHUTDOWN_ACK_SENT);
	sctplogtx(sp, "sending SHUTDOWN-ACK");
	sctp_bundle_more(sp, sd, mp, 1);	/* not DATA */
	sctp_send_msg(sp, sd, mp);
	abnormal(sp->retry);
	freechunks(xchg(&sp->retry, mp));
	return (0);
      enobufs:
	return (-ENOBUFS);
      noroute:
	sctplogerr(sp, "%s() no route", __FUNCTION__);
	return (-EFAULT);
}

/*
 *  SEND SHUTDOWN COMPLETE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_shutdown_complete(struct sctp *sp)
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);

#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (bufq_size(&sp->sndq)
	    || bufq_size(&sp->urgq)
	    || bufq_size(&sp->rtxq))
		sctplogerr(sp, "%s() with queued chunks", __FUNCTION__);
#endif				/* defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST */
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending SHUTDOWN-COMPLETE");
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
	return;
      enobufs:
      noroute:
	sctp_change_state(sp, sp->conind ? SCTP_LISTEN : SCTP_CLOSED);
	return;
}

#ifdef SCTP_CONFIG_ADD_IP
/*
 *  ABORT ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_abort_asconf(struct sctp *sp)
{
	struct sctp_saddr *ss;

	for (ss = sp->saddr; ss; ss = ss->next) {
		if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
			ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
			ss->flags |= SCTP_SRCEF_ADD_REQUEST;
			ss->flags |= SCTP_SACKF_ASC;
		}
		if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
			ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
			ss->flags |= SCTP_SRCEF_DEL_REQUEST;
			ss->flags |= SCTP_SACKF_ASC;
		}
		if (ss->flags & SCTP_SRCEF_SET_PENDING) {
			ss->flags &= ~SCTP_SRCEF_SET_PENDING;
			ss->flags |= SCTP_SRCEF_SET_REQUEST;
			ss->flags |= SCTP_SACKF_ASC;
		}
	}
	if (sp->state == SCTP_ESTABLISHED) {
		sp_timer_cancel_asconf(sp);
		freechunks(xchg(&sp->retry, NULL));
	}
	return;
}

/*
 *  SEND ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_asconf(struct sctp *sp)
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_asconf *m;
	struct sctp_add_ip *a;
	struct sctp_del_ip *d;
	struct sctp_set_ip *s;
	struct sctp_ipv4_addr *p;
	size_t clen = sizeof(*m);
	struct sctp_saddr *ss;
	int requested = 0;

	if (sp->state != SCTP_ESTABLISHED)
		goto skip;
	if (!(sp->p_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto skip;
	/* ADD-IP 4.1 (A3) */
	if (sp->reply || sctp_timeout_pending(&sp->timer_asconf))
		goto skip;
	ensure(sp->sackf & SCTP_SACKF_ASC, goto skip);
	if (!(sd = sctp_route_normal(sp)))
		goto noroute;
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (sp->p_caps & SCTP_CAPS_ADD_IP) {
			if (ss->flags & SCTP_SRCEF_ADD_REQUEST) {
				clen += sizeof(*a) + sizeof(*p);
				requested++;
			}
			if (ss->flags & SCTP_SRCEF_DEL_REQUEST) {
				clen += sizeof(*d) + sizeof(*p);
				requested++;
			}
		}
		if (sp->p_caps & SCTP_CAPS_SET_IP) {
			if (ss->flags & SCTP_SRCEF_SET_REQUEST) {
				clen += sizeof(*s) + sizeof(*p);
				requested++;
			}
		}
	}
	if (!requested)
		goto skip;
	/* ADD-IP 4.1 (A1) */
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ASCONF;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	/* ADD-IP 4.1 (A2) */
	m->asn = htonl(++sp->l_asn);
	mp->b_wptr += sizeof(*m);
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (sp->p_caps & SCTP_CAPS_ADD_IP) {
			if (ss->flags & SCTP_SRCEF_ADD_REQUEST) {
				a = (typeof(a)) mp->b_wptr;
				a->ph.type = SCTP_PTYPE_ADD_IP;
				a->ph.len = __constant_htons(sizeof(*a) + sizeof(*p));
				a->id = htonl((uint32_t) (long) ss);
				mp->b_wptr += sizeof(*a);
				p = (typeof(p)) mp->b_wptr;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				mp->b_wptr += sizeof(*p);
				ss->flags &= ~SCTP_SRCEF_ADD_REQUEST;
				ss->flags |= SCTP_SRCEF_ADD_PENDING;
			}
			if (ss->flags & SCTP_SRCEF_DEL_REQUEST) {
				d = (typeof(d)) mp->b_wptr;
				d->ph.type = SCTP_PTYPE_DEL_IP;
				d->ph.len = __constant_htons(sizeof(*d) + sizeof(*p));
				d->id = htonl((uint32_t) (long) ss);
				mp->b_wptr += sizeof(*d);
				p = (typeof(p)) mp->b_wptr;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				mp->b_wptr += sizeof(*p);
				ss->flags &= ~SCTP_SRCEF_DEL_REQUEST;
				ss->flags |= SCTP_SRCEF_DEL_PENDING;
			}
		}
		if (sp->p_caps & SCTP_CAPS_SET_IP) {
			if (ss->flags & SCTP_SRCEF_SET_REQUEST) {
				s = (typeof(s)) mp->b_wptr;
				s->ph.type = SCTP_PTYPE_SET_IP;
				s->ph.len = __constant_htons(sizeof(*s) + sizeof(*p));
				s->id = htonl((uint32_t) (long) ss);
				mp->b_wptr += sizeof(*s);
				p = (typeof(p)) mp->b_wptr;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				mp->b_wptr += sizeof(*p);
				ss->flags &= ~SCTP_SRCEF_SET_REQUEST;
				ss->flags |= SCTP_SRCEF_SET_PENDING;
			}
		}
	}
	sp->sackf &= ~SCTP_SACKF_ASC;
	SCTP_INC_STATS(SctpOutCtrlChunks);
	/* ADD-IP 4.1 (A4) */
	sp_timer_asconf(sp, sd->rto);
	sctplogtx(sp, "sending ASCONF");
	sctp_bundle_more(sp, sd, mp, 1);
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
      skip:
	return;
      enobufs:
	return;
      noroute:
	sctplogerr(sp, "%s() no route", __FUNCTION__);
	return;
}

/*
 *  SEND ASCONF ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_asconf_ack(struct sctp *sp, caddr_t rptr, size_t rlen)
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_asconf_ack *m;
	size_t clen = sizeof(*m) + rlen;

	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ASCONF_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->asn = ntohl(sp->p_asn++);
	bcopy(rptr, m + 1, rlen);	/* copy in response TLVs */
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending ASCONF-ACK");
	sctp_bundle_more(sp, sd, mp, 1);
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->reply, mp));
	return;
      enobufs:
	return;
      noroute:
	sctplogerr(sp, "%s() no route", __FUNCTION__);
	return;
}
#endif				/* SCTP_CONFIG_ADD_IP */

/*
 *  SENDING WITHOUT TCB  (Responding to OOTB packets)
 *  -------------------------------------------------------------------------
 *  When sending without an SCTP TCB, we only have the IP header and the SCTP header from which to
 *  work.  We have no associated socket or stream.  These are usually used for replying to OOTB
 *  messages.
 *
 *  SEND ABORT (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_abort_ootb(uint32_t daddr, uint32_t saddr, struct sctphdr *sh)
{
	mblk_t *mp;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);

	if (!sh)
		goto noroute;
	if (!(mp = sctp_alloc_reply(NULL, sh, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	strlog(CONFIG_STREAMS_SCTP_T_MAJOR, 0, SCTPLOGTX, SL_TRACE, "sending ABORT (OOTB)");
	sctp_xmit_ootb(daddr, saddr, mp);
	return;
      noroute:
	ptrace(("no route"));
	return;
      enobufs:
	return;
}

/*
 *  SEND ABORT (w/ERROR CAUSE) (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_abort_error_ootb(uint32_t daddr,	/* dest address */
			   uint32_t saddr,	/* srce address */
			   struct sctphdr *sh,	/* SCTP header */
			   int errn,	/* error number */
			   void *aptr,	/* argument ptr */
			   size_t alen /* argument len */ )
{
	mblk_t *mp;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;

	if (!sh)
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(mp = sctp_alloc_reply(NULL, sh, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) (m + 1);
	eh->code = htons(errn);
	eh->len = htons(elen);
	bcopy(aptr, (eh + 1), alen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	strlog(CONFIG_STREAMS_SCTP_T_MAJOR, 0, SCTPLOGTX, SL_TRACE, "sending ABORT w/Cause (OOTB)");
	sctp_xmit_ootb(daddr, saddr, mp);
	return;
      noerror:
	sctp_send_abort_ootb(daddr, saddr, sh);
	return;
      noroute:
	ptrace(("no route"));
	return;
      enobufs:
	return;
}

/*
 *  SENDING With TCB  (Responding to INIT packets)
 *  -------------------------------------------------------------------------
 *  When sending without an SCTP TCB, we only have the IP header and the SCTP header from which to
 *  work.  We only have a listening socket.  These are usually used for replying to INIT messages.
 *
 *  SEND ABORT (With TCB) Responding to INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void
sctp_send_abort_init(struct sctp *sp, uint32_t daddr, uint32_t saddr, struct sctphdr *sh,
		     uint32_t i_tag)
{
	mblk_t *mp;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);

	if (!sh)
		goto noroute;
	if (!(mp = sctp_alloc_reply(sp, sh, clen)))
		goto enobufs;
	(((struct sctphdr *) mp->b_wptr) - 1)->v_tag = i_tag;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending ABORT (INIT)");
	sctp_xmit_msg(saddr, daddr, mp, sp);
	return;
      noroute:
      enobufs:
	return;
}

/*
 *  SEND ABORT (w/ERROR CAUSE) (With TCB) Responding to INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_abort_error_init(struct sctp *sp, uint32_t daddr, uint32_t saddr, struct sctphdr *sh,
			   uint32_t i_tag, int errn, caddr_t aptr, size_t alen)
{
	mblk_t *mp;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;

	if (!sh)
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(mp = sctp_alloc_reply(sp, sh, clen)))
		goto enobufs;
	(((struct sctphdr *) mp->b_wptr) - 1)->v_tag = i_tag;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) (m + 1);
	eh->code = htons(errn);
	eh->len = htons(elen);
	bcopy(aptr, (eh + 1), alen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctplogtx(sp, "sending ABORT w/Cause (INIT)");
	sctp_xmit_msg(saddr, daddr, mp, sp);
	return;
      noerror:
	sctp_send_abort_init(sp, daddr, saddr, sh, i_tag);
	return;
      noroute:
      enobufs:
	return;
}

/*
 *  SEND SHUTDOWN COMPLETE (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_shutdown_complete_ootb(uint32_t daddr, uint32_t saddr, struct sctphdr *sh)
{
	mblk_t *mp;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);

	if (!sh)
		goto noroute;
	if (!(mp = sctp_alloc_reply(NULL, sh, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += PADC(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	strlog(CONFIG_STREAMS_SCTP_T_MAJOR, 0, SCTPLOGTX, SL_TRACE,
	       "sending SHUTDOWN-COMPLETE (OOTB)");
	sctp_xmit_ootb(daddr, saddr, mp);
      enobufs:
	return;
      noroute:
	ptrace(("no route"));
	return;
}

/*
 *  =========================================================================
 *
 *  SCTP Peer --> SCTP Primitives (Receive Messages)
 *
 *  =========================================================================
 */
/*
 *  RETURN VALUE FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  RETURN VALUE when expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Pull the previous chunk and range check next chunk header.
 */
STATIC int
sctp_return_more(mblk_t *mp)
{
	size_t dlen = (mp->b_wptr > mp->b_rptr) ? mp->b_wptr - mp->b_rptr : 0;
	struct sctpchdr *ch = SCTP_CH(mp);
	size_t clen = ntohs(ch->len);
	size_t plen = PADC(clen);

	if ((ch = (typeof(ch)) (mp->b_rptr += plen))
	    && ((dlen = (mp->b_wptr > mp->b_rptr) ? mp->b_wptr - mp->b_rptr : 0) >= sizeof(*ch))
	    && ((clen = ntohs(ch->len)) >= sizeof(*ch))
	    && ((plen = PADC(clen)) <= dlen))
		return (clen);
	if (mp->b_wptr > mp->b_rptr)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  RETURN VALUE when not expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_stop(mblk_t *mp)
{
	int ret = sctp_return_more(mp) ? -EPROTO : 0;

	unusual(ret < 0);
	return (ret);
}

#if 0
/*
 *  RETURN VALUE when expecting specific chunk or nothing
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_check(mblk_t *mp, uint ctype)
{
	int ret = sctp_return_more(mp);

	ret = (ret > 0 && SCTP_CH(mp)->type != ctype) ? -EPROTO : ret;
	unusual(ret < 0);
	return (ret);
}
#endif

/*
 *  RESET IDLE
 *  -------------------------------------------------------------------------
 *  Reset the idle timer for generation of heartbeats.  Stop any heartbeating
 *  that we might be involved in at the moment.
 */
STATIC void
sctp_reset_idle(struct sctp_daddr *sd)
{
	unsigned long rtt;

	ensure(sd, return);
	sd_timer_cancel_heartbeat(sd);
	rtt = sd->rto + sd->hb_itvl + ((jiffies & 0x1) * (sd->rto >> 1));
	sctplogte(sd->sp, "%p setting idle timer to %d ticks", sd, (int) rtt);
	sd_timer_idle(sd, rtt);
}

/*
 *  RTT CALC
 *  -------------------------------------------------------------------------
 *  Round Trip Time calculations for messages acknowledged on the first transmission.  When a
 *  message is acknowledged, this function peforms and update of the RTT calculation if appropriate.
 *  It is called by sctp_ack_calc for control chunks which expect acknowledgements, and by
 *  sctp_dest_calc when DATA chunks are acknolwedged on first transmission via SACK or SHUTDOWN
 *  chunks, and by sctp_recv_heartbeat_ack when calculating RTTs for HEARTBEAT chunks.
 */
STATIC void
sctp_rtt_calc(struct sctp_daddr *sd, unsigned long time)
{
	unsigned long rtt;

	ensure(sd, return);
	ensure(jiffies >= time, return);
	/* RFC 2960 6.3.1 */
	rtt = jiffies - time;
	if (sd->srtt) {
		unsigned long rttvar;

		/* RFC 2960 6.3.1 (C3) */
		if (rtt > sd->srtt) {
			rttvar = rtt - sd->srtt;
			sd->srtt += (rtt - sd->srtt) >> 3;
		} else {
			rttvar = sd->srtt - rtt;
			sd->srtt += (sd->srtt - rtt) >> 3;
		}
		if (rttvar > sd->rttvar)
			sd->rttvar += (rttvar - sd->rttvar) >> 2;
		else
			sd->rttvar += (sd->rttvar - rttvar) >> 2;
		sd->rto = sd->srtt + (sd->rttvar << 2);
	} else {
		/* RFC 2960 6.3.1 (C2) */
		sd->rttvar = rtt >> 1;
		sd->srtt = rtt;
		sd->rto = rtt + (rtt << 1);
	}
	/* RFC 2960 6.3.1 (G1) */
	sd->rttvar = sd->rttvar ? sd->rttvar : 1;
	/* RFC 2960 6.3.1 (C6) */
	sd->rto = (sd->rto_min > sd->rto) ? sd->rto_min : sd->rto;
	/* RFC 2960 6.3.1 (C7) */
	sd->rto = (sd->rto_max < sd->rto) ? sd->rto_max : sd->rto;
#if (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) && defined SCTP_CONFIG_ERROR_GENERATOR
	if (sd->retransmits && (sd->sp->debug & SCTP_OPTION_BREAK)
	    && (sd->packets > SCTP_CONFIG_BREAK_GENERATOR_LEVEL))
		ptrace(("Aaaarg! Reseting counts for address %d.%d.%d.%d\n",
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff, (sd->daddr >> 16) & 0xff,
			(sd->daddr >> 24) & 0xff));
#endif				/* (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) &&
				   defined SCTP_CONFIG_ERROR_GENERATOR */
	sd->dups = 0;
	/* RFC 2960 8.2 */
	sd->retransmits = 0;
	/* RFC 2960 8.1 */
	sd->sp->retransmits = 0;
	/* reset idle timer */
	sctp_reset_idle(sd);
}

/*
 *  DEST CALC
 *  -------------------------------------------------------------------------
 *  This function performs delayed processing of RTT and CWND calculations on destinations which
 *  need it and is called from sctp_recv_shutdown and sctp_recv_sack once all ack and gap ack
 *  processing is complete.  This performs the necessary calculations for each destination before
 *  closing processing of the received SHUTDOWN or SACK chunk.
 */
STATIC void
sctp_dest_calc(struct sctp *sp)
{
	struct sctp_daddr *sd;

	assert(sp);
	usual(sp->daddr);
	for (sd = sp->daddr; sd; sd = sd->next) {
		size_t accum;

		if (sd->when) {
			/* calculate RTT based on latest sent acked TSN */
			sctp_rtt_calc(sd, sd->when);
			sd->when = 0;
		}
		/* NOTE:- first we grow the congestion window according to whatever TSNs were
		   cummulatively acked to the destination and then we back off if the destination
		   is dropping (as indiciated by gap reports). */
		if ((accum = sd->ack_accum)) {
			if (sd->cwnd <= sd->ssthresh) {
				/* RFC 2960 7.2.1 */
				if (sd->in_flight > sd->cwnd)
					sd->cwnd += (accum < sd->mtu) ? accum : sd->mtu;
			} else {
				/* RFC 2960 7.2.2 */
				sd->partial_ack += accum;
				if (sd->in_flight >= sd->cwnd && sd->partial_ack >= sd->cwnd) {
					sd->cwnd += sd->mtu;
					sd->partial_ack -= sd->cwnd;
				}
			}
			/* credit of destination (accum) */
			normal(sd->in_flight >= accum);
			sd->in_flight = (sd->in_flight > accum) ? sd->in_flight - accum : 0;
			/* RFC 2960 6.3.2 (R3) */
			sd_timer_cancel_retrans(sd);
			sd->ack_accum = 0;
		}
		if (sd->flags & SCTP_DESTF_DROPPING) {
			/* RFC 2960 7.2.4 (2), 7.2.3 */
			sd->ssthresh =
			    ((sd->cwnd >> 1) > (sd->mtu << 1)) ? sd->cwnd >> 1 : sd->mtu << 1;
			sd->cwnd = sd->ssthresh;
			/* SCTP IG Section 2.9 */
			sd->partial_ack = 0;
			sd->flags &= ~SCTP_DESTF_DROPPING;
		}
		if (sd->flags & SCTP_DESTF_ELIGIBLE) {
			rare();
			sd->flags &= ~SCTP_DESTF_ELIGIBLE;
		}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		sd->flags &= ~SCTP_DESTF_FORWDTSN;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
		/* RFC 2960 6.3.2 (R2) */
		if (!sd->in_flight) {
			sd_timer_cancel_retrans(sd);
#ifdef SCTP_CONFIG_ADD_IP
			if (sd->flags & SCTP_DESTF_UNUSABLE)
				sctp_del_daddr(sd);
#endif				/* SCTP_CONFIG_ADD_IP */
		} else
			sd_timer_cond_retrans(sd, sd->rto);
	}
}

/*
 *  CUMM ACK
 *  -------------------------------------------------------------------------
 *  This function is responsible for moving the cummulative ack point.  The sender must check that
 *  the ack is valid (monotonically increasing), but it may be the same TSN as was previously
 *  acknowledged.  When the ack point advances, this function stikes DATA chunks from the retransmit
 *  buffer.  This also indirectly updates the amount of data outstanding for retransmission.  This
 *  function is called by both sctp_recv_sack and sctp_recv_shutdown.
 *
 *  We only perform calculations on TSNs that were not previously acknowledged by a GAP Ack.  If the
 *  TSN has not been retransmitted (Karn's algorithm RFC 2960 6.3.1 (C5)), the destination that it
 *  was set to is marked for RTT calculation update.  If the TSN is currently marked for
 *  retransmission awaiting available cwnd, it is unmarked and retranmission of the TSN is aborted.
 *  If the TSN is not marked for retransmission, the destination's acked bytes accumulator is
 *  increased and the need for a CWND calculation for the destination indicated.  The association
 *  number of bytes in flight is decreased to account for the acknowledged TSN.  If any calculations
 *  are pending as a result of the ack, the function returns non-zero to indication that
 *  calculations (RTT and CWND) must be processed before message processing is complete.
 */
STATIC void
sctp_cumm_ack(struct sctp *sp, uint32_t ack)
{
	mblk_t *mp;
	pl_t pl;

	assert(sp);
	/* PR-SCTP 3.5 (A3) */
	sp->t_ack = ack;
	pl = bufq_lock(&sp->rtxq);
	while ((mp = bufq_head(&sp->rtxq)) && !after(SCTP_TCB(mp)->tsn, ack)) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
			struct sctp_daddr *sd = cb->daddr;

			/* RFC 2960 6.3.1 (C5) */
			if (sd && cb->trans < 2) {
				/* remember latest transmitted packet acked for rtt calc */
				sd->when = (sd->when > cb->when) ? sd->when : cb->when;
			}
			if (cb->flags & SCTPCB_FLAG_RETRANS) {
				cb->flags &= ~SCTPCB_FLAG_RETRANS;
				ensure(sp->nrtxs > 0, sp->nrtxs = 1);
				sp->nrtxs--;
			} else if (!(cb->flags & SCTPCB_FLAG_DROPPED)) {
				size_t dlen = cb->dlen;

				if (sd) {
					/* credit destination (later) */
					normal(sd->in_flight >= sd->ack_accum + dlen);
					sd->ack_accum =
					    (sd->in_flight >
					     sd->ack_accum + dlen) ? sd->ack_accum +
					    dlen : sd->in_flight;
				}
				/* credit association (now) */
				normal(sp->in_flight >= dlen);
				sp->in_flight = (sp->in_flight > dlen) ? sp->in_flight - dlen : 0;
			}
		} else {
			/* can't reneg cummulatively acked chunks */
			cb->flags &= ~SCTPCB_FLAG_SACKED;
			ensure(sp->nsack, sp->nsack = 1);
			sp->nsack--;
		}
		__bufq_unlink(&sp->rtxq, mp);
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG
		    && sp->ops->datack_ind)
			bufq_queue(&sp->ackq, mp);
		else
			freemsg(mp);
		/* NOTE: In STREAMS we need to back-enable the write queue if required.  In the
		   Linux socket code this is accomplished by the skb destructor with the kfree_skb
		   above, or when the ackq is purged. */
		if (sp->wq->q_count)
			qenable(sp->wq);
	}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	/* PR-SCTP 3.5 (A3 - C1) */
	if (before(sp->l_fsn, sp->t_ack))
		sp->l_fsn = sp->t_ack;
	/* PR-SCTP 3.5 (A3 - C2) */
	for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		if (!after(cb->tsn, sp->l_fsn))
			continue;
		if (!(cb->flags & SCTPCB_FLAG_DROPPED))
			break;
		/* push the forward ack point */
		if (cb->tsn == sp->l_fsn + 1)
			sp->l_fsn++;
	}
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sp);
		sp_timer_cond_sack(sp, sp->max_sack);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_timer_cancel_sack(sp);
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	bufq_unlock(&sp->rtxq, pl);
	return;
}

/*
 *  ACK CALC
 *  -------------------------------------------------------------------------
 *  This one is for messages for which a timer is set an retransmission occurs until acknowledged.
 *  We stop the timer, perform an RTT calculation if the message was not retransmitted, free the
 *  retry buffer, and clear the association and destination retransmission counts.
 */
STATIC void
sctp_ack_calc(struct sctp *sp, volatile toid_t *tp)
{
	sctp_tcb_t *cb;

	assert(sp);
	assert(sp->retry);
	cb = SCTP_TCB(sp->retry);
	sp_timer_cancel(sp, tp);
	if (cb->daddr && cb->trans < 2)
		sctp_rtt_calc(cb->daddr, cb->when);
	freechunks(xchg(&sp->retry, NULL));
}

/*
 *  DELIVER DATA
 *  -------------------------------------------------------------------------
 *  There is reason to believe that there is data waiting in gaps that can be delivered.  This
 *  function delivers whatever data is possible and pushes the necessary ack points.
 */
STATIC void
sctp_deliver_data(struct sctp *sp)
{
	sctp_tcb_t *cb, *cb_next = sp->gaps;

	while ((cb = cb_next)) {
		mblk_t *mp = cb->mp;

		cb_next = cb->next;
		if (!(cb->flags & SCTPCB_FLAG_DELIV)) {
			mblk_t *db;
			struct sctp_strm *st = cb->st;
			uint flags = cb->flags;
			int ord = !(flags & SCTPCB_FLAG_URG);
			int more = !(flags & SCTPCB_FLAG_LAST_FRAG);
			int frag = !(flags & SCTPCB_FLAG_FIRST_FRAG);

			/* test for advanced delivery around gaps */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			if (after(cb->tsn, sp->p_fsn + 1))
#else				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
			if (after(cb->tsn, sp->r_ack + 1))
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
			{
				/* after gap */
				if (frag)
					continue;
				if (ord) {
					if (st->n.more & SCTP_STRMF_MORE)
						continue;
					if (cb->ssn != ((st->ssn + 1) & 0xffff))
						continue;
				} else {
					if (st->x.more & SCTP_STRMF_MORE)
						continue;
					if (more)
						continue;
				}
			}
			if ((db = sctp_dupb(sp, mp, 0))) { /* must be a dup, not a copy */
				pl_t pl;

				pl = bufq_lock(&sp->oooq);
				sp->oooq.q_count -= cb->dlen;
				db->b_cont = xchg(&mp->b_cont, NULL);
				cb->flags |= SCTPCB_FLAG_DELIV;
				bufq_unlock(&sp->oooq, pl);
				if (ord) {
					pl = bufq_lock(&sp->rcvq);
					__bufq_queue(&sp->rcvq, db);
					/* IMPLEMENTATION NOTE:- Sockets version indicates data
					   here.  STREAMS doesn't because it indicates data only
					   when it is actually delivered upstream. */
					if (!more) {
						st->n.more &= ~SCTP_STRMF_MORE;
						st->ssn = cb->ssn;
					} else
						st->n.more |= SCTP_STRMF_MORE;
					bufq_unlock(&sp->rcvq, pl);
					SCTP_INC_STATS(SctpInOrderChunks);
				} else {
					pl = bufq_lock(&sp->expq);
					__bufq_queue(&sp->expq, db);
					/* IMPLEMENTATION NOTE:- Sockets version indicates data
					   here.  STREAMS doesn't because it indicates data only
					   when it is actually delivered upstream. */
					if (!more)
						st->x.more &= ~SCTP_STRMF_MORE;
					else
						st->x.more |= SCTP_STRMF_MORE;
					bufq_unlock(&sp->expq, pl);
					SCTP_INC_STATS(SctpInUnorderChunks);
				}
				/* STREAMS calculates reassembled user messages here.  Sockets does
				   not.  Sockets calculates reassembled user messages in
				   sctp_recvmsg. Thus sockets calculates delivered reassembled user
				   messages whereas STREAMS calculates provider reassembled user
				   messages. */
				if (!(cb)->flags & SCTPCB_FLAG_FIRST_FRAG)
					SCTP_INC_STATS_USER(SctpReasmUsrMsgs);
				sp->nunds--;
			} else {
				rare();
				break;
			}	/* no buffers */
		}
		if (cb->tsn == sp->r_ack + 1) {
			sp->r_ack++;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			if (before(sp->p_fsn, sp->r_ack))
				sp->p_fsn = sp->r_ack;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
			sctp_send_sack(sp);
		}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		else if (after(cb->tsn, sp->r_ack) && !after(cb->tsn, sp->p_fsn)) {
			/* FIXME: There is more to do here: we have to somehow inform further
			   receive functions in sctp_recv_data() that partial data chunks on the
			   stream which were skipped should be discarded.  Also, we need to somehow
			   inform waiters in sctp_recvmsg() to wake up and set MSG_TRUNC when
			   waiting on subsequent data chunks for the same packet.  We need to store
			   that in the receive stream datastructure and then pass it into the
			   message control block.  For now, intermediate messages will simply
			   disappear (even if they are in the middle of fragmented messages). For
			   STREAMS we might send a zero-length data message to the stream head. */
			sp->r_ack = cb->tsn;
			sctp_send_sack(sp);
		}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
		if ((cb->flags & SCTPCB_FLAG_DELIV) && !after(cb->tsn, sp->r_ack)) {
			assure(sp->gaps == cb);
			if (!(sp->gaps = cb->next))
				sp->ngaps = 0;
			else {
				if (cb == cb->tail)
					sp->ngaps--;
				else
					cb->next->tail = cb->tail;
			}
			freemsg(bufq_unlink(&sp->oooq, mp));
			ptrace(("INFO: oooq = %u:%u\n", (uint) bufq_length(&sp->oooq),
				(uint) bufq_size(&sp->oooq)));
		}
	}
}

/**
 * sctp_recv_data: - receive DATA chunk(s)
 * @sp: private structure
 * @mp: the message containing the chunk(s)
 *
 * We have received a DATA chunk in a T-Provider state where is it valid to receive DATA
 * (TS_DATA_XFER and TS_WIND_ORDREL).  We can also receive data chunks in TS_IDLE or TS_WRES_CIND on
 * a listening socket bundled with a COOKIE ECHO.  In any other states we discard the data.  Because
 * no other chunks can be bundled after a DATA chunk (just yet), we process all the DATA chunks in
 * the remainder of the message in a single loop here.
 */
STATIC inline fastcall __hot_in int
sctp_recv_data(struct sctp *sp, mblk_t *mp)
{
	int err;
	size_t plen;

#ifdef SCTP_CONFIG_ECN
	int is_ce = INET_ECN_is_ce(SCTP_IPH(mp)->tos);
#endif				/* SCTP_CONFIG_ECN */
	int newd, data, progress = 0;	/* number of new data chunks */
	sctp_tcb_t *cb;
	struct sctp_data *m;

	sctplogda(sp, "received DATA");
	assert(sp);
	assert(mp);
	if ((1 << sp->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	for (newd = 0, data = 0, plen = 0, err = 0; err == 0; mp->b_rptr += plen) {
		mblk_t *dp, *db;
		struct sctp_strm *st;
		sctp_tcb_t **gap;
		uint32_t tsn, ppi;
		uint16_t sid, ssn;
		size_t clen, dlen;
		uint flags;
		int ord;

#if 0
		int more;
#endif
		if (mp->b_rptr == mp->b_wptr)
			break;	/* we're done */
		if (mp->b_rptr > mp->b_wptr) {
			sctplogerr(sp, "%s() should have been caught on last iteration",
				   __FUNCTION__);
			goto emsgsize;
		}
		if (mp->b_rptr + sizeof(struct sctpchdr) > mp->b_wptr)
			goto emsgsize;
		m = (typeof(m)) mp->b_rptr;
		clen = ntohs(m->ch.len);
		if (clen < sizeof(struct sctpchdr))
			goto emsgsize;
		if (mp->b_rptr + clen > mp->b_wptr)
			goto emsgsize;
		plen = PADC(clen);
		if (mp->b_rptr + plen > mp->b_wptr)
			goto emsgsize;
		if (m->ch.type != SCTP_CTYPE_DATA) {
			todo(("Need to ignore padding chunk here...\n"));
			goto eproto;
		}
		if (clen < sizeof(*m))
			goto emsgsize;
		if ((dlen = clen - sizeof(*m)) == 0)
			goto baddata;	/* zero length data */

#if 0
		if ((!(m = (typeof(m)) mp->b_rptr))
		    || ((mp->b_rptr += plen) > mp->b_wptr)
		    || ((blen = mp->b_wptr - mp->b_rptr) < sizeof(struct sctpchdr))
		    || ((clen = ntohs(m->ch.len)) < sizeof(*m))
		    || ((plen = PADC(clen)) > blen)) {
			if (mp->b_wptr > mp->b_rptr)
				goto emsgsize;
			break;
		}
#endif
		flags = (m->ch.flags);
		ord = !(flags & SCTPCB_FLAG_URG);
#if 0
		more = !(flags & SCTPCB_FLAG_LAST_FRAG) ? SCTP_STRMF_MORE : 0;
#endif
		tsn = ntohl(m->tsn);
		sid = ntohs(m->sid);
		ssn = ntohs(m->ssn);
		ppi = ntohl(m->ppi);
		if (sid >= sp->n_istr)
			goto badstream;
		if ((sp->a_rwnd << 1) <=
		    bufq_size(&sp->oooq) + bufq_size(&sp->dupq) + bufq_size(&sp->rcvq) +
		    bufq_size(&sp->expq))
			goto flowcontrol;
		if (!(st = sctp_istrm_find(sp, sid, &err)))
			goto enomem;
		if (!(dp = sctp_dupb(sp, mp, sizeof(*m) + dlen)))
			goto enobufs;
		data++;
		/* pull copy to data only */
		dp->b_rptr += sizeof(*m);
		/* trim copy to data only */
		dp->b_wptr = dp->b_rptr + dlen;
#if 0
		/* fast path, next expected, nothing backed up, nothing out of order */
		if (tsn == sp->r_ack + 1 && !bufq_head(&sp->rcvq) && !bufq_head(&sp->expq)
		    && !bufq_head(&sp->oooq)) {
			/* we have next expected TSN, just process it */
			if ((err = sctp_data_ind(sp, ppi, sid, ssn, tsn, ord, more, dp)))
				goto free_error;
			if (ord) {
				if (!more) {
					st->n.more &= ~SCTP_STRMF_MORE;
					st->ssn = ssn;
				} else
					st->n.more |= SCTP_STRMF_MORE;
				SCTP_INC_STATS(SctpInOrderChunks);
			} else {
				if (!more)
					st->x.more &= ~SCTP_STRMF_MORE;
				else
					st->x.more |= SCTP_STRMF_MORE;
				SCTP_INC_STATS(SctpInUnorderChunks);
			}
			sp->r_ack++;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			if (before(sp->p_fsn, sp->r_ack))
				sp->p_fsn = sp->r_ack;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ECN
			if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
				sp->l_lsn = tsn;
				sp->sackf |= SCTP_SACKF_ECN;
			}
#endif				/* SCTP_CONFIG_ECN */
			newd++;
			continue;
		}
#endif
		if (!(db = sctp_alloc_ctl(sp, 0, clen - sizeof(*m))))
			goto free_nobufs;
		cb = SCTP_TCB(db);
		cb->when = jiffies;
		cb->tsn = tsn;
		cb->sid = sid;
		cb->ssn = ssn;
		cb->ppi = ppi;
		cb->flags = flags & 0x7;
		cb->daddr = sp->caddr;
		cb->st = st;
		cb->tail = cb;
		db->b_cont = dp;
		usual(sp->caddr);
		/* fast path, next expected, nothing out of order */
		if (tsn == sp->r_ack + 1 && !bufq_head(&sp->oooq)) {
			/* we have next expected TSN, just process it */
			sctplogda(sp, "fast-tracking received DATA tsn = %u", tsn);
			cb->flags |= SCTPCB_FLAG_DELIV;
			if (ord) {
				bufq_queue(&sp->rcvq, db);
			} else {
				bufq_queue(&sp->expq, db);
			}
			sp->r_ack++;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			if (before(sp->p_fsn, sp->r_ack))
				sp->p_fsn = sp->r_ack;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ECN
			if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
				if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
					sp->l_lsn = tsn;
					sctp_send_ecne(sp);
				}
			}
#endif				/* SCTP_CONFIG_ECN */
			newd++;
			continue;
		}
		if (!after(tsn, sp->r_ack))
			goto sctp_recv_data_duplicate;
		for (gap = &sp->gaps; (*gap); gap = &((*gap)->tail->next)) {
			if (between(tsn, (*gap)->tsn, (*gap)->tail->tsn))
				goto sctp_recv_data_duplicate;
			if (before(tsn, (*gap)->tsn)) {
				/* insert in front of gap */
				ptrace(("INFO: Expecting tsn = %u, inserting %u\n", sp->r_ack,
					tsn));
				bufq_insert(&sp->oooq, (*gap)->mp, db);
				cb->next = (*gap);
				(*gap) = cb;
				sp->ngaps++;
			} else if (tsn == (*gap)->tail->tsn + 1) {
				/* expand at end of gap */
				ptrace(("INFO: Expecting tsn = %u, expanding %u\n", sp->r_ack,
					tsn));
				bufq_queue(&sp->oooq, db);
				cb->next = (*gap)->tail->next;
				(*gap)->tail->next = cb;
				(*gap)->tail = cb;
				progress = 1;
			} else {
				/* try next gap */
				continue;
			}
			if (cb->next && cb->next->tsn == tsn + 1) {
				/* join two gaps */
				ptrace(("INFO: Joining gaps\n"));
				(*gap)->tail = cb->next->tail;
				usual(sp->ngaps);
				sp->ngaps--;
				progress = 1;
			}
			break;
		}
		if (!(*gap)) {
			/* append to list */
			ptrace(("INFO: Expecting tsn = %u, appending %u\n", sp->r_ack, tsn));
			bufq_queue(&sp->oooq, db);
			cb->next = (*gap);
			(*gap) = cb;
			sp->ngaps++;
		}
		sp->nunds++;	/* more undelivered data */
#ifdef SCTP_CONFIG_ECN
		if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
			if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
				sp->l_lsn = tsn;
				sctp_send_ecne(sp);
			}
		}
#endif				/* SCTP_CONFIG_ECN */
		newd++;
		continue;
	      sctp_recv_data_duplicate:
		/* message is a duplicate tsn */
		ptrace(("INFO: Expecting tsn = %u, duplicate %u\n", sp->r_ack, tsn));
		bufq_queue(&sp->dupq, db);
		cb->next = sp->dups;
		sp->dups = cb;
		sp->ndups++;
		continue;
#if 0
	      free_error:
		freemsg(dp);
		break;
#endif
	      free_nobufs:
		freemsg(dp);
		/* We should not break with ENOBUFS or ENOMEM or EBUSY here... I'm not sure that we 
		   have left the buffer in a state where it can be put back on the queue and
		   processed later. */
	      enobufs:
		ptrace(("ERROR: could not allocate buffer\n"));
		err = -ENOBUFS;
		break;
	      enomem:
		err = -ENOMEM;
		ptrace(("ERROR: could not allocate stream\n"));
		break;
	      flowcontrol:
#if 0
		sctplogerr(sp, "%s() teardrop protection", __FUNCTION__);
		sctplogerr(sp, "oooq size is %d", (int) bufq_size(&sp->oooq));
		sctplogerr(sp, "dupq size is %d", (int) bufq_size(&sp->dupq));
		sctplogerr(sp, "rcvq size is %d", (int) bufq_size(&sp->rcvq));
		sctplogerr(sp, "expq size is %d", (int) bufq_size(&sp->expq));
		sctplogerr(sp, "a_rwnd is %d", (int) sp->a_rwnd);
		/* Protect from teardrop attack without renegging */
		if (bufq_size(&sp->rcvq) + bufq_size(&sp->expq)) {
			/* TODO: need to put renegging code in here: renegging should first walk
			   the gap queue looking for any data that we have gap acked which has not
			   yet been delivered to the user.  Any such data can be deleted. Once
			   deleting this data we can check if we can process the current message
			   anyway.  If that is true, we can mark that a sack is required
			   immediately and jump back up to the top where we entered flow control.
			   If we are still flow controlled, we will need to abort the association.
			   For now we just abort the association. */
			err = -ECONNRESET;
			sctp_send_abort(sp);
			sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, err, NULL);
			sctp_abort(sp, SCTP_ORIG_PROVIDER, err);
			break;
		}
#endif
		/* If we have data that the user has not read yet, then we keep all our gaps,
		   because the user reading data will make some room to process things later.  We
		   must send an immediate SACK regardless, per the IG requirements. */
		/* SCTP Implementor's Guide Section 2.15.1 */
		sctp_send_sack(sp);
		ptrace(("ERROR: flow controlled (discard)\n"));
		err = -EBUSY;
		break;		/* flow controlled (discard) */
	      badstream:
		ptrace(("PROTO: invalid stream id\n"));
		sctp_send_error(sp, SCTP_CAUSE_INVALID_STR, (caddr_t) &m->sid, sizeof(m->sid));
		continue;	/* just skip that DATA chunk */
	      baddata:
		ptrace(("PROTO: no user data in data chunk\n"));
		/* RFC 2960 6.2: ...no user data... */
		sctp_send_abort_error(sp, SCTP_CAUSE_NO_DATA, &m->tsn, sizeof(m->tsn));
		sctp_abort(sp, SCTP_ORIG_PROVIDER, SCTP_CAUSE_NO_DATA);
		return (-ECONNRESET);
	      eproto:
		err = -EPROTO;
		ptrace(("PROTO: non-data chunk after data chunk\n"));
		break;
	      emsgsize:
		err = -EMSGSIZE;
		ptrace(("PROTO: bad message or chunk size\n"));
		break;		/* bad message or chunk sizes */
	}
	if (!data)
		goto no_data;
	if (!newd)
		goto no_new_data;
	/* we have underlivered data and new data */
	sctp_deliver_data(sp);
      no_new_data:
	/* RFC 2960 6.2 */
	if (sp->ndups) {
		/* IMPLEMENTATION NOTE:- If we are receiving duplicates the probability is high
		   that our SACKs aren't getting through (or have been delayed too long).  If we do 
		   not have a sack pending (one being delayed) then we will peg the duplicates
		   against the destination. This will change where we are sending SACKs. Because we 
		   are being cruel to this destination and we don't really know that this is where
		   the offending SACKs were sent, we send an immediate heartbeat if there is no
		   data is in flight to the destination (i.e., no retransmission timer running for
		   the destination).  This fixes some sticky problems when one-way data is being
		   sent. */
		struct sctp_daddr *sd = sp->taddr;

		sp->sackf |= SCTP_SACKF_DUP;
		if (sd && !(sp->sackf & SCTP_SACKF_NEW)) {
			sd->dups += sp->ndups;
			if (!sd->in_flight && !sctp_timeout_pending(&sd->timer_heartbeat)) {
				sd_timer_cancel_idle(sd);
				sctp_send_heartbeat(sp, sd);
			}
		}
	}
	/* RFC 2960 7.2.4 */
	if (sp->ngaps && !progress) {
		/* IMPLEMENTATION NOTE:- If we have gaps then we should send a SACK with gap
		   reports immediately; however, it is better not to send so many SACKs and gap
		   reports when the peer is making progress on filling the gaps.  Consider that a
		   retransmission filling a gap is not so different than a retransmission that
		   simply moves the cummulative ack point forward, and that in the latter case we
		   do normal delayed acknowledgements.  So, when the peer is making progress on
		   filling a gap, we do not issue a SACK immediately, but perform normal delayed
		   acknowledgement. */
		sp->sackf |= SCTP_SACKF_GAP;
	}
	/* RFC 2960 6.2 */
	if (newd) {
		/* IMPLEMENTATION NOTE:- The SACK timer is probably too slow.  For unidirectional
		   operation, the sender may have timed out before we send a sack.  We should
		   really not wait any longer than some fraction of the RTO for the destination
		   from which we are receiving (or sending) data. However, if we wait too long we
		   will just get a retransmission and a dup. If the sack delay is set to zero, we
		   do not set the timer, but issue the sack immediately. */
		if (sp->max_sack) {
#ifdef ETSI
			/* support ETSI SACK frequency */
			sp->sackf |= SCTP_SACKF_NEW;
			if (++sp->sackcnt >= sp->sack_freq) {
				sp->sackf |= SCTP_SACKF_WUP;
				sp->sackcnt = 0;
			}
#else
			sp->sackf += ((sp->sackf & 0x3) < 3) ? SCTP_SACKF_NEW : 0;
#endif
			sp_timer_cond_sack(sp, sp->max_sack);
		} else
			sp->sackf |= SCTP_SACKF_TIM;
	}
	if (sp->state == SCTP_SHUTDOWN_SENT)
		sctp_send_shutdown(sp);
      no_data:
	goto done;
      done:
	/* We should not break with ENOBUFS or ENOMEM or EBUSY here... I'm not sure that we have
	   left the buffer in a state where it can be put back on the queue and processed later. */
	switch (err) {
	case -ENOBUFS:
	case -EAGAIN:
	case -ENOMEM:
	case -EBUSY:
		/* SCTP IG 2.15.1 */
		sctp_send_sack(sp);
		err = 0;
	}
	return (err);
      outstate:
	/* We have received DATA in the wrong state.  If so, it is probably an old packet that was
	   stuck in the network and just got delivered to us.  Nevertheless we should just ignore
	   any message containing DATA when we are not expecting it.  The only exception to this
	   might be receiving DATA in the COOKIE-WAIT state.  There should not be data hanging
	   around in the network that matches our tags.  If that is the case, we should abandon the
	   connection attempt and let the user try again with a different verification tag.  In the
	   COOKIE-ECHOED state, if we receive data, we might very well assume that we have received
	   a COOKIE-ACK and process the data anyway.  */
	sctplogerr(sp, "%s() data in incorrect state %s", __FUNCTION__, sctp_statename(sp->state));
	if (sp->state == SCTP_COOKIE_WAIT) {
		sctp_abort(sp, SCTP_ORIG_PROVIDER, -ECONNRESET);
		err = -EPROTO;
	}
	err = QR_DONE;
	goto done;
}

/*
 *  RECV SACK
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_sack(struct sctp *sp, mblk_t *mp)
{

	mblk_t *dp;
	struct sctp_sack *m;
	size_t ngaps, ndups;
	uint32_t ack, rwnd;
	uint16_t *gaps;
	pl_t pl;

	sctplogda(sp, "received SACK");
	assert(sp);
	assert(mp);
	if ((1 << sp->state) & ~(SCTPF_SENDING))
		goto outstate;
	m = (struct sctp_sack *) mp->b_rptr;
	{
		size_t clen;

		if ((clen = ntohs(m->ch.len)) < sizeof(*m))
			goto emsgsize;
		ngaps = ntohs(m->ngaps);
		ndups = ntohs(m->ndups);
		if (clen < sizeof(*m) + (ndups + ngaps) * sizeof(uint32_t))
			goto emsgsize;
	}
	ack = ntohl(m->c_tsn);
	rwnd = ntohl(m->a_rwnd);
	gaps = m->gaps;
	/* RFC 2960 6.2.1 (D) i) */
	if (before(ack, sp->t_ack))
		goto discard;
	/* If the receive window is increasing and we have data in the write queue, we might need
	   to backenable. */
	if (rwnd > sp->p_rwnd && sp->wq && sp->wq->q_count)
		qenable(sp->wq);
	/* RFC 1122 4.2.3.4 IMPLEMENTATION (3) */
	sp->p_rbuf = (sp->p_rbuf >= rwnd) ? sp->p_rbuf : rwnd;
	/* RFC 2960 6.2.1 (D) ii) */
	sp->p_rwnd = rwnd;	/* we keep in_flight separate from a_rwnd */
	if (after(ack, sp->t_ack)) {
		/* advance the cummulative ack point and check need to perform per-round-trip and
		   cwnd calcs */
		sctp_cumm_ack(sp, ack);
		/* if we have cummulatively acked something, we will skip FR analysis for this
		   SACK. */
		/* XXX: for now just assume that if something is cummulatively
		 * acked that the write queue needs to be enabled. */
		qenable(sp->wq);
		goto skip_rtx_analysis;
	}
	if (ndups) {
		/* TODO: we could look through the list of duplicate TSNs.  Duplicate TSNs really
		   means that the peer's SACKs aren't getting back to us.  But there is nothing
		   really that we can do about that.  The peer has already detected the problem and
		   should sent SACKs via an alternative route if possible.  But that's how this SACK 
		   got here... */
	}
	/* process gap acks */
	if (!ngaps) {
		/* If there are no gap acks in the received SACK, the SACK might still constitute a 
		   reneg on previous SACKs.  If some chunks have been previously gap acked, we
		   check for the reneg, otherwise we skip the loop.  sp->nsack here contains the
		   number of renegable gap acked data chunks. */
		if (!sp->nsack)
			goto skip_rtx_analysis;
		/* process renegs, fast retransmission cannot occur because we have no gaps */
		pl = bufq_lock(&sp->rtxq);
		for (dp = bufq_head(&sp->rtxq); dp; dp = dp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(dp);

			/* RFC 2960 6.3.2 (R4) (reneg) */
			if (cb->flags & SCTPCB_FLAG_SACKED) {
				struct sctp_daddr *sd = cb->daddr;

				cb->flags &= ~SCTPCB_FLAG_SACKED;
				ensure(sp->nsack, sp->nsack = 1);
				sp->nsack--;
				if (sd)
					sd_timer_cond_retrans(sd, sd->rto);
				else
					seldom();
			}
		}
		bufq_unlock(&sp->rtxq, pl);
	} else {
		int eligible = 0;

		{
			struct sctp_daddr *sd;

			/* We skip gap analysis if we are running ultra-fast destinations with data 
			   outstanding after cummulative ack, because the gap reports are probably
			   out of date and it will just consume processing power checking them when
			   an RTO is coming fast anyway */
			for (sd = sp->daddr; sd; sd = sd->next)
				if (sd->in_flight && sd->rto > 0)
					goto do_gap_analysis;
			goto skip_rtx_analysis;
		}
	      do_gap_analysis:
		/* perform fast retransmission algorithm on gaps */
		while (ngaps--) {
			uint32_t beg = ack + ntohs(*gaps++);
			uint32_t end = ack + ntohs(*gaps++);

			if (before(end, beg)) {
				rare();
				continue;
			}
			/* move to the acks */
			pl = bufq_lock(&sp->rtxq);
			dp = bufq_head(&sp->rtxq);
			for (; dp && before(SCTP_TCB(dp)->tsn, beg); dp = dp->b_next) {
				sctp_tcb_t *cb = SCTP_TCB(dp);
				struct sctp_daddr *sd = cb->daddr;

				cb->flags |= SCTPCB_FLAG_NACK;
				if (sd && !(sd->flags & SCTP_DESTF_ELIGIBLE)
				    && (cb->flags & SCTPCB_FLAG_ACK)
				    && !(cb->flags & SCTPCB_FLAG_SACKED)
				    && (cb->sacks < SCTP_FR_COUNT)) {
					sd->flags |= SCTP_DESTF_ELIGIBLE;
					eligible++;
				}
			}
			/* sack the acks */
			for (; dp && !after(SCTP_TCB(dp)->tsn, end); dp = dp->b_next) {
				sctp_tcb_t *cb = SCTP_TCB(dp);
				struct sctp_daddr *sd = cb->daddr;

				cb->flags |= SCTPCB_FLAG_ACK;
				if (sd && !(sd->flags & SCTP_DESTF_ELIGIBLE)
				    && (cb->flags & SCTPCB_FLAG_NACK)
				    && !(cb->flags & SCTPCB_FLAG_SACKED)
				    && (cb->sacks < SCTP_FR_COUNT)) {
					sd->flags |= SCTP_DESTF_ELIGIBLE;
					eligible++;
				}
			}
			bufq_unlock(&sp->rtxq, pl);
		}
		/* walk the whole retrans buffer looking for holes and renegs */
		pl = bufq_lock(&sp->rtxq);
		for (dp = bufq_head(&sp->rtxq); dp; dp = dp->b_next) {
			sctp_tcb_t *cb = SCTP_TCB(dp);
			struct sctp_daddr *sd = cb->daddr;

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			/* msg was dropped */
			if ((cb->flags & SCTPCB_FLAG_DROPPED)) {
				cb->flags &= ~(SCTPCB_FLAG_ACK | SCTPCB_FLAG_NACK);
				continue;
			}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
			/* msg is inside gapack block */
			if (cb->flags & SCTPCB_FLAG_ACK) {
				cb->flags &= ~SCTPCB_FLAG_ACK;
				if (cb->flags & SCTPCB_FLAG_NACK) {
					cb->flags &= ~SCTPCB_FLAG_NACK;
					if (sd && (sd->flags & SCTP_DESTF_ELIGIBLE)
					    && !(cb->flags & SCTPCB_FLAG_SACKED)
					    && (cb->sacks < SCTP_FR_COUNT)) {
						sd->flags &= ~SCTP_DESTF_ELIGIBLE;
						eligible--;
					}

				}
				if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
					cb->flags |= SCTPCB_FLAG_SACKED;
					sp->nsack++;
					/* RFC 2960 6.3.1 (C5) */
					if (sd && cb->trans < 2) {
						/* remember latest transmitted packet acked for rtt 
						   calc */
						sd->when = (sd->when > cb->when)
						    ? sd->when : cb->when;
					}
					if (cb->flags & SCTPCB_FLAG_RETRANS) {
						cb->flags &= ~SCTPCB_FLAG_RETRANS;
						ensure(sp->nrtxs > 0, sp->nrtxs = 1);
						sp->nrtxs--;
					} else {
						size_t dlen = cb->dlen;

						if (sd) {
							/* credit destination */
							normal(sd->in_flight >= dlen);
							sd->in_flight =
							    (sd->in_flight >
							     dlen) ? sd->in_flight - dlen : 0;
						}
						/* credit association */
						normal(sp->in_flight >= dlen);
						sp->in_flight = (sp->in_flight > dlen)
						    ? sp->in_flight - dlen : 0;
					}
				}
				continue;
			}
			/* msg is between gapack blocks */
			if (cb->flags & SCTPCB_FLAG_NACK) {
				cb->flags &= ~SCTPCB_FLAG_NACK;
				/* RFC 2960 7.2.4 */
				if (eligible > 0 && (!sd || (sd->flags & SCTP_DESTF_ELIGIBLE))
				    && !(cb->flags & SCTPCB_FLAG_RETRANS)
				    && ++(cb->sacks) == SCTP_FR_COUNT) {
					/* IMPLEMENTATION NOTE:- Performing fast retransmission can
					   be bad when large or many chunks have gotten reordered or
					   delayed (and gap reported) yet progress on those gaps are
					   (now) being reported, it indicates that reordered or
					   delayed packets are now arriving and fast retransmission
					   should not be considered, because the packets are not
					   likely missing. */
					size_t dlen = cb->dlen;

					/* RFC 2960 7.2.4 (1) */
					cb->flags |= SCTPCB_FLAG_RETRANS;
					sp->nrtxs++;
					if (sd) {
						/* RFC 2960 7.2.4 (2) */
						sd->flags |= SCTP_DESTF_DROPPING;
						/* credit destination (now) */
						normal(sd->in_flight >= dlen);
						sd->in_flight = (sd->in_flight > dlen)
						    ? sd->in_flight - dlen : 0;
					}
					/* credit association (now) */
					normal(sp->in_flight >= dlen);
					sp->in_flight = (sp->in_flight > dlen)
					    ? sp->in_flight - dlen : 0;
				}
				/* RFC 2960 6.3.2 (R4) (reneg) */
				if (cb->flags & SCTPCB_FLAG_SACKED) {
					cb->flags &= ~SCTPCB_FLAG_SACKED;
					ensure(sp->nsack, sp->nsack = 1);
					sp->nsack--;
					if (sd)
						sd_timer_cond_retrans(sd, sd->rto);
					else
						seldom();
				}
				continue;
			}
			/* msg is after all gapack blocks */
			break;
		}
		bufq_unlock(&sp->rtxq, pl);
	}
      skip_rtx_analysis:
	sctp_dest_calc(sp);
	/* After receiving a cummulative ack, I want to check if the sndq, urgq and rtxq is empty
	   and a SHUTDOWN or SHUTDOWN-ACK is pending.  If so, I want to issue these primitives. */
	if (((1 << sp->state) & (SCTPF_SHUTDOWN_PENDING | SCTPF_SHUTDOWN_RECEIVED
				 | SCTPF_SHUTDOWN_RECVWAIT))
	    && !bufq_head(&sp->sndq)
	    && !bufq_head(&sp->urgq)
	    && !bufq_head(&sp->rtxq)) {
		seldom();
		switch (sp->state) {
		case SCTP_SHUTDOWN_PENDING:
			/* Send the SHUTDOWN I didn't send before. */
			sctp_send_shutdown(sp);
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			/* Send the SHUTDOWN-ACK I didn't send before */
			if (!sp->ops->ordrel_ind)
				sctp_send_shutdown_ack(sp);
			break;
		case SCTP_SHUTDOWN_RECVWAIT:
			/* Send the SHUTDOWN-ACK I didn't send before */
			sctp_send_shutdown_ack(sp);
			break;
		default:
			sctplogerr(sp, "%s() SACK received in wrong SCTP state %s", __FUNCTION__,
				   sctp_statename(sp->state));
			return (-EFAULT);
		}
	}
      done:
	{
		int ret;

		if ((ret = sctp_return_more(mp)) > 0) {
			struct sctpchdr *ch = SCTP_CH(mp);

			switch (ch->type) {
				/* RFC 2960 6 */
			case SCTP_CTYPE_DATA:
				/* RFC 2960 6.5 */
			case SCTP_CTYPE_ERROR:
				/* RFC 2960 3.3.7 */
			case SCTP_CTYPE_ABORT:
				break;
			default:
				sctplogrx(sp, "%s(): message syntax error", __FUNCTION__);
				return (-EPROTO);
			}
		}
		return (ret);
	}
      discard:
	return (0);
      emsgsize:
	return (-EMSGSIZE);
      outstate:
	/* We may have received a SACK in the wrong state.  Because SACKs are completely advisory,
	   there is no reason to get too upset about this. Simply ignore them.  No need to process
	   them. */
	goto done;
}

/*
 *  RECV ERROR
 *  -------------------------------------------------------------------------
 *  We have received an ERROR chunk in opening, connected or closing states.
 */
STATIC int
sctp_recv_error(struct sctp *sp, mblk_t *mp)
{
	int err;
	struct sctp_error *m;
	struct sctpehdr *eh;
	int ecode;
	size_t clen, elen;

	sctplogrx(sp, "received ERROR");
	assert(sp);
	assert(mp);
	seldom();
	m = (typeof(m)) mp->b_rptr;
	eh = (typeof(eh)) (m + 1);
	if ((clen = ntohs(m->ch.len)) < sizeof(*m) + sizeof(*eh))
		goto emsgsize;
	if ((elen = ntohs(eh->len)) < sizeof(*eh) || sizeof(*m) + elen > clen)
		goto emsgsize;
	ecode = ntohs(eh->code);
	switch (ecode) {
	case SCTP_CAUSE_STALE_COOKIE:
	{
		sctp_tcb_t *cb;
		struct sctp_daddr *sd;

		if (sp->state != SCTP_COOKIE_ECHOED)
			break;
		ensure(sp->retry, goto recv_error_error);
		cb = SCTP_TCB(sp->retry);
		sd = cb->daddr;
		ensure(sd, goto recv_error_error);
		/* We can try again with cookie preservative, and then we can keep trying until we
		   have tried as many times as we can... */
		if (!sp->ck_inc) {
			sp->ck_inc = sp->ck_inc + (sd->rto >> 1);
			sctp_ack_calc(sp, &sp->timer_init);
			if ((err = sctp_send_init(sp)))
				return (err);
			return sctp_return_stop(mp);
		}
		/* RFC 2960 5.2.6 (1) */
		if (cb->trans < sp->max_inits) {
			sp_timer_cancel_sack(sp);
			/* RFC 2960 5.2.6 (3) */
			if (cb->trans < 2)
				sctp_rtt_calc(sd, cb->when);
			sctp_send_msg(sp, sd, sp->retry);
			return sctp_return_stop(mp);
		}
		/* RFC 2960 5.2.6 (2) */
		goto recv_error_error;
	}
	case SCTP_CAUSE_INVALID_PARM:
	case SCTP_CAUSE_BAD_ADDRESS:
		/* If the sender of the ERROR has already given us a valid INIT-ACK then we can
		   ignore these errors. */
		if (sp->state == SCTP_COOKIE_ECHOED)
			break;
		seldom();
	case SCTP_CAUSE_MISSING_PARM:
	case SCTP_CAUSE_NO_RESOURCE:
	case SCTP_CAUSE_INVALID_STR:
		/* These errors are bad.  If we don't get an abort with them then we must abort the 
		   association. */
	      recv_error_error:
		sctp_send_abort(sp);
		sctp_abort(sp, SCTP_ORIG_PROVIDER, ecode);
		return sctp_return_stop(mp);
	case SCTP_CAUSE_BAD_CHUNK_TYPE:
#if defined(SCTP_CONFIG_ADD_IP)||defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	{
		struct sctpchdr *ch = (typeof(ch)) (eh + 1);

		switch (ch->type) {
#if defined(SCTP_CONFIG_ADD_IP)
		case SCTP_CTYPE_ASCONF:
		case SCTP_CTYPE_ASCONF_ACK:
			sctp_abort_asconf(sp);
			sp->p_caps &= ~SCTP_CAPS_ADD_IP;
			sp->p_caps &= ~SCTP_CAPS_SET_IP;
			break;
#endif				/* defined(SCTP_CONFIG_ADD_IP) */
#if defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
		case SCTP_CTYPE_FORWARD_TSN:
			/* This is actually worse than it looks: we were told that the other end
			   was PR capable. */
			sp->p_caps &= ~SCTP_CAPS_PR;
			break;
#endif				/* defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
		}
		break;
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP)||defined(SCTP_CONFIG_PARTIAL_RELIABILITY) 
				 */
	case SCTP_CAUSE_BAD_PARM:
#if defined(SCTP_CONFIG_ADD_IP)||defined(SCTP_CONFIG_PARTIAL_RELIABILITY)||defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	{
		struct sctpphdr *ph = (typeof(ph)) (eh + 1);
		int plen;

		if (elen < sizeof(*eh) + sizeof(*ph))
			goto emsgsize;
		if ((plen = ntohs(ph->len)) < sizeof(*ph) || PADC(plen) + mp->b_rptr > mp->b_wptr)
			goto emsgsize;
		switch (ph->type) {
#if defined(SCTP_CONFIG_ADD_IP)
		case SCTP_PTYPE_ADD_IP:
		case SCTP_PTYPE_DEL_IP:
			/* FIXME: what about valid sets with invalid adds ? */
			sctp_abort_asconf(sp);
			sp->p_caps &= ~SCTP_CAPS_ADD_IP;
			break;
		case SCTP_PTYPE_SET_IP:
			/* FIXME: what about valid sets with invalid adds ? */
			sctp_abort_asconf(sp);
			sp->p_caps &= ~SCTP_CAPS_SET_IP;
			break;
#endif				/* defined(SCTP_CONFIG_ADD_IP) */
#if defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
		case SCTP_PTYPE_PR_SCTP:
			/* This is actually worse than it looks: we were told that the other end
			   was PR capable. */
			sp->p_caps &= ~SCTP_CAPS_PR;
			break;
#endif				/* defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
		case SCTP_PTYPE_ALI:
			sp->p_caps &= ~SCTP_CAPS_ALI;
			break;
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_ECN
		case SCTP_PTYPE_ECN_CAPABLE:
			sp->p_caps &= ~SCTP_CAPS_ECN;
			INET_ECN_dontxmit(sp);
			break;
#endif				/* SCTP_CONFIG_ECN */
		}
		break;
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP)||defined(SCTP_CONFIG_PARTIAL_RELIABILITY)||defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) 
				 */
	default:
	case SCTP_CAUSE_NO_DATA:
	case SCTP_CAUSE_SHUTDOWN:
	case SCTP_CAUSE_USER_INITIATED:
	case SCTP_CAUSE_PROTO_VIOLATION:
		/* These should either not come here or result in an abort */
	case SCTP_CAUSE_ILLEGAL_ASCONF:	/* Illegal ASCONF-ACK */
	case SCTP_CAUSE_LAST_ADDR:	/* Request to delete last address */
	case SCTP_CAUSE_RES_SHORTAGE:	/* Operation refused resource shortage */
	case SCTP_CAUSE_SOURCE_ADDR:	/* Request to delete source address */
		rare();
		break;
	}
	return sctp_return_more(mp);	/* ignore */
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV ABORT
 *  -------------------------------------------------------------------------
 *  We have received an ABORT chunk in opening, connected or closing states.  If there is a user
 *  around we want to send a disconnect indication, otherwise we want to just go away.
 */
STATIC int
sctp_recv_abort_listening(struct sctp *sp, mblk_t *mp)
{

	mblk_t *cp;
	struct iphdr *iph = SCTP_IPH(mp);
	struct sctphdr *sh = SCTP_SH(mp);
	struct sctp_abort *m = (typeof(m)) mp->b_rptr;
	t_uscalar_t orig = (m->ch.flags & 0x1) ? SCTP_ORIG_PROVIDER : SCTP_ORIG_USER;
	pl_t pl;

	/* FIXME: above orig is not completely true, it also depends on whether there is a cause
	   value, what the cause value is, and the state. */
	sctplogrx(sp, "received ABORT (listening)");
	pl = bufq_lock(&sp->conq);
	for (cp = bufq_head(&sp->conq); cp; cp = cp->b_next) {
		struct sctp_cookie_echo *ce = (typeof(ce)) cp->b_rptr;
		struct sctp_cookie *ck = (typeof(ck)) ce->cookie;

		if (ck->v_tag == sh->v_tag && ck->sport == sh->dest && ck->dport == sh->srce
#if 0
		    && ck->saddr == iph->daddr && ck->daddr == iph->saddr
#endif
		    ) {
			int err;

			/* FIXME: the reasons -ECONNRESET is only if there is no cause value in the 
			   abort message. If there is a cause value, the reason should reflect the
			   cause value if it makes sense. */
			sctplogrx(sp, "removed conn ind on ABORT");
			/* NOTE: The disconnect indication function will free the connection
			   indication when the function does not return an error */
			__bufq_unlink(&sp->conq, cp);
			bufq_unlock(&sp->conq, pl);
			if ((err = sctp_discon_ind(sp, orig, -ECONNRESET, cp))) {
				bufq_queue(&sp->conq, cp);
				return (err);
			}
			goto done;
		} else {
			(void) iph;
			sctplogrx(sp, "conn ind %p did not match", cp);
			sctplogrx(sp, "ck->v_tag = %08x, sh->v_tag = %08x", (int) ck->v_tag,
				  (int) sh->v_tag);
			sctplogrx(sp, "ck->sport = %d, sh->dest = %d", (int) ntohs(ck->sport),
				  (int) ntohs(sh->dest));
			sctplogrx(sp, "ck->dport = %d, sh->srce = %d", (int) ntohs(ck->dport),
				  (int) ntohs(sh->srce));
			sctplogrx(sp, "ck->saddr = %08x, iph->daddr = %08x", (int) ck->saddr,
				  (int) iph->daddr);
			sctplogrx(sp, "ck->daddr = %08x, iph->saddr = %08x", (int) ck->daddr,
				  (int) iph->saddr);
		}
	}
	bufq_unlock(&sp->conq, pl);
	usual(cp);
      done:
	return sctp_return_stop(mp);
}
STATIC int
sctp_recv_abort(struct sctp *sp, mblk_t *mp)
{
	t_scalar_t reason;
	struct sctp_abort *m = (typeof(m)) mp->b_rptr;
	t_uscalar_t orig = (m->ch.flags & 0x1) ? SCTP_ORIG_PROVIDER : SCTP_ORIG_USER;

	/* FIXME: above orig is not completely true, it also depends on whether there is a cause
	   value, what the cause value is, and the state. */
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	sctplogrx(sp, "received ABORT");
	switch (sp->state) {
	case SCTP_LISTEN:
		return sctp_recv_abort_listening(sp, mp);
	case SCTP_COOKIE_WAIT:
	case SCTP_COOKIE_ECHOED:
		reason = -ECONNREFUSED;
		break;
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
	case SCTP_SHUTDOWN_ACK_SENT:
		reason = -EPIPE;
		break;
	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
		reason = -ECONNRESET;
		break;
	default:
		sctplogerr(sp, "%s() ABORT received in wrong SCTP state %s", __FUNCTION__,
			   sctp_statename(sp->state));
		return sctp_return_stop(mp);
	}
	/* FIXME: the reasons set above are only if there is no cause value in the abort message.
	   If there is a cause value, the reason should reflect the cause value if it makes sense. */
	sctp_abort(sp, orig, reason);
	return sctp_return_stop(mp);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV INIT (Listener only)
 *  -------------------------------------------------------------------------
 *  We have receive an INIT in the LISTEN state.  This is the normal path for the establishment of
 *  an SCTP association.  There can be no other stream bound to this local port but we can have
 *  accepted streams which share the same local binding.
 *
 *  INIT chunks cannot have other chunks bundled with them (RFC 2960 6.10).
 */
STATIC int
sctp_recv_init(struct sctp *sp, mblk_t *mp)
{
	struct iphdr *iph;
	struct sctphdr *sh;
	struct sctp_init *m;
	struct sctp *oldsp;
	union sctp_parm *ph;
	unsigned char *pptr, *pend;
	size_t plen;
	size_t anum = 0;
	ulong ck_inc = 0;
	int err = 0;
	size_t errl = 0;
	unsigned char *errp = NULL;
	struct sctp_cookie ck;
	mblk_t *unrec = NULL;
	uint32_t p_caps, p_ali;

#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	static ulong last_init = 0;

	if (last_init && jiffies < last_init + sp->throttle)
		goto ebusy;
	last_init = jiffies;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
	sctplogrx(sp, "received INIT");
	assert(sp);
	assert(mp);
	iph = SCTP_IPH(mp);
	sh = SCTP_SH(mp);
	m = (struct sctp_init *) mp->b_rptr;
	{
		size_t clen = ntohs(m->ch.len);

		pptr = (unsigned char *) (m + 1);
		pend = pptr + clen - sizeof(*m);
		if (clen < sizeof(*m))
			goto emsgsize;
		/* SCTP IG 2.11 not allowed to bundle */
		if (PADC(clen) + mp->b_rptr < mp->b_wptr)
			goto eproto;
	}
	/* RFC 2960 p.26 initiate tag zero */
	if (!m->i_tag)
		goto invalid_parm;
	if (!(unrec = sctp_allocb(sp, mp->b_wptr - mp->b_rptr, BPRI_MED)))
		goto enobufs;
	p_ali = 0;
	p_caps = 0;
	for (ph = (union sctp_parm *) pptr;
	     pptr + sizeof(ph->ph) <= pend && pptr + (plen = ntohs(ph->ph.len)) <= pend;
	     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
		uint type;

		if (plen < sizeof(ph->ph.type))
			goto eproto;
		type = ph->ph.type;
		if (plen < sizeof(ph->ph))
			goto init_bad_parm;
		switch (type) {
		case SCTP_PTYPE_IPV6_ADDR:
			break;
		case SCTP_PTYPE_HOST_NAME:
			goto bad_address;
		case SCTP_PTYPE_COOKIE_PSRV:
			if (plen < sizeof(ph->cookie_prsv))
				goto init_bad_parm;
			ck_inc = ntohl(ph->cookie_prsv.ck_inc);
			break;
		case SCTP_PTYPE_IPV4_ADDR:
			if (plen < sizeof(ph->ipv4_addr))
				goto init_bad_parm;
			/* skip primary */
			if (ph->ipv4_addr.addr != iph->saddr)
				anum++;
			break;
		case SCTP_PTYPE_ADDR_TYPE:
			if (plen < sizeof(ph->addr_type))
				goto init_bad_parm;
			/* Ensure that address types supported includes IPv4.  Actually address
			   types must include IPv4 so we just ignore. */
			break;
#ifdef SCTP_CONFIG_ECN
		case SCTP_PTYPE_ECN_CAPABLE:
			if (plen < sizeof(ph->ecn_capable))
				goto init_bad_parm;
			p_caps |= SCTP_CAPS_ECN;
			INET_ECN_xmit(sp);
			break;
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
		case SCTP_PTYPE_ALI:
			if (plen < sizeof(ph->ali))
				goto init_bad_parm;
			p_ali = ntohl(ph->ali.ali);
			p_caps |= SCTP_CAPS_ALI;
			break;
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		case SCTP_PTYPE_PR_SCTP:
			if (plen < sizeof(ph->pr_sctp))
				goto init_bad_parm;
			p_caps |= SCTP_CAPS_PR;
			break;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
		default:
		      init_bad_parm:
			/* SCTP IG 2.27 */
			if (!(type & SCTP_PTYPE_MASK_CONTINUE)) {
				if (type & SCTP_PTYPE_MASK_REPORT)
					goto bad_parm;
				goto discard;
			}
			if (type & SCTP_PTYPE_MASK_REPORT) {
				/* Queue up unrecognized parameters to add to an INIT-ACK per SCTP
				   IG 2.27.  */
				bcopy(pptr, unrec->b_wptr, PADC(plen));
				unrec->b_wptr += PADC(plen);
			}
			continue;
		}
	}
	/* put together cookie */
	{
		size_t istrs = ntohs(m->n_istr);
		size_t ostrs = ntohs(m->n_ostr);

		/* negotiate inbound and outbound streams */
		if (!istrs || !ostrs)
			goto invalid_parm;
		istrs = (sp->n_istr && sp->n_istr <= istrs) ? sp->n_istr : istrs;
		ostrs = (sp->n_ostr && sp->n_ostr <= ostrs) ? sp->n_ostr : ostrs;
		if (!istrs || !ostrs)
			goto invalid_parm;
		/* RFC 2969 5.2.6 */
		if (ck_inc) {
			seldom();
			ck_inc = (ck_inc * HZ + 999) / 1000;
			usual(ck_inc);
			if (ck_inc > sp->rto_ini + sp->ck_inc) {
				rare();
				ck_inc = sp->rto_ini + sp->ck_inc;
			}
		}
		ck.timestamp = jiffies;
		ck.lifespan = sp->ck_life + ck_inc;
		ck.daddr = iph->saddr;
		ck.saddr = iph->daddr;
		ck.dport = sh->srce;
		ck.sport = sh->dest;
		ck.v_tag = sctp_get_vtag(ck.daddr, ck.saddr, ck.dport, ck.sport);
		ck.p_tag = m->i_tag;
		ck.p_tsn = ntohl(m->i_tsn);
		ck.p_rwnd = ntohl(m->a_rwnd);
		ck.n_istr = istrs;
		ck.n_ostr = ostrs;
		ck.l_caps = sp->l_caps;
		ck.p_caps = p_caps;
		ck.l_ali = sp->l_ali;
		ck.p_ali = p_ali;
		ck.danum = anum;
		sctplogtx(sp, " ck.timestamp = %u", (uint) ck.timestamp);
		sctplogtx(sp, " ck.lifespan  = %u", (uint) ck.lifespan);
		sctplogtx(sp, " ck.daddr     = %d.%d.%d.%d", (ck.daddr >> 0) & 0xff,
			  (ck.daddr >> 8) & 0xff, (ck.daddr >> 16) & 0xff, (ck.daddr >> 24) & 0xff);
		sctplogtx(sp, " ck.saddr     = %d.%d.%d.%d", (ck.saddr >> 0) & 0xff,
			  (ck.saddr >> 8) & 0xff, (ck.saddr >> 16) & 0xff, (ck.saddr >> 24) & 0xff);
		sctplogtx(sp, " ck.dport     = %hu", ntohs(ck.dport));
		sctplogtx(sp, " ck.sport     = %hu", ntohs(ck.sport));
		sctplogtx(sp, " ck.v_tag     = %08x", ck.v_tag);
		sctplogtx(sp, " ck.p_tag     = %08x", ck.p_tag);
		sctplogtx(sp, " ck.p_tsn     = %u", ck.p_tsn);
		sctplogtx(sp, " ck.p_rwnd    = %u", ck.p_rwnd);
		sctplogtx(sp, " ck.n_istr    = %hu", ck.n_istr);
		sctplogtx(sp, " ck.n_ostr    = %hu", ck.n_ostr);
		sctplogtx(sp, " ck.l_caps    = %u", ck.l_caps);
		sctplogtx(sp, " ck.p_caps    = %u", ck.p_caps);
		sctplogtx(sp, " ck.l_ali     = %u", ck.l_ali);
		sctplogtx(sp, " ck.p_ali     = %u", ck.p_ali);
		sctplogtx(sp, " ck.danum     = %u", ck.danum);
	}
	/* RFC 2960 5.2.2 Note */
	ck.l_ttag = 0;
	ck.p_ttag = 0;
	/* SCTP IG 2.6 replacement: This IG section would have us do a hard lookup to return an
	   error.  We just lookup on IP/SCTP header information which means if a hijacker sends an
	   INIT with an extra address attempting to hijack a connection we will respond with an
	   INIT ACK but will not populate tie tags or initiate tag with existing tags.  This way
	   the tags of the existing association and information about the addresses for the
	   association are never exposed, yet we reconcile the situation properly when we receive
	   the COOKIE ECHO. */
	if ((oldsp = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))) {
		/* RFC 2960 5.2.1 and 5.2.2 Note */
		if (oldsp->state != SCTP_COOKIE_WAIT) {
			rare();
			ck.l_ttag = oldsp->v_tag;
			ck.p_ttag = oldsp->p_tag;
		}
		sctp_put(oldsp);
	}
	ck.opt_len = 0;
	if (sp->state == SCTP_LISTEN && !(sp->userlocks & SCTP_BINDADDR_LOCK)) {
		/* always include destination address */
		if (!sctp_saddr_include(sp, ck.saddr, &err) && err)
			goto no_resource;
		/* build a temporary address list */
		__sctp_get_addrs(sp, ck.daddr);
	}
	if (sp->sanum < 1)
		goto no_resource;
	ck.sanum = sp->sanum - 1;	/* don't include primary */
	sctplogtx(sp, " ck.sanum     = %u", ck.sanum);
	sctplogtx(sp, " ck.opt_len   = %u", ck.opt_len);
	sctp_send_init_ack(sp, iph->daddr, iph->saddr, sh, &ck, unrec);
      cleanup:
	if (sp->state == SCTP_LISTEN && !(sp->userlocks & SCTP_BINDADDR_LOCK))
		__sctp_free_saddrs(sp);	/* remove temporary address list */
      discard:
	if (unrec)
		freemsg(unrec);
	/* SCTP IG 2.2, 2.11 */
	return sctp_return_stop(mp);
      no_resource:
	err = -SCTP_CAUSE_NO_RESOURCE;
	errp = NULL;
	errl = 0;
	sctplogtx(sp, "couldn't allocate source addresses");
	goto error;
      bad_address:
	err = -SCTP_CAUSE_BAD_ADDRESS;
	errp = pptr;
	errl = plen;
	sctplogtx(sp, "unsupported address type - hostname or IPv6 address");
	goto error;
      invalid_parm:
	err = -SCTP_CAUSE_INVALID_PARM;
	errp = NULL;
	errl = 0;
	sctplogtx(sp, "missing initiate tag or zero streams requested");
	goto error;
      bad_parm:
	err = -SCTP_CAUSE_BAD_PARM;
	errp = pptr;
	errl = plen;
	sctplogtx(sp, "unrecognized or poorly formatted optional parameter");
	goto error;
      error:
	/* SCTP IG 2.23 Abort uses initiate tag as verification tag. */
	sctp_send_abort_error_init(sp, iph->saddr, iph->daddr, sh, m->i_tag, -err, errp, errl);
	goto cleanup;
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
      ebusy:
	err = -EBUSY;		/* FIXME: need to reenable. */
	goto return_error;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
      eproto:
	err = -EPROTO;
	goto return_error;
      emsgsize:
	err = -EMSGSIZE;
	goto return_error;
      enobufs:
	err = -ENOBUFS;		/* FIXME need to generate bufcall */
	goto return_error;
      return_error:
	if (unrec)
		freemsg(unrec);
	return (err);
}

/*
 *  RECV INIT ACK
 *  -------------------------------------------------------------------------
 *  We have recevied an INIT ACK in the SCTP_COOKIE_WAIT (TS_WCON_CREQ) state.
 *  (RFC 2960 5.2.3)
 */
STATIC int
sctp_recv_init_ack(struct sctp *sp, mblk_t *mp)
{
	int err = 0;
	struct iphdr *iph;
	struct sctp_init_ack *m;
	unsigned char *kptr = NULL;
	size_t klen = 0;
	unsigned char *pptr;
	unsigned char *pend;
	size_t plen;
	union sctp_parm *ph;

	sctplogrx(sp, "received INIT-ACK");
	assert(sp);
	assert(mp);
	if (sp->state != SCTP_COOKIE_WAIT)
		goto eproto;
	iph = SCTP_IPH(mp);
	if (!sctp_daddr_include(sp, iph->saddr, &err))
		goto addr_error;
	m = (typeof(m)) mp->b_rptr;
	/* SCTP IG 2.22 */
	if (!m->i_tag)
		goto invalid_parm;
	{
		size_t clen;

		if ((clen = ntohs(m->ch.len)) < sizeof(*m))
			goto emsgsize;
		/* SCTP IG 2.11 not allowed to bundle */
		if (PADC(clen) + mp->b_rptr < mp->b_wptr)
			goto eproto;
		pptr = (unsigned char *) (m + 1);
		pend = pptr + PADC(clen) - sizeof(*m);
	}
	ph = (typeof(ph)) pptr;
	{
		size_t ostr = ntohs(m->n_ostr);
		size_t istr = ntohs(m->n_istr);

		/* SCTP IG 2.5 */
		if (!ostr || !istr || ostr > sp->max_istr)
			goto invalid_parm;
		sp->p_rwnd = ntohl(m->a_rwnd);
		sp->p_rbuf = (sp->p_rbuf >= sp->p_rwnd) ? sp->p_rbuf : sp->p_rwnd;
		/* SCTP IG 2.19 correct */
		sp->n_ostr = ostr;
		sp->n_istr = istr;
		sp->r_ack =
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		    sp->p_fsn =
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ADD_IP
		    sp->p_asn =
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_ECN
		    sp->p_lsn =
#endif
		    ntohl(m->i_tsn) - 1;
	}
	sp->p_caps = 0;
	for (ph = (typeof(ph)) pptr;
	     pptr + sizeof(ph->ph) <= pend && pptr + (plen = ntohs(ph->ph.len)) <= pend;
	     pptr += PADC(plen), ph = (typeof(ph)) pptr) {
		uint type;

		if (plen < sizeof(ph->ph.type))
			goto proto_error;
		type = ph->ph.type;
		if (plen < sizeof(ph->ph))
			goto init_ack_bad_parm;
		switch (type) {
		case SCTP_PTYPE_IPV6_ADDR:
		case SCTP_PTYPE_HOST_NAME:
			goto bad_address;
		case SCTP_PTYPE_IPV4_ADDR:
			if (plen != sizeof(ph->ipv4_addr))
				goto init_ack_bad_parm;
			if (!sctp_daddr_include(sp, ph->ipv4_addr.addr, &err))
				goto addr_error;
			continue;
		case SCTP_PTYPE_STATE_COOKIE:
			kptr = pptr + sizeof(ph->ph);
			klen = plen - sizeof(ph->ph);
			continue;
#ifdef SCTP_CONFIG_ECN
		case SCTP_PTYPE_ECN_CAPABLE:
			if (plen != sizeof(ph->ecn_capable))
				goto init_ack_bad_parm;
			sp->p_caps |= SCTP_CAPS_ECN;
			INET_ECN_xmit(sp);
			break;
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
		case SCTP_PTYPE_ALI:
			if (plen != sizeof(ph->ali))
				goto init_ack_bad_parm;
			sp->p_ali = ntohl(((struct sctp_ali *) ph)->ali);
			sp->p_caps |= SCTP_CAPS_ALI;
			break;
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		case SCTP_PTYPE_PR_SCTP:
			if (plen != sizeof(ph->pr_sctp))
				goto init_ack_bad_parm;
			sp->p_caps |= SCTP_CAPS_PR;
			break;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
		case SCTP_PTYPE_UNREC_PARMS:
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
			/* IMPLEMENTATION NOTE:- We really don't care about any unrecognized
			   parameters that are returned.  This is because any unrecognized
			   parameters (such as ECN, ADD-IP and PR-SCTP) have a positive response
			   and the lack of a positive response indicates a negative response. */
			break;
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
		default:
		      init_ack_bad_parm:
			/* SCTP IG 2.2, 2.27 */
			if (!(type & SCTP_PTYPE_MASK_CONTINUE)) {
				if (type & SCTP_PTYPE_MASK_REPORT)
					goto bad_parm;
				goto discard;
			}
			if (type & SCTP_PTYPE_MASK_REPORT)
				sctp_send_error(sp, SCTP_CAUSE_BAD_PARM, pptr, plen);
		}
	}
	if (!kptr)		/* no cookie? */
		goto missing_parm;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->prel == SCTP_PR_REQUIRED && !(sp->p_caps & SCTP_CAPS_PR))
		/* require partial reliabilty support :- abort the init process */
		goto proto_error;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	sctp_ack_calc(sp, &sp->timer_init);
	local_bh_disable();
	/* not an error to be in the phashes */
	if (sp->pprev)
		__sctp_phash_unhash(sp);
	sp->p_tag = m->i_tag;
	__sctp_phash_insert(sp);
	local_bh_enable();
	sctp_send_cookie_echo(sp, kptr, klen);
	return sctp_return_stop(mp);
      discard:
	bufq_purge(&sp->errq);
	return sctp_return_stop(mp);
      eproto:
	sctplogrx(sp, "%s(): incorrect state or bundled INIT-ACK", __FUNCTION__);
	err = -EPROTO;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	sctplogrx(sp, "%s(): bad chunk or parameter length", __FUNCTION__);
	goto error;
      addr_error:
	switch (err) {
	case -EDESTADDRREQ:
	case -EADDRNOTAVAIL:
		goto bad_address;
	}
      error:
	rare();
	return (err);		/* fall back on timer init */
      proto_error:
	err = SCTP_CAUSE_PROTO_VIOLATION;
	sctplogrx(sp, "%s(): protocol violation", __FUNCTION__);
	goto disconnect;
#if 0
      no_resource:
	err = SCTP_CAUSE_NO_RESOURCE;
	sctplogrx(sp, "%s(): could not allocate destination addresses", __FUNCTION__);
	goto disconnect;
#endif
      bad_address:
	err = SCTP_CAUSE_BAD_ADDRESS;
	sctplogrx(sp, "%s(): unsupported address type - hostname or IPv6 address", __FUNCTION__);
	goto disconnect;
      invalid_parm:
	err = SCTP_CAUSE_INVALID_PARM;
	sctplogrx(sp, "%s(): missing initiate tag or invalid number of streams requested", __FUNCTION__);
	goto disconnect;
      bad_parm:
	err = SCTP_CAUSE_BAD_PARM;
	sctplogrx(sp, "%s(): unrecognized or poorly formatted optional parameter", __FUNCTION__);
	goto disconnect;
      missing_parm:
	err = SCTP_CAUSE_MISSING_PARM;
	sctplogrx(sp, "%s(): missing mandatory (state cookie) parameter", __FUNCTION__);
	goto disconnect;
      disconnect:
	/* abort the init process */
	sctplogrx(sp, "%s(): abort the init process", __FUNCTION__);
	sctp_abort(sp, SCTP_ORIG_PROVIDER, err);
	return (-ECONNABORTED);
}

/*
 *  RECV COOKIE ECHO
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ECHO for a socket or stream.  We have already determined the socket or
 *  stream to which the COOKIE ECHO applies.  We must still verify the cookie and apply the
 *  appropriate action per RFC 2960 5.2.4.
 */
STATIC void sctp_reset(struct sctp *sp);
STATIC void sctp_clear(struct sctp *sp);
STATIC int
sctp_update_from_cookie(struct sctp *sp, struct sctp_cookie *ck)
{
	int err = 0;
	uint32_t *daddrs = (uint32_t *) (ck + 1);
	uint32_t *saddrs = daddrs + ck->danum;

	if (!(sp->userlocks & SCTP_BINDADDR_LOCK)) {
		if (!sctp_saddr_include(sp, ck->saddr, &err) && err)
			goto error;
		if ((err = sctp_alloc_saddrs(sp, ck->sport, saddrs, ck->sanum)))
			goto error;
	}
	if (!sctp_daddr_include(sp, ck->daddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_daddrs(sp, ck->dport, daddrs, ck->danum)) < 0)
		goto error;
	if ((err = sctp_conn_hash(sp)))
		goto error;
	if ((err = sctp_update_routes(sp, 1)))
		goto error;
	sp->inet.id = ck->v_tag ^ jiffies;
	sp->n_istr = ck->n_istr;
	sp->n_ostr = ck->n_ostr;
	sp->l_caps = ck->l_caps;
	sp->p_caps = ck->p_caps;
	sp->l_ali = ck->l_ali;
	sp->p_ali = ck->p_ali;
	sp->t_tsn = ck->v_tag;
	sp->t_ack =
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    sp->l_fsn =
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	    sp->m_tsn =
#ifdef SCTP_CONFIG_ADD_IP
	    sp->l_asn =
#endif				/* SCTP_CONFIG_ADD_IP */
	    ck->v_tag - 1;
	sp->r_ack =
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    sp->p_fsn =
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ADD_IP
	    sp->p_asn =
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_ECN
	    sp->p_lsn =
#endif				/* SCTP_CONFIG_ECN */
	    ck->p_tsn - 1;
	sp->p_rwnd = ck->p_rwnd;
	sp->p_rbuf = ck->p_rwnd;
	return (0);
      error:
	return (err);
}
STATIC void sctp_unhash(struct sctp *sp);
STATIC int
sctp_recv_cookie_echo(struct sctp *sp, mblk_t *mp)
{
	int err;
	struct sctp_cookie_echo *m;
	struct sctp_cookie *ck;

#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	static ulong last_cookie_echo = 0;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
	sctplogrx(sp, "received COOKIE-ECHO");
	assert(sp);
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	if (last_cookie_echo && jiffies < last_cookie_echo + sp->throttle)
		return (-EBUSY);
	last_cookie_echo = jiffies;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
	assert(mp);
	m = (typeof(m)) mp->b_rptr;
	{
		size_t clen = ntohs(m->ch.len);

		if (clen < sizeof(*m) + sizeof(*ck))
			goto emsgsize;
		ck = (typeof(ck)) m->cookie;
		if (clen < sizeof(*m) + raw_cookie_size(ck) + HMAC_SIZE)
			goto emsgsize;
	}
	/* RFC 2960 5.2.4 (1) & (2) */
	if ((err = sctp_verify_cookie(sp, ck))) {
		rare();
		return (err);
	}
	if (sp->v_tag) {
		if (ck->v_tag != sp->v_tag) {
			if (ck->p_tag != sp->p_tag) {
				if (ck->l_ttag == sp->v_tag && ck->p_ttag == sp->p_tag)
					/* RFC 2960 5.2.4. Action (A) */
					goto recv_cookie_echo_action_a;
				if (ck->l_ttag == 0 && ck->p_ttag == 0)
					goto recv_cookie_echo_action_p;
			} else if (ck->l_ttag == 0 && ck->p_ttag == 0)
				/* RFC 2960 5.2.4. Action (C). */
				goto recv_cookie_echo_action_c;
		} else {
			if (!sp->p_tag
			    || ((1 << sp->state) & (SCTPF_COOKIE_WAIT | SCTPF_COOKIE_ECHOED)))
				/* RFC 2960 5.2.4 Action (B). */
				goto recv_cookie_echo_action_b;
			else if (ck->p_tag != sp->p_tag)
				/* RFC 2960 5.2.4 Action (B) */
				goto recv_cookie_echo_action_b;
			else
				/* RFC 2960 5.2.4 Action (D). */
				goto recv_cookie_echo_action_d;
		}
	} else
		/* RFC 2960 5.1 */
		goto recv_cookie_echo_listen;
	rare();
	/* RFC 2960 5.2.4 ...silently discarded */
	return (0);
      recv_cookie_echo_action_a:
	sctplogrx(sp, "performing cookie echo action (A)");
	rare();
	/* 
	 *  RFC 2960 5.2.4 Action (A)
	 *
	 *  In this case, the peer may have restarted.  When the endpoint recognizes this potential
	 *  'restart', the existing session is treated the same as if it received an ABORT followed
	 *  by a new COOKIED ECHO with the following exceptions:  - Any SCTP DATA Chunks MAY be
	 *  retained (this is an implementation specific option).  - A notification of RESTART
	 *  SHOULD be sent to the ULP instead of "COMMUNICATION LOST" notification.
	 *
	 *  All the Congestion control parameters (e.g., cwnd, ssthresh) related to this peer MUST
	 *  be reset to their initial values (see Section 6.2.1).  After this the endpoint shall
	 *  enter the ESTABLISHED state.
	 *
	 *  If the endpoint is in the SHUTDOWN-ACK-SENT state and recognizes the peer has restarted
	 *  (Action A), it MUST NOT setup a new association but instead resend the SHUTDOWN ACK and
	 *  send an ERROR chunk with a "Cookie Received while Shutting Down" error cause to its
	 *  peer.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan)
		goto stale_cookie;
	switch (sp->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		rare();
		/* RFC 2960 5.2.4 (A) */
		sctp_send_abort_error(sp, SCTP_CAUSE_SHUTDOWN, NULL, 0);
		sctp_send_shutdown_ack(sp);
		return sctp_return_stop(mp);
	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
		rare();
		/* RFC 2960 5.2.4 (A) */
		sctp_send_abort(sp);	/* abort old association */
		if (sp->rq) {
			sctp_reset(sp);
			sp->v_tag = ck->v_tag;
			sp->p_tag = ck->p_tag;
			if ((err = sctp_update_from_cookie(sp, ck)))
				goto error_abort;
			sctp_change_state(sp, SCTP_ESTABLISHED);
			/* Notify user of reset or disconnect */
			if ((err = sctp_reset_ind(sp, SCTP_ORIG_USER, -ECONNRESET, mp)))
				return (err);
			/* Yes, data (and other chunks) can be bundled with COOKIE-ECHO.  This
			   processes them. */
			if (sctp_return_more(mp) > 0)
				sctp_recv_msg_slow(sp, mp);
			/* Wait to send COOKIE-ACK until we've processed the reset of the packet:
			   there might be some other things to bundle with the COOKIE-ACK. */
			sctp_send_cookie_ack(sp);
			return (0);
		}
		sctp_abort(sp, SCTP_ORIG_PROVIDER, -ECONNRESET);
		return (-ECONNRESET);
	default:
		sctplogerr(sp, "%s() wrong state for action", __FUNCTION__);
		return (-EFAULT);
	}
	never();
      recv_cookie_echo_action_b:
	sctplogrx(sp, "performing cookie echo action (B)");
	rare();
	/* 
	 *  RFC 2960 5.2.4 Action (B)
	 *
	 *  In this case, both sides may be attempting to start and association at about the same
	 *  time but the peer endpoint started its INIT after responding to the local endpoint's
	 *  INIT.  Thus it may have picked a new Verification Tag not being aware of the previous
	 *  Tag it had sent this endpoint.  The endpoint should stay in or enter the ESTABLISHED
	 *  state but it MUST update its peer's Verification Tag from the State Cookie, stop any
	 *  init or cookie timers that may be running and send a COOKIE ACK.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan)
		goto stale_cookie;
	switch (sp->state) {
	case SCTP_COOKIE_WAIT:
		sp_timer_cancel_sack(sp);
		/* fall through */
	case SCTP_COOKIE_ECHOED:
		sp_timer_cancel_cookie(sp);
		/* We haven't got an INIT ACK yet so we need some stuff from the cookie. */
		sctp_unhash(sp);
		if ((err = sctp_update_from_cookie(sp, ck)))
			goto error_abort;
		/* fall through */
	case SCTP_ESTABLISHED:
		break;
	default:
		sctplogerr(sp, "%s() wrong state for action", __FUNCTION__);
		return (-EFAULT);
	}
	local_bh_disable();
	if (sp->pprev)
		__sctp_phash_unhash(sp);
	sp->p_tag = ck->p_tag;
	__sctp_phash_insert(sp);
	local_bh_enable();
	sctp_send_sack(sp);
	goto recv_cookie_echo_action_d;
      recv_cookie_echo_action_c:
	sctplogrx(sp, "performing cookie echo action (C)");
	rare();
	/* 
	 *  RFC 2960 5.2.4 Action (C)
	 *
	 *  In this case, the local endpoint's cookie has arrived late.  Before it arrived, the
	 *  local sendpoint sent an INIT and receive an INIT-ACK and finally sent a COOKIE ECHO with
	 *  the peer's same tag but a new tag of its own.  The cookie should be silently discarded.
	 *  The endpoint SHOULD NOT change states and should leave any timers running.
	 */
	rare();
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan)
		goto stale_cookie;
	return (0);
      recv_cookie_echo_action_d:
	sctplogrx(sp, "performing cookie echo action (D)");
	/* 
	 *  RFC 2960 5.2.4 Action (D)
	 *
	 *  When both local and remote tags match the endpoint should always enter the ESTABLISHED
	 *  state, it if has not already done so.  It should stop any init or cookie timers that may
	 *  be running and send a COOKIE ACK.
	 */
	/* SCTP IG 2.29 */
	switch (sp->state) {
	case SCTP_COOKIE_WAIT:
		sp_timer_cancel_sack(sp);
		/* fall through */
	case SCTP_COOKIE_ECHOED:
		sp_timer_cancel_cookie(sp);
		/* will change state to established */
		if ((err = sctp_conn_con(sp)))
			return (err);
		/* fall through */
	case SCTP_ESTABLISHED:
		/* process data bundled with cookie echo */
		if (sctp_return_more(mp) > 0)
			sctp_recv_msg_slow(sp, mp);
		sctp_send_cookie_ack(sp);
		return (0);
	default:
		rare();
		/* RFC 2960 5.2.4 ...silently discarded */
		return (0);
	}
	never();
      recv_cookie_echo_listen:
	sctplogrx(sp, "performing cookie echo (LISTEN)");
	if (sp->conind) {
		mblk_t *cp;
		pl_t pl;

		/* check for existing connection indication */
		pl = bufq_lock(&sp->conq);
		for (cp = bufq_head(&sp->conq); cp; cp = cp->b_next) {
			struct sctp_cookie_echo *ce = (typeof(ce)) cp->b_rptr;
			struct sctp_cookie *co = (typeof(co)) ce->cookie;

			/* FIXME: we should check a little harder for cookie echo matches. */
			if (co->v_tag == ck->v_tag
			    || (co->dport == ck->dport && co->sport == ck->sport
				&& co->daddr == ck->daddr && co->saddr == ck->saddr)) {
				sctplogrx(sp, "discarding multiple COOKIE-ECHO");
				bufq_unlock(&sp->conq, pl);
				return (0);	/* discard multiple */
			}
		}
		bufq_unlock(&sp->conq, pl);
		/* RFC 2960 5.2.4 (4) */
		if ((err = sctp_conn_ind(sp, mp)))
			goto no_resource;
		return (0);
	}
	return (0);
      recv_cookie_echo_action_p:
	sctplogrx(sp, "performing cookie echo action (P)");
	/* 
	 *  SCTP IG 2.6 replacement:
	 *
	 *  In this case we may have refused to divulge existing tags to a potential attacker and
	 *  poisoned the cookie and the peer did the same to us or we have not received an INIT-ACK
	 *  yet.  The INIT address is now known to us, we will process the cookie as though we had
	 *  handed out the verification tag in the initiate tag.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan)
		goto stale_cookie;
	switch (sp->state) {
	case SCTP_COOKIE_WAIT:
		local_bh_disable();
		__sctp_vhash_unhash(sp);
		sp->v_tag = ck->v_tag;
		__sctp_vhash_insert(sp);
		local_bh_enable();
		goto recv_cookie_echo_action_b;
	case SCTP_COOKIE_ECHOED:
		ck->v_tag = sp->v_tag;
		goto recv_cookie_echo_action_b;
	}
	rare();
	goto no_resource;
	return (0);
      error_abort:
	{
		struct iphdr *iph = SCTP_IPH(mp);
		struct sctphdr *sh = SCTP_SH(mp);

		/* We had a fatal error processing the COOKIE ECHO and must abort the association. */
		sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		sctp_abort(sp, SCTP_ORIG_PROVIDER, err);
		return (err);
	}
      emsgsize:
	err = -EMSGSIZE;
	return (err);
      stale_cookie:
	{
		uint32_t staleness;
		struct iphdr *iph = SCTP_IPH(mp);
		struct sctphdr *sh = SCTP_SH(mp);

		rare();
		staleness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
      no_resource:
	{
		struct iphdr *iph = SCTP_IPH(mp);
		struct sctphdr *sh = SCTP_SH(mp);

		seldom();
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
					   SCTP_CAUSE_NO_RESOURCE, NULL, 0);
		if (err == -ERESTART)
#ifdef HAVE_KINC_LINUX_SNMP_H
			NET_INC_STATS_BH(LINUX_MIB_LISTENOVERFLOWS);
#else
			NET_INC_STATS_BH(ListenOverflows);
#endif
		return (err);
	}
}

/*
 *  RECV COOKIE ACK
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ACK.  If we are in COOKIE ECHOED state then we inform the user
 *  interface that the previous connection request is confirmed, cancel the cookie timer while
 *  performing an RTO calculation on the message and enter the ESTABLISHED state.  Any DATA that is
 *  bundled with the COOKIE ACK will be separately indicated to the user with data indications.  In
 *  state other than COOKIE ECHOED the entire message is silently discarded.
 */
STATIC int
sctp_recv_cookie_ack(struct sctp *sp, mblk_t *mp)
{
	int err;
	struct sctp_daddr *sd;
	struct sctp_cookie_ack *m = (typeof(m)) mp->b_rptr;

	sctplogrx(sp, "received COOKIE-ACK");
	assert(sp);
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	switch (sp->state) {
	case SCTP_COOKIE_ECHOED:
		/* RFC 2960 5.1 (E) */
		/* will change state to established */
		if ((err = sctp_conn_con(sp)))
			return (err);
		sctp_ack_calc(sp, &sp->timer_cookie);
		SCTP_INC_STATS_BH(SctpActiveEstabs);
		/* start idle timers */
		usual(sp->daddr);
		for (sd = sp->daddr; sd; sd = sd->next)
			sctp_reset_idle(sd);
		return sctp_return_more(mp);
	default:
		/* RFC 2960 5.2.5 */
		rare();
		break;
	}
	return (0);		/* silently discard */
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV HEARTBEAT
 *  -------------------------------------------------------------------------
 *  We have received a HEARTBEAT.  Quite frankly we don't care what state we are in, we take the
 *  heartbeat info and turn it back around as a HEARTBEAT ACK msg.  We do a little bit of error
 *  checking here to make sure that we are not wasting our time on the packet.  We should only
 *  receive HEARTBEAT messages if we are in the vtag hashes: meaning that we will accept and reply
 *  to a HEARTBEAT in any state other than CLOSED and LISTEN.
 */
STATIC int
sctp_recv_heartbeat(struct sctp *sp, mblk_t *mp)
{
	int err;
	struct sctp_heartbeat *m = (typeof(m)) mp->b_rptr;
	struct sctpphdr *ph = (typeof(ph)) (m + 1);
	size_t clen = ntohs(m->ch.len);
	size_t mlen = sizeof(*m) + sizeof(*ph);
	size_t hlen;

	sctplogrx(sp, "received HEARTBEAT");
	if (clen < mlen || mp->b_wptr < mp->b_rptr + mlen)
		goto emsgsize;
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
	{
		if (jiffies < sp->hb_rcvd + sp->hb_tint)
			goto ebusy;
		sp->hb_rcvd = jiffies;
	}
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
	if (ph->type != SCTP_PTYPE_HEARTBEAT_INFO)
		goto eproto;
	hlen = PADC(ntohs(ph->len));
	if (hlen < sizeof(*ph) || hlen > clen - sizeof(*m))
		goto emsgsize;
	sctp_send_heartbeat_ack(sp, (caddr_t) ph, min(PADC(clen), hlen));
	return sctp_return_stop(mp);
      eproto:
	err = -EPROTO;
	sctplogrx(sp, "%s(): bad message", __FUNCTION__);
	goto error;
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
      ebusy:
	err = -EBUSY;
	sctplogrx(sp, "%s(): throttling heartbeat", __FUNCTION__);
	goto error;
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
      emsgsize:
	err = -EMSGSIZE;
	sctplogrx(sp, "%s(): bad message size", __FUNCTION__);
	goto error;
      error:
	return (err);
}

/*
 *  RECV HEARTBEAT ACK
 *  -------------------------------------------------------------------------
 *  This is our HEARTBEAT coming back.  We check that the HEARTBEAT information matches the
 *  information of the last sent HEARTBEAT message to ensure that no fiddling with the HEARTBEAT
 *  info has occured.  The only information we trust initially is the destination address which is
 *  contained in the HEARTBEAT INFO.  This just helps us index the remainder of the hearbeat
 *  information.
 *
 *  We have received a HEARTBEAT ACK message on an ESTABLISHED socket or stream.  Perform the
 *  procedures from RFC 2960 8.3.
 *
 *  TODO: SCTP IG 2.36 - Need to verify nonce and verify heartbeats for all unverified destinations.
 */
STATIC int
sctp_recv_heartbeat_ack(struct sctp *sp, mblk_t *mp)
{
	int err;
	struct sctp_daddr *sd;
	struct sctp_heartbeat_ack *m = (typeof(m)) mp->b_rptr;
	struct sctp_heartbeat_info *hb = (typeof(hb)) (m + 1);

	sctplogrx(sp, "received HEARTBEAT-ACK");
	if (ntohs(m->ch.len) < sizeof(*m) + sizeof(*hb))
		goto emsgsize;
	if (!(sd = sctp_find_daddr(sp, hb->hb_info.daddr)))
		goto badaddr;
	if (ntohs(hb->ph.len) != sizeof(*hb) + sd->hb_fill)
		goto emsgsize;
	if (hb->ph.type != SCTP_PTYPE_HEARTBEAT_INFO)
		goto eproto;
	if (!sd->hb_time || sd->hb_time != hb->hb_info.timestamp)
		goto einval;
	sd_timer_cancel_heartbeat(sd);
	sctp_rtt_calc(sd, sd->hb_time);
      ignore:
	return sctp_return_stop(mp);
      einval:
	err = -EINVAL;
	sctplogrx(sp, "%s(): old or fiddled timestamp", __FUNCTION__);
	goto ignore;
      eproto:
	err = -EPROTO;
	sctplogrx(sp, "%s(): bad heartbeat parameter type", __FUNCTION__);
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	sctplogrx(sp, "%s(): bad heartbeat parameter or chunk size", __FUNCTION__);
	goto error;
      badaddr:
	sctplogrx(sp, "%s(): bad destination parameter", __FUNCTION__);
	goto ignore;
      error:
	return (err);
}

/*
 *  RECV SHUTDOWN
 *  -------------------------------------------------------------------------
 */
STATIC void
sctp_shutdown_ack_calc(struct sctp *sp, uint32_t ack)
{
	if (before(ack, sp->t_ack)) {
		rare();
		/* If the SHUTDOWN acknowledges our sent data chunks that have already been
		   acknowledged, then it is an old (or erroneous) message and we will ignore it. */
		return;
	}
	if (after(ack, sp->t_ack))
		sctp_cumm_ack(sp, ack);
	sctp_dest_calc(sp);
}
STATIC int
sctp_recv_shutdown(struct sctp *sp, mblk_t *mp)
{
	int err;
	struct sctp_shutdown *m = (typeof(m)) mp->b_rptr;
	uint32_t ack = ntohl(m->c_tsn);

	sctplogrx(sp, "received SHUTDOWN");
	assert(sp);
	if ((1 << sp->state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	if (sp->ngaps) {
		rare();
		/* Check sanity of sender: if we have gaps in our acks to the peer and the peer
		   sends a SHUTDOWN, then it is in error. The peer cannot send SHUTDOWN when it has 
		   unacknowledged data. If this is the case, we zap the connection. */
		sctp_send_abort_error(sp, SCTP_CAUSE_PROTO_VIOLATION, NULL, 0);
		sctp_abort(sp, SCTP_ORIG_PROVIDER, -EPIPE);
		return sctp_return_stop(mp);
	}
	switch (sp->state) {
	case SCTP_ESTABLISHED:
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sp)))
			goto error;
		sctp_change_state(sp, SCTP_SHUTDOWN_RECEIVED);
		/* fall thru */
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_shutdown_ack_calc(sp, ack);
		if (!sp->ops->ordrel_ind)
			if (!bufq_head(&sp->sndq) && !bufq_head(&sp->urgq) && !bufq_head(&sp->rtxq))
				sctp_send_shutdown_ack(sp);
		break;
	case SCTP_SHUTDOWN_PENDING:	/* only when we have ordrel */
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sp)))
			goto error;
		sctp_change_state(sp, SCTP_SHUTDOWN_RECVWAIT);
		/* fall thru */
	case SCTP_SHUTDOWN_RECVWAIT:
		sctp_shutdown_ack_calc(sp, ack);
		if (!bufq_head(&sp->sndq) && !bufq_head(&sp->urgq) && !bufq_head(&sp->rtxq))
			sctp_send_shutdown_ack(sp);
		break;
	case SCTP_SHUTDOWN_SENT:	/* only when we have ordrel */
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sp)))
			goto error;
		sctp_shutdown_ack_calc(sp, ack);
		/* fail thru */
	case SCTP_SHUTDOWN_ACK_SENT:
		sctp_send_shutdown_ack(sp);	/* We do this */
		break;
	default:
	      outstate:
		/* ignore the SHUTDOWN chunk */
		rare();
		break;
	}
	return sctp_return_more(mp);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	rare();
	return (err);
}

/*
 *  RECV SHUTDOWN ACK
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN ACK chunk through the vtag hashes.  We are expecting the SHUTDOWN
 *  ACK because we have previously sent a SHUTDOWN or SHUTDOWN ACK or we ignore the message.
 */
STATIC int
sctp_recv_shutdown_ack(struct sctp *sp, mblk_t *mp)
{
	int err;
	struct sctp_shutdown_ack *m = (typeof(m)) mp->b_rptr;

	sctplogrx(sp, "received SHUTDOWN-ACK");
	assert(sp);
	assert(mp);
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	switch (sp->state) {
	case SCTP_SHUTDOWN_SENT:
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sp)))
			goto error;
		/* fall thru */
	case SCTP_SHUTDOWN_ACK_SENT:
#if 0
		sctp_ack_calc(sp, &sp->timer_shutdown);	/* WHY? */
#endif
		sctp_send_shutdown_complete(sp);
		SCTP_INC_STATS_BH(SctpShutdowns);
		sctp_reset(sp);
		break;
	default:
		/* ignore unexpected SHUTDOWN ACK */
		rare();
		break;
	}
	return sctp_return_stop(mp);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	rare();
	return (err);
}

/*
 *  RECV SHUTDOWN COMPLETE
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN COMPLETE in the SHUTDOWN ACK SENT state.  This is the normal path
 *  for shutting down an SCTP association.  Outstanding data has already been processed.  Remove
 *  ourselves from the hashes and process any backlog.  RFC 2960 6.10: SHUTDOWN COMPLETE cannot have
 *  any other chunks bundled with them.
 *
 *  We receive SHUTDOWN COMPLETE chunks through the vtag hashes.  We are expecting SHUTDOWN COMPLETE
 *  because we sent a SHUTDOWN ACK.  We are in the TS_IDLE, NS_IDLE states but are still in the vtag
 *  hashes.
 */
STATIC int
sctp_recv_shutdown_complete(struct sctp *sp, mblk_t *mp)
{
	struct sctp_shutdown_comp *m = (typeof(m)) mp->b_rptr;

	sctplogrx(sp, "received SHUTDOWN-COMPLETE");
	assert(sp);
	assert(mp);
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	switch (sp->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
#if 0
		sctp_ack_calc(sp, &sp->timer_shutdown);	/* WHY? */
#endif
		SCTP_INC_STATS_BH(SctpShutdowns);
		sctp_reset(sp);
		break;
	default:
		/* ignore unexpected SHUTDOWN COMPLETE */
		rare();
		break;
	}
	return sctp_return_stop(mp);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV UNRECOGNIZED CHUNK TYPE
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_unrec_ctype(struct sctp *sp, mblk_t *mp)
{
	struct sctpchdr *ch = SCTP_CH(mp);
	uint8_t ctype;

	sctplogrx(sp, "received unrecognized chunk type");
	if (ntohs(ch->len) < sizeof(*ch))
		goto emsgsize;
	ctype = ch->type;
	if (ctype & SCTP_CTYPE_MASK_REPORT)
		sctp_send_error(sp, SCTP_CAUSE_BAD_CHUNK_TYPE, mp->b_rptr, mp->b_wptr - mp->b_rptr);
	if (ctype & SCTP_CTYPE_MASK_CONTINUE)
		return sctp_return_more(mp);
	return (0);		/* discard packet */
      emsgsize:
	return (-EMSGSIZE);
}

#ifdef SCTP_CONFIG_ECN
/*
 *  RECV ECNE (Explicit Contestion Notification Echo)
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_ecne(struct sctp *sp, mblk_t *mp)
{

	struct sctp_ecne *m = (typeof(m)) mp->b_rptr;
	uint32_t l_tsn = ntohl(m->l_tsn);
	struct sctp_daddr *sd;
	pl_t pl;

	sctplogrx(sp, "received ECNE");
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	l_tsn = ntohl(m->l_tsn);
	sp->p_caps |= SCTP_CAPS_ECN;
	INET_ECN_xmit(sp);
	if (!after(l_tsn, sp->p_lsn))
		goto done;
	sp->p_lsn = l_tsn;
	/* need to find the destination to which this TSN was transmitted */
	pl = bufq_lock(&sp->rtxq);
	for (mp = bufq_head(&sp->rtxq); mp && SCTP_TCB(mp)->tsn != l_tsn; mp = mp->b_next) ;
	if (!mp)
		goto unlock_done;
	if (!(sd = SCTP_TCB(mp)->daddr))
		goto unlock_done;
	bufq_unlock(&sp->rtxq, pl);
	sctp_assoc_timedout(sp, sd, 0, 0);
      done:
	return sctp_return_more(mp);
      unlock_done:
	bufq_unlock(&sp->rtxq, pl);
	goto done;
      emsgsize:
	return (-EMSGSIZE);
}
#endif				/* SCTP_CONFIG_ECN */

#ifdef SCTP_CONFIG_ECN
/*
 *  RECV CWR (Congestion Window Reduction)
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_cwr(struct sctp *sp, mblk_t *mp)
{

	struct sctp_cwr *m = (typeof(m)) mp->b_rptr;
	uint32_t l_tsn;

	sctplogrx(sp, "received CWR");
	(void) l_tsn;
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	l_tsn = ntohl(m->l_tsn);
	fixme(("FIXME: Finish this function\n"));
	sp->sackf &= ~SCTP_SACKF_ECN;
	return sctp_return_more(mp);
      emsgsize:
	return (-EMSGSIZE);
}
#endif				/* SCTP_CONFIG_ECN */

#ifdef SCTP_CONFIG_ADD_IP
/*
 *  RECV ASCONF
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_asconf(struct sctp *sp, mblk_t *mp)
{

	struct sctp_asconf *m = (typeof(m)) mp->b_rptr;
	uint32_t asn;
	struct sctp_daddr *sd;
	int reconfig = 0;

	sctplogrx(sp, "received ASCONF");
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	if (!(sp->l_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto refuse;
	/* ADD-IP 4.1.1 R4 */
	if (sp->state != SCTP_ESTABLISHED)
		goto discard;
	asn = ntohl(m->asn);
	if (mp->b_wptr < mp->b_rptr + sizeof(*m))
		goto emsgsize;
	if (asn == sp->p_asn + 1) {
		/* ADD-IP 4.2 Rule C1 & C2 */
		struct sctpehdr *eh;
		struct sctp_success_report *sr;
		struct sctp_error_cause *er;
		struct sctp_daddr *sd;
		union sctp_parm *ph;
		struct sctp_ipv4_addr *a;
		unsigned char *pptr = (unsigned char *) (m + 1);
		unsigned char *pend = pptr + ntohs(m->ch.len) - sizeof(*m);
		unsigned char *rptr;
		unsigned char *bptr;
		size_t rlen = 0;
		size_t plen;

		/* process the ASCONF chunk */
		for (ph = (union sctp_parm *) pptr;
		     pptr + sizeof(ph->ph) <= pend && pptr + (plen = ntohs(ph->ph.len)) <= pend;
		     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
			uint type;

			if (plen < sizeof(ph->ph))
				return -EPROTO;
			switch ((type = ph->ph.type)) {
			case SCTP_PTYPE_ADD_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					goto bad_parm;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					goto bad_parm;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					goto bad_parm;
				if ((sd = sctp_find_daddr(sp, ntohl(a->addr)))) {
					/* fake success */
					rlen += sizeof(*sr);
					continue;
				}
				if ((sd = sctp_add_daddr(sp, ntohl(a->addr)))) {
					rlen += sizeof(*sr);
					reconfig++;
					continue;
				}
				/* resources or scope error */
				rlen += sizeof(*er) + sizeof(*eh) + plen;
				continue;
			case SCTP_PTYPE_DEL_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					goto bad_parm;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					goto bad_parm;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					goto bad_parm;
				if (!(sd = sctp_find_daddr(sp, ntohl(a->addr)))) {
					/* fake success */
					rlen += sizeof(*sr);
					continue;
				}
				if (a->addr != SCTP_IPH(mp)->saddr && sp->danum != 1) {
					sctp_del_daddr(sd);
					rlen += sizeof(*sr);
					reconfig++;
					continue;
				}
				/* request to delete source address */
				/* last address need to send error */
				rlen += sizeof(*er) + sizeof(*eh) + plen;
				continue;
			case SCTP_PTYPE_SET_IP:
				if (!(sp->l_caps & SCTP_CAPS_SET_IP))
					goto bad_parm;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					goto bad_parm;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					goto bad_parm;
				if (!(sd = sctp_find_daddr(sp, ntohl(a->addr))))
					goto bad_parm;
				/* fake success */
				rlen += sizeof(*sr);
				continue;
			default:
			      bad_parm:
				rare();
				if (type & SCTP_PTYPE_MASK_REPORT)
					sctp_send_error(sp, SCTP_CAUSE_BAD_PARM, (caddr_t) ph,
							plen);
				if (type & SCTP_PTYPE_MASK_CONTINUE)
					continue;
				/* SCTP IG 2.2 */
				goto discard;
			}
		}
		/* second pass to build response */
		if (!(bptr = rptr = kmalloc(rlen, GFP_ATOMIC)))
			return -ENOMEM;
		for (pptr = (unsigned char *) (m + 1), rlen = 0, ph = (union sctp_parm *) pptr;
		     pptr + sizeof(ph->ph) <= pend && pptr + (plen = ntohs(ph->ph.len)) <= pend;
		     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
			uint type;

			switch ((type = ph->ph.type)) {
			case SCTP_PTYPE_ADD_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					continue;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					continue;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					continue;
				if ((sd = sctp_find_daddr(sp, ntohl(a->addr)))) {
					sr = (typeof(sr)) bptr;
					sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
					sr->ph.len = __constant_htons(sizeof(*sr));
					sr->cid = ph->add_ip.id;
					bptr += sizeof(*sr);
					rlen += sizeof(*sr);
					continue;
				} else {
					er = (typeof(er)) bptr;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh) + plen);
					er->cid = ph->del_ip.id;
					bptr += sizeof(*er);
					eh = (typeof(eh)) bptr;
					eh->code = __constant_htons(SCTP_CAUSE_RES_SHORTAGE);
					eh->len = htons(sizeof(*eh) + plen);
					bptr += sizeof(*eh);
					bcopy(pptr, bptr, plen);
					bptr += plen;
					rlen += sizeof(*er) + sizeof(*eh) + plen;
					continue;
				}
				break;
			case SCTP_PTYPE_DEL_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					continue;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					continue;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					continue;
				if (!(sd = sctp_find_daddr(sp, ntohl(a->addr)))) {
					sr = (typeof(sr)) bptr;
					sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
					sr->ph.len = __constant_htons(sizeof(*sr));
					sr->cid = ph->add_ip.id;
					bptr += sizeof(*sr);
					rlen += sizeof(*sr);
					continue;
				}
				if (a->addr == SCTP_IPH(mp)->saddr) {
					/* request to delete source address */
					er = (typeof(er)) bptr;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh) + plen);
					er->cid = ph->del_ip.id;
					bptr += sizeof(*er);
					eh = (typeof(eh)) bptr;
					eh->code = __constant_htons(SCTP_CAUSE_SOURCE_ADDR);
					eh->len = htons(sizeof(*eh) + plen);
					bptr += sizeof(*eh);
					bcopy(pptr, bptr, plen);
					bptr += plen;
					rlen += sizeof(*er) + sizeof(*eh) + plen;
					continue;
				}
				if (sp->danum == 1) {
					/* last address need to send error */
					er = (typeof(er)) bptr;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh)
							   + plen);
					er->cid = ph->del_ip.id;
					bptr += sizeof(*er);
					eh = (typeof(eh)) bptr;
					eh->code = __constant_htons(SCTP_CAUSE_LAST_ADDR);
					eh->len = htons(sizeof(*eh) + plen);
					bptr += sizeof(*eh);
					bcopy(pptr, bptr, plen);
					bptr += plen;
					rlen += sizeof(*er) + sizeof(*eh) + plen;
					continue;
				}
				break;
			case SCTP_PTYPE_SET_IP:
				if (!(sp->l_caps & SCTP_CAPS_SET_IP))
					continue;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					continue;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					continue;
				if (!(sd = sctp_find_daddr(sp, ntohl(a->addr))))
					continue;
				/* fake success */
				sr = (typeof(sr)) bptr;
				sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
				sr->ph.len = __constant_htons(sizeof(*sr));
				sr->cid = ph->add_ip.id;
				bptr += sizeof(*sr);
				rlen += sizeof(*sr);
				continue;
			}
		}
		sp->p_caps |= SCTP_CAPS_ADD_IP;
		sctp_send_asconf_ack(sp, rptr, rlen);
		/* update routes now if a configuration change occured */
		if (reconfig)
			sctp_update_routes(sp, 1);
	} else if (asn == sp->p_asn) {
		/* ADD-IP 4.2 Rule C1 & C3 */
		/* give the same reply as before */
		if (sp->reply) {
			if (!(sd = sctp_route_response(sp)))
				goto noroute;
			sctplogtx(sp, "sending ASCONF-ACK");
			sctp_send_msg(sp, sd, sp->reply);
		}
	}
      discard:
	/* ADD-IP 4.2 Rule C1 & C4, R4 */
	return sctp_return_more(mp);
      noroute:
	sctplogrx(sp, "%s(): no route to peer", __FUNCTION__);
	return -EHOSTUNREACH;
      emsgsize:
	sctplogrx(sp, "%s(): bad message size", __FUNCTION__);
	return -EMSGSIZE;
      refuse:
	return sctp_recv_unrec_ctype(sp, mp);
}

/*
 *  RECV ASCONF ACK
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_asconf_ack(struct sctp *sp, mblk_t *mp)
{

	struct sctp_asconf_ack *m = (typeof(m)) mp->b_rptr;
	uint32_t asn;

	sctplogrx(sp, "received ASCONF-ACK");
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	/* ADD-IP 4.1.1 R4 */
	if (sp->state != SCTP_ESTABLISHED)
		goto discard;
	asn = ntohl(m->asn);
	if (before(asn, sp->l_asn))
		goto discard;
	if (after(asn, sp->l_asn))
		goto abort;
	/* ADD-IP 4.3 D0 */
	if (!sp->retry && !sctp_timeout_pending(&sp->timer_asconf))
		goto abort;
	/* ADD-IP 4.1 (A5) */
	sctp_ack_calc(sp, &sp->timer_asconf);
	/* process the ASCONF ACK */
	{
		union sctp_parm *ph;
		unsigned char *pptr = (unsigned char *) (m + 1);
		unsigned char *pend = pptr + ntohs(m->ch.len) - sizeof(*m);
		size_t plen;
		struct sctp_saddr *ss;
		uint32_t s2;

		/* process the ASCONF chunk */
		for (ph = (union sctp_parm *) pptr;
		     pptr + sizeof(ph->ph) <= pend && pptr + (plen = ntohs(ph->ph.len)) <= pend;
		     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
			uint type;

			if (plen < sizeof(ph->ph))
				return -EPROTO;
			switch ((type = ph->ph.type)) {
			case SCTP_PTYPE_ERROR_CAUSE:
				if (plen != sizeof(ph->error_cause))
					goto bad_parm;
				s2 = (typeof(s2)) ntohl(ph->error_cause.cid);
				for (ss = sp->saddr; ss && (uint32_t) (long) ss != s2;
				     ss = ss->next) ;
				if (!ss)
					goto bad_parm;
				if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
					ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
					ss->flags |= SCTP_SRCEF_ADD_REQUEST;
				}
				if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
					ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
					ss->flags |= SCTP_SRCEF_DEL_REQUEST;
				}
				if (ss->flags & SCTP_SRCEF_SET_PENDING) {
					ss->flags &= ~SCTP_SRCEF_SET_PENDING;
					ss->flags |= SCTP_SRCEF_SET_REQUEST;
				}
				break;
			case SCTP_PTYPE_SUCCESS_REPORT:
				if (plen != sizeof(ph->success_report))
					goto bad_parm;
				s2 = (typeof(s2)) ntohl(ph->success_report.cid);
				for (ss = sp->saddr; ss && (uint32_t) (long) ss != s2;
				     ss = ss->next) ;
				if (!ss)
					goto bad_parm;
				if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
					ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
				}
				if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
					ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
					__sctp_saddr_free(ss);
				}
				if (ss->flags & SCTP_SRCEF_SET_PENDING) {
					ss->flags &= ~SCTP_SRCEF_SET_PENDING;
				}
				break;
			default:
			      bad_parm:
				rare();
				if (type & SCTP_PTYPE_MASK_REPORT)
					sctp_send_error(sp, SCTP_CAUSE_BAD_PARM, (caddr_t) ph,
							plen);
				if (type & SCTP_PTYPE_MASK_CONTINUE)
					continue;
				/* SCTP IG 2.2 */
				goto discard;
			}
		}
		/* no news is good news */
		for (ss = sp->saddr; ss; ss = ss->next) {
			if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
				ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
			}
			if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
				ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
				__sctp_saddr_free(ss);
			}
			if (ss->flags & SCTP_SRCEF_SET_PENDING) {
				ss->flags &= ~SCTP_SRCEF_SET_PENDING;
			}
		}
	}
	sp->p_caps |= SCTP_CAPS_ADD_IP;
      discard:
	/* ADD-IP 4.2 Rule C1 & C4, R4 */
	return sctp_return_more(mp);
      abort:
	/* ADD-IP 4.3 D0 */
	/* abort the association with reason INVALID ASCONF ACK */
	sctp_send_abort_error(sp, SCTP_CAUSE_ILLEGAL_ASCONF, NULL, 0);
	sctp_abort(sp, SCTP_ORIG_PROVIDER, SCTP_CAUSE_ILLEGAL_ASCONF);
	return (-ECONNABORTED);
      emsgsize:
	return (-EMSGSIZE);
}
#endif				/* SCTP_CONFIG_ADD_IP */

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
/*
 *  RECV FORWARD TSN
 *  -------------------------------------------------------------------------
 *  Do a force cleanup of the receive gaps and move the cummulative ack point.
 */
STATIC int
sctp_recv_forward_tsn(struct sctp *sp, mblk_t *mp)
{

	struct sctp_forward_tsn *m = (typeof(m)) mp->b_rptr;
	uint32_t f_tsn;
	size_t nstrs, clen = ntohs(m->ch.len);

	sctplogrx(sp, "received FORWARD-TSN");
	if ((1 << sp->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen < sizeof(*m))
		goto emsgsize;
	f_tsn = ntohl(m->f_tsn);
	if (!(sp->p_caps & SCTP_CAPS_PR)) {
		seldom();
		/* strange, the peer didn't tell us on INIT(ACK) */
		sp->p_caps |= SCTP_CAPS_PR;
	}
	if (!after(f_tsn, sp->p_fsn))
		goto sctp_recv_fsn_duplicate;
	sp->p_fsn = f_tsn;
	if ((nstrs = (clen - sizeof(*m)) / sizeof(uint32_t))) {
		int err = 0;
		uint16_t *p = (uint16_t *) (m + 1);

		while (nstrs--) {
			uint16_t sid, ssn;
			struct sctp_strm *st;

			sid = *p++;
			ssn = *p++;
			/* push the sequence number */
			if ((st = sctp_istrm_find(sp, sid, &err)))
				if (!after_s(st->ssn, ssn))
					st->ssn = ssn;
		}
	}
	if (!sp->ngaps) {
		/* push ack point and mark sack */
		sp->r_ack = sp->p_fsn;
		sctp_send_sack(sp);
	} else {
		/* need to clean up any gaps */
		sctp_deliver_data(sp);
	}
	return sctp_return_more(mp);
      sctp_recv_fsn_duplicate:
	/* FSN is duplicate. */
	/* TODO: Here we should peg the duplicate against the last destination to which we sent a
	   SACK.  Receiving duplicate FSNs is a weak indication that our SACKs might not be getting 
	   through. */
      outstate:
	sctplogerr(sp, "%s() FORWARD TSN received in wrong SCTP state %s", __FUNCTION__,
		   sctp_statename(sp->state));
	return sctp_return_more(mp);
      emsgsize:
	sctplogerr(sp, "%s() invalid message size", __FUNCTION__);
	return (-EMSGSIZE);
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

/*
 *  RECV SCTP MESSAGE
 *  -------------------------------------------------------------------------
 */
STATIC void sctp_rcv_ootb(mblk_t *mp);

noinline fastcall __unlikely int
sctp_recv_msg_error(struct sctp *sp, mblk_t *msg, int err)
{
	sctplogrx(sp, "%s(): error %d returned", __FUNCTION__, err);
	/* NOTE: There are some errors that are returned by the receive functions that are not
	   handled by those function but are handled here.  These are exceptional error conditions. 
	 */
	switch (err) {
	case -ENOMEM:
	case -ENOBUFS:
	case -EBUSY:
		sctplogrx(sp, "%s(): resource problem", __FUNCTION__);
		/* These are resource problems */
		if ((1 << sp->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sp, SCTP_CAUSE_RES_SHORTAGE, NULL, 0);
		sctp_abort(sp, SCTP_ORIG_PROVIDER, -ECONNABORTED);
		break;
	case -EPROTO:
		sctplogrx(sp, "%s(): protocol violation", __FUNCTION__);
		/* This is a protocol violation */
		if ((1 << sp->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sp, SCTP_CAUSE_PROTO_VIOLATION, NULL, 0);
		sctp_abort(sp, SCTP_ORIG_PROVIDER, err);
		break;
	case -EINVAL:
		sctplogrx(sp, "%s(): invalid parameter", __FUNCTION__);
		/* This is an invalid parameter */
		if ((1 << sp->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sp, SCTP_CAUSE_INVALID_PARM, NULL, 0);
		sctp_abort(sp, SCTP_ORIG_PROVIDER, err);
		break;
	case -EMSGSIZE:
		sctplogrx(sp, "%s(): invalid message size", __FUNCTION__);
		/* This is a message formatting error */
		if ((1 << sp->state) & (SCTPF_NEEDABORT))
			sctp_send_abort(sp);
		sctp_abort(sp, SCTP_ORIG_PROVIDER, err);
		break;
	default:
		sctplogerr(sp, "%s() got unexpected error %d", __FUNCTION__, err);
		/* ignore others handled specially inside receive functions */
		return (err);
	}
	/* discard it after all */
	freemsg(msg);
	return (QR_ABSORBED);
}

noinline fastcall int
sctp_recv_chunk_slow(struct sctp *sp, mblk_t *mp, const uint8_t type)
{
	int err;

	if (unlikely(type == SCTP_CTYPE_DATA)) {
		sctplogda(sp, "receiving DATA chunk");
		err = sctp_recv_data(sp, mp);
	} else if (unlikely(type == SCTP_CTYPE_SACK)) {
		sctplogda(sp, "receiving DATA chunk");
		err = sctp_recv_sack(sp, mp);
	} else {
		SCTP_INC_STATS(SctpInCtrlChunks);
		switch (type) {
		case SCTP_CTYPE_INIT:
			sctplogrx(sp, "receiving INIT chunk");
			err = sctp_recv_init(sp, mp);
			break;
		case SCTP_CTYPE_INIT_ACK:
			sctplogrx(sp, "receiving INIT-ACK chunk");
			err = sctp_recv_init_ack(sp, mp);
			break;
		case SCTP_CTYPE_SACK:
			sctplogda(sp, "receiving SACK chunk");
			err = sctp_recv_sack(sp, mp);
			break;
		case SCTP_CTYPE_HEARTBEAT:
			sctplogrx(sp, "receiving HEARTBEAT chunk");
			err = sctp_recv_heartbeat(sp, mp);
			break;
		case SCTP_CTYPE_HEARTBEAT_ACK:
			sctplogrx(sp, "receiving HEARTBEAT-ACK chunk");
			err = sctp_recv_heartbeat_ack(sp, mp);
			break;
		case SCTP_CTYPE_ABORT:
			sctplogrx(sp, "receiving ABORT chunk");
			err = sctp_recv_abort(sp, mp);
			break;
		case SCTP_CTYPE_SHUTDOWN:
			sctplogrx(sp, "receiving SHUTDOWN chunk");
			err = sctp_recv_shutdown(sp, mp);
			break;
		case SCTP_CTYPE_SHUTDOWN_ACK:
			sctplogrx(sp, "receiving SHUTDOWN-ACK chunk");
			err = sctp_recv_shutdown_ack(sp, mp);
			break;
		case SCTP_CTYPE_ERROR:
			sctplogrx(sp, "receiving ERROR chunk");
			err = sctp_recv_error(sp, mp);
			break;
		case SCTP_CTYPE_COOKIE_ECHO:
			sctplogrx(sp, "receiving COOKIE-ECHO chunk");
			err = sctp_recv_cookie_echo(sp, mp);
			break;
		case SCTP_CTYPE_COOKIE_ACK:
			sctplogrx(sp, "receiving COOKIE-ACK chunk");
			err = sctp_recv_cookie_ack(sp, mp);
			break;
#ifdef SCTP_CONFIG_ECN
		case SCTP_CTYPE_ECNE:
			sctplogrx(sp, "receiving ECNE chunk");
			err = sctp_recv_ecne(sp, mp);
			break;
		case SCTP_CTYPE_CWR:
			sctplogrx(sp, "receiving CWR chunk");
			err = sctp_recv_cwr(sp, mp);
			break;
#endif				/* SCTP_CONFIG_ECN */
		case SCTP_CTYPE_SHUTDOWN_COMPLETE:
			sctplogrx(sp, "receiving SHUTDOWN-COMPLETE chunk");
			err = sctp_recv_shutdown_complete(sp, mp);
			break;
#ifdef SCTP_CONFIG_ADD_IP
		case SCTP_CTYPE_ASCONF:
			sctplogrx(sp, "receiving ASCONF chunk");
			err = sctp_recv_asconf(sp, mp);
			break;
		case SCTP_CTYPE_ASCONF_ACK:
			sctplogrx(sp, "receiving ASCONF-ACK chunk");
			err = sctp_recv_asconf_ack(sp, mp);
			break;
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		case SCTP_CTYPE_FORWARD_TSN:
			sctplogrx(sp, "receiving FORWARD-TSN chunk");
			err = sctp_recv_forward_tsn(sp, mp);
			break;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
		default:
			sctplogrx(sp, "receiving unrecognized chunk");
			err = sctp_recv_unrec_ctype(sp, mp);
			break;
		}
	}
	return (err);
}

/**
 * sctp_recv_chunk: - process received chunk
 * @sp: private structure
 * @mp: the SCTP message containing the chunk
 * @type: the chunk type
 *
 * This is the fast path for DATA and SACK chunks.
 */
STATIC inline fastcall __hot_in int
sctp_recv_chunk(struct sctp *sp, mblk_t *mp, uint8_t type)
{
	int err;

	if (likely(type == SCTP_CTYPE_DATA)) {
		sctplogda(sp, "receiving DATA chunk");
		err = sctp_recv_data(sp, mp);
	} else if (likely(type == SCTP_CTYPE_SACK)) {
		sctplogda(sp, "receiving SACK chunk");
		err = sctp_recv_sack(sp, mp);
	} else {
		sctplogrx(sp, "receiving OTHER chunk");
		err = sctp_recv_chunk_slow(sp, mp, type);
	}
	return (err);
}

/**
 * sctp_recv_msg_slow: - receive an SCTP message
 * @sp: private structure
 * @mp: the SCTP message
 *
 * This is the slower version with all exceptions included.  This function always returns either
 * QR_DONE to indicate that the message was not consumed, or a negative error number indicating that
 * the message is to be (re)queued.
 */
noinline fastcall int
sctp_recv_msg_slow(struct sctp *sp, mblk_t *mp)
{

	int err = mp->b_wptr - mp->b_rptr;
	struct sctpchdr *ch;

	__ensure(sp, goto efault);
	__ensure(mp, goto efault);
	sctplogda(sp, "%s: received mp = %p", __FUNCTION__, mp);
#if 0
	/* currently we linearize the skb and esballoc in the bottom end so we always get only 1
	   mblk per received packet. */
	/* pull up message */
	if (mp->b_cont && !pullupmsg(mp, -1))
		goto enomem;	/* for now */
#endif
	/* set address for reply chunks */
	if (sp->state != SCTP_LISTEN && !(sp->caddr = sctp_find_daddr(sp, SCTP_IPH(mp)->saddr))) {
		ch = SCTP_CH(mp);
		switch (ch->type) {
		case SCTP_CTYPE_INIT:
		case SCTP_CTYPE_INIT_ACK:
		case SCTP_CTYPE_COOKIE_ECHO:
			break;
		default:
			SCTP_INC_STATS_BH(SctpOutOfBlues);
#if 0
			icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
#endif
			sctp_rcv_ootb(mp);
			return (0);
		}
	}
	do {
		size_t clen;

		ch = SCTP_CH(mp);
		if (sizeof(*ch) + mp->b_rptr > mp->b_wptr || (clen = ntohs(ch->len)) < sizeof(*ch)
		    || PADC(clen) + mp->b_rptr > mp->b_wptr)
			goto emsgsize;
		err = sctp_recv_chunk(sp, mp, ch->type);
	} while (err >= sizeof(struct sctpchdr)
		 && mp->b_wptr >= mp->b_rptr + sizeof(struct sctpchdr));
	assure(err <= 0 || mp->b_wptr == mp->b_rptr);
	if (err < 0)
		goto error;
	return (QR_DONE);

	goto efault;
      efault:
	err = -EFAULT;
	sctplogerr(sp, "%s() fault", __FUNCTION__);
	goto error;
#if 0
      enomem:
	err = -ENOBUFS;
	sctplogerr(sp, "%s() no buffers for msgb handling (dropping)", __FUNCTION__);
	goto error;
#endif
      emsgsize:
	err = -EMSGSIZE;
	sctplogerr(sp, "%s() message too short (dropping), size = %d, chunk length = %d",
		   __FUNCTION__, (int) (mp->b_wptr - mp->b_rptr), (int) ntohs(ch->len));
	goto error;
      error:
	return sctp_recv_msg_error(sp, mp, err);
}

/**
 * sctp_recv_msg: - receive an SCTP message
 * @sp: private structure
 * @mp: the message
 *
 * This version is built for speed.  This is the fast path for data.  This function always returns
 * either QR_ABSORBED to indicate that the message was consumed, or a negative error number
 * indicating that the message is to be (re)queued.
 */
STATIC inline fastcall __hot_in int
sctp_recv_msg(struct sctp *sp, mblk_t *mp)
{
	int err = mp->b_wptr - mp->b_rptr;
	struct sctpchdr *ch;

	if (likely(sp->state != SCTP_LISTEN) &&
	    unlikely(!(sp->caddr = sctp_find_daddr(sp, SCTP_IPH(mp)->saddr))))
		goto go_slow;
	do {
		size_t clen;

		ch = SCTP_CH(mp);
		if (sizeof(*ch) + mp->b_rptr > mp->b_wptr || (clen = ntohs(ch->len)) < sizeof(*ch)
		    || PADC(clen) + mp->b_rptr > mp->b_wptr)
			goto go_slow;
		err = sctp_recv_chunk(sp, mp, ch->type);
	} while (likely(err >= sizeof(struct sctpchdr))
		 && likely(mp->b_wptr >= mp->b_rptr + sizeof(struct sctpchdr)));
	if (unlikely(err < 0))
		return sctp_recv_msg_error(sp, mp, err);
      done:
	freemsg(mp);
	return (QR_ABSORBED);
      go_slow:
	if ((err = sctp_recv_msg_slow(sp, mp)) != QR_DONE)
		return (err);
	goto done;
}

#if 0
/* *INDENT-OFF* */
struct {
	int errno;
	unsigned fatal:1;
	unsigned dest:1;
} sctp_icmp_err_convert[] = {
	[ICMP_NET_UNREACH]    = { ENETUNREACH,  0, 1 },
	[ICMP_HOST_UNREACH]   = { EHOSTUNREACH, 0, 1 },
	[ICMP_PROT_UNREACH]   = { ENOPROTOOPT,  1, 1 },
	[ICMP_PORT_UNREACH]   = { ECONNREFUSED, 0, 1 },
	[ICMP_FRAG_NEEDED]    = { EMSGSIZE,     0, 1 },
	[ICMP_SR_FAILED]      = { EOPNOTSUPP,   0, 1 },
	[ICMP_NET_UNKNOWN]    = { ENETUNREACH,  1, 1 },
	[ICMP_HOST_UNKNOWN]   = { EHOSTDOWN,    1, 1 },
	[ICMP_HOST_ISOLATED]  = { ENONET,       1, 1 },
	[ICMP_NET_ANO]        = { ENETUNREACH,  1, 1 },
	[ICMP_HOST_ANO]       = { EHOSTUNREACH, 1, 1 },
	[ICMP_NET_UNR_TOS]    = { ENETUNREACH,  0, 1 },
	[ICMP_HOST_UNR_TOS]   = { EHOSTUNREACH, 0, 1 },
	[ICMP_PKT_FILTERED]   = { EHOSTUNREACH, 1, 1 },
	[ICMP_PREC_VIOLATION] = { EHOSTUNREACH, 1, 1 },
	[ICMP_PREC_CUTOFF]    = { EHOSTUNREACH, 1, 1 },
};
/* *INDENT-ON* */
#endif

/**
 * sctp_recv_err: - process received ICMP error message
 * @sp: private structure
 * @mp: the message
 *
 * We do not expect a lot of these.  This function always consumes the message and returns
 * QR_ABSORBED.
 */
noinline fastcall int
sctp_recv_err(struct sctp *sp, mblk_t *mp)
{
	struct sctp_daddr *sd;
	uint32_t daddr = *((uint32_t *) mp->b_rptr);
	struct icmphdr *icmph = (struct icmphdr *) (mp->b_rptr + sizeof(uint32_t));
	int type = icmph->type;
	int code = icmph->code;
	int err = 0, abt = 0;

	if (sp->state == SCTP_CLOSED)
		goto closed;
	sd = sctp_find_daddr(sp, daddr);
	switch (type) {
	case ICMP_SOURCE_QUENCH:
		if (!sd)
			goto done;
		sctplogrx(sp, "ICMP: Source quench");
		/* Sockets does this directly from the bottom half, streams does not. */
		/* Adjust destination specifics. */
		sd->ssthresh = ((sd->cwnd >> 1) > (sd->mtu << 1)) ? sd->cwnd >> 1 : sd->mtu << 1;
		sd->cwnd = sd->mtu;
		/* SCTP IG Section 2.9 */
		sd->partial_ack = 0;
		sd->rto = (sd->rto) ? sd->rto << 1 : 1;
		sd->rto = (sd->rto_min > sd->rto) ? sd->rto_min : sd->rto;
		sd->rto = (sd->rto_max < sd->rto) ? sd->rto_max : sd->rto;
		goto done;
	case ICMP_DEST_UNREACH:
		if (code > NR_ICMP_UNREACH)
			goto done;
		sctplogrx(sp, "ICMP: Destination unreachable");
		if (code == ICMP_FRAG_NEEDED) {
			sctplogrx(sp, "ICMP: Fragmentation needed");
			if (sd && sd->dst_cache) {
				size_t mtu = ntohs(icmph->un.frag.mtu);

				ip_rt_update_pmtu(sd->dst_cache, mtu);
				if (dst_pmtu(sd->dst_cache) > mtu && mtu && mtu >= 68
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
				    && !(sd->dst_cache->mxlock & (1 << RTAX_MTU))
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL */
				    ) {
					dst_update_pmtu(sd->dst_cache, mtu);
					dst_set_expires(sd->dst_cache, ip_rt_mtu_expires);
				}
			}
		}
		sctplogrx(sp, "ICMP: error code %d", (int) code);
#if 1
		err = icmp_err_convert[code].errno;
		abt = icmp_err_convert[code].fatal;
#else
		err = sctp_icmp_err_convert[code].errno;
		abt = sctp_icmp_err_convert[code].fatal;
#endif
		break;
	case ICMP_PARAMETERPROB:
		sctplogrx(sp, "ICMP: Parameter problem");
		err = EPROTO;
		abt = 1;	/* fatal */
		break;
	case ICMP_TIME_EXCEEDED:
		sctplogrx(sp, "ICMP: Time exceeded");
		err = EHOSTUNREACH;
		abt = 0;	/* transient */
		break;
	default:
		goto done;
	}
	if (sp->state == SCTP_LISTEN)
		/* NOTE: Unlike TCP, we do not have partially formed sockets in the accept queue,
		   so this ICMP error should have gone to the established socket in the accept
		   queue that sent the COOKIE-ACK that generated the error.  Otherwise, it is for
		   an INIT-ACK that can't get to its destination, so we don't care, just ignore it. 
		 */
		goto listening;
	/* Try to be a little bit smarter about ICMP errors received while trying to form a
	   connection.  This can speed things up or make them more reliable. */
	if (abt && ((1 << sp->state) & (SCTPF_OPENING))) {
		switch (sp->state) {
		case SCTP_COOKIE_WAIT:
			/* advance timeout */
			sctplogte(sp, "truncating init timeout");
			if (sp->timer_init)
				sp_timer_init(sp, 1);
			else
				sctplogte(sp, "truncating init timeout failed");
			break;
		case SCTP_COOKIE_ECHOED:
			/* advance timeout */
			sctplogte(sp, "truncating cookie timeout");
			if (sp->timer_cookie)
				sp_timer_cookie(sp, 1);
			else
				sctplogte(sp, "truncating cookie timeout failed");
			break;
		}
	}
	if (sd && sd->dst_cache)
		dst_negative_advice(&sd->dst_cache);
	goto done;
      done:
	freemsg(mp);
	return (QR_ABSORBED);
      listening:
	sctplogerr(sp, "ICMP for listening stream");
	goto done;
      closed:
	sctplogerr(sp, "ICMP for closed stream");
	goto done;
}

/*
 *  ==========================================================================
 *
 *  SCTP PROTOCOL INTERFACE
 *
 *  ==========================================================================
 */
/*
 *  SCTP CLEAR
 *  -------------------------------------------------------------------------
 *  Clear the connection information hanging around on a socket or stream.  This includes any queued
 *  data blocks that are waiting around for retrieval.  It is OK to call this function twice in a
 *  row on the same socket or stream.
 *
 *  The objective of sctp_clear() is to remove any data remaining for retrieval before the socket or
 *  stream is reused or stopped.
 */
STATIC void
sctp_clear(struct sctp *sp)
{
#if 0
	sp->pmtu = 576;
#endif
	sctplogno(sp, "clearing stream, state = %d", (int) sp->state);
	/* purge queues */
	bufq_purge(&sp->expq);
	bufq_purge(&sp->rcvq);
	/* rmem_queued cleared */
	bufq_purge(&sp->urgq);
	bufq_purge(&sp->sndq);
	/* wmem_queued cleared */
	bufq_purge(&sp->errq);
	bufq_purge(&sp->oooq);
	sp->gaps = NULL;
	sp->ngaps = 0;
	sp->nunds = 0;
	bufq_purge(&sp->dupq);
	sp->dups = NULL;
	sp->ndups = 0;
	bufq_purge(&sp->rtxq);
	sp->nrtxs = 0;
	sp->nsack = 0;
	bufq_purge(&sp->ackq);
	/* free input and output streams */
	if (sp->ostrm || sp->istrm) {
		sctp_free_strms(sp);
	}
}

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP UNHASH
 *
 *  ---------------------------------------------------------------------------
 *  Removes the socket from all hashes.  It is unnecessary to put this in the protocol operations
 *  because this is only called from inet_sock_release() after calling sctp_destroy().  Because we
 *  have to drain the receive queues after unhashing the socket, we call this function directly from
 *  sctp_destroy() before flushing sctp private receive queues.
 */
STATIC void
sctp_unhash(struct sctp *sp)
{
	sctplogno(sp, "unhashing stream, state = %d", (int) sp->state);
	local_bh_disable();
	if (sp->prev) {
		if (sp->vprev)
			__sctp_vhash_unhash(sp);
		sp->v_tag = 0;
		if (sp->pprev)
			__sctp_phash_unhash(sp);
		sp->p_tag = 0;
		if (sp->tprev)
			__sctp_thash_unhash(sp);
		if (sp->lprev && !sp->conind) {
			__sctp_lhash_unhash(sp);
		}
		if (sp->daddr)
			__sctp_free_daddrs(sp);
		if (sp->bprev && !(sp->userlocks & SCTP_BINDPORT_LOCK))
			__sctp_bhash_unhash(sp);
		if (sp->saddr && !(sp->userlocks & SCTP_BINDADDR_LOCK))
			__sctp_free_saddrs(sp);
	} else {
		sctplogerr(sp, "%s() not hashed", __FUNCTION__);
	}
	local_bh_enable();
}

/*
 *  SCTP RESET
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Reset the connection.  This just stops timers, pulls the socket or stream out of the hashes
 *  (except for perhaps bind), frees simple retransmission, and zeroes connection info.  The socket
 *  or stream is left bound and souce addresses are left allocated if these were explicitly bound
 *  with sctp_bind.  For streams, the stream is left in the listening hash if it is listening.
 *
 *  The objective of sctp_reset() is to put the stream back to the bound state without any
 *  indications.
 */
STATIC void
sctp_reset(struct sctp *sp)
{
	sctplogno(sp, "resetting stream, state = %d", (int) sp->state);
	local_bh_disable();
	/* unhash and delete address lists */
	sctp_change_state(sp, sp->conind ? SCTP_LISTEN : SCTP_CLOSED);
	sctp_unhash(sp);
	/* stop timers */
	sp->timers = 0;
	sp_timer_cancel_init(sp);
	sp_timer_cancel_cookie(sp);
	sp_timer_cancel_shutdown(sp);
	sp_timer_cancel_guard(sp);
	sp_timer_cancel_sack(sp);
#ifdef SCTP_CONFIG_ADD_IP
	sp_timer_cancel_asconf(sp);
#endif				/* SCTP_CONFIG_ADD_IP */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	sp_timer_cancel_life(sp);
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
	/* free retry buffer */
	freechunks(xchg(&sp->retry, NULL));
	/* free reply buffer */
	freechunks(xchg(&sp->reply, NULL));
	/* zero simple retransmission information */
	sp->sackf = 0;
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->p_caps = 0;
	sp->p_ali = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
	sp->p_rbuf = 0;
#if 0
	/* clear routing cache */
	if (sp->dst_cache)
		dst_release(xchg(&sp->dst_cache, NULL));
#endif
	sp->route_caps = 0;
	sp->pmtu = ip_rt_min_pmtu;
	sp->amps = sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
	/* decide what to keep for retrieval */
	switch (sp->disp) {
	case SCTP_DISPOSITION_NONE:
	{
		mblk_t *mp;

		while ((mp = bufq_dequeue(&sp->rtxq))) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->flags & SCTPCB_FLAG_CONF)
				bufq_queue(&sp->ackq, mp);
			else
				freemsg(mp);
		}
		while ((mp = bufq_dequeue(&sp->urgq))) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->flags & SCTPCB_FLAG_CONF)
				bufq_queue(&sp->ackq, mp);
			else
				freemsg(mp);
		}
		while ((mp = bufq_dequeue(&sp->sndq))) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->flags & SCTPCB_FLAG_CONF)
				bufq_queue(&sp->ackq, mp);
			else
				freemsg(mp);
		}
		break;
	}
	case SCTP_DISPOSITION_UNSENT:
	{
		mblk_t *mp;

		while ((mp = bufq_dequeue(&sp->rtxq))) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->flags & SCTPCB_FLAG_CONF)
				bufq_queue(&sp->ackq, mp);
			else
				freemsg(mp);
		}
		while ((mp = bufq_dequeue(&sp->urgq)))
			bufq_queue(&sp->ackq, mp);
		while ((mp = bufq_dequeue(&sp->sndq)))
			bufq_queue(&sp->ackq, mp);
		break;
	}
	case SCTP_DISPOSITION_SENT:
	{
		mblk_t *mp;

		while ((mp = bufq_dequeue(&sp->rtxq))) {
			sctp_tcb_t *cb = SCTP_TCB(mp);

			if (cb->flags & SCTPCB_FLAG_CONF)
				bufq_queue(&sp->ackq, mp);
			else if (cb->flags & SCTPCB_FLAG_SACKED || cb->flags & SCTPCB_FLAG_DROPPED)
				freemsg(mp);
			else
				bufq_queue(&sp->ackq, mp);
		}
		while ((mp = bufq_dequeue(&sp->urgq)))
			bufq_queue(&sp->ackq, mp);
		while ((mp = bufq_dequeue(&sp->sndq)))
			bufq_queue(&sp->ackq, mp);
		break;
	}
	case SCTP_DISPOSITION_GAP_ACKED:
	case SCTP_DISPOSITION_ACKED:
	default:
	{
		mblk_t *mp;

		while ((mp = bufq_dequeue(&sp->rtxq)))
			bufq_queue(&sp->ackq, mp);
		while ((mp = bufq_dequeue(&sp->urgq)))
			bufq_queue(&sp->ackq, mp);
		while ((mp = bufq_dequeue(&sp->sndq)))
			bufq_queue(&sp->ackq, mp);
		break;
	}
	}
	sp->nrtxs = 0;
	sp->nsack = 0;
	local_bh_enable();
	return;
}

/*
 *  ABORT
 *  -------------------------------------------------------------------------
 *  Abort the STREAM.  The caller must send a disconnect indication to the user interface if
 *  necessary and send an abort if necessary.  This just pulls the stream out of the hashes, stops
 *  timers, frees simple retransmission, and zeros connection info.  The stream is left bound and
 *  destination addressses are left allocated.  Any connection indications are left queued against
 *  the stream.
 */
/*
 *  Non-locking version for use from within timeouts (runninga at bottom-half so don't do
 *  bottom-half locks).
 */
STATIC void
sctp_abort(struct sctp *sp, t_uscalar_t origin, t_scalar_t reason)
{
	sctplogrx(sp, "disconnect indication");
	assert(sp);
	assure(origin && reason);
	if (sp->state == SCTP_LISTEN)
		goto outstate;
	sctp_reset(sp);
	{
		sp->origin = origin;
		sp->reason = reason;
		qenable(sp->rq);
	}
	return;
      outstate:
	sctplogerr(sp, "%s() in wrong SCTP state %s", __FUNCTION__, sctp_statename(sp->state));
	return;
}

/*
 *  CONN_REQ:
 *  -------------------------------------------------------------------------
 *  Connect to the peer.  This launches the INIT process.
 *
 *  This is a direct extension of the connect(2) or t_connect(3) call.  For SCTP in accordance with
 *  draft-ietf-tsvwg-sctpsocket-06.txt this connect will result in the (ultimate) establishment of
 *  an association.  This is only performed after a bind (otherwise and automatic binding is
 *  attempted).  Only one address is required for a connect.
 *
 *  For sockets bound to INADDR_ANY, we generate the local IP address list on demand at the time
 *  that the connect() function is called.  This address list will include every device address
 *  available on the system.
 *
 *  We also register INADDR_ANY bound sockets with the netdevice_notifier so that when local
 *  addresses are added or removed, we can add and remove them from the association with ADD-IP.
 */
STATIC int sctp_get_port(struct sctp *sp, uint16_t port);
STATIC fastcall int
sctp_conn_req(struct sctp *sp, uint16_t dport, struct sockaddr_in *dsin, size_t dnum, mblk_t *dp)
{
	int err;
	unsigned short num = ntohs(dport);

	assert(sp);
	sctplogrx(sp, "X_CONN_REQ ->");
	if (!dnum)
		goto einval;
	if (dsin->sin_family != AF_INET && dsin->sin_family)
		goto eafnosupport;
	if (sp->state != SCTP_CLOSED)
		goto eisconn;
	if (!num)
		goto eaddrnotavail;
	if (num < PROT_SOCK && sp->cred.cr_uid != 0)
		goto eacces;
	if ((err = sctp_alloc_sock_daddrs(sp, dport, dsin, dnum)) < 0)
		goto error;
	if (err == 0)
		goto eaddrnotavail;
	if (!sp->sanum || !sp->saddr) {
		if (!(sp->userlocks & SCTP_BINDADDR_LOCK)) {
			if (sctp_get_addrs(sp, sp->daddr->daddr))
				goto eaddrnotavail;
		} else {
			swerr();
			goto eaddrnotavail;
		}
	}
	if (!sp->bprev) {
		if (!(sp->userlocks & SCTP_BINDPORT_LOCK)) {
			if (sctp_get_port(sp, 0))
				goto eaddrnotavail;
			sp->sport = htons(sp->num);
		} else {
			swerr();
			goto eaddrnotavail;
		}
	}
	sp->dport = dport;
	/* obtain a verification tag */
	sp->v_tag = sctp_get_vtag(sp->saddr->saddr, sp->daddr->daddr, sp->sport, sp->dport);
	sp->p_tag = 0;
	sctplogno(sp, "assigned vtag %08X", sp->v_tag);
	sp->inet.id = sp->v_tag ^ jiffies;
	sp->ext_header_len = 0;
	if ((err = sctp_update_routes(sp, 1)))
		goto error;
	sctp_clear(sp);		/* clear old information */
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->t_tsn = sp->v_tag;
	sp->t_ack =
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    sp->l_fsn =
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	    sp->m_tsn =
#ifdef SCTP_CONFIG_ADD_IP
	    sp->l_asn =
#endif				/* SCTP_CONFIG_ADD_IP */
	    sp->v_tag - 1;
	sp->r_ack =
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    sp->p_fsn =
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ADD_IP
	    sp->p_asn =
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_ECN
	    sp->p_lsn =
#endif				/* SCTP_CONFIG_ECN */
	    0;
	sp->ck_inc = 0;
	/* place in connection hashes */
	if ((err = sctp_conn_hash(sp)))
		goto error;
	/* fake a data request if data in conn req */
	if (dp && (err = sctp_data_req(sp, sp->ppi, sp->sid, 0, 0, 0, dp)))
		goto unhash_error;
	if ((err = sctp_send_init(sp)))
		goto unhash_error;
	return (0);
      einval:
	sctplogrx(sp, "%s() invalid address", __FUNCTION__);
	err = -EINVAL;
	goto error;
      eacces:
	sctplogrx(sp, "%s() no permission to address", __FUNCTION__);
	err = -EACCES;
	goto error;
      eafnosupport:
	sctplogrx(sp, "%s() address family not supported", __FUNCTION__);
	err = -EAFNOSUPPORT;
	goto error;
      eisconn:
	sctplogrx(sp, "%s() already connected", __FUNCTION__);
	err = -EISCONN;
	goto error;
      eaddrnotavail:
	sctplogrx(sp, "%s() address not available", __FUNCTION__);
	err = -EADDRNOTAVAIL;
	goto error;
      unhash_error:
	sctp_unhash(sp);
	goto error;
      error:
	return (err);
}

/*
 *  DISCON_REQ:
 *  -------------------------------------------------------------------------
 *  STREAMS is able to issue a disconnect request in response to a connection indication.  Sockets
 *  is not, because the connection indication is inherently responded to on accept(2).  Therefore,
 *  the sockets version of this function will never populate a real connection pointer and can only
 *  be used for abortive disconnect of an socket that is established or which has an outgoing
 *  connection pending.
 */
STATIC fastcall int
sctp_discon_req(struct sctp *sp, mblk_t *cp)
{
	sctplogrx(sp, "X_DISCON_REQ ->");
	/* Caller must ensure that sp and cp (if any) are correct and appropriate. */
	if (cp) {
		struct iphdr *iph = SCTP_IPH(cp);
		struct sctphdr *sh = SCTP_SH(cp);

		sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		/* conn ind will be unlinked by caller */
		return (0);
	}
	if ((1 << sp->state) & (SCTPF_NEEDABORT))
		/* SCTP IG 2.21 */
		sctp_send_abort_error(sp, SCTP_CAUSE_USER_INITIATED, NULL, 0);
	sctp_reset(sp);
	return (0);
}

/*
 *  UNBIND
 *  -------------------------------------------------------------------------
 *  We should already be in a disconnected state.  This pulls us from the bind hashes and
 *  deallocates source addresses, any connection indications that are queued against the socket or
 *  stream are purged (these might occur if a connection indication comes in just before we unbind
 *  and we have made an error somewhere else: normally the response to an X_UNBIND_REQ in a
 *  connection indication state will be a X_ERROR_ACK).
 *
 *  The objective of sctp_unbind() is to place the socket or stream into the idle state without any
 *  indications.
 */
STATIC void
sctp_unbind(struct sctp *sp)
{
	assert(sp);
	sctp_change_state(sp, SCTP_CLOSED);
	sp->userlocks = 0;	/* break user locks */
	sp->conind = 0;		/* remove from listen hashes */
	sctp_reset(sp);
	sctp_clear(sp);
	/* XXX: should really send an abort to each connection indication */
	unusual(sp->conq.q_msgs);
	bufq_purge(&sp->conq);
}

/*
 *  UNBIND_REQ:
 *  -------------------------------------------------------------------------
 *  STREAMS is able to issue an unbind request directly.  Sockets is not.  For sockets, according to
 *  X/Open and POSIX, the way to unbind a socket is to bind the socket to AF_UNSPEC.  This is
 *  normally intercepted by the socket code which then unhashes the socket.
 */
STATIC fastcall int
sctp_unbind_req(struct sctp *sp)
{
	sctplogrx(sp, "X_UNBIND_REQ ->");
	switch (sp->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		/* can't wait for SHUTDOWN COMPLETE any longer */
		SCTP_INC_STATS_BH(SctpShutdowns);
	case SCTP_CLOSED:
	case SCTP_LISTEN:
		sctp_unbind(sp);
		return (0);
	}
	sctplogerr(sp, "%s() in unexpected state %s", __FUNCTION__, sctp_statename(sp->state));
	return (-EPROTO);
}

/*
 *  RESET_REQ:
 *  -------------------------------------------------------------------------
 *  Unlike OSI protocols, there is no direct way in SCTP to generate a reset request.  What we could
 *  do is keep a copy of the old cookie against the socket or stream (the cookie used to activate
 *  the socket or stream) and then send a COOKIE ECHO with this cookie to generate a RESTART at the
 *  other end.  Although this would work, it is probably better to discconnect and reconnect the
 *  association.  For now we just confirm the reset and do nothing.
 */
STATIC fastcall int
sctp_reset_req(struct sctp *sp)
{
	int err;

	sctplogrx(sp, "X_RESET_REQ ->");
	/* do nothing */
	if ((err = sctp_reset_con(sp)))
		return (err);
	return (0);
}

/*
 *  RESET_RES:
 *  -------------------------------------------------------------------------
 *  Only the STREAMS NPI interface protocol provides a mechanism to directly respond to reset
 *  indications with a reset response (N_RESET_RES).  Neither STREAMS XTI/TPI nor sockets supports
 *  such a direct mechanism.  For XTI/TPI and Sockets, the response should be automatically
 *  generated internal to the provider.
 */
STATIC fastcall int
sctp_reset_res(struct sctp *sp)
{
	mblk_t *cp;

	sctplogrx(sp, "X_RESET_RES ->");
	if (!(cp = bufq_dequeue(&sp->conq))) {
		sctplogerr(sp, "%s() fault", __FUNCTION__);
		return (-EFAULT);
	}
	return sctp_conn_res(sp, cp, sp, NULL);
}

/*
 *  CONN_RES:
 *  -------------------------------------------------------------------------
 *  This is the accept(2) or t_accept(3) call being performed on the socket or stream.  This will
 *  result the the waiting for acceptance of an incoming SCTP association.  This is only performed
 *  after a bind and a listen.  The stream must be in the listening state.
 */
STATIC fastcall int
sctp_conn_res(struct sctp *sp, mblk_t *cp, struct sctp *ap, mblk_t *dp)
{
	int err;
	struct sctp_cookie_echo *m;
	struct sctp_cookie *ck;
	uint32_t *daddrs;
	uint32_t *saddrs;

	sctplogrx(sp, "X_CONN_RES ->");
	assert(sp);
	assert(cp);
	assert(ap);
	m = (struct sctp_cookie_echo *) cp->b_rptr;
	ck = (struct sctp_cookie *) m->cookie;
	daddrs = (uint32_t *) (ck + 1);
	saddrs = daddrs + ck->danum;

	/* STREAMS uses either the listening stream or another stream that is possibly already
	   bound to an accepting address.  Because of this it is necessary in STREAMS to clear the
	   state of the accepting stream and rebind it if necessary.  For sockets, we always start
	   with a freshly created socket upon which to accept the connection and do not have to
	   clear old state on the socket. */
	if (ap != sp)
		ap->userlocks = 0;	/* break locks */
	sctp_reset(ap);		/* we need to rebind the accepting stream */
	sctp_clear(ap);
	/* Force user locks because both the source port and source addresses are specified in the
	   cookie.  */
	if (ap != sp)
		ap->userlocks |= (SCTP_BINDPORT_LOCK | SCTP_BINDADDR_LOCK);
	if (!sctp_saddr_include(ap, ck->saddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_saddrs(ap, ck->sport, saddrs, ck->sanum)))
		goto error;
	if (ap != sp)
		__sctp_inherit_port(ap, sp);
	if (!sctp_daddr_include(ap, ck->daddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_daddrs(ap, ck->dport, daddrs, ck->danum)) < 0)
		goto error;
	ap->v_tag = ck->v_tag;
	ap->p_tag = ck->p_tag;
	ap->n_istr = ck->n_istr;
	ap->n_ostr = ck->n_ostr;
	ap->t_tsn = ck->v_tag;
	ap->t_ack =
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    ap->l_fsn =
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	    ap->m_tsn =
#ifdef SCTP_CONFIG_ADD_IP
	    ap->l_asn =
#endif				/* SCTP_CONFIG_ADD_IP */
	    ck->v_tag - 1;
	ap->r_ack =
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    ap->p_fsn =
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ADD_IP
	    ap->p_asn =
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_ECN
	    ap->p_lsn =
#endif				/* SCTP_CONFIG_ECN */
	    ck->p_tsn - 1;
	ap->p_rwnd = ck->p_rwnd;
	ap->p_rbuf = ck->p_rwnd;
	if ((err = sctp_conn_hash(ap)))
		goto error;
	if ((err = sctp_update_routes(ap, 1)))
		goto error;
	sctp_change_state(ap, SCTP_ESTABLISHED);
	/* unlock accepting socket or stream */
	/* process any chunks bundled with cookie echo on accepting stream */
	if (sctp_return_more(cp) > 0)
		sctp_recv_msg_slow(ap, cp);
	/* fake a data request if data in conn res */
	if (dp && (err = sctp_data_req(ap, ap->ppi, ap->sid, 0, 0, 0, dp)))
		goto release_error;
	sctp_send_cookie_ack(ap);
	SCTP_INC_STATS_USER(SctpPassiveEstabs);
	/* caller will unlink connect indication */
	return (0);
      error:
	if (ap != sp)
		ap->userlocks = 0;	/* break bind locks */
	sctp_unhash(ap);
	return (err);
      release_error:
	if (ap != sp)
		ap->userlocks = 0;	/* break bind locks */
	sctp_unhash(ap);
	return (err);
}

/*
 *  ORDREL_REQ:
 *  -------------------------------------------------------------------------
 */
STATIC fastcall int
sctp_ordrel_req(struct sctp *sp)
{
	sctplogrx(sp, "X_ORDREL_REQ ->");
	switch (sp->state) {
	case SCTP_ESTABLISHED:
		sctp_change_state(sp, SCTP_SHUTDOWN_PENDING);
	case SCTP_SHUTDOWN_PENDING:
		/* check for empty send queues */
		if (!bufq_head(&sp->sndq)
		    && !bufq_head(&sp->urgq)
		    && !bufq_head(&sp->rtxq))
			sctp_send_shutdown(sp);
		break;
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_change_state(sp, SCTP_SHUTDOWN_RECVWAIT);
	case SCTP_SHUTDOWN_RECVWAIT:
		/* check for empty send queues */
		if (!bufq_head(&sp->sndq)
		    && !bufq_head(&sp->urgq)
		    && !bufq_head(&sp->rtxq))
			sctp_send_shutdown_ack(sp);
		break;
	default:
		sctplogerr(sp, "%s() in unexpected state %s", __FUNCTION__, sctp_statename(sp->state));
		return (-EPROTO);
	}
	return (0);
}

/**
 * sctp_data_req: - process data send request
 * @sp: private structure
 * @ppi: payload protocol identifier
 * @sid: stream identifier
 * @ord: ordered (1) or unordered (0)
 * @more: more data in record (1) or end of record (0)
 * @rcpt: receipt confirmation requested (1) or not requested (0)
 * @mp: user data
 *
 * FIXME: this function should be altered to accept a flags word instead of all these integer flags.
 *
 * This function returns either zero (0) or a negative error message usable by m_error_reply().
 */
STATIC fastcall __hot_write int
sctp_data_req(struct sctp *sp, uint32_t ppi, uint16_t sid, uint ord, uint more, uint rcpt,
	      mblk_t *mp)
{
	uint err = 0, flags = 0;
	struct sctp_strm *st;

	sctplogda(sp, "X_DATA_REQ ->");
	__ensure(mp, return (-EFAULT));
	/* don't allow zero-length data through */
	if (mp && !msgdsize(mp)) {
		sctplogerr(sp, "%s() mp = %p, msgdsize = %d", __FUNCTION__, mp, (int) msgdsize(mp));
		freemsg(mp);
		return (0);
	}
	if (!(st = sctp_ostrm_find(sp, sid, &err))) {
		sctplogno(sp, "error path taken");
		return (err);
	}
	/* we probably want to data ack out of order as well */
#if 0
	if (rcpt || (ord && (sp->flags & SCTP_FLAG_DEFAULT_RC_SEL)))
#else
	if (rcpt)
#endif
		flags |= SCTPCB_FLAG_CONF;
	if (!ord) {
		flags |= SCTPCB_FLAG_URG;
		if (!st->x.more) {
			flags |= SCTPCB_FLAG_FIRST_FRAG;
			st->x.ppi = ppi;
		}
	} else {
		if (!st->n.more) {
			flags |= SCTPCB_FLAG_FIRST_FRAG;
			st->n.ppi = ppi;
		}
	}
	if (!more)
		flags |= SCTPCB_FLAG_LAST_FRAG;
	sctplogda(sp, "%s() sending message %p", __FUNCTION__, mp);
	return sctp_send_data(sp, st, flags, mp);
}

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP GET PORT
 *
 *  ---------------------------------------------------------------------------
 *
 *  Obtain a reference to a local port for the given socket or stream.  If port is zero it means to
 *  select any available local port.  This is used in a number of binding and autobinding operations
 *  of the socket or stream.
 *
 *  SO_REUSEADDR
 *	Indicates that the rules used in validating addresses supplied in a bind(2) call should
 *	allow reuse of local addresses.  For PF_INET sockets this means that a socket may bind,
 *	except when there is an active listening socket bound to the address.  When there is an
 *	active listening socket bound to INADDR_ANY with a specific port then it is not possible to
 *	bind to this port for any local address.
 *
 */
STATIC int sctp_port_rover = 0;

/* FIXME: the following should be done in the hooks file. */
#ifdef HAVE_SYSCTL_LOCAL_PORT_RANGE_SYMBOL
extern int sysctl_local_port_range[2];
#else				/* sysctl_local_port_range */
STATIC int sysctl_local_port_range[2] = { 1024, 4999 };
#endif				/* sysctl_local_port_range */
STATIC INLINE int
sctp_addr_match(sctp_t * sp1, sctp_t * sp2)
{
	struct sctp_saddr *ss1, *ss2;

	/* This loop isn't as bad as it looks, normally we only have 2 or 3 source addresses on our 
	   list. */
	for (ss1 = sp1->saddr; ss1; ss1 = ss1->next)
		for (ss2 = sp2->saddr; ss2; ss2 = ss2->next)
			if (ss1->saddr == ss2->saddr)
				return 1;
	return 0;
}
STATIC INLINE int
sctp_bind_conflict(struct sctp *sp, struct sctp_bind_bucket *sb)
{
	struct sctp *sp2;

	/* The bind bucket is marked for fastreuse when there is no listening or non-reusable
	   socket on the owners list.  */
	if (sb->owners && sb->fastreuse != 0 && sp->reuse != 0 && sp->state != SCTP_LISTEN)
		return 0;
	for (sp2 = sb->owners; sp2; sp2 = sp2->bnext)
		if (sp != sp2
		    && (!sp->reuse || !sp2->reuse || sp->state == SCTP_LISTEN
			|| sp2->state == SCTP_LISTEN))
			if (!(sp->userlocks & SCTP_BINDADDR_LOCK)
			    || !(sp2->userlocks & SCTP_BINDADDR_LOCK) ||
			    /* dig into the source address list */
			    sctp_addr_match(sp, sp2))
				break;
	if (sp2) {
		sctplogno(sp, "port %d conflict", sb->port);
	}
	return (sp2 != NULL);
}
STATIC int
sctp_get_port(struct sctp *sp, uint16_t port)
{
	struct sctp_bind_bucket *sb = NULL;
	unsigned long flags;

	if (port == 0) {
		/* This approach to selecting an available port number is identical to that used
		   for TCP IPv4. We use the same port ranges.  */
		static spinlock_t sctp_portalloc_lock = SPIN_LOCK_UNLOCKED;
		int low = sysctl_local_port_range[0];
		int high = sysctl_local_port_range[1];
		int rem = (high - low) + 1;
		int rover;

		/* find a fresh, completely unused port number (that way we are guaranteed not to
		   have a port conflict */
		spin_lock(&sctp_portalloc_lock);
		rover = sctp_port_rover;
		for (; rem > 0; rover++, rem--) {
			if (rover > high || rover < low)
				rover = low;
			if (!(sb = sctp_bindb_get(rover, &flags)))
				break;
			sctp_bindb_put(rover, &flags);
		}
		sctp_port_rover = rover;
		spin_unlock(&sctp_portalloc_lock);
		if (rem <= 0 || sb)
			goto fail;
		if (!(port = rover)) {
			swerr();
			goto fail_put;
		}
	} else
		sb = sctp_bindb_get(port, &flags);
	if (!sb) {
		/* create an entry if there isn't one already */
		/* we still have a write lock on the hash slot */
		if (!(sb = __sctp_bindb_create(port)))
			goto fail_put;
	} else {
		/* check reusability or conflict if entry exists */
		if (sctp_bind_conflict(sp, sb))
			goto fail_put;
	}
	/* put this socket in the bind hash */
	___sctp_bhash_insert(sp, sb);
	if (sp->state == SCTP_LISTEN && sp->conind)
		__sctp_lhash_insert(sp);
	sctp_bindb_put(port, &flags);
	return 0;
      fail_put:
	sctp_bindb_put(port, &flags);
      fail:
	return 1;
}

/*
 *  BIND_REQ:
 *  -------------------------------------------------------------------------
 *  IMPLEMENTATION NOTES:- There are some interesting differences between the SCTP bind function and
 *  the TCP bind function.  Multiple addresses can be provided (by increasing addr_len) in the (now)
 *  array pointed to by struct sockaddr.  For SCTP when we do a bind, we want to bind to a number of
 *  source addresses.  When binding to INADDR_ANY, we want to use the entire list of local addresses
 *  available on this host; however, we delay determining that list until either a t_listen() or
 *  t_connect() is performed on the socket.
 */
#ifndef PROT_SOCK
#define PROT_SOCK 1024
#endif
STATIC fastcall int
sctp_bind_req(struct sctp *sp, uint16_t sport, struct sockaddr_in *ssin, size_t snum,
	      t_uscalar_t cons)
{
	int err;
	uint32_t saddr = 0;
	unsigned short num;

	sctplogrx(sp, "X_BIND_REQ ->");
	assert(sp);
	/* stream is already locked */
	if (snum < 1)
		goto einval;
	num = ntohs(sport);
	usual(num);
	if (num && num < PROT_SOCK && sp->cred.cr_uid != 0)
		goto eacces;
	if (sp->state != SCTP_CLOSED || sp->num != 0)
		goto einval;
	if ((err = sctp_alloc_sock_saddrs(sp, sport, ssin, snum)) < 0)
		goto error;
	usual(err >= 0);
	(void) saddr;
	/* also places us in bind (and possibly listen for streams) hashes */
	sctp_change_state(sp, (sp->conind = cons) ? SCTP_LISTEN : SCTP_CLOSED);
	if (sctp_get_port(sp, num))
		goto eaddrinuse;
	if (sp->saddr && sp->saddr->saddr != INADDR_ANY)
		sp->userlocks |= SCTP_BINDADDR_LOCK;
	if (num)
		sp->userlocks |= SCTP_BINDPORT_LOCK;
	sp->sport = htons(sp->num);
	__sctp_free_daddrs(sp);
	/* stream was not locked */
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      eacces:
	err = -EACCES;
	goto error;
      eaddrinuse:
	err = -EADDRINUSE;
	__sctp_free_saddrs(sp);
	goto error;
      error:
	sctp_change_state(sp, SCTP_CLOSED);
	sp->conind = 0;
	/* stream was not locked */
	return (err);
}

#ifdef SCTP_CONFIG_ADD_IP
/*  
 *  ADD IP Support
 *  -------------------------------------------------------------------------
 *  The following functions provide ADD IP support for SCTP.  The sockets version calls these
 *  functions from setsockopt(2); XTI/TPI similarly calls these from t_optmgmt(3); NPI calls these
 *  from N_BIND_REQ with a heirarchal bind request.
 *
 *  SCTP ADD IP
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Adds the specified inet socket address to the association.  If the socket is is bound but not
 *  yet connected, then this has the same effect as adding an address to the bind.  If the socket is
 *  connected, then this invokes the ADD IP procedure if the appropriate compile option was set.
 */
STATIC int
sctp_add_ip(struct sctp *sp, struct sockaddr_in *addr)
{
	int err = 0;
	struct sctp_saddr *ss;

	if ((1 << sp->state) & (SCTPF_DISCONNECTED)) {
		todo(("TODO: perform an addition to the bind\n"));
		return -ENOTCONN;
	} else if (sp->state == SCTP_ESTABLISHED) {
		if (sp->sanum == 0)
			return -EINVAL;
		if ((ss = sctp_find_saddr(sp, addr->sin_addr.s_addr)))
			return (0);	/* silent success */
		if (!(sp->p_caps & SCTP_CAPS_ADD_IP)
		    || !(sp->l_caps & SCTP_CAPS_ADD_IP))
			return -EOPNOTSUPP;
		if (!(ss = __sctp_saddr_alloc(sp, addr->sin_addr.s_addr, &err)))
			return err;
		ss->flags |= SCTP_SRCEF_ADD_REQUEST;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sp);
		return (0);
	} else
		return -EPROTO;
}

/*  
 *  SCTP DEL IP
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Deletes the specified inet socket asddress from the association.  If the socket is bound but not
 *  yet connected, then this has the same effect as removing an address from the bind.  If the
 *  socket is connected, then this invokes the ADD IP procedure if the appropriate compile option
 *  was set.
 */
STATIC int
sctp_del_ip(struct sctp *sp, struct sockaddr_in *addr)
{
	struct sctp_saddr *ss;

	if ((1 << sp->state) & (SCTPF_DISCONNECTED)) {
		todo(("TODO: perform an removal from the bind\n"));
		return -ENOTCONN;
	} else if (sp->state == SCTP_ESTABLISHED) {
		if (sp->sanum < 2)
			return -EINVAL;
		if (!(ss = sctp_find_saddr(sp, addr->sin_addr.s_addr)))
			return (0);	/* silent success */
		ss->flags |= SCTP_SRCEF_DEL_REQUEST;
		if (!(sp->p_caps & SCTP_CAPS_ADD_IP)
		    || !(sp->l_caps & SCTP_CAPS_ADD_IP))
			return -EOPNOTSUPP;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sp);
		return (0);
	} else
		return -EPROTO;
}

/*  
 *  SCTP SET IP
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Sets the specified inet socket ddress as the primary address.  Because we do Concurrent
 *  Multipath Transfer (CMT), this has little effect on us, but may have some effect on the peer.
 *  If the socket is not connected we could reorder the bind so that the peer would likely use the
 *  specified address as the primary address.
 */
STATIC int
sctp_set_ip(struct sctp *sp, struct sockaddr_in *addr)
{
	struct sctp_saddr *ss;

	if (sp->state == SCTP_ESTABLISHED) {
		if (sp->sanum == 0)
			return -EINVAL;
		if (!(ss = sctp_find_saddr(sp, addr->sin_addr.s_addr)))
			return -EINVAL;
		ss->flags |= SCTP_SRCEF_SET_REQUEST;
		if (!(sp->p_caps & SCTP_CAPS_SET_IP)
		    || !(sp->l_caps & SCTP_CAPS_SET_IP))
			return -EOPNOTSUPP;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sp);
		return (0);
	} else
		return -EINVAL;
}
#endif				/* SCTP_CONFIG_ADD_IP */

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP INIT STRUCT
 *
 *  -------------------------------------------------------------------------
 */
STATIC void
sctp_init_struct(struct sctp *sp)
{
	(void) sp;
	sctplogno(sp, "initializing stream");
	sp->sackf = SCTP_SACKF_NEW;	/* don't delay first sack */
	/* initialize timers */
	sp->timers = 0;
	sp->timer_init = 0;
	sp->timer_cookie = 0;
	sp->timer_shutdown = 0;
	sp->timer_guard = 0;
	sp->timer_sack = 0;
#ifdef SCTP_CONFIG_ADD_IP
	sp->timer_asconf = 0;
#endif				/* SCTP_CONFIG_ADD_IP */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	sp->timer_life = 0;
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
	/* initialize queues */
	bufq_init(&sp->conq);
	bufq_init(&sp->expq);
	bufq_init(&sp->urgq);
	bufq_init(&sp->oooq);
	bufq_init(&sp->errq);
	bufq_init(&sp->rtxq);
	bufq_init(&sp->dupq);
	bufq_init(&sp->ackq);
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
	sp->hb_tint = (sctp_defaults.sctp.heartbeat_itvl >> 1) + 1;
	sp->hb_rcvd = jiffies;
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
	/* start in closed state */
	sctp_change_state(sp, SCTP_CLOSED);
	/* ip defaults */
	sp->inet.tos = sctp_defaults.ip.tos;
#if 1
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
	sp->inet.ttl = sysctl_ip_default_ttl;
#else
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
	sp->inet.uc_ttl = sysctl_ip_default_ttl;
#endif
#endif				/* HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
#else
	sp->inet.uc_ttl = sysctl_ip_default_ttl;
#endif
	sp->protocol = IPPROTO_SCTP;
	sp->localroute = sctp_defaults.ip.dontroute;
	sp->broadcast = sctp_defaults.ip.broadcast;
#if 0
	sp->priority = sctp_defaults.ip.priority;
#endif
	/* association defaults */
	sp->a_rwnd = sctp_defaults.xti.rcvbuf;
	sp->max_istr = sctp_defaults.sctp.istreams;
	sp->req_ostr = sctp_defaults.sctp.ostreams;
	sp->max_inits = sctp_defaults.sctp.max_init_retries;
	sp->max_retrans = sctp_defaults.sctp.assoc_max_retrans;
	sp->ck_life = sctp_defaults.sctp.cookie_life;
	sp->ck_inc = sctp_defaults.sctp.cookie_inc;
	sp->hmac = sctp_defaults.sctp.mac_type;
	sp->cksum = sctp_defaults.sctp.cksum_type;
	sp->throttle = sctp_defaults.sctp.throttle_itvl;
	sp->sid = sctp_defaults.sctp.sid;
	sp->ppi = sctp_defaults.sctp.ppi;
	sp->max_sack = sctp_defaults.sctp.sack_delay;
#ifdef ETSI
	sp->sack_freq = sctp_defaults.sctp.sack_freq;
#endif
	sp->max_burst = sctp_defaults.sctp.max_burst;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	sp->prel = sctp_defaults.sctp.pr;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	sp->l_caps = 0;
#ifdef SCTP_CONFIG_ADAPTATION_LAYER_INFO
	sp->l_ali = sctp_defaults.sctp.ali;
	if (sp->l_ali)
		sp->l_caps |= SCTP_CAPS_ALI;
#endif				/* SCTP_CONFIG_ADAPTATION_LAYER_INFO */
	sp->pmtu = ip_rt_min_pmtu;
	sp->amps = sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
	/* destination defaults */
	sp->rto_ini = sctp_defaults.sctp.rto_initial;
	sp->rto_min = sctp_defaults.sctp.rto_min;
	sp->rto_max = sctp_defaults.sctp.rto_max;
	sp->rtx_path = sctp_defaults.sctp.path_max_retrans;
	sp->hb_itvl = sctp_defaults.sctp.heartbeat_itvl;
	sctplogno(sp, "HZ is %d", (int) HZ);
	sctplogno(sp, "sp->rto_ini = %d", (int) sp->rto_ini);
	sctplogno(sp, "sp->rto_min = %d", (int) sp->rto_min);
	sctplogno(sp, "sp->rto_max = %d", (int) sp->rto_max);
	sctplogno(sp, "sp->rtx_path = %d", (int) sp->rtx_path);
	sctplogno(sp, "sp->hb_itvl = %d", (int) sp->hb_itvl);
}

/*
 *  /////////////////////////////////////////////////////////////////////////
 *
 *  STREAMS Interface Implementation
 *
 *  /////////////////////////////////////////////////////////////////////////
 */

/*
 *  =========================================================================
 *
 *  SCTP Private Data Structure Functions
 *
 *  =========================================================================
 *
 *  We use Linux hardware aligned cache here for speedy access to information contained in the
 *  private data structure.
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP ALLOC PRIV
 *
 *  --------------------------------------------------------------------------
 */
#ifndef ip_rt_min_pmtu
#define ip_rt_min_pmtu 552
#endif
struct sctp *
sctp_alloc_priv(queue_t *q, struct sctp **spp, int cmajor, int cminor, struct sctp_ifops *ops,
		cred_t *crp)
{
	struct sctp *sp;

	assert(q);
	assert(spp);
	/* must have these 4 */
	ensure(ops->conn_ind, return (NULL));
	ensure(ops->conn_con, return (NULL));
	ensure(ops->data_ind, return (NULL));
	ensure(ops->discon_ind, return (NULL));
	assure(cmajor);
	assure(cminor);
	if ((sp = sctp_get())) {
		RD(q)->q_ptr = WR(q)->q_ptr = sp;
		sp->rq = RD(q);
		sp->wq = WR(q);
		sp->cmajor = cmajor;
		sp->cminor = cminor;
		sp->ops = ops;
		sp->i_state = 0;
		sp->cred = *crp;
#if 1
		/* initialize queues */
		bufq_init(&sp->rcvq);
		bufq_init(&sp->sndq);
#endif
		sp->conind = 0;
		sctp_init_struct(sp);
		sctp_init_lock(sp);
		sp->users = 0;
		init_waitqueue_head(&sp->waitq);
		/* FIXME: must take list lock here! */
		/* link into master list */
		if ((sp->next = *spp))
			sp->next->prev = &sp->next;
		sp->prev = spp;
		*spp = sp;
		atomic_inc(&sctp_stream_count);
		qprocson(q);
	}
	usual(sp);
	return (sp);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP FREE PRIV
 *
 *  --------------------------------------------------------------------------
 */
STATIC void
sctp_free_priv(queue_t *q)
{
	struct sctp *sp;

	sp = sctp_sleeplock(q);	/* wait to lock queue */
	sctplogno(sp, "freeing private structure");
#if 0
	/* This causes really big problems on SMP for some reason: just drop the Stream and let the 
	   protocol module respond with an abort. */
	if ((1 << sp->state) & SCTPF_NEEDABORT)	/* SCTP IG 2.21 */
		sctp_send_abort_error(sp, SCTP_CAUSE_USER_INITIATED, NULL, 0);
#endif
	/* cancel bufcalls */
	sctp_unbufcall(sp);
	/* free timers */
	sp->timers = 0;
	sp_timer_cancel_init(sp);
	sp_timer_cancel_cookie(sp);
	sp_timer_cancel_shutdown(sp);
	sp_timer_cancel_guard(sp);
	sp_timer_cancel_sack(sp);
#ifdef SCTP_CONFIG_ADD_IP
	sp_timer_cancel_asconf(sp);
#endif				/* SCTP_CONFIG_ADD_IP */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	sp_timer_cancel_life(sp);
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
	qprocsoff(q);
	sctp_unbind(sp);

	/* once more for fun */
	/* cancel bufcalls */
	sctp_unbufcall(sp);
	/* free timers */
	sp->timers = 0;
	sp_timer_cancel_init(sp);
	sp_timer_cancel_cookie(sp);
	sp_timer_cancel_shutdown(sp);
	sp_timer_cancel_guard(sp);
	sp_timer_cancel_sack(sp);
#ifdef SCTP_CONFIG_ADD_IP
	sp_timer_cancel_asconf(sp);
#endif				/* SCTP_CONFIG_ADD_IP */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	sp_timer_cancel_life(sp);
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */

	/* take off master list */
	spin_lock(&sctp_protolock);
	if ((*(sp->prev) = sp->next))
		sp->next->prev = sp->prev;
	sp->next = NULL;
	sp->prev = NULL;
	atomic_dec(&sctp_stream_count);
	spin_unlock(&sctp_protolock);

	sctplogno(sp, "there are now %d streams allocated", atomic_read(&sctp_stream_count));
	usual(atomic_read(&sp->refcnt) == 1);
	if (atomic_read(&sp->refcnt) == 1) {
		sctplogno(sp, "immediate close of stream");
	} else {
		sctplogno(sp, "delayed close of stream, refcnt = %d", atomic_read(&sp->refcnt));
	}
	sp->rq = sp->wq = NULL;
	sctp_put(sp);
}

/*
 *  SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
 *
 *  Common STREAMS functions
 *
 *  SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
 */

/**
 * ___sctp_deferred_timers_slow: - process deferred timeouts
 * @sp: private structure (locked)
 */
noinline fastcall void
___sctp_deferred_timers_slow(struct sctp *sp, uint timers)
{
	if (timers & SCTP_TBIT_INIT)
		__sctp_init_timeout(sp);
	if (timers & SCTP_TBIT_COOKIE)
		__sctp_cookie_timeout(sp);
	if (timers & SCTP_TBIT_SHUTDOWN)
		__sctp_shutdown_timeout(sp);
	if (timers & SCTP_TBIT_GUARD)
		__sctp_guard_timeout(sp);
	if (timers & SCTP_TBIT_SACK)
		__sctp_sack_timeout(sp);
	if (timers & SCTP_TBIT_ASCONF)
		__sctp_asconf_timeout(sp);
	if (timers & SCTP_TBIT_LIFE)
		__sctp_life_timeout(sp);
	if (timers & (SCTP_TBIT_HEARTBEAT | SCTP_TBIT_RETRANS | SCTP_TBIT_IDLE)) {
		struct sctp_daddr *sd;

		for (sd = sp->daddr; sd; sd = sd->next) {
			if (likely((timers = xchg(&sd->timers, 0)) != 0)) {
				if (timers & SCTP_TBIT_HEARTBEAT)
					__sctp_heartbeat_timeout(sd);
				if (timers & SCTP_TBIT_RETRANS)
					__sctp_retrans_timeout(sd);
				if (timers & SCTP_TBIT_IDLE)
					__sctp_idle_timeout(sd);
			}
		}
	}
}

/**
 * ___sctp_deferred_timers: - process deferred timeouts
 * @sp: private structure (locked)
 */
STATIC inline fastcall __hot_in void
___sctp_deferred_timers(struct sctp *sp)
{
	uint timers;

	if (likely((timers = xchg(&sp->timers, 0)) == 0))
		return;
	___sctp_deferred_timers_slow(sp, timers);
}

/**
 * _sctp_deferred_timers: - process deferred timeouts
 * @q: active queue (read queue)
 */
STATIC inline fastcall void
_sctp_deferred_timers(queue_t *q)
{
	struct sctp *sp = SCTP_PRIV(q);

	___sctp_deferred_timers(sp);
}

/**
 * ___sctp_cleanup_read: - clean up read queues
 * @sp: private structure (locked)
 *
 * This is called to clean up the read queue by the STREAMS read service routine.  This permits
 * backenabling to work.
 */
STATIC inline streamscall __hot_in void
___sctp_cleanup_read(struct sctp *sp)
{
	mblk_t *mp;

	assert(sp);

	/* do not deliver data when disconnected */
	if ((1 << sp->state) & (SCTPF_DISCONNECTED))
		goto skip;

	/* converted to work without locks head across putnext */
	while ((mp = bufq_dequeue(&sp->ackq))) {
		sctp_tcb_t *cb = SCTP_TCB(mp);

		if (!sctp_datack_ind(sp, cb->ppi, cb->sid, cb->ssn, cb->tsn)) {
			freemsg(mp);
			continue;
		}
		bufq_queue_head(&sp->ackq, mp);
		break;		/* error on delivery (ENOBUFS, EBUSY) */
	}
	if (bufq_head(&sp->rcvq) || bufq_head(&sp->expq)) {
		int need_sack = ((sp->a_rwnd << 1) <= bufq_size(&sp->oooq)
				 + bufq_size(&sp->dupq)
				 + bufq_size(&sp->rcvq)
				 + bufq_size(&sp->expq));

		/* converted to work without locks head across putnext */
		while ((mp = bufq_dequeue(&sp->expq)) || (mp = bufq_dequeue(&sp->rcvq))) {
			sctp_tcb_t *cb = SCTP_TCB(mp);
			struct sctp_strm *st = cb->st;
			int ord = !(cb->flags & SCTPCB_FLAG_URG);
			int more = (!(cb->flags & SCTPCB_FLAG_LAST_FRAG)) ? SCTP_STRMF_MORE : 0;

			assert(st != NULL);
			if (!sctp_data_ind(sp, cb->ppi, cb->sid, cb->ssn, cb->tsn,
					   ord, more, mp->b_cont)) {
				if (ord) {
					if (!more)
						st->n.more &= ~SCTP_STRMF_MORE;
					else
						st->n.more |= SCTP_STRMF_MORE;
					st->ssn = cb->ssn;
					SCTP_INC_STATS(SctpInOrderChunks);
				} else {
					if (!more)
						st->x.more &= ~SCTP_STRMF_MORE;
					else
						st->x.more |= SCTP_STRMF_MORE;
					SCTP_INC_STATS(SctpInUnorderChunks);
				}
				freeb(mp);
				if (!need_sack)
					continue;
				/* Should really do SWS here.  */
				sctp_send_sack(sp);
				need_sack = 0;
				continue;
			}
			bufq_queue_head(ord ? &sp->rcvq : &sp->expq, mp);
			break;	/* error on delivery (ENOBUFS, EBUSY, EPROTO) */
		}
	}
      skip:
	if (sp->origin && sp->reason) {
		/* This is, I believe, the source of a lot of out of state disconnect indication
		   errors.  There are a bunch in the idle state and one in the TS_WACK_CREQ state,
		   but I belive that the latter is due to a flush instead. */
		if (sctp_discon_ind(sp, sp->origin, sp->reason, NULL) < 0) {
			qenable(sp->rq);
			return;
		}
	}
	sp->origin = 0;
	sp->reason = 0;
}

/**
 * sctp_r_other: - other messages on read queue
 * @q: active queue (read queue)
 * @mp: the unexpected message
 *
 * If SCTP is pushed as a module, pass unexpected messages along with flow control.  If a driver,
 * discard them.  This function is called with the private structure unlocked.  It returns either
 * -%EBUSY if SCTP is a module and flow control prevents passing the message, or QR_ABSORBED in all
 * other cases.
 */
STATIC int
sctp_r_other(queue_t *q, mblk_t *mp)
{
	if (OTHERQ(q)->q_next != NULL) {
		/* module */
		if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
			return (-EBUSY);
		putnext(q, mp);
	} else {
		/* driver */
		rare();
		cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
			SCTP_PRIV(q)->cminor);
		freemsg(mp);
	}
	return (QR_ABSORBED);
}

/**
 * sctp_r_data: M_DATA handling
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 *
 * M_DATA messages are IP packets received from the IP layer.  This procedure must do its own
 * locking.  Return -%EAGAIN if the lock cannot be acquired to cause the message to be (re)queued at
 * the read queue.
 */
noinline fastcall int
sctp_r_data(queue_t *q, mblk_t *mp)
{
	struct sctp *sp;

	if (likely((sp = sctp_trylockq(q)) != NULL)) {
		int rtn;

		rtn = sctp_recv_msg(sp, mp);
		___sctp_cleanup_read(sp);
		___sctp_transmit_wakeup(sp);
		sctp_unlockq(sp);
		return (rtn);
	}
	return (-EDEADLK);
}

/**
 * sctp_r_ctl: M_CTL handling
 * @q: active queue (read queue)
 * @mp: the M_CTL message
 *
 * M_CTL messages are ICMP packets received from the IP layer.  This procedure must do its own
 * locking.  Return -%EDEADLK if the lock cannot be acquired to cause the message to be (re)queued
 * at the read queue.
 */
noinline fastcall int
sctp_r_ctl(queue_t *q, mblk_t *mp)
{
	struct sctp *sp;

	if (likely((sp = sctp_trylockq(q)) != NULL)) {
		int rtn;

		rtn = sctp_recv_err(sp, mp);
		___sctp_cleanup_read(sp);
		___sctp_transmit_wakeup(sp);
		sctp_unlockq(sp);
		return (rtn);
	}
	return (-EDEADLK);
}

/**
 * sctp_r_flush: M_FLUSH handling
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 *
 * As we are a driver it is not normal to receive a flush message on the read queue (unless we
 * explicitly place it there); however, at some point this driver might be pushed as a module over
 * an IP driver stream in which case we will need to perform proper canonical flushing.  The private
 * structure is unlocked at this point.  This function always consumes the message and returns
 * QR_ABSORBED.
 */
noinline fastcall __unlikely int
sctp_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		putnext(q, mp);
	} else if (mp->b_rptr[0] & FLUSHW) {
		putnext(q, mp);
	} else
		freemsg(mp);
	return (QR_ABSORBED);
}

/**
 * sctp_w_flush: M_FLUSH handling
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 *
 * This is flush handling on the write queue.  Note that in the future SCTP could be pushed as
 * module over an IP stream, in which case, we want to automagically handle both canonical module
 * and driver flushing.  No locks are taken at this point.  This function always consumes the
 * message and returns QR_ABSORBED.
 */
noinline fastcall int
sctp_w_flush(queue_t *q, mblk_t *mp)
{
	if (unlikely(q->q_next == NULL)) {
		/* canonical driver flush */
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			*mp->b_rptr &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(OTHERQ(q), mp->b_rptr[1], FLUSHALL);
			else
				flushq(OTHERQ(q), FLUSHALL);
			qreply(q, mp);
		} else
			freemsg(mp);
	} else {
		/* canonical module flush */
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			putnext(q, mp);
		} else if (mp->b_rptr[0] & FLUSHW) {
			putnext(q, mp);
		} else
			freemsg(mp);
	}
	return (QR_ABSORBED);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */

/**
 * sctp_r_prim_put: - read primitive handling
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 * This is the put procedure version of message handling.  All procedures that require private
 * structure locks take them.  Note that we do not expect M_DATA or M_CTL here unless SCTP is pushed
 * as a module: sctp_v4_rcv() and sctp_v4_err() place messages directly on the queue for processing
 * by the service procedure.
 */
STATIC inline streamscall __hot_in int
sctp_r_prim_put(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_FLUSH)) {
	case M_FLUSH:
		return sctp_r_flush(q, mp);
	case M_DATA:
		return sctp_r_data(q, mp);
	case M_CTL:
		return sctp_r_ctl(q, mp);
	default:
		return sctp_r_other(q, mp);
	}
}

/**
 * sctp_rput: - read put procedure
 * @q: active queue (read queue)
 * @mp: message to put
 *
 * This is a canonical put procedure for the read queue.  Messages placed on the read queue either
 * come from this module internally (e.g. timers), the IP sublayer or when SCTP is pushed as a
 * module from the driver.  Never call put() from soft interrupt.  In sctp_v4_rcv() and
 * sctp_v4_err() messages are placed directly on the queue with putq().
 */
STATIC streamscall __hot_in int
sctp_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(sctp_r_prim_put(q, mp) != QR_ABSORBED)) {
		// q->q_qinfo->qi_mstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* must succeed */
		}
	}
	return (0);
}

/**
 * sctp_r_prim_srv: - read primitive handling
 * @q: active queue (read queue)
 * @mp: the primitive
 *
 */
STATIC inline streamscall __hot_in int
sctp_r_prim_srv(struct sctp *sp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_DATA)) {
	case M_DATA:
		return sctp_recv_msg(sp, mp);
	case M_CTL:
		return sctp_recv_err(sp, mp);
	case M_FLUSH:
		return sctp_r_flush(q, mp);
	default:
		return sctp_r_other(q, mp);
	}
}

/**
 * sctp_rsrv: - read service procedure
 * @q: active queue (read queue)
 * @mp: message to put
 *
 * This is a canonical service procedure for the read queue.  Messages placed on the read queue
 * either come from this module internally, the IP sublayer or when SCTP is pushed as a module from
 * the driver.  The service procedure takes private structure locks once for the entire loop for
 * speed.
 */
STATIC streamscall __hot_in int
sctp_rsrv(queue_t *q)
{
	struct sctp *sp;

	if (likely((sp = sctp_trylockq(q)) != NULL)) {
		mblk_t *mp;
		int rtn;

		___sctp_deferred_timers(sp);

		if (likely((mp = getq(q)) != NULL)) {
			___sctp_cleanup_read(sp);
			do {
				if (unlikely((rtn = sctp_r_prim_srv(sp, q, mp)) != QR_ABSORBED)) {
					if (unlikely(!putbq(q, mp))) {
						mp->b_band = 0;
						putbq(q, mp);	/* must succeed */
					}
					sctplogrx(sp, "read queue stalled %d", rtn);
					___sctp_transmit_wakeup(sp);
					break;
				}
			} while (likely((mp = getq(q)) != NULL));
		}
		___sctp_cleanup_read(sp);
		___sctp_transmit_wakeup(sp);
		sctp_unlockq(sp);
	}
	return (0);
}

/*
 *  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 *
 *  Multiplex Definitions
 *
 *  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#if defined WITH_NPI_IP_MUX

STATIC struct module_info sctp_m_info = {
	.mi_idnum = 0,			/* Module ID number (nothing for mux) */
	.mi_idname = "sctp_m",		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = (1 << 18),		/* Hi water mark */
	.mi_lowat = (1 << 16),		/* Lo water mark */
};

STATIC streamscall int sctp_m_open(queue_t *q, dev_t *, int, int, cred_t *);
STATIC streamscall int sctp_m_close(queue_t *q, int, cred_t *);

STATIC streamscall int sctp_m_rput(queue_t *q, mblk_t *);
STATIC streamscall int sctp_m_rsrv(queue_t *q);

static struct qinit sctp_m_rinit = {
	.qi_putp = sctp_m_rput,		/* Read put (msg from below) */
	.qi_srvp = sctp_m_rsrv,		/* Read queue service */
	.qi_qopen = sctp_m_open,	/* Each open */
	.qi_qclose = sctp_m_close,	/* Last close */
	.qi_minfo = &sctp_m_minfo,	/* Information */
};

STATIC streamscall int sctp_m_wput(queue_t *q, mblk_t *);
STATIC streamscall int sctp_m_wsrv(queue_t *q);

static struct qinit sctp_m_winit = {
	.qi_putp = sctp_m_wput,		/* Write put (msg from above) */
	.qi_srvp = sctp_m_wsrv,		/* Write queue service */
	.qi_minfo = &sctp_m_minfo,	/* Information */
};

#endif				/* defined WITH_NPI_IP_MUX */

/**
 * m_flush - generate an M_FLUSH message upstream
 * @sp: private structure
 * @msg: message to free upon success
 * @how: how to flush the stream
 * @band: band to flush (if FLUSHBAND set in @how)
 *
 * Generates an M_FLUSH message upstream.  This is used to flush the stream when required by the TPI
 * or NPI specifications.  It returns -ENOBUFS if a buffer could not be allocated (and a bufcall is
 * pending) or QR_ABSORBED when successful.
 */
noinline fastcall __unlikely int
m_flush(struct sctp *sp, mblk_t *msg, int how, int band)
{
	mblk_t *mp;

	if ((mp = sctp_allocb(sp, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_FLUSH;
		*mp->b_wptr++ = how;
		*mp->b_wptr++ = band;
		flushq(sp->rq, FLUSHALL);
		freemsg(msg);
		sctplogtx(sp, "<- M_FLUSH");
		putnext(sp->rq, mp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

/**
 * m_error - generate an M_ERROR message upstream
 * @sp: private structure
 * @msg: message to free upon success
 * @error: error to place in the M_ERROR_ message
 *
 * Generates an M_ERROR message upstream.  This is used to generate fatal errors when required by
 * the TPI or NPI specifications.  It returns -ENOBUFS if a buffer could not be allocated (and a
 * bufcall is pending) or QR_ABSORBED when successful.
 */
STATIC inline fastcall __unlikely int
m_error(struct sctp *sp, mblk_t *msg, int error)
{
	mblk_t *mp;

	if ((mp = sctp_allocb(sp, 2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = error;
		*mp->b_wptr++ = error;
		freemsg(msg);
		sctplogtx(sp, "<- M_ERROR");
		putnext(sp->rq, mp);
		if ((1 << sp->state) & SCTPF_NEEDABORT)	/* SCTP IG 2.21 */
			sctp_send_abort_error(sp, SCTP_CAUSE_USER_INITIATED, NULL, 0);
		sctp_unbind(sp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

/**
 * m_hangup - generate an M_HANGUP messag eupstream
 * @sp: private structure
 * @msg: message to free upon success
 *
 * Generates an M_HANGUP message upstream.  This is used to generate fatal errors when required by
 * the TPI or NPI specifications.  It returns -ENOBUFS if a buffer could not be allocated (and a
 * bufcall is pending) or QR_ABSORBED when successful.
 */
STATIC inline fastcall __unlikely int
m_hangup(struct sctp *sp, mblk_t *msg)
{
	mblk_t *mp;

	if ((mp = sctp_allocb(sp, 0, BPRI_HI))) {
		mp->b_datap->db_type = M_HANGUP;
		freemsg(msg);
		sctplogtx(sp, "<- M_HANGUP");
		putnext(sp->rq, mp);
		if ((1 << sp->state) & SCTPF_NEEDABORT)	/* SCTP IG 2.21 */
			sctp_send_abort_error(sp, SCTP_CAUSE_USER_INITIATED, NULL, 0);
		sctp_unbind(sp);
		return (QR_ABSORBED);
	}
	return (-ENOBUFS);
}

/**
 * m_error_reply - reply with an M_ERROR or M_HANGUP message
 * @sp: private structure
 * @msg: message to free upon success
 * @err: error to place in M_ERROR message
 *
 * Generates an M_ERROR or M_HANGUP message upstream to generate a fatal error when required by the
 * TPI or NPI specification.  The four errors that require the message to be placed on the message
 * queue (%EBUSY, %ENOBUFS, %EAGAIN, %ENOMEM) are simply returned.  Other errors are either
 * translated into an M_ERROR or M_HANGUP message.  If a message block cannot be allocated -%ENOBUFS
 * is returns, otherwise %QR_ABSORBED is returned.
 */
noinline fastcall __unlikely int
m_error_reply(struct sctp *sp, mblk_t *msg, int err)
{
	int hangup = 0;
	int error = err;

	switch (error) {
	case -EBUSY:
	case -ENOBUFS:
	case -EAGAIN:
	case -ENOMEM:
	case -EDEADLK:
		return (error);
	case QR_DONE:
		freemsg(msg);
		return (QR_ABSORBED);
	case -EPIPE:
	case -ENETDOWN:
	case -EHOSTUNREACH:
		hangup = 1;
		error = EPIPE;
		break;
	case -EFAULT:
		sctplogerr(sp, "%s() fault", __FUNCTION__);
	default:
	case -EPROTO:
		error = (err < 0) ? -err : err;
		break;
	}
	if (hangup)
		return m_hangup(sp, msg);
	return m_error(sp, msg, error);
}

/*
 *  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 *
 *  Network Provider Interface (NPI) Interface Definitions
 *
 *  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 *  This driver defines two user interfaces: one NPI, the other TPI.
 */

#ifndef _OPTIMIZE_SPEED
STATIC const char *
sctp_n_statename(t_scalar_t state)
{
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
	default:
		return ("????");
	}
}

STATIC inline void
sctp_n_setstate(struct sctp *sp, t_scalar_t newstate)
{
	t_scalar_t oldstate;
	
	if ((oldstate = XCHG(&sp->i_state, newstate)) != newstate)
		sctplogst(sp, "%s <- %s", sctp_n_statename(newstate), sctp_n_statename(oldstate));
}
#else
#define sctp_n_setstate(sp, newstate) do { sp->i_state = newstate; } while (0)
#endif

#ifdef LFS
#define SCTP_N_DRV_ID	    CONFIG_STREAMS_SCTP_N_MODID
#define SCTP_N_DRV_NAME	    CONFIG_STREAMS_SCTP_N_NAME
#define SCTP_N_CMAJORS	    CONFIG_STREAMS_SCTP_N_NMAJORS
#define SCTP_N_CMAJOR_0	    CONFIG_STREAMS_SCTP_N_MAJOR
#define SCTP_N_UNITS	    CONFIG_STREAMS_SCTP_N_NMINORS
#endif				/* LFS */

#undef DRV_ID
#undef DRV_NAME
#undef CMAJORS
#undef CMAJOR_0
#undef UNITS

#define DRV_ID		SCTP_N_DRV_ID
#define DRV_NAME	SCTP_N_DRV_NAME
#define CMAJORS		SCTP_N_CMAJORS
#define CMAJOR_0	SCTP_N_CMAJOR_0
#define UNITS		SCTP_N_UNITS

STATIC struct module_info sctp_n_rinfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT << 3,	/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC struct module_info sctp_n_winfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT,		/* Hi water mark */
	.mi_lowat = SHEADLOWAT,		/* Lo water mark */
};

STATIC struct module_stat sctp_n_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
STATIC struct module_stat sctp_n_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

STATIC streamscall int sctp_n_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int sctp_n_close(queue_t *, int, cred_t *);

STATIC streamscall int sctp_rput(queue_t *, mblk_t *);
STATIC streamscall int sctp_rsrv(queue_t *);

STATIC struct qinit sctp_n_rinit = {
	.qi_putp = sctp_rput,		/* Read put (msg from below) */
	.qi_srvp = sctp_rsrv,		/* Read queue service */
	.qi_qopen = sctp_n_open,	/* Each open */
	.qi_qclose = sctp_n_close,	/* Last close */
	.qi_minfo = &sctp_n_rinfo,	/* Information */
	.qi_mstat = &sctp_n_rstat,	/* Statistics */
};

STATIC streamscall int sctp_n_wput(queue_t *, mblk_t *);
STATIC streamscall int sctp_n_wsrv(queue_t *);

STATIC struct qinit sctp_n_winit = {
	.qi_putp = sctp_n_wput,		/* Write put (msg from above) */
	.qi_srvp = sctp_n_wsrv,		/* Write queue service */
	.qi_minfo = &sctp_n_winfo,	/* Information */
	.qi_mstat = &sctp_n_wstat,	/* Statistics */
};

STATIC struct streamtab sctp_n_info = {
	.st_rdinit = &sctp_n_rinit,	/* Upper read queue */
	.st_wrinit = &sctp_n_winit,	/* Upper write queue */
#if defined WITH_NPI_IP_MUX
	.st_muxrinit = &sctp_m_rinit,	/* Lower read queue */
	.st_muxwinit = &sctp_m_winit,	/* Lower write queue */
#endif					/* defined WITH_NPI_IP_MUX */
};

/*
 *  =========================================================================
 *
 *  NPI Provider (SCTP) -> NPI User Primitives
 *
 *  =========================================================================
 */
/**
 * n_conn_ind - generate an N_CONN_IND primitive upstream
 * @sp: private structure
 * @cp: connection pointer
 *
 * Generates an %N_CONN_IND incoming connection indication primitive upstream.
 */
STATIC int
n_conn_ind(struct sctp *sp, mblk_t *cp)
{
	mblk_t *mp, *bp;
	N_conn_ind_t *p;
	N_qos_sel_conn_sctp_t *q;
	struct sctp_cookie_echo *m = (struct sctp_cookie_echo *) cp->b_rptr;
	struct sctp_cookie *ck = (struct sctp_cookie *) m->cookie;
	size_t danum = ck->danum + 1;
	uint32_t *daptr = (uint32_t *) (((caddr_t) (ck + 1) + ck->opt_len));
	struct sockaddr_in *sin;
	size_t dst_len = sizeof(*sin);
	size_t src_len = danum * sizeof(*sin);
	size_t qos_len = sizeof(*q);

	if ((1 << sp->i_state) & ~(NSF_IDLE | NSF_WRES_CIND))
		goto outstate;
	if (bufq_length(&sp->conq) >= sp->conind)
		goto erestart;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + dst_len + src_len + qos_len, BPRI_MED)))
		goto enobufs;
	if (!(bp = sctp_dupmsg(sp, cp)))
		goto enodup;
	mp->b_datap->db_type = M_PROTO;
	p = (N_conn_ind_t *) mp->b_wptr;
	p->PRIM_type = N_CONN_IND;
	p->DEST_length = dst_len;
	p->DEST_offset = dst_len ? sizeof(*p) : 0;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
	p->SEQ_number = (t_uscalar_t) (long) bp;
	p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
	mp->b_wptr += sizeof(*p);
	sin = (typeof(sin)) mp->b_wptr;
	sin->sin_family = AF_INET;
	sin->sin_port = ck->sport;
	sin->sin_addr.s_addr = ck->saddr;
	mp->b_wptr += sizeof(*sin);
	/* place address information from cookie */
	if (danum--) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = ck->dport;
		sin->sin_addr.s_addr = ck->daddr;
		mp->b_wptr += sizeof(*sin);
	}
	while (danum--) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = ck->dport;
		sin->sin_addr.s_addr = *daptr++;
		mp->b_wptr += sizeof(*sin);
	}
	q = (N_qos_sel_conn_sctp_t *) mp->b_wptr;
	q->n_qos_type = N_QOS_SEL_CONN_SCTP;
	q->i_streams = ck->n_istr;
	q->o_streams = ck->n_ostr;
	mp->b_wptr += sizeof(*q);
	bufq_queue(&sp->conq, bp);
	sctp_n_setstate(sp, NS_WRES_CIND);
	sctplogtx(sp, "<- N_CONN_IND");
	putnext(sp->rq, mp);
	return (0);
      enodup:
	freemsg(mp);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "%s() flow controlled", __FUNCTION__);
	return (-EBUSY);
      erestart:
	sctplogtx(sp, "%s() too many connection indications", __FUNCTION__);
	return (-ERESTART);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (-ECONNREFUSED);
}

/*
 *  N_CONN_CON      12 - Connection established
 *  ---------------------------------------------------------------
 */
STATIC int
n_conn_con(struct sctp *sp)
{
	mblk_t *mp;
	N_conn_con_t *p;
	N_qos_sel_conn_sctp_t *q;
	struct sctp_daddr *sd;
	struct sockaddr_in *sin;
	size_t res_len = sp->danum * sizeof(*sin);
	size_t qos_len = sizeof(*q);

	if (sp->i_state != NS_WCON_CREQ)
		goto outstate;
	if (!bcanputnext(sp->rq, 1))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + res_len + qos_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (N_conn_con_t *) mp->b_wptr;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
	mp->b_wptr += sizeof(*p);
	for (sd = sp->daddr; sd; sd = sd->next) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = sp->dport;
		sin->sin_addr.s_addr = sd->daddr;
		mp->b_wptr += sizeof(*sin);
	}
	q = (N_qos_sel_conn_sctp_t *) mp->b_wptr;
	q->n_qos_type = N_QOS_SEL_CONN_SCTP;
	q->i_streams = sp->n_istr;
	q->o_streams = sp->n_ostr;
	mp->b_wptr += sizeof(*q);
	sctp_n_setstate(sp, NS_DATA_XFER);
	sctplogtx(sp, "<- N_CONN_CON");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "%s() flow controlled", __FUNCTION__);
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
}

/**
 * n_discon_ind: - issue N_DISCON_IND (13 - NC disconnected)
 * @sp: private structure (locked)
 * @orig: origin of disconnect
 * @reason: reason for disconnect
 * @seq: disconnecting sequence number
 *
 * For SCTP the responding address in a NC connection refusal is always the destination address to
 * which the NC connection request was sent.  If a connection indication identifier (seq) is
 * provided, this function must either free it or return an error.  The origin is used only by the
 * STREAMS NPI interface.
 */
STATIC int
n_discon_ind(struct sctp *sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *seq)
{
	mblk_t *mp;
	N_discon_ind_t *p;

	if ((1 << sp->i_state) & ~NSM_CONNECTED)
		goto outstate;
	/* TPI spec says that if the interface is in the TS_DATA_XFER, TS_WIND_ORDREL or
	   TS_WACK_ORDREL [sic] state, the stream must be flushed before sending up the
	   T_DISCON_IND primitive. */
	if ((1 << sp->i_state) & (NSM_INDATA | NSM_OUTDATA))
		if (m_flush(sp, NULL, FLUSHRW, 0) == -ENOBUFS)
			goto enobufs;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	if (!canputnext(sp->rq))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (N_discon_ind_t *) mp->b_wptr;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = orig;
	p->DISCON_reason = reason;
	/* FIXME: We really need to map SCTP core protocol engine reasons to NPI reasons here.  All 
	   negative reasons are UNIX error codes, all positive reasons are SCTP cause values. */
	p->RES_length = 0;
	p->RES_offset = 0;
	p->SEQ_number = (t_uscalar_t) (long) seq;
	mp->b_wptr += sizeof(*p);
	if (seq)
		freemsg(seq);
	if (!bufq_length(&sp->conq))
		sctp_n_setstate(sp, NS_IDLE);
	else
		sctp_n_setstate(sp, NS_WRES_CIND);
	sctplogtx(sp, "<- N_DISCON_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	freemsg(mp);
	return (-EBUSY);
      outstate:
	/* Sometimes this function is called in the idle state. */
	if (sp->i_state != NS_IDLE)
		sctplogerr(sp, "%s() in NPI state %s", __FUNCTION__, sctp_n_statename(sp->i_state));
	return (0);
}

/**
 * n_data_ind: - issue N_DATA_IND (14 - Incoming connection-mode data indication)
 * @sp: private structure (locked)
 * @more: another primitive completes the TSDU
 * @dp: user data
 *
 * This indication is only useful for delivering data indications for the default stream.  The
 * caller should check that the ppi and sid match the default before using this indication.  Note
 * that NPI does not really have a data indication with options similar to the T_OPTDATA_IND of TPI,
 * so we add a QOS structure immediately following the N_DATA_IND primitive that can be used by the
 * application.  This should be compatible with standard behaviour as the primitive is just slightly
 * larger, but otherwise compatible.
 */
STATIC int
n_data_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint more,
	   mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	N_qos_sel_data_sctp_t *q;

#if 0
	if (!((1 << sp->i_state) & NSM_INDATA)) {
		sctplogda(sp, "interface in state %d", (int) sp->i_state);
		sctplogda(sp, "mblk size is %d", (int) msgdsize(dp));
	}
#endif
	if ((1 << sp->i_state) & ~NSM_INDATA)
		goto outstate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + sizeof(*q), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (N_data_ind_t *) mp->b_wptr;
	p->PRIM_type = N_DATA_IND;
	p->DATA_xfer_flags = (more ? N_MORE_DATA_FLAG : 0);
	mp->b_wptr += sizeof(*p);
	q = (N_qos_sel_data_sctp_t *) mp->b_wptr;
	q->n_qos_type = N_QOS_SEL_DATA_SCTP;
	q->ppi = ppi;
	q->sid = sid;
	q->ssn = ssn;
	q->tsn = tsn;
	q->more = more ? 1 : 0;
	mp->b_wptr += sizeof(*q);
	mp->b_cont = dp;
	sctplogda(sp, "<- N_DATA_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "%s() flow controlled", __FUNCTION__);
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	freemsg(dp);
	return (0);
}

/**
 * n_exdata_ind: - issue N_EXDATA_IND (15 - Incoming expedited data indication)
 * @sp: private structure (locked)
 * @ppi: payload protocol identifier
 * @sid: SCTP stream identifier
 * @ssn: SCTP stream sequence number
 * @tsn: transmit sequence number
 * @more: subsequent primitive completes NSDU
 * @dp: user data
 * 
 * The lack of a more flag presents a challenge.  Perhaps we should be reassembling expedited data.
 */
STATIC int
n_exdata_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint more,
	     mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;
	N_qos_sel_data_sctp_t *q;

	if ((1 << sp->i_state) & ~NSM_INDATA)
		goto outstate;
	if (!bcanputnext(sp->rq, 1))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + sizeof(*q), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (N_exdata_ind_t *) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);
	q = (N_qos_sel_data_sctp_t *) mp->b_wptr;
	q->n_qos_type = N_QOS_SEL_DATA_SCTP;
	q->ppi = ppi;
	q->sid = sid;
	q->ssn = ssn;
	q->tsn = tsn;
	q->more = more ? 1 : 0;
	mp->b_wptr += sizeof(*q);
	mp->b_cont = dp;
	sctplogda(sp, "<- N_EXDATA_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "%s() flow controlled", __FUNCTION__);
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in NPI state %s", __FUNCTION__, sctp_n_statename(sp->i_state));
	freemsg(dp);
	return (0);
}

/*
 *  N_INFO_ACK      16 - Information Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_info_ack(struct sctp *sp)
{
	mblk_t *mp;
	N_info_ack_t *p;

#ifdef ETSI
	N_qos_sel_info_sctp2_t *qos;
#else
	N_qos_sel_info_sctp_t *qos;
#endif
	N_qos_range_info_sctp_t *qor;
	size_t add_len = sp->sanum ? sizeof(uint16_t) + sp->sanum * sizeof(uint32_t) : 0;
	size_t qos_len = sizeof(*qos);
	size_t qor_len = sizeof(*qor);
	size_t pro_len = sizeof(uint32_t);
	struct sctp_saddr *ss = sp->saddr;

	if (!(mp = sctp_allocb(sp, sizeof(*p) + add_len + qos_len + qor_len + pro_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (N_info_ack_t *) mp->b_wptr;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = -1;	/* no limit on NSDU size */
	p->ENSDU_size = -1;	/* no limit on ENSDU size */
	p->CDATA_size = -1;	/* no limit on CDATA size */
	p->DDATA_size = -1;	/* no limit on DDATA size */
	p->ADDR_size = -1;	/* no limit on ADDR size */
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->QOS_length = qos_len;
	p->QOS_offset = qos_len ? sizeof(*p) + add_len : 0;
	p->QOS_range_length = qor_len;
	p->QOS_range_offset = qor_len ? sizeof(*p) + add_len + qos_len : 0;
	p->OPTIONS_flags =
	    REC_CONF_OPT | EX_DATA_OPT | ((sp->flags & SCTP_FLAG_DEFAULT_RC_SEL) ? DEFAULT_RC_SEL :
					  0);
	p->NIDU_size = -1;	/* no limit on NIDU size */
	p->SERV_type = N_CONS;
	p->CURRENT_state = sp->i_state;
	p->PROVIDER_type = N_SNICFP;
	p->NODU_size = sp->pmtu >> 1;
	p->PROTOID_length = pro_len;
	p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + qor_len : 0;
	p->NPI_version = N_VERSION_2;
	mp->b_wptr += sizeof(*p);
	if (ss) {
		*(uint16_t *) mp->b_wptr = sp->sport;
		mp->b_wptr += sizeof(uint16_t);
	}
	for (; ss; ss = ss->next) {
		*(uint32_t *) mp->b_wptr = ss->saddr;
		mp->b_wptr += sizeof(uint32_t);
	}
#ifdef ETSI
	qos = (N_qos_sel_info_sctp2_t *) mp->b_wptr;
#else
	qos = (N_qos_sel_info_sctp_t *) mp->b_wptr;
#endif
	/* report N_QOS_SEL_INFO_SCTP but actually provide N_QOS_SEL_INFO_SCTP2 */
	qos->n_qos_type = N_QOS_SEL_INFO_SCTP;
	if ((1 << sp->i_state) & (NSF_UNBND | NSF_IDLE | NSF_WCON_CREQ)) {
		qos->i_streams = sp->max_istr;
		qos->o_streams = sp->req_ostr;
	} else {
		qos->i_streams = sp->n_istr;
		qos->o_streams = sp->n_ostr;
	}
	qos->ppi = sp->ppi;
	qos->sid = sp->sid;
	qos->max_inits = sp->max_inits;
	qos->max_retrans = sp->max_retrans;
	qos->ck_life = sp->ck_life;
	qos->ck_inc = sp->ck_inc;
	qos->hmac = sp->hmac;
	qos->throttle = sp->throttle;
	qos->max_sack = sp->max_sack;
#ifdef ETSI
	qos->sack_freq = sp->sack_freq;
#endif
	qos->rto_ini = sp->rto_ini;
	qos->rto_min = sp->rto_min;
	qos->rto_max = sp->rto_max;
	qos->rtx_path = sp->rtx_path;
	qos->hb_itvl = sp->hb_itvl;
	mp->b_wptr += sizeof(*qos);
	qor = (N_qos_range_info_sctp_t *) mp->b_wptr;
	qor->n_qos_type = N_QOS_RANGE_INFO_SCTP;
	mp->b_wptr += sizeof(*qor);
	{
		*(uint32_t *) mp->b_wptr = sp->ppi;
		mp->b_wptr += sizeof(uint32_t);
	}
	sctplogtx(sp, "<- N_INFO_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  N_BIND_ACK      17 - NS User bound to network address
 *  ---------------------------------------------------------------
 */
STATIC int
n_bind_ack(struct sctp *sp)
{
	mblk_t *mp;
	N_bind_ack_t *p;
	struct sctp_saddr *ss = sp->saddr;
	struct sockaddr_in *add;
	size_t add_len = sp->sanum ? sp->sanum * sizeof(*add) : sizeof(*add);

	if (sp->i_state != NS_WACK_BREQ)
		goto outstate;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + add_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (N_bind_ack_t *) mp->b_wptr;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = sp->conind;
	p->TOKEN_value = (t_uscalar_t) (long) sp->rq;
	p->PROTOID_length = 0;
	p->PROTOID_offset = 0;
	mp->b_wptr += sizeof(*p);
	if (sp->sanum) {
		for (; ss; ss = ss->next) {
			add = (typeof(add)) mp->b_wptr;
			add->sin_family = AF_INET;
			add->sin_port = sp->sport;
			add->sin_addr.s_addr = ss->saddr;
			mp->b_wptr += sizeof(*add);
		}
	} else {
		add = (typeof(add)) mp->b_wptr;
		add->sin_family = AF_INET;
		add->sin_port = sp->sport;
		add->sin_addr.s_addr = INADDR_ANY;
		mp->b_wptr += sizeof(*add);
	}
	sctp_n_setstate(sp, NS_IDLE);
	sctplogtx(sp, "<- N_BIND_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
}

/**
 * n_error_ack - generate an N_ERROR_ACK error acknowlegement
 * @sp: private structure
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: negative system error or positive NPI error
 *
 * Note that some system errors translate to NPI errors.  This function returns either -ENOBUFS or a
 * specific queue error that must result in requeuing of the message block, or QR_ABSORBED,
 * indicating that the message block has been released.
 */
noinline __unlikely int
n_error_ack(struct sctp *sp, mblk_t *msg, int prim, int err)
{
	mblk_t *mp;
	N_error_ack_t *p;

	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
		return (err);
	case QR_DONE:
		freemsg(msg);
		return (QR_ABSORBED);
	case -EADDRINUSE:
		err = NBOUND;
		break;
	case -EADDRNOTAVAIL:
		err = NNOADDR;
		break;
	case -EACCES:
		err = NACCESS;
		break;
	case -EDESTADDRREQ:
		err = NBADADDR;
		break;
	}
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (N_error_ack_t *) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = prim;
	p->NPI_error = (err < 0) ? NSYSERR : err;
	p->UNIX_error = (err < 0) ? -err : 0;
	mp->b_wptr += sizeof(*p);
	switch (sp->i_state) {
	case NS_WACK_OPTREQ:
	case NS_WACK_UREQ:
	case NS_WCON_CREQ:
		sctp_n_setstate(sp, NS_IDLE);
		break;
	case NS_WCON_RREQ:
		sctp_n_setstate(sp, NS_DATA_XFER);
		break;
	case NS_WACK_BREQ:
		sctp_n_setstate(sp, NS_UNBND);
		break;
	case NS_WACK_CRES:
		sctp_n_setstate(sp, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ6:
		sctp_n_setstate(sp, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		sctp_n_setstate(sp, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		sctp_n_setstate(sp, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		sctp_n_setstate(sp, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		sctp_n_setstate(sp, NS_WRES_RIND);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send NOUTSTATE or NNOTSUPPORT or are responding to an
		   N_OPTMGMT_REQ in other than the NS_IDLE state. */
	}
	freemsg(msg);
	sctplogtx(sp, "<- N_ERROR_ACK");
	putnext(sp->rq, mp);
	return (QR_ABSORBED);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  N_OK_ACK        19 - Success Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_ok_ack(struct sctp *sp, t_uscalar_t prim, mblk_t *cp, struct sctp *ap)
{
	mblk_t *mp;
	N_ok_ack_t *p;

	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (N_ok_ack_t *) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	/* switch by prim and not by state */
	switch (prim) {
	case N_OPTMGMT_REQ:
		if (sp->i_state == NS_WACK_OPTREQ)
			sctp_n_setstate(sp, NS_IDLE);
		break;
	case N_RESET_RES:
		sctp_n_setstate(sp, NS_DATA_XFER);
		break;
	case N_UNBIND_REQ:
		sctp_n_setstate(sp, NS_UNBND);
		break;
	case N_CONN_RES:
		if (ap != NULL)
			sctp_n_setstate(ap, NS_DATA_XFER);
		if (cp != NULL)
			freemsg(cp);
		if (ap == NULL || ap->rq != sp->rq) {
			if (bufq_length(&sp->conq))
				sctp_n_setstate(sp, NS_WRES_CIND);
			else
				sctp_n_setstate(sp, NS_IDLE);
		}
		break;
	case N_DISCON_REQ:
		switch (sp->i_state) {
		case NS_WACK_DREQ7:
			if (cp != NULL)
				freemsg(cp);
		case NS_WACK_DREQ6:
			if (bufq_length(&sp->conq))
				sctp_n_setstate(sp, NS_WRES_CIND);
			else
				sctp_n_setstate(sp, NS_IDLE);
			break;
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			/* TPI spec says that if the interface is in the TS_DATA_XFER,
			   TS_WIND_ORDREL or TS_WACK_ORDREL [sic] state, the stream must be flushed 
			   before responding with the T_OK_ACK primitive. */
			if (m_flush(sp, NULL, FLUSHRW, 0) == -ENOBUFS)
				goto enobufs;
			if (bufq_length(&sp->conq))
				sctp_n_setstate(sp, NS_WRES_CIND);
			else
				sctp_n_setstate(sp, NS_IDLE);
			break;
		}
		break;
	}
	/* Note: if we are not in a WACK state we simply do not change state.  This occurs normally 
	   when we are responding to an N_OPTMGMT_REQ in other than the NS_IDLE state. */
	sctplogtx(sp, "<- N_OK_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  N_DATACK_IND    24 - Data acknowledgement indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_datack_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn)
{
	mblk_t *mp;
	N_datack_ind_t *p;
	N_qos_sel_data_sctp_t *q;

	if ((1 << sp->i_state) & ~(NSF_DATA_XFER))
		goto outstate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + sizeof(*q), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (N_datack_ind_t *) mp->b_wptr;
	p->PRIM_type = N_DATACK_IND;
	mp->b_wptr += sizeof(*p);
	q = (N_qos_sel_data_sctp_t *) mp->b_wptr;
	q->n_qos_type = N_QOS_SEL_DATA_SCTP;
	q->ppi = ppi;
	q->sid = sid;
	q->ssn = ssn;
	q->tsn = tsn;
	q->more = 0;
	mp->b_wptr += sizeof(*q);
	sctplogda(sp, "<- N_DATACK_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
}

/*
 *  N_RESET_IND     26 - Inccoming NC reset request indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_reset_ind(struct sctp *sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *cp)
{
	mblk_t *mp, *bp;
	N_reset_ind_t *p;

	if ((1 << sp->i_state) & ~(NSF_DATA_XFER))
		goto outstate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	if (!(bp = sctp_dupmsg(sp, cp)))
		goto enodup;
	mp->b_datap->db_type = M_PROTO;
	p = (N_reset_ind_t *) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = orig;
	p->RESET_reason = reason;
	/* FIXME: We really need to map SCTP core protocol engine reasons to NPI reasons here.  All 
	   negative reasons are UNIX error codes, all positive reasons are SCTP cause values. */
	mp->b_wptr += sizeof(*p);
	bufq_queue(&sp->conq, bp);
	sctp_n_setstate(sp, NS_WRES_RIND);
	sctplogtx(sp, "<- N_RESET_IND");
	putnext(sp->rq, mp);
	return (0);
      enodup:
	freemsg(mp);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
}

/*
 *  N_RESET_CON     28 - Reset processing complete
 *  ---------------------------------------------------------------
 */
STATIC int
n_reset_con(struct sctp *sp)
{
	mblk_t *mp;
	N_reset_con_t *p;

	if ((1 << sp->i_state) & ~(NSF_WCON_RREQ))
		goto outstate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (N_reset_con_t *) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);
	sctp_n_setstate(sp, NS_DATA_XFER);
	sctplogtx(sp, "<- N_RESET_CON");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
}

#if 0
/*
 *  N_RECOVER_IND   29 - NC Recovery indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_recover_ind(void)
{
	mblk_t *mp;
	N_recover_ind_t *p;

	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (N_recover_ind_t *) mp->b_wptr;
	p->PRIM_type = N_RECOVER_IND;
	mp->b_wptr += sizeof(*p);
	sctp_n_setstate(sp, NS_DATA_XFER);
	sctplogtx(sp, "<- N_RECOVER_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/*
 *  N_RETRIEVE_IND  32 - NC Retrieval indication
 *  ---------------------------------------------------------------
 */
STATIC mblk_t *
n_retrieve_ind(mblk_t *dp)
{
	mblk_t *mp;
	N_retrieve_ind_t *p;

	if ((1 << sp->i_state) & ~(NSF_IDLE))
		goto oustate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (N_retrieve_ind_t *) mp->b_wptr;
	p->PRIM_type = N_RETRIEVE_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	sctp_n_setstate(sp, NS_IDLE);
	sctplogtx(sp, "<- N_RETRIEVE_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
}

/*
 *  N_RETRIEVE_CON  33 - NC Retrieval complete confirmation
 *  ---------------------------------------------------------------
 */
STATIC mblk_t *
n_retrieve_con(void)
{
	mblk_t *mp;
	N_retrieve_con_t *p;

	if ((1 << sp->i_state) & ~(NSF_IDLE))
		goto oustate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (N_retrieve_con_t *) mp->b_wptr;
	p->PRIM_type = N_RETREIVE_CON;
	mp->b_wptr += sizeof(*p);
	sctp_n_setstate(sp, NS_IDLE);
	sctplogtx(sp, "<- N_RETRIEVE_CON");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong NPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
}
#endif
STATIC int
sctp_n_conn_ind(struct sctp *sp, mblk_t *cp)
{
	return n_conn_ind(sp, cp);
}
STATIC int
sctp_n_conn_con(struct sctp *sp)
{
	return n_conn_con(sp);
}
STATIC int
sctp_n_data_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
		uint more, mblk_t *dp)
{
	if (ord)
		return n_data_ind(sp, ppi, sid, ssn, tsn, more, dp);
	else
		return n_exdata_ind(sp, ppi, sid, ssn, tsn, more, dp);
}
STATIC int
sctp_n_datack_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn)
{
	return n_datack_ind(sp, ppi, sid, ssn, tsn);
}
STATIC int
sctp_n_reset_ind(struct sctp *sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *cp)
{
	return n_reset_ind(sp, orig, reason, cp);
}
STATIC int
sctp_n_reset_con(struct sctp *sp)
{
	return n_reset_con(sp);
}
STATIC int
sctp_n_discon_ind(struct sctp *sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *cp)
{
	return n_discon_ind(sp, orig, reason, cp);
}
STATIC int
sctp_n_ordrel_ind(struct sctp *sp)
{
	sctp_ordrel_req(sp);	/* shut it down right now */
	return sctp_n_discon_ind(sp, 0, 0, NULL);
}

STATIC struct sctp_ifops n_ops = {
	.conn_ind = &sctp_n_conn_ind,
	.conn_con = &sctp_n_conn_con,
	.data_ind = &sctp_n_data_ind,
	.datack_ind = &sctp_n_datack_ind,
	.reset_ind = &sctp_n_reset_ind,
	.reset_con = &sctp_n_reset_con,
	.discon_ind = &sctp_n_discon_ind,
	.ordrel_ind = &sctp_n_ordrel_ind,
	.retr_ind = NULL,
	.retr_con = NULL,
};

/*
 *  =========================================================================
 *
 *  NPI User --> NPI Provider (SCTP) Primitives
 *
 *  =========================================================================
 */
/*
 *  N_CONN_REQ           0 - NC request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_conn_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const N_conn_req_t *p = (N_conn_req_t *) mp->b_rptr;
	const N_qos_sel_conn_sctp_t *q = (N_qos_sel_conn_sctp_t *) (mp->b_rptr + p->QOS_offset);
	struct sockaddr_storage sa;
	uint16_t dport = 0;
	struct sockaddr_in *dsin = (struct sockaddr_in *) &sa;
	size_t dnum = 0, dlen, qlen;

	if (sp->i_state != NS_IDLE) {
		if (sp->conind > 0)
			goto access;
		goto outstate;
	}
	sctp_n_setstate(sp, NS_WCON_CREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->CONN_flags & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto badflag;
	if ((dlen = p->DEST_length)) {
		unsigned char *dptr = mp->b_rptr + p->DEST_offset;

		if (mp->b_wptr < dptr + dlen)
			goto badaddr;
		if (dlen < sizeof(struct sockaddr_in))
			goto badaddr;
		if (dlen > sizeof(struct sockaddr_storage))
			goto badaddr;
		if (dlen % sizeof(struct sockaddr_in))
			goto badaddr;
		dnum = dlen / sizeof(struct sockaddr_in);
		/* avoid alignment problems */
		bcopy(dptr, dsin, dlen);
		if (dsin->sin_family != AF_INET)
			goto badaddr;
		dport = dsin->sin_port;
	} else
		goto noaddr;
	if (!dport)
		goto badaddr;
	if (mp->b_cont && (!msgdsize(mp->b_cont) || msgdsize(mp->b_cont) > sp->pmtu - 16))
		goto baddata;
	if (sp->conind > 0)
		goto access;
	if ((qlen = p->QOS_length)) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + qlen)
			goto badopt1;
		if (q->n_qos_type != N_QOS_SEL_CONN_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
		if (q->i_streams != -1)
			sp->max_istr = q->i_streams ? q->i_streams : 1;
		if (q->o_streams != -1)
			sp->req_ostr = q->o_streams ? q->o_streams : 1;
	}
	if (!sp->max_istr)
		sp->max_istr = 1;
	if (!sp->req_ostr)
		sp->req_ostr = 1;
	sp->flags &= ~(SCTP_FLAG_REC_CONF_OPT | SCTP_FLAG_EX_DATA_OPT);
	if (p->CONN_flags & REC_CONF_OPT)
		sp->flags |= SCTP_FLAG_REC_CONF_OPT;
	if (p->CONN_flags & EX_DATA_OPT)
		sp->flags |= SCTP_FLAG_EX_DATA_OPT;
	if ((err = sctp_conn_req(sp, dport, dsin, dnum, mp->b_cont)))
		goto error;
	freeb(mp);
	return (QR_ABSORBED);
#if 0
      access:
	err = NACCESS;
	sctplogrx(sp, "no permission for requested address");
	goto error;
#endif
      baddata:
	err = NBADDATA;
	sctplogrx(sp, "amount of data is invalid");
	goto error;
      noaddr:
	err = NNOADDR;
	sctplogrx(sp, "destination address required");
	goto error;
      badaddr:
	err = NBADADDR;
	sctplogrx(sp, "address is unusable");
	goto error;
      badopt2:
	sctplogrx(sp, "options are unusable");
	err = NBADQOSPARAM;
	goto error;
      badqostype:
	err = NBADQOSTYPE;
	sctplogrx(sp, "QOS structure type not supported");
	goto error;
      badopt1:
	err = NBADOPT;
	sctplogrx(sp, "options are unusable");
	goto error;
      badflag:
	err = NBADFLAG;
	sctplogrx(sp, "bad connection flags");
	goto error;
      einval:
	err = -EINVAL;
	sctplogrx(sp, "invalid message format");
	goto error;
      access:
	err = NACCESS;
	sctplogrx(sp, "cannot connect on listening stream");
	goto error;
      outstate:
	err = NOUTSTATE;
	sctplogrx(sp, "would place interface out of state %d", (int) sp->i_state);
	goto error;
      error:
	sctplogrx(sp, "error %d", err);
	return n_error_ack(sp, mp, N_CONN_REQ, err);
}

/*
 *  N_CONN_RES           1 - Accept previous connection indication
 *  -------------------------------------------------------------------------
 *  IMPLEMENTATION NOTES:- Sequence numbers are actually the address of the mblk which contains the
 *  COOKIE-ECHO chunk and contains the cookie as a connection indication.  To find if a particular
 *  sequence number is valid, we walk the connection indication queue looking for a mblk with the
 *  same address as the sequence number.  Sequence numbers are only valid on the stream for which
 *  the connection indication is queued.
 */
STATIC mblk_t *
n_seq_check(struct sctp *sp, t_uscalar_t seq)
{
	mblk_t *mp;
	pl_t pl;

	pl = bufq_lock(&sp->conq);
	for (mp = bufq_head(&sp->conq); mp && (t_uscalar_t) (long) mp != seq; mp = mp->b_next) ;
	if (mp)
		__bufq_unlink(&sp->conq, mp);
	bufq_unlock(&sp->conq, pl);
	usual(mp);
	return (mp);
}
STATIC struct sctp *
n_tok_check(struct sctp *sp, t_uscalar_t tok)
{
	struct sctp *ap;

	/* FIXME: must take list lock here! */
	for (ap = sctp_protolist; ap && (t_uscalar_t) (long) ap->rq != tok; ap = ap->next) ;
	/* FIXME: must really hold a reference on the ap structure */
	usual(ap);
	return (ap);
}
STATIC int
n_conn_res(struct sctp *sp, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	struct sctp *ap = NULL;
	N_conn_res_t *p = (N_conn_res_t *) mp->b_rptr;
	N_qos_sel_conn_sctp_t *q = (N_qos_sel_conn_sctp_t *) (mp->b_rptr + p->QOS_offset);

	if (sp->i_state != NS_WRES_CIND)
		goto outstate;
	sctp_n_setstate(sp, NS_WACK_CRES);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->RES_length) {
	}
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt1;
		if (q->n_qos_type != N_QOS_SEL_CONN_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
	}
	if (!(cp = n_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if (!(ap = n_tok_check(sp, p->TOKEN_value)))
		goto badtoken1;
	if (ap != sp && !(ap = sctp_trylockq(ap->rq)))
		goto badtoken1;
	if (ap != sp && !((1 << ap->i_state) & (NSF_UNBND | NSF_IDLE)))
		goto badtoken1;
	if (ap->i_state == NS_IDLE && ap->conind)
		goto badtoken2;
	/* protect at least r00t streams from users */
	if (sp->cred.cr_uid != 0 && (ap->cred.cr_uid != sp->cred.cr_uid))
		goto access;
	{
		uint ap_oldstate = ap->i_state;
		uint ap_oldflags = ap->flags;

		sctp_n_setstate(sp, NS_DATA_XFER);
		ap->flags &= ~(SCTP_FLAG_REC_CONF_OPT | SCTP_FLAG_EX_DATA_OPT);
		if (p->CONN_flags & REC_CONF_OPT)
			ap->flags |= SCTP_FLAG_REC_CONF_OPT;
		if (p->CONN_flags & EX_DATA_OPT)
			ap->flags |= SCTP_FLAG_EX_DATA_OPT;
		if ((err = sctp_conn_res(sp, cp, ap, mp->b_cont))) {
			sctp_n_setstate(ap, ap_oldstate);
			ap->flags = ap_oldflags;
			goto error;
		}
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		if ((err = n_ok_ack(sp, N_CONN_RES, cp, ap))) {
			sctp_n_setstate(ap, ap_oldstate);
			ap->flags = ap_oldflags;
			goto error;
		}
		if (ap != sp)
			sctp_unlockq(ap);
		return (QR_DONE);
	}
      access:
	sctplogrx(sp, "%s() no access to accepting queue", __FUNCTION__);
	err = NACCESS;
	goto error;		/* no access to accepting queue */
      badtoken2:
	sctplogrx(sp, "%s() accepting queue is listening", __FUNCTION__);
	err = NBADTOKEN;
	goto error;		/* accepting queue is listening */
      badtoken1:
	sctplogrx(sp, "%s() accepting queue id is invalid", __FUNCTION__);
	err = NBADTOKEN;
	goto error;		/* accepting queue id is invalid */
      badseq:
	sctplogrx(sp, "%s() conn ind reference is invalid", __FUNCTION__);
	err = NBADSEQ;
	goto error;		/* connection ind reference is invalid */
      badopt2:
	sctplogrx(sp, "%s() QOS options are bad", __FUNCTION__);
	err = NBADOPT;
	goto error;		/* quality of service options are bad */
      badqostype:
	sctplogrx(sp, "%s() QOS options are bad", __FUNCTION__);
	err = NBADQOSTYPE;
	goto error;		/* quality of service options are bad */
      badopt1:
	sctplogrx(sp, "%s() QOS options are bad", __FUNCTION__);
	err = NBADOPT;
	goto error;		/* quality of service options are bad */
      einval:
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	sctplogrx(sp, "%s() would place interface out of state %d", __FUNCTION__,
		  (int) sp->i_state);
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	if (ap && ap != sp)
		sctp_unlockq(ap);
	if (cp)
		bufq_queue(&sp->conq, cp);
	seldom();
	return n_error_ack(sp, mp, N_CONN_RES, err);
}

/*
 *  N_DISCON_REQ         2 - NC disconnection request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_discon_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	N_discon_req_t *p = (N_discon_req_t *) mp->b_rptr;

	if (!((1 << sp->i_state) & (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER
				    | NSF_WCON_RREQ | NSF_WRES_RIND)))
		goto outstate;
	switch (sp->i_state) {
	case NS_WCON_CREQ:
		sctp_n_setstate(sp, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		sctp_n_setstate(sp, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		sctp_n_setstate(sp, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		sctp_n_setstate(sp, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		sctp_n_setstate(sp, NS_WACK_DREQ11);
		break;
	default:
		goto outstate;
	}
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->RES_length)
		goto badaddr;
	if (sp->i_state == NS_WACK_DREQ7 && !(cp = n_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if (sp->i_state != NS_WACK_DREQ7 && (p->SEQ_number != 0))
		goto badseq;
	/* XXX: What do we do with the disconnect reason? Nothing? */
	if ((err = sctp_discon_req(sp, cp)))
		goto error;

	if ((err = n_ok_ack(sp, N_DISCON_REQ, cp, NULL)))
		goto error;
	return (QR_DONE);
      badseq:
	sctplogrx(sp, "%s() conn ind reference is invalid", __FUNCTION__);
	err = NBADSEQ;
	goto error;		/* connection ind reference is invalid */
      badaddr:
	sctplogrx(sp, "%s() responding address is invalid", __FUNCTION__);
	err = NBADADDR;
	goto error;		/* responding address is inavlid */
      einval:
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	sctplogrx(sp, "%s() would place interface out of state %d", __FUNCTION__,
		  (int) sp->i_state);
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	if (cp)
		bufq_queue(&sp->conq, cp);
	seldom();
	return n_error_ack(sp, mp, N_DISCON_REQ, err);
}

/**
 * n_write: - process M_DATA
 * @sp: private structure
 * @mp: the M_DATA primitive
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number indicating that the message is to be (re)queued.
 */
STATIC inline fastcall __hot_write int
n_write(struct sctp *sp, mblk_t *mp)
{
	int err;

	if (sp->i_state == NS_IDLE)
		goto discard;
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	if (msgdsize(mp) == 0)
		goto baddata;
	{
		t_uscalar_t ppi = sp->ppi;
		t_uscalar_t sid = sp->sid;
		t_uscalar_t ord = 1;
		t_uscalar_t more = (mp->b_flag & MSGDELIM) ? 0 : N_MORE_DATA_FLAG;
		t_uscalar_t rcpt = (sp->flags & SCTP_FLAG_DEFAULT_RC_SEL) ? N_RC_FLAG : 0;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)))
			goto error;
		return (QR_ABSORBED);	/* absorbed mp */
	}
      baddata:
	sctplogrx(sp, "%s() invalid amount of user data", __FUNCTION__);
	err = -EPROTO;
	goto error;
      eproto:
	sctplogrx(sp, "%s() would place interface out of state %d", __FUNCTION__,
		  (int) sp->i_state);
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      discard:
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	err = 0;
	goto error;		/* ignore in idle state */
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

/**
 * n_data_req: - process N_DATA_REQ (3 - Connection-Mode data transfer request)
 * @sp: private structure
 * @mp: N_DATA_REQ message
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number indicating that the message is to be (re)queued.
 */
STATIC inline fastcall __hot_write int
n_data_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_qos_sel_data_sctp_t *q = NULL;
	N_data_req_t *p = (N_data_req_t *) mp->b_rptr;

	if (sp->i_state == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr >= mp->b_rptr + sizeof(*p) + sizeof(*q)) {
		q = (N_qos_sel_data_sctp_t *) (p + 1);
		if (q->n_qos_type != N_QOS_SEL_DATA_SCTP)
			q = NULL;
	}
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	if (mp->b_cont == NULL || msgdsize(mp->b_cont) == 0)
		goto baddata;
	{
		t_uscalar_t ppi = q ? q->ppi : sp->ppi;
		t_uscalar_t sid = q ? q->sid : sp->sid;
		t_uscalar_t ord = 1;
		t_uscalar_t more = p->DATA_xfer_flags & N_MORE_DATA_FLAG;
		t_uscalar_t rcpt = p->DATA_xfer_flags & N_RC_FLAG;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		freeb(mp);
		return (QR_ABSORBED);
	}
      baddata:
	sctplogrx(sp, "%s() invalid amount of user data", __FUNCTION__);
	err = -EPROTO;
	goto error;
      eproto:
	sctplogrx(sp, "%s() would place interface out of state %d", __FUNCTION__,
		  (int) sp->i_state);
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	sctplogrx(sp, "%s() ignore in idle state", __FUNCTION__);
	err = 0;
	goto error;		/* ignore in idle state */
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

/**
 * n_exdata_req: - process N_EXDATA_REQ (4 - Expedited data request)
 * @sp: private structure
 * @mp: the N_EXDATA_REQ message
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number indicating that the message is to be (re)queued.
 */
STATIC fastcall __hot_write int
n_exdata_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_qos_sel_data_sctp_t *q = NULL;
	N_exdata_req_t *p = (N_exdata_req_t *) mp->b_rptr;

	if (sp->i_state == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr >= mp->b_rptr + sizeof(*p) + sizeof(*q)) {
		q = (N_qos_sel_data_sctp_t *) (p + 1);
		if (q->n_qos_type != N_QOS_SEL_DATA_SCTP)
			q = NULL;
	}
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	if (mp->b_cont == NULL || msgdsize(mp->b_cont) == 0)
		goto baddata;
	{
		t_uscalar_t ppi = q ? q->ppi : sp->ppi;
		t_uscalar_t sid = q ? q->sid : sp->sid;
		t_uscalar_t ord = 0;
		t_uscalar_t more = 0;
		t_uscalar_t rcpt = 0;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		freeb(mp);
		return (QR_ABSORBED);
	}
      baddata:
	sctplogrx(sp, "%s() invalid amount of user data", __FUNCTION__);
	err = -EPROTO;
	goto error;
      eproto:
	sctplogrx(sp, "%s() would place interface out of state %d", __FUNCTION__,
		  (int) sp->i_state);
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	err = 0;
	goto error;		/* ignore in idle state */
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

/*
 *  N_INFO_REQ           5 - Information request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_info_req(struct sctp *sp, mblk_t *mp)
{
	(void) mp;
	return n_info_ack(sp);
}

/*
 *  N_BIND_REQ           6 - Bind a NS user to network address
 *  -------------------------------------------------------------------------
 */
STATIC int
n_bind_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_bind_req_t *p = (N_bind_req_t *) mp->b_rptr;

	if (sp->i_state != NS_UNBND)
		goto outstate;
	sctp_n_setstate(sp, NS_WACK_BREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((p->BIND_flags & ~(TOKEN_REQUEST | DEFAULT_DEST | DEFAULT_LISTENER)))
		goto badflag;
	if ((p->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
		goto notsupport;
	if (p->ADDR_length && (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto badaddr;
	{
		struct sockaddr_storage sa;
		struct sockaddr_in *add_in = (struct sockaddr_in *) &sa;
		size_t add_len = p->ADDR_length;
		size_t anum;
		uint16_t sport;

		if (add_len) {
			if (add_len < sizeof(struct sockaddr_in))
				goto badaddr;
			if (add_len > sizeof(struct sockaddr_storage))
				goto badaddr;
			anum = add_len / sizeof(struct sockaddr_in);
			if (add_len != anum * sizeof(struct sockaddr_in))
				goto badaddr;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + p->ADDR_offset, add_in, add_len);
			if (add_in->sin_family != AF_INET)
				goto badaddr;
			sport = add_in->sin_port;
		} else {
			add_in->sin_family = AF_INET;
			add_in->sin_port = 0;
			add_in->sin_addr.s_addr = INADDR_ANY;
			anum = 1;
			sport = 0;
		}
		if ((err = sctp_bind_req(sp, sport, add_in, anum, p->CONIND_number)))
			goto error;
		if ((err = n_bind_ack(sp)) == 0)
			return (QR_DONE);
		sctp_unbind_req(sp);
		return (err);
	}
#if 0
      access:
	sctplogrx(sp, "%s() no permission for requested address", __FUNCTION__);
	err = NACCESS;
	goto error;		/* no permission for requested address */
      noaddr:
	sctplogrx(sp, "%s() could not allocate address", __FUNCTION__);
	err = NNOADDR;
	goto error;		/* could not allocate address */
#endif
      badaddr:
	seldom();
	sctplogrx(sp, "%s() invalid address", __FUNCTION__);
	err = NBADADDR;
	goto error;		/* address is invalid */
      notsupport:
	sctplogrx(sp, "%s() not supported", __FUNCTION__);
	err = NNOTSUPPORT;
	goto error;
      badflag:
	sctplogrx(sp, "%s() invalid flags", __FUNCTION__);
	err = NBADFLAG;
	goto error;
      einval:
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	sctplogrx(sp, "%s() would place interface out of state %d", __FUNCTION__,
		  (int) sp->i_state);
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, mp, N_BIND_REQ, err);
}

/*
 *  N_UNBIND_REQ         7 - Unbind NS user from network address
 *  -------------------------------------------------------------------------
 */
STATIC int
n_unbind_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_unbind_req_t *p = (N_unbind_req_t *) mp->b_rptr;

	(void) p;
	if (sp->i_state != NS_IDLE)
		goto outstate;
	sctp_n_setstate(sp, NS_WACK_UREQ);
	if ((err = sctp_unbind_req(sp)))
		goto error;
	return n_ok_ack(sp, N_UNBIND_REQ, NULL, NULL);
      outstate:
	sctplogrx(sp, "%s() would place interface out of state %d", __FUNCTION__,
		  (int) sp->i_state);
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, mp, N_UNBIND_REQ, err);
}

/*
 *  N_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_optmgmt_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_optmgmt_req_t *p = (N_optmgmt_req_t *) mp->b_rptr;

#ifdef ETSI
	N_qos_sel_info_sctp2_t *q = (N_qos_sel_info_sctp2_t *) (mp->b_rptr + p->QOS_offset);
#else
	N_qos_sel_info_sctp_t *q = (N_qos_sel_info_sctp_t *) (mp->b_rptr + p->QOS_offset);
#endif

	if (sp->i_state == NS_IDLE)
		sctp_n_setstate(sp, NS_WACK_OPTREQ);
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt;
#ifdef ETSI
		switch (q->n_qos_type) {
		case N_QOS_SEL_INFO_SCTP:
			if (p->QOS_length != sizeof(N_qos_sel_info_sctp_t))
				goto badopt2;
			break;
		case N_QOS_SEL_INFO_SCTP2:
			if (p->QOS_length != sizeof(N_qos_sel_info_sctp2_t))
				goto badopt2;
			break;
		default:
			goto badqostype;
		}
#else
		if (q->n_qos_type != N_QOS_SEL_INFO_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
#endif
	}
	if (p->QOS_length) {
		if (q->i_streams != -1) {
			if (q->i_streams < 1)
				goto badqosparam;
			if (q->i_streams > 0x00010000)
				goto badqosparam;
			sp->max_istr = q->i_streams ? q->i_streams : 1;
		}
		if (q->o_streams != -1) {
			if (q->o_streams < 1)
				goto badqosparam;
			if (q->o_streams > 0x00010000)
				goto badqosparam;
			sp->req_ostr = q->o_streams ? q->o_streams : 1;
		}
		if (q->ppi != -1) {
			sp->ppi = q->ppi;
		}
		if (q->sid != -1) {
			if (q->sid > 0x0000ffff)
				goto badqosparam;
			sp->sid = q->sid;
		}
		if (q->max_inits != -1) {
			sp->max_inits = q->max_inits;
		}
		if (q->max_retrans != -1) {
			sp->max_retrans = q->max_retrans;
		}
		if (q->ck_life != -1) {
			if (q->ck_life < 1)
				goto badqosparam;
#ifndef __LP64__
			if (q->ck_life * HZ > MAX_SCHEDULE_TIMEOUT)
				goto badqosparam;
#endif
			sp->ck_life = q->ck_life * HZ;
		}
		if (q->ck_inc != -1) {
#ifndef __LP64__
			if (q->ck_inc * HZ > MAX_SCHEDULE_TIMEOUT)
				goto badqosparam;
#endif
			sp->ck_inc = q->ck_inc * HZ;
		}
		if (q->hmac != -1) {
			if (q->hmac != T_SCTP_HMAC_NONE
#if defined SCTP_CONFIG_HMAC_SHA1
			    && q->hmac != T_SCTP_HMAC_SHA1
#endif
#if defined SCTP_CONFIG_HMAC_MD5
			    && q->hmac != T_SCTP_HMAC_MD5
#endif
			    )
				goto badqosparam;
			sp->hmac = q->hmac;
		}
		if (q->throttle != -1) {
			if (q->throttle < 1000 / HZ)
				goto badqosparam;
			if (q->throttle < 1)
				goto badqosparam;
#ifndef __LP64__
			if (q->throttle / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
				goto badqosparam;
#endif
			sp->throttle = q->throttle * HZ / 1000;
		}
		if (q->max_sack != -1) {
			if (q->max_sack) {
				if (q->max_sack < 1000 / HZ)
					goto badqosparam;
				if (q->max_sack < 1)
					goto badqosparam;
				if (q->max_sack > 500)
					goto badqosparam;
			}
			sp->max_sack = q->max_sack * HZ / 1000;
		}
#ifdef ETSI
		if (q->n_qos_type == N_QOS_SEL_INFO_SCTP2) {
			/* have extra sack frequency field */
			if (q->sack_freq != -1) {
				if (q->sack_freq) {
					if (sp->sack_freq < 1)
						goto badqosparam;
					if (sp->sack_freq > 5)
						goto badqosparam;
				}
				sp->sack_freq = q->sack_freq;
			}
		}
#endif
		if (q->rto_ini != -1) {
			if (q->rto_ini) {
				if (q->rto_ini < 1000 / HZ)
					goto badqosparam;
				if (q->rto_ini < 1)
					goto badqosparam;
#ifndef __LP64__
				if (q->rto_ini / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					goto badqosparam;
#endif
			}
			sp->rto_ini = q->rto_ini * HZ / 1000;
			sctplogno(sp, "sp->rto_ini = %d", (int) sp->rto_ini);
		}
		if (q->rto_min != -1) {
			if (q->rto_min) {
				if (q->rto_min < 1000 / HZ)
					goto badqosparam;
				if (q->rto_min < 1)
					goto badqosparam;
#ifndef __LP64__
				if (q->rto_min / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					goto badqosparam;
#endif
			}
			sp->rto_min = q->rto_min * HZ / 1000;
			sctplogno(sp, "sp->rto_min = %d", (int) sp->rto_min);
		}
		if (q->rto_max != -1) {
			if (q->rto_max) {
				if (q->rto_max < 1000 / HZ)
					goto badqosparam;
				if (q->rto_max < 1)
					goto badqosparam;
#ifndef __LP64__
				if (q->rto_max / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					goto badqosparam;
#endif
			}
			sp->rto_max = q->rto_max * HZ / 1000;
			sctplogno(sp, "sp->rto_max = %d", (int) sp->rto_max);
		}
		if (q->rtx_path != -1) {
			if (q->rtx_path < 0)
				goto badqosparam;
			sp->rtx_path = q->rtx_path;
			sctplogno(sp, "sp->rtx_path = %d", (int) sp->rtx_path);
		}
		if (q->hb_itvl != -1) {
			if (q->hb_itvl < 1)
				goto badqosparam;
#ifndef __LP64__
			if (q->hb_itvl * HZ > MAX_SCHEDULE_TIMEOUT)
				goto badqosparam;
#endif
			sp->hb_itvl = q->hb_itvl * HZ;
			sctplogno(sp, "sp->hb_itvl = %d", (int) sp->hb_itvl);
		}
		if (q->options != -1) {
			sp->debug = q->options;
		}
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		sp->flags |= SCTP_FLAG_DEFAULT_RC_SEL;
	else
		sp->flags &= ~SCTP_FLAG_DEFAULT_RC_SEL;
	return n_ok_ack(sp, N_OPTMGMT_REQ, NULL, NULL);
      badqosparam:
	err = NBADQOSPARAM;
	goto error;		/* QOS parameter value was incorrect */
      badopt2:
	seldom();
	err = NBADOPT;
	goto error;		/* QOS options were invalid */
      badqostype:
	seldom();
	err = NBADQOSTYPE;
	goto error;		/* QOS structure type not supported */
      badopt:
	seldom();
	err = NBADOPT;
	goto error;		/* QOS options were invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, mp, N_OPTMGMT_REQ, err);
}

/*
 *  N_RESET_REQ         25 - NC reset request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_reset_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_reset_req_t *p = (N_reset_req_t *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (sp->i_state != NS_DATA_XFER)
		goto outstate;
	sctp_n_setstate(sp, NS_WCON_RREQ);
	if ((err = sctp_reset_req(sp)))
		goto error;
	return (0);
      outstate:
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      discard:
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	err = 0;
	goto error;		/* ignore in idle state */
      einval:
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, mp, N_RESET_REQ, err);
}

/*
 *  N_RESET_RES         27 - Reset processing accepted
 *  -------------------------------------------------------------------------
 */
STATIC int
n_reset_res(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_reset_res_t *p = (N_reset_res_t *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (sp->i_state != NS_WRES_RIND)
		goto outstate;
	sctp_n_setstate(sp, NS_WACK_RRES);
	if ((err = sctp_reset_res(sp)))
		goto error;
	return n_ok_ack(sp, N_RESET_RES, NULL, NULL);
      outstate:
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      discard:
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	err = 0;
	goto error;		/* ignore in idle state */
      einval:
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, mp, N_RESET_RES, err);
}

STATIC int
n_datack_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	N_datack_req_t *p;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_DATACK_REQ);
#if 0
	err = NNOTSUPPORT;
	if (unlikely(sp->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(sp->info.SERV_type != N_CONS))
		goto error;
#endif
	/* Note: If the interface is in the NS_IDLE state when the provider receives the
	   N_DATACK_REQ primitive, then the NS provider should discard the request without
	   generating a fatal error. */
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (unlikely(!((1 << sp->i_state) & (NSM_CONNECTED))))
		goto error;
	/* Note: If the NS provider had no knowledge of a previous N_DATA_IND with the receipt
	   confirmation flag set, then the NS provider should just ignore the request without
	   generating a fatal error. */
	if (unlikely(bufq_length(&sp->ackq) <= 0))
		goto error;
#if 0
	if (unlikely((err = np_datack(q)) < 0))
		goto error;
#else
	err = -EOPNOTSUPP;
	goto error;
#endif
      discard:
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	err = 0;
	goto error;
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err ? -EPROTO : 0);
}

/**
 * n_other_req: - process unsupported primitives
 * @sp: private structure
 * @mp: the unsupporting primitive
 */
noinline fastcall __unlikely int
n_other_req(struct sctp *sp, mblk_t *mp)
{
	np_long prim = *((np_long *) mp->b_rptr);

	return n_error_ack(sp, mp, prim, NNOTSUPPORT);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

/**
 * sctp_n_w_proto_return: - handle M_PROTO and M_PCPROTO processing return value
 * @mp: the M_PROTO or M_PCPROTO message
 * @rtn: the return value
 *
 * Some M_PROTO and M_PCPROTO processing functions return an error just to cause the primary state
 * of the endpoint to be restored.  Also, some functions may still return QR_DONE.  These return
 * values are processed here.
 */
noinline fastcall int
sctp_n_w_proto_return(mblk_t *mp, int err)
{
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
		return (err);
	default:
	case QR_DONE:
		freemsg(mp);
	case QR_ABSORBED:
		return (QR_ABSORBED);
	}
}

/**
 * sctp_n_w_proto_slow: process M_PROTO or M_PCPROTO primitive
 * @sp: private structure (locked)
 * @mp: the M_PROTO or M_PCPROTO primitive
 * @prim: the primitive
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number when the message is to be (re)queued.  This function must be called with the private
 * structure locked.
 */
noinline fastcall int
sctp_n_w_proto_slow(struct sctp *sp, mblk_t *mp, np_long prim)
{
	np_long oldstate = sp->i_state;
	int rtn;

	switch (prim) {
	case N_CONN_REQ:
		sctplogrx(sp, "-> N_CONN_REQ");
		rtn = n_conn_req(sp, mp);
		break;
	case N_CONN_RES:
		sctplogrx(sp, "-> N_CONN_RES");
		rtn = n_conn_res(sp, mp);
		break;
	case N_DISCON_REQ:
		sctplogrx(sp, "-> N_DISCON_REQ");
		rtn = n_discon_req(sp, mp);
		break;
	case N_DATA_REQ:
		sctplogda(sp, "-> N_DATA_REQ");
		rtn = n_data_req(sp, mp);
		break;
	case N_EXDATA_REQ:
		sctplogda(sp, "-> N_EXDATA_REQ");
		rtn = n_exdata_req(sp, mp);
		break;
	case N_INFO_REQ:
		sctplogrx(sp, "-> N_INFO_REQ");
		rtn = n_info_req(sp, mp);
		break;
	case N_BIND_REQ:
		sctplogrx(sp, "-> N_BIND_REQ");
		rtn = n_bind_req(sp, mp);
		break;
	case N_UNBIND_REQ:
		sctplogrx(sp, "-> N_UNBIND_REQ");
		rtn = n_unbind_req(sp, mp);
		break;
	case N_OPTMGMT_REQ:
		sctplogrx(sp, "-> N_OPTMGMT_REQ");
		rtn = n_optmgmt_req(sp, mp);
		break;
	case N_RESET_REQ:
		sctplogrx(sp, "-> N_RESET_REQ");
		rtn = n_reset_req(sp, mp);
		break;
	case N_RESET_RES:
		sctplogrx(sp, "-> N_RESET_RES");
		rtn = n_reset_res(sp, mp);
		break;
	case N_DATACK_REQ:
		sctplogrx(sp, "-> N_DATACK_REQ");
		rtn = n_datack_req(sp, mp);
		break;
	case N_UNITDATA_REQ:
		sctplogda(sp, "-> N_UNITDATA_REQ");
		rtn = m_error_reply(sp, mp, -EPROTO);
		break;
	default:
		sctplogrx(sp, "-> N_????_???");
		rtn = n_other_req(sp, mp);
		break;
	}
	if (rtn < 0)
		sctp_n_setstate(sp, oldstate);
	/* The put and srv procedures do not recognize all errors.  Sometimes we return an error to 
	   here just to restore the previous state.  */
	return sctp_n_w_proto_return(mp, rtn);
}

/**
 * __sctp_n_w_proto: - process M_PROTO or M_PCPROTO message locked
 * @sp: private structure (locked)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locked version is for use by the service procedure which takes private structure locks once
 * for the entire service procedure run.
 */
STATIC inline fastcall __hot_out int
__sctp_n_w_proto(struct sctp *sp, mblk_t *mp)
{
	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(np_ulong))) {
		np_ulong prim = *(np_ulong *) mp->b_rptr;
		np_ulong oldstate = sp->i_state;
		int rtn;

		if (likely(prim == N_DATA_REQ)) {
			if (likely((rtn = n_data_req(sp, mp)) == QR_ABSORBED))
				return (QR_ABSORBED);
		} else if (likely(prim == N_EXDATA_REQ)) {
			if (likely((rtn = n_exdata_req(sp, mp)) == QR_ABSORBED))
				return (QR_ABSORBED);
		} else
			return sctp_n_w_proto_slow(sp, mp, prim);

		if (rtn < 0)
			sctp_n_setstate(sp, oldstate);
		return sctp_n_w_proto_return(mp, rtn);
	}
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, -EPROTO);
	return m_error_reply(sp, mp, -EPROTO);
}

/**
 * sctp_n_w_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locking version is for use by the put procedure which does not take locks.
 */
STATIC inline fastcall __hot_write int
sctp_n_w_proto(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(np_ulong))) {
		np_ulong prim = *(np_ulong *) mp->b_rptr;
		struct sctp *sp;

		if (likely(prim == N_DATA_REQ))
			return (-EAGAIN);
		else if (likely(prim == N_EXDATA_REQ))
			return (-EAGAIN);

		if (likely((sp = sctp_trylockq(q)) != NULL)) {
			int rtn;

			if ((rtn = sctp_n_w_proto_slow(sp, mp, prim)) == QR_ABSORBED)
				___sctp_transmit_wakeup(sp);
			sctp_unlockq(sp);
			return (rtn);
		}
		return (-EDEADLK);
	}
	sctplogrx(SCTP_PRIV(q), "%s() replying with error %d", __FUNCTION__, -EPROTO);
	return m_error_reply(SCTP_PRIV(q), mp, -EPROTO);
}

/**
 * __sctp_n_w_data: - process M_DATA message
 * @sp: private structure (locked)
 * @mp: the M_DATA message
 *
 * This function either returns QR_ABSORBED when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This non-locking version is used by the service procedure.
 */
STATIC inline fastcall __hot_write int
__sctp_n_w_data(struct sctp *sp, mblk_t *mp)
{
	return n_write(sp, mp);
}

STATIC inline fastcall __hot_write int
__sctp_n_w_data_put(struct sctp *sp, mblk_t *mp)
{
	return (-EAGAIN);
}

/**
 * sctp_n_w_data: - M_DATA processing
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 *
 * This function either returns QR_ABSORBED when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This locking version is used by the put procedure.
 */
STATIC inline fastcall __hot_write int
sctp_n_w_data(queue_t *q, mblk_t *mp)
{
#if 0
	struct sctp *sp;

	if (likely((sp = sctp_trylockq(q)) != NULL)) {
		int rtn;

		if ((rtn = __sctp_n_w_data_put(sp, mp)) == QR_ABSORBED)
			___sctp_transmit_wakeup(sp);
		sctp_unlockq(sp);
		return (rtn);
	}
	return (-EDEADLK);
#else
	return (-EAGAIN);
#endif
}

/**
 * sctp_n_w_other: - other messages on write queue
 * @q: active queue (write queue)
 * @mp: the unexpected message
 *
 * If SCTP is pushed as a module, pass unexpected messages long with flow control.  If a driver,
 * respond to some and discard the rest.  This function is called with the private structure
 * unlocked.  It returns either -%EBUSY if SCTP is a modules and flow control prevents passing the
 * message, or QR_ABSORBED in all other cases.
 */
noinline fastcall __unlikely int
sctp_n_w_other(queue_t *q, mblk_t *mp)
{
	struct sctp *sp = SCTP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	if (q->q_next == NULL) {
		switch (mp->b_datap->db_type) {
		case M_IOCTL:
		case M_IOCDATA:
			/* FIXME: nak these instead of discarding */
			break;
		}
		freemsg(mp);
	} else {
		if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
			return (-EBUSY);
		putnext(q, mp);
	}
	return (QR_ABSORBED);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */

/**
 * sctp_n_w_prim_put: - put an NPI primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the message processing for the NPI write put procedure.  Data is simply queued by
 * returning -%EAGAIN.  This provides better flow control and scheduling for maximum throughput.
 * Also note that no private structure locks are acquired in the data fast path.
 */
STATIC inline fastcall __hot_write int
sctp_n_w_prim_put(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sctp_n_w_proto(q, mp);
	case M_DATA:
		return sctp_n_w_data(q, mp);
	case M_FLUSH:
		return sctp_w_flush(q, mp);
	default:
		return sctp_n_w_other(q, mp);
	}
}

#define PRELOAD (FASTBUF<<2)

/**
 * sctp_n_wput: - NPI write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 * 
 * This is a canonical put procedure for NPI  write.  Locking is performed by the individual message
 * handling procedures.
 */
STATIC streamscall __hot_write int
sctp_n_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(sctp_n_w_prim_put(q, mp) != QR_ABSORBED)) {
		// q->q_qinfo->qi_minfo->mi_mstat->ms_acnt++;
		/* apply backpressure */
		mp->b_wptr += PRELOAD;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* must succeed */
		}
	}
	return (0);
}

/**
 * sctp_n_w_prim_srv: - service an NPI primitive
 * @sp: locked private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the message processing for the NPI write service procedure.  Data is processed.
 * Processing data from the write service prcedure provides better flow control and scheduling for
 * maximum throughput.  Private structure locks must be held by the caller.
 */
STATIC inline fastcall __hot_out int
sctp_n_w_prim_srv(struct sctp *sp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __sctp_n_w_proto(sp, mp);
	case M_DATA:
		return __sctp_n_w_data(sp, mp);
	case M_FLUSH:
		return sctp_w_flush(q, mp);
	default:
		return sctp_n_w_other(q, mp);
	}
}

/**
 * sctp_n_wsrv: - NPI write service procedure
 * @q: active queue (write queue)
 *
 * This is a canonical service procedure for NPI write.  Locking is performed outside the loop so
 * that locks do not heed to be released and acquired again with each loop.  Note that the wakeup
 * function must also be executed with the private structure locked.
 */
STATIC streamscall __hot_out int
sctp_n_wsrv(queue_t *q)
{
	struct sctp *sp;

	if (likely((sp = sctp_trylockq(q)) != NULL)) {
		mblk_t *mp;

		while (likely((mp = getq(q)) != NULL)) {
			/* remove backpressure */
			mp->b_wptr -= PRELOAD;
			if (unlikely(sctp_n_w_prim_srv(sp, q, mp) != QR_ABSORBED)) {
				/* reapply backpressure */
				mp->b_wptr += PRELOAD;
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;
					putbq(q, mp);	/* must succeed */
				}
				break;
			}
		}
		___sctp_transmit_wakeup(sp);
		sctp_unlockq(sp);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC streamscall int
sctp_n_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	struct sctp *sp, **spp = &sctp_protolist;
	int err = 0;
	mblk_t *mp;
	struct stroptions *so;
	unsigned long flags;

	(void) crp;
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		rare();
		return (EIO);	/* can't open as module */
	}
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN)
		cminor = 1;
	if (!(mp = allocb(sizeof(*so), BPRI_WAITOK)))
		return (ENOBUFS);
	spin_lock_irqsave(&sctp_protolock, flags);
	for (; *spp && (*spp)->cmajor < cmajor; spp = &(*spp)->next) ;
	for (; *spp && cminor <= NMINORS; spp = &(*spp)->next) {
		ushort dminor = (*spp)->cminor;

		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag != CLONEOPEN)
				goto enxio;	/* requested device in use */
			cminor++;
		}
	}
	if (cminor > NMINORS)
		goto enxio;
	*devp = makedevice(cmajor, cminor);
	if (!(sp = sctp_alloc_priv(q, spp, cmajor, cminor, &n_ops, crp)))
		goto enomem;
	sctplogno(sp, "opened tpi device");
      unlock_exit:
	spin_unlock_irqrestore(&sctp_protolock, flags);
	if (err == 0) {
		so = (typeof(so)) mp->b_wptr;
		bzero(so, sizeof(*so));
		so->so_flags |= SO_HIWAT;
		so->so_hiwat = SHEADHIWAT << 1;
		so->so_flags |= SO_LOWAT;
		so->so_lowat = SHEADLOWAT << 1;
		mp->b_wptr += sizeof(*so);
		mp->b_datap->db_type = M_SETOPTS;
		putnext(q, mp);
	} else {
		freemsg(mp);
	}
	return (err);
      enxio:
	rare();
	err = ENXIO;
	goto unlock_exit;
      enomem:
	rare();
	err = ENOMEM;
	goto unlock_exit;
}
STATIC streamscall int
sctp_n_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sctp_free_priv(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

#ifdef LINUX
unsigned short n_modid = SCTP_N_DRV_ID;

#ifndef module_param
MODULE_PARM(n_modid, "h");
#else
module_param(n_modid, ushort, 0444);
#endif
MODULE_PARM_DESC(n_modid, "Module ID number for STREAMS SCTP NPI driver (0 for allocation).");

major_t n_major = SCTP_N_CMAJOR_0;

#ifndef module_param
MODULE_PARM(n_major, "h");
#else
module_param(n_major, uint, 0444);
#endif
MODULE_PARM_DESC(n_major, "Major device number for STREAMS SCTP NPI driver (0 for allocation).");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(SCTP_N_DRV_ID));
MODULE_ALIAS("streams-major-" __stringify(SCTP_N_CMAJOR_0));
MODULE_ALIAS("/dev/streams/sctp_n");
MODULE_ALIAS("/dev/streams/sctp_n/*");
MODULE_ALIAS("/dev/streams/clone/sctp_n");
#endif
MODULE_ALIAS("char-major-" __stringify(SCTP_N_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(SCTP_N_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(SCTP_N_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/sctp_n");
#endif				/* MODULE_ALIAS */

#endif				/* LINUX */

#ifdef LFS
/*
 *  =========================================================================
 *
 *  LfS Module Initialization
 *
 *  =========================================================================
 */

STATIC struct cdevsw sctp_n_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sctp_n_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

#ifdef WITH_NPI_IP_DRV
STATIC struct fmodsw sctp_n_fmod = {
	.f_name = DRV_NAME,
	.f_str = &sctp_n_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};
#endif				/* WITH_NPI_IP_DRV */
STATIC int
sctp_n_init(void)
{
	int err;

	if ((err = register_strdev(&sctp_n_cdev, n_major)) < 0)
		return (err);
	if (n_major == 0)
		n_major = err;
#ifdef WITH_NPI_IP_DRV
	if ((err = register_strmod(&sctp_n_fmod)) < 0) {
		unregister_strdev(&sctp_n_cdev, n_major);
		return (err);
	}
	n_modid = err;
#endif				/* WITH_NPI_IP_DRV */
	return (0);
};
STATIC void
sctp_n_term(void)
{
#ifdef WITH_NPI_IP_DRV
	unregister_strmod(&sctp_n_fmod);
#endif				/* WITH_NPI_IP_DRV */
	unregister_strdev(&sctp_n_cdev, n_major);
}
#endif				/* LFS */

#ifdef LIS
/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
STATIC void
sctp_n_init(void)
{
	int err;

	if ((err = lis_register_strdev(n_major, &sctp_n_info, UNITS, DRV_NAME)) < 0) {
		cmn_err(CE_PANIC, "%s: cannot register driver\n", DRV_NAME);
		return;
	}
	if (!n_major && err > 0)
		n_major = err;
	if ((err = lis_register_module_qlock_option(n_major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(n_major);
		cmn_err(CE_PANIC, "%s: cannot register driver\n", DRV_NAME);
		return;
	}
	return;
}
STATIC void
sctp_n_term(void)
{
	int err;

	if ((err = lis_unregister_strdev(n_major))) {
		cmn_err(CE_PANIC, "%s: cannot unregister driver!\n", DRV_NAME);
	}
}
#endif				/* LIS */

/*
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  Transport Provider Interface
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 *  TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 */

#ifdef LFS
#define SCTP_T_DRV_ID	    CONFIG_STREAMS_SCTP_T_MODID
#define SCTP_T_DRV_NAME	    CONFIG_STREAMS_SCTP_T_NAME
#define SCTP_T_CMAJORS	    CONFIG_STREAMS_SCTP_T_NMAJORS
#define SCTP_T_CMAJOR_0	    CONFIG_STREAMS_SCTP_T_MAJOR
#define SCTP_T_UNITS	    CONFIG_STREAMS_SCTP_T_NMINORS
#endif				/* LFS */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 *  This driver defines two user interfaces: one NPI, the other TPI.
 */

#ifndef _OPTIMIZE_SPEED
STATIC const char *
sctp_t_statename(t_scalar_t state)
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
		return ("????");
	}
}

STATIC inline void
sctp_t_setstate(struct sctp *sp, t_scalar_t newstate)
{
	t_scalar_t oldstate;
	
	if ((oldstate = XCHG(&sp->i_state, newstate)) != newstate)
		sctplogst(sp, "%s <- %s", sctp_t_statename(newstate), sctp_t_statename(oldstate));
}
#else
#define sctp_t_setstate(sp, newstate) do { sp->i_state = newstate; } while (0)
#endif

#undef DRV_ID
#undef DRV_NAME
#undef CMAJORS
#undef CMAJOR_0
#undef UNITS

#define DRV_ID		SCTP_T_DRV_ID
#define DRV_NAME	SCTP_T_DRV_NAME
#define CMAJORS		SCTP_T_CMAJORS
#define CMAJOR_0	SCTP_T_CMAJOR_0
#define UNITS		SCTP_T_UNITS

STATIC struct module_info sctp_t_rinfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT << 3,	/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC struct module_info sctp_t_winfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT,		/* Hi water mark */
	.mi_lowat = SHEADLOWAT,		/* Lo water mark */
};

STATIC struct module_stat sctp_t_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
STATIC struct module_stat sctp_t_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

STATIC streamscall int sctp_t_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int sctp_t_close(queue_t *, int, cred_t *);

STATIC streamscall int sctp_rput(queue_t *, mblk_t *);
STATIC streamscall int sctp_rsrv(queue_t *);

STATIC struct qinit sctp_t_rinit = {
	.qi_putp = sctp_rput,		/* Read put (msg from below) */
	.qi_srvp = sctp_rsrv,		/* Read queue service */
	.qi_qopen = sctp_t_open,	/* Each open */
	.qi_qclose = sctp_t_close,	/* Last close */
	.qi_minfo = &sctp_t_rinfo,	/* Information */
	.qi_mstat = &sctp_t_rstat,	/* Statistics */
};

STATIC streamscall int sctp_t_wput(queue_t *, mblk_t *);
STATIC streamscall int sctp_t_wsrv(queue_t *);

STATIC struct qinit sctp_t_winit = {
	.qi_putp = sctp_t_wput,		/* Write put (msg from above) */
	.qi_srvp = sctp_t_wsrv,		/* Write queue service */
	.qi_minfo = &sctp_t_winfo,	/* Information */
	.qi_mstat = &sctp_t_wstat,	/* Statistics */
};

STATIC struct streamtab sctp_t_info = {
	.st_rdinit = &sctp_t_rinit,	/* Upper read queue */
	.st_wrinit = &sctp_t_winit,	/* Upper write queue */
#if defined WITH_NPI_IP_MUX
	.st_muxrinit = &sctp_m_rinit,	/* Lower read queue */
	.st_muxwinit = &sctp_m_winit,	/* Lower write queue */
#endif					/* defined WITH_NPI_IP_MUX */
};

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */
#define T_SPACE(len) \
	(sizeof(struct t_opthdr) + T_ALIGN(len))

#define T_LENGTH(len) \
	(sizeof(struct t_opthdr) + len)

#define _T_SPACE_SIZEOF(s) \
	T_SPACE(sizeof(s))

#define _T_LENGTH_SIZEOF(s) \
	T_LENGTH(sizeof(s))

#ifdef __LP64__
#undef MAX_SCHEDULE_TIMEOUT
#define MAX_SCHEDULE_TIMEOUT INT_MAX
#endif

/*
 *  Parse connection request or response options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Only legal options can be negotiated; illegal options cause failure.  An option is illegal if
 *  the following applies: 1) the length specified in t_opthdr.len exceeds the remaining size of the
 *  option buffer (counted from the beginning of the option); 2) the option value is illegal: the
 *  legal values are defined for each option.  If an illegal option is passed to XTI, the following
 *  will happen: ... if an illegal option is passed to t_accept() or t_connect() then either the
 *  function failes with t_errno set to [TBADOPT] or the connection establishment fails at a later
 *  stage, depending on when the implementation detects the illegal option. ...
 *
 *  If the tansport user passes multiple optiohs in one call and one of them is illegal, the call
 *  fails as described above.  It is, however, possible that some or even all of the smbmitted legal
 *  options were successfully negotiated.  The transport user can check the current status by a call
 *  to t_optmgmt() with the T_CURRENT flag set.
 *
 *  Specifying an option level unknown to the transport provider does not fail in calls to
 *  t_connect(), t_accept() or t_sndudata(); the option is discarded in these cases.  The function
 *  t_optmgmt() fails with [TBADOPT].
 *
 *  Specifying an option name that is unknown to or not supported by the protocol selected by the
 *  option level does not cause failure.  The option is discarded in calles to t_connect(),
 *  t_accept() or t_sndudata().  The function t_optmgmt() returns T_NOTSUPPORT in the status field
 *  of the option.
 *
 *  If a transport user requests negotiation of a read-only option, or a non-privileged user
 *  requests illegal access to a privileged option, the following outcomes are possible: ... 2) if
 *  negotiation of a read-only option is required, t_accept() or t_connect() either fail with
 *  [TACCES], or the connection establishment aborts and a T_DISCONNECT event occurs.  If the
 *  connection aborts, a synchronous call to t_connect() failes with [TLOOK].  It depdends on timing
 *  an implementation conditions whether a t_accept() call still succeeds or failes with [TLOOK].
 *  If a privileged option is illegally requested, the option is quietly ignored.  (A non-privileged
 *  user shall not be able to select an option which is privileged or unsupported.)
 *
 *  If multiple options are submitted to t_connect(), t_accept() or t_sndudata() and a read-only
 *  option is rejected, the connection or the datagram transmission fails as described.  Options
 *  that could be successfully negotiated before the erroneous option was processed retain their
 *  negotiated values.  There is no roll-back mechanmism.
 */
STATIC int
t_parse_conn_opts(struct sctp *sp, const unsigned char *ip, size_t ilen, int request)
{
	const struct t_opthdr *ih;

	/* clear flags, these flags will be used when sending a connection confirmation to
	   determine which options to include in the confirmstion. */
	bzero(sp->options.flags, sizeof(sp->options.flags));
	if (ip == NULL || ilen == 0)
		return (0);
	if (!sp)
		goto eproto;
	/* For each option recognized, we test the requested value for legallity, and then set the
	   requested value in the stream's option buffer and mark the option requested in the
	   options flags.  If it is a request (and not a response), we negotiate the value to the
	   underlying stream.  Once the protocol has completed remote negotiation, we will
	   determine whether the negotiation was successful or partially successful.  See
	   t_build_conn_opts(). */
	/* For connection responses, test the legality of each option and mark the option in the
	   options flags.  We do not negotiate to the socket because the final socket is not
	   present.  t_set_options() will read the flags and negotiate to the final socket after
	   the connection has been accepted. */
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		switch (ih->level) {
		case XTI_GENERIC:
			switch (ih->name) {
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (((ih->len - sizeof(*ih)) % sizeof(t_uscalar_t)) != 0)
					goto einval;
				if (ih->len >= sizeof(*ih) + 4 * sizeof(t_uscalar_t))
					sp->options.xti.debug[3] = valp[3];
				else
					sp->options.xti.debug[3] = 0;
				if (ih->len >= sizeof(*ih) + 3 * sizeof(t_uscalar_t))
					sp->options.xti.debug[2] = valp[2];
				else
					sp->options.xti.debug[2] = 0;
				if (ih->len >= sizeof(*ih) + 2 * sizeof(t_uscalar_t))
					sp->options.xti.debug[1] = valp[1];
				else
					sp->options.xti.debug[1] = 0;
				if (ih->len >= sizeof(*ih) + 1 * sizeof(t_uscalar_t))
					sp->options.xti.debug[0] = valp[0];
				else
					sp->options.xti.debug[1] = 0;
				t_set_bit(_T_BIT_XTI_DEBUG, sp->options.flags);
				continue;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (struct t_linger *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					goto einval;
				if (valp->l_linger != T_INFINITE && valp->l_linger != T_UNSPEC
				    && valp->l_linger < 0)
					goto einval;
				sp->options.xti.linger = *valp;
				t_set_bit(_T_BIT_XTI_LINGER, sp->options.flags);
				if (!request)
					continue;
				if (valp->l_onoff == T_NO)
					valp->l_linger = T_UNSPEC;
				else {
					if (valp->l_linger == T_UNSPEC)
						valp->l_linger = t_defaults.xti.linger.l_linger;
					if (valp->l_linger == T_INFINITE)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
					if (valp->l_linger >= MAX_SCHEDULE_TIMEOUT / HZ)
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
				}
				if (valp->l_onoff == T_YES) {
					sp->linger = 1;
					sp->lingertime = valp->l_linger * HZ;
				} else {
					sp->linger = 0;
					sp->lingertime = 0;
				}
				continue;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.xti.rcvbuf = *valp;
				t_set_bit(_T_BIT_XTI_RCVBUF, sp->options.flags);
				if (!request)
					continue;
				if (*valp > sysctl_rmem_max)
					*valp = sysctl_rmem_max;
				if (*valp < SOCK_MIN_RCVBUF / 2)
					*valp = SOCK_MIN_RCVBUF / 2;
				sp->rcvbuf = *valp * 2;
				continue;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.xti.rcvlowat = *valp;
				t_set_bit(_T_BIT_XTI_RCVLOWAT, sp->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > INT_MAX)
					*valp = INT_MAX;
				sp->rcvlowat = *valp;
				continue;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.xti.sndbuf = *valp;
				t_set_bit(_T_BIT_XTI_SNDBUF, sp->options.flags);
				if (!request)
					continue;
				if (*valp > sysctl_wmem_max)
					*valp = sysctl_wmem_max;
				if (*valp < SOCK_MIN_SNDBUF / 2)
					*valp = SOCK_MIN_SNDBUF / 2;
				sp->sndbuf = *valp * 2;
				continue;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.xti.sndlowat = *valp;
				t_set_bit(_T_BIT_XTI_SNDLOWAT, sp->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > 1)
					*valp = 1;
				continue;
			}
			}
			continue;
		case T_INET_IP:
		{
			struct inet_opt *np = &sp->inet;

			switch (ih->name) {
			case T_IP_OPTIONS:
			{
				unsigned char *valp = (unsigned char *) T_OPT_DATA(ih);

				(void) valp;	/* FIXME */
				t_set_bit(_T_BIT_IP_OPTIONS, sp->options.flags);
				continue;
			}
			case T_IP_TOS:
			{
				unsigned char *valp = (unsigned char *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.ip.tos = *valp;
				t_set_bit(_T_BIT_IP_TOS, sp->options.flags);
				if (!request)
					continue;
				np->tos = *valp;
				continue;
			}
			case T_IP_TTL:
			{
				unsigned char *valp = (unsigned char *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.ip.ttl = *valp;
				t_set_bit(_T_BIT_IP_TTL, sp->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
#if 0
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
				np->ttl = *valp;
#else
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
				np->uc_ttl = *valp;
#endif
#endif				/* defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
#else
				np->uc_ttl = *valp;
#endif
				continue;
			}
			case T_IP_REUSEADDR:
			{
				unsigned int *valp = (unsigned int *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				sp->options.ip.reuseaddr = *valp;
				t_set_bit(_T_BIT_IP_REUSEADDR, sp->options.flags);
				if (!request)
					continue;
				sp->reuse = (*valp == T_YES) ? 1 : 0;
				continue;
			}
			case T_IP_DONTROUTE:
			{
				unsigned int *valp = (unsigned int *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				sp->options.ip.dontroute = *valp;
				t_set_bit(_T_BIT_IP_DONTROUTE, sp->options.flags);
				if (!request)
					continue;
				sp->localroute = (*valp == T_YES) ? 1 : 0;
				continue;
			}
			case T_IP_BROADCAST:
			{
				unsigned int *valp = (unsigned int *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				sp->options.ip.broadcast = *valp;
				t_set_bit(_T_BIT_IP_BROADCAST, sp->options.flags);
				if (!request)
					continue;
				sp->broadcast = (*valp == T_YES) ? 1 : 0;
				continue;
			}
#if 0
			case T_IP_ADDR:
			{
				uint32_t *valp = (unsigned int *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.ip.addr = *valp;
				t_set_bit(_T_BIT_IP_ADDR, sp->options.flags);
				if (!request)
					continue;
				sk->saddr = *valp;
				continue;
			}
#endif
			}
		}
			continue;
#if 0
		case T_INET_UDP:
			switch (ih->name) {
			case T_UDP_CHECKSUM:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				ss->options.udp.checksum = *valp;
				t_set_bit(_T_BIT_UDP_CHECKSUM, ss->options.flags);
				if (!request)
					continue;
				sk->sk_no_check =
				    (*valp == T_YES) ? UDP_CSUM_DEFAULT : UDP_CSUM_NOXMIT;
				continue;
			}
			}
		case T_INET_TCP:
			switch (ih->name) {
			case T_TCP_NODELAY:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				ss->options.tcp.nodelay = *valp;
				t_set_bit(_T_BIT_TCP_NODELAY, ss->options.flags);
				if (!request)
					continue;
				tp->nonagle = (*valp == T_YES) ? 1 : 0;
				continue;
			}
			case T_TCP_MAXSEG:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				ss->options.tcp.maxseg = *valp;
				t_set_bit(_T_BIT_TCP_MAXSEG, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 8)
					*valp = 8;
				if (*valp > MAX_TCP_WINDOW)
					*valp = MAX_TCP_WINDOW;
				tp->user_mss = *valp;
				continue;
			}
			case T_TCP_KEEPALIVE:
			{
				struct t_kpalive *valp = (struct t_kpalive *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->kp_onoff != T_YES && valp->kp_onoff != T_NO)
					goto einval;
				if (valp->kp_timeout != T_INFINITE && valp->kp_timeout != T_UNSPEC
				    && valp->kp_timeout < 0)
					goto einval;
				ss->options.tcp.keepalive = *valp;
				t_set_bit(_T_BIT_TCP_KEEPALIVE, ss->options.flags);
				if (!request)
					continue;
				if (valp->kp_onoff == T_NO)
					valp->kp_timeout = T_UNSPEC;
				else {
					if (valp->kp_timeout == T_UNSPEC)
						valp->kp_timeout =
						    t_defaults.tcp.keepalive.kp_timeout;
					if (valp->kp_timeout < 1)
						valp->kp_timeout = 1;
					if (valp->kp_timeout > MAX_SCHEDULE_TIMEOUT / 60 / HZ)
						valp->kp_timeout = MAX_SCHEDULE_TIMEOUT / 60 / HZ;
				}
				if (valp->kp_onoff == T_YES)
					tp->keepalive_time = valp->kp_timeout * 60 * HZ;
#if defined HAVE_TCP_SET_KEEPALIVE_ADDR
				tcp_set_keepalive(sk, valp->kp_onoff == T_YES ? 1 : 0);
#endif				/* defined HAVE_TCP_SET_KEEPALIVE_ADDR */
				if (valp->kp_onoff == T_YES)
					sock_set_keepopen(sk);
				else
					sock_clr_keepopen(sk);
				continue;
			}
			case T_TCP_CORK:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				if (*valp != T_YES && *valp != T_NO)
					goto einval;
				ss->options.tcp.cork = *valp;
				t_set_bit(_T_BIT_TCP_CORK, ss->options.flags);
				if (!request)
					continue;
				if (*valp == T_YES && tp->nonagle == 0)
					tp->nonagle = 2;
				if (*valp == T_NO && tp->nonagle == 2)
					tp->nonagle = 0;
				continue;
			}
			case T_TCP_KEEPIDLE:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				ss->options.tcp.keepidle = *valp;
				t_set_bit(_T_BIT_TCP_KEEPIDLE, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_KEEPIDLE)
					*valp = MAX_TCP_KEEPIDLE;
				tp->keepalive_time = *valp * HZ;
				/* TODO: need to reset the keepalive timer */
				continue;
			}
			case T_TCP_KEEPINTVL:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				ss->options.tcp.keepitvl = *valp;
				t_set_bit(_T_BIT_TCP_KEEPINTVL, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_KEEPINTVL)
					*valp = MAX_TCP_KEEPINTVL;
				tp->keepalive_intvl = *valp * HZ;
				continue;
			}
			case T_TCP_KEEPCNT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				ss->options.tcp.keepcnt = *valp;
				t_set_bit(_T_BIT_TCP_KEEPCNT, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_KEEPCNT)
					*valp = MAX_TCP_KEEPCNT;
				tp->keepalive_probes = *valp;
				continue;
			}
			case T_TCP_SYNCNT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				ss->options.tcp.syncnt = *valp;
				t_set_bit(_T_BIT_TCP_SYNCNT, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
				if (*valp > MAX_TCP_SYNCNT)
					*valp = MAX_TCP_SYNCNT;
				tp->syn_retries = *valp;
				continue;
			}
			case T_TCP_LINGER2:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				ss->options.tcp.linger2 = *valp;
				t_set_bit(_T_BIT_TCP_LINGER2, ss->options.flags);
				if (!request)
					continue;
				if (*valp < 0)
					tp->linger2 = -1;
				else if (*valp > sysctl_tcp_fin_timeout / HZ)
					tp->linger2 = 0;
				else
					tp->linger2 = *valp * HZ;
				continue;
			}
			case T_TCP_DEFER_ACCEPT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				ss->options.tcp.defer_accept = *valp;
				t_set_bit(_T_BIT_TCP_DEFER_ACCEPT, ss->options.flags);
				if (!request)
					continue;
				if (*valp == 0)
					tp->defer_accept = 0;
				else {
					for (tp->defer_accept = 0;
					     tp->defer_accept < 32
					     && *valp >
					     ((TCP_TIMEOUT_INIT / HZ) << tp->defer_accept);
					     tp->defer_accept++) ;
					tp->defer_accept++;
				}
				continue;
			}
			case T_TCP_WINDOW_CLAMP:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len != sizeof(*ih) + sizeof(*valp))
					goto einval;
				ss->options.tcp.window_clamp = *valp;
				t_set_bit(_T_BIT_TCP_WINDOW_CLAMP, ss->options.flags);
				if (!request)
					continue;
				if (*valp < SOCK_MIN_RCVBUF / 2)
					*valp = SOCK_MIN_RCVBUF / 2;
				tp->window_clamp = *valp;
				continue;
			}
#if 0
			case T_TCP_INFO:
			{
				/* read-only */
			}
			case T_TCP_QUICKACK:
			{
				/* not valid in this state */
			}
#endif
			}
#endif
		case T_INET_SCTP:
		{
			switch (ih->name) {
			case T_SCTP_NODELAY:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				sp->options.sctp.nodelay = *valp;
				t_set_bit(_T_BIT_SCTP_NODELAY, sp->options.flags);
				if (!request)
					continue;
				continue;
			}
			case T_SCTP_MAXSEG:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.maxseg = *valp;
				t_set_bit(_T_BIT_SCTP_MAXSEG, sp->options.flags);
				if (!request)
					continue;
				if (*valp < 1)
					*valp = 1;
#if 0
				if (*valp > MAX_SCTP_WINDOW)
					*valp = MAX_SCTP_WINDOW;
#endif
				sp->user_amps = *valp;
				continue;
			}
			case T_SCTP_CORK:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				sp->options.sctp.cork = *valp;
				t_set_bit(_T_BIT_SCTP_CORK, sp->options.flags);
				if (!request)
					continue;
				if (sp->nonagle != 1)
					sp->nonagle = (*valp == T_YES) ? 2 : 0;
				continue;
			}
			case T_SCTP_PPI:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.ppi = *valp;
				t_set_bit(_T_BIT_SCTP_PPI, sp->options.flags);
				if (!request)
					continue;
				sp->ppi = *valp;
				continue;
			}
			case T_SCTP_SID:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp > sp->n_ostr)
					goto einval;
				sp->options.sctp.sid = *valp;
				t_set_bit(_T_BIT_SCTP_SID, sp->options.flags);
				if (!request)
					continue;
				sp->sid = *valp;
				continue;
			}
			case T_SCTP_SSN:
			{
				/* read-only, and only per-packet */
				goto einval;
			}
			case T_SCTP_TSN:
			{
				/* read-only, and only per-packet */
				goto einval;
			}
			case T_SCTP_RECVOPT:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_NO && *valp != T_YES)
					goto einval;
				sp->options.sctp.recvopt = *valp;
				t_set_bit(_T_BIT_SCTP_RECVOPT, sp->options.flags);
				if (!request)
					continue;
#if 0
				/* FIXME */
				if (*valp == T_YES)
					sp->cmsg_flags |=
					    (SCTP_CMSGF_RECVSID | SCTP_CMSGF_RECVPPI |
					     SCTP_CMSGF_RECVSSN | SCTP_CMSGF_RECVTSN);
				else
					sp->cmsg_flags &=
					    ~(SCTP_CMSGF_RECVSID | SCTP_CMSGF_RECVPPI |
					      SCTP_CMSGF_RECVSSN | SCTP_CMSGF_RECVTSN);
#endif
				continue;
			}
			case T_SCTP_COOKIE_LIFE:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.cookie_life = *valp;
				t_set_bit(_T_BIT_SCTP_COOKIE_LIFE, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->ck_life = *valp / 1000 * HZ;
				continue;
			}
			case T_SCTP_SACK_DELAY:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.sack_delay = *valp;
				t_set_bit(_T_BIT_SCTP_SACK_DELAY, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->max_sack = *valp / 1000 * HZ;
				continue;
			}
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.sack_freq = *valp;
				t_set_bit(_T_BIT_SCTP_SACK_FREQUENCY, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = 5;
				if (*valp > 5)
					*valp = 5;
				if (*valp < 1)
					*valp = 1;
				sp->sack_freq = *valp;
				continue;
			}
#endif
			case T_SCTP_PATH_MAX_RETRANS:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.path_max_retrans = *valp;
				t_set_bit(_T_BIT_SCTP_PATH_MAX_RETRANS, sp->options.flags);
				if (!request)
					continue;
				sp->rtx_path = *valp;
				sctplogno(sp, "sp->rtx_path = %d", (int) sp->rtx_path);
				continue;
			}
			case T_SCTP_ASSOC_MAX_RETRANS:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.assoc_max_retrans = *valp;
				t_set_bit(_T_BIT_SCTP_ASSOC_MAX_RETRANS, sp->options.flags);
				if (!request)
					continue;
				sp->max_retrans = *valp;
				continue;
			}
			case T_SCTP_MAX_INIT_RETRIES:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.max_init_retries = *valp;
				t_set_bit(_T_BIT_SCTP_MAX_INIT_RETRIES, sp->options.flags);
				if (!request)
					continue;
				sp->max_inits = *valp;
				continue;
			}
			case T_SCTP_HEARTBEAT_ITVL:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.heartbeat_itvl = *valp;
				t_set_bit(_T_BIT_SCTP_HEARTBEAT_ITVL, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->hb_itvl = *valp / 1000 * HZ;
				sctplogno(sp, "sp->hb_itvl = %d", (int) sp->hb_itvl);
#if defined SCTP_CONFIG_THROTTLE_HEARTBEATS
				sp->hb_tint = (*valp >> 1) + 1;
#endif
				continue;
			}
			case T_SCTP_RTO_INITIAL:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.rto_initial = *valp;
				t_set_bit(_T_BIT_SCTP_RTO_INITIAL, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp > sp->rto_max / HZ * 1000)
					*valp = sp->rto_max / HZ * 1000;
				if (*valp < sp->rto_min / HZ * 1000)
					*valp = sp->rto_min / HZ * 1000;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->rto_ini = *valp / 1000 * HZ;
				sctplogno(sp, "sp->rto_ini = %d", (int) sp->rto_ini);
				continue;
			}
			case T_SCTP_RTO_MIN:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.rto_min = *valp;
				t_set_bit(_T_BIT_SCTP_RTO_MIN, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp > sp->rto_max / HZ * 1000)
					*valp = sp->rto_max / HZ * 1000;
				if (*valp > sp->rto_ini / HZ * 1000)
					*valp = sp->rto_ini / HZ * 1000;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->rto_min = *valp / 1000 * HZ;
				sctplogno(sp, "sp->rto_min = %d", (int) sp->rto_min);
				continue;
			}
			case T_SCTP_RTO_MAX:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.rto_max = *valp;
				t_set_bit(_T_BIT_SCTP_RTO_MAX, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp < sp->rto_min / HZ * 1000)
					*valp = sp->rto_min / HZ * 1000;
				if (*valp < sp->rto_ini / HZ * 1000)
					*valp = sp->rto_ini / HZ * 1000;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->rto_max = *valp / 1000 * HZ;
				sctplogno(sp, "sp->rto_max = %d", (int) sp->rto_max);
				continue;
			}
			case T_SCTP_OSTREAMS:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp < 1 || *valp > 0x0ffff)
					goto einval;
				sp->options.sctp.ostreams = *valp;
				t_set_bit(_T_BIT_SCTP_OSTREAMS, sp->options.flags);
				if (!request)
					continue;
				sp->req_ostr = *valp;
				continue;
			}
			case T_SCTP_ISTREAMS:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp < 1 || *valp > 0x0ffff)
					goto einval;
				sp->options.sctp.istreams = *valp;
				t_set_bit(_T_BIT_SCTP_ISTREAMS, sp->options.flags);
				if (!request)
					continue;
				sp->max_istr = *valp;
				continue;
			}
			case T_SCTP_COOKIE_INC:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.cookie_inc = *valp;
				t_set_bit(_T_BIT_SCTP_COOKIE_INC, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->ck_inc = *valp / 1000 * HZ;
				continue;
			}
			case T_SCTP_THROTTLE_ITVL:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.throttle_itvl = *valp;
				t_set_bit(_T_BIT_SCTP_THROTTLE_ITVL, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->throttle = *valp / 1000 * HZ;
				continue;
			}
			case T_SCTP_MAC_TYPE:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_SCTP_HMAC_NONE
#if defined SCTP_CONFIG_HMAC_SHA1
				    && *valp != T_SCTP_HMAC_SHA1
#endif
#if defined SCTP_CONFIG_HMAC_MD5
				    && *valp != T_SCTP_HMAC_MD5
#endif
				    )
					goto einval;
				sp->options.sctp.mac_type = *valp;
				t_set_bit(_T_BIT_SCTP_MAC_TYPE, sp->options.flags);
				if (!request)
					continue;
				sp->hmac = *valp;
				continue;
			}
			case T_SCTP_CKSUM_TYPE:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (1
#if defined SCTP_CONFIG_ADLER_32 || !defined SCTP_CONFIG_CRC_32C
				    && *valp != T_SCTP_CSUM_ADLER32
#endif
#if defined SCTP_CONFIG_CRC_32C
				    && *valp != T_SCTP_CSUM_CRC32C
#endif
				    )
					goto einval;
				sp->options.sctp.cksum_type = *valp;
				t_set_bit(_T_BIT_SCTP_CKSUM_TYPE, sp->options.flags);
				if (!request)
					continue;
				sp->cksum = *valp;
				continue;
			}
			case T_SCTP_HB:
			{
				struct t_sctp_hb *valp = (struct t_sctp_hb *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->hb_onoff != T_YES && valp->hb_onoff != T_NO)
					goto einval;
				if (valp->hb_itvl != T_INFINITE && valp->hb_itvl != T_UNSPEC
				    && valp->hb_itvl <= 0)
					goto einval;
				sp->options.sctp.hb = *valp;
				t_set_bit(_T_BIT_SCTP_HB, sp->options.flags);
				if (!request)
					continue;
				if (valp->hb_itvl == T_UNSPEC)
					valp->hb_itvl = t_defaults.sctp.hb.hb_itvl;
				if (valp->hb_itvl == T_INFINITE)
					valp->hb_itvl = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->hb_itvl / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->hb_itvl = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->hb_itvl < 1000 / HZ)
					valp->hb_itvl = 1000 / HZ;
				/* FIXME: set values for destination address */
				continue;
			}
			case T_SCTP_RTO:
			{
				struct t_sctp_rto *valp = (struct t_sctp_rto *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->rto_initial != T_INFINITE && valp->rto_initial != T_UNSPEC
				    && valp->rto_initial < 0)
					goto einval;
				if (valp->rto_min != T_INFINITE && valp->rto_min != T_UNSPEC
				    && valp->rto_min < 0)
					goto einval;
				if (valp->rto_max != T_INFINITE && valp->rto_max != T_UNSPEC
				    && valp->rto_max < 0)
					goto einval;
				if (valp->max_retrans != T_INFINITE && valp->max_retrans != T_UNSPEC
				    && valp->max_retrans < 0)
					goto einval;
				if (valp->rto_initial < valp->rto_min
				    || valp->rto_max < valp->rto_min
				    || valp->rto_max < valp->rto_initial)
					goto einval;
				sp->options.sctp.rto = *valp;
				t_set_bit(_T_BIT_SCTP_RTO, sp->options.flags);
				if (!request)
					continue;
				if (valp->rto_initial == T_INFINITE)
					valp->rto_initial = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_initial / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->rto_initial = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_min == T_INFINITE)
					valp->rto_min = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_min / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->rto_min = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_max == T_INFINITE)
					valp->rto_max = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (valp->rto_max / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					valp->rto_max = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				/* FIXME: set values for destination address */
				continue;
			}
			case T_SCTP_STATUS:
			{
				/* this is a read-only option */
				goto einval;
			}
			case T_SCTP_DEBUG:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.debug = *valp;
				t_set_bit(_T_BIT_SCTP_DEBUG, sp->options.flags);
				if (!request)
					continue;
#if 0
				/* FIXME */
				sp->options = *valp;
#endif
				continue;
			}
#if defined SCTP_CONFIG_ECN
			case T_SCTP_ECN:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_YES && *valp != T_NO)
					goto einval;
				if (*valp == T_YES && !t_defaults.sctp.ecn)
					goto eacces;
				sp->options.sctp.ecn = *valp;
				t_set_bit(_T_BIT_SCTP_ECN, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_YES)
					sp->l_caps |= SCTP_CAPS_ECN;
				else
					sp->l_caps &= ~SCTP_CAPS_ECN;
				continue;
			}
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
			case T_SCTP_ALI:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.ali = *valp;
				t_set_bit(_T_BIT_SCTP_ALI, sp->options.flags);
				if (!request)
					continue;
				sp->l_ali = *valp;
				if (*valp)
					sp->l_caps |= SCTP_CAPS_ALI;
				else
					sp->l_caps &= ~SCTP_CAPS_ALI;
				continue;
			}
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
			case T_SCTP_ADD:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_YES && *valp != T_NO)
					goto einval;
				sp->options.sctp.add = *valp;
				t_set_bit(_T_BIT_SCTP_ADD, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_YES)
					sp->l_caps |= SCTP_CAPS_ADD_IP;
				else
					sp->l_caps &= ~SCTP_CAPS_ADD_IP;
				continue;
			}
			case T_SCTP_SET:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_YES && *valp != T_NO)
					goto einval;
				sp->options.sctp.set = *valp;
				t_set_bit(_T_BIT_SCTP_SET, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_YES)
					sp->l_caps |= SCTP_CAPS_SET_IP;
				else
					sp->l_caps &= ~SCTP_CAPS_SET_IP;
				continue;
			}
#if 0
				/* We do not add, delete or set ip addresses on connection request
				   or on connection response. */
			case T_SCTP_ADD_IP:
			{
				struct sockaddr_in *valp = (struct sockaddr_in *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->sin_family != AF_INET)
					goto einval;
				if (!sp->sport || valp->sin_port != sp->sport)
					goto einval;
				sp->options.sctp.add_ip = *valp;
				t_set_bit(_T_BIT_SCTP_ADD_IP, sp->options.flags);
				if (!request)
					continue;
				sctp_add_ip(sp, valp);
				continue;
			}
			case T_SCTP_DEL_IP:
			{
				struct sockaddr_in *valp = (struct sockaddr_in *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->sin_family != AF_INET)
					goto einval;
				if (!sp->sport || valp->sin_port != sp->sport)
					goto einval;
				sp->options.sctp.del_ip = *valp;
				t_set_bit(_T_BIT_SCTP_DEL_IP, sp->options.flags);
				if (!request)
					continue;
				sctp_del_ip(sp, valp);
				continue;
			}
			case T_SCTP_SET_IP:
			{
				struct sockaddr_in *valp = (struct sockaddr_in *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (valp->sin_family != AF_INET)
					goto einval;
				if (!sp->sport || valp->sin_port != sp->sport)
					goto einval;
				sp->options.sctp.set_ip = *valp;
				t_set_bit(_T_BIT_SCTP_SET_IP, sp->options.flags);
				if (!request)
					continue;
				sctp_set_ip(sp, valp);
				continue;
			}
#endif
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_PR:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != SCTP_PR_NONE && *valp != SCTP_PR_PREFERRED
				    && *valp != SCTP_PR_REQUIRED)
					goto einval;
				sp->options.sctp.pr = *valp;
				t_set_bit(_T_BIT_SCTP_PR, sp->options.flags);
				if (!request)
					continue;
				sp->prel = *valp;
				continue;
			}
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_LIFETIME:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				sp->options.sctp.lifetime = *valp;
				t_set_bit(_T_BIT_SCTP_LIFETIME, sp->options.flags);
				if (!request)
					continue;
				if (*valp == T_INFINITE)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000)
					*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
				sp->life = *valp / 1000 * HZ;
				continue;
			}
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
			case T_SCTP_DISPOSITION:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp != T_SCTP_DISPOSITION_NONE
				    && *valp != T_SCTP_DISPOSITION_UNSENT
				    && *valp != T_SCTP_DISPOSITION_SENT
				    && *valp != T_SCTP_DISPOSITION_GAP_ACKED
				    && *valp != T_SCTP_DISPOSITION_ACKED)
					goto einval;
				sp->options.sctp.disposition = *valp;
				t_set_bit(_T_BIT_SCTP_DISPOSITION, sp->options.flags);
				if (!request)
					continue;
				sp->disp = *valp;
				continue;
			}
			case T_SCTP_MAX_BURST:
			{
				t_uscalar_t *valp = (t_uscalar_t *) T_OPT_DATA(ih);

				if (ih->len - sizeof(*ih) != sizeof(*valp))
					goto einval;
				if (*valp <= 0)
					goto einval;
				sp->options.sctp.max_burst = *valp;
				t_set_bit(_T_BIT_SCTP_MAX_BURST, sp->options.flags);
				if (!request)
					continue;
				sp->max_burst = *valp;
				continue;
			}
			}
		}
			continue;
		}
	}
	return (0);
      einval:
	return (-EINVAL);
      eacces:
	return (-EACCES);
      eproto:
	swerr();
	return (-EPROTO);
}

/*
 *  Size connection indication options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Return an option buffer size for a connection indication.  Options without end-to-end
 *  significance are not indicated.  Options with end-to-end significance are always indicated.  For
 *  more information on XTI connection indication option handling, see t_rcvconnect(3).
 */
STATIC int
sctp_size_conn_ind_opts(void)
{
	int size = 0;

#if 0
	size += _T_SPACE_SIZEOF(t_defaults.xti.debug);
	size += _T_SPACE_SIZEOF(t_defaults.xti.linger);
	size += _T_SPACE_SIZEOF(t_defaults.xti.rcvbuf);
	size += _T_SPACE_SIZEOF(t_defaults.xti.rcvlowat);
	size += _T_SPACE_SIZEOF(t_defaults.xti.sndbuf);
	size += _T_SPACE_SIZEOF(t_defaults.xti.sndlowat);
#endif
	/* These two have end-to-end significance for connection indications and responses. */
	size += _T_SPACE_SIZEOF(t_defaults.ip.options);
	size += _T_SPACE_SIZEOF(t_defaults.ip.tos);
#if 0
	size += _T_SPACE_SIZEOF(t_defaults.ip.ttl);
	size += _T_SPACE_SIZEOF(t_defaults.ip.reuseaddr);
	size += _T_SPACE_SIZEOF(t_defaults.ip.dontroute);
	size += _T_SPACE_SIZEOF(t_defaults.ip.broadcast);
	size += _T_SPACE_SIZEOF(t_defaults.ip.addr);
	size += _T_SPACE_SIZEOF(t_defaults.ip.retopts);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.nodelay);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.maxseg);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.cork);
#endif
	/* If we have data associated with the connection indication, these four might have to be
	   delivered as well.  Allow space for them. */
	size += _T_SPACE_SIZEOF(t_defaults.sctp.ppi);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.sid);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.ssn);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.tsn);
#if 0
	size += _T_SPACE_SIZEOF(t_defaults.sctp.recvopt);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.cookie_life);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.sack_delay);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.path_max_retrans);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.assoc_max_retrans);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.max_init_retries);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.heartbeat_itvl);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.rto_initial);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.rto_min);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.rto_max);
#endif
	/* These two have end-to-end significance for connection indications and connection
	   responses */
	size += _T_SPACE_SIZEOF(t_defaults.sctp.ostreams);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.istreams);
#if 0
	size += _T_SPACE_SIZEOF(t_defaults.sctp.cookie_inc);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.throttle_itvl);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.mac_type);
#endif
	/* Checksum type could be negotiated.  In which case the checksum type needs to be
	   indicated because it has end-to-end significance. */
	size += _T_SPACE_SIZEOF(t_defaults.sctp.cksum_type);
#if 0
	size += _T_SPACE_SIZEOF(t_defaults.sctp.hb);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.rto);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.status);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.debug);
#endif
#if defined SCTP_CONFIG_ECN
	size += _T_SPACE_SIZEOF(t_defaults.sctp.ecn);
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	size += _T_SPACE_SIZEOF(t_defaults.sctp.ali);
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
	size += _T_SPACE_SIZEOF(t_defaults.sctp.add);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.set);
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if 0
	size += _T_SPACE_SIZEOF(t_defaults.sctp.add_ip);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.del_ip);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.set_ip);
#endif
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
	size += _T_SPACE_SIZEOF(t_defaults.sctp.pr);
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if 0
	size += _T_SPACE_SIZEOF(t_defaults.sctp.lifetime);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.disposition);
	size += _T_SPACE_SIZEOF(t_defaults.sctp.max_burst);
#endif
	return (size);
}

/*
 *  Size connection confirmation options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Return an option buffer size for a connection confirmation.  Options without end-to-end
 *  significance are only confirmed if requested.  Options with end-to-end significance are always
 *  confirmed. For more information on XTI connection confirmation handling, see t_connect(3).
 */
STATIC int
sctp_size_conn_con_opts(struct sctp *sp)
{
	int size = 0;

	if (t_tst_bit(_T_BIT_XTI_DEBUG, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.xti.debug);
	if (t_tst_bit(_T_BIT_XTI_LINGER, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.xti.linger);
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.xti.rcvbuf);
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.xti.rcvlowat);
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.xti.sndbuf);
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.xti.sndlowat);
	/* These two have end-to-end significance for connection indications and responses. */
#if 0
	if (t_tst_bit(_T_BIT_IP_OPTIONS, sp->options.flags))
#endif
		size += _T_SPACE_SIZEOF(sp->options.ip.options);
#if 0
	if (t_tst_bit(_T_BIT_IP_TOS, sp->options.flags))
#endif
		size += _T_SPACE_SIZEOF(sp->options.ip.tos);
	if (t_tst_bit(_T_BIT_IP_TTL, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.ip.ttl);
	if (t_tst_bit(_T_BIT_IP_REUSEADDR, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.ip.reuseaddr);
	if (t_tst_bit(_T_BIT_IP_DONTROUTE, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.ip.dontroute);
	if (t_tst_bit(_T_BIT_IP_BROADCAST, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.ip.broadcast);
	if (t_tst_bit(_T_BIT_IP_ADDR, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.ip.addr);
	if (t_tst_bit(_T_BIT_SCTP_NODELAY, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.nodelay);
	if (t_tst_bit(_T_BIT_SCTP_MAXSEG, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.maxseg);
	if (t_tst_bit(_T_BIT_SCTP_CORK, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.cork);
	if (t_tst_bit(_T_BIT_SCTP_PPI, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.ppi);
	if (t_tst_bit(_T_BIT_SCTP_SID, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.sid);
	if (t_tst_bit(_T_BIT_SCTP_SSN, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.ssn);
	if (t_tst_bit(_T_BIT_SCTP_TSN, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.tsn);
	if (t_tst_bit(_T_BIT_SCTP_RECVOPT, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.recvopt);
	if (t_tst_bit(_T_BIT_SCTP_COOKIE_LIFE, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.cookie_life);
	if (t_tst_bit(_T_BIT_SCTP_SACK_DELAY, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.sack_delay);
	if (t_tst_bit(_T_BIT_SCTP_PATH_MAX_RETRANS, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.path_max_retrans);
	if (t_tst_bit(_T_BIT_SCTP_ASSOC_MAX_RETRANS, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.assoc_max_retrans);
	if (t_tst_bit(_T_BIT_SCTP_MAX_INIT_RETRIES, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.max_init_retries);
	if (t_tst_bit(_T_BIT_SCTP_HEARTBEAT_ITVL, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.heartbeat_itvl);
	if (t_tst_bit(_T_BIT_SCTP_RTO_INITIAL, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.rto_initial);
	if (t_tst_bit(_T_BIT_SCTP_RTO_MIN, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.rto_min);
	if (t_tst_bit(_T_BIT_SCTP_RTO_MAX, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.rto_max);
	/* These two have end-to-end significance for connection indications and connection
	   responses */
#if 0
	if (t_tst_bit(_T_BIT_SCTP_OSTREAMS, sp->options.flags))
#endif
		size += _T_SPACE_SIZEOF(sp->options.sctp.ostreams);
#if 0
	if (t_tst_bit(_T_BIT_SCTP_ISTREAMS, sp->options.flags))
#endif
		size += _T_SPACE_SIZEOF(sp->options.sctp.istreams);
	if (t_tst_bit(_T_BIT_SCTP_COOKIE_INC, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.cookie_inc);
	if (t_tst_bit(_T_BIT_SCTP_THROTTLE_ITVL, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.throttle_itvl);
	if (t_tst_bit(_T_BIT_SCTP_MAC_TYPE, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.mac_type);
	if (t_tst_bit(_T_BIT_SCTP_CKSUM_TYPE, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.cksum_type);
	if (t_tst_bit(_T_BIT_SCTP_HB, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.hb);
	if (t_tst_bit(_T_BIT_SCTP_RTO, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.rto);
	if (t_tst_bit(_T_BIT_SCTP_STATUS, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.status);
	if (t_tst_bit(_T_BIT_SCTP_DEBUG, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.debug);
#if defined SCTP_CONFIG_ECN
	if (t_tst_bit(_T_BIT_SCTP_ECN, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.ecn);
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	if (t_tst_bit(_T_BIT_SCTP_ALI, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.ali);
#endif
#if defined SCTP_CONFIG_ADD_IP
	if (t_tst_bit(_T_BIT_SCTP_ADD, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.add);
	if (t_tst_bit(_T_BIT_SCTP_SET, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.set);
	if (t_tst_bit(_T_BIT_SCTP_ADD_IP, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.add_ip);
	if (t_tst_bit(_T_BIT_SCTP_DEL_IP, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.del_ip);
	if (t_tst_bit(_T_BIT_SCTP_SET_IP, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.set_ip);
#endif
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
	if (t_tst_bit(_T_BIT_SCTP_PR, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.pr);
#endif
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
	if (t_tst_bit(_T_BIT_SCTP_LIFETIME, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.lifetime);
#endif
	if (t_tst_bit(_T_BIT_SCTP_DISPOSITION, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.disposition);
	if (t_tst_bit(_T_BIT_SCTP_MAX_BURST, sp->options.flags))
		size += _T_SPACE_SIZEOF(sp->options.sctp.max_burst);
	return (size);
}

/*
 *  Size Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Check the validity of the options structure, check for correct size of each supplied option
 *  given the option management flag, and return the size required of the acknowledgement options
 *  field.
 */
STATIC int
t_size_default_options(const struct sctp *t, const unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (const unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t_defaults.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				olen += _T_SPACE_SIZEOF(t_defaults.ip.addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.nodelay);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CORK:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.cork);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_PPI:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.ppi);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SID:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.sid);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SSN:
				/* read only, can't get default */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_TSN:
				/* read only, can't get default */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RECVOPT:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.recvopt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_LIFE:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.cookie_life);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SACK_DELAY:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.sack_delay);
				if (ih->name != T_ALLOPT)
					continue;
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.sack_freq);
				if (ih->name != T_ALLOPT)
					continue;
#endif
			case T_SCTP_PATH_MAX_RETRANS:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.path_max_retrans);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ASSOC_MAX_RETRANS:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.assoc_max_retrans);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_INIT_RETRIES:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.max_init_retries);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HEARTBEAT_ITVL:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.heartbeat_itvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_INITIAL:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto_initial);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MIN:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto_min);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MAX:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto_max);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_OSTREAMS:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.ostreams);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ISTREAMS:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.istreams);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_INC:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.cookie_inc);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_THROTTLE_ITVL:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.throttle_itvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAC_TYPE:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.mac_type);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CKSUM_TYPE:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.cksum_type);
				if (ih->name != T_ALLOPT)
					continue;
#if defined SCTP_CONFIG_ECN
			case T_SCTP_ECN:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.ecn);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
			case T_SCTP_ALI:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.ali);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
			case T_SCTP_ADD:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.add);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.set);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ADD_IP:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.add_ip);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEL_IP:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.del_ip);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET_IP:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.set_ip);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_PR:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.pr);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_LIFETIME:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.lifetime);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
			case T_SCTP_DISPOSITION:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.disposition);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_BURST:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.max_burst);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HB:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.hb);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.rto);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAXSEG:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.maxseg);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_STATUS:
				/* read-only, no default */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEBUG:
				olen += _T_SPACE_SIZEOF(t_defaults.sctp.debug);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	sctplogno(t, "option output size = %u", olen);
	return (olen);
      einval:
	sctplogno(t, "invalid input options");
	return (-EINVAL);
}

STATIC int
t_size_current_options(const struct sctp *t, unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				olen += _T_SPACE_SIZEOF(t->options.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				olen += _T_SPACE_SIZEOF(t->options.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				olen += _T_SPACE_SIZEOF(t->options.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				olen += _T_SPACE_SIZEOF(t->options.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				olen += T_SPACE(0);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				olen += _T_SPACE_SIZEOF(t->options.ip.tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				olen += _T_SPACE_SIZEOF(t->options.ip.ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				olen += _T_SPACE_SIZEOF(t->options.ip.reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				olen += _T_SPACE_SIZEOF(t->options.ip.dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				olen += _T_SPACE_SIZEOF(t->options.ip.broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				olen += _T_SPACE_SIZEOF(t->options.ip.addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				olen += T_SPACE(0);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
				olen += _T_SPACE_SIZEOF(t->options.sctp.nodelay);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CORK:
				olen += _T_SPACE_SIZEOF(t->options.sctp.cork);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_PPI:
				olen += _T_SPACE_SIZEOF(t->options.sctp.ppi);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SID:
				olen += _T_SPACE_SIZEOF(t->options.sctp.sid);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SSN:
				olen += _T_SPACE_SIZEOF(t->options.sctp.ssn);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_TSN:
				olen += _T_SPACE_SIZEOF(t->options.sctp.tsn);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RECVOPT:
				olen += _T_SPACE_SIZEOF(t->options.sctp.recvopt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_LIFE:
				olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_life);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SACK_DELAY:
				olen += _T_SPACE_SIZEOF(t->options.sctp.sack_delay);
				if (ih->name != T_ALLOPT)
					continue;
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
				olen += _T_SPACE_SIZEOF(t->options.sctp.sack_freq);
				if (ih->name != T_ALLOPT)
					continue;
#endif
			case T_SCTP_PATH_MAX_RETRANS:
				olen += _T_SPACE_SIZEOF(t->options.sctp.path_max_retrans);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ASSOC_MAX_RETRANS:
				olen += _T_SPACE_SIZEOF(t->options.sctp.assoc_max_retrans);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_INIT_RETRIES:
				olen += _T_SPACE_SIZEOF(t->options.sctp.max_init_retries);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HEARTBEAT_ITVL:
				olen += _T_SPACE_SIZEOF(t->options.sctp.heartbeat_itvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_INITIAL:
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto_initial);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MIN:
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto_min);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MAX:
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto_max);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_OSTREAMS:
				olen += _T_SPACE_SIZEOF(t->options.sctp.ostreams);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ISTREAMS:
				olen += _T_SPACE_SIZEOF(t->options.sctp.istreams);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_INC:
				olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_inc);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_THROTTLE_ITVL:
				olen += _T_SPACE_SIZEOF(t->options.sctp.throttle_itvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAC_TYPE:
				olen += _T_SPACE_SIZEOF(t->options.sctp.mac_type);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CKSUM_TYPE:
				olen += _T_SPACE_SIZEOF(t->options.sctp.cksum_type);
				if (ih->name != T_ALLOPT)
					continue;
#if defined SCTP_CONFIG_ECN
			case T_SCTP_ECN:
				olen += _T_SPACE_SIZEOF(t->options.sctp.ecn);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
			case T_SCTP_ALI:
				olen += _T_SPACE_SIZEOF(t->options.sctp.ali);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
			case T_SCTP_ADD:
				olen += _T_SPACE_SIZEOF(t->options.sctp.add);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET:
				olen += _T_SPACE_SIZEOF(t->options.sctp.set);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ADD_IP:
				olen += _T_SPACE_SIZEOF(t->options.sctp.add_ip);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEL_IP:
				olen += _T_SPACE_SIZEOF(t->options.sctp.del_ip);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET_IP:
				olen += _T_SPACE_SIZEOF(t->options.sctp.set_ip);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_PR:
				olen += _T_SPACE_SIZEOF(t->options.sctp.pr);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_LIFETIME:
				olen += _T_SPACE_SIZEOF(t->options.sctp.lifetime);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
			case T_SCTP_DISPOSITION:
				olen += _T_SPACE_SIZEOF(t->options.sctp.disposition);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_BURST:
				olen += _T_SPACE_SIZEOF(t->options.sctp.max_burst);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HB:
				olen += _T_SPACE_SIZEOF(t->options.sctp.hb);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO:
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAXSEG:
				olen += _T_SPACE_SIZEOF(t->options.sctp.maxseg);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_STATUS:
				olen += _T_SPACE_SIZEOF(t->options.sctp.status);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEBUG:
				olen += _T_SPACE_SIZEOF(t->options.sctp.debug);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	sctplogno(t, "option output size = %u", olen);
	return (olen);
      einval:
	sctplogno(t, "invalid input options");
	return (-EINVAL);
}

STATIC int
t_size_check_options(const struct sctp *t, unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				/* can be any non-zero array of t_uscalar_t */
				if (optlen
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (optlen && optlen != sizeof(t->options.xti.linger))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (optlen && optlen != sizeof(t->options.xti.rcvbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (optlen && optlen != sizeof(t->options.xti.rcvlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (optlen && optlen != sizeof(t->options.xti.sndbuf))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (optlen && optlen != sizeof(t->options.xti.sndlowat))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				if (optlen && optlen != sizeof(t->options.ip.options))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				if (optlen && optlen != sizeof(t->options.ip.tos))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				if (optlen && optlen != sizeof(t->options.ip.ttl))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				if (optlen && optlen != sizeof(t->options.ip.reuseaddr))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				if (optlen && optlen != sizeof(t->options.ip.dontroute))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				if (optlen && optlen != sizeof(t->options.ip.broadcast))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				if (optlen && optlen != sizeof(t->options.ip.addr))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
				if (optlen && optlen != sizeof(t->options.sctp.nodelay))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CORK:
				if (optlen && optlen != sizeof(t->options.sctp.cork))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_PPI:
				if (optlen && optlen != sizeof(t->options.sctp.ppi))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SID:
				if (optlen && optlen != sizeof(t->options.sctp.sid))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SSN:
				if (optlen && optlen != sizeof(t->options.sctp.ssn))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_TSN:
				if (optlen && optlen != sizeof(t->options.sctp.tsn))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RECVOPT:
				if (optlen && optlen != sizeof(t->options.sctp.recvopt))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_LIFE:
				if (optlen && optlen != sizeof(t->options.sctp.cookie_life))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SACK_DELAY:
				if (optlen && optlen != sizeof(t->options.sctp.sack_delay))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
				if (optlen && optlen != sizeof(t->options.sctp.sack_freq))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#endif
			case T_SCTP_PATH_MAX_RETRANS:
				if (optlen && optlen != sizeof(t->options.sctp.path_max_retrans))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ASSOC_MAX_RETRANS:
				if (optlen && optlen != sizeof(t->options.sctp.assoc_max_retrans))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_INIT_RETRIES:
				if (optlen && optlen != sizeof(t->options.sctp.max_init_retries))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HEARTBEAT_ITVL:
				if (optlen && optlen != sizeof(t->options.sctp.heartbeat_itvl))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_INITIAL:
				if (optlen && optlen != sizeof(t->options.sctp.rto_initial))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MIN:
				if (optlen && optlen != sizeof(t->options.sctp.rto_min))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MAX:
				if (optlen && optlen != sizeof(t->options.sctp.rto_max))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_OSTREAMS:
				if (optlen && optlen != sizeof(t->options.sctp.ostreams))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ISTREAMS:
				if (optlen && optlen != sizeof(t->options.sctp.istreams))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_INC:
				if (optlen && optlen != sizeof(t->options.sctp.cookie_inc))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_THROTTLE_ITVL:
				if (optlen && optlen != sizeof(t->options.sctp.throttle_itvl))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAC_TYPE:
				if (optlen && optlen != sizeof(t->options.sctp.mac_type))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CKSUM_TYPE:
				if (optlen && optlen != sizeof(t->options.sctp.cksum_type))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#if defined SCTP_CONFIG_ECN
			case T_SCTP_ECN:
				if (optlen && optlen != sizeof(t->options.sctp.ecn))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
			case T_SCTP_ALI:
				if (optlen && optlen != sizeof(t->options.sctp.ali))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
			case T_SCTP_ADD:
				if (optlen && optlen != sizeof(t->options.sctp.add))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET:
				if (optlen && optlen != sizeof(t->options.sctp.set))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ADD_IP:
				if (optlen && optlen != sizeof(t->options.sctp.add_ip))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEL_IP:
				if (optlen && optlen != sizeof(t->options.sctp.del_ip))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET_IP:
				if (optlen && optlen != sizeof(t->options.sctp.set_ip))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_PR:
				if (optlen && optlen != sizeof(t->options.sctp.pr))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_LIFETIME:
				if (optlen && optlen != sizeof(t->options.sctp.lifetime))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
			case T_SCTP_DISPOSITION:
				if (optlen && optlen != sizeof(t->options.sctp.disposition))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_BURST:
				if (optlen && optlen != sizeof(t->options.sctp.max_burst))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HB:
				if (optlen && optlen != sizeof(t->options.sctp.hb))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO:
				if (optlen && optlen != sizeof(t->options.sctp.rto))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAXSEG:
				if (optlen && optlen != sizeof(t->options.sctp.maxseg))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_STATUS:
				/* read-only */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEBUG:
				if (optlen && optlen != sizeof(t->options.sctp.debug))
					goto einval;
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	sctplogno(t, "option output size = %u", olen);
	return (olen);
      einval:
	sctplogno(t, "invalid input options");
	return (-EINVAL);
}

STATIC int
t_size_negotiate_options(const struct sctp *t, unsigned char *ip, size_t ilen)
{
	int olen = 0, optlen;
	const struct t_opthdr *ih;
	struct t_opthdr all;

	if (ip == NULL || ilen == 0) {
		/* For zero-length options fake an option header for all names with all levels */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0); ih; ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0)) {
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				if (ih->name != T_ALLOPT
				    && ((optlen % sizeof(t_uscalar_t)) != 0
					|| optlen > 4 * sizeof(t_uscalar_t)))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.debug);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_LINGER:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.linger))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.linger);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.rcvbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_RCVLOWAT:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.xti.rcvlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.rcvlowat);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDBUF:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.xti.sndbuf))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.sndbuf);
				if (ih->name != T_ALLOPT)
					continue;
			case XTI_SNDLOWAT:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.xti.sndlowat))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.xti.sndlowat);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_IP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
				   T_READONLY, the returned option value is the same as the one
				   requested on input. */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TOS:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.ip.tos))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.tos);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_TTL:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.ip.ttl))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.ttl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_REUSEADDR:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.ip.reuseaddr))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.reuseaddr);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_DONTROUTE:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.ip.dontroute))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.dontroute);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_BROADCAST:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.ip.broadcast))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.broadcast);
				if (ih->name != T_ALLOPT)
					continue;
			case T_IP_ADDR:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.ip.addr))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.ip.addr);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				olen += T_SPACE(optlen);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.nodelay))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.nodelay);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CORK:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.cork))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.cork);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_PPI:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.ppi))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.ppi);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SID:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.sid))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.sid);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SSN:
				/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
				   T_READONLY, the returned option value is the same as the one
				   requested on input. */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_TSN:
				/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
				   T_READONLY, the returned option value is the same as the one
				   requested on input. */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RECVOPT:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.recvopt))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.recvopt);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_LIFE:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.cookie_life))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_life);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SACK_DELAY:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.sack_delay))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.sack_delay);
				if (ih->name != T_ALLOPT)
					continue;
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.sack_freq))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.sack_freq);
				if (ih->name != T_ALLOPT)
					continue;
#endif
			case T_SCTP_PATH_MAX_RETRANS:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.path_max_retrans))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.path_max_retrans);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ASSOC_MAX_RETRANS:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.assoc_max_retrans))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.assoc_max_retrans);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_INIT_RETRIES:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.max_init_retries))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.max_init_retries);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HEARTBEAT_ITVL:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.heartbeat_itvl))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.heartbeat_itvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_INITIAL:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.rto_initial))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto_initial);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MIN:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.rto_min))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto_min);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO_MAX:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.rto_max))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto_max);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_OSTREAMS:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.ostreams))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.ostreams);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ISTREAMS:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.istreams))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.istreams);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_COOKIE_INC:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.cookie_inc))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.cookie_inc);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_THROTTLE_ITVL:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.throttle_itvl))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.throttle_itvl);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAC_TYPE:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.mac_type))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.mac_type);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_CKSUM_TYPE:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.cksum_type))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.cksum_type);
				if (ih->name != T_ALLOPT)
					continue;
#if defined SCTP_CONFIG_ECN
			case T_SCTP_ECN:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.ecn))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.ecn);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
			case T_SCTP_ALI:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.ali))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.ali);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
			case T_SCTP_ADD:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.add))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.add);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.set))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.set);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_ADD_IP:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.add_ip))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.add_ip);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEL_IP:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.del_ip))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.del_ip);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_SET_IP:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.set_ip))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.set_ip);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_PR:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.pr))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.pr);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
			case T_SCTP_LIFETIME:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.lifetime))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.lifetime);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
			case T_SCTP_DISPOSITION:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.disposition))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.disposition);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAX_BURST:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.max_burst))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.max_burst);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_HB:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.hb))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.hb);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_RTO:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.rto))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.rto);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_MAXSEG:
				if (ih->name != T_ALLOPT
				    && optlen != sizeof(t->options.sctp.maxseg))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.maxseg);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_STATUS:
				/* If the status is T_SUCCESS, T_FAILURE, T_NOTSUPPORT or
				   T_READONLY, the returned option value is the same as the one
				   requested on input. */
				olen += T_SPACE(optlen);
				if (ih->name != T_ALLOPT)
					continue;
			case T_SCTP_DEBUG:
				if (ih->name != T_ALLOPT && optlen != sizeof(t->options.sctp.debug))
					goto einval;
				olen += _T_SPACE_SIZEOF(t->options.sctp.debug);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	sctplogno(t, "option output size = %u", olen);
	return (olen);
      einval:
	sctplogno(t, "invalid input options");
	return (-EINVAL);
}

/*
 *  Overall Option Result
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Calculates the overall T_OPTMGMT_ACK flag result from individual results.
 */
STATIC t_scalar_t
t_overall_result(t_scalar_t *overall, t_scalar_t result)
{
	switch (result) {
	case T_NOTSUPPORT:
		if (!(*overall & (T_NOTSUPPORT)))
			*overall = T_NOTSUPPORT;
		break;
	case T_READONLY:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY)))
			*overall = T_READONLY;
		break;
	case T_FAILURE:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE)))
			*overall = T_FAILURE;
		break;
	case T_PARTSUCCESS:
		if (!(*overall & (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS)))
			*overall = T_PARTSUCCESS;
		break;
	case T_SUCCESS:
		if (!
		    (*overall &
		     (T_NOTSUPPORT | T_READONLY | T_FAILURE | T_PARTSUCCESS | T_SUCCESS)))
			*overall = T_SUCCESS;
		break;
	}
	return (result);
}

/*
 *  Build connection indication options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  With connection indications, according to XTI spec, we only deliver up options that are of
 *  end-to-end significance.  For SCTP, there are only 3 or 4 options of end-to-end significance:
 *  T_IP_OPTIONS, T_IP_TOS, T_SCTP_ISTREAMS and T_SCTP_OSTREAMS.  Additional options to consider are
 *  Payload Protocol Identifier, Stream Identifier (if there is any data).  If we support Explicit
 *  Congestion Notification, Application Layer Information, Partial Reliability and Add IP, then
 *  there are additional association level end-to-end options to consider.
 *
 *  The options delivered on connection indication are not options associated with the listening
 *  stream, but are options associated only with the connection indication.  Because of this, it is
 *  necessary to obtain the options from the connection indication itself.
 */
STATIC void
sctp_build_conn_ind_opts(struct sctp *sp, unsigned char *op, size_t olen, struct sctp_cookie *ck)
{
	struct t_opthdr *oh;

	if (op == NULL || olen == 0)
		return;
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	{
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->len = _T_LENGTH_SIZEOF(sp->options.ip.options);
		oh->status = T_SUCCESS;
		/* get options from connection indication */
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->len = _T_LENGTH_SIZEOF(sp->options.ip.tos);
		oh->status = T_SUCCESS;
		/* get options from connection indication */
		*((unsigned char *) T_OPT_DATA(oh)) = sp->options.ip.tos;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.ppi;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.sid;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SSN;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.ssn;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_TSN;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.tsn;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* get options from connection indication */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.ostreams;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* get options from connection indication */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.istreams;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if defined SCTP_CONFIG_ECN
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ECN;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) =
		    (ck->p_caps & ck->l_caps & SCTP_CAPS_ECN) ? T_YES : T_NO;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ALI;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) =
		    (ck->p_caps & ck->l_caps & SCTP_CAPS_ALI) ? ck->p_ali : T_UNSPEC;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ADD;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) =
		    (ck->p_caps & ck->l_caps & SCTP_CAPS_ADD_IP) ? T_YES : T_NO;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SET;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) =
		    (ck->p_caps & ck->l_caps & SCTP_CAPS_SET_IP) ? T_YES : T_NO;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PR;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) =
		    (ck->p_caps & ck->l_caps & SCTP_CAPS_PR) ? T_YES : T_NO;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
}

/*
 *  Build connection confirmation options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  These are options with end-to-end significance plus any options without end-to-end significance
 *  that were requested for negotiation in the connection request.  For a connection indication,
 *  this is only options with end-to-end significance.  For this to work with connection
 *  indications, all options request flags must be set to zero.  The retrn value is the resulting
 *  size of the options buffer, or a negative error number on software fault.
 *
 *  The t_connect() or t_rcvconnect() functions return the values of all options with end-to-end
 *  significance that were received with the connection response and the negotiated values of those
 *  options without end-to-end significance that had been specified on input.  However, options
 *  specified on input with t_connect() call that are not supported or refer to an unknown option
 *  level are discarded and not returned on output.
 *
 *  The status field of each option returned with t_connect() or t_rcvconnect() indicates if the
 *  proposed value (T_SUCCESS) or a degraded value (T_PARTSUCCESS) has been negotiated.  The status
 *  field of received ancillary information (for example, T_IP options) that is not subject to
 *  negotiation is always set to T_SUCCESS.
 */
STATIC int
t_build_conn_opts(struct sctp *sp, unsigned char *op, size_t olen)
{
	struct inet_opt *np = &sp->inet;
	struct t_opthdr *oh;

	if (op == NULL || olen == 0)
		return (0);
	oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
	if (t_tst_bit(_T_BIT_XTI_DEBUG, sp->options.flags)) {
		oh->level = XTI_GENERIC;
		oh->name = XTI_DEBUG;
		oh->len = _T_LENGTH_SIZEOF(sp->options.xti.debug);
		oh->status = T_SUCCESS;
		bcopy(sp->options.xti.debug, T_OPT_DATA(oh), sizeof(sp->options.xti.debug));
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_LINGER, sp->options.flags)) {
		oh->level = XTI_GENERIC;
		oh->name = XTI_LINGER;
		oh->len = _T_LENGTH_SIZEOF(sp->options.xti.linger);
		oh->status = T_SUCCESS;
		if ((sp->linger == 1) != (sp->options.xti.linger.l_onoff == T_YES)) {
			sp->options.xti.linger.l_onoff = sp->linger ? T_YES : T_NO;
		}
		if (sp->options.xti.linger.l_onoff == T_YES) {
			if (sp->options.xti.linger.l_linger != sp->lingertime / HZ) {
				if (sp->options.xti.linger.l_linger != T_UNSPEC
				    && sp->options.xti.linger.l_linger < sp->lingertime / HZ)
					oh->status = T_PARTSUCCESS;
				sp->options.xti.linger.l_linger = sp->lingertime / HZ;
			}
		} else
			sp->options.xti.linger.l_linger = T_UNSPEC;
		*((struct t_linger *) T_OPT_DATA(oh)) = sp->options.xti.linger;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVBUF, sp->options.flags)) {
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVBUF;
		oh->len = _T_LENGTH_SIZEOF(sp->options.xti.rcvbuf);
		oh->status = T_SUCCESS;
		if (sp->options.xti.rcvbuf != sp->rcvbuf / 2) {
			if (sp->options.xti.rcvbuf != T_UNSPEC
			    && sp->options.xti.rcvbuf < sp->rcvbuf / 2)
				oh->status = T_PARTSUCCESS;
			sp->options.xti.rcvbuf = sp->rcvbuf / 2;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.xti.rcvbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_RCVLOWAT, sp->options.flags)) {
		oh->level = XTI_GENERIC;
		oh->name = XTI_RCVLOWAT;
		oh->len = _T_LENGTH_SIZEOF(sp->options.xti.rcvlowat);
		oh->status = T_SUCCESS;
		if (sp->options.xti.rcvlowat != sp->rcvlowat) {
			if (sp->options.xti.rcvlowat != T_UNSPEC
			    && sp->options.xti.rcvlowat < sp->rcvlowat)
				oh->status = T_PARTSUCCESS;
			sp->options.xti.rcvlowat = sp->rcvlowat;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.xti.rcvlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDBUF, sp->options.flags)) {
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDBUF;
		oh->len = _T_LENGTH_SIZEOF(sp->options.xti.sndbuf);
		oh->status = T_SUCCESS;
		if (sp->options.xti.sndbuf != sp->sndbuf / 2) {
			if (sp->options.xti.sndbuf != T_UNSPEC
			    && sp->options.xti.sndbuf < sp->sndbuf / 2)
				oh->status = T_PARTSUCCESS;
			sp->options.xti.sndbuf = sp->sndbuf / 2;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.xti.sndbuf;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_XTI_SNDLOWAT, sp->options.flags)) {
		oh->level = XTI_GENERIC;
		oh->name = XTI_SNDLOWAT;
		oh->len = _T_LENGTH_SIZEOF(sp->options.xti.sndlowat);
		oh->status = T_SUCCESS;
		if (sp->options.xti.sndlowat != 1) {
			if (sp->options.xti.sndlowat != T_UNSPEC && sp->options.xti.sndlowat < 1)
				oh->status = T_PARTSUCCESS;
			sp->options.xti.sndlowat = 1;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndlowat;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if 0
	if (t_tst_bit(_T_BIT_IP_OPTIONS, sp->options.flags))
#endif
	{
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->len = _T_LENGTH_SIZEOF(sp->options.ip.options);
		oh->status = T_SUCCESS;
		/* get options from stream */
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if 0
	if (t_tst_bit(_T_BIT_IP_TOS, sp->options.flags))
#endif
	{
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->len = _T_LENGTH_SIZEOF(sp->options.ip.tos);
		oh->status = T_SUCCESS;
		/* get options from stream */
		if (sp->options.ip.tos != np->tos) {
			if (sp->options.ip.tos > np->tos)
				oh->status = T_PARTSUCCESS;
			sp->options.ip.tos = np->tos;
		}
		*((unsigned char *) T_OPT_DATA(oh)) = sp->options.ip.tos;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_IP_TTL, sp->options.flags)) {
		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->len = _T_LENGTH_SIZEOF(sp->options.ip.ttl);
		oh->status = T_SUCCESS;
#if 0
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
		if (sp->options.ip.ttl != np->ttl) {
			if (sp->options.ip.ttl > np->ttl)
				oh->status = T_PARTSUCCESS;
			sp->options.ip.ttl = np->ttl;
		}
#else
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
		if (sp->options.ip.ttl != np->uc_ttl) {
			if (sp->options.ip.ttl > np->uc_ttl)
				oh->status = T_PARTSUCCESS;
			sp->options.ip.ttl = np->uc_ttl;
		}
#endif
#endif				/* HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
#else
		if (sp->options.ip.ttl != np->uc_ttl) {
			if (sp->options.ip.ttl > np->uc_ttl)
				oh->status = T_PARTSUCCESS;
			sp->options.ip.ttl = np->uc_ttl;
		}
#endif
		*((unsigned char *) T_OPT_DATA(oh)) = sp->options.ip.ttl;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_IP_REUSEADDR, sp->options.flags)) {
		oh->level = T_INET_IP;
		oh->name = T_IP_REUSEADDR;
		oh->len = _T_LENGTH_SIZEOF(unsigned int);

		oh->status = T_SUCCESS;
		if ((sp->options.ip.reuseaddr == T_NO) != (sp->reuse == 0)) {
			oh->status = T_PARTSUCCESS;
			sp->options.ip.reuseaddr = sp->reuse ? T_YES : T_NO;
		}
		*((unsigned int *) T_OPT_DATA(oh)) = sp->options.ip.reuseaddr;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_IP_DONTROUTE, sp->options.flags)) {
		oh->level = T_INET_IP;
		oh->name = T_IP_DONTROUTE;
		oh->len = _T_LENGTH_SIZEOF(unsigned int);

		oh->status = T_SUCCESS;
		if ((sp->options.ip.dontroute == T_NO) != (sp->localroute == 0)) {
			oh->status = T_PARTSUCCESS;
			sp->options.ip.dontroute = sp->localroute ? T_YES : T_NO;
		}
		*((unsigned int *) T_OPT_DATA(oh)) = sp->options.ip.dontroute;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_IP_BROADCAST, sp->options.flags)) {
		oh->level = T_INET_IP;
		oh->name = T_IP_BROADCAST;
		oh->len = _T_LENGTH_SIZEOF(unsigned int);

		oh->status = T_SUCCESS;
		if ((sp->options.ip.broadcast == T_NO) != (sp->broadcast == 0)) {
			oh->status = T_PARTSUCCESS;
			sp->options.ip.broadcast = sp->broadcast ? T_YES : T_NO;
		}
		*((unsigned int *) T_OPT_DATA(oh)) = sp->options.ip.broadcast;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_IP_ADDR, sp->options.flags)) {
		oh->level = T_INET_IP;
		oh->name = T_IP_ADDR;
		oh->len = _T_LENGTH_SIZEOF(uint32_t);

		oh->status = T_SUCCESS;
		*((uint32_t *) T_OPT_DATA(oh)) = sp->options.ip.addr;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_NODELAY, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_NODELAY;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if ((sp->options.sctp.nodelay != T_NO) != (sp->nonagle != 0)) {
			oh->status = T_PARTSUCCESS;
			sp->options.sctp.nodelay = sp->nonagle ? T_YES : T_NO;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.nodelay;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_MAXSEG, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAXSEG;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.maxseg != sp->user_amps) {
			if (sp->options.sctp.maxseg > sp->user_amps)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.maxseg = sp->user_amps;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.maxseg;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_CORK, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CORK;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.cork;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_PPI, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		sp->options.sctp.ppi = sp->ppi;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.ppi;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_SID, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		sp->options.sctp.sid = sp->sid;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.sid;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_SSN, sp->options.flags)) {
		/* only per-packet */
	}
	if (t_tst_bit(_T_BIT_SCTP_TSN, sp->options.flags)) {
		/* only per-packet */
	}
	if (t_tst_bit(_T_BIT_SCTP_RECVOPT, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RECVOPT;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.recvopt;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_COOKIE_LIFE, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_LIFE;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.cookie_life != sp->ck_life / HZ * 1000) {
			if (sp->options.sctp.cookie_life < sp->ck_life / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.cookie_life = sp->ck_life / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.cookie_life;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_SACK_DELAY, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_DELAY;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.sack_delay != sp->max_sack / HZ * 1000) {
			if (sp->options.sctp.sack_delay < sp->max_sack / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.sack_delay = sp->max_sack / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.sack_delay;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#ifdef ETSI
	if (t_tst_bit(_T_BIT_SCTP_SACK_FREQUENCY, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_FREQUENCY;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.sack_freq != sp->sack_freq) {
			if (sp->options.sctp.sack_freq > 5) {
				oh->status = T_PARTSUCCESS;
				sp->options.sctp.sack_freq = 5;
			}
			if (sp->options.sctp.sack_freq < 1)
				sp->options.sctp.sack_freq = 1;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.sack_delay;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif
	if (t_tst_bit(_T_BIT_SCTP_PATH_MAX_RETRANS, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PATH_MAX_RETRANS;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.path_max_retrans;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_ASSOC_MAX_RETRANS, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ASSOC_MAX_RETRANS;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.assoc_max_retrans;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_MAX_INIT_RETRIES, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_INIT_RETRIES;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.max_init_retries;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_HEARTBEAT_ITVL, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HEARTBEAT_ITVL;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.heartbeat_itvl != sp->hb_itvl / HZ * 1000) {
			if (sp->options.sctp.heartbeat_itvl < sp->hb_itvl / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.heartbeat_itvl = sp->hb_itvl / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.heartbeat_itvl;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_RTO_INITIAL, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_INITIAL;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.rto_initial != sp->rto_ini / HZ * 1000) {
			if (sp->options.sctp.rto_initial < sp->rto_ini / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.rto_initial = sp->rto_ini / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.rto_initial;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_RTO_MIN, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MIN;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.rto_min != sp->rto_min / HZ * 1000) {
			if (sp->options.sctp.rto_min < sp->rto_min / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.rto_min = sp->rto_min / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.rto_min;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_RTO_MAX, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MAX;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.rto_max != sp->rto_max / HZ * 1000) {
			if (sp->options.sctp.rto_max < sp->rto_max / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.rto_max = sp->rto_max / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.rto_max;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if 0
	if (t_tst_bit(_T_BIT_SCTP_OSTREAMS, sp->options.flags))
#endif
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* get options from stream */
		if (sp->options.sctp.ostreams != sp->req_ostr) {
			if (sp->options.sctp.ostreams > sp->req_ostr)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.ostreams = sp->req_ostr;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.ostreams;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if 0
	if (t_tst_bit(_T_BIT_SCTP_ISTREAMS, sp->options.flags))
#endif
	{
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* get options from stream */
		if (sp->options.sctp.istreams != sp->max_istr) {
			if (sp->options.sctp.istreams > sp->max_istr)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.istreams = sp->max_istr;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.istreams;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_COOKIE_INC, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_INC;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.cookie_inc != sp->ck_inc / HZ * 1000) {
			if (sp->options.sctp.cookie_inc < sp->ck_inc / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.cookie_inc = sp->ck_inc / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.cookie_inc;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_THROTTLE_ITVL, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_THROTTLE_ITVL;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.throttle_itvl != sp->throttle / HZ * 1000) {
			if (sp->options.sctp.throttle_itvl < sp->throttle / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.throttle_itvl = sp->throttle / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.throttle_itvl;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_MAC_TYPE, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAC_TYPE;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.mac_type;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_CKSUM_TYPE, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CKSUM_TYPE;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.cksum_type;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_HB, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HB;
		oh->len = _T_LENGTH_SIZEOF(struct t_sctp_hb);

		oh->status = T_SUCCESS;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_RTO, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO;
		oh->len = _T_LENGTH_SIZEOF(struct t_sctp_rto);

		oh->status = T_SUCCESS;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_STATUS, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_STATUS;
		oh->len = T_SPACE(sizeof(struct t_sctp_status) + sizeof(struct t_sctp_dest_status));
		oh->status = T_SUCCESS;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_DEBUG, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEBUG;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.debug;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#if defined SCTP_CONFIG_ECN
	if (t_tst_bit(_T_BIT_SCTP_ECN, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ECN;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.ecn;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	if (t_tst_bit(_T_BIT_SCTP_ALI, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ALI;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.ali;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
	if (t_tst_bit(_T_BIT_SCTP_ADD, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ADD;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.add;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_SET, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SET;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.set;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_ADD_IP, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ADD_IP;
		oh->len = _T_LENGTH_SIZEOF(struct sockaddr_in);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((struct sockaddr_in *) T_OPT_DATA(oh)) = sp->options.sctp.add_ip;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_DEL_IP, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEL_IP;
		oh->len = _T_LENGTH_SIZEOF(struct sockaddr_in);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((struct sockaddr_in *) T_OPT_DATA(oh)) = sp->options.sctp.del_ip;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_SET_IP, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SET_IP;
		oh->len = _T_LENGTH_SIZEOF(struct sockaddr_in);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((struct sockaddr_in *) T_OPT_DATA(oh)) = sp->options.sctp.set_ip;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
	if (t_tst_bit(_T_BIT_SCTP_PR, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PR;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.pr;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
	if (t_tst_bit(_T_BIT_SCTP_LIFETIME, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_LIFETIME;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		if (sp->options.sctp.lifetime != sp->life / HZ * 1000) {
			if (sp->options.sctp.lifetime < sp->life / HZ * 1000)
				oh->status = T_PARTSUCCESS;
			sp->options.sctp.lifetime = sp->life / HZ * 1000;
		}
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.lifetime;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
	if (t_tst_bit(_T_BIT_SCTP_DISPOSITION, sp->options.flags)) {
		/* FIXME: this should probably be read-only */
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DISPOSITION;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.disposition;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	if (t_tst_bit(_T_BIT_SCTP_MAX_BURST, sp->options.flags)) {
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_BURST;
		oh->len = _T_LENGTH_SIZEOF(t_uscalar_t);

		oh->status = T_SUCCESS;
		/* absolute requirement */
		*((t_uscalar_t *) T_OPT_DATA(oh)) = sp->options.sctp.max_burst;
		oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0);
	}
	assure(oh == NULL);
	return (olen);
	// return ((unsigned char *) oh - op); /* return actual length */
}

/*
 *  Default Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT placing the output in the provided buffer.
 */
STATIC t_scalar_t
t_build_default_options(const struct sctp *t, const unsigned char *ip, size_t ilen,
			unsigned char *op, size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t_defaults.xti.debug, T_OPT_DATA(oh),
				      sizeof(t_defaults.xti.debug));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				*((struct t_linger *) T_OPT_DATA(oh)) = t_defaults.xti.linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.rcvbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.xti.sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				oh->len = sizeof(*oh);
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				/* not supported yet */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.tos);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.tos;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.ttl);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				*((unsigned char *) T_OPT_DATA(oh)) = t_defaults.ip.ttl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.reuseaddr);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.reuseaddr;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.dontroute);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.dontroute;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.broadcast);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				*((unsigned int *) T_OPT_DATA(oh)) = t_defaults.ip.broadcast;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.ip.addr);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				*((uint32_t *) T_OPT_DATA(oh)) = t_defaults.ip.addr;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.nodelay);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_NODELAY;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.nodelay;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAXSEG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.maxseg);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAXSEG;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.maxseg;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_CORK:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cork);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CORK;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.cork;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_PPI:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ppi);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.ppi;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SID:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.sid);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.sid;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SSN:
				oh->len = sizeof(*oh);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_TSN:
				oh->len = sizeof(*oh);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_TSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RECVOPT:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.recvopt);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RECVOPT;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.recvopt;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_COOKIE_LIFE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cookie_life);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_LIFE;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.cookie_life;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SACK_DELAY:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.sack_delay);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_DELAY;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.sack_delay;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.sack_freq);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_FREQUENCY;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.sack_freq;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
#endif
			case T_SCTP_PATH_MAX_RETRANS:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.path_max_retrans);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PATH_MAX_RETRANS;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    t_defaults.sctp.path_max_retrans;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ASSOC_MAX_RETRANS:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.assoc_max_retrans);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ASSOC_MAX_RETRANS;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    t_defaults.sctp.assoc_max_retrans;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAX_INIT_RETRIES:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.max_init_retries);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_INIT_RETRIES;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    t_defaults.sctp.max_init_retries;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_HEARTBEAT_ITVL:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.heartbeat_itvl);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HEARTBEAT_ITVL;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.heartbeat_itvl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_INITIAL:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto_initial);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_INITIAL;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.rto_initial;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_MIN:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto_min);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MIN;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.rto_min;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_MAX:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto_max);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MAX;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.rto_max;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_OSTREAMS:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ostreams);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_OSTREAMS;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.ostreams;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ISTREAMS:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.istreams);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ISTREAMS;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.istreams;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_COOKIE_INC:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cookie_inc);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_INC;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.cookie_inc;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_THROTTLE_ITVL:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.throttle_itvl);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_THROTTLE_ITVL;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.throttle_itvl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAC_TYPE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.mac_type);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAC_TYPE;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.mac_type;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_CKSUM_TYPE:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.cksum_type);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CKSUM_TYPE;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.cksum_type;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_HB:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.hb);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HB;
				oh->status = T_SUCCESS;
				*((struct t_sctp_hb *) T_OPT_DATA(oh)) = t_defaults.sctp.hb;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.rto);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO;
				oh->status = T_SUCCESS;
				*((struct t_sctp_rto *) T_OPT_DATA(oh)) = t_defaults.sctp.rto;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_STATUS:
				/* read-only */
				oh->len = sizeof(*oh);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_STATUS;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.debug);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEBUG;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.debug;
				if (ih->name != T_ALLOPT)
					continue;
#if defined SCTP_CONFIG_ECN
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ECN:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ECN;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ecn);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.ecn;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ALI:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ALI;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.ali);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.ali;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ADD:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.add);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.add;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SET:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.set);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.set;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ADD_IP:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD_IP;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.add_ip);
				oh->status = T_SUCCESS;
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = t_defaults.sctp.add_ip;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DEL_IP:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEL_IP;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.del_ip);
				oh->status = T_SUCCESS;
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = t_defaults.sctp.del_ip;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SET_IP:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET_IP;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.set_ip);
				oh->status = T_SUCCESS;
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = t_defaults.sctp.set_ip;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_PR:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PR;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.pr);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.pr;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_LIFETIME:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_LIFETIME;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.lifetime);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.lifetime;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DISPOSITION:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DISPOSITION;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.disposition);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.disposition;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAX_BURST:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_BURST;
				oh->len = _T_LENGTH_SIZEOF(t_defaults.sctp.max_burst);
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t_defaults.sctp.max_burst;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	sctplogerr(t, "%s() fault", __FUNCTION__);
	return (-EFAULT);
}

/*
 *  Current Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_CURRENT placing the output in the
 *  provided buffer.
 */
STATIC t_scalar_t
t_build_current_options(const struct sctp *t, const unsigned char *ip, size_t ilen,
			unsigned char *op, size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.debug);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(t->options.xti.debug, T_OPT_DATA(oh),
				      4 * sizeof(t_uscalar_t));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.linger);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_linger *) T_OPT_DATA(oh)) = t->options.xti.linger;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.rcvbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.rcvbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.rcvlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.rcvlowat;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.sndbuf);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.sndbuf;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = _T_LENGTH_SIZEOF(t->options.xti.sndlowat);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.xti.sndlowat;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				oh->len = sizeof(*oh);
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				/* not supported yet */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.tos);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned char *) T_OPT_DATA(oh)) = t->options.ip.tos;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.ttl);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned char *) T_OPT_DATA(oh)) = t->options.ip.ttl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.reuseaddr);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = t->options.ip.reuseaddr;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.dontroute);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = t->options.ip.dontroute;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.broadcast);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((unsigned int *) T_OPT_DATA(oh)) = t->options.ip.broadcast;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = _T_LENGTH_SIZEOF(t->options.ip.addr);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((uint32_t *) T_OPT_DATA(oh)) = t->options.ip.addr;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
#if 0
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				oh->len = _T_LENGTH_SIZEOF(t->options.udp.checksum);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.udp.checksum;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		case T_INET_TCP:
			if (t->p.prot.family == PF_INET && t->p.prot.protocol == T_INET_TCP) {
				struct tcp_opt *tp = tcp_sk(sk);

				(void) tp;
				switch (ih->name) {
				default:
					oh->len = sizeof(*oh);
					oh->level = ih->level;
					oh->name = ih->name;
					oh->status = t_overall_result(&overall, T_NOTSUPPORT);
					continue;
				case T_ALLOPT:
				case T_TCP_NODELAY:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.nodelay);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_NODELAY;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.nodelay;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_MAXSEG:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.maxseg);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_MAXSEG;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.maxseg;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPALIVE:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepalive);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPALIVE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct t_kpalive *) T_OPT_DATA(oh)) =
					    t->options.tcp.keepalive;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_CORK:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.cork);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_CORK;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.cork;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPIDLE:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepidle);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPIDLE;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.keepidle;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPINTVL:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepitvl);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPINTVL;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.keepitvl;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_KEEPCNT:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.keepcnt);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_KEEPCNT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.keepcnt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_SYNCNT:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.syncnt);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_SYNCNT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.syncnt;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_LINGER2:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.linger2);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_LINGER2;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.linger2;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_DEFER_ACCEPT:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.defer_accept);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_DEFER_ACCEPT;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.tcp.defer_accept;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_WINDOW_CLAMP:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.window_clamp);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_WINDOW_CLAMP;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) =
					    t->options.tcp.window_clamp;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_INFO:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.info);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_INFO;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((struct t_tcp_info *) T_OPT_DATA(oh)) =
					    t->options.tcp.info;
					if (ih->name != T_ALLOPT)
						continue;
					if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
						goto efault;
				case T_TCP_QUICKACK:
					oh->len = _T_LENGTH_SIZEOF(t->options.tcp.quickack);
					oh->level = T_INET_TCP;
					oh->name = T_TCP_QUICKACK;
					oh->status = T_SUCCESS;
					/* refresh current value */
					*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.tcp.quickack;
					if (ih->name != T_ALLOPT)
						continue;
				}
				if (ih->level != T_ALLLEVELS)
					continue;
			} else {
				if (ih->level != T_ALLLEVELS)
					goto einval;
			}
#endif
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				oh->len = sizeof(*oh);
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.nodelay);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_NODELAY;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.nodelay;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAXSEG:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.maxseg);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAXSEG;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.maxseg;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_CORK:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cork);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CORK;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.cork;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_PPI:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ppi);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ppi;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SID:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.sid);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.sid;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SSN:
				/* read-only */
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ssn);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ssn;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_TSN:
				/* read-only */
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.tsn);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_TSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.tsn;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RECVOPT:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.recvopt);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RECVOPT;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.recvopt;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_COOKIE_LIFE:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cookie_life);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_LIFE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.cookie_life;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SACK_DELAY:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.sack_delay);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_DELAY;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.sack_delay;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.sack_freq);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_FREQUENCY;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.sack_freq;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
#endif
			case T_SCTP_PATH_MAX_RETRANS:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.path_max_retrans);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PATH_MAX_RETRANS;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    t->options.sctp.path_max_retrans;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ASSOC_MAX_RETRANS:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.assoc_max_retrans);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ASSOC_MAX_RETRANS;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    t->options.sctp.assoc_max_retrans;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAX_INIT_RETRIES:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.max_init_retries);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_INIT_RETRIES;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) =
				    t->options.sctp.max_init_retries;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_HEARTBEAT_ITVL:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.heartbeat_itvl);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HEARTBEAT_ITVL;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.heartbeat_itvl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_INITIAL:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto_initial);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_INITIAL;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.rto_initial;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_MIN:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto_min);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MIN;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.rto_min;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_MAX:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto_max);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MAX;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.rto_max;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_OSTREAMS:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ostreams);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_OSTREAMS;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ostreams;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ISTREAMS:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.istreams);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ISTREAMS;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.istreams;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_COOKIE_INC:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cookie_inc);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_INC;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.cookie_inc;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_THROTTLE_ITVL:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.throttle_itvl);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_THROTTLE_ITVL;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.throttle_itvl;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAC_TYPE:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.mac_type);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAC_TYPE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.mac_type;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_CKSUM_TYPE:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.cksum_type);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CKSUM_TYPE;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.cksum_type;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_HB:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.hb);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HB;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_sctp_hb *) T_OPT_DATA(oh)) = t->options.sctp.hb;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.rto);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct t_sctp_rto *) T_OPT_DATA(oh)) = t->options.sctp.rto;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_STATUS:
				/* read-only */
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.status);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_STATUS;
				oh->status = t_overall_result(&overall, T_READONLY);
				/* refresh current value */
				bcopy(&t->options.sctp.status, T_OPT_DATA(oh),
				      sizeof(struct t_sctp_status) +
				      sizeof(struct t_sctp_dest_status));
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DEBUG:
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.debug);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEBUG;
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.debug;
				if (ih->name != T_ALLOPT)
					continue;
#if defined SCTP_CONFIG_ECN
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ECN:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ECN;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ecn);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ecn;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ALI:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ALI;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.ali);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.ali;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ADD:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.add);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.add;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SET:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.set);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.set;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ADD_IP:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD_IP;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.add_ip);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = t->options.sctp.add_ip;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DEL_IP:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEL_IP;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.del_ip);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = t->options.sctp.del_ip;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SET_IP:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET_IP;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.set_ip);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((struct sockaddr_in *) T_OPT_DATA(oh)) = t->options.sctp.set_ip;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_PR:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PR;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.pr);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.pr;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_LIFETIME:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_LIFETIME;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.lifetime);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.lifetime;
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DISPOSITION:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DISPOSITION;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.disposition);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.disposition;
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAX_BURST:
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_BURST;
				oh->len = _T_LENGTH_SIZEOF(t->options.sctp.max_burst);
				oh->status = T_SUCCESS;
				/* refresh current value */
				*((t_uscalar_t *) T_OPT_DATA(oh)) = t->options.sctp.max_burst;
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	sctplogerr(t, "%s() fault", __FUNCTION__);
	return (-EFAULT);
}

/*
 *  Check Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_CHECK placing the output in the provided
 *  buffer.
 */
STATIC t_scalar_t
t_build_check_options(const struct sctp *t, const unsigned char *ip, size_t ilen,
		      unsigned char *op, size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				if (optlen) {
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (!capable(CAP_NET_ADMIN)) {
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
					} else if (optlen) {
						t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

						bcopy(T_OPT_DATA(oh), valp, optlen);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_LINGER:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if ((valp->l_onoff != T_NO && valp->l_onoff != T_YES)
					    || (valp->l_linger == T_UNSPEC
						&& valp->l_onoff != T_NO))
						goto einval;
					if (valp->l_linger == T_UNSPEC) {
						valp->l_linger = t_defaults.xti.linger.l_linger;
					}
					if (valp->l_linger == T_INFINITE) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->l_linger < 0)
						goto einval;
					if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
						valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_RCVBUF / 2) {
						*valp = SOCK_MIN_RCVBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_RCVLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > INT_MAX) {
						*valp = INT_MAX;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDBUF:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > sysctl_rmem_max) {
						*valp = sysctl_rmem_max;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp < SOCK_MIN_SNDBUF / 2) {
						*valp = SOCK_MIN_SNDBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case XTI_SNDLOWAT:
				oh->len = ih->len;
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					} else if (*valp <= 0) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					break;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
				/* not supported yet */
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TOS:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);
					unsigned char prec, type;

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					prec = (*valp >> 5) & 0x7;
					type = *valp & (T_LDELAY | T_HITHRPT | T_HIREL | T_LOCOST);
					if (*valp != SET_TOS(prec, type))
						goto einval;
					if (prec >= T_CRITIC_ECP && !capable(CAP_NET_ADMIN))
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_TTL:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp == 0) {
						*valp = sysctl_ip_default_ttl;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#if 0
					if (*valp > 255) {
						*valp = 255;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#endif
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_REUSEADDR:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_DONTROUTE:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_BROADCAST:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				if (optlen) {
					unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_IP_ADDR:
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
#if 0
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
				oh->len = ih->len;
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
#if 0
		case T_INET_TCP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_MAXSEG:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 8) {
						*valp = 8;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_WINDOW) {
						*valp = MAX_TCP_WINDOW;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPALIVE:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_kpalive *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (valp->kp_onoff != T_YES && valp->kp_onoff != T_NO)
						goto einval;
					if (valp->kp_timeout == T_UNSPEC)
						valp->kp_timeout =
						    t_defaults.tcp.keepalive.kp_timeout;
					if (valp->kp_timeout < 0)
						goto einval;
					if (valp->kp_timeout > MAX_SCHEDULE_TIMEOUT / 60 / HZ) {
						valp->kp_timeout = MAX_SCHEDULE_TIMEOUT / 60 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->kp_onoff == T_YES && valp->kp_timeout < 1) {
						valp->kp_timeout = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_CORK:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_CORK;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPIDLE:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPIDLE;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_KEEPIDLE) {
						*valp = MAX_TCP_KEEPIDLE;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPINTVL:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPINTVL;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_KEEPINTVL) {
						*valp = MAX_TCP_KEEPINTVL;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_KEEPCNT:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPCNT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_KEEPCNT) {
						*valp = MAX_TCP_KEEPCNT;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_SYNCNT:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_SYNCNT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > MAX_TCP_SYNCNT) {
						*valp = MAX_TCP_SYNCNT;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_LINGER2:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_LINGER2;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_INFINITE) {
						if (*valp > sysctl_tcp_fin_timeout / HZ)
							*valp = 0;
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_DEFER_ACCEPT:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_DEFER_ACCEPT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp == T_INFINITE
					    || *valp > ((TCP_TIMEOUT_INIT / HZ) << 31)) {
						*valp = (TCP_TIMEOUT_INIT / HZ) << 31;
					} else if (*valp != 0) {
						int count;

						for (count = 0;
						     count < 32
						     && *valp > ((TCP_TIMEOUT_INIT / HZ) << count);
						     count++) ;
						count++;
						if (*valp != ((TCP_TIMEOUT_INIT / HZ) << count)) {
							*valp = (TCP_TIMEOUT_INIT / HZ) << count;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_WINDOW_CLAMP:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_WINDOW_CLAMP;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < SOCK_MIN_RCVBUF / 2) {
						*valp = SOCK_MIN_RCVBUF / 2;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_INFO:
				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_INFO;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_TCP_QUICKACK:
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_QUICKACK;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_NODELAY;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAXSEG:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAXSEG;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#if 0
					if (*valp > MAX_SCTP_WINDOW) {
						*valp = MAX_SCTP_WINDOW;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
#endif
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_CORK:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CORK;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_NO && *valp != T_YES)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_PPI:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SID:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp > 0x0000ffff)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SSN:
				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_TSN:
				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_TSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RECVOPT:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RECVOPT;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_COOKIE_LIFE:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_LIFE;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1000 / HZ) {
						*valp = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SACK_DELAY:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_DELAY;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1000 / HZ) {
						*valp = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > 500) {
						*valp = 500;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_FREQUENCY;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > 5) {
						*valp = 5;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
#endif
			case T_SCTP_PATH_MAX_RETRANS:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PATH_MAX_RETRANS;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ASSOC_MAX_RETRANS:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ASSOC_MAX_RETRANS;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAX_INIT_RETRIES:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_INIT_RETRIES;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_HEARTBEAT_ITVL:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HEARTBEAT_ITVL;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_INITIAL:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_INITIAL;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1000 / HZ) {
						*valp = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_MIN:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MIN;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1000 / HZ) {
						*valp = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO_MAX:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MAX;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1000 / HZ) {
						*valp = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_OSTREAMS:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_OSTREAMS;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > 0x00010000) {
						*valp = 0x00010000;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ISTREAMS:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ISTREAMS;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp > 0x00010000) {
						*valp = 0x00010000;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_COOKIE_INC:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_INC;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1000 / HZ) {
						*valp = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_THROTTLE_ITVL:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_THROTTLE_ITVL;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp < 1000 / HZ) {
						*valp = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp < 1) {
						*valp = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (*valp / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						*valp = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAC_TYPE:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAC_TYPE;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_CKSUM_TYPE:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CKSUM_TYPE;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_HB:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HB;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_sctp_hb *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (valp->hb_onoff != T_YES && valp->hb_onoff != T_NO)
						goto einval;
					if (valp->hb_itvl == T_UNSPEC)
						valp->hb_itvl = t_defaults.sctp.hb.hb_itvl;
					if (valp->hb_itvl < 0)
						goto einval;
					if (valp->hb_itvl / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						valp->hb_itvl = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->hb_onoff == T_YES && valp->hb_itvl < 1) {
						valp->hb_itvl = 1;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_RTO:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct t_sctp_rto *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (valp->rto_initial < valp->rto_min
					    || valp->rto_initial > valp->rto_max)
						goto einval;
					if (valp->rto_initial < 1000 / HZ) {
						valp->rto_initial = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->rto_min < 1000 / HZ) {
						valp->rto_min = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->rto_max < 1000 / HZ) {
						valp->rto_max = 1000 / HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->rto_initial / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						valp->rto_initial =
						    MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->rto_min / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						valp->rto_min = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
					if (valp->rto_max / HZ > MAX_SCHEDULE_TIMEOUT / 1000) {
						valp->rto_max = MAX_SCHEDULE_TIMEOUT / 1000 * HZ;
						oh->status =
						    t_overall_result(&overall, T_PARTSUCCESS);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_STATUS:
				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_STATUS;
				oh->status = t_overall_result(&overall, T_READONLY);
				if (optlen)
					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DEBUG:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEBUG;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ECN:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ECN;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ALI:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ALI;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ADD:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SET:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_ADD_IP:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD_IP;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DEL_IP:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEL_IP;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_SET_IP:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET_IP;
				oh->status = T_SUCCESS;
				if (optlen) {
					struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_PR:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PR;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (*valp != T_YES && *valp != T_NO)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_LIFETIME:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_LIFETIME;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					if (!*valp || *valp == T_UNSPEC)
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_DISPOSITION:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DISPOSITION;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
					switch (*valp) {
					case T_SCTP_DISPOSITION_NONE:
					case T_SCTP_DISPOSITION_UNSENT:
					case T_SCTP_DISPOSITION_SENT:
					case T_SCTP_DISPOSITION_GAP_ACKED:
					case T_SCTP_DISPOSITION_ACKED:
						break;
					default:
						goto einval;
					}
					(void) valp;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			case T_SCTP_MAX_BURST:
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_BURST;
				oh->status = T_SUCCESS;
				if (optlen) {
					t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

					bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
					if (optlen != sizeof(*valp))
						goto einval;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	sctplogerr(t, "%s() fault", __FUNCTION__);
	return (-EFAULT);
}

/*
   recreate this structure because it is used by an inline 
 */
__u8 ip_tos2prio[16] = { 0, 1, 0, 0, 2, 2, 2, 2, 6, 6, 6, 6, 4, 4, 4, 4 };

/*
 *  Process Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK and
 *  T_NEGOTIARE, placing the output in the provided buffer.
 */
STATIC t_scalar_t
t_build_negotiate_options(struct sctp *t, const unsigned char *ip, size_t ilen, unsigned char *op,
			  size_t *olen)
{
	t_scalar_t overall = T_SUCCESS;
	const struct t_opthdr *ih;
	struct t_opthdr *oh, all;
	int optlen;

	if (ilen == 0) {
		/* For zero-length options fake an option for all names within all levels. */
		all.level = T_ALLLEVELS;
		all.name = T_ALLOPT;
		all.len = sizeof(all);
		all.status = 0;
		ip = (unsigned char *) &all;
		ilen = sizeof(all);
	}
	for (ih = _T_OPT_FIRSTHDR_OFS(ip, ilen, 0), oh = _T_OPT_FIRSTHDR_OFS(op, *olen, 0);
	     ih && oh;
	     ih = _T_OPT_NEXTHDR_OFS(ip, ilen, ih, 0), oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)) {
		/* don't need to do this, it was done when we sized options */
		if (ih->len < sizeof(*ih))
			goto einval;
		if ((unsigned char *) ih + ih->len > ip + ilen)
			goto einval;
		optlen = ih->len - sizeof(*ih);
		switch (ih->level) {
		default:
			goto einval;
		case T_ALLLEVELS:
			if (ih->name != T_ALLOPT)
				goto einval;
		case XTI_GENERIC:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case XTI_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.debug[0];
					} else {
						bcopy(T_OPT_DATA(ih), valp, optlen);
					}
					if (!capable(CAP_NET_ADMIN)) {	/* XXX */
						oh->status =
						    t_overall_result(&overall, T_NOTSUPPORT);
					} else {
						bzero(t->options.xti.debug,
						      sizeof(t->options.xti.debug));
						if (oh->len > sizeof(*oh))
							bcopy(valp, t->options.xti.debug,
							      oh->len - sizeof(*oh));
						t->debug = t->options.xti.debug[0] & 0x01;
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_LINGER:
			{
				struct t_linger *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_LINGER;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.linger;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if ((valp->l_onoff != T_NO
						     && valp->l_onoff != T_YES)
						    || (valp->l_linger == T_UNSPEC
							&& valp->l_onoff != T_NO))
							goto einval;
						if (valp->l_linger == T_UNSPEC) {
							valp->l_linger =
							    t_defaults.xti.linger.l_linger;
						}
						if (valp->l_linger == T_INFINITE) {
							valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->l_linger < 0)
							goto einval;
						if (valp->l_linger > MAX_SCHEDULE_TIMEOUT / HZ) {
							valp->l_linger = MAX_SCHEDULE_TIMEOUT / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.linger = *valp;
					if (valp->l_onoff) {
						t->linger = 1;
						t->lingertime = valp->l_linger * HZ;
					} else {
						t->linger = 0;
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.rcvbuf;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > sysctl_rmem_max) {
							*valp = sysctl_rmem_max;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp < SOCK_MIN_RCVBUF / 2) {
							*valp = SOCK_MIN_RCVBUF / 2;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.rcvbuf = *valp;
					t->rcvbuf = *valp << 1;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_RCVLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_RCVLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.rcvlowat;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > INT_MAX) {
							*valp = INT_MAX;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp <= 0) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.rcvlowat = *valp;
					t->rcvlowat = *valp;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDBUF:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDBUF;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.sndbuf;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > sysctl_rmem_max) {
							*valp = sysctl_rmem_max;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp < SOCK_MIN_SNDBUF / 2) {
							*valp = SOCK_MIN_SNDBUF / 2;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.sndbuf = *valp;
					t->sndbuf = *valp << 1;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case XTI_SNDLOWAT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = XTI_GENERIC;
				oh->name = XTI_SNDLOWAT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.xti.sndlowat;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp > 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						} else if (*valp <= 0) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.xti.sndlowat = *valp;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		case T_INET_IP:
		{
			struct inet_opt *np = &t->inet;

			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_IP_OPTIONS:
			{
				/* not supported yet */
				oh->len = ih->len;
				oh->level = T_INET_IP;
				oh->name = T_IP_OPTIONS;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TOS:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_TOS;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.ip.tos;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						{
							unsigned char prec = (*valp >> 5) & 0x7;
							unsigned char type =
							    *valp & (T_LDELAY | T_HITHRPT | T_HIREL
								     | T_LOCOST);
							if (*valp != SET_TOS(prec, type))
								goto einval;
							if (prec >= T_CRITIC_ECP
							    && !capable(CAP_NET_ADMIN)) {
								oh->status =
								    t_overall_result(&overall,
										     T_NOTSUPPORT);
								break;
							}
						}
					}
					t->options.ip.tos = *valp;
					if (np->tos != t->options.ip.tos) {
						np->tos = *valp;
						t->priority = rt_tos2priority(*valp);
						sp_dst_reset(t);
					}
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_TTL:
			{
				unsigned char *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_TTL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.ip.ttl;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp == 0) {
							*valp = sysctl_ip_default_ttl;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp < 1) {
							*valp = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
#if 0
						if (*valp > 255) {
							*valp = 255;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
#endif
					}
					t->options.ip.ttl = *valp;
#if 0
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
					np->ttl = *valp;
#else
#ifdef HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
					np->uc_ttl = *valp;
#endif
#endif				/* defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
#else
					np->uc_ttl = *valp;
#endif
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_REUSEADDR:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_REUSEADDR;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.ip.reuseaddr;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					t->options.ip.reuseaddr = *valp;
					t->reuse = (*valp == T_YES) ? 1 : 0;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_DONTROUTE:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_DONTROUTE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.ip.dontroute;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					t->options.ip.dontroute = *valp;
					t->localroute = (*valp == T_YES) ? 1 : 0;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_BROADCAST:
			{
				unsigned int *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_BROADCAST;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.ip.broadcast;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					t->options.ip.broadcast = *valp;
					t->broadcast = (*valp == T_YES) ? 1 : 0;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_IP_ADDR:
			{
				uint32_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_IP;
				oh->name = T_IP_ADDR;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.ip.addr;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
					}
					t->options.ip.addr = *valp;
#if 0
					/* FIXME: adjust from sockets to streams */
					sk->saddr = *valp;
#endif
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
			if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
				goto efault;
		}
#if 0
		case T_INET_UDP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_UDP_CHECKSUM:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_UDP;
				oh->name = T_UDP_CHECKSUM;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.udp.checksum;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					t->options.udp.checksum = *valp;
					sk->sk_no_check =
					    (*valp == T_YES) ? UDP_CSUM_DEFAULT : UDP_CSUM_NOXMIT;
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
#if 0
		case T_INET_TCP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_TCP_NODELAY:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_NODELAY;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.nodelay;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					t->options.tcp.nodelay = *valp;
					if ((tp->nonagle = (*valp == T_YES) ? 1 : 0))
						tcp_push_pending_frames(sk, tp);
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_MAXSEG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_MAXSEG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.maxseg;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp < 8) {
							*valp = 8;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (*valp > MAX_TCP_WINDOW) {
							*valp = MAX_TCP_WINDOW;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.tcp.maxseg = *valp;
					tp->user_mss = *valp;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPALIVE:
			{
				struct t_kpalive *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPALIVE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.keepalive;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (valp->kp_onoff != T_YES
						    && valp->kp_onoff != T_NO)
							goto einval;
						if (valp->kp_timeout == T_UNSPEC) {
							valp->kp_timeout =
							    t_defaults.tcp.keepalive.kp_timeout;
						}
						if (valp->kp_timeout < 0)
							goto einval;
						if (valp->kp_timeout >
						    MAX_SCHEDULE_TIMEOUT / 60 / HZ) {
							valp->kp_timeout =
							    MAX_SCHEDULE_TIMEOUT / 60 / HZ;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
						if (valp->kp_onoff == T_YES && valp->kp_timeout < 1) {
							valp->kp_timeout = 1;
							oh->status =
							    t_overall_result(&overall,
									     T_PARTSUCCESS);
						}
					}
					t->options.tcp.keepalive = *valp;
					if (valp->kp_onoff)
						tp->keepalive_time = valp->kp_timeout * 60 * HZ;
#if defined HAVE_TCP_SET_KEEPALIVE_ADDR
					tcp_set_keepalive(sk, (valp->kp_onoff == T_YES) ? 1 : 0);
#endif				/* defined HAVE_TCP_SET_KEEPALIVE_ADDR */
					if (valp->kp_onoff == T_YES)
						sock_set_keepopen(sk);
					else
						sock_clr_keepopen(sk);
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_CORK:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_CORK;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.cork;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.cork = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPIDLE:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPIDLE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.keepidle;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.keepidle = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPINTVL:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPINTVL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.keepitvl;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.keepitvl = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_KEEPCNT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_KEEPCNT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.keepcnt;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.keepcnt = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_SYNCNT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_SYNCNT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.syncnt;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.syncnt = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_LINGER2:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_LINGER2;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.linger2;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.linger2 = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_DEFER_ACCEPT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_DEFER_ACCEPT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.defer_accept;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.defer_accept = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_WINDOW_CLAMP:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_WINDOW_CLAMP;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.window_clamp;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.window_clamp = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_INFO:
			{
				struct t_tcp_info *valp = (typeof(valp)) T_OPT_DATA(oh);

				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_TCP;
				oh->name = T_TCP_INFO;
				oh->status = t_overall_result(&overall, T_READONLY);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_TCP_QUICKACK:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_TCP;
				oh->name = T_TCP_QUICKACK;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (!sk) {
					oh->status = t_overall_result(&overall, T_FAILURE);
				} else {
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.tcp.quickack;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.tcp.quickack = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
#endif
		case T_INET_SCTP:
			switch (ih->name) {
			default:
				oh->len = ih->len;
				oh->level = ih->level;
				oh->name = ih->name;
				oh->status = t_overall_result(&overall, T_NOTSUPPORT);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				continue;
			case T_ALLOPT:
			case T_SCTP_NODELAY:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_NODELAY;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.nodelay;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						if (*valp != T_YES && *valp != T_NO)
							goto einval;
					}
					t->options.sctp.nodelay = *valp;
					t->nonagle = (*valp == T_YES) ? 1 : 0;
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_MAXSEG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAXSEG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.maxseg;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.maxseg = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_CORK:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CORK;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.cork;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.cork = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_PPI:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.ppi;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.ppi = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_SID:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.sid;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.sid = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_SSN:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_TSN:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_TSN;
				oh->status = t_overall_result(&overall, T_READONLY);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_RECVOPT:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RECVOPT;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.recvopt;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.recvopt = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_COOKIE_LIFE:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_LIFE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.cookie_life;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.cookie_life = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_SACK_DELAY:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_DELAY;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.sack_delay;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.sack_delay = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
#ifdef ETSI
			case T_SCTP_SACK_FREQUENCY:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SACK_FREQUENCY;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.sack_freq;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.sack_freq = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
#endif
			case T_SCTP_PATH_MAX_RETRANS:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PATH_MAX_RETRANS;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.path_max_retrans;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.path_max_retrans = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_ASSOC_MAX_RETRANS:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ASSOC_MAX_RETRANS;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.assoc_max_retrans;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.assoc_max_retrans = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_MAX_INIT_RETRIES:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_INIT_RETRIES;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.max_init_retries;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.max_init_retries = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_HEARTBEAT_ITVL:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HEARTBEAT_ITVL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.heartbeat_itvl;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.heartbeat_itvl = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_RTO_INITIAL:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_INITIAL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.rto_initial;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.rto_initial = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_RTO_MIN:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MIN;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.rto_min;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.rto_min = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_RTO_MAX:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO_MAX;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.rto_max;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.rto_max = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_OSTREAMS:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_OSTREAMS;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.ostreams;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.ostreams = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_ISTREAMS:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ISTREAMS;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.istreams;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.istreams = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_COOKIE_INC:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_COOKIE_INC;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.cookie_inc;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.cookie_inc = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_THROTTLE_ITVL:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_THROTTLE_ITVL;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.throttle_itvl;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.throttle_itvl = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_MAC_TYPE:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAC_TYPE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.mac_type;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.mac_type = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_CKSUM_TYPE:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_CKSUM_TYPE;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.cksum_type;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.cksum_type = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_HB:
			{
				struct t_sctp_hb *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_HB;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.hb;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.hb = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_RTO:
			{
				struct t_sctp_rto *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_RTO;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.rto;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.rto = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_STATUS:
			{
				struct t_sctp_status_pair *valp = (typeof(valp)) T_OPT_DATA(oh);

				/* read-only */
				oh->len = ih->len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_STATUS;
				oh->status = t_overall_result(&overall, T_READONLY);
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_DEBUG:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEBUG;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				{
					if (ih->name == T_ALLOPT) {
						*valp = t_defaults.sctp.debug;
					} else {
						*valp = *((typeof(valp)) T_OPT_DATA(ih));
						/* negotiate value */
					}
					t->options.sctp.debug = *valp;
					/* set value on socket or stream */
				}
				if (ih->name != T_ALLOPT)
					continue;
#if defined SCTP_CONFIG_ECN
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_ECN:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ECN;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.ecn;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.ecn = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_ALI:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ALI;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.ali;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.ali = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#if defined SCTP_CONFIG_ADD_IP
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_ADD:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.add;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.add = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_SET:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.set;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.set = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_ADD_IP:
			{
				struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ADD_IP;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.add_ip;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.add_ip = *valp;
				/* set value on socket or stream */
				sctp_add_ip(t, valp);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_DEL_IP:
			{
				struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DEL_IP;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.del_ip;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.del_ip = *valp;
				/* set value on socket or stream */
				sctp_del_ip(t, valp);
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_SET_IP:
			{
				struct sockaddr_in *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SET_IP;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.set_ip;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.set_ip = *valp;
				/* set value on socket or stream */
				sctp_set_ip(t, valp);
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_ADD_IP */
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_PR:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PR;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.pr;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.pr = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_LIFETIME:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_LIFETIME;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.lifetime;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.lifetime = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_DISPOSITION:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_DISPOSITION;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.disposition;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.disposition = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
				if (!(oh = _T_OPT_NEXTHDR_OFS(op, *olen, oh, 0)))
					goto efault;
			}
			case T_SCTP_MAX_BURST:
			{
				t_uscalar_t *valp = (typeof(valp)) T_OPT_DATA(oh);

				oh->len = _T_LENGTH_SIZEOF(*valp);
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_MAX_BURST;
				oh->status = T_SUCCESS;
				bcopy(T_OPT_DATA(ih), T_OPT_DATA(oh), optlen);
				if (ih->name == T_ALLOPT) {
					*valp = t_defaults.sctp.max_burst;
				} else {
					*valp = *((typeof(valp)) T_OPT_DATA(ih));
					/* negotiate value */
				}
				t->options.sctp.max_burst = *valp;
				/* set value on socket or stream */
				if (ih->name != T_ALLOPT)
					continue;
			}
			}
			if (ih->level != T_ALLLEVELS)
				continue;
		}
	}
	if (ih && !oh)
		goto efault;
	if (oh)
		*olen = (unsigned char *) oh - op;
	return (overall);
      einval:
	return (-EINVAL);
      efault:
	sctplogerr(t, "%s() fault", __FUNCTION__);
	return (-EFAULT);
}

/*
 *  Process Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK and
 *  T_NEGOTIARE, placing the output in the provided buffer.
 */
STATIC t_scalar_t
t_build_options(struct sctp *t, unsigned char *ip, size_t ilen, unsigned char *op, size_t *olen,
		t_scalar_t flag)
{
	switch (flag) {
	case T_DEFAULT:
		return t_build_default_options(t, ip, ilen, op, olen);
	case T_CURRENT:
		return t_build_current_options(t, ip, ilen, op, olen);
	case T_CHECK:
		return t_build_check_options(t, ip, ilen, op, olen);
	case T_NEGOTIATE:
		return t_build_negotiate_options(t, ip, ilen, op, olen);
	}
	return (-EINVAL);
}

/*  =========================================================================
 *  =========================================================================
 *
 *  SCTP T-Provider --> T-User Primitives (Indication, Confirmation and Ack)
 *
 *  =========================================================================
 *
 *  T_CONN_IND      11 - connection indication
 *  -----------------------------------------------------------------
 *  We get the connection indication information from the cookie received in the COOKIE ECHO which
 *  invokes the indication.  (We queue the COOKIE ECHO chunks themselves as indications.)
 */
STATIC int
t_conn_ind(struct sctp *sp, mblk_t *cp)
{
	mblk_t *mp, *bp;
	struct T_conn_ind *p;
	struct sctp_cookie_echo *m = (struct sctp_cookie_echo *) cp->b_rptr;
	struct sctp_cookie *ck = (struct sctp_cookie *) m->cookie;
	size_t danum = ck->danum + 1;
	uint32_t *daptr = (uint32_t *) (((caddr_t) (ck + 1) + ck->opt_len));
	struct sockaddr_in *sin;
	size_t src_len = danum * sizeof(*sin);
	size_t opt_len = sctp_size_conn_ind_opts();

	if ((1 << sp->i_state) & ~TSM_LISTEN)
		goto outstate;
	if (bufq_length(&sp->conq) >= sp->conind)
		goto erestart;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + src_len + opt_len, BPRI_MED)))
		goto enobufs;
	if (!(bp = sctp_dupmsg(sp, cp)))
		goto enodup;
	mp->b_datap->db_type = M_PROTO;
	p = (struct T_conn_ind *) mp->b_wptr;
	p->PRIM_type = T_CONN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = src_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
	p->SEQ_number = (t_uscalar_t) (long) bp;
	mp->b_wptr += sizeof(*p);
	/* place address information from cookie */
	if (danum--) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = ck->dport;
		sin->sin_addr.s_addr = ck->daddr;
		mp->b_wptr += sizeof(*sin);
	}
	while (danum--) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = ck->dport;
		sin->sin_addr.s_addr = *daptr++;
		mp->b_wptr += sizeof(*sin);
	}
	if (opt_len) {
		sctp_build_conn_ind_opts(sp, mp->b_wptr, opt_len, ck);
		mp->b_wptr += opt_len;
	}
	bufq_queue(&sp->conq, bp);
	sctp_t_setstate(sp, TS_WRES_CIND);
	sctplogtx(sp, "<- T_CONN_IND");
	putnext(sp->rq, mp);
	return (0);
      enodup:
	freemsg(mp);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
      erestart:
	seldom();
	return (-ERESTART);
      outstate:
	sctplogerr(sp, "%s() in wrong TPI state %d", __FUNCTION__, (int) sp->i_state);
	return (-ECONNREFUSED);
}

/*
 *  T_CONN_CON      12 - connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated are the number of inbound and
 *  outbound streams.
 */
STATIC int
t_conn_con(struct sctp *sp)
{
	mblk_t *mp;
	struct T_conn_con *p;
	struct sctp_daddr *sd;
	struct sockaddr_in *sin;
	size_t res_len = sp->danum * sizeof(*sin);
	size_t opt_len = sctp_size_conn_con_opts(sp);

	if (sp->i_state != TS_WCON_CREQ)
		goto outstate;
	if (!bcanputnext(sp->rq, 1))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + res_len + opt_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (struct T_conn_con *) mp->b_wptr;
	p->PRIM_type = T_CONN_CON;
	p->RES_length = res_len;
	p->RES_offset = res_len ? sizeof(*p) : 0;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;
	mp->b_wptr += sizeof(*p);
	/* place destination (responding) address */
	for (sd = sp->daddr; sd; sd = sd->next) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = sp->dport;
		sin->sin_addr.s_addr = sd->daddr;
		mp->b_wptr += sizeof(*sin);
	}
	if (opt_len) {
		if (t_build_conn_opts(sp, mp->b_wptr, opt_len) < 0)
			goto efault;
		mp->b_wptr += opt_len;
	}
	sctp_t_setstate(sp, TS_DATA_XFER);
	sctplogtx(sp, "<- T_CONN_CON");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in wrong TPI state %d", __FUNCTION__, (int) sp->i_state);
	return (0);
      efault:
	sctplogerr(sp, "%s() in wrong TPI state %d", __FUNCTION__, (int) sp->i_state);
	freemsg(mp);
	return (-EFAULT);
}

/**
 * t_discon_ind: - issue T_DISCON_IND (13 - disconnect indication)
 * @sp: private structure (locked)
 * @orig: origin of disconnect
 * @reason: reason for disconnect
 * @seq: disconnecting sequence number
 *
 * We use the address of the mblk that contains the COOKIE-ECHO chunk as a SEQ_number for connect
 * indications that are rejected with a disconnect indication as well.  We can use this to directly
 * address the mblk in the connection indication bufq.
 *
 * If the caller provides disconnect data, the caller needs to set the current ord, ppi, sid, and
 * ssn fields so that the user can examine them with T_OPTMGMT_REQ T_CURRENT if it has need to know
 * them.
 *
 * If a connection indication identifier (seq) is provided, this function muse either free it or
 * return an error.  The origin is used only by the STREAMS NPI interface.
 */
STATIC int
t_discon_ind(struct sctp *sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *seq)
{
	mblk_t *mp;
	struct T_discon_ind *p;

	(void) orig;
	if ((1 << sp->i_state) & ~TSM_CONNECTED)
		goto outstate;
	/* TPI spec says that if the interface is in the TS_DATA_XFER, TS_WIND_ORDREL or
	   TS_WACK_ORDREL [sic] state, the stream must be flushed before sending up the
	   T_DISCON_IND primitive. */
	if ((1 << sp->i_state) & (TSM_INDATA | TSM_OUTDATA))
		if (m_flush(sp, NULL, FLUSHRW, 0) == -ENOBUFS)
			goto enobufs;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	if (!canputnext(sp->rq))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (struct T_discon_ind *) mp->b_wptr;
	p->PRIM_type = T_DISCON_IND;
	p->DISCON_reason = reason;
	mp->b_wptr += sizeof(*p);
	/* FIXME: We really need to map SCTP core protocol engine reasons to TPI reasons here.  All 
	   negative reasons are UNIX error codes, all positive reasons are SCTP cause values. */
	p->SEQ_number = (t_uscalar_t) (long) seq;
	if (seq)
		freemsg(seq);
	if (!bufq_length(&sp->conq))
		sctp_t_setstate(sp, TS_IDLE);
	else
		sctp_t_setstate(sp, TS_WRES_CIND);
	sctplogtx(sp, "<- T_DISCON_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	freemsg(mp);
	return (-EBUSY);
      outstate:
	/* Sometimes this function is called in the idle state. */
	if (sp->i_state != TS_IDLE)
		sctplogerr(sp, "%s() in TPI state %s", __FUNCTION__, sctp_t_statename(sp->i_state));
	return (0);
}

/**
 * t_data_ind: - issue T_DATA_IND (14 - data indication)
 * @sp: private structure (locked)
 * @more: another primitive completes the TSDU
 * @dp: user data
 *
 * This indication is only useful for delivering data indications for the default stream.  The
 * caller should check that ppi and sid match the default before using this indication.  Otherwise
 * the caller should use the T_OPTDATA_IND.
 */
STATIC int
t_data_ind(struct sctp *sp, t_uscalar_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_ind *p;

	if ((1 << sp->i_state) & ~TSM_INDATA)
		goto outstate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (struct T_data_ind *) mp->b_wptr;
	p->PRIM_type = T_DATA_IND;
	p->MORE_flag = more;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	sctplogda(sp, "<- T_DATA_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "%s() flow controlled", __FUNCTION__);
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in TPI state %s", __FUNCTION__, sctp_t_statename(sp->i_state));
	freemsg(dp);
	return (0);
}

/*
 *  T_EXDATA_IND    15 - expedited data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default stream.  The
 *  caller should check that ppi and ssn match the default before using this indication.  Otherwise
 *  the caller should use the T_OPTDATA_IND.
 */
STATIC int
t_exdata_ind(struct sctp *sp, t_uscalar_t more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;

	if ((1 << sp->i_state) & ~TSM_INDATA)
		goto outstate;
	if (!bcanputnext(sp->rq, 1))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;		/* expedite */
	p = (struct T_exdata_ind *) mp->b_wptr;
	p->PRIM_type = T_EXDATA_IND;
	p->MORE_flag = more;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	sctplogda(sp, "<- T_EXDATA_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "%s() flow controlled", __FUNCTION__);
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in TPI state %s", __FUNCTION__, sctp_t_statename(sp->i_state));
	freemsg(dp);
	return (0);
}

/*
 *  T_INFO_ACK      16 - information acknowledgement
 *  -----------------------------------------------------------------
 *  Although there is no limit on CDATA and DDATA size, if these are too large then we will IP
 *  fragment the message.
 */
STATIC int
t_info_ack(struct sctp *sp)
{
	mblk_t *mp;
	struct T_info_ack *p;

	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (struct T_info_ack *) mp->b_wptr;
	p->PRIM_type = T_INFO_ACK;
	p->TSDU_size = T_INFINITE;	/* no limit on TSDU size */
	p->ETSDU_size = T_INFINITE;	/* no limit on ETSDU size */
	p->CDATA_size = sp->pmtu - 16;	/* one MTU limit on CDATA size */
	p->DDATA_size = T_INVALID;	/* no DDATA size */
	p->ADDR_size = sizeof(struct sockaddr_storage);	/* no limit on ADDR size */
	p->OPT_size = T_INFINITE;	/* no limit on OPTIONS size */
	p->TIDU_size = 65535;	/* largest message limit on TIDU size */
	p->SERV_type = T_COTS_ORD;	/* COTS with orderly release */
	p->CURRENT_state = sp->i_state;
	p->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
	mp->b_wptr += sizeof(*p);
	sctplogtx(sp, "<- T_INFO_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  T_BIND_ACK      17 - bind acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_ack(struct sctp *sp)
{
	mblk_t *mp;
	struct T_bind_ack *p;
	struct sctp_saddr *ss = sp->saddr;
	struct sockaddr_in *add;
	size_t add_len = sp->sanum ? sp->sanum * sizeof(*add) : sizeof(*add);

	if (sp->i_state != TS_WACK_BREQ)
		goto outstate;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + add_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (struct T_bind_ack *) mp->b_wptr;
	p->PRIM_type = T_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->CONIND_number = sp->conind;
	mp->b_wptr += sizeof(*p);
	if (sp->sanum) {
		for (; ss; ss = ss->next) {
			add = (typeof(add)) mp->b_wptr;
			add->sin_family = AF_INET;
			add->sin_port = sp->sport;
			add->sin_addr.s_addr = ss->saddr;
			mp->b_wptr += sizeof(*add);
		}
	} else {
		add = (typeof(add)) mp->b_wptr;
		add->sin_family = AF_INET;
		add->sin_port = sp->sport;
		add->sin_addr.s_addr = INADDR_ANY;
		mp->b_wptr += sizeof(*add);
	}
	sctp_t_setstate(sp, TS_IDLE);
	sctplogtx(sp, "<- T_BIND_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      outstate:
	sctplogerr(sp, "%s() in TPI state %s", __FUNCTION__, sctp_t_statename(sp->i_state));
	return (0);
}

/**
 * t_error_ack - generate T_ERROR_ACK error acknowledgement
 * @sp: private structure
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: negative system error or positive TPI error
 *
 * Note that some system errors returned by the SCTP core translate to TPI errors.  Returns either
 * QR_ABSORBED or -ENOBUFS.
 */
noinline __unlikely int
t_error_ack(struct sctp *sp, mblk_t *msg, t_uscalar_t prim, t_scalar_t err)
{
	mblk_t *mp;
	struct T_error_ack *p;

	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
		return (err);
	case QR_DONE:
		freemsg(msg);
		return (QR_ABSORBED);
	case -EADDRINUSE:
		err = TADDRBUSY;
		break;
	case -EADDRNOTAVAIL:
		err = TNOADDR;
		break;
	case -EACCES:
		err = TACCES;
		break;
	case -EISCONN:
		err = TOUTSTATE;
		break;
	case -EAFNOSUPPORT:
		err = TBADADDR;
		break;
	case -EOPNOTSUPP:
		err = TNOTSUPPORT;
		break;
	}
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (struct T_error_ack *) mp->b_wptr;
	p->PRIM_type = T_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TLI_error = (err < 0) ? TSYSERR : err;
	p->UNIX_error = (err < 0) ? -err : 0;
	mp->b_wptr += sizeof(*p);
	switch (sp->i_state) {
#ifdef TS_WACK_OPTREQ
	case TS_WACK_OPTREQ:
#endif
	case TS_WACK_UREQ:
	case TS_WACK_CREQ:
		sctp_t_setstate(sp, TS_IDLE);
		break;
	case TS_WACK_BREQ:
		sctp_t_setstate(sp, TS_UNBND);
		break;
	case TS_WACK_CRES:
		sctp_t_setstate(sp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ6:
		sctp_t_setstate(sp, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		sctp_t_setstate(sp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		sctp_t_setstate(sp, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		sctp_t_setstate(sp, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		sctp_t_setstate(sp, TS_WREQ_ORDREL);
		break;
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs
		   normally when we send TOUTSTATE or TNOTSUPPORT or are responding to a
		   T_OPTMGMT_REQ in other then TS_IDLE state. */
	}
	freemsg(msg);
	sctplogtx(sp, "<- T_ERROR_ACK");
	putnext(sp->rq, mp);
	return (QR_ABSORBED);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  T_OK_ACK        19 - success acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_ok_ack(struct sctp *sp, t_uscalar_t prim, mblk_t *cp, struct sctp *ap)
{
	mblk_t *mp;
	struct T_ok_ack *p;

	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (struct T_ok_ack *) mp->b_wptr;
	p->PRIM_type = T_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	/* switch by prim not by state */
	switch (prim) {
	case T_CONN_REQ:
		sctp_t_setstate(sp, TS_WCON_CREQ);
		break;
	case T_UNBIND_REQ:
		sctp_t_setstate(sp, TS_UNBND);
		break;
	case T_CONN_RES:
		if (ap != NULL)
			sctp_t_setstate(ap, TS_DATA_XFER);
		if (cp != NULL)
			freemsg(cp);
		if (ap == NULL || ap->rq != sp->rq) {
			if (bufq_length(&sp->conq))
				sctp_t_setstate(sp, TS_WRES_CIND);
			else
				sctp_t_setstate(sp, TS_IDLE);
		}
		break;
	case T_DISCON_REQ:
		switch (sp->i_state) {
		case TS_WACK_DREQ7:
			if (cp != NULL)
				freemsg(cp);
		case TS_WACK_DREQ6:
			if (bufq_length(&sp->conq))
				sctp_t_setstate(sp, TS_WRES_CIND);
			else
				sctp_t_setstate(sp, TS_IDLE);
			break;
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			/* TPI spec says that if the interface is in the TS_DATA_XFER,
			   TS_WIND_ORDREL or TS_WACK_ORDREL [sic] state, the stream must be flushed 
			   before responding with the T_OK_ACK primitive. */
			if (m_flush(sp, NULL, FLUSHRW, 0) == -ENOBUFS)
				goto enobufs;
			if (bufq_length(&sp->conq))
				sctp_t_setstate(sp, TS_WRES_CIND);
			else
				sctp_t_setstate(sp, TS_IDLE);
			break;
		}
		break;
	}
	/* Note: if we are not in a WACK state we simply do not change state.  This occurs normally 
	   when we are responding to a T_OPTMGMT_REQ in other than the TS_IDLE state. */
	sctplogtx(sp, "<- T_OK_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  T_OPTMGMT_ACK       22 - Options Management Acknowledge
 *  -------------------------------------------------------------------------
 *  Note: opt_len is conservative but might not be actual size of the output
 *  options.  This will be adjusted when the option buffer is built.
 */
STATIC int
t_optmgmt_ack(struct sctp *sp, long flags, unsigned char *req, size_t req_len, size_t opt_len)
{
	mblk_t *mp;
	struct T_optmgmt_ack *p;

	if (!(mp = sctp_allocb(sp, sizeof(*p) + opt_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	if ((flags = t_build_options(sp, req, req_len, mp->b_wptr, &opt_len, flags)) < 0) {
		freemsg(mp);
		return (flags);
	}
	p->PRIM_type = T_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	p->MGMT_flags = flags;
	if (opt_len) {
		mp->b_wptr += opt_len;
	}
#ifdef TS_WACK_OPTREQ
	if (sp->i_state == TS_WACK_OPTREQ)
		sctp_t_setstate(sp, TS_IDLE);
#endif
	sctplogtx(sp, "<- T_OPTMGMT_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  T_ORDREL_IND    23 - orderly release indication
 *  -----------------------------------------------------------------
 */
STATIC int
t_ordrel_ind(struct sctp *sp)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;

	if ((1 << sp->i_state) & ~TSM_INDATA)
		goto outstate;
	if (!canputnext(sp->rq))
		goto ebusy;
	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (struct T_ordrel_ind *) mp->b_wptr;
	p->PRIM_type = T_ORDREL_IND;
	mp->b_wptr += sizeof(*p);
	switch (sp->i_state) {
	case TS_DATA_XFER:
		sctp_t_setstate(sp, TS_WREQ_ORDREL);
		break;
	case TS_WIND_ORDREL:
		sctp_t_setstate(sp, TS_IDLE);
		break;
	}
	sctplogtx(sp, "<- T_ORDREL_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "flow controlled");
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in TPI state %s", __FUNCTION__, sctp_t_statename(sp->i_state));
	return (0);
}

/*
 *  T_OPTDATA_IND   26 - data with options indication
 *  -----------------------------------------------------------------
 */
STATIC int
t_optdata_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
	      uint more, mblk_t *dp)
{
	mblk_t *mp;
	struct t_opthdr *oh;
	struct T_optdata_ind *p;
	size_t str_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t opt_len = 0;

	if ((1 << sp->i_state) & ~TSM_OUTDATA)
		goto outstate;
	if (!bcanputnext(sp->rq, ord ? 0 : 1))
		goto ebusy;
	if (sp->options.sctp.recvopt)
		opt_len = 4 * str_len;
	if (!(mp = sctp_allocb(sp, sizeof(*p) + opt_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = ord ? 0 : 1;	/* expedite */
	p = (struct T_optdata_ind *) mp->b_wptr;
	p->PRIM_type = T_OPTDATA_IND;
	p->DATA_flag = (more ? T_ODF_MORE : 0) | (ord ? 0 : T_ODF_EX);
	p->OPT_length = opt_len;
	p->OPT_offset = opt_len ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	/* indicate options */
	if (sp->options.sctp.recvopt) {
		oh = (struct t_opthdr *) mp->b_wptr;
		oh->len = str_len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->status = T_SUCCESS;
		mp->b_wptr += sizeof(*oh);
		*(t_uscalar_t *) mp->b_wptr = ppi;
		mp->b_wptr += sizeof(t_uscalar_t);
		oh = (struct t_opthdr *) mp->b_wptr;
		oh->len = str_len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->status = T_SUCCESS;
		mp->b_wptr += sizeof(*oh);
		*(t_uscalar_t *) mp->b_wptr = sid;
		mp->b_wptr += sizeof(t_uscalar_t);
		oh = (struct t_opthdr *) mp->b_wptr;
		oh->len = str_len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SSN;
		oh->status = T_SUCCESS;
		mp->b_wptr += sizeof(*oh);
		*(t_uscalar_t *) mp->b_wptr = ssn;
		mp->b_wptr += sizeof(t_uscalar_t);
		oh = (struct t_opthdr *) mp->b_wptr;
		oh->len = str_len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_TSN;
		oh->status = T_SUCCESS;
		mp->b_wptr += sizeof(*oh);
		*(t_uscalar_t *) mp->b_wptr = tsn;
		mp->b_wptr += sizeof(t_uscalar_t);
	}
	mp->b_cont = dp;
	sctplogda(sp, "<- T_OPTDATA_IND");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	sctplogtx(sp, "flow controlled");
	return (-EBUSY);
      outstate:
	sctplogerr(sp, "%s() in TPI state %s", __FUNCTION__, sctp_t_statename(sp->i_state));
	freemsg(dp);
	return (0);
}

/*
 *  T_ADDR_ACK      27 - address acknowledgement
 *  -----------------------------------------------------------------
 *  Version 2 uses socket addresses instead of the Version 1 sctp_addr_t.
 */
STATIC int
t_addr_ack(struct sctp *sp)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	struct sockaddr_in *sin;
	struct sctp_saddr *ss = sp->saddr;
	struct sctp_daddr *sd = sp->daddr;
	size_t loc_len = sp->sanum * sizeof(*sin);
	size_t rem_len = sp->danum * sizeof(*sin);

	if (!(mp = sctp_allocb(sp, sizeof(*p) + loc_len + rem_len, BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = M_PCPROTO;
	p = (struct T_addr_ack *) mp->b_wptr;
	p->PRIM_type = T_ADDR_ACK;
	p->LOCADDR_length = loc_len;
	p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
	p->REMADDR_length = rem_len;
	p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
	mp->b_wptr += sizeof(*p);
	for (; ss; ss = ss->next) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = sp->sport;
		sin->sin_addr.s_addr = ss->saddr;
		mp->b_wptr += sizeof(*sin);
	}
	for (; sd; sd = sd->next) {
		sin = (typeof(sin)) mp->b_wptr;
		sin->sin_family = AF_INET;
		sin->sin_port = sp->dport;
		sin->sin_addr.s_addr = sd->daddr;
		mp->b_wptr += sizeof(*sin);
	}
	sctplogtx(sp, "<- T_ADDR_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

#ifdef T_CAPABILITY_ACK
/*
 *  T_CAPABILITY_ACK ?? - protocol capability ack
 *  -----------------------------------------------------------------
 */
STATIC int
t_capability_ack(struct sctp *sp, t_uscalar_t caps, int type)
{
	mblk_t *mp;
	struct T_capability_ack *p;

	if (!(mp = sctp_allocb(sp, sizeof(*p), BPRI_MED)))
		goto enobufs;
	mp->b_datap->db_type = type;
	p = (struct T_capability_ack *) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = T_CAPABILITY_ACK;
	p->CAP_bits1 = caps;
	p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (t_uscalar_t) (long) sp->rq : 0;
	if (caps & TC1_INFO) {
		p->INFO_ack.PRIM_type = T_INFO_ACK;
		p->INFO_ack.TSDU_size = T_INFINITE;
		p->INFO_ack.ETSDU_size = T_INFINITE;
		p->INFO_ack.CDATA_size = sp->pmtu - 16;
		p->INFO_ack.DDATA_size = T_INVALID;
		p->INFO_ack.ADDR_size = sizeof(struct sockaddr_storage);
		p->INFO_ack.OPT_size = T_INFINITE;
		p->INFO_ack.TIDU_size = 65535;
		p->INFO_ack.SERV_type = T_COTS_ORD;
		p->INFO_ack.CURRENT_state = sp->i_state;
		p->INFO_ack.PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
	} else
		bzero(&p->INFO_ack, sizeof(p->INFO_ack));
	sctplogtx(sp, "<- T_CAPABILITY_ACK");
	putnext(sp->rq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
#endif

/*
 *  NOTES:- Sockets and XTI/TPI cannot do data acknowledgements, resets or retrieval directly.  Data
 *  acknowledgements and retrieval are different forms of the same service.  For data
 *  acknowledgement, use the STREAMS NPI interface.  For reset support (SCTP Restart indication
 *  different from SCTP CDI), use the STREAMS NPI interface.
 */
STATIC int
sctp_t_conn_ind(struct sctp *sp, mblk_t *cp)
{
	return t_conn_ind(sp, cp);
}
STATIC int
sctp_t_conn_con(struct sctp *sp)
{
	return t_conn_con(sp);
}
STATIC int
sctp_t_data_ind(struct sctp *sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
		uint more, mblk_t *dp)
{
	if (sp->options.sctp.recvopt) {
		return t_optdata_ind(sp, ppi, sid, ssn, tsn, ord, more, dp);
	}
	if (ord) {
		return t_data_ind(sp, more, dp);
	} else {
		return t_exdata_ind(sp, more, dp);
	}
}
STATIC int
sctp_t_discon_ind(struct sctp *sp, t_uscalar_t orig, t_scalar_t reason, mblk_t *cp)
{
	return t_discon_ind(sp, orig, reason, cp);
}
STATIC int
sctp_t_ordrel_ind(struct sctp *sp)
{
	return t_ordrel_ind(sp);
}

STATIC struct sctp_ifops t_ops = {
	.conn_ind = &sctp_t_conn_ind,
	.conn_con = &sctp_t_conn_con,
	.data_ind = &sctp_t_data_ind,
	.datack_ind = NULL,
	.reset_ind = NULL,
	.reset_con = NULL,
	.discon_ind = &sctp_t_discon_ind,
	.ordrel_ind = &sctp_t_ordrel_ind,
	.retr_ind = NULL,
	.retr_con = NULL,
};

/*
 *  =========================================================================
 *
 *  SCTP T-User --> T-Provider Primitives (Request and Response)
 *
 *  =========================================================================
 *  These represent primitive requests and responses from the Transport Provider Interface (TPI)
 *  transport user.  Each of these requests or responses invoked a protocol action depending on the
 *  current state of the provider.
 */
/*
 *  T_CONN_REQ           0 - TC requeset
 *  -------------------------------------------------------------------------
 *  We have received a connection request from the user.  We use the characteristics of the
 *  primitive and the options on the stream to formulate and INIT message and send it to the peer.
 *  We acknowledge the success or failure to starting this process to the user.  Once the process is
 *  started, a successful connection will conclude with COOKIE_ACK message that will generate a
 *  T_CONN_CON.  If the init process times out or there are other errors associated with
 *  establishing the SCTP association, they will be returned via the T_DISCON_IND primitive.
 *
 *  The time-sequence diagrams look like this:
 *
 *                       |                            |                      
 *   T_CONN_REQ -------->|---+ TS_WACK_CREQ           |                      
 *                       |   |                        |                      
 *   T_ERROR_ACK <-------|<--+ TS_IDLE                |                      
 *                       |                            |                      
 *                       |           INIT             |                      
 *   T_CONN_REQ -------->|---+----------------------->|---+                  
 *                       |   | TS_WACK_CREQ           |   |                  
 *   T_OK_ACK <----------|<--+ TS_WCON_CREQ           |   |  Listener Stream 
 *                       |                            |   |  w/ Options      
 *                       |           ABORT            |   |                  
 *   T_DISCON_IND <------|<----TS_IDLE----------------|<--+                  
 *                       |    Timeout, other problem  |                      
 *                       |                            |                      
 *                       |           INIT             |                      
 *   T_CONN_REQ --+----->|---+----------------------->|---+                  
 *                |      |   | TS_WACK_CREQ           |   |                  
 *   T_OK_ACK <----------|<--+ TS_WCON_CREQ           |   |  Listener Stream 
 *                |      |                            |   |  w/ Options      
 *                |      |         INIT ACK           |   |                  
 *                |  +---|<---------------------------|<--+                  
 *                |  |   |       COOKIE ECHO          |                      
 *           DATA +--+-->|--------------------------->|----------> T_CONN_IND
 *                       |                            |                      
 *                       |       COOKIE ACK           |                      
 *   T_CONN_CON <--------|<---------------------------|<---------- T_CONN_RES
 *                       |     TS_DATA_XFER           |                      
 *                       |                            |                      
 *                       |                            |                      
 */
STATIC int
t_conn_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const struct T_conn_req *p = (struct T_conn_req *) mp->b_rptr;
	struct sockaddr_storage sa;
	uint16_t dport = 0;
	struct sockaddr_in *dsin = (struct sockaddr_in *) &sa;
	size_t dnum = 0, dlen;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state != TS_IDLE)
		goto outstate;
	sctp_t_setstate(sp, TS_WACK_CREQ);
	if (mp->b_cont && !msgdsize(mp->b_cont))
		goto baddata;
	if ((dlen = p->DEST_length)) {
		unsigned char *dptr = mp->b_rptr + p->DEST_offset;

		if (mp->b_wptr < dptr + dlen)
			goto badaddr;
		if (dlen < sizeof(struct sockaddr_in))
			goto badaddr;
		if (dlen > sizeof(struct sockaddr_storage))
			goto badaddr;
		if (dlen % sizeof(struct sockaddr_in))
			goto badaddr;
		dnum = dlen / sizeof(struct sockaddr_in);
		/* avoid alignment problems */
		bcopy(dptr, dsin, dlen);
		if (dsin->sin_family != AF_INET)
			goto badaddr;
		dport = dsin->sin_port;
	}
	if (!dport)
		goto badaddr;
	if ((err = t_parse_conn_opts(sp, mp->b_rptr + p->OPT_offset, p->OPT_length, 1)) < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto error;
		}
	}
#if 0
	/* allocate addresses now */
	if ((err = sctp_alloc_daddrs(sp, a->port, a->addr, anum)) < 0)
		goto error;
	/* address per-destination options */
	if (rto) {
		struct sctp_daddr
		*sd;
		t_sctp_rto_t *op = (t_sctp_rto_t *) rto->value;
		t_sctp_rto_t *oe = (t_sctp_rto_t *) (((caddr_t) rto) + rto->len);

		for (; op + 1 <= oe; op++) {
			if ((sd = sctp_find_daddr(sp, op->rto_dest))) {
				sd->rto = op->rto_initial;
				sd->rto_min = op->rto_min;
				sd->rto_max = op->rto_max;
				sd->max_retrans = op->max_retrans;
				continue;
			}
			err = TBADOPT;
			goto error;
		}
		if (op != oe)
			err = TBADOPT;
		if (err)
			goto error;
	}
	if (hb) {
		struct sctp_daddr
		*sd;
		t_sctp_hb_t *op = (t_sctp_hb_t *) hb->value;
		t_sctp_hb_t *oe = (t_sctp_hb_t *) (((caddr_t) hb) + hb->len);

		for (; op + 1 <= oe; op++) {
			if ((sd = sctp_find_daddr(sp, op->hb_dest))) {
				sd->hb_onoff = op->hb_onoff;
				sd->hb_itvl = op->hb_itvl;
				continue;
			}
			err = TBADOPT;
			goto error;
		}
		if (op != oe)
			err = TBADOPT;
		if (err)
			goto error;
	}
#endif
	if ((err = sctp_conn_req(sp, dport, dsin, dnum, mp->b_cont)))
		goto error;
	mp->b_cont = NULL;	/* absorbed mp->b_cont */
	return t_ok_ack(sp, T_CONN_REQ, NULL, NULL);
      acces:
	err = TACCES;
	sctplogrx(sp, "%s() no permission for address or option", __FUNCTION__);
	goto error;
      badaddr:
	err = TBADADDR;
	sctplogrx(sp, "%s() address is unusable", __FUNCTION__);
	goto error;
      badopt:
	err = TBADOPT;
	sctplogrx(sp, "%s() options are unusable", __FUNCTION__);
	goto error;
      baddata:
	err = TBADDATA;
	sctplogrx(sp, "%s() invalid amount of data", __FUNCTION__);
	goto error;
      einval:
	err = -EINVAL;
	sctplogrx(sp, "%s() invalid message format", __FUNCTION__);
	goto error;
      outstate:
	err = TOUTSTATE;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      error:
	sctplogrx(sp, "error %d", err);
	return t_error_ack(sp, mp, T_CONN_REQ, err);
}

/*
 *  T_CONN_RES           1 - Accept previous connection indication
 *  -----------------------------------------------------------------
 */
/*
 *  IMPLEMENTATION NOTE:- Sequence numbers are actually the address of the mblk which contains the
 *  COOKIE-ECHO chunk and contains the cookie as a connection indication.  To find if a particular
 *  sequence number is valid, we walk the connection indication queue looking for a mblk with the
 *  same address as the sequence number.  Sequence numbers are only valid on the stream for which
 *  the connection indication is queued.
 */
STATIC mblk_t *
t_seq_check(struct sctp *sp, t_uscalar_t seq)
{
	mblk_t *mp;
	pl_t pl;

	pl = bufq_lock(&sp->conq);
	for (mp = bufq_head(&sp->conq); mp && (t_uscalar_t) (long) mp != seq; mp = mp->b_next) ;
	if (mp)
		__bufq_unlink(&sp->conq, mp);
	bufq_unlock(&sp->conq, pl);
	usual(mp);
	/* leave list locked while reference held on message block */
	return (mp);
}

STATIC struct sctp *
t_tok_check(t_uscalar_t tok)
{
	struct sctp *ap;

	/* FIXME: must take list lock here! */
	for (ap = sctp_protolist; ap && (t_uscalar_t) (long) ap->rq != tok; ap = ap->next) ;
	/* FIXME: must really hold a reference on the ap structure */
	usual(ap);
	return (ap);
}
STATIC int
t_conn_res(struct sctp *sp, mblk_t *mp)
{
	int err = 0;
	mblk_t *cp = NULL;
	struct sctp *ap = NULL;
	const struct T_conn_res *p = (struct T_conn_res *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state != TS_WRES_CIND)
		goto outstate;
	sctp_t_setstate(sp, TS_WACK_CRES);
	if (mp->b_cont) {
		long mlen, mmax;

		mlen = msgdsize(mp->b_cont);
		if (((mmax = sp->pmtu - 16) > 0 && mlen > mmax) || mmax == T_INVALID
		    || ((mmax = 65535) > 0 && mlen > mmax) || mmax == T_INVALID)
			goto baddata;
	}
	if (p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
	if (!(cp = t_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if (!(ap = t_tok_check(p->ACCEPTOR_id)))
		goto badf;
	if (ap != sp && !(ap = sctp_trylockq(ap->rq)))
		goto badf;
	if (ap != sp && ((1 << ap->i_state) & ~TSM_DISCONN))
		goto badf;
	if (ap->i_state == TS_IDLE && ap->conind)
		goto resqlen;
	/* protect at least r00t streams from users */
	if (sp->cred.cr_uid != 0 && ap->cred.cr_uid != sp->cred.cr_uid)
		goto acces;
	if ((err = t_parse_conn_opts(ap, mp->b_rptr + p->OPT_offset, p->OPT_length, 0)) < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto error;
		}
	}
	{
		uint ap_oldstate = ap->i_state;

#if 0
		/* address per-destination options */
		if (rto) {
			struct sctp_daddr
			*sd;
			t_sctp_rto_t *op = (t_sctp_rto_t *) rto->value;
			t_sctp_rto_t *oe = (t_sctp_rto_t *) (((caddr_t) rto) + rto->len);

			for (; op + 1 <= oe; op++) {
				if ((sd = sctp_find_daddr(sp, op->rto_dest))) {
					sd->rto = op->rto_initial;
					sd->rto_min = op->rto_min;
					sd->rto_max = op->rto_max;
					sd->max_retrans = op->max_retrans;
					continue;
				}
				goto badopt;
			}
			if (op != oe)
				goto badopt;
			if (err)
				goto error;
		}
		if (hb) {
			struct sctp_daddr
			*sd;
			t_sctp_hb_t *op = (t_sctp_hb_t *) hb->value;
			t_sctp_hb_t *oe = (t_sctp_hb_t *) (((caddr_t) hb) + hb->len);

			for (; op + 1 <= oe; op++) {
				if ((sd = sctp_find_daddr(sp, op->hb_dest))) {
					sd->hb_onoff = op->hb_onoff;
					sd->hb_itvl = op->hb_itvl;
					continue;
				}
				goto badopt;
			}
			if (op != oe)
				goto badopt;
			if (err)
				goto error;
		}
#endif
		sctp_t_setstate(ap, TS_DATA_XFER);
		if ((err = sctp_conn_res(sp, cp, ap, mp->b_cont))) {
			sctp_t_setstate(ap, ap_oldstate);
			goto error;
		}
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		if ((err = t_ok_ack(sp, T_CONN_RES, cp, ap))) {
			sctp_t_setstate(ap, ap_oldstate);
			goto error;
		}
		if (ap != sp)
			sctp_unlockq(ap);
		return (QR_DONE);
	}
      acces:
	err = TACCES;
	sctplogrx(sp, "%s() no access to accepting queue", __FUNCTION__);
	goto error;
      resqlen:
	err = TRESQLEN;
	sctplogrx(sp, "%s() accepting queue is listening", __FUNCTION__);
	goto error;
      badf:
	err = TBADF;
	sctplogrx(sp, "%s() accepting queue id is invalid", __FUNCTION__);
	goto error;
      badseq:
	err = TBADSEQ;
	sctplogrx(sp, "%s() connection ind referenced is invalid", __FUNCTION__);
	goto error;
      badopt:
	err = TBADOPT;
	sctplogrx(sp, "%s() options are bad", __FUNCTION__);
	goto error;
      baddata:
	err = TBADDATA;
	sctplogrx(sp, "%s() invalid amount of data", __FUNCTION__);
	goto error;
      einval:
	err = -EINVAL;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      outstate:
	err = TOUTSTATE;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      error:
	if (ap && ap != sp)
		sctp_unlockq(ap);
	if (cp)
		bufq_queue(&sp->conq, cp);
	return t_error_ack(sp, mp, T_CONN_RES, err);
}

/*
 *  T_DISCON_REQ        2 - TC disconnection request
 *  -----------------------------------------------------------------
 *  The time-sequence diagrams for abortive disconnect looks like this:
 *
 *
 *  Refusing a connection indication:
 *
 *                       |                            |                      
 *                       |        COOKIE ECHO         |                      
 *  T_CONN_IND <---------|<---------------------------|                      
 *                       |     TS_WRES_CIND           |                      
 *  T_DISCON_REQ ------->|---+                        |                      
 *                       |   | TS_WACK_DREQ7          |                      
 *  T_ERROR_ACK <--------|<--+                        |                      
 *                       |     TS_WRES_CIND           |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |        COOKIE ECHO         |                      
 *  T_CONN_IND <---------|<---------------------------|                      
 *    (DATA)             |     TS_WRES_CIND           |                      
 *                       |                            |                      
 *                       |       DATA + ABORT         |                      
 *  T_DISCON_REQ ------->|---+----------------------->|--------> T_DISCON_IND
 *    (DATA)             |   | TS_WACK_DREQ7          |            (DATA)    
 *                       |   |                        |                      
 *  T_OK_ACK <-----------|<--+ TS_IDLE                |                      
 *                       |                            |                      
 *                       |                            |                      
 *
 *  Disconnecting an established connection:
 *
 *                       |                            |                      
 *  T_DISCON_REQ ------->|---+                        |                      
 *    (DATA)             |   |                        |                      
 *  T_ERROR_ACK <--------|<--+                        |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |        DATA + ABORT        |                      
 *  T_DISCON_REQ ------->|---+----------------------->|--------> T_DISCON_IND
 *    (DATA)             |   |                        |            (DATA)    
 *  T_OK_ACK <-----------|<--+                        |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |        DATA + ABORT        |                      
 *  T_DISCON_REQ ------->|---+-------------------+--->|--------> T_DATA_IND  
 *    (DATA)             |   |                   |    |                      
 *  T_OK_ACK <-----------|<--+                   +--->|--------> T_DISCON_IND
 *                       |                            |                      
 *
 *  Any data that is associated wtih the T_DISCON_REQ will be bundled as DATA chunks before the
 *  ABORT chunk in the mesage.  These DATA chunks will be sent out of order and unreliably on the
 *  default stream id and with the default payload protocol identifier in the hope that it makes it
 *  through to the other ULP before the ABORT chunk is processed.
 *
 */
STATIC int
t_discon_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	struct T_discon_req *p = (struct T_discon_req *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sp->i_state) & ~TSM_CONNECTED)
		goto outstate;
	switch (sp->i_state) {
	case TS_WCON_CREQ:
		sctp_t_setstate(sp, TS_WACK_DREQ6);
		break;
	case TS_WRES_CIND:
		sctp_t_setstate(sp, TS_WACK_DREQ7);
		break;
	case TS_DATA_XFER:
		sctp_t_setstate(sp, TS_WACK_DREQ9);
		break;
	case TS_WIND_ORDREL:
		sctp_t_setstate(sp, TS_WACK_DREQ10);
		break;
	case TS_WREQ_ORDREL:
		sctp_t_setstate(sp, TS_WACK_DREQ11);
		break;
	default:
		goto outstate;
	}
	if (mp->b_cont)
		goto baddata;
	if (sp->i_state == TS_WACK_DREQ7 && !(cp = t_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if ((err = sctp_discon_req(sp, cp)))
		goto error;
	if ((err = t_ok_ack(sp, T_DISCON_REQ, cp, NULL)))
		goto error;
	return (QR_DONE);
      badseq:
	err = TBADSEQ;
	sctplogrx(sp, "%s() connection ind reference is invalid", __FUNCTION__);
	goto error;
      baddata:
	err = TBADDATA;
	sctplogrx(sp, "%s() invalid amount of data", __FUNCTION__);
	goto error;
      einval:
	err = -EINVAL;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      outstate:
	err = TOUTSTATE;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      error:
	if (cp)
		bufq_queue(&sp->conq, cp);
	return t_error_ack(sp, mp, T_DISCON_REQ, err);
}

/**
 * t_write: - process M_DATA
 * @sp: private structure
 * @mp: the M_DATA message
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number indicating that the message is to be (re)queued.
 */
STATIC inline fastcall __hot_write int
t_write(struct sctp *sp, mblk_t *mp)
{
	int err;

	if (sp->i_state == TS_IDLE)
		goto discard;
	if ((1 << sp->i_state) & ~TSM_OUTDATA)
		goto outstate;
	if (!msgdsize(mp))
		goto emsgsize;
	{
		t_uscalar_t ppi = sp->ppi;
		t_uscalar_t sid = sp->sid;
		t_uscalar_t ord = 1;
		t_uscalar_t more = ((mp->b_flag & MSGDELIM) == 0);
		t_uscalar_t rcpt = 0;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)))
			goto provspec;
		return (QR_ABSORBED);
	}
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
      emsgsize:
	err = -EPROTO;
	sctplogrx(sp, "%s() bad amount of data", __FUNCTION__);
	goto error;
      outstate:
	err = -EPROTO;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      discard:
	err = 0;
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	goto error;
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

/**
 * t_data_req: - process T_DATA_REQ (3 - Connection-Mode data transfer request)
 * @sp: private structure
 * @mp: the T_DATA_REQ primitive
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number indicating that the message is to be (re)queued.
 */
STATIC inline fastcall __hot_write int
t_data_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const struct T_data_req *p = (struct T_data_req *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if ((1 << sp->i_state) & ~TSM_OUTDATA)
		goto outstate;
	if (!mp->b_cont || !msgdsize(mp->b_cont))
		goto emsgsize;
	{
		t_uscalar_t ppi = sp->ppi;
		t_uscalar_t sid = sp->sid;
		t_uscalar_t ord = 1;
		t_uscalar_t more = p->MORE_flag;
		t_uscalar_t rcpt = 0;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto provspec;
		freeb(mp);
		return (QR_ABSORBED);
	}
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
      emsgsize:
	err = -EPROTO;
	sctplogrx(sp, "%s() bad amount of data", __FUNCTION__);
	goto error;
      outstate:
	err = -EPROTO;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      einval:
	err = -EPROTO;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      discard:
	err = 0;
	sctplogrx(sp, "%s() ignore in idle state", __FUNCTION__);
	goto error;
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

/**
 * t_exdata_req: - process T_EXDATA_REQ (4 - Expedited data request)
 * @sp: private structure
 * @mp: the T_EXDATA_REQ message
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number indicating that the message is to be (re)queued.
 */
STATIC inline fastcall __hot_write int
t_exdata_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const struct T_exdata_req *p = (struct T_exdata_req *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if ((1 << sp->i_state) & ~TSM_OUTDATA)
		goto outstate;
	if (!mp->b_cont || !msgdsize(mp->b_cont))
		goto emsgsize;
	{
		t_uscalar_t ppi = sp->ppi;
		t_uscalar_t sid = sp->sid;
		t_uscalar_t ord = 0;
		t_uscalar_t more = p->MORE_flag;
		t_uscalar_t rcpt = 0;

		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto provspec;
		freeb(mp);
		return (QR_ABSORBED);
	}
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
      emsgsize:
	err = -EPROTO;
	sctplogrx(sp, "%s() bad amount of data", __FUNCTION__);
	goto error;
      outstate:
	err = -EPROTO;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      einval:
	err = -EPROTO;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      discard:
	err = 0;
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	goto error;
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

/*
 *  T_INFO_REQ           5 - Information Request
 *  -----------------------------------------------------------------
 */
STATIC int
t_info_req(struct sctp *sp, mblk_t *mp)
{
	(void) mp;
	return t_info_ack(sp);
}

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const struct T_bind_req *p = (struct T_bind_req *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->ADDR_length && (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)) {
		sctplogrx(sp,
			  "ADDR_offset(%u) or ADDR_length(%u) are incorrect", p->ADDR_offset,
			  p->ADDR_length);
		goto badaddr;
	}
	if (sp->i_state != TS_UNBND)
		goto outstate;
	sctp_t_setstate(sp, TS_WACK_BREQ);
	{
		struct sockaddr_storage sa;
		struct sockaddr_in *bsin = (struct sockaddr_in *) &sa;
		size_t bnum, blen = p->ADDR_length, boff = p->ADDR_offset;
		uint16_t bport;

		if (blen) {
			if (blen < sizeof(struct sockaddr_in))
				goto badaddr;
			if (blen > sizeof(struct sockaddr_storage))
				goto badaddr;
			bnum = blen / sizeof(struct sockaddr_in);
			if (blen != bnum * sizeof(struct sockaddr_in))
				goto badaddr;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + boff, bsin, blen);
			if (bsin->sin_family != AF_INET)
				goto badaddr;
			bport = bsin->sin_port;
		} else {
			bnum = 1;
			bport = 0;
			bsin->sin_family = AF_INET;
			bsin->sin_port = 0;
			bsin->sin_addr.s_addr = INADDR_ANY;
		}
		if ((err = sctp_bind_req(sp, bport, bsin, bnum, p->CONIND_number)))
			goto provspec;
		if ((err = t_bind_ack(sp)) == 0)
			return (QR_DONE);
		sctp_unbind_req(sp);
		return (err);
	}
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
#if 0
      acces:
	err = TACCES;
	sctplogrx(sp, "%s() no permission for requested address", __FUNCTION__);
	goto error;
      noaddr:
	err = TNOADDR;
	sctplogrx(sp, "%s() could not allocate address", __FUNCTION__);
	goto error;
#endif
      badaddr:
	err = TBADADDR;
	sctplogrx(sp, "%s() address is invalid", __FUNCTION__);
	goto error;
      einval:
	err = -EINVAL;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      outstate:
	err = TOUTSTATE;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      error:
	return t_error_ack(sp, mp, T_BIND_REQ, err);
}

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_unbind_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const struct T_unbind_req *p = (struct T_unbind_req *) mp->b_rptr;

	(void) p;
	if (sp->i_state != TS_IDLE)
		goto outstate;
	sctp_t_setstate(sp, TS_WACK_UREQ);
	if ((err = sctp_unbind_req(sp)))
		goto provspec;
	return t_ok_ack(sp, T_UNBIND_REQ, NULL, NULL);
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
      outstate:
	err = TOUTSTATE;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      error:
	return t_error_ack(sp, mp, T_UNBIND_REQ, err);
}

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------
 *  The T_OPTMGMT_REQ is responsible for establishing options while the stream is in the T_IDLE
 *  state.  When the stream is bound to a local address using the T_BIND_REQ, the settings of
 *  options with end-to-end significance will have an affect on how the driver response to an INIT
 *  with INIT-ACK for SCTP.  For example, the bound list of addresses is the list of addresses that
 *  will be sent in the INIT-ACK.  The number of inbound streams and outbound streams are the
 *  numbers that will be used in the INIT-ACK.
 */
/*
 *  Errors:
 *
 *  TACCES:     the user did not have proper permissions for the user of the requested options.
 *
 *  TBADFLAG:   the flags as specified were incorrect or invalid.
 *
 *  TBADOPT:    the options as specified were in an incorrect format, or they contained invalid
 *		information.
 *
 *  TOUTSTATE:  the primitive would place the transport interface out of state.
 *
 *  TNOTSUPPORT: this prmitive is not supported.
 *
 *  TSYSERR:    a system error has occured and the UNIX system error is indicated in the primitive.
 */
STATIC int
t_optmgmt_req(struct sctp *sp, mblk_t *mp)
{
	int err, opt_len;
	const struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->OPT_length && mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
#ifdef TS_WACK_OPTREQ
	if (sp->i_state == TS_IDLE)
		sctp_t_setstate(sp, TS_WACK_OPTREQ);
#endif
	switch (p->MGMT_flags) {
	case T_DEFAULT:
		opt_len = t_size_default_options(sp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CURRENT:
		opt_len = t_size_current_options(sp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_CHECK:
		opt_len = t_size_check_options(sp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	case T_NEGOTIATE:
		opt_len = t_size_negotiate_options(sp, mp->b_rptr + p->OPT_offset, p->OPT_length);
		break;
	default:
		goto badflag;
	}
	if (opt_len < 0) {
		switch (-(err = opt_len)) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		default:
			goto provspec;
		}
	}
	if ((err =
	     t_optmgmt_ack(sp, p->MGMT_flags, mp->b_rptr + p->OPT_offset, p->OPT_length,
			   opt_len)) < 0) {
		switch (-err) {
		case EINVAL:
			goto badopt;
		case EACCES:
			goto acces;
		case ENOBUFS:
		case ENOMEM:
			break;
		default:
			goto provspec;
		}
	}
	return (err);
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
      badflag:
	err = TBADFLAG;
	sctplogrx(sp, "%s() bad options flags", __FUNCTION__);
	goto error;
      acces:
	err = TACCES;
	sctplogrx(sp, "%s() no permission for options", __FUNCTION__);
	goto error;
      badopt:
	err = TBADOPT;
	sctplogrx(sp, "%s() bad options", __FUNCTION__);
	goto error;
      einval:
	err = -EINVAL;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      error:
	return t_error_ack(sp, mp, T_OPTMGMT_REQ, err);
}

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -------------------------------------------------------------------
 *
 *  The time-sequence diagrams look like this:
 *
 *                       |                            |                      
 *                  TS_DATA_XFER                 TS_DATA_XFER                
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |            DATA            |                      
 *  T_DATA_REQ --------->|--------------------------->|----------> T_DATA_IND
 *                       |                            |                      
 *  T_ORDREL_REQ ------->|<---------- SACK -----------|                      
 *                 TS_WIND_ORDREL                     |                      
 *                       |<---------- SACK -----------|                      
 *                       |<---------- SACK -----------|                      
 *                       |<---------- SACK -----------|                      
 *                       |         SHUTDOWN           |                      
 *                       |--------------------------->|--------> T_ORDREL_IND
 *                       |                      TS_WREQ_ORDREL               
 *                       |           DATA             |                      
 *  T_DATA_IND <---------|<---+-----------------------|<---------- T_DATA_REQ
 *                       |    |    SHUTDOWN           |                      
 *                       |    +---------------------->|                      
 *                       |           DATA             |                      
 *  T_DATA_IND <---------|<---+-----------------------|<---------- T_DATA_REQ
 *                       |    |    SHUTDOWN           |                      
 *                       |    +---------------------->|                      
 *                       |                            |                      
 *                       |       SHUTDOWN ACK         |                      
 *  T_ORDREL_IND <-------|<---+-----------------------|<-------- T_ORDREL_REQ
 *                    TS_IDLE |                    TS_IDLE                   
 *                       |    |SHUTDOWN COMPLETE      |                      
 *                       |    +---------------------->|                      
 *                       |                            |                      
 *  =====================|============================|======================
 *                       |                            |                      
 *  T_ORDREL_REQ ------->|                            |<-------- T_ORDREL_REQ
 *                 TS_WIND_ORDREL               TS_WIND_ORDREL               
 *                       |<---------- SACK -----------|                      
 *                       |----------- SACK ---------->|                      
 *                       |<---------- SACK -----------|                      
 *                       |----------- SACK ---------->|                      
 *                       |                            |                      
 *                       |<-------- SHUTDOWN ---------|                      
 *                       |--------- SHUTDOWN -------->|                      
 *  T_ORDREL_IND <-------|<--+--- SHUTDOWN ACK -------|                      
 *                       |---|--- SHUTDOWN ACK ---+-->|--------> T_ORDREL_IND
 *                       |   +- SHUTDOWN COMPLETE-|-->|                      
 *                       |<-----SHUTDOWN COMPLETE-+   |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 *                       |                            |                      
 */
STATIC int
t_ordrel_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((1 << sp->i_state) & ~TSM_OUTDATA)
		goto outstate;
	switch (sp->i_state) {
	case TS_DATA_XFER:
		sctp_t_setstate(sp, TS_WIND_ORDREL);
		break;
	case TS_WREQ_ORDREL:
		sctp_t_setstate(sp, TS_IDLE);
		break;
	}
	if (mp->b_cont)
		goto baddata;
	if ((err = sctp_ordrel_req(sp)))
		goto provspec;
	return (QR_DONE);
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
      baddata:
	err = -EPROTO;
	sctplogrx(sp, "%s() invalid amount of data", __FUNCTION__);
	goto error;
      einval:
	err = -EPROTO;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      outstate:
	err = -EPROTO;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

/**
 * t_optdata_req: - process T_OPTDATA_REQ (24- data with options request)
 * @sp: private structure
 * @mp: the T_OPTDATA_REQ primitive
 *
 * Basically the purpose of this for SCTP is to be able to set the SCTP Unordered Bit (U-bit),
 * Payload Protocol Identifier (PPI) and Stream Id (sid) associated with the data message.  If not
 * specified, each option will reduce to the current default settings.
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number indicating that the message is to be (re)queued.
 */
STATIC inline fastcall __hot_write int
t_optdata_req(struct sctp *sp, mblk_t *mp)
{
	int err;
	const struct T_optdata_req *p = (struct T_optdata_req *) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if ((p->OPT_length && (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)))
		goto einval;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if ((1 << sp->i_state) & ~TSM_OUTDATA)
		goto outstate;
	if (!mp->b_cont || !msgdsize(mp->b_cont))
		goto emsgsize;
	{
		t_uscalar_t ppi = sp->ppi;
		t_uscalar_t sid = sp->sid;
		t_uscalar_t ord = !(p->DATA_flag & T_ODF_EX);
		t_uscalar_t more = (p->DATA_flag & T_ODF_MORE);
		t_uscalar_t rcpt = 0;

		if (p->OPT_length) {
			unsigned char *op = mp->b_rptr + p->OPT_offset;
			int olen = p->OPT_length;
			struct t_opthdr *oh;

			for (oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0);
			     oh; oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0)) {
				if (oh->len < sizeof(*oh))
					break;
				if (oh->level == T_INET_SCTP)
					switch (oh->name) {
					case T_SCTP_SID:
						sid = (*((t_scalar_t *) (oh + 1))) & 0xffff;
						continue;
					case T_SCTP_PPI:
						ppi = (*((t_scalar_t *) (oh + 1))) & 0xffffffff;
						continue;
					}
			}
		}
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto provspec;
		freeb(mp);
		return (QR_ABSORBED);
	}
      provspec:
	err = err;
	sctplogrx(sp, "%s() provider specific %d", __FUNCTION__, err);
	goto error;
#if 0
      badopt:
	err = -EPROTO;
	sctplogrx(sp, "%s() bad options", __FUNCTION__);
	goto error;
      acces:
	err = -EPROTO;
	sctplogrx(sp, "%s() no permission to options", __FUNCTION__);
	goto error;
#endif
      emsgsize:
	err = -EPROTO;
	sctplogrx(sp, "%s() bad amount of data", __FUNCTION__);
	goto error;
      outstate:
	err = -EPROTO;
	sctplogrx(sp, "%s() would place interface out of state %d",
		  __FUNCTION__, (int) sp->i_state);
	goto error;
      einval:
	err = -EPROTO;
	sctplogrx(sp, "%s() invalid primitive format", __FUNCTION__);
	goto error;
      discard:
	err = 0;
	sctplogrx(sp, "%s() ignored in idle state", __FUNCTION__);
	goto error;		/* ignore the idle state */
#if 0
      notsupport:
	sctplogrx(sp, "%s() primitive not support for T_CLTS", __FUNCTION__);
	err = -EPROTO;
	goto error;
#endif
      error:
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, err);
	return m_error_reply(sp, mp, err);
}

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - address request
 *  -------------------------------------------------------------------
 */
STATIC int
t_addr_req(struct sctp *sp, mblk_t *mp)
{
	(void) mp;
	return t_addr_ack(sp);
}
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
STATIC int
t_capability_req(struct sctp *sp, mblk_t *mp)
{
	int err = -EFAULT;
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	return t_capability_ack(sp, p->CAP_bits1, mp->b_datap->db_type);
      einval:
	err = -EINVAL;
	sctplogrx(sp, "invalid message format");
	goto error;
      error:
	return t_error_ack(sp, mp, T_CAPABILITY_REQ, err);
}
#endif

/**
 * t_other_req: - process unsupported primitives
 * @sp: private structure
 * @mp: the unsupported primitive
 */
STATIC int
t_other_req(struct sctp *sp, mblk_t *mp)
{
	t_uscalar_t prim = *((t_uscalar_t *) mp->b_rptr);

	return t_error_ack(sp, mp, prim, TNOTSUPPORT);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

/**
 * sctp_t_w_proto_return: - handle M_PROTO and M_PCPROTO processing return value
 * @mp: the M_PROTO or M_PCPROTO message
 * @rtn: the return value
 *
 * Some M_PROTO and M_PCPROTO processing functions return an error just to cause the primary state
 * of the endpoint to be restored.  Also, some functions may still return QR_DONE.  These return
 * values are processed here.
 */
noinline fastcall int
sctp_t_w_proto_return(mblk_t *mp, int rtn)
{
	switch (rtn) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
		return (rtn);
	default:
	case QR_DONE:
		freemsg(mp);
	case QR_ABSORBED:
		return (QR_ABSORBED);
	}
}

/**
 * sctp_t_w_proto_slow: process M_PROTO or M_PCPROTO primitive
 * @sp: private structure (locked)
 * @mp: the M_PROTO or M_PCPROTO primitive
 * @prim: primitive
 *
 * This function either returns QR_ABSORBED when the message has been consumed, or a negative error
 * number when the message is to be (re)queued.  This function must be called with the private
 * structure locked.
 */
noinline fastcall int
sctp_t_w_proto_slow(struct sctp *sp, mblk_t *mp, t_scalar_t prim)
{
	t_uscalar_t oldstate = sp->i_state;
	int rtn;

	switch (prim) {
	case T_CONN_REQ:
		sctplogrx(sp, "-> T_CONN_REQ");
		rtn = t_conn_req(sp, mp);
		break;
	case T_CONN_RES:
		sctplogrx(sp, "-> T_CONN_RES");
		rtn = t_conn_res(sp, mp);
		break;
	case T_DISCON_REQ:
		sctplogrx(sp, "-> T_DISCON_REQ");
		rtn = t_discon_req(sp, mp);
		break;
	case T_DATA_REQ:
		sctplogda(sp, "-> T_DATA_REQ");
		rtn = t_data_req(sp, mp);
		break;
	case T_EXDATA_REQ:
		sctplogda(sp, "-> T_EXDATA_REQ");
		rtn = t_exdata_req(sp, mp);
		break;
	case T_INFO_REQ:
		sctplogrx(sp, "-> T_INFO_REQ");
		rtn = t_info_req(sp, mp);
		break;
	case T_BIND_REQ:
		sctplogrx(sp, "-> T_BIND_REQ");
		rtn = t_bind_req(sp, mp);
		break;
	case T_UNBIND_REQ:
		sctplogrx(sp, "-> T_UNBIND_REQ");
		rtn = t_unbind_req(sp, mp);
		break;
	case T_OPTMGMT_REQ:
		sctplogrx(sp, "-> T_OPTMGMT_REQ");
		rtn = t_optmgmt_req(sp, mp);
		break;
	case T_ORDREL_REQ:
		sctplogrx(sp, "-> T_ORDREL_REQ");
		rtn = t_ordrel_req(sp, mp);
		break;
	case T_OPTDATA_REQ:
		sctplogda(sp, "-> T_OPTDATA_REQ");
		rtn = t_optdata_req(sp, mp);
		break;
#ifdef T_ADDR_REQ
	case T_ADDR_REQ:
		sctplogrx(sp, "-> T_ADDR_REQ");
		rtn = t_addr_req(sp, mp);
		break;
#endif
#ifdef T_CAPABILITY_REQ
	case T_CAPABILITY_REQ:
		sctplogrx(sp, "-> T_CAPABILITY_REQ");
		rtn = t_capability_req(sp, mp);
		break;
#endif
	case T_UNITDATA_REQ:
		sctplogda(sp, "-> T_UNITDATA_REQ");
		sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, -EPROTO);
		rtn = m_error_reply(sp, mp, -EPROTO);
		/* FIXME: we should really handle T_UNITDATA_REQ even though we are a
		   connection-oriented protocol. */
		break;
	case T_CONN_IND:
	case T_CONN_CON:
	case T_DISCON_IND:
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_INFO_ACK:
	case T_BIND_ACK:
	case T_ERROR_ACK:
	case T_OK_ACK:
	case T_UNITDATA_IND:
	case T_UDERROR_IND:
	case T_OPTMGMT_ACK:
	case T_ORDREL_IND:
	case T_OPTDATA_IND:
	case T_ADDR_ACK:
#ifdef T_CAPABILITY_ACK
	case T_CAPABILITY_ACK:
#endif
		sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, -EPROTO);
		rtn = m_error_reply(sp, mp, -EPROTO);
		break;
	default:
		sctplogrx(sp, "-> T_????_???");
		rtn = t_other_req(sp, mp);
		break;
	}
	if (rtn < 0)
		sctp_t_setstate(sp, oldstate);
	/* The put and srv procedures do not recognize all errors.  Sometimes we return an error to 
	   here just to restore the previous state. */
	return sctp_t_w_proto_return(mp, rtn);
}

/**
 * __sctp_t_w_proto: - process M_PROTO or M_PCPROTO message locked
 * @sp: private structure (locked)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locked version is for use by the service procedure which takes private structure locks once
 * for the entire service procedure run.
 */
STATIC inline fastcall __hot_write int
__sctp_t_w_proto(struct sctp *sp, mblk_t *mp)
{
	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
		t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;
		t_scalar_t oldstate = sp->i_state;
		int rtn;

		if (likely(prim == T_OPTDATA_REQ)) {
			if (likely((rtn = t_optdata_req(sp, mp)) == QR_ABSORBED))
				return (QR_ABSORBED);
		} else if (likely(prim == T_DATA_REQ)) {
			if (likely((rtn = t_data_req(sp, mp)) == QR_ABSORBED))
				return (QR_ABSORBED);
		} else if (likely(prim == T_EXDATA_REQ)) {
			if (likely((rtn = t_exdata_req(sp, mp)) == QR_ABSORBED))
				return (QR_ABSORBED);
		} else
			return sctp_t_w_proto_slow(sp, mp, prim);

		if (rtn < 0)
			sctp_t_setstate(sp, oldstate);
		return sctp_t_w_proto_return(mp, rtn);
	}
	sctplogrx(sp, "%s() replying with error %d", __FUNCTION__, -EPROTO);
	return m_error_reply(sp, mp, -EPROTO);
}

/**
 * sctp_t_w_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 *
 * This locking version is for use by the put procedure which does not take locks.
 */
STATIC inline fastcall __hot_write int
sctp_t_w_proto(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(t_scalar_t))) {
		t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;
		struct sctp *sp;

		if (likely(prim == T_OPTDATA_REQ))
			return (-EAGAIN);
		else if (likely(prim == T_DATA_REQ))
			return (-EAGAIN);
		else if (likely(prim == T_EXDATA_REQ))
			return (-EAGAIN);

		if (likely((sp = sctp_trylockq(q)) != NULL)) {
			int rtn;

			if ((rtn = sctp_t_w_proto_slow(sp, mp, prim)) == QR_ABSORBED)
				___sctp_transmit_wakeup(sp);
			sctp_unlockq(sp);
			return (rtn);
		}
		return (-EDEADLK);
	}
	sctplogrx(SCTP_PRIV(q), "%s() replying with error %d", __FUNCTION__, -EPROTO);
	return m_error_reply(SCTP_PRIV(q), mp, -EPROTO);
}

/**
 * __sctp_t_w_data: - process M_DATA message
 * @sp: private structure (locked)
 * @mp: the M_DATA message
 *
 * This function either returns QR_ABSORBED when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This non-locking version is used by the service procedure.
 */
STATIC inline fastcall __hot_write int
__sctp_t_w_data(struct sctp *sp, mblk_t *mp)
{
	return t_write(sp, mp);
}

STATIC inline fastcall __hot_write int
__sctp_t_w_data_put(struct sctp *sp, mblk_t *mp)
{
	return (-EAGAIN);
}

/**
 * sctp_t_w_data: - process M_DATA messages
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 *
 * This function either returns QR_ABSORBED when the message is consumed, or a negative error number
 * when the message is to be (re)queued.  This locking version is used by the put procedure.
 */
STATIC inline fastcall __hot_write int
sctp_t_w_data(queue_t *q, mblk_t *mp)
{
#if 0
	struct sctp *sp;

	if (likely((sp = sctp_trylockq(q)) != NULL)) {
		int rtn;

		if ((rtn = __sctp_t_w_data_put(sp, mp)) == QR_ABSORBED)
			___sctp_transmit_wakeup(sp);
		sctp_unlockq(sp);
		return (rtn);
	}
	return (-EDEADLK);
#else
	return (-EAGAIN);
#endif
}

/**
 * sctp_t_w_other: - other messages on write queue
 * @q: active queue (write queue)
 * @mp: the unexpected message
 *
 * If SCTP is pushed as a module, pass unexpected messages along with flow control.  If a driver,
 * respond to some and discard the rest.  This function is called with the private structure
 * unlocked.  It returns either -%EBUSY if SCTP is a module and flow control prevents passing the
 * message, or QR_ABSORBED in all other cases.
 */
noinline fastcall __unlikely int
sctp_t_w_other(queue_t *q, mblk_t *mp)
{
	struct sctp *sp = SCTP_PRIV(q);

	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	if (q->q_next == NULL) {
		switch (mp->b_datap->db_type) {
		case M_IOCTL:
		case M_IOCDATA:
			/* FIXME: nak these instead of discarding */
			break;
		}
		freemsg(mp);
	} else {
		if (mp->b_datap->db_type < QPCTL && !bcanputnext(q, mp->b_band))
			return (-EBUSY);
		putnext(q, mp);
	}
	return (QR_ABSORBED);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */

/**
 * sctp_t_w_prim_put: - put a TPI primitive
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the fast path for the TPI write put procedure.  Data is simply queued by returning
 * -%EAGAIN.  This provides better flow control and scheduling for maximum throughput.
 */
STATIC inline fastcall __hot_write int
sctp_t_w_prim_put(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sctp_t_w_proto(q, mp);
	case M_DATA:
		return sctp_t_w_data(q, mp);
	case M_FLUSH:
		return sctp_w_flush(q, mp);
	default:
		return sctp_t_w_other(q, mp);
	}
}

/**
 * sctp_t_wput: - TPI write put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 * 
 * This is a canonical put procedure for TPI  write.  Locking is performed by the individual message
 * handling procedures.
 */
STATIC streamscall __hot_write int
sctp_t_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(sctp_t_w_prim_put(q, mp) != QR_ABSORBED)) {
		// q->q_qinfo->qi_minfo->mi_mstat->ms_acnt++;
		/* apply backpressure */
		mp->b_wptr += PRELOAD;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* must succeed */
		}
	}
	return (0);
}

/**
 * sctp_t_w_prim_srv: - service a TPI primitive
 * @sp: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This is the fast path for the TPI write service procedure.  Data is processed.  Processing data
 * from the write service prcedure provides better flow control and scheduling for maximum
 * throughput.  Note that out-of-order SCTP data is issued as N_EXDATA_REQ.
 */
STATIC inline fastcall __hot_out int
sctp_t_w_prim_srv(struct sctp *sp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __sctp_t_w_proto(sp, mp);
	case M_DATA:
		return __sctp_t_w_data(sp, mp);
	case M_FLUSH:
		return sctp_w_flush(q, mp);
	default:
		return sctp_t_w_other(q, mp);
	}
}

/**
 * sctp_t_wsrv: - TPI write service procedure
 * @q: active queue (write queue)
 *
 * This is a canonical service procedure for TPI write.  Locking is performed outside the loop so
 * that locks do not need to be released and acquired with each loop.  Note that the wakeup function
 * must also be executed with the private structure locked.
 */
STATIC streamscall __hot_out int
sctp_t_wsrv(queue_t *q)
{
	struct sctp *sp;

	if (likely((sp = sctp_trylockq(q)) != NULL)) {
		mblk_t *mp;
		int rtn;

		while (likely((mp = getq(q)) != NULL)) {
			/* remove backpressure */
			mp->b_wptr -= PRELOAD;
			if (unlikely((rtn = sctp_t_w_prim_srv(sp, q, mp)) != QR_ABSORBED)) {
				/* reapply backpressure */
				mp->b_wptr += PRELOAD;
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				sctplogtx(sp, "write queue stalled %d", rtn);
				break;
			}
		}
		___sctp_transmit_wakeup(sp);
		sctp_unlockq(sp);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC streamscall int
sctp_t_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	struct sctp *sp, **spp = &sctp_protolist;
	int err = 0;
	mblk_t *mp;
	struct stroptions *so;
	unsigned long flags;

	(void) crp;
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (EIO);	/* can't open as module */
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN)
		cminor = 1;
	if (!(mp = allocb(sizeof(*so), BPRI_WAITOK)))
		return (ENOBUFS);
	spin_lock_irqsave(&sctp_protolock, flags);
	for (; *spp && (*spp)->cmajor < cmajor; spp = &(*spp)->next) ;
	for (; *spp && cminor <= NMINORS; spp = &(*spp)->next) {
		ushort dminor = (*spp)->cminor;

		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag != CLONEOPEN)
				goto enxio;	/* requested device in use */
			cminor++;
		}
	}
	if (cminor > NMINORS)
		goto enxio;
	*devp = makedevice(cmajor, cminor);
	if (!(sp = sctp_alloc_priv(q, spp, cmajor, cminor, &t_ops, crp)))
		goto enomem;
      unlock_exit:
	spin_unlock_irqrestore(&sctp_protolock, flags);
	if (err == 0) {
		so = (typeof(so)) mp->b_wptr;
		bzero(so, sizeof(*so));
		so->so_flags |= SO_HIWAT;
		so->so_hiwat = SHEADHIWAT << 1;
		so->so_flags |= SO_LOWAT;
		so->so_lowat = SHEADLOWAT << 1;
		mp->b_wptr += sizeof(*so);
		mp->b_datap->db_type = M_SETOPTS;
		putnext(q, mp);
	} else {
		freemsg(mp);
	}
	return (err);
      enxio:
	rare();
	err = ENXIO;
	goto unlock_exit;
      enomem:
	rare();
	err = ENOMEM;
	goto unlock_exit;
}
STATIC streamscall int
sctp_t_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sctp_free_priv(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

#ifdef LINUX
unsigned short t_modid = SCTP_T_DRV_ID;

#ifndef module_param
MODULE_PARM(t_modid, "h");
#else
module_param(t_modid, ushort, 0444);
#endif
MODULE_PARM_DESC(t_modid, "Module ID number for STREAMS SCTP TPI driver (0 for allocation).");

major_t t_major = SCTP_T_CMAJOR_0;

#ifndef module_param
MODULE_PARM(t_major, "h");
#else
module_param(t_major, uint, 0444);
#endif
MODULE_PARM_DESC(t_major, "Major device number for STREAMS SCTP TPI driver (0 for allocation).");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(SCTP_T_DRV_ID));
MODULE_ALIAS("streams-major-" __stringify(SCTP_T_CMAJOR_0));
MODULE_ALIAS("/dev/streams/sctp_t");
MODULE_ALIAS("/dev/streams/sctp_t/*");
MODULE_ALIAS("/dev/streams/clone/sctp_t");
#endif
MODULE_ALIAS("char-major-" __stringify(SCTP_T_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(SCTP_T_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(SCTP_T_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/sctp_t");
#endif				/* MODULE_ALIAS */

#endif				/* LINUX */

#ifdef LFS
/*
 *  =========================================================================
 *
 *  LfS Module Initialization
 *
 *  =========================================================================
 *  YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY
 *  YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY
 *  YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY
 *  YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY
 *  YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY
 */
STATIC struct cdevsw sctp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sctp_t_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};
STATIC int
sctp_t_init(void)
{
	int err;

	if ((err = register_strdev(&sctp_cdev, t_major)) < 0)
		return (err);
	if (t_major == 0)
		t_major = err;
	return (0);
}
STATIC void
sctp_t_term(void)
{
	unregister_strdev(&sctp_cdev, t_major);
}
#endif				/* LFS */

#ifdef LIS
/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 *  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
 *  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
 *  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
 *  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
 *  ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
 */
STATIC void
sctp_t_init(void)
{
	int err;

	if ((err = lis_register_strdev(t_major, &sctp_t_info, UNITS, DRV_NAME)) < 0) {
		cmn_err(CE_PANIC, "%s: cannot register driver\n", DRV_NAME);
		return;
	}
	if (!t_major && err > 0)
		t_major = err;
	if ((err = lis_register_module_qlock_option(t_major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(t_major);
		cmn_err(CE_PANIC, "%s: cannot register driver\n", DRV_NAME);
		return;
	}
	return;
}
STATIC void
sctp_t_term(void)
{
	int err;

	if ((err = lis_unregister_strdev(t_major))) {
		cmn_err(CE_PANIC, "%s: cannot unregister driver!\n", DRV_NAME);
	}
};
#endif				/* LIS */

/*
 *  QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 */

/*
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  Sockets Interface
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *  KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK
 *
 *  --------------------------------------------------------------------------
 *
 *  SCTP CLOSE
 *
 *  --------------------------------------------------------------------------
 *
 *  This is the close(2) call being peformed on the socket.  This will either result in the orderly
 *  shutdown of the SCTP association (if no data is left unread), or in the abort of the SCTP
 *  association (if data has been left unread on the socket).
 *
 *  Unlike TCP, we have no partially open sockets, really.  Partially open sockets have a state
 *  cookie in flight which can be ignored upon cookie echo.
 *
 *  SCTP LISTEN STOP
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for stopping a listening socket.  This breaks user locks, removes the listening socket
 *  from all the hashes (including bind), and orphans, disconnects and destroys each of the child
 *  sockets in the accept queue.
 */

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP CONNECT
 *
 *  ---------------------------------------------------------------------------
 *  This is the socket connect(2) call.  It directly calls the generic sctp connection request
 *  function sctp_conn_req().  Multiple addresses can be specified in uaddr with an array of
 *  sockaddr_in structures.  The address family and port number are only significant on the first
 *  sockaddr_in in the array.  Further elements contain only IP addresses.
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP DISCONNECT
 *
 *  -------------------------------------------------------------------------
 *  Note: Although we are disconnecting there may be other processes waiting in wait queues if the
 *  socket is not orphaned.  This is because one can now disconnect in linux using the AF_UNSPEC
 *  approach, which allows the socket to be reused without closing and reopening the socket.
 *
 *  This function is called in three circumstances by inet socket code: when connect(2) is called
 *  with an address family of AF_UNSPEC; when connect(2) fails after calling the protocol specific
 *  connect function; when shutdown(2) is called on a outgoing connecting socket.  In the former two
 *  cases, disconnect is called with the socket file flags in the flags argument.  In the latter
 *  case, it is always called with the O_NONBLOCK flag.
 */

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP ACCEPT
 *
 *  ---------------------------------------------------------------------------
 *  This is the accept(2) call being performed on the socket.  This will result the the waiting for
 *  acceptance of an incoming SCTP association.  This is only performed after a bind and a listen.
 *  The socket must be in the listening state.  We do not autobind any longer.
 *
 *  Unlike TCP, we don't keep half-open sockets on the accept list.  This is because we do not keep
 *  a record of half-open sockets in SCTP, all this state information is contained in the state
 *  cookie.  The only sockets on our sp->accept_next list are sockets for which a COOKIE ACK was
 *  sent and are fully formed sockets.
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP IOCTL
 *
 *  --------------------------------------------------------------------------
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP INIT
 *
 *  --------------------------------------------------------------------------
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP DESTROY
 *
 *  --------------------------------------------------------------------------
 */

/*
 *  SCTP DESTROY OPHANED SOCKET
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for destroying an orphaned socket.  Orphaned sockets will never have their
 *  sk->prot->destroy functions called by socket code because they have been orphaned from the
 *  socket.  This function calls the destructor for the socket, drains socket queues, and does the
 *  final put on the socket.
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP SHUTDOWN
 *
 *  --------------------------------------------------------------------------
 *  We can shutdown the sending side of an SCTP association just by marking setting shutdown and the
 *  socket code will keep the application from sending on the socket.  The receive can only be
 *  shutdown if both sides are shut down for SCTP (unlike TCP's half-close).
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP SETSOCKOPT
 *
 *  --------------------------------------------------------------------------
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP GETSOCKOPT
 *
 *  --------------------------------------------------------------------------
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP SENDMSG
 *
 *  --------------------------------------------------------------------------
 *  This copies from a user buffer into a socket and starts transmission.  The buffer is copied and
 *  partial checksummed at the same time for maximum speed.  Only a zero-copy sk_buff system could
 *  do better.  I hear one is coming.
 *
 *  SCTP permits TCP compatible SOCK_STREAM operation with the appropriate configuration options
 *  set.  Otherwise, SCTP uses SOCK_SEQPACKET operation.
 *
 */

/*
 *  WAIT FOR CONNECT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This waitq waits for a socket to become connected.  This function permits sending on a socket
 *  immediately after the connect() call but waits until the socket enters the established state
 *  before
 */

/*
 *  WAIT FOR WRITE MEMORY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This waitq waits for a socket to have available memory to which to write new data.  This is
 *  congestion flow control.
 */

/*
 *  SCTP SENDMSG
 *  -----------------------------------
 *  Note: we could probably accept one packet in the opening state so that we can add it to the
 *  outbound COOKIE-ECHO message.  That would be cool.
 */
/*
 *  IMPLEMENTATION NOTE:- I have set sctp_sendmsg() to handle having addresses provided in the same
 *  fashion as udp_sendmsg() as long as the address is one of the destination addresses available to
 *  the association.  This permits the user to specify the destination address of a specific data
 *  message.
 */
/*
 *  IMPLEMENTATION NOTE:-  The user can specify the destination address (within those available to
 *  the association), the source address (within those available to the association), and/or the
 *  interface upon which to send the message.  However, for SOCK_STREAM sockets it will then not be
 *  possible to concatenate these bytes into other DATA chunks, and for SOCK_SEQPACKET and SOCK_RDM
 *  sockets it will not be possible to bundle these DATA chunks with DATA chunks for other
 *  destinations.
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP RECVMSG
 *
 *  --------------------------------------------------------------------------
 *  Handle the recvmsg(2) call to an SCTP socket.  The special considerations here are the handling
 *  of urgent data (which we interpret as out-of-order delivery data) and the reception of
 *  stream-specific data.
 *
 */

/*
 *  SCTP RECVMSG
 *  -------------------------------------------------------------------------
 *  This function handles datagram semantics (SOCK_RDM, SOCK_SEQPACKET) as well as stream semantics
 *  (SOCK_STREAM).  SOCK_RDM and SOCK_SEQPACKET sockets will have MSG_EOR to delimit messages;
 *  MSG_TRUNC will be set if a data chunk cannot be fit into the supplied buffer, but data beyond
 *  the supplied buffer will not be discarded unless it is a SOCK_RDM socket.  SOCK_RDM and
 *  SOCK_SEQPACKET can only read one data chunk at a time, whereas SOCK_STREAM sockets can read as
 *  much data as possible and can use the MSG_WAITALL flag.
 *
 *  SCTP IG 2.20 is not applicable.
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP BIND
 *
 *  --------------------------------------------------------------------------
 *  IMPLEMENTATION NOTES:- There are some interesting differences between the SCTP bind function and
 *  the TCP bind function.  Multiple addresses can be provided (by increasing addr_len) in the (now)
 *  array pointed to by struct sockaddr.  For SCTP when we do a bind, we want to bind to a number of
 *  source addresses.  When binding to INADDR_ANY, we want to use the entire list of local addresses
 *  available on this host; however, we delay determining that list until either a listen() or
 *  connect() is performed on the socket.  Binding to AF_UNSPEC results in an unbind.  This is
 *  likely the only way to rebind the stream.
 */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP BACKLOG RCV
 *
 *  ---------------------------------------------------------------------------
 *  This is the main receive hook for processing SCTP packets which are either processed directly or
 *  queued in a backlog queue against a socket.  All these packets have a socket associated with
 *  them.  Out of the blue packets are handled separately in the bottom half.  If the Berkeley
 *  Packet Filter is configured against the socket, we also do packet filtering.
 */

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP HASH
 *
 *  ---------------------------------------------------------------------------
 *  This function is never called by the common inet socket code.  Ala TCP, we hash as required
 *  elsewhere.
 */

/*
 *  =========================================================================
 *  -------------------------------------------------------------------------
 *  SCTP Interface
 *  -------------------------------------------------------------------------
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  SCTP S-Provider --> S-User Primitives (Indication, Confirmation and Ack)
 *
 *  =========================================================================
 *
 *  S_CONN_IND		- connection indication
 *  -----------------------------------------------------------------
 *  We get the connection indication information from the cookie received in the COOKIE ECHO which
 *  invokes the indication.  (We queue the COOKIE ECHO chunks themselves as indications.)
 */

/*
 *  S_CONN_CON		- connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated are the number of inbound and
 *  outbound streams.
 */

/*
 *  S_DATA_IND		- data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default socket.  The
 *  caller should check that ppi and sid match the default before using this indication.  Otherwise
 *  the caller should use S_OPTDATA_IND.  Well, for sockets we always use the equivalent of
 *  S_OPTDATA_IND.  This just indicates availability of data to the user under sockets and whether
 *  to include ancilliary information is made when the data is actually read with recvmsg(2).
 */

/*
 *  S_EXDATA_IND	- expedited data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default socket.  The
 *  caller should check that ppi and sid match the default before using this indication.  Otherwise
 *  the caller should use S_OPTDATA_IND.  Well, for sockets we always use the equivalent of
 *  S_OPTDATA_IND.  This just indicates availability of data to the user under sockets and whether
 *  to include ancilliary information is made when the data is actually read with recvmsg(2).
 */

/*
 *  S_DATACK_IND	- data acknolwedgement indication
 *  -----------------------------------------------------------------
 */

/*
 *  S_RESET_IND		- reset indication
 *  -----------------------------------------------------------------
 */

/*
 *  S_RESET_CON		- reset confirmation
 *  -----------------------------------------------------------------
 */

/*
 *  S_DISCON_IND	- disconnect indication
 *  -----------------------------------------------------------------
 *  We use the address of the socket buffer that contains the COOKIE-ECHO chunk as a identifier for
 *  connection indications that are rejected with a diconnect indication as well.  We can use this
 *  to directly address the socket buffer in the connection indication queue.
 *
 *  If the caller provides diconnect data, the caller needs to set the current ord, ppi, sid, and
 *  ssn fields so that the user can examine them with getsockopt(2) if it has a need to known them.
 *
 *  If a connection indication identifier (seq) is provided, this function must either unlink and
 *  free it or return an error.  The origin is used only by the STREAMS NPI interface.
 */
/*
 *  S_ORDREL_IND	- orderly release indication
 *  -----------------------------------------------------------------
 */
/*
 *  S_RETR_IND		- retrieval indication
 *  -----------------------------------------------------------------
 */
/*
 *  S_RETR_CON		- retrieval confirmation
 *  -----------------------------------------------------------------
 */
/*
 *  NOTES:- Sockets and XTI/TPI cannot do data acknowledgements, resets or retrieval directly.  Data
 *  acknowledgements and retrieval are different forms of the same service.  For data
 *  acknowledgement, use the STREAMS NPI interface.  For reset support (SCTP Restart indication
 *  different from SCTP CDI), use the STREAMS NPI interface.
 */
/*
 *  ==========================================================================
 *
 *  IP PROTOCOL INTERFACE
 *
 *  ==========================================================================
 */

/**
 * sctp_rcv_ootb: - receive out-of-the-blue message
 * @mp: the message
 *
 * Care must be taken because we are still executing at network bottom-half here.
 */
STATIC void
sctp_rcv_ootb(mblk_t *mp)
{
	struct iphdr *iph = SCTP_IPH(mp);
	struct sctphdr *sh = SCTP_SH(mp);
	struct sctpchdr *ch = SCTP_CH(mp);
	int sat = inet_addr_type(iph->saddr);
	struct sctp *sp;

	if (sat != RTN_UNICAST && sat != RTN_LOCAL) {
		/* RFC 2960 8.4(1). */
		return;
	}
#if 0
	/* We shouldn't have to range check the chunk header here because it the first chunk header 
	   in an ootb package is already checked by the bottom end receive function. */
	{
		size_t clen;

		ch = (typeof(ch)) mp->b_rptr;
		if (sizeof(*ch) > skb->len || (clen = ntohs(ch->len)) < sizeof(*ch)
		    || PADC(clen) + mp->b_rptr > mp->b_wptr) {
			return;
		}
	}
#endif
	switch (ch->type) {
	case SCTP_CTYPE_COOKIE_ACK:	/* RFC 2960 8.4(7). */
	case SCTP_CTYPE_ERROR:	/* RFC 2960 8.4(7). */
	case SCTP_CTYPE_ABORT:	/* RFC 2960 8.4(2). */
	case SCTP_CTYPE_SHUTDOWN_COMPLETE:	/* RFC 2960 8.4(6). */
	case SCTP_CTYPE_INIT:	/* RFC 2960 8.4(3) and (8). */
	case SCTP_CTYPE_INIT_ACK:	/* RFC 2960 8.4(8). */
	default:
		switch (ch->type) {
		case SCTP_CTYPE_COOKIE_ACK:
			printd(("Received OOTB COOKIE-ACK\n"));
			break;
		case SCTP_CTYPE_ERROR:
			printd(("Received OOTB ERROR\n"));
			break;
		case SCTP_CTYPE_ABORT:
			printd(("Received OOTB ABORT\n"));
			break;
		case SCTP_CTYPE_SHUTDOWN_COMPLETE:
			printd(("Received OOTB SHUTDOWN-COMPLETE\n"));
			break;
		case SCTP_CTYPE_INIT:
			printd(("Received OOTB INIT\n"));
			break;
		case SCTP_CTYPE_INIT_ACK:
			printd(("Received OOTB INIT-ACK\n"));
			break;
		case SCTP_CTYPE_COOKIE_ECHO:
			printd(("Received OOTB COOKIE-ECHO\n"));
			break;
		}
		break;;
	case SCTP_CTYPE_COOKIE_ECHO:	/* RFC 2960 8.4(4) and (8). */
	case SCTP_CTYPE_SHUTDOWN:	/* RFC 2960 8.4(8). */
	case SCTP_CTYPE_SACK:	/* RFC 2960 8.4(8). */
	case SCTP_CTYPE_HEARTBEAT:	/* RFC 2960 8.4(8). */
	case SCTP_CTYPE_HEARTBEAT_ACK:	/* RFC 2960 8.4(8). */
	case SCTP_CTYPE_DATA:	/* RFC 2960 8.4(8). */
		switch (ch->type) {
		case SCTP_CTYPE_SHUTDOWN:
			printd(("Received OOTB SHUTDOWN\n"));
			break;
		case SCTP_CTYPE_SACK:
			printd(("Received OOTB SACK\n"));
			break;
		case SCTP_CTYPE_HEARTBEAT:
			printd(("Received OOTB HEARTBEAT\n"));
			break;
		case SCTP_CTYPE_HEARTBEAT_ACK:
			printd(("Received OOTB HEARTBEAT-ACK\n"));
			break;
		case SCTP_CTYPE_DATA:
			printd(("Received OOTB DATA\n"));
			break;
		case SCTP_CTYPE_COOKIE_ECHO:
			printd(("Received OOTB COOKIE-ECHO\n"));
			break;
		}
		if (!(sp = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))) {
			printd(("Do not have TCB, aborting\n"));
			sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		} else {
			rare();
			printd(("Have TCB, not aborting\n"));
			sctp_put(sp);
		}
		break;
	case SCTP_CTYPE_SHUTDOWN_ACK:
		printd(("Received OOTB SHUTDOWN-ACK\n"));
		if (!(sp = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))) {
			printd(("Do not have TCB, aborting\n"));
			sctp_send_shutdown_complete_ootb(iph->saddr, iph->daddr, sh);
		} else {
			rare();
			printd(("Have TCB, not aborting\n"));
			sctp_put(sp);
		}
		break;
	}
	return;
}

/**
 * sctp_v4_err: - receive ICMP error message
 * @skb: the message
 * @info: the ICMP error information
 *
 * This is the error handler.  We have received an ICMP error for the protocol number.  This routine
 * is called by the ICMP module when it gets some sort of error condition.  It err < 0 then the
 * socket should be closed and the error returned to the user.  If err > 0 it's just the icmp type
 * << 8 | icmp code.  After adjustment header points to the first 8 bytes of the SCTP Common Header.
 * We need to find the appropriate port or verification tag.
 *
 * For sockets, the locking strategy used here is very "optimistic".  sctp_recv_err is called
 * directly.  When someone else accesses the socket the ICMP is just dropped and for some paths
 * there is no check at all.  A more general error queue to queue errors for later handling is
 * probably better.
 *
 * For STREAMS, because we don't want any races here, we place a M_CTL message (in band 1) on the
 * read queue of the stream to which the message applies.  This distinguishes it from M_DATA
 * messages.  It is processed within the stream with queues locked by sctp_recv_err when the M_CTL
 * message is dequeued and processed.  We have to copy the information because the skb will go away
 * after this call.  Because we check for flow control, this approach is also more resilient agains
 * ICMP flooding attacks.
 */
STATIC void
sctp_v4_err(struct sk_buff *skb, uint32_t info)
{
	struct sctp *sp;
	struct sctphdr *sh;
	struct iphdr *iph = (struct iphdr *) skb->data;
	size_t ihl;

	ensure(skb, return);
#define ICMP_MIN_LENGTH 8
	if (skb->len < (ihl = iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
	sh = (struct sctphdr *) (skb->data + ihl);
	if (sh->v_tag)
		sp = sctp_lookup_ptag(sh->v_tag, sh->srce, sh->dest, iph->saddr, iph->daddr);
	else
		sp = sctp_lookup_tcb(sh->srce, sh->dest, iph->saddr, iph->daddr);
	usual(sp);
	if (!sp)
		goto no_stream;
	if (sp->state == SCTP_CLOSED)
		goto closed;
	/* No need to take locks here, the reference held from sctp_lookup_xxx() is sufficient for
	   our purposes here. */
	{
		mblk_t *mp;
		size_t mlen = sizeof(uint32_t) + sizeof(struct icmphdr *);

		if (!(mp = allocb(mlen, BPRI_MED)))
			goto no_buffers;
		if (!sp->rq || !bcanput(sp->rq, 1))
			goto flow_controlled;
		mp->b_datap->db_type = M_CTL;
		mp->b_band = 1;
		*((uint32_t *) mp->b_wptr) = iph->daddr;
		mp->b_wptr += sizeof(uint32_t);
		*((struct icmphdr *) mp->b_wptr) = *(skb->h.icmph);
		mp->b_wptr += sizeof(struct icmphdr);
		putq(sp->rq, mp);	/* must succeed, band 1 already allocated */
		goto discard_and_put;
	      no_buffers:
		ptrace(("ERROR: could not allocate buffer\n"));
		goto discard_and_put;
	      flow_controlled:
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_and_put;
	}
	goto discard_and_put;
      discard_and_put:
	sctp_put(sp);
	return;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_and_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	goto drop;
      drop:
#ifdef HAVE_KINC_LINUX_SNMP_H
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

#ifdef LIS
/**
 * sctp_free: - free routine for handling esballoc'ed skbuffs
 * @data: a pointer to the skbuff
 */
STATIC streamscall __hot_get void
sctp_free(caddr_t data)
{
	struct sk_buff *skb = (struct sk_buff *) data;

	trace();
	ensure(skb, return);
	kfree_skb(skb);
	return;
}
#endif

#define COPY_INSTEAD_OF_ESBALLOC 1
#undef COPY_INSTEAD_OF_ESBALLOC

/**
 * sctp_v4_rcv: - receive message from IP layer
 * @skb: the message
 *
 * This is the received frame handler for SCTP IPv4.  All packets received by IPv4 with the protocol
 * number IPPROTO_SCTP will arrive here first.  We should be performing the Adler-32 checksum on the
 * packet.  If the Adler-32 checksum fails then we should silently discard the packet per RFC 2960.
 */
STATIC int
sctp_v4_rcv(struct sk_buff *skb)
{
	mblk_t *mp;
	struct sctp *sp;
	struct sctphdr *sh;

#ifdef LIS
	struct sctphdr *sh2;
	frtn_t fr = { &sctp_free, (char *) skb };
#endif

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif
	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	SCTP_INC_STATS_BH(SctpInSCTPPacks);
	/* For now...  We should actually place non-linear fragments into seperate mblks and pass
	   them up as a chain, or deal with non-linear sk_buffs directly.  As it winds up, the
	   netfilter hooks linearize anyway. */
#ifdef HAVE_KFUNC_SKB_LINEARIZE_1_ARG
	if (skb_is_nonlinear(skb) && skb_linearize(skb) != 0)
		goto linear_fail;
#else				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
#endif				/* HAVE_KFUNC_SKB_LINEARIZE_1_ARG */
	/* pull up the ip header */
	sh = SCTP_SKB_SH(skb);
	{
		size_t clen;
		struct sctpchdr *ch;

		/* sanity check length and first chunk header */
		if (skb->len & 0x3)
			goto bad_padding;
		if (skb->len < sizeof(*ch))
			goto too_small;
		ch = (typeof(ch)) skb_pull(skb, sizeof(*sh));
		if ((clen = ntohs(ch->len)) < sizeof(*ch))
			goto bad_chunk_len;
		if (skb->len < PADC(clen))
			goto bad_chunk_len;
	}
#ifdef COPY_INSTEAD_OF_ESBALLOC
	/* There seems to be some problem with skbuff corruption, so we will
	 * try this: allocate a full STREAMS message block and copy the data
	 * into the STREAMS message block and free the SKBUFF. */
	{
		size_t plen = skb->len + (skb->data - skb->nh.raw);
		
		if (!(mp = allocb(plen, BPRI_MED)))
			goto no_buffers;
		bcopy(skb->nh.raw, mp->b_wptr, plen);
		mp->b_wptr += plen;
		mp->b_rptr += (skb->data - skb->nh.raw);
	}
#else
#ifdef LIS
	if (!(mp = esballoc(skb->nh.raw, skb->len + (skb->data - skb->nh.raw), BPRI_MED, &fr)))
		goto no_buffers;
#ifndef LIS
	/* tell others it is a socket buffer */
	mp->b_datap->db_flag |= DB_SKBUFF;
#endif
	printd(("%s: mp %p ref count %d\n", __FUNCTION__, mp, mp->b_datap->db_ref));
	mp->b_datap->db_type = M_DATA;
	mp->b_wptr = mp->b_rptr + skb->len + (skb->data - skb->nh.raw);
	/* trim the ip header */
	mp->b_rptr += skb->h.raw - skb->nh.raw;
	sh2 = (typeof(sh2)) mp->b_rptr;
	if (sh->check != sh2->check)
		goto sanity;
	mp->b_rptr += sizeof(struct sctphdr);
#else
	if (!(mp = skballoc(skb, BPRI_MED)))
		goto no_buffers;
	// mp->b_rptr = skb->data;
	// mp->b_wptr = mp->b_rptr + skb->len;
	mp->b_datap->db_base = skb->nh.raw;	/* important */
	mp->b_datap->db_lim = mp->b_wptr;
	mp->b_datap->db_size = mp->b_datap->db_lim - mp->b_datap->db_base;
#endif
#endif
	/* we do the lookup before the checksum */
	{
		struct iphdr *iph = skb->nh.iph;

		printd(("%s: mp %p lookup up stream\n", __FUNCTION__, mp));
		if (!(sp = sctp_lookup(sh, iph->daddr, iph->saddr)))
			goto no_sctp_stream;
	}
	/* perform the stream-specific checksum */
	skb->csum = ntohl(sh->check);
	if (!(skb->dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM))) {
		sh->check = 0;
		if (!cksum_sp_verify(sp, skb->csum, sh, skb->len + sizeof(*sh))) {
			sh->check = htonl(skb->csum);
			sctp_put(sp);
			goto bad_checksum;
		}
		sh->check = htonl(skb->csum);
	}
	skb->dev = NULL;
	if (!sp->rq || !canput(sp->rq))
		goto flow_controlled;
	putq(sp->rq, mp);	/* must succeed, band 0 */
	/* all done */
	printd(("%s: mp %p put to stream %p\n", __FUNCTION__, mp, sp));
	sctp_put(sp);
#ifdef COPY_INSTEAD_OF_ESBALLOC
	kfree_skb(skb);
#endif
	return (0);
      bad_checksum:
	SCTP_INC_STATS_BH(SctpChecksumErrors);
	ptrace(("ERROR: Bad checksum\n"));
	goto free_it;
      no_sctp_stream:
	ptrace(("ERROR: No stream\n"));
	/* perform the default checksum */
	skb->csum = ntohl(sh->check);
	if (!(skb->dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM))) {
		sh->check = 0;
		if (!cksum_verify(skb->csum, sh, skb->len + sizeof(*sh))) {
			sh->check = htonl(skb->csum);
			goto bad_checksum;
		}
		sh->check = htonl(skb->csum);
	}
	ptrace(("ERROR: Received OOTB packet\n"));
	SCTP_INC_STATS_BH(SctpOutOfBlues);
#ifndef SCTP_CONFIG_DISCARD_OOTB
	icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
	/* RFC 2960 Section 8.4 */
	sctp_rcv_ootb(mp);
	freemsg(mp);
#ifdef COPY_INSTEAD_OF_ESBALLOC
	kfree_skb(skb);
#endif
	return (0);
#endif				/* SCTP_CONFIG_DISCARD_OOTB */
	goto free_it;
      free_it:
	ptrace(("ERROR: Discarding message\n"));
	/* free skb in sockets, free mp in streams */
	freemsg(mp);
#ifdef COPY_INSTEAD_OF_ESBALLOC
	kfree_skb(skb);
#endif
	return (0);
      too_small:
	ptrace(("ERROR: Packet too small\n"));
	goto discard_it;
      bad_chunk_len:
	ptrace(("ERROR: Chunk length incorrect\n"));
	goto discard_it;
      bad_padding:
	ptrace(("ERROR: Packet not padded\n"));
	goto discard_it;
      linear_fail:
	ptrace(("ERROR: Couldn't linearize skb\n"));
	goto discard_it;
      bad_pkt_type:
	ptrace(("ERROR: Packet not PACKET_HOST\n"));
	goto discard_it;
      discard_it:
	/* Discard frame silently. */
	ptrace(("ERROR: Discarding frame silently\n"));
	kfree_skb(skb);
	return (0);
      flow_controlled:
	sctp_put(sp);
	ptrace(("ERROR: Flow Controlled\n"));
	goto free_it;
#ifdef LIS
      sanity:
	ptrace(("ERROR: Sanity check on esballoc sh->check = %08x, sh2->check = %08x\n", sh->check,
		sh2->check));
	goto free_it;
#endif
      no_buffers:
	ptrace(("ERROR: Couldn't allocate mblk\n"));
	goto discard_it;
}

#ifdef SCTP_CONFIG_ADD_IP
/*
 *  ==========================================================================
 *
 *  Netdevice Notifier for ADD-IP
 *
 *  ==========================================================================
 *  
 *  This is called whenever something happens with an IP network device.  When interfaces go up we
 *  check to see if IP addresses can be added to any SCTP sockets or streams.  When interfaces go
 *  down, we check to see if IP addresses need to be removed from any SCTP sockets or streams.
 */
STATIC int
sctp_notifier(struct notifier_block *self, unsigned long msg, void *data)
{
	struct net_device *dev = (struct net_device *) data;
	struct in_device *in_dev;

#if ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
	rcu_read_lock();
#endif
#ifdef HAVE_KFUNC___IN_DEV_GET_RCU
	if (!(in_dev = __in_dev_get_rcu(dev)))
		goto done;
#else
	if (!(in_dev = __in_dev_get(dev)))
		goto done;
#endif
	switch (msg) {
	case NETDEV_UP:
	case NETDEV_REBOOT:
	{
		struct sctp *sp;
		struct in_ifaddr *ifa;

		spin_lock(&sctp_protolock);
		for (sp = sctp_protolist; sp; sp = sp->next) {

			/* FIXME: need a separate lock to protect the source address list, or maybe
			   we should just use the protocol list lock as a reader writer lock. */
			bh_lock_sctp(sp);
			/* only do ADD-IP on established sockets */
			if (sp->state != SCTP_ESTABLISHED)
				continue;
			/* peer must be capable of ADD-IP */
			if (sp->p_caps & SCTP_CAPS_ADD_IP)
				continue;
			/* stream must be wildcard bound */
			if (sp->userlocks & SCTP_BINDADDR_LOCK)
				continue;
#if !( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
			read_lock(&in_dev->lock);
#endif
			for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
				struct sctp_saddr *ss;
				int err = 0;

				/* skip loopback on wildcard */
				if (LOOPBACK(ifa->ifa_local))
					continue;
				if (!(ss = sctp_find_saddr(sp, ifa->ifa_local))) {
					if ((ss = __sctp_saddr_alloc(sp, ifa->ifa_local, &err))) {
						ss->flags |= SCTP_SRCEF_ADD_REQUEST;
						sp->sackf |= SCTP_SACKF_ASC;
					}
				} else
					ss->flags &= ~SCTP_SRCEF_DEL_REQUEST;
				/* should we clear del pending too? */
			}
#if ! ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
			read_unlock(&in_dev->lock);
#endif
#if 0
			sctp_transmit_wakeup(sp);
#else
			qenable(sp->wq);
#endif
			/* FIXME: need a separate lock to protect the source address list, or maybe
			   we should just use the protocol list lock as a reader writer lock. */
			bh_unlock_sctp(sp);
		}
		spin_unlock(&sctp_protolock);
		break;
	}
	case NETDEV_DOWN:
	case NETDEV_GOING_DOWN:
	{
		struct sctp *sp;
		struct in_ifaddr *ifa;

		spin_lock(&sctp_protolock);
		for (sp = sctp_protolist; sp; sp = sp->next) {

			/* FIXME: need a separate lock to protect the source address list, or maybe
			   we should just use the protocol list lock as a reader writer lock. */
			bh_lock_sctp(sp);
			/* only do ADD-IP on established sockets */
			if (sp->state != SCTP_ESTABLISHED)
				continue;
			/* peer must be capable of ADD-IP */
			if (sp->p_caps & SCTP_CAPS_ADD_IP)
				continue;
			/* socket must be wildcard bound */
			if (sp->userlocks & SCTP_BINDADDR_LOCK)
				continue;
#if !( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
			read_lock(&in_dev->lock);
#endif
			for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
				struct sctp_saddr *ss;

				/* skip loopback on wildcard */
				if (LOOPBACK(ifa->ifa_local))
					continue;
				if ((ss = sctp_find_saddr(sp, ifa->ifa_local))) {
					ss->flags |= SCTP_SRCEF_DEL_REQUEST;
					sp->sackf |= SCTP_SACKF_ASC;
					ss->flags &= ~SCTP_SRCEF_ADD_REQUEST;
					/* should we clear add pending too? */
				}
			}
#if !( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
			read_unlock(&in_dev->lock);
#endif
#if 0
			sctp_transmit_wakeup(sp);
#else
			qenable(sp->wq);
#endif
			/* FIXME: need a separate lock to protect the source address list, or maybe
			   we should just use the protocol list lock as a reader writer lock. */
			bh_unlock_sctp(sp);
		}
		spin_unlock(&sctp_protolock);
		break;
	}
	case NETDEV_CHANGEADDR:
		/* we should probably do something for this, but I don't know wheterh it is
		   possible to change addresses on an up interface anyway */
	default:
	case NETDEV_CHANGE:
	case NETDEV_REGISTER:
	case NETDEV_UNREGISTER:
	case NETDEV_CHANGEMTU:
	case NETDEV_CHANGENAME:
		break;
	}
      done:
#if ( defined HAVE_KFUNC_RCU_READ_LOCK || defined HAVE_KMACRO_RCU_READ_LOCK )
	rcu_read_unlock();
#endif
	return NOTIFY_DONE;
}
#endif				/* SCTP_CONFIG_ADD_IP */

/*
 *  ==========================================================================
 *
 *  PROC FS Hooks
 *
 *  ==========================================================================
 *  Proc filesystem hooks are a socket-specific hook.  These hooks are not used for the STREAMS
 *  version.
 */

#ifdef CONFIG_PROC_FS

#ifdef HAVE_KINC_LINUX_PERCPU_H
STATIC void
sctp_init_stats(void)
{
	sctp_statistics[0] = alloc_percpu(struct sctp_mib);
	sctp_statistics[1] = alloc_percpu(struct sctp_mib);
}
STATIC void
sctp_term_stats(void)
{
	free_percpu(sctp_statistics[1]);
	free_percpu(sctp_statistics[0]);
}
#endif				/* HAVE_KINC_LINUX_PERCPU_H */

#endif				/* CONFIG_PROC_FS */

/*
 *  ==========================================================================
 *
 *  SYSCTL Hooks
 *
 *  ==========================================================================
 *  Sysctl hooks are currently socket-specific hooks.  We do not yet use them for the STREAMS
 *  version.  These are really just defaults.  For STREAMS the default values should be changed with
 *  XTI_OPTIONS or other options on the stream.
 */

STATIC void
sctp_init_protosw(void)
{
	sctp_n_init();
	sctp_t_init();
	return;
}

STATIC void
sctp_term_protosw(void)
{
	sctp_t_term();
	sctp_n_term();
	return;
}

/*
 *  ==========================================================================
 *
 *  Kernel Module Initialization and Cleanup
 *
 *  ==========================================================================
 */

#ifdef SCTP_CONFIG_ADD_IP
STATIC struct notifier_block sctp_netdev_notifier = {
	.notifier_call = &sctp_notifier,
};
STATIC void
sctp_init_notify(void)
{
	register_netdevice_notifier(&sctp_netdev_notifier);
}
STATIC void
sctp_term_notify(void)
{
	unregister_netdevice_notifier(&sctp_netdev_notifier);
}
#endif				/* SCTP_CONFIG_ADD_IP */

/* *INDENT-OFF* */
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
STATIC struct inet_protocol sctp_protocol = {
	.handler = sctp_v4_rcv,		/* SCTP data handler */
	.err_handler = sctp_v4_err,	/* SCTP error control */
	.protocol = IPPROTO_SCTP,	/* protocol ID */
	.name = "SCTP",			/* name */
};
STATIC void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol);
}
STATIC void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol);
}
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL */

#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
STATIC struct inet_protocol sctp_protocol = {
	.handler = sctp_v4_rcv,		/* SCTP data handler */
	.err_handler = sctp_v4_err,	/* SCTP error control */
	.no_policy = 1,
};
STATIC void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol, IPPROTO_SCTP);
}
STATIC void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol, IPPROTO_SCTP);
}
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY */

#ifdef HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY
STATIC struct net_protocol sctp_protocol = {
	.handler = sctp_v4_rcv,		/* SCTP data handler */
	.err_handler = sctp_v4_err,	/* SCTP error control */
	.no_policy = 1,
};
STATIC void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol, IPPROTO_SCTP);
}
STATIC void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol, IPPROTO_SCTP);
}
#endif				/* HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY */
/* *INDENT-ON* */

STATIC void
sctp_init(void)
{
#ifdef MODULE
	cmn_err(CE_NOTE, SCTP_BANNER);	/* console splash */
#else
	cmn_err(CE_NOTE, SCTP_SPLASH);	/* console splash */
#endif
	sctp_init_caches();
	sctp_init_hashes();
#ifdef SCTP_CONFIG_ADD_IP
	sctp_init_notify();
#endif				/* SCTP_CONFIG_ADD_IP */
	sctp_init_proto();
	sctp_init_protosw();
	/* TODO: adjust buffer size sysctls, port ranges and other parameters based on dynamic
	   allocations.  */
#ifdef HAVE_KINC_LINUX_PERCPU_H
	sctp_init_stats();
#endif				/* HAVE_KINC_LINUX_PERCPU_H */
	return;
}

STATIC void
sctp_terminate(void)
{
#ifdef HAVE_KINC_LINUX_PERCPU_H
	sctp_term_stats();
#endif				/* HAVE_KINC_LINUX_PERCPU_H */
	sctp_term_protosw();
	sctp_term_proto();
#ifdef SCTP_CONFIG_ADD_IP
	sctp_term_notify();
#endif				/* SCTP_CONFIG_ADD_IP */
	sctp_term_hashes();
	sctp_term_caches();
	return;
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
#ifdef SCTP_CONFIG_MODULE
int
init_module(void)
{
	sctp_init();
	return (0);
}

void
cleanup_module(void)
{
	sctp_terminate();
	return;
}
#endif				/* SCTP_CONFIG_MODULE */
