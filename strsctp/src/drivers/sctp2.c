/*****************************************************************************

 @(#) $RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:00 $

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

 Last Modified $Date: 2004/06/22 06:39:00 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:00 $"

static char const ident[] =
    "$RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:00 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>

#undef sctp_addr
#define sctp_addr n_sctp_addr
#undef sctp_addr_t
#define sctp_addr_t n_sctp_addr_t
#include <sys/npi.h>
#include <sys/npi_sctp.h>

#undef sctp_addr
#define sctp_addr t_sctp_addr
#undef sctp_addr_t
#define sctp_addr_t t_sctp_addr_t
#include <sys/tpi.h>
#include <sys/tpi_sctp.h>
#include <sys/xti_ip.h>
#include <sys/xti_sctp.h>

#include "debug.h"
#include "bufq.h"

#include <asm/softirq.h>	/* for start_bh_atomic, end_bh_atomic */
#include <linux/random.h>	/* for secure_tcp_sequence_number */

#undef min			/* LiS should not have defined these */
#define min lis_min
#undef max			/* LiS should not have defined these */
#define max lis_min

#define sctp_daddr sctp_daddr__
#define sctp_saddr sctp_saddr__
#define sctp_strm  sctp_strm__
#define sctp_dup   sctp_dup__
#ifdef ASSERT
#undef ASSERT
#endif

#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>

#undef sctp_daddr
#undef sctp_saddr
#undef sctp_strm
#undef sctp_dup
#ifdef ASSERT
#undef ASSERT
#endif

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#define SCTP_DESCRIP	"SCTP/IP STREAMS (NPI/TPI) DRIVER." "\n" \
			"Part of the OpenSS7 Stack for LiS STREAMS."
#define SCTP_REVISION	"OpenSS7 $RCSfile: sctp2.c,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/06/22 06:39:00 $"
#define SCTP_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corp. All Rights Reserved."
#define SCTP_DEVICE	"Supports LiS STREAMS and Linux NET4."
#define SCTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCTP_LICENSE	"GPL"
#define SCTP_BANNER	SCTP_DESCRIP	"\n" \
			SCTP_REVISION	"\n" \
			SCTP_COPYRIGHT	"\n" \
			SCTP_DEVICE	"\n" \
			SCTP_CONTACT	"\n"

MODULE_AUTHOR(SCTP_CONTACT);
MODULE_DESCRIPTION(SCTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCTP_LICENSE);
#endif

#ifndef tid_t
typedef int tid_t;
#endif

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
   ip defaults */
#define SCTP_DEFAULT_IP_TOS		(0x0)
#define SCTP_DEFAULT_IP_TTL		(64)
#define SCTP_DEFAULT_IP_PROTO		(132)
#define SCTP_DEFAULT_IP_DONTROUTE	(0)
#define SCTP_DEFAULT_IP_BROADCAST	(0)
#define SCTP_DEFAULT_IP_PRIORITY	(0)
/* 
   per association defaults */
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
   per destination defaults */
#define SCTP_DEFAULT_PATH_MAX_RETRANS	(5)
#define SCTP_DEFAULT_RTO_INITIAL	(3*HZ)
#define SCTP_DEFAULT_RTO_MIN		(1*HZ)
#define SCTP_DEFAULT_RTO_MAX		(60*HZ)
#define SCTP_DEFAULT_HEARTBEAT_ITVL	(30*HZ)
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
	tid_t timer_heartbeat;		/* heartbeat timer (for acks) */
	tid_t timer_retrans;		/* retrans (RTO) timer */
	tid_t timer_idle;		/* idle timer */
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
	struct sctp_ifops *ops;		/* interface operations */
	uint i_flags;			/* Interface flags */
	uint i_state;			/* Interface state */
	uint s_state;			/* SCTP state */
	uint options;			/* options flags */
	size_t conind;			/* max number outstanding conn_inds */
	lis_spin_lock_t lock;		/* stream lock */
	void *userq;			/* queue holding this lock */
	queue_t *waitq;			/* queue waiting on locks */
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
	tid_t timer_init;		/* init timer */
	tid_t timer_cookie;		/* cookie timer */
	tid_t timer_shutdown;		/* shutdown timer */
	tid_t timer_sack;		/* sack timer */
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
/* 
 *  =========================================================================
 *
 *  SCTP Message Structures
 *
 *  =========================================================================
 */
#ifndef PADC
#define PADC(__len) (((__len)+0x3)&~0x3)
#endif
/* 
   basic headers */
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
/* 
 *  SCTP Origin Codes (Match with NPI).
 */
#define SCTP_ORIG_PROVIDER	0x0100	/* provider originated reset/disconnect */
#define SCTP_ORIG_USER		0x0101	/* user originated reset/disconnect */
#define SCTP_ORIG_UNDEFINED	0x0102	/* reset/disconnect originator undefined */

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
/* 
 *  SCTP Parameter types
 */
#define SCTP_PTYPE_HEARTBEAT_INFO	(__constant_htons(1))
#define SCTP_PTYPE_IPV4_ADDR		(__constant_htons(5))
#define SCTP_PTYPE_IPV6_ADDR		(__constant_htons(6))
#define SCTP_PTYPE_STATE_COOKIE		(__constant_htons(7))
#define SCTP_PTYPE_UNREC_PARMS		(__constant_htons(8))
#define SCTP_PTYPE_COOKIE_PSRV		(__constant_htons(9))
#define SCTP_PTYPE_HOST_NAME		(__constant_htons(11))
#define SCTP_PTYPE_ADDR_TYPE		(__constant_htons(12))
#define SCTP_PTYPE_MASK			(__constant_htons(0x3fff))
#define	SCTP_PTYPE_MASK_CONTINUE	(__constant_htons(0x8000))
#define SCTP_PTYPE_MASK_REPORT		(__constant_htons(0x4000))
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
};
/* 
 *  SCTP Chunk Types
 */
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
#define SCTP_CTYPE_MASK			0x3f
#define SCTP_CTYPE_MASK_CONTINUE	0x80
#define SCTP_CTYPE_MASK_REPORT		0x40
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
};
/* 
 *  SCTP Message Structures
 */
struct sctp_msg {
	struct sctphdr mh;
	union sctp_chunk chunk;
};
#if 0
/* 
   The Adler32 checksum is deprecated. */
/* 
 *  ADLER 32 CHECKSUM
 *  -------------------------------------------------------------------------
 *  Compute the adler32 checksum.  This follows the description in RFC 2960
 *  Appendix B.  Borrowed from zlib.c.
 */
#define BASE 65521L		/* largest prime smaller than 65536 */
#define NMAX 5552		/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <=
				   2^32-1 */
#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);
static inline uint32_t
adler32(uint32_t adler, void *buf, size_t len)
{
	register uint32_t s1 = adler;
	register uint32_t s2 = (adler >> 16) & 0xffff;
	register uint8_t *ptr = buf;
	int k;
	if (!ptr)
		return 1L;
	while (len > 0) {
		k = len < NMAX ? len : NMAX;
		len -= k;
		while (k >= 16) {
			DO16(ptr);
			ptr += 16;
			k -= 16;
		}
		if (k != 0)
			do {
				s1 += *ptr++;
				s2 += s1;
			}
			while (--k);
		s1 %= BASE;
		s2 %= BASE;
	}
	return (s2 << 16) | s1;
}
#endif
/* 
 *  CRC-32C Checksum
 *  -------------------------------------------------------------------------
 *  Compute the CRC-32C checksum.  This follows the description in the new
 *  checksum draft.  Borrowed from D. Otis.
 */
/* 
   #define CRC32C_POLY 0x1EDC6F41 */
/* 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * Copyright 2001, D. Otis.  Use this program, code or tables 
 * extracted from it, as desired without restriction. 
 *
 * 32 Bit Reflected CRC table generation for SCTP. 
 * To accommodate serial byte data being shifted out least 
 * significant bit first, the table's 32 bit words are reflected 
 * which flips both byte and bit MS and LS positions.  The CRC 
 * is calculated MS bits first from the perspective of the serial 
 * stream.  The x^32 term is implied and the x^0 term may also 
 * be shown as +1.  The polynomial code used is 0x1EDC6F41. 
 * Castagnoli93 
 * x^32+x^28+x^27+x^26+x^25+x^23+x^22+x^20+x^19+x^18+x^14+x^13+ 
 * x^11+x^10+x^9+x^8+x^6+x^0 
 * Guy Castagnoli Stefan Braeuer and Martin Herrman 
 * "Optimization of Cyclic Redundancy-Check Codes 
 * with 24 and 32 Parity Bits", 
 * IEEE Transactions on Communications, Vol.41, No.6, June 1993 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */
static uint32_t crc_table[] = {
	0x00000000L, 0xF26B8303L, 0xE13B70F7L, 0x1350F3F4L,
	0xC79A971FL, 0x35F1141CL, 0x26A1E7E8L, 0xD4CA64EBL,
	0x8AD958CFL, 0x78B2DBCCL, 0x6BE22838L, 0x9989AB3BL,
	0x4D43CFD0L, 0xBF284CD3L, 0xAC78BF27L, 0x5E133C24L,
	0x105EC76FL, 0xE235446CL, 0xF165B798L, 0x030E349BL,
	0xD7C45070L, 0x25AFD373L, 0x36FF2087L, 0xC494A384L,
	0x9A879FA0L, 0x68EC1CA3L, 0x7BBCEF57L, 0x89D76C54L,
	0x5D1D08BFL, 0xAF768BBCL, 0xBC267848L, 0x4E4DFB4BL,
	0x20BD8EDEL, 0xD2D60DDDL, 0xC186FE29L, 0x33ED7D2AL,
	0xE72719C1L, 0x154C9AC2L, 0x061C6936L, 0xF477EA35L,
	0xAA64D611L, 0x580F5512L, 0x4B5FA6E6L, 0xB93425E5L,
	0x6DFE410EL, 0x9F95C20DL, 0x8CC531F9L, 0x7EAEB2FAL,
	0x30E349B1L, 0xC288CAB2L, 0xD1D83946L, 0x23B3BA45L,
	0xF779DEAEL, 0x05125DADL, 0x1642AE59L, 0xE4292D5AL,
	0xBA3A117EL, 0x4851927DL, 0x5B016189L, 0xA96AE28AL,
	0x7DA08661L, 0x8FCB0562L, 0x9C9BF696L, 0x6EF07595L,
	0x417B1DBCL, 0xB3109EBFL, 0xA0406D4BL, 0x522BEE48L,
	0x86E18AA3L, 0x748A09A0L, 0x67DAFA54L, 0x95B17957L,
	0xCBA24573L, 0x39C9C670L, 0x2A993584L, 0xD8F2B687L,
	0x0C38D26CL, 0xFE53516FL, 0xED03A29BL, 0x1F682198L,
	0x5125DAD3L, 0xA34E59D0L, 0xB01EAA24L, 0x42752927L,
	0x96BF4DCCL, 0x64D4CECFL, 0x77843D3BL, 0x85EFBE38L,
	0xDBFC821CL, 0x2997011FL, 0x3AC7F2EBL, 0xC8AC71E8L,
	0x1C661503L, 0xEE0D9600L, 0xFD5D65F4L, 0x0F36E6F7L,
	0x61C69362L, 0x93AD1061L, 0x80FDE395L, 0x72966096L,
	0xA65C047DL, 0x5437877EL, 0x4767748AL, 0xB50CF789L,
	0xEB1FCBADL, 0x197448AEL, 0x0A24BB5AL, 0xF84F3859L,
	0x2C855CB2L, 0xDEEEDFB1L, 0xCDBE2C45L, 0x3FD5AF46L,
	0x7198540DL, 0x83F3D70EL, 0x90A324FAL, 0x62C8A7F9L,
	0xB602C312L, 0x44694011L, 0x5739B3E5L, 0xA55230E6L,
	0xFB410CC2L, 0x092A8FC1L, 0x1A7A7C35L, 0xE811FF36L,
	0x3CDB9BDDL, 0xCEB018DEL, 0xDDE0EB2AL, 0x2F8B6829L,
	0x82F63B78L, 0x709DB87BL, 0x63CD4B8FL, 0x91A6C88CL,
	0x456CAC67L, 0xB7072F64L, 0xA457DC90L, 0x563C5F93L,
	0x082F63B7L, 0xFA44E0B4L, 0xE9141340L, 0x1B7F9043L,
	0xCFB5F4A8L, 0x3DDE77ABL, 0x2E8E845FL, 0xDCE5075CL,
	0x92A8FC17L, 0x60C37F14L, 0x73938CE0L, 0x81F80FE3L,
	0x55326B08L, 0xA759E80BL, 0xB4091BFFL, 0x466298FCL,
	0x1871A4D8L, 0xEA1A27DBL, 0xF94AD42FL, 0x0B21572CL,
	0xDFEB33C7L, 0x2D80B0C4L, 0x3ED04330L, 0xCCBBC033L,
	0xA24BB5A6L, 0x502036A5L, 0x4370C551L, 0xB11B4652L,
	0x65D122B9L, 0x97BAA1BAL, 0x84EA524EL, 0x7681D14DL,
	0x2892ED69L, 0xDAF96E6AL, 0xC9A99D9EL, 0x3BC21E9DL,
	0xEF087A76L, 0x1D63F975L, 0x0E330A81L, 0xFC588982L,
	0xB21572C9L, 0x407EF1CAL, 0x532E023EL, 0xA145813DL,
	0x758FE5D6L, 0x87E466D5L, 0x94B49521L, 0x66DF1622L,
	0x38CC2A06L, 0xCAA7A905L, 0xD9F75AF1L, 0x2B9CD9F2L,
	0xFF56BD19L, 0x0D3D3E1AL, 0x1E6DCDEEL, 0xEC064EEDL,
	0xC38D26C4L, 0x31E6A5C7L, 0x22B65633L, 0xD0DDD530L,
	0x0417B1DBL, 0xF67C32D8L, 0xE52CC12CL, 0x1747422FL,
	0x49547E0BL, 0xBB3FFD08L, 0xA86F0EFCL, 0x5A048DFFL,
	0x8ECEE914L, 0x7CA56A17L, 0x6FF599E3L, 0x9D9E1AE0L,
	0xD3D3E1ABL, 0x21B862A8L, 0x32E8915CL, 0xC083125FL,
	0x144976B4L, 0xE622F5B7L, 0xF5720643L, 0x07198540L,
	0x590AB964L, 0xAB613A67L, 0xB831C993L, 0x4A5A4A90L,
	0x9E902E7BL, 0x6CFBAD78L, 0x7FAB5E8CL, 0x8DC0DD8FL,
	0xE330A81AL, 0x115B2B19L, 0x020BD8EDL, 0xF0605BEEL,
	0x24AA3F05L, 0xD6C1BC06L, 0xC5914FF2L, 0x37FACCF1L,
	0x69E9F0D5L, 0x9B8273D6L, 0x88D28022L, 0x7AB90321L,
	0xAE7367CAL, 0x5C18E4C9L, 0x4F48173DL, 0xBD23943EL,
	0xF36E6F75L, 0x0105EC76L, 0x12551F82L, 0xE03E9C81L,
	0x34F4F86AL, 0xC69F7B69L, 0xD5CF889DL, 0x27A40B9EL,
	0x79B737BAL, 0x8BDCB4B9L, 0x988C474DL, 0x6AE7C44EL,
	0xBE2DA0A5L, 0x4C4623A6L, 0x5F16D052L, 0xAD7D5351L
};
#define DOCRC1(buf,i)	{crc=(crc>>8)^crc_table[(crc^(buf[i]))&0xff];}
#define DOCRC2(buf,i)	DOCRC1(buf,i); DOCRC1(buf,i+1);
#define DOCRC4(buf,i)	DOCRC2(buf,i); DOCRC2(buf,i+2);
#define DOCRC8(buf,i)	DOCRC4(buf,i); DOCRC4(buf,i+4);
#define DOCRC16(buf)	DOCRC8(buf,0); DOCRC8(buf,8)
/* 
   Note crc should be initialized to 0xffffffff */
static inline uint32_t
crc32c(register uint32_t crc, void *buf, register int len)
{
	register uint8_t *ptr = buf;
	if (ptr) {
		while (len >= 16) {
			DOCRC16(ptr);
			ptr += 16;
			len -= 16;
		}
		if (len != 0)
			do {
				crc = (crc >> 8) ^ crc_table[(crc ^ (*ptr++)) & 0xff];
			} while (--len);
	}
	return crc;
}

/* 
 *  Message control block datastructures:
 *  -------------------------------------------------------------------------
 */
/* 
   message control block */
typedef struct sctp_tcb {
	/* for gaps, dups and acks on receive, frag on transmit */
	struct sctp_tcb *next;		/* message linkage */
	struct sctp_tcb *prev;		/* message linkage */
	struct sctp_tcb *tail;		/* message linkage */
	struct sctp_tcb *head;		/* message linkage */
	mblk_t *mp;			/* message linkage */
	struct sctp_daddr *daddr;	/* daddr tx to or rx from */
	struct sctp_strm *st;		/* strm tx to or rx from */
	struct sctp_cookie *cookie;	/* cookie if this is COOKIE ECHO msg */
	unsigned long when;		/* time of tx/rx/ack */
	size_t trans;			/* number of times retransmitted */
	size_t sacks;			/* number of times gap acked */
	size_t dlen;			/* data length */
	ulong flags;			/* general flags inc. data chunk flags */
	uint32_t tsn;			/* why do I need these?, they are in the chunk header */
	uint16_t sid;
	uint16_t ssn;
	uint32_t ppi;
} sctp_tcb_t;
#define SCTP_TCB(__mp) ((struct sctp_tcb *)((__mp)->b_datap->db_base))
#define SCTP_IPH(__mp) ((struct iphdr *)((__mp)->b_datap->db_base))
/* 
 *  Some sctp_tcb flags.
 */
#define SCTPCB_FLAG_LAST_FRAG	0x0001	/* aligned with SCTP DATA chunk flags */
#define SCTPCB_FLAG_FIRST_FRAG	0x0002	/* aligned with SCTP DATA chunk flags */
#define SCTPCB_FLAG_URG		0x0004	/* aligned with SCTP DATA chunk flags */
#define SCTPCB_FLAG_UNUSED1	0x0100
#define SCTPCB_FLAG_DELIV	0x0200	/* delivered to user on read */
#define SCTPCB_FLAG_ACK		0x0400
#define SCTPCB_FLAG_NACK	0x0800
#define SCTPCB_FLAG_CONF	0x1000
#define SCTPCB_FLAG_SENT	0x2000
#define SCTPCB_FLAG_RETRANS	0x4000
#define SCTPCB_FLAG_SACKED	0x8000
/* 
 *  Hashing Functions
 *  -------------------------------------------------------------------------
 */
/* 
   for now: adjust it and scale it with kernel pages later */
#define sctp_bhash_size (1<<6)
#define sctp_lhash_size (1<<6)
#define sctp_phash_size (1<<6)
#define sctp_cache_size (1<<6)
#define sctp_vhash_size (1<<6)
#define sctp_thash_size (1<<6)
static inline uint
sctp_bhashfn(uint16_t port)
{
	return ((sctp_bhash_size - 1) & port);
}
static inline uint
sctp_lhashfn(uint16_t port)
{
	return ((sctp_lhash_size - 1) & port);
}
static inline uint
sctp_phashfn(uint32_t ptag)
{
	return ((sctp_phash_size - 1) & ptag);
}
static inline uint
sctp_cachefn(uint32_t vtag)
{
	return ((sctp_cache_size - 1) & vtag);
}
static inline uint
sctp_vhashfn(uint32_t vtag)
{
	return ((sctp_vhash_size - 1) & vtag);
}
static inline uint
sctp_thashfn(uint16_t sport, uint16_t dport)
{
	return ((sctp_thash_size - 1) & (sport + (dport << 4)));
}
static inline uint
sctp_sp_bhashfn(sctp_t * sp)
{
	return sctp_bhashfn(sp->sport);
}
static inline uint
sctp_sp_lhashfn(sctp_t * sp)
{
	return sctp_lhashfn(sp->sport);
}
static inline uint
sctp_sp_phashfn(sctp_t * sp)
{
	return sctp_phashfn(sp->p_tag);
}
static inline uint
sctp_sp_cachefn(sctp_t * sp)
{
	return (sp->hashent = sctp_cachefn(sp->v_tag));
}
static inline uint
sctp_sp_vhashfn(sctp_t * sp)
{
	return sctp_vhashfn(sp->v_tag);
}
static inline uint
sctp_sp_thashfn(sctp_t * sp)
{
	return sctp_thashfn(sp->sport, sp->dport);
}

rwlock_t sctp_hash_lock;
#define SCTPHASH_RLOCK()    read_lock_bh(&sctp_hash_lock)
#define SCTPHASH_RUNLOCK()  read_unlock_bh(&sctp_hash_lock)
#define SCTPHASH_WLOCK()    write_lock_bh(&sctp_hash_lock)
#define SCTPHASH_WUNLOCK()  write_unlock_bh(&sctp_hash_lock)
#define SCTPHASH_BH_RLOCK()    read_lock(&sctp_hash_lock)
#define SCTPHASH_BH_RUNLOCK()  read_unlock(&sctp_hash_lock)
#define SCTPHASH_BH_WLOCK()    write_lock(&sctp_hash_lock)
#define SCTPHASH_BH_WUNLOCK()  write_unlock(&sctp_hash_lock)
static void __sctp_unhash(sctp_t * sp);	/* without locks */
static void __sctp_bind_unhash(sctp_t * sp);	/* without locks */
static void __sctp_conn_unhash(sctp_t * sp);	/* without locks */
/* 
 *  Find a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static inline struct sctp_daddr *
sctp_find_daddr(sp, daddr)
	sctp_t *sp;
	uint32_t daddr;
{
	struct sctp_daddr *sd;
	for (sd = sp->daddr; sd && sd->daddr != daddr; sd = sd->next) ;
	return (sd);
}

/* 
 *  Find a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static inline struct sctp_saddr *
sctp_find_saddr(sp, saddr)
	sctp_t *sp;
	uint32_t saddr;
{
	struct sctp_saddr *ss;
	for (ss = sp->saddr; ss && ss->saddr != saddr; ss = ss->next) ;
	return (ss);
}

struct sctp_strm *sctp_strm_alloc(struct sctp_strm **stp, uint16_t sid, int *erp);
/* 
 *  Find or Add an Inbound or Outbound Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static inline struct sctp_strm *
sctp_istrm_find(sp, sid, errp)
	sctp_t *sp;
	uint16_t sid;
	int *errp;
{
	struct sctp_strm *st;
	if (!(st = sp->istr) || st->sid != sid)
		for (st = sp->istrm; st && st->sid != sid; st = st->next) ;
	if (st)
		sp->istr = st;	/* cache */
	else if ((st = sctp_strm_alloc(&sp->istrm, sid, errp)))
		st->ssn = -1;
	return (st);
}
static inline struct sctp_strm *
sctp_ostrm_find(sp, sid, errp)
	sctp_t *sp;
	uint16_t sid;
	int *errp;
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

#define sctp_init_lock(__sp) lis_spin_lock_init(&((__sp)->lock),"sctp-private")
#define sctp_locked(__sp) ((__sp)->userq)
static inline int
sctp_trylock(queue_t *q)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	psw_t flags;
	if (lis_spin_is_locked(&sp->lock))
		return (!0);
	lis_spin_lock_irqsave(&sp->lock, &flags);
	if (sp->userq && sp->userq != q) {
		lis_spin_unlock_irqrestore(&sp->lock, &flags);
		return (!0);
	}
	sp->userq = q;
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
	return (0);
}
static inline int
sctp_waitlock(queue_t *q)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	psw_t flags;
	lis_spin_lock_irqsave(&sp->lock, &flags);
	if (sp->userq && sp->userq != q) {
		if (sp->waitq && sp->waitq != q)
			ptrace(("SWERR: More than two queues in pair!\n"));
		sp->waitq = q;
		lis_spin_unlock_irqrestore(&sp->lock, &flags);
		return (!0);
	}
	sp->userq = q;
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
	return (0);
}
static void sctp_cleanup_read(sctp_t * sp);
static void sctp_transmit_wakeup(sctp_t * sp);
static inline void
sctp_unlock(queue_t *q)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	psw_t flags;
	sctp_cleanup_read(sp);	/* deliver to userq what is possible */
	sctp_transmit_wakeup(sp);	/* reply to peer what is necessary */
	lis_spin_lock_irqsave(&sp->lock, &flags);
	if (sp->userq && sp->userq == q) {
		sp->userq = NULL;
		if (sp->waitq && sp->waitq != q)	/* run the other queue in the queue * pair
							   if it was blocked */
			qenable(xchg(&sp->waitq, NULL));
		else
			ptrace(("SWERR: Bogus wait\n"));
	} else
		ptrace(("SWERR: Bogus unlock\n"));
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
}

/* 
 *  These two are used by timeout functions to lock normal queue functions
 *  from entering put and srv routines.
 */
#define sctp_bh_lock(__sp) lis_spin_lock(&((__sp)->lock))
#define sctp_bh_unlock(__sp) lis_spin_unlock(&((__sp)->lock))
// 
// TLI interface state flags
// 
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
#define TF_SCTP_HB			(1<<27)
#define TF_SCTP_RTO			(1<<28)
#define TF_SCTP_MAXSEG			(1<<29)
#define TF_SCTP_STATUS			(1<<30)
#define TF_SCTP_DEBUG			(1<<31)
#define TF_SCTP_ALLOPS			0xffffffff

/* 
 *  =========================================================================
 *
 *  SCTP Provider --> SCTP User Interface Primitives
 *
 *  =========================================================================
 *
 *  There must be defined by the specific SCTP User and linked.
 *
 *  CONN_IND:   provides indication of a incoming connection from the peer and
 *              provides a pointer to the connection indication structure
 *              which contains information from the verified cookie in the
 *              COOKIE-ECHO message.  The interface should indicate the
 *              connection to its user and buffer the indication as required.
 *              A return value of non-zero indicates that the interface could
 *              not buffer the connection indication.  Any data received in the
 *              COOKIE-ECHO message will be separately indicated with the
 *              DATA_IND primitive.
 *
 *  CONN_CON:   provides confirmation that the connection has been
 *              established.This is called when a COOKIE-ACK has been
 *              successfully received.  Any data received with the SCTP
 *              message that contained the COOKIE-ACK will be separately
 *              indicated with the data indication primitive.
 *
 *  DATA_IND:   provides indication of data that was received in sequence for
 *              the specified message control block parameters.  The message
 *              control block parameters of interest are PPI, SID, and SSN.
 *              Also, the MORE argument indicates that there are further data
 *              indications that make up the same data record.  The M_DATA
 *              block provided contains a message control block and the data
 *              payload.
 *
 *  DATACK_IND: provides indication that the message with the given message
 *              control block parameters was received in entirety and
 *              positively acknowledged by the peer.  Message control block
 *              parameters of interest include SID and SSN.  Also of interest
 *              might be the per chunk statistics present in the control block
 *              (how many times sent, delay) This only applies to ordered data
 *              unless a token was provided on write.  The M_DATA block
 *              provided contains only a message control block and no data.
 *
 *  RESET_IND:  provids indication that the association has been reset as a
 *              result of an association restart condition.  It also means
 *              that any unacknowledged transmit data will be discarded and
 *              that stream sequence numbers are being reset.
 *
 *  DISCON_IND: provides indication that either an outstanding connection
 *              indication or an existing connection has been abotively
 *              disconnected for the provided reason.  When the CP argument is
 *              included, it identifies the connection indication being
 *              disconnected.  When the CP argument is NULL, the stream to
 *              which the DISCON_IND is sent is the one being disconnected.
 *              In SCTP no data is ever associated with an DISCON_IND.
 *
 *  ORDREL_IND: provides indication that an existing connection has received a
 *              SHUTDOWN or SHUTDOWN-ACK from the peer.
 *
 *  RETR_IND:   provides indication of a retrieved message with the given
 *              message control block parameters.  Message control block
 *              parameters of interest are SID and SSN, STATUS and statistics
 *              associated with the data.  The M_DATA block provided contains
 *              a message control block and the data payload.
 *
 *  RETR_CON:   provides confirmation that all unsent and unacked data has
 *              been retrieved.
 *
 */
struct sctp_ifops {
	int (*sctp_conn_ind) (sctp_t * sp, mblk_t *cp);
	int (*sctp_conn_con) (sctp_t * sp);
	int (*sctp_data_ind) (sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn,
			      uint ord, uint more, mblk_t *dp);
	int (*sctp_datack_ind) (sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn,
				uint32_t tsn);
	int (*sctp_reset_ind) (sctp_t * sp, ulong orig, ulong reason, mblk_t *cp);
	int (*sctp_reset_con) (sctp_t * sp);
	int (*sctp_discon_ind) (sctp_t * sp, ulong orig, long reason, mblk_t *cp);
	int (*sctp_ordrel_ind) (sctp_t * sp);
	int (*sctp_retr_ind) (sctp_t * sp, mblk_t *dp);
	int (*sctp_retr_con) (sctp_t * sp);
};
/* 
 *  =========================================================================
 *
 *  SCTP User Interface --> SCTP Provider Primitives
 *
 *  =========================================================================
 *
 *  BIND_REQ:   requests that the provider bind the stream to an SCTP port and address
 *              list and set the requested number of connection indications.  The provider
 *              allocates any necessary resources for binding the stream.
 *
 *  CONN_REQ:   requests that the provider initiate an association with the provided data
 *              (if any).  This will initiate an association by starting the
 *              INIT/INIT-ACK/COOKIE-ECHO/COOKIE-ACK procedure.  The user will either
 *              receive and error number in return, or will receive indication via the
 *              DISCON_IND or CONN_CON callbacks that the connection has failed or was
 *              successful.
 *
 *  CONN_RES:   responds to a previous connection indication from the SCTP provider.  The
 *              connection response includes the conndication indication that was earlier
 *              provided and a pointer to the accepting stream  the accepting stream may
 *              be the same as the responding stream.  Any data provided will be bundled
 *              with the COOKIE-ACK.
 *
 *  DATA_REQ:   requests that the provided data with the provide message control block
 *              parameters be sent on the connection.  Message control block parameters of
 *              interest are PPI, SID, ordered flag, receipt confirmation flag.  The
 *              M_DATA block provided contains a message control block and the data
 *              payload.
 *
 *  DATACK_REQ: requests that the provider acknowledge receipt of a previous DATA_IND with
 *              the given transmit sequence number.  (This will also acknowledge any
 *              previous data fragments that made up the data record that ended with this
 *              TSN.)
 *
 *  RESET_CON:  used to acknowledge a previous reset indication.  This sends a COOKIE ACK
 *              in an SCTP restart scenario.
 *
 *  DISCON_REQ: requests that either the the requesting stream be aborted or that the
 *              connection indication provided by rejected.  This results in an ABORT
 *              being sent.
 *
 *  ORDREL_REQ: request that the SCTP provider accept no more data for transmission on the
 *              connection and inform the other side that it is finished sending data.
 *              This results in a SHUTDOWN or SHUTDOWN-ACK being sent.
 *
 *  UNBIND_REQ: requests that the provider unbind the stream in preparation for having the
 *              stream closed.  The provider frees any and all resources associated with
 *              the stream regardless of what s_state the stream is in.
 *
 */
static int sctp_bind_req(sctp_t * sp, uint16_t sport, uint32_t * sptr, size_t snum, ulong cons);
static int sctp_conn_req(sctp_t * sp, uint16_t dport, uint32_t * dptr, size_t dnum, mblk_t *dp);
static int sctp_conn_res(sctp_t * sp, mblk_t *cp, sctp_t * ap, mblk_t *dp);
static int sctp_data_req(sctp_t * sp, uint32_t ppi, uint16_t sid, uint ord, uint more, uint rctp,
			 mblk_t *dp);
static int sctp_reset_req(sctp_t * sp);
static int sctp_reset_res(sctp_t * sp);
static int sctp_discon_req(sctp_t * sp, mblk_t *cp);
static int sctp_ordrel_req(sctp_t * sp);
static int sctp_unbind_req(sctp_t * sp);
/* 
 *  =========================================================================
 *
 *  SCTP Peer --> SCTP Primitives (Receive Messages)
 *
 *  =========================================================================
 */
static int sctp_recv_msg(sctp_t * sp, mblk_t *mp);
static int sctp_recv_err(sctp_t * sp, mblk_t *mp);
/* 
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */
#define ERROR_GENERATOR
#define ERROR_GENERATOR_LEVEL  8
#define ERROR_GENERATOR_LIMIT 13
#define BREAK_GENERATOR_LEVEL 50
#define BREAK_GENERATOR_LIMIT 200

/* 
   ip defaults */
uint sctp_default_ip_tos = SCTP_DEFAULT_IP_TOS;
uint sctp_default_ip_ttl = SCTP_DEFAULT_IP_TTL;
uint sctp_default_ip_proto = SCTP_DEFAULT_IP_PROTO;
uint sctp_default_ip_dontroute = SCTP_DEFAULT_IP_DONTROUTE;
uint sctp_default_ip_broadcast = SCTP_DEFAULT_IP_BROADCAST;
uint sctp_default_ip_priority = SCTP_DEFAULT_IP_PRIORITY;
/* 
   per association defaults */
size_t sctp_default_max_init_retries = SCTP_DEFAULT_MAX_INIT_RETRIES;
size_t sctp_default_valid_cookie_life = SCTP_DEFAULT_VALID_COOKIE_LIFE;
size_t sctp_default_max_sack_delay = SCTP_DEFAULT_MAX_SACK_DELAY;
size_t sctp_default_assoc_max_retrans = SCTP_DEFAULT_ASSOC_MAX_RETRANS;
size_t sctp_default_mac_type = SCTP_DEFAULT_MAC_TYPE;
size_t sctp_default_cookie_inc = SCTP_DEFAULT_COOKIE_INC;
size_t sctp_default_throttle_itvl = SCTP_DEFAULT_THROTTLE_ITVL;
size_t sctp_default_req_ostreams = SCTP_DEFAULT_REQ_OSTREAMS;
size_t sctp_default_max_istreams = SCTP_DEFAULT_MAX_ISTREAMS;
size_t sctp_default_rmem = SCTP_DEFAULT_RMEM;
size_t sctp_default_ppi = SCTP_DEFAULT_PPI;
size_t sctp_default_sid = SCTP_DEFAULT_SID;
/* 
   per destination defaults */
size_t sctp_default_path_max_retrans = SCTP_DEFAULT_PATH_MAX_RETRANS;
size_t sctp_default_rto_initial = SCTP_DEFAULT_RTO_INITIAL;
size_t sctp_default_rto_min = SCTP_DEFAULT_RTO_MIN;
size_t sctp_default_rto_max = SCTP_DEFAULT_RTO_MAX;
size_t sctp_default_heartbeat_itvl = SCTP_DEFAULT_HEARTBEAT_ITVL;
/* 
 *  Cache pointers
 *  -------------------------------------------------------------------------
 */
kmem_cache_t *sctp_sctp_cachep = NULL;
kmem_cache_t *sctp_dest_cachep = NULL;
kmem_cache_t *sctp_srce_cachep = NULL;
kmem_cache_t *sctp_strm_cachep = NULL;
static void
sctp_init_caches(void)
{
	if (!sctp_sctp_cachep
	    && !(sctp_sctp_cachep =
		 kmem_cache_create("sctp_sctp_cachep", sizeof(sctp_t), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL)))
		panic("%s:Cannot alloc sctp_sctp_cachep.\n", __FUNCTION__);
	if (!sctp_dest_cachep
	    && !(sctp_dest_cachep =
		 kmem_cache_create("sctp_dest_cachep", sizeof(sctp_daddr_t), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s:Cannot alloc sctp_dest_cachep.\n", __FUNCTION__);
	if (!sctp_srce_cachep
	    && !(sctp_srce_cachep =
		 kmem_cache_create("sctp_srce_cachep", sizeof(sctp_saddr_t), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s:Cannot alloc sctp_srce_cachep.\n", __FUNCTION__);
	if (!sctp_strm_cachep
	    && !(sctp_strm_cachep =
		 kmem_cache_create("sctp_strm_cachep", sizeof(sctp_strm_t), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL)))
		panic("%s:Cannot alloc sctp_strm_cachep.\n", __FUNCTION__);
	return;
}
static void
sctp_term_caches(void)
{
	if (sctp_sctp_cachep)
		if (kmem_cache_destroy(sctp_sctp_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_sctp_cachep");
	if (sctp_dest_cachep)
		if (kmem_cache_destroy(sctp_dest_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_dest_cachep");
	if (sctp_srce_cachep)
		if (kmem_cache_destroy(sctp_srce_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_srce_cachep");
	if (sctp_strm_cachep)
		if (kmem_cache_destroy(sctp_strm_cachep))
			cmn_err(CE_WARN, "%s: did not destroy sctp_strm_cachep");
	return;
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
__sctp_daddr_alloc(sp, daddr, errp)
	sctp_t *sp;
	uint32_t daddr;
	int *errp;
{
	struct sctp_daddr *sd;
	assert(errp);
	ensure(sp, *errp = -EFAULT;
	       return (NULL));
	if (!daddr)
		return (NULL);
	if (!(daddr & 0xff000000)) {	/* zeronet is illegal */
		assure(daddr & 0xff000000);
		*errp = -EADDRNOTAVAIL;
		seldom();
		return (NULL);
	}
	/* TODO: need to check permissions (TACCES) for broadcast or multicast addresses and
	   whether host addresses are valid (TBADADDR). */
	if ((sd = kmem_cache_alloc(sctp_dest_cachep, SLAB_ATOMIC))) {
		bzero(sd, sizeof(*sd));
		if ((sd->next = sp->daddr))
			sd->next->prev = &sd->next;
		sd->prev = &sp->daddr;
		sp->daddr = sd;
		sp->danum++;
		sd->sp = sp;
		sd->daddr = daddr;
		sd->mtu = 576;	/* fix up after routing */
		sd->ssthresh = 2 * sd->mtu;	/* fix up after routing */
		sd->cwnd = sd->mtu;	/* fix up after routing */
		/* per destination defaults */
		sd->hb_itvl = sp->hb_itvl;	/* heartbeat interval */
		sd->rto_max = sp->rto_max;	/* maximum RTO */
		sd->rto_min = sp->rto_min;	/* minimum RTO */
		sd->rto = sp->rto_ini;	/* initial RTO */
		sd->max_retrans = sp->rtx_path;	/* max path retrans */
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
struct sctp_daddr *
sctp_daddr_include(sp, daddr, errp)
	sctp_t *sp;
	uint32_t daddr;
	int *errp;
{
	struct sctp_daddr *sd;
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	SCTPHASH_WLOCK();
	if (!(sd = sctp_find_daddr(sp, daddr)))
		sd = __sctp_daddr_alloc(sp, daddr, errp);
	SCTPHASH_WUNLOCK();
	usual(sd);
	return (sd);
}

/* 
 *  Free a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_daddr_free(sd)
	struct sctp_daddr *sd;
{
	ensure(sd, return);
	/* Need to free any cached IP routes.  */
	if (sd->dst_cache) {
		rare();
		dst_release(xchg(&sd->dst_cache, NULL));
	}
	if (sd->timer_idle) {
		rare();
		untimeout(xchg(&sd->timer_idle, 0));
	}
	if (sd->timer_heartbeat) {
		rare();
		untimeout(xchg(&sd->timer_heartbeat, 0));
	}
	if (sd->timer_retrans) {
		rare();
		untimeout(xchg(&sd->timer_retrans, 0));
	}
	if (sd->sp)
		sd->sp->danum--;
	if ((*sd->prev = sd->next))
		sd->next->prev = sd->prev;
	kmem_cache_free(sctp_dest_cachep, sd);
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
	sp->dport = 0;
	sp->taddr = NULL;
	sp->raddr = NULL;
	sp->caddr = NULL;
}

/* 
 *  Allocate a group of Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
sctp_alloc_daddrs(sctp_t * sp, uint16_t dport, uint32_t * daddrs, size_t dnum)
{
	int err = 0;
	ensure(sp, return (-EFAULT));
	ensure(daddrs || !dnum, return (-EFAULT));
	SCTPHASH_WLOCK();
	if (sp->daddr || sp->danum) {
		rare();
		__sctp_free_daddrs(sp);
	}			/* start clean */
	sp->dport = dport;
	if (dnum) {
		while (dnum--)
			if (!__sctp_daddr_alloc(sp, daddrs[dnum], &err) && err) {
				rare();
				break;
			}
	} else
		usual(dnum);
	if (err) {
		rare();
		__sctp_free_daddrs(sp);
	}
	SCTPHASH_WUNLOCK();
	if (err) {
		abnormal(err);
		ptrace(("Returning error %d\n", err));
	}
	return (err);
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
__sctp_saddr_alloc(sp, saddr, errp)
	sctp_t *sp;
	uint32_t saddr;
	int *errp;
{
	struct sctp_saddr *ss;
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	if (!saddr) {
		rare();
		return (NULL);
	}
	if ((ss = kmem_cache_alloc(sctp_srce_cachep, SLAB_ATOMIC))) {
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
	rare();
	return (ss);
}

/* 
 *  Find or Add a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
struct sctp_saddr *
sctp_saddr_include(sp, saddr, errp)
	sctp_t *sp;
	uint32_t saddr;
	int *errp;
{
	struct sctp_saddr *ss;
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	SCTPHASH_WLOCK();
	if (!(ss = sctp_find_saddr(sp, saddr)))
		ss = __sctp_saddr_alloc(sp, saddr, errp);
	SCTPHASH_WUNLOCK();
	usual(ss);
	return (ss);
}

/* 
 *  Free a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
__sctp_saddr_free(ss)
	struct sctp_saddr *ss;
{
	assert(ss);
	if (ss->sp)
		ss->sp->sanum--;
	if ((*ss->prev = ss->next))
		ss->next->prev = ss->prev;
	kmem_cache_free(sctp_srce_cachep, ss);
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
	sp->sport = 0;
}

/* 
 *  Allocate a group of Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
sctp_alloc_saddrs(sp, sport, saddrs, snum)
	sctp_t *sp;
	uint16_t sport;
	uint32_t *saddrs;
	size_t snum;
{
	int err = 0;
	ensure(sp, return (-EFAULT));
	ensure(saddrs || !snum, return (-EFAULT));
	SCTPHASH_WLOCK();
	if (sp->saddr || sp->sanum) {
		rare();
		__sctp_free_saddrs(sp);
	}			/* start clean */
	sp->sport = sport;
	if (snum) {
		while (snum--)
			if (!__sctp_saddr_alloc(sp, saddrs[snum], &err) && err) {
				rare();
				break;
			}
	} else
		usual(snum);
	if (err) {
		rare();
		__sctp_free_saddrs(sp);
	}
	SCTPHASH_WUNLOCK();
	return (err);
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
struct sctp_strm *
sctp_strm_alloc(stp, sid, errp)
	struct sctp_strm **stp;
	uint16_t sid;
	int *errp;
{
	struct sctp_strm *st;
	if ((st = kmem_cache_alloc(sctp_strm_cachep, SLAB_ATOMIC))) {
		bzero(st, sizeof(*st));
		if ((st->next = (*stp)))
			st->next->prev = &st->next;
		st->prev = stp;
		(*stp) = st;
		st->sid = sid;
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
sctp_strm_free(st)
	struct sctp_strm *st;
{
	assert(st);
	if ((*st->prev = st->next))
		st->next->prev = st->prev;
	kmem_cache_free(sctp_strm_cachep, st);
}

/* 
 *  Free all Streams
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_free_strms(sp)
	sctp_t *sp;
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
 *  SCTP Private Data Structure Functions
 *
 *  =========================================================================
 *
 *  We use Linux hardware aligned cache here for speedy access to information
 *  contained in the private data structure.
 */
sctp_t *
sctp_alloc_priv(q, spp, cmajor, cminor, ops)
	queue_t *q;
	sctp_t **spp;
	int cmajor;
	int cminor;
	struct sctp_ifops *ops;
{
	sctp_t *sp;
	assert(q);
	assert(spp);
	/* must have these 4 */
	ensure(ops->sctp_conn_ind, return (NULL));
	ensure(ops->sctp_conn_con, return (NULL));
	ensure(ops->sctp_data_ind, return (NULL));
	ensure(ops->sctp_discon_ind, return (NULL));
	assure(cmajor);
	assure(cminor);
	if ((sp = kmem_cache_alloc(sctp_sctp_cachep, SLAB_ATOMIC))) {
		MOD_INC_USE_COUNT;
		bzero(sp, sizeof(*sp));
		/* link into master list */
		if ((sp->next = *spp))
			sp->next->prev = &sp->next;
		sp->prev = spp;
		*spp = sp;
		RD(q)->q_ptr = WR(q)->q_ptr = sp;
		sp->rq = RD(q);
		sp->wq = WR(q);
		sp->cmajor = cmajor;
		sp->cminor = cminor;
		sp->ops = ops;
		sp->i_state = 0;
		sp->s_state = SCTP_CLOSED;
		/* ip defaults */
		sp->ip_tos = sctp_default_ip_tos;
		sp->ip_ttl = sctp_default_ip_ttl;
		sp->ip_proto = sctp_default_ip_proto;
		sp->ip_dontroute = sctp_default_ip_dontroute;
		sp->ip_broadcast = sctp_default_ip_broadcast;
		sp->ip_priority = sctp_default_ip_priority;
		/* per association defaults */
		sp->max_istr = sctp_default_max_istreams;
		sp->req_ostr = sctp_default_req_ostreams;
		sp->max_inits = sctp_default_max_init_retries;
		sp->max_retrans = sctp_default_assoc_max_retrans;
		sp->a_rwnd = sctp_default_rmem;
		sp->ck_life = sctp_default_valid_cookie_life;
		sp->ck_inc = sctp_default_cookie_inc;
		sp->hmac = sctp_default_mac_type;
		sp->throttle = sctp_default_throttle_itvl;
		sp->sid = sctp_default_sid;
		sp->ppi = sctp_default_ppi;
		sp->max_sack = sctp_default_max_sack_delay;
		/* per destination association defaults */
		sp->rto_ini = sctp_default_rto_initial;
		sp->rto_min = sctp_default_rto_min;
		sp->rto_max = sctp_default_rto_max;
		sp->rtx_path = sctp_default_path_max_retrans;
		sp->hb_itvl = sctp_default_heartbeat_itvl;
		bufq_init(&sp->rcvq);
		bufq_init(&sp->sndq);
		bufq_init(&sp->urgq);
		bufq_init(&sp->errq);
		bufq_init(&sp->oooq);
		bufq_init(&sp->dupq);
		bufq_init(&sp->rtxq);
		bufq_init(&sp->ackq);
		bufq_init(&sp->conq);
		sctp_init_lock(sp);
		sp->pmtu = 576;
		sp->s_state = SCTP_CLOSED;
	}
	usual(sp);
	return (sp);
}
static void
sctp_free_priv(q)
	queue_t *q;
{
	sctp_t *sp;
	ensure(q, return);
	sp = SCTP_PRIV(q);
	ensure(sp, return);
	SCTPHASH_WLOCK();
	{
		sp->s_state = SCTP_CLOSED;
		__sctp_unhash(sp);
		if (sp->timer_init) {
			rare();
			untimeout(xchg(&sp->timer_init, 0));
		}
		if (sp->timer_cookie) {
			rare();
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		if (sp->timer_shutdown) {
			rare();
			untimeout(xchg(&sp->timer_shutdown, 0));
		}
		if (sp->timer_sack) {
			rare();
			untimeout(xchg(&sp->timer_sack, 0));
		}
		if (sp->saddr) {
			__sctp_free_saddrs(sp);
		}
		if (sp->daddr) {
			__sctp_free_daddrs(sp);
		}
	}
	SCTPHASH_WUNLOCK();
	unusual(sp->retry);
	freechunks(xchg(&sp->retry, NULL));
	sp->sackf = 0;
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
	unusual(sp->conq.q_msgs);
	bufq_purge(&sp->conq);
	unusual(sp->rq->q_count);
	flushq(sp->rq, FLUSHALL);
	unusual(sp->wq->q_count);
	flushq(sp->wq, FLUSHALL);
	unusual(sp->rcvq.q_msgs);
	bufq_purge(&sp->rcvq);
	unusual(sp->sndq.q_msgs);
	bufq_purge(&sp->sndq);
	unusual(sp->urgq.q_msgs);
	bufq_purge(&sp->urgq);
	unusual(sp->errq.q_msgs);
	bufq_purge(&sp->errq);
	unusual(sp->dupq.q_msgs);
	bufq_purge(&sp->dupq);
	unusual(sp->dups);
	sp->dups = NULL;
	unusual(sp->ndups);
	sp->ndups = 0;
#ifdef _DEBUG
	if (sp->oooq.q_msgs && sp->oooq.q_head) {
		mblk_t *mp;
		for (mp = sp->oooq.q_head; mp; mp = mp->b_next) {
			struct sctp_tcb *cb = SCTP_TCB(mp);
			printk("oooq tsn = %u\n", cb->tsn);
		}
	}
#endif
	unusual(sp->oooq.q_msgs);
	bufq_purge(&sp->oooq);
	unusual(sp->gaps);
	sp->gaps = NULL;
	unusual(sp->ngaps);
	sp->ngaps = 0;
	unusual(sp->nunds);
	sp->nunds = 0;
#ifdef _DEBUG
	if (sp->rtxq.q_msgs && sp->rtxq.q_head) {
		mblk_t *mp;
		for (mp = sp->rtxq.q_head; mp; mp = mp->b_next) {
			struct sctp_tcb *cb = SCTP_TCB(mp);
			printk("rtxq tsn = %u\n", cb->tsn);
		}
	}
#endif
	unusual(sp->rtxq.q_msgs);
	bufq_purge(&sp->rtxq);
	unusual(sp->nrtxs);
	sp->nrtxs = 0;
	unusual(sp->ackq.q_msgs);
	bufq_purge(&sp->ackq);
	/* do we really need to keep this stuff hanging around for retrieval? */
	if (sp->ostrm || sp->istrm) {
		sctp_free_strms(sp);
	}
	unusual(RD(q)->q_count);
	if (RD(q)->q_count)
		flushq(RD(q), FLUSHALL);
	unusual(WR(q)->q_count);
	if (WR(q)->q_count)
		flushq(WR(q), FLUSHALL);
	RD(q)->q_ptr = WR(q)->q_ptr = NULL;
	if ((*sp->prev = sp->next))
		sp->next->prev = sp->prev;
	sp->next = NULL;
	sp->prev = NULL;
	kmem_cache_free(sctp_sctp_cachep, sp);
	MOD_DEC_USE_COUNT;
}

/* 
 *  DISCONNECT
 *  -------------------------------------------------------------------------
 *  Disconnect the STREAM.  The caller must send a disconnect indication to
 *  the user interface if necessary and send an abort if necessary.  This just
 *  pulls the stream out of the hashes, stops timers, frees simple
 *  retransmission, and zeros connection info.  The stream is left bound and
 *  destination addressses are left allocated.  Any connection indications are
 *  left queued against the stream.
 */
static void
sctp_disconnect(sp)
	sctp_t *sp;
{
	assert(sp);
	SCTPHASH_WLOCK();
	{
		struct sctp_daddr *sd;
		sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
		/* remove from connected hashes */
		__sctp_conn_unhash(sp);
		/* stop timers */
		if (sp->timer_init)
			untimeout(xchg(&sp->timer_init, 0));
		if (sp->timer_cookie)
			untimeout(xchg(&sp->timer_cookie, 0));
		if (sp->timer_shutdown)
			untimeout(xchg(&sp->timer_shutdown, 0));
		if (sp->timer_sack)
			untimeout(xchg(&sp->timer_sack, 0));
		for (sd = sp->daddr; sd; sd = sd->next) {
			if (sd->dst_cache)
				dst_release(xchg(&sd->dst_cache, 0));
			if (sd->timer_idle)
				untimeout(xchg(&sd->timer_idle, 0));
			if (sd->timer_retrans) {
				seldom();
				untimeout(xchg(&sd->timer_retrans, 0));
			}
			if (sd->timer_heartbeat) {
				seldom();
				untimeout(xchg(&sd->timer_heartbeat, 0));
			}
		}
	}
	SCTPHASH_WUNLOCK();
	unusual(sp->retry);
	freechunks(xchg(&sp->retry, NULL));
	sp->sackf = 0;
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
}

/* 
 *  Non-locking version for use from within timeouts (runninga at
 *  bottom-half so don't do bottom-half locks).
 */
static void
__sctp_disconnect(sp)
	sctp_t *sp;
{
	struct sctp_daddr *sd;
	assert(sp);
	SCTPHASH_BH_WLOCK();
	{
		sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
		/* remove from connected hashes */
		__sctp_conn_unhash(sp);
		/* stop timers */
		if (sp->timer_init) {
			seldom();
			untimeout(xchg(&sp->timer_init, 0));
		}
		if (sp->timer_cookie) {
			seldom();
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		if (sp->timer_shutdown) {
			seldom();
			untimeout(xchg(&sp->timer_shutdown, 0));
		}
		if (sp->timer_sack) {
			seldom();
			untimeout(xchg(&sp->timer_sack, 0));
		}
		for (sd = sp->daddr; sd; sd = sd->next) {
			if (sd->dst_cache) {
				seldom();
				dst_release(xchg(&sd->dst_cache, 0));
			}
			if (sd->timer_idle) {
				seldom();
				untimeout(xchg(&sd->timer_idle, 0));
			}
			if (sd->timer_retrans) {
				rare();
				untimeout(xchg(&sd->timer_retrans, 0));
			}
			if (sd->timer_heartbeat) {
				rare();
				untimeout(xchg(&sd->timer_heartbeat, 0));
			}
		}
	}
	SCTPHASH_BH_WUNLOCK();
	unusual(sp->retry);
	freechunks(xchg(&sp->retry, NULL));
	sp->sackf = 0;
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
}

/* 
 *  UNBIND
 *  -------------------------------------------------------------------------
 *  We should already be in a disconnected state.  This pulls us from the bind
 *  hashes and deallocates source addresses, any connection indications that
 *  are queued against the stream are purged (these might occur if a
 *  connection indication comes in just before we unbind and we have made an
 *  error somewhere else: normally the response to an X_UNBIND_REQ in a
 *  connection indication state will be a X_ERROR_ACK).
 */
static void
sctp_unbind(sp)
	sctp_t *sp;
{
	assert(sp);
	SCTPHASH_WLOCK();
	{
		sp->s_state = SCTP_CLOSED;
		__sctp_bind_unhash(sp);
		__sctp_free_saddrs(sp);
	}
	SCTPHASH_WUNLOCK();
	unusual(sp->conq.q_msgs);
	bufq_purge(&sp->conq);
}

/* 
 *  RESET
 *  -------------------------------------------------------------------------
 *  Clear the connection information hanging around on a stream.  This include
 *  any queued data blocks that are waitinga around for retrieval.  It is OK
 *  to call this function twice in a row on the same streeam.
 */
static void
sctp_reset(sp)
	sctp_t *sp;
{
	unusual(sp->rq->q_count);
	flushq(sp->rq, FLUSHALL);
	unusual(sp->wq->q_count);
	flushq(sp->wq, FLUSHALL);
	sp->pmtu = 576;
	/* purge queues */
	unusual(sp->rcvq.q_msgs);
	bufq_purge(&sp->rcvq);
	unusual(sp->sndq.q_msgs);
	bufq_purge(&sp->sndq);
	unusual(sp->urgq.q_msgs);
	bufq_purge(&sp->urgq);
	unusual(sp->errq.q_msgs);
	bufq_purge(&sp->errq);
	unusual(sp->dupq.q_msgs);
	bufq_purge(&sp->dupq);
	unusual(sp->dups);
	sp->dups = NULL;
	unusual(sp->ndups);
	sp->ndups = 0;
	unusual(sp->oooq.q_msgs);
	bufq_purge(&sp->oooq);
	unusual(sp->gaps);
	sp->gaps = NULL;
	unusual(sp->ngaps);
	sp->ngaps = 0;
	unusual(sp->nunds);
	sp->nunds = 0;
	unusual(sp->rtxq.q_msgs);
	bufq_purge(&sp->rtxq);
	unusual(sp->nrtxs);
	sp->nrtxs = 0;
	unusual(sp->ackq.q_msgs);
	bufq_purge(&sp->ackq);
	if (sp->ostrm || sp->istrm) {
		sctp_free_strms(sp);
	}
}
sctp_t *sctp_bhash[sctp_bhash_size] = { NULL, };	/* bind */
sctp_t *sctp_lhash[sctp_lhash_size] = { NULL, };	/* listen */
sctp_t *sctp_phash[sctp_phash_size] = { NULL, };	/* p_tag */
sctp_t *sctp_cache[sctp_cache_size] = { NULL, };	/* v_tag *//* level 1 */
sctp_t *sctp_vhash[sctp_vhash_size] = { NULL, };	/* v_tag *//* level 2 */
sctp_t *sctp_thash[sctp_thash_size] = { NULL, };	/* tcb *//* level 4 */
STATIC void
sctp_bhash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_bhash[sctp_sp_bhashfn(sp)];
	if ((sp->bnext = *spp))
		sp->bnext->bprev = &sp->bnext;
	sp->bprev = spp;
	*spp = sp;
}
STATIC void
sctp_lhash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_lhash[sctp_sp_lhashfn(sp)];
	if ((sp->lnext = *spp))
		sp->lnext->lprev = &sp->lnext;
	sp->lprev = spp;
	*spp = sp;
}
STATIC void
sctp_phash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_phash[sctp_sp_phashfn(sp)];
	if ((sp->pnext = *spp))
		sp->pnext->pprev = &sp->pnext;
	sp->pprev = spp;
	*spp = sp;
}
STATIC void
sctp_vhash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_vhash[sctp_sp_vhashfn(sp)];
	sctp_t **scp = &sctp_cache[sctp_sp_cachefn(sp)];
	if ((sp->vnext = *spp))
		sp->vnext->vprev = &sp->vnext;
	sp->vprev = spp;
	*spp = sp;
	*scp = sp;
}
STATIC void
sctp_thash_insert(sctp_t * sp)
{
	sctp_t **spp = &sctp_thash[sctp_sp_thashfn(sp)];
	if ((sp->tnext = *spp))
		sp->tnext->tprev = &sp->tnext;
	sp->tprev = spp;
	*spp = sp;
}
STATIC void
sctp_bhash_unhash(sctp_t * sp)
{
	if (sp->bprev) {
		if ((*(sp->bprev) = sp->bnext))
			sp->bnext->bprev = sp->bprev;
		sp->bnext = NULL;
		sp->bprev = NULL;
	}
}
STATIC void
sctp_lhash_unhash(sctp_t * sp)
{
	if (sp->lprev) {
		if ((*(sp->lprev) = sp->lnext))
			sp->lnext->lprev = sp->lprev;
		sp->lnext = NULL;
		sp->lprev = NULL;
	}
}
STATIC void
sctp_phash_unhash(sctp_t * sp)
{
	if (sp->pprev) {
		if ((*(sp->pprev) = sp->pnext))
			sp->pnext->pprev = sp->pprev;
		sp->pnext = NULL;
		sp->pprev = NULL;
	}
}
STATIC void
sctp_vhash_unhash(sctp_t * sp)
{
	if (sp->vprev) {
		if ((*(sp->vprev) = sp->vnext))
			sp->vnext->vprev = sp->vprev;
		sp->vnext = NULL;
		sp->vprev = NULL;
		if (sctp_cache[sp->hashent] == sp)
			sctp_cache[sp->hashent] = NULL;
		sp->hashent = 0;
	}
}
STATIC void
sctp_thash_unhash(sctp_t * sp)
{
	if (sp->tprev) {
		if ((*(sp->tprev) = sp->tnext))
			sp->tnext->tprev = sp->tprev;
		sp->tnext = NULL;
		sp->tprev = NULL;
	}
}
static void
__sctp_unhash(sctp_t * sp)
{
	assert(sp);
	sctp_phash_unhash(sp);
	sctp_thash_unhash(sp);
	sctp_vhash_unhash(sp);
	sctp_lhash_unhash(sp);
	sctp_bhash_unhash(sp);
}
STATIC void
__sctp_phash_rehash(sctp_t * sp)
{
	sctp_phash_unhash(sp);
	if (sp->p_tag)
		sctp_phash_insert(sp);
}
static void
sctp_phash_rehash(sctp_t * sp)
{
	SCTPHASH_WLOCK(); {
		__sctp_phash_rehash(sp);
	} SCTPHASH_WUNLOCK();
}

/* 
 *  Add to Bind Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
sctp_bind_hash(sp, cons)
	sctp_t *sp;
	uint cons;
{
	assert(sp);
	SCTPHASH_WLOCK();
	{
		unusual(sp->bprev);
		sctp_bhash_unhash(sp);	/* start clean */
		unusual(sp->lprev);
		sctp_lhash_unhash(sp);	/* start clean */
		if (cons) {
			sctp_t *sp2, **spp = &sctp_lhash[sctp_sp_lhashfn(sp)];
			/* check for conflicts */
			for (sp2 = *spp; sp2; sp2 = sp2->lnext) {
				if (!sp2->sport || !sp->sport || sp2->sport == sp->sport) {
					if (sp2->sanum && sp->sanum) {
						struct sctp_saddr *ss, *ss2;
						for (ss = sp->saddr; ss; ss = ss->next)
							for (ss2 = sp2->saddr; ss2; ss2 = ss2->next) {
								if (ss->saddr != ss2->saddr)
									continue;
								SCTPHASH_WUNLOCK();
								return (-EADDRINUSE);
							}
					} else {
						SCTPHASH_WUNLOCK();
						return (-EADDRINUSE);
					}
				}
			}
			sctp_lhash_insert(sp);
		}
		sctp_bhash_insert(sp);
	}
	SCTPHASH_WUNLOCK();
	return (0);
}

/* 
 *  Remove from Bind Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static void
__sctp_bind_unhash(sp)
	sctp_t *sp;
{
	assert(sp);
	sctp_lhash_unhash(sp);
	sctp_bhash_unhash(sp);
}

/* 
 *  Add to listening Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This also checks for conflicts.  This uses the rule that no two sockets
 *  can be listening on the same primary transport address (IP/port).
 */

/* 
 *  Get Local Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Get all the available local addresses and build the source address list
 *  from those that are available.
 */
STATIC int
__sctp_get_addrs(sp, daddr)
	sctp_t *sp;
	uint32_t daddr;
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
			if (LOOPBACK(ifa->ifa_locak) != LOOPBACK(daddr))
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
sctp_get_addrs(sp, daddr)
	sctp_t *sp;
	uint32_t daddr;
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
 *  This also checks for conflicts.  This uses the rule that there cannot be
 *  more than one SCTP association between any given pair of local and remote
 *  transport addresses (IP/port).
 */
STATIC int
sctp_conn_hash(sp)
	sctp_t *sp;
{
	sctp_t *sp2, **spp;
	struct sctp_saddr *ss, *ss2, *ss_next;
	struct sctp_daddr *sd, *sd2;
	assert(sp);
	SCTPHASH_WLOCK();
	unusual(sp->tprev);
	sctp_thash_unhash(sp);	/* start clean */
	unusual(sp->vprev);
	sctp_vhash_unhash(sp);	/* start clean */
	unusual(sp->pprev);
	sctp_phash_unhash(sp);	/* start clean */
	/* Check for conflicts: we search the TCB hash list for other streams with the same port
	   pair and with the same pairing of src and dst addresses.  We do this with write locks on 
	   for the hash list so that we can add our stream if we succeed.  This is the TCB hash, so
	   even if this is a long search, we are only blocking other connection hash calls and TCB
	   lookups (for ootb packets) for this hash slot. */
	spp = &sctp_thash[sctp_sp_thashfn(sp)];
	for (sp2 = *spp; sp2; sp2 = sp2->tnext) {
		if (sp2->sport != sp->sport || sp2->dport != sp->dport)
			continue;
		ss_next = sp->saddr;
		while ((ss = ss_next)) {
			ss_next = ss->next;
			for (ss2 = sp2->saddr; ss2; ss2 = ss2->next) {
				if (ss->saddr != ss2->saddr)
					continue;
				for (sd = sp->daddr; sd; sd = sd->next)
					for (sd2 = sp2->daddr; sd2; sd2 = sd2->next) {
						if (sd->daddr != sd2->daddr)
							continue;
						/* Now, if we have a matching
						   saddr/sport/daddr/dport n-tuple, then we should
						   check if we are SCTP_BINADDR_LOCK'd.  If we are
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
		      next_saddr:
		}
	}
	sctp_thash_insert(sp);
	sctp_vhash_insert(sp);
	sctp_phash_insert(sp);
	SCTPHASH_WUNLOCK();
	return (0);
      eaddrinuse:
	SCTPHASH_WUNLOCK();
	return (-EADDRINUSE);	/* conflict */
}

/* 
 *  Remove from Connection Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static void
__sctp_conn_unhash(sp)
	sctp_t *sp;
{
	assert(sp);
	sctp_thash_unhash(sp);
	sctp_vhash_unhash(sp);
	sctp_phash_unhash(sp);
}

/* 
 *  Get a fresh unused local port number
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int sctp_port_rover = 0;
#ifdef sysctl_local_port_range
extern int sysctl_local_port_range[2];
#else
STATIC int sysctl_local_port_range[2] = { 1024, 4999 };
#endif
uint16_t
sctp_get_port(void)
{
	uint16_t snum;
	uint16_t sport = 0;
	sctp_t *sb = NULL;
	int low = sysctl_local_port_range[0];
	int high = sysctl_local_port_range[1];
	int rem = (high - low) + 1;
	seldom();
	snum = sctp_port_rover;
	if (snum > high || snum < low) {
		rare();
		snum = low;
	}
	/* find a fresh, completely unused port number */
	for (; rem > 0; snum++, rem--) {
		if (snum > high || snum < low) {
			rare();
			snum = low;
		}
		sport = htons(snum);
		if (!(sb = sctp_bhash[sctp_bhashfn(sport)]))
			break;
		for (; sb; sb = sb->bnext)
			if (sb->sport == sport) {
				seldom();
				break;
			}
		if (sb)
			break;
	}
	if (rem <= 0 || sb) {
		rare();
		return (0);
	}
	sctp_port_rover = snum;
	usual(sport);
	return (sport);
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
 *  IMPLEMENTATION NOTES:- All but a few SCTP messages carry our Verification
 *  Tag.  If the message requires our Verification Tag and we cannot lookup
 *  the stream on the Verification Tag we treat the packet similar to an OOTB
 *  packet.  The only restriction that this approach imposes is in the
 *  selection of our Verification Tag, which cannot be identical to any other
 *  Verification Tag which we have chosen so far.  We, therefore, check the
 *  Verification Tag selected at initialization against the cache for
 *  uniqueness.  This also allows us to acquire the Verification Tag to
 *  minimize collisions on the hash table.  This allows us to simplify the
 *  hashing function because we are guaranteeing equal hash coverage using
 *  selection.
 */
/* 
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP LISTEN - LISTEN hash (sport)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Listener stream lookup with wildcards.  This will find any bound listener
 *  stream from the destination address, destination port and device index.
 *  This is only for INIT and COOKIE ECHO.
 */
STATIC sctp_t *
sctp_lookup_listen(uint16_t dport, uint32_t daddr)
{
	sctp_t *sp, *result = NULL;
	int hiscore = 0;
	for (sp = sctp_lhash[sctp_lhashfn(dport)]; sp; sp = sp->next) {
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
	usual(result);
	return result;
}

/* 
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP BIND - BIND hash (port)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Like the listen lookup, but done on the bind hashes.  This is to find
 *  potential conflicts for INIT.  We are just lookup for a non-closed stream
 *  bound to the port with a compatible destination address considering
 *  wildcards.  This is used for exceptional INIT cases when no listening
 *  stream is found.  Port locks must have been taken on the bind (the port
 *  must have been specified by the user and not dynamically assigned
 *  otherwise the socket will not accept INITs).
 */
STATIC sctp_t *
sctp_lookup_bind(dport, daddr)
	uint16_t dport;
	uint32_t daddr;
{
	sctp_t *sp, *result = NULL;
	int hiscore = 0;
	for (sp = sctp_bhash[sctp_bhashfn(dport)]; sp; sp = sp->bnext) {
		int score = 0;
		if ((1 << sp->s_state) & (SCTPF_DISCONNECTED))
			continue;
		if (!(sp->userlocks & SCTP_BINDPORT_LOCK))
			continue;
		if (sp->sport) {
			if (sp->sport != sport)
				continue;
			score++;
			if (sp->saddr) {
				struct sctp_saddr *ss;
				if (!(ss = sctp_find_saddr(sk, daddr)))
					continue;
				if (ss->flags & SCTP_SRCEM_ASCONF)
					continue;
				score++;
			}
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
	usual(result);
	return result;
}

/* 
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP - TCB hash (port pair)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We do the hard match here, because we don't have valid v_tags or p_tags.
 *  We don't have the information anyways.  We are just looking for an
 *  established stream which can accept the packet based on port numbers and
 *  source and destination addresses.  This is used for INIT, OOTB determination
 *  as well as ICMP lookups for failed sent INIT messages.
 */
#define sctp_match_tcb(__sp, __saddr, __daddr, __sport, __dport) \
	( ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__saddr))) && \
	  (sctp_find_daddr((__sp),(__daddr))) )
STATIC sctp_t *
sctp_lookup_tcb(sport, dport, saddr, daddr)
	uint16_t sport;
	uint16_t dport;
	uint32_t saddr;
	uint32_t daddr;
{
	sctp_t *sp;
	for (sp = sctp_thash[sctp_thashfn(sport, dport)]; sp; sp = sp->tnext)
		if (sctp_match_tcb(sp, saddr, daddr, sport, dport))
			break;
	return sp;
}

/* 
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP PTAG - Peer hash (peer tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We can either match fast and loose or slow and sure.  We have a peer tag
 *  which is validation enough without walking the address lists most of the
 *  time.  The INIT and COOKIE ECHO stuff needs it for checking peer tags and
 *  peer tie tags.  ICMP needs this for looking up all packets which were
 *  returned that we sent out with the peer's tag (excepts INIT which has no
 *  tag, we use TCB ICMP lookup for that).  ICMP lookups match with reversed
 *  source and destination addresses.
 *
 *  ADD-IP must ignore addresses at times (when retransmitting an ASCONF which
 *  successfully deleted an IP address but for which the ASCONF ACK was lost).
 *  Therefore, we always use just verification tag and port for ADD-IP
 *  regardless of the slow verification setting.
 */
#if !defined SCTP_CONFIG_SLOW_VERIFICATION || defined SCTP_CONFIG_ADD_IP
#define	sctp_match_ptag(__sp, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == (__sp)->p_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) )
#else
#define	sctp_match_ptag(__sp, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == (__sp)->p_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__saddr))) && \
	  (sctp_find_daddr((__sp),(__daddr))) )
#endif
STATIC sctp_t *
sctp_lookup_ptag(p_tag, sport, dport, saddr, daddr)
	uint32_t p_tag;
	uint16_t sport;
	uint16_t dport;
	uint32_t saddr;
	uint16_t daddr;
{
	sctp_t *sp;
	(void) saddr;
	(void) daddr;
	for (sp = sctp_phash[sctp_phashfn(p_tag)]; sp; sp = sp->pnext)
		if (sctp_match_ptag(sp, saddr, daddr, p_tag, sport, dport))
			break;
	return sp;
}

/* 
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP VTAG - Established hash (local tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  This is the main lookup for data transfer on established streams.  This
 *  should run as fast and furious as possible.  We run fast and loose and
 *  rely on the verification tag and port numbers only.  We cache and hash
 *  so a stream of back-to-back packets to the same destination (bursty
 *  traffic) will whirl.  Because we MD4 hash verification tags when we
 *  generate them, the hash should get good random distribution with minimum
 *  collisions.
 *
 *  ADD-IP must ignore addresses at times (when retransmitting an ASCONF which
 *  successfully deleted an IP address but for which the ASCONF ACK was lost).
 *  Therefore, we always use just verificiation tag for ADD-IP regardless of the
 *  slow verificiation setting.
 */
#if !defined SCTP_CONFIG_SLOW_VERIFICATION || defined SCTP_CONFIG_ADD_IP
#define sctp_match_vtag(__sp, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == (__sp)->v_tag) )
#else
#define sctp_match_vtag(__sp, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == (__sp)->v_tag) && \
	  ((__sport) == (__sp)->sport) && \
	  ((__dport) == (__sp)->dport) && \
	  (sctp_find_saddr((__sp),(__daddr))) )
#endif
STATIC sctp_t *
sctp_lookup_vtag(v_tag, sport, dport, saddr, daddr)
	uint32_t v_tag;
	uint16_t sport;
	uint16_t dport;
	uint32_t saddr;
	uint16_t daddr;
{
	sctp_t *sp;
	unsigned int hash = sctp_cachefn(v_tag);
	(void) saddr;
	(void) daddr;
	if (!(sp = sctp_cache[hash]) || !sctp_match_vtag(sp, saddr, daddr, v_tag, sport, dport))
		for (sp = sctp_vhash[sctp_vhashfn(v_tag)]; sp; sp = sp->next)
			if (sctp_match_vtag(sp, saddr, daddr, v_tag, sport, dport)) {
				sctp_cache[hash] = sp;
				break;
			}
	return sp;
}

/* 
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP COOKIE ECHO - Special lookup rules for cookie echo chunks
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC sctp_t *
sctp_lookup_cookie_echo(ck, v_tag, sport, dport, saddr, daddr)
	struct sctp_cookie *ck;
	uint32_t v_tag;
	uint16_t dport;
	uint16_t sport;
	uint32_t daddr;
	uint32_t saddr;
{
	sctp_t *sp = NULL;
	/* quick sanity checks on cookie */
	if (ck->v_tag == v_tag && ck->sport == sport && ck->dport == dport) {
		if (		/* RFC 2960 5.2.4 (A) */
			   (ck->l_ttag && ck->p_ttag
			    && (sp = sctp_lookup_vtag(ck->l_ttag, sport, dport, saddr, daddr))
			    && sp == sctp_lookup_ptag(sk->p_ttag, sport, dport, saddr, daddr))
			   /* RFC 2960 5.2.4 (B) or (D) */
			   || ((sp = sctp_lookup_vtag(v_tag, sport, dport, saddr, daddr)))
			   /* RFC 2960 5.2.4 (C) or IG 2.6 replacement */
			   || (!ck->l_ttag && !ck->p_ttag
			       && ((sp = sctp_lookup_ptag(ck->p_tag, sport, dport, saddr, daddr))
				   || (sp = sctp_lookup_tcb(sport, dport, saddr, ck->daddr))))
			   /* RFC 2960 5.1 */
			   || ((sp = sctp_lookup_listen(sport, saddr))))
			if (sp->s_state == SCTP_LISTEN || (sp->userlocks & SCTP_BINDPORT_LOCK))
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
 *  Stream hash lookup with fast path for data.  This uses verification tag
 *  when it is available.  Source address and port are checked after the
 *  verification tag matches.  When called for INIT and COOKIE ECHO messages,
 *  the function returns a listening (bind) hash lookup.  For SHUTDOWN
 *  COMPLETE and ABORT messages with the T-bit set, an icmp (peer tag) lookup
 *  is performed instead.
 *
 *  Note: SCTP IG 2.18 is unnecessary.
 */
sctp_t *
sctp_lookup(struct sctphdr * sh, uint32_t daddr, uint32_t saddr)
{
	sctp_t *sp = NULL;
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
			struct sctp_cookie *ck =
			    (struct sctp_cookie *) ((struct sctp_cookie_echo *) ch)->cookie;
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

/* XXX: put these somewhere else */
static void
sctp_init_locks(void)
{
	rwlock_init(&sctp_hash_lock);
}
static void
sctp_term_locks(void)
{
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
	while (!(ret = secure_tcp_sequence_number(daddr, saddr, dport, sport))
	       || sctp_lookup_vtag(ret, sport, dport, saddr, daddr)) ;
	return (ret);
}

#define HMAC_SIZE (160/8)
#define raw_cookie_size(__ck) \
		( sizeof(struct sctp_cookie) \
		+ ((__ck)->opt_len) \
		+ ((__ck)->danum * sizeof(uint32_t)) \
		+ ((__ck)->sanum * sizeof(uint32_t)) )

/* 
 *  =========================================================================
 *
 *  SHA-1
 *
 *  =========================================================================
 *  
 *  This is a GPL version fo the FIPS 180-1 Secure Hash Algorithm written by
 *  Brian Bidulock <bidulock@openss7.org>.
 *
 *  Adapted from code written November 2000 by David Ireland of DI Management
 *  Services Pty Limited <code@di-mgt.com.au>
 *  
 *  Adapted from code in the Python Cryptography Toolkit, version 1.0.0 by
 *  A.M.  Kuchling 1995.
 */
#ifdef SCTP_CONFIG_HMAC_SHA1

/* 
 *  The structure for storing SHS info
 */
typedef struct {
	uint32_t dig[5];		/* Message digest */
	uint32_t lo, hi;		/* 64-bit bit count */
	uint32_t dat[16];		/* SHS data buffer */
} SHA_CTX;
/* 
 *  The SHS f()-functions.  The f1 and f3 functions can be optimized to save one
 *  boolean operation each - thanks to Rich Schroeppel, rcs@cs.arizona.edu for
 *  discovering this
 */
#define f1(x,y,z)   (z^(x&(y^z)))	/* Rounds 0-19 */
#define f2(x,y,z)   (x^y^z)	/* Rounds 20-39 */
#define f3(x,y,z)   ((x&y)|(z&(x|y)))	/* Rounds 40-59 */
#define f4(x,y,z)   (x^y^z)	/* Rounds 60-79 */
/* 
 *  32-bit rotate left - kludged with shifts
 */
#define ROTL(n,X)   (((X)<<n)|((X)>>(32-n)))
/* 
 *  The initial expanding function.  The hash function is defined over an
 *  80-UINT2 expanded input array W, where the first 16 are copies of the input
 *  data, and the remaining 64 are defined by
 *
 *      W[i] = W[i - 16] ^ W[i - 14] ^ W[i - 8] ^ W[i - 3]
 *
 *  This implementation generates these values on the fly in a circular buffer -
 *  thanks to Colin Plumb, colin@nyx10.cs.du.edu for this optimization.
 *
 *  The updated SHS changes the expanding function by adding a rotate of 1 bit.
 *  Thanks to Jim Gillogly, jim@rand.org, and an anonymous contributor for this
 *  information
 */
#define x(W,i) \
	(W[i & 15] = \
	  ROTL(1, (W[i&15] ^ W[(i-14)&15] ^ W[(i-8)&15] ^ W[(i-3)&15])))
/* 
 *  The prototype SHS sub-round.  The fundamental sub-round is:
 *
 *      a' = e + ROTL(5, a) + f(b, c, d) + k + data;
 *      b' = a;
 *      c' = ROTL(30, b);
 *      d' = c;
 *      e' = d;
 *
 *  but this is implemented by unrolling the loop 5 times and renaming the
 *  variables (e, a, b, c, d) = (a', b', c', d', e') each iteration.  This
 *  code is then replicated 20 times for each of the 4 functions, using the next
 *  20 values from the W[] array each time
 */
#define subRound(a,b,c,d,e,f,k,data) \
    (e += ROTL(5,a)+f(b,c,d)+k+data, b = ROTL(30,b))
/* 
 *  Initialize the SHS values
 */
STATIC void
SHAInit(SHA_CTX * sha1)
{
	/* Set the h-vars to their initial values */
	sha1->dig[0] = 0x67452301L;
	sha1->dig[1] = 0xefcdab89L;
	sha1->dig[2] = 0x98badcfeL;
	sha1->dig[3] = 0x10325476L;
	sha1->dig[4] = 0xc3d2e1f0L;
	/* Initialise bit count */
	sha1->lo = sha1->hi = 0;
}

/* 
 *  Perform the SHS transformation.  Note that this code, like MD5, seems to
 *  break some optimizing compilers due to the complexity of the expressions and
 *  the size of the basic block.  It may be necessary to split it into sections,
 *  e.g. based on the four subrounds
 *
 *  Note that this corrupts the sha1->data area
 */
STATIC void
SHSTransform(uint32_t * dig, uint32_t * dat)
{
	uint32_t A, B, C, D, E;		/* Local vars */
	uint32_t xd[16];		/* Expanded data */
	/* Set up first buffer and local data buffer */
	A = dig[0];
	B = dig[1];
	C = dig[2];
	D = dig[3];
	E = dig[4];
	memcpy(xd, dat, 64);
	/* Heavy mangling, in 4 sub-rounds of 20 interations each. */
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[0]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[1]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[2]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[3]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[4]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[5]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[6]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[7]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[8]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[9]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[10]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[11]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[12]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[13]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[14]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[15]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, x(xd, 16));
	subRound(D, E, A, B, C, f1, 0x5a827999L, x(xd, 17));
	subRound(C, D, E, A, B, f1, 0x5a827999L, x(xd, 18));
	subRound(B, C, D, E, A, f1, 0x5a827999L, x(xd, 19));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 20));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 21));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 22));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 23));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 24));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 25));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 26));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 27));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 28));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 29));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 30));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 31));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 32));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 33));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 34));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 35));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 36));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 37));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 38));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 39));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 40));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 41));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 42));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 43));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 44));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 45));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 46));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 47));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 48));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 49));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 50));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 51));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 52));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 53));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 54));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 55));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 56));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 57));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 58));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 59));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 60));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 61));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 62));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 63));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 64));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 65));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 66));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 67));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 68));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 69));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 70));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 71));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 72));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 73));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 74));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 75));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 76));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 77));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 78));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 79));
	/* Build message digest */
	dig[0] += A;
	dig[1] += B;
	dig[2] += C;
	dig[3] += D;
	dig[4] += E;
}

/* 
 *  When run on a little-endian CPU we need to perform byte reversal on an array
 *  of long words.
 */
#ifdef __LITTLE_ENDIAN
STATIC void
longReverse(uint32_t * buf, int cnt)
{
	uint32_t val;
	cnt /= sizeof(uint32_t);
	while (cnt--) {
		val = *buf;
		val = ((val & 0xff00ff00L) >> 8) | ((val & 0x00ff00ffL) << 8);
		*buf++ = (val << 16) | (val >> 16);
	}
}
#else
#define longReverse(__buf, __cnt) do { } while(0)
#endif
/* 
 *  Update SHS for a block of data
 */
STATIC void
SHAUpdate(SHA_CTX * sha1, uint8_t * buf, int len)
{
	uint32_t tmp;
	int cnt;
	/* Update bitcount */
	tmp = sha1->lo;
	if ((sha1->lo = tmp + ((uint32_t) len << 3)) < tmp)
		sha1->hi++;	/* Carry from low to high */
	sha1->hi += len >> 29;
	/* Get count of bytes already in data */
	cnt = (int) (tmp >> 3) & 0x3F;
	/* Handle any leading odd-sized chunks */
	if (cnt) {
		uint8_t *p = (uint8_t *) sha1->dat + cnt;
		cnt = 64 - cnt;
		if (len < cnt) {
			memcpy(p, buf, len);
			return;
		}
		memcpy(p, buf, cnt);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		buf += cnt;
		len -= cnt;
	}
	/* Process data in 64 chunks */
	while (len >= 64) {
		memcpy(sha1->dat, buf, 64);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		buf += 64;
		len -= 64;
	}
	/* Handle any remaining bytes of data. */
	memcpy(sha1->dat, buf, len);
}

/* 
 *  Final wrapup - pad to 64-byte boundary with the bit pattern 1 0*
 *  (64-bit count of bits processed, MSB-first)
 */
STATIC void
SHAFinal(uint8_t * out, SHA_CTX * sha1)
{
	int len;
	unsigned int i, j;
	uint8_t *dat;
	/* Compute number of bytes mod 64 */
	len = (int) sha1->lo;
	len = (len >> 3) & 0x3F;
	/* Set the first char of padding to 0x80.  This is safe since there is always at least one
	   byte free */
	dat = (uint8_t *) sha1->dat + len;
	*dat++ = 0x80;
	/* Bytes of padding needed to make 64 bytes */
	len = 64 - 1 - len;
	/* Pad out to 56 mod 64 */
	if (len < 8) {
		/* Two lots of padding: Pad the first block to 64 bytes */
		memset(dat, 0, len);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		/* Now fill the next block with 56 bytes */
		memset(sha1->dat, 0, 64 - 8);
	} else
		/* Pad block to 56 bytes */
		memset(dat, 0, len - 8);
	/* Append length in bits and transform */
	sha1->dat[14] = sha1->hi;
	sha1->dat[15] = sha1->lo;
	longReverse(sha1->dat, 64 - 8);
	SHSTransform(sha1->dig, sha1->dat);
	/* Output SHA digest in byte array */
	for (i = 0, j = 0; j < 20; i++, j += 4) {
		out[j + 3] = (uint8_t) (sha1->dig[i] & 0xff);
		out[j + 2] = (uint8_t) ((sha1->dig[i] >> 8) & 0xff);
		out[j + 1] = (uint8_t) ((sha1->dig[i] >> 16) & 0xff);
		out[j] = (uint8_t) ((sha1->dig[i] >> 24) & 0xff);
	}
	/* Zeroise sensitive stuff */
	memset(sha1, 0, sizeof(sha1));
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  HMAC-SHA-1
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2401.
 */
STATIC void
hmac_sha1(uint8_t * text, int tlen, uint8_t * key, int klen, uint8_t * digest)
{
	SHA_CTX context;
	uint8_t k_ipad[64];
	uint8_t k_opad[64];
	uint8_t tk[16];
	int i;
	if (klen > 64) {
		SHA_CTX ctx;
		SHAInit(&ctx);
		SHAUpdate(&ctx, key, klen);
		SHAFinal(tk, &ctx);
		key = tk;
		klen = 16;
	}
	memset(k_ipad, 0, sizeof(k_ipad));
	memset(k_opad, 0, sizeof(k_opad));
	memcpy(k_ipad, key, klen);
	memcpy(k_opad, key, klen);
	for (i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}
	/* inner */
	SHAInit(&context);
	SHAUpdate(&context, k_ipad, 64);
	SHAUpdate(&context, text, tlen);
	SHAFinal(digest, &context);
	/* outer */
	SHAInit(&context);
	SHAUpdate(&context, k_opad, 64);
	SHAUpdate(&context, digest, 20);
	SHAFinal(digest, &context);
}

#endif				/* SCTP_CONFIG_HMAC_SHA1 */

/* 
 *  =========================================================================
 *
 *  MD5
 *
 *  =========================================================================
 */
#ifdef SCTP_CONFIG_HMAC_MD5

typedef struct {
	uint32_t buf[4];
	uint32_t lo, hi;
	uint32_t dat[16];
} MD5_CTX;
/* The four core functions - F1 is optimized somewhat */
/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))
/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,dat,s) \
	 (w += f(x,y,z) + dat, w = (w<<s | w>>(32-s)) + x)
/* 
 *  The core of the MD5 algorithm, this alters an existing MD5 hash to reflect
 *  the addition of 16 longwords of new data.  MD5Update blocks the data and
 *  converts bytes into longwords for this routine.
 */
STATIC void
MD5Transform(uint32_t dig[4], uint32_t const dat[16])
{
	register uint32_t a, b, c, d;
	a = dig[0];
	b = dig[1];
	c = dig[2];
	d = dig[3];
	MD5STEP(F1, a, b, c, d, dat[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, dat[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, dat[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, dat[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, dat[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, dat[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, dat[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, dat[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, dat[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, dat[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, dat[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, dat[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, dat[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, dat[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, dat[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, dat[15] + 0x49b40821, 22);
	MD5STEP(F2, a, b, c, d, dat[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, dat[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, dat[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, dat[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, dat[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, dat[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, dat[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, dat[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, dat[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, dat[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, dat[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, dat[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, dat[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, dat[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, dat[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, dat[12] + 0x8d2a4c8a, 20);
	MD5STEP(F3, a, b, c, d, dat[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, dat[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, dat[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, dat[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, dat[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, dat[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, dat[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, dat[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, dat[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, dat[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, dat[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, dat[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, dat[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, dat[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, dat[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, dat[2] + 0xc4ac5665, 23);
	MD5STEP(F4, a, b, c, d, dat[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, dat[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, dat[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, dat[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, dat[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, dat[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, dat[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, dat[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, dat[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, dat[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, dat[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, dat[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, dat[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, dat[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, dat[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, dat[9] + 0xeb86d391, 21);
	dig[0] += a;
	dig[1] += b;
	dig[2] += c;
	dig[3] += d;
}

#ifdef __BIG_ENDIAN
static void
byteSwap(uint32_t * buf, unsigned cnt)
{
	uint8_t *p = (uint8_t *) buf;
	do {
		*buf++ =
		    (uint32_t) ((unsigned) p[3] << 8 | p[2]) << 16 | ((unsigned) p[1] << 8 | p[0]);
		p += 4;
	} while (--cnt);
}
#else
#define byteSwap(__buf,__cnt)
#endif
/* 
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
STATIC void
MD5Init(MD5_CTX * md5)
{
	md5->buf[0] = 0x67452301;
	md5->buf[1] = 0xefcdab89;
	md5->buf[2] = 0x98badcfe;
	md5->buf[3] = 0x10325476;
	md5->lo = 0;
	md5->hi = 0;
}

/* 
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
STATIC void
MD5Update(MD5_CTX * md5, uint8_t const *buf, unsigned len)
{
	uint32_t t;
	/* Update byte count */
	t = md5->lo;
	if ((md5->lo = t + len) < t)
		md5->hi++;	/* Carry from low to high */
	t = 64 - (t & 0x3f);	/* Space available in md5->dat (at least 1) */
	if (t > len) {
		memcpy((uint8_t *) md5->dat + 64 - t, buf, len);
		return;
	}
	/* First chunk is an odd size */
	memcpy((uint8_t *) md5->dat + 64 - t, buf, t);
	byteSwap(md5->dat, 16);
	MD5Transform(md5->buf, md5->dat);
	buf += t;
	len -= t;
	/* Process data in 64-byte chunks */
	while (len >= 64) {
		memcpy(md5->dat, buf, 64);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		buf += 64;
		len -= 64;
	}
	/* Handle any remaining bytes of data. */
	memcpy(md5->dat, buf, len);
}

/* 
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
STATIC void
MD5Final(uint8_t dig[16], MD5_CTX * md5)
{
	int count = md5->lo & 0x3f;	/* Number of bytes in md5->dat */
	uint8_t *p = (uint8_t *) md5->dat + count;
	/* Set the first char of padding to 0x80.  There is always room. */
	*p++ = 0x80;
	/* Bytes of padding needed to make 56 bytes (-8..55) */
	count = 56 - 1 - count;
	if (count < 0) {	/* Padding forces an extra block */
		memset(p, 0, count + 8);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		p = (uint8_t *) md5->dat;
		count = 56;
	}
	memset(p, 0, count);
	byteSwap(md5->dat, 14);
	/* Append length in bits and transform */
	md5->dat[14] = md5->lo << 3;
	md5->dat[15] = md5->hi << 3 | md5->lo >> 29;
	MD5Transform(md5->buf, md5->dat);
	byteSwap(md5->buf, 4);
	memcpy(dig, md5->buf, 16);
	memset(md5, 0, sizeof(md5));	/* In case it's sensitive */
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  HMAC-MD5
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2401.
 */
STATIC void
hmac_md5(uint8_t * text, int tlen, uint8_t * key, int klen, uint8_t * digest)
{
	MD5_CTX context;
	uint8_t k_ipad[65];
	uint8_t k_opad[65];
	uint8_t tk[16];
	int i;
	if (klen > 64) {
		MD5_CTX ctx;
		MD5Init(&ctx);
		MD5Update(&ctx, key, klen);
		MD5Final(tk, &ctx);
		key = tk;
		klen = 16;
	}
	memset(k_ipad, 0, sizeof(k_ipad));
	memset(k_opad, 0, sizeof(k_opad));
	memcpy(k_ipad, key, klen);
	memcpy(k_opad, key, klen);
	for (i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}
	/* inner */
	MD5Init(&context);
	MD5Update(&context, k_ipad, 64);
	MD5Update(&context, text, tlen);
	MD5Final(digest, &context);
	/* outer */
	MD5Init(&context);
	MD5Update(&context, k_opad, 64);
	MD5Update(&context, digest, 16);
	MD5Final(digest, &context);
}

#endif				/* SCTP_CONFIG_HMAC_MD5 */

/* 
 *  =========================================================================
 *
 *  SCTP Key handling
 *
 *  =========================================================================
 *
 *  This algorithm uses 2^n keys (NUM_KEYS) which are recycled quickly and
 *  evenly.  Each key has a key tag which is placed in the SCTP cookie.  Each
 *  key is valid for the cookie lifespan plus a key life.  The key life should
 *  be set to whatever cookie life increment has been permitted.  When a cookie
 *  is checked for validity, its MAC is verified using the key with the key tag
 *  in the cookie.  If the key has already been recycled, the tagged key will
 *  not fit the lock anymore.  Note that the keys are cycled only as quickly as
 *  the requests for signatures come in.  This adds another degree of
 *  variability to the key selection.
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
 *  TODO:  This rekeying is too predicatable.  There are several things bad
 *  about it: (1) the key has a historic component, which is bad; (2) initial
 *  keys have zeros in alot of places which makes it no stronger than if only
 *  32 bit keys were used.
 */
STATIC void
sctp_rekey(k)
	int k;
{
	uint32_t *seq;
	int n = (sctp_keys[k].last + 1) & 0xf;
	sctp_keys[k].last = n;
	seq = &sctp_keys[k].u.seq[n];
	*seq = secure_tcp_sequence_number(*(seq + 1), *(seq + 2), *(seq + 3), *(seq + 4));
}
STATIC int
sctp_get_key(sp)
	sctp_t *sp;
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
sctp_hmac(sp, text, tlen, key, klen, hmac)
	sctp_t *sp;
	uint8_t *text;
	int tlen;
	uint8_t *key;
	int klen;
	uint8_t *hmac;
{
	memset(hmac, 0xff, HMAC_SIZE);
	switch (sp->hmac) {
#ifdef SCTP_CONFIG_HMAC_SHA1
	case SCTP_HMAC_SHA_1:
		hmac_sha1(text, tlen, key, klen, hmac);
		break;
#endif
#ifdef SCTP_CONFIG_HMAC_MD5
	case SCTP_HMAC_MD5:
		hmac_md5(text, tlen, key, klen, hmac);
		break;
#endif
	default:
	case SCTP_HMAC_NONE:
		break;
	}
}
static void
sctp_sign_cookie(sp, ck)
	sctp_t *sp;
	struct sctp_cookie *ck;
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
static int
sctp_verify_cookie(sp, ck)
	sctp_t *sp;
	struct sctp_cookie *ck;
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
 *
 *  CHOSE BEST
 *  -------------------------------------------------------------------------
 *  This function is called by sctp_update_routes to choose the best primary
 *  and secondary addresses.  We alway return a usable address if possible,
 *  even if the secondary is not different from the primary.  This is called
 *  only when a route fails or a destination goes inactive due to too many
 *  timeouts, so destinations that are timing out are not so large a factor.
 */
STATIC struct sctp_daddr *
sctp_choose_best(sp, not)
	sctp_t *sp;
	struct sctp_daddr *not;
{
	struct sctp_daddr *best, *sd;
	assert(sp);
	if ((best = sp->daddr))
		for (sd = best->next; sd; sd = sd->next) {
			/* choose usable over unusable */
			if (!best->dst_cache && sd->dst_cache) {
				best = sd;
				continue;
			}
			/* choose active routes */
			if (best->retransmits > best->max_retrans
			    && sd->retransmits <= sd->max_retrans) {
				best = sd;
				continue;
			}
			/* choose routes without timeouts or dups */
			if ((best->retransmits || best->dups)
			    && !(sd->retransmits || sd->dups)) {
				best = sd;
				continue;
			}
			/* choose usable alternate if possible */
			if (best == not && sd != not) {
				best = sd;
				continue;
			}
			/* choose routes with least excessive timeouts */
			if (best->retransmits + sd->max_retrans >
			    sd->retransmits + best->max_retrans) {
				best = sd;
				continue;
			}
			/* choose routes with the least duplicates */
			if (best->dups > sd->dups) {
				best = sd;
				continue;
			}
			/* choose lowest rto */
			if (best->rto > sd->rto) {
				best = sd;
				continue;
			}
			/* choose not to slow start */
			if (best->cwnd <= best->ssthresh && sd->cwnd > sd->ssthresh) {
				best = sd;
				continue;
			}
			/* choose largest available window */
			if (best->cwnd + best->mtu - 1 - best->in_flight <
			    sd->cwnd + sd->mtu - 1 - sd->in_flight) {
				best = sd;
				continue;
			}
		}
	return (best);
}

/* 
 *  UPDATE ROUTES
 *  -------------------------------------------------------------------------
 *  This function is called whenever there is a routing problem or whenever a
 *  timeout occurs on a destination.  It's purpose is to perform a complete
 *  re-analysis of the available destination addresses and IP routing and
 *  establish new routes as required and set the (primary) and (secondary)
 *  destination addresses.
 */
#ifndef sysctl_ip_dynaddr
#define sysctl_ip_dynaddr 0
#else
extern int sysctl_ip_dynaddr;
#endif
#ifndef ip_rt_min_pmtu
#define ip_rt_min_pmtu 552
#else
extern int ip_rt_min_pmtu;
#endif
/* 
 *  sysctl_ip_dynaddr: this symbol is normally not exported, but we exported
 *  for the Linux Native version of SCTP, so we may have to treat it as extern
 *  here...
 */
STATIC int
sctp_update_routes(sp, force_reselect)
	sctp_t *sp;
	int force_reselect;
{
	int err = -EHOSTUNREACH;
	int mtu_changed = 0;
	int viable_route = 0;
	int route_changed = 0;
	struct sctp_daddr *sd;
	struct sctp_daddr *taddr, *raddr;
	int old_pmtu;
	assert(sp);
	ensure(sp->daddr, return (-EFAULT));
	old_pmtu = xchg(&sp->pmtu, INT_MAX);	/* big enough? */
	sp->route_caps = -1L;
	taddr = sp->taddr;
	raddr = sp->raddr;
	/* First we check our cached routes..  */
	for (sd = sp->daddr; sd; sd = sd->next) {
		struct rtable *rt = (struct rtable *) sd->dst_cache;
#ifdef SCTP_CONFIG_ADD_IP
		if (sd->flags & SCTP_DESTF_UNUSABLE)
			continue;
#endif
		printd(("INFO: Checking stream %p route for %d.%d.%d.%d\n", sp,
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
			(sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff));
		if (rt && (rt->u.dst.obsolete || rt->u.dst.ops->check(&rt->u.dst, 0))) {
			rare();
			__printd(("INFO: Obsolete route for %d.%d.%d.%d\n",
				  (sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
				  (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff));
			sd->dst_cache = NULL;
			ip_rt_put(rt);
			sd->saddr = 0;
			route_changed = 1;
		}
		if (!rt) {
			if (sd->dif)
				sd->saddr = 0;
			if ((err =
			     ip_route_connect(&rt, sd->daddr, sd->saddr,
					      RT_TOS(sp->ip_tos) | RTO_CONN | sp->ip_dontroute,
					      sd->dif)) < 0) {
				rare();
				continue;
			}
			if (rt->rt_flags & (RTCF_MULTICAST | RTCF_BROADCAST) && !sp->ip_broadcast) {
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
				if (sp->p_caps & SCTP_CAPS_ADD_IP &&
				    !(sp->userlocks & SCTP_BINDADDR_LOCK)) {
					int err = 0;
					struct sctp_saddr *ss;
					if ((ss = __sctp_saddr_alloc(sp, rt->rt_src, &err))) {
						ss->flags |= SCTP_SRCEF_ADD_REQUEST;
						sp->sackf |= SCTP_SACKF_ASC;
					}
				}
#endif
				if (!sp->saddr) {
					rare();
					ip_rt_put(rt);
					assure(sp->saddr);
					err = -EADDRNOTAVAIL;
					continue;
				}
				sd->saddr = sp->saddr->saddr;
			}
			/* always revert to initial settings when rerouting */
			sd->rto = sp->rto_ini;
			sd->rttvar = 0;
			sd->srtt = 0;
			sd->mtu = rt->u.dst.pmtu;
			sd->ssthresh = 2 * rt->u.dst.pmtu;
			sd->cwnd = rt->u.dst.pmtu;
			/* SCTP IG 2.9 */
			sd->partial_ack = 0;
			sd->dst_cache = &rt->u.dst;
			sd->route_caps = rt->u.dst.dev->features;
			route_changed = 1;
		}
		/* You're welcome diald! */
		if (sysctl_ip_dynaddr && sp->s_state == SCTP_COOKIE_WAIT && sd == sp->daddr) {
			/* see if route changed on primary as result of INIT that was discarded */
			struct rtable *rt2 = NULL;
			if (!ip_route_connect
			    (&rt2, rt->rt_dst, 0, RT_TOS(sp->ip_tos) | sp->ip_dontroute, sd->dif)) {
				if (rt2->rt_src != rt->rt_src) {
					rare();
					rt2 = xchg(&rt, rt2);
					sd->rto = sp->rto_ini;
					sd->rttvar = 0;
					sd->srtt = 0;
					sd->mtu = rt->u.dst.pmtu;
					sd->ssthresh = 2 * rt->u.dst.pmtu;
					sd->cwnd = rt->u.dst.pmtu;
					/* SCTP IG 2.9 */
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
		sp->route_caps &= rt->u.dst.dev->features;
		if (sd->mtu != rt->u.dst.pmtu) {
			sd->mtu = rt->u.dst.pmtu;
			mtu_changed = 1;
			rare();
		}
		if (sp->pmtu > sd->mtu)
			sp->pmtu = sd->mtu;
	}
	if (!viable_route) {
		rare();
		/* set defaults */
		sp->taddr = sp->daddr;
		sp->raddr = sp->daddr->next ? sp->daddr->next : sp->daddr;
		sp->pmtu = ip_rt_min_pmtu;
		return (err);
	}
	/* if we have made or need changes then we want to reanalyze routes */
	if (force_reselect || route_changed || mtu_changed ||
	    sp->pmtu != old_pmtu || !sp->taddr || !sp->raddr) {
#if defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR
		int bad_choice = 0;
#endif				/* defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR 
				 */
		sp->taddr = sctp_choose_best(sp, NULL);
		usual(sp->taddr);
#if defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR
		if ((sp->options & SCTP_OPTION_BREAK)
		    && (sp->taddr == sp->daddr || sp->taddr == sp->daddr->next)
		    && sp->taddr->packets > BREAK_GENERATOR_LEVEL) {
			ptrace(("Primary sp->taddr %03d.%03d.%03d.%03d chosen poorly on %p\n",
				(sp->taddr->daddr >> 0) & 0xff, (sp->taddr->daddr >> 8) & 0xff,
				(sp->taddr->daddr >> 16) & 0xff, (sp->taddr->daddr >> 24) & 0xff,
				sp));
			bad_choice = 1;
		}
		if (sp->taddr && sp->taddr != taddr)
			ptrace(("sp = %p, New primary route: %d.%d.%d.%d\n", sp,
				(sp->taddr->daddr >> 0) & 0xff, (sp->taddr->daddr >> 8) & 0xff,
				(sp->taddr->daddr >> 16) & 0xff, (sp->taddr->daddr >> 24) & 0xff));
#endif				/* defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR 
				 */
		sp->raddr = sctp_choose_best(sp, sp->taddr);
		usual(sp->raddr);
#if defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR
		if ((sp->options & SCTP_OPTION_BREAK)
		    && (sp->raddr == sp->daddr || sp->raddr == sp->daddr->next)
		    && sp->raddr->packets > BREAK_GENERATOR_LEVEL) {
			ptrace(("Secondary sp->raddr %03d.%03d.%03d.%03d chosen poorly on %x\n",
				(sp->raddr->daddr >> 0) & 0xff, (sp->raddr->daddr >> 8) & 0xff,
				(sp->raddr->daddr >> 16) & 0xff, (sp->raddr->daddr >> 24) & 0xff,
				(uint) sp));
			bad_choice = 1;
		}
		if (bad_choice) {
			for (sd = sp->daddr; sd; sd = sd->next) {
				printk(KERN_INFO
				       "%03d.%03d.%03d.%03d: %x, %u:%u, %u, %lu, %u:%u, %u, %u\n",
				       (sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
				       (sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff,
				       (uint) sd->dst_cache, sd->retransmits, sd->max_retrans,
				       sd->dups, sd->rto, sd->cwnd, sd->ssthresh, sd->mtu,
				       sd->in_flight);
			}
		}
		if (sp->raddr && sp->raddr != raddr)
			ptrace(("sp = %p, New secondary route: %d.%d.%d.%d\n", sp,
				(sp->raddr->daddr >> 0) & 0xff, (sp->raddr->daddr >> 8) & 0xff,
				(sp->raddr->daddr >> 16) & 0xff, (sp->raddr->daddr >> 24) & 0xff));
#endif				/* defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR 
				 */
	}
	return (0);
}

/* 
 *  =========================================================================
 *
 *  MESSAGE OUTPUT
 *
 *  =========================================================================
 */
/* 
 *  QUEUE XMIT (Queue for transmission)
 *  -------------------------------------------------------------------------
 *  We need this broken out so that we can use the netfilter hooks.
 */
static inline int
sctp_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;
	if (skb->len > rt->u.dst.pmtu) {
		rare();
		return ip_fragment(skb, skb->dst->output);
	} else {
		iph->frag_off |= __constant_htons(IP_DF);
		ip_send_check(iph);
		return skb->dst->output(skb);
	}
}

/* 
 *  XMIT OOTB (Disconnect Send with no Listening STREAM).
 *  -------------------------------------------------------------------------
 *  This sends disconnected without a STREAM.  All that is needed is a
 *  destination address and a message block.  The only time that we use this
 *  is for responding to OOTB packets with ABORT or SHUTDOWN COMPLETE.
 */
static void
sctp_xmit_ootb(daddr, saddr, mp)
	uint32_t daddr;
	uint32_t saddr;
	mblk_t *mp;
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
		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			struct sctphdr *sh;
			unsigned char *data;
			skb_reserve(skb, hlen);
			iph = (struct iphdr *) __skb_put(skb, tlen);
			sh = (struct sctphdr *) (iph + 1);
			data = (unsigned char *) (sh);
			skb->dst = &rt->u.dst;
			skb->priority = 0;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = 0;
			iph->frag_off = 0;
			iph->ttl = 0;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = 132;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);
			for (bp = mp; bp; bp = bp->b_cont) {
				int blen = bp->b_wptr - bp->b_rptr;
				if (blen > 0) {
					bcopy(bp->b_rptr, data, blen);
					data += blen;
				} else
					rare();
			}
			sh->check = 0;
			if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				sh->check = htonl(cksum_generate(sh, plen));
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
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
 *  This sends disconnected sends.  All that is needed is a destination
 *  address and a message block.  The only time that we use this is for
 *  sending INIT ACKs (because we have not built a complete stream yet,
 *  however we do at least have a Listening STREAM or possibly an established
 *  STREAM that is partially qualitfied and can provide some TOS and other
 *  information for the packet.
 */
static void
sctp_xmit_msg(daddr, mp, sp)
	uint32_t daddr;
	mblk_t *mp;
	sctp_t *sp;
{
	struct rtable *rt = NULL;
	ensure(mp, return);
	/* SCTP IG 2.27 */
	/* Error chunks bundled with INIT-ACK */
	if (!ip_route_output(&rt, daddr, 0, RT_TOS(sp->ip_tos) | sp->ip_dontroute, 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt->u.dst.dev;
		size_t plen = SCTP_TCB(mp)->dlen;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t tlen = plen + sizeof(struct iphdr);
		size_t alen = 0;
		usual(hlen);
		usual(plen);
		if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
			mblk_t *bp;
			struct iphdr *iph;
			struct sctphdr *sh;
			unsigned char *data;
			skb_reserve(skb, hlen);
			iph = (struct iphdr *) __skb_put(skb, tlen);
			sh = (struct sctphdr *) (iph + 1);
			data = (unsigned char *) (sh);
			skb->dst = &rt->u.dst;
			skb->priority = sp->ip_priority;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = sp->ip_tos;
			iph->frag_off = 0;
			iph->ttl = sp->ip_ttl;
			iph->daddr = rt->rt_dst;
			iph->saddr = rt->rt_src;
			iph->protocol = sp->ip_proto;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);
			/* SCTP IG 2.27 */
			/* Error chunks bundled with INIT-ACK */
			for (bp = mp; bp; bp = bp->b_next) {
				mblk_t *db;
				size_t clen = 0;
				size_t pad;
				for (db = bp; db; db = db->b_cont) {
					int blen = db->b_wptr - db->b_rptr;
					if (blen > 0) {
						bcopy(db->b_rptr, data, blen);
						data += blen;
						clen += blen;
					} else
						rare();
				}
				/* pad each chunk if not padded already */
				pad = PADC(clen) - clen;
				bzero(data, pad);
				data += pad;
				alen += clen + pad;
			}
			if (alen == plen) {
				sh->check = 0;
				if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
					sh->check = htonl(cksum(sp, sh, plen));
				NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
			} else {
				usual(alen == plen);
				kfree_skb(skb);
			}
		} else
			rare();
	} else
		rare();
	/* sending INIT ACKs are one time events, if we can't get the response off, we just drop
	   the INIT ACK: the peer should send us another INIT * in a short while... */
	freechunks(mp);
	return;
}

/* 
 *  SEND MSG  (Connected Send)
 *  -------------------------------------------------------------------------
 *  This sends connected sends.  It requires a STREAM a desination address
 *  structure and a message block.  This function does not free the message
 *  block.  The caller is responsible for the message block.
 */
#if defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR
STATIC int break_packets = 0;
STATIC int seed = 152;
STATIC unsigned char
random(void)
{
	return (unsigned char) (seed = seed * 60691 + 1);
}
#endif				/* defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR 
				 */
static void
sctp_send_msg(sp, sd, mp)
	sctp_t *sp;
	struct sctp_daddr *sd;
	mblk_t *mp;
{
	int tried_update;
	ensure(sp, return);
	if (!mp) {
		swerr();
		return;
	}
	for (tried_update = 0; !tried_update; tried_update++) {
		if (sd && sd->dst_cache
		    && (!sd->dst_cache->obsolete || sd->dst_cache->ops->check(sd->dst_cache, 0))) {
			struct sk_buff *skb;
			struct net_device *dev = sd->dst_cache->dev;
			size_t plen = SCTP_TCB(mp)->dlen;
			size_t hlen = (dev->hard_header_len + 15) & ~15;
			size_t tlen = sizeof(struct iphdr) + plen;
#ifdef ERROR_GENERATOR
			if ((sp->options & SCTP_OPTION_DBREAK)
			    && sd->daddr == 0x010000ff && ++break_packets > BREAK_GENERATOR_LEVEL) {
				if (break_packets > BREAK_GENERATOR_LIMIT)
					break_packets = 0;
				return;
			}
			if ((sp->options & SCTP_OPTION_BREAK)
			    && (sd == sp->daddr || sd == sp->daddr->next)
			    && ++sd->packets > BREAK_GENERATOR_LEVEL) {
				return;
			}
			if ((sp->options & SCTP_OPTION_DROPPING)
			    && ++sd->packets > ERROR_GENERATOR_LEVEL) {
				if (sd->packets > ERROR_GENERATOR_LIMIT)
					sd->packets = 0;
				return;
			}
			if ((sp->options & SCTP_OPTION_RANDOM)
			    && ++sd->packets > 2 * ERROR_GENERATOR_LEVEL) {
				if (!(random() & 0x03))
					return;
			}
#endif
			unusual(plen == 0 || plen > 1 << 14);
			/* IMPLEMENTATION NOTE:- We could dup these mblks and put them into an
			   sk_buff fraglist, however, this would result allocating the sk_buff
			   header on each data chunk (just to get the new next pointer).
			   Unfortunately, for the most part in SCTP, data chunks are very small:
			   even smaller that the sk_buff header so it is probably not worth duping
			   versus copying into a single sk_buff. */
			if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
				mblk_t *bp;
				struct iphdr *iph;
				struct sctphdr *sh;
				unsigned char *head, *data;
				size_t alen = 0;
				skb_reserve(skb, hlen);
				iph = (struct iphdr *) __skb_put(skb, tlen);
				sh = (struct sctphdr *) (iph + 1);
				data = (unsigned char *) (sh);
				head = data;
				skb->dst = dst_clone(sd->dst_cache);
				skb->priority = sp->ip_priority;
				iph->version = 4;
				iph->ihl = 5;
				iph->tos = sp->ip_tos;
				iph->frag_off = 0;
				iph->ttl = sp->ip_ttl;
				iph->daddr = sd->daddr;
				iph->saddr = sd->saddr;
				iph->protocol = sp->ip_proto;
				iph->tot_len = htons(tlen);
				skb->nh.iph = iph;
				__ip_select_ident(iph, sd->dst_cache);
				for (bp = mp; bp; bp = bp->b_next) {
					mblk_t *db;
					size_t clen = 0;
					struct sctp_tcb *cb = SCTP_TCB(bp);
					cb->daddr = sd;
					cb->when = jiffies;
					cb->flags |= SCTPCB_FLAG_SENT;
					cb->trans++;
					for (db = bp; db; db = db->b_cont) {
						int blen = db->b_wptr - db->b_rptr;
						normal(db->b_datap->db_type == M_DATA);
						if (db->b_datap->db_type == M_DATA) {
							normal(blen > 0);
							if (blen > 0) {
								ensure(head + plen >= data + blen,
								       kfree_skb(skb);
								       return);
								bcopy(db->b_rptr, data, blen);
								data += blen;
								clen += blen;
							}
						}
					}
					{
						/* pad each chunk if not padded already */
						size_t pad = PADC(clen) - clen;
						ensure(head + plen >= data + pad, kfree_skb(skb);
						       return);
						bzero(data, pad);
						data += pad;
						alen += clen + pad;
					}
				}
				if (alen != plen) {
					usual(alen == plen);
					ptrace(("alen = %u, plen = %u discarding\n", alen, plen));
					kfree_skb(skb);
					return;
				}
				/* IMPLEMENTATION NOTE:= Although the functional support is
				   included for partial checksum and copy from user, we don't use
				   it.  The only time that partial checksum is going to save us in
				   when chunks are bundled repeatedly (for retransmission).  But
				   then we are network bound rather than processor bound, so there
				   is probably little use there either.  It might be better,
				   however, to combine checksumming below with copying above for
				   more efficiency. */
				sh->check = 0;
				if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
					sh->check = htonl(cksum(sp, sh, plen));
				NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
				/* Whenever we transmit something, we expect a reply to our v_tag,
				   so we put ourselves in the 1st level vtag caches expecting a
				   quick reply. */
				if (!((1 << sp->s_state) & (SCTPF_CLOSED | SCTPF_LISTEN)))
					sctp_cache[sp->hashent] = sp;
				break;
			} else {
				rare();
			}
		} else {
			usual(sd && sd->dst_cache
			      && (!sd->dst_cache->obsolete
				  || sd->dst_cache->ops->check(sd->dst_cache, 0)));
			if (sctp_update_routes(sp, 1))
				break;
		}
	}
}

static inline void
set_timeout(sctp_t * sp, int *tidp, timo_fcn_t *fnc, void *data, long ticks)
{
	psw_t flags;
	assert(tidp);
	assert(data);
	lis_spin_lock_irqsave(&sp->lock, &flags);
	{
		if (*tidp) {
			abnormal(*tidp);
			untimeout(xchg(tidp, 0));
		}
		*tidp = timeout(fnc, data, ticks ? ticks : 1);
	}
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
}
static inline void
mod_timeout(sctp_t * sp, int *tidp, timo_fcn_t *fnc, void *data, long ticks)
{
	psw_t flags;
	assert(tidp);
	assert(data);
	lis_spin_lock_irqsave(&sp->lock, &flags);
	{
		if (*tidp) {
			untimeout(xchg(tidp, 0));
		}
		*tidp = timeout(fnc, data, ticks ? ticks : 1);
	}
	lis_spin_unlock_irqrestore(&sp->lock, &flags);
}

/* 
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */

struct sctp_bundle_cookie {
	mblk_t **dpp;			/* place to link buffer */
	size_t dmps;			/* destination max packet size */
	size_t amps;			/* association max packet size */
	size_t mrem;			/* reminaing payload in packet */
	size_t mlen;			/* current length of message */
	size_t swnd;			/* remaining send window */
	size_t pbuf;			/* peer buffer */
};

/* 
 *  CONGESTION/RECEIVE WINDOW AVAILABILITY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Calculate of the remaining space in the current packet, how much is
 *  available for use by data according to the current peer receive window,
 *  the current destination congestion window, and the current outstanding
 *  data bytes in flight.
 *
 *  This is called iteratively as each data chunk is tested for bundling into
 *  the current message.  The usable length returned does not include the data
 *  chunk header.
 */
STATIC INLINE size_t
sctp_avail(sp, sd, dmps, mrem)
	sctp_t *sp;
	struct sctp_daddr *sd;
	size_t dmps;
	size_t mrem;
{
	size_t ulen = mrem > sizeof(struct sctp_data) ? mrem - sizeof(struct sctp_data) : 0;
	if (ulen) {
		size_t cwnd, rwnd, swnd, awnd;
		cwnd = sd->cwnd + dmps;
		cwnd = cwnd > sd->in_flight ? cwnd - sd->in_flight : 0;
		rwnd = sp->p_rwnd;
		rwnd = rwnd > sp->in_flight ? rwnd - sp->in_flight : 0;
		swnd = cwnd < rwnd ? cwnd : rwnd;
		awnd = sp->in_flight ? swnd : cwnd;
		ulen = awnd < ulen ? awnd : ulen;
	}
	return (ulen);
}

/* 
 *  BUNDLING FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  BUNDLE SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_sack(sp, sd, ckp)
	sctp_t *sp;			/* association */
	struct sctp_daddr *sd;		/* destination */
	struct sctp_bundle_cookie *ckp;	/* lumped arguments */
{
	mblk_t *mp;
	struct sctp_tcb *cb;
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
#endif
#ifdef SCTP_CONFIG_ECN
	plen += PADC(elen);
#endif
	if ((1 << sp->s_state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen > ckp->dmps) {
		size_t too_many_dups;
		rare();		/* trim down sack */
		too_many_dups = (clen - ckp->mrem + 3) / sizeof(uint32_t);
		ndups = ndups > too_many_dups ? ndups - too_many_dups : 0;
		clen = sizeof(*m) + glen + ndups * sizeof(uint32_t);
		if (ckp->mrem < clen) {
			size_t too_many_gaps;
			rare();	/* trim some more */
			too_many_gaps = (clen - ckp->mrem + 3) / sizeof(uint32_t);
			ngaps = ngaps > too_many_gaps ? ngaps - too_many_gaps : 0;
			clen = sizeof(*m) + ngaps * sizeof(uint32_t);
		}
	}
	if (plen > ckp->mrem && plen <= ckp->dmps)
		goto wait_for_next_packet;
	if (!(mp = sctp_alloc_chk(sp, clen)))
		goto enobufs;
	{
		struct sctp_tcb *gap = sp->gaps;
		struct sctp_tcb *dup = sp->dups;
		size_t arwnd = sp->a_rwnd;
		size_t count = bufq_size(&sp->oooq) + bufq_size(&sp->dupq) + bufq_size(&sp->rcvq);
		arwnd = count < arwnd ? arwnd - count : 0;
		// __ptrace(("oooq = %u:%u, dupq = %u:%u, rcvq = %u:%u\n",
		// bufq_size(&sp->oooq),bufq_length(&sp->oooq),
		// bufq_size(&sp->dupq),bufq_length(&sp->dupq),
		// bufq_size(&sp->rcvq),bufq_length(&sp->rcvq)));
		// if ( !canputnext(sp->rq) ) arwnd = 0;
		cb = (struct sctp_tcb *) mp->b_datap->db_base;
		cb->mp = mp;
		cb->dlen = clen;	/* XXX */
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
			*((uint16_t *) mp->b_wptr)++ = htons(gap->tsn - sp->r_ack);
			gap = gap->tail;
			*((uint16_t *) mp->b_wptr)++ = htons(gap->tsn - sp->r_ack);
		}
		for (; dup && ndups; dup = dup->next, ndups--)
			*((uint32_t *) mp->b_wptr)++ = htonl(dup->tsn);
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
		}
#endif				/* SCTP_CONFIG_ECN */
		sp->sackf &= ~SCTP_SACKF_ANY;
		if (sp->timer_sack)
			untimeout(xchg(&sp->timer_sack, 0));
		ckp->mrem = ckp->mrem > plen ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = mp;
		ckp->dpp = &(mp->b_next);
		mp->b_next = NULL;
		return (0);
	}
      enobufs:
	rare();
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
      outstate:
	swerr();
	return (0);
}

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY

/* 
 *  BUNDLE FORWARD SEQUENCE NUMBER
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_fsn(sp, sd, ckp)
	struct sctp *sp;		/* association */
	struct sctp_daddr *sd;		/* destination */
	struct sctp_bundle_cookie *ckp;	/* lumped arguments */
{
	mblk_t *mp, *db;
	struct sctp_forward_tsn *m;
	size_t clen = sizeof(*m);
	size_t plen;
	size_t nstrs = 0;
	size_t mstrs = (ckp->dmps - sizeof(*m)) / sizeof(uint32_t);
	uint32_t l_fsn = sp->t_ack;
	struct sctp_tcb *cb;
	if ((1 << sp->s_state) & ~(SCTPF_SENDING))
		goto outstate;
	if (!after(sp->l_fsn, sp->t_ack))
		goto outstate;
	for (db = bufq_head(&sp->rtxq); db; db = db->b_next) {
		struct sctp_tcb *tcb = SCTP_TCB(db);
		if (!after(tcb->tsn, sp->t_ack))
			continue;
		if (after(tcb->tsn, sp->l_fsn))
			break;
		l_fsn = tcb->tsn;
		if (tcb->st && !(tcb->flags & SCTPCB_FLAG_URG)
		    && !(tcb->st->n.more & SCTP_STRMF_DROP)) {
			tcb->st->n.more |= SCTP_STRMF_DROP;
			nstrs++;
			if (nstrs >= mstrs)
				break;
		}
	}
	clen += nstrs * sizeof(uint32_t);
	plen = PADC(clen);
	if (plen >= ckp->mrem && plen <= ckp->dmps)
		goto wait_for_next_packet;
	if (!(mp = sctp_alloc_chk(sp, clen)))
		goto enobufs;
	cb = (struct sctp_tcb *) mp->b_datab->db_base;
	cb->mp = mp;
	cb->dlen = clen;
	bzero(mp->b_wptr + clen, plen - clen);
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_FORWARD_TSN;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->f_tsn = htonl(l_fsn);
	mp->b_wptr += sizeof(*m);
	/* run backwards to build stream number list */
	for (db = bufq_tail(&sp->rtxq); db; db = db->b_prev) {
		struct sctp_tcb tcb = SCTP_TCB(db);
		if (after(tcb->tsn, l_fsn))
			continue;
		if (!after(tcb->tsn, sp->t_ack))
			break;
		if (tcb->st && !(tcb->flags & SCTPCB_FLAG_URG) &&
		    (tcb - st->n.more & SCTP_STRMF_DROP)) {
			*((uint16_t *) mp->b_wptr)++ = tcb->st->sid;
			*((uint16_t *) mp->b_wptr)++ = tcb->ssn;
			tcb->st->n.more &= ~SCTP_STRMF_DROP;
			assure(nstr);
			nstr--;
		}
	}
	assure(!nstr);
	sp->sackf &= ~SCTP_SACKF_FSN;
	if (!(sp->sackf & SCTP_SACKF_ANY))
		if (sp->timer_sack)
			untimeout(xchg(&sp->timer_sack, 0));
	sd->flags |= SCTP_DESTF_FORWDTSN;
	ckp->mrem = ckp->mrem > plen ? ckp->mrem - plen : 0;
	ckp->mlen += plen;
	*ckp->dpp = mp;
	ckp->dpp = &(mp->b_next);
	mp->b_next = NULL;
	return (0);
}

#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ECN

/* 
 *  BUNDLE CONGESTION WINDOW REDUCTION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_cwr(sp, sd, ckp)
	struct sctp *sp;		/* association */
	struct sctp_daddr *sd;		/* destination */
	struct sctp_bundle_cookie *ckp;	/* lumped arguments */
{
	mblk_t *mp;
	struct sctp_cwr *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	if ((1 << sp->s_state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen >= ckp->mrem && plen <= ckp->dmps)
		goto wait_for_next_packet;
	if (!(mp = sctp_alloc_chk(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_TYPE_CWR;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(sizeof(*m));
	m->l_tsn = htonl(sp->p_lsn);
	mp->b_wptr += plen;
	sp->sackf &= ~SCTP_SACKF_CWR;
	ckp->mrem = ckp->mrem > plen ? ckp->mrem - plen : 0;
	ckp->mlen += plen;
	*ckp->dpp = mp;
	ckp->dpp = &(mp->b_next);
	mp->b_next = NULL;
	return (0);
      enobufs:
	rare();
	return (0);
      wait_for_next_packet:
	rare();
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
sctp_bundle_error(sp, sd, dmps, amps, dpp, mrem, mlen)
	sctp_t *sp;			/* association */
	struct sctp_daddr *sd;		/* destination */
	struct sctp_bundle_cookie *ckp;	/* lumped arguments */
{
	mblk_t *mp;
	while (ckp->mrem && (mp = bufq_head(&sp->errq))) {
		size_t clen = mp->b_wptr - mp->b_rptr;
		size_t plen = PADC(clen);
		if (plen > ckp->mrem && plen <= ckp->dmps)
			goto wait_for_next_packet;
		ckp->mrem = ckp->mrem > plen ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		mp = bufq_dequeue(&sp->errq);
		*ckp->dpp = mp;
		ckp->dpp = &(mp->b_next);
		mp->b_next = NULL;
	}
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
}

/* 
 *  BUNDLE DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
trimhead(mblk_t *mp, int len)
{
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
 *  Try to fragment a DATA chunk which has not been transmitted yet into
 *  two chunks, the first small enough to fit into the pmtu and the second one
 *  containing the remainder of the data in a chunk.  This is called
 *  iteratively, so the reminaing data may also be further fragmented
 *  according to the pmtu experienced at the time that it is further
 *  fragmented.
 */
#if defined(_DEBUG)||defined(_SAFE)
STATIC void
sctp_frag_chunk(bq, mp, mps)
	bufq_t *bq;
	mblk_t *mp;
	size_t mps;
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
		/* copyb does not copy the hiddle control block */
		bcopy(mp->b_datap->db_base, dp->b_datap->db_base,
		      mp->b_datap->db_lim - mp->b_datap->db_base);
		/* duplicate the message blocks which form the data */
		if (!(bp = dupmsg(mp->b_cont))) {
			rare();
			freeb(dp);
			return;
		}
		dp->b_cont = bp;
	}
	{
		struct sctp_tcb *cb1 = SCTP_TCB(mp);
		struct sctp_tcb *cb2 = SCTP_TCB(dp);
		struct sctp_data *m1 = (struct sctp_data *) mp->b_rptr;
		struct sctp_data *m2 = (struct sctp_data *) dp->b_rptr;
		size_t dlen = (mps - sizeof(struct sctp_data));
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
			ensure(ret, freemsg(dp);
			       return;
			    );
			ret = trimtail(dp, dlen);	/* trim fragment */
			ensure(ret, freemsg(dp);
			       return;
			    );
		}
#else
		fixme(("Should consider multiple mblks\n"));
		mp->b_cont->b_wptr = mp->b_cont->b_rptr + dlen;	/* trim original */
		dp->b_cont->b_rptr = dp->b_cont->b_rptr + dlen;	/* trim fragment */
#endif
	}
	/* insert the fresh copy after the existing copy in the bufq */
	__ctrace(bufq_append(bq, mp, dp));
}
#endif

/* 
 *  BUNDLE DATA for RETRANSMISSION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_data_retrans(sp, sd, dmps, amps, dpp, mrem, mlen)
	sctp_t *sp;			/* association */
	struct sctp_daddr *sd;		/* destination */
	struct sctp_bundle_cookie *ckp;	/* lumped arguments */
{
	mblk_t *mp = bufq_head(&sp->rtxq);
	for (; mp && ckp->mrem && ckp->swnd; mp = mp->b_next) {
		if ((SCTP_TCB(mp)->flags & SCTPCB_FLAG_RETRANS)) {
			mblk_t *db;
			struct sctp_tcb *cb = SCTP_TCB(mp);
			size_t dlen = cb->dlen;
			size_t plen = PADC(sizeof(struct sctp_data) + dlen);
			if (plen > ckp->mrem && plen <= ckp->dmps)
				goto wait_for_next_packet;
			if (dlen > ckp->swnd && cb->sacks != 4)
				goto congested;
			if (!(db = dupmsg(mp)))
				goto enobufs;
			ensure(sp->nrtxs > 0, sp->ntrxs = 1);
			sp->nrtxs--;
			cb->flags &= ~SCTPCB_FLAG_RETRANS;
			cb->when = jiffies;
			cb->daddr = sd;
			// cb->trans += 1; done by sctp_send_msg */
			cb->sacks = 0;
			ckp->swnd -= dlen;
			sd->in_flight += dlen;
			sp->in_flight += dlen;
			ckp->mrem = ckp->mrem > plen ? ckp->mrem - plen : 0;
			ckp->mlen += plen;
			*ckp->dpp = db;
			ckp->dpp = &(db->b_next);
			db->b_next = NULL;
		}
	}
	return (0);
      enobufs:
	rare();
	return (1);
      congested:
	seldom();
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
sctp_bundle_data_urgent(sp, sd, ckp)
	sctp_t *sp;			/* association */
	struct sctp_daddr *sd;		/* destination */
	struct sctp_bundle_cookie *ckp;	/* lumped arguments */
{
	mblk_t *mp;
	while (ckp->mrem && ckp->swnd && (mp = bufq_head(&sp->urgq))) {
		mblk_t *db;
		struct sctp_data *m = (typeof(m)) mp->b_rptr;
		struct sctp_tcb *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(*m) + dlen);
		ensure(cb->st, continue);
#if defined(_DEBUG)||defined(_SAFE)
		/* this should only occur if the pmtu is falling */
		if (ckp->amps <= ckp->mrem && plen > ckp->amps) {
			rare();
			sctp_frag_chunk(&sp->urgq, mp, ckp->amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
		if (plen > ckp->mrem && plen <= ckp->dmps)
			goto wait_for_next_packet;
		if (dlen > ckp->swnd && sd->in_flight)
			goto congested;
		if ((mp == cb->st->x.head)) {
			rare();
			cb->st->x.head = NULL;	/* steal partial */
		}
		if (!(db = dupmsg(mp)))
			goto enobufs;
		cb->tsn = sp->t_tsn++;
		cb->flags |= SCTPCB_FLAG_SENT;
		cb->when = jiffies;
		cb->daddr = sd;
		// cb->trans = 1; /* done by sctp_send_msg */
		cb->sacks = 0;
		m->tsn = htonl(cb->tsn);
		ckp->swnd -= dlen;
		sd->in_flight += dlen;
		sp->in_flight += dlen;
		ckp->mrem = ckp->mrem > plen ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = db;
		ckp->dpp = &(db->b_next);
		db->b_next = NULL;
		bufq_queue(&sp->rtxq, bufq_dequeue(&sp->urgq));
	}
	return;
      enobufs:
	rare();
	return (1);
      congested:
	seldom();
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
sctp_bundle_data_normal(sp, sd, ckp)
	sctp_t *sp;			/* association */
	struct sctp_daddr *sd;		/* destination */
	struct sctp_bundle_cookie *ckp;	/* lumped arguments */
{
	mblk_t *mp;
	while (ckp->mrem && ckp->swnd && (mp = bufq_head(&sp->sndq))) {
		mblk_t *db;
		struct sctp_data *m = (typeof(m)) mp->b_rptr;
		struct sctp_tcb *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(*m) + dlen);
		ensure(cb->st, continue);
#if defined(_DEBUG)||defined(_SAFE)
		/* this should only occur if the pmtu is falling */
		if (ckp->amps <= ckp->mrem && plen > ckp->amps) {
			rare();
			sctp_frag_chunk(&sp->sndq, mp, ckp->amps);
			dlen = cb->dlen;
			plen = PADC(sizeof(struct sctp_data) + dlen);
		}
#endif
		if (plen > ckp->mrem && plen < -ckp->dmps)
			goto wait_for_next_packet;
		if (dlen > ckp->swnd && sd->in_flight)
			goto congested;
		if ((mp == cb->st->n.head)) {
			seldom();
			cb->st->n.head = NULL;	/* stead partial */
		}
		if (!(db = dupmsg(mp)))
			goto enobufs;
		cb->tsn = sp->t_tsn++;
		cb->flags |= SCTPCB_FLAG_SENT;	/* this is data */
		// cb->trans = 1; /* done by sctp_send_msg */
		cb->sacks = 0;
		cb->when = jiffies;
		cb->daddr = sd;
		m->tsn = htonl(cb->tsn);
		ckp->swnd -= dlen;
		sd->in_flight += dlen;
		sp->in_flight += dlen;
		ckp->mrem = ckp->mrem > plen ? ckp->mrem - plen : 0;
		ckp->mlen += plen;
		*ckp->dpp = db;
		ckp->dpp = &(db->b_next);
		db->b_next = NULL;
		bufq_queue(&sp->rtxq, bufq_dequeue(&sp->sndq));
	}
	return (0);
      enobufs:
	rare();
	return (1);
      congested:
	seldom();
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
STATIC void sctp_retrans_timeout(caddr_t data);
STATIC void
sctp_bundle_more(sp, sd, mp, nonagle)
	sctp_t *sp;
	struct sctp_daddr *sd;
	mblk_t *mp;
	int nonagle;
{
	struct sctp_bundle_cookie cookie;
	static const size_t hlen = sizeof(struct iphdr);
	static const size_t htax = hlen + sizeof(struct sctphdr);
	int rtn = 0;
	cookie.dpp = &(mp->b_next);
	cookie.dmps = sd->mtu - htax;	/* destintaion max payload size */
	cookie.amps = sp->pmtu - htax;	/* association max payload size */
	cookie.mlen = msgdsize(mp);
	cookie.mrem = (cookie.mlen + hlen) < sd->mtu ? sd->mtu - (cookie.mlen + hlen) : 0;
	cookie.swnd = sctp_avail(sp, sd, cookie.dmps, cookie.mrem);
	cookie.pbuf = sp->p_rbuf >> 1;
	if ((sp->sackf & SCTP_SACKF_NOW)
	    || (sp->ntrxs)
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
		if (cookie.mrem && bufq_head(&sp->urgq)) {
			res = sctp_bundle_urgent(sp, sd, &cookie);
			max |= res;
			rtn += res;
		}
		if (cookie.mrem && sp->nrtxs) {
			res = sctp_bundle_retrans(sp, sd, &cookie);
			max |= res;
		}
		if (cookie.mrem && bufq_head(&sp->sndq)) {
			res = sctp_bundle_normal(sp, sd, &cookie);
			max |= res;
			rtn += res;
		}
		SCTP_TCB(mp)->dlen = cookie.mlen;
		if ((sd->in_flight || (sd->flags & SCTP_DESTF_FORWDTSN)) && !sd->timer_retrans) {
			set_timeout(sp, &sd->timer_retrans, &sctp_retrans_timeout, sd, sd->rto);
		}
		if (!max && bufq_head(&sp->sndq) && after(sp->t_tsn, t_tsn))
			sp->m_tsn = t_tsn;	/* sent a short packet */
		else if (!was_in_flight)
			sp->m_tsn = sp->t_ack;
	}
	return (rtn);
}

/* 
 *  BUNDLE ERRORS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Bundle errors to an INIT-ACK chunk per SCTP IG 2.27.
 */
STATIC int
sctp_bundle_errs(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_bundle_cookie cookie;
	static const size_t hlen = sizeof(struct iphdr);
	static const size_t htax = sizeof(struct sctphdr) + hlen;
	int rtn = 0;
	cookie.dpp = &(mp->b_next);
	cookie.dmps = sp->pmtu - htax;	/* destination max payload size */
	cookie.amps = sp->pmtu - htax;	/* association max payload size */
	cookie.mlen = msgdsize(mp);
	cookie.mrem = (cookie.mlen + hlen) < sp->pmtu ? sp->pmtu - (cookie.mlen + hlen) : 0;
	cookie.swnd = 0;
	cookie.pbuf = 0;
	*cookie.dpp = NULL;
	if (cookie.mrem & bufq_head(&sp->errq)) {
		sctp_bundle_error(sp, NULL, &cookie);
	}
	SCTP_TCB(mp)->dlen = mlen;
	return (rtn);
}

/* 
 *  ALLOC CHK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocate a mblk for a chunk without hard-header, IP header of SCTP header
 *  headroom.  This is used for allocating mblks which are used to contain a
 *  chunk for bundling.
 */
STATIC mblk_t *
sctp_alloc_chk(sp, clen)
	sctp_t *sp;
	size_t clen;
{
	mblk_t *mp;
	struct sctp_tcb *cb;
	size_t plen = PADC(clen);
	assert(sp);
	if ((mp = allocb(sizeof(*cb) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		cb = (struct sctp_tcb *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = plen;
		mp->b_rptr += sizeof(*cb);
		mp->b_wptr += sizeof(*cb);
		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/* 
 *  ALLOC MSG
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocates a new message block with a hidden transmission control block, an
 *  SCTP message header, and the first chunk of a message.
 */
STATIC mblk_t *
sctp_alloc_msg(sp, clen)
	sctp_t *sp;
	size_t clen;
{
	mblk_t *mp;
	struct sctphdr *sh;
	struct sctp_tcb *cb;
	size_t plen = PADC(clen);
	assert(sp);
	if ((mp = allocb(sizeof(*cb) + sizeof(*sh) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		cb = (struct sctp_tcb *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = sizeof(*sh) + plen;
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);
		sh = (struct sctphdr *) mp->b_wptr;
		sh->srce = sp->sport;
		sh->dest = sp->dport;
		sh->v_tag = sp->p_tag;
		sh->check = 0;
		mp->b_wptr += sizeof(*sh);
		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/* 
 *  ALLOC REPLY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for allocating a message block with a hidden transmission control
 *  block for a message sith an STCP header.  This is for out of the blue
 *  replies and all that is required is the SCTP header of the message to
 *  which this is a reply.
 */
STATIC mblk_t *
sctp_alloc_reply(rh, clen)
	struct sctphdr *rh;
	size_t clen;
{
	mblk_t *mp;
	struct sctphdr *sh;
	struct sctp_tcb *cb;
	size_t plen = PADC(clen);
	assert(rh);
	if ((mp = allocb(sizeof(*cb) + sizeof(*sh) + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_DATA;
		cb = (struct sctp_tcb *) mp->b_wptr;
		bzero(cb, sizeof(*cb));
		cb->mp = mp;
		cb->dlen = sizeof(*sh) + plen;
		mp->b_rptr += sizeof(*cb);	/* hide control block */
		mp->b_wptr += sizeof(*cb);
		sh = (struct sctphdr *) mp->b_wptr;
		sh->srce = rh->dest;
		sh->dest = rh->srce;
		sh->v_tag = rh->v_tag;
		sh->check = 0;
		mp->b_wptr += sizeof(*sh);
		bzero(mp->b_wptr + clen, plen - clen);
		mp->b_next = NULL;
	}
	return (mp);
}

/* 
 *  FREE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for freeing a chain list of mblk_ts that represent a packet build
 *  out of separate chunks.
 */
STATIC INLINE void
freechunks(mp)
	mblk_t *mp;
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
 *  Send to the normal transmit (primary) address.  If that has timedout, we
 *  use the retransmit (secondary) address.  We always use the secondary
 *  address if we have retransmit chunks to bundle or if we have be receiving
 *  duplicates (our SACKs are not getting through on the primary address).
 *
 */
STATIC struct sctp_daddr *
sctp_route_normal(sp)
	sctp_t *sp;
{
	int err;
	struct sctp_daddr *sd;
	assert(sp);
	if ((err = sctp_update_routes(sp, 1))) {
		rare();
		/* we have no viable route */
		sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -EHOSTUNREACH, NULL);
		__sctp_disconnect(sp);
		return (NULL);
	}
	sd = sp->taddr;
	normal(sd);
	return (sd);
}

/* 
 *  ROUTE RESPONSE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  In response to control chunks we normally send back to the address that
 *  the control chunk came from.  If that address is unusable or wasn't
 *  provided we send as normal.
 */
STATIC struct sctp_daddr *
sctp_route_response(sp)
	sctp_t *sp;
{
	struct sctp_daddr *sd;
	assert(sp);
	sd = sp->caddr;
	if (!sd || !sd->dst_cache || sd->retransmits || sd->dups)
		sd = sctp_route_normal(sp);
	normal(sd);
	return (sd);
}

/* 
 *  WAKEUP (Send SACK, ERROR, DATA)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  TODO: When the user has specified a destination address to sctp_sendmsg(),
 *  we want to suppress nagle and do a destination-specific bundling instead of
 *  our normal route selection.  We could do this by peeking the urgq and the
 *  sndq to check for user-specified destinations before selecting the
 *  desintation and calling sctp_bundle_more.  This looks like it will have a
 *  performance impact so we will leave it for now.
 */
#ifdef SCTP_CONFIG_ADD_IP
STATIC void sctp_send_asconf(sctp_t * sp);
#endif
static void
sctp_transmit_wakeup(sp)
	sctp_t *sp;
{
	int i;
	mblk_t *mp;
	struct sctp_daddr *sd;
	int loop_max = 100;
	printd(("INFO: Performing transmitter wakeup\n"));
	ensure(sp, return);
	if ((1 << sp->s_state) & ~(SCTPF_SENDING | SCTPF_RECEIVING))
		goto skip;
#ifdef SCTP_CONFIG_ADD_IP
	if (sp->s_state == SCTP_ESTABLISHED && sp->sackf & SCTP_SACKF_ASC)
		sctp_send_asconf(sk);
#endif
	/* SCTP IG 2.8, 2.14 */
	for (i = 0, n = 0; i < loop_max && n <= sp->max_burst; i++) {
		/* placed in order of probability */
		if (!bufq_head(&sp->sndq)
		    && !(sp->sackf & SCTP_SACKF_NOW)
		    && !sp->nrtxs && !bufq_head(&sp->urgq)
		    && !bufq_head(&sp->errq))
			goto done;
		if (!sd = sctp_route_normal(sp))
			goto noroute;
		if (!(mp = sctp_alloc_msg(sp, 0)))
			goto enobufs;
		n += sctp_bundle_more(sp, sd, mp, sp->nonagle);
		if (!mp->b_next)
			goto discard;
		sctp_send_msg(sp, sd, mp);
		freechunks(mp);
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
	assure(i > 0 || !(sp->sackf & SCTP_SACKF_NOW));
      skip:
	return;
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
sctp_conn_ind(sp, cp)
	sctp_t *sp;
	mblk_t *cp;
{
	int err;
	ensure(sp->ops->sctp_conn_ind, return (-EFAULT));
	if ((err = sp->ops->sctp_conn_ind(sp, cp))) {
		rare();
		return (err);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  CONNECTION Confirmation
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_conn_con(sp)
	sctp_t *sp;
{
	int err;
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	ensure(sp->ops->sctp_conn_con, return (-EFAULT));
	if ((err = sp->ops->sctp_conn_con(sp))) {
		rare();
		return (err);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  (EX)DATA Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_data_ind(sp, ppi, sid, ssn, tsn, ord, more, dp)
	sctp_t *sp;
	uint32_t ppi;
	uint16_t sid;
	uint16_t ssn;
	uint32_t tsn;
	uint ord;
	uint more;
	mblk_t *dp;
{
	int err;
	ensure(dp, return (-EFAULT));
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if ((err = sp->ops->sctp_data_ind(sp, ppi, sid, ssn, tsn, ord, more, dp))) {
		rare();
		return (err);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  DATACK Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_datack_ind(sp, ppi, sid, ssn, tsn)
	sctp_t *sp;
	uint32_t ppi;
	uint16_t sid;
	uint16_t ssn;
	uint32_t tsn;
{
	int err;
	ensure(dp, return (-EFAULT));
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if ((err = sp->ops->sctp_datack_ind(sp, ppi, sid, ssn, tsn))) {
		rare();
		return (err);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  RESET Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_reset_ind(sp, origin, reason, cp)
	sctp_t *sp;
	ulong origin;
	long reason;
	mblk_t *cp;
{
	int err;
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
		if (sp->ops->sctp_reset_ind) {
			if ((err = sp->ops->sctp_reset_ind(sp, origin, reason, cp))) {
				rare();
				return (err);
			}
			return (0);
		}
		ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
		if ((err = sp->ops->sctp_discon_ind(sp, origin, reason, cp)))
			return (err);
		return (0);
	}
	rare();
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  RESET Confirmation
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_reset_con(sp)
	sctp_t *sp;
{
	int err;
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if (sp->ops->sctp_reset_con) {
		if ((err = sp->ops->sctp_reset_con(sp))) {
			rare();
			return (err);
		}
		return (0);
	}
	seldom();
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  DISCONNECT Indication
 *
 *  -------------------------------------------------------------------------
 *  FIXME:  Need to add data in to support disconnect with data.
 */
STATIC INLINE int
sctp_discon_ind(sp, origin, reason, cp)
	sctp_t *sp;
	ulong origin;
	long reason;
	mblk_t *cp;
{
	int err;
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if ((1 << sp->s_state) & (SCTPF_HAVEUSER)) {
		ensure(sp->ops->sctp_discon_ind, return (-EFAULT));
		if ((err = sp->ops->sctp_discon_ind(sp, origin, reason, cp)))
			return (err);
	}
	if ((err = sctp_disconnect(sp)))
		return (err);
	return (err);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  ORDERLY RELEASE Indication
 *
 *  -------------------------------------------------------------------------
 *  FIXME:  Need to add data in to support orderly release with data and
 *  disconnect with data.
 */
STATIC INLINE int
sctp_ordrel_ind(sp)
	sctp_t *sp;
{
	int err;
	printd(("INFO: Orderly release indication on stream %p\n", sp));
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if (sp->ops->sctp_ordrel_ind)
		return sp->ops->sctp_ordrel_ind(sp);
	if (sp->ops->sctp_discon_ind)
		return sp->ops->sctp_discon_ind(sp, SCTP_ORIG_USER, 0, NULL);
	never();
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  RETRIEVAL Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_retr_ind(sp, dp)
	sctp_t *sp;
	mblk_t *dp;
{
	int err;
	ensure(dp, return (-EFAULT));
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if (sp->ops->sctp_retr_ind)
		return sp->ops->sctp_retr_ind(sp, dp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  RETRIEVAL Confirmation
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sctp_retr_con(sp)
	sctp_t *sp;
{
	int err;
	ensure(dp, return (-EFAULT));
	ensure(sp, return (-EFAULT));
	ensure(sp->ops, return (-EFAULT));
	if (sp->ops->sctp_retr_con)
		return sp->ops->sctp_retr_con(sp);
	return (0);
}

/* 
 *  =========================================================================
 *
 *  SCTP State Machine TIMEOUTS
 *
 *  =========================================================================
 */
#ifdef SCTP_CONFIG_ECN
STATIC INLINE void sctp_send_cwr(sctp_t * sp);
#endif
STATIC void sctp_send_heartbeat(sctp_t * sp, struct sctp_daddr *sd);
STATIC void sctp_reset_idle(struct sctp_daddr *sd);
/* 
 *  ASSOCIATION TIMEOUT FUNCTION
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_assoc_timedout(sp, sd, rmax, stroke)
	sctp_t *sp;			/* association */
	struct sctp_daddr *sd;		/* destination */
	size_t rmax;			/* retry maximum */
	int stroke;			/* whether to stroke counts */
{
	ensure(sp, return (-EFAULT));
	ensure(sd, return (-EFAULT));
	/* RFC 2960 6.3.3 E1 and 7.2.3, E2, E3 and 8.3 */
	sd->ssthresh = sd->cwnd >> 1 > sd->mtu << 1 ? sd->cwnd >> 1 : sd->mtu << 1;
	sd->cwnd = sd->mtu;
	/* SCTP IG 2.9 */
	sd->partial_ack = 0;
	sd->rto = sd->rto ? sd->rto << 1 : 1;
	sd->rto = sd->rto_min > sd->rto ? sd->rto_min : sd->rto;
	sd->rto = sd->rto_max < sd->rto ? sd->rto_max : sd->rto;
#ifdef SCTP_CONFIG_ECN
	sctp_send_cwr(sp);
#endif
	/* SCTP IG 2.15: don't stroke counts on zero window probes, or ECN */
	if (!stroke && sp->s_state != SCTP_COOKIE_WAIT)
		return (0);
	/* See RFC 2960 Section 8.3 */
	if (++sd->retransmits > sd->max_retrans) {
		if (sd->retransmits > sd->max_retrans + 1) {
			if ((1 << sp->s_state) & (SCTPF_CONNECTED))
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
			   available again quickly it if passes the heartbeat. */
			if (sd->rto_max < 2 && sd->max_retries == 0 &&
			    (1 << sp->s_state) & (SCTPF_CONNECTED | SCTPF_CLOSING)) {
				if (sd->timer_idle)
					untimeout(xchg(&sd->timer_idle, 0));
				sctp_send_heartbeat(sp, sd);
			}
#endif
		}
	}
	/* See RFC 2960 Section 8.2 */
	if (rmax && sp->retransmits++ >= rmax) {
		seldom();
		sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -ETIMEDOUT, NULL);
		__sctp_disconnect(sp);
		return (-ETIMEDOUT);
	}
	return (0);
}

#define SCTP_TIMER_BACKOFF 1
/* 
 *  INIT TIMEOUT (T1-init)
 *  -------------------------------------------------------------------------
 *  The init timer has expired indicating that we have not received an INIT
 *  ACK within timer T1-init.  This means that we should attempt to retransmit
 *  the INIT until we have attempted Max.Init.Retrans times.
 */
STATIC void
sctp_init_timeout(data)
	caddr_t data;
{
	sctp_t *sp = (typeof(sp)) data;
	struct sctp_daddr *sd;
	ensure(sp, return);
	sd = sp->taddr;
	ensure(sd, return);
	sctp_bh_lock(sp);
	if (!xchg(&sp->timer_init, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if (sp->s_state != SCTP_COOKIE_WAIT)
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_inits ? sp->max_inits : 1, sp->p_rwnd))
		goto timedout;
	sd = sp->taddr;		/* might have new primary */
	ensure(sd, break);
	set_timeout(sp, &sp->timer_init, sctp_init_timeout, (caddr_t) sd, sd->rto);
	usual(sp->retry);
	sctp_send_msg(sp, sd, sp->retry);
      done:
	sctp_bh_unlock(sp);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sd->timer_init, sctp_init_timeout, sp, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

/* 
 *  COOKIE TIMEOUT
 *  -------------------------------------------------------------------------
 *  The cookie timer has expired indicating that we have not yet received a
 *  COOKIE ACK within timer T1-cookie.  This means that we should attempt to
 *  retransmit the COOKIE ECHO until we have attempted Path.Max.Retrans times.
 */
STATIC void
sctp_cookie_timeout(data)
	caddr_t data;
{
	mblk_t *mp;
	sctp_t *sp;
	struct sctp_daddr *sd = (typeof(sd)) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);
	sctp_bh_lock(sp);
	if (!xchg(&sp->timer_cookie, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if (sp->s_state != SCTP_COOKIE_ECHOED)
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	/* See RFC 2960 6.3.3 E3 */
	for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
		struct sctp_tcb *cb = SCTP_TCB(mp);
		seldom();
		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
		    && !(cb->flags & (SCTPCB_FLAG_RETRANS | SCTPCB_FLAG_DROPPED))) {
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			cb->sacks = 4;	/* not eligible for FR now */
		}
	}
	sd = sp->taddr;		/* might have new primary */
	ensure(sd, goto done);
	set_timeout(sp, &sp->timer_cookie, sctp_cookie_timeout, sd, sd->rto);
	usual(sp->retry);
	sctp_send_msg(sp, sd, sp->retry);
      done:
	sctp_bh_unlock(sp);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sd->timer_cookie, sctp_cookie_timeout, sd, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

/* 
 *  RETRANS TIMEOUT (T3-rtx)
 *  -------------------------------------------------------------------------
 *  This means that we have not received an ack for a DATA chunk within timer
 *  T3-rtx.  This means that we should mark all outstanding DATA chunks for
 *  retransmission and start a retransmission cycle.
 */
STATIC void
sctp_retrans_timeout(data)
	caddr_t data;
{
	mblk_t *mp;
	sctp_t *sp;
	struct sctp_daddr *sd = (typeof(sd)) data;
	int retransmits = 0;
	assert(sd);
	sp = sd->sp;
	assert(sp);
	sctp_bh_lock(sp);
	if (!xchg(&sd->timer_retrans, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if ((1 << sp->s_state) & ~(SCTPF_CONNECTED))
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	/* See RFC 2960 6.3.3 E3 */
	for (mp = bufq_head(&sp->rtxq); mp; mp = mp->b_next) {
		struct sctp_tcb *cb = SCTP_TCB(mp);
		size_t dlen = cb->dlen;
		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
		    && !(cb->flags &
			 (SCTPCB_FLAG_RETRANS | SCTPCB_FLAG_SACKED | SCTPCB_FLAG_DROPPED))) {
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			cb->sacks = 4;	/* not eligible for FR now */
			normal(sd->in_flight >= dlen);
			normal(sp->in_flight >= dlen);
			/* credit dest and assoc */
			sd->in_flight = sd->in_flight > dlen ? sd->in_flight - dlen : 0;
			sp->in_flight = sp->in_flight > dlen ? sp->in_flight - dlen : 0;
			retransmits++;
		}
	}
	normal(retransmits);
#ifdef SCTP_CONFIG_ADD_IP
	if (sd->flags & SCTP_DESTF_UNUSABLE)
		sctp_del_daddr(sd);
#endif
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sp);
		if (!sp->timer_sack)
			mod_timeout(sp, &sp->timer_sack, &sctp_sack_timeout, sp, sp->max_sack);
		sp->sackf |= SCTP_SACKF_NOD;	/* don't delay retransmitted FWD TSN */
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			if (sp->timer_sack)
				untimeout(xchg(&sp->timer_sack, 0));
	}
#endif
	sctp_transmit_wakeup(sp);
      done:
	sctp_bh_unlock(sp);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sd->timer_retrans, sctp_retrans_timeout, sd, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

/* 
 *  SACK TIMEOUT
 *  -------------------------------------------------------------------------
 *  This timer is the 200ms timer which ensures that a SACK is sent within
 *  200ms of the receipt of an unacknoweldged DATA chunk.  When an
 *  unacknowledged DATA chunks i receive and the timer is not running, the
 *  timer is set.  Whenever a DATA chunks(s) are acknowledged, the timer is
 *  stopped.
 */
STATIC void
sctp_sack_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	sp = (sctp_t *) data;
	assert(sp);
	sctp_bh_lock(sp);
	if (!xchg(&sp->timer_sack, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if ((1 << sp->s_state) & ~(SCTPF_RECEIVING))
		goto outstate;
	sp->sackf |= SCTP_SACKF_TIM;	/* RFC 2960 6.2 */
	sctp_transmit_wakeup(sp);
      done:
	sctp_bh_unlock(sp);
	return;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sp->timer_sack, sctp_sack_timeout, sp, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

/* 
 *  IDLE TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that a destination has been idle for longer than the hb.itvl or
 *  the interval for which we must send heartbeats.  This timer is reset every
 *  time we do an RTT calculation for a destination.  It is stopped while
 *  sending heartbeats and started again whenever an RTT calculation is done.
 *  While this timer is stopped, heartbeats will be sent until they are
 *  acknowledged.
 */
STATIC void
sctp_idle_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	struct sctp_daddr *sd = (typeof(sd)) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);
	sctp_bh_lock(sp);
	if (!xchg(&sd->timer_idle, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if ((1 << sp->s_state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	sctp_send_heartbeat(sp, sd);
      done:
	sctp_bh_unlock(sp);
	return;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sp->timer_idle, sctp_idle_timeout, sd, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

/* 
 *  HEARTBEAT TIMEOUT
 *  -------------------------------------------------------------------------
 *  If we get a heartbeat timeout we adjust RTO the same as we do for
 *  retransmit (and the congestion window) and resend the heartbeat.  Once we
 *  have sent Path.Max.Retrans heartbeats unsuccessfully, we mark the
 *  destination as unusable, but continue heartbeating until they get
 *  acknowledged.  (Well!  That's not really true, is it?)
 */
STATIC void
sctp_heartbeat_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	struct sctp_daddr *sd = (typeof(sd)) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);
	sctp_bh_lock(sp);
	if (!xchg(&sd->timer_heartbeat, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	/* SCTP IG 2.10 but we continue through closing states */
	if ((1 << sp->s_state) & ~(SCTPF_CONNECTED | SCTPF_CLOSING))
		goto outstate;
	/* SCTP IG 2.10 */
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, 1))
		goto timedout;
	sctp_reset_idle(sd);
	// sctp_send_heartbeat(sp, sd);
      done:
	sctp_bh_unlock(sp);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sp->timer_heartbeat, sctp_heartbeat_timeout, sd, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

/* 
 *  SHUTDOWN TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timedout on sending a SHUTDOWN or a SHUTDOWN ACK
 *  message.  We simply resend the message.
 */
STATIC void
sctp_shutdown_timeout(data)
	caddr_t data;
{
	sctp_t *sp;
	struct sctp_daddr *sd;
	sd = (struct sctp_daddr *) data;
	assert(sd);
	sp = sd->sp;
	assert(sp);
	sctp_bh_lock(sp);
	if (!xchg(&sp->timer_shutdown, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if (!((1 << sp->s_state) & (SCTPF_CLOSING)))
		goto outstate;
	if (sctp_assoc_timedout(sp, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	sd = sp->taddr;
	ensure(sd, break);
	set_timeout(sp, &sp->timer_shutdown, sctp_shutdown_timeout, (caddr_t) sd, sd->rto);
	usual(sp->retry);
	sctp_send_msg(sp, sd, sp->retry);
      done:
	sctp_bh_unlock(sp);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sp->timer_shutdown, sctp_shutdown_timeout, sd, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

/* 
 *  GUARD TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timed out in the over SHUTDOWN process and need to
 *  ABORT the association per SCTP IG 2.12.
 */
STATIC void sctp_send_abort(sctp_t * sp);
STATIC void
sctp_guard_timeout(data)
	caddr_t data;
{
	sctp_t *sp = (typeof(sp)) data;
	ensure(sp, return);
	sctp_bh_lock(sp);
	if (!xchg(&sp->timer_guard, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if ((1 << sp->s_state) & ~(SCTPF_CLOSING))
		goto outstate;
	sctp_send_abort(sp);
	sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -ETIMEDOUT, NULL);
	__sctp_disconnect(sp);
      done:
	sctp_bh_unlock(sp);
	return;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sp->timer_guard, sctp_guard_timeout, sp, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

#ifdef SCTP_CONFIG_ADD_IP

/* 
 *  ASCONF TIMEOUT
 *  -------------------------------------------------------------------------
 */
STATIC void
sctp_asconf_timeout(data)
	caddr_t data;
{
	sctp_t *sp = (typeof(sp)) data;
	struct sctp_daddr *sd;
	ensure(sp, return);
	sd = sp->taddr;
	ensure(sd, return);
	sctp_bh_lock(sp);
	if (!xchg(&sp->timer_asconf, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if ((1 << sp->s_state) & ~(SCTPF_ESTABLISHED))
		goto outstate;
	/* ADD-IP (B1), (B2) and (B3) */
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto timedout;
	if (!(sd = sctp_route_normal(sp)))
		goto done;
	/* ADD-IP (B5) */
	mod_timeout(sp, &sp->timer_asconf, &sctp_asconf_timeout, sp, sd->rto);
	/* ADD-IP (B4) */
	normal(sp->retry);
	sctp_send_msg(sp, sd, sp->retry);
      done:
	sctp_bh_unlock(sp);
	return;
      timedout:
	ptrace(("WARNING: association timed out\n"));
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sp->timer_asconf, sctp_asconf_timeout, sp, SCTP_TIMER_BACKOFF);
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

#endif				/* SCTP_CONFIG_ADD_IP */

#if defined SCTP_CONFIG_LIFETIMES || defined SCTP_CONFIG_PARTIAL_RELIABILITY

/* 
 *  LIFETIME TIMEOUT
 *  -------------------------------------------------------------------------
 */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
STATIC INLINE void sctp_send_forward_tsn(struct sctp *sp);
#endif
STATIC void
sctp_life_timeout(data)
	caddr_t data;
{
	sctp_t *sp = (typeof(sp)) data;
	mblk_t *mp, *mp_next;
	unsigned long expires = -1UL;
	ensure(sp, return);
	sctp_bh_lock(sp);
	if (!xchg(&sp->timer_life, 0))
		goto cancelled;
	if (sctp_locked(sp))
		goto locked;
	if ((1 << sp->s_state) & ~(SCTPF_SENDING))
		goto outstate;
	mp_next = bufq_head(&sp->sndq);
	while ((mp = mp_next)) {
		struct sctp_tcb *cb = SCTP_TCB(mp);
		mp_next = mp->b_next;
		if (!cb->expires)
			continue;
		if (cb->expires > jiffies) {
			if (expires > cb->expires)
				exports = cb->expires;
			continue;
		}
		cb->flags |= (SCTPCB_FLAG_DROPPED | SCTPCB_FLAG_NACK);
		cb->when = jiffies;
		cb->next = NULL;
		bufq_unlink(mp);
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG)
			bufq_queue(&sp->ackq, mp);
		else
			freemsg(mp);
	}
	mp_next = bufq_head(&sp->urgq);
	while ((mp = mp_next)) {
		struct sctp_tcb *cb = SCTP_TCB(mp);
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
		bufq_unlink(mp);
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG)
			bufq_queue(&sp->ackq, mp);
		else
			freemsg(mp);
	}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	/* need to walk through retransmit queue as well */
	if (!(sp->p_caps & SCTP_CAPS_PR))
		goto done;
	for (mp = bufq_head(&sp->rtxq); mp, mp = mp->b_next) {
		struct sctp_tcb *cb = SCTP_TCB(mp);
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
				/* credit association (now) */
				sd->in_flight = sd->in_flight > dlen ? sd->in_flight - dlen : 0;
				if (!sd->in_flight) {
					if (sd->timer_retrans)
						untimeout(xchg(&sd->timer_retrans, 0));
#ifdef SCTP_CONFIG_ADD_IP
					if (sd->flags & SCTP_DESTF_UNUSABLE)
						sctp_del_daddr(sd);
#endif				/* SCTP_CONFIG_ADD_IP */
				}
			}
			/* credit association (now) */
			normal(sp->in_flight >= dlen);
			sp->in_flight = sp->in_flight > dlen ? sp->in_flight - dlen : 0;
		}
		cb->flags |= SCTPCB_FLAG_DROPPED;
	      push:
		/* push the forward ack point */
		if (cb->tsn == sp->l_fsn + 1) {
			sp->l_fsn++;
		}
	}
	if (expires != -1UL)
		mod_timeout(sp, &sp->timer_life, &sctp_life_timeout, sp, expires - jiffies);
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sp);
		if (!sp->timer_sack)
			mod_timeout(sp, &sp->timer_sack, &sctp_sack_timeout, sp, sp->max_sack);
		sctp_transmit_wakeup(sp);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			if (sp->timer_sack)
				untimeout(xchg(&sp->timer_sack, 0));
	}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
      done:
	sctp_bh_unlock(sp);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	set_timeout(sp, &sp->timer_life, sctp_life_timeout, sp, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
      cancelled:
	ptrace(("WARNING: timer cancelled\n"));
	goto done;
}

#endif				/* defined SCTP_CONFIG_LIFETIMES || defined
				   SCTP_CONFIG_PARTIAL_RELIABILITY */

/* 
 *  SEND DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This function slaps a chunk header onto the M_DATA blocks which form the
 *  data and places it onto the stream's write queue.  The message blocks
 *  input to this function already have a chunk control block prepended.
 *
 *  Under sockets, this function has been replaced by the sockets sendmsg
 *  function.
 */
STATIC int
sctp_send_data(sp, st, flags, dp)
	sctp_t *sp;
	struct sctp_strm *st;
	ulong flags;
	mblk_t *dp;
{
	uint *more;
	mblk_t **head;
	uint32_t ppi;
	size_t mlen, dlen;
	ulong dflags = flags;
	uint urg = (dflags & SCTPCB_FLAG_URG);
	assert(sp);
	assert(st);
	assert(dp);
	if (urg) {
		more = &st->x.more;
		head = &st->x.head;
		ppi = st->x.ppi;
	} else {
		more = &st->n.more;
		head = &st->n.head;
		ppi = st->n.ppi;
	}
	for (mlen = msgdsize(dp); mlen; mlen -= dlen, dflags &= ~SCTPCB_FLAG_FIRST_FRAG) {
		mblk_t *bp;
		struct sctp_daddr *sd;
		if (!(sd = sctp_route_normal(sp)))
			return (-EHOSTUNREACH);
		/* If there is not enough room in the current send window to handle all or at least 
		   1/2 MTU of the data and the current send backlog then return (-EBUSY) and put
		   the message back on the queue so that backpressure will result.  We only do this 
		   separately for normal data and urgent data (urgent data will be expedited ahead
		   of even retransmissions). */
		{
			size_t cwnd, rwnd, swnd, awnd, plen, amps, dmps, used;
			plen = PADC(sizeof(struct sctp_data) + mlen);
			amps = sp->pmtu - sizeof(struct iphdr) - sizeof(struct sctphdr);
			dmps = sd->mtu - sizeof(struct iphdr) - sizeof(struct sctphdr);
			used = urg ? sp->urgq.q_count : sp->sndq.q_count;
			cwnd = sd->cwnd + dmps;
			cwnd = cwnd > sd->in_flight ? cwnd - sd->in_flight : 0;
			rwnd = sp->p_rwnd;
			rwnd = rwnd > sp->in_flight ? rwnd - sp->in_flight : 0;
			swnd = cwnd < rwnd ? cwnd : rwnd;
			awnd = sp->in_flight ? swnd : cwnd;
			awnd = awnd > used ? awnd - used : 0;
			if (plen > awnd || plen > amps) {
				if (plen > amps && awnd >= amps >> 1) {	/* SWS avoidance */
					if ((bp = dupmsg(dp))) {
						dlen = awnd < amps ? amps >> 1 : amps;
						ensure(dlen > sizeof(struct sctp_data), freemsg(bp);
						       return (-EFAULT));
						dlen -= sizeof(struct sctp_data);
						ensure(dlen < mlen, freemsg(bp);
						       return (-EFAULT));
#if 1
						{
							int ret;
							ret = trimhead(dp, dlen);	/* trim
											   original 
											 */
							unless(ret, freemsg(bp);
							       return (-EFAULT));
							ret = trimtail(bp, dlen);	/* trim
											   fragment 
											 */
							unless(ret, freemsg(bp);
							       return (-EFAULT));
						}
#else
						fixme(("Should consider multiple mblks\n"));
						dp->b_rptr = dp->b_rptr + dlen;	/* trim original */
						bp->b_wptr = bp->b_rptr + dlen;	/* trim fragment */
#endif
						dflags &= ~SCTPCB_FLAG_LAST_FRAG;
					} else {
						rare();
						return (-ENOBUFS);
					}
				} else {
					rare();
					return (-EBUSY);
				}
			} else {
				bp = dp;
				dlen = mlen;	/* use entire */
				dflags |= flags & SCTPCB_FLAG_LAST_FRAG;
				/* If we have an existing SDU being built that hasn't been
				   transmitted yet, we just tack data onto it.  We concatenate only 
				   to an MTU. */
				if (*more && *head && plen + SCTP_TCB(*head)->dlen <= amps) {
					struct sctp_data *m;
					struct sctp_tcb *cb = SCTP_TCB(*head);
					cb->flags |= (dflags & 0x7);
					cb->dlen += dlen;
					m = (struct sctp_data *) (*head)->b_rptr;
					m->ch.flags = cb->flags & 0x7;
					m->ch.len = htons(sizeof(*m) + cb->dlen);
					linkb(*head, bp);
					normal(cb->dlen == msgdsize((*head)->b_cont));
					if (dflags & SCTPCB_FLAG_LAST_FRAG) {
						*head = NULL;
						*more = 0;
					}
					return (0);
				}
			}
		}
		{
			mblk_t *mp;
			struct sctp_tcb *cb;
			struct sctp_data *m;
			if ((mp = allocb(sizeof(*cb) + sizeof(*m), BPRI_MED))) {
				mp->b_datap->db_type = M_DATA;
				cb = (struct sctp_tcb *) mp->b_wptr;
				bzero(cb, sizeof(*cb));
				cb->mp = mp;
				cb->dlen = dlen;
				cb->flags = dflags;
				cb->st = st;
				cb->when = jiffies;
				cb->daddr = NULL;	/* set when transmitted */
				cb->ppi = ppi;
				cb->sid = st->sid;
				cb->ssn = urg ? 0 : st->ssn;
				mp->b_rptr += sizeof(*cb);	/* hide control block */
				mp->b_wptr += sizeof(*cb);
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
				/* Remember where we can add more data in case data completing a
				   SDU comes before we are forced to bundle the DATA. */
				*more = (dflags & SCTPCB_FLAG_LAST_FRAG) ? 0 : 1;
				*head = (dflags & SCTPCB_FLAG_LAST_FRAG) ? NULL : mp;
				if (urg) {
					bufq_queue(&sp->urgq, mp);
				} else {
					if (dflags & SCTPCB_FLAG_LAST_FRAG)
						st->ssn = (st->ssn + 1) & 0xffff;
					bufq_queue(&sp->sndq, mp);
				}
			} else {
				rare();
				return (-ENOBUFS);
			}
		}
	}
	return (0);
}

/* 
 *  SEND SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_sack(sp)
	sctp_t *sp;
{
	sp->sackf |= SCTP_SACKF_NOD;
}

/* 
 *  SEND ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We just queue the error, we don't send it out...  It gets bundled with
 *  other things.  This also provides for SCTP IG 2.27.
 */
STATIC int
sctp_send_error(sp, ecode, eptr, elen)
	sctp_t *sp;
	uint ecode;
	caddr_t eptr;
	size_t elen;
{
	mblk_t *mp;
	struct sctp_tcb *cb;
	struct sctp_error *m;
	struct sctpehdr *eh;
	size_t clen = sizeof(*m) + sizeof(*eh) + elen;
	size_t plen = PADC(clen);
	assert(sp);
	if (!(mp = sctp_alloc_chk(sp, clen)))
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
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
}

/* 
 *  SEND INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the INIT and get timers started, we must return an
 *  error to the user interface calling this function.
 */
STATIC int
sctp_send_init(sp)
	sctp_t *sp;
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
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	struct sctp_ali *ai;
#endif
#ifdef SCTP_CONFIG_PARITAL_RELIABILITY
	struct sctp_pr_sctp *pr;
#endif
	size_t sanum = sp->sanum;
	size_t clen = sizeof(*m)
	    + (sanum * PADC(sizeof(*ap)))
	    + (sp->ck_inc ? PADC(sizeof(*cp)) : 0)
	    + (sizeof(*at) + sizeof(at->type[0]))
#ifdef SCTP_CONFIG_ECN
	    + (sp->l_caps & SCTP_CAPS_ECN ? PADC(sizeof(*ec)) : 0)
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	    + (sp->l_caps & SCTP_CAPS_ALI ? PADC(sizeof(*ai)) : 0)
#endif
#ifdef SCTP_CONFIG_PARITAL_RELIABILITY
	    + (sp->l_caps & SCTP_CAPS_PR ? PADC(sizeof(*pr)) : 0)
#endif
	;
	if (!(sd = sp->daddr))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr++;
	m->ch.type = SCTP_CTYPE_INIT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = sp->v_tag;
	m->a_rwnd = htonl(sp->a_rwnd);
	m->n_istr = htons(sp->max_istr);
	m->n_ostr = htons(sp->req_ostr);
	m->i_tsn = htonl(sp->v_tag);
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
	if (sp->l_caps & SCTP_CAPS_ECN) {
		ec = (typeof(ec)) mp->b_wptr;
		ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
		ec->ph.len = __constant_htons(sizeof(*ec));
		mp->b_wptr += PADC(sizeof(*ec));
	}
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) mp->b_wptr;
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htons(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
		mp->b_wptr += PADC(sizeof(*ai));
	}
#endif
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) mp->b_wptr;
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
	}
#endif
	mod_timeout(sp, &sp->timer_init, &sctp_init_timeout, sp, sd->rto);
	unusual(sp->retry);
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
	sp->s_state = SCTP_COOKIE_WAIT;
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      noroute:
	rare();
	return (-EFAULT);
}

/* 
 *  SEND INIT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  No s_state change results from replying to an INIT.  INIT ACKs are sent
 *  without a TCB but a STREAM is referenced.  INIT ACK chunks cannot have any
 *  other chunks bundled with them. (RFC 2960 6.10).
 */
STATIC void
sctp_send_init_ack(sp, daddr, sh, ck)
	sctp_t *sp;
	uint32_t daddr;
	struct sctphdr *sh;
	struct sctp_cookie *ck;
{
	mblk_t *mp;
	struct sctp_saddr *ss;
	struct sctphdr *rh;
	struct sctp_init_ack *m;
	struct sctpphdr *ph;
#ifdef SCTP_CONFIG_ECN
	struct sctp_ecn_capable *ec;
#endif
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	struct sctp_pr_sctp *pr;
#endif
#if defined SCTP_CONFIG_ADD_IP || SCTP_CONFIG_ADAPTATION_LAYER_INFO
	struct sctp_ali *ai;
#endif
	struct sctp_cookie *cp;
	struct sctp_ipv4_addr *ap;
	struct sctp_init *im = (struct sctp_init *) (sh + 1);
	unsigned char *init = (unsigned char *) im;
	int anum = ck->danum;
	int snum = ck->sanum;
	size_t klen = sizeof(*ph) + raw_cookie_size(ck) + HMAC_SIZE;
	size_t dlen = sp->sanum * PADC(sizeof(*ap));
	size_t clen = sizeof(*m) + dlen + klen
#ifdef SCTP_CONFIG_ECN
	    + PADC(sizeof(*ec))
#endif
#ifdef SCTP_CONFIG_ADD_IP || SCTP_CONFIG_ADAPTATION_LAYER_INFO
	    + ((sp->l_caps & SCTP_CAPS_ALI) ? PADC(sizeof(*ai)) : 0)
#endif
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	    + ((sp->l_caps & SCTP_CAPS_PR) ? PADC(sizeof(*pr)) : 0)
#endif
	;
	int arem, alen;
	assert(sp);
	if (!(mp = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	rh = ((struct sctphdr *) mp->b_wptr) - 1;
	rh->v_tag = im->i_tag;
	m = (struct sctp_init_ack *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_INIT_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = ck->v_tag;
	m->a_rwnd = htonl(sp->a_rwnd);
	m->n_istr = htons(ck->n_istr);
	m->n_ostr = htons(ck->n_ostr);
	m->i_tsn = htonl(ck->v_tag);
	mp->b_wptr += sizeof(*m);
	if (!(sp->userlocks & SCTP_BINDADDR_LOCK)) {
	}
	for (ss = sp->saddr; ss; ss = ss->next) {
		ap = (struct sctp_ipv4_addr *) mp->b_wptr;
		ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
		ap->ph.len = __constant_htons(sizeof(*ap));
		ap->addr = ss->saddr;
		mp->b_wptr += PADC(sizeof(*ap));
	}
	ph = (typeof(ph)) mp->b_wptr++;
	ph->type = SCTP_PTYPE_STATE_COOKIE;
	ph->len = htons(klen);
	cp = (typeof(cp)) mp->b_wptr++;
	bcopy(ck, cp, sizeof(*cp));
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
	for (ap = (typeof(ap)) (init + sizeof(struct sctp_init)),
	     arem = PADC(htons(((struct sctpchdr *) init)->len)) - sizeof(struct sctp_init);
	     anum && arem >= sizeof(struct sctpphdr);
	     arem -= PADC(alen), ap = (typeof(ap)) (((uint8_t *) ap) + PADC(alen))) {
		if ((alen = ntohs(ap->ph.len)) > arem) {
			assure(alen <= arem);
			freemsg(mp);
			rare();
			return;
		}
		if (ap->ph.type == SCTP_PTYPE_IPV4_ADDR) {
			/* skip primary */
			if (ap->addr != ck->daddr) {
				*((uint32_t *) mp->b_wptr)++ = ap->addr;
				anum--;
			}
		}
	}
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (ss->saddr != ck->saddr) {
			*((uint32_t *) mp->b_wptr)++ = ss->saddr;
			snum--;
		}
	}
	assure(!anum);
	assure(!snum);
	sctp_sign_cookie(sp, cp);
	mp->b_wptr += HMAC_SIZE;
#ifdef SCTP_CONFIG_ECN
	ec = (typeof(ec)) mp->b_wptr++;
	ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
	ec->ph.len = __constant_htons(sizeof(*ec));
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) mp->b_wptr++;
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htnos(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
	}
#endif
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) mp->b_wptr++;
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
	}
#endif
	/* SCTP IG 2.27 */
	sctp_bundle_errs(sp, mp);
	sctp_xmit_msg(daddr, mp, sp);
	return;
      enobufs:
	rare();
	return;
}

/* 
 *  SEND COOKIE ECHO
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the COOKIE ECHO and get timers started, we must
 *  return an error to the user interface calling this function.
 */
STATIC int
sctp_send_cookie_echo(sp, kptr, klen)
	sctp_t *sp;
	caddr_t kptr;
	size_t klen;
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_cookie_echo *m;
	size_t clen = sizeof(*m) + klen;
	size_t plen = PADC(clen);
	assert(sp);
	if (!(sd = sp->daddr))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_cookie_echo *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_COOKIE_ECHO;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	bcopy(kptr, (m + 1), klen);
	mp->b_wptr += plen;
	sctp_bundle_more(sp, sd, mp, 1);	/* don't nagle */
	mod_timeout(sp, &sp->timer_cookie, &sctp_cookie_timeout, sd, sd->rto);
	unusual(sp->retry);
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
	sp->s_state = SCTP_COOKIE_ECHOED;
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
STATIC void
sctp_send_cookie_ack(sp)
	sctp_t *sp;
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_cookie_ack *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	assert(sp);
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_cookie_ack *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_COOKIE_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += plen;
	sctp_bundle_more(sp, sd, mp, 1);	/* don't nagle */
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
      enobufs:
	sp->s_state = SCTP_ESTABLISHED;
	/* start idle timers */
	for (sd = sp->daddr; sd; sd = sd->next)
		sctp_reset_idle(sd);
      noroute:
	return;
}

/* 
 *  SEND HEARTBEAT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We don't send heartbeats when idle timers expire if we are in the wrong
 *  state, we just reset the idle timer.
 */
STATIC void
sctp_send_heartbeat(sp, sd)
	sctp_t *sp;
	struct sctp_daddr *sd;
{
	mblk_t *mp;
	struct sctp_heartbeat *m;
	struct sctp_heartbeat_info *h;
	size_t fill, clen, hlen, plen;
	assert(sp);
	ensure(sd, return);
	fill = sd->hb_fill;
	clen = sizeof(*m) + sizeof(*h) + fill;
	hlen = clen - sizeof(struct sctpchdr);
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
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
      enobufs:
	mod_timeout(sp, &sd->timer_heartbeat, &sctp_heartbeat_timeout, sd, sd->rto);
}

/* 
 *  SEND HEARTBEAT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Take the incoming HEARTBEAT message and turn it back around as a HEARTBEAT
 *  ACK message.  Note that if the incoming chunk parameters are invalid, so
 *  are the outgoing parameters, this is because the hb_info parameter is
 *  opaque to us.  This is consistent with draft-stewart-ong-sctpbakeoff-
 *  sigtran-01.
 */
STATIC void
sctp_send_heartbeat_ack(sp, hptr, hlen)
	sctp_t *sp;
	caddr_t hptr;
	size_t hlen;
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_heartbeat_ack *m;
	size_t clen = sizeof(*m) + hlen;
	size_t plen = PADC(clen);
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
	mp->b_wptr += plen;
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
	return;
      noroute:
      enobufs:
	rare();
	return;
}

/* 
 *  SEND ABORT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  There is no point in bundling control chunks after an ABORT chunk.  Also,
 *  DATA chunks are not to be bundled with ABORT chunks.
 */
STATIC void
sctp_send_abort(sp)
	sctp_t *sp;
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	if ((1 << sp->s_state) & ~(SCTPF_NEEDABORT))
		goto notneeded;
	if ((1 << sp->s_state) & SCTPF_CONNECTED)
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
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
      enobufs:
      noroute:
	sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
      notneeded:
	return;
}

/* 
 *  SEND ABORT (w/ERROR CAUSE)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Errors (beyond the error header) must be formatted by the called and
 *  indicated by are and len.  There is no point in bundling data or control
 *  chunks after and abort chunk.
 */
STATIC void
sctp_send_abort_error(sp, errn, aptr, alen)
	sctp_t *sp;
	int errn;
	void *aptr;			/* argument ptr */
	size_t alen;			/* argument len */
{
	struct sctp_daddr *sd;
	mblk_t *mp;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	if ((1 << sp->s_state) & ~(SCTPF_NEEDABORT))
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
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
      enobufs:
      noroute:
	sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
	return;
      noerror:
	sctp_send_abort(sp);
      notneeded:
	return;
}

/* 
 *  SEND SHUTDOWN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  RFC 2960 6.2 "... DATA chunks cannot be bundled with SHUTDOWN or SHUTDOWN
 *  ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN and get timers started, we must inform
 *  the user interface calling this function.
 */
STATIC int
sctp_send_shutdown(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_shutdown *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	assert(sp);
	if (!(sd = sctp_route_normal(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_shutdown *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	m->c_tsn = htonl(sp->r_ack);
	mp->b_wptr += plen;
	/* shutdown acks everything but dups and gaps */
	sp->sackf &= (SCTP_SACKF_DUP | SCTP_SACKF_GAP);
	if (sp->timer_sack)
		untimeout(xchg(&sp->timer_sack, 0));
#ifdef SCTP_CONFIG_ADD_IP
	if (sp->timer_asconf)
		untimeout(xchg(&sp->timer_asconf, 0));
#endif
	sctp_bundle_more(sp, sd, mp, 1);	/* not DATA */
	mod_timeout(sp, &sp->timer_shutdown, &sctp_shutdown_timeout, sd, sd->rto);
	// unusual( sp->retry ); /* not that unusual */
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sp->s_state != SCTP_SHUTDOWN_SENT)
		mod_timeout(sp, &sp->timer_guard, &sctp_guard_timeout, sp, 5 * sd->rto);
	sp->s_state = SCTP_SHUTDOWN_SENT;
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
 *  SHUTDOWN ACK is sent in response to a SHUTDOWN message after all data has
 *  cleared or in reponse to a COOKIE ECHO during the SHUTDOWN_ACK_SENT s_state.
 *  If the error flag is set, we want to bundle and ERROR chunk with the
 *  SHUTDOWN ACK indicating "cookie received while shutting down."
 *
 *  RFC 2960 6.2. "...  DATA chunks cannot be bundled with SHUTDOWN or
 *  SHUTDOWN ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN ACK and get timers started, we must return
 *  an error to the user interface calling this function.
 */
STATIC int
sctp_send_shutdown_ack(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_shutdown_ack *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	assert(sp);
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (struct sctp_shutdown_ack *) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += plen;
	sctp_bundle_more(sp, sd, mp, 1);	/* not DATA */
	if (sp->timer_sack)
		untimeout(xchg(&sp->timer_sack, 0));
#ifdef SCTP_CONFIG_ADD_IP
	if (sp->timer_asconf)
		untimeout(xchg(&sp->timer_asconf, 0));
#endif
	mod_timeout(sp, &sp->timer_shutdown, &sctp_shutdown_timeout, sd, sd->rto);
	unusual(sp->retry);
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sp->s_state != SCTP_SHUTDOWN_ACK_SENT)
		mod_timeout(sp, &sp->timer_guard, &sctp_guard_timeout, sp, 5 * sd->rto);
	sp->s_state = SCTP_SHUTDOWN_ACK_SENT;
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
sctp_send_shutdown_complete(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	if (bufq_size(&sp->sndq) || bufq_size(&sp->urgq) || bufq_size(&sp->rtxq))
		swerr();
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += plen;
	sctp_send_msg(sp, sd, mp);
	freechunks(mp);
	sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
	return;
      enobufs:
      noroute:
	sp->s_state = sp->conind ? SCTP_LISTEN : SCTP_CLOSED;
	return;
}

#ifdef SCTP_CONFIG_ECN
/* 
 *  SEND_ECNE (Explicit Congestion Notification Echo)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Just mark an ECNE chunk to be sent with the next SACK (and don't delay
 *  SACKs).
 */
STATIC INLINE void
sctp_send_ecne(sp)
	sctp_t *sp;
{
	if (sp->p_casp & SCTP_CAPS_ECN)
		sp->sackf |= SCTP_SACKF_ECN;
}

/* 
 *  SEND_CWR (Congestion Window Reduction)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Just mark a CWR chunk to be bundled with the next DATA.
 */
STATIC INLINE void
sctp_send_cwr(sp)
	sctp_t *sp;
{
	sp->sackf |= SCTP_SACKF_CWR;
}
#endif				/* SCTP_CONFIG_ECN */

#ifdef SCTP_CONFIG_ADD_IP
/* 
 *  ABORT ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_abort_asconf(sp)
	sctp_t *sp;
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
	if (sp->s_state == SCTP_ESTABLISHED) {
		if (sp->timer_asconf)
			untimeout(xchg(&sp->timer_asconf, 0));
	}
	return;
}

/* 
 *  SEND ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_asconf(sp)
	sctp_t *sp;
{
	struct mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_asconf *m;
	struct sctp_add_ip *a;
	struct sctp_del_ip *d;
	struct sctp_set_ip *s;
	struct sctp_ipv4_addr *p;
	size_t clen = sizeof(*m);
	struct sctp_saddr *ss;
	int requested = 0;
	if (sp->s_state != SCTP_ESTABLISHED)
		goto skip;
	if (!(sp->p_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto skip;
	ensure(sp->sackf & SCTP_SACKF_ASC, goto skip);
	if (!(sd = sctp_route_normal(sp)))
		goto noroute;
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (sp->p_caps & SCTP_CAPS_ADD_IP) {
			if (ss->flfags & SCTP_SRCEF_ADD_REQUEST) {
				clen += sizeof(*a) + sizeof(*p);
				requested++;
			}
			if (ss->flfags & SCTP_SRCEF_DEL_REQUEST) {
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
	m = ((typeof(m)) mp->b_wptr)++;
	m->ch.type = SCTP_CTYPE_ASCONF;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	/* ADD-IP 4.1 (A2) */
	m->asn = htonl(++sp->l_asn);
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (sp->p_caps & SCTP_CAPS_ADD_IP) {
			if (ss->flags & SCTP_SRCEF_ADD_REQUEST) {
				a = ((typeof(a)) mp->b_wptr)++;
				a->ph.type = SCTP_PTYPE_ADD_IP;
				a->ph.len = __constant_htons(sizeof(*a) + sizeof(*p));
				a->id = htonl((uint32_t) ss);
				p = ((typeof(p)) mp->b_wptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_ADD_REQUEST;
				ss->flags |= SCTP_SRCEF_ADD_PENDING;
			}
			if (ss->flags & SCTP_SRCEF_DEL_REQUEST) {
				d = ((typeof(d)) mp->b_wptr)++;
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
				s = ((typeof(s)) mp->b_wptr)++;
				s->ph.type = SCTP_PTYPE_SET_IP;
				s->ph.len = __constant_htons(sizeof(*s) + sizeof(*p));
				s->id = htonl((uint32_t) ss);
				p = ((typeof(p)) mp->b_wptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_SET_REQUEST;
				ss->flags |= SCTP_SRCEF_SET_PENDING;
			}
		}
	}
	sp->sackf &= ~SCTP_SACKF_ASC;
	sctp_bundle_more(sp, sd, mp, 1);
	/* ADD-IP 4.1 (A4) */
	sctp_send_msg(sp, sd, mp);
	freechunks(xchg(&sp->retry, mp));
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
sctp_send_asconf_ack(sp, rptr, rlen)
	sctp_t *sp;
	caddr_t rptr;
	size_t rlen;
{
	mblk_t *mp;
	struct sctp_daddr *sd;
	struct sctp_asconf_ack *m;
	size_t clen = sizeof(*m) + rlen;
	size_t plen = PADC(clen);
	if (!(sd = sctp_route_response(sp)))
		goto noroute;
	if (!(mp = sctp_alloc_msg(sp, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ASCONF_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->asn = ntohl(sp->p_asn)++;
	memcpy(m + 1, rptr, rlen);	/* copy in response TLVs */
	sctp_bundle_more(sp, sd, mp, 1);
	sctp_send_msg(sp, sd, mp);
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
STATiC INLINE void
sctp_send_forward_tsn(sp)
	sctp_t *sp;
{
	struct sctp_daddr *sd;
	/* PR-SCTP 3.5 (F2) */
	sp->sackf |= SCTP_SACKF_FSN;
	for (sd = sp->daddr; sd; sd = sd->next) {
		if (sd->flags & SCTP_DESTF_FORWDTSN) {
			if (!sd->in_flight && sd->timer_retrans)
				untimeout(xchg(&sd->timer_retrans, 0));
			sd->flags &= ~SCTP_DESTF_FORWDTSN;
		}
	}
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

/* 
 *  SENDING WITHOUT TCB  (Responding to OOTB packets)
 *  -------------------------------------------------------------------------
 *  When sending without an SCTP TCB, we only have the IP header and the SCTP
 *  header from which to work.  We have no associated STREAM.  These are
 *  usually used for replying to OOTB messages.
 *
 *  SEND ABORT (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static void
sctp_send_abort_ootb(daddr, saddr, sh)
	uint32_t daddr;
	uint32_t saddr;
	struct sctphdr *sh;
{
	mblk_t *mp;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	if (!sh)
		goto noroute;
	if (!(mp = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += plen;
	sctp_xmit_ootb(daddr, saddr, mp);
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
sctp_send_abort_error_ootb(daddr, saddr, sh, errn, aptr, alen)
	uint32_t daddr;			/* dest address */
	uint32_t saddr;			/* srce address */
	struct sctphdr *sh;		/* SCTP header */
	int errn;			/* error number */
	caddr_t aptr;			/* argument ptr */
	size_t alen;			/* argument len */
{
	mblk_t *mp;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	size_t plen = PADC(clen);
	if (!sh)
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(mp = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) (m + 1);
	eh->code = htons(errn);
	eh->len = htons(elen);
	bcopy(aptr, (eh + 1), alen);
	mp->b_wptr += plen;
	sctp_xmit_ootb(daddr, saddr, mp);
	return;
      noerror:
	sctp_send_abort_ootb(daddr, saddr, sh);
      noroute:
      enobufs:
	rare();
	return;
}

/* 
 *  SEND SHUTDOWN COMPLETE (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static void
sctp_send_shutdown_complete_ootb(daddr, saddr, sh)
	uint32_t daddr;
	uint32_t saddr;
	struct sctphdr *sh;
{
	mblk_t *mp;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	if (!sh)
		goto noroute;
	if (!(mp = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) mp->b_wptr;
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
	mp->b_wptr += plen;
	sctp_xmit_ootb(daddr, saddr, mp);
      enobufs:
      noroute:
	rare();
	return;
}

/* 
 *  BIND_REQ:
 *  -------------------------------------------------------------------------
 *  Bind the stream to the addresses provided in its bound address lists.
 *  There are some errors that may be returned here:
 *
 *  Any UNIX error.
 *
 *  NOADDR      - A wildcard address was specified and we don't support
 *                wildcards (maybe we will).
 *
 *  ADDRBUSY    - 
 *
 */
static int
sctp_bind_req(sp, sport, sptr, snum, cons)
	sctp_t *sp;
	uint16_t sport;
	uint32_t *sptr;
	size_t snum;
	ulong cons;
{
	int err;
	assert(sp);
	if (!cons && !sport) {
		rare();
		return (-EADDRNOTAVAIL);
	}
	if ((err = sctp_alloc_saddrs(sp, sport, sptr, snum))) {
		rare();
		return (err);
	}
	if (cons && (!sp->sanum || !sp->saddr)) {
		rare();
		return (-EADDRNOTAVAIL);
	}
	if (cons)
		sp->s_state = SCTP_LISTEN;
	else
		sp->s_state = SCTP_CLOSED;
	if ((err = sctp_bind_hash(sp, cons))) {
		rare();
		return (err);
	}
	sp->conind = cons;
	return (0);
}

/* 
 *  CONN_REQ:
 *  -------------------------------------------------------------------------
 *  Connect to the peer.  This launches the INIT process.
 */
static int
sctp_conn_req(sp, dport, dptr, dnum, dp)
	sctp_t *sp;
	uint16_t dport;
	uint32_t *dptr;
	size_t dnum;
	mblk_t *dp;
{
	int err;
	assert(sp);
	if (!dport) {
		rare();
		return (-EADDRNOTAVAIL);
	}
	if ((err = sctp_alloc_daddrs(sp, dport, dptr, dnum))) {
		rare();
		return (err);
	}
	if (!sp->daddr || !sp->danum) {
		rare();
		return (-EADDRNOTAVAIL);
	}
	sp->v_tag = sctp_get_vtag(sp->daddr->daddr, sp->saddr->saddr, sp->dport, sp->sport);
	sp->p_tag = 0;
	if ((err = sctp_conn_hash(sp))) {
		rare();
		return (err);
	}
	/* XXX */
	if ((err = sctp_update_routes(sp, 1))) {
		rare();
		return (err);
	}
	sctp_reset(sp);		/* clear old information */
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->t_tsn = sp->v_tag;
	sp->t_ack = sp->v_tag - 1;
	sp->r_ack = 0;
	/* fake a data request if data in conn req */
	if (dp) {
		seldom();
		if ((err = sctp_data_req(sp, sp->ppi, sp->sid, 0, 0, 0, dp))) {
			rare();
			return (err);
		}
	}
	if ((err = sctp_send_init(sp))) {
		rare();
		return (err);
	}
	return (0);
}

/* 
 *  CONN_RES:
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_return_more(mblk_t *mp);
static int
sctp_conn_res(sp, cp, ap, dp)
	sctp_t *sp;
	mblk_t *cp;
	sctp_t *ap;
	mblk_t *dp;
{
	int err;
	struct sctp_cookie_echo *m;
	struct sctp_cookie *ck;
	uint32_t *daddrs;
	uint32_t *saddrs;
	assert(sp);
	assert(cp);
	assert(ap);
	m = (struct sctp_cookie_echo *) cp->b_rptr;
	ck = (struct sctp_cookie *) m->cookie;
	daddrs = (uint32_t *) (ck + 1);
	saddrs = daddrs + ck->danum;
	sctp_unbind(ap);	/* we need to rebind the accepting stream */
	if ((err = sctp_alloc_saddrs(ap, ck->sport, saddrs, ck->sanum))) {
		rare();
		return (err);
	}
	if (!sctp_saddr_include(ap, ck->saddr, &err) && err) {
		rare();
		return (err);
	}
	if ((err = sctp_bind_hash(ap, ap->conind))) {
		rare();
		return (err);
	}
	if ((err = sctp_alloc_daddrs(ap, ck->dport, daddrs, ck->danum))) {
		rare();
		return (err);
	}
	if (!sctp_daddr_include(ap, ck->daddr, &err) && err) {
		rare();
		return (err);
	}
	ap->v_tag = ck->v_tag;
	ap->p_tag = ck->p_tag;
	if ((err = sctp_conn_hash(ap))) {
		rare();
		return (err);
	}
	/* XXX */
	if ((err = sctp_update_routes(ap, 1))) {
		rare();
		return (err);
	}
	sctp_reset(ap);		/* clear old information */
	ap->n_istr = ck->n_istr;
	ap->n_ostr = ck->n_ostr;
	ap->t_tsn = ck->v_tag;
	ap->t_ack = ck->v_tag - 1;
	ap->r_ack = ck->p_tsn - 1;
	ap->p_rwnd = ck->p_rwnd;
	ap->s_state = SCTP_ESTABLISHED;
	/* process any chunks bundled with cookie echo on accepting stream */
	if (sctp_return_more(cp) > 0)
		sctp_recv_msg(ap, cp);
	/* fake a data request if data in conn res */
	if (dp) {
		if ((err = sctp_data_req(ap, ap->ppi, ap->sid, 0, 0, 0, dp))) {
			rare();
			return (err);
		}
	}
	sctp_send_cookie_ack(ap);
	/* caller will unlink connect indication */
	return (0);
}

/* 
 *  DATA_REQ:
 *  -------------------------------------------------------------------------
 */
static int
sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)
	sctp_t *sp;
	uint32_t ppi;
	uint16_t sid;
	uint ord;			/* when non-zero, indicates ordered delivery */
	uint more;			/* when non-zero, indicates more data to follow */
	uint rcpt;			/* when non-zero, indicates receipt conf requested */
	mblk_t *mp;
{
	uint err = 0, flags = 0;
	struct sctp_strm *st;

	ensure(mp, return (-EFAULT));

	/* don't allow zero-length data through */
	if (!msgdsize(mp)) {
		freemsg(mp);
		return (0);
	}
	if (!(st = sctp_ostrm_find(sp, sid, &err))) {
		rare();
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
	return sctp_send_data(sp, st, flags, mp);
}

/* 
 *  RESET_REQ:
 *  -------------------------------------------------------------------------
 *  Don't know what to do here, probably nothing...
 *
 *  Gee we could keep a copy of the old cookie against the stream of we
 *  actively connected and send a COOKIE ECHO to generate a RESTART at the
 *  other end????
 */
static int
sctp_reset_req(sp)
	sctp_t *sp;
{
	int err;
	/* do nothing */
	if ((err = sctp_reset_con(sp)))
		return (err);
	return (0);
}

/* 
 *  RESET_RES:
 *  -------------------------------------------------------------------------
 */
static int
sctp_reset_res(sp)
	sctp_t *sp;
{
	mblk_t *cp;
	if (!(cp = bufq_dequeue(&sp->conq))) {
		rare();
		return (-EFAULT);
	}
	return sctp_conn_res(sp, cp, sp, NULL);
}

/* 
 *  DISCON_REQ:
 *  -------------------------------------------------------------------------
 */
static int
sctp_discon_req(sp, cp)
	sctp_t *sp;
	mblk_t *cp;
{
	/* Caller must ensure that sp and cp (if any) are correct and appropriate. */
	if (cp) {
		struct iphdr *iph = (struct iphdr *) cp->b_datap->db_base;
		struct sctphdr *sh = (struct sctphdr *) (cp->b_datap->db_base + (iph->ihl << 2));
		sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		/* conn ind will be unlinked by caller */
		return (0);
	}
	sctp_send_abort(sp);
	sctp_disconnect(sp);
	return (0);
}

/* 
 *  ORDREL_REQ:
 *  -------------------------------------------------------------------------
 */
static int
sctp_ordrel_req(sp)
	sctp_t *sp;
{
	switch (sp->s_state) {
	case SCTP_ESTABLISHED:
		if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
			sctp_send_shutdown(sp);
		else
			sp->s_state = SCTP_SHUTDOWN_PENDING;
		return (0);
	case SCTP_SHUTDOWN_RECEIVED:
		if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
			sctp_send_shutdown_ack(sp);
		else
			sp->s_state = SCTP_SHUTDOWN_RECVWAIT;
		return (0);
	}
	rare();
	// ptrace(("sp->s_state = %d\n", sp->s_state));
	return (-EPROTO);
}

/* 
 *  UNBIND_REQ:
 *  -------------------------------------------------------------------------
 */
static int
sctp_unbind_req(sp)
	sctp_t *sp;
{
	switch (sp->s_state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		/* can't wait for SHUTDOWN COMPLETE any longer */
		sctp_disconnect(sp);
	case SCTP_CLOSED:
	case SCTP_LISTEN:
		sctp_unbind(sp);
		return (0);
	}
	rare();
	return (-EPROTO);
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
 */
STATIC int
sctp_return_more(mp)
	mblk_t *mp;
{
	int ret;
	size_t plen;
	struct sctpchdr *ch;
	assert(mp);
	ch = (struct sctpchdr *) mp->b_rptr;
	plen = PADC(ntohs(ch->len));
	mp->b_rptr += plen;
	ret = mp->b_wptr - mp->b_rptr;
	if (mp->b_rptr > mp->b - wptr)
		mp->b_rptr = mp->b_wptr;
	ret = (ret < 0 || (0 < ret && ret < sizeof(*ch))) ? -EMSGSIZE : ret;
	unusual(ret < 0);
	return (ret);
}

/* 
 *  RETURN VALUE when not expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_return_stop(mp)
	mblk_t *mp;
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
sctp_return_check(mp, ctype)
	mblk_t *mp;
	uint ctype;
{
	int ret = sctp_return_more(mp);
	ret = (ret > 0 && ((struct sctpchdr *) mp->b_rptr)->type != ctype) ? -EPROTO : ret;
	unusual(ret < 0);
	return (ret);
}
#endif
/* is s2<=s1<=s3 ? */
#define between(__s1,__s2,__s3)((uint32_t)(__s3)-(uint32_t)(__s2)>=(uint32_t)(__s1)-(uint32_t)(__s2))
#define before(__s1,__s2) (((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))<0))
#define after(__s1,__s2) (((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))<0))

#define between_s(__s1,__s2,__s3)((uint16_t)(__s3)-(uint16_t)(__s2)>=(uint16_t)(__s1)-(uint16_t)(__s2))
#define before_s(__s1,__s2) (((int16_t)((uint16_t)(__s1)-(uint16_t)(__s2))<0))
#define after_s(__s1,__s2) (((int16_t)((uint16_t)(__s2)-(uint16_t)(__s1))<0))

/* 
 *  RESET IDLE
 *  -------------------------------------------------------------------------
 *  Reset the idle timer for generation of heartbeats.  Stop any heartbeating
 *  that we might be involved in at the moment.
 */
STATIC void
sctp_reset_idle(sd)
	struct sctp_daddr *sd;
{
	unsigned long rtt;
	assert(sd);
	if (sd->timer_heartbeat)
		untimeout(xchg(&sd->timer_heartbeat, 0));
	rtt = sd->rto + sd->hb_itvl + ((jiffies & 0x1) * (sd->rto >> 1));
	mod_timeout(sd->sp, &sd->timer_idle, &sctp_idle_timeout, sd, rtt);
}

/* 
 *  RTT CALC
 *  -------------------------------------------------------------------------
 *  Round Trip Time calculations for messages acknowledged on the first
 *  transmission.  When a message is acknowledged, this function peforms and
 *  update of the RTT calculation if appropriate.  It is called by
 *  sctp_ack_calc for control chunks which expect acknowledgements, and by
 *  sctp_dest_calc when DATA chunks are acknolwedged on first transmission via
 *  SACK or SHUTDOWN chunks, and by sctp_recv_heartbeat_ack when calculating
 *  RTTs for HEARTBEAT chunks.
 */
STATIC void
sctp_rtt_calc(sd, time)
	struct sctp_daddr *sd;
	unsigned long time;
{
	unsigned long rtt;
	assert(sd);
	ensure(jiffies >= time, return);
	/* RFC 2960 6.3.1 */
	rtt = jiffies - time;
	if (sd->srtt) {
		unsigned long rttvar = sd->srtt > rtt ? sd->srtt - rtt : rtt - sd->srtt;
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
		sd->rto = rtt + (sd->rttvar << 2);
	} else {
		/* RFC 2960 6.3.1 (C2) */
		sd->rttvar = rtt >> 1;
		sd->srtt = rtt;
		sd->rto = rtt + (rtt << 1);
	}
	/* RFC 2960 6.3.1 (G1) */
	sd->rttvar = sd->rttvar ? sd->rttvar : 1;
	/* RFC 2960 6.3.1 (C6) */
	sd->rto = sd->rto_min > sd->rto ? sd->rto_min : sd->rto;
	/* RFC 2960 6.3.1 (C7) */
	sd->rto = sd->rto_max < sd->rto ? sd->rto_max : sd->rto;
#if defined SCTP_CONFIG_DEBUG && defined SCTP_CONFIG_ERROR_GENERATOR
	if (sd->retransmits && (sd->sp->options & SCTP_OPTION_BREAK)
	    && (sd->packets > BREAK_GENERATOR_LEVEL))
		ptrace(("Aaaarg! Reseting counts for address %d.%d.%d.%d\n",
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff, (sd->daddr >> 16) & 0xff,
			(sd->daddr >> 24) & 0xff));
#endif
	sd->dups = 0;
	/* RFC 2960 8.2 */
	sd->retransmits = 0;
	/* RFC 2960 8.1 */
	sd->sp->retransmits = 0;
	/* reset idle timer */
	sctp_reset_idle(sd);
}

#ifndef SCTP_DESTF_DROPPING
#define SCTP_DESTF_DROPPING 0x10000000	/* destination is dropping packets */
#endif
/* 
 *  DEST CALC
 *  -------------------------------------------------------------------------
 *  This function performs delayed processing of RTT and CWND calculations on
 *  destinations which need it and is called from sctp_recv_shutdown and
 *  sctp_recv_sack once all ack and gap ack processing is complete.  This
 *  performs the necessary calculations for each destination before closing
 *  processing of the received SHUTDOWN or SACK chunk.
 */
STATIC void
sctp_dest_calc(sp)
	sctp_t *sp;
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
					sd->cwnd += accum < sd->mtu ? accum : sd->mtu;
			} else {
				/* RFC 2960 7.2.2 */
#ifdef SCTP_IG_UPDATES
				sd->partial_ack += accum;
				if (sd->in_flight >= sd->cwnd && sd->partial_ack >= sd->cwnd) {
					sd->cwnd += sd->mtu;
					sd->partial_ack -= sd->cwnd;
				}
#else				/* SCTP_IG_UPDATES */
				if (sd->in_flight > sd->cwnd)
					sd->cwnd += sd->mtu;
#endif				/* SCTP_IG_UPDATES */
			}
			/* credit of destination (accum) */
			normal(sd->in_flight >= accum);
			sd->in_flight = sd->in_flight > accum ? sd->in_flight - accum : 0;
			/* RFC 2960 6.3.2 (R3) */
			if (sd->timer_retrans)
				untimeout(xchg(&sd->timer_retrans, 0));
			sd->ack_accum = 0;
		}
		if (sd->flags & SCTP_DESTF_DROPPING) {
			/* RFC 2960 7.2.4 (2), 7.2.3 */
			sd->ssthresh = sd->cwnd >> 1 > sd->mtu << 1 ? sd->cwnd >> 1 : sd->mtu << 1;
			sd->cwnd = sd->ssthresh;
#ifdef SCTP_IG_UPDATES
			/* SCTP IG Section 2.9 */
			sd->partial_ack = 0;
#endif				/* SCTP_IG_UPDATES */
			sd->flags &= ~SCTP_DESTF_DROPPING;
		}
		/* RFC 2960 6.3.2 (R2) */
		if (!sd->in_flight) {
			if (sd->timer_retrans)
				untimeout(xchg(&sd->timer_retrans, 0));
#ifdef SCTP_CONFIG_ADD_IP
			if (sd->flags & SCTP_DESTF_UNUSABLE)
				sctp_del_daddr(sd);
#endif				/* SCTP_CONFIG_ADD_IP */
		} else if (!sd->timer_retrans)
			set_timeout(sp, &sd->timer_retrans, &sctp_retrans_timeout, sd, sd->rto);
	}
}

/* 
 *  CUMM ACK
 *  -------------------------------------------------------------------------
 *  This function is responsible for moving the cummulative ack point.  The
 *  sender must check that the ack is valid (monotonically increasing), but it
 *  may be the same TSN as was previously acknowledged.  When the ack point
 *  advances, this function stikes DATA chunks from the retransmit buffer.  This
 *  also indirectly updates the amount of data outstanding for retransmission.
 *  This function is called by both sctp_recv_sack and sctp_recv_shutdown.
 *
 *  We only perform calculations on TSNs that were not previously acknowledged
 *  by a GAP Ack.  If the TSN has not been retransmitted (Karn's algorithm RFC
 *  2960 6.3.1 (C5)), the destination that it was set to is marked for RTT
 *  calculation update.  If the TSN is currently marked for retransmission
 *  awaiting available cwnd, it is unmarked and retranmission of the TSN is
 *  aborted.  If the TSN is not marked for retransmission, the destination's
 *  acked bytes accumulator is increased and the need for a CWND calculation
 *  for the destination indicated.  The association number of bytes in flight
 *  is decreased to account for the acknowledged TSN.  If any calculations are
 *  pending as a result of the ack, the function returns non-zero to
 *  indication that calculations (RTT and CWND) must be processed before
 *  message processing is complete.
 */
STATIC void
sctp_cumm_ack(sp, ack)
	sctp_t *sp;
	uint32_t ack;
{
	mblk_t *mp;
	assert(sp);
	/* make sure we actually move the ack point */
	if (after(ack, sp->t_ack))
		goto done;
	sp->t_ack = ack;
	while ((mp = bufq_head(&sp->rtxq)) && !after(SCTP_TCB(mp)->tsn, ack)) {
		struct sctp_tcb *cb = SCTP_TCB(mp);
		if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
			struct sctp_daddr *sd = cb->daddr;
			/* RFC 2960 6.3.1 (C5) */
			if (cb->trans < 2) {
				/* remember latest transmitted packet acked for rtt calc */
				sd->when = sd->when > cb->when ? sd->when : cb->when;
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
					    sd->in_flight >
					    sd->ack_accum + dlen ? sd->ack_accum +
					    dlen : sd->in_flight;
				}
				/* credit association (now) */
				normal(sp->in_flight >= dlen);
				sp->in_flight = sp->in_flight > dlen ? sp->in_flight - dlen : 0;
			}
		} else {
			/* can't reneg cummulatively acked chunks */
			cb->flags &= ~SCTPCB_FLAG_SACKED;
			ensure(sp->nsack, sp->nsack = 1);
			sp->nsack--;
		}
		if (cb->flags & SCTPCB_FLAG_CONF && cb->flags & SCTPCB_FLAG_LAST_FRAG
		    && sp->ops->sctp_datack_ind)
			bufq_queue(&sp->ackq, bufq_dequeue(&sp->rtxq));
		else
			freemsg(bufq_dequeue(&sp->rtxq));
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
		struct sctp_tcb *cb = SCTP_TCB(mp);
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
		if (!sp->timer_sack)
			set_timeout(sp, &sp->timer_sack, &sctp_sack_timeout, sp, sp->max_sack);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			if (sp->timer_sack)
				untimeout(xchg(&sp->timer_sack, 0));
	}
#endif
      done:
	return;
}

/* 
 *  ACK CALC
 *  -------------------------------------------------------------------------
 *  This one is for messages for which a timer is set an retransmission occurs
 *  until acknowledged.  We stop the timer, perform an RTT calculation if the
 *  message was not retransmitted, free the retry buffer, and clear the
 *  association and destination retransmission counts.
 */
STATIC void
sctp_ack_calc(sp, tp)
	sctp_t *sp;			/* private structure for stream */
	tid_t *tp;			/* timer to cancel */
{
	struct sctp_tcb *cb;
	struct sctp_daddr *sd;
	assert(sp);
	assert(sp->retry);
	cb = SCTP_TCB(sp->retry);
	sd = cb->daddr;
	untimeout(xchg(tp, 0));
	if (sd && cb->trans < 2)
		sctp_rtt_calc(sd, cb->when);
	freechunks(xchg(&sp->retry, NULL));
}

/* 
 *  DELIVER DATA
 *  -------------------------------------------------------------------------
 *  There is reason to belive that there is data waiting in gaps that can be
 *  delivered.  This function delivers whatever data is possible and pushes the
 *  necessary ack points.
 */
STATIC void
sctp_deliver_data(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_tcb *cb, *cb_next = sp->gaps;
	while ((cb = cb_next)) {
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
			if (after(cb->tsn, sp->p_fsn + 1)) {
#else
			if (after(cb->tsn, sp->r_ack + 1)) {
#endif
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
			if ((db = dupb(cb->mp))) {
				db->b_cont = cb->mp->b_cont;
				cb->mp->b_cont = NULL;
				bufq_queue(&sp->rcvq, db);
				cb->flags |= SCTPCB_FLAG_DELIV;
				if (ord) {
					if (!more) {
						st->n.more &= ~SCTP_STRM_MORE;
						st->ssn = cb->ssn;
					} else
						st->n.more |= SCTP_STRMF_MORE;
				} else {
					if (!more) {
						st->x.more &= ~SCTP_STRM_MORE;
					} else
						st->x.more |= SCTP_STRMF_MORE;
				}
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
#endif
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
			   message control block.  For now, intermediate message will simply
			   disappear (even if they are in the middle of fragmented messages.  For
			   STREAMS we might send a zero-length data message to the stream head. */
			sp->r_ack = cb->tsn;
			sctp_send_sack(sp);
		}
#endif
		if ((cb->flags & SCTPCB_FLAG_DELIV) && !after(cb->tsn, sp->r_ack)) {
			assure(sp->gaps == cb);
			if (!(sp->gaps = cb->next))
				sp->ngaps = 0;
			else {
				if (cb == cb->tail)
					sp->ngaps--;
				else {
					cb->next->tail = cb->tail;
					cb->tail->head = cb->next;	/* XXX */
				}
			}
			freemsg(bufq_unlink(&sp->oooq, cb->mp));
		}
	}
}

/* 
 *  RECV DATA
 *  -------------------------------------------------------------------------
 *  We have received a DATA chunk in a T-Provider s_state where is it valid to
 *  receive DATA (TS_DATA_XFER and TS_WIND_ORDREL).  We can also receive data
 *  chunks in TS_IDLE or TS_WRES_CIND on a listening socket bundled with a
 *  COOKIE ECHO.  In any other states we discard the data.  Because no other
 *  chunks can be bundled after a DATA chunk (just yet), we process all the DATA
 *  chunks in the remainder of the message in a single loop here.
 */
STATIC int
sctp_recv_data(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	int blen;
	size_t plen;
#ifdef SCTP_CONFIG_ECN
	int is_ce = INET_ECN_is_ce(((struct iphdr *) (mp->b_datap->db_base))->tos);
#endif
	uint newd;			/* number of new data chunks */
	assert(sp);
	assert(mp);
	if ((1 << sp->s_state) & ~(SCTPF_RECEIVING))
		goto outstate;
	for (newd = 0; (blen = mp->b_wptr - mp->b_rptr); mp->b_rptr += plen) {
		mblk_t *dp, *db;
		struct sctp_tcb *cb;
		struct sctp_strm *st;
		struct sctp_tcb **gap;
		struct sctp_data *m = (struct sctp_data *) mp->b_rptr;
		size_t clen = ntohs(m->ch.len);
		size_t dlen = clen > sizeof(*m) ? clen - sizeof(*m) : 0;
		uint32_t tsn = ntohl(m->tsn);
		uint16_t sid = ntohs(m->sid);
		uint16_t ssn = ntohs(m->ssn);
		uint32_t ppi = ntohl(m->ppi);
		uint flags = (m->ch.flags);
		int ord = !(flags & SCTPCB_FLAG_URG);
		int more = !(flags & SCTPCB_FLAG_LAST_FRAG);
		err = 0;
		plen = PADC(clen);
		if (blen <= 0 || blen < sizeof(*m) || clen < sizeof(*m) || blen < plen)
			goto emsgsize;
		if (m->ch.type != SCTP_CTYPE_DATA)
			goto eproto;
		if (dlen <= 0)
			goto baddata;
		if (sid >= sp->n_istr)
			goto badsid;
		if (sp->a_rwnd <=
		    bufq_size(&sp->oooq) + bufq_size(&sp->dupq) + bufq_size(&sp->rcvq))
			goto ebusy;
		if (!(st = sctp_istrm_find(sp, sid, &err)))
			goto enomem;
		if (!(dp = dupb(mp)))
			goto enobufs;
		/* trim copy to data only */
		dp->b_wptr = dp->b_rptr + clen;
		dp->b_rptr += sizeof(*m);
		/* fast path, nothing backed up */
		if (tsn == sp->r_ack + 1 && !bufq_head(&sp->rcvq) && !bufq_head(&sp->oooq)) {
			/* we have next expected TSN, just process it */
			if ((err = sctp_data_ind(sp, ppi, sid, ssn, tsn, ord, more, dp)))
				goto free_error;
			if (ord) {
				if (!more) {
					st->n.more &= ~SCTP_STRMF_MORE;
					st->ssn = ssn;
				} else
					st->n.more |= SCTP_STRMF_MORE;
			} else {
				if (!more) {
					st->x.more &= ~SCTP_STRMF_MORE;
				} else
					st->x.more |= SCTP_STRMF_MORE;
			}
			sp->r_ack++;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			if (before(sp->p_fsn, sp->r_ack))
				sp->p_fsn = sp->r_ack;
#endif
#ifdef SCTP_CONFIG_ECN
			if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
				sp->l_lsn = tsn;
				sp->sackf |= SCTP_SACKF_ECN;
			}
#endif
			newd++;
			continue;
		}
		if (!(db = allocb(sizeof(*cb), BPRI_MED)))
			goto free_nobufs;
		gap = &sp->gaps;
		cb = (struct sctp_tcb *) db->b_rptr;
		db->b_datap->db_type = M_CTL;
		bzero(db->b_wptr, sizeof(*cb));
		db->b_cont = dp;
		cb->dlen = dlen;
		cb->when = jiffies;
		cb->tsn = tsn;
		cb->sid = sid;
		cb->ssn = ssn;
		cb->ppi = ppi;
		cb->flags = flags & 0x7;
		cb->daddr = sp->caddr;
		cb->mp = db;
		cb->st = st;
		cb->tail = cb;
		cb->head = cb;
		usual(sp->caddr);
		if (!after(tsn, sp->r_ack))
			goto sctp_recv_data_duplicate;
		for (; (*gap); gap = &((*gap)->tail->next)) {
			if (between(tsn, (*gap)->tsn, (*gap)->tail->tsn))
				goto sctp_recv_data_duplicate;
			if (before(tsn, (*gap)->tsn)) {
				/* insert in front of gap */
				bufq_insert(&sp->oooq, (*gap)->mp, db);
				cb->next = (*gap);
				(*gap) = cb;
				sp->ngaps++;
			} else if (tsn == (*gap)->tail->tsn + 1) {
				/* expand at end of gap */
				bufq_queue(&sp->oooq, db);
				cb->next = (*gap)->tail->next;
				(*gap)->tail->next = cb;
				cb->head = (*gap);
				(*gap)->tail = cb;
			} else
				/* try next gap */
				continue;
			if (cb->next && cb->next->tsn == tsn + 1) {
				/* join two gaps */
				cb->next->tail->head = (*gap);
				(*gap)->tail = cb->next->tail;
				usual(sp->ngaps);
				sp->ngaps--;
			}
			break;
		}
		if (!(*gap)) {
			/* append to list */
			bufq_queue(&sp->oooq, db);
			cb->next = (*gap);
			(*gap) = cb;
			sp->ngaps++;
		}
		sp->nunds++;	/* more undelivered data */
#ifdef SCTP_CONFIG_ECN
		if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
			sl->l_tsn = tsn;
			sp->sackf |= SCTP_SACKF_ECN;
		}
#endif
		newd++;
		continue;
	      sctp_recv_data_duplicate:
		/* message is a duplicate tsn */
		bufq_queue(&sp->dupq, db);
		cb->next = sp->dups;
		sp->dups = cb;
		sp->ndups++;
		continue;
	      free_error:
		freemsg(dp);
		rare();
		break;
	      free_nobufs:
		freemsg(dp);
		rare();
		ptrace(("ERROR: could not allocate buffer\n"));
		err = -ENOBUFS;
		break;		/* couldn't allocate buffer */
	      enobufs:
		rare();
		ptrace(("ERROR: could not allocate buffer\n"));
		err = -ENOBUFS;
		break;		/* couldn't allocate buffer */
	      enomem:
		rare();
		ptrace(("ERROR: could not allocate stream\n"));
		err = -ENOMEM;
		break;		/* couldn't allocate stream */
	      ebusy:
		rare();
		/* Protect from teardrop attack without renegging */
		if (bufq_size(&sp->rcvq)) {
			/* TODO: need to put renegging code in here: renegging should first walk
			   the gap queue looking for any data that we have gap acked which has not
			   yet been delivered to the user.  Any such data can be deleted. Once
			   deleting this data we can check if we can process the current message
			   anyway.  If that is true, we can mark that a sack is required
			   immediately and jump back up to thw top where we entered flow control.
			   If we are still flow controlled, we will need to abort the association.
			   For now we just abort the association. */
			err = -ECONNRESET;
			sctp_send_abort(sk);
			sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, err, NULL);
			sctp_disconnect(sp);
			break;
		}
		/* If we have data that the user has not read yet, then we keep all our gaps,
		   because the user reading data will make some room to process things later.  We
		   must send an immediate SACK regardless, per the IG requiremeents. */
		ptrace(("ERROR: flow controlled (discard)\n"));
		err = -EBUSY;
		break;		/* flow controlled (discard) */
	      badsid:
		rare();
		ptrace(("PROTO: invalid stream id\n"));
		sctp_send_error(sp, SCTP_CAUSE_INVALID_STR, &m->sid, sizeof(m->sid));
		continue;	/* just skip that DATA chunk */
	      baddata:
		rare();
		/* RFC 2960 6.2: ...no user data... */
		if ((err = sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, SCTP_CAUSE_NO_DATA, NULL)))
			break;
		sctp_send_abort_error(sp, SCTP_CAUSE_NO_DATA, &m->tsn, sizeof(m->tsn));
		sctp_disconnect(sp);
		return (-EPROTO);
	      eproto:
		rare();
		ptrace(("PROTO: non-data chunk after data chunk\n"));
		err = -EPROTO;
		break;		/* non-data chunk after data */
	      emsgsize:
		rare();
		ptrace(("PROTO: bad message or chunk size\n"));
		err = -EMSGSIZE;
		break;		/* bad message or chunk sizes */
	}
	if (!newd)
		goto no_new_data;
	/* we have underlivered data and new data */
	sctp_deliver_data(sp, mp);
      no_new_data:
	/* RFC 2960 6.2 */
	/* 
	 * IMPLEMENTATION NOTE:- If we are receiving duplicates the probability is high that our
	 * SACKs aren't getting through (or have been delayed too long).  If we do not have a sack
	 * pending (one being delayed) then we will peg the duplicates against the destination. This 
	 * will change where we are sending SACKs.
	 *
	 * Because we are being cruel to this destination and we don't really know that this is
	 * where the offending SACKs were sent, we send an immediate heartbeat if there is no data
	 * is in flight to the destination (i.e., no retransmission timer running for the
	 * destination).  This fixes some sticky problems when one-way data is being sent.
	 */
	if (sp->ndups) {
		struct sctp_daddr *sd = sp->taddr;
		sp->sackf |= SCTP_SACKF_DUP;
		if (sd && !(sp->sackf & SCTP_SACKF_NEW)) {
			sd->dups += sp->ndups;
			if (!sd->in_flight && !sd->timer_heartbeat)
				if (sd->timer_idle)
					untimeout(xchg(&sd->timer_idle, 0));
				sctp_send_heartbeat(sp, sd);
			}
		}
	}
	/* RFC 2960 7.2.4 */
	if (sp->ngaps) {
		sp->sackf |= SCTP_SACKF_GAP;
	}
	/* RFC 2960 6.2 */
	/* 
	 * IMPLEMENTATION NOTE:- The SACK timer is probably too slow.  For unidirectional
	 * operation, the sender may have timed out before we send a sack.  We should really not
	 * wait any longer than some fraction of the RTO for the destination from which we are
	 * receiving (or sending) data. However, if we wait too long we will just get a
	 * retransmission and a dup.
	 *
	 * If the sack delay is set to zero, we do not set the timer, but issue the sack
	 * immediately.
	 */
	if (newd) {
		if (sp->max_sack) {
			sp->sackf += ((sp->sackf & 0x3) < 3) ? SCTP_SACKF_NEW : 0;
			if (!sp->timer_sack)
				set_timeout(sp, &sp->timer_sack, &sctp_sack_timeout, sp,
					    sp->max_sack);
		} else
			sp->sackf |= SCTP_SACKF_TIM;
	}
	if (sp->s_state == SCTP_SHUTDOWN_SENT)
		sctp_send_shutdown(sp);
	goto done;
      outstate:
	/* We have received DATA in the wrong state.  If so, it is probably an old packet that was
	   stuck in the network and just got delivered to us.  Nevertheless we should just ignore
	   any message containing DATA when we are not expecting it.  The only exception to this
	   might be receiving DATA in the COOKIE-WAIT s_state.  There should not be data hanging
	   around in the network that matches our tags.  If that is the case, we should abandon the 
	   connection attempt and let the user try again with a different verification tag. */
	rare();
	if (sp->s_state == SCTP_COOKIE_WAIT) {
		rare();
		if (!(err = sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, -EPROTO, NULL)))
			sctp_disconnect(sp);
		err = -EPROTO;
	}
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
}

/* 
 *  CLEANUP READ
 *  -------------------------------------------------------------------------
 *  This is called to clean up the read queue by the STREAMS read service
 *  routine.  This permits backenabling to work.
 */
static void
sctp_cleanup_read(sp)
	sctp_t *sp;
{
	mblk_t *mp;
	assert(sp);
	while ((mp = bufq_head(&sp->ackq))) {
		struct sctp_tcb *cb = SCTP_TCB(mp);
		if (!sctp_datack_ind(sp, cb->ppi, cb->sid, cb->ssn, cb->tsn)) {
			mp = bufq_dequeue(&sp->ackq);
			freemsg(mp);
			continue;
		}
		seldom();
		break;		/* error on delivery (ENOBUFS, EBUSY) */
	}
	if (bufq_head(&sp->rcvq)) {
		int need_sack = (sp->a_rwnd <= bufq_size(&sp->oooq)
				 + bufq_size(&sp->dupq)
				 + bufq_size(&sp->rcvq));
		while ((mp = bufq_head(&sp->rcvq))) {
			struct sctp_tcb *cb = SCTP_TCB(mp);
			struct sctp_strm *st = cb->st;
			int ord = !(cb->flags & SCTPCB_FLAG_URG);
			int more = !(cb->flags & SCTPCB_FLAG_LAST_FRAG);
			ensure(st, return);
			if (!sctp_data_ind
			    (sp, cb->ppi, cb->sid, cb->ssn, cb->tsn, ord, more, mp->b_cont)) {
				mp = bufq_dequeue(&sp->rcvq);
				mp->b_cont = NULL;
				freemsg(mp);
				if (ord) {
					if (!(st->n.more = more))
						st->ssn = cb->ssn;
				} else
					st->x.more = more;
				if (!need_sack)
					continue;
				/* Should really do SWS here.  */
				sp->sackf |= SCTP_SACKF_NOD;
				need_sack = 0;
				continue;
			}
			seldom();
			break;	/* error on delivery (ENOBUFS, EBUSY) */
		}
	}
}

/* 
 *  RECV SACK
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_sack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	mblk_t *dp;
	struct sctp_sack *m;
	size_t ngaps, ndups, mlen, clen, plen;
	uint32_t ack, rwnd;
	uint16_t *gaps;
	assert(sp);
	assert(mp);
	if ((1 << sp->s_state) & ~(SCTPF_SENDING))
		goto outstate;
	m = (struct sctp_sack *) mp->b_rptr;
	ack = htonl(m->c_tsn);
	rwnd = htonl(m->a_rwnd);
	ngaps = htons(m->ngaps);
	ndups = htons(m->ndups);
	gaps = m->gaps;
	clen = htons(m->ch.len);
	plen = PADC(clen);
	mlen = sizeof(*m) + (ndups + ngaps) * sizeof(uint32_t);
	if (clen < mlen || mp->b_wptr < mp->b_rptr + plen)
		goto emsgsize;
	/* RFC 2960 6.2.1 (D) i) */
	if (before(ack, sp->t_ack)) {
		rare();
		return 0;
	}
	/* If the receive window is increasing and we have data in the write queue, we might need
	   to backenable. */
	if (rwnd > sp->p_rwnd && sp->wq->q_count)
		qenable(sp->wq);
	/* RFC 1122 4.2.3.4 IMPLEMENTATION (3) */
	sp->p_rbuf = sp->p_rbuf >= rwnd ? sp->p_rbuf : rwnd;
	/* RFC 2960 6.2.1 (D) ii) */
	sp->p_rwnd = rwnd;	/* we keep in_flight separate from a_rwnd */
	/* advance the cummulative ack point and check need to perform per-round-trip and cwnd
	   calcs */
	sctp_cumm_ack(sp, ack);
	if (ndups) {
		// sp->sackf |= SCTP_SACKF_DUP;
		/* TODO: we could look through the list of duplicate TSNs.  Duplicate TSNs really
		   means that the peer's SACKs aren't getting back to us.  But there is nothing
		   really that we can do about that.  The peer has already detected the problem and 
		   should sent SACKs via an alternative route if possible.  But that's how this
		   SACK got here... */
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
		for (dp = bufq_head(&sp->rtxq); dpk dp = dp->b_next) {
			sctp_tcp_t *cb = SCTP_TCB(dp);
			/* RFC 2960 6.3.2 (R4) reneg */
			if (cb->flags & SCTPCB_FLAG_SACKED) {
				struct sctp_daddr *sd = cb->daddr;
				cb->flags &= ~SCTPCB_FLAG_SACKED;
				ensure(sp->nsack, sp->nsack = 1);
				sp->nsack--;
				if (sd && !sd->timer_retrans) {
					set_timeout(sp, &sd->timer_retrans, &sctp_retrans_timeout,
						    sd, sd->rto);
				} else
					seldom();
			}
		}
	} else {
		int growth = 0;
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
			dp = bufq_head(&sp->rtxq);
			for (; dp && before(SCTP_TCB(dp)->tsn, beg); dp = dp->b_next)
				SCTP_TCB(dp)->flags |= SCTPCB_FLAG_NACK;
			/* sack the acks */
			for (; dp && !after(SCTP_TCB(dp)->tsn, end); dp = dp->b_next)
				SCTP_TCB(dp)->flags |= SCTPCB_FLAG_ACK;
		}
		/* walk the whole retrans buffer looking for holes and renegs */
		for (dp = bufq_head(&sp->rtxq); dp; dp = dp->b_next) {
			struct sctp_tcb *cb = SCTP_TCB(dp);
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			if ((cb->flags & SCTPCB_FLAG_DROPPED)) {
				cb->flags &= ~SCTPCB_FLAG_ACK;
				cb->flags &= ~SCTPCB_FLAG_NACK;
				continue;
			}
#endif
			/* msg is inside gapack block */
			if (cb->flags & SCTPCB_FLAG_ACK) {
				cb->flags &= ~SCTPCB_FLAG_ACK;
				if (cb->flags & SCTPCB_FLAG_NACK) {
					cb->flags &= ~SCTPCB_FLAG_NACK;
					if (cb->sacks < SCTP_FR_COUNT)
						growth = 1;
				}
				if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
					struct sctp_daddr *sd = cb->daddr;
					cb->flags |= SCTPCB_FLAG_SACKED;
					sp->nsack++;
					/* RFC 2960 6.3.1 (C5) */
					if (sd && cb->trans < 2) {
						/* remember latest transmitted packet acked for rtt 
						   calc */
						sd->when =
						    sd->when > cb->when ? sd->when : cb->when;
					}
					if (cb->flags & SCTPCB_FLAG_RETRANS) {
						cb->flags &= ~SCTPCB_FLAG_RETRANS;
						ensure(sp->nrtxs > 0, sp->nrtxs = 1);
						sp->nrtxs--;
					} else {
						size_t dlen = cb->dlen;
						/* credit destination */
						normal(sd->in_flight >= dlen);
						sd->in_flight =
						    sd->in_flight > dlen ? sd->in_flight - dlen : 0;
						/* credit association */
						normal(sp->in_flight >= dlen);
						sp->in_flight =
						    sp->in_flight > dlen ? sp->in_flight - dlen : 0;
					}
				}
				continue;
			}
			/* msg is between gapack blocks */
			if (cb->flags & SCTPCB_FLAG_NACK) {
				cb->flags &= ~SCTPCB_FLAG_NACK;
				/* RFC 2960 7.2.4 */
				if (!growth && !(cb->flags & SCTPCB_FLAG_RETRANS)
				    && ++(cb->sacks) == 4) {
					size_t dlen = cb->dlen;
					struct sctp_daddr *sd = cb->daddr;
					/* RFC 2960 7.2.4 (1) */
					cb->flags |= SCTPCB_FLAG_RETRANS;
					sp->nrtxs++;
					cb->sacks = 0;	/* ??? do we clear the sack count */
					if (sd) {
						/* RFC 2960 7.2.4 (2) */
						sd->flags |= SCTP_DESTF_DROPPING;
						/* credit destination (now) */
						normal(sd->in_flight >= dlen);
						sd->in_flight =
						    sd->in_flight > dlen ? sd->in_flight - dlen : 0;
					}
					/* credit association (now) */
					normal(sp->in_flight >= dlen);
					sp->in_flight =
					    sp->in_flight > dlen ? sp->in_flight - dlen : 0;
				}
				/* RFC 2960 6.3.2 (R4) (reneg) */
				/* reneg */
				if (cb->flags & SCTPCB_FLAG_SACKED) {
					struct sctp_daddr *sd = cb->daddr;
					cb->flags &= ~SCTPCB_FLAG_SACKED;
					ensure(sp->nsack, sp->nsack = 1);
					sp->nsack--;
					if (!sd->timer_retrans) {
						set_timeout(sp, &sd->timer_retrans,
							    &sctp_retrans_timeout, sd, sd->rto);
					} else
						seldom();
				}
				continue;
			}
			/* msg is after all gapack blocks */
			break;
		}
	}
      skip_rtx_analysis:
	sctp_dest_calc(sp);
	/* After receiving a cummulative ack, I want to check if the sndq and rtxq is empty and a
	   SHUTDOWN or SHUTDOWN-ACK is pending.  If so, I want to issue these primitives. */
	if (((1 << sp->
	      s_state) & (SCTPF_SHUTDOWN_PENDING | SCTPF_SHUTDOWN_RECEIVED |
			  SCTPF_SHUTDOWN_RECVWAIT))
	    && !bufq_head(&sp->sndq)
	    && !bufq_head(&sp->rtxq)) {
		seldom();
		switch (sp->s_state) {
		case SCTP_SHUTDOWN_PENDING:
			/* Send the SHUTDOWN I didn't send before. */
			sctp_send_shutdown(sp);
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			/* Send the SHUTDOWN-ACK I didn't send before */
			if (!sp->ops->sctp_ordrel_ind)
				sctp_send_shutdown_ack(sp);
			break;
		case SCTP_SHUTDOWN_RECVWAIT:
			/* Send the SHUTDOWN-ACK I didn't send before */
			sctp_send_shutdown_ack(sp);
			break;
		default:
			never();
			return (-EFAULT);
		}
	}
      outstate:
	/* We may have received a SACK in the wrong state.  Because SACKs are completely advisory,
	   there is no reason to get too upset about this. Simply ignore them.  No need to process
	   them. */
	{
		int ret;
		if ((ret = sctp_return_more(mp)) > 0) {
			struct sctpchdr *ch = (struct sctpchdr *) mp->b_rptr;
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
      emsgsize:
	return (-EMSGSIZE);
}

/* 
 *  RECV ERROR
 *  -------------------------------------------------------------------------
 *  We have received an ERROR chunk in opening, connected or closing states.
 */
STATIC int
sctp_recv_error(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	struct sctp_error *m;
	struct sctpehdr *eh;
	int ecode;
	assert(sp);
	assert(mp);
	seldom();
	m = (struct sctp_error *) mp->b_rptr;
	eh = (struct sctpehdr *) (m + 1);
	ecode = ntohs(eh->code);
	switch (ecode) {
	case SCTP_CAUSE_STALE_COOKIE:
		if (sp->s_state == SCTP_COOKIE_ECHOED) {
			struct sctp_tcb *cb;
			struct sctp_daddr *sd;
			assert(sp->retry);
			cb = SCTP_TCB(sp->retry);
			sd = cb->daddr;
			assert(sd);
			seldom();
			/* We can try again with cookie preservative, and then we can keep trying
			   until we have tried as many times as we can... */
			if (!sp->ck_inc) {
				int err;
				rare();
				sp->ck_inc = sp->ck_inc + (sd->rto >> 1);
				sctp_ack_calc(sp, &sp->timer_init);
				if ((err = sctp_send_init(sp))) {
					rare();
					return (err);
				}
				return sctp_return_stop(mp);
			}
			/* RFC 2960 5.2.6 (1) */
			if (cb->trans < sp->max_inits) {
				if (sp->timer_init)
					untimeout(xchg(&sp->timer_init, 0));
				/* RFC 2960 5.2.6 (3) */
				if (cb->trans < 2)
					sctp_rtt_calc(sd, cb->when);
				usual(sp->retry);
				sctp_send_msg(sp, sd, sp->retry);
				return sctp_return_stop(mp);
			}
			/* RFC 2960 5.2.6 (2) */
			goto recv_error_error;
		}
		break;
	case SCTP_CAUSE_INVALID_PARM:
	case SCTP_CAUSE_BAD_ADDRESS:
		/* If the sender of the ERROR has already given us a valid INIT-ACK then we can
		   ignore these errors. */
		if (sp->s_state == SCTP_COOKIE_ECHOED)
			break;
		seldom();
	case SCTP_CAUSE_MISSING_PARM:
	case SCTP_CAUSE_NO_RESOURCE:
	case SCTP_CAUSE_INVALID_STR:
		/* These errors are bad.  If we don't get an abort with them then we must abort the 
		   association. */
	      recv_error_error:
		if ((err = sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, ecode, NULL)))
			return (err);
		sctp_send_abort(sp);
		sctp_disconnect(sp);
		return sctp_return_stop(mp);
	case SCTP_CAUSE_BAD_CHUNK_TYPE:
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_PARTIAL_RELIABILITY
	{
		switch (ch->type) {
#if defined SCTP_CONFIG_ADD_IP
		case SCTP_CTYPE_ASCONF:
		case SCTP_CTYPE_ASCONF_ACK:
			sctp_abort_asconf(sp);
			sp->p_caps &= ~SCTP_CAPS_ADD_IP;
			sp->p_caps &= ~SCTP_CAPS_SET_IP;
			break;
#endif
#if defined SCTP_CONFIG_PARTIAL_RELIABILITY
		case SCTP_CTYPE_FORWARD_TSN:
			/* This is actually worse than it looks: we were told that the other end
			   was PR capable. */
			sp->p_caps &= ~SCTP_CAPS_PR;
			break;
#endif
		}
		break;
	}
	case SCTP_CAUSE_BAD_PARM:
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_PARTIAL_RELIABILITY || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
	{
		struct sctpphdr *ph = (typeof(ph)) (eh + 1);
		switch (ph->type) {
#if defined SCTP_CONFIG_ADD_IP
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
#endif
#if defined SCTP_CONFIG_PARTIAL_RELIABLIITY
		case SCTP_PTYPE_PR_SCTP:
			/* This is actually worse than it looks: we were told that the other end
			   was PR capable. */
			sp->p_caps &= ~SCTP_CAPS_PR;
			break;
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
		case SCTP_PTYPE_ALI:
			sp->p_caps &= ~SCTP_CAPS_ALI;
			break;
#endif
#if defined SCTP_CONFIG_ECN
		case SCTP_PTYPE_ECN_CAPABLE:
			sp->p_caps &= ~SCTP_CAPS_ECN;
			INET_ECN_dontxmit(sp);
			break;
#endif
		}
		break;
	}
#endif
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
}

/* 
 *  RECV ABORT
 *  -------------------------------------------------------------------------
 *  We have received an ABORT chunk in opening, connected or closing states.
 *  If there is a user around we want to send a disconnect indication,
 *  otherwise we want to just go away.
 */
STATIC int
sctp_recv_abort_listening(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	mblk_t *cp;
	struct sctp_cookie_echo *ce;
	struct sctp_cookie *ck;
	struct iphdr *iph = (typeof(iph)) mp->b_datap->db_base;
	struct sctphdr *sh = (typeof(sh)) (mp->b_datap->db_base + (iph->ihl << 2));
	struct sctp_abort *m = (typeof(m)) mp->b_rptr;
	ulong orig = (m->ch.flags & 0x1) ? SCTP_ORIG_PROVIDER : SCTP_ORIG_USER;
	for (cp = bufq_head(&sp->conq); cp; cp = cp->b_next) {
		ce = (typeof(ce) cp->b_rptr);
		ck = (typeof(ck)) ce->cookie;
		if (ck->v_tag == sh->v_tag && ck->sport == sh->dest && ck->dport == sh->srce
		    && ck->saddr == iph->daddr && ck->daddr == iph->saddr) {
			printd(("INFO: Removing Connection Indication on ABORT\n"));
			if ((err = sctp_discon_ind(sp, orig, 0, cp)))	/* XXX reason */
				return (err);
			return sctp_return_stop(mp);
		}
	}
	usual(cp);
	return (-EPROTO);	/* discard it */
}
STATIC int
sctp_recv_abort(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	long reason;
	mblk_t *cp = NULL;
	struct sctp_abort *m = (typeof(m)) mp->b_rptr;
	ulong orig = (m->ch.flags & 0x1) ? SCTP_ORIG_PROVIDER : SCTP_ORIG_USER;
	switch (sp->s_state) {
	case SCTP_LISTEN:
		return sctp_recv_abort_listening(sp, mp);
	case SCTP_COOKIE_WAIT:
	case SCTP_COOKIE_ECHOED:
		reason = ECONNREFUSED;
		break;
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
	case SCTP_SHUTDOWN_ACK_SENT:
		reason = EPIPE;
		break;
	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
		reason = ECONNRESET;
		break;
	default:
		swerr();
		return sctp_return_stop(mp);
	}
	sctp_discon_ind(sp, orig, reason, cp);
	return sctp_return_stop(mp);
}

/* 
 *  RECV INIT (Listener only)
 *  -------------------------------------------------------------------------
 *  We have receive an INIT in the LISTEN s_state.  This is the normal path for
 *  the establishment of an SCTP association.  There can be no other stream
 *  bound to this local port but we can have accepted streams which share the
 *  same local binding.
 *
 *  INIT chunks cannot have other chunks bundled with them (RFC 2960 6.10).
 */
STATIC int
sctp_recv_init(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct iphdr *iph;
	struct sctphdr *sh;
	struct sctp_init *m;
	sctp_t *oldsp;
	union sctp_parm *ph;
	unsigned char *pptr, *pend;
	size_t plen;
	size_t anum = 0;
	ulong ck_inc = 0;
	int err = 0;
	size_t errl = 0;
	unsigned char *errp = NULL;
	struct sctp_cookie ck;
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	static ulong last_init = 0;
	if (last_init && jiffies < last_init + sp->throttle)
		goto ebusy;
	last_init = jiffies;
#endif
	assert(sp);
	assert(mp);
	iph = (struct iphdr *) mp->b_datap->db_base;
	sh = (struct sctphdr *) (mp->b_datap->db_base + (iph->ihl << 2));
	m = (struct sctp_init *) mp->b_rptr;
	pptr = (unsigned char *) (m + 1);
	pend = pptr + ntohs(m->ch.len) - sizeof(*m);
	if (mp->b_wptr < mp->b_rptr + sizeof(*m))
		goto emsgsize;
	/* SCTP IG 2.11 not allowed to bundle */
	if (mp->b_wptr > mp->b_rptr + PADC(m->ch.len) + sizeof(struct sctpchdr))
		goto eproto;
	/* RFC 2960 p.26 initiate tag zero */
	if (!m->i_tag)
		goto invalid_parm;
	for (ph = (union sctp_parm *) pptr;
	     pptr + sizeof(ph->ph) <= pend && pptr + (plen = ntohs(ph->ph.len)) <= pend;
	     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
		uint type;
		switch ((type = ph->ph.type)) {
		case SCTP_PTYPE_IPV6_ADDR:
		case SCTP_PTYPE_HOST_NAME:
			goto bad_address;
		case SCTP_PTYPE_COOKIE_PSRV:
			if (plen == sizeof(ph->cookie_prsv)) {
				ck_inc = ntohl(ph->cookie_prsv.ck_inc);
				break;
			}
			goto init_bad_parm;
		case SCTP_PTYPE_IPV4_ADDR:
			if (plen == sizeof(ph->ipv4_addr)) {
				/* skip primary */
				if (ph->ipv4_addr.addr != iph->saddr)
					anum++;
				break;
			}
			goto init_bad_parm;
		case SCTP_PTYPE_ADDR_TYPE:
			/* Ensure that address types supported includes IPv4.  Actually address
			   types must include IPv4 so we just ignore. */
			break;
#ifdef SCTP_CONFIG_ECN
		case SCTP_PTYPE_ECN_CAPABLE:
			sp->p_caps |= SCTP_CAPS_ECN;
			INET_ECN_xmit(sp);
			break;
#endif				/* SCTP_CONFIG_ECN */
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
		case SCTP_PTYPE_ALI:
			sp->p_ali = ntohl(ph->ali.ali);
			sp->p_caps |= SCTP_CAPS_ALI;
			break;
#endif				/* defined SCTP_CONFIG_ADD_IP || defined
				   SCTP_CONFIG_ADAPTATION_LAYER_INFO */
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
		case SCTP_PTYPE_PR_SCTP:
			sp->p_caps |= SCTP_CAPS_PR;
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
			if (type & SCTP_PTYPE_MASK_REPORT)
				sctp_send_error(sp, SCTP_CAUSE_BAD_PARM, pptr, plen);
			continue;
		}
	}
	/* put together cookie */
	{
		/* negotiate inbound and outbound streams */
		size_t istrs = sp->n_istr;
		size_t ostrs = sp->n_ostr;
		istrs = ((istrs && istrs <= ntohs(m->n_istr))) ? istrs : ntohs(m->n_istr);
		ostrs = ((ostrs && ostrs <= ntohs(m->n_ostr))) ? ostrs : ntohs(m->n_ostr);
		errl = 0;
		errp = NULL;
		err = -SCTP_CAUSE_INVALID_PARM;
		if ((m->n_istr && !istrs) | (m->n_ostr && !ostrs))
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
		ck.danum = anum;
	}
	/* SCTP IG 2.6 replacement: This IG section would have us do a hard lookup to return an
	   error.  We just lookup on IP/SCTP header information which means if a highjacker sends
	   an INIT with an extra address attempting to highjack a connection we will respond with
	   an INIT ACK but will not populate tie tages or initiate tag with existing tags.  This
	   way the tags of the existing association and information about the addresses for the
	   association are never exposed, yet we reconcile the situation properly when we receive
	   the COOKIE ECHO. */
	SCTPHASH_RLOCK();
	oldsp = sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr);
	SCTPHASH_RUNLOCK();
	if (oldsp && oldsp->s_state != SCTP_COOKIE_WAIT) {
		rare();
		/* RFC 2960 5.2.1 and 5.2.2 Note */
		ck.l_ttag = oldsp->v_tag;
		ck.p_ttag = oldsp->p_tag;
	} else {
		/* RFC 2960 5.2.2 Note */
		ck.l_ttag = 0;
		ck.p_ttag = 0;
	}
	ck.opt_len = 0;
	if (sp->s_state == SCTP_LISTEN && !(sp->userlocks & SCTP_BINDADDR_LOCK)) {
		/* always include destination address */
		if (!sctp_saddr_include(sp, ck.saddr, &err) && err)
			goto no_resource;
		/* build a temporary address list */
		__sctp_get_addrs(sp, iph->saddr);
	}
	if (sp->sanum < 1)
		goto no_resource;
	ck.sanum = sp->sanum - 1;	/* don't include primary */
	sctp_send_init_ack(sp, iph->daddr, iph->saddr, sh, &ck);
      cleanup:
	if (sp->s_state == SCTP_LISTEN && !(sp->userlocks & SCTP_BINDADDR_LOCK))
		__sctp_free_saddrs(sp);	/* remove temporary address list */
      discard:
	bufq_purge(&sp->errq);
	/* SCTP IG 2.2, 2.11 */
	return sctp_return_stop(mp);
      no_resource:
	err = -SCTP_CAUSE_NO_RESOURCE;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: missing initiate tag\n"));
	goto error;
      bad_address:
	err = -SCTP_CAUSE_BAD_ADDRESS;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: missing initiate tag\n"));
	goto error;
      invalid_parm:
	err = -SCTP_CAUSE_INVALID_PARM;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: missing initiate tag\n"));
	goto error;
      bad_parm:
	err = -SCTP_CAUSE_BAD_PARM;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: missing initiate tag\n"));
	goto error;
      error:
	sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, -err, errp, errl);
	goto cleanup;
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
      ebusy:
	err = -EBUSY;
	goto return_error;
#endif
      eproto:
	err = -EPROTO;
	goto return_error;
      emsgsize:
	err = -EMSGSIZE;
	goto return_error;
      return_error:
	return (err);
}

/* 
 *  RECV INIT ACK
 *  -------------------------------------------------------------------------
 *  We have recevied an INIT ACK in the SCTP_COOKIE_WAIT (TS_WCON_CREQ) s_state.
 *  (RFC 2960 5.2.3)
 */
STATIC int
sctp_recv_init_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	int reason;
	assert(sp);
	assert(mp);
	if (sp->s_state == SCTP_COOKIE_WAIT) {
		struct iphdr *iph = (typeof(iph)) mp->b_datap->db_base;
		struct sctp_init_ack *m = (typeof(m)) mp->b_rptr;
		if (sctp_daddr_include(sp, iph->saddr, &err)) {
			unsigned char *kptr = NULL;
			size_t klen = 0;
			unsigned char *pptr = (unsigned char *) (m + 1);
			unsigned char *pend = pptr + ntohs(m->ch.len) - sizeof(*m);
			size_t plen;
			struct sctpphdr *ph = (struct sctpphdr *) pptr;
			ensure((pend <= mp->b_wptr), return (-EMSGSIZE));
			{
				size_t ostr = htons(m->n_ostr);
				size_t istr = htons(m->n_istr);
				/* SCTP IG 2.5 */
				if (!ostr || !istr || istr > sp->max_istr) {
					err = -EPROTO;
					goto sctp_recv_init_ack_error;
				}
				/* SCTP IG 2.22 */
				if (!m->i_tag) {
					err = -EPROTO;
					goto sctp_recv_init_ack_error;
				}
				sp->p_tag = m->i_tag;
				sctp_phash_rehash(sp);
				sp->p_rwnd = ntohl(m->a_rwnd);
				sp->p_rbuf = sp->p_rbuf >= sp->p_rwnd ? sp->p_rbuf : sp->p_rwnd;
				/* SCTP IG 2.19 correct */
				sp->n_ostr = ostr;
				sp->n_istr = istr;
				sp->r_ack = ntohl(m->i_tsn) - 1;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
				sp->p_fsn = ntohl(m->i_tsn) - 1;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ADD_IP
				sp->p_asn = ntohl(m->i_tsn) - 1;
#endif				/* SCTP_CONFIG_ADD_IP */
			}
			sp->p_caps = 0;
			for (ph = (struct sctpphdr *) pptr;
			     pptr + sizeof(*ph) <= pend && pptr + (plen = ntohs(ph->len)) <= pend;
			     pptr += PADC(plen), ph = (struct sctpphdr *) pptr) {
				uint type;
				switch ((type = ph->type)) {
				case SCTP_PTYPE_IPV4_ADDR:
					if (!sctp_daddr_include(sp, *((uint32_t *) (ph + 1)), &err)) {
						rare();
						sctp_send_error(sp, SCTP_CAUSE_BAD_ADDRESS, ph,
								plen);
					}
					continue;
				case SCTP_PTYPE_STATE_COOKIE:
					kptr = pptr + sizeof(*ph);
					klen = plen - sizeof(*ph);
					continue;
				case SCTP_PTYPE_IPV6_ADDR:
				case SCTP_PTYPE_HOST_NAME:
					rare();
					sctp_send_error(sp, SCTP_CAUSE_BAD_ADDRESS, ph, plen);
					continue;
#ifdef SCTP_CONFIG_ECN
				case SCTP_PTYPE_ECN_CAPABLE:
					sp->p_caps |= SCTP_CAPS_ECN;
					INET_ECN_xmit(sp);
					break;
#endif
#if defined SCTP_CONFIG_ADD_IP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO
				case SCTP_PTYPE_ALI:
					sp->p_ali = ntohl(((struct sctp_ali *) ph)->ali);
					sp->p_caps |= SCTP_CAPS_ALI;
					break;
#endif
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
				case SCTP_PTYPE_PR_SCTP:
					sp->p_caps |= SCTP_CAPS_PR;
					break;
#endif
				case SCTP_PTYPE_UNREC_PARMS:
#if defined SCTP_CONFIG_ADDIP || defined SCTP_CONFIG_ADAPTATION_LAYER_INFO || defined SCTP_CONFIG_PARTIAL_RELIABILITY
					/* unrecognized parameters valid */
					break;
#endif
				default:
					rare();
					reason = SCTP_CAUSE_BAD_PARM;
					if (type & SCTP_PTYPE_MASK_REPORT)
						sctp_send_error(sp, reason, ph, plen);
					if (type & SCTP_PTYPE_MASK_CONTINUE)
						continue;
					/* SCTP IG 2.2 */
					return sctp_return_stop(mp);
				}
			}
			if (!kptr) {	/* no cookie? */
				struct {
					uint32_t num;
					uint16_t mparm;
				} es = {
				1, SCTP_PTYPE_STATE_COOKIE};
				rare();
				reason = SCTP_CAUSE_MISSING_PARM;
				sctp_send_error(sp, reason, &es, sizeof(es));
				return (-EPROTO);
			}
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			if (sp->prel == SCTP_PR_REQUIRED && !(sp->p_caps & SCTP_CAPS_PR)) {
				/* require partial reliabilty support */
				/* abort the init process */
				err = -EPROTO;
				goto sctp_recv_init_ack_error;
			}
#endif
			sctp_ack_calc(sp, &sp->timer_init);
			if ((err = sctp_send_cookie_echo(sp, kptr, klen))) {
				rare();
				return (err);
			}
			return sctp_return_stop(mp);
		}
		rare();
		return (err);	/* fall back on timer init */
	}
	rare();
	return (-EPROTO);
      sctp_recv_init_ack_error:
	rare();
	if ((err = sctp_discon_ind(sp, SCTP_ORIG_PROVIDER, err, NULL)))
		return (err);
	sctp_disconnect(sp);	/* we won't be back */
	return (err);
}

/* 
 *  RECV COOKIE ECHO
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ECHO for a STREAM.  We have already determined
 *  the STREAM to which the COOKIE ECHO applies.  We must still verify the
 *  cookie and apply the appropriate action per RFC 2960 5.2.4.
 */
STATIC int
sctp_update_from_cookie(sp, ck)
	sctp_t *sp;
	struct sctp_cookie *ck;
{
	int err;
	uint32_t *daddrs = (uint32_t *) (ck + 1);
	uint32_t *saddrs = daddrs + ck->danum;
	if (!(sp->userlocks & SCTP_BINDADDR_LOCK)) {
		if (!sctp_addr_include(sp, ck->saddr, &err) && err)
			goto error;
		if ((err = sctp_alloc_saddrs(sp, ck->sport, saddrs, ck->sanum)))
			goto error;
	}
	if (!sctp_daddr_include(sp, ck->daddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_daddrs(sp, ck->dport, daddrs, ck->danum)))
		goto error;
	if ((err = sctp_conn_hash(sp)))
		goto error;
	if ((err = sctp_update_routes(sp, 1)))
		goto error;
	sp->daddr = ck->daddr;
	sp->saddr = ck->saddr;
	sp->n_istr = ck->n_istr;
	sp->n_ostr = ck->n_ostr;
	sp->t_tsn = ck->v_tag;
	sp->t_ack = ck->v_tag - 1;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	sp->l_fsn = ck->v_tag;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ADD_IP
	sp->l_asn = ck->v_tag;
#endif				/* SCTP_CONFIG_ADD_IP */
	sp->m_tsn = ck->v_tag - 1;
	sp->r_ack = ck->p_tsn - 1;
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
	sp->p_fsn = ck->p_tsn - 1;
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */
#ifdef SCTP_CONFIG_ADD_IP
	sp->p_asn = ck->p_tsn - 1;
#endif				/* SCTP_CONFIG_ADD_IP */
	sp->p_rwnd = ck->p_rwnd;
	sp->p_rbuf = ck->p_rwnd;
	/* sp->a_rwnd = FIXME; */
	return (0);
      error:
	return (err);
}
STATIC int
sctp_recv_cookie_echo(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	struct iphdr *iph = (typeof(iph)) mp->b_datap->db_base;
	struct sctphdr *sh = (typeof(sh)) (mp->b_datap->db_base + (iph->ihl << 2));
	struct sctp_cookie_echo *m = (typeof(m)) mp->b_rptr;
	struct sctp_cookie *ck = ((typeof(ck))) m->cookie;
#ifdef SCTP_CONFIG_THROTTLE_PASSIVEOPENS
	static ulong last_cookie_echo = 0;
	if (last_cookie_echo && jiffies < last_cookie_echo + sp->throttle)
		return (-EBUSY);
	last_cookie_echo = jiffies;
#endif				/* SCTP_CONFIG_THROTTLE_PASSIVEOPENS */
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
			    || ((1 << sp->s_state) & (SCTPF_COOKIE_WAIT | SCTPF_COOKIE_ECHOED)))
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
		goto recv_cookie_echo_conn_ind;
	rare();
	/* RFC 2960 5.2.4 ...silently discarded */
	return (0);
      recv_cookie_echo_action_a:
	rare();
	/* 
	 *  RFC 2960 5.2.4 Action (A)
	 *
	 *  In this case, the peer may have restarted.  When the endpoint
	 *  recognizes this potential 'restart', the existing session is treated
	 *  the same as if it received an ABORT followed by a new COOKIED ECHO
	 *  with the following exceptions:  - Any SCTP DATA Chunks MAY be
	 *  retained (this is an implementation specific option).  - A
	 *  notification of RESTART SHOULD be sent to the ULP instead of
	 *  "COMMUNICATION LOST" notification.
	 *
	 *  All the Congestion control parameters (e.g., cwnd, ssthresh) related
	 *  to this peer MUST be reset to their initial values (see Section
	 *  6.2.1).  After this the endpoint shall enter the ESTABLISHED
	 *  s_state.
	 *
	 *  If the endpoint is in the SHUTDOWN-ACK-SENT state and recognizes the
	 *  peer has restarted (Action A), it MUST NOT stupe a new association
	 *  but instead resend the SHUTDOWN ACK and send an ERROR chunk with a
	 *  "Cookie Received while Shutting Down" error cause to its peer.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan) {
		uint32_t staleness;
		rare();
		stateness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	switch (sp->s_state) {
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
		/* XXX this is rather different from current sockets version */
		/* We trash all existing data in queue. */
		sctp_disconnect(sp);
		/* Notify user of reset or disconnect */
		fixme(("Need reason argument to reset ind\n"));
		if ((err = sctp_reset_ind(sp, SCTP_ORIG_USER, 0, mp)))	/* XXX reason */
			return (err);
		goto recv_cookie_echo_conn_ind;
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
	 *  In this case, both sides may be attempting to start and association
	 *  at about the same time but the peer endpoint started its INIT after
	 *  responding to the local endpoint's INIT.  Thus it may have picked a
	 *  new Verification Tag not being aware of the previous Tag it had sent
	 *  this endpoint.  The endpoint should stay in or enter the ESTABLISHED
	 *  s_state but it MUST update its peer's Verification Tag from the
	 *  State Cookie, stop any init or cookie timers that may be running and
	 *  send a COOKIE ACK.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan) {
		uint32_t staleness;
		rare();
		stateness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	switch (sp->s_state) {
	case SCTP_COOKIE_WAIT:
		if (sp->timer_init) {
			rare();
			untimeout(xchg(&sp->timer_init, 0));
		}
		/* We haven't got an INIT ACK yet so we need some stuff from the cookie */
		if ((err = sctp_update_from_cookie(sp, ck)))
			goto error_abort;
		if ((err = sctp_conn_con(sp)))
			return (err);
		break;
	case SCTP_COOKIE_ECHOED:
		if (sp->timer_cookie) {
			rare();
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		break;
	case SCTP_ESTABLISHED:
		break;
	default:
		rare();
		return (-EFAULT);
	}
	sp->p_tag = ck->p_tag;
	sctp_phash_rehash(sp);
	sctp_send_sack(sp);
	sctp_send_cookie_ack(sp);
	return sctp_return_more(mp);
      recv_cookie_echo_action_c:
	rare();
	/* 
	 *  RFC 2960 5.2.4 Action (C)
	 *
	 *  In this case, the local endpoint's cookie has arrived late.  Before
	 *  it arrived, the local sendpoint sent an INIT and receive an INIT-ACK
	 *  and finally sent a COOKIE ECHO with the peer's same tag but a new
	 *  tag of its own.  The cookie should be silently discarded.  The
	 *  endpoint SHOULD NOT change states and should leave any timers
	 *  running.
	 */
	rare();
	/* RFC 2960 5.2.4 (3) */
	if (jiffies - ck->timestamp > ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	return (0);
      recv_cookie_echo_action_d:
	/* 
	 *  RFC 2960 5.2.4 Action (D)
	 *
	 *  When both local and remote tags match the endpoint should always
	 *  enter the ESTABLISHED state, it if has not already done so.  It
	 *  should stop any init or cookie timers that may be running and send a
	 *  COOKIE ACK.
	 */
	/* SCTP IG 2.29 */
	switch (sp->s_state) {
	case SCTP_COOKIE_ECHOED:
		sp->s_state = SCTP_ESTABLISHED;
		if (sp->timer_cookie) {
			rare();
			untimeout(xchg(&sp->timer_cookie, 0));
		}
		if ((err = sctp_conn_con(sp)))
			return (err);
		sctp_send_sack(sp);
		sctp_send_cookie_ack(sp);
		return sctp_return_more(mp);
	default:
		rare();
		/* RFC 2960 5.2.4 ...silently discarded */
		return (0);
	}
	never();
      recv_cookie_echo_conn_ind:
	if (sp->conind) {
		mblk_t *cp;		/* check for existing conn ind */
		for (cp = bufq_head(&sp->conq); cp; cp = cp->b_next) {
			struct sctp_cookie_echo *ce = (struct sctp_cookie_echo *) cp->b_rptr;
			struct sctp_cookie *co = (struct sctp_cookie *) ce->cookie;
			if (co->v_tag == ck->v_tag) {
				seldom();
				return (0);
			}
			if (co->dport != ck->dport || co->sport != ck->sport)
				continue;
			if (co->daddr == ck->daddr && co->saddr == ck->saddr) {
				seldom();
				return (0);
			}
		}
		/* RFC 2960 5.2.4 (4) */
		if (bufq_length(&sp->conq) >= sp->conind) {
			seldom();
			sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
						   SCTP_CAUSE_NO_RESOURCE, NULL, 0);
			return (0);
		}
		if ((err = sctp_conn_ind(sp, mp)))
			return (err);
		return (1);	/* leave cookie and data in the message for conn_res */
	}
	normal(sp->s_state == SCTP_ESTABLISHED);
	sctp_send_cookie_ack(sp);
	return (0);
      recv_cookie_echo_action_p:
	/* 
	 *  SCTP IG 2.6 replacement:
	 *
	 *  In this case we may have refused to divulge existing tags to a
	 *  potential attacker and poisoned the cookie and the peer did the same
	 *  to us or we have not received an INIT-ACK yet.  The INIT address is
	 *  now known to us, we will process the cookie as though we had handed
	 *  out the verification tag in the initiate tag.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies - ck->timestamp > ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness = htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh, SCTP_CAUSE_STALE_COOKIE,
					   &staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	switch (sp->s_state) {
	case SCTP_COOKIE_WAIT:
		sctp_vhash_unhash(sp);
		sp->v_tag = ck->v_tag;
		sctp_vhash_insert(sp);
		goto recv_cookie_echo_action_b;
	case SCTP_COOKIE_ECHOED:
		ck->v_tag = sp->v_tag;
		goto recv_cookie_echo_action_b;
	}
	rare();
	return (0);
      error_abort:
	/* We had a fatal error processing the COOKIE ECHO and must abort the association. */
	sctp_discon_ind(sp, orig, SCTP_ORIG_PROVIDER, -err);
	return (-err);
}

/* 
 *  RECV COOKIE ACK
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ACK.  If we are in COOKIE ECHOED s_state then we
 *  inform the user interface that the previous connection request is
 *  confirmed, cancel the cookie timer while performing an RTO calculation on
 *  the message and enter the ESTABLISHED s_state.  Any DATA that is bundled
 *  with the COOKIE ACK will be separately indicated to the user with data
 *  indications.  In s_state other than COOKIE ECHOED the entire message is
 *  silently discarded.
 */
STATIC int
sctp_recv_cookie_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	struct sctp_daddr *sd;
	assert(sp);
	switch (sp->s_state) {
	case SCTP_COOKIE_ECHOED:
		/* RFC 2960 5.1 (E) */
		sctp_ack_calc(sp, &sp->timer_cookie);
		sp->s_state = SCTP_ESTABLISHED;
		/* start idle timers */
		usual(sp->daddr);
		for (sd = sp->daddr; sd; sd = sd->next)
			sctp_reset_idle(sd);
		if ((err = sctp_conn_con(sp)))
			return (err);
		return sctp_return_more(mp);
	default:
		/* RFC 2960 5.2.5 */
		rare();
		break;
	}
	return (0);		/* silently discard */
}

/* 
 *  RECV HEARTBEAT
 *  -------------------------------------------------------------------------
 *  We have received a HEARTBEAT.  Quite frankly we don't care what s_state we
 *  are in, we take the heartbeat info and turn it back around as a HEARTBEAT
 *  ACK msg.  We do a little bit of error checking here to make sure that we are
 *  not wasting our time on the packet.  We should only receive HEARTBEAT
 *  messages if we are in the vtag hashes: meaning that we will accept and reply
 *  to a HEARTBEAT in any s_state other than CLOSED and LISTEN.
 *
 */
STATIC int
sctp_recv_heartbeat(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_heartbeat *m = (typeof(m)) mp->b_rptr;
	struct sctpphdr *ph = (typeof(ph)) (m + 1);
	size_t clen = htons(m->ch.len);
	size_t plen = PADC(clen);
	size_t mlen = sizeof(*m) + sizeof(*ph);
	if (clen < mlen || mp->b_wptr < mp->b_rptr + mlen)
		goto emsgsize;
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
	if (jiffies < sp->hb_rcvd + sp->hb_tint)
		goto throttle;
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
	if (ph->type != SCTP_PTYPE_HEARTBEAT_INFO)
		goto eproto;
	sctp_send_heartbeat_ack(sp, (caddr_t) ph, min(plen, PADC(htons(ph->len))));
	return sctp_return_stop(mp);
      eproto:
	ptrace(("PROTO: bad message\n"));
	return (-EPROTO);
#ifdef SCTP_CONFIG_THROTTLE_HEARTBEATS
      throttle:
	ptrace(("WARNING: throttling hearbeat\n"));
	return (-EBUSY);
#endif				/* SCTP_CONFIG_THROTTLE_HEARTBEATS */
      emsgsize:
	ptrace(("PROTO: bad message size\n"));
	return (-EMSGSIZE);
}

/* 
 *  RECV HEARTBEAT ACK
 *  -------------------------------------------------------------------------
 *  This is our HEARTBEAT coming back.  We check that the HEARTBEAT information
 *  matches the information of the last sent HEARTBEAT message to ensure that no
 *  fiddling with the HEARTBEAT info has occured.  The only information we trust
 *  initially is the destination address which is contained in the HEARTBEAT
 *  INFO.  This just helps us index the remainder of the hearbeat information.
 *
 *  We have received a HEARTBEAT ACK message on an ESTABLISHED STREAM.  Perform
 *  the procedures from RFC 2960 8.3.
 */
STATIC int
sctp_recv_heartbeat_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_daddr *sd;
	struct sctp_heartbeat_ack *m = (typeof(m)) mp->b_rptr;
	struct sctp_heartbeat_info *hb = (typeof(hb)) (m + 1);
	if (!(sd = sctp_find_daddr(sp, hb->hb_info.daddr)))
		goto badaddr;
	if (ntohs(hb->ph.len) != sizeof(*hb) + sd->hb_fill)
		goto emsgsize;
	if (hb->ph.type != SCTP_PTYPE_HEARTBEAT_INFO)
		goto badparm;
	if (!sd->hb_time || sd->hb_time != hb->hb_info.timestamp)
		goto einval;
	if (sd->timer_heartbeat)
		untimeout(xchg(&sd->timer_heartbeat, 0));
	sctp_rtt_calc(sd, sd->hb_time);
      done:
	return sctp_return_stop(mp);
      einval:
	ptrace(("PROTO: old or fiddled timestamp\n"));
	goto done;
      badparm:
	ptrace(("PROTO: bad heartbeat parameter type\n"));
	goto done;
      emsgsize:
	ptrace(("PROTO: bad heartbeat parameter size\n"));
	goto done;
      badaddr:
	ptrace(("PROTO: bad destination parameter\n"));
	goto done;
}

/* 
 *  RECV SHUTDOWN
 *  -------------------------------------------------------------------------
 */
STATIC void
sctp_shutdown_ack_calc(sp, ack)
	sctp_t sp;
	uint32_t ack;
{
	if (before(ack, sp->t_ack)) {
		rare();
		/* If the SHUTDOWN acknowledges our sent data chunks that have already been
		   acknowledged, then it is an old (or erroneous) message and we will ignore it. */
		return;
	}
	sctp_cumm_ack(sp, ack);
	sctp_dest_calc(sp);
}

STATIC int
sctp_recv_shutdown(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_shutdown *m = (struct sctp_shutdown *) mp->b_rptr;
	uint32_t ack = ntohl(m->c_tsn);
	assert(sp);
	switch (sp->s_state) {
	case SCTP_ESTABLISHED:
		sp->s_state = SCTP_SHUTDOWN_RECEIVED;
		/* fall thru */
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_shutdown_ack_calc(sp, ack);
		if (!sp->ops->sctp_ordrel_ind)
			if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
				sctp_send_shutdown_ack(sp);
		break;
	case SCTP_SHUTDOWN_PENDING:	/* only when we have ordrel */
		sp->s_state = SCTP_SHUTDOWN_RECVWAIT;
		/* fall thru */
	case SCTP_SHUTDOWN_RECVWAIT:
		sctp_shutdown_ack_calc(sp, ack);
		if (!bufq_head(&sp->sndq) && !bufq_head(&sp->rtxq))
			sctp_send_shutdown_ack(sp);
		break;
	case SCTP_SHUTDOWN_SENT:	/* only when we have ordrel */
		sctp_shutdown_ack_calc(sp, ack);
		/* faill thru */
	case SCTP_SHUTDOWN_ACK_SENT:
		sctp_send_shutdown_ack(sp);	/* We do this */
		break;
	default:
		/* ignore the SHUTDOWN chunk */
		rare();
		return sctp_return_more(mp);
	}
	if (sp->ngaps) {
		rare();
		/* Check sanity of sender: if we have gaps in our acks to the peer and the peer
		   sends a SHUTDOWN, then it is in error.  The peer cannot send SHUTDOWN when it
		   has unacknowledged data.  If this is the case, we zap the connection. */
		sctp_send_abort_error(sp, SCTP_CAUSE_PROTO_VIOLATION, NULL, 0);
		sctp_discon_ind(sp, SCTP_ORIG_USER, -EPIPE);
		return sctp_return_stop(mp);
	}
	/* send up orderly release indication to ULP */
	sctp_ordrel_ind(sp);
	return sctp_return_more(mp);
}

/* 
 *  RECV SHUTDOWN ACK
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN ACK chunk through the vtag hashes.  We are
 *  expecting the SHUTDOWN ACK because we have previously sent a SHUTDOWN or
 *  SHUTDOWN ACK or we ignore the message.
 */
STATIC int
sctp_recv_shutdown_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err;
	assert(sp);
	assert(mp);
	switch (sp->s_state) {
	case SCTP_SHUTDOWN_SENT:
		/* send up orderly release indication to ULP */
		if ((err = sctp_ordrel_ind(sp)))
			return (err);
		/* fall thru */
	case SCTP_SHUTDOWN_ACK_SENT:
		// sctp_ack_calc(sp, &sp->timer_shutdown); /* WHY? */
		sctp_send_shutdown_complete(sp);
		sctp_disconnect(sp);
		break;
	default:
		/* ignore unexpected SHUTDOWN ACK */
		rare();
		break;
	}
	return sctp_return_stop(mp);
}

/* 
 *  RECV SHUTDOWN COMPLETE
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN COMPLETE in the SHUTDOWN ACK SENT s_state.  This
 *  is the normal path for shutting down an SCTP association.  Outstanding data
 *  has already been processed.  Remove ourselves from the hashes and process
 *  any backlog.  RFC 2960 6.10: SHUTDOWN COMPLETE cannot have any other chunks
 *  bundled with them.
 *
 *  We receive SHUTDOWN COMPLETE chunks through the vtag hashes.  We are
 *  expecting SHUTDOWN COMPLETE because we sent a SHUTDOWN ACK.  We are in the
 *  TS_IDLE, NS_IDLE states but are still in the vtag hashes.
 */
STATIC int
sctp_recv_shutdown_complete(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	assert(sp);
	assert(mp);
	switch (sp->s_state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		// sctp_ack_calc(sp, &sp->timer_shutdown); /* WHY? */
		sctp_disconnect(sp);
		break;
	default:
		/* ignore unexpected SHUTDOWN COMPLETE */
		rare();
		break;
	}
	return sctp_return_stop(mp);
}

/* 
 *  RECV UNRECOGNIZED CHUNK TYPE
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_unrec_ctype(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int ctype = ((struct sctpchdr *) mp->b_rptr)->type;
	if (ctype & SCTP_CTYPE_MASK_REPORT)
		sctp_send_abort_error(sp, SCTP_CAUSE_BAD_CHUNK_TYPE, mp->b_rptr,
				      mp->b_wptr - mp->b_rptr);
	if (ctype & SCTP_CTYPE_MASK_CONTINUE)
		return sctp_return_more(mp);
	return (-EPROTO);
}

#ifdef SCTP_CONFIG_ECN
#undef INET_ECN_xmit
#define	INET_ECN_xmit(sp) do { (sp)->ip_tos |= 2; } while (0)
#undef INET_ECN_dontxmit
#define	INET_ECN_dontxmit(sp) do { (sp)->ip_tos &= ~3; } while (0)
#endif				/* SCTP_CONFIG_ECN */

/* 
 *  RECV ECNE (Explicit Contestion Notification Echo)
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_ecne(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
#ifdef SCTP_CONFIG_ECN
	struct sctp_ecne *m = (typeof(m)) mp->b_rptr;
	uint32_t l_tsn = ntohl(m->l_tsn);
	struct sctp_daddr *sd;
	sp->p_caps |= SCTP_CAPS_ECN;
	INET_ECN_xmit(sp);
	if (!after(l_tsn, sp->p_lsn))
		goto done;
	sp->p_lsn = l_tsn;
	/* need to find the destination to which this TSN was transmitted */
	for (mp = bufq_head(&sp->rtxq); mp && SCTP_TCB(mp)->tsn != l_tsn; mp = mp->b_next) ;
	if (!mp)
		goto done;
	if (!(sd = SCTP_TCB(mp)->daddr))
		goto done;
	sctp_assoc_timedout(sp, sd, 0, 0);
      done:
	return sctp_return_more(mp);
#else
	return sctp_recv_unrec_ctype(sp, mp);
#endif
}

/* 
 *  RECV CWR (Congestion Window Reduction)
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_cwr(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
#ifdef SCTP_CONFIG_ECN
	struct sctp_cwr *m = (typeof(m)) mp->b_rptr;
	uint32_t l_tsn = ntohl(m->l_tsn);
	(void) l_tsn;
	fixme(("FIXME: Finish this function\n"));
	sp->sackf &= ~SCTP_SACKF_ECN;
	return sctp_return_more(mp);
#else
	return sctp_recv_unrec_ctype(sp, mp);
#endif
}

#ifdef SCTP_CONFIG_ADD_IP

/* 
 *  RECV ASCONF
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_asconf(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_asconf *m = (typeof(m)) mp->b_rptr;
	uint32_t asn = ntohl(m->asn);
	struct sctp_daddr *sd;
	int reconfig = 0;
	size_t mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
	if (!(sp->l_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto refuse;
	/* ADD-IP 4.1.1 R4 */
	if (sp->s_state != SCTP_ESTABLISHED)
		goto discard;
	if (mlen < sizeof(*m))
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
				if ((sd = sctp_add_daddr(sp, htonl(a->addr)))) {
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
				if (a->addr != ((struct iphdr *) mp->b_datap->db_base)->saddr
				    && sp->danum != 1) {
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
					memcpy(bptr, pptr, plen);
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
				if (a->addr == ((struct iphdr *) mp->b_datap->db_base)->saddr) {
					/* request to delete source address */
					er = ((typeof(er)) bptr)++;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh) + plen);
					er->cid = ph->del_ip.id;
					eh = ((typeof(eh)) bptr)++;
					eh->code = __constant_htons(SCTP_CAUSE_SOURCE_ADDR);
					eh->len = htons(sizeof(*eh) + plen);
					memcpy(bptr, pptr, plen);
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
					memcpy(bptr, pptr, plen);
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
			sctp_send_msg(sp, sd, sp->reply);
		}
	}
      discard:
	/* ADD-IP 4.2 Rule C1 & C4, R4 */
	return sctp_return_more(mp);
      noroute:
	ptrace(("ERROR: no route to peer\n"));
	return -EHOSTUNREACH;
      emsgsize:
	ptrace(("ERROR: bad message size\n"));
	return -EMSGSIZE;
      refuse:
	return sctp_recv_unrec_ctype(sp, mp);
}

/* 
 *  RECV ASCONF ACK
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_asconf_ack(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_asconf_ack *m = (typeof(m)) mp->b_rptr;
	uint32_t asn = ntohl(m->asn);
	/* ADD-IP 4.1.1 R4 */
	if (sp->s_state != SCTP_ESTABLISHED)
		goto discard;
	if (before(asn, sp->l_asn))
		goto discard;
	if (after(asn, sp->l_asn))
		goto abort;
	/* ADD-IP 4.3 D0 */
	if (!sp->retry && !timer_pending(&sp->timer_asconf))
		goto abort;
	/* ADD-IP 4.1 (A5) */
	sctp_ack_calc(sp, &sp->timer_asconf);
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
	sctp_discon_ind(sp, ECONNABORTED);
	return (-ECONNABORTED);
}

#endif				/* SCTP_CONFIG_ADD_IP */

#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
/* 
 *  RECV FORWARD TSN
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_recv_forward_tsn(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_forward_tsn *m = (typeof(m)) mp->b_rptr;
	uint32_t f_tsn;
	size_t mlen, clen, plen, nstrs;
	if ((1 << sp->s_state) & ~(SCTPF_RECEIVING))
		goto outstate;
	f_tsn = ntohl(m->f_tsn);
	mlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
	clen = htons(m->ch.len);
	plen = PADC(clen);
	if (clen < sizeof(*m) || mlen < plen)
		goto emsgsize;
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
				if (!after_s(st->n.ssn, ssn))
					st->n.ssn = ssn;
		}
	}
	if (!sp->ngaps) {
		/* push the ack point and mark sack */
		sp->r_ack = sp->p_fsn;
		sctp_send_sack(sp);
	} else {
		/* need to clean up any gaps */
		sctp_deliver_data(sp, mp);
	}
	return sctp_return_more(mp);
      sctp_recv_fsn_duplicate:
	/* FSN is duplicate. */
	/* TODO: Here we should peg the duplicate agains the last destination to which we send a
	   SACK.  Receiving duplicate FSNs is a weak indication that our SACKs might not be getting 
	   through. */
      outstate:
	return sctp_return_more(mp);
}
#endif				/* SCTP_CONFIG_PARTIAL_RELIABILITY */

/* 
 *  RECV SCTP MESSAGE
 *  -------------------------------------------------------------------------
 */
static int sctp_rcv_ootb(mblk_t *mp);
static int
sctp_recv_msg(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	int err = -EMSGSIZE;
	struct sctpchdr *ch;
	ensure(sp, goto efault);
	ensure(mp, goto efault);
	/* set address for reply chunks */
	if (!(sp->caddr = sctp_find_daddr(sp, ((struct iphdr *) mp->b_datap->db_base)->saddr))) {
		ch = (typeof(ch)) mp->b_rptr;
		switch (ch->type) {
		case SCTP_CTYPE_INIT:
		case SCTP_CTYPE_INIT_ACK:
		case SCTP_CTYPE_COOKIE_ECHO:
			break;
		default:
			return sctp_rcv_ootb(mp);
		}
	}
	do {
		size_t clen, plen, dlen;
		dlen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;
		if (dlen < sizeof(*ch))
			goto emsgsize;
		if (mp->b_cont && !pullupmsg(mp, -1))
			goto enomem;	/* for now */
		ch = (typeof(ch)) mp->b_rptr;
		clen = htohs(ch->len);
		plen = PADC(clen);
		if (dlen < plen)
			goto emsgsize;
		if (ch->type == SCTP_CTYPE_DATA) {
			err = sctp_recv_data(sp, mp);
		} else {
			switch (ch->type) {
			case SCTP_CTYPE_INIT:
				err = sctp_recv_init(sp, mp);
				break;
			case SCTP_CTYPE_INIT_ACK:
				err = sctp_recv_init_ack(sp, mp);
				break;
			case SCTP_CTYPE_SACK:
				err = sctp_recv_sack(sp, mp);
				break;
			case SCTP_CTYPE_HEARTBEAT:
				err = sctp_recv_heartbeat(sp, mp);
				break;
			case SCTP_CTYPE_HEARTBEAT_ACK:
				err = sctp_recv_heartbeat_ack(sp, mp);
				break;
			case SCTP_CTYPE_ABORT:
				err = sctp_recv_abort(sp, mp);
				break;
			case SCTP_CTYPE_SHUTDOWN:
				err = sctp_recv_shutdown(sp, mp);
				break;
			case SCTP_CTYPE_SHUTDOWN_ACK:
				err = sctp_recv_shutdown_ack(sp, mp);
				break;
			case SCTP_CTYPE_ERROR:
				err = sctp_recv_error(sp, mp);
				break;
			case SCTP_CTYPE_COOKIE_ECHO:
				err = sctp_recv_cookie_echo(sp, mp);
				break;
			case SCTP_CTYPE_COOKIE_ACK:
				err = sctp_recv_cookie_ack(sp, mp);
				break;
			case SCTP_CTYPE_ECNE:
				err = sctp_recv_ecne(sp, mp);
				break;
			case SCTP_CTYPE_CWR:
				err = sctp_recv_cwr(sp, mp);
				break;
			case SCTP_CTYPE_SHUTDOWN_COMPLETE:
				err = sctp_recv_shutdown_complete(sp, mp);
				break;
#ifdef SCTP_CONFIG_ADD_IP
			case SCTP_CTYPE_ASCONF:
				err = sctp_recv_asconf(sp, mp);
				break;
			case SCTP_TYPE_ASCONF_ACK:
				err = sctp_recv_asconf_ack(sp, mp);
				break;
#endif
#ifdef SCTP_CONFIG_PARTIAL_RELIABILITY
			case SCTP_CTYPE_FORWARD_TSN:
				err = sctp_recv_forward_tsn(sp, mp);
				break;
#endif
			default:
				err = sctp_recv_unrec_ctype(sp, mp);
				break;
			}
		}
	}
	while (err >= sizeof(*ch));
	if (err > 0)
		goto emsgsize;
	else if (err < 0)
		goto error;
      done:
	return (err);
      efault:
	err = -EFAULT;
	swerr();
	goto done;
      enomem:
	err = -ENOBUFS;
	ptrace(("ERROR: no buffers for msgb handling (dropping)\n"));
	goto done;
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("ERROR: message too short (dropping)\n"));
	goto done;
      error:
	ptrace(("sp = %p, Error %d returned.\n", sp, err));
	goto done;
}

/* 
 *  RECV SCTP ICMP ERROR
 *  -------------------------------------------------------------------------
 */
#ifndef ip_rt_mtu_expires
#define ip_rt_mtu_expires (10*60*HZ)
#else
extern int ip_rt_mtu_expires;
#endif
static int
sctp_recv_err(sp, mp)
	sctp_t *sp;
	mblk_t *mp;
{
	struct sctp_daddr *sd;
	uint32_t daddr = *((uint32_t *) mp->b_rptr)++;
	struct icmphdr *icmph = (struct icmphdr *) mp->b_rptr;
	int type = icmph->type;
	int code = icmph->code;
	if (!(sd = sctp_find_daddr(sp, daddr)))
		goto drop;
	switch (type) {
	case ICMP_SOURCE_QUENCH:
		if (!sd)
			goto drop;
		sd->ssthresh = sd->cwnd >> 1 > sd->mtu << 1 ? sd->cwnd >> 1 : sd->mtu << 1;
		sd->cwnd = sd->mtu;
#ifdef SCTP_IG_UPDATES
		/* SCTP IG Section 2.9 */
		sd->partial_ack = 0;
#endif
		sd->rto = sd->rto << 1 ? sd->rto << 1 : 1;
		sd->rto = sd->rto_min > sd->rto ? sd->rto_min : sd->rto;
		sd->rto = sd->rto_max < sd->rto ? sd->rto_max : sd->rto;
		break;
	case ICMP_DEST_UNREACH:
		if (code > NR_ICMP_UNREACH)
			goto drop;
		if (code == ICMP_FRAG_NEEDED) {
			size_t mtu = ntohs(icmph->un.frag.mtu);
			ip_rt_update_pmtu(sd->dst_cache, mtu);
			if (sd->dst_cache->pmtu > mtu && mtu && mtu >= 68
			    && !(sd->dst_cache->mxlock & (1 << RTAX_MTU))) {
				sd->dst_cache->pmtu = mtu;
				dst_set_expires(sd->dst_cache, ip_rt_mtu_expires);
			}
		}
		break;
	case ICMP_PARAMETERPROB:
	case ICMP_TIME_EXCEEDED:
	default:
		goto drop;
	}
      drop:
	return (0);
}

/* 
 *  =========================================================================
 *
 *  IP Packet Handling
 *
 *  =========================================================================
 */
/* 
 *  -------------------------------------------------------------------------
 *
 *  SCTP RCV OOTB
 *
 *  -------------------------------------------------------------------------
 */
/* *INDENT-OFF* */
STATIC int
sctp_rcv_ootb(mblk_t *mp)
{
	struct iphdr *iph = (typeof(iph)) mp->b_datap->db_base;
	struct sctphdr *sh = (typeof(sh)) (mp->b_datap->db_base + (iph->ihl << 2));
	struct sctpchdr *ch = (typeof(ch)) mp->b_rptr;
	int sat = inet_addr_type(iph->saddr);
	seldom();
	ensure(mp, return (-EFAULT));
	if (sat != RTN_UNICAST && sat != RTN_LOCAL) {
		/* RFC 2960 8.4(1).  */
		freemsg(mp);
		return (0);
	}
	switch (ch->type) {
	case SCTP_CTYPE_COOKIE_ACK:		/* RFC 2960 8.4(7).  */
	case SCTP_CTYPE_ERROR:			/* RFC 2960 8.4(7).  */
	case SCTP_CTYPE_ABORT:			/* RFC 2960 8.4(2).  */
	case SCTP_CTYPE_SHUTDOWN_COMPLETE:	/* RFC 2960 8.4(6).  */
	case SCTP_CTYPE_INIT:			/* RFC 2960 8.4(3) and (8). */
	case SCTP_CTYPE_INIT_ACK:		/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_COOKIE_ECHO:		/* RFC 2960 8.4(4) and (8). */
	default:
		seldom();
		break;
	case SCTP_CTYPE_SHUTDOWN:		/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_SACK:			/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_HEARTBEAT:		/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_HEARTBEAT_ACK:		/* RFC 2960 8.4(8).  */
	case SCTP_CTYPE_DATA:			/* RFC 2960 8.4(8).  */
		seldom();
		if (!sctp_lookup_tcb(sh->srce, sh->dest, iph->saddr, iph->daddr))
			sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		break;
	case SCTP_CTYPE_SHUTDOWN_ACK:
		seldom();
		if (!sctp_lookup_tcb(sh->srce, sh->dest, iph->saddr, iph->daddr))
			sctp_send_shutdown_complete_ootb(iph->saddr, iph->daddr, sh);
		break;
	}
	ptrace(("Freeing mblk %p\n", mp));
	freemsg(mp);
	return (0);
}
/* *INDENT-ON* */

/* 
 *  -------------------------------------------------------------------------
 *
 *  SCTP ERR
 *
 *  -------------------------------------------------------------------------
 *  We have received an ICMP error for the protocol number.  Because we don't
 *  want an races here we place a M_ERROR message on the read queue of the
 *  stream to which the message applies.  This distinguishes it from M_DATA
 *  messages.  It is processed within the stream with queues locked.  We have
 *  to copy the information because the skb will go away after this call.
 */
STATIC void
sctp_err(struct sk_buff *skb, uint32_t info)
{
	sctp_t *sp;
	struct sctphdr *sh;
	mblk_t *mp;
	struct iphdr *iph = (struct iphdr *) skb->data;
	size_t mlen = sizeof(uint32_t) + sizeof(struct icmphdr *);
	size_t ihl;
	ensure(skb, return);
#define ICMP_MIN_LENGTH 8
	if (skb->len < (ihl = iph->ihl << 2) + ICMP_MIN_LENGTH) {
		seldom();
		return;
	}
	sh = (struct sctphdr *) (skb->data + ihl);
	SCTPHASH_BH_RLOCK();
	sp = sh->v_tag ? sctp_lookup_ptag(sh->v_tag, sh->srce, sh->dest, iph->saddr, iph->daddr)
	    : sctp_lookup_tcb(sh->srce, sh->dest, iph->saddr, iph->daddr);
	SCTPHASH_BH_RUNLOCK();
	usual(sp);
	if (!sp)
		goto nostream;
	if (!canput(sp->rq))
		goto flow_controlled;
	if (!(mp = allocb(mlen, BPRI_MED)))
		goto no_buffers;
	mp->b_datap->db_type = M_ERROR;
	*((uint32_t *) mp->b_wptr)++ = iph->daddr;
	*((struct icmphdr *) mp->b_wptr)++ = *(skb->h.icmph);
	putq(sp->rq, mp);
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto drop;
      flow_controlled:
	ptrace(("ERROR: stream is flow controlled\n"));
	goto drop;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	goto drop;
      drop:
	return;
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  SCTP RCV
 *
 *  -------------------------------------------------------------------------
 *  This is the received frame handler for SCTP IPv4.  All packets received by
 *  IPv4 with the protocol number IPPROTO_SCTP will arrive here first.  We
 *  should be performing the Adler-32 checksum on the packet.  If the Adler-32
 *  checksum fails, then we should silently discard per RFC 2960.
 */
STATIC void
sctp_free(char *data)
{
	struct sk_buff *skb = (struct sk_buff *) data;
	trace();
	ensure(skb, return);
	kfree_skb(skb);
	return;
}
STATIC int
sctp_rcv(struct sk_buff *skb)
{
	mblk_t *mp;
	sctp_t *sp;
	uint32_t csum0, csum1, csum2;
	struct sctphdr *sh;
	unsigned short len;
	frtn_t fr = { &sctp_free, (char *) skb };
	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
	/* For now...  We should actually place non-linear fragments into seperate mblks and pass
	   them up as a chain. */
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
	/* pull up the ip header */
	__skb_pull(skb, skb->h.raw - skb->data);
	sh = (struct sctphdr *) skb->h.raw;
	len = skb->len;
	/* perform the crc-32c checksum per RFC 2960 Appendix B. */
	csum0 = sh->check;
	csum1 = ntohl(csum0);
	sh->check = 0;
	csum2 = crc32c(~0UL, sh, len);
	sh->check = csum0;
	if (csum1 != csum2)
		goto bad_checksum;
	/* pull to the ip header */
	__skb_push(skb, skb->data - skb->nh.raw);
	if (!(mp = esballoc(skb->data, skb->len, BPRI_MED, &fr)))
		goto no_buffers;
	ptrace(("Allocated mblk %p\n", mp));
	mp->b_datap->db_type = M_DATA;
	mp->b_wptr = mp->b_rptr + skb->len;
	/* trim the ip header */
	mp->b_rptr += skb->h.raw - skb->nh.raw;
	mp->b_rptr += sizeof(struct sctphdr);
	SCTPHASH_BH_RLOCK();
	sp = sctp_lookup(sh, skb->nh.iph->daddr, skb->nh.iph->saddr);
	SCTPHASH_BH_RUNLOCK();
	if (!sp)
		goto no_sctp_stream;
	if (!canput(sp->rq))
		goto flow_controlled;
	skb->dev = NULL;
	putq(sp->rq, mp);
	return (0);
      flow_controlled:
	freemsg(mp);
	return (0);
      no_sctp_stream:
	return sctp_rcv_ootb(mp);
      no_buffers:
	ptrace(("ERROR: Couldn't allocate mblk\n"));
	goto discard_it;
      bad_checksum:
	ptrace(("ERROR: Bad checksum\n"));
	goto discard_it;
      linear_fail:
	ptrace(("ERROR: Couldn't linearize skb\n"));
	goto discard_it;
      bad_pkt_type:
	ptrace(("ERROR: Packet not PACKET_HOST\n"));
	goto discard_it;
      discard_it:
	/* Discard frame silently */
	kfree_skb(skb);
	return (0);
}

/* 
 *  =========================================================================
 *
 *  IP Protocol Registration
 *
 *  =========================================================================
 */
#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP 132
#endif
STATIC struct inet_protocol sctp_protocol = {
	sctp_rcv,			/* SCTP data handler */
	sctp_err,			/* SCTP error control */
	NULL,				/* next */
	IPPROTO_SCTP,			/* protocol ID */
	0,				/* copy */
	NULL,				/* data */
	"SCTP"				/* name */
};
static void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol);
}
static void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol);
}

#define SCTP_N_CMINORS 255
/* 
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 *  This driver defines two user interfaces: one NPI, the other TPI.
 */
STATIC struct module_info sctp_n_minfo = {
	mi_idnum:0,			/* Module ID number */
	mi_idname:"sctp",		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10		/* Lo water mark */
};
STATIC int sctp_n_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sctp_n_close(queue_t *, int, cred_t *);
STATIC int sctp_n_rput(queue_t *, mblk_t *);
STATIC int sctp_n_rsrv(queue_t *);
STATIC struct qinit sctp_n_rinit = {
	qi_putp:sctp_n_rput,		/* Read put (msg from below) */
	qi_srvp:sctp_n_rsrv,		/* Read queue service */
	qi_qopen:sctp_n_open,		/* Each open */
	qi_qclose:sctp_n_close,		/* Last close */
	qi_minfo:&sctp_n_minfo,		/* Information */
};
STATIC int sctp_n_wput(queue_t *, mblk_t *);
STATIC int sctp_n_wsrv(queue_t *);
STATIC struct qinit sctp_n_winit = {
	qi_putp:sctp_n_wput,		/* Write put (msg from above) */
	qi_srvp:sctp_n_wsrv,		/* Write queue service */
	qi_minfo:&sctp_n_minfo,		/* Information */
};
STATIC struct streamtab sctp_n_info = {
	st_rdinit:&sctp_n_rinit,	/* Upper read queue */
	st_wrinit:&sctp_n_winit,	/* Upper write queue */
};
#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
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
STATIC int
n_conn_ind(sctp_t * sp, mblk_t *cp)
{
	mblk_t *mp;
	N_conn_ind_t *p;
	N_qos_sel_conn_sctp_t *q;
	struct sctp_cookie_echo *m = (struct sctp_cookie_echo *) cp->b_rptr;
	struct sctp_cookie *ck = (struct sctp_cookie *) m->cookie;
	size_t danum = ck->danum + 1;
	uint32_t *daptr = (uint32_t *) (((caddr_t) (ck + 1) + ck->opt_len));
	size_t dst_len = sizeof(uint16_t) + sizeof(uint32_t);
	size_t src_len = danum ? sizeof(uint16_t) + danum * sizeof(uint32_t) : 0;
	size_t qos_len = sizeof(*q);
	ensure(((1 << sp->i_state) & (NSF_IDLE | NSF_WRES_CIND)), return (-EFAULT));
	if (bufq_length(&sp->conq) < sp->conind) {
		if (canputnext(sp->rq)) {
			if ((mp = allocb(sizeof(*p) + dst_len + src_len + qos_len, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = (N_conn_ind_t *) mp->b_wptr;
				p->PRIM_type = N_CONN_IND;
				p->DEST_length = dst_len;
				p->DEST_offset = dst_len ? sizeof(*p) : 0;
				p->SRC_length = src_len;
				p->SRC_offset = src_len ? sizeof(*p) + dst_len : 0;
				p->SEQ_number = (ulong) cp;
				p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
				p->QOS_length = qos_len;
				p->QOS_offset = qos_len ? sizeof(*p) + dst_len + src_len : 0;
				mp->b_wptr += sizeof(*p);
				*((uint16_t *) mp->b_wptr)++ = ck->sport;
				*((uint32_t *) mp->b_wptr)++ = ck->saddr;
				if (danum)
					*((uint16_t *) mp->b_wptr)++ = ck->dport;
				if (danum--)
					*((uint32_t *) mp->b_wptr)++ = ck->daddr;
				while (danum--)
					*((uint32_t *) mp->b_wptr)++ = *daptr++;
				q = (N_qos_sel_conn_sctp_t *) mp->b_wptr;
				q->n_qos_type = N_QOS_SEL_CONN_SCTP;
				q->i_streams = ck->n_istr;
				q->o_streams = ck->n_ostr;
				mp->b_wptr += sizeof(*q);
				bufq_queue(&sp->conq, cp);
				sp->i_state = NS_WRES_CIND;
				putnext(sp->rq, mp);
				return (0);
			}
			seldom();
			return (-ENOBUFS);
		}
		seldom();
		return (-EBUSY);
	}
	seldom();
	return (-ERESTART);
}

/* 
 *  N_CONN_CON      12 - Connection established
 *  ---------------------------------------------------------------
 */
STATIC int
n_conn_con(sctp_t * sp)
{
	mblk_t *mp;
	N_conn_con_t *p;
	N_qos_sel_conn_sctp_t *q;
	struct sctp_daddr *sd = sp->daddr;
	size_t res_len = sp->danum ? sizeof(uint16_t) + sp->danum * sizeof(sd->daddr) : 0;
	size_t qos_len = sizeof(*q);
	ensure((sp->i_state == NS_WCON_CREQ), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + res_len + qos_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = (N_conn_con_t *) mp->b_wptr;
			p->PRIM_type = N_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) + res_len : 0;
			mp->b_wptr += sizeof(*p);
			if (sd)
				*((uint16_t *) mp->b_wptr)++ = sp->dport;
			for (; sd; sd = sd->next)
				*((uint32_t *) mp->b_wptr)++ = sd->daddr;
			q = (N_qos_sel_conn_sctp_t *) mp->b_wptr;
			q->n_qos_type = N_QOS_SEL_CONN_SCTP;
			q->i_streams = sp->n_istr;
			q->o_streams = sp->n_ostr;
			mp->b_wptr += sizeof(*q);
			sp->i_state = NS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  N_DISCON_IND    13 - NC disconnected
 *  ---------------------------------------------------------------
 *  For SCTP the responding address in a NC connection refusal is always the
 *  destination address to which the NC connection request was sent.
 */
STATIC int
n_discon_ind(sctp_t * sp, ulong orig, long reason, mblk_t *seq)
{
	mblk_t *mp;
	N_discon_ind_t *p;
	ensure(((1 << sp->
		 i_state) & (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ |
			     NSF_WRES_RIND)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_discon_ind_t *) mp->b_wptr;
			p->PRIM_type = N_DISCON_IND;
			p->DISCON_orig = orig;
			p->DISCON_reason = reason;
			p->RES_length = 0;
			p->RES_offset = 0;
			p->SEQ_number = (ulong) seq;
			mp->b_wptr += sizeof(*p);
			if (seq) {
				bufq_unlink(&sp->conq, seq);
				freemsg(seq);
			}
			if (!bufq_length(&sp->conq))
				sp->i_state = NS_IDLE;
			else
				sp->i_state = NS_WRES_CIND;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  N_DATA_IND      14 - Incoming connection-mode data indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_data_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint more,
	   mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	N_qos_sel_data_sctp_t *q;
	if (!((1 << sp->i_state) & (NSF_DATA_XFER))) {
		printd(("Interace in state %u\n", sp->i_state));
		printd(("mblk size is %d\n", msgdsize(dp)));
	}
	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
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
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  N_EXDATA_IND    15 - Incoming expedited data indication
 *  ---------------------------------------------------------------
 *  The lack of a more flag presents a challenge.  Perhaps we should be
 *  reassembling expedited data.
 */
STATIC int
n_exdata_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint more,
	     mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;
	N_qos_sel_data_sctp_t *q;
	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
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
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  N_INFO_ACK      16 - Information Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_info_ack(sctp_t * sp)
{
	mblk_t *mp;
	N_info_ack_t *p;
	N_qos_sel_info_sctp_t *q;
	N_qos_range_info_sctp_t *r;
	size_t add_len = sp->sanum ? sizeof(uint16_t) + sp->sanum * sizeof(uint32_t) : 0;
	size_t qos_len = sizeof(*q);
	size_t qor_len = sizeof(*r);
	size_t pro_len = sizeof(uint32_t);
	struct sctp_saddr *ss = sp->saddr;
	if ((mp = allocb(sizeof(*p) + add_len + qos_len + qor_len + pro_len, BPRI_MED))) {
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
		    REC_CONF_OPT | EX_DATA_OPT | ((sp->flags & SCTP_FLAG_DEFAULT_RC_SEL) ?
						  DEFAULT_RC_SEL : 0);
		p->NIDU_size = -1;	/* no limit on NIDU size */
		p->SERV_type = N_CONS;
		p->CURRENT_state = sp->i_state;
		p->PROVIDER_type = N_SUBNET;
		p->NODU_size = sp->pmtu >> 1;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len + qos_len + qor_len : 0;
		p->NPI_version = N_VERSION_2;
		mp->b_wptr += sizeof(*p);
		if (ss)
			*((uint16_t *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((uint32_t *) mp->b_wptr)++ = ss->saddr;
		q = (N_qos_sel_info_sctp_t *) mp->b_wptr;
		q->n_qos_type = N_QOS_SEL_INFO_SCTP;
		if ((1 << sp->i_state) & (NSF_UNBND | NSF_IDLE | NSF_WCON_CREQ)) {
			q->i_streams = sp->max_istr;
			q->o_streams = sp->req_ostr;
		} else {
			q->i_streams = sp->n_istr;
			q->o_streams = sp->n_ostr;
		}
		q->ppi = sp->ppi;
		q->sid = sp->sid;
		q->max_inits = sp->max_inits;
		q->max_retrans = sp->max_retrans;
		q->ck_life = sp->ck_life;
		q->ck_inc = sp->ck_inc;
		q->hmac = sp->hmac;
		q->throttle = sp->throttle;
		q->max_sack = sp->max_sack;
		q->rto_ini = sp->rto_ini;
		q->rto_min = sp->rto_min;
		q->rto_max = sp->rto_max;
		q->rtx_path = sp->rtx_path;
		q->hb_itvl = sp->hb_itvl;
		mp->b_wptr += sizeof(*q);
		r = (N_qos_range_info_sctp_t *) mp->b_wptr;
		r->n_qos_type = N_QOS_RANGE_INFO_SCTP;
		mp->b_wptr += sizeof(*r);
		*((uint32_t *) mp->b_wptr)++ = sp->ppi;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  N_BIND_ACK      17 - NS User bound to network address
 *  ---------------------------------------------------------------
 */
STATIC int
n_bind_ack(sctp_t * sp)
{
	mblk_t *mp;
	N_bind_ack_t *p;
	struct sctp_saddr *ss = sp->saddr;
	size_t add_len = sp->sanum ? sizeof(sp->sport) + sp->sanum * sizeof(ss->saddr) : 0;
	size_t pro_len = sizeof(sp->ppi);
	if ((mp = allocb(sizeof(*p) + add_len + pro_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_bind_ack_t *) mp->b_wptr;
		p->PRIM_type = N_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->conind;
		p->TOKEN_value = (ulong) sp->rq;
		p->PROTOID_length = pro_len;
		p->PROTOID_offset = pro_len ? sizeof(*p) + add_len : 0;
		mp->b_wptr += sizeof(*p);
		if (ss)
			*((typeof(sp->sport) *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((typeof(ss->saddr) *) mp->b_wptr)++ = ss->saddr;
		*((typeof(sp->ppi) *) mp->b_wptr)++ = sp->ppi;
		sp->i_state = NS_IDLE;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  N_ERROR_ACK     18 - Error Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_error_ack(sctp_t * sp, int prim, int err)
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
		never();
		return (err);
	}
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_error_ack_t *) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = err < 0 ? NSYSERR : err;
		p->UNIX_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		switch (sp->i_state) {
		case NS_WACK_OPTREQ:
		case NS_WACK_UREQ:
		case NS_WCON_CREQ:
			sp->i_state = NS_IDLE;
			break;
		case NS_WCON_RREQ:
			sp->i_state = NS_DATA_XFER;
			break;
		case NS_WACK_BREQ:
			sp->i_state = NS_UNBND;
			break;
		case NS_WACK_CRES:
			sp->i_state = NS_WRES_CIND;
			break;
		case NS_WACK_DREQ6:
			sp->i_state = NS_WCON_CREQ;
			break;
		case NS_WACK_DREQ7:
			sp->i_state = NS_WRES_CIND;
			break;
		case NS_WACK_DREQ9:
			sp->i_state = NS_DATA_XFER;
			break;
		case NS_WACK_DREQ10:
			sp->i_state = NS_WCON_RREQ;
			break;
		case NS_WACK_DREQ11:
			sp->i_state = NS_WRES_RIND;
			break;
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we send NOUTSTATE or NNOTSUPPORT or are responding
			   to an N_OPTMGMT_REQ in other than the NS_IDLE state. */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  N_OK_ACK        19 - Success Acknowledgement
 *  ---------------------------------------------------------------
 */
STATIC int
n_ok_ack(sctp_t * sp, ulong prim, ulong seq, ulong tok)
{
	mblk_t *mp;
	N_ok_ack_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (N_ok_ack_t *) mp->b_wptr;
		p->PRIM_type = N_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (sp->i_state) {
		case NS_WACK_OPTREQ:
			sp->i_state = NS_IDLE;
			break;
		case NS_WACK_RRES:
			sp->i_state = NS_DATA_XFER;
			break;
		case NS_WACK_UREQ:
			sp->i_state = NS_UNBND;
			break;
		case NS_WACK_CRES:
		{
			queue_t *aq = (queue_t *) tok;
			sctp_t *ap = (sctp_t *) aq->q_ptr;
			if (ap) {
				ap->i_state = NS_DATA_XFER;
				sctp_cleanup_read(sp);	/* deliver to user what is possible */
				sctp_transmit_wakeup(ap);	/* reply to peer what is necessary */
			}
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
			if (aq != sp->rq) {
				if (bufq_length(&sp->conq))
					sp->i_state = NS_WRES_CIND;
				else
					sp->i_state = NS_IDLE;
			}
			break;
		}
		case NS_WACK_DREQ7:
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			if (bufq_length(&sp->conq))
				sp->i_state = NS_WRES_CIND;
			else
				sp->i_state = NS_IDLE;
			break;
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to an N_OPTMGMT_REQ in other than 
			   the NS_IDLE state. */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  N_DATACK_IND    24 - Data acknowledgement indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_datack_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn)
{
	mblk_t *mp;
	N_datack_ind_t *p;
	N_qos_sel_data_sctp_t *q;
	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + sizeof(*q), BPRI_MED))) {
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
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  N_RESET_IND     26 - Inccoming NC reset request indication
 *  ---------------------------------------------------------------
 */
STATIC int
n_reset_ind(sctp_t * sp, ulong orig, ulong reason, mblk_t *cp)
{
	mblk_t *mp;
	N_reset_ind_t *p;
	ensure(((1 << sp->i_state) & (NSF_DATA_XFER)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_reset_ind_t *) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = orig;
			p->RESET_reason = reason;
			mp->b_wptr += sizeof(*p);
			bufq_queue(&sp->conq, cp);
			sp->i_state = NS_WRES_RIND;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  N_RESET_CON     28 - Reset processing complete
 *  ---------------------------------------------------------------
 */
STATIC int
n_reset_con(sctp_t * sp)
{
	mblk_t *mp;
	N_reset_con_t *p;
	ensure(((1 << sp->i_state) & (NSF_WCON_RREQ)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_reset_con_t *) mp->b_wptr;
			p->PRIM_type = N_RESET_CON;
			mp->b_wptr += sizeof(*p);
			sp->i_state = NS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
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
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_recover_ind_t *) mp->b_wptr;
			p->PRIM_type = N_RECOVER_IND;
			mp->b_wptr += sizeof(*p);
			sp->i_state = NS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
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
	ensure(((1 << sp->i_state) & (NSF_IDLE)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_retrieve_ind_t *) mp->b_wptr;
			p->PRIM_type = N_RETRIEVE_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			sp->i_state = NS_IDLE;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
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
	ensure(((1 << sp->i_state) & (NSF_IDLE)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (N_retrieve_con_t *) mp->b_wptr;
			p->PRIM_type = N_RETREIVE_CON;
			mp->b_wptr += sizeof(*p);
			sp->i_state = NS_IDLE;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}
#endif
STATIC int
sctp_n_conn_ind(sctp_t * sp, mblk_t *cp)
{
	// ptrace(("sp = %x, CONN_IND: seq = %x\n", (uint)sp, (uint)cp));
	return n_conn_ind(sp, cp);
}
STATIC int
sctp_n_conn_con(sctp_t * sp)
{
	// ptrace(("sp = %x, CONN_CONF\n", (uint)sp));
	return n_conn_con(sp);
}
STATIC int
sctp_n_data_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
		uint more, mblk_t *dp)
{
	// ptrace(("sp = %x, DATA_IND: ppi=%u,sid=%u,ssn=%u,tsn=%u,ord=%u,more=%u\n", (uint)sp,
	// ppi,sid,ssn,tsn,ord,more));
	if (ord)
		return n_data_ind(sp, ppi, sid, ssn, tsn, more, dp);
	else
		return n_exdata_ind(sp, ppi, sid, ssn, tsn, more, dp);
}
STATIC int
sctp_n_datack_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn)
{
	// ptrace(("sp = %x, DATACK_IND: ppi=%u,sid=%u,ssn=%u,tsn=%u\n", (uint)sp,
	// ppi,sid,ssn,tsn));
	return n_datack_ind(sp, ppi, sid, ssn, tsn);
}
STATIC int
sctp_n_reset_ind(sctp_t * sp, ulong orig, ulong reason, mblk_t *cp)
{
	// ptrace(("sp = %x, RESET_IND\n", (uint)sp));
	return n_reset_ind(sp, orig, reason, cp);
}
STATIC int
sctp_n_reset_con(sctp_t * sp)
{
	// ptrace(("sp = %x, RESET_CON\n", (uint)sp));
	return n_reset_con(sp);
}
STATIC int
sctp_n_discon_ind(sctp_t * sp, ulong orig, long reason, mblk_t *cp)
{
	// ptrace(("sp = %x, DISCON_IND\n", (uint)sp));
	return n_discon_ind(sp, orig, reason, cp);
}
STATIC int
sctp_n_ordrel_ind(sctp_t * sp)
{
	// ptrace(("sp = %x, ORDREL_IND\n", (uint)sp));
	sctp_ordrel_req(sp);	/* shut it down right now */
	return sctp_n_discon_ind(sp, 0, 0, NULL);
}

STATIC struct sctp_ifops n_ops = {
	sctp_conn_ind:sctp_n_conn_ind,
	sctp_conn_con:sctp_n_conn_con,
	sctp_data_ind:sctp_n_data_ind,
	sctp_datack_ind:sctp_n_datack_ind,
	sctp_reset_ind:sctp_n_reset_ind,
	sctp_reset_con:sctp_n_reset_con,
	sctp_discon_ind:sctp_n_discon_ind,
	sctp_ordrel_ind:sctp_n_ordrel_ind,
	sctp_retr_ind:NULL,
	sctp_retr_con:NULL,
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
n_conn_req(sctp_t * sp, mblk_t *mp)
{
	int err = -EFAULT;
	size_t anum;
	struct sctp_addr *a;
	N_conn_req_t *p = (N_conn_req_t *) mp->b_rptr;
	N_qos_sel_conn_sctp_t *q = (N_qos_sel_conn_sctp_t *) (mp->b_rptr + p->QOS_offset);
	if (sp->i_state != NS_IDLE)
		goto outstate;
	sp->i_state = NS_WCON_CREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt1;
		if (q->n_qos_type != N_QOS_SEL_CONN_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
	}
	a = (struct sctp_addr *) (mp->b_rptr + p->DEST_offset);
	anum = (p->DEST_length - sizeof(a->port)) / sizeof(a->addr[0]);
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length || !anum
	    || p->DEST_length != sizeof(a->port) + anum * sizeof(a->addr[0]) || !a->port)
		goto badaddr;
	if (sp->cred.cr_uid != 0 && a->port < 1024)
		goto access;
	if (p->QOS_length) {
		if (q->i_streams != -1UL)
			sp->max_istr = q->i_streams ? q->i_streams : 1;
		if (q->o_streams != -1UL)
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
	if ((err = sctp_conn_req(sp, a->port, a->addr, anum, mp->b_cont)))
		goto error;
	return (QR_TRIMMED);
      access:
	seldom();
	err = NACCESS;
	goto error;		/* no permission for requested address */
      badaddr:
	seldom();
	err = NBADADDR;
	goto error;		/* addresss is unusable */
      badopt2:
	seldom();
	err = NBADOPT;
	goto error;		/* options are unusable */
      badqostype:
	seldom();
	err = NBADQOSTYPE;
	goto error;		/* QOS structure type not supported */
      badopt1:
	seldom();
	err = NBADOPT;
	goto error;		/* options are unusable */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid message format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_CONN_REQ, err);
}

/* 
 *  N_CONN_RES           1 - Accept previous connection indication
 *  -------------------------------------------------------------------------
 *  IMPLEMENTATION NOTES:- Sequence numbers are actually the address of the
 *  mblk which contains the COOKIE-ECHO chunk and contains the cookie as a
 *  connection indication.  To find if a particular sequence number is valid,
 *  we walk the connection indication queue looking for a mblk with the same
 *  address as the sequence number.  Sequence numbers are only valid on the
 *  stream for which the connection indication is queued.
 */
STATIC mblk_t *
n_seq_check(sctp_t * sp, ulong seq)
{
	mblk_t *mp = bufq_head(&sp->conq);
	for (; mp && mp != (mblk_t *) seq; mp = mp->b_next) ;
	usual(mp);
	return (mp);
}

sctp_t *sctp_n_list;
STATIC sctp_t *
n_tok_check(sctp_t * sp, ulong tok)
{
	sctp_t *ap;
	queue_t *aq = (queue_t *) tok;
	for (ap = sctp_n_list; ap && ap->rq != aq; ap = ap->next) ;
	usual(ap);
	return (ap);
}
STATIC int
n_conn_res(sctp_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *cp;
	sctp_t *ap;
	N_conn_res_t *p = (N_conn_res_t *) mp->b_rptr;
	N_qos_sel_conn_sctp_t *q = (N_qos_sel_conn_sctp_t *) (mp->b_rptr + p->QOS_offset);
	if (sp->i_state != NS_WRES_CIND)
		goto outstate;
	sp->i_state = NS_WACK_CRES;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
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
		ap->i_state = NS_DATA_XFER;
		ap->flags &= ~(SCTP_FLAG_REC_CONF_OPT | SCTP_FLAG_EX_DATA_OPT);
		if (p->CONN_flags & REC_CONF_OPT)
			ap->flags |= SCTP_FLAG_REC_CONF_OPT;
		if (p->CONN_flags & EX_DATA_OPT)
			ap->flags |= SCTP_FLAG_EX_DATA_OPT;
		if ((err = sctp_conn_res(sp, cp, ap, mp->b_cont))) {
			ap->i_state = ap_oldstate;
			ap->flags = ap_oldflags;
			goto error;
		}
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return n_ok_ack(sp, N_CONN_RES, p->SEQ_number, p->TOKEN_value);
	}
      access:
	seldom();
	err = NACCESS;
	goto error;		/* no access to accepting queue */
      badtoken2:
	seldom();
	err = NBADTOKEN;
	goto error;		/* accepting queue is listening */
      badtoken1:
	seldom();
	err = NBADTOKEN;
	goto error;		/* accepting queue id is invalid */
      badseq:
	seldom();
	err = NBADSEQ;
	goto error;		/* connection ind reference is invalid */
      badopt2:
	seldom();
	err = NBADOPT;
	goto error;		/* quality of service options are bad */
      badqostype:
	seldom();
	err = NBADQOSTYPE;
	goto error;		/* quality of service options are bad */
      badopt1:
	seldom();
	err = NBADOPT;
	goto error;		/* quality of service options are bad */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_CONN_RES, err);
}

/* 
 *  N_DISCON_REQ         2 - NC disconnection request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_discon_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	N_discon_req_t *p = (N_discon_req_t *) mp->b_rptr;
	if (!
	    ((1 << sp->
	      i_state) & (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ |
			  NSF_WRES_RIND)))
		goto outstate;
	switch (sp->i_state) {
	case NS_WCON_CREQ:
		sp->i_state = NS_WACK_DREQ6;
		break;
	case NS_WRES_CIND:
		sp->i_state = NS_WACK_DREQ7;
		break;
	case NS_DATA_XFER:
		sp->i_state = NS_WACK_DREQ9;
		break;
	case NS_WCON_RREQ:
		sp->i_state = NS_WACK_DREQ10;
		break;
	case NS_WRES_RIND:
		sp->i_state = NS_WACK_DREQ11;
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
	/* XXX: What do we do with the disconnect reason? Nothing? */
	if ((err = sctp_discon_req(sp, cp)))
		goto error;
	return n_ok_ack(sp, N_DISCON_REQ, p->SEQ_number, 0);
      badseq:
	seldom();
	err = NBADSEQ;
	goto error;		/* connection ind reference is invalid */
      badaddr:
	seldom();
	err = NBADADDR;
	goto error;		/* responding address is inavlid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_DISCON_REQ, err);
}

/* 
 *  N_DATA_REQ           3 - Connection-Mode data transfer request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_error_reply(sctp_t * sp, int err)
{
	mblk_t *mp;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
	case 1:
	case 2:
		never();
		return (err);
	}
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}
STATIC int
n_write(sctp_t * sp, mblk_t *mp)
{
	int err;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 1;
		ulong more = 0;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)))
			goto error;
		return (QR_ABSORBED);	/* absorbed mp */
	}
      eproto:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);
	goto error;		/* ignore in idle state */
      error:
	seldom();
	return n_error_reply(sp, err);
}
STATIC int
n_data_req(sctp_t * sp, mblk_t *mp)
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
	{
		ulong ppi = q ? q->ppi : sp->ppi;
		ulong sid = q ? q->sid : sp->sid;
		ulong ord = 1;
		ulong more = p->DATA_xfer_flags & N_MORE_DATA_FLAG;
		ulong rcpt = p->DATA_xfer_flags & N_RC_FLAG;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_TRIMMED);
	}
      eproto:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return n_error_reply(sp, err);
}

/* 
 *  N_EXDATA_REQ         4 - Expedited data request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_exdata_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_qos_sel_data_sctp_t *q = NULL;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	N_exdata_req_t *p = (N_exdata_req_t *) mp->b_rptr;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mlen >= sizeof(*p) + sizeof(*q)) {
		q = (N_qos_sel_data_sctp_t *) (p + 1);
		if (q->n_qos_type != N_QOS_SEL_DATA_SCTP)
			q = NULL;
	}
	if (!((1 << sp->i_state) & (NSF_DATA_XFER | NSF_WRES_RIND)))
		goto eproto;
	{
		ulong ppi = q ? q->ppi : sp->ppi;
		ulong sid = q ? q->sid : sp->sid;
		ulong ord = 0;
		ulong more = 0;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		return (QR_TRIMMED);
	}
      eproto:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return n_error_reply(sp, err);
}

/* 
 *  N_INFO_REQ           5 - Information request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_info_req(sctp_t * sp, mblk_t *mp)
{
	(void) mp;
	return n_info_ack(sp);
}

/* 
 *  N_BIND_REQ           6 - Bind a NS user to network address
 *  -------------------------------------------------------------------------
 */
STATIC int
n_bind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	struct sctp_addr *a;
	size_t anum;
	N_bind_req_t *p = (N_bind_req_t *) mp->b_rptr;
	if (sp->i_state != NS_UNBND)
		goto outstate;
	sp->i_state = NS_WACK_BREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	a = (struct sctp_addr *) (mp->b_rptr + p->ADDR_offset);
	anum = (p->ADDR_length - sizeof(a->port)) / sizeof(a->addr[0]);
	if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
		goto badaddr;
	if (p->ADDR_length != sizeof(a->port) + anum * sizeof(a->addr[0]))
		goto badaddr;
	/* we don't allow wildcards just yet */
	if (!anum || (!a->port && !(a->port = sctp_get_port())))
		goto noaddr;
	if (sp->cred.cr_uid != 0 && a->port < 1024)
		goto access;
	if ((err = sctp_bind_req(sp, a->port, a->addr, anum, p->CONIND_number)))
		goto error;
	return n_bind_ack(sp);
      access:
	seldom();
	err = NACCESS;
	goto error;		/* no permission for requested address */
      noaddr:
	seldom();
	err = NNOADDR;
	goto error;		/* could not allocate address */
      badaddr:
	seldom();
	err = NBADADDR;
	goto error;		/* address is invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_BIND_REQ, err);
}

/* 
 *  N_UNBIND_REQ         7 - Unbind NS user from network address
 *  -------------------------------------------------------------------------
 */
STATIC int
n_unbind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_unbind_req_t *p = (N_unbind_req_t *) mp->b_rptr;
	(void) p;
	if (sp->i_state != NS_IDLE)
		goto outstate;
	sp->i_state = NS_WACK_UREQ;
	if ((err = sctp_unbind_req(sp)))
		goto error;
	return n_ok_ack(sp, N_UNBIND_REQ, 0, 0);
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return n_error_ack(sp, N_UNBIND_REQ, err);
}

/* 
 *  N_OPTMGMT_REQ        9 - Options management request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_optmgmt_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_optmgmt_req_t *p = (N_optmgmt_req_t *) mp->b_rptr;
	N_qos_sel_info_sctp_t *q = (N_qos_sel_info_sctp_t *) (mp->b_rptr + p->QOS_offset);
	if (sp->i_state == NS_IDLE)
		sp->i_state = NS_WACK_OPTREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (p->QOS_length) {
		if (mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length)
			goto badopt;
		if (q->n_qos_type != N_QOS_SEL_INFO_SCTP)
			goto badqostype;
		if (p->QOS_length != sizeof(*q))
			goto badopt2;
	}
	if (p->QOS_length) {
		if (q->i_streams != -1UL)
			sp->max_istr = q->i_streams ? q->i_streams : 1;
		if (q->o_streams != -1UL)
			sp->req_ostr = q->o_streams ? q->o_streams : 1;
		if (q->ppi != -1UL)
			sp->ppi = q->ppi;
		if (q->sid != -1UL)
			sp->sid = q->sid;
		if (q->max_inits != -1UL)
			sp->max_inits = q->max_inits;
		if (q->max_retrans != -1UL)
			sp->max_retrans = q->max_retrans;
		if (q->ck_life != -1UL)
			sp->ck_life = q->ck_life;
		if (q->ck_inc != -1UL)
			sp->ck_inc = q->ck_inc;
		if (q->hmac != -1UL)
			sp->hmac = q->hmac;
		if (q->throttle != -1UL)
			sp->throttle = q->throttle;
		if (q->max_sack != -1UL)
			sp->max_sack = q->max_sack;
		if (q->rto_ini != -1UL)
			sp->rto_ini = q->rto_ini;
		if (q->rto_min != -1UL)
			sp->rto_min = q->rto_min;
		if (q->rto_max != -1UL)
			sp->rto_max = q->rto_max;
		if (q->rtx_path != -1UL)
			sp->rtx_path = q->rtx_path;
		if (q->hb_itvl != -1UL)
			sp->hb_itvl = q->hb_itvl;
		if (q->options != -1UL)
			sp->options = q->options;
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		sp->flags |= SCTP_FLAG_DEFAULT_RC_SEL;
	else
		sp->flags &= ~SCTP_FLAG_DEFAULT_RC_SEL;
	return n_ok_ack(sp, N_OPTMGMT_REQ, 0, 0);
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
	return n_error_ack(sp, N_OPTMGMT_REQ, err);
}

/* 
 *  N_RESET_REQ         25 - NC reset request
 *  -------------------------------------------------------------------------
 */
STATIC int
n_reset_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_reset_req_t *p = (N_reset_req_t *) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (sp->i_state != NS_DATA_XFER)
		goto outstate;
	sp->i_state = NS_WCON_RREQ;
	if ((err = sctp_reset_req(sp)))
		goto error;
	return (0);
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, N_RESET_REQ, err);
}

/* 
 *  N_RESET_RES         27 - Reset processing accepted
 *  -------------------------------------------------------------------------
 */
STATIC int
n_reset_res(sctp_t * sp, mblk_t *mp)
{
	int err;
	N_reset_res_t *p = (N_reset_res_t *) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (sp->i_state == NS_IDLE)
		goto discard;
	if (sp->i_state != NS_WRES_RIND)
		goto outstate;
	sp->i_state = NS_WACK_RRES;
	if ((err = sctp_reset_res(sp)))
		goto error;
	return n_ok_ack(sp, N_RESET_RES, 0, 0);
      outstate:
	seldom();
	err = NOUTSTATE;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return n_error_ack(sp, N_RESET_RES, err);
}

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
STATIC int
sctp_n_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sctp_t *sp = (sctp_t *) q->q_ptr;
	ulong oldstate = sp->i_state;
	switch ((prim = *((ulong *) mp->b_rptr))) {
	case N_CONN_REQ:
		rtn = n_conn_req(sp, mp);
		break;
	case N_CONN_RES:
		rtn = n_conn_res(sp, mp);
		break;
	case N_DISCON_REQ:
		rtn = n_discon_req(sp, mp);
		break;
	case N_DATA_REQ:
		rtn = n_data_req(sp, mp);
		break;
	case N_EXDATA_REQ:
		rtn = n_exdata_req(sp, mp);
		break;
	case N_INFO_REQ:
		rtn = n_info_req(sp, mp);
		break;
	case N_BIND_REQ:
		rtn = n_bind_req(sp, mp);
		break;
	case N_UNBIND_REQ:
		rtn = n_unbind_req(sp, mp);
		break;
	case N_OPTMGMT_REQ:
		rtn = n_optmgmt_req(sp, mp);
		break;
	case N_RESET_REQ:
		rtn = n_reset_req(sp, mp);
		break;
	case N_RESET_RES:
		rtn = n_reset_res(sp, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		seldom();
		sp->i_state = oldstate;
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
sctp_n_w_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	return n_write(sp, mp);
}
STATIC int
sctp_n_r_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	return sctp_recv_msg(sp, mp);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_n_r_error(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	rare();
	return sctp_recv_err(sp, mp);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 *  This is complete flush handling in both directions.  Standard stuff.
 */
STATIC int
sctp_n_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag, const uint8_t oflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~mflag;
	}
	if (*mp->b_rptr & oflag) {
		queue_t *oq = q->q_other;
		if (*mp->b_rptr & FLUSHBAND)
			flushband(oq, mp->b_rptr[1], FLUSHALL);
		else
			flushq(oq, FLUSHALL);
		if (oq->q_next) {
			putnext(oq, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~oflag;
	}
	return (0);
}
STATIC int
sctp_n_w_flush(queue_t *q, mblk_t *mp)
{
	return sctp_n_m_flush(q, mp, FLUSHW, FLUSHR);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_n_r_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	putnext(q, mp);
	return (QR_ABSORBED);
}
STATIC int
sctp_n_w_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	return (-EOPNOTSUPP);
}

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
STATIC INLINE int
sctp_n_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_n_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sctp_n_w_proto(q, mp);
	case M_FLUSH:
		return sctp_n_w_flush(q, mp);
	default:
		return sctp_n_w_other(q, mp);
	}
}

/* 
 *  IP Read Message
 */
STATIC INLINE int
sctp_n_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_n_r_data(q, mp);
	case M_ERROR:
		return sctp_n_r_error(q, mp);
	default:
		return sctp_n_r_other(q, mp);
	}
}

/* 
 *  PUTQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_n_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type < QPCTL || q->q_count) {
		putq(q, mp);
		return (0);
	}
	if (!sctp_trylock(q)) {
		int rtn;
		switch ((rtn = proc(q, mp))) {
		case QR_DONE:
			freemsg(mp);
		case QR_ABSORBED:
			break;
		case QR_TRIMMED:
			freeb(mp);
			break;
		case QR_LOOP:
			if (!q->q_next) {
				qreply(q, mp);
				break;
			}
		case QR_PASSALONG:
			if (q->q_next) {
				putnext(q, mp);
				break;
			}
			rtn = -EOPNOTSUPP;
		default:
			ptrace(("ERROR: (q dropping) %d\n", rtn));
			freemsg(mp);
			break;
		case QR_DISABLE:
			putq(q, mp);
			rtn = 0;
			break;
		case QR_PASSFLOW:
			if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
				putnext(q, mp);
				break;
			}
		case -ENOBUFS:
		case -EBUSY:
		case -ENOMEM:
		case -EAGAIN:
			putq(q, mp);
			break;
		}
		sctp_unlock(q);
		return (rtn);
	} else {
		seldom();
		putq(q, mp);
		return (0);
	}
}

/* 
 *  SRVQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_n_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	if (!sctp_waitlock(q)) {
		int rtn = 0;
		mblk_t *mp;
		while ((mp = getq(q))) {
			switch ((rtn = proc(q, mp))) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				rtn = -EOPNOTSUPP;
			default:
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				ptrace(("ERROR: (q disabling) %d\n", rtn));
				noenable(q);
				putbq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must schedule bufcall */
			case -EBUSY:	/* proc must fail canput */
			case -ENOMEM:	/* proc must schedule re-enable */
			case -EAGAIN:	/* proc must schedule re-enable */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("ERROR: (q stalled) %d\n", rtn));
					putbq(q, mp);
					break;
				}
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					mp->b_band = 255;
					putq(q, mp);
					break;
				}
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			}
			break;
		}
		sctp_unlock(q);
		return (rtn);
	}
	return (-EAGAIN);
}
STATIC int
sctp_n_rput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_n_putq(q, mp, &sctp_n_r_prim);
}
STATIC int
sctp_n_rsrv(queue_t *q)
{
	return (int) sctp_n_srvq(q, &sctp_n_r_prim);
}
STATIC int
sctp_n_wput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_n_putq(q, mp, &sctp_n_w_prim);
}
STATIC int
sctp_n_wsrv(queue_t *q)
{
	return (int) sctp_n_srvq(q, &sctp_n_w_prim);
}

/* 
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
sctp_t *sctp_n_list = NULL;
STATIC int
sctp_n_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	sctp_t *sp, **spp = &sctp_n_list;
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
	for (; *spp && (*spp)->cmajor < cmajor; spp = &(*spp)->next) ;
	for (; *spp && cminor <= SCTP_N_CMINORS; spp = &(*spp)->next) {
		ushort dminor = (*spp)->cminor;
		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag != CLONEOPEN) {
				rare();
				return (ENXIO);	/* requested device in use */
			}
			cminor++;
		}
	}
	if (cminor > SCTP_N_CMINORS) {
		rare();
		return (ENXIO);
	}
	*devp = makedevice(cmajor, cminor);
	if (!(sp = sctp_alloc_priv(q, spp, cmajor, cminor, &n_ops))) {
		rare();
		return (ENOMEM);
	}
	return (0);
}
STATIC int
sctp_n_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sctp_free_priv(q);
	return (0);
}

/* 
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
static void
sctp_n_init(void)
{
	int cmajor;
	if ((cmajor =
	     lis_register_strdev(SCTP_N_CMAJOR_0, &sctp_n_info, SCTP_N_CMINORS,
				 sctp_n_minfo.mi_idname)) < 0) {
		sctp_n_minfo.mi_idnum = 0;
		rare();
		cmn_err(CE_NOTE, "sctp: couldn't register driver\n");
		return;
	}
	sctp_n_minfo.mi_idnum = cmajor;
}
static void
sctp_n_term(void)
{
	if (sctp_n_minfo.mi_idnum) {
		if ((sctp_n_minfo.mi_idnum = lis_unregister_strdev(sctp_n_minfo.mi_idnum))) {
			sctp_n_minfo.mi_idnum = 0;
			rare();
			cmn_err(CE_WARN, "sctp: couldn't unregister driver!\n");
		}
	}
}

#define SCTP_T_CMINORS 255
/* 
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 *  This driver defines two user interfaces: one NPI, the other TPI.
 */
STATIC struct module_info sctp_t_minfo = {
	mi_idnum:0,			/* Module ID number */
	mi_idname:"sctp",		/* Module name */
	mi_minpsz:0,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1 << 15,		/* Hi water mark */
	mi_lowat:1 << 10,		/* Lo water mark */
};
STATIC int sctp_t_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sctp_t_close(queue_t *, int, cred_t *);
STATIC int sctp_t_rput(queue_t *, mblk_t *);
STATIC int sctp_t_rsrv(queue_t *);
STATIC struct qinit sctp_t_rinit = {
	qi_putp:sctp_t_rput,		/* Read put (msg from below) */
	qi_srvp:sctp_t_rsrv,		/* Read queue service */
	qi_qopen:sctp_t_open,		/* Each open */
	qi_qclose:sctp_t_close,		/* Last close */
	qi_minfo:&sctp_t_minfo,		/* Information */
};
STATIC int sctp_t_wput(queue_t *, mblk_t *);
STATIC int sctp_t_wsrv(queue_t *);
STATIC struct qinit sctp_t_winit = {
	qi_putp:sctp_t_wput,		/* Write put (msg from above) */
	qi_srvp:sctp_t_wsrv,		/* Write queue service */
	qi_minfo:&sctp_t_minfo,		/* Information */
};
STATIC struct streamtab sctp_t_info = {
	st_rdinit:&sctp_t_rinit,	/* Upper read queue */
	st_wrinit:&sctp_t_winit,	/* Upper write queue */
};
#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
/* 
 *  =========================================================================
 *
 *  Option handling functions
 *
 *  =========================================================================
 */
typedef struct sctp_opts {
	uint flags;
	struct t_opthdr *bcast;		/* T_IP_BROADCAST */
	struct t_opthdr *norte;		/* T_IP_DONTROUTE */
	struct t_opthdr *opts;		/* T_IP_OPTIONS */
	struct t_opthdr *reuse;		/* T_IP_REUSEADDR */
	struct t_opthdr *tos;		/* T_IP_TOS */
	struct t_opthdr *ttl;		/* T_IP_TTL */
	struct t_opthdr *nd;		/* T_SCTP_NODELAY */
	struct t_opthdr *cork;		/* T_SCTP_CORK */
	struct t_opthdr *ppi;		/* T_SCTP_PPI */
	struct t_opthdr *sid;		/* T_SCTP_SID */
	struct t_opthdr *ssn;		/* T_SCTP_SSN */
	struct t_opthdr *tsn;		/* T_SCTP_TSN */
	struct t_opthdr *ropt;		/* T_SCTP_RECVOPT */
	struct t_opthdr *cklife;	/* T_SCTP_COOKIE_LIFE */
	struct t_opthdr *sack;		/* T_SCTP_SACK_DELAY */
	struct t_opthdr *path;		/* T_SCTP_PATH_MAX_RETRANS */
	struct t_opthdr *assoc;		/* T_SCTP_ASSOC_MAX_RETRANS */
	struct t_opthdr *init;		/* T_SCTP_MAX_INIT_RETRIES */
	struct t_opthdr *hbitvl;	/* T_SCTP_HEARTBEAT_ITVL */
	struct t_opthdr *rtoinit;	/* T_SCTP_RTO_INITIAL */
	struct t_opthdr *rtomin;	/* T_SCTP_RTO_MIN */
	struct t_opthdr *rtomax;	/* T_SCTP_RTO_MAX */
	struct t_opthdr *ostr;		/* T_SCTP_OSTREAMS */
	struct t_opthdr *istr;		/* T_SCTP_ISTREAMS */
	struct t_opthdr *ckinc;		/* T_SCTP_COOKIE_INC */
	struct t_opthdr *titvl;		/* T_SCTP_THROTTLE_ITVL */
	struct t_opthdr *hmac;		/* T_SCTP_MAC_TYPE */
	struct t_opthdr *hb;		/* T_SCTP_HB */
	struct t_opthdr *rto;		/* T_SCTP_RTO */
	struct t_opthdr *mseg;		/* T_SCTP_MAXSEG */
	struct t_opthdr *status;	/* T_SCTP_STATUS */
	struct t_opthdr *debug;		/* T_SCTP_DEBUG */
} sctp_opts_t;
/* 
 *  Size and Build Default options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Either builds the default options requested or builds all default options.
 */
STATIC size_t
sctp_default_opts_size(sctp_t * sp, sctp_opts_t * ops)
{
	size_t len = 0;
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (!ops || ops->bcast) {
		len += olen;
	}
	if (!ops || ops->norte) {
		len += olen;
	}
	if (!ops || ops->opts) {
		len += olen;
	}
	if (!ops || ops->reuse) {
		len += olen;
	}
	if (!ops || ops->tos) {
		len += olen;
	}
	if (!ops || ops->ttl) {
		len += olen;
	}
	if (!ops || ops->nd) {
		len += olen;
	}
	if (!ops || ops->cork) {
		len += olen;
	}
	if (!ops || ops->ppi) {
		len += olen;
	}
	if (!ops || ops->sid) {
		len += olen;
	}
	if (!ops || ops->ropt) {
		len += olen;
	}
	if (!ops || ops->cklife) {
		len += olen;
	}
	if (!ops || ops->sack) {
		len += olen;
	}
	if (!ops || ops->path) {
		len += olen;
	}
	if (!ops || ops->assoc) {
		len += olen;
	}
	if (!ops || ops->init) {
		len += olen;
	}
	if (!ops || ops->hbitvl) {
		len += olen;
	}
	if (!ops || ops->rtoinit) {
		len += olen;
	}
	if (!ops || ops->rtomin) {
		len += olen;
	}
	if (!ops || ops->rtomax) {
		len += olen;
	}
	if (!ops || ops->ostr) {
		len += olen;
	}
	if (!ops || ops->istr) {
		len += olen;
	}
	if (!ops || ops->ckinc) {
		len += olen;
	}
	if (!ops || ops->titvl) {
		len += olen;
	}
	if (!ops || ops->hmac) {
		len += olen;
	}
	if (!ops || ops->mseg) {
		len += olen;
	}
	if (!ops || ops->debug) {
		len += olen;
	}
	return (len);
}
STATIC void
sctp_build_default_opts(sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	struct t_opthdr *oh;
	const size_t hlen = sizeof(*oh);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (!ops || ops->bcast) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_BROADCAST;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->norte) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_DONTROUTE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->opts) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 0;
	}
	if (!ops || ops->reuse) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_REUSEADDR;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->tos) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_ip_tos;
	}
	if (!ops || ops->ttl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_ip_ttl;
	}
	if (!ops || ops->nd) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_NODELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_YES;
	}
	if (!ops || ops->cork) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CORK;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->ppi) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_ppi;
	}
	if (!ops || ops->sid) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_sid;
	}
	/* note ssn and tsn are per-packet */
	if (!ops || ops->ropt) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RECVOPT;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = T_NO;
	}
	if (!ops || ops->cklife) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_LIFE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_valid_cookie_life * 1000 / HZ;
	}
	if (!ops || ops->sack) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_DELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_max_sack_delay * 1000 / HZ;
	}
	if (!ops || ops->path) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PATH_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_path_max_retrans;
	}
	if (!ops || ops->assoc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ASSOC_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) * p)++ = sctp_default_assoc_max_retrans;
	}
	if (!ops || ops->init) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_INIT_RETRIES;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_max_init_retries;
	}
	if (!ops || ops->hbitvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HEARTBEAT_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_heartbeat_itvl * 1000 / HZ;
	}
	if (!ops || ops->rtoinit) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_INITIAL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_rto_initial * 1000 / HZ;
	}
	if (!ops || ops->rtomin) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MIN;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_rto_min * 1000 / HZ;
	}
	if (!ops || ops->rtomax) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MAX;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_rto_max * 1000 / HZ;
	}
	if (!ops || ops->ostr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_req_ostreams;
	}
	if (!ops || ops->istr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_max_istreams;
	}
	if (!ops || ops->ckinc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_INC;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_cookie_inc * 1000 / HZ;
	}
	if (!ops || ops->titvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_THROTTLE_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_throttle_itvl * 1000 / HZ;
	}
	if (!ops || ops->hmac) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAC_TYPE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sctp_default_mac_type;
	}
	if (!ops || ops->mseg) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAXSEG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 576;
	}
	if (!ops || ops->debug) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEBUG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 0;
	}
}

/* 
 *  Size and Build Current options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Either builds the current options requested or builds all current options.
 */
STATIC size_t
sctp_current_opts_size(sctp_t * sp, sctp_opts_t * ops)
{
	size_t len = 0;
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (!ops || ops->bcast) {
		len += olen;
	}
	if (!ops || ops->norte) {
		len += olen;
	}
	if (!ops || ops->opts) {
		len += olen;
	}
	if (!ops || ops->reuse) {
		len += olen;
	}
	if (!ops || ops->tos) {
		len += olen;
	}
	if (!ops || ops->ttl) {
		len += olen;
	}
	if (!ops || ops->nd) {
		len += olen;
	}
	if (!ops || ops->cork) {
		len += olen;
	}
	if (!ops || ops->ppi) {
		len += olen;
	}
	if (!ops || ops->sid) {
		len += olen;
	}
	if (!ops || ops->ropt) {
		len += olen;
	}
	if (!ops || ops->cklife) {
		len += olen;
	}
	if (!ops || ops->sack) {
		len += olen;
	}
	if (!ops || ops->path) {
		len += olen;
	}
	if (!ops || ops->assoc) {
		len += olen;
	}
	if (!ops || ops->init) {
		len += olen;
	}
	if (!ops || ops->hbitvl) {
		len += olen;
	}
	if (!ops || ops->rtoinit) {
		len += olen;
	}
	if (!ops || ops->rtomin) {
		len += olen;
	}
	if (!ops || ops->rtomax) {
		len += olen;
	}
	if (!ops || ops->ostr) {
		len += olen;
	}
	if (!ops || ops->istr) {
		len += olen;
	}
	if (!ops || ops->ckinc) {
		len += olen;
	}
	if (!ops || ops->titvl) {
		len += olen;
	}
	if (!ops || ops->hmac) {
		len += olen;
	}
	if (!ops || ops->mseg) {
		len += olen;
	}
	if (!ops || ops->debug) {
		len += olen;
	}
	if (!ops || ops->hb) {
		size_t n = ops ? (ops->hb->len - hlen) / sizeof(t_sctp_hb_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n)
			len += hlen + n * sizeof(t_sctp_hb_t);
	}
	if (!ops || ops->rto) {
		size_t n = ops ? (ops->rto->len - hlen) / sizeof(t_sctp_rto_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n)
			len += hlen + n * sizeof(t_sctp_rto_t);
	}
	if (!ops || ops->status) {
		size_t n =
		    ops ? (ops->status->len - hlen -
			   sizeof(t_sctp_status_t)) / sizeof(t_sctp_dest_status_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n)
			len += hlen + sizeof(t_sctp_status_t) + n * sizeof(t_sctp_dest_status_t);
	}
	return (len);
}
STATIC void
sctp_build_current_opts(sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	struct t_opthdr *oh;
	const size_t hlen = sizeof(*oh);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (!ops || ops->bcast) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_BROADCAST;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_broadcast ? T_YES : T_NO;
	}
	if (!ops || ops->norte) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_DONTROUTE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_dontroute ? T_YES : T_NO;
	}
	if (!ops || ops->opts) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = 0;
	}
	if (!ops || ops->reuse) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_REUSEADDR;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->i_flags & TF_IP_REUSEADDR ? T_YES : T_NO;
	}
	if (!ops || ops->tos) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_tos;
	}
	if (!ops || ops->ttl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ip_ttl;
	}
	if (!ops || ops->nd) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_NODELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->options & SCTP_OPTION_NAGLE ? T_NO : T_YES;
	}
	if (!ops || ops->cork) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CORK;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->options & SCTP_OPTION_CORK ? T_YES : T_NO;
	}
	if (!ops || ops->ppi) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->ppi;
	}
	if (!ops || ops->sid) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->sid;
	}
	/* note ssn and tsn are per-packet */
	if (!ops || ops->ropt) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RECVOPT;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->i_flags & TF_SCTP_RECVOPT ? T_YES : T_NO;
	}
	if (!ops || ops->cklife) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_LIFE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->ck_life * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->sack) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_DELAY;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->max_sack * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->path) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PATH_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->rtx_path;
	}
	if (!ops || ops->assoc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ASSOC_MAX_RETRANS;
		oh->status = T_SUCCESS;
		*((t_uscalar_t *) * p)++ = sp->max_retrans;
	}
	if (!ops || ops->init) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_INIT_RETRIES;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->max_inits;
	}
	if (!ops || ops->hbitvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HEARTBEAT_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->hb_itvl * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->rtoinit) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_INITIAL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->rto_ini * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->rtomin) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MIN;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->rto_min * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->rtomax) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MAX;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->rto_max * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->ostr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->status = T_SUCCESS;
		if ((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL))
			*((t_scalar_t *) * p)++ = sp->n_ostr;
		else
			*((t_scalar_t *) * p)++ = sp->req_ostr;
	}
	if (!ops || ops->istr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->status = T_SUCCESS;
		if ((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL | TSF_WREQ_ORDREL))
			*((t_scalar_t *) * p)++ = sp->n_istr;
		else
			*((t_scalar_t *) * p)++ = sp->max_istr;
	}
	if (!ops || ops->ckinc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_INC;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->ck_inc * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->titvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_THROTTLE_ITVL;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = (sp->throttle * 1000 + HZ - 1) / HZ;
	}
	if (!ops || ops->hmac) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAC_TYPE;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->hmac;
	}
	if (!ops || ops->mseg) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAXSEG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->pmtu;
	}
	if (!ops || ops->debug) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEBUG;
		oh->status = T_SUCCESS;
		*((t_scalar_t *) * p)++ = sp->options;
	}
	if (!ops || ops->hb) {
		size_t n = ops ? (ops->hb->len - hlen) / sizeof(t_sctp_hb_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n) {
			struct sctp_daddr *sd;
			oh = ((struct t_opthdr *) *p)++;
			oh->len = sizeof(*oh) + n * sizeof(t_sctp_hb_t);
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_HB;
			oh->status = T_SUCCESS;
			for (sd = sp->daddr; n && sd; n--, sd = sd->next) {
				t_sctp_hb_t *hb = ((t_sctp_hb_t *) * p)++;
				hb->hb_dest = sd->daddr;
				hb->hb_onoff = sd->hb_onoff;
				hb->hb_itvl = (sd->hb_itvl * 1000 + HZ - 1) / HZ;
			}
		}
	}
	if (!ops || ops->rto) {
		size_t n = ops ? (ops->rto->len - hlen) / sizeof(t_sctp_rto_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n) {
			struct sctp_daddr *sd;
			oh = ((struct t_opthdr *) *p)++;
			oh->len = sizeof(*oh) + n * sizeof(t_sctp_rto_t);
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_RTO;
			oh->status = T_SUCCESS;
			for (sd = sp->daddr; n && sd; n--, sd = sd->next) {
				t_sctp_rto_t *rto = ((t_sctp_rto_t *) * p)++;
				rto->rto_dest = sd->daddr;
				rto->rto_initial = (sp->rto_ini * 1000 + HZ - 1) / HZ;
				rto->rto_min = (sd->rto_min * 1000 + HZ - 1) / HZ;
				rto->rto_max = (sd->rto_max * 1000 + HZ - 1) / HZ;
				rto->max_retrans = sd->max_retrans;
			}
		}
	}
	if (!ops || ops->status) {
		size_t n =
		    ops ? (ops->status->len - hlen -
			   sizeof(t_sctp_status_t)) / sizeof(t_sctp_dest_status_t) : sp->danum;
		if (!n)
			n = sp->danum;
		if (n) {
			struct sctp_daddr *sd;
			t_sctp_status_t *curr;
			oh = ((struct t_opthdr *) *p)++;
			oh->len =
			    sizeof(*oh) + sizeof(t_sctp_status_t) +
			    n * sizeof(t_sctp_dest_status_t);
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_STATUS;
			oh->status = T_SUCCESS;
			curr = ((t_sctp_status_t *) * p)++;
			curr->curr_rwnd = sp->p_rwnd - sp->in_flight;
			curr->curr_rbuf = sp->a_rwnd;
			curr->curr_nrep = n;
			for (sd = sp->daddr; n && sd; n--, sd = sd->next) {
				t_sctp_dest_status_t *dest = ((t_sctp_dest_status_t *) * p)++;
				dest->dest_addr = sd->daddr;
				dest->dest_cwnd = sd->cwnd;
				dest->dest_unack = sd->in_flight;
				dest->dest_srtt = (sd->srtt * 1000 + HZ - 1) / HZ;
				dest->dest_rvar = sd->rttvar;
				dest->dest_rto = (sd->rto * 1000 + HZ - 1) / HZ;
				dest->dest_sst = sd->ssthresh;
			}
		}
	}
}

/* 
 *  Size and Build Checked or Negotiated options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Builds the checked or negotiated options.
 */
STATIC size_t
sctp_set_opts_size(sctp_t * sp, sctp_opts_t * ops)
{
	size_t len = 0;
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (ops->bcast) {
		len += olen;
	}
	if (ops->norte) {
		len += olen;
	}
	if (ops->opts) {
		len += olen;
	}
	if (ops->reuse) {
		len += olen;
	}
	if (ops->tos) {
		len += olen;
	}
	if (ops->ttl) {
		len += olen;
	}
	if (ops->nd) {
		len += olen;
	}
	if (ops->cork) {
		len += olen;
	}
	if (ops->ppi) {
		len += olen;
	}
	if (ops->sid) {
		len += olen;
	}
	if (ops->ssn) {
		len += olen;
	}
	if (ops->tsn) {
		len += olen;
	}
	if (ops->ropt) {
		len += olen;
	}
	if (ops->cklife) {
		len += olen;
	}
	if (ops->sack) {
		len += olen;
	}
	if (ops->path) {
		len += olen;
	}
	if (ops->assoc) {
		len += olen;
	}
	if (ops->init) {
		len += olen;
	}
	if (ops->hbitvl) {
		len += olen;
	}
	if (ops->rtoinit) {
		len += olen;
	}
	if (ops->rtomin) {
		len += olen;
	}
	if (ops->rtomax) {
		len += olen;
	}
	if (ops->ostr) {
		len += olen;
	}
	if (ops->istr) {
		len += olen;
	}
	if (ops->ckinc) {
		len += olen;
	}
	if (ops->titvl) {
		len += olen;
	}
	if (ops->hmac) {
		len += olen;
	}
	if (ops->mseg) {
		len += olen;
	}
	if (ops->debug) {
		len += olen;
	}
	if (ops->hb) {
		len += ops->hb->len;
	}
	if (ops->rto) {
		len += ops->rto->len;
	}
	if (ops->status) {
		len += ops->status->len;
	}
	return (len);
}
STATIC void
sctp_build_set_opts(sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	struct t_opthdr *oh;
	const size_t hlen = sizeof(*oh);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (ops->bcast) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_BROADCAST;
		oh->status = ops->flags & TF_IP_BROADCAST ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->bcast + 1));
	}
	if (ops->norte) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_DONTROUTE;
		oh->status = ops->flags & TF_IP_DONTROUTE ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->norte + 1));
	}
	if (ops->opts) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_OPTIONS;
		oh->status = ops->flags & TF_IP_OPTIONS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->opts + 1));
	}
	if (ops->reuse) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_REUSEADDR;
		oh->status = ops->flags & TF_IP_REUSEADDR ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->reuse + 1));
	}
	if (ops->tos) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TOS;
		oh->status = ops->flags & TF_IP_TOS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->tos + 1));
	}
	if (ops->ttl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_IP;
		oh->name = T_IP_TTL;
		oh->status = ops->flags & TF_IP_TTL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ttl + 1));
	}
	if (ops->nd) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_NODELAY;
		oh->status = ops->flags & TF_SCTP_NODELAY ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->nd + 1));
	}
	if (ops->cork) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_CORK;
		oh->status = ops->flags & TF_SCTP_CORK ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->cork + 1));
	}
	if (ops->ppi) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PPI;
		oh->status = ops->flags & TF_SCTP_PPI ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ppi + 1));
	}
	if (ops->sid) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SID;
		oh->status = ops->flags & TF_SCTP_SID ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->sid + 1));
	}
	if (ops->ssn) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SSN;
		oh->status = ops->flags & TF_SCTP_SSN ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ssn + 1));
	}
	if (ops->tsn) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_TSN;
		oh->status = ops->flags & TF_SCTP_TSN ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->tsn + 1));
	}
	if (ops->ropt) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RECVOPT;
		oh->status = ops->flags & TF_SCTP_RECVOPT ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ropt + 1));
	}
	if (ops->cklife) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_LIFE;
		oh->status = ops->flags & TF_SCTP_COOKIE_LIFE ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->cklife + 1));
	}
	if (ops->sack) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_SACK_DELAY;
		oh->status = ops->flags & TF_SCTP_SACK_DELAY ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->sack + 1));
	}
	if (ops->path) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_PATH_MAX_RETRANS;
		oh->status = ops->flags & TF_SCTP_PATH_MAX_RETRANS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->path + 1));
	}
	if (ops->assoc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ASSOC_MAX_RETRANS;
		oh->status = ops->flags & TF_SCTP_ASSOC_MAX_RETRANS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->assoc + 1));
	}
	if (ops->init) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAX_INIT_RETRIES;
		oh->status = ops->flags & TF_SCTP_MAX_INIT_RETRIES ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->init + 1));
	}
	if (ops->hbitvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HEARTBEAT_ITVL;
		oh->status = ops->flags & TF_SCTP_HEARTBEAT_ITVL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->hbitvl + 1));
	}
	if (ops->rtoinit) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_INITIAL;
		oh->status = ops->flags & TF_SCTP_RTO_INITIAL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->rtoinit + 1));
	}
	if (ops->rtomin) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MIN;
		oh->status = ops->flags & TF_SCTP_RTO_MIN ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->rtomin + 1));
	}
	if (ops->rtomax) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO_MAX;
		oh->status = ops->flags & TF_SCTP_RTO_MAX ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->rtomax + 1));
	}
	if (ops->ostr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_OSTREAMS;
		oh->status = ops->flags & TF_SCTP_OSTREAMS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ostr + 1));
	}
	if (ops->istr) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_ISTREAMS;
		oh->status = ops->flags & TF_SCTP_ISTREAMS ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->istr + 1));
	}
	if (ops->ckinc) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_COOKIE_INC;
		oh->status = ops->flags & TF_SCTP_COOKIE_INC ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->ckinc + 1));
	}
	if (ops->titvl) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_THROTTLE_ITVL;
		oh->status = ops->flags & TF_SCTP_THROTTLE_ITVL ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->titvl + 1));
	}
	if (ops->hmac) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAC_TYPE;
		oh->status = ops->flags & TF_SCTP_MAC_TYPE ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->hmac + 1));
	}
	if (ops->mseg) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_MAXSEG;
		oh->status = ops->flags & TF_SCTP_MAXSEG ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->mseg + 1));
	}
	if (ops->debug) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = olen;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_DEBUG;
		oh->status = ops->flags & TF_SCTP_DEBUG ? T_SUCCESS : T_FAILURE;
		*((t_scalar_t *) * p)++ = *((t_scalar_t *) (ops->debug + 1));
	}
	if (ops->hb) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = ops->hb->len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_HB;
		oh->status = ops->flags & TF_SCTP_HB ? T_SUCCESS : T_FAILURE;
		bcopy(ops->hb + 1, *p, ops->hb->len - sizeof(*oh));
		*p += ops->hb->len - sizeof(*oh);
	}
	if (ops->rto) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = ops->rto->len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_RTO;
		oh->status = ops->flags & TF_SCTP_RTO ? T_SUCCESS : T_FAILURE;
		bcopy(ops->rto + 1, *p, ops->rto->len - sizeof(*oh));
		*p += ops->rto->len - sizeof(*oh);
	}
	if (ops->status) {
		oh = ((struct t_opthdr *) *p)++;
		oh->len = ops->status->len;
		oh->level = T_INET_SCTP;
		oh->name = T_SCTP_STATUS;
		oh->status = ops->flags & TF_SCTP_STATUS ? T_SUCCESS : T_FAILURE;
		bcopy(ops->status + 1, *p, ops->status->len - sizeof(*oh));
		*p += ops->status->len - sizeof(*oh);
	}
}

/* 
 *  Size and Build options.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC size_t
sctp_opts_size(ulong flags, sctp_t * sp, sctp_opts_t * ops)
{
	switch (flags) {
	case T_CHECK:
	case T_NEGOTIATE:
		if (ops)
			return sctp_set_opts_size(sp, ops);
	case T_DEFAULT:
		return sctp_default_opts_size(sp, ops);
	case T_CURRENT:
		return sctp_current_opts_size(sp, ops);
	}
	return (0);
}
STATIC void
sctp_build_opts(ulong flags, sctp_t * sp, sctp_opts_t * ops, unsigned char **p)
{
	switch (flags) {
	case T_CHECK:
	case T_NEGOTIATE:
		if (ops)
			return sctp_build_set_opts(sp, ops, p);
	case T_DEFAULT:
		return sctp_build_default_opts(sp, ops, p);
	case T_CURRENT:
		return sctp_build_current_opts(sp, ops, p);
	}
	return;
}

/* 
 *  Parse options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int
sctp_parse_opts(sctp_opts_t * ops, unsigned char *opt_ptr, size_t opt_len)
{
	struct t_opthdr *oh = (struct t_opthdr *) opt_ptr;
	unsigned char *opt_end = opt_ptr + opt_len;
	for (; opt_ptr + sizeof(*oh) <= opt_end && oh->len >= sizeof(*oh)
	     && opt_ptr + oh->len <= opt_end; opt_ptr += oh->len, oh = (struct t_opthdr *) opt_ptr) {
		switch (oh->level) {
		case T_INET_IP:
			switch (oh->name) {
			case T_IP_BROADCAST:
				ops->bcast = oh;
				continue;
			case T_IP_DONTROUTE:
				ops->norte = oh;
				continue;
			case T_IP_OPTIONS:
				ops->opts = oh;
				continue;
			case T_IP_REUSEADDR:
				ops->reuse = oh;
				continue;
			case T_IP_TOS:
				ops->tos = oh;
				continue;
			case T_IP_TTL:
				ops->ttl = oh;
				continue;
			}
			break;
		case T_INET_SCTP:
			switch (oh->name) {
			case T_SCTP_NODELAY:
				ops->nd = oh;
				continue;
			case T_SCTP_CORK:
				ops->cork = oh;
				continue;
			case T_SCTP_PPI:
				ops->ppi = oh;
				continue;
			case T_SCTP_SID:
				ops->sid = oh;
				continue;
			case T_SCTP_SSN:
				ops->ssn = oh;
				continue;
			case T_SCTP_TSN:
				ops->tsn = oh;
				continue;
			case T_SCTP_RECVOPT:
				ops->ropt = oh;
				continue;
			case T_SCTP_COOKIE_LIFE:
				ops->cklife = oh;
				continue;
			case T_SCTP_SACK_DELAY:
				ops->sack = oh;
				continue;
			case T_SCTP_PATH_MAX_RETRANS:
				ops->path = oh;
				continue;
			case T_SCTP_ASSOC_MAX_RETRANS:
				ops->assoc = oh;
				continue;
			case T_SCTP_MAX_INIT_RETRIES:
				ops->init = oh;
				continue;
			case T_SCTP_HEARTBEAT_ITVL:
				ops->hbitvl = oh;
				continue;
			case T_SCTP_RTO_INITIAL:
				ops->rtoinit = oh;
				continue;
			case T_SCTP_RTO_MIN:
				ops->rtomin = oh;
				continue;
			case T_SCTP_RTO_MAX:
				ops->rtomax = oh;
				continue;
			case T_SCTP_OSTREAMS:
				ops->ostr = oh;
				continue;
			case T_SCTP_ISTREAMS:
				ops->istr = oh;
				continue;
			case T_SCTP_COOKIE_INC:
				ops->ckinc = oh;
				continue;
			case T_SCTP_THROTTLE_ITVL:
				ops->titvl = oh;
				continue;
			case T_SCTP_MAC_TYPE:
				ops->hmac = oh;
				continue;
			case T_SCTP_MAXSEG:
				ops->mseg = oh;
				continue;
			case T_SCTP_DEBUG:
				ops->debug = oh;
				continue;
			case T_SCTP_HB:
				ops->hb = oh;
				continue;
			case T_SCTP_RTO:
				ops->rto = oh;
				continue;
			case T_SCTP_STATUS:
				ops->status = oh;
				continue;
			}
			break;
		}
		return (TBADOPT);
	}
	if (opt_ptr != opt_end)
		return (TBADOPT);
	return (0);
}

/* 
 *  Negotiate options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_negotiate_opts(sctp_t * sp, sctp_opts_t * ops)
{
	const size_t hlen = sizeof(struct t_opthdr);
	const size_t olen = hlen + sizeof(t_scalar_t);
	if (!ops)
		return;
	if (ops->bcast) {
		if (ops->bcast->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->bcast + 1);
			switch (*val) {
			case T_YES:
				sp->ip_broadcast = 1;
				break;
			case T_NO:
				sp->ip_broadcast = 0;
				break;
			}
			*val = sp->ip_broadcast;
			ops->flags |= TF_IP_BROADCAST;
		}
	}
	if (ops->norte) {
		if (ops->norte->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->norte + 1);
			switch (*val) {
			case T_YES:
				sp->ip_dontroute = 1;
				break;
			case T_NO:
				sp->ip_dontroute = 0;
				break;
			}
			*val = sp->ip_dontroute;
			ops->flags |= TF_IP_DONTROUTE;
		}
	}
	if (ops->opts) {
		/* not supported yet */
	}
	if (ops->reuse) {
		/* not supported yet */
	}
	if (ops->tos) {
		if (ops->tos->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->tos + 1);
			sp->ip_tos = *val & 0xff;
			*val = sp->ip_tos;
			ops->flags |= TF_IP_TOS;
		}
	}
	if (ops->ttl) {
		if (ops->ttl->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ttl + 1);
			sp->ip_ttl = *val & 0xff;
			*val = sp->ip_ttl;
			ops->flags |= TF_IP_TTL;
		}
	}
	if (ops->nd) {
		if (ops->nd->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->nd + 1);
			switch (*val) {
			case T_YES:
				sp->options &= ~SCTP_OPTION_NAGLE;
				break;
			case T_NO:
				sp->options |= SCTP_OPTION_NAGLE;
				break;
				break;
			}
			*val = (sp->options & SCTP_OPTION_NAGLE) ? T_NO : T_YES;
			ops->flags |= TF_SCTP_NODELAY;
		}
	}
	if (ops->cork) {
		if (ops->cork->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->cork + 1);
			switch (*val) {
			case T_YES:
				sp->options |= SCTP_OPTION_CORK;
				break;
			case T_NO:
				sp->options &= ~SCTP_OPTION_CORK;
				break;
			}
			*val = (sp->options & SCTP_OPTION_CORK) ? T_YES : T_NO;
			ops->flags |= TF_SCTP_CORK;
		}
	}
	if (ops->ppi) {
		if (ops->ppi->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ppi + 1);
			sp->ppi = *val;
			*val = sp->ppi;
			ops->flags |= TF_SCTP_PPI;
		}
	}
	if (ops->sid) {
		if (ops->sid->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->sid + 1);
			sp->sid = *val;
			*val = sp->sid;
			ops->flags |= TF_SCTP_SID;
		}
	}
	if (ops->ssn) {
		/* not writeable */
	}
	if (ops->tsn) {
		/* not writeable */
	}
	if (ops->ropt) {
		if (ops->ropt->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ropt + 1);
			switch (*val) {
			case T_YES:
				sp->i_flags |= TF_SCTP_RECVOPT;
				break;
			case T_NO:
				sp->i_flags &= ~TF_SCTP_RECVOPT;
				break;
			}
			*val = (sp->i_flags & TF_SCTP_RECVOPT) ? T_YES : T_NO;
			ops->flags |= TF_SCTP_RECVOPT;
		}
	}
	if (ops->cklife) {
		if (ops->cklife->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->cklife + 1);
			if (*val < 10)
				*val = 10;
			sp->ck_life = (*val * HZ + 999) / 1000;
			*val = (sp->ck_life * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_COOKIE_LIFE;
		}
	}
	if (ops->sack) {
		if (ops->sack->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->sack + 1);
			sp->max_sack = (*val * HZ + 999) / 1000;
			*val = (sp->max_sack * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_SACK_DELAY;
		}
	}
	if (ops->path) {
		if (ops->path->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->path + 1);
			sp->rtx_path = *val;
			*val = sp->rtx_path;
			ops->flags |= TF_SCTP_PATH_MAX_RETRANS;
		}
	}
	if (ops->assoc) {
		if (ops->assoc->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->assoc + 1);
			sp->max_retrans = *val;
			*val = sp->max_retrans;
			ops->flags |= TF_SCTP_ASSOC_MAX_RETRANS;
		}
	}
	if (ops->init) {
		if (ops->init->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->init + 1);
			sp->max_inits = *val;
			*val = sp->max_inits;
			ops->flags |= TF_SCTP_MAX_INIT_RETRIES;
		}
	}
	if (ops->hbitvl) {
		if (ops->hbitvl->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->hbitvl + 1);
			sp->hb_itvl = (*val * HZ + 999) / 1000;
			*val = (sp->hb_itvl * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_HEARTBEAT_ITVL;
		}
	}
	if (ops->rtoinit) {
		if (ops->rtoinit->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->rtoinit + 1);
			sp->rto_ini = (*val * HZ + 999) / 1000;
			*val = (sp->rto_ini * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_RTO_INITIAL;
		}
	}
	if (ops->rtomin) {
		if (ops->rtomin->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->rtomin + 1);
			sp->rto_min = (*val * HZ + 999) / 1000;
			*val = (sp->rto_min * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_RTO_MIN;
		}
	}
	if (ops->rtomax) {
		if (ops->rtomax->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->rtomax + 1);
			sp->rto_max = (*val * HZ + 999) / 1000;
			*val = (sp->rto_max * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_RTO_MAX;
		}
	}
	if (ops->ostr) {
		if (ops->ostr->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ostr + 1);
			sp->req_ostr = *val;
			*val = sp->req_ostr;
			ops->flags |= TF_SCTP_OSTREAMS;
		}
	}
	if (ops->istr) {
		if (ops->istr->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->istr + 1);
			sp->max_istr = *val;
			*val = sp->max_istr;
			ops->flags |= TF_SCTP_ISTREAMS;
		}
	}
	if (ops->ckinc) {
		if (ops->ckinc->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->ckinc + 1);
			sp->ck_inc = (*val * HZ + 999) / 1000;
			*val = (sp->ck_inc * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_COOKIE_INC;
		}
	}
	if (ops->titvl) {
		if (ops->titvl->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->titvl + 1);
			sp->throttle = (*val * HZ + 999) / 1000;
			*val = (sp->throttle * 1000 + HZ - 1) / HZ;
			ops->flags |= TF_SCTP_THROTTLE_ITVL;
		}
	}
	if (ops->hmac) {
		if (ops->hmac->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->hmac + 1);
			sp->hmac = *val;
			*val = sp->hmac;
			ops->flags |= TF_SCTP_MAC_TYPE;
		}
	}
	if (ops->mseg) {
		/* not writeable */
	}
	if (ops->debug) {
		if (ops->debug->len >= olen) {
			t_scalar_t *val = (t_scalar_t *) (ops->debug + 1);
			sp->options = *val;
			*val = sp->options;
			ops->flags |= TF_SCTP_DEBUG;
		}
	}
	if (ops->hb) {
		/* not support yet */
	}
	if (ops->rto) {
		/* not support yet */
	}
	if (ops->status) {
		/* not writeable */
	}
	return;
}

/* 
 *  Check options
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_check_opts(sctp_t * sp, sctp_opts_t * ops)
{
	if (!ops)
		return;
	ops->flags = TF_SCTP_ALLOPS;
	/* FIXME: actually check some options.  */
	fixme(("Actually check some options.\n"));
	return;
}

/* 
 *  =========================================================================
 *
 *  SCTP T-Provider --> T-User Primitives (Indication, Confirmation and Ack)
 *
 *  =========================================================================
 *
 *  T_CONN_IND      11 - connection indication
 *  -----------------------------------------------------------------
 *  We get the connection indication information from the cookie received in the COOKIE ECHO
 *  which invokes the indication.  (We queue the COOKIE ECHO chunks themselves as
 *  indications.)
 */
STATIC int
t_conn_ind(sctp_t * sp, mblk_t *cp)
{
	mblk_t *mp;
	struct T_conn_ind *p;
	struct t_opthdr *oh;
	struct sctp_cookie_echo *m = (struct sctp_cookie_echo *) cp->b_rptr;
	struct sctp_cookie *ck = (struct sctp_cookie *) m->cookie;
	size_t danum = ck->danum + 1;
	uint32_t *daptr = (uint32_t *) (((caddr_t) (ck + 1) + ck->opt_len));
	size_t src_len = danum ? sizeof(uint16_t) + danum * sizeof(uint32_t) : 0;
	size_t str_len = sizeof(struct t_opthdr) + sizeof(t_scalar_t);
	size_t opt_len = 2 * str_len;
	ensure(((1 << sp->i_state) & (TSF_IDLE | TSF_WRES_CIND)), return (-EFAULT));
	if (bufq_length(&sp->conq) < sp->conind) {
		if (canputnext(sp->rq)) {
			if ((mp = allocb(sizeof(*p) + src_len + opt_len, BPRI_MED))) {
				mp->b_datap->db_type = M_PROTO;
				p = ((struct T_conn_ind *) mp->b_wptr)++;
				p->PRIM_type = T_CONN_IND;
				p->SRC_length = src_len;
				p->SRC_offset = src_len ? sizeof(*p) : 0;
				p->OPT_length = opt_len;
				p->OPT_offset = opt_len ? sizeof(*p) + src_len : 0;
				p->SEQ_number = (ulong) cp;
				/* place address information from cookie */
				if (danum)
					*((uint16_t *) mp->b_wptr)++ = ck->dport;
				if (danum--)
					*((uint32_t *) mp->b_wptr)++ = ck->daddr;
				while (danum--)
					*((uint32_t *) mp->b_wptr)++ = *daptr++;
				/* indicate options */
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_ISTREAMS;
				oh->status = T_SUCCESS;
				*((t_scalar_t *) mp->b_wptr)++ = ck->n_istr;
				/* indicate options */
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_OSTREAMS;
				oh->status = T_SUCCESS;
				*((t_scalar_t *) mp->b_wptr)++ = ck->n_ostr;
				bufq_queue(&sp->conq, cp);
				sp->i_state = TS_WRES_CIND;
				putnext(sp->rq, mp);
				return (0);
			}
			seldom();
			return (-ENOBUFS);
		}
		seldom();
		return (-EBUSY);
	}
	seldom();
	return (-ERESTART);
}

/* 
 *  T_CONN_CON      12 - connection confirmation
 *  -----------------------------------------------------------------
 *  The only options with end-to-end significance that are negotiated are the number of
 *  inbound and outbound streams.
 */
STATIC int
t_conn_con(sctp_t * sp)
{
	mblk_t *mp;
	struct T_conn_con *p;
	struct t_opthdr *oh;
	struct sctp_daddr *sd = sp->daddr;
	size_t res_len = sp->danum ? sizeof(uint16_t) + sp->danum * sizeof(sd->daddr) : 0;
	size_t str_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t opt_len = 2 * str_len;
	ensure((sp->i_state == TS_WCON_CREQ), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p) + res_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = ((struct T_conn_con *) mp->b_wptr)++;
			p->PRIM_type = T_CONN_CON;
			p->RES_length = res_len;
			p->RES_offset = res_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + res_len : 0;
			/* place destination (responding) address */
			if (sd)
				*((uint16_t *) mp->b_wptr)++ = sp->dport;
			for (; sd; sd = sd->next)
				*((uint32_t *) mp->b_wptr)++ = sd->daddr;
			/* indicate options */
			oh = ((struct t_opthdr *) mp->b_wptr)++;
			oh->len = str_len;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_ISTREAMS;
			oh->status = T_SUCCESS;
			*((t_scalar_t *) mp->b_wptr)++ = sp->n_istr;
			/* indicate options */
			oh = ((struct t_opthdr *) mp->b_wptr)++;
			oh->len = str_len;
			oh->level = T_INET_SCTP;
			oh->name = T_SCTP_OSTREAMS;
			oh->status = T_SUCCESS;
			*((t_scalar_t *) mp->b_wptr)++ = sp->n_ostr;
			sp->i_state = TS_DATA_XFER;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  T_DISCON_IND    13 - disconnect indication
 *  -----------------------------------------------------------------
 *  We use the address of the mblk that contains the COOKIE-ECHO chunk as a SEQ_number for
 *  connect indications that are rejected with a disconnect indication as well.  We can use
 *  this to directly address the mblk in the connection indication bufq.
 *
 *  If the caller provides disconnect data, the caller needs to set the current ord, ppi,
 *  sid, and ssn fields so that the user can examine them with T_OPTMGMT_REQ T_CURRENT if it
 *  has need to know them.
 */
STATIC int
t_discon_ind(sctp_t * sp, long reason, mblk_t *seq)
{
	mblk_t *mp;
	struct T_discon_ind *p;
	ensure(((1 << sp->
		 i_state) & (TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL |
			     TSF_WREQ_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((struct T_discon_ind *) mp->b_wptr)++;
			p->PRIM_type = T_DISCON_IND;
			p->DISCON_reason = reason;
			p->SEQ_number = (ulong) seq;
			if (seq) {
				bufq_unlink(&sp->conq, seq);
				freemsg(seq);
			}
			if (!bufq_length(&sp->conq))
				sp->i_state = TS_IDLE;
			else
				sp->i_state = TS_WRES_CIND;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  T_DATA_IND      14 - data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default stream.
 *  The caller should check that ppi and sid match the default before using this indication.
 *  Otherwise the caller should use the T_OPTDATA_IND.
 */
STATIC int
t_data_ind(sctp_t * sp, ulong more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_data_ind *p;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((struct T_data_ind *) mp->b_wptr)++;
			p->PRIM_type = T_DATA_IND;
			p->MORE_flag = more;
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  T_EXDATA_IND    15 - expedited data indication
 *  -----------------------------------------------------------------
 *  This indication is only useful for delivering data indications for the default stream.
 *  The caller should check that ppi and ssn match the default before using this indication.
 *  Otherwise the caller should use the T_OPTDATA_IND.
 */
STATIC int
t_exdata_ind(sctp_t * sp, ulong more, mblk_t *dp)
{
	mblk_t *mp;
	struct T_exdata_ind *p;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;	/* expedite */
			p = ((struct T_exdata_ind *) mp->b_wptr)++;
			p->PRIM_type = T_EXDATA_IND;
			p->MORE_flag = more;
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  T_INFO_ACK      16 - information acknowledgement
 *  -----------------------------------------------------------------
 *  Although there is no limit on CDATA and DDATA size, if these are too large then we will
 *  IP fragment the message.
 */
STATIC int
t_info_ack(sctp_t * sp)
{
	mblk_t *mp;
	struct T_info_ack *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_info_ack *) mp->b_wptr)++;
		p->PRIM_type = T_INFO_ACK;
		p->TSDU_size = -1;	/* no limit on TSDU size */
		p->ETSDU_size = -1;	/* no limit on ETSDU size */
		p->CDATA_size = -1;	/* no limit on CDATA size */
		p->DDATA_size = -1;	/* no limit on DDATA size */
		p->ADDR_size = -1;	/* no limit on ADDR size */
		p->OPT_size = -1;	/* no limit on OPTIONS size */
		p->TIDU_size = -1;	/* no limit on TIDU size */
		p->SERV_type = T_COTS_ORD;	/* COTS with orderly release */
		p->CURRENT_state = sp->i_state;
		p->PROVIDER_flag = XPG4_1 & ~T_SNDZERO;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  T_BIND_ACK      17 - bind acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_ack(sctp_t * sp)
{
	mblk_t *mp;
	struct T_bind_ack *p;
	struct sctp_saddr *ss = sp->saddr;
	size_t add_len = sp->sanum ? sizeof(sp->sport) + sp->sanum * sizeof(ss->saddr) : 0;
	ensure((sp->i_state == TS_WACK_BREQ), return (-EFAULT));
	if ((mp = allocb(sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_bind_ack *) mp->b_wptr)++;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = sp->conind;
		if (ss)
			*((typeof(sp->sport) *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((typeof(ss->saddr) *) mp->b_wptr)++ = ss->saddr;
		sp->i_state = TS_IDLE;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  T_ERROR_ACK     18 - error acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_error_ack(sctp_t * sp, ulong prim, long err)
{
	mblk_t *mp;
	struct T_error_ack *p;
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
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_error_ack *) mp->b_wptr)++;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = err < 0 ? TSYSERR : err;
		p->UNIX_error = err < 0 ? -err : 0;
		switch (sp->i_state) {
#ifdef TS_WACK_OPTREQ
		case TS_WACK_OPTREQ:
#endif
		case TS_WACK_UREQ:
		case TS_WACK_CREQ:
			sp->i_state = TS_IDLE;
			break;
		case TS_WACK_BREQ:
			sp->i_state = TS_UNBND;
			break;
		case TS_WACK_CRES:
			sp->i_state = TS_WRES_CIND;
			break;
		case TS_WACK_DREQ6:
			sp->i_state = TS_WCON_CREQ;
			break;
		case TS_WACK_DREQ7:
			sp->i_state = TS_WRES_CIND;
			break;
		case TS_WACK_DREQ9:
			sp->i_state = TS_DATA_XFER;
			break;
		case TS_WACK_DREQ10:
			sp->i_state = TS_WIND_ORDREL;
			break;
		case TS_WACK_DREQ11:
			sp->i_state = TS_WREQ_ORDREL;
			break;
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we send TOUTSTATE or TNOTSUPPORT or are responding
			   to a T_OPTMGMT_REQ in other then TS_IDLE state. */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  T_OK_ACK        19 - success acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_ok_ack(sctp_t * sp, ulong prim, ulong seq, ulong tok)
{
	mblk_t *mp;
	struct T_ok_ack *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_ok_ack *) mp->b_wptr)++;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		switch (sp->i_state) {
		case TS_WACK_CREQ:
			sp->i_state = TS_WCON_CREQ;
			break;
		case TS_WACK_UREQ:
			sp->i_state = TS_UNBND;
			break;
		case TS_WACK_CRES:
		{
			queue_t *aq = (queue_t *) tok;
			sctp_t *ap = (sctp_t *) aq->q_ptr;
			if (ap) {
				ap->i_state = TS_DATA_XFER;
				sctp_cleanup_read(sp);	/* deliver to user what is possible */
				sctp_transmit_wakeup(ap);	/* reply to peer what is necessary */
			}
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
			if (aq != sp->rq) {
				if (bufq_length(&sp->conq))
					sp->i_state = TS_WRES_CIND;
				else
					sp->i_state = TS_IDLE;
			}
			break;
		}
		case TS_WACK_DREQ7:
			if (seq) {
				bufq_unlink(&sp->conq, (mblk_t *) seq);
				freemsg((mblk_t *) seq);
			}
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ9:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			if (bufq_length(&sp->conq))
				sp->i_state = TS_WRES_CIND;
			else
				sp->i_state = TS_IDLE;
			break;
			/* Note: if we are not in a WACK state we simply do not change state.  This 
			   occurs normally when we are responding to a T_OPTMGMT_REQ in other than
			   the TS_IDLE state. */
		}
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  T_OPTMGMT_ACK   22 - options management acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_optmgmt_ack(sctp_t * sp, ulong flags, sctp_opts_t * ops)
{
	mblk_t *mp;
	size_t opt_len = sctp_opts_size(flags, sp, ops);
	struct T_optmgmt_ack *p;
	if ((mp = allocb(sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_optmgmt_ack *) mp->b_wptr)++;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		sctp_build_opts(flags, sp, ops, &mp->b_wptr);
#ifdef TS_WACK_OPTREQ
		if (sp->i_state == TS_WACK_OPTREQ)
			sp->i_state = TS_IDLE;
#endif
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

/* 
 *  T_ORDREL_IND    23 - orderly release indication
 *  -----------------------------------------------------------------
 */
STATIC int
t_ordrel_ind(sctp_t * sp)
{
	mblk_t *mp;
	struct T_ordrel_ind *p;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WIND_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = ((struct T_ordrel_ind *) mp->b_wptr)++;
			p->PRIM_type = T_ORDREL_IND;
			switch (sp->i_state) {
			case TS_DATA_XFER:
				sp->i_state = TS_WREQ_ORDREL;
				break;
			case TS_WIND_ORDREL:
				sp->i_state = TS_IDLE;
				break;
			}
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  T_OPTDATA_IND   26 - data with options indication
 *  -----------------------------------------------------------------
 */
STATIC int
t_optdata_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
	      uint more, mblk_t *dp)
{
	mblk_t *mp;
	struct t_opthdr *oh;
	struct T_optdata_ind *p;
	size_t str_len = sizeof(*oh) + sizeof(t_scalar_t);
	size_t opt_len = 0;
	ensure(((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)), return (-EFAULT));
	if (canputnext(sp->rq)) {
		if (sp->i_flags & TF_SCTP_RECVOPT)
			opt_len = 4 * str_len;
		if ((mp = allocb(sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = ord ? 0 : 1;	/* expedite */
			p = ((struct T_optdata_ind *) mp->b_wptr)++;
			p->PRIM_type = T_OPTDATA_IND;
			p->DATA_flag = (more ? T_ODF_MORE : 0) | (ord ? 0 : T_ODF_EX);
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			/* indicate options */
			if (sp->i_flags & TF_SCTP_RECVOPT) {
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_PPI;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = ppi;
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SID;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = sid;
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_SSN;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = ssn;
				oh = ((struct t_opthdr *) mp->b_wptr)++;
				oh->len = str_len;
				oh->level = T_INET_SCTP;
				oh->name = T_SCTP_TSN;
				oh->status = T_SUCCESS;
				*((t_uscalar_t *) mp->b_wptr)++ = tsn;
			}
			mp->b_cont = dp;
			putnext(sp->rq, mp);
			return (0);
		}
		seldom();
		return (-ENOBUFS);
	}
	seldom();
	return (-EBUSY);
}

/* 
 *  T_ADDR_ACK      27 - address acknowledgement
 *  -----------------------------------------------------------------
 */
STATIC int
t_addr_ack(sctp_t * sp)
{
	mblk_t *mp;
	struct T_addr_ack *p;
	struct sctp_saddr *ss = sp->saddr;
	struct sctp_daddr *sd = sp->daddr;
	size_t loc_len = sp->sanum ? sizeof(sp->sport) + sp->sanum * sizeof(ss->saddr) : 0;
	size_t rem_len = sp->danum ? sizeof(sp->dport) + sp->danum * sizeof(sd->daddr) : 0;
	if ((mp = allocb(sizeof(*p) + loc_len + rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_addr_ack *) mp->b_wptr)++;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = loc_len;
		p->LOCADDR_offset = loc_len ? sizeof(*p) : 0;
		p->REMADDR_length = rem_len;
		p->REMADDR_offset = rem_len ? sizeof(*p) + loc_len : 0;
		if (ss)
			*((typeof(sp->sport) *) mp->b_wptr)++ = sp->sport;
		for (; ss; ss = ss->next)
			*((typeof(ss->saddr) *) mp->b_wptr)++ = ss->saddr;
		if (sd)
			*((typeof(sp->dport) *) mp->b_wptr)++ = sp->dport;
		for (; sd; sd = sd->next)
			*((typeof(sd->daddr) *) mp->b_wptr)++ = sd->daddr;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}

#if 0
/* 
 *  T_CAPABILITY_ACK ?? - protocol capability ack
 *  -----------------------------------------------------------------
 */
STATIC int
t_capability_ack(sctp_t * sp, ulong caps)
{
	mblk_t *mp;
	struct T_capability_ack *p;
	uint caps = (acceptor ? TC1_ACCEPTOR_ID : 0) | (info ? TC1_INFO : 0);
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = ((struct T_capability_ack *) mp->b_wptr)++;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = caps;
		p->ACCEPTOR_id = (caps & TC1_ACCEPTOR_ID) ? (ulong) sp->rq : 0;
		if (caps & TC1_INFO) {
			p->INFO_ack.PRIM_type = T_INFO_ACK;
			p->INFO_ack.TSDU_size = sp->tsdu;
			p->INFO_ack.ETSDU_size = sp->etsdu;
			p->INFO_ack.CDATA_size = sp->cdata;
			p->INFO_ack.DDATA_size = sp->ddata;
			p->INFO_ack.ADDR_size = sp->addlen;
			p->INFO_ack.OPT_size = sp->optlen;
			p->INFO_ack.TIDU_size = sp->tidu;
			p->INFO_ack.SERV_type = sp->stype;
			p->INFO_ack.CURRENT_state = sp->i_state;
			p->INFO_ack.PROVIDER_flag = sp->ptype;
		} else
			bzero(&p->INFO_ack, sizeof(p->INFO_ack));
		putnext(sp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/* 
 *  NOTES:- TPI cannot do data acknowledgements, resets or retrieval.  Data
 *  acknowledgements and retrieval are different forms of the same service.
 *  For data acknowledgement, use the NPI interface.  For reset support (SCTP
 *  Restart indication different from SCTP CDI), use the NPI interface.
 */
STATIC int
sctp_t_conn_ind(sctp_t * sp, mblk_t *cp)
{
	// ptrace(("sp = %x, CONN_IND: seq = %x\n", (uint)sp, (uint)cp));
	return t_conn_ind(sp, cp);
}
STATIC int
sctp_t_conn_con(sctp_t * sp)
{
	// ptrace(("sp = %x, CONN_CONF\n", (uint)sp));
	return t_conn_con(sp);
}
STATIC int
sctp_t_data_ind(sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn, uint ord,
		uint more, mblk_t *dp)
{
	// ptrace(("sp = %x, DATA_IND: ppi=%u,sid=%u,ssn=%u,tsn=%u,ord=%u,more=%u\n", (uint)sp,
	// ppi,sid,ssn,tsn,ord,more));
	if (sp->i_flags & TF_SCTP_RECVOPT)
		return t_optdata_ind(sp, ppi, sid, ssn, tsn, ord, more, dp);
	if (ord)
		return t_data_ind(sp, more, dp);
	else
		return t_exdata_ind(sp, more, dp);
}
STATIC int
sctp_t_discon_ind(sctp_t * sp, ulong orig, long reason, mblk_t *cp)
{
	// ptrace(("sp = %x, DISCON_IND\n", (uint)sp));
	(void) orig;
	return t_discon_ind(sp, reason, cp);
}
STATIC int
sctp_t_ordrel_ind(sctp_t * sp)
{
	// ptrace(("sp = %x, ORDREL_IND\n", (uint)sp));
	return t_ordrel_ind(sp);
}

STATIC struct sctp_ifops t_ops = {
	sctp_conn_ind:sctp_t_conn_ind,
	sctp_conn_con:sctp_t_conn_con,
	sctp_data_ind:sctp_t_data_ind,
	sctp_datack_ind:NULL,
	sctp_reset_ind:NULL,
	sctp_reset_con:NULL,
	sctp_discon_ind:sctp_t_discon_ind,
	sctp_ordrel_ind:sctp_t_ordrel_ind,
	sctp_retr_ind:NULL,
	sctp_retr_con:NULL,
};

/* 
 *  =========================================================================
 *
 *  SCTP T-User --> T-Provider Primitives (Request and Response)
 *
 *  =========================================================================
 *  These represent primitive requests and responses from the Transport Provider Interface
 *  (TPI) transport user.  Each of these requests or responses invoked a protocol action
 *  depending on the current state of the provider.
 */
/* 
 *  T_CONN_REQ           0 - TC requeset
 *  -------------------------------------------------------------------------
 *  We have received a connection request from the user.  We use the characteristics of the
 *  primitive and the options on the stream to formulate and INIT message and send it to the
 *  peer.  We acknowledge the success or failure to starting this process to the user.  Once
 *  the process is started, a successful connection will conclude with COOKIE_ACK message
 *  that will generate a T_CONN_CON.  If the init process times out or there are other errors
 *  associated with establishing the SCTP association, they will be returned via the
 *  T_DISCON_IND primitive.
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
t_conn_req(sctp_t * sp, mblk_t *mp)
{
	int err = -EFAULT;
	struct sctp_addr *a;
	size_t anum;
	const struct T_conn_req *p = (struct T_conn_req *) mp->b_rptr;
	if (sp->i_state != TS_IDLE)
		goto outstate;
	sp->i_state = TS_WACK_CREQ;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto einval;
	if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
		goto badopt;
	a = (struct sctp_addr *) (mp->b_rptr + p->DEST_offset);
	anum = (p->DEST_length - sizeof(a->port)) / sizeof(a->addr[0]);
	if (mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length)
		goto badaddr;
	if ((!anum || p->DEST_length != sizeof(a->port) + anum * sizeof(a->addr[0])) || !a->port)
		goto badaddr;
	if (sp->cred.cr_uid != 0 && a->port < 1024)
		goto access;
	{
		struct t_opthdr *rto = NULL;
		struct t_opthdr *hb = NULL;
		/* address per-association options */
		if (p->OPT_length) {
			unsigned char *op = mp->b_rptr + p->OPT_offset;
			unsigned char *oe = op + p->OPT_length;
			struct t_opthdr *oh = (struct t_opthdr *) op;
			err = 0;
			for (;
			     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh) && op + oh->len <= oe;
			     op += oh->len, oh = (struct t_opthdr *) op) {
				t_scalar_t val = *((t_scalar_t *) (oh + 1));
				if (oh->level == T_INET_SCTP)
					switch (oh->name) {
					case T_SCTP_ISTREAMS:
						sp->max_istr = val ? val : 1;
						continue;
					case T_SCTP_OSTREAMS:
						sp->req_ostr = val ? val : 1;
						continue;
					case T_SCTP_SID:
						val = min(val, 0);
						val = max(val, sp->n_ostr);
						sp->sid = val;
						continue;
					case T_SCTP_PPI:
						sp->ppi = val;
						continue;
					case T_SCTP_SACK_DELAY:
						val = min(val, 10);
						sp->max_sack = val;
						continue;
					case T_SCTP_PATH_MAX_RETRANS:
						val = min(val, 0);
						sp->rtx_path = val;
						continue;
					case T_SCTP_ASSOC_MAX_RETRANS:
						val = min(val, 0);
						sp->max_retrans = val;
						continue;
					case T_SCTP_HEARTBEAT_ITVL:
						val = min(val, 10);
						sp->hb_itvl = val;
						continue;
					case T_SCTP_RTO_INITIAL:
						val = min(val, 10);
						sp->rto_ini = val;
						continue;
					case T_SCTP_RTO_MIN:
						val = min(val, 10);
						val = min(val, sp->rto_max);
						sp->rto_min = val;
						continue;
					case T_SCTP_RTO_MAX:
						val = min(val, 10);
						val = min(val, sp->rto_min);
						sp->rto_max = val;
						continue;
					case T_SCTP_HB:
						hb = oh;
						continue;
					case T_SCTP_RTO:
						rto = oh;
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
#if 0
		/* allocate addresses now */
		if ((err = sctp_alloc_daddrs(sp, a->port, a->addr, anum)))
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
	}
	if ((err = sctp_conn_req(sp, a->port, a->addr, anum, mp->b_cont)))
		goto error;
	mp->b_cont = NULL;	/* absorbed mp->b_cont */
	return t_ok_ack(sp, T_CONN_REQ, 0, 0);
      access:
	seldom();
	err = TACCES;
	goto error;		/* no permission for requested address */
      badaddr:
	seldom();
	err = TBADADDR;
	goto error;		/* address is unusable */
      badopt:
	seldom();
	err = TBADOPT;
	goto error;		/* options are unusable */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid message format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_CONN_REQ, err);
}

/* 
 *  T_CONN_RES           1 - Accept previous connection indication
 *  -----------------------------------------------------------------
 */
/* 
 *  IMPLEMENTATION NOTE:- Sequence numbers are actually the address of the mblk which
 *  contains the COOKIE-ECHO chunk and contains the cookie as a connection indication.  To
 *  find if a particular sequence number is valid, we walk the connection indication queue
 *  looking for a mblk with the same address as the sequence number.  Sequence numbers are
 *  only valid on the stream for which the connection indication is queued.
 */
STATIC mblk_t *
t_seq_check(sctp_t * sp, ulong seq)
{
	mblk_t *mp = bufq_head(&sp->conq);
	for (; mp && mp != (mblk_t *) seq; mp = mp->b_next) ;
	usual(mp);
	return (mp);
}

sctp_t *sctp_t_list;
STATIC sctp_t *
t_tok_check(ulong acceptor)
{
	sctp_t *ap;
	queue_t *aq = (queue_t *) acceptor;
	for (ap = sctp_t_list; ap && ap->rq != aq; ap = ap->next) ;
	usual(ap);
	return (ap);
}
STATIC int
t_conn_res(sctp_t * sp, mblk_t *mp)
{
	int err = 0;
	mblk_t *cp;
	sctp_t *ap;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_conn_res *p = (struct T_conn_res *) mp->b_rptr;
	if (sp->i_state != TS_WRES_CIND)
		goto outstate;
	sp->i_state = TS_WACK_CRES;
	if (mlen < sizeof(*p))
		goto einval;
	if (mlen < p->OPT_offset + p->OPT_length)
		goto badopt;
	if (!(cp = t_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if (!(ap = t_tok_check(p->ACCEPTOR_id))
	    || (ap != sp && !((1 << ap->i_state) & (TSF_UNBND | TSF_IDLE))))
		goto badf;
	if (ap->i_state == TS_IDLE && ap->conind)
		goto resqlen;
	/* protect at least r00t streams from users */
	if (sp->cred.cr_uid != 0 && ap->cred.cr_uid != sp->cred.cr_uid)
		goto access;
	{
		uint ap_oldstate = ap->i_state;
		struct t_opthdr *rto = NULL;
		struct t_opthdr *hb = NULL;
		/* address per-association options */
		if (p->OPT_length) {
			unsigned char *op = mp->b_rptr + p->OPT_offset;
			unsigned char *oe = op + p->OPT_length;
			struct t_opthdr *oh = (struct t_opthdr *) op;
			for (;
			     op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh) && op + oh->len <= oe;
			     op += oh->len, oh = (struct t_opthdr *) op) {
				t_scalar_t val = *((t_scalar_t *) (oh + 1));
				if (oh->level == T_INET_SCTP)
					switch (oh->name) {
					case T_SCTP_SID:
						val = min(val, 0);
						val = max(val, sp->n_ostr);
						sp->sid = val;
						continue;
					case T_SCTP_PPI:
						sp->ppi = val;
						continue;
					case T_SCTP_SACK_DELAY:
						val = min(val, 10);
						sp->max_sack = val;
						continue;
					case T_SCTP_PATH_MAX_RETRANS:
						val = min(val, 0);
						sp->rtx_path = val;
						continue;
					case T_SCTP_ASSOC_MAX_RETRANS:
						val = min(val, 0);
						sp->max_retrans = val;
						continue;
					case T_SCTP_HEARTBEAT_ITVL:
						val = min(val, 10);
						sp->hb_itvl = val;
						continue;
					case T_SCTP_RTO_INITIAL:
						val = min(val, 10);
						sp->rto_ini = val;
						continue;
					case T_SCTP_RTO_MIN:
						val = min(val, 10);
						val = min(val, sp->rto_max);
						sp->rto_min = val;
						continue;
					case T_SCTP_RTO_MAX:
						val = min(val, 10);
						val = min(val, sp->rto_min);
						sp->rto_max = val;
						continue;
					case T_SCTP_HB:
						hb = oh;
						continue;
					case T_SCTP_RTO:
						rto = oh;
						continue;
					}
				goto badopt;
			}
			if (op != oe)
				goto badopt;
			if (err)
				goto error;
		}
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
		ap->i_state = TS_DATA_XFER;
		if ((err = sctp_conn_res(sp, cp, ap, mp->b_cont))) {
			ap->i_state = ap_oldstate;
			goto error;
		}
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return t_ok_ack(sp, T_CONN_RES, (ulong) cp, (ulong) ap);
	}
      access:
	seldom();
	err = TACCES;
	goto error;		/* no access to accepting queue */
      resqlen:
	seldom();
	err = TRESQLEN;
	goto error;		/* accepting queue is listening */
      badf:
	seldom();
	err = TBADF;
	goto error;		/* accepting queue id is invalid */
      badseq:
	seldom();
	err = TBADSEQ;
	goto error;		/* connection ind referenced is invalid */
      badopt:
	seldom();
	err = TBADOPT;
	goto error;		/* options are bad */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_CONN_RES, err);
}

/* 
 *  T_DISCON_REQ        2 - TC disconnection request
 *  -----------------------------------------------------------------
 *
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
 *  Any data that is associated wtih the T_DISCON_REQ will be bundled as DATA chunks before
 *  the ABORT chunk in the mesage.  These DATA chunks will be sent out of order and
 *  unreliably on the default stream id and with the default payload protocol identifier in
 *  the hope that it makes it through to the other ULP before the ABORT chunk is processed.
 *
 */
STATIC int
t_discon_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	mblk_t *cp = NULL;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	struct T_discon_req *p = (struct T_discon_req *) mp->b_rptr;
	if (!
	    ((1 << sp->
	      i_state) & (TSF_WCON_CREQ | TSF_WRES_CIND | TSF_DATA_XFER | TSF_WIND_ORDREL |
			  TSF_WREQ_ORDREL)))
		goto outstate;
	switch (sp->i_state) {
	case TS_WCON_CREQ:
		sp->i_state = TS_WACK_DREQ6;
		break;
	case TS_WRES_CIND:
		sp->i_state = TS_WACK_DREQ7;
		break;
	case TS_DATA_XFER:
		sp->i_state = TS_WACK_DREQ9;
		break;
	case TS_WIND_ORDREL:
		sp->i_state = TS_WACK_DREQ10;
		break;
	case TS_WREQ_ORDREL:
		sp->i_state = TS_WACK_DREQ11;
		break;
	default:
		goto outstate;
	}
	if (mlen < sizeof(*p))
		goto einval;
	if (sp->i_state == TS_WACK_DREQ7 && !(cp = t_seq_check(sp, p->SEQ_number)))
		goto badseq;
	if ((err = sctp_discon_req(sp, cp)))
		goto error;
	return t_ok_ack(sp, T_DISCON_REQ, p->SEQ_number, 0);
      badseq:
	seldom();
	err = TBADSEQ;
	goto error;		/* connection ind reference is invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_DISCON_REQ, err);
}

/* 
 *  T_DATA_REQ          3 - Connection-Mode data transfer request
 *  -----------------------------------------------------------------
 */
STATIC int
t_error_reply(sctp_t * sp, int err)
{
	mblk_t *mp;
	switch (err) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		seldom();
		return (err);
	case 0:
	case 1:
	case 2:
		never();
		return (err);
	}
	if ((mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		putnext(sp->rq, mp);
		return (0);
	}
	seldom();
	return (-ENOBUFS);
}
STATIC int
t_write(sctp_t * sp, mblk_t *mp)
{
	int err;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto oustate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 1;
		ulong more = 0;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp)))
			goto error;
		return (1);	/* absorbed */
	}
      oustate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return t_error_reply(sp, err);
}
STATIC int
t_data_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_data_req *p = (struct T_data_req *) mp->b_rptr;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (mlen < sizeof(*p))
		goto einval;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto oustate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 1;
		ulong more = p->MORE_flag;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return (0);
	}
      oustate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invlaid primitive format */
      discard:
	seldom();
	return (0);		/* ignore in idle state */
      error:
	seldom();
	return t_error_reply(sp, err);
}

/* 
 *  T_EXDATA_REQ         4 - Expedited data request
 *  -----------------------------------------------------------------
 */
STATIC int
t_exdata_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_exdata_req *p = (struct T_exdata_req *) mp->b_rptr;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (mlen < sizeof(*p))
		goto einval;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto oustate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = 0;
		ulong more = p->MORE_flag;
		ulong rcpt = 0;
		if ((err = sctp_data_req(sp, ppi, sid, ord, more, rcpt, mp->b_cont)))
			goto error;
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return (0);
	}
      oustate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invlaid primitive format */
      discard:
	seldom();
	return (0);
      error:
	seldom();
	return t_error_reply(sp, err);
}

/* 
 *  T_INFO_REQ           5 - Information Request
 *  -----------------------------------------------------------------
 */
STATIC int
t_info_req(sctp_t * sp, mblk_t *mp)
{
	(void) mp;
	return t_info_ack(sp);
}

/* 
 *  T_BIND_REQ           6 - Bind a TS user to a transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_bind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_bind_req *p = (struct T_bind_req *) mp->b_rptr;
	if (sp->i_state != TS_UNBND)
		goto outstate;
	sp->i_state = TS_WACK_BREQ;
	if (mlen < sizeof(*p))
		goto einval;
	{
		struct sctp_addr *a = (struct sctp_addr *) (mp->b_rptr + p->ADDR_offset);
		size_t anum = (p->ADDR_length - sizeof(a->port)) / sizeof(a->addr[0]);
		if ((mlen < p->ADDR_offset + p->ADDR_length)
		    || (p->ADDR_length != sizeof(*a) + anum * sizeof(a->addr[0])))
			goto badaddr;
		/* we don't allow wildcards just yet */
		if (!anum || (!a->port && !(a->port = sctp_get_port())))
			goto noaddr;
		if (sp->cred.cr_uid != 0 && a->port < 1024)
			goto acces;
		if ((err = sctp_bind_req(sp, a->port, a->addr, anum, p->CONIND_number)))
			goto error;
		return t_bind_ack(sp);
	}
      acces:
	seldom();
	err = TACCES;
	goto error;		/* no permission for requested address */
      noaddr:
	seldom();
	err = TNOADDR;
	goto error;		/* cound not allocatea address */
      badaddr:
	seldom();
	err = TBADADDR;
	goto error;		/* address is invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_BIND_REQ, err);
}

/* 
 *  T_UNBIND_REQ         7 - Unbind TS user from transport address
 *  -----------------------------------------------------------------
 */
STATIC int
t_unbind_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	const struct T_unbind_req *p = (struct T_unbind_req *) mp->b_rptr;
	(void) p;
	if (sp->i_state != TS_IDLE)
		goto outstate;
	sp->i_state = TS_WACK_UREQ;
	if ((err = sctp_unbind_req(sp)))
		goto error;
	return t_ok_ack(sp, T_UNBIND_REQ, 0, 0);
      outstate:
	seldom();
	err = TOUTSTATE;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_ack(sp, T_UNBIND_REQ, err);
}

/* 
 *  T_OPTMGMT_REQ        9 - Options management request
 *  -----------------------------------------------------------------
 *  The T_OPTMGMT_REQ is responsible for establishing options while the stream
 *  is in the T_IDLE state.  When the stream is bound to a local address using
 *  the T_BIND_REQ, the settings of options with end-to-end significance will
 *  have an affect on how the driver response to an INIT with INIT-ACK for SCTP.
 *  For example, the bound list of addresses is the list of addresses that will
 *  be sent in the INIT-ACK.  The number of inbound streams and outbound streams
 *  are the numbers that will be used in the INIT-ACK.
 */
/* 
 *  Errors:
 *
 *  TACCES:     the user did not have proper permissions for the user of the requested
 *              options.
 *
 *  TBADFLAG:   the flags as specified were incorrect or invalid.
 *
 *  TBADOPT:    the options as specified were in an incorrect format, or they contained
 *              invalid information.
 *
 *  TOUTSTATE:  the primitive would place the transport interface out of state.
 *
 *  TNOTSUPPORT: this prmitive is not supported.
 *
 *  TSYSERR:    a system error has occured and the UNIX system error is indicated in the
 *              primitive.
 */
STATIC int
t_optmgmt_req(sctp_t * sp, mblk_t *mp)
{
	int err = 0;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optmgmt_req *p = (struct T_optmgmt_req *) mp->b_rptr;
#ifdef TS_WACK_OPTREQ
	if (sp->i_state == TS_IDLE)
		sp->i_state = TS_WACK_OPTREQ;
#endif
	if (mlen < sizeof(*p))
		goto einval;
	if (mlen < p->OPT_offset + p->OPT_length)
		goto badopt;
	{
		ulong flags = p->MGMT_flags;
		size_t opt_len = p->OPT_length;
		unsigned char *opt_ptr = mp->b_rptr + p->OPT_offset;
		struct sctp_opts ops = { 0L, NULL, };
		struct sctp_opts *opsp = NULL;
		if (opt_len) {
			if ((err = sctp_parse_opts(&ops, opt_ptr, opt_len)))
				goto error;
			opsp = &ops;
		}
		switch (flags) {
		case T_CHECK:
			sctp_check_opts(sp, opsp);
			return t_optmgmt_ack(sp, flags, opsp);
		case T_NEGOTIATE:
			sctp_negotiate_opts(sp, opsp);
			return t_optmgmt_ack(sp, flags, opsp);
		case T_DEFAULT:
			/* return defaults for the specified options */
		case T_CURRENT:
			return t_optmgmt_ack(sp, flags, opsp);
		default:
			goto badflag;
		}
	}
      badflag:
	seldom();
	err = TBADFLAG;
	goto error;		/* bad options flags */
      badopt:
	seldom();
	err = TBADOPT;
	goto error;		/* options were invalid */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      error:
	seldom();
	return t_error_ack(sp, T_OPTMGMT_REQ, err);
}

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
STATIC int
t_ordrel_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto outstate;
	switch (sp->i_state) {
	case TS_DATA_XFER:
		sp->i_state = TS_WIND_ORDREL;
		break;
	case TS_WREQ_ORDREL:
		sp->i_state = TS_IDLE;
		break;
	}
	if ((err = sctp_ordrel_req(sp)))
		goto error;
	return (0);
      outstate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      error:
	seldom();
	return t_error_reply(sp, err);
}

/* 
 *  T_OPTDATA_REQ       24- data with options request
 *  -----------------------------------------------------------------
 *  Basically the purpose of this for SCTP is to be able to set the SCTP Unordered Bit
 *  (U-bit), Payload Protocol Identifier (PPI) and Stream Id (sid) associated with the data
 *  message.  If not specified, each option will reduce to the current default settings.
 */
STATIC int
t_optdata_req(sctp_t * sp, mblk_t *mp)
{
	int err;
	const size_t mlen = mp->b_wptr - mp->b_rptr;
	const struct T_optdata_req *p = (struct T_optdata_req *) mp->b_rptr;
	if (sp->i_state == TS_IDLE)
		goto discard;
	if (mlen < sizeof(*p))
		goto einval;
	if ((p->OPT_length && (mlen < p->OPT_offset + p->OPT_length)))
		goto einval;
	if (!((1 << sp->i_state) & (TSF_DATA_XFER | TSF_WREQ_ORDREL)))
		goto outstate;
	{
		ulong ppi = sp->ppi;
		ulong sid = sp->sid;
		ulong ord = !(p->DATA_flag & T_ODF_EX);
		ulong more = (p->DATA_flag & T_ODF_MORE);
		ulong rcpt = 0;
		if (p->OPT_length) {
			unsigned char *op = mp->b_rptr + p->OPT_offset;
			unsigned char *oe = op + p->OPT_length;
			struct t_opthdr *oh = (struct t_opthdr *) op;
			for (; op + sizeof(*oh) <= oe && oh->len >= sizeof(*oh)
			     && op + oh->len <= oe; op += oh->len, oh = (struct t_opthdr *) op) {
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
			goto error;
		mp->b_cont = NULL;	/* absorbed mp->b_cont */
		return (0);
	}
      outstate:
	seldom();
	err = -EPROTO;
	goto error;		/* would place interface out of state */
      einval:
	seldom();
	err = -EINVAL;
	goto error;		/* invalid primitive format */
      discard:
	seldom();
	return (0);		/* ignore the idle state */
      error:
	seldom();
	return t_error_reply(sp, err);
}

/* 
 *  T_ADDR_REQ          25 - address request
 *  -----------------------------------------------------------------
 */
STATIC int
t_addr_req(sctp_t * sp, mblk_t *mp)
{
	(void) mp;
	return t_addr_ack(sp);
}

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
STATIC int
sctp_t_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	sctp_t *sp = (sctp_t *) q->q_ptr;
	ulong oldstate = sp->i_state;
	switch ((prim = *((ulong *) mp->b_rptr))) {
	case T_CONN_REQ:
		rtn = t_conn_req(sp, mp);
		break;
	case T_CONN_RES:
		rtn = t_conn_res(sp, mp);
		break;
	case T_DISCON_REQ:
		rtn = t_discon_req(sp, mp);
		break;
	case T_DATA_REQ:
		rtn = t_data_req(sp, mp);
		break;
	case T_EXDATA_REQ:
		rtn = t_exdata_req(sp, mp);
		break;
	case T_INFO_REQ:
		rtn = t_info_req(sp, mp);
		break;
	case T_BIND_REQ:
		rtn = t_bind_req(sp, mp);
		break;
	case T_UNBIND_REQ:
		rtn = t_unbind_req(sp, mp);
		break;
	case T_OPTMGMT_REQ:
		rtn = t_optmgmt_req(sp, mp);
		break;
	case T_ORDREL_REQ:
		rtn = t_ordrel_req(sp, mp);
		break;
	case T_OPTDATA_REQ:
		rtn = t_optdata_req(sp, mp);
		break;
	case T_ADDR_REQ:
		rtn = t_addr_req(sp, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0) {
		rare();
		sp->i_state = oldstate;
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
sctp_t_w_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	return t_write(sp, mp);
}
STATIC int
sctp_t_r_data(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	return sctp_recv_msg(sp, mp);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_t_r_error(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = (sctp_t *) q->q_ptr;
	rare();
	return sctp_recv_err(sp, mp);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 *  This is complete flush handling in both directions.  Standard stuff.
 */
STATIC int
sctp_t_m_flush(queue_t *q, mblk_t *mp, const uint8_t mflag, const uint8_t oflag)
{
	if (*mp->b_rptr & mflag) {
		if (*mp->b_rptr & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
		if (q->q_next) {
			putnext(q, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~mflag;
	}
	if (*mp->b_rptr & oflag) {
		queue_t *oq = q->q_other;
		if (*mp->b_rptr & FLUSHBAND)
			flushband(oq, mp->b_rptr[1], FLUSHALL);
		else
			flushq(oq, FLUSHALL);
		if (oq->q_next) {
			putnext(oq, mp);
			return (QR_ABSORBED);
		}
		*mp->b_rptr &= ~oflag;
	}
	return (0);
}
STATIC int
sctp_t_w_flush(queue_t *q, mblk_t *mp)
{
	return sctp_t_m_flush(q, mp, FLUSHW, FLUSHR);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Other messages (e.g. M_IOCACK)
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sctp_t_r_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on RD(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	putnext(q, mp);
	return (QR_ABSORBED);
}
STATIC int
sctp_t_w_other(queue_t *q, mblk_t *mp)
{
	sctp_t *sp = SCTP_PRIV(q);
	rare();
	cmn_err(CE_WARN, "Unsupported block type %d on WR(q) %d\n", mp->b_datap->db_type,
		sp->cminor);
	return (-EOPNOTSUPP);
}

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
STATIC INLINE int
sctp_t_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_t_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sctp_t_w_proto(q, mp);
	case M_FLUSH:
		return sctp_t_w_flush(q, mp);
	default:
		return sctp_t_w_other(q, mp);
	}
}

/* 
 *  IP Read Message
 */
STATIC INLINE int
sctp_t_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sctp_t_r_data(q, mp);
	case M_ERROR:
		return sctp_t_r_error(q, mp);
	default:
		return sctp_t_r_other(q, mp);
	}
}

/* 
 *  PUTQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_t_putq(queue_t *q, mblk_t *mp, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	ensure(mp, return (-EFAULT));
	if (mp->b_datap->db_type >= QPCTL && !q->q_count && !sctp_trylock(q)) {
		int rtn;
		switch ((rtn = proc(q, mp))) {
		case QR_DONE:
			freemsg(mp);
		case QR_ABSORBED:
			break;
		case QR_TRIMMED:
			freeb(mp);
			break;
		case QR_LOOP:
			if (!q->q_next) {
				qreply(q, mp);
				break;
			}
		case QR_PASSALONG:
			if (q->q_next) {
				putnext(q, mp);
				break;
			}
			rtn = -EOPNOTSUPP;
		default:
			ptrace(("ERROR: (q dropping) %d\n", rtn));
			freemsg(mp);
			break;
		case QR_DISABLE:
			putq(q, mp);
			rtn = 0;
			break;
		case QR_PASSFLOW:
			if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
				putnext(q, mp);
				break;
			}
		case -ENOBUFS:
		case -EBUSY:
		case -ENOMEM:
		case -EAGAIN:
			putq(q, mp);
			break;
		}
		sctp_unlock(q);
		return (rtn);
	} else {
		seldom();
		putq(q, mp);
		return (0);
	}
}

/* 
 *  SRVQ Put Routine
 *  -----------------------------------
 */
STATIC INLINE int
sctp_t_srvq(queue_t *q, int (*proc) (queue_t *, mblk_t *))
{
	ensure(q, return (-EFAULT));
	if (!sctp_waitlock(q)) {
		int rtn = 0;
		mblk_t *mp;
		while ((mp = getq(q))) {
			switch ((rtn = proc(q, mp))) {
			case QR_DONE:
				freemsg(mp);
			case QR_ABSORBED:
				continue;
			case QR_TRIMMED:
				freeb(mp);
				continue;
			case QR_LOOP:
				if (!q->q_next) {
					qreply(q, mp);
					continue;
				}
			case QR_PASSALONG:
				if (q->q_next) {
					putnext(q, mp);
					continue;
				}
				rtn = -EOPNOTSUPP;
			default:
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			case QR_DISABLE:
				ptrace(("ERROR: (q disabling) %d\n", rtn));
				noenable(q);
				putbq(q, mp);
				rtn = 0;
				break;
			case QR_PASSFLOW:
				if (mp->b_datap->db_type >= QPCTL || canputnext(q)) {
					putnext(q, mp);
					continue;
				}
			case -ENOBUFS:	/* proc must schedule bufcall */
			case -EBUSY:	/* proc must fail canput */
			case -ENOMEM:	/* proc must schedule re-enable */
			case -EAGAIN:	/* proc must schedule re-enable */
				if (mp->b_datap->db_type < QPCTL) {
					ptrace(("ERROR: (q stalled) %d\n", rtn));
					putbq(q, mp);
					break;
				}
				if (mp->b_datap->db_type == M_PCPROTO) {
					mp->b_datap->db_type = M_PROTO;
					mp->b_band = 255;
					putq(q, mp);
					break;
				}
				ptrace(("ERROR: (q dropping) %d\n", rtn));
				freemsg(mp);
				continue;
			}
			break;
		}
		sctp_unlock(q);
		return (rtn);
	}
	return (-EAGAIN);
}
STATIC int
sctp_t_rput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_t_putq(q, mp, &sctp_t_r_prim);
}
STATIC int
sctp_t_rsrv(queue_t *q)
{
	return (int) sctp_t_srvq(q, &sctp_t_r_prim);
}
STATIC int
sctp_t_wput(queue_t *q, mblk_t *mp)
{
	return (int) sctp_t_putq(q, mp, &sctp_t_w_prim);
}
STATIC int
sctp_t_wsrv(queue_t *q)
{
	return (int) sctp_t_srvq(q, &sctp_t_w_prim);
}

/* 
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
sctp_t *sctp_t_list = NULL;
STATIC int
sctp_t_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	sctp_t *sp, **spp = &sctp_t_list;
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
	for (; *spp && (*spp)->cmajor < cmajor; spp = &(*spp)->next) ;
	for (; *spp && cminor <= SCTP_T_CMINORS; spp = &(*spp)->next) {
		ushort dminor = (*spp)->cminor;
		if (cminor < dminor)
			break;
		if (cminor == dminor) {
			if (sflag != CLONEOPEN) {
				rare();
				return (ENXIO);	/* requested device in use */
			}
			cminor++;
		}
	}
	if (cminor > SCTP_T_CMINORS) {
		rare();
		return (ENXIO);
	}
	*devp = makedevice(cmajor, cminor);
	if (!(sp = sctp_alloc_priv(q, spp, cmajor, cminor, &t_ops))) {
		rare();
		return (ENOMEM);
	}
	return (0);
}
STATIC int
sctp_t_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sctp_free_priv(q);
	return (0);
}

/* 
 *  =========================================================================
 *
 *  LiS Module Initialization
 *
 *  =========================================================================
 */
static void
sctp_t_init(void)
{
	int cmajor;
	if ((cmajor =
	     lis_register_strdev(SCTP_T_CMAJOR_0, &sctp_t_info, SCTP_T_CMINORS,
				 sctp_t_minfo.mi_idname)) < 0) {
		sctp_t_minfo.mi_idnum = 0;
		rare();
		cmn_err(CE_NOTE, "sctp: couldn't register driver\n");
		return;
	}
	sctp_t_minfo.mi_idnum = cmajor;
}
static void
sctp_t_term(void)
{
	if (sctp_t_minfo.mi_idnum) {
		if ((sctp_t_minfo.mi_idnum = lis_unregister_strdev(sctp_t_minfo.mi_idnum))) {
			sctp_t_minfo.mi_idnum = 0;
			rare();
			cmn_err(CE_WARN, "sdt: couldn't unregister driver!\n");
		}
	}
};

/* 
 *  =========================================================================
 *  =========================================================================
 *  =========================================================================
 */
STATIC void
sctp_init(void)
{
	sctp_init_locks();
	sctp_init_caches();
	sctp_init_proto();
}
STATIC void
sctp_term(void)
{
	sctp_term_locks();
	sctp_term_caches();
	sctp_term_proto();
}

/* 
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int
init_module(void)
{
	cmn_err(CE_NOTE, SCTP_BANNER);	/* console splash */
	sctp_init();
	sctp_n_init();
	sctp_t_init();
	return (0);
}

void
cleanup_module(void)
{
	sctp_term();
	sctp_n_term();
	sctp_t_term();
	return;
}
