/*****************************************************************************

 @(#) $RCSfile: strnsdev.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:33 $

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

 Last Modified $Date: 2004/04/22 12:08:33 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strnsdev.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:33 $"

static char const ident[] = "$RCSfile: strnsdev.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:33 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/modversions.h>
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strspecfs.h"
#include "strreg.h"
#include "strhead.h"

#include "sys/config.h"

#define NSDEV_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NSDEV_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define NSDEV_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/04/22 12:08:33 $"
#define NSDEV_DEVICE	"SVR 4.2 STREAMS Named Stream Device (NSDEV) Driver"
#define NSDEV_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NSDEV_LICENSE	"GPL"
#define NSDEV_BANNER	NSDEV_DESCRIP	"\n" \
			NSDEV_COPYRIGHT	"\n" \
			NSDEV_REVISION	"\n" \
			NSDEV_DEVICE	"\n" \
			NSDEV_CONTACT	"\n"
#define NSDEV_SPLASH	NSDEV_DEVICE	" - " \
			NSDEV_REVISION	"\n"

MODULE_AUTHOR(NSDEV_CONTACT);
MODULE_DESCRIPTION(NSDEV_DESCRIP);
MODULE_SUPPORTED_DEVICE(NSDEV_DEVICE);
MODULE_LICENSE(NSDEV_LICENSE);

#ifndef CONFIG_STREAMS_NSDEV_NAME
//#define CONFIG_STREAMS_NSDEV_NAME "nsdev"
#error "CONFIG_STREAMS_NSDEV_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_NSDEV_MODID
//#define CONFIG_STREAMS_NSDEV_MODID 7
#error "CONFIG_STREAMS_NSDEV_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_NSDEV_MAJOR
//#define CONFIG_STREAMS_NSDEV_MAJOR 0
#error "CONFIG_STREAMS_NSDEV_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_NSDEV_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for NSDEV driver.");

static struct module_info nsdev_minfo = {
	mi_idnum:CONFIG_STREAMS_NSDEV_MODID,
	mi_idname:CONFIG_STREAMS_NSDEV_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static struct qinit nsdev_rinit = {
	qi_putp:putq,
	qi_minfo:&nsdev_minfo,
};

static struct qinit nsdev_winit = {
	qi_putp:putq,
	qi_minfo:&nsdev_minfo,
};

static struct streamtab nsdev_info = {
	st_rdinit:&nsdev_rinit,
	st_wrinit:&nsdev_winit,
};

/* 
 *  NSDEVOPEN
 *  -------------------------------------------------------------------------
 *  This is rather simple.  We are going to do a redirected open on the a new
 *  device with the major device number mapped according to name.  We do this
 *  by nesting another strm_open() inside the first one with an adjusted
 *  device number. It helps that the orginal file and dentry pointer is stored
 *  with the args passed as fsdata attached to the dentry.  We use this to
 *  find the original file pointer and dentry and get the name of the opened
 *  file.
 *
 *  If we don't find a reasonable match and kmod is equipped, we try to load
 *  the module with 'streams-' prefixed to the name and run through the list
 *  again.
 */
static int nsdevopen(struct inode *inode, struct file *file)
{
	struct str_args *argp = file->f_dentry->d_fsdata;
	struct dentry *dentry = argp->file->f_dentry;	/* original file dentry */
	struct qstr *name = &dentry->d_name;
	int retry = 0;
	do {
		int i, j, maj_max = 0, len_max = 0;
		const struct cdevsw *cdev, *cdev_max = NULL;
		read_lock(&cdevsw_lock);
		/* do a name search looking for the device */
		for (i = 0; i < MAX_CHRDEV; i++)
			if ((cdev = cdevsw[i])) {
				for (j = 0; j < name->len && j < FMNAMESZ + 1; j++)
					if (cdev->d_name[j] != name->name[j])
						break;
				if (j > len_max) {
					maj_max = i;
					len_max = j;
					cdev_max = cdev;
				}
			}
		read_unlock(&cdevsw_lock);
		/* aww come on ... at least 3 char match */
		if (cdev_max && (len_max == name->len || len_max > 2)) {
			argp->dev = makedevice(maj_max, getminor(argp->dev));
			argp->sflag = DRVOPEN;	/* this is a directed open */
			return strm_open(inode, file);
		}
#ifdef CONFIG_KMOD
		{
			char buf[FMNAMESZ + 10];
			snprintf(buf, FMNAMESZ + 10, "streams-%s", name->name);
			request_module(buf);
			continue;
		}
#endif
		break;
	} while (!retry++);
	return (-ENOENT);	/* sounds good */
}

struct file_operations nsdev_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:nsdevopen,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

static int open_nsdev(struct inode *inode, struct file *file)
{
	struct str_args args = {
		file:file,
		oflag:make_oflag(file),
		crp:current_creds,
		name:{args.buf, 0, 0},
	};
	args.dev = makedevice(major, 0);
	args.sflag = CLONEOPEN;
	file->f_op = &nsdev_f_ops;
	return sdev_open(inode, file, specfs_mnt, &args);
}

static struct file_operations nsdev_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:open_nsdev,
};

static struct cdevsw nsdev_cdev = {
	d_name:CONFIG_STREAMS_NSDEV_NAME,
	d_str:&nsdev_info,
	d_fop:&nsdev_ops,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

static int __init nsdev_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO NSDEV_BANNER);
#else
	printk(KERN_INFO NSDEV_SPLASH);
#endif
	if ((err = register_inode(makedevice(major, 0), &nsdev_cdev, &nsdev_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit nsdev_exit(void)
{
	int err;
	if ((err = unregister_inode(makedevice(major, 0), &nsdev_cdev)))
		return (void) (err);
	return (void) (0);
};

module_init(nsdev_init);
module_exit(nsdev_exit);
