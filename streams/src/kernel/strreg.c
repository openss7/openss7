/*****************************************************************************

 @(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.50 $) $Date: 2005/07/29 22:20:09 $

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

 Last Modified $Date: 2005/07/29 22:20:09 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.50 $) $Date: 2005/07/29 22:20:09 $"

static char const ident[] =
    "$RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.50 $) $Date: 2005/07/29 22:20:09 $";

#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif
#include <linux/kernel.h>	/* for FASTCALL */
#include <linux/sched.h>	/* for current */
#include <linux/file.h>		/* for fput */
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/mount.h>	/* for vfsmount and friends */
#if HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>	/* for lookup_hash on 2.6 */
#endif
#include <asm/hardirq.h>

#include <sys/kmem.h>		/* for kmem_ */
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "src/modules/sth.h"	/* for stream operations */
#include "src/kernel/strlookup.h"	/* cdevsw_list, etc. */
#include "src/kernel/strspecfs.h"	/* for specfs_get and specfs_put */
#include "src/kernel/strreg.h"	/* extern verification */
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
int
register_strmod(struct fmodsw *fmod)
{
	int err = 0;

	write_lock(&fmodsw_lock);
	do {
		modID_t modid;
		struct module_info *mi;

		err = -EINVAL;
		if (!fmod || !fmod->f_name || !fmod->f_name[0])
			break;
		else {
			struct streamtab *st;
			struct qinit *qi;

			err = -EINVAL;
			if (!(st = fmod->f_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo))
				break;
		}
		err = -EBUSY;
		/* check name for another module */
		if (__smod_search(fmod->f_name))
			break;
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			for (modid = (modID_t) (-1UL); modid && __fmod_lookup(modid); modid--) ;
			err = -ENXIO;
			if (!modid) {
				break;
			}
			mi->mi_idnum = modid;
		} else {
			err = -EBUSY;
			/* use specified module id */
			if (__fmod_lookup(modid)) {
				break;
			}
		}
		fmod_add(fmod, modid);
		err = modid;
	} while (0);
	write_unlock(&fmodsw_lock);
	return (err);
}

EXPORT_SYMBOL(register_strmod);

/**
 *  unregister_strmod:
 *  @fmod: STREAMS module structure to unregister
 */
int
unregister_strmod(struct fmodsw *fmod)
{
	int err = 0;

	write_lock(&fmodsw_lock);
	do {
		err = -EINVAL;
		if (!fmod || !fmod->f_name || !fmod->f_name[0])
			break;
		err = -ENXIO;
		if (!fmod->f_list.next || list_empty(&fmod->f_list))
			break;
		fmod_del(fmod);
	} while (0);
	write_unlock(&fmodsw_lock);
	return (err);
}

EXPORT_SYMBOL(unregister_strmod);

/**
 *  register_strdrv: - register STREAMS driver
 *  @cdev: STREAMS device structure to register
 *  @mnt: mount point of the Shadow Special Filesystem
 */
int
register_strdrv(struct cdevsw *cdev)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		modID_t modid;
		struct module_info *mi;
		struct cdevsw *c;

		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		else {
			struct streamtab *st;
			struct qinit *qi;

			err = -EINVAL;
			if (!(st = cdev->d_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo)) {
				break;
			}
		}
		/* check name for another module */
		if ((c = __smod_search(cdev->d_name))) {
			err = -EPERM;
			if (c != cdev)
				break;
			err = -EBUSY;
			break;
		}
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			for (modid = (modID_t) (-1UL); modid && __cdrv_lookup(modid); modid--) ;
			err = -ENXIO;
			if (!modid)
				break;
			mi->mi_idnum = modid;
		} else {
			/* use specified module id */
			if ((c = __cdrv_lookup(modid))) {
				if (c != cdev) {
					err = -EPERM;
					break;
				}
				/* already registered to us */
				err = -EBUSY;
				break;
			}
		}
		if ((err = cdev_add(cdev, modid)))
			break;
		err = modid;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL(register_strdrv);

/**
 *  unregister_strdrv:
 *  @cdev: STREAMS driver structure to unregister
 */
int
unregister_strdrv(struct cdevsw *cdev)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		err = -ENXIO;
		if (!cdev->d_list.next || list_empty(&cdev->d_list))
			break;
		err = -EBUSY;
		if (!list_empty(&cdev->d_majors))
			break;
		if (!list_empty(&cdev->d_minors))
			break;
		if (!list_empty(&cdev->d_apush))
			break;
		if (!list_empty(&cdev->d_stlist))
			break;
		cdev_del(cdev);
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL(unregister_strdrv);

/*
 *  register_xinode: - register a character device inode
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
 *  register_xinode() can be called multiple times for the same registered cdevsw entries to
 *  register additional major device numbers for the same entry.  On each call to register_xinode()
 *  only one character major device number will be allocated.  If @major is zero on each call, a new
 *  available major device number will be allocated on each call.
 */
STATIC int
register_xinode(struct cdevsw *cdev, struct devnode *cmaj, major_t major,
		struct file_operations *fops)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0] || !cmaj)
			break;
		err = -EINVAL;
#ifndef MAX_CHRDEV
		if (major != MAJOR(MKDEV(major, 0)))
#else
		if (major >= MAX_CHRDEV)
#endif
			break;
		err = -EINVAL;
		/* ensure that the device is registered (as a module) */
		if (!cdev->d_list.next || list_empty(&cdev->d_list))
			break;
		err = -EBUSY;
		if (major && __cmaj_lookup(major))
			break;
		/* register the character device */
		if ((err = register_chrdev(major, cdev->d_name, fops)) < 0)
			break;
		if (err > 0 && major == 0)
			major = err;
		cmaj_add(cmaj, cdev, major);
		err = major;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

/*
 *  unregister_xinode: - unregister a special device inode
 *  @cdev: character device switch structure pointer
 *  @major: major device number (character special devices only)
 *
 *  Deregisters an external character device major number previously registered with
 *  register_xinode().  @major must be the major number returned from a successful call to
 *  register_xinode() or the special value zero (0).  When @major is specified, all major device
 *  numbers associated with the driver will be deregistered.  In this way, one one call to
 *  unregister_xinode() with @major set to zero (0) is necessary after multiple successful calls to
 *  register_xinode().
 *
 *  Notices: The major device number must be the major device number returned from a successful
 *  register_xinode() call.  cdev->d_name must have the same value as on the
 *  call to register_xinode()
 *  or the call will fail.
 */
STATIC int
unregister_xinode(struct cdevsw *cdev, struct devnode *cmaj, major_t major)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		struct devnode *d;

		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		err = -EINVAL;
#ifndef MAX_CHRDEV
		if (major != MAJOR(MKDEV(major, 0)))
#else
		if (major >= MAX_CHRDEV)
#endif
			break;
		if (major) {
			/* deregister one specific major device number */
			err = -ENXIO;
			if (!(d = __cmaj_lookup(major)))
				break;
			err = -EPERM;
			if (d != cmaj)
				break;
			if ((err = unregister_chrdev(major, cdev->d_name)) < 0)
				break;
			cmaj_del(cmaj, cdev);
		} else {
			struct list_head *pos;

			/* deregister all major device numbers */
			list_for_each(pos, &cdev->d_majors) {
				cmaj = list_entry(pos, struct devnode, n_list);

				unregister_chrdev(cmaj->n_major, cdev->d_name);
				cmaj_del(cmaj, cdev);
			}
		}
		err = 0;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

int
register_cmajor(struct cdevsw *cdev, major_t major, struct file_operations *fops)
{
	int err;
	struct devnode *cmaj;

	if ((err = register_strdrv(cdev)) < 0 && err != -EBUSY)
		goto no_strdrv;
	err = -ENOMEM;
	if (!(cmaj = kmalloc(sizeof(*cmaj), GFP_ATOMIC)))
		goto no_cmaj;
	memset(cmaj, 0, sizeof(*cmaj));
	INIT_LIST_HEAD(&cmaj->n_list);
	INIT_LIST_HEAD(&cmaj->n_hash);
	cmaj->n_name = cdev->d_name;
	cmaj->n_str = cdev->d_str;
	cmaj->n_flag = cdev->d_flag;
	cmaj->n_modid = cdev->d_modid;
	cmaj->n_count = (atomic_t) ATOMIC_INIT(0);
	cmaj->n_sqlvl = cdev->d_sqlvl;
	cmaj->n_kmod = cdev->d_kmod;
	cmaj->n_major = major;
	cmaj->n_mode = cdev->d_mode;
	cmaj->n_minor = 0;
	cmaj->n_dev = cdev;
	if ((err = register_xinode(cdev, cmaj, major, fops)) < 0)
		goto no_xinode;
	return (err);
      no_xinode:
	kfree(cmaj);
      no_cmaj:
	unregister_strdrv(cdev);
      no_strdrv:
	return (err);
}

EXPORT_SYMBOL(register_cmajor);

int
unregister_cmajor(struct cdevsw *cdev, major_t major)
{
	int err;
	struct devnode *cmaj;

	err = -ENXIO;
	if (!(cmaj = cmaj_get(cdev, major)))
		goto error;
	if ((err = unregister_xinode(cdev, cmaj, major)) < 0)
		goto error;
	kfree(cmaj);
	if ((err = unregister_strdrv(cdev)) < 0 && err != -EBUSY)
		goto error;
	return (0);
      error:
	return (err);
}

EXPORT_SYMBOL(unregister_cmajor);

/**
 *  register_strnod: - register a minor device node
 *  @cdev: character device switch structure pointer
 *  @cmin: minor device node structure pointer
 *  @minor: minor device number
 */
int
register_strnod(struct cdevsw *cdev, struct devnode *cmin, minor_t minor)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		struct devnode *n;

		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		err = -EINVAL;
		if (!cmin || !cmin->n_name || !cmin->n_name[0])
			break;
		err = -EINVAL;
		/* ensure that the device is regsitered (as a module) */
		if (!cdev->d_list.next || list_empty(&cdev->d_list))
			break;
#if 0
		/* well, no, it doesn't have to be registered with an external character major
		   device; we can assign minor nodes internal to the specfs. */
		err = -EINVAL;
		/* ensure that the device is regsitered (as a driver) */
		if (!cdev->d_majors.next || list_empty(&cdev->d_majors))
			break;
#endif
		/* check name for another cmin */
		if (!cdev->d_minors.next)
			INIT_LIST_HEAD(&cdev->d_minors);
		if ((n = __cmin_search(cdev, cmin->n_name))) {
			err = -EPERM;
			if (n != cmin)
				break;
			err = -EBUSY;
			break;
		}
		if ((minor == (minor_t) (-1UL))) {
			/* find a free minor */
			for (minor = 0; (minor != (minor_t) (-1UL)) && __cmin_lookup(cdev, minor);
			     minor++) ;
			err = -ENXIO;
			if ((minor == (minor_t) (-1UL))) {
				break;
			}
		} else {
			/* use specified minor */
			if ((n = __cmin_lookup(cdev, minor))) {
				err = -EPERM;
				if (n != cmin)
					break;
				err = -EBUSY;
				break;
			}
		}
		if ((err = cmin_add(cmin, cdev, minor)))
			break;
		err = minor;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL(register_strnod);

/**
 *  unregister_strnod: - unregister a minor device node
 *  @cdev: character device switch structure pointer
 *  @cmin: minor device node structure pointer
 *  @minor: minor device number
 */
int
unregister_strnod(struct cdevsw *cdev, minor_t minor)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		struct devnode *cmin;

		err = -EINVAL;
		if (!cdev)
			break;
		if (minor != (minor_t) (-1UL)) {
			/* deregister one specific minor device number */
			err = -ENXIO;
			if (!(cmin = __cmin_lookup(cdev, minor)))
				break;
			cmin_del(cmin, cdev);
		} else {
			/* deregister all minor devices */
			struct list_head *pos;

			if (!cdev->d_minors.next)
				INIT_LIST_HEAD(&cdev->d_minors);
			/* deregister all minor device numbers */
			list_for_each(pos, &cdev->d_minors) {
				cmin = list_entry(pos, struct devnode, n_list);

				cmin_del(cmin, cdev);
			}
			INIT_LIST_HEAD(&cdev->d_minors);
		}
		err = 0;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL(unregister_strnod);

/*
 *  -------------------------------------------------------------------------
 *  -------------------------------------------------------------------------
 */

#ifndef HAVE_FILE_MOVE_ADDR
STATIC void
_file_move(struct file *file, struct list_head *list)
{
	if (!list)
		return;
	file_list_lock();
	list_del(&file->f_list);
	list_add(&file->f_list, list);
	file_list_unlock();
}
#endif

STATIC void
file_swap_put(struct file *f1, struct file *f2)
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

#ifndef O_CLONE
#define O_CLONE (O_CREAT|O_EXCL)
#endif

/**
 *  spec_dentry: - find a dentry in the specfs to open.
 *  @dev: device for which to find a dentry
 *  @sflagp: flags to modify (if any)
 */
struct dentry *
spec_dentry(dev_t dev, int *sflagp)
{
	struct dentry *dentry;
	struct cdevsw *cdev;

	{
		struct qstr name;
		char buf[32];

		dentry = ERR_PTR(-ENXIO);
		if (!(cdev = cdrv_get(getmajor(dev))))
			goto done;
		if (sflagp && cdev->d_flag & D_CLONE)
			*sflagp = CLONEOPEN;
		snprintf(buf, 32, "%s", cdev->d_name);
		cdrv_put(cdev);
		name.name = buf;
		name.len = strnlen(buf, 32 - 1);
		name.hash = full_name_hash(name.name, name.len);
		{
			struct vfsmount *mnt;

			dentry = ERR_PTR(-EIO);
			if (!(mnt = specfs_get()))
				goto done;
			down(&mnt->mnt_root->d_inode->i_sem);
			dentry = lookup_hash(&name, mnt->mnt_root);
			up(&mnt->mnt_root->d_inode->i_sem);
			specfs_put();
		}
	}
	if (IS_ERR(dentry))
		goto done;
	if (!dentry->d_inode)
		goto enoent;
	{
		struct qstr name;
		char buf[32];
		struct dentry *parent = dentry;

		{
			struct devnode *cmin;

			if ((cmin = cmin_get(cdev, getminor(dev)))) {
				if (sflagp && cmin->n_flag & D_CLONE)
					*sflagp = CLONEOPEN;
				snprintf(buf, 32, "%s", cmin->n_name);
			} else {
				snprintf(buf, 32, "%u", getminor(dev));
			}
			name.name = buf;
			name.len = strnlen(buf, 32 - 1);
			name.hash = full_name_hash(name.name, name.len);
		}
		down(&parent->d_inode->i_sem);
		dentry = lookup_hash(&name, parent);
		up(&parent->d_inode->i_sem);
		dput(parent);
	}
	if (IS_ERR(dentry)) {
		goto done;
	}
	if (!dentry->d_inode) {
		goto enoent;
	}
      done:
	return (dentry);
      enoent:
	dput(dentry);
	dentry = ERR_PTR(-ENOENT);
	goto done;
}

#if defined CONFIG_STREAMS_STH_MODULE || !defined CONFIG_STREAMS_STH
EXPORT_SYMBOL(spec_dentry);
#endif

/**
 *  spec_open:	- chain open to an internal special device.
 *  @i:	external (or chaining) filesystem inode
 *  @f:	external (or chaining) filesystem file pointer (user file pointer)
 *
 *  The f->f_flags has the O_CLONE flags set if CLONEOPEN was set by a previous operation.
 */
int
spec_open(struct inode *i, struct file *f, dev_t dev, int sflag)
{
	int err;
	struct file *file;		/* new file pointer to use */
	struct dentry *dentry;
	struct vfsmount *mnt;

	dentry = spec_dentry(dev, &sflag);
	err = PTR_ERR(dentry);
	if (IS_ERR(dentry)) {
		goto exit;
	}
	err = -ENODEV;
	if (!(mnt = specfs_get()))
		goto dput_exit;
	file = dentry_open(dentry, mntget(mnt), f->f_flags | ((sflag == CLONEOPEN) ? O_CLONE : 0));
	specfs_put();
	err = PTR_ERR(file);
	if (IS_ERR(file))
		goto exit;
	file_swap_put(f, file);
	err = 0;
	goto exit;
      dput_exit:
	dput(dentry);
      exit:
	return (err);
}

#if defined CONFIG_STREAMS_STH_MODULE || defined CONFIG_STREAMS_CLONE_MODULE || defined CONFIG_STREAMS_NSDEV_MODULE \
         || !defined CONFIG_STREAMS_STH || !defined CONFIG_STREAMS_CLONE || !defined CONFIG_STREAMS_NSDEV
EXPORT_SYMBOL(spec_open);
#endif
