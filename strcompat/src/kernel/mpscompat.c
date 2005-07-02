/*****************************************************************************

 @(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/07/01 20:16:30 $

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

 Last Modified $Date: 2005/07/01 20:16:30 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mpscompat.c,v $
 Revision 0.9.2.1  2005/07/01 20:16:30  brian
 - added and updated manpages for some Mentat compatibility

 *****************************************************************************/

#ident "@(#) $RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/07/01 20:16:30 $"

static char const ident[] = "$RCSfile: mpscompat.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/07/01 20:16:30 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>	/* for MOD_DEC_USE_COUNT etc */
#include <linux/init.h>

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

#define __MI_EXTERN_INLINE inline

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

#define _MI_SOURCE
#include <sys/kmem.h>		/* for SVR4 style kmalloc functions */
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/kernel/strsched.h"
#include "src/kernel/strutil.h"
#include "src/modules/sth.h"
#include "src/kernel/strreg.h"
#include "src/kernel/strsad.h"

#define MICOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MICOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define MICOMP_REVISION		"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.1 $) $Date: 2005/07/01 20:16:30 $"
#define MICOMP_DEVICE		"Mentat Compatibility"
#define MICOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MICOMP_LICENSE		"GPL"
#define MICOMP_BANNER		MICOMP_DESCRIP		"\n" \
				MICOMP_COPYRIGHT	"\n" \
				MICOMP_REVISION	"\n" \
				MICOMP_DEVICE		"\n" \
				MICOMP_CONTACT		"\n"
#define MICOMP_SPLASH		MICOMP_DEVICE		" - " \
				MICOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_MI_MODULE
MODULE_AUTHOR(MICOMP_CONTACT);
MODULE_DESCRIPTION(MICOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(MICOMP_DEVICE);
MODULE_LICENSE(MICOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-micompat");
#endif
#endif

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
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */
_MI_EXTERN_INLINE void *mi_alloc(size_t size, unsigned int pri);
EXPORT_SYMBOL(mi_alloc);

/*
 *  MI_ALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
_MI_EXTERN_INLINE void *mi_alloc_sleep(size_t size, unsigned int pri);
EXPORT_SYMBOL(mi_alloc_sleep);

/*
 *  MI_ZALLOC
 *  -------------------------------------------------------------------------
 */
_MI_EXTERN_INLINE caddr_t mi_zalloc(size_t size);
EXPORT_SYMBOL(mi_alloc);

/*
 *  MI_ZALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
_MI_EXTERN_INLINE caddr_t mi_zalloc_sleep(size_t size);
EXPORT_SYMBOL(mi_alloc_sleep);

/*
 *  MI_FREE
 *  -------------------------------------------------------------------------
 */
_MI_EXTERN_INLINE void mi_free(void *ptr);
EXPORT_SYMBOL(mi_free);

/*
 *  MI_OPEN_ALLOC
 *  -------------------------------------------------------------------------
 */
caddr_t mi_open_alloc(size_t size)
{
	return kmem_zalloc(sizeof(struct mi_comm) + size, KM_NOSLEEP);
}
EXPORT_SYMBOL(mi_open_alloc);

/*
 *  MI_OPEN_ALLOC_SLEEP
 *  -------------------------------------------------------------------------
 */
caddr_t mi_open_alloc_sleep(size_t size)
{
	return kmem_zalloc(sizeof(struct mi_comm) + size, KM_SLEEP);
}
EXPORT_SYMBOL(mi_open_alloc_sleep);

#define MI_COPY_IN			1
#define MI_COPY_OUT			2
#define MI_COPY_CASE(_dir, _cnt)	(((cnt)<<2|dir))
#define MI_COPY_DIRECTION(_mp)		(*(int *)&(_mp)->b_cont->b_next)
#define MI_COPY_COUNT(_mp)		(*(int *)&(_mp)->b_cont->b_prev)
#define MI_COPY_STATE(_mp)		MI_COPY_CASE(MI_COPY_DIRECTION(_mp), MI_COPY_COUNT(_mp))
/*
 *  MI_COPYIN
 *  -------------------------------------------------------------------------
 */
void mi_copyin(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	if (ioc->iocblk.ioc_count == TRANSPARENT) {
		/* for transparent ioctl perform a copy in */
		mp->b_datap->db_type = M_COPYIN;
		ioc->copyreq.cq_private = xchg(&mp->b_cont, NULL);
		ioc->copyreq.cq_size = len;
		ioc->copyreq.cq_flag = 0;
		ioc->copyreq.cq_addr = mp->b_cont->b_rptr;
		MI_COPY_DIRECTION(mp) = MI_COPY_IN;
		MI_COPY_COUNT(mp) = 1;
		qreply(q, mp);
		return;
	} else {
		mblk_t *db;
		if ((db = allocb(0, BPRI_MED))) {
			/* for a non-transparent ioctl the first copyin is already performed for
			   us, fake out an M_IOCDATA response. */
			mp->b_datap->db_type = M_IOCDATA;
			ioc->copyresp.cp_private = db;
			db = xchg(&mp->b_cont, db);
			MI_COPY_DIRECTION(mp) = MI_COPY_IN;
			MI_COPY_COUNT(mp) = 1;
			db = xchg(&mp->b_cont, db);
			putq(q, mp);
			return;
		}
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_error = ENOSTR;
		ioc->iocblk.ioc_count = 0;
		if (mp->b_cont)
			freemsg(xchg(&mp->b_cont, NULL));
		qreply(q, mp);
		return;
	}
}
EXPORT_SYMBOL(mi_copyin);

/*
 *  MI_COPYOUT_ALLOC
 *  -------------------------------------------------------------------------
 */
mblk_t *mi_copyout_alloc(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len, int free_on_error)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *db;
	if (mp->b_datap->db_type == M_IOCTL) {
		if (ioc->iocblk.ioc_count != TRANSPARENT) {
			mblk_t *bp;
			if (!(bp = allocb(0, BPRI_MED))) {
				if (free_on_eror) {
					mp->b_datap->db_type = M_IOCNAK;
					ioc->iocblk.ioc_error = ENOSTR;
					ioc->ioc_count = 0;
					if (mp->b_cont)
						freemsg(xchg(&mp->b_cont, 0));
					qreply(q, mp);
				}
				return (bp);
			}
			bp->b_cont = xchg(&mp->b_cont, bp);
		}
		mp->b_datap->db_type = M_IOCDATA;
		ioc->copyresp.cp_rval = 0;
		MI_COPY_COUNT(mp) = 0;
		MI_COPY_DIRECTION(mp) = MI_COPY_OUT;
	}
	if (!(db = allocb(len, BRPI_MED))) {
		if (free_on_error)
			mi_copy_done(q, mp, ENOSTR);
		return (db);
	}
	linkb(mp, db);
	db->b_next = (mblk_t *) uaddr;
	return (db);
}
EXPORT_SYMBOL(mi_copyout_alloc);

/*
 *  MI_COPYOUT
 *  -------------------------------------------------------------------------
 */
void mi_copyout(queue_t *q, mblk_t *mp)
{
}
EXPORT_SYMBOL(mi_copyout);

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */

/*
 *  MI_ALLOC
 *  -------------------------------------------------------------------------
 */
