/*****************************************************************************

 @(#) sctp.h,v 1.1.6.2 2004/01/26 12:58:54 brian Exp

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified 2004/01/26 12:58:54 by brian

 *****************************************************************************/

#ifndef __OS7_LINUX_LINUX_SCTP_H__
#define __OS7_LINUX_LINUX_SCTP_H__

#ident "@(#) sctp.h,v LINUX-2-4-20-SCTP(1.1.6.2) 2004/01/26 12:58:54"

#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/tcp.h>		/* for TCP states */
#include <linux/socket.h>
#include <linux/in.h>

struct __os7_sctphdr {
	__u16 srce;
	__u16 dest;
	__u32 v_tag;
	__u32 check;
};

struct __os7_sctpchdr {
	__u8 type;
	__u8 flags;
	__u16 len;
};

#ifdef HAVE_MEMBER_SK_BUFF_H_SH
#define SCTP_SKB_SH(__skb)	((__skb)->h.sh)
#else
#define SCTP_SKB_SH(__skb)	((struct __os7_sctphdr *)((__skb)->h.raw))
#endif
#define SCTP_SKB_SH_SRCE(__skb)	(SCTP_SKB_SH(__skb)->srce)
#define SCTP_SKB_SH_DEST(__skb)	(SCTP_SKB_SH(__skb)->dest)
#define SCTP_SKB_SH_VTAG(__skb)	(SCTP_SKB_SH(__skb)->v_tag)
#define SCTP_SKB_SH_CSUM(__skb)	(SCTP_SKB_SH(__skb)->check)
#define SCTP_SKB_CH(__skb)	((struct __os7_sctpchdr *)(__skb)->data)
#define SCTP_SKB_CH_TYPE(__skb)	(SCTP_SKB_CH(__skb)->type)
#define SCTP_SKB_CH_LEN(__skb)	(ntohs(SCTP_SKB_CH(__skb)->len))

/*
 *  SCTP Cause Codes
 */
#define __OS7_SCTP_CAUSE_INVALID_STR      1	/* Invalid Stream Identifier */
#define __OS7_SCTP_CAUSE_MISSING_PARM     2	/* Missing Mandatory Parameter */
#define __OS7_SCTP_CAUSE_STALE_COOKIE     3	/* Stale Cookie Error */
#define __OS7_SCTP_CAUSE_NO_RESOURCE      4	/* Out of Resource */
#define __OS7_SCTP_CAUSE_BAD_ADDRESS      5	/* Unresolvable Address */
#define __OS7_SCTP_CAUSE_BAD_CHUNK_TYPE   6	/* Unrecognized Chunk Type */
#define __OS7_SCTP_CAUSE_INVALID_PARM     7	/* Invalid Mandatory Parameter */
#define __OS7_SCTP_CAUSE_BAD_PARM         8	/* Unrecognized Parameters */
#define __OS7_SCTP_CAUSE_NO_DATA          9	/* No User Data */
#define __OS7_SCTP_CAUSE_SHUTDOWN         10	/* Cookie Received While Shutting Down */
#define __OS7_SCTP_CAUSE_NEW_ADDR         11	/* Restart of an assoc w/ new address */
#define __OS7_SCTP_CAUSE_USER_INITIATED   12	/* User Initiated Abort */
#define __OS7_SCTP_CAUSE_PROTO_VIOLATION  13	/* Protocol Violation */
#define __OS7_SCTP_CAUSE_LAST_ADDR      0x100	/* Request to delete last ip address */
#define __OS7_SCTP_CAUSE_RES_SHORTAGE   0x101	/* Operation refused resource shortage */
#define __OS7_SCTP_CAUSE_SOURCE_ADDR    0x102	/* Request to delete source ip address */
#define __OS7_SCTP_CAUSE_ILLEGAL_ASCONF 0x103	/* Assoc. aborted illegal ASCONF-ACK */

#define __OS7_SCTP_CF_INVALID_STR	(1<< __OS7_SCTP_CAUSE_INVALID_STR      )
#define __OS7_SCTP_CF_MISSING_PARM	(1<< __OS7_SCTP_CAUSE_MISSING_PARM     )
#define __OS7_SCTP_CF_STALE_COOKIE	(1<< __OS7_SCTP_CAUSE_STALE_COOKIE     )
#define __OS7_SCTP_CF_NO_RESOURCE	(1<< __OS7_SCTP_CAUSE_NO_RESOURCE      )
#define __OS7_SCTP_CF_BAD_ADDRESS	(1<< __OS7_SCTP_CAUSE_BAD_ADDRESS      )
#define __OS7_SCTP_CF_BAD_CHUNK_TYPE	(1<< __OS7_SCTP_CAUSE_BAD_CHUNK_TYPE   )
#define __OS7_SCTP_CF_INVALID_PARM	(1<< __OS7_SCTP_CAUSE_INVALID_PARM     )
#define __OS7_SCTP_CF_BAD_PARM		(1<< __OS7_SCTP_CAUSE_BAD_PARM         )
#define __OS7_SCTP_CF_NO_DATA		(1<< __OS7_SCTP_CAUSE_NO_DATA          )
#define __OS7_SCTP_CF_SHUTDOWN		(1<< __OS7_SCTP_CAUSE_SHUTDOWN         )

#define __OS7_SCTP_CF_MASK_ANY	(__OS7_SCTP_CF_INVALID_STR \
				|__OS7_SCTP_CF_MISSING_PARM \
				|__OS7_SCTP_CF_STALE_COOKIE \
				|__OS7_SCTP_CF_NO_RESOURCE \
				|__OS7_SCTP_CF_BAD_ADDRESS \
				|__OS7_SCTP_CF_BAD_CHUNK_TYPE \
				|__OS7_SCTP_CF_INVALID_PARM \
				|__OS7_SCTP_CF_BAD_PARM \
				|__OS7_SCTP_CF_NO_DATA \
				|__OS7_SCTP_CF_SHUTDOWN )

/*
 *  SCTP Cause Code Structures
 */
struct __os7_sctpehdr {
	__u16 code;
	__u16 len;
};
struct __os7_sctp_bad_stri {		/* Invalid Stream Identifier */
	struct __os7_sctpehdr eh;
	__u16 sid;			/* stream identifier */
	__u16 res;			/* reserved */
};
struct __os7_sctp_no_mparm {		/* Missing Mandatory Parameter */
	struct __os7_sctpehdr eh;
	__u32 num;			/* number of missing parms */
	__u16 mp[0];			/* array of missing parms */
};
struct __os7_sctp_stale_ck {		/* Stale Cookie Error */
	struct __os7_sctpehdr eh;
	__u32 staleness;		/* measure of staleness (usec) */
};
struct __os7_sctp_no_rsrce {		/* Out of Resource */
	struct __os7_sctpehdr eh;
};
struct __os7_sctp_bad_addr {		/* Unresolvable Address */
	struct __os7_sctpehdr eh;
	/* address parameter */
};
struct __os7_sctp_bad_chnk {		/* Unrecognized Chunk Type */
	struct __os7_sctpehdr eh;
	struct __os7_sctpchdr ch;	/* header of unrecognized chunk */
};
struct __os7_sctp_bad_parm {		/* Invalid Mandatory Parameter */
	struct __os7_sctpehdr eh;
};
struct __os7_sctp_unk_parm {		/* Unrecognized Parameters */
	struct __os7_sctpehdr eh;
	unsigned char parm[0];		/* unrecognized parameters */
};
struct __os7_sctp_no_udata {		/* No user Data */
	struct __os7_sctpehdr eh;
	__u32 tsn;			/* tsn of data chunk */
};
struct __os7_sctp_ck_shutd {		/* Cookie Received While Shutting Down */
	struct __os7_sctpehdr eh;
};
struct __os7_sctp_last_add {		/* Request to delete last address */
	struct __os7_sctpehdr eh;
	__u32 cid;
	/* followed by ASCONF TLV */
};
struct __os7_sctp_op_short {		/* Operation refused resource shortage */
	struct __os7_sctpehdr eh;
	__u32 cid;
	/* followed by ASCONF TLV */
};
struct __os7_sctp_srce_add {		/* Request to delete source address */
	struct __os7_sctpehdr eh;
	__u32 cid;
	/* followed by ASCONF TLV */
};
struct __os7_sctp_bad_conf {		/* Illegal ASCONF-ACK */
	struct __os7_sctpehdr eh;
};
union __os7_sctp_cause {
	struct __os7_sctpehdr eh;
	struct __os7_sctp_bad_stri bad_stri;	/* Invalid Stream Identifier */
	struct __os7_sctp_no_mparm no_mparm;	/* Missing Mandatory Parameter */
	struct __os7_sctp_stale_ck stale_ck;	/* Stale Cookie Error */
	struct __os7_sctp_no_rsrce no_rsrce;	/* Out of Resource */
	struct __os7_sctp_bad_addr bad_addr;	/* Unresolvable Address */
	struct __os7_sctp_bad_chnk bad_chnk;	/* Unrecognized Chunk Type */
	struct __os7_sctp_bad_parm bad_parm;	/* Invalid Mandatory Parameter */
	struct __os7_sctp_unk_parm unk_parm;	/* Unrecognized Parameters */
	struct __os7_sctp_no_udata no_udata;	/* No user Data */
	struct __os7_sctp_ck_shutd ck_shutd;	/* Cookie Received While Shutting Down */
};

/*
 *  COOKIE parameter
 *
 *  TODO: Put IP OPTIONS (struct ip_options + __data) from the INIT message
 *  into the cookie and pull them back out of the COOKIE ECHO.  As it stands
 *  now we are only supporting IP OPTIONS with our own INIT.
 */
struct __os7_sctp_cookie {
	unsigned long timestamp;	/* timestamp of the cookie */
	unsigned long lifespan;		/* lifespan of the cookie */

	u32 v_tag;			/* local verification tag */

	u32 daddr;			/* dest address */
	u32 saddr;			/* srce address */
	u16 dport;			/* dest port */
	u16 sport;			/* dest port */

	u32 p_tag;			/* peer's verification tag */
	u32 p_tsn;			/* peer initial TSN */
	u32 p_rwnd;			/* perr a_rwnd */

	u16 n_istr;			/* number of inbound streams */
	u16 n_ostr;			/* number of outbound streams */

	u32 l_ttag;			/* local tie tag */
	u32 p_ttag;			/* peer tie tag */

	u16 danum;			/* number of dest transport addresses */
	u16 sanum;			/* number of srce transport addresses */

	u16 key_tag;			/* sender's tag for key */
	u16 opt_len;			/* length of included ip options */

	/* followed by opt.__data */
	/* followed by dtas */
	/* u32 dtas[0]; dest transport addresses */
	/* followed by stas */
	/* u32 stas[0]; srce transport addresses */
	/* followed by mac */
	/* u8 mac[160]; message authentication code */
};

/*
 *  Our HEARTBEAT INFO structure:
 */
struct __os7_sctp_hb_info {
	unsigned long timestamp;	/* jiffies timestamp of when it was sent */
	u32 daddr;			/* destination address sent to */
	int mtu;			/* Destingation MTU being tested */
	unsigned char fill[0];		/* Fill for Destination MTU testing */
};

/*
 *  SCTP Parameter types
 */
#define __OS7_SCTP_PTYPE_MASK		(__constant_htons(0x3fff))
#define __OS7_SCTP_PTYPE_MASK_CONTINUE	(__constant_htons(0x8000))
#define __OS7_SCTP_PTYPE_MASK_REPORT	(__constant_htons(0x4000))
#define __OS7_SCTP_PTYPE_MASK_REP_CONT	(__constant_htons(0xC000))

#define __OS7_SCTP_PTYPE_HEARTBEAT_INFO	(__constant_htons(1))
#define __OS7_SCTP_PTYPE_IPV4_ADDR	(__constant_htons(5))
#define __OS7_SCTP_PTYPE_IPV6_ADDR	(__constant_htons(6))
#define __OS7_SCTP_PTYPE_STATE_COOKIE	(__constant_htons(7))
#define __OS7_SCTP_PTYPE_UNREC_PARMS	(__constant_htons(8))
#define __OS7_SCTP_PTYPE_COOKIE_PSRV	(__constant_htons(9))
#define __OS7_SCTP_PTYPE_HOST_NAME	(__constant_htons(11))
#define __OS7_SCTP_PTYPE_ADDR_TYPE	(__constant_htons(12))
#define __OS7_SCTP_PTYPE_ECN_CAPABLE	(__constant_htons(0)|__OS7_SCTP_PTYPE_MASK_CONTINUE)
#define __OS7_SCTP_PTYPE_PR_SCTP	(__constant_htons(0)|__OS7_SCTP_PTYPE_MASK_REP_CONT)
#define __OS7_SCTP_PTYPE_ADD_IP		(__constant_htons(1)|__OS7_SCTP_PTYPE_MASK_REP_CONT)
#define __OS7_SCTP_PTYPE_DEL_IP		(__constant_htons(2)|__OS7_SCTP_PTYPE_MASK_REP_CONT)
#define __OS7_SCTP_PTYPE_ERROR_CAUSE	(__constant_htons(3)|__OS7_SCTP_PTYPE_MASK_REP_CONT)
#define __OS7_SCTP_PTYPE_SET_IP		(__constant_htons(4)|__OS7_SCTP_PTYPE_MASK_REP_CONT)
#define __OS7_SCTP_PTYPE_SUCCESS_REPORT	(__constant_htons(5)|__OS7_SCTP_PTYPE_MASK_REP_CONT)
#define __OS7_SCTP_PTYPE_ALI		(__constant_htons(6)|__OS7_SCTP_PTYPE_MASK_REP_CONT)

struct __os7_sctpphdr {
	__u16 type;
	__u16 len;
};

struct __os7_sctp_heartbeat_info {
	struct __os7_sctpphdr ph;
	struct __os7_sctp_hb_info hb_info;
};
struct __os7_sctp_ipv4_addr {
	struct __os7_sctpphdr ph;
	u32 addr;
};
struct __os7_sctp_ipv6_addr {
	struct __os7_sctpphdr ph;
};
struct __os7_sctp_state_cookie {
	struct __os7_sctpphdr ph;
	struct __os7_sctp_cookie cookie[0];
};
struct __os7_sctp_unrec_parms {
	struct __os7_sctpphdr ph;
};
struct __os7_sctp_cookie_psrv {
	struct __os7_sctpphdr ph;
	__u32 ck_inc;
};
struct __os7_sctp_host_name {
	struct __os7_sctpphdr ph;
};
struct __os7_sctp_addr_type {
	struct __os7_sctpphdr ph;
	__u16 type[0];
};
struct __os7_sctp_ecn_capable {
	struct __os7_sctpphdr ph;
};
struct __os7_sctp_pr_sctp {
	struct __os7_sctpphdr ph;
};
struct __os7_sctp_ali {
	struct __os7_sctpphdr ph;
	__u32 ali;
};
struct __os7_sctp_add_ip {
	struct __os7_sctpphdr ph;
	__u32 id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct __os7_sctp_del_ip {
	struct __os7_sctpphdr ph;
	__u32 id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct __os7_sctp_set_ip {
	struct __os7_sctpphdr ph;
	__u32 id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct __os7_sctp_error_cause {
	struct __os7_sctpphdr ph;
	__u32 cid;
	/* followed by error causes */
};
struct __os7_sctp_success_report {
	struct __os7_sctpphdr ph;
	__u32 cid;
};
union __os7_sctp_parm {
	struct __os7_sctpphdr ph;
	struct __os7_sctp_heartbeat_info heartbeat_info;
	struct __os7_sctp_ipv4_addr ipv4_addr;
	struct __os7_sctp_ipv6_addr ipv6_addr;
	struct __os7_sctp_state_cookie state_cookie;
	struct __os7_sctp_unrec_parms unrec_parms;
	struct __os7_sctp_cookie_psrv cookie_prsv;
	struct __os7_sctp_host_name host_name;
	struct __os7_sctp_addr_type addr_type;
	struct __os7_sctp_ecn_capable ecn_capable;
	struct __os7_sctp_pr_sctp pr_sctp;
	struct __os7_sctp_ali ali;
	struct __os7_sctp_add_ip add_ip;
	struct __os7_sctp_del_ip del_ip;
	struct __os7_sctp_set_ip set_ip;
	struct __os7_sctp_error_cause error_cause;
	struct __os7_sctp_success_report success_report;
};

/*
 *  SCTP Chunk Types
 */
#define __OS7_SCTP_CTYPE_MASK			0x3f
#define __OS7_SCTP_CTYPE_MASK_CONTINUE		0x80
#define __OS7_SCTP_CTYPE_MASK_REPORT		0x40
#define __OS7_SCTP_CTYPE_MASK_REP_CONT		0xC0

#define __OS7_SCTP_CTYPE_DATA			0
#define __OS7_SCTP_CTYPE_INIT			1
#define __OS7_SCTP_CTYPE_INIT_ACK		2
#define __OS7_SCTP_CTYPE_SACK			3
#define __OS7_SCTP_CTYPE_HEARTBEAT		4
#define __OS7_SCTP_CTYPE_HEARTBEAT_ACK		5
#define __OS7_SCTP_CTYPE_ABORT			6
#define __OS7_SCTP_CTYPE_SHUTDOWN		7
#define __OS7_SCTP_CTYPE_SHUTDOWN_ACK		8
#define __OS7_SCTP_CTYPE_ERROR			9
#define __OS7_SCTP_CTYPE_COOKIE_ECHO		10
#define __OS7_SCTP_CTYPE_COOKIE_ACK		11
#define __OS7_SCTP_CTYPE_ECNE			12
#define __OS7_SCTP_CTYPE_CWR			13
#define __OS7_SCTP_CTYPE_SHUTDOWN_COMPLETE	14
#define __OS7_SCTP_CTYPE_ASCONF_ACK		(0|__OS7_SCTP_CTYPE_MASK_CONTINUE)
#define __OS7_SCTP_CTYPE_ASCONF			(1|__OS7_SCTP_CTYPE_MASK_REP_CONT)
#define __OS7_SCTP_CTYPE_FORWARD_TSN		(0|__OS7_SCTP_CTYPE_MASK_REP_CONT)

/*
 *  SCTP Chunk Structures
 */
struct __os7_sctp_data {
	struct __os7_sctpchdr ch;
	__u32 tsn;			/* Transmit Sequence Number */
	__u16 sid;			/* Stream Identifier */
	__u16 ssn;			/* Stream Sequence Number */
	__u32 ppi;			/* Payload Protocol Identifier */
	__u8 udat[0];			/* User data */
};
struct __os7_sctp_init {
	struct __os7_sctpchdr ch;
	__u32 i_tag;			/* Initiate Tag */
	__u32 a_rwnd;			/* Advertised Received Window Credit */
	__u16 n_ostr;			/* Number of Outbound Streams */
	__u16 n_istr;			/* Number of Inbound Streams */
	__u32 i_tsn;			/* Initial TSN */

};
struct __os7_sctp_init_ack {
	struct __os7_sctpchdr ch;
	__u32 i_tag;			/* Initiate Tag */
	__u32 a_rwnd;			/* Advertised Received Window Credit */
	__u16 n_ostr;			/* Number of Outbound Streams */
	__u16 n_istr;			/* Number of Inbound Streams */
	__u32 i_tsn;			/* Initial TSN */
};
struct __os7_sctp_sack {
	struct __os7_sctpchdr ch;
	__u32 c_tsn;			/* Cumulative TSN Ack */
	__u32 a_rwnd;			/* Advertized Receiver Window Credit */
	__u16 ngaps;			/* Number of Gap Blocks */
	__u16 ndups;			/* Number of Duplicate TSNs */
	__u16 gaps[0];			/* Gap blocks */
	__u32 dups[0];			/* Duplicate TSNs */
};
struct __os7_sctp_heartbeat {
	struct __os7_sctpchdr ch;
};
struct __os7_sctp_heartbeat_ack {
	struct __os7_sctpchdr ch;
};
struct __os7_sctp_abort {
	struct __os7_sctpchdr ch;
	union __os7_sctp_cause cause[0];
};
struct __os7_sctp_shutdown {
	struct __os7_sctpchdr ch;
	__u32 c_tsn;			/* Cummulative TSN Ack */
};
struct __os7_sctp_shutdown_ack {
	struct __os7_sctpchdr ch;
};
struct __os7_sctp_error {
	struct __os7_sctpchdr ch;
	union __os7_sctp_cause cause[0];
};
struct __os7_sctp_cookie_echo {
	struct __os7_sctpchdr ch;
	unsigned char cookie[0];
};
struct __os7_sctp_cookie_ack {
	struct __os7_sctpchdr ch;
};
struct __os7_sctp_ecne {
	struct __os7_sctpchdr ch;
	__u32 l_tsn;
};
struct __os7_sctp_cwr {
	struct __os7_sctpchdr ch;
	__u32 l_tsn;
};
struct __os7_sctp_shutdown_comp {
	struct __os7_sctpchdr ch;
};
struct __os7_sctp_asconf {
	struct __os7_sctpchdr ch;
	__u32 asn;
};
struct __os7_sctp_asconf_ack {
	struct __os7_sctpchdr ch;
	__u32 asn;
};
struct __os7_sctp_forward_tsn {
	struct __os7_sctpchdr ch;
	__u32 f_tsn;
};
union __os7_sctp_chunk {
	struct __os7_sctpchdr ch;
	struct __os7_sctp_data data;
	struct __os7_sctp_init init;
	struct __os7_sctp_init_ack init_ack;
	struct __os7_sctp_sack sack;
	struct __os7_sctp_heartbeat heartbeat;
	struct __os7_sctp_heartbeat_ack heartbeat_ack;
	struct __os7_sctp_abort abort;
	struct __os7_sctp_shutdown shutdown;
	struct __os7_sctp_shutdown_ack shutdown_ack;
	struct __os7_sctp_error error;
	struct __os7_sctp_cookie_echo cookie_echo;
	struct __os7_sctp_cookie_ack cookie_ack;
	struct __os7_sctp_ecne ecne;
	struct __os7_sctp_cwr cwr;
	struct __os7_sctp_shutdown_comp shutdown_comp;
	struct __os7_sctp_asconf asconf;
	struct __os7_sctp_asconf_ack asconf_ack;
	struct __os7_sctp_forward_tsn forward_tsn;
};

/*
 *  SCTP Message Structures
 */
struct __os7_sctp_msg {
	struct __os7_sctphdr mh;
	union __os7_sctp_chunk chunk;
};

/*
 *  Base SCTP states off of TCP states to avoid rewriting a lot of TCP code at
 *  the inet_socket level.
 */

#define __OS7_SCTP_ESTABLISHED		TCP_ESTABLISHED	/* ESTABLISHED */
#define __OS7_SCTP_COOKIE_WAIT		TCP_SYN_SENT	/* COOKIE-WAIT */
#define __OS7_SCTP_COOKIE_ECHOED	TCP_SYN_RECV	/* COOKIE-ECHOED */
#define __OS7_SCTP_SHUTDOWN_PENDING	TCP_FIN_WAIT1	/* SHUTDOWN-PENDING */
#define __OS7_SCTP_SHUTDOWN_SENT	TCP_FIN_WAIT2	/* SHUTDOWN-SENT */
#define __OS7_SCTP_UNREACHABLE		TCP_TIME_WAIT	/* (not used) */
#define __OS7_SCTP_CLOSED		TCP_CLOSE	/* CLOSED */
#define __OS7_SCTP_SHUTDOWN_RECEIVED	TCP_CLOSE_WAIT	/* SHUTDOWN-RECEIVED */
#define __OS7_SCTP_SHUTDOWN_RECVWAIT	TCP_LAST_ACK	/* SHUTDOWN-ACK-SENT (from
							   SHUTDOWN-RECEIVED) */
#define __OS7_SCTP_LISTEN		TCP_LISTEN	/* LISTEN */
#define __OS7_SCTP_SHUTDOWN_ACK_SENT	TCP_CLOSING	/* SHUTDOWN-ACK-SENT (from SHUTDOWN-SENT) */
#define __OS7_SCTP_MAX_STATES		TCP_MAX_STATES

#define __OS7_SCTPF_ESTABLISHED		TCPF_ESTABLISHED
#define __OS7_SCTPF_COOKIE_WAIT		TCPF_SYN_SENT
#define __OS7_SCTPF_COOKIE_ECHOED	TCPF_SYN_RECV
#define __OS7_SCTPF_SHUTDOWN_PENDING	TCPF_FIN_WAIT1
#define __OS7_SCTPF_SHUTDOWN_SENT	TCPF_FIN_WAIT2
#define __OS7_SCTPF_UNREACHABLE		TCPF_TIME_WAIT
#define __OS7_SCTPF_CLOSED		TCPF_CLOSE
#define __OS7_SCTPF_SHUTDOWN_RECEIVED	TCPF_CLOSE_WAIT
#define __OS7_SCTPF_SHUTDOWN_RECVWAIT	TCPF_LAST_ACK
#define __OS7_SCTPF_LISTEN		TCPF_LISTEN
#define __OS7_SCTPF_SHUTDOWN_ACK_SENT	TCPF_CLOSING

#define __OS7_SCTPF_OPENING		(__OS7_SCTPF_COOKIE_WAIT| \
					 __OS7_SCTPF_COOKIE_ECHOED)

#define __OS7_SCTPF_CLOSING		(__OS7_SCTPF_SHUTDOWN_SENT| \
					 __OS7_SCTPF_SHUTDOWN_ACK_SENT)

#define __OS7_SCTPF_CONNECTED		(__OS7_SCTPF_ESTABLISHED| \
					 __OS7_SCTPF_SHUTDOWN_PENDING| \
					 __OS7_SCTPF_SHUTDOWN_RECEIVED| \
					 __OS7_SCTPF_SHUTDOWN_RECVWAIT)

#define	__OS7_SCTPF_DISCONNECTED	(__OS7_SCTPF_CLOSED| \
					 __OS7_SCTPF_LISTEN| \
					 __OS7_SCTPF_UNREACHABLE)

#define __OS7_SCTPF_SENDING		(__OS7_SCTPF_COOKIE_ECHOED| \
					 __OS7_SCTPF_ESTABLISHED| \
					 __OS7_SCTPF_SHUTDOWN_PENDING| \
					 __OS7_SCTPF_SHUTDOWN_RECEIVED| \
					 __OS7_SCTPF_SHUTDOWN_RECVWAIT)

#define __OS7_SCTPF_RECEIVING		(__OS7_SCTPF_COOKIE_ECHOED| \
					 __OS7_SCTPF_ESTABLISHED| \
					 __OS7_SCTPF_SHUTDOWN_PENDING| \
					 __OS7_SCTPF_SHUTDOWN_SENT)

#define __OS7_SCTPF_NEEDABORT		(__OS7_SCTPF_COOKIE_ECHOED| \
					 __OS7_SCTPF_ESTABLISHED| \
					 __OS7_SCTPF_SHUTDOWN_PENDING| \
					 __OS7_SCTPF_SHUTDOWN_RECEIVED| \
					 __OS7_SCTPF_SHUTDOWN_RECVWAIT| \
					 __OS7_SCTPF_SHUTDOWN_SENT)

#define __OS7_SCTPF_HAVEUSER		(__OS7_SCTPF_COOKIE_WAIT| \
					 __OS7_SCTPF_COOKIE_ECHOED| \
					 __OS7_SCTPF_ESTABLISHED| \
					 __OS7_SCTPF_SHUTDOWN_PENDING| \
					 __OS7_SCTPF_SHUTDOWN_RECEIVED| \
					 __OS7_SCTPF_SHUTDOWN_SENT)

#endif				/* __OS7_LINUX_LINUX_SCTP_H__ */
