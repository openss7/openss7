/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"

#define NULS_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NULS_COPYRIGHT	"Copyright (c) 2008-2009  Monavacon Limited.  All Rights Reserved."
#define NULS_REVISION	"LfS $RCSfile$ $Name$($Revision$) $Date$"
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

#ifdef CONFIG_STREAMS_NULS_MODULE
MODULE_AUTHOR(NULS_CONTACT);
MODULE_DESCRIPTION(NULS_DESCRIP);
MODULE_SUPPORTED_DEVICE(NULS_DEVICE);
MODULE_LICENSE(NULS_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_NULS_MODULE */

#ifndef CONFIG_STREAMS_NULS_NAME
#define CONFIG_STREAMS_NULS_NAME "nuls"
#endif
#ifndef CONFIG_STREAMS_NULS_MODID
#error CONFIG_STREAMS_NULS_MODID must be defined.
#endif
#ifndef CONFIG_STREAMS_NULS_MAJOR
#error CONFIG_STREAMS_NULS_MAJOR must be defined.
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-nuls");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_NULS_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_NULS_MODID;

#ifdef CONFIG_STREAMS_NULS_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module id number for NULS driver. (0 for auto allocation)");
#endif				/* CONFIG_STREAMS_NULS_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NULS_MODID));
MODULE_ALIAS("streams-driver-nuls");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_NULS_MODULE
static
#endif
major_t major = CONFIG_STREAMS_NULS_MAJOR;

#ifdef CONFIG_STREAMS_NULS_MODULE
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for NULS driver. (0 for auto allocation)");
#endif				/* CONFIG_STREAMS_NULS_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_NULS_MAJOR) "-*");
MODULE_ALIAS("/dev/nuls");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NULS_MAJOR));
MODULE_ALIAS("/dev/streams/nuls");
MODULE_ALIAS("/dev/streams/nuls/*");
MODULE_ALIAS("/dev/streams/clone/nuls");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

static struct module_info nuls_minfo = {
	.mi_idnum = CONFIG_STREAMS_NULS_MODID,
	.mi_idname = CONFIG_STREAMS_NULS_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat nuls_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat nuls_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int
nuls_put(queue_t *q, mblk_t *mp)
{
	int err = 0;

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			queue_t *rq = RD(q);

			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(rq, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(rq, FLUSHDATA);
			qreply(q, mp);
			return (0);
		}
		break;
	case M_IOCTL:
		err = -EINVAL;
		goto nak;
	case M_IOCDATA:
		err = -EINVAL;
		goto nak;
	case M_READ:
		mp->b_wptr = mp->b_rptr;
		mp->b_datap->db_type = M_DATA;
		mp->b_flag |= MSGDELIM;
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
		ioc->iocblk.ioc_count = 0;
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

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static streamscall int
nuls_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct nuls *p, **pp = &nuls_list;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		return (ENXIO);	/* can't open as module */
	}
	if (!(p = kmem_alloc(sizeof(*p), KM_NOSLEEP))) {	/* we could sleep */
		return (ENOMEM);	/* no memory */
	}
	bzero(p, sizeof(*p));
	switch (sflag) {
	case CLONEOPEN:
		if (cminor < 1)
			cminor = 1;
	case DRVOPEN:
	{
		major_t dmajor = cmajor;

		if (cminor < 1) {
			return (ENXIO);
		}
		spin_lock(&nuls_lock);
		for (; *pp && (dmajor = getmajor((*pp)->dev)) < cmajor; pp = &(*pp)->next) ;
		for (; *pp && dmajor == getmajor((*pp)->dev) &&
		     getminor(makedevice(cmajor, cminor)) != 0; pp = &(*pp)->next) {
			minor_t dminor = getminor((*pp)->dev);

			if (cminor < dminor)
				break;
			if (cminor == dminor) {
				if (sflag == CLONEOPEN)
					cminor++;
				else {
					spin_unlock(&nuls_lock);
					kmem_free(p, sizeof(*p));
					return (EIO);	/* bad error */
				}
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {	/* no minors left */
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
		qprocson(q);
		return (0);
	}
	}
	return (ENXIO);
}

static streamscall int
nuls_close(queue_t *q, int oflag, cred_t *crp)
{
	struct nuls *p;

	if ((p = q->q_ptr) == NULL) {
		return (0);	/* already closed */
	}
	qprocsoff(q);
	spin_lock(&nuls_lock);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	spin_unlock(&nuls_lock);
	return (0);
}

static struct qinit nuls_rqinit = {
	.qi_putp = NULL,
	.qi_qopen = nuls_open,
	.qi_qclose = nuls_close,
	.qi_minfo = &nuls_minfo,
	.qi_mstat = &nuls_rstat,
};

static struct qinit nuls_wqinit = {
	.qi_putp = nuls_put,
	.qi_srvp = NULL,
	.qi_minfo = &nuls_minfo,
	.qi_mstat = &nuls_wstat,
};

#ifdef CONFIG_STREAMS_NULS_MODULE
static
#endif
struct streamtab nulsinfo = {
	.st_rdinit = &nuls_rqinit,
	.st_wrinit = &nuls_wqinit,
};

static struct cdevsw nuls_cdev = {
	.d_name = CONFIG_STREAMS_NULS_NAME,
	.d_str = &nulsinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_NULS_MODULE
static
#endif
int __init
nulsinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_NULS_MODULE
	printk(KERN_INFO NULS_BANNER);
#else
	printk(KERN_INFO NULS_SPLASH);
#endif
	nuls_minfo.mi_idnum = modid;
	if ((err = register_strdev(&nuls_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_NULS_MODULE
static
#endif
void __exit
nulsexit(void)
{
	unregister_strdev(&nuls_cdev, major);
};

#ifdef CONFIG_STREAMS_NULS_MODULE
module_init(nulsinit);
module_exit(nulsexit);
#endif
