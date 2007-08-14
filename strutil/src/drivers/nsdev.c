/*****************************************************************************

 @(#) $RCSfile: nsdev.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/14 12:58:01 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 12:58:01 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: nsdev.c,v $
 Revision 0.9.2.38  2007/08/14 12:58:01  brian
 - GNUv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: nsdev.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/14 12:58:01 $"

static char const ident[] =
    "$RCSfile: nsdev.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/14 12:58:01 $";

#define _LFS_SOURCE

#include <sys/os7/compat.h>

#ifdef LIS
#define	CONFIG_STREAMS_NSDEV_MODID	NSDEV_DRV_ID
#define	CONFIG_STREAMS_NSDEV_NAME	NSDEV_DRV_NAME
#define	CONFIG_STREAMS_NSDEV_MAJOR	NSDEV_CMAJOR_0
#define LFSSTATIC
#endif
#ifdef LFS
#define LFSSTATIC static
#endif

#define NSDEV_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NSDEV_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define NSDEV_REVISION	"LfS $RCSfile: nsdev.c,v $ $Name:  $($Revision: 0.9.2.38 $) $Date: 2007/08/14 12:58:01 $"
#define NSDEV_DEVICE	"SVR 4.2 STREAMS Named Stream Device (NSDEV) Driver"
#define NSDEV_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NSDEV_LICENSE	"GPL v2"
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
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-nsdev");
#endif
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

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module id number for NSDEV driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NSDEV_MODID));
MODULE_ALIAS("streams-driver-nsdev");
#endif

major_t major = CONFIG_STREAMS_NSDEV_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for NSDEV driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_NSDEV_MAJOR) "-*");
MODULE_ALIAS("/dev/nsdev");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NSDEV_MAJOR));
MODULE_ALIAS("/dev/streams/nsdev");
MODULE_ALIAS("/dev/streams/nsdev/*");
MODULE_ALIAS("/dev/streams/clone/nsdev");
#endif
#endif

LFSSTATIC struct module_info nsdev_minfo = {
	.mi_idnum = CONFIG_STREAMS_NSDEV_MODID,
	.mi_idname = CONFIG_STREAMS_NSDEV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat nsdev_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

LFSSTATIC struct qinit nsdev_rinit = {
	// qi_putp:putq,
	qi_minfo:&nsdev_minfo,
	qi_mstat:&nsdev_mstat,
};

LFSSTATIC struct qinit nsdev_winit = {
	// qi_putp:putq,
	qi_minfo:&nsdev_minfo,
	qi_mstat:&nsdev_mstat,
};

LFSSTATIC struct streamtab nsdev_info = {
	st_rdinit:&nsdev_rinit,
	st_wrinit:&nsdev_winit,
};

/**
 *  nsdevopen:	- open the named streams device
 *  @inode:	shadow special filesystem inode to open
 *  @file:	shadow special filesystem file pointer to open
 *
 *  This is rather simple.  We are going to do a redirected open on the a new device with the major
 *  device number mapped according to name.  We do this by nesting another spec_open() inside the
 *  first one with an adjusted device number. It helps that the orginal file pointer is stored with
 *  the args passed as private_data attached to the current file pointer.  We use this to find the
 *  original file pointer and dentry and get the name of the opened file.
 *
 *  If we don't find a reasonable match and kmod is equipped, we try to load the module with
 *  'streams-' prefixed to the name and run through the list again.  We could also generate the
 *  request from the path to the original dentry.
 */
LFSSTATIC int
nsdevopen(struct inode *inode, struct file *file)
{
#ifdef LIS
	return (ENXIO);
#else
	struct cdevsw *cdev;
	int err;

	if ((cdev = cdev_match(file->f_dentry->d_name.name))) {
		major_t major = cdev->d_modid;
		minor_t minor = getminor(inode->i_ino);
		dev_t dev = makedevice(major, minor);
		int sflag = (file->f_flags & O_CLONE) ? CLONEOPEN : DRVOPEN;

		_printd(("%s: %s: matched device\n", __FUNCTION__, cdev->d_name));
		err = spec_open(file, cdev, dev, sflag);
		_printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
		_ctrace(sdev_put(cdev));
	} else
		err = -ENOENT;
	return (err);
#endif
}

struct file_operations nsdev_ops ____cacheline_aligned = {
	.owner = NULL,			/* yes NULL */
	.open = nsdevopen,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

LFSSTATIC struct cdevsw nsdev_cdev = {
	.d_name = CONFIG_STREAMS_NSDEV_NAME,
	.d_str = &nsdev_info,
	.d_flag = D_MP,
	.d_fop = &nsdev_ops,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for clone devices.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  nsdev_open: - open a named clone device node
 *  @inode: the external filesystem inode
 *  @file: the external filesystem file pointer
 *
 *  nsdev_open() is only used to open a named clone device from a character device node in an
 *  external filesystem.  This is never called for direct opens of a specfs device node (for direct
 *  opens see spec_dev_open() in strspecfs.c).  The name of the device in the external filesystem
 *  combined with the minor device number is used to determine the shadow special filesystem
 *  (internal) inode an chain the open call.
 *
 *  This is a separation point between the external and internal filesystem where we convert the
 *  external device number to an internal device number.  The external device number is contained in
 *  inode->i_rdev.
 */

STATIC int
nsdev_open(struct inode *inode, struct file *file)
{
#ifdef LIS
	return (ENXIO);
#else
	int err;
	struct cdevsw *cdev;
	major_t major;
	minor_t minor;
	modID_t modid, instance;
	dev_t dev;

#if defined HAVE_KFUNC_TO_KDEV_T
	minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	major = MAJOR(kdev_t_to_nr(inode->i_rdev));
#else
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
#endif
	minor = cdev_minor(&nsdev_cdev, major, minor);
	major = nsdev_cdev.d_major;
	modid = nsdev_cdev.d_modid;
	err = -ENXIO;
	if (!(cdev = cdev_match(file->f_dentry->d_name.name)))
		goto exit;
	_printd(("%s: %s: matched device\n", __FUNCTION__, cdev->d_name));
	err = -ENXIO;
	if (cdev == &nsdev_cdev)
		goto cdev_put_exit;	/* would loop */
	instance = cdev->d_modid;
	dev = makedevice(modid, instance);
	err = spec_open(file, cdev, dev, CLONEOPEN);
      cdev_put_exit:
	_printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
	_ctrace(sdev_put(cdev));
      exit:
	return (err);
#endif
}

LFSSTATIC struct file_operations nsdev_f_ops ____cacheline_aligned = {
	.owner = THIS_MODULE,
	.open = nsdev_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZAION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_NSDEV_MODULE
LFSSTATIC
#endif
int __init
nsdev_init(void)
{
#ifdef LIS
	return (ENXIO);
#else
	int err;

#ifdef CONFIG_STREAMS_NSDEV_MODULE
	printk(KERN_INFO NSDEV_BANNER);
#else
	printk(KERN_INFO NSDEV_SPLASH);
#endif
	nsdev_minfo.mi_idnum = modid;
	if ((err = register_cmajor(&nsdev_cdev, major, &nsdev_f_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
#endif
};

#ifdef CONFIG_STREAMS_NSDEV_MODULE
LFSSTATIC
#endif
void __exit
nsdev_exit(void)
{
#ifdef LFS
	unregister_cmajor(&nsdev_cdev, major);
#endif
};

#ifdef CONFIG_STREAMS_NSDEV_MODULE
module_init(nsdev_init);
module_exit(nsdev_exit);
#endif
