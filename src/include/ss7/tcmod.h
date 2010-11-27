/*****************************************************************************

 @(#) $Id: tcmod.h,v 1.1.2.1 2009-06-21 11:25:34 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-06-21 11:25:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tcmod.h,v $
 Revision 1.1.2.1  2009-06-21 11:25:34  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SS7_TCMOD_H__
#define __SS7_TCMOD_H__

/* This file can be processed by doxygen(1). */

/** @addtogroup tcmod
  * @{ */

/** @file
  * Transaction Capabilities Sub-layer Module "tcmod" Header file.  */

/*
 * Transaction Capabilities "tcmod" Header File.
 */

#define TCMOD	    ('C'<<8)

/** @name tcmod Input/Output Controls
  * @{*/
#define TC_GETINFO	(TCMOD|100)	/**< Get information from TC. */
#define TC_OPTMGMT	(TCMOD|101)	/**< Manage options for TC. */
#define TC_BIND		(TCMOD|102)	/**< Bind TC endpoint. */
#define TC_UNBIND	(TCMOD|103)	/**< Unbind a TC endpoint. */

#define TC_BEGIN	(TCMOD|104)	/**< Begin a dialogue. */
#define TC_ACCEPT	(TCMOD|105)	/**< Accept a dialogue. */
#define TC_CONT		(TCMOD|106)	/**< Continue a dialogue. */
#define TC_END		(TCMOD|107)	/**< End a dialogue. */
#define TC_ABORT	(TCMOD|108)	/**< Abort a dialogue. */

#define TC_INVOKE	(TCMOD|109)	/**< Invoke an operation. */
#define TC_RESULT	(TCMOD|110)	/**< Result of an operation. */
#define TC_CANCEL	(TCMOD|111)	/**< Cancel an operation. */
#define TC_ERROR	(TCMOD|112)	/**< Error for an operation. */
#define TC_REJECT	(TCMOD|113)	/**< Reject an operation. */

#define TC_COORD	(TCMOD|114)	/**< Request coordinated withdrawal from mate. */
#define TC_ALLOW	(TCMOD|115)	/**< Allow coordinated withdrawal to mate. */
#define TC_STATUS	(TCMOD|116)	/**< Set endpoint TC user status. */
/** @} */

#endif				/* __SS7_TCMOD_H__ */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
