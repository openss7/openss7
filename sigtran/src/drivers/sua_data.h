/*****************************************************************************

 @(#) $RCSfile: sua_data.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:42 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

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

 Last Modified $Date: 2006/10/17 11:55:42 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SUA_DATA_H__
#define __SUA_DATA_H__

#ident "@(#) $RCSfile: sua_data.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:42 $"

#include "../ua/ua_data.h"
/*
 *  =========================================================================
 *
 *  SUA Private Data Structures
 *
 *  =========================================================================
 *  SUA uses private data structures which are common to most of the User
 *  Adapatation layers.  SUA defines a new device/link and routing key class:
 */

typedef struct sccp {
	struct pp dp;			/* inherited device structure */
	uint state;			/* state of the SCCP-U/P interface */
	uint version;			/* version of SCCP interface */
	uint32_t flags;			/* flags to be included in SUA messages */
	uint32_t srn;			/* srce reference number */
	uint32_t drn;			/* dest reference number */
	uint32_t credit;		/* credit for class 3 operation */
	/* 
	 *  More...
	 */
} sccp_t;

typedef struct sccp_key {
	struct rk rk;			/* inherited routing key class */
	uint ssn;			/* subsystem number */
	uint pclass;			/* protocol class */
	/* 
	 *  More...
	 */
} sccp_key_t;

#endif				/* __SUA_DATA_H__ */
