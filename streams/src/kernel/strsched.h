/*****************************************************************************

 @(#) $RCSfile: strsched.h,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/12/05 22:49:05 $

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

 Last Modified $Date: 2005/12/05 22:49:05 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRSCHED_H__
#define __LOCAL_STRSCHED_H__

#ifndef BIG_STATIC
#define BIG_STATIC
#endif

#ifndef BIG_STATIC_INLINE
#define BIG_STATIC_INLINE
#endif

#undef STR
#include <linux/interrupt.h>	/* for in_irq() and friends */
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif

#ifndef __SCHED_EXTERN_INLINE
#define __SCHED_EXTERN_INLINE extern __inline__
#endif

#ifndef STREAMS_SOFTIRQ
#define STREAMS_SOFTIRQ (TASKLET_SOFTIRQ+1)
#endif

/* ctors and dtors for mdbblocks */
BIG_STATIC mblk_t *STREAMS_FASTCALL(mdbblock_alloc(uint priority, void *func));
BIG_STATIC void STREAMS_FASTCALL(mdbblock_free(mblk_t *mp));

/* queue gets and puts */
BIG_STATIC queue_t *STREAMS_FASTCALL(qget(queue_t *q));
BIG_STATIC void STREAMS_FASTCALL(qput(queue_t **qp));

/* ctors and dtors for queue bands */
BIG_STATIC struct qband *allocqb(void);
BIG_STATIC void freeqb(struct qband *qb);
#if 0
/* queue band gets and puts */
BIG_STATIC qband_t *STREAMS_FASTCALL(bget(qband_t *qb));
BIG_STATIC void STREAMS_FASTCALL(bput(qband_t **bp));
#endif

/* ctors and dtors for stream heads */
extern struct stdata *allocstr(void);
extern void freestr(struct stdata *sd);
extern struct stdata *STREAMS_FASTCALL(sd_get(struct stdata *sd));
extern void STREAMS_FASTCALL(sd_put(struct stdata **sdp));

/* ctors and dtors for autopush entries */
BIG_STATIC struct apinfo *ap_alloc(struct strapush *sap);
BIG_STATIC struct apinfo *ap_get(struct apinfo *api);
BIG_STATIC void ap_put(struct apinfo *api);
/* XXX: not even in strsched.c */
extern int autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag, int sflag,
		    cred_t *crp);

#if 0
/* ctors and dtors for devinfo */
BIG_STATIC struct devinfo *di_alloc(struct cdevsw *cdev);
BIG_STATIC struct devinfo *di_get(struct devinfo *di);
BIG_STATIC void di_put(struct devinfo *di);

/* ctors and dtors for mdlinfo */
BIG_STATIC struct mdlinfo *modi_alloc(struct fmodsw *fmod);
BIG_STATIC struct mdlinfo *modi_get(struct mdlinfo *mi);
BIG_STATIC void modi_put(struct mdlinfo *mi);
#endif

/* ctors and dtors for linkblk */
extern struct linkblk *alloclk(void);
extern void freelk(struct linkblk *l);

#if defined CONFIG_STREAMS_SYNCQS
/* ctors and dtors for syncq */
BIG_STATIC struct syncq *sq_alloc(void);
BIG_STATIC struct syncq *sq_get(struct syncq *sq);
BIG_STATIC void sq_put(struct syncq **sqp);
#endif

/* freeing chains of message blocks */
BIG_STATIC void STREAMS_FASTCALL(freechain(mblk_t *mp, mblk_t **mpp));

/* force scheduling queues */
// extern void qschedule(queue_t *q);

#if defined CONFIG_STREAMS_SYNCQS
/* synq functions */ /* XXX: not even in strsched.c */
extern void __defer_put(syncq_t *sq, queue_t *q, mblk_t *mp);
#endif

/* stuff for examining streams information lists */
BIG_STATIC struct strinfo Strinfo[DYN_SIZE];
extern struct strthread strthreads[NR_CPUS] ____cacheline_aligned;

#define this_thread (&strthreads[smp_processor_id()])

typedef enum {
	CTX_PROC,			/* process context */
	CTX_ATOMIC,			/* atomic (no postpone) context */
	CTX_STREAMS,			/* STREAMS softirq interrupt context */
	CTX_INT,			/* soft interrupt context */
	CTX_ISR,			/* hard interrupt context */
} context_t;


#define in_streams() (!in_irq() && atomic_read(&this_thread->lock) != 0)

#define can_enter_streams() (atomic_read(&this_thread->lock) == 0 || !in_interrupt())

#define local_str_disable() \
do { \
	struct strthread *t = this_thread; \
	atomic_inc(&t->lock); \
} while (0)

#define local_str_enable() \
do { \
	struct strthread *t = this_thread; \
	if (atomic_dec_and_test(&t->lock) && test_and_clear_bit(qwantrun, &t->flags)) \
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

__SCHED_EXTERN_INLINE streams_fastcall context_t
current_context(void)
{
	if (in_irq())
		return (CTX_ISR);
	if (in_streams())
		return (CTX_STREAMS);
	if (in_interrupt())
		return (CTX_INT);
#if HAVE_KFUNC_IN_ATOMIC
	if (in_atomic())
		return (CTX_ATOMIC);
#endif
	return (CTX_PROC);
}

/* for initialization */
BIG_STATIC int strsched_init(void);
BIG_STATIC void strsched_exit(void);
#if defined CONFIG_STREAMS_SYNCQS
BIG_STATIC void sqsched(syncq_t *sq);
#endif
#endif				/* __LOCAL_STRSCHED_H__ */
