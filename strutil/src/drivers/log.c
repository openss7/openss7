/*****************************************************************************

 @(#) $RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/07/17 08:06:36 $

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

 Last Modified $Date: 2005/07/17 08:06:36 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/07/17 08:06:36 $"

static char const ident[] =
    "$RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/07/17 08:06:36 $";

#define _LFS_SOURCE

#include <sys/os7/compat.h>

#include "log.h"

#ifdef LIS
#define CONFIG_STREAMS_LOG_MODID	LOG_DRV_ID
#define CONFIG_STREAMS_LOG_NAME		LOG_DRV_NAME
#define CONFIG_STREAMS_LOG_MAJOR	LOG_CMAJOR_0
#endif

#define LOG_DESCRIP	"UNIX/SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LOG_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define LOG_REVISION	"LfS $RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2005/07/17 08:06:36 $"
#define LOG_DEVICE	"SVR 4.2 STREAMS Log Driver (STRLOG)"
#define LOG_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LOG_LICENSE	"GPL"
#define LOG_BANNER	LOG_DESCRIP	"\n" \
			LOG_COPYRIGHT	"\n" \
			LOG_REVISION	"\n" \
			LOG_DEVICE	"\n" \
			LOG_CONTACT	"\n"
#define LOG_SPLASH	LOG_DEVICE	" - " \
			LOG_REVISION	"\n"

#ifdef CONFIG_STREAMS_UTIL_LOG_MODULE
MODULE_AUTHOR(LOG_CONTACT);
MODULE_DESCRIPTION(LOG_DESCRIP);
MODULE_SUPPORTED_DEVICE(LOG_DEVICE);
MODULE_LICENSE(LOG_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-log");
#endif
#endif

#ifndef CONFIG_STREAMS_LOG_NAME
#error CONFIG_STREAMS_LOG_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_LOG_MAJOR
#error CONFIG_STREAMS_LOG_MAJOR must be defined.
#endif
#ifndef CONFIG_STREAMS_LOG_MODID
#error CONFIG_STREAMS_LOG_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_LOG_MODID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for STREAMS-log driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LOG_MODID));
MODULE_ALIAS("streams-driver-log");
#endif

major_t major = CONFIG_STREAMS_LOG_MAJOR;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for STREAMS-log driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_LOG_MAJOR) "-*");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LOG_MAJOR));
MODULE_ALIAS("/dev/log");
MODULE_ALIAS("/dev/streams/log");
MODULE_ALIAS("/dev/streams/log/*");
#endif

static struct module_info log_minfo = {
	mi_idnum:CONFIG_STREAMS_LOG_MODID,
	mi_idname:CONFIG_STREAMS_LOG_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

queue_t *log_errq = NULL;
queue_t *log_trcq = NULL;

int errlog_sequence = 0;
int trclog_sequence = 0;

/* private structures */
struct log {
	struct log *next;		/* list linkage */
	struct log **prev;		/* list linkage */
	dev_t dev;			/* device number */
	int traceids;			/* the number of trace ids in the trace block */
	mblk_t *traceblk;		/* a message block containing trace ids */
};

static int log_put(queue_t *q, mblk_t *mp)
{
	struct log *log = q->q_ptr;
	union ioctypes *ioc;
	int err = 0, rval = 0;
	mblk_t *dp = mp->b_cont;
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
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
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->iocblk.ioc_cmd) {
		case I_ERRLOG:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			err = -EOPNOTSUPP;
			if (ioc->iocblk.ioc_count == TRANSPARENT)
				goto nak;
			err = -EFAULT;
			if (!dp)
				goto nak;
			err = -ENXIO;
			if (log_errq != NULL)
				goto nak;
			log_errq = RD(q);
			goto ack;
		case I_TRCLOG:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			err = -EOPNOTSUPP;
			if (ioc->iocblk.ioc_count == TRANSPARENT)
				goto nak;
			err = -EFAULT;
			if (!dp)
				goto nak;
			err = -ENXIO;
			if (log_trcq != NULL)
				goto nak;
			err = -EINVAL;
			if (dp->b_wptr < dp->b_rptr + sizeof(struct trace_ids)
			    || (dp->b_wptr - dp->b_rptr) % sizeof(struct trace_ids) != 0)
				goto nak;
			err = -ENOSR;
			if ((log->traceblk = dupb(dp)) == NULL)
				goto nak;
			log->traceids = (dp->b_wptr - dp->b_rptr) / sizeof(struct trace_ids);
			log_trcq = RD(q);
			goto ack;
		}
		err = -EOPNOTSUPP;
		goto nak;
	case M_IOCDATA:
		ioc = (typeof(ioc)) mp->b_rptr;
		err = -EINVAL;
		goto nak;
	case M_PROTO:
	case M_PCPROTO:
		break;
	}
	freemsg(mp);
	return (0);
      nak:
	mp->b_datap->db_type = M_IOCNAK;
	ioc->iocblk.ioc_rval = -1;
	ioc->iocblk.ioc_error = -err;
	qreply(q, mp);
	return (0);
      ack:
	mp->b_datap->db_type = M_IOCACK;
	ioc->iocblk.ioc_rval = rval;
	ioc->iocblk.ioc_error = 0;
	qreply(q, mp);
	return (0);
}

static spinlock_t log_lock = SPIN_LOCK_UNLOCKED;
static struct log *log_list = NULL;

static int log_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct log *p, **pp = &log_list;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next)
		return (ENXIO);	/* can't open as module */
	if (!(p = kmem_alloc(sizeof(*p), KM_NOSLEEP)))	/* we could sleep */
		return (ENOMEM);	/* no memory */
	bzero(p, sizeof(*p));
	switch (sflag) {
	case CLONEOPEN:
		if (cminor < 1)
			cminor = 1;
	case DRVOPEN:
	{
		major_t dmajor = 0;
		minor_t dminor = 0;
		if (cminor < 1)
			return (ENXIO);
		spin_lock(&log_lock);
		for (; *pp && (dmajor = getmajor((*pp)->dev)) < cmajor; pp = &(*pp)->next) ;
		for (; *pp && dmajor == getmajor((*pp)->dev) &&
		     getminor(makedevice(cmajor, cminor)) != 0; pp = &(*pp)->next, cminor++) {
			dminor = getminor((*pp)->dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor && sflag != CLONEOPEN) {
				spin_unlock(&log_lock);
				kmem_free(p, sizeof(*p));
				return (EIO);	/* bad error */
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {
			spin_unlock(&log_lock);
			kmem_free(p, sizeof(*p));
			return (EBUSY);	/* no minors left */
		}
		p->dev = *devp = makedevice(cmajor, cminor);
		if ((p->next = *pp))
			p->next->prev = &p->next;
		p->prev = pp;
		*pp = p;
		q->q_ptr = OTHERQ(q)->q_ptr = p;
		spin_unlock(&log_lock);
		return (0);
	}
	}
	return (ENXIO);
}

static int log_close(queue_t *q, int oflag, cred_t *crp)
{
	struct log *p;
	if ((p = q->q_ptr) == NULL)
		return (0);	/* already closed */
	spin_lock(&log_lock);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	spin_unlock(&log_lock);
	return (0);
}

static struct qinit log_rqinit = {
	qi_qopen:log_open,
	qi_qclose:log_close,
	qi_minfo:&log_minfo,
};

static struct qinit log_wqinit = {
	qi_putp:log_put,
	qi_minfo:&log_minfo,
};

static struct streamtab log_info = {
	st_rdinit:&log_rqinit,
	st_wrinit:&log_wqinit,
};

static struct cdevsw log_cdev = {
	d_name:CONFIG_STREAMS_LOG_NAME,
	d_str:&log_info,
	d_flag:0,
	d_fop:NULL,
	d_mode:S_IFCHR | S_IRUGO | S_IWUGO,
	d_kmod:THIS_MODULE,
};

#ifdef CONFIG_STREAMS_UTIL_LOG_MODULE
static
#endif
int __init log_init(void)
{
	int err;
#ifdef CONFIG_STREAMS_UTIL_LOG_MODULE
	printk(KERN_INFO LOG_BANNER);
#else
	printk(KERN_INFO LOG_SPLASH);
#endif
	log_minfo.mi_idnum = modid;
	if ((err = register_strdev(&log_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
}
#ifdef CONFIG_STREAMS_UTIL_LOG_MODULE
static
#endif
void __exit log_exit(void)
{
	unregister_strdev(&log_cdev, major);
}

#ifdef CONFIG_STREAMS_UTIL_LOG_MODULE
module_init(log_init);
module_exit(log_exit);
#endif
