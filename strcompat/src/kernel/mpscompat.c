/*****************************************************************************

 @(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/07/05 22:46:05 $

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

 Last Modified $Date: 2005/07/05 22:46:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mpscompat.c,v $
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

#ident "@(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/07/05 22:46:05 $"

static char const ident[] =
    "$RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/07/05 22:46:05 $";

#if 0
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>	/* for MOD_DEC_USE_COUNT etc */
#include <linux/init.h>
#endif

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

#if 0
#include <linux/kernel.h>	/* for vsprintf and friends */
#include <linux/vmalloc.h>	/* for vmalloc */
#ifdef CONFIG_PCI
#include <linux/pci.h>		/* for many pci functions */
#include <asm/pci.h>		/* for many pci functions */
#endif
#include <linux/ioport.h>	/* for check_region and friends */
#include <asm/uaccess.h>	/* for verify_area and friends */
#include <linux/timer.h>	/* for del_timer and friends */
#include <asm/semaphore.h>	/* for semaphores */
#include <linux/sched.h>	/* for kill_proc, jiffies and friends */
#include <linux/kmod.h>		/* for request_module and friends */
#include <linux/threads.h>	/* for NR_CPUS */
#include <asm/dma.h>		/* for request_dma and friends */
#include <linux/fs.h>		/* for filesystem related stuff */
#include <linux/time.h>		/* for do_gettimeofday and friends */
#include <asm/io.h>		/* for virt_to_page and friends */
#include <linux/slab.h>		/* for kmalloc and friends */
#include <asm/irq.h>		/* for disable_irq */
#include <asm/system.h>		/* for sti, cli */
#include <asm/delay.h>		/* for udelay */
#include <linux/spinlock.h>	/* for spinlock functions */
#include <asm/atomic.h>		/* for atomic functions */
#include <linux/poll.h>		/* for poll_table */
#include <linux/string.h>
#endif

#ifdef LIS
#define _LFS_SOURCE
#endif
#define _MPS_SOURCE

#include "os7/compat.h"

#if 0
#include <sys/kmem.h>		/* for SVR4 style kmalloc functions */
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#endif

#include <sys/mpsddi.h>

#if LFS
//#include "sys/config.h"
#include "src/kernel/strsched.h"
#include "src/kernel/strutil.h"
//#include "src/modules/sth.h"
#include "src/kernel/strreg.h"
#include "src/kernel/strsad.h"
#else
#if 0
#include "include/sys/strdebug.h"
#endif
#endif

#define MPSCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MPSCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define MPSCOMP_REVISION	"LfS $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/07/05 22:46:05 $"
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
MODULE_ALIAS("streams-micompat");
#endif
#endif

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void *mi_alloc(size_t size, unsigned int pri);
EXPORT_SYMBOL(mi_alloc);	/* mpsddi.h */

/*
 *  MI_ALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void *mi_alloc_sleep(size_t size, unsigned int pri);
EXPORT_SYMBOL(mi_alloc_sleep);	/* mpsddi.h */

/*
 *  MI_ZALLOC
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE caddr_t mi_zalloc(size_t size);
EXPORT_SYMBOL(mi_zalloc);	/* mpsddi.h */

/*
 *  MI_ZALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE caddr_t mi_zalloc_sleep(size_t size);
EXPORT_SYMBOL(mi_zalloc_sleep);	/* mpsddi.h */

/*
 *  MI_FREE
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_free(void *ptr);
EXPORT_SYMBOL(mi_free);		/* mpsddi.h */

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

EXPORT_SYMBOL(mi_open_alloc);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_open_alloc_sleep);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_first_ptr);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_first_dev_ptr);	/* mpsddi.h */

/*
 *  MI_NEXT_PTR
 *  -------------------------------------------------------------------------
 */
caddr_t mi_next_ptr(caddr_t ptr)
{
	struct mi_comm *mi = ptr ? (((struct mi_comm *) ptr) - 1)->mi_next : NULL;
	return mi ? ((caddr_t) (mi + 1)) : NULL;
}

EXPORT_SYMBOL(mi_next_ptr);	/* mpsddi.h, aixddi.h */

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

EXPORT_SYMBOL(mi_next_dev_ptr);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_prev_ptr);	/* mpsddi.h, aixddi.h */

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

EXPORT_SYMBOL(mi_open_link);	/* mpsddi.h */

/*
 *  MI_OPEN_DETACHED
 *  -------------------------------------------------------------------------
 */
caddr_t mi_open_detached(caddr_t *mi_head, size_t size, dev_t *devp);
EXPORT_SYMBOL(mi_open_detached);	/* mpsddi.h */

/*
 *  MI_ATTACH
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_attach(queue_t *q, caddr_t ptr);
EXPORT_SYMBOL(mi_attach);	/* mpsddi.h, macddi.h */

/* 
 *  MI_OPEN_COMM
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE int mi_open_comm(caddr_t *mi_head, uint size, queue_t *q, dev_t *devp, int flag,
				     int sflag, cred_t *credp);
EXPORT_SYMBOL(mi_open_comm);	/* mpsddi.h, aixddi.h */

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

EXPORT_SYMBOL(mi_close_unlink);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_close_free);	/* mpsddi.h */

/*
 *  MI_DETACH
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_detach(queue_t *q, caddr_t ptr);
EXPORT_SYMBOL(mi_detach);	/* mpsddi.h */

/*
 *  MI_CLOSE_DETACHED
 *  -------------------------------------------------------------------------
 */
__MPS_EXTERN_INLINE void mi_close_detached(caddr_t *mi_head, caddr_t ptr);
EXPORT_SYMBOL(mi_close_detached);	/* mpsddi.h */

/* 
 *  MI_CLOSE_COMM
 *  -------------------------------------------------------------------------
 */
int mi_close_comm(caddr_t *mi_head, queue_t *q);
EXPORT_SYMBOL(mi_close_comm);	/* mpsddi.h, aixddi.h */

struct mi_iocblk {
	caddr_t mi_uaddr;
	short mi_dir;
	short mi_cnt;
};

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

EXPORT_SYMBOL(mi_copy_done);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_copyin);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_copyin_n);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_copyout_alloc);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_copyout);	/* mpsddi.h */

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

EXPORT_SYMBOL(mi_copy_state);	/* mpsddi.h */

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
