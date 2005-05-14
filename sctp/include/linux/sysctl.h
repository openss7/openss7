/*****************************************************************************

 @(#) $Id: sysctl.h,v 0.9.2.7 2005/05/14 08:29:33 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:29:33 $ by $Author: brian $

 *****************************************************************************/

#ifndef __OS7_LINUX_LINUX_SYSCTL_H__
#define __OS7_LINUX_LINUX_SYSCTL_H__

#ident "@(#) $RCSfile: sysctl.h,v $ $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 2001-2004 OpenSS7 Corporation."

enum {
	__OS7_NET_SCTP_RTO_INITIAL = 200,
	__OS7_NET_SCTP_RTO_MIN = 201,
	__OS7_NET_SCTP_RTO_MAX = 202,
	__OS7_NET_SCTP_HEARTBEAT_ITVL = 203,
	__OS7_NET_SCTP_INIT_RETRIES = 204,
	__OS7_NET_SCTP_VALID_COOKIE_LIFE = 205,
	__OS7_NET_SCTP_MAX_SACK_DELAY = 206,
	__OS7_NET_SCTP_PATH_MAX_RETRANS = 207,
	__OS7_NET_SCTP_ASSOC_MAX_RETRANS = 208,
	__OS7_NET_SCTP_MAC_TYPE = 209,
	__OS7_NET_SCTP_CSUM_TYPE = 210,
	__OS7_NET_SCTP_COOKIE_INC = 211,
	__OS7_NET_SCTP_THROTTLE_ITVL = 212,
	__OS7_NET_SCTP_MEM = 213,
	__OS7_NET_SCTP_WMEM = 214,
	__OS7_NET_SCTP_RMEM = 215,
	__OS7_NET_SCTP_MAX_ISTREAMS = 216,
	__OS7_NET_SCTP_REQ_OSTREAMS = 217,
	__OS7_NET_SCTP_ECN = 218,
	__OS7_NET_SCTP_ADAPTATION_LAYER_INFO = 219,
	__OS7_NET_SCTP_PARTIAL_RELIABILITY = 220,
	__OS7_NET_SCTP_MAX_BURST = 221,
};

#endif				/* __OS7_LINUX_LINUX_SYSCTL_H__ */
