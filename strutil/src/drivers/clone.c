/*****************************************************************************

 @(#) $RCSfile: clone.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2005/12/28 10:01:21 $

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

 Last Modified $Date: 2005/12/28 10:01:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: clone.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2005/12/28 10:01:21 $"

static char const ident[] =
    "$RCSfile: clone.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2005/12/28 10:01:21 $";

#define _LFS_SOURCE

#include <sys/os7/compat.h>

#include "clone.h"		/* extern verification */

#ifdef LIS
#define CONFIG_STREAMS_CLONE_MODID	CLONE_DRV_ID
#define CONFIG_STREAMS_CLONE_NAME	CLONE_DRV_NAME
#define CONFIG_STREAMS_CLONE_MAJOR	CLONE_CMAJOR_0
#define LFSSTATIC
#endif
#ifdef LFS
#define LFSSTATIC static
#endif

#define CLONE_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CLONE_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define CLONE_REVISION	"LfS $RCSfile: clone.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2005/12/28 10:01:21 $"
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
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-clone");
#endif
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

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for CLONE driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CLONE_MODID));
MODULE_ALIAS("streams-driver-clone");
#endif

major_t major = CONFIG_STREAMS_CLONE_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for CLONE driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR));
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-*");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-0");
MODULE_ALIAS("/dev/clone");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR));
MODULE_ALIAS("/dev/streams/clone");
MODULE_ALIAS("/dev/streams/clone/*");
#endif
#endif

LFSSTATIC struct module_info clone_minfo = {
	mi_idnum:CONFIG_STREAMS_CLONE_MODID,
	mi_idname:CONFIG_STREAMS_CLONE_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

LFSSTATIC struct qinit clone_rinit = {
	// qi_putp:putq,
	qi_minfo:&clone_minfo,
};

LFSSTATIC struct qinit clone_winit = {
	// qi_putp:putq,
	qi_minfo:&clone_minfo,
};

LFSSTATIC struct streamtab clone_info = {
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
LFSSTATIC int
cloneopen(struct inode *inode, struct file *file)
{
#ifdef LIS
	return (ENXIO);
#else
	struct cdevsw *cdev;
	dev_t dev = inode->i_ino;

	if (file->private_data)
		/* Darn.  Somebody passed us a FIFO inode. */
		return (-EIO);

#if 0
	if ((cdev = cdrv_get(getminor(dev))))
#else
	if ((cdev = sdev_get(getminor(dev))))
#endif
	{
		int err;

		dev = makedevice(cdev->d_modid, 0);
		err = spec_open(file, cdev, dev, CLONEOPEN);
		sdev_put(cdev);
		return (err);
	}
	return (-ENOENT);
#endif
}

#ifdef LFS
struct file_operations clone_ops ____cacheline_aligned = {
	.owner = THIS_MODULE,
	.open = cloneopen,
};
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef LFS
static struct cdevsw clone_cdev = {
	.d_name = "clone",
	.d_str = &clone_info,
	.d_flag = D_CLONE | D_MP,
	.d_fop = &clone_ops,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  REGISTRATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef LFS
int
register_clone(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin;

	ptrace(("%s: registering clone minor for %s\n", __FUNCTION__, cdev->d_name));
	err = -ENOMEM;
	if (!(cmin = kmalloc(sizeof(*cmin), GFP_ATOMIC))) {
		printd(("could not allocate minor devnode structure\n"));
		goto error;
	}
	memset(cmin, 0, sizeof(*cmin));
	INIT_LIST_HEAD(&cmin->n_list);
	INIT_LIST_HEAD(&cmin->n_hash);
	cmin->n_name = cdev->d_name;
	cmin->n_str = cdev->d_str;
	cmin->n_flag = clone_cdev.d_flag;
	cmin->n_modid = cdev->d_modid;
	cmin->n_count = (atomic_t) ATOMIC_INIT(0);
	cmin->n_sqlvl = cdev->d_sqlvl;
	cmin->n_syncq = cdev->d_syncq;
	cmin->n_kmod = cdev->d_kmod;
	cmin->n_major = clone_cdev.d_major;
	cmin->n_mode = clone_cdev.d_mode;
	cmin->n_minor = cdev->d_major;
	cmin->n_dev = &clone_cdev;
#if 0
	if ((err = register_strnod(&clone_cdev, cmin, cdev->d_modid)) < 0)
#else
	if ((err = register_strnod(&clone_cdev, cmin, cdev->d_major)) < 0)
#endif
	{
		printd(("%s: could not register minor node for %s, err = %d\n", __FUNCTION__,
			cdev->d_name, -err));
		kfree(cmin);
		goto error;
	}
	printd(("%s: registered clone minor for %s\n", __FUNCTION__, cdev->d_name));
      error:
	return (err);
}

EXPORT_SYMBOL(register_clone);

int
unregister_clone(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin;

	err = -ENXIO;
	if (!(cmin = cmin_get(&clone_cdev, cdev->d_modid)))
		goto error;
	if ((err = unregister_strnod(&clone_cdev, cdev->d_modid)))
		goto error;
	kfree(cmin);
      error:
	return (err);
}

EXPORT_SYMBOL(unregister_clone);
#endif


/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for clone devices.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  clone_open: - open a clone device node
 *  @inode: the external filesystem inode
 *  @file: the external filesystem file pointer
 *
 *  clone_open() is only used to open a clone device from a character device node in an external
 *  filesystem.  This is never called for direct opens of a specfs device node (for direct opens see
 *  spec_dev_open() in strspecfs.c).  The character device number from the inode is used to
 *  determine the shadow special filesystem (internal) inode and chain the open call.
 *
 *  This is the separation point where we convert the external device number to an internal device
 *  number.  The external device number is contained in inode->i_rdev.
 */
LFSSTATIC int
clone_open(struct inode *inode, struct file *file)
{
#ifdef LIS
	return (ENXIO);
#else
	int err;
	struct cdevsw *cdev;
	major_t major;
	minor_t minor;
	modID_t modid, instance;

	ptrace(("%s: opening clone device\n", __FUNCTION__));
	if ((err = down_interruptible(&inode->i_sem)))
		goto exit;
#ifdef HAVE_KFUNC_TO_KDEV_T
	minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	major = MAJOR(kdev_t_to_nr(inode->i_rdev));
#else
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
#endif
	printd(("%s: external major %hu, minor %hu\n", __FUNCTION__, major, minor));
	minor = cdev_minor(&clone_cdev, major, minor);
	major = clone_cdev.d_major;
	printd(("%s: base major %hu, extended minor %hu\n", __FUNCTION__, major, minor));
	modid = clone_cdev.d_modid;
	printd(("%s: internal major %hu\n", __FUNCTION__, modid));
	err = -ENXIO;
	printd(("%s: device maps to internal major %hu, minor %hu\n", __FUNCTION__, modid, 0));
	if (!(cdev = sdev_get(minor))) {
		printd(("%s: could not find driver for minor %hu\n", __FUNCTION__, minor));
		goto up_exit;
	}
	printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
	instance = cdev->d_modid;
	printd(("%s: opening driver %s\n", __FUNCTION__, cdev->d_name));
	err = spec_open(file, cdev, makedevice(modid, instance), CLONEOPEN);
	printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
	sdev_put(cdev);
      up_exit:
	up(&inode->i_sem);
      exit:
	return (err);
#endif
}

LFSSTATIC struct file_operations clone_f_ops ____cacheline_aligned = {
	.owner = NULL, /* yes NULL */
	.open = clone_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_CLONE_MODULE
LFSSTATIC
#endif
int __init
clone_init(void)
{
#ifdef LFS
	int err;

#ifdef CONFIG_STREAMS_CLONE_MODULE
	printk(KERN_INFO CLONE_BANNER);
#else
	printk(KERN_INFO CLONE_SPLASH);
#endif
	clone_minfo.mi_idnum = modid;
	if ((err = register_cmajor(&clone_cdev, major, &clone_f_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
#endif
	return (0);
};

#ifdef CONFIG_STREAMS_CLONE_MODULE
LFSSTATIC
#endif
void __exit
clone_exit(void)
{
#ifdef LFS
	if (unregister_cmajor(&clone_cdev, major) != 0)
		swerr();
#endif
};

#ifdef CONFIG_STREAMS_CLONE_MODULE
module_init(clone_init);
module_exit(clone_exit);
#endif
