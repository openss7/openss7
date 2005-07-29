/*****************************************************************************

 @(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/29 07:37:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2005/07/29 07:37:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mpscompat.c,v $
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

#ident "@(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/29 07:37:51 $"

static char const ident[] =
    "$RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/29 07:37:51 $";

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

#define __MPS_EXTERN_INLINE inline

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
#define MPSCOMP_REVISION	"LfS $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2005/07/29 07:37:51 $"
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

EXPORT_SYMBOL(mi_bcmp);

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void *mi_alloc(size_t size, unsigned int pri);

EXPORT_SYMBOL(mi_alloc);	/* mps/ddi.h */

/*
 *  MI_ALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void *mi_alloc_sleep(size_t size, unsigned int pri);

EXPORT_SYMBOL(mi_alloc_sleep);	/* mps/ddi.h */

/*
 *  MI_ZALLOC
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE caddr_t mi_zalloc(size_t size);

EXPORT_SYMBOL(mi_zalloc);	/* mps/ddi.h */

/*
 *  MI_ZALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE caddr_t mi_zalloc_sleep(size_t size);

EXPORT_SYMBOL(mi_zalloc_sleep);	/* mps/ddi.h */

/*
 *  MI_FREE
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_free(void *ptr);

EXPORT_SYMBOL(mi_free);		/* mps/ddi.h */

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
	union {
		dev_t dev;		/* device number (or NODEV for modules) */
		int index;		/* link index */
	} mi_u;
	size_t mi_size;			/* size of this structure plus private data */
	char mi_priv[0];		/* followed by private data */
};

#define mi_dev mi_u.dev
#define mi_index mi_u.index

/*
 *  MI_OPEN_ALLOC
 *  -------------------------------------------------------------------------
 */
caddr_t mi_open_alloc(size_t size)
{
	struct mi_comm *mi;
	if ((mi = kmem_zalloc(sizeof(struct mi_comm) + size, KM_NOSLEEP))) {
		mi->mi_size = size;
		return ((caddr_t) (mi + 1));
	}
	return (NULL);
}

EXPORT_SYMBOL(mi_open_alloc);	/* mps/ddi.h */

/*
 *  MI_OPEN_ALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
caddr_t mi_open_alloc_sleep(size_t size)
{
	struct mi_comm *mi;
	if ((mi = kmem_zalloc(sizeof(struct mi_comm) + size, KM_SLEEP))) {
		mi->mi_size = size;
		return ((caddr_t) (mi + 1));
	}
	return (NULL);
}

EXPORT_SYMBOL(mi_open_alloc_sleep);	/* mps/ddi.h */

/*
 * MI_FIRST_PTR
 * -------------------------------------------------------------------------
 * OpenSolaris uses a head structure, we don't, primarily because the
 * documentation for mi_open_comm for both AIX and MacOT says that we use a
 * "static pointer" initialized to NULL.
 */
caddr_t mi_first_ptr(caddr_t *mi_head)
{
	struct mi_comm *mi = *(struct mi_comm **) mi_head;

	return mi ? ((caddr_t) (mi + 1)) : NULL;
}

EXPORT_SYMBOL(mi_first_ptr);	/* mps/ddi.h */

/*
 *  MI_FIRST_DEV_PTR
 *  -------------------------------------------------------------------------
 *  OpenSolaris tracks modules and drivers on the same list.  We don't,
 *  primarily because AIX and MacOT don't document it that way.  If you have a
 *  STREAMS driver that can also be pushed as a module, use two separate lists.
 */
caddr_t mi_first_dev_ptr(caddr_t *mi_head)
{
	struct mi_comm *mi = *(struct mi_comm **) mi_head;

	for (; mi && getmajor(mi->mi_dev) != 0; mi = mi->mi_next) ;
	return mi ? ((caddr_t) (mi + 1)) : NULL;
}

EXPORT_SYMBOL(mi_first_dev_ptr);	/* mps/ddi.h */

/*
 *  MI_NEXT_PTR
 *  -------------------------------------------------------------------------
 */
caddr_t mi_next_ptr(caddr_t ptr)
{
	struct mi_comm *mi = ptr ? (((struct mi_comm *) ptr) - 1)->mi_next : NULL;

	return mi ? ((caddr_t) (mi + 1)) : NULL;
}

EXPORT_SYMBOL(mi_next_ptr);	/* mps/ddi.h, aix/ddi.h */

/*
 *  MI_NEXT_DEV_PTR
 *  -------------------------------------------------------------------------
 *  OpenSolaris tracks modules and drivers on the same list.  We don't,
 *  primarily because AIX and MacOT don't document it that way.  If you have a
 *  STREAMS driver that can also be pushed as a module, use two separate lists.
 */
caddr_t mi_next_dev_ptr(caddr_t *mi_head, caddr_t ptr)
{
	struct mi_comm *mi = ptr ? (((struct mi_comm *) ptr) - 1) : NULL;

	for (mi = mi ? mi->mi_next : NULL; mi && getmajor(mi->mi_dev) != 0; mi = mi->mi_next) ;
	return mi ? ((caddr_t) (mi + 1)) : NULL;
}

EXPORT_SYMBOL(mi_next_dev_ptr);	/* mps/ddi.h */

/* 
 *  MI_PREV_PTR
 *  -------------------------------------------------------------------------
 *  Linux Fast-STREAMS embellishment.
 */
caddr_t mi_prev_ptr(caddr_t ptr)
{
	struct mi_comm *mi = ptr ? (((struct mi_comm *) ptr) - 1) : NULL;

	if (mi && mi->mi_prev != &mi->mi_next && mi->mi_prev != mi->mi_head)
		return ((caddr_t) (((struct mi_comm *) mi->mi_prev) + 1));
	return (NULL);
}

EXPORT_SYMBOL(mi_prev_ptr);	/* mps/ddi.h, aix/ddi.h */

static spinlock_t mi_list_lock = SPIN_LOCK_UNLOCKED;

/*
 *  MI_OPEN_LINK
 *  -------------------------------------------------------------------------
 */
int mi_open_link(caddr_t *mi_head, caddr_t ptr, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	struct mi_comm *mi = ((struct mi_comm *) ptr) - 1, **mip = (struct mi_comm **) mi_head;
	major_t cmajor = devp ? getmajor(*devp) : 0;
	minor_t cminor = devp ? getminor(*devp) : 0;

	spin_lock(&mi_list_lock);
	switch (sflag) {
	case CLONEOPEN:
		/* first clone minor (above 5 per AIX docs, above 10 per MacOT docs), but the
		   caller can start wherever they want above that */
#define MI_OPEN_COMM_CLONE_MINOR 10
		if (cminor <= MI_OPEN_COMM_CLONE_MINOR)
			cminor = MI_OPEN_COMM_CLONE_MINOR + 1;
		/* fall through */
	default:
	case DRVOPEN:
	{
		major_t dmajor = cmajor;

		for (; *mip && (dmajor = getmajor((*mip)->mi_dev)) < cmajor;
		     mip = &(*mip)->mi_next) ;
		for (; *mip && dmajor == getmajor((*mip)->mi_dev)
		     && getminor(makedevice(0, cminor)) != 0; mip = &(*mip)->mi_next, cminor++) {
			minor_t dminor = getminor((*mip)->mi_dev);

			if (cminor < dminor)
				break;
			if (cminor == dminor)
				if (sflag != CLONEOPEN) {
					spin_unlock(&mi_list_lock);
					return (ENXIO);
				}
		}
		if (getminor(makedevice(0, cminor)) == 0) {	/* no minors left */
			spin_unlock(&mi_list_lock);
			return (EAGAIN);
		}
		mi->mi_dev = makedevice(cmajor, cminor);
		break;
	}
	case MODOPEN:
	{
		/* just push modules on list with no device */
#ifdef NODEV
		mi->mi_dev = NODEV;
#else
		mi->mi_dev = 0;
#endif
		break;
	}
	}
	if ((mi->mi_next = *mip))
		mi->mi_next->mi_prev = &mi;
	mi->mi_prev = mip;
	*mip = mi;
	mi->mi_head = (struct mi_comm **) mi_head;
	spin_unlock(&mi_list_lock);
	return (0);
}

EXPORT_SYMBOL(mi_open_link);	/* mps/ddi.h */

/*
 *  MI_OPEN_DETACHED
 *  -------------------------------------------------------------------------
 */
caddr_t mi_open_detached(caddr_t *mi_head, size_t size, dev_t *devp);

EXPORT_SYMBOL(mi_open_detached);	/* mps/ddi.h */

/*
 *  MI_ATTACH
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_attach(queue_t *q, caddr_t ptr);

EXPORT_SYMBOL(mi_attach);	/* mps/ddi.h, mac/ddi.h */

/* 
 *  MI_OPEN_COMM
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE int mi_open_comm(caddr_t *mi_head, uint size, queue_t *q, dev_t *devp, int flag,
				     int sflag, cred_t *credp);
EXPORT_SYMBOL(mi_open_comm);	/* mps/ddi.h, aix/ddi.h */

/*
 *  MI_CLOSE_UNLINK
 *  -------------------------------------------------------------------------
 */
void mi_close_unlink(caddr_t *mi_head, caddr_t ptr)
{
	if (ptr) {
		struct mi_comm *mi = ((struct mi_comm *) ptr) - 1;

		spin_lock(&mi_list_lock);
		if (mi_head == NULL || mi->mi_head == (struct mi_comm **) mi_head) {
			if ((*mi->mi_prev = mi->mi_next))
				mi->mi_next->mi_prev = mi->mi_prev;
			mi->mi_next = NULL;
			mi->mi_prev = &mi->mi_next;
			mi->mi_head = NULL;
		}
		spin_unlock(&mi_list_lock);
	}
}

EXPORT_SYMBOL(mi_close_unlink);	/* mps/ddi.h */

/*
 *  MI_CLOSE_FREE
 *  -------------------------------------------------------------------------
 */
void mi_close_free(caddr_t ptr)
{
	struct mi_comm *mi = ((struct mi_comm *) ptr) - 1;

	if (ptr && mi->mi_head == NULL && mi->mi_next == NULL)
		kmem_free(mi, mi->mi_size);
}

EXPORT_SYMBOL(mi_close_free);	/* mps/ddi.h */

/*
 *  MI_DETACH
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_detach(queue_t *q, caddr_t ptr);

EXPORT_SYMBOL(mi_detach);	/* mps/ddi.h */

/*
 *  MI_CLOSE_DETACHED
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_close_detached(caddr_t *mi_head, caddr_t ptr);

EXPORT_SYMBOL(mi_close_detached);	/* mps/ddi.h */

/* 
 *  MI_CLOSE_COMM
 *  -------------------------------------------------------------------------
 */
int mi_close_comm(caddr_t *mi_head, queue_t *q);

EXPORT_SYMBOL(mi_close_comm);	/* mps/ddi.h, aix/ddi.h */

struct mi_iocblk {
	caddr_t mi_uaddr;
	short mi_dir;
	short mi_cnt;
};

/*
 *  =========================================================================
 *
 *  Buffer call helper function.
 *
 *  =========================================================================
 */
__MPS_EXTERN_INLINE void mi_bufcall(queue_t *q, int size, int priority);

EXPORT_SYMBOL(mi_bufcall);

/*
 *  =========================================================================
 *
 *  Message block allocation helper functions.
 *
 *  =========================================================================
 */

mblk_t *mi_reuse_proto(mblk_t *mp, size_t size, int keep_on_error)
{
	if (unlikely(mp == NULL || (size > FASTBUF && mp->b_datap->db_lim - mp->b_rptr < size)
		     || mp->b_datap->db_ref > 1 || mp->b_datap->db_frtnp != NULL)) {
		/* can't reuse this message block (or no message block to begin with) */
		if (mp && !keep_on_error)
			freemsg(xchg(&mp, NULL));
	} else
		/* simply resize it - leave everything else intact */
		mp->b_wptr = mp->b_rptr + size;
	return (mp);
}

EXPORT_SYMBOL(mi_reuse_proto);

mblk_t *mi_reallocb(mblk_t *mp, size_t size)
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

EXPORT_SYMBOL(mi_reallocb);

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

/*
 *  MI_COPY_DONE
 *  -------------------------------------------------------------------------
 */
void mi_copy_done(queue_t *q, mblk_t *mp, int err)
{
	union ioctypes *ioc;

	ensure(mp, return);
	ensure(q && mp->b_wptr >= mp->b_rptr + sizeof(*ioc), goto error);
	ioc = (typeof(ioc)) mp->b_rptr;
	switch (mp->b_datap->db_type) {
	case M_IOCDATA:
		if (ioc->copyresp.cp_private)
			freemsg(xchg(&ioc->copyresp.cp_private, NULL));
		break;
	case M_IOCTL:
		break;
	default:
		swerr();
		freemsg(mp);
		break;
	}
	mp->b_datap->db_type = err ? M_IOCNAK : M_IOCACK;
	ioc->iocblk.ioc_error = (err < 0) ? -err : err;
	ioc->iocblk.ioc_rval = err ? -1 : ioc->iocblk.ioc_rval;
	qreply(q, mp);
}

EXPORT_SYMBOL(mi_copy_done);	/* mps/ddi.h */

/*
 *  MI_COPYIN
 *  -------------------------------------------------------------------------
 */
void mi_copyin(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mip;
	mblk_t *bp;
	int err = EPROTO;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		if (ioc->iocblk.ioc_count == TRANSPARENT) {
			/* for transparent ioctl perform a copy in */
			if ((bp = xchg(&mp->b_cont, NULL))) {
				mip = (typeof(mip)) bp->b_rptr;
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_private = bp;
				ioc->copyreq.cq_size = len;
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_addr = mip->mi_uaddr;
				mip->mi_cnt = 1;
				mip->mi_dir = MI_COPY_IN;
				if (mp->b_cont)
					freemsg(xchg(&mp->b_cont, NULL));
				qreply(q, mp);
				return;
			}
		} else {
			/* for a non-transparent ioctl the first copyin is already performed for
			   us, fake out an M_IOCDATA response. */
			if ((bp = allocb(sizeof(*mip), BPRI_MED))) {
				mip = (typeof(mip)) bp->b_rptr;
				mp->b_datap->db_type = M_IOCDATA;
				ioc->copyresp.cp_private = bp;
				mip->mi_cnt = 1;
				mip->mi_dir = MI_COPY_IN;
				putq(q, mp);
				return;
			}
			err = ENOSR;
		}
		break;
	case M_IOCDATA:
		/* for a subsequent M_IOCDATA, we should already have our private message block
		   stacked up against the private pointer */
		if ((bp = xchg(&ioc->copyresp.cp_private, NULL))) {
			mip = (typeof(mip)) bp->b_rptr;
			mp->b_datap->db_type = M_COPYIN;
			ioc->copyreq.cq_private = bp;
			ioc->copyreq.cq_size = len;
			ioc->copyreq.cq_flag = 0;
			ioc->copyreq.cq_addr = uaddr;
			mip->mi_uaddr = uaddr;
			mip->mi_cnt += 1;
			mip->mi_dir = MI_COPY_IN;
			if (mp->b_cont)
				freemsg(xchg(&mp->b_cont, NULL));
			qreply(q, mp);
			return;
		}
		break;
	}
	mi_copy_done(q, mp, err);
}

EXPORT_SYMBOL(mi_copyin);	/* mps/ddi.h */

/*
 *  MI_COPYIN_N
 *  -------------------------------------------------------------------------
 */
void mi_copyin_n(queue_t *q, mblk_t *mp, size_t offset, size_t len)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mip;
	mblk_t *bp;
	int err = EPROTO;

	if (mp->b_datap->db_type == M_IOCDATA) {
		if ((bp = ioc->copyresp.cp_private)) {
			mip = (typeof(mip)) mp->b_rptr;
			if (mip->mi_dir == MI_COPY_IN) {
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_private = bp;
				ioc->copyreq.cq_size = len;
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_addr = mip->mi_uaddr + offset;
				mip->mi_cnt++;
				mip->mi_dir = MI_COPY_IN;
				if (mp->b_cont)
					freemsg(xchg(&mp->b_cont, NULL));
				qreply(q, mp);
				return;
			}
		}
	}
	mi_copy_done(q, mp, err);
}

EXPORT_SYMBOL(mi_copyin_n);	/* mps/ddi.h */

/*
 *  MI_COPYOUT_ALLOC
 *  -------------------------------------------------------------------------
 */
mblk_t *mi_copyout_alloc(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len, int free_on_error)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mip;
	mblk_t *db, *bp;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		if (ioc->iocblk.ioc_count == TRANSPARENT) {
			bp = xchg(&mp->b_cont, NULL);
		} else
			bp = allocb(sizeof(*mip), BPRI_MED);
		if (!bp) {
			if (free_on_error)
				mi_copy_done(q, mp, ENOSR);
			return (bp);
		}
		bp->b_wptr = bp->b_rptr + sizeof(*mip);
		mip = (typeof(mip)) bp->b_rptr;
		if (ioc->iocblk.ioc_count == TRANSPARENT && !uaddr)
			uaddr = mip->mi_uaddr;
		mip->mi_cnt = 0;
		mip->mi_dir = MI_COPY_OUT;
		mp->b_datap->db_type = M_IOCDATA;	/* for next pass */
		ioc->copyresp.cp_private = bp;
		ioc->copyresp.cp_rval = 0;
		if (mp->b_cont)
			freemsg(xchg(&mp->b_cont, NULL));
		/* fall through */
	case M_IOCDATA:
		if (!(db = allocb(len, BPRI_MED))) {
			if (free_on_error)
				mi_copy_done(q, mp, ENOSR);
			return (db);
		}
		linkb(ioc->copyresp.cp_private, db);
		db->b_next = (mblk_t *) uaddr;
		return (db);
	}
	if (free_on_error)
		mi_copy_done(q, mp, EFAULT);
	return (NULL);
}

EXPORT_SYMBOL(mi_copyout_alloc);	/* mps/ddi.h */

/*
 *  MI_COPYOUT
 *  -------------------------------------------------------------------------
 */
void mi_copyout(queue_t *q, mblk_t *mp)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mip;
	mblk_t *bp, *db;

	if (mp->b_datap->db_type != M_IOCDATA || !mp->b_cont || !(bp = ioc->copyresp.cp_private))
		return mi_copy_done(q, mp, EPROTO);
	if (ioc->copyresp.cp_rval || !(db = bp->b_cont))
		return mi_copy_done(q, mp, (int) (intptr_t) ioc->copyresp.cp_rval);
	bp->b_cont = xchg(&db->b_cont, NULL);
	mip = (typeof(mip)) bp->b_rptr;
	mp->b_datap->db_type = M_COPYOUT;
	ioc->copyreq.cq_private = bp;
	ioc->copyreq.cq_size = (db->b_wptr > db->b_rptr) ? db->b_wptr - db->b_rptr : 0;
	ioc->copyreq.cq_flag = 0;
	ioc->copyreq.cq_addr = (caddr_t) xchg(&db->b_next, NULL);
	mip->mi_cnt = (mip->mi_dir == MI_COPY_IN) ? 1 : mip->mi_cnt + 1;
	mip->mi_dir = MI_COPY_OUT;
	if (mp->b_cont)
		freemsg(xchg(&mp->b_cont, NULL));
	mp->b_cont = db;
	qreply(q, mp);
}

EXPORT_SYMBOL(mi_copyout);	/* mps/ddi.h */

/*
 *  MI_COPY_STATE
 *  -------------------------------------------------------------------------
 */
int mi_copy_state(queue_t *q, mblk_t *mp, mblk_t **mpp)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	struct mi_iocblk *mip;
	mblk_t *bp, *db = NULL;
	int err = EPROTO;

	if (mp->b_datap->db_type != M_IOCDATA || !(db = mp->b_cont)
	    || !(bp = ioc->copyresp.cp_private))
		goto error;
	if ((err = (int) (intptr_t) ioc->copyresp.cp_rval))
		goto error;
	switch (mp->b_datap->db_type) {
	case M_IOCDATA:
		err = ENOMEM;
		if (!pullupmsg(db, -1))
			goto error;
		mip = (typeof(mip)) bp->b_rptr;
		if (mip->mi_dir == MI_COPY_IN)
			*mpp = db;
		return MI_COPY_CASE(mip->mi_dir, mip->mi_cnt);
	case M_IOCTL:
		err = EPROTO;
		goto error;
	}
      error:
	mi_copy_done(q, mp, err);
	return (-1);
}

EXPORT_SYMBOL(mi_copy_state);	/* mps/ddi.h */

/*
 *  MI_COPY_SET_RVAL
 *  -------------------------------------------------------------------------
 */
void mi_copy_set_rval(mblk_t *mp, int rval)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;

	ioc->iocblk.ioc_rval = rval;
}

EXPORT_SYMBOL(mi_copy_set_rval);

/*
 *  =========================================================================
 *
 *  Timeout helper functions
 *
 *  =========================================================================
 */

#define TB_ZOMBIE	-2	/* tb is a zombie, waiting to be freed */
#define TB_CANCELLED	-1	/* tb has been cancelled - but is still on queue */
#define TB_IDLE		 0	/* tb is idle */
#define TB_ACTIVE	 1	/* tb has timer actively running */
#define TB_RESCHEDULED	 2	/* tb is being rescheduled */
#define TB_TRANSIENT	 3	/* tb is in a transient state */

typedef struct mi_timeb {
	long tb_state;
	toid_t tb_tid;
	clock_t tb_time;
	mblk_t *tb_mp;
	queue_t *tb_q;
} tblk_t;

#undef mi_timer_alloc
#undef mi_timer

static void mi_timer_expiry(caddr_t data)
{
	tblk_t *tb = (typeof(tb)) data;
	toid_t tid;

	if ((tid = xchg(&tb->tb_tid, 0))) {
		if (tb->tb_q && tb->tb_mp)
			put(tb->tb_q, tb->tb_mp);
		else
			swerr();
	}
}

/*
 *  MacOT variety
 */
/*
 *  MI_TIMER_ALLOC (MacOT variety)
 *  -------------------------------------------------------------------------
 */
mblk_t *mi_timer_alloc_MAC(queue_t *q, size_t size)
{
	mblk_t *mp;
	tblk_t *tb;

	if ((mp = allocb(sizeof(*tb) + size, BPRI_HI))) {
		mp->b_datap->db_type = M_PCSIG;
		tb = (typeof(tb)) mp->b_rptr;
		mp->b_rptr = (typeof(mp->b_rptr)) (tb + 1);
		mp->b_wptr = mp->b_rptr + size;
		tb->tb_state = TB_IDLE;
		tb->tb_tid = (toid_t) (0);
		tb->tb_time = (clock_t) (0);
		tb->tb_mp = mp;
		tb->tb_q = q;
	}
	return (mp);
}

EXPORT_SYMBOL(mi_timer_alloc_MAC);

/*
 *  MI_TIMER (MacOT variety)
 *  -------------------------------------------------------------------------
 */
void mi_timer_SUN(queue_t *q, mblk_t *mp, clock_t msec);
void mi_timer_MAC(mblk_t *mp, clock_t msec)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;

	return mi_timer_SUN(tb->tb_q, mp, msec);
}

EXPORT_SYMBOL(mi_timer_MAC);

/*
 *  MI_TIMER_CANCEL (MacOT variety)
 *  -------------------------------------------------------------------------
 */
int mi_timer_cancel(mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	long state;

	switch ((state = xchg(&tb->tb_state, TB_TRANSIENT))) {
	case TB_ACTIVE:
	{
		toid_t tid;

		if (!(tid = xchg(&tb->tb_tid, 0))) {
			tb->tb_state = TB_CANCELLED;
			return (-1);
		}
		untimeout(tid);
		tb->tb_state = TB_IDLE;
		return (0);
	}
	case TB_RESCHEDULED:
		tb->tb_state = TB_CANCELLED;
		return (-1);
	case TB_CANCELLED:
	case TB_TRANSIENT:
	case TB_ZOMBIE:
	case TB_IDLE:
	default:
		tb->tb_state = state;
		swerr();
		return (-1);
	}
}

EXPORT_SYMBOL(mi_timer_cancel);

/*
 *  MI_TIMER_Q_SWITCH (MacOT variety)
 *  -------------------------------------------------------------------------
 */
mblk_t *mi_timer_q_switch(mblk_t *mp, queue_t *q, mblk_t *new_mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	tblk_t *t2 = (typeof(t2)) new_mp->b_datap->db_base;

	if (!mi_timer_cancel(mp)) {
		mi_timer_SUN(q, new_mp, drv_hztomsec(tb->tb_time - jiffies));
		return (new_mp);
	}
	tb->tb_state = TB_IDLE;
	mi_timer_SUN(q, mp, drv_hztomsec(tb->tb_time - jiffies));
	return (mp);
}

EXPORT_SYMBOL(mi_timer_q_switch);

/*
 *  OpenSolaris variety
 */
/*
 *  MI_TIMER_ALLOC (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
mblk_t *mi_timer_alloc_SUN(size_t size)
{
	return mi_timer_alloc_MAC(NULL, size);
}

EXPORT_SYMBOL(mi_timer_alloc_SUN);

/*
 *  MI_TIMER (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
void mi_timer_SUN(queue_t *q, mblk_t *mp, clock_t msec)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;

	if (msec >= 0) {
		long state;

		tb->tb_q = q;
		switch ((state = xchg(&tb->tb_state, TB_TRANSIENT))) {
		case TB_ACTIVE:
			/* tb has timer actively running */
		{
			toid_t tid;

			if (!(tid = xchg(&tb->tb_tid, 0)))
				goto reschedule;
			untimeout(tid);
		}
			/* fall through */
		case TB_IDLE:
			/* tb is idle */
			tb->tb_state = TB_ACTIVE;
			tb->tb_time = jiffies + drv_msectohz(msec);
			if (!(tb->tb_tid = timeout((timo_fcn_t *) mi_timer_expiry,
						   (caddr_t) tb, tb->tb_time - jiffies))) {
				/* failed timeout allocation */
				if (tb->tb_q) {
					tb->tb_state = TB_RESCHEDULED;
					put(tb->tb_q, mp);
					break;
				}
				swerr();
				tb->tb_state = TB_IDLE;
				break;
			}
			break;
		case TB_CANCELLED:
			/* tb has been cancelled - but is still on queue */
		      reschedule:
			/* fall through */
		case TB_RESCHEDULED:
			/* tb is being rescheduled */
			tb->tb_state = TB_RESCHEDULED;
			tb->tb_time = jiffies + drv_msectohz(msec);
			break;
		case TB_TRANSIENT:
			/* tb is in a transient state */
		case TB_ZOMBIE:
			/* tb is a zombie, waiting to be freed */
		default:
			tb->tb_state = state;
			swerr();
			break;
		}
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

EXPORT_SYMBOL(mi_timer_SUN);

/*
 *  MI_TIMER_STOP (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
void mi_timer_stop(mblk_t *mp)
{
	mi_timer_cancel(mp);
}

EXPORT_SYMBOL(mi_timer_stop);

/*
 *  MI_TIMER_MOVE (OpenSolaris variety)
 *  -------------------------------------------------------------------------
 */
void mi_timer_move(queue_t *q, mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	long state;

	switch ((state = xchg(&tb->tb_state, TB_TRANSIENT))) {
	case TB_ACTIVE:
	{
		toid_t tid;

		if (!(tid = xchg(&tb->tb_tid, 0)))
			goto dequeue;
		untimeout(tid);
		tb->tb_q = q;
		if (!(tb->tb_tid = timeout((timo_fcn_t *) mi_timer_expiry,
					   (caddr_t) tb, tb->tb_time - jiffies))) {
			/* failed timeout allocation */
			if (q) {
				tb->tb_state = TB_RESCHEDULED;
				put(q, mp);
			} else {
				tb->tb_state = TB_IDLE;
				swerr();
			}
		}
		break;
	}
	default:
	{
		queue_t *oldq;

	      dequeue:
		/* double race breaker */
		if ((oldq = xchg(&tb->tb_q, NULL))) {
			/* did we make it to a queue */
			if (mp->b_next || mp->b_prev) {
				fixme(("too dangerous"));	/* FIXME: too dangerous */
				rmvq(oldq, mp);
			}
		}
		if ((tb->tb_q = q))
			put(q, mp);
		else
			swerr();
		break;
	}
	case TB_ZOMBIE:
		swerr();
		break;
	case TB_IDLE:
		tb->tb_q = q;
		break;
	}
	tb->tb_state = state;
}

EXPORT_SYMBOL(mi_timer_move);

/*
 *  Common
 */
/*
 *  MI_TIMER_VALID (Common)
 *  -------------------------------------------------------------------------
 */
int mi_timer_valid(mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	long state;

	switch ((state = xchg(&tb->tb_state, TB_TRANSIENT))) {
	case TB_ACTIVE:
	default:
		tb->tb_state = TB_IDLE;
		return (1);
	case TB_ZOMBIE:
		freemsg(mp);
		return (0);
	case TB_CANCELLED:
		tb->tb_state = TB_IDLE;
		return (0);
	case TB_RESCHEDULED:
		tb->tb_state = TB_ACTIVE;
		if (!(tb->tb_tid = timeout((timo_fcn_t *) mi_timer_expiry,
					   (caddr_t) tb, tb->tb_time - jiffies))) {
			/* failed timeout allocation */
			tb->tb_state = TB_RESCHEDULED;
			if (tb->tb_q) {
				put(tb->tb_q, mp);
				return (0);
			}
			swerr();
			tb->tb_state = TB_IDLE;
			return (1);
		}
		return (0);
	}
}

EXPORT_SYMBOL(mi_timer_valid);

/*
 *  MI_TIMER_FREE (Common)
 *  -------------------------------------------------------------------------
 */
void mi_timer_free(mblk_t *mp)
{
	tblk_t *tb = (typeof(tb)) mp->b_datap->db_base;
	long state;

	switch ((state = xchg(&tb->tb_state, TB_TRANSIENT))) {
	case TB_ACTIVE:
	{
		toid_t tid;

		if (!(tid = xchg(&tb->tb_tid, 0)))
			goto zombie;
		untimeout(tid);
	}
	case TB_IDLE:
		break;
	default:
	      zombie:
		tb->tb_state = TB_ZOMBIE;
		return;
	}
	freemsg(mp);
}

EXPORT_SYMBOL(mi_timer_free);

queue_t *mi_allocq(struct streamtab *st)
{
#if LIS
	return lis_allocq(st->st_rdinit->qi_minfo->mi_idname);
#endif
#if LFS
	queue_t *q;

	if ((q = allocq()))
		setq(q, st->st_rdinit, st->st_wrinit);
	return (q);
#endif
}

EXPORT_SYMBOL(mi_allocq);

void mi_freeq(queue_t *q)
{
#if LIS
	lis_freeq(q);
#endif
#if LFS
	freeq(q);
#endif
}

EXPORT_SYMBOL(mi_freeq);

int mi_strlog(queue_t *q, char level, ushort flags, char *fmt, ...)
{
	int result = 0;

	if (vstrlog != NULL) {
		va_list args;
		modID_t mid = q->q_qinfo->qi_minfo->mi_idnum;
		minor_t sid = 0;	/* FIXME - should be minor devce number */

		va_start(args, fmt);
		result = vstrlog(mid, sid, level, flags, fmt, args);
		va_end(args);
	}
	return (result);
}

EXPORT_SYMBOL(mi_strlog);

#define _MI_FLAG_ZEROPAD	(1<<0)
#define _MI_FLAG_SIGN		(1<<1)
#define _MI_FLAG_PLUS		(1<<2)
#define _MI_FLAG_SPACE		(1<<3)
#define _MI_FLAG_LEFT		(1<<4)
#define _MI_FLAG_SPECIAL	(1<<5)
#define _MI_FLAG_LARGE		(1<<6)

static int mi_putc(char c, mblk_t **mpp)
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

static int mi_number(mblk_t **mpp, long long num, int base, int width, int decimal, int flags)
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
		if (num < 0) {
			sign = '-';
			num = -num;
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

static int mi_vmpprintf(mblk_t *mp, char *fmt, va_list args)
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

int mi_mpprintf(mblk_t *mp, char *fmt, ...)
{
	va_list args;
	int count = -1;

	va_start(args, fmt);
	if (mp)
		count = mi_vmpprintf(mp, fmt, args);
	va_end(args);
	return (count);
}

EXPORT_SYMBOL(mi_mpprintf);

int mi_mpprintf_nr(mblk_t *mp, char *fmt, ...)
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

EXPORT_SYMBOL(mi_mpprintf_nr);

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
int mi_set_sth_hiwat(queue_t *q, size_t size)
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

EXPORT_SYMBOL(mi_set_sth_hiwat);

/*
 *  MI_SET_STH_LOWAT
 *  -------------------------------------------------------------------------
 */
int mi_set_sth_lowat(queue_t *q, size_t size)
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

EXPORT_SYMBOL(mi_set_sth_lowat);

/*
 *  MI_SET_STH_MAXBLK
 *  -------------------------------------------------------------------------
 */
int mi_set_sth_maxblk(queue_t *q, ssize_t size)
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

EXPORT_SYMBOL(mi_set_sth_maxblk);

/*
 *  MI_SET_STH_COPYOPT
 *  -------------------------------------------------------------------------
 */
int mi_set_sth_copyopt(queue_t *q, int copyopt)
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

EXPORT_SYMBOL(mi_set_sth_copyopt);

/*
 *  MI_SET_STH_WROFF
 *  -------------------------------------------------------------------------
 */
int mi_set_sth_wroff(queue_t *q, size_t size)
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

EXPORT_SYMBOL(mi_set_sth_wroff);

/*
 *  =========================================================================
 *
 *  System wrapper functions
 *
 *  =========================================================================
 */
int mi_sprintf(char *buf, char *fmt, ...)
{
	int result;
	va_list args;

	va_start(args, fmt);
	result = vsprintf(buf, fmt, args);
	va_end(args);
	return result;
}

EXPORT_SYMBOL(mi_sprintf);

int mi_strcmp(const caddr_t cp1, const caddr_t cp2)
{
	return strcmp(cp1, cp2);
}

EXPORT_SYMBOL(mi_strcmp);

int mi_strlen(const caddr_t str)
{
	return strlen(str);
}

EXPORT_SYMBOL(mi_strlen);

long mi_strtol(const caddr_t str, caddr_t *ptr, int base)
{
	return simple_strtol(str, ptr, base);
}

EXPORT_SYMBOL(mi_strtol);

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
uint8_t *mi_offset_param(mblk_t *mp, size_t offset, size_t len)
{
	if (mp || len == 0) {
		size_t blen = mp->b_wptr > mp->b_rptr ? mp->b_wptr - mp->b_rptr : 0;

		if (blen >= offset + len)
			return (mp->b_rptr + offset);
	}
	return (NULL);
}

EXPORT_SYMBOL(mi_offset_param);

/*
 *  MI_OFFSET_PARAMC
 *  -------------------------------------------------------------------------
 */
uint8_t *mi_offset_paramc(mblk_t *mp, size_t offset, size_t len)
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

EXPORT_SYMBOL(mi_offset_paramc);

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
void mps_become_writer(queue_t *q, mblk_t *mp, proc_ptr_t proc)
{
#if LIS
	lis_flags_t flags;

	LIS_QISRLOCK(q, &flags);
	(*proc) (q, mp);
	LIS_QISRUNLOCK(q, &flags);
#endif
#if LFS
	extern void __strwrit(queue_t *q, mblk_t *mp, void (*func)(queue_t *, mblk_t *), int perim);
	__strwrit(q, mp, proc, PERIM_INNER|PERIM_OUTER);
#endif
}

EXPORT_SYMBOL(mps_become_writer);

/*
 *  MPS_INTR_DISABLE
 *  -------------------------------------------------------------------------
 */
void mps_intr_disable(pl_t * plp)
{
	unsigned long flags = *plp;

	local_irq_save(flags);
	*plp = flags;
}

EXPORT_SYMBOL(mps_intr_disable);

/*
 *  MPS_INTR_ENABLE
 *  -------------------------------------------------------------------------
 */
void mps_intr_enable(pl_t pl)
{
	unsigned long flags = pl;

	local_irq_restore(flags);
}

EXPORT_SYMBOL(mps_intr_enable);

#ifdef CONFIG_STREAMS_COMPAT_MPS_MODULE
static
#endif
int __init mpscomp_init(void)
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
void __exit mpscomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_MPS_MODULE
module_init(mpscomp_init);
module_exit(mpscomp_exit);
#endif
