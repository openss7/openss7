/*****************************************************************************

 @(#) $RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2005/07/12 14:06:22 $

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

 Last Modified $Date: 2005/07/12 14:06:22 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2005/07/12 14:06:22 $"

static char const ident[] =
    "$RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2005/07/12 14:06:22 $";

//#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h>	/* for FASTCALL() */
#include <linux/sched.h>	/* for send_sig_info() */
#include <linux/spinlock.h>
#include <linux/fs.h>		/* for file */
#include <linux/file.h>		/* for fget() */
#include <linux/poll.h>		/* for poll_wait */
#include <linux/highmem.h>	/* for kmap, kunmap */
#include <linux/uio.h>		/* for iovec */

#include <asm/sockios.h>	/* for FIOCGETOWN, etc. */

#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/kernel/strsched.h"	/* for allocstr */
#include "src/kernel/strreg.h"	/* for spec_open() */
#include "src/kernel/strlookup.h"	/* for cmin_get() */
#include "sth.h"		/* extern verification */
#include "src/kernel/strsysctl.h"	/* for sysctls */
#include "src/kernel/strsad.h"	/* for autopush */
#include "src/kernel/strutil.h"	/* for q locking and puts and gets */
#include "src/kernel/strattach.h"	/* for do_fattach/do_fdetach */
#include "src/kernel/strpipe.h"	/* for do_spipe */
#include "src/drivers/clone.h"	/* for (un)register_clone() */

#define STH_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define STH_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define STH_REVISION	"LfS $RCSfile: sth.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2005/07/12 14:06:22 $"
#define STH_DEVICE	"SVR 4.2 STREAMS STH Module"
#define STH_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define STH_LICENSE	"GPL"
#define STH_BANNER	STH_DESCRIP	"\n" \
			STH_COPYRIGHT	"\n" \
			STH_REVISION	"\n" \
			STH_DEVICE	"\n" \
			STH_CONTACT	"\n"
#define STH_SPLASH	STH_DEVICE	" - " \
			STH_REVISION	"\n"

#ifdef CONFIG_STREAMS_STH_MODULE
MODULE_AUTHOR(STH_CONTACT);
MODULE_DESCRIPTION(STH_DESCRIP);
MODULE_SUPPORTED_DEVICE(STH_DEVICE);
MODULE_LICENSE(STH_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sth");
#endif
#endif				/* CONFIG_STREAMS_STH_MODULE */

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
//#define CONFIG_STREAMS_STH_NAME "sth"
#error "CONFIG_STREAMS_STH_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_STH_MODID
//#define CONFIG_STREAMS_STH_MODID 0
#error "CONFIG_STREAMS_STH_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_STH_MODID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module identification number for STH module.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_STH_MODID));
MODULE_ALIAS("streams-module-sth");
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

int strrput(queue_t *q, mblk_t *mp);
int strwsrv(queue_t *q);

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

#define stri_lookup(__f) ((__f)->private_data)

#undef verify_area
#define verify_area(__x,__y,__z) (access_ok((__x),(__y),(__z)) ? 0 : -EFAULT)

/* 
 *  -------------------------------------------------------------------------
 *
 *  STREAMS IO Controls
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  check_stream_io: - checks if io can be performed on a stream
 *  @file: user file pointer
 *  @sd: stream head pointer
 */
static int check_stream_io(struct file *file, struct stdata *sd)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
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

/**
 *  check_stream_rd: - checks if read operation can be performed on a stream
 *  @file: user file pointer
 *  @sd: stream head pointer
 */
static int check_stream_rd(struct file *file, struct stdata *sd)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
	if (!(file->f_mode & FREAD))
		return (-EBADF);
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

/**
 *  check_stream_wr: - checks if write operation can be performed on a stream
 *  @file: user file pointer
 *  @sd: stream head
 */
static int check_stream_wr(struct file *file, struct stdata *sd)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
	if (!(file->f_mode & FWRITE))
		return (-EBADF);
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

/**
 *  check_stream_oc: - check if open or close can be performed on a stream
 *  @file: user file pointer
 *  @sd: stream head
 */
static int check_stream_oc(struct file *file, struct stdata *sd)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
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

/**
 *  check_wakeup_io: - wait to perform io operations on a stream
 *  @file: user file pointer
 *  @sd: stream head
 */
static int check_wakeup_io(struct file *file, struct stdata *sd, long *timeo)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
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

/**
 *  check_wakeup_rd: - wait to perform read operation on a stream
 *  @file: user file pointer
 *  @sd: stream head
 */
static int check_wakeup_rd(struct file *file, struct stdata *sd, long *timeo)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
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

/**
 *  check_wakeup_wr: - wait to perform write operation on a stream
 *  @file: user file pointer
 *  @sd: stream head
 */
static int check_wakeup_wr(struct file *file, struct stdata *sd, long *timeo)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
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

/**
 *  check_wakeup_oc: - wait to perform open or close operation on a stream
 *  @file: user file pointer
 *  @sd: stream head
 */
static int check_wakeup_oc(struct file *file, struct stdata *sd, long *timeo)
{
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
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

/**
 *  strwaitband: - timed wait to perform write on a flow controlled band
 *  @file: user file pointer
 *  @sd: stream head
 *  @band: band number
 *  @timeo: pointer to time remaining
 */
static int strwaitband(struct file *file, struct stdata *sd, int band, long *timeo)
{
	int err = 0;
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
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

/**
 *  strgetq: - get a message from a stream ala getpmsg
 *  @sd: stream head
 *  @flags: flags from getpmsg (%MSG_HIPRI or zero)
 *  @band: band from which to retrieve message
 */
static mblk_t *strgetq(struct stdata *sd, int flags, int band)
{
	ensure(sd, return (NULL));
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
			if ((1 << mp->b_datap->db_type) & ((1 << M_PROTO) | (1 << M_DATA))
			    && mp->b_band >= band)
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

/**
 *  strwaitgmsg: - wait to get a message from a stream ala getpmsg
 *  @file: user file pointer
 *  @sd: stream head
 *  @flags: flags from getpmsg (%MSG_HIPRI or zero)
 *  @band: priority band from which to get message
 */
static mblk_t *strwaitgmsg(struct file *file, struct stdata *sd, int flags, int band)
{
	mblk_t *mp;
	ensure(file, return (NULL));
	ensure(sd, return (NULL));
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

/**
 *  strwakeopen: - wake waiters waiting to open a stream
 *  @file: user file pointer
 *  @sd: stream head
 */
static void strwakeopen(struct file *file, struct stdata *sd)
{
	ensure(file, return);
	ensure(sd, return);
	/* release open bit */
	clear_bit(STWOPEN_BIT, &sd->sd_flag);
	/* wake up anybody waiting on open bit */
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
}

/*
 *  strwaitopen: - wait to open a stream head
 *  @file: user file pointer
 *  @sd: stream head
 */
static int strwaitopen(struct file *file, struct stdata *sd)
{
	int err = 0;
	int locked, frozen;
	ensure(file, return (-EBADF));
	ensure(sd, return (-ENOSTR));
	if ((err = check_stream_oc(file, sd)) == 0
	    && ((locked = test_and_set_bit(STWOPEN_BIT, &sd->sd_flag))
		|| (frozen = test_bit(STFROZEN_BIT, &sd->sd_flag)))) {
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

/**
 *  strsendioctl: - send a %M_IOCTL message downstream
 *  @file: user file pointer
 *  @cmd: ioctl command
 *  @crp: user credentials
 *  @dp: data for arg part of ioctl
 *  @dlen: length of data
 *  @timeo: pointer to wait time remaining
 *  @rval: pointer to ioctl return value
 *
 *  This function performs all of the actions necessary for sending an %M_IOCTL message downstream,
 *  waiting for a response, processing the response and resulting in a return value.  Any number of
 *  %COPYIN and %COPYOUT operations can be included in the process.
 */
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
				err = -EFAULT;
				if (__copy_from_user(dp->b_wptr, ptr, len))
					goto ioc_error;
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
				err = -EFAULT;
				if (__copy_to_user(ptr, dp->b_rptr, len))
					goto ioc_error;
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
 *  -------------------------------------------------------------------------
 *
 *  IO CONTROLS
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  str_i_atmark: - perform streamio(7) %I_ATMARK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_ATMARK
 *  @arg: ioctl argument
 */
static int str_i_atmark(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err = 0;
	queue_t *q = sd->sd_rq;
	trace();
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

/**
 *  str_i_canput: - perform streamio(7) %I_CANPUT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_CANPUT
 *  @arg: ioctl argument
 */
static int str_i_canput(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = -EINVAL;
	trace();
	if (arg < 256 || arg == ANYBAND)
		if (!(err = check_stream_wr(file, sd)))
			return (bcanput(sd->sd_wq, arg) ? 1 : 0);
	return (err);
}

/**
 *  str_i_ckband: - perform streamio(7) %I_CKBAND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_CKBAND
 *  @arg: ioctl argument
 */
static int str_i_ckband(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = -EINVAL;
	trace();
	if (arg < 256 || arg == ANYBAND)
		if (!(err = check_stream_io(file, sd)))
			return (bcanget(sd->sd_rq, arg) ? 1 : 0);
	return (err);
}

/**
 *  str_i_fdinsert: - perform streamio(7) %I_FDINSERT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_FDINSERT
 *  @arg: ioctl argument
 */
static int str_i_fdinsert(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	mblk_t *mp;
	struct strfdinsert fdi, *valp = (typeof(valp)) arg;
	struct stdata *sd2;
	struct file *f2;
	trace();
	if ((err = verify_area(VERIFY_READ, valp, sizeof(*valp))) < 0)
		goto exit;
	err = -EFAULT;
	if (__copy_from_user(&fdi, valp, sizeof(fdi)))
		goto exit;
	if ((err = check_stream_wr(file, sd)) < 0)
		goto exit;
	if (fdi.offset < 0 || fdi.ctlbuf.len < fdi.offset + sizeof(queue_t *)
	    || (fdi.flags != 0 && fdi.flags != RS_HIPRI))
		goto einval;
	if ((err = verify_area(VERIFY_READ, fdi.ctlbuf.buf, fdi.ctlbuf.len)) < 0)
		goto einval;
	if (fdi.databuf.len <= 0)
		goto no_databuf;
	if ((err = verify_area(VERIFY_READ, fdi.databuf.buf, fdi.databuf.len)) < 0)
		goto einval;
      no_databuf:
	if (fdi.ctlbuf.len > sysctl_str_strctlsz)
		goto erange;
	{
		long timeo;
		if (fdi.databuf.len >= 0) {
			queue_t *wq;
			if (fdi.databuf.len > sysctl_str_strmsgsz)
				goto erange;
			wq = sd->sd_wq->q_next;
			if (wq->q_minpsz > fdi.databuf.len || fdi.databuf.len > wq->q_maxpsz)
				goto erange;
		}
		if (!(f2 = fget(fdi.fildes)))
			goto einval;
		if (!(sd2 = stri_lookup(f2)))
			goto put_einval;
		timeo = (file->f_flags & (O_NONBLOCK | O_NDELAY)) ? 0 : MAX_SCHEDULE_TIMEOUT;
		if ((err = strwaitband(file, stri_lookup(file), 0, &timeo)) < 0)
			goto put_exit;
		if (!(mp = allocb(fdi.ctlbuf.len, BPRI_MED)))
			goto enosr;
		mp->b_datap->db_type = (fdi.flags == RS_HIPRI) ? M_PCPROTO : M_PROTO;
		if (fdi.databuf.len >= 0) {
			mblk_t *dp;
			if (!(dp = allocb(fdi.databuf.len, BPRI_MED)))
				goto free_enosr;
			dp->b_datap->db_type = M_DATA;
			__copy_from_user(dp->b_wptr, fdi.databuf.buf, fdi.databuf.len);
			dp->b_wptr += fdi.databuf.len;
			linkb(mp, dp);
		}
		__copy_from_user(mp->b_wptr, fdi.ctlbuf.buf, fdi.ctlbuf.len);
		mp->b_wptr += fdi.ctlbuf.len;
		bcopy(&sd2->sd_rq, mp->b_rptr + fdi.offset, sizeof(sd2->sd_rq));
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

/**
 *  str_i_find: - perform streamio(7) %I_FIND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_FIND
 *  @arg: ioctl argument
 */
static int str_i_find(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	const char *name = (typeof(name)) arg;
	queue_t *wq;
	int err;
	trace();
	err = -EFAULT;
	if (strnlen_user(name, FMNAMESZ + 1)) {
		err = 0;
		srlock(sd);
		for (wq = sd->sd_wq; wq; wq = SAMESTR(wq) ? wq->q_next : NULL) {
			const char *idname = wq->q_qinfo->qi_minfo->mi_idname;
			if (!strncmp(idname, name, FMNAMESZ))
				err = 1;
		}
		srunlock(sd);
	}
	return (err);
}

/**
 *  str_i_flushband: - perform streamio(7) %I_FLUSHBAND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_FLUSHBAND
 *  @arg: ioctl argument
 */
static int str_i_flushband(struct file *file, struct stdata *sd, unsigned int cmd,
			   unsigned long arg)
{
	int err;
	mblk_t *mp;
	struct bandinfo bi, *valp = (struct bandinfo *) arg;
	trace();
	if ((err = verify_area(VERIFY_READ, valp, sizeof(*valp))))
		return (err);
	if (__copy_from_user(&bi, valp, sizeof(bi)))
		return (-EFAULT);
	if (bi.bi_flag & ~(FLUSHR | FLUSHW | FLUSHRW))
		return (-EINVAL);
	if ((err = check_stream_io(file, sd)))
		return (err);
	if (!(mp = allocb(2, BPRI_MED)))
		return (-ENOSR);
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = bi.bi_flag | FLUSHBAND;
	*mp->b_wptr++ = bi.bi_pri;
	mp->b_flag |= MSGNOLOOP;
	putnext(sd->sd_wq, mp);
	return (0);
}

/**
 *  str_i_flush: - perform streamio(7) %I_FLUSH ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_FLUSH
 *  @arg: ioctl argument
 */
static int str_i_flush(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	mblk_t *mp;
	trace();
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

/**
 *  str_i_getband: - perform streamio(7) %I_GETBAND ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_GETBAND
 *  @arg: ioctl argument
 */
static int str_i_getband(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err;
	int *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_io(file, sd)))
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
	err = -EFAULT;
	if (__copy_to_user(valp, &err, sizeof(*valp)))
		return (err);
	return (0);
}

/**
 *  str_i_getcltime: - perform streamio(7) %I_GETCLTIME ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_GETCLTIME
 *  @arg: ioctl argument
 */
static int str_i_getcltime(struct file *file, struct stdata *sd, unsigned int cmd,
			   unsigned long arg)
{
	int err;
	int closetime, *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_wr(file, sd)))
		return (err);
	closetime = drv_hztomsec(sd->sd_closetime);
	err = -EFAULT;
	if (__copy_to_user(valp, &closetime, sizeof(*valp)))
		return (err);
	return (0);
}

/**
 *  str_i_setcltime: - perform streamio(7) %I_SETCLTIME ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_SETCLTIME
 *  @arg: ioctl argument
 */
static int str_i_setcltime(struct file *file, struct stdata *sd, unsigned int cmd,
			   unsigned long arg)
{
	int err;
	int closetime, *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_READ, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_io(file, sd)))
		return (err);
	if (__copy_from_user(&closetime, valp, sizeof(*valp)))
		return (-EFAULT);
	if (0 > closetime || closetime > 300000)
		return (-EINVAL);
	sd->sd_closetime = drv_msectohz(closetime);
	return (0);
}

/**
 *  str_i_getsig: - perform streamio(7) %I_GETSIG ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_GETSIG
 *  @arg: ioctl argument
 */
static int str_i_getsig(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, flags, *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))))
		return (err);
	if ((err = check_stream_io(file, sd)))
		return (err);
	flags = sd->sd_sigflags;
	err = -EFAULT;
	if (__copy_to_user(valp, &flags, sizeof(*valp)))
		return (err);
	return (0);
}

/**
 *  str_i_setsig: - perform streamio(7) %I_SETSIG ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_SETSIG
 *  @arg: ioctl argument
 */
static int str_i_setsig(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	trace();
	err = -EINVAL;
	if (arg & ~(S_ALL))
		goto error;
	if (arg == 0) {
		/* if the argument is zero it means to remove the calling process from the list, if 
		   the calling process is not on the list, return EINVAL */
	} else {
	}
	if ((err = check_stream_io(file, sd)))
		goto error;
	sd->sd_sigflags = arg;
	return (0);
      error:
	return (err);
}

/**
 *  str_i_grdopt: - perform streamio(7) %I_GRDOPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_GRDOPT
 *  @arg: ioctl argument
 */
static int str_i_grdopt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, rdopt, *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if ((err = check_stream_io(file, sd)))
		return (err);
	rdopt = sd->sd_rdopt & (RMODEMASK | RPROTMASK);
	err = -EFAULT;
	if (__copy_to_user(valp, &rdopt, sizeof(*valp)))
		return (err);
	return (0);
}

/**
 *  str_i_srdopt: - perform streamio(7) %I_SRDOPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_SRDOPT
 *  @arg: ioctl argument
 */
static int str_i_srdopt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, mode, prot;
	trace();
	mode = arg & (RMODEMASK);
	prot = arg & (RPROTMASK);
	if ((arg & ~(RMODEMASK | RPROTMASK))
	    || (mode != RNORM && mode != RMSGD && mode != RMSGN)
	    || (prot != RPROTNORM && prot != RPROTDAT && prot != RPROTDIS))
		return (-EINVAL);
	if ((err = check_stream_io(file, sd)))
		return (err);
	sd->sd_rdopt = arg;
	return (0);
}

/**
 *  str_i_gwropt: - perform streamio(7) %I_GWROPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_GWROPT
 *  @arg: ioctl argument
 */
static int str_i_gwropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, wropt, *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if ((err = check_stream_wr(file, sd)) < 0)
		return (err);
	wropt = sd->sd_wropt & (SNDZERO | SNDPIPE | SNDHOLD);
	err = -EFAULT;
	if (__copy_to_user(valp, &wropt, sizeof(*valp)))
		return (err);
	return (0);
}

/**
 *  str_i_swropt: - perform streamio(7) %I_SWROPT ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_SWROPT
 *  @arg: ioctl argument
 */
static int str_i_swropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	trace();
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

/**
 *  str_send_link_ioctl: - sends the %M_IOCTL command for %I_LINK operation
 *  @file: user file pointer
 *  @cmd: ioctl command to send
 *  @crp: user credentials pointer
 *  @qtop: upper multiplexing driver write queue pointer
 *  @qbot: lower multiplexing driver write queue pointer
 *  @index: multiplex index
 */
static int str_send_link_ioctl(struct file *file, int cmd, cred_t *crp, queue_t *qtop,
			       queue_t *qbot, int index)
{
	int err, rval;
	struct linkblk *lbp;
	mblk_t *mp;
	trace();
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

/**
 *  str_i_xlink: - perform streamio(7) %I_LINK or %I_PLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_LINK or %I_PLINK
 *  @arg: ioctl argument
 */
static int str_i_xlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct file *f2;
	struct stdata *sd2, **sdp;
	struct linkblk *l;
	queue_t *qtop, *qbot;
	int index;
	trace();
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
	sdp = (cmd == I_LINK) ? &sd->sd_links : &sd->sd_cdevsw->d_plinks;
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

/**
 *  str_i_link: - perform streamio(7) %I_LINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_LINK
 *  @arg: ioctl argument
 */
static int str_i_link(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	return str_i_xlink(file, sd, cmd, arg);
}

/**
 *  str_i_plink: - perform streamio(7) %I_PLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_PLINK
 *  @arg: ioctl argument
 */
static int str_i_plink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	trace();
	return str_i_xlink(file, sd, cmd, arg);
}

/**
 *  str_i_xunlink: - perform streamio(7) %I_UNLINK or %I_PUNLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_UNLINK or %I_PUNLINK
 *  @arg: ioctl argument
 */
static int str_i_xunlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	struct stdata **sdp;
	int err;
	long index = arg;
	trace();
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	sdp = (cmd == I_UNLINK) ? &sd->sd_links : &sd->sd_cdevsw->d_plinks;
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

/**
 *  str_i_unlink: - perform streamio(7) %I_UNLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_UNLINK
 *  @arg: ioctl argument
 */
static int str_i_unlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	trace();
	return str_i_xunlink(file, sd, cmd, arg);
}

/**
 *  str_i_punlink: - perform streamio(7) %I_PUNLINK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_PUNLINK
 *  @arg: ioctl argument
 */
static int str_i_punlink(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	return str_i_xunlink(file, sd, cmd, arg);
}

/**
 *  str_i_list: - perform streamio(7) %I_LIST ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_LIST
 *  @arg: ioctl argument
 */
static int str_i_list(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, nmods, i;
	struct str_list sl, *valp = (struct str_list *) arg;
	struct str_mlist *sm;
	struct queue **qp;
	char fmname[FMNAMESZ + 1];
	trace();
	nmods = sd->sd_pushcnt + 1;
	if (valp == NULL)
		/* return number of modules and drivers */
		return (nmods);
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if (__copy_from_user(&sl, valp, sizeof(sl)))
		return (-EFAULT);
	nmods = sl.sl_nmods < nmods ? sl.sl_nmods : nmods;
	if (nmods < 0 || (sm = sl.sl_modlist) == NULL)
		return (-EINVAL);
	if ((err = verify_area(VERIFY_WRITE, sm, sizeof(*sm) * nmods)) < 0)
		return (err);
	for (qp = &sd->sd_wq->q_next, i = 0; *qp && i < nmods; qp = &(*qp)->q_next, i++, sm++) {
		snprintf(fmname, FMNAMESZ + 1, (*qp)->q_qinfo->qi_minfo->mi_idname);
		err = -EFAULT;
		if (__copy_to_user(sm->l_name, fmname, FMNAMESZ + 1))
			return (err);
	}
	return (0);
}

/**
 *  str_i_look: - perform streamio(7) %I_LOOK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_LOOK
 *  @arg: ioctl argument
 */
static int str_i_look(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	queue_t *q;
	char fmname[FMNAMESZ + 1], *valp = (char *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, FMNAMESZ + 1)))
		goto exit;
	srlock(sd);
	err = -EINVAL;
	if (sd->sd_pushcnt <= 0)
		goto unlock_exit;
	err = -EINVAL;
	if (!(q = sd->sd_wq->q_next) || !SAMESTR(sd->sd_wq))
		goto unlock_exit;
	snprintf(fmname, FMNAMESZ + 1, q->q_qinfo->qi_minfo->mi_idname);
	srunlock(sd);
	err = -EFAULT;
	if (__copy_to_user(valp, fmname, FMNAMESZ + 1))
		return (err);
	return (0);
      unlock_exit:
	srunlock(sd);
      exit:
	return (err);
}

/**
 *  str_i_nread: - perform streamio(7) %I_NREAD ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_NREAD
 *  @arg: ioctl argument
 */
static int str_i_nread(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err, bytes, *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		goto exit;
	if ((err = check_stream_io(file, sd)))
		goto exit;
	qrlock(sd->sd_rq, &flags);
	err = qsize(sd->sd_rq);
	bytes = msgdsize(sd->sd_rq->q_first);
	qrunlock(sd->sd_rq, &flags);
	err = -EFAULT;
	if (__copy_to_user(valp, &bytes, sizeof(bytes)))
		return (err);
	return (0);
      exit:
	return (err);
}

/**
 *  str_i_peek: - perform streamio(7) %I_PEEK ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_PEEK
 *  @arg: ioctl argument
 */
static int str_i_peek(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	unsigned long flags;
	int err, rtn = 0;
	struct strpeek sp, *valp = (struct strpeek *) arg;
	queue_t *q = sd->sd_rq;
	trace();
	if (!valp)
		return (-EINVAL);
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if (__copy_from_user(&sp, valp, sizeof(sp)))
		return (-EFAULT);
	if (sp.ctlbuf.maxlen < 0 || sp.databuf.maxlen < 0 || (sp.ctlbuf.maxlen && !sp.ctlbuf.buf)
	    || (sp.databuf.maxlen && !sp.databuf.buf))
		return (-EINVAL);
	if (sp.ctlbuf.maxlen
	    && (err = verify_area(VERIFY_WRITE, sp.ctlbuf.buf, sp.ctlbuf.maxlen)) < 0)
		return (err);
	if (sp.databuf.maxlen
	    && (err = verify_area(VERIFY_WRITE, sp.databuf.buf, sp.databuf.maxlen)) < 0)
		return (err);
	/* all areas verified, check readable */
	if ((err = check_stream_io(file, sd)))
		return (err);
	qrlock(q, &flags);
	do {
		mblk_t *mp, *dp;
		ssize_t blen;
		ssize_t clen = sp.ctlbuf.maxlen;
		ssize_t dlen = sp.databuf.maxlen;
		ssize_t davail, cavail;
		if (!(mp = q->q_first)) {
			err = 0;
			rtn = 0;
			break;
		}
		davail = msgdsize(mp);	/* M_DATA blocks */
		cavail = msgsize(mp) - davail;	/* isddatablks - M_DATA blocks */
		sp.flags = mp->b_datap->db_type > QPCTL ? RS_HIPRI : 0;
		sp.ctlbuf.len = 0;
		sp.databuf.len = 0;
		for (dp = mp; dp; dp = dp->b_cont) {
			if ((blen = dp->b_wptr - dp->b_rptr) <= 0)
				continue;
			if (dp->b_datap->db_type == M_DATA) {
				/* goes in data part */
				if (dlen == -1)
					break;
				if (blen > dlen) {
					if (__copy_to_user(sp.databuf.buf + sp.databuf.len, dp->b_rptr,
						       dlen))
						return (-EFAULT);
					sp.databuf.len += dlen;
					dlen = 0;
					break;
				}
				if (__copy_to_user(sp.databuf.buf + sp.databuf.len, dp->b_rptr, blen))
					return (-EFAULT);
				sp.databuf.len += blen;
				dlen -= blen;
			} else {
				/* goes in ctrl part */
				if (clen == -1)
					break;
				if (blen > clen) {
					if (__copy_to_user(sp.ctlbuf.buf + sp.ctlbuf.len, dp->b_rptr,
						       clen))
						return (-EFAULT);
					sp.ctlbuf.len += clen;
					clen = 0;
					break;
				}
				if (__copy_to_user(sp.ctlbuf.buf + sp.ctlbuf.len, dp->b_rptr, blen))
					return (-EFAULT);
				sp.ctlbuf.len += blen;
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

/**
 *  str_i_push: - perform streamio(7) %I_PUSH ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_PUSH
 *  @arg: ioctl argument
 */
static int str_i_push(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	char name[FMNAMESZ + 1];
	dev_t dev = sd->sd_dev;
	trace();
	if ((err = strncpy_from_user(name, (const char *) arg, FMNAMESZ + 1)) < 0)
		goto error;
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	if ((err = qpush(sd, name, &dev, make_oflag(file), current_creds)) < 0)
		goto error;
      error:
	return (err);
}

/**
 *  str_i_pop: - perform streamio(7) %I_POP ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_POP
 *  @arg: ioctl argument
 */
static int str_i_pop(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	trace();
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	if ((err = qpop(sd, make_oflag(file), current_creds)) < 0)
		goto error;
      error:
	return (err);
}

/**
 *  freefd_func: - free function for file descriptor
 *  @arg: file pointer as caddr_t argument
 */
static void freefd_func(caddr_t arg)
{
	struct file *file = (struct file *) arg;
	trace();
	/* sneaky trick to free the file pointer when mblk freed, this means that M_PASSFP messages 
	   flushed from a queue will free the file pointers referenced by them */
	fput(file);
	return;
}

/**
 *  str_i_sendfd: - perform streamio(7) %I_SENDFD ioctl
 *  @file: user file pointer
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_SENDFD
 *  @arg: ioctl argument
 */
static int str_i_sendfd(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct file *f2;
	mblk_t *mp;
	queue_t *rq;
	trace();
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
 *  str_i_recvfd: - receive file descriptor streamio(7) %I_RECVFD ioctl
 *  @file: user file pointer for the current open stream
 *  @sd: stream head of current open stream
 *  @cmd: ioctl command, always %I_RECVFD
 *  @arg: ioctl argument, a pointer to a &struct strrecvfd structure
 */
static int str_i_recvfd(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	struct strrecvfd sr, *valp = (struct strrecvfd *) arg;
	mblk_t *mp;
	int fd, err;
	struct file *f2;
	trace();
	if (!valp)
		goto einval;
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
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
	sr.fd = fd;
	sr.uid = f2->f_uid;
	sr.gid = f2->f_gid;
	if (__copy_to_user(valp, &sr, sizeof(sr)))
		goto efault;
	freemsg(mp);
	return (0);
      efault:
	err = (-EFAULT);
	goto putbq_error;
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

/**
 *  str_i_str: - perform streamio(7) %I_STR ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @cmd: ioctl command, always %I_STR
 *  @arg: ioctl argument, pointer to a &struct strioctl structure
 */
static int str_i_str(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, rval = 0;
	mblk_t *mp;
	struct strioctl ic, *valp = (typeof(valp)) arg;
	trace();
	if ((err = check_stream_io(file, sd)) < 0)
		goto error;
	if ((err = verify_area(VERIFY_READ, valp, sizeof(*valp))) < 0)
		goto error;
	if (__copy_from_user(&ic, valp, sizeof(*valp)))
		return (-EFAULT);
	if ((err = verify_area(VERIFY_READ, ic.ic_dp, ic.ic_len)) < 0)
		goto error;
	err = -ENOSR;
	if (!(mp = allocb(ic.ic_len, BPRI_MED)))
		goto error;
	if (__copy_from_user(mp->b_wptr, ic.ic_dp, ic.ic_len))
		return (-EFAULT);
	mp->b_wptr += ic.ic_len;
	{
		long timeo = (file->f_flags & (O_NONBLOCK | O_NDELAY)) ? 0 : MAX_SCHEDULE_TIMEOUT;
		timeo = ic.ic_timout == -1 ? timeo : drv_msectohz(ic.ic_timout);
		err = strsendioctl(file, I_STR, current_creds, mp, ic.ic_len, &timeo, &rval);
	}
	if (err < 0)
		goto error;
	return (rval);
      error:
	return (err);
}

/**
 *  str_i_gerropt: - get error options streamio(7) %I_GERROPT ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @cmd: ioctl command, always %I_GERROPT
 *  @arg: ioctl argument, pointer to an integer to contain the returned error options
 */
static int str_i_gerropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, eropt, *valp = (int *) arg;
	trace();
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
		return (err);
	if ((err = check_stream_io(file, sd)) < 0)
		return (err);
	eropt = sd->sd_eropt & (RERRNONPERSIST | WERRNONPERSIST);
	err = -EFAULT;
	if (__copy_to_user(valp, &eropt, sizeof(*valp)))
		return (err);
	return (0);
}

/**
 *  str_i_serropt: - set error options streamio(7) %I_SERROPT ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @cmd: ioctl command, always %I_SERROPT
 *  @arg: ioctl argument, integer error options to set
 */
static int str_i_serropt(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	trace();
	if ((arg & ~(RERRNONPERSIST | WERRNONPERSIST)))
		return (-EINVAL);
	if ((err = check_stream_io(file, sd)) < 0)
		return (err);
	sd->sd_eropt = arg;
	return (0);
}

/**
 *  str_i_anchor: - set a push anchor streamio(7) %I_ANCHOR ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @cmd: ioctl command, always %I_ANCHOR
 *  @arg: ioctl argument, integer index at which point to position anchor
 */
static int str_i_anchor(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	trace();
	if ((err = check_stream_io(file, sd)) < 0)
		return (err);
	sd->sd_nanchor = sd->sd_pushcnt;
	return (0);
}

/**
 *  str_i_transparent: - perform transparent ioctl(2) ioctl
 *  @file: user file pointer for the open stream
 *  @sd: stream head of the open stream
 *  @cmd: ioctl command
 *  @arg: ioctl argument
 */
static int str_i_transparent(struct file *file, struct stdata *sd, unsigned int cmd,
			     unsigned long arg)
{
	int err, rval;
	mblk_t *mp;
	trace();
	err = -ENOSR;
	if (!(mp = allocb(sizeof(arg), BPRI_MED)))
		goto error;
	*((unsigned long *) mp->b_wptr) = arg;
	mp->b_wptr += sizeof(unsigned long);
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

/**
 *  strllseek: - llseek file operation on stream
 *  @file: user file pointer for the open stream
 *  @off: offset to which to seek
 *  @when: from whence to seek
 */
loff_t strllseek(struct file *file, loff_t off, int whence)
{
	return (-ESPIPE);
}

/**
 *  strpoll: - poll file operation on stream
 *  @file: user file pointer for the open stream
 *  @poll: poll table pointer
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

/**
 *  strmmap: - mmap file operation for a stream
 *  @file: user file pointer for the open stream
 *  @vma: pointer to a virtual memory area structure
 */
int strmmap(struct file *file, struct vm_area_struct *vma)
{
	return (-ENODEV);
}

#ifndef O_CLONE
#define O_CLONE (O_CREAT|O_EXCL)
#endif

/**
 *  stropen: - open file operation for a stream
 *  @inode: shadow special filesystem device inode
 *  @file: shadow special filesystem file pointer
 *
 *  stropen() can be called by spec_open() (or an inplace open) which was called on an external
 *  filesystem inode, or a direct open of a mounted internal shadow special filesystem inode.  The
 *  @file and @inode represented here could be external or internal inode and file pointer.  For
 *  external inodes, the file->private_data points to the cdevsw structure for the driver.  Internal
 *  inodes do not.  Only fifo and socket (UNIX domain) inodes (regardless of S_IFMT) pass external
 *  inodes to this function.
 */
int stropen(struct inode *inode, struct file *file)
{
	int err, sflag;
	struct stdata *sd;
	dev_t dev;
	ptrace(("%s: opening stream\n", __FUNCTION__));
	dev = file->private_data ? *((dev_t *) file->private_data) : inode->i_ino;
	sflag = ((file->f_flags & O_CLONE) == O_CLONE) ? CLONEOPEN : DRVOPEN;
	/* first find out of we already have a stream head, or we need a new one anyway */
	if (sflag == CLONEOPEN || !(sd = sd_get((struct stdata *) inode->i_pipe))) {
		queue_t *q;
		struct cdevsw *cdev;
		printd(("%s: need new stream head\n", __FUNCTION__));
		if (!(cdev = cdrv_get(getmajor(dev))))
			return (-ENXIO);
		printd(("%s: %s: got driver\n", __FUNCTION__, cdev->d_name));
		printd(("%s: allocating queues for %s\n", __FUNCTION__, cdev->d_name));
		/* we don't have a stream yet (or want a new one), so allocate one */
		if (!(q = allocq())) {
			printd(("%s: %s: putting driver\n", __FUNCTION__, cdev->d_name));
			cdrv_put(cdev);
			return (-ENOSR);
		}
		printd(("%s: creating stream head for %s\n", __FUNCTION__, cdev->d_name));
		/* FIXME: need to find/create and attach syncq. */
		if (!(sd = allocstr())) {
			printd(("%s: %s: putting driver\n", __FUNCTION__, cdev->d_name));
			cdrv_put(cdev);
			freeq(q);
			return (-ENOSR);
		}
		printd(("%s: initializing stream head %p for %s\n", __FUNCTION__, sd,
			cdev->d_name));
		/* initialization of the stream head */
		sd->sd_dev = dev;
		sd->sd_flag = STWOPEN;	/* hold open bit */
		sd->sd_rq = q;
		sd->sd_wq = q + 1;
		((struct queinfo *) q)->qu_str = sd;
		sd->sd_cdevsw = cdev;
		switch (cdev->d_mode & S_IFMT) {
		case S_IFIFO:
			printd(("%s: stream head is FIFO\n", __FUNCTION__));
			sd->sd_flag |= (cdev->d_flag & D_CLONE) ? STRISPIPE : STRISFIFO;
			sd->sd_wropt = SNDZERO | SNDPIPE;	/* special write ops */
			sd->sd_strtab = cdev->d_str;	/* driver *is* stream head */
			setq(q, cdev->d_str->st_rdinit, cdev->d_str->st_wrinit);
			break;
		case S_IFSOCK:
			printd(("%s: stream head is SOCK\n", __FUNCTION__));
			sd->sd_flag |= STRISSOCK;
			sd->sd_wropt = SNDZERO | SNDPIPE;	/* special write ops */
			sd->sd_strtab = cdev->d_str;	/* driver *is* stream head */
			setq(q, cdev->d_str->st_rdinit, cdev->d_str->st_wrinit);
			break;
		default:
			swerr();
		case S_IFCHR:
			printd(("%s: stream head is CHR\n", __FUNCTION__));
			sd->sd_wropt = 0;	/* default write ops */
			sd->sd_strtab = &str_info;	/* no driver yet */
			setq(q, &str_rinit, &str_winit);
			break;
		}
		/* only link to initial file pointer */
		sd->sd_file = file;
		/* don't graft onto inode just yet */
		sd->sd_inode = NULL;
		/* done setup, do the open */
		printd(("%s: %s: putting driver\n", __FUNCTION__, cdev->d_name));
		cdrv_put(cdev);
	} else {
		printd(("%s: waiting on open of stream head %p\n", __FUNCTION__, sd));
		if ((err = strwaitopen(file, sd)))
			goto sdput_exit;
		printd(("%s: holding existing stream head %p\n", __FUNCTION__, sd));
	}
	/* here we hold the STWOPEN bit - stream head open must clear */
	printd(("%s: calling qopen for stream head %p\n", __FUNCTION__, sd));
	dev = sd->sd_dev;
	if ((err = qopen(sd->sd_rq, &sd->sd_dev, make_oflag(file), sflag, current_creds)))
		goto wakeup_exit;
	printd(("%s: qopen call for stream head %p successful\n", __FUNCTION__, sd));
	sd->sd_opens++;
	sd->sd_readers += (file->f_mode & FREAD) ? 1 : 0;
	sd->sd_writers += (file->f_mode & FWRITE) ? 1 : 0;
	if (!sd->sd_inode && dev != sd->sd_dev) {
		struct dentry *dentry;
		/* we need a new dentry and inode in the device shadow directory */
		printd(("%s: changing inode\n", __FUNCTION__));
		dentry = spec_dentry(sd->sd_dev, NULL);
		err = PTR_ERR(dentry);
		if (IS_ERR(dentry))
			goto detach_exit;
		err = 0;
		/* swap file operations and directory entry -- both directory entries are on the
		   same vfsmount and superblock */
		fops_put(xchg(&file->f_op, fops_get(inode->i_fop)));
		dput(xchg(&file->f_dentry, dget(dentry)));
		/* inode needs to change - use the new one */
		inode = dentry->d_inode;
	} else {
		printd(("%s: using current inode\n", __FUNCTION__));
		/* current inode is ok */
	}
	printd(("%s: got inode %p (%ld), i_count %d, i_nlink %d\n", __FUNCTION__, inode,
		inode->i_ino, atomic_read(&inode->i_count), inode->i_nlink));
	/* hold inode while stream head attached */
	if (!sd->sd_inode && (sd->sd_inode = igrab(inode))) {
		/* link into clone list */
		sd->sd_clone = (void *) inode->i_pipe;
		inode->i_pipe = (void *) sd;
	}
	/* always write file->private_data */
	stri_lookup(file) = sd;
      wakeup_exit:
	printd(("%s: waking up waiters on stream head\n", __FUNCTION__));
	strwakeopen(file, sd);	/* clear STWOPEN bit - wake up anybody waiting on open bit */
      sdput_exit:
	if (err != 0)
		sd_put(sd);
	return (err <= 0 ? err : -err);
      detach_exit:
	/* we already called qopen but failed later */
	printd(("%s: detaching opened queues\n", __FUNCTION__));
	qdetach(sd->sd_rq, make_oflag(file), current_creds);
	sd->sd_rq = sd->sd_wq = NULL;
	printd(("%s: putting stream head %p\n", __FUNCTION__, sd));
	sd_put(sd);		/* should be final put */
	goto sdput_exit;
}

/**
 *  strflush: - flush file operation for a stream
 *  @file: user file pointer for stream
 *
 *  sys_close() calls filp_close() that calls f_op->flush().  We don't actualy do any flushing
 *  because this is not necessarily the last close.  If we ever support multiple simultaenous
 *  partial readers or writers we could tear down partials here.
 *
 *  TODO: Because this is the only function from which we can return an error return value, we need
 *  to investigate a number of things about the stream at this point and decide whether to return an
 *  error code now.  Returning an error here does not stop the strclose function from being called
 *  afterward.
 */
int strflush(struct file *file)
{
	ptrace(("%s: stream close\n", __FUNCTION__));
	return (0);
}

/**
 *  strclose: - close file operation for a stream
 *  @inode: shadow special filesystem inode for the stream
 *  @file: shadow special file pointer for the stream
 *
 *  This function is called by fput() on final put of a file pointer associated with the file.
 *  After this function returns a dput() will be peformed on the dentry and a mntput() on the mount.
 *
 *  When the last close is about to occur we will get a d_delete() operation followed by a d_iput()
 *  operation if special dops are assigned to the dentry.  The d_iput() chains or passes to an
 *  iput() which uses a superblock put_inode() operation followed by a superblock delete_inode()
 *  operation.
 *
 *  Return Value: The return value from this function is ignored.  It is the return value from the
 *  flush() above that is used in return to the close(2) call.
 */
int strclose(struct inode *inode, struct file *file)
{
	struct stdata *sd;
	ptrace(("%s: closing stream\n", __FUNCTION__));
	if ((sd = sd_get(stri_lookup(file)))) {
		int err = 0;
		printd(("%s: exiting stream head\n", __FUNCTION__));
		stri_lookup(file) = NULL;
		sd->sd_readers -= (file->f_mode & FREAD) ? 1 : 0;
		sd->sd_writers -= (file->f_mode & FWRITE) ? 1 : 0;
		if (--sd->sd_opens == 0) {
			queue_t *wq, *qq;
			int oflag = make_oflag(file);
			cred_t *crp = current_creds;
			printd(("%s: last close\n", __FUNCTION__));
			/* last close */
			if (test_and_set_bit(STRCLOSE_BIT, &sd->sd_flag)) {
				printd(("%s: already closing\n", __FUNCTION__));
				goto put_exit;
			}
			if (test_bit(STPLEX_BIT, &sd->sd_flag)) {
				printd(("%s: linked under multiplexing driver\n", __FUNCTION__));
				goto put_exit;
			}
			/* 1st step: unlink any (temporary) linked streams */
			printd(("%s: unlink temporary linked streams\n", __FUNCTION__));
			err = str_i_unlink(file, sd, I_UNLINK, MUXID_ALL);
			/* 2nd step: call the close routine of each module and pop the module. */
			printd(("%s: closing queues\n", __FUNCTION__));
			wq = sd->sd_wq;
			while ((qq = wq->q_next) && SAMESTR(wq)) {
				if (!(oflag & (O_NONBLOCK | O_NDELAY)) && qq->q_msgs
				    && !signal_pending(current)) {
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
				printd(("%s: detaching module queue %p\n", __FUNCTION__, (qq - 1)));
				qdetach(qq - 1, oflag, crp);
			}
			/* 3rd step: call the close routine of the driver and qdetach the driver */
			printd(("%s: detaching driver queue %p\n", __FUNCTION__, (wq - 1)));
			qdetach(wq - 1, oflag, crp);
			sd->sd_rq = sd->sd_wq = NULL;
			printd(("%s: removing from inode clone list\n", __FUNCTION__));
			/* remove from the inode clone list */
			assert(inode == sd->sd_inode);
			{
				struct stdata **sdp;
				for (sdp = (struct stdata **) &(inode->i_pipe); *sdp && *sdp != sd;
				     sdp = &((*sdp)->sd_clone)) ;
				if (*sdp && *sdp == sd) {
					printd(("%s: stream head delete from clone list\n",
						__FUNCTION__));
					*sdp = sd->sd_clone;
					sd->sd_clone = NULL;
				} else {
					pswerr(("%s: could not find stream head in clone list\n",
						__FUNCTION__));
				}
				sd->sd_inode = NULL;
				sd_put(sd);
			}
		} else {
			assert(inode == sd->sd_inode);
		}
		/* we need to put the inode on each close because we grabbed it on each open */
		printd(("%s: putting inode %p (%ld), i_count %d, i_nlink %d\n",
			__FUNCTION__, inode, inode->i_ino,
			atomic_read(&inode->i_count), inode->i_nlink));
		iput(inode);
	      put_exit:
		printd(("%s: putting stream head %p\n", __FUNCTION__, sd));
		sd_put(sd);	/* could be final put */
		return (err);
	}
	pswerr(("%s: no stream head\n", __FUNCTION__));
	return (-ENOSTR);
}

/**
 *  strfasync: - fasync file operation for a stream
 *  @fd: user file descriptor
 *  @file: user file pointer
 *  @on: aysnc io flag
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

/**
 *  strreadv: - readv file operation for a stream
 *  @file: shadow special filesystem file pointer
 *  @iov: an io vector
 *  @len: length of the io vector
 *  @ppos: seek position
 *
 *  This performs the readv file operation for a stream directly and provides whatever optimization
 *  can be acheived by mapping mblks into separate io vector components.
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
	/* If the driver has registered for direct IO, then we call the driver's readv procedure. */
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

int strgetpmsg(struct file *, struct strbuf *, struct strbuf *, int *, int *);
/**
 *  strread: - read file operations for a stream
 *  @file: file pointer for the stream
 *  @buf: user buffer for read data
 *  @len: length of the user buffer
 *  @ppos: seek position
 */
ssize_t strread(struct file *file, char *buf, size_t len, loff_t *ppos)
{
	struct iovec iov;
	struct stdata *sd = stri_lookup(file);
#if !defined HAVE_PUTPMSG_GETPMSG_SYS_CALLS || defined LFS_GETMSG_PUTMSG_ULEN
	/* read emulation of the putpmsg system call: the problem with this approach is that it
	   almost completely destroys the ability to have a 64-bit application running against a
	   32-bit kernel because the pointers cannot be properly converted. */
	if (len == LFS_GETMSG_PUTMSG_ULEN) {
		int err;
		struct strpmsg sg, *valp = (struct strpmsg *) buf;
		if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))) < 0)
			goto error;
		if (__copy_from_user(&sg, valp, sizeof(sg)))
			return (-EFAULT);
		if (sg.ctlbuf.maxlen > 0
		    && (err = verify_area(VERIFY_WRITE, sg.ctlbuf.buf, sg.ctlbuf.maxlen)) < 0)
			goto error;
		if (sg.databuf.maxlen > 0
		    && (err = verify_area(VERIFY_WRITE, sg.databuf.buf, sg.databuf.maxlen)) < 0)
			goto error;
		return strgetpmsg(file, &sg.ctlbuf, &sg.databuf, &sg.band, &sg.flags);
	      error:
		return (err);
	}
#endif
	/* If the driver has registered for direct IO, then we call the driver's read procedure. */
	if (sd->sd_directio && sd->sd_directio->read)
		return sd->sd_directio->read(file, buf, len, ppos);
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return strreadv(file, &iov, 1, ppos);
}

/**
 *  strwritev: - writev file operation for a stream
 *  @file: file pointer for the stream
 *  @iov: io vector to write
 *  @count: size of io vector
 *  @ppos: seek position
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
	/* If the driver has established direct IO then we call the driver's writev procedure */
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

int strputpmsg(struct file *, struct strbuf *, struct strbuf *, int, int);
/**
 *  strwrite: - write file operation for a stream
 *  @file: file pointer for the stream
 *  @buf: user buffer to write
 *  @len: length of user data
 *  @ppos: seek position
 */
ssize_t strwrite(struct file *file, const char *buf, size_t len, loff_t *ppos)
{
	struct iovec iov;
	struct stdata *sd = stri_lookup(file);
#if !defined HAVE_PUTPMSG_GETPMSG_SYS_CALLS || defined LFS_GETMSG_PUTMSG_ULEN
	/* write emulation of the putpmsg system call: the problem with this approach is that it
	   almost completely destroys the ability to have a 64-bit application running against a
	   32-bit kernel because the pointers cannot be properly converted. */
	if (len == LFS_GETMSG_PUTMSG_ULEN) {
		int err;
		struct strpmsg sp, *valp = (struct strpmsg *) buf;
		if ((err = verify_area(VERIFY_READ, valp, sizeof(*valp))))
			goto error;
		if (__copy_from_user(&sp, valp, sizeof(sp)))
			return (-EFAULT);
		if (sp.ctlbuf.len > 0
		    && (err = verify_area(VERIFY_READ, sp.ctlbuf.buf, sp.ctlbuf.len)) < 0)
			goto error;
		if (sp.databuf.len > 0
		    && (err = verify_area(VERIFY_READ, sp.databuf.buf, sp.databuf.len)) < 0)
			goto error;
		return strputpmsg(file, &sp.ctlbuf, &sp.databuf, sp.band, sp.flags);
	      error:
		return (err);
	}
#endif
	/* If the driver has established direct IO then we call the driver's write procedure */
	if (sd->sd_directio && sd->sd_directio->write)
		return sd->sd_directio->write(file, buf, len, ppos);
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return strwritev(file, &iov, 1, ppos);
}

/**
 *  __strfreepage: - free routine function to free a esballoc'ed page
 *  @data: &struct page pointer passed as a caddr_t argument
 */
static void __strfreepage(caddr_t data)
{
	struct page *page = (struct page *) data;
	(void) page;		/* shut up compiler */
	kunmap(page);
}

/**
 *  strwaitpage: - wait to send a page
 *  @file: file pointer for the stream
 *  @size: size of the data to write
 *  @prio: priority of data
 *  @band: priority band for data
 *  @type: message block type
 *  @base: base address
 *  @frtn: free routine to free the allocated page
 *  @timeo: the amount of time remaining to wait
 */
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

/**
 *  strsendpage: - sendpage file operation for a stream
 *  @file: file pointer for the stream
 *  @page: page descriptor
 *  @offset: offset into the page
 *  @size: amount of data within the page
 *  @ppos: seek position
 *  @more: more pages to send
 */
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

/**
 *  strputpmsg: - putpmsg file operation for a stream
 *  @file: file pointer for the stream
 *  @ctlp: &struct strbuf pointer describing the ctrl part of the message
 *  @datp: &struct strbuf pointer describing the data part of the message
 *  @band: priority band to which to put
 *  @flags: normal or high priority flag
 *
 *  strputpmsg() implements the putpmsg(2) and putmsg(2) system calls for STREAMS.
 *
 *  Notices: GLIBC2 puts -1 in band when it is called as putmsg().
 */
int strputpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int band, int flags)
{
	struct stdata *sd = stri_lookup(file);
	mblk_t *mp = NULL, *dp;
	ssize_t clen, dlen;
	int err;
	if (sd->sd_directio && sd->sd_directio->putpmsg)
		return sd->sd_directio->putpmsg(file, ctlp, datp, band, flags);
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
		if ((band != -1 && (band < 0 || band > 255))
		    || (clen <= 0 && dlen <= 0 && !(sd->sd_wropt & SNDZERO)))
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

EXPORT_SYMBOL(strputpmsg);

/**
 *  strgetpmsg: - getpmsg file operation for a stream
 *  @file: file pointer for the stream
 *  @ctlp: &struct strbuf pointer receiving the ctrl part of the message
 *  @datp: &struct strbuf pointer receiving the data part of the message
 *  @bandp: pointer to integer to return the priority band of the received message
 *  @flagsp: pointer to integer to return the normal or high priority flag
 *
 *  strgetpmsg() implements the getpmsg(2) and getmsg(2) system calls for STREAMS.
 *
 *  Notices: If a hangup occurs on a stream from which messages are to be retrieved, getpmsg()
 *  continues to operate normally, until the stream head read queue is empty.  Thereafter it returns
 *  0 in the len fields of @ctlp and @datp.
 *
 *  GLIBC2 puts NULL in bandp for getmsg().
 */
int strgetpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int *bandp, int *flagsp)
{
	struct stdata *sd = stri_lookup(file);
	mblk_t *mp = NULL, *dp;
	if (sd->sd_directio && sd->sd_directio->getpmsg)
		return sd->sd_directio->getpmsg(file, ctlp, datp, bandp, flagsp);
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
	if (!IS_ERR(dp = ERR_PTR(check_stream_rd(file, sd)))
	    && !IS_ERR(dp = strwaitgmsg(file, sd, *flagsp, bandp ? *bandp : 0))) {
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

EXPORT_SYMBOL(strgetpmsg);

/* 
 *  -------------------------------------------------------------------------
 *
 *  ADDITIONAL PRIVATE IO CONTROLS
 *
 *  -------------------------------------------------------------------------
 * 
 *  Linux Fast-STREAMS System Call Emulation
 *  ----------------------------------------
 */

/**
 *  str_i_putpmsg: - perform putpmsg(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_PUTPMSG
 *  @arg: ioctl argument, a pointer to the &struct strpmsg structure
 */
static int str_i_putpmsg(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct strpmsg sp, *valp = (struct strpmsg *) arg;
	/* verify all areas */
	if ((err = verify_area(VERIFY_READ, valp, sizeof(*valp))))
		goto error;
	if (__copy_from_user(&sp, valp, sizeof(sp)))
		return (-EFAULT);
	if (sp.ctlbuf.len > 0 && (err = verify_area(VERIFY_READ, sp.ctlbuf.buf, sp.ctlbuf.len)) < 0)
		goto error;
	if (sp.databuf.len > 0
	    && (err = verify_area(VERIFY_READ, sp.databuf.buf, sp.databuf.len)) < 0)
		goto error;
	return strputpmsg(file, &sp.ctlbuf, &sp.databuf, sp.band, sp.flags);
      error:
	return (err);
}

/**
 *  str_i_getpmsg: - perform getpmsg(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_GETPMSG
 *  @arg: ioctl argument, a pointer to the &struct strpmsg structure
 */
static int str_i_getpmsg(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err;
	struct strpmsg sg, *valp = (struct strpmsg *) arg;
	/* verify all areas */
	if ((err = verify_area(VERIFY_WRITE, valp, sizeof(*valp))))
		goto error;
	if (__copy_from_user(&sg, valp, sizeof(sg)))
		return (-EFAULT);
	if (sg.ctlbuf.maxlen > 0
	    && (err = verify_area(VERIFY_WRITE, sg.ctlbuf.buf, sg.ctlbuf.maxlen)) < 0)
		goto error;
	if (sg.databuf.maxlen > 0
	    && (err = verify_area(VERIFY_WRITE, sg.databuf.buf, sg.databuf.maxlen)) < 0)
		goto error;
	return strgetpmsg(file, &sg.ctlbuf, &sg.databuf, &sg.band, &sg.flags);
      error:
	return (err);
}

/**
 *  strfattach: - fattach system call
 *  @file: file pointer for stream
 *  @path: path to which to fattach the stream
 */
static int strfattach(struct file *file, const char *path)
{
#if HAVE_KERNEL_FATTACH_SUPPORT
	return do_fattach(file, path);	/* see strattach.c */
#else
	return (-ENOSYS);
#endif
}

/**
 *  str_i_fattach: - perform fattach(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_FATTACH
 *  @arg: a pointer to a character string describing the path
 */
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

/**
 *  strfdetach: - fdetach system call
 *  @path: path from which to fdetach the stream
 */
static int strfdetach(const char *path)
{
#if HAVE_KERNEL_FATTACH_SUPPORT
	return do_fdetach(path);	/* see strattach.c */
#else
	return (-ENOSYS);
#endif
}

/**
 *  str_i_fdetach: - perform fdetach(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_FDETACH
 *  @arg: a pointer to a character string describing the path
 */
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

/**
 *  strpipe: - pipe system call
 *  @fds: array to which to return two file descriptors
 */
static int strpipe(int fds[2])
{
#if HAVE_KERNEL_PIPE_SUPPORT
	return do_spipe(fds);	/* see strpipe.c */
#else
	return (-ENOSYS);
#endif
}

/**
 *  str_i_pipe: - perform pipe(2) system call emulation as streamio(7) ioctl
 *  @file: file pointer for the stream
 *  @sd: stream head
 *  @cmd: ioctl command, always %I_PIPE
 *  @arg: pointer to array into which to receive two file descriptors
 */
static int str_i_pipe(struct file *file, struct stdata *sd, unsigned int cmd, unsigned long arg)
{
	int err, *fds = (int *) arg;
	if ((err = verify_area(VERIFY_WRITE, fds, 2 * sizeof(*fds))))
		goto error;
	return strpipe(fds);
      error:
	return (err);
}

/**
 *  strioctl: - ioctl file operation for a stream
 *  @inode: shadow special filsystem device inode
 *  @file: shadown special filesystem file pointer
 *  @cmd: ioctl command
 *  @arg: ioctl arg
 */
int strioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct stdata *sd = stri_lookup(file);
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	(void) size;
	trace();
	switch (cmd) {
	case FIOGETOWN:
	case FIOSETOWN:
	case SIOCSPGRP:
	case SIOCGPGRP:
	case SIOCATMARK:
	case SIOCGSTAMP:
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
 *  Stream Head file operations
 *
 *  -------------------------------------------------------------------------
 */
struct file_operations strm_f_ops ____cacheline_aligned = {
	owner:NULL,
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
#ifdef HAVE_PUTPMSG_GETPMSG_FILE_OPS
	getpmsg:strgetpmsg,
	putpmsg:strputpmsg,
#endif
};

EXPORT_SYMBOL(strm_f_ops);

/* 
 *  -------------------------------------------------------------------------
 *
 *  SRVP Write Service Procedure
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  strwsrv: - STREAM head write queue service procedure
 *  @q: write queue to service
 *
 *  We don't actually ever put a message on the write queue, we just use the service procedure to
 *  wake up any synchronous or asynchronous waiters waiting for flow control to subside.  This
 *  permit back-enabling from the module beneath the stream head to work properly.
 */
int strwsrv(queue_t *q)
{
	struct stdata *sd = q->q_ptr;
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
	return (0);
}

#if defined CONFIG_STREAMS_FIFO_MODULE || defined CONFIG_STREAMS_PIPE_MODULE
EXPORT_SYMBOL(strwsrv);
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUTP Read Put Procedure
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  strrput: - STREAM head read queue put procedure
 *  @q: pointer to the queue to which to put the message
 *  @mp: the message to put on the queue
 */
int strrput(queue_t *q, mblk_t *mp)
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
				strqset(q, QHIWAT, so->so_band, so->so_hiwat);
			if (so->so_flags & SO_LOWAT)
				strqset(q, QLOWAT, so->so_band, so->so_lowat);
		} else {
			if (so->so_flags & SO_HIWAT)
				strqset(q, QHIWAT, 0, so->so_hiwat);
			if (so->so_flags & SO_LOWAT)
				strqset(q, QLOWAT, 0, so->so_lowat);
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
			if (!test_and_set_bit(STRDERR_BIT, &sd->sd_flag)) {
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
			if (!test_and_set_bit(STWRERR_BIT, &sd->sd_flag)) {
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
		if (!test_and_set_bit(STRHUP_BIT, &sd->sd_flag)) {
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
		if (!sd->sd_iocblk && test_bit(IOCWAIT_BIT, &sd->sd_flag)
		    && !(sd->sd_flag & (STRDERR | STWRERR))
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
		if (!sd->sd_iocblk && test_bit(IOCWAIT_BIT, &sd->sd_flag)
		    && !(sd->sd_flag & (STRDERR | STWRERR))
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

#if defined CONFIG_STREAMS_FIFO_MODULE || defined CONFIG_STREAMS_PIPE_MODULE
EXPORT_SYMBOL(strrput);
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  str_open:	- STREAMS qopen procedure for stream heads
 *  @q:		read queue of stream to open
 *  @devp:	pointer to a dev_t from which to read and into which to return the device number
 *  @oflag:	open flags
 *  @sflag:	STREAMS flags (%DRVOPEN or %MODOPEN or %CLONEOPEN)
 *  @crp:	pointer to user's credentials structure
 */
static int str_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int err;
	struct stdata *sd;
	ptrace(("%s: stream head open for major %hu, minor %hu, sflag %d, crp %p\n", __FUNCTION__,
		getmajor(*devp), getminor(*devp), sflag, crp));
	if (q->q_ptr != NULL) {
		/* already open: we walk down the queue chain calling open on each of the modules
		   and the driver */
		queue_t *wq = WR(q), *wq_next;
		printd(("%s: stream head already open, opening down\n", __FUNCTION__));
		wq_next = SAMESTR(wq) ? wq->q_next : NULL;
		while ((wq = wq_next)) {
			int new_sflag;
			wq_next = SAMESTR(wq) ? wq->q_next : NULL;
			new_sflag = wq_next ? MODOPEN : sflag;
			printd(("%s: calling qopen for module/driver\n", __FUNCTION__));
			if ((err = qopen(wq - 1, devp, oflag, new_sflag, crp)))
				goto error;
		}
		return (0);	/* already open */
	}
	if (sflag == DRVOPEN || sflag == CLONEOPEN || WR(q)->q_next == NULL) {
		printd(("%s: stream head first open\n", __FUNCTION__));
		if ((sd = ((struct queinfo *) q)->qu_str)) {
			struct cdevsw *cdev = sd->sd_cdevsw;
			struct fmodsw *fmod = (struct fmodsw *) cdev;
			struct streamtab *st;
			printd(("%s: performing qattach\n", __FUNCTION__));
			/* 1st step: attach the driver and call its open routine */
			st = sd->sd_strtab = cdev->d_str;
			if ((err = qattach(sd, fmod, devp, oflag, sflag, crp)))
				goto error;
			/* 2nd step: check for redirected return */
			/* qattach() above does the right thing with regard to setq() */
			/* 3rd step: autopush modules and call their open routines */
			printd(("%s: performing autopush\n", __FUNCTION__));
			if ((err = autopush(sd, cdev, devp, oflag, MODOPEN, crp)))
				goto error;
			/* lastly, attach our privates and return */
			q->q_ptr = WR(q)->q_ptr = sd;
			return (0);
		}
	}
	return (-EIO);		/* can't be opened as module or clone */
      error:
	return (err > 0 ? -err : err);
}

/**
 *  str_close: - STREAMS qclose procedure for stream heads
 *  @q: read queue of stream to close
 *  @oflag: open flags
 *  @crp: pointer to user's credentials structure
 */
static int str_close(queue_t *q, int oflag, cred_t *crp)
{
	if (!q->q_ptr || q->q_ptr != ((struct queinfo *) q)->qu_str)
		return (ENXIO);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for character based streams, fifos and pipes.
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  cdev_open: - open a character special device node
 *  @inode: the character device inode
 *  @file: the user file pointer
 *
 *  cdev_open() is only used to open a stream from a character device node in an external
 *  filesystem.  This is never called for direct opens of a specfs device node (for direct opens see
 *  spec_dev_open() in strspecfs.c).  It is also not used for direct opens of fifos, pipes or
 *  sockets.  Those devices provide their own file operations to the main operating system.  The
 *  character device number from the inode is used to determine the shadow special file system
 *  (internal) inode and chain the open call.
 *
 *  This is the separation point where we convert the external device number to an internal device
 *  number.  The external device number is contained in inode->i_rdev.
 */
STATIC int cdev_open(struct inode *inode, struct file *file)
{
	int err;
	struct cdevsw *cdev;
	struct devnode *cmin;
	major_t major;
	minor_t minor;
	modID_t modid;
	dev_t dev;
	int sflag;
	ptrace(("%s: opening character device\n", __FUNCTION__));
	if ((err = down_interruptible(&inode->i_sem)))
		goto exit;
#if HAVE_KFUNC_TO_KDEV_T
	minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	major = MAJOR(kdev_t_to_nr(inode->i_rdev));
#else
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
#endif
	printd(("%s: character device external major %hu, minor %hu\n", __FUNCTION__, major,
		minor));
	err = -ENXIO;
	if (!(cdev = sdev_get(major)))
		goto up_exit;
	printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
	printd(("%s: major maps to streams driver %s\n", __FUNCTION__, cdev->d_name));
	minor = cdev_minor(cdev, major, minor);
	printd(("%s: minor maps to internal minor %hu\n", __FUNCTION__, minor));
	major = cdev->d_major;
	printd(("%s: major maps to base major %hu\n", __FUNCTION__, major));
	modid = cdev->d_modid;
	printd(("%s: major maps to internal major %hu\n", __FUNCTION__, modid));
	dev = makedevice(modid, minor);
	sflag = (cmin = cmin_get(cdev, minor)) ?
	    ((cmin->n_flag & D_CLONE) ? CLONEOPEN : DRVOPEN) :
	    ((cdev->d_flag & D_CLONE) ? CLONEOPEN : DRVOPEN);
	err = spec_open(inode, file, dev, sflag);
	printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
	sdev_put(cdev);
      up_exit:
	up(&inode->i_sem);
      exit:
	return (err);
}

STATIC struct file_operations cdev_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:cdev_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  REGISTRATION
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  register_strdev: - register a STREAMS device against a device major number
 *  @cdev: STREAMS character device structure to register
 *  @major: requested major device number or 0 for automatic major selection
 *
 *  register_strdev() registers the device specified by the @cdev to the device major number
 *  specified by @major.
 *
 *  register_strdev() will register the STREAMS character device specified by @cdev against the
 *  major device number @major.  If the major device number is zero, then it requests that
 *  register_strdev() allocate an available major device number and assign it to @cdev.
 *
 *  Context: register_strdev() is intended to be called from kernel __init() or module_init()
 *  routines only.  It cannot be called from in_irq() level.
 *
 *  Return Values: Upon success, register_strdev() will return the requested or assigned major
 *  device number as a positive integer value.  Upon failure, the registration is denied and a
 *  negative error number is returned.
 *
 *  Errors: Upon failure, register_strdev() returns on of the negative error numbers listed below.
 *
 *  -[%ENOMEM]	insufficient memory was available to complete the request.
 *
 *  -[%EINVAL]	@cdev was NULL
 *
 *  -[%EBUSY]	a device was already registered against the requested major device number, or no
 *	        device numbers were available for automatic major device number assignment.
 *
 *  Notes: Linux Fast-STREAMS provides improvements over LiS.
 *
 *  LfS uses a small hash instead of a cdevsw[] table and requires that the driver (statically)
 *  allocate its &struct cdevsw structure using an approach more likened to the Solaris &struct
 *  cb_ops.
 */
int register_strdev(struct cdevsw *cdev, major_t major)
{
	int err;
	if (!cdev->d_fop)
		cdev->d_fop = &strm_f_ops;
	if (!(cdev->d_mode & S_IFMT))
		cdev->d_mode = (cdev->d_mode & ~S_IFMT) | S_IFCHR;
	if ((err = register_cmajor(cdev, major, &cdev_f_ops)) < 0)
		return (err);
	register_clone(cdev);
	return (err);
}

EXPORT_SYMBOL(register_strdev);

/**
 *  unregister_strdev: - unregister previously registered STREAMS device
 *  @cdev: STREAMS character device structure to unregister
 *  @major: major device number to unregister or 0 for all majors
 *
 *  unregister_strdev() unregisters the device specified by the @cdev from the device major number
 *  specified by @dev.  Only the getmajor(@dev) component of @dev is significant and the
 *  getminor(@dev) component must be coded zero (0).
 *
 *  unregister_strdev() will unregister the STREAMS character device specified by @cdev from the
 *  major device number in getmajor(@dev).  If the major device number is zero, then it requests
 *  that unregister_strdev() unregister @cdev from any device majors with which it is currently
 *  registered.
 *
 *  Context: unregister_strdev() is intended to be called from kernel __exit() or module_exit()
 *  routines only.  It cannot be called from in_irq() level.
 *
 *  Return Values: Upon success, unregister_strdev() will return zero (0).  Upon failure, the
 *  deregistration is denied and a negative error number is returned.
 *
 *  Errors: Upon failure, unregister_strdev() returns one of the negative error numbers listed
 *  below.
 *
 *  -[%ENXIO]	The specified device does not exist in the registration tables.
 *
 *  -[%EINVAL]	@cdev is NULL, or the @d_name component associated with @cdev has changed since
 *              registration.
 *
 *  -[%EPERM]	The device number specified does not belong to the &struct cdev structure specified
 *		and permission is therefore denied.
 */
int unregister_strdev(struct cdevsw *cdev, major_t major)
{
	unregister_clone(cdev);
	return unregister_cmajor(cdev, major);
}

EXPORT_SYMBOL(unregister_strdev);

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

/* Note: we do not register stream heads - the above operations are exported and used by all
   regular STREAMS devices */

/* Perhaps we should...  The stream head should be entered into the fmodsw table at position 0,
   with module id 0 */

/* We actually register it a position 1 with module id 1 */

static struct fmodsw sth_fmod = {
	f_name:CONFIG_STREAMS_STH_NAME,
	f_str:&str_info,
	f_flag:D_MP,
	f_kmod:THIS_MODULE,
};

/* bleedin' mercy! */
static inline void put_filesystem(struct file_system_type *fs)
{
#if HAVE_KFUNC_MODULE_PUT
	module_put(fs->owner);
#else
	if (fs->owner)
		__MOD_DEC_USE_COUNT(fs->owner);
#endif
}

#ifdef CONFIG_STREAMS_STH_MODULE
static
#endif
int __init sth_init(void)
{
	int result;
#ifdef CONFIG_STREAMS_STH_MODULE
	printk(KERN_INFO STH_BANNER);
#else
	printk(KERN_INFO STH_SPLASH);
#endif
	str_minfo.mi_idnum = modid;
	if ((result = register_strmod(&sth_fmod)) < 0)
		goto no_strmod;
	if (modid == 0 && result >= 0)
		modid = result;
	return (0);
      no_strmod:
	return (result);
}

#ifdef CONFIG_STREAMS_STH_MODULE
static
#endif
void __exit sth_exit(void)
{
	unregister_strmod(&sth_fmod);
}

#ifdef CONFIG_STREAMS_STH_MODULE
module_init(sth_init);
module_exit(sth_exit);
#endif				/* CONFIG_STREAMS_STH_MODULE */
