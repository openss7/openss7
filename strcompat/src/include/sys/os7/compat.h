/*****************************************************************************

 @(#) $Id: compat.h,v 0.9.2.14 2005/04/05 02:00:47 brian Exp $

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

 Last Modified $Date: 2005/04/05 02:00:47 $ by $Author: brian $

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
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#endif				/* LINUX */

#ifdef LINUX
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/kdev_t.h>
#if HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>
#endif
#if ! HAVE_KTYPE_IRQRETURN_T
typedef void irqreturn_t;
#endif
#undef IRQ_NONE
#define IRQ_NONE	(0)
#undef IRQ_HANDLED
#define IRQ_HANDLED	(1)
#undef IRQ_RETVAL
#define IRQ_RETVAL(x)	((x) != 0)
#endif				/* LINUX */

#ifndef __MOD_INC_USE_COUNT
#undef MOD_INC_USE_COUNT
#define MOD_INC_USE_COUNT
#endif
#ifndef __MOD_DEC_USE_COUNT
#undef MOD_DEC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/kmem.h>
#include <sys/dki.h>
#include <sys/ddi.h>

#ifdef LFS
#include <sys/strsubr.h>
#include <sys/strconf.h>
#endif				/* LFS */

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

#include "os7/debug.h"		/* generic debugging macros */
#include "os7/bufq.h"		/* generic buffer queues */
#include "os7/priv.h"		/* generic data structures */
#include "os7/lock.h"		/* generic queue locking functions */
#include "os7/queue.h"		/* generic put and srv routines */
#include "os7/allocb.h"		/* generic buffer allocation routines */
#include "os7/timer.h"		/* generic timer handling */

#ifdef LISASSERT
#undef LISASSERT
#define LISASSERT(__assertion) assert((__assertion))
#undef ASSERT
#endif

#ifndef ASSERT
#define ASSERT(__assertion) assert((__assertion))
#endif

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

#ifdef LFS
#define ALLOC(__s) kmem_alloc((__s), KM_NOSLEEP)
#define FREE(__p) kmem_free((__p), sizeof(*(__p)))
#define SPLSTR(__pl) ({ (__pl) = splstr(); (void)0; })
#define SPLX(__pl) splx(__pl)
#else				/* LFS */
typedef lis_flags_t pl_t;
#endif				/* LFS */

#endif				/* __LOCAL_COMPAT_H__ */
