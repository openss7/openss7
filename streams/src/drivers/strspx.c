/*****************************************************************************

 @(#) strspx.c,v (0.9.2.9) 2003/10/26 17:25:55

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

 Last Modified 2003/10/26 17:25:55 by brian

 *****************************************************************************/

#ident "@(#) strspx.c,v (0.9.2.9) 2003/10/26 17:25:55"

static char const ident[] = "strspx.c,v (0.9.2.9) 2003/10/26 17:25:55";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strreg.h"		/* for struct str_args */
#include "strsched.h"		/* for sd_get/sd_put */
#include "strhead.h"		/* for autopush */
#include "strpipe.h"		/* for pipe stuff */

#define SPX_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SPX_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define SPX_REVISION	"LfS strspx.c,v (0.9.2.9) 2003/10/26 17:25:55"
#define SPX_DEVICE	"SVR 4.2 STREAMS-based PIPEs"
#define SPX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SPX_LICENSE	"GPL"
#define SPX_BANNER	SPX_DESCRIP	"\n" \
			SPX_COPYRIGHT	"\n" \
			SPX_REVISION	"\n" \
			SPX_DEVICE	"\n" \
			SPX_CONTACT	"\n"
#define SPX_SPLASH	SPX_DEVICE	" - " \
			SPX_REVISION	"\n"

MODULE_AUTHOR(SPX_CONTACT);
MODULE_DESCRIPTION(SPX_DESCRIP);
MODULE_SUPPORTED_DEVICE(SPX_DEVICE);
MODULE_LICENSE(SPX_LICENSE);

#ifndef CONFIG_STREAMS_SPX_NAME
#define CONFIG_STREAMS_SPX_NAME "spx"
//#error "CONFIG_STREAMS_SPX_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_SPX_MODID
#define CONFIG_STREAMS_SPX_MODID 10
//#error "CONFIG_STREAMS_SPX_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_SPX_MAJOR
#define CONFIG_STREAMS_SPX_MAJOR 0
//#error "CONFIG_STREAMS_SPX_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_SPX_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for STREAMS-based PIPEs (0 for allocation).");

static struct module_info spx_minfo = {
	mi_idnum:CONFIG_STREAMS_SPX_MODID,
	mi_idname:CONFIG_STREAMS_SPX_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

extern struct file_operations pipe_f_ops;

#define stri_lookup(__f) ((struct stdata *)(__f)->private_data)
#define sdev_lookup(__i) ((struct cdevsw *)(__i)->i_cdev->data)

/* 
 *  SPXLSEEK
 *  -------------------------------------
 */
static loff_t spxllseek(struct file *file, loff_t off, int whence)
{
	return pipe_f_ops.llseek(file, off, whence);
}

/* 
 *  SPXREAD
 *  -------------------------------------
 */
#if 0
static mblk_t *spxwaitread(struct stdata *sd, long *timeo)
{
	DECLARE_WAITQUEUE(wait, current);
	// mblk_t *mp;
	add_wait_queue(&sd->sd_waitq, &wait);
	sd->sd_rwaiters++;
	for (;;) {
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
	mp = ERR_PTR(*timeo = MAX_SCHEDULE_TIMEOUT ? -ERESTARTSYS : -EINTR);
	goto out;
      rderror:
	mp = ERR_PTR(sd->sd_rerror ? sd->sd_rerror : -EPIPE);
	goto out;
}
#endif
static ssize_t spxreadv(struct file *file, const struct iovec *iov, unsigned long len,
			loff_t *ppos);
static ssize_t spxread(struct file *file, char *buf, size_t len, loff_t *ppos)
{
	struct iovec iov;
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return spxreadv(file, &iov, 1, ppos);
}

/* 
 *  SPXWRITE
 *  -------------------------------------
 */
static ssize_t spxwrite(struct file *file, const char *buf, size_t len, loff_t *ppos)
{
	return pipe_f_ops.write(file, buf, len, ppos);
}

/* 
 *  SPXPOLL
 *  -------------------------------------
 */
static unsigned int spxpoll(struct file *file, struct poll_table_struct *poll)
{
	return pipe_f_ops.poll(file, poll);
}

/* 
 *  SPXIOCTL
 *  -------------------------------------
 */
static int spxioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct stdata *sd = stri_lookup(file);
	switch (cmd) {
	case I_FDINSERT:
	{
		struct stdata *sd2;
		/* I_FDINSERT for spx is a rather special ioctl.  It is used under UnixWare7 to
		   connect two disjoint stream heads into a STREAMS-based bidirectional pipe. */
		if (!sd || sd->sd_other)
			break;
		/* Once the pipes ends are connected, we no longer consider this IO control as
		   special. */
		sd2 = stri_lookup(f2);
		if (!sd2)
			return (-EINVAL);	/* XXX */
		/* link stream heads */
		sd->sd_other = sd2;
		sd2->sd_other = sd;
		/* link queues */
		sd->sd_wq->q_next = sd2->sd_rq;
		sd2->sd_wq->q_next = sd->sd_rq;
		return (0);
	}
	}
	return pipe_f_ops.ioctl(inode, file, cmd, arg);
}

/* 
 *  SPXMMAP
 *  -------------------------------------
 */
static int spxmmap(struct file *file, struct vm_area_struct *vma)
{
	return pipe_f_ops.mmap(file, vma);
}

/* 
 *  SPXOPEN
 *  -------------------------------------
 */
 /* Pipes are never associated with a filesystem entry outside the special filesystem.  Therefore,
    the passed in file pointer is always the real file pointer.  */
static struct streamtab spx_info;
static struct qinit spx_rinit;
static struct qinit spx_winit;
static int spxopen(struct inode *inode, struct file *file)
{
	struct str_args *argp = file->f_dentry->d_fsdata;
	int err = 0;
	struct stdata *sd;
	/* first find out if we already have a stream head (or need a new one anyway) */
	if (!(sd = sd_get((struct stdata *)inode->i_pipe)) || (sdev_lookup(inode)->d_flag & D_CLONE)) {
		/* We only do not have a stream head on initial open of the inode.  This only
		   occurs in response to a pipe(2) or s_pipe(3) system call */
		queue_t *q;
		/* we don't have a stream yeat, so allocate one */
		if (!(q = allocq()))
			return (-ENOSR);
		/* FIXME: need to find/create and attach synq.  We are going to need the devinfo
		   structure for this.  Actually, it is the same as the cdevsw structure. */
		if (!(sd = allocsd())) {
			freeq(q);
			return (-ENOSR);
		}
		/* initialization of the stream head */
		sd->sd_strtab = &spx_info;	/* we *are* the driver */
		sd->sd_wropt = SNDZERO | SNDPIPE;
		sd->sd_flag = STWOPEN;
		sd->sd_rq = q;
		sd->sd_wq = q + 1;
		((struct queinfo *) q)->qu_str = sd;
		setq(q, spx_info.st_rdinit, spx_info.st_wrinit);
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
		/* here we hold the STWOPEN bit and a reference to the stream head, spx_open must
		   clear the STWOPEN bit */
		err = qopen(sd->sd_wq, &argp->dev, argp->oflag, argp->sflag, argp->crp);
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

/* 
 *  SPXFLUSH
 *  -------------------------------------
 */
static int spxflush(struct file *file)
{
	return pipe_f_ops.flush(file);
}

/* 
 *  SPXCLOSE
 *  -------------------------------------
 */
static int spxclose(struct inode *inode, struct file *file)
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

/* 
 *  SPXFASYNC
 *  -------------------------------------
 */
static int spxfasync(int fd, struct file *file, int on)
{
	return pipe_f_ops.fasync(fd, file, on);
}

/* 
 *  SPXREADV
 *  -------------------------------------
 */
static ssize_t spxreadv(struct file *file, const struct iovec *iov, unsigned long len, loff_t *ppos)
{
	return pipe_f_ops.readv(file, iov, len, ppos);
}

/* 
 *  SPXWRITEV
 *  -------------------------------------
 */
static ssize_t spxwritev(struct file *file, const struct iovec *iov, unsigned long count,
			 loff_t *ppos)
{
	return pipe_f_ops.writev(file, iov, count, ppos);
}

/* 
 *  SPXSENDPAGE
 *  -------------------------------------
 */
static ssize_t spxsendpage(struct file *file, struct page *page, int offset, size_t size,
			   loff_t *ppos, int more)
{
	return pipe_f_ops.sendpage(file, page, offset, size, ppos, more);
}

/* 
 *  SPXPUTPMSG
 *  -------------------------------------
 */
static int spxputpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int band,
		      int flags)
{
	return pipe_f_ops.putpmsg(file, ctlp, datp, band, flags);
}

/* 
 *  SPXGETPMSG
 *  -------------------------------------
 */
static int spxgetpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int *band,
		      int *flagsp)
{
	return pipe_f_ops.getpmsg(file, ctlp, datp, band, flagsp);
}

struct file_operations spx_f_ops __cacheline_aligned = {
	owner:THIS_MODULE,
	llseek:spxllseek,
	read:spxread,
	write:spxwrite,
	poll:spxpoll,
	ioctl:spxioctl,
	mmap:spxmmap,
	open:spxopen,
	flush:spxflush,
	release:spxclose,
	fasync:spxfasync,
	readv:spxreadv,
	writev:spxwritev,
	sendpage:spxsendpage,
	getpmsg:spxgetpmsg,
	putpmsg:spxputpmsg,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static int spx_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int err;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		/* we walk down the queue chain calling open on each of the modules and the driver */
		queue_t *wq = WR(q), *wq_next;
		wq_next = SAMESTR(wq) ? wq->q_next : NULL;
		while ((wq = wq_next)) {
			/* all opens are module opens on pipes, there is no driver */
			wq_next = SAMESTR(wq) ? wq->q_next : NULL;
			if ((err = qopen(wq - 1, devp, oflag, MODOPEN, crp)))
				goto error;
		}
		MOD_DEC_USE_COUNT;	/* already open */
		return (0);
	}
	if (sflag == DRVOPEN || sflag == CLONEOPEN || WR(q)->q_next == NULL) {
		dev_t dev = *devp;
		struct stdata *sd;
		if ((sd = ((struct queinfo *) q)->qu_str)) {
			struct cdevsw *sdev = sd->sd_inode->i_cdev->data;
			/* 1st step: attach the driver and call its open routine */
			/* we are the driver and this *is* the open routine */
			/* 2nd step: check for redirected return */
			/* we are the driver and this *is* the open routine and there is no
			   redirection. */
			/* 3rd step: autopush modules and call their open routines */
			if ((err = autopush(sd, sdev, &dev, oflag, MODOPEN, crp)))
				goto error;
			/* lastly, attach our privates and return */
			q->q_ptr = WR(q)->q_ptr = sd;
			return (0);
		}
	}
	err = -EIO;		/* can't be opened as module or clone */
      error:
	MOD_DEC_USE_COUNT;
	return (err <= 0 ? err : -err);
}
static int spx_close(queue_t *q, int oflag, cred_t *crp)
{
	return (0);
}

static struct qinit spx_rinit = {
	qi_putp:spx_rput,
	qi_srvp:spx_rsrv,
	qi_qopen:spx_open,
	qi_qclose:spx_close,
	qi_minfo:&spx_minfo,
};

static struct qinit spx_winit = {
	qi_putp:spx_wput,
	qi_srvp:spx_wsrv,
	qi_minfo:&spx_minfo,
};

static struct streamtab spx_info = {
	st_rdinit:&spx_rinit,
	st_wrinit:&spx_winit,
};

static int open_spx(struct inode *inode, struct file *file)
{
	struct str_args args = {
		file:file,
		oflag:make_oflag(file),
		crp:current_creds,
		name:{args.buf, 0, 0},
	};
	args.dev = makedevice(major, 0);
	args.sflag = CLONEOPEN;
	file->f_ops = &spx_f_ops;	/* fops_get already done */
	return sdev_open(inode, file, specfs_mnt, &args);
}

static struct file_operations spx_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:open_spx,
};

static struct cdevsw spx_cdev = {
	d_name:CONFIG_STREAMS_SPX_NAME,
	d_str:&spx_info,
	d_flag:D_CLONE,
	d_fop:&spx_ops,
	d_mode:S_IFIFO,
	d_kmod:THIS_MODULE,
};

static int __init spx_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO PIPE_BANNER);
#else
	printk(KERN_INFO PIPE_SPLASH);
#endif
	if ((err = register_inode(makedevice(major, 0), &spx_cdev, &spx_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit spx_exit(void)
{
	int err;
	if ((err = unregister_inode(makedevice(major, 0), &spx_cdev)))
		return (void) (err);
	return (void) (0);
};

module_init(spx_init);
module_exit(spx_exit);
