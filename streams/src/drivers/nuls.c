/*****************************************************************************

 @(#) $RCSfile: nuls.c,v $ $Name:  $($Revision: 0.9.2.50 $) $Date: 2006/12/18 07:32:40 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2006/12/18 07:32:40 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: nuls.c,v $ $Name:  $($Revision: 0.9.2.50 $) $Date: 2006/12/18 07:32:40 $"

static char const ident[] =
    "$RCSfile: nuls.c,v $ $Name:  $($Revision: 0.9.2.50 $) $Date: 2006/12/18 07:32:40 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#ifdef LFS
#include <sys/strsubr.h>
#endif
#include <sys/ddi.h>

#ifdef LFS
#include "sys/config.h"
#endif

#define NULS_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NULS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define NULS_REVISION	"LfS $RCSfile: nuls.c,v $ $Name:  $($Revision: 0.9.2.50 $) $Date: 2006/12/18 07:32:40 $"
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

#if defined LiS && defined MODULE
#define CONFIG_STREAMS_NULS_MODULE MODULE
#endif

#ifdef CONFIG_STREAMS_NULS_MODULE
MODULE_AUTHOR(NULS_CONTACT);
MODULE_DESCRIPTION(NULS_DESCRIP);
MODULE_SUPPORTED_DEVICE(NULS_DEVICE);
MODULE_LICENSE(NULS_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-nuls");
#endif
#endif

#ifndef CONFIG_STREAMS_NULS_NAME
#ifdef LIS
#define CONFIG_STREAMS_NULS_NAME NULS__DRV_NAME
#endif
#ifdef LFS
#define CONFIG_STREAMS_NULS_NAME "nuls"
#endif
#endif
#ifndef CONFIG_STREAMS_NULS_MODID
#ifdef LIS
#define CONFIG_STREAMS_NULS_MODID NULS__ID
#endif
#ifdef LFS
#error CONFIG_STREAMS_NULS_MODID must be defined.
#endif
#endif
#ifndef CONFIG_STREAMS_NULS_MAJOR
#ifdef LIS
#define CONFIG_STREAMS_NULS_MAJOR NULS__CMAJOR_0
#endif
#ifdef LFS
#error CONFIG_STREAMS_NULS_MAJOR must be defined.
#endif
#endif

modID_t modid = CONFIG_STREAMS_NULS_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for NULS driver. (0 for auto allocation)");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NULS_MODID));
MODULE_ALIAS("streams-driver-nuls");
#endif

major_t major = CONFIG_STREAMS_NULS_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for NULS driver. (0 for auto allocation)");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_NULS_MAJOR) "-*");
MODULE_ALIAS("/dev/nuls");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NULS_MAJOR));
MODULE_ALIAS("/dev/streams/nuls");
MODULE_ALIAS("/dev/streams/nuls/*");
MODULE_ALIAS("/dev/streams/clone/nuls");
#endif
#endif

#ifdef LIS
#define STRMINPSZ   0
#define STRMAXPSZ   4096
#define STRHIGH	    5120
#define STRLOW	    1024
#endif

static struct module_info nuls_minfo = {
	.mi_idnum = CONFIG_STREAMS_NULS_MODID,
	.mi_idname = CONFIG_STREAMS_NULS_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat nuls_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat nuls_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

#ifdef LIS
#define _trace() while (0) { }
#define _ptrace(__x) while (0) { }
#define _printd(__x) while (0) { }
#define pswerr(__x) while (0) { }

union ioctypes {
	struct iocblk iocblk;
	struct copyreq copyreq;
	struct copyresp copyresp;
};

#define streamscall _RP

#endif

static streamscall int
nuls_put(queue_t *q, mblk_t *mp)
{
	int err = 0;

	_trace();
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

	_ptrace(("%s: opening major %hu, minor %hu, sflag %d\n", __FUNCTION__, cmajor, cminor,
		sflag));
	if (q->q_ptr != NULL) {
		_printd(("%s: stream is already open\n", __FUNCTION__));
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		_printd(("%s: cannot open as module\n", __FUNCTION__));
		return (ENXIO);	/* can't open as module */
	}
	if (!(p = kmem_alloc(sizeof(*p), KM_NOSLEEP))) {	/* we could sleep */
		_printd(("%s: could not allocate private structure\n", __FUNCTION__));
		return (ENOMEM);	/* no memory */
	}
	bzero(p, sizeof(*p));
	switch (sflag) {
	case CLONEOPEN:
		_printd(("%s: clone open\n", __FUNCTION__));
		if (cminor < 1)
			cminor = 1;
	case DRVOPEN:
	{
		major_t dmajor = cmajor;

		_printd(("%s: driver open\n", __FUNCTION__));
		if (cminor < 1) {
			_printd(("%s: attempt to open minor zero non-clone\n", __FUNCTION__));
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
					pswerr(("%s: stream already open!\n", __FUNCTION__));
					return (EIO);	/* bad error */
				}
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {	/* no minors left */
			spin_unlock(&nuls_lock);
			kmem_free(p, sizeof(*p));
			_printd(("%s: no minor devices left\n", __FUNCTION__));
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
		_printd(("%s: opened major %hu, minor %hu\n", __FUNCTION__, cmajor, cminor));
		return (0);
	}
	}
	pswerr(("%s: bad sflag %d\n", __FUNCTION__, sflag));
	return (ENXIO);
}

static streamscall int
nuls_close(queue_t *q, int oflag, cred_t *crp)
{
	struct nuls *p;

	_trace();
	if ((p = q->q_ptr) == NULL) {
		pswerr(("%s: already closed\n", __FUNCTION__));
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
	_printd(("%s: closed stream with read queue %p\n", __FUNCTION__, q));
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

static struct streamtab nuls_info = {
	.st_rdinit = &nuls_rqinit,
	.st_wrinit = &nuls_wqinit,
};

#ifdef LFS
static struct cdevsw nuls_cdev = {
	.d_name = CONFIG_STREAMS_NULS_NAME,
	.d_str = &nuls_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};
#endif

#ifdef CONFIG_STREAMS_NULS_MODULE
static
#endif
int __init
nuls_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_NULS_MODULE
	printk(KERN_INFO NULS_BANNER);
#else
	printk(KERN_INFO NULS_SPLASH);
#endif
	nuls_minfo.mi_idnum = modid;
#ifdef LFS
	if ((err = register_strdev(&nuls_cdev, major)) < 0)
		return (err);
#endif
#ifdef LIS
	if ((err = lis_register_strdev(major, &nuls_info, 255, CONFIG_STREAMS_NULS_NAME)) < 0)
		return (err);
#endif
	if (major == 0 && err > 0)
		major = err;
#ifdef LIS
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
#endif
	return (0);
};

#ifdef CONFIG_STREAMS_NULS_MODULE
static
#endif
void __exit
nuls_exit(void)
{
#ifdef LFS
	unregister_strdev(&nuls_cdev, major);
#endif
#ifdef LIS
	lis_unregister_strdev(major);
#endif
};

#ifdef CONFIG_STREAMS_NULS_MODULE
module_init(nuls_init);
module_exit(nuls_exit);
#endif
