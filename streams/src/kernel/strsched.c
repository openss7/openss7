/*****************************************************************************

 @(#) $RCSfile: strsched.c,v $ $Name:  $($Revision: 0.9.2.57 $) $Date: 2005/07/27 20:34:12 $

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

 Last Modified $Date: 2005/07/27 20:34:12 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strsched.c,v $ $Name:  $($Revision: 0.9.2.57 $) $Date: 2005/07/27 20:34:12 $"

static char const ident[] =
    "$RCSfile: strsched.c,v $ $Name:  $($Revision: 0.9.2.57 $) $Date: 2005/07/27 20:34:12 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/compiler.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
#if HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
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

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/modules/sth.h"	/* for str_minfo */
#include "src/kernel/strsysctl.h"	/* for sysctl_str_ defs */
#include "src/kernel/strsched.h"	/* for in_stream */
#include "src/kernel/strutil.h"	/* for q locking and puts and gets */
#include "src/kernel/strsched.h"	/* verification of externs */

struct strthread strthreads[NR_CPUS] ____cacheline_aligned;
struct strinfo Strinfo[DYN_SIZE] ____cacheline_aligned;

#if HAVE_RAISE_SOFTIRQ_IRQOFF_EXPORT && ! HAVE_RAISE_SOFTIRQ_EXPORT
void fastcall
raise_softirq(unsigned int nr)
{
	unsigned long flags;

	local_irq_save(flags);
	raise_softirq_irqoff(nr);
	local_irq_restore(flags);
}
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  MDBBLOCK ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void
mdbblock_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct mdbblock *md = obj;

		bzero(md, sizeof(*md));
		atomic_set(&md->datablk.d_dblock.db_users, 0);
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD(&md->msgblk.m_list);
		INIT_LIST_HEAD(&md->datablk.db_list);
#endif
	}
}

/*
 *  queue_guess: - guess the current queue being processed
 *  @q: current queue, if known, NULL otherwise
 *
 *  Use the current queue being processed by the thread as a guess as to which queue the thing
 *  (message block, bufcall callback, timout callback) belongs to if the supplied queue is %NULL,
 *  but, if we are at irq, then we will only use the supplied value, even if %NULL.
 */

static queue_t *
queue_guess(queue_t *q)
{
	queue_t *guess;

	if ((guess = q) == NULL && current_context() == CTX_STREAMS)
		guess = this_thread->currentq;
	return (guess);
}

/**
 *  mdbblock_alloc: - allocate a combined message/data block
 *  @q: the queue with which to associate the allocation for debugging
 *  @priority: the priority of the allocation
 *  @func: allocating function pointer
 *
 *  This is not exported, but is called by the streams D3DK functions allocb(), dupb(), eballoc()
 *  and pullupmsg() to allocate a combined message/data block.  Because mdbblock_free() does a fast
 *  free to the free list, we first check if a mdbblock is sitting on the per-cpu free list.  If
 *  there is no memory block available then we take one from the memory cache.  Unfortunately we
 *  must shut out interrupts briefly otherwise an ISR running on the same processor freeing while we
 *  are allocating will hose the free list.
 *
 *  We reimplement the SVR3 priority scheme with a twist.  BPRI_HI is allowed to use the current
 *  free list for high priority tasks.  BPRI_MED is not allowed the free list but may grow the cache
 *  if necessary to get a block.  BPRI_LO is only allowed to get a free block from the cache and is
 *  not allowed to grow the cache.
 *
 *  Return Values: This function will return NULL when there are no more blocks.
 *
 *  Because misbehaving STREAMS modules and drivers normaly leak message blocks at an amazing rate,
 *  we also return an allocation failure if the number of message blocks exceeds a tunable
 *  threshold.
 */
mblk_t *
mdbblock_alloc(uint priority, void *func)
{
	struct strthread *t = this_thread;
	struct strinfo *sdi = &Strinfo[DYN_MDBBLOCK];
	mblk_t *mp = NULL;
	int slab_flags;

	if (atomic_read(&sdi->si_cnt) > sysctl_str_nstrmsgs)
		goto fail;
	switch (priority) {
	case BPRI_HI:
	{
		unsigned long flags;

		local_irq_save(flags);	/* MP-SAFE */
		/* This atomic exchange sequence could break if an ISR is freeing blocks at the
		   same time as this is executing, and we allow freeb() to be called from ISR, so
		   we suppress interrupts. */
		if ((mp = xchg(&t->freemblk_head, NULL)))
			t->freemblk_head = xchg(&mp->b_next, NULL);
		local_irq_restore(flags);
		if (mp != NULL)
			return (mp);
	}
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
#if defined CONFIG_STREAMS_DEBUG
		struct strinfo *smi = &Strinfo[DYN_MSGBLOCK];
		struct mdbblock *md = (struct mdbblock *) mp;
		unsigned long flags;

		md->msgblk.m_func = func;
		md->msgblk.m_queue = qget(queue_guess(NULL));
		write_lock_irqsave(&smi->si_rwlock, flags);
		list_add_tail(&md->msgblk.m_list, &smi->si_head);
		list_add_tail(&md->datablk.db_list, &sdi->si_head);
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

/*
 *  raise_local_bufcalls: - raise buffer callbacks on the local STREAMS scheduler thread.
 */
static void
raise_local_bufcalls(void)
{
	struct strthread *t = this_thread;

	if (test_bit(strbcwait, &t->flags))
		if (!test_and_set_bit(strbcflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
}

/*
 *  raise_bufcalls: - raise buffer callbacks on all STREAMS scheduler threads
 *
 *  Raise buffer callbacks on all STREAMS scheduler threads.
 *
 *  NOTICES: Unfortunately, more recent 2.6 kernels have gotten rid of the cpu_raise_softirq() so
 *  that it is not easy to raise a softirq to schedule a STREAMS sheduler thread on a differnt CPU.
 *  I'm not quite sure what this is so.  There may be some more differences between the softirq
 *  mechanism on 2.6 and on 2.4 that I do not know about.  Likely it is just that 2.6's new
 *  per-thread information cannot be indexed as easily as it used to be on 2.4.
 */
static void
raise_bufcalls(void)
{
#if HAVE_KFUNC_CPU_RAISE_SOFTIRQ
	struct strthread *t;
	int i;

	for (i = 0, t = &strthreads[0]; i < NR_CPUS; i++, t++)
		if (test_bit(strbcwait, &t->flags))
			if (!test_and_set_bit(strbcflag, &t->flags))
				cpu_raise_softirq(i, STREAMS_SOFTIRQ);
#else
	return raise_local_bufcalls();
#endif
}

/**
 *  mdbblock_free: - free a combined message/data block
 *  @mp:    the mdbblock to free
 *
 *  This is not exported but is called by the streams D3DK function freeb() when freeing a &struct
 *  mddbblock.  For speed, we just link the block into the per-cpu free list.  Other queue
 *  proceudres calling allocb() or esballoc() in the same runqueues() pass can use them without
 *  locking the memory caches.  At the end of the runqueues() pass, any remaining blocks will be
 *  freed back to the kmem cache.  Whenever we place the first block on the free list or whenever
 *  there are streams waiting on buffers, we raise the softirq to ensure that another runqueues()
 *  pass will occur.
 *
 *  This function uses the fact that it is using a per-cpu list to avoid locking.  It uses the
 *  atomic xchg() function to ensure the integrity of the list while interrupts are still enabled.
 *  
 *  To reduce latency on allocation to a minimum, we null the state of the blocks when they are
 *  placed to the free list rather than when they are allocated.
 */
void
mdbblock_free(mblk_t *mp)
{
	struct strthread *t = this_thread;
	struct mdbblock *md = (typeof(md)) mp;

	/* don't zero hidden list structures */
	bzero(&md->datablk.d_dblock, sizeof(md->datablk.d_dblock));
	bzero(&md->msgblk.m_mblock, sizeof(md->msgblk.m_mblock));
	md->msgblk.m_func = NULL;
	md->msgblk.m_queue = NULL;
	bzero(md, sizeof(*md));	/* reset mdbblock */
	*xchg(&t->freemblk_tail, &mp->b_next) = mp;	/* MP-SAFE */
	if (!test_and_set_bit(freeblks, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
	raise_local_bufcalls();
	return;
}

/* 
 *  freeblocks: - free message blocks
 *  @t:	    the STREAMS executive thread
 *
 *  freeblocks() is invoked by runqueues() when there are blocks to free (i.e.  the %FREEBLKS flag is
 *  set on the thread).  It frees all blocks from the free list.  We first steal the entire list and
 *  then work on them one by one.  After we free something, we want to raise pending buffer
 *  callbacks on all STREAMS scheduler threads in an attempt to let them use the freed blocks.
 */
static void
freeblocks(struct strthread *t)
{
	register mblk_t *mp, *mp_next;

	clear_bit(freeblks, &t->flags);
	if ((mp_next = xchg(&t->freemblk_head, NULL))) {	/* MP-SAFE */
		t->freemblk_tail = &t->freemblk_head;
		while ((mp = mp_next)) {
			struct strinfo *sdi = &Strinfo[DYN_MDBBLOCK];

#if defined CONFIG_STREAMS_DEBUG
			struct strinfo *smi = &Strinfo[DYN_MSGBLOCK];
			unsigned long flags;
			struct mdbblock *md = (struct mdbblock *) mp;

			write_lock_irqsave(&smi->si_rwlock, flags);
			list_del_init(&md->msgblk.m_list);
			list_del_init(&md->datablk.db_list);
			write_unlock_irqrestore(&smi->si_rwlock, flags);
			atomic_dec(&smi->si_cnt);
#endif
			mp_next = xchg(&mp->b_next, NULL);
			mp->b_prev = NULL;
			kmem_cache_free(sdi->si_cache, mp);
			atomic_dec(&sdi->si_cnt);
		}
		raise_bufcalls();
	}
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  QBAND ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void
qbinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct qbinfo *qbi = obj;

		bzero(qbi, sizeof(*qbi));
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD(&qbi->qbi_list);
#endif
	}
}
struct qband *
allocqb(void)
{
	struct qband *qb;
	struct strinfo *si = &Strinfo[DYN_QBAND];

	if ((qb = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		struct qbinfo *qbi = (struct qbinfo *) qb;

		atomic_set(&qbi->qbi_refs, 1);
#if defined CONFIG_STREAMS_DEBUG
		{
			unsigned long flags;

			write_lock_irqsave(&si->si_rwlock, flags);
			list_add_tail(&qbi->qbi_list, &si->si_head);
			write_unlock_irqrestore(&si->si_rwlock, flags);
		}
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
	}
	return (qb);
}

void
freeqb(struct qband *qb)
{
	struct strinfo *si = &Strinfo[DYN_QBAND];

#if defined CONFIG_STREAMS_DEBUG
	unsigned long flags;
	struct qbinfo *qbi = (struct qbinfo *) qb;

	write_lock_irqsave(&si->si_rwlock, flags);
	list_del_init(&qbi->qbi_list);
	write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
	atomic_dec(&si->si_cnt);
	/* clean it up before putting it back in the cache */
	bzero(qb, sizeof(*qb));
	kmem_cache_free(si->si_cache, qb);
}
static void
__freebands(queue_t *q)
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
static void
apinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct apinfo *api = obj;

		bzero(api, sizeof(*api));
		INIT_LIST_HEAD(&api->api_more);
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD(&api->api_list);
#endif
	}
}
struct apinfo *
ap_alloc(struct strapush *sap)
{
	struct apinfo *api;
	struct strinfo *si = &Strinfo[DYN_STRAPUSH];

	if ((api = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
#if defined CONFIG_STREAMS_DEBUG
		unsigned long flags;

		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail(&api->api_list, &si->si_head);
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
struct apinfo *
ap_get(struct apinfo *api)
{
	if (api) {
		if (atomic_read(&api->api_refs) < 1)
			swerr();
		atomic_inc(&api->api_refs);
	}
	return (api);
}

void
ap_put(struct apinfo *api)
{
	if (api) {
		if (atomic_dec_and_test(&api->api_refs)) {
			struct strinfo *si = &Strinfo[DYN_STRAPUSH];

#if defined CONFIG_STREAMS_DEBUG
			unsigned long flags;

			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init(&api->api_list);
			write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
			atomic_dec(&si->si_cnt);
			list_del_init(&api->api_more);
			/* clean it up before putting it back in the cache */
			bzero(api, sizeof(*api));
			INIT_LIST_HEAD(&api->api_more);
#if defined CONFIG_STREAMS_DEBUG
			INIT_LIST_HEAD(&api->api_list);
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
static void
devinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct devinfo *di = obj;

		bzero(di, sizeof(*di));
		INIT_LIST_HEAD(&di->di_list);
		INIT_LIST_HEAD(&di->di_hash);
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &di->di_next);
#endif
	}
}
struct devinfo *
di_alloc(struct cdevsw *cdev)
{
	struct devinfo *di;
	struct strinfo *si = &Strinfo[DYN_DEVINFO];

	if ((di = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
#if defined CONFIG_STREAMS_DEBUG
		unsigned long flags;

		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail((struct list_head *) &di->di_next, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
		atomic_set(&di->di_refs, 1);
		di->di_dev = cdev;
		atomic_set(&di->di_count, 0);
		di->di_info = cdev->d_str->st_rdinit->qi_minfo;
	}
	return (di);
}

#if defined CONFIG_STREAMS_STH_MODULE || !defined CONFIG_STREAMS_STH
EXPORT_SYMBOL(di_alloc);	/* include/sys/streams/strsubr.h */
#endif
struct devinfo *
di_get(struct devinfo *di)
{
	if (di) {
		if (atomic_read(&di->di_refs) < 1)
			swerr();
		atomic_inc(&di->di_refs);
	}
	return (di);
}

void
di_put(struct devinfo *di)
{
	if (di) {
		if (atomic_dec_and_test(&di->di_refs)) {
			struct strinfo *si = &Strinfo[DYN_DEVINFO];

#if defined CONFIG_STREAMS_DEBUG
			unsigned long flags;

			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &di->di_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
			atomic_dec(&si->si_cnt);
			list_del_init(&di->di_list);
			list_del_init(&di->di_hash);
			/* clean it up before putting it back in the cache */
			bzero(di, sizeof(*di));
			INIT_LIST_HEAD(&di->di_list);
			INIT_LIST_HEAD(&di->di_hash);
#if defined CONFIG_STREAMS_DEBUG
			INIT_LIST_HEAD((struct list_head *) &di->di_next);
#endif
			kmem_cache_free(si->si_cache, di);
		}
		return;
	}
	swerr();
}

#if defined CONFIG_STREAMS_STH_MODULE || !defined CONFIG_STREAMS_STH
EXPORT_SYMBOL(di_put);		/* include/sys/streams/strsubr.h */
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  MODINFO ctors and dtors
 *
 *  -------------------------------------------------------------------------
 */
static void
mdlinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct mdlinfo *mi = obj;

		bzero(mi, sizeof(*mi));
		INIT_LIST_HEAD(&mi->mi_list);
		INIT_LIST_HEAD(&mi->mi_hash);
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD((struct list_head *) &mi->mi_next);
#endif
	}
}
struct mdlinfo *
modi_alloc(struct fmodsw *fmod)
{
	struct mdlinfo *mi;
	struct strinfo *si = &Strinfo[DYN_MODINFO];

	if ((mi = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
#if defined CONFIG_STREAMS_DEBUG
		unsigned long flags;

		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail((struct list_head *) &mi->mi_next, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
		atomic_set(&mi->mi_refs, 1);
		mi->mi_mod = fmod;
		atomic_set(&mi->mi_count, 0);
		mi->mi_info = fmod->f_str->st_rdinit->qi_minfo;
	}
	return (mi);
}
struct mdlinfo *
modi_get(struct mdlinfo *mi)
{
	if (mi) {
		if (atomic_read(&mi->mi_refs) < 1)
			swerr();
		atomic_inc(&mi->mi_refs);
	}
	return (mi);
}

void
modi_put(struct mdlinfo *mi)
{
	if (mi) {
		if (atomic_dec_and_test(&mi->mi_refs)) {
			struct strinfo *si = &Strinfo[DYN_MODINFO];

#if defined CONFIG_STREAMS_DEBUG
			unsigned long flags;

			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &mi->mi_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
			atomic_dec(&si->si_cnt);
			list_del_init(&mi->mi_list);
			list_del_init(&mi->mi_hash);
			/* clean it up before putting it back in the cache */
			bzero(mi, sizeof(*mi));
			INIT_LIST_HEAD(&mi->mi_list);
			INIT_LIST_HEAD(&mi->mi_hash);
#if defined CONFIG_STREAMS_DEBUG
			INIT_LIST_HEAD((struct list_head *) &mi->mi_next);
#endif
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
 *  __rmvq:	- remove a message from a queue
 *  @q:		queue from which to remove the message
 *  @mp:	message to remove
 */
static int
__rmvq(queue_t *q, mblk_t *mp)
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

/*
 *  __flushq:	- flush messages from a queue
 *  @q:		queue from which to flush messages
 *  @flag:	how, %FLUSHDATA or %FLUSHALL
 *  @mppp:	pointer to a pointer to the end of a message chain
 */
static int
__flushq(queue_t *q, int flag, mblk_t ***mppp)
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

void freechain(mblk_t *mp, mblk_t **mpp);

/**
 *  flushq:	- flush messages from a queue
 *  @q:		the queue to flush
 *  @flag:	how to flush: %FLUSHDATA or %FLUSHALL
 */
void
flushq(queue_t *q, int flag)
{
	int backenable;
	mblk_t *mp = NULL, **mpp = &mp;

	ensure(q, return);
	qwlock_bh(q);
	backenable = __flushq(q, flag, &mpp);
	qwunlock_bh(q);
	if (backenable)
		qbackenable(q);
	/* we want to free messages with the locks off so that other CPUs can process this queue
	   and we don't block interrupts too long */
	freechain(mp, mpp);
}

EXPORT_SYMBOL(flushq);		/* include/sys/streams/stream.h */

/* 
 *  -------------------------------------------------------------------------
 *
 *  QUEUE ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void
queinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
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
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD(&qu->qu_list);
#endif
	}
}

/**
 *  allocq:	- allocate a queue pair
 *
 *  Can be called by the module writer to get a private queue pair.
 */
queue_t *
allocq(void)
{
	queue_t *rq;
	struct strinfo *si = &Strinfo[DYN_QUEUE];

	if ((rq = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		queue_t *wq = rq + 1;
		struct queinfo *qu = (struct queinfo *) rq;

		atomic_set(&qu->qu_refs, 1);
		rq->q_other = wq;
		wq->q_other = rq;
#if defined CONFIG_STREAMS_DEBUG
		{
			unsigned long flags;

			write_lock_irqsave(&si->si_rwlock, flags);
			list_add_tail(&qu->qu_list, &si->si_head);
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

EXPORT_SYMBOL(allocq);		/* include/sys/streams/stream.h */

/*
 *  __freeq:	- free a queue pair
 *  @rq:	read queue of queue pair
 *
 *  Frees a queue pair allocated with allocq().  Does not flush messages or clear use bits.
 */
static void
__freeq(queue_t *rq)
{
	queue_t *wq = rq + 1;
	struct strinfo *si = &Strinfo[DYN_QUEUE];

#if defined CONFIG_STREAMS_DEBUG
	unsigned long flags;
	struct queinfo *qu = (struct queinfo *) rq;

	write_lock_irqsave(&si->si_rwlock, flags);
	list_del_init(&qu->qu_list);
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

void freechain(mblk_t *mp, mblk_t **mpp);

/**
 *  freeq:	- free a queue pair
 *  @rq:	read queue of queue pair
 *
 *  Can be called by the module writer on private queue pairs allocated with allocq().  All message
 *  blocks will be flushed.
 */
void
freeq(queue_t *rq)
{
	queue_t *wq = rq + 1;
	mblk_t *mp = NULL, **mpp = &mp;

	clear_bit(QUSE_BIT, &rq->q_flag);
	clear_bit(QUSE_BIT, &wq->q_flag);
	__flushq(rq, FLUSHALL, &mpp);
	__flushq(wq, FLUSHALL, &mpp);
	__freebands(rq);
	__freebands(wq);
	__freeq(rq);
	freechain(mp, mpp);
}

EXPORT_SYMBOL(freeq);		/* include/sys/streams/stream.h */

/* 
 *  -------------------------------------------------------------------------
 *
 *  LINKBLK ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void
linkinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		static spinlock_t link_index_lock = SPIN_LOCK_UNLOCKED;
		static int link_index = 0;
		unsigned long flags;
		struct linkinfo *li = obj;
		struct linkblk *l = &li->li_linkblk;

		bzero(li, sizeof(*li));
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD(&li->li_list);
#endif
		/* XXX: are these strict locks necessary? */
		spin_lock_irqsave(&link_index_lock, flags);
		l->l_index = ++link_index;
		if (++link_index < 1)
			link_index = 1;
		spin_unlock_irqrestore(&link_index_lock, flags);
	}
}
struct linkblk *
alloclk(void)
{
	struct linkblk *l;
	struct strinfo *si = &Strinfo[DYN_LINKBLK];

	if ((l = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
#if defined CONFIG_STREAMS_DEBUG
		unsigned long flags;
		struct linkinfo *li = (struct linkinfo *) l;

		write_lock_irqsave(&si->si_rwlock, flags);
		list_add_tail(&li->li_list, &si->si_head);
		write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
	}
	return (l);
}

#if defined CONFIG_STREAMS_STH_MODULE || !defined CONFIG_STREAMS_STH
EXPORT_SYMBOL(alloclk);		/* include/sys/streams/strsubr.h */
#endif

void
freelk(struct linkblk *l)
{
	struct strinfo *si = &Strinfo[DYN_LINKBLK];
	struct linkinfo *li = (struct linkinfo *) l;

#if defined CONFIG_STREAMS_DEBUG
	{
		unsigned long flags;

		write_lock_irqsave(&si->si_rwlock, flags);
		list_del_init(&li->li_list);
		write_unlock_irqrestore(&si->si_rwlock, flags);
	}
#endif
	atomic_dec(&si->si_cnt);
	kmem_cache_free(si->si_cache, li);
}

#if defined CONFIG_STREAMS_STH_MODULE || !defined CONFIG_STREAMS_STH
EXPORT_SYMBOL(freelk);		/* include/sys/streams/strsubr.h */
#endif

#if defined CONFIG_STREAMS_SYNCQS
/* 
 *  -------------------------------------------------------------------------
 *
 *  SYNCQ ctos and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */

static void
syncq_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
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
struct syncq *
sq_alloc(void)
{
	struct syncq *sq;
	struct strinfo *si = &Strinfo[DYN_SYNCQ];

	if ((sq = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
		unsigned long flags;

		/* XXX: are these strict locks necessary? */
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
struct syncq *
sq_get(struct syncq *sq)
{
	if (sq) {
		if (atomic_read(&sq->sq_refs) < 1)
			swerr();
		atomic_inc(&sq->sq_refs);
	}
	return (sq);
}

void
sq_put(struct syncq **sqp)
{
	struct syncq *sq;

	if ((sq = xchg(sqp, NULL))) {
		if (atomic_dec_and_test(&sq->sq_refs)) {
			struct strinfo *si = &Strinfo[DYN_SYNCQ];
			unsigned long flags;

			/* XXX: are these strict locks necessary? */
			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &sq->sq_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
			/* clean it up before puting it back in the cache */
			sq_put(&sq->sq_other);	/* recurse once */
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
#endif

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
static void
seinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR) {
		struct seinfo *s = obj;
		struct strevent *se = obj;
		unsigned long flags;

		bzero(s, sizeof(*s));
#if defined CONFIG_STREAMS_DEBUG
		INIT_LIST_HEAD(&s->s_list);
#endif
		/* XXX: are these strict locks necessary? */
		write_lock_irqsave(&event_hash_lock, flags);
		se->se_id = event_id;
		se->se_prev = xchg(&event_hash[event_id & EVENT_HASH_MASK], se);
		event_id++;
		write_unlock_irqrestore(&event_hash_lock, flags);
	}
}
static inline struct strevent *
event_alloc(int type, queue_t *q)
{
	struct strinfo *si = &Strinfo[DYN_STREVENT];
	struct strevent *se = NULL;

	if (atomic_read(&si->si_cnt) < EVENT_LIMIT) {
		if ((se = kmem_cache_alloc(si->si_cache, SLAB_ATOMIC))) {
			struct seinfo *s = (struct seinfo *) se;

			s->s_type = type;
#if defined CONFIG_STREAMS_DEBUG
			{
				unsigned long flags;

				s->s_queue = qget(queue_guess(q));
				write_lock_irqsave(&si->si_rwlock, flags);
				list_add_tail(&s->s_list, &si->si_head);
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
static inline void
event_free(struct strevent *se)
{
	struct strinfo *si = &Strinfo[DYN_STREVENT];

#if defined CONFIG_STREAMS_DEBUG
	unsigned long flags;
	struct seinfo *s = (struct seinfo *) se;

	qput(&s->s_queue);
	write_lock_irqsave(&si->si_rwlock, flags);
	list_del_init(&s->s_list);
	write_unlock_irqrestore(&si->si_rwlock, flags);
#endif
	atomic_dec(&si->si_cnt);
	se->se_seq++;
	kmem_cache_free(si->si_cache, se);
}
static struct strevent *
find_event(int event_id)
{
	struct strevent **sep;
	int id = (event_id >> EVENT_ID_SHIFT) & EVENT_ID_MASK;
	int seq = event_id & EVENT_SEQ_MASK;
	unsigned long flags;

	sep = &event_hash[id & EVENT_HASH_MASK];
	/* XXX: are these strict locks necessary? */
	read_lock_irqsave(&event_hash_lock, flags);
	for (; *sep; sep = &(*sep)->se_prev)
		if ((*sep)->se_id == id && (*sep)->se_seq == seq)
			break;
	read_unlock_irqrestore(&event_hash_lock, flags);
	return (*sep);
}

/**
 *  sealloc:	- allocate a stream event structure
 */
struct strevent *
sealloc(void)
{
	return event_alloc(SE_STREAM, NULL);
}

EXPORT_SYMBOL(sealloc);		/* include/sys/streams/strsubr.h */

/**
 *  sefree:	- deallocate a stream event structure
 *  @se:	the stream event structure to deallocate
 */
int
sefree(struct strevent *se)
{
	event_free(se);
	return (0);
}

EXPORT_SYMBOL(sefree);		/* include/sys/streams/strsubr.h */

/*
 *  -------------------------------------------------------------------------
 *
 *  Event scheduling.
 *
 *  -------------------------------------------------------------------------
 */

/*
 * schedule_event:	- schedule an event for the STREAMS scheduler
 * @se:			the event to schedule
 *
 * This is how we schedule general purpose events.  Most of these events are only scheduled because
 * they are always deferred (such as qwriter()) for execution some time in the future.  Rather than
 * attempting to acquire the necessary locks at this point, which could lead to deadlock situations,
 * we defer the event and execute it directly from the STREAMS scheduler instead of nested in other
 * procedures that could lead to deadlock.
 *
 * The executed callout functions have the following characteristics:
 *
 * - The function is not executed until the function calling this deferral function from the STREAMS
 *   environment returns back to the STREAMS scheduler.  This is also true for deferals invoked by
 *   interrupt service routines, softirq processing.  When invoked from user context, the deferred
 *   function could start execution before the caller returns.
 *
 * - The deferred function is intially queued against the STREAMS scheduler.  When the STREAMS
 *   scheduler goes to service the event, attempts will be made to acquire the appropriate
 *   synchronization before deferred function execution.  If an attempt fails to enter a barrier,
 *   the STREAMS event will be queued against the barrier.  Events queued against barriers are
 *   processed once the threads raising the barrier exit.  When processed in this way, the events
 *   are processed by the exiting thread.  For the most part, threads exiting barriers are STREAMS
 *   scheduler threads.  Process context threads exiting the outer perimeter will pass the
 *   synchronization queue back to the STREAMS scheduler for backlog processing.
 */
static void
schedule_event(struct strevent *se)
{
	struct strthread *t = this_thread;

	*xchg(&t->strevents_tail, &se->se_next) = se;	/* MP-SAFE */
	if (!test_and_set_bit(strevents, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
}

/*
 * schedule_bufcall:	- schedule an buffer callback for the STREAMS scheduler
 * @se:			the buffer callback to schedule
 *
 * This function schedules a buffer callback for future execution.  All buffer callbacks invoked on
 * the current processor are kept in a list against the current STREAMS scheduler thread.  Whenever,
 * kmem_free() frees memory of @size or greater, it sets the flag to run buffer callbacks and wakes
 * the scheduler thread.
 */
static void
schedule_bufcall(struct strevent *se)
{
	struct strthread *t = this_thread;

	*xchg(&t->strbcalls_tail, &se->se_next) = se;	/* MP-SAFE */
	set_bit(strbcwait, &t->flags);
}

/*
 * schedule_timeout:	- schedule a timer for the STREAMS scheduler
 * @se:			the timer to schedule
 */
static void
schedule_timeout(struct strevent *se)
{
	se->x.t.timer.data = (long) se;
	se->x.t.timer.function = timeout_function;
	add_timer(&se->x.t.timer);
}

static struct strevent *
defer_stream_event()
{
	struct strevent *se;

	if ((se = event_alloc(SE_STREAM, q))) {
		schedule_event(se);
	}
	return (se);
}
static struct strevent *
defer_bufcall_event(queue_t *q, unsigned size, int priority, void (*func) (long), long arg)
{
	struct strevent *se;

	if ((se = event_alloc(SE_BUFCALL, q))) {
		se->x.b.queue = qget(q);
		se->x.b.func = func;
		se->x.b.arg = arg;
		se->x.b.size = size;
		schedule_bufcall(se);
	}
	return (se);
}
static struct strevent *
defer_timeout_event(queue_t *q, timo_fcn_t *func, caddr_t arg, long ticks, unsigned long pl,
		    int cpu)
{
	struct strevent *se;

	if ((se = event_alloc(SE_TIMEOUT, q))) {
		se->x.t.queue = qget(q);
		se->x.t.func = func;
		se->x.t.arg = arg;
		se->x.t.pl = pl;
		se->x.t.cpu = cpu;
		se->x.t.timer.expires = jiffies + ticks;
		schedule_timeout(se);
	}
	return (se);
}
static struct strevent *
defer_weldq_event(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func,
		  weld_arg_t arg, queue_t *q)
{
	struct strevent *se;

	if ((se = event_alloc(SE_WELDQ, q))) {
		se->x.w.queue = qget(q);
		se->x.w.func = func;
		se->x.w.arg = arg;
		se->x.w.q1 = getq(q1);
		se->x.w.q2 = getq(q2);
		se->x.w.q3 = getq(q3);
		se->x.w.q4 = getq(q4);
		schedule_event(se);
	}
	return (se);
}
static struct strevent *
defer_unweldq_event(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func,
		    weld_arg_t arg, queue_t *q)
{
	struct strevent *se;

	if ((se = event_alloc(SE_UNWELDQ, q))) {
		se->x.w.queue = qget(q);
		se->x.w.func = func;
		se->x.w.arg = arg;
		se->x.w.q1 = getq(q1);
		se->x.w.q2 = getq(q2);
		se->x.w.q3 = getq(q3);
		se->x.w.q4 = getq(q4);
		schedule_event(se);
	}
	return (se);
}

/*
 *  __bufcall:	- generate a buffer callback
 *  @q:		queue against which to synchronize callback
 *  @size:	size of message block requested
 *  @priority:	priority of message block request
 *  @function:	callback function
 *  @arg:	argument to callback function
 *
 *  Notices: Note that, for MP safety, bufcalls are always raised against the same processor that
 *  invoked the buffer call.  This means that the callback function (at least on 2.4 kernels) will
 *  not execute until after the caller exits or hits a pre-emption point.
 */
bcid_t
__bufcall(queue_t *q, unsigned size, int priority, void (*function) (long), long arg)
{
	bcid_t bcid = 0;
	struct strevent *se;

	if ((se = event_alloc(SE_BUFCALL, q))) {
		struct strthread *t = this_thread;

		se->x.b.queue = qget(q);	/* hold a reference */
		se->x.b.func = function;
		se->x.b.arg = arg;
		se->x.b.size = size;
		bcid = ((se->se_id << EVENT_ID_SHIFT) | (se->se_seq & EVENT_SEQ_MASK));
		*xchg(&t->strbcalls_tail, &se->se_next) = se;	/* MP-SAFE */
		set_bit(strbcwait, &t->flags);
	}
	return (bcid);
}

EXPORT_SYMBOL(__bufcall);	/* include/sys/streams/strsubr.h */

/**
 *  bufcall:	- schedule a buffer callout
 *  @size:	the number of bytes of data buffer needed
 *  @priority:	the priority of the buffer allocation (ignored)
 *  @function:	the callback function when bytes and headers are available
 *  @arg:	a client argument to pass to the callback function
 */
bcid_t
bufcall(unsigned size, int priority, void (*function) (long), long arg)
{
	return __bufcall(queue_guess(NULL), size, priority, function, arg);
}

EXPORT_SYMBOL(bufcall);		/* include/sys/streams/stream.h */

/**
 *  esbbcall:	- schedule a buffer callout
 *  @priority:	the priority of the buffer allocation (ignored)
 *  @function:	the callback function when bytes and headers are available
 *  @arg:	a client argument to pass to the callback function
 */
__EXTERN_INLINE bcid_t esbbcall(int priority, void (*function) (long), long arg);

EXPORT_SYMBOL(esbbcall);	/* include/sys/streams/stream.h */

/**
 *  unbufcall:	- cancel a buffer callout
 *  @bcid:	buffer call id returned by bufcall() or esbbcall()
 *  Notices:	Don't ever call this function with an expired bufcall id.
 */
void
unbufcall(bcid_t bcid)
{
	struct strevent *se;

	if ((se = find_event(bcid)))
		xchg(&se->x.b.func, NULL);
}

EXPORT_SYMBOL(unbufcall);	/* include/sys/streams/stream.h */

/*
 *  timeout_function: - execute a linux kernel timer timeout
 *  @arg:	a pointer to the STREAMS event structure
 *
 *  When a kernel timer times out we link the original timer STREAMS event structure into the cpu
 *  list on the invoking cpu and wake up the STREAMS scheduler on that cpu.  The only problem with
 *  this approach is that recent 2.6 kernels no longer provide the ability to raise a soft irq on a
 *  different cpu.
 *
 *  Invoking the STREAMS scheduler in this way causes the timeout to be handled within the STREAMS
 *  scheduler environment.  Further deferral of the timeout callback might occur if processing of
 *  the timeout fails to enter the synchronization queues, if any.
 */
static void
timeout_function(unsigned long arg)
{
	struct strevent *se = (struct strevent *) arg;

#if HAVE_KFUNC_CPU_RAISE_SOFTIRQ
	struct strthread *t = &strthreads[se->x.t.cpu];
#else
	struct strthread *t = this_thread;
#endif

	*xchg(&t->strtimout_tail, &se->se_next) = se;	/* MP-SAFE */
	if (!test_and_set_bit(strtimout, &t->flags))
#if HAVE_KFUNC_CPU_RAISE_SOFTIRQ
		/* bind timeout back to the CPU that called for it */
		cpu_raise_softirq(se->x.t.cpu, STREAMS_SOFTIRQ);
#else
		raise_softirq(STREAMS_SOFTIRQ);
#endif
}

toid_t
__timeout(queue_t *q, timo_fcn_t *timo_fcn, caddr_t arg, long ticks, unsigned long pl, int cpu)
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
		toid = ((se->se_id << EVENT_ID_SHIFT) | (se->se_seq & EVENT_SEQ_MASK));
		add_timer(&se->x.t.timer);
	}
	return (toid);
}

EXPORT_SYMBOL(__timeout);	/* include/sys/streams/strsubr.h */

/**
 *  timeout:	- issue a timeout callback
 *  @arg:	client data
 *
 *  Notices:	Note that, for MP safety, timeouts are always raised against the same processor that
 *  invoked the timeout.  This means that the callback function will not execute until after the
 *  caller hits a pre-emption point.
 */
toid_t
timeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks)
{
	return __timeout(queue_guess(NULL), timo_fcn, arg, ticks, 0, smp_processor_id());
}

EXPORT_SYMBOL(timeout);		/* include/sys/streams/stream.h */

/**
 *  untimeout:	- cancel a timeout callback
 *  @toid:	timeout identifier
 */
clock_t
untimeout(toid_t toid)
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

EXPORT_SYMBOL(untimeout);	/* include/sys/streams/stream.h */

/*
 *  __weldq:	- weld/unweld two queue pairs together/apart
 *  @q1:	first queue to weld/unweld
 *  @q2:	second queue to weld/unweld
 *  @q3:	third queue to weld/unweld
 *  @q4:	forth queue to weld/unweld
 *  @func:	callback function after weld/unweld complete
 *  @arg:	argument to callback function
 *  @protq:	queue to use for synchronization
 *  @type:	event type (%SE_WELD or %SE_UNWELD)
 *
 *  Issues the STREAMS event necessary to weld two queue pairs together with synchronization.
 */
static int
__weldq(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func,
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
		*xchg(&t->strevents_tail, &se->se_next) = se;	/* MP-SAFE */
		if (!test_and_set_bit(strevents, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
		return (0);
	}
	return (EAGAIN);
}

/**
 *  weldq:	- weld two queue pairs together
 *  @q1:	first queue to weld (to @q3)
 *  @q2:	second queue to weld (to @q4)
 *  @q3:	third queue to weld (from @q1)
 *  @q4:	forth queue to weld (from @q2)
 *  @func:	callback function after weld complete
 *  @arg:	argument to callback function
 *  @protq:	queue to use for synchronization
 *
 *  weldq() ssues a request to the STREAMS schedule to weld two queue pairs together (@q1 to @q3 and
 *  @q4 to @q2) with synchronization against @protq.  Once the weld is complete the @func callback
 *  with be called with argument @arg.
 *
 *  Notices: The @func callback function will be called by the same CPU upon which weldq() was issued.
 */
int
weldq(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func, weld_arg_t arg,
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

EXPORT_SYMBOL(weldq);		/* include/sys/streams/stream.h */

/**
 *  unweldq:	- unweld two queue pairs from each other
 *  @q1:	first queue to unweld (from @q3)
 *  @q2:	second queue to unweld (from @q4)
 *  @q3:	third queue to unweld
 *  @q4:	fouth queue to unweld
 *  @func:	callback function after weld complete
 *  @arg:	argument to callback function
 *  @protq:	queue to use for synchronization
 *
 *  unwelq() issues a request to the STREAMS scheduler to unweld two queue pairs from each other
 *  (@q1 from @q3 and @q4 from @q2) with synchronization against @protq.  Once the unwelding is
 *  complete, the @func callback function will be called with argument @arg.
 *
 *  Notices: The @func callback function will be called by the same CPU upon which unweldq() was issued.
 *
 */
int
unweldq(queue_t *q1, queue_t *q2, queue_t *q3, queue_t *q4, weld_fcn_t func, weld_arg_t arg,
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

EXPORT_SYMBOL(unweldq);		/* include/sys/streams/stream.h */

/* 
 *  DEFERRAL FUNCTION ON SYNCH QUEUES
 *  -------------------------------------------------------------------------
 */
#if defined CONFIG_STREAMS_SYNCQS

/* Execution of deferred events */
static void
defer_event(syncq_t *sq, struct strevent *se, unsigned long *flagsp)
{
	*xchg(&sq->sq_tail, &se->se_next) = se;
	unlock_synq_irqrestore(sq, flagsp);
}

/*
 *  Immediate event processing.
 */

/*
 *  strwrit:	- execute a function
 *  @q:		the queue to pass to the function
 *  @mp:	the message to pass to the function
 *  @func:	the function
 */
static void
strwrit(queue_t *q, mblk_t *mp, void (*func) (queue_t *, mblk_t *))
{
	assert(q);
	assert(func);
	{
		struct strthread *t = this_thread;
		queue_t *oldq;

		assert(!t->curentq || in_irq());
		/* oldq could be non-NULL if called from hardirq */
		oldq = t->currentq;
		t->currentq = qget(q);
		func(arg, mp);
		t->currentq = oldq;
		qput(&q);
	}
}

/*
 *  strfunc:	- execute a function like a queue's put procedure
 *  @func:	the function to execute
 *  @q:		the queue whose put procedure to immitate
 *  @mp:	the message to pass to the function
 *  @arg:	the first argument to pass to the function
 *
 *  strfunc() is similar to put(9) with the following exceptions
 *
 *  - strfunc() executes func(arg, mp) instead of qi_putp(q, mp)
 *  - strfunc() returns void
 *  - strfunc() does not perform any synchronization
 */
static void
strfunc(void (*func) (void *, mblk_t *), queue_t *q, mblk_t *mp, void *arg)
{
	int result = -1;

	assert(q);
	assert(func);
	{
		struct strthread *t = this_thread;
		queue_t *oldq;

		assert(!t->curentq || in_irq());
		/* oldq could be non-NULL if called from hardirq */
		oldq = t->currentq;
		t->currentq = qget(q);
		func(arg, mp);
		t->currentq = oldq;
		qput(&q);
	}
}

/*
 *  putp:	- execute a queue's put procedure
 *  @q:		the queue onto which to put the message
 *  @mp:	the message to put
 *
 *  putp() is similar to put(9) with the following exceptions:
 *
 *  - putp() returns the integer result from the modules put procedure.
 *  - putp() does not perform any synchronization
 */
static int
putp(queue_t *q, mblk_t *mp)
{
	int result = -1;

	assert(q);
	assert(q->q_qinfo);
	assert(q->q_qinfo->qi_putp);
	{
		struct strthread *t = this_thread;
		queue_t *oldq;

		assert(!t->curentq || in_irq());
		/* oldq could be non-NULL if called from hardirq */
		oldq = t->currentq;
		t->currentq = qget(q);
		result = q->q_qinfo->qi_putp(q, mp);
		t->currentq = oldq;
		qwakeup(RD(q));
		qput(&q);
	}
      done:
	return (result);
}

/**
 *  srvp:	- execute a queue's service procedure
 *  @q:		the queue to service
 *
 *  Execute a queue's service procedure.  This method of executing the queue service procedure sets
 *  up the appropriate STREAMS environment variables and queue flags and dirctly executes the queue
 *  service procedure without synchronization.  For the queue service procedure to be invoked by
 *  srvp(), the queue service procedure must have first been successfully enabled with enableq(9),
 *  qenable(9), or otherwise enabled by STREAMS functions, and, also, the STREAMS scheduler must
 *  have not already have started execution of the service procedure; otherwise, the service
 *  procedure will not be invoked.
 *
 *  RETURN VALUE: Upon success, srvp() returns the integer value that was returned by the queue's
 *  service procedure (normally zero (0)).  Upon failure, srvp() returns minus one (-1).  The return
 *  value can be safely ignored.
 *
 *  ERRORS: srvp() fails to execute the service procedure when the %QENAB bit is not set on the
 *  queue (e.g. it was not set since the last service procedure run, or the STREAMS scheduler
 *  executed the service procedure before the caller could).
 *
 *  LOCKING: The test-and-set and test-and-clear on the queue's %QENAB bit ensures that only one
 *  service procedure on one processor will be executing the service procedure at any given time,
 *  regardless of context.  This means that queue service procedures do not have to be re-entrant
 *  even for full MP modules.  They do, however, have to be able to run concurrent with other
 *  procedures unless syncrhonization is used.
 *
 *  NOTICES: This call to the service procedure bypasses all sychronization.  Any syncrhonization
 *  required of the queue service procedure must be performed across the call to this function.
 *
 *  Do not pass this function null or invalid queue pointers, or pointers to queues that have no
 *  service procedure.  In safe mode, this will cause kernel assertions to fail and will panic the
 *  kernel.
 */
static int
srvp(queue_t *q)
{
	int result = -1;

	assert(q);
	if (test_and_clear_bit(QENAB_BIT, &q->q_flag)) {
		struct strthread *t = this_thread;
		queue_t *oldq;

		assert(q->q_qinfo);
		assert(q->q_qinfo->qi_srvp);
		assert(!t->curentq || in_irq());
		/* oldq could be non-NULL if called from hardirq */
		oldq = t->currentq;
		t->currentq = qget(q);
		/* just for compatibilty, this bit is not actually used */
		set_bit(QSVCBUSY_BIT, &q->q_flag);
		result = q->q_qinfo->qi_srvp(q, mp);
		clear_bit(QSVCBUSY_BIT, &q->q_flag);
		t->currentq = oldq;
		qwakeup(RD(q));
	}
	qput(&q);
	return (result);
}

EXPORT_SYMBOL(srvp);

#if CONFIG_STREAMS_SYNCQS
/*
 *  -------------------------------------------------------------------------
 *
 *  Synchronization functions.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  Enter functions:
 */

/*
 *  enter_outer_syncq_wait: = enter outer barrier shared or exclusive from process context
 *  @q:	    the queue against which to synchronize
 *  @se:    the STREAMS even to syncrhonize
 *  @sqp:   a pointer to the place to return the syncq reference
 */
static int
enter_outer_syncq_wait(queue_t *q, struct syncq **sqp)
{
	struct syncq *sq = NULL;
	int err = 0;

	if (unlikely(q == NULL))
		goto done;
	if (unlikely((sq = q->q_syncq) == NULL))
		goto done;
	if (unlikely((sq->sq_flag & D_MTOUTPERIM) == 0) && (sq = sq->sq_outer) == NULL)
		goto done;
	{
		unsigned long flags;
		struct stdata *sd;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_owner == current)
			sq->sq_nest++;
		else if (sq->sq_count == 0) {
			sq->sq_owner = current;
			--sq->sq_count;
		} else {
			DECLARE_WAITQUEUE(wait, current);
			add_wait_queue(&sq->sq_waitq, &wait);
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				sd = ((struct queinfo *) q)->qu_str;
				if (!sd) {
					err = -ENOSTR;
					break;
				}
				if (sd->sd_flag & (STPLEX | STRCLOSE | STRHUP)) {
					if (test_bit(STPLEX_BIT, &sd->sd_flag)) {
						err = -EINVAL;
						break;
					}
					if (test_bit(STRCLOSE_BIT, &sd->sd_flag)) {
						err = -EIO;
						break;
					}
					if (test_bit(STRHUP_BIT, &sd->sd_flag)) {
						if (sd->sd_flag & (STRISFIFO | STRISPIPE))
							err = -EPIPE;
						else
							err = -ENXIO;
						break;
					}
				}
				if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
					if (sd->sd_other == NULL) {
						err = -EPIPE;
						break;
					}
					if (test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)) {
						err = -EPIPE;
						break;
					}
				}
				if (signal_pending(current)) {
					err = -EINTR;
					break;
				}
				if (sq->sq_flag & D_MTOCEXCL) {
					if (sq->sq_count == 0
					    || (sq->sq_count == -1 && sq->sq_owner == NULL)) {
						/* available or left for us by leave function */
						/* set exclusive */
						sq->sq_owner = current;
						sq->sq_count = -1;
						err = 0;
						break;
					}
				} else {
					if (sq->sq_count >= 0
					    || (sq->sq_count == -1 && sq->sq_owner == NULL)) {
						/* available or left for us by leave function */
						/* set shared */
						sq->sq_owner = NULL;
						sq->sq_count =
						    (sq->sq_count == -1) ? 1 : sq->sq_count + 1;
						err = 0;
						break;
					}
				}
				spinunlock_irqrestore(&sq->sq_lock, flags);
				schedule();
				spinlock_irqsave(&sq->sq_lock, flags);
			}
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&sq->sq_waitq, &wait);
		}
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
      done:
	if (sqp != NULL)
		*sqp = sq;
	return (err);
}

struct syncq_cookie {
	queue_t *sc_q;
	mblk_t *sc_mp;
	struct strevent *sc_se;
	syncq_t *sc_osq;
	syncq_t *sc_isq;
	syncq_t *sc_sq;
};

static int
defer_syncq(struct syncq_cookie *sc, unsigned long *flagsp)
{
	context_t ctx = current_context();

	switch (ctx) {
	case CTX_PROC:
	case CTX_ATOMIC:
	{
		/* refuse to defer in process context */
		struct syncq *sq = sc->sc_sq;
		queue_t *q = sc->sc_q;
		int rval = 0;

		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue(&sq->sq_waitq, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			sd = ((struct queinfo *) RD(q))->qu_str;
			if (!sd) {
				rval = -ENOSTR;
				break;
			}
			if (sd->sd_flag & (STPLEX | STRCLOSE | STRHUP)) {
				if (test_bit(STPLEX_BIT, &sd->sd_flag)) {
					rval = -EINVAL;
					break;
				}
				if (test_bit(STRCLOSE_BIT, &sd->sd_flag)) {
					rval = -EIO;
					break;
				}
				if (test_bit(STRHUP_BIT, &sd->sd_flag)) {
					if (sd->sd_flag & (STRISFIFO | STRISPIPE))
						rval = -EPIPE;
					else
						rval = -ENXIO;
					break;
				}
			}
			if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
				if (sd->sd_other == NULL) {
					rval = -EPIPE;
					break;
				}
				if (test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)) {
					rval = -EPIPE;
					break;
				}
			}
			if (signal_pending(current)) {
				rval = -EINTR;
				break;
			}
			if (sc->sc_exclus) {
				if (sq->sq_count == 0
				    || (sq->sq_count == -1 && sq->sq_owner == NULL)) {
					/* available or left for us by leave function */
					/* set exclusive */
					sq->sq_owner = current;
					sq->sq_count = -1;
					rval = 1;
					break;
				}
			} else {
				if (sq->sq_count >= 0
				    || (sq->sq_count == -1 && sq->sq_owner == NULL)) {
					/* available or left for us by leave function */
					/* set shared */
					sq->sq_owner = NULL;
					sq->sq_count = (sq->sq_count == -1) ? 1 : sq->sq_count + 1;
					rval = 1;
					break;
				}
			}
			spinunlock_irqrestore(&sq->sq_lock, *flagsp);
			schedule();
			spinlock_irqsave(&sq->sq_lock, *flagsp);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sq->sq_waitq, &wait);
		return (rval);
	}
	default:
	case CTX_STREAMS:
	case CTX_INT:
	case CTX_ISR:
	{
		/* always defer if not in process context */
		struct strevent *se;

		if ((se = sc->sc_se))
			*(sc->se_prev = xchg(&sc->sc_sq->sq_etail, &se->se_next)) = se;
		else {
			mblk_t *mp;

			if ((mp = sc->sc_mp))
				*xchg(&sc->sc_sq->sq_mtail, &mp->b_next) = mp;
			else {
				queue_t *q;

				if ((q = sc->sc_q))
					*xchg(&sc->sc_sq->sq_qtail, &q->q_link) = q;
			}
		}
		return (0);
	}
	}
}
static int
find_syncq(struct syncq_cookie *sc)
{
	if (sc->sc_q && (sc->sc_osq = sc->sc_q->q_syncq) && !(sc->sc_osq->sq_flag & D_MTOUTPERIM)) {
		sc->sc_isq = sc->sc_osq;
		sc->sc_osq = sc->sc_isq->sq_outer;
	}
	return (sc->sc_osq != NULL);
}
static int
find_syncqs(struct syncq_cookie *sc)
{
	if (q && (sc->sc_osq = q->q_syncq) && !(sc->sc_osq->sq_flag & D_MTOUTPERIM)) {
		sc->sc_isq = sc->sc_osq;
		sc->sc_osq = sc->sc_isq->sq_outer;
	}
	return (sc->sc_osq || sc->sc_isq);
}
static int
enter_syncq_exclus(struct syncq_cookie *sc)
{
	int rval = 1;
	unsigned long flags;
	syncq_t *sq = sc->sc_sq;

	sc->sc_exclus = 1;
	spinlock_irqsave(&sq->sq_lock, flags);
	if (sq->sq_owner == current)
		sq->sq_nest++;
	else if (sq->sq_count == 0) {
		sq->sq_owner = current;
		--sq->sq_count;
	} else
		rval = defer_syncq(sc, &flags);
	spinunlock_irqrestore(&sq->sq_lock, flags);
	return (rval);
}
static int
enter_syncq_shared(struct syncq_cookie *sc)
{
	int rval = 1;
	unsigned long flags;
	syncq_t *sq = sc->sc_sq;

	sc->sc_exclus = 0;
	spinlock_irqsave(&sq->sq_lock, flags);
	if (sq->sq_owner == current)
		sq->sq_nest++;
	else if (sq->sq_count >= 0)
		sq->sq_count++;
	else
		rval = defer_syncq(sc, &flags);
	spinunlock_irqrestore(&sq->sq_lock, flags);
	return (rval);
}
static int
enter_outer_syncq_exclus(struct syncq_cookie *sc)
{
	if (find_syncq(sc) && (sc->sc_sq = sc->sc_osq))
		return enter_syncq_exclus(sc);
	return (1);
}
static int
enter_outer_syncq_shared(struct syncq_cookie *sc)
{
	if (find_syncq(sc) && (sc->sc_sq = sc->sc_osq))
		return enter_syncq_shared(sc);
	return (1);
}
static int
enter_inner_syncq_exclus(struct syncq_cookie *sc)
{
	if (find_syncqs(sc)) {
		if ((sc->sc_sq = sc->sc_osq)) {
			int rval;

			if ((rval = enter_syncq_shared(sc)) <= 0)
				return (rval);
		}
		if ((sc->sc_sq = sc->sc_isq))
			return enter_syncq_exclus(sc);
	}
	return (1);
}
static int
enter_inner_syncq_shared(struct syncq_cookie *sc)
{
	if (find_syncqs(sc)) {
		if ((sc->sc_sq = sc->sc_osq)) {
			int rval;

			if ((rval = enter_syncq_shared(sc)) <= 0)
				return (rval);
		}
		if ((sc->sc_sq = sc->sc_isq))
			return enter_syncq_shared(sc);
	}
	return (1);
}
static int
enter_inner_syncq_asputp(struct syncq_cookie *sc)
{
	if (find_syncqs(sc)) {
		if ((sc->sc_sq = sc->sc_osq)) {
			int rval;

			if ((rval = enter_syncq_shared(sc)) <= 0)
				return (rval);
		}
		if ((sc->sc_sq = sc->sc_isq)) {
			if (sc->sc_isq->sq_flag & D_MTPUTSHARED)
				return enter_syncq_shared(sc);
			else
				return enter_syncq_exclus(sc);
		}
	}
	return (1);
}
static int
enter_inner_syncq_asopen(struct syncq_cookie *sc)
{
	if (find_syncqs(sc)) {
		if ((sc->sc_sq = sc->sc_osq)) {
			if (sc->sc_sq->sq_flag & D_MTOCEXCL)
				return enter_syncq_exclus(sc);
			else {
				int rval;

				if ((rval = enter_syncq_shared(sc)) <= 0)
					return (rval);
			}
		}
		if ((sc->sc_sq = sc->sc_isq))
			return enter_syncq_exclus(sc);
	}
	return (1);
}

static int
enter_syncq_writer(struct syncq_cookie *sc, void (*func) (queue_t *, mblk_t *), int perim)
{
	struct mbinfo *m = (typeof(m)) sc->sc_mp;

	m->m_func = (void *) strwrit;
	m->m_queue = sc->sc_q;
	m->m_private = (void *) func;

	assert(perim == PERIM_OUTER || perim == PERIM_INNER);

	if (perim & PERIM_OUTER)
		return enter_outer_syncq_exclus(sc);
	if (perim & PERIM_INNER)
		return enter_inner_syncq_exclus(sc);
	return (1);
}

static int
enter_inner_syncq_func(struct syncq_cookie *sc, void (*func) (void *, mblk_t *), void *arg)
{
	struct mbinfo *m = (typeof(m)) sc->sc_mp;

	m->m_func = (void *) func;
	m->m_queue = sc->sc_q;
	m->m_private = arg;

	return enter_inner_syncq_asputp(sc);
}

static int
enter_inner_syncq_putp(struct syncq_cookie *sc)
{
	struct mbinfo *m = (typeof(m)) sc->sc_mp;

	m->m_func = (void *) putp;
	m->m_queue = sc->sc_q;
	m->m_private = NULL;

	return enter_inner_syncq_asputp(sc);
}

static int
enter_inner_syncq_srvp(struct syncq_cookie *sc)
{
	return enter_inner_syncq_exclus(sc);
}

static void
leave_syncq(struct syncq_cookie *sc)
{
	syncq_t *sq;
	if (likely((sq = sc->sc_sq) == NULL))
		return;
	{
		unsigned long flags;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sc->sc_exclus) {
			/* last entry was exclusive */
			if (sq->sq_owner == current && --sq->sq_nest < 0) {
				register struct strevent *se, *se_next;

				sq->sq_nest = 0;
				/* we are still in the barrier exclusive */
				/* just run them all exclusive: anything that wanted to enter the
				   outer perimeter shared and the inner perimeter exclusive will
				   run nicely with just the outer perimeter exclusive; this is
				   because the outer perimeter is always more restrictive than the
				   inner perimeter. */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					/* fake it out that we are STREAMS executive */
					local_bh_disable();
					set_bit(queueflag, &this_thread->flags);
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* remove fakeout */
					clear_bit(queueflag, &this_thread->flags);
					local_bh_enable();
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		} else {
			/* last entry was shared */
			if (--sq->sq_count == 0) {
				struct strthread *t = current_thread;
				struct strevent *se;

				/* just left shared for the last time, the last guy out is
				   responsible for processing the backlog */
				sq->sq_nest = 0;
				sq->sq_owner == current;
				sq->sq_count = -1;
				/* we are now in the barrier exclusive */
				/* just run them all exclusive: anything that wanted to enter the
				   outer perimeter shared and the inner perimeter exclusive will
				   run nicely with just the outer perimeter exclusive; this is
				   because the outer perimeter is always more restrictive than the
				   inner perimeter. */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					/* fake it out that we are STREAMS executive */
					local_bh_disable();
					set_bit(queueflag, &this_thread->flags);
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* remove fakeout */
					clear_bit(queueflag, &this_thread->flags);
					local_bh_enable();
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		}
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
}

static int
leave_outer_syncq_wait(struct syncq *sq)
{
	struct syncq *osq = NULL;

	/* not inside barrier */
	if (likely(sq == NULL))
		return;
	/* no sychronization */
	if (likely((osq = global_sync) == NULL && (osq = q->q_sync) == NULL))
		return;
	/* no outer barrier */
	if (likely((osq->sq_flag & D_MTOUTPERIM) == 0 && (osq = osq->sq_outer) == NULL))
		return;
	if (unlikely(osq != sq))
		swerr();
	{
		unsigned long flags;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_flag & D_MTOCEXCL) {
			if (sq->sq_owner == current && --sq->sq_nest < 0) {
				register struct strevent *se, *se_next;

				sq->sq_nest = 0;
				/* we are still in the barrier exclusive */
				/* just run them all exclusive: anything that wanted to enter the
				   outer perimeter shared and the inner perimeter exclusive will
				   run nicely with just the outer perimeter exclusive; this is
				   because the outer perimeter is always more restrictive than the
				   inner perimeter. */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					/* fake it out that we are STREAMS executive */
					local_bh_disable();
					set_bit(queueflag, &this_thread->flags);
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* remove fakeout */
					clear_bit(queueflag, &this_thread->flags);
					local_bh_enable();
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		} else {
			if (--sq->sq_count == 0) {
				struct strthread *t = current_thread;
				struct strevent *se;

				/* just left shared for the last time, the last guy out is
				   responsible for processing the backlog */
				sq->sq_nest = 0;
				sq->sq_owner == current;
				sq->sq_count = -1;
				/* we are now in the barrier exclusive */
				/* just run them all exclusive: anything that wanted to enter the
				   outer perimeter shared and the inner perimeter exclusive will
				   run nicely with just the outer perimeter exclusive; this is
				   because the outer perimeter is always more restrictive than the
				   inner perimeter. */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					/* fake it out that we are STREAMS executive */
					local_bh_disable();
					set_bit(queueflag, &this_thread->flags);
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
					/* remove fakeout */
					clear_bit(queueflag, &this_thread->flags);
					local_bh_enable();
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		}
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
}

/*
 *  leave_outer_syncq_exclus: - leave the outer barrier that was entered exclusive
 *  @sq:    the synchronization queue to leave
 */
static void
leave_outer_syncq_exclus(struct syncq *sq)
{
	struct syncq *osq = NULL;

	/* not inside barrier */
	if (likely(sq == NULL))
		return;
	/* no sychronization */
	if (likely((osq = global_sync) == NULL && (osq = q->q_sync) == NULL))
		return;
	/* no outer barrier */
	if (likely((osq->sq_flag & D_MTOUTPERIM) == 0 && (osq = osq->sq_outer) == NULL))
		return;
	if (unlikely(osq != sq))
		swerr();
	{
		unsigned long flags;

		spinlock_irqsave(&sq->sq_lock, flags);
		if (sq->sq_owner == current) {
			if (--sq->sq_nest < 0) {
				register struct strevent *se, *se_next;

				sq->sq_nest = 0;
				/* we are still in the barrier exclusive */
				while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
					sq->sq_tail = &sq->sq_head;
					spin_unlock_irqrestore(&sq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						/* just run them all exclusive: anything that
						   wanted to enter the outer perimeter shared and
						   the inner perimeter exclusive will run nicely
						   with just the outer perimeter exclusive; this is 
						   because the outer perimeter is always more
						   restrictive than the inner perimeter. */
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&sq->sq_lock, flags);
				}
				/* finally unlock the queue */
				sq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&sq->sq_waitq)))
					wake_up_all(&sq->sq_waitq);
				else
					sq->sq_count = 0;
			}
		} else
			swerr();
		spinunlock_irqrestore(&sq->sq_lock, flags);
	}
}

/*
 *  leave_inner_syncq_exclus: - leave the inner barrier that was entered exclusive
 *  @sq:    the synchronization queue to leave
 */
static void
leave_inner_syncq_exclus(struct syncq *sq)
{
	struct syncq *osq = NULL, *isq = NULL;

	if (unlikely(sq == NULL))
		return;
	if (unlikely((osq = q->q_syncq) == NULL))
		return;
	if (unlikely((osq->sq_flag & D_MTOUTPERIM) == 0)) {
		isq = osq;
		osq = isq->sq_outer;
	}
	{
		unsigned long flags;

		spinlock_irqsave(&isq->sq_lock, flags);
		if (isq->sq_owner == current) {
			if (--isq->sq_nest < 0) {
				struct strevent *se;

				isq->sq_nest = 0;
				/* we are still in the barrier exclusive */
				while ((se_next = xchg(&isq->sq_head, NULL))) {	/* MP-SAFE */
					isq->sq_tail = &isq->sq_head;
					spin_unlock_irqrestore(&isq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						/* just run them all exclusive: anything that
						   wanted to enter the inner perimeter shared will
						   run nicely with the inner perimeter exclusive. */
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&isq->sq_lock, flags);
				}
				/* finally unlock the queue */
				isq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&isq->sq_waitq)))
					wake_up_all(&isq->sq_waitq);
				else
					isq->sq_count = 0;
			}
		} else
			swerr();
		spinunlock_irqrestore(&isq->sq_lock, flags);
		if (osq) {
			spinlock_irqsave(&osq->sq_lock, flags);
			if (--osq->sq_count == 0) {
				/* just left shared for the last time */
				struct strthread *t = current_thread;
				struct strevent *se;

				osq->sq_nest = 0;
				osq->sq_owner == current;
				osq->sq_count = -1;
				/* we are now in the barrier exclusive */
				while ((se_next = xchg(&osq->sq_head, NULL))) {	/* MP-SAFE */
					osq->sq_tail = &osq->sq_head;
					spin_unlock_irqrestore(&osq->sq_lock, flags);
					/* process backlog */
					while ((se = se_next)) {
						se_next = xchg(&se->se_next, NULL);
						/* just run them all exclusive: anything that
						   wanted to enter the outer perimeter shared and
						   the inner perimeter exclusive will run nicely
						   with just the outer perimeter exclusive; this is 
						   because the outer perimeter is always more
						   restrictive than the inner perimeter. */
						sq_doevent_synced(se);
					}
					spin_lock_irqsave(&osq->sq_lock, flags);
				}
				/* finally unlock the queue */
				osq->sq_owner = NULL;
				/* if we have waiters, we leave the queue locked but with a NULL
				   owner, and one of the waiters will pick it up, the others will
				   wait some more */
				if (unlikely(waitqueue_active(&osq->sq_waitq)))
					wake_up_all(&osq->sq_waitq);
				else
					osq->sq_count = 0;
			}
			spinunlock_irqrestore(&osq->sq_lock, flags);
		}
	}
}

#endif

/**
 *  qwriter:	- call a function after gaining exclusive access to a perimeter
 *  @q:		the queue against which to synchronize and pass to the function
 *  @mp:	a message to pass to the function
 *  @func:	the function to call once access is gained
 *  @perim:	the perimeter to which to gain access
 *
 *  Queue writers enter the requested perimeter exclusive.  If the perimeter deos not exist, the
 *  function is invoked without sychronization.  The call to @func might be immediate or might be
 *  deferred.  If this function is called from ISR context, be aware that @func might execute at ISR
 *  context.
 *
 *  qwriter() callbacks enter the outer perimeter exclusive if the outer perimeter exists and
 *  PERIM_OUTER is set in perim; otherwise, it enters the inner perimeter exclusive if the inner
 *  perimeter exists and PERIM_INNER is set in perim.  If no perimeters exist, or the perimeter
 *  corresponding to the PERIM_OUTER and PERIM_INNER setting does not exist, the callback function
 *  is still executed, however only STREAMS MP safety syncrhonization is performed.
 *
 *  NOTICES: Solaris restricts from where this function can be called to put(9), srv(9), qwriter(9),
 *  qtimeout(9) or qbufcall(9) procedures.  We make no restrictions.
 *
 *  @mp must not be NULL, otherwise it is not possible to defer the callback.  @mp cannot be on an
 *  existing queue.
 *
 */
void
qwriter(queue_t *q, mblk_t *mp, void (*func) (queue_t *, mblk_t *), int perim)
{
#if CONFIG_STREAMS_SYNCQS
	struct syncq_cookie ck = {.sc_q = q,.sc_mp = mp, }, *sc = &ck;

	if (unlikely(enter_syncq_writer(sc, func, perim) == 0))
		return;
#endif
	strwrit(q, mp, func);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
}

EXPORT_SYMBOL(qwriter);

/**
 *  qstrfunc:	- execute a function like the queue's put procedure synchronized
 *  @func:	the function to imitate the queue's put procedure
 *  @q:		the queue whose put procedure is to be imitated
 *  @mp:	the message to pass to the function
 *  @arg:	the first argument to pass to the function
 *
 *  streams_put(9) callouts function precisely like put(9), however, they invoke a callout function
 *  instead of the queue's put procedure.  streams_put() events always need a valid queue reference
 *  against which to synchronize.
 */
static int
qstrfunc(void (*func) (void *, mblk_t *), queue_t *q, mblk_t *mp, void *arg)
{
#if CONFIG_STREAMS_SYNCQS
	struct syncq_cookie ck = {.sc_q = q,.sc_mp = mp, }, *sc = &ck;

	if (unlikely(enter_inner_syncq_func(sc, func, arg) == 0))
		return;
#endif
	strfunc(func, q, mp, arg);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
	return;
}

/**
 *  qputp:	- execute a queue's put procedure synchronized
 *  @q:		the queue whose put procedure is to be executed
 *  @mp:	the message to pass to the put procedure
 *
 *  Put procedures enter the outer perimeter shared and the inner perimeter exclusive, unless the
 *  D_MTPUTSHARED flag is set, in which case they enter the inner perimeter shared. If the outer
 *  perimeter does not exist, only the inner perimeter will be entered.  If the inner perimeter does
 *  not exist, the put procedure is executed without synchronization.  If the event cannot enter a
 *  perimeter (is blocked) it is deferred against the synchronization queue.  Put procedures always
 *  have a valid queue reference against which to synchronize.
 *  
 *  If this function is called from process context, and the barrier is raised, the calling process
 *  will block until it can enter the barrier.  If this function is called from interrupt context
 *  (soft or hard irq) the event will be deferred and the thread will return.
 *
 *  NOTICES: AIX-style message filtering is performed outside synchronization.  This means that the
 *  message filtering function must be fully re-entrant and able to run concurrently with other
 *  procedures.  Care should be taken if the filtering function accesses shared state (e.g. the
 *  queue's private structure).
 */
static int
qputp(queue_t *q, mblk_t *mp)
{
	int result;

	/* skip message filtering at hard irq */
	if (!in_irq()) {
		queue_t *newq, *q_next;

		for (newq = qget(q); newq && (!newq->q_ftmsg || !newq->q_ftmsg(mp));
		     q_next = qget(newq->q_next), qput(&newq), newq = q_next) ;
		if (!newq) {
			/* no queue wants the message - throw it away */
			freemsg(mp);
			goto done;
		}
		q = newq;
		qput(&newq);
	}
#if CONFIG_STREAMS_SYNCQS
	struct syncq_cookie ck = {.sc_q = q,.sc_mp = mp, }, *sc = &ck;

	if (unlikely(enter_inner_syncq_putp(sc) == 0))
		return;
#endif
	result = putp(q, mp);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
	return (result);
}

/**
 *  qsrvp:	- execute a queue's service procedure synchronized
 *  @q:		the queue whose service procedure is to be executed
 *
 *  Service procedures enter the outer perimeter shared and the inner perimeter exclusive.  If a
 *  perimeter does not exist, it will be treated as entered automatically.  If they cannot enter a
 *  perimeter (blocked), the event will be stacked against the synchronization queue.  Service
 *  procedures always have a valid queue reference against which to synchronize.
 *  
 *  If this function is called from process context, and the barrier is raised, the calling process
 *  will block until it can enter the barrier.  If this function is called from interrupt context
 *  (soft or hard irq) the event will be deferred and the thread will return.
 */
static int
qsrvp(queue_t *q)
{
	int result;

#if CONFIG_STREAMS_SYNCQS
	struct syncq *sq;
	struct syncq_cookie ck = {.sc_q = q, }, &sc = &ck;

	if (unlikely(enter_inner_syncq_srvp(sc) == 0))
		return;
#endif
	result = srvp(q);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
	return (result);
}

/**
 *  qopen:	- call a module's qi_qopen entry point
 *  @q:		the read queue of the module queue pair to open
 *  @devp:	pointer to the opening and returned device number
 *  @oflag:	open flags
 *  @sflag:	stream flag, can be %DRVOPEN, %MODOPEN, %CLONEOPEN
 *  @crp:	pointer to the opening task's credential structure
 *
 *  Note that if a syncrhonization queue exitsts and Solaris compatible flags D_MTOUTPERIM and
 *  D_MTOCEXCL are set in the outer perimeter syncrhonization queue, then we must either enter the
 *  outer perimeter exclusive, or block awaiting exclusive access to the outer perimeter.
 *
 *  CONTEXT: Must only be called from a blockable context.
 */
int
qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int (*q_open) (queue_t *, dev_t *, int, int, cred_t *);
	int err = -ENOPKG;

	assert(q);
	assert(q->q_qinfo);

	if ((q_open = q->q_qinfo->qi_qopen)) {
#if CONFIG_STREAMS_SYNCQS
		struct syncq_cookie ck = {.sc_q = q, }, *se = &ck;

		if (unlikely((err = enter_inner_syncq_asopen(sc)) <= 0))
			goto error;
#endif
		err = q_open(q, devp, oflag, sflag, crp);
#if CONFIG_STREAMS_SYNCQS
		leave_outer_syncq_wait(sc);
#endif
	}
	goto error;
      error:
	return (err);
}

EXPORT_SYMBOL(qopen);

/**
 *  qclose:	- invoke a queue pair's qi_qclose entry point
 *  @q:		read queue of the queue pair to close
 *  @oflag:	open flags
 *  @crp:	credentials of closing task
 *
 *  CONTEXT: Must only be called from a blockable context.
 */
int
qclose(queue_t *q, int oflag, cred_t *crp)
{
	int err = -ENXIO;
	int (*q_close) (queue_t *, int, cred_t *);

	assert(q);
	assert(q->q_qinfo);

	if ((q_close = q->q_qinfo->qi_qclose)) {
#if CONFIG_STREAMS_SYNCQS
		struct syncq_cookie ck = {.sc_q = q, }, *se = &ck;

		if (unlikely((err = enter_inner_syncq_asopen(sc)) <= 0))
			goto error;
#endif
		err = q_close(q, oflag, crp);
#if CONFIG_STREAMS_SYNCQS
		leave_outer_syncq_wait(sc);
#endif
	}
	goto error;
      error:
	return (err);
}

EXPORT_SYMBOL(qclose);

/**
 *  streams_put: - call a function like a queue's put procedure
 *  @func:	the function to call
 *  @q:		the queue whose put procedure to immitate
 *  @mp:	the message to pass to the function
 *  @arg:	the first argument to pass to the function
 *
 *  NOTICES: Don't call functions that do not exist.
 *
 *  streams_put(NULL, q, mp, NULL) is identical to calling put(q, mp).
 *
 *  CONTEXT: Any.  But beware that if you call this function from an ISR that the function is aware
 *  that it may be called in ISR context.  Also, if called in hardirq context, message filtering
 *  will not be performed.
 */
void
streams_put(void (*func) (void *, mblk_t *), queue_t *q, mblk_t *mp, void *arg)
{
	(void) qstrfunc(func, q, mp, arg);
}

EXPORT_SYMBOL(streams_put);

/**
 *  put:	- call a queue's qi_putq() procedure
 *  @q:		the queue's procedure to call
 *  @mp:	the message to place on the queue
 *
 *  NOTICES: Don't put to put routines that do not exist.
 *
 *  CONTEXT: Any.  But beware that if you call this function from an ISR that the put procedure is
 *  aware that it may be called in ISR context.  Also, if called in hardirq context, message
 *  filtering will not be performed.
 */
void
put(queue_t *q, mblk_t *mp)
{
	assert(q);
	assert(q->q_qinfo);
	assert(q->q_qinfo->qi_putp);
	(void) qputp(q, mp);
}

EXPORT_SYMBOL(put);

/*
 *  Synchronized event processing.
 */

static void
do_stream_synced(struct strevent *se)
{
}

static void
do_bufcall_synced(struct strevent *se)
{
	queue_t *q;
	unsigned long flags;
	int safe;

	q = se->x.b.queue;
	safe = (q && test_bit(QSAFE_BIT, &q->q_flag));
	if (safe)
		local_irq_save(flags);
	this_thread->currentq = q;
	if (se->x.b.func)
		se->x.b.func(se->x.b.arg);
	this_thread->currentq = NULL;
	if (safe)
		local_irq_restore(flags);
	qput(&q);
}

/**
 *  do_timeout_synced: - process timeout callback after synchronization
 *  @se:	the timer STREAMS event to process
 *
 *  LOCKING: no locking is required aside from suppressing local interrupts if the timeout function
 *  has requested it (the pl member of the timeout event) or the module has requested it (the %QSAFE
 *  bit is set on the queue).
 *
 *  Note that if the timeout is against a queue, a reference is held for the queue so that it cannot
 *  be removed while the timeout is still pending.  Without syncrhonization, the timeout callback
 *  can run concurrent with any other queue procedure (qi_putp, qi_srvp, qi_qopen, qi_qclose) or
 *  concurrent with any other callback or callout (other timeouts, bufcalls, free routines, qwriter,
 *  streams_put, weldq or unweldq).
 */
static void
do_timeout_synced(struct strevent *se)
{
	queue_t *q;
	void (*func) (caddr_t);

	q = se->x.t.queue;
	if ((func = se->x.t.func)) {
		unsigned long flags;
		int safe = (se->x.t.pl || (q && test_bit(QSAFE_BIT, &q->q_flag)));
		struct strthread *t = this_thread;

		t->currentq = q;
		if (safe)
			local_irq_save(flags);
		func(se->x.t.arg);
		if (safe)
			local_irq_restore(flags);
		t->currentq = NULL;
	}
	qput(&q);
}

/**
 *  do_weldq_synced: - process a weldq callback after syncrhonization
 *  @se:	the weldq STREAMS event to process
 *
 *  LOCKING: Stream head write locking is required to keep the queue read and write side pointers
 *  atomic across operations that use both (such as backq()).  Operations that use both will take a
 *  stream head read lock across the operation, we take a stream head write lock to block such
 *  operations until the queue has been welded.  We only take write locks on unique stream heads and
 *  we figure out uniqueness before taking the lock.  No other locking is required.
 *
 *  The weld callback function is executed outside the locks.
 *
 *  Note that a reference is held for all queues, including the protection queue, so that these
 *  queues cannot disappear while the event is outstanding.
 */

static void
do_weldq_synced(struct strevent *se)
{
	unsigned long flags;
	queue_t *q, *qn1 = NULL, *qn3 = NULL;
	queue_t *q1, *q2, *q3, *q4;
	struct stdata *sd1, *sd3;

	/* get all the queues */
	q1 = se->x.w.q1;
	q2 = se->x.w.q2;
	q3 = se->x.w.q3;
	q4 = se->x.w.q4;
	/* get all the stream heads */
	sd1 = q1 ? ((struct queinfo *) RD(q1))->qu_str : NULL;
	sd3 = q2 ? ((struct queinfo *) RD(q3))->qu_str : NULL;
	/* find the unique stream heads */
	if (sd3 == sd1)
		sd3 = NULL;
	q = se->x.w.queue;
	local_irq_save(flags);
	if (sd1)
		swlock(sd1);
	if (sd3)
		swlock(sd3);
	if (q1)
		qn1 = xchg(&q1->q_next, qget(q2));
	if (q3)
		qn3 = xchg(&q3->q_next, qget(q4));
	if (sd3)
		swunlock(sd3);
	if (sd1)
		swunlock(sd1);
	local_irq_restore(flags);
	qput(&q1);
	qput(&q2);
	qput(&q3);
	qput(&q4);
	qput(&qn1);
	qput(&qn3);
	if (se->x.w.func) {
		int safe = (q && test_bit(QSAFE_BIT, &q->q_flag));
		struct strthread *t = this_thread;

		t->currentq = q;
		if (safe)
			local_irq_save(flags);
		se->x.w.func(se->x.w.arg);
		if (safe)
			local_irq_restore(flags);
		t->currentq = NULL;
	}
	qput(&q);
}

static void
do_unweldq_synced(struct strevent *se)
{
	/* same operation, different arguments */
	return do_weldq_synced(se);
}

static void
sq_doevent_synced(struct strevent *se)
{
	struct seinfo *s = (typeof(s)) se;

	switch (s->s_type) {
	case SE_STREAM:
		return do_stream_synced(se);
	case SE_BUFCALL:
		return do_bufcall_synced(se);
	case SE_TIMEOUT:
		return do_timeout_synced(se);
	case SE_WELDQ:
		return do_weldq_synced(se);
	case SE_UNWELDQ:
		return do_unweldq_synced(se);
	default:
		swerr();
		return;
	}
}

static inline void
do_stream_event(struct strevent *se)
{
}

/*
 * bufcall events enter the outer barrier shared and the inner barrier exclusive.  Otherwise,
 * STREAMS MP safety is performed.
 */
static void
do_bufcall_event(struct strevent *se)
{
#if CONFIG_STREAMS_SYNCQS
	struct syncq_cookie ck = {.sc_q = se->x.b.queue,.sc_se = se, }, *sc = &ck;

	if (unlikely(enter_inner_syncq_exclus(sc) == 0))
		return;
#endif
	do_bufcall_synced(se);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
	sefree(se);
}

/*
 * Timeout events enter the outer barrier shared and the inner barrier exclusive.  Otherwise,
 * STREAMS MP safety is performed.  Timeouts also suppress interrupts if requested.
 */
static void
do_timeout_event(struct strevent *se)
{
#if CONFIG_STREAMS_SYNCQS
	struct syncq_cookie ck = {.sc_q = se->x.t.queue,.sc_se = se, }, *sc = &ck;

	if (unlikely(enter_inner_syncq_exclus(sc) == 0))
		return;
#endif
	do_timeout_synced(se);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
	sefree(se);
}

/* 
 *  Weld events are either synchronous or not.  A synchronization queue must be provided if
 *  synchronization is required.  Outer barriers are entered exclusive and if an outer barrier does
 *  not exist the inner barrier is entered exclusive, and if that does not exist, only normal
 *  STREAMS MP safety is used.  stream head write locking is used to protect pointer dereferencing.
 */
static void
do_weldq_event(struct strevent *se)
{
#if CONFIG_STREAMS_SYNCQS
	struct syncq_cookie ck = {.sc_q = se->x.w.queue,.sc_se = se, }, *sc = &ck;

	if (unlikely(enter_outer_syncq_exclus(sc) == 0))
		return;
#endif
	do_weldq_synced(se->x.w.queue, se);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
	sefree(se);
}

/*
 *  Unweld events are either synchronous or not.  A synchronization queue must be provided if
 *  synchronization is required.  Outer barriers are entered exclusive and if an outer barrier does
 *  not exist the inner barrier is entered exclusive, and if that does not exist, only normal
 *  STREAMS MP safety is used.  stream head write locking is used to protect pointer dereferencing.
 */
static inline void
do_unweldq_event(struct strevent *se)
{
#if CONFIG_STREAMS_SYNCQS
	struct syncq_cookie ck = {.sc_q = se->x.w.queue,.sc_se = se, }, *sc = &ck;

	if (unlikely(enter_outer_syncq_exclus(sc) == 0))
		return;
#endif
	do_unweldq_synced(se);
#if CONFIG_STREAMS_SYNCQS
	leave_syncq(sc);
#endif
	sefree(se);
}

static inline void
sq_doevent(struct strevent *se)
{
	struct seinfo *s = (typeof(s)) se;

	switch (s->s_type) {
	case SE_STREAM:
		return do_stream_event(se);
	case SE_BUFCALL:
		return do_bufcall_event(se);
	case SE_TIMEOUT:
		return do_timeout_event(se);
	case SE_WELDQ:
		return do_weldq_event(se);
	case SE_UNWELDQ:
		return do_unweldq_event(se);
	default:
		swerr();
		return;
	}
}
#endif

/**
 *  kmem_alloc:	- allocate memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 */
__EXTERN_INLINE void *kmem_alloc(size_t size, int flags);

EXPORT_SYMBOL(kmem_alloc);	/* include/sys/streams/kmem.h */

/**
 *  kmem_zalloc: - allocate and zero memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 */
__EXTERN_INLINE void *kmem_zalloc(size_t size, int flags);

EXPORT_SYMBOL(kmem_zalloc);	/* include/sys/streams/kmem.h */

/**
 *  kmem_free:	- free memory
 *  @addr:	address of memory
 *  @size:	amount of memory to free
 *
 *  Frees memory allocated with kmem_alloc() or kmem_zalloc().  When we free a non-zero segment of
 *  memory, we also want to raise pending buffer callbacks on all STREAMS scheduler threads so that
 *  they can attempt to use the memory.
 */
void
kmem_free(void *addr, size_t size)
{
	kfree(addr);
	if (likely(size >= 0))
		raise_bufcalls();
}

EXPORT_SYMBOL(kmem_free);	/* include/sys/streams/kmem.h */

/**
 *  kmem_alloc_node: - allocate memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 *  @node:
 */
__EXTERN_INLINE void *kmem_alloc_node(size_t size, int flags, cnodeid_t node);

EXPORT_SYMBOL(kmem_alloc_node);	/* include/sys/streams/kmem.h */

/**
 *  kmem_zalloc: - allocate and zero memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 *  @node:
 */
__EXTERN_INLINE void *kmem_zalloc_node(size_t size, int flags, cnodeid_t node);

EXPORT_SYMBOL(kmem_zalloc_node);	/* include/sys/streams/kmem.h */

/* 
 *  -------------------------------------------------------------------------
 *
 *  STREAMS Scheduler SOFTIRQ kernel thread runs
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  timeouts:	- process timeouts
 *  @t:		STREAMS execution thread
 *  
 *  Process all oustanding timeouts in the order in which they were received.
 */
static inline void
timeouts(struct strthread *t)
{
	register struct strevent *se, *se_next;

	do {
		clear_bit(strtimout, &t->flags);
		if ((se_next = xchg(&t->strtimout_head, NULL))) {	/* MP-SAFE */
			t->strtimout_tail = &t->strtimout_head;
			while ((se = se_next)) {
				se_next = xchg(&se->se_next, NULL);
				/* this might further defer against a synchronization queue */
				sq_doevent(se);
			}
		}
	} while (test_bit(strtimout, &t->flags));
}

/*
 *  doevents:	- process STREAMS events
 *  @t:		STREAMS execution thread
 */
static inline void
doevents(struct strthread *t)
{
	register struct strevent *se, *se_next;

	do {
		clear_bit(strevents, &t->flags);
		if ((se_next = xchg(&t->strevents_head, NULL))) {	/* MP-SAFE */
			t->strevents_tail = &t->strevents_head;
			while ((se = se_next)) {
				se_next = xchg(&se->se_next, NULL);
				/* this might further defer against a synchronization queue */
				sq_doevent(se);
			}
		}
	} while (test_bit(strevents, &t->flags));
}

#if defined CONFIG_STREAMS_SYNCQS
/*
 * runsyncq:	- process backlog on a synchronization queue
 * @sq:		the sychronization queue to process
 * @flagsp:	pointer to flags from irq_save lock
 *
 * This function processes synchornization queue backlog events on a synchronization queue that is
 * already locked for exclusive access.  After processing the backlog, the syncrhonization queue is
 * unlocked.
 *
 * This function is used both by backlog() processing in the STREAMS scheduler as well as processing
 * for leaving synchronization queues and processing of the backlog by the leaving thread.
 *
 * LOCKING: Although the removal of events from the syncrhonization queue is MP-safe for deletion,
 * it is not for insertion.  That is, if an event is added to the list just after we find it empty,
 * we will fail to process the event.  So, we take the synchronization queue lock across finding an
 * empty list and unlocking the barrier, so that subsequent events will find the barrier down.  This
 * function must be called with the syncrhonization queue spin lock locked and flags saved to
 * @flagsp.
 */
void
runsynq(struct syncq, unsigned long *flagsp)
{
	register struct strevent *se, *se_next;

	while ((se_next = xchg(&sq->sq_head, NULL))) {	/* MP-SAFE */
		sq->sq_tail = &sq->sq_head;
		spin_unlock_irqrestore(&sq->sq_lock, *flagsp);
		while ((se = se_next)) {
			se_next = xchg(&se->se_next, NULL);
			sq_doevent_synced(se);
		}
		spin_lock_irqsave(&sq->sq_lock, *flagsp);
	}
	/* finally unlock the queue */
	sq->sq_owner = NULL;
	sq->sq_count = 0;
	sq->sq_nest = 0;
}

/*
 *  backlog:	- process message backlog
 *  @t:		STREAMS execution thread
 *
 *  When a process context thread (i.e. qopen, qclose and stream head functions) enters a
 *  synchronization barrier exclusive or shared and finds a backlog on exit, it indicates to the
 *  STREAMS scheduler that backlog processing is required by placing the synchornization queue on
 *  the scheduler sq list and raises the qsyncflag.  Take careful note that the synchronization
 *  queue placed onto this list is left exclusively locked.  This is to preserve event ordering.
 *  This procedure must unlock the sync queue after servicing.
 *
 *  Note also that all event run exclusive at the barrier on which they queued, even if only shared
 *  access was requested.  This is acceptable and reduces the burder of tracking two perimeters with
 *  shared or exclusive access.
 */
static inline void
backlog(struct strthread *t)
{
	register syncq_t *sq, *sq_link;

	do {
		clear_bit(qsyncflag, &t->flags);
		if ((sq_link = xchg(&t->sqhead, NULL))) {	/* MP-SAFE */
			t->sqtail = &t->sqhead;
			while ((sq = sq_link)) {
				unsigned long flags;

				sq_link = xchg(&sq->sq_link, NULL);
				spin_lock_irqsave(&sq->sq_lock, flags);
				runsyncq(sq, &flags);
				spin_unlock_irqrestore(&sq->sq_lock, flags);
			}
		}
	} while (test_bit(qsyncflag, &t->flags));
}
#endif

/*
 *  bufcalls:	- process bufcalls
 *  @t:		STREAMS execution thread
 *
 *  First order of business for runqueues() is to call bufcalls if there are bufcalls waiting and
 *  there are now blocks available.  We only keep track of blocks and memory that the streams
 *  subsystem frees, so streams modules will only be allowed to proceed if other modules free
 *  something.  Unfortunately, this means that the streams subsystem can lock.  If all modules hang
 *  onto their memory and blocks, and request more, and fail on allocation, then the streams
 *  subsystem will hang until an external event kicks it.  Therefore, we kick the chain every time
 *  an allocation is successful.
 */
static inline void
bufcalls(struct strthread *t)
{
	register struct strevent *se, *se_next;

	do {
		clear_bit(strbcwait, &t->flags);
		if ((se_next = xchg(&t->strbcalls_head, NULL))) {	/* MP-SAFE */
			t->strbcalls_tail = &t->strbcalls_head;
			while ((se = se_next)) {
				se_next = xchg(&se->se_next, NULL);
				/* this might further defer against a synchronization queue */
				sq_doevent(se);
			}
		}
	} while (test_bit(strbcwait, &t->flags));
}

/**
 *  queuerun:	- process service procedures
 *  @t:		STREAMS execution thread
 *
 *  Run queue service procedures.
 */
static inline void
queuerun(struct strthread *t)
{
	register queue_t *q, *q_link;

	do {
		clear_bit(qrunflag, &t->flags);
		if ((q_link = xchg(&t->qhead, NULL))) {	/* MP-SAFE */
			t->qtail = &t->qhead;
			while ((q = q_link)) {
				q_link = xchg(&q->q_link, NULL);
				srvp(q);
			}
		}
	} while (test_bit(qrunflag, &t->flags));
}

/**
 *  setqsched:	- schedule execution of queue procedures
 */
void inline
setqsched(void)
{
	struct strthread *t = this_thread;

	if (!test_and_set_bit(qrunflag, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
}

EXPORT_SYMBOL(setqsched);	/* include/sys/streams/stream.h */

/**
 *  qready:	- test if queue procedures are scheduled
 */
int inline
qready(void)
{
	struct strthread *t = this_thread;

	return (test_bit(qrunflag, &t->flags) != 0);
}

EXPORT_SYMBOL(qready);		/* include/sys/streams/stream.h */

/*
 *  qschedule:	- schedule a queue for service
 *  @q:		queue to schedule for service
 *
 *  NOTICES: Note that, for MP safety, queues are always scheduled against the same CPU that enabled
 *  the queue.  This means that the service procedure will not run until after the caller exits or
 *  hits a pre-emption point.
 *
 *  MP-STREAMS: Note that because we are just pushing the tail, the atomic exchange takes care of
 *  concurrency and other exclusion measures are not necessary here.
 */
static void
qschedule(queue_t *q)
{
	if (!test_and_set_bit(QENAB_BIT, &q->q_flag)) {
		struct strthread *t = this_thread;

		/* put ourselves on the run list */
		*xchg(&t->qtail, &q->q_link) = qget(q);	/* MP-SAFE */
		setqsched();
	}
}

#if defined CONFIG_STREAMS_SYNCQS
/**
 *  sqsched - schedule a synchronization queue
 *  @sq: the synchronization queue to schedule
 *
 *  sqsched() schedules the synchronization queue @sq to have its backlog of events serviced, if
 *  necessary.  sqsched() is called when the last thread leaves a sychronization queue (barrier).
 *
 *  MP-STREAMS: Note that because we are just pushing the tail, the atomic exchange takes care of
 *  concurrency and other exclusion measures are not necessary here.
 *
 *  IMPLEMENTATION:  I don't think that we should do this.  When we leave a barrier, we should check
 *  if we can simply run the backlog and, if we can, simply run the backlog in the current thread.
 */
void
sqsched(syncq_t *sq)
{
	/* called with sq locked */
	if (!sq->sq_link) {
		struct strthread *t = this_thread;

		/* put ourselves on the run list */
		*xchg(&t->sqtail, &sq->sq_link) = sq_get(sq);	/* MP-SAFE */
		if (!test_and_set_bit(qsyncflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
	}
}
#endif

/**
 *  qenable:	- schedule a queue for execution
 *  @q:		queue to schedule for service
 *
 *  Another name for qschedule(9), qenable() schedules a queue for service regardless of the setting
 *  of the %QNOENB_BIT.
 */
void
qenable(queue_t *q)
{
	(void) qschedule(q);
}

EXPORT_SYMBOL(qenable);		/* include/sys/streams/stream.h */

/**
 *  enableq:	- enable a queue service procedure
 *  @q:		queue for which service procedure is to be enabled
 *
 *  Schedule a queue's service procedure for execution.  enableq() only sechdules a queue service
 *  procedure if a service procedure exists for @q, and if the queue has not been previously
 *  noenabled with noenable() (i.e. the %QNOENB flag is set on the queue).
 */
__EXTERN_INLINE int enableq(queue_t *q);

EXPORT_SYMBOL(enableq);		/* include/sys/streams/stream.h */

/**
 *  enableok:	- permit scheduling of a queue service procedure
 *  @q:		queue to permit service procedure scheduling
 *
 *  This function simply clears the %QNOENB flag on the queue.  It does not schedule the queue.
 *  That must be done with a separate call to enableq() or qenable().
 */
__EXTERN_INLINE void enableok(queue_t *q);

EXPORT_SYMBOL(enableok);	/* include/sys/streams/stream.h */

/**
 *  noenable:	- defer scheduling of a queue service procedure
 *  @q:		queue to defer service procedure scheduling
 *
 *  This function simply sets the %QNOENB flag on the queue.
 */
__EXTERN_INLINE void noenable(queue_t *q);

EXPORT_SYMBOL(noenable);	/* include/sys/streams/stream.h */

/*
 *  freechains:	- free chains of message blocks
 *  @t:		STREAMS execution thread
 *
 *  Free chains of message blocks outstanding from flush operations that were left over at the end
 *  of the CPU run.
 */
static inline void
freechains(struct strthread *t)
{
	register mblk_t *mp, *mp_next;

	clear_bit(flushwork, &t->flags);
	if ((mp_next = xchg(&t->freemsg_head, NULL))) {	/* MP-SAFE */
		t->freemsg_tail = &t->freemsg_head;
		while ((mp = mp_next)) {
			mp_next = xchg(&mp->b_next, NULL);
			/* fake out freeb */
			qput(&mp->b_queue);
			bput(&mp->b_bandp);
			mp->b_next = mp->b_prev = NULL;
			freemsg(mp);
		}
	}
}

/*
 *  freechain:	- place a chain of message blocks on the free list
 *  @mp:	head of message block chain
 *  @mpp:	
 *
 *  Frees a chain of message blocks to the freechains list.  The freechains list contians one big
 *  chain of message blocks formed by concatenating these freed chains.  They will be dealt with at
 *  the end of the STREAMS scheduler SOFTIRQ run.
 */
void
freechain(mblk_t *mp, mblk_t **mpp)
{
	struct strthread *t = this_thread;

	*xchg(&t->freemsg_tail, mpp) = mp;	/* MP-SAFE */
	if (!test_and_set_bit(flushwork, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
}

/*
 *  _runqueues:	- run scheduled STRAMS events on the current processor
 *  @unused:	unused
 *
 *  This is the internal softirq version of runqueues().
 */
static void
_runqueues(struct softirq_action *unused)
{
	struct strthread *t = this_thread;

	set_bit(queueflag, &t->flags);
#if defined CONFIG_STREAMS_SYNCQS
	/* catch up on backlog first */
	if (test_bit(qsyncflag, &t->flags))
		backlog(t);
#endif
	/* do timeouts */
	if (test_bit(strtimout, &t->flags))
		timeouts(t);
	/* do pending events */
	if (test_bit(strevents, &t->flags))
		doevents(t);
	/* do buffer calls if necessary */
	if (test_bit(strbcflag, &t->flags) || test_bit(strbcwait, &t->flags))
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

/**
 *  _runqueues:	- run scheduled STRAMS events on the current processor
 *  @unused:	unused
 *
 *  This is the external version of runqueues() that can be called in user context at the end of a
 *  system call.  All stream heads (regular stream head, fifo/pipe stream head, socket stream
 *  head) need this function exported so that they can be called at the end of a system call.
 */
void
runqueues(void)
{
	local_bh_disable();	/* simulates softirq context */
	if (this_thread->
	    flags & (QSYNCFLAG | STRTIMOUT | STREVENTS | STRBCFLAG | STRBCWAIT | QRUNFLAG |
		     FLUSHWORK | FREEBLKS))
		_runqueues(NULL);
	local_bh_enable();	/* go back to user context */
}

EXPORT_SYMBOL(runqueues);	/* include/sys/streams/strsubr.h */

/* 
 *  -------------------------------------------------------------------------
 *
 *  SHINFO ctors and dtors
 *
 *  -------------------------------------------------------------------------
 *  Keep the cache ctors and the object ctors and dtors close to each other.
 */
static void
clear_shinfo(struct shinfo *sh)
{
	struct stdata *sd = &sh->sh_stdata;

	bzero(sh, sizeof(*sh));
	atomic_set(&sh->sh_refs, 0);
#if defined CONFIG_STREAMS_DEBUG
	INIT_LIST_HEAD(&sh->sh_list);
#endif
	sd->sd_rdopt = RNORM | RPROTNORM;
	sd->sd_wropt = 0;
	sd->sd_eropt = RERRNORM | WERRNORM;
	sd->sd_closetime = sysctl_str_cltime;
//      sd->sd_rtime = sysctl_str_rtime;
	init_waitqueue_head(&sd->sd_waitq);
	slockinit(sd);
//      init_MUTEX(&sd->sd_mutex);
}
static void
shinfo_ctor(void *obj, kmem_cache_t *cachep, unsigned long flags)
{
	if ((flags & (SLAB_CTOR_VERIFY | SLAB_CTOR_CONSTRUCTOR)) == SLAB_CTOR_CONSTRUCTOR)
		clear_shinfo(obj);
}
struct stdata *
allocstr(void)
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
			list_add_tail(&sh->sh_list, &si->si_head);
			write_unlock_irqrestore(&si->si_rwlock, flags);
		}
#endif
		atomic_inc(&si->si_cnt);
		if (atomic_read(&si->si_cnt) > si->si_hwl)
			si->si_hwl = atomic_read(&si->si_cnt);
	}
	return (sd);
}

EXPORT_SYMBOL(allocstr);	/* include/sys/streams/strsubr.h */

static void
__freestr(struct stdata *sd)
{
	struct strinfo *si = &Strinfo[DYN_STREAM];
	struct shinfo *sh = (struct shinfo *) sd;

#if defined(CONFIG_STREAMS_DEBUG) || defined (CONFIG_STREAMS_MNTSPECFS)
	{
		unsigned long flags;

		write_lock_irqsave(&si->si_rwlock, flags);
		list_del_init(&sh->sh_list);
		write_unlock_irqrestore(&si->si_rwlock, flags);
	}
#endif
	/* clear structure before putting it back */
	clear_shinfo(sh);
	atomic_dec(&si->si_cnt);
	kmem_cache_free(si->si_cache, sh);
}

void
freestr(struct stdata *sd)
{
	/* FIXME: need to deallocate anything attached to the stream head */
	sd_put(sd);
}

EXPORT_SYMBOL(freestr);		/* include/sys/streams/strsubr.h */

struct stdata *
sd_get(struct stdata *sd)
{
	if (sd) {
		struct shinfo *sh = (struct shinfo *) sd;

		if (atomic_read(&sh->sh_refs) <= 0)
			swerr();
		atomic_inc(&sh->sh_refs);
	} else
		swerr();
	return (sd);
}

#if defined CONFIG_STREAMS_STH_MODULE || !defined CONFIG_STREAMS_STH
EXPORT_SYMBOL(sd_get);		/* include/sys/streams/strsubr.h */
#endif
void
sd_put(struct stdata *sd)
{
	if (sd) {
		struct shinfo *sh = (struct shinfo *) sd;

		if (atomic_dec_and_test(&sh->sh_refs)) {
			/* the last reference is gone, there should be nothing left */
			if (sd->sd_rq)
				freeq(sd->sd_rq);
			if (sd->sd_iocblk)
				freemsg(sd->sd_iocblk);
			__freestr(sd);
		}
	} else
		swerr();
	return;
}

#if defined CONFIG_STREAMS_STH_MODULE || !defined CONFIG_STREAMS_STH
EXPORT_SYMBOL(sd_put);		/* include/sys/streams/strsubr.h */
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
		"DYN_MODINFO", sizeof(struct mdlinfo), 0, SLAB_HWCACHE_ALIGN, &mdlinfo_ctor, NULL
#if defined CONFIG_STREAMS_SYNCQS
	}, {
		"DYN_SYNCQ", sizeof(struct syncq), 0, SLAB_HWCACHE_ALIGN, &syncq_ctor, NULL
#endif
	}
};

/* Note: that we only have one cache for both MSGBLOCKs and MDBBLOCKs */

/*
 *  str_term_caches:	- terminate caches
 */
static void
str_term_caches(void)
{
	int j;
	struct strinfo *si = Strinfo;
	struct cacheinfo *ci = Cacheinfo;

	for (j = 0; j < DYN_SIZE; j++, si++, ci++) {
		if (!si->si_cache)
			continue;
#if defined CONFIG_STREAMS_DEBUG
		/* if we are tracking the allocations we can kill things whether they refer to each 
		   other or not. I hope we don't have any inodes kicking around... */
#endif
		if (kmem_cache_destroy(xchg(&si->si_cache, NULL)) == 0)
			continue;
		printk(KERN_ERR "%s: could not destroy %s cache\n", __FUNCTION__, ci->name);
		swerr();
	}
}

/*
 *  str_init_caches:	- initialize caches
 */
static int
str_init_caches(void)
{
	int j;
	struct strinfo *si = Strinfo;
	struct cacheinfo *ci = Cacheinfo;

	for (j = 0; j < DYN_SIZE; j++, si++, ci++) {
#if defined CONFIG_STREAMS_DEBUG
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

#ifndef open_softirq
#ifdef HAVE_OPEN_SOFTIRQ_ADDR
void
open_softirq(int nr, void (*action) (struct softirq_action *), void *data)
{
	static void (*func) (int, void (*)(struct softirq_action *), void *) =
	    (typeof(func)) HAVE_OPEN_SOFTIRQ_ADDR;
	return func(nr, action, data);
}
#endif
#endif

/**
 *  strsched_init:  - initialize the STREAMS scheduler
 */
int
strsched_init(void)
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
	open_softirq(STREAMS_SOFTIRQ, _runqueues, NULL);
	return (0);
}

void
strsched_exit(void)
{
	open_softirq(STREAMS_SOFTIRQ, NULL, NULL);
	str_term_caches();
}
