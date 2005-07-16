/*****************************************************************************

 @(#) $Id: sctp.h,v 0.9.2.4 2005/07/15 23:09:55 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/07/15 23:09:55 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCTP_H__
#define __SCTP_H__

#ident "@(#) $RCSfile: sctp.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2005/07/15 23:09:55 $"

/*
 *  =========================================================================
 *
 *  DEFAULT Protocol Values
 *
 *  =========================================================================
 */

#define SCTP_HMAC_NONE      0
#define SCTP_HMAC_SHA_1     1
#define SCTP_HMAC_MD5       2

/*
   ip defaults 
 */
#define SCTP_DEFAULT_IP_TOS		(0x0)
#define SCTP_DEFAULT_IP_TTL		(64)
#define SCTP_DEFAULT_IP_PROTO		(132)
#define SCTP_DEFAULT_IP_DONTROUTE	(0)
#define SCTP_DEFAULT_IP_BROADCAST	(0)
#define SCTP_DEFAULT_IP_PRIORITY	(0)

/*
   per association defaults 
 */
#define SCTP_DEFAULT_MAX_INIT_RETRIES	(8)
#define SCTP_DEFAULT_VALID_COOKIE_LIFE	(60*HZ)
#define SCTP_DEFAULT_MAX_SACK_DELAY	(200*HZ/1000)
#define SCTP_DEFAULT_ASSOC_MAX_RETRANS	(10)
#define SCTP_DEFAULT_MAC_TYPE		(SCTP_HMAC_NONE)
#define SCTP_DEFAULT_COOKIE_INC		(1*HZ)
#define SCTP_DEFAULT_THROTTLE_ITVL	(50*HZ/1000)
#define SCTP_DEFAULT_REQ_OSTREAMS	(1)
#define SCTP_DEFAULT_MAX_ISTREAMS	(33)
#define SCTP_DEFAULT_RMEM		(1<<15)
#define SCTP_DEFAULT_PPI		(0)
#define SCTP_DEFAULT_SID		(0)

/*
   per destination defaults 
 */
#define SCTP_DEFAULT_PATH_MAX_RETRANS	(5)
#define SCTP_DEFAULT_RTO_INITIAL	(3*HZ)
#define SCTP_DEFAULT_RTO_MIN		(1*HZ)
#define SCTP_DEFAULT_RTO_MAX		(60*HZ)
#define SCTP_DEFAULT_HEARTBEAT_ITVL	(30*HZ)

/*
   ip defaults 
 */
extern uint sctp_default_ip_tos;
extern uint sctp_default_ip_ttl;
extern uint sctp_default_ip_proto;
extern uint sctp_default_ip_dontroute;
extern uint sctp_default_ip_broadcast;
extern uint sctp_default_ip_priority;

/*
   per association defaults 
 */
extern size_t sctp_default_max_init_retries;
extern size_t sctp_default_valid_cookie_life;
extern size_t sctp_default_max_sack_delay;
extern size_t sctp_default_assoc_max_retrans;
extern size_t sctp_default_mac_type;
extern size_t sctp_default_cookie_inc;
extern size_t sctp_default_throttle_itvl;
extern size_t sctp_default_req_ostreams;
extern size_t sctp_default_max_istreams;
extern size_t sctp_default_rmem;
extern size_t sctp_default_ppi;
extern size_t sctp_default_sid;

/*
   per destination defaults 
 */
extern size_t sctp_default_path_max_retrans;
extern size_t sctp_default_rto_initial;
extern size_t sctp_default_rto_min;
extern size_t sctp_default_rto_max;
extern size_t sctp_default_heartbeat_itvl;

/*
 *  =========================================================================
 *
 *  DATA STRUCTURES
 *
 *  =========================================================================
 */
struct sctp_dup {
	struct sctp_dup *next;
	struct sctp_dup **prev;
	uint32_t tsn;
};
struct sctp_strm {
	struct sctp_strm *next;		/* linkage to stream list */
	struct sctp_strm **prev;	/* linkage to stream list */
	struct sctp *sp;
	uint16_t sid;			/* stream identifier */
	uint16_t ssn;			/* stream sequence number */
	struct {
		uint32_t ppi;		/* payload protocol id */
		mblk_t *head;		/* head pointer */
		uint more;		/* more data in (E)TSDU */
	} x, n;				/* expedited (x) and normal (n) */
};
struct sctp_saddr {
	struct sctp_saddr *next;	/* linkage for srce address list */
	struct sctp_saddr **prev;	/* linkage for srce address list */
	struct sctp *sp;		/* linkage for srce address list */
	uint32_t saddr;			/* srce address (network order) */
};
struct sctp_daddr {
	struct sctp_daddr *next;	/* linkage for dest address list */
	struct sctp_daddr **prev;	/* linkage for dest address list */
	struct sctp *sp;		/* linkage for dest address list */

	uint32_t daddr;			/* dest address (network order) */
	uint32_t saddr;			/* srce address (network order) */
	uint dif;			/* device interface */

	uint flags;			/* flags for this destination */
	size_t header_len;		/* header length */
	size_t mtu;			/* mtu */

	uint hb_onoff;			/* activation of heartbeats */
	uint hb_fill;			/* fill for heartbeat (PMTUDISC) */
	size_t in_flight;		/* bytes in flight */
	size_t retransmits;		/* retransmits this dest */
	size_t max_retrans;		/* max path retransmits */
	size_t dups;			/* number of duplicates */

	size_t cwnd;			/* congestion window */
	size_t ssthresh;		/* slow start threshold */

	toid_t timer_heartbeat;		/* heartbeat timer (for acks) */
	toid_t timer_retrans;		/* retrans (RTO) timer */
	toid_t timer_idle;		/* idle timer */

	ulong when;			/* last time transmitting */
	size_t ack_accum;		/* accumulator for acks */

	ulong hb_time;			/* time of last heartbeat sent */
	ulong hb_itvl;			/* interval between heartbeats */
	ulong rto_max;			/* maximum RTO value */
	ulong rto_min;			/* minimum RTO value */

	ulong rto;			/* current RTO value */
	ulong rttvar;			/* current RTT variance */
	ulong srtt;			/* current smoothed RTT */

	struct dst_entry *dst_cache;	/* destination cache */

	size_t packets;			/* packet count */
};
/*
 *  Some destination flags.
 */
#define SCTP_DESTF_INACTIVE     0x01	/* DEST is inactive */
#define SCTP_DESTF_HBACTIVE	0x02	/* DEST heartbeats */
#define SCTP_DESTF_TIMEDOUT	0x04	/* DEST timeout occured */
#define SCTP_DESTF_PMTUDISC	0x08	/* DEST pmtu discovered */
#define SCTP_DESTF_ROUTFAIL	0x10	/* DEST routing failure */
#define SCTP_DESTF_UNUSABLE	0x20	/* DEST unusable */
#define SCTP_DESTF_CONGESTD	0x40	/* DEST congested */

#define SCTP_DESTM_DONT_USE	(SCTP_DESTF_INACTIVE| \
				 SCTP_DESTF_UNUSABLE| \
				 SCTP_DESTF_ROUTFAIL| \
				 SCTP_DESTF_CONGESTD| \
				 SCTP_DESTF_TIMEDOUT)
#define SCTP_DESTM_CANT_USE	(SCTP_DESTF_UNUSABLE| \
				 SCTP_DESTF_ROUTFAIL)

struct sctp_ifops;			/* interface operations */

struct sctp {
	struct sctp *next;		/* linkage for master list */
	struct sctp **prev;		/* linkage for master list */

	ushort cmajor;			/* major device number */
	ushort cminor;			/* minor device number */

	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	cred_t cred;			/* credentials of opener */

	spinlock_t qlock;		/* queue lock */
	queue_t *rwait;			/* RD queue waiting on lock */
	queue_t *wwait;			/* WR queue waiting on lock */
	int users;			/* lock holders */

	struct sctp_ifops *ops;		/* interface operations */
	uint i_flags;			/* Interface flags */
	uint i_state;			/* Interface state */
	uint s_state;			/* SCTP state */
	uint options;			/* options flags */
	size_t conind;			/* max number outstanding conn_inds */

	struct sctp *bnext;		/* linkage for bind hash */
	struct sctp **bprev;		/* linkage for bind hash */
	struct sctp *lnext;		/* linkage for list hash */
	struct sctp **lprev;		/* linkage for list hash */
	struct sctp *pnext;		/* linkage for ptag hash */
	struct sctp **pprev;		/* linkage for ptag hash */
	struct sctp *vnext;		/* linkage for vtag hash */
	struct sctp **vprev;		/* linkage for vtag hash */
	struct sctp *tnext;		/* linkage for tcb hash */
	struct sctp **tprev;		/* linkage for tcb hash */
	uint hashent;			/* vtag cache entry */

	uint ip_tos;			/* IP TOS */
	uint ip_ttl;			/* IP TTL */
	uint ip_proto;			/* IP protocol number */
	uint ip_dontroute;		/* IP localroute */
	uint ip_broadcast;		/* IP broadcast */
	uint ip_priority;		/* IP priority */

	uint16_t sport;			/* loc port (network byte order) */
	uint16_t dport;			/* rem port (network byte order) */
	struct sctp_saddr *saddr;	/* list of loc addresses */
	struct sctp_daddr *daddr;	/* list of rem addresses */
	size_t sanum;			/* number of srce addresses in list */
	size_t danum;			/* number of dest addresses in list */

	struct sctp_daddr *taddr;	/* primary transmit destination address */
	struct sctp_daddr *raddr;	/* retransmission desintation address */
	struct sctp_daddr *caddr;	/* last received destination address */

	struct sctp_strm *ostrm;	/* list of outbound streams */
	struct sctp_strm *istrm;	/* list if inbound streams */
	size_t osnum;			/* number of outbound stream structures */
	size_t isnum;			/* number of inbound stream structures */
	struct sctp_strm *ostr;		/* current output stream */
	struct sctp_strm *istr;		/* current input stream */

	uint16_t req_ostr;		/* requested outbound streams */
	uint16_t max_istr;		/* maximum inbound streams */

	ulong max_sack;			/* maximum sack delay */
	ulong throttle;			/* throttle hb interval */
	ulong ck_life;			/* valid cookie life */
	ulong ck_inc;			/* cookie increment */
	uint hmac;			/* hmac type */

	uint16_t sid;			/* default sid */
	uint32_t ppi;			/* default ppi */

	ulong rto_ini;			/* default rto initial */
	ulong rto_min;			/* default rto minimum */
	ulong rto_max;			/* default rto maximum */
	size_t rtx_path;		/* default path max retrans */
	ulong hb_itvl;			/* default hb interval */

	mblk_t *retry;			/* msg to retry on timer expiry */

	bufq_t rcvq;			/* read queue */
	bufq_t sndq;			/* write queue */
	bufq_t urgq;			/* urgent queue */
	bufq_t errq;			/* error queue */
	bufq_t conq;			/* connect queue */

	bufq_t oooq;			/* out of order queue */
	struct sctp_tcb *gaps;		/* gaps acks for this stream */
	size_t ngaps;			/* number of gap reports in list */
	size_t nunds;			/* number of undelivered in list */
	bufq_t dupq;			/* duplicate queue */
	struct sctp_tcb *dups;		/* dup tsns for this stream */
	size_t ndups;			/* number of dup reports in list */
	bufq_t rtxq;			/* retransmit queue */
	size_t nrtxs;			/* number of retransmits in list */
	bufq_t ackq;			/* pending acknowledgement queue */

	uint16_t n_ostr;		/* number of outbound streams */
	uint16_t n_istr;		/* number of inbound streams */

	uint32_t v_tag;			/* locl verification tag */
	uint32_t a_rwnd;		/* locl receive window */

	uint32_t p_tag;			/* peer verification tag */
	uint32_t p_rwnd;		/* peer receive window */

	uint32_t t_tsn;			/* transmit TSN */
	uint32_t t_ack;			/* transmit TSN cum acked */
	uint32_t r_ack;			/* received TSN cum acked */

	uint sackf;			/* sack flags for association */
	uint flags;			/* flags */
	uint pmtu;			/* path MTU for association */

	size_t in_flight;		/* number of bytes in flight */
	size_t retransmits;		/* number of retransmits this assoc */
	size_t max_retrans;		/* max association retransmits */
	size_t max_inits;		/* max init retransmits */

	toid_t timer_init;		/* init timer */
	toid_t timer_cookie;		/* cookie timer */
	toid_t timer_shutdown;		/* shutdown timer */
	toid_t timer_sack;		/* sack timer */
};
/*
 *  Some flags.
 */
#define SCTP_FLAG_REC_CONF_OPT		0x00000001
#define SCTP_FLAG_EX_DATA_OPT		0x00000002
#define SCTP_FLAG_DEFAULT_RC_SEL	0x00000004
#define SCTP_FLAG_NEED_CLEANUP		0x00000008
/*
 *  Some sack flags.
 */
#define SCTP_SACKF_NEW          0x01	/* SACK for new data RFC 2960 6.2 */
#define SCTP_SACKF_WUP		0x02	/* SACK for wakeup RFC 2960 6.2 */
#define SCTP_SACKF_GAP          0x04	/* SACK for lost data RFC 2960 7.2.4 */
#define SCTP_SACKF_DUP          0x08	/* SACK for duplic data RFC 2960 6.2 */
#define SCTP_SACKF_URG          0x10	/* SACK for urgent data RFC 2960 ??? */
#define SCTP_SACKF_WND          0x20	/* SACK for new a_rwnd RFC 2960 6.2 */
#define SCTP_SACKF_TIM          0x40	/* SACK for timeout RFC 2960 6.2 */
#define SCTP_SACKF_NOD		0x80	/* SACK no delay RFC 2960 9.2 */
#define SCTP_SACKF_NOW		0xfe	/* SACK forced mask */
/*
 *  Some options flags.
 */
#define SCTP_OPTION_DROPPING	0x01	/* stream will drop packets */
#define SCTP_OPTION_BREAK	0x02	/* stream will break dest #1 and 2 one way */
#define SCTP_OPTION_DBREAK	0x04	/* stream will break dest both ways */
#define SCTP_OPTION_RANDOM	0x08	/* stream will drop packets at random */
#define SCTP_OPTION_NAGLE	0x10	/* stream will nagle bundling normal data */
#define SCTP_OPTION_CORK	0x20	/* stream will cork bundling normal data */

typedef struct sctp sctp_t;
typedef struct sctp_strm sctp_strm_t;
typedef struct sctp_saddr sctp_saddr_t;
typedef struct sctp_daddr sctp_daddr_t;

#define SCTP_PRIV(__q) ((sctp_t *)(__q)->q_ptr)

/*
 *  =========================================================================
 *
 *  SCTP STATES
 *
 *  =========================================================================
 */

#define SCTP_CLOSED		 0	/* CLOSED */
#define SCTP_LISTEN		 1	/* LISTEN */
#define SCTP_COOKIE_WAIT	 2	/* COOKIE-WAIT */
#define SCTP_COOKIE_ECHOED	 3	/* COOKIE-ECHOED */
#define SCTP_ESTABLISHED	 4	/* ESTABLISHED */
#define SCTP_SHUTDOWN_PENDING	 5	/* SHUTDOWN-PENDING */
#define SCTP_SHUTDOWN_SENT	 6	/* SHUTDOWN-SENT */
#define SCTP_SHUTDOWN_RECEIVED	 7	/* SHUTDOWN-RECEIVED */
#define SCTP_SHUTDOWN_RECVWAIT	 8	/* SHUTDOWN-RECEIVED */
#define SCTP_SHUTDOWN_ACK_SENT	 9	/* SHUTDOWN-ACK-SENT */
#define SCTP_UNREACHABLE	10	/* (not used) */
#define SCTP_MAX_STATES		 9

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

#endif				/* __SCTP_H__ */
