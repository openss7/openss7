/*****************************************************************************

 @(#) $Id: xti_sctp.h,v 0.9 2004/04/05 12:37:53 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 1997-2004 OpenSS7 Corporation.  All Rights Reserved.

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

 Last Modified $Date: 2004/04/05 12:37:53 $ by $Author: brian $

 *****************************************************************************/

#ifndef _XTI_SCTP_H
#define _XTI_SCTP_H

#ident "@(#) $RCSfile: xti_sctp.h,v $ $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#ifndef t_uscalar_t
#define t_uscalar_t ulong
#define t_scalar_t  long
#endif

typedef struct sctp_addr {
	unsigned short int port __attribute__ ((packed));
	t_uscalar_t addr[0] __attribute__ ((packed));
} sctp_addr_t;

#define T_INET_SCTP	132	/* SCTP level (same as protocol number) */

/*
 *  SCTP Transport Provider Options
 */
#define T_SCTP_NODELAY			 1
#define T_SCTP_CORK			 2
#define T_SCTP_PPI			 3
#define T_SCTP_SID			 4
#define T_SCTP_SSN			 5
#define T_SCTP_TSN			 6
#define T_SCTP_RECVOPT			 7
#define T_SCTP_COOKIE_LIFE		 8
#define T_SCTP_SACK_DELAY		 9
#define T_SCTP_PATH_MAX_RETRANS		10
#define T_SCTP_ASSOC_MAX_RETRANS	11
#define T_SCTP_MAX_INIT_RETRIES		12
#define T_SCTP_HEARTBEAT_ITVL		13
#define T_SCTP_RTO_INITIAL		14
#define T_SCTP_RTO_MIN			15
#define T_SCTP_RTO_MAX			16
#define T_SCTP_OSTREAMS			17
#define T_SCTP_ISTREAMS			18
#define T_SCTP_COOKIE_INC		19
#define T_SCTP_THROTTLE_ITVL		20
#define T_SCTP_MAC_TYPE			21

#define T_SCTP_HMAC_NONE	0
#define T_SCTP_HMAC_SHA1	1
#define T_SCTP_HMAC_MD5		2

#define T_SCTP_CKSUM_TYPE		22

#define T_SCTP_CSUM_ADLER32	0
#define T_SCTP_CSUM_CRC32C	1

#define T_SCTP_ECN			23
#define T_SCTP_ALI			24
#define T_SCTP_ADD			25
#define T_SCTP_SET			26
#define T_SCTP_ADD_IP			27
#define T_SCTP_DEL_IP			28
#define T_SCTP_SET_IP			29
#define T_SCTP_PR			30
#define T_SCTP_LIFETIME			31

#define T_SCTP_DISPOSITION		32

#define T_SCTP_DISPOSITION_NONE		0
#define T_SCTP_DISPOSITION_UNSENT	1
#define T_SCTP_DISPOSITION_SENT		2
#define T_SCTP_DISPOSITION_GAP_ACKED	3
#define T_SCTP_DISPOSITION_ACKED	4

#define T_SCTP_MAX_BURST		33

#define T_SCTP_HB			34
typedef struct t_sctp_hb {
	t_uscalar_t hb_dest;		/* destination address */
	t_uscalar_t hb_onoff;		/* activation flag */
	t_uscalar_t hb_itvl;		/* interval in milliseconds */
} t_sctp_hb_t;

#define T_SCTP_RTO			35
typedef struct t_sctp_rto {
	t_uscalar_t rto_dest;		/* destination address */
	t_uscalar_t rto_initial;	/* RTO.Initial (milliseconds) */
	t_uscalar_t rto_min;		/* RTO.Min (milliseconds) */
	t_uscalar_t rto_max;		/* RTO.Max (milliseconds) */
	t_uscalar_t max_retrans;	/* Path.Max.Retrans (retries) */
} t_sctp_rto_t;

/*
 *  Read-only options...
 */
#define T_SCTP_MAXSEG			36
#define T_SCTP_STATUS			37
typedef struct t_sctp_dest_status {
	t_uscalar_t dest_addr;		/* dest address */
	t_uscalar_t dest_cwnd;		/* dest congestion window */
	t_uscalar_t dest_unack;		/* dest unacknowledged chunks */
	t_uscalar_t dest_srtt;		/* dest smooth round trip time */
	t_uscalar_t dest_rvar;		/* dest rtt variance */
	t_uscalar_t dest_rto;		/* dest current rto */
	t_uscalar_t dest_sst;		/* dest slow start threshold */
} t_sctp_dest_status_t;

typedef struct t_sctp_status {
	t_uscalar_t curr_rwnd;		/* current receive window */
	t_uscalar_t curr_rbuf;		/* current receive buffer */
	t_uscalar_t curr_nrep;		/* current dests reported */
	t_sctp_dest_status_t curr_dest[0];	/* current primary dest */
} t_sctp_status_t;

#define T_SCTP_DEBUG			38

#ifndef SCTP_OPTION_DROPPING
#define SCTP_OPTION_DROPPING	0x01	/* stream will drop packets */
#define SCTP_OPTION_BREAK	0x02	/* stream will break dest #1 */
#define SCTP_OPTION_DBREAK	0x04	/* stream will break dest both ways */
#define SCTP_OPTION_RANDOM	0x08	/* stream will drop packets at random */
#endif

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _XTI_SCTP_H */
