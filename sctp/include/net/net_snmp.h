/*****************************************************************************

 @(#) $Id: net_snmp.h,v 0.9.2.4 2004/12/22 11:27:48 brian Exp $

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

 Last Modified $Date: 2004/12/22 11:27:48 $ by $Author: brian $

 *****************************************************************************/

#ifndef __OS7_NET_NET_SNMP_H__
#define __OS7_NET_NET_SNMP_H__

#ident "@(#) $RCSfile: net_snmp.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2004 OpenSS7 Corporation."

#ifndef HAVE_STRUCT_SCTP_MIB
#define HAVE_STRUCT_SCTP_MIB 1
/*
 *  In accordance with draft-ietf-sigtran-sctp-mib-07
 */
struct sctp_mib {
	unsigned long SctpRtoAlgorithm;
#define HAVE_STRUCT_SCTP_MIB_SCTPRTOALGORITHM 1
	unsigned long SctpRtoMin;
#define HAVE_STRUCT_SCTP_MIB_SCTPRTOMIN 1
	unsigned long SctpRtoMax;
#define HAVE_STRUCT_SCTP_MIB_SCTPRTOMAX 1
	unsigned long SctpRtoInitial;
#define HAVE_STRUCT_SCTP_MIB_SCTPRTOINITIAL 1
	unsigned long SctpMaxAssoc;
#define HAVE_STRUCT_SCTP_MIB_SCTPMAXASSOC 1
	unsigned long SctpValCookieLife;
#define HAVE_STRUCT_SCTP_MIB_SCTPVALCOOKIELIFE 1
	unsigned long SctpMaxInitRetr;
#define HAVE_STRUCT_SCTP_MIB_SCTPMAXINITRETR 1
	unsigned long SctpCurrEstab;
#define HAVE_STRUCT_SCTP_MIB_SCTPCURRESTAB 1
	unsigned long SctpActiveEstabs;
#define HAVE_STRUCT_SCTP_MIB_SCTPACTIVEESTABS 1
	unsigned long SctpPassiveEstabs;
#define HAVE_STRUCT_SCTP_MIB_SCTPPASSIVEESTABS 1
	unsigned long SctpAborteds;
#define HAVE_STRUCT_SCTP_MIB_SCTPABORTEDS 1
	unsigned long SctpShutdowns;
#define HAVE_STRUCT_SCTP_MIB_SCTPSHUTDOWNS 1
	unsigned long SctpOutOfBlues;
#define HAVE_STRUCT_SCTP_MIB_SCTPOUTOFBLUES 1
	unsigned long SctpChecksumErrors;
#define HAVE_STRUCT_SCTP_MIB_SCTPCHECKSUMERRORS 1
	unsigned long SctpOutCtrlChunks;
#define HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS 1
	unsigned long SctpOutOrderChunks;
#define HAVE_STRUCT_SCTP_MIB_SCTPOUTORDERCHUNKS 1
	unsigned long SctpOutUnorderChunks;
#define HAVE_STRUCT_SCTP_MIB_SCTPOUTUNORDERCHUNKS 1
	unsigned long SctpInCtrlChunks;
#define HAVE_STRUCT_SCTP_MIB_SCTPINCTRLCHUNKS 1
	unsigned long SctpInOrderChunks;
#define HAVE_STRUCT_SCTP_MIB_SCTPINORDERCHUNKS 1
	unsigned long SctpInUnorderChunks;
#define HAVE_STRUCT_SCTP_MIB_SCTPINUNORDERCHUNKS 1
	unsigned long SctpFragUsrMsgs;
#define HAVE_STRUCT_SCTP_MIB_SCTPFRAGUSRMSGS 1
	unsigned long SctpReasmUsrMsgs;
#define HAVE_STRUCT_SCTP_MIB_SCTPREASMUSRMSGS 1
	unsigned long SctpOutSCTPPacks;
#define HAVE_STRUCT_SCTP_MIB_SCTPOUTSCTPPACKS 1
	unsigned long SctpInSCTPPacks;
#define HAVE_STRUCT_SCTP_MIB_SCTPINSCTPPACKS 1
	unsigned long SctpDiscontinuityTime;
#define HAVE_STRUCT_SCTP_MIB_SCTPDISCONTINUITYTIME 1
	unsigned long __pad[0];
} ____cacheline_aligned;

#endif				/* HAVE_STRUCT_SCTP_MIB */

#endif				/* __OS7_NET_NET_SNMP_H__ */
