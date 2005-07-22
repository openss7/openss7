/*****************************************************************************

 @(#) $RCSfile: strsched.c,v $ $Name:  $($Revision: 0.9.2.51 $) $Date: 2005/07/21 22:52:09 $

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

 Last Modified $Date: 2005/07/21 22:52:09 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strsched.c,v $ $Name:  $($Revision: 0.9.2.51 $) $Date: 2005/07/21 22:52:09 $"

static char const ident[] =
    "$RCSfile: strsched.c,v $ $Name:  $($Revision: 0.9.2.51 $) $Date: 2005/07/21 22:52:09 $";

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

#if defined CONFIG_STREAMS_DEBUG
EXPORT_SYMBOL(strthreads);
#endif

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

/* use the current queue being processed by the thread as a guess as to which queue the thing
   (message block, bufcall callback, timout callback) belongs to, but, if we are at irq, then we
   will only used the supplied value, even if null. */
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
#if defined CONFIG_STREAMS_DEBUG
		struct strinfo *smi = &Strinfo[DYN_MSGBLOCK];
		struct mdbblock *md = (struct mdbblock *) mp;

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
	*xchg(&t->freemblk_tail, &mp->b_next) = mp;
	if (!test_and_set_bit(freeblks, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
	if (test_bit(strbcwait, &t->flags))
		if (!test_and_set_bit(strbcflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
	return;
}

/* 
 *  freeblocks: - free message blocks
 *  @t:	    the STREAMS executive thread
 *
 *  freeblocks() is invoked by runqueues() when there are blocks to free (i.e.  the %FREEBLKS flag is
 *  set on the thread).  It frees all blocks from the free list.  We first steal the entire list and
 *  then work on them one by one.
 */
static void
freeblocks(struct strthread *t)
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
EXPORT_SYMBOL(di_alloc);
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
EXPORT_SYMBOL(di_put);
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

EXPORT_SYMBOL(flushq);

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

EXPORT_SYMBOL(allocq);

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

EXPORT_SYMBOL(freeq);

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
EXPORT_SYMBOL(alloclk);
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
EXPORT_SYMBOL(freelk);
#endif

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
#if 0
		init_waitqueue_head(&sq->sq_waitq);
#endif
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

			write_lock_irqsave(&si->si_rwlock, flags);
			list_del_init((struct list_head *) &sq->sq_next);
			write_unlock_irqrestore(&si->si_rwlock, flags);
			/* clean it up before puting it back in the cache */
			// sq_put(&sq->sq_other); /* recurse once */
			if (!spin_trylock(&sq->sq_lock))
				pswerr(("breaking syncq lock\n"));
			spin_lock_init(&sq->sq_lock);
			if (sq->sq_head != NULL)
				pswerr(("events lost!\n"));
			sq->sq_head = NULL;
			sq->sq_tail = &sq->sq_head;
#if 0
			if (waitqueue_active(&sq->sq_waitq))
				pswerr(("destroying syncq with waiters!\n"));
			wake_up_all(&sq->sq_waitq);
			init_waitqueue_head(&sq->sq_waitq);
#endif
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

EXPORT_SYMBOL(sealloc);

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

EXPORT_SYMBOL(sefree);

/*
 *  __bufcall:	- generate a buffer callback
 *  @q:		queue against which to synchronize callback
 *  @size:	size of message block requested
 *  @priority:	priority of message block request
 *  @function:	callback function
 *  @arg:	argument to callback function
 *
 *  Notices: Note that, for MP safety, bufcalls are always raised against the same processor that
 *  invoked the buffer call.  This means that the callback function will not execute until after the
 *  caller exits or hits a pre-emption point.
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
		*xchg(&t->strbcalls_tail, &se->se_next) = se;
		set_bit(strbcwait, &t->flags);
	}
	return (bcid);
}

EXPORT_SYMBOL(__bufcall);

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

EXPORT_SYMBOL(bufcall);

/**
 *  esbbcall:	- schedule a buffer callout
 *  @priority:	the priority of the buffer allocation (ignored)
 *  @function:	the callback function when bytes and headers are available
 *  @arg:	a client argument to pass to the callback function
 */
__EXTERN_INLINE bcid_t esbbcall(int priority, void (*function) (long), long arg);

EXPORT_SYMBOL(esbbcall);

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

EXPORT_SYMBOL(unbufcall);

static void
timeout_function(unsigned long arg)
{
	struct strevent *se = (struct strevent *) arg;
	struct strthread *t = &strthreads[se->x.t.cpu];

	/* FIXME: this is dangerous - need a per-cpu list spin */
	*xchg(&t->strtimout_tail, &se->se_next) = se;
	/* bind timeout back to the CPU that called for it */
	if (!test_and_set_bit(strtimout, &t->flags))
#if HAVE_KFUNC_CPU_RAISE_SOFTIRQ
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
		toid = (se->se_id << EVENT_ID_SHIFT) | (se->se_seq & EVENT_SEQ_MASK);
		add_timer(&se->x.t.timer);
	}
	return (toid);
}

EXPORT_SYMBOL(__timeout);

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

EXPORT_SYMBOL(timeout);

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

EXPORT_SYMBOL(untimeout);

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
		*xchg(&t->strevents_tail, &se->se_next) = se;
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

EXPORT_SYMBOL(weldq);

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

EXPORT_SYMBOL(unweldq);

/*
 *  defer_func:	- defer a STREAMS procedure call
 *  @func:	function call to defer
 *  @q:		associated queue
 *  @mp:	associated message block
 *  @arg:	associated argument
 *  @perim:	sychornization perimiter
 *  @type:	function call type, currently %SE_STRPUT or %SE_WRITER
 *
 *  Deferrable functions that use defer_func() are streams_put() and qwriter().
 */
int
defer_func(void (*func) (void *, mblk_t *), queue_t *q, mblk_t *mp, void *arg, int perim, int type)
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
		if (!test_and_set_bit(strevents, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
		return (0);
	}
	return (ENOMEM);
}

EXPORT_SYMBOL(defer_func);

/* 
 *  DEFERRAL FUNCTION ON SYNCH QUEUES
 *  -------------------------------------------------------------------------
 */
void
__defer_put(syncq_t *sq, queue_t *q, mblk_t *mp)
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
static void
defer_event(syncq_t *sq, struct strevent *se, unsigned long *flagsp)
{
	*xchg(&sq->sq_tail, &se->se_next) = se;
	unlock_syncq(sq, flagsp);
}

/*
 *  sq_stream:	- execute deferred %SE_STREAM event
 *  @se:	deferred event
 */
static void
sq_stream(struct strevent *se)
{
}

/*
 *  sq_bufcall:	- execute deferred %SE_BUFCALL event
 *  @se:	deferred event
 */
static void
sq_bufcall(struct strevent *se)
{
	queue_t *q = se->x.b.queue;
	syncq_t *isq = q ? q->q_syncq : NULL, *osq = isq ? isq->sq_outer : isq;	/* XXX */
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

/*
 *  sq_timeout:	- execute deferred %SE_TIMEOUT event
 *  @se:	deferred event
 */
static void
sq_timeout(struct strevent *se)
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

/*
 *  sq_weldq:	- execute deferred %SE_WELDQ event
 *  @se:	deferred event
 */
static void
sq_weldq(struct strevent *se)
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

/*
 *  sq_unweldq:	- execute deferred %SE_UNWELDQ event
 *  @se:	deferred event
 */
static void
sq_unweldq(struct strevent *se)
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

/*
 *  sq_strput:	- execute deferred %SE_STRPUT event
 *  @se:	deferred event
 */
static void
sq_strput(struct strevent *se)
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

/*
 *  sq_writer:	- execute deferred %SE_WRITER event
 *  @se:	deferred event
 */
static void
sq_writer(struct strevent *se)
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

/*
 *  sq_putp:	- execute deferred %SE_PUTP event
 *  @se:	deferred event
 */
static void
sq_putp(struct strevent *se)
{
	/* this will defer itself if necessary */
	se->x.p.func(se->x.p.arg, se->x.p.mp);
	event_free(se);
}

/**
 *  kmem_alloc:	- allocate memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 */
__EXTERN_INLINE void *kmem_alloc(size_t size, int flags);

EXPORT_SYMBOL(kmem_alloc);

/**
 *  kmem_zalloc: - allocate and zero memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 */
__EXTERN_INLINE void *kmem_zalloc(size_t size, int flags);

EXPORT_SYMBOL(kmem_zalloc);

/**
 *  kmem_free:	- free memory
 *  @addr:	address of memory
 *  @size:	amount of memory to free
 *
 *  Frees memory allocated with kmem_alloc() or kmem_zalloc().
 */
void
kmem_free(void *addr, size_t size)
{
	kfree(addr);
	if (size) {
		struct strthread *t = this_thread;

		if (test_bit(strbcwait, &t->flags) && !test_and_set_bit(strbcflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
	}
}

EXPORT_SYMBOL(kmem_free);

/**
 *  kmem_alloc_node: - allocate memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 *  @node:
 */
__EXTERN_INLINE void *kmem_alloc_node(size_t size, int flags, cnodeid_t node);

EXPORT_SYMBOL(kmem_alloc_node);

/**
 *  kmem_zalloc: - allocate and zero memory
 *  @size:	amount of memory to allocate in bytes
 *  @flags:	either %KM_SLEEP or %KM_NOSLEEP
 *  @node:
 */
__EXTERN_INLINE void *kmem_zalloc_node(size_t size, int flags, cnodeid_t node);

EXPORT_SYMBOL(kmem_zalloc_node);

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
 */
static inline void
timeouts(struct strthread *t)
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

/*
 *  doevents:	- process STREAMS events
 *  @t:		STREAMS execution thread
 */
static inline void
doevents(struct strthread *t)
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
					unsigned long flags = 0;

					hwlock(se->x.w.q1, &flags);
					se->x.w.q1->q_next = se->x.w.q2;
					hwunlock(se->x.w.q1, &flags);
					qput(&se->x.w.q1);
					qput(&se->x.w.q2);
				}
				if (se->x.w.q3) {
					unsigned long flags = 0;

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

/*
 *  backlog:	- process message backlog
 *  @t:		STREAMS execution thread
 */
static inline void
backlog(struct strthread *t)
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

/*
 *  queuerun:	- process service procedures
 *  @t:		STREAMS execution thread
 */
static inline void
queuerun(struct strthread *t)
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

EXPORT_SYMBOL(setqsched);

/**
 *  qready:	- test if queue procedures are scheduled
 */
int inline
qready(void)
{
	struct strthread *t = this_thread;

	return (test_bit(qrunflag, &t->flags) != 0);
}

EXPORT_SYMBOL(qready);

/**
 *  qschedule:	- schedule a queue for service
 *  @q:		queue to schedule for service
 *
 *  Notices: Note that, for MP safety, queues are always scheduled against the same CPU that enabled
 *  the queue.  This means that the service procedure will not run until after the caller exits or
 *  hits a pre-emption point.
 */
static void
qschedule(queue_t *q)
{
	if (!test_and_set_bit(QENAB_BIT, &q->q_flag)) {
		struct strthread *t = this_thread;

		/* put ourselves on the run list */
		*xchg(&t->qtail, &q->q_link) = qget(q);
		setqsched();
	}
}

/**
 *  sqsched - schedule a synchronization queue
 *  @sq: the synchronization queue to schedule
 *
 *  sqsched() schedules the synchronization queue @sq to have its backlog of events serviced, if
 *  necessary.  sqsched() is called when the last thread leaves a sychronization queue (barrier).
 */
void
sqsched(syncq_t *sq)
{
	/* called with sq locked */
	if (!sq->sq_link) {
		struct strthread *t = this_thread;

		/* put ourselves on the run list */
		*xchg(&t->sqtail, &sq->sq_link) = sq_get(sq);
		if (!test_and_set_bit(qsyncflag, &t->flags))
			raise_softirq(STREAMS_SOFTIRQ);
	}
}

/**
 *  enableq:	- enable a queue service procedure
 *  @q:		queue for which service procedure is to be enabled
 */
int
enableq(queue_t *q)
{
	if (q->q_qinfo->qi_srvp && !test_bit(QNOENB_BIT, &q->q_flag)) {
		qschedule(q);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL(enableq);

/**
 *  qenable:	- schedule a queue for execution
 *  @q:		queue to schedule for service
 *
 *  Another name for qschedule().
 */
void
qenable(queue_t *q)
{
	(void) qschedule(q);
}

EXPORT_SYMBOL(qenable);

/**
 *  enableok:	- permit scheduling of a queue service procedure
 *  @q:		queue to permit service procedure scheduling
 */
__EXTERN_INLINE void enableok(queue_t *q);

EXPORT_SYMBOL(enableok);

/**
 *  noenable:	- defer scheduling of a queue service procedure
 *  @q:		queue to defer service procedure scheduling
 */
__EXTERN_INLINE void noenable(queue_t *q);

EXPORT_SYMBOL(noenable);

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

	*xchg(&t->freemsg_tail, mpp) = mp;
	if (!test_and_set_bit(flushwork, &t->flags))
		raise_softirq(STREAMS_SOFTIRQ);
}

/*
 *  runqueues:	- run scheduled STRAMS events on the current processor
 *  @unused:	unused
 */
static void
runqueues(struct softirq_action *unused)
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

EXPORT_SYMBOL(allocstr);

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

EXPORT_SYMBOL(freestr);

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
EXPORT_SYMBOL(sd_get);
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
EXPORT_SYMBOL(sd_put);
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
	"DYN_MODINFO", sizeof(struct mdlinfo), 0, SLAB_HWCACHE_ALIGN, &mdlinfo_ctor, NULL}, {
	"DYN_SYNCQ", sizeof(struct syncq), 0, SLAB_HWCACHE_ALIGN, &syncq_ctor, NULL}
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
	open_softirq(STREAMS_SOFTIRQ, runqueues, NULL);
	return (0);
}

void
strsched_exit(void)
{
	open_softirq(STREAMS_SOFTIRQ, NULL, NULL);
	str_term_caches();
}
