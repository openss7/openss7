/*****************************************************************************

 @(#) $Id: tli.h,v 0.9.2.2 2005/05/14 08:30:48 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:48 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_TLI_H
#define _SYS_TLI_H

#ident "@(#) $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

#if 0
#if !defined _TIUSER_H && !defined _TIHDR_H && !defined __KERNEL__
#error ****
#error **** DO NOT INCLUDE SYSTEM HEADER FILES DIRECTLY IN USER-SPACE
#error **** PROGRAMS.  LIKELY YOU SHOULD HAVE INCLUDED <tiuser.h>
#error **** OR <tihdr.h> INSTEAD OF <sys/tiuser.h> OR <sys/tihdr.h>.
#error ****
#endif				/* !defined _TIUSER_H && !defined _TIHDR_H && !defined __KERNEL__ */
#endif

/* This header file is included by <tiuser.h> and <tihdr.h>. */

#if (!defined _XPG4_2 && !defined _XOPEN_SOURCE) || defined __KERNEL__
struct opthdr {
	u_int32_t level;		/* protocol level */
	u_int32_t name;			/* option name */
	u_int32_t len;			/* length of option value (excluding header) */
};

#define OPTLEN(len) ((((len) + sizeof(u_int32_t) - 1) / sizeof(u_int32_t)) * sizeof(u_int32_t))
#define OPTVAL(opt) ((caddr_t)((struct opthdr *)opt + 1))
#endif				/* (!defined _XPG4_2 && !defined _XOPEN_SOURCE) || defined
				   __KERNEL__ */

#endif				/* _SYS_TLI_H */
