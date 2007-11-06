/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __CONFIG_CCUR_H__
#define __CONFIG_CCUR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ccur.h - site configuration for Concurrent RTU 6.0 */

/* 
 * Header: /xtel/isode/isode/config/RCS/ccur.h,v 9.0 1992/06/16 12:08:13 isode Rel
 *
 *
 * Log: ccur.h,v
 * Revision 9.0  1992/06/16  12:08:13  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef	_CONFIG_
#define	_CONFIG_

#define	DEBUG	2
#define	SYS5
#define	WRITEV
#define	VSPRINTF		/* varargs */

#define	TCP			/* has TCP/IP (of course) */
#define	TLI_TCP			/* provided by TLI */
#define	SOCKETS			/* sockets also available */
#define	BIND

#define X25			/* has X.25 */
#define CCUR_X25		/* provided by RTnet-X25/PLUS R01 */

#define TP4			/* has Transport Class 4 */
#define TLI_TP			/* provided by the TLI */
#define RTnet_R02		/* using RTnet-OSI R02 or later */
#define TSDU_COPY_LIMIT 10240

#if 0
#define TLI_POLL		/* use poll(2) instead of select(2) WARNING: only use this if all
				   network stacks are provided via streams.  With select(2) you can 
				   only have 32 file descriptors. Poll(2) will handle as many as
				   are congigured in the kernal but *only* works over streams
				   devices.  To get more than 32 descriptors in ISODE you will need 
				   to modify the size of fdset in h/manifest.h. */
#endif

#define NOGOSIP

#define NFS

#endif				/* _CONFIG_ */

#endif				/* __CONFIG_CCUR_H__ */
