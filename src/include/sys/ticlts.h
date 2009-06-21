/*****************************************************************************

 @(#) $Id: ticlts.h,v 1.1.2.1 2009-06-21 11:25:38 brian Exp $

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

 Last Modified $Date: 2009-06-21 11:25:38 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ticlts.h,v $
 Revision 1.1.2.1  2009-06-21 11:25:38  brian
 - added files to new distro

 *****************************************************************************/

#ifndef _SYS_TICLTS_H
#define _SYS_TICLTS_H

#ident "@(#) $RCSfile: ticlts.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

/* This file can be processed with doxygen(1). */

/** @addtogroup loopback
  * @{ */

/** @file
  * TPI CLTS Loopback header file.  */

/*
 * TPI CLTS Loopback Header File.
 */

#if 0
#if !defined _TICLTS_H && !defined __KERNEL__
#error ****
#error **** DO NOT INCLUDE SYSTEM HEADER FILES DIRECTLY IN USER-SPACE
#error **** PROGRAMS.  LIKELY YOU SHOULD HAVE INCLUDED <ticlts.h>
#error **** INSTEAD OF <sys/ticlts.h>.
#error ****
#endif				/* !defined _TICLTS_H && !defined __KERNEL__ */
#endif

#if __SVID
#define TCL_BADADDR	    1	/**< Bad address format or illegal address values. */
#define TCL_BADOPT	    2	/**< Bad options format or illegal options values. */
#define TCL_NOPEER	    3	/**< Destination address is not bound. */
#define TCL_PEERBADSTATE    4	/**< Transport peer in incorrect state. */
#define TCL_DEFAULTADDRSZ   4
#else				/* __SVID */
#define TCL_BADADDR	    EINVAL	/**< Bad address format or illegal address values. */
#define TCL_BADOPT	    EINVAL	/**< Bad options format or illegal options values. */
#define TCL_NOPEER	    EFAULT	/**< Destination address is not bound. */
#define TCL_PEERBADSTATE    EPROTO	/**< Transport peer in incorrect state. */
#define TCL_DEFAULTADDRSZ   4
#endif				/* __SVID */

#endif				/* _SYS_TICLTS_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
