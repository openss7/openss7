/*****************************************************************************

 @(#) $RCSfile: strsched.h,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/08/22 06:17:55 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2004/08/22 06:17:55 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRSCHED_H__
#define __LOCAL_STRSCHED_H__

#undef STR
#include <linux/interrupt.h>	/* for in_irq() and friends */

#ifndef __SCHED_EXTERN_INLINE
#define __SCHED_EXTERN_INLINE extern __inline__
#endif

#ifndef STREAMS_SOFTIRQ
#define STREAMS_SOFTIRQ (TASKLET_SOFTIRQ+1)
#endif

/* ctors and dtors for mdbblocks */
extern mblk_t *mdbblock_alloc(uint priority, void *func);
extern void mdbblock_free(mblk_t *mp);

/* ctors and dtors for queue bands */
extern struct qband *allocqb(void);
extern void freeqb(struct qband *qb);

/* ctors and dtors for stream heads */
extern struct stdata *allocsd(void);
extern void freesd(struct stdata *sd);
extern struct stdata *sd_get(struct stdata *sd);
extern void sd_put(struct stdata *sd);

/* ctors and dtors for autopush entries */
extern struct apinfo *ap_alloc(struct strapush *sap);
extern struct apinfo *ap_get(struct apinfo *api);
extern void ap_put(struct apinfo *api);
extern int autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag, int sflag,
		    cred_t *crp);

/* ctors and dtors for devinfo */
extern struct devinfo *di_alloc(struct cdevsw *cdev);
extern struct devinfo *di_get(struct devinfo *di);
extern void di_put(struct devinfo *di);

/* ctors and dtors for modinfo */
extern struct modinfo *mi_alloc(struct fmodsw *fmod);
extern struct modinfo *mi_get(struct modinfo *mi);
extern void mi_put(struct modinfo *mi);

/* ctors and dtors for linkblk */
extern struct linkblk *alloclk(void);
extern void freelk(struct linkblk *l);

/* ctors and dtors for syncq */
extern struct syncq *sq_alloc(void);
extern struct syncq *sq_get(struct syncq *sq);
extern void sq_put(struct syncq **sqp);

/* freeing chains of message blocks */
extern void freechain(mblk_t *mp, mblk_t **mpp);

/* synq functions */
extern void __defer_put(syncq_t *sq, queue_t *q, mblk_t *mp);

/* stuff for examining streams information lists */
extern struct strinfo Strinfo[DYN_SIZE];
extern struct strthread strthreads[NR_CPUS] ____cacheline_aligned;

#define this_thread (&strthreads[smp_processor_id()])

typedef enum {
	CTX_PROC,			/* process context */
	CTX_STREAMS,			/* STREAMS softirq interrupt context */
	CTX_INT,			/* soft interrupt context */
	CTX_ISR,			/* hard interrupt context */
} context_t;

__SCHED_EXTERN_INLINE int in_streams(void)
{
	return (!in_irq() && (this_thread->flags & QUEUEFLAG));
}
__SCHED_EXTERN_INLINE context_t current_context(void)
{
	if (in_irq())
		return (CTX_ISR);
	if (this_thread->flags & QUEUEFLAG)
		return (CTX_STREAMS);
	if (in_interrupt())
		return (CTX_INT);
	return (CTX_PROC);
}

/* for initialization */
extern int strsched_init(void);
extern void strsched_exit(void);
extern void sqsched(syncq_t *sq);
#endif				/* __LOCAL_STRSCHED_H__ */
