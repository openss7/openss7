/*****************************************************************************

 @(#) $Id: strsched.h,v 0.9.2.33 2007/03/28 13:44:18 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/03/28 13:44:18 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strsched.h,v $
 Revision 0.9.2.33  2007/03/28 13:44:18  brian
 - updates to syncrhonization, release notes and documentation

 Revision 0.9.2.32  2007/03/25 00:52:47  brian
 - synchronization updates

 Revision 0.9.2.31  2007/03/02 09:23:29  brian
 - build updates and esballoc() feature

 Revision 0.9.2.30  2006/12/18 10:08:59  brian
 - updated headers for release

 *****************************************************************************/

#ifndef __LOCAL_STRSCHED_H__
#define __LOCAL_STRSCHED_H__

#ident "@(#) $RCSfile: strsched.h,v $ $Name:  $($Revision: 0.9.2.33 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __EXTERN
#define __EXTERN extern
#endif

#ifndef __STREAMS_EXTERN
#define __STREAMS_EXTERN __EXTERN streams_fastcall
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __SCHED_EXTERN_INLINE
#define __SCHED_EXTERN_INLINE __EXTERN_INLINE streams_fastcall __unlikely
#endif

#ifndef BIG_STATIC
#define BIG_STATIC __EXTERN
#endif

#ifndef BIG_STATIC_INLINE
#define BIG_STATIC_INLINE __EXTERN_INLINE
#endif

#ifndef BIG_STATIC_STH
#define BIG_STATIC_STH __EXTERN
#endif

#ifndef BIG_STATIC_INLINE_STH
#define BIG_STATIC_INLINE_STH __EXTERN_INLINE
#endif

#undef STR
#include <linux/interrupt.h>	/* for in_irq() and friends */
#if defined HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif

#ifndef STREAMS_SOFTIRQ
#define STREAMS_SOFTIRQ (TASKLET_SOFTIRQ+1)
#endif

/* ctors and dtors for mdbblocks */
BIG_STATIC mblk_t *streams_fastcall mdbblock_alloc(uint priority, void *func);
BIG_STATIC void streams_fastcall mdbblock_free(mblk_t *mp);

/* queue gets and puts */
BIG_STATIC queue_t *streams_fastcall qget(queue_t *q);
BIG_STATIC void streams_fastcall qput(queue_t **qp);

/* ctors and dtors for queue bands */
BIG_STATIC struct qband *streams_fastcall allocqb(void);
BIG_STATIC void streams_fastcall freeqb(struct qband *qb);

#if 0
/* queue band gets and puts */
BIG_STATIC qband_t *streams_fastcall bget(qband_t *qb);
BIG_STATIC void streams_fastcall bput(qband_t **bp);
#endif

/* ctors and dtors for stream heads */
__STREAMS_EXTERN struct stdata *allocstr(void);
__STREAMS_EXTERN void freestr(struct stdata *sd);
BIG_STATIC_STH struct stdata *streams_fastcall sd_get(struct stdata *sd);
BIG_STATIC_STH void streams_fastcall sd_put(struct stdata **sdp);

/* ctors and dtors for autopush entries */
BIG_STATIC struct apinfo *streams_fastcall ap_alloc(struct strapush *sap);
BIG_STATIC struct apinfo *streams_fastcall ap_get(struct apinfo *api);
BIG_STATIC void streams_fastcall ap_put(struct apinfo *api);

/* XXX: not even in strsched.c */
BIG_STATIC_STH int streams_fastcall autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag,
			    int sflag, cred_t *crp);

#if 0
/* ctors and dtors for devinfo */
BIG_STATIC struct devinfo *streams_fastcall di_alloc(struct cdevsw *cdev);
BIG_STATIC struct devinfo *streams_fastcall di_get(struct devinfo *di);
BIG_STATIC void streams_fastcall di_put(struct devinfo *di);

/* ctors and dtors for mdlinfo */
BIG_STATIC struct mdlinfo *streams_fastcall modi_alloc(struct fmodsw *fmod);
BIG_STATIC struct mdlinfo *streams_fastcall modi_get(struct mdlinfo *mi);
BIG_STATIC void streams_fastcall modi_put(struct mdlinfo *mi);
#endif

/* ctors and dtors for linkblk */
BIG_STATIC_STH struct linkblk *streams_fastcall alloclk(void);
BIG_STATIC_STH void streams_fastcall freelk(struct linkblk *l);

#if defined CONFIG_STREAMS_SYNCQS
/* ctors and dtors for syncq */
BIG_STATIC struct syncq *streams_fastcall sq_locate(const char *sq_info);
BIG_STATIC struct syncq *streams_fastcall sq_alloc(void);
BIG_STATIC struct syncq *streams_fastcall sq_get(struct syncq *sq);
BIG_STATIC void streams_fastcall sq_put(struct syncq **sqp);
BIG_STATIC void streams_fastcall sq_release(struct syncq **sqp);
#endif

/* freeing chains of message blocks */
BIG_STATIC void streams_fastcall freechain(mblk_t *mp, mblk_t **mpp);

/* force scheduling queues */
// __STREAMS_EXTERN void qschedule(queue_t *q);

#if defined CONFIG_STREAMS_SYNCQS
							     /* synq functions *//* XXX: not even in strsched.c */
__STREAMS_EXTERN void __defer_put(syncq_t *sq, queue_t *q, mblk_t *mp);
#endif

/* stuff for examining streams information lists */
BIG_STATIC struct streams_fastcall strinfo Strinfo[DYN_SIZE];
BIG_STATIC_STH struct strthread strthreads[NR_CPUS] ____cacheline_aligned;

#define this_thread (&strthreads[smp_processor_id()])

typedef enum {
	CTX_PROC,			/* process context */
	CTX_ATOMIC,			/* atomic (no postpone) context */
	CTX_STREAMS,			/* STREAMS softirq interrupt context */
	CTX_INT,			/* soft interrupt context */
	CTX_ISR,			/* hard interrupt context */
} context_t;

#ifdef CONFIG_STREAMS_KTHREADS
#define in_streams() (!in_interrupt() && atomic_read(&this_thread->lock) != 0)
#else				/* CONFIG_STREAMS_KTHREADS */
#define in_streams() (!in_irq() && atomic_read(&this_thread->lock) != 0)
#endif				/* CONFIG_STREAMS_KTHREADS */

#define can_enter_streams() (atomic_read(&this_thread->lock) == 0 || !in_interrupt())

#define local_str_disable() \
do { \
	struct strthread *t = this_thread; \
	atomic_inc(&t->lock); \
} while (0)

#define local_str_enable() \
do { \
	struct strthread *t = this_thread; \
	if (atomic_dec_and_test(&t->lock)) \
		if (unlikely(test_and_clear_bit(qwantrun, &t->flags))) \
			if (unlikely((t->flags & (QRUNFLAGS)) != 0)) \
				__raise_streams(); \
} while (0)

#if defined CONFIG_STREAMS_KTHREADS
#define enter_streams() \
do { \
} while (0)

#define leave_streams() \
do { \
} while (0)
#else				/* defined CONFIG_STREAMS_KTHREADS */
#define enter_streams() \
do { \
	local_bh_disable(); \
} while (0)

#define leave_streams() \
do { \
	local_bh_enable(); \
} while (0)
#endif				/* defined CONFIG_STREAMS_KTHREADS */

#if 0
__SCHED_EXTERN_INLINE streams_fastcall context_t
current_context(void)
{
	if (in_irq())
		return (CTX_ISR);
	if (in_streams())
		return (CTX_STREAMS);
	if (in_interrupt())
		return (CTX_INT);
#if defined HAVE_KFUNC_IN_ATOMIC
	if (in_atomic())
		return (CTX_ATOMIC);
#endif
	return (CTX_PROC);
}
#endif

/* for initialization */
BIG_STATIC int strsched_init(void);
BIG_STATIC void strsched_exit(void);

#if 0
#if defined CONFIG_STREAMS_SYNCQS
BIG_STATIC void streams_fastcall sqsched(syncq_t *sq);
#endif
#endif
#endif				/* __LOCAL_STRSCHED_H__ */
