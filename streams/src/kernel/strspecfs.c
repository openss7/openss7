/*****************************************************************************

 @(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2004/05/29 08:28:17 $

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

 Last Modified $Date: 2004/05/29 08:28:17 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2004/05/29 08:28:17 $"

static char const ident[] =
    "$RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2004/05/29 08:28:17 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#include <linux/compiler.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/locks.h>
#include <linux/delay.h>
#include <linux/sysctl.h>
#include <linux/file.h>
#include <linux/poll.h>
#include <linux/fs.h>
#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#endif
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif
#include <linux/major.h>
#include <asm/atomic.h>
#include <linux/kernel.h>	/* for simple_strtoul */

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>
#include <sys/kmem.h>

#include "sys/config.h"
#include "strdebug.h"
#include "sth.h"
#include "fifo.h"
#include "pipe.h"
#include "sock.h"
#include "strsched.h"
#include "strreg.h"		/* for cdevsw_list */
#include "strspecfs.h"		/* for str_args */

/* 
 *  -------------------------------------------------------------------------
 *
 *  SPEC (Special Device Shadow Filesystem) Ala SVR4.
 *
 *  -------------------------------------------------------------------------
 */

/*
    --------------------------------------------------------------------------------------------------------
                                                                                                             
    Structure layout:                                                                                        
                                                                                                             
      cdevsw              struct cdevsw          struct devnode                                              
    __________               __________                               struct stdata   struct stdata          
   |          |------------>|          |            __________         __________      __________            
   |__________|     ,------>|          |           |          |------>|          |--->|          |           
   |          |     |,----->|          |---------->| non-clone|<--.   |          |    |          |           
   |__________|     ||      |          |       ,---|__________|<-.|   |          |    |          |           
   |          |     ||      |__________|       |    __________   ||   |__________|    |__________|           
   |__________|     ||          |  ^           '-->|          |  ||        ^                ^                
   |          |_____||  d_inode |  |               |  clone   |  ||        |                |                
   |__________|      |          |  |           ,---|__________|  ||        |                |                
   |          |      |          |  |           |    __________   ||        |                |                
   |__________|      |          |  |           '-->|          |  ||        |                |                
   |          |      |          |  |               |          |  ||        |                |                
   |__________|      |          |  |           ,---|__________|  ||        |                |                
   |          |______|          |  |           |                 ||        |                |                
   |__________|                 |  |           '-->              ||        |                |                
   |          |                 |  |                             ||        |                |                
   |__________|                 |  |                             ||        |                |                
   |          |                 |  |                       ______||        |                |                
   |__________|                 |  |                      |       |        |                |                
   |          |                 |  |                      | ______|        |                |                
   |__________|                 |  | u.generic_ip         || u.generic_ip  |                |                
                                |  |                      ||               |                |                
                             ___v__|___             ______||__             |                |                
                    +------>|          |<-.        |          |<--. i_pipe |                |                
                    |  ,----|__________|  |        |__________|---|--------'                |                
     sb inode       |  |     __________   |         ______|___    |                         |                
    __________      |  +--->|          |<-|-.      |          |<--|-.      i_pipe           |                
   |          |_____|  ,----|__________|  | |      |__________|---|-|-----------------------'                
   |__________|        +--->|          |  | |      |          |<--|-|-.                                      
                       ,----|__________|  | |      |__________|   | | |                                      
                       |     __________   | |       __________    | | |       struct stdata                  
		       +--->|          |  | |      |          |   | | |        __________                    
		       ,----|__________|  | |      |__________|---|-|-|------>|          |                   
                       |                  | |                     | | | i_pipe|          |                   
                       +---> dir inodes   | |       dev inodes    | | |       |          |                   
                                          | |                     | | |       |__________|                   
                                          | |                     | | |                                      
                             __________   | |       __________    | | |                                      
                     ,----->|          |__| |  /-->|          |___| | |                                      
                     |      |__________|----|-/.---|__________|d_inode|                                      
                     |       __________     |  |    __________      | |                                      
    __________       |----->|          |____|  '-->|          |_____| |                                      
   |          |______|      |__________|       .---|__________|d_inode|                                      
   |__________|      |       __________        '-->|          |-------'                                      
                     |----->|          |       .---|__________|d_inode                                       
    root dentry      |      |__________|       |                                                             
                     |       __________        '-->                                                          
                     '----->|          |       .---                                                          
                            |__________|       |                                                             
                             dir dentry        '--> dev dentry                                               
                                                                                                             
                                                                                                             
    Notes:
                                                                                                             
    - all devices accessed using external device numbers use the cdevsw hash table on the major
      device number, index the cdevsw entry for the major device number.  The internal major device
      number can be calculated using the external major device number and the cdevsw entry.  The
      internal minor device number can also be calculated using the external major and external
      minor.  The minor inode can be found using an iget4() lookup by inode number calculated from
      the internal major and minor device numbers.  The i_pipe pointer for the inode can be checked
      to see if an stdata structure already exists.  If there is a devnode structure associated with
      the inode (u.generic_ip pointer) then minor device specifications and and autopush lists can
      be determined from the devnode structure.  The superblock inode_create operation will create a
      new inode and associate it with a devnode if one exists for the minor device instance.
                                                                                                             
    - if Linux does not find a character device entry for the major device number, it will load the
      "/dev/%s" (with devfs) or "char-major-%d" (without devfs) device which will either establish a
      cdevsw table entry or fail.
                                                                                                             
    - all devices access using the specfs directory hierarchy will enter at the root dentry.
                                                                                                             
    - If the subdirectory dentry equivalent to the STREAMS module name does not exist, i_lookup on
      the superblock inode will attempt to load "/dev/streams/%s" and then "streams-%s" with the
      subdirectory (driver/module) name.   When the kernel module loads and registers the cdevsw
      table entry will be created and the next lookup operation should succeed.  i_lookup uses the
      dentry name to find the cdevsw or fmodsw table entry of the same name and creates an inode
      for it if one does not already exist.  The inode number of the directory inode is the module
      id.  A cdevsw table entry can be found by moduile id with an iget4() using the module id as
      the inode number.
                                                                                                             
    - If the device dentry equivalent to the device node name does not exist, i_lookup on the module
      inode will attempt to load "/dev/stream/%s/%s" and then "streams-%s-%s" with the module and
      device instance names.  When the kernel module loads and registers the device name, the
      devnode entry will be created and the next lookup operation should succeed.  i_lookup uses the
      dentry name to find the devnode entry of the same name and creates an inode for it if one
      does not already exist.  The inode number of the device inode is the module id and the device
      instance number concatenated.  A device inode and corresponding devnode can be found with an
      iget4() using the module id and instance number as the inode number.
                                                                                                             
    - If the device dentry name is numeric and no other device node can be found, then the number
      will be used as an instance number and the inode acquired using the concatentation of the
      module id and the instance number using iget4().  The resulting inode will be attached to the
      dentry.  Device inodes can exist briefly without any devnode structure or dentry.  These
      device inodes are created in response to clone opens when new unique instance numbers are
      assigned that have no associated dentry or devnode.  Because a dentry is ultimately needed for
      the shadow open, to replace the original non-shadow entry, one must be created if it does not
      exist.
                                                                                                             
    --------------------------------------------------------------------------------------------------------
 */

#define SPEC_SBI_MAGIC 0XFEEDDEAF
struct spec_sb_info {
	u32 sbi_magic;
	int sbi_setuid;
	int sbi_setgid;
	uid_t sbi_uid;
	gid_t sbi_gid;
	umode_t sbi_mode;
};

/* 
 *  =========================================================================
 *
 *  Shadow Special Filesystem Device nodes
 *
 *  =========================================================================
 */
/*
 *  Shadow Special Filesystem Device node file operations.
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

/*
 *  dentry_open2: - simplified form of dentry_open()
 *  @dentry:	    the dentry to open
 *  @mnt:	    the vfsmount to open
 *  @cdev:	    the STREAMS device entry
 *  @argp:	    pointer to arguments
 */
STATIC struct file *dentry_open2(struct dentry *dentry, struct vfsmount *mnt, int flags,
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
	parent = lookup_hash(&argp->name, specfs_mnt->mnt_root);
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
	file = dentry_open2(dentry, specfs_mnt, ext_file->f_flags, cdev, argp);
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

/**
 *  spec_dev_open: - open a stream from an internal character special device, fifo or socket
 *  @inode:	internal shadown special filesystem inode
 *  @file:	file pointer (user file pointer)
 *
 *  This open procedure is only called from spec_dev_f_ops directly attached to a shadow special
 *  filesystem device inode.  Entering spec_dev_open() means that a direct open of the internal
 *  shadow special filesystem has been performed on the mounted filesystem.  We need to establish
 *  our streams arguments and nest into the devices open procedure.
 */
STATIC int spec_dev_open(struct inode *inode, struct file *file)
{
	struct str_args args;
	struct cdevsw *cdev;
	if (!(cdev = file->f_dentry->d_parent->d_inode->u.generic_ip))
		return (-ENOENT);
	args.inode = inode;
	args.file = file;
	args.dev = inode->i_ino;
	args.name.name = file->f_dentry->d_name.name;
	args.name.len = file->f_dentry->d_name.len;
	args.name.hash = file->f_dentry->d_name.hash;
	args.oflag = make_oflag(file);
	args.sflag = (cdev->d_flag & D_CLONE) ? CLONEOPEN : DRVOPEN;
	args.crp = current_creds;
	fops_put(xchg(&file->f_op, fops_get(cdev->d_fop)));
	cdev_put(cdev);
	if (!file->f_op || !file->f_op->open)
		return (-EIO);
	file->private_data = &args;
	return file->f_op->open(inode, file);
}

STATIC struct file_operations spec_dev_f_ops = {
	owner:THIS_MODULE,
	open:&spec_dev_open,
};

/* 
 *  =========================================================================
 *
 *  Shadow Special Filesystem Module directory
 *
 *  =========================================================================
 */
/*
 *  Shadow Special Filesystem Module directory inode operations.
 *  -------------------------------------------------------------------------
 */
/**
 *  spec_dir_i_lookup:	- lookup a device in a driver/module subdirectory
 *  @dir:		inode of the driver subdirectory
 *  @new:		directory entry for which to search for a device inode
 *
 *  In the shadow special filesystem, each directory corresponds to a registered STREAMS device.
 *  The inode numbers of these directories are made up of '0' as the major device number and the
 *  first registered major device number as the minor device number.  Whenever a STREAMS device is
 *  registered, it appears under the root directory.  We hang the cdevsw table entry off of the
 *  u.generic_ip pointer (but we could actually put it right inside the inode).
 *
 *  if someone goes to look up a name in a specific device directory and the name does not exist, we
 *  try to load a module with the name streams-%s-%s.o where the first string is the major device
 *  name and the second string is the minor device name.  We also do a similar thing on the root
 *  directory for autoloading devices.  Then we just repeat the lookup and echo back whatever
 *  appeared (or didn't).
 */
static struct dentry *spec_dir_i_lookup(struct inode *dir, struct dentry *new)
{
	struct inode *inode;
	struct cdevsw *cdev;
	struct devnode *node = NULL;
	const char *name = new->d_name.name;
	ptrace(("SPECFS: lookup dentry %p, %s in directory %p\n", new, new->d_name.name, dir));
	if ((cdev = dir->u.generic_ip)) {
		major_t major = cdev->d_str->st_rdinit->qi_minfo->mi_idnum;
		/* if the name length is zero, the hash *is* the inode number */
		if (new->d_name.len == 0) {
			minor_t minor = getminor(new->d_name.hash);
			if ((inode = iget(dir->i_sb, makedevice(major, minor))))
				goto done;
			goto enomem;
		}
		/* check if the name is a valid number */
		if (*name != '\0') {
			char *tail = (char *) name;
			minor_t minor = simple_strtoul(name, &tail, 0);
			if (*tail == '\0') {
				if ((inode = iget(dir->i_sb, makedevice(major, minor))))
					goto done;
				goto enomem;
			}
		}
		/* check if the name is registered as a minor device node name */
		if ((node = node_find(cdev, name))) {
			minor_t minor = node->n_minor;
			if ((inode = node->n_inode) && (inode = igrab(inode)))
				goto done;
			if ((inode = iget(dir->i_sb, makedevice(major, minor))))
				goto done;
			goto enomem;
		}
	}
	goto enoent;
      done:
	d_add(new, inode);
#if 0
	if (node)
		node_put(node);
#endif
	return (NULL);
      enomem:
#if 0
	if (node)
		node_put(node);
#endif
	return ERR_PTR(-ENOMEM);
      enoent:
	return ERR_PTR(-ENOENT);
}

 /* TODO: we can make links in this directory.  We just have to instantiate a devnode minor device
    node structure and attach it to the cdevsw structure and assign it the appropriate minor device 
    number.  That would be quite simple. */

static struct inode_operations spec_dir_i_ops = {
	lookup:spec_dir_i_lookup,
};

/* 
 *  Shadow Special Filesystem Module directory file operations.
 *  -------------------------------------------------------------------------
 */
/**
 *  spec_dir_readdir:	- read a driver/module subdirectory
 *  @file:		user file pointer
 *  @dirent:		pointer to user directory entry array
 *  @filldir:		procedure for filling the directory entries
 *
 *  Lists a driver subdirectory.  The driver subdirectory contains registered minor device nodes and
 *  active (open) devices.  spec_dir_readdir() walks the allocated device node list and then the
 *  active stream head list associated with the driver.
 */
static int spec_dir_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = file->f_dentry;
	struct inode *inode = dentry->d_inode;
	struct list_head *pos, *tmp;
	struct cdevsw *cdev = inode->u.generic_ip;
	int nr = file->f_pos, start = nr, err;
	ptrace(("SPECFS: listing device directory\n"));
	if (nr < 0)
		return (-EINVAL);
	switch (nr) {
	case 0:
		err = filldir(dirent, ".", 1, nr, inode->i_ino, DT_DIR);
		if (err == -EINVAL)
			break;
		if (err < 0)
			return (err);
		file->f_pos = ++nr;
		/* fall through */
	case 1:
		err = filldir(dirent, "..", 2, nr, dentry->d_parent->d_inode->i_ino, DT_DIR);
		if (err == -EINVAL)
			break;
		if (err < 0)
			return (err);
		file->f_pos = ++nr;
		/* fall through */
	default:
	{
		/* walk the allocated minor device node list */
		list_for_each_safe(pos, tmp, &cdev->d_nodes) {
			struct devnode *node = list_entry(pos, struct devnode, n_list);
			major_t major = cdev->d_str->st_rdinit->qi_minfo->mi_idnum;
			minor_t minor = node->n_minor;
			if (!node->n_inode && !(node->n_inode = iget(inode->i_sb, makedevice(major, minor))))
				continue;
			err = filldir(dirent, node->n_name, strnlen(node->n_name, FMNAMESZ),
				      file->f_pos, node->n_inode->i_ino, node->n_inode->i_mode >> 12);
			if (err == -EINVAL)
				break;
			if (err < 0)
				return (err);
			file->f_pos = ++nr;
		}
#if 0
		/* this would be a fairly big security breach */
		/* walk the active stream head list */
		list_for_each_safe(pos, tmp, &cdev->d_stlist) {
			struct stdata *sd = list_entry(pos, struct stdata, sd_list);
			struct file *file;
			if (!(file = sd->sd_file) || !(dentry = file->f_dentry)
			    || !(inode = dentry->d_inode))
				continue;
			err = filldir(dirent, dentry->d_name.name, dentry->d_name.len, file->f_pos,
				      inode->i_ino, inode->i_mode >> 12);
			if (err == -EINVAL)
				break;
			if (err < 0)
				return (err);
			file->f_pos = ++nr;
		}
#endif
		break;
	}
	}
	return (file->f_pos - start);
}

static struct file_operations spec_dir_f_ops = {
	owner:THIS_MODULE,
	read:generic_read_dir,
	readdir:spec_dir_readdir,
};

/* 
 *  =========================================================================
 *
 *  Shadow Special Filesystem ROOT
 *
 *  =========================================================================
 */
/*
 *  Shadow Special Filesystem Root inode operations.
 *  -------------------------------------------------------------------------
 */
/**
 *  spec_root_i_lookup:	- look up a subdirectory module inode
 *  @dir:		inode for the root directory
 *  @dentry:		directory entry to look up
 *
 *  The lookup() method is called on directory inodes (in our case the root) when a cached_lookup()
 *  of the dentry fails on the name.  We are given a newly created dentry, "new", that has no inode.
 *  We lookup the inode by name from the module switch table and then look up the inode by module id
 *  from the device/module switch table entry from the super block.  The iget() called here will
 *  check the inode hash an either find an existing inode, or it invokes the superblock
 *  spec_read_inode() operation, below, that will create a new inode.  We only fail to get an inode
 *  when the inode allocate fails -%ENOMEM.
 *
 *  When we get here we either do not have an inode associated with the dentry name, or there is
 *  simply no dentry in the dentry cache for this inode yet.  We walk through the cdevsw and fmodsw
 *  lists looking for the dentry name.  If we find one, we lookup or create a inode with the inode
 *  number equal to the module id and return that inode for attachment to the dentry.  If the name
 *  cannot be found and the name is numeric, we check again with the module id equal to the numeric
 *  name.  If we cannot find a corresponding cdevsw or fmodsw entry and we are configured for kernel
 *  module loading, we request a module of the name streams-%s, where %s is the name requested.
 */
static struct dentry *spec_root_i_lookup(struct inode *dir, struct dentry *new)
{
	struct inode *inode;
	struct cdevsw *cdev = NULL;
	const char *name = new->d_name.name;
	ptrace(("SPECFS: lookup dentry %p, %s in root dir inode %p\n", new, new->d_name.name, dir));
	/* if the name length is zero, the hash *is* the inode number */
	if (new->d_name.len == 0) {
		ulong modid = new->d_name.hash;
		printd(("SPECFS: name length is zero\n"));
		if ((inode = iget(dir->i_sb, modid)))
			goto done;
		goto enomem;
	}
	/* check if the name is a valid number */
	if (*name != '\0') {
		char *tail = (char *) name;
		ulong modid = simple_strtoul(name, &tail, 0);
		if (*tail == '\0') {
			printd(("SPECFS: name is a number\n"));
			if ((inode = iget(dir->i_sb, modid)))
				goto done;
			goto enomem;
		}
	}
	/* this will also attempt to demand load the "streams-%s" module if required */
	if ((cdev = cdev_find(name))) {
		ulong modid = cdev->d_str->st_rdinit->qi_minfo->mi_idnum;
		printd(("SPECFS: name is a device or module\n"));
		if ((inode = cdev->d_inode) && (inode = igrab(inode)))
			goto done;
		if ((inode = iget(dir->i_sb, modid)))
			goto done;
		goto enomem;
	}
	goto enoent;
      done:
	d_add(new, inode);
	if (cdev)
		cdev_put(cdev);
	return (NULL);
      enomem:
	if (cdev)
		cdev_put(cdev);
	return ERR_PTR(-ENOMEM);
      enoent:
	return ERR_PTR(-ENOENT);
}

static struct inode_operations spec_root_i_ops = {
	lookup:spec_root_i_lookup,
};

/* 
 *  Shadow Special Filesystem Root file operations.
 *  -------------------------------------------------------------------------
 */
/**
 *  spec_root_readdir:	- read the root directory
 *  @file:		user file pointer
 *  @dirent:		pointer to user directory entry array
 *  @filldir:		procedure for filling the directory entries
 *
 *  Lists the root directory.  The root directory contains subdirectories for each device in the
 *  cdevsw hash.  It walks the cdevsw_list and fills a dirent structure for each item on the list.
 *  Each cdevsw entry that has an associated directory inode is listed.
 */
static int spec_root_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = file->f_dentry;
	struct inode *inode = dentry->d_inode;
	struct list_head *pos, *tmp;
	int nr = file->f_pos, start = nr, err;
	ptrace(("SPECFS: listing root directory\n"));
	if (nr < 0)
		return (-EINVAL);
	switch (nr) {
	case 0:
		err = filldir(dirent, ".", 1, nr, inode->i_ino, DT_DIR);
		if (err == -EINVAL)
			break;
		if (err < 0)
			return (err);
		file->f_pos = ++nr;
		/* fall through */
	case 1:
		err = filldir(dirent, "..", 2, nr, dentry->d_parent->d_inode->i_ino, DT_DIR);
		if (err == -EINVAL)
			break;
		if (err < 0)
			return (err);
		file->f_pos = ++nr;
		/* fall through */
	default:
	{
		list_for_each_safe(pos, tmp, &cdevsw_list) {
			struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_list);
			modID_t modid = cdev->d_str->st_rdinit->qi_minfo->mi_idnum;
			if (!cdev->d_inode && !(cdev->d_inode = iget(inode->i_sb, modid)))
				continue;
			err = filldir(dirent, cdev->d_name, strnlen(cdev->d_name, FMNAMESZ),
				      file->f_pos, cdev->d_inode->i_ino, cdev->d_inode->i_mode >> 12);
			if (err == -EINVAL)
				break;
			if (err < 0)
				return (err);
			file->f_pos = ++nr;
		}
		break;
	}
	}
	return (file->f_pos - start);
}

struct file_operations spec_root_f_ops = {
	owner:THIS_MODULE,
	read:generic_read_dir,
	readdir:spec_root_readdir,
};

/* 
 *  Shadow Special Filesystem Root directory entry operations.
 *  -------------------------------------------------------------------------
 */
#if 0
/**
 *  spec_root_d_revalidate: - revalidate a directory entry
 *  @dentry:	directory entry
 *  @flags:	flags
 *
 *  Revalidate the directory entry.  Always occurs on a hashed lookup if we have this method.
 */
static int spec_root_d_revalidate(struct dentry *dentry, int flags)
{
	struct inode *inode;
	ptrace(("SPECFS: revalidating dentry %p\n", dentry));
	if (!(inode = dentry->d_inode))
		goto invalid;	/* we need an inode */
#if 0
	/* directories don't need to be marked special */
	if (inode->i_cdev == NULL)
		goto invalid;	/* it must be marked special */
#endif
	return (1);
      invalid:
	return (0);
}
#endif

/**
 *  spec_root_d_hash: - hash a directory entry name
 *  @dentry:	directory entry
 *  @dname:	name to hash
 *
 *  The d_hash() method is called to hash the directory name for directory lookups.  We do not have
 *  an inode yet.  In the specfs, when we are opening a dentry internally, the dentry is unnamed
 *  (has a name length of zero) but has a hash number equal to the device number.  The inode number
 *  is in turn formulated from the file mode and device number.  We use the inode number itself as
 *  the hash.  So, if the name is zero length, we called internally, and the hash is already valid.
 *  If the name length is non-zero, then this is an external lookup from the mounted specfs, and we
 *  need to look through the instantiated minor device list to find the name and map back to the
 *  device number. We can return an error number (-ENOENT) here if we don't like the name.
 */
static int spec_root_d_hash(struct dentry *dentry, struct qstr *dname)
{
	ptrace(("SPECFS: hashing dentry %p\n", dentry));
	/* Note that this is also called directly from lookup_hash() with a zero length name and
	   the hash value already set to the inode number.  So, if the dentry name length is zero,
	   we just leave the hash alone. */
	if (dname->len)
		dname->hash = full_name_hash(dname->name, dname->len);
	return (0);		/* success */
}

/**
 *  spec_root_d_compare: - compare two dentries
 *  @parent:	directory entry of parent
 *  @child:	child quick string
 *  @name:	comparison quick string
 *
 *  The d_compare() method is called to compare names.  We ignore the name on lookups.  The reason
 *  for this is that we have our own hash above and we really don't care what name is used as long
 *  as the hash values match. The hash is formulated from the inode device number and is the inode
 *  number anyway.  Therefore the hash itself uniquely identifies the inode
 */
static int spec_root_d_compare(struct dentry *parent, struct qstr *child, struct qstr *name)
{
	ptrace(("SPECFS: comparing dentry %p\n", parent));
	/* Note that we also call lookup_hash() directly sometimes with a zero length name. */
	return (child->hash != name->hash);	/* ignore name on lookups */
}

/**
 *  spec_root_d_delete: - delete a directory entry
 *  @dentry:	directory entry
 *
 *  The d_delete() method is called when the dentry use count drops to zero, before performing any
 *  other functions. Typically the system would just marks the dentry unusable if it is unhashed (as
 *  though we returned (0) to this function). When the dentry is deleted on the specfs, we can
 *  deallocate it altogether, so we return (1) here. That's what sockfs, pipefs and other hidden
 *  filesystems do.
 */
static int spec_root_d_delete(struct dentry *dentry)
{
	ptrace(("SPECFS: deleting dentry %p\n", dentry));
	return (1);
}

#if 0
/**
 *  spec_root_d_iput: - put a directory entry
 *  @dentry:	directory entry
 *  @inode:	inode to put
 *
 *  The d_iput() method is called to put the inode.  The system normally just calls iput() but calls
 *  this method instead.  We reference the stream head data structure from the inode, but we will
 *  get another kick at the can with the superblock put_inode() and delete_inode() operations that
 *  are called by iput(), see spec_delete_inode().
 */
static void spec_root_d_iput(struct dentry *dentry, struct inode *inode)
{
	ptrace(("SPECFS: putting inode %p, for dentry %p\n", inode, dentry));
	iput(inode);		/* this is what the system does otherwise */
}
#endif

/**
 *  spec_root_d_release: - release a directory entry
 *  @dentry:	directory entry
 *
 *  The d_release() method is called just before deleting the dentry altogether.  If we are
 *  referencing anything with the d_fsdata pointer, now is the time to remove the reference.  We
 *  actually point directory entries at stream head data structures, so we need to dereference that
 *  now.
 */
static void spec_root_d_release(struct dentry *dentry)
{
	ptrace(("SPECFS: releasing dentry %p\n", dentry));
	return;
}

struct dentry_operations spec_root_d_ops ____cacheline_aligned = {
#if 0
	d_revalidate:spec_root_d_revalidate,
#endif
	d_hash:spec_root_d_hash,
	d_compare:spec_root_d_compare,
	d_delete:spec_root_d_delete,
	d_release:spec_root_d_release,
#if 0
	d_iput:spec_root_d_iput,
#endif
};

/* 
 *  =========================================================================
 *
 *  Shadow Special Filesystem super block.
 *
 *  =========================================================================
 */
/* 
 *  Shadow Special Filesystem super block.
 *  -------------------------------------------------------------------------
 */
struct spec_sb_info *spec_sbi_alloc(void)
{
	struct spec_sb_info *sbi;
	if ((sbi = kmem_zalloc(sizeof(*sbi), KM_NOSLEEP)))
		sbi->sbi_magic = SPEC_SBI_MAGIC;
	ptrace(("SPECFS: allocated options %p\n", sbi));
	return (sbi);
}

void spec_sbi_free(struct spec_sb_info *sbi)
{
	ptrace(("SPECFS: freeing options %p\n", sbi));
	sbi->sbi_magic = 0;
	kmem_free(sbi, sizeof(*sbi));
}

/**
 *  spec_parse_options: - parse shadow special filesystem options
 *  @options:	character string of options
 *  @sbi:	pointer to a superblock information structure
 *
 *  Parses the character string options and places the parsed values into the supplied shadow
 *  special filesystem superblock information structure.  If option processing fails, reasonable
 *  values will be assigned to all super block options.
 */
static int spec_parse_options(char *options, struct spec_sb_info *sbi)
{
	int err = 0;
	int setuid = 0;
	int setgid = 0;
	uid_t uid = 0;
	uid_t gid = 0;
	umode_t mode = 0600;
	char *this_char, *value;
	ptrace(("SPECFS: paring options %p\n", sbi));
	this_char = NULL;
	if (!sbi || sbi->sbi_magic != SPEC_SBI_MAGIC)
		goto einval;
	if (!options)
		goto done_options;
	for (this_char = strtok(options, ","); this_char; this_char = strtok(NULL, ",")) {
		if ((value = strchr(this_char, '=')) != NULL)
			*value++ = 0;
		if (!strcmp(this_char, "uid")) {
			if (!value || !*value)
				goto einval;
			uid = simple_strtoul(value, &value, 0);
			if (*value)
				goto einval;
			setuid = 1;
		} else if (!strcmp(this_char, "gid")) {
			if (!value || !*value)
				goto einval;
			gid = simple_strtoul(value, &value, 0);
			if (*value)
				goto einval;
			setgid = 1;
		} else if (!strcmp(this_char, "mode")) {
			if (!value || !*value)
				goto einval;
			mode = simple_strtoul(value, &value, 0);
			if (*value)
				goto einval;
		} else
			goto einval;
	}
      done_options:
	sbi->sbi_setuid = setuid;
	sbi->sbi_setgid = setgid;
	sbi->sbi_uid = uid;
	sbi->sbi_gid = gid;
	sbi->sbi_mode = mode & ~S_IFMT;
	return (0);
      einval:
	err = -EINVAL;
	goto done_options;
}

/**
 *  spec_read_inode: - read a inode from the filesystem
 *  @inode:	initialized inode to read
 *
 *  The read_inode() method is called on the superblock when we cannot find an inode by inode number
 *  in the inode hashes for the super block.  A fresh inode is created with the inode number
 *  requested and then this method is called to "read" the inode.
 *
 *  For module nodes, the major component of the inode number is zero indicating that this is a
 *  module node; the minor component of the inode number is the module id.  For device nodes, the
 *  major component of the inode number is the module id; the minor component of the inode number is
 *  the device instance.
 *
 *  We use the inode number in this way to determine if the inode is for a module node or whether it
 *  is for a device node and then construct and format the inode accordingly.  We don't call
 *  init_special_inode but get the char_device ourselves and populate the inode.  We then populate
 *  all the operations with our own.  Finally we hash the inode.  The inode is returned to iget() in
 *  the inode lookup() method on the specfs root directory or module directory.
 */
static void spec_read_inode(struct inode *inode)
{
	ptrace(("SPECFS: reading inode %p\n", inode));
	if (getmajor(inode->i_ino)) {
		struct cdevsw *cdev;
		/* for device nodes, the major component of the i_ino is the module id */
		if ((cdev = cdrv_get(getmajor(inode->i_ino)))) {
			struct spec_sb_info *sbi = inode->i_sb->u.generic_sbp;
			dev_t dev = MKDEV(cdev->d_major, getminor(inode->i_ino));
			struct devnode *node;
			if ((node = node_get(cdev, getminor(inode->i_ino)))) {
				inode->u.generic_ip = node;
				node->n_inode = igrab(inode);
			}
			inode->i_mode = sbi->sbi_mode | cdev->d_mode;
			inode->i_uid = sbi->sbi_setuid ? sbi->sbi_uid : current->fsuid;
			inode->i_gid = sbi->sbi_setgid ? sbi->sbi_gid : current->fsgid;
			inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
			// inode->i_op = &spec_dev_i_ops; /* leave at empty_iops */
			inode->i_fop = fops_get(&spec_dev_f_ops);
			inode->i_rdev = to_kdev_t(dev);
			inode->i_cdev = cdget(dev);
		}
	} else {
		struct cdevsw *cdev;
		/* for module nodes, the minor component of the i_ino is the module id */
		if ((cdev = cdrv_get(getminor(inode->i_ino)))) {
			struct spec_sb_info *sbi = inode->i_sb->u.generic_sbp;
			dev_t dev = NODEV;
			inode->u.generic_ip = cdev;
			cdev->d_inode = igrab(inode);
			inode->i_mode = sbi->sbi_mode | S_IFDIR;
			inode->i_uid = sbi->sbi_setuid ? sbi->sbi_uid : current->fsuid;
			inode->i_gid = sbi->sbi_setgid ? sbi->sbi_gid : current->fsgid;
			inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
			inode->i_op = &spec_dir_i_ops;
			inode->i_fop = fops_get(&spec_dir_f_ops);
			inode->i_rdev = to_kdev_t(dev);
			inode->i_cdev = NULL;
		}
	}
	make_bad_inode(inode);
	// insert_inode_hash(inode); /* already hashed */
	return;
}

/**
 *  spec_put_inode: - put an inode to the filesystem
 *  @inode:	inode to put
 *
 *  The put_inode() superblock operation is called whenever performing iput() on an inode, and
 *  before checking i_nlink on an inode if the i_count has dropped to zero.  This is our chance to
 *  set the number of links to zero so that the inode will just be destroyed (and delete_inode()
 *  below will be called as well).  A force delete here will just set i_nlinks to zero if the
 *  i_count is at 1.
 */
static void spec_put_inode(struct inode *inode)
{
	ptrace(("SPECFS: putting inode %p\n", inode));
	force_delete(inode);
}

/**
 *  spec_delete_inode: - delete an inode from the filesystem
 *  @inode:	inode to delete
 *
 *  The delete_inode() superblock operation is called before removing an inode.  We need to detach
 *  the inode from the underlying stream head structure and initiate the close procedure on that
 *  stream head if it has no other references.
 *
 *  Module (directory) inodes do not have any stream head associated with them and therefore do not
 *  have an i_pipe pointer set.
 *
 *  Because the inode numbers are controls and the mode of the inode is controlled by us, we can
 *  know what type of underlying structure may be hanging off of the u.generic_ip pointer.
 */
static void spec_delete_inode(struct inode *inode)
{
	ptrace(("SPECFS: deleting inode %p\n", inode));
	switch (inode->i_mode & S_IFMT) {
		struct fmodsw *fmod;
		struct devnode *node;
//              struct stdata *sd;
	case S_IFDIR:
		/* directory inodes potentially have a cdevsw or fmodsw structure hanging off of
		   the u.generic_ip pointer, these are for sanity checks only. When we referemce
		   the inode from the module structure, we hold a reference count on the inode.  We 
		   should never get here with either the u.generic_ip pointer set or the f_inode
		   reference still held.  Forced deletions might get us here anyway. */
		if ((fmod = inode->u.generic_ip)) {
			swerr();
			inode->u.generic_ip = NULL;
			fmod->f_inode = NULL;
			fmod_put(fmod);
		}
		break;
	case S_IFCHR:
		/* character special device inodes potentially have a minor devnode structure
		   hanging off of the u.generic_ip pointer and stream heads hanging off of the
		   i_pipe pointer. When we referemce the inode from the devnode structure, we hold
		   a reference count on the inode.  We should never get here with either the
		   u.generic_ip pointer set or the n_inode reference still held.  Forced deletions
		   might get us here anyway. */
		if ((node = inode->u.generic_ip)) {
			swerr();
			inode->u.generic_ip = NULL;
			node->n_inode = NULL;
		}
		/* fall through */
	default:
#ifdef CONFIG_STREAMS_FIFO
	case S_IFIFO:
		/* unnamed pipe inodes potentially have stream heads hanging off of the i_pipe
		   pointer and nothing hanging off of the u.generic_ip pointer. */
#endif
#ifdef CONFIG_STREAMS_SOCKSYS
	case S_IFSOCK:
		/* socket inodes potentially have stream heads hanging off of the i_pipe pointer
		   and nothing hanging off of the u.generic_ip_pointer. */
#endif
		/* When we referemce the inode from the stdata structure, we hold a reference count 
		   on the inode.  We should never get here with the the sd_inode reference still
		   held.  Forced deletions might get us here anyway. */
#if 0
		if ((sd = (struct stdata *) inode->i_pipe)) {
			swerr();
			do {
				inode->i_pipe = (void *) xchg(&sd->sd_clone, NULL);
				if (sd->sd_inode == inode)
					sd->sd_inode = NULL;
				sd_put(sd);
			}
			while ((sd = (struct stdata *) inode->i_pipe));
		}
#endif
		break;
	}
	clear_inode(inode);	/* we must call clear_inode when we take this hook */
	/* note that clear_inode() will remove the i_cdev regardless of the mode of the inode; note 
	   that i_pipe is not cleared (we did it above) */
}

/**
 *  spec_put_super: - put a superblock for the filesystem
 *  @sb:	super block to put
 *
 *  When we put a super block away, we must detach the options structure from the superblock and
 *  free it.
 */
static void spec_put_super(struct super_block *sb)
{
	struct spec_sb_info *sbi;
	ptrace(("SPECFS: putting superblock %p\n", sb));
	/* just free our optional mount information */
	if ((sbi = xchg(&sb->u.generic_sbp, NULL)))
		kmem_free(sbi, sizeof(*sbi));
}

#define SPECFS_MAGIC 0xDEADBEEF

/**
 *  spec_statfs: - provide statfs information
 *  @sb:	super block
 *  @buf:	buffer for statfs data
 */
static int spec_statfs(struct super_block *sb, struct statfs *buf)
{
	ptrace(("SPECFS: stat filesystem\n"));
	buf->f_type = SPECFS_MAGIC;
	buf->f_bsize = 1024;
	buf->f_namelen = 255;
	return 0;
}

/**
 *  spec_remount_fs: - remount the shadow special filesystem
 *  @sb:	super block
 *  @flags:	flags
 *  @data:	options data
 *
 *  Notices: When we are remounted, we must check for new options.
 */
static int spec_remount_fs(struct super_block *sb, int *flags, char *data)
{
	struct spec_sb_info *sbi = sb->u.generic_sbp;
	return spec_parse_options(data, sbi);
}

static struct super_operations spec_s_ops ____cacheline_aligned = {
	read_inode:spec_read_inode,
//      dirty_inode:NULL,
//      write_inode:NULL,
	put_inode:spec_put_inode,
	delete_inode:spec_delete_inode,
	put_super:spec_put_super,
//      write_super:NULL,
//      write_super_lockfs:NULL,
//      unlockfs:NULL,
	statfs:spec_statfs,
	remount_fs:spec_remount_fs,
//      clear_inode:NULL,
//      umount_begin:NULL,
};

/**
 *  specfs_read_super: - create a shadow special filesystem super block
 *  @sb:	superblock for which to read a superblock inode
 *  @data:	options data
 *  @silent:	whether to ignore options errors
 *
 *  Create the super block inode.
 */
static struct super_block *specfs_read_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct dentry *root;
	struct spec_sb_info *sbi;
	ptrace(("SPECFS: reading superblock\n"));
	/* need to read options on mount */
	if (!(sbi = spec_sbi_alloc()))
		goto fail;
	if (spec_parse_options(data, sbi) < 0 && !silent)
		goto free_fail;
	if (!(inode = new_inode(sb)))
		goto free_fail;
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
	inode->i_blocks = 0;
	inode->i_blksize = PAGE_SIZE;
	inode->i_uid = inode->i_gid = 0;
	inode->i_mode = S_IFDIR | S_IRUGO | S_IXUGO | S_IWUSR;
	inode->i_op = &spec_root_i_ops;
	inode->i_nlink = 2;
	sb->s_blocksize = 1024;
	sb->s_blocksize_bits = 10;
	sb->s_magic = SPECFS_MAGIC;
	sb->s_op = &spec_s_ops;
#if 0				/* !defined CONFIG_STREAMS_MNTSPECFS */
	if (!(root = d_alloc(NULL, &(const struct qstr) {
			     "spec:", 5, 0})))
		goto iput_fail;
	root->d_sb = sb;
	root->d_parent = root;
	d_instantiate(root, inode);
	root->d_op = &spec_root_d_ops;
#else
	inode->i_fop = &spec_root_f_ops;
	if (!(root = d_alloc_root(inode)))
		goto iput_fail;
	root->d_op = &spec_root_d_ops;
#endif
	sb->s_root = root;
	sb->u.generic_sbp = sbi;
	return (sb);
      iput_fail:
	__unless(inode->i_state == I_CLEAR, iput(inode));
      free_fail:
	spec_sbi_free(sbi);
      fail:
	return (NULL);
}

#if defined HAVE_KERNEL_PIPE_SUPPORT
static struct inode *get_spipe_inode(void)
{
	struct inode *inode;
	if ((inode = new_inode(specfs_mnt->mnt_sb))) {
		struct spec_sb_info *sbi = inode->i_sb->u.generic_sbp;
		inode->i_uid = sbi->sbi_setuid ? sbi->sbi_uid : current->fsuid;
		inode->i_gid = sbi->sbi_setgid ? sbi->sbi_gid : current->fsgid;
		inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
		inode->u.generic_ip = NULL;
		inode->i_mode = S_IFIFO | sbi->sbi_mode;
		inode->i_fop = fops_get(&spec_dev_f_ops);
		inode->i_rdev = to_kdev_t(NODEV);
	}
	return (inode);
}
long do_spipe(int *fds)
{
	struct qstr str1, str2;
	struct file *file1, *file2;
	struct inode *inode1, *inode2;
	struct dentry *dentry1, *dentry2;
	char name[32];
	int fd1, fd2;
	int err;
	err = -ENFILE;
	if (!(file1 = get_empty_filp()))
		goto no_file1;
	if (!(file2 = get_empty_filp()))
		goto no_file2;
	if (!(inode1 = get_spipe_inode()))
		goto no_inode1;
	if (!(inode2 = get_spipe_inode()))
		goto no_inode2;
	if ((fd1 = err = get_unused_fd()) < 0)
		goto no_fd1;
	if ((fd2 = err = get_unused_fd()) < 0)
		goto no_fd2;
	snprintf(name, sizeof(name), "[%lu]", inode1->i_ino);
	str1.name = name;
	str1.len = strlen(name);
	str1.hash = inode1->i_ino;
	if (!(dentry1 = d_alloc(specfs_mnt->mnt_sb->s_root, &str1)))
		goto no_dentry1;
	d_add(dentry1, inode1);
	snprintf(name, sizeof(name), "[%lu]", inode2->i_ino);
	str2.name = name;
	str2.len = strlen(name);
	str2.hash = inode1->i_ino;
	if (!(dentry2 = d_alloc(specfs_mnt->mnt_sb->s_root, &str2)))
		goto no_dentry2;
	d_add(dentry2, inode2);
	/* FIXME: need to call open for each dentry */
	/* file 1 */
	file1->f_vfsmnt = mntget(specfs_mnt);
	file1->f_dentry = dget(dentry1);
	file1->f_pos = 0;
	file1->f_flags = O_RDWR;
	file1->f_op = &spec_dev_f_ops;
	file1->f_mode = 3;
	file1->f_version = 0;
	/* file 2 */
	file2->f_vfsmnt = mntget(specfs_mnt);
	file2->f_dentry = dget(dentry2);
	file1->f_pos = 0;
	file1->f_flags = O_RDWR;
	file1->f_op = &spec_dev_f_ops;
	file1->f_mode = 3;
	file1->f_version = 0;
	/* */
	fd_install(fd1, file1);
	fd_install(fd2, file2);
	fds[0] = fd1;
	fds[1] = fd2;
	return (0);
      no_dentry2:
	dput(dentry1);
      no_dentry1:
	put_unused_fd(fd2);
      no_fd2:
	put_unused_fd(fd1);
      no_fd1:
	__unless(inode2->i_state == I_CLEAR, iput(inode2));
      no_inode2:
	__unless(inode1->i_state == I_CLEAR, iput(inode1));
      no_inode1:
	put_filp(file2);
      no_file2:
	put_filp(file1);
      no_file1:
	goto error;
      error:
	return (err);
}

#if defined CONFIG_STREAMS_STH_MODULE
EXPORT_SYMBOL_GPL(do_spipe);
#endif
#endif				/* defined HAVE_KERNEL_PIPE_SUPPORT */

struct vfsmount *specfs_mnt;

static DECLARE_FSTYPE(spec_fs_type, "spec", specfs_read_super, FS_SINGLE);

/**
 *  strspecfs_init: - initialize the shadow special filesystem
 */
int strspecfs_init(void)
{
	int err;
	ptrace(("SPECFS: registering filesystem\n"));
	if (!(err = register_filesystem(&spec_fs_type))) {
		if (IS_ERR(specfs_mnt = kern_mount(&spec_fs_type))) {
			err = PTR_ERR(specfs_mnt);
			unregister_filesystem(&spec_fs_type);
		} else
			MOD_DEC_USE_COUNT;	/* once for the kern mount */
	}
	return (err);
}

/**
 *  strspecfs_exit: - deinitialize the shadow special filesystem
 */
void strspecfs_exit(void)
{
	ptrace(("SPECFS: unregistering filesystem\n"));
	MOD_INC_USE_COUNT;	/* once for the kern mount */
	kern_umount(specfs_mnt);
	unregister_filesystem(&spec_fs_type);
}
