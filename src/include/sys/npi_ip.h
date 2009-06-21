/*****************************************************************************

 @(#) $Id: npi_ip.h,v 1.1.2.1 2009-06-21 11:25:37 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-06-21 11:25:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_ip.h,v $
 Revision 1.1.2.1  2009-06-21 11:25:37  brian
 - added files to new distro

 *****************************************************************************/

#ifndef SYS_NPI_IP_H
#define SYS_NPI_IP_H

#ident "@(#) $RCSfile: npi_ip.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/* This file can be processed by doxygen(1). */

/// @file
/// @brief This header file provides the Network Provider Interface (NPI) for Internet Protocols (IP).

#include <sys/npi.h>

/*
 *  LiS npi.h is version 1
 */
#ifndef N_VERSION_2

#define N_CURRENT_VERSION   0x02	/* current version of NPI */
#define N_VERSION_2	    0x02	/* version of npi, December 16, 1991 */

/*
 * Information acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_INFO_ACK */
	np_ulong NSDU_size;		/* maximum NSDU size */
	np_ulong ENSDU_size;		/* maximum ENSDU size */
	np_ulong CDATA_size;		/* connect data size */
	np_ulong DDATA_size;		/* discon data size */
	np_ulong ADDR_size;		/* address size */
	np_ulong ADDR_length;		/* address length */
	np_ulong ADDR_offset;		/* address offset */
	np_ulong QOS_length;		/* QOS values length */
	np_ulong QOS_offset;		/* QOS values offset */
	np_ulong QOS_range_length;	/* length of QOS values' range */
	np_ulong QOS_range_offset;	/* offset of QOS values' range */
	np_ulong OPTIONS_flags;		/* bit masking for options supported */
	np_ulong NIDU_size;		/* network i/f data unit size */
	np_long SERV_type;		/* service type */
	np_ulong CURRENT_state;		/* current state */
	np_ulong PROVIDER_type;		/* type of NS provider */
	np_ulong NODU_size;		/* optimal NSDU size */
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
	np_ulong NPI_version;		/* version # of npi that is supported */
} __N_info_ack_t;

#define N_info_ack_t __N_info_ack_t

/*
 * Bind acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_BIND_ACK */
	np_ulong ADDR_length;		/* address length */
	np_ulong ADDR_offset;		/* offset of address */
	np_ulong CONIND_number;		/* connection indications */
	np_ulong TOKEN_value;		/* value of "token" assigned to stream */
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
} __N_bind_ack_t;

#define N_bind_ack_t __N_bind_ack_t

#endif				/* N_VERSION_2 */

#ifndef NSF_UNBND
#define NSF_UNBND	(1<<NS_UNBND	  )
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ  )
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ  )
#define NSF_IDLE	(1<<NS_IDLE	  )
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES  )
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ  )
#define NSF_WRES_CIND	(1<<NS_WRES_CIND  )
#define NSF_WACK_CRES	(1<<NS_WACK_CRES  )
#define NSF_DATA_XFER	(1<<NS_DATA_XFER  )
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ  )
#define NSF_WRES_RIND	(1<<NS_WRES_RIND  )
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6 )
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7 )
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9 )
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif				/* NSF_UNBND */

#define IP_FLAG_DEFAULT_RC_SEL	(1<<0)
#define IP_BINDPORT_LOCK	(1<<1)

#define N_QOS_SEL_INFO_IP	0x0201
#define N_QOS_RANGE_INFO_IP	0x0202
#define N_QOS_SEL_CONN_IP	0x0203
#define N_QOS_SEL_UD_IP		0x0204

typedef struct N_qos_sel_info_ip {
	np_ulong n_qos_type;		/* always N_QOS_SEL_INFO_IP */
	np_ulong protocol;		/* default protocol for data transmission */
	np_ulong priority;		/* default priority for data transmission */
	np_ulong ttl;			/* default time-to-live for outgoing packets */
	np_ulong tos;			/* default type-of-service for outgoing packets */
	np_ulong mtu;			/* default mtu for outgoing packets */
	np_ulong saddr;			/* default srce IP address for outgoing packets */
	np_ulong daddr;			/* default dest IP address for outgoing packets */
} N_qos_sel_info_ip_t;

typedef struct N_qos_sel_conn_ip {
	np_ulong n_qos_type;		/* always N_QOS_SEL_CONN_IP */
	np_ulong protocol;
	np_ulong priority;
	np_ulong ttl;			/* default time-to-live for connection */
	np_ulong tos;			/* default type-of-service for connection */
	np_ulong mtu;			/* default maximum-transfer-unit for connection */
	np_ulong saddr;			/* default srce IP address for outgoing packets */
	np_ulong daddr;			/* default dest IP address for outgoing packets */
} N_qos_sel_conn_ip_t;

typedef struct N_qos_sel_ud_ip {
	np_ulong n_qos_type;		/* always N_QOS_SEL_UD_IP */
	np_ulong protocol;		/* protocol for outgoing packet */
	np_ulong priority;		/* priority for outgoing packet */
	np_ulong ttl;			/* time to live for outgoing packet */
	np_ulong tos;			/* type of service for outgoing packet */
	np_ulong saddr;			/* source IP address for outgoing packet */
} N_qos_sel_ud_ip_t;

typedef struct {
	np_long priority_min_value;	/* */
	np_long priority_max_value;	/* */
} ip_priority_values_t;

typedef struct {
	np_long ttl_min_value;		/* */
	np_long ttl_max_value;		/* */
} ip_ttl_values_t;

typedef struct {
	np_long tos_min_value;		/* */
	np_long tos_max_value;		/* */
} ip_tos_values_t;

typedef struct {
	np_long mtu_min_value;		/* */
	np_long mtu_max_value;		/* */
} ip_mtu_values_t;

typedef struct N_qos_range_info_ip {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_INFO_IP */
	ip_priority_values_t priority;	/* priority range */
	ip_ttl_values_t ttl;		/* ttl range */
	ip_tos_values_t tos;		/* tos range */
	ip_mtu_values_t mtu;		/* mtu range */
} N_qos_range_info_ip_t;

union N_qos_ip_types {
	np_ulong n_qos_type;
	struct N_qos_sel_info_ip n_qos_sel_info;
	struct N_qos_sel_conn_ip n_qos_sel_conn;
	struct N_qos_sel_ud_ip n_qos_sel_ud;
	struct N_qos_range_info_ip n_qos_range_info;
};

/* TOS values, logical OR of one from each group below */

/* TOS precedence */
#define N_ROUTINE		0x00
#define N_PRIORITY		0x20
#define N_IMMEDIATE		0x40
#define N_FLASH			0x60
#define N_OVERRIDEFLASH		0x80
#define N_CRITIC_ECP		0xa0
#define N_INETCONTROL		0xc0
#define N_NETCONTROL		0xe0

/* TOS type of service */
#define N_NOTOS			0
#define N_LDELAY		(1<<4)
#define N_HITHRPT		(1<<3)
#define N_HIREL			(1<<2)
#define N_LOCOST		(1<<1)

#endif				/* SYS_NPI_IP_H */
