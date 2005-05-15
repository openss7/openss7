/*****************************************************************************

 @(#) $RCSfile: svr4compat.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/15 04:08:15 $

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

 Last Modified $Date: 2005/05/15 04:08:15 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: svr4compat.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/15 04:08:15 $"

static char const ident[] =
    "$RCSfile: svr4compat.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/15 04:08:15 $";

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

#define __SVR4_EXTERN_INLINE inline

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
#include <linux/interrupt.h>	/* for local_irq functions */
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
#if HAVE_KINC_ASM_SOFTIRQ_H
#include <asm/softirq.h>	/* for local_bh_ functions */
#endif
#include <linux/poll.h>		/* for poll_table */
#include <linux/string.h>

#define _SVR4_SOURCE
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

#define SVR4COMP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SVR4COMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define SVR4COMP_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/05/15 04:08:15 $"
#define SVR4COMP_DEVICE		"UNIX(R) SVR 4.2 MP Compatibility"
#define SVR4COMP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SVR4COMP_LICENSE	"GPL"
#define SVR4COMP_BANNER		SVR4COMP_DESCRIP	"\n" \
				SVR4COMP_COPYRIGHT	"\n" \
				SVR4COMP_REVISION	"\n" \
				SVR4COMP_DEVICE		"\n" \
				SVR4COMP_CONTACT	"\n"
#define SVR4COMP_SPLASH		SVR4COMP_DEVICE		" - " \
				SVR4COMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_SVR4_MODULE
MODULE_AUTHOR(SVR4COMP_CONTACT);
MODULE_DESCRIPTION(SVR4COMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SVR4COMP_DEVICE);
MODULE_LICENSE(SVR4COMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-svr4compat");
#endif
#endif

/* don't use these functions, they are way too dangerous */
long MPSTR_QLOCK(queue_t *q)
{
	unsigned long flags;
	qwlock(q, &flags);
	return (flags);
}

EXPORT_SYMBOL(MPSTR_QLOCK);	/* svr4ddi.h */
void MPSTR_QRELE(queue_t *q, long s)
{
	qwunlock(q, (unsigned long *) &s);
}

EXPORT_SYMBOL(MPSTR_QRELE);	/* svr4ddi.h */
long MPSTR_STPLOCK(struct stdata *stp)
{
	unsigned long flags;
	swlock(stp, &flags);
	return (flags);
}

EXPORT_SYMBOL(MPSTR_STPLOCK);	/* svr4ddi.h */
void MPSTR_STPRELE(struct stdata *stp, long s)
{
	swunlock(stp, (unsigned long *) &s);
}

EXPORT_SYMBOL(MPSTR_STPRELE);	/* svr4ddi.h */

static pl_t current_spl[NR_CPUS] __cacheline_aligned;

pl_t spl0(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 0);
	local_irq_enable();
	local_bh_enable();
	return (old_level);
}

__SVR4_EXTERN_INLINE toid_t dtimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl, processorid_t processor);
EXPORT_SYMBOL(dtimeout);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE toid_t itimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl);
EXPORT_SYMBOL(itimeout);	/* svr4ddi.h */

EXPORT_SYMBOL(spl0);		/* svr4ddi.h */

pl_t spl1(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 1);
	local_irq_enable();
	local_bh_enable();
	return (old_level);
}

EXPORT_SYMBOL(spl1);		/* svr4ddi.h */

pl_t spl2(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 2);
	local_irq_enable();
	local_bh_enable();
	return (old_level);
}

EXPORT_SYMBOL(spl2);		/* svr4ddi.h */

pl_t spl3(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 3);
	local_bh_disable();
	local_irq_enable();
	return (old_level);
}

EXPORT_SYMBOL(spl3);		/* svr4ddi.h */

pl_t spl4(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 4);
	local_bh_disable();
	local_irq_enable();
	return (old_level);
}

EXPORT_SYMBOL(spl4);		/* svr4ddi.h */

pl_t spl5(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 5);
	local_irq_disable();
	local_bh_disable();
	return (old_level);
}

EXPORT_SYMBOL(spl5);		/* svr4ddi.h */

pl_t spl6(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 6);
	local_irq_disable();
	local_bh_disable();
	return (old_level);
}

EXPORT_SYMBOL(spl6);		/* svr4ddi.h */

pl_t spl7(void)
{
	pl_t old_level = xchg(&current_spl[smp_processor_id()], 7);
	local_irq_disable();
	local_bh_disable();
	return (old_level);
}

EXPORT_SYMBOL(spl7);		/* svr4ddi.h */

pl_t spl(const pl_t level)
{
	switch (level) {
	case 0:
		return spl0();
	case 1:
		return spl1();
	case 2:
		return spl2();
	case 3:
		return spl3();
	case 4:
		return spl4();
	case 5:
		return spl5();
	case 6:
		return spl6();
	case 7:
		return spl7();
	}
	swerr();
	return (invpl);
}

EXPORT_SYMBOL(spl);		/* svr4ddi.h */
void splx(const pl_t level)
{
	return (void) spl(level);
}

EXPORT_SYMBOL(splx);		/* svr4ddi.h */

__SVR4_EXTERN_INLINE major_t getemajor(dev_t dev);
EXPORT_SYMBOL(getemajor);	/* uw7ddi.h */
__SVR4_EXTERN_INLINE minor_t geteminor(dev_t dev);
EXPORT_SYMBOL(geteminor);	/* uw7ddi.h */

int etoimajor(major_t emajor)
{
	struct cdevsw *cdev;
#ifdef NODEV
	major_t major = NODEV;
#else
	major_t major = 0;
#endif
	if ((cdev = sdev_get(emajor))) {
		printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
		major = cdev->d_modid;
		printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
		sdev_put(cdev);
	}
	return (major);
}

EXPORT_SYMBOL(etoimajor);	/* uw7ddi.h */
int itoemajor(major_t imajor, int prevemaj)
{
	struct cdevsw *cdev;
	if ((cdev = cdrv_get(imajor)) && cdev->d_majors.next && !list_empty(&cdev->d_majors)) {
		struct list_head *pos;
#ifdef NODEV
		int found_previous = (prevemaj == NODEV) ? 1 : 0;
#else
		int found_previous = (prevemaj == 0) ? 1 : 0;
#endif
		printd(("%s: %s: got driver\n", __FUNCTION__, cdev->d_name));
		list_for_each(pos, &cdev->d_majors) {
			struct devnode *cmaj = list_entry(pos, struct devnode, n_list);
			if (found_previous)
				return (cmaj->n_major);
			if (prevemaj == cmaj->n_major)
				found_previous = 1;
		}
	}
#ifdef NODEV
	return (NODEV);
#else
	return (0);
#endif
}

EXPORT_SYMBOL(itoemajor);	/* uw7ddi.h */


//__SVR4_EXTERN_INLINE pl_t LOCK(lock_t * lockp, pl_t pl);
//EXPORT_SYMBOL(LOCK);          /* svr4ddi.h */
__SVR4_EXTERN_INLINE lock_t *LOCK_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop,
					int flag);
EXPORT_SYMBOL(LOCK_ALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void LOCK_DEALLOC(lock_t * lockp);
EXPORT_SYMBOL(LOCK_DEALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE pl_t TRYLOCK(lock_t * lockp, pl_t pl);
EXPORT_SYMBOL(TRYLOCK);		/* svr4ddi.h */
__SVR4_EXTERN_INLINE void UNLOCK(lock_t * lockp, pl_t pl);
EXPORT_SYMBOL(UNLOCK);		/* svr4ddi.h */
__SVR4_EXTERN_INLINE int LOCK_OWNED(lock_t * lockp);
EXPORT_SYMBOL(LOCK_OWNED);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE rwlock_t *RW_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop, int flag);
EXPORT_SYMBOL(RW_ALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void RW_DEALLOC(rwlock_t *lockp);
EXPORT_SYMBOL(RW_DEALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE pl_t RW_RDLOCK(rwlock_t *lockp, pl_t pl);
EXPORT_SYMBOL(RW_RDLOCK);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE pl_t RW_TRYRDLOCK(rwlock_t *lockp, pl_t pl);
EXPORT_SYMBOL(RW_TRYRDLOCK);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE pl_t RW_TRYWRLOCK(rwlock_t *lockp, pl_t pl);
EXPORT_SYMBOL(RW_TRYWRLOCK);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void RW_UNLOCK(rwlock_t *lockp, pl_t pl);
EXPORT_SYMBOL(RW_UNLOCK);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE pl_t RW_WRLOCK(rwlock_t *, pl_t pl);
EXPORT_SYMBOL(RW_WRLOCK);	/* svr4ddi.h */

__SVR4_EXTERN_INLINE sleep_t *SLEEP_ALLOC(int arg, lkinfo_t * lkinfop, int flag);
EXPORT_SYMBOL(SLEEP_ALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void SLEEP_DEALLOC(sleep_t * lockp);
EXPORT_SYMBOL(SLEEP_DEALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_LOCKAVAIL(sleep_t * lockp);
EXPORT_SYMBOL(SLEEP_LOCKAVAIL);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void SLEEP_LOCK(sleep_t * lockp, int priority);
EXPORT_SYMBOL(SLEEP_LOCK);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_LOCKOWNED(sleep_t * lockp);
EXPORT_SYMBOL(SLEEP_LOCKOWNED);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_LOCK_SIG(sleep_t * lockp, int priority);
EXPORT_SYMBOL(SLEEP_LOCK_SIG);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_TRYLOCK(sleep_t * lockp);
EXPORT_SYMBOL(SLEEP_TRYLOCK);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void SLEEP_UNLOCK(sleep_t * lockp);
EXPORT_SYMBOL(SLEEP_UNLOCK);	/* svr4ddi.h */

__SVR4_EXTERN_INLINE sv_t *SV_ALLOC(int flag);
EXPORT_SYMBOL(SV_ALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void SV_BROADCAST(sv_t * svp, int flags);
EXPORT_SYMBOL(SV_BROADCAST);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void SV_DEALLOC(sv_t * svp);
EXPORT_SYMBOL(SV_DEALLOC);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void SV_SIGNAL(sv_t * svp);
EXPORT_SYMBOL(SV_SIGNAL);	/* svr4ddi.h */
__SVR4_EXTERN_INLINE void SV_WAIT(sv_t * svp, int priority, lock_t * lkp);
EXPORT_SYMBOL(SV_WAIT);		/* svr4ddi.h */
__SVR4_EXTERN_INLINE int SV_WAIT_SIG(sv_t * svp, int priority, lock_t * lkp);
EXPORT_SYMBOL(SV_WAIT_SIG);	/* svr4ddi.h */

int ts_kmdpris[] = {
	60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
	90, 91, 92, 93, 94, 95, 96, 97, 98, 99
};

int ts_maxkmdpri = 39;

#define PSWP	     0
#define PMEM	     0
#define PINOD	    10
#define PRIBIO	    20
#define PZERO	    25
#define PPIPE	    26
#define PVFS	    27
#define TTIPRI	    28
#define TTOPRIO	    29
#define PWAIT	    30
#define PSLEP	    39

#define PCATCH	    0x8000
#define PNOSTOP	    0x4000


int sleep(caddr_t event, pl_t pl)
{
	return 1;
}
EXPORT_SYMBOL(sleep);		/* svr4ddi.h */

void wakeup(caddr_t event)
{
	return;
}
EXPORT_SYMBOL(wakeup);		/* svr4ddi.h */

#ifdef CONFIG_STREAMS_COMPAT_SVR4_MODULE
static
#endif
int __init svr4comp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_SVR4_MODULE
	printk(KERN_INFO SVR4COMP_BANNER);
#else
	printk(KERN_INFO SVR4COMP_SPLASH);
#endif
	return (0);
}
#ifdef CONFIG_STREAMS_COMPAT_SVR4_MODULE
static
#endif
void __exit svr4comp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_SVR4_MODULE
module_init(svr4comp_init);
module_exit(svr4comp_exit);
#endif
