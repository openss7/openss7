/*****************************************************************************

 @(#) echo.c,v (0.9.2.2) 2003/10/21 21:50:08

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

 Last Modified 2003/10/21 21:50:08 by brian

 *****************************************************************************/

#ident "@(#) echo.c,v (0.9.2.2) 2003/10/21 21:50:08"

static char const ident[] = "echo.c,v (0.9.2.2) 2003/10/21 21:50:08";

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

#include "sys/config.h"

#include "strdebug.h"
#include "strreg.h"		/* for struct str_args */
#include "strsched.h"		/* for ap_get/ap_put */

#define ECHO_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define ECHO_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define ECHO_REVISION	"LfS echo.c,v (0.9.2.2) 2003/10/21 21:50:08"
#define ECHO_DEVICE	"SVR 4.2 STREAMS Echo (ECHO) Device"
#define ECHO_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ECHO_LICENSE	"GPL"
#define ECHO_BANNER	ECHO_DESCRIP	"\n" \
			ECHO_COPYRIGHT	"\n" \
			ECHO_REVISION	"\n" \
			ECHO_DEVICE	"\n" \
			ECHO_CONTACT	"\n"
#define ECHO_SPLASH	ECHO_DEVICE	" - " \
			ECHO_REVISION	"\n"

MODULE_AUTHOR(ECHO_CONTACT);
MODULE_DESCRIPTION(ECHO_DESCRIP);
MODULE_SUPPORTED_DEVICE(ECHO_DEVICE);
MODULE_LICENSE(ECHO_LICENSE);

#ifndef CONFIG_STREAMS_ECHO_NAME
//#define CONFIG_STREAMS_ECHO_NAME "echo"
#error "CONFIG_STREAMS_ECHO_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_ECHO_MODID
//#define CONFIG_STREAMS_ECHO_MODID 2
#error "CONFIG_STREAMS_ECHO_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_ECHO_MAJOR
//#define CONFIG_STREAMS_ECHO_MAJOR 0
#error "CONFIG_STREAMS_ECHO_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_ECHO_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for ECHO driver. (0 for auto allocation)");

static struct module_info echo_minfo = {
	mi_idnum:CONFIG_STREAMS_ECHO_MODID,
	mi_idname:CONFIG_STREAMS_ECHO_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static int echo_put(queue_t *q, mblk_t *mp)
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
	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_RSE:
	case M_PCRSE:
		qreply(q, mp);
		return (0);
	default:
		freemsg(mp);
		return (0);
	}
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

typedef struct echo {
	struct echo *next;
	struct echo **prev;
	dev_t dev;
} echo_t;

static spinlock_t echo_lock = SPIN_LOCK_UNLOCKED;
static struct echo *echo_list = NULL;

static int echo_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct echo *p, **pp = &echo_list;
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
		spin_lock(&echo_lock);
		for (; *pp && (dmajor = getmajor((*pp)->dev)) < cmajor; pp = &(*pp)->next) ;
		for (; *pp && dmajor == getmajor((*pp)->dev) &&
		     cminor == getminor(makedevice(cmajor, cminor)); pp = &(*pp)->next, cminor++) {
			int dminor = getminor((*pp)->dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor && sflag != CLONEOPEN) {
				spin_unlock(&echo_lock);
				kmem_free(p, sizeof(*p));
				return (EIO);	/* bad error */
			}
		}
		if (cminor != getminor(makedevice(cmajor, cminor))) {
			spin_unlock(&echo_lock);
			kmem_free(p, sizeof(*p));
			return (EBUSY);	/* no minors left */
		}
		p->dev = *devp = makedevice(cmajor, cminor);
		if ((p->next = *pp))
			p->next->prev = &p->next;
		p->prev = pp;
		*pp = p;
		q->q_ptr = OTHERQ(q)->q_ptr = p;
		spin_unlock(&echo_lock);
		return (0);
	}
	}
	return (ENXIO);
}
static int echo_close(queue_t *q, int oflag, cred_t *crp)
{
	struct echo *p;
	if ((p = q->q_ptr) == NULL)
		return (0);	/* already closed */
	spin_lock(&echo_lock);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	spin_unlock(&echo_lock);
	return (0);
}

static struct qinit echo_qinit = {
	qi_putp:echo_put,
	qi_qopen:echo_open,
	qi_qclose:echo_close,
	qi_minfo:&echo_minfo,
};

static struct streamtab echo_info = {
	st_rdinit:&echo_qinit,
	st_wrinit:&echo_qinit,
};

static struct cdevsw echo_cdev = {
	d_name:CONFIG_STREAMS_ECHO_NAME,
	d_str:&echo_info,
	d_flag:0,
	d_fop:NULL,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

static int __init echo_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO ECHO_BANNER);
#else
	printk(KERN_INFO ECHO_SPLASH);
#endif
	if ((err = register_strdev(makedevice(major, 0), &echo_cdev)) < 0)
		return (err);
	if (err > 0)
		major = err;
	return (0);
};
static void __exit echo_exit(void)
{
	unregister_strdev(makedevice(major, 0), &echo_cdev);
};

module_init(echo_init);
module_exit(echo_exit);
