/*****************************************************************************

 @(#) $RCSfile: fifo.c,v $ $Name:  $($Revision: 0.9.2.46 $) $Date: 2007/04/12 20:06:08 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2007/04/12 20:06:08 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: fifo.c,v $ $Name:  $($Revision: 0.9.2.46 $) $Date: 2007/04/12 20:06:08 $"

static char const ident[] =
    "$RCSfile: fifo.c,v $ $Name:  $($Revision: 0.9.2.46 $) $Date: 2007/04/12 20:06:08 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <asm/semaphore.h>

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#ifndef HAVE_KFUNC_MODULE_PUT
#define module_refcount(__m) atomic_read(&(__m)->uc.usecount)
#endif

#include "sys/config.h"
#include "src/kernel/strsched.h"	/* for allocstr */
#include "src/kernel/strsad.h"	/* for autopush */
#include "src/modules/sth.h"
#include "fifo.h"		/* extern verification */

#define FIFO_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define FIFO_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define FIFO_REVISION	"LfS $RCSfile: fifo.c,v $ $Name:  $($Revision: 0.9.2.46 $) $Date: 2007/04/12 20:06:08 $"
#define FIFO_DEVICE	"SVR 4.2 STREAMS-based FIFOs"
#define FIFO_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define FIFO_LICENSE	"GPL"
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
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-fifo");
#endif
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

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module identification number for STREAMS-based FIFOs");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_FIFO_MODID));
MODULE_ALIAS("streams-driver-fifo");
#endif

major_t major = CONFIG_STREAMS_FIFO_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for STREAMS-based FIFOs.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_FIFO_MAJOR) "-*");
MODULE_ALIAS("/dev/fifo");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_FIFO_MAJOR));
MODULE_ALIAS("/dev/streams/fifo");
MODULE_ALIAS("/dev/streams/fifo/*");
#endif
#endif

static struct module_info fifo_minfo = {
	.mi_idnum = CONFIG_STREAMS_FIFO_MODID,
	.mi_idname = CONFIG_STREAMS_FIFO_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat fifo_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat fifo_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit fifo_rinit = {
	.qi_putp = strrput,
	.qi_qopen = str_open,
	.qi_qclose = str_close,
	.qi_minfo = &fifo_minfo,
	.qi_mstat = &fifo_rstat,
};

static struct qinit fifo_winit = {
	.qi_putp = strwput,
	.qi_minfo = &fifo_minfo,
	.qi_mstat = &fifo_wstat,
};

static struct streamtab fifo_info = {
	.st_rdinit = &fifo_rinit,
	.st_wrinit = &fifo_winit,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

static struct cdevsw fifo_cdev = {
	.d_name = CONFIG_STREAMS_FIFO_NAME,
	.d_str = &fifo_info,
	.d_flag = D_MP,
	.d_fop = &strm_f_ops,
	.d_mode = S_IFIFO | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for fifo devices.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  fifo_open: - open a fifo device node
 *  @inode: the external filesystem inode
 *  @file: the external filesystem file pointer
 *
 *  fifo_open() is only used to open a fifo device (named pipe) from a character device node in an
 *  external filesystem.  This is never called for direct opens of a specfs device node (for direct
 *  opens, see spec_dev_open() in strspecfs.c).  The character device inode is opened directly and
 *  no inode in the shadow filesystem is addressed.
 */
STATIC int
fifo_open(struct inode *inode, struct file *file)
{
	int err;
	dev_t dev = makedevice(fifo_cdev.d_modid, 0);

	{
		struct file_operations *f_op;

		err = -ENXIO;
		if (!(f_op = fops_get(fifo_cdev.d_fop))) {
			goto error;
		}
		fops_put(file->f_op);
		file->f_op = f_op;
	}
	file->private_data = &dev;	/* use this device number instead of inode number */
	file->f_flags &= ~O_CLONE;	/* FIFOs never clone */
	err = file->f_op->open(inode, file);
      error:
	return (err);
}

STATIC struct file_operations fifo_f_ops ____cacheline_aligned = {
	.owner = NULL,			/* yes NULL */
	.open = &fifo_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  REGISTRATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_FIFO_OVERRIDE
static const struct file_operations *_def_fifo_ops =
    ((typeof(_def_fifo_ops)) HAVE_DEF_FIFO_OPS_ADDR);

static struct file_operations fifo_tmp_ops;

STATIC void
register_fifo(void)
{
	fifo_tmp_ops = *_def_fifo_ops;
	_def_fifo_ops->owner = fifo_f_ops.owner;
	_def_fifo_ops->open = fifo_f_ops.open;
}
STATIC void
unregister_fifo(void)
{
	_def_fifo_ops->open = fifo_tmp_ops.open;
	_def_fifo_ops->owner = fifo_tmp_ops.owner;
}
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZAION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_FIFO_MODULE
static
#endif
int __init
fifo_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_FIFO_MODULE
	printk(KERN_INFO FIFO_BANNER);
#else
	printk(KERN_INFO FIFO_SPLASH);
#endif
	fifo_minfo.mi_idnum = modid;
	if ((err = register_cmajor(&fifo_cdev, major, &fifo_f_ops)) < 0)
		return (err);
#ifdef CONFIG_STREAMS_FIFO_OVERRIDE
	register_fifo();	/* This is safe */
#endif
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_FIFO_MODULE
static
#endif
void __exit
fifo_exit(void)
{
#ifdef CONFIG_STREAMS_FIFO_OVERRIDE
	unregister_fifo();	/* This is not safe... */
#endif
	unregister_cmajor(&fifo_cdev, major);
};

#ifdef CONFIG_STREAMS_FIFO_MODULE
module_init(fifo_init);
module_exit(fifo_exit);
#endif
