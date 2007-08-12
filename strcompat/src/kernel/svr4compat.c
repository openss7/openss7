/*****************************************************************************

 @(#) $RCSfile: svr4compat.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/12 15:51:19 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/12 15:51:19 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: svr4compat.c,v $
 Revision 0.9.2.38  2007/08/12 15:51:19  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.37  2007/07/14 01:35:42  brian
 - make license explicit, add documentation

 Revision 0.9.2.36  2007/03/30 11:59:24  brian
 - heavy rework of MP syncrhonization

 Revision 0.9.2.35  2007/03/25 03:15:20  brian
 - somewhat more workable RW_UNLOCK

 Revision 0.9.2.34  2007/03/02 10:04:08  brian
 - updates to common build process and versions for all exported symbols

 Revision 0.9.2.33  2006/11/03 10:39:28  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ident "@(#) $RCSfile: svr4compat.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/12 15:51:19 $"

static char const ident[] =
    "$RCSfile: svr4compat.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/12 15:51:19 $";

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

#define __SVR4_EXTERN_INLINE __inline__ streamscall __unlikely
#define __SVR4_EXTERN streamscall

#define _SVR4_SOURCE

#include "sys/os7/compat.h"

#define SVR4COMP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SVR4COMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define SVR4COMP_REVISION	"LfS $RCSfile: svr4compat.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/12 15:51:19 $"
#define SVR4COMP_DEVICE		"UNIX(R) SVR 4.2 MP Compatibility"
#define SVR4COMP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SVR4COMP_LICENSE	"GPL v2"
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

#if defined CONFIG_STREAMS_NOIRQ || defined _TEST

#define spin_lock_str(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)
#define write_lock_str(__lkp, __flags) \
	do { (void)__flags; write_lock_bh(__lkp); } while (0)
#define write_unlock_str(__lkp, __flags) \
	do { (void)__flags; write_unlock_bh(__lkp); } while (0)
#define read_lock_str(__lkp, __flags) \
	do { (void)__flags; read_lock_bh(__lkp); } while (0)
#define read_unlock_str(__lkp, __flags) \
	do { (void)__flags; read_unlock_bh(__lkp); } while (0)
#define local_save_str(__flags) \
	do { (void)__flags; local_bh_disable(); } while (0)
#define local_restore_str(__flags) \
	do { (void)__flags; local_bh_enable(); } while (0)

#else

#define spin_lock_str(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)
#define write_lock_str(__lkp, __flags) \
	write_lock_irqsave(__lkp, __flags)
#define write_unlock_str(__lkp, __flags) \
	write_unlock_irqrestore(__lkp, __flags)
#define read_lock_str(__lkp, __flags) \
	read_lock_irqsave(__lkp, __flags)
#define read_unlock_str(__lkp, __flags) \
	read_unlock_irqrestore(__lkp, __flags)
#define local_save_str(__flags) \
	local_irq_save(__flags)
#define local_restore_str(__flags) \
	local_irq_restore(__flags)

#endif

#undef MPSTR_QLOCK
__SVR4_EXTERN pl_t
MPSTR_QLOCK(queue_t *q)
{
#ifdef LIS
	lis_flags_t flags;

	lis_rw_write_lock_irqsave(&q->q_isr_lock, &flags);
	return (flags);
#endif
#ifdef LFS
#if 0
	if (q->q_klock.kl_owner == current)
		q->q_klock.kl_nest++;
	else {
		pl_t pl;

		pl = splstr();
		write_lock(&q->q_klock.kl_lock);
		q->q_klock.kl_isrflags = pl;
		q->q_klock.kl_owner = current;
		q->q_klock.kl_nest = 0;
	}
	return (q->q_klock.kl_isrflags);
#else
	pl_t pl = splstr();

	write_lock(&q->q_lock);
	return (pl);
#endif
#endif
}

EXPORT_SYMBOL(MPSTR_QLOCK);	/* svr4/ddi.h */

#undef MPSTR_QRELE
__SVR4_EXTERN void
MPSTR_QRELE(queue_t *q, pl_t pl)
{
#ifdef LIS
	lis_flags_t flags = pl;

	lis_rw_write_unlock_irqrestore(&q->q_isr_lock, &flags);
	return;
#endif
#ifdef LFS
#if 0
	if (q->q_klock.kl_nest > 0)
		q->q_klock.kl_nest--;
	else {
		unsigned long flags = q->q_klock.kl_isrflags;

		q->q_klock.kl_owner = NULL;
		q->q_klock.kl_nest = 0;
		if (waitqueue_active(&q->q_klock.kl_waitq))
			wake_up(&q->q_klock.kl_waitq);
		write_unlock(&q->q_klock.kl_lock);
		local_restore_str(flags);
	}
	return;
#else
	write_unlock(&q->q_lock);
	splx(pl);
#endif
#endif
}

EXPORT_SYMBOL(MPSTR_QRELE);	/* svr4/ddi.h */

#undef MPSTR_STPLOCK
__SVR4_EXTERN pl_t
MPSTR_STPLOCK(struct stdata * sd)
{
#ifdef LIS
	lis_flags_t flags;

	lis_spin_lock_irqsave(&sd->sd_lock, &flags);
	return (flags);
#endif
#ifdef LFS
#if 0
	if (sd->sd_klock.kl_owner == current)
		sd->sd_klock.kl_nest++;
	else {
		unsigned long flags;

		local_save_str(flags);
		write_lock(&sd->sd_klock.kl_lock);
		sd->sd_klock.kl_isrflags = flags;
		sd->sd_klock.kl_owner = current;
		sd->sd_klock.kl_nest = 0;
	}
	return (sd->sd_klock.kl_isrflags);
#else
	pl_t pl = splstr();

	write_lock(&sd->sd_lock);
	return (pl);
#endif
#endif
}

EXPORT_SYMBOL(MPSTR_STPLOCK);	/* svr4/ddi.h */

#undef MPSTR_STPRELE
__SVR4_EXTERN void
MPSTR_STPRELE(struct stdata *sd, pl_t pl)
{
#ifdef LIS
	lis_flags_t flags = pl;

	lis_spin_unlock_irqrestore(&sd->sd_lock, &flags);
	return;
#endif
#ifdef LFS
#if 0
	if (sd->sd_klock.kl_nest > 0)
		sd->sd_klock.kl_nest--;
	else {
		unsigned long flags = sd->sd_klock.kl_isrflags;

		sd->sd_klock.kl_owner = NULL;
		sd->sd_klock.kl_nest = 0;
		if (waitqueue_active(&sd->sd_klock.kl_waitq))
			wake_up(&sd->sd_klock.kl_waitq);
		write_unlock(&sd->sd_klock.kl_lock);
		local_restore_str(flags);
	}
#else
	write_unlock(&sd->sd_lock);
	splx(pl);
#endif
#endif
}

EXPORT_SYMBOL(MPSTR_STPRELE);	/* svr4/ddi.h */

#ifdef LFS
__SVR4_EXTERN_INLINE toid_t dtimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl,
				     processorid_t processor);
EXPORT_SYMBOL(dtimeout);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE toid_t itimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl);

EXPORT_SYMBOL(itimeout);	/* svr4/ddi.h */
#endif

#if defined CONFIG_STREAMS_NOIRQ || defined _TEST

#define pl_base	    0
#define pl_atomic   2
#define pl_bh	    3
#define pl_irq	    6

#else				/* defined CONFIG_STREAMS_NOIRQ || defined _TEST */

#define pl_base	    0
#define pl_atomic   2
#define pl_bh	    3
#define pl_irq	    4

#endif				/* defined CONFIG_STREAMS_NOIRQ || defined _TEST */

/**
 * slp: - raise priority level
 * @level: level to raise
 *
 * spl() can only be used to raise the priority level.
 * splx() only be used to reduce (restore) the priority level.
 */
__SVR4_EXTERN pl_t
spl(const pl_t level)
{
	if (level > 7 || level < 0) {
		swerr();
		return invpl;
	}
	if (in_irq())
		return (pl_irq);
	if (in_interrupt()) {
		if (level >= pl_irq)
			local_irq_disable();
		return (pl_bh);
	}
#if defined HAVE_KFUNC_IN_ATOMIC
	if (in_atomic()) {
		if (level >= pl_irq)
			local_irq_disable();
		else if (level >= pl_bh)
			local_bh_disable();
		return (pl_atomic);
	}
#endif				/* defined HAVE_KFUNC_IN_ATOMIC */
	if (level >= pl_irq)
		local_irq_disable();
	else if (level >= pl_bh)
		local_bh_disable();
	else if (level >= pl_atomic)
		preempt_disable();
	return (pl_base);
}

EXPORT_SYMBOL(spl);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl0(void)
{
	return spl(0);
}

EXPORT_SYMBOL(spl0);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl1(void)
{
	return spl(1);
}

EXPORT_SYMBOL(spl1);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl2(void)
{
	return spl(2);
}

EXPORT_SYMBOL(spl2);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl3(void)
{
	return spl(3);
}

EXPORT_SYMBOL(spl3);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl4(void)
{
	return spl(4);
}

EXPORT_SYMBOL(spl4);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl5(void)
{
	return spl(5);
}

EXPORT_SYMBOL(spl5);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl6(void)
{
	return spl(6);
}

EXPORT_SYMBOL(spl6);		/* svr4/ddi.h */

__SVR4_EXTERN pl_t
spl7(void)
{
	return spl(7);
}

EXPORT_SYMBOL(spl7);		/* svr4/ddi.h */

/**
 * slpx: - restore priority level
 * @level: level to restore
 *
 * splx() only be used to reduce (restore) the priority level.
 * spl() can only be used to raise the priority level.
 */
__SVR4_EXTERN void
splx(const pl_t level)
{
	if (likely(level >= 0)) {
		if (likely(level < pl_irq) && likely(in_irq()))
			local_irq_enable();
		else if (likely(level < pl_bh) && likely(in_interrupt()))
			local_bh_enable();
#if defined HAVE_KFUNC_IN_ATOMIC
		else if (likely(level < pl_atomic) && likely(in_atomic()))
			preempt_enable();
#endif				/* defined HAVE_KFUNC_IN_ATOMIC */
	} else
		swerr();
	return;
}

EXPORT_SYMBOL(splx);		/* svr4/ddi.h */

__SVR4_EXTERN_INLINE major_t getemajor(dev_t dev);

EXPORT_SYMBOL(getemajor);	/* uw7/ddi.h */
__SVR4_EXTERN_INLINE minor_t geteminor(dev_t dev);

EXPORT_SYMBOL(geteminor);	/* uw7/ddi.h */

#ifndef NODEV
#define NODEV 0
#endif
__SVR4_EXTERN int
etoimajor(major_t emajor)
{
	major_t major = NODEV;

#ifdef LFS
	struct cdevsw *cdev;

	if ((cdev = sdev_get(emajor))) {
		printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
		major = cdev->d_modid;
		printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
		sdev_put(cdev);
	}
#endif
	return (major);
}

EXPORT_SYMBOL(etoimajor);	/* uw7/ddi.h */

int
itoemajor(major_t imajor, int prevemaj)
{
#ifdef LFS
	struct cdevsw *cdev;

	if ((cdev = cdrv_get(imajor)) && cdev->d_majors.next && !list_empty(&cdev->d_majors)) {
		struct list_head *pos;
		int found_previous = (prevemaj == NODEV) ? 1 : 0;

		printd(("%s: %s: got driver\n", __FUNCTION__, cdev->d_name));
		list_for_each(pos, &cdev->d_majors) {
			struct devnode *cmaj = list_entry(pos, struct devnode, n_list);

			if (found_previous)
				return (cmaj->n_major);
			if (prevemaj == cmaj->n_major)
				found_previous = 1;
		}
	}
#endif
	return (NODEV);
}

EXPORT_SYMBOL(itoemajor);	/* uw7/ddi.h */

//__SVR4_EXTERN_INLINE pl_t LOCK(lock_t * lockp, pl_t pl);
//EXPORT_SYMBOL(LOCK);          /* svr4/ddi.h */
__SVR4_EXTERN_INLINE lock_t *LOCK_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop,
					int flag);
EXPORT_SYMBOL(LOCK_ALLOC);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void LOCK_DEALLOC(lock_t * lockp);

EXPORT_SYMBOL(LOCK_DEALLOC);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE pl_t TRYLOCK(lock_t * lockp, pl_t pl);

EXPORT_SYMBOL(TRYLOCK);		/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void UNLOCK(lock_t * lockp, pl_t pl);

EXPORT_SYMBOL(UNLOCK);		/* svr4/ddi.h */
__SVR4_EXTERN_INLINE int LOCK_OWNED(lock_t * lockp);

EXPORT_SYMBOL(LOCK_OWNED);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE rwlock_t *RW_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop,
					int flag);
EXPORT_SYMBOL(RW_ALLOC);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void RW_DEALLOC(rwlock_t *lockp);

EXPORT_SYMBOL(RW_DEALLOC);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE pl_t RW_RDLOCK(rwlock_t *lockp, pl_t pl);

EXPORT_SYMBOL(RW_RDLOCK);	/* svr4/ddi.h */
pl_t
RW_TRYRDLOCK(rwlock_t *lockp, pl_t pl)
{
	pl_t old_pl = spl(pl);

#if defined CONFIG_SMP && (defined HAVE_KFUNC_READ_TRYLOCK || defined HAVE_KMACRO_READ_TRYLOCK)
	if (read_trylock(lockp))
		return (old_pl);
#else
#if defined CONFIG_SMP && (defined HAVE_KFUNC_WRITE_TRYLOCK || defined HAVE_KMACRO_WRITE_TRYLOCK)
	if (write_trylock(lockp))
		return (old_pl);
#else
#if defined CONFIG_SMP
	/* this will jam up sometimes */
	if (!spin_is_locked(lockp)) {
#endif
		read_lock(lockp);
		return (old_pl);
#if defined CONFIG_SMP
	}
#endif
#endif
#endif
	splx(old_pl);
	return (invpl);
}

EXPORT_SYMBOL(RW_TRYRDLOCK);	/* svr4/ddi.h */
__SVR4_EXTERN pl_t
RW_TRYWRLOCK(rwlock_t *lockp, pl_t pl)
{
	pl_t old_pl = spl(pl);

#if defined CONFIG_SMP && (defined HAVE_KFUNC_WRITE_TRYLOCK || defined HAVE_KMACRO_WRITE_TRYLOCK)
	if (write_trylock(lockp))
		return (old_pl);
#else
#if defined CONFIG_SMP
	/* this will jam up sometimes */
	if (!spin_is_locked(lockp)) {
#endif
		write_lock(lockp);
		return (old_pl);
#if defined CONFIG_SMP
	}
#endif
#endif
	splx(old_pl);
	return (invpl);
}

EXPORT_SYMBOL(RW_TRYWRLOCK);	/* svr4/ddi.h */
__SVR4_EXTERN void
RW_UNLOCK(rwlock_t *lockp, pl_t pl)
{
#if defined CONFIG_SMP && (defined HAVE_KFUNC_READ_TRYLOCK || defined HAVE_KMACRO_READ_TRYLOCK)
	if (read_trylock(lockp)) {
		read_unlock(lockp);
		read_unlock(lockp);
	} else {
		write_unlock(lockp);
	}
#else
	/* this is not really workable */
	read_unlock(lockp);
#endif
	splx(pl);
}

EXPORT_SYMBOL(RW_UNLOCK);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE pl_t RW_WRLOCK(rwlock_t *, pl_t pl);

EXPORT_SYMBOL(RW_WRLOCK);	/* svr4/ddi.h */

__SVR4_EXTERN_INLINE sleep_t *SLEEP_ALLOC(int arg, lkinfo_t * lkinfop, int flag);

EXPORT_SYMBOL(SLEEP_ALLOC);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void SLEEP_DEALLOC(sleep_t * lockp);

EXPORT_SYMBOL(SLEEP_DEALLOC);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_LOCKAVAIL(sleep_t * lockp);

EXPORT_SYMBOL(SLEEP_LOCKAVAIL);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void SLEEP_LOCK(sleep_t * lockp, int priority);

EXPORT_SYMBOL(SLEEP_LOCK);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_LOCKOWNED(sleep_t * lockp);

EXPORT_SYMBOL(SLEEP_LOCKOWNED);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_LOCK_SIG(sleep_t * lockp, int priority);

EXPORT_SYMBOL(SLEEP_LOCK_SIG);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE int SLEEP_TRYLOCK(sleep_t * lockp);

EXPORT_SYMBOL(SLEEP_TRYLOCK);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void SLEEP_UNLOCK(sleep_t * lockp);

EXPORT_SYMBOL(SLEEP_UNLOCK);	/* svr4/ddi.h */

__SVR4_EXTERN_INLINE sv_t *SV_ALLOC(int flag);

EXPORT_SYMBOL(SV_ALLOC);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void SV_BROADCAST(sv_t * svp, int flags);

EXPORT_SYMBOL(SV_BROADCAST);	/* svr4/ddi.h */
__SVR4_EXTERN_INLINE void SV_DEALLOC(sv_t * svp);

EXPORT_SYMBOL(SV_DEALLOC);	/* svr4/ddi.h */
#if ! ( defined HAVE___WAKE_UP_SYNC_ADDR || defined HAVE___WAKE_UP_SYNC_EXPORT )
#undef	__wake_up_sync
#define __wake_up_sync __wake_up
#endif
__SVR4_EXTERN void
SV_SIGNAL(sv_t * svp)
{
#ifdef HAVE___WAKE_UP_SYNC_ADDR
#undef	__wake_up_sync
	typeof(&__wake_up_sync) ___wake_up_sync =
	    (typeof(___wake_up_sync)) HAVE___WAKE_UP_SYNC_ADDR;
#define	__wake_up_sync ___wake_up_sync
#endif
	svp->sv_condv = 1;
	wake_up_interruptible_sync(&svp->sv_waitq);
}

EXPORT_SYMBOL(SV_SIGNAL);	/* svr4/ddi.h */

#ifdef LFS
#undef schedule
#define schedule() streams_schedule()
#endif

__SVR4_EXTERN void
SV_WAIT(sv_t * svp, int priority, lock_t * lkp)
{
	pl_t pl = plbase;

	DECLARE_WAITQUEUE(wait, current);
	set_current_state(TASK_UNINTERRUPTIBLE);
	add_wait_queue(&svp->sv_waitq, &wait);
	for (;;) {
		if (!svp->sv_condv--)
			break;
		svp->sv_condv++;
		UNLOCK(lkp, pl);
		schedule();
		pl = LOCK(lkp, plstr);
		if (lkp)
			spin_lock(lkp);
		set_current_state(TASK_UNINTERRUPTIBLE);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&svp->sv_waitq, &wait);
	UNLOCK(lkp, priority);
}

EXPORT_SYMBOL(SV_WAIT);		/* svr4/ddi.h */

__SVR4_EXTERN int
SV_WAIT_SIG(sv_t * svp, int priority, lock_t * lkp)
{
	int signal = 0;
	pl_t pl = plbase;

	DECLARE_WAITQUEUE(wait, current);
	set_current_state(TASK_INTERRUPTIBLE);
	add_wait_queue(&svp->sv_waitq, &wait);
	for (;;) {
		signal = 1;
		if (signal_pending(current) || --svp->sv_condv == 0)
			break;
		svp->sv_condv++;
		UNLOCK(lkp, pl);
		schedule();
		pl = LOCK(lkp, plstr);
		set_current_state(TASK_INTERRUPTIBLE);
		signal = 0;
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&svp->sv_waitq, &wait);
	UNLOCK(lkp, priority);
	splx(priority);
	return signal;
}

EXPORT_SYMBOL(SV_WAIT_SIG);	/* svr4/ddi.h */

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

__SVR4_EXTERN int
sleep(caddr_t event, pl_t pl)
{
	return 1;
}

EXPORT_SYMBOL(sleep);		/* svr4/ddi.h */

__SVR4_EXTERN void
wakeup(caddr_t event)
{
	return;
}

EXPORT_SYMBOL(wakeup);		/* svr4/ddi.h */

#ifdef CONFIG_STREAMS_COMPAT_SVR4_MODULE
static
#endif
int __init
svr4comp_init(void)
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
void __exit
svr4comp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_SVR4_MODULE
module_init(svr4comp_init);
module_exit(svr4comp_exit);
#endif
