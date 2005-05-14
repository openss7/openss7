/*****************************************************************************

 @(#) $RCSfile: irixcompat.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/05/14 08:34:38 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2005/05/14 08:34:38 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: irixcompat.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/05/14 08:34:38 $"

static char const ident[] =
    "$RCSfile: irixcompat.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/05/14 08:34:38 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>	/* for MOD_DEC_USE_COUNT etc */
#include <linux/init.h>

/* 
 *  This is my solution for those who don't want to inline GPL'ed functions or
 *  who don't use optimizations when compiling or specifies
 *  -fnoinline-functions or something of the like.  This file implements all
 *  of the extern inlines from the header files by just including the header
 *  files with the functions declared 'inline' instead of 'extern inline'.
 *
 *  There are implemented here in a separate object, out of the way of the
 *  modules that don't use them.
 */

#define __IRIX_EXTERN_INLINE inline

#include <linux/kernel.h>	/* for vsprintf and friends */
#include <linux/vmalloc.h>	/* for vmalloc */
#ifdef CONFIG_PCI
#include <linux/pci.h>		/* for many pci functions */
#include <asm/pci.h>		/* for many pci functions */
#endif
#include <linux/ioport.h>	/* for check_region and friends */
#include <asm/uaccess.h>	/* for verify_area and friends */
#include <linux/timer.h>	/* for del_timer and friends */
#include <asm/semaphore.h>	/* for semaphores */
#include <linux/sched.h>	/* for kill_proc, jiffies and friends */
#include <linux/kmod.h>		/* for request_module and friends */
#include <linux/threads.h>	/* for NR_CPUS */
#include <asm/dma.h>		/* for request_dma and friends */
#include <linux/fs.h>		/* for filesystem related stuff */
#include <linux/time.h>		/* for do_gettimeofday and friends */
#include <asm/io.h>		/* for virt_to_page and friends */
#include <linux/slab.h>		/* for kmalloc and friends */
#include <asm/irq.h>		/* for disable_irq */
#include <asm/system.h>		/* for sti, cli */
#include <asm/delay.h>		/* for udelay */
#include <linux/spinlock.h>	/* for spinlock functions */
#include <asm/atomic.h>		/* for atomic functions */
#include <linux/poll.h>		/* for poll_table */
#include <linux/string.h>

#define _IRIX_SOURCE
#include <sys/kmem.h>		/* for SVR4 style kmalloc functions */
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/kernel/strsched.h"
#include "src/kernel/strutil.h"
#include "src/modules/sth.h"
#include "src/kernel/strsad.h"

#define IRIXCOMP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define IRIXCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define IRIXCOMP_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/05/14 08:34:38 $"
#define IRIXCOMP_DEVICE		"IRIX 6.5.17 Compatibility"
#define IRIXCOMP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define IRIXCOMP_LICENSE	"GPL"
#define IRIXCOMP_BANNER		IRIXCOMP_DESCRIP	"\n" \
				IRIXCOMP_COPYRIGHT	"\n" \
				IRIXCOMP_REVISION	"\n" \
				IRIXCOMP_DEVICE		"\n" \
				IRIXCOMP_CONTACT	"\n"
#define IRIXCOMP_SPLASH		IRIXCOMP_DEVICE		" - " \
				IRIXCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_IRIX_MODULE
MODULE_AUTHOR(IRIXCOMP_CONTACT);
MODULE_DESCRIPTION(IRIXCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(IRIXCOMP_DEVICE);
MODULE_LICENSE(IRIXCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-irixcompat");
#endif
#endif


#ifdef CONFIG_STREAMS_COMPAT_IRIX_MODULE
static
#endif
int __init irixcomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_IRIX_MODULE
	printk(KERN_INFO IRIXCOMP_BANNER);
#else
	printk(KERN_INFO IRIXCOMP_SPLASH);
#endif
	return (0);
}
#ifdef CONFIG_STREAMS_COMPAT_IRIX_MODULE
static
#endif
void __exit irixcomp_exit(void)
{
	return;
}

__IRIX_EXTERN_INLINE void icmn_err(int err_lvl, const char *fmt, va_list args);
EXPORT_SYMBOL(icmn_err);	/* irixddi.h */
__IRIX_EXTERN_INLINE void cmn_err_tag(int sequence, int err_lvl, const char *fmt, ... /* args */ ) __attribute__ ((format(printf, 3, 4)));
EXPORT_SYMBOL(cmn_err_tag);	/* irixddi.h */

#ifdef CONFIG_STREAMS_COMPAT_IRIX_MODULE
module_init(irixcomp_init);
module_exit(irixcomp_exit);
#endif
