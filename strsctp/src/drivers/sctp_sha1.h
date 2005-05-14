/*****************************************************************************

 @(#) $Id: sctp_sha1.h,v 0.9.2.3 2005/05/14 08:29:19 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:29:19 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCTP_SHA1_H__
#define __SCTP_SHA1_H__

#ident "@(#) $RCSfile: sctp_sha1.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2004 OpenSS7 Corporation."

/* 
 *  The structure for storing SHS info
 */
typedef struct {
	uint32_t dig[5];		/* Message digest */
	uint32_t lo, hi;		/* 64-bit bit count */
	uint32_t dat[16];		/* SHS data buffer */
} SHA_CTX;

extern void SHAInit(SHA_CTX * sha1);
extern void SHAUpdate(SHA_CTX * sha1, uint8_t *buf, int len);
extern void SHAFinal(uint8_t *out, SHA_CTX * sha1);

#endif				/* __SCTP_SHA1_H__ */
