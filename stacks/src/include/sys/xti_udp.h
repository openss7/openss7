/*****************************************************************************

 @(#) $Id: xti_udp.h,v 0.9.2.5 2006/09/25 12:10:09 brian Exp $

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

 Last Modified $Date: 2006/09/25 12:10:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_udp.h,v $
 Revision 0.9.2.5  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 Revision 0.9.2.4  2006/09/18 13:52:37  brian
 - added doxygen markers to sources

 Revision 0.9.2.3  2005/05/14 08:30:49  brian
 - copyright header correction

 Revision 0.9.2.2  2004/09/02 09:31:15  brian
 - Synchronization with other packages.

 Revision 0.9.2.1  2004/05/12 08:01:40  brian
 - Added in xti library and STREAMS modules.

 Revision 1.1.2.1  2004/04/13 12:12:52  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _SYS_XTI_UDP_H
#define _SYS_XTI_UDP_H

#ident "@(#) $RCSfile: xti_udp.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI UDP-Specific header file.
  *
  * In accordance with OpenGroup Single UNIX Specifications, the symbols in this
  * file are exposed by including the <xti.h> header file.  */

/*
 *  XTI UDP-Specific Header File
 */

/**
  * UDP Level.
  */
#define T_INET_UDP		17	/**< UDP level (same as protocol number). */

/**
  * @name UDP Level Options
  * @{ */
#define T_UDP_CHECKSUM		1	/**< Checksum computation. */
/** @} */

#endif				/* _SYS_XTI_UDP_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
