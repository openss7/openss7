/*****************************************************************************

 @(#) $Id: sctp_defs.h,v 0.9.2.1 2004/02/17 06:21:49 brian Exp $

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

 Last Modified $Date: 2004/02/17 06:21:49 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCTP_DEFS_H__
#define __SCTP_DEFS_H__

#ident "@(#) $RCSfile: sctp_defs.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/02/17 06:21:49 $"

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
	   followed by 
 *//*
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
	   followed by opt.__data 
	 */
	/*
	   followed by dtas 
	 */
	/*
	   uint32_t dtas[0]; dest transport addresses 
	 */
	/*
	   followed by stas 
	 */
	/*
	   uint32_t stas[0]; srce transport addresses 
	 */
	/*
	   followed by mac 
	 */
	/*
	   uint8_t mac[160]; message authentication code 
	 */
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
   The Adler32 checksum is deprecated. 
 */
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
   #define CRC32C_POLY 0x1EDC6F41 
 */
/*
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 */
/*
   Copyright 2001, D. Otis.  Use this program, code or tables 
 */
/*
   extracted from it, as desired without restriction.  
 */
/*
 */
/*
   32 Bit Reflected CRC table generation for SCTP.  
 */
/*
   To accommodate serial byte data being shifted out least 
 */
/*
   significant bit first, the table's 32 bit words are reflected 
 */
/*
   which flips both byte and bit MS and LS positions.  The CRC 
 */
/*
   is calculated MS bits first from the perspective of the serial
 */
/*
   stream.  The x^32 term is implied and the x^0 term may also 
 */
/*
   be shown as +1.  The polynomial code used is 0x1EDC6F41.  
 */
/*
   Castagnoli93 
 */
/*
   x^32+x^28+x^27+x^26+x^25+x^23+x^22+x^20+x^19+x^18+x^14+x^13+ 
 */
/*
   x^11+x^10+x^9+x^8+x^6+x^0 
 */
/*
   Guy Castagnoli Stefan Braeuer and Martin Herrman 
 */
/*
   "Optimization of Cyclic Redundancy-Check Codes 
 */
/*
   with 24 and 32 Parity Bits", 
 */
/*
   IEEE Transactions on Communications, Vol.41, No.6, June 1993 
 */
/*
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
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
   Note crc should be initialized to 0xffffffff 
 */
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
	crc = swab32(crc);
	return ~crc;
}

/*
 *  Message control block datastructures:
 *  -------------------------------------------------------------------------
 */

/*
   message control block 
 */
typedef struct sctp_tcb {
	/*
	   for gaps, dups and acks on receive, frag on transmit 
	 */
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
