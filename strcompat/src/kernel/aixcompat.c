/*****************************************************************************

 @(#) $RCSfile: aixcompat.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/04/21 01:54:07 $

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

 Last Modified $Date: 2005/04/21 01:54:07 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: aixcompat.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/04/21 01:54:07 $"

static char const ident[] =
    "$RCSfile: aixcompat.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/04/21 01:54:07 $";

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

#define __AIX_EXTERN_INLINE inline

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

#define _AIX_SOURCE
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

#define AIXCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define AIXCOMP_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define AIXCOMP_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/04/21 01:54:07 $"
#define AIXCOMP_DEVICE		"AIX 5L Version 5.1 Compatibility"
#define AIXCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define AIXCOMP_LICENSE		"GPL"
#define AIXCOMP_BANNER		AIXCOMP_DESCRIP		"\n" \
				AIXCOMP_COPYRIGHT	"\n" \
				AIXCOMP_REVISION	"\n" \
				AIXCOMP_DEVICE		"\n" \
				AIXCOMP_CONTACT		"\n"
#define AIXCOMP_SPLASH		AIXCOMP_DEVICE		" - " \
				AIXCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
MODULE_AUTHOR(AIXCOMP_CONTACT);
MODULE_DESCRIPTION(AIXCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(AIXCOMP_DEVICE);
MODULE_LICENSE(AIXCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-aixcompat");
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
 *  MI_BUFCALL
 *  -------------------------------------------------------------------------
 */
__AIX_EXTERN_INLINE void mi_bufcall(queue_t *q, int size, int priority);
EXPORT_SYMBOL(mi_bufcall);

static spinlock_t mi_list_lock = SPIN_LOCK_UNLOCKED;
/* 
 *  MI_OPEN_COMM
 *  -------------------------------------------------------------------------
 */
int mi_open_comm(caddr_t *mi_list, uint size, queue_t *q, dev_t *devp, int flag, int sflag,
		 cred_t *credp)
{
	struct mi_comm *mi, **mip = (struct mi_comm **) mi_list;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN) {
		if (!WR(q)->q_next)
			return (EIO);
	} else {
		if (WR(q)->q_next && SAMESTR(q))
			return (EIO);
	}
	if ((mi = kmem_alloc(sizeof(*mi) + size, KM_NOSLEEP)))	/* we could probably sleep */
		return (EAGAIN);
	bzero(mi, sizeof(*mi) + size);
	spin_lock(&mi_list_lock);
	switch (sflag) {
	case CLONEOPEN:
		/* first clone minor (above 5 per AIX docs), but the caller can start wherever they 
		   want above that */
		if (cminor <= 5)
			cminor = 5 + 1;
		/* fall through */
	default:
	case DRVOPEN:
	{
		major_t dmajor = cmajor;
		for (; *(mip) && (dmajor = getmajor((*mip - 1)->mi_dev)) < cmajor;
		     mip = &(*mip - 1)->mi_next) ;
		for (; *(mip) && dmajor == getmajor((*mip - 1)->mi_dev) &&
		     getminor(makedevice(0, cminor)) != 0; mip = &(*mip - 1)->mi_next, cminor++) {
			minor_t dminor = getminor((*mip - 1)->mi_dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor)
				if (sflag != CLONEOPEN) {
					spin_unlock(&mi_list_lock);
					kmem_free(mi, sizeof(*mi) + size);
					return (ENXIO);
				}
		}
		if (getminor(makedevice(0, cminor)) == 0) {	/* no minors left */
			spin_unlock(&mi_list_lock);
			kmem_free(mi, sizeof(*mi) + size);
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
	mi->mi_size = sizeof(*mi) + size;
	if ((mi->mi_next = *mip))
		mi->mi_next->mi_prev = &mi;
	mi->mi_prev = mip;
	*mip = mi + 1;
	mi->mi_head = (struct mi_comm **) mi_list;
	q->q_ptr = OTHERQ(q)->q_ptr = mi + 1;
	spin_unlock(&mi_list_lock);
	return (0);
}

EXPORT_SYMBOL(mi_open_comm);	/* aixddi.h */

/* 
 *  MI_CLOSE_COMM
 *  -------------------------------------------------------------------------
 */
int mi_close_comm(caddr_t *mi_list, queue_t *q)
{
	spin_lock(&mi_list_lock);
	if (q->q_ptr) {
		struct mi_comm *mi = ((struct mi_comm *) q->q_ptr) - 1;
		if (mi->mi_head == (struct mi_comm **) mi_list) {
			size_t size = mi->mi_size;
			/* found it */
			if ((*mi->mi_prev = mi->mi_next))
				mi->mi_next->mi_prev = mi->mi_prev;
			mi->mi_next = NULL;
			mi->mi_prev = &mi->mi_next;
			mi->mi_head = NULL;
			mi->mi_size = 0;
			q->q_ptr = OTHERQ(q)->q_ptr = NULL;
			kmem_free(mi, size);
		}
	}
	spin_unlock(&mi_list_lock);
	return (0);
}

EXPORT_SYMBOL(mi_close_comm);	/* aixddi.h */

/* 
 *  MI_NEXT_PTR
 *  -------------------------------------------------------------------------
 */
caddr_t mi_next_ptr(caddr_t priv)
{
	struct mi_comm *mi = ((struct mi_comm *) priv) - 1;
	return ((caddr_t) mi->mi_next);
}

EXPORT_SYMBOL(mi_next_ptr);	/* aixddi.h */

/* 
 *  MI_PREV_PTR
 *  -------------------------------------------------------------------------
 *  Linux Fast-STREAMS embellishment.
 */
caddr_t mi_prev_ptr(caddr_t priv)
{
	struct mi_comm *mi = ((struct mi_comm *) priv) - 1;
	if (mi && mi->mi_prev != mi->mi_head)
		return ((caddr_t) (((struct mi_comm *) mi->mi_prev) + 1));
	return (NULL);
}

EXPORT_SYMBOL(mi_prev_ptr);	/* aixddi.h */

/* 
 *  WANTIO
 *  -------------------------------------------------------------------------
 */
int wantio(queue_t *q, struct wantio *w)
{
	queue_t *wq = WR(q);
	struct stdata *sd = ((struct queinfo *) (wq - 1))->qu_str;
	if (w) {
		qget(wq);
		xchg(&sd->sd_directio, w);
	} else {
		xchg(&sd->sd_directio, w);
		qput(&wq);
	}
	return (0);
}

EXPORT_SYMBOL(wantio);		/* aixddi.h */

/* 
 *  WANTMSG
 *  -------------------------------------------------------------------------
 */
__AIX_EXTERN_INLINE int wantmsg(queue_t *q, int (*func) (mblk_t *));
EXPORT_SYMBOL(wantmsg);		/* aixddi.h */

/* 
 *  STR_INSTALL
 *  -------------------------------------------------------------------------
 */
EXPORT_SYMBOL(str_install_AIX);	/* strconf.h */
int str_install_AIX(int cmd, strconf_t * sc)
{
	if (!sc)
		return (EINVAL);
	switch (cmd) {
	case STR_LOAD_DEV:
	{
		struct cdevsw *cdev;
		int err;
#ifdef MAX_CHRDEV
		if (0 >= sc->sc_major || sc->sc_major >= MAX_CHRDEV)
#else
		if (sc->sc_major != MAJOR(MKDEV(sc->sc_major, 0)))
#endif
			return (EINVAL);
		/* We don't do old-style opens */
		if (!(sc->sc_open_stylesc_flags & STR_NEW_OPEN))
			return (ENOSYS);
		if (sc->sc_open_stylesc_flags & STR_Q_NOTTOSPEC) {
			/* Modules can always sleep because we are running at soft IRQ. */
		}
		if (!(cdev = kmem_zalloc(sizeof(*cdev), KM_NOSLEEP)))
			return (ENOMEM);
		cdev->d_name = sc->sc_name;
		cdev->d_str = sc->sc_str;
		/* build flags */
		cdev->d_flag = 0;
		if (sc->sc_open_stylesc_flags & STR_MPSAFE) {
			cdev->d_flag |= D_MP;
		}
		if (sc->sc_open_stylesc_flags & STR_QSAFETY) {
			cdev->d_flag |= D_SAFE;
		}
		if (sc->sc_open_stylesc_flags & STR_PERSTREAM) {
			cdev->d_flag |= D_UP;
		}
		if (sc->sc_open_stylesc_flags & STR_NEWCLONING) {
			cdev->d_flag |= D_CLONE;
		}
		switch ((cdev->d_sqlvl = sc->sc_sqlevel)) {
		case SQLVL_NOP:
			cdev->d_flag |= D_MP;
			break;
		case SQLVL_QUEUE:
			cdev->d_flag |= D_MTPERQ;
			break;
		case SQLVL_QUEUEPAIR:
			cdev->d_flag |= D_MTQPAIR;
			break;
		case SQLVL_MODULE:
			cdev->d_flag |= D_MTPERMOD;
			break;
		case SQLVL_ELSEWHERE:
			cdev->d_flag |= D_MTOUTPERIM;
			break;
		case SQLVL_GLOBAL:
			/* can't really support this, but its only used for debug anyway */
			cdev->d_flag &= ~D_MP;
			break;
		case SQLVL_DEFAULT:
			cdev->d_flag |= D_MTPERMOD;
			break;
		}
		if ((err = register_strdev(cdev, sc->sc_major)) < 0)
			kmem_free(cdev, sizeof(*cdev));
		return (-err);
	}
	case STR_UNLOAD_DEV:
	{
		struct cdevsw *cdev;
		int err;
		if (0 >= sc->sc_major || sc->sc_major >= MAX_STRDEV)
			return (EINVAL);
		if ((cdev = cdev_get(sc->sc_major)) == NULL)
			return (ENOENT);
		printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
		printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
		cdev_put(cdev);
		if ((err = unregister_strdev(cdev, sc->sc_major)) == 0)
			kmem_free(cdev, sizeof(*cdev));
		return (-err);
	}
	case STR_LOAD_MOD:
	{
		struct fmodsw *fmod;
		int err;
		if (!sc->sc_str || !sc->sc_str->st_rdinit || !sc->sc_str->st_rdinit->qi_minfo)
			return (EINVAL);
		if (0 >= sc->sc_major || sc->sc_major >= MAX_STRDEV)
			return (EINVAL);
		/* We don't do old-style opens */
		if (!(sc->sc_open_stylesc_flags & STR_NEW_OPEN))
			return (ENOSYS);
		if (!(fmod = kmem_zalloc(sizeof(*fmod), KM_NOSLEEP)))
			return (ENOMEM);
		fmod->f_name = sc->sc_name;
		fmod->f_str = sc->sc_str;
		/* build flags */
		fmod->f_flag = 0;
		if (sc->sc_open_stylesc_flags & STR_MPSAFE) {
			fmod->f_flag |= D_MP;
		}
		if (sc->sc_open_stylesc_flags & STR_PERSTREAM) {
			fmod->f_flag |= D_UP;
		}
		if (sc->sc_open_stylesc_flags & STR_Q_NOTTOSPEC) {
			/* Modules can always sleep because we are running at soft IRQ; however, it 
			   has a horrendous impact.  We just ignore it. */
		}
		if (sc->sc_open_stylesc_flags & STR_QSAFETY) {
			fmod->f_flag |= D_SAFE;
		}
		if (sc->sc_open_stylesc_flags & STR_NEWCLONING) {
			fmod->f_flag |= D_CLONE;
		}
		switch ((fmod->f_sqlvl = sc->sc_sqlevel)) {
		case SQLVL_NOP:
			fmod->f_flag |= D_MP;
			break;
		case SQLVL_QUEUE:
			fmod->f_flag |= D_MTPERQ;
			break;
		case SQLVL_QUEUEPAIR:
			fmod->f_flag |= D_MTQPAIR;
			break;
		case SQLVL_MODULE:
			fmod->f_flag |= D_MTPERMOD;
			break;
		case SQLVL_ELSEWHERE:
			fmod->f_flag |= D_MTOUTPERIM;
			break;
		case SQLVL_GLOBAL:
			/* can't really support this, but its only used for debug anyway */
			fmod->f_flag &= ~D_MP;
			break;
		case SQLVL_DEFAULT:
			fmod->f_flag |= D_MTPERMOD;
			break;
		}
		if ((err = register_strmod(fmod)) < 0)
			kmem_free(fmod, sizeof(*fmod));
		return (-err);
	}
	case STR_UNLOAD_MOD:
	{
		struct fmodsw *fmod;
		int err;
		if (!(fmod = fmod_str(sc->sc_str)))
			return (ENOENT);
		if ((err = unregister_strmod(fmod)) == 0)
			kmem_free(fmod, sizeof(fmod));
		return (-err);
	}
	}
	return (EINVAL);
}

#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
static
#endif
int __init aixcomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
	printk(KERN_INFO AIXCOMP_BANNER);
#else
	printk(KERN_INFO AIXCOMP_SPLASH);
#endif
	return (0);
}
#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
static
#endif
void __exit aixcomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_AIX_MODULE
module_init(aixcomp_init);
module_exit(aixcomp_exit);
#endif
