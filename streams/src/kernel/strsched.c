/*****************************************************************************

 @(#) strsched.c,v (0.9.2.27) 2003/10/28 08:00:05

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

#ident "@(#) strsched.c,v (0.9.2.27) 2003/10/28 08:00:05"

static char const ident[] = "strsched.c,v (0.9.2.27) 2003/10/28 08:00:05";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/compiler.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/locks.h>
#include <linux/delay.h>
#include <linux/sysctl.h>
#include <linux/file.h>
#include <linux/poll.h>
#include <linux/fs.h>
#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#endif
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif
#include <linux/major.h>
// #include <asm/atomic.h>

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>

#if defined(CONFIG_STREAMS_COMPAT_SVR4)||defined(CONFIG_STREAMS_COMPAT_SVR4_MODULE)
#define _SVR4_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_AIX)||defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)
#define _AIX_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_HPUX)||defined(CONFIG_STREAMS_COMPAT_HPUX_MODULE)
#define _HPUX_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_OSF)||defined(CONFIG_STREAMS_COMPAT_OSF_MODULE)
#define _OSF_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_SUN)||defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
#define _SUN_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_UW7)||defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)
#define _UW7_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_LIS)||defined(CONFIG_STREAMS_COMPAT_LIS_MODULE)
#define _LIS_SOURCE
#endif

#include <sys/ddi.h>

#include "strdebug.h"
#include "strhead.h"		/* for str_minfo */
#include "strsysctl.h"		/* for sysctl_str_ defs */
#include "strsched.h"		/* for in_stream */
#include "strutil.h"		/* for q locking and puts and gets */
#undef  __SCHED_EXTERN_INLINE
#define __SCHED_EXTERN_INLINE extern
#include "strsched.h"		/* verification of externs */

static struct strthread strthreads[NR_CPUS] ____cacheline_aligned;
struct strinfo Strinfo[DYN_SIZE] ____cacheline_aligned;

/* 
 *  -------------------------------------------------------------------------
 *
 *  MDBBLOCK ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void mdbblock_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct mdbblock *md = obj;
		bzero(md, sizeof(*md));
		atomic_set(&md->datablk.d_dblock.db_users, 0);
#ifdef CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &md->msgblk.m_next);
		INIT_LIST_HEAD((struct list_head *) &md->datablk.db_next);
#endif
	}
}

#ifdef CONFIG_STREAMS_DEBUG
/* use the current queue being processed by the thread as a guess as to which queue the thing
   (message block, bufcall callback, timout callback) belongs to, but, if we are at irq, then we
   will only used the supplied value, even if null. */
static queue_t *queue_guess(queue_t *q)
{
	queue_t *guess;
	if ((guess = q) == NULL && current_context() == CTX_STREAMS)
		guess = this_thread->currentq;
	return (qget(guess));
}
#endif

/**
 *  mdbblock_alloc - allocate a combined message/data block
 *  @q:the queue with which to associate the allocation for debugging
 *  @priority:the priority of the allocation
 *  @func:allocating function pointer
 *
 *  This is not exported, but is called by the streams D3DK functions
 *  allocb(), dupb(), eballoc() and pullupmsg() to allocate a combined
 *  message/data block.  Because mdbblock_free() does a fast free to the free
 *  list, we first check if a mdbblock is sitting on the per-cpu free list.
 *  If there is no memory block available then we take one from the memory
 *  cache.  Unfortunately we must shut out interrupts briefly otherwise an ISR
 *  running on the same processor freeing while we are allocating will hose
 *  the free list.
 *
 *  We reimplement the SVR3 priority scheme with a twist.  BPRI_HI is allowed
 *  to use the current free list for high priority tasks.  BPRI_MED is not
 *  allowed the free list but may grow the cache if necessary to get a block.
 *  BPRI_LO is only allowed to get a free block from the cache and is not
 *  allowed to grow the cache.
 *
 *  Return Values:This function will return NULL when there are no more
 *  blocks.
 *
 *  Because misbehaving STREAMS modules and drivers normaly leak message
 *  blocks at an amazing rate, we also return an allocation failure if the
 *  number of message blocks exceeds a tunable threshold.
 */
mblk_t *mdbblock_alloc(uint priority, void *func)
{
	struct strthread *t = this_thread;
	struct strinfo *sdi = &Strinfo[DYN_MDBBLOCK];
	mblk_t *mp = NULL;
	int slab_flags;
	unsigned long flags;
	if (atomic_read(&sdi->si_cnt) > sysctl_str_nstrmsgs)
		goto fail;
	switch (priority) {
	case BPRI_HI:
		local_irq_save(flags);
		if ((mp = xchg(&t->freemblk_head, NULL)))
			t->freemblk_head = xchg(&mp->b_next, NULL);
		local_irq_restore(flags);
		if (mp != NULL)
			return (mp);
	case BPRI_MED:
		slab_flags = SLAB_ATOMIC;
		break;
	default:
	case BPRI_LO:
		slab_flags = SLAB_ATOMIC | SLAB_NO_GROW;
		break;
	case BPRI_WAITOK:
		slab_flags = 0;
		break;
	}
	if ((mp = kmem_cache_alloc(sdi->si_cache, slab_flags))) {
#ifdef CONFIG_STREAMS_DEBUG
		struct strinfo *smi = &Strinfo[DYN_MSGBLOCK];
		struct mdbblock *md = (struct mdbblock *) mp;
		md->msgblk.m_func = func;
		md->msgblk.m_queue = queue_guess(NULL);
		write_lock_irqsave(&smi->si_rwlock, flags);
		list_add_tail((struct list_head *) &md->msgblk.m_next, &smi->si_head);
		list_add_tail((struct list_head *) &md->datablk.db_next, &sdi->si_head);
		write_unlock_irqrestore(&smi->si_rwlock, flags);
		atomic_inc(&smi->si_cnt);
		if (atomic_read(&smi->si_cnt) > smi->si_hwl)
			smi->si_hwl = atomic_read(&smi->si_cnt);
#endif
		atomic_inc(&sdi->si_cnt);
		if (atomic_read(&sdi->si_cnt) > sdi->si_hwl)
			sdi->si_hwl = atomic_read(&sdi->si_cnt);
	}
      fail:
	return (mp);
}

/**
 *  mdbblock_free - free a combined message/data block
 *  @mp:    the mdbblock to free
 *
 *  This is not exported but is called by the streams D3DK function freeb()
 *  when freeing a &struct mddbblock.  For speed, we just link the block into
 *  the per-cpu free list.  Other queue proceudres calling allocb() or
 *  esballoc() in the same runqueues() pass can use them without locking the
 *  memory caches.  At the end of the runqueues() pass, any remaining blocks
 *  will be freed back to the kmem cache.  Whenever we place the first block
 *  on the free list or whenever there are streams waiting on buffers, we
 *  raise the softirq to ensure that another runqueues() pass will occur.
 *
 *  This function uses the fact that it is using a per-cpu list to avoid
 *  locking.  It uses the atomic xchg() function to ensure the integrity of
 *  the list while interrupts are still enabled.
 *  
 *  To reduce latency on allocation to a minimum, we null the state of the
 *  blocks when they are placed to the free list rather than when they are
 *  allocated.
 */
void mdbblock_free(mblk_t *mp)
{
	struct strthread *t = this_thread;
	struct mdbblock *md = (typeof(md)) mp;
	/* don't zero hidden list structures */
	bzero(&md->datablk.d_dblock, sizeof(md->datablk.d_dblock));
	bzero(&md->msgblk.m_mblock, sizeof(md->msgblk.m_mblock));
	md->msgblk.m_func = NULL;
	md->msgblk.m_queue = NULL;
	bzero(md, sizeof(*md));	/* reset mdbblock */
	*xchg(&t->freemblk_tail, &mp->b_next) = mp;
	if (!test_and_set_bit(freeblks, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
	if (test_bit(strbcwait, &t->flags))
		if (!test_and_set_bit(strbcflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
	return;
}

/* 
 *  freeblocks() is invoked by runqueues() when there are blocks to free (i.e.
 *  the FREEBLKS flag is set on the thread).  It frees all blocks from the
 *  free list.  We first steal the entire list and then work on them one by
 *  one.
 */
static void freeblocks(struct strthread *t)
{
	register mblk_t *mp, *mp_next;
	unsigned long flags;
	clear_bit(freeblks, &t->flags);
	local_irq_save(flags);
	if ((mp_next = xchg(&t->freemblk_head, NULL))) {
		t->freemblk_tail = &t->freemblk_head;
		local_irq_restore(flags);
		while ((mp = mp_next)) {
			struct strinfo *sdi = &Strinfo[DYN_MDBBLOCK];
#ifdef CONFIG_STREAMS_DEBUG
			struct strinfo *smi = &Strinfo[DYN_MSGBLOCK];
			unsigned long flags;
			struct mdbblock *md = (struct mdbblock *) mp;
			write_lock_irqsave(&smi->si_rwlock, flags);
			list_del_init((struct list_head *) &md->msgblk.m_next);
			list_del_init((struct list_head *) &md->datablk.db_next);
			write_unlock_irqrestore(&smi->si_rwlock, flags);
			atomic_dec(&smi->si_cnt);
#endif
			mp_next = xchg(&mp->b_next, NULL);
			mp->b_prev = NULL;
			kmem_cache_free(sdi->si_cache, mp);
			atomic_dec(&sdi->si_cnt);
		}
	} else
		local_irq_restore(flags);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  QBAND ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void qbinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct qbinfo *qbi = obj;
		bzero(qbi, sizeof(*qbi));
#ifdef CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &qbi->qbi_next);
#endif
	}
}
struct qband *allocqb(void)
{
	struct qband *qb;
	struct strinfo *si = &Strinfo[DYN_QBAND];
	if ((qb = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		struct qbinfo *qbi = (struct qbinfo *) qb;
		atomic_set(&qbi->qbi_refs, 1);
#ifdef CONFIG_STREAMS_DEBUG
		{
			unsigned long flags;
			write_lock_irqsave(&si->si_rwlock, flags);
			list_add_tail((struct list_head *) &qbi->qbi_next, &si->si_head);
			write_unlock_irqrestore(&si->si_rwlock, flags);
		}
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
	}
	return (qb);
}
void freeqb(struct qband *qb)
{
	struct strinfo *si = &Strinfo[DYN_QBAND];
#ifdef CONFIG_STREAMS_DEBUG
	unsigned long flags;
	struct qbinfo *qbi = (struct qbinfo *) qb;
	write_lock_irqsave(&si->si_rwlock, flags);
	list_del_init((struct list_head *) &qbi->qbi_next);
	write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
	atomic_dec(&si->si_cnt);
	/* clean it up before putting it back in the cache */
	bzero(qb, sizeof(*qb));
	kmem_cache_free(si->si_cache, qb);
}
static void __freebands(queue_t *q)
{
	struct qband *qb, *qb_next;
	if ((qb_next = xchg(&q->q_bandp, NULL))) {
		while ((qb = qb_next)) {
			qb_next = qb->qb_next;
			freeqb(qb);
		}
	}
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  APUSH ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  keep the cache ctors and the object ctors and dtors close to each other.
 */
static void apinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct apinfo *api = obj;
		bzero(api, sizeof(*api));
		INIT_LIST_HEAD(&api->api_list);
#ifdef CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &api->api_next);
#endif
	}
}
struct apinfo *ap_get(struct strapush *sap)
{
	struct apinfo *api;
	struct strinfo *si = &Strinfo[DYN_STRAPUSH];
	if ((api = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
#ifdef CONFIG_STREAMS_DEBUG
		unsigned long flags;
		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail((struct list_head *) &api->api_next, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
		atomic_set(&api->api_refs, 1);
		api->api_sap = *sap;
	}
	return (api);
}
struct apinfo *ap_grab(struct apinfo *api)
{
	if (api) {
		if (atomic_read(&api->api_refs) < 1)
			swerr();
		atomic_inc(&api->api_refs);
	}
	return (api);
}
void ap_put(struct apinfo *api)
{
	if (api) {
		if (atomic_dec_and_test(&api->api_refs)) {
			struct strinfo *si = &Strinfo[DYN_STRAPUSH];
#ifdef CONFIG_STREAMS_DEBUG
			unsigned long flags;
			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &api->api_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
			atomic_dec(&si->si_cnt);
			list_del_init(&api->api_list);
			/* clean it up before putting it back in the cache */
			bzero(api, sizeof(*api));
			INIT_LIST_HEAD(&api->api_list);
#ifdef CONFIG_STREAMS_DEBUG
			INIT_LIST_HEAD((struct list_head *) &api->api_next);
#endif
			kmem_cache_free(si->si_cache, api);
		}
		return;
	}
	swerr();
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  DEVINFO ctors and dtors
 *
 *  -------------------------------------------------------------------------
 */
static void devinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct devinfo *di = obj;
		bzero(di, sizeof(*di));
		INIT_LIST_HEAD(&di->di_list);
		INIT_LIST_HEAD((struct list_head *) &di->di_next);
	}
}
struct devinfo *di_get(struct cdevsw *cdev)
{
	struct devinfo *di;
	struct strinfo *si = &Strinfo[DYN_DEVINFO];
	if ((di = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		unsigned long flags;
		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail((struct list_head *) &di->di_next, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
		atomic_set(&di->di_refs, 1);
		di->di_dev = cdev;
		atomic_set(&di->di_count, 0);
		di->di_info = cdev->d_str->st_rdinit->qi_minfo;
		INIT_LIST_HEAD(&di->di_list);
	}
	return (di);
}
struct devinfo *di_grab(struct devinfo *di)
{
	if (di) {
		if (atomic_read(&di->di_refs) < 1)
			swerr();
		atomic_inc(&di->di_refs);
	}
	return (di);
}
void di_put(struct devinfo *di)
{
	if (di) {
		if (atomic_dec_and_test(&di->di_refs)) {
			struct strinfo *si = &Strinfo[DYN_DEVINFO];
			unsigned long flags;
			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &di->di_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
			atomic_dec(&si->si_cnt);
			list_del_init(&di->di_list);
			/* clean it up before putting it back in the cache */
			bzero(di, sizeof(*di));
			INIT_LIST_HEAD(&di->di_list);
			INIT_LIST_HEAD((struct list_head *) &di->di_next);
			kmem_cache_free(si->si_cache, di);
		}
		return;
	}
	swerr();
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  MODINFO ctors and dtors
 *
 *  -------------------------------------------------------------------------
 */
static void modinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct modinfo *mi = obj;
		bzero(mi, sizeof(*mi));
		INIT_LIST_HEAD(&mi->mi_list);
		INIT_LIST_HEAD((struct list_head *) &mi->mi_next);
	}
}
struct modinfo *mi_get(struct fmodsw *fmod)
{
	struct modinfo *mi;
	struct strinfo *si = &Strinfo[DYN_MODINFO];
	if ((mi = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		unsigned long flags;
		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail((struct list_head *) &mi->mi_next, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
		atomic_set(&mi->mi_refs, 1);
		mi->mi_mod = fmod;
		atomic_set(&mi->mi_count, 0);
		mi->mi_info = fmod->f_str->st_rdinit->qi_minfo;
		INIT_LIST_HEAD(&mi->mi_list);
	}
	return (mi);
}
struct modinfo *mi_grab(struct modinfo *mi)
{
	if (mi) {
		if (atomic_read(&mi->mi_refs) < 1)
			swerr();
		atomic_inc(&mi->mi_refs);
	}
	return (mi);
}
void mi_put(struct modinfo *mi)
{
	if (mi) {
		if (atomic_dec_and_test(&mi->mi_refs)) {
			struct strinfo *si = &Strinfo[DYN_MODINFO];
			unsigned long flags;
			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &mi->mi_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
			atomic_dec(&si->si_cnt);
			list_del_init(&mi->mi_list);
			/* clean it up before putting it back in the cache */
			bzero(mi, sizeof(*mi));
			INIT_LIST_HEAD(&mi->mi_list);
			INIT_LIST_HEAD((struct list_head *) &mi->mi_next);
			kmem_cache_free(si->si_cache, mi);
		}
		return;
	}
	swerr();
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  MODINFO ctors and dtors
 *
 *  -------------------------------------------------------------------------
 */

/* 
 *  FLUSHQ
 *  -------------------------------------------------------------------------
 */
static int __rmvq(queue_t *q, mblk_t *mp)
{
	int backenable = 0;
	struct qband *qb;
	if (mp->b_prev)
		mp->b_prev->b_next = mp->b_next;
	if (mp->b_next)
		mp->b_next->b_prev = mp->b_prev;
	if (q->q_first == mp)
		q->q_first = mp->b_next;
	if (q->q_last == mp)
		q->q_last = mp->b_prev;
	if (q->q_pctl == mp)
		q->q_pctl = mp->b_prev;
	if (!(qb = mp->b_bandp)) {
		q->q_count -= mp->b_size;
		if (q->q_count < q->q_hiwat)
			clear_bit(QFULL_BIT, &q->q_flag);
		if (q->q_count <= q->q_lowat && test_and_clear_bit(QWANTW_BIT, &q->q_flag))
			backenable = 1;
	} else {
		if (qb->qb_first == mp)
			qb->qb_first = mp->b_next;
		if (qb->qb_last == mp)
			qb->qb_last = mp->b_prev;
		qb->qb_count -= mp->b_size;
		qb->qb_msgs--;
		if (qb->qb_count < qb->qb_hiwat)
			clear_bit(QB_FULL_BIT, &qb->qb_flag);
		if (qb->qb_count <= qb->qb_lowat && test_and_clear_bit(QB_WANTW_BIT, &qb->qb_flag))
			backenable = 1;
	}
	q->q_msgs--;
	mp->b_queue = NULL;
	mp->b_bandp = NULL;
	mp->b_next = mp->b_prev = NULL;
	return (backenable);
}
static int __flushq(queue_t *q, int flag, mblk_t ***mppp)
{
	mblk_t *mp, *mp_next;
	struct qband *qb;
	int backenable = 0;
	switch (flag) {
	case FLUSHDATA:
		mp_next = q->q_first;
		while ((mp = mp_next)) {
			mp_next = mp->b_next;
			if (isdatamsg(mp)) {
				backenable |= __rmvq(q, mp);
				**mppp = mp;
				*mppp = &mp->b_next;
				**mppp = NULL;
			}
		}
		break;
	default:
		swerr();
	case FLUSHALL:
		/* This is fast! For flushall, we link the whole chain onto the free list and null
		   out counts and markers */
		if ((**mppp = q->q_first)) {
			*mppp = &q->q_last->b_next;
			**mppp = NULL;
			q->q_first = q->q_pctl = q->q_last = NULL;
			q->q_count = 0;
			q->q_msgs = 0;
			clear_bit(QFULL_BIT, &q->q_flag);
			clear_bit(QWANTW_BIT, &q->q_flag);
			for (qb = q->q_bandp; qb; qb = qb->qb_next) {
				qb->qb_first = qb->qb_last = NULL;
				qb->qb_count = 0;
				qb->qb_msgs = 0;
				clear_bit(QB_FULL_BIT, &qb->qb_flag);
				clear_bit(QB_WANTW_BIT, &qb->qb_flag);
			}
			backenable = 1;	/* always backenable when queue empty */
		}
		break;
	}
	return (backenable);
}
void flushq(queue_t *q, int flag)
{
	int backenable;
	void freechain(mblk_t *mp, mblk_t **mpp);
	mblk_t *mp = NULL, **mpp = &mp;
	unsigned long flags;
	qwlock(q, &flags);
	backenable = __flushq(q, flag, &mpp);
	qwunlock(q, &flags);
	if (backenable)
		qbackenable(q);
	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	freechain(mp, mpp);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  QUEUE ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void queinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct queinfo *qu = obj;
		bzero(qu, sizeof(*qu));
		/* initialize queue locks */
		qplockinit((queue_t *) qu);
		qlockinit(&qu->rq);
		qlockinit(&qu->wq);
		qu->rq.q_flag = QREADR;
		qu->wq.q_flag = 0;
#ifdef CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &qu->qu_next);
#endif
	}
}

/* 
 *  ALLOCQ
 *  -------------------------------------------------------------------------
 *  Can be called by the module writer to get a private queue pair, but must
 *  then be called with NULL.
 */
queue_t *allocq(void)
{
	queue_t *rq;
	struct strinfo *si = &Strinfo[DYN_QUEUE];
	if ((rq = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		queue_t *wq = rq + 1;
		struct queinfo *qu = (struct queinfo *) rq;
		atomic_set(&qu->qu_refs, 1);
#ifdef CONFIG_STREAMS_DEBUG
		{
			unsigned long flags;
			write_lock_irqsave(&si->si_rwlock, flags);
			list_add_tail((struct list_head *) &qu->qu_next, &si->si_head);
			write_unlock_irqrestore(&si->si_rwlock, flags);
		}
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
		rq->q_flag = QUSE | QREADR;
		wq->q_flag = QUSE;
	}
	return (rq);
}

/* 
 *  FREEQ
 *  -------------------------------------------------------------------------
 *  Can be called by the module writer on private queue pairs allocated with
 *  allocq().  All message blocks will be flushed.
 */
static void __freeq(queue_t *rq)
{
	queue_t *wq = rq + 1;
	struct strinfo *si = &Strinfo[DYN_QUEUE];
#ifdef CONFIG_STREAMS_DEBUG
	unsigned long flags;
	struct queinfo *qu = (struct queinfo *) rq;
	write_lock_irqsave(&si->si_rwlock, flags);
	list_del_init((struct list_head *) &qu->qu_next);
	write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
	atomic_dec(&si->si_cnt);
	/* clear flags */
	rq->q_flag = QREADR;
	wq->q_flag = 0;
	/* break locks */
	qlockinit(rq);
	qlockinit(wq);
	/* put back in cache */
	kmem_cache_free(si->si_cache, rq);
}
void freeq(queue_t *rq)
{
	queue_t *wq = rq + 1;
	mblk_t *mp = NULL, **mpp = &mp;
	void freechain(mblk_t *mp, mblk_t **mpp);
	clear_bit(QUSE_BIT, &rq->q_flag);
	clear_bit(QUSE_BIT, &wq->q_flag);
	__flushq(rq, FLUSHALL, &mpp);
	__flushq(wq, FLUSHALL, &mpp);
	__freebands(rq);
	__freebands(wq);
	__freeq(rq);
	freechain(mp, mpp);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  LINKBLK ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void linkinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		static spinlock_t link_index_lock = SPIN_LOCK_UNLOCKED;
		static int link_index = 0;
		unsigned long flags;
		struct linkinfo *li = obj;
		struct linkblk *l = &li->li_linkblk;
		bzero(li, sizeof(*li));
#ifdef CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &li->li_next);
#endif
		spin_lock_irqsave(&link_index_lock, flags);
		l->l_index = ++link_index;
		if (++link_index < 1)
			link_index = 1;
		spin_unlock_irqrestore(&link_index_lock, flags);
	}
}
struct linkblk *alloclk(void)
{
	struct linkblk *l;
	struct strinfo *si = &Strinfo[DYN_LINKBLK];
	if ((l = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
#ifdef CONFIG_STREAMS_DEBUG
		unsigned long flags;
		struct linkinfo *li = (struct linkinfo *) l;
		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail((struct list_head *) &li->li_next, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
	}
	return (l);
}
void freelk(struct linkblk *l)
{
	struct strinfo *si = &Strinfo[DYN_LINKBLK];
	struct linkinfo *li = (struct linkinfo *) l;
#ifdef CONFIG_STREAMS_DEBUG
	{
		unsigned long flags;
		write_lock_irqsave(&si->si_rwlock, flags);
		list_del_init((struct list_head *) &li->li_next);
		write_unlock_irqrestore(&si->si_rwlock, flags);
	}
#endif
	atomic_dec(&si->si_cnt);
	kmem_cache_free(si->si_cache, li);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  SYNCQ ctos and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */

static void syncq_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct syncq *sq = obj;
		bzero(sq, sizeof(*sq));
		spin_lock_init(&sq->sq_lock);
		init_waitqueue_head(&sq->sq_waitq);
		sq->sq_head = NULL;
		sq->sq_tail = &sq->sq_head;
		atomic_set(&sq->sq_refs, 0);
		INIT_LIST_HEAD((struct list_head *) &sq->sq_next);
	}
}
struct syncq *sq_get(void)
{
	struct syncq *sq;
	struct strinfo *si = &Strinfo[DYN_SYNCQ];
	if ((sq = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		unsigned long flags;
		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail((struct list_head *) &sq->sq_next, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
		atomic_set(&sq->sq_refs, 1);
	}
	return (sq);
}
struct syncq *sq_grab(struct syncq *sq)
{
	if (sq) {
		if (atomic_read(&sq->sq_refs) < 1)
			swerr();
		atomic_inc(&sq->sq_refs);
	}
	return (sq);
}
void sq_put(struct syncq **sqp)
{
	struct syncq *sq;
	if ((sq = xchg(sqp, NULL))) {
		if (atomic_dec_and_test(&sq->sq_refs)) {
			struct strinfo *si = &Strinfo[DYN_SYNCQ];
			unsigned long flags;
			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &sq->sq_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
			/* clean it up before puting it back in the cache */
			// sq_put(&sq->sq_other);	/* recurse once */
			if (!spin_trylock(&sq->sq_lock))
				pswerr(("breaking syncq lock\n"));
			spin_lock_init(&sq->sq_lock);
			if (sq->sq_head != NULL)
				pswerr(("events lost!\n"));
			sq->sq_head = NULL;
			sq->sq_tail = &sq->sq_head;
			if (waitqueue_active(&sq->sq_waitq))
				pswerr(("destroying syncq with waiters!\n"));
			wake_up_all(&sq->sq_waitq);
			init_waitqueue_head(&sq->sq_waitq);
			kmem_cache_free(si->si_cache, sq);
		}
	}
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  STREVENT ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
#define EVENT_ID_BITS 16
#define EVENT_ID_MASK ((1<<EVENT_ID_BITS)-1)
#define EVENT_ID_SHIFT (sizeof(bcid_t)*8-EVENT_ID_BITS)
#define EVENT_SEQ_MASK ((1<<EVENT_ID_SHIFT)-1)
#define EVENT_LIMIT (1<<EVENT_ID_BITS)
#define EVENT_HASH_SIZE (1<<10)
#define EVENT_HASH_MASK (EVENT_HASH_SIZE-1)
static rwlock_t event_hash_lock = RW_LOCK_UNLOCKED;
static struct strevent *event_hash[EVENT_HASH_SIZE] __cacheline_aligned;
static int event_id = 0;
static void seinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct seinfo *s = obj;
		struct strevent *se = obj;
		unsigned long flags;
		bzero(s, sizeof(*s));
#ifdef CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &s->s_next);
#endif
		write_lock_irqsave(&event_hash_lock, flags);
		se->se_id = event_id;
		se->se_prev = xchg(&event_hash[event_id & EVENT_HASH_MASK], se);
		event_id++;
		write_unlock_irqrestore(&event_hash_lock, flags);
	}
}
static inline struct strevent *event_alloc(int type, queue_t *q)
{
	struct strinfo *si = &Strinfo[DYN_STREVENT];
	struct strevent *se = NULL;
	if (atomic_read(&si->si_cnt) < EVENT_LIMIT) {
		if ((se = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
			struct seinfo *s = (struct seinfo *) se;
			s->s_type = type;
#ifdef CONFIG_STREAMS_DEBUG
			{
				unsigned long flags;
				s->s_queue = queue_guess(q);
				write_lock_irqsave(&si->si_rwlock, flags);
				list_add_tail((struct list_head *) &s->s_next, &si->si_head);
				write_unlock_irqrestore(&si->si_rwlock, flags);
			}
#endif
			atomic_inc(&si->si_cnt);
			if (atomic_read(&si->si_cnt) > si->si_hwl)
				si->si_hwl = atomic_read(&si->si_cnt);
		}
	}
	return (se);
}
static inline void event_free(struct strevent *se)
{
	struct strinfo *si = &Strinfo[DYN_STREVENT];
#ifdef CONFIG_STREAMS_DEBUG
	unsigned long flags;
	struct seinfo *s = (struct seinfo *) se;
	qput(&s->s_queue);
	write_lock_irqsave(&si->si_rwlock, flags);
	list_del_init((struct list_head *) &s->s_next);
	write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
	atomic_dec(&si->si_cnt);
	se->se_seq++;
	kmem_cache_free(si->si_cache, se);
}
static struct strevent *find_event(int event_id)
{
	struct strevent **sep;
	int id = (event_id >> EVENT_ID_SHIFT) & EVENT_ID_MASK;
	int seq = event_id & EVENT_SEQ_MASK;
	unsigned long flags;
	sep = &event_hash[id & EVENT_HASH_MASK];
	read_lock_irqsave(&event_hash_lock, flags);
	for (; *sep; sep = &(*sep)->se_prev)
		if ((*sep)->se_id == id && (*sep)->se_seq == seq)
			break;
	read_unlock_irqrestore(&event_hash_lock, flags);
	return (*sep);
}

/* Note that, for MP safety, bufcalls are always raised against the same processor that invoked the 
   buffer call.  This means that the callback function will not execute until after the caller hits 
   a pre-emption point. */
static inline bcid_t __bufcall(queue_t *q, unsigned size, int priority, void (*function) (long),
			       long arg)
{
	bcid_t bcid = 0;
	struct strevent *se;
	if ((se = event_alloc(SE_BUFCALL, q))) {
		struct strthread *t = this_thread;
		se->x.b.queue = qget(q);	/* hold a reference */
		se->x.b.func = function;
		se->x.b.arg = arg;
		se->x.b.size = size;
		*xchg(&t->strbcalls_tail, &se->se_next) = se;
		set_bit(strbcwait, &t->flags);
	}
	return (bcid);
}

/**
 *  BUFCALL - schedule a buffer callout
 *  @size:the number of bytes of data buffer needed
 *  @priority:the priority of the buffer allocation (ignored)
 *  @function:the callback function when bytes and headers are available
 *  @arg:a client argument to pass to the callback function
 */
bcid_t bufcall(unsigned size, int priority, void (*function) (long), long arg)
{
	return __bufcall(NULL, size, priority, function, arg);
}

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
/**
 *  QBUFCALL - schedule a buffer callout
 *  @q:queue used for synchronization
 *  @size:the number of bytes of data buffer needed
 *  @priority:the priority of the buffer allocation (ignored)
 *  @function:the callback function when bytes and headers are available
 *  @arg:a client argument to pass to the callback function
 */
bufcall_id_t qbufcall(queue_t *q, size_t size, int priority, void (*function) (void *), void *arg)
{
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	return __bufcall(q, size, priority, (void (*)(long)) function, (long) arg);
}
#endif

#if defined(CONFIG_STREAMS_COMPAT_AIX) || defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)
/**
 *  MI_BUFCALL - enable a queue when a buffer available
 *  @q:queue used for synchronization
 *  @size:the number of bytes of data buffer needed
 *  @priority:the priority of the buffer allocation (ignored)
 */
void mi_bufcall(queue_t *q, int size, int priority)
{
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	__bufcall(q, size, priority, (void (*)) (long) qenable, (long) q);
	return;
}
#endif

/**
 *  ESBBCALL - schedule a buffer callout
 *  @priority:the priority of the buffer allocation (ignored)
 *  @function:the callback function when bytes and headers are available
 *  @arg:a client argument to pass to the callback function
 */
__EXTERN_INLINE bcid_t esbbcall(int priority, void (*function) (long), long arg);

/**
 *  UNBUFCALL - cancel a buffer callout
 *  @bcid:buffer call id returned by bufcall() or esbbcall()
 *  NOTICES:Don't ever call this function with an expired bufcall id.
 */
void unbufcall(bcid_t bcid)
{
	struct strevent *se;
	if ((se = find_event(bcid)))
		xchg(&se->x.b.func, NULL);
}

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
/**
 *  QUNBUFCALL - cancel a buffer callout
 *  @q:queue used for synchronization
 *  @bcid:buffer call id returned by qbufcall()
 *  NOTICES:Don't ever call this function with an expired bufcall id.
 */
void qunbufcall(queue_t *q, bufcall_id_t bcid)
{
	unbufcall(bcid);
}
#endif

/* 
 *  TIMEOUT
 *  -------------------------------------------------------------------------
 *  Note that, for MP safety, timeouts are always raised against the same
 *  processor that invoked the timeout.  This means that the callback function
 *  will not execute until after the caller hits a pre-emption point.
 */
static void timeout_function(unsigned long arg)
{
	struct strevent *se = (struct strevent *) arg;
	struct strthread *t = &strthreads[se->x.t.cpu];
	/* FIXME: this is dangerous - need a per-cpu list spin */
	*xchg(&t->strtimout_tail, &se->se_next) = se;
	/* bind timeout back to the CPU that called for it */
	if (!test_and_set_bit(strtimout, &t->flags))
		cpu_raise_softirq(se->x.t.cpu, STREAMS_SOFTIRQ);
}
static toid_t __timeout(queue_t *q, timo_fcn_t *timo_fcn, caddr_t arg, long ticks, unsigned long pl,
			int cpu)
{
	toid_t toid = 0;
	struct strevent *se;
	if ((se = event_alloc(SE_TIMEOUT, q))) {
		init_timer(&se->x.t.timer);
		se->x.t.queue = qget(q);	/* hold a reference */
		se->x.t.func = timo_fcn;
		se->x.t.arg = arg;
		se->x.t.cpu = cpu;
		se->x.t.pl = pl;
		se->x.t.timer.expires = jiffies + ticks;
		se->x.t.timer.data = (long) se;
		se->x.t.timer.function = timeout_function;
		toid = (se->se_id << EVENT_ID_SHIFT) | (se->se_seq & EVENT_SEQ_MASK);
		add_timer(&se->x.t.timer);
	}
	return (toid);
}
toid_t timeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks)
{
	return __timeout(NULL, timo_fcn, arg, ticks, 0, smp_processor_id());
}

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
timeout_id_t qtimeout(queue_t *q, void (*timo_fcn) (void *), void *arg, long ticks)
{
	// queue_t *rq = RD(q);
	// assert(!test_bit(QHLIST_BIT, &rq->q_flag));
	return __timeout(q, (timo_fcn_t *) timo_fcn, (caddr_t) arg, ticks, 0, smp_processor_id());
}
#endif

#if defined(CONFIG_STREAMS_COMPAT_UW7) || defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)
toid_t dtimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl, processorid_t cpu)
{
	return __timeout(NULL, timo_fcn, arg, ticks, pl, cpu);
}
toid_t itimeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks, pl_t pl)
{
	return __timeout(NULL, timo_fcn, arg, ticks, pl, smp_processor_id());
}
#endif

/* 
 *  UNTIMEOUT
 *  -------------------------------------------------------------------------
 */
clock_t untimeout(toid_t toid)
{
	struct strevent *se;
	clock_t rem = 0;
	if ((se = find_event(toid))) {
		xchg(&se->x.t.func, NULL);
		qput(&se->x.t.queue);
		rem = se->x.t.timer.expires - jiffies;
		if (rem < 0)
			rem = 0;
		if (del_timer(&se->x.t.timer))
			event_free(se);
	}
	return (rem);
}

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
clock_t quntimeout(queue_t *q, timeout_id_t toid)
{
	return untimeout(toid);
}
#endif

/* 
 *  WELDQ
 *  -------------------------------------------------------------------------
 */
static int __weldq(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func,
		   weld_arg_t arg, queue_t *protq, int type)
{
	struct strevent *se;
	if ((se = event_alloc(type, protq))) {
		struct strthread *t = this_thread;
		se->x.w.queue = qget(protq);
		se->x.w.func = func;
		se->x.w.arg = arg;
		se->x.w.q1 = qget(q1);
		se->x.w.q2 = qget(q2);
		se->x.w.q3 = qget(q3);
		se->x.w.q4 = qget(q4);
		*xchg(&t->strevents_tail, &se->se_next) = se;
		if (test_and_set_bit(strevents, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
		return (0);
	}
	return (EAGAIN);
}
int weldq(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func, weld_arg_t arg,
	  queue_t *protq)
{
	if (!q1)
		return (EINVAL);
	if (q1->q_next != NULL || test_bit(QWELDED_BIT, &q1->q_flag))
		return (EINVAL);
	if (!q3)
		goto onepair;
	if (q3->q_next != NULL || test_bit(QWELDED_BIT, &q3->q_flag))
		return (EINVAL);
      onepair:
	if (q1)
		set_bit(QWELDED_BIT, &q1->q_flag);
	if (q3)
		set_bit(QWELDED_BIT, &q3->q_flag);
	return __weldq(q1, q2, q3, q4, func, arg, protq, SE_WELDQ);
}

/* 
 *  UNWELDQ
 *  -------------------------------------------------------------------------
 */
int unweldq(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func, weld_arg_t arg,
	    queue_t *protq)
{
	if (!q1)
		return (EINVAL);
	if (q1->q_next != q2 || !test_bit(QWELDED_BIT, &q1->q_flag))
		return (EINVAL);
	if (!q3)
		goto onepair;
	if (q3->q_next != q4 || !test_bit(QWELDED_BIT, &q3->q_flag))
		return (EINVAL);
      onepair:
	if (q1)
		clear_bit(QWELDED_BIT, &q1->q_flag);
	if (q3)
		clear_bit(QWELDED_BIT, &q3->q_flag);
	return __weldq(q1, NULL, q3, NULL, func, arg, protq, SE_UNWELDQ);
}

/* 
 *  STREAMS_PUT
 *  -------------------------------------------------------------------------
 */
#if defined(CONFIG_STREAMS_COMPAT_HPUX) || defined(CONFIG_STREAMS_COMPAT_HPUX_MODULE) || defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
static int defer_func(void (*func) (void *, mblk_t *), queue_t *q, mblk_t *mp, void *arg,
		      int perim, int type)
{
	struct strevent *se;
	if ((se = event_alloc(type, q))) {
		struct strthread *t = this_thread;
		se->x.p.queue = qget(q);
		se->x.p.func = func;
		se->x.p.arg = arg;
		se->x.p.perim = perim;
		se->x.p.mp = mp;
		*xchg(&t->strevents_tail, &se->se_next) = se;
		if (test_and_set_bit(strevents, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
		return (0);
	}
	return (ENOMEM);
}
#endif

#if defined(CONFIG_STREAMS_COMPAT_HPUX) || defined(CONFIG_STREAMS_COMPAT_HPUX_MODULE)
void streams_put(void (*func) (void *, mblk_t *), queue_t *q, mblk_t *mp, void *priv)
{
	if (defer_func(func, q, mp, priv, 0, SE_STRPUT) == 0)
		return;
	never();
}
#endif
#if 0
void __defer_put(queue_t *q, mblk_t *mp)
{
	if (defer_func((void (*)(void *, mblk_t *)) put, q, mp, q, 0, SE_STRPUT) == 0)
		return;
	never();
}
#endif

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
/* 
 *  QWRITER
 *  ---------------------------------------------------------------------------
 */
void qwriter(queue_t *qp, mblk_t *mp, void (*func) (queue_t *qp, mblk_t *mp), int perimeter)
{
	if (defer_func((void (*)(void *, mblk_t *)) func, qp, mp, qp, perimeter, SE_WRITER) == 0)
		return;
	never();
}
#endif

/* 
 *  DEFERRAL FUNCTION ON SYNCH QUEUES
 *  -------------------------------------------------------------------------
 */
void __defer_put(syncq_t *sq, queue_t *q, mblk_t *mp)
{
	/* must be called with sq locked */
	struct strevent *se;
	if ((se = event_alloc(SE_STRPUT, q))) {
		se->x.p.queue = qget(q);
		se->x.p.func = (void *) put;
		se->x.p.arg = q;
		se->x.p.perim = PERIM_INNER | PERIM_OUTER;
		se->x.p.mp = mp;
		*xchg(&sq->sq_tail, &se->se_next) = se;
		return;
	}
	never();
}

/* Execution of deferred events */
static void defer_event(syncq_t *sq, struct strevent *se, unsigned long *flagsp)
{
	*xchg(&sq->sq_tail, &se->se_next) = se;
	unlock_syncq(sq, flagsp);
}

static void sq_stream(struct strevent *se)
{
}

static void sq_bufcall(struct strevent *se)
{
	queue_t *q = se->x.b.queue;
	syncq_t *isq = q ? q->q_syncq : NULL, *osq = isq ? isq->sq_outer : isq; /* XXX */
	unsigned long flags;
	if (enter_shared(osq, &flags)) {
		if (enter_exclus(isq, &flags)) {
			/* we are inside the perimeters */
			/* if no sync, running on correct cpu */
			if (se->x.b.func)
				se->x.b.func(se->x.b.arg);
			leave_exclus(isq);
			event_free(se);
		} else
			defer_event(isq, se, &flags);
	} else
		defer_event(osq, se, &flags);
}

static void sq_timeout(struct strevent *se)
{
	queue_t *q = se->x.t.queue;
	syncq_t *isq = q ? q->q_syncq : NULL, *osq = isq ? isq->sq_outer : isq;
	unsigned long flags;
	if (enter_shared(osq, &flags)) {
		if (enter_exclus(isq, &flags)) {
			/* we are inside the perimeters */
			/* if no sync, running on correct cpu */
			if (se->x.t.pl)
				local_irq_save(flags);
			if (se->x.t.func)
				se->x.t.func(se->x.t.arg);
			if (se->x.t.pl)
				local_irq_restore(flags);
			leave_exclus(isq);
			event_free(se);
		} else
			defer_event(isq, se, &flags);
		leave_shared(osq);
	} else
		defer_event(osq, se, &flags);
}

static void sq_weldq(struct strevent *se)
{
	queue_t *q = se->x.w.queue;
	syncq_t *isq = q ? q->q_syncq : NULL, *osq = isq ? isq->sq_outer : isq;
	unsigned long flags;
	if (enter_shared(osq, &flags)) {
		if (enter_exclus(isq, &flags)) {
			queue_t *qn1 = NULL, *qn3 = NULL;
			local_irq_save(flags);
			if (se->x.w.q1)
				__hwlock(se->x.w.q1);
			if (se->x.w.q3)
				__hwlock(se->x.w.q3);
			if (se->x.w.q1) {
				qn1 = xchg(&se->x.w.q1->q_next, NULL);
				se->x.w.q1->q_next = qget(se->x.w.q2);
			}
			if (se->x.w.q3) {
				qn3 = xchg(&se->x.w.q1->q_next, NULL);
				se->x.w.q3->q_next = qget(se->x.w.q4);
			}
			if (se->x.w.q3)
				__hwunlock(se->x.w.q3);
			if (se->x.w.q1)
				__hwunlock(se->x.w.q1);
			local_irq_restore(flags);
			leave_exclus(isq);
			qput(&se->x.w.q1);
			qput(&se->x.w.q2);
			qput(&se->x.w.q3);
			qput(&se->x.w.q4);
			event_free(se);
			qput(&qn1);
			qput(&qn3);
		} else
			defer_event(isq, se, &flags);
		leave_shared(osq);
	} else
		defer_event(osq, se, &flags);
}

static void sq_unweldq(struct strevent *se)
{
	queue_t *q = se->x.w.queue;
	syncq_t *isq = q ? q->q_syncq : NULL, *osq = isq ? isq->sq_outer : isq;
	unsigned long flags;
	if (enter_shared(osq, &flags)) {
		if (enter_exclus(isq, &flags)) {
			queue_t *qn1 = NULL, *qn3 = NULL;
			local_irq_save(flags);
			if (se->x.w.q1)
				__hwlock(se->x.w.q1);
			if (se->x.w.q3)
				__hwlock(se->x.w.q3);
			if (se->x.w.q1) {
				qn1 = xchg(&se->x.w.q1->q_next, NULL);
				se->x.w.q1->q_next = qget(se->x.w.q2);
			}
			if (se->x.w.q3) {
				qn3 = xchg(&se->x.w.q1->q_next, NULL);
				se->x.w.q3->q_next = qget(se->x.w.q4);
			}
			if (se->x.w.q3)
				__hwunlock(se->x.w.q3);
			if (se->x.w.q1)
				__hwunlock(se->x.w.q1);
			local_irq_restore(flags);
			leave_exclus(isq);
			qput(&se->x.w.q1);
			qput(&se->x.w.q2);
			qput(&se->x.w.q3);
			qput(&se->x.w.q4);
			event_free(se);
			qput(&qn1);
			qput(&qn3);
		} else
			defer_event(isq, se, &flags);
		leave_shared(osq);
	} else
		defer_event(osq, se, &flags);
}

static void sq_strput(struct strevent *se)
{
	queue_t *q = se->x.p.queue;
	syncq_t *isq = q ? q->q_syncq : NULL, *osq = isq ? isq->sq_outer : isq;
	unsigned long flags;
	hrlock(q);
	if (enter_shared(osq, &flags)) {
		if (isq->sq_flag & D_MTPUTSHARED) {
			if (enter_shared(isq, &flags)) {
				se->x.p.func(q, se->x.p.mp);
				leave_exclus(isq);
				event_free(se);
			} else
				defer_event(isq, se, &flags);
		} else {
			if (enter_exclus(isq, &flags)) {
				se->x.p.func(q, se->x.p.mp);
				leave_exclus(isq);
				event_free(se);
			} else
				defer_event(isq, se, &flags);
		}
		leave_shared(osq);
	} else
		defer_event(osq, se, &flags);
	hrunlock(q);
}

static void sq_writer(struct strevent *se)
{
	queue_t *q = se->x.p.queue;
	syncq_t *isq = q ? q->q_syncq : NULL, *osq = isq ? isq->sq_outer : isq;
	unsigned long flags;
	hrlock(q);
	switch (se->x.p.perim) {
	case PERIM_INNER:
		if (enter_shared(osq, &flags)) {
			if (enter_exclus(isq, &flags)) {
				se->x.p.func(q, se->x.p.mp);
				leave_exclus(isq);
				event_free(se);
			} else
				defer_event(isq, se, &flags);
			leave_shared(osq);
		} else
			defer_event(osq, se, &flags);
		break;
	case PERIM_OUTER:
		if (enter_exclus(osq, &flags)) {
			se->x.p.func(q, se->x.p.mp);
			leave_exclus(osq);
			event_free(se);
		} else
			defer_event(osq, se, &flags);
		break;
	default:
		swerr();
		break;
	}
	hrunlock(q);
}

static void sq_putp(struct strevent *se)
{
	/* this will defer itself if necessary */
	se->x.p.func(se->x.p.arg, se->x.p.mp);
	event_free(se);
}

/* 
 *  KMEM_ALLOC
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE void *kmem_alloc(size_t size, int flags);

/* 
 *  KMEM_ZALLOC
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE void *kmem_zalloc(size_t size, int flags);

/* 
 *  KMEM_FREE
 *  -------------------------------------------------------------------------
 */
void kmem_free(void *addr, size_t size)
{
	kfree(addr);
	if (size) {
		struct strthread *t = this_thread;
		if (test_bit(strbcwait, &t->flags) && !test_and_set_bit(strbcflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
	}
}

/* 
 *  -------------------------------------------------------------------------
 *  -------------------------------------------------------------------------
 */

static inline void timeouts(struct strthread *t)
{
	register struct strevent *se, *se_next;
	unsigned long flags;
	clear_bit(strtimout, &t->flags);
	local_irq_save(flags);
	if ((se_next = xchg(&t->strtimout_head, NULL))) {
		t->strtimout_tail = &t->strtimout_head;
		local_irq_restore(flags);
		while ((se = se_next)) {
			void (*func) (caddr_t);
			se_next = xchg(&se->se_next, NULL);
			if ((func = se->x.t.func)) {
				queue_t *q;
				if ((q = se->x.t.queue)) {
					if (test_bit(QUSE_BIT, &q->q_flag)) {
						hrlock(q);
						qprlock(q);
						t->currentq = q;
						func(se->x.t.arg);
						t->currentq = NULL;
						qprunlock(q);
						hrunlock(q);
					}
					qput(&se->x.t.queue);
				} else {
					func(se->x.t.arg);
				}
			}
			event_free(se);
		}
	} else
		local_irq_restore(flags);
}

static inline void doevents(struct strthread *t)
{
	register struct strevent *se, *se_next;
	unsigned long flags;
	clear_bit(strevents, &t->flags);
	local_irq_save(flags);
	if ((se_next = xchg(&t->strevents_head, NULL))) {
		t->strevents_tail = &t->strevents_head;
		local_irq_restore(flags);
		while ((se = se_next)) {
			struct seinfo *s = (typeof(s)) se;
			se_next = xchg(&se->se_next, NULL);
			switch (s->s_type) {
			case SE_STREAM:
			case SE_BUFCALL:
			case SE_TIMEOUT:
				event_free(se);
				continue;;
			case SE_WELDQ:
			case SE_UNWELDQ:
			{
				queue_t *q = xchg(&se->x.w.queue, NULL);
				if (q)
					hwlock(q, &flags);
				if (se->x.w.q1) {
					unsigned long flags;
					hwlock(se->x.w.q1, &flags);
					se->x.w.q1->q_next = se->x.w.q2;
					hwunlock(se->x.w.q1, &flags);
					qput(&se->x.w.q1);
					qput(&se->x.w.q2);
				}
				if (se->x.w.q3) {
					unsigned long flags;
					hwlock(se->x.w.q3, &flags);
					se->x.w.q3->q_next = se->x.w.q4;
					hwunlock(se->x.w.q3, &flags);
					qput(&se->x.w.q3);
					qput(&se->x.w.q4);
				}
				if (se->x.w.func) {
					t->currentq = q;
					se->x.w.func(se->x.w.arg);
					t->currentq = NULL;
				}
				if (q) {
					hwunlock(q, &flags);
					qput(&q);
				}
				event_free(se);
				continue;;
			}
			case SE_STRPUT:
			case SE_WRITER:
			{
				hwlock(se->x.p.queue, &flags);

				qpwlock(se->x.p.queue);
				if (se->x.p.func)
					se->x.p.func(se->x.p.arg, se->x.p.mp);
				qpwunlock(se->x.p.queue);
				hwunlock(se->x.p.queue, &flags);
				qput(&se->x.p.queue);
				event_free(se);
				continue;;
			}
			}
		}
	} else
		local_irq_restore(flags);
}

static inline void backlog(struct strthread *t)
{
	register syncq_t *sq, *sq_link;
	unsigned long flags;
	clear_bit(qsyncflag, &t->flags);
	local_irq_save(flags);
	if ((sq_link = xchg(&t->sqhead, NULL))) {
		t->sqtail = &t->sqhead;
		local_irq_restore(flags);
		while ((sq = sq_link)) {
			register struct strevent *se, *se_next;
			sq_link = xchg(&sq->sq_link, NULL);
			spin_lock_irqsave(&sq->sq_lock, flags);
			if ((se_next = xchg(&sq->sq_head, NULL))) {
				sq->sq_tail = &sq->sq_head;
				spin_unlock_irqrestore(&sq->sq_lock, flags);
				while ((se = se_next)) {
					struct seinfo *s = (typeof(s)) se;
					se_next = xchg(&se->se_next, NULL);
					switch (s->s_type) {
					case SE_STREAM:
						sq_stream(se);
						break;
					case SE_BUFCALL:	/* this is a qbufcall */
						sq_bufcall(se);
						break;
					case SE_TIMEOUT:	/* this is a qtimeout */
						sq_timeout(se);
						break;
					case SE_WELDQ:	/* this is a weldq */
						sq_weldq(se);
						break;
					case SE_UNWELDQ:	/* this is an unweldq */
						sq_unweldq(se);
						break;
					case SE_STRPUT:	/* this is a streams_put */
						sq_strput(se);
						break;
					case SE_WRITER:	/* this is a qwriter */
						sq_writer(se);
						break;
					case SE_PUTP:	/* this is a put */
						sq_putp(se);
						break;
					}
				}
			} else
				spin_unlock_irqrestore(&sq->sq_lock, flags);
		}
	} else
		local_irq_restore(flags);
}

/* 
 *  First order of business for runqueues() is to call bufcalls if there are
 *  bufcalls waiting and there are now blocks available.  We only keep track
 *  of blocks and memory that the streams subsystem frees, so streams modules
 *  will only be allowed to proceed if other modules free something.
 *  Unfortunately, this means that the streams subsystem can lock.  If all
 *  modules hang onto their memory and blocks, and request more, and fail on
 *  allocation, then the streams subsystem will hang until an external event
 *  kicks it.  Therefore, we kick the chain every time an allocation is
 *  successful.
 */
static inline void bufcalls(struct strthread *t)
{
	register struct strevent *se, *se_next;
	unsigned long flags;
	clear_bit(strbcwait, &t->flags);
	local_irq_save(flags);
	if ((se_next = xchg(&t->strbcalls_head, NULL))) {
		t->strbcalls_tail = &t->strbcalls_head;
		local_irq_restore(flags);
		while ((se = se_next)) {
			void (*func) (long);
			se_next = xchg(&se->se_next, NULL);
			if ((func = se->x.b.func)) {
				queue_t *q;
				if ((q = xchg(&se->x.b.queue, NULL))) {
					if (test_bit(QUSE_BIT, &q->q_flag)) {
						hrlock(q);
						qprlock(q);
						t->currentq = q;
						func(se->x.b.arg);
						t->currentq = NULL;
						qprunlock(q);
						hrunlock(q);
					}
					qput(&q);
				} else {
					func(se->x.b.arg);
				}
			}
			event_free(se);
		}
	} else
		local_irq_restore(flags);
}

static inline void queuerun(struct strthread *t)
{
	register queue_t *q, *q_link;
	unsigned long flags;
	clear_bit(qrunflag, &t->flags);
	local_irq_save(flags);
	if ((q_link = xchg(&t->qhead, NULL))) {
		t->qtail = &t->qhead;
		local_irq_restore(flags);
		while ((q = q_link)) {
			queue_t *rq = RD(q);
			struct queinfo *qu = (typeof(qu)) rq;
			q_link = xchg(&q->q_link, NULL);
			if (likely(test_and_clear_bit(QENAB_BIT, &q->q_flag))) {
				int (*srvp) (queue_t *);
				if ((srvp = q->q_qinfo->qi_srvp)) {
					hrlock(rq);
					qprlock(rq);
					set_bit(QSVCBUSY_BIT, &q->q_flag);
					t->currentq = q;
					srvp(q);
					t->currentq = NULL;
					clear_bit(QSVCBUSY_BIT, &q->q_flag);
					qprunlock(rq);
					hrunlock(q);
				}
			}
			if (unlikely(waitqueue_active(&qu->qu_qwait)))
				wake_up_all(&qu->qu_qwait);
			if (unlikely(atomic_dec_and_test(&qu->qu_refs)))
				freeq(rq);
		}
	} else
		local_irq_restore(flags);
}

/* 
 *  SETQSCHED
 *  -------------------------------------------------------------------------
 */
void inline setqsched(void)
{
	struct strthread *t = this_thread;
	if (test_and_set_bit(qrunflag, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
}

/* 
 *  QSCHEDULE
 *  -------------------------------------------------------------------------
 *  Note that, for MP safety,  queues are always scheduled against the same
 *  cpu that enabled the queue.  This means that the service procedure will
 *  not run until after the caller hits a pre-emption point.
 */
static void qschedule(queue_t *q)
{
	if (!test_and_set_bit(QENAB_BIT, &q->q_flag)) {
		struct strthread *t = this_thread;
		/* put ourselves on the run list */
		*xchg(&t->qtail, &q->q_link) = qget(q);
		setqsched();
	}
}

/**
 *  SQSCHED - schedule a synchronization queue
 *  @sq:the synchronization queue to schedule
 *
 *  DESCRIPTION:SQSCHED schedules the synchronization queue @sq to have its
 *  backlog of events serviced, if necessary.  SQSCHED is called when the last
 *  thread leaves a sychronization queue (barrier).
 */
void sqsched(syncq_t *sq)
{
	/* called with sq locked */
	if (!sq->sq_link) {
		struct strthread *t = this_thread;
		/* put ourselves on the run list */
		*xchg(&t->sqtail, &sq->sq_link) = sq_grab(sq);
		if (test_and_set_bit(qsyncflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
	}
}

/* 
 *  ENABLEQ
 *  -------------------------------------------------------------------------
 */
int enableq(queue_t *q)
{
	if (q->q_qinfo->qi_srvp && !test_bit(QNOENB_BIT, &q->q_flag)) {
		qschedule(q);
		return (1);
	}
	return (0);
}

/* 
 *  QENABLE
 *  -------------------------------------------------------------------------
 */
void qenable(queue_t *q)
{
	(void) qschedule(q);
}

/* 
 *  ENABLEOK
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE void enableok(queue_t *q);

/* 
 *  NOENABLE
 *  -------------------------------------------------------------------------
 */
__EXTERN_INLINE void noenable(queue_t *q);

static inline void freechains(struct strthread *t)
{
	register mblk_t *mp, *mp_next;
	unsigned long flags;
	clear_bit(flushwork, &t->flags);
	local_irq_save(flags);
	if ((mp_next = xchg(&t->freemsg_head, NULL))) {
		t->freemsg_tail = &t->freemsg_head;
		local_irq_restore(flags);
		while ((mp = mp_next)) {
			mp_next = xchg(&mp->b_next, NULL);
			/* fake out freeb */
			qput(&mp->b_queue);
			bput(&mp->b_bandp);
			mp->b_next = mp->b_prev = NULL;
			freemsg(mp);
		}
	} else
		local_irq_restore(flags);
}

void freechain(mblk_t *mp, mblk_t **mpp)
{
	struct strthread *t = this_thread;
	*xchg(&t->freemsg_tail, mpp) = mp;
	if (!test_and_set_bit(flushwork, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
}

static void runqueues(struct softirq_action *unused)
{
	struct strthread *t = this_thread;
	set_bit(queueflag, &t->flags);
	/* catch up on backlog first */
	if (t->flags & QSYNCFLAG)
		backlog(t);
	/* do timeouts */
	if (t->flags & STRTIMOUT)
		timeouts(t);
	/* do pending events */
	if (t->flags & STREVENTS)
		doevents(t);
	/* do buffer calls if necessary */
	if (t->flags & (STRBCFLAG | STRBCWAIT))
		bufcalls(t);
	/* run queue service procedures if necessary */
	if (test_bit(qrunflag, &t->flags))
		queuerun(t);
	/* free flush chains if necessary */
	if (test_bit(flushwork, &t->flags))
		freechains(t);
	/* free mdbblocks to cache, if memory needed */
	if (test_bit(freeblks, &t->flags))
		freeblocks(t);
	clear_bit(queueflag, &t->flags);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  SHINFO ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void clear_shinfo(struct shinfo *sh)
{
	struct stdata *sd = &sh->sh_stdata;
	bzero(sh, sizeof(*sh));
	atomic_set(&sh->sh_refs, 0);
#ifdef CONFIG_STREAMS_DEBUG
	INIT_LIST_HEAD((struct list_head *) &sh->sh_next);
#endif
	sd->sd_rdopt = RNORM | RPROTNORM;
	sd->sd_wropt = 0;
	sd->sd_eropt = RERRNORM | WERRNORM;
	init_waitqueue_head(&sd->sd_waitq);
	slockinit(sd);
	init_MUTEX(&sd->sd_mutex);
}
static void shinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR)
		clear_shinfo(obj);
}
struct stdata *allocsd(void)
{
	struct strinfo *si = &Strinfo[DYN_STREAM];
	struct stdata *sd;
	if ((sd = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		struct shinfo *sh = (struct shinfo *) sd;
		atomic_set(&sh->sh_refs, 1);
#if defined(CONFIG_STREAMS_DEBUG) || defined (CONFIG_STREAMS_MNTSPECFS)
		{
			unsigned long flags;
			write_lock_irqsave(&si->si_rwlock, flags);
			list_add_tail((struct list_head *) &sh->sh_next, &si->si_head);
			write_unlock_irqrestore(&si->si_rwlock, flags);
		}
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
	}
	return (sd);
}
static void __freesd(struct stdata *sd)
{
	struct strinfo *si = &Strinfo[DYN_STREAM];
	struct shinfo *sh = (struct shinfo *) sd;
#if defined(CONFIG_STREAMS_DEBUG) || defined (CONFIG_STREAMS_MNTSPECFS)
	{
		unsigned long flags;
		write_lock_irqsave(&si->si_rwlock, flags);
		list_del_init((struct list_head *) &sh->sh_next);
		write_unlock_irqrestore(&si->si_rwlock, flags);
	}
#endif
	/* clear structure before putting it back */
	clear_shinfo(sh);
	atomic_dec(&si->si_cnt);
	kmem_cache_free(si->si_cache, sh);
}
void freesd(struct stdata *sd)
{
	/* FIXME: need to deallocate anything attached to the stream head */
	sd_put(sd);
}
struct stdata *sd_get(struct stdata *sd)
{
	if (sd) {
		struct shinfo *sh = (struct shinfo *) sd;
		if (atomic_read(&sh->sh_refs) <= 0)
			swerr();
		atomic_inc(&sh->sh_refs);
	}
	return (sd);
}
void sd_put(struct stdata *sd)
{
	if (sd) {
		struct shinfo *sh = (struct shinfo *) sd;
		if (atomic_dec_and_test(&sh->sh_refs)) {
			/* the last reference is gone, there should be nothing left */
			if (sd->sd_rq)
				freeq(sd->sd_rq);
			if (sd->sd_iocblk)
				freemsg(sd->sd_iocblk);
			__freesd(sd);
		}
	}
	swerr();
	return;
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Mountable Special Filesystem Interface
 *
 *  -------------------------------------------------------------------------
 */
#ifdef CONFIG_STREAMS_MNTSPECFS
 /* In case we are mounted and someone ls's us... */
static int spec_root_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct inode *inode = file->f_dentry->d_inode;
	struct strinfo *si = &Strinfo[DYN_STREAM];
	struct stdata *sd;
	struct list_head *pos, *tmp;
	char buf[64];
	off_t nr;
	nr = file->f_pos;
	switch (nr) {
	case 0:
		if (filldir(dirent, ".", 1, nr, inode->i_ino, DT_DIR) < 0)
			return (0);
		file->f_pos = ++nr;
	case 1:
		if (filldir(dirent, "..", 2, nr, inode->i_ino, DT_DIR) < 0)
			return (0);
		file->f_pos = ++nr;
	default:
	list_for_each_safe(pos, tmp, &si->si_head) {
		sd = (struct stdata *) list_entry(pos, struct shinfo, sh_next);
		if (!(inode = sd->sd_inode))
			continue;
		snprintf(buf, 64, "[%ld]", inode->i_ino);
		if (filldir(dirent, buf, strnlen(buf, 64), nr, inode->i_ino, DT_CHR) < 0)
			return (0);
		file->f_pos = ++nr;
	}
		break;
	}
	return (0);
}
struct file_operations spec_root_f_ops = {
	read:generic_read_dir,
	readdir:spec_root_readdir,
};
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  Kernel Memory Cache Initialization and Termination
 *
 *  -------------------------------------------------------------------------
 */

static struct cacheinfo {
	const char *name;
	size_t size;
	size_t offset;
	unsigned long flags;
	void (*ctor) (void *, kmem_cache_t *, unsigned long);
	void (*dtor) (void *, kmem_cache_t *, unsigned long);
} Cacheinfo[DYN_SIZE] = {
	{
	"DYN_STREAM", sizeof(struct shinfo), 0, SLAB_HWCACHE_ALIGN, &shinfo_ctor, NULL}, {
	"DYN_QUEUE", sizeof(struct queinfo), 0, SLAB_HWCACHE_ALIGN, &queinfo_ctor, NULL}, {
	"DYN_MSGBLOCK", 0, 0, SLAB_HWCACHE_ALIGN, NULL, NULL}, {
	"DYN_MDBBLOCK", sizeof(struct mdbblock), 0, SLAB_HWCACHE_ALIGN, &mdbblock_ctor, NULL}, {
	"DYN_LINKBLK", sizeof(struct linkinfo), 0, SLAB_HWCACHE_ALIGN, &linkinfo_ctor, NULL}, {
	"DYN_STREVENT", sizeof(struct seinfo), 0, SLAB_HWCACHE_ALIGN | SLAB_NO_REAP,
		    &seinfo_ctor, NULL}, {
	"DYN_QBAND", sizeof(struct qbinfo), 0, SLAB_HWCACHE_ALIGN, &qbinfo_ctor, NULL}, {
	"DYN_STRAPUSH", sizeof(struct apinfo), 0, SLAB_HWCACHE_ALIGN, &apinfo_ctor, NULL}, {
	"DYN_DEVINFO", sizeof(struct devinfo), 0, SLAB_HWCACHE_ALIGN, &devinfo_ctor, NULL}, {
	"DYN_MODINFO", sizeof(struct modinfo), 0, SLAB_HWCACHE_ALIGN, &modinfo_ctor, NULL}, {
	"DYN_SYNQ", sizeof(struct syncq), 0, SLAB_HWCACHE_ALIGN, &syncq_ctor, NULL}
};

/* Note: that we only have one cache for both MSGBLOCKs and MDBBLOCKs */

static void str_term_caches(void)
{
	int j;
	struct strinfo *si = Strinfo;
	struct cacheinfo *ci = Cacheinfo;
	for (j = 0; j < DYN_SIZE; j++, si++, ci++) {
		if (!si->si_cache)
			continue;
#ifdef CONFIG_STREAMS_DEBUG
		/* if we are tracking the allocations we can kill things whether they refer to each 
		   other or not. I hope we don't have any inodes kicking around... */
#endif
		if (kmem_cache_destroy(xchg(&si->si_cache, NULL)) == 0)
			continue;
		printk(KERN_ERR "%s: could not destroy %s cache\n", __FUNCTION__, ci->name);
		swerr();
	}
}

static int str_init_caches(void)
{
	int j;
	struct strinfo *si = Strinfo;
	struct cacheinfo *ci = Cacheinfo;
	for (j = 0; j < DYN_SIZE; j++, si++, ci++) {
#ifdef CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD(&si->si_head);
#endif
		rwlock_init(&si->si_rwlock);
		atomic_set(&si->si_cnt, 0);
		si->si_hwl = 0;
		if (si->si_cache != NULL)
			continue;
		if (ci->size == 0)
			continue;
		si->si_cache =
		    kmem_cache_create(ci->name, ci->size, ci->offset, ci->flags, ci->ctor,
				      ci->dtor);
		if (si->si_cache != NULL)
			continue;
		printk(KERN_ERR "%s: could not allocate %s cache\n", __FUNCTION__, ci->name);
		str_term_caches();
		return (-ENOMEM);
	}
	return (0);
}

int strsched_init(void)
{
	int result, i;
	if ((result = str_init_caches()) < 0)
		return (result);
	for (i = 0; i < NR_CPUS; i++) {
		struct strthread *t = &strthreads[i];
		/* initialize all the lists */
		t->qhead = NULL;
		t->qtail = &t->qhead;
		t->strbcalls_head = NULL;
		t->strbcalls_tail = &t->strbcalls_head;
		t->strtimout_head = NULL;
		t->strtimout_tail = &t->strtimout_head;
		t->strevents_head = NULL;
		t->strevents_tail = &t->strevents_head;
		t->scanqhead = NULL;
		t->scanqtail = &t->scanqhead;
		t->freemsg_head = NULL;
		t->freemsg_tail = &t->freemsg_head;
		t->freemblk_head = NULL;
		t->freemblk_tail = &t->freemblk_head;
		t->freeevnt_head = NULL;
		t->freeevnt_tail = &t->freeevnt_head;
	}
	open_softirq(STREAMS_SOFTIRQ, runqueues, NULL);
	return (0);
}

void strsched_exit(void)
{
	open_softirq(STREAMS_SOFTIRQ, NULL, NULL);
	str_term_caches();
}
