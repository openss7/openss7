/*****************************************************************************

 @(#) $Id: dlpi_sgi.h,v 0.9.2.1 2008-06-18 16:43:16 brian Exp $

 -----------------------------------------------------------------------------

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

 Last Modified $Date: 2008-06-18 16:43:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi_sgi.h,v $
 Revision 0.9.2.1  2008-06-18 16:43:16  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_DLPI_SGI_H__
#define __SYS_DLPI_SGI_H__

#ident "@(#) $RCSfile: dlpi_sgi.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 * This file includes some specifics for IRIX.  This is amazingly like the
 * Solaris equivalents.  IRIX does not appear to extend the primitive
 * interface.
 */

/*
 * The normal mode of operation is when a bind is performed with the value of
 * the SAP information being in range 0x02 to 0xFE (a one-byte, even value).
 * This is the SAP as specified under 802.2 LLC, and is the only mode of
 * operation for the connection (i.e. LLC2) service mode.  The Sub-Network
 * Access Protocol (SNAP) also uses this mode of operation.
 *
 * The DLSAP address for Normal mode have the following format:
 */
struct llc_dlsap {
	u_char dl_mac[6];		/* hardware address */
	u_char dl_sap;			/* LLC SAP */
};

/*
 * The DSLAP address may be modified through DL_SUBS_BIND_REQ primtive when
 * the SNAP is used to extend the LLC header.  The extended SNAP DLSAP
 * addresses have the following format:
 */
struct llc_snap_dlsap {
	u_char dl_mac[6];		/* hardware address */
	u_char dl_sap;			/* SNAP sap: 0xAA */
	u_char dl_oui[3];		/* OUI information */
	u_char dl_proto[2];		/* protocol ID */
};

/*
 * DSL users should use the llc_dlsap format in constructing the
 * DL_UNITDATA_REQ primitive and it is DLS users' responsibility to put the
 * OUI information and protocol ID in front of their data.  Upon receipt of
 * DL_UNITDATA_IND, the DLSAP addresses are also of llc_dlsap format.  It is
 * DLS users' responsibility to skip the OUI information and protocol ID for
 * users' data.
 *
 * The DLSAP address may also be modified if source routing is used for Token
 * Ring networks through TEST and/or XID primitives.  The source routing
 * information field (rif) is appended to the end of the llc_dlsap format.
 * The DL_CONNECT_* primitives should also use this llc_sri_dlsap format when
 * source routing information is present.  The extended SRI DLSAP addresses
 * have the following format:
 */
struct llc_sri_dlsap {
	u_char dl_mac[6];		/* hardware address */
	u_char dl_sap;			/* LLC SAP */
	u_char dl_rif;			/* start of rif */
};

/*
 * The Ethernet mode of operation occurs when a bind is performed for two
 * bytes (the high byte being non-zero).  When this occurs the binding driver
 * will be sent packets for the Ethernet types registered for.
 *
 * The DLSAP addresses for Ethernet mode have the following format:
 */
struct llc_eth_dlsap {
	u_char dl_mac[6];		/* hardware address */
	u_short dl_sap;			/* Ethernet SAP */
};

#ifdef _IRIX_SOURCE
#include <sys/dlpi.h>
#else				/* _IRIX_SOURCE */
#define _IRIX_SOURCE
#include <sys/dlpi.h>
#undef _IRIX_SOURCE
#endif				/* _IRIX_SOURCE */

#endif				/* __SYS_DLPI_SGI_H__ */
