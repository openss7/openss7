/*****************************************************************************

 @(#) $Id: compat.h,v 0.9.2.3 2004/08/29 20:25:01 brian Exp $

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

 Last Modified $Date: 2004/08/29 20:25:01 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_COMPAT_H__
#define __LOCAL_COMPAT_H__

#if defined LIS && !defined _LIS_SOURCE
#define _LIS_SOURCE
#endif

/*
 *  Unfortunately this is necessary for older non-rpm LIS releases.
 */
#ifdef LINUX
#   include <linux/config.h>
#   include <linux/version.h>
#   ifndef HAVE_SYS_LIS_MODULE_H
#	ifdef MODVERSIONS
#	    include <linux/modversions.h>
#	endif			/* MODVERSIONS */
#	include <linux/module.h>
#	include <linux/modversions.h>
#	ifndef __GENKSYMS__
#	    if defined HAVE_SYS_LIS_MODVERSIONS_H
#		include <sys/LiS/modversions.h>
#	    elif defined HAVE_SYS_STREAMS_MODVERSIONS_H
#		include <sys/streams/modversions.h>
#	    endif
#	endif			/* __GENKSYMS__ */
#	include <linux/init.h>
#   else			/* HAVE_SYS_LIS_MODULE_H */
#	include <sys/LiS/module.h>
#   endif			/* HAVE_SYS_LIS_MODULE_H */
#endif				/* LINUX */

#ifdef LINUX
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/kdev_t.h>
#endif				/* LINUX */

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/kmem.h>
#include <sys/dki.h>
#include <sys/ddi.h>

#ifdef LFS
#include <sys/strsubr.h>
#include <sys/strconf.h>
#endif				/* LFS */

#ifdef LIS
#include <sys/osif.h>
typedef void (*bufcall_fnc_t) (long);
#ifdef LIS_2_12
#define INT int
#else				/* LIS_2_12 */
#define INT void
#endif				/* LIS_2_12 */
#else				/* LIS */
#define INT int
#endif				/* LIS */

#ifndef tid_t
typedef int tid_t;
#define tid_t tid_t
#endif				/* tid_t */

/* minor device number range */
#if defined LIS
#define NMINORS		MINORMASK	/* really 255 for LiS */
#elif defined LFS
#define NMINORS		((1UL<<16)-1)	/* really big for LFS */
#else
#define NMINORS		((1UL<<8)-1)	/* be conservative for others */
#endif

#include "debug.h"		/* generic debugging macros */
#include "bufq.h"		/* generic buffer queues */
#include "priv.h"		/* generic data structures */
#include "lock.h"		/* generic queue locking functions */
#include "queue.h"		/* generic put and srv routines */
#include "allocb.h"		/* generic buffer allocation routines */
#include "timer.h"		/* generic timer handling */

//#ifdef LINUX
//#include <linux/interrupt.h>
//#include "bufpool.h"          /* generic buffer pooling */
//#endif

#ifdef LINUX
#ifdef MODULE
#define MODULE_STATIC STATIC
#else				/* MODULE */
#define MODULE_STATIC
#endif				/* MODULE */
#else				/* LINUX */
#define MODULE_STATIC STATIC
#endif				/* LINUX */

#endif				/* __LOCAL_COMPAT_H__ */
