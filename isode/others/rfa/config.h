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

#ifndef __RFA_CONFIG_H__
#define __RFA_CONFIG_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * Contributed by Oliver Wenzel 1990
 *
 * config.h - installation dependant configuration
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/config.h,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: config.h,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/*--- default root for RFA tree (can be changed at runtime) ---*/
#define FS_BASE		"/usr/something"

/*--- dir where isotailor and .rfarc files are expected ---*/
#define RFA_TAILDIR     "/usr/local/isode/etc"

/*--- timeout in sec. for lock operation of .rfainfo file  ---*/
#define LOCK_TIMEOUT    30

/*--- number of secs. for sending the time during the timeSync operation ---*/
#define SENDTIME_DELAY	1

/*--- default limit for filesize when compression of file shall occur. 
      (can be changed at runtime) ---*/
#define COMPRESSLIMIT	4500
/*--- Will be calculated as follows:

	     C0 : time required to start compress process
	     U0 : time required to start compress process
	     C	: time required to compress 1K data
	     U	: time required to compress 1K data
	     T	: time required to transfer 1K data
	     FC : factor by which the data size is reduced by compression
	     L  : Limit when data should be transfered compressed

	           FC * (C0 + U0) 
	     L = ---------------------------
		  (FC - 1) * T - FC * (U + C)

	     With:

		  C0 = 1s
		  CU = 1s
		  C  = 1/50s (1/100 on Sparc with load = 1)
		  U  = 1/70s (1/140 on Sparc with load = 1)
		  T  = 8/10 s (X.25)
		  FC = 2.5

		  then  L =  4.48 Kbytes 
---*/

#endif				/* __RFA_CONFIG_H__ */
