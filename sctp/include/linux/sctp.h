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

#ifndef _LINUX_SCTP_H
#define _LINUX_SCTP_H

#ident "@(#) sctp.h,v LINUX-2-4-20-SCTP(1.1.6.2) 2004/01/26 12:58:54"

#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/tcp.h>      /* for TCP states */
#include <linux/socket.h>
#include <linux/in.h>

struct sctphdr {
        __u16   srce;
        __u16   dest;
        __u32   v_tag;
        __u32   check;
};

struct sctpchdr {
        __u8    type;
        __u8    flags;
        __u16   len;
};

#define SCTP_SKB_SH(__skb)	((struct sctphdr *)((__skb)->h.raw))
#define SCTP_SKB_SH_SRCE(__skb)	(SCTP_SKB_SH(__skb)->srce)
#define SCTP_SKB_SH_DEST(__skb)	(SCTP_SKB_SH(__skb)->dest)
#define SCTP_SKB_SH_VTAG(__skb)	(SCTP_SKB_SH(__skb)->v_tag)
#define SCTP_SKB_SH_CSUM(__skb)	(SCTP_SKB_SH(__skb)->check)
#define SCTP_SKB_CH(__skb)	((struct sctpchdr *)(__skb)->data)
#define SCTP_SKB_CH_TYPE(__skb) (SCTP_SKB_CH(__skb)->type)
#define SCTP_SKB_CH_LEN(__skb)	(ntohs(SCTP_SKB_CH(__skb)->len))

/*
 *  SCTP Cause Codes
 */
#define SCTP_CAUSE_INVALID_STR      1   /* Invalid Stream Identifier            */
#define SCTP_CAUSE_MISSING_PARM     2   /* Missing Mandatory Parameter          */
#define SCTP_CAUSE_STALE_COOKIE     3   /* Stale Cookie Error                   */
#define SCTP_CAUSE_NO_RESOURCE      4   /* Out of Resource                      */
#define SCTP_CAUSE_BAD_ADDRESS      5   /* Unresolvable Address                 */
#define SCTP_CAUSE_BAD_CHUNK_TYPE   6   /* Unrecognized Chunk Type              */
#define SCTP_CAUSE_INVALID_PARM     7   /* Invalid Mandatory Parameter          */
#define SCTP_CAUSE_BAD_PARM         8   /* Unrecognized Parameters              */
#define SCTP_CAUSE_NO_DATA          9   /* No User Data                         */
#define SCTP_CAUSE_SHUTDOWN         10  /* Cookie Received While Shutting Down  */
#define SCTP_CAUSE_NEW_ADDR         11  /* Restart of an assoc w/ new address   */
#define SCTP_CAUSE_USER_INITIATED   12  /* User Initiated Abort                 */
#define SCTP_CAUSE_PROTO_VIOLATION  13  /* Protocol Violation                   */
#define SCTP_CAUSE_LAST_ADDR      0x100 /* Request to delete last ip address    */
#define SCTP_CAUSE_RES_SHORTAGE   0x101 /* Operation refused resource shortage  */
#define SCTP_CAUSE_SOURCE_ADDR    0x102 /* Request to delete source ip address  */
#define SCTP_CAUSE_ILLEGAL_ASCONF 0x103 /* Assoc. aborted illegal ASCONF-ACK    */

#define SCTP_CF_INVALID_STR      (1<< SCTP_CAUSE_INVALID_STR      )
#define SCTP_CF_MISSING_PARM     (1<< SCTP_CAUSE_MISSING_PARM     )
#define SCTP_CF_STALE_COOKIE     (1<< SCTP_CAUSE_STALE_COOKIE     )
#define SCTP_CF_NO_RESOURCE      (1<< SCTP_CAUSE_NO_RESOURCE      )
#define SCTP_CF_BAD_ADDRESS      (1<< SCTP_CAUSE_BAD_ADDRESS      )
#define SCTP_CF_BAD_CHUNK_TYPE   (1<< SCTP_CAUSE_BAD_CHUNK_TYPE   )
#define SCTP_CF_INVALID_PARM     (1<< SCTP_CAUSE_INVALID_PARM     )
#define SCTP_CF_BAD_PARM         (1<< SCTP_CAUSE_BAD_PARM         )
#define SCTP_CF_NO_DATA          (1<< SCTP_CAUSE_NO_DATA          )
#define SCTP_CF_SHUTDOWN         (1<< SCTP_CAUSE_SHUTDOWN         )

#define SCTP_CF_MASK_ANY	(SCTP_CF_INVALID_STR \
				|SCTP_CF_MISSING_PARM \
				|SCTP_CF_STALE_COOKIE \
				|SCTP_CF_NO_RESOURCE \
				|SCTP_CF_BAD_ADDRESS \
				|SCTP_CF_BAD_CHUNK_TYPE \
				|SCTP_CF_INVALID_PARM \
				|SCTP_CF_BAD_PARM \
				|SCTP_CF_NO_DATA \
				|SCTP_CF_SHUTDOWN )

/*
 *  SCTP Cause Code Structures
 */
struct sctpehdr {
        __u16   code;
        __u16   len;
};
struct sctp_bad_stri { /* Invalid Stream Identifier            */
        struct sctpehdr eh;
        __u16           sid;    /* stream identifier */
        __u16           res;    /* reserved          */
};
struct sctp_no_mparm { /* Missing Mandatory Parameter          */
        struct sctpehdr eh;
        __u32           num;    /* number of missing parms */
        __u16           mp[0];  /* array of missing parms  */
};
struct sctp_stale_ck { /* Stale Cookie Error                   */
        struct sctpehdr eh;
        __u32           staleness;  /* measure of staleness (usec) */
};
struct sctp_no_rsrce { /* Out of Resource                      */
        struct sctpehdr eh;
};
struct sctp_bad_addr { /* Unresolvable Address                 */
        struct sctpehdr eh;
        /* address parameter */
};
struct sctp_bad_chnk { /* Unrecognized Chunk Type              */
        struct sctpehdr eh;
        struct sctpchdr ch; /* header of unrecognized chunk */
};
struct sctp_bad_parm { /* Invalid Mandatory Parameter          */
        struct sctpehdr eh;
};
struct sctp_unk_parm { /* Unrecognized Parameters              */
        struct sctpehdr eh;
        unsigned char   parm[0];    /* unrecognized parameters */
};
struct sctp_no_udata { /* No user Data                         */
        struct sctpehdr eh;
        __u32           tsn;    /* tsn of data chunk */
};
struct sctp_ck_shutd { /* Cookie Received While Shutting Down  */
        struct sctpehdr eh;
};
struct sctp_last_add { /* Request to delete last address       */
        struct sctpehdr eh;
	__u32		cid;
	/* followed by ASCONF TLV */
};
struct sctp_op_short { /* Operation refused resource shortage  */
        struct sctpehdr eh;
	__u32		cid;
	/* followed by ASCONF TLV */
};
struct sctp_srce_add { /* Request to delete source address     */
        struct sctpehdr eh;
	__u32		cid;
	/* followed by ASCONF TLV */
};
struct sctp_bad_conf { /* Illegal ASCONF-ACK                   */
        struct sctpehdr eh;
};
union sctp_cause {
        struct sctpehdr         eh;
        struct sctp_bad_stri    bad_stri;   /* Invalid Stream Identifier            */
        struct sctp_no_mparm    no_mparm;   /* Missing Mandatory Parameter          */
        struct sctp_stale_ck    stale_ck;   /* Stale Cookie Error                   */
        struct sctp_no_rsrce    no_rsrce;   /* Out of Resource                      */
        struct sctp_bad_addr    bad_addr;   /* Unresolvable Address                 */
        struct sctp_bad_chnk    bad_chnk;   /* Unrecognized Chunk Type              */
        struct sctp_bad_parm    bad_parm;   /* Invalid Mandatory Parameter          */
        struct sctp_unk_parm    unk_parm;   /* Unrecognized Parameters              */
        struct sctp_no_udata    no_udata;   /* No user Data                         */
        struct sctp_ck_shutd    ck_shutd;   /* Cookie Received While Shutting Down  */
};

/*
 *  COOKIE parameter
 *
 *  TODO: Put IP OPTIONS (struct ip_options + __data) from the INIT message
 *  into the cookie and pull them back out of the COOKIE ECHO.  As it stands
 *  now we are only supporting IP OPTIONS with our own INIT.
 */
struct sctp_cookie {
        unsigned long   timestamp;  /* timestamp of the cookie              */
        unsigned long   lifespan;   /* lifespan  of the cookie              */

        u32             v_tag;      /* local  verification tag              */

	u32		daddr;	    /* dest address			    */
	u32		saddr;	    /* srce address			    */
	u16		dport;	    /* dest port			    */
	u16		sport;	    /* dest port			    */

        u32             p_tag;      /* peer's verification tag              */
	u32		p_tsn;	    /* peer initial TSN			    */
	u32		p_rwnd;	    /* perr a_rwnd			    */

	u16		n_istr;	    /* number of  inbound streams	    */
	u16		n_ostr;	    /* number of outbound streams	    */

        u32             l_ttag;     /* local tie tag                        */
        u32             p_ttag;     /* peer  tie tag                        */

	u16		danum;	    /* number of dest transport addresses   */
	u16		sanum;	    /* number of srce transport addresses   */

        u16             key_tag;    /* sender's tag for key                 */
	u16		opt_len;    /* length of included ip options	    */

	/* followed by opt.__data */
	/* followed by dtas */
   /*	u32		dtas[0];       dest transport addresses		    */
	/* followed by stas */
   /*	u32		stas[0];       srce transport addresses		    */
	/* followed by mac */
   /*   u8              mac[160];      message authentication code          */
};

/*
 *  Our HEARTBEAT INFO structure:
 */
struct sctp_hb_info {
        unsigned long   timestamp;  /* jiffies timestamp of when it was sent */
        u32             daddr;      /* destination address sent to           */
        int             mtu;        /* Destingation MTU being tested         */
        unsigned char   fill[0];    /* Fill for Destination MTU testing      */
};

/*
 *  SCTP Parameter types
 */
#define SCTP_PTYPE_MASK			(__constant_htons(0x3fff))
#define	SCTP_PTYPE_MASK_CONTINUE	(__constant_htons(0x8000))
#define SCTP_PTYPE_MASK_REPORT		(__constant_htons(0x4000))
#define SCTP_PTYPE_MASK_REP_CONT	(__constant_htons(0xC000))

#define SCTP_PTYPE_HEARTBEAT_INFO	(__constant_htons(1))
#define SCTP_PTYPE_IPV4_ADDR		(__constant_htons(5))
#define SCTP_PTYPE_IPV6_ADDR		(__constant_htons(6))
#define SCTP_PTYPE_STATE_COOKIE		(__constant_htons(7))
#define SCTP_PTYPE_UNREC_PARMS		(__constant_htons(8))
#define SCTP_PTYPE_COOKIE_PSRV		(__constant_htons(9))
#define SCTP_PTYPE_HOST_NAME		(__constant_htons(11))
#define SCTP_PTYPE_ADDR_TYPE		(__constant_htons(12))
#define SCTP_PTYPE_ECN_CAPABLE		(__constant_htons(0)|SCTP_PTYPE_MASK_CONTINUE)
#define SCTP_PTYPE_PR_SCTP		(__constant_htons(0)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_ADD_IP		(__constant_htons(1)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_DEL_IP		(__constant_htons(2)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_ERROR_CAUSE		(__constant_htons(3)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_SET_IP		(__constant_htons(4)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_SUCCESS_REPORT	(__constant_htons(5)|SCTP_PTYPE_MASK_REP_CONT)
#define SCTP_PTYPE_ALI			(__constant_htons(6)|SCTP_PTYPE_MASK_REP_CONT)

struct sctpphdr {
        __u16   type;
        __u16   len;
};

struct sctp_heartbeat_info {
        struct sctpphdr     ph;
        struct sctp_hb_info hb_info;
};
struct sctp_ipv4_addr {
        struct sctpphdr     ph;
        u32                 addr;
};
struct sctp_ipv6_addr {
        struct sctpphdr     ph;
};
struct sctp_state_cookie {
        struct sctpphdr     ph;
        struct sctp_cookie  cookie[0];
};
struct sctp_unrec_parms {
        struct sctpphdr     ph;
};
struct sctp_cookie_psrv {
        struct sctpphdr     ph;
        __u32               ck_inc;
};
struct sctp_host_name {
        struct sctpphdr     ph;
};
struct sctp_addr_type {
        struct sctpphdr     ph;
        __u16               type[0];
};
struct sctp_ecn_capable {
	struct sctpphdr	    ph;
};
struct sctp_pr_sctp {
	struct sctpphdr	    ph;
};
struct sctp_ali {
	struct sctpphdr	    ph;
	__u32		    ali;
};
struct sctp_add_ip {
	struct sctpphdr	    ph;
	__u32		    id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct sctp_del_ip {
	struct sctpphdr	    ph;
	__u32		    id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct sctp_set_ip {
	struct sctpphdr	    ph;
	__u32		    id;
	/* followed by ipv4 or ipv6 address parameter */
};
struct sctp_error_cause {
	struct sctpphdr	    ph;
	__u32		    cid;
	/* followed by error causes */
};
struct sctp_success_report {
	struct sctpphdr	    ph;
	__u32		    cid;
};
union sctp_parm {
        struct sctpphdr             ph;
        struct sctp_heartbeat_info  heartbeat_info;
        struct sctp_ipv4_addr       ipv4_addr;
        struct sctp_ipv6_addr       ipv6_addr;
        struct sctp_state_cookie    state_cookie;
        struct sctp_unrec_parms     unrec_parms;
        struct sctp_cookie_psrv     cookie_prsv;
        struct sctp_host_name       host_name;
        struct sctp_addr_type       addr_type;
	struct sctp_ecn_capable	    ecn_capable;
	struct sctp_pr_sctp	    pr_sctp;
	struct sctp_ali		    ali;
	struct sctp_add_ip	    add_ip;
	struct sctp_del_ip	    del_ip;
	struct sctp_set_ip	    set_ip;
	struct sctp_error_cause	    error_cause;
	struct sctp_success_report  success_report;
};

/*
 *  SCTP Chunk Types
 */
#define SCTP_CTYPE_MASK			0x3f
#define SCTP_CTYPE_MASK_CONTINUE	0x80
#define SCTP_CTYPE_MASK_REPORT		0x40
#define SCTP_CTYPE_MASK_REP_CONT	0xC0

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
#define SCTP_CTYPE_ASCONF_ACK		(0|SCTP_CTYPE_MASK_CONTINUE)
#define SCTP_CTYPE_ASCONF		(1|SCTP_CTYPE_MASK_REP_CONT)
#define SCTP_CTYPE_FORWARD_TSN		(0|SCTP_CTYPE_MASK_REP_CONT)

/*
 *  SCTP Chunk Structures
 */
struct sctp_data {
        struct sctpchdr ch;
        __u32           tsn;    /* Transmit Sequence Number          */
        __u16           sid;    /* Stream Identifier                 */
        __u16           ssn;    /* Stream Sequence Number            */
        __u32           ppi;    /* Payload Protocol Identifier       */
        __u8        udat[0];    /* User data                         */
};
struct sctp_init {
        struct sctpchdr ch;
        __u32           i_tag;  /* Initiate Tag                      */
        __u32           a_rwnd; /* Advertised Received Window Credit */
        __u16           n_ostr; /* Number of Outbound Streams        */
        __u16           n_istr; /* Number of Inbound  Streams        */
        __u32           i_tsn;  /* Initial TSN                       */
        
};
struct sctp_init_ack {
        struct sctpchdr ch;
	__u32		i_tag;	/* Initiate Tag			     */
	__u32		a_rwnd;	/* Advertised Received Window Credit */
	__u16		n_ostr;	/* Number of Outbound Streams	     */
	__u16		n_istr;	/* Number of Inbound  Streams	     */
	__u32		i_tsn;	/* Initial TSN			     */
};
struct sctp_sack {
        struct sctpchdr	ch;
	__u32		c_tsn;	/* Cumulative TSN Ack		     */
	__u32		a_rwnd;	/* Advertized Receiver Window Credit */
	__u16		ngaps;	/* Number of Gap Blocks		     */
	__u16		ndups;	/* Number of Duplicate TSNs	     */
	__u16           gaps[0];/* Gap blocks			     */
	__u32		dups[0];/* Duplicate TSNs		     */
};
struct sctp_heartbeat {
        struct sctpchdr ch;
};
struct sctp_heartbeat_ack {
        struct sctpchdr ch;
};
struct sctp_abort {
        struct sctpchdr	ch;
        union sctp_cause cause[0];
};
struct sctp_shutdown {
        struct sctpchdr ch;
	__u32		c_tsn;	/* Cummulative TSN Ack		     */
};
struct sctp_shutdown_ack {
        struct sctpchdr ch;
};
struct sctp_error {
        struct sctpchdr	ch;
        union sctp_cause cause[0];
};
struct sctp_cookie_echo {
        struct sctpchdr	ch;
        unsigned char	cookie[0];
};
struct sctp_cookie_ack {
        struct sctpchdr ch;
};
struct sctp_ecne {
        struct sctpchdr	ch;
	__u32		l_tsn;
};
struct sctp_cwr {
        struct sctpchdr ch;
	__u32		l_tsn;
};
struct sctp_shutdown_comp {
	struct sctpchdr	ch;
};
struct sctp_asconf {
	struct sctpchdr ch;
	__u32		asn;
};
struct sctp_asconf_ack {
	struct sctpchdr ch;
	__u32		asn;
};
struct sctp_forward_tsn {
	struct sctpchdr ch;
	__u32		f_tsn;
};
union sctp_chunk {
        struct sctpchdr ch;
        struct sctp_data            data;
        struct sctp_init            init;
        struct sctp_init_ack        init_ack;
        struct sctp_sack            sack;
        struct sctp_heartbeat       heartbeat;
        struct sctp_heartbeat_ack   heartbeat_ack;
        struct sctp_abort           abort;
        struct sctp_shutdown        shutdown;
        struct sctp_shutdown_ack    shutdown_ack;
        struct sctp_error           error;
        struct sctp_cookie_echo     cookie_echo;
        struct sctp_cookie_ack      cookie_ack;
        struct sctp_ecne            ecne;
        struct sctp_cwr             cwr;
        struct sctp_shutdown_comp   shutdown_comp;
	struct sctp_asconf	    asconf;
	struct sctp_asconf_ack	    asconf_ack;
	struct sctp_forward_tsn	    forward_tsn;
};

/*
 *  SCTP Message Structures
 */
struct sctp_msg {
        struct sctphdr      mh;
        union sctp_chunk    chunk;
};

/*
 *  Base SCTP states off of TCP states to avoid rewriting a lot of TCP code at
 *  the inet_socket level.
 */

#define SCTP_ESTABLISHED         TCP_ESTABLISHED     /* ESTABLISHED       */
#define SCTP_COOKIE_WAIT         TCP_SYN_SENT        /* COOKIE-WAIT       */
#define SCTP_COOKIE_ECHOED       TCP_SYN_RECV        /* COOKIE-ECHOED     */
#define SCTP_SHUTDOWN_PENDING    TCP_FIN_WAIT1       /* SHUTDOWN-PENDING  */
#define SCTP_SHUTDOWN_SENT       TCP_FIN_WAIT2       /* SHUTDOWN-SENT     */
#define SCTP_UNREACHABLE         TCP_TIME_WAIT       /* (not used)        */
#define SCTP_CLOSED              TCP_CLOSE           /* CLOSED            */
#define SCTP_SHUTDOWN_RECEIVED   TCP_CLOSE_WAIT      /* SHUTDOWN-RECEIVED */
#define SCTP_SHUTDOWN_RECVWAIT	 TCP_LAST_ACK        /* SHUTDOWN-ACK-SENT (from SHUTDOWN-RECEIVED) */
#define SCTP_LISTEN              TCP_LISTEN          /* LISTEN            */
#define SCTP_SHUTDOWN_ACK_SENT   TCP_CLOSING         /* SHUTDOWN-ACK-SENT (from SHUTDOWN-SENT)     */
#define SCTP_MAX_STATES          TCP_MAX_STATES

#define SCTPF_ESTABLISHED        TCPF_ESTABLISHED
#define SCTPF_COOKIE_WAIT        TCPF_SYN_SENT
#define SCTPF_COOKIE_ECHOED      TCPF_SYN_RECV
#define SCTPF_SHUTDOWN_PENDING   TCPF_FIN_WAIT1
#define SCTPF_SHUTDOWN_SENT      TCPF_FIN_WAIT2
#define SCTPF_UNREACHABLE        TCPF_TIME_WAIT
#define SCTPF_CLOSED             TCPF_CLOSE
#define SCTPF_SHUTDOWN_RECEIVED  TCPF_CLOSE_WAIT
#define SCTPF_SHUTDOWN_RECVWAIT	 TCPF_LAST_ACK
#define SCTPF_LISTEN             TCPF_LISTEN
#define SCTPF_SHUTDOWN_ACK_SENT	 TCPF_CLOSING

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

#endif  /* _LINUX_SCTP_H */
