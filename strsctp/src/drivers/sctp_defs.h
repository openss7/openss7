/*****************************************************************************

 @(#) $Id: sctp_defs.h,v 0.9.2.7 2008-04-28 23:13:25 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-28 23:13:25 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp_defs.h,v $
 Revision 0.9.2.7  2008-04-28 23:13:25  brian
 - updated headers for release

 Revision 0.9.2.6  2007/08/14 06:22:22  brian
 - GPLv3 header update

 Revision 0.9.2.5  2007/06/17 01:57:11  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SCTP_DEFS_H__
#define __SCTP_DEFS_H__

#ident "@(#) $RCSfile: sctp_defs.h,v $ $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

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
   basic headers 
 */
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
#define SCTP_CAUSE_FIRST	   1
#define SCTP_CAUSE_INVALID_STR     1	/* Invalid Stream Identifier */
#define SCTP_CAUSE_MISSING_PARM    2	/* Missing Mandatory Parameter */
#define SCTP_CAUSE_STALE_COOKIE    3	/* Stale Cookie Error */
#define SCTP_CAUSE_NO_RESOURCE     4	/* Out of Resource */
#define SCTP_CAUSE_BAD_ADDRESS     5	/* Unresolvable Address */
#define SCTP_CAUSE_BAD_CHUNK_TYPE  6	/* Unrecognized Chunk Type */
#define SCTP_CAUSE_INVALID_PARM    7	/* Invalid Mandatory Parameter */
#define SCTP_CAUSE_BAD_PARM        8	/* Unrecognized Parameters */
#define SCTP_CAUSE_NO_DATA         9	/* No User Data */
#define SCTP_CAUSE_SHUTDOWN       10	/* Cookie Received While Shutting Down */
#define SCTP_CAUSE_LAST           10

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
	/* 
   followed by *//*
   address parameter            
 */
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

	/* 
	   followed by opt.__data */
	/* 
	   followed by dtas */
	/* 
	   uint32_t dtas[0]; dest transport addresses */
	/* 
	   followed by stas */
	/* 
	   uint32_t stas[0]; srce transport addresses */
	/* 
	   followed by mac */
	/* 
	   uint8_t mac[160]; message authentication code */
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
#include "sctp_adler32.h"	/* The Adler32 checksum is deprecated.  */
#endif
#include "sctp_crc32c.h"

/*
 *  Message control block datastructures:
 *  -------------------------------------------------------------------------
 */

/*
   message control block 
 */
typedef struct sctp_tcb {
	/* 
	   for gaps, dups and acks on receive, frag on transmit */
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

#define SCTP_TCB(__mp) ((sctp_tcb_t *)((__mp)->b_datap->db_base))
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

#endif				/* __SCTP_DEFS_H__ */
