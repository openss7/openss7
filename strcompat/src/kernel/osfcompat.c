/*****************************************************************************

 @(#) osfcompat.c,v (1.1.2.6) 2003/10/28 08:00:04

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified 2003/10/28 08:00:04 by brian

 *****************************************************************************/

#ident "@(#) osfcompat.c,v (1.1.2.6) 2003/10/28 08:00:04"

static char const ident[] =
    "osfcompat.c,v (1.1.2.6) 2003/10/28 08:00:04";

#include <linux/config.h>
#include <linux/module.h>	/* for MOD_DEC_USE_COUNT etc */

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

#define __OSF_EXTERN_INLINE inline

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

#define _OSF_SOURCE
#include <linux/kmem.h>		/* for SVR4 style kmalloc functions */
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strconf.h>
#include <linux/strsubr.h>
#include <linux/ddi.h>

#include "strdebug.h"
#include "strsched.h"
#include "strutil.h"
#include "strhead.h"
#include "strsad.h"

#define OSFCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define OSFCOMP_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define OSFCOMP_REVISION	"LfS osfcompat.c,v (1.1.2.6) 2003/10/28 08:00:04"
#define OSFCOMP_DEVICE		"OSF/1.2 Compatibility"
#define OSFCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define OSFCOMP_LICENSE		"GPL"
#define OSFCOMP_BANNER		OSFCOMP_DESCRIP		"\n" \
				OSFCOMP_COPYRIGHT	"\n" \
				OSFCOMP_REVISION	"\n" \
				OSFCOMP_DEVICE		"\n" \
				OSFCOMP_CONTACT		"\n"
#define OSFCOMP_SPLASH		OSFCOMP_DEVICE		" - " \
				OSFCOMP_REVISION	"\n"

MODULE_AUTHOR(OSFCOMP_CONTACT);
MODULE_DESCRIPTION(OSFCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(OSFCOMP_DEVICE);
MODULE_LICENSE(OSFCOMP_LICENSE);

__OSF_EXTERN_INLINE void puthere(queue_t *q, mblk_t *mp);
__OSF_EXTERN_INLINE time_t lbolt(void);
__OSF_EXTERN_INLINE time_t time(void);

struct str_comm {
	struct str_comm **prev;		/* must be first */
	struct str_comm *next;
	dev_t dev;			/* device number (or NODEV for modules) */
	size_t size;			/* size of private data */
	char priv[0];			/* followed by private data */
};

static spinlock_t str_list_lock = SPIN_LOCK_UNLOCKED;
static struct str_comm *str_list = NULL;

/* 
 *  STREAMS_OPEN_COMM
 *  -------------------------------------------------------------------------
 *  for V4 open
 */
int streams_open_comm(unsigned int size, queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct str_comm *sp, **spp = &str_list;
	int cmajor = getmajor(*devp);
	int cminor = getminor(*devp);
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN) {
		if (!WR(q)->q_next)
			return (EIO);
	} else {
		if (WR(q)->q_next && SAMESTR(q))
			return (EIO);
	}
	if ((sp = kmem_alloc(sizeof(*sp) + size, KM_NOSLEEP)))	/* we could probably sleep */
		return (ENOMEM);
	bzero(sp, sizeof(*sp) + size);
	spin_lock(&str_list_lock);
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
		int dmajor = cmajor;
		for (; *(spp) && (dmajor = getmajor((*spp)->dev)) < cmajor; spp = &(*spp)->next) ;
		for (; *(spp) && dmajor == getmajor((*spp)->dev) &&
		     cminor == getminor(makedevice(0, cminor)); spp = &(*spp)->next, cminor++) {
			int dminor = getminor((*spp)->dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor)
				if (sflag != CLONEOPEN) {
					spin_unlock(&str_list_lock);
					kmem_free(sp, sizeof(*sp) + size);
					return (ENXIO);
				}
		}
		if (cminor != getminor(makedevice(0, cminor))) {	/* no minors left */
			spin_unlock(&str_list_lock);
			kmem_free(sp, sizeof(*sp) + size);
			return (EBUSY);
		}
		sp->dev = makedevice(cmajor, cminor);
		break;
	}
	case MODOPEN:
	{
		/* just push modules on list with no device */
		sp->dev = NODEV;
		break;
	}
	}
	sp->size = size;
	if ((sp->next = *spp))
		sp->next->prev = &sp;
	sp->prev = spp;
	*spp = sp;
	q->q_ptr = OTHERQ(q)->q_ptr = (sp + 1);
	spin_unlock(&str_list_lock);
	return (0);
}

/* 
 *  STREAMS_OPEN_OCOMM
 *  -------------------------------------------------------------------------
 *  for V3 open
 */
int streams_open_ocomm(dev_t dev, unsigned int size, queue_t *q, dev_t *devp, int oflag, int sflag,
		       cred_t *crp)
{
	dev_t mydev = dev;
	int err;
	if ((err = streams_open_comm(size, q, &mydev, oflag, sflag, crp)))
		return (err);
	*devp = mydev;
	return (0);
}

/* 
 *  STREAMS_CLOSE_COMM
 *  -------------------------------------------------------------------------
 *  for both V3 and V4 close
 */
int streams_close_comm(queue_t *q, int oflag, cred_t *crp)
{
	spin_lock(&str_list_lock);
	if (q->q_ptr) {
		struct str_comm *sp = ((struct str_comm *) q->q_ptr) - 1;
		size_t size = sp->size;
		/* found it */
		if ((*sp->prev = sp->next))
			sp->next->prev = sp->prev;
		sp->next = NULL;
		sp->prev = &sp->next;
		sp->size = 0;
		q->q_ptr = OTHERQ(q)->q_ptr = NULL;
		kmem_free(sp, sizeof(*sp) + size);
	}
	spin_unlock(&str_list_lock);
	return (0);
}

static int __init osfcomp_init(void)
{
#ifdef MODULE
	printk(KERN_INFO OSFCOMP_BANNER);
#else
	printk(KERN_INFO OSFCOMP_SPLASH);
#endif
	return (0);
}
static void __exit osfcomp_exit(void)
{
	return;
}

module_init(osfcomp_init);
module_exit(osfcomp_exit);
