/*****************************************************************************

 @(#) File: src/drivers/bpf.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

static char const ident[] = "src/drivers/bpf.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 * This is a bpf driver that works like the clone driver.  It accepts registration from other
 * drivers and provides a /dev/streams/bpf major device that has a /dev/streams/bpf/[driver] minor
 * device for each driver that registers.  [driver] is the driver name.  When the device is opened,
 * instead of forwarding to (minor,0) as does the clone driver, it forwards to (minor,2) with the
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

#include <net/bpf.h>		/* extern verification */

#include "sys/config.h"

#define BPF_DESCRIP	"Berkeley Packet Filter (BPF) STREAMS Driver"
#define BPF_EXTRA	"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define BPF_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define BPF_REVISION	"OpenSS7 src/drivers/bpf.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define BPF_DEVICE	"SVR 4.2 MP STREAMS BPF Driver"
#define BPF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define BPF_LICENSE	"GPL"
#define BPF_BANNER	BPF_DESCRIP	"\n" \
			BPF_EXTRA	"\n" \
			BPF_COPYRIGHT	"\n" \
			BPF_REVISION	"\n" \
			BPF_DEVICE	"\n" \
			BPF_CONTACT	"\n"
#define BPF_SPLASH	BPF_DEVICE	" - " \
			BPF_REVISION	"\n"

#ifdef CONFIG_STREAMS_BPF_MODULE
MODULE_AUTHOR(BPF_CONTACT);
MODULE_DESCRIPTION(BPF_DESCRIP);
MODULE_SUPPORTED_DEVICE(BPF_DEVICE);
MODULE_LICENSE(BPF_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_BPF_MODULE */

#ifndef CONFIG_STREAMS_BPF_NAME
#error "CONFIG_STREAMS_BPF_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_BPF_MODID
#error "CONFIG_STREAMS_BPF_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_BPF_MAJOR
#error "CONFIG_STREAMS_BPF_MAJOR must be defined."
#endif

#ifdef CONFIG_STREAMS_BPF_MODULE
modID_t bpf_modid = CONFIG_STREAMS_BPF_MODID;

#ifndef module_param
MODULE_PARM(bpf_modid, "h");
#else
module_param(bpf_modid, ushort, 0444);
#endif
MODULE_PARM_DESC(bpf_modid, "Module id number for BPF driver.");
#else				/* CONFIG_STREAMS_BPF_MODULE */
static modID_t bpf_modid = CONFIG_STREAMS_BPF_MODID;
#endif				/* CONFIG_STREAMS_BPF_MODULE */

#ifdef CONFIG_STREAMS_BPF_MODULE
major_t major = CONFIG_STREAMS_BPF_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Major device number for BPF driver.");
#else				/* CONFIG_STREAMS_BPF_MODULE */
static major_t major = CONFIG_STREAMS_BPF_MAJOR;
#endif				/* CONFIG_STREAMS_BPF_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-bpf");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_BPF_MODID));
MODULE_ALIAS("streams-driver-bpf");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_BPF_MAJOR) "-*");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_BPF_MAJOR));
MODULE_ALIAS("/dev/streams/bpf");
MODULE_ALIAS("/dev/streams/bpf/*");
#endif
#endif

static struct module_info bpf_minfo = {
	.mi_idnum = CONFIG_STREAMS_BPF_MODID,
	.mi_idname = CONFIG_STREAMS_BPF_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat bpf_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat bpf_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit bpf_rinit = {
	.qi_putp = strrput,
	.qi_srvp = strrsrv,
	.qi_qopen = str_open,
	.qi_qclose = str_close,
	.qi_minfo = &bpf_minfo,
	.qi_mstat = &bpf_rstat,
};

static struct qinit bpf_winit = {
	.qi_putp = strwput,
	.qi_srvp = strwsrv,
	.qi_minfo = &bpf_minfo,
	.qi_mstat = &bpf_wstat,
};

#ifdef CONFIG_STREAMS_BPF_MODULE
static
#endif
struct streamtab bpfinfo = {
	.st_rdinit = &bpf_rinit,
	.st_wrinit = &bpf_winit,
};

/**
 *  bpfopen: - open a bpf special device
 *  @inode:	shadow special filesystem inode
 *  @file:	shadow special filesystem file pointer
 *
 *  bpfopen() is called only from within the shadow special filesystem.  This can occur by
 *  chaining from the external filesystem (e.g. openining a character device with bpf major) or by
 *  direct open of the inode within the mounted shadow special filesystem.  Either way, the inode
 *  number has our extended device numbering as a inode number and we chain the call within the
 *  shadow special filesystem.
 */
static int
bpfopen(struct inode *inode, struct file *file)
{
	struct cdevsw *cdev;
	dev_t dev = inode->i_ino;

	if (file->private_data)
		/* Darn.  Somebody passed us a FIFO inode. */
		return (-EIO);

	if ((cdev = sdev_get(getminor(dev)))) {
		int err;

		dev = makedevice(cdev->d_modid, BPF_NODE);
		err = spec_open(file, cdev, dev, CLONEOPEN);
		sdev_put(cdev);
		return (err);
	}
	return (-ENOENT);
}

struct file_operations bpf_ops ____cacheline_aligned = {
	.owner = THIS_MODULE,
	.open = bpfopen,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

static struct cdevsw bpf_cdev = {
	.d_name = "bpf",
	.d_str = &bpfinfo,
	.d_flag = D_CLONE | D_MP,
	.d_fop = &bpf_ops,
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
register_bpf(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin, *node;
	struct strapush *sap;

	err = -ENOMEM;
	if (!(cmin = kmalloc(2 * sizeof(*cmin) + sizeof(*sap), GFP_ATOMIC))) {
		goto error;
	}
	memset(cmin, 0, sizeof(*cmin));
	INIT_LIST_HEAD(&cmin->n_list);
	INIT_LIST_HEAD(&cmin->n_hash);
	cmin->n_name = cdev->d_name;
	cmin->n_str = cdev->d_str;
	cmin->n_flag = bpf_cdev.d_flag;
	cmin->n_modid = cdev->d_modid;
	cmin->n_count = (atomic_t) ATOMIC_INIT(0);
	cmin->n_sqlvl = cdev->d_sqlvl;
	cmin->n_syncq = cdev->d_syncq;
	cmin->n_kmod = cdev->d_kmod;
	cmin->n_major = bpf_cdev.d_major;
	cmin->n_mode = bpf_cdev.d_mode;
	cmin->n_minor = cdev->d_major;
	cmin->n_dev = &bpf_cdev;
	if ((err = register_strnod(&bpf_cdev, cmin, cdev->d_major)) < 0) {
		kfree(cmin);
		goto error;
	}
	node = cmin + 1;
	memset(node, 0, sizeof(*node));
	INIT_LIST_HEAD(&node->n_list);
	INIT_LIST_HEAD(&node->n_hash);
	node->n_name = "bpf";
	node->n_str = cdev->d_str;
	node->n_flag = D_CLONE;
	node->n_modid = cdev->d_modid;
	node->n_count = (atomic_t) ATOMIC_INIT(0);
	node->n_sqlvl = cdev->d_sqlvl;
	node->n_syncq = cdev->d_syncq;
	node->n_kmod = cdev->d_kmod;
	node->n_major = cdev->d_major;
	node->n_mode = cdev->d_mode;
	node->n_minor = BPF_NODE;
	node->n_dev = cdev;
	if ((err = register_strnod(cdev, node, BPF_NODE)) < 0) {
		unregister_strnod(&bpf_cdev, cdev->d_major);
		kfree(cmin);
		goto error;
	}
	sap = (typeof(sap)) (node + 1);
	memset(sap, 0, sizeof(*sap));
	sap->sap_cmd = SAP_CLONE;
	sap->sap_major = cdev->d_major;
	sap->sap_minor = BPF_NODE;
	sap->sap_lastminor = -1;
	sap->sap_npush = 1;
	snprintf(sap->sap_list[0], FMNAMESZ+1, "bfpmod");
	if ((err = autopush_add(sap)) < 0) {
		unregister_strnod(cdev, BPF_NODE);
		unregister_strnod(&bpf_cdev, cdev->d_major);
		kfree(cmin);
		goto error;
	}
      error:
	return (err);
}

EXPORT_SYMBOL_GPL(register_bpf);

streams_fastcall int
unregister_bpf(struct cdevsw *cdev)
{
	int err;
	struct devnode *cmin;
	struct strapush *sap;

	err = -ENXIO;
	if (!(cmin = cmin_get(&bpf_cdev, cdev->d_major)))
		goto error;
	sap = (typeof(sap)) (cmin + 2);
	if ((err = autopush_del(sap)) < 0)
		goto error;
	if ((err = unregister_strnod(cdev, BPF_NODE)))
		goto error;
	if ((err = unregister_strnod(&bpf_cdev, cdev->d_major)))
		goto error;
	kfree(cmin);
      error:
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_bpf);

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for bpf devices.
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  bpf_open: - open a bpf device node
 *  @inode: the external filesystem inode
 *  @file: the external filesystem file pointer
 *
 *  bpf_open() is only used to open a bpf device from a character device node in an external
 *  filesystem.  This is never called for direct opens of a specfs device node (for direct opens see
 *  spec_dev_open() in strspecfs.c).  The character device number from the inode is used to
 *  determine the shadow special filesystem (internal) inode and chain the open call.
 *
 *  This is the separation point where we convert the external device number to an internal device
 *  number.  The external device number is contained in inode->i_rdev.
 */
STATIC int
bpf_open(struct inode *inode, struct file *file)
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
	minor = cdev_minor(&bpf_cdev, major, minor);
	major = bpf_cdev.d_major;
	modid = bpf_cdev.d_modid;
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

STATIC struct file_operations bpf_f_ops ____cacheline_aligned = {
	.owner = NULL,		/* yes NULL */
	.open = bpf_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_BPF_MODULE
static
#endif
int __init
bpfinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_BPF_MODULE
	printk(KERN_INFO BPF_BANNER);
#else
	printk(KERN_INFO BPF_SPLASH);
#endif
	bpf_minfo.mi_idnum = bpf_modid;
	if ((err = register_cmajor(&bpf_cdev, major, &bpf_f_ops)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};

#ifdef CONFIG_STREAMS_BPF_MODULE
static
#endif
void __exit
bpfexit(void)
{
	if (unregister_cmajor(&bpf_cdev, major) != 0)
		swerr();
};

#ifdef CONFIG_STREAMS_BPF_MODULE
module_init(bpfinit);
module_exit(bpfexit);
#endif
