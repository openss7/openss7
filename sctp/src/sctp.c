/*****************************************************************************

 @(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/03/08 19:29:40 $

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

 Last Modified $Date: 2005/03/08 19:29:40 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/03/08 19:29:40 $"

static char const ident[] =
    "$RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/03/08 19:29:40 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#   include <linux/modversions.h>
#endif				/* MODVERSIONS */
#include <linux/module.h>
#include <linux/modversions.h>

/* get rid of header namespace polution */
#define sctp_addr		sctp_addr__
#define sctp_daddr		sctp_daddr__
#define sctp_saddr		sctp_saddr__
#define sctp_strm		sctp_strm__
#define sctp_dup		sctp_dup__
#define sctp_bind_bucket	sctp_bind_bucket__
#define sctp_mib		sctp_mib__
#define sctphdr			sctphdr__
#define sctp_cookie		sctp_cookie__
#define sctp_chunk		sctp_chunk__
#define sctp_opt		sctp_opt__
#define NET_SCTP_RTO_INITIAL		NET_SCTP_RTO_INITIAL__
#define NET_SCTP_RTO_MIN		NET_SCTP_RTO_MIN__
#define NET_SCTP_RTO_MAX		NET_SCTP_RTO_MAX__
#define NET_SCTP_RTO_ALPHA		NET_SCTP_RTO_ALPHA__
#define NET_SCTP_RTO_BETA		NET_SCTP_RTO_BETA__
#define NET_SCTP_VALID_COOKIE_LIFE	NET_SCTP_VALID_COOKIE_LIFE__
#define NET_SCTP_ASSOCIATION_MAX_RETRANS NET_SCTP_ASSOCIATION_MAX_RETRANS__
#define NET_SCTP_PATH_MAX_RETRANS	NET_SCTP_PATH_MAX_RETRANS__
#define NET_SCTP_MAX_INIT_RETRANSMITS	NET_SCTP_MAX_INIT_RETRANSMITS__
#define NET_SCTP_HB_INTERVAL		NET_SCTP_HB_INTERVAL__
#define NET_SCTP_PRESERVE_ENABLE	NET_SCTP_PRESERVE_ENABLE__
#define NET_SCTP_MAX_BURST		NET_SCTP_MAX_BURST__

#include <linux/sysctl.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/ipsec.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>

#ifdef SCTP_CONFIG_MODULE
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <net/protocol.h>
#endif				/* SCTP_CONFIG_MODULE */

#include <net/inet_common.h>
#ifdef HAVE_NET_XFRM_H
#include <net/xfrm.h>
#endif				/* HAVE_NET_XFRM_H */
#include <net/icmp.h>
#ifdef HAVE_NET_DST_H
#include <net/dst.h>
#endif				/* HAVE_NET_DST_H */
#include <net/ip.h>
#ifdef	CONFIG_IP_MASQUERADE
#include <net/ip_masq.h>
#endif				/* CONFIG_IP_MASQUERADE */
#ifdef	SCTP_CONFIG_ECN
#include <net/inet_ecn.h>
#endif				/* SCTP_CONFIG_ECN */

#include <asm/uaccess.h>
#include <asm/segment.h>

#include <linux/inet.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <asm/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

/* get rid of header namespace polution */
#undef sctp_addr
#undef sctp_daddr
#undef sctp_saddr
#undef sctp_strm
#undef sctp_dup
#undef sctp_bind_bucket
#undef sctp_mib
#undef sctphdr
#undef sctp_cookie
#undef sctp_chunk
#undef sctp_opt
#undef NET_SCTP_RTO_INITIAL
#undef NET_SCTP_RTO_MIN
#undef NET_SCTP_RTO_MAX
#undef NET_SCTP_RTO_ALPHA
#undef NET_SCTP_RTO_BETA
#undef NET_SCTP_VALID_COOKIE_LIFE
#undef NET_SCTP_ASSOCIATION_MAX_RETRANS
#undef NET_SCTP_PATH_MAX_RETRANS
#undef NET_SCTP_MAX_INIT_RETRANSMITS
#undef NET_SCTP_HB_INTERVAL
#undef NET_SCTP_PRESERVE_ENABLE
#undef NET_SCTP_MAX_BURST

#include "include/linux/sysctl.h"
#include "sctp_debug.h"
#include "include/linux/hooks.h"
#include "include/netinet/sctp.h"

#define SCTP_DESCRIP	"SCTP/IP (RFC 2960) FOR LINUX NET4 $Name:  $($Revision: 0.9.2.25 $)"
#define SCTP_EXTRA	"Part of the OpenSS7 Stack for Linux."
#define SCTP_REVISION	"OpenSS7 $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/03/08 19:29:40 $"
#define SCTP_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define SCTP_DEVICE	"Supports Linux NET4."
#define SCTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCTP_LICENSE	"GPL"
#define SCTP_BANNER	SCTP_DESCRIP	"\n" \
			SCTP_EXTRA	"\n" \
			SCTP_REVISION	"\n" \
			SCTP_COPYRIGHT	"\n" \
			SCTP_DEVICE	"\n" \
			SCTP_CONTACT	"\n"
#define SCTP_SPLASH	SCTP_DESCRIP	"\n" \
			SCTP_REVISION	"\n"

#ifdef LINUX
#ifdef SCTP_CONFIG_MODULE
MODULE_AUTHOR(SCTP_CONTACT);
MODULE_DESCRIPTION(SCTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCTP_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* SCTP_CONFIG_MODULE */
#endif				/* LINUX */

#define STREAMS 0
#define SOCKETS 1

#if SOCKETS
#define skb_next(__skb) \
	(((__skb)->next != ((struct sk_buff *) (__skb)->list)) ? (__skb)->next : NULL)
#define skb_prev(__skb) \
	(((__skb)->prev != ((struct sk_buff *) (__skb)->list)) ? (__skb)->prev : NULL)
#endif				/* SOCKETS */

/*
 *  =========================================================================
 *
 *  DEFAULT Protocol Values
 *
 *  =========================================================================
 */
#if STREAMS
/* ip defaults */
#define SCTP_DEFAULT_IP_TOS			(0x0)
#define SCTP_DEFAULT_IP_TTL			(64)
#define SCTP_DEFAULT_IP_PROTO			(132)
#define SCTP_DEFAULT_IP_DONTROUTE		(0)
#define SCTP_DEFAULT_IP_BROADCAST		(0)
#define SCTP_DEFAULT_IP_PRIORITY		(0)
#endif				/* STREAMS */
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
#define SCTP_DEFAULT_MAX_BURST			(4)
#define SCTP_DEFAULT_ADAPTATION_LAYER_INFO	(0)
#define SCTP_DEFAULT_PARTIAL_RELIABILITY	(0)
/* per destination defaults */
#define SCTP_DEFAULT_PATH_MAX_RETRANS		(5)
#define SCTP_DEFAULT_RTO_INITIAL		(3*HZ)
#define SCTP_DEFAULT_RTO_MIN			(1*HZ)
#define SCTP_DEFAULT_RTO_MAX			(60*HZ)
#define SCTP_DEFAULT_HEARTBEAT_ITVL		(30*HZ)

#if SOCKETS
/*
 *  sysctls
 */
/* FIXME: These memory sysctls are not actually used. */
STATIC int sysctl_sctp_mem[3];		/* XXX */
STATIC int sysctl_sctp_rmem[3] = { 4 * 1024, 87380, 87380 * 2 };	/* XXX */
STATIC int sysctl_sctp_wmem[3] = { 4 * 1024, 16 * 1024, 128 * 1024 };	/* XXX */

/* per association defaults */
STATIC int sysctl_sctp_max_istreams = SCTP_DEFAULT_MAX_ISTREAMS;
STATIC int sysctl_sctp_req_ostreams = SCTP_DEFAULT_REQ_OSTREAMS;
STATIC int sysctl_sctp_max_init_retries = SCTP_DEFAULT_MAX_INIT_RETRIES;
STATIC int sysctl_sctp_assoc_max_retrans = SCTP_DEFAULT_ASSOC_MAX_RETRANS;
STATIC int sysctl_sctp_valid_cookie_life = SCTP_DEFAULT_VALID_COOKIE_LIFE;
STATIC int sysctl_sctp_cookie_inc = SCTP_DEFAULT_COOKIE_INC;
STATIC int sysctl_sctp_mac_type = SCTP_DEFAULT_MAC_TYPE;
STATIC int sysctl_sctp_csum_type = SCTP_DEFAULT_CSUM_TYPE;
STATIC int sysctl_sctp_throttle_itvl = SCTP_DEFAULT_THROTTLE_ITVL;
STATIC int sysctl_sctp_default_sid = SCTP_DEFAULT_SID;
STATIC int sysctl_sctp_default_ppi = SCTP_DEFAULT_PPI;
STATIC int sysctl_sctp_max_sack_delay = SCTP_DEFAULT_MAX_SACK_DELAY;
STATIC int sysctl_sctp_max_burst = SCTP_DEFAULT_MAX_BURST;
STATIC int sysctl_sctp_adaptation_layer_info = SCTP_DEFAULT_ADAPTATION_LAYER_INFO;
STATIC int sysctl_sctp_partial_reliability = SCTP_DEFAULT_PARTIAL_RELIABILITY;
/* per destination defaults */
STATIC int sysctl_sctp_rto_initial = SCTP_DEFAULT_RTO_INITIAL;
STATIC int sysctl_sctp_rto_min = SCTP_DEFAULT_RTO_MIN;
STATIC int sysctl_sctp_rto_max = SCTP_DEFAULT_RTO_MAX;
STATIC int sysctl_sctp_path_max_retrans = SCTP_DEFAULT_PATH_MAX_RETRANS;
STATIC int sysctl_sctp_heartbeat_itvl = SCTP_DEFAULT_HEARTBEAT_ITVL;

#ifdef CONFIG_INET_ECN
STATIC int sysctl_sctp_ecn = 1;
#else				/* CONFIG_INET_ECN */
STATIC int sysctl_sctp_ecn = 0;
#endif				/* CONFIG_INET_ECN */

STATIC int min_sctp_max_istreams = 1;
STATIC int min_sctp_req_ostreams = 1;
STATIC int min_sctp_max_burst = 1;
#else				/* SOCKETS */
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
STATIC size_t sctp_default_max_burst = SCTP_DEFAULT_MAX_BURST;
STATIC size_t sctp_default_adaptation_layer_info = SCTP_DEFAULT_ADAPTATION_LAYER_INFO;
STATIC size_t sctp_default_partial_reliability = SCTP_DEFAULT_PARTIAL_RELIABILITY;
/* per destination defaults */
STATIC size_t sctp_default_rto_initial = SCTP_DEFAULT_RTO_INITIAL;
STATIC size_t sctp_default_rto_min = SCTP_DEFAULT_RTO_MIN;
STATIC size_t sctp_default_rto_max = SCTP_DEFAULT_RTO_MAX;
STATIC size_t sctp_default_path_max_retrans = SCTP_DEFAULT_PATH_MAX_RETRANS;
STATIC size_t sctp_default_heartbeat_itvl = SCTP_DEFAULT_HEARTBEAT_ITVL;
#endif				/* SOCKETS */

/*
 *  =========================================================================
 *
 *  DATA STRUCTURES
 *
 *  =========================================================================
 */
struct sctp_opt;
typedef struct sctp_opt sctp_t;
struct sctp_skb_cb;
typedef struct sctp_skb_cb sctp_tcb_t;
struct timer_list;
typedef struct timer_list sctp_timer_t;
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
		struct sk_buff *head;	/* head pointer */
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
	sctp_timer_t timer_heartbeat;	/* heartbeat timer (for acks) */
	sctp_timer_t timer_retrans;	/* retrans (RTO) timer */
	sctp_timer_t timer_idle;	/* idle timer */
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
struct sctp_ifops;			/* interface operations */
struct sctp_opt_dummy {
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
	ulong throttle;			/* throttle init interval */
	ulong life;			/* data lifetime */
	uint8_t disp;			/* data disposition */
	uint8_t prel;			/* partial reliabliity */
	ulong hb_rcvd;			/* hb received timestamp */
	struct sk_buff *retry;		/* msg to retry on timer expiry */
	struct sk_buff_head conq;	/* accept queue */
	struct sk_buff_head expq;	/* expedited queue */
	int rmem_queued;		/* queued read memeory */
	struct sk_buff_head urgq;	/* urgent queue */
	struct sk_buff_head errq;	/* error queue */
	struct sk_buff_head oooq;	/* out of order queue */
	sctp_tcb_t *gaps;		/* gaps acks for this stream */
	size_t ngaps;			/* number of gap reports in list */
	size_t nunds;			/* number of undelivered in list */
	struct sk_buff_head dupq;	/* duplicate queue */
	sctp_tcb_t *dups;		/* dup tsns for this stream */
	size_t ndups;			/* number of dup reports in list */
	struct sk_buff_head rtxq;	/* retransmit queue */
	size_t nrtxs;			/* number of retransmits in list */
	size_t nsack;			/* number of sacked in list */
	struct sk_buff_head ackq;	/* pending acknowledgement queue */
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
	uint sackf;			/* sack flags for association */
	uint flags;			/* flags */
	uint pmtu;			/* path MTU for association */
	uint amps;			/* assoc max payload size */
	int ext_header_len;		/* extra header length for options */
	size_t in_flight;		/* number of bytes in flight */
	size_t retransmits;		/* number of retransmits this assoc */
	size_t max_retrans;		/* max association retransmits */
	size_t max_burst;		/* max number of burst packets */
};

#define beg_protinfo (&((struct sock *)(0))->protinfo)
#define siz_protinfo (sizeof(((struct sock *)0)->protinfo))
#define beg_tp_pinfo (&((struct sock *)(0))->tp_pinfo)
#define siz_tp_pinfo (sizeof(((struct sock *)0)->tp_pinfo))

#define gap_size \
	(size_t)((uint8_t *)beg_protinfo - (uint8_t *)beg_tp_pinfo) \
	- sizeof(struct sctp_opt_dummy)

/*
 *  Primary data structure.
 */
struct sctp_opt {
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
	ulong throttle;			/* throttle init interval */
	ulong life;			/* data lifetime */
	uint8_t disp;			/* data disposition */
	uint8_t prel;			/* partial reliability */
	ulong hb_rcvd;			/* hb received timestamp */
	struct sk_buff *retry;		/* msg to retry on timer expiry */
	struct sk_buff_head conq;	/* accept queue */
	struct sk_buff_head expq;	/* expedited queue */
	int rmem_queued;		/* queued read memeory */
	struct sk_buff_head urgq;	/* urgent queue */
	struct sk_buff_head errq;	/* error queue */
	struct sk_buff_head oooq;	/* out of order queue */
	sctp_tcb_t *gaps;		/* gaps acks for this stream */
	size_t ngaps;			/* number of gap reports in list */
	size_t nunds;			/* number of undelivered in list */
	struct sk_buff_head dupq;	/* duplicate queue */
	sctp_tcb_t *dups;		/* dup tsns for this stream */
	size_t ndups;			/* number of dup reports in list */
	struct sk_buff_head rtxq;	/* retransmit queue */
	size_t nrtxs;			/* number of retransmits in list */
	size_t nsack;			/* number of sacked in list */
	struct sk_buff_head ackq;	/* pending acknowledgement queue */
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
	uint8_t __jump[gap_size];	/* jump over reserved structures */
	/* ------------------------------------------------------------------ */
	struct inet_opt inet;		/* inet options */
	uint cmsg_flags;		/* flags */
	sctp_timer_t timer_init;	/* init timer */
	sctp_timer_t timer_cookie;	/* cookie timer */
	sctp_timer_t timer_shutdown;	/* shutdown timer */
	sctp_timer_t timer_guard;	/* shutdown guard timer */
	sctp_timer_t timer_sack;	/* sack timer */
	sctp_timer_t timer_asconf;	/* asconf timer */
	sctp_timer_t timer_life;	/* lifetime timer */
	uint8_t hmac;			/* hmac type */
	uint8_t cksum;			/* checksum type */
	ulong hb_tint;			/* hb throttle interval */
	uint16_t sid;			/* default sid */
	uint32_t ppi;			/* default ppi */
	ulong ck_life;			/* valid cookie life */
	ulong ck_inc;			/* cookie increment */
	uint8_t l_caps;			/* local capabilities */
	uint8_t p_caps;			/* peer capabilities */
	struct sk_buff *reply;		/* saved reply to ASCONF chunk */
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
};

STATIC struct sock *sctp_protolist = NULL;
STATIC atomic_t sctp_socket_count = ATOMIC_INIT(0);
STATIC atomic_t sctp_orphan_count = ATOMIC_INIT(0);

#if SOCKETS
/*
 *  Private struct macros
 */
#define __sp_offset ((unsigned int)&(((struct sock *)(0))->tp_pinfo))
#define SCTP_PROT(__sk) ((struct sctp_opt *)&(__sk)->tp_pinfo)
#define SCTP_SOCK(__sp) ((struct sock *)(((uint8_t *)(__sp)) - __sp_offset))
#endif				/* SOCKETS */

#if STREAMS
/*
 *  User locks.
 */
#define SCTP_SNDBUF_LOCK	1
#define SCTP_RCVBUF_LOCK	2
#define SCTP_BINDADDR_LOCK	4
#define SCTP_BINDPORT_LOCK	8
#endif				/* STREAMS */
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
#if SOCKETS
/*
 *  SCTP cmsg flags
 */
#define SCTP_CMSGF_RECVSID  0x01
#define SCTP_CMSGF_RECVPPI  0x02
#define SCTP_CMSGF_RECVSSN  0x04
#define SCTP_CMSGF_RECVTSN  0x08
#endif				/* SOCKETS */
#if STREAMS
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
#endif				/* STREAMS */

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

#ifndef HAVE_OPENSS7_SCTP
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

struct sctp_mib sctp_statistics[NR_CPUS * 2];
#endif				/* HAVE_OPENSS7_SCTP */

#if SOCKETS
#define SCTP_INC_STATS(field)		SNMP_INC_STATS(sctp_statistics, field)
#define SCTP_INC_STATS_BH(field)	SNMP_INC_STATS_BH(sctp_statistics, field)
#define SCTP_INC_STATS_USER(field)	SNMP_INC_STATS_USER(sctp_statistics, field)
#else				/* SOCKETS */
#define SCTP_INC_STATS(field)		SNMP_INC_STATS(sctp_statistics, field)
#define SCTP_INC_STATS_BH(field)	SNMP_INC_STATS(sctp_statistics, field)
#define SCTP_INC_STATS_USER(field)	SNMP_INC_STATS(sctp_statistics, field)
#endif				/* SOCKETS */

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
#if STREAMS
#define SCTP_ESTABLISHED	 TCP_ESTABLISHED	/* ESTABLISHED */
#define SCTP_COOKIE_WAIT	 TCP_SYN_SENT		/* COOKIE-WAIT */
#define SCTP_COOKIE_ECHOED	 TCP_SYN_RECV		/* COOKIE-ECHOED */
#define SCTP_SHUTDOWN_PENDING	 TCP_FIN_WAIT1		/* SHUTDOWN-PENDING */
#define SCTP_SHUTDOWN_SENT	 TCP_FIN_WAIT2		/* SHUTDOWN-SENT */
#define SCTP_UNREACHABLE	 TCP_TIME_WAIT		/* (not used) */
#define SCTP_CLOSED		 TCP_CLOSE		/* CLOSED */
#define SCTP_SHUTDOWN_RECEIVED	 TCP_CLOSE_WAIT		/* SHUTDOWN-RECEIVED */
#define SCTP_SHUTDOWN_RECVWAIT	 TCP_LAST_ACK		/* SHUTDOWN-ACK-SENT (from SHUTDOWN-RECEIVED) */
#define SCTP_LISTEN		 TCP_LISTEN		/* LISTEN */
#define SCTP_SHUTDOWN_ACK_SENT	 TCP_CLOSING		/* SHUTDOWN-ACK-SENT (from SHUTDOWN-SENT) */
#define SCTP_MAX_STATES		 TCP_MAX_STATES
#endif				/* STREAMS */
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

STATIC INLINE int
sctp_change_state(struct sock *sk, int newstate)
{
	int oldstate;
	if ((oldstate = xchg(&sk->state, newstate)) != newstate) {
		char *oldname, *newname;
		switch (oldstate) {
		case 0:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_ESTABLISHED:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_COOKIE_WAIT:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_COOKIE_ECHOED:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_SHUTDOWN_PENDING:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_SHUTDOWN_SENT:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_CLOSED:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_SHUTDOWN_RECVWAIT:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_LISTEN:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_SHUTDOWN_ACK_SENT:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_UNREACHABLE:
			oldname = sctp_state_name[oldstate];
			break;
		case SCTP_MAX_STATES:
			oldname = sctp_state_name[oldstate];
			break;
		default:
			oldname = "(Invalid)";
			break;
		}
		switch (newstate) {
		case 0:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_ESTABLISHED:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_COOKIE_WAIT:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_COOKIE_ECHOED:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_SHUTDOWN_PENDING:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_SHUTDOWN_SENT:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_CLOSED:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_SHUTDOWN_RECVWAIT:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_LISTEN:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_SHUTDOWN_ACK_SENT:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_UNREACHABLE:
			newname = sctp_state_name[newstate];
			break;
		case SCTP_MAX_STATES:
			newname = sctp_state_name[newstate];
			break;
		default:
			newname = "(Invalid)";
			break;
		}
		printd(("INFO: STATE CHANGE: sk %p, %s to %s\n", sk, oldname, newname));
	}
	return (oldstate);
}

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

#define SCTP_IPH(__skb) ((__skb)->nh.iph)
#define SCTP_SH(__skb) (SCTP_SKB_SH(__skb))
#define SCTP_CH(__skb) (SCTP_SKB_CH(__skb))

#ifdef HAVE_MEMBER_SK_BUFF_H_SH
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
struct sctp_skb_cb {
	/* for gaps, dups and acks on receive, frag on transmit */
	sctp_tcb_t *next;		/* message linkage */
	sctp_tcb_t *tail;		/* message linkage */
	struct sk_buff *skb;		/* message linkage */
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

#define SCTP_TCB(__skb) ((struct sctp_skb_cb *)(&(__skb)->cb[0]))

#if SOCKETS
#define __cb_offset ((unsigned int)&(((struct sk_buff *)(NULL))->cb[0]))
#define SCTP_SKB_CB(__skb) ((struct sctp_skb_cb *)(&(__skb)->cb[0]))
#define SCTP_CB_SKB(__cb) ((struct sk_buff *)(((uint8_t *)(__cb)) - __cb_offset))
#endif				/* SOCKETS */

/*
 *  Queue name shortcuts
 */
#define conq conq
#define rcvq receive_queue
#define expq expq
#define sndq write_queue
#define errq errq
#define oooq oooq
#define urgq urgq
#define priq priority_queue
#define rtxq rtxq
#define dupq dupq
#define ackq ackq

/*
 *  -------------------------------------------------------------------------
 *
 *  Locking
 *
 *  -------------------------------------------------------------------------
 */
#define sock_locked(__sk) ((__sk)->lock.users != 0)

STATIC spinlock_t sctp_protolock = SPIN_LOCK_UNLOCKED;

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
	int (*conn_ind) (struct sock * sk, struct sk_buff * cp);
	int (*conn_con) (struct sock * sk);
	int (*data_ind) (struct sock * sk, size_t len, int ord);
	int (*datack_ind) (struct sock * sk, size_t len);
	int (*reset_ind) (struct sock * sk, ulong orig, long reason, struct sk_buff * cp);
	int (*reset_con) (struct sock * sk);
	int (*discon_ind) (struct sock * sk, ulong orig, long reason, struct sk_buff * cp);
	int (*ordrel_ind) (struct sock * sk);
	int (*retr_ind) (struct sock * sk, struct sk_buff * dp);
	int (*retr_con) (struct sock * sk);
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
STATIC int sctp_bind_req(struct sock *sk, uint16_t sport, struct sockaddr_in *ssin, size_t snum,
			 ulong cons);
STATIC int sctp_conn_req(struct sock *sk, uint16_t dport, struct sockaddr_in *dsin, size_t dnum,
			 struct sk_buff *dp);
STATIC int sctp_conn_res(struct sock *sk, struct sk_buff *cp, struct sock *ak, struct sk_buff *dp);
#if STREAMS
STATIC int sctp_data_req(struct sock *sk, uint32_t ppi, uint16_t sid, uint ord, uint more,
			 uint rctp, struct sk_buff *dp);
STATIC int sctp_reset_req(struct sock *sk);
STATIC int sctp_reset_res(struct sock *sk);
#endif				/* STREAMS */
STATIC int sctp_discon_req(struct sock *sk, struct sk_buff *cp);
STATIC int sctp_ordrel_req(struct sock *sk);
STATIC int sctp_unbind_req(struct sock *sk);
/*
 *  =========================================================================
 *
 *  SCTP Peer --> SCTP Primitives (Receive Messages)
 *
 *  =========================================================================
 */
STATIC int sctp_recv_msg(struct sock *sk, struct sk_buff *skb);
STATIC int sctp_recv_err(struct sock *sk, struct sk_buff *skb);
/*
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */
#define SCTP_FR_COUNT 4

#if STREAMS
/* STREAMS needs to redefine these for streams structure instead of socket. */
#ifdef SCTP_CONFIG_ECN
#undef INET_ECN_xmit
#define	INET_ECN_xmit(sp) do { (sp)->inet.tos |= 2; } while (0)
#undef INET_ECN_dontxmit
#define	INET_ECN_dontxmit(sp) do { (sp)->inet.tos &= ~3; } while (0)
#endif				/* SCTP_CONFIG_ECN */
#endif				/* STREAMS */

/*
 *  =========================================================================
 *
 *  CHECKSUMS
 *
 *  =========================================================================
 */
STATIC INLINE uint32_t
nocsum_and_copy_from_user(const char *src, char *dst, int len, int sum, int *errp)
{
	if (!verify_area(VERIFY_READ, src, len)) {
		bcopy(src, dst, len);
		return (sum);
	}
	if (len)
		*errp = -EFAULT;
	return sum;
}

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
	if (!verify_area(VERIFY_READ, src, len))
		return __adler32_partial_copy_from_user(src, dst, len, sum, errp);
	if (len)
		*errp = -EFAULT;
	return sum;
}
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
#ifdef SCTP_CONFIG_CRC_32C
/*
 *  -------------------------------------------------------------------------
 *
 *  CRC-32C Checksum
 *
 *  -------------------------------------------------------------------------
 */
#include "sctp_crc32c.h"
/*
 *  Partial checksumming a CCITT checksum relies on the fact that the result of the checksum is a
 *  long division.  If there are two partial checksums on *  two strings of length `l' bits, then
 *
 *  A message d can be split into two substrings d1 and d2 such that d = (d1 * 2**l2) + d2.
 *  Dividing this by the generator polynomial p, we have:
 *
 *  d/p = (d1/p * 2**l2/p) + d2/p.
 *
 *  The term is brackets is really the long division on d1 taken to l2 more places.  By staring with
 *  the value 1 and calculating the normal crc with a subsequent message of all zeros to l2 places,
 *  a table in l2 can be built which will supply the contant term, or the constant term can be
 *  calculated by taking a CRO  on zeros (abbreviated calculation) to l2 places.
 *
 */
#define DOZ1	{sum=(sum>>8)^crc_table[sum&0xff];}
#define DOZ2	DOZ1; DOZ1;
#define DOZ4	DOZ2; DOZ2;
#define DOZ8	DOZ4; DOZ4;
#define DOZ16	DOZ8; DOZ8
STATIC INLINE uint32_t
__add_crc32c(register uint32_t sum, uint32_t csum2, register uint16_t len)
{
	if (len) {
		while (len >= 16) {
			DOZ16;
			len -= 16;
		}
		if (len != 0)
			do {
				DOZ1;
			} while (--len);
		sum += csum2;
	}
	return (sum);
}
STATIC INLINE uint32_t
__final_crc32c(uint32_t csum)
{
	return csum;
}

#define CPCRC1(i)   {crc=(crc>>8)^crc_table[(crc^(dst[i]=src[i]))&0xff];}
#define CPCRC2(i)   CPCRC1(i); CPCRC1(i+1);
#define CPCRC4(i)   CPCRC2(i); CPCRC2(i+2);
#define CPCRC8(i)   CPCRC4(i); CPCRC4(i+4);
#define CPCRC16	    CPCRC8(0); CPCRC8(8)
/* Note crc should be initialized to 0xffffffff */
STATIC INLINE uint32_t
__crc32c_partial_copy_from_user(register const unsigned char *src, register unsigned char *dst,
				register int len, register uint32_t crc, int *errp)
{
	(void) errp;
	if (len) {
		while (len >= 16) {
			CPCRC16;
			src += 16;
			dst += 16;
			len -= 16;
		}
		if (len != 0)
			do {
				crc = (crc >> 8) ^ crc_table[(crc ^ (*dst++ = *src++))
							     & 0xff];
			} while (--len);
	}
	return crc;
}
STATIC INLINE uint32_t
crc32c_and_copy_from_user(const char *src, char *dst, int len, int sum, int *errp)
{
	if (!verify_area(VERIFY_READ, src, len))
		return __crc32c_partial_copy_from_user(src, dst, len, sum, errp);
	if (len)
		*errp = -EFAULT;
	return sum;
}
#endif				/* SCTP_CONFIG_CRC_32C */
/*
 *  -------------------------------------------------------------------------
 *
 *  CHECKSUM
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE uint32_t
cksum(struct sock *sk, void *buf, size_t len)
{
	sctp_t *sp = SCTP_PROT(sk);
	switch (sp->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		return crc32c(~0UL, buf, len);
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return adler32(1UL, buf, len);
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
	}
}
STATIC INLINE uint32_t
__add_cksum(struct sock *sk, uint32_t csum1, uint32_t csum2, uint16_t l2)
{
	sctp_t *sp = SCTP_PROT(sk);
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
__final_cksum(struct sock *sk, uint32_t csum)
{
	sctp_t *sp = SCTP_PROT(sk);
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
STATIC INLINE uint32_t
cksum_and_copy_from_user(struct sock *sk, const char *src, char *dst, int len, int sum, int *errp)
{
	sctp_t *sp = SCTP_PROT(sk);
	switch (sp->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		return crc32c_and_copy_from_user(src, dst, len, sum, errp);
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return adler32_and_copy_from_user(src, dst, len, sum, errp);
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
	}
}
STATIC INLINE int
cksum_verify(uint32_t csum, void *buf, size_t len)
{
#if defined(SCTP_CONFIG_CRC_32C)||!defined(SCTP_CONFIG_ADLER_32)
	if (csum != crc32c(~0UL, buf, len))
#endif				/* defined(SCTP_CONFIG_CRC_32C)||!defined(SCTP_CONFIG_ADLER_32) */
#ifdef SCTP_CONFIG_ADLER_32
		if (csum != adler32(1UL, buf, len))
#endif				/* SCTP_CONFIG_ADLER_32 */
			return (0);
	return (1);
}
STATIC INLINE int
cksum_sk_verify(struct sock *sk, uint32_t csum, void *buf, size_t len)
{
	sctp_t *sp = SCTP_PROT(sk);
	switch (sp->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		if (csum != crc32c(~0UL, buf, len))
			return (0);
		return (1);
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		if (csum != adler32(1UL, buf, len))
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
	csum = crc32c(~0UL, buf, len);
#elif defined(SCTP_CONFIG_ADLER_32)
	csum = adler32(1UL, buf, len);
#endif				/* defined(SCTP_CONFIG_ADLER_32) */
	return (csum);
}

/*
 *  FIXME: These two functions serve a purpose on the sockets verion.  I need to take a look an see
 *  if equivalent functions are necessary in the STREAMS version. This is the only place that
 *  SCTPCB_FLAG_CKSUMMED is currently used.
 */
#if SOCKETS
STATIC INLINE int
skb_add_data(struct sk_buff *skb, char *from, int copy)
{
	int err = 0;
	uint32_t csum;
	int off = skb->len;
	if (!(SCTP_SKB_CB(skb)->flags & SCTPCB_FLAG_CKSUMMED))
		csum = nocsum_and_copy_from_user(from, skb_put(skb, copy), copy, skb->csum, &err);
	else
		csum =
		    cksum_and_copy_from_user(skb->sk, from, skb_put(skb, copy), copy, skb->csum,
					     &err);
	if (!err) {
		skb->csum = csum;
		return (0);
	}
	__skb_trim(skb, off);
	return (-EFAULT);
}
#endif				/* SOCKETS */
#if SOCKETS
STATIC INLINE void
skb_init_cksum(struct sk_buff *skb)
{
	if ((skb->sk->route_caps & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM))) {
		skb->csum = 0;
		return;
	}
	SCTP_SKB_CB(skb)->flags |= SCTPCB_FLAG_CKSUMMED;
	switch (SCTP_PROT(skb->sk)->cksum) {
	default:
#ifdef SCTP_CONFIG_CRC_32C
	case SCTP_CSUM_CRC32C:
		skb->csum = 1UL;
		return;
#endif				/* SCTP_CONFIG_CRC_32C */
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
	case SCTP_CSUM_ADLER32:
		skb->csum = ~0UL;
		return;
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
	}
}
#endif				/* SOCKETS */

/*
 *  =========================================================================
 *
 *  CACHES
 *
 *  =========================================================================
 *  Note that the STREAMS version has an extra cache for the SCTP private structure.  This is
 *  contained inside the sock structure on the Linux Natvie (Sockets) version.
 */

STATIC kmem_cache_t *sctp_bind_cachep = NULL;
STATIC kmem_cache_t *sctp_dest_cachep = NULL;
STATIC kmem_cache_t *sctp_srce_cachep = NULL;
STATIC kmem_cache_t *sctp_strm_cachep = NULL;
STATIC void
sctp_init_caches(void)
{
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
	if (sctp_bind_cachep)
		if (kmem_cache_destroy(sctp_bind_cachep))
			printk(KERN_WARNING "%s: did not destroy sctp_bind_cachep\n", __FUNCTION__);
	if (sctp_dest_cachep)
		if (kmem_cache_destroy(sctp_dest_cachep))
			printk(KERN_WARNING "%s: did not destroy sctp_dest_cachep\n", __FUNCTION__);
	if (sctp_srce_cachep)
		if (kmem_cache_destroy(sctp_srce_cachep))
			printk(KERN_WARNING "%s: did not destroy sctp_srce_cachep\n", __FUNCTION__);
	if (sctp_strm_cachep)
		if (kmem_cache_destroy(sctp_strm_cachep))
			printk(KERN_WARNING "%s: did not destroy sctp_strm_cachep\n", __FUNCTION__);
	return;
}
#endif				/* defined SCTP_CONFIG_MODULE */
#if STREAMS
STATIC INLINE sctp_t *
sctp_get(void)
{
	sctp_t *sp;
	if ((sp = kmem_cache_alloc(sctp_sctp_cachep, SLAB_ATOMIC))) {
		MOD_INC_USE_COUNT;
		bzero(sp, sizeof(*sp));
		atomic_set(&sp->refcnt, 1);
	}
	return (sp);
}
#endif				/* STREAMS */
STATIC INLINE void
sctp_hold(sctp_t *sp)
{
	if (sp)
		sock_hold(SCTP_SOCK(sp));
}
STATIC INLINE void
sctp_put(sctp_t *sp)
{
	if (sp)
		sock_put(SCTP_SOCK(sp));
}

STATIC INLINE struct sctp_daddr *
sctp_dget(void)
{
	struct sctp_daddr *sd;
	if ((sd = kmem_cache_alloc(sctp_dest_cachep, SLAB_ATOMIC))) {
		MOD_INC_USE_COUNT;
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
			_printd(("INFO: Deallocating destination address %d.%d.%d.%d for sp = %p\n",
				 (sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
				 (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff, sp));
			kmem_cache_free(sctp_dest_cachep, sd);
			MOD_DEC_USE_COUNT;
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

#if STREAMS
#endif				/* STREAMS */

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
typedef void timo_fcn_t (unsigned long);

#define SCTP_TIMER_BACKOFF 1
#define SCTP_TIMER_MINIMUM 1

STATIC INLINE int
sctp_timeout_pending(sctp_timer_t *tidp)
{
	return timer_pending(tidp);
}

STATIC INLINE int
sctp_timeout_cancelled(sctp_timer_t *tidp)
{
	return 0;
}

STATIC INLINE void
sp_init_timeout(sctp_t *sp, sctp_timer_t *tidp, timo_fcn_t *fnc)
{
	init_timer(tidp);
	tidp->expires = jiffies;
	tidp->data = (unsigned long) SCTP_SOCK(sp);
	tidp->function = fnc;
}

STATIC INLINE void
sd_init_timeout(struct sctp_daddr *sd, sctp_timer_t *tidp, timo_fcn_t *fnc)
{
	init_timer(tidp);
	tidp->expires = jiffies;
	tidp->data = (unsigned long) sd;
	tidp->function = fnc;
}

STATIC INLINE void
sp_del_timeout(sctp_t *sp, sctp_timer_t *tidp)
{
	if (del_timer(tidp)) {
		sctp_put(sp);
	}
}
STATIC INLINE void
sd_del_timeout(struct sctp_daddr *sd, sctp_timer_t *tidp)
{
	if (del_timer(tidp)) {
		sctp_dput(sd);
	}
}
STATIC INLINE void
sp_set_timeout(sctp_t *sp, sctp_timer_t *tidp, unsigned long ticks)
{
	sctp_hold(sp);
	if (ticks < SCTP_TIMER_MINIMUM)
		ticks = SCTP_TIMER_MINIMUM;
	if (mod_timer(tidp, jiffies + ticks)) {
		swerr();
		sctp_put(sp);
	}
}
STATIC INLINE void
sd_set_timeout(struct sctp_daddr *sd, sctp_timer_t *tidp, unsigned long ticks)
{
	sctp_dhold(sd);
	if (ticks < SCTP_TIMER_MINIMUM)
		ticks = SCTP_TIMER_MINIMUM;
	if (mod_timer(tidp, jiffies + ticks)) {
		swerr();
		sctp_dput(sd);
	}
}
STATIC INLINE void
sp_mod_timeout(sctp_t *sp, sctp_timer_t *tidp, unsigned long ticks)
{
	sctp_hold(sp);
	if (ticks < SCTP_TIMER_MINIMUM)
		ticks = SCTP_TIMER_MINIMUM;
	if (mod_timer(tidp, jiffies + ticks)) {
		sctp_put(sp);
	}
}
STATIC INLINE void
sd_mod_timeout(struct sctp_daddr *sd, sctp_timer_t *tidp, unsigned long ticks)
{
	sctp_dhold(sd);
	if (ticks < SCTP_TIMER_MINIMUM)
		ticks = SCTP_TIMER_MINIMUM;
	if (mod_timer(tidp, jiffies + ticks)) {
		sctp_dput(sd);
	}
}

/*
 *  Find a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE struct sctp_daddr *
sctp_find_daddr(sctp_t *sp, uint32_t daddr)
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
sctp_find_saddr(sctp_t *sp, uint32_t saddr)
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
sctp_istrm_find(sctp_t *sp, uint16_t sid, int *errp)
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
sctp_ostrm_find(sctp_t *sp, uint16_t sid, int *errp)
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
STATIC void sctp_heartbeat_timeout(unsigned long data);
STATIC void sctp_retrans_timeout(unsigned long data);
STATIC void sctp_idle_timeout(unsigned long data);
STATIC struct sctp_daddr *
__sctp_daddr_alloc(sctp_t *sp, uint32_t daddr, int *errp)
{
	struct sctp_daddr *sd;
	int dat = inet_addr_type(daddr);
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	if (daddr == INADDR_ANY) {
		*errp = -EDESTADDRREQ;
		ptrace(("ERROR: cannot connect to INADDR_ANY for sp = %p\n", sp));
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
		printd(("WARNING: skipping incompatible %d.%d.%d.%d for sp = %p\n",
			(daddr >> 0) & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff,
			(daddr >> 24) & 0xff, sp));
		return (NULL);
	}
	if (dat != RTN_UNICAST && dat != RTN_LOCAL && dat != RTN_ANYCAST) {
		*errp = -EADDRNOTAVAIL;
		printd(("WARNING: skipping non-unicast %d.%d.%d.%d for sp = %p\n",
			(daddr >> 0) & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff,
			(daddr >> 24) & 0xff, sp));
		return (NULL);
	}
	/* TODO: need to check permissions (TACCES) for broadcast or multicast addresses and whether
	   host addresses are valid (TBADADDR). */
	if ((sd = sctp_dget())) {
		printd(("INFO: Allocating destination address %d.%d.%d.%d for sp = %p\n",
			(daddr >> 0) & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff,
			(daddr >> 24) & 0xff, sp));
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
		sd->max_retrans = sp->rtx_path;	/* max path retrans */
		/* init timers */
		sd_init_timeout(sd, &sd->timer_heartbeat, &sctp_heartbeat_timeout);
		sd_init_timeout(sd, &sd->timer_retrans, &sctp_retrans_timeout);
		sd_init_timeout(sd, &sd->timer_idle, &sctp_idle_timeout);
		printd(("INFO: sd->hb_itvl = %lu\n", sd->hb_itvl));
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
sctp_daddr_include(sctp_t *sp, uint32_t daddr, int *errp)
{
	struct sctp_daddr *sd;
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	if (!(sd = sctp_find_daddr(sp, daddr)))
		sd = __sctp_daddr_alloc(sp, daddr, errp);
	_usual(sd);
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
	sd_del_timeout(sd, &sd->timer_retrans);
	sd_del_timeout(sd, &sd->timer_heartbeat);
	sd_del_timeout(sd, &sd->timer_idle);
	/* Need to free any cached IP routes.  */
	if (sd->dst_cache)
		dst_release(xchg(&sd->dst_cache, NULL));
	if (sd->sp)
		sd->sp->danum--;
	else
		swerr();
	if ((*sd->prev = sd->next))
		sd->next->prev = sd->prev;
	printd(("INFO: Deallocating destination address %d.%d.%d.%d for sp = %p\n",
		(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff, (sd->daddr >> 16) & 0xff,
		(sd->daddr >> 24) & 0xff, sd->sp));
	sd->next = NULL;
	sd->prev = &sd->next;
	sctp_dput(sd);
}

/*
 *  Free all Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_free_daddrs(sctp_t *sp)
{
	struct sock *sk = SCTP_SOCK(sp);
	struct sctp_daddr *sd, *sd_next;
	ensure(sp, return);
	sd_next = sp->daddr;
	_usual(sd_next);
	while ((sd = sd_next)) {
		sd_next = sd->next;
		__sctp_daddr_free(sd);
	}
	unless(sp->danum, sp->danum = 0);
	sp->taddr = NULL;
	sp->raddr = NULL;
	sp->caddr = NULL;
	sk->dport = 0;
	sk->daddr = 0;
}

#ifdef SCTP_CONFIG_ADD_IP
/*
 *  Add a Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_daddr *
sctp_add_daddr(sctp_t *sp, uint32_t daddr)
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
	struct sock *sk;
	struct sk_buff *skb;
	ensure(sd, return);
	ensure(sd->sp, return);
	sp = sd->sp;
	sk = SCTP_SOCK(sp);
	if (!(sd->flags & SCTP_DESTF_UNUSABLE)) {
		/* IMPLEMENTATION NOTE:- This is the lazy way to do this, if we maintained
		   reference counts on sctp_daddr structures, then we could at least check if
		   walking all these queues was necessary. */
		for (skb = skb_peek(&sk->rcvq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->expq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sk->sndq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->urgq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->oooq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->dupq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
#if 0
		for (skb = skb_peek(&sp->ackq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
#endif
		sd_del_timeout(sd, &sd->timer_heartbeat);
		sd_del_timeout(sd, &sd->timer_idle);
		sd->flags |= SCTP_DESTF_UNUSABLE;
		if (sd->dst_cache)
			dst_release(xchg(&sd->dst_cache, NULL));
	}
	if (!sctp_timeout_pending(&sd->timer_retrans) && !sd->in_flight) {
		for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		__sctp_daddr_free(sd);
	}
}
#endif				/* SCTP_CONFIG_ADD_IP */

/*
 *  Allocate a group of Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_alloc_daddrs(sctp_t *sp, uint16_t dport, uint32_t *daddrs, size_t dnum)
{
	int err = 0, allocated = 0;
	ensure(sp, return (-EFAULT));
	ensure(daddrs || !dnum, return (-EFAULT));
	SCTP_SOCK(sp)->dport = dport;
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
		ptrace(("Returning error %d\n", err));
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
sctp_alloc_sock_daddrs(sctp_t *sp, uint16_t dport, struct sockaddr_in *daddrs, size_t dnum)
{
	int err = 0, allocated = 0;
	ensure(sp, return (-EFAULT));
	ensure(daddrs || !dnum, return (-EFAULT));
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
__sctp_saddr_alloc(sctp_t *sp, uint32_t saddr, int *errp)
{
	struct sctp_saddr *ss;
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	if (saddr == INADDR_ANY) {
		printd(("WARNING: skipping INADDR_ANY for sp = %p\n", sp));
		return (NULL);
	}
	if (sp->saddr && LOOPBACK(sp->saddr->saddr) != LOOPBACK(saddr)) {
		*errp = -EADDRNOTAVAIL;
		printd(("WARNING: skipping incompatible %d.%d.%d.%d for sp = %p\n",
			(saddr >> 0) & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff,
			(saddr >> 24) & 0xff, sp));
		return (NULL);
	}
#ifdef sysctl_ip_nonlocal_bind
	if (!sysctl_ip_nonlocal_bind && inet_addr_type(saddr) != RTN_LOCAL) {
		*errp = -EADDRNOTAVAIL;
		printd(("WARNING: skipping non-local %d.%d.%d.%d for sp = %p\n",
			(saddr >> 0) & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff,
			(saddr >> 24) & 0xff, sp));
		return (NULL);
	}
#endif				/* sysctl_ip_nonlocal_bind */
	if ((ss = kmem_cache_alloc(sctp_srce_cachep, SLAB_ATOMIC))) {
		printd(("INFO: Allocating source address %d.%d.%d.%d for sp = %p\n",
			(saddr >> 0) & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff,
			(saddr >> 24) & 0xff, sp));
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
sctp_saddr_include(sctp_t *sp, uint32_t saddr, int *errp)
{
	struct sctp_saddr *ss;
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	if (!(ss = sctp_find_saddr(sp, saddr)))
		ss = __sctp_saddr_alloc(sp, saddr, errp);
	_usual(ss);
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
	printd(("INFO: Deallocating source address %d.%d.%d.%d for sk = %p\n",
		(ss->saddr >> 0) & 0xff, (ss->saddr >> 8) & 0xff, (ss->saddr >> 16) & 0xff,
		(ss->saddr >> 24) & 0xff, ss->sp ? SCTP_SOCK(ss->sp) : NULL));
	bzero(ss, sizeof(*ss));	/* debug */
	ss->prev = &ss->next;
	ss->next = NULL;
	kmem_cache_free(sctp_srce_cachep, ss);
}

/*
 *  Free all Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_free_saddrs(sctp_t *sp)
{
	struct sock *sk = SCTP_SOCK(sp);
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
	sk->rcv_saddr = sk->saddr = 0;
}

/*
 *  Allocate a group of Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_alloc_saddrs(sctp_t *sp, uint16_t sport, uint32_t *saddrs, size_t snum)
{
	struct sock *sk = SCTP_SOCK(sp);
	int err = 0;
	ensure(sp, return (-EFAULT));
	ensure(saddrs || !snum, return (-EFAULT));
	sk->sport = sport;
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
sctp_alloc_sock_saddrs(sctp_t *sp, uint16_t sport, struct sockaddr_in *saddrs, size_t snum)
{
	int err = 0, allocated = 0;
	ensure(sp, return (-EFAULT));
	ensure(saddrs || !snum, return (-EFAULT));
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
	if ((st = kmem_cache_alloc(sctp_strm_cachep, SLAB_ATOMIC))) {
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
	kmem_cache_free(sctp_strm_cachep, st);
}

/*
 *  Free all Streams
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_free_strms(sctp_t *sp)
{
	struct sctp_strm *st, *st_next;
	assert(sp);
	st_next = sp->ostrm;
	_usual(st_next);
	while ((st = st_next)) {
		st_next = st->next;
		sctp_strm_free(st);
	}
	sp->ostr = NULL;
	st_next = sp->istrm;
	_usual(st_next);
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
	for (order = 0; (1UL << order) < goal; order++) ;
	do {
		sctp_vhash_order = order;
		sctp_vhash_size = (1UL << order) * PAGE_SIZE / sizeof(struct sctp_hash_bucket);
		sctp_vhash = (struct sctp_hash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (sctp_vhash == NULL && --order >= 0);
	if (!sctp_vhash)
		panic("%s: Failed to allocate SCTP established hash table\n", __FUNCTION__);
	/* size and allocate bind hash table */
	goal = (((1UL << order) * PAGE_SIZE) / sizeof(struct sctp_bhash_bucket));
	if (goal > (64 * 1024)) {
		goal = (((64 * 1024) * sizeof(struct sctp_bhash_bucket)) / PAGE_SIZE);
		for (order = 0; (1UL << order) < goal; order++) ;
	}
	do {
		sctp_bhash_order = order;
		sctp_bhash_size = (1UL << order) * PAGE_SIZE / sizeof(struct sctp_bhash_bucket);
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
	printd(("INFO: bind hash table configured size = %d\n", sctp_bhash_size));
	printd(("INFO: list hash table configured size = %d\n", sctp_lhash_size));
	printd(("INFO: ptag hash table configured size = %d\n", sctp_phash_size));
	printd(("INFO: vtag cach table configured size = %d\n", sctp_cache_size));
	printd(("INFO: vtag hash table configured size = %d\n", sctp_vhash_size));
	printd(("INFO: tcb  hash table configured size = %d\n", sctp_thash_size));
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
sctp_sp_bhashfn(sctp_t *sp)
{
	return sctp_bhashfn(SCTP_SOCK(sp)->num);
}
STATIC INLINE uint
sctp_sp_lhashfn(sctp_t *sp)
{
	return sctp_lhashfn(SCTP_SOCK(sp)->sport);
}
STATIC INLINE uint
sctp_sp_phashfn(sctp_t *sp)
{
	return sctp_phashfn(sp->p_tag);
}
STATIC INLINE uint
sctp_sp_cachefn(sctp_t *sp)
{
	return (sp->hashent = sctp_cachefn(sp->v_tag));
}
STATIC INLINE uint
sctp_sp_vhashfn(sctp_t *sp)
{
	return sctp_vhashfn(sp->v_tag);
}
STATIC INLINE uint
sctp_sp_thashfn(sctp_t *sp)
{
	return sctp_thashfn(SCTP_SOCK(sp)->sport, SCTP_SOCK(sp)->dport);
}
STATIC void
__sctp_lhash_insert(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_sp_lhashfn(sp)];
	printd(("INFO: Adding socket %p to Listen hashes\n", SCTP_SOCK(sp)));
	write_lock(&hp->lock);
	if (!sp->lprev) {
		if ((sp->lnext = hp->list))
			sp->lnext->lprev = &sp->lnext;
		sp->lprev = &hp->list;
		hp->list = sp;
	} else
		swerr();
	write_unlock(&hp->lock);
}
STATIC void
__sctp_phash_insert(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_sp_phashfn(sp)];
	printd(("INFO: Adding socket %p to Peer Tag hashes\n", SCTP_SOCK(sp)));
	write_lock(&hp->lock);
	if (!sp->pprev) {
		if ((sp->pnext = hp->list))
			sp->pnext->pprev = &sp->pnext;
		sp->pprev = &hp->list;
		hp->list = sp;
	} else
		swerr();
	write_unlock(&hp->lock);
}
STATIC void
__sctp_vhash_insert(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_sp_vhashfn(sp)];
	struct sctp_hash_bucket *cp = &sctp_cache[sctp_sp_cachefn(sp)];
	printd(("INFO: Adding socket %p to Verification Tag hashes\n", SCTP_SOCK(sp)));
	write_lock(&hp->lock);
	if (!sp->pprev) {
		if ((sp->vnext = hp->list))
			sp->vnext->vprev = &sp->vnext;
		sp->vprev = &hp->list;
		hp->list = sp;
		cp->list = sp;
	} else
		swerr();
	write_unlock(&hp->lock);
}
STATIC void
___sctp_thash_insert(sctp_t *sp, struct sctp_hash_bucket *hp)
{
	printd(("INFO: Adding socket %p to TCB hashes\n", SCTP_SOCK(sp)));
	if (!sp->tprev) {
		if ((sp->tnext = hp->list))
			sp->tnext->tprev = &sp->tnext;
		sp->tprev = &hp->list;
		hp->list = sp;
	} else
		swerr();
}

#if SOCKETS
/* STREAMS doesn't need the locking version, sockets do. */
STATIC void
__sctp_thash_insert(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_sp_thashfn(sp)];
	write_lock(&hp->lock);
	___sctp_thash_insert(sp, hp);
	write_unlock(&hp->lock);
}
#endif				/* SOCKETS */
STATIC void
__sctp_lhash_unhash(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_sp_lhashfn(sp)];
	printd(("INFO: Removing socket %p from Listen hashes\n", SCTP_SOCK(sp)));
	if (sp->lprev) {
		write_lock(&hp->lock);
		if ((*(sp->lprev) = sp->lnext))
			sp->lnext->lprev = sp->lprev;
		sp->lnext = NULL;
		sp->lprev = NULL;
		write_unlock(&hp->lock);
	} else
		swerr();
}
STATIC void
__sctp_phash_unhash(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_sp_phashfn(sp)];
	printd(("INFO: Removing socket %p from Peer Tag hashes\n", SCTP_SOCK(sp)));
	if (sp->pprev) {
		write_lock(&hp->lock);
		if ((*(sp->pprev) = sp->pnext))
			sp->pnext->pprev = sp->pprev;
		sp->pnext = NULL;
		sp->pprev = NULL;
		write_unlock(&hp->lock);
	} else
		swerr();
}
STATIC void
__sctp_vhash_unhash(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_sp_vhashfn(sp)];
	printd(("INFO: Removing socket %p from Verification Tag hashes\n", SCTP_SOCK(sp)));
	if (sp->vprev) {
		write_lock(&hp->lock);
		if ((*(sp->vprev) = sp->vnext))
			sp->vnext->vprev = sp->vprev;
		sp->vnext = NULL;
		sp->vprev = NULL;
		if (sctp_cache[sp->hashent].list == sp)
			sctp_cache[sp->hashent].list = NULL;
		sp->hashent = 0;
		write_unlock(&hp->lock);
	} else
		swerr();
}
STATIC void
__sctp_thash_unhash(sctp_t *sp)
{
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_sp_thashfn(sp)];
	printd(("INFO: Removing socket %p from TCB hashes\n", SCTP_SOCK(sp)));
	if (sp->tprev) {
		write_lock(&hp->lock);
		if ((*(sp->tprev) = sp->tnext))
			sp->tnext->tprev = sp->tprev;
		sp->tnext = NULL;
		sp->tprev = NULL;
		write_unlock(&hp->lock);
	} else
		swerr();
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
sctp_bindb_get(unsigned short snum)
{
	struct sctp_bind_bucket *sb;
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	write_lock_bh(&hp->lock);
	printd(("INFO: Getting bind bucket for port = %d\n", snum));
	for (sb = hp->list; sb && sb->port != snum; sb = sb->next) ;
	return (sb);
}
STATIC void
sctp_bindb_put(unsigned short snum)
{
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	(void) hp;
	printd(("INFO: Putting bind bucket for port = %d\n", snum));
	write_unlock_bh(&hp->lock);
}
STATIC INLINE struct sctp_bind_bucket *
__sctp_bindb_create(unsigned short snum)
{
	struct sctp_bind_bucket *sb;
	if ((sb = kmem_cache_alloc(sctp_bind_cachep, SLAB_ATOMIC))) {
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
__sctp_bhash_insert(sctp_t *sp, struct sctp_bind_bucket *sb)
{
	struct sock *sk = SCTP_SOCK(sp);
	if (!sp->bprev) {
		printd(("INFO: Adding socket %p to bind bucket at port = %d\n", sk, sb->port));
		if ((sp->bnext = sb->owners))
			sp->bnext->bprev = &sp->bnext;
		sp->bprev = &sb->owners;
		sb->owners = sp;
		sp->bindb = sb;
		sk->num = sb->port;
		sk->sport = htons(sb->port);
	} else if (sk->state == SCTP_LISTEN) {
		printd(("INFO: Re-adding listening socket %p to bind bucket at port = %d\n", sk,
			sb->port));
	} else
		swerr();
	if (!sk->reuse || sk->state == SCTP_LISTEN)
		sb->fastreuse = 0;
}

STATIC void
__sctp_inherit_port(struct sock *sk, struct sock *lsk)
{
	struct sctp_bind_bucket *sb;
	sk->num = lsk->num;
	sk->sport = lsk->sport;
	__sctp_bindb_get(sk->num);
	if ((sb = SCTP_PROT(lsk)->bindb))
		__sctp_bhash_insert(SCTP_PROT(sk), sb);
	else
		swerr();
	__sctp_bindb_put(sk->num);
}
STATIC void
__sctp_bhash_unhash(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_bhash_bucket *bp = &sctp_bhash[sctp_sp_bhashfn(sp)];
	write_lock(&bp->lock);
	if (sp->bprev) {
		printd(("INFO: Removing socket %p from bind bucket at port = %d, num = %d\n", sk,
			sp->bindb ? sp->bindb->port : -1U, sk->num));
		if ((*(sp->bprev) = sp->bnext))
			sp->bnext->bprev = sp->bprev;
		sp->bnext = NULL;
		sp->bprev = NULL;
		if (sp->bindb) {
			if (sp->bindb->owners == NULL) {
				struct sctp_bind_bucket *sb = sp->bindb;
				printd(("INFO: Deallocating bind bucket for port = %d\n",
					sb->port));
				if (sb->prev) {
					if ((*(sb->prev) = sb->next))
						sb->next->prev = sb->prev;
					sb->next = NULL;
					sb->prev = NULL;
					bzero(sb, sizeof(*sb));	/* debug */
					kmem_cache_free(sctp_bind_cachep, sb);
				} else
					swerr();
			}
			sp->bindb = NULL;
		} else
			swerr();
	} else
		swerr();
	sk->num = 0;
	sk->sport = 0;
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
__sctp_get_addrs(sctp_t *sp, uint32_t daddr)
{
	int allocated = 0;
	struct net_device *dev;
	read_lock(&dev_base_lock);
	read_lock(&inetdev_lock);
	for (dev = dev_base; dev; dev = dev->next) {
		struct in_device *in_dev;
		struct in_ifaddr *ifa;
		if (!(in_dev = __in_dev_get(dev)))
			continue;
		read_lock(&in_dev->lock);
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
		read_unlock(&in_dev->lock);
	}
	read_unlock(&inetdev_lock);
	read_unlock(&dev_base_lock);
	return (allocated == 0);
}
STATIC int
sctp_get_addrs(sctp_t *sp, uint32_t daddr)
{
	int ret;
	local_bh_disable();
	ret = __sctp_get_addrs(sp, daddr);
	local_bh_enable();
	return (ret);
}

/*
 *  Add to Connection Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This also checks for conflicts.  This uses the rule that there cannot be more than one SCTP
 *  association between an given pair of local and remote transport addresses (IP/port).
 */
STATIC int
sctp_conn_hash(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk), *sp2;
	struct sctp_hash_bucket *hp;
	struct sock *sk2;
	struct sctp_saddr *ss, *ss2, *ss_next;
	struct sctp_daddr *sd, *sd2;
	assert(sk);
	local_bh_disable();
#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (sp->tprev) {
		swerr();
		__sctp_thash_unhash(sp);
	}
	if (sp->vprev) {
		swerr();
		__sctp_vhash_unhash(sp);
	}
	if (sp->pprev) {
		swerr();
		__sctp_phash_unhash(sp);
	}
#endif				/* SCTP_CONFIG_DEBUG */
	/* Check for conflicts: we search the TCB hash list for other sockets with the same port
	   pair and with the same pairing of src and dst addresses.  We do this with write locks on 
	   for the hash list so that we can add our socket if we succeed. This is the TCB hash, so
	   even if this is a long search, we are only blocking other connection hash calls and TCB
	   lookups (for ootb packets) for this hash bucket. */
	hp = &sctp_thash[sctp_sp_thashfn(sp)];
	write_lock(&hp->lock);
	for (sp2 = hp->list; sp2; sp2 = sp2->tnext) {
		sk2 = SCTP_SOCK(sp2);
		if (sk2->sport != sk->sport || sk2->dport != sk->dport)
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
						   check if we are SOCK_BINDADDR_LOCK'd.  If we are 
						   not, then we were bound to INADDR_ANY and (for
						   connect) we are permitted to kick addresses out
						   of the source address list to resolve conflicts. 
						 */
						if (!(sk->userlocks & SOCK_BINDADDR_LOCK)) {
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
	if (!(sk->userlocks & SOCK_BINDADDR_LOCK) && !sp->saddr) {
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
STATIC struct sock *
sctp_lookup_listen(uint16_t dport, uint32_t daddr)
{
	sctp_t *sp, *result = NULL;
	int hiscore = 0;
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_lhashfn(dport)];
	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->lnext) {
		struct sock *sk = SCTP_SOCK(sp);
		int score = 0;
		if (sk->sport) {
			if (sk->sport != dport)
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
			ptrace(("ERROR: Unassigned port number socket = %p\n", sk));
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
	read_unlock(&hp->lock);
	usual(result);
	if (result) {
		sctp_hold(result);
		return SCTP_SOCK(result);
	}
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
STATIC struct sock *
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
			struct sock *sk = SCTP_SOCK(sp);
			int score = 0;
			if ((1 << sk->state) & (SCTPF_DISCONNECTED))
				continue;
			if (!(sk->userlocks & SOCK_BINDPORT_LOCK))
				continue;
			if (sk->sport) {
				if (sk->sport != dport)
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
				_ptrace(("ERROR: Unassigned port number socket = %p\n", sk));
			}
			if (score > 0) {
				result = sp;
				break;
			}
			if (score > hiscore) {
				hiscore = score;
				result = sp;
			}
		}
	}
	read_unlock(&hp->lock);
	usual(result);
	if (result) {
		sctp_hold(result);
		return SCTP_SOCK(result);
	}
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
#define sctp_match_tcb(__sk, __saddr, __daddr, __sport, __dport) \
	( ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) && \
	  (sctp_find_saddr(SCTP_PROT(__sk),(__saddr))) && \
	  (sctp_find_daddr(SCTP_PROT(__sk),(__daddr))) )

STATIC struct sock *
sctp_lookup_tcb(uint16_t sport, uint16_t dport, uint32_t saddr, uint32_t daddr)
{
	sctp_t *sp;
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_thashfn(sport, dport)];
	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->tnext)
		if (sctp_match_tcb(SCTP_SOCK(sp), saddr, daddr, sport, dport))
			break;
	read_unlock(&hp->lock);
	if (sp) {
		sctp_hold(sp);
		return SCTP_SOCK(sp);
	}
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
#define	sctp_match_ptag(__sk, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == SCTP_PROT(__sk)->p_tag) && \
	  ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) )
#else				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */
#define	sctp_match_ptag(__sk, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == SCTP_PROT(__sk)->p_tag) && \
	  ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) && \
	  (sctp_find_saddr(SCTP_PROT(__sk),(__saddr))) && \
	  (sctp_find_daddr(SCTP_PROT(__sk),(__daddr))) )
#endif				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */

STATIC struct sock *
sctp_lookup_ptag(uint32_t p_tag, uint16_t sport, uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	sctp_t *sp;
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_phashfn(p_tag)];
	(void) saddr;
	(void) daddr;
	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->pnext)
		if (sctp_match_ptag(SCTP_SOCK(sp), saddr, daddr, p_tag, sport, dport))
			break;
	read_unlock(&hp->lock);
	if (sp) {
		sctp_hold(sp);
		return SCTP_SOCK(sp);
	}
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
#define sctp_match_vtag(__sk, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == SCTP_PROT(__sk)->v_tag) )
#else				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */
#define sctp_match_vtag(__sk, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == SCTP_PROT(__sk)->v_tag) && \
	  ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) && \
	  (sctp_find_saddr(SCTP_PROT(__sk),(__daddr))) )
#endif				/* !defined(SCTP_CONFIG_SLOW_VERIFICATION) ||
				   defined(SCTP_CONFIG_ADD_IP) */

STATIC struct sock *
sctp_lookup_vtag(uint32_t v_tag, uint16_t sport, uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	sctp_t *sp;
	unsigned int hash = sctp_cachefn(v_tag);
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_vhashfn(v_tag)];
	(void) saddr;
	(void) daddr;
	read_lock(&hp->lock);
	if (!(sp = sctp_cache[hash].list)
	    || !sctp_match_vtag(SCTP_SOCK(sp), saddr, daddr, v_tag, sport, dport)) {
		for (sp = hp->list; sp; sp = sp->vnext)
			if (sctp_match_vtag(SCTP_SOCK(sp), saddr, daddr, v_tag, sport, dport)) {
				sctp_cache[hash].list = sp;
				break;
			}
	}
	read_unlock(&hp->lock);
	if (sp) {
		sctp_hold(sp);
		return SCTP_SOCK(sp);
	}
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP COOKIE ECHO - Special lookup rules for cookie echo chunks
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sock *
sctp_lookup_cookie_echo(struct sctp_cookie *ck, uint32_t v_tag, uint16_t sport, uint16_t dport,
			uint32_t daddr, uint32_t saddr)
{
	struct sock *sk = NULL;
	/* quick sanity checks on cookie */
	if (ck->v_tag == v_tag && ck->sport == sport && ck->dport == dport) {
		if (		/* RFC 2960 5.2.4 (A) */
			   (ck->l_ttag && ck->p_ttag
			    && (sk = sctp_lookup_vtag(ck->l_ttag, sport, dport, saddr, daddr))
			    && sk == sctp_lookup_ptag(ck->p_ttag, sport, dport, saddr, daddr))
			   /* RFC 2960 5.2.4 (B) or (D) */
			   || ((sk = sctp_lookup_vtag(v_tag, sport, dport, saddr, daddr)))
			   /* RFC 2960 5.2.4 (C) or IG 2.6 replacement */
			   || (!ck->l_ttag && !ck->p_ttag
			       && ((sk = sctp_lookup_ptag(ck->p_tag, sport, dport, saddr, daddr))
				   || (sk = sctp_lookup_tcb(sport, dport, saddr, ck->daddr))))
			   /* RFC 2960 5.1 */
			   || ((sk = sctp_lookup_listen(sport, daddr))))
			if (sk->state == SCTP_LISTEN || (sk->userlocks & SOCK_BINDPORT_LOCK))
				return (sk);
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
STATIC struct sock *
sctp_lookup(struct sctphdr *sh, uint32_t daddr, uint32_t saddr)
{
	struct sock *sk = NULL;
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
			if ((sk = sctp_lookup_vtag(v_tag, dport, sport, daddr, saddr)))
				return (sk);
			if (ctype == SCTP_CTYPE_ABORT)
				/* check abort for conn ind */
				if ((sk = sctp_lookup_listen(dport, daddr)))
					return (sk);
		case SCTP_CTYPE_INIT:
			break;
		case SCTP_CTYPE_COOKIE_ECHO:{
			struct sctp_cookie *ck = (struct sctp_cookie *)
			    ((struct sctp_cookie_echo *) ch)->cookie;
			return sctp_lookup_cookie_echo(ck, v_tag, dport, sport, daddr, saddr);
		}
		}
	} else if (ctype == SCTP_CTYPE_INIT)
		if ((sk = sctp_lookup_listen(dport, daddr))
		    || (sk = sctp_lookup_bind(dport, daddr)))
			return (sk);
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
		struct sock *sk;
		if (!(ret = secure_tcp_sequence_number(daddr, saddr, dport, sport)))
			continue;
		if ((sk = sctp_lookup_vtag(ret, sport, dport, saddr, daddr))) {
			sock_put(sk);
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
sctp_get_key(sctp_t *sp)
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
sctp_hmac(sctp_t *sp, uint8_t *text, int tlen, uint8_t *key, int klen, uint8_t *hmac)
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
sctp_sign_cookie(sctp_t *sp, struct sctp_cookie *ck)
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
sctp_verify_cookie(sctp_t *sp, struct sctp_cookie *ck)
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
STATIC INLINE size_t sctp_avail(sctp_t *sp, struct sctp_daddr *sd);
STATIC INLINE struct sctp_daddr *
sctp_break_tie(sctp_t *sp, struct sctp_daddr *sd1, struct sctp_daddr *sd2)
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
sctp_rate_route(sctp_t *sp, struct sctp_daddr *sd)
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
sctp_choose_best(sctp_t *sp)
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
#if defined(SCTP_CONFIG_DEBUG) && defined(SCTP_CONFIG_ERROR_GENERATOR)
#define SCTP_CONFIG_ERROR_GENERATOR_LEVEL  8
#define SCTP_CONFIG_ERROR_GENERATOR_LIMIT 13
#define SCTP_CONFIG_BREAK_GENERATOR_LEVEL 50
#define SCTP_CONFIG_BREAK_GENERATOR_LIMIT 200
#endif				/* defined(SCTP_CONFIG_DEBUG) &&
				   defined(SCTP_CONFIG_ERROR_GENERATOR) */
STATIC INLINE int
dst_check(struct dst_entry **dstp)
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

STATIC int
sctp_update_routes(struct sock *sk, int force_reselect)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err = -EHOSTUNREACH;
	int mtu_changed = 0;
	int viable_route = 0;
	int route_changed = 0;
	struct sctp_daddr *sd;
	struct sctp_daddr *taddr;
	int old_pmtu;
	assert(sk);
	ensure(sp->daddr, return (-EFAULT));
	old_pmtu = xchg(&sp->pmtu, INT_MAX);	/* big enough? */
	sk->route_caps = -1L;
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
		printd(("INFO: Checking socket %p route for %d.%d.%d.%d\n", sk,
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff, (sd->daddr >> 16) & 0xff,
			(sd->daddr >> 24) & 0xff));
		if (!sd->dst_cache || (sd->dst_cache->obsolete && !dst_check(&sd->dst_cache))) {
			rt = NULL;
			sd->saddr = 0;
			route_changed = 1;
			/* try wildcard saddr and dif routing */
#if defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
			err = ip_route_connect(&rt, sd->daddr, 0, RT_CONN_FLAGS(sk), 0);
#elif defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
			err =
			    ip_route_connect(&rt, sd->daddr, 0, RT_CONN_FLAGS(sk), 0, IPPROTO_SCTP,
					     sk->sport, sk->dport, NULL);
#else				/* defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY */
#error One of HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL or HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY must be defined.
#endif				/* defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY */
			if (err < 0 || !rt || rt->u.dst.obsolete) {
				rare();
				if (rt)
					ip_rt_put(rt);
				if (err == 0)
					err = -EHOSTUNREACH;
				continue;
			}
			if (rt->rt_flags & (RTCF_MULTICAST | RTCF_BROADCAST)
			    && !sk->broadcast) {
				rare();
				ip_rt_put(rt);
				err = -ENETUNREACH;
				continue;
			}
			sd->saddr = rt->rt_src;
			if (!sctp_find_saddr(sp, sd->saddr)) {
				rare();
#ifdef SCTP_CONFIG_ADD_IP
				/* Candidate for ADD-IP but we can't use it yet */
				if (sp->p_caps & SCTP_CAPS_ADD_IP
				    && !(sk->userlocks & SOCK_BINDADDR_LOCK)) {
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
		if (sysctl_ip_dynaddr && sk->state == SCTP_COOKIE_WAIT && sd == sp->daddr) {
			/* see if route changed on primary as result of INIT that was discarded */
			struct rtable *rt2 = NULL;
#if defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
			if (!ip_route_connect(&rt2, rt->rt_dst, 0, RT_CONN_FLAGS(sk), sd->dif))
#elif defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
			if (!ip_route_connect
			    (&rt2, rt->rt_dst, 0, RT_CONN_FLAGS(sk), sd->dif, IPPROTO_SCTP,
			     sk->sport, sk->dport, NULL))
#else				/* defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY */
#error One of HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL or HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY must be defined.
#endif				/* defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY */
			{
				if (rt2->rt_src != rt->rt_src) {
					rare();
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
		sk->route_caps &= rt->u.dst.dev->features;	/* XXX */
		if (sd->mtu != dst_pmtu(&rt->u.dst)) {
			sd->mtu = dst_pmtu(&rt->u.dst);
			sd->dmps =
			    sd->mtu - sp->ext_header_len - sizeof(struct iphdr) -
			    sizeof(struct sctphdr);
			mtu_changed = 1;
			rare();
		}
		if (sp->pmtu > sd->mtu) {
			sp->pmtu = sd->mtu;
			sp->amps = sd->dmps;
		}
	}
	if (!viable_route) {
		rare();
		/* set defaults */
		sp->taddr = sp->daddr;
		sp->pmtu = ip_rt_min_pmtu;
		sp->amps =
		    sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
		__ptrace(("ERROR: no viable route\n"));
		return (err);
	}
	/* if we have made or need changes then we want to reanalyze routes */
	if (force_reselect || route_changed || mtu_changed || sp->pmtu != old_pmtu || !sp->taddr) {
#if defined(SCTP_CONFIG_DEBUG) && defined(SCTP_CONFIG_ERROR_GENERATOR)
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

/*
 *  XMIT OOTB (Disconnect Send with no Listening Socket or STREAM).
 *  -------------------------------------------------------------------------
 *  This sends disconnected without a socket or stream.  All that is needed is a destination address
 *  and a socket buffer or message block.  The only time that we use this is for responding to OOTB
 *  packets with ABORT or SHUTDOWN COMPLETE.
 */
STATIC void
sctp_xmit_ootb(uint32_t daddr, uint32_t saddr, struct sk_buff *skb)
{
	struct rtable *rt = NULL;
	struct sk_buff *skb2 = skb;
	ensure(skb, return);
	if (!ip_route_output(&rt, daddr, 0, 0, 0)) {
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = skb->len;
		size_t tlen = plen + sizeof(struct iphdr);
		size_t head = hlen + sizeof(struct iphdr);
		usual(hlen);
		usual(plen);
		/* IMPLEMENTATION NOTE:- Sockets ensures that there is sufficient headroom in the
		   passed in sk_buff, fills out the IP header, calculates the checksum and sends
		   the buffer.  For STREAMS, no socket buffer is passed in, just a message block,
		   so a new socket buffer is allocated with the necessary headroom and the message
		   blocks are copied to the socket buffer. */
		if (head > skb_headroom(skb2))
			skb = skb_realloc_headroom(skb2, head);
		if (skb) {
			struct iphdr *iph;
			struct sctphdr *sh;
			/* find headers */
			iph = (typeof(iph)) skb_push(skb, sizeof(*iph));
			sh = (typeof(sh)) skb->data;
			skb->dst = &rt->u.dst;
			skb->priority = 0;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = 0;
			iph->frag_off = 0;
			iph->ttl = sysctl_ip_default_ttl;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = 132;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);
			/* For sockets the passed in sk_buff represents a single packet.  For
			   STREAMS, the passed in mblk_t pointer is possibly a message buffer chain
			   and we must iterate along the b_cont pointer. */
			/* TODO:- For STREAMS it would be better to combine copying the buffer
			   segments above with performing the checking below.  */
			sh->check = 0;
			if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				sh->check = htonl(cksum_generate(sh, plen));
			SCTP_INC_STATS(SctpOutSCTPPacks);
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
			return;
		} else
			rare();
	} else
		rare();
	/* sending OOTB reponses are one time events, if we can't send the message we just drop it, 
	   the peer will probably come back again later */
	kfree_skb(skb2);
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
STATIC INLINE void freechunks(struct sk_buff *skb);
STATIC void
sctp_xmit_msg(uint32_t saddr, uint32_t daddr, struct sk_buff *skb, struct sock *sk)
{
	struct inet_opt *ip = &sk->protinfo.af_inet;
	struct rtable *rt = NULL;
	struct sk_buff *skb2 = skb;
	ensure(skb, return);
	if (!ip_route_output(&rt, daddr, saddr, RT_TOS(ip->tos) | sk->localroute, 0)) {
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = skb->len;
		size_t tlen = plen + sizeof(struct iphdr);
		size_t head = hlen + sizeof(struct iphdr);
		_usual(hlen);
		_usual(plen);
		if (head > skb_headroom(skb2))
			skb = skb_realloc_headroom(skb2, head);
		if (skb) {
			struct sctphdr *sh = (typeof(sh)) skb->data;
			struct iphdr *iph = (typeof(iph)) skb_push(skb, sizeof(*iph));
			_printd(("INFO: Sending messsage %d.%d.%d.%d -> %d.%d.%d.%d\n",
				 (saddr >> 0) & 0xff, (saddr >> 8) & 0xff, (saddr >> 16) & 0xff,
				 (saddr >> 24) & 0xff, (daddr >> 0) & 0xff, (daddr >> 8) & 0xff,
				 (daddr >> 16) & 0xff, (daddr >> 24) & 0xff));
			skb->dst = &rt->u.dst;
			skb->priority = sk->priority;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = ip->tos;
			iph->frag_off = 0;
#if defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
			iph->ttl = ip->ttl;
#elif defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
			iph->ttl = ip->uc_ttl;
#endif				/* defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = sk->protocol;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);
			/* For sockets the passed in sk_buff represents a single packet.  For
			   STREAMS, the passed in mblk_t pointer is possibly a message buffer chain
			   and we must iterate along the b_cont pointer. */
			/* TODO:- For STREAMS it would be better to combine copying the buffer
			   segments above with performing the checking below.  */
			sh->check = 0;
			if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				sh->check = htonl(cksum(sk, sh, plen));
			SCTP_INC_STATS(SctpOutSCTPPacks);
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
			return;
		} else
			rare();
	} else
		rare();
	/* sending INIT ACKs are one time events, if we can't get the response off, we just drop
	   the INIT ACK: the peer should send us another INIT * in a short while... */
	kfree_skb(skb2);
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
sctp_send_msg(struct sock *sk, struct sctp_daddr *sd, struct sk_buff *skc)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct inet_opt *ip = &sp->inet;
	struct sk_buff *skb;
	struct net_device *dev;
	size_t plen, hlen, tlen;
	ensure(sk, return);
	ensure(sd, return);
	ensure(skc, return);
	ensure(sd->dst_cache, return);
	ensure(!sd->dst_cache->obsolete, return);
	dev = sd->dst_cache->dev;
	plen = SCTP_SKB_CB(skc)->dlen;
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
			printd(("WARNING: Dropping packet\n"));
			return;
		}
	}
#endif				/* (defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST) &&
				   defined SCTP_CONFIG_ERROR_GENERATOR */
	_printd(("INFO: Preparing message sk %p, hlen %u, plen %u, tlen %u\n", sk, hlen, plen,
		 tlen));
	_unusual(plen == 0 || plen > 1 << 15);
	/* IMPLEMENTATION NOTE:- We could clone these sk_buffs or dup these mblks and put them into 
	   a fraglist, however, this would require copying the sk_buff header on each data chunk
	   (just to get a new next pointer). Unfortunately, for the most part in SCTP, data chunks
	   are very small: even smaller than the sk_buff header so it is probably not worth cloning 
	   or duping versus copying messages.  */
	/* A workable sendpages might be a better approach to larger data chunks.  */
	if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
		struct sk_buff *skp;
		sctp_tcb_t *cb;
		struct iphdr *iph;
		struct sctphdr *sh;
		unsigned char *head, *data;
		size_t alen = 0;
		_printd(("INFO: Sending messsage %d.%d.%d.%d -> %d.%d.%d.%d\n",
			 (sd->saddr >> 0) & 0xff, (sd->saddr >> 8) & 0xff, (sd->saddr >> 16) & 0xff,
			 (sd->saddr >> 24) & 0xff, (sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
			 (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff));
		skb_reserve(skb, hlen);
		iph = (struct iphdr *) __skb_put(skb, tlen);	/* XXX */
		sh = (struct sctphdr *) (iph + 1);
		head = data = (unsigned char *) sh;
		skb->dst = dst_clone(sd->dst_cache);
		skb->priority = sk->priority;
		iph->version = 4;
		iph->ihl = 5;
		iph->tos = ip->tos;
		iph->frag_off = 0;
#if defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_TTL
		iph->ttl = ip->ttl;
#elif defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL
		iph->ttl = ip->uc_ttl;
#endif				/* defined HAVE_KMEMB_STRUCT_SOCK_PROTINFO_AF_INET_UC_TTL */
		iph->daddr = sd->daddr;	/* XXX */
		iph->saddr = sd->saddr;
		iph->protocol = sk->protocol;
		iph->tot_len = htons(tlen);
		skb->nh.iph = iph;
		__ip_select_ident(iph, sd->dst_cache);
		/* For sockets, socket buffers representing chunks are chained together by control
		   block pointers, however, each socket buffer in the chain is a complete chunk.
		   For message blocks, blocks representing chunks are chained together with the
		   b_next pointer, but each chunk can consist of one or more segments chained
		   together by the b_cont pointer. */
		for (cb = SCTP_SKB_CB(skc); cb; cb = cb->next) {
			size_t pad, blen, clen = 0;
			skp = SCTP_CB_SKB(cb);
			/* for destination RTT calculation */
			cb->daddr = sd;
			cb->when = jiffies;
			cb->flags |= SCTPCB_FLAG_SENT;	/* should have been set by caller */
			cb->trans++;
			/* For sockets the passed in sk_buff represents a single packet.  For
			   STREAMS, the passed in mblk_t pointer is possibly a message buffer chain
			   and we must iterate along the b_cont pointer. */
			if ((blen = skp->len) > 0) {
				ensure(head + plen >= data + blen, kfree_skb(skb); return);
				bcopy(skp->data, data, blen);
				data += blen;
				clen += blen;
			}
			/* pad each chunk if not padded already */
			pad = PADC(clen) - clen;
			ensure(head + plen >= data + pad, kfree_skb(skb); return);
			bzero(data, pad);
			data += pad;
			alen += clen + pad;
		}
		if (alen != plen) {
			_usual(alen == plen);
			ptrace(("alen = %u, plen = %u discarding\n", alen, plen));
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
			sh->check = htonl(cksum(sk, sh, plen));
		SCTP_INC_STATS(SctpOutSCTPPacks);
		NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
		/* Whenever we transmit something, we expect a reply to our v_tag, so we put
		   ourselves in the 1st level vtag caches expecting a quick reply. */
		if (!((1 << sk->state) & (SCTPF_DISCONNECTED)))
			sctp_cache[sp->hashent].list = sp;
	} else {
		ptrace(("ERROR: couldn't allocate skbuf len %u\n", hlen + tlen));
	}
}

/*
 *  ==========================================================================
 *
 *  Socket Buffer Management
 *
 *  ==========================================================================
 *  All of the following perambulations are for the management of socket buffers.  Unlike STREAMS
 *  (and I don't know that it really follows BSD either), Linux needs to have buffer occupancy in
 *  terms of bytes of data managed separate from the socket buffer queues themselves.  Under STREAMS
 *  we use our own buffer management routines for this.
 */

#if SOCKETS
STATIC INLINE int
sctp_rspace(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	int amt = 0;
	if (!(sk->shutdown & RCV_SHUTDOWN)) {
		amt = sk->rcvbuf - atomic_read(&sk->rmem_alloc);
		if (PADC(amt + sizeof(struct sctp_data)) < sp->amps)
			amt = 0;
	}
	return amt;
}
STATIC INLINE int
sctp_min_wspace(struct sock *sk)
{
	return atomic_read(&sk->wmem_alloc) >> 1;
}
STATIC INLINE int
sctp_wspace(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	int amt = 0;
	if (!(sk->shutdown & SEND_SHUTDOWN)) {
		ensure(atomic_read(&sk->wmem_alloc) >= 0, atomic_set(&sk->wmem_alloc, 0));
		amt = sk->sndbuf - atomic_read(&sk->wmem_alloc);
		if (PADC(amt + sizeof(struct sctp_data)) < sp->amps)
			amt = 0;
	}
	return amt;
}
STATIC void
sctp_write_space(struct sock *sk)
{
	struct socket *sock = sk->socket;
	if (sctp_wspace(sk) >= sctp_min_wspace(sk) && sock) {
		clear_bit(SOCK_NOSPACE, &sock->flags);
		if (sk->sleep && waitqueue_active(sk->sleep))
			wake_up_interruptible(sk->sleep);
		if (sock->fasync_list && !(sk->shutdown & SEND_SHUTDOWN))
			sock_wake_async(sock, 2, POLL_OUT);
	}
}
STATIC void
sctp_rfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;
	sctp_t *sp = SCTP_PROT(sk);
	atomic_sub(SCTP_SKB_CB(skb)->dlen, &sk->rmem_alloc);
	if (!(sk->shutdown & RCV_SHUTDOWN)) {
		/* SCTP IG 2.15.2, Receive SWS Avoidance RFC 1122 4.2.3.3 */
		size_t rbuf = sk->rcvbuf >> 1;
		size_t amps = sp->amps;
		size_t alloc = atomic_read(&sk->rmem_alloc);
		size_t a_rwnd = (sk->rcvbuf > alloc) ? sk->rcvbuf - alloc : 0;
		if (a_rwnd >= sp->a_rwnd + ((amps < rbuf) ? amps : rbuf)) {
			sp->a_rwnd = a_rwnd;
			sp->sackf |= SCTP_SACKF_WND;	/* RFC 2960 6.2 */
		}
	}
}
STATIC void
sctp_wfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;
	atomic_sub(skb->truesize, &sk->wmem_alloc);
	ensure(atomic_read(&sk->wmem_alloc) >= 0, atomic_set(&sk->wmem_alloc, 0));
	if (!sk->use_write_queue)
		sk->write_space(sk);
	sock_put(sk);
}
STATIC INLINE void
sctp_set_owner_r(struct sk_buff *skb, struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	skb->sk = sk;
	skb->destructor = sctp_rfree;
	atomic_add(SCTP_SKB_CB(skb)->dlen, &sk->rmem_alloc);
	if (!(sk->shutdown & RCV_SHUTDOWN)) {
		/* SCTP IG 2.15.2, Receive SWS Avoidance RFC 1122 4.2.3.3 */
		size_t rbuf = sk->rcvbuf >> 1;
		size_t amps = sp->amps;
		size_t alloc = atomic_read(&sk->rmem_alloc);
		size_t a_rwnd = (sk->rcvbuf > alloc) ? sk->rcvbuf - alloc : 0;
		if (a_rwnd > amps || a_rwnd > rbuf)
			sp->a_rwnd = a_rwnd;
		else
			sp->a_rwnd = 0;
	}
}
STATIC INLINE void
sctp_set_owner_w(struct sk_buff *skb, struct sock *sk)
{
	sock_hold(sk);
	skb->sk = sk;
	skb->destructor = sctp_wfree;
	atomic_add(skb->truesize, &sk->wmem_alloc);
	ensure(atomic_read(&sk->wmem_alloc) >= 0, atomic_set(&sk->wmem_alloc, 0));
}
STATIC INLINE void
sctp_get_owner_r(struct sk_buff *new, struct sk_buff *old)
{
	new->sk = old->sk;
	new->destructor = old->destructor;
	old->sk = NULL;
	old->destructor = NULL;
}
STATIC INLINE void
sctp_put_owner_r(struct sk_buff *skb)
{
	if (skb->destructor)
		skb->destructor(skb);
	skb->sk = NULL;
	skb->destructor = NULL;
}
STATIC struct sk_buff *
sctp_wmalloc(struct sock *sk, unsigned long size, int force, int priority)
{
	if (force || atomic_read(&sk->wmem_alloc) < sk->sndbuf) {
		struct sk_buff *skb;
		if ((skb = alloc_skb(size, priority))) {
			sctp_set_owner_w(skb, sk);
			return skb;
		}
	}
	return NULL;
}
#endif				/* SOCKETS */

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
	sctp_tcb_t **dpp;		/* place to link buffer */
	size_t mrem;			/* reminaing payload in packet */
	size_t mlen;			/* current length of message */
	size_t swnd;			/* remaining send window */
	size_t pbuf;			/* peer buffer */
};

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
sctp_avail(sctp_t *sp, struct sctp_daddr *sd)
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

STATIC struct sk_buff *sctp_alloc_chk(struct sock *sk, size_t clen, size_t dlen);
/*
 *  BUNDLING FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  BUNDLE SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_sack(struct sock *sk,	/* association */
		 struct sctp_daddr *sd,	/* destination */
		 struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_sack *m;
	size_t ngaps = sp->ngaps;
	size_t ndups = sp->ndups;
	size_t glen = ngaps * sizeof(uint32_t);
	size_t dlen = ndups * sizeof(uint32_t);
	size_t clen = sizeof(*m) + glen + dlen;
	size_t plen = PADC(clen);
#ifdef SCTP_CONFIG_ECN
	struct sctp_ecne *e;
	size_t elen = ((sp->sackf & SCTP_SACKF_ECN) ? sizeof(*e) : 0);
#endif				/* SCTP_CONFIG_ECN */
	assert(sk);
#ifdef SCTP_CONFIG_ECN
	plen += PADC(elen);
#endif				/* SCTP_CONFIG_ECN */
	if ((1 << sk->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen > sd->dmps) {
		size_t too_many_dups;
		rare();		/* trim down sack */
		too_many_dups = (clen - ckp->mrem + 3) / sizeof(uint32_t);
		ndups = (ndups > too_many_dups) ? ndups - too_many_dups : 0;
		clen = sizeof(*m) + glen + ndups * sizeof(uint32_t);
		if (ckp->mrem < clen) {
			size_t too_many_gaps;
			rare();	/* trim some more */
			too_many_gaps = (clen - ckp->mrem + 3) / sizeof(uint32_t);
			ngaps = (ngaps > too_many_gaps) ? ngaps - too_many_gaps : 0;
			clen = sizeof(*m) + ngaps * sizeof(uint32_t);
		}
	}
	if (plen > ckp->mrem && plen <= sd->dmps)
		goto wait_for_next_packet;
	if (!(skb = sctp_alloc_chk(sk, clen, plen)))
		goto enobufs;
	{
		sctp_tcb_t *gap = sp->gaps;
		sctp_tcb_t *dup = sp->dups;
		sctp_tcb_t *tcb = SCTP_SKB_CB(skb);
		unsigned char *b_wptr = skb_put(skb, plen);
		size_t arwnd = sp->a_rwnd;
		/* For sockets, socket buffer management maintaines the sp->a_rwnd value at the
		   current available receive window. For STREAMS, sp->a_rwnd is the maximum
		   available receive window and we subtract the queued bytes in each of the receive 
		   buffers. For STREAMS we are also including the duplicate queue which protects us 
		   more from possible attacks on the duplicate queue. */
		/* TODO: double check the a_rwnd calculation for STREAMS as it does not seem to
		   take into account SWS. */
		/* fill out sack message information */
		m = ((typeof(m)) b_wptr)++;
		m->ch.type = SCTP_CTYPE_SACK;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->c_tsn = htonl(sp->r_ack);
		m->a_rwnd = htonl(arwnd);
		m->ngaps = htons(ngaps);
		m->ndups = htons(ndups);
		for (; gap && ngaps; gap = gap->next, ngaps--) {
			*((uint16_t *) b_wptr)++ = htons(gap->tsn - sp->r_ack);
			gap = gap->tail;
			*((uint16_t *) b_wptr)++ = htons(gap->tsn - sp->r_ack);
		}
		for (; dup && ndups; dup = dup->next, ndups--)
			*((uint32_t *) b_wptr)++ = htonl(dup->tsn);
		__skb_queue_purge(&sp->dupq);
		sp->ndups = 0;
		sp->dups = NULL;
#ifdef SCTP_CONFIG_ECN
		if (sp->sackf & SCTP_SACKF_ECN) {
			e = ((typeof(e)) b_wptr)++;
			e->ch.type = SCTP_CTYPE_ECNE;
			e->ch.flags = 0;
			e->ch.len = __constant_htons(sizeof(*e));
			e->l_tsn = htonl(sp->l_lsn);
			SCTP_INC_STATS(SctpOutCtrlChunks);
			printd(("INFO: Bundled ECNE chunk.\n"));
		}
#endif				/* SCTP_CONFIG_ECN */
		sp->sackf &= ~SCTP_SACKF_ANY;
		sp_del_timeout(sp, &sp->timer_sack);
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = tcb;
		ckp->dpp = &(tcb->next);
		tcb->next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		printd(("INFO: Bundled SACK chunk.\n"));
		return (0);
	}
      enobufs:
	return (0);
      wait_for_next_packet:
	return (1);
      outstate:
	swerr();
	sp->sackf &= ~SCTP_SACKF_ANY;
	return (0);
}

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
/*
 *  BUNDLE FORWARD SEQUENCE NUMBER
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_fsn(struct sock *sk,	/* association */
		struct sctp_daddr *sd,	/* destination */
		struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_forward_tsn *m;
	size_t clen = sizeof(*m);
	size_t plen;
	size_t nstrs = 0;
	size_t mstrs = (sd->dmps - sizeof(*m)) / sizeof(uint32_t);
	uint32_t l_fsn = sp->t_ack;
	if ((1 << sk->state) & ~(SCTPF_SENDING))
		goto outstate;
	if (!after(sp->l_fsn, sp->t_ack))
		goto outstate;
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
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
	clen += nstrs * sizeof(uint32_t);
	plen = PADC(clen);
	if (plen >= ckp->mrem && plen <= sd->dmps)
		goto wait_for_next_packet;
	if (!(skb = sctp_alloc_chk(sk, clen, plen)))
		goto enobufs;
	{
		sctp_tcb_t *tcb = SCTP_SKB_CB(skb);
		unsigned char *b_wptr = skb_put(skb, plen);
		/* fill out fsn message headers */
		m = ((typeof(m)) b_wptr)++;
		m->ch.type = SCTP_CTYPE_FORWARD_TSN;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->f_tsn = htonl(l_fsn);
		/* run backwards to build stream number list */
		for (skb = skb_peek_tail(&sp->rtxq); skb; skb = skb_prev(skb)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (after(cb->tsn, l_fsn))
				continue;
			if (!after(cb->tsn, sp->t_ack))
				break;
			if (cb->st && !(cb->flags & SCTPCB_FLAG_URG)
			    && (cb->st->n.more & SCTP_STRMF_DROP)) {
				*((uint16_t *) b_wptr)++ = cb->st->sid;
				*((uint16_t *) b_wptr)++ = cb->ssn;
				cb->st->n.more &= ~SCTP_STRMF_DROP;
				assure(nstrs);
				nstrs--;
			}
		}
		assure(!nstrs);
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_del_timeout(sp, &sp->timer_sack);
		sd->flags |= SCTP_DESTF_FORWDTSN;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = tcb;
		ckp->dpp = &tcb->next;
		tcb->next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		printd(("INFO: Bundled FORWARD-TSN chunk.\n"));
		return (0);
	}
      enobufs:
	return (1);
      wait_for_next_packet:
	return (1);
      outstate:
	swerr();
	sp->sackf &= ~SCTP_SACKF_FSN;
	if (!(sp->sackf & SCTP_SACKF_ANY))
		sp_del_timeout(sp, &sp->timer_sack);
	return (0);
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

#ifdef SCTP_CONFIG_ECN
/*
 *  BUNDLE CONGESTION WINDOW REDUCTION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_cwr(struct sock *sk,	/* association */
		struct sctp_daddr *sd,	/* destination */
		struct sctp_bundle_cookie *ckp /* lumped argument */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_cwr *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	if ((1 << sk->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen >= ckp->mrem && plen <= sd->dmps)
		goto wait_for_next_packet;
	if (!(skb = sctp_alloc_chk(sk, clen, plen)))
		goto enobufs;
	{
		sctp_tcb_t *tcb = SCTP_SKB_CB(skb);
		/* fill out cwr message headers */
		m = ((typeof(m)) skb_put(skb, plen));
		m->ch.type = SCTP_CTYPE_CWR;
		m->ch.flags = 0;
		m->ch.len = __constant_htons(sizeof(*m));
		m->l_tsn = htonl(sp->p_lsn);
		sp->sackf &= ~SCTP_SACKF_CWR;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = tcb;
		ckp->dpp = &tcb->next;
		tcb->next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		printd(("INFO: Bundled CWR chunk.\n"));
		return (0);
	}
      enobufs:
	return (0);
      wait_for_next_packet:
	return (1);
      outstate:
	swerr();
	sp->sackf &= ~SCTP_SACKF_CWR;
	return (0);
}
#endif				/* SCTP_CONFIG_ECN */

/*
 *  BUNDLE ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_error(struct sock *sk,	/* association */
		  struct sctp_daddr *sd,	/* destination */
		  struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	/* SCTP IG 2.27 - Bundle errors with COOKIE-ECHO or after receiving COOKIE-WAIT */
	if ((1 << sk->state) & (SCTPF_COOKIE_ECHOED | SCTPF_DISCONNECTED))
		goto outstate;
	while (ckp->mrem && (skb = skb_peek(&sp->errq))) {
		sctp_tcb_t *tcb = SCTP_SKB_CB(skb);
		size_t plen = skb->len;
		if (plen > ckp->mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		skb_unlink(skb);
		*ckp->dpp = tcb;
		ckp->dpp = &(tcb->next);
		tcb->next = NULL;
		SCTP_INC_STATS(SctpOutCtrlChunks);
		printd(("INFO: Bundled ERROR chunk.\n"));
	}
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
      outstate:
	swerr();
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
trimhead(struct sk_buff *skb, int len)
{
	/* NOTE: Trimming the head is more difficult for STREAMS as the message block can be
	   chained together.  For sockets, there is only one socket buffer to trim. */
	if (skb->len > len) {
		__skb_trim(skb, len);
	} else
		len = 0;
	assure(!len);
	return (len);
}
STATIC INLINE int
trimtail(struct sk_buff *skb, int len)
{
	/* NOTE: Trimming the tail is more difficult for STREAMS as the message block can be
	   chained together.  For sockets, there is only one socket buffer to trim. */
	if (skb->len > len) {
		__skb_pull(skb, len);
	} else
		len = 0;
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
sctp_frag_chunk(struct sk_buff_head *bq, struct sk_buff *skb, size_t mps)
{
	struct sk_buff *skd;
	assert(bq);
	assert(skb);
	rare();
	/* This should be extremely rare, now that we are fragmenting in sctp_send_data.  This
	   fragmentation only occurs if the path MTU has dropped since we buffered data for
	   transmission.  It is probably not necessary any more. */
	{
		struct sk_buff *skp;
		/* copy the transmission control block and data header */
		if (!(skp = skb_copy(skb))) {
			rare();
			return;
		}
		/* skb_copy and copyb do not copy the hidden control block */
		bcopy(SCTP_SKB_CB(skb), SCTP_SKB_CB(skp), sizeof(sctp_tcb_t));
#if STREAMS
		/* For socket buffers, when we copy the buffer we copy all of the data associated
		   with the buffer.  For message blocks, we must separately duplicate the message
		   blocks that form the data. */
		if (!(bp = dupmsg(mp->b_cont))) {
			rare();
			freeb(dp);
			return;
		}
		dp->b_cont = bp;
#endif				/* STREAMS */
	}
	{
		sctp_tcb_t *cb1;
		sctp_tcb_t *cb2;
		struct sctp_data *m1;
		struct sctp_data *m2;
		size_t dlen;
		cb1 = SCTP_SKB_CB(skb);
		cb2 = SCTP_SKB_CB(skp);
		m1 = (struct sctp_data *) skb->data;
		m2 = (struct sctp_data *) skp->data;
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
			ret = trimhead(skb, dlen);	/* trim original */
			ensure(ret, kfree_skb(skp); return;);
			ret = trimtail(skp, dlen);	/* trim fragment */
			ensure(ret, kfree_skb(skp); return;);
		}
#else
		fixme(("Should consider multiple mblks\n"));
		skb->len = dlen;
		skb_pull(skp, dlen);
#endif
	}
	/* insert the fresh copy after the existing copy in the buffer queue */
	__skb_insert(skp, skb, skb->next, skb->list);
}
#endif				/* defined(SCTP_CONFIG_DEBUG)||defined(SCTP_CONFIG_TEST)||defined(SCTP_CONFIG_SAFE) 
				 */
#endif

/*
 *  BUNDLE DATA for RETRANSMISSION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

STATIC INLINE int
sctp_bundle_data_retrans(struct sock *sk,	/* association */
			 struct sctp_daddr *sd,	/* destination */
			 struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	for (skb = skb_peek(&sp->rtxq); ckp->mrem && skb; skb = skb_next(skb)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		if (cb->flags & SCTPCB_FLAG_RETRANS) {
			size_t dlen = cb->dlen;
			size_t plen = PADC(sizeof(struct sctp_data) + dlen);
			if (plen > ckp->mrem && plen <= sd->dmps)
				goto wait_for_next_packet;
			if (dlen > ckp->swnd && cb->sacks != SCTP_FR_COUNT)
				goto congested;
			cb->flags &= ~SCTPCB_FLAG_RETRANS;
			ensure(sp->nrtxs > 0, sp->nrtxs = 1);
			sp->nrtxs--;
			sk->wmem_queued -= PADC(skb->len);
			ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
			cb->when = jiffies;
			cb->daddr = sd;
			ckp->swnd -= dlen;
			sd->in_flight += dlen;
			sp->in_flight += dlen;
			ckp->mrem = (ckp->mrem > plen) ? ckp->mrem - plen : 0;
			ckp->mlen += plen;
			*ckp->dpp = cb;
			ckp->dpp = &(cb->next);
			cb->next = NULL;
			skb_get(skb);
		}
	}
	return (0);
      congested:
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
}

/*
 *  BUNDLE NEW EXPEDITED (OUT OF ORDER) DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_data_urgent(struct sock *sk,	/* association */
			struct sctp_daddr *sd,	/* destination */
			struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	while (ckp->mrem && (skb = skb_peek(&sp->urgq))) {
		struct sctp_data *m = (typeof(m)) skb->data;
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(*m) + dlen);
		ensure(cb->st, continue);
#if 0
#if defined(SCTP_CONFIG_DEBUG)||defined(SCTP_CONFIG_TEST)||defined(SCTP_CONFIG_SAFE)
		/* this should only occur if the pmtu is falling */
		if (sp->amps <= ckp->mrem && plen > sp->amps) {
			rare();
			sctp_frag_chunk(&sp->urgq, skb, sp->amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
#endif
		if (plen > ckp->mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		if (dlen > ckp->swnd && sd->in_flight)
			goto congested;
		if ((skb == cb->st->x.head)) {
			printd(("INFO: %s: stole partial\n", __FUNCTION__));
			cb->st->x.head = NULL;	/* steal partial */
			sk->wmem_queued += PADC(skb->len);
		}
		skb_get(skb);
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
		*ckp->dpp = cb;
		ckp->dpp = &cb->next;
		cb->next = NULL;
		skb_unlink(skb);
		sk->wmem_queued -= PADC(skb->len);
		ensure(sk->wmem_queued > 0, sk->wmem_queued = 0);
		__skb_queue_tail(&sp->rtxq, skb);
		SCTP_INC_STATS(SctpOutUnorderChunks);
		printd(("INFO: Bundling DATA chunk (unordered).\n"));
	}
	return (0);
      congested:
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
}

/*
 *  BUNDLE NEW NORMAL (ORDERED) DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_data_normal(struct sock *sk,	/* association */
			struct sctp_daddr *sd,	/* destination */
			struct sctp_bundle_cookie *ckp /* lumped arguments */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	while (ckp->mrem && (skb = skb_peek(&sk->sndq))) {
		struct sctp_data *m = (typeof(m)) skb->data;
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(*m) + dlen);
		ensure(cb->st, continue);
#if 0
#if defined(SCTP_CONFIG_DEBUG)||defined(SCTP_CONFIG_TEST)||defined(SCTP_CONFIG_SAFE)
		/* this should only occur if the pmtu is falling */
		if (sp->amps <= ckp->mrem && plen > sp->amps) {
			rare();
			sctp_frag_chunk(&sp->sndq, skb, sp->amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
#endif
		if (plen > ckp->mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		if (dlen > ckp->swnd && sd->in_flight)
			goto congested;
		if ((skb == cb->st->n.head)) {
			printd(("INFO: %s: stole partial\n", __FUNCTION__));
			cb->st->n.head = NULL;	/* steal partial */
			sk->wmem_queued += PADC(skb->len);
		}
		skb_get(skb);
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
		*ckp->dpp = cb;
		ckp->dpp = &cb->next;
		cb->next = NULL;
		skb_unlink(skb);
		sk->wmem_queued -= PADC(skb->len);
		ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
		__skb_queue_tail(&sp->rtxq, skb);
		SCTP_INC_STATS(SctpOutOrderChunks);
		printd(("INFO: Bundling DATA chunk (ordered).\n"));
	}
	return (0);
      congested:
	return (0);
      wait_for_next_packet:
	seldom();
	return (1);
}

/*
 *  BUNDLE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Tack on SACK, ERROR, and DATA chunks up to the destination MTU considering
 *  congestion windows and fragmentation sizes.
 */
STATIC int
sctp_bundle_more(struct sock *sk, struct sctp_daddr *sd, struct sk_buff *skb, int nonagle)
{
	struct sctp_bundle_cookie cookie;
	static const size_t hlen = sizeof(struct sctphdr);
	int rtn = 0;
	sctp_t *sp = SCTP_PROT(sk);
	/* find transmission control block */
	sctp_tcb_t *cb = SCTP_SKB_CB(skb);
	/* initialize argument cookie */
	cookie.dpp = &(cb->next);
	cookie.mlen = skb->len;
	cookie.mrem = (cookie.mlen < sd->dmps + hlen) ? sd->dmps + hlen - cookie.mlen : 0;
	cookie.swnd = sctp_avail(sp, sd);
	cookie.pbuf = sp->p_rbuf >> 1;
	if (((sp->sackf & SCTP_SACKF_NOW) && !sk->backlog.head)
	    || (sp->nrtxs)
	    || (!skb_queue_empty(&sp->urgq))
	    || (!skb_queue_empty(&sp->errq))
	    || ((!sd->in_flight || nonagle == 1) && sk->wmem_queued)
	    /* SCTP IG 2.15.2, SWS avoidance sending RFC 1122 4.2.3.4 */
	    || (sk->wmem_queued >= cookie.mrem && cookie.swnd >= cookie.mrem)
	    || (sk->wmem_queued >= cookie.pbuf && cookie.swnd >= cookie.pbuf)
	    /* Manshall modification */
	    || !before(sp->m_tsn, sp->t_ack)
	    ) {
		uint32_t t_tsn = sp->t_tsn;
		int res, max = 0, was_in_flight = sp->in_flight;
		*cookie.dpp = NULL;
		if (cookie.mrem && (sp->sackf & SCTP_SACKF_ANY)) {
			res = sctp_bundle_sack(sk, sd, &cookie);
			max |= res;
		}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		if (cookie.mrem && (sp->sackf & SCTP_SACKF_FSN)) {
			res = sctp_bundle_fsn(sk, sd, &cookie);
			max |= res;
		}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
		if (cookie.mrem && !skb_queue_empty(&sp->errq)) {
			res = sctp_bundle_error(sk, sd, &cookie);
			max |= res;
		}
#ifdef SCTP_CONFIG_ECN
		/* really only supposed to be bundled with new data */
		if (cookie.mrem && (sp->sackf & SCTP_SACKF_CWR) && sk->wmem_queued) {
			res = sctp_bundle_cwr(sk, sd, &cookie);
			max |= res;
		}
#endif				/* SCTP_CONFIG_ECN */
		if (cookie.swnd) {	/* subject to flow control */
			if (cookie.mrem && !skb_queue_empty(&sp->urgq)) {
				res = sctp_bundle_data_urgent(sk, sd, &cookie);
				max |= res;
				rtn += res;
			}
			if (cookie.mrem && sp->nrtxs) {
				res = sctp_bundle_data_retrans(sk, sd, &cookie);
				max |= res;
			}
			if (cookie.mrem && !skb_queue_empty(&sk->sndq)) {
				res = sctp_bundle_data_normal(sk, sd, &cookie);
				max |= res;
				rtn += res;
			}
			cb->dlen = cookie.mlen;
			if ((sd->in_flight || (sd->flags & SCTP_DESTF_FORWDTSN))
			    && !sctp_timeout_pending(&sd->timer_retrans)) {
				sd_set_timeout(sd, &sd->timer_retrans, sd->rto);
			}
			if (!max && sk->wmem_queued > 0 && after(sp->t_tsn, t_tsn))
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
STATIC struct sk_buff *
sctp_alloc_chk(struct sock *sk, size_t clen, size_t dlen)
{
	struct sk_buff *skb;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);
	assert(sk);
	if ((skb = alloc_skb(plen, GFP_ATOMIC))) {
		/* initialized control block */
		cb = SCTP_SKB_CB(skb);
		bzero(cb, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = dlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		/* pre-zero padding */
		bzero(skb->tail + clen, plen - clen);
		cb->next = NULL;
	}
	return (skb);
}

#if STREAMS
/*
 *  ALLOC CTL
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocate a mblk for a chunk without hard-header, IP header or SCTP header headroom.  This is
 *  used for allocating mblks which are used to track a received chunk.  This is really only to
 *  create a transmission control block for STREAMS which does not have a control block in the
 *  message block structure.  Sockets has a transmission control block in each socket buffer.
 */
STATIC struct sk_buff *
sctp_alloc_ctl(struct sock *sk, size_t clen, size_t dlen)
{
	struct sk_buff *skb;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);
	assert(sk);
	if ((skb = alloc_skb(plen, GFP_ATOMIC))) {
		/* initialized control block */
		cb = SCTP_SKB_CB(skb);
		bzero(cb, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = dlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		/* pre-zero padding */
		bzero(skb->tail + clen, plen - clen);
		cb->next = NULL;
	}
	return (mp);
}
#endif				/* STREAMS */

/*
 *  ALLOC MSG
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocates a new message block with a control block, an SCTP message header, (for sockets, room
 *  for an IP header and hard header) and, optionally, the first chunk of a message.  This is used
 *  for allocating messages that contain a single control chunk and for allocating the header for a
 *  message that will have additional chunks bundled.
 */
STATIC struct sk_buff *
sctp_alloc_msg(struct sock *sk, size_t clen)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctphdr *sh;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);
	size_t mlen = sizeof(*sh) + plen;
	size_t hlen = MAX_HEADER + sizeof(struct iphdr);
	assert(sk);
	if ((skb = alloc_skb(hlen + mlen, GFP_ATOMIC))) {
		/* initialize control block */
		cb = SCTP_SKB_CB(skb);
		bzero(cb, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = mlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		/* pull the hard and IP header headroom */
		skb_reserve(skb, hlen);
		sh = ((struct sctphdr *) skb_put(skb, sizeof(*sh)));
		sh->srce = sk->sport;
		sh->dest = sk->dport;
		sh->v_tag = sp->p_tag;
		sh->check = 0;
		/* pre-zero padding */
		bzero(skb->tail + clen, plen - clen);
		cb->next = NULL;
	}
	return (skb);
}

/*
 *  ALLOC REPLY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for allocating a message block with a hidden transmission control block for a message
 *  (with IP header room for sockets) and an STCP header.  This is for out of the blue replies and
 *  all that is required is the SCTP header of the message to which this is a reply.
 */
STATIC struct sk_buff *
sctp_alloc_reply(struct sctphdr *rh, size_t clen)
{
	struct sk_buff *skb;
	struct sctphdr *sh;
	sctp_tcb_t *cb;
	size_t plen = PADC(clen);
	size_t mlen = sizeof(*sh) + plen;
	size_t hlen = MAX_HEADER + sizeof(struct iphdr);
	ensure(rh, return NULL);
	if ((skb = alloc_skb(hlen + mlen, GFP_ATOMIC))) {
		/* initialize control block */
		cb = SCTP_SKB_CB(skb);
		bzero(cb, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = mlen;
		/* Sockets control blocks are part of the sk_buff structure and do not need to be
		   hidden. */
		/* reserve IP and hard header room */
		skb_reserve(skb, hlen);
		sh = ((struct sctphdr *) skb_put(skb, sizeof(*sh)));
		sh->srce = rh->dest;
		sh->dest = rh->srce;
		sh->v_tag = rh->v_tag;
		sh->check = 0;
		/* pre-zero padding */
		bzero(skb->tail + clen, plen - clen);
		cb->next = NULL;
	}
	return (skb);
}

/*
 *  FREE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for freeing a chain list of mblks or sk_buffs that represent a packet built out of
 *  separate chunks.  Chunks in mblks are chained together with the mp->b_next pointer, chunks in
 *  sk_buffs are chained together with the cb->next pointer.
 */
STATIC INLINE void
freechunks(struct sk_buff *skb)
{
	if (skb) {
		sctp_tcb_t *cb, *cb_next = SCTP_SKB_CB(skb);
		while ((cb = cb_next)) {
			cb_next = cb->next;
			kfree_skb(SCTP_CB_SKB(cb));
		}
	}
}

STATIC INLINE void
sctp_error_report(struct sock *sk, int err)
{
	/* FIXME: For STREAMS, on an error report we need to set the poll error bit and send up an
	   error using a M_ERROR message.  Or do we...? */
	if (sock_locked(sk) || !sk->protinfo.af_inet.recverr)
		sk->err_soft = err;
	else {
		sk->err = err;
		if (!sk->dead)
			sk->error_report(sk);
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
STATIC INLINE int sctp_abort(struct sock *sk, ulong origin, long reason);
/*
 *  This version for use by bottom halves that have the socket or stream locked so that it will not
 *  attempt to lock the socket or stream on disconnect.
 */
STATIC struct sctp_daddr *
sctp_route_normal(struct sock *sk)
{
	int err;
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	assert(sk);
	if ((err = sctp_update_routes(sk, 1))) {
		rare();
		/* we have no viable route */
		if ((1 << sk->state) & (SCTPF_OPENING)) {
			/* Only abort the association if we are opening and have no viable route.
			   If we have an established or closing association, wait for timers to
			   fire and retransmissions to peg before aborting.  Then associations will
			   not abort simply due to temporary routing changes. */
			if (sock_locked(sk))
				sk->err_soft = err;
			else
				sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
			return (NULL);
		}
		/* report the error */
		sctp_error_report(sk, err);
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
sctp_route_response(struct sock *sk)
{
	struct sctp_daddr *sd;
	assert(sk);
	sd = SCTP_PROT(sk)->caddr;
	if (!sd || !sd->dst_cache || sd->dst_cache->obsolete || sd->retransmits || sd->dups)
		sd = sctp_route_normal(sk);
	normal(sd);
	return (sd);
}

/*
 *  WAKEUP (Send SACK, ERROR, DATA)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  TODO: When the user has specified a destination address to t_sndopt() or sctp_sendmsg() we want
 *  to suppress nagle and do a destination-specific bundling instead of our normal route selection.
 *  We could do this by peeking the urgq and the sndq to check for user-specified destinations
 *  before selecting the destination and calling sctp_bundle_more.  This looks like it will have a
 *  performance impact so we will leave it for now.
 */
#ifdef SCTP_CONFIG_ADD_IP
STATIC void sctp_send_asconf(struct sock *sk);
#endif				/* SCTP_CONFIG_ADD_IP */
STATIC void
___sctp_transmit_wakeup(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	int i, n, reroute = 0;
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	int loop_max = 1000;
	printd(("INFO: Performing transmitter wakeup\n"));
	ensure(sk, return);
	if ((1 << sk->state) & ~(SCTPF_SENDING | SCTPF_RECEIVING))
		goto skip;
#ifdef SCTP_CONFIG_ADD_IP
	if (sk->state == SCTP_ESTABLISHED && sp->sackf & SCTP_SACKF_ASC)
		sctp_send_asconf(sk);
#endif				/* SCTP_CONFIG_ADD_IP */
	/* SCTP IG 2.8, 2.14 */
//      for (i = 0, n = 0; i < loop_max && n <= sp->max_burst; i++) {
	for (i = 0, n = 0; i < loop_max; i++) {
		/* placed in order of probability */
		if (!sk->wmem_queued && (!(sp->sackf & SCTP_SACKF_NOW) || sk->backlog.head)
		    && !sp->nrtxs && skb_queue_empty(&sp->urgq)
		    && skb_queue_empty(&sp->errq))
			goto done;
		if (!(sd = sctp_route_normal(sk)))
			goto noroute;
		if (!(skb = sctp_alloc_msg(sk, 0)))
			goto enobufs;
		n += sctp_bundle_more(sk, sd, skb, sp->nonagle);
		if (SCTP_SKB_CB(skb)->next) {
			reroute = 0;
			sctp_send_msg(sk, sd, skb);
			freechunks(skb);
			continue;
		}
		if (!reroute) {
			reroute = 1;
			kfree_skb(skb);
			continue;
		}
		goto discard;
	}
	assure(i < loop_max);
	return;
      discard:
	kfree_skb(skb);
	goto done;
      noroute:
	ptrace(("ERROR: no route to peer\n"));
	goto done;
      enobufs:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto done;
      done:
	assure(i > 0 || !(sp->sackf & SCTP_SACKF_NOW) || sk->backlog.head);
      skip:
	return;
}

STATIC void
sctp_transmit_wakeup(struct sock *sk)
{
	/* The STREAMS version defers transmitter wakeup by scheduling the write queue of the
	   stream.  This eventually calls ___sctp_transmit_wakeup. On the sockets version, we have
	   to call ___sctp_transmit_wakeup directly. */
#if SOCKETS
	return ___sctp_transmit_wakeup(sk);
#else				/* SOCKETS */
	if (sp->wq) {
		qenable(sp->wq);
		return;
	}
	swerr();
#endif				/* SOCKETS */
}

/*
 *  =========================================================================
 *
 *  SCTP State Machine Indications
 *
 *  =========================================================================
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  CONNECTION Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_conn_ind(struct sock *sk, struct sk_buff *skb)
{
	/* connection indication */
	printd(("%p: X_CONN_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->conn_ind)
			return sp->ops->conn_ind(sk, skb);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CONNECTION Confirmation
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_conn_con(struct sock *sk)
{
	printd(("%p: X_CONN_CON -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		int err = 0;
		int oldstate = sctp_change_state(sk, SCTP_ESTABLISHED);
		if (sp->ops->conn_con)
			err = sp->ops->conn_con(sk);
		if (err < 0)
			sctp_change_state(sk, oldstate);
		return (err);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  (EX)DATA Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_data_ind(struct sock *sk, int len)
{
	printd(("%p: X_DATA_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->data_ind)
			return sp->ops->data_ind(sk, len, 1);
	}
	swerr();
	return (-EFAULT);
}

STATIC INLINE int
sctp_exdata_ind(struct sock *sk, int len)
{
	printd(("%p: X_EXDATA_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->data_ind)
			return sp->ops->data_ind(sk, len, 0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DATACK Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_datack_ind(struct sock *sk, size_t len)
{
	printd(("%p: X_DATACK_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->datack_ind)
			return sp->ops->datack_ind(sk, len);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DISCONNECT Indication
 *
 *  -------------------------------------------------------------------------
 *  When passed an active connection indication, the interface specific disconnect indication
 *  function must unlink and free the connection indication or return an error.  Other than that,
 *  the interface specific disconnect indication function merely informs the interface of the
 *  disconnect and takes no other action.
 */
STATIC INLINE int
sctp_discon_ind(struct sock *sk, ulong origin, long reason, struct sk_buff *cp)
{
	sctp_t *sp = SCTP_PROT(sk);
	printd(("%p: X_DISCON_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		if (sp->ops->discon_ind)
			return sp->ops->discon_ind(sk, origin, reason, cp);
	}
	rare();
	if (cp) {
		skb_unlink(cp);
		kfree_skb(cp);
		sk->ack_backlog--;
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ORDERLY RELEASE Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_ordrel_ind(struct sock *sk)
{
	printd(("%p: X_ORDREL_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->ordrel_ind)
			return sp->ops->ordrel_ind(sk);
		sctp_abort(sk, SCTP_ORIG_USER, SCTP_CAUSE_USER_INITIATED);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  RESET Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_reset_ind(struct sock *sk, ulong origin, long reason, struct sk_buff *cp)
{
	printd(("%p: X_RESET_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->reset_ind)
			return sp->ops->reset_ind(sk, origin, reason, cp);
		sctp_abort(sk, origin, reason);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  RESET Confirmation
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_reset_con(struct sock *sk)
{
	printd(("%p: X_RESET_CON -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->reset_con)
			return sp->ops->reset_con(sk);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  RETRIEVAL Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_retr_ind(struct sock *sk, struct sk_buff *dp)
{
	printd(("%p: X_RETRV_IND -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->retr_ind)
			return sp->ops->retr_ind(sk, dp);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  RETRIEVAL Confirmation
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_retr_con(struct sock *sk)
{
	printd(("%p: X_RETRV_CON -> \n", sk));
	assert(sk);
	if (!sk->dead || sk->socket) {
		sctp_t *sp = SCTP_PROT(sk);
		if (sp->ops->retr_con)
			return sp->ops->retr_con(sk);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  SCTP State Machine TIMEOUTS
 *
 *  =========================================================================
 */
#ifdef SCTP_CONFIG_ECN
STATIC INLINE void sctp_send_cwr(struct sock *sk);
#endif				/* SCTP_CONFIG_ECN */
STATIC void sctp_send_heartbeat(struct sock *sk, struct sctp_daddr *sd);
/*
 *  ASSOCIATION TIMEOUT FUNCTION
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_assoc_timedout(struct sock *sk,	/* association */
		    struct sctp_daddr *sd,	/* destination */
		    size_t rmax,	/* retry maximum */
		    int stroke /* whether to stroke counts */ )
{
	sctp_t *sp = SCTP_PROT(sk);
	ensure(sk, return (-EFAULT));
	ensure(sd, return (-EFAULT));
	/* RFC 2960 6.3.3 E1 and 7.2.3, E2, E3 and 8.3 */
	sd->ssthresh = ((sd->cwnd >> 1) > (sd->mtu << 1)) ? sd->cwnd >> 1 : sd->mtu << 1;
	sd->cwnd = sd->mtu;
	/* SCTP IG Section 2.9 */
	sd->partial_ack = 0;
	sd->rto = (sd->rto) ? sd->rto << 1 : 1;
	sd->rto = (sd->rto_min > sd->rto) ? sd->rto_min : sd->rto;
	sd->rto = (sd->rto_max < sd->rto) ? sd->rto_max : sd->rto;
	ptrace(("Timeout calculation:\n  sd->retransmits = %d\n  sp->retransmits = %d\n  ssthresh = %d\n  cwnd = %d\n  rto = %lu\n", sd->retransmits, sp->retransmits, sd->ssthresh, sd->cwnd, sd->rto));
#ifdef SCTP_CONFIG_ECN
	sctp_send_cwr(sk);
#endif				/* SCTP_CONFIG_ECN */
	/* SCTP IG 2.15: don't stroke counts on zero window probes, or ECN */
	if (!stroke && sk->state != SCTP_COOKIE_WAIT)
		return (0);
	/* See RFC 2960 Section 8.3 */
	if (++sd->retransmits > sd->max_retrans) {
		if (sd->retransmits > sd->max_retrans + 1) {
			if ((1 << sk->state) & (SCTPF_CONNECTED))
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
			    && (1 << sk->state) & (SCTPF_CONNECTED | SCTPF_CLOSING)) {
				sd_del_timeout(sd, &sd->timer_idle);
				sctp_send_heartbeat(sk, sd);
			}
#endif
		}
	}
	/* See RFC 2960 Section 8.2 */
	if (rmax && sp->retransmits++ >= rmax) {
		int err = sk->err_soft ? -sk->err_soft : -ETIMEDOUT;
		seldom();
		sctp_discon_ind(sk, SCTP_ORIG_PROVIDER, err, NULL);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
		return (err);
	}
	return (0);
}

/*
 *  INIT TIMEOUT (T1-init)
 *  -------------------------------------------------------------------------
 *  The init timer has expired indicating that we have not received an INIT ACK within timer
 *  T1-init.  This means that we should attempt to retransmit the INIT until we have attempted
 *  Max.Init.Retrans times.
 */
STATIC void
sctp_init_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sp->timer_init))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if (sk->state != SCTP_COOKIE_WAIT)
		goto outstate;
	if (sctp_assoc_timedout(sk, sd, sp->max_inits ? sp->max_inits : 1, sp->p_rwnd))
		goto timedout;
	sd = sp->taddr;		/* might have new primary */
	ensure(sd, goto done);
	sp_set_timeout(sp, &sp->timer_init, sd->rto);
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	sock_put(sk);
      unlock:
	bh_unlock_sock(sk);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sp_set_timeout(sp, &sp->timer_init, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  COOKIE TIMEOUT
 *  -------------------------------------------------------------------------
 *  The cookie timer has expired indicating that we have not yet received a COOKIE ACK within timer
 *  T1-cookie.  This means that we should attempt to retransmit the COOKIE ECHO until we have
 *  attempted Path.Max.Retrans times.
 */
STATIC void
sctp_cookie_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sp->timer_cookie))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if (sk->state != SCTP_COOKIE_ECHOED)
		goto outstate;
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	/* See RFC 2960 6.3.3 E3 */
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		seldom();
		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
		    && !(cb->flags & (SCTPCB_FLAG_RETRANS | SCTPCB_FLAG_DROPPED))) {
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			sk->wmem_queued += PADC(skb->len);
			cb->sacks = SCTP_FR_COUNT;	/* not eligble for FR now */
		}
	}
	sd = sp->taddr;		/* might have new primary */
	ensure(sd, goto done);
	sp_set_timeout(sp, &sp->timer_cookie, sd->rto);
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	sock_put(sk);
      unlock:
	bh_unlock_sock(sk);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sp_set_timeout(sp, &sp->timer_cookie, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  RETRANS TIMEOUT (T3-rtx)
 *  -------------------------------------------------------------------------
 *  This means that we have not received an ack for a DATA chunk within timer T3-rtx.  This means
 *  that we should mark all outstanding DATA chunks for retransmission and start a retransmission
 *  cycle.
 */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
STATIC INLINE void sctp_send_forward_tsn(struct sock *sk);
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
STATIC void
sctp_retrans_timeout(unsigned long data)
{
	struct sctp_daddr *sd = (struct sctp_daddr *) data;
	sctp_t *sp;
	struct sock *sk;
	struct sk_buff *skb;
	int retransmits = 0;
	ensure(sd, return);
	sp = sd->sp;
	ensure(sp, return);
	sk = SCTP_SOCK(sp);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sd->timer_retrans))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_CONNECTED)))
		goto outstate;
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	/* See RFC 2960 6.3.3 E3 */
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		size_t dlen = cb->dlen;
		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
		    && !(cb->
			 flags & (SCTPCB_FLAG_RETRANS | SCTPCB_FLAG_SACKED | SCTPCB_FLAG_DROPPED)))
		{
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			sk->wmem_queued += PADC(skb->len);
			cb->sacks = SCTP_FR_COUNT;	/* not eligble for FR now */
			normal(sd->in_flight >= dlen);
			normal(sp->in_flight >= dlen);
			/* credit dest and assoc */
			sd->in_flight = (sd->in_flight > dlen) ? sd->in_flight - dlen : 0;
			sp->in_flight = (sp->in_flight > dlen) ? sp->in_flight - dlen : 0;
			retransmits++;
		}
	}
	normal(retransmits);
#ifdef SCTP_CONFIG_ADD_IP
	if (sd->flags & SCTP_DESTF_UNUSABLE)
		sctp_del_daddr(sd);
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sk);
		if (!sctp_timeout_pending(&sp->timer_sack))
			sp_set_timeout(sp, &sp->timer_sack, sp->max_sack);
		sp->sackf |= SCTP_SACKF_NOD;	/* don't delay retransmitted FWD TSN */
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_del_timeout(sp, &sp->timer_sack);
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	sctp_transmit_wakeup(sk);
      done:
	sctp_dput((struct sctp_daddr *) data);
      unlock:
	bh_unlock_sock(sk);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sd_set_timeout(sd, &sd->timer_retrans, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  SACK TIMEOUT
 *  -------------------------------------------------------------------------
 *  This timer is the 200ms timer which ensures that a SACK is sent within 200ms of the receipt of
 *  an unacknoweldged DATA chunk.  When an unacknowledged DATA chunks i receive and the timer is not
 *  running, the timer is set.  Whenever a DATA chunks(s) are acknowledged, the timer is stopped.
 */
STATIC void
sctp_sack_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	sctp_t *sp = SCTP_PROT(sk);
	ensure(sk, return);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sp->timer_sack))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if ((1 << sk->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	sp->sackf |= SCTP_SACKF_TIM;	/* RFC 2960 6.2 */
	sctp_transmit_wakeup(sk);
      done:
	sock_put(sk);
      unlock:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sp_set_timeout(sp, &sp->timer_sack, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  IDLE TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that a destination has been idle for longer than the hb.itvl or the interval for
 *  which we must send heartbeats.  This timer is reset every time we do an RTT calculation for a
 *  destination.  It is stopped while sending heartbeats and started again whenever an RTT
 *  calculation is done.  While this timer is stopped, heartbeats will be sent until they are
 *  acknowledged.
 */
STATIC void
sctp_idle_timeout(unsigned long data)
{
	struct sctp_daddr *sd = (struct sctp_daddr *) data;
	sctp_t *sp;
	struct sock *sk;
	ensure(sd, return);
	sp = sd->sp;
	ensure(sp, return);
	sk = SCTP_SOCK(sp);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sd->timer_idle))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	/* SCTP IG 2.10 but we continue through closing states */
	if ((1 << sk->state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	sctp_send_heartbeat(sk, sd);
      done:
	sctp_dput((struct sctp_daddr *) data);
      unlock:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sd_set_timeout(sd, &sd->timer_idle, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  HEARTBEAT TIMEOUT
 *  -------------------------------------------------------------------------
 *  If we get a heartbeat timeout we adjust RTO the same as we do for retransmit (and the congestion
 *  window) and resend the heartbeat.  Once we have sent Path.Max.Retrans heartbeats unsuccessfully,
 *  we mark the destination as unusable, but continue heartbeating until they get acknowledged.
 *  (Well!  That's not really true, is it?)
 */
STATIC void sctp_reset_idle(struct sctp_daddr *sd);
STATIC void
sctp_heartbeat_timeout(unsigned long data)
{
	struct sctp_daddr *sd = (typeof(sd)) data;
	struct sock *sk;
	sctp_t *sp;
	ensure(sd, return);
	sp = sd->sp;
	ensure(sp, return);
	sk = SCTP_SOCK(sp);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sd->timer_heartbeat))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	/* SCTP IG 2.10 but we continue through closing states */
	if ((1 << sk->state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	/* SCTP IG 2.10 */
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, 1))
		goto timedout;
	sctp_reset_idle(sd);
#if 0
	sctp_send_heartbeat(sk, sd);
#endif
      done:
	sctp_dput((struct sctp_daddr *) data);
      unlock:
	bh_unlock_sock(sk);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sd_set_timeout(sd, &sd->timer_heartbeat, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  SHUTDOWN TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timedout on sending a SHUTDOWN or a SHUTDOWN ACK message.  We simply
 *  resend the message.
 */
STATIC void
sctp_shutdown_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sp->timer_shutdown))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_CLOSING)))
		goto outstate;
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	if (!(sd = sctp_route_normal(sk)))
		goto done;
	sp_set_timeout(sp, &sp->timer_shutdown, sd->rto);
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	sock_put(sk);
      unlock:
	bh_unlock_sock(sk);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sp_set_timeout(sp, &sp->timer_shutdown, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  GUARD TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timed out in the over SHUTDOWN process and need to ABORT the association
 *  per SCTP IG 2.12.
 */
STATIC void sctp_send_abort(struct sock *sk);
STATIC void
sctp_guard_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	sctp_t *sp = SCTP_PROT(sk);
	ensure(sp, return);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sp->timer_guard))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if ((1 << sk->state) & ~(SCTPF_CLOSING))
		goto outstate;
	sctp_send_abort(sk);
	{
		int err = sk->err_soft ? -sk->err_soft : -ETIMEDOUT;
		sctp_discon_ind(sk, SCTP_ORIG_PROVIDER, err, NULL);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
	}
      done:
	sock_put(sk);
      unlock:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sp_set_timeout(sp, &sp->timer_guard, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

#ifdef SCTP_CONFIG_ADD_IP
/*
 *  ASCONF TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timedout on sending a ASCONF message.  We simply resend the message.
 */
STATIC void
sctp_asconf_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sp->timer_asconf))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if ((1 << sk->state) & ~(SCTPF_ESTABLISHED))
		goto outstate;
	/* ADD-IP (B1), (B2) and (B3) */
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	if (!(sd = sctp_route_normal(sk)))
		goto noroute;
	/* ADD-IP (B5) */
	sp_set_timeout(sp, &sp->timer_asconf, sd->rto);
	/* ADD-IP (B4) */
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	sock_put(sk);
      unlock:
	bh_unlock_sock(sk);
	return;
      noroute:
	ptrace(("WARNING: no route\n"));
	goto done;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sp_set_timeout(sp, &sp->timer_asconf, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}
#endif				/* SCTP_CONFIG_ADD_IP */

#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
/*
 *  LIFETIME TIMEOUT
 *  -------------------------------------------------------------------------
 */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
STATIC INLINE void sctp_send_forward_tsn(struct sock *sk);
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
STATIC void
sctp_life_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb, *skb_next;
	unsigned long expires = -1UL;
	ensure(sk, return);
	bh_lock_sock(sk);
	if (sctp_timeout_cancelled(&sp->timer_life))
		goto cancelled;
	if (sock_locked(sk))
		goto locked;
	if ((1 << sk->state) & ~(SCTPF_SENDING))
		goto outstate;
	skb_next = skb_peek(&sk->sndq);
	while ((skb = skb_next)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		skb_next = skb_next(skb);
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
		skb_unlink(skb);
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG)
			__skb_queue_tail(&sp->ackq, skb);
		else
			kfree_skb(skb);
	}
	skb_next = skb_peek(&sp->urgq);
	while ((skb = skb_next)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		skb_next = skb_next(skb);
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
		skb_unlink(skb);
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG)
			__skb_queue_tail(&sp->ackq, skb);
		else
			kfree_skb(skb);
	}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	/* need to walk through retransmit queue as well */
	if (!(sp->p_caps & SCTP_CAPS_PR))
		goto nocaps;
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
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
			sk->wmem_queued -= PADC(skb->len);
			ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
		} else {
			struct sctp_daddr *sd = cb->daddr;
			size_t dlen = cb->dlen;
			if (sd) {
				/* credit destination (now) */
				sd->in_flight = (sd->in_flight > dlen) ? sd->in_flight - dlen : 0;
				if (!sd->in_flight) {
					sd_del_timeout(sd, &sd->timer_retrans);
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
	if (expires != -1UL)
		sp_set_timeout(sp, &sp->timer_life, expires - jiffies);
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sk);
		if (!sctp_timeout_pending(&sp->timer_sack))
			sp_set_timeout(sp, &sp->timer_sack, sp->max_sack);
		sctp_transmit_wakeup(sk);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_del_timeout(sp, &sp->timer_sack);
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
      done:
	sock_put(sk);
      unlock:
	bh_unlock_sock(sk);
	return;
      nocaps:
	ptrace(("WARNING: no capabilities\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sp_set_timeout(sp, &sp->timer_life, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto unlock;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */

/*
 *  SEND DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This function slaps a chunk header onto the M_DATA blocks which form the data and places it onto
 *  the stream's write queue.  The message blocks input to this function already have a chunk
 *  control block prepended.
 *
 *  Under sockets, this function has been replaced by the sockets sendmsg function.
 */

/*
 *  SEND SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE void
sctp_send_sack(struct sock *sk)
{
	SCTP_PROT(sk)->sackf |= SCTP_SACKF_NOD;
	printd(("Marking SACK from socket %p\n", sk));
}

/*
 *  SEND ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We just queue the error, we don't send it out...  It gets bundled with other things.  This also
 *  provides for SCTP IG 2.27.
 */
STATIC int
sctp_send_error(struct sock *sk, uint ecode, caddr_t eptr, size_t elen)
{
	struct sk_buff *skb;
	struct sctp_error *m;
	struct sctpehdr *eh;
	size_t clen = sizeof(*m) + sizeof(*eh) + elen;
	size_t plen = PADC(clen);
	if (!(skb = sctp_alloc_chk(sk, clen, plen)))
		goto enobufs;
	m = (typeof(m)) skb->data;
	m->ch.type = SCTP_CTYPE_ERROR;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) (m + 1);
	eh->code = htons(ecode);
	eh->len = htons(sizeof(*eh) + elen);
	bcopy(eptr, (eh + 1), elen);
	skb_put(skb, plen);
	__skb_queue_tail(&SCTP_PROT(sk)->errq, skb);
	return (0);
      enobufs:
	ptrace(("ERROR: couldn't allocate buffer\n"));
	return (-ENOBUFS);
}

/*
 *  SEND INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the INIT and get timers started, we must return an error to the user
 *  interface calling this function.
 */
STATIC int
sctp_send_init(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	struct sk_buff *skb;
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
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	    + ((sp->l_caps & SCTP_CAPS_ALI) ? PADC(sizeof(*ai)) : 0)
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    + ((sp->l_caps & SCTP_CAPS_PR) ? PADC(sizeof(*pr)) : 0)
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	    ;
	if (!(sd = sp->daddr))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_INIT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = sp->v_tag;
	m->a_rwnd = htonl(sk->rcvbuf);
	m->n_istr = htons(sp->max_istr);
	m->n_ostr = htons(sp->req_ostr);
	m->i_tsn = htonl(sp->v_tag);
	for (ss = sp->saddr; ss && sanum; ss = ss->next, sanum--) {
		ap = (typeof(ap)) skb_put(skb, PADC(sizeof(*ap)));
		ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
		ap->ph.len = htons(sizeof(*ap));
		ap->addr = ss->saddr;
	}
	unusual(ss);
	unusual(sanum);
	if (sp->ck_inc) {
		cp = (typeof(cp)) skb_put(skb, PADC(sizeof(*cp)));
		cp->ph.type = SCTP_PTYPE_COOKIE_PSRV;
		cp->ph.len = __constant_htons(sizeof(*cp));
		cp->ck_inc = htonl(sp->ck_inc);
	}
	at = (typeof(at)) skb_put(skb, PADC(sizeof(*at) + sizeof(at->type[0])));
	at->ph.type = SCTP_PTYPE_ADDR_TYPE;
	at->ph.len = htons(sizeof(*at) + sizeof(at->type[0]));
	at->type[0] = SCTP_PTYPE_IPV4_ADDR;
#ifdef SCTP_CONFIG_ECN
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
		ec = (typeof(ec)) skb_put(skb, PADC(sizeof(*ec)));
		ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
		ec->ph.len = __constant_htons(sizeof(*ec));
	}
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) skb_put(skb, PADC(sizeof(*ai)));
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htons(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) skb_put(skb, PADC(sizeof(*pr)));
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sp_mod_timeout(sp, &sp->timer_init, sd->rto);
	abnormal(sp->retry);
	sctp_change_state(sk, SCTP_COOKIE_WAIT);
	printd(("Sending INIT from socket %p\n", sk));
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	ptrace(("ERROR: no buffers\n"));
	return (-ENOBUFS);
      noroute:
	rare();
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
sctp_send_init_ack(struct sock *sk, uint32_t saddr, uint32_t daddr, struct sctphdr *sh,
		   struct sctp_cookie *ck, struct sk_buff *unrec)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
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
	size_t ulen = (ulen = unrec->len) ? PADC(sizeof(*up) + ulen) : 0;
	size_t clen = sizeof(*m) + dlen + klen + ulen
#ifdef SCTP_CONFIG_ECN
	    + PADC(sizeof(*ec))
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	    + ((sp->l_caps & SCTP_CAPS_ALI) ? PADC(sizeof(*ai)) : 0)
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    + ((sp->l_caps & SCTP_CAPS_PR) ? PADC(sizeof(*pr)) : 0)
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	    ;
	int arem, alen;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	rh = (typeof(rh)) skb->data;
	rh->v_tag = im->i_tag;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_INIT_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = ck->v_tag;
	m->a_rwnd = htonl(sk->rcvbuf);
	m->n_istr = htons(ck->n_istr);
	m->n_ostr = htons(ck->n_ostr);
	m->i_tsn = htonl(ck->v_tag);
	for (ss = sp->saddr; ss; ss = ss->next) {
		ap = (typeof(ap)) skb_put(skb, PADC(sizeof(*ap)));
		ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
		ap->ph.len = __constant_htons(sizeof(*ap));
		ap->addr = ss->saddr;
	}
	ph = (typeof(ph)) skb_put(skb, sizeof(*ph));
	ph->type = SCTP_PTYPE_STATE_COOKIE;
	ph->len = htons(klen);
	cp = (typeof(cp)) skb_put(skb, sizeof(*cp));
	bcopy(ck, cp, sizeof(*cp));
#if 0
	/* copy in IP reply options */
	if (ck->opt_len) {
		assure(opt);
		bcopy(opt, skb->b_wptr, optlength(opt));
		kfree_s(opt, optlength(opt));
		sp->opt = (struct ip_options *) skb->b_wptr;
		skb->b_wptr += ck->opt_len;
	}
#endif
	for (ap = (typeof(ap)) (init + sizeof(struct sctp_init)), arem =
	     PADC(htons(((struct sctpchdr *) init)->len)) - sizeof(struct sctp_init);
	     anum && arem >= sizeof(struct sctpphdr);
	     arem -= PADC(alen), ap = (typeof(ap)) (((uint8_t *) ap) + PADC(alen))) {
		if ((alen = ntohs(ap->ph.len)) > arem) {
			assure(alen <= arem);
			kfree_skb(skb);
			rare();
			return;
		}
		if (ap->ph.type == SCTP_PTYPE_IPV4_ADDR) {
			/* skip primary */
			if (ap->addr != ck->daddr) {
				*((uint32_t *) skb_put(skb, sizeof(uint32_t))) = ap->addr;
				anum--;
			}
		}
	}
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (ss->saddr != ck->saddr) {
			*((uint32_t *) skb_put(skb, sizeof(uint32_t))) = ss->saddr;
			snum--;
		}
	}
	assure(!anum);
	assure(!snum);
	sctp_sign_cookie(sp, cp);
	skb_put(skb, HMAC_SIZE);
#ifdef SCTP_CONFIG_ECN
	ec = (typeof(ec)) skb_put(skb, PADC(sizeof(*ec)));
	ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
	ec->ph.len = __constant_htons(sizeof(*ec));
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) skb_put(skb, PADC(sizeof(*ai)));
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htons(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) skb_put(skb, PADC(sizeof(*pr)));
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	/* SCTP IG 2.27 add unrecognized parameters parameter */
	if (ulen) {
		up = (typeof(up)) skb_put(skb, PADC(ulen));
		up->ph.type = SCTP_PTYPE_UNREC_PARMS;
		up->ph.len = __constant_htons(ulen);
		bcopy(unrec->data, (up + 1), unrec->len);
	}
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending INIT-ACK from socket %p\n", sk));
	sctp_xmit_msg(saddr, daddr, skb, sk);
	return;
      enobufs:
	rare();
	return;
}

/*
 *  SEND COOKIE ECHO
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the COOKIE ECHO and get timers started, we must return an error to the user
 *  interface calling this function.
 */
STATIC int
sctp_send_cookie_echo(struct sock *sk, caddr_t kptr, size_t klen)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_cookie_echo *m;
	size_t clen = sizeof(*m) + klen;
	assert(sk);
	if (!(sd = sctp_route_normal(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_COOKIE_ECHO;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	bcopy(kptr, skb_put(skb, PADC(klen)), klen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctp_change_state(sk, SCTP_COOKIE_ECHOED);
	sp_mod_timeout(sp, &sp->timer_cookie, sd->rto);
	printd(("Sending COOKIE-ECHO from socket %p\n", sk));
	sctp_bundle_more(sk, sd, skb, 1);	/* don't nagle */
	sctp_send_msg(sk, sd, skb);
	abnormal(sp->retry);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      noroute:
	rare();
	return (-EFAULT);
}

/*
 *  SEND COOKIE ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  SACK and DATA can be bundled with the COOKIE ACK.
 */
STATIC int sctp_return_more(struct sk_buff *skb);
STATIC int sctp_recv_msg(struct sock *sk, struct sk_buff *skb);
STATIC void
sctp_send_cookie_ack(struct sock *sk, struct sk_buff *cp)
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_cookie_ack *m;
	size_t clen = sizeof(*m);
	assert(sk);
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	sctp_change_state(sk, SCTP_ESTABLISHED);
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_COOKIE_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending COOKIE-ACK from socket %p\n", sk));
	/* process data bundled with cookie echo on new socket */
	if (sctp_return_more(cp) > 0) {
		sctp_recv_msg(sk, skb_get(cp));
		if (sk->state != SCTP_ESTABLISHED)
			goto done;
	}
	sctp_bundle_more(sk, sd, skb, 1);	/* don't nagle */
	sctp_send_msg(sk, sd, skb);
      done:
	freechunks(skb);
      enobufs:
	/* start idle timers */
	for (sd = SCTP_PROT(sk)->daddr; sd; sd = sd->next)
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
sctp_send_heartbeat(struct sock *sk, struct sctp_daddr *sd)
{
	struct sk_buff *skb;
	struct sctp_heartbeat *m;
	struct sctp_heartbeat_info *h;
	size_t fill, clen, hlen, plen;
	assert(sk);
	ensure(sd, return);
	fill = sd->hb_fill;
	hlen = sizeof(*h) + fill;
	clen = sizeof(*m) + hlen;
	plen = PADC(clen);
	sd->hb_time = jiffies;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_HEARTBEAT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	h = (typeof(h)) skb_put(skb, sizeof(*h));
	h->ph.type = SCTP_PTYPE_HEARTBEAT_INFO;
	h->ph.len = htons(hlen);
	h->hb_info.timestamp = sd->hb_time;
	h->hb_info.daddr = sd->daddr;
	h->hb_info.mtu = sd->mtu;
	bzero(skb_put(skb, PADC(fill)), fill);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending HEARTBEAT from socket %p\n", sk));
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
      enobufs:
	sd_mod_timeout(sd, &sd->timer_heartbeat, sd->rto);
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
sctp_send_heartbeat_ack(struct sock *sk, caddr_t hptr, size_t hlen)
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_heartbeat_ack *m;
	size_t clen = sizeof(*m) + hlen;
	assert(sk);
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_HEARTBEAT_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	bcopy(hptr, skb_put(skb, PADC(hlen)), hlen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending HEARTBEAT-ACK from socket %p\n", sk));
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
	return;
      noroute:
      enobufs:
	rare();
	return;
}

/*
 *  SEND ABORT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  There is no point in bundling control chunks after an ABORT chunk.  Also, DATA chunks are not to
 *  be bundled with ABORT chunks.
 */
STATIC void
sctp_send_abort(struct sock *sk)
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	if ((1 << sk->state) & ~(SCTPF_NEEDABORT))
		goto notneeded;
	if ((1 << sk->state) & SCTPF_CONNECTED)
		sd = sctp_route_normal(sk);
	else
		sd = SCTP_PROT(sk)->daddr;
	if (!sd)
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending ABORT from socket %p\n", sk));
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
      enobufs:
      noroute:
	if ((1 << sk->state) & ~(SCTPF_DISCONNECTED)) {
		if (sk->state != SCTP_CLOSED)
			SCTP_INC_STATS(SctpAborteds);
		sctp_change_state(sk, SCTP_CLOSED);
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
sctp_send_abort_error(struct sock *sk, int errn, void *aptr,	/* argument ptr */
		      size_t alen /* argument len */ )
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	if ((1 << sk->state) & ~(SCTPF_NEEDABORT))
		goto notneeded;
	if (!(sd = sctp_route_normal(sk)))
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) skb_put(skb, sizeof(*eh));
	eh->code = htons(errn);
	eh->len = htons(elen);
	bcopy(aptr, skb_put(skb, PADC(alen)), alen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending ABORT w/Cause from socket %p\n", sk));
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
      enobufs:
      noroute:
	if ((1 << sk->state) & ~(SCTPF_DISCONNECTED)) {
		if (sk->state != SCTP_CLOSED)
			SCTP_INC_STATS(SctpAborteds);
		sctp_change_state(sk, SCTP_CLOSED);
	}
	return;
      noerror:
	sctp_send_abort(sk);
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
sctp_send_shutdown(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_shutdown *m;
	size_t clen = sizeof(*m);
	assert(sk);
#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (!skb_queue_empty(&sk->sndq) || !skb_queue_empty(&sp->urgq)
	    || !skb_queue_empty(&sp->rtxq))
		swerr();
#endif				/* defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST */
	if (!(sd = sctp_route_normal(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	m->c_tsn = htonl(sp->r_ack);
	/* shutdown acks everything but dups and gaps */
	sp->sackf &= (SCTP_SACKF_DUP | SCTP_SACKF_GAP);
	if (sctp_timeout_pending(&sp->timer_sack))
		sp_del_timeout(sp, &sp->timer_sack);
#ifdef SCTP_CONFIG_ADD_IP
	if (sctp_timeout_pending(&sp->timer_asconf))
		sp_del_timeout(sp, &sp->timer_asconf);
#endif				/* SCTP_CONFIG_ADD_IP */
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sp_mod_timeout(sp, &sp->timer_shutdown, sd->rto);
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sk->state != SCTP_SHUTDOWN_SENT)
		sp_mod_timeout(sp, &sp->timer_guard, 5 * sd->rto);
	sctp_change_state(sk, SCTP_SHUTDOWN_SENT);
	printd(("Sending SHUTDOWN from socket %p\n", sk));
	sctp_bundle_more(sk, sd, skb, 1);	/* not DATA */
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      noroute:
	rare();
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
sctp_send_shutdown_ack(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_shutdown_ack *m;
	size_t clen = sizeof(*m);
	assert(sk);
#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (!skb_queue_empty(&sk->sndq) || !skb_queue_empty(&sp->urgq)
	    || !skb_queue_empty(&sp->rtxq))
		swerr();
#endif				/* defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST */
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sp_del_timeout(sp, &sp->timer_sack);
#ifdef SCTP_CONFIG_ADD_IP
	sp_del_timeout(sp, &sp->timer_asconf);
#endif				/* SCTP_CONFIG_ADD_IP */
	sp_mod_timeout(sp, &sp->timer_shutdown, sd->rto);
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sk->state != SCTP_SHUTDOWN_ACK_SENT)
		sp_mod_timeout(sp, &sp->timer_guard, 5 * sd->rto);
	sctp_change_state(sk, SCTP_SHUTDOWN_ACK_SENT);
	printd(("Sending SHUTDOWN-ACK from socket %p\n", sk));
	sctp_bundle_more(sk, sd, skb, 1);	/* not DATA */
	sctp_send_msg(sk, sd, skb);
	abnormal(sp->retry);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      noroute:
	rare();
	return (-EFAULT);
}

/*
 *  SEND SHUTDOWN COMPLETE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_shutdown_complete(struct sock *sk)
{
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);
#if defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST
	if (!skb_queue_empty(&sk->sndq)
	    || !skb_queue_empty(&SCTP_PROT(sk)->urgq)
	    || !skb_queue_empty(&SCTP_PROT(sk)->rtxq))
		swerr();
#endif				/* defined SCTP_CONFIG_DEBUG || defined SCTP_CONFIG_TEST */
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending SHUTDOWN-COMPLETE from socket %p\n", sk));
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
	return;
      enobufs:
      noroute:
	sctp_change_state(sk, (sk->state == SCTP_LISTEN) ? SCTP_LISTEN : SCTP_CLOSED);
	return;
}

#ifdef SCTP_CONFIG_ECN
/*
 *  SEND ECNE (Explicit Congestion Notification Echo)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Just mark an ECNE chunk to be sent with the next SACK (and don't delay SACKs).
 */
STATIC INLINE void
sctp_send_ecne(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
		printd(("Marking ECNE from socket %p\n", sk));
		sp->sackf |= SCTP_SACKF_ECN;
	}
}

/*
 *  SEND CWR (Congestion Window Reduction)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Just mark a CWR chunk to be bundled with the next DATA.
 */
STATIC INLINE void
sctp_send_cwr(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
		printd(("Marking CWR from socket %p\n", sk));
		sp->sackf |= SCTP_SACKF_CWR;
	}
}
#endif				/* SCTP_CONFIG_ECN */

#ifdef SCTP_CONFIG_ADD_IP
/*
 *  ABORT ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_abort_asconf(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
			ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
			ss->flags |= SCTP_SRCEF_ADD_REQUEST;
			sp->sackf |= SCTP_SACKF_ASC;
		}
		if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
			ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
			ss->flags |= SCTP_SRCEF_DEL_REQUEST;
			sp->sackf |= SCTP_SACKF_ASC;
		}
		if (ss->flags & SCTP_SRCEF_SET_PENDING) {
			ss->flags &= ~SCTP_SRCEF_SET_PENDING;
			ss->flags |= SCTP_SRCEF_SET_REQUEST;
			sp->sackf |= SCTP_SACKF_ASC;
		}
	}
	if (sk->state == SCTP_ESTABLISHED) {
		sp_del_timeout(sp, &sp->timer_asconf);
		freechunks(xchg(&sp->retry, NULL));
	}
	return;
}

/*
 *  SEND ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_asconf(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_asconf *m;
	struct sctp_add_ip *a;
	struct sctp_del_ip *d;
	struct sctp_set_ip *s;
	struct sctp_ipv4_addr *p;
	size_t clen = sizeof(*m);
	struct sctp_saddr *ss;
	int requested = 0;
	unsigned char *ptr;
	if (sk->state != SCTP_ESTABLISHED)
		goto skip;
	if (!(sp->p_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto skip;
	/* ADD-IP 4.1 (A3) */
	if (sp->reply || sctp_timeout_pending(&sp->timer_asconf))
		goto skip;
	ensure(sp->sackf & SCTP_SACKF_ASC, goto skip);
	if (!(sd = sctp_route_normal(sk)))
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
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ASCONF;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	/* ADD-IP 4.1 (A2) */
	m->asn = htonl(++sp->l_asn);
	ptr = (unsigned char *) (m + 1);
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (sp->p_caps & SCTP_CAPS_ADD_IP) {
			if (ss->flags & SCTP_SRCEF_ADD_REQUEST) {
				a = ((typeof(a)) ptr)++;
				a->ph.type = SCTP_PTYPE_ADD_IP;
				a->ph.len = __constant_htons(sizeof(*a) + sizeof(*p));
				a->id = htonl((uint32_t) ss);
				p = ((typeof(p)) ptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_ADD_REQUEST;
				ss->flags |= SCTP_SRCEF_ADD_PENDING;
			}
			if (ss->flags & SCTP_SRCEF_DEL_REQUEST) {
				d = ((typeof(d)) ptr)++;
				d->ph.type = SCTP_PTYPE_DEL_IP;
				d->ph.len = __constant_htons(sizeof(*d) + sizeof(*p));
				d->id = htonl((uint32_t) ss);
				p = ((typeof(p)) ptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_DEL_REQUEST;
				ss->flags |= SCTP_SRCEF_DEL_PENDING;
			}
		}
		if (sp->p_caps & SCTP_CAPS_SET_IP) {
			if (ss->flags & SCTP_SRCEF_SET_REQUEST) {
				s = ((typeof(s)) ptr)++;
				s->ph.type = SCTP_PTYPE_SET_IP;
				s->ph.len = __constant_htons(sizeof(*s) + sizeof(*p));
				s->id = htonl((uint32_t) ss);
				p = ((typeof(p)) ptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_SET_REQUEST;
				ss->flags |= SCTP_SRCEF_SET_PENDING;
			}
		}
	}
	sp->sackf &= ~SCTP_SACKF_ASC;
	SCTP_INC_STATS(SctpOutCtrlChunks);
	/* ADD-IP 4.1 (A4) */
	sp_mod_timeout(sp, &sp->timer_asconf, sd->rto);
	printd(("Sending ASCONF from socket %p\n", sk));
	sctp_bundle_more(sk, sd, skb, 1);
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->retry, skb));
      skip:
	return;
      enobufs:
	rare();
	return;
      noroute:
	rare();
	return;
}

/*
 *  SEND ASCONF ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_asconf_ack(struct sock *sk, caddr_t rptr, size_t rlen)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_asconf_ack *m;
	size_t clen = sizeof(*m) + rlen;
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ASCONF_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->asn = ntohl(sp->p_asn++);
	bcopy(rptr, m + 1, rlen);	/* copy in response TLVs */
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending ASCONF-ACK from socket %p\n", sk));
	sctp_bundle_more(sk, sd, skb, 1);
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->reply, skb));
	return;
      enobufs:
	rare();
	return;
      noroute:
	rare();
	return;
}
#endif				/* SCTP_CONFIG_ADD_IP */

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
/*
 *  SEND FORWARD TSN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE void
sctp_send_forward_tsn(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	/* PR-SCTP 3.5 (F2) */
	sp->sackf |= SCTP_SACKF_FSN;
	for (sd = sp->daddr; sd; sd = sd->next) {
		if (sd->flags & SCTP_DESTF_FORWDTSN) {
			if (!sd->in_flight && sctp_timeout_pending(&sd->timer_retrans))
				sd_del_timeout(sd, &sd->timer_retrans);
			sd->flags &= ~SCTP_DESTF_FORWDTSN;
		}
	}
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

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
void
sctp_send_abort_ootb(uint32_t daddr, uint32_t saddr, struct sctphdr *sh)
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	if (!sh)
		goto noroute;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending ABORT (OOTB)\n"));
	sctp_xmit_ootb(daddr, saddr, skb);
	return;
      noroute:
      enobufs:
	rare();
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
			   caddr_t aptr,	/* argument ptr */
			   size_t alen /* argument len */ )
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	if (!sh)
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) skb_put(skb, sizeof(*eh));
	eh->code = htons(errn);
	eh->len = htons(elen);
	bcopy(aptr, skb_put(skb, PADC(alen)), alen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctp_xmit_ootb(daddr, saddr, skb);
	return;
      noerror:
	sctp_send_abort_ootb(daddr, saddr, sh);
	return;
      noroute:
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
sctp_send_abort_init(struct sock *sk, uint32_t daddr, uint32_t saddr, struct sctphdr *sh,
		     uint32_t i_tag)
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	if (!sh)
		goto noroute;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	(((struct sctphdr *) skb->data) - 1)->v_tag = i_tag;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending ABORT (INIT)\n"));
	sctp_xmit_msg(saddr, daddr, skb, sk);
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
sctp_send_abort_error_init(struct sock *sk, uint32_t daddr, uint32_t saddr, struct sctphdr *sh,
			   uint32_t i_tag, int errn, caddr_t aptr, size_t alen)
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	if (!sh)
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	(((struct sctphdr *) skb->data) - 1)->v_tag = i_tag;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) skb_put(skb, sizeof(*eh));
	eh->code = htons(errn);
	eh->len = htons(elen);
	bcopy(aptr, skb_put(skb, PADC(alen)), alen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	printd(("Sending ABORT w/CAUSE (INIT)\n"));
	sctp_xmit_msg(saddr, daddr, skb, sk);
	return;
      noerror:
	sctp_send_abort_init(sk, daddr, saddr, sh, i_tag);
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
	struct sk_buff *skb;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);
	if (!sh)
		goto noroute;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
	SCTP_INC_STATS(SctpOutCtrlChunks);
	sctp_xmit_ootb(daddr, saddr, skb);
	return;
      enobufs:
      noroute:
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
sctp_return_more(struct sk_buff *skb)
{
	size_t dlen = skb->len;
	struct sctpchdr *ch = SCTP_CH(skb);
	size_t clen = ntohs(ch->len);
	size_t plen = PADC(clen);
	if ((ch = (typeof(ch)) skb_pull(skb, plen))
	    && ((dlen = skb->len) >= sizeof(*ch))
	    && ((clen = ntohs(ch->len)) >= sizeof(*ch))
	    && ((plen = PADC(clen)) <= dlen))
		return (clen);
	if (skb->len)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  RETURN VALUE when not expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_stop(struct sk_buff *skb)
{
	int ret = sctp_return_more(skb) ? -EPROTO : 0;
	unusual(ret < 0);
	return (ret);
}

#if 0
/*
 *  RETURN VALUE when expecting specific chunk or nothing
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_check(struct sk_buff *skb, uint ctype)
{
	int ret = sctp_return_more(skb);
	ret = (ret > 0 && SCTP_CH(skb)->type != ctype) ? -EPROTO : ret;
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
	sd_del_timeout(sd, &sd->timer_heartbeat);
	rtt = sd->rto + sd->hb_itvl + ((jiffies & 0x1) * (sd->rto >> 1));
	printd(("INFO: 0x%p setting idle timer to %lu jiffies\n", sd, rtt));
	sd_mod_timeout(sd, &sd->timer_idle, rtt);
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
			sd->srtt -= (sd->srtt - rtt) >> 3;
		}
		if (rttvar > sd->rttvar)
			sd->rttvar += (rttvar - sd->rttvar) >> 2;
		else
			sd->rttvar -= (sd->rttvar - rttvar) >> 2;
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
		_ptrace(("Aaaarg! Reseting counts for address %d.%d.%d.%d\n",
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
sctp_dest_calc(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
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
			sd_del_timeout(sd, &sd->timer_retrans);
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
			sd_del_timeout(sd, &sd->timer_retrans);
#ifdef SCTP_CONFIG_ADD_IP
			if (sd->flags & SCTP_DESTF_UNUSABLE)
				sctp_del_daddr(sd);
#endif				/* SCTP_CONFIG_ADD_IP */
		} else if (!sctp_timeout_pending(&sd->timer_retrans))
			sd_set_timeout(sd, &sd->timer_retrans, sd->rto);
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
sctp_cumm_ack(struct sock *sk, uint32_t ack)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	/* PR-SCTP 3.5 (A3) */
	sp->t_ack = ack;
	while ((skb = skb_peek(&sp->rtxq)) && !after(SCTP_SKB_CB(skb)->tsn, ack)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
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
				sk->wmem_queued -= PADC(skb->len);
				ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
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
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG)
			__skb_queue_tail(&sp->ackq, __skb_dequeue(&sp->rtxq));
		else
			kfree_skb(__skb_dequeue(&sp->rtxq));
		/* NOTE: In STREAMS we need to back-enable the write queue if required.  In the
		   Linux socket code this is accomplished by the skb destructor with the kfree_skb
		   above, or when the ackq is purged. */
	}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	/* PR-SCTP 3.5 (A3 - C1) */
	if (before(sp->l_fsn, sp->t_ack))
		sp->l_fsn = sp->t_ack;
	/* PR-SCTP 3.5 (A3 - C2) */
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		sctp_tcb_t *cb = SCTP_SKB_CB(skb);
		if (!after(cb->tsn, sp->l_fsn))
			continue;
		if (!(cb->flags & SCTPCB_FLAG_DROPPED))
			break;
		/* push the forward ack point */
		if (cb->tsn == sp->l_fsn + 1)
			sp->l_fsn++;
	}
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sk);
		if (!sctp_timeout_pending(&sp->timer_sack))
			sp_set_timeout(sp, &sp->timer_sack, sp->max_sack);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			sp_del_timeout(sp, &sp->timer_sack);
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
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
sctp_ack_calc(struct sock *sk, sctp_timer_t *tp)
{
	sctp_t *sp = SCTP_PROT(sk);
	sctp_tcb_t *cb = SCTP_SKB_CB(sp->retry);
	assert(sp->retry);
	sp_del_timeout(sp, tp);
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
sctp_deliver_data(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	sctp_tcb_t *cb, *cb_next = sp->gaps;
	while ((cb = cb_next)) {
		cb_next = cb->next;
		if (!(cb->flags & SCTPCB_FLAG_DELIV)) {
			struct sk_buff *skp;
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
			if ((skp = skb_clone(cb->skb, GFP_ATOMIC))) {
				sctp_get_owner_r(skp, skb);	/* Why do I do this??? */
				sp->rmem_queued += cb->dlen;
				cb->flags |= SCTPCB_FLAG_DELIV;
				if (ord) {
					__skb_queue_tail(&sk->rcvq, skp);
					sctp_data_ind(sk, cb->dlen);
					/* IMPLEMENTATION NOTE:- Sockets version indicates data
					   here.  STREAMS doesn't because it indicates data only
					   when it is actually delivered upstream. */
					if (!more) {
						st->n.more &= ~SCTP_STRMF_MORE;
						st->ssn = cb->ssn;
					} else
						st->n.more |= SCTP_STRMF_MORE;
					SCTP_INC_STATS(SctpInOrderChunks);
				} else {
					__skb_queue_tail(&sp->expq, skp);
					sctp_exdata_ind(sk, cb->dlen);
					/* IMPLEMENTATION NOTE:- Sockets version indicates data
					   here.  STREAMS doesn't because it indicates data only
					   when it is actually delivered upstream. */
					if (!more)
						st->x.more &= ~SCTP_STRMF_MORE;
					else
						st->x.more |= SCTP_STRMF_MORE;
					SCTP_INC_STATS(SctpInUnorderChunks);
				}
#if STREAMS
				/* STREAMS calculates reassembled user messages here.  Sockets does
				   not.  Sockets calculates reassembled user messages in
				   sctp_recvmsg. Thus sockets calculates delivered reassembled user
				   messages whereas STREAMS calculates provider reassembled user
				   messages. */
				if (!(cb)->flags & SCTPCB_FLAG_FIRST_FRAG)
					SCTP_INC_STATS_USER(SctpReasmUsrMsgs);
#endif				/* STREAMS */
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
			sctp_send_sack(sk);
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
			sctp_send_sack(sk);
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
			skb_unlink(cb->skb);
			kfree_skb(cb->skb);
		}
	}
}

/*
 *  RECV DATA
 *  -------------------------------------------------------------------------
 *  We have received a DATA chunk in a T-Provider state where is it valid to receive DATA
 *  (TS_DATA_XFER and TS_WIND_ORDREL).  We can also receive data chunks in TS_IDLE or TS_WRES_CIND
 *  on a listening socket bundled with a COOKIE ECHO.  In any other states we discard the data.
 *  Because no other chunks can be bundled after a DATA chunk (just yet), we process all the DATA
 *  chunks in the remainder of the message in a single loop here.
 */
STATIC int
sctp_recv_data(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err;
	size_t plen;
#ifdef SCTP_CONFIG_ECN
	int is_ce = INET_ECN_is_ce(SCTP_IPH(skb)->tos);
#endif				/* SCTP_CONFIG_ECN */
	int newd, data, progress = 0;	/* number of new data chunks */
	sctp_tcb_t *cb;
	struct sctp_data *m;
	printd(("Received DATA on socket %p\n", sk));
	assert(sk);
	assert(skb);
	if ((1 << sk->state) & ~(SCTPF_RECEIVING) || sk->dead || sk->shutdown & RCV_SHUTDOWN)
		goto outstate;
	for (newd = 0, data = 0, plen = 0, err = 0; err == 0;) {
		struct sk_buff *skd;
		struct sctp_strm *st;
		sctp_tcb_t **gap;
		uint32_t tsn, ppi;
		uint16_t sid, ssn;
		size_t clen, blen, dlen;
		uint flags;
		int ord, more;
		if ((!(m = (typeof(m)) skb_pull(skb, plen)))
		    || (skb->len <= 0)
		    || ((blen = skb->len) < sizeof(struct sctpchdr))
		    || ((clen = ntohs(m->ch.len)) < sizeof(*m))
		    || ((plen = PADC(clen)) > blen)) {
			if (skb->len)
				goto emsgsize;
			break;
		}
		flags = (m->ch.flags);
		ord = !(flags & SCTPCB_FLAG_URG);
		more = !(flags & SCTPCB_FLAG_LAST_FRAG) ? SCTP_STRMF_MORE : 0;
		tsn = ntohl(m->tsn);
		sid = ntohs(m->sid);
		ssn = ntohs(m->ssn);
		ppi = ntohl(m->ppi);
		dlen = clen - sizeof(*m);
		if (m->ch.type != SCTP_CTYPE_DATA)
			goto eproto;
		if (dlen == 0)
			goto baddata;
		if (sid >= sp->n_istr)
			goto badstream;
		sctp_put_owner_r(skb);
		if (atomic_read(&sk->rmem_alloc) >= sk->rcvbuf)
			goto flowcontrol;
		if (!(st = sctp_istrm_find(sp, sid, &err)))
			goto enomem;
		if (!(skd = skb_clone(skb, GFP_ATOMIC)))
			goto enobufs;
		data++;
		/* pull copy to data only */
		skb_pull(skd, sizeof(*m));
		/* trim copy to data only */
		skb_trim(skd, dlen);
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
		cb = SCTP_SKB_CB(skd);
		bzero(cb, sizeof(*cb));
		cb->dlen = dlen;
		cb->when = jiffies;
		cb->tsn = tsn;
		cb->sid = sid;
		cb->ssn = ssn;
		cb->ppi = ppi;
		cb->flags = flags & 0x7;
		cb->daddr = sp->caddr;
		cb->skb = skd;
		cb->st = st;
		cb->tail = cb;
		_usual(sp->caddr);
		sctp_set_owner_r(skd, sk);
		/* fast path, next expected, nothing out of order */
		if (tsn == sp->r_ack + 1 && skb_queue_empty(&sp->oooq)) {
			/* we have next expected TSN, just process it */
			sp->rmem_queued += dlen;
			cb->flags |= SCTPCB_FLAG_DELIV;
			if (ord) {
				__skb_queue_tail(&sk->rcvq, skd);
				sctp_data_ind(sk, dlen);
				if (!more) {
					st->n.more &= ~SCTP_STRMF_MORE;
					st->ssn = cb->ssn;
				} else
					st->n.more |= SCTP_STRMF_MORE;
				SCTP_INC_STATS(SctpInOrderChunks);
			} else {
				__skb_queue_tail(&sp->expq, skd);
				sctp_exdata_ind(sk, dlen);
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
			if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
				if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
					sp->l_lsn = tsn;
					sctp_send_ecne(sk);
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
				__skb_insert(skd, (*gap)->skb->prev, (*gap)->skb,
					     (*gap)->skb->list);
				cb->next = (*gap);
				(*gap) = cb;
				sp->ngaps++;
			} else if (tsn == (*gap)->tail->tsn + 1) {
				/* expand at end of gap */
				ptrace(("INFO: Expecting tsn = %u, expanding %u\n", sp->r_ack,
					tsn));
				__skb_queue_tail(&sp->oooq, skd);
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
				_usual(sp->ngaps);
				sp->ngaps--;
				progress = 1;
			}
			break;
		}
		if (!(*gap)) {
			/* append to list */
			printd(("INFO: Expecting tsn = %u, appending %u\n", sp->r_ack, tsn));
			__skb_queue_tail(&sp->oooq, skd);
			cb->next = (*gap);
			(*gap) = cb;
			sp->ngaps++;
		}
		sp->nunds++;	/* more undelivered data */
#ifdef SCTP_CONFIG_ECN
		if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
			if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
				sp->l_lsn = tsn;
				sctp_send_ecne(sk);
			}
		}
#endif				/* SCTP_CONFIG_ECN */
		newd++;
		continue;
	      sctp_recv_data_duplicate:
		/* message is a duplicate tsn */
		ptrace(("INFO: Expecting tsn = %u, duplicate %u\n", sp->r_ack, tsn));
		__skb_queue_tail(&sp->dupq, skd);
		cb->next = sp->dups;
		sp->dups = cb;
		sp->ndups++;
		continue;
#if 0
	      free_error:
		freemsg(dp);
		break;
#endif
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
		rare();
		/* Protect from teardrop attack without renegging */
		if (skb_queue_empty(&sk->rcvq)) {
			/* TODO: need to put renegging code in here: renegging should first walk
			   the gap queue looking for any data that we have gap acked which has not
			   yet been delivered to the user.  Any such data can be deleted. Once
			   deleting this data we can check if we can process the current message
			   anyway.  If that is true, we can mark that a sack is required
			   immediately and jump back up to the top where we entered flow control.
			   If we are still flow controlled we will need to abort the association.
			   For now we just abort the association. */
			err = -ECONNRESET;
			sctp_send_abort(sk);
			sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
			break;
		}
		/* If we have data that the user has not read yet, then we keep all our gaps,
		   because the user reading data will make some room to process things later.  We
		   must send an immediate SACK regardless, per the IG requirements. */
		/* SCTP Implementor's Guide Section 2.15.1 */
		sctp_send_sack(sk);
		ptrace(("ERROR: flow controlled (discard)\n"));
		err = 0;
		break;		/* flow controlled (discard) */
	      badstream:
		ptrace(("PROTO: invalid stream id\n"));
		sctp_send_error(sk, SCTP_CAUSE_INVALID_STR, (caddr_t) &m->sid, sizeof(m->sid));
		continue;	/* just skip that DATA chunk */
	      baddata:
		ptrace(("PROTO: no user data in data chunk\n"));
		/* RFC 2960 6.2: ...no user data... */
		sctp_send_abort_error(sk, SCTP_CAUSE_NO_DATA, &m->tsn, sizeof(m->tsn));
		sctp_abort(sk, SCTP_ORIG_PROVIDER, -ECONNRESET);
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
	sctp_deliver_data(sk, skb);
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
				sd_del_timeout(sd, &sd->timer_idle);
				sctp_send_heartbeat(sk, sd);
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
			sp->sackf += ((sp->sackf & 0x3) < 3) ? SCTP_SACKF_NEW : 0;
			if (!sctp_timeout_pending(&sp->timer_sack))
				sp_set_timeout(sp, &sp->timer_sack, sp->max_sack);
		} else
			sp->sackf |= SCTP_SACKF_TIM;
	}
	if (sk->state == SCTP_SHUTDOWN_SENT)
		sctp_send_shutdown(sk);
      no_data:
	goto done;
      done:
#if STREAMS
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
#endif				/* STREAMS */
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
	rare();
	if (sk->state == SCTP_COOKIE_WAIT) {
		sctp_abort(sk, SCTP_ORIG_PROVIDER, -ECONNRESET);
		err = -EPROTO;
	}
	goto done;
}

/*
 *  RECV SACK
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_sack(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skd;
	struct sctp_sack *m;
	size_t ngaps, ndups;
	uint32_t ack, rwnd;
	uint16_t *gaps;
	printd(("Received SACK on socket %p\n", sk));
	assert(sk);
	assert(skb);
	if ((1 << sk->state) & ~(SCTPF_SENDING))
		goto outstate;
	m = (struct sctp_sack *) skb->data;
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
#if STREAMS
	/* If the receive window is increasing and we have data in the write queue, we might need
	   to backenable. */
	if (rwnd > sp->p_rwnd && sp->wq && sp->wq->q_count)
		qenable(sp->wq);
#endif				/* STREAMS */
	/* RFC 1122 4.2.3.4 IMPLEMENTATION (3) */
	sp->p_rbuf = (sp->p_rbuf >= rwnd) ? sp->p_rbuf : rwnd;
	/* RFC 2960 6.2.1 (D) ii) */
	sp->p_rwnd = rwnd;	/* we keep in_flight separate from a_rwnd */
	if (after(ack, sp->t_ack)) {
		/* advance the cummulative ack point and check need to perform per-round-trip and
		   cwnd calcs */
		sctp_cumm_ack(sk, ack);
		/* if we have cummulatively acked something, we will skip FR analysis for this
		   SACK. */
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
		for (skd = skb_peek(&sp->rtxq); skd; skd = skb_next(skd)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skd);
			/* RFC 2960 6.3.2 (R4) (reneg) */
			if (cb->flags & SCTPCB_FLAG_SACKED) {
				struct sctp_daddr *sd = cb->daddr;
				cb->flags &= ~SCTPCB_FLAG_SACKED;
				ensure(sp->nsack, sp->nsack = 1);
				sp->nsack--;
				if (sd && !sctp_timeout_pending(&sd->timer_retrans))
					sd_set_timeout(sd, &sd->timer_retrans, sd->rto);
				else
					seldom();
			}
		}
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
			skd = skb_peek(&sp->rtxq);
			for (; skd && before(SCTP_SKB_CB(skd)->tsn, beg); skd = skb_next(skd)) {
				sctp_tcb_t *cb = SCTP_SKB_CB(skd);
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
			for (; skd && !after(SCTP_SKB_CB(skd)->tsn, end); skd = skb_next(skd)) {
				sctp_tcb_t *cb = SCTP_SKB_CB(skd);
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
		}
		/* walk the whole retrans buffer looking for holes and renegs */
		for (skd = skb_peek(&sp->rtxq); skd; skd = skb_next(skd)) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skd);
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
						sk->wmem_queued -= PADC(skd->len);
						ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
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
					sk->wmem_queued += PADC(skd->len);
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
					if (sd && !sctp_timeout_pending(&sd->timer_retrans))
						sd_set_timeout(sd, &sd->timer_retrans, sd->rto);
					else
						seldom();
				}
				continue;
			}
			/* msg is after all gapack blocks */
			break;
		}
	}
      skip_rtx_analysis:
	sctp_dest_calc(sk);
	/* After receiving a cummulative ack, I want to check if the sndq, urgq and rtxq is empty
	   and a SHUTDOWN or SHUTDOWN-ACK is pending. If so, I want to issue these primitives. */
	if (((1 << sk->
	      state) & (SCTPF_SHUTDOWN_PENDING | SCTPF_SHUTDOWN_RECEIVED | SCTPF_SHUTDOWN_RECVWAIT))
	    && atomic_read(&sk->wmem_alloc) <= 0) {
		seldom();
		switch (sk->state) {
		case SCTP_SHUTDOWN_PENDING:
			/* Send the SHUTDOWN I didn't send before. */
			sctp_send_shutdown(sk);
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			/* Send the SHUTDOWN-ACK I didn't send before */
			if (sk->dead)
				sctp_send_shutdown_ack(sk);
			break;
		case SCTP_SHUTDOWN_RECVWAIT:
			/* Send the SHUTDOWN-ACK I didn't send before */
			sctp_send_shutdown_ack(sk);
			break;
		default:
			swerr();
			return (-EFAULT);
		}
	}
      done:
	{
		int ret;
		if ((ret = sctp_return_more(skb)) > 0) {
			struct sctpchdr *ch = SCTP_CH(skb);
			switch (ch->type) {
				/* RFC 2960 6 */
			case SCTP_CTYPE_DATA:
				/* RFC 2960 6.5 */
			case SCTP_CTYPE_ERROR:
				/* RFC 2960 3.3.7 */
			case SCTP_CTYPE_ABORT:
				break;
			default:
				rare();
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
sctp_recv_error(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_error *m;
	struct sctpehdr *eh;
	int ecode;
	size_t clen, elen;
	printd(("Received ERROR on socket %p\n", sk));
	assert(sk);
	assert(skb);
	seldom();
	m = (typeof(m)) skb->data;
	eh = (typeof(eh)) (m + 1);
	if ((clen = ntohs(m->ch.len)) < sizeof(*m) + sizeof(*eh))
		goto emsgsize;
	if ((elen = ntohs(eh->len)) < sizeof(*eh) || sizeof(*m) + elen > clen)
		goto emsgsize;
	ecode = ntohs(eh->code);
	switch (ecode) {
	case SCTP_CAUSE_STALE_COOKIE:
	{
		sctp_t *sp = SCTP_PROT(sk);
		sctp_tcb_t *cb;
		struct sctp_daddr *sd;
		if (sk->state != SCTP_COOKIE_ECHOED)
			break;
		ensure(sp->retry, goto recv_error_error);
		cb = SCTP_SKB_CB(sp->retry);
		sd = cb->daddr;
		ensure(sd, goto recv_error_error);
		/* We can try again with cookie preservative, and then we can keep trying until we
		   have tried as many times as we can... */
		if (!sp->ck_inc) {
			sp->ck_inc = sp->ck_inc + (sd->rto >> 1);
			sctp_ack_calc(sk, &sp->timer_init);
			if ((err = sctp_send_init(sk)))
				return (err);
			return sctp_return_stop(skb);
		}
		/* RFC 2960 5.2.6 (1) */
		if (cb->trans < sp->max_inits) {
			sp_del_timeout(sp, &sp->timer_init);
			/* RFC 2960 5.2.6 (3) */
			if (cb->trans < 2)
				sctp_rtt_calc(sd, cb->when);
			sctp_send_msg(sk, sd, sp->retry);
			return sctp_return_stop(skb);
		}
		/* RFC 2960 5.2.6 (2) */
		goto recv_error_error;
	}
	case SCTP_CAUSE_INVALID_PARM:
	case SCTP_CAUSE_BAD_ADDRESS:
		/* If the sender of the ERROR has already given us a valid INIT-ACK then we can
		   ignore these errors. */
		if (sk->state == SCTP_COOKIE_ECHOED)
			break;
		seldom();
	case SCTP_CAUSE_MISSING_PARM:
	case SCTP_CAUSE_NO_RESOURCE:
	case SCTP_CAUSE_INVALID_STR:
		/* These errors are bad.  If we don't get an abort with them then we must abort the 
		   association. */
	      recv_error_error:
		sctp_send_abort(sk);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, -ECONNABORTED);
		return sctp_return_stop(skb);
	case SCTP_CAUSE_BAD_CHUNK_TYPE:
#if defined(SCTP_CONFIG_ADD_IP)||defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	{
		sctp_t *sp = SCTP_PROT(sk);
		struct sctpchdr *ch = (typeof(ch)) (eh + 1);
		switch (ch->type) {
#if defined(SCTP_CONFIG_ADD_IP)
		case SCTP_CTYPE_ASCONF:
		case SCTP_CTYPE_ASCONF_ACK:
			sctp_abort_asconf(sk);
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
		sctp_t *sp = SCTP_PROT(sk);
		struct sctpphdr *ph = (typeof(ph)) (eh + 1);
		int plen;
		if (elen < sizeof(*eh) + sizeof(*ph))
			goto emsgsize;
		if ((plen = ntohs(ph->len)) < sizeof(*ph) || PADC(plen) > skb->len)
			goto emsgsize;
		switch (ph->type) {
#if defined(SCTP_CONFIG_ADD_IP)
		case SCTP_PTYPE_ADD_IP:
		case SCTP_PTYPE_DEL_IP:
			/* FIXME: what about valid sets with invalid adds ? */
			sctp_abort_asconf(sk);
			sp->p_caps &= ~SCTP_CAPS_ADD_IP;
			break;
		case SCTP_PTYPE_SET_IP:
			/* FIXME: what about valid sets with invalid adds ? */
			sctp_abort_asconf(sk);
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
			INET_ECN_dontxmit(sk);
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
	return sctp_return_more(skb);	/* ignore */
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
sctp_recv_abort_listening(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *cp;
	struct iphdr *iph = SCTP_IPH(skb);
	struct sctphdr *sh = SCTP_SH(skb);
	struct sctp_abort *m = (typeof(m)) skb->data;
	ulong orig = (m->ch.flags & 0x1) ? SCTP_ORIG_PROVIDER : SCTP_ORIG_USER;
	/* FIXME: above orig is not completely true, it also depends on whether there is a cause
	   value, what the cause value is, and the state. */
	printd(("Received ABORT in listening state on stream %p\n", sp));
	for (cp = skb_peek(&sp->conq); cp; cp = skb_next(cp)) {
		struct sctp_cookie_echo *ce = (typeof(ce)) cp->data;
		struct sctp_cookie *ck = (typeof(ck)) ce->cookie;
		if (ck->v_tag == sh->v_tag && ck->sport == sh->dest && ck->dport == sh->srce
		    && ck->saddr == iph->daddr && ck->daddr == iph->saddr) {
			int err;
			/* FIXME: the reasons -ECONNRESET is only if there is no cause value in the 
			   abort message. If there is a cause value, the reason should reflect the
			   cause value if it makes sense. */
			printd(("INFO: Removing Connection Indication on ABORT\n"));
			/* NOTE: The disconnect indicaiton function will unlink and free the
			   connection indication when the function does not return an error */
			if ((err = sctp_discon_ind(sk, orig, -ECONNRESET, cp)))
				return (err);
			break;
		}
	}
	_usual(cp);
	return sctp_return_stop(skb);
}
STATIC int
sctp_recv_abort(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_abort *m = (typeof(m)) skb->data;
	ulong orig = (m->ch.flags & 0x1) ? SCTP_ORIG_PROVIDER : SCTP_ORIG_USER;
	/* FIXME: above orig is not completely true, it also depends on whether there is a cause
	   value, what the cause value is, and the state. */
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	printd(("Received ABORT on socket %p\n", sk));
	switch (sk->state) {
	case SCTP_LISTEN:
		return sctp_recv_abort_listening(sk, skb);
	case SCTP_COOKIE_WAIT:
	case SCTP_COOKIE_ECHOED:
		err = -ECONNREFUSED;
		break;
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
	case SCTP_SHUTDOWN_ACK_SENT:
		err = -EPIPE;
		break;
	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
		err = -ECONNRESET;
		break;
	default:
		swerr();
		return sctp_return_stop(skb);
	}
	/* FIXME: the reasons set above are only if there is no cause value in the abort message.
	   If there is a cause value, the reason should reflect the cause value if it makes sense. */
	sctp_abort(sk, orig, err);
	return sctp_return_stop(skb);
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
sctp_recv_init(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct iphdr *iph;
	struct sctphdr *sh;
	struct sctp_init *m;
	struct sock *oldsk;
	union sctp_parm *ph;
	unsigned char *pptr, *pend;
	size_t plen;
	size_t anum = 0;
	ulong ck_inc = 0;
	int err = 0;
	size_t errl = 0;
	unsigned char *errp = NULL;
	struct sctp_cookie ck;
	struct sk_buff *unrec = NULL;
	uint32_t p_caps, p_ali;
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	static ulong last_init = 0;
	if (last_init && jiffies < last_init + sp->throttle)
		goto ebusy;
	last_init = jiffies;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
	printd(("Received INIT on socket %p\n", sk));
	assert(sk);
	assert(skb);
	iph = SCTP_IPH(skb);
	sh = SCTP_SH(skb);
	m = (struct sctp_init *) skb->data;
	{
		size_t clen = ntohs(m->ch.len);
		pptr = (unsigned char *) (m + 1);
		pend = pptr + clen - sizeof(*m);
		if (clen < sizeof(*m))
			goto emsgsize;
		/* SCTP IG 2.11 not allowed to bundle */
		if (PADC(clen) < skb->len)
			goto eproto;
	}
	/* RFC 2960 p.26 initiate tag zero */
	if (!m->i_tag)
		goto invalid_parm;
	if (!(unrec = alloc_skb(skb->len, GFP_ATOMIC)))
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
			INET_ECN_xmit(sk);
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
				bcopy(pptr, skb_put(unrec, PADC(plen)), PADC(plen));
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
		_printd(("INFO: ck.timestamp = %lu\n", ck.timestamp));
		_printd(("INFO: ck.lifespan  = %lu\n", ck.lifespan));
		_printd(("INFO: ck.daddr     = %d.%d.%d.%d\n", (ck.daddr >> 0) & 0xff,
			 (ck.daddr >> 8) & 0xff, (ck.daddr >> 16) & 0xff, (ck.daddr >> 24) & 0xff));
		_printd(("INFO: ck.saddr     = %d.%d.%d.%d\n", (ck.saddr >> 0) & 0xff,
			 (ck.saddr >> 8) & 0xff, (ck.saddr >> 16) & 0xff, (ck.saddr >> 24) & 0xff));
		_printd(("INFO: ck.dport     = %hu\n", ntohs(ck.dport)));
		_printd(("INFO: ck.sport     = %hu\n", ntohs(ck.sport)));
		_printd(("INFO: ck.v_tag     = %08x\n", ck.v_tag));
		_printd(("INFO: ck.p_tag     = %08x\n", ck.p_tag));
		_printd(("INFO: ck.p_tsn     = %u\n", ck.p_tsn));
		_printd(("INFO: ck.p_rwnd    = %u\n", ck.p_rwnd));
		_printd(("INFO: ck.n_istr    = %hu\n", ck.n_istr));
		_printd(("INFO: ck.n_ostr    = %hu\n", ck.n_ostr));
		_printd(("INFO: ck.l_caps    = %u\n", ck.l_caps));
		_printd(("INFO: ck.p_caps    = %u\n", ck.p_caps));
		_printd(("INFO: ck.l_ali     = %u\n", ck.l_ali));
		_printd(("INFO: ck.p_ali     = %u\n", ck.p_ali));
		_printd(("INFO: ck.danum     = %u\n", ck.danum));
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
	if ((oldsk = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))) {
		sctp_t *oldsp = SCTP_PROT(oldsk);
		/* RFC 2960 5.2.1 and 5.2.2 Note */
		if (oldsk->state != SCTP_COOKIE_WAIT) {
			rare();
			ck.l_ttag = oldsp->v_tag;
			ck.p_ttag = oldsp->p_tag;
		}
		sock_put(oldsk);
	}
	ck.opt_len = 0;
	if (sk->state == SCTP_LISTEN && !(sk->userlocks & SOCK_BINDADDR_LOCK)) {
		/* always include destination address */
		if (!sctp_saddr_include(sp, ck.saddr, &err) && err)
			goto no_resource;
		/* build a temporary address list */
		__sctp_get_addrs(sp, iph->saddr);
	}
	if (sp->sanum < 1)
		goto no_resource;
	ck.sanum = sp->sanum - 1;	/* don't include primary */
	_printd(("INFO: ck.sanum     = %u\n", ck.sanum));
	_printd(("INFO: ck.opt_len   = %u\n", ck.opt_len));
	sctp_send_init_ack(sk, iph->daddr, iph->saddr, sh, &ck, unrec);
      cleanup:
	if (sk->state == SCTP_LISTEN && !(sk->userlocks & SOCK_BINDADDR_LOCK))
		__sctp_free_saddrs(sp);	/* remove temporary address list */
      discard:
	if (unrec)
		kfree_skb(unrec);
	/* SCTP IG 2.2, 2.11 */
	return sctp_return_stop(skb);
      no_resource:
	err = -SCTP_CAUSE_NO_RESOURCE;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: couldn't allocate source addresses\n"));
	goto error;
      bad_address:
	err = -SCTP_CAUSE_BAD_ADDRESS;
	errp = pptr;
	errl = plen;
	ptrace(("PROTO: unsupported address type - hostname or IPv6 address\n"));
	goto error;
      invalid_parm:
	err = -SCTP_CAUSE_INVALID_PARM;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: missing initiate tag or zero streams requested\n"));
	goto error;
      bad_parm:
	err = -SCTP_CAUSE_BAD_PARM;
	errp = pptr;
	errl = plen;
	ptrace(("PROTO: unrecognized or poorly formatted optional parameter\n"));
	goto error;
      error:
	/* SCTP IG 2.23 Abort uses initiate tag as verification tag. */
	sctp_send_abort_error_init(sk, iph->saddr, iph->daddr, sh, m->i_tag, -err, errp, errl);
	goto cleanup;
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
      ebusy:
	err = -EBUSY;
	goto return_error;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
      eproto:
	err = -EPROTO;
	goto return_error;
      emsgsize:
	err = -EMSGSIZE;
	goto return_error;
      enobufs:
	err = -ENOBUFS;
	goto return_error;
      return_error:
	if (unrec)
		kfree_skb(unrec);
	return (err);
}

/*
 *  RECV INIT ACK
 *  -------------------------------------------------------------------------
 *  We have recevied an INIT ACK in the SCTP_COOKIE_WAIT (TS_WCON_CREQ) state.
 *  (RFC 2960 5.2.3)
 */
STATIC int
sctp_recv_init_ack(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err = 0;
	struct iphdr *iph;
	struct sctp_init_ack *m;
	unsigned char *kptr = NULL;
	size_t klen = 0;
	unsigned char *pptr;
	unsigned char *pend;
	size_t plen;
	union sctp_parm *ph;
	printd(("Received INIT-ACK on stream %p\n", sp));
	assert(sp);
	assert(skb);
	if (sk->state != SCTP_COOKIE_WAIT)
		goto eproto;
	iph = SCTP_IPH(skb);
	if (!sctp_daddr_include(sp, iph->saddr, &err))
		goto addr_error;
	m = (typeof(m)) skb->data;
	/* SCTP IG 2.22 */
	if (!m->i_tag)
		goto invalid_parm;
	{
		size_t clen;
		if ((clen = ntohs(m->ch.len)) < sizeof(*m))
			goto emsgsize;
		/* SCTP IG 2.11 not allowed to bundle */
		if (PADC(clen) < skb->len)
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
			INET_ECN_xmit(sk);
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
				sctp_send_error(sk, SCTP_CAUSE_BAD_PARM, pptr, plen);
		}
	}
	if (!kptr)		/* no cookie? */
		goto missing_parm;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->prel == SCTP_PR_REQUIRED && !(sp->p_caps & SCTP_CAPS_PR))
		/* require partial reliabilty support :- abort the init process */
		goto proto_error;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	sctp_ack_calc(sk, &sp->timer_init);
	local_bh_disable();
	if (sp->pprev) {
		swerr();
		__sctp_phash_unhash(sp);
	}
	sp->p_tag = m->i_tag;
	__sctp_phash_insert(sp);
	local_bh_enable();
	sctp_send_cookie_echo(sk, kptr, klen);
	return sctp_return_stop(skb);
      discard:
	__skb_queue_purge(&sp->errq);
	return sctp_return_stop(skb);
      eproto:
	rare();
	err = -EPROTO;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("Bad chunk or parameter length\n"));
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
	ptrace(("PROTO: protocol violation\n"));
	goto disconnect;
#if 0
      no_resource:
	err = SCTP_CAUSE_NO_RESOURCE;
	ptrace(("PROTO: couldn't allocate destination addresses\n"));
	goto disconnect;
#endif
      bad_address:
	err = SCTP_CAUSE_BAD_ADDRESS;
	ptrace(("PROTO: unsupported address type - hostname or IPv6 address\n"));
	goto disconnect;
      invalid_parm:
	err = SCTP_CAUSE_INVALID_PARM;
	ptrace(("PROTO: missing initiate tag or invalid number of streams requested\n"));
	goto disconnect;
      bad_parm:
	err = SCTP_CAUSE_BAD_PARM;
	ptrace(("PROTO: unrecognized or poorly formatted optional parameter\n"));
	goto disconnect;
      missing_parm:
	err = SCTP_CAUSE_MISSING_PARM;
	ptrace(("PROTO: missing mandatory (state cookie) parameter\n"));
	goto disconnect;
      disconnect:
	/* abort the init process */
	sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
	return (-ECONNABORTED);
}

/*
 *  RECV COOKIE ECHO
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ECHO for a socket or stream.  We have already determined the socket or
 *  stream to which the COOKIE ECHO applies.  We must still verify the cookie and apply the
 *  appropriate action per RFC 2960 5.2.4.
 */
STATIC void sctp_reset(struct sock *sk);
STATIC void sctp_clear(struct sock *sk);
STATIC int
sctp_update_from_cookie(struct sock *sk, struct sctp_cookie *ck)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err = 0;
	uint32_t *daddrs = (uint32_t *) (ck + 1);
	uint32_t *saddrs = daddrs + ck->danum;
	if (!(sk->userlocks & SOCK_BINDADDR_LOCK)) {
		if (!sctp_saddr_include(sp, ck->saddr, &err) && err)
			goto error;
		if ((err = sctp_alloc_saddrs(sp, ck->sport, saddrs, ck->sanum)))
			goto error;
	}
	if (!sctp_daddr_include(sp, ck->daddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_daddrs(sp, ck->dport, daddrs, ck->danum)) < 0)
		goto error;
	if ((err = sctp_conn_hash(sk)))
		goto error;
	if ((err = sctp_update_routes(sk, 1)))
		goto error;
#if SOCKETS
	sk->daddr = ck->daddr;
	sk->rcv_saddr = sk->saddr = ck->saddr;
	sk->protinfo.af_inet.opt = NULL;
#endif
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
#if SOCKETS
	sp->a_rwnd = sk->rcvbuf;
#endif
	return (0);
      error:
	return (err);
}
STATIC void sctp_unhash(struct sock *sk);
STATIC int
sctp_recv_cookie_echo(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err;
	struct sctp_cookie_echo *m;
	struct sctp_cookie *ck;
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	static ulong last_cookie_echo = 0;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
	printd(("Received COOKIE-ECHO on socket %p\n", sk));
	assert(sk);
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	if (last_cookie_echo && jiffies < last_cookie_echo + SCTP_PROT(sk)->throttle)
		return (-EBUSY);
	last_cookie_echo = jiffies;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
	assert(skb);
	m = (typeof(m)) skb->data;
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
			    || ((1 << sk->state) & (SCTPF_COOKIE_WAIT | SCTPF_COOKIE_ECHOED)))
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
	switch (sk->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		rare();
		/* RFC 2960 5.2.4 (A) */
		sctp_send_abort_error(sk, SCTP_CAUSE_SHUTDOWN, NULL, 0);
		sctp_send_shutdown_ack(sk);
		return sctp_return_stop(skb);
	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
		rare();
		/* RFC 2960 5.2.4 (A) */
		sctp_send_abort(sk);	/* abort old association */
		if (!sk->dead) {
			sctp_reset(sk);
			sp->v_tag = ck->v_tag;
			sp->p_tag = ck->p_tag;
			if ((err = sctp_update_from_cookie(sk, ck)))
				goto error_abort;
#if SOCKETS
			/* For sockets there is no way of responding the reset, so we autorespond
			   now.  Under STREAMS we can respond to the reset and do not clear the
			   stream state until the response has been received.  This allows
			   retrieval between reset indication and reset response on STREAMS. */
			sctp_clear(sk);
#endif				/* SOCKETS */
			sctp_change_state(sk, SCTP_ESTABLISHED);
			/* Notify user of reset or disconnect */
			if ((err = sctp_reset_ind(sk, SCTP_ORIG_USER, -ECONNRESET, skb)))
				return (err);
			/* Yes, data (and other chunks) can be bundled with COOKIE-ECHO.  This
			   processes them. */
			if (sctp_return_more(skb) > 0)
				sctp_recv_msg(sk, skb);
			/* Wait to send COOKIE-ACK until we've processed the reset of the packet:
			   there might be some other things to bundle with the COOKIE-ACK. */
			sctp_send_cookie_ack(sk, skb);
			return (0);
		}
		sctp_abort(sk, SCTP_ORIG_PROVIDER, -ECONNRESET);
		return (-ECONNRESET);
	default:
		rare();
		return (-EFAULT);
	}
	never();
      recv_cookie_echo_action_b:
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
	switch (sk->state) {
	case SCTP_COOKIE_WAIT:
		if (sctp_timeout_pending(&sp->timer_init))
			sp_del_timeout(sp, &sp->timer_init);
		/* fall through */
	case SCTP_COOKIE_ECHOED:
		if (sctp_timeout_pending(&sp->timer_cookie))
			sp_del_timeout(sp, &sp->timer_cookie);
		/* We haven't got an INIT ACK yet so we need some stuff from the cookie. */
		sctp_unhash(sk);
		if ((err = sctp_update_from_cookie(sk, ck)))
			goto error_abort;
		/* fall through */
	case SCTP_ESTABLISHED:
		break;
	default:
		rare();
		return (-EFAULT);
	}
	local_bh_disable();
	if (sp->pprev)
		__sctp_phash_unhash(sp);
	sp->p_tag = ck->p_tag;
	__sctp_phash_insert(sp);
	local_bh_enable();
	sctp_send_sack(sk);
	goto recv_cookie_echo_action_d;
      recv_cookie_echo_action_c:
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
	/* 
	 *  RFC 2960 5.2.4 Action (D)
	 *
	 *  When both local and remote tags match the endpoint should always enter the ESTABLISHED
	 *  state, it if has not already done so.  It should stop any init or cookie timers that may
	 *  be running and send a COOKIE ACK.
	 */
	/* SCTP IG 2.29 */
	switch (sk->state) {
	case SCTP_COOKIE_WAIT:
		if (sctp_timeout_pending(&sp->timer_init))
			sp_del_timeout(sp, &sp->timer_init);
		/* fall through */
	case SCTP_COOKIE_ECHOED:
		if (sctp_timeout_pending(&sp->timer_cookie))
			sp_del_timeout(sp, &sp->timer_cookie);
		/* will change state to established */
		if ((err = sctp_conn_con(sk)))
			return (err);
		/* fall through */
	case SCTP_ESTABLISHED:
		/* process data bundled with cookie echo */
		if (sctp_return_more(skb) > 0)
			sctp_recv_msg(sk, skb);
		sctp_send_cookie_ack(sk, skb);
		return (0);
	default:
		rare();
		/* RFC 2960 5.2.4 ...silently discarded */
		return (0);
	}
	never();
      recv_cookie_echo_listen:
	if (sk->max_ack_backlog) {
		struct sk_buff *cp;
		/* check for existing connection indication */
		for (cp = skb_peek(&sp->conq); cp; cp = skb_next(cp)) {
			struct sctp_cookie_echo *ce = (typeof(ce)) cp->data;
			struct sctp_cookie *co = (typeof(co)) ce->cookie;
			/* FIXME: we should check a little harder for cookie echo matches. */
			if (co->v_tag == ck->v_tag
			    || (co->dport == ck->dport && co->sport == ck->sport
				&& co->daddr == ck->daddr && co->saddr == ck->saddr)) {
				printd(("INFO: Discarding multiple COOKIE-ECHO\n"));
				return (0);	/* discard multiple */
			}
		}
		/* RFC 2960 5.2.4 (4) */
		if (sk->ack_backlog >= sk->max_ack_backlog)
			goto no_resource;
		sctp_conn_ind(sk, skb);
		return (0);
	}
	return (0);
      recv_cookie_echo_action_p:
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
	switch (sk->state) {
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
	return (0);
      error_abort:
	{
		struct iphdr *iph = SCTP_IPH(skb);
		struct sctphdr *sh = SCTP_SH(skb);
		/* We had a fatal error processing the COOKIE ECHO and must abort the association. */
		sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
		return (err);
	}
      emsgsize:
	err = -EMSGSIZE;
	return (err);
      stale_cookie:
	{
		uint32_t staleness;
		struct iphdr *iph = SCTP_IPH(skb);
		struct sctphdr *sh = SCTP_SH(skb);
		rare();
		staleness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
      no_resource:
	{
		struct iphdr *iph = SCTP_IPH(skb);
		struct sctphdr *sh = SCTP_SH(skb);
		seldom();
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
					   SCTP_CAUSE_NO_RESOURCE, NULL, 0);
		NET_INC_STATS_BH(ListenOverflows);
		return (0);
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
sctp_recv_cookie_ack(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err;
	struct sctp_daddr *sd;
	struct sctp_cookie_ack *m = (typeof(m)) skb->data;
	printd(("Received COOKIE-ACK on socket %p\n", sk));
	assert(sk);
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	switch (sk->state) {
	case SCTP_COOKIE_ECHOED:
		/* RFC 2960 5.1 (E) */
		/* will change state to established */
		if ((err = sctp_conn_con(sk)))
			return (err);
		sctp_ack_calc(sk, &sp->timer_cookie);
		SCTP_INC_STATS_BH(SctpActiveEstabs);
		/* start idle timers */
		_usual(sp->daddr);
		for (sd = sp->daddr; sd; sd = sd->next)
			sctp_reset_idle(sd);
		return sctp_return_more(skb);
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
sctp_recv_heartbeat(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_heartbeat *m = (typeof(m)) skb->data;
	struct sctpphdr *ph = (typeof(ph)) (m + 1);
	size_t clen = ntohs(m->ch.len);
	size_t mlen = sizeof(*m) + sizeof(*ph);
	size_t hlen;
	printd(("Received HEARTBEAT on socket %p\n", sk));
	if (clen < mlen || skb->len < mlen)
		goto emsgsize;
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
	{
		sctp_t *sp = SCTP_PROT(sk);
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
	sctp_send_heartbeat_ack(sk, (caddr_t) ph, min(PADC(clen), hlen));
	return sctp_return_stop(skb);
      eproto:
	err = -EPROTO;
	ptrace(("PROTO: bad message\n"));
	goto error;
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
      ebusy:
	err = -EBUSY;
	ptrace(("WARNING: throttling heartbeat\n"));
	goto error;
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("PROTO: bad message size\n"));
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
sctp_recv_heartbeat_ack(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err;
	struct sctp_daddr *sd;
	struct sctp_heartbeat_ack *m = (typeof(m)) skb->data;
	struct sctp_heartbeat_info *hb = (typeof(hb)) (m + 1);
	printd(("Received HEARTBEAT-ACK on socket %p\n", sk));
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
	sd_del_timeout(sd, &sd->timer_heartbeat);
	sctp_rtt_calc(sd, sd->hb_time);
      ignore:
	return sctp_return_stop(skb);
      einval:
	err = -EINVAL;
	ptrace(("PROTO: old or fiddled timestamp\n"));
	goto ignore;
      eproto:
	err = -EPROTO;
	ptrace(("PROTO: bad heartbeat parameter type\n"));
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("PROTO: bad heartbeat parameter or chunk size\n"));
	goto error;
      badaddr:
	ptrace(("PROTO: bad destination parameter\n"));
	goto ignore;
      error:
	return (err);
}

/*
 *  RECV SHUTDOWN
 *  -------------------------------------------------------------------------
 */
STATIC void
sctp_shutdown_ack_calc(struct sock *sk, uint32_t ack)
{
	sctp_t *sp = SCTP_PROT(sk);
	if (before(ack, sp->t_ack)) {
		rare();
		/* If the SHUTDOWN acknowledges our sent data chunks that have already been
		   acknowledged, then it is an old (or erroneous) message and we will ignore it. */
		return;
	}
	if (after(ack, sp->t_ack))
		sctp_cumm_ack(sk, ack);
	sctp_dest_calc(sk);
}
STATIC int
sctp_recv_shutdown(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err;
	struct sctp_shutdown *m = (typeof(m)) skb->data;
	uint32_t ack = ntohl(m->c_tsn);
	printd(("Received SHUTDOWN on socket %p\n", sk));
	assert(sk);
	if ((1 << sk->state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	if (sp->ngaps) {
		rare();
		/* Check sanity of sender: if we have gaps in our acks to the peer and the peer
		   sends a SHUTDOWN, then it is in error. The peer cannot send SHUTDOWN when it has 
		   unacknowledged data. If this is the case, we zap the connection. */
		sctp_send_abort_error(sk, SCTP_CAUSE_PROTO_VIOLATION, NULL, 0);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, -EPIPE);
		return sctp_return_stop(skb);
	}
	switch (sk->state) {
	case SCTP_ESTABLISHED:
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sk)))
			goto error;
		sctp_change_state(sk, SCTP_SHUTDOWN_RECEIVED);
		/* fall thru */
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_shutdown_ack_calc(sk, ack);
		if (!sk->dead)
			break;
		/* fall thru */
	case SCTP_SHUTDOWN_PENDING:	/* only when we have ordrel */
		sctp_change_state(sk, SCTP_SHUTDOWN_RECVWAIT);
		/* fall thru */
	case SCTP_SHUTDOWN_RECVWAIT:
		sctp_shutdown_ack_calc(sk, ack);
		if (atomic_read(&sk->wmem_alloc) <= 0)
			sctp_send_shutdown_ack(sk);
		break;
	case SCTP_SHUTDOWN_SENT:	/* only when we have ordrel */
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sk)))
			goto error;
		sctp_shutdown_ack_calc(sk, ack);
		/* fail thru */
	case SCTP_SHUTDOWN_ACK_SENT:
		sctp_send_shutdown_ack(sk);	/* We do this */
		break;
	default:
	      outstate:
		/* ignore the SHUTDOWN chunk */
		rare();
		break;
	}
	return sctp_return_more(skb);
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
sctp_recv_shutdown_ack(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_shutdown_ack *m = (typeof(m)) skb->data;
	printd(("Received SHUTDOWN-ACK on socket %p\n", sk));
	assert(sk);
	assert(skb);
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	switch (sk->state) {
	case SCTP_SHUTDOWN_SENT:
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sk)))
			goto error;
		/* fall thru */
	case SCTP_SHUTDOWN_ACK_SENT:
#if 0
		sctp_ack_calc(sk, &SCTP_PROT(sk)->timer_shutdown);	/* WHY? */
#endif
		sctp_send_shutdown_complete(sk);
		SCTP_INC_STATS_BH(SctpShutdowns);
		sctp_reset(sk);
		break;
	default:
		/* ignore unexpected SHUTDOWN ACK */
		rare();
		break;
	}
	return sctp_return_stop(skb);
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
sctp_recv_shutdown_complete(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_shutdown_comp *m = (typeof(m)) skb->data;
	printd(("Received SHUTDOWN-COMPLETE on socket %p\n", sk));
	assert(sk);
	assert(skb);
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	switch (sk->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
#if 0
		sctp_ack_calc(sk, &SCTP_PROT(sk)->timer_shutdown);	/* WHY? */
#endif
		SCTP_INC_STATS_BH(SctpShutdowns);
		sctp_reset(sk);
		break;
	default:
		/* ignore unexpected SHUTDOWN COMPLETE */
		rare();
		break;
	}
	return sctp_return_stop(skb);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV UNRECOGNIZED CHUNK TYPE
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_unrec_ctype(struct sock *sk, struct sk_buff *skb)
{
	struct sctpchdr *ch = SCTP_CH(skb);
	uint8_t ctype;
	printd(("Received unrecognized chunk type on socket %p\n", sk));
	if (ntohs(ch->len) < sizeof(*ch))
		goto emsgsize;
	ctype = ch->type;
	if (ctype & SCTP_CTYPE_MASK_REPORT)
		sctp_send_error(sk, SCTP_CAUSE_BAD_CHUNK_TYPE, skb->data, skb->len);
	if (ctype & SCTP_CTYPE_MASK_CONTINUE)
		return sctp_return_more(skb);
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
sctp_recv_ecne(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_ecne *m = (typeof(m)) skb->data;
	uint32_t l_tsn;
	struct sctp_daddr *sd;
	printd(("Received ECNE on socket %p\n", sk));
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	l_tsn = ntohl(m->l_tsn);
	sp->p_caps |= SCTP_CAPS_ECN;
	INET_ECN_xmit(sk);
	if (!after(l_tsn, sp->p_lsn))
		goto done;
	sp->p_lsn = l_tsn;
	/* need to find the destination to which this TSN was transmitted */
	for (skb = skb_peek(&sp->rtxq); skb && SCTP_SKB_CB(skb)->tsn != l_tsn;
	     skb = skb_next(skb)) ;
	if (!skb)
		goto done;
	if (!(sd = SCTP_SKB_CB(skb)->daddr))
		goto done;
	sctp_assoc_timedout(sk, sd, 0, 0);
      done:
	return sctp_return_more(skb);
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
sctp_recv_cwr(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_cwr *m = (typeof(m)) skb->data;
	uint32_t l_tsn;
	printd(("Received CWR on socket %p\n", sk));
	(void) l_tsn;
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	l_tsn = ntohl(m->l_tsn);
	fixme(("FIXME: Finish this function\n"));
	sp->sackf &= ~SCTP_SACKF_ECN;
	return sctp_return_more(skb);
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
sctp_recv_asconf(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_asconf *m = (typeof(m)) skb->data;
	uint32_t asn;
	struct sctp_daddr *sd;
	int reconfig = 0;
	printd(("Received ASCONF on socket %p\n", sk));
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	if (!(sp->l_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto refuse;
	/* ADD-IP 4.1.1 R4 */
	if (sk->state != SCTP_ESTABLISHED)
		goto discard;
	asn = ntohl(m->asn);
	if (skb->len < sizeof(*m))
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
				if (a->addr != SCTP_IPH(skb)->saddr && sp->danum != 1) {
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
					sctp_send_error(sk, SCTP_CAUSE_BAD_PARM, (caddr_t) ph,
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
					sr = ((typeof(sr)) bptr)++;
					sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
					sr->ph.len = __constant_htons(sizeof(*sr));
					sr->cid = ph->add_ip.id;
					rlen += sizeof(*sr);
					continue;
				} else {
					er = ((typeof(er)) bptr)++;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh) + plen);
					er->cid = ph->del_ip.id;
					eh = ((typeof(eh)) bptr)++;
					eh->code = __constant_htons(SCTP_CAUSE_RES_SHORTAGE);
					eh->len = htons(sizeof(*eh) + plen);
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
					sr = ((typeof(sr)) bptr)++;
					sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
					sr->ph.len = __constant_htons(sizeof(*sr));
					sr->cid = ph->add_ip.id;
					rlen += sizeof(*sr);
					continue;
				}
				if (a->addr == SCTP_IPH(skb)->saddr) {
					/* request to delete source address */
					er = ((typeof(er)) bptr)++;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh) + plen);
					er->cid = ph->del_ip.id;
					eh = ((typeof(eh)) bptr)++;
					eh->code = __constant_htons(SCTP_CAUSE_SOURCE_ADDR);
					eh->len = htons(sizeof(*eh) + plen);
					bcopy(pptr, bptr, plen);
					bptr += plen;
					rlen += sizeof(*er) + sizeof(*eh) + plen;
					continue;
				}
				if (sp->danum == 1) {
					/* last address need to send error */
					er = ((typeof(er)) bptr)++;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh)
							   + plen);
					er->cid = ph->del_ip.id;
					eh = ((typeof(eh)) bptr)++;
					eh->code = __constant_htons(SCTP_CAUSE_LAST_ADDR);
					eh->len = htons(sizeof(*eh) + plen);
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
				sr = ((typeof(sr)) bptr)++;
				sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
				sr->ph.len = __constant_htons(sizeof(*sr));
				sr->cid = ph->add_ip.id;
				rlen += sizeof(*sr);
				continue;
			}
		}
		sp->p_caps |= SCTP_CAPS_ADD_IP;
		sctp_send_asconf_ack(sk, rptr, rlen);
		/* update routes now if a configuration change occured */
		if (reconfig)
			sctp_update_routes(sk, 1);
	} else if (asn == sp->p_asn) {
		/* ADD-IP 4.2 Rule C1 & C3 */
		/* give the same reply as before */
		if (sp->reply) {
			if (!(sd = sctp_route_response(sk)))
				goto noroute;
			printd(("Sending ASCONF-ACK from socket %p\n", sk));
			sctp_send_msg(sk, sd, sp->reply);
		}
	}
      discard:
	/* ADD-IP 4.2 Rule C1 & C4, R4 */
	return sctp_return_more(skb);
      noroute:
	ptrace(("ERROR: no route to peer\n"));
	return -EHOSTUNREACH;
      emsgsize:
	ptrace(("ERROR: bad message size\n"));
	return -EMSGSIZE;
      refuse:
	return sctp_recv_unrec_ctype(sk, skb);
}

/*
 *  RECV ASCONF ACK
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_asconf_ack(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_asconf_ack *m = (typeof(m)) skb->data;
	uint32_t asn;
	printd(("Received ASCONF-ACK on socket %p\n", sk));
	if (ntohs(m->ch.len) < sizeof(*m))
		goto emsgsize;
	/* ADD-IP 4.1.1 R4 */
	if (sk->state != SCTP_ESTABLISHED)
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
	sctp_ack_calc(sk, &sp->timer_asconf);
	/* process the ASCONF ACK */
	{
		union sctp_parm *ph;
		unsigned char *pptr = (unsigned char *) (m + 1);
		unsigned char *pend = pptr + ntohs(m->ch.len) - sizeof(*m);
		size_t plen;
		struct sctp_saddr *ss, *s2;
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
				for (ss = sp->saddr; ss && ss != s2; ss = ss->next) ;
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
				for (ss = sp->saddr; ss && ss != s2; ss = ss->next) ;
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
					sctp_send_error(sk, SCTP_CAUSE_BAD_PARM, (caddr_t) ph,
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
	return sctp_return_more(skb);
      abort:
	/* ADD-IP 4.3 D0 */
	/* abort the association with reason INVALID ASCONF ACK */
	sctp_send_abort_error(sk, SCTP_CAUSE_ILLEGAL_ASCONF, NULL, 0);
	sctp_abort(sk, SCTP_ORIG_PROVIDER, -ECONNABORTED);
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
sctp_recv_forward_tsn(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_forward_tsn *m = (typeof(m)) skb->data;
	uint32_t f_tsn;
	size_t nstrs, clen = ntohs(m->ch.len);
	printd(("Received FORWARD-TSN on socket %p\n", sk));
	if ((1 << sk->state) & ~(SCTPF_RECEIVING))
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
		sctp_send_sack(sk);
	} else {
		/* need to clean up any gaps */
		sctp_deliver_data(sk, skb);
	}
	return sctp_return_more(skb);
      sctp_recv_fsn_duplicate:
	/* FSN is duplicate. */
	/* TODO: Here we should peg the duplicate against the last destination to which we sent a
	   SACK.  Receiving duplicate FSNs is a weak indication that our SACKs might not be getting 
	   through. */
      outstate:
	return sctp_return_more(skb);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

/*
 *  RECV SCTP MESSAGE
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_rcv_ootb(struct sk_buff *skb);
STATIC int
sctp_recv_msg(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err = skb->len;
	struct sctpchdr *ch;
	ensure(sk, goto efault);
	ensure(skb, goto efault);
	printd(("INFO: %s: received skb = 0x%p\n", __FUNCTION__, skb));
	/* set address for reply chunks */
	if (sk->state != SCTP_LISTEN && !(sp->caddr = sctp_find_daddr(sp, SCTP_IPH(skb)->saddr))) {
		ch = SCTP_CH(skb);
		switch (ch->type) {
		case SCTP_CTYPE_INIT:
		case SCTP_CTYPE_INIT_ACK:
		case SCTP_CTYPE_COOKIE_ECHO:
			break;
		default:
			SCTP_INC_STATS_BH(SctpOutOfBlues);
			return sctp_rcv_ootb(skb);
		}
	}
	do {
		size_t clen;
		uint8_t type;
		ch = SCTP_CH(skb);
		if (sizeof(*ch) > skb->len || (clen = ntohs(ch->len)) < sizeof(*ch)
		    || PADC(clen) > skb->len)
			goto emsgsize;
		if ((type = ch->type) == SCTP_CTYPE_DATA) {
			err = sctp_recv_data(sk, skb);
		} else {
			SCTP_INC_STATS(SctpInCtrlChunks);
			switch (type) {
			case SCTP_CTYPE_INIT:
				err = sctp_recv_init(sk, skb);
				break;
			case SCTP_CTYPE_INIT_ACK:
				err = sctp_recv_init_ack(sk, skb);
				break;
			case SCTP_CTYPE_SACK:
				err = sctp_recv_sack(sk, skb);
				break;
			case SCTP_CTYPE_HEARTBEAT:
				err = sctp_recv_heartbeat(sk, skb);
				break;
			case SCTP_CTYPE_HEARTBEAT_ACK:
				err = sctp_recv_heartbeat_ack(sk, skb);
				break;
			case SCTP_CTYPE_ABORT:
				err = sctp_recv_abort(sk, skb);
				break;
			case SCTP_CTYPE_SHUTDOWN:
				err = sctp_recv_shutdown(sk, skb);
				break;
			case SCTP_CTYPE_SHUTDOWN_ACK:
				err = sctp_recv_shutdown_ack(sk, skb);
				break;
			case SCTP_CTYPE_ERROR:
				err = sctp_recv_error(sk, skb);
				break;
			case SCTP_CTYPE_COOKIE_ECHO:
				err = sctp_recv_cookie_echo(sk, skb);
				break;
			case SCTP_CTYPE_COOKIE_ACK:
				err = sctp_recv_cookie_ack(sk, skb);
				break;
#ifdef SCTP_CONFIG_ECN
			case SCTP_CTYPE_ECNE:
				err = sctp_recv_ecne(sk, skb);
				break;
			case SCTP_CTYPE_CWR:
				err = sctp_recv_cwr(sk, skb);
				break;
#endif				/* SCTP_CONFIG_ECN */
			case SCTP_CTYPE_SHUTDOWN_COMPLETE:
				err = sctp_recv_shutdown_complete(sk, skb);
				break;
#ifdef SCTP_CONFIG_ADD_IP
			case SCTP_CTYPE_ASCONF:
				err = sctp_recv_asconf(sk, skb);
				break;
			case SCTP_CTYPE_ASCONF_ACK:
				err = sctp_recv_asconf_ack(sk, skb);
				break;
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			case SCTP_CTYPE_FORWARD_TSN:
				err = sctp_recv_forward_tsn(sk, skb);
				break;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
			default:
				err = sctp_recv_unrec_ctype(sk, skb);
				break;
			}
		}
	} while (err >= sizeof(struct sctpchdr) && skb->len >= sizeof(struct sctpchdr));
	assure(err <= 0 || skb->len == 0);
	if (err < 0)
		goto error;
      done:
	/* NOTE: For sockets we always free the skb here.  If the receive function called wishes to 
	   keep a copy it whould either clone the skb or do an skb_get() to increase the reference
	   count.  For streams we never free the mblk at this point because it could be returned to 
	   the queue because of congesion, lack of buffers or other reasons. */
	kfree_skb(skb);
	return (err);
	goto efault;
      efault:
	err = -EFAULT;
	swerr();
	goto done;
#if 0
      enomem:
	err = -ENOBUFS;
	_ptrace(("ERROR: no buffers for msgb handling (dropping)\n"));
	goto done;
#endif
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("ERROR: message too short (dropping)\n"));
	goto done;
      error:
	ptrace(("sk = %x, Error %d returned.\n", (uint) sk, err));
	/* NOTE: There are some errors that are returned by the receive functions that are not
	   handled by those function but are handled here.  These are exceptional error conditions. 
	 */
	switch (err) {
	case -ENOMEM:
	case -ENOBUFS:
	case -EBUSY:
		/* These are resource problems */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sk, SCTP_CAUSE_RES_SHORTAGE, NULL, 0);
		sctp_discon_ind(sk, SCTP_ORIG_PROVIDER, -ECONNABORTED, NULL);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, -ECONNABORTED);
		break;
	case -EPROTO:
		/* This is a protocol violation */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sk, SCTP_CAUSE_PROTO_VIOLATION, NULL, 0);
		sctp_discon_ind(sk, SCTP_ORIG_PROVIDER, err, NULL);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
		break;
	case -EINVAL:
		/* This is an invalid parameter */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sk, SCTP_CAUSE_INVALID_PARM, NULL, 0);
		sctp_discon_ind(sk, SCTP_ORIG_PROVIDER, err, NULL);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
		break;
	case -EMSGSIZE:
		/* This is a message formatting error */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort(sk);
		sctp_discon_ind(sk, SCTP_ORIG_PROVIDER, err, NULL);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, err);
		break;
	default:
		/* ignore others handled specially inside receive functions */
		goto done;
	}
	goto done;
}

/*
 *  RECV SCTP ICMP ERROR
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_err(struct sock *sk, struct sk_buff *skb)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct iphdr *iph = (struct iphdr *) skb->data;
	struct sctp_daddr *sd;
	uint32_t daddr = iph->daddr;
	struct icmphdr *icmph = skb->h.icmph;
	int type = icmph->type;
	int code = icmph->code;
	int err = 0;
	if (sk->state == SCTP_CLOSED)
		goto closed;
	sd = sctp_find_daddr(sp, daddr);
	switch (type) {
	case ICMP_SOURCE_QUENCH:
		if (!sd)
			goto done;
		/* Sockets does this directly from the bottom half, streams does not. */
#if SOCKETS
		if (sock_locked(sk))
			goto lock_dropped;
#endif				/* SOCKETS */
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
		if (code == ICMP_FRAG_NEEDED) {
#if SOCKETS
			if (sock_locked(sk))
				goto lock_dropped;
#endif				/* SOCKETS */
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
		err = icmp_err_convert[code].errno;
		break;
	case ICMP_PARAMETERPROB:
		err = EPROTO;
		break;
	case ICMP_TIME_EXCEEDED:
		err = EHOSTUNREACH;
		break;
	default:
		goto done;
	}
	if (sk->state == SCTP_LISTEN)
		/* NOTE: Unlike TCP, we do not have partially formed sockets in the accept queue,
		   so this ICMP error should have gone to the established socket in the accept
		   queue that sent the COOKIE-ACK that generated the error.  Otherwise, it is for
		   an INIT-ACK that can't get to its destination, so we don't care, just ignore it. 
		 */
		goto listening;
	sctp_error_report(sk, err);
	if (sd && sd->dst_cache)
		dst_negative_advice(&sd->dst_cache);
	goto done;
      done:
	return (0);
      listening:
	ptrace(("ERROR: ICMP for listening socket\n"));
	goto done;
      closed:
	ptrace(("ERROR: ICMP for closed socket\n"));
	goto done;
#if SOCKETS
      lock_dropped:
	ptrace(("ERROR: Hit locks on socket\n"));
	NET_INC_STATS_BH(LockDroppedIcmps);
	goto done;
#endif				/* SOCKETS */
}

/*
 *  ==========================================================================
 *
 *  SOCKET PROTOCOL INTERFACE
 *
 *  ==========================================================================
 *
 *  MAJOR INET FUNCTIONS:
 *
 *  --------------------------------------------------------------------------
 *
 *  SCTP POLL
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Wait for an SCTP event, ala TCP.
 *
 */
STATIC unsigned int
sctp_poll(struct file *file, struct socket *sock, poll_table *wait)
{
	struct sock *sk = sock->sk;
	sctp_t *sp = SCTP_PROT(sk);
	unsigned int mask = 0;
	poll_wait(file, sk->sleep, wait);
	if (sk->state == SCTP_LISTEN) {
		if (!skb_queue_empty(&sp->conq))
			mask |= POLLIN | POLLRDNORM;
		return mask;
	}
	if (sk->err)
		mask = POLLERR;
	if (sk->shutdown == SHUTDOWN_MASK && sk->state == SCTP_CLOSED)
		mask |= POLLHUP;
	if (sk->shutdown & RCV_SHUTDOWN)
		mask |= POLLIN | POLLRDNORM;
	if ((1 << sk->state) & ~(SCTPF_OPENING)) {
		if (!skb_queue_empty(&sk->rcvq)
		    || (sk->urginline && !skb_queue_empty(&sp->expq)))
			mask |= POLLIN | POLLRDNORM;
		if (!(sk->shutdown & SEND_SHUTDOWN)) {
			if (sctp_wspace(sk) >= sctp_min_wspace(sk))
				mask |= POLLOUT | POLLWRNORM;
			else {
				set_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);
				set_bit(SOCK_NOSPACE, &sk->socket->flags);
				if (sctp_wspace(sk) >= sctp_min_wspace(sk))
					mask |= POLLOUT | POLLWRNORM;
			}
		}
		if (!skb_queue_empty(&sp->expq))
			mask |= POLLPRI;
	}
	return mask;
}

/*
 *  SCTP LISTEN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Place the socket into the listen hashes, make sure to rebind the protocol
 *  number for listening and recheck for conflicts.  If there socket is
 *  already in the listening state, just adjust the backlog.
 */
STATIC int
sctp_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	unsigned char old_state;
	int err;
	lock_sock(sk);
	err = -EINVAL;
	if (sock->state != SS_UNCONNECTED || (
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
						     sock->type != SOCK_STREAM &&
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
						     sock->type != SOCK_SEQPACKET))
		goto out;
	old_state = sk->state;
	if (!(1 << old_state) & (SCTPF_CLOSED | SCTPF_LISTEN))
		goto out;
	if (old_state != SCTP_LISTEN) {
		sctp_change_state(sk, SCTP_LISTEN);
		if (sk->prot->get_port(sk, sk->num))
			goto eaddrinuse;
		sk->sport = htons(sk->num);
		sk->prot->hash(sk);
		sk->ack_backlog = 0;
	}
	sk->max_ack_backlog = (backlog < SOMAXCONN) ? backlog : SOMAXCONN;
	err = 0;
      out:
	release_sock(sk);
	return (err);
      eaddrinuse:
	sctp_change_state(sk, SCTP_CLOSED);
	err = -EADDRINUSE;
	goto out;
}

/*
 *  SCTP CLEAR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Clear the connection information hanging around on a socket or stream.  This includes any queued
 *  data blocks that are waiting around for retrieval.  It is OK to call this function twice in a
 *  row on the same socket or stream.
 *
 *  The objective of sctp_clear() is to remove any data remaining for retrieval before the socket or
 *  stream is reused or stopped.
 */
STATIC void
sctp_clear(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	ptrace(("Clearing socket sk=%p, state = %d\n", sk, sk->state));
	/* clear flags */
	sk->shutdown = 0;
	sk->done = 0;
	/* purge queues */
	__skb_queue_purge(&sp->expq);
	__skb_queue_purge(&sk->rcvq);
	sp->rmem_queued = 0;
	__skb_queue_purge(&sp->urgq);
	__skb_queue_purge(&sk->sndq);
	sk->wmem_queued = 0;
	__skb_queue_purge(&sp->errq);
	__skb_queue_purge(&sp->oooq);
	sp->gaps = NULL;
	sp->ngaps = 0;
	sp->nunds = 0;
	__skb_queue_purge(&sp->dupq);
	sp->dups = NULL;
	sp->ndups = 0;
	__skb_queue_purge(&sp->rtxq);
	sp->nrtxs = 0;
	sp->nsack = 0;
	__skb_queue_purge(&sp->ackq);
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
sctp_unhash(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	ptrace(("Unhashing socket sk=%p, state = %d\n", sk, sk->state));
	local_bh_disable();
	if (sk->pprev) {
		if (sp->vprev)
			__sctp_vhash_unhash(sp);
		sp->v_tag = 0;
		if (sp->pprev)
			__sctp_phash_unhash(sp);
		sp->p_tag = 0;
		if (sp->tprev)
			__sctp_thash_unhash(sp);
		if (sp->lprev) {
			__sctp_lhash_unhash(sp);
			sk->max_ack_backlog = 0;
		}
		if (sp->daddr)
			__sctp_free_daddrs(sp);
		if (sp->bprev && !(sk->userlocks & SOCK_BINDPORT_LOCK))
			__sctp_bhash_unhash(sk);
		if (sp->saddr && !(sk->userlocks & SOCK_BINDADDR_LOCK))
			__sctp_free_saddrs(sp);
	} else
		swerr();
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
sctp_reset(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	ptrace(("Resetting socket sk=%p, state = %d\n", sk, sk->state));
	local_bh_disable();
	/* unhash and delete address lists */
	sctp_change_state(sk, SCTP_CLOSED);
	sctp_unhash(sk);
	/* stop timers */
	sp_del_timeout(sp, &sp->timer_init);
	sp_del_timeout(sp, &sp->timer_cookie);
	sp_del_timeout(sp, &sp->timer_shutdown);
	sp_del_timeout(sp, &sp->timer_guard);
	sp_del_timeout(sp, &sp->timer_sack);
#ifdef SCTP_CONFIG_ADD_IP
	sp_del_timeout(sp, &sp->timer_asconf);
#endif				/* SCTP_CONFIG_ADD_IP */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	sp_del_timeout(sp, &sp->timer_life);
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
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
	sp->p_rbuf = 0;
#if SOCKETS
	/* FIXME: this is probably not the right thing to do... */
	sp->a_rwnd = sk->rcvbuf;
#endif				/* SOCKETS */
#if 0
	/* clear routing cache */
	__sk_dst_reset(sk);
#endif
	sk->route_caps = 0;
	sp->pmtu = ip_rt_min_pmtu;
	sp->amps = sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
#if 1
	/* decide what to keep for retrieval */
	switch (sp->disp) {
	case SCTP_DISPOSITION_NONE:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq))) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		while ((skb = __skb_dequeue(&sp->urgq))) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		while ((skb = __skb_dequeue(&sk->sndq))) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		break;
	}
	case SCTP_DISPOSITION_UNSENT:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq))) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		while ((skb = __skb_dequeue(&sp->urgq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sk->sndq)))
			__skb_queue_tail(&sp->ackq, skb);
		break;
	}
	case SCTP_DISPOSITION_SENT:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq))) {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else if (cb->flags & SCTPCB_FLAG_SACKED || cb->flags & SCTPCB_FLAG_DROPPED)
				kfree_skb(skb);
			else
				__skb_queue_tail(&sp->ackq, skb);
		}
		while ((skb = __skb_dequeue(&sp->urgq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sk->sndq)))
			__skb_queue_tail(&sp->ackq, skb);
		break;
	}
	case SCTP_DISPOSITION_GAP_ACKED:
	case SCTP_DISPOSITION_ACKED:
	default:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sp->urgq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sk->sndq)))
			__skb_queue_tail(&sp->ackq, skb);
		break;
	}
	}
#else				/* SOCKETS */
	__skb_queue_purge(&sp->urgq);
	__skb_queue_purge(&sk->sndq);
	__skb_queue_purge(&sp->rtxq);
#endif				/* SOCKETS */
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
STATIC void sctp_destroy_orphan(struct sock *sk);
STATIC INLINE int
sctp_abort(struct sock *sk, ulong origin, long reason)
{
	sctp_t *sp = SCTP_PROT(sk);
	printd(("INFO: Disconnect indication on socket %p\n", sk));
	assert(sk);
	assure(origin && reason);
	if (sk->state == SCTP_LISTEN)
		goto outstate;
	sctp_reset(sk);
	{
		if ((sk->err = -reason) && !sk->dead)
			sk->error_report(sk);
		sk->shutdown = SHUTDOWN_MASK;
		sp->sackf = 0;
		if (!sk->dead)
			sk->state_change(sk);
		else
			sctp_destroy_orphan(sk);
	}
	return (0);
      outstate:
	swerr();
	return (-EFAULT);
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
STATIC int sctp_get_port(struct sock *sk, unsigned short port);
STATIC int
sctp_conn_req(struct sock *sk, uint16_t dport, struct sockaddr_in *dsin, size_t dnum,
	      struct sk_buff *dp)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err;
	unsigned short num = ntohs(dport);
	printd(("%p: X_CONN_REQ <- \n", sk));
	assert(sk);
	if (!dnum)
		goto einval;
	if (dsin->sin_family != AF_INET && dsin->sin_family)
		goto eafnosupport;
	if (sk->state != SCTP_CLOSED)
		goto eisconn;
	if (!num)
		goto eaddrnotavail;
#if STREAMS
	if (num < PROT_SOCK && sp->cred.cr_uid != 0)
		goto eacces;
#endif
	if ((err = sctp_alloc_sock_daddrs(sp, dport, dsin, dnum)) < 0)
		goto error;
	if (err == 0)
		goto eaddrnotavail;
	if (!sp->sanum || !sp->saddr) {
		if (!(sk->userlocks & SOCK_BINDADDR_LOCK)) {
			if (sctp_get_addrs(sp, sp->daddr->daddr))
				goto eaddrnotavail;
		} else {
			swerr();
			goto eaddrnotavail;
		}
	}
	if (!sp->bprev) {
		if (!(sk->userlocks & SOCK_BINDPORT_LOCK)) {
			if (sctp_get_port(sk, 0))
				goto eaddrnotavail;
			sk->sport = htons(sk->num);
		} else {
			swerr();
			goto eaddrnotavail;
		}
	}
	sk->dport = dport;
	sk->daddr = sp->daddr->daddr;
	sk->saddr = sp->saddr->saddr;
	/* obtain a verification tag */
	sp->v_tag = sctp_get_vtag(sk->saddr, sk->daddr, sk->sport, sk->dport);
	sp->p_tag = 0;
	printd(("%s: %p: INFO: Assigned verification tag %08X\n", "sctp", sk, sp->v_tag));
	sp->inet.id = sp->v_tag ^ jiffies;
	sp->ext_header_len = 0;
	if ((err = sctp_update_routes(sk, 1)))
		goto error;
	sctp_clear(sk);
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
	sk->err = 0;
	sk->err_soft = 0;
	sk->done = 0;
	/* place in connection hashes */
	if ((err = sctp_conn_hash(sk)))
		goto error;
#if STREAMS
	/* fake a data request if data in conn req */
	if (dp && (err = sctp_data_req(sk, sp->ppi, sp->sid, 0, 0, 0, dp)))
		goto unhash_error;
#endif				/* STREAMS */
#if SOCKETS
	/* why do we do this? */
	if (!sk->dead)
		sk->state_change(sk);
	else
		swerr();
#endif				/* SOCKETS */
	if ((err = sctp_send_init(sk)))
		goto unhash_error;
	return (0);
      einval:
	err = -EINVAL;
	goto error;
#if STREAMS
      eacces:
	err = -EACCES;
	goto error;
#endif				/* STREAMS */
      eafnosupport:
	err = -EAFNOSUPPORT;
	goto error;
      eisconn:
	err = -EISCONN;
	goto error;
      eaddrnotavail:
	err = -EADDRNOTAVAIL;
	goto error;
      unhash_error:
	sctp_unhash(sk);
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
STATIC void sctp_listen_stop(struct sock *sk);
STATIC int
sctp_discon_req(struct sock *sk, struct sk_buff *cp)
{
	printd(("%p: X_DISCON_REQ <- \n", sk));
	/* Caller must ensure that sk and cp (if any) are correct and appropriate. */
	if (cp) {
		struct iphdr *iph = SCTP_IPH(cp);
		struct sctphdr *sh = SCTP_SH(cp);
		sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		/* conn ind will be unlinked by caller */
		return (0);
	}
	if (sk->state == SCTP_LISTEN) {
		sctp_listen_stop(sk);
	} else if ((1 << sk->state) & (SCTPF_NEEDABORT)) {
		/* SCTP IG 2.21 */
		sctp_send_abort_error(sk, SCTP_CAUSE_USER_INITIATED, NULL, 0);
		sk->err = ECONNRESET;
	} else if ((1 << sk->state) & (SCTPF_OPENING)) {
		sk->err = ECONNRESET;
	}
	sctp_reset(sk);
	if (!sk->dead)
		sk->error_report(sk);
	else
		swerr();
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
sctp_unbind(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	assert(sk);
	sctp_change_state(sk, SCTP_CLOSED);
	sk->userlocks = 0;	/* break user locks */
#if STREAMS
	sp->conind = 0;		/* remove from listen hashes */
#endif				/* STREAMS */
	sctp_reset(sk);
	sctp_clear(sk);
	unusual(!skb_queue_empty(&sp->conq));
	__skb_queue_purge(&sp->conq);
}

/*
 *  UNBIND_REQ:
 *  -------------------------------------------------------------------------
 *  STREAMS is able to issue an unbind request directly.  Sockets is not.  For sockets, according to
 *  X/Open and POSIX, the way to unbind a socket is to bind the socket to AF_UNSPEC.  This is
 *  normally intercepted by the socket code which then unhashes the socket.
 */
STATIC int
sctp_unbind_req(struct sock *sk)
{
	printd(("%p: X_UNBIND_REQ <- \n", sk));
	switch (sk->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		/* can't wait for SHUTDOWN COMPLETE any longer */
		SCTP_INC_STATS_BH(SctpShutdowns);
	case SCTP_CLOSED:
	case SCTP_LISTEN:
		local_bh_disable();
		bh_lock_sock(sk);
		sctp_unbind(sk);
		bh_unlock_sock(sk);
		local_bh_enable();
		return (0);
	}
	rare();
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
#if STREAMS
STATIC int
sctp_reset_req(struct sock *sk)
{
	int err;
	printd(("%p: X_RESET_REQ <- \n", sk));
	/* do nothing */
	if ((err = sctp_reset_con(sk)))
		return (err);
	return (0);
}
#endif				/* STREAMS */

/*
 *  RESET_RES:
 *  -------------------------------------------------------------------------
 *  Only the STREAMS NPI interface protocol provides a mechanism to directly respond to reset
 *  indications with a reset response (N_RESET_RES).  Neither STREAMS XTI/TPI nor sockets supports
 *  such a direct mechanism.  For XTI/TPI and Sockets, the response should be automatically
 *  generated internal to the provider.
 */
#if STREAMS
STATIC int
sctp_reset_res(struct sock *sk)
{
	struct sk_buff *cp;
	printd(("%p: X_RESET_RES <- \n", sk));
	if (!(cp = __skb_dequeue(&SCTP_PROT(sk)->conq))) {
		rare();
		return (-EFAULT);
	}
	return sctp_conn_res(sk, cp, sk, NULL);
}
#endif				/* STREAMS */

/*
 *  CONN_RES:
 *  -------------------------------------------------------------------------
 *  This is the accept(2) or t_accept(3) call being performed on the socket or stream.  This will
 *  result the the waiting for acceptance of an incoming SCTP association.  This is only performed
 *  after a bind and a listen.  The stream must be in the listening state.
 */
STATIC int
sctp_conn_res(struct sock *sk, struct sk_buff *cp, struct sock *ak, struct sk_buff *dp)
{
	sctp_t *sp = SCTP_PROT(sk);
	sctp_t *ap = SCTP_PROT(ak);
	int err;
	struct sctp_cookie_echo *m;
	struct sctp_cookie *ck;
	uint32_t *daddrs;
	uint32_t *saddrs;
	printd(("%p: X_CONN_RES <- \n", sk));
	assert(sk);
	assert(cp);
	assert(ak);
	m = (struct sctp_cookie_echo *) cp->data;
	ck = (struct sctp_cookie *) m->cookie;
	daddrs = (uint32_t *) (ck + 1);
	saddrs = daddrs + ck->danum;
	local_bh_disable();
	bh_lock_sock(ak);
#if STREAMS
	/* STREAMS uses either the listening stream or another stream that is possibly already
	   bound to an accepting address.  Because of this it is necessary in STREAMS to clear the
	   state of the accepting stream and rebind it if necessary.  For sockets, we always start
	   with a freshly created socket upon which to accept the connection and do not have to
	   clear old state on the socket. */
	if (ap != sp) {
		__usual(ap->users == 0);
		ak->userlocks = 0;	/* break locks */
	}
	sctp_reset(ak);		/* we need to rebind the accepting stream */
	sctp_clear(ak);
#endif				/* STREAMS */
	/* Force user locks because both the source port and source addresses are specified in the
	   cookie.  */
	if (ap != sp)
		ak->userlocks |= (SOCK_BINDPORT_LOCK | SOCK_BINDADDR_LOCK);
	if (!sctp_saddr_include(ap, ck->saddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_saddrs(ap, ck->sport, saddrs, ck->sanum)))
		goto error;
	if (ap != sp)
		__sctp_inherit_port(ak, sk);
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
#if SOCKETS
	ap->a_rwnd = ak->rcvbuf;
	atomic_set(&sk->refcnt, 1);
#endif				/* SOCKETS */
	if ((err = sctp_conn_hash(ak)))
		goto error;
	if ((err = sctp_update_routes(ak, 1)))
		goto error;
	sctp_change_state(ak, SCTP_ESTABLISHED);
#if STREAMS
	ap->users++;
#endif				/* STREAMS */
#if SOCKETS
#ifdef INET_REFCNT_DEBUG
	atomic_inc(&inet_sock_nr);
#endif				/* INET_REFCNT_DEBUG */
	/* put on master list */
	spin_lock_bh(&sctp_protolock);
	if ((ak->next = sctp_protolist))
		ak->next->pprev = &ak->next;
	ak->pprev = &sctp_protolist;
	sctp_protolist = ak;
#ifdef SCTP_CONFIG_MODULE
	MOD_INC_USE_COUNT;
#endif				/* SCTP_CONFIG_MODULE */
	atomic_inc(&sctp_socket_count);
	spin_unlock_bh(&sctp_protolock);
	printd(("INFO: There are now %d sockets allocated\n", atomic_read(&sctp_socket_count)));
	ak->use_write_queue = 0;
#endif				/* SOCKETS */
	/* unlock accepting socket or stream */
	bh_unlock_sock(ak);
	local_bh_enable();
	/* process any chunks bundled with cookie echo on accepting stream */
	if (sctp_return_more(cp) > 0)
		sctp_recv_msg(ak, cp);
#if STREAMS
	/* fake a data request if data in conn res */
	if (dp && (err = sctp_data_req(ak, ap->ppi, ap->sid, 0, 0, 0, dp)))
		goto release_error;
#endif				/* STREAMS */
	sctp_send_cookie_ack(ak, cp);
	SCTP_INC_STATS_USER(SctpPassiveEstabs);
	/* caller will unlink connect indication */
	release_sock(ak);
	return (0);
      error:
	if (ap != sp)
		ak->userlocks = 0;	/* break bind locks */
	sctp_unhash(ak);
	bh_unlock_sock(ak);
	local_bh_enable();
	return (err);
#if STREAMS
      release_error:
	if (ap != sp)
		ak->userlocks = 0;	/* break bind locks */
	sctp_unhash(ak);
	release_sock(ak);
	return (err);
#endif				/* STREAMS */
}

/*
 *  ORDREL_REQ:
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_ordrel_req(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	printd(("%p: X_ORDREL_REQ <- \n", sk));
	switch (sk->state) {
	case SCTP_ESTABLISHED:
		sctp_change_state(sk, SCTP_SHUTDOWN_PENDING);
	case SCTP_SHUTDOWN_PENDING:
		/* check for empty send queues */
		if (skb_queue_empty(&sk->sndq)
		    && skb_queue_empty(&sp->urgq)
		    && skb_queue_empty(&sp->rtxq))
			sctp_send_shutdown(sk);
		break;
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_change_state(sk, SCTP_SHUTDOWN_RECVWAIT);
	case SCTP_SHUTDOWN_RECVWAIT:
		/* check for empty send queues */
		if (skb_queue_empty(&sk->sndq)
		    && skb_queue_empty(&sp->urgq)
		    && skb_queue_empty(&sp->rtxq))
			sctp_send_shutdown_ack(sk);
		break;
	default:
		rare();
		_ptrace(("sk->state = %d\n", sk->state));
		return (-EPROTO);
	}
	return (0);
}

/*
 *  DATA_REQ:
 *  -------------------------------------------------------------------------
 */
#if STREAMS
STATIC int
sctp_data_req(struct sock *sk, uint32_t ppi, uint16_t sid, uint ord, uint more, uint rcpt,
	      struct sk_buff *skb)
{
	uint err = 0, flags = 0;
	struct sctp_strm *st;
	printd(("%p: X_DATA_REQ <- \n", sk));
	ensure(skb, return (-EFAULT));
	/* don't allow zero-length data through */
	if (skb && !skb->len) {
		pswerr(("sctp: %p: skb = 0x%p msgdsize = %d\n", sk, skb, skb->len));
		kfree_skb(skb);
		return (0);
	}
	if (!(st = sctp_ostrm_find(SCTP_PROT(sk), sid, &err)))
		return (err);
	/* we probably want to data ack out of order as well */
#if 0
	if (rcpt || (ord && (SCTP_PROT(sk)->flags & SCTP_FLAG_DEFAULT_RC_SEL)))
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
	return sctp_send_data(sk, st, flags, skb);
}
#endif				/* STREAMS */

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
#ifdef sysctl_local_port_range
extern int sysctl_local_port_range[2];
#else				/* sysctl_local_port_range */
STATIC int sysctl_local_port_range[2] = { 1024, 4999 };
#endif				/* sysctl_local_port_range */
STATIC INLINE int
sctp_addr_match(sctp_t *sp1, sctp_t *sp2)
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
sctp_bind_conflict(struct sock *sk, struct sctp_bind_bucket *sb)
{
	sctp_t *sp2, *sp = SCTP_PROT(sk);
	struct sock *sk2 = NULL;
	/* The bind bucket is marked for fastreuse when there is no listening or non-reusable
	   socket on the owners list.  */
	if (sb->owners && sb->fastreuse != 0 && sk->reuse != 0 && sk->state != SCTP_LISTEN)
		return 0;
	for (sp2 = sb->owners, sk2 = SCTP_SOCK(sp2); sp2; sp2 = sp2->bnext, sk2 = SCTP_SOCK(sp2))
		if (sk != sk2 && sk->bound_dev_if == sk2->bound_dev_if
		    && (!sk->reuse || !sk2->reuse || sk->state == SCTP_LISTEN
			|| sk2->state == SCTP_LISTEN))
			if (!(sk->userlocks & SOCK_BINDADDR_LOCK)
			    || !(sk2->userlocks & SOCK_BINDADDR_LOCK) ||
			    /* dig into the source address list */
			    sctp_addr_match(sp, sp2))
				break;
	if (sp2)
		ptrace(("CONFLICT: port %d conflict sk = %p with sk2 = %p\n", sb->port, sk, sk2));
	return (sp2 != NULL);
}
STATIC int
sctp_get_port(struct sock *sk, unsigned short port)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_bind_bucket *sb = NULL;
	local_bh_disable();
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
			if (!(sb = sctp_bindb_get(rover)))
				break;
			sctp_bindb_put(rover);
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
		sb = sctp_bindb_get(port);
	if (!sb) {
		/* create an entry if there isn't one already */
		/* we still have a write lock on the hash slot */
		if (!(sb = __sctp_bindb_create(port)))
			goto fail_put;
	} else {
		/* check reusability or conflict if entry exists */
		if (sctp_bind_conflict(sk, sb))
			goto fail_put;
	}
	/* put this socket in the bind hash */
	__sctp_bhash_insert(sp, sb);
	sctp_bindb_put(port);
	local_bh_enable();
	return 0;
      fail_put:
	sctp_bindb_put(port);
      fail:
	local_bh_enable();
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
STATIC int
sctp_bind_req(struct sock *sk, uint16_t sport, struct sockaddr_in *ssin, size_t snum, ulong cons)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err;
	uint32_t saddr = 0;
	unsigned short num;
	printd(("%p: X_BIND_REQ <- \n", sk));
	assert(sk);
	lock_sock(sk);
	if (snum < 1)
		goto einval;
	num = ntohs(sport);
	usual(num);
	if (num && num < PROT_SOCK && !capable(CAP_NET_BIND_SERVICE))
		goto eacces;
	if (sk->state != SCTP_CLOSED || sk->num != 0)
		goto einval;
	if ((err = sctp_alloc_sock_saddrs(sp, sport, ssin, snum)) < 0)
		goto error;
	usual(err >= 0);
	/* set primary */
	if (err > 0) {
		if (sp->saddr)
			saddr = sp->saddr->saddr;
		else
			swerr();
	}
	usual(saddr);
	sk->rcv_saddr = sk->saddr = saddr;
	(void) saddr;
	/* also places us in bind (and possibly listen for streams) hashes */
	sctp_change_state(sk, SCTP_CLOSED);
	if (sctp_get_port(sk, num))
		goto eaddrinuse;
	if (sp->saddr && sp->saddr->saddr != INADDR_ANY)
		sk->userlocks |= SOCK_BINDADDR_LOCK;
	if (num)
		sk->userlocks |= SOCK_BINDPORT_LOCK;
	sk->sport = htons(sk->num);
	__sctp_free_daddrs(sp);
	release_sock(sk);
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
	sctp_change_state(sk, SCTP_CLOSED);
#if STREAMS
	sp->conind = 0;
#endif				/* STREAMS */
	release_sock(sk);
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
sctp_add_ip(struct sock *sk, struct sockaddr_in *addr)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err = 0;
	struct sctp_saddr *ss;
	if ((1 << sk->state) & (SCTPF_DISCONNECTED)) {
		todo(("TODO: perform an addition to the bind\n"));
		return -ENOTCONN;
	} else if (sk->state == SCTP_ESTABLISHED) {
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
		sctp_transmit_wakeup(sk);
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
sctp_del_ip(struct sock *sk, struct sockaddr_in *addr)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	if ((1 << sk->state) & (SCTPF_DISCONNECTED)) {
		todo(("TODO: perform an removal from the bind\n"));
		return -ENOTCONN;
	} else if (sk->state == SCTP_ESTABLISHED) {
		if (sp->sanum < 2)
			return -EINVAL;
		if (!(ss = sctp_find_saddr(sp, addr->sin_addr.s_addr)))
			return (0);	/* silent success */
		ss->flags |= SCTP_SRCEF_DEL_REQUEST;
		if (!(sp->p_caps & SCTP_CAPS_ADD_IP)
		    || !(sp->l_caps & SCTP_CAPS_ADD_IP))
			return -EOPNOTSUPP;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sk);
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
sctp_set_ip(struct sock *sk, struct sockaddr_in *addr)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	if (sk->state == SCTP_ESTABLISHED) {
		if (sp->sanum == 0)
			return -EINVAL;
		if (!(ss = sctp_find_saddr(sp, addr->sin_addr.s_addr)))
			return -EINVAL;
		ss->flags |= SCTP_SRCEF_SET_REQUEST;
		if (!(sp->p_caps & SCTP_CAPS_SET_IP)
		    || !(sp->l_caps & SCTP_CAPS_SET_IP))
			return -EOPNOTSUPP;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sk);
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
sctp_init_struct(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	ptrace(("Initializing socket sk=%p\n", sk));
	sp->sackf = SCTP_SACKF_NEW;	/* don't delay first sack */
	/* initialize timers */
	sp_init_timeout(sp, &sp->timer_init, &sctp_init_timeout);
	sp_init_timeout(sp, &sp->timer_cookie, &sctp_cookie_timeout);
	sp_init_timeout(sp, &sp->timer_shutdown, &sctp_shutdown_timeout);
	sp_init_timeout(sp, &sp->timer_guard, &sctp_guard_timeout);
	sp_init_timeout(sp, &sp->timer_sack, &sctp_sack_timeout);
#ifdef SCTP_CONFIG_ADD_IP
	sp_init_timeout(sp, &sp->timer_asconf, &sctp_asconf_timeout);
#endif				/* SCTP_CONFIG_ADD_IP */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	sp_init_timeout(sp, &sp->timer_life, &sctp_life_timeout);
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
	/* initialize queues */
	skb_queue_head_init(&sp->conq);
	skb_queue_head_init(&sp->expq);
	skb_queue_head_init(&sp->urgq);
	skb_queue_head_init(&sp->oooq);
	skb_queue_head_init(&sp->errq);
	skb_queue_head_init(&sp->rtxq);
	skb_queue_head_init(&sp->dupq);
	skb_queue_head_init(&sp->ackq);
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
	sp->hb_tint = (sysctl_sctp_heartbeat_itvl >> 1) + 1;
	sp->hb_rcvd = jiffies;
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
	/* start in closed state */
	sctp_change_state(sk, SCTP_CLOSED);
	/* association defaults */
	sp->a_rwnd = sk->rcvbuf;
	sp->max_istr = sysctl_sctp_max_istreams;
	sp->req_ostr = sysctl_sctp_req_ostreams;
	sp->max_inits = sysctl_sctp_max_init_retries;
	sp->max_retrans = sysctl_sctp_assoc_max_retrans;
	sp->ck_life = sysctl_sctp_valid_cookie_life;
	sp->ck_inc = sysctl_sctp_cookie_inc;
	sp->hmac = sysctl_sctp_mac_type;
	sp->cksum = sysctl_sctp_csum_type;
	sp->throttle = sysctl_sctp_throttle_itvl;
	sp->sid = sysctl_sctp_default_sid;
	sp->ppi = sysctl_sctp_default_ppi;
	sp->max_sack = sysctl_sctp_max_sack_delay;
	sp->max_burst = sysctl_sctp_max_burst;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	sp->prel = sysctl_sctp_partial_reliability;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
	sp->l_caps = 0;
#ifdef SCTP_CONFIG_ADAPTATION_LAYER_INFO
	sp->l_ali = sysctl_sctp_adaptation_layer_info;
	if (sp->l_ali)
		sp->l_caps |= SCTP_CAPS_ALI;
#endif				/* SCTP_CONFIG_ADAPTATION_LAYER_INFO */
	sp->pmtu = ip_rt_min_pmtu;
	sp->amps = sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
	/* destination defaults */
	sp->rto_ini = sysctl_sctp_rto_initial;
	sp->rto_min = sysctl_sctp_rto_min;
	sp->rto_max = sysctl_sctp_rto_max;
	sp->rtx_path = sysctl_sctp_path_max_retrans;
	sp->hb_itvl = sysctl_sctp_heartbeat_itvl;
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

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP FREE PRIV
 *
 *  --------------------------------------------------------------------------
 */

/*
 *  SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
 *
 *  Common STREAMS functions
 *
 *  SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
 */

/*
 *  CLEANUP READ
 *  -------------------------------------------------------------------------
 *  This is called to clean up the read queue by the STREAMS read service routine.  This permits
 *  backenabling to work.
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */

/*
 *  IP Read Message
 */

/*
 *  PUTQ Put Routine
 *  -----------------------------------
 */

/*
 *  SRVQ Service Routine
 *  -----------------------------------
 */

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

/*
 *  =========================================================================
 *
 *  NPI Provider (SCTP) -> NPI User Primitives
 *
 *  =========================================================================
 */
/*
 *  N_CONN_IND      11 - Incoming connection indication
 *  ---------------------------------------------------------------
 */

/*
 *  N_CONN_CON      12 - Connection established
 *  ---------------------------------------------------------------
 */

/*
 *  N_DISCON_IND    13 - NC disconnected
 *  ---------------------------------------------------------------
 *  For SCTP the responding address in a NC connection refusal is always the destination address to
 *  which the NC connection request was sent.  If a connection indication identifier (seq) is
 *  provided, this function must either unlink and free it or return an error.  The origin is used
 *  only by the STREAMS NPI interface.
 */

/*
 *  N_DATA_IND      14 - Incoming connection-mode data indication
 *  ---------------------------------------------------------------
 */

/*
 *  N_EXDATA_IND    15 - Incoming expedited data indication
 *  ---------------------------------------------------------------
 *  The lack of a more flag presents a challenge.  Perhaps we should be reassembling expedited data.
 */

/*
 *  N_INFO_ACK      16 - Information Acknowledgement
 *  ---------------------------------------------------------------
 */

/*
 *  N_BIND_ACK      17 - NS User bound to network address
 *  ---------------------------------------------------------------
 */

/*
 *  N_ERROR_ACK     18 - Error Acknowledgement
 *  ---------------------------------------------------------------
 */

/*
 *  N_OK_ACK        19 - Success Acknowledgement
 *  ---------------------------------------------------------------
 */

/*
 *  N_DATACK_IND    24 - Data acknowledgement indication
 *  ---------------------------------------------------------------
 */

/*
 *  N_RESET_IND     26 - Inccoming NC reset request indication
 *  ---------------------------------------------------------------
 */

/*
 *  N_RESET_CON     28 - Reset processing complete
 *  ---------------------------------------------------------------
 */
#if 0
/*
 *  N_RECOVER_IND   29 - NC Recovery indication
 *  ---------------------------------------------------------------
 */

/*
 *  N_RETRIEVE_IND  32 - NC Retrieval indication
 *  ---------------------------------------------------------------
 */

/*
 *  N_RETRIEVE_CON  33 - NC Retrieval complete confirmation
 *  ---------------------------------------------------------------
 */
#endif

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

/*
 *  N_CONN_RES           1 - Accept previous connection indication
 *  -------------------------------------------------------------------------
 *  IMPLEMENTATION NOTES:- Sequence numbers are actually the address of the mblk which contains the
 *  COOKIE-ECHO chunk and contains the cookie as a connection indication.  To find if a particular
 *  sequence number is valid, we walk the connection indication queue looking for a mblk with the
 *  same address as the sequence number.  Sequence numbers are only valid on the stream for which
 *  the connection indication is queued.
 */

/*
 *  N_DISCON_REQ         2 - NC disconnection request
 *  -------------------------------------------------------------------------
 */

/*
 *  N_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------------
 */

/*
 *  N_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------------
 */

/*
 *  N_INFO_REQ           5 - Information request
 *  -------------------------------------------------------------------------
 */
/*
 *  N_BIND_REQ           6 - Bind a NS user to network address
 *  -------------------------------------------------------------------------
 */

/*
 *  N_UNBIND_REQ         7 - Unbind NS user from network address
 *  -------------------------------------------------------------------------
 */

/*
 *  N_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------------
 */

/*
 *  N_RESET_REQ         25 - NC reset request
 *  -------------------------------------------------------------------------
 */

/*
 *  N_RESET_RES         27 - Reset processing accepted
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */
/*
 *  NPI Write Message
 */

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  =========================================================================
 *
 *  LfS Module Initialization
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
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

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 *  This driver defines two user interfaces: one NPI, the other TPI.
 */

/*
 *  =========================================================================
 *
 *  OPTION Handling
 *
 *  =========================================================================
 */

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

/*
 *  Size connection indication options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Return an option buffer size for a connection indication.  Options without end-to-end
 *  significance are not indicated.  Options with end-to-end significance are always indicated.  For
 *  more information on XTI connection indication option handling, see t_rcvconnect(3).
 */

/*
 *  Size connection confirmation options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Return an option buffer size for a connection confirmation.  Options without end-to-end
 *  significance are only confirmed if requested.  Options with end-to-end significance are always
 *  confirmed. For more information on XTI connection confirmation handling, see t_connect(3).
 */

/*
 *  Size Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Check the validity of the options structure, check for correct size of each supplied option
 *  given the option management flag, and return the size required of the acknowledgement options
 *  field.
 */

/*
 *  Overall Option Result
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Calculates the overall T_OPTMGMT_ACK flag result from individual results.
 */

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

/*
 *  Default Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT placing the output in the provided buffer.
 */

/*
 *  Current Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_CURRENT placing the output in the provided buffer.
 */

/*
 *  Check Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_CHECK placing the output in the provided buffer.
 */

/*
 *  Process Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK and T_NEGOTIARE, placing the
 *  output in the provided buffer.
 */

/*
 *  Process Options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Perform the actions required of T_DEFAULT, T_CURRENT, T_CHECK and T_NEGOTIARE, placing the
 *  output in the provided buffer.
 */

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

/*
 *  T_CONN_CON      12 - connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated are the number of inbound and
 *  outbound streams.
 */

/*
 *  T_DISCON_IND    13 - disconnect indication
 *  -----------------------------------------------------------------
 *  We use the address of the mblk that contains the COOKIE-ECHO chunk as a SEQ_number for connect
 *  indications that are rejected with a disconnect indication as well.  We can use this to directly
 *  address the mblk in the connection indication bufq.
 *
 *  If the caller provides disconnect data, the caller needs to set the current ord, ppi, sid, and
 *  ssn fields so that the user can examine them with T_OPTMGMT_REQ T_CURRENT if it has need to know
 *  them.
 *
 *  If a connection indication identifier (seq) is provided, this function muse either unlink and
 *  free it or return an error.  The origin is used only by the STREAMS NPI interface.
 */

/*
 *  T_DATA_IND      14 - data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default stream.  The
 *  caller should check that ppi and sid match the default before using this indication.  Otherwise
 *  the caller should use the T_OPTDATA_IND.
 */

/*
 *  T_EXDATA_IND    15 - expedited data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default stream.  The
 *  caller should check that ppi and ssn match the default before using this indication.  Otherwise
 *  the caller should use the T_OPTDATA_IND.
 */

/*
 *  T_INFO_ACK      16 - information acknowledgement
 *  -----------------------------------------------------------------
 *  Although there is no limit on CDATA and DDATA size, if these are too large then we will IP
 *  fragment the message.
 */

/*
 *  T_BIND_ACK      17 - bind acknowledgement
 *  -----------------------------------------------------------------
 */

/*
 *  T_ERROR_ACK     18 - error acknowledgement
 *  -----------------------------------------------------------------
 */

/*
 *  T_OK_ACK        19 - success acknowledgement
 *  -----------------------------------------------------------------
 */

/*
 *  T_OPTMGMT_ACK   22 - options management acknowledgement
 *  -----------------------------------------------------------------
 */

/*
 *  T_ORDREL_IND    23 - orderly release indication
 *  -----------------------------------------------------------------
 */

/*
 *  T_OPTDATA_IND   26 - data with options indication
 *  -----------------------------------------------------------------
 */

/*
 *  T_ADDR_ACK      27 - address acknowledgement
 *  -----------------------------------------------------------------
 */
#ifdef T_CAPABILITY_ACK
/*
 *  T_CAPABILITY_ACK ?? - protocol capability ack
 *  -----------------------------------------------------------------
 */
#endif

/*
 *  NOTES:- Sockets and XTI/TPI cannot do data acknowledgements, resets or retrieval directly.  Data
 *  acknowledgements and retrieval are different forms of the same service.  For data
 *  acknowledgement, use the STREAMS NPI interface.  For reset support (SCTP Restart indication
 *  different from SCTP CDI), use the STREAMS NPI interface.
 */

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

/*
 *  T_DATA_REQ          3 - Connection-Mode data transfer request
 *  -----------------------------------------------------------------
 */

/*
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -----------------------------------------------------------------
 */

/*
 *  T_INFO_REQ           5 - Information Request
 *  -----------------------------------------------------------------
 */

/*
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -----------------------------------------------------------------
 */

/*
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -----------------------------------------------------------------
 */

/*
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -----------------------------------------------------------------
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

/*
 *  T_ORDREL_REQ        10 - TS user is finished sending
 *  -----------------------------------------------------------------
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

/*
 *  T_OPTDATA_REQ       24- data with options request
 *  -----------------------------------------------------------------
 *  Basically the purpose of this for SCTP is to be able to set the SCTP Unordered Bit (U-bit),
 *  Payload Protocol Identifier (PPI) and Stream Id (sid) associated with the data message.  If not
 *  specified, each option will reduce to the current default settings.
 */

#ifdef T_ADDR_REQ
/*
 *  T_ADDR_REQ          25 - address request
 *  -----------------------------------------------------------------
 */
#endif

#ifdef T_CAPABILITY_REQ
/*
 *  T_CAPABILITY_REQ    ?? - Capability Request
 *  -------------------------------------------------------------------
 */
#endif

/*
 *  Other primitives    XX - other invalid primitives
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  =========================================================================
 *
 *  STREAMS PUTQ and SRVQ routines
 *
 *  =========================================================================
 */
/*
 *  TPI Write Message
 */

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
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
STATIC void
sctp_listen_stop(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	/* break user locks */
	sk->userlocks = 0;
	sctp_unhash(sk);
	sctp_change_state(sk, SCTP_CLOSED);
	__skb_queue_purge(&sp->conq);
	sk->ack_backlog = 0;
}
STATIC void
sctp_close(struct sock *sk, long timeout)
{
	sctp_t *sp = SCTP_PROT(sk);
	int data_was_unread = 0;
	ptrace(("Closing socket sk=%p, state = %d\n", sk, sk->state));
	lock_sock(sk);
	sk->shutdown = SHUTDOWN_MASK;
	if (sk->state == SCTP_LISTEN) {
		ptrace(("Closing listening socket sk=%p\n", sk));
		sctp_listen_stop(sk);
		goto dead;
	}
	if ((1 << sk->state) & (SCTPF_OPENING)) {
		ptrace(("Closing opening socket sk=%p\n", sk));
		sctp_reset(sk);
		goto dead;
	}
	/* why would socket be dead? when could we double close? */
	if (!sk->dead)
		sk->state_change(sk);
	data_was_unread = skb_queue_len(&sk->rcvq) + skb_queue_len(&sp->expq);
	{
		__skb_queue_purge(&sk->rcvq);
		__skb_queue_purge(&sp->expq);
		sp->rmem_queued = 0;
		__skb_queue_purge(&sp->oooq);
		sp->gaps = NULL;
		sp->ngaps = 0;
		sp->nunds = 0;
		__skb_queue_purge(&sp->dupq);
		sp->dups = NULL;
		sp->ndups = 0;
		__skb_queue_purge(&sp->ackq);
	}
	if (data_was_unread) {
		printd(("INFO: Closing with unread data, sk=%p\n", sk));
		sk->prot->disconnect(sk, 0);
	} else if (sk->linger && sk->lingertime == 0) {
		sk->prot->disconnect(sk, 0);
	} else if ((1 << sk->state) & (SCTPF_CONNECTED)) {
		/* Perform SHUTDOWN procedure if all data read.  */
		printd(("INFO: Performing orderly shutdown sk=%p\n", sk));
		sk->shutdown = SHUTDOWN_MASK;
		switch (sk->state) {
		case SCTP_ESTABLISHED:
			sctp_change_state(sk, SCTP_SHUTDOWN_PENDING);
		case SCTP_SHUTDOWN_PENDING:
			if (atomic_read(&sk->wmem_alloc) <= 0)
				sctp_send_shutdown(sk);
			else
				sk->prot->disconnect(sk, 0);
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			sctp_change_state(sk, SCTP_SHUTDOWN_RECVWAIT);
		case SCTP_SHUTDOWN_RECVWAIT:
			if (atomic_read(&sk->wmem_alloc) <= 0)
				sctp_send_shutdown_ack(sk);
			else
				sk->prot->disconnect(sk, 0);
			break;
		default:
			swerr();
			break;
		}
	}
	if (timeout) {
		DECLARE_WAITQUEUE(wait, current);
		ptrace(("Waiting for socket sk=%p to close\n", sk));
		printd(("INFO: Socket %p entering Linger wait queue\n", sk));
		add_wait_queue(sk->sleep, &wait);
		do {
			set_current_state(TASK_INTERRUPTIBLE);
			if ((1 << sk->state) & ~(SCTPF_CLOSING))
				break;
			release_sock(sk);
			if ((1 << sk->state) & ~(SCTPF_CLOSING))
				timeout = schedule_timeout(timeout);
			lock_sock(sk);
		} while (!signal_pending(current) && timeout);
		set_current_state(TASK_RUNNING);
		remove_wait_queue(sk->sleep, &wait);
	}
      dead:
	release_sock(sk);
	local_bh_disable();
	bh_lock_sock(sk);
	sock_hold(sk);
	/* we're dead */
	sk->use_write_queue = 1;	/* don't callback write space */
	sock_orphan(sk);
	atomic_inc(&sctp_orphan_count);
	printd(("INFO: There are now %d orphan sockets\n", atomic_read(&sctp_orphan_count)));
	/* release socket now, otherwise wait for the protocol to release the socket when the last
	   reference to it is released (the protocol will release the last reference when the
	   connection is closed with sk->dead == 1 */
	if (sk->state == SCTP_CLOSED)
		sctp_destroy_orphan(sk);
	bh_unlock_sock(sk);
	local_bh_enable();
	usual(atomic_read(&sk->refcnt) == 1);
	if (atomic_read(&sk->refcnt) == 1)
		ptrace(("Immediate close socket %p\n", sk));
	else
		ptrace(("Delayed close socket %p, with refcnt = %d\n", sk,
			atomic_read(&sk->refcnt) - 1));
	sock_put(sk);
}

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
STATIC int
sctp_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	struct sockaddr_in *dsin = (struct sockaddr_in *) uaddr;
	int dnum = addr_len / sizeof(struct sockaddr_in);
	unsigned short dport = dsin->sin_port;
	ptrace(("Connecting socket sk=%p, state = %d\n", sk, sk->state));
	return sctp_conn_req(sk, dport, dsin, dnum, NULL);
}

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
STATIC int
sctp_disconnect(struct sock *sk, int flags)
{
	(void) flags;		/* XXX: well, this is an issue */
	return sctp_discon_req(sk, NULL);
}

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
STATIC struct sock *
sctp_passive_connect(struct sock *lsk, struct sk_buff *skb, int *errp)
{
	sctp_t *lsp = SCTP_PROT(lsk);
	sctp_t *sp;
	struct sock *sk;
	int err = 0;
	struct sctp_cookie_echo *m = (typeof(m)) skb->data;
	struct sctp_cookie *ck = (typeof(ck)) m->cookie;
	err = -ENOMEM;
	if (!(sk = sk_alloc(PF_INET, GFP_ATOMIC, 0)))
		goto error;
	/* 
	 *  Initialize socket information
	 */
	sp = SCTP_PROT(sk);
	bcopy(lsk, sk, sizeof(*sk));
	sock_lock_init(sk);
	bh_lock_sock(sk);
	sk->pprev = NULL;
	sk->prev = NULL;
	sk->dst_lock = RW_LOCK_UNLOCKED;
	sk->dst_cache = NULL;
	sk->route_caps = 0;	/* this is set up in update routes */
	atomic_set(&sk->rmem_alloc, 0);
	atomic_set(&sk->wmem_alloc, 0);
	atomic_set(&sk->omem_alloc, 0);
	skb_queue_head_init(&sk->rcvq);
	skb_queue_head_init(&sk->sndq);
	sk->wmem_queued = 0;
	skb_queue_head_init(&sp->errq);
	sk->forward_alloc = 0;
	sk->done = 0;
	sk->proc = 0;
	sk->backlog.head = sk->backlog.tail = NULL;
	sk->callback_lock = RW_LOCK_UNLOCKED;
#ifdef CONFIG_FILTER
	if (sk->filter)
		sk_filter_charge(sk, sk->filter);
#endif				/* CONFIG_FILTER */
#if defined HAVE___XFRM_SK_CLONE_POLICY_EXPORT || defined HAVE___XFRM_SK_CLONE_POLICY_ADDR
	if (unlikely(xfrm_sk_clone_policy(sk))) {
		sk->destruct = NULL;
		sk_free(sk);
		return NULL;
	}
#endif				/* defined HAVE___XFRM_SK_CLONE_POLICY_EXPORT || defined
				   HAVE___XFRM_SK_CLONE_POLICY_ADDR */
	sk->err = 0;
	sk->priority = 0;
	sk->socket = NULL;
	sk->sleep = NULL;
	init_timer(&sk->timer);
//      sk->timer.function = &sctp_timeout;
//      sk->timer.data = (unsigned long) sk;
//      sk->timeout = 0;
	sk->daddr = ck->daddr;
	sk->rcv_saddr = sk->saddr = ck->saddr;
	/* TODO: handle IP options sk->protinfo.af_inet.opt */
	sk->protinfo.af_inet.opt = NULL;
	sk->protinfo.af_inet.id = ck->v_tag ^ jiffies;
	sk->shutdown = 0;
	/* 
	 *  Initialize protocol information
	 */
	sp = SCTP_PROT(sk);
#if 0
	bzero(sp, sizeof(*sp));	/* more zeros than anything else */
#else
	/* break zero set into two pieces */
	bzero(&sk->tp_pinfo, sizeof(sk->tp_pinfo));
	bzero((__u8 *) & sk->protinfo + sizeof(sk->protinfo.af_inet),
	      sizeof(sk->protinfo) - sizeof(sk->protinfo.af_inet));
#endif
	/* initialize timers */
	sp_init_timeout(sp, &sp->timer_init, &sctp_init_timeout);
	sp_init_timeout(sp, &sp->timer_cookie, &sctp_cookie_timeout);
	sp_init_timeout(sp, &sp->timer_shutdown, &sctp_shutdown_timeout);
	sp_init_timeout(sp, &sp->timer_guard, &sctp_guard_timeout);
	sp_init_timeout(sp, &sp->timer_sack, &sctp_sack_timeout);
#ifdef SCTP_CONFIG_ADD_IP
	sp_init_timeout(sp, &sp->timer_asconf, &sctp_asconf_timeout);
#endif				/* SCTP_CONFIG_ADD_IP */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	sp_init_timeout(sp, &sp->timer_life, &sctp_life_timeout);
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
	/* initialize queues */
	skb_queue_head_init(&sp->conq);
	skb_queue_head_init(&sp->urgq);
	skb_queue_head_init(&sp->expq);
	skb_queue_head_init(&sp->oooq);
	skb_queue_head_init(&sp->errq);
	skb_queue_head_init(&sp->rtxq);
	skb_queue_head_init(&sp->dupq);
	skb_queue_head_init(&sp->ackq);
	/* TODO: handle IP options sk->protinfo.af_inet.opt */
	/* initialize hash linkage */
	sp->debug = lsp->debug;
	/* *INDENT-OFF* */
	/* association defaults */
	sp->max_istr	= lsp->max_istr;
	sp->req_ostr	= lsp->req_ostr;
	sp->max_inits	= lsp->max_inits;
	sp->max_retrans	= lsp->max_retrans;
	sp->ck_life	= lsp->ck_life;
	sp->ck_inc	= lsp->ck_inc;
	sp->hmac	= lsp->hmac;
	sp->cksum	= lsp->cksum;
	sp->throttle	= lsp->throttle;
	sp->sid		= lsp->sid;
	sp->ppi		= lsp->ppi;
	sp->max_sack	= lsp->max_sack;
	sp->max_burst	= lsp->max_burst;
	sp->pmtu	= ip_rt_min_pmtu;
	sp->amps	= sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
	/* destination defaults */
	sp->rto_ini	= lsp->rto_ini;
	sp->rto_min	= lsp->rto_min;
	sp->rto_max	= lsp->rto_max;
	sp->rtx_path	= lsp->rtx_path;
	sp->hb_itvl	= lsp->hb_itvl;
	/* *INDENT-ON* */
	sp->sackf = SCTP_SACKF_NEW;	/* don't delay first sack */
//      sp->in_flight = 0;
//      sp->retransmits = 0;
	return sk;
      error:
	sk->userlocks = 0;	/* break bind locks */
	sctp_unhash(sk);
	sk_free(sk);
	*errp = err;
	return NULL;
}
STATIC INLINE int
sctp_accept_wait(struct sock *sk, long timeo)
{
	sctp_t *sp = SCTP_PROT(sk);
	DECLARE_WAITQUEUE(wait, current);
	int err;
	printd(("INFO: Socket %p entering Accept wait queue\n", sk));
	add_wait_queue_exclusive(sk->sleep, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		release_sock(sk);
		if (skb_queue_empty(&sp->conq))
			timeo = schedule_timeout(timeo);
		lock_sock(sk);
		err = 0;
		if (!skb_queue_empty(&sp->conq))
			break;
		err = -EINVAL;
		if (sk->state != SCTP_LISTEN)
			break;
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			break;
		err = -EAGAIN;
		if (!timeo)
			break;
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk->sleep, &wait);
	return err;
}
STATIC struct sock *
sctp_accept(struct sock *sk, int flags, int *errp)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	int err = 0;
	ptrace(("Accepting on listening socket %p\n", sk));
	lock_sock(sk);
	err = -EINVAL;
	if (sk->state != SCTP_LISTEN)
		goto out;
	if (skb_queue_empty(&sp->conq)) {
		long timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);
		err = -EAGAIN;
		if (!timeo)
			goto out;
		err = sctp_accept_wait(sk, timeo);
		if (err)
			goto out;
	}
	if ((skb = skb_peek(&sp->conq))) {
		struct sock *newsk;
		if (!(newsk = sctp_passive_connect(sk, skb, &err)))
			goto out;
		if ((err = sctp_conn_res(sk, skb, newsk, NULL)))
			goto out;
		skb_unlink(skb);
		kfree_skb(skb);
		sk->ack_backlog--;
		release_sock(sk);
		printd(("INFO: Socket %p accepted on socket %p\n", newsk, sk));
		return (newsk);
	}
      out:
	release_sock(sk);
	ptrace(("ERROR: couldn't accept on socket %p, error %d\n", sk, err));
	*errp = err;
	return NULL;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP IOCTL
 *
 *  --------------------------------------------------------------------------
 */
STATIC int
sctp_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	int answ = 0;
	sctp_t *sp = SCTP_PROT(sk);
	switch (cmd) {
	case SIOCINQ:
		/* The amount of normal data waiting to be read in the receive queue.  */
		if (sk->state == SCTP_LISTEN)
			return (-EINVAL);
		answ = sp->rmem_queued;
		break;
	case SIOCATMARK:
		/* Returns true when all urgent data has been received by the user program.  */
		if (sk->state == SCTP_LISTEN)
			return (-EINVAL);
		answ = !skb_queue_empty(&sp->expq);
		break;
	case SIOCOUTQ:
		/* (Properly) returns the amount of unsent data in the send queue.  */
		if (sk->state == SCTP_LISTEN)
			return (-EINVAL);
		answ = sk->wmem_queued;
		break;
	default:
		return (-ENOIOCTLCMD);
	}
	return put_user(answ, (int *) arg);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP INIT
 *
 *  --------------------------------------------------------------------------
 */
STATIC struct sctp_ifops s_ops;
STATIC int
sctp_init_sock(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
	if (sk->type != SOCK_STREAM)
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
		sk->keepopen = 1;
#if 0
	sk->timer.function = &sctp_timeout;
	sk->timer.data = (unsigned long) sk;
	sk->timeout = 0;
#endif
#if 0
	/* initialize queues */
	skb_queue_head_init(&sk->rcvq);
	skb_queue_head_init(&sk->sndq);
#endif
	sk->max_ack_backlog = 0;
	sk->shutdown = 0;
	sctp_init_struct(sk);
	sp->ops = &s_ops;
	/* put on master list */
	spin_lock_bh(&sctp_protolock);
	if ((sk->next = sctp_protolist))
		sk->next->pprev = &sk->next;
	sk->pprev = &sctp_protolist;
	sctp_protolist = sk;
#ifdef SCTP_CONFIG_MODULE
	MOD_INC_USE_COUNT;
#endif				/* SCTP_CONFIG_MODULE */
	sk->write_space = sctp_write_space;
	atomic_inc(&sctp_socket_count);
	spin_unlock_bh(&sctp_protolock);
	printd(("INFO: There are now %d sockets allocated\n", atomic_read(&sctp_socket_count)));
	ptrace(("Socket %p creation reference count = %d\n", sk, atomic_read(&sk->refcnt)));
	sk->use_write_queue = 0;
	return (0);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP DESTROY
 *
 *  --------------------------------------------------------------------------
 */
STATIC int
sctp_destroy(struct sock *sk)
{
	ptrace(("Destroying sk = %p\n", sk));
	if (sk->pprev) {
		if (sk->userlocks & (SOCK_BINDPORT_LOCK | SOCK_BINDADDR_LOCK)) {
			ptrace(("Breaking user locks on destroy sk %p\n", sk));
			sk->userlocks = 0;
			sctp_reset(sk);
		}
		sctp_clear(sk);
		/* take off master list */
		spin_lock_bh(&sctp_protolock);
		if ((*(sk->pprev) = sk->next))
			sk->next->pprev = sk->pprev;
		sk->next = NULL;
		sk->pprev = NULL;
		atomic_dec(&sctp_socket_count);
		spin_unlock_bh(&sctp_protolock);
		printd(("INFO: There are now %d sockets allocated\n",
			atomic_read(&sctp_socket_count)));
#ifdef SCTP_CONFIG_MODULE
		MOD_DEC_USE_COUNT;
#endif				/* SCTP_CONFIG_MODULE */
		return (0);
	}
	ptrace(("ERROR: Double destroying sk = %p\n", sk));
	swerr();
	return (-EFAULT);
}

/*
 *  SCTP DESTROY OPHANED SOCKET
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for destroying an orphaned socket.  Orphaned sockets will never have their
 *  sk->prot->destroy functions called by socket code because they have been orphaned from the
 *  socket.  This function calls the destructor for the socket, drains socket queues, and does the
 *  final put on the socket.
 */
STATIC void
sctp_destroy_orphan(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	printd(("INFO: Destroying orphan socket %p\n", sk));
#ifdef SCTP_CONFIG_DEBUG
	if (sk->zapped) {
		ptrace(("ERROR: double destroy sk = %p\n", sk));
		sock_hold(sk);
	}
	sk->zapped = 1;
#endif				/* SCTP_CONFIG_DEBUG */
	sk->prot->destroy(sk);
	__skb_queue_purge(&sk->rcvq);
	__skb_queue_purge(&sp->expq);
	sp->rmem_queued = 0;
	__skb_queue_purge(&sp->errq);
	__skb_queue_purge(&sk->sndq);
	__skb_queue_purge(&sp->urgq);
	sk->wmem_queued = 0;
#if defined HAVE_XFRM_POLICY_DELETE_EXPORT || defined HAVE_XFRM_POLICY_DELETE_ADDR
	xfrm_sk_free_policy(sk);
#endif				/* defined HAVE_XFRM_POLICY_DELETE_EXPORT || defined
				   HAVE_XFRM_POLICY_DELETE_ADDR */
	atomic_dec(&sctp_orphan_count);
	printd(("INFO: There are now %d orphan sockets\n", atomic_read(&sctp_orphan_count)));
	if (atomic_read(&sk->refcnt) != 1)
		ptrace(("Delayed close socket %p, with refcnt = %d\n", sk,
			atomic_read(&sk->refcnt) - 1));
	sock_put(sk);
}

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
STATIC void
sctp_shutdown(struct sock *sk, int how)
{
	sk->shutdown |= how;
	if (!(how & SEND_SHUTDOWN))
		return;
	/* Do an orderly shutdown if possible.  */
	lock_sock(sk);
	sctp_ordrel_req(sk);
	release_sock(sk);
	return;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP SETSOCKOPT
 *
 *  --------------------------------------------------------------------------
 */
#define MAX_WINDOW (0x00010000 - sizeof(struct sctp_data))
STATIC int
sctp_setsockopt(struct sock *sk, int level, int optname, char *optval, int optlen)
{
	sctp_t *sp = SCTP_PROT(sk);
	int val;
	if (level != SOL_SCTP)
		return ip_setsockopt(sk, level, optname, optval, optlen);
	if (optlen < sizeof(int))
		return -EINVAL;
	if (get_user(val, (int *) optval))
		return -EFAULT;
	switch (optname) {
	case SCTP_MAXSEG:
		if (val < 1 || val > MAX_WINDOW)
			return -EINVAL;
		sp->user_amps = val;
		return 0;
	case SCTP_NODELAY:
		if (sp->nonagle == 2)
			return -EINVAL;
		sp->nonagle = (val == 0) ? 0 : 1;
		return 0;
	case SCTP_CORK:
		if (sp->nonagle == 1)
			return -EINVAL;
		if (val != 0)
			sp->nonagle = 2;
		else {
			sp->nonagle = 0;
			lock_sock(sk);
			release_sock(sk);
		}
		return 0;
	case SCTP_RECVSID:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVSID;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVSID;
		return 0;
	case SCTP_RECVPPI:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVPPI;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVPPI;
		return 0;
	case SCTP_RECVSSN:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVSSN;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVSSN;
		return 0;
	case SCTP_RECVTSN:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVTSN;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVTSN;
		return 0;
	case SCTP_SID:
		if (sk->state != SCTP_ESTABLISHED)
			return -ENOTCONN;
		if (val < 0 || val >= sp->n_ostr)
			return -EINVAL;
		sp->sid = val;
		return 0;
	case SCTP_PPI:
		sp->ppi = val;
		return 0;
	case SCTP_HB:
	{
		struct sctp_hbitvl hb;
		struct sctp_daddr *sd;
		if (optlen < sizeof(hb))
			return -EINVAL;
		if (copy_from_user(&hb, optval, sizeof(hb)))
			return -EFAULT;
		if (hb.hb_itvl < 10)
			return -EINVAL;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sp, hb.dest.sin_addr.s_addr))) {
			sd->hb_itvl = hb.hb_itvl * HZ / 1000;
			sd->hb_act = hb.hb_act ? 1 : 0;
			release_sock(sk);
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_RTO:
	{
		struct sctp_rtoval rv;
		struct sctp_daddr *sd;
		if (optlen < sizeof(rv))
			return -EINVAL;
		if (copy_from_user(&rv, optval, sizeof(rv)))
			return -EFAULT;
		if (rv.rto_initial < rv.rto_min || rv.rto_max < rv.rto_min
		    || rv.rto_max < rv.rto_initial)
			return -EINVAL;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sp, rv.dest.sin_addr.s_addr))) {
			sd->rto_ini = rv.rto_initial * HZ / 1000;
			sd->rto_min = rv.rto_min * HZ / 1000;
			sd->rto_max = rv.rto_max * HZ / 1000;
			sd->max_retrans = rv.max_retrans;
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_COOKIE_LIFE:
	{
		if (val < 0)
			return -EINVAL;
		sp->ck_life = val * HZ / 1000;
		return (0);
	}
	case SCTP_SACK_DELAY:
	{
		if (val < 0)
			return -EINVAL;
		sp->max_sack = val * HZ / 1000;
		return (0);
	}
	case SCTP_PATH_MAX_RETRANS:
	{
		if (val < 0)
			return -EINVAL;
		sp->rtx_path = val;
		return (0);
	}
	case SCTP_ASSOC_MAX_RETRANS:
	{
		if (val < 0)
			return -EINVAL;
		sp->max_retrans = val;
		return (0);
	}
	case SCTP_MAX_INIT_RETRIES:
	{
		if (val < 0)
			return -EINVAL;
		sp->max_inits = val;
		return (0);
	}
	case SCTP_MAX_BURST:
	{
		if (val <= 0)
			return -EINVAL;
		sp->max_burst = val;
		return (0);
	}
	case SCTP_HEARTBEAT_ITVL:
	{
		if (val < 0)
			return -EINVAL;
		sp->hb_itvl = val * HZ / 1000;
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
		sp->hb_tint = (val >> 1) + 1;
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
		return (0);
	}
	case SCTP_RTO_INITIAL:
	{
		if ((val > sp->rto_max * 1000 / HZ)
		    || (val < sp->rto_min * 1000 / HZ))
			return -EINVAL;
		sp->rto_ini = val * HZ / 1000;
		return (0);
	}
	case SCTP_RTO_MIN:
	{
		if ((val < 0) || (val > sp->rto_max * 1000 / HZ)
		    || (val > sp->rto_ini * 1000 / HZ))
			return -EINVAL;
		sp->rto_min = val * HZ / 1000;
		return (0);
	}
	case SCTP_RTO_MAX:
	{
		if ((val < 0) || (val < sp->rto_min * 1000 / HZ)
		    || (val < sp->rto_ini * 1000 / HZ))
			return -EINVAL;
		sp->rto_max = val * HZ / 1000;
		return (0);
	}
	case SCTP_OSTREAMS:
	{
		if (val < 1 || val > 0x0000ffff)
			return -EINVAL;
		sp->req_ostr = val;
		return (0);
	}
	case SCTP_ISTREAMS:
	{
		if (val < 1 || val > 0x0000ffff)
			return -EINVAL;
		sp->max_istr = val;
		return (0);
	}
	case SCTP_COOKIE_INC:
	{
		if (val < 0)
			return -EINVAL;
		sp->ck_inc = val * HZ / 1000;
		return (0);
	}
	case SCTP_THROTTLE_ITVL:
	{
		if (val < 0)
			return -EINVAL;
		sp->throttle = val * HZ / 1000;
		return (0);
	}
	case SCTP_MAC_TYPE:
	{
		switch (val) {
		case SCTP_HMAC_NONE:
#ifdef SCTP_CONFIG_HMAC_SHA1
		case SCTP_HMAC_SHA_1:
#endif				/* SCTP_CONFIG_HMAC_SHA1 */
#ifdef SCTP_CONFIG_HMAC_MD5
		case SCTP_HMAC_MD5:
#endif				/* SCTP_CONFIG_HMAC_MD5 */
			sp->hmac = val;
			return (0);
		default:
			return -EINVAL;
		}
	}
	case SCTP_CKSUM_TYPE:
	{
		switch (val) {
#if defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C)
		case SCTP_CSUM_ADLER32:
#endif				/* defined(SCTP_CONFIG_ADLER_32)||!defined(SCTP_CONFIG_CRC_32C) */
#ifdef SCTP_CONFIG_CRC_32C
		case SCTP_CSUM_CRC32C:
#endif				/* SCTP_CONFIG_CRC_32C */
			sp->cksum = val;
			return (0);
		default:
			return -EINVAL;
		}
	}
	case SCTP_DEBUG_OPTIONS:
	{
		sp->debug = val;
		return (0);
	}
#ifdef SCTP_CONFIG_ECN
	case SCTP_ECN:
	{
		if (val) {
			if (!sysctl_sctp_ecn)
				return -EINVAL;
			sp->l_caps |= SCTP_CAPS_ECN;
		} else
			sp->l_caps &= ~SCTP_CAPS_ECN;
		return (0);
	}
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	case SCTP_ALI:
	{
		sp->l_ali = val;
		if (val)
			sp->l_caps |= SCTP_CAPS_ALI;
		else
			sp->l_caps &= ~SCTP_CAPS_ALI;
		return (0);
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_ADD_IP
	case SCTP_ADD:
	{
		if (val)
			sp->l_caps |= SCTP_CAPS_ADD_IP;
		else
			sp->l_caps &= ~SCTP_CAPS_ADD_IP;
		return (0);
	}
	case SCTP_SET:
	{
		if (val)
			sp->l_caps |= SCTP_CAPS_SET_IP;
		else
			sp->l_caps &= ~SCTP_CAPS_SET_IP;
		return (0);
	}
	case SCTP_ADD_IP:
	{
		int rtn;
		struct sockaddr_in addr;
		if (optlen < sizeof(addr))
			return -EINVAL;
		if (copy_from_user(&addr, optval, sizeof(addr)))
			return -EFAULT;
		if (addr.sin_family != AF_INET)
			return -EINVAL;
		if (!sk->sport || addr.sin_port != sk->sport)
			return -EINVAL;
		lock_sock(sk);
		rtn = sctp_add_ip(sk, &addr);
		release_sock(sk);
		return rtn;
	}
	case SCTP_DEL_IP:
	{
		int rtn;
		struct sockaddr_in addr;
		if (optlen < sizeof(addr))
			return -EINVAL;
		if (copy_from_user(&addr, optval, sizeof(addr)))
			return -EFAULT;
		if (addr.sin_family != AF_INET)
			return -EINVAL;
		if (!sk->sport || addr.sin_port != sk->sport)
			return -EINVAL;
		lock_sock(sk);
		rtn = sctp_del_ip(sk, &addr);
		release_sock(sk);
		return rtn;
	}
	case SCTP_SET_IP:
	{
		int rtn;
		struct sockaddr_in addr;
		if (optlen < sizeof(addr))
			return -EINVAL;
		if (copy_from_user(&addr, optval, sizeof(addr)))
			return -EFAULT;
		if (addr.sin_family != AF_INET)
			return -EINVAL;
		if (!sk->sport || addr.sin_port != sk->sport)
			return -EINVAL;
		lock_sock(sk);
		rtn = sctp_set_ip(sk, &addr);
		release_sock(sk);
		return rtn;
	}
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	case SCTP_PR:
	{
		switch (val) {
		case SCTP_PR_NONE:
		case SCTP_PR_PREFERRED:
		case SCTP_PR_REQUIRED:
			sp->prel = val;
			return (0);
		default:
			return (-EINVAL);
		}
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	case SCTP_LIFETIME:
	{
		sp->life = val * HZ / 1000;
		return (0);
	}
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
	case SCTP_DISPOSITION:
	{
		switch (val) {
		case SCTP_DISPOSITION_NONE:
		case SCTP_DISPOSITION_UNSENT:
		case SCTP_DISPOSITION_SENT:
		case SCTP_DISPOSITION_GAP_ACKED:
		case SCTP_DISPOSITION_ACKED:
			sp->disp = val;
			return (0);
		default:
			return (-EINVAL);
		}
	}
	default:
		return -ENOPROTOOPT;
	}
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP GETSOCKOPT
 *
 *  --------------------------------------------------------------------------
 */
STATIC int
sctp_getsockopt(struct sock *sk, int level, int optname, char *optval, int *optlen)
{
	sctp_t *sp = SCTP_PROT(sk);
	int val, len;
	if (level != SOL_SCTP)
		return ip_getsockopt(sk, level, optname, optval, optlen);
	if (get_user(len, optlen))
		return -EFAULT;
	len = (len < sizeof(int)) ? len : sizeof(int);
	switch (optname) {
	case SCTP_MAXSEG:
		val = sp->user_amps;
		break;
	case SCTP_NODELAY:
		val = (sp->nonagle == 1);
		break;
	case SCTP_CORK:
		val = (sp->nonagle == 2);
		break;
	case SCTP_RECVSID:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVSID;
		break;
	case SCTP_RECVPPI:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVPPI;
		break;
	case SCTP_RECVSSN:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVSSN;
		break;
	case SCTP_RECVTSN:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVTSN;
		break;
	case SCTP_SID:
		val = sp->sid;
		break;
	case SCTP_PPI:
		val = sp->ppi;
		break;
	case SCTP_HB:
	{
		struct sctp_hbitvl hb;
		struct sctp_daddr *sd;
		if (len < sizeof(hb))
			return -EINVAL;
		if (copy_from_user(&hb, optval, sizeof(hb)))
			return -EFAULT;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sp, hb.dest.sin_addr.s_addr))) {
			hb.hb_itvl = (sd->hb_itvl * 1000) / HZ;
			hb.hb_act = sd->hb_act;
			release_sock(sk);
			if (copy_to_user((void *) optval, &hb, sizeof(hb)))
				return -EFAULT;
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_RTO:
	{
		struct sctp_rtoval rv;
		struct sctp_daddr *sd;
		if (len < sizeof(rv))
			return -EINVAL;
		if (copy_from_user(&rv, optval, sizeof(rv)))
			return -EFAULT;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sp, rv.dest.sin_addr.s_addr))) {
			rv.rto_initial = (sd->rto_ini * 1000 / HZ);
			rv.rto_min = (sd->rto_min * 1000 / HZ);
			rv.rto_max = (sd->rto_max * 1000 / HZ);
			rv.max_retrans = sd->max_retrans;
			release_sock(sk);
			if (copy_to_user((void *) optval, &rv, sizeof(rv)))
				return -EFAULT;
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_COOKIE_LIFE:
	{
		val = sp->ck_life * 1000 / HZ;
		break;
	}
	case SCTP_SACK_DELAY:
	{
		val = sp->max_sack * 1000 / HZ;
		break;
	}
	case SCTP_PATH_MAX_RETRANS:
	{
		val = sp->rtx_path;
		break;
	}
	case SCTP_ASSOC_MAX_RETRANS:
	{
		val = sp->max_retrans;
		break;
	}
	case SCTP_MAX_INIT_RETRIES:
	{
		val = sp->max_inits;
		break;
	}
	case SCTP_MAX_BURST:
	{
		val = sp->max_burst;
		break;
	}
	case SCTP_HEARTBEAT_ITVL:
	{
		val = sp->hb_itvl * 1000 / HZ;
		break;
	}
	case SCTP_RTO_INITIAL:
	{
		val = sp->rto_ini * 1000 / HZ;
		break;
	}
	case SCTP_RTO_MIN:
	{
		val = sp->rto_min * 1000 / HZ;
		break;
	}
	case SCTP_RTO_MAX:
	{
		val = sp->rto_max * 1000 / HZ;
		break;
	}
	case SCTP_OSTREAMS:
	{
		val = sp->n_ostr ? sp->n_ostr : sp->req_ostr;
		break;
	}
	case SCTP_ISTREAMS:
	{
		val = sp->n_istr ? sp->n_istr : sp->max_istr;
		break;
	}
	case SCTP_COOKIE_INC:
	{
		val = sp->ck_inc * 1000 / HZ;
		break;
	}
	case SCTP_THROTTLE_ITVL:
	{
		val = sp->throttle * 1000 / HZ;
		break;
	}
	case SCTP_MAC_TYPE:
	{
		val = sp->hmac;
		break;
	}
	case SCTP_CKSUM_TYPE:
	{
		val = sp->cksum;
		break;
	}
	case SCTP_DEBUG_OPTIONS:
	{
		val = sp->debug;
		break;
	}
#ifdef SCTP_CONFIG_ECN
	case SCTP_ECN:
	{
		val = (sp->p_caps & SCTP_CAPS_ECN) ? 1 : 0;
		break;
	}
#endif				/* SCTP_CONFIG_ECN */
#if defined(SCTP_CONFIG_ADD_IP) || defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO)
	case SCTP_ALI:
	{
		val = (sp->p_caps & SCTP_CAPS_ALI) ? sp->p_ali : 0;
		break;
	}
#endif				/* defined(SCTP_CONFIG_ADD_IP) ||
				   defined(SCTP_CONFIG_ADAPTATION_LAYER_INFO) */
#ifdef SCTP_CONFIG_ADD_IP
	case SCTP_ADD:
	{
		val = (sp->p_caps & SCTP_CAPS_ADD_IP) ? 1 : 0;
		break;
	}
	case SCTP_SET:
	{
		val = (sp->p_caps & SCTP_CAPS_SET_IP) ? 1 : 0;
		break;
	}
#endif				/* SCTP_CONFIG_ADD_IP */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	case SCTP_PR:
	{
		val = (sp->p_caps & SCTP_CAPS_PR) ? 1 : 0;
		break;
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
	case SCTP_LIFETIME:
	{
		val = sp->life;
		break;
	}
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
	case SCTP_DISPOSITION:
	{
		val = sp->disp;
		break;
	}
	default:
		return -ENOPROTOOPT;
	}
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;
	return 0;
}

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
STATIC int
sctp_cmsg_send(struct msghdr *msg, int *sid, int *ppi, unsigned long *life, struct ipcm_cookie *ipc)
{
	/* 
	 *  IMPLEMENTATION NOTE:-  I have also set sctp_sendmsg to accept
	 *  ancillary data which can be used to select the stream identifier
	 *  and protocol payload identifier for this message.  Both default to
	 *  that which is set for the socket with setsockopt, or to 0 if not
	 *  set.
	 */
	if (msg->msg_controllen) {
		int ipcmsg = 0;
		int err;
		struct cmsghdr *cmsg;
		for (cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
			if (cmsg->cmsg_len < sizeof(struct cmsghdr)
			    || (unsigned long) (((char *) cmsg - (char *) msg->msg_control)
						+ cmsg->cmsg_len) > msg->msg_controllen)
				return -EINVAL;
			if (cmsg->cmsg_level == SOL_IP)
				ipcmsg = 1;
			if (cmsg->cmsg_level != SOL_SCTP)
				continue;
			switch (cmsg->cmsg_type) {
			case SCTP_SID:
				if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
					return -EINVAL;
				*sid = *((int *) CMSG_DATA(cmsg));
				break;
			case SCTP_PPI:
				if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
					return -EINVAL;
				*ppi = *((int *) CMSG_DATA(cmsg));
				break;
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
			case SCTP_LIFETIME:
				if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
					return -EINVAL;
				*life = *((int *) CMSG_DATA(cmsg));
				break;
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
			default:
				return -EINVAL;
			}
		}
		/* 
		 *  IMPLEMENTATION NOTE:-  I also support the UDP style ancillary data here for ip
		 *  cmsg, but only IP_PKTINFO and not IP_RETOPTS.
		 *
		 *  Gee, yes! You can even specify the interface that you wish the data to be sent
		 *  on.  This allows the user to specify at least the transmission policy for SCTP.
		 *  Note that this SCTP implementation might not bundle DATA chunks that are thus
		 *  directed.
		 */
		if (ipcmsg) {
			if ((err = ip_cmsg_send(msg, ipc)))
				return err;
			if (ipc->opt) {
				kfree(ipc->opt);
				return -EINVAL;
			}
		}
	}
	return (0);
}

/*
 *  WAIT FOR CONNECT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This waitq waits for a socket to become connected.  This function permits sending on a socket
 *  immediately after the connect() call but waits until the socket enters the established state
 *  before
 */
STATIC int
sctp_wait_for_connect(struct sock *sk, long *timeo)
{
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;
	printd(("INFO: Socket %p entering Connect wait queue\n", sk));
	add_wait_queue_exclusive(sk->sleep, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		release_sock(sk);
		if ((1 << sk->state) & ~(SCTPF_ESTABLISHED | SCTPF_COOKIE_ECHOED))
			*timeo = schedule_timeout(*timeo);
		lock_sock(sk);
		/* non-restartable states */
		if ((1 << sk->state) & (SCTPF_ESTABLISHED | SCTPF_COOKIE_ECHOED))
			break;
		if ((1 << sk->state) & (SCTPF_DISCONNECTED | SCTPF_SHUTDOWN_ACK_SENT))
			goto epipe;
		if (signal_pending(current))
			goto eintr;
		if (!*timeo)
			goto eagain;
	}
      out:
	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk->sleep, &wait);
	return err;
      epipe:
	err = -EPIPE;
	goto out;
      eagain:
	err = -EAGAIN;
	goto out;
      eintr:
	err = sock_intr_errno(*timeo);
	goto out;
}

/*
 *  WAIT FOR WRITE MEMORY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This waitq waits for a socket to have available memory to which to write new data.  This is
 *  congestion flow control.
 */
STATIC int
sctp_wait_for_wmem(struct sock *sk, long *timeo)
{
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;
	printd(("INFO: Socket %p entering Write wait queue\n", sk));
	add_wait_queue(sk->sleep, &wait);
	for (;;) {
		set_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);
		set_current_state(TASK_INTERRUPTIBLE);
		if (sk->err || (sk->shutdown & SEND_SHUTDOWN))
			goto epipe;
		if (!*timeo)
			goto eagain;
		if (signal_pending(current))
			goto eintr;
		clear_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);
		if (atomic_read(&sk->wmem_alloc) < sk->sndbuf) {
			clear_bit(SOCK_NOSPACE, &sk->socket->flags);
			break;
		}
		set_bit(SOCK_NOSPACE, &sk->socket->flags);
		release_sock(sk);
		if (atomic_read(&sk->wmem_alloc) >= sk->sndbuf)
			*timeo = schedule_timeout(*timeo);
		lock_sock(sk);
	}
      out:
	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk->sleep, &wait);
	return err;
      epipe:
	err = -EPIPE;
	goto out;
      eagain:
	err = -EAGAIN;
	goto out;
      eintr:
	err = sock_intr_errno(*timeo);
	goto out;
}

/*
 *  SCTP SENDMSG
 *  -----------------------------------
 *  Note: we could probably accept one packet in the opening state so that we can add it to the
 *  outbound COOKIE-ECHO message.  That would be cool.
 */
STATIC int
sctp_sendmsg(struct sock *sk, struct msghdr *msg, int len)
{
	struct iovec *iov;
	struct ipcm_cookie ipc;
	sctp_t *sp = SCTP_PROT(sk);
	struct sctp_daddr *daddr = NULL;
	struct sctp_strm *st = NULL;
	struct sk_buff *skb = NULL, **head;
	struct sk_buff_head *sndq;
	uint32_t saddr = 0;
	int flags, oob, err = 0, bundle, iovlen, copied = 0, sid = 0, ppi = 0, oif =
	    0, *more, *ppip, stream = 0;
	long timeo;
	unsigned long life = sp->life;
	flags = msg->msg_flags;
	if ((flags &
	     ~(MSG_OOB | MSG_DONTROUTE | MSG_DONTWAIT | MSG_NOSIGNAL | MSG_EOR | MSG_MORE |
	       MSG_CONFIRM | MSG_EOF)))
		return -EINVAL;
	/* TODO: need to support MSG_PROBE per documentation.  MSG_PROBE should place data in a
	   heartbeat. */
#ifdef SCTP_CONFIG_UDP_COMPATIBLE
	if ((!(flags & MSG_EOR) || (flags & MSG_MORE)) && sk->type == SOCK_RDM)
		return -EINVAL;	/* more never used for SOCK_RDM */
#endif				/* SCTP_CONFIG_UDP_COMPATIBLE */
	(void) stream;
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
	stream = (sk->type == SOCK_STREAM);
	if (stream) {
		flags &= ~MSG_EOR;
		flags |= MSG_MORE;
	}
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
	if ((1 << sk->state) & (SCTPF_DISCONNECTED))
		return -ENOTCONN;
/*
 *  IMPLEMENTATION NOTE:- I have set sctp_sendmsg() to handle having addresses provided in the same
 *  fashion as udp_sendmsg() as long as the address is one of the destination addresses available to
 *  the association.  This permits the user to specify the destination address of a specific data
 *  message.
 */
	if (msg->msg_name) {
		struct sockaddr_in *addr = (struct sockaddr_in *) msg->msg_name;
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
		if (sk->type == SOCK_STREAM)
			return -EINVAL;
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
		if (msg->msg_namelen < sizeof(*addr))
			return -EINVAL;
		if (addr->sin_family && addr->sin_family != AF_INET)
			return -EINVAL;
		if (addr->sin_port && addr->sin_port != sk->dport)
			return -EINVAL;
		if (addr->sin_addr.s_addr && !(daddr = sctp_find_daddr(sp, addr->sin_addr.s_addr)))
			return -EINVAL;
	}
	lock_sock(sk);
	timeo = sock_sndtimeo(sk, flags & MSG_DONTWAIT);
	/* Wait for connection (or disconnection) to finish. */
	if ((1 << sk->state) & ~(SCTPF_ESTABLISHED | SCTPF_COOKIE_ECHOED))
		if ((err = sctp_wait_for_connect(sk, &timeo)))
			goto out;
	/* This should be in poll */
	clear_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);	/* clear SIGIO XXX */
/*
 *  IMPLEMENTATION NOTE:-  The user can specify the destination address (within those available to
 *  the association), the source address (within those available to the association), and/or the
 *  interface upon which to send the message.  However, for SOCK_STREAM sockets it will then not be
 *  possible to concatenate these bytes into other DATA chunks, and for SOCK_SEQPACKET and SOCK_RDM
 *  sockets it will not be possible to bundle these DATA chunks with DATA chunks for other
 *  destinations.
 */
	/* Handle SCTP and IP ancillary data */
	sid = sp->sid;		/* start with association default */
	ppi = sp->ppi;		/* start with association default */
	bzero(&ipc, sizeof(ipc));
	if ((err = sctp_cmsg_send(msg, &sid, &ppi, &life, &ipc)))
		goto out;
	err = -EINVAL;
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
	if (sid != 0 && stream)
		goto out;
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
	if (sid >= sp->n_ostr)
		goto out;
	if (ipc.addr && sctp_find_saddr(sp, ipc.addr))
		saddr = ipc.addr;
	if (ipc.oif)
		oif = ipc.oif;
	bundle = (!daddr && !saddr);	/* && !oif); later */
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
	if (!bundle && stream)
		goto out;
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
#ifdef SCTP_CONFIG_UDP_COMPATIBLE
	if (sk->type == SOCK_RDM)
		flags |= MSG_OOB;
#endif				/* SCTP_CONFIG_UDP_COMPATIBLE */
	if (!(st = sctp_ostrm_find(sp, sid, &err)))
		goto out;
	if ((oob = (flags & MSG_OOB))) {
		sndq = &sp->urgq;
		more = &st->x.more;
		head = &st->x.head;
		ppip = &st->x.ppi;
	} else {
		sndq = &sk->sndq;
		more = &st->n.more;
		head = &st->n.head;
		ppip = &st->n.ppi;
	}
	if (!bundle && (*more & SCTP_STRMF_MORE))
		goto out;
	/* Commence sending */
	iovlen = msg->msg_iovlen;
	iov = msg->msg_iov;
	copied = 0;
	err = -EPIPE;
	if (sk->err || (sk->shutdown & SEND_SHUTDOWN))
		goto out;
	while (--iovlen >= 0) {
		int seglen = iov->iov_len;
		unsigned char *from = iov->iov_base;
		iov++;
		while (seglen > 0) {
			int copy;
			/* Find out how much data can fit in a DATA chunk. We never fill data
			   chunks to bigger than would fit one to a path MTU. We call this the AMPS 
			   (Association Maximum Payload Size). */
			int amps = sp->amps;
			int dmps = amps - sizeof(struct sctp_data);
			if (!(skb = *head) || (copy = amps - skb->len) <= 0 || !bundle
			    || !(*more & SCTP_STRMF_MORE)) {
				sctp_tcb_t *cb;
				struct sctp_data *m;
			      new_chunk:
				if (!(copy = len))
					goto out;
				/* preallocate more if more data coming */
				if ((flags & MSG_MORE) && copy < dmps)
					copy = dmps;
				if (copy > dmps)
					copy = dmps;
				if (!(skb = sctp_wmalloc(sk, copy + sizeof(*m), 0, sk->allocation)))
					goto wait_for_sndbuf;
				m = (struct sctp_data *) skb_put(skb, sizeof(*m));
				cb = SCTP_SKB_CB(skb);
				cb->skb = skb;
				cb->dlen = 0;
				cb->st = st;
				cb->expires = 0;
				cb->when = jiffies;	/* reset when transmitted */
				cb->daddr = daddr;	/* reset when transmitted */
//                              cb->dif = oif;          /* TODO */
				cb->st = st;
				cb->tsn = 0;	/* assigned later */
				cb->sid = sid;
				cb->ssn = oob ? 0 : (st->ssn + 1) & 0xffff;
				cb->ppi = (*more & SCTP_STRMF_MORE) ? *ppip : ppi;
				cb->flags = ((*more & SCTP_STRMF_MORE)
					     ? 0 : SCTPCB_FLAG_FIRST_FRAG)
				    | (!oob ? 0 : SCTPCB_FLAG_URG);
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
				/* ease reassembly at peer */
				if (stream)
					cb->flags |= SCTPCB_FLAG_FIRST_FRAG | SCTPCB_FLAG_LAST_FRAG;
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
				if (sp->disp == SCTP_DISPOSITION_ACKED || flags & MSG_CONFIRM)
					cb->flags |= SCTPCB_FLAG_CONF;
				m->ch.type = SCTP_CTYPE_DATA;
				m->ch.flags = cb->flags & 0x7;
				m->ch.len = htons(skb->len);
				m->tsn = 0;	/* assigned before transmission */
				m->sid = htons(cb->sid);
				m->ssn = htons(cb->ssn);
				m->ppi = htonl(cb->ppi);
				skb_init_cksum(skb);
				/* don't put on queue till it has data in it */
			}
			if (copy > seglen)
				copy = seglen;
			if (skb_tailroom(skb) <= 0 || skb->len >= amps) {
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
				sctp_tcb_t *cb = SCTP_SKB_CB(skb);
				if (life) {
					cb->expires = jiffies + life;
					if (!sctp_timeout_pending(&sp->timer_life)
					    || sp->timer_life.expires > cb->expires)
						sp_set_timeout(sp, &sp->timer_life, life);
				}
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
				/* push the head buffer go */
				*head = NULL;
				sk->wmem_queued += PADC(skb->len);
				skb_queue_tail(sndq, skb);
				goto new_chunk;
			}
			/* Continue building packet */
			if (copy > skb_tailroom(skb))
				copy = skb_tailroom(skb);
			if (!(err = skb_add_data(skb, from, copy))) {
				/* touch up chunk header */
				sctp_tcb_t *cb = SCTP_SKB_CB(skb);
				struct sctp_data *m = (struct sctp_data *) skb->data;
				cb->dlen = skb->len - sizeof(*m);
				m->ch.len = htons(skb->len);
				/* if fresh buffer queue it now */
				if (!skb->list) {
					bundle = 1;
					*head = skb;
					*more |= SCTP_STRMF_MORE;
					*ppip = cb->ppi;
					skb_queue_tail(sndq, skb);
#if defined(SCTP_CONFIG_LIFETIMES) || defined(SCTP_CONFIG_PARTIAL_RELIABILITY)
					if (life) {
						cb->expires = jiffies + life;
						if (!sctp_timeout_pending(&sp->timer_life)
						    || sp->timer_life.expires > cb->expires)
							sp_set_timeout(sp, &sp->timer_life, life);
					}
#endif				/* defined(SCTP_CONFIG_LIFETIMES) ||
				   defined(SCTP_CONFIG_PARTIAL_RELIABILITY) */
				}
				from += copy;
				copied += copy;
				seglen -= copy;
				len -= copy;
				if (len <= 0 && (!(flags & MSG_MORE) || (flags & MSG_EOR))) {
					*head = NULL;
					sk->wmem_queued += PADC(skb->len);
					*more &= ~SCTP_STRMF_MORE;
					*ppip = 0;
					if (!oob)
						st->ssn = cb->ssn;
					cb->flags |= SCTPCB_FLAG_LAST_FRAG;
					m->ch.flags = cb->flags & 0x7;
					if (flags & MSG_EOF && !(sk->shutdown & SEND_SHUTDOWN)) {
						sk->shutdown |= SEND_SHUTDOWN;
						switch (sk->state) {
						case SCTP_ESTABLISHED:
							sctp_change_state(sk,
									  SCTP_SHUTDOWN_PENDING);
							break;
						case SCTP_SHUTDOWN_RECEIVED:
							sctp_change_state(sk,
									  SCTP_SHUTDOWN_RECVWAIT);
							break;
						}
					}
				} else if (skb->len >= amps) {
					*head = NULL;
					sk->wmem_queued += PADC(skb->len);
					SCTP_INC_STATS_USER(SctpFragUsrMsgs);
				}
				continue;
			} else {
				if (!skb->list)
					kfree_skb(skb);
				goto out;
			}
		      wait_for_sndbuf:
			set_bit(SOCK_NOSPACE, &sk->socket->flags);
			sctp_transmit_wakeup(sk);
			if ((err = sctp_wait_for_wmem(sk, &timeo)))
				goto out;
		}
	}
	if (copied)
		sctp_transmit_wakeup(sk);
	release_sock(sk);
	return copied;
      out:
	if (copied || !err) {
		if (copied)
			sctp_transmit_wakeup(sk);
		release_sock(sk);
		return copied;
	}
	if (err == -EPIPE)
		err = sock_error(sk) ? : -EPIPE;
	if (err == -EPIPE && !(flags & MSG_NOSIGNAL))
		send_sig(SIGPIPE, current, 0);
	release_sock(sk);
	return err;
}

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
STATIC void
sctp_cmsg_recv(struct msghdr *msg, struct sk_buff *skb, unsigned cmsg_flags, unsigned flags)
{
	sctp_tcb_t *cb = SCTP_SKB_CB(skb);
	if (cmsg_flags & SCTP_CMSGF_RECVSID) {
		int sid = cb->sid;
		put_cmsg(msg, SOL_SCTP, SCTP_SID, sizeof(sid), &sid);
	}
	if (cmsg_flags & SCTP_CMSGF_RECVPPI) {
		int ppi = cb->ppi;
		put_cmsg(msg, SOL_SCTP, SCTP_PPI, sizeof(ppi), &ppi);
	}
	if (cmsg_flags & SCTP_CMSGF_RECVSSN) {
		int ssn = cb->ssn;
		put_cmsg(msg, SOL_SCTP, SCTP_SSN, sizeof(ssn), &ssn);
	}
	if (cmsg_flags & SCTP_CMSGF_RECVTSN) {
		int tsn = cb->tsn;
		put_cmsg(msg, SOL_SCTP, SCTP_TSN, sizeof(tsn), &tsn);
	}
	if (flags & MSG_CONFIRM) {
		int dsp;
		if (!(cb->flags & SCTPCB_FLAG_SENT))
			dsp = SCTP_DISPOSITION_UNSENT;
		else if (!(cb->flags & SCTPCB_FLAG_SACKED)
			 && !(cb->flags & SCTPCB_FLAG_ACK))
			dsp = SCTP_DISPOSITION_SENT;
		else if (!(cb->flags & SCTPCB_FLAG_ACK))
			dsp = SCTP_DISPOSITION_GAP_ACKED;
		else
			dsp = SCTP_DISPOSITION_ACKED;
		put_cmsg(msg, SOL_SCTP, SCTP_DISPOSITION, sizeof(dsp), &dsp);
	}
}
STATIC void
sctp_name_recv(struct sock *sk, struct msghdr *msg, struct sk_buff *skb)
{
	struct sockaddr_in *sin = (struct sockaddr_in *) msg->msg_name;
	if (sin) {
		sin->sin_family = AF_INET;
		sin->sin_port = SCTP_SKB_SH(skb)->srce;
		sin->sin_addr.s_addr = skb->nh.iph->saddr;
		bzero(&sin->sin_zero, sizeof(sin->sin_zero));
		/* why doesn't udp do this? : */
		msg->msg_namelen = sizeof(*sin);
	}
}

STATIC long
sctp_data_wait(struct sock *sk, long timeo)
{
	sctp_t *sp = SCTP_PROT(sk);
	DECLARE_WAITQUEUE(wait, current);
	printd(("INFO: Socket %p entering Read wait queue\n", sk));
	add_wait_queue(sk->sleep, &wait);
	set_current_state(TASK_INTERRUPTIBLE);
	set_bit(SOCK_ASYNC_WAITDATA, &sk->socket->flags);
	release_sock(sk);
	/* release clears backlog, so check again */
	if (!sp->rmem_queued)
		timeo = schedule_timeout(timeo);
	lock_sock(sk);
	clear_bit(SOCK_ASYNC_WAITDATA, &sk->socket->flags);
	remove_wait_queue(sk->sleep, &wait);
	set_current_state(TASK_RUNNING);
	return timeo;
}

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
STATIC int
sctp_recvmsg(struct sock *sk, struct msghdr *msg, int len, int noblock, int flags, int *addr_len)
{
	sctp_t *sp = SCTP_PROT(sk);
	struct sk_buff_head *head;
	struct sk_buff *skb = NULL;
	int err, target, sid, copied = 0, used, avail;
	long timeo;
	if ((err = sock_error(sk)))
		return (err);
	if ((1 << sk->state) & (SCTPF_DISCONNECTED) && !(flags & MSG_CONFIRM))
		return (-ENOTCONN);
	if (flags &
	    ~(MSG_TRUNC | MSG_OOB | MSG_PEEK | MSG_DONTWAIT | MSG_WAITALL | MSG_NOSIGNAL |
	      MSG_CONFIRM))
		return (-EINVAL);
	lock_sock(sk);
	if (flags & MSG_CONFIRM) {
		timeo = 0;
		if (!(skb = skb_peek(&sp->ackq))) {
			err = -EINVAL;
			goto out;
		}
		msg->msg_flags |= MSG_CONFIRM;
		head = &sp->ackq;
		target = 0;
	} else if (flags & MSG_OOB) {
		timeo = 0;
		if (sk->urginline || !(skb = skb_peek(&sp->expq))) {
			err = -EINVAL;
			goto out;
		}
		msg->msg_flags |= MSG_OOB;
		head = &sp->expq;
		target = 0;
	} else {
		timeo = sock_rcvtimeo(sk, (flags & MSG_DONTWAIT));
		while ((!sk->urginline || !(skb = skb_peek(&sp->expq)))
		       && !(skb = skb_peek(&sk->rcvq))) {
			if (!timeo) {
				err = -EAGAIN;
				goto out;
			}
			if ((err = sock_error(sk)))
				goto out;
			if (((1 << sk->state) & (SCTPF_DISCONNECTED))
			    || (sk->shutdown & RCV_SHUTDOWN)) {
				err = -ENOTCONN;
				goto out;
			}
			/* unreadable urgent data exists */
			if (!sk->urginline && !skb_queue_empty(&sp->expq))
				goto out;
			if (signal_pending(current)) {
				err = timeo ? sock_intr_errno(timeo) : -EAGAIN;
				goto out;
			}
			timeo = sctp_data_wait(sk, timeo);
		}
		if (SCTP_SKB_CB(skb)->flags & SCTPCB_FLAG_URG)
			msg->msg_flags |= MSG_OOB;
		head = skb->list;
		target = sock_rcvlowat(sk, flags & MSG_WAITALL, len);
	}
	sid = SCTP_SKB_CB(skb)->sid;
	do {
		struct sk_buff *next_skb;
		do {
			sctp_tcb_t *cb = SCTP_SKB_CB(skb);
			/* work the list on one stream id only */
			if (cb->sid != sid)
				goto out;
			used = avail = skb->len;
			if (len < used) {
				used = len;
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
				if (sk->type != SOCK_STREAM)
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
					msg->msg_flags |= MSG_TRUNC;
				if (!(flags & MSG_TRUNC))
					avail = used;
			}
			printd(("INFO: Reading len %d, used %d, avail %d\n", len, used, avail));
			err = skb_copy_datagram_iovec(skb, 0, msg->msg_iov, used);
			if (err)
				goto out;
			copied += avail;
			if (cb->flags & SCTPCB_FLAG_LAST_FRAG) {
				if (used == skb->len)
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
					if (sk->type != SOCK_STREAM) {
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
						msg->msg_flags |= MSG_EOR;
						len = used;	/* stop reading */
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
					}
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
				if (!(flags & MSG_CONFIRM))
					if (!(cb)->flags & SCTPCB_FLAG_FIRST_FRAG)
						SCTP_INC_STATS_USER(SctpReasmUsrMsgs);
			}
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
			if (sk->type != SOCK_STREAM) {
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
				if (sk->rcvtstamp || msg->msg_name
				    || sk->protinfo.af_inet.cmsg_flags || sp->cmsg_flags
				    || (flags & MSG_CONFIRM)) {
					if (sk->rcvtstamp)
						sock_recv_timestamp(msg, sk, skb);
					if (msg->msg_name)
						sctp_name_recv(sk, msg, skb);
					if (sk->protinfo.af_inet.cmsg_flags)
						ip_cmsg_recv(msg, skb);
					if (sp->cmsg_flags || (flags & MSG_CONFIRM))
						sctp_cmsg_recv(msg, skb, sp->cmsg_flags, flags);
					len = used;	/* stop reading */
				}
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
			}
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
			next_skb = skb_next(skb);
			if (!(flags & MSG_PEEK)) {
				if (used < skb->len
#ifdef SCTP_CONFIG_UDP_COMPATIBLE
				    && sk->type != SOCK_RDM
#endif				/* SCTP_CONFIG_UDP_COMPATIBLE */
				    ) {
					/* don't discard on MSG_TRUNC */
					__skb_pull(skb, used);
				} else {
					skb_unlink(skb);
					if (!(flags & MSG_CONFIRM)) {
						sp->rmem_queued -= cb->dlen;
						ensure(sp->rmem_queued >= 0, sp->rmem_queued = 0);
					}
					kfree_skb(skb);
				}
			}
			len -= used;
		} while ((skb = next_skb) && len > 0);
		if (len > 0) {
			if (head == &sp->expq)
				goto out;
			while (!(skb = skb_peek(head))) {
				if (copied >= target && sk->backlog.tail == NULL)
					goto out;
				if (copied) {
					if (sk->err || sk->state == SCTP_CLOSED
					    || (sk->shutdown & RCV_SHUTDOWN) || !timeo
					    || (flags & MSG_PEEK) || signal_pending(current)
					    || !skb_queue_empty(&sp->expq))
						goto out;
				} else {
					if (sk->done)
						goto out;
					if (sk->err) {
						err = sock_error(sk);
						goto out;
					}
					if (((1 << sk->state) & (SCTPF_DISCONNECTED))
					    || sk->shutdown & RCV_SHUTDOWN) {
						err = -ENOTCONN;
						goto out;
					}
					if (signal_pending(current)) {
						err = timeo ? sock_intr_errno(timeo) : -EAGAIN;
						goto out;
					}
				}
				if (copied >= target) {
					/* just want to process backlog, not block */
					release_sock(sk);
					lock_sock(sk);
				} else {
					timeo = sctp_data_wait(sk, timeo);
					if (!timeo) {
						err = -EAGAIN;
						goto out;
					}
					if (!skb_queue_empty(&sp->expq))
						goto out;
				}
			}
		}
	} while (len > 0);
      out:
	if (copied) {
		/* SACK if the window opened */
		sctp_transmit_wakeup(sk);
		err = copied;
	}
	release_sock(sk);
	return (err);
}

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
STATIC int
sctp_bind(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	struct sockaddr_in *ssin = (struct sockaddr_in *) uaddr;
	uint16_t sport = ssin->sin_port;
	int snum = addr_len / sizeof(*ssin);
	if (addr_len >= sizeof(*uaddr) && ssin->sin_family == AF_UNSPEC)
		return sctp_unbind_req(sk);
	return sctp_bind_req(sk, sport, ssin, snum, 0);
}

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
STATIC int sctp_rcv_ootb(struct sk_buff *skb);
STATIC int
sctp_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
#ifdef CONFIG_FILTER
#if defined HAVE_SK_FILTER_2_ARGS
	if (sk->filter && sk_filter(skb, sk->filter))
#elif defined HAVE_SK_FILTER_3_ARGS
	if (sk->filter && sk_filter(sk, skb, 1))
#else
#error HAVE_SK_FILTER_2_ARGS or HAVE_SK_FILTER_3_ARGS must be defined.
#endif
		goto discard;
#endif				/* CONFIG_FILTER */
	if (sk->state != SCTP_CLOSED) {
		sctp_recv_msg(sk, skb);
		sctp_transmit_wakeup(sk);
		return (0);
	} else {
		/* the socket may have closed with a backlog */
		struct sock *nsk;
		if ((nsk = sctp_lookup(SCTP_SKB_SH(skb), skb->nh.iph->daddr, skb->nh.iph->saddr))) {
			bh_lock_sock(nsk);
#if defined HAVE_IPSEC_SK_POLICY_EXPORT || defined HAVE_IPSEC_SK_POLICY_ADDR
			if (!ipsec_sk_policy(sk, skb)) {
				bh_unlock_sock(nsk);
				goto discard;
			}
#endif				/* defined HAVE_IPSEC_SK_POLICY_EXPORT || defined
				   HAVE_IPSEC_SK_POLICY_ADDR */
#if defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined HAVE___XFRM_POLICY_CHECK_ADDR
			if (!xfrm4_policy_check(sk, XFRM_POLICY_IN, skb)) {
				bh_unlock_sock(nsk);
				goto discard;
			}
#endif				/* defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined
				   HAVE___XFRM_POLICY_CHECK_ADDR */
			if (sock_locked(nsk))
				sk_add_backlog(nsk, skb);
			else
				sctp_backlog_rcv(nsk, skb);
			bh_unlock_sock(nsk);
			return (0);
		}
		SCTP_INC_STATS_BH(SctpOutOfBlues);
#ifndef SCTP_CONFIG_DISCARD_OOTB
		return sctp_rcv_ootb(skb);
#endif				/* SCTP_CONFIG_DISCARD_OOTB */
	}
      discard:
	kfree_skb(skb);
	return (0);
}

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP HASH
 *
 *  ---------------------------------------------------------------------------
 *  This function is never called by the common inet socket code.  Ala TCP, we hash as required
 *  elsewhere.
 */
STATIC void
sctp_hash(struct sock *sk)
{
	sctp_t *sp = SCTP_PROT(sk);
	switch (sk->state) {
	case SCTP_ESTABLISHED:
		local_bh_disable();
		__sctp_phash_insert(sp);
		__sctp_thash_insert(sp);
		__sctp_vhash_insert(sp);
		local_bh_enable();
		return;
	case SCTP_LISTEN:
		/* this is the first place we see listening sockets */
		/* want just the listening hash */
		local_bh_disable();
		__sctp_lhash_insert(sp);
		local_bh_enable();
		return;
	case SCTP_COOKIE_WAIT:
		/* want established and tcb but not icmp */
		local_bh_disable();
		__sctp_thash_insert(sp);
		__sctp_vhash_insert(sp);
		local_bh_enable();
		return;
	case SCTP_CLOSED:
		/* If we are CLOSED, then the only hash we can live in is the bind hash.  We are
		   added to the bind hash with get_port only.  This is so that noone steals our
		   hash bind bucket after we assign a port number.  This might be called, however,
		   by inet_bind (for historical reasons, I believe). */
		return;
	}
	/* bad news */
	swerr();
}

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
STATIC int
s_conn_ind(struct sock *sk, struct sk_buff *cp)
{
	sctp_t *sp = SCTP_PROT(sk);
	skb_queue_tail(&sp->conq, skb_get(cp));
	sk->ack_backlog++;
	if (!sk->dead) {
		sk->data_ready(sk, 0);
		return (0);
	}
	swerr();
	return (0);
}

/*
 *  S_CONN_CON		- connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated are the number of inbound and
 *  outbound streams.
 */
STATIC int
s_conn_con(struct sock *sk)
{
	if (!sk->dead)
		sk->state_change(sk);
	else
		swerr();
	if (sk->socket)
		sk_wake_async(sk, 0, POLL_OUT);
	return (0);
}

/*
 *  S_DATA_IND		- data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default socket.  The
 *  caller should check that ppi and sid match the default before using this indication.  Otherwise
 *  the caller should use S_OPTDATA_IND.  Well, for sockets we always use the equivalent of
 *  S_OPTDATA_IND.  This just indicates availability of data to the user under sockets and whether
 *  to include ancilliary information is made when the data is actually read with recvmsg(2).
 */
STATIC int
s_data_ind(struct sock *sk, size_t len)
{
	if (!sk->dead) {
		sk->data_ready(sk, len);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  S_EXDATA_IND	- expedited data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default socket.  The
 *  caller should check that ppi and sid match the default before using this indication.  Otherwise
 *  the caller should use S_OPTDATA_IND.  Well, for sockets we always use the equivalent of
 *  S_OPTDATA_IND.  This just indicates availability of data to the user under sockets and whether
 *  to include ancilliary information is made when the data is actually read with recvmsg(2).
 */
STATIC int
s_exdata_ind(struct sock *sk, size_t len)
{
	if (!sk->dead) {
		if (sk->proc != 0) {
			if (sk->proc > 0)
				kill_proc(sk->proc, SIGURG, 1);
			else
				kill_pg(-sk->proc, SIGURG, 1);
			sk_wake_async(sk, 3, POLL_PRI);
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  S_DATACK_IND	- data acknolwedgement indication
 *  -----------------------------------------------------------------
 */
STATIC int
s_datack_ind(struct sock *sk, size_t len)
{
	if (!sk->dead) {
		sk->data_ready(sk, len);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  S_RESET_IND		- reset indication
 *  -----------------------------------------------------------------
 */
STATIC int
s_reset_ind(struct sock *sk, long reason, struct sk_buff *cp)
{
	if (!sk->dead) {
		sctp_error_report(sk, ECONNRESET);
		sk->state_change(sk);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  S_RESET_CON		- reset confirmation
 *  -----------------------------------------------------------------
 */
STATIC int
s_reset_con(struct sock *sk)
{
	sctp_change_state(sk, SCTP_ESTABLISHED);
	if (!sk->dead)
		sk->state_change(sk);
	else
		swerr();
	if (sk->socket)
		sk_wake_async(sk, 0, POLL_OUT);
	return (0);
}

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
STATIC int
s_discon_ind(struct sock *sk, ulong orig, long reason, struct sk_buff *seq)
{
	sctp_t *sp = SCTP_PROT(sk);
	int err = (reason < 0) ? -reason : reason;
	(void) orig;
	if (seq) {
		skb_unlink(seq);
		kfree_skb(seq);
		sk->ack_backlog--;
	}
	if ((sk->err = err) && !sk->dead)
		sk->error_report(sk);
	sk->shutdown = SHUTDOWN_MASK;
	sp->sackf = 0;
	if (!sk->dead)
		sk->state_change(sk);
	else
		sctp_destroy_orphan(sk);
	return (0);
}

/*
 *  S_ORDREL_IND	- orderly release indication
 *  -----------------------------------------------------------------
 */
STATIC int
s_ordrel_ind(struct sock *sk)
{
	sk->shutdown |= RCV_SHUTDOWN;
	sk->done = 1;
	if (!sk->dead) {
		sk->state_change(sk);
		if (sk->shutdown == SHUTDOWN_MASK && sk->state == SCTP_CLOSED) {
			if (sk->socket)
				sk_wake_async(sk, 1, POLL_HUP);
		} else {
			if (sk->socket)
				sk_wake_async(sk, 1, POLL_IN);
		}
	}
	return (0);
}

/*
 *  S_RETR_IND		- retrieval indication
 *  -----------------------------------------------------------------
 */
STATIC int
s_retr_ind(struct sock *sk, struct sk_buff *dp)
{
	if (!sk->dead) {
		sk->data_ready(sk, dp->len);
		return (0);
	}
	swerr();
	return (-EFAULT);
}

/*
 *  S_RETR_CON		- retrieval confirmation
 *  -----------------------------------------------------------------
 */
STATIC int
s_retr_con(struct sock *sk)
{
	return (0);		/* nothing to indicate really */
}

/*
 *  NOTES:- Sockets and XTI/TPI cannot do data acknowledgements, resets or retrieval directly.  Data
 *  acknowledgements and retrieval are different forms of the same service.  For data
 *  acknowledgement, use the STREAMS NPI interface.  For reset support (SCTP Restart indication
 *  different from SCTP CDI), use the STREAMS NPI interface.
 */
STATIC int
sctp_s_conn_ind(struct sock *sk, struct sk_buff *cp)
{
	_printd(("=========================================\n"));
	_printd(("S_CONN_IND ->\n"));
	return s_conn_ind(sk, cp);
}
STATIC int
sctp_s_conn_con(struct sock *sk)
{
	_printd(("=========================================\n"));
	_printd(("S_CONN_CON ->\n"));
	return s_conn_con(sk);
}
STATIC int
sctp_s_data_ind(struct sock *sk, size_t len, int ord)
{
	if (ord) {
		_printd(("=========================================\n"));
		_printd(("S_DATA_IND ->\n"));
		return s_data_ind(sk, len);
	} else {
		_printd(("=========================================\n"));
		_printd(("S_EXDATA_IND ->\n"));
		return s_exdata_ind(sk, len);
	}
}
STATIC int
sctp_s_datack_ind(struct sock *sk, size_t len)
{
	_printd(("=========================================\n"));
	_printd(("S_DATACK_IND -->\n"));
	return s_datack_ind(sk, len);
}
STATIC int
sctp_s_reset_ind(struct sock *sk, ulong orig, long reason, struct sk_buff *cp)
{
	_printd(("=========================================\n"));
	_printd(("S_RESET_IND -->\n"));
	(void) orig;
	return s_reset_ind(sk, reason, cp);
}
STATIC int
sctp_s_reset_con(struct sock *sk)
{
	_printd(("=========================================\n"));
	_printd(("S_RESET_CON -->\n"));
	return s_reset_con(sk);
}
STATIC int
sctp_s_discon_ind(struct sock *sk, ulong origin, long reason, struct sk_buff *cp)
{
	_printd(("=========================================\n"));
	_printd(("S_DISCON_IND -->\n"));
	return s_discon_ind(sk, origin, reason, cp);
}
STATIC int
sctp_s_ordrel_ind(struct sock *sk)
{
	_printd(("=========================================\n"));
	_printd(("S_ORDREL_IND -->\n"));
	return s_ordrel_ind(sk);
}
STATIC int
sctp_s_retr_ind(struct sock *sk, struct sk_buff *dp)
{
	_printd(("=========================================\n"));
	_printd(("S_RETR_IND -->\n"));
	return s_retr_ind(sk, dp);
}
STATIC int
sctp_s_retr_con(struct sock *sk)
{
	_printd(("=========================================\n"));
	_printd(("S_RETR_CON -->\n"));
	return s_retr_con(sk);
}

STATIC struct sctp_ifops s_ops = {
	.conn_ind = &sctp_s_conn_ind,
	.conn_con = &sctp_s_conn_con,
	.data_ind = &sctp_s_data_ind,
	.datack_ind = &sctp_s_datack_ind,
	.reset_ind = &sctp_s_reset_ind,
	.reset_con = &sctp_s_reset_con,
	.discon_ind = &sctp_s_discon_ind,
	.ordrel_ind = &sctp_s_ordrel_ind,
	.retr_ind = &sctp_s_retr_ind,
	.retr_con = &sctp_s_retr_con,
};

/*
 *  ==========================================================================
 *
 *  IP PROTOCOL INTERFACE
 *
 *  ==========================================================================
 */
/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP RCV OOTB
 *
 *  --------------------------------------------------------------------------
 */
STATIC int
sctp_rcv_ootb(struct sk_buff *skb)
{
	struct iphdr *iph = SCTP_IPH(skb);
	struct sctphdr *sh = SCTP_SH(skb);
	struct sctpchdr *ch = SCTP_CH(skb);
	int sat = inet_addr_type(iph->saddr);
	struct sock *sk;
	ensure(skb, return (-EFAULT));
	if (sat != RTN_UNICAST && sat != RTN_LOCAL) {
		/* RFC 2960 8.4(1). */
		kfree_skb(skb);
		return (0);
	}
#if 0
	/* We shouldn't have to range check the chunk header here because it the first chunk header 
	   in an ootb package is already checked by the bottom end receive function. */
	{
		size_t clen;
		ch = (typeof(ch)) mp->b_rptr;
		if (sizeof(*ch) > skb->len || (clen = ntohs(ch->len)) < sizeof(*ch)
		    || PADC(clen) + mp->b_rptr > mp->b_wptr) {
			freemsg(mp);
			return (0);
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
	case SCTP_CTYPE_COOKIE_ECHO:	/* RFC 2960 8.4(4) and (8). */
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
		}
		if (!(sk = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))) {
			printd(("Do not have TCB, aborting\n"));
			sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		} else {
			rare();
			printd(("Have TCB, not aborting\n"));
			sock_put(sk);
		}
		break;
	case SCTP_CTYPE_SHUTDOWN_ACK:
		printd(("Received OOTB SHUTDOWN-ACK\n"));
		if (!(sk = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))) {
			printd(("Do not have TCB, aborting\n"));
			sctp_send_shutdown_complete_ootb(iph->saddr, iph->daddr, sh);
		} else {
			rare();
			printd(("Have TCB, not aborting\n"));
			sock_put(sk);
		}
		break;
	}
	kfree_skb(skb);
	return (0);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP ERR
 *
 *  --------------------------------------------------------------------------
 *  This is the error handler.  We have received an ICMP error for the protocol number.  This
 *  routine is called by the ICMP module when it gets some sort of error condition.  It err < 0 then
 *  the socket should be closed and the error returned to the user.  If err > 0 it's just the icmp
 *  type << 8 | icmp code.  After adjustment header points to the first 8 bytes of the SCTP Common
 *  Header.  We need to find the appropriate port or verification tag.
 *
 *  For sockets, the locking strategy used here is very "optimistic".  sctp_recv_err is called
 *  directly.  When someone else accesses the socket the ICMP is just dropped and for some paths
 *  there is no check at all.  A more general error queue to queue errors for later handling is
 *  probably better.
 *
 *  For STREAMS, because we don't want any races here, we place a M_ERROR message on the read queue
 *  of the stream to which the message applies.  This distinguishes it from M_DATA messages.  It is
 *  processed within the stream with queues locked by sctp_recv_err when the M_ERROR message is
 *  dequeued and processed.  We have to copy the information because the skb will go away after this
 *  call.  Because we check for flow control, this approach is also more resilient agains ICMP
 *  flooding attacks.
 */
__SCTP_STATIC void
sctp_v4_err(struct sk_buff *skb, uint32_t info)
{
	struct sock *sk;
	struct sctphdr *sh;
	struct iphdr *iph = (struct iphdr *) skb->data;
	size_t ihl;
	ensure(skb, return);
#define ICMP_MIN_LENGTH 8
	if (skb->len < (ihl = iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
	sh = (struct sctphdr *) (skb->data + ihl);
	if (sh->v_tag)
		sk = sctp_lookup_ptag(sh->v_tag, sh->srce, sh->dest, iph->saddr, iph->daddr);
	else
		sk = sctp_lookup_tcb(sh->srce, sh->dest, iph->saddr, iph->daddr);
	_usual(sk);
	if (!sk)
		goto no_socket;
	bh_lock_sock(sk);
	if (sk->state == SCTP_CLOSED)
		goto closed;
	/* For sockets we call sctp_recv_err directly.  For STREAMS we create and queue an mblk
	   M_ERROR message that is placed on the stream's lower read queue. */
	{
		sctp_recv_err(sk, skb);
	}
	goto discard_and_put;
      discard_and_put:
	bh_unlock_sock(sk);
	sock_put(sk);
	return;
      closed:
	ptrace(("ERROR: ICMP for closed socket\n"));
	goto discard_and_put;
      no_socket:
	ptrace(("ERROR: could not find socket for ICMP message\n"));
	goto drop;
      drop:
	ICMP_INC_STATS_BH(IcmpInErrors);
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP RCV
 *
 *  -------------------------------------------------------------------------
 *  This is the received frame handler for SCTP IPv4.  All packets received by IPv4 with the
 *  protocol number IPPROTO_SCTP will arrive here first.  We should be performing the Adler-32
 *  checksum on the packet.  If the Adler-32 checksum fails then we should silently discard the
 *  packet per RFC 2960.
 */

__SCTP_STATIC int
sctp_v4_rcv(struct sk_buff *skb)
{
	int ret = 0;
	struct sock *sk;
	struct sctphdr *sh;
	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	SCTP_INC_STATS_BH(SctpInSCTPPacks);
	/* For now...  We should actually place non-linear fragments into seperate mblks and pass
	   them up as a chain, or deal with non-linear sk_buffs directly.  As it winds up, the
	   netfilter hooks linearize anyway. */
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
	/* pull up the ip header */
	sh = SCTP_SKB_SH(skb);
	/* sanity check length and first chunk header */
	{
		size_t clen;
		struct sctpchdr *ch;
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
	/* we do the lookup before the checksum */
	{
		struct iphdr *iph = skb->nh.iph;
		printd(("%s: skb %p lookup up stream\n", __FUNCTION__, skb));
		if (!(sk = sctp_lookup(sh, iph->daddr, iph->saddr)))
			goto no_sctp_socket;
	}
	printd(("%s: skb %p locking socket %p\n", __FUNCTION__, skb, sk));
	bh_lock_sock(sk);
	/* perform the socket-specific checksum */
	skb->csum = ntohl(sh->check);
	if (!(skb->dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM))) {
		sh->check = 0;
		if (!cksum_sk_verify(sk, skb->csum, sh, skb->len + sizeof(*sh))) {
			sh->check = htonl(skb->csum);
			bh_unlock_sock(sk);
			sock_put(sk);
			goto bad_checksum;
		}
		sh->check = htonl(skb->csum);
	}
#if SOCKETS
	/* TODO: We have no easy way of performing IPSEC security under STREAMS, so we leave this
	   out for the moment.  Below is the sockets version. */
#if defined HAVE_IPSEC_SK_POLICY_EXPORT || defined HAVE_IPSEC_SK_POLICY_ADDR
	if (!ipsec_sk_policy(sk, skb))
		goto failed_security;
#endif				/* defined HAVE_IPSEC_SK_POLICY_EXPORT || defined
				   HAVE_IPSEC_SK_POLICY_ADDR */
#if defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined HAVE___XFRM_POLICY_CHECK_ADDR
	if (!xfrm4_policy_check(sk, XFRM_POLICY_IN, skb))
		goto failed_security;
#endif				/* defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined
				   HAVE___XFRM_POLICY_CHECK_ADDR */
#endif				/* SOCKETS */
	skb->dev = NULL;
	if (sock_locked(sk))
		sk_add_backlog(sk, skb);
	else
		ret = sctp_backlog_rcv(sk, skb);
	/* all done */
	_printd(("%s: skb %p put to socket %p\n", __FUNCTION__, skb, sk));
	bh_unlock_sock(sk);
	sock_put(sk);
	return (ret);
      bad_checksum:
	SCTP_INC_STATS_BH(SctpChecksumErrors);
	ptrace(("ERROR: Bad checksum\n"));
	goto free_it;
      no_sctp_socket:
	ptrace(("ERROR: No socket\n"));
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
#if SOCKETS
	/* Another IPSEC policy check that we cannot perform on STREAMS yet. */
#if defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined HAVE___XFRM_POLICY_CHECK_ADDR
	if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb))
		goto discard_it;
#endif				/* defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined
				   HAVE___XFRM_POLICY_CHECK_ADDR */
#endif				/* SOCKETS */
#ifndef SCTP_CONFIG_DISCARD_OOTB
	/* RFC 2960 Section 8.4 */
	return sctp_rcv_ootb(skb);
#endif				/* SCTP_CONFIG_DISCARD_OOTB */
	goto free_it;
      free_it:
	ptrace(("ERROR: Discarding message\n"));
	/* free skb in sockets, free mp in streams */
	kfree_skb(skb);
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
	kfree_skb(skb);
	return (0);
	goto failed_security;	/* shut up compiler */
      failed_security:
	ptrace(("ERROR: Packet failed security policy\n"));
	goto discard_and_putsk;
      discard_and_putsk:
	bh_unlock_sock(sk);
	sock_put(sk);
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
	if (!(in_dev = __in_dev_get(dev)))
		goto done;
	switch (msg) {
	case NETDEV_UP:
	case NETDEV_REBOOT:
	{
		struct sock *sk;
		struct in_ifaddr *ifa;
		spin_lock(&sctp_protolock);
		for (sk = sctp_protolist; sk; sk = sk->next) {
			sctp_t *sp = SCTP_PROT(sk);
			bh_lock_sock(sk);
			/* only do ADD-IP on established sockets */
			if (sk->state != SCTP_ESTABLISHED)
				continue;
			/* peer must be capable of ADD-IP */
			if (sp->p_caps & SCTP_CAPS_ADD_IP)
				continue;
			/* socket must be wildcard bound */
			if (sk->userlocks & SOCK_BINDADDR_LOCK)
				continue;
			read_lock(&in_dev->lock);
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
			read_unlock(&in_dev->lock);
			sctp_transmit_wakeup(sk);
			bh_unlock_sock(sk);
		}
		spin_unlock(&sctp_protolock);
	}
	case NETDEV_DOWN:
	case NETDEV_GOING_DOWN:
	{
		struct sock *sk;
		struct in_ifaddr *ifa;
		spin_lock(&sctp_protolock);
		for (sk = sctp_protolist; sk; sk = sk->next) {
			sctp_t *sp = SCTP_PROT(sk);
			bh_lock_sock(sk);
			/* only do ADD-IP on established sockets */
			if (sk->state != SCTP_ESTABLISHED)
				continue;
			/* peer must be capable of ADD-IP */
			if (sp->p_caps & SCTP_CAPS_ADD_IP)
				continue;
			/* socket must be wildcard bound */
			if (sk->userlocks & SOCK_BINDADDR_LOCK)
				continue;
			read_lock(&in_dev->lock);
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
			read_unlock(&in_dev->lock);
			sctp_transmit_wakeup(sk);
			bh_unlock_sock(sk);
		}
		spin_unlock(&sctp_protolock);
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
#ifdef SOCKETS
STATIC void
get_sctp_sock(struct sock *sk, char *tmpbuf, int i)
{
	unsigned int daddr, saddr;
	uint16_t dport, sport;
	int timer_active;
	unsigned long timer_expires;
	sctp_t *sp = SCTP_PROT(sk);
	daddr = sk->daddr;
	saddr = sk->rcv_saddr;
	dport = ntohs(sk->dport);
	sport = ntohs(sk->sport);
	if (sctp_timeout_pending(&sp->timer_init)) {
		timer_active = 1;
		timer_expires = sp->timer_init.expires;
	} else if (sctp_timeout_pending(&sp->timer_cookie)) {
		timer_active = 2;
		timer_expires = sp->timer_cookie.expires;
	} else if (sctp_timeout_pending(&sp->timer_asconf)) {
		timer_active = 3;
		timer_expires = sp->timer_asconf.expires;
	} else if (sctp_timeout_pending(&sp->timer_shutdown)) {
		timer_active = 4;
		timer_expires = sp->timer_shutdown.expires;
	} else if (sctp_timeout_pending(&sp->timer_guard)) {
		timer_active = 5;
		timer_expires = sp->timer_guard.expires;
	} else if (sctp_timeout_pending(&sp->timer_life)) {
		timer_active = 6;
		timer_expires = sp->timer_life.expires;
	} else if (sctp_timeout_pending(&sp->timer_sack)) {
		timer_active = 7;
		timer_expires = sp->timer_sack.expires;
	} else {
		timer_active = 0;
		timer_expires = jiffies;
	}
	sprintf(tmpbuf,
		"%4d: %08X:%04X %08X:%04X " " %02X %08X:%08X %02X:%08lX %08X %5d %8d %lu %d %p", i,
		saddr, sport, daddr, dport, sk->state, atomic_read(&sk->wmem_alloc),
		atomic_read(&sk->rmem_alloc), timer_active, timer_expires - jiffies,
		sp->retransmits, sock_i_uid(sk), 0, sock_i_ino(sk), atomic_read(&sk->refcnt), sk);
}

/*
 *  Use this as a replacement for afinet_get_info.
 */
__SCTP_STATIC struct proto sctp_prot;
static int
fold_prot_inuse(struct proto *proto)
{
	int res = 0;
	int cpu;
	for (cpu = 0; cpu < smp_num_cpus; cpu++)
		res += proto->stats[cpu_logical_map(cpu)].inuse;
	return res;
}
__SCTP_STATIC int
__os7_afinet_get_info(char *buffer, char **start, off_t offset, int length)
{
	int len = socket_get_info(buffer, start, offset, length);
	len +=
	    sprintf(buffer + len, "SCTP: inuse %d orphan %d alloc %d\n",
		    fold_prot_inuse(&sctp_prot), atomic_read(&sctp_orphan_count),
		    atomic_read(&sctp_socket_count));
	len +=
	    sprintf(buffer + len, "TCP: inuse %d orphan %d tw %d alloc %d mem %d\n",
		    fold_prot_inuse(&tcp_prot), atomic_read(&tcp_orphan_count), tcp_tw_count,
		    atomic_read(&tcp_sockets_allocated), atomic_read(&tcp_memory_allocated));
	len += sprintf(buffer + len, "UDP: inuse %d\n", fold_prot_inuse(&udp_prot));
	len += sprintf(buffer + len, "RAW: inuse %d\n", fold_prot_inuse(&raw_prot));
	len +=
	    sprintf(buffer + len, "FRAG: inuse %d memory %d\n", ip_frag_nqueues,
		    atomic_read(&ip_frag_mem));
	if (offset >= len) {
		*start = buffer;
		return 0;
	}
	*start = buffer + offset;
	len -= offset;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

static unsigned long
fold_field(unsigned long *begin, int sz, int nr)
{
	unsigned long res = 0;
	int i;
	sz /= sizeof(unsigned long);
	for (i = 0; i < smp_num_cpus; i++) {
		res += begin[2 * cpu_logical_map(i) * sz + nr];
		res += begin[(2 * cpu_logical_map(i) + 1) * sz + nr];
	}
	return res;
}
__SCTP_STATIC int
__os7_snmp_get_info(char *buffer, char **start, off_t offset, int length)
{
	int len = snmp_get_info(buffer, start, offset, length), i;
	len +=
	    sprintf(buffer + len,
		    "Sctp: RtoAlgorithm RtoMin RtoMax RtoInitial MaxAssoc ValCookieLife MaxInitRetr CurrEstab ActiveEstabs PassiveEstabs Aborteds Shutdowns OutOfBlues ChecksumErrors OutCtrlChunks OutOrderChunks OutUnorderChunks InCtrlChunks InOrderChunks InUnorderChunks FragUsrMsgs ReasmUserMsgs OutSCTPPacks InSCTPPacks DiscontinuityTime\n"
		    "Sctp:");
	for (i = 0; i < offsetof(struct sctp_mib, __pad) / sizeof(unsigned long); i++)
		len +=
		    sprintf(buffer + len, " %lu",
			    fold_field((unsigned long *) sctp_statistics, sizeof(struct sctp_mib),
				       i));
	len += sprintf(buffer + len, "\n");
	if (offset >= len) {
		*start = buffer;
		return 0;
	}
	*start = buffer + offset;
	len -= offset;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}

#define TMPSZ 150
__SCTP_STATIC int
sctp_get_info(char *buffer, char **start, off_t offset, int length)
{
	int len = 0, num = 0, i;
	off_t begin, pos = 0;
	char tmpbuf[TMPSZ + 1];
	if (offset < TMPSZ)
		len +=
		    sprintf(buffer, "%-*s\n", TMPSZ - 1,
			    "  sl  local_address rem_address   st tx_queue "
			    "rx_queue tr tm->when retrnsmt   uid  timeout inode");
	pos = TMPSZ;
	/* First, walk listening socket table */
	local_bh_disable();
	for (i = 0; i < sctp_lhash_size; i++) {
		sctp_t *sp;
		struct sctp_hash_bucket *hp = &sctp_lhash[i];
		read_lock_bh(&hp->lock);
		for (sp = hp->list; sp; sp = sp->lnext) {
			struct sock *sk = SCTP_SOCK(sp);
			pos += TMPSZ;
			if (pos <= offset)
				continue;
			get_sctp_sock(sk, tmpbuf, num);
			len += sprintf(buffer + len, "%-*s\n", TMPSZ - 1, tmpbuf);
			if (pos >= offset + length) {
				read_unlock_bh(&hp->lock);
				goto out;
			}
			/* all open requests are in normal hash table */
		}
		read_unlock_bh(&hp->lock);
	}
	/* next walk established hash */
	for (i = 0; i < sctp_thash_size; i++) {
		sctp_t *sp;
		struct sctp_hash_bucket *hp = &sctp_thash[i];
		read_lock_bh(&hp->lock);
		for (sp = hp->list; sp; sp = sp->tnext) {
			struct sock *sk = SCTP_SOCK(sp);
			pos += TMPSZ;
			if (pos <= offset)
				continue;
			get_sctp_sock(sk, tmpbuf, num);
			len += sprintf(buffer + len, "%-*s\n", TMPSZ - 1, tmpbuf);
			if (pos >= offset + length) {
				read_unlock_bh(&hp->lock);
				goto out;
			}
		}
		read_unlock_bh(&hp->lock);
	}
      out:
	local_bh_enable();
	begin = len - (pos - offset);
	*start = buffer + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}
#endif				/* SOCKETS */

STATIC void
sctp_init_procfs(void)
{
#ifdef SOCKETS
	proc_net_create("sctp", 0, sctp_get_info);
	proc_net_remove("snmp");
	proc_net_create("snmp", 0, __os7_snmp_get_info);
	proc_net_remove("sockstat");
	proc_net_create("sockstat", 0, __os7_afinet_get_info);
#endif				/* SOCKETS */
	return;
}

STATIC void
sctp_term_procfs(void)
{
#ifdef SOCKETS
	proc_net_remove("sctp");
	proc_net_remove("snmp");
	proc_net_create("snmp", 0, snmp_get_info);
	proc_net_remove("sockstat");
	proc_net_create("sockstat", 0, afinet_get_info);
#endif				/* SOCKETS */
	return;
}
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
#ifdef CONFIG_SYSCTL
#ifdef SOCKETS
enum {
	NET_SCTP_RTO_INITIAL = 200,
	NET_SCTP_RTO_MIN = 201,
	NET_SCTP_RTO_MAX = 202,
	NET_SCTP_HEARTBEAT_ITVL = 203,
	NET_SCTP_INIT_RETRIES = 204,
	NET_SCTP_VALID_COOKIE_LIFE = 205,
	NET_SCTP_MAX_SACK_DELAY = 206,
	NET_SCTP_PATH_MAX_RETRANS = 207,
	NET_SCTP_ASSOC_MAX_RETRANS = 208,
	NET_SCTP_MAC_TYPE = 209,
	NET_SCTP_CSUM_TYPE = 210,
	NET_SCTP_COOKIE_INC = 211,
	NET_SCTP_THROTTLE_ITVL = 212,
	NET_SCTP_DEFAULT_PPI = 213,
	NET_SCTP_DEFAULT_SID = 214,
	NET_SCTP_MEM = 215,
	NET_SCTP_WMEM = 216,
	NET_SCTP_RMEM = 217,
	NET_SCTP_MAX_ISTREAMS = 218,
	NET_SCTP_REQ_OSTREAMS = 219,
	NET_SCTP_ECN = 220,
	NET_SCTP_ADAPTATION_LAYER_INFO = 221,
	NET_SCTP_PARTIAL_RELIABILITY = 222,
	NET_SCTP_MAX_BURST = 223,
};
static struct sctp_sysctl_table {
	struct ctl_table_header *sysctl_header;
	ctl_table sctp_vars[25];
	ctl_table sctp_proto_dir[2];
	ctl_table sctp_root_dir[2];
} sctp_sysctl = {
	NULL, { {
	NET_SCTP_RTO_INITIAL, "sctp_rto_initial", &sysctl_sctp_rto_initial,
			    sizeof(int), 0644, NULL,
			    &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec,
			    NULL, &sysctl_sctp_rto_min, &sysctl_sctp_rto_max}, {
	NET_SCTP_RTO_MIN, "sctp_rto_min", &sysctl_sctp_rto_min, sizeof(int), 0644, NULL,
		    &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL, NULL,
		    &sysctl_sctp_rto_max}, {
	NET_SCTP_RTO_MAX, "sctp_rto_max", &sysctl_sctp_rto_max, sizeof(int), 0644, NULL,
		    &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL,
		    &sysctl_sctp_rto_min, NULL}, {
	NET_SCTP_HEARTBEAT_ITVL, "sctp_heartbeat_itvl", &sysctl_sctp_heartbeat_itvl,
		    sizeof(int), 0644, NULL, &proc_dointvec_jiffies, &sysctl_jiffies}, {
	NET_SCTP_INIT_RETRIES, "sctp_init_retries", &sysctl_sctp_max_init_retries,
		    sizeof(int), 0644, NULL, &proc_dointvec}, {
	NET_SCTP_VALID_COOKIE_LIFE, "sctp_valid_cookie_life",
		    &sysctl_sctp_valid_cookie_life, sizeof(int), 0644, NULL,
		    &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL, NULL, NULL}, {
	NET_SCTP_MAX_SACK_DELAY, "sctp_max_sack_delay", &sysctl_sctp_max_sack_delay,
		    sizeof(int), 0644, NULL, &proc_doulongvec_ms_jiffies_minmax,
		    &sysctl_intvec, NULL, NULL, NULL}, {
	NET_SCTP_PATH_MAX_RETRANS, "sctp_path_max_retrans", &sysctl_sctp_path_max_retrans,
		    sizeof(int), 0644, NULL, &proc_dointvec}, {
	NET_SCTP_ASSOC_MAX_RETRANS, "sctp_assoc_max_retrans",
		    &sysctl_sctp_assoc_max_retrans, sizeof(int), 0644, NULL, &proc_dointvec}, {
	NET_SCTP_MAC_TYPE, "sctp_mac_type", &sysctl_sctp_mac_type, sizeof(int), 0644, NULL,
		    &proc_dointvec}, {
	NET_SCTP_CSUM_TYPE, "sctp_csum_type", &sysctl_sctp_csum_type, sizeof(int), 0644,
		    NULL, &proc_dointvec}, {
	NET_SCTP_COOKIE_INC, "sctp_cookie_inc", &sysctl_sctp_cookie_inc, sizeof(int), 0644,
		    NULL, &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL, NULL, NULL}, {
	NET_SCTP_THROTTLE_ITVL, "sctp_throttle_itvl", &sysctl_sctp_throttle_itvl,
		    sizeof(int), 0644, NULL, &proc_doulongvec_ms_jiffies_minmax}, {
	NET_SCTP_DEFAULT_PPI, "sctp_default_ppi", &sysctl_sctp_default_ppi, sizeof(int),
		    0644, NULL, &proc_dointvec}, {
	NET_SCTP_DEFAULT_SID, "sctp_default_sid", &sysctl_sctp_default_sid, sizeof(int),
		    0644, NULL, &proc_dointvec}, {
	NET_SCTP_MEM, "sctp_mem", &sysctl_sctp_mem, sizeof(sysctl_sctp_mem), 0644, NULL,
		    &proc_dointvec}
	, {
	NET_SCTP_WMEM, "sctp_wmem", &sysctl_sctp_wmem, sizeof(sysctl_sctp_wmem), 0644, NULL,
		    &proc_dointvec}
	, {
	NET_SCTP_RMEM, "sctp_rmem", &sysctl_sctp_rmem, sizeof(sysctl_sctp_rmem), 0644, NULL,
		    &proc_dointvec}
	, {
	NET_SCTP_MAX_ISTREAMS, "sctp_max_istreams", &sysctl_sctp_max_istreams, sizeof(int),
		    0644, NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
		    &min_sctp_max_istreams, NULL}, {
	NET_SCTP_REQ_OSTREAMS, "sctp_req_ostreams", &sysctl_sctp_req_ostreams, sizeof(int),
		    0644, NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL,
		    &min_sctp_req_ostreams, NULL}, {
	NET_SCTP_ECN, "sctp_ecn", &sysctl_sctp_ecn, sizeof(int), 0644, NULL, &proc_dointvec}, {
	NET_SCTP_ADAPTATION_LAYER_INFO, "sctp_adaptation_layer_info",
		    &sysctl_sctp_adaptation_layer_info, sizeof(int), 0644, NULL, &proc_dointvec}, {
	NET_SCTP_PARTIAL_RELIABILITY, "sctp_partial_reliability",
		    &sysctl_sctp_partial_reliability, sizeof(int), 0644, NULL, &proc_dointvec}, {
	NET_SCTP_MAX_BURST, "sctp_max_burst", &sysctl_sctp_max_burst, sizeof(int), 0644,
		    NULL, &proc_dointvec_minmax, &sysctl_intvec, NULL, &min_sctp_max_burst, NULL}, {
	0}}, { {
	NET_IPV4, "ipv4", NULL, 0, 0555, sctp_sysctl.sctp_vars}, {
	0}}, { {
	CTL_NET, "net", NULL, 0, 0555, sctp_sysctl.sctp_proto_dir}, {
	0}}
};
#endif				/* SOCKETS */

STATIC void
sctp_init_sysctl(void)
{
#ifdef SOCKETS
	sctp_sysctl.sysctl_header = register_sysctl_table(sctp_sysctl.sctp_root_dir, 0);
#endif				/* SOCKETS */
	return;
}

STATIC void
sctp_term_sysctl(void)
{
#ifdef SOCKETS
	unregister_sysctl_table(sctp_sysctl.sysctl_header);
#endif				/* SOCKETS */
	return;
}
#endif				/* CONFIG_SYSCTL */

/*
 *  ==========================================================================
 *
 *  Sockets Hooks
 *
 *  ==========================================================================
 *  The sockets version hooks into the inet socket protocol family to add SCTP to the SOCK_STREAM
 *  and SOCK_SEQPACKET socket types for inet.  STREAMS does not do this in this way.  The sockets
 *  (over XTI) facility provided by STREAMS accomplishes this.
 */

__SCTP_STATIC struct proto sctp_prot = {
	.name = "SCTP",
	.close = &sctp_close,
	.connect = &sctp_connect,
	.disconnect = &sctp_disconnect,
	.accept = &sctp_accept,
	.ioctl = &sctp_ioctl,
	.init = &sctp_init_sock,
	.destroy = &sctp_destroy,
	.shutdown = &sctp_shutdown,
	.setsockopt = &sctp_setsockopt,
	.getsockopt = &sctp_getsockopt,
	.sendmsg = &sctp_sendmsg,
	.recvmsg = &sctp_recvmsg,
	.bind = &sctp_bind,
	.backlog_rcv = &sctp_backlog_rcv,
	.hash = &sctp_hash,
	.unhash = &sctp_unhash,
	.get_port = &sctp_get_port,
};
struct proto_ops inet_sctp_ops = {
	.family = PF_INET,
	.release = &inet_release,
	.bind = &inet_bind,
	.connect = &inet_stream_connect,
	.socketpair = &sock_no_socketpair,
	.accept = &inet_accept,
#if defined(HAVE_INET_MULTI_GETNAME_EXPORT) || defined (HAVE_INET_MULTI_GETNAME_ADDR)
	.getname = &inet_multi_getname,
#endif
#if defined(HAVE_INET_GETNAME_EXPORT) || defined (HAVE_INET_GETNAME_ADDR)
	.getname = &inet_getname,
#endif
	.poll = &sctp_poll,
	.ioctl = &inet_ioctl,
	.listen = &sctp_listen,
	.shutdown = &inet_shutdown,
	.setsockopt = &inet_setsockopt,
	.getsockopt = &inet_getsockopt,
	.sendmsg = &inet_sendmsg,
	.recvmsg = &inet_recvmsg,
	.mmap = &sock_no_mmap,
	.sendpage = &sock_no_sendpage,	/* TODO: make an SCTP sendpage */
};

STATIC struct inet_protosw sctpsw_array[] = {
	{
	 .type = SOCK_SEQPACKET,
	 .protocol = IPPROTO_SCTP,
	 .prot = &sctp_prot,
	 .ops = &inet_sctp_ops,
	 .capability = -1,
	 .no_check = 0,
	 .flags = 0,
	 }
#ifdef SCTP_CONFIG_TCP_COMPATIBLE
	,
	{
	 .type = SOCK_STREAM,
	 .protocol = IPPROTO_SCTP,
	 .prot = &sctp_prot,
	 .ops = &inet_sctp_ops,
	 .capability = -1,
	 .no_check = 0,
	 .flags = 0,
	 }
#endif				/* SCTP_CONFIG_TCP_COMPATIBLE */
#ifdef SCTP_CONFIG_UDP_COMPATIBLE
	,
	{
	 .type = SOCK_RDM,
	 .protocol = IPPROTO_SCTP,
	 .prot = &sctp_prot,
	 .ops = &inet_sctp_ops,
	 .capability = -1,
	 .no_check = 0,
	 .flags = 0,
	 }
#endif				/* SCTP_CONFIG_UDP_COMPATIBLE */
};

#define SCTPSW_ARRAY_LEN (sizeof(sctpsw_array)/sizeof(struct inet_protosw))

STATIC void
sctp_init_protosw(void)
{
	struct inet_protosw *s;
	for (s = sctpsw_array; s < &sctpsw_array[SCTPSW_ARRAY_LEN]; ++s)
		inet_register_protosw(s);
	return;
}

STATIC void
sctp_term_protosw(void)
{
	struct sock *sk;
	struct inet_protosw *s;
	spin_lock_bh(&sctp_protolock);
	for (sk = sctp_protolist; sk; sk = sk->next) {
		sctp_discon_ind(sk, SCTP_ORIG_PROVIDER, -ECONNABORTED, NULL);
		sctp_abort(sk, SCTP_ORIG_PROVIDER, -ECONNABORTED);
	}
	spin_unlock_bh(&sctp_protolock);
	for (s = sctpsw_array; s < &sctpsw_array[SCTPSW_ARRAY_LEN]; ++s)
		inet_unregister_protosw(s);
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
/* *INDENT-ON* */

extern void __skb_cb_too_small_for_sctp(int, int);	/* never defined */
STATIC int
sctp_load_test(void)
{
	struct sk_buff *skb = NULL;
	if (sizeof(sctp_tcb_t) > sizeof(skb->cb)) {
		/* 
		 *  The control block asigned to an sk_buff is too small for
		 *  SCTP, so I suppose we are doomed.  This (never defined) call
		 *  will fail to resolve when compiler detects above.  Sneaky
		 *  trick taken from NET4 TCP code.
		 */
		__skb_cb_too_small_for_sctp(sizeof(sctp_tcb_t), sizeof(skb->cb));
	}
#ifdef SCTP_CONFIG_DEBUG
	if (SCTP_SOCK(SCTP_PROT((struct sock *) NULL)) != NULL) {
		ptrace(("ERROR: macro failure\n"));
		return -EFAULT;
	}
	if (SCTP_CB_SKB(SCTP_SKB_CB((struct sk_buff *) NULL)) != NULL) {
		ptrace(("ERROR: macro failure\n"));
		return -EFAULT;
	}
#endif				/* SCTP_CONFIG_DEBUG */
	return (0);
}

__SCTP_STATIC void __SCTP_INIT
sctp_init(void)
{
#ifdef MODULE
	printk(KERN_INFO SCTP_BANNER);	/* console splash */
#else
	printk(KERN_INFO SCTP_SLPASH);	/* console splash */
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
#ifdef CONFIG_SYSCTL
	sctp_init_sysctl();
#endif				/* CONFIG_SYSCTL */
#ifdef CONFIG_PROC_FS
	sctp_init_procfs();
#endif				/* CONFIG_PROC_FS */
	return;
}

STATIC void
sctp_terminate(void)
{
#ifdef CONFIG_PROC_FS
	sctp_term_procfs();
#endif				/* CONFIG_PROC_FS */
#ifdef CONFIG_SYSCTL
	sctp_term_sysctl();
#endif				/* CONFIG_SYSCTL */
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
	if (sctp_load_test != 0)
		return (-EFAULT);
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
