/*****************************************************************************

 @(#) $Id: m3ua.h,v 0.9 2004/01/17 08:08:12 brian Exp $

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

 Last Modified $Date: 2004/01/17 08:08:12 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_M3UA_H__
#define __SS7_M3UA_H__

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

typedef unsigned long m3ua_ulong;
typedef unsigned short m3ua_ushort;
typedef unsigned char m3ua_uchar;

typedef struct m3ua_phdr {
	u16 tag;
	u16 len;
} m3ua_phdr_t;

#define M_TAG_NETWORK_APPEARANCE	1
#define	M_TAG_PROTOCOL_DATA		3
#define M_TAG_INFO_STRING		4
#define M_TAG_AFFECTED_DPC		5
#define M_TAG_ROUTING_CONTEXT		6
#define M_TAG_DIAGNOSTIC_INFORMATION	7
#define M_TAG_HEARTBEAT_DATA		8
#define M_TAG_UNAVAILABILITY_CAUSE	9
#define M_TAG_REASON			10
#define	M_TAG_TRAFFIC_MODE_TYPE		11
#define M_TAG_ERROR_CODE		12
#define	M_TAG_STATUS_TYPE		13
#define M_TAG_CONGESTED_INDICATIONS	14

typedef struct m3ua_msg {
	u8 vers;
	u8 res;
	u16 type;
	u32 len;
	m3ua_phdr_t ph[0];
} m3ua_msg_t;

#define M_VERSION_REL1   1

#define M_CLASS_MGMT	0x0000
#define M_CLASS_XFER	0x0100
#define	M_CLASS_SSNM	0x0200
#define M_CLASS_ASPSM	0x0300
#define M_CLASS_ASPTM	0x0400

#define M_TYPE_ERR		(0|M_CLASS_MGMT

#define M_TYPE_NTFY		(1|M_CLASS_XFER)
#define M_TYPE_DATA		(1|M_CLASS_XFER)

#define M_TYPE_DUNA		(1|M_CLASS_SSNM)
#define M_TYPE_DAVA		(2|M_CLASS_SSNM)
#define M_TYPE_DUAD		(3|M_CLASS_SSNM)
#define M_TYPE_SCON		(4|M_CLASS_SSNM)
#define M_TYPE_DUPU		(5|M_CLASS_SSNM)

#define	M_TYPE_UP		(1|M_CLASS_ASPSM)
#define	M_TYPE_DOWN		(2|M_CLASS_ASPSM)
#define	M_TYPE_BEAT		(3|M_CLASS_ASPSM)
#define	M_TYPE_UP_ACK		(4|M_CLASS_ASPSM)
#define	M_TYPE_DOWN_ACK		(5|M_CLASS_ASPSM)
#define	M_TYPE_BEAT_ACK		(6|M_CLASS_ASPSM)

#define M_TYPE_ACTIVE		(1|M_CLASS_ASPTM)
#define M_TYPE_INACTIVE		(2|M_CLASS_ASPTM)
#define M_TYPE_ACTIVE_ACK	(3|M_CLASS_ASPTM)
#define M_TYPE_INACTIVE_ACK	(4|M_CLASS_ASPTM)

#define M_CLASS_MASK	0xff00
#define	M_TYPE_MASK	0x00ff

/*
 *  LAYER MANAGEMENT PRIMITIVES
 */

#define M_T_STATUS_REQ
#define M_T_ESTABLISH_REQ
#define M_T_RELEASE_REQ
#define M_ASP_STATUS_REQ
#define M_ASP_UP_REQ
#define M_ASP_DOWN_REQ
#define M_ASP_ACTIVE_REQ
#define M_ASP_INACTIVE_REQ
#define M_AS_STATUS_REQ

#define M_T_STATUS_ACK
#define M_T_ESTABLISH_IND
#define M_T_RELEASE_IND
#define M_T_ESTABLISH_CON
#define M_T_RELEASE_CON
#define M_NOTIFY_IND
#define M_ERROR_IND
#define M_ASP_STATUS_CON
#define M_ASP_UP_CON
#define M_ASP_DOWN_CON
#define M_ASP_ACTIVE_CON
#define M_ASP_INACTIVE_CON
#define M_ASP_UP_IND
#define M_ASP_DOWN_IND
#define M_ASP_ACTIVE_IND
#define M_ASP_INACTIVE_IND
#define M_AS_ACTIVE_IND
#define M_AS_INACTIVE_IND
#define M_AS_DOWN_IND
#define M_AS_STATUS_ACK

#endif				/* __SS7_M3UA_H__ */
