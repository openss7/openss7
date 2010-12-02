/*****************************************************************************

 @(#) $Id: npi_udp.h,v 1.1.2.2 2010-11-28 14:21:49 brian Exp $

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

 Last Modified $Date: 2010-11-28 14:21:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_udp.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:49  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:37  brian
 - added files to new distro

 *****************************************************************************/

#ifndef SYS_NPI_UDP_H
#define SYS_NPI_UDP_H

/* This file can be processed by doxygen(1). */

/// @file
/// @brief This header file provides the Network Provider Interface (NPI) for User Datagram Protocol (UDP).

#include <sys/npi.h>
#include <sys/npi_ip.h>

#define UDP_FLAG_DEFAULT_RC_SEL	(1<<0)
#define UDP_BINDPORT_LOCK	(1<<1)

#define N_QOS_SEL_INFO_UDP	0x0301
#define N_QOS_RANGE_INFO_UDP	0x0302
#define N_QOS_SEL_CONN_UDP	0x0303
#define N_QOS_SEL_UD_UDP	0x0304

typedef struct N_qos_sel_info_udp {
	np_ulong n_qos_type;		/* always N_QOS_SEL_INFO_UDP */
	np_ulong protocol;		/* default protocol for data transmission */
	np_ulong priority;		/* default priority for data transmission */
	np_ulong ttl;			/* default time-to-live for outgoing packets */
	np_ulong tos;			/* default type-of-service for outgoing packets */
	np_ulong mtu;			/* default mtu for outgoing packets */
	np_ulong saddr;			/* default srce IP address for outgoing packets */
	np_ulong daddr;			/* default dest IP address for outgoing packets */
        np_ulong checksum;              /* default checksum for packets */
} N_qos_sel_info_udp_t;

typedef struct N_qos_sel_conn_udp {
	np_ulong n_qos_type;		/* always N_QOS_SEL_CONN_UDP */
	np_ulong protocol;
	np_ulong priority;
	np_ulong ttl;			/* default time-to-live for connection */
	np_ulong tos;			/* default type-of-service for connection */
	np_ulong mtu;			/* default maximum-transfer-unit for connection */
	np_ulong saddr;			/* default srce IP address for outgoing packets */
	np_ulong daddr;			/* default dest IP address for outgoing packets */
        np_ulong checksum;              /* default checksum for packets */
} N_qos_sel_conn_udp_t;

typedef struct N_qos_sel_ud_udp {
	np_ulong n_qos_type;		/* always N_QOS_SEL_UD_UDP */
	np_ulong protocol;		/* protocol for outgoing packet */
	np_ulong priority;		/* priority for outgoing packet */
	np_ulong ttl;			/* time to live for outgoing packet */
	np_ulong tos;			/* type of service for outgoing packet */
	np_ulong saddr;			/* source IP address for outgoing packet */
        np_ulong checksum;              /* default checksum for packets */
} N_qos_sel_ud_udp_t;

typedef struct {
	np_long priority_min_value;	/* */
	np_long priority_max_value;	/* */
} udp_priority_values_t;

typedef struct {
	np_long ttl_min_value;		/* */
	np_long ttl_max_value;		/* */
} udp_ttl_values_t;

typedef struct {
	np_long tos_min_value;		/* */
	np_long tos_max_value;		/* */
} udp_tos_values_t;

typedef struct {
	np_long mtu_min_value;		/* */
	np_long mtu_max_value;		/* */
} udp_mtu_values_t;

typedef struct N_qos_range_info_udp {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_INFO_UDP */
	udp_priority_values_t priority;	/* priority range */
	udp_ttl_values_t ttl;		/* ttl range */
	udp_tos_values_t tos;		/* tos range */
	udp_mtu_values_t mtu;		/* mtu range */
} N_qos_range_info_udp_t;

union N_qos_udp_types {
	np_ulong n_qos_type;
	struct N_qos_sel_info_udp n_qos_sel_info;
	struct N_qos_sel_conn_udp n_qos_sel_conn;
	struct N_qos_sel_ud_udp n_qos_sel_ud;
	struct N_qos_range_info_udp n_qos_range_info;
};

#endif				/* SYS_NPI_UDP_H */
