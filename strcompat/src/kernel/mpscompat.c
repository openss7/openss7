/*****************************************************************************

 @(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.36 $) $Date: 2007/02/14 14:09:16 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/02/14 14:09:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mpscompat.c,v $
 Revision 0.9.2.36  2007/02/14 14:09:16  brian
 - broad changes updating support for SS7 MTP and M3UA

 Revision 0.9.2.35  2007/01/21 20:22:43  brian
 - working up drivers

 Revision 0.9.2.34  2007/01/15 12:16:38  brian
 - updated archive sizes, new development work

 Revision 0.9.2.33  2006/12/29 05:51:21  brian
 - changes for successful master build

 Revision 0.9.2.32  2006/12/19 11:47:58  brian
 - better mi_bufcall implementation

 Revision 0.9.2.31  2006/12/19 00:51:55  brian
 - corrections to mi_open/close functions

 Revision 0.9.2.30  2006/12/16 16:12:56  brian
 - strapped out unused manpages and allow mi_timers to be placed back on queue

 Revision 0.9.2.29  2006/12/08 05:08:24  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.28  2006/11/03 10:39:28  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.27  2006/10/06 12:13:18  brian
 - updated manual pages to pass make check and for release
 - updated release files for release
 - added missing MacOT OSF/1 and MPS compatibility functions

 Revision 0.9.2.26  2006/07/24 09:01:06  brian
 - results of udp2 optimizations

 Revision 0.9.2.25  2006/07/07 20:49:55  brian
 - change to correct LIS-only compile on FC5

 Revision 0.9.2.24  2006/06/22 13:11:33  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.23  2006/03/03 11:11:14  brian
 - 64-bit compatibility, fixes, updates for release

 Revision 0.9.2.22  2005/12/29 21:36:14  brian
 - a few idiosynchrasies for PPC, old 2.95 compiler, and 2.6.14 builds

 Revision 0.9.2.21  2005/12/28 09:51:50  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.20  2005/12/22 10:28:54  brian
 - no symbol mangling for 2.4 kernels

 Revision 0.9.2.19  2005/09/18 07:36:19  brian
 - M_IOCDATA bug fix

 Revision 0.9.2.18  2005/07/29 22:20:02  brian
 - corrections for debug compile
 - some size optimizations

 Revision 0.9.2.17  2005/07/29 07:37:51  brian
 - changes to compile with latest streams package.

 Revision 0.9.2.16  2005/07/22 06:06:51  brian
 - working up streams/src/kernel/strsched.h

 Revision 0.9.2.15  2005/07/18 15:52:13  brian
 - implemented mps mpprintf functions

 Revision 0.9.2.14  2005/07/18 12:25:42  brian
 - standard indentation

 Revision 0.9.2.13  2005/07/14 22:04:09  brian
 - updates for check pass and header splitting

 Revision 0.9.2.12  2005/07/14 03:40:12  brian
 - updates for check pass

 Revision 0.9.2.11  2005/07/13 12:01:49  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 0.9.2.10  2005/07/13 01:40:39  brian
 - changes for check pass

 Revision 0.9.2.9  2005/07/12 13:54:46  brian
 - changes for os7 compatibility and check pass

 Revision 0.9.2.8  2005/07/12 08:42:42  brian
 - changes for check pass

 Revision 0.9.2.7  2005/07/09 21:51:21  brian
 - remove dependency on LFS headers

 Revision 0.9.2.6  2005/07/07 20:29:17  brian
 - changes for release

 Revision 0.9.2.5  2005/07/05 22:46:05  brian
 - change for strcompat package

 Revision 0.9.2.4  2005/07/04 20:14:30  brian
 - fixed spelling of CVS keyword

 Revision 0.9.2.3  2005/07/04 19:29:16  brian
 - first cut at streams compatibility package

 Revision 0.9.2.2  2005/07/03 17:41:27  brian
 - separated out MPS compatibility module

 Revision 0.9.2.1  2005/07/01 20:16:30  brian
 - added and updated manpages for some Mentat compatibility

 *****************************************************************************/

#ident "@(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.36 $) $Date: 2007/02/14 14:09:16 $"

static char const ident[] =
    "$RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.36 $) $Date: 2007/02/14 14:09:16 $";

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

#define __MPS_EXTERN_INLINE __inline__ streamscall __unlikely

#ifdef LIS
#define _LFS_SOURCE
#endif
#define _MPS_SOURCE

#include "sys/os7/compat.h"

#include <linux/types.h>	/* for ptrdiff_t */
#include <linux/ctype.h>	/* for isdigit */
#include <asm/div64.h>		/* for do_div */

#define MPSCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MPSCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define MPSCOMP_REVISION	"LfS $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.36 $) $Date: 2007/02/14 14:09:16 $"
#define MPSCOMP_DEVICE		"Mentat Portable STREAMS Compatibility"
#define MPSCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MPSCOMP_LICENSE		"GPL"
#define MPSCOMP_BANNER		MPSCOMP_DESCRIP		"\n" \
				MPSCOMP_COPYRIGHT	"\n" \
				MPSCOMP_REVISION	"\n" \
				MPSCOMP_DEVICE		"\n" \
				MPSCOMP_CONTACT		"\n"
#define MPSCOMP_SPLASH		MPSCOMP_DEVICE		" - " \
				MPSCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_MPS_MODULE
MODULE_AUTHOR(MPSCOMP_CONTACT);
MODULE_DESCRIPTION(MPSCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(MPSCOMP_DEVICE);
MODULE_LICENSE(MPSCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mpscompat");
#endif
#endif

/*
 *  MI_BCMP
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE int mi_bcmp(const void *s1, const void *s2, size_t len);

EXPORT_SYMBOL_NOVERS(mi_bcmp);

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void *mi_alloc(size_t size, unsigned int pri);

EXPORT_SYMBOL_NOVERS(mi_alloc);	/* mps/ddi.h */

/*
 *  MI_ALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void *mi_alloc_sleep(size_t size, unsigned int pri);

EXPORT_SYMBOL_NOVERS(mi_alloc_sleep);	/* mps/ddi.h */

/*
 *  MI_ZALLOC
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE caddr_t mi_zalloc(size_t size);

EXPORT_SYMBOL_NOVERS(mi_zalloc);	/* mps/ddi.h */

/*
 *  MI_ZALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE caddr_t mi_zalloc_sleep(size_t size);

EXPORT_SYMBOL_NOVERS(mi_zalloc_sleep);	/* mps/ddi.h */

/*
 *  MI_FREE
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_free(void *ptr);

EXPORT_SYMBOL_NOVERS(mi_free);	/* mps/ddi.h */

/*
 *  =========================================================================
 *
 *  Module or driver open and close helper functions.
 *
 *  =========================================================================
 */
struct mi_comm {
	struct mi_comm **mi_prev;	/* must be first */
	struct mi_comm **mi_head;
	struct mi_comm *mi_next;
	size_t mi_size;			/* size of this structure plus private data */
	unsigned short mi_mid;		/* module identifier */
	unsigned short mi_sid;		/* stream identifier */
	bcid_t mi_rbid;			/* rd queue bufcall */
	bcid_t mi_wbid;			/* wr queue bufcall */
	spinlock_t mi_lock;		/* structure lock */
	uint mi_users;			/* number of users */
	queue_t *mi_qwait;		/* queue waiting on lock */
	wait_queue_head_t mi_waitq;	/* processes waiting on lock */
	union {
		dev_t dev;		/* device number (or NODEV for modules) */
		int index;		/* link index */
	} mi_u;
	char mi_ptr[0];			/* followed by private data */
};

#define mi_dev mi_u.dev
#define mi_index mi_u.index

#define mi_to_ptr(mi) ((mi) ? (mi)->mi_ptr : NULL)
#define ptr_to_mi(ptr) ((ptr) ? (struct mi_comm *)(ptr) - 1 : NULL)

static void
mi_open_init(struct mi_comm *mi, size_t size)
{
	bzero(mi, sizeof(*mi));
	mi->mi_prev = mi->mi_head = &mi->mi_next;
	mi->mi_size = size;
	spin_lock_init(&mi->mi_lock);
	init_waitqueue_head(&mi->mi_waitq);
	return;
}

static caddr_t
mi_open_alloc_flag(size_t size, int flag)
{
	struct mi_comm *mi;

	if ((mi = kmem_zalloc(sizeof(struct mi_comm) + size, flag))) {
		mi_open_init(mi, size);
		return (mi_to_ptr(mi));
	}
	return (NULL);
}

/*
 *  MI_OPEN_ALLOC
 *  -------------------------------------------------------------------------
 */
caddr_t
mi_open_alloc(size_t size)
{
	return mi_open_alloc_flag(size, KM_NOSLEEP);
}

EXPORT_SYMBOL_NOVERS(mi_open_alloc);	/* mps/ddi.h */

/*
 *  MI_OPEN_ALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
caddr_t
mi_open_alloc_sleep(size_t size)
{
	return mi_open_alloc_flag(size, KM_SLEEP);
}

EXPORT_SYMBOL_NOVERS(mi_open_alloc_sleep);	/* mps/ddi.h */

/*
 * MI_FIRST_PTR
 * -------------------------------------------------------------------------
 * OpenSolaris uses a head structure, we don't, primarily because the
 * documentation for mi_open_comm for both AIX and MacOT says that we use a
 * "static pointer" initialized to NULL.
 */
caddr_t
mi_first_ptr(caddr_t *mi_head)
{
	struct mi_comm *mi = *(struct mi_comm **) mi_head;

	return (mi_to_ptr(mi));
}

EXPORT_SYMBOL_NOVERS(mi_first_ptr);	/* mps/ddi.h */

/*
 *  MI_FIRST_DEV_PTR
 *  -------------------------------------------------------------------------
 *  OpenSolaris tracks modules and drivers on the same list.  We don't,
 *  primarily because AIX and MacOT don't document it that way.  If you have a
 *  STREAMS driver that can also be pushed as a module, use two separate lists.
 */
caddr_t
mi_first_dev_ptr(caddr_t *mi_head)
{
	struct mi_comm *mi = *(struct mi_comm **) mi_head;

	for (; mi && getmajor(mi->mi_dev) != 0; mi = mi->mi_next) ;
	return (mi_to_ptr(mi));
}

EXPORT_SYMBOL_NOVERS(mi_first_dev_ptr);	/* mps/ddi.h */

/*
 *  MI_NEXT_PTR
 *  -------------------------------------------------------------------------
 */
caddr_t
mi_next_ptr(caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);

	if (mi)
		mi = mi->mi_next;
	return (mi_to_ptr(mi));
}

EXPORT_SYMBOL_NOVERS(mi_next_ptr);	/* mps/ddi.h, aix/ddi.h */

/*
 *  MI_NEXT_DEV_PTR
 *  -------------------------------------------------------------------------
 *  OpenSolaris tracks modules and drivers on the same list.  We don't,
 *  primarily because AIX and MacOT don't document it that way.  If you have a
 *  STREAMS driver that can also be pushed as a module, use two separate lists.
 */
caddr_t
mi_next_dev_ptr(caddr_t *mi_head, caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);

	for (mi = mi ? mi->mi_next : NULL; mi && getmajor(mi->mi_dev) != 0; mi = mi->mi_next) ;
	return (mi_to_ptr(mi));
}

EXPORT_SYMBOL_NOVERS(mi_next_dev_ptr);	/* mps/ddi.h */

/* 
 *  MI_PREV_PTR
 *  -------------------------------------------------------------------------
 *  Linux Fast-STREAMS embellishment.
 */
caddr_t
mi_prev_ptr(caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);

	if (mi && mi->mi_prev != &mi->mi_next && mi->mi_prev != mi->mi_head)
		return ((caddr_t) (((struct mi_comm *) mi->mi_prev) + 1));
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(mi_prev_ptr);	/* mps/ddi.h, aix/ddi.h */

static spinlock_t mi_list_lock = SPIN_LOCK_UNLOCKED;

/*
 *  MI_OPEN_LINK
 *  -------------------------------------------------------------------------
 */
int
mi_open_link(caddr_t *mi_head, caddr_t ptr, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	struct mi_comm *mi = ptr_to_mi(ptr);
	struct mi_comm **mip = (struct mi_comm **) mi_head;

	major_t cmajor = devp ? getmajor(*devp) : 0;
	minor_t cminor = devp ? getminor(*devp) : 0;
	major_t dmajor;

	spin_lock(&mi_list_lock);
	if (sflag == CLONEOPEN) {
		/* first clone minor (above 5 per AIX docs, above 10 per MacOT docs), but the
		   caller can start wherever they want above that */
#define MI_OPEN_COMM_CLONE_MINOR 10
		if (cminor <= MI_OPEN_COMM_CLONE_MINOR)
			cminor = MI_OPEN_COMM_CLONE_MINOR + 1;
	}
	if (sflag == MODOPEN) {
		cmajor = 0;
		/* caller can specify a preferred instance number */
		if (cminor == 0)
			cminor = 1;
	}
	dmajor = cmajor;
	for (; *mip && (dmajor = getmajor((*mip)->mi_dev)) < cmajor; mip = &(*mip)->mi_next) ;
	for (; *mip && dmajor == getmajor((*mip)->mi_dev)
	     && getminor(makedevice(0, cminor)) != 0; mip = &(*mip)->mi_next, cminor++) {
		minor_t dminor = getminor((*mip)->mi_dev);

		if (cminor < dminor)
			break;
		if (cminor == dminor)
			if (sflag == DRVOPEN) {
				spin_unlock(&mi_list_lock);
				return (ENXIO);
			}
	}
	if (getminor(makedevice(0, cminor)) == 0) {	/* no minors left */
		spin_unlock(&mi_list_lock);
		return (EAGAIN);
	}
	mi->mi_dev = makedevice(cmajor, cminor);
	mi->mi_mid = cmajor;
	mi->mi_sid = cminor;
	if ((mi->mi_next = *mip))
		mi->mi_next->mi_prev = &mi;
	mi->mi_prev = mip;
	*mip = mi;
	mi->mi_head = (struct mi_comm **) mi_head;
	spin_unlock(&mi_list_lock);
	return (0);
}

EXPORT_SYMBOL_NOVERS(mi_open_link);	/* mps/ddi.h */

/*
 *  MI_OPEN_DETACHED
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE caddr_t mi_open_detached(caddr_t *mi_head, size_t size, dev_t *devp);

EXPORT_SYMBOL_NOVERS(mi_open_detached);	/* mps/ddi.h */

/*
 *  MI_ATTACH
 *  -------------------------------------------------------------------------
 */
void
mi_attach(queue_t *q, caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);

	mi->mi_mid = q->q_qinfo->qi_minfo->mi_idnum;
	q->q_ptr = WR(q)->q_ptr = ptr;
}

EXPORT_SYMBOL_NOVERS(mi_attach);	/* mps/ddi.h, mac/ddi.h */

/* 
 *  MI_OPEN_COMM
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE int mi_open_comm(caddr_t *mi_head, size_t size, queue_t *q, dev_t *devp,
				     int flag, int sflag, cred_t *credp);
EXPORT_SYMBOL_NOVERS(mi_open_comm);	/* mps/ddi.h, aix/ddi.h */

/*
 *  MI_CLOSE_UNLINK
 *  -------------------------------------------------------------------------
 */
void
mi_close_unlink(caddr_t *mi_head, caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);

	if (mi) {
		spin_lock(&mi_list_lock);
		if ((*mi->mi_prev = mi->mi_next))
			mi->mi_next->mi_prev = mi->mi_prev;
		mi->mi_next = NULL;
		mi->mi_prev = &mi->mi_next;
		mi->mi_head = NULL;
		spin_unlock(&mi_list_lock);
	}
}

EXPORT_SYMBOL_NOVERS(mi_close_unlink);	/* mps/ddi.h */

/*
 *  MI_CLOSE_FREE
 *  -------------------------------------------------------------------------
 */
void
mi_close_free(caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);

	if (mi && mi->mi_head == NULL && mi->mi_next == NULL)
		kmem_free(mi, mi->mi_size);
}

EXPORT_SYMBOL_NOVERS(mi_close_free);	/* mps/ddi.h */

/*
 *  MI_DETACH
 *  -------------------------------------------------------------------------
 */
void
mi_detach(queue_t *q, caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);
	bcid_t bid;

	if (mi) {
		if ((bid = xchg(&mi->mi_rbid, 0)))
			unbufcall(bid);
		if ((bid = xchg(&mi->mi_wbid, 0)))
			unbufcall(bid);
	}
	q->q_ptr = WR(q)->q_ptr = NULL;
}

EXPORT_SYMBOL_NOVERS(mi_detach);	/* mps/ddi.h */

/*
 *  MI_CLOSE_DETACHED
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_close_detached(caddr_t *mi_head, caddr_t ptr);

EXPORT_SYMBOL_NOVERS(mi_close_detached);	/* mps/ddi.h */

/* 
 *  MI_CLOSE_COMM
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE int mi_close_comm(caddr_t *mi_head, queue_t *q);

EXPORT_SYMBOL_NOVERS(mi_close_comm);	/* mps/ddi.h, aix/ddi.h */

struct mi_iocblk {
	caddr_t mi_caddr;		/* uaddr of TRANSPARENT ioctl, NULL for I_STR */
	caddr_t mi_uaddr;		/* uaddr of last copyin operation, NULL for implicit */
	short mi_dir;			/* direction of operation MI_COPY_IN or MI_COPY_OUT */
	short mi_cnt;			/* operation count, starting from 1 for each direction */
	int mi_rval;			/* return value */
};

/*
 *  =========================================================================
 *
 *  Locking helper function.
 *
 *  =========================================================================
 */
caddr_t
mi_trylock(queue_t *q)
{
	caddr_t ptr = q->q_ptr;
	struct mi_comm *mi = ptr_to_mi(ptr);
	unsigned long flags;

	if (mi) {
		spin_lock_irqsave(&mi->mi_lock, flags);
		if (mi->mi_users != 0)
			ptr = NULL;
		else
			mi->mi_users = 1;
		spin_unlock_irqrestore(&mi->mi_lock, flags);
	}
	return (ptr);
}

EXPORT_SYMBOL_NOVERS(mi_trylock);

caddr_t
mi_sleeplock(queue_t *q)
{
	caddr_t ptr = q->q_ptr;
	struct mi_comm *mi = ptr_to_mi(ptr);
	unsigned long flags;

	DECLARE_WAITQUEUE(wait, current);

	if (mi) {
		add_wait_queue(&mi->mi_waitq, &wait);
		spin_lock_irqsave(&mi->mi_lock, flags);
		if (mi->mi_users != 0) {
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				if (signal_pending(current)) {
					ptr = NULL;
					break;
				}
				if (mi->mi_users == 0) {
					mi->mi_users = 1;
					break;
				}
				spin_unlock_irqrestore(&mi->mi_lock, flags);
				schedule();
				spin_lock_irqsave(&mi->mi_lock, flags);
			}
		} else
			mi->mi_users = 1;
		spin_unlock_irqrestore(&mi->mi_lock, flags);
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&mi->mi_waitq, &wait);
	}
	return (ptr);

}

EXPORT_SYMBOL_NOVERS(mi_sleeplock);

void
mi_unlock(caddr_t ptr)
{
	struct mi_comm *mi = ptr_to_mi(ptr);
	unsigned long flags;
	queue_t *newq;

	if (mi) {
		spin_lock_irqsave(&mi->mi_lock, flags);
		mi->mi_users = 0;
		if ((newq = mi->mi_qwait))
			mi->mi_qwait = NULL;
		spin_unlock_irqrestore(&mi->mi_lock, flags);
		if (waitqueue_active(&mi->mi_waitq))
			wake_up_all(&mi->mi_waitq);
		if (newq)
			qenable(newq);
	}
	return;
}

EXPORT_SYMBOL_NOVERS(mi_unlock);

/*
 *  =========================================================================
 *
 *  Buffer call helper function.
 *
 *  =========================================================================
 */
static streamscall void
mi_qenable(long data)
{
	queue_t *q = (queue_t *) data;
	struct mi_comm *mi = ptr_to_mi(q->q_ptr);
	bcid_t *bidp = (q->q_flag & QREADR) ? &mi->mi_rbid : &mi->mi_wbid;

	*bidp = 0;
	qenable(q);
}

void
mi_bufcall(queue_t *q, int size, int priority)
{
	struct mi_comm *mi = ptr_to_mi(q->q_ptr);
	bcid_t bid, *bidp = (q->q_flag & QREADR) ? &mi->mi_rbid : &mi->mi_wbid;

	if ((bid = bufcall(size, priority, &mi_qenable, (long) q))) {
		if ((bid = xchg(bidp, bid)))
			unbufcall(bid);
		return;
	}
	qenable(q);
}

EXPORT_SYMBOL_NOVERS(mi_bufcall);

/*
 *  =========================================================================
 *
 *  Message block allocation helper functions.
 *
 *  =========================================================================
 */

mblk_t *
mi_reuse_proto(mblk_t *mp, size_t size, int keep_on_error)
{
	if (unlikely(mp == NULL || (size > FASTBUF && mp->b_datap->db_lim - mp->b_rptr < size)
		     || mp->b_datap->db_ref > 1 || mp->b_datap->db_frtnp != NULL)) {
		/* can't reuse this message block (or no message block to begin with) */
		if (mp && !keep_on_error)
			freemsg(xchg(&mp, NULL));
		mp = NULL;
	} else
		/* simply resize it - leave everything else intact */
		mp->b_wptr = mp->b_rptr + size;
	return (mp);
}

EXPORT_SYMBOL_NOVERS(mi_reuse_proto);

mblk_t *
mi_reallocb(mblk_t *mp, size_t size)
{
	if (unlikely(mp == NULL ||
		     (size > FASTBUF && mp->b_datap->db_lim - mp->b_datap->db_base < size) ||
		     mp->b_datap->db_ref > 1 || mp->b_datap->db_frtnp != NULL)) {
		/* can't reuse this message block (or no message block to begin with) */
		if (mp)
			freemsg(xchg(&mp, NULL));
		mp = allocb(size, BPRI_MED);
	} else {
		/* prepare existing message block for reuse as though just allocated */
		mp->b_next = mp->b_prev = NULL;
		if (mp->b_cont)
			freemsg(xchg(&mp->b_cont, NULL));
		mp->b_rptr = mp->b_wptr = mp->b_datap->db_base;
		mp->b_band = 0;
		mp->b_flag = 0;
		mp->b_datap->db_type = M_DATA;
	}
	return (mp);
}

EXPORT_SYMBOL_NOVERS(mi_reallocb);

__MPS_EXTERN_INLINE mblk_t *mi_allocb(queue_t *q, size_t size, int priority);

EXPORT_SYMBOL_NOVERS(mi_allocb);	/* mps/stream.h */

/*
 *  =========================================================================
 *
 *  Copyin and copyout M_IOCTL helper functions.
 *
 *  =========================================================================
 */

#define MI_COPY_IN			1
#define MI_COPY_OUT			2
#define MI_COPY_CASE(_dir, _cnt)	(((_cnt)<<2|_dir))

#if 0
#define MI_COPY_DIRECTION(_mp)		(*(int *)&(_mp)->b_cont->b_next)
#define MI_COPY_COUNT(_mp)		(*(int *)&(_mp)->b_cont->b_prev)
#else
#define MI_COPY_DIRECTION(_mp)		(((struct mi_iocblk *)(_mp)->b_cont->b_rptr)->mi_dir)
#define MI_COPY_COUNT(_mp)		(((struct mi_iocblk *)(_mp)->b_cont->b_rptr)->mi_cnt)
#define MI_COPY_UADDR(_mp)		(((struct mi_iocblk *)(_mp)->b_cont->b_rptr)->mi_uaddr)
#endif

#define MI_COPY_STATE(_mp)		MI_COPY_CASE(MI_COPY_DIRECTION(_mp), MI_COPY_COUNT(_mp))

/**
 * mi_copy_done: - complete an input-output control operation
 * @q: write queue upon which message was received
 * @mp: the M_IOCTL/M_IOCDATA message
 * @err: non-zero error to return, or zero for success
 *
 * mi_copy_done() is used to complete an input-output control operation where there is no data to be
 * copied out (neither implicit nor explicit).
 *
 * Almost all versions of LiS have a pretty nasty bug with regard to M_IOCDATA.  According to SVR 4
 * STREAMS Programmer's Guide and all major UNIX implementations of STREAMS, when M_IOCDATA returns
 * an error (cp_rval != NULL), the module or driver is supposed to clean up and abort the M_IOCTL.
 * The Stream head has already returned an error to the user and does not expect an M_IOCACK or
 * M_IOCNAK.  LiS makes the error of sleeping again and expecting a negative acknowledgement, so,
 * for LiS (and not Linux Fast-STREAMS) we return an M_IOCNAK with what will be the ultimate error
 * code.  Modules and drivers ported to LiS that don't know about this will hang until timeout or
 * signal (SIGKILL, SIGTERM), possibly indefinitely when an M_IOCDATA error is sent.  Even if LiS is
 * fixed, this code will still work, it will just send an extraneous M_IOCNAK to the Stream head
 * that will be discarded because no M_IOCTL will be in progress anymore.
 *
 */
void
mi_copy_done(queue_t *q, mblk_t *mp, int err)
{
	union ioctypes *ioc;
	struct mi_iocblk *mi;
	int rval;

	assert(mp);
	assert(q);
	assert(mp->b_wptr >= mp->b_rptr + sizeof(*ioc));

	ioc = (typeof(ioc)) mp->b_rptr;
	rval = ioc->iocblk.ioc_rval;
	switch (mp->b_datap->db_type) {
	case M_IOCDATA:
		if (ioc->copyresp.cp_private) {
			mi = (typeof(mi)) ioc->copyresp.cp_private->b_rptr;
			rval = mi->mi_rval;
			freemsg(XCHG(&ioc->copyresp.cp_private, NULL));
		}
#ifdef LIS
		/* LiS bug, see above. */
		break;
#else
		if (ioc->copyresp.cp_rval == (caddr_t) 0)
			break;
		/* SVR 4 SPG says don't return ACK/NAK on M_IOCDATA error */
		freemsg(mp);
		return;
#endif
	case M_IOCTL:
		break;
	default:
		swerr();
		freemsg(mp);
		return;
	}
	mp->b_datap->db_type = err ? M_IOCNAK : M_IOCACK;
	ioc->iocblk.ioc_error = (err < 0) ? -err : err;
	ioc->iocblk.ioc_rval = err ? -1 : rval;
	ioc->iocblk.ioc_count = 0;
	qreply(q, mp);
}

EXPORT_SYMBOL_NOVERS(mi_copy_done);	/* mps/ddi.h */

/**
 * mi_copyin: - perform explicit or implicit copyin() operation
 * @q: write queue on which message was received
 * @mp: the M_IOCTL or M_IOCDATA message
 * @uaddr: NULL for first copyin operation, or user address for subsequent operation
 * @len: length of data to copy in
 */
void
mi_copyin(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mi;
	mblk_t *bp, *dp;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		/* Note that uaddr is ignored on the first operation, but should be specified as
		   NULL according to the documentation anyway. */
		if (ioc->iocblk.ioc_count == TRANSPARENT) {
			/* for transparent ioctl perform a copy in */
			if (!(bp = XCHG(&mp->b_cont, NULL)))
				return mi_copy_done(q, mp, EPROTO);
			mi = (typeof(mi)) bp->b_rptr;
			mp->b_datap->db_type = M_COPYIN;
			ioc->copyreq.cq_private = bp;
			ioc->copyreq.cq_size = len;
			ioc->copyreq.cq_flag = 0;
			ioc->copyreq.cq_addr = mi->mi_caddr;
			mi->mi_uaddr = mi->mi_caddr;
			mi->mi_cnt = 1;
			mi->mi_dir = MI_COPY_IN;
			if (mp->b_cont)
				freemsg(XCHG(&mp->b_cont, NULL));
			qreply(q, mp);
			return;
		}
		/* for a non-transparent ioctl the first copyin is already performed for us, fake
		   out an M_IOCDATA response.  However, only place the requested data from the
		   implicit copyin into the data block.  Save the rest for a subsequent
		   mi_copyin_n() operation. */

		/* If the amount copied in implicitly is less than that requested in the
		   input-output control operation, then it is the user's fault and EINVAL is
		   returned. */
		if (msgdsize(mp->b_cont) < len)
			return mi_copy_done(q, mp, EINVAL);

		if (!pullupmsg(mp->b_cont, -1) || !(dp = copyb(mp->b_cont)))
			return mi_copy_done(q, mp, ENOBUFS);
		if (!(bp = allocb(sizeof(*mi), BPRI_MED))) {
			freemsg(dp);
			return mi_copy_done(q, mp, ENOBUFS);
		}
		dp->b_wptr = dp->b_rptr + len;
		bp->b_cont = mp->b_cont;	/* save original */
		mp->b_cont = dp;
		mi = (typeof(mi)) bp->b_rptr;
		mp->b_datap->db_type = M_IOCDATA;
		ioc->copyresp.cp_private = bp;
		ioc->copyresp.cp_rval = (caddr_t) 0;
		mi->mi_caddr = NULL;
		mi->mi_uaddr = NULL;
		mi->mi_cnt = 1;
		mi->mi_dir = MI_COPY_IN;
		mi->mi_rval = 0;
		putq(q, mp);
		/* There are two clues to the fact that an implicit copyin was performed to
		   subsequent operations: mi->mi_uaddr == NULL and cp_private->b_cont != NULL while 
		   mi->mi_dir == MI_COPY_IN.  When the operation switches over to copyout, this
		   copied in buffer will be discarded. */
		return;
	case M_IOCDATA:
		/* for a subsequent M_IOCDATA, we should already have our private message block
		   stacked up against the private pointer */
		if (!(bp = XCHG(&ioc->copyresp.cp_private, NULL)))
			return mi_copy_done(q, mp, EPROTO);
		mi = (typeof(mi)) bp->b_rptr;
		mp->b_datap->db_type = M_COPYIN;
		ioc->copyreq.cq_private = bp;
		ioc->copyreq.cq_size = len;
		ioc->copyreq.cq_flag = 0;
		ioc->copyreq.cq_addr = uaddr;
		/* leave mi_caddr untouched for implicit copyout operation */
		mi->mi_uaddr = uaddr;
		mi->mi_cnt += 1;
		mi->mi_dir = MI_COPY_IN;
		if (mp->b_cont)
			freemsg(XCHG(&mp->b_cont, NULL));
		if (bp->b_cont)
			freemsg(XCHG(&bp->b_cont, NULL));
		qreply(q, mp);
		return;
	default:
		return mi_copy_done(q, mp, EPROTO);
	}
}

EXPORT_SYMBOL_NOVERS(mi_copyin);	/* mps/ddi.h */

/**
 * mi_copyin_n: - perform subsequent copyin operation
 * @q: write queue on which message was received
 * @mp: the M_IOCDATA message
 * @offset: offset into original memory extent
 * @len: length of data to copy in
 *
 * Note that this function can only be used on M_IOCDATA blocks, and that M_IOCDATA block must
 * contain our private data and the direction of the previous operation must have been an
 * MI_COPY_IN.  That is, mi_copyin() must be called before this function.
 *
 * When the mi_copyin() was the first mi_copyin() operation on an I_STR input-output control, the
 * implicit copyin buffer was attached as cp_private->b_cont.  The subsequent mi_copyin_n()
 * operation will take from that buffer.  An explicit mi_copyin() can subsequently be performed,
 * even for an I_STR input-output control, in which case, a subsequent mi_copyin_n() operation will
 * always issue an M_COPYIN resulting in an M_IOCDATA.
 */
void
mi_copyin_n(queue_t *q, mblk_t *mp, size_t offset, size_t len)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mi;
	mblk_t *bp, *dp;

	if (mp->b_datap->db_type != M_IOCDATA || !(bp = ioc->copyresp.cp_private))
		return mi_copy_done(q, mp, EPROTO);
	mi = (typeof(mi)) bp->b_rptr;
	if (mi->mi_dir != MI_COPY_IN)
		return mi_copy_done(q, mp, EPROTO);
	if (mi->mi_uaddr != NULL) {
		/* last operation was an explicit copyin */
		mp->b_datap->db_type = M_COPYIN;
		ioc->copyreq.cq_private = bp;
		ioc->copyreq.cq_size = len;
		ioc->copyreq.cq_flag = 0;
		ioc->copyreq.cq_addr = mi->mi_uaddr + offset;
		mi->mi_cnt++;
		if (mp->b_cont)
			freemsg(XCHG(&mp->b_cont, NULL));
		qreply(q, mp);
	} else {
		/* last operation was an implicit copyin */
		if ((dp = bp->b_cont) == NULL)
			return mi_copy_done(q, mp, EPROTO);
		/* If the implicit copyin buffer was to small to satisfy the request, then it was
		   the user's fault in supplying a buffer that was too small, so return EINVAL. */
		if (msgdsize(dp) < offset + len)
			return mi_copy_done(q, mp, EINVAL);
		if (!(dp = copyb(dp)))
			return mi_copy_done(q, mp, ENOSR);
		dp->b_rptr += offset;
		dp->b_wptr = dp->b_rptr + len;
		ioc->copyresp.cp_rval = (caddr_t) 0;
		if (mp->b_cont)
			freemsg(XCHG(&mp->b_cont, NULL));
		mp->b_cont = dp;
		mi->mi_cnt++;
		putq(q, mp);
	}
}

EXPORT_SYMBOL_NOVERS(mi_copyin_n);	/* mps/ddi.h */

/**
 * mi_copyout_alloc: - allocate a message block for a copyout operation
 * @q: write queue on which messagew as received
 * @mp: the M_IOCTL or M_IOCDATA message
 * @uaddr: explicit user address (or NULL for implicit address) to copy out to
 * @len: length of data to copy out
 * @free_on_error: when non-zero close input-output operation automatically on error
 */
mblk_t *
mi_copyout_alloc(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len, int free_on_error)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mi;
	mblk_t *db, *bp;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		if (ioc->iocblk.ioc_count == TRANSPARENT) {
			bp = XCHG(&mp->b_cont, NULL);
		} else {
			bp = allocb(sizeof(*mi), BPRI_MED);
			bzero(bp->b_rptr, sizeof(*mi));
		}
		if (!bp) {
			if (free_on_error)
				mi_copy_done(q, mp, ENOSR);
			return (NULL);
		}
		bp->b_wptr = bp->b_rptr + sizeof(*mi);
		mi = (typeof(mi)) bp->b_rptr;
		mi->mi_cnt = 0;
		mi->mi_dir = MI_COPY_OUT;
		mp->b_datap->db_type = M_IOCDATA;	/* for next pass */
		ioc->copyresp.cp_private = bp;
		ioc->copyresp.cp_rval = 0;
		break;
	case M_IOCDATA:
		if (!(bp = ioc->copyresp.cp_private)) {
			if (free_on_error)
				mi_copy_done(q, mp, EPROTO);
			return (NULL);
		}
		mi = (typeof(mi)) bp->b_rptr;
		if (mi->mi_dir == MI_COPY_IN) {
			if (bp->b_cont)
				freemsg(XCHG(&bp->b_cont, NULL));
			mi->mi_dir = MI_COPY_OUT;
		}
		break;
	default:
		if (free_on_error)
			mi_copy_done(q, mp, EPROTO);
		return (NULL);
	}
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	if (!(db = allocb(len, BPRI_MED))) {
		if (free_on_error)
			mi_copy_done(q, mp, ENOSR);
		return (db);
	}
	linkb(bp, db);
	db->b_next = (mblk_t *) (uaddr ? : mi->mi_caddr);
	return (db);
}

EXPORT_SYMBOL_NOVERS(mi_copyout_alloc);	/* mps/ddi.h */

/**
 * mi_copyout: - perform a pending copyout operation and possible close input-output control
 * @q: write queue on which message was received
 * @mp: the M_IOCTL or M_IOCDATA message
 */
void
mi_copyout(queue_t *q, mblk_t *mp)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mi;
	mblk_t *bp, *db;
	caddr_t uaddr;

	if (mp->b_datap->db_type != M_IOCDATA || !mp->b_cont || !(bp = ioc->copyresp.cp_private))
		return mi_copy_done(q, mp, EPROTO);
	/* This is for LiS that puts an error code in the cp_rval and expects an M_IOCNAK. */
	if (ioc->copyresp.cp_rval)
		return mi_copy_done(q, mp, (int) (long) ioc->copyresp.cp_rval);
	mi = (typeof(mi)) bp->b_rptr;
	if (!(db = bp->b_cont) || mi->mi_dir == MI_COPY_IN)
		return mi_copy_done(q, mp, 0);
	bp->b_cont = XCHG(&db->b_cont, NULL);
	uaddr = (caddr_t) XCHG(&db->b_next, NULL);
	if (uaddr == NULL)
		uaddr = mi->mi_caddr;
	if (uaddr != NULL) {
		/* explicit copyout operation */
		mp->b_datap->db_type = M_COPYOUT;
		ioc->copyreq.cq_private = bp;
		ioc->copyreq.cq_size = msgdsize(db);
		ioc->copyreq.cq_flag = 0;
		ioc->copyreq.cq_addr = uaddr;
		mi->mi_cnt++;
		mi->mi_dir = MI_COPY_OUT;
	} else {
		/* implicit copyout operation */
		if (ioc->copyresp.cp_private)	/* won't be back */
			freemsg(XCHG(&ioc->copyresp.cp_private, NULL));
		mp->b_datap->db_type = M_IOCACK;
		ioc->iocblk.ioc_error = 0;
		ioc->iocblk.ioc_count = msgdsize(db);
	}
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	mp->b_cont = db;
	qreply(q, mp);
	return;
}

EXPORT_SYMBOL_NOVERS(mi_copyout);	/* mps/ddi.h */

/**
 * mi_copy_state: - determine the state of an input-output control operation
 * @q: write queue on which message was received
 * @mp: the M_IOCDATA message
 * @mpp: message pointer to set do copied in data
 *
 * mi_copy_state() is only used on M_IOCDATA messages.  It checks the return code from the previous
 * copyin or copyout operation and returns an M_IOCNAK or aborts if an error occurred.  If data was
 * copied in, mpp is set to point to the data block containing the data.  The user must not free
 * this data block: it remains attached to the M_IOCDATA message.  If data was copied out, the
 * pointer pointed to by mpp is untouched.  The function returns -1 if an error occurred (in which
 * case the passed in message block is used to pass an M_IOCNAK upstream).  If no error occured, it
 * returns the state of the input-output control operation.
 */
int
mi_copy_state(queue_t *q, mblk_t *mp, mblk_t **mpp)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mi;
	mblk_t *bp, *db = NULL;
	int err = EPROTO;

	if (mp->b_datap->db_type != M_IOCDATA || !(db = mp->b_cont)
	    || !(bp = ioc->copyresp.cp_private))
		goto error;
	/* This is for LiS that puts an error code in the cp_rval and expects an M_IOCNAK. */
	if ((err = (int) (long) ioc->copyresp.cp_rval))
		goto error;
	switch (mp->b_datap->db_type) {
	case M_IOCDATA:
		err = ENOMEM;
		if (!pullupmsg(db, -1))
			goto error;
		mi = (typeof(mi)) bp->b_rptr;
		if (mi->mi_dir == MI_COPY_IN)
			*mpp = db;
		return MI_COPY_CASE(mi->mi_dir, mi->mi_cnt);
	case M_IOCTL:
		err = EPROTO;
		goto error;
	}
      error:
	mi_copy_done(q, mp, err);
	return (-1);
}

EXPORT_SYMBOL_NOVERS(mi_copy_state);	/* mps/ddi.h */

/**
 * mi_copy_set_rval: - set the input-output control operation return value
 * @mp: the M_IOCTL or M_IOCDATA message
 * @rval: the return value to set
 *
 * mi_copy_set_rval() is used to set the return value to something other than zero before the last
 * function in the input-output control operation is called.  The last functions called would be
 * either mi_copyout() or mi_copy_done().
 */
void
mi_copy_set_rval(mblk_t *mp, int rval)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mi;
	mblk_t *bp;

	switch (mp->b_datap->db_type) {
	case M_IOCDATA:
		if ((bp = ioc->copyresp.cp_private)) {
			mi = (typeof(mi)) bp->b_rptr;
			mi->mi_rval = rval;
			break;
		}
		/* fall through */
	case M_IOCTL:
		ioc->iocblk.ioc_rval = rval;
		break;
	}
	return;
}

EXPORT_SYMBOL_NOVERS(mi_copy_set_rval);

/*
 *  =========================================================================
 *
 *  Timeout helper functions
 *
 *  =========================================================================
 */

#define TB_ZOMBIE	-2	/* tb is a zombie, waiting to be freed, but may be on queue */
#define TB_CANCELLED	-1	/* tb has been cancelled, but may be on queue */
#define TB_IDLE		 0	/* tb is idle, not on queue */
#define TB_ACTIVE	 1	/* tb has timer actively running, but may be on queue */
#define TB_RESCHEDULED	 2	/* tb is being rescheduled, but may be on queue */
#define TB_TRANSIENT	 3	/* tb is in a transient state (unused) */

typedef struct mi_timeb {
	spinlock_t tb_lock;
	long tb_state;			/* the state of the timer */
	toid_t tb_tid;			/* the timeout id when a timeout is running */
	clock_t tb_time;		/* the time of expiry */
	queue_t *tb_q;			/* where to queue the timer message on timeout */
} tblk_t;

#undef mi_timer_alloc
#undef mi_timer

static streamscall void
mi_timer_expiry(caddr_t data)
{
	mblk_t *mp = (typeof(mp)) data;
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;

	if (xchg(&tb->tb_tid, 0)) {
		dassert(tb->tb_q != NULL);
		put(tb->tb_q, mp);
	}
}

/*
 *  MacOT variety
 */
/*
 *  MI_TIMER_ALLOC (MacOT variety)
 *  -------------------------------------------------------------------------
 */
mblk_t *
mi_timer_alloc_MAC(queue_t *q, size_t size)
{
	mblk_t *mp;
	tblk_t *tb;

	if ((mp = allocb(sizeof(*tb) + size, BPRI_HI))) {
		mp->b_datap->db_type = M_PCSIG;
		tb = (typeof(tb)) mp->b_rptr;
		mp->b_rptr = (typeof(mp->b_rptr)) (tb + 1);
		mp->b_wptr = mp->b_rptr + size;
		tb->tb_lock = SPIN_LOCK_UNLOCKED;
		tb->tb_state = TB_IDLE;
		tb->tb_tid = (toid_t) (0);
		tb->tb_time = (clock_t) (0);
		tb->tb_q = q;
	}
	return (mp);
}

EXPORT_SYMBOL_NOVERS(mi_timer_alloc_MAC);

/*
 *  MI_TIMER (MacOT variety)
 *  -------------------------------------------------------------------------
 */
void mi_timer_SUN(queue_t *q, mblk_t *mp, clock_t msec);
void
mi_timer_MAC(mblk_t *mp, clock_t msec)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;

	return mi_timer_SUN(tb->tb_q, mp, msec);
}

EXPORT_SYMBOL_NOVERS(mi_timer_MAC);

/*
 *  MI_TIMER_CANCEL (MacOT variety)
 *  -------------------------------------------------------------------------
 */
int
mi_timer_cancel(mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	unsigned long flags;
	toid_t tid;
	int rval;

	if ((tid = xchg(&tb->tb_tid, 0)))
		untimeout(tid);

	spin_lock_irqsave(&tb->tb_lock, flags);
	switch (tb->tb_state) {
	case TB_ACTIVE:
		if (tid) {
			/* sucessful cancel, not on queue */
			tb->tb_state = TB_IDLE;
			rval = 0;
			break;
		} else {
			/* unsuccessful cancel, will be queued */
			tb->tb_state = TB_CANCELLED;
			rval = -1;
			break;
		}
	case TB_RESCHEDULED:
		/* unsuccessful cancel, will be queued */
		tb->tb_state = TB_CANCELLED;
		rval = -1;
		break;
	case TB_IDLE:
		/* already successfully cancelled, not on queue */
		mp->b_datap->db_type = M_PCSIG;
		rval = 0;
		break;
	case TB_CANCELLED:
	case TB_ZOMBIE:
		/* already unsuccessfully cancelled, will be queued */
		rval = -1;
		break;
	case TB_TRANSIENT:
	default:
		swerr();
		rval = -1;
		break;
	}
	spin_unlock_irqrestore(&tb->tb_lock, flags);
	return (rval);
}

EXPORT_SYMBOL_NOVERS(mi_timer_cancel);

/*
 *  MI_TIMER_Q_SWITCH (MacOT variety)
 *  -------------------------------------------------------------------------
 */
mblk_t *
mi_timer_q_switch(mblk_t *mp, queue_t *q, mblk_t *new_mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;

	if (!mi_timer_cancel(mp)) {
		/* not on queue, can be rescheduled on new queue */
		mi_timer_SUN(q, mp, drv_hztomsec(tb->tb_time - jiffies));
		return (mp);
	} else {
		/* possibly on queue, use new timer block */
		mi_timer_SUN(q, new_mp, drv_hztomsec(tb->tb_time - jiffies));
		return (new_mp);
	}
}

EXPORT_SYMBOL_NOVERS(mi_timer_q_switch);

/*
 *  OpenSolaris variety
 */
/*
 *  MI_TIMER_ALLOC (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
mblk_t *
mi_timer_alloc_SUN(size_t size)
{
	return mi_timer_alloc_MAC(NULL, size);
}

EXPORT_SYMBOL_NOVERS(mi_timer_alloc_SUN);

/*
 *  MI_TIMER (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
void
mi_timer_SUN(queue_t *q, mblk_t *mp, clock_t msec)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;

	if (msec >= 0) {
		unsigned long flags;
		toid_t tid;

		/* tb has timer actively running */
		if ((tid = xchg(&tb->tb_tid, 0)))
			untimeout(tid);

		if (xchg(&mp->b_datap->db_type, M_PCSIG) == M_SIG)
			if (mp->b_next || tb->tb_q->q_first == mp)
				rmvq(tb->tb_q, mp);

		spin_lock_irqsave(&tb->tb_lock, flags);
		switch (tb->tb_state) {
		case TB_ACTIVE:
			/* tb is running */
			if (!tid)
				/* unsuccessful cancel, will be queued */
				goto reschedule;
			/* fall through */
		case TB_IDLE:
			/* tb is idle, not on queue */
			tb->tb_q = q;
			tb->tb_state = TB_ACTIVE;
			tb->tb_time = jiffies + drv_msectohz(msec);
			if (tb->tb_time > jiffies &&
			    !(tb->tb_tid = timeout(mi_timer_expiry, (caddr_t) mp,
						   tb->tb_time - jiffies)))
				tb->tb_state = TB_RESCHEDULED;
			if (!tb->tb_tid) {
				/* expired or needs reschedule */
				spin_unlock_irqrestore(&tb->tb_lock, flags);
				put(tb->tb_q, mp);
				return;
			}
			break;
		case TB_CANCELLED:
			/* tb has been cancelled, but will be queued */
		      reschedule:
			/* fall through */
		case TB_RESCHEDULED:
			/* tb is being rescheduled, and will be queued */
			tb->tb_q = q;
			tb->tb_state = TB_RESCHEDULED;
			tb->tb_time = jiffies + drv_msectohz(msec);
			break;
#if 0
		case TB_TRANSIENT:
			/* tb is in a transient state */
#endif
		case TB_ZOMBIE:
			/* tb is a zombie, waiting to be freed */
		default:
			swerr();
			break;
		}
		spin_unlock_irqrestore(&tb->tb_lock, flags);
	} else {
		switch (msec) {
		case ((clock_t) (-1)):
			mi_timer_stop(mp);
			break;
		case ((clock_t) (-2)):
			mi_timer_move(q, mp);
			break;
		default:
			swerr();
			break;
		}
	}
}

EXPORT_SYMBOL_NOVERS(mi_timer_SUN);

/*
 *  MI_TIMER_STOP (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
void
mi_timer_stop(mblk_t *mp)
{
	mi_timer_cancel(mp);
}

EXPORT_SYMBOL_NOVERS(mi_timer_stop);

/*
 *  MI_TIMER_MOVE (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
void
mi_timer_move(queue_t *q, mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	unsigned long flags;
	toid_t tid;

	if ((tid = xchg(&tb->tb_tid, 0)))
		untimeout(tid);

	spin_lock_irqsave(&tb->tb_lock, flags);
	switch (tb->tb_state) {
	case TB_ACTIVE:
		/* tb is running */
		if (tid) {
			/* sucessful cancel, not on queue, simply start */
			tb->tb_q = q;
			tb->tb_state = TB_ACTIVE;
			if (tb->tb_time > jiffies &&
			    !(tb->tb_tid = timeout(mi_timer_expiry, (caddr_t) mp,
						   tb->tb_time - jiffies)))
				tb->tb_state = TB_RESCHEDULED;
			if (!tb->tb_tid) {
				/* expired or needs reschedule */
				spin_unlock_irqrestore(&tb->tb_lock, flags);
				put(tb->tb_q, mp);
				return;
			}
		} else {
			/* unsuccessful cancel, will be queued */
			/* ask for reschedule on new queue */
			tb->tb_q = q;
			tb->tb_state = TB_RESCHEDULED;
		}
		break;
	case TB_IDLE:
		/* tb is idle, not on queue */
	case TB_CANCELLED:
	case TB_RESCHEDULED:
		tb->tb_q = q;
		break;
	case TB_ZOMBIE:
	default:
		swerr();
		break;
	}
	spin_unlock_irqrestore(&tb->tb_lock, flags);
}

EXPORT_SYMBOL_NOVERS(mi_timer_move);

/*
 *  Common
 */
/*
 *  MI_TIMER_VALID (Common)
 *  -------------------------------------------------------------------------
 *  Slight variation on a theme here.  We allow the message to be requeued and
 *  processed later.  We mark the message type to M_SIG when valid is returned
 *  so that the message will be requeued as a normal priority message.  When
 *  mi_timer_valid is called for the second time on the message, this function
 *  returns true again if no other action has been taken on the timer while it
 *  was waiting on queue.  If any other action was taken, the second pass
 *  will return false.
 */
int
mi_timer_valid(mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	unsigned long flags;
	int rval;

	spin_lock_irqsave(&tb->tb_lock, flags);
	switch (tb->tb_state) {
	case TB_ACTIVE:
		if (tb->tb_tid == (toid_t) 0) {
			/* queued as a result of a timeout */
			tb->tb_state = TB_IDLE;
			mp->b_datap->db_type = M_SIG;
			rval = (1);
			break;
		} else {
			/* timer still running, will be requeued */
			rval = (0);
			break;
		}
	case TB_CANCELLED:
		/* were cancelled, now we are idle (off queue) */
		tb->tb_state = TB_IDLE;
		rval = (0);
		break;
	case TB_IDLE:
		rval = (mp->b_datap->db_type != M_PCSIG);
		break;
	case TB_ZOMBIE:
		/* were zombied, now we can be freed (off queue) */
		local_irq_restore(flags);
		freemsg(mp);
		return (0);
	case TB_RESCHEDULED:
		tb->tb_state = TB_ACTIVE;
		if (tb->tb_time > jiffies &&
		    !(tb->tb_tid = timeout(mi_timer_expiry, (caddr_t) mp, tb->tb_time - jiffies)))
			tb->tb_state = TB_RESCHEDULED;
		if (!tb->tb_tid) {
			spin_unlock_irqrestore(&tb->tb_lock, flags);
			put(tb->tb_q, mp);
			return (0);
		}
		rval = (0);
		break;
	default:
		swerr();
		rval = (0);
		break;
	}
	spin_unlock_irqrestore(&tb->tb_lock, flags);
	return (rval);
}

EXPORT_SYMBOL_NOVERS(mi_timer_valid);

/*
 *  MI_TIMER_FREE (Common)
 *  -------------------------------------------------------------------------
 */
void
mi_timer_free(mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	unsigned long flags;
	toid_t tid;

	if ((tid = xchg(&tb->tb_tid, 0)))
		untimeout(tid);

	spin_lock_irqsave(&tb->tb_lock, flags);
	switch (tb->tb_state) {
	case TB_ACTIVE:
		if (!tid)
			/* unsuccessful cancel, will be queued */
			goto zombie;
		/* fall through */
	case TB_IDLE:
		/* not on queue */
		mp->b_datap->db_type = M_PCSIG;
		local_irq_restore(flags);
		freemsg(mp);
		return;
	default:
	      zombie:
		tb->tb_state = TB_ZOMBIE;
		break;
	}
	spin_unlock_irqrestore(&tb->tb_lock, flags);
}

EXPORT_SYMBOL_NOVERS(mi_timer_free);

queue_t *
mi_allocq(struct streamtab *st)
{
#ifdef LIS
	return lis_allocq(st->st_rdinit->qi_minfo->mi_idname);
#endif
#ifdef LFS
	queue_t *q;

	if ((q = allocq()))
		setq(q, st->st_rdinit, st->st_wrinit);
	return (q);
#endif
}

EXPORT_SYMBOL_NOVERS(mi_allocq);

void
mi_freeq(queue_t *q)
{
#ifdef LIS
	lis_freeq(q);
#endif
#ifdef LFS
	freeq(q);
#endif
}

EXPORT_SYMBOL_NOVERS(mi_freeq);

int
mi_strlog(queue_t *q, char level, ushort flags, char *fmt, ...)
{
	int result = 0;

	if (vstrlog != NULL) {
		struct mi_comm *mi = (struct mi_comm *) q->q_ptr + 1;
		modID_t mid = mi->mi_mid;
		minor_t sid = mi->mi_sid;
		va_list args;

		va_start(args, fmt);
		result = vstrlog(mid, sid, level, flags, fmt, args);
		va_end(args);
	}
	return (result);
}

EXPORT_SYMBOL_NOVERS(mi_strlog);

#define _MI_FLAG_ZEROPAD	(1<<0)
#define _MI_FLAG_SIGN		(1<<1)
#define _MI_FLAG_PLUS		(1<<2)
#define _MI_FLAG_SPACE		(1<<3)
#define _MI_FLAG_LEFT		(1<<4)
#define _MI_FLAG_SPECIAL	(1<<5)
#define _MI_FLAG_LARGE		(1<<6)

static int
mi_putc(char c, mblk_t **mpp)
{
	if (unlikely((*mpp)->b_wptr >= (*mpp)->b_datap->db_lim)) {
		mblk_t *mp;

		mp = allocb(256, BPRI_MED);
		(*mpp)->b_cont = mp;
		mpp = &(*mpp)->b_cont;
	}
	if (likely(*mpp != NULL)) {
		(*mpp)->b_wptr[0] = (unsigned char) c;
		(*mpp)->b_wptr++;
		return (1);
	}
	return (0);
}

static int
mi_number(mblk_t **mpp, unsigned long long num, int base, int width, int decimal, int flags)
{
	char sign;
	int i, count = 0, rval = 0;
	char tmp[66];

	if (flags & _MI_FLAG_LEFT)
		flags &= ~_MI_FLAG_ZEROPAD;
	if (base < 2 || base > 16)
		return (count);
	sign = '\0';
	if (flags & _MI_FLAG_SIGN) {
		if ((signed long long) num < 0) {
			sign = '-';
			num = -(signed long long) num;
			width--;
		} else if (flags & _MI_FLAG_PLUS) {
			sign = '+';
			width--;
		} else if (flags & _MI_FLAG_SPACE) {
			sign = ' ';
			width--;
		}
	}
	if (flags & _MI_FLAG_SPECIAL) {
		switch (base) {
		case 16:
			width -= 2;	/* for 0x */
			break;
		case 8:
			width--;	/* for 0 */
			break;
		}
	}
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else {
		const char *hexdig =
		    (flags & _MI_FLAG_LARGE) ? "0123456789ABCDEF" : "0123456789abcdef";
		for (; num != 0; tmp[i++] = hexdig[do_div(num, base)]) ;
	}
	if (i > decimal)
		decimal = i;
	width -= decimal;
	if (!(flags & (_MI_FLAG_ZEROPAD | _MI_FLAG_LEFT)))
		while (width-- > 0) {
			if (!mi_putc(' ', mpp))
				goto abort;
			count++;
		}
	if (sign != '\0') {
		if (!mi_putc(sign, mpp))
			goto abort;
		count++;
	}
	if (flags & _MI_FLAG_SPECIAL) {
		switch (base) {
		case 8:
			if (!mi_putc('0', mpp))
				goto abort;
			count++;
			break;
		case 16:
		{
			char xchar = (flags & _MI_FLAG_LARGE) ? 'X' : 'x';

			if (!mi_putc('0', mpp))
				goto abort;
			count++;
			if (!mi_putc(xchar, mpp))
				goto abort;
			count++;
			break;
		}
		}
	}
	if (!(flags & _MI_FLAG_LEFT)) {
		char pad = (flags & _MI_FLAG_ZEROPAD) ? '0' : ' ';

		while (width-- > 0) {
			if (!mi_putc(pad, mpp))
				goto abort;
			count++;
		}
	}
	while (i < decimal--) {
		if (!mi_putc('0', mpp))
			goto abort;
		count++;
	}
	while (i-- > 0) {
		if (!mi_putc(tmp[i], mpp))
			goto abort;
		count++;
	}
	while (width-- > 0) {
		if (!mi_putc(' ', mpp))
			goto abort;
		count++;
	}
	rval = count;
      abort:
	return (rval);
}

static int
mi_vmpprintf(mblk_t *mp, char *fmt, va_list args)
{
	char type;
	int count = 0, rval = -1, flags = 0, width = -1, decimal = -1, base = 10;
	mblk_t **mpp = &mp;

	for (; *fmt; ++fmt) {
		const char *pos;
		unsigned long long num = 0;

		if (*fmt != '%') {
			if (!mi_putc(*fmt, mpp))
				goto abort;
			count++;
			continue;
		}
		pos = fmt;	/* remember position of % */
		/* process flags */
		for (++fmt;; ++fmt) {
			switch (*fmt) {
			case '-':
				flags |= _MI_FLAG_LEFT;
				continue;
			case '+':
				flags |= _MI_FLAG_PLUS;
				continue;
			case ' ':
				flags |= _MI_FLAG_SPACE;
				continue;
			case '#':
				flags |= _MI_FLAG_SPECIAL;
				continue;
			case '0':
				flags |= _MI_FLAG_ZEROPAD;
				continue;
			default:
				break;
			}
		}
		/* get field width */
		if (isdigit(*fmt))
			for (width = 0; isdigit(*fmt); width *= 10, width += (*fmt - '0')) ;
		else if (*fmt == '*') {
			++fmt;
			if ((width = va_arg(args, int)) < 0) {
				width = -width;
				flags |= _MI_FLAG_LEFT;
			}
		}
		/* get the decimal precision */
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt))
				for (decimal = 0; isdigit(*fmt);
				     decimal += 10, decimal += (*fmt - '0')) ;
			else if (*fmt == '*') {
				if ((decimal = va_arg(args, int)) < 0)
					 decimal = 0;
			}
		}
		/* get conversion type */
		type = 'i';
		switch (*fmt) {
		case 'h':
			type = *fmt;
			++fmt;
			if (*fmt == 'h') {
				type = 'c';
				++fmt;
			}
			break;
		case 'l':
			type = *fmt;
			++fmt;
			if (*fmt == 'l') {
				type = 'L';
				++fmt;
			}
			break;
		case 'q':
		case 'L':
		case 'Z':
		case 'z':
		case 't':
			type = *fmt;
			++fmt;
			break;
		}
		switch (*fmt) {
		case 'c':
		{
			char c;

			if (!(flags & _MI_FLAG_LEFT))
				while (--width > 0) {
					if (!mi_putc(' ', mpp))
						goto abort;
					count++;
				}
			c = va_arg(args, int);

			if (!mi_putc(c, mpp))
				goto abort;
			count++;
			while (--width > 0) {
				if (!mi_putc(' ', mpp))
					goto abort;
				count++;
			}
			continue;
		}
		case 's':
		{
			const char *s;
			int i;
			size_t len = 0;
			s = va_arg(args, char *);

			len = strnlen(s, 1024);
			if (!(flags & _MI_FLAG_LEFT))
				while (len < width--) {
					if (!mi_putc(' ', mpp))
						goto abort;
					count++;
				}
			for (i = 0; i < len; ++i, ++s) {
				if (!mi_putc(*s, mpp))
					goto abort;
				count++;
			}
			while (len < width--) {
				if (!mi_putc(' ', mpp))
					goto abort;
				count++;
			}
			continue;
		}
		case '%':
			if (!mi_putc('%', mpp))
				goto abort;
			count++;
			continue;
		case 'p':
		case 'o':
		case 'X':
		case 'x':
		case 'd':
		case 'i':
		case 'u':
			switch (*fmt) {
			case 'p':
				type = 'p';
				if (width == -1) {
					width = 2 * sizeof(void *);
					flags |= _MI_FLAG_ZEROPAD;
				}
				base = 16;
				break;
			case 'o':
				base = 8;
				break;
			case 'X':
				flags |= _MI_FLAG_LARGE;
			case 'x':
				base = 16;
				break;
			case 'd':
			case 'i':
				flags |= _MI_FLAG_SIGN;
			case 'u':
				base = 10;
				break;
			}
			switch (type) {
			case 'c':
				num = va_arg(args, int);

				if (flags & _MI_FLAG_SIGN)
					num = (signed char) num;
				break;
			case 'h':
				num = va_arg(args, int);

				if (flags & _MI_FLAG_SIGN)
					num = (signed short) num;
				break;
			case 'p':
				num = va_arg(args, uintptr_t);
				flags &= ~_MI_FLAG_SIGN;
				break;
			case 'l':
				num = va_arg(args, unsigned long);

				if (flags & _MI_FLAG_SIGN)
					num = (signed long) num;
				break;
			case 'q':
			case 'L':
				num = va_arg(args, unsigned long long);

				if (flags & _MI_FLAG_SIGN)
					num = (signed long long) num;
				break;
			case 'Z':
			case 'z':
				num = va_arg(args, size_t);

				if (flags & _MI_FLAG_SIGN)
					num = (ssize_t) num;
				break;
			case 't':
				num = va_arg(args, ptrdiff_t);
				break;
			case 'i':
			default:
				num = va_arg(args, unsigned int);

				if (flags & _MI_FLAG_SIGN)
					num = (signed int) num;
				break;
			}
			{
				int result;

				if (!(result = mi_number(mpp, num, base, width, decimal, flags)))
					goto abort;
				count += result;
			}
			continue;
		case '\0':
		default:
			/* put origianl '%' */
			if (!mi_putc('%', mpp))
				goto abort;
			count++;
			/* put the bad format characters */
			for (; fmt > pos; pos++, count++)
				if (!mi_putc(*pos, mpp))
					goto abort;
			if (*fmt == '\0')
				break;
			continue;
		}
		break;
	}
	if (!mi_putc('\0', mpp))
		goto abort;
	rval = count;
      abort:
	return (rval);
}

int
mi_mpprintf(mblk_t *mp, char *fmt, ...)
{
	va_list args;
	int count = -1;

	va_start(args, fmt);
	if (mp)
		count = mi_vmpprintf(mp, fmt, args);
	va_end(args);
	return (count);
}

EXPORT_SYMBOL_NOVERS(mi_mpprintf);

int
mi_mpprintf_nr(mblk_t *mp, char *fmt, ...)
{
	va_list args;
	int count = -1;

	va_start(args, fmt);
	if (mp) {
		adjmsg(mp, -1);
		count = mi_vmpprintf(mp, fmt, args);
	}
	va_end(args);
	return (count);
}

EXPORT_SYMBOL_NOVERS(mi_mpprintf_nr);

/*
 *  =========================================================================
 *
 *  Stream head helper functions
 *
 *  =========================================================================
 */
/*
 *  MI_SET_STH_HIWAT
 *  -------------------------------------------------------------------------
 */
int
mi_set_sth_hiwat(queue_t *q, size_t size)
{
	struct stroptions *so;
	mblk_t *mp;

	assert(q == RD(q));
	if ((mp = allocb(sizeof(*so), BPRI_MED))) {
		mp->b_datap->db_type = M_SETOPTS;
		mp->b_wptr += sizeof(*so);
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = SO_HIWAT;
		so->so_band = 0;
		so->so_hiwat = size;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(mi_set_sth_hiwat);

/*
 *  MI_SET_STH_LOWAT
 *  -------------------------------------------------------------------------
 */
int
mi_set_sth_lowat(queue_t *q, size_t size)
{
	struct stroptions *so;
	mblk_t *mp;

	assert(q == RD(q));
	if ((mp = allocb(sizeof(*so), BPRI_MED))) {
		mp->b_datap->db_type = M_SETOPTS;
		mp->b_wptr += sizeof(*so);
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = SO_LOWAT;
		so->so_band = 0;
		so->so_lowat = size;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(mi_set_sth_lowat);

/*
 *  MI_SET_STH_MAXBLK
 *  -------------------------------------------------------------------------
 */
int
mi_set_sth_maxblk(queue_t *q, ssize_t size)
{
#ifdef SO_MAXBLK
	struct stroptions *so;
	mblk_t *mp;

	assert(q == RD(q));
	if ((mp = allocb(sizeof(*so), BPRI_MED))) {
		mp->b_datap->db_type = M_SETOPTS;
		mp->b_wptr += sizeof(*so);
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = SO_MAXBLK;
		so->so_maxblk = size;
		putnext(q, mp);
		return (1);
	}
#endif
	return (0);
}

EXPORT_SYMBOL_NOVERS(mi_set_sth_maxblk);

/*
 *  MI_SET_STH_COPYOPT
 *  -------------------------------------------------------------------------
 */
int
mi_set_sth_copyopt(queue_t *q, int copyopt)
{
#ifdef SO_COPYOPT
	struct stroptions *so;
	mblk_t *mp;

	assert(q == RD(q));
	if ((mp = allocb(sizeof(*so), BPRI_MED))) {
		mp->b_datap->db_type = M_SETOPTS;
		mp->b_wptr += sizeof(*so);
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = SO_COPYOPT;
		so->so_copyopt = copyopt;
		putnext(q, mp);
		return (1);
	}
#endif
	return (0);
}

EXPORT_SYMBOL_NOVERS(mi_set_sth_copyopt);

/*
 *  MI_SET_STH_WROFF
 *  -------------------------------------------------------------------------
 */
int
mi_set_sth_wroff(queue_t *q, size_t size)
{
	struct stroptions *so;
	mblk_t *mp;

	assert(q == RD(q));
	if ((mp = allocb(sizeof(*so), BPRI_MED))) {
		mp->b_datap->db_type = M_SETOPTS;
		mp->b_wptr += sizeof(*so);
		so = (typeof(so)) mp->b_rptr;
		so->so_flags = SO_WROFF;
		so->so_wroff = size;
		putnext(q, mp);
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL_NOVERS(mi_set_sth_wroff);

/*
 *  =========================================================================
 *
 *  System wrapper functions
 *
 *  =========================================================================
 */
int
mi_sprintf(char *buf, char *fmt, ...)
{
	int result;
	va_list args;

	va_start(args, fmt);
	result = vsprintf(buf, fmt, args);
	va_end(args);
	return result;
}

EXPORT_SYMBOL_NOVERS(mi_sprintf);

int
mi_strcmp(const caddr_t cp1, const caddr_t cp2)
{
	return strcmp(cp1, cp2);
}

EXPORT_SYMBOL_NOVERS(mi_strcmp);

int
mi_strlen(const caddr_t str)
{
	return strlen(str);
}

EXPORT_SYMBOL_NOVERS(mi_strlen);

long
mi_strtol(const caddr_t str, caddr_t *ptr, int base)
{
	return simple_strtol(str, ptr, base);
}

EXPORT_SYMBOL_NOVERS(mi_strtol);

/*
 *  =========================================================================
 *
 *  Message block functions
 *
 *  =========================================================================
 */
/*
 *  MI_OFFSET_PARAM
 *  -------------------------------------------------------------------------
 */
uint8_t *
mi_offset_param(mblk_t *mp, size_t offset, size_t len)
{
	if (mp || len == 0) {
		size_t blen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

		if (blen >= offset + len)
			return (mp->b_rptr + offset);
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(mi_offset_param);

/*
 *  MI_OFFSET_PARAMC
 *  -------------------------------------------------------------------------
 */
uint8_t *
mi_offset_paramc(mblk_t *mp, size_t offset, size_t len)
{
	uint8_t *result = NULL;

	for (; mp; mp = mp->b_cont) {
		if (isdatamsg(mp)) {
			if ((result = mi_offset_param(mp, offset, len)))
				break;
			else {
				size_t blen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

				if (offset < blen)
					/* spans blocks - should do a pullup */
					break;
				offset -= blen;
			}
		}
	}
	return (result);
}

EXPORT_SYMBOL_NOVERS(mi_offset_paramc);

/*
 *  =========================================================================
 *
 *  Some internal functions showing...
 *
 *  =========================================================================
 */
/*
 *  MPS_BECOME_WRITER
 *  -------------------------------------------------------------------------
 */
void
mps_become_writer(queue_t *q, mblk_t *mp, proc_ptr_t proc)
{
#ifdef LIS
	lis_flags_t flags;

	LIS_QISRLOCK(q, &flags);
	(*proc) (q, mp);
	LIS_QISRUNLOCK(q, &flags);
#endif
#ifdef LFS
	extern void __strwrit(queue_t *q, mblk_t *mp, void (*func) (queue_t *, mblk_t *),
			      int perim);
	__strwrit(q, mp, proc, PERIM_INNER | PERIM_OUTER);
#endif
}

EXPORT_SYMBOL_NOVERS(mps_become_writer);

/*
 *  MPS_INTR_DISABLE
 *  -------------------------------------------------------------------------
 */
void
mps_intr_disable(pl_t * plp)
{
	unsigned long flags = *plp;

	local_irq_save(flags);
	*plp = flags;
}

EXPORT_SYMBOL_NOVERS(mps_intr_disable);

/*
 *  MPS_INTR_ENABLE
 *  -------------------------------------------------------------------------
 */
void
mps_intr_enable(pl_t pl)
{
	unsigned long flags = pl;

	local_irq_restore(flags);
}

EXPORT_SYMBOL_NOVERS(mps_intr_enable);

#ifdef CONFIG_STREAMS_COMPAT_MPS_MODULE
static
#endif
int __init
mpscomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_MPS_MODULE
	printk(KERN_INFO MPSCOMP_BANNER);
#else
	printk(KERN_INFO MPSCOMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_MPS_MODULE
static
#endif
void __exit
mpscomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_MPS_MODULE
module_init(mpscomp_init);
module_exit(mpscomp_exit);
#endif
