/*****************************************************************************

 @(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2005/05/14 08:34:42 $

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

 Last Modified $Date: 2005/05/14 08:34:42 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2005/05/14 08:34:42 $"

static char const ident[] =
    "$RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2005/05/14 08:34:42 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/compiler.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#if HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
//#include <linux/locks.h>
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
#include <linux/pagemap.h>	/* for PAGE_CACHE_SIZE */
#include <linux/mount.h>	/* for mntget and friends */

#if HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif

#include "sys/config.h"

#define SPECFS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SPECFS_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define SPECFS_REVISION		"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.42 $) $Date: 2005/05/14 08:34:42 $"
#define SPECFS_DEVICE		"SVR 4.2 Special Shadow Filesystem (SPECFS)"
#define SPECFS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SPECFS_LICENSE		"GPL"
#define SPECFS_BANNER		SPECFS_DESCRIP		"\n" \
				SPECFS_COPYRIGHT	"\n" \
				SPECFS_REVISION		"\n" \
				SPECFS_DEVICE		"\n" \
				SPECFS_CONTACT		"\n"
#define SPECFS_SPLASH		SPECFS_DEVICE		" - " \
				SPECFS_REVISION		"\n"

#ifdef CONFIG_STREAMS_MODULE
MODULE_AUTHOR(SPECFS_CONTACT);
MODULE_DESCRIPTION(SPECFS_DESCRIP);
MODULE_SUPPORTED_DEVICE(SPECFS_DEVICE);
MODULE_LICENSE(SPECFS_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("specfs");
#endif
#endif

#ifdef CONFIG_STREAMS_MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("/dev/streams");
MODULE_ALIAS("/dev/streams/*");
#endif
#endif

#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "strlookup.h"		/* for cdevsw_list */
#include "strspecfs.h"		/* for own struct spec_sb_info */

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
      minor.  The minor inode can be found using an iget() lookup by inode number calculated from
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
      dentry name to find the cdevsw table entry of the same name and creates an inode for it if one
      does not already exist.  The inode number of the directory inode is the module id.  A cdevsw
      table entry can be found by moduile id with an iget() using the module id as the inode
      number.
                                                                                                             
    - If the device dentry equivalent to the device node name does not exist, i_lookup on the module
      inode will attempt to load "/dev/stream/%s/%s" and then "streams-%s-%s" with the module and
      device instance names.  When the kernel module loads and registers the device name, the
      devnode entry will be created and the next lookup operation should succeed.  i_lookup uses the
      dentry name to find the devnode entry of the same name and creates an inode for it if one
      does not already exist.  The inode number of the device inode is the module id and the device
      instance number concatenated.  A device inode and corresponding devnode can be found with an
      iget() using the module id and instance number as the inode number.
                                                                                                             
    - If the device dentry name is numeric and no other device node can be found, then the number
      will be used as an instance number and the inode acquired using the concatentation of the
      module id and the instance number using iget().  The resulting inode will be attached to the
      dentry.  Device inodes can exist briefly without any devnode structure or dentry.  These
      device inodes are created in response to clone opens when new unique instance numbers are
      assigned that have no associated dentry or devnode.  Because a dentry is ultimately needed for
      the shadow open, to replace the original non-shadow entry, one must be created if it does not
      exist.
                                                                                                             
    --------------------------------------------------------------------------------------------------------
 */

/* we want macro versions of these */

#undef getmajor
#define getmajor(__ino) (((__ino)>>16)&0x0000ffff)

#undef getminor
#define getminor(__ino) (((__ino)>>0)&0x0000ffff)

#undef makedevice
#define makedevice(__maj,__min) ((((__maj)<<16)&0xffff0000)|(((__min)<<0)&0x0000ffff))

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
#if 0
/**
 *  spec_dev_open: - open a stream from an internal character special device, fifo or socket
 *  @inode:	internal shadow special filesystem inode
 *  @file:	file pointer (user file pointer)
 *
 *  This open procedure is only called from spec_dev_f_ops directly attached to a shadow special
 *  filesystem device inode.  Entering spec_dev_open() means that a direct open of the internal
 *  shadow special filesystem has been performed on the mounted filesystem.  We need to establish
 *  our streams arguments and nest into the devices open procedure.
 */
STATIC int spec_dev_open(struct inode *inode, struct file *file)
{
	int err;
	struct str_args args;
	struct cdevsw *cdev;
	err = -ENOENT;
	/* XXX: can the module unload in our face this way? */
	if (!(cdev = file->f_dentry->d_parent->d_inode->u.generic_ip))
		goto exit;
	args.dev = inode->i_ino;
	args.oflag = make_oflag(file);
	args.sflag = (cdev->d_flag & D_CLONE) ? CLONEOPEN : DRVOPEN;
	args.crp = current_creds;
	printd(("%s: %s: putting file operations\n", __FUNCTION__, file->f_dentry->d_name.name));
	printd(("%s: %s: getting file operations\n", __FUNCTION__, cdev->d_name));
	fops_put(xchg(&file->f_op, fops_get(cdev->d_fop)));
	err = -EIO;
	if (!file->f_op || !file->f_op->open)
		goto exit;
	file->private_data = &args;
	err = file->f_op->open(inode, file);
      exit:
	return (err);
}

struct file_operations spec_dev_f_ops = {
      owner:THIS_MODULE,
      open:&spec_dev_open,
};
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
#if HAVE_INODE_OPERATIONS_LOOKUP_NAMEIDATA
STATIC struct dentry *spec_dir_i_lookup(struct inode *dir, struct dentry *new,
					struct nameidata *dummy)
#else
STATIC struct dentry *spec_dir_i_lookup(struct inode *dir, struct dentry *new)
#endif
{
	struct cdevsw *cdev;
	ptrace(("%s: lookup dentry %p, %s in directory %p\n", __FUNCTION__, new, new->d_name.name,
		dir));
	if ((cdev = dir->u.generic_ip)) {
		struct devnode *cmin;
		/* check if the name is registered as a minor device node name */
		if ((cmin = cmin_find(cdev, new->d_name.name))) {
			struct inode *inode;
			printd(("%s: name is a device cmin\n", __FUNCTION__));
			if ((inode = cmin->n_inode)) {
				igrab(inode);
				printd(("%s: adding inode %p to dentry %p\n", __FUNCTION__, inode,
					new));
				d_add(new, inode);
				return (NULL);	/* success */
			}
			swerr();
			return ERR_PTR(-EIO);
		} else {
			/* check if the name is a valid number */
			char *tail = (char *) new->d_name.name;
			minor_t minor = simple_strtoul(tail, &tail, 0);
			if (*tail == '\0') {
				if ((minor & ~MINORMASK) == 0) {
					struct inode *inode;
					dev_t dev = makedevice(cdev->d_modid, minor);
					printd(("%s: name is a valid number\n", __FUNCTION__));
#if HAVE_KFUNC_IGET_LOCKED
					if ((inode = iget_locked(dir->i_sb, dev)))
#else
					if ((inode = iget4(dir->i_sb, dev, NULL, cdev)))
#endif
					{
#if HAVE_KFUNC_IGET_LOCKED
						if ((inode->i_state & I_NEW)) {
							inode->u.generic_ip = cdev;
							dir->i_sb->s_op->read_inode(inode);
							unlock_new_inode(inode);
						}
#endif
						printd(("%s: adding inode %p to dentry %p\n",
							__FUNCTION__, inode, new));
						d_add(new, inode);
						return (NULL);	/* success */
					}
					ptrace(("%s: can't get inode\n", __FUNCTION__));
					return ERR_PTR(-ENOMEM);
				}
			}
		}
	}
	return ERR_PTR(-ENOENT);
}

 /* TODO: we can make links in this directory.  We just have to instantiate a devnode minor device
    node structure and attach it to the cdevsw structure and assign it the appropriate minor device 
    number.  That would be quite simple. */

STATIC struct inode_operations spec_dir_i_ops = {
      lookup:spec_dir_i_lookup,
};

/* 
 *  Shadow Special Filesystem Module directory file operations.
 *  -------------------------------------------------------------------------
 */

#if 1
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
STATIC int spec_dir_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = file->f_dentry;
	int nr = file->f_pos;
	ptrace(("%s: listing device directory, dentry %p, inode %p\n", __FUNCTION__,
		dentry, dentry->d_inode));
	switch (nr) {
		struct cdevsw *cdev;
	case 0:
		if (filldir(dirent, ".", 1, nr, dentry->d_inode->i_ino, DT_DIR) < 0)
			break;
		nr++;
		file->f_pos++;
		/* fall through */
	case 1:
		if (filldir(dirent, "..", 2, nr, dentry->d_parent->d_inode->i_ino, DT_DIR) < 0)
			break;
		nr++;
		file->f_pos++;
		/* fall through */
	default:
		read_lock(&cdevsw_lock);
		if ((cdev = dentry->d_inode->u.generic_ip) && cdev->d_minors.next) {
			struct list_head *pos;
			int i = nr - 2, err = 0;
			/* skip to position */
			for (pos = cdev->d_minors.next; pos != &cdev->d_minors && i;
			     pos = pos->next, i--) ;
			/* start writing */
			for (; err >= 0 && pos != &cdev->d_minors;
			     pos = pos->next, nr++, file->f_pos++) {
				struct devnode *cmin = list_entry(pos, struct devnode, n_list);
				if (cmin->n_inode) {
					read_unlock(&cdevsw_lock);
					err = filldir(dirent, cmin->n_name,
						      strnlen(cmin->n_name, FMNAMESZ), file->f_pos,
						      cmin->n_inode->i_ino,
						      cmin->n_inode->i_mode >> 12);
					read_lock(&cdevsw_lock);
				}
			}
		}
		read_unlock(&cdevsw_lock);
#if 0
		/* TODO: do this later */
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
	return (0);
}
#endif

STATIC struct file_operations spec_dir_f_ops = {
      owner:THIS_MODULE,
      read:generic_read_dir,
      readdir:spec_dir_readdir,
};

/*
 *  Shadow Special Filesystem Directory dentry operations.
 *  -------------------------------------------------------------------------
 */
#if 0
STATIC int spec_dir_d_revalidate(struct dentry *dentry, int flags)
{
	struct inode *inode;
	ptrace(("%s: revalidating dentry %p (%s), inode %p (%ld)\n", __FUNCTION__, dentry,
		dentry->d_name.name, dentry->d_inode,
		dentry->d_inode ? dentry->d_inode->i_ino : 0));
	if (dentry == dentry->d_sb->s_root)
		goto valid;	/* root is always valid */
	if (!(inode = dentry->d_inode))
		goto invalid;
	switch (inode->i_mode & S_IFMT) {
		struct cdevsw *cdev;
		struct devnode *cmin;
	case S_IFDIR:
		/* If the inode is a directory, then the inode corresponds to a module directory
		   inode.  In this case, the driver may have unloaded while the dentry was still
		   cached.  In that case the inode no longer references the character device switch 
		   entry. */
		if ((cdev = inode->u.generic_ip) && cdev->d_inode == inode)
			goto valid;
		break;
	case S_IFCHR:
		if ((cmin = inode->u.generic_ip) && cmin->n_inode && cmin->n_inode == inode)
			goto valid;
		break;
#ifdef CONFIG_STREAMS_FIFO
	case S_IFIFO:
		break;
#endif
#ifdef CONFIG_STREAMS_SOCKSYS
	case S_IFSOCK:
		break;
#endif
	}
      invalid:
	printd(("%s: dentry %p (%s) is invalid (dropping)\n", __FUNCTION__, dentry,
		dentry->d_name.name));
	d_drop(dentry);
	return (0);
      valid:
	printd(("%s: dentry %p (%s) is valid\n", __FUNCTION__, dentry, dentry->d_name.name));
	return (1);
}
#endif

#if 0
STATIC int spec_dir_d_hash(struct dentry *dentry, struct qstr *dname)
{
	ptrace(("%s: hashing dentry %p (%s)\n", __FUNCTION__, dentry, dname->name));
	if (dname->len)
		dname->hash = full_name_hash(dname->name, dname->len);
	return (0);		/* success */
}

STATIC int spec_dir_d_compare(struct dentry *parent, struct qstr *child, struct qstr *name)
{
	ptrace(("%s: comparing dentry %p (%s), %s to %s\n", __FUNCTION__, parent,
		parent->d_name.name, name->name, child->name));
	/* Note that we also call lookup_hash() directly sometimes with a zero length name. */
	if (name->len) {
		/* This is what the system would do if we didn't take this hook: */
		if (child->len != name->len)
			return (1);	/* failed */
		if (memcmp(child->name, name->name, name->len))
			return (1);	/* failed */
		return (0);	/* success */
	}
	return (0);		/* success */
}

STATIC int spec_dir_d_delete(struct dentry *dentry)
{
	ptrace(("%s: deleting dentry %p (%s), inode %p (%ld)\n", __FUNCTION__, dentry,
		dentry->d_name.name, dentry->d_inode,
		dentry->d_inode ? dentry->d_inode->i_ino : 0));
	/* delete unused dentries instead of just dropping them */
	return (1);
}

#if 0
STATIC void spec_dir_d_iput(struct dentry *dentry, struct inode *inode)
{
	ptrace(("%s: putting inode %p (%ld), for dentry %p (%s)\n", __FUNCTION__, inode,
		inode->i_ino, dentry, dentry->d_name.name));
	iput(inode);		/* this is what the system does otherwise */
}

STATIC void spec_dir_d_release(struct dentry *dentry)
{
	ptrace(("%s: releasing dentry %p, inode %p\n", __FUNCTION__, dentry, dentry->d_inode));
	return;
}
#endif

STATIC struct dentry_operations spec_dir_d_ops ____cacheline_aligned = {
#if 0
      d_revalidate:spec_dir_d_revalidate,
#endif
      d_hash:spec_dir_d_hash,
      d_compare:spec_dir_d_compare,
      d_delete:spec_dir_d_delete,
#if 0
      d_release:spec_dir_d_release,
      d_iput:spec_dir_d_iput,
#endif
};
#endif

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
 *  simply no dentry in the dentry cache for this inode yet.  We walk through the cdevsw list
 *  looking for the dentry name.  If we find one, we lookup or create a inode with the inode number
 *  equal to the module id and return that inode for attachment to the dentry.  If the name cannot
 *  be found and the name is numeric, we check again with the module id equal to the numeric name.
 *  If we cannot find a corresponding cdevsw entry and we are configured for kernel module loading,
 *  we request a module of the name streams-%s, where %s is the name requested.
 */
#if HAVE_INODE_OPERATIONS_LOOKUP_NAMEIDATA
STATIC struct dentry *spec_root_i_lookup(struct inode *dir, struct dentry *new,
					 struct nameidata *dummy)
#else
STATIC struct dentry *spec_root_i_lookup(struct inode *dir, struct dentry *new)
#endif
{
	struct cdevsw *cdev;
	ptrace(("%s: lookup dentry %p, %s in root dir inode %p\n", __FUNCTION__, new,
		new->d_name.name, dir));
	/* this will also attempt to demand load the "streams-%s" module if required */
	if ((cdev = cdev_find(new->d_name.name))) {
		struct inode *inode;
		printd(("%s: %s: found device\n", __FUNCTION__, cdev->d_name));
		printd(("%s: name is a device or module\n", __FUNCTION__));
		if ((inode = cdev->d_inode)) {
			igrab(inode);
			cdev_put(cdev);
			printd(("%s: adding inode %p to dentry %p\n", __FUNCTION__, inode, new));
			d_add(new, inode);
			return (NULL);	/* success */
		}
		/* It must be a module.  This has the rather unwanted side-effect that searching
		   directory %s could demand load streams-%s, where streams-%s is not a driver but
		   a module. */
		cdev_put(cdev);
	}
	return ERR_PTR(-ENOENT);
}

STATIC struct inode_operations spec_root_i_ops = {
      lookup:spec_root_i_lookup,
};

/* 
 *  Shadow Special Filesystem Root file operations.
 *  -------------------------------------------------------------------------
 */

#if 1
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
STATIC int spec_root_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = file->f_dentry;
	int nr = file->f_pos;
	ptrace(("%s: listing root directory, dentry %p, inode %p\n", __FUNCTION__, dentry,
		dentry->d_inode));
	switch (nr) {
	case 0:
		if (filldir(dirent, ".", 1, nr, dentry->d_inode->i_ino, DT_DIR) < 0)
			break;
		nr++;
		file->f_pos++;
		/* fall through */
	case 1:
		if (filldir(dirent, "..", 2, nr, dentry->d_parent->d_inode->i_ino, DT_DIR) < 0)
			break;
		nr++;
		file->f_pos++;
		/* fall through */
	default:
		read_lock(&cdevsw_lock);
		if (cdevsw_list.next) {
			struct list_head *pos;
			int i = nr - 2, err = 0;
			/* skip to position */
			for (pos = cdevsw_list.next; pos != &cdevsw_list && i;
			     pos = pos->next, i--) ;
			/* start writing */
			for (; err >= 0 && pos != &cdevsw_list;
			     pos = pos->next, nr++, file->f_pos++) {
				struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_list);
				if (cdev->d_inode) {
					read_unlock(&cdevsw_lock);
					err = filldir(dirent, cdev->d_name,
						      strnlen(cdev->d_name, FMNAMESZ), file->f_pos,
						      cdev->d_inode->i_ino,
						      cdev->d_inode->i_mode >> 12);
					read_lock(&cdevsw_lock);
				}
			}
		}
		read_unlock(&cdevsw_lock);
		break;
	}
	return 0;
}
#endif

STATIC struct file_operations spec_root_f_ops = {
      owner:THIS_MODULE,
      read:generic_read_dir,
#if 1
      readdir:spec_root_readdir,
#else
      readdir:dcache_readdir,
#endif
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
STATIC int spec_root_d_revalidate(struct dentry *dentry, int flags)
{
	struct inode *inode;
	ptrace(("%s: revalidating dentry %p (%s), inode %p (%ld)\n", __FUNCTION__, dentry,
		dentry->d_name.name, dentry->d_inode,
		dentry->d_inode ? dentry->d_inode->i_ino : 0));
	if (dentry == dentry->d_sb->s_root)
		goto valid;	/* root is always valid */
	if (!(inode = dentry->d_inode))
		goto invalid;
	switch (inode->i_mode & S_IFMT) {
		struct cdevsw *cdev;
		struct devnode *cmin;
	case S_IFDIR:
		/* If the inode is a directory, then the inode corresponds to a module directory
		   inode.  In this case, the driver may have unloaded while the dentry was still
		   cached.  In that case the inode no longer references the character device switch 
		   entry. */
		if ((cdev = inode->u.generic_ip) && cdev->d_inode == inode)
			goto valid;
		break;
	case S_IFCHR:
		if ((cmin = inode->u.generic_ip) && cmin->n_inode && cmin->n_inode == inode)
			goto valid;
		break;
#ifdef CONFIG_STREAMS_FIFO
	case S_IFIFO:
		break;
#endif
#ifdef CONFIG_STREAMS_SOCKSYS
	case S_IFSOCK:
		break;
#endif
	}
      invalid:
	printd(("%s: dentry %p (%s) is invalid (dropping)\n", __FUNCTION__, dentry,
		dentry->d_name.name));
	d_drop(dentry);
	return (0);
      valid:
	printd(("%s: dentry %p (%s) is valid\n", __FUNCTION__, dentry, dentry->d_name.name));
	return (1);
}
#endif

#if 0
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
STATIC int spec_root_d_hash(struct dentry *dentry, struct qstr *dname)
{
	ptrace(("%s: hashing dentry %p (%s)\n", __FUNCTION__, dentry, dname->name));
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
STATIC int spec_root_d_compare(struct dentry *parent, struct qstr *child, struct qstr *name)
{
	ptrace(("%s: comparing dentry %p (%s), %s to %s\n", __FUNCTION__, parent,
		parent->d_name.name, name->name, child->name));
	/* Note that we also call lookup_hash() directly sometimes with a zero length name. */
	if (name->len) {
		/* This is what the system would do if we didn't take this hook: */
		if (child->len != name->len)
			return (1);	/* failed */
		if (memcmp(child->name, name->name, name->len))
			return (1);	/* failed */
		return (0);	/* success */
	}
	return (0);		/* success */
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
STATIC int spec_root_d_delete(struct dentry *dentry)
{
	ptrace(("%s: deleting dentry %p (%s), inode %p (%ld)\n", __FUNCTION__, dentry,
		dentry->d_name.name, dentry->d_inode,
		dentry->d_inode ? dentry->d_inode->i_ino : 0));
	return (1);
}
#endif

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
STATIC void spec_root_d_iput(struct dentry *dentry, struct inode *inode)
{
	ptrace(("%s: putting inode %p (%ld), for dentry %p (%s)\n", __FUNCTION__, inode,
		inode->i_ino, dentry, dentry->d_name.name));
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
STATIC void spec_root_d_release(struct dentry *dentry)
{
	ptrace(("%s: releasing dentry %p, inode %p\n", __FUNCTION__, dentry, dentry->d_inode));
	return;
}
#endif

#if 0
STATIC struct dentry_operations spec_root_d_ops ____cacheline_aligned = {
#if 0
      d_revalidate:spec_root_d_revalidate,
#endif
#if 0
      d_hash:spec_root_d_hash,
      d_compare:spec_root_d_compare,
      d_delete:spec_root_d_delete,
#endif
#if 0
      d_release:spec_root_d_release,
      d_iput:spec_root_d_iput,
#endif
};
#endif

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
STATIC INLINE struct spec_sb_info *spec_sbi_alloc(void)
{
	struct spec_sb_info *sbi;
	if ((sbi = kmalloc(sizeof(*sbi), GFP_ATOMIC))) {
		memset(sbi, 0, sizeof(*sbi));
		sbi->sbi_magic = SPEC_SBI_MAGIC;
		printd(("%s: allocated options %p\n", __FUNCTION__, sbi));
	}
	return (sbi);
}

STATIC INLINE void spec_sbi_free(struct spec_sb_info *sbi)
{
	if (sbi) {
		printd(("%s: freeing options %p\n", __FUNCTION__, sbi));
		sbi->sbi_magic = 0;
		kfree(sbi);
	}
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
STATIC INLINE int spec_parse_options(char *options, struct spec_sb_info *sbi)
{
	int err = 0;
	int setuid = 0;
	int setgid = 0;
	int setmod = 0;
	uid_t uid = 0;
	uid_t gid = 0;
	umode_t mode = 0666;
	char *this_char, *value, *token = options;
	ptrace(("%s: paring options %p\n", __FUNCTION__, sbi));
	if (!sbi || sbi->sbi_magic != SPEC_SBI_MAGIC)
		goto einval;
	while ((this_char = strsep(&token, ","))) {
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
			setmod = 1;
		} else
			goto einval;
	}
      done_options:
	sbi->sbi_setuid = setuid;
	sbi->sbi_setgid = setgid;
	sbi->sbi_setmod = setmod;
	sbi->sbi_uid = uid;
	sbi->sbi_gid = gid;
	sbi->sbi_mode = mode & ~S_IFMT;
	return (err);
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
STATIC void spec_read_inode(struct inode *inode)
{
	struct cdevsw *cdev = inode->u.generic_ip;
	ptrace(("%s: reading inode %p (%ld)\n", __FUNCTION__, inode, inode->i_ino));
	if (!cdev)
		goto bad_inode;
	else {
#if HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
		struct spec_sb_info *sbi = inode->i_sb->s_fs_info;
#elif HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
		struct spec_sb_info *sbi = inode->i_sb->u.generic_sbp;
#else
#error HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO or HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP must be defined.
#endif
		inode->i_mode = (sbi->sbi_mode & ~S_IFMT);
		if (!sbi->sbi_setmod)
			inode->i_mode &= ~current->fs->umask;
		inode->i_uid = sbi->sbi_setuid ? sbi->sbi_uid : current->fsuid;
		inode->i_gid = sbi->sbi_setgid ? sbi->sbi_gid : current->fsgid;
	}
	if (getmajor(inode->i_ino)) {
		dev_t dev = MKDEV(cdev->d_major, getminor(inode->i_ino) & MINORMASK);
		/* for device nodes, the major component of the i_ino is the module id */
		printd(("%s: reading device node %p (%ld)\n", __FUNCTION__, inode, inode->i_ino));
		inode->i_mode |= (cdev->d_mode & S_IFMT) ? (cdev->d_mode & S_IFMT) : S_IFCHR;
		inode->i_mode &= ~S_IXUGO;
		inode->i_op = NULL;
		inode->i_fop = cdev->d_fop;
#if HAVE_KFUNC_TO_KDEV_T
		inode->i_rdev = to_kdev_t(dev);
#else
		inode->i_rdev = dev;
#endif
		// inode->i_op = &spec_dev_i_ops; /* leave at empty_iops */
		// inode->i_fop = &spec_dev_f_ops;
	} else {
		/* for module nodes, the minor component of the i_ino is the module id */
		printd(("%s: reading driver node %p (%ld)\n", __FUNCTION__, inode, inode->i_ino));
		inode->i_mode |= S_IFDIR;
		inode->i_mode |= ((inode->i_mode & S_IRUGO) >> 2);
		inode->i_op = &spec_dir_i_ops;
		inode->i_fop = &spec_dir_f_ops;
	}
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
	return;
      bad_inode:
	pswerr(("%s: couldn't read inode %p (%ld)\n", __FUNCTION__, inode, inode->i_ino));
	make_bad_inode(inode);
	return;
}

#if HAVE_KMEMB_STRUCT_SUPER_OPERATIONS_READ_INODE2
/**
 *  spec_read_inode2: - read a inode from the filesystem
 *  @inode:	initialized inode to read
 *
 *  The read_inode2() method is called on the superblock when we cannot find an inode by inode number
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
STATIC void spec_read_inode2(struct inode *inode, void *opaque)
{
	inode->u.generic_ip = opaque;
	return spec_read_inode(inode);
}
#endif

/**
 *  spec_put_inode: - put an inode to the filesystem
 *  @inode:	inode to put
 *
 *  The put_inode() superblock operation is called whenever performing iput() on an inode, and
 *  before checking i_nlink on an inode if the i_count has dropped to zero.  This is our chance to
 *  set the number of links to zero so that the inode will just be destroyed (and delete_inode()
 *  below will be called as well).  A force delete here will just set i_nlink to zero if the
 *  i_count is at 1.
 */
STATIC void spec_put_inode(struct inode *inode)
{
	ptrace(("%s: putting inode %p (%ld), i_count %d, i_nlink %d\n", __FUNCTION__,
		inode, inode->i_ino, atomic_read(&inode->i_count),
		inode->i_nlink));
	if (inode->i_nlink != 0) {
		pswerr(("%s: number of links non-zero: i_nlink = %d, force deleting\n",
			__FUNCTION__, inode->i_nlink));
		/* We shouldn't have been force deleting for 2.4 anyway.  We should let just let
		   the inode be forgotten.  I need to think this out a bit more */
#if HAVE_KFUNC_FORCE_DELETE
		force_delete(inode);
#else
		if (atomic_read(&inode->i_count) == 1)
			inode->i_nlink = 0;
#endif
	}
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
STATIC void spec_delete_inode(struct inode *inode)
{
	ptrace(("%s: deleting inode %p (%ld)\n", __FUNCTION__, inode, inode->i_ino));
	switch (inode->i_mode & S_IFMT) {
		// struct cdevsw *cdev;
		// struct devnode *cmin;
	case S_IFDIR:
		/* directory inodes potentially have a cdevsw structure hanging off of the
		   u.generic_ip pointer, these are for sanity checks only. When we referemce the
		   inode from the module structure, we hold a reference count on the inode.  We
		   should never get here with either the u.generic_ip pointer set or the d_inode
		   reference still held.  Forced deletions might get us here anyway. */
		if (inode->u.generic_ip)
			inode->u.generic_ip = NULL;
		break;
	case S_IFCHR:
		/* character special device inodes potentially have a minor devnode structure
		   hanging off of the u.generic_ip pointer and stream heads hanging off of the
		   i_pipe pointer. When we referemce the inode from the devnode structure, we hold
		   a reference count on the inode.  We should never get here with either the
		   u.generic_ip pointer set or the n_inode reference still held.  Forced deletions
		   might get us here anyway. */
		if (inode->u.generic_ip)
			inode->u.generic_ip = NULL;
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
	/* well, we clear i_pipe in spec_clear_inode below */
}

/**
 *  spec_put_super: - put a superblock for the filesystem
 *  @sb:	super block to put
 *
 *  When we put a super block away, we must detach the options structure from the superblock and
 *  free it.
 */
STATIC void spec_put_super(struct super_block *sb)
{
	ptrace(("%s: putting superblock %p\n", __FUNCTION__, sb));
	/* just free our optional mount information */
#if HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
	spec_sbi_free(xchg(&sb->s_fs_info, NULL));
#elif HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
	spec_sbi_free(xchg(&sb->u.generic_sbp, NULL));
#endif
}

#define SPECFS_MAGIC 0xDEADBEEF

/**
 *  spec_statfs: - provide statfs information
 *  @sb:	super block
 *  @buf:	buffer for statfs data
 */
#if HAVE_KMEMB_STRUCT_KSTATFS_F_TYPE
STATIC int spec_statfs(struct super_block *sb, struct kstatfs *buf)
#else
STATIC int spec_statfs(struct super_block *sb, struct statfs *buf)
#endif
{
	(void) sb;
	ptrace(("%s: stat filesystem superblock %p\n", __FUNCTION__, sb));
	buf->f_type = sb->s_magic;
	buf->f_bsize = PAGE_CACHE_SIZE;
	buf->f_namelen = NAME_MAX;
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
STATIC int spec_remount_fs(struct super_block *sb, int *flags, char *data)
{
#if HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
	struct spec_sb_info *sbi = sb->s_fs_info;
#elif HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
	struct spec_sb_info *sbi = sb->u.generic_sbp;
#endif
	(void) flags;
	ptrace(("%s: remouting superblock %p\n", __FUNCTION__, sb));
	return spec_parse_options(data, sbi);
}

/**
 *  spec_clear_inode: - clear filesystem specific junk out of inode
 *  @inode:	inode to clear
 *
 *  This is called before bd_forget or cdput are called on i_bdev and i_cdev.  This means that if we
 *  want to overload those pointers within the specfs, we need to clear them here before vfs
 *  mistakes those pointers as block or character device pointers.  We use i_pipe, but let's clear
 *  it here too.
 */
STATIC void spec_clear_inode(struct inode *inode)
{
	ptrace(("%s: cleaning inode %p (%ld)\n", __FUNCTION__, inode, inode->i_ino));
	if (inode->i_pipe) {
#if 0
		sd_put((struct stdata *) inode->i_pipe);
#endif
		swerr();
		inode->i_pipe = NULL;
	}
}

STATIC void spec_umount_begin(struct super_block *sb)
{
	ptrace(("%s: unmounting with force, superblock %p\n", __FUNCTION__, sb));
	(void) sb;
	return;
}

STATIC struct super_operations spec_s_ops ____cacheline_aligned = {
	.read_inode = spec_read_inode,
#if HAVE_KMEMB_STRUCT_SUPER_OPERATIONS_READ_INODE2
	.read_inode2 = spec_read_inode2,
#endif
//      .dirty_inode = NULL,
//      .write_inode = NULL,
	.put_inode = spec_put_inode,
	.delete_inode = spec_delete_inode,
	.put_super = spec_put_super,
//      .write_super = NULL,
//      .write_super_lockfs = NULL,
//      .unlockfs = NULL,
	.statfs = spec_statfs,	/* like simple_statfs */
	.remount_fs = spec_remount_fs,
	.clear_inode = spec_clear_inode,
	.umount_begin = spec_umount_begin,
};

/* TODO: handle extended attributes */
/**
 *  specfs_read_super: - create a shadow special filesystem super block
 *  @sb:	superblock for which to read a superblock inode
 *  @data:	options data
 *  @silent:	whether to ignore options errors
 *
 *  Create the super block inode.
 */
STATIC int specfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct spec_sb_info *sbi;
	int err;
	ptrace(("%s: filling superblock\n", __FUNCTION__));
	sb->s_blocksize = 1024;
	sb->s_blocksize_bits = 10;
	sb->s_magic = SPECFS_MAGIC;
	sb->s_op = &spec_s_ops;
	err = -ENOMEM;
	/* need to read options on mount */
	if (!(sbi = spec_sbi_alloc()))
		goto error;
	if ((err = spec_parse_options(data, sbi) < 0 && !silent))
		goto free_error;
	err = -ENOMEM;
	if (!(inode = new_inode(sb)))
		goto free_error;
	inode->i_ino = -1UL;	/* unused (non-zero) inode number */
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
	inode->i_blocks = 0;
	inode->i_blksize = 1024;
	inode->i_uid = inode->i_gid = 0;
	inode->i_mode = S_IFDIR | S_IRUGO | S_IXUGO | S_IWUSR;
	inode->i_op = &spec_root_i_ops;
	inode->i_fop = &spec_root_f_ops;
	inode->i_nlink = 2;
	if (!(sb->s_root = d_alloc_root(inode)))
		goto iput_error;
#if 0
	sb->s_root->d_op = &spec_root_d_ops;
#endif
#if HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
	sb->s_fs_info = sbi;
#elif HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
	sb->u.generic_sbp = sbi;
#endif
	return (0);
      iput_error:
	iput(inode);
      free_error:
	spec_sbi_free(sbi);
      error:
	return (err);
}

#if HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_GET_SB
STATIC struct super_block *specfs_get_sb(struct file_system_type *fs_type, int flags,
					 const char *dev_name, void *data)
{
	return get_sb_single(fs_type, flags, data, specfs_fill_super);
}
STATIC void specfs_kill_sb(struct super_block *sb)
{
	return kill_anon_super(sb);
}

struct file_system_type spec_fs_type = {
	.owner = THIS_MODULE,
	.name = "specfs",
	.get_sb = specfs_get_sb,
	.kill_sb = specfs_kill_sb,
};
#elif HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_READ_SUPER
/**
 *  specfs_read_super: - create a shadow special filesystem super block
 *  @sb:	superblock for which to read a superblock inode
 *  @data:	options data
 *  @silent:	whether to ignore options errors
 *
 *  Create the super block inode.
 */
STATIC struct super_block *specfs_read_super(struct super_block *sb, void *data, int silent)
{
	ptrace(("%s: reading superblock\n", __FUNCTION__));
	return (specfs_fill_super(sb, data, silent) ? NULL : sb);
}

STATIC DECLARE_FSTYPE(spec_fs_type, "specfs", specfs_read_super, FS_SINGLE | FS_LITTER);
#else
#error HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_GET_SB or HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_READ_SUPER must be defined.
#endif

STATIC struct vfsmount *specfs_mnt = NULL;
STATIC spinlock_t specfs_lock = SPIN_LOCK_UNLOCKED;

struct vfsmount *specfs_get(void)
{
	spin_lock(&specfs_lock);
	if (!specfs_mnt) {
		if (IS_ERR(specfs_mnt = kern_mount(&spec_fs_type)))
			specfs_mnt = NULL;
	}
	spin_unlock(&specfs_lock);
	return (mntget(specfs_mnt));
}

#if ! HAVE_KFUNC_KERN_UMOUNT
#undef kern_umount
#define kern_umount mntput
#endif

void specfs_put(void)
{
	if (specfs_mnt) {
		mntput(specfs_mnt);
		spin_lock(&specfs_lock);
		if (atomic_read(&specfs_mnt->mnt_count) == 1)
			kern_umount(xchg(&specfs_mnt, NULL));
		spin_unlock(&specfs_lock);
	}
}

/**
 *  strspecfs_init: - initialize the shadow special filesystem
 */
#ifdef CONFIG_STREAMS_MODULE
static
#endif
int __init specfs_init(void)
{
	int result;
#ifdef CONFIG_STREAMS_MODULE
	printk(KERN_INFO SPECFS_BANNER);	/* log message */
#else
	printk(KERN_INFO SPECFS_SPLASH);	/* console splash */
#endif
	ptrace(("%s: registering filesystem\n", __FUNCTION__));
	if ((result = register_filesystem(&spec_fs_type)))
		goto no_specfs;
	if ((result = strlookup_init()))
		goto no_lookup;
	return (0);
      no_lookup:
	unregister_filesystem(&spec_fs_type);
      no_specfs:
	return (result);
}

/**
 *  strspecfs_exit: - deinitialize the shadow special filesystem
 */
#ifdef CONFIG_STREAMS_MODULE
static
#endif
void __exit specfs_exit(void)
{
	strlookup_exit();
	ptrace(("%s: unregistering filesystem\n", __FUNCTION__));
	unregister_filesystem(&spec_fs_type);
}

#ifdef CONFIG_STREAMS_MODULE
module_init(specfs_init);
module_exit(specfs_exit);
#endif
