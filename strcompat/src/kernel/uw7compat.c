/*****************************************************************************

 @(#) uw7compat.c,v (1.1.2.6) 2003/10/28 08:00:05

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified 2003/10/28 08:00:05 by brian

 *****************************************************************************/

#ident "@(#) uw7compat.c,v (1.1.2.6) 2003/10/28 08:00:05"

static char const ident[] = "uw7compat.c,v (1.1.2.6) 2003/10/28 08:00:05";

#include <linux/config.h>
#include <linux/module.h>	/* for MOD_DEC_USE_COUNT etc */

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

#define __UW7_EXTERN_INLINE inline

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
#include <linux/kmem.h>		/* for SVR4 style kmalloc functions */

#define _UW7_SOURCE
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strconf.h>
#include <linux/strsubr.h>
#include <linux/ddi.h>

#include "strdebug.h"
#include "strsched.h"
#include "strutil.h"
#include "strhead.h"
#include "strsad.h"

#define UW7COMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define UW7COMP_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define UW7COMP_REVISION	"LfS uw7compat.c,v (1.1.2.6) 2003/10/28 08:00:05"
#define UW7COMP_DEVICE		"UnixWare(R) 7.1.3 Compatibility"
#define UW7COMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define UW7COMP_LICENSE		"GPL"
#define UW7COMP_BANNER		UW7COMP_DESCRIP		"\n" \
				UW7COMP_COPYRIGHT	"\n" \
				UW7COMP_REVISION	"\n" \
				UW7COMP_DEVICE		"\n" \
				UW7COMP_CONTACT		"\n"
#define UW7COMP_SPLASH		UW7COMP_DEVICE		" - " \
				UW7COMP_REVISION	"\n"

MODULE_AUTHOR(UW7COMP_CONTACT);
MODULE_DESCRIPTION(UW7COMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(UW7COMP_DEVICE);
MODULE_LICENSE(UW7COMP_LICENSE);

/* don't use these functions, they are way too dangerous */
long MPSTR_QLOCK(queue_t *q)
{
	unsigned long flags;
	qwlock(q, &flags);
	return (flags);
}
void MPSTR_QRELE(queue_t *q, long s)
{
	qwunlock(q, (unsigned long *) &s);
}
long MPSTR_STPLOCK(struct stdata *stp)
{
	unsigned long flags;
	swlock(stp, &flags);
	return (flags);
}
void MPSTR_STPRELE(struct stdata *stp, long s)
{
	swunlock(stp, (unsigned long *) &s);
}

/* don't use these - these are fakes */
mblk_t *allocb_physreq(size_t size, uint priority, physreq_t * prp)
{
	if (prp->phys_align > 8)
		return (NULL);
	if (prp->phys_boundary != 0)
		return (NULL);
	if (prp->phys_dmasize != 0)
		return (NULL);
	if (prp->phys_flags & PREQ_PHYSCONTIG)
		return (NULL);
	return (allocb(size, priority));
}
mblk_t *msgphysreq(mblk_t *mp, physreq_t * prp)
{
	if (prp->phys_align > 8)
		return (NULL);
	if (prp->phys_boundary != 0)
		return (NULL);
	if (prp->phys_dmasize != 0)
		return (NULL);
	if (prp->phys_flags & PREQ_PHYSCONTIG)
		return (NULL);
	return (mp);
}
mblk_t *msgpullup_physreq(mblk_t *mp, size_t len, physreq_t * prp)
{
	if (prp->phys_align > 8)
		return (NULL);
	if (prp->phys_boundary != 0)
		return (NULL);
	if (prp->phys_dmasize != 0)
		return (NULL);
	if (prp->phys_flags & PREQ_PHYSCONTIG)
		return (NULL);
	return msgpullup(mp, len);
}
mblk_t *msgscgth(mblk_t *mp, physreq_t * prp, scgth_t * sgp)
{
	return (NULL);
}

__UW7_EXTERN_INLINE major_t getemajor(dev_t dev);
__UW7_EXTERN_INLINE minor_t geteminor(dev_t dev);
__UW7_EXTERN_INLINE major_t emajor(dev_t dev);
__UW7_EXTERN_INLINE minor_t eminor(dev_t dev);
__UW7_EXTERN_INLINE int etoimajor(major_t emajor);
__UW7_EXTERN_INLINE int itoemajor(major_t imajor, int prevemaj);

__UW7_EXTERN_INLINE int printf(char *fmt, ...);

__UW7_EXTERN_INLINE int LOCK_OWNED(lock_t * lockp);

__UW7_EXTERN_INLINE rwlock_t *RW_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop, int flag);
__UW7_EXTERN_INLINE void RW_DEALLOC(rwlock_t *lockp);
__UW7_EXTERN_INLINE pl_t RW_RDLOCK(rwlock_t *lockp, pl_t pl);
__UW7_EXTERN_INLINE pl_t RW_TRYRDLOCK(rwlock_t *lockp, pl_t pl);
__UW7_EXTERN_INLINE pl_t RW_TRYWRLOCK(rwlock_t *lockp, pl_t pl);
__UW7_EXTERN_INLINE void RW_UNLOCK(rwlock_t *lockp, pl_t pl);
__UW7_EXTERN_INLINE pl_t RW_WRLOCK(rwlock_t *, pl_t pl);

__UW7_EXTERN_INLINE void ATOMIC_INT_ADD(atomic_int_t *counter, int value);
__UW7_EXTERN_INLINE atomic_int_t *ATOMIC_INT_ALLOC(int flag);
__UW7_EXTERN_INLINE void ATOMIC_INT_DEALLOC(atomic_int_t *counter);
__UW7_EXTERN_INLINE int ATOMIC_INT_DECR(atomic_int_t *counter);
__UW7_EXTERN_INLINE void ATOMIC_INT_INCR(atomic_int_t *counter);
__UW7_EXTERN_INLINE void ATOMIC_INT_INIT(atomic_int_t *counter, int value);
__UW7_EXTERN_INLINE int ATOMIC_INT_READ(atomic_int_t *counter);
__UW7_EXTERN_INLINE void ATOMIC_INT_SUB(atomic_int_t *counter, int value);
__UW7_EXTERN_INLINE void ATOMIC_INT_WRITE(atomic_int_t *counter, int value);

__UW7_EXTERN_INLINE sleep_t *SLEEP_ALLOC(int arg, lkinfo_t *lkinfop, int flag);
__UW7_EXTERN_INLINE void SLEEP_DEALLOC(sleep_t *lockp);
__UW7_EXTERN_INLINE int SLEEP_LOCKAVAIL(sleep_t *lockp);
__UW7_EXTERN_INLINE void SLEEP_LOCK(sleep_t *lockp, int priority);
__UW7_EXTERN_INLINE int SLEEP_LOCKOWNED(sleep_t *lockp);
__UW7_EXTERN_INLINE int SLEEP_LOCK_SIG(sleep_t *lockp, int priority);
__UW7_EXTERN_INLINE int SLEEP_TRYLOCK(sleep_t *lockp);
__UW7_EXTERN_INLINE void SLEEP_UNLOCK(sleep_t *lockp);

static int __init uw7comp_init(void)
{
#ifdef MODULE
	printk(KERN_INFO UW7COMP_BANNER);
#else
	printk(KERN_INFO UW7COMP_SPLASH);
#endif
	return (0);
}
static void __exit uw7comp_exit(void)
{
	return;
}

module_init(uw7comp_init);
module_exit(uw7comp_exit);

