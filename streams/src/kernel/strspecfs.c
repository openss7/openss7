/*****************************************************************************

 @(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/04/28 04:11:34 $

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

 Last Modified $Date: 2004/04/28 04:11:34 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/04/28 04:11:34 $"

static char const ident[] =
    "$RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/04/28 04:11:34 $";

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

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/strsubr.h>
#include <sys/strconf.h>

#include "strdebug.h"
#include "strhead.h"
#include "strfifo.h"
#include "strpipe.h"
#include "strsock.h"
#include "strsched.h"

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
      d_entry name to find the cdevsw or fmodsw table entry of the same name and creates an inode
      for it if one does not already exist.  The inode number of the directory inode is the module
      id.  A cdevsw table entry can be found by moduile id with an iget4() using the module id as
      the inode number.
                                                                                                             
    - If the device dentry equivalent to the device node name does not exist, i_lookup on the module
      inode will attempt to load "/dev/stream/%s/%s" and then "streams-%s-%s" with the module and
      device instance names.  When the kernel module loads and registers the device name, the
      devnode entry will be created and the next lookup operation should succeed.  i_lookup uses the
      d_entry name to find the devnode entry of the same name and creates an inode for it if one
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
 *  Shadow Special Filesystem ROOT
 *
 *  =========================================================================
 *
 *  Shadow Special Filesystem Root inode operations.
 *  -------------------------------------------------------------------------
 *
 *  I_LOOKUP
 *  -------------------------------------------
 *  The lookup() method is called on directory inodes (in our case the root) when a cached_lookup()
 *  of the dentry fails on the name.  We are given a newly created dentry, "new", that has no inode.
 *  We lookup the inode by name from the module switch table and then look up the inode by module id
 *  from the device/module switch table entry from the super block.  The iget() called here will
 *  check the inode hash an either find an existing inode, or it invokes the superblock
 *  spec_read_inode() operation, below, that will create a new inode.  We only fail to get an inode
 *  when the inode allocate fails -ENOMEM.
 *
 *  When we get here we either do not have an inode associated with the dentry name, or there is
 *  simply no dentry in the dentry cache for this inode yet.  We walk through the cdevsw and fmodsw
 *  lists looking for the dentry name.  If we find one, we lookup or create a inode with the inode
 *  number equal to the module id and return that inode for attachment to the dentry.  If the name
 *  cannot be found and the name is numeric, we check again with the module id equal to the numeric
 *  name.  If we cannot find a corresponding cdevsw or fmodsw entry and we are configured for kernel
 *  module loading, we request a module of the name streams-%s, where %s is the name requested.
 */
static struct dentry *spec_root_i_lookup(struct inode *dir, struct dentry *dentry)
{
	{
		struct cdevsw *cdev;
		/* this will also attempt to demand load the driver if required */
		if ((cdev = sdev_find(dentry->d_name.name))) {
			struct inode *inode;
			if (!(inode = cdev->d_inode)
			    && (inode = iget(dir->i_sb, cdev->d_str->st_rinit->qi_minfo->mi_idnum))) {
				if (inode->u.generic_ip == NULL)
					inode->u.generic_ip = cdev;
				cdev->d_inode = inode;
			}
			if (inode)
				d_add(dentry, inode);
			else
				sdev_put(cdev);
			return (NULL);
		}
	}
	{
		struct fmodsw *fmod;
		/* this will also attempt to demand load the module if required */
		if ((fmod = smod_find(dentry->d_name.name))) {
			struct inode *inode;
			if (!(inode = fmod->f_inode)
			    && (inode = iget(dir->i_sb, fmod->f_str->st_rinit->qi_minfo->mi_idnum))) {
				if (inode->u.generic_ip == NULL)
					inode->u.generic_ip = fmod;
				fmod->f_inode = inode;
			}
			if (inode)
				d_add(dentry, inode);
			else
				smod_put(fmod);
			return (NULL);
		}
	}
	return ERR_PTR(-ENOENT);
}

/* 
 *  I_LINK
 *  -------------------------------------------
 */
static int spec_root_i_link(struct dentry *, struct inode *, struct dentry *)
{
}

/* 
 *  I_UNLINK
 *  -------------------------------------------
 *  Unfortunately we cannot delete modules.  This really consists of unlinking the module
 *  subdirectory from the root directory entry and does not actually delete the module.
 */
static int spec_root_i_unlink(struct inode *dir, struct dentry *dentry)
{
	struct inode *inode = dentry->d_inode;
	struct fmodsw *fmod = inode->u.generic_ip;
	if (fmod == NULL)
		return (-ENOENT);
	if (inode->i_nlink > 1)
		return (-EBUSY);
	inode->i_ctime = dir->i_ctime;
	xchg(fmod->f_inode, NULL);
	xchg(inode->i_pipe, NULL);
	dir->i_nlink--;
	return (0);
}

/* 
 *  I_PERMISSION
 *  -------------------------------------------
 */
static int spec_root_i_permission(struct inode *, int)
{
}

/* 
 *  I_SETATTR
 *  -------------------------------------------
 */
static int spec_root_i_setattr(struct dentry *, struct iattr *)
{
}

/* 
 *  I_GETATTR
 *  -------------------------------------------
 */
static int spec_root_i_getattr(struct dentry *, struct iattr *)
{
}

/* *INDENT-OFF* */
static struct inode_operations spec_root_i_ops = {
//	create:		NULL, /* will return -EACCES */
	lookup:		spec_root_i_lookup,
	link:		spec_root_i_link,
	unlink:		spec_root_i_unlink,
//	symlink:	spec_root_i_symlink,
//	mkdir:		NULL, /* will return -EPERM */
//	rmdir:		NULL, /* will return -EPERM */
//	mknod:		NULL, /* will return -EPERM */
//	rename:		NULL, /* will return -EPERM */
//	readlink:	NULL, /* will return -EINVAL */
//	follow_link:	NULL, /* will return -EINVAL */
//	truncate:	NULL,
//	permission:	spec_root_i_permission,
//	revalidate:	NULL, /* will do nothing */
//	setattr:	spec_root_i_setattr,
//	getattr:	spec_root_i_getattr,
//	setxattr:	spec_root_i_setxattr,
//	getxattr:	spec_root_i_getxattr,
//	listxattr:	spec_root_i_listxattr,
//	removexattr:	spec_root_i_removexattr,
};
/* *INDENT-ON* */

/* 
 *  =========================================================================
 *
 *  Shadow Special Filesystem ROOT
 *
 *  =========================================================================
 *
 *  Shadow Special Filesystem Directory inode operations.
 *  -------------------------------------------------------------------------
 *
 *  I_LOOKUP
 *  -------------------------------------------------------------------------
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
static struct dentry *spec_dir_i_lookup(struct inode *dir, struct dentry *dentry)
{
	struct cdevsw *cdev = (typeof(cdev)) dir->u.generic_ip;
	struct dentry *parent = cdev->d_dentry;
	char devname[64];
	/* specfs(5) documentation says that we will first attempt to load the kernel module named
	   "/dev/streams/%s/%s" XXX: perhaps we should use the actual path taken to here rather
	   than /dev/streams */
	snprintf(sizeof(devname), devname, "/dev/streams/%s/%s",
		 parent->d_name.name, dentry->d_name.name);
	request_module(devname);
	/* specfs(5) documentation says that we will next attempt to load the kernel module named
	   "streams-%s-%s" */
	snprintf(sizeof(devname), devname, "streams-%s-%s",
		 parent->d_name.name, dentry->d_name.name);
	request_module(devname);
	if (!(dentry = d_lookup(parent, &dentry->d_name)))
		return ERR_PTR(-ENOENT);
	return (dentry);
}

/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_link(struct dentry *, struct inode *, struct dentry *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_unlink(struct inode *, struct dentry *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_symlink(struct inode *, struct dentry *, const char *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_mkdir(struct inode *, struct dentry *, int)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_rmdir(struct inode *, struct dentry *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_mknod(struct inode *, struct dentry *, int, int)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_rename(struct inode *, struct dentry *, struct inode *, struct dentry *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_readlink(struct dentry *, char *, int)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_follow_link(struct dentry *, struct nameidata *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static void spec_dir_i_truncate(struct inode *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_permission(struct inode *, int)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_revalidate(struct dentry *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_setattr(struct dentry *, struct iattr *)
{
}
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_dir_i_getattr(struct dentry *, struct iattr *)
{
}

/*
 *  Shadow Special Filesystem Directory inode operations
 *  -------------------------------------------------------------------------
 */
/* *INDENT-OFF* */
static struct inode_operations spec_dir_i_ops = {
	create:		spec_dir_i_create,
	lookup:		spec_dir_i_lookup,
	link:		spec_dir_i_link,
	unlink:		spec_dir_i_unlink,
	symlink:	spec_dir_i_symlink,
	mkdir:		spec_dir_i_mkdir,
	rmdir:		spec_dir_i_rmdir,
	mknod:		spec_dir_i_mknod,
	rename:		spec_dir_i_rename,
	readlink:	spec_dir_i_readlink,
	follow_link:	spec_dir_i_follow_link,
	truncate:	spec_dir_i_truncate,
	permission:	spec_dir_i_permission,
	revalidate:	spec_dir_i_revalidate,
	setattr:	spec_dir_i_setattr,
	getattr:	spec_dir_i_getattr,
//      setxattr:	spec_dir_i_setxattr,
//      getxattr:	spec_dir_i_getxattr,
//      listxattr:	spec_dir_i_listxattr,
//      removexattr:	spec_dir_i_removexattr,
};
/* *INDENT-ON* */

/* 
 *  Shadow Special Filesystem Root inode operations.
 *  -------------------------------------------------------------------------
 */
/*
 *  I_LOOKUP
 *  -------------------------------------------------------------------------
 *  The lookup() method is called on directory inodes (in our case the root) when a cached_lookup()
 *  of the dentry fails on the name.  We are given a newly created dentry, "new", that has no inode,
 *  but has our hash value in the name. We lookup the inode by hash from the super block.  The
 *  iget() called here will check the inode hash an either find an existing inode, or it invokes the
 *  superblock spec_read_inode() operation, below, that will create a new inode.  We only fail to
 *  get an inode when the inode allocate fails -ENOMEM.
 */
static struct dentry *spec_i_lookup(struct inode *dir, struct dentry *new)
{
	struct inode *inode;
	/* we always used the hash as the inode number */
	if ((inode = iget(dir->i_sb, new->d_name.hash)))
		d_add(new, inode);
	return (NULL);
}

#if 0
/*
 *  I_LINK
 *  -------------------------------------------------------------------------
 */
static int spec_i_link(struct dentry *old_dentry, struct inode *dir, struct dentry *new_dentry)
{
	struct inode *inode = old_dentry->d_inode;
	if (S_ISDIR(inode->i_mode))
		return (-EPERM);
	if (inode->i_nlink >= 8)
		return -EMLINK;
	inode->i_ctime = CURRENT_TIME;
	inode->i_nlink++;
	igrab(inode);
	/* FIXME: need to link the two together somehow... */
	d_instantiate(new_dentry, inode);
	return (0);
}
#endif

/*
 *  I_UNLINK
 *  -------------------------------------------------------------------------
 */
static int spec_i_unlink(struct inode *dir, struct dentry *child)
{
	struct inode *inode = child->d_inode;
	inode->i_ctime = dir->i_ctime;
	dir->i_nlink--;
	return (0);
}

/*
 *  I_MKNOD
 *  -------------------------------------------------------------------------
 */
static int spec_i_mknod(struct inode *dir, struct dentry *new, int mode, int dev)
{
	struct inode *inode;
	if ((inode = iget(dir->i_sb, dev))) {
		d_instantiate(new, inode);
		return (0);
	}
	return (ENOMEM);
}

/* *INDENT-OFF* */
struct inode_operations spec_root_i_ops ____cacheline_aligned = {
	lookup:		spec_i_lookup,
	unlink:		spec_i_unlink,
	mknod:		spec_i_mknod,
};
/* *INDENT-ON* */

/* 
 *  Shadow Special Filesystem Device inode operations.
 *  -------------------------------------------------------------------------
 */
/* *INDENT-OFF* */
struct inode_operations spec_dev_i_ops ____cacheline_aligned = {
	create:		spec_dev_i_create,
	lookup:		spec_dev_i_lookup,
	link:		spec_dev_i_link,
	unlink:		spec_dev_i_unlink,
//      symlink:	spec_dev_i_symlink,
	mkdir:		spec_dev_i_mkdir,
	rmdir:		spec_dev_i_rmdir,
	mknod:		spec_dev_i_mknod,
	rename:		spec_dev_i_rename,
	readlink:	spec_dev_i_readlink,
	follow_link:	spec_dev_i_follow_link,
	truncate:	spec_dev_i_truncate,
	permission:	spec_dev_i_permission,
	revalidate:	spec_dev_i_revalidate,
	setattr:	spec_dev_i_setattr,
	getattr:	spec_dev_i_getattr,
	// setxattr:	spec_dev_i_setxattr,
	// getxattr:	spec_dev_i_getxattr,
	// listxattr:	spec_dev_i_listxattr,
	// removexattr:	spec_dev_i_removexattr,
};
/* *INDENT-ON* */

#ifdef CONFIG_STREAMS_MNTSPECFS
/* 
 *  Shadow Special Filesystem Directory file operations.
 *  -------------------------------------------------------------------------
 */
/*
 *  READDIR
 *  -------------------------------------------------------------------------
 */
static int spec_dir_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = file->f_dentry;
	struct inode *inode = dentry->d_inode;
	struct list_head *pos, *tmp;
	struct cdevsw *cdev = (struct cdevsw *) inode->u.generic_ip;
	struct stdata *sd;
	int nr = file->f_pos, pos = nr, err;
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
		list_for_each_safe(pos, tmp, &cdev->d_stlist) {
			sd = (struct stdata *) list_entry(pos, struct stdata, sd_list);
			if (!(dentry = sd->sd_dentry))
				continue;
			inode = dentry->d_inode;
			err =
			    filldir(dirent, dentry->d_name.name, dentry->d_name.len, file->f_pos,
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
	return (file->f_pos - pos);
}
/* *INDENT-OFF* */
static struct file_operations spec_dir_f_ops = {
	read:		generic_read_dir,
	readdir:	spec_dir_readdir,
};
/* *INDENT-ON* */
#endif				/* CONFIG_STREAMS_MNTSPECFS */

#ifdef CONFIG_STREAMS_MNTSPECFS
/* 
 *  Shadow Special Filesystem Root file operations.
 *  -------------------------------------------------------------------------
 */
/*
 *  READDIR
 *  -------------------------------------------------------------------------
 */
static int spec_root_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct inode *inode = file->f_dentry->d_inode;
	struct strinfo *si = &Strinfo[DYN_STREAM];
	struct stdata *sd;
	struct list_head *pos, *tmp;
	char buf[64];
	off_t nr;
	nr = file->f_pos;
	switch (nr) {
	case 0:
		if (filldir(dirent, ".", 1, nr, inode->i_ino, DT_DIR) < 0)
			return (0);
		file->f_pos = ++nr;
	case 1:
		if (filldir(dirent, "..", 2, nr, inode->i_ino, DT_DIR) < 0)
			return (0);
		file->f_pos = ++nr;
	default:
	list_for_each_safe(pos, tmp, &si->si_head) {
		sd = (struct stdata *) list_entry(pos, struct shinfo, sh_next);
		if (!(inode = sd->sd_inode))
			continue;
		snprintf(buf, 64, "[%ld]", inode->i_ino);
		if (filldir(dirent, buf, strnlen(buf, 64), nr, inode->i_ino, DT_CHR) < 0)
			return (0);
		file->f_pos = ++nr;
	}
		break;
	}
	return (0);
}
/* *INDENT-OFF* */
struct file_operations spec_root_f_ops = {
	read:		generic_read_dir,
	readdir:	spec_root_readdir,
};
/* *INDENT-ON* */
#endif				/* CONFIG_STREAMS_MNTSPECFS */

/* 
 *  Shadow Special Filesystem Root directory entry operations.
 *  -------------------------------------------------------------------------
 */
/*
 *  D_REVALIDATE
 *  -------------------------------------------------------------------------
 *  Revalidate the directory entry.  Always occurs on a hashed lookup if we have this method.
 */
static int spec_d_revalidate(struct dentry *dentry, int flags)
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

/*
 *  D_HASH
 *  -------------------------------------------------------------------------
 *  The d_hash() method is called to hash the directory name for directory lookups.  We do not have
 *  an inode yet.  In the specfs, when we are opening a dentry internally, the dentry is unnamed
 *  (has a name length of zero) but has a hash number equal to the device number.  The inode number
 *  is in turn formulated from the file mode and device number.  We use the inode number itself as
 *  the hash.  So, if the name is zero length, we called internally, and the hash is already valid.
 *  If the name length is non-zero, then this is an external lookup from the mounted specfs, and we
 *  need to look through the instantiated minor device list to find the name and map back to the
 *  device number. We can return an error number (-ENOENT) here if we don't like the name.
 */
static int spec_d_hash(struct dentry *dentry, struct qstr *dname)
{
	/* Note that this is also called directly from lookup_hash() with a zero length name and
	   the hash value already set to the inode number.  So, if the dentry name length is zero,
	   we just leave the hash alone. */
	if (dname->len) {
	}
	return (0);		/* success */
}

/*
 *  D_COMPARE
 *  -------------------------------------------------------------------------
 *  The d_compare() method is called to compare names.  We ignore the name on lookups.  The reason
 *  for this is that we have our own hash above and we really don't care what name is used as long
 *  as the hash values match. The hash is formulated from the inode device number and is the inode
 *  number anyway.  Therefore the hash itself uniquely identifies the inode
 */
static int spec_d_compare(struct dentry *parent, struct qstr *child, struct qstr *name)
{
	/* Note that we also call lookup_hash() directly sometimes with a zero length name. */
	return (child->hash != name->hash);	/* ignore name on lookups */
}

/*
 *  D_IPUT
 *  -------------------------------------------------------------------------
 *  The d_delete() method is called when the dentry use count drops to zero, before performing any
 *  other functions. Typically the system would just marks the dentry unusable if it is unhashed (as
 *  though we returned (0) to this function). When the dentry is deleted on the specfs, we can
 *  deallocate it altogether, so we return (1) here. That's what sockfs, pipefs and other hidden
 *  filesystems do.
 */
static int spec_d_delete(struct dentry *dentry)
{
	return (1);
}

/*
 *  D_IPUT
 *  -------------------------------------------------------------------------
 *  The d_iput() method is called to put the inode.  The system normally just calls iput() but calls
 *  this method instead.  We reference the stream head data structure from the inode, but we will
 *  get another kick at the can with the superblock put_inode() and delete_inode() operations that
 *  are called by iput(), see spec_delete_inode().
 */
static void spec_d_iput(struct dentry *dentry, struct inode *inode)
{
	iput(inode);		/* this is what the system does otherwise */
}

/*
 *  D_RELEASE
 *  -------------------------------------------------------------------------
 *  The d_release() method is called just before deleting the dentry altogether.  If we are
 *  referencing anything with the d_fsdata pointer, now is the time to remove the reference.  We
 *  actually point directory entries at stream head data structures, so we need to dereference that
 *  now.
 */
static void spec_d_release(struct dentry *dentry)
{
	return;
}

/* *INDENT-OFF* */
struct dentry_operations spec_root_d_ops ____cacheline_aligned = {
	d_revalidate:	spec_d_revalidate,
	d_hash:		spec_d_hash,
	d_compare:	spec_d_compare,
	d_delete:	spec_d_delete,
	d_release:	spec_d_release,
	d_iput:		spec_d_iput,
};
/* *INDENT-ON* */

/* 
 *  Shadow Special Filesystem super block.
 *  -------------------------------------------------------------------------
 */
#define SPEC_SBI_MAGIC 0XFEEDDEEAF
struct spec_sb_info {
	u32 magic;
	int setuid;
	int setgid;
	uid_t uid;
	gid_t gid;
	umode_t mode;
};

static int spec_parse_options(char *options, struct spec_sb_info *sbi)
{
	int setuid = 0;
	int setgid = 0;
	uid_t uid = 0;
	uid_t gid = 0;
	umode_t mode = 0600;
	char *this_char, *value;
	this_char = NULL;
	if (options)
		this_char = strtok(options, ",");
	for (; this_char; this_char = strtok(NULL, ",")) {
		if ((value = strchr(this_char, '=')) != NULL)
			*value++ = 0;
		if (!strcmp(this_char, "uid")) {
			if (!value || !*value)
				return (-EINVAL);
			uid = simple_strtoul(value, &value, 0);
			if (*value)
				return (-EINVAL);
			setuid = 1;
		} else if (!strcmp(this_char, "gid")) {
			if (!value || !*value)
				return (-EINVAL);
			gid = simple_strtoul(value, &value, 0);
			if (*value)
				return (-EINVAL);
			setgid = 1;
		} else if (!strcmp(this_char, "mode")) {
			if (!value || !*value)
				return (-EINVAL);
			mode = simple_strtoul(value, &value, 0);
			if (*value)
				return (-EINVAL);
		} else
			return (-EINVAL);
	}
	sbi->setuid = setuid;
	sbi->setgid = setgid;
	sbi->uid = uid;
	sbi->gid = gid;
	sbi->mode = mode & ~S_IFMT;
	return (0);
}

/*
 *  READ_INODE
 *  -------------------------------------------------------------------------
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
	if ((modid = (inode->i_no >> 16) & 0x0000ffff)) {
		/* For device nodes, the upper 16 bits of the inode number is the module ide; the
		   lower 16 bits of the inode number is the device instance. */
		if ((sdev = (struct cdevsw *) smod_get(modid))) {
			dev = makedevice(sdev->d_index, inode->i_no & 0x0000ffff);
			if ((sdev->d_flags & F_CDEVSW_DEVICE)) {
				/* initialize specfs specific fields */
				struct spec_sb_info *sbi;
				sbi = inode->i_sb->u.generic_sbp;
				inode->i_uid = sbi->setuid ? sbi->uid : current->fsuid;
				inode->i_gid = sbi->setgid ? sbi->gid : current->fsgid;
				inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
				inode->u.generic_ip = sdev;
				inode->i_mode = sdev->d_mode | sbi->mode;
				inode->i_op = &specfs_dev_iops;
				inode->i_fop = fops_get(sdev->d_fop);
				inode->i_rdev = to_kdev_t(dev);
				inode->i_cdev = cdget(dev);
				insert_inode_hash(inode);
				return;
			}
		}
	} else {
		/* For module nodes, the upper 16 bits of the inode number is zero indicating that
		   this is a module node; the lower 16 bits is the module id. */
		if ((modid = inode->i_no & 0x0000ffff)) {
			dev = NODEV;
			if ((sdev = (struct cdevsw *) smod_get(modid))) {
				/* initialize specfs specific fields */
				struct spec_sb_info *sbi;
				sbi = inode->i_sb->u.generic_sbp;
				inode->i_uid = sbi->setuid ? sbi->uid : current->fsuid;
				inode->i_gid = sbi->setgid ? sbi->gid : current->fsgid;
				inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
				inode->u.generic_ip = sdev;
				inode->i_mode = S_IFDIR | sbi->mode;
				inode->i_op = &spec_dir_i_ops;
#ifdef CONFIG_STREAMS_MNTSPECFS
				inode->i_fop = fops_get(&spec_dir_f_ops);
#endif
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

/*
 *  PUT_INODE
 *  -------------------------------------------------------------------------
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

/*
 *  DELETE_INODE
 *  -------------------------------------------------------------------------
 *  The delete_inode() superblock operation is called before removing an inode.  We need to detach
 *  the inode from the underlying stream head structure and initiate the close procedure on that
 *  stream head if it has no other references.
 *
 *  Module (directory) inodes do not have any stream head associated with them and therefore do not
 *  have an i_pipe pointer set.
 */
static void spec_delete_inode(struct inode *inode)
{
	struct stdata *sd;
	/* we detach the stream head from the inode */
	if ((sd = (struct stdata *) inode->i_pipe)) {
		inode->i_pipe = NULL;
		if (sd->sd_inode == inode)
			sd->sd_inode = NULL;
		sd_put(sd);
		/* we are completely detached now */
	}
	/* we must call clear_inode when we take this hook */
	clear_inode(inode);
	/* note that clear_inode() will remove the i_cdev regardless of the mode of the inode; note 
	   that i_pipe is not cleared (we did it above) */
}

/*
 *  PUT_SUPER
 *  -------------------------------------------------------------------------
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

/* basic stuff */
static int spec_statfs(struct super_block *sb, struct statfs *buf)
{
	buf->f_type = SPECFS_MAGIC;
	buf->f_bsize = 1024;
	buf->f_namelen = 255;
	return 0;
}

/*
 *  REMOUNT_FS
 *  -------------------------------------------------------------------------
 *  when we are remounted, we must check for new options
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

/*
 *  READ_SUPER
 *  -------------------------------------------------------------------------
 *  create the super block inode
 */
static struct super_block *specfs_read_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct dentry *root;
	struct spec_sb_info *sbi;
	/* need to read options on mount */
	if (!(sbi = kmem_alloc(sizeof(*sbi), KM_NOSLEEP)))
		goto fail;
	bzero(sbi, sizeof(*sbi));
	if (spec_parse_options(data, sbi) < 0 && !silent)
		goto free_fail;
	if (!(inode = new_inode(sb)))
		goto free_fail;
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
	inode->i_blocks = 0;
	inode->i_blksize = 1024;
	inode->i_uid = inode->i_gid = 0;
	inode->i_mode = S_IFDIR | S_IRUGO | S_IXUGO | S_IWUSR;
	inode->i_op = &spec_root_i_ops;
	inode->i_nlink = 2;
	sb->s_blocksize = 1024;
	sb->s_blocksize_bits = 10;
	sb->s_magic = SPECFS_MAGIC;
	sb->s_op = &spec_s_ops;
#ifdef CONFIG_STREAMS_MNTSPECFS
	inode->i_fop = &spec_root_f_ops;
	if (!(root = d_alloc_root(inode)))
		goto iput_fail;
#else
	if (!(root = d_alloc(NULL, &(const struct qstr) {
			     "spec:", 5, 0})))
		goto iput_fail;
	root->d_sb = sb;
	root->d_parent = root;
	d_instantiate(root, inode);
#endif
	root->d_op = &spec_root_d_ops;
	sb->s_root = root;
	sb->u.generic_sbp = sbi;
	return (sb);
      iput_fail:
	iput(inode);
      free_fail:
	kmem_free(sbi, sizeof(*sbi));
      fail:
	return (NULL);
}

struct vfsmount *specfs_mnt;

static DECLARE_FSTYPE(spec_fs_type, "spec", specfs_read_super, FS_SINGLE);

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

void strspecfs_exit(void)
{
	MOD_INC_USE_COUNT;	/* once for the kern mount */
	kern_umount(specfs_mnt);
	unregister_filesystem(&spec_fs_type);
}
