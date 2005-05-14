/*****************************************************************************

 @(#) $Id: sctp.h,v 0.9.2.9 2005/05/14 08:29:33 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:29:33 $ by $Author: brian $

 *****************************************************************************/

#ifndef _NETINET_SCTP_H
#define _NETINET_SCTP_H 1

#ident "@(#) $Name:  $($Revision: 0.9.2.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#ifndef __KERNEL__
#include <features.h>
#include <sys/types.h>
#else				/* __KERNEL__ */
#include <linux/types.h>
#endif				/* __KERNEL__ */

#ifndef __KERNEL__
__BEGIN_DECLS
#endif

enum {
	__OS7_SCTP_ESTABLISHED = 1,
	__OS7_SCTP_COOKIE_WAIT,
	__OS7_SCTP_COOKIE_ECHOED,
	__OS7_SCTP_SHUTDOWN_PENDING,
	__OS7_SCTP_SHUTDOWN_SENT,
	__OS7_SCTP_UNREACHABLE,
	__OS7_SCTP_CLOSED,
	__OS7_SCTP_SHUTDOWN_RECEIVED,
	__OS7_SCTP_SHUTDOWN_RECVWAIT,
	__OS7_SCTP_LISTEN,
	__OS7_SCTP_SHUTDOWN_ACK_SENT,
	__OS7_SCTP_MAX_STATES
};

/*
 *  User __OS7_SCTP_HB socket option structure
 */
struct __os7_sctp_hbitvl {
	struct sockaddr_in dest;	/* destination IP address */
	uint hb_act;			/* activation flag */
	uint hb_itvl;			/* interval in milliseconds */
};
/*
 *  User __OS7_SCTP_RTO socket option structure
 */
struct __os7_sctp_rtoval {
	struct sockaddr_in dest;	/* destination IP address */
	uint rto_initial;		/* RTO.Initial (milliseconds) */
	uint rto_min;			/* RTO.Min (milliseconds) */
	uint rto_max;			/* RTO.Max (milliseconds) */
	uint max_retrans;		/* Path.Max.Retrans (retires) */
};
/*
 *  User __OS7_SCTP_STATUS socket option structure
 */
struct __os7_sctp_dstat {
	struct sockaddr_in dest;	/* destination IP address */
	uint dst_cwnd;			/* congestion window */
	uint dst_unack;			/* unacknowledged chunks */
	uint dst_srtt;			/* smoothed round trip time */
	uint dst_rvar;			/* rtt variance */
	uint dst_rto;			/* current rto */
	uint dst_sst;			/* slow start threshold */
};
struct __os7_sctp_astat {
	uint assoc_rwnd;		/* receive window */
	uint assoc_rbuf;		/* receive buffer */
	uint assoc_nrep;		/* destinations reported */
};

/*
 *  User-settable options (used with setsockopt).
 */
#define __OS7_SCTP_NODELAY		 1	/* don't delay send to defrag or bundle chunks */
#define __OS7_SCTP_MAXSEG		 2	/* set artificial path MTU */
#define __OS7_SCTP_CORK			 3	/* control sending of partial chunks */
#define __OS7_SCTP_RECVSID		 4	/* control addition of stream id ancillary data */
#define __OS7_SCTP_RECVPPI		 5	/* control addition of payload proto id ancillary
						   data */
#define __OS7_SCTP_RECVSSN		 6
#define __OS7_SCTP_RECVTSN		 7
#define __OS7_SCTP_SID			 8	/* stream id default and ancillary data */
#define __OS7_SCTP_PPI			 9	/* payload protocol id default and ancillary data */
#define __OS7_SCTP_SSN			10
#define __OS7_SCTP_TSN			11
#define __OS7_SCTP_HB			12	/* control heartbeat setting and interval */
#define __OS7_SCTP_RTO			13	/* control and check RTO values */
#define __OS7_SCTP_COOKIE_LIFE		14
#define __OS7_SCTP_SACK_DELAY		15
#define __OS7_SCTP_PATH_MAX_RETRANS	16
#define __OS7_SCTP_ASSOC_MAX_RETRANS	17
#define __OS7_SCTP_MAX_INIT_RETRIES	18
#define __OS7_SCTP_HEARTBEAT_ITVL	19
#define __OS7_SCTP_RTO_INITIAL		20
#define __OS7_SCTP_RTO_MIN		21
#define __OS7_SCTP_RTO_MAX		22
#define __OS7_SCTP_OSTREAMS		23
#define __OS7_SCTP_ISTREAMS		24
#define __OS7_SCTP_COOKIE_INC		25
#define __OS7_SCTP_THROTTLE_ITVL	26
#define __OS7_SCTP_MAC_TYPE		27
#define __OS7_SCTP_CKSUM_TYPE		28
#define __OS7_SCTP_DEBUG_OPTIONS	29
#define __OS7_SCTP_STATUS		30
#define __OS7_SCTP_ALI			31
#define __OS7_SCTP_PR			32
#define __OS7_SCTP_DISPOSITION		33
#define __OS7_SCTP_LIFETIME		34
#define __OS7_SCTP_ADD			35
#define __OS7_SCTP_ADD_IP		36
#define __OS7_SCTP_DEL_IP		37
#define __OS7_SCTP_SET			38
#define __OS7_SCTP_SET_IP		39
#define __OS7_SCTP_ECN			40
#define __OS7_SCTP_MAX_BURST		41

/*
 *  HMAC settings for cookie verification for use with __OS7_SCTP_MAC_TYPE socket
 *  option.
 */
#define __OS7_SCTP_HMAC_NONE		0	/* no hmac (all one's) */
#define __OS7_SCTP_HMAC_SHA_1		1	/* SHA-1 coded hmac */
#define __OS7_SCTP_HMAC_MD5		2	/* MD5 coded hmac */

/*
 *  CSUM settings for checksum algorithm selection with __OS7_SCTP_CHKSUM_TYPE
 *  socket option.
 */
#define __OS7_SCTP_CSUM_ADLER32		0	/* Adler32 checksum output */
#define __OS7_SCTP_CSUM_CRC32C		1	/* CRC-32c checksum output */

/*
 *  Debugging flags for use with __OS7_SCTP_DEBUG_OPTIONS socket option.
 */
#define __OS7_SCTP_OPTION_DROPPING	0x01	/* stream will drop packets */
#define __OS7_SCTP_OPTION_BREAK		0x02	/* stream will break dest #1 and 2 one way */
#define __OS7_SCTP_OPTION_DBREAK	0x04	/* stream will break dest both ways */
#define __OS7_SCTP_OPTION_RANDOM	0x08	/* stream will drop packets at random */

/*
 *  Partial reliability preference for use wtih __OS7_SCTP_PR socket option
 */
#define __OS7_SCTP_PR_NONE		0x0	/* no partial reliability */
#define __OS7_SCTP_PR_PREFERRED		0x1	/* partial reliability preferred */
#define __OS7_SCTP_PR_REQUIRED		0x2	/* partial reliability required */

/*
 *  Message retrieval dispositions for use with MSG_CONFIRM to  recvmsg()
 */
#define __OS7_SCTP_DISPOSITION_NONE		0x0
#define __OS7_SCTP_DISPOSITION_UNSENT		0x1
#define __OS7_SCTP_DISPOSITION_SENT		0x2
#define __OS7_SCTP_DISPOSITION_GAP_ACKED	0x3
#define __OS7_SCTP_DISPOSITION_ACKED		0x4

/* this should be in in.h */
#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP	132	/* Stream Control Transmission Protocol */
#endif

/* this should be in socket.h */
#ifndef SOL_SCTP
#define SOL_SCTP	132	/* SCTP level */
#endif

#undef SCTP_ADD_IP
#undef SCTP_ADD
#undef SCTP_ALI
#undef SCTP_ASSOC_MAX_RETRANS
#undef sctp_astat
#undef SCTP_CKSUM_TYPE
#undef SCTP_CLOSED
#undef SCTP_COOKIE_ECHOED
#undef SCTP_COOKIE_INC
#undef SCTP_COOKIE_LIFE
#undef SCTP_COOKIE_WAIT
#undef SCTP_CORK
#undef SCTP_CSUM_ADLER32
#undef SCTP_CSUM_CRC32C
#undef SCTP_DEBUG_OPTIONS
#undef SCTP_DEL_IP
#undef SCTP_DISPOSITION_ACKED
#undef SCTP_DISPOSITION_GAP_ACKED
#undef SCTP_DISPOSITION_NONE
#undef SCTP_DISPOSITION
#undef SCTP_DISPOSITION_SENT
#undef SCTP_DISPOSITION_UNSENT
#undef sctp_dstat
#undef SCTP_ECN
#undef SCTP_ESTABLISHED
#undef sctp_hbitvl
#undef SCTP_HB
#undef SCTP_HEARTBEAT_ITVL
#undef SCTP_HMAC_MD5
#undef SCTP_HMAC_NONE
#undef SCTP_HMAC_SHA_1
#undef SCTP_ISTREAMS
#undef SCTP_LIFETIME
#undef SCTP_LISTEN
#undef SCTP_MAC_TYPE
#undef SCTP_MAX_BURST
#undef SCTP_MAX_INIT_RETRIES
#undef SCTP_MAXSEG
#undef SCTP_MAX_STATES
#undef SCTP_NODELAY
#undef SCTP_OPTION_BREAK
#undef SCTP_OPTION_DBREAK
#undef SCTP_OPTION_DROPPING
#undef SCTP_OPTION_RANDOM
#undef SCTP_OSTREAMS
#undef SCTP_PATH_MAX_RETRANS
#undef SCTP_PPI
#undef SCTP_PR_NONE
#undef SCTP_PR
#undef SCTP_PR_PREFERRED
#undef SCTP_PR_REQUIRED
#undef SCTP_RECVPPI
#undef SCTP_RECVSID
#undef SCTP_RECVSSN
#undef SCTP_RECVTSN
#undef SCTP_RTO_INITIAL
#undef SCTP_RTO_MAX
#undef SCTP_RTO_MIN
#undef SCTP_RTO
#undef sctp_rtoval
#undef SCTP_SACK_DELAY
#undef SCTP_SET_IP
#undef SCTP_SET
#undef SCTP_SHUTDOWN_ACK_SENT
#undef SCTP_SHUTDOWN_PENDING
#undef SCTP_SHUTDOWN_RECEIVED
#undef SCTP_SHUTDOWN_RECVWAIT
#undef SCTP_SHUTDOWN_SENT
#undef SCTP_SID
#undef SCTP_SSN
#undef SCTP_STATUS
#undef SCTP_THROTTLE_ITVL
#undef SCTP_TSN
#undef SCTP_UNREACHABLE

#define SCTP_ADD_IP			__OS7_SCTP_ADD_IP
#define SCTP_ADD			__OS7_SCTP_ADD
#define SCTP_ALI			__OS7_SCTP_ALI
#define SCTP_ASSOC_MAX_RETRANS		__OS7_SCTP_ASSOC_MAX_RETRANS
#define sctp_astat			__os7_sctp_astat
#define SCTP_CKSUM_TYPE			__OS7_SCTP_CKSUM_TYPE
#define SCTP_CLOSED			__OS7_SCTP_CLOSED
#define SCTP_COOKIE_ECHOED		__OS7_SCTP_COOKIE_ECHOED
#define SCTP_COOKIE_INC			__OS7_SCTP_COOKIE_INC
#define SCTP_COOKIE_LIFE		__OS7_SCTP_COOKIE_LIFE
#define SCTP_COOKIE_WAIT		__OS7_SCTP_COOKIE_WAIT
#define SCTP_CORK			__OS7_SCTP_CORK
#define SCTP_CSUM_ADLER32		__OS7_SCTP_CSUM_ADLER32
#define SCTP_CSUM_CRC32C		__OS7_SCTP_CSUM_CRC32C
#define SCTP_DEBUG_OPTIONS		__OS7_SCTP_DEBUG_OPTIONS
#define SCTP_DEL_IP			__OS7_SCTP_DEL_IP
#define SCTP_DISPOSITION_ACKED		__OS7_SCTP_DISPOSITION_ACKED
#define SCTP_DISPOSITION_GAP_ACKED	__OS7_SCTP_DISPOSITION_GAP_ACKED
#define SCTP_DISPOSITION_NONE		__OS7_SCTP_DISPOSITION_NONE
#define SCTP_DISPOSITION		__OS7_SCTP_DISPOSITION
#define SCTP_DISPOSITION_SENT		__OS7_SCTP_DISPOSITION_SENT
#define SCTP_DISPOSITION_UNSENT		__OS7_SCTP_DISPOSITION_UNSENT
#define sctp_dstat			__os7_sctp_dstat
#define SCTP_ECN			__OS7_SCTP_ECN
#define SCTP_ESTABLISHED		__OS7_SCTP_ESTABLISHED
#define sctp_hbitvl			__os7_sctp_hbitvl
#define SCTP_HB				__OS7_SCTP_HB
#define SCTP_HEARTBEAT_ITVL		__OS7_SCTP_HEARTBEAT_ITVL
#define SCTP_HMAC_MD5			__OS7_SCTP_HMAC_MD5
#define SCTP_HMAC_NONE			__OS7_SCTP_HMAC_NONE
#define SCTP_HMAC_SHA_1			__OS7_SCTP_HMAC_SHA_1
#define SCTP_ISTREAMS			__OS7_SCTP_ISTREAMS
#define SCTP_LIFETIME			__OS7_SCTP_LIFETIME
#define SCTP_LISTEN			__OS7_SCTP_LISTEN
#define SCTP_MAC_TYPE			__OS7_SCTP_MAC_TYPE
#define SCTP_MAX_BURST			__OS7_SCTP_MAX_BURST
#define SCTP_MAX_INIT_RETRIES		__OS7_SCTP_MAX_INIT_RETRIES
#define SCTP_MAXSEG			__OS7_SCTP_MAXSEG
#define SCTP_MAX_STATES			__OS7_SCTP_MAX_STATES
#define SCTP_NODELAY			__OS7_SCTP_NODELAY
#define SCTP_OPTION_BREAK		__OS7_SCTP_OPTION_BREAK
#define SCTP_OPTION_DBREAK		__OS7_SCTP_OPTION_DBREAK
#define SCTP_OPTION_DROPPING		__OS7_SCTP_OPTION_DROPPING
#define SCTP_OPTION_RANDOM		__OS7_SCTP_OPTION_RANDOM
#define SCTP_OSTREAMS			__OS7_SCTP_OSTREAMS
#define SCTP_PATH_MAX_RETRANS		__OS7_SCTP_PATH_MAX_RETRANS
#define SCTP_PPI			__OS7_SCTP_PPI
#define SCTP_PR_NONE			__OS7_SCTP_PR_NONE
#define SCTP_PR				__OS7_SCTP_PR
#define SCTP_PR_PREFERRED		__OS7_SCTP_PR_PREFERRED
#define SCTP_PR_REQUIRED		__OS7_SCTP_PR_REQUIRED
#define SCTP_RECVPPI			__OS7_SCTP_RECVPPI
#define SCTP_RECVSID			__OS7_SCTP_RECVSID
#define SCTP_RECVSSN			__OS7_SCTP_RECVSSN
#define SCTP_RECVTSN			__OS7_SCTP_RECVTSN
#define SCTP_RTO_INITIAL		__OS7_SCTP_RTO_INITIAL
#define SCTP_RTO_MAX			__OS7_SCTP_RTO_MAX
#define SCTP_RTO_MIN			__OS7_SCTP_RTO_MIN
#define SCTP_RTO			__OS7_SCTP_RTO
#define sctp_rtoval			__os7_sctp_rtoval
#define SCTP_SACK_DELAY			__OS7_SCTP_SACK_DELAY
#define SCTP_SET_IP			__OS7_SCTP_SET_IP
#define SCTP_SET			__OS7_SCTP_SET
#define SCTP_SHUTDOWN_ACK_SENT		__OS7_SCTP_SHUTDOWN_ACK_SENT
#define SCTP_SHUTDOWN_PENDING		__OS7_SCTP_SHUTDOWN_PENDING
#define SCTP_SHUTDOWN_RECEIVED		__OS7_SCTP_SHUTDOWN_RECEIVED
#define SCTP_SHUTDOWN_RECVWAIT		__OS7_SCTP_SHUTDOWN_RECVWAIT
#define SCTP_SHUTDOWN_SENT		__OS7_SCTP_SHUTDOWN_SENT
#define SCTP_SID			__OS7_SCTP_SID
#define SCTP_SSN			__OS7_SCTP_SSN
#define SCTP_STATUS			__OS7_SCTP_STATUS
#define SCTP_THROTTLE_ITVL		__OS7_SCTP_THROTTLE_ITVL
#define SCTP_TSN			__OS7_SCTP_TSN
#define SCTP_UNREACHABLE		__OS7_SCTP_UNREACHABLE

#ifndef __KERNEL__
__END_DECLS
#endif

#endif				/* _NETINET_SCTP_H */
