/*****************************************************************************

 @(#) $Id: xti_ip.h,v 0.9.2.9 2008-04-28 18:38:37 brian Exp $

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

 Last Modified $Date: 2008-04-28 18:38:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_ip.h,v $
 Revision 0.9.2.9  2008-04-28 18:38:37  brian
 - header updates for release

 Revision 0.9.2.8  2007/08/14 04:00:46  brian
 - GPLv3 header update

 Revision 0.9.2.7  2006/09/25 12:04:43  brian
 - updated headers, moved xnsl

 *****************************************************************************/

#ifndef _SYS_XTI_IP_H
#define _SYS_XTI_IP_H

#ident "@(#) $RCSfile: xti_ip.h,v $ $Name:  $($Revision: 0.9.2.9 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI INET header file.
  * 
  * In accordance with OpenGroup Single UNIX Specifications, the symbols in this
  * file are exposed by including the <xti.h> header file.  */

/*
 * XTI INET Header File.
 */

/*
 * IP level
 */
#define T_INET_IP		0	/**< IP level (same as protocol number). */

/**
  * @name IP Level Options
  *
  * @{ */
#define T_IP_OPTIONS		1	/**< IP per-packet options. */
#define T_IP_TOS		2	/**< IP per-packet type of service. */
#define T_IP_TTL		3	/**< IP per-packet time to live. */
#define T_IP_REUSEADDR		4	/**< allow local address reuse. */
#define T_IP_DONTROUTE		5	/**< just use interface addresses. */
#define T_IP_BROADCAST		6	/**< permit sending of broadcast msgs. */
#define T_IP_ADDR		7	/**< dest/srce address of recv/sent packet. */
/** @} */

/**
  * @name IP_TOS Precedence Levels
  *
  * @{ */
#define T_ROUTINE		0	/**< Routine. */
#define T_PRIORITY		1	/**< Priority. */
#define T_IMMEDIATE		2	/**< Immediate. */
#define T_FLASH			3	/**< Flash. */
#define T_OVERRIDEFLASH		4	/**< Override Flash. */
#define T_CRITIC_ECP		5	/**< Critical. */
#define T_INETCONTROL		6	/**< Internet Control. */
#define T_NETCONTROL		7	/**< Network Control. */
/** @} */

/**
  * @name IP_TOS Type of Service
  *
  * @{ */
#define T_NOTOS			0	/**< No Type of Service. */
#define T_LDELAY		(1<<4)	/**< Low delay. */
#define T_HITHRPT		(1<<3)	/**< High throughput. */
#define T_HIREL			(1<<2)	/**< High reliability. */
#define T_LOCOST		(1<<1)	/**< Low cost. */
/** @} */

/** Set TOS value. */
#define SET_TOS(prec, tos) \
	(((0x7 & (prec)) << 5) | ((T_NOTOS|T_LDELAY|T_HITHRPT|T_HIREL|T_LOCOST) & (tos)))

#endif				/* _SYS_XTI_IP_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
