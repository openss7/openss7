/*****************************************************************************

 @(#) $RCSfile: xti_ip.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/09/18 13:52:37 $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (C) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/09/18 13:52:37 $ by $Author: brian $

 $Log: xti_ip.h,v $
 Revision 0.9.2.4  2006/09/18 13:52:37  brian
 - added doxygen markers to sources

 Revision 0.9.2.3  2005/05/14 08:30:49  brian
 - copyright header correction

 Revision 0.9.2.2  2004/09/02 09:31:14  brian
 - Synchronization with other packages.

 Revision 0.9.2.1  2004/05/12 08:01:39  brian
 - Added in xti library and STREAMS modules.

 Revision 1.1.2.1  2004/04/13 12:12:52  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _SYS_XTI_IP_H
#define _SYS_XTI_IP_H

#ident "@(#) $RCSfile: xti_ip.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/*
 * IP level
 */
#define T_INET_IP		0	/* IP level (same as protocol number) */

/*
 * IP level Options
 */
#define T_IP_OPTIONS		1	/* IP per-packet options */
#define T_IP_TOS		2	/* IP per-packet type of service */
#define T_IP_TTL		3	/* IP per-packet time to live */
#define T_IP_REUSEADDR		4	/* allow local address reuse */
#define T_IP_DONTROUTE		5	/* just use interface addresses */
#define T_IP_BROADCAST		6	/* permit sending of broadcast msgs */
#define T_IP_ADDR		7	/* dest/srce address of recv/sent packet */

/*
 *  IP_TOS precedence levels
 */
#define T_ROUTINE		0
#define T_PRIORITY		1
#define T_IMMEDIATE		2
#define T_FLASH			3
#define T_OVERRIDEFLASH		4
#define T_CRITIC_ECP		5
#define T_INETCONTROL		6
#define T_NETCONTROL		7

/*
 *  IP_TOS type of service
 */
#define T_NOTOS			0
#define T_LDELAY		(1<<4)
#define T_HITHRPT		(1<<3)
#define T_HIREL			(1<<2)
#define T_LOCOST		(1<<1)

#define SET_TOS(prec, tos) \
	(((0x7 & (prec)) << 5) | ((T_NOTOS|T_LDELAY|T_HITHRPT|T_HIREL|T_LOCOST) & (tos)))

#endif				/* _SYS_XTI_IP_H */

