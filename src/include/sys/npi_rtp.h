/*****************************************************************************

 @(#) $Id: npi_rtp.h,v 1.1.2.1 2011-01-12 00:12:09 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-01-12 00:12:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_rtp.h,v $
 Revision 1.1.2.1  2011-01-12 00:12:09  brian
 - added RTP drivers

 *****************************************************************************/

#ifndef SYS_NPI_RTP_H
#define SYS_NPI_RTP_H

/* This file can be processed by doxygen(1). */

/// @file
/// @brief This header file provides the Network Provider Interface (NPI) for Real-Time Transport Protocol (RTP).

#include <sys/npi.h>
#include <sys/npi_ip.h>
#include <sys/npi_udp.h>

/*
 *  Primitives for cross-connect.
 */
#define N_XCON_REQ		29	/* Cross-connect connection request */
#define N_XDIS_REQ		30	/* Cross-connect disconnection request */

/*
 *  Cross-connect connection request. (M_PROTO)
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_XCON_REQ */
	np_ulong TOKEN_value;		/* cross-connect pair */
	np_ulong CONN_flags;		/* connection flags */
	np_ulong QOS_length;		/* QOS values length */
	np_ulong QOS_offset;		/* QOS values offset */
} N_xcon_req_t;

/*
 *  Cross-connect disconnection request. (M_PROTO)
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_XDIS_REQ */
	np_ulong DISCON_reason;		/* disconnection reason */
} N_xdis_req_t;

#define RTP_FLAG_DEFAULT_RC_SEL (1<<0)
#define RTP_BINDPORT_LOCK       (1<<1)

#define N_QOS_SEL_INFO_RTP      0x401
#define N_QOS_RANGE_INFO_RTP    0x402
#define N_QOS_SEL_CONN_RTP      0x403
#define N_QOS_SEL_UD_RTP        0x404

typedef struct N_qos_sel_info_rtp {
	np_ulong n_qos_type;		/* always N_QOS_SEL_INFO_RTP */
	np_ulong protocol;		/* default protocol for data transmission */
	np_ulong priority;		/* default priority for data transmsision */
	np_ulong ttl;			/* default time-to-live for outgoing packets */
	np_ulong tos;			/* default type-of-service for outgoing packets */
	np_ulong mtu;			/* default mtu for outgoing packets */
	np_ulong checksum;		/* default checksum for packets */
	np_ulong daddr;			/* connected destination address (network byte order) */
	np_short dport;			/* connected destination port (network byte order) */
} N_qos_sel_info_rtp_t;

typedef struct N_qos_sel_conn_rtp {
	np_ulong n_qos_type;		/* always N_QOS_SEL_CONN_RTP */
	np_ulong protocol;		/* always IPPROTO_UDP */
	np_ulong priority;		/* priority for connection */
	np_ulong ttl;			/* time-to-live for connection */
	np_ulong tos;			/* type-of-service for connection */
	np_ulong mtu;			/* mtu for outgoing packets */
	np_ulong checksum;		/* checksum packets */
	np_ulong saddr;			/* autobound source address (network byte order) */
	np_ulong sport;			/* autobound source port (network byte order) */
	np_ulong payload_type;		/* payload type for connection */
} N_qos_sel_conn_rtp_t;

typedef struct N_qos_sel_ud_rtp {
	np_ulong n_qos_type;		/* always N_QOS_SEL_UD_RTP */
	np_ulong protocol;		/* always IPPROTO_UDP */
	np_ulong priority;		/* priority for packet */
	np_ulong ttl;			/* time-to-live for packet */
	np_ulong tos;			/* type-of-service for packet */
	np_ulong checksum;		/* checksum packet */
	np_ulong saddr;			/* source IP address for outgoing packet. */
	np_ulong sport;			/* source UDP port for outgoing packet. */
	np_ulong payload_type;		/* payload type for RTP packet */
} N_qos_sel_ud_rtp_t;

typedef struct {
	np_long priority_min_value;	/* */
	np_long priority_max_value;	/* */
} rtp_priority_values_t;

typedef struct {
	np_long ttl_min_value;		/* */
	np_long ttl_max_value;		/* */
} rtp_ttl_values_t;

typedef struct {
	np_long tos_min_value;		/* */
	np_long tos_max_value;		/* */
} rtp_tos_values_t;

typedef struct {
	np_long mtu_min_value;		/* */
	np_long mtu_max_value;		/* */
} rtp_mtu_values_t;

typedef struct {
	np_ushort port_min_value;
	np_ushort port_max_value;
} rtp_port_values_t;

typedef struct {
	np_ushort ptype_min_value;
	np_ushort ptype_max_value;
} rtp_ptype_values_t;

typedef struct N_qos_range_info_rtp {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_INFO_RTP */
	rtp_priority_values_t priority;	/* priority range */
	rtp_ttl_values_t ttl;		/* ttl range */
	rtp_tos_values_t tos;		/* tos range */
	rtp_mtu_values_t mtu;		/* mtu range */
	rtp_port_values_t port;		/* port range */
	rtp_ptype_values_t ptype_static;	/* static payload type range */
	rtp_ptype_values_t ptype_dynamic;	/* dynamic payload type range */
} N_qos_range_info_rtp_t;

#endif				/* SYS_NPI_RTP_H */
