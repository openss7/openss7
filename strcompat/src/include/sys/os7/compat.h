/*****************************************************************************

 @(#) $Id: compat.h,v 0.9.2.30 2006/04/22 01:05:35 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/04/22 01:05:35 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_COMPAT_H__
#define __LOCAL_COMPAT_H__

#ident "@(#) $RCSfile: compat.h,v $ $Name:  $($Revision: 0.9.2.30 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

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
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/kdev_t.h>
#ifdef HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>
#endif
#ifndef HAVE_KTYPE_IRQRETURN_T
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
#define streamscall _RP
#define STREAMSCALL(__X) __X streamscall
#define streams_fastcall __attribute__((__regparm__(3)))
#define STREAMS_FASTCALL(__X) __X streams_fastcall
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
#ifdef LIS
#define NMINORS		MINORMASK	/* really 255 for LiS */
#else
#ifdef LFS
#define NMINORS		((1UL<<16)-1)	/* really big for LFS */
#else
#define NMINORS		((1UL<<8)-1)	/* be conservative for others */
#endif
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
