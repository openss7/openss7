/*****************************************************************************

 @(#) $Id: os7_namespace.h,v 0.9.2.4 2005/07/18 11:56:31 brian Exp $

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

 Last Modified $Date: 2005/07/18 11:56:31 $ by $Author: brian $

 *****************************************************************************/

#ifndef __OS7_OS7_NAMESPACE_H__
#define __OS7_OS7_NAMESPACE_H__

#ident "@(#) $RCSfile: os7_namespace.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2004 OpenSS7 Corporation."

#undef NET_SCTP_ADAPTATION_LAYER_INFO
#undef NET_SCTP_ASSOC_MAX_RETRANS
#undef NET_SCTP_COOKIE_INC
#undef NET_SCTP_CSUM_TYPE
#undef NET_SCTP_ECN
#undef NET_SCTP_HEARTBEAT_ITVL
#undef NET_SCTP_INIT_RETRIES
#undef NET_SCTP_MAC_TYPE
#undef NET_SCTP_MAX_BURST
#undef NET_SCTP_MAX_ISTREAMS
#undef NET_SCTP_MAX_SACK_DELAY
#undef NET_SCTP_MEM
#undef NET_SCTP_PARTIAL_RELIABILITY
#undef NET_SCTP_PATH_MAX_RETRANS
#undef NET_SCTP_REQ_OSTREAMS
#undef NET_SCTP_RMEM
#undef NET_SCTP_RTO_INITIAL
#undef NET_SCTP_RTO_MAX
#undef NET_SCTP_RTO_MIN
#undef NET_SCTP_THROTTLE_ITVL
#undef NET_SCTP_VALID_COOKIE_LIFE
#undef NET_SCTP_WMEM

#undef sctp_abort
#undef sctp_add_ip
#undef SCTP_ADD_IP
#undef SCTP_ADD
#undef sctp_addr_type
#undef sctp_ali
#undef SCTP_ALI
#undef sctp_asconf_ack
#undef sctp_asconf
#undef SCTP_ASSOC_MAX_RETRANS
#undef sctp_astat
#undef sctp_bad_addr
#undef sctp_bad_chnk
#undef sctp_bad_conf
#undef sctp_bad_parm
#undef sctp_bad_stri
#undef sctp_bind_bucket
#undef sctp_bind_cachep
#undef SCTP_CAPS_ADD_IP
#undef SCTP_CAPS_ALI
#undef SCTP_CAPS_ECN
#undef SCTP_CAPS_PR
#undef SCTP_CAPS_SET_IP
#undef SCTP_CAUSE_BAD_ADDRESS
#undef SCTP_CAUSE_BAD_CHUNK_TYPE
#undef SCTP_CAUSE_BAD_PARM
#undef SCTP_CAUSE_ILLEGAL_ASCONF
#undef SCTP_CAUSE_INVALID_PARM
#undef SCTP_CAUSE_INVALID_STR
#undef SCTP_CAUSE_LAST_ADDR
#undef SCTP_CAUSE_MISSING_PARM
#undef SCTP_CAUSE_NEW_ADDR
#undef SCTP_CAUSE_NO_DATA
#undef SCTP_CAUSE_NO_RESOURCE
#undef sctp_cause
#undef SCTP_CAUSE_PROTO_VIOLATION
#undef SCTP_CAUSE_RES_SHORTAGE
#undef SCTP_CAUSE_SHUTDOWN
#undef SCTP_CAUSE_SOURCE_ADDR
#undef SCTP_CAUSE_STALE_COOKIE
#undef SCTP_CAUSE_USER_INITIATED
#undef SCTPCB_FLAG_ACK
#undef SCTPCB_FLAG_CKSUMMED
#undef SCTPCB_FLAG_CONF
#undef SCTPCB_FLAG_DELIV
#undef SCTPCB_FLAG_DROPPED
#undef SCTPCB_FLAG_FIRST_FRAG
#undef SCTPCB_FLAG_LAST_FRAG
#undef SCTPCB_FLAG_NACK
#undef SCTPCB_FLAG_RETRANS
#undef SCTPCB_FLAG_SACKED
#undef SCTPCB_FLAG_SENT
#undef SCTPCB_FLAG_URG
#undef SCTP_CF_BAD_ADDRESS
#undef SCTP_CF_BAD_CHUNK_TYPE
#undef SCTP_CF_BAD_PARM
#undef SCTP_CF_INVALID_PARM
#undef SCTP_CF_INVALID_STR
#undef SCTP_CF_MASK_ANY
#undef SCTP_CF_MISSING_PARM
#undef SCTP_CF_NO_DATA
#undef SCTP_CF_NO_RESOURCE
#undef SCTP_CF_SHUTDOWN
#undef SCTP_CF_STALE_COOKIE
#undef sctpchdr
#undef sctp_chunk
#undef sctp_ck_shutd
#undef SCTP_CKSUM_TYPE
#undef SCTP_CLOSED
#undef SCTP_CMSGF_RECVPPI
#undef SCTP_CMSGF_RECVSID
#undef SCTP_CMSGF_RECVSSN
#undef SCTP_CMSGF_RECVTSN
#undef sctp_cookie_ack
#undef SCTP_COOKIE_ECHOED
#undef sctp_cookie_echo
#undef SCTP_COOKIE_INC
#undef SCTP_COOKIE_LIFE
#undef sctp_cookie
#undef sctp_cookie_psrv
#undef SCTP_COOKIE_WAIT
#undef SCTP_CORK
#undef SCTP_CSUM_ADLER32
#undef SCTP_CSUM_CRC32C
#undef SCTP_CTYPE_ABORT
#undef SCTP_CTYPE_ASCONF_ACK
#undef SCTP_CTYPE_ASCONF
#undef SCTP_CTYPE_COOKIE_ACK
#undef SCTP_CTYPE_COOKIE_ECHO
#undef SCTP_CTYPE_CWR
#undef SCTP_CTYPE_DATA
#undef SCTP_CTYPE_ECNE
#undef SCTP_CTYPE_ERROR
#undef SCTP_CTYPE_FORWARD_TSN
#undef SCTP_CTYPE_HEARTBEAT_ACK
#undef SCTP_CTYPE_HEARTBEAT
#undef SCTP_CTYPE_INIT_ACK
#undef SCTP_CTYPE_INIT
#undef SCTP_CTYPE_MASK_CONTINUE
#undef SCTP_CTYPE_MASK
#undef SCTP_CTYPE_MASK_REP_CONT
#undef SCTP_CTYPE_MASK_REPORT
#undef SCTP_CTYPE_SACK
#undef SCTP_CTYPE_SHUTDOWN_ACK
#undef SCTP_CTYPE_SHUTDOWN_COMPLETE
#undef SCTP_CTYPE_SHUTDOWN
#undef sctp_cwr
#undef sctp_daddr
#undef sctp_data
#undef SCTP_DEBUG_OPTIONS
#undef sctp_del_ip
#undef SCTP_DEL_IP
#undef sctp_dest_cachep
#undef SCTP_DESTF_CONGESTD
#undef SCTP_DESTF_DROPPING
#undef SCTP_DESTF_FORWDTSN
#undef SCTP_DESTF_HBACTIVE
#undef SCTP_DESTF_INACTIVE
#undef SCTP_DESTF_PMTUDISC
#undef SCTP_DESTF_ROUTFAIL
#undef SCTP_DESTF_TIMEDOUT
#undef SCTP_DESTF_UNUSABLE
#undef SCTP_DESTM_CANT_USE
#undef SCTP_DESTM_DONT_USE
#undef SCTP_DISPOSITION_ACKED
#undef SCTP_DISPOSITION_GAP_ACKED
#undef SCTP_DISPOSITION_NONE
#undef SCTP_DISPOSITION
#undef SCTP_DISPOSITION_SENT
#undef SCTP_DISPOSITION_UNSENT
#undef sctp_dstat
#undef sctp_dup
#undef sctp_ecn_capable
#undef sctp_ecne
#undef SCTP_ECN
#undef sctpehdr
#undef sctp_error_cause
#undef sctp_error
#undef SCTP_ESTABLISHED
#undef SCTPF_CLOSED
#undef SCTPF_CLOSING
#undef SCTPF_CONNECTED
#undef SCTPF_COOKIE_ECHOED
#undef SCTPF_COOKIE_WAIT
#undef SCTPF_DISCONNECTED
#undef SCTPF_ESTABLISHED
#undef SCTPF_HAVEUSER
#undef SCTP_FLAG_DEFAULT_RC_SEL
#undef SCTP_FLAG_EX_DATA_OPT
#undef SCTP_FLAG_NEED_CLEANUP
#undef SCTP_FLAG_REC_CONF_OPT
#undef SCTPF_LISTEN
#undef SCTPF_NEEDABORT
#undef SCTPF_OPENING
#undef sctp_forward_tsn
#undef SCTPF_RECEIVING
#undef SCTPF_SENDING
#undef SCTPF_SHUTDOWN_ACK_SENT
#undef SCTPF_SHUTDOWN_PENDING
#undef SCTPF_SHUTDOWN_RECEIVED
#undef SCTPF_SHUTDOWN_RECVWAIT
#undef SCTPF_SHUTDOWN_SENT
#undef SCTPF_UNREACHABLE
#undef sctp_hb_info
#undef sctp_hbitvl
#undef SCTP_HB
#undef sctphdr
#undef sctp_heartbeat_ack
#undef sctp_heartbeat_info
#undef SCTP_HEARTBEAT_ITVL
#undef sctp_heartbeat
#undef SCTP_HMAC_MD5
#undef SCTP_HMAC_NONE
#undef SCTP_HMAC_SHA_1
#undef sctp_host_name
#undef sctp_init_ack
#undef sctp_init
#undef sctp_ipv4_addr
#undef sctp_ipv6_addr
#undef SCTP_ISTREAMS
#undef sctp_last_add
#undef SCTP_LIFETIME
#undef sctp_listen
#undef SCTP_LISTEN
#undef SCTP_MAC_TYPE
#undef SCTP_MAX_BURST
#undef SCTP_MAX_INIT_RETRIES
#undef SCTP_MAXSEG
#undef SCTP_MAX_STATES
#undef sctp_mib
#undef sctp_msg
#undef SCTP_NODELAY
#undef sctp_no_mparm
#undef sctp_no_rsrce
#undef sctp_no_udata
#undef sctp_op_short
#undef SCTP_OPTION_BREAK
#undef SCTP_OPTION_DBREAK
#undef SCTP_OPTION_DROPPING
#undef SCTP_OPTION_RANDOM
#undef sctp_opt
#undef sctp_orphan_count
#undef SCTP_OSTREAMS
#undef sctp_parm
#undef SCTP_PATH_MAX_RETRANS
#undef sctpphdr
#undef sctp_poll
#undef SCTP_PPI
#undef SCTP_PR_NONE
#undef SCTP_PR
#undef sctp_prot_hook
#undef sctp_protolist
#undef sctp_prot
#undef SCTP_PR_PREFERRED
#undef SCTP_PR_REQUIRED
#undef sctp_pr_sctp
#undef SCTP_PTYPE_ADD_IP
#undef SCTP_PTYPE_ADDR_TYPE
#undef SCTP_PTYPE_ALI
#undef SCTP_PTYPE_COOKIE_PSRV
#undef SCTP_PTYPE_DEL_IP
#undef SCTP_PTYPE_ECN_CAPABLE
#undef SCTP_PTYPE_ERROR_CAUSE
#undef SCTP_PTYPE_HEARTBEAT_INFO
#undef SCTP_PTYPE_HOST_NAME
#undef SCTP_PTYPE_IPV4_ADDR
#undef SCTP_PTYPE_IPV6_ADDR
#undef SCTP_PTYPE_MASK_CONTINUE
#undef SCTP_PTYPE_MASK
#undef SCTP_PTYPE_MASK_REP_CONT
#undef SCTP_PTYPE_MASK_REPORT
#undef SCTP_PTYPE_PR_SCTP
#undef SCTP_PTYPE_SET_IP
#undef SCTP_PTYPE_STATE_COOKIE
#undef SCTP_PTYPE_SUCCESS_REPORT
#undef SCTP_PTYPE_UNREC_PARMS
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
#undef SCTP_SACKF_ANY
#undef SCTP_SACKF_ASC
#undef SCTP_SACKF_CWR
#undef SCTP_SACKF_DUP
#undef SCTP_SACKF_ECN
#undef SCTP_SACKF_FSN
#undef SCTP_SACKF_GAP
#undef SCTP_SACKF_NEW
#undef SCTP_SACKF_NOD
#undef SCTP_SACKF_NOW
#undef SCTP_SACKF_TIM
#undef SCTP_SACKF_URG
#undef SCTP_SACKF_WND
#undef SCTP_SACKF_WUP
#undef sctp_sack
#undef sctp_saddr
#undef sctp_set_ip
#undef SCTP_SET_IP
#undef SCTP_SET
#undef sctp_shutdown_ack
#undef SCTP_SHUTDOWN_ACK_SENT
#undef sctp_shutdown_comp
#undef sctp_shutdown
#undef SCTP_SHUTDOWN_PENDING
#undef SCTP_SHUTDOWN_RECEIVED
#undef SCTP_SHUTDOWN_RECVWAIT
#undef SCTP_SHUTDOWN_SENT
#undef SCTP_SID
#undef sctp_skb_cb
#undef sctp_socket_count
#undef sctp_srce_add
#undef sctp_srce_cachep
#undef SCTP_SRCEF_ADD_PENDING
#undef SCTP_SRCEF_ADD_REQUEST
#undef SCTP_SRCEF_DEL_PENDING
#undef SCTP_SRCEF_DEL_REQUEST
#undef SCTP_SRCEF_SET_PENDING
#undef SCTP_SRCEF_SET_REQUEST
#undef SCTP_SRCEM_ASCONF
#undef SCTP_SSN
#undef sctp_stale_ck
#undef sctp_state_cookie
#undef sctp_statistics
#undef SCTP_STATUS
#undef sctp_strm_cachep
#undef SCTP_STRMF_DROP
#undef SCTP_STRMF_MORE
#undef sctp_strm
#undef sctp_success_report
#undef SCTP_THROTTLE_ITVL
#undef SCTP_TSN
#undef sctp_unk_parm
#undef SCTP_UNREACHABLE
#undef sctp_unrec_parms
#undef sctp_v4_err
#undef sctp_v4_rcv

#define NET_SCTP_ADAPTATION_LAYER_INFO	__OS7_NET_SCTP_ADAPTATION_LAYER_INFO
#define NET_SCTP_ASSOC_MAX_RETRANS	__OS7_NET_SCTP_ASSOC_MAX_RETRANS
#define NET_SCTP_COOKIE_INC		__OS7_NET_SCTP_COOKIE_INC
#define NET_SCTP_CSUM_TYPE		__OS7_NET_SCTP_CSUM_TYPE
#define NET_SCTP_ECN			__OS7_NET_SCTP_ECN
#define NET_SCTP_HEARTBEAT_ITVL		__OS7_NET_SCTP_HEARTBEAT_ITVL
#define NET_SCTP_INIT_RETRIES		__OS7_NET_SCTP_INIT_RETRIES
#define NET_SCTP_MAC_TYPE		__OS7_NET_SCTP_MAC_TYPE
#define NET_SCTP_MAX_BURST		__OS7_NET_SCTP_MAX_BURST
#define NET_SCTP_MAX_ISTREAMS		__OS7_NET_SCTP_MAX_ISTREAMS
#define NET_SCTP_MAX_SACK_DELAY		__OS7_NET_SCTP_MAX_SACK_DELAY
#define NET_SCTP_MEM			__OS7_NET_SCTP_MEM
#define NET_SCTP_PARTIAL_RELIABILITY	__OS7_NET_SCTP_PARTIAL_RELIABILITY
#define NET_SCTP_PATH_MAX_RETRANS	__OS7_NET_SCTP_PATH_MAX_RETRANS
#define NET_SCTP_REQ_OSTREAMS		__OS7_NET_SCTP_REQ_OSTREAMS
#define NET_SCTP_RMEM			__OS7_NET_SCTP_RMEM
#define NET_SCTP_RTO_INITIAL		__OS7_NET_SCTP_RTO_INITIAL
#define NET_SCTP_RTO_MAX		__OS7_NET_SCTP_RTO_MAX
#define NET_SCTP_RTO_MIN		__OS7_NET_SCTP_RTO_MIN
#define NET_SCTP_THROTTLE_ITVL		__OS7_NET_SCTP_THROTTLE_ITVL
#define NET_SCTP_VALID_COOKIE_LIFE	__OS7_NET_SCTP_VALID_COOKIE_LIFE
#define NET_SCTP_WMEM			__OS7_NET_SCTP_WMEM

#define sctp_abort			__os7_sctp_abort
#define sctp_add_ip			__os7_sctp_add_ip
#define SCTP_ADD_IP			__OS7_SCTP_ADD_IP
#define SCTP_ADD			__OS7_SCTP_ADD
#define sctp_addr_type			__os7_sctp_addr_type
#define sctp_ali			__os7_sctp_ali
#define SCTP_ALI			__OS7_SCTP_ALI
#define sctp_asconf_ack			__os7_sctp_asconf_ack
#define sctp_asconf			__os7_sctp_asconf
#define SCTP_ASSOC_MAX_RETRANS		__OS7_SCTP_ASSOC_MAX_RETRANS
#define sctp_astat			__os7_sctp_astat
#define sctp_bad_addr			__os7_sctp_bad_addr
#define sctp_bad_chnk			__os7_sctp_bad_chnk
#define sctp_bad_conf			__os7_sctp_bad_conf
#define sctp_bad_parm			__os7_sctp_bad_parm
#define sctp_bad_stri			__os7_sctp_bad_stri
#define sctp_bind_bucket		__os7_sctp_bind_bucket
#define sctp_bind_cachep		__os7_sctp_bind_cachep
#define SCTP_CAPS_ADD_IP		__OS7_SCTP_CAPS_ADD_IP
#define SCTP_CAPS_ALI			__OS7_SCTP_CAPS_ALI
#define SCTP_CAPS_ECN			__OS7_SCTP_CAPS_ECN
#define SCTP_CAPS_PR			__OS7_SCTP_CAPS_PR
#define SCTP_CAPS_SET_IP		__OS7_SCTP_CAPS_SET_IP
#define SCTP_CAUSE_BAD_ADDRESS		__OS7_SCTP_CAUSE_BAD_ADDRESS
#define SCTP_CAUSE_BAD_CHUNK_TYPE	__OS7_SCTP_CAUSE_BAD_CHUNK_TYPE
#define SCTP_CAUSE_BAD_PARM		__OS7_SCTP_CAUSE_BAD_PARM
#define SCTP_CAUSE_ILLEGAL_ASCONF	__OS7_SCTP_CAUSE_ILLEGAL_ASCONF
#define SCTP_CAUSE_INVALID_PARM		__OS7_SCTP_CAUSE_INVALID_PARM
#define SCTP_CAUSE_INVALID_STR		__OS7_SCTP_CAUSE_INVALID_STR
#define SCTP_CAUSE_LAST_ADDR		__OS7_SCTP_CAUSE_LAST_ADDR
#define SCTP_CAUSE_MISSING_PARM		__OS7_SCTP_CAUSE_MISSING_PARM
#define SCTP_CAUSE_NEW_ADDR		__OS7_SCTP_CAUSE_NEW_ADDR
#define SCTP_CAUSE_NO_DATA		__OS7_SCTP_CAUSE_NO_DATA
#define SCTP_CAUSE_NO_RESOURCE		__OS7_SCTP_CAUSE_NO_RESOURCE
#define sctp_cause			__os7_sctp_cause
#define SCTP_CAUSE_PROTO_VIOLATION	__OS7_SCTP_CAUSE_PROTO_VIOLATION
#define SCTP_CAUSE_RES_SHORTAGE		__OS7_SCTP_CAUSE_RES_SHORTAGE
#define SCTP_CAUSE_SHUTDOWN		__OS7_SCTP_CAUSE_SHUTDOWN
#define SCTP_CAUSE_SOURCE_ADDR		__OS7_SCTP_CAUSE_SOURCE_ADDR
#define SCTP_CAUSE_STALE_COOKIE		__OS7_SCTP_CAUSE_STALE_COOKIE
#define SCTP_CAUSE_USER_INITIATED	__OS7_SCTP_CAUSE_USER_INITIATED
#define SCTPCB_FLAG_ACK			__OS7_SCTPCB_FLAG_ACK
#define SCTPCB_FLAG_CKSUMMED		__OS7_SCTPCB_FLAG_CKSUMMED
#define SCTPCB_FLAG_CONF		__OS7_SCTPCB_FLAG_CONF
#define SCTPCB_FLAG_DELIV		__OS7_SCTPCB_FLAG_DELIV
#define SCTPCB_FLAG_DROPPED		__OS7_SCTPCB_FLAG_DROPPED
#define SCTPCB_FLAG_FIRST_FRAG		__OS7_SCTPCB_FLAG_FIRST_FRAG
#define SCTPCB_FLAG_LAST_FRAG		__OS7_SCTPCB_FLAG_LAST_FRAG
#define SCTPCB_FLAG_NACK		__OS7_SCTPCB_FLAG_NACK
#define SCTPCB_FLAG_RETRANS		__OS7_SCTPCB_FLAG_RETRANS
#define SCTPCB_FLAG_SACKED		__OS7_SCTPCB_FLAG_SACKED
#define SCTPCB_FLAG_SENT		__OS7_SCTPCB_FLAG_SENT
#define SCTPCB_FLAG_URG			__OS7_SCTPCB_FLAG_URG
#define SCTP_CF_BAD_ADDRESS		__OS7_SCTP_CF_BAD_ADDRESS
#define SCTP_CF_BAD_CHUNK_TYPE		__OS7_SCTP_CF_BAD_CHUNK_TYPE
#define SCTP_CF_BAD_PARM		__OS7_SCTP_CF_BAD_PARM
#define SCTP_CF_INVALID_PARM		__OS7_SCTP_CF_INVALID_PARM
#define SCTP_CF_INVALID_STR		__OS7_SCTP_CF_INVALID_STR
#define SCTP_CF_MASK_ANY		__OS7_SCTP_CF_MASK_ANY
#define SCTP_CF_MISSING_PARM		__OS7_SCTP_CF_MISSING_PARM
#define SCTP_CF_NO_DATA			__OS7_SCTP_CF_NO_DATA
#define SCTP_CF_NO_RESOURCE		__OS7_SCTP_CF_NO_RESOURCE
#define SCTP_CF_SHUTDOWN		__OS7_SCTP_CF_SHUTDOWN
#define SCTP_CF_STALE_COOKIE		__OS7_SCTP_CF_STALE_COOKIE
#define sctpchdr			__os7_sctpchdr
#define sctp_chunk			__os7_sctp_chunk
#define sctp_ck_shutd			__os7_sctp_ck_shutd
#define SCTP_CKSUM_TYPE			__OS7_SCTP_CKSUM_TYPE
#define SCTP_CLOSED			__OS7_SCTP_CLOSED
#define SCTP_CMSGF_RECVPPI		__OS7_SCTP_CMSGF_RECVPPI
#define SCTP_CMSGF_RECVSID		__OS7_SCTP_CMSGF_RECVSID
#define SCTP_CMSGF_RECVSSN		__OS7_SCTP_CMSGF_RECVSSN
#define SCTP_CMSGF_RECVTSN		__OS7_SCTP_CMSGF_RECVTSN
#define sctp_cookie_ack			__os7_sctp_cookie_ack
#define SCTP_COOKIE_ECHOED		__OS7_SCTP_COOKIE_ECHOED
#define sctp_cookie_echo		__os7_sctp_cookie_echo
#define SCTP_COOKIE_INC			__OS7_SCTP_COOKIE_INC
#define SCTP_COOKIE_LIFE		__OS7_SCTP_COOKIE_LIFE
#define sctp_cookie			__os7_sctp_cookie
#define sctp_cookie_psrv		__os7_sctp_cookie_psrv
#define SCTP_COOKIE_WAIT		__OS7_SCTP_COOKIE_WAIT
#define SCTP_CORK			__OS7_SCTP_CORK
#define SCTP_CSUM_ADLER32		__OS7_SCTP_CSUM_ADLER32
#define SCTP_CSUM_CRC32C		__OS7_SCTP_CSUM_CRC32C
#define SCTP_CTYPE_ABORT		__OS7_SCTP_CTYPE_ABORT
#define SCTP_CTYPE_ASCONF_ACK		__OS7_SCTP_CTYPE_ASCONF_ACK
#define SCTP_CTYPE_ASCONF		__OS7_SCTP_CTYPE_ASCONF
#define SCTP_CTYPE_COOKIE_ACK		__OS7_SCTP_CTYPE_COOKIE_ACK
#define SCTP_CTYPE_COOKIE_ECHO		__OS7_SCTP_CTYPE_COOKIE_ECHO
#define SCTP_CTYPE_CWR			__OS7_SCTP_CTYPE_CWR
#define SCTP_CTYPE_DATA			__OS7_SCTP_CTYPE_DATA
#define SCTP_CTYPE_ECNE			__OS7_SCTP_CTYPE_ECNE
#define SCTP_CTYPE_ERROR		__OS7_SCTP_CTYPE_ERROR
#define SCTP_CTYPE_FORWARD_TSN		__OS7_SCTP_CTYPE_FORWARD_TSN
#define SCTP_CTYPE_HEARTBEAT_ACK	__OS7_SCTP_CTYPE_HEARTBEAT_ACK
#define SCTP_CTYPE_HEARTBEAT		__OS7_SCTP_CTYPE_HEARTBEAT
#define SCTP_CTYPE_INIT_ACK		__OS7_SCTP_CTYPE_INIT_ACK
#define SCTP_CTYPE_INIT			__OS7_SCTP_CTYPE_INIT
#define SCTP_CTYPE_MASK_CONTINUE	__OS7_SCTP_CTYPE_MASK_CONTINUE
#define SCTP_CTYPE_MASK			__OS7_SCTP_CTYPE_MASK
#define SCTP_CTYPE_MASK_REP_CONT	__OS7_SCTP_CTYPE_MASK_REP_CONT
#define SCTP_CTYPE_MASK_REPORT		__OS7_SCTP_CTYPE_MASK_REPORT
#define SCTP_CTYPE_SACK			__OS7_SCTP_CTYPE_SACK
#define SCTP_CTYPE_SHUTDOWN_ACK		__OS7_SCTP_CTYPE_SHUTDOWN_ACK
#define SCTP_CTYPE_SHUTDOWN_COMPLETE	__OS7_SCTP_CTYPE_SHUTDOWN_COMPLETE
#define SCTP_CTYPE_SHUTDOWN		__OS7_SCTP_CTYPE_SHUTDOWN
#define sctp_cwr			__os7_sctp_cwr
#define sctp_daddr			__os7_sctp_daddr
#define sctp_data			__os7_sctp_data
#define SCTP_DEBUG_OPTIONS		__OS7_SCTP_DEBUG_OPTIONS
#define sctp_del_ip			__os7_sctp_del_ip
#define SCTP_DEL_IP			__OS7_SCTP_DEL_IP
#define sctp_dest_cachep		__os7_sctp_dest_cachep
#define SCTP_DESTF_CONGESTD		__OS7_SCTP_DESTF_CONGESTD
#define SCTP_DESTF_DROPPING		__OS7_SCTP_DESTF_DROPPING
#define SCTP_DESTF_FORWDTSN		__OS7_SCTP_DESTF_FORWDTSN
#define SCTP_DESTF_HBACTIVE		__OS7_SCTP_DESTF_HBACTIVE
#define SCTP_DESTF_INACTIVE		__OS7_SCTP_DESTF_INACTIVE
#define SCTP_DESTF_PMTUDISC		__OS7_SCTP_DESTF_PMTUDISC
#define SCTP_DESTF_ROUTFAIL		__OS7_SCTP_DESTF_ROUTFAIL
#define SCTP_DESTF_TIMEDOUT		__OS7_SCTP_DESTF_TIMEDOUT
#define SCTP_DESTF_UNUSABLE		__OS7_SCTP_DESTF_UNUSABLE
#define SCTP_DESTM_CANT_USE		__OS7_SCTP_DESTM_CANT_USE
#define SCTP_DESTM_DONT_USE		__OS7_SCTP_DESTM_DONT_USE
#define SCTP_DISPOSITION_ACKED		__OS7_SCTP_DISPOSITION_ACKED
#define SCTP_DISPOSITION_GAP_ACKED	__OS7_SCTP_DISPOSITION_GAP_ACKED
#define SCTP_DISPOSITION_NONE		__OS7_SCTP_DISPOSITION_NONE
#define SCTP_DISPOSITION		__OS7_SCTP_DISPOSITION
#define SCTP_DISPOSITION_SENT		__OS7_SCTP_DISPOSITION_SENT
#define SCTP_DISPOSITION_UNSENT		__OS7_SCTP_DISPOSITION_UNSENT
#define sctp_dstat			__os7_sctp_dstat
#define sctp_dup			__os7_sctp_dup
#define sctp_ecn_capable		__os7_sctp_ecn_capable
#define sctp_ecne			__os7_sctp_ecne
#define SCTP_ECN			__OS7_SCTP_ECN
#define sctpehdr			__os7_sctpehdr
#define sctp_error_cause		__os7_sctp_error_cause
#define sctp_error			__os7_sctp_error
#define SCTP_ESTABLISHED		__OS7_SCTP_ESTABLISHED
#define SCTPF_CLOSED			__OS7_SCTPF_CLOSED
#define SCTPF_CLOSING			__OS7_SCTPF_CLOSING
#define SCTPF_CONNECTED			__OS7_SCTPF_CONNECTED
#define SCTPF_COOKIE_ECHOED		__OS7_SCTPF_COOKIE_ECHOED
#define SCTPF_COOKIE_WAIT		__OS7_SCTPF_COOKIE_WAIT
#define SCTPF_DISCONNECTED		__OS7_SCTPF_DISCONNECTED
#define SCTPF_ESTABLISHED		__OS7_SCTPF_ESTABLISHED
#define SCTPF_HAVEUSER			__OS7_SCTPF_HAVEUSER
#define SCTP_FLAG_DEFAULT_RC_SEL	__OS7_SCTP_FLAG_DEFAULT_RC_SEL
#define SCTP_FLAG_EX_DATA_OPT		__OS7_SCTP_FLAG_EX_DATA_OPT
#define SCTP_FLAG_NEED_CLEANUP		__OS7_SCTP_FLAG_NEED_CLEANUP
#define SCTP_FLAG_REC_CONF_OPT		__OS7_SCTP_FLAG_REC_CONF_OPT
#define SCTPF_LISTEN			__OS7_SCTPF_LISTEN
#define SCTPF_NEEDABORT			__OS7_SCTPF_NEEDABORT
#define SCTPF_OPENING			__OS7_SCTPF_OPENING
#define sctp_forward_tsn		__os7_sctp_forward_tsn
#define SCTPF_RECEIVING			__OS7_SCTPF_RECEIVING
#define SCTPF_SENDING			__OS7_SCTPF_SENDING
#define SCTPF_SHUTDOWN_ACK_SENT		__OS7_SCTPF_SHUTDOWN_ACK_SENT
#define SCTPF_SHUTDOWN_PENDING		__OS7_SCTPF_SHUTDOWN_PENDING
#define SCTPF_SHUTDOWN_RECEIVED		__OS7_SCTPF_SHUTDOWN_RECEIVED
#define SCTPF_SHUTDOWN_RECVWAIT		__OS7_SCTPF_SHUTDOWN_RECVWAIT
#define SCTPF_SHUTDOWN_SENT		__OS7_SCTPF_SHUTDOWN_SENT
#define SCTPF_UNREACHABLE		__OS7_SCTPF_UNREACHABLE
#define sctp_hb_info			__os7_sctp_hb_info
#define sctp_hbitvl			__os7_sctp_hbitvl
#define SCTP_HB				__OS7_SCTP_HB
#define sctphdr				__os7_sctphdr
#define sctp_heartbeat_ack		__os7_sctp_heartbeat_ack
#define sctp_heartbeat_info		__os7_sctp_heartbeat_info
#define SCTP_HEARTBEAT_ITVL		__OS7_SCTP_HEARTBEAT_ITVL
#define sctp_heartbeat			__os7_sctp_heartbeat
#define SCTP_HMAC_MD5			__OS7_SCTP_HMAC_MD5
#define SCTP_HMAC_NONE			__OS7_SCTP_HMAC_NONE
#define SCTP_HMAC_SHA_1			__OS7_SCTP_HMAC_SHA_1
#define sctp_host_name			__os7_sctp_host_name
#define sctp_init_ack			__os7_sctp_init_ack
#define sctp_init			__os7_sctp_init
#define sctp_ipv4_addr			__os7_sctp_ipv4_addr
#define sctp_ipv6_addr			__os7_sctp_ipv6_addr
#define SCTP_ISTREAMS			__OS7_SCTP_ISTREAMS
#define sctp_last_add			__os7_sctp_last_add
#define SCTP_LIFETIME			__OS7_SCTP_LIFETIME
#define sctp_listen			__os7_sctp_listen
#define SCTP_LISTEN			__OS7_SCTP_LISTEN
#define SCTP_MAC_TYPE			__OS7_SCTP_MAC_TYPE
#define SCTP_MAX_BURST			__OS7_SCTP_MAX_BURST
#define SCTP_MAX_INIT_RETRIES		__OS7_SCTP_MAX_INIT_RETRIES
#define SCTP_MAXSEG			__OS7_SCTP_MAXSEG
#define SCTP_MAX_STATES			__OS7_SCTP_MAX_STATES
#define sctp_mib			__os7_sctp_mib
#define sctp_msg			__os7_sctp_msg
#define SCTP_NODELAY			__OS7_SCTP_NODELAY
#define sctp_no_mparm			__os7_sctp_no_mparm
#define sctp_no_rsrce			__os7_sctp_no_rsrce
#define sctp_no_udata			__os7_sctp_no_udata
#define sctp_op_short			__os7_sctp_op_short
#define SCTP_OPTION_BREAK		__OS7_SCTP_OPTION_BREAK
#define SCTP_OPTION_DBREAK		__OS7_SCTP_OPTION_DBREAK
#define SCTP_OPTION_DROPPING		__OS7_SCTP_OPTION_DROPPING
#define SCTP_OPTION_RANDOM		__OS7_SCTP_OPTION_RANDOM
#define sctp_opt			__os7_sctp_opt
#define sctp_orphan_count		__os7_sctp_orphan_count
#define SCTP_OSTREAMS			__OS7_SCTP_OSTREAMS
#define sctp_parm			__os7_sctp_parm
#define SCTP_PATH_MAX_RETRANS		__OS7_SCTP_PATH_MAX_RETRANS
#define sctpphdr			__os7_sctpphdr
#define sctp_poll			__os7_sctp_poll
#define SCTP_PPI			__OS7_SCTP_PPI
#define SCTP_PR_NONE			__OS7_SCTP_PR_NONE
#define SCTP_PR				__OS7_SCTP_PR
#define sctp_prot_hook			__os7_sctp_prot_hook
#define sctp_protolist			__os7_sctp_protolist
#define sctp_prot			__os7_sctp_prot
#define SCTP_PR_PREFERRED		__OS7_SCTP_PR_PREFERRED
#define SCTP_PR_REQUIRED		__OS7_SCTP_PR_REQUIRED
#define sctp_pr_sctp			__os7_sctp_pr_sctp
#define SCTP_PTYPE_ADD_IP		__OS7_SCTP_PTYPE_ADD_IP
#define SCTP_PTYPE_ADDR_TYPE		__OS7_SCTP_PTYPE_ADDR_TYPE
#define SCTP_PTYPE_ALI			__OS7_SCTP_PTYPE_ALI
#define SCTP_PTYPE_COOKIE_PSRV		__OS7_SCTP_PTYPE_COOKIE_PSRV
#define SCTP_PTYPE_DEL_IP		__OS7_SCTP_PTYPE_DEL_IP
#define SCTP_PTYPE_ECN_CAPABLE		__OS7_SCTP_PTYPE_ECN_CAPABLE
#define SCTP_PTYPE_ERROR_CAUSE		__OS7_SCTP_PTYPE_ERROR_CAUSE
#define SCTP_PTYPE_HEARTBEAT_INFO	__OS7_SCTP_PTYPE_HEARTBEAT_INFO
#define SCTP_PTYPE_HOST_NAME		__OS7_SCTP_PTYPE_HOST_NAME
#define SCTP_PTYPE_IPV4_ADDR		__OS7_SCTP_PTYPE_IPV4_ADDR
#define SCTP_PTYPE_IPV6_ADDR		__OS7_SCTP_PTYPE_IPV6_ADDR
#define SCTP_PTYPE_MASK_CONTINUE	__OS7_SCTP_PTYPE_MASK_CONTINUE
#define SCTP_PTYPE_MASK			__OS7_SCTP_PTYPE_MASK
#define SCTP_PTYPE_MASK_REP_CONT	__OS7_SCTP_PTYPE_MASK_REP_CONT
#define SCTP_PTYPE_MASK_REPORT		__OS7_SCTP_PTYPE_MASK_REPORT
#define SCTP_PTYPE_PR_SCTP		__OS7_SCTP_PTYPE_PR_SCTP
#define SCTP_PTYPE_SET_IP		__OS7_SCTP_PTYPE_SET_IP
#define SCTP_PTYPE_STATE_COOKIE		__OS7_SCTP_PTYPE_STATE_COOKIE
#define SCTP_PTYPE_SUCCESS_REPORT	__OS7_SCTP_PTYPE_SUCCESS_REPORT
#define SCTP_PTYPE_UNREC_PARMS		__OS7_SCTP_PTYPE_UNREC_PARMS
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
#define SCTP_SACKF_ANY			__OS7_SCTP_SACKF_ANY
#define SCTP_SACKF_ASC			__OS7_SCTP_SACKF_ASC
#define SCTP_SACKF_CWR			__OS7_SCTP_SACKF_CWR
#define SCTP_SACKF_DUP			__OS7_SCTP_SACKF_DUP
#define SCTP_SACKF_ECN			__OS7_SCTP_SACKF_ECN
#define SCTP_SACKF_FSN			__OS7_SCTP_SACKF_FSN
#define SCTP_SACKF_GAP			__OS7_SCTP_SACKF_GAP
#define SCTP_SACKF_NEW			__OS7_SCTP_SACKF_NEW
#define SCTP_SACKF_NOD			__OS7_SCTP_SACKF_NOD
#define SCTP_SACKF_NOW			__OS7_SCTP_SACKF_NOW
#define SCTP_SACKF_TIM			__OS7_SCTP_SACKF_TIM
#define SCTP_SACKF_URG			__OS7_SCTP_SACKF_URG
#define SCTP_SACKF_WND			__OS7_SCTP_SACKF_WND
#define SCTP_SACKF_WUP			__OS7_SCTP_SACKF_WUP
#define sctp_sack			__os7_sctp_sack
#define sctp_saddr			__os7_sctp_saddr
#define sctp_set_ip			__os7_sctp_set_ip
#define SCTP_SET_IP			__OS7_SCTP_SET_IP
#define SCTP_SET			__OS7_SCTP_SET
#define sctp_shutdown_ack		__os7_sctp_shutdown_ack
#define SCTP_SHUTDOWN_ACK_SENT		__OS7_SCTP_SHUTDOWN_ACK_SENT
#define sctp_shutdown_comp		__os7_sctp_shutdown_comp
#define sctp_shutdown			__os7_sctp_shutdown
#define SCTP_SHUTDOWN_PENDING		__OS7_SCTP_SHUTDOWN_PENDING
#define SCTP_SHUTDOWN_RECEIVED		__OS7_SCTP_SHUTDOWN_RECEIVED
#define SCTP_SHUTDOWN_RECVWAIT		__OS7_SCTP_SHUTDOWN_RECVWAIT
#define SCTP_SHUTDOWN_SENT		__OS7_SCTP_SHUTDOWN_SENT
#define SCTP_SID			__OS7_SCTP_SID
#define sctp_skb_cb			__os7_sctp_skb_cb
#define sctp_socket_count		__os7_sctp_socket_count
#define sctp_srce_add			__os7_sctp_srce_add
#define sctp_srce_cachep		__os7_sctp_srce_cachep
#define SCTP_SRCEF_ADD_PENDING		__OS7_SCTP_SRCEF_ADD_PENDING
#define SCTP_SRCEF_ADD_REQUEST		__OS7_SCTP_SRCEF_ADD_REQUEST
#define SCTP_SRCEF_DEL_PENDING		__OS7_SCTP_SRCEF_DEL_PENDING
#define SCTP_SRCEF_DEL_REQUEST		__OS7_SCTP_SRCEF_DEL_REQUEST
#define SCTP_SRCEF_SET_PENDING		__OS7_SCTP_SRCEF_SET_PENDING
#define SCTP_SRCEF_SET_REQUEST		__OS7_SCTP_SRCEF_SET_REQUEST
#define SCTP_SRCEM_ASCONF		__OS7_SCTP_SRCEM_ASCONF
#define SCTP_SSN			__OS7_SCTP_SSN
#define sctp_stale_ck			__os7_sctp_stale_ck
#define sctp_state_cookie		__os7_sctp_state_cookie
#define sctp_statistics			__os7_sctp_statistics
#define SCTP_STATUS			__OS7_SCTP_STATUS
#define sctp_strm_cachep		__os7_sctp_strm_cachep
#define SCTP_STRMF_DROP			__OS7_SCTP_STRMF_DROP
#define SCTP_STRMF_MORE			__OS7_SCTP_STRMF_MORE
#define sctp_strm			__os7_sctp_strm
#define sctp_success_report		__os7_sctp_success_report
#define SCTP_THROTTLE_ITVL		__OS7_SCTP_THROTTLE_ITVL
#define SCTP_TSN			__OS7_SCTP_TSN
#define sctp_unk_parm			__os7_sctp_unk_parm
#define SCTP_UNREACHABLE		__OS7_SCTP_UNREACHABLE
#define sctp_unrec_parms		__os7_sctp_unrec_parms
#define sctp_v4_err			__os7_sctp_v4_err
#define sctp_v4_rcv			__os7_sctp_v4_rcv

#endif				/* __OS7_OS7_NAMESPACE_H__ */
