/*****************************************************************************

 @(#) strhead.c,v (0.9.2.19) 2003/10/27 12:23:15

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

 Last Modified 2003/10/27 12:23:15 by brian

 *****************************************************************************/

#ident "@(#) strhead.c,v (0.9.2.19) 2003/10/27 12:23:15"

static char const ident[] = "strhead.c,v (0.9.2.19) 2003/10/27 12:23:15";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/kernel.h>	/* for FASTCALL() */
#include <linux/sched.h>	/* for send_sig_info() */
#include <linux/spinlock.h>
#include <linux/fs.h>		/* for file */
#include <linux/file.h>		/* for fget() */
#include <linux/poll.h>		/* for poll_wait */
#include <linux/highmem.h>	/* for kmap, kunmap */

#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strsubr.h>
#include <linux/strconf.h>
#include <linux/ddi.h>

#include "strdebug.h"
#include "strsched.h"		/* for allocsd */
#include "strreg.h"		/* For str_args */
#include "strhead.h"		/* extern verification */
#include "strsysctl.h"		/* for sysctls */
#include "strsad.h"		/* for autopush_find */
#include "strutil.h"		/* for q locking and puts and gets */
#include "strattach.h"		/* for do_fattach/do_fdetach */

#define QR_DONE		0
#define QR_ABSORBED	1
#define QR_TRIMMED	2
#define QR_LOOP		3
#define QR_PASSALONG	4
#define QR_PASSFLOW	5
#define QR_DISABLE	6
#define QR_STRIP	7
#define QR_RETRY	8

#ifndef CONFIG_STREAMS_STH_NAME
#define CONFIG_STREAMS_STH_NAME "sth"
// #warning "CONFIG_STREAMS_STH_NAME not defined."
#endif
#ifndef CONFIG_STREAMS_STH_MODID
#define CONFIG_STREAMS_STH_MODID 0
// #warning "CONFIG_STREAMS_STH_MODID not defined."
#endif

struct module_info str_minfo = {
	mi_idnum:CONFIG_STREAMS_STH_MODID,
	mi_idname:CONFIG_STREAMS_STH_NAME,
	mi_minpsz:STRMINPSZ,
	mi_maxpsz:STRMAXPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static int str_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp);
static int str_close(queue_t *q, int oflag, cred_t *crp);

static int strrput(queue_t *q, mblk_t *mp);
static int strwsrv(queue_t *q);

static struct qinit str_rinit = {
	qi_putp:strrput,
	qi_qopen:str_open,
	qi_qclose:str_close,
	qi_minfo:&str_minfo,
};

static struct qinit str_winit = {
	qi_srvp:strwsrv,
	qi_minfo:&str_minfo,
};

struct streamtab str_info = {
	st_rdinit:&str_rinit,
	st_wrinit:&str_winit,
};

#define stri_lookup(__f) ((struct stdata *)(__f)->private_data)
#define sdev_lookup(__i) ((struct cdevsw *)(__i)->i_cdev->data)

/* 
 *  -------------------------------------------------------------------------
 *
 *  STREAMS IO Controls
 *
 *  -------------------------------------------------------------------------
 */
static int check_stream_io(struct file *file, struct stdata *sd)
{
	if (unlikely(file == NULL))
		return (-EBADF);
	if (unlikely(sd == NULL))
		return (-ENOSTR);
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag)))
			return (!(sd->sd_flag & (STRISFIFO | STRISPIPE)) ? -ENXIO : -EPIPE);
	}
	return (0);
}
static int check_stream_rd(struct file *file, struct stdata *sd)
{
	if (unlikely(file == NULL || !(file->f_mode & FREAD)))
		return (-EBADF);
	if (unlikely(sd == NULL))
		return (-ENOSTR);
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP | STRDERR))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag)))
			return (!(sd->sd_flag & (STRISFIFO | STRISPIPE)) ? -ENXIO : -EPIPE);
		if (unlikely(test_bit(STRDERR_BIT, &sd->sd_flag))) {
			if (sd->sd_eropt & RERRNONPERSIST)
				clear_bit(STRDERR_BIT, &sd->sd_flag);
			return (sd->sd_rerror);
		}
	}
	if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
		if (likely(sd->sd_other == NULL))
			return (-EPIPE);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)))
			return (-EPIPE);
		if (unlikely(sd->sd_other->sd_writers == 0))
			return (-ESTRPIPE);
	}
	return (0);
}
static int check_stream_wr(struct file *file, struct stdata *sd)
{
	if (unlikely(file == NULL || !(file->f_mode & FWRITE)))
		return (-EBADF);
	if (unlikely(sd == NULL))
		return (-ENOSTR);
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP | STWRERR))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag))) {
			if (!(sd->sd_flag & (STRISFIFO | STRISPIPE)))
				return (-ENXIO);
			send_sig_info(SIGPIPE, (struct siginfo *) 1, current);
			return (-EPIPE);
		}
		if (unlikely(test_bit(STWRERR_BIT, &sd->sd_flag))) {
			if (sd->sd_eropt & WERRNONPERSIST)
				clear_bit(STWRERR_BIT, &sd->sd_flag);
			if (sd->sd_wropt & SNDPIPE)
				send_sig_info(SIGPIPE, (struct siginfo *) 1, current);
			return (sd->sd_werror);
		}
	}
	if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
		if (likely(sd->sd_other == NULL)) {
			send_sig_info(SIGPIPE, (struct siginfo *) 1, current);
			return (-EPIPE);
		}
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag))) {
			send_sig_info(SIGPIPE, (struct siginfo *) 1, current);
			return (-EPIPE);
		}
		if (unlikely(sd->sd_other->sd_readers == 0))
			return (-ESTRPIPE);
	}
	return (0);
}
static int check_stream_oc(struct file *file, struct stdata *sd)
{
	if (unlikely(file == NULL))
		return (-EBADF);
	if (unlikely(sd == NULL))
		return (-ENOSTR);
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP | STRDERR | STWRERR))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag)))
			return (!(sd->sd_flag & (STRISFIFO | STRISPIPE)) ? -ENXIO : -EPIPE);
		if (unlikely(test_bit(STRDERR_BIT, &sd->sd_flag) && (file->f_mode & FREAD))) {
			if (sd->sd_eropt & RERRNONPERSIST)
				clear_bit(STRDERR_BIT, &sd->sd_flag);
			return (sd->sd_rerror);
		}
		if (unlikely(test_bit(STWRERR_BIT, &sd->sd_flag) && (file->f_mode & FWRITE))) {
			if (sd->sd_eropt & WERRNONPERSIST)
				clear_bit(STWRERR_BIT, &sd->sd_flag);
			return (sd->sd_rerror);
		}
	}
	if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
		if (likely(sd->sd_other == NULL))
			return (-EPIPE);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)))
			return (-EPIPE);
	}
	return (0);
}

static int check_wakeup_io(struct file *file, struct stdata *sd, long *timeo)
{
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag)))
			return (!(sd->sd_flag & (STRISFIFO | STRISPIPE)) ? -ENXIO : -EPIPE);
	}
	if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
		if (likely(sd->sd_other == NULL))
			return (-EPIPE);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)))
			return (-EPIPE);
	}
	if (unlikely(signal_pending(current)))
		return ((*timeo == MAX_SCHEDULE_TIMEOUT) ? -ERESTARTSYS : -EINTR);
	if (unlikely(*timeo == 0))
		return ((file->f_flags & (O_NONBLOCK | O_NDELAY)) ? -EAGAIN : -ETIME);
	return (0);
}
static int check_wakeup_rd(struct file *file, struct stdata *sd, long *timeo)
{
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP | STRDERR))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag)))
			return (!(sd->sd_flag & (STRISFIFO | STRISPIPE)) ? -ENXIO : -EPIPE);
		if (unlikely(test_bit(STRDERR_BIT, &sd->sd_flag))) {
			if (sd->sd_eropt & RERRNONPERSIST)
				clear_bit(STRDERR_BIT, &sd->sd_flag);
			return (sd->sd_rerror);
		}
	}
	if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
		if (likely(sd->sd_other == NULL))
			return (-EPIPE);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)))
			return (-EPIPE);
		if (unlikely(sd->sd_other->sd_writers == 0))
			return (-ESTRPIPE);
	}
	if (unlikely(signal_pending(current)))
		return ((*timeo == MAX_SCHEDULE_TIMEOUT) ? -ERESTARTSYS : -EINTR);
	if (unlikely(*timeo == 0))
		return ((file->f_flags & (O_NONBLOCK | O_NDELAY)) ? -EAGAIN : -ETIME);
	return (0);
}
static int check_wakeup_wr(struct file *file, struct stdata *sd, long *timeo)
{
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP | STWRERR))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag))) {
			if (!(sd->sd_flag & (STRISFIFO | STRISPIPE)))
				return (-ENXIO);
			send_sig_info(SIGPIPE, (struct siginfo *) 1, current);
			return (-EPIPE);
		}
		if (unlikely(test_bit(STWRERR_BIT, &sd->sd_flag))) {
			if (sd->sd_eropt & WERRNONPERSIST)
				clear_bit(STWRERR_BIT, &sd->sd_flag);
			return (sd->sd_werror);
		}
	}
	if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
		if (likely(sd->sd_other == NULL)) {
			send_sig_info(SIGPIPE, (struct siginfo *) 1, current);
			return (-EPIPE);
		}
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag))) {
			send_sig_info(SIGPIPE, (struct siginfo *) 1, current);
			return (-EPIPE);
		}
		if (unlikely(sd->sd_other->sd_readers == 0))
			return (-ESTRPIPE);
	}
	if (unlikely(signal_pending(current)))
		return ((*timeo == MAX_SCHEDULE_TIMEOUT) ? -ERESTARTSYS : -EINTR);
	if (unlikely(*timeo == 0))
		return ((file->f_flags & (O_NONBLOCK | O_NDELAY)) ? -EAGAIN : -ETIME);
	return (0);
}
static int check_wakeup_oc(struct file *file, struct stdata *sd, long *timeo)
{
	if (unlikely(sd->sd_flag & (STPLEX | STRCLOSE | STRHUP | STRDERR | STWRERR))) {
		if (unlikely(test_bit(STPLEX_BIT, &sd->sd_flag)))
			return (-EINVAL);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_flag)))
			return (-EIO);
		if (unlikely(test_bit(STRHUP_BIT, &sd->sd_flag)))
			return (!(sd->sd_flag & (STRISFIFO | STRISPIPE)) ? -ENXIO : -EPIPE);
		if (unlikely(test_bit(STRDERR_BIT, &sd->sd_flag) && (file->f_mode & FREAD))) {
			if (sd->sd_eropt & RERRNONPERSIST)
				clear_bit(STRDERR_BIT, &sd->sd_flag);
			return (sd->sd_rerror);
		}
		if (unlikely(test_bit(STWRERR_BIT, &sd->sd_flag) && (file->f_mode & FWRITE))) {
			if (sd->sd_eropt & WERRNONPERSIST)
				clear_bit(STWRERR_BIT, &sd->sd_flag);
			return (sd->sd_rerror);
		}
	}
	if (sd->sd_flag & (STRISFIFO | STRISPIPE)) {
		if (likely(sd->sd_other == NULL))
			return (-EPIPE);
		if (unlikely(test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag)))
			return (-EPIPE);
	}
	if (unlikely(signal_pending(current)))
		return ((*timeo == MAX_SCHEDULE_TIMEOUT) ? -ERESTARTSYS : -EINTR);
	if (unlikely(*timeo == 0))
		return ((file->f_flags & (O_NONBLOCK | O_NDELAY)) ? -EAGAIN : -ETIME);
	return (0);
}

/* wait for flow control on a band */
static int strwaitband(struct file *file, struct stdata *sd, int band, long *timeo)
{
	int err = 0;
	if (!bcanputnext(sd->sd_wq, band)) {
		/* wait for band to become available */
		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue(&sd->sd_waitq, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			set_bit(WSLEEP_BIT, &sd->sd_flag);
			if ((err = check_wakeup_wr(file, sd, timeo)))
				break;
			if (bcanputnext(sd->sd_wq, band))
				break;
			*timeo = schedule_timeout(*timeo);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sd->sd_waitq, &wait);
	}
	return (err);
}

static mblk_t *strgetq(struct stdata *sd, int flags, int band)
{
	if (flags == MSG_HIPRI) {
		/* M_PCPROTO is the only high priority messsage on the queue and there can only be
		   one of them. (And we do not expect another...) */
		if (test_and_clear_bit(STRPRI_BIT, &sd->sd_flag))
			return (getq(sd->sd_rq));
		return (NULL);
	} else {
		mblk_t *mp;
		queue_t *q = sd->sd_rq;
	      get_another:
		if (test_bit(STRPRI_BIT, &sd->sd_flag))
			return (ERR_PTR(-EBADMSG));
		if ((mp = getq(q))) {
			/* process signals now */
			if (mp->b_datap->db_type == M_SIG) {
				if (mp->b_rptr[0] == SIGPOLL)
					kill_fasync(&sd->sd_siglist, SIGPOLL, POLL_MSG);
				/* otherwise we would send the signal if it were a control
				   terminal; however, streams devices are never control terminals
				   in Linux */
				freemsg(mp);
				goto get_another;
			}
			if (flags == MSG_HIPRI && mp->b_datap->db_type == M_PCPROTO)
				return (mp);
			if (flags == MSG_ANY && mp->b_datap->db_type != M_PASSFP)
				return (mp);
			if ((1 << mp->b_datap->db_type) & ((1 << M_PROTO) | (1 << M_DATA)) &&
			    mp->b_band >= band)
				return (mp);
			putbq(q, mp);
			return (ERR_PTR(-EBADMSG));
		}
		/* need M_READ? */
		if (test_bit(SNDMREAD_BIT, &sd->sd_flag))
			if (!putnextctl(sd->sd_wq, M_READ))
				return (ERR_PTR(-ENOSR));
		return (mp);
	}
}

 /* wait to get a message of a given type */
static mblk_t *strwaitgmsg(struct file *file, struct stdata *sd, int flags, int band)
{
	mblk_t *mp;
	/* maybe we'll get lucky... */
	/* also we must make an attempt before returning ENXIO, EPIPE or rderror */
	if ((mp = strgetq(sd, flags, band)))
		return (mp);
	/* only here it there's nothing left on the queue */
	{
		DECLARE_WAITQUEUE(wait, current);
		long timeo = file->f_flags & (O_NONBLOCK | O_NDELAY) ? 0 : MAX_SCHEDULE_TIMEOUT;
		add_wait_queue(&sd->sd_waitq, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			set_bit(RSLEEP_BIT, &sd->sd_flag);
			if (IS_ERR(mp = ERR_PTR(check_wakeup_rd(file, sd, &timeo))))
				break;
			if ((mp = strgetq(sd, flags, band)))
				break;
			timeo = schedule_timeout(timeo);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sd->sd_waitq, &wait);
		return (mp);
	}
}

/* wait to call open on a stream head */
static void strwakeopen(struct file *file, struct stdata *sd)
{
	/* release open bit */
	clear_bit(STWOPEN_BIT, &sd->sd_flag);
	/* wake up anybody waiting on open bit */
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
}
static int strwaitopen(struct file *file, struct stdata *sd)
{
	int err = 0;
	int locked, frozen;
	if ((err = check_stream_oc(file, sd)) == 0 &&
	    ((locked = test_and_set_bit(STWOPEN_BIT, &sd->sd_flag)) ||
	     (frozen = test_bit(STFROZEN_BIT, &sd->sd_flag)))) {
		DECLARE_WAITQUEUE(wait, current);
		long timeo = file->f_flags & (O_NONBLOCK | O_NDELAY) ? 0 : MAX_SCHEDULE_TIMEOUT;
		add_wait_queue(&sd->sd_waitq, &wait);
		if (locked) {
			/* Wait for the STWOPEN bit.  Only one open can be performed on a stream at 
			   a time. See Magic Garden. */
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				if ((err = check_wakeup_oc(file, sd, &timeo)))
					break;
				if (!test_and_set_bit(STWOPEN_BIT, &sd->sd_flag))
					break;
				schedule();
			}
		}
		if (test_bit(STFROZEN_BIT, &sd->sd_flag)) {
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				if ((err = check_wakeup_oc(file, sd, &timeo)))
					break;
				if (!test_bit(STFROZEN_BIT, &sd->sd_flag))
					break;
				schedule();
			}
			if (err)
				strwakeopen(file, sd);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sd->sd_waitq, &wait);
	}
	return err;
}

/* routines for handling ioctls passed as M_IOCTL */
static int strsendioctl(struct file *file, int cmd, cred_t *crp, mblk_t *dp, size_t dlen,
			long *timeo, int *rval)
{
	struct stdata *sd = stri_lookup(file);
	union ioctypes *ioc;
	mblk_t *mp;
	int err;
	ulong type;
	err = -ENOSR;
	if (!(mp = allocb(sizeof(*ioc), BPRI_MED)))
		goto freedp_and_out;
	mp->b_datap->db_type = M_IOCTL;
	/* use caller supplied data message block and length */
	linkb(mp, dp);
	ioc = (typeof(ioc)) mp->b_wptr++;
	bzero(ioc, sizeof(*ioc));
	ioc->iocblk.ioc_cmd = cmd;
	ioc->iocblk.ioc_cr = crp;
	ioc->iocblk.ioc_id = ++sd->sd_iocid;
	ioc->iocblk.ioc_count = dlen;
	/* we wait here for the iocwait bit */
	if (test_and_set_bit(IOCWAIT_BIT, &sd->sd_flag)) {
		/* Wait for the IOCWAIT bit.  Only one ioctl can be performed on a stream at a
		   time.  See Magic Garden. */
		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue(&sd->sd_waitq, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if ((err = check_wakeup_io(file, sd, timeo)))
				break;
			if (!test_and_set_bit(IOCWAIT_BIT, &sd->sd_flag))
				break;
			*timeo = schedule_timeout(*timeo);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sd->sd_waitq, &wait);
		if (err)
			return (err);
	}
	/* here we are holding the IOCWAIT bit */
	for (;;) {
		putnext(sd->sd_wq, mp);
		if (!(mp = xchg(&sd->sd_iocblk, NULL))) {
			/* We are waiting for a response to our downwards ioctl message.  Wait
			   until the message arrives or the io check fails. */
			DECLARE_WAITQUEUE(wait, current);
			add_wait_queue(&sd->sd_waitq, &wait);
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				if ((err = check_wakeup_io(file, sd, timeo)))
					break;
				if ((mp = xchg(&sd->sd_iocblk, NULL)))
					break;
				*timeo = schedule_timeout(*timeo);
			}
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&sd->sd_waitq, &wait);
			if (err)
				break;
		}
		/* now we have a response message block */
		switch ((type = mp->b_datap->db_type)) {
			union ioctypes *ioc;
		case M_IOCACK:
		case M_IOCNAK:
			ioc = (typeof(ioc)) mp->b_rptr;
			*rval = ioc->iocblk.ioc_rval;
			err = ioc->iocblk.ioc_error;
			err = err > 0 ? -err : err;
			freemsg(mp);
			break;
		case M_COPYIN:
		case M_COPYOUT:
		{
			char *ptr;
			size_t len;
			ioc = (typeof(ioc)) mp->b_rptr;
			ptr = ioc->copyreq.cq_addr;
			len = ioc->copyreq.cq_size;
			mp->b_datap->db_type = M_IOCDATA;
			mp->b_wptr = mp->b_rptr + sizeof(ioc->copyresp);
			if (type == M_COPYIN) {
				if ((err = verify_area(VERIFY_READ, ptr, len)))
					goto ioc_error;
				/* always get a free data buffer */
				if ((dp = xchg(&mp->b_cont, NULL)))
					freemsg(dp);
				err = -ENOSR;
				/* use the one we just freed if possible */
				if (!(dp = allocb(len, BPRI_HI)))
					goto ioc_error;
				dp->b_datap->db_type = M_DATA;
				bcopy(ptr, dp->b_wptr, len);
				dp->b_wptr += len;
			} else {
				if ((err = verify_area(VERIFY_WRITE, ptr, len)))
					goto ioc_error;
				err = -EIO;
				if (!(dp = mp->b_cont))
					goto ioc_error;
				err = -EMSGSIZE;
				if (dp->b_wptr < dp->b_rptr + len)
					goto ioc_error;
				bcopy(dp->b_rptr, ptr, len);
			}
			ioc->copyresp.cp_rval = (caddr_t) 0;
			continue;
		      ioc_error:
			ioc->copyresp.cp_rval = (caddr_t) -err;
			continue;
		}
		default:
			swerr();
			freemsg(mp);
			err = -EIO;
			break;
		}
		break;
	}
	if (sd->sd_iocblk)
		freemsg(xchg(&sd->sd_iocblk, NULL));
	/* release the IOCWAIT bit */
	clear_bit(IOCWAIT_BIT, &sd->sd_flag);
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
	return (err);
      freedp_and_out:
	freemsg(dp);
	return PTR_ERR(mp);
}

/* 
 *  I_ATMARK
 *  -----------------------------------
 */
static int str_i_atmark(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err = 0;
	queue_t *q = sd->sd_rq;
	qrlock(q, &flags);
	switch (arg) {
	case ANYMARK:
		if (q->q_first && q->q_first->b_flag & MSGMARK)
			err = 1;
		break;
	case LASTMARK:
		if (q->q_first && q->q_first->b_flag & MSGMARK) {
			err = 1;
			if (q->q_first->b_next && q->q_first->b_next->b_flag & MSGMARK)
				err = 0;
		}
		break;
	default:
		err = -EINVAL;
		break;
	}
	qrunlock(q, &flags);
	return (err);
}

/* 
 *  I_CANPUT
 *  -----------------------------------
 */
static int str_i_canput(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = -EINVAL;
	if (arg < 256 || arg == ANYBAND)
		if (!(err = check_stream_wr(file, sd)))
			return (bcanput(sd->sd_wq, arg) ? 1 : 0);
	return (err);
}

/* 
 *  I_CKBAND
 *  -----------------------------------
 */
static int str_i_ckband(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = -EINVAL;
	if (arg < 256 || arg == ANYBAND)
		if (!(err = check_stream_rd(file, sd)) || (err == -ENXIO) || (err = -EPIPE))
			return (bcanget(sd->sd_rq, arg) ? 1 : 0);
	return (err);
}

/* 
 *  I_FDINSERT
 *  -----------------------------------
 */
static int str_i_fdinsert(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	mblk_t *mp;
	struct strfdinsert *fdi = (typeof(fdi)) arg;
	struct stdata *sd2;
	struct file *f2;
	if ((err = verify_area(VERIFY_READ, fdi, sizeof(*fdi))) < 0)
		goto exit;
	if ((err = check_stream_wr(file, sd)) < 0)
		goto exit;
	if (fdi->offset < 0 || fdi->ctlbuf.len < fdi->offset + sizeof(queue_t *) ||
	    (fdi->flags != 0 && fdi->flags != RS_HIPRI))
		goto einval;
	if ((err = verify_area(VERIFY_READ, fdi->ctlbuf.buf, fdi->ctlbuf.len)) < 0)
		goto einval;
	if ((err = verify_area(VERIFY_READ, fdi->databuf.buf, fdi->databuf.len)) < 0)
		goto einval;
	if (fdi->ctlbuf.len > sysctl_str_strctlsz)
		goto erange;
	{
		long timeo;
		if (fdi->databuf.len >= 0) {
			queue_t *wq;
			if (fdi->databuf.len > sysctl_str_strmsgsz)
				goto erange;
			wq = sd->sd_wq->q_next;
			if (wq->q_minpsz > fdi->databuf.len || fdi->databuf.len > wq->q_maxpsz)
				goto erange;
		}
		if (!(f2 = fget(fdi->fildes)))
			goto einval;
		if (!(sd2 = stri_lookup(f2)))
			goto put_einval;
		timeo = (file->f_flags & (O_NONBLOCK | O_NDELAY)) ? 0 : MAX_SCHEDULE_TIMEOUT;
		if ((err = strwaitband(file, stri_lookup(file), 0, &timeo)) < 0)
			goto put_exit;
		if (!(mp = allocb(fdi->ctlbuf.len, BPRI_MED)))
			goto enosr;
		mp->b_datap->db_type = (fdi->flags == RS_HIPRI) ? M_PCPROTO : M_PROTO;
		if (fdi->databuf.len >= 0) {
			mblk_t *dp;
			if (!(dp = allocb(fdi->databuf.len, BPRI_MED)))
				goto free_enosr;
			dp->b_datap->db_type = M_DATA;
			bcopy(fdi->databuf.buf, dp->b_wptr, fdi->databuf.len);
			dp->b_wptr += fdi->databuf.len;
			linkb(mp, dp);
		}
		bcopy(fdi->ctlbuf.buf, mp->b_wptr, fdi->ctlbuf.len);
		mp->b_wptr += fdi->ctlbuf.len;
		bcopy(&sd2->sd_rq, mp->b_rptr + fdi->offset, sizeof(sd2->sd_rq));
		putnext(sd->sd_wq, mp);
	}
      put_exit:
	fput(f2);
      exit:
	return (err);
      free_enosr:
	freemsg(mp);
      enosr:
	err = -ENOSR;
	goto put_exit;
      put_einval:
	err = -EINVAL;
	goto put_exit;
      einval:
	err = -EINVAL;
	goto exit;
      erange:
	err = -ERANGE;
	goto exit;
}

/* 
 *  I_FIND
 *  -----------------------------------
 */
static int str_i_find(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	const char *name = (typeof(name)) arg;
	queue_t *wq;
	int err;
	err = -EFAULT;
	if (strnlen_user(name, FMNAMESZ + 1)) {
		err = 0;
		srlock(sd);
		for (wq = sd->sd_wq; wq; wq = SAMESTR(wq) ? wq->q_next : NULL) {
			const char *idname = wq->q_qinfo->qi_minfo->mi_idname;
			if (!strncmp(idname, name, FMNAMESZ + 1))
				err = 1;
		}
		srunlock(sd);
	}
	return (err);
}

/* 
 *  I_FLUSHBAND
 *  -----------------------------------
 */
static int str_i_flushband(struct file *file, struct stdata *sd, unsigned int cmd,
			   unsigned long arg)
{
	int err;
	mblk_t *mp;
	struct bandinfo *bi = (struct bandinfo *) arg;
	if ((err = verify_area(VERIFY_READ, bi, sizeof(*bi))))
		return (err);
	if (bi->bi_flag & ~(FLUSHR | FLUSHW | FLUSHRW))
		return (-EINVAL);
	if ((err = check_stream_io(file, sd)))
		return (err);
	if (!(mp = allocb(2, BPRI_MED)))
		return (-ENOSR);
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = bi->bi_flag | FLUSHBAND;
	*mp->b_wptr++ = bi->bi_pri;
	mp->b_flag |= MSGNOLOOP;
	putnext(sd->sd_wq, mp);
	return (0);
}

/* 
 *  I_FLUSH
 *  -----------------------------------
 */
static int str_i_flush(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	mblk_t *mp;
	if (arg & ~(FLUSHR | FLUSHW | FLUSHRW))
		return (-EINVAL);
	if ((err = check_stream_io(file, sd)))
		return (err);
	if (!(mp = allocb(1, BPRI_MED)))
		return (-ENOSR);
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = arg;
	mp->b_flag |= MSGNOLOOP;
	putnext(sd->sd_wq, mp);
	return (0);
}

/* 
 *  I_GETBAND
 *  -----------------------------------
 */
static int str_i_getband(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err;
	int *valp = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_rd(file, sd)) && (err != -ENXIO) && (err != -EPIPE))
		return (err);
	qrlock(sd->sd_rq, &flags);
	do {
		err = -ENODATA;
		if (!(sd->sd_rq->q_first))
			break;
		err = sd->sd_rq->q_first->b_band;
	} while (0);
	qrunlock(sd->sd_rq, &flags);
	if (err == -ENODATA)
		return (err);
	bcopy(&err, valp, sizeof(*valp));
	return (0);
}

/* 
 *  I_GETCLTIME
 *  -----------------------------------
 */
static int str_i_getcltime(struct file *file, struct stdata *sd, unsigned int cmd,
			   unsigned long arg)
{
	int err;
	long closetime, *valp = (long *) arg;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_wr(file, sd)))
		return (err);
	closetime = sd->sd_closetime;
	bcopy(&closetime, valp, sizeof(*valp));
	return (0);
}

/* 
 *  I_SETCLTIME
 *  -----------------------------------
 */
static int str_i_setcltime(struct file *file, struct stdata *sd, unsigned int cmd,
			   unsigned long arg)
{
	int err;
	long closetime, *valp = (long *) arg;
	if ((err = verify_area(VERIFY_READ, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_io(file, sd)))
		return (err);
	bcopy(valp, &closetime, sizeof(*valp));
	sd->sd_closetime = closetime;
	return (0);
}

/* 
 *  I_GETSIG
 *  -----------------------------------
 */
static int str_i_getsig(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, flags, *valp = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_io(file, sd)))
		return (err);
	flags = sd->sd_sigflags;
	bcopy(&flags, valp, sizeof(*valp));
	return (0);
}

/* 
 *  I_SETSIG
 *  -----------------------------------
 */
static int str_i_setsig(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	if ((err = check_stream_io(file, sd)))
		return (err);
	sd->sd_sigflags = arg;
	return (0);
}

/* 
 *  I_GRDOPT
 *  -----------------------------------
 */
static int str_i_grdopt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, rdopt, *valp = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if ((err = check_stream_rd(file, sd)) && (err != -ENXIO) && (err != -EPIPE))
		return (err);
	rdopt = sd->sd_rdopt & RPROTMASK;
	bcopy(&rdopt, valp, sizeof(*valp));
	return (0);
}

/* 
 *  I_SRDOPT
 *  -----------------------------------
 */
static int str_i_srdopt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, mode, prot;
	mode = arg & (RNORM | RMSGD | RMSGN);
	prot = arg & (RPROTNORM | RPROTDAT | RPROTDIS);
	if ((arg & ~(RPROTMASK)) || (mode != RNORM && mode != RMSGD && mode != RMSGN)
	    || (prot != RPROTNORM && prot != RPROTDAT && prot != RPROTDIS))
		return (-EINVAL);
	if ((err = check_stream_rd(file, sd)) && (err != -ENXIO) && (err != -EPIPE))
		return (err);
	sd->sd_rdopt = arg;
	return (0);
}

/* 
 *  I_GWROPT
 *  -----------------------------------
 */
static int str_i_gwropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, wropt, *valp = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if ((err = check_stream_wr(file, sd)) < 0)
		return (err);
	wropt = sd->sd_wropt & (SNDZERO | SNDPIPE | SNDHOLD);
	bcopy(&wropt, valp, sizeof(*valp));
	return (0);
}

/* 
 *  I_SWROPT
 *  -----------------------------------
 */
static int str_i_swropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	if ((arg & ~(SNDZERO | SNDPIPE | SNDHOLD)))
		return (-EINVAL);
	if ((err = check_stream_wr(file, sd)) < 0)
		return (err);
	sd->sd_wropt = arg;
	if (sd->sd_wropt & SNDHOLD)
		set_bit(STRHOLD_BIT, &sd->sd_flag);
	else
		clear_bit(STRHOLD_BIT, &sd->sd_flag);
	return (0);
}

/* 
 *  I_LINK
 *  -----------------------------------
 */
static int str_send_link_ioctl(struct file *file, int cmd, cred_t *crp, queue_t *qtop,
			       queue_t *qbot, int index)
{
	int err, rval;
	struct linkblk *lbp;
	mblk_t *mp;
	err = -ENOSR;
	if (!(mp = allocb(sizeof(*lbp), BPRI_MED)))
		goto error;
	lbp = (struct linkblk *) mp->b_wptr++;
	bzero(lbp, sizeof(*lbp));
	lbp->l_qtop = qtop;
	lbp->l_qbot = qbot;
	lbp->l_index = index;
	{
		long timeo = (file->f_flags & (O_NONBLOCK | O_NDELAY)) ? 0 : MAX_SCHEDULE_TIMEOUT;
		err = strsendioctl(file, cmd, crp, mp, sizeof(*lbp), &timeo, &rval);
	}
	if (err < 0)
		goto error;
	return (0);
      error:
	return (err);
}

static int str_i_xlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct file *f2;
	struct stdata *sd2, **sdp;
	struct linkblk *l;
	queue_t *qtop, *qbot;
	int index;
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	err = -EINVAL;
	if (!sd->sd_strtab || !sd->sd_strtab->st_muxrinit || !sd->sd_strtab->st_muxwinit)
		goto error;
	err = -EBADF;
	if (!(f2 = fget(arg)))
		goto error;
	err = -EINVAL;
	if (!(sd2 = stri_lookup(f2)))
		goto fput_error;
	if ((err = check_stream_io(f2, sd2)) < 0)
		goto fput_error;
	err = -ENOMEM;
	if (!(l = alloclk()))
		goto fput_error;
	index = l->l_index;
	err = -EINVAL;
	if (test_and_set_bit(STPLEX_BIT, &sd2->sd_flag))
		goto free_error;
	qbot = sd2->sd_wq;
	setq(qbot - 1, sd->sd_strtab->st_muxrinit, sd->sd_strtab->st_muxwinit);
	qbot->q_ptr = (qbot - 1)->q_ptr = NULL;
	for (qtop = sd->sd_wq; qtop && SAMESTR(qtop); qtop = qtop->q_next) ;
	if ((err = str_send_link_ioctl(file, cmd, current_creds, qtop, qbot, index)) < 0)
		goto resetq_error;
	l->l_qtop = qtop;
	l->l_qbot = qbot;
	sdp = (cmd = I_LINK) ?  &sd->sd_links : &sd->sd_cdevsw->d_plinks;
	sd2->sd_linkblk = l;
	sd2->sd_link_next = xchg(sdp, sd2);
	/* simulate an open */
	sd2->sd_opens++;
	sd_get(sd2);
	fput(f2);
	return (index);
      resetq_error:
	/* set it back to a stream head */
	setq(qbot - 1, &str_rinit, &str_winit);
	qbot->q_ptr = (qbot - 1)->q_ptr = sd2;
	clear_bit(STPLEX_BIT, &sd2->sd_flag);
      free_error:
	freelk(l);
      fput_error:
	fput(f2);
      error:
	return (err);
}

static int str_i_link(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	return str_i_xlink(file, sd, cmd, arg);
}

/* 
 *  I_PLINK
 *  -----------------------------------
 */
static int str_i_plink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	return str_i_xlink(file, sd, cmd, arg);
}

/* 
 *  I_UNLINK
 *  -----------------------------------
 */
static int str_i_xunlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	struct stdata **sdp;
	int err;
	long index = arg;
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	sdp = (cmd == I_UNLINK) ?  &sd->sd_links : &sd->sd_cdevsw->d_plinks;
	if (index != MUXID_ALL) {
		queue_t *qtop, *qbot;
		struct stdata *sd2;
		for (; *sdp && (*sdp)->sd_linkblk->l_index != index; sdp = &(*sdp)->sd_link_next) ;
		err = -EINVAL;
		if ((sd2 = *sdp)) {
			qbot = sd2->sd_wq;
			for (qtop = sd->sd_wq; qtop && SAMESTR(qtop); qtop = qtop->q_next) ;
			if ((err = str_send_link_ioctl(file, cmd, current_creds,
						       qtop, qbot, index)) < 0)
				goto error;
			*sdp = xchg(&sd2->sd_link_next, NULL);
			freelk(xchg(&sd2->sd_linkblk, NULL));
			clear_bit(STPLEX_BIT, &sd2->sd_flag);
			if (--sd2->sd_opens == 0)
				/* last close */
				qclose(sd2->sd_rq, make_oflag(file), current_creds);
			sd_put(sd2);	/* coule be final put */
		}
	} else {
		queue_t *qtop, *qbot;
		struct stdata *sd2;
		for (qtop = sd->sd_wq; qtop && SAMESTR(qtop); qtop = qtop->q_next) ;
		while ((sd2 = *sdp)) {
			index = sd2->sd_linkblk->l_index;
			qbot = sd2->sd_wq;
			if ((err = str_send_link_ioctl(file, cmd, current_creds,
						       qtop, qbot, index)) < 0)
				goto error;
			*sdp = xchg(&sd2->sd_link_next, NULL);
			freelk(xchg(&sd2->sd_linkblk, NULL));
			clear_bit(STPLEX_BIT, &sd2->sd_flag);
			if (--sd2->sd_opens == 0)
				/* last close */
				qclose(sd2->sd_rq, make_oflag(file), current_creds);
			sd_put(sd2);	/* coule be final put */
		}
	}
      error:
	return (err);
}
static int str_i_unlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	return str_i_xunlink(file, sd, cmd, arg);
}

/* 
 *  I_PUNLINK
 *  -----------------------------------
 */
static int str_i_punlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	return str_i_xunlink(file, sd, cmd, arg);
}

/* 
 *  I_LOOK
 *  -----------------------------------
 */
static int str_i_list(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, nmods, i;
	struct str_list *sl = (struct str_list *) arg;
	struct str_mlist *sm;
	struct queue **qp;
	nmods = sd->sd_pushcnt + 1;
	if (sl == NULL)
		/* return number of modules and drivers */
		return (nmods);
	if ((err = verify_area(VERIFY_WRITE, sl, sizeof(*sl))) < 0)
		return (err);
	nmods = sl->sl_nmods < nmods ? sl->sl_nmods : nmods;
	if (nmods < 0 || (sm = sl->sl_modlist) == NULL)
		return (-EINVAL);
	if ((err = verify_area(VERIFY_WRITE, sm, sizeof(*sm) * nmods)) < 0)
		return (err);
	for (qp = &sd->sd_wq->q_next, i = 0; *qp && i < nmods; qp = &(*qp)->q_next, i++, sm++)
		snprintf(sm->l_name, FMNAMESZ + 1, (*qp)->q_qinfo->qi_minfo->mi_idname);
	return (0);
}

/* 
 *  I_LOOK
 *  -----------------------------------
 */
static int str_i_look(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	queue_t *q;
	char *name = (char *) arg;
	if ((err = verify_area(VERIFY_WRITE, (void *) arg, FMNAMESZ + 1)))
		goto exit;
	srlock(sd);
	err = -EINVAL;
	if (!(q = sd->sd_wq->q_next) || !SAMESTR(sd->sd_wq))
		goto unlock_exit;
	snprintf(name, FMNAMESZ + 1, q->q_qinfo->qi_minfo->mi_idname);
	return (err);
      unlock_exit:
	srunlock(sd);
      exit:
	return (err);
}

/* 
 *  I_NREAD
 *  -----------------------------------
 */
static int str_i_nread(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err, bytes, *valp = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0 ||
	    ((err = check_stream_rd(file, sd)) && err != ENXIO && err != -EPIPE))
		goto exit;
	qrlock(sd->sd_rq, &flags);
	err = qsize(sd->sd_rq);
	if (sd->sd_rq->q_first)
		bytes = msgdsize(sd->sd_rq->q_first);
	qrunlock(sd->sd_rq, &flags);
	bcopy(&bytes, valp, sizeof(*valp));
      exit:
	return (err);
}

/* 
 *  I_PEEK
 *  -----------------------------------
 */
static int str_i_peek(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err, rtn = 0;
	struct strpeek *sp;
	int cmax, dmax;
	queue_t *q = sd->sd_rq;
	caddr_t cbuf, dbuf;
	if (!(sp = (struct strpeek *) arg))
		return (-EINVAL);
	if ((err = verify_area(VERIFY_WRITE, sp, sizeof(*sp))) < 0)
		return (err);
	cmax = sp->ctlbuf.maxlen;
	dmax = sp->databuf.maxlen;
	cbuf = sp->ctlbuf.buf;
	dbuf = sp->databuf.buf;
	if (cmax < 0 || dmax < 0 || (cmax && !cbuf) || (dmax && !dbuf))
		return (-EINVAL);
	if (cmax && (err = verify_area(VERIFY_WRITE, cbuf, cmax)) < 0)
		return (err);
	if (dmax && (err = verify_area(VERIFY_WRITE, dbuf, dmax)) < 0)
		return (err);
	/* all areas verified, check readable */
	if ((err = check_stream_rd(file, sd)) && err != -ENXIO && err != -EPIPE)
		return (err);
	qrlock(q, &flags);
	do {
		mblk_t *mp, *dp;
		ssize_t blen;
		ssize_t clen = sp->ctlbuf.maxlen;
		ssize_t dlen = sp->databuf.maxlen;
		ssize_t davail, cavail;
		if (!(mp = q->q_first)) {
			err = 0;
			rtn = 0;
			break;
		}
		davail = msgdsize(mp);	/* M_DATA blocks */
		cavail = msgsize(mp) - davail;	/* isddatablks - M_DATA blocks */
		sp->flags = mp->b_datap->db_type > QPCTL ? RS_HIPRI : 0;
		sp->ctlbuf.len = 0;
		sp->databuf.len = 0;
		for (dp = mp; dp; dp = dp->b_cont) {
			if ((blen = dp->b_wptr - dp->b_rptr) <= 0)
				continue;
			if (dp->b_datap->db_type == M_DATA) {
				/* goes in data part */
				if (dlen == -1)
					break;
				if (blen > dlen) {
					bcopy(dp->b_rptr, sp->databuf.buf + sp->databuf.len, dlen);
					sp->databuf.len += dlen;
					dlen = 0;
					break;
				}
				bcopy(dp->b_rptr, sp->databuf.buf + sp->databuf.len, blen);
				sp->databuf.len += blen;
				dlen -= blen;
			} else {
				/* goes in ctrl part */
				if (clen == -1)
					break;
				if (blen > clen) {
					bcopy(dp->b_rptr, sp->ctlbuf.buf + sp->ctlbuf.len, clen);
					sp->ctlbuf.len += clen;
					clen = 0;
					break;
				}
				bcopy(dp->b_rptr, sp->ctlbuf.buf + sp->ctlbuf.len, blen);
				sp->ctlbuf.len += blen;
				clen -= blen;
			}
		}
		rtn = 1;
	} while (0);
	qrunlock(q, &flags);
	if (err < 0)
		return (err);
	return (rtn);
}

/* 
 *  I_PUSH
 *  -----------------------------------
 */
static int str_i_push(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	char name[FMNAMESZ + 1];
	dev_t dev;
	if ((err = strncpy_from_user(name, (const char *) arg, FMNAMESZ + 1)) < 0)
		goto error;
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	dev = kdev_t_to_nr(sd->sd_inode->i_rdev);
	if ((err = qpush(sd, name, &dev, make_oflag(file), current_creds)) < 0)
		goto error;
      error:
	return (err);
}

/* 
 *  I_POP
 *  -----------------------------------
 */
static int str_i_pop(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	dev_t dev;
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	dev = kdev_t_to_nr(sd->sd_inode->i_rdev);
	if ((err = qpop(sd, make_oflag(file), current_creds)) < 0)
		goto error;
      error:
	return (err);
}

/* 
 *  I_SENDFD
 *  -----------------------------------
 */
static void freefd_func(caddr_t arg)
{
	struct file *file = (struct file *) arg;
	/* sneaky trick to free the file pointer when mblk freed, this means that M_PASSFP messages 
	   flushed from a queue will free the file pointers referenced by them */
	fput(file);
	return;
}
static int str_i_sendfd(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct file *f2;
	mblk_t *mp;
	queue_t *rq;
	if (!(f2 = fget(arg)))
		goto ebadf;
	srlock(sd);
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	rq = sd->sd_other->sd_rq;
	if (!canput(rq))
		goto eagain;
	else {
		frtn_t freefd = { freefd_func, (caddr_t) f2 };
		if (!(mp = esballoc((void *) f2, sizeof(*f2), BPRI_MED, &freefd)))
			goto enosr;
	}
	mp->b_datap->db_type = M_PASSFP;
	mp->b_wptr += sizeof(*f2);
	putq(rq, mp);
	srunlock(sd);
	return (0);
      enosr:
	err = (-ENOSR);
	goto error;
      eagain:
	err = (-EAGAIN);
      error:
	fput(f2);
	srunlock(sd);
	return (err);
      ebadf:
	return (-EBADF);
}

/**
 *  I_RECVFD - receive file descriptor
 *  @file:file pointer of current open stream
 *  @sd:stream head of current open stream
 *  @arg:pointer to a &struct strrecvfd structure
 */
static int str_i_recvfd(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	struct strrecvfd *sr;
	mblk_t *mp;
	int fd, err;
	struct file *f2;
	if ((sr = (struct strrecvfd *) arg) == NULL)
		goto einval;
	if ((err = verify_area(VERIFY_WRITE, sr, sizeof(*sr))) < 0)
		goto error;
	/* yes, we return ENXIO on hangup */
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	if ((err = fd = get_unused_fd()) < 0)
		goto error;
	if (test_bit(STRPRI_BIT, &sd->sd_flag))
		goto pri_msg;
	if ((mp = getq(sd->sd_rq)) == NULL)
		goto eagain;
	if (mp->b_datap->db_type != M_PASSFP)
		goto ebadmsg;
	/* we now have a M_PASSFP message in mp */
	f2 = (struct file *) mp->b_rptr;
	/* we need to do another get because an fput will be done when the mblk is freed */
	get_file(f2);
	fd_install(fd, f2);
	sr->fd = fd;
	sr->uid = f2->f_uid;
	sr->gid = f2->f_gid;
	freemsg(mp);
	return (0);
      ebadmsg:
	err = (-EBADMSG);
	goto putbq_error;
      eagain:
	err = (-EAGAIN);
	goto put_fd_error;
      pri_msg:
	err = (-EBADMSG);
	goto put_fd_error;
      einval:
	err = (-EINVAL);
	goto error;
      putbq_error:
	putbq(sd->sd_rq, mp);
      put_fd_error:
	put_unused_fd(fd);
      error:
	return (err);
}

/* 
 *  I_STR
 *  -----------------------------------
 */
static int str_i_str(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, rval = 0;
	mblk_t *mp;
	struct strioctl *ic = (typeof(ic)) arg;
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	if ((err = verify_area(VERIFY_READ, ic, sizeof(*ic))) < 0)
		goto error;
	if ((err = verify_area(VERIFY_READ, ic->ic_dp, ic->ic_len)) < 0)
		goto error;
	err = -ENOSR;
	if (!(mp = allocb(ic->ic_len, BPRI_MED)))
		goto error;
	bcopy(ic->ic_dp, mp->b_wptr, ic->ic_len);
	mp->b_wptr += ic->ic_len;
	{
		long timeo = (file->f_flags & (O_NONBLOCK | O_NDELAY)) ? 0 : MAX_SCHEDULE_TIMEOUT;
		timeo = ic->ic_timout == -1 ? timeo : drv_msectohz(ic->ic_timout);
		err = strsendioctl(file, I_STR, current_creds, mp, ic->ic_len, &timeo, &rval);
	}
	if (err < 0)
		goto error;
	return (rval);
      error:
	return (err);
}

/* 
 *  I_GERROPT
 *  -----------------------------------
 */
static int str_i_gerropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, eropt, *valp = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if ((err = check_stream_io(file, sd)) < 0)
		return (err);
	eropt = sd->sd_eropt & (RERRNONPERSIST | WERRNONPERSIST);
	bcopy(&eropt, valp, sizeof(*valp));
	return (0);
}

/* 
 *  I_SERROPT
 *  -----------------------------------
 */
static int str_i_serropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	if ((arg & ~(RERRNONPERSIST | WERRNONPERSIST)))
		return (-EINVAL);
	if ((err = check_stream_io(file, sd)) < 0)
		return (err);
	sd->sd_eropt = arg;
	return (0);
}

/* 
 *  I_ANCHOR
 *  -----------------------------------
 */
static int str_i_anchor(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	if ((err = check_stream_io(file, sd)) < 0)
		return (err);
	sd->sd_nanchor = sd->sd_pushcnt;
	return (0);
}

/* 
 *  Transparent IOCTL
 *  -----------------------------------
 */
static int str_i_transparent(struct file *file, struct stdata *sd, unsigned int cmd,
			     unsigned long arg)
{
	int err, rval;
	mblk_t *mp;
	err = -ENOSR;
	if (!(mp = allocb(sizeof(arg), BPRI_MED)))
		goto error;
	*((unsigned long *) mp->b_wptr)++ = arg;
	{
		long timeo = (file->f_flags & (O_NONBLOCK | O_NDELAY)) ? 0 : MAX_SCHEDULE_TIMEOUT;
		err = strsendioctl(file, cmd, current_creds, mp, TRANSPARENT, &timeo, &rval);
	}
	if (err < 0)
		goto error;
	return (rval);
      error:
	return (err);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  FILE OPERATIONS
 *
 *  -------------------------------------------------------------------------
 */

/* 
 *  STRLSEEK
 *  -------------------------------------------------------------------------
 */
loff_t strllseek(struct file *file, loff_t off, int whence)
{
	return (-ESPIPE);
}

/* 
 *  STRPOLL
 *  -------------------------------------------------------------------------
 */
unsigned int strpoll(struct file *file, struct poll_table_struct *poll)
{
	struct stdata *sd;
	unsigned int mask;
	sd = stri_lookup(file);
	mask = 0;
	poll_wait(file, &sd->sd_waitq, poll);
	if (test_bit(STRDERR_BIT, &sd->sd_flag) || test_bit(STWRERR_BIT, &sd->sd_flag))
		mask |= POLLERR;
	if (test_bit(STRHUP_BIT, &sd->sd_flag))
		mask |= POLLHUP;
	if (test_bit(STRPRI_BIT, &sd->sd_flag))
		mask |= POLLPRI;
	if (test_bit(STRMSIG_BIT, &sd->sd_flag))
		mask |= POLLMSG;
	if (bcanget(sd->sd_rq, 0))
		mask |= POLLIN | POLLRDNORM;
	if (bcanget(sd->sd_rq, ANYBAND))
		mask |= POLLIN | POLLRDBAND;
	if (bcanput(sd->sd_wq, 0))
		mask |= POLLOUT | POLLWRNORM;
	if (bcanput(sd->sd_wq, ANYBAND))
		mask |= POLLOUT | POLLWRBAND;
	if (sd->sd_directio && sd->sd_directio->poll)
		mask |= sd->sd_directio->poll(file, poll);
	return (mask);
}

/* 
 *  STRMMAP
 *  -------------------------------------------------------------------------
 */
int strmmap(struct file *filp, struct vm_area_struct *vma)
{
	return (-ENODEV);
}

/* 
 *  STROPEN
 *  -------------------------------------------------------------------------
 */
int autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct apinfo *api;
	int err;
	if ((api = (typeof(api)) autopush_find(*devp)) != NULL) {
		int k;
		for (k = 0; k < MAX_APUSH; k++) {
			struct fmodsw *fmod;
			dev_t dev;
			if (api->api_sap.sap_list[k][0] == 0)
				break;
			if (!(fmod = smod_get(api->api_sap.sap_list[k]))) {
				err = -EIO;
				goto abort_autopush;
			}
			dev = *devp;	/* don't change dev nr */
			if (fmod->f_str == NULL) {
				smod_put(fmod);
				err = -EIO;
				goto abort_autopush;
			}
			if ((err = qattach(sd, fmod, &dev, oflag, sflag, crp))) {
				smod_put(fmod);
				goto abort_autopush;
			}
			atomic_inc(&fmod->f_count);
		}
	}
	return (0);
      abort_autopush:
	{
		/* detach everything, including the driver */
		queue_t *wq = sd->sd_wq;
		if (wq)
			while (wq->q_next && SAMESTR(wq))
				qdetach(wq->q_next - 1, oflag, crp);
		return (err);
	}
}

 /* Because we are called by spec_open on the filesystem device, the inode represented here is the
    shadow inode and the file is the shadow file pointer. We don't have to do any swapping, just
    return the error code. Because it is a specfs inode, the dentry has our d_fsdata. */
int stropen(struct inode *inode, struct file *file)
{
	struct str_args *argp = file->f_dentry->d_fsdata;
	int err = 0;
	struct stdata *sd;
	/* first find out if we already have a stream head */
	if (!(sd = sd_get((struct stdata *)inode->i_pipe))) {
		queue_t *q;
		struct cdevsw *cdev;
		/* we don't have a stream yet (or want a new one), so allocate one */
		if (!(cdev = sdev_get(argp->dev)))
			return (-ENXIO);
		if (!(q = allocq())) {
			sdev_put(cdev);
			return (-ENOSR);
		}
		if (!(sd = allocsd())) {
			sdev_put(cdev);
			freeq(q);
			return (-ENOSR);
		}
		/* initialization of the stream head */
		sd->sd_flag = STWOPEN;	/* hold open bit */
		sd->sd_rq = q;
		sd->sd_wq = q + 1;
		((struct queinfo *) q)->qu_str = sd;
		switch (cdev->d_mode & S_IFMT) {
		case S_IFIFO:
			sd->sd_flag |= ((cdev->d_flag & D_CLONE) ? STRISPIPE : STRISFIFO);
			sd->sd_wropt = SNDZERO | SNDPIPE;	/* special write ops */
			break;
		case S_IFSOCK:
			sd->sd_flag |= STRISSOCK;
			sd->sd_wropt = SNDZERO | SNDPIPE;	/* special write ops */
			break;
		default:
			swerr();
		case S_IFCHR:
			sd->sd_wropt = 0;	/* default write ops */
			break;
		}
		if (cdev->d_flag & D_CLONE)
			sd->sd_flag |= STRCLONE;
		sd->sd_strtab = cdev->d_str;	/* stream head streamtab */
		setq(q, cdev->d_str->st_rdinit, cdev->d_str->st_wrinit);
		sd->sd_strtab = &str_info;	/* no driver yet */
		setq(q, str_info.st_rdinit, str_info.st_wrinit);
		/* grafting onto inode */
		sd->sd_inode = inode;	/* shadow inode */
		inode->i_pipe = (void *)sd;
		stri_lookup(file) = (void *) sd;
		/* done setup, do the open */
	} else
		err = strwaitopen(file, sd);
	if (!err) {
		/* here we hold the STWOPEN bit - stream head open must clear */
		argp->sflag = (sd->sd_flag & STRCLONE) ? CLONEOPEN : argp->sflag;
		if (!(err = qopen(sd->sd_rq, &argp->dev, argp->oflag, argp->sflag, argp->crp))) {
			sd->sd_opens++;
			sd->sd_readers += (file->f_mode & FREAD) ? 1 : 0;
			sd->sd_writers += (file->f_mode & FWRITE) ? 1 : 0;
			strwakeopen(file, sd);	/* wake up anybody waiting on open bit */
			return (0);
		}
		strwakeopen(file, sd);	/* wake up anybody waiting on open bit */
	}
	sd_put(sd);
	return (err <= 0 ? err : -err);
}

/* 
 *  STRFLUSH
 *  -------------------------------------------------------------------------
 *  sys_close() calls filp_close() that calls f_op->flush().
 */
int strflush(struct file *file)
{
	return (0);
}

/* 
 *  STRCLOSE
 *  -------------------------------------------------------------------------
 *  This function is called by fput() on final put of a file pointer
 *  associated with the file.  After this function returns a dput() will be
 *  peformed on the dentry and a mntput() on the mount.
 *
 *  When the last close is about to occur we will get a d_delete() operation
 *  followed by a d_iput() operation if special dops are assigned to the
 *  dentry.  The d_iput() chains or passes to an iput() which uses a superblock
 *  put_inode() operation followed by a superblock delete_inode() operation.
 *
 *  The return value from this function is isgnored.
 *
 */
int strclose(struct inode *inode, struct file *file)
{
	struct stdata *sd;
	if ((sd = sd_get(stri_lookup(file)))) {
		int err = 0;
		stri_lookup(file) = NULL;
		sd->sd_readers -= (file->f_mode & FREAD) ? 1 : 0;
		sd->sd_writers -= (file->f_mode & FWRITE) ? 1 : 0;
		if (--sd->sd_opens == 0) {
			queue_t *wq, *qq;
			int oflag = make_oflag(file);
			cred_t *crp = current_creds;
			/* last close */
			if (!test_and_set_bit(STRCLOSE_BIT, &sd->sd_flag))
				goto put_exit;
			if (test_bit(STPLEX_BIT, &sd->sd_flag))
				goto put_exit;
			/* 1st step: unlink any (temporary) linked streams */
			err = str_i_unlink(file, sd, I_UNLINK, MUXID_ALL);
			/* 2nd step: call the close routine of each module and pop the module. */
			/* 3rd step: call the close routine of the driver and qdetach the driver */
			wq = sd->sd_wq;
			while ((qq = wq->q_next) && SAMESTR(wq)) {
				if ((oflag & (O_NONBLOCK | O_NDELAY)) && qq->q_msgs) {
					struct queinfo *qu = (typeof(qu)) (qq - 1);
					DECLARE_WAITQUEUE(wait, current);
					long timeo = sd->sd_closetime;
					add_wait_queue(&qu->qu_qwait, &wait);
					for (;;) {
						set_current_state(TASK_INTERRUPTIBLE);
						if ((err = check_wakeup_io(file, sd, &timeo)))
							break;
						if (!qq->q_msgs)
							break;
						timeo = schedule_timeout(timeo);
					}
					set_current_state(TASK_RUNNING);
					remove_wait_queue(&qu->qu_qwait, &wait);
				}
				qdetach(qq - 1, oflag, crp);
			}
			qdetach(wq - 1, oflag, crp);
			sd->sd_rq = sd->sd_wq = NULL;
		}
	      put_exit:
		sd_put(sd);	/* could be final put */
		return (err);
	}
	return (-ENOSTR);
}

/* 
 *  -------------------------------------------------------------------------
 */
int strfasync(int fd, struct file *file, int on)
{
	int err;
	struct stdata *sd;
	sd = stri_lookup(file);
	if ((err = fasync_helper(fd, file, on, &sd->sd_siglist)) < 0)
		goto error;
	return (0);
      error:
	return (err);
}

/* 
 *  STRREADV
 *  -------------------------------------------------------------------------
 */
ssize_t strreadv(struct file *file, const struct iovec *iov, unsigned long len, loff_t *ppos)
{
	struct stdata *sd = stri_lookup(file);
	queue_t *q = sd->sd_rq;
	unsigned short msgdelim;
	size_t total = 0;
	mblk_t *mp;
	int err;
	if (ppos != &file->f_pos)
		return (-ESPIPE);
	if (len == 0)
		return (0);
	if ((err = check_stream_rd(file, sd))) {
		if (err == -ENXIO || err == -EPIPE)
			return (0);	/* end of file on hangup */
		return (err);
	}
	if (sd->sd_directio && sd->sd_directio->readv)
		return sd->sd_directio->readv(file, iov, len, ppos);
	for (;;) {
		if ((err = PTR_ERR(mp = strwaitgmsg(file, sd, MSG_BAND, 0))) < 0)
			return (err);
		switch (sd->sd_rdopt & (RPROTDAT | RPROTDIS | RPROTNORM)) {
		default:
		case RPROTNORM:
		{
			/* M_PROTO messages make read return EBADMSG error code (default).  Fail
			   read when control part present.  Fail read(2) with [EBADMSG] if a
			   message containing a control part is at the front of the stream head
			   read queue. */
			if (mp->b_datap->db_type != M_DATA) {
				if (mp->b_datap->db_type == M_PCPROTO)
					set_bit(STRPRI_BIT, &sd->sd_flag);
				putbq(q, mp);
				return (-EBADMSG);
			}
			break;
		}
		case RPROTDAT:
		{
			/* M_PROTO messages are converted to M_DATA messages, which means they are
			   read correctly. Deliver control part of a message as data.  The control
			   part of the message is prepended to the data part. */
			if (mp->b_datap->db_type != M_DATA) {
				mp->b_datap->db_type = M_DATA;
				mp->b_flag |= MSGDELIM;
			}
			break;
		}
		case RPROTDIS:
		{
			/* M_PROTO messages are discarded.  Discard control part of message,
			   delivering any data part.  The control part of the message is discarded
			   and the data part is processed. */
			mblk_t *dp, **mpp, **mpp_next = &mp;
			while ((dp = *(mpp = mpp_next))) {
				if (dp->b_datap->db_type != M_DATA) {
					*mpp = dp->b_cont;
					freeb(dp);
				} else
					mpp_next = &dp->b_cont;
			}
			if (!mp)
				continue;
			break;
		}
		}
		{
			size_t count = msgsize(mp);
			/* All read modes terminate on a zero length message. */
			if (!count) {
				freemsg(mp);
				if (!total)
					/* XXX there is some confusion here in the Magic Garden
					   descriptions. Returning zero from read when len > 0 in
					   POSIX always means end of file. We should not really
					   terminate on a zero-length message if nothing has been
					   read yet. */
					continue;
				break;
			}
			/* All read modes terminate once the request is satisfied. */
			if (count > len - total)
				count = len - total;
			msgdelim = mp->b_flag & MSGDELIM;
			while (mp && total < len) {
				/* FIXME: read the message */
			}
			if (mp)
				mp->b_flag |= msgdelim;
		}
		switch (sd->sd_rdopt & (RNORM | RMSGD | RMSGN)) {
		default:
		case RNORM:
		{
			/* the default mode.  This is the normal byte-stream mode where message
			   boundaries are ignored; strread() returns data until the read count has
			   been satisfied or a zero length message is received. */
			if (mp)
				putbq(q, mp);
			if (mp || total == len)
				break;
			continue;
		}
		case RMSGN:
		{
			/* the read will return when either the count is satisfied, a zero length
			   message is received, or a message boundary is encountered.  If there is
			   any data left in a message after the read count has been satisfied, the
			   message is placed back on the read queue.  The data will be read on a
			   subsequent read call. */
			if (mp)
				putbq(q, mp);
			if (mp || msgdelim || total == len)
				break;
			continue;
		}
		case RMSGD:
		{
			/* similar to RMSGN but data that remains in a message after the read count 
			   has been satisfied is discarded. */
			if (mp)
				freemsg(mp);
			if (mp || msgdelim || total == len)
				break;
			continue;
		}
		}
		break;
	}
	return (total);
}

/* 
 *  STRREAD
 *  -------------------------------------------------------------------------
 */
ssize_t strread(struct file *file, char *buf, size_t len, loff_t *ppos)
{
	struct iovec iov;
	struct stdata *sd = stri_lookup(file);
	if (sd->sd_directio && sd->sd_directio->read)
		return sd->sd_directio->read(file, buf, len, ppos);
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return strreadv(file, &iov, 1, ppos);
}

/* 
 *  STRWRITEV
 *  -------------------------------------------------------------------------
 */
ssize_t strwritev(struct file *file, const struct iovec *iov, unsigned long count, loff_t *ppos)
{
	int i;
	mblk_t *mp = NULL, *dp;
	struct stdata *sd = stri_lookup(file);
	size_t size = 0, total;
	long timeo;
	queue_t *wq;
	int err;
	if ((err = check_stream_wr(file, sd)))
		goto error;
	if (sd->sd_directio && sd->sd_directio->writev)
		return sd->sd_directio->writev(file, iov, count, ppos);
	for (total = 0, i = 0; i < count; total += iov[i].iov_len, i++) ;
	if (total > sysctl_str_strmsgsz)
		goto erange;
	wq = sd->sd_wq->q_next;
	if (wq->q_minpsz > total || total > wq->q_maxpsz)
		goto erange;
	timeo = file->f_flags & (O_NONBLOCK | O_NDELAY) ? 0 : MAX_SCHEDULE_TIMEOUT;
	if ((err = strwaitband(file, sd, 0, &timeo)))
		goto error;
	if (total > 0) {
		size_t wroff;
		for (i = 0, wroff = sd->sd_wroff; i < count; i++, wroff = 0) {
			size_t len;
			if ((len = iov[i].iov_len)) {
				if (!(dp = allocb(wroff + len, BPRI_MED)))
					goto abort;
				bcopy(iov[i].iov_base, dp->b_wptr, len);
				dp->b_rptr += wroff;
				dp->b_wptr += wroff + len;
				mp = linkmsg(mp, dp);
				size += len;
			}
		}
	} else {
		size_t wroff = sd->sd_wroff;
		if (!(sd->sd_wropt & SNDZERO))
			goto einval;
		if (!(dp = allocb(wroff + 0, BPRI_MED)))
			goto enosr;
		dp->b_rptr += wroff;
		dp->b_wptr += wroff;
		mp = linkmsg(mp, dp);
	}
	mp->b_flag |= MSGDELIM;
	putnext(sd->sd_wq, mp);
	return (size);
      abort:
	if (size) {
		putnext(sd->sd_wq, mp);
		return (size);
	}
      error:
	return (err);
      einval:
	return (-EINVAL);
      erange:
	return (-ERANGE);
      enosr:
	return (-ENOSR);
}

/* 
 *  STRWRITE
 *  -------------------------------------------------------------------------
 */
ssize_t strwrite(struct file *file, const char *buf, size_t len, loff_t *ppos)
{
	struct iovec iov;
	struct stdata *sd = stri_lookup(file);
	if (sd->sd_directio && sd->sd_directio->write)
		return sd->sd_directio->write(file, buf, len, ppos);
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return strwritev(file, &iov, 1, ppos);
}

/* 
 *  STRSENDPAGE
 *  -------------------------------------------------------------------------
 */
static void __strfreepage(caddr_t data)
{
	struct page *page = (struct page *) data;
	(void) page;		/* shut up compiler */
	kunmap(page);
}
static mblk_t *strwaitpage(struct file *file, size_t size, int prio, int band, int type,
			   caddr_t base, struct free_rtn *frtn, long *timeo)
{
	struct stdata *sd = stri_lookup(file);
	queue_t *wq = sd->sd_wq->q_next;
	mblk_t *mp;
	if (unlikely(size > sysctl_str_strmsgsz))
		return ERR_PTR(-ERANGE);
	if (type == M_DATA && (wq->q_minpsz > size || size > wq->q_maxpsz))
		return ERR_PTR(-ERANGE);
	if ((band != -1 && !bcanput(wq, band))) {
		/* wait for band to become available */
		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue(&sd->sd_waitq, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			set_bit(WSLEEP_BIT, &sd->sd_flag);
			if (IS_ERR(mp = ERR_PTR(check_wakeup_wr(file, sd, timeo))))
				break;
			if (bcanput(wq, band))
				break;
			*timeo = schedule_timeout(*timeo);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sd->sd_waitq, &wait);
		if (IS_ERR(mp))
			return (mp);
	}
	if ((mp = esballoc(base, size, prio, frtn))) {
		mp->b_datap->db_type = type;
		mp->b_band = (band == -1) ? 0 : band;
		return (mp);
	}
	return ERR_PTR(-ENOSR);
}

ssize_t strsendpage(struct file *file, struct page *page, int offset, size_t size, loff_t *ppos,
		    int more)
{
	mblk_t *mp;
	struct stdata *sd = stri_lookup(file);
	queue_t *q;
	if (sd->sd_directio && sd->sd_directio->sendpage)
		return sd->sd_directio->sendpage(file, page, offset, size, ppos, more);
	if (!(q = sd->sd_wq->q_next))
		goto espipe;
	if (q->q_minpsz > size || size > q->q_maxpsz || size > sysctl_str_strmsgsz)
		goto erange;
	if (ppos == &file->f_pos) {
		char *base = kmap(page) + offset;
		struct free_rtn frtn = { __strfreepage, (caddr_t) page };
		long timeo = file->f_flags & (O_NONBLOCK | O_NDELAY) ? 0 : MAX_SCHEDULE_TIMEOUT;
		mp = strwaitpage(file, size, BPRI_MED, 0, M_DATA, base, &frtn, &timeo);
		if (IS_ERR(mp)) {
			kunmap(page);
			return PTR_ERR(mp);
		}
		if (!more)
			mp->b_flag |= MSGDELIM;
		putnext(sd->sd_wq, mp);
		return (size);
	}
      erange:
	return (-ERANGE);
      espipe:
	return (-ESPIPE);
}

/* 
 *  STRPUTPMSG
 *  -------------------------------------------------------------------------
 *  GLIBC2 puts -1 in band when it is called as putmsg().
 */
int strputpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int band, int flags)
{
	struct stdata *sd = stri_lookup(file);
	mblk_t *mp = NULL, *dp;
	ssize_t clen, dlen;
	int err;
	if ((err = check_stream_wr(file, sd)))
		goto exit;
	if (band != -1) {
		if (flags != MSG_HIPRI && flags != MSG_BAND)
			goto einval;
	} else {
		if (flags != RS_HIPRI && flags != 0)	/* RS_NORM */
			goto einval;
	}
	clen = ctlp ? ctlp->len : -1;
	dlen = datp ? datp->len : -1;
	if ((band != -1 && flags == MSG_HIPRI) || (band == -1 && flags == RS_HIPRI)) {
		/* need an M_PCPROTO block and zero or more M_DATA message blocks */
		if (clen <= 0 || band != 0)
			goto einval;
	} else {
		/* need zero or one M_PROTO block and zero or more M_DATA message blocks */
		if ((band != -1 && (band < 0 || band > 255)) ||
		    (clen <= 0 && dlen <= 0 && !(sd->sd_wropt & SNDZERO)))
			goto einval;
	}
	if (clen > 0) {
		if (clen > sysctl_str_strctlsz)
			goto erange;
	}
	if (dlen >= 0) {
		queue_t *wq = sd->sd_wq->q_next;
		if (dlen > sysctl_str_strmsgsz)
			goto erange;
		if (wq->q_minpsz > dlen || dlen > wq->q_maxpsz)
			goto erange;
	}
	if ((band != -1 && flags == MSG_BAND) || (band == -1 && flags == 0)) {
		long timeo;
		timeo = file->f_flags & (O_NONBLOCK | O_NDELAY) ? 0 : MAX_SCHEDULE_TIMEOUT;
		if ((err = strwaitband(file, sd, band == -1 ? 0 : band, &timeo)))
			goto exit;
	}
	if (clen > 0) {
		if (!(dp = allocb(clen, BPRI_HI)))
			goto enosr;
		mp->b_datap->db_type = (flags == MSG_HIPRI) ? M_PCPROTO : M_PROTO;
		bcopy(ctlp->buf, dp->b_wptr, clen);
		dp->b_wptr += clen;
		mp = linkmsg(mp, dp);
	}
	if (dlen >= 0) {	/* zero-length data parts are allowed */
		size_t wroff = sd->sd_wroff;
		if (!(dp = allocb(wroff + dlen, BPRI_HI)))
			goto enosr;
		mp->b_datap->db_type = M_DATA;
		mp->b_rptr += wroff;
		mp->b_wptr += wroff;
		bcopy(datp->buf, dp->b_wptr, dlen);
		dp->b_wptr += dlen;
		mp = linkmsg(mp, dp);
		if (clen <= 0)
			mp->b_flag |= MSGDELIM;
	}
	putnext(sd->sd_wq, mp);
	return (0);
      einval:
	return (-EINVAL);
      erange:
	return (-ERANGE);
      enosr:
	err = -ENOSR;
	if (mp)
		freemsg(mp);
      exit:
	return (err);
}

/* 
 *  STRGETPMSG
 *  -------------------------------------------------------------------------
 *  Note: If a hangup occurs on a stream from which messages are to be
 *  retrieved, getpmsg() continues to operate normally, until the stream head
 *  read queue is empty.  Thereafter it returns 0 in the len fields of ctlptr
 *  and dataptr.
 *
 *  GLIBC2 puts NULL in bandp for getmsg().
 */
int strgetpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int *bandp, int *flagsp)
{
	struct stdata *sd = stri_lookup(file);
	mblk_t *mp = NULL, *dp;
	if (!flagsp)
		goto einval;
	ctlp->len = datp->len = 0;
	if (bandp) {
		/* getpmsg */
		switch (*flagsp) {
		case MSG_HIPRI:
			if (*bandp != 0)
				goto einval;
			break;
		case MSG_ANY:
			if (*bandp != 0)
				goto einval;
			break;
		case MSG_BAND:
			if (0 > *bandp || *bandp > 255)
				goto einval;
			break;
		default:
			goto einval;
		}
	} else {
		/* getmsg */
		switch (*flagsp) {
		case RS_HIPRI:
		case 0:	/* RS_NORM */
			break;
		default:
			goto einval;
		}
	}
	if (!IS_ERR(dp = ERR_PTR(check_stream_rd(file, sd))) &&
	    !IS_ERR(dp = strwaitgmsg(file, sd, *flagsp, bandp ? *bandp : 0))) {
		int rval = 0;
		ssize_t blen;
		ssize_t clen = ctlp ? ctlp->maxlen : -1;
		ssize_t dlen = datp ? datp->maxlen : -1;
		ssize_t davail = msgdsize(dp);	/* M_DATA blocks */
		ssize_t cavail = msgsize(dp) - davail;	/* isdatablks */
		if (cavail && cavail > clen)
			rval |= MORECTL;
		if (davail && davail > dlen)
			rval |= MOREDATA;
		if (bandp) {
			/* getpmsg */
			if (dp->b_datap->db_type > QPCTL)
				*flagsp = MSG_HIPRI;
			else {
				*flagsp = MSG_BAND;
				*bandp = dp->b_band;
			}
		} else {
			/* getmsg */
			if (dp->b_datap->db_type > QPCTL)
				*flagsp = RS_HIPRI;
			else
				*flagsp = 0;
		}
		for (mp = dp; (dp = mp); mp = unlinkb(dp), freeb(dp)) {
			if ((blen = dp->b_wptr - dp->b_rptr) <= 0)
				continue;
			if (dp->b_datap->db_type == M_DATA) {
				/* goes in data part */
				if (dlen == -1)
					break;
				if (blen > dlen) {
					bcopy(dp->b_rptr, datp->buf, dlen);
					dp->b_rptr += dlen;
					datp->len += dlen;
					dlen = 0;
					break;
				}
				bcopy(dp->b_rptr, datp->buf, blen);
				dp->b_rptr += blen;
				datp->len += blen;
				dlen -= blen;
			} else {
				/* goes in ctrl part */
				if (clen == -1)
					break;;
				if (blen > clen) {
					bcopy(dp->b_rptr, ctlp->buf, clen);
					dp->b_rptr += clen;
					ctlp->len += clen;
					clen = 0;
					break;
				}
				bcopy(dp->b_rptr, ctlp->buf, blen);
				dp->b_rptr += blen;
				ctlp->len += blen;
				clen -= blen;
			}
		}
		dp = ERR_PTR(rval);
	}
	if (mp) {
		/* mark if putting back high priority */
		if ((bandp && *flagsp == MSG_HIPRI) || (!bandp && *flagsp == RS_HIPRI))
			set_bit(STRPRI_BIT, &sd->sd_flag);
		putbq(sd->sd_rq, mp);
	}
	if (PTR_ERR(dp) == -ENXIO) {
		/* end of file on hangup */
		if (ctlp)
			ctlp->len = 0;
		if (datp)
			datp->len = 0;
		return (0);
	}
	return (PTR_ERR(dp));
      einval:
	return (-EINVAL);
}

/* 
 *  Linux Fast-STREAMS System Call Emulation
 *  ----------------------------------------
 */
static int str_i_putpmsg(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct {
		struct strbuf *ctl;
		struct strbuf *dat;
		int band;
		int flags;
	} *sp = (typeof(sp)) arg;
	/* verify all areas */
	if ((err = verify_area(VERIFY_READ, sp, sizeof(*sp))))
		goto error;
	if (sp->ctl) {
		if ((err = verify_area(VERIFY_READ, sp->ctl, sizeof(*sp->ctl))) < 0)
			goto error;
		if ((err = verify_area(VERIFY_READ, sp->ctl->buf, sp->ctl->len)) < 0)
			goto error;
	}
	if (sp->dat) {
		if ((err = verify_area(VERIFY_READ, sp->dat, sizeof(*sp->dat))) < 0)
			goto error;
		if ((err = verify_area(VERIFY_READ, sp->dat->buf, sp->dat->len)))
			goto error;
	}
	return strputpmsg(file, sp->ctl, sp->dat, sp->band, sp->flags);
      error:
	return (err);
}
static int str_i_getpmsg(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct {
		struct strbuf *ctl;
		struct strbuf *dat;
		int *band;
		int *flags;
	} *sg = (typeof(sg)) arg;
	/* verify all areas */
	if ((err = verify_area(VERIFY_READ, sg, sizeof(*sg))))
		goto error;
	if (sg->ctl) {
		if ((err = verify_area(VERIFY_WRITE, sg->ctl, sizeof(*sg->ctl))) < 0)
			goto error;
		if ((err = verify_area(VERIFY_WRITE, sg->ctl->buf, sg->ctl->maxlen)) < 0)
			goto error;
	}
	if (sg->dat) {
		if ((err = verify_area(VERIFY_WRITE, sg->dat, sizeof(*sg->dat))) < 0)
			goto error;
		if ((err = verify_area(VERIFY_WRITE, sg->dat->buf, sg->dat->maxlen)) < 0)
			goto error;
	}
	if ((err = verify_area(VERIFY_WRITE, sg->band, sizeof(*sg->band))))
		goto error;
	if ((err = verify_area(VERIFY_WRITE, sg->flags, sizeof(*sg->flags))))
		goto error;
	return strgetpmsg(file, sg->ctl, sg->dat, sg->band, sg->flags);
      error:
	return (err);
}
static int strfattach(struct file *file, const char *path)
{
	return do_fattach(file, path);
}
static int str_i_fattach(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	char path[256];
	if ((err = strncpy_from_user(path, (const char *) arg, 256)) < 0)
		goto error;
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	return strfattach(file, path);
      error:
	return (err);
}
static int strfdetach(const char *path)
{
	return do_fdetach(path);
}
static int str_i_fdetach(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	char path[256];
	if ((err = strncpy_from_user(path, (const char *) arg, 256)) < 0)
		goto error;
	return strfdetach(path);
      error:
	return (err);
}
static int strpipe(int fds[2])
{
	return (-ENOSYS);
}
static int str_i_pipe(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, *fds = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, fds, 2 * sizeof(*fds))))
		goto error;
	return strpipe(fds);
      error:
	return (err);
}

/* 
 *  STRIOCTL
 *  -------------------------------------------------------------------------
 */
int strioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct stdata *sd = stri_lookup(file);
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	(void) size;
	switch (cmd) {
	case FIOGETOWN:
	case FIOSETOWN:
	default:
		break;
	}
	switch (type) {
	case _IOC_TYPE(__SID):
		switch (nr) {
		case _IOC_NR(I_ATMARK):
			return str_i_atmark(file, sd, cmd, arg);
		case _IOC_NR(I_CANPUT):
			return str_i_canput(file, sd, cmd, arg);
		case _IOC_NR(I_CKBAND):
			return str_i_ckband(file, sd, cmd, arg);
		case _IOC_NR(I_FDINSERT):
			return str_i_fdinsert(file, sd, cmd, arg);
		case _IOC_NR(I_FIND):
			return str_i_find(file, sd, cmd, arg);
		case _IOC_NR(I_FLUSHBAND):
			return str_i_flushband(file, sd, cmd, arg);
		case _IOC_NR(I_FLUSH):
			return str_i_flush(file, sd, cmd, arg);
		case _IOC_NR(I_GETBAND):
			return str_i_getband(file, sd, cmd, arg);
		case _IOC_NR(I_GETCLTIME):
			return str_i_getcltime(file, sd, cmd, arg);
		case _IOC_NR(I_GETSIG):
			return str_i_getsig(file, sd, cmd, arg);
		case _IOC_NR(I_GRDOPT):
			return str_i_grdopt(file, sd, cmd, arg);
		case _IOC_NR(I_GWROPT):
			return str_i_gwropt(file, sd, cmd, arg);
		case _IOC_NR(I_LINK):
			return str_i_link(file, sd, cmd, arg);
		case _IOC_NR(I_LIST):
			return str_i_list(file, sd, cmd, arg);
		case _IOC_NR(I_LOOK):
			return str_i_look(file, sd, cmd, arg);
		case _IOC_NR(I_NREAD):
			return str_i_nread(file, sd, cmd, arg);
		case _IOC_NR(I_PEEK):
			return str_i_peek(file, sd, cmd, arg);
		case _IOC_NR(I_PLINK):
			return str_i_plink(file, sd, cmd, arg);
		case _IOC_NR(I_POP):
			return str_i_pop(file, sd, cmd, arg);
		case _IOC_NR(I_PUNLINK):
			return str_i_punlink(file, sd, cmd, arg);
		case _IOC_NR(I_PUSH):
			return str_i_push(file, sd, cmd, arg);
		case _IOC_NR(I_RECVFD):
			return str_i_recvfd(file, sd, cmd, arg);
		case _IOC_NR(I_SENDFD):
			return str_i_sendfd(file, sd, cmd, arg);
		case _IOC_NR(I_SETCLTIME):
			return str_i_setcltime(file, sd, cmd, arg);
		case _IOC_NR(I_SETSIG):
			return str_i_setsig(file, sd, cmd, arg);
		case _IOC_NR(I_SRDOPT):
			return str_i_srdopt(file, sd, cmd, arg);
		case _IOC_NR(I_STR):
			return str_i_str(file, sd, cmd, arg);
		case _IOC_NR(I_SWROPT):
			return str_i_swropt(file, sd, cmd, arg);
		case _IOC_NR(I_UNLINK):
			return str_i_unlink(file, sd, cmd, arg);
			/* are these Solaris specific? */
		case _IOC_NR(I_SERROPT):
			return str_i_serropt(file, sd, cmd, arg);
		case _IOC_NR(I_GERROPT):
			return str_i_gerropt(file, sd, cmd, arg);
		case _IOC_NR(I_ANCHOR):
			return str_i_anchor(file, sd, cmd, arg);
			/* Linux Fast-STREAMS special ioctls */
		case _IOC_NR(I_PUTPMSG):	/* putpmsg syscall emulation */
			return str_i_putpmsg(file, sd, cmd, arg);
		case _IOC_NR(I_GETPMSG):	/* getpmsg syscall emulation */
			return str_i_getpmsg(file, sd, cmd, arg);
		case _IOC_NR(I_FATTACH):	/* fattach syscall emulation */
			return str_i_fattach(file, sd, cmd, arg);
		case _IOC_NR(I_FDETACH):	/* fdetach syscall emulation */
			return str_i_fdetach(file, sd, cmd, arg);
		case _IOC_NR(I_PIPE):	/* pipe syscall emulation */
			return str_i_pipe(file, sd, cmd, arg);
		}
		break;
	default:
		break;
	}
	/* ioctl is not known to the stream head: formulate a non-transparent M_IOCTL and pass it
	   to the driver */
	return str_i_transparent(file, sd, cmd, arg);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  SRVP Write Service Procedure
 *
 *  -------------------------------------------------------------------------
 */
static int strwsrv(queue_t *q)
{
	/* we don't actually ever put a message on the write queue, we just use the service
	   procedure to wake up any synchronous or asynchronous waiters waiting for flow control to 
	   subside.  This permit back-enabling from the module beneath the stream head to work
	   properly. */
	struct stdata *sd = q->q_ptr;
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUTP Read Put Procedure
 *
 *  -------------------------------------------------------------------------
 */
static int strrput(queue_t *q, mblk_t *mp)
{
	struct stdata *sd = q->q_ptr;
	switch ((msg_type_t) mp->b_datap->db_type) {
	case M_PCPROTO:	/* bi - protocol info */
	{
		if (test_bit(STRPRI_BIT, &sd->sd_flag)) {
			freemsg(mp);
			return (0);
		}
		putq(q, mp);
		if (test_and_clear_bit(RSLEEP_BIT, &sd->sd_flag))
			wake_up_interruptible(&sd->sd_waitq);
		if (test_bit(S_HIPRI_BIT, &sd->sd_sigflags))
			kill_fasync(&sd->sd_siglist, SIGPOLL, POLL_PRI);
		return (0);
	}
	case M_DATA:		/* bi - data */
	case M_PROTO:		/* bi - protocol info */
	case M_PASSFP:		/* bi - pass file pointer */
	{
		putq(q, mp);
		if (test_and_clear_bit(RSLEEP_BIT, &sd->sd_flag))
			wake_up_interruptible(&sd->sd_waitq);
		if (test_bit(S_INPUT_BIT, &sd->sd_sigflags))
			kill_fasync(&sd->sd_siglist, SIGPOLL, POLL_IN);
		return (0);
	}
	case M_FLUSH:		/* bi - flush queues */
	{
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[1] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			mp->b_rptr[0] &= ~FLUSHR;
		}
		if (mp->b_flag & MSGNOLOOP) {
			freemsg(mp);
			return (0);
		}
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[1] & FLUSHBAND)
				flushband(WR(q), mp->b_rptr[1], FLUSHALL);
			else
				flushq(WR(q), FLUSHALL);
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
			return (0);
		}
		freemsg(mp);
		return (0);
	}
	case M_SETOPTS:	/* up - set stream head options */
	case M_PCSETOPTS:	/* up - set stream head options */
	{
		struct stroptions *so = (typeof(so)) mp->b_rptr;
		if (so->so_flags ^ (SO_MREADON | SO_MREADOFF)) {
			if (so->so_flags & SO_MREADON)
				set_bit(SNDMREAD_BIT, &sd->sd_flag);
			else if (so->so_flags & SO_MREADOFF)
				clear_bit(SNDMREAD_BIT, &sd->sd_flag);
		}
		if (so->so_flags ^ (SO_NDELON | SO_NDELOFF)) {
			if (so->so_flags & SO_NDELON)
				clear_bit(STRHOLD_BIT, &sd->sd_flag);
			else if (so->so_flags & SO_NDELOFF)
				set_bit(STRHOLD_BIT, &sd->sd_flag);
		}
		if (so->so_flags & SO_STRHOLD)
			set_bit(STRHOLD_BIT, &sd->sd_flag);
		if (so->so_flags ^ (SO_ISTTY | SO_ISNTTY)) {
			if (so->so_flags & SO_ISTTY)
				set_bit(STRISTTY_BIT, &sd->sd_flag);
			else if (so->so_flags & SO_ISNTTY)
				clear_bit(STRISTTY_BIT, &sd->sd_flag);
		}
		if (so->so_flags ^ (SO_TOSTOP | SO_TONSTOP)) {
			if (so->so_flags & SO_TOSTOP)
				set_bit(STRTOSTOP_BIT, &sd->sd_flag);
			if (so->so_flags & SO_TONSTOP)
				clear_bit(STRTOSTOP_BIT, &sd->sd_flag);
		}
		if (so->so_flags ^ (SO_DELIM | SO_NODELIM)) {
			if (so->so_flags & SO_DELIM)
				set_bit(STRDELIM_BIT, &sd->sd_flag);
			if (so->so_flags & SO_NODELIM)
				clear_bit(STRDELIM_BIT, &sd->sd_flag);
		}
		if (so->so_flags & SO_READOPT)
			sd->sd_rdopt = so->so_flags & RPROTMASK;
		if (so->so_flags & SO_WROFF)
			sd->sd_wroff = so->so_wroff;
		if (so->so_flags & SO_MINPSZ)
			strqset(q, QMINPSZ, 0, so->so_minpsz);
		if (so->so_flags & SO_MAXPSZ)
			strqset(q, QMAXPSZ, 0, so->so_maxpsz);
		if (so->so_flags & SO_BAND) {
			if (so->so_flags & SO_HIWAT)
				strqset(q, QHIWAT, so->so_band, so->so_minpsz);
			if (so->so_flags & SO_LOWAT)
				strqset(q, QLOWAT, so->so_band, so->so_minpsz);
		} else {
			if (so->so_flags & SO_HIWAT)
				strqset(q, QHIWAT, 0, so->so_minpsz);
			if (so->so_flags & SO_LOWAT)
				strqset(q, QLOWAT, 0, so->so_minpsz);
		}
		return (0);
	}
	case M_SIG:		/* up - signal */
	{
		putq(q, mp);
		return (0);
	}
	case M_PCSIG:		/* up - signal */
	{
		if (mp->b_rptr[0] == SIGPOLL)
			kill_fasync(&sd->sd_siglist, SIGPOLL, POLL_MSG);
		/* otherwise we would send the signal if it were a control terminal; however,
		   streams devices are never control terminals in Linux */
		freemsg(mp);
		return (0);
	}
	case M_ERROR:		/* up - report error */
	{
		int what = 0;
		if (mp->b_rptr[0] != (typeof(mp->b_rptr[0])) NOERROR) {
			sd->sd_rerror = mp->b_rptr[0];
			if (test_and_set_bit(STRDERR_BIT, &sd->sd_flag)) {
				if (test_and_clear_bit(RSLEEP_BIT, &sd->sd_flag))
					wake_up_interruptible(&sd->sd_waitq);
				what |= FLUSHR;
			}
		} else {
			clear_bit(STRDERR_BIT, &sd->sd_flag);
			sd->sd_rerror = NOERROR;
		}
		if (mp->b_rptr[1] != (typeof(mp->b_rptr[1])) NOERROR) {
			sd->sd_werror = mp->b_rptr[1];
			if (test_and_set_bit(STWRERR_BIT, &sd->sd_flag)) {
				if (test_and_clear_bit(WSLEEP_BIT, &sd->sd_flag))
					wake_up_interruptible(&sd->sd_waitq);
				what |= FLUSHW;
			}
		} else {
			clear_bit(STWRERR_BIT, &sd->sd_flag);
			sd->sd_werror = NOERROR;
		}
		/* send a flush if required */
		if (what) {
			mp->b_datap->db_type = M_FLUSH;
			mp->b_band = 0;
			mp->b_rptr[0] = what;
			mp->b_rptr[1] = 0;
			mp->b_flag |= MSGNOLOOP;
			qreply(q, mp);
			if (test_bit(S_ERROR_BIT, &sd->sd_sigflags))
				kill_fasync(&sd->sd_siglist, SIGPOLL, POLL_ERR);
			return (0);
		}
		freemsg(mp);
		return (0);
	}
	case M_HANGUP:		/* up - report hangup */
	{
		if (test_and_set_bit(STRHUP_BIT, &sd->sd_flag)) {
			if (test_and_clear_bit(RSLEEP_BIT, &sd->sd_flag))
				wake_up_interruptible(&sd->sd_waitq);
			if (test_and_clear_bit(WSLEEP_BIT, &sd->sd_flag))
				wake_up_interruptible(&sd->sd_waitq);
			if (test_bit(S_HANGUP_BIT, &sd->sd_sigflags))
				kill_fasync(&sd->sd_siglist, SIGPOLL, POLL_HUP);
		}
		freemsg(mp);
		return (0);
	}
	case M_UNHANGUP:	/* up - report recovery */
	{
		if (test_and_clear_bit(STRHUP_BIT, &sd->sd_flag)) {
			if (test_bit(S_HANGUP_BIT, &sd->sd_sigflags))
				kill_fasync(&sd->sd_siglist, SIGPOLL, POLL_MSG);
		}
		freemsg(mp);
		return (0);
	}
	case M_COPYIN:		/* up - copy data from user */
	case M_COPYOUT:	/* up - copy data to user */
	{
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
		if (!sd->sd_iocblk && test_bit(IOCWAIT_BIT, &sd->sd_flag) &&
		    !(sd->sd_flag & (STRDERR | STWRERR))
		    && ioc->copyreq.cq_id == sd->sd_iocid && waitqueue_active(&sd->sd_waitq)) {
			sd->sd_iocblk = mp;
			wake_up_interruptible(&sd->sd_waitq);
			return (0);
		}
		mp->b_datap->db_type = M_IOCDATA;
		mp->b_wptr = mp->b_rptr + sizeof(ioc->copyresp);
		ioc->copyresp.cp_rval = (caddr_t) EPERM;
		qreply(q, mp);
		return (0);
	}
	case M_IOCACK:		/* up - acknolwedge ioctl */
	case M_IOCNAK:		/* up - refuse ioctl */
	{
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
		if (!sd->sd_iocblk && test_bit(IOCWAIT_BIT, &sd->sd_flag) &&
		    !(sd->sd_flag & (STRDERR | STWRERR))
		    && ioc->iocblk.ioc_id == sd->sd_iocid && waitqueue_active(&sd->sd_waitq)) {
			sd->sd_iocblk = mp;
			wake_up_interruptible(&sd->sd_waitq);
			return (0);
		}
		freemsg(mp);
		return (0);
	}
	case M_IOCTL:		/* dn - io control */
	{
		union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_error = EOPNOTSUPP;
		ioc->iocblk.ioc_rval = -1;
		qreply(q, mp);
		return (0);
	}
	case M_LETSPLAY:	/* up - AIX only */
	{
		struct strlp *lp = (typeof(lp)) mp->b_rptr;
		if (lp->lp_count == sd->sd_pushcnt) {
			/* we don't queue data in the stream head write queue */
			mp->b_datap->db_type = M_BACKDONE;
			qreply(q, mp);
			return (0);
		} else {
			mp->b_datap->db_type = M_DONTPLAY;
			putq(lp->lp_queue, mp);
			return (0);
		}
	}
	default:
	case M_BREAK:		/* dn - request to send "break" */
	case M_DELAY:		/* dn - request delay on output */
	case M_IOCDATA:	/* dn - copy data response */
	case M_READ:		/* dn - read notification */
	case M_STOP:		/* dn - suspend output */
	case M_START:		/* dn - resume output */
	case M_STARTI:		/* dn - resume input */
	case M_STOPI:		/* dn - suspend input */
	case M_BACKWASH:	/* dn - backwash clearing */
	case M_BACKDONE:	/* dn - backwash clearning done */
	case M_DONTPLAY:	/* dn - direct io not permitted */
	case M_CTL:		/* bi - control info */
	case M_PCCTL:		/* bi - control info */
	case M_EVENT:		/* -- - Solaris only? */
	case M_PCEVENT:	/* -- - Solaris only? */
	case M_RSE:		/* -- - reserved */
	case M_PCRSE:		/* -- - reserved */
		/* Other messages are silently discarded */
		freemsg(mp);
		return (0);
	}
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static int str_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int err;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		/* we walk down the queue chain calling open on each of the modules and the driver */
		queue_t *wq = WR(q), *wq_next;
		wq_next = SAMESTR(wq) ? wq->q_next : NULL;
		while ((wq = wq_next)) {
			int new_sflag;
			wq_next = SAMESTR(wq) ? wq->q_next : NULL;
			new_sflag = wq_next ? MODOPEN : sflag;
			if ((err = qopen(wq - 1, devp, oflag, new_sflag, crp))) {
				MOD_DEC_USE_COUNT;
				return (err > 0 ? -err : err);
			}
		}
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == DRVOPEN || sflag == CLONEOPEN || WR(q)->q_next == NULL) {
		dev_t dev = *devp;
		struct stdata *sd;
		if ((sd = ((struct queinfo *) q)->qu_str)) {
			struct char_device *cd;
			struct cdevsw *sdev;
			struct streamtab *st;
			cd = sd->sd_inode->i_cdev;
			sdev = sd->sd_cdevsw;
			/* 1st step: attach the driver and call its open routine */
			st = sd->sd_strtab = sdev->d_str;
			if ((err = qattach(sd, (struct fmodsw *) sdev, &dev, oflag, sflag, crp))) {
				MOD_DEC_USE_COUNT;
				return (err > 0 ? -err : err);
			}
			/* 2nd step: check for redirected return */
			if (dev != *devp) {
				// struct char_device *cd_old = cd;
				struct cdevsw *sdev_old = sdev;
				struct streamtab *st_old = st;
				if ((cd = cdget(dev)) == NULL)
					goto done_check;
				if ((sdev = sdev_get(dev)) == NULL)
					goto done_check;
				if (sdev == sdev_old)
					goto put_done_check;
				if ((st = sd->sd_strtab = sdev->d_str) == st_old)
					goto put_done_check;
				setq(sd->sd_rq, st->st_rdinit, st->st_wrinit);
				/* need to handle situation where cdevsw entry is different and a
				   new syncq structure is required as well. */
			      put_done_check:
				sdev_put(sdev);
			      done_check:
				/* always rehash the inode */
				remove_inode_hash(sd->sd_inode);
				sd->sd_inode->i_ino = to_kdev_t(dev);
				sd->sd_inode->i_rdev = to_kdev_t(dev);
				insert_inode_hash(sd->sd_inode);
			}
			/* 3rd step: autopush modules and call their open routines */
			if ((err = autopush(sd, sdev, devp, oflag, MODOPEN, crp))) {
				MOD_DEC_USE_COUNT;
				return (err > 0 ? -err : err);
			}
			/* lastly, attach our privates and return */
			q->q_ptr = WR(q)->q_ptr = sd;
			sd->sd_opens++;	/* first successful open */
			return (0);
		}
	}
	MOD_DEC_USE_COUNT;
	return (-EIO);		/* can't be opened as module or clone */
}
static int str_close(queue_t *q, int oflag, cred_t *crp)
{
	if (!q->q_ptr || q->q_ptr != ((struct queinfo *) q)->qu_str)
		return (ENXIO);
	q->q_ptr = WR(q)->q_ptr = NULL;
	MOD_DEC_USE_COUNT;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */
struct file_operations strm_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	llseek:strllseek,
	read:strread,
	write:strwrite,
	poll:strpoll,
	ioctl:strioctl,
	mmap:strmmap,
	open:stropen,
	flush:strflush,
	release:strclose,
	fasync:strfasync,
	readv:strreadv,
	writev:strwritev,
	sendpage:strsendpage,
//	getpmsg:strgetpmsg,
//	putpmsg:strputpmsg,
};

/* Note: we do not register stream heads - the above operations are exported and used by all
   regular STREAMS devices */

/* Perhaps we should...  The stream head should be entered into the fmodsw table at position 0,
   with module id 0 */
