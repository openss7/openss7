/*****************************************************************************

 @(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2004/06/01 12:04:38 $

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

 Last Modified $Date: 2004/06/01 12:04:38 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2004/06/01 12:04:38 $"

static char const ident[] =
    "$RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.25 $) $Date: 2004/06/01 12:04:38 $";

#define __NO_VERSION__

#include <linux/compiler.h>
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
#include <linux/kernel.h>	/* for FASTCALL */
#include <linux/sched.h>	/* for current */
#include <linux/file.h>		/* for fput */
#include <linux/poll.h>
#include <linux/fs.h>
#include <asm/hardirq.h>

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/kmem.h>		/* for kmem_ */
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "strdebug.h"
#include "sth.h"		/* for stream operations */
#include "strsched.h"		/* for di_alloc and di_put */
#include "strlookup.h"		/* cdevsw_list, etc. */
#include "strreg.h"		/* extern verification */
/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and Deregistration
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  register_strmod: - register STREAMS module
 *  @fmod: STREAMS module structure to register
 */
int register_strmod(struct fmodsw *fmod)
{
	int err = 0;
	write_lock(&fmodsw_lock);
	do {
		modID_t modid;
		struct module_info *mi;
		err = -EINVAL;
		if (!fmod || !fmod->f_name || !fmod->f_name[0]) {
			ptrace(("invalid argument\n"));
			break;
		} else {
			struct streamtab *st;
			struct qinit *qi;
			err = -EINVAL;
			if (!(st = fmod->f_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo)) {
				ptrace(("invalid argument\n"));
				break;
			}
		}
		err = -EBUSY;
		/* check name for another module */
		if (__smod_search(fmod->f_name)) {
			ptrace(("%s already registered\n", fmod->f_name));
			break;
		}
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			for (modid = (modID_t) (-1UL); modid && __fmod_lookup(modid); modid--) ;
			err = -ENXIO;
			if (!modid) {
				ptrace(("table full\n"));
				break;
			}
			mi->mi_idnum = modid;
		} else {
			err = -EBUSY;
			/* use specified module id */
			if (__fmod_lookup(modid)) {
				ptrace(("module id %hu already registered", modid));
				break;
			}
		}
		fmod_add(fmod, modid);
		err = modid;
		printd(("STREAMS: registered module %s, modid %hu\n", fmod->f_name, modid));
	} while (0);
	write_unlock(&fmodsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(register_strmod);

/**
 *  unregister_strmod:
 *  @fmod: STREAMS module structure to unregister
 */
int unregister_strmod(struct fmodsw *fmod)
{
	int err = 0;
	write_lock(&fmodsw_lock);
	do {
		err = -EINVAL;
		if (!fmod || !fmod->f_name || !fmod->f_name[0]) {
			ptrace(("invalid argument\n"));
			break;
		}
		err = -ENXIO;
		if (!fmod->f_list.next || list_empty(&fmod->f_list)) {
			ptrace(("not registered\n"));
			break;
		}
		fmod_del(fmod);
		printd(("STREAMS: unregistered module %s\n", fmod->f_name));
	} while (0);
	write_unlock(&fmodsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_strmod);

/**
 *  register_strdrv: - register STREAMS driver
 *  @cdev: STREAMS device structure to register
 *  @mnt: mount point of the Shadow Special Filesystem
 */
int register_strdrv(struct cdevsw *cdev, struct vfsmount *mnt)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	do {
		modID_t modid;
		struct module_info *mi;
		struct cdevsw *c;
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0]) {
			ptrace(("invalid argument\n"));
			break;
		} else {
			struct streamtab *st;
			struct qinit *qi;
			err = -EINVAL;
			if (!(st = cdev->d_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo)) {
				ptrace(("invalid argument\n"));
				break;
			}
		}
		/* check name for another module */
		if ((c = __smod_search(cdev->d_name))) {
			err = -EPERM;
			if (c != cdev) {
				ptrace(("module name %s registered to another\n", cdev->d_name));
				break;
			}
			err = -EBUSY;
			ptrace(("module name %s already registered to us\n", cdev->d_name));
			break;
		}
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			for (modid = (modID_t) (-1UL); modid && __cdrv_lookup(modid); modid--) ;
			err = -ENXIO;
			if (!modid) {
				ptrace(("table full\n"));
				break;
			}
			mi->mi_idnum = modid;
		} else {
			/* use specified module id */
			if ((c = __cdrv_lookup(modid))) {
				if (c != cdev) {
					err = -EPERM;
					ptrace(("module id %hu registered to another\n", modid));
					break;
				}
				/* already registered to us */
				err = -EBUSY;
				ptrace(("module id %hu already registered to us\n", modid));
				break;
			}
		}
		if ((err = cdev_add(mnt->mnt_root, cdev, modid))) {
			ptrace(("couldn't allocate dentry\n"));
			break;
		}
		err = modid;
		printd(("STREAMS: registered driver %s, modid %hu\n", cdev->d_name, modid));
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(register_strdrv);

/**
 *  unregister_strdrv:
 *  @cdev: STREAMS driver structure to unregister
 *  @mnt: mount point of the Shadow Special Filesystem
 */
int unregister_strdrv(struct cdevsw *cdev, struct vfsmount *mnt)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	do {
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0]) {
			ptrace(("invalid argument\n"));
			break;
		}
		err = -ENXIO;
		if (!cdev->d_list.next || list_empty(&cdev->d_list)) {
			ptrace(("not registered\n"));
			break;
		}
		err = -EBUSY;
		if (!list_empty(&cdev->d_majors)) {
			pswerr(("still have registered majors"));
			break;
		}
		if (!list_empty(&cdev->d_minors)) {
			pswerr(("still have registered minors"));
			break;
		}
		if (!list_empty(&cdev->d_apush)) {
			pswerr(("still have registered autopush list"));
			break;
		}
		if (!list_empty(&cdev->d_stlist)) {
			pswerr(("still have active stream heads"));
			break;
		}
		cdev_del(mnt->mnt_root, cdev);
		printd(("STREAMS: unregistered module %s\n", cdev->d_name));
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_strdrv);

/**
 *  register_cmajor: - register a character device inode
 *  @cdev: character device switch structure pointer
 *  @major: major device number
 *  @fops: file operations to apply to external character device nodes
 *
 *  Registers and extenal character special device major number with Linux outside the STREAMS
 *  subsystem.  @fops is the file operations that will be used to open the character device.
 *
 *  The major device number can be specified as zero (0), in which case the major device number will
 *  be assigned to a free major device number by register_chrdev() and returned as a positive return
 *  value.  Any valid external major device number can be used for @major.
 *
 *  register_cmajor() can be called multiple times for the same registered cdevsw entries to
 *  register additional major device numbers for the same entry.  On each call to register_cmajor()
 *  only one character major device number will be allocated.  If @major is zero on each call, a new
 *  available major device number will be allocated on each call.
 */
int register_cmajor(struct cdevsw *cdev, struct devinfo *devi, major_t major, struct file_operations *fops)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	do {
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0] || !devi) {
			ptrace(("invalid argument\n"));
			break;
		}
		err = -EINVAL;
		if (major >= MAX_CHRDEV) {
			ptrace(("invalid major device number\n"));
			break;
		}
		err = -EINVAL;
		/* ensure that the device is registered (as a module) */
		if (!cdev->d_list.next || list_empty(&cdev->d_list)) {
			ptrace(("module not registered\n"));
			break;
		}
		err = -EBUSY;
		if (major && __devi_lookup(major)) {
			ptrace(("major %hu already registered\n", major));
			break;
		}
		/* register the character device */
		if ((err = register_chrdev(major, cdev->d_name, fops)) < 0) {
			ptrace(("could not register character device, errno=%d\n", -err));
			break;
		}
		if (err > 0 && major == 0)
			major = err;
		devi_add(devi, cdev, major);
		err = major;
		printd(("STREAMS: registered driver %s, major %hu\n", cdev->d_name, major));
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(register_cmajor);

/**
 *  unregister_cmajor: - unregister a special device inode
 *  @cdev: character device switch structure pointer
 *  @major: major device number (character special devices only)
 *
 *  Deregisters an external character device major number previously registered with
 *  register_cmajor().  @major must be the major number returned from a successful call to
 *  register_cmajor() or the special value zero (0).  When @major is specified, all major device
 *  numbers associated with the driver will be deregistered.  In this way, one one call to
 *  unregister_cmajor() with @major set to zero (0) is necessary after multiple successful calls to
 *  register_cmajor().
 *
 *  Notices: The major device number must be the major device number returned from a successful
 *  register_cmajor() call.  cdev->d_name must have the same value as on the
 *  call to register_cmajor()
 *  or the call will fail.
 */
int unregister_cmajor(struct cdevsw *cdev, struct devinfo *devi, major_t major)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	do {
		struct devinfo *d;
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0]) {
			ptrace(("invalid arguments\n"));
			break;
		}
		err = -EINVAL;
		if (major >= MAX_CHRDEV) {
			ptrace(("invalid major argument\n"));
			break;
		}
		if (major) {
			/* deregister one specific major device number */
			err = -ENXIO;
			if (!(d = __devi_lookup(major))) {
				ptrace(("major not registered\n"));
				break;
			}
			err = -EPERM;
			if (d != devi) {
				ptrace(("major registered to another device\n"));
				break;
			}
			if ((err = unregister_chrdev(major, cdev->d_name)) < 0) {
				pswerr(("could not unregister character device, errno=%d\n", -err));
				break;
			}
			devi_del(devi, cdev);
			printd(("STREAMS: unregistered driver %s, major %hu\n", cdev->d_name, devi->major));
		} else {
			struct list_head *pos;
			/* deregister all major device numbers */
			list_for_each(pos, &cdev->d_majors) {
				devi = list_entry(pos, struct devinfo, di_list);
				unregister_chrdev(devi->major, cdev->d_name);
				devi_del(devi, cdev);
				printd(("STREAMS: unregistered driver %s, major %hu\n", cdev->d_name, devi->major));
			}
		}
		err = 0;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_cmajor);

/**
 *  register_strnod: - register a minor device node
 *  @cdev: character device switch structure pointer
 *  @node: minor device node structure pointer
 *  @minor: minor device number
 */
int register_strnod(struct cdevsw *cdev, struct devnode *node, minor_t minor)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	do {
		struct devnode *n;
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0]) {
			ptrace(("invalid argument\n"));
			break;
		}
		err = -EINVAL;
		if (!node || !node->n_name || !node->n_name[0]) {
			ptrace(("invalid argument\n"));
			break;
		}
		err = -EINVAL;
		/* ensure that the device is regsitered (as a module) */
		if (!cdev->d_list.next || list_empty(&cdev->d_list)) {
			ptrace(("module not registered\n"));
			break;
		}
#if 0
		/* well, no, it doesn't have to be registered with an external character major
		   device; we can assign minor nodes internal to the specfs. */
		err = -EINVAL;
		/* ensure that the device is regsitered (as a driver) */
		if (!cdev->d_majors.next || list_empty(&cdev->d_majors)) {
			ptrace(("driver not registered\n"));
			break;
		}
#endif
		/* check name for another node */
		if (!cdev->d_minors.next)
			INIT_LIST_HEAD(&cdev->d_minors);
		if ((n = __node_search(cdev, node->n_name))) {
			err = -EPERM;
			if (n != node) {
				ptrace(("name %s registered to another\n", node->n_name));
				break;
			}
			err = -EBUSY;
			ptrace(("name %s already registered to us\n", node->n_name));
			break;
		}
		if ((minor == (minor_t) (-1UL))) {
			/* find a free minor */
			for (minor = 0; (minor != (minor_t) (-1UL)) && __node_lookup(cdev, minor);
			     minor++) ;
			err = -ENXIO;
			if ((minor == (minor_t) (-1UL))) {
				ptrace(("table full\n"));
				break;
			}
		} else {
			/* use specified minor */
			if ((n = __node_lookup(cdev, minor))) {
				err = -EPERM;
				if (n != node) {
					ptrace(("minor %hu registered to another\n", minor));
					break;
				}
				err = -EBUSY;
				ptrace(("minor %hu already registered to us\n", minor));
				break;
			}
		}
		if ((err = node_add(node, cdev, minor))) {
			ptrace(("couldn't allocate dentry\n"));
			break;
		}
		err = minor;
		printd(("STREAMS: registered driver %s, minor %hu, %s\n", cdev->d_name, minor,
			node->n_name));
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(register_strnod);

/**
 *  unregister_strnod: - unregister a minor device node
 *  @cdev: character device switch structure pointer
 *  @node: minor device node structure pointer
 *  @minor: minor device number
 */
int unregister_strnod(struct cdevsw *cdev, minor_t minor)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	do {
		struct devnode *node;
		err = -EINVAL;
		if (!cdev) {
			ptrace(("invalid arguments\n"));
			break;
		}
		if (minor != (minor_t) (-1UL)) {
			/* deregister one specific minor device number */
			err = -ENXIO;
			if (!(node = __node_lookup(cdev, minor))) {
				ptrace(("minor not registered\n"));
				break;
			}
			node_del(node, cdev);
			printd(("STREAMS: unregistered driver %s, minor %hu, %s\n", cdev->d_name,
				node->n_minor, node->n_name));
		} else {
			/* deregister all minor devices */
			struct list_head *pos;
			if (!cdev->d_minors.next)
				INIT_LIST_HEAD(&cdev->d_minors);
			/* deregister all minor device numbers */
			list_for_each(pos, &cdev->d_minors) {
				node = list_entry(pos, struct devnode, n_list);
				node_del(node, cdev);
				printd(("STEAMS: unregistered driver %s, minor %hu, %s\n",
					cdev->d_name, node->n_minor, node->n_name));
			}
			INIT_LIST_HEAD(&cdev->d_minors);
		}
		err = 0;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL_GPL(unregister_strnod);

/*
 *  -------------------------------------------------------------------------
 *  -------------------------------------------------------------------------
 */

#ifndef HAVE_FILE_MOVE_ADDR
STATIC void _file_move(struct file *file, struct list_head *list)
{
	if (!list)
		return;
	file_list_lock();
	list_del(&file->f_list);
	list_add(&file->f_list, list);
	file_list_unlock();
}
#endif

STATIC INLINE void file_swap_put(struct file *f1, struct file *f2)
{
#ifdef HAVE_FILE_MOVE_ADDR
	typeof(&file_move) _file_move = (typeof(_file_move)) HAVE_FILE_MOVE_ADDR;
#endif
	f1->f_op = xchg(&f2->f_op, f1->f_op);
	f1->f_dentry = xchg(&f2->f_dentry, f1->f_dentry);
	f1->f_vfsmnt = xchg(&f2->f_vfsmnt, f1->f_vfsmnt);
	f1->private_data = xchg(&f2->private_data, f1->private_data);
	_file_move(f1, &f1->f_dentry->d_inode->i_sb->s_files);
	_file_move(f2, &f2->f_dentry->d_inode->i_sb->s_files);
	fput(f2);
}

/*
 *  dentry_open2: - simplified form of dentry_open()
 *  @dentry:	    the dentry to open
 *  @mnt:	    the vfsmount to open
 *  @cdev:	    the STREAMS device entry
 *  @argp:	    pointer to arguments
 */
STATIC INLINE struct file *dentry_open2(struct dentry *dentry, struct vfsmount *mnt, int flags,
					struct cdevsw *cdev, struct str_args *argp)
{
#ifdef HAVE_FILE_MOVE_ADDR
	typeof(&file_move) _file_move = (typeof(_file_move)) HAVE_FILE_MOVE_ADDR;
#endif
	static LIST_HEAD(kill_list);
	struct inode *inode = dentry->d_inode;
	struct file *file;
	int err;
	err = -ENFILE;
	if (!(file = get_empty_filp()))
		goto dput_error;
	file->f_flags = flags;
	file->f_mode = (flags + 1) & O_ACCMODE;
	file->f_dentry = dentry;
	file->f_vfsmnt = mnt;
	file->f_pos = 0;
	file->f_reada = 0;
	file->f_op = fops_get(cdev->d_fop);
	_file_move(file, &inode->i_sb->s_files);
	file->f_iobuf = NULL;
	file->f_iobuf_lock = 0;
	if (file->f_op && file->f_op->open && (err = file->f_op->open(inode, file)))
		goto error;
	file->f_flags &= ~(O_CREAT | O_EXCL | O_NOCTTY | O_TRUNC);
	return (file);
      error:
	fops_put(file->f_op);
	_file_move(file, &kill_list);	/* out of the way.. */
	file->f_dentry = NULL;
	file->f_vfsmnt = NULL;
	put_filp(file);
      dput_error:
	dput(dentry);
	mntput(mnt);
	return ERR_PTR(err);
}

/**
 *  strm_open:	- open a stream from an external character special device,
 *		fifo, or socket.  This is also used for nesting clone calls.
 *  @ext_inode:	external (or chaining) filesystem inode
 *  @ext_file:	external (or chaining) filesystem file pointer (user file pointer)
 *  @argp:	arguments to qopen
 */
int strm_open(struct inode *ext_inode, struct file *ext_file, struct str_args *argp)
{
	struct dentry *parent, *dentry = NULL;
	struct file *file;		/* next file pointer to use */
	struct inode *inode = ext_inode;	/* next inode to use */
	int err;
	struct cdevsw *cdev;
	struct devnode *node;
	if ((err = down_interruptible(&inode->i_sem)) < 0)
		goto exit;
	err = ENOENT;
	if (!(cdev = cdrv_get(getmajor(argp->dev))))
		goto up_exit;
	argp->name.name = cdev->d_name;
	argp->name.len = strnlen(cdev->d_name, FMNAMESZ);
	argp->name.hash = getmajor(argp->dev);
	parent = lookup_hash(&argp->name, argp->mnt->mnt_root);
	if (!parent)
		goto cput_exit;
	if ((err = PTR_ERR(parent)) < 0)
		goto cput_exit;
	err = -ENOMEM;
	if (!parent->d_inode)
		goto pput_exit;
	err = -ENODEV;
	if (is_bad_inode(parent->d_inode))
		goto pput_exit;
	up(&inode->i_sem);
	inode = parent->d_inode;
	if ((err = down_interruptible(&inode->i_sem)) < 0) {
		inode = NULL;
		goto pput_exit;
	}
	if ((node = node_get(cdev, getminor(argp->dev)))) {
		argp->name.name = node->n_name;
		argp->name.len = strnlen(node->n_name, FMNAMESZ);
		argp->name.hash = argp->dev;
	} else {
		argp->name.name = argp->buf;
		argp->name.len = snprintf(argp->buf, sizeof(argp->buf), "%u", getminor(argp->dev));
		argp->name.hash = argp->dev;
	}
	dentry = lookup_hash(&argp->name, parent);
	if (!dentry)
		goto pput_exit;
	if ((err = PTR_ERR(dentry)) < 0)
		goto pput_exit;
	/* we only fail to get an inode when memory allocation fails */
	err = -ENOMEM;
	if (!dentry->d_inode)
		goto dput_exit;
	/* we only get a bad inode when there is no device entry */
	err = -ENODEV;
	if (is_bad_inode(dentry->d_inode))
		goto dput_exit;
	up(&inode->i_sem);
	inode = NULL;
	file = dentry_open2(dentry, argp->mnt, ext_file->f_flags, cdev, argp);
	if ((err = PTR_ERR(file)) < 0)
		goto dput_exit;
	if (err == 0) {
		file_swap_put(ext_file, file);
		goto cput_exit;
	}
	/* fifo returns 1 on exit to cleanup shadow pointer and use existing file pointer */
	err = 0;
      dput_exit:
	dput(dentry);
      pput_exit:
	dput(parent);
      cput_exit:
	cdrv_put(cdev);
      up_exit:
	if (inode)
		up(&inode->i_sem);
      exit:
	return (err);
}

#if defined CONFIG_STREAMS_STH_MODULE || CONFIG_STREAMS_CLONE_MODULE || CONFIG_STREAMS_NSDEV_MODULE
EXPORT_SYMBOL_GPL(strm_open);
#endif
