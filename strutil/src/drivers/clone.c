/*****************************************************************************

 @(#) $RCSfile: clone.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/06/02 21:24:48 $

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

 Last Modified $Date: 2004/06/02 21:24:48 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: clone.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/06/02 21:24:48 $"

static char const ident[] =
    "$RCSfile: clone.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/06/02 21:24:48 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "strdebug.h"
#include "strargs.h"		/* for struct str_args */
#include "strreg.h"		/* for spec_open() */
#include "strsched.h"		/* for di_alloc()/di_put() */
#include "clone.h"		/* extern verification */

#define CLONE_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CLONE_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define CLONE_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/06/02 21:24:48 $"
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

#ifdef CONFIG_STREAMS_CLONE_MODULE
MODULE_AUTHOR(CLONE_CONTACT);
MODULE_DESCRIPTION(CLONE_DESCRIP);
MODULE_SUPPORTED_DEVICE(CLONE_DEVICE);
MODULE_LICENSE(CLONE_LICENSE);
#endif

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

modID_t modid = CONFIG_STREAMS_CLONE_MODID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module id number for CLONE driver.");

major_t major = CONFIG_STREAMS_CLONE_MAJOR;
MODULE_PARM(major, "h");
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
 *  chaining from the external filesystem (e.g. openining a character device with clone major) or by
 *  direct open of the inode within the mounted shadow special filesystem.  Either way, the inode
 *  number has our extended device numbering as a inode number and we chain the call within the
 *  shadow special filesystem.
 */
static int cloneopen(struct inode *inode, struct file *file)
{
	struct str_args *argp;
	struct cdevsw *cdev;
	int err;
	if (!(argp = file->private_data))
		return (-EIO);
	if (!(cdev = cdrv_get(getminor(argp->dev))))
		return (-ENOENT);
	argp->dev = makedevice(cdev->d_modid, 0);
	argp->sflag = CLONEOPEN;
	err = spec_open(inode, file);
	cdev_put(cdev);
	return (err);
}

struct file_operations clone_ops ____cacheline_aligned = {
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
	d_fop:&clone_ops,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

STATIC struct vfsmount *specfs = NULL;

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for clone devices.
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  clone_open: - open a clone device node
 *  @inode: the external filesystem inode
 *  @file: the external filesystem file pointer
 *
 *  clone_open() is only used to open a clone device from a character device node in an external
 *  filesystem.  This is never calle for direct opens of a specfs device node (for direct opens see
 *  spec_dev_open() in strspecfs.c).  The character device number from the inode is used to
 *  determine the shadow special filesystem (internal) inode and chain the open call.
 *
 *  This is the separation point where we convert the external device number to an internal device
 *  number.  The external device number is contained in inode->i_rdev.
 */
STATIC int clone_open(struct inode *inode, struct file *file)
{
	int err;
	struct cdevsw *cdev;
	dev_t dev;
	struct str_args args;
	void *old_data = file->private_data;
	major_t major;
	minor_t minor;
	modID_t modid, instance;
	minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	major = MAJOR(kdev_t_to_nr(inode->i_rdev));
	minor = cdev_minor(&clone_cdev, major, minor);
	major = clone_cdev.d_major;
	modid = clone_cdev.d_modid;
	err = -ENXIO;
	if (!(cdev = cdev_get(minor)))
		goto exit;
	instance = cdev->d_modid;
	cdev_put(cdev);
	dev = makedevice(modid, instance);
	args.mnt = specfs;
	args.inode = inode;
	args.file = file;
	args.dev = dev;
	args.oflag = make_oflag(file);
	args.sflag = CLONEOPEN;
	args.crp = current_creds;
	args.buf[0] = '\0';
	args.name.name = args.buf;
	args.name.len = 0;
	args.name.hash = dev;
	file->private_data = &args;
	err = spec_open(inode, file);
	file->private_data = old_data;
      exit:
	return (err);
}

STATIC struct file_operations clone_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:clone_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  REGISTRATION
 *
 *  -------------------------------------------------------------------------
 */

int register_clone(struct cdevsw *cdev)
{
	int err;
	struct devnode *node;
	if (!(node = kmalloc(sizeof(*node), GFP_ATOMIC)))
		return (-ENOMEM);
	memset(node, 0, sizeof(*node));
	node->n_name = cdev->d_name;
	node->n_flag = clone_cdev.d_flag;
	node->n_mode = clone_cdev.d_mode;
	if ((err = register_strnod(&clone_cdev, node, cdev->d_modid)) < 0) {
		kfree(node);
		return (err);
	}
	return (err);
}

EXPORT_SYMBOL_GPL(register_clone);

int unregister_clone(struct cdevsw *cdev)
{
	int err;
	struct devnode *node;
	if (!(node = node_get(&clone_cdev, cdev->d_modid)))
		return (-ENXIO);
	if ((err = unregister_strnod(&clone_cdev, cdev->d_modid)))
		return (err);
	kfree(node);
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_clone);

/* bleedin' mercy! */
static inline void put_filesystem(struct file_system_type *fs)
{
	if (fs->owner)
		__MOD_DEC_USE_COUNT(fs->owner);
}

static int __init clone_init(void)
{
	int err;
	struct devinfo *devi;
	struct file_system_type *fstype;
#ifdef CONFIG_STREAMS_CLONE_MODULE
	printk(KERN_INFO CLONE_BANNER);
#else
	printk(KERN_INFO CLONE_SPLASH);
#endif
	err = -ENODEV;
	if (!(fstype = get_fs_type("specfs")))
		goto exit;
	err = PTR_ERR(specfs = kern_mount(fstype));
	put_filesystem(fstype);
	if (IS_ERR(specfs))
		goto exit;
	clone_minfo.mi_idnum = modid;
	if ((err = register_strdrv(&clone_cdev, specfs) < 0))
		goto no_strdrv;
	err = -ENOMEM;
	if (!(devi = di_alloc(&clone_cdev)))
		goto no_devi;
	if ((err = register_cmajor(&clone_cdev, devi, major, &clone_f_ops)) < 0)
		goto no_cmajor;
	if (major == 0 && err > 0)
		major = err;
	return (0);
      no_cmajor:
	di_put(devi);
      no_devi:
	unregister_strdrv(&clone_cdev, specfs);
      no_strdrv:
	kern_umount(specfs);
      exit:
	return (err);
};

static void __exit clone_exit(void)
{
	struct devinfo *devi;
	if ((devi = devi_get(&clone_cdev, major))) {
		if (unregister_cmajor(&clone_cdev, devi, major) < 0)
			swerr();
		di_put(devi);
		if (unregister_strdrv(&clone_cdev, specfs) < 0)
			swerr();
	} else
		swerr();
	kern_umount(specfs);
	return;
};

#ifdef CONFIG_STREAMS_CLONE_MODULE
module_init(clone_init);
module_exit(clone_exit);
#endif
