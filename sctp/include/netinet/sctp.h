/*****************************************************************************

 @(#) $Id: sctp.h,v 0.9.2.6 2004/12/22 11:27:51 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/12/22 11:27:51 $ by $Author: brian $

 *****************************************************************************/

#ifndef _NETINET_SCTP_H
#define _NETINET_SCTP_H 1

#ident "@(#) $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

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

__END_DECLS

#endif				/* _NETINET_SCTP_H */
