/*****************************************************************************

 @(#) $Id: sock.h,v 0.9.2.6 2007/08/14 09:42:29 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 09:42:29 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sock.h,v $
 Revision 0.9.2.6  2007/08/14 09:42:29  brian
 - GPLv3 header update

 Revision 0.9.2.5  2007/06/17 01:55:27  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __OS7_NET_NET_SOCK_H__
#define __OS7_NET_NET_SOCK_H__

#ident "@(#) $RCSfile: sock.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include <linux/autoconf.h>
#include <linux/timer.h>
#include <linux/cache.h>
#include <linux/in.h>		/* struct sockaddr_in */

#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
#include <linux/in6.h>		/* struct sockaddr_in6 */
#include <linux/ipv6.h>		/* dest_cache, inet6_options */
#include <linux/icmpv6.h>
#include <net/if_inet6.h>	/* struct ipv6_mc_socklist */
#endif

#if defined(CONFIG_INET) || defined (CONFIG_INET_MODULE)
#include <linux/icmp.h>
#endif
#include <linux/tcp.h>		/* struct tcphdr */

#include <linux/netdevice.h>
#include <linux/skbuff.h>	/* struct sk_buff */
#include <net/protocol.h>	/* struct inet_protocol */
#if defined(CONFIG_X25) || defined(CONFIG_X25_MODULE)
#include <net/x25.h>
#endif
#if defined(CONFIG_WAN_ROUTER) || defined(CONFIG_WAN_ROUTER_MODULE)
#include <linux/if_wanpipe.h>
#endif

#if defined(CONFIG_AX25) || defined(CONFIG_AX25_MODULE)
#include <net/ax25.h>
#if defined(CONFIG_NETROM) || defined(CONFIG_NETROM_MODULE)
#include <net/netrom.h>
#endif
#if defined(CONFIG_ROSE) || defined(CONFIG_ROSE_MODULE)
#include <net/rose.h>
#endif
#endif

#if defined(CONFIG_PPPOE) || defined(CONFIG_PPPOE_MODULE)
#include <linux/if_pppox.h>
#include <linux/ppp_channel.h>	/* struct ppp_channel */
#endif

#if defined(CONFIG_IPX) || defined(CONFIG_IPX_MODULE)
#if defined(CONFIG_SPX) || defined(CONFIG_SPX_MODULE)
#include <net/spx.h>
#else
#include <net/ipx.h>
#endif				/* CONFIG_SPX */
#endif				/* CONFIG_IPX */

#if defined(CONFIG_ATALK) || defined(CONFIG_ATALK_MODULE)
#include <linux/atalk.h>
#endif

#if defined(CONFIG_DECNET) || defined(CONFIG_DECNET_MODULE)
#include <net/dn.h>
#endif

#if defined(CONFIG_IRDA) || defined(CONFIG_IRDA_MODULE)
#include <net/irda/irda.h>
#endif

#if defined(CONFIG_ATM) || defined(CONFIG_ATM_MODULE)
struct atm_vcc;
#endif

#ifdef CONFIG_FILTER
#include <linux/filter.h>
#endif

#include <asm/atomic.h>
#include <net/dst.h>

struct __os7_sctp_opt;
struct __os7_sctp_skb_cb;
struct __os7_sctp_bind_bucket {
	unsigned short port;
	unsigned short fastreuse;
	struct __os7_sctp_bind_bucket *next;
	struct __os7_sctp_opt *owners;
	struct __os7_sctp_bind_bucket **prev;
};
struct __os7_sctp_dup {
	struct __os7_sctp_dup *next;
	struct __os7_sctp_dup **prev;
	__u32 tsn;
};
struct __os7_sctp_strm {
	struct __os7_sctp_strm *next;	/* linkage to stream list */
	struct __os7_sctp_strm **prev;	/* linkage to stream list */
	struct __os7_sctp_opt *sp;
	__u16 sid;			/* stream identifier */
	__u16 ssn;			/* stream sequence number */
	struct {
		__u32 ppi;		/* payload protocol id */
		struct sk_buff *head;	/* head pointer */
		uint more;		/* more data in (E)TSDU */
	} x, n;				/* expedited (x) and normal (n) */
};
struct __os7_sctp_saddr {
	struct __os7_sctp_saddr *next;	/* linkage for srce address list */
	struct __os7_sctp_saddr **prev;	/* linkage for srce address list */
	struct __os7_sctp_opt *sp;	/* linkage for srce address list */
	__u32 saddr;			/* srce address (network order) */
	uint flags;			/* flags for this address */
};
struct __os7_sctp_daddr {
	struct __os7_sctp_daddr *next;	/* linkage for dest address list */
	struct __os7_sctp_daddr **prev;	/* linkage for dest address list */
	struct __os7_sctp_opt *sp;	/* linkage for dest address list */
	__u32 daddr;			/* dest address (network order) */
	__u32 saddr;			/* srce address (network order) */
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
	struct timer_list timer_heartbeat;	/* heartbeat timer (for acks) */
	struct timer_list timer_retrans;	/* retrans (RTO) timer */
	struct timer_list timer_idle;	/* idle timer */
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

struct __os7_sctp_opt_dummy {
	struct __os7_sctp_opt *bnext;	/* linkage for bind hash */
	struct __os7_sctp_opt **bprev;	/* linkage for bind hash */
	struct __os7_sctp_bind_bucket *bindb;	/* linkage for bind hash */
	struct __os7_sctp_opt *lnext;	/* linkage for list hash */
	struct __os7_sctp_opt **lprev;	/* linkage for list hash */
	struct __os7_sctp_opt *pnext;	/* linkage for ptag hash */
	struct __os7_sctp_opt **pprev;	/* linkage for ptag hash */
	struct __os7_sctp_opt *vnext;	/* linkage for vtag hash */
	struct __os7_sctp_opt **vprev;	/* linkage for vtag hash */
	struct __os7_sctp_opt *tnext;	/* linkage for tcb hash */
	struct __os7_sctp_opt **tprev;	/* linkage for tcb hash */
	__u8 hashent;			/* vtag cache entry */
	__u8 nonagle;			/* Nagle setting */
	struct __os7_sctp_saddr *saddr;	/* list of loc addresses */
	struct __os7_sctp_daddr *daddr;	/* list of rem addresses */
	__u16 sanum;			/* number of srce addresses in list */
	__u16 danum;			/* number of dest addresses in list */
	struct __os7_sctp_daddr *taddr;	/* primary transmit dest address */
	struct __os7_sctp_daddr *raddr;	/* retransmission dest address */
	struct __os7_sctp_daddr *caddr;	/* last received dest address */
	struct __os7_sctp_strm *ostrm;	/* list of outbound streams */
	struct __os7_sctp_strm *istrm;	/* list if inbound streams */
	__u16 osnum;			/* number of outbound stream struct */
	__u16 isnum;			/* number of inbound stream struct */
	struct __os7_sctp_strm *ostr;	/* current output stream */
	struct __os7_sctp_strm *istr;	/* current input stream */
	ulong max_sack;			/* maximum sack delay */
	ulong throttle;			/* throttle init interval */
	ulong life;			/* data lifetime */
	__u8 disp;			/* data disposition */
	__u8 prel;			/* partial reliabliity */
	ulong hb_rcvd;			/* hb received timestamp */
	struct sk_buff *retry;		/* msg to retry on timer expiry */
	struct sk_buff_head accept_queue;	/* accept queue */
	struct sk_buff_head expedited_queue;	/* expedited queue */
	int rmem_queued;		/* queued read memeory */
	struct sk_buff_head urgent_queue;	/* urgent queue */
	struct sk_buff_head error_queue;	/* error queue */
	struct sk_buff_head out_of_order_queue;	/* out of order queue */
	struct __os7_sctp_skb_cb *gaps;	/* gaps acks for this stream */
	size_t ngaps;			/* number of gap reports in list */
	size_t nunds;			/* number of undelivered in list */
	struct sk_buff_head duplicate_queue;	/* duplicate queue */
	struct __os7_sctp_skb_cb *dups;	/* dup tsns for this stream */
	size_t ndups;			/* number of dup reports in list */
	struct sk_buff_head retrans_queue;	/* retransmit queue */
	size_t nrtxs;			/* number of retransmits in list */
	size_t nsack;			/* number of sacked in list */
	struct sk_buff_head ack_queue;	/* pending acknowledgement queue */
	__u16 n_ostr;			/* number of outbound streams */
	__u16 n_istr;			/* number of inbound streams */
	__u32 v_tag;			/* locl verification tag */
	__u32 p_tag;			/* peer verification tag */
	__u32 p_rwnd;			/* peer receive window */
	__u32 p_rbuf;			/* send receive buffer */
	__u32 a_rwnd;			/* advertized receive window */
	__u32 m_tsn;			/* manshall TSN */
	__u32 t_tsn;			/* transmit TSN */
	__u32 t_ack;			/* transmit TSN cum acked */
	__u32 r_ack;			/* received TSN cum acked */
	__u32 l_fsn;			/* local advanced TSN */
	__u32 p_fsn;			/* peer advanced TSN */
	__u32 l_lsn;			/* local lowest ECN TSN */
	__u32 p_lsn;			/* peer lowest ECN TSN */
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

#define __os7_beg_protinfo (&((struct sock *)(0))->protinfo)
#define __os7_siz_protinfo (sizeof(((struct sock *)0)->protinfo))
#define __os7_beg_tp_pinfo (&((struct sock *)(0))->tp_pinfo)
#define __os7_siz_tp_pinfo (sizeof(((struct sock *)0)->tp_pinfo))

#define __os7_gap_size \
	(size_t)((__u8 *)__os7_beg_protinfo - (__u8 *)__os7_beg_tp_pinfo) \
	+ sizeof(struct inet_opt) \
	- sizeof(struct __os7_sctp_opt_dummy)

struct __os7_sctp_opt {
	struct __os7_sctp_opt *bnext;	/* linkage for bind hash */
	struct __os7_sctp_opt **bprev;	/* linkage for bind hash */
	struct __os7_sctp_bind_bucket *bindb;	/* linkage for bind hash */
	struct __os7_sctp_opt *lnext;	/* linkage for list hash */
	struct __os7_sctp_opt **lprev;	/* linkage for list hash */
	struct __os7_sctp_opt *pnext;	/* linkage for ptag hash */
	struct __os7_sctp_opt **pprev;	/* linkage for ptag hash */
	struct __os7_sctp_opt *vnext;	/* linkage for vtag hash */
	struct __os7_sctp_opt **vprev;	/* linkage for vtag hash */
	struct __os7_sctp_opt *tnext;	/* linkage for tcb hash */
	struct __os7_sctp_opt **tprev;	/* linkage for tcb hash */
	__u8 hashent;			/* vtag cache entry */
	__u8 nonagle;			/* Nagle setting */
	struct __os7_sctp_saddr *saddr;	/* list of loc addresses */
	struct __os7_sctp_daddr *daddr;	/* list of rem addresses */
	__u16 sanum;			/* number of srce addresses in list */
	__u16 danum;			/* number of dest addresses in list */
	struct __os7_sctp_daddr *taddr;	/* primary transmit dest address */
	struct __os7_sctp_daddr *raddr;	/* retransmission dest address */
	struct __os7_sctp_daddr *caddr;	/* last received dest address */
	struct __os7_sctp_strm *ostrm;	/* list of outbound streams */
	struct __os7_sctp_strm *istrm;	/* list if inbound streams */
	__u16 osnum;			/* number of outbound stream struct */
	__u16 isnum;			/* number of inbound stream struct */
	struct __os7_sctp_strm *ostr;	/* current output stream */
	struct __os7_sctp_strm *istr;	/* current input stream */
	ulong max_sack;			/* maximum sack delay */
	ulong throttle;			/* throttle init interval */
	ulong life;			/* data lifetime */
	__u8 disp;			/* data disposition */
	__u8 prel;			/* partial reliabliity */
	ulong hb_rcvd;			/* hb received timestamp */
	struct sk_buff *retry;		/* msg to retry on timer expiry */
	struct sk_buff_head accept_queue;	/* accept queue */
	struct sk_buff_head expedited_queue;	/* expedited queue */
	int rmem_queued;		/* queued read memeory */
	struct sk_buff_head urgent_queue;	/* urgent queue */
	struct sk_buff_head error_queue;	/* error queue */
	struct sk_buff_head out_of_order_queue;	/* out of order queue */
	struct __os7_sctp_skb_cb *gaps;	/* gaps acks for this stream */
	size_t ngaps;			/* number of gap reports in list */
	size_t nunds;			/* number of undelivered in list */
	struct sk_buff_head duplicate_queue;	/* duplicate queue */
	struct __os7_sctp_skb_cb *dups;	/* dup tsns for this stream */
	size_t ndups;			/* number of dup reports in list */
	struct sk_buff_head retrans_queue;	/* retransmit queue */
	size_t nrtxs;			/* number of retransmits in list */
	size_t nsack;			/* number of sacked in list */
	struct sk_buff_head ack_queue;	/* pending acknowledgement queue */
	__u16 n_ostr;			/* number of outbound streams */
	__u16 n_istr;			/* number of inbound streams */
	__u32 v_tag;			/* locl verification tag */
	__u32 p_tag;			/* peer verification tag */
	__u32 p_rwnd;			/* peer receive window */
	__u32 p_rbuf;			/* send receive buffer */
	__u32 a_rwnd;			/* advertized receive window */
	__u32 m_tsn;			/* manshall TSN */
	__u32 t_tsn;			/* transmit TSN */
	__u32 t_ack;			/* transmit TSN cum acked */
	__u32 r_ack;			/* received TSN cum acked */
	__u32 l_fsn;			/* local advanced TSN */
	__u32 p_fsn;			/* peer advanced TSN */
	__u32 l_lsn;			/* local lowest ECN TSN */
	__u32 p_lsn;			/* peer lowest ECN TSN */
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
	__u8 __jump[__os7_gap_size];	/* jump over reserved structures */
	/* ------------------------------------------------------------------ */
	uint cmsg_flags;		/* flags */
	struct timer_list timer_init;	/* init timer */
	struct timer_list timer_cookie;	/* cookie timer */
	struct timer_list timer_shutdown;	/* shutdown timer */
	struct timer_list timer_guard;	/* shutdown guard timer */
	struct timer_list timer_sack;	/* sack timer */
	struct timer_list timer_asconf;	/* asconf timer */
	struct timer_list timer_life;	/* lifetime timer */
	__u8 hmac;			/* hmac type */
	__u8 cksum;			/* checksum type */
	ulong hb_tint;			/* hb throttle interval */
	__u16 sid;			/* default sid */
	__u32 ppi;			/* default ppi */
	ulong ck_life;			/* valid cookie life */
	ulong ck_inc;			/* cookie increment */
	__u8 l_caps;			/* local capabilities */
	__u8 p_caps;			/* peer capabilities */
	struct sk_buff *reply;		/* saved reply to ASCONF chunk */
	__u32 l_asn;			/* local ASCONF sequence number */
	__u32 p_asn;			/* peer ASCONF sequence number */
	__u32 l_ali;			/* local adaptation layer info */
	__u32 p_ali;			/* peer adaptation layer info */
	__u16 req_ostr;			/* requested outbound streams */
	__u16 max_istr;			/* maximum inbound streams */
	size_t max_inits;		/* max init retransmits */
	ulong rto_ini;			/* default rto initial */
	ulong rto_min;			/* default rto minimum */
	ulong rto_max;			/* default rto maximum */
	size_t rtx_path;		/* default path max retrans */
	ulong hb_itvl;			/* default hb interval */
	__u8 options;			/* options flags */
	int user_amps;			/* user max assoc payload size */
};

#endif				/* __OS7_NET_NET_SOCK_H__ */
