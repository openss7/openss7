/*****************************************************************************

 @(#) $Id: sctp.h,v 0.9.2.1 2001/06/06 17:09:42 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 2000  OpenSS7 Corporation.  All Rights Reserved.


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

 Last Modified $Date: 2001/06/06 17:09:42 $ by $Author: brian $

 $Log: sctp.h,v $
 Revision 0.9.2.1  2001/06/06 17:09:42  brian
 Added common user header file.

 Revision 0.1  2001/06/06 17:09:42  brian
 Added common user header file.

 *****************************************************************************/

#ifndef _NETINET_SCTP_H
#define _NETINET_SCTP_H 1

#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

enum
{
        SCTP_ESTABLISHED = 1,
        SCTP_COOKIE_WAIT,
        SCTP_COOKIE_ECHOED,
        SCTP_SHUTDOWN_PENDING,
        SCTP_SHUTDOWN_SENT,
        SCTP_UNREACHABLE,
        SCTP_CLOSED,
        SCTP_SHUTDOWN_RECEIVED,
        SCTP_SHUTDOWN_ACK_SENT2,
        SCTP_LISTEN,
        SCTP_SHUTDOWN_ACK_SENT1
};

/*
 *  User SCTP_HDR socket option structure
 */
struct sctpchdr {
        uint8_t     type;       /* Chunk type, always DATA      */
        uint8_t     flags;      /* Chunk flags                  */
        uint16_t    len;        /* Chunk length                 */
};
struct sctp_data {
        struct sctpchdr ch;
        uint32_t        tsn;    /* Transmit Sequence Number     */
        uint16_t        sid;    /* Stream Identifier            */
        uint16_t        ssn;    /* Stream Sequence Number       */
        uint32_t        ppi;    /* Payload Protocol Identifier  */
};

/*
 *  User SCTP_HB socket option structure
 */
struct sctp_hbitvl {
        struct sockaddr_in
		dest;       /* destination IP address */
	uint	active;     /* activation flag */
        uint	itvl;       /* interval in milliseconds */
};
/*
 *  User SCTP_RTO socket option structure
 */
struct sctp_rtoval {
	struct sockaddr_in
		dest;		/* destination IP address	*/
	uint	rto_initial;	/* RTO.Initial (milliseconds)	*/
	uint	rto_min;	/* RTO.Min     (milliseconds)	*/
	uint	rto_max;	/* RTO.Max     (milliseconds)	*/
	uint	max_retrans;	/* Path.Max.Retrans (retires)	*/
};

/*
 *  User-settable options (used with setsockopt).
 */
#define SCTP_NODELAY    0x01    /* don't delay send to defrag or bundle chunks */
#define SCTP_MAXSEG     0x02    /* set artificial path MTU */
#define SCTP_CORK       0x03    /* control sending of partial chunks */
#define SCTP_RECVSID    0x04    /* control addition of stream id ancillary data */
#define SCTP_RECVPPI    0x05    /* control addition of payload proto id ancillary data */
#define SCTP_RECVHDR    0x06    /* control addition of DATA chunk header */
#define SCTP_SID        0x07    /* stream id default and ancillary data */
#define SCTP_PPI        0x08    /* payload protocol id default and ancillary data */
#define SCTP_HB         0x09    /* control heartbeat setting and interval */
#define SCTP_RTO        0x0A    /* control and check RTO values */
#define SCTP_HDR        0x0B    /* DATA chunk header ancillary data */

/*
 *  Additional CMSG flags for use with sendmsg/recvmsg.
 */
#define SCTP_CMSGF_RECVSID  0x01
#define SCTP_CMSGF_RECVPPI  0x02
#define SCTP_CMSGF_RECVHDR  0x04

/*
 *  HMAC settings for cookie verification.
 */
#define SCTP_HMAC_NONE      0
#define SCTP_HMAC_SHA_1     1
#define SCTP_HMAC_MD5       2

/* this should be in in.h */
#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP        132     /* Stream Control Transmission Protocol */
#endif

#define SOL_SCTP            132     /* SCTP level */

__END_DECLS

#endif  /* _NETINET_SCTP_H */
