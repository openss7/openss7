/*****************************************************************************

 @(#) strfifo.c,v (0.9.2.16) 2003/10/26 17:25:54

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

 Last Modified 2003/10/26 17:25:54 by brian

 *****************************************************************************/

#ident "@(#) LfS strfifo.c,v (0.9.2.16) 2003/10/26 17:25:54"

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <asm/semaphore.h>

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strreg.h"		/* struct str_args */
#include "strsched.h"		/* allocsd(), freesd() */
#include "strhead.h"		/* for autopush */
#include "strfifo.h"		/* extern verification */

#define FIFO_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define FIFO_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define FIFO_REVISION	"LfS strfifo.c,v (0.9.2.16) 2003/10/26 17:25:54"
#define FIFO_DEVICE	"SVR 4.2 STREAMS-based FIFOs"
#define FIFO_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define FIFO_LICENSE	"GPL and additional rights"
#define FIFO_BANNER	FIFO_DESCRIP	"\n" \
			FIFO_COPYRIGHT	"\n" \
			FIFO_REVISION	"\n" \
			FIFO_DEVICE	"\n" \
			FIFO_CONTACT	"\n"
#define FIFO_SPLASH	FIFO_DEVICE	" - " \
			FIFO_REVISION	"\n"

MODULE_AUTHOR(FIFO_CONTACT);
MODULE_DESCRIPTION(FIFO_DESCRIP);
MODULE_SUPPORTED_DEVICE(FIFO_DEVICE);
MODULE_LICENSE(FIFO_LICENSE);

#ifndef CONFIG_STREAMS_FIFOS_NAME
#define CONFIG_STREAMS_FIFOS_NAME "fifo"
//#error "CONFIG_STREAMS_FIFOS_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_FIFOS_MODID
#define CONFIG_STREAMS_FIFOS_MODID 6
//#error "CONFIG_STREAMS_FIFOS_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_FIFOS_MAJOR
#define CONFIG_STREAMS_FIFOS_MAJOR 0
//#error "CONFIG_STREAMS_FIFOS_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_FIFOS_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for STREAMS-based FIFOs.");

#if 0
/* 
 *  FIFOREAD
 *  ---------------------------------------------------------------------------
 */
static mblk_t *fifowaitread(struct stdata *sd, long *timeo)
{
	DECLARE_WAITQUEUE(wait, current);
	mblk_t *mp;
	add_wait_queue(&sd->sd_waitq, &wait);
	sd->sd_rwaiters++;
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (test_bit(STRDERR_BIT, &sd->sd_flag))
			goto rderror;
		if (test_bit(STRHUP_BIT, &sd->sd_flag))
			goto epipe;
		if (signal_pending(current))
			goto eintr;
		if ((mp = getq(sd->sd_rq))) {
			clear_bit(RSLEEP_BIT, &sd->sd_flag);
			break;
		}
		if (!*timeo)
			goto eagain;
		set_bit(RSLEEP_BIT, &sd->sd_flag);
		*timeo = schedule_timeout(*timeo);
	}
      out:
	sd->sd_rwaiters--;
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&sd->sd_waitq, &wait);
	return (mp);
      eagain:
	mp = ERR_PTR(-EAGAIN);
	goto out;
      epipe:
	mp = ERR_PTR(-EPIPE);
	goto out;
      eintr:
	mp = ERR_PTR(*timeo == MAX_SCHEDULE_TIMEOUT ? -ERESTARTSYS : -EINTR);
	goto out;
      rderror:
	mp = ERR_PTR(sd->sd_rerror ? sd->sd_rerror : -EPIPE);
	goto out;
}
#endif

#define stri_lookup(__f) ((struct stdata *)(__f)->private_data)
#define sdev_lookup(__i) ((struct cdevsw *)(__i)->i_cdev->data)

static ssize_t fiforeadv(struct file *file, const struct iovec *, unsigned long len, loff_t *ppos);
static ssize_t fiforead(struct file *file, char *buf, size_t len, loff_t *ppos)
{
	struct iovec iov;
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return fiforeadv(file, &iov, 1, ppos);
}

static ssize_t fifowritev(struct file *file, const struct iovec *iov, unsigned long count,
			  loff_t *ppos);
static ssize_t fifowrite(struct file *file, const char *buf, size_t len, loff_t *ppos)
{
	struct iovec iov;
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return fifowritev(file, &iov, 1, ppos);
}

static struct streamtab fifo_info;
static struct qinit fifo_rinit;
static struct qinit fifo_winit;
 /* Because we are called directly from a fifo or character node in the filesystem, the indode and
    file pointer are the real inode and file pointer in the filesystem.  We don't do any specfs
    creation of any inodes, we do not remap the dentry and inode, we simply attach a stream head on
    the first open and use it after that.  We don't care whether the filesystem node that was opened 
    was an S_IFIFO or an S_IFCHR node, we use the STREAMS-fifo device number anyway.  */
static int fifoopen(struct inode *inode, struct file *file)
{
	struct stdata *sd;
	int err = 0;
	/* either the FIFO is already open or we need a stream head */
	if (!(sd = stri_lookup(file)) &&
	    (!(sd = sd_get((struct stdata *)inode->i_pipe)) || (sd->sd_flag & STRCLONE))) {
		queue_t *q;
		if (!(q = allocq()))
			return (-ENOSR);
		/* FIXME: need to find/create and attach synq.  We are going to need the devinfo
		   structure for this.  We need to get it from the dev number. */
		if (!(sd = allocsd())) {
			freeq(q);
			return (-ENOSR);
		}
		/* initialization of the stream head */
		sd->sd_strtab = &fifo_info;	/* we *are* the driver */
		sd->sd_wropt = SNDZERO | SNDPIPE;	/* special write options */
		sd->sd_flag = STWOPEN;
		sd->sd_rq = q;
		sd->sd_wq = q + 1;
		sd->sd_other = sd;
		((struct queinfo *) q)->qu_str = sd;
		setq(q, fifo_info.st_rdinit, fifo_info.st_wrinit);
		/* grafting onto inode */
		sd->sd_inode = inode;	/* real inode */
		inode->i_pipe = (void *)sd;
		/* done setup, do the open */
	} else if (test_and_set_bit(STWOPEN_BIT, &sd->sd_flag)) {
		DECLARE_WAITQUEUE(wait, current);
		add_wait_queue(&sd->sd_waitq, &wait);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (sd->sd_flag & (STPLEX | STRCLOSE | STRHUP | STRDERR | STWRERR)) {
				err = -EINVAL;
				if (test_bit(STPLEX_BIT, &sd->sd_flag))
					break;
				err = -EIO;
				if (test_bit(STRCLOSE_BIT, &sd->sd_flag))
					break;
				err = -EPIPE;
				if (test_bit(STRHUP_BIT, &sd->sd_flag))
					break;
				err = sd->sd_rerror;
				if (test_bit(STRDERR_BIT, &sd->sd_flag))
					if ((file->f_mode & FREAD) && err)
						break;
				err = sd->sd_werror;
				if (test_bit(STWRERR_BIT, &sd->sd_flag))
					if ((file->f_mode & FWRITE) && err)
						break;
			}
			err = -EPIPE;
			if (sd->sd_other == NULL)
				break;
			err = -EPIPE;
			if (test_bit(STRCLOSE_BIT, &sd->sd_other->sd_flag))
				break;
			err = 0;
			if (!test_and_set_bit(STWOPEN_BIT, &sd->sd_flag))
				break;
			schedule();
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sd->sd_waitq, &wait);
	}
	if (err == 0) {
		dev_t dev = makedevice(major, 0);
		int oflag = make_oflag(file);
		int sflag = (sd->sd_flag & STRCLONE) ? CLONEOPEN : DRVOPEN;
		cred_t *crp = current_creds;
		/* here we hold the STWOPEN bit - fifo_open must clear */
		err = qopen(sd->sd_rq, &dev, oflag, sflag, crp);
		clear_bit(STWOPEN_BIT, &sd->sd_flag);
		/* wake up anybody waiting on open bit */
		if (waitqueue_active(&sd->sd_waitq))
			wake_up_interruptible(&sd->sd_waitq);
	}
	if (err != 0)
		sd_put(sd);
	else {
		sd->sd_opens++;
		sd->sd_readers += (file->f_mode & FREAD) ? 1 : 0;
		sd->sd_writers += (file->f_mode & FWRITE) ? 1 : 0;
	}
	return (err <= 0 ? err : -err);
}

static int fifoclose(struct inode *inode, struct file *file)
{
	struct stdata *sd;
	if ((sd = sd_get(stri_lookup(file)))) {
		stri_lookup(file) = NULL;
		sd->sd_opens--;
		sd->sd_readers -= (file->f_mode & FREAD) ? 1 : 0;
		sd->sd_writers -= (file->f_mode & FWRITE) ? 1 : 0;
		sd_put(sd);
		return (0);
	}
	return (-ENOSTR);
}

static int check_fifo_rd(struct file *file, struct stdata *sd)
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
static mblk_t *fifogetq(struct stdata *sd, int flags, int band)
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

 /* wait to get a message of a given type */
static mblk_t *fifowaitgmsg(struct file *file, struct stdata *sd, int flags, int band)
{
	mblk_t *mp;
	/* maybe we'll get lucky... */
	/* also we must make an attempt before returning ENXIO, EPIPE or rderror */
	if ((mp = fifogetq(sd, flags, band)))
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
			if ((mp = fifogetq(sd, flags, band)))
				break;
			timeo = schedule_timeout(timeo);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&sd->sd_waitq, &wait);
		return (mp);
	}
}
static ssize_t fiforeadv(struct file *file, const struct iovec *iov, unsigned long len,
			 loff_t *ppos)
{
	struct stdata *sd = stri_lookup(file);
	queue_t *q = sd->sd_rq;
	mblk_t *mp;
	int err, count, total = 0;
	if (ppos != &file->f_pos)
		return (-ESPIPE);
	if (len == 0)
		return (0);
	if ((err = check_fifo_rd(file, sd))) {
		if (err == -ENXIO)
			return (0);	/* end of file on hangup */
		return (err);
	}
	for (;;) {
		if ((err = PTR_ERR(mp = fifowaitgmsg(file, sd, MSG_BAND, 0))) < 0)
			return (err);
		switch (sd->sd_rdopt & (RPROTDAT | RPROTDIS | RPROTNORM)) {
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
			if (mp->b_datap->db_type != M_DATA)
				mp->b_datap->db_type = M_DATA;
			break;
		}
		case RPROTDIS:
		{
			mblk_t *dp, **mpp, **mpp_next = &mp;
			/* M_PROTO messages are discarded.  Discard control part of message,
			   delivering any data part.  The control part of the message is discarded
			   and the data part is processed. */
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
		/* All read modes terminate on a zero length message. */
		if (!(count = msgsize(mp)))
			goto free_total;
		if (count > len - total)
			count = len - total;
		if (!count)
			goto putback_total;
		switch (sd->sd_rdopt & (RNORM | RMSGD | RMSGN)) {
		case RNORM:
		{
			/* The default mode.  This is the normal byte-stream mode where message
			   boundaries are ignored; fiforead() returns data until the read count has
			   been satisfied or a zero length message is received. */
			while (mp && total < len) {
				/* FIXME: read the message */
			}
			if (mp)
				putbq(q, mp);
			if (len == total)
				return (total);
			break;
		}
		case RMSGD:
		{
			/* The read will return when either the count is satisfied, a zero length
			   message is received, or a message boundary is encountered.  If there is
			   any data left in a message after the read count has been satisfied, the
			   message is placed back on the read queue.  The data will be read on a
			   subsequent read call. */
			if (total > 0 && mp->b_flag & MSGDELIM)
				goto putback_total;
			while (mp && total < len) {
				/* FIXME: read the message */
			}
			if (mp) {
				mp->b_flag &= ~MSGDELIM;
				putbq(q, mp);
			}
			if (len == total)
				return (total);
			break;
		}
		case RMSGN:
		{
			/* Similar to RMSGN but data that remains in a message after the read count 
			   has been satisfied is discarded. */
			if (total > 0 && mp->b_flag & MSGDELIM)
				goto putback_total;
			while (mp && total < len) {
				/* FIXME: read the message */
			}
			if (len == total) {
				if (mp)
					freemsg(mp);
				return (total);
			}
			break;
		}
		}
	}
      free_total:
	freemsg(mp);
	return (total);
      putback_total:
	putbq(q, mp);
	return (total);
}

static ssize_t fifowritev(struct file *file, const struct iovec *iov, unsigned long count,
			  loff_t *ppos)
{
	return strwritev(file, iov, count, ppos);
}

static ssize_t fifosendpage(struct file *file, struct page *page, int offset, size_t size,
			    loff_t *ppos, int more)
{
	return strsendpage(file, page, offset, size, ppos, more);
}

static struct file_operations fifo_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	llseek:strllseek,
	read:fiforead,
	write:fifowrite,
	poll:strpoll,
	ioctl:strioctl,
	mmap:strmmap,
	open:fifoopen,
	release:fifoclose,
	fasync:strfasync,
	readv:fiforeadv,
	writev:fifowritev,
	sendpage:fifosendpage,
//	getpmsg:strgetpmsg,
//	putpmsg:strputpmsg,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static struct cdevsw fifo_cdev;
static int fifo_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int err = 0;
	struct stdata *sd;
	/* Here's the rest of the fifo open. */
	if ((sd = q->q_ptr) == NULL) {
		if (sflag != DRVOPEN || WR(q)->q_next != NULL)
			return (-ENXIO);
		sd = ((struct queinfo *) q)->qu_str;
		/* 1st step: attach the driver and call its open routine */
		/* we are the driver and we are the driver's open routine */
		/* attach our queue pointers together */
		WR(q)->q_next = q;
		/* 2nd step: check for redirected return */
		/* we don't actually call, so we can't have a redirected return */
		/* 3rd step: autopush modules and call their open routine */
		if ((err = autopush(sd, &fifo_cdev, devp, oflag, MODOPEN, crp)))
			return (err < 0 ? err : -err);
	}
	switch (oflag & (FNDELAY | FWRITE | FREAD)) {
	case (0):
	case (FNDELAY):
		err = -EINVAL;
		break;
	case (FWRITE):
		if (sd->sd_readers <= 0) {
			DECLARE_WAITQUEUE(wait, current);
			add_wait_queue(&sd->sd_waitq, &wait);
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				err = -ERESTARTSYS;
				if (signal_pending(current))
					break;
				err = 0;
				if (sd->sd_readers > 0)
					break;
				up(&sd->sd_inode->i_sem);
				schedule();
				down(&sd->sd_inode->i_sem);
			}
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&sd->sd_waitq, &wait);
		}
		break;
	case (FWRITE | FNDELAY):
		if (sd->sd_readers > 0)
			break;
		err = -ENXIO;
		break;
	case (FREAD):
		if (sd->sd_writers <= 0) {
			DECLARE_WAITQUEUE(wait, current);
			add_wait_queue(&sd->sd_waitq, &wait);
			for (;;) {
				set_current_state(TASK_INTERRUPTIBLE);
				err = -ERESTARTSYS;
				if (signal_pending(current))
					break;
				err = 0;
				if (sd->sd_writers > 0)
					break;
				up(&sd->sd_inode->i_sem);
				schedule();
				down(&sd->sd_inode->i_sem);
			}
		}
		break;
	case (FREAD | FNDELAY):
		break;
	case (FREAD | FWRITE):
	case (FREAD | FWRITE | FNDELAY):
		break;
	}
	if (!err) {
		sd->sd_opens++;
		sd->sd_readers += (oflag & FREAD) ? 1 : 0;
		sd->sd_writers += (oflag & FWRITE) ? 1 : 0;
		if (q->q_ptr == NULL)
			/* lastly: attach our privates and return */
			q->q_ptr = WR(q)->q_ptr = sd;
	}
	clear_bit(STWOPEN_BIT, &sd->sd_flag);
	/* wake up anybody wiating on open bit */
	if (waitqueue_active(&sd->sd_waitq))
		wake_up_interruptible(&sd->sd_waitq);
	return (err);
}
static int fifo_close(queue_t *q, int oflag, cred_t *crp)
{
	return (0);
}

static int fifo_rput(queue_t *q, mblk_t *mp)
{
	return (0);
}
static int fifo_rsrv(queue_t *q)
{
	return (0);
}
static int fifo_wput(queue_t *q, mblk_t *mp)
{
	return (0);
}
static int fifo_wsrv(queue_t *q)
{
	return (0);
}

static struct module_info fifo_minfo = {
	mi_idnum:CONFIG_STREAMS_FIFOS_MODID,
	mi_idname:CONFIG_STREAMS_FIFOS_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static struct qinit fifo_rinit = {
	qi_putp:fifo_rput,
	qi_srvp:fifo_rsrv,
	qi_qopen:fifo_open,
	qi_qclose:fifo_close,
	qi_minfo:&fifo_minfo,
};

static struct qinit fifo_winit = {
	qi_putp:fifo_wput,
	qi_srvp:fifo_wsrv,
	qi_minfo:&fifo_minfo,
};

static struct streamtab fifo_info = {
	st_rdinit:&fifo_rinit,
	st_wrinit:&fifo_winit,
};

static struct cdevsw fifo_cdev = {
	d_name:CONFIG_STREAMS_FIFOS_NAME,
	d_str:&fifo_info,
	d_flag:0,
	d_fop:&fifo_f_ops,
	d_mode:S_IFIFO,
	d_kmod:THIS_MODULE,
};

static int open_fifo(struct inode *inode, struct file *file)
{
	struct str_args args = {
		file:file,
		dev:makedevice(major, 0),
		oflag:make_oflag(file),
		sflag:DRVOPEN,
		crp:current_creds,
		name:{args.buf, 0, 0},
	};
	void *fsdata = xchg(&file->f_dentry->d_fsdata, &args);
	int err;
	err = fifoopen(inode, file);
	file->f_dentry->d_fsdata = fsdata;
	return (err);
};

static struct file_operations fifo_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:open_fifo,
};

static int __init fifo_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO FIFO_BANNER);
#else
	printk(KERN_INFO FIFO_SPLASH);
#endif
	if ((err = register_inode(makedevice(major, 0), &fifo_cdev, &fifo_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit fifo_exit(void)
{
	int err;
	if ((err = unregister_inode(makedevice(major, 0), &fifo_cdev)))
		return (void) (err);
	return (void) (0);
};

module_init(fifo_init);
module_exit(fifo_exit);
