/*****************************************************************************

 @(#) $Id: sctp.h,v 0.9.2.4 2003/03/06 12:32:35 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 1997-2002 OpenSS7 Corporation.  All Rights Reserved.

                                  PUBLIC LICENSE

     This license is provided without fee, provided that the above copy-
     right notice and this public license must be retained on all copies,
     extracts, compilations and derivative works.  Use or distribution of
     this work in a manner that restricts its use except as provided here
     will render this license void.

     The author(s) hereby waive any and all other restrictions in respect
     of their copyright in this software and its associated documentation.
     The authors(s) of this software place in the public domain any novel
     methods or processes which are embodied in this software.

     The author(s) undertook to write it for the sake of the advancement
     of the Arts and Sciences, but it is provided as is, and the author(s)
     will not take any responsibility in it.

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

 Last Modified $Date: 2003/03/06 12:32:35 $ by $Author: brian $

 *****************************************************************************/

#ifndef _NETINET_SCTP_H
#define _NETINET_SCTP_H 1

#ident "@(#) $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

enum {
	SCTP_ESTABLISHED = 1,
	SCTP_COOKIE_WAIT,
	SCTP_COOKIE_ECHOED,
	SCTP_SHUTDOWN_PENDING,
	SCTP_SHUTDOWN_SENT,
	SCTP_UNREACHABLE,
	SCTP_CLOSED,
	SCTP_SHUTDOWN_RECEIVED,
	SCTP_SHUTDOWN_RECVWAIT,
	SCTP_LISTEN,
	SCTP_SHUTDOWN_ACK_SENT,
	SCTP_MAX_STATES
};

/*
 *  User SCTP_HB socket option structure
 */
struct sctp_hbitvl {
	struct sockaddr_in
	 dest;					/* destination IP address */
	uint hb_act;				/* activation flag */
	uint hb_itvl;				/* interval in milliseconds */
};
/*
 *  User SCTP_RTO socket option structure
 */
struct sctp_rtoval {
	struct sockaddr_in
	 dest;					/* destination IP address */
	uint rto_initial;			/* RTO.Initial (milliseconds) */
	uint rto_min;				/* RTO.Min (milliseconds) */
	uint rto_max;				/* RTO.Max (milliseconds) */
	uint max_retrans;			/* Path.Max.Retrans (retires) */
};
/*
 *  User SCTP_STATUS socket option structure
 */
struct sctp_dstat {
	struct sockaddr_in
	 dest;					/* destination IP address */
	uint dst_cwnd;				/* congestion window */
	uint dst_unack;				/* unacknowledged chunks */
	uint dst_srtt;				/* smoothed round trip time */
	uint dst_rvar;				/* rtt variance */
	uint dst_rto;				/* current rto */
	uint dst_sst;				/* slow start threshold */
};
struct sctp_astat {
	uint assoc_rwnd;			/* receive window */
	uint assoc_rbuf;			/* receive buffer */
	uint assoc_nrep;			/* destinations reported */
};

/*
 *  User-settable options (used with setsockopt).
 */
#define SCTP_NODELAY		 1	/* don't delay send to defrag or bundle chunks */
#define SCTP_MAXSEG		 2	/* set artificial path MTU */
#define SCTP_CORK		 3	/* control sending of partial chunks */
#define SCTP_RECVSID		 4	/* control addition of stream id ancillary data */
#define SCTP_RECVPPI		 5	/* control addition of payload proto id ancillary data */
#define SCTP_RECVSSN		 6
#define SCTP_RECVTSN		 7
#define SCTP_SID		 8	/* stream id default and ancillary data */
#define SCTP_PPI		 9	/* payload protocol id default and ancillary data */
#define SCTP_SSN		10
#define SCTP_TSN		11
#define SCTP_HB			12	/* control heartbeat setting and interval */
#define SCTP_RTO		13	/* control and check RTO values */
#define SCTP_COOKIE_LIFE	14
#define SCTP_SACK_DELAY		15
#define SCTP_PATH_MAX_RETRANS	16
#define SCTP_ASSOC_MAX_RETRANS	17
#define SCTP_MAX_INIT_RETRIES	18
#define SCTP_HEARTBEAT_ITVL	19
#define SCTP_RTO_INITIAL	20
#define SCTP_RTO_MIN		21
#define SCTP_RTO_MAX		22
#define SCTP_OSTREAMS		23
#define SCTP_ISTREAMS		24
#define SCTP_COOKIE_INC		25
#define SCTP_THROTTLE_ITVL	26
#define SCTP_MAC_TYPE		27
#define SCTP_CKSUM_TYPE		28
#define SCTP_DEBUG_OPTIONS	29
#define SCTP_STATUS		30
#define SCTP_ALI		31
#define SCTP_PR			32
#define SCTP_DISPOSITION	33
#define SCTP_LIFETIME		34
#define SCTP_ADD		35
#define SCTP_ADD_IP		36
#define SCTP_DEL_IP		37
#define SCTP_SET		38
#define SCTP_SET_IP		39
#define SCTP_ECN		40
#define SCTP_MAX_BURST		41

/*
 *  HMAC settings for cookie verification for use with SCTP_MAC_TYPE socket
 *  option.
 */
#define SCTP_HMAC_NONE		0	/* no hmac (all one's) */
#define SCTP_HMAC_SHA_1		1	/* SHA-1 coded hmac */
#define SCTP_HMAC_MD5		2	/* MD5 coded hmac */

/*
 *  CSUM settings for checksum algorithm selection with SCTP_CHKSUM_TYPE
 *  socket option.
 */
#define SCTP_CSUM_ADLER32	0	/* Adler32 checksum output */
#define SCTP_CSUM_CRC32C	1	/* CRC-32c checksum output */

/*
 *  Debugging flags for use with SCTP_DEBUG_OPTIONS socket option.
 */
#define SCTP_OPTION_DROPPING	0x01	/* stream will drop packets */
#define SCTP_OPTION_BREAK	0x02	/* stream will break dest #1 and 2 one way */
#define SCTP_OPTION_DBREAK	0x04	/* stream will break dest both ways */
#define SCTP_OPTION_RANDOM	0x08	/* stream will drop packets at random */

/*
 *  Partial reliability preference for use wtih SCTP_PR socket option
 */
#define SCTP_PR_NONE		0x0	/* no partial reliability */
#define SCTP_PR_PREFERRED	0x1	/* partial reliability preferred */
#define SCTP_PR_REQUIRED	0x2	/* partial reliability required */

/*
 *  Message retrieval dispositions for use with MSG_CONFIRM to  recvmsg()
 */
#define SCTP_DISPOSITION_NONE		0x0
#define SCTP_DISPOSITION_UNSENT		0x1
#define SCTP_DISPOSITION_SENT		0x2
#define SCTP_DISPOSITION_GAP_ACKED	0x3
#define SCTP_DISPOSITION_ACKED		0x4

/* this should be in in.h */
#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP        132	/* Stream Control Transmission Protocol */
#endif

/* this should be in socket.h */
#ifndef SOL_SCTP
#define SOL_SCTP            132	/* SCTP level */
#endif

__END_DECLS
#endif				/* _NETINET_SCTP_H */
