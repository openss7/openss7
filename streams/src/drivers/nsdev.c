/*****************************************************************************

 @(#) $RCSfile: nsdev.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:34 $

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

 Last Modified $Date: 2004/06/01 12:04:34 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: nsdev.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:34 $"

static char const ident[] =
    "$RCSfile: nsdev.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:34 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "strdebug.h"
#include "strreg.h"	    /* for strm_open() */
#include "sth.h"

#define NSDEV_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NSDEV_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define NSDEV_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/06/01 12:04:34 $"
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

#ifdef CONFIG_STREAMS_NSDEV_MODULE
MODULE_AUTHOR(NSDEV_CONTACT);
MODULE_DESCRIPTION(NSDEV_DESCRIP);
MODULE_SUPPORTED_DEVICE(NSDEV_DEVICE);
MODULE_LICENSE(NSDEV_LICENSE);
#endif

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

modID_t modid = CONFIG_STREAMS_NSDEV_MODID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module id number for NSDEV driver.");

major_t major = CONFIG_STREAMS_NSDEV_MAJOR;
MODULE_PARM(major, "h");
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

/**
 *  nsdevopen:	- open the named streams device
 *  @inode:	shadow special filesystem inode to open
 *  @file:	shadow special filesystem file pointer to open
 *
 *  This is rather simple.  We are going to do a redirected open on the a new device with the major
 *  device number mapped according to name.  We do this by nesting another strm_open() inside the
 *  first one with an adjusted device number. It helps that the orginal file pointer is stored with
 *  the args passed as private_data attached to the current file pointer.  We use this to find the
 *  original file pointer and dentry and get the name of the opened file.
 *
 *  If we don't find a reasonable match and kmod is equipped, we try to load the module with
 *  'streams-' prefixed to the name and run through the list again.  We could also generate the
 *  request from the path to the original dentry.
 */
static int nsdevopen(struct inode *inode, struct file *file)
{
	struct str_args *argp;
	struct cdevsw *cdev;
	if (!(argp = file->private_data))
		return (-EIO);
	if (!(cdev = cdev_match(argp->file->f_dentry->d_name.name)))
		return (-ENOENT);
	// argp->mnt = argp->mnt;
	// argp->inode = argp->inode;
	// argp->file = argp->file;
	argp->dev = makedevice(cdev->d_modid, getminor(argp->dev));
	argp->name.name = file->f_dentry->d_name.name;
	argp->name.len = file->f_dentry->d_name.len;
	argp->name.hash = file->f_dentry->d_name.hash;
	// argp->oflag = argp->oflag;
	// argp->sflag = argp->sflag;
	// argp->crp = argp->crp;
	cdev_put(cdev);
	return strm_open(inode, file, argp);
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

static struct cdevsw nsdev_cdev = {
	d_name:CONFIG_STREAMS_NSDEV_NAME,
	d_str:&nsdev_info,
	d_flag:0,
	d_fop:&nsdev_f_ops,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

static int __init nsdev_init(void)
{
	int err;
#ifdef CONFIG_STREAMS_NSDEV_MODULE
	printk(KERN_INFO NSDEV_BANNER);
#else
	printk(KERN_INFO NSDEV_SPLASH);
#endif
	nsdev_minfo.mi_idnum = modid;
	if ((err = register_strdev(&nsdev_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit nsdev_exit(void)
{
	unregister_strdev(&nsdev_cdev, major);
};

#ifdef CONFIG_STREAMS_NSDEV_MODULE
module_init(nsdev_init);
module_exit(nsdev_exit);
#endif
