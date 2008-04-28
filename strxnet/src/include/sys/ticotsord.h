/*****************************************************************************

 @(#) $Id: ticotsord.h,v 0.9.2.10 2008-04-28 18:38:36 brian Exp $

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

 Last Modified $Date: 2008-04-28 18:38:36 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ticotsord.h,v $
 Revision 0.9.2.10  2008-04-28 18:38:36  brian
 - header updates for release

 Revision 0.9.2.9  2007/08/14 04:00:45  brian
 - GPLv3 header update

 Revision 0.9.2.8  2006/09/25 12:04:43  brian
 - updated headers, moved xnsl

 Revision 0.9.2.7  2006/09/24 21:57:22  brian
 - documentation and library updates

 Revision 0.9.2.6  2006/09/22 20:59:27  brian
 - prepared header file for use with doxygen, touching many lines

 Revision 0.9.2.5  2006/09/18 13:52:45  brian
 - added doxygen markers to sources

 Revision 0.9.2.4  2005/05/14 08:28:29  brian
 - copyright header correction

 Revision 0.9.2.3  2005/04/22 22:49:27  brian
 - move tpi documentation to strxnet

 Revision 0.9.2.2  2005/01/11 08:47:24  brian
 - Minor additions and corrections.

 Revision 0.9.2.1  2004/05/16 04:12:32  brian
 - Updating strxnet release.

 Revision 0.9  2004/05/14 08:00:02  brian
 - Updated xns, tli, inet, xnet and documentation.

 Revision 0.9.2.1  2004/04/13 12:12:52  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _SYS_TICOTSORD_H
#define _SYS_TICOTSORD_H

#ident "@(#) $RCSfile: ticotsord.h,v $ $Name:  $($Revision: 0.9.2.10 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup loopback
  * @{ */

/** @file
  * TPI COTS w/ Orderly Release header file.  */

/*
 * TPI COTS w/ Orderly Release Header File.
 */

#if 0
#if !defined _TICOTSORD_H && !defined __KERNEL__
#error ****
#error **** DO NOT INCLUDE SYSTEM HEADER FILS DIRECTLY IN USER-SPACE
#error **** PROGRAMS.  LIKELY YOU SHOULD HAVE INCLUDED <ticotsord.h>
#error **** INSTEAD OF <sys/ticotsord.h>.
#error ****
#endif				/* !defined _TICOTSORD_H && !defined __KERNEL__ */
#endif

#if __SVID
#define TCOO_NOPEER		1	/* destiniation address is not listening */
#define TCOO_PEERNOROMMONQ	2	/* no room on connection indication queue */
#define TCOO_PEERBADSTATE	3	/* transport peer in incorrect state */
#define TCOO_PEERINITIATED	4	/* transport peer user-initiated disconnect */
#define TCOO_PROVIDERINITIATED	5	/* transport peer provider-initiated disconnect */
#define TCOO_DEFAULTADDRSZ	4
#else				/* __SVID */
#define TCOO_NOPEER		ECONNREFUSED	/* destiniation address is not listening */
#define TCOO_PEERNOROMMONQ	ECONNREFUSED	/* no room on connection indication queue */
#define TCOO_PEERBADSTATE	ECONNREFUSED	/* transport peer in incorrect state */
#define TCOO_PEERINITIATED	ECONNRESET	/* transport peer user-initiated disconnect */
#define TCOO_PROVIDERINITIATED	ECONNABORTED	/* transport peer provider-initiated disconnect */
#define TCOO_DEFAULTADDRSZ	4
#endif				/* __SVID */

#endif				/* _SYS_TICOTSORD_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
