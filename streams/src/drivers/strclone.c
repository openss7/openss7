/*****************************************************************************

 @(#) $RCSfile: strclone.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/05/03 06:30:20 $

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

 Last Modified $Date: 2004/05/03 06:30:20 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strclone.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/05/03 06:30:20 $"

static char const ident[] = "$RCSfile: strclone.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/05/03 06:30:20 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/modversions.h>
#include <linux/module.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strspecfs.h"		/* for spec_open() and str_args */
#include "strhead.h"		/* for make_oflags */

#include "sys/config.h"

#define CLONE_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CLONE_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define CLONE_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/05/03 06:30:20 $"
#define CLONE_DEVICE	"SVR 4.2 STREAMS CLONE Driver"
#define CLONE_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CLONE_LICENSE	"GPL"
#define CLONE_BANNER	CLONE_DESCRIP	"\n" \
			CLONE_COPYRIGHT	"\n" \
			CLONE_REVISION	"\n" \
			CLONE_DEVICE	"\n" \
			CLONE_CONTACT	"\n"
#define CLONE_SPLASH	CLONE_DEVICE	" - " \
			CLONE_REVISION	"\n"

MODULE_AUTHOR(CLONE_CONTACT);
MODULE_DESCRIPTION(CLONE_DESCRIP);
MODULE_SUPPORTED_DEVICE(CLONE_DEVICE);
MODULE_LICENSE(CLONE_LICENSE);

#ifndef CONFIG_STREAMS_CLONE_NAME
//#define CONFIG_STREAMS_CLONE_NAME "clone"
#error "CONFIG_STREAMS_CLONE_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_CLONE_MODID
//#define CONFIG_STREAMS_CLONE_MODID 5
#error "CONFIG_STREAMS_CLONE_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_CLONE_MAJOR
//#define CONFIG_STREAMS_CLONE_MAJOR 54
#error "CONFIG_STREAMS_CLONE_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_CLONE_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for CLONE driver.");

static struct module_info clone_minfo = {
	mi_idnum:CONFIG_STREAMS_CLONE_MODID,
	mi_idname:CONFIG_STREAMS_CLONE_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static struct qinit clone_rinit = {
	qi_putp:putq,
	qi_minfo:&clone_minfo,
};

static struct qinit clone_winit = {
	qi_putp:putq,
	qi_minfo:&clone_minfo,
};

static struct streamtab clone_info = {
	st_rdinit:&clone_rinit,
	st_wrinit:&clone_winit,
};

/**
 *  cloneopen: - open a clone special device
 *  @inode:	shadow special filesystem inode
 *  @file:	shadow special filesystem file pointer
 *
 *  cloneopen() is called only from within the shadow special filesystem.  This can occur by
 *  chaining from the external filesystem (e.g. openining a character device with clone major as
 *  both major and minor) or by direct open of the inode within the mounted shadow special
 *  filesystem.  Either way, the inode number has our extended device numbering as a inode number
 *  and we chain the call within the shadow special filesystem.
 */
static int cloneopen(struct inode *inode, struct file *file)
{
	struct cdevsw *cdev;
	// major_t major = (inode->i_ino >> sizeof(minor_t) * 8);
	minor_t minor = (inode->i_ino & ((1 << sizeof(minor_t) * 8) - 1));
	struct str_args args = {
		file:file,
		dev:inode->i_ino,
		oflag:make_oflag(file),
		sflag:CLONEOPEN,
		crp:current_creds,
		name:{args.buf, 0, 0},
	};
	int err;
	void *fsdata;
	err = -ENXIO;
	if (!(cdev = (struct cdevsw *)fmod_get(minor))) /* FIXME */
		goto error;
	err = -EIO;
	if (!cdev->d_fop || !cdev->d_fop->open)
		goto put_error;
	fsdata = xchg(&file->f_dentry->d_fsdata, &args);
	err = (*cdev->d_fop->open) (inode, file);
	file->f_dentry->d_fsdata = fsdata;
      put_error:
	cdev_put(cdev);
      error:
	return (err);
}

struct file_operations clone_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:cloneopen,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

static struct cdevsw clone_cdev = {
	d_name:"clone",
	d_str:&clone_info,
	d_flag:D_CLONE,
	d_fop:&clone_f_ops,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

/**
 *  clone_open: - open a clone character device
 *  @inode;	the external filesystem inode
 *  @file:	the user file pointer
 *
 *  clone_open() is called directly from chrdev_open().  This is an external filesystem entry point
 *  so the @inode and @file correspond to an inode in the extenal filesystem and the user file
 *  pointer.  For external opens, there is no need to enter the shadow special filesystem on the
 *  clone device, we redirect the call from here instead to the new character device's internal file
 *  operations.
 */
static int clone_open(struct inode *inode, struct file *file)
{
	struct cdevsw *cdev;
	/// major_t major = MAJOR(kdev_t_to_nr(inode->i_rdev));
	minor_t minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	struct str_args args = {
		file:file,
		dev:makedevice(minor, 0),	/* swapped minor and major */
		oflag:make_oflag(file),
		sflag:CLONEOPEN,	/* this is a clone open */
		crp:current_creds,
		name:{args.buf, 0, 0},
	};
	int err;
	void *fsdata;
	err = -ENXIO;
	if (!(cdev = cdev_get(minor)))
		goto error;
	err = -EIO;
	if (!cdev->d_fop || !cdev->d_fop->open)
		goto put_error;
	fsdata = xchg(&file->f_dentry->d_fsdata, &args);
	err = (*cdev->d_fop->open) (inode, file);
	file->f_dentry->d_fsdata = fsdata;
      put_error:
	cdev_put(cdev);
      error:
	return (err);
}

struct file_operations clone_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:clone_open,
};

static int __init clone_init(void)
{
	int err;
#ifdef MODULE
	printk(KERN_INFO CLONE_BANNER);
#else
	printk(KERN_INFO CLONE_SPLASH);
#endif
	if ((err = register_strdrv(&clone_cdev)) < 0)
		return (err);
	if ((err = register_cmajor(&clone_cdev, major, &clone_ops)) < 0) {
		unregister_strdrv(&clone_cdev);
		return (err);
	}
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit clone_exit(void)
{
	unregister_cmajor(&clone_cdev, 0);
	unregister_strdrv(&clone_cdev);
};

module_init(clone_init);
module_exit(clone_exit);
