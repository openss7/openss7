/*****************************************************************************

 @(#) $RCSfile: loop.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/27 23:19:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2006/10/27 23:19:34 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: loop.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/27 23:19:34 $"

static char const ident[] =
    "$RCSfile: loop.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/27 23:19:34 $";

/*
 *  This file contains a classic loop driver for SVR 4.2 STREAMS.  The loop driver is a general
 *  purpose STREAMS-based pipe-like driver that can be used for connecting two upper-level STREAMS
 *  together to form a structure similar to a STREAMS-based Pipe, but which has an additional driver
 *  module beneath each STREAM head.  Upper level STREAMS are linked together using ioctl()
 *  commands contained in <sys/loop.h>.
 */
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/loop.h>

#include "sys/config.h"

#define LOOP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LOOP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define LOOP_REVISION	"LfS $RCSfile: loop.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2006/10/27 23:19:34 $"
#define LOOP_DEVICE	"SVR 4.2 STREAMS Null Stream (LOOP) Device"
#define LOOP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LOOP_LICENSE	"GPL"
#define LOOP_BANNER	LOOP_DESCRIP	"\n" \
			LOOP_COPYRIGHT	"\n" \
			LOOP_REVISION	"\n" \
			LOOP_DEVICE	"\n" \
			LOOP_CONTACT	"\n"
#define LOOP_SPLASH	LOOP_DEVICE	" - " \
			LOOP_REVISION	"\n"

#ifdef CONFIG_STREAMS_LOOP_MODULE
MODULE_AUTHOR(LOOP_CONTACT);
MODULE_DESCRIPTION(LOOP_DESCRIP);
MODULE_SUPPORTED_DEVICE(LOOP_DEVICE);
MODULE_LICENSE(LOOP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-loop");
#endif
#endif

#ifndef CONFIG_STREAMS_LOOP_NAME
#define CONFIG_STREAMS_LOOP_NAME "loop"
#endif
#ifndef CONFIG_STREAMS_LOOP_MODID
#error CONFIG_STREAMS_LOOP_MODID must be defined.
#endif
#ifndef CONFIG_STREAMS_LOOP_MAJOR
#error CONFIG_STREAMS_LOOP_MAJOR must be defined.
#endif

modID_t modid = CONFIG_STREAMS_LOOP_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for LOOP driver. (0 for auto allocation)");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LOOP_MODID));
MODULE_ALIAS("streams-driver-loop");
#endif

major_t major = CONFIG_STREAMS_LOOP_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for LOOP driver. (0 for auto allocation)");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_LOOP_MAJOR) "-*");
MODULE_ALIAS("/dev/loop");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LOOP_MAJOR));
MODULE_ALIAS("/dev/streams/loop");
MODULE_ALIAS("/dev/streams/loop/*");
#endif
#endif

STATIC struct module_info loop_minfo = {
	.mi_idnum = CONFIG_STREAMS_LOOP_MODID,
	.mi_idname = CONFIG_STREAMS_LOOP_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat loop_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat loop_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

typedef struct loop {
	struct loop *next;		/* list linkage */
	struct loop **prev;		/* list linkage */
	struct loop *other;		/* other stream */
	dev_t dev;			/* device number */
	queue_t *rq;			/* this rd queue */
	queue_t *wq;			/* this wr queue */
} loop_t;

STATIC struct loop *loop_opens = NULL;
STATIC spinlock_t loop_lock = SPIN_LOCK_UNLOCKED;

/*
 *  Locking
 */
#if defined CONFIG_STREAMS_NOIRQ || defined _TEST

#define spin_lock_str(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)

#else

#define spin_lock_str(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)

#endif

STATIC streamscall int
loop_wput(queue_t *q, mblk_t *mp)
{
	struct loop *p = q->q_ptr, *o;
	unsigned long flags;
	int err;

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	{
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;

		switch (ioc->iocblk.ioc_cmd) {
		case LOOP_SET:
		{
			dev_t dev;

			if (ioc->iocblk.ioc_count != sizeof(dev)) {
				err = -EINVAL;
				break;
			}
			if (mp->b_cont == NULL) {
				err = -EINVAL;
				break;
			}
			dev = *(dev_t *) mp->b_cont->b_rptr;
			spin_lock_str(&loop_lock, flags);
			for (o = loop_opens; o; o = o->next)
				if (o->dev == dev)
					break;
			if (o == NULL) {
				spin_unlock_str(&loop_lock, flags);
				err = -ENXIO;
				break;
			}
			if (p->other || o->other) {
				spin_unlock_str(&loop_lock, flags);
				err = -EBUSY;
				break;
			}
			p->other = o;
			o->other = p;
			enableok(o->wq);
			enableok(p->wq);
			qenable(o->wq);
			qenable(p->wq);
			spin_unlock_str(&loop_lock, flags);
			err = 0;
			break;
		}
		default:
			err = -EINVAL;
			break;
		}
		if (err == 0) {
			mp->b_datap->db_type = M_IOCACK;
			ioc->iocblk.ioc_count = 0;
			ioc->iocblk.ioc_rval = 0;
			ioc->iocblk.ioc_error = 0;
		} else {
			mp->b_datap->db_type = M_IOCNAK;
			ioc->iocblk.ioc_count = 0;
			ioc->iocblk.ioc_rval = -1;
			ioc->iocblk.ioc_error = -err;
		}
		qreply(q, mp);
		break;
	}
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
		}
		spin_lock_str(&loop_lock, flags);
		if (p->other) {
			queue_t *rq;

			rq = p->other->rq;
			spin_unlock_str(&loop_lock, flags);
			/* switch sense of flush flags */
			switch (mp->b_rptr[0] & (FLUSHW | FLUSHR | FLUSHRW)) {
			case FLUSHR:
				mp->b_rptr[0] = (mp->b_rptr[0] & ~FLUSHR) | FLUSHW;
				break;
			case FLUSHW:
				mp->b_rptr[0] = (mp->b_rptr[0] & ~FLUSHW) | FLUSHR;
				break;
			}
			putnext(rq, mp);
		} else {
			spin_unlock_str(&loop_lock, flags);
			if (mp->b_rptr[0] & FLUSHR) {
				if (mp->b_rptr[0] & FLUSHBAND)
					flushband(RD(q), mp->b_rptr[1], FLUSHALL);
				else
					flushq(RD(q), FLUSHALL);
				mp->b_rptr[0] &= ~FLUSHW;
				qreply(q, mp);
			} else {
				freemsg(mp);
			}
		}
		break;
	default:
		spin_lock_str(&loop_lock, flags);
		if (p->other) {
			if (!q->q_first) {
				queue_t *rq;

				rq = p->other->rq;
				spin_unlock_str(&loop_lock, flags);
				if (rq) {
					if (mp->b_datap->db_type >= QPCTL
					    || bcanputnext(rq, mp->b_band)) {
						putnext(rq, mp);
						break;
					}
				}
			} else
				spin_unlock_str(&loop_lock, flags);
			putq(q, mp);
		} else {
			spin_unlock_str(&loop_lock, flags);
			freemsg(mp);
			putnextctl2(OTHERQ(q), M_ERROR, ENXIO, ENXIO);
		}
		break;
	}
	return (0);
}

/*
 *  The driver only puts messages on its write queue when they fail flow control on the queue beyond
 *  the other read queue.  When flow control subsides, the read service procedure on the othe STREAM
 *  will explicitly enable us.
 */
STATIC streamscall int
loop_wsrv(queue_t *q)
{
	struct loop *p = q->q_ptr;
	unsigned long flags;
	queue_t *rq = NULL;
	mblk_t *mp;

	spin_lock_str(&loop_lock, flags);
	if (p->other)
		rq = p->other->rq;
	spin_unlock_str(&loop_lock, flags);

	if (rq) {
		while ((mp = getq(q))) {
			if (mp->b_datap->db_type >= QPCTL || bcanputnext(rq, mp->b_band)) {
				putnext(rq, mp);
				continue;
			}
			putbq(q, mp);
			break;
		}
	} else
		flushq(q, FLUSHALL);

	return (0);
}

/*
 *  The driver never places messages on the read queue, therefore, the read service procedure is
 *  only invoked when backenabled from upstream.  Back enabling is due to flow control that caused a
 *  message to tbe put (back) to the other STREAM's write queue so the driver needs to explicitly
 *  enable that write queue now.
 */
STATIC streamscall int
loop_rsrv(queue_t *q)
{
	struct loop *p = q->q_ptr;
	unsigned long flags;

	spin_lock_str(&loop_lock, flags);
	if (p->other && p->other->wq)
		qenable(p->other->wq);
	spin_unlock_str(&loop_lock, flags);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
loop_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct loop *p, **pp = &loop_opens;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	unsigned long flags;

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
		spin_lock_str(&loop_lock, flags);
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
					spin_unlock_str(&loop_lock, flags);
					kmem_free(p, sizeof(*p));
					pswerr(("%s: stream already open!\n", __FUNCTION__));
					return (EIO);	/* bad error */
				}
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {	/* no minors left */
			spin_unlock_str(&loop_lock, flags);
			kmem_free(p, sizeof(*p));
			_printd(("%s: no minor devices left\n", __FUNCTION__));
			return (EBUSY);	/* no minors left */
		}
		p->dev = *devp = makedevice(cmajor, cminor);
		p->rq = q;
		p->wq = WR(q);
		noenable(p->wq);
		if ((p->next = *pp))
			p->next->prev = &p->next;
		p->prev = pp;
		*pp = p;
		q->q_ptr = WR(q)->q_ptr = p;
		spin_unlock_str(&loop_lock, flags);
		qprocson(q);
		_printd(("%s: opened major %hu, minor %hu\n", __FUNCTION__, cmajor, cminor));
		return (0);
	}
	}
	pswerr(("%s: bad sflag %d\n", __FUNCTION__, sflag));
	return (ENXIO);
}

STATIC streamscall int
loop_close(queue_t *q, int oflag, cred_t *crp)
{
	struct loop *p;
	unsigned long flags;

	_trace();
	if ((p = q->q_ptr) == NULL) {
		pswerr(("%s: already closed\n", __FUNCTION__));
		return (0);	/* already closed */
	}
	qprocsoff(q);
	spin_lock_str(&loop_lock, flags);
	if (p->other && p->other->rq)
		putnextctl(p->other->rq, M_HANGUP);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	p->other = NULL;
	p->dev = 0;
	p->rq = NULL;
	p->wq = NULL;
	q->q_ptr = WR(q)->q_ptr = NULL;
	spin_unlock_str(&loop_lock, flags);
	_printd(("%s: closed stream with read queue %p\n", __FUNCTION__, q));
	return (0);
}

STATIC struct qinit loop_rqinit = {
	.qi_srvp = loop_rsrv,
	.qi_qopen = loop_open,
	.qi_qclose = loop_close,
	.qi_minfo = &loop_minfo,
	.qi_mstat = &loop_rstat,
};

STATIC struct qinit loop_wqinit = {
	.qi_putp = loop_wput,
	.qi_srvp = loop_wsrv,
	.qi_minfo = &loop_minfo,
	.qi_mstat = &loop_wstat,
};

STATIC struct streamtab loop_info = {
	.st_rdinit = &loop_rqinit,
	.st_wrinit = &loop_wqinit,
};

STATIC struct cdevsw loop_cdev = {
	.d_name = CONFIG_STREAMS_LOOP_NAME,
	.d_str = &loop_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_LOOP_MODULE
STATIC
#endif
int __init
loop_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_LOOP_MODULE
	printk(KERN_INFO LOOP_BANNER);
#else
	printk(KERN_INFO LOOP_SPLASH);
#endif
	loop_minfo.mi_idnum = modid;
	if ((err = register_strdev(&loop_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_LOOP_MODULE
STATIC
#endif
void __exit
loop_exit(void)
{
	unregister_strdev(&loop_cdev, major);
};

#ifdef CONFIG_STREAMS_LOOP_MODULE
module_init(loop_init);
module_exit(loop_exit);
#endif
