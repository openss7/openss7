/*****************************************************************************

 @(#) $Id: svr4ddi.h,v 0.9.2.7 2004/11/08 19:56:36 brian Exp $

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

 Last Modified $Date: 2004/11/08 19:56:36 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_SVR4DDI_H__
#define __SYS_SVR4DDI_H__

#ident "@(#) $RCSfile: svr4ddi.h,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/11/08 19:56:36 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __SVR4_EXTERN_INLINE
#define __SVR4_EXTERN_INLINE extern __inline__
#endif				/* __SVR4_EXTERN_INLINE */

#include <sys/strconf.h>

#ifndef _SVR4_SOURCE
#warning "_SVR4_SOURCE not defined but svr4ddi.h,v included"
#endif

#if defined(CONFIG_STREAMS_COMPAT_SVR4) || defined(CONFIG_STREAMS_COMPAT_SVR4_MODULE)

#include <sys/kmem.h>		/* for kmem_alloc/free */

typedef spinlock_t lock_t;

typedef long pl_t;

typedef struct lkinfo {
	char *lk_name;
} lkinfo_t;

typedef struct sv {
	wait_queue_head_t sv_waitq;
	int sv_condv;
} sv_t;

#define invpl	    -1
#define plbase	    0
#define pltimeout   3
#define pltty	    4
#define plstr	    5
#define pldisk	    6
#define plhi	    7

extern void splx(const pl_t level);
extern pl_t spl(const pl_t level);

extern pl_t spl0(void);
extern pl_t spl1(void);
extern pl_t spl2(void);
extern pl_t spl3(void);
extern pl_t spl4(void);
extern pl_t spl5(void);
extern pl_t spl6(void);
extern pl_t spl7(void);

#define splbase	    spl0
#define spltimeout  spl3
#define spltty	    spl4
#define splstr	    spl5
#define spldisk	    spl6
#define splhi	    spl7

#ifdef LOCK_ALLOC
#undef LOCK_ALLOC
#endif
__SVR4_EXTERN_INLINE lock_t *LOCK_ALLOC(unsigned char hierarchy, pl_t min_pl, lkinfo_t * lkinfop,
					int flag)
{
	lock_t *lockp;
	if ((lockp = kmem_alloc(sizeof(*lockp), flag)))
		spin_lock_init(lockp);
	return (lockp);
}

#ifdef LOCK_DEALLOC
#undef LOCK_DEALLOC
#endif
__SVR4_EXTERN_INLINE void LOCK_DEALLOC(lock_t * lockp)
{
	kmem_free(lockp, sizeof(*lockp));
}

#ifdef TRYLOCK
#undef TRYLOCK
#endif
__SVR4_EXTERN_INLINE pl_t TRYLOCK(lock_t * lockp, pl_t pl)
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
__SVR4_EXTERN_INLINE void UNLOCK(lock_t * lockp, pl_t pl)
{
	spin_unlock(lockp);
	splx(pl);
}

#ifdef LOCK
#undef LOCK
#endif
__SVR4_EXTERN_INLINE pl_t LOCK(lock_t * lockp, pl_t pl)
{
	pl_t old_pl = spl(pl);
	spin_lock(lockp);
	return (old_pl);
}

__SVR4_EXTERN_INLINE sv_t *SV_ALLOC(int flag)
{
	sv_t *svp;
	if ((svp = kmem_alloc(sizeof(*svp), flag))) {
		init_waitqueue_head(&svp->sv_waitq);
		svp->sv_condv = 1;
	}
	return (svp);
}
__SVR4_EXTERN_INLINE void SV_BROADCAST(sv_t * svp, int flags)
{
	svp->sv_condv = 1;
	wake_up_interruptible_all(&svp->sv_waitq);
}
__SVR4_EXTERN_INLINE void SV_DEALLOC(sv_t * svp)
{
	kmem_free(svp, sizeof(*svp));
}
#ifndef HAVE___WAKE_UP_SYNC_ADDR
#undef	__wake_up_sync
#define __wake_up_sync __wake_up
#endif
__SVR4_EXTERN_INLINE void SV_SIGNAL(sv_t * svp)
{
#ifdef HAVE___WAKE_UP_SYNC_ADDR
#undef	__wake_up_sync
	typeof(&__wake_up_sync) ___wake_up_sync = (typeof(___wake_up_sync)) HAVE___WAKE_UP_SYNC_ADDR;
#define	__wake_up_sync ___wake_up_sync
#endif
	svp->sv_condv = 1;
	wake_up_interruptible_sync(&svp->sv_waitq);
}
__SVR4_EXTERN_INLINE void SV_WAIT(sv_t * svp, int priority, lock_t * lkp)
{
	DECLARE_WAITQUEUE(wait, current);
	add_wait_queue(&svp->sv_waitq, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (!svp->sv_condv--)
			break;
		svp->sv_condv++;
		if (lkp)
			spin_unlock(lkp);
		schedule();
		if (lkp)
			spin_lock(lkp);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&svp->sv_waitq, &wait);
}
__SVR4_EXTERN_INLINE int SV_WAIT_SIG(sv_t * svp, int priority, lock_t * lkp)
{
	int signal = 0;
	DECLARE_WAITQUEUE(wait, current);
	add_wait_queue(&svp->sv_waitq, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		signal = 1;
		if (signal_pending(current) || --svp->sv_condv == 0)
			break;
		svp->sv_condv++;
		if (lkp)
			spin_unlock(lkp);
		schedule();
		if (lkp)
			spin_lock(lkp);
		signal = 0;
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&svp->sv_waitq, &wait);
	return signal;
}

#ifdef DEBUG
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(__exp) \
do { if (!(__exp)) { panic("assertion failed: " #__exp ", file: " __FILE__ ", line: %d\n", __LINE__); } } while(0)
#endif

#elif defined(_SVR4_SOURCE)
#warning "_SVR4_SOURCE defined but not CONFIG_STREAMS_COMPAT_SVR4"
#endif				/* CONFIG_STREAMS_COMPAT_SVR4 */

#endif				/* __SYS_SVR4DDI_H__ */
