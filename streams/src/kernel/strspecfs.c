/*****************************************************************************

 @(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/05/03 06:30:21 $

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

 Last Modified $Date: 2004/05/03 06:30:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/05/03 06:30:21 $"

static char const ident[] =
    "$RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2004/05/03 06:30:21 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
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
#include <sys/modversions.h>
#endif

#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>
#include <sys/kmem.h>

#include "strdebug.h"
#include "strhead.h"
#include "strfifo.h"
#include "strpipe.h"
#include "strsock.h"
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
      d_dentry name to find the cdevsw or fmodsw table entry of the same name and creates an inode
      for it if one does not already exist.  The inode number of the directory inode is the module
      id.  A cdevsw table entry can be found by moduile id with an iget4() using the module id as
      the inode number.
                                                                                                             
    - If the device dentry equivalent to the device node name does not exist, i_lookup on the module
      inode will attempt to load "/dev/stream/%s/%s" and then "streams-%s-%s" with the module and
      device instance names.  When the kernel module loads and registers the device name, the
      devnode entry will be created and the next lookup operation should succeed.  i_lookup uses the
      d_dentry name to find the devnode entry of the same name and creates an inode for it if one
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

STATIC struct dentry *dentry_lookup(struct cdevsw *cdev)
{
	struct dentry *dentry = NULL;
	if (cdev && !(dentry = cdev->d_dentry)) {
		char buf[FMNAMESZ + 1];
		struct qstr name = { name:buf, len:sizeof(buf), };
		if ((name.len = snprintf(buf, sizeof(buf), cdev->d_name)) > FMNAMESZ)
			name.len = FMNAMESZ;
		if ((dentry = lookup_hash(&name, specfs_mnt->mnt_root))) {
			if (IS_ERR(dentry)) {
				dentry = NULL;
			} else if (!dentry->d_inode || is_bad_inode(dentry->d_inode)) {
				dput(dentry);
				dentry = NULL;
			}
		}
	}
	return (dget(dentry));
}

/**
 *  spec_open: - open a stream from an external character special device, fifo, or socket.
 *  @ext_inode:	external filesystem inode
 *  @ext_file:	external filesystem file pointer (user file pointer)
 *  @args:	arguments to qopen
 */
int spec_open(struct inode *ext_inode, struct file *ext_file, struct str_args *argp)
{
	struct dentry *dentry;
	struct file *file;		/* next file pointer to use */
	struct inode *inode;		/* next inode to use */
	int err;
	struct cdevsw *cdev;
	if ((err = down_interruptible(&ext_inode->i_sem)) < 0)
		goto exit;
	err = ENOENT;
	if (!(cdev = cdev_get(getmajor(argp->dev))))
		goto up_exit;
	if (!(dentry = dentry_lookup(cdev)))
		goto up_exit;
	argp->name.len = snprintf(argp->buf, sizeof(argp->buf), "%u", getminor(argp->dev));
	argp->name.hash = (cdev->d_str->st_rdinit->qi_minfo->mi_idnum << 16) | getminor(argp->dev);
	dentry = lookup_hash(&argp->name, specfs_mnt->mnt_root);
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
	file = dentry_open(dentry, specfs_mnt, ext_file->f_flags);
	if ((err = PTR_ERR(file)) < 0)
		goto up_exit;
	if (err == 0) {
		file_swap_put(ext_file, file);
		goto up_exit;
	}
	/* fifo returns 1 on exit to cleanup shadow pointer and use existing file pointer */
	err = 0;
      dput_exit:
	dput(dentry);
      up_exit:
	up(&ext_inode->i_sem);
      exit:
	return (err);
}

#if 0
/**
 *  nest_open: - nest an open within the shadow special filesystem
 *  @old_inode:	old shadow special filesystem inode
 *  @old_file:	old shadow special filesystem file pointer
 *  @args:	arguments to qopen
 */
int nest_open(struct inode *old_inode, struct file *old_file, struct str_args *argp)
{
	struct dentry *dentry;
	struct inode *inode;		/* next shadow special filesystem inode to use */
	struct file *file;		/* next shadow special filesystem file pointer to use */
	struct cdevsw *cdevsw;
	int err;
	err = -ENXIO;
	if (!(cdevsw = cdrv_get(getmajor(argp->dev))))
		goto exit;
	argp->name.len = 0;
	argp->name.hash = argp->dev;
	dentry = lookup_hash(&argp->name, specfs_mnt->mnt_root);
	cdev_put(cdev);
	if ((err = PTR_ERR(dentry)) < 0)
		goto exit;
	err = -ENOMEM;		/* we only fail to get an inode when memory allocation fails */
	if (!(inode = dentry->d_inode))
		goto exit;
	err = -ENODEV;		/* we only get a bad inode when there is no device entry */
	if (is_bad_inode(inode))
		goto dput_exit;
	dentry->d_fsdata = argp;	/* this is how we pass qopen() arguments */
	file = dentry_open(dentry, specfs_mnt, old_file->f_flags);
	if ((err = PTR_ERR(file)) < 0)
		goto dput_exit;
	file_swap_put(old_file, file);
	err = 0;
      dput_exit:
	dput(dentry);
      exit:
	return (err);
}
#endif

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
	struct cdevsw *cdev;
	if ((cdev = dir->u.generic_ip)) {
		struct devnode *node;
		const char *name = new->d_name.name;
		ulong modid = (new->d_name.hash & 0xffff0000) ? cdev->d_str->st_rdinit->qi_minfo->mi_idnum : 0;
		/* if the name length is zero, the hash *is* the inode number */
		if (new->d_name.len == 0) {
			struct inode *inode;
			ulong minor = (new->d_name.hash & 0x0000ffff);
			if (!(inode = iget(dir->i_sb, (modid << 16) | minor)))
				return ERR_PTR(-ENOMEM);
			if (!is_bad_inode(inode)) {
				if ((node = inode->u.generic_ip) && node->n_dentry) {
					iput(inode);
					return (dget(node->n_dentry));
				}
				d_add(new, inode);
				return (NULL);
			}
			iput(inode);
			return ERR_PTR(-ENOENT);
		}
		/* check if the name is a valid number */
		if (*name != '\0') {
			char *tail = (char *) name;
			ulong minor = simple_strtoul(name, &tail, 0);
			if (*tail != '\0') {
				struct inode *inode;
				if (!(inode = iget(dir->i_sb, (modid << 16) | minor)))
					return ERR_PTR(-ENOMEM);
				if (!is_bad_inode(inode)) {
					if ((node = inode->u.generic_ip) && node->n_dentry) {
						iput(inode);
						return (dget(node->n_dentry));
					}
					d_add(new, inode);
					return (NULL);
				}
				iput(inode);
				return ERR_PTR(-ENOENT);
			}
		}
		/* check if the name is registered as a minor device node name */
		if ((node = node_find(cdev, name))) {
			if (!node->n_dentry || !node->n_dentry->d_inode) {
				ulong minor = node->n_minor;
				struct inode *inode;
				if (!(inode = iget(dir->i_sb, (modid << 16) | minor)))
					return ERR_PTR(-ENOMEM);
				if (!is_bad_inode(inode)) {
					if (inode->u.generic_ip == NULL)
						inode->u.generic_ip = node;
					if (node->n_dentry) {
						iput(inode);
						return (dget(node->n_dentry));
					}
					d_add(new, inode);
					return (NULL);
				}
				iput(inode);
				return ERR_PTR(-ENOENT);
			}
			return (dget(node->n_dentry));
		}
	}
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
	struct cdevsw *cdev = (struct cdevsw *) inode->u.generic_ip;
	int nr = file->f_pos, start = nr, err;
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
			if (!node->n_dentry || !(inode = node->n_dentry->d_inode))
				continue;
			if (!(inode = dentry->d_inode))
				continue;
			err = filldir(dirent, node->n_name, strnlen(node->n_name, FMNAMESZ + 1),
				      file->f_pos, inode->i_ino, inode->i_mode >> 12);
			if (err == -EINVAL)
				break;
			if (err < 0)
				return (err);
			file->f_pos = ++nr;
		}
		/* walk the active stream head list */
		list_for_each_safe(pos, tmp, &cdev->d_stlist) {
			struct stdata *sd = list_entry(pos, struct stdata, sd_list);
			if (!(dentry = sd->sd_dentry) || !(inode = dentry->d_inode))
				continue;
			err = filldir(dirent, dentry->d_name.name, dentry->d_name.len, file->f_pos,
				      inode->i_ino, inode->i_mode >> 12);
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

static struct file_operations spec_dir_f_ops = {
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
	struct fmodsw *fmod;
	const char *name = new->d_name.name;
	/* if the name length is zero, the hash *is* the inode number */
	if (new->d_name.len == 0) {
		ulong modid = new->d_name.hash;
		if (!(inode = iget(dir->i_sb, modid)))
			return ERR_PTR(-ENOMEM);
		if (!is_bad_inode(inode)) {
			if ((fmod = inode->u.generic_ip) && fmod->f_dentry) {
				iput(inode);
				return (dget(fmod->f_dentry));
			}
			d_add(new, inode);
			return (NULL);
		}
		iput(inode);
		return ERR_PTR(-ENOENT);
	}
	/* check if the name is a valid number */
	if (*name != '\0') {
		char *tail = (char *) name;
		ulong modid = simple_strtoul(name, &tail, 0);
		if (*tail != '\0') {
			if (!(inode = iget(dir->i_sb, modid)))
				return ERR_PTR(-ENOMEM);
			if (!is_bad_inode(inode)) {
				if ((fmod = inode->u.generic_ip) && fmod->f_dentry) {
					iput(inode);
					return (dget(fmod->f_dentry));
				}
				d_add(new, inode);
				return (NULL);
			}
			iput(inode);
			return ERR_PTR(-ENOENT);
		}
	}
	/* this will also attempt to demand load the "streams-%s" if required */
	if ((fmod = (struct fmodsw *) cdev_find(name)) ||
	    (fmod = (struct fmodsw *) fmod_find(name))) {
		if (!fmod->f_dentry || !(inode = fmod->f_dentry->d_inode)) {
			ulong modid = fmod->f_str->st_rdinit->qi_minfo->mi_idnum;
			struct inode *inode;
			if (!(inode = iget(dir->i_sb, modid))) {
				fmod_put(fmod);
				return ERR_PTR(-ENOMEM);
			}
			if (!is_bad_inode(inode)) {
				if (inode->u.generic_ip == NULL)
					inode->u.generic_ip = fmod;
				if (fmod->f_dentry) {
					iput(inode);
					dget(fmod->f_dentry);
					return (fmod->f_dentry);
				}
				fmod_put(fmod);
				d_add(new, inode);
				return (NULL);
			}
			iput(inode);
			return ERR_PTR(-ENOENT);
		}
		return (dget(fmod->f_dentry));
	}
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
			if (!cdev->d_dentry || !(inode = cdev->d_dentry->d_inode))
				continue;
			err = filldir(dirent, dentry->d_name.name, dentry->d_name.len, file->f_pos,
				      inode->i_ino, inode->i_mode >> 12);
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
	read:generic_read_dir,
	readdir:spec_root_readdir,
};

/* 
 *  Shadow Special Filesystem Root directory entry operations.
 *  -------------------------------------------------------------------------
 */
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
	if (!(inode = dentry->d_inode))
		goto invalid;	/* we need an inode */
	if (inode->i_cdev == NULL)
		goto invalid;	/* it must be marked special */
	return (1);
      invalid:
	return (0);
}

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
	return (1);
}

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
	iput(inode);		/* this is what the system does otherwise */
}

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
	return;
}

struct dentry_operations spec_root_d_ops ____cacheline_aligned = {
	d_revalidate:spec_root_d_revalidate,
	d_hash:spec_root_d_hash,
	d_compare:spec_root_d_compare,
	d_delete:spec_root_d_delete,
	d_release:spec_root_d_release,
	d_iput:spec_root_d_iput,
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
#define SPEC_SBI_MAGIC 0XFEEDDEAF
struct spec_sb_info {
	u32 sbi_magic;
	int sbi_setuid;
	int sbi_setgid;
	uid_t sbi_uid;
	gid_t sbi_gid;
	umode_t sbi_mode;
};

struct spec_sb_info *spec_sbi_alloc(void)
{
	struct spec_sb_info *sbi;
	if ((sbi = kmem_zalloc(sizeof(*sbi), KM_NOSLEEP)))
		sbi->sbi_magic = SPEC_SBI_MAGIC;
	return (sbi);
}

void spec_sbi_free(struct spec_sb_info *sbi)
{
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
 *  For module nodes, the upper 16 bits of the inode number is zero indicating that this is a module
 *  node; the lower 16 bits of the inode number is the module id.  For device nodes, the upper 16
 *  bits of the inode number is the module id; the lower 16 bits of the inode number is the device
 *  instance.
 *
 *  We use the inode number in this way to determine if the inode is for a module node or whether it
 *  is for a device node and then construct and format the inode accordingly.  We don't call
 *  init_special_inode but get the char_device ourselves and populate the inode.  We then populate
 *  all the operations with our own.  Finally we hash the inode.  The inode is returned to iget() in
 *  the inode lookup() method on the specfs root directory or module directory.
 */
static void spec_read_inode(struct inode *inode)
{
	modID_t modid;
	struct cdevsw *sdev;
	dev_t dev;
	if ((modid = (inode->i_ino >> 16) & 0x0000ffff)) {
		/* For device nodes, the upper 16 bits of the inode number is the module id; the
		   lower 16 bits of the inode number is the device instance. */
		if ((sdev = (struct cdevsw *) fmod_get(modid))) {
			struct spec_sb_info *sbi;
			dev = makedevice(sdev->d_major, inode->i_ino & 0x0000ffff);
			/* initialize specfs specific fields */
			sbi = inode->i_sb->u.generic_sbp;
			inode->i_uid = sbi->sbi_setuid ? sbi->sbi_uid : current->fsuid;
			inode->i_gid = sbi->sbi_setgid ? sbi->sbi_gid : current->fsgid;
			inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
			inode->u.generic_ip = sdev;
			inode->i_mode = sdev->d_mode | sbi->sbi_mode;
			inode->i_fop = fops_get(sdev->d_fop);
			inode->i_rdev = to_kdev_t(dev);
			inode->i_cdev = cdget(dev);
			insert_inode_hash(inode);
			return;
		}
	} else {
		/* For module nodes, the upper 16 bits of the inode number is zero indicating that
		   this is a module node; the lower 16 bits is the module id. */
		if ((modid = inode->i_ino & 0x0000ffff)) {
			dev = NODEV;
			if ((sdev = (struct cdevsw *) fmod_get(modid))) {
				/* initialize specfs specific fields */
				struct spec_sb_info *sbi;
				sbi = inode->i_sb->u.generic_sbp;
				inode->i_uid = sbi->sbi_setuid ? sbi->sbi_uid : current->fsuid;
				inode->i_gid = sbi->sbi_setgid ? sbi->sbi_gid : current->fsgid;
				inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
				inode->u.generic_ip = sdev;
				inode->i_mode = S_IFDIR | sbi->sbi_mode;
				inode->i_op = &spec_dir_i_ops;
				inode->i_fop = fops_get(&spec_dir_f_ops);
				inode->i_rdev = to_kdev_t(NODEV);
				inode->i_cdev = NULL;
				insert_inode_hash(inode);
				return;
			}
			/* No module with that id. */
		}
	}
	make_bad_inode(inode);
	insert_inode_hash(inode);
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
	switch (inode->i_mode & S_IFMT) {
		struct fmodsw *fmod;
		struct devnode *node;
		struct stdata *sd;
	case S_IFDIR:
		/* directory inodes potentially have a cdevsw or fmodsw structure hanging off of
		   the u.generic_ip pointer, these are for sanity checks only. When we referemce
		   the inode from the module structure, we hold a reference count on the inode.  We 
		   should never get here with either the u.generic_ip pointer set or the f_dentry
		   reference still held.  Forced deletions might get us here anyway. */
		if ((fmod = inode->u.generic_ip)) {
			swerr();
			inode->u.generic_ip = NULL;
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
	iput(inode);
      free_fail:
	spec_sbi_free(sbi);
      fail:
	return (NULL);
}

struct vfsmount *specfs_mnt;

static DECLARE_FSTYPE(spec_fs_type, "spec", specfs_read_super, FS_SINGLE);

/**
 *  strspecfs_init: - initialize the shadow special filesystem
 */
int strspecfs_init(void)
{
	int err;
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
	MOD_INC_USE_COUNT;	/* once for the kern mount */
	kern_umount(specfs_mnt);
	unregister_filesystem(&spec_fs_type);
}
