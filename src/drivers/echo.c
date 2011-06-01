/*****************************************************************************

 @(#) $RCSfile: echo.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-05-31 09:46:03 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-05-31 09:46:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: echo.c,v $
 Revision 1.1.2.4  2011-05-31 09:46:03  brian
 - new distros

 Revision 1.1.2.3  2010-11-28 14:21:32  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.2  2009-06-29 07:35:43  brian
 - SVR 4.2 => SVR 4.2 MP

 Revision 1.1.2.1  2009-06-21 11:20:45  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: echo.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-05-31 09:46:03 $";

#ifdef NEED_LINUX_AUTOCONF_H
#include <linux/autoconf.h>
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"

#define ECHO_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define ECHO_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define ECHO_REVISION	"LfS $RCSfile: echo.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-05-31 09:46:03 $"
#define ECHO_DEVICE	"SVR 4.2 MP STREAMS Echo (ECHO) Device"
#define ECHO_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ECHO_LICENSE	"GPL"
#define ECHO_BANNER	ECHO_DESCRIP	"\n" \
			ECHO_COPYRIGHT	"\n" \
			ECHO_REVISION	"\n" \
			ECHO_DEVICE	"\n" \
			ECHO_CONTACT	"\n"
#define ECHO_SPLASH	ECHO_DEVICE	" - " \
			ECHO_REVISION	"\n"

#ifdef CONFIG_STREAMS_ECHO_MODULE
MODULE_AUTHOR(ECHO_CONTACT);
MODULE_DESCRIPTION(ECHO_DESCRIP);
MODULE_SUPPORTED_DEVICE(ECHO_DEVICE);
MODULE_LICENSE(ECHO_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif
#endif

#ifndef CONFIG_STREAMS_ECHO_NAME
#define CONFIG_STREAMS_ECHO_NAME "echo"
#endif
#ifndef CONFIG_STREAMS_ECHO_MODID
#error CONFIG_STREAMS_ECHO_MODID must be defined.
#endif
#ifndef CONFIG_STREAMS_ECHO_MAJOR
#error CONFIG_STREAMS_ECHO_MAJOR must be defined.
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-echo");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_ECHO_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_ECHO_MODID;

#ifdef CONFIG_STREAMS_ECHO_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module id number for ECHO driver. (0 for auto allocation)");
#endif				/* CONFIG_STREAMS_ECHO_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_ECHO_MODID));
MODULE_ALIAS("streams-driver-echo");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_ECHO_MODULE
static
#endif
major_t major = CONFIG_STREAMS_ECHO_MAJOR;

#ifdef CONFIG_STREAMS_ECHO_MODULE
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for ECHO driver. (0 for auto allocation)");
#endif				/* CONFIG_STREAMS_ECHO_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_ECHO_MAJOR) "-*");
MODULE_ALIAS("/dev/echo");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_ECHO_MAJOR));
MODULE_ALIAS("/dev/streams/echo");
MODULE_ALIAS("/dev/streams/echo/*");
MODULE_ALIAS("/dev/streams/clone/echo");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

static struct module_info echo_minfo = {
	.mi_idnum = CONFIG_STREAMS_ECHO_MODID,
	.mi_idname = CONFIG_STREAMS_ECHO_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat echo_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat echo_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int
echo_rput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static streamscall int
echo_rsrv(queue_t *q)
{
	qenable(OTHERQ(q));
	return (0);
}

static streamscall int
echo_wput(queue_t *q, mblk_t *mp)
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
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
			else
				flushq(RD(q), FLUSHDATA);
			qreply(q, mp);
			/* never makes it here */
			return (0);
		}
		break;
	case M_IOCTL:
	case M_IOCDATA:
	{
		union ioctypes *ioc;

		err = -EINVAL;

		mp->b_datap->db_type = M_IOCNAK;
		ioc = (typeof(ioc)) mp->b_rptr;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_rval = -1;
		ioc->iocblk.ioc_error = -err;
		qreply(q, mp);
		return (0);
	}
	case M_READ:
		mp->b_wptr = mp->b_rptr;
		mp->b_datap->db_type = M_DATA;
		mp->b_flag |= MSGDELIM;
		qreply(q, mp);
		return (0);
	case M_DATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_RSE:
	case M_PCRSE:
		/* Check QSVCBUSY flag in MP drivers to avoid missequencing of messages when
		   service procedure is running concurrent with put procedure. */
		if (mp->b_datap->db_type < QPCTL && (q->q_first || (q->q_flag & QSVCBUSY)
						     || !bcanputnext(OTHERQ(q), mp->b_band)))
			putq(q, mp);
		else
			qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static streamscall int
echo_wsrv(queue_t *q)
{
	mblk_t *mp;
	queue_t *rq = RD(q);

	while ((mp = getq(q))) {
		if (bcanputnext(rq, mp->b_band)) {
			putnext(rq, mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	return (0);
}

typedef struct echo {
	struct echo *next;
	struct echo **prev;
	dev_t dev;
} echo_t;

static spinlock_t echo_lock = SPIN_LOCK_UNLOCKED;
static struct echo *echo_list = NULL;

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static streamscall int
echo_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct echo *p, **pp = &echo_list;
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
		spin_lock(&echo_lock);
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
					spin_unlock(&echo_lock);
					kmem_free(p, sizeof(*p));
					return (EIO);	/* bad error */
				}
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {	/* no minors left */
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
		qprocson(q);
		return (0);
	}
	}
	return (ENXIO);
}

static streamscall int
echo_close(queue_t *q, int oflag, cred_t *crp)
{
	struct echo *p;

	if ((p = q->q_ptr) == NULL) {
		return (0);	/* already closed */
	}
	qprocsoff(q);
	spin_lock(&echo_lock);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	spin_unlock(&echo_lock);
	return (0);
}

static struct qinit echo_rqinit = {
	.qi_putp = echo_rput,
	.qi_srvp = echo_rsrv,
	.qi_qopen = echo_open,
	.qi_qclose = echo_close,
	.qi_minfo = &echo_minfo,
	.qi_mstat = &echo_rstat,
};

static struct qinit echo_wqinit = {
	.qi_putp = echo_wput,
	.qi_srvp = echo_wsrv,
	.qi_minfo = &echo_minfo,
	.qi_mstat = &echo_wstat,
};

#ifdef CONFIG_STREAMS_ECHO_MODULE
static
#endif
struct streamtab echoinfo = {
	.st_rdinit = &echo_rqinit,
	.st_wrinit = &echo_wqinit,
};

static struct cdevsw echo_cdev = {
	.d_name = CONFIG_STREAMS_ECHO_NAME,
	.d_str = &echoinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_ECHO_MODULE
static
#endif
int __init
echoinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_ECHO_MODULE
	printk(KERN_INFO ECHO_BANNER);
#else
	printk(KERN_INFO ECHO_SPLASH);
#endif
	echo_minfo.mi_idnum = modid;
	if ((err = register_strdev(&echo_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_ECHO_MODULE
static
#endif
void __exit
echoexit(void)
{
	unregister_strdev(&echo_cdev, major);
};

#ifdef CONFIG_STREAMS_ECHO_MODULE
module_init(echoinit);
module_exit(echoexit);
#endif
