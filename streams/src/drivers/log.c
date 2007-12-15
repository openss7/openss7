/*****************************************************************************

 @(#) $RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.54 $) $Date: 2007/12/15 20:19:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/12/15 20:19:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: log.c,v $
 Revision 0.9.2.54  2007/12/15 20:19:53  brian
 - updates

 Revision 0.9.2.53  2007/08/15 05:33:21  brian
 - GPLv3 updates

 Revision 0.9.2.52  2007/08/13 22:46:15  brian
 - GPLv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.54 $) $Date: 2007/12/15 20:19:53 $"

static char const ident[] =
    "$RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.54 $) $Date: 2007/12/15 20:19:53 $";

/*
 *  This driver provides a STREAMS based error and trace logger for the STREAMS subsystem.  This is
 *  distinct from the Linux syslog and klog.  One of the primary purposes of providing such a
 *  logging device is to suport the strlog(9) trace faciltiy.  This is a useful tool for debugging
 *  and interrogation of a live system.  Both Linux STREAMS (LiS) and Linux Fast-STREAMS (LfS) have
 *  been equipped with a hook to the strlog(9) function.  This hook is a vstrlog function pointer
 *  that defaults to internal kernel logging.  When the strlog(9) function is called by a STREAMS
 *  driver or module, we here first scan for whether there is an open trace or error logging stream
 *  willing to accept the message (from mid, sid and level).  If there is a logger that wishes to
 *  accept the message, it is formatted and sent upstream.  If there is no logger, or if the logger
 *  is flow controlled, the message is simply dropped.  This means that a STREAMS driver or module
 *  can generate alot of trace information without worrying about its impact on system performance.
 *  Normally calls to strlog(9) would be simply null operations.  Only when a trace or error logger
 *  is present in the system would information be passed upstream.  This is far preferrable to
 *  cmn_err(9) which generates each and every message to the kernel log.
 */
#include <stdarg.h>

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>	/* for local_bh_disable */
#include <linux/ctype.h>	/* for isdigit */

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>
#include <sys/strlog.h>

#include "sys/config.h"
#include "log.h"
#if !defined HAVE_KFUNC_ATOMIC_ADD_RETURN
#include "src/kernel/strutil.h"
#endif

#define LOG_DESCRIP	"UNIX/SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LOG_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define LOG_REVISION	"LfS $RCSfile: log.c,v $ $Name:  $($Revision: 0.9.2.54 $) $Date: 2007/12/15 20:19:53 $"
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

#ifdef CONFIG_STREAMS_LOG_MODULE
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
module_param(modid, ushort, 0444);
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
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for STREAMS-log driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_LOG_MAJOR) "-*");
MODULE_ALIAS("/dev/log");
MODULE_ALIAS("/dev/strlog");
MODULE_ALIAS("/dev/conslog");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LOG_MAJOR));
MODULE_ALIAS("/dev/streams/log");
MODULE_ALIAS("/dev/streams/log/*");
MODULE_ALIAS("/dev/streams/clone/log");
#endif
#endif

static struct module_info log_minfo = {
	.mi_idnum = CONFIG_STREAMS_LOG_MODID,
	.mi_idname = CONFIG_STREAMS_LOG_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat log_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat log_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

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

queue_t *log_conq = NULL;
queue_t *log_errq = NULL;
queue_t *log_trcq = NULL;

atomic_t conlog_sequence = ATOMIC_INIT(0);
atomic_t errlog_sequence = ATOMIC_INIT(0);
atomic_t trclog_sequence = ATOMIC_INIT(0);

#if !defined HAVE_KFUNC_ATOMIC_ADD_RETURN
static spinlock_t my_atomic_lock = SPIN_LOCK_UNLOCKED;
int
my_atomic_add_return(int val, atomic_t *atomic)
{
	int ret;
	unsigned long flags;

	/* XXX: do these locks have to be so severe? */
	streams_spin_lock(&my_atomic_lock, flags);
	atomic_add(val, atomic);
	ret = atomic_read(atomic);
	streams_spin_unlock(&my_atomic_lock, flags);
	return (ret);
}

#undef atomic_add_return
#define atomic_add_return my_atomic_add_return
#endif

/* private structures */
struct log {
	struct log *next;		/* list linkage */
	struct log **prev;		/* list linkage */
	dev_t dev;			/* device number */
	int traceids;			/* the number of trace ids in the trace block */
	mblk_t *traceblk;		/* a message block containing trace ids */
	queue_t **logq;			/* log for which we are a queue */
};

#define PROMOTE_SIZE		(sizeof(int))
#define PROMOTE_ALIGN(__x)	(((__x) + PROMOTE_SIZE - 1) & ~(PROMOTE_SIZE - 1))
#define PROMOTE_SIZEOF(__x)	((sizeof(__x) < PROMOTE_SIZE) ? PROMOTE_SIZE : sizeof(__x))

/*
 *  Determine whether the message is filtered or not.  Return 1 if the message
 *  is to be delivered to the trace logger and 0 if it is not.
 */
static int
log_trace_filter(queue_t *q, short mid, short sid, char level)
{
	int rval = 0;

	if (q) {
		struct log *p = (typeof(p)) q->q_ptr;
		struct trace_ids *ti;
		int i;

		for (i = 0, ti = (typeof(ti)) p->traceblk->b_rptr; i < p->traceids; i++, ti++) {
			if ((ti->ti_mid == mid || ti->ti_mid == (short) (-1UL))
			    && (ti->ti_sid == sid || ti->ti_sid == (short) (-1UL))
			    && (ti->ti_level >= level || ti->ti_level == (char) (-1UL))) {
				/* trace logger sees this message */
				rval = 1;
				break;
			}
		}
	}
	return (rval);
}

static streamscall int
log_rput(queue_t *q, mblk_t *mp)
{
	/* pass message along with flow control */
	if (mp->b_datap->db_type >= QPCTL
	    || (!q->q_first && !(q->q_flag & QSVCBUSY) && bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (1);
	}
	if (bcanput(q, mp->b_band) && putq(q, mp))
		return (1);
	freemsg(mp);
	return (0);
}

static streamscall int
log_rsrv(queue_t *q)
{
	mblk_t *mp;

	/* simply drain the queue */
	while ((mp = getq(q))) {
		if (mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			continue;
		}
		putbq(q, mp);
		break;
	}
	return (0);
}

/*
 *  Allocate a control block portion for a log message and fill it out.
 */
static mblk_t *
log_alloc_ctl(queue_t *q, short mid, short sid, char level, unsigned short flags, int seq_no,
	      int source)
{
	struct log_ctl *lp;
	mblk_t *mp = NULL;
	int pri;

	if (flags & SL_WARN)
		pri = LOG_WARNING;
	else if (flags & SL_FATAL)
		pri = LOG_ERR;
	else if (flags & SL_NOTE)
		pri = LOG_NOTICE;
	else if (flags & SL_TRACE)
		pri = LOG_DEBUG;
	else
		pri = LOG_INFO;
	if ((mp = allocb(sizeof(*lp), BPRI_MED))) {
		struct timeval tv;

		mp->b_datap->db_type = (flags & SL_NOPUTBUF) ? M_PCPROTO : M_PROTO;
		lp = (typeof(lp)) mp->b_rptr;
		mp->b_wptr = mp->b_rptr + sizeof(*lp);
		mp->b_band = (7 - pri);
		lp->mid = mid;
		lp->sid = sid;
		lp->level = level;
		lp->flags = flags;
		lp->ttime = jiffies;
		do_gettimeofday(&tv);
		lp->ltime = tv.tv_sec;
		lp->seq_no = seq_no;
		lp->pri = source | pri;
	}
	return (mp);
}

/*
 *  Try to allocate a control block, duplicate the data block, and deliver the
 *  message.  Return 1 on success, 0 on failure.
 */
static int
log_deliver_msg(queue_t *q, short mid, short sid, char level, int flags, int seq, mblk_t *dp,
		int source)
{
	mblk_t *bp, *mp;

	if (q && (bp = dupmsg(dp)) && (mp = log_alloc_ctl(q, mid, sid, level, flags, seq, source))) {
		mp->b_cont = bp;
		return log_rput(q, mp);
	}
	return (0);
}

static streamscall int
log_wput(queue_t *q, mblk_t *mp)
{
	struct log *log = q->q_ptr;
	union ioctypes *ioc;
	int err = 0, rval = 0;
	mblk_t *dp = mp->b_cont;

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
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
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->iocblk.ioc_cmd) {
		case I_CONSLOG:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			err = -EINVAL;
			if (ioc->iocblk.ioc_count == TRANSPARENT)
				goto nak;
			err = -ENXIO;
			if (log_conq != NULL)
				goto nak;
			log_conq = RD(q);
			log->logq = &log_conq;
			goto ack;
		case I_ERRLOG:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			err = -EINVAL;
			if (ioc->iocblk.ioc_count == TRANSPARENT)
				goto nak;
			err = -ENXIO;
			if (log_errq != NULL)
				goto nak;
			log_errq = RD(q);
			log->logq = &log_errq;
			goto ack;
		case I_TRCLOG:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			err = -EINVAL;
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
			if ((log->traceblk = copyb(dp)) == NULL)
				goto nak;
			log->traceids = (dp->b_wptr - dp->b_rptr) / sizeof(struct trace_ids);
			log_trcq = RD(q);
			log->logq = &log_trcq;
			goto ack;
		}
		err = -EINVAL;
		goto nak;
	case M_IOCDATA:
		ioc = (typeof(ioc)) mp->b_rptr;
		err = -EINVAL;
		goto nak;
	case M_PROTO:
	case M_PCPROTO:
	{
		struct log_ctl *lp;
		struct timeval tv;

		if (!log_conq && !log_errq && !log_trcq)
			break;
		if (mp->b_wptr < mp->b_rptr + sizeof(*lp))
			break;
		do_gettimeofday(&tv);
		lp = (typeof(lp)) mp->b_rptr;
		if (lp->flags & (SL_CONSOLE | SL_ERROR | SL_TRACE)) {
			if (lp->flags & SL_CONSOLE)
				log_deliver_msg(log_conq, lp->mid, lp->sid, lp->level, lp->flags,
						atomic_add_return(1, &conlog_sequence), mp->b_cont,
						LOG_USER);
			if (lp->flags & SL_ERROR)
				log_deliver_msg(log_errq, lp->mid, lp->sid, lp->level, lp->flags,
						atomic_add_return(1, &errlog_sequence), mp->b_cont,
						LOG_USER);
			if (lp->flags & SL_TRACE
			    && log_trace_filter(log_trcq, lp->mid, lp->sid, lp->level))
				log_deliver_msg(log_trcq, lp->mid, lp->sid, lp->level, lp->flags,
						atomic_add_return(1, &trclog_sequence), mp->b_cont,
						LOG_USER);
		}
		break;
	}
	case M_DATA:
		/* just for /dev/conslog */
		if (getminor(log->dev) != 1)
			break;
		/* only for the console */
		log_deliver_msg(log_conq, 0, 0, 0, SL_CONSOLE,
				atomic_add_return(1, &conlog_sequence), mp, LOG_USER);
		break;
	}
	freemsg(mp);
	return (0);
      nak:
	mp->b_datap->db_type = M_IOCNAK;
	ioc->iocblk.ioc_count = 0;
	ioc->iocblk.ioc_rval = -1;
	ioc->iocblk.ioc_error = -err;
	qreply(q, mp);
	return (0);
      ack:
	mp->b_datap->db_type = M_IOCACK;
	ioc->iocblk.ioc_count = 0;
	ioc->iocblk.ioc_rval = rval;
	ioc->iocblk.ioc_error = 0;
	qreply(q, mp);
	return (0);
}

static spinlock_t log_lock = SPIN_LOCK_UNLOCKED;
static struct log *log_list = NULL;

static streamscall int
log_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct log *p, **pp = &log_list;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	unsigned long flags;

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
			cminor = 2;
	case DRVOPEN:
	{
		major_t dmajor = 0;
		minor_t dminor = 0;

		if (cminor < 1)
			return (ENXIO);
		spin_lock_str(&log_lock, flags);
		for (; *pp && (dmajor = getmajor((*pp)->dev)) < cmajor; pp = &(*pp)->next) ;
		for (; *pp && dmajor == getmajor((*pp)->dev) &&
		     getminor(makedevice(cmajor, cminor)) != 0; pp = &(*pp)->next) {
			dminor = getminor((*pp)->dev);
			if (cminor < dminor)
				break;
			if (cminor == dminor) {
				if (sflag == CLONEOPEN)
					cminor++;
				else {
					spin_unlock_str(&log_lock, flags);
					kmem_free(p, sizeof(*p));
					return (EIO);	/* bad error */
				}
			}
		}
		if (getminor(makedevice(cmajor, cminor)) == 0) {
			spin_unlock_str(&log_lock, flags);
			kmem_free(p, sizeof(*p));
			return (EBUSY);	/* no minors left */
		}
		p->dev = *devp = makedevice(cmajor, cminor);
		if ((p->next = *pp))
			p->next->prev = &p->next;
		p->prev = pp;
		*pp = p;
		q->q_ptr = OTHERQ(q)->q_ptr = p;
		spin_unlock_str(&log_lock, flags);
		qprocson(q);
		return (0);
	}
	}
	return (ENXIO);
}

static streamscall int
log_close(queue_t *q, int oflag, cred_t *crp)
{
	struct log *p;
	unsigned long flags;

	if ((p = q->q_ptr) == NULL)
		return (0);	/* already closed */
	qprocsoff(q);
	if (p->logq)
		*p->logq = NULL;
	if (p->traceblk)
		freemsg(xchg(&p->traceblk, NULL));
	spin_lock_str(&log_lock, flags);
	if ((*(p->prev) = p->next))
		p->next->prev = p->prev;
	p->next = NULL;
	p->prev = &p->next;
	q->q_ptr = OTHERQ(q)->q_ptr = NULL;
	spin_unlock_str(&log_lock, flags);
	return (0);
}

static struct qinit log_rqinit = {
	.qi_putp = log_rput,
	.qi_srvp = log_rsrv,
	.qi_qopen = log_open,
	.qi_qclose = log_close,
	.qi_minfo = &log_minfo,
	.qi_mstat = &log_rstat,
};

static struct qinit log_wqinit = {
	.qi_putp = log_wput,
	.qi_minfo = &log_minfo,
	.qi_mstat = &log_wstat,
};

static struct streamtab log_info = {
	.st_rdinit = &log_rqinit,
	.st_wrinit = &log_wqinit,
};

static struct cdevsw log_cdev = {
	.d_name = CONFIG_STREAMS_LOG_NAME,
	.d_str = &log_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

/*
 * This is where we hook into the STREAMS strlog(9) function.
 */

static vstrlog_t oldlog = NULL;

#ifndef va_copy
#define va_copy(__x,__y) __va_copy((__x),(__y))
#endif
/*
 *  Allocate a data block and fill it in with the format string and an argument
 *  list beginning with the integer aligned following the format string.  We
 *  need to parse the format string (twice: once for sizing and once to assemble
 *  the list) to do this, so the number of arguments passed should be small
 *  (NLOGARGS is 3).  We process as many arguments as passed but you should not
 *  send more than NLOGARGS.
 */
static mblk_t *
log_alloc_data(char *fmt, va_list args)
{
	mblk_t *bp;
	size_t len = strnlen(fmt, LOGMSGSZ);
	size_t plen = PROMOTE_ALIGN(len + 1);

	fmt[len] = '\0';	/* force null termination and truncate */
	if ((bp = allocb(plen, BPRI_MED))) {
		va_list args2;
		size_t nargs = 0, alen = 0;
		char type;
		mblk_t *dp;

		bp->b_datap->db_type = M_DATA;
		bp->b_wptr = bp->b_rptr;
		va_copy(args2, args);
		/* two passes, one for sizing the next for argument list preparation */
		for (; *fmt; ++fmt, bp->b_wptr++) {
			if ((*bp->b_wptr = *fmt) != '%')
				continue;
			/* flags */
			for (++fmt, bp->b_wptr++;; ++fmt, bp->b_wptr++) {
				switch ((*bp->b_wptr = *fmt)) {
				case '-':
				case '+':
				case ' ':
				case '#':
				case '0':
					continue;
				default:
					break;
				}
				break;
			}
			/* field width */
			if (isdigit((*bp->b_wptr = *fmt))) {
				++fmt;
				bp->b_wptr++;
				while (isdigit((*bp->b_wptr = *fmt))) {
					++fmt;
					bp->b_wptr++;
				}
			} else if ((*bp->b_wptr = *fmt) == '*') {
				++fmt;
				bp->b_wptr++;
				alen += PROMOTE_SIZEOF(int);
				(void) va_arg(args2, int);

				if (++nargs == NLOGARGS)
					break;
			}
			/* get precision */
			if ((*bp->b_wptr = *fmt) == '.') {
				++fmt;
				bp->b_wptr++;
				if (isdigit((*bp->b_wptr = *fmt))) {
					++fmt;
					bp->b_wptr++;
					while (isdigit((*bp->b_wptr = *fmt))) {
						++fmt;
						bp->b_wptr++;
					}
				} else if ((*bp->b_wptr = *fmt) == '*') {
					++fmt;
					bp->b_wptr++;
					alen += PROMOTE_SIZEOF(int);
					(void) va_arg(args2, int);

					if (++nargs == NLOGARGS)
						break;
				}
			}
			/* get qualifier */
			type = 'i';
			switch ((*bp->b_wptr = *fmt)) {
			case 'h':
				type = *fmt;
				++fmt;
				bp->b_wptr++;
				if ((*bp->b_wptr = *fmt) == 'h') {
					++fmt;
					bp->b_wptr++;
					type = 'c';
				}
				break;
			case 'l':
				type = *fmt;
				++fmt;
				bp->b_wptr++;
				if ((*bp->b_wptr = *fmt) == 'l') {
					++fmt;
					bp->b_wptr++;
					type = 'L';
				}
				break;
			case 'q':
			case 'L':
			case 'Z':
			case 'z':
			case 't':
				type = *fmt;
				++fmt;
				bp->b_wptr++;
				break;
			}
			switch ((*bp->b_wptr = *fmt)) {
			case 'c':
			case 'p':
			case 'o':
			case 'X':
			case 'x':
			case 'd':
			case 'i':
			case 'u':
				switch (*fmt) {
				case 'c':
					type = 'c';
					break;
				case 'p':
					type = 'p';
					break;
				}
				switch (type) {
				case 'p':
					alen += PROMOTE_SIZEOF(void *);
					(void) va_arg(args2, void *);

					break;
				case 't':
					alen += PROMOTE_SIZEOF(ptrdiff_t);
					(void) va_arg(args2, ptrdiff_t);
					break;
				case 'l':
					alen += PROMOTE_SIZEOF(long);
					(void) va_arg(args2, long);

					break;
				case 'q':
				case 'L':
					alen += PROMOTE_SIZEOF(long long);
					(void) va_arg(args2, long long);

					break;
				case 'Z':
				case 'z':
					alen += PROMOTE_SIZEOF(size_t);
					(void) va_arg(args2, size_t);

					break;
				case 'h':
					alen += PROMOTE_SIZEOF(short);
					(void) va_arg(args2, int);

					break;
				case 'c':
					alen += PROMOTE_SIZEOF(char);
					(void) va_arg(args2, int);

					break;
				case 'i':
				default:
					alen += PROMOTE_SIZEOF(int);
					(void) va_arg(args2, int);

					break;
				}
				if (++nargs == NLOGARGS)
					break;
				continue;
			case 's':
			{
				char *s = va_arg(args2, char *);
				size_t slen = strnlen(s, LOGMSGSZ);
				size_t splen = PROMOTE_ALIGN(slen + 1);

				alen += splen;
				if (++nargs == NLOGARGS)
					break;
				continue;
			}
			case '%':
			default:
				continue;
			}
			break;
		}
		*bp->b_wptr++ = '\0';	/* terminate format string */
		bp->b_wptr = bp->b_rptr + plen;
		va_end(args2);
		/* pass through once more with arguments */
		if ((dp = allocb(alen, BPRI_MED))) {
			fmt = bp->b_rptr;
			for (; *fmt; ++fmt) {
				if (*fmt != '%')
					continue;
				/* flags */
				for (++fmt;; ++fmt) {
					switch (*fmt) {
					case '-':
					case '+':
					case ' ':
					case '#':
					case '0':
						continue;
					default:
						break;
					}
					break;
				}
				/* field width */
				if (isdigit(*fmt)) {
					++fmt;
					while (isdigit(*fmt)) {
						++fmt;
					}
				} else if (*fmt == '*') {
					++fmt;
					*(int *) dp->b_wptr = va_arg(args, int);
					dp->b_wptr += PROMOTE_SIZEOF(int);

					if (!--nargs)
						break;
				}
				/* get precision */
				if (*fmt == '.') {
					++fmt;
					if (isdigit(*fmt)) {
						++fmt;
						while (isdigit(*fmt)) {
							++fmt;
						}
					} else if (*fmt == '*') {
						++fmt;
						*(int *) dp->b_wptr = va_arg(args, int);
						dp->b_wptr += PROMOTE_SIZEOF(int);

						if (!--nargs)
							break;
					}
				}
				/* get qualifier */
				type = 'i';
				switch (*fmt) {
				case 'h':
					++fmt;
					if (*fmt == 'h') {
						type = 'c';
						++fmt;
					}
					break;
				case 'l':
					++fmt;
					if (*fmt == 'l') {
						type = 'L';
						++fmt;
					}
					break;
				case 'L':
				case 'Z':
				case 'z':
				case 't':
					type = *fmt;
					++fmt;
					break;
				}
				switch (*fmt) {
				case 'c':
				case 'n':
				case 'p':
				case 'o':
				case 'X':
				case 'x':
				case 'd':
				case 'i':
				case 'u':
					switch (*fmt) {
					case 'c':
						type = 'c';
						break;
					case 'p':
						type = 'p';
						break;
					}
					switch (type) {
					case 'p':
						*(void **) dp->b_wptr = va_arg(args, void *);
						dp->b_wptr += PROMOTE_SIZEOF(void *);

						break;
					case 'l':
						*(long *) dp->b_wptr = va_arg(args, long);
						dp->b_wptr += PROMOTE_SIZEOF(long);

						break;
					case 'L':
						*(long long *) dp->b_wptr = va_arg(args, long long);
						dp->b_wptr += PROMOTE_SIZEOF(long long);

						break;
					case 'Z':
					case 'z':
						*(size_t *) dp->b_wptr = va_arg(args, size_t);
						dp->b_wptr += PROMOTE_SIZEOF(size_t);

						break;
					case 't':
						*(ptrdiff_t *) dp->b_wptr = va_arg(args, ptrdiff_t);
						dp->b_wptr += PROMOTE_SIZEOF(ptrdiff_t);
						break;
					case 'h':
						*(int *) dp->b_wptr = va_arg(args, int);
						dp->b_wptr += PROMOTE_SIZEOF(short);

						break;
					case 'c':
						*(int *) dp->b_wptr = va_arg(args, int);
						dp->b_wptr += PROMOTE_SIZEOF(short);

						break;
					default:
						*(int *) dp->b_wptr = va_arg(args, int);
						dp->b_wptr += PROMOTE_SIZEOF(int);

						break;
					}
					if (!--nargs)
						break;
					continue;
				case 's':
				{
					char *s = va_arg(args, char *);
					size_t slen = strnlen(s, LOGMSGSZ);
					size_t splen = PROMOTE_ALIGN(slen + 1);

					strncpy(dp->b_wptr, s, slen);
					dp->b_wptr[slen] = '\0';
					dp->b_wptr += splen;
					if (!--nargs)
						break;
					continue;
				}
				case '%':
				default:
					continue;
				}
				break;
			}
			bp->b_cont = dp;
		} else {
			freemsg(bp);
			bp = NULL;
		}
	}
	return (bp);
}

static int
log_vstrlog(short mid, short sid, char level, unsigned short flags, char *fmt, va_list args)
{
	/* check if the log is for the console, error or trace logger */
	int rval = 1;

	if (flags & (SL_CONSOLE | SL_ERROR | SL_TRACE)) {
		mblk_t *dp;

		/* allocate the data block first so we can dup it */
		if ((dp = log_alloc_data(fmt, args))) {
			/* the console logger sees all logs */
			if (flags & SL_CONSOLE
			    && !log_deliver_msg(log_conq, mid, sid, level, flags,
						atomic_add_return(1, &conlog_sequence), dp,
						LOG_KERN))
				rval = 0;
			/* the error logger sees all logs */
			if (flags & SL_ERROR
			    && !log_deliver_msg(log_errq, mid, sid, level, flags,
						atomic_add_return(1, &errlog_sequence), dp,
						LOG_KERN))
				rval = 0;
			/* the trace logger sees selected logs */
			if (flags & SL_TRACE && log_trace_filter(log_trcq, mid, sid, level)
			    && !log_deliver_msg(log_trcq, mid, sid, level, flags,
						atomic_add_return(1, &trclog_sequence), dp,
						LOG_KERN))
				rval = 0;
			/* free original data block */
			freemsg(dp);
		}
	}
	return (rval);
}

#ifdef CONFIG_STREAMS_LOG_MODULE
static
#endif
int __init
log_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_LOG_MODULE
	printk(KERN_INFO LOG_BANNER);
#else
	printk(KERN_INFO LOG_SPLASH);
#endif
	log_minfo.mi_idnum = modid;
	if ((err = register_strdev(&log_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	/* hook into vstrlog */
#if 0
	oldlog = xchg(&vstrlog, &log_vstrlog);
#else
	oldlog = register_strlog(&log_vstrlog);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_LOG_MODULE
static
#endif
void __exit
log_exit(void)
{
	/* unhook from vstrlog */
#if 0
	vstrlog = oldlog;
#else
	register_strlog(oldlog);
#endif
	unregister_strdev(&log_cdev, major);
}

#ifdef CONFIG_STREAMS_LOG_MODULE
module_init(log_init);
module_exit(log_exit);
#endif
