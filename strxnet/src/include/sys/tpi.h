/*****************************************************************************

 @(#) $Id: tpi.h,v 0.9 2004/04/05 12:37:53 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/05 12:37:53 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_TPI_H
#define _SYS_TPI_H

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

#if !defined _XTI_H && !defined _TIUSER_H && !defined _TIHDR_H && !defined __KERNEL__
#error ****
#error **** DO NOT INCLUDE SYSTEM HEADER FILS DIRECTLY IN USER-SPACE
#error **** PROGRAMS.  LIKELY YOU SHOULD HAVE INCLUDED <timod.h>
#error **** INSTEAD OF <sys/timod.h>.
#error ****
#endif

/*
 *  This file contains definitions that are common to XTI, TLI and TPI.  It is
 *  included by <xti.h> and <tiuser.h> as well as <tihdr.h>.  TLI specifics
 *  can be found in <sys/tli.h>.
 */

/* 
 *  Transport service error numbers
 *  (error codes used by TLI transport providers)
 */
#define TBADADDR	 1	/* Bad address format */
#define TBADOPT		 2	/* Bad options format */
#define TACCES		 3	/* Bad permissions */
#define TBADF		 4	/* Bad file descriptor */
#define TNOADDR		 5	/* Unable to allocate an address */
#define TOUTSTATE	 6	/* Would place interface out of state */
#define TBADSEQ		 7	/* Bad call sequence number */
#define TSYSERR		 8	/* System error */
#define TLOOK		 9	/* Has to t_look() for event			*/	/* not TPI */
#define TBADDATA	10	/* Bad amount of data */
#define TBUFOVFLW	11	/* Buffer was too small				*/	/* not TPI */
#define TFLOW		12	/* Would block due to flow control		*/	/* not TPI */
#define TNODATA		13	/* No data indication				*/	/* not TPI */
#define TNODIS		14	/* No disconnect indication			*/	/* not TPI */
#define TNOUDERR	15	/* No unitdata error indication			*/	/* not TPI */
#define TBADFLAG	16	/* Bad flags */
#define TNOREL		17	/* No orderly release indication		*/	/* not TPI */
#define TNOTSUPPORT	18	/* Not supported */
#define TSTATECHNG	19	/* State is currently changing			*/	/* not TPI */
/* 
 *  The following are XPG3 and up.
 */
#if defined _XPG3 || defined _XPG4_2 || defined _XOPEN_SOURCE || defined __KERNEL__
#define TNOSTRUCTYPE	20	/* Structure type not supported			*/	/* not TPI */
#define TBADNAME	21	/* Bad transport provider name			*/	/* not TPI */
#define TBADQLEN	22	/* Listener queue length limit is zero		*/	/* not TPI */
#define TADDRBUSY	23	/* Address already in use			*/	/* not TPI */
#endif
/* 
 *  The following are _XOPEN_SOURCE (XPG4 and up).
 */
#if defined _XPG4_2 || defined _XOPEN_SOURCE || defined __KERNEL__
#define TINDOUT		24	/* Outstanding connect indications		*/	/* not TPI */
#define TPROVMISMATCH	25	/* Not same transport provider			*/	/* not TPI */
#define TRESQLEN	26	/* Connection acceptor has qlen > 0		*/	/* not TPI */
#define TRESADDR	27	/* Conn. acceptor bound to different address	*/	/* not TPI */
#define TQFULL		28	/* Connection indicator queue is full		*/	/* not TPI */
#define TPROTO		29	/* Protocol error				*/	/* not TPI */
#endif



#endif  _SYS_TPI_H

