/*****************************************************************************

 @(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/05/04 21:36:58 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2004/05/04 21:36:58 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/05/04 21:36:58 $"

static char const ident[] =
    "$RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/05/04 21:36:58 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strreg.h"		/* for struct str_args */
#include "strsched.h"		/* fort sd_get/sd_put */
#include "strhead.h"		/* for autopush */
#include "strspecfs.h"		/* for strm_open() */
#include "strpipe.h"		/* extern verification */

#include "sys/config.h"

#define PIPE_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PIPE_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define PIPE_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/05/04 21:36:58 $"
#define PIPE_DEVICE	"SVR 4.2 STREAMS-based PIPEs"
#define PIPE_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define PIPE_LICENSE	"GPL"
#define PIPE_BANNER	PIPE_DESCRIP	"\n" \
			PIPE_COPYRIGHT	"\n" \
			PIPE_REVISION	"\n" \
			PIPE_DEVICE	"\n" \
			PIPE_CONTACT	"\n"
#define PIPE_SPLASH	PIPE_DEVICE	" - " \
			PIPE_REVISION	"\n"

MODULE_AUTHOR(PIPE_CONTACT);
MODULE_DESCRIPTION(PIPE_DESCRIP);
MODULE_SUPPORTED_DEVICE(PIPE_DEVICE);
MODULE_LICENSE(PIPE_LICENSE);

#ifndef CONFIG_STREAMS_PIPE_NAME
//#define CONFIG_STREAMS_PIPE_NAME "pipe"
#error "CONFIG_STREAMS_PIPE_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_PIPE_MODID
//#define CONFIG_STREAMS_PIPE_MODID 8
#error "CONFIG_STREAMS_PIPE_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_PIPE_MAJOR
//#define CONFIG_STREAMS_PIPE_MAJOR 0
#error "CONFIG_STREAMS_PIPE_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_PIPE_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for STREAMS-based PIPEs (0 for allocation).");

static struct module_info pipe_minfo = {
	mi_idnum:CONFIG_STREAMS_PIPE_MODID,
	mi_idname:CONFIG_STREAMS_PIPE_NAME,
	mi_minpsz:STRMINPSZ,
	mi_maxpsz:STRMAXPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

#define stri_lookup(__f) ((struct stdata *)(__f)->private_data)
#define sdev_lookup(__i) ((struct cdevsw *)(__i)->i_cdev->data)

/* 
 *  PIPEOPEN
 *  -------------------------------------
 */
 /* Pipes are never associated with a filesystem entry outside the special filesystem.  Therefore,
    the passed in file pointer is always the real file pointer.  */
static struct streamtab pipe_info;
static struct qinit pipe_rinit;
static struct qinit pipe_winit;
static int pipeopen(struct inode *inode, struct file *file)
{
	struct str_args *argp = file->f_dentry->d_fsdata;
	int err = 0;
	struct stdata *sd;
	/* first find out of we already have a stream head, or we need a new one anyway */
	if (!(sd = sd_get((struct stdata *) inode->i_pipe)) || (sd->sd_cdevsw->d_flag & D_CLONE)) {
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
		sd->sd_strtab = &pipe_info;	/* we *are* the driver */
		sd->sd_wropt = SNDZERO | SNDPIPE;
		sd->sd_flag = STWOPEN;
		sd->sd_rq = q;
		sd->sd_wq = q + 1;
		((struct queinfo *) q)->qu_str = sd;
		setq(q, pipe_info.st_rdinit, pipe_info.st_wrinit);
		/* grafting onto inode */
		sd->sd_file = file;	/* real file */
		inode->i_pipe = (void *) sd;
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
		/* here we hold the STWOPEN bit and a reference to the stream head, pipe_open must
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
 *  PIPECLOSE
 *  -------------------------------------
 */
static int pipeclose(struct inode *inode, struct file *file)
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

struct file_operations pipe_f_ops ____cacheline_aligned;
EXPORT_SYMBOL_GPL(pipe_f_ops);

static int pipe_rput(queue_t *q, mblk_t *mp)
{
	return (0);
}
static int pipe_rsrv(queue_t *q)
{
	return (0);
}
static int pipe_wput(queue_t *q, mblk_t *mp)
{
	return (0);
}
static int pipe_wsrv(queue_t *q)
{
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static int pipe_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
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
			struct cdevsw *sdev = sd->sd_cdevsw;
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
static int pipe_close(queue_t *q, int oflag, cred_t *crp)
{
	return (0);
}

static struct qinit pipe_rinit = {
	qi_putp:pipe_rput,
	qi_srvp:pipe_rsrv,
	qi_qopen:pipe_open,
	qi_qclose:pipe_close,
	qi_minfo:&pipe_minfo,
};

static struct qinit pipe_winit = {
	qi_putp:pipe_wput,
	qi_srvp:pipe_wsrv,
	qi_minfo:&pipe_minfo,
};

static struct streamtab pipe_info = {
	st_rdinit:&pipe_rinit,
	st_wrinit:&pipe_winit,
};

static int open_pipe(struct inode *inode, struct file *file)
{
	struct str_args args = {
		file:file,
		dev:makedevice(major, 0),
		oflag:make_oflag(file),
		sflag:CLONEOPEN,
		crp:current_creds,
		name:{args.buf, 0, 0},
	};
	file->f_op = &pipe_f_ops;	/* fops_get already done */
	return strm_open(inode, file, &args);
}

static struct file_operations pipe_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:open_pipe,
};

static struct cdevsw pipe_cdev = {
	d_name:CONFIG_STREAMS_PIPE_NAME,
	d_str:&pipe_info,
	d_flag:D_CLONE,
	d_fop:&pipe_f_ops,
	d_mode:S_IFIFO,
	d_kmod:THIS_MODULE,
};

static int __init pipe_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO PIPE_BANNER);
#else
	printk(KERN_INFO PIPE_SPLASH);
#endif
	pipe_f_ops = strm_f_ops;
	pipe_f_ops.owner = THIS_MODULE;
	pipe_f_ops.open = pipeopen;
	pipe_f_ops.release = pipeclose;

	if ((err = register_strdrv(&pipe_cdev)) < 0)
		return (err);
	if ((err = register_cmajor(&pipe_cdev, major, &pipe_ops)) < 0) {
		unregister_strdrv(&pipe_cdev);
		return (err);
	}
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit pipe_exit(void)
{
	unregister_cmajor(&pipe_cdev, 0);
	unregister_strdrv(&pipe_cdev);
};

module_init(pipe_init);
module_exit(pipe_exit);
