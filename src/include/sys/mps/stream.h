/*****************************************************************************

 @(#) $Id: stream.h,v 1.1.2.3 2011-01-12 04:10:31 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-01-12 04:10:31 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 1.1.2.3  2011-01-12 04:10:31  brian
 - code updates for 2.6.32 kernel and gcc 4.4

 Revision 1.1.2.2  2010-11-28 14:21:51  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:26:47  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_MPS_STREAM_H__
#define __SYS_MPS_STREAM_H__

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/mps/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __MPS_EXTERN_INLINE
#define __MPS_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif

#ifndef __MPS_EXTERN
#define __MPS_EXTERN extern streamscall
#endif

#ifndef _MPS_SOURCE
#warning "_MPS_SOURCE not defined but MPS stream.h included."
#endif

#include <sys/openss7/config.h>

#if defined CONFIG_STREAMS_COMPAT_MPS || defined CONFIG_STREAMS_COMPAT_MPS_MODULE

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

/*
 *  Module or driver open and close helper functions.
 */
__MPS_EXTERN caddr_t mi_open_grab(caddr_t ptr);
__MPS_EXTERN caddr_t mi_close_put(caddr_t ptr);
__MPS_EXTERN void *mi_close_obj(caddr_t ptr);
__MPS_EXTERN size_t mi_open_size(size_t size);
__MPS_EXTERN size_t mi_close_size(caddr_t ptr);
__MPS_EXTERN caddr_t mi_open_obj(void *obj, size_t size, kmem_cachep_t cachep);
__MPS_EXTERN caddr_t mi_open_alloc(size_t size);
__MPS_EXTERN caddr_t mi_open_alloc_sleep(size_t size);
__MPS_EXTERN caddr_t mi_open_alloc_cache(kmem_cachep_t cachep, int flag);
__MPS_EXTERN caddr_t mi_first_ptr(caddr_t *mi_head);
__MPS_EXTERN caddr_t mi_first_dev_ptr(caddr_t *mi_head);
__MPS_EXTERN caddr_t mi_next_ptr(caddr_t ptr);
__MPS_EXTERN caddr_t mi_next_dev_ptr(caddr_t *mi_head, caddr_t ptr);
__MPS_EXTERN caddr_t mi_prev_ptr(caddr_t ptr);
__MPS_EXTERN int mi_open_link(caddr_t *mi_head, caddr_t ptr, dev_t *devp, int flag, int sflag,
			      cred_t *credp);
__MPS_EXTERN void mi_close_free(caddr_t ptr);
__MPS_EXTERN void mi_close_free_cache(kmem_cachep_t cachep, caddr_t ptr);

__MPS_EXTERN_INLINE caddr_t
mi_open_detached(caddr_t *mi_head, size_t size, dev_t *devp)
{
	caddr_t ptr;
	int err;

	if (!(ptr = mi_open_alloc_sleep(size)))
		return (NULL);
	if (!(err = mi_open_link(mi_head, ptr, devp, 0, DRVOPEN, NULL)))
		return (ptr);
	mi_close_free(ptr);
	return (NULL);
}

__MPS_EXTERN void mi_attach(queue_t *q, caddr_t ptr);

__MPS_EXTERN_INLINE int
mi_open_comm(caddr_t *mi_head, size_t size, queue_t *q, dev_t *devp, int flag, int sflag,
	     cred_t *credp)
{
	caddr_t ptr;
	int err;

	if (sflag == MODOPEN) {
		if (!WR(q)->q_next)
			return (EIO);
	} else {
		if (WR(q)->q_next && SAMESTR(q))
			return (EIO);
	}
	if (!(ptr = mi_open_alloc_sleep(size)))
		return (EAGAIN);
	if (!(err = mi_open_link(mi_head, ptr, devp, flag, sflag, credp))) {
		mi_attach(q, ptr);
		return (0);
	}
	mi_close_free(ptr);
	return (err);
}

__MPS_EXTERN void mi_close_unlink(caddr_t *mi_head, caddr_t ptr);

__MPS_EXTERN void mi_detach(queue_t *q, caddr_t ptr);

__MPS_EXTERN_INLINE void
mi_close_detached(caddr_t *mi_head, caddr_t ptr)
{
	mi_close_unlink(mi_head, ptr);
	mi_close_free(ptr);
}

__MPS_EXTERN_INLINE int
mi_close_comm(caddr_t *mi_head, queue_t *q)
{
	caddr_t ptr = q->q_ptr;

	mi_detach(q, ptr);
	mi_close_detached(mi_head, ptr);
	return (0);
}

__MPS_EXTERN_INLINE caddr_t
mi_open_detached_cache(caddr_t *mi_head, kmem_cachep_t cachep, dev_t *devp)
{
	caddr_t ptr;
	int err;

	if (!(ptr = mi_open_alloc_cache(cachep, GFP_KERNEL)))
		return (NULL);
	if (!(err = mi_open_link(mi_head, ptr, devp, 0, DRVOPEN, NULL)))
		return (ptr);
	mi_close_free_cache(cachep, ptr);
	return (NULL);
}

__MPS_EXTERN_INLINE int
mi_open_comm_cache(caddr_t *mi_head, kmem_cachep_t cachep, queue_t *q, dev_t *devp,
		   int flag, int sflag, cred_t *credp)
{
	caddr_t ptr;
	int err;

	if (sflag == MODOPEN) {
		if (!WR(q)->q_next)
			return (EIO);
	} else {
		if (WR(q)->q_next && SAMESTR(q))
			return (EIO);
	}
	if (!(ptr = mi_open_alloc_cache(cachep, GFP_KERNEL)))
		return (EAGAIN);
	if (!(err = mi_open_link(mi_head, ptr, devp, flag, sflag, credp))) {
		mi_attach(q, ptr);
		return (0);
	}
	mi_close_free_cache(cachep, ptr);
	return (err);
}

__MPS_EXTERN_INLINE void
mi_close_detached_cache(caddr_t *mi_head, kmem_cachep_t cachep, caddr_t ptr)
{
	mi_close_unlink(mi_head, ptr);
	mi_close_free_cache(cachep, ptr);
}

__MPS_EXTERN_INLINE int
mi_close_comm_cache(caddr_t *mi_head, kmem_cachep_t cachep, queue_t *q)
{
	caddr_t ptr = q->q_ptr;

	mi_detach(q, ptr);
	mi_close_detached_cache(mi_head, cachep, ptr);
	return (0);
}

/*
 *  Timer helper functions.
 */

/* MacOT flavor */
__MPS_EXTERN mblk_t *mi_timer_alloc_MAC(queue_t *q, size_t size);
__MPS_EXTERN void mi_timer_MAC(mblk_t *mp, clock_t msec);
__MPS_EXTERN int mi_timer_cancel(mblk_t *mp);
__MPS_EXTERN mblk_t *mi_timer_q_switch(mblk_t *mp, queue_t *q, mblk_t *new_mp);

/* OpenSolaris flavor */
__MPS_EXTERN mblk_t *mi_timer_alloc_SUN(size_t size);
__MPS_EXTERN void mi_timer_SUN(queue_t *q, mblk_t *mp, clock_t msec);
__MPS_EXTERN void mi_timer_stop(mblk_t *mp);
__MPS_EXTERN void mi_timer_move(queue_t *q, mblk_t *mp);

/* common */
__MPS_EXTERN void mi_timer_ticks(mblk_t *mp, clock_t ticks);
__MPS_EXTERN int mi_timer_valid(mblk_t *mp);
__MPS_EXTERN int mi_timer_requeue(mblk_t *mp);
__MPS_EXTERN void mi_timer_free(mblk_t *mp);
__MPS_EXTERN unsigned long mi_timer_remain(mblk_t *mp);
__MPS_EXTERN int mi_timer_running(mblk_t *mp);
__MPS_EXTERN int mi_timer_cond(mblk_t *mp, clock_t msec);

/*
 *  Locking helper function.
 */
__MPS_EXTERN caddr_t mi_acquire_sleep(caddr_t ptrw, caddr_t *ptrp, rwlock_t *lockp,
				      unsigned long *flagsp);
__MPS_EXTERN caddr_t mi_acquire_sleep_nosignal(caddr_t ptrw, caddr_t *ptrp, rwlock_t *lockp,
					       unsigned long *flagsp);
__MPS_EXTERN caddr_t mi_acquire(caddr_t ptr, queue_t *q);
__MPS_EXTERN void mi_release(caddr_t ptr);
__MPS_EXTERN caddr_t mi_sleeplock(queue_t *q);
__MPS_EXTERN caddr_t mi_trylock(queue_t *q);
__MPS_EXTERN void mi_unlock(caddr_t ptr);

/*
 *  Buffer call helper function.
 */
__MPS_EXTERN void mi_bufcall(queue_t *q, int size, int priority);

__MPS_EXTERN_INLINE void mi_esbbcall(queue_t *q, int priority)
{
	mi_bufcall(q, 0, priority);
}

/*
 *  Message block allocation helper functions.
 */
__MPS_EXTERN mblk_t *mi_reuse_proto(mblk_t *mp, size_t size, int keep_on_error);
__MPS_EXTERN mblk_t *mi_reallocb(mblk_t *mp, size_t size);

__MPS_EXTERN_INLINE mblk_t *
mi_allocb(queue_t *q, size_t size, int priority)
{
	mblk_t *mp;

	if (unlikely((mp = allocb(size, priority)) == NULL))
		mi_bufcall(q, size, priority);
	return (mp);
}

__MPS_EXTERN_INLINE mblk_t *
mi_esballoc(queue_t *q, unsigned char *base, size_t size, uint priority, frtn_t *freeinfo)
{
	mblk_t *mp;

	if (unlikely((mp = esballoc(base, size, priority, freeinfo)) == NULL))
		mi_esbbcall(q, priority);
	return (mp);
}

__MPS_EXTERN_INLINE mblk_t *
mi_copyb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (unlikely((mp = copyb(bp)) == NULL))
		if (likely(bp != NULL))
			mi_bufcall(q, bp->b_wptr - bp->b_rptr, BPRI_MED);
	return (mp);
}

__MPS_EXTERN_INLINE mblk_t *
mi_dupb(queue_t *q, mblk_t *bp)
{
	mblk_t *mp;

	if (unlikely((mp = dupb(bp)) == NULL))
		if (likely(bp != NULL && bp->b_datap->db_ref < 255))
			mi_bufcall(q, 0, BPRI_MED);
	return (mp);
}

__MPS_EXTERN_INLINE mblk_t *
mi_copymsg(queue_t *q, mblk_t *mp)
{
	mblk_t *msg;

	if (unlikely((msg = copymsg(mp)) == NULL))
		if (likely(mp != NULL))
			mi_bufcall(q, msgsize(mp), BPRI_MED);
	return (msg);
}

__MPS_EXTERN_INLINE mblk_t *
mi_dupmsg(queue_t *q, mblk_t *mp)
{
	mblk_t *msg;

	if (unlikely((msg = dupmsg(mp)) == NULL))
		if (likely(mp != NULL) )
			mi_bufcall(q, 0, BPRI_MED);
	return (msg);
}

__MPS_EXTERN_INLINE mblk_t *
mi_msgpullup(queue_t *q, mblk_t *mp, ssize_t length)
{
	mblk_t *msg;

	if (unlikely((msg = msgpullup(mp, length)) == NULL))
		if (likely(mp != NULL && length != -1 && msgsize(mp) >= length))
			mi_bufcall(q, msgsize(mp), BPRI_MED);
	return (msg);
}

/*
 *  M_IOTCL handling helper functions.
 */
#define MI_COPY_IN			1
#define MI_COPY_OUT			2
#define MI_COPY_CASE(_dir, _cnt)	(((_cnt)<<2|_dir))

__MPS_EXTERN void mi_copyin(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len);
__MPS_EXTERN void mi_copyin_n(queue_t *q, mblk_t *mp, size_t offset, size_t len);
__MPS_EXTERN void mi_copyout(queue_t *q, mblk_t *mp);
__MPS_EXTERN mblk_t *mi_copyout_alloc(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len,
				      int free_on_error);
__MPS_EXTERN void mi_copy_done(queue_t *q, mblk_t *mp, int err);
__MPS_EXTERN int mi_copy_state(queue_t *q, mblk_t *mp, mblk_t **mpp);
__MPS_EXTERN void mi_copy_set_rval(mblk_t *mp, int rval);

/*
 *  Stream head helper functions.
 */
__MPS_EXTERN int mi_set_sth_hiwat(queue_t *q, size_t size);
__MPS_EXTERN int mi_set_sth_lowat(queue_t *q, size_t size);
__MPS_EXTERN int mi_set_sth_maxblk(queue_t *q, ssize_t size);
__MPS_EXTERN int mi_set_sth_copyopt(queue_t *q, int copyopt);
__MPS_EXTERN int mi_set_sth_wroff(queue_t *q, size_t size);

/*
 *  STREAMS wrapper functions.
 */
__MPS_EXTERN queue_t *mi_allocq(struct streamtab *st);
__MPS_EXTERN void mi_freeq(queue_t *q);
__MPS_EXTERN int mi_strlog(queue_t *q, char level, ushort flags, char *fmt, ...)
    __attribute__ ((format(printf, 4, 5)));
__MPS_EXTERN int mi_mpprintf(mblk_t *mp, char *fmt, ...) __attribute__ ((format(printf, 2, 3)));
__MPS_EXTERN int mi_mpprintf_nr(mblk_t *mp, char *fmt, ...) __attribute__ ((format(printf, 2, 3)));

/*
 *  Message block functions
 */
__MPS_EXTERN uint8_t *mi_offset_param(mblk_t *mp, size_t offset, size_t len);
__MPS_EXTERN uint8_t *mi_offset_paramc(mblk_t *mp, size_t offset, size_t len);

/*
 *  Some internals showing.
 */
typedef void streamscall (*proc_ptr_t) (queue_t *, mblk_t *);
__MPS_EXTERN void mps_become_writer(queue_t *q, mblk_t *mp, proc_ptr_t proc);

#else
#ifdef _MPS_SOURCE
#warning "_MPS_SOURCE defined by not CONFIG_STREAMS_COMPAT_MPS"
#endif
#endif

#endif				/* __SYS_MPS_STREAM_H__ */
