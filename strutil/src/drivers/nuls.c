/*****************************************************************************

 @(#) nuls.c,v (1.1.2.2) 2003/10/21 21:50:19

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

 Last Modified 2003/10/21 21:50:19 by brian

 *****************************************************************************/

#ident "@(#) nuls.c,v (1.1.2.2) 2003/10/21 21:50:19"

static char const ident[] = "nuls.c,v (1.1.2.2) 2003/10/21 21:50:19";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/kmem.h>
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strreg.h"		/* for struct str_args */
#include "strsched.h"		/* for ap_get/ap_put */

#include "sys/config.h"

#define NULS_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NULS_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define NULS_REVISION	"LfS nuls.c,v (1.1.2.2) 2003/10/21 21:50:19"
#define NULS_DEVICE	"SVR 4.2 STREAMS Null Stream (NULS) Device"
#define NULS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NULS_LICENSE	"GPL"
#define NULS_BANNER	NULS_DESCRIP	"\n" \
			NULS_COPYRIGHT	"\n" \
			NULS_REVISION	"\n" \
			NULS_DEVICE	"\n" \
			NULS_CONTACT	"\n"
#define NULS_SPLASH	NULS_DEVICE	" - " \
			NULS_REVISION	"\n"

MODULE_AUTHOR(NULS_CONTACT);
MODULE_DESCRIPTION(NULS_DESCRIP);
MODULE_SUPPORTED_DEVICE(NULS_DEVICE);
MODULE_LICENSE(NULS_LICENSE);

#ifndef CONFIG_STREAMS_NULS_NAME
//#define CONFIG_STREAMS_NULS_NAME "nuls"
#error "CONFIG_STREAMS_NULS_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_NULS_MODID
//#define CONFIG_STREAMS_NULS_MODID 3
#error "CONFIG_STREAMS_NULS_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_NULS_MAJOR
//#define CONFIG_STREAMS_NULS_MAJOR 0
#error "CONFIG_STREAMS_NULS_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_NULS_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for NULS driver. (0 for auto allocation)");

static struct module_info nuls_minfo = {
	mi_idnum:CONFIG_STREAMS_NULS_MODID,
	mi_idname:CONFIG_STREAMS_NULS_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static int nuls_put(queue_t *q, mblk_t *mp)
{
	int err = 0;
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			queue_t *rq = RD(q);
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(rq, mp->b_rptr[1], FLUSHALL);
			else
				flushq(rq, FLUSHALL);
			qreply(q, mp);
			return (0);
		}
		break;
	case M_IOCTL:
		err = -EOPNOTSUPP;
		goto nak;
	case M_IOCDATA:
		err = -EINVAL;
		goto nak;
	}
	freemsg(mp);
	return (0);
      nak:
	{
		union ioctypes *ioc;
		mp->b_datap->db_type = M_IOCNAK;
		ioc = (typeof(ioc)) mp->b_rptr;
		ioc->iocblk.ioc_rval = -1;
		ioc->iocblk.ioc_error = -err;
		qreply(q, mp);
		return (0);
	}
}

typedef struct nuls {
	struct nuls *next;
	struct nuls **prev;
	dev_t dev;
} nuls_t;

static spinlock_t nuls_lock = SPIN_LOCK_UNLOCKED;
static struct nuls *nuls_list = NULL;

static int nuls_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct nuls *p, **pp = &nuls_list;
	int cmajor, cminor;
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (ENXIO);	/* can't open as module */
	cmajor = getmajor(*devp);
	cminor = getminor(*devp);
	if ((p = kmem_alloc(sizeof(*p), KM_NOSLEEP)))	/* we could sleep */
		return (ENOMEM);	/* no memory */
	bzero(p, sizeof(*p));
	switch (sflag) {
	case CLONEOPEN:
		if (cminor < 1)
			cminor = 1;
	case DRVOPEN:
	{
		int dmajor = cmajor;
		if (cminor < 1)
			return (ENXIO);
		spin_lock(&nuls_lock);
		for (; *pp && (dmajor = getmajor((*pp)->dev)) < cmajor; pp = &(*pp)->next) ;
		for (; *pp && dmajor == getmajor((*pp)->dev) &&
		     cminor == getminor(makedevice(cmajor, cminor)); pp = &(*pp)->next, cminor++) {
			int dminor = getminor((*pp)->dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor && sflag != CLONEOPEN) {
				spin_unlock(&nuls_lock);
				kmem_free(p, sizeof(*p));
				return (EIO);	/* bad error */
			}
		}
		if (cminor != getminor(makedevice(cmajor, cminor))) {
			spin_unlock(&nuls_lock);
			kmem_free(p, sizeof(*p));
			return (EBUSY);	/* no minors left */
		}
		p->dev = *devp = makedevice(cmajor, cminor);
		if ((p->next = *pp))
			p->next->prev = &p->next;
		p->prev = pp;
		*pp = p;
		q->q_ptr = OTHERQ(q)->q_ptr = p;
		spin_unlock(&nuls_lock);
		return (0);
	}
	}
	return (ENXIO);
}

static int nuls_close(queue_t *q, int oflag, cred_t *crp)
{
	struct nuls *p;
	if ((p = q->q_ptr) == NULL)
		return (0);	/* already closed */
	spin_lock(&nuls_lock);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	spin_unlock(&nuls_lock);
	return (0);
}

static struct qinit nuls_qinit = {
	qi_putp:nuls_put,
	qi_qopen:nuls_open,
	qi_qclose:nuls_close,
	qi_minfo:&nuls_minfo,
};

static struct streamtab nuls_info = {
	st_rdinit:&nuls_qinit,
	st_wrinit:&nuls_qinit,
};

static struct cdevsw nuls_cdev = {
	d_name:CONFIG_STREAMS_NULS_NAME,
	d_str:&nuls_info,
	d_flag:0,
	d_fop:NULL,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

static int __init nuls_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO NULS_BANNER);
#else
	printk(KERN_INFO NULS_SPLASH);
#endif
	if ((err = register_strdev(makedevice(major, 0), &nuls_cdev)) < 0)
		return (err);
	if (err > 0)
		major = err;
	return (0);
};
static void __exit nuls_exit(void)
{
	unregister_strdev(makedevice(major, 0), &nuls_cdev);
};

module_init(nuls_init);
module_exit(nuls_exit);
