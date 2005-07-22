/*****************************************************************************

 @(#) $RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2005/07/21 20:47:21 $

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

 Last Modified $Date: 2005/07/21 20:47:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2005/07/21 20:47:21 $"

static char const ident[] =
    "$RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2005/07/21 20:47:21 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/kernel/strreg.h"
#include "src/kernel/strsched.h"
#include "src/kernel/strsad.h"	/* for autopush */
#include "src/modules/sth.h"
#include "pipe.h"		/* extern verification */

#define PIPE_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PIPE_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define PIPE_REVISION	"LfS $RCSfile: pipe.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2005/07/21 20:47:21 $"
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

#ifdef CONFIG_STREAMS_PIPE_MODULE
MODULE_AUTHOR(PIPE_CONTACT);
MODULE_DESCRIPTION(PIPE_DESCRIP);
MODULE_SUPPORTED_DEVICE(PIPE_DEVICE);
MODULE_LICENSE(PIPE_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-pipe");
#endif
#endif

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

modID_t modid = CONFIG_STREAMS_PIPE_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for STREAMS-based PIPEs (0 for allocation).");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PIPE_MODID));
MODULE_ALIAS("streams-driver-pipe");
#endif

major_t major = CONFIG_STREAMS_PIPE_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for STREAMS-based PIPEs (0 for allocation).");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_PIPE_MAJOR) "-*");
MODULE_ALIAS("/dev/pipe");
#if LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_PIPE_MAJOR));
MODULE_ALIAS("/dev/streams/pipe");
MODULE_ALIAS("/dev/streams/pipe/*");
#endif
#endif

static struct module_info pipe_minfo = {
	mi_idnum:CONFIG_STREAMS_PIPE_MODID,
	mi_idname:CONFIG_STREAMS_PIPE_NAME,
	mi_minpsz:STRMINPSZ,
	mi_maxpsz:STRMAXPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static int pipe_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp);
static int pipe_close(queue_t *q, int oflag, cred_t *crp);

static struct qinit pipe_rinit = {
	qi_putp:strrput,
	qi_qopen:pipe_open,
	qi_qclose:pipe_close,
	qi_minfo:&pipe_minfo,
};

static struct qinit pipe_winit = {
	qi_srvp:strwsrv,
	qi_minfo:&pipe_minfo,
};

static struct streamtab pipe_info = {
	st_rdinit:&pipe_rinit,
	st_wrinit:&pipe_winit,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  pipe_open:	- STREAMS qopen procedure for pipe stream heads
 *  @q:		read queue of stream to open
 *  @devp:	pointer to a dev_t from which to read and into which to return the device number
 *  @oflag:	open flags
 *  @sflag:	STREAMS flags (%DRVOPEN or %MODOPEN or %CLONEOPEN)
 *  @crp:	pointer to user's credentials structure
 */
static int
pipe_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int err;

	if (q->q_ptr != NULL) {
		/* we walk down the queue chain calling open on each of the modules and the driver */
		queue_t *wq = WR(q), *wq_next;

		wq_next = SAMESTR(wq) ? wq->q_next : NULL;
		while ((wq = wq_next)) {
			int new_sflag;

			wq_next = SAMESTR(wq) ? wq->q_next : NULL;
			new_sflag = wq_next ? MODOPEN : sflag;
			if ((err = qopen(wq - 1, devp, oflag, MODOPEN, crp)))
				return (err > 0 ? -err : err);
		}
		return (0);
	}
	if (sflag == DRVOPEN || sflag == CLONEOPEN || WR(q)->q_next == NULL) {
		dev_t dev = *devp;
		struct stdata *sd;

		if ((sd = ((struct queinfo *) q)->qu_str)) {
			/* 1st step: attach the driver and call its open routine */
			/* we are the driver and this *is* the open routine */
			/* FIXME: create another stream head and attach it to the first */
			/* FIXME: place a M_PASSFP message on the first stream head */
			/* start off life as a fifo */
			WR(q)->q_next = q;
			/* 2nd step: check for redirected return */
			/* we are the driver and this *is* the open routine and there is no
			   redirection. */
			/* 3rd step: autopush modules and call their open routines */
			if ((err = autopush(sd, sd->sd_cdevsw, &dev, oflag, MODOPEN, crp)))
				return (err > 0 ? -err : err);
			/* lastly, attach our privates and return */
			q->q_ptr = WR(q)->q_ptr = sd;
			return (0);
		}
	}
	return (-EIO);		/* can't be opened as module or clone */
}
static int
pipe_close(queue_t *q, int oflag, cred_t *crp)
{
	if (!q->q_ptr || q->q_ptr != ((struct queinfo *) q)->qu_str)
		return (ENXIO);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

static struct cdevsw pipe_cdev = {
	d_name:CONFIG_STREAMS_PIPE_NAME,
	d_str:&pipe_info,
	d_flag:D_CLONE,
	d_fop:&strm_f_ops,
	d_mode:S_IFIFO | S_IRUGO | S_IWUGO,
	d_kmod:THIS_MODULE,
};

#ifdef CONFIG_STREAMS_PIPE_MODULE
static
#endif
int __init
pipe_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_PIPE_MODULE
	printk(KERN_INFO PIPE_BANNER);
#else
	printk(KERN_INFO PIPE_SPLASH);
#endif
	pipe_minfo.mi_idnum = modid;
	if ((err = register_strdev(&pipe_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_PIPE_MODULE
static
#endif
void __exit
pipe_exit(void)
{
	unregister_strdev(&pipe_cdev, major);
};

#ifdef CONFIG_STREAMS_PIPE_MODULE
module_init(pipe_init);
module_exit(pipe_exit);
#endif
