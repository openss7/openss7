/*****************************************************************************

 @(#) $RCSfile: fifo.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:33 $

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

 Last Modified $Date: 2004/06/01 12:04:33 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: fifo.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:33 $"

static char const ident[] =
    "$RCSfile: fifo.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:33 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#include <asm/semaphore.h>

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "strdebug.h"
#include "strsad.h"		/* for autopush */
#include "sth.h"
#include "fifo.h"		/* extern verification */

#define FIFO_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define FIFO_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define FIFO_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:33 $"
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

#ifdef CONFIG_STREAMS_FIFO_MODULE
MODULE_AUTHOR(FIFO_CONTACT);
MODULE_DESCRIPTION(FIFO_DESCRIP);
MODULE_SUPPORTED_DEVICE(FIFO_DEVICE);
MODULE_LICENSE(FIFO_LICENSE);
#endif

#ifndef CONFIG_STREAMS_FIFO_NAME
//#define CONFIG_STREAMS_FIFO_NAME "fifo"
#error "CONFIG_STREAMS_FIFO_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_FIFO_MODID
//#define CONFIG_STREAMS_FIFO_MODID 6
#error "CONFIG_STREAMS_FIFO_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_FIFO_MAJOR
//#define CONFIG_STREAMS_FIFO_MAJOR 0
#error "CONFIG_STREAMS_FIFO_MAJOR must be defined."
#endif

modID_t modid = CONFIG_STREAMS_FIFO_MODID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module identification number for STREAMS-based FIFOs");

major_t major = CONFIG_STREAMS_FIFO_MAJOR;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Major device number for STREAMS-based FIFOs.");

static struct module_info fifo_minfo = {
	mi_idnum:CONFIG_STREAMS_FIFO_MODID,
	mi_idname:CONFIG_STREAMS_FIFO_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static int fifo_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp);
static int fifo_close(queue_t *q, int oflag, cred_t *crp);

static struct qinit fifo_rinit = {
	qi_putp:strrput,
	qi_qopen:fifo_open,
	qi_qclose:fifo_close,
	qi_minfo:&fifo_minfo,
};

static struct qinit fifo_winit = {
	qi_srvp:strwsrv,
	qi_minfo:&fifo_minfo,
};

static struct streamtab fifo_info = {
	st_rdinit:&fifo_rinit,
	st_wrinit:&fifo_winit,
};

#define stri_lookup(__f) ((struct stdata *)(__f)->private_data)

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  fifo_open:	- STREAMS qopen procedure for fifo stream heads
 *  @q:		read queue of stream to open
 *  @devp:	pointer to a dev_t from which to read and into which to return the device number
 *  @oflag:	open flags
 *  @sflag:	STREAMS flags (%DRVOPEN or %MODOPEN or %CLONEOPEN)
 *  @crp:	pointer to user's credentials structure
 */
static int fifo_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct stdata *sd;
	int err = 0;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if ((sd = q->q_ptr) != NULL) {
		/* we walk down the queue chain calling open on each of the modules and the driver */
		queue_t *wq = WR(q), *wq_next;
		wq_next = SAMESTR(wq) ? wq->q_next : NULL;
		while ((wq = wq_next)) {
			int new_sflag;
			wq_next = SAMESTR(wq) ? wq->q_next : NULL;
			new_sflag = wq_next ? MODOPEN : sflag;
			if ((err = qopen(wq - 1, devp, oflag, MODOPEN, crp))) {
				MOD_DEC_USE_COUNT;
				return (err > 0 ? -err : err);
			}
		}
		goto done;
	}
	if (sflag == DRVOPEN || sflag == CLONEOPEN || WR(q)->q_next == NULL) {
		dev_t dev = *devp;
		if ((sd = ((struct queinfo *) q)->qu_str)) {
			/* 1st step: attach the driver and call its open routine */
			/* we are the driver and this *is* the open routine */
			/* start off life as a fifo */
			WR(q)->q_next = q;
			/* 2nd step: check for redirected return */
			/* we are the driver and this *is* the open routine and there is no
			   redirection. */
			/* 3rd step: autopush modules and call their open routines */
			if ((err = autopush(sd, sd->sd_cdevsw, &dev, oflag, MODOPEN, crp))) {
				MOD_DEC_USE_COUNT;
				return (err > 0 ? -err : err);
			}
			goto done;
		}
	}
	MOD_DEC_USE_COUNT;
	return (-EIO);		/* can't be opened as module or clone */
      done:
	/* TODO: should proabably move this up to stropen */
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
				up(&sd->sd_file->f_dentry->d_inode->i_sem);
				/* FIXME: probably need to release open bit as well... */
				schedule();
				down(&sd->sd_file->f_dentry->d_inode->i_sem);
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
				up(&sd->sd_file->f_dentry->d_inode->i_sem);
				/* FIXME: probably need to release open bit as well... */
				schedule();
				down(&sd->sd_file->f_dentry->d_inode->i_sem);
			}
		}
		break;
	case (FREAD | FNDELAY):
		break;
	case (FREAD | FWRITE):
	case (FREAD | FWRITE | FNDELAY):
		break;
	}
	return (err);
	if (q->q_ptr) {
		MOD_DEC_USE_COUNT;	/* already open */
	} else {
		/* lastly, attach our privates and return */
		q->q_ptr = WR(q)->q_ptr = sd;
	}
	return (0);
}
static int fifo_close(queue_t *q, int oflag, cred_t *crp)
{
	if (!q->q_ptr || q->q_ptr != ((struct queinfo *) q)->qu_str)
		return (ENXIO);
	q->q_ptr = WR(q)->q_ptr = NULL;
	MOD_DEC_USE_COUNT;
	return (0);
}

static struct cdevsw fifo_cdev = {
	d_name:CONFIG_STREAMS_FIFO_NAME,
	d_str:&fifo_info,
	d_flag:0,
	d_fop:&strm_f_ops,
	d_mode:S_IFIFO,
	d_kmod:THIS_MODULE,
};

static int __init fifo_init(void)
{
	int err;
#ifdef CONFIG_STREAMS_FIFO_MODULE
	printk(KERN_INFO FIFO_BANNER);
#else
	printk(KERN_INFO FIFO_SPLASH);
#endif
	fifo_minfo.mi_idnum = modid;
	if ((err = register_strdev(&fifo_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit fifo_exit(void)
{
	unregister_strdev(&fifo_cdev, major);
};

#ifdef CONFIG_STREAMS_FIFO_MODULE
module_init(fifo_init);
module_exit(fifo_exit);
#endif
