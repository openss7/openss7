/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2013  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * This is a nit driver that works like the clone driver.  It accepts registration from other
 * drivers and provides a /dev/streams/nit major device that has a /dev/streams/nit/[driver] minor
 * device for each driver that registers.  [driver] is the driver name.  When the device is opened,
 * instead of forwarding to (minor,0) as does the clone driver, it forwards to (minor,1) with the
 * CLONEOPEN flag set.  There is also a dlpi and bpf driver that does a similar thing.
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <net/nit_if.h>		/* extern verification */

#include "sys/config.h"

#define NIT_DESCRIP	"SVR 4.1 SNIT (NIT_IF) STREAMS Driver"
#define NIT_EXTRA	"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NIT_COPYRIGHT	"Copyright (c) 2008-2013  Monavacon Limited.  All Rights Reserved."
#define NIT_REVISION	"LfS $RCSfile$ $Name$($Revision$) $Date$"
#define NIT_DEVICE	"SVR 4.2 MP STREAMS NIT Driver"
#define NIT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NIT_LICENSE	"GPL"
#define NIT_BANNER	NIT_DESCRIP	"\n" \
			NIT_EXTRA	"\n" \
			NIT_COPYRIGHT	"\n" \
			NIT_REVISION	"\n" \
			NIT_DEVICE	"\n" \
			NIT_CONTACT	"\n"
#define NIT_SPLASH	NIT_DEVICE	" - " \
			NIT_REVISION	"\n"

#ifdef CONFIG_STREAMS_NIT_MODULE
MODULE_AUTHOR(NIT_CONTACT);
MODULE_DESCRIPTION(NIT_DESCRIP);
MODULE_SUPPORTED_DEVICE(NIT_DEVICE);
MODULE_LICENSE(NIT_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_NIT_MODULE */

#ifndef CONFIG_STREAMS_NIT_NAME
#error "CONFIG_STREAMS_NIT_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_NIT_MODID
#error "CONFIG_STREAMS_NIT_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_NIT_MAJOR
#error "CONFIG_STREAMS_NIT_MAJOR must be defined."
#endif

#ifdef CONFIG_STREAMS_NIT_MODULE
modID_t nit_modid = CONFIG_STREAMS_NIT_MODID;

#ifndef module_param
MODULE_PARM(nit_modid, "h");
#else
module_param(nit_modid, ushort, 0444);
#endif
MODULE_PARM_DESC(nit_modid, "Module id number for NIT driver.");
#else				/* CONFIG_STREAMS_NIT_MODULE */
static modID_t nit_modid = CONFIG_STREAMS_NIT_MODID;
#endif				/* CONFIG_STREAMS_NIT_MODULE */

#ifdef CONFIG_STREAMS_NIT_MODULE
major_t major = CONFIG_STREAMS_NIT_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for NIT driver.");
#else				/* CONFIG_STREAMS_NIT_MODULE */
static major_t major = CONFIG_STREAMS_NIT_MAJOR;
#endif				/* CONFIG_STREAMS_NIT_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-nit");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NIT_MODID));
MODULE_ALIAS("streams-driver-nit");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(CONFIG_STREAMS_NIT_MAJOR));
MODULE_ALIAS("/dev/nit");
MODULE_ALIAS("devname:nit");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NIT_MAJOR));
MODULE_ALIAS("/dev/streams/nit");
MODULE_ALIAS("/dev/streams/nit/*");
#endif
#endif

static struct module_info nit_minfo = {
	.mi_idnum = CONFIG_STREAMS_NIT_MODID,
	.mi_idname = CONFIG_STREAMS_NIT_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat nit_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat nit_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit nit_rinit = {
	.qi_putp = strrput,
	.qi_srvp = strrsrv,
	.qi_qopen = str_open,
	.qi_qclose = str_close,
	.qi_minfo = &nit_minfo,
	.qi_mstat = &nit_rstat,
};

static struct qinit nit_winit = {
	.qi_putp = strwput,
	.qi_srvp = strwsrv,
	.qi_minfo = &nit_minfo,
	.qi_mstat = &nit_wstat,
};

#ifdef CONFIG_STREAMS_NIT_MODULE
static
#endif
struct streamtab nitinfo = {
	.st_rdinit = &nit_rinit,
	.st_wrinit = &nit_winit,
};

/**
 *  nitopen: - open a nit special device
 *  @inode:	shadow special filesystem inode
 *  @file:	shadow special filesystem file pointer
 *
 *  nitopen() is called only from within the shadow special filesystem.  This can occur by
 *  chaining from the external filesystem (e.g. openining a character device with nit major) or by
 *  direct open of the inode within the mounted shadow special filesystem.  Either way, the inode
 *  number has our extended device numbering as a inode number and we chain the call within the
 *  shadow special filesystem.
 */
static int
nitopen(struct inode *inode, struct file *file)
{
	struct cdevsw *cdev;
	dev_t dev = inode->i_ino;

	if (file->private_data)
		/* Darn.  Somebody passed us a FIFO inode. */
		return (-EIO);

	if ((cdev = sdev_get(getminor(dev)))) {
		int err;

		dev = makedevice(cdev->d_modid, NIT_NODE);
		err = spec_open(file, cdev, dev, CLONEOPEN);
		sdev_put(cdev);
		return (err);
	}
	return (-ENOENT);
}

struct file_operations nit_ops ____cacheline_aligned = {
	.owner = THIS_MODULE,
	.open = nitopen,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

static struct cdevsw nit_cdev = {
	.d_name = "nit",
	.d_str = &nitinfo,
	.d_flag = D_CLONE | D_MP,
	.d_fop = &nit_ops,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  REGISTRATION
 *
 *  -------------------------------------------------------------------------
 */

streams_fastcall int
register_nit(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin, *node;

	err = -ENOMEM;
	if (!(cmin = kmalloc(2 * sizeof(*cmin), GFP_ATOMIC))) {
		goto error;
	}
	memset(cmin, 0, sizeof(*cmin));
	INIT_LIST_HEAD(&cmin->n_list);
	INIT_LIST_HEAD(&cmin->n_hash);
	cmin->n_name = cdev->d_name;
	cmin->n_str = cdev->d_str;
	cmin->n_flag = nit_cdev.d_flag;
	cmin->n_modid = cdev->d_modid;
	cmin->n_count = (atomic_t) ATOMIC_INIT(0);
	cmin->n_sqlvl = cdev->d_sqlvl;
	cmin->n_syncq = cdev->d_syncq;
	cmin->n_kmod = cdev->d_kmod;
	cmin->n_major = nit_cdev.d_major;
	cmin->n_mode = nit_cdev.d_mode;
	cmin->n_minor = cdev->d_major;
	cmin->n_dev = &nit_cdev;
	if ((err = register_strnod(&nit_cdev, cmin, cdev->d_major)) < 0) {
		kfree(cmin);
		goto error;
	}
	node = cmin + 1;
	memset(node, 0, sizeof(*node));
	INIT_LIST_HEAD(&node->n_list);
	INIT_LIST_HEAD(&node->n_hash);
	node->n_name = "nit";
	node->n_str = cdev->d_str;
	node->n_flag = D_CLONE;
	node->n_modid = cdev->d_modid;
	node->n_count = (atomic_t) ATOMIC_INIT(0);
	node->n_sqlvl = cdev->d_sqlvl;
	node->n_syncq = cdev->d_syncq;
	node->n_kmod = cdev->d_kmod;
	node->n_major = cdev->d_major;
	node->n_mode = cdev->d_mode;
	node->n_minor = NIT_NODE;
	node->n_dev = cdev;
	if ((err = register_strnod(cdev, node, NIT_NODE)) < 0) {
		unregister_strnod(&nit_cdev, cdev->d_major);
		kfree(cmin);
		goto error;
	}
      error:
	return (err);
}

EXPORT_SYMBOL_GPL(register_nit);

streams_fastcall int
unregister_nit(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin;

	err = -ENXIO;
	if (!(cmin = cmin_get(&nit_cdev, cdev->d_major)))
		goto error;
	if ((err = unregister_strnod(cdev, NIT_NODE)))
		goto error;
	if ((err = unregister_strnod(&nit_cdev, cdev->d_major)))
		goto error;
	kfree(cmin);
      error:
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_nit);

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for nit devices.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  nit_open: - open a nit device node
 *  @inode: the external filesystem inode
 *  @file: the external filesystem file pointer
 *
 *  nit_open() is only used to open a nit device from a character device node in an external
 *  filesystem.  This is never called for direct opens of a specfs device node (for direct opens see
 *  spec_dev_open() in strspecfs.c).  The character device number from the inode is used to
 *  determine the shadow special filesystem (internal) inode and chain the open call.
 *
 *  This is the separation point where we convert the external device number to an internal device
 *  number.  The external device number is contained in inode->i_rdev.
 */
STATIC int
nit_open(struct inode *inode, struct file *file)
{
	int err;
	struct cdevsw *cdev;
	major_t major;
	minor_t minor;
	modID_t modid, instance;

#if defined HAVE_KFUNC_TO_KDEV_T
	minor = MINOR(kdev_t_to_nr(inode->i_rdev));
	major = MAJOR(kdev_t_to_nr(inode->i_rdev));
#else
	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);
#endif
	minor = cdev_minor(&nit_cdev, major, minor);
	major = nit_cdev.d_major;
	modid = nit_cdev.d_modid;
	err = -ENXIO;
	if (!(cdev = sdev_get(minor))) {
		goto exit;
	}
	instance = cdev->d_modid;
	err = spec_open(file, cdev, makedevice(modid, instance), CLONEOPEN);
	sdev_put(cdev);
      exit:
	return (err);
}

STATIC struct file_operations nit_f_ops ____cacheline_aligned = {
	.owner = NULL,		/* yes NULL */
	.open = nit_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_NIT_MODULE
static
#endif
int __init
nitinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_NIT_MODULE
	printk(KERN_INFO NIT_BANNER);
#else
	printk(KERN_INFO NIT_SPLASH);
#endif
	nit_minfo.mi_idnum = nit_modid;
	if ((err = register_cmajor(&nit_cdev, major, &nit_f_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_NIT_MODULE
static
#endif
void __exit
nitexit(void)
{
	if (unregister_cmajor(&nit_cdev, major) != 0)
		swerr();
};

#ifdef CONFIG_STREAMS_NIT_MODULE
module_init(nitinit);
module_exit(nitexit);
#endif
