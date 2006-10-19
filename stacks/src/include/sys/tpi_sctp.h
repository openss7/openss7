/*****************************************************************************

 @(#) $Id: tpi_sctp.h,v 0.9.2.4 2006/10/19 17:18:51 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/10/19 17:18:51 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_TPI_SCTP_H
#define _SYS_TPI_SCTP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#ifndef T_CURRENT_VERSION

#define T_CURRENT_VERSION 5

/*
 *  Missing from LiS tihdr.h
 */
#ifndef T_MORE
#define T_MORE		0x0001
#endif
#ifndef T_EXPEDITED
#define T_EXPEDITED	0x0002
#endif
#ifndef T_ODF_MORE
#define T_ODF_MORE	T_MORE
#endif
#ifndef T_ODF_EX
#define T_ODF_EX	T_EXPEDITED
#endif
#ifndef XPG4_1
#define XPG4_1		2	/* This indicates that the transport provider conforms to XTI in
				   XPG4 and supports the new primitives T_ADDR_REQ and T_ADDR_ACK */
#endif

/*
 *  Errors in LiS tihdr.h
 */
#define T_optdata_ind T_optdata_ack
#define optdata_ind optdata_ack

#endif				/* T_CURRENT_VERSION */

#endif				/* _SYS_TPI_SCTP_H */
