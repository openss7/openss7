/*****************************************************************************

 @(#) strreg.c,v (0.9.2.25) 2003/10/27 12:23:16

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified 2003/10/27 12:23:16 by brian

 *****************************************************************************/

#ident "@(#) strreg.c,v (0.9.2.25) 2003/10/27 12:23:16"

static char const ident[] =
    "strreg.c,v (0.9.2.25) 2003/10/27 12:23:16";

#define __NO_VERSION__

#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif
#include <linux/kernel.h>	/* for FASTCALL */
#include <linux/sched.h>	/* for current */
#include <linux/file.h>		/* for fput */
#include <linux/poll.h>
#include <linux/fs.h>

#include <linux/kmem.h>		/* for kmem_ */
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strsubr.h>
#include <linux/strconf.h>
#include <linux/ddi.h>

#include "strdebug.h"
#include "strhead.h"		/* for stream operations */
#include "strreg.h"		/* extern verification and str_args */

#ifndef CONFIG_STREAMS_FIFOS_MAJOR
#define CONFIG_STREAMS_FIFOS_MAJOR 211
//#error "CONFIG_STREAMS_FIFOS_MAJOR must be defined."
#endif
#ifndef CONFIG_STREAMS_SOCKSYS_MAJOR
#define CONFIG_STREAMS_SOCKSYS_MAJOR 40
//#error "CONFIG_STREAMS_SOCKSYS_MAJOR must be defined."
#endif

rwlock_t cdevsw_lock = RW_LOCK_UNLOCKED;
rwlock_t fmodsw_lock = RW_LOCK_UNLOCKED;
rwlock_t minors_lock = RW_LOCK_UNLOCKED;

struct list_head cdevsw_list = LIST_HEAD_INIT(cdevsw_list);	/* Devices go here */
struct list_head fmodsw_list = LIST_HEAD_INIT(fmodsw_list);	/* Modules go here */
struct list_head minors_list = LIST_HEAD_INIT(minors_list);	/* Minors go here */

struct fmodsw *fmodsw[MAX_STRMOD] ____cacheline_aligned = { NULL, };
struct cdevsw *cdevsw[MAX_STRDEV] ____cacheline_aligned = { NULL, };

/* 
 *  -------------------------------------------------------------------------
 *
 *  Get and put devices and modules.
 *
 *  -------------------------------------------------------------------------
 */
struct cdevsw *sdev_grab(struct cdevsw *sdev, dev_t dev)
{
	if (sdev) {
		int retry;
		for (retry = 0; retry < 2; retry++) {
			if (sdev->d_str && try_inc_mod_count(sdev->d_kmod))
				return (sdev);
#ifdef CONFIG_KMOD
			if (!retry) {
				char devname[64];
				snprintf(devname, 64, "streams-major-%d-minor-%d", getmajor(dev),
					 getminor(dev));
				request_module(devname);
				continue;
			}
#endif
			break;
		}
	}
	return (NULL);
}

/**
 *  sdev_get - get a reference to a STREAMS device
 *  @dev:device number of the STREAMS device
 *
 */
struct cdevsw *sdev_get(dev_t dev)
{
	struct cdevsw *sdev = NULL;
	struct char_device *cd;
	if ((cd = cdget(dev))) {
		// sdev = cd->data;
		cdput(cd);
	}
	if (!sdev)
		sdev = cdevsw[getmajor(dev)];
	return sdev_grab(sdev, dev);
}

/**
 *  sdev_put - put a reference to a STREAMS device
 *  @sdev:STREAMS device structure pointer to put
 *
 */
void sdev_put(struct cdevsw *sdev)
{
	if (sdev)
		__MOD_DEC_USE_COUNT(sdev->d_kmod);
}

/**
 *  sdev_find - find a STREAMS device by its name
 *  @name - the name to find
 */
struct cdevsw *sdev_find(const char *name)
{
	struct list_head *pos;
	size_t len = strnlen(name, FMNAMESZ + 1);
	read_lock(&cdevsw_lock);
	list_for_each(pos, &cdevsw_list) {
		struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_list);
		if (strnlen(cdev->d_name, FMNAMESZ + 1) != len)
			continue;
		if (memcmp(name, cdev->d_name, len))
			continue;
		sdev_grab(cdev, makedevice(cdev->d_index, 0));
		read_unlock(&cdevsw_lock);
		return (cdev);
	}
	read_unlock(&cdevsw_lock);
	return (NULL);
}

/**
 *  smod_get - get a reference to a STREAMS module
 *  @name:name of the module
 */
struct fmodsw *smod_get(const char *name)
{
	struct list_head *pos;
	int try = 0;
      try_again:
	try++;
	read_lock(&fmodsw_lock);
	list_for_each(pos, &fmodsw_list) {
		struct fmodsw *fmod = list_entry(pos, struct fmodsw, f_list);
		if (strncmp(fmod->f_name, name, FMNAMESZ + 1))
			continue;
		if (!try_inc_mod_count(fmod->f_kmod)) {
			if (try < 2) {
				char modname[32 + FMNAMESZ + 1];
				read_unlock(&fmodsw_lock);
				snprintf(modname, 32 + FMNAMESZ + 1, "streams-%s", name);
#ifdef CONFIG_KMOD
				request_module(modname);
#endif
				goto try_again;
			}
			read_unlock(&fmodsw_lock);
			return (NULL);
		}
		read_unlock(&fmodsw_lock);
		return (fmod);
	}
	read_unlock(&fmodsw_lock);
	return (NULL);
}

/**
 *  smod_put - put a reference to a STREAMS module
 *  @smod:STREAMS module structure pointer to put
 *
 */
void smod_put(struct fmodsw *smod)
{
	if (smod)
		__MOD_DEC_USE_COUNT(smod->f_kmod);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for character based streams, fifos and pipes.
 *
 *  -------------------------------------------------------------------------
 */

static struct dentry *create_hash(struct qstr *name, struct dentry *base)
{
	struct dentry *new;
	struct inode *inode;
	struct super_block *sb;
	int err;
	inode = base->d_inode;
	if ((err = permission(inode, MAY_EXEC)))
		goto out;
	if (base->d_op && base->d_op->d_hash)
		if ((err = base->d_op->d_hash(base, name)) < 0)
			goto out;
	err = -ENOMEM;
	if (!(new = d_alloc(base, name)))
		goto out;
	sb = inode->i_sb;
	inode = new_inode(sb);
	if (!inode)
		goto dput_out;
	inode->i_ino = name->hash;
	inode->i_state = I_LOCK;
	sb->s_op->read_inode(inode);
	inode->i_state &= ~I_LOCK;
	wake_up(&inode->i_wait);
	d_add(new, inode);
	err = -ENOMEM;
	if (!inode->i_cdev)
		goto dput_out;
	err = -ENODEV;
	if (is_bad_inode(inode))
		goto dput_out;
	return (new);
      dput_out:
	dput(new);
      out:
	return (ERR_PTR(err));
}

void file_swap_put(struct file *f1, struct file *f2)
{
	f1->f_op = xchg(&f2->f_op, f1->f_op);
	f1->f_dentry = xchg(&f2->f_dentry, f1->f_dentry);
	f1->f_vfsmnt = xchg(&f2->f_vfsmnt, f1->f_vfsmnt);
	f1->private_data = xchg(&f2->private_data, f1->private_data);
	file_move(f1, &f1->f_dentry->d_inode->i_sb->s_files);
	file_move(f2, &f2->f_dentry->d_inode->i_sb->s_files);
	fput(f2);
}

int sdev_open(struct inode *i, struct file *f, struct vfsmount *mnt, struct str_args *argp)
{
	struct dentry *dentry;
	struct file *file;
	struct inode *inode;
	int err;
	argp->name.len = 0;	/* sprintf(argp->buf, "[%d]", argp->dev); */
	argp->name.hash = argp->dev;
	if ((err = down_interruptible(&i->i_sem)) < 0)
		goto exit;
	switch (argp->sflag) {
	case DRVOPEN:
		dentry = lookup_hash(&argp->name, mnt->mnt_root);
		break;
	case CLONEOPEN:
		/* we always need a new stream head, so we always create a new dentry and inode */
		dentry = create_hash(&argp->name, mnt->mnt_root);
		break;
	default:
		swerr();
		err = -EIO;
		goto up_exit;
	}
	err = PTR_ERR(dentry);
	if (IS_ERR(dentry))
		goto up_exit;
	/* we only fail to get an inode when memory allocation fails */
	err = -ENOMEM;
	if (!(inode = dentry->d_inode))
		goto dput_exit;
	/* we only get a bad inode when there is no device entry */
	err = -ENODEV;
	if (is_bad_inode(inode))
		goto dput_exit;
	dentry->d_fsdata = argp;	/* this is how we pass open arguments */
	file = dentry_open(dentry, mnt, f->f_flags);
	if ((err = PTR_ERR(file)) < 0)
		goto up_exit;
	if (err == 0)
		file_swap_put(f, file);
	else
		/* fifo returns 1 on exit to cleanup shadow pointer and use existing file pointer */
		err = 0;
	goto up_exit;
      dput_exit:
	dput(dentry);
      up_exit:
	up(&i->i_sem);
      exit:
	return (err);
}

/* 
 *  strm_open() is used to nest another ridirected call.  It is used by clone
 *  devices such as the Clone device and the Named Stream Device.
 */
int strm_open(struct inode *inode, struct file *file)
{
	return sdev_open(inode, file, file->f_vfsmnt, xchg(&file->f_dentry->d_fsdata, NULL));
}

extern struct vfsmount *specfs_mnt;

static int spec_open(struct inode *inode, struct file *file)
{
	struct char_device *cd;
	struct cdevsw *cdev;
	struct str_args args = {
		file:file,
		dev:kdev_t_to_nr(inode->i_rdev),
		oflag:make_oflag(file),
		sflag:DRVOPEN,
		crp:current_creds,
		name:{args.buf, 0, 0},
	};
	int err;
	switch (inode->i_mode & S_IFMT) {
#ifdef CONFIG_STREAMS_FIFOS
	case S_IFIFO:
		args.dev = makedevice(CONFIG_STREAMS_FIFOS_MAJOR, 0);
		break;
#endif
#ifdef CONFIG_STREAMS_SOCKSYS
	case S_IFSOCK:
		args.dev = makedevice(CONFIG_STREAMS_SOCKSYS_MAJOR, 0);
		break;
#endif
	case S_IFCHR:
		args.dev = kdev_t_to_nr(inode->i_rdev);
		break;
	default:
		swerr();
		return (-EIO);
	}
	if (!(cd = inode->i_cdev) && !(cd = inode->i_cdev = cdget(args.dev)))
		return (-ENOMEM);	/* memory problem */
	/* autoload and determine cloning right now */
	if (!(cdev = sdev_get(args.dev)))
		return (-ENXIO);
	args.sflag = (cdev->d_flag & D_CLONE) ? CLONEOPEN : DRVOPEN;
	err = sdev_open(inode, file, specfs_mnt, &args);
	sdev_put(cdev);
	return (err);
}

static struct file_operations spec_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	open:spec_open,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and Deregistration
 *
 *  -------------------------------------------------------------------------
 */

int register_inode_major(dev_t dev, struct cdevsw *cdev, struct file_operations *fops)
{
	int err;
	major_t major = getmajor(dev);
	minor_t minor = getminor(dev);
	if (!cdev || major >= MAX_STRDEV || minor != 0)
		goto einval;
	write_lock(&cdevsw_lock);
	if (cdevsw[major] && cdevsw[major] != cdev)
		goto ebusy;
	if ((err = register_chrdev(major, cdev->d_name, fops)) >= 0) {
		if (err == 0 && major != 0)
			err = major;
		if (!cdev->d_regs++) {
			list_add_tail(&cdev->d_list, &cdevsw_list);
			if (!cdev->d_apush.next || !cdev->d_apush.prev)
				INIT_LIST_HEAD(&cdev->d_apush);
		}
		cdevsw[major] = cdev;
	} else
		ptrace(("[ERROR:%d] - cdev %p, major %hhu, minor %hhu\n", err, cdev, major, minor));
      unlock_and_out:
	write_unlock(&cdevsw_lock);
      out:
	return (err);
      ebusy:
	ptrace(("[EBUSY] - cdev %p, major %hhu, minor %hhu\n", cdev, major, minor));
	err = -EBUSY;
	goto unlock_and_out;
      einval:
	ptrace(("[EINVAL] - cdev %p, major %hhu, minor %hhu\n", cdev, major, minor));
	err = -EINVAL;
	goto out;
};
int unregister_inode_major(dev_t dev, struct cdevsw *cdev)
{
	int err;
	major_t major = getmajor(dev);
	minor_t minor = getminor(dev);
	if (major >= MAX_STRMOD || minor != 0)
		goto einval;
	write_lock(&cdevsw_lock);
	cdev = cdev ? : cdevsw[major];
	if (!cdev)
		goto enoent;
	if (!cdevsw[major])
		goto enxio;
	if (cdevsw[major] != cdev)
		goto eperm;
	if ((err = unregister_chrdev(major, cdev->d_name)) >= 0) {
		if (!--cdev->d_regs) {
			list_del_init(&cdev->d_list);
		}
		cdevsw[major] = NULL;
	} else
		ptrace(("[ERROR:%d] - cdev %p, major %hhu, minor %hhu\n", err, cdev, major, minor));
      unlock_and_out:
	write_unlock(&cdevsw_lock);
      out:
	return (err);
      eperm:
	ptrace(("[EPERM] - cdev %p, major %hhu, minor %hhu\n", cdev, major, minor));
	err = -EPERM;
	goto unlock_and_out;
      enoent:
	ptrace(("[ENOENT] - cdev %p, major %hhu, minor %hhu\n", cdev, major, minor));
	err = -ENOENT;
	goto unlock_and_out;
      enxio:
	ptrace(("[ENXIO] - cdev %p, major %hhu, minor %hhu\n", cdev, major, minor));
	err = -ENXIO;
	goto unlock_and_out;
      einval:
	ptrace(("[EINVAL] - cdev %p, major %hhu, minor %hhu\n", cdev, major, minor));
	err = -EINVAL;
	goto out;
};

#if 0
int register_inode_minor(dev_t dev, struct cdevsw *cdev)
{
	unsigned long flags;
	struct char_device *cd;
	int err;
	err = -EINVAL;
	if (!cdev)
		goto exit;
	spin_lock_irqsave(&cdevsw_lock, flags);
	err = -ENOMEM;
	if (!(cd = cdget(dev)))
		goto unlock_exit;
	err = -EBUSY;
	if (cd->data && cd->data != cdev)
		goto put_unlock;
	err = 0;
	if (cd->data == cdev)
		goto unlock_exit;
	err = -ENODEV;
	if (!try_inc_mod_count(cdev->d_kmod))
		goto put_unlock;
	err = 0;
	if (!cdev->d_regs++) {
		list_add_tail(&cdev->d_list, &cdevsw_list);
		if (!cdev->d_apush.next || !cdev->d_apush.prev)
			INIT_LIST_HEAD(&cdev->d_apush);
	}
	cd->data = cdev;
	/* we keep a hold on the char_device until it is unregistered */
	goto unlock_exit;
      put_unlock:
	cdput(cd);
      unlock_exit:
	spin_unlock_irqrestore(&cdevsw_lock, flags);
      exit:
	return (err);
}
int unregister_inode_minor(dev_t dev, struct cdevsw *cdev)
{
	unsigned long flags;
	struct char_device *cd;
	int err;
	spin_lock_irqsave(&cdevsw_lock, flags);
	err = -ENOENT;
	if (!(cd = cdget(dev)))
		goto unlock_exit;
	err = -ENOENT;
	cdev = cdev ? : cd->data;
	if (!cdev)
		goto put_unlock;
	err = -ENXIO;
	if (!cd->data)
		goto put_unlock;
	err = -EPERM;
	if (cd->data != cdev)
		goto put_unlock;
	err = 0;
	if (!--cdev->d_regs)
		list_del_init(&cdev->d_list);
	cdput(cd);		/* put it away */
      put_unlock:
	cdput(cd);
      unlock_exit:
	spin_unlock_irqrestore(&cdevsw_lock, flags);
	return (err);
}
#endif
int register_inode(dev_t dev, struct cdevsw *cdev, struct file_operations *fops)
{
#if 0
	if (getminor(dev))
		return register_inode_minor(dev, cdev);
#endif
	return register_inode_major(dev, cdev, fops);
}
int unregister_inode(dev_t dev, struct cdevsw *cdev)
{
#if 0
	if (getminor(dev))
		return unregister_inode_minor(dev, cdev);
#endif
	return unregister_inode_major(dev, cdev);
}

/**
 *  register_strdev_major - register a STREAMS device against a device major number
 *  @dev:device number containing requested major or 0 for automatic major selection
 *  @cdev:STREAMS character device structure to register
 *
 *  DESCRIPTION:register_strdev_major() registers the device specified by the
 *  @cdev to the device major number specified by @dev.  Only the
 *  getmajor(@dev) component of @dev is significant and the getminor(@dev)
 *  component must be coded zero (0).
 *
 *  register_strdev_major() will register the STREAMS character device
 *  specified by @cdev against the major device number in getmajor(@dev).  If
 *  the major device number is zero, then it requests that register_strdev()
 *  allocate an available major device number and assign it to @cdev.
 *
 *  CONTEXT:register_strdev_major() is intended to be called from kernel
 *  __init() or module_init() routines only.  It cannot be called from
 *  in_irq() level.
 *
 *  RETURN VALUES:Upon success, register_strdev_major() will return the
 *  requested or assigned major device number as a positive integer value.
 *  Upon failure, the registration is denied and a negative error number is
 *  returned.
 *
 *  ERRORS:Upon failure, register_strdev_major() returns on of the negative
 *  error numbers listed below.
 *
 *  -[%EINVAL]	@cdev was NULL or getminmor(@dev) was non-zero.
 *
 *  -[%EBUSY]	a device was already registered against the requested major
 *		device number, or no device numbers were available for
 *		automatic major device number assignment.
 *
 *  NOTES:Linux Fast-STREAMS provides improvements over LiS.
 *
 *  LfS maintains a much smaller cdevsw[] table (one pointer per device major)
 *  by requiring the driver to (statically) allocate its &struct cdevsw
 *  structure using an approach more like the Solaris &struct cb_ops.
 */
int register_strdev_major(dev_t dev, struct cdevsw *cdev)
{
	if (!cdev)
		return (-EINVAL);
	cdev->d_fop = &strm_f_ops;
	cdev->d_mode = S_IFCHR;
	return register_inode_major(dev, cdev, &spec_f_ops);
}

/**
 *  unregister_strdev_major - unregister previously registered STREAMS device
 *  @dev:device number containing major to unregister or 0 for all majors
 *  @cdev:STREAMS character device structure to unregister
 *
 *  DESCRIPTION:unregister_strdev_major() unregisters the device specified by
 *  the @cdev from the device major number specified by @dev.  Only the
 *  getmajor(@dev) component of @dev is significant and the getminor(@dev)
 *  component must be coded zero (0).
 *
 *  unregister_strdev_major() will unregister the STREAMS character device
 *  specified by @cdev from the major device number in getmajor(@dev).  If the
 *  major device number is zero, then it requests that unregister_strdev()
 *  unregister @cdev from any device majors with which it is currently
 *  registered.
 *
 *  CONTEXT:unregister_strdev_major() is intended to be called from kernel
 *  __exit() or module_exit() routines only.  It cannot be called from
 *  in_irq() level.
 *
 *  RETURN VALUES:Upon success, unregister_strdev_major() will return zero
 *  (0).  Upon failure, the deregistration is denied and a negative error
 *  number is returned.
 *
 *  ERRORS:Upon failure, unregister_strdev_major() returns one of the negative
 *  error numbers listed below.
 *
 *  -[%ENXIO]	The specified device does not exist in the registration
 *		tables.
 *
 *  -[%EINVAL]	@cdev is NULL, or the minor number component of @dev is
 *		non-zero, or the @d_name component associated with @cdev has
 *		changed since registration.
 *
 *  -[%EPERM]	The device number specified does not belong to the &struct
 *		cdev structure specified and permission is therefore denied.
 */
int unregister_strdev_major(dev_t dev, struct cdevsw *cdev)
{
	return unregister_inode_major(dev, cdev);
}

#if 0
/**
 *  register_strdev_minor - register STREAMS device
 *  @dev:device number containing requested major and minor
 *  @cdev:STREAMS character device structure to register
 */
int register_strdev_minor(dev_t dev, struct cdevsw *cdev)
{
	if (!cdev)
		goto einval;
	cdev->d_fop = &strm_f_ops;
	cdev->d_mode = S_IFCHR;
	return register_inode_minor(dev, cdev);
      einval:
	return (-EINVAL);
}

/**
 *  unregister_strdev_minor - unregister STREAMS device
 *  @dev:device number containing existing major and minor
 *  @cdev:STREAMS character device structure to unregister
 *  
 *  DESCRIPTION:unregister_strdev_minor() unregisters the device specified by
 *  @cdev from the device number specified by @dev.  Both the major and minor
 *  number of @dev are significant.
 */
int unregister_strdev_minor(dev_t dev, struct cdevsw *cdev)
{
	return unregister_inode_minor(dev, cdev);
}
#endif

/**
 *  register_strdev - register STREAMS device
 *  @dev:device number to register
 *  @cdev:STREAMS character device structure to register
 *
 *  DESCRIPTION:register_strdev() registers the device specified by the @cdev
 *  to the device number specified by @dev.  Both the major and minor number
 *  of @dev are sigificant.  If the getminor(@dev) component of @dev is zero,
 *  then register_strdev() will register the specified STREAMS character
 *  device @cdev against the getmajor(@dev) major device number.  If the
 *  getminor(@dev) component of @dev is non-zero, then register_strdev() will
 *  register the specified STREAMS character device @cdev against the
 *  major/minor device number combination.
 *
 *  NOTES:register_strdev() is implemented as a call to either
 *  register_strdev_major() or register_strdev_minor() depending on whether
 *  getminor(@dev) is zero or non-zero.
 *
 *  RETURN VALUES:register_strdev() will return the same values as
 *  register_strdev_minor() or register_strdev_major().  Upon success, the
 *  a positive value indicating the major device number used will be returned.
 *  Upon failure, a negative error number is returned.
 *
 *  ERRORS:Upon failure, the requested registration is not performed and one
 *  of the negative error numbers listed below is returned.
 *
 *  -[%ENOMEM]	insufficient memory was available to complete the request.
 *
 *  -[%EBUSY]	a different device structure is already registered against the
 *		requested device number, or there were no device numbers
 *		available to be assigned.
 *
 *  -[%EINVAL]	the request was invalid.  (@cdev was NULL).
 */
int register_strdev(dev_t dev, struct cdevsw *cdev)
{
	if (getminor(dev) != 0)
		return register_strdev_minor(dev, cdev);
	return register_strdev_major(dev, cdev);
}

int unregister_strdev(dev_t dev, struct cdevsw *cdev)
{
	if (getminor(dev) != 0)
		return unregister_strdev_minor(dev, cdev);
	return unregister_strdev_major(dev, cdev);
}

/**
 *  register_strmod - register STREAMS module
 *  @modid:requested module id or 0 for automatic allocation
 *  @fmod:STREAMS module structure to register
 */
int register_strmod(modID_t modid, struct fmodsw *fmod)
{
	int err, i;
	if (!fmod || !fmod->f_name || modid >= MAX_STRMOD)
		goto einval;
	write_lock(&fmodsw_lock);
	/* check if already in list */
	for (i = MAX_STRMOD - 1; i > 0; i--)
		if (fmodsw[i] == fmod)
			goto name_free;
	/* check for uniqueness of f_name */
	for (i = MAX_STRMOD - 1; i > 0; i--)
		if (fmodsw[i] && !strncmp(fmodsw[i]->f_name, fmod->f_name, FMNAMESZ + 1))
			break;
	if (i)
		/* name is taken */
		goto ebusy;
      name_free:
	/* name is free (or already assigned) */
	if (i != 0 && modid == 0)
		modid = i;
	if (modid == 0) {
		/* assign first free module id */
		for (modid = MAX_STRMOD - 1; modid > 0; modid--)
			if (!fmodsw[modid])
				break;
		/* table full */
		if (!modid)
			goto enxio;
	}
	/* check if slot free or already assigned */
	if (fmodsw[modid] && fmodsw[modid] != fmod)
		goto ebusy;
	if (!fmod->f_regs++)
		list_add_tail(&fmod->f_list, &fmodsw_list);
	fmodsw[modid] = fmod;
	err = modid;
      done:
	write_unlock(&fmodsw_lock);
      out:
	return (err);
      ebusy:
	err = -EBUSY;
	goto done;
      enxio:
	err = -ENXIO;
	goto done;
      einval:
	err = -EINVAL;
	goto out;
}

/**
 *  unregister_strmod
 *  @modid:modid to unregister or 0 for don't care
 *  @fmod:STREAMS module structure to unregister
 */
int unregister_strmod(modID_t modid, struct fmodsw *fmod)
{
	int err;
	if (0 >= modid || modid >= MAX_STRMOD)
		goto einval;
	write_lock(&fmodsw_lock);
	if (modid == 0) {
		/* check if exists in list */
		for (modid = MAX_STRMOD - 1; modid > 0; modid--)
			if (fmodsw[modid] == fmod)
				break;
	}
	if (!modid || !fmodsw[modid])
		goto enxio;
	if (fmodsw[modid] != fmod)
		goto eperm;
	if (!--fmod->f_regs)
		list_del_init(&fmod->f_list);
	fmodsw[modid] = NULL;
	err = modid;
      unlock_out:
	write_unlock(&fmodsw_lock);
      out:
	return (err);
      enxio:
	err = -ENXIO;
	goto unlock_out;
      eperm:
	err = -EPERM;
	goto unlock_out;
      einval:
	err = -EINVAL;
	goto out;
}

modID_t getmid(const char *name)
{
	ushort modid = 0;
	int i;
	struct cdevsw *cdev;
	struct fmodsw *fmod;
	struct streamtab *st;
	struct smodule_info *mi;
	if (!name || !*name)
		return (modid);
	for (i = MAX_STRDEV - 1; i; i--)
		if ((cdev = cdevsw[i]) && (st = cdev->d_str) && st->st_rdinit &&
		    (mi = st->st_rdinit->qi_minfo) && !strncmp(name, mi->mi_idname, FMNAMESZ)) {
			modid = mi->mi_idnum;
			break;
		}
	if (!modid)
		return (modid);
	for (i = MAX_STRMOD - 1; i; i--)
		if ((fmod = fmodsw[i]) && (st = fmod->f_str) && st->st_rdinit &&
		    (mi = st->st_rdinit->qi_minfo) && !strncmp(name, mi->mi_idname, FMNAMESZ)) {
			modid = mi->mi_idnum;
			break;
		}
	return (modid);
}

qi_qadmin_t getadmin(modID_t modid)
{
	qi_qadmin_t qadmin = NULL;
	int i;
	struct cdevsw *cdev;
	struct fmodsw *fmod;
	struct streamtab *st;
	struct smodule_info *mi;
	for (i = MAX_STRDEV - 1; i; i--)
		if ((cdev = cdevsw[i]) && (st = cdev->d_str) && st->st_rdinit &&
		    (mi = st->st_rdinit->qi_minfo) && mi->mi_idnum == modid) {
			qadmin = st->st_rdinit->qi_qadmin;
			break;
		}
	if (qadmin)
		return (qadmin);
	for (i = MAX_STRMOD - 1; i; i--)
		if ((fmod = fmodsw[i]) && (st = fmod->f_str) && st->st_rdinit &&
		    (mi = st->st_rdinit->qi_minfo) && mi->mi_idnum == modid) {
			qadmin = st->st_rdinit->qi_qadmin;
			break;
		}
	return (qadmin);
}

int strreg_init(void)
{
	return (0);
}
void strreg_exit(void)
{
	return;
}
