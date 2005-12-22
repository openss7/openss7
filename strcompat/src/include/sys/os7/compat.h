/*****************************************************************************

 @(#) $Id: compat.h,v 0.9.2.27 2005/12/22 10:28:51 brian Exp $

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

 Last Modified $Date: 2005/12/22 10:28:51 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_COMPAT_H__
#define __LOCAL_COMPAT_H__

/*
 *  Unfortunately this is necessary for older non-rpm LIS releases.
 */
#ifdef LINUX
#include <linux/config.h>
#include <linux/version.h>
#include <linux/compiler.h>
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

#ifndef EXPORT_SYMBOL_NOVERS
#define EXPORT_SYMBOL_NOVERS(__sym) EXPORT_SYMBOL(__sym)
#endif

#ifdef LIS
#define STREAMSCALL(__X) __X _RP
#define streamscall STREAMSCALL()
#define STREAMS_FASTCALL(__X) __X __attribute__((__regparm__(3)))
#define streams_fastcall STREAMS_FASTCALL()
#endif

#ifdef LFS
#define _RP streamscall
#endif

#define _OS7_SOURCE

#include <sys/stream.h>

#include <sys/cmn_err.h>
#include <sys/kmem.h>
#include <sys/dki.h>
#include <sys/ddi.h>

#include <sys/strconf.h>
#include <sys/strlog.h>

/* queue flags */
#if defined LIS
#define QSVCBUSY QRUNNING
#endif

/* minor device number range */
#if defined LIS
#define NMINORS		MINORMASK	/* really 255 for LiS */
#elif defined LFS
#define NMINORS		((1UL<<16)-1)	/* really big for LFS */
#else
#define NMINORS		((1UL<<8)-1)	/* be conservative for others */
#endif

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
