/*****************************************************************************

 @(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2004/04/30 10:42:02 $

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

 Last Modified $Date: 2004/04/30 10:42:02 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2004/04/30 10:42:02 $"

static char const ident[] =
    "$RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2004/04/30 10:42:02 $";

#define __NO_VERSION__

#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
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
#include <sys/modversions.h>
#endif

#include <sys/kmem.h>		/* for kmem_ */
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strhead.h"		/* for stream operations */
#include "strreg.h"		/* extern verification and str_args */

#ifndef CONFIG_STREAMS_FIFO_MAJOR
#define CONFIG_STREAMS_FIFO_MAJOR 211
//#error "CONFIG_STREAMS_FIFO_MAJOR must be defined."
#endif
#ifndef CONFIG_STREAMS_SOCKSYS_MAJOR
#define CONFIG_STREAMS_SOCKSYS_MAJOR 40
//#error "CONFIG_STREAMS_SOCKSYS_MAJOR must be defined."
#endif

/* these don't need to be so big */

#define STRMOD_HASH_ORDER 5
#define STRDEV_HASH_ORDER 5

#define STRMOD_HASH_SIZE (1 << STRMOD_HASH_ORDER)
#define STRDEV_HASH_SIZE (1 << STRDEV_HASH_ORDER)

#define STRMOD_HASH_MASK ((unsigned long)(STRMOD_HASH_SIZE-1))
#define STRDEV_HASH_MASK ((unsigned long)(STRDEV_HASH_SIZE-1))

#define strmod_hash_slot(_modid) \
	(cdevsw_hash + (((_modid) + ((_modid) >> STRMOD_HASH_ORDER) + ((_modid) >> 2 * STRMOD_HASH_ORDER)) & STRMOD_HASH_MASK))

#define strdev_hash_slot(_major) \
	(fmodsw_hash + (((_major) + ((_major) >> STRDEV_HASH_ORDER) + ((_major) >> 2 * STRDEV_HASH_ORDER)) & STRDEV_HASH_MASK))

rwlock_t cdevsw_lock = RW_LOCK_UNLOCKED;
rwlock_t fmodsw_lock = RW_LOCK_UNLOCKED;
rwlock_t minors_lock = RW_LOCK_UNLOCKED;

struct list_head cdevsw_list = LIST_HEAD_INIT(cdevsw_list);	/* Devices go here */
struct list_head fmodsw_list = LIST_HEAD_INIT(fmodsw_list);	/* Modules go here */
struct list_head minors_list = LIST_HEAD_INIT(minors_list);	/* Minors go here */

//struct fmodsw *fmodsw[MAX_STRMOD] ____cacheline_aligned = { NULL, };
//struct cdevsw *cdevsw[MAX_STRDEV] ____cacheline_aligned = { NULL, };

struct list_head fmodsw_hash[STRMOD_HASH_SIZE] __cacheline_aligned = { {NULL,}, };
struct list_head cdevsw_hash[STRDEV_HASH_SIZE] __cacheline_aligned = { {NULL,}, };

STATIC struct cdevsw *cdevsw_major_cache = NULL;	/* L1 major cache */
STATIC struct fmodsw *fmodsw_modid_cache = NULL;	/* L1 modid cache */
STATIC struct cdevsw *cdevsw_mname_cache = NULL;	/* L1 mname cache */
STATIC struct fmodsw *fmodsw_mname_cache = NULL;	/* L1 mname cache */

int cdev_count = 0;
int fmod_count = 0;

/*
 *  -------------------------------------------------------------------------
 *
 *  Hash functions
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  init_cdev_hash: - initialize the list_head structures in the cdev hash
 */
STATIC void init_cdev_hash(void)
{
	int i;
	for (i = 0; i < STRDEV_HASH_SIZE; i++)
		INIT_LIST_HEAD((cdevsw_hash + i));
}

/**
 *  init_fmod_hash: - initialize the list_head structures in the fmod hash
 */
STATIC void init_fmod_hash(void)
{
	int i;
	for (i = 0; i < STRMOD_HASH_SIZE; i++)
		INIT_LIST_HEAD((fmodsw_hash + i));
}

/**
 *  __cdev_lookup: - look up a cdev by major device number in cdev hashes
 *  @major: major device number to look up
 *
 *  Look up a STREAMS driver by major device number in the cdevsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @major with
 *  very little performance impact.
 */
STATIC struct cdevsw *__cdev_lookup(major_t major)
{
	if (cdevsw_major_cache && cdevsw_major_cache->d_major == major)
		return (cdevsw_major_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, strdev_hash_slot(major)) {
			struct cdevsw *cdev;
			cdev = list_entry(pos, struct cdevsw, d_hash);
			if (cdev->d_major != major)
				continue;
			return ((cdevsw_major_cache = cdev));
		}
		return (NULL);
	}
}

/**
 *  __fmod_lookup: - look up a fmod by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *
 *  Look up a STREAMS module by module identifier in the fmodsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @modid with
 *  very little performance impact.
 */
STATIC struct fmodsw *__fmod_lookup(modID_t modid)
{
	if (fmodsw_modid_cache && fmodsw_modid_cache->f_str->st_rdinit->qi_minfo->mi_idnum == modid)
		return (fmodsw_modid_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, strmod_hash_slot(modid)) {
			struct fmodsw *fmod;
			fmod = list_entry(pos, struct fmodsw, f_hash);
			if (fmod->f_str->st_rdinit->qi_minfo->mi_idnum != modid)
				continue;
			return ((fmodsw_modid_cache = fmod));
		}
		return (NULL);
	}
}

STATIC struct cdevsw *__cdev_search(const char *name)
{
	size_t len = strnlen(name, FMNAMESZ + 1);
	if (cdevsw_mname_cache && !strncmp(cdevsw_mname_cache->d_name, name, len))
		return (cdevsw_mname_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, &cdevsw_list) {
			struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_list);
			if (strncmp(cdev->d_name, name, len))
				continue;
			return ((cdevsw_mname_cache = cdev));
		}
		return (NULL);
	}
}

STATIC struct fmodsw *__fmod_search(const char *name)
{
	size_t len = strnlen(name, FMNAMESZ + 1);
	if (fmodsw_mname_cache && !strncmp(fmodsw_mname_cache->f_name, name, len))
		return (fmodsw_mname_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, &fmodsw_list) {
			struct fmodsw *fmod = list_entry(pos, struct fmodsw, f_list);
			if (strncmp(fmod->f_name, name, len))
				continue;
			return ((fmodsw_mname_cache = fmod));
		}
		return (NULL);
	}
}

STATIC struct devnode *__node_search(const struct cdevsw *cdev, const char *name)
{
	size_t len = strnlen(name, FMNAMESZ + 1);
	struct list_head *pos;
	list_for_each(pos, &cdev->d_nodes) {
		struct devnode *node = list_entry(pos, struct devnode, n_list);
		if (strncmp(node->n_name, name, len))
			continue;
		return (node);
	}
	return (NULL);
}

/**
 *  cdev_lookup: - look up a cdev by major device number in cdev hashes
 *  @major: major device number to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct cdevsw *cdev_lookup(major_t major, int load)
{
	struct cdevsw *cdev = NULL;
	int reload;
	read_lock(&cdevsw_lock);
#ifdef CONFIG_KMOD
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char devname[64];
			if ((cdev = __cdev_lookup(major)))
				break;
			if (!load)
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "char-major-%d", major);
			request_module(devname);
			read_lock(&cdevsw_lock);
			if ((cdev = __cdev_lookup(major)))
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "streams-major-%d", major);
			request_module(devname);
			read_lock(&cdevsw_lock);
#ifdef CONFIG_DEVFS
			if ((cdev = __cdev_lookup(major)))
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "/dev/streams/%d", major);
			request_module(devname);
			read_lock(&cdevsw_lock);
#endif				/* CONFIG_DEVFS */
		} while (0);
		/* try to acquire the module */
		if (cdev && cdev->d_str && try_inc_mod_count(cdev->d_kmod))
			break;
		cdev = NULL;
	}
#else
	cdev = __cdev_lookup(major);
#endif				/* CONFIG_KMOD */
	read_unlock(&cdevsw_lock);
	return (cdev);
}

/**
 *  fmod_lookup: - look up a fmod by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct fmodsw *fmod_lookup(modID_t modid, int load)
{
	struct fmodsw *fmod = NULL;
	int reload;
	read_lock(&fmodsw_lock);
#ifdef CONFIG_KMOD
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char modname[64];
			if ((fmod = __fmod_lookup(modid)))
				break;
			if (!load)
				break;
			read_unlock(&fmodsw_lock);
			snprintf(modname, sizeof(modname), "streams-modid-%d", modid);
			request_module(modname);
			read_lock(&fmodsw_lock);
			if ((fmod = __fmod_lookup(modid)))
				break;
		} while (0);
		/* try to acquire the module */
		if (fmod && fmod->f_str && try_inc_mod_count(fmod->f_kmod))
			break;
		fmod = NULL;
	}
#else
	fmod = __fmod_lookup(modid);
#endif				/* CONFIG_KMOD */
	read_unlock(&fmodsw_lock);
	return (fmod);
}

/**
 *  cdev_search: - look up a cdev by name
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 *
 *  The search is exhaustive.  If this function is called multiple times with the same name, the
 *  peformance impact will be minimal.  If the device is not found by name, an attempt will be made
 *  to demand load the kernel module "streams-%s" and then "/dev/streams/%s".
 */
STATIC struct cdevsw *cdev_search(const char *name, int load)
{
	struct cdevsw *cdev = NULL;
	int reload;
	read_lock(&cdevsw_lock);
#ifdef CONFIG_KMOD
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char devname[64];
			if ((cdev = __cdev_search(name)))
				break;
			if (!load)
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "streams-%s", name);
			request_module(devname);
			read_lock(&cdevsw_lock);
#ifdef CONFIG_DEVFS
			if ((cdev = __cdev_search(name)))
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "/dev/streams/%s", name);
			request_module(devname);
			read_lock(&cdevsw_lock);
#endif				/* CONFIG_DEVFS */
		} while (0);
		/* try to acquire the module */
		if (cdev && cdev->d_str && try_inc_mod_count(cdev->d_kmod))
			break;
		cdev = NULL;
	}
#else				/* CONFIG_KMOD */
	cdev = __cdev_search(name);
#endif				/* CONFIG_KMOD */
	read_unlock(&cdevsw_lock);
	return (cdev);
}

/**
 *  fmod_search: - look up a fmod by name
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 *
 *  The search is exhaustive.  If this function is called multiple times with the same name, the
 *  peformance impact will be minimal.  If the module is not found by name, an attempt will be made
 *  to demand load the kernel module "streams-%s".
 */
STATIC struct fmodsw *fmod_search(const char *name, int load)
{
	struct fmodsw *fmod = NULL;
	int reload;
	read_lock(&fmodsw_lock);
#ifdef CONFIG_KMOD
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char devname[64];
			if ((fmod = __fmod_search(name)))
				break;
			if (!load)
				break;
			read_unlock(&fmodsw_lock);
			snprintf(devname, sizeof(devname), "streams-%s", name);
			request_module(devname);
			read_lock(&fmodsw_lock);
		} while (0);
		/* try to acquire the module */
		if (fmod && fmod->f_str && try_inc_mod_count(fmod->f_kmod))
			break;
		fmod = NULL;
	}
#else				/* CONFIG_KMOD */
	fmod = __fmod_search(name);
#endif				/* CONFIG_KMOD */
	read_unlock(&fmodsw_lock);
	return (fmod);
}

/**
 *  node_search: - looke up a minor device node by name
 *  @cdev: character device major structure
 *  @name: name to look up
 */
STATIC struct devnode *node_search(const struct cdevsw *cdev, const char *name)
{
	struct devnode *node = NULL;
	read_lock(&cdevsw_lock);
	do {
		if (!cdev || !cdev->d_nodes.next)
			break;
		if ((node = __node_search(cdev, name)))
			break;
	} while (0);
	read_unlock(&cdevsw_lock);
	return (node);
}

/**
 *  smod_search: - look up a cdev or fmod by name
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 */
STATIC struct fmodsw *smod_search(const char *name, int load)
{
	void *fmod = NULL;
	if (!fmod)
		fmod = cdev_search(name, load);
	if (!fmod)
		fmod = fmod_search(name, load);
	return (fmod);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Get and put devices and modules.
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  cdev_get: - get a reference to a STREAMS device
 *  @major: (internal) major device number of the STREAMS device
 *
 *  Context: When the calling context can block, an attempt will be made to load the driver by major
 *  device number.
 */
struct cdevsw *cdev_get(major_t major)
{
	return cdev_lookup(major, !in_interrupt());
}

/**
 *  cdev_put: - put a reference to a STREAMS device
 *  @sdev: STREAMS device structure pointer to put
 */
void cdev_put(struct cdevsw *sdev)
{
	if (sdev)
		__MOD_DEC_USE_COUNT(sdev->d_kmod);
}

/**
 *  fmod_get: - get a reference to a STREAMS module
 *  @modid: module id number of the STREAMS module
 *
 *  Context: When the calling context can block, an attempt will be made to load the module by
 *  module identifier.
 */
struct fmodsw *fmod_get(modID_t modid)
{
	return fmod_lookup(modid, !in_interrupt());
}

/**
 *  fmod_put: - put a reference to a STREAMS module
 *  @smod: STREAMS module structure pointer to put
 *
 */
void fmod_put(struct fmodsw *smod)
{
	if (smod)
		__MOD_DEC_USE_COUNT(smod->f_kmod);
}

/**
 *  cdev_find: - find a STREAMS device by its name
 *  @name: the name to find
 *
 *  Attempt to find a STREAMS device by name.  If the device cannot be found by name, attempt to
 *  load the kernel module streams-%s where %s is the name requested and check again.
 *
 *  Return Value: A pointer to the STREAMS device, with use count incremented, or %NULL if not found
 *  and could not be loaded.
 *
 *  Context: When the calling context can block, an attempt will be made to load the driver by name.
 */
struct cdevsw *cdev_find(const char *name)
{
	return cdev_search(name, !in_interrupt());
}

/**
 *  fmod_find: - get a reference to a STREAMS module
 *  @name: name of the module
 *
 *  Attempt to find a STREAMS module by name.  If the module cannot be found by name, attempt to
 *  load the kernel module streams-%s where %s is the name requested and check again.
 *
 *  Return Value: A pointer to the STREAMS module, with use count incremented, or %NULL if not found
 *  and could not be loaded.
 *
 *  Context: When the calling context can block, an attempt will be made to load the module by name.
 */
struct fmodsw *fmod_find(const char *name)
{
	return fmod_search(name, !in_interrupt());
}

struct devnode *node_find(const struct cdevsw *cdev, const char *name)
{
	return node_search(cdev, name);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Special open for character based streams, fifos and pipes.
 *
 *  -------------------------------------------------------------------------
 */

#if 0
/**
 *  create_hash: - create a dentry and hash for a name without lookup
 *  @name: the name to create
 *  @base: the base directory entry from which to create the new dentry
 *
 *  Like VFS lookup_hash(), but always creates a new dentry and a new inode for the dentry.  This
 *  uses VFS to stack potential multiple instances of the same inode number.
 */
STATIC struct dentry *create_hash(struct qstr *name, struct dentry *base)
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
#endif

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

void file_swap_put(struct file *f1, struct file *f2)
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

/**
 *  sdev_open: - open a stream from a character special device or a nested call
 */
int sdev_open(struct inode *i, struct file *f, struct vfsmount *mnt, struct str_args *argp)
{
	struct dentry *dentry;
	struct file *file;		/* next file pointer to use */
	struct inode *inode;		/* next inode to use */
	int err;
	struct cdevsw *cdev;
	if ((err = down_interruptible(&i->i_sem)) < 0)
		goto exit;
	if (!(cdev = cdev_get(getmajor(argp->dev))))
		goto up_exit;
	argp->name.len = snprintf(argp->buf, sizeof(argp->buf), "[%d]", getminor(argp->dev));
	argp->name.hash = (cdev->d_str->st_rdinit->qi_minfo->mi_idnum << 16) | getminor(argp->dev);
	dentry = lookup_hash(&argp->name, mnt->mnt_root);
	cdev_put(cdev);
	if ((err = PTR_ERR(dentry)) < 0)
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
	if (err == 0) {
		file_swap_put(f, file);
		goto up_exit;
	}
	/* fifo returns 1 on exit to cleanup shadow pointer and use existing file pointer */
	err = 0;
      dput_exit:
	dput(dentry);
      up_exit:
	up(&i->i_sem);
      exit:
	return (err);
}

/**
 *  strm_open: - nest another open call on a different device number
 *  @inode: the previous inode
 *  @file: the user file pointer
 *
 *  strm_open() is used to nest another ridirected call.  It is used by clone devices such as the
 *  Clone device and the Named Stream Device.
 */
int strm_open(struct inode *inode, struct file *file)
{
	return sdev_open(inode, file, file->f_vfsmnt, xchg(&file->f_dentry->d_fsdata, NULL));
}

extern struct vfsmount *specfs_mnt;

/**
 *  spec_open: - open a character special device node
 *  @inode: the character device inode
 *  @file: the user file pointer
 *
 *  spec_open() is only used to open a stream from a character device node in an external
 *  filesystem.  This is never called for direct opens of a specfs device node.  Therefore, the
 *  character device number from the inode is used to determine the shadow special file system
 *  (internal) inode and chain the open call.
 */
STATIC int spec_open(struct inode *inode, struct file *file)
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
#ifdef CONFIG_STREAMS_FIFO
	case S_IFIFO:
		args.dev = makedevice(CONFIG_STREAMS_FIFO_MAJOR, 0);
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
	if (!(cdev = cdev_get(getmajor(args.dev))))
		return (-ENXIO);
	args.sflag = (cdev->d_flag & D_CLONE) ? CLONEOPEN : DRVOPEN;
	err = sdev_open(inode, file, specfs_mnt, &args);
	cdev_put(cdev);
	return (err);
}

STATIC struct file_operations spec_f_ops ____cacheline_aligned = {
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

/**
 *  register_inode: - unregister a special device inode
 *  @major: major device number (character special devices only)
 *  @cdev: character device switch structure pointer
 *  @fops: file operations to apply to external special device nodes
 *
 *  Registers and extenal character special device major number with the Linux operating system.
 *  @fops is the file operations that will be used to open the character device.  These operations
 *  only need an open procedure as they will be replaced with the operations of the shadow special
 *  filesystem node which are contained in the @cdev structure in the d_ops member.
 *
 *  The major device number can be specified as zero (0), in which case the major device number will
 *  be assigned to a free major device number by register_chrdev() and returned as a positive return
 *  value.
 */
int register_inode(major_t major, struct cdevsw *cdev, struct file_operations *fops)
{
	int err = 0;
	write_lock(&cdevsw_lock);	/* always this order, never the other way around */
	write_lock(&fmodsw_lock);
	do {
		struct cdevsw *c;
		modID_t modid;
		struct module_info *mi;
		err = -EINVAL;
		if (major >= MAX_STRDEV || !cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		{
			struct streamtab *st;
			struct qinit *qi;
			err = -EINVAL;
			if (!(st = cdev->d_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo))
				break;
		}
		err = -EBUSY;
		if ((c = cdev_lookup(major, 0)) && c != cdev)
			break;
		if ((c = (struct cdevsw *) smod_search(cdev->d_name, 0)) && c != cdev)
			break;
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			modid = 0x7fff;
			while (modid && (c = (struct cdevsw *) fmod_lookup(modid, 0)) && c != cdev)
				modid--;
			err = -ENXIO;
			if (!modid)	/* table full */
				break;
		} else {
			/* use specified module id */
			err = -EBUSY;
			if ((c = (struct cdevsw *) fmod_lookup(modid, 0)) && c != cdev)
				break;
		}
		/* register the character device */
		if ((err = register_chrdev(major, cdev->d_name, fops)) < 0)
			break;
		if (err > 0 && major == 0)
			major = err;
		{
			if (!cdev->d_list.next)
				INIT_LIST_HEAD(&cdev->d_list);
			if (!cdev->d_mod_hash.next)
				INIT_LIST_HEAD(&cdev->d_mod_hash);
			if (!cdev->d_hash.next)
				INIT_LIST_HEAD(&cdev->d_hash);
			if (!cdev->d_nodes.next)
				INIT_LIST_HEAD(&cdev->d_nodes);
			if (!cdev->d_apush.next)
				INIT_LIST_HEAD(&cdev->d_apush);
			if (!cdev->d_stlist.next)
				INIT_LIST_HEAD(&cdev->d_stlist);
			if (!list_empty(&cdev->d_list)) {
				list_del_init(&cdev->d_list);
				cdev_count--;
			}
			list_add(&cdev->d_list, &cdevsw_list);
			cdev_count++;
			if (!list_empty(&cdev->d_mod_hash)) {
				list_del_init(&cdev->d_mod_hash);
				fmod_count--;
			}
			mi->mi_idnum = modid;
			list_add(&cdev->d_mod_hash, strmod_hash_slot(modid));
			fmod_count++;
			if (!list_empty(&cdev->d_hash))
				list_del_init(&cdev->d_hash);
			list_add(&cdev->d_hash, strdev_hash_slot(major));
		}
		err = major;
	} while (0);
	write_unlock(&fmodsw_lock);
	write_unlock(&cdevsw_lock);
	return (err);
}

/**
 *  unregister_inode: - unregister a special device inode
 *  @major: major device number (character special devices only)
 *  @cdev: character device switch structure pointer
 *
 *  Deregisters an external character device major number previously registered with
 *  register_inode().
 *
 *  Notices: The major device number must be the major device number returned from a successful
 *  register_inode() call.  cdev->d_name must have the same value as on the call to register_inode()
 *  or the call will fail.
 */
int unregister_inode(major_t major, struct cdevsw *cdev)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	write_lock(&fmodsw_lock);
	do {
		err = -EINVAL;
		if (major >= MAX_STRDEV || !cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		err = -ENXIO;
		if (!cdev->d_list.next || list_empty(&cdev->d_list))
			break;
		err = -EPERM;
		if (major && major != cdev->d_major)
			break;
		if ((err = unregister_chrdev(major, cdev->d_name)) < 0)
			break;
		major = cdev->d_major;
		{
			list_del_init(&cdev->d_list);
			cdev_count--;
			list_del_init(&cdev->d_mod_hash);
			fmod_count--;
			list_del_init(&cdev->d_hash);
		}
		err = major;
	} while (0);
	write_unlock(&fmodsw_lock);
	write_unlock(&cdevsw_lock);
	return (err);
}

/**
 *  register_strdev: - register a STREAMS device against a device major number
 *  @major: requested major device number or 0 for automatic major selection
 *  @cdev: STREAMS character device structure to register
 *
 *  register_strdev() registers the device specified by the @cdev to the device major number
 *  specified by @major.
 *
 *  register_strdev() will register the STREAMS character device specified by @cdev against the
 *  major device number @major.  If the major device number is zero, then it requests that
 *  register_strdev() allocate an available major device number and assign it to @cdev.
 *
 *  Context: register_strdev() is intended to be called from kernel __init() or module_init()
 *  routines only.  It cannot be called from in_irq() level.
 *
 *  Return Values: Upon success, register_strdev() will return the requested or assigned major
 *  device number as a positive integer value.  Upon failure, the registration is denied and a
 *  negative error number is returned.
 *
 *  Errors: Upon failure, register_strdev() returns on of the negative error numbers listed below.
 *
 *  -[%ENOMEM]	insufficient memory was available to complete the request.
 *
 *  -[%EINVAL]	@cdev was NULL
 *
 *  -[%EBUSY]	a device was already registered against the requested major device number, or no
 *	        device numbers were available for automatic major device number assignment.
 *
 *  Notes: Linux Fast-STREAMS provides improvements over LiS.
 *
 *  LfS uses a small hash instead of a cdevsw[] table and requires that the driver (statically)
 *  allocate its &struct cdevsw structure using an approach more likened to the Solaris &struct
 *  cb_ops.
 */
int register_strdev(major_t major, struct cdevsw *cdev)
{
	if (!cdev)
		return (-EINVAL);
	cdev->d_fop = &strm_f_ops;
	cdev->d_mode = (cdev->d_mode & S_IFMT) | S_IFCHR;
	return register_inode(major, cdev, &spec_f_ops);
}

/**
 *  unregister_strdev: - unregister previously registered STREAMS device
 *  @major: major device number to unregister or 0 for all majors
 *  @cdev: STREAMS character device structure to unregister
 *
 *  unregister_strdev() unregisters the device specified by the @cdev from the device major number
 *  specified by @dev.  Only the getmajor(@dev) component of @dev is significant and the
 *  getminor(@dev) component must be coded zero (0).
 *
 *  unregister_strdev() will unregister the STREAMS character device specified by @cdev from the
 *  major device number in getmajor(@dev).  If the major device number is zero, then it requests
 *  that unregister_strdev() unregister @cdev from any device majors with which it is currently
 *  registered.
 *
 *  Context: unregister_strdev() is intended to be called from kernel __exit() or module_exit()
 *  routines only.  It cannot be called from in_irq() level.
 *
 *  Return Values: Upon success, unregister_strdev() will return zero (0).  Upon failure, the
 *  deregistration is denied and a negative error number is returned.
 *
 *  Errors: Upon failure, unregister_strdev() returns one of the negative error numbers listed
 *  below.
 *
 *  -[%ENXIO]	The specified device does not exist in the registration tables.
 *
 *  -[%EINVAL]	@cdev is NULL, or the @d_name component associated with @cdev has changed since
 *              registration.
 *
 *  -[%EPERM]	The device number specified does not belong to the &struct cdev structure specified
 *		and permission is therefore denied.
 */
int unregister_strdev(major_t major, struct cdevsw *cdev)
{
	return unregister_inode(major, cdev);
}

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
		struct fmodsw *f = NULL;
		struct module_info *mi;
		err = -EINVAL;
		if (!fmod || !fmod->f_name || !fmod->f_name[0])
			break;
		{
			struct streamtab *st;
			struct qinit *qi;
			err = -EINVAL;
			if (!(st = fmod->f_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo))
				break;
		}
		err = -EBUSY;
		if ((f = smod_search(fmod->f_name, 0)) && f != fmod)
			break;
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			modid = 0xffff;
			while (modid && (f = fmod_lookup(modid, 0)) && f != fmod)
				modid--;
			err = -ENXIO;
			if (!modid)	/* table full */
				break;
			mi->mi_idnum = modid;
		} else {
			/* use specified module id */
			err = -EBUSY;
			if ((f = fmod_lookup(modid, 0)) && f != fmod)
				break;
		}
		{
			if (!list_empty(&fmod->f_list)) {
				list_del_init(&fmod->f_list);
				list_del_init(&fmod->f_hash);
				fmod_count--;
			}
			list_add(&fmod->f_list, &fmodsw_list);
			list_add(&fmod->f_hash, strmod_hash_slot(modid));
			fmod_count++;
		}
		err = modid;
	} while (0);
	write_unlock(&fmodsw_lock);
	return (err);
}

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
		if (!fmod || !fmod->f_name || !fmod->f_name[0])
			break;
		err = -ENXIO;
		if (!fmod->f_list.next || list_empty(&fmod->f_list))
			break;
		err = -EPERM;
		{
			if (!list_empty(&fmod->f_list)) {
				list_del_init(&fmod->f_list);
				list_del_init(&fmod->f_hash);
				fmod_count--;
			}
		}
	} while (0);
	write_unlock(&fmodsw_lock);
	return (err);
}

/**
 *  getmid: - get the module identifier associated with a module name
 *  @name: the name of the module
 *
 *  Obtains the module id of the named module.
 *
 *  Return Value: Returns the module identifier associated with the named module or zero (0) if no
 *  module of the specified name can be found on the system.
 *
 *  Context: Can be called from any context.  When called from a blocking context, the function has
 *  the side-effect that the named module may be loaded by module name.  The kernel module demand
 *  loaded will have the module name or alias "streams-%s".
 */
modID_t getmid(const char *name)
{
	modID_t modid = 0;
	struct fmodsw *fmod;
	if ((fmod = smod_search(name, !in_interrupt()))) {
		struct streamtab *st;
		struct qinit *qi;
		struct module_info *mi;
		if ((st = fmod->f_str) && (qi = st->st_rdinit) && (mi = qi->qi_minfo))
			modid = mi->mi_idnum;
		fmod_put(fmod);
	}
	return (modid);
}

/**
 *  getadmin: - get the administrative function associated with a module identifier
 *  @modid: the module identifier
 *
 *  Obtains the qi_qadmin function pointer for the module identifier by the module identifier
 *  @modid.
 *
 *  Return Value: Returns a function pointer to the qi_qadmin() procedure for the module, which may be
 *  %NULL, or returns %NULL on failure.
 *
 *  Context: Can be called from any context.  When called from a blocking context, the function has
 *  the side-effect that the identified module may be loaded by module identifier.  The kernel
 *  module demand loaded will have the module name or alias "streams-modid-%u".
 */
qi_qadmin_t getadmin(modID_t modid)
{
	qi_qadmin_t qadmin = NULL;
	struct fmodsw *fmod;
	if ((fmod = fmod_lookup(modid, !in_interrupt()))) {
		struct streamtab *st;
		struct qinit *qi;
		if ((st = fmod->f_str) && (qi = st->st_rdinit))
			qadmin = qi->qi_qadmin;
		fmod_put(fmod);
	}
	return (qadmin);
}

int strreg_init(void)
{
	init_cdev_hash();
	init_fmod_hash();
	return (0);
}
void strreg_exit(void)
{
	return;
}
