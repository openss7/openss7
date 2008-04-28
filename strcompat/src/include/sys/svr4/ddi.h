/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.30 2008-04-28 16:47:12 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-28 16:47:12 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.30  2008-04-28 16:47:12  brian
 - updates for release

 Revision 0.9.2.29  2007/08/12 15:51:15  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.28  2007/03/30 11:59:22  brian
 - heavy rework of MP syncrhonization

 Revision 0.9.2.27  2007/03/25 03:15:19  brian
 - somewhat more workable RW_UNLOCK

 Revision 0.9.2.26  2006/12/08 05:08:22  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.25  2006/11/03 10:39:25  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_SVR4_DDI_H__
#define __SYS_SVR4_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.30 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __SVR4_EXTERN_INLINE
#define __SVR4_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __SVR4_EXTERN_INLINE */

#ifndef __SVR4_EXTERN
#define __SVR4_EXTERN extern streamscall
#endif

#ifndef _SVR4_SOURCE
#warning "_SVR4_SOURCE not defined but SVR4 ddi.h included"
#define _SVR4_SOURCE 1
#endif

#include <sys/kmem.h>		/* for kmem_alloc/free */

#ifndef lock_t
typedef spinlock_t lock_t;

#define lock_t lock_t
#endif

#ifndef pl_t
typedef long pl_t;

#define pl_t pl_t
#endif

typedef struct lkinfo {
	char *lk_name;
} lkinfo_t;

typedef struct sv {
	wait_queue_head_t sv_waitq;
	int sv_condv;
} sv_t;

#define invpl		-1
#define plbase		 0
#define pltimeout	 3
#define pltty		 4
#define plstr		 5
#define pldisk		 6
#define plhi		 7

__SVR4_EXTERN void splx(const pl_t level);
__SVR4_EXTERN pl_t spl(const pl_t level);

__SVR4_EXTERN pl_t spl0(void);
__SVR4_EXTERN pl_t spl1(void);
__SVR4_EXTERN pl_t spl2(void);
__SVR4_EXTERN pl_t spl3(void);
__SVR4_EXTERN pl_t spl4(void);
__SVR4_EXTERN pl_t spl5(void);
__SVR4_EXTERN pl_t spl6(void);
__SVR4_EXTERN pl_t spl7(void);

#define splbase()	spl0()
#define spltimeout()	spl3()
#define spltty()	spl4()
#define splstr()	spl5()
#define spldisk()	spl6()
#define splhi()		spl7()

typedef int processorid_t;

#ifdef LFS
__SVR4_EXTERN_INLINE toid_t
dtimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl, processorid_t processor)
{
	return __timeout(NULL, timo_fcn, arg, ticks, pl, processor);
}
__SVR4_EXTERN_INLINE toid_t
itimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl)
{
	return __timeout(NULL, timo_fcn, arg, ticks, pl, smp_processor_id());
}
#endif

__SVR4_EXTERN_INLINE major_t
getemajor(dev_t dev)
{
	return (getmajor(dev) + MAJOR(getminor(dev)));
}
__SVR4_EXTERN_INLINE minor_t
geteminor(dev_t dev)
{
	return (MINOR(getminor(dev)));
}
__SVR4_EXTERN int etoimajor(major_t emajor);
__SVR4_EXTERN int itoemajor(major_t imajor, int prevemaj);

#ifdef LOCK_ALLOC
#undef LOCK_ALLOC
#endif
__SVR4_EXTERN_INLINE lock_t *
LOCK_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop, int flag)
{
	lock_t *lockp;

	if ((lockp = kmem_alloc(sizeof(*lockp), flag)))
		spin_lock_init(lockp);
	return (lockp);
}

#ifdef LOCK_DEALLOC
#undef LOCK_DEALLOC
#endif
__SVR4_EXTERN_INLINE void
LOCK_DEALLOC(lock_t * lockp)
{
	kmem_free(lockp, sizeof(*lockp));
}

#ifdef LOCK_OWNED
#undef LOCK_OWNED
#endif
__SVR4_EXTERN_INLINE int
LOCK_OWNED(lock_t * lockp)
{
	return (1);
}

#ifdef TRYLOCK
#undef TRYLOCK
#endif
__SVR4_EXTERN_INLINE pl_t
TRYLOCK(lock_t * lockp, pl_t pl)
{
	pl_t old_pl = spl(pl);

	if (spin_trylock(lockp))
		return (old_pl);
	splx(old_pl);
	return (invpl);
}

#ifdef UNLOCK
#undef UNLOCK
#endif
__SVR4_EXTERN_INLINE void
UNLOCK(lock_t * lockp, pl_t pl)
{
	spin_unlock(lockp);
	splx(pl);
}

#ifdef LOCK
#undef LOCK
#endif
__SVR4_EXTERN_INLINE pl_t
LOCK(lock_t * lockp, pl_t pl)
{
	pl_t old_pl = spl(pl);

	spin_lock(lockp);
	return (old_pl);
}

__SVR4_EXTERN_INLINE rwlock_t *
RW_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop, int flag)
{
	rwlock_t *lockp;

	if ((lockp = kmem_alloc(sizeof(*lockp), flag)))
		rwlock_init(lockp);
	return (lockp);
}
__SVR4_EXTERN_INLINE void
RW_DEALLOC(rwlock_t *lockp)
{
	kmem_free(lockp, sizeof(*lockp));
}

__SVR4_EXTERN_INLINE pl_t
RW_RDLOCK(rwlock_t *lockp, pl_t pl)
{
	pl_t old_pl = spl(pl);

	read_lock(lockp);
	return (old_pl);
}
__SVR4_EXTERN pl_t RW_TRYRDLOCK(rwlock_t *lockp, pl_t pl);
__SVR4_EXTERN pl_t RW_TRYWRLOCK(rwlock_t *lockp, pl_t pl);
__SVR4_EXTERN void RW_UNLOCK(rwlock_t *lockp, pl_t pl);

__SVR4_EXTERN_INLINE pl_t
RW_WRLOCK(rwlock_t *lockp, pl_t pl)
{
	pl_t old_pl = spl(pl);

	write_lock(lockp);
	return (old_pl);
}

typedef struct semaphore sleep_t;

__SVR4_EXTERN_INLINE sleep_t *
SLEEP_ALLOC(int arg, lkinfo_t * lkinfop, int flag)
{
	sleep_t *lockp;

	if ((lockp = kmem_alloc(sizeof(*lockp), flag)))
		init_MUTEX(lockp);
	return (lockp);
}
__SVR4_EXTERN_INLINE void
SLEEP_DEALLOC(sleep_t * lockp)
{
	kmem_free(lockp, sizeof(*lockp));
}
__SVR4_EXTERN_INLINE int
SLEEP_LOCKAVAIL(sleep_t * lockp)
{
	if (!down_trylock(lockp)) {
		up(lockp);
		return (1);
	}
	return (0);
}
__SVR4_EXTERN_INLINE void
SLEEP_LOCK(sleep_t * lockp, int priority)
{
	down(lockp);
}
__SVR4_EXTERN_INLINE int
SLEEP_LOCKOWNED(sleep_t * lockp)
{
	return (1);
}
__SVR4_EXTERN_INLINE int
SLEEP_LOCK_SIG(sleep_t * lockp, int priority)
{
	return down_interruptible(lockp);
}
__SVR4_EXTERN_INLINE int
SLEEP_TRYLOCK(sleep_t * lockp)
{
	return (down_trylock(lockp) == 0);
}
__SVR4_EXTERN_INLINE void
SLEEP_UNLOCK(sleep_t * lockp)
{
	up(lockp);
}

__SVR4_EXTERN_INLINE sv_t *
SV_ALLOC(int flag)
{
	sv_t *svp;

	if ((svp = kmem_alloc(sizeof(*svp), flag))) {
		init_waitqueue_head(&svp->sv_waitq);
		svp->sv_condv = 1;
	}
	return (svp);
}
__SVR4_EXTERN_INLINE void
SV_BROADCAST(sv_t * svp, int flags)
{
	svp->sv_condv = 1;
	wake_up_interruptible_all(&svp->sv_waitq);
}
__SVR4_EXTERN_INLINE void
SV_DEALLOC(sv_t * svp)
{
	kmem_free(svp, sizeof(*svp));
}
__SVR4_EXTERN void SV_SIGNAL(sv_t * svp);
__SVR4_EXTERN void SV_WAIT(sv_t * svp, int priority, lock_t * lkp);
__SVR4_EXTERN int SV_WAIT_SIG(sv_t * svp, int priority, lock_t * lkp);

__SVR4_EXTERN int sleep(caddr_t event, pl_t pl);
__SVR4_EXTERN void wakeup(caddr_t event);

#undef ASSERT
#define ASSERT(__assertion) assert((__assertion))

#endif				/* __SYS_SVR4_DDI_H__ */
