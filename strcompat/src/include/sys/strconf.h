/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.8 2005/07/12 13:54:41 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/07/12 13:54:41 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STRCONF_H__
#define __SYS_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2005/07/12 13:54:41 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifdef LIS
#include <sys/LiS/strconf.h>
#endif
#ifdef LFS
#include <sys/streams/strconf.h>
#endif

#ifdef LIS
/*
 *  We need these for all variations.
 */
#define STR_IS_DEVICE   0x00000001	/* device */
#define STR_IS_MODULE   0x00000002	/* module */
#define STR_TYPE_MASK   (STR_IS_DEVICE|STR_IS_MODULE)
#define STR_SYSV4_OPEN  0x00000100	/* V.4 open signature/return */
#define STR_QSAFETY     0x00000200	/* Module needs safe callbacks */
#define STR_IS_SECURE   0x00010000	/* Module/device is secure */

typedef enum {
	SQLVL_DEFAULT = 0,		/* default level */
	SQLVL_GLOBAL = 1,	/* STREAMS scheduler level */
	SQLVL_ELSEWHERE = 2,	/* module group level */
	SQLVL_MODULE = 3,	/* module level (default) */
	SQLVL_QUEUEPAIR = 4,	/* queue pair level */
	SQLVL_QUEUE = 5,	/* queue level */
	SQLVL_NOP = 6,		/* no synchronization */
} sqlvl_t;
#endif

/*
 *  Pull in the appropriate system specific strconf.h file.
 */

#ifdef _SVR3_SOURCE
#include <sys/svr3/strconf.h>
#endif
#ifdef _SVR4_SOURCE
#include <sys/svr4/strconf.h>
#endif
#ifdef _LFS_SOURCE
#ifdef LIS
#include <sys/lfs/strconf.h>
#endif
#endif				/* _LFS_SOURCE */
#ifdef _LIS_SOURCE
#ifdef LFS
#include <sys/lis/strconf.h>
#endif
#endif				/* _LIS_SOURCE */
#ifdef _MPS_SOURCE
#include <sys/mps/strconf.h>
#endif
#ifdef _OSF_SOURCE
#include <sys/osf/strconf.h>
#endif
#ifdef _AIX_SOURCE
#include <sys/aix/strconf.h>
#endif
#ifdef _HPUX_SOURCE
#include <sys/hpux/strconf.h>
#endif
#ifdef _UW7_SOURCE
#include <sys/uw7/strconf.h>
#endif
#ifdef _SUN_SOURCE
#include <sys/sun/strconf.h>
#endif
#ifdef _MAC_SOURCE
#include <sys/mac/strconf.h>
#endif
#ifdef _IRIX_SOURCE
#include <sys/irix/strconf.h>
#endif
#ifdef _OS7_SOURCE
#include <sys/os7/strconf.h>
#endif

#endif				/* __SYS_STRCONF_H__ */
