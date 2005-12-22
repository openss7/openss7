/*****************************************************************************

 @(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.66 $) $Date: 2005/12/22 10:28:43 $

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

 Last Modified $Date: 2005/12/22 10:28:43 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.66 $) $Date: 2005/12/22 10:28:43 $"

static char const ident[] =
    "$RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.66 $) $Date: 2005/12/22 10:28:43 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/compiler.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#if defined HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
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

#include <linux/kernel.h>	/* for simple_strtoul, FASTCALL(), fastcall */
#include <linux/pagemap.h>	/* for PAGE_CACHE_SIZE */
#if defined HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>	/* for lookup_hash on 2.6 */
#endif
#include <linux/mount.h>	/* for mntget and friends */

#if defined HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif

#include "sys/strdebug.h"

#if ! defined HAVE_KFUNC_MODULE_PUT
#define module_refcount(__m) atomic_read(&(__m)->uc.usecount)
#endif

#include "sys/config.h"

#define SPECFS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SPECFS_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define SPECFS_REVISION		"LfS $RCSfile: strspecfs.c,v $ $Name:  $($Revision: 0.9.2.66 $) $Date: 2005/12/22 10:28:43 $"
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

STATIC struct vfsmount *specfs_mnt = NULL;

/*
 *  -------------------------------------------------------------------------
 *  -------------------------------------------------------------------------
 */

BIG_STATIC struct inode *
spec_snode(dev_t dev, struct cdevsw *cdev)
{
	struct inode *snode;
	struct super_block *sb;

	assert(specfs_mnt);
	sb = specfs_mnt->mnt_sb;

#if defined HAVE_KFUNC_IGET_LOCKED
	if (!(snode = iget_locked(sb, dev))) {
		ptrace(("couldn't allocate inode\n"));
		return ERR_PTR(-ENOMEM);
	}
	if (snode->i_state & I_NEW) {
		snode->u.generic_ip = cdev;
		sb->s_op->read_inode(snode);
		unlock_new_inode(snode);
	}
#else
	if (!(snode = iget4(sb, dev, NULL, cdev))) {
		ptrace(("couldn't allocate inode\n"));
		return ERR_PTR(-ENOMEM);
	}
#endif
	_ptrace(("inode %p no %lu refcount now %d\n", snode, snode->i_ino,
		 atomic_read(&snode->i_count)));
	return (snode);
}

streams_fastcall int
spec_reparent(struct file *file, struct cdevsw *cdev, dev_t dev)
{
	struct inode *snode;
	struct dentry *dentry;
	struct vfsmount *mnt;
	int err;

#if defined HAVE_FILE_MOVE_ADDR
	typeof(&file_move) _file_move = (typeof(_file_move)) HAVE_FILE_MOVE_ADDR;
#define file_move(__f, __l) _file_move(__f, __l)
#endif
	if (!(mnt = mntget(specfs_mnt))) {
		ptrace(("Error path taken!\n"));
		return (-ENODEV);
	}
	{
		struct devnode *cmin;
		struct qstr name;
		char buf[25];

		name.name = buf;
		if ((cmin = cmin_get(cdev, getminor(dev))))
			name.len = snprintf(buf, sizeof(buf), "STR %s/%s",
					    cdev->d_name, cmin->n_name);
		else
			name.len = snprintf(buf, sizeof(buf), "STR %s/%lu",
					    cdev->d_name, getminor(dev));

		if (!(dentry = d_alloc(NULL, &name))) {
			ptrace(("Error path taken!\n"));
			err = -ENOMEM;
			goto mnt_error;
		}
		dentry->d_sb = mnt->mnt_sb;
		dentry->d_parent = dentry;
	}

	if (IS_ERR((snode = spec_snode(dev, cdev)))) {
		ptrace(("Error path taken!\n"));
		err = PTR_ERR(snode);
		goto put_error;
	}
	d_instantiate(dentry, snode);

	err = -ENXIO;
	if (!snode->i_fop || !snode->i_fop->open) {
		ptrace(("Error path taken!\n"));
		goto put_error;
	}

	{
		struct file_operations *f_op;

		if (!(f_op = fops_get(snode->i_fop))) {
			ptrace(("Error path taken!\n"));
			goto put_error;
		}
#ifdef CONFIG_STREAMS_DEBUG
		if (f_op->owner)
			_printd(("%s: [%s] new f_ops count is now %d\n", __FUNCTION__,
				 f_op->owner->name, module_refcount(f_op->owner)));
		else
			_printd(("%s: new f_ops have no owner!\n", __FUNCTION__));
#endif
#ifdef CONFIG_STREAMS_DEBUG
		if (file->f_op->owner)
			_printd(("%s: [%s] old f_ops count is now %d\n", __FUNCTION__,
				 file->f_op->owner->name, module_refcount(file->f_op->owner) - 1));
		else
			_printd(("%s: old f_ops have no owner!\n", __FUNCTION__));
#endif
		fops_put(file->f_op);
		file->f_op = f_op;
	}
	dput(file->f_dentry);
	file->f_dentry = dentry;
	mntput(file->f_vfsmnt);
	file->f_vfsmnt = mnt;
	file_move(file, &mnt->mnt_sb->s_files);
	return (0);

      put_error:
	dput(dentry);
      mnt_error:
	mntput(mnt);
	return (err);
}

#if defined CONFIG_STREAMS_STH_MODULE || defined CONFIG_STREAMS_CLONE_MODULE || defined CONFIG_STREAMS_NSDEV_MODULE \
         || !defined CONFIG_STREAMS_STH || !defined CONFIG_STREAMS_CLONE || !defined CONFIG_STREAMS_NSDEV
EXPORT_SYMBOL_NOVERS(spec_reparent);
#endif

streams_fastcall int
spec_open(struct file *file, struct cdevsw *cdev, dev_t dev, int sflag)
{
	struct dentry *dentry;
	struct vfsmount *mnt;
	int err;

	/* I wish dentry_open() would use what's in the file pointer instead of what it has in
	   local variables */
	dentry = dget(file->f_dentry);
	mnt = mntget(file->f_vfsmnt);

	if (!(err = spec_reparent(file, cdev, dev))) {
		file->f_flags =
		    (sflag == CLONEOPEN) ? (file->f_flags | O_CLONE) : (file->f_flags & ~O_CLONE);
		err = file->f_op->open(file->f_dentry->d_inode, file);
		file->f_flags &= ~O_CLONE;
		if (!err) {
			dput(dentry);
			mntput(mnt);
			return (0);

		} else
			ptrace(("Error path taken!\n"));
	} else
		ptrace(("Error path taken!\n"));

	dput(file->f_dentry);
	mntput(file->f_vfsmnt);
	return (err > 0 ? -err : err);
}

#if defined CONFIG_STREAMS_STH_MODULE || defined CONFIG_STREAMS_CLONE_MODULE || defined CONFIG_STREAMS_NSDEV_MODULE \
         || !defined CONFIG_STREAMS_STH || !defined CONFIG_STREAMS_CLONE || !defined CONFIG_STREAMS_NSDEV
EXPORT_SYMBOL_NOVERS(spec_open);
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
 *
 *  Directory inodes can exist after the module has been unloaded, so we cannot use the u.generic_ip
 *  pointer, we need to get the cdev entry from the inode number again.
 */
#if defined HAVE_INODE_OPERATIONS_LOOKUP_NAMEIDATA
STATIC struct dentry *
spec_dir_i_lookup(struct inode *dir, struct dentry *new, struct nameidata *dummy)
#else
STATIC struct dentry *
spec_dir_i_lookup(struct inode *dir, struct dentry *new)
#endif
{
	struct cdevsw *cdev;

	_ptrace(("looking up %s\n", new->d_name.name));
	if ((cdev = cdrv_get(getminor(dir->i_ino)))) {
		struct devnode *cmin;

		_ptrace(("found cdev %s\n", cdev->d_name));
		/* check if the name is registered as a minor device node name */
		if ((cmin = cmin_find(cdev, new->d_name.name))) {
			struct inode *inode;

			_ptrace(("found cmin %s\n", cmin->n_name));
			if ((inode = cmin->n_inode)) {
				_ptrace(("found inode for cmin %s\n", cmin->n_name));
				igrab(inode);
				_ptrace(("inode %p no %lu refcount now %d\n", inode, inode->i_ino,
					 atomic_read(&inode->i_count)));
				d_add(new, inode);
				ctrace(cdrv_put(cdev));
				return (NULL);	/* success */
			}
			_ptrace(("no inode for cmin %s\n", cmin->n_name));
			ctrace(cdrv_put(cdev));
			return ERR_PTR(-EIO);
		} else {
			/* check if the name is a valid number */
			char *tail = (char *) new->d_name.name;
			minor_t minor = simple_strtoul(tail, &tail, 10);	/* base 10 only! */

			if (*tail == '\0') {
				if ((minor & ~MINORMASK) == 0) {
					struct inode *inode;
					dev_t dev = makedevice(cdev->d_modid, minor);

					_ptrace(("looking up inode for number %s\n",
						 new->d_name.name));
					if (IS_ERR((inode = spec_snode(dev, cdev)))) {
						_ptrace(("no inode for number %s\n",
							 new->d_name.name));
						ctrace(cdrv_put(cdev));
						return ((struct dentry *) inode);
						/* already contains error */
					}
					_ptrace(("found inode for number %s\n", new->d_name.name));
					d_add(new, inode);
					ctrace(cdrv_put(cdev));
					return (NULL);	/* success */
				}
			}
		}
		_ptrace(("no inode for %s\n", new->d_name.name));
		ctrace(cdrv_put(cdev));
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
STATIC int
spec_dir_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = file->f_dentry;
	int nr = file->f_pos;

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
		/* Cannot take cdevsw structure pointer from u.generic_ip because the directory
		   inode might exist even though the module is unloaded.  So we need to look it up
		   again, but without locks... */
		if ((cdev = cdrv_get(getminor(dentry->d_inode->i_ino)))) {
			int i = nr - 2, err = 0;

			read_lock(&cdevsw_lock);
			if (cdev->d_minors.next && !list_empty(&cdev->d_minors)) {
				struct list_head *pos;

				/* skip to position */
				for (pos = cdev->d_minors.next; pos != &cdev->d_minors && i;
				     pos = pos->next, i--) ;
				/* start writing */
				for (; err >= 0 && pos != &cdev->d_minors;
				     pos = pos->next, nr++, file->f_pos++) {
					struct devnode *cmin =
					    list_entry(pos, struct devnode, n_list);

					if (cmin->n_inode == NULL)
						continue;

					read_unlock(&cdevsw_lock);	/* Not safe */
					err = filldir(dirent, cmin->n_name,
						      strnlen(cmin->n_name, FMNAMESZ),
						      file->f_pos, cmin->n_inode->i_ino,
						      cmin->n_inode->i_mode >> 12);
					read_lock(&cdevsw_lock);
				}
			}
			/* walk the active stream head list */
			if (cdev->d_stlist.next && !list_empty(&cdev->d_stlist)) {
				struct list_head *pos;

				/* skil to position */
				for (pos = cdev->d_stlist.next; pos != &cdev->d_stlist && i;
				     pos = pos->next, i--) ;
				for (; err >= 0 && pos != &cdev->d_stlist;
				     pos = pos->next, nr++, file->f_pos++) {
					struct stdata *sd = list_entry(pos, struct stdata, sd_list);
					char numstr[24];
					int len;

					if (sd->sd_inode == NULL)
						continue;

					len = snprintf(numstr, sizeof(numstr),
						       "%lu", getminor(sd->sd_inode->i_ino));

					read_unlock(&cdevsw_lock);	/* Not safe */
					err = filldir(dirent, numstr, len,
						      file->f_pos,
						      sd->sd_inode->i_ino,
						      sd->sd_inode->i_mode >> 12);
					read_lock(&cdevsw_lock);
				}
			}
			read_unlock(&cdevsw_lock);
			ctrace(cdrv_put(cdev));
		}
		break;
	}
	return (0);
}

STATIC struct file_operations spec_dir_f_ops = {
	.owner = THIS_MODULE,
	.read = generic_read_dir,
	.readdir = spec_dir_readdir,
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
 *  simply no dentry in the dentry cache for this inode yet.  We walk through the cdevsw list
 *  looking for the dentry name.  If we find one, we lookup or create a inode with the inode number
 *  equal to the module id and return that inode for attachment to the dentry.  If the name cannot
 *  be found and the name is numeric, we check again with the module id equal to the numeric name.
 *  If we cannot find a corresponding cdevsw entry and we are configured for kernel module loading,
 *  we request a module of the name streams-%s, where %s is the name requested.
 */
#if defined HAVE_INODE_OPERATIONS_LOOKUP_NAMEIDATA
STATIC struct dentry *
spec_root_i_lookup(struct inode *dir, struct dentry *new, struct nameidata *dummy)
#else
STATIC struct dentry *
spec_root_i_lookup(struct inode *dir, struct dentry *new)
#endif
{
	struct cdevsw *cdev;

	_ptrace(("looking up %s\n", new->d_name.name));
	/* this will also attempt to demand load the "streams-%s" module if required */
	if ((cdev = cdev_find(new->d_name.name))) {
		struct inode *inode;

		_ptrace(("found cdev %s\n", cdev->d_name));
		if ((inode = cdev->d_inode)) {
			_ptrace(("found inode for cdev %s\n", cdev->d_name));
			igrab(inode);
			_ptrace(("inode %p no %lu refcount now %d\n", inode, inode->i_ino,
				 atomic_read(&inode->i_count)));
			ctrace(sdev_put(cdev));
			d_add(new, inode);
			return (NULL);	/* success */
		}
		/* It must be a module.  This has the rather unwanted side-effect that searching
		   directory %s could demand load streams-%s, where streams-%s is not a driver but
		   a module. */
		_ptrace(("no inode for cdev %s\n", cdev->d_name));
		ctrace(sdev_put(cdev));
	} else
		_ptrace(("no cdev for %s\n", new->d_name.name));
	return ERR_PTR(-ENOENT);
}

STATIC struct inode_operations spec_root_i_ops = {
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
STATIC int
spec_root_readdir(struct file *file, void *dirent, filldir_t filldir)
{
	struct dentry *dentry = file->f_dentry;
	int nr = file->f_pos;

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

STATIC struct file_operations spec_root_f_ops = {
	.owner = THIS_MODULE,
	.read = generic_read_dir,
	.readdir = spec_root_readdir,
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
STATIC struct spec_sb_info *
spec_sbi_alloc(void)
{
	struct spec_sb_info *sbi;

	if ((sbi = kmalloc(sizeof(*sbi), GFP_ATOMIC))) {
		memset(sbi, 0, sizeof(*sbi));
		sbi->sbi_magic = SPEC_SBI_MAGIC;
	}
	return (sbi);
}

STATIC void
spec_sbi_free(struct spec_sb_info *sbi)
{
	if (sbi) {
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
STATIC int
spec_parse_options(char *options, struct spec_sb_info *sbi)
{
	int err = 0;
	int setuid = 0;
	int setgid = 0;
	int setmod = 0;
	uid_t uid = 0;
	uid_t gid = 0;
	umode_t mode = 0777;
	char *this_char, *value, *token = options;

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
STATIC void
spec_read_inode(struct inode *inode)
{
	struct cdevsw *cdev = inode->u.generic_ip;

	/* generic_ip is just another way of passing another argument to iget() */
	_printd(("%s: reading inode %p no %lu\n", __FUNCTION__, inode, inode->i_ino));
	if (!cdev)
		goto bad_inode;
	else {
#if defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
		struct spec_sb_info *sbi = inode->i_sb->s_fs_info;
#elif defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
		struct spec_sb_info *sbi = inode->i_sb->u.generic_sbp;
#else
#error HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO or HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP must be defined.
#endif
		inode->i_mode = (sbi->sbi_mode & ~S_IFMT);
		inode->i_uid = sbi->sbi_uid;
		inode->i_gid = sbi->sbi_gid;
	}
	if (getmajor(inode->i_ino)) {
		dev_t dev = MKDEV(cdev->d_major, getminor(inode->i_ino) & MINORMASK);

		/* XXX: To handle the special case of clone devices, where the minor component of
		   the internal device number is the module identifier of the major device to open,
		   check if cdev has a minor device with a (non-zero) module id the same as the
		   minor device component.  If one exists, use the n_minor component of that device.
		   What is done right now is to simply use the external major device number as the
		   internal minor number.  Unfortunately, this means that all clone drivers need an
		   external major device number. */
		/* XXX: Another approach is to make module identifiers to major nodes equal to the
		   external major device number and the point would be moot. */
		_printd(("%s: reading minor device inode %lu\n", __FUNCTION__,
			 (ulong) getminor(inode->i_ino)));
		/* for device nodes, the major component of the i_ino is the module id */
		inode->i_mode |= (cdev->d_mode & S_IFMT) ? (cdev->d_mode & S_IFMT) : S_IFCHR;
		inode->i_mode &= ~S_IXUGO;
#if defined HAVE_KFUNC_TO_KDEV_T
		inode->i_rdev = to_kdev_t(dev);
#else
		inode->i_rdev = dev;
#endif
		inode->i_op = inode->i_op;	/* leave at empty_iops */
		inode->i_fop = cdev->d_fop;
	} else {
		_printd(("%s: reading major device inode %lu\n", __FUNCTION__,
			 (ulong) getminor(inode->i_ino)));
		/* for module nodes, the minor component of the i_ino is the module id */
		inode->i_mode |= S_IFDIR;
		inode->i_mode |= ((inode->i_mode & S_IRUGO) >> 2);
		inode->i_op = &spec_dir_i_ops;
		inode->i_fop = &spec_dir_f_ops;
		inode->i_nlink = 2;
	}
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
	inode->u.generic_ip = NULL;	/* done with it */
	return;
      bad_inode:
	ptrace(("bad inode no %lu\n", inode->i_ino));
	make_bad_inode(inode);
	return;
}

#if defined HAVE_KMEMB_STRUCT_SUPER_OPERATIONS_READ_INODE2
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
STATIC void
spec_read_inode2(struct inode *inode, void *opaque)
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
STATIC void
spec_put_inode(struct inode *inode)
{
	/* all specfs inodes self destruct when put for the last time */
	_printd(("%s: put of inode %p no %lu\n", __FUNCTION__, inode, inode->i_ino));
	_printd(("%s: inode %p no %lu refcount now %d\n", __FUNCTION__, inode, inode->i_ino,
		 atomic_read(&inode->i_count) - 1));
	if (inode->i_nlink != 0) {
		/* This should not happen because we decrement i_nlink when we remove the STREAM
		   head.  The inode should never be able to go away with a STREAM head attached. */
#if defined HAVE_KFUNC_FORCE_DELETE
		force_delete(inode);
#else
		if (atomic_read(&inode->i_count) == 1) {
			_printd(("%s: final put of inode %p no %lu\n", __FUNCTION__, inode,
				 inode->i_ino));
			inode->i_nlink = 0;
		}
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
STATIC void
spec_delete_inode(struct inode *inode)
{
	_printd(("%s: deleting inode %p no %lu\n", __FUNCTION__, inode, inode->i_ino));
	switch (inode->i_mode & S_IFMT) {
	case S_IFDIR:
		/* directory inodes potentially have a cdevsw structure hanging off of the
		   u.generic_ip pointer, these are for sanity checks only. When we referemce the
		   inode from the module structure, we hold a reference count on the inode.  We
		   should never get here with either the u.generic_ip pointer set or the d_inode
		   reference still held.  Forced deletions might get us here anyway. */
		assert(inode->u.generic_ip == NULL);
		break;
	case S_IFCHR:
		/* character special device inodes potentially have a minor devnode structure
		   hanging off of the u.generic_ip pointer and stream heads hanging off of the
		   i_pipe pointer. When we reference the inode from the devnode structure, we hold
		   a reference count on the inode.  We should never get here with either the
		   u.generic_ip pointer set or the n_inode reference still held.  Forced deletions
		   might get us here anyway. */
		assert(inode->u.generic_ip == NULL);
		/* fall through */
	default:
	case S_IFIFO:
		/* unnamed pipe inodes potentially have stream heads hanging off of the i_pipe
		   pointer and nothing hanging off of the u.generic_ip pointer. */
	case S_IFSOCK:
		/* socket inodes potentially have stream heads hanging off of the i_pipe pointer
		   and nothing hanging off of the u.generic_ip_pointer. */
		/* When we referemce the inode from the stdata structure, we hold a reference count 
		   on the inode.  We should never get here with the the sd_inode reference still
		   held.  Forced deletions might get us here anyway. */

		/* otherwise bad things will happen in clear_inode() */
		assert(inode->i_pipe == NULL);
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
STATIC void
spec_put_super(struct super_block *sb)
{
	/* just free our optional mount information */
#if defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
	spec_sbi_free(sb->s_fs_info);
	sb->s_fs_info = NULL;
#elif defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
	spec_sbi_free(sb->u.generic_sbp);
	sb->u.generic_sbp = NULL;
#endif
}

#define SPECFS_MAGIC 0xDEADBEEF

/**
 *  spec_statfs: - provide statfs information
 *  @sb:	super block
 *  @buf:	buffer for statfs data
 */
#if defined HAVE_KMEMB_STRUCT_KSTATFS_F_TYPE
STATIC int
spec_statfs(struct super_block *sb, struct kstatfs *buf)
#else
STATIC int
spec_statfs(struct super_block *sb, struct statfs *buf)
#endif
{
	(void) sb;
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
STATIC int
spec_remount_fs(struct super_block *sb, int *flags, char *data)
{
#if defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
	struct spec_sb_info *sbi = sb->s_fs_info;
#elif defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
	struct spec_sb_info *sbi = sb->u.generic_sbp;
#endif
	(void) flags;
	return (data ? spec_parse_options(data, sbi) : 0);
}

#if defined HAVE_KMEMB_STRUCT_INODE_I_LOCK
#define stri_trylock(__i)   (int)({ spin_lock(&(__i)->i_lock); 0; })
#define stri_lock(__i)	    spin_lock(&(__i)->i_lock)
#define stri_unlock(__i)    spin_unlock(&(__i)->i_lock);
#else
#define stri_trylock(__i)   down_interruptible(&(__i)->i_sem)
#define stri_lock(__i)	    down(&(__i)->i_sem)
#define stri_unlock(__i)    up(&(__i)->i_sem)
#endif

/**
 *  spec_clear_inode: - clear filesystem specific junk out of inode
 *  @inode:	inode to clear
 *
 *  This is called before bd_forget or cdput are called on i_bdev and i_cdev.  This means that if we
 *  want to overload those pointers within the specfs, we need to clear them here before vfs
 *  mistakes those pointers as block or character device pointers.  We use i_pipe, but let's clear
 *  it here too.
 */
STATIC void
spec_clear_inode(struct inode *inode)
{
	/* Never adjust i_pipe without taking the inode semaphore. */
	stri_lock(inode);
	if (inode->i_pipe)
		inode->i_pipe = NULL;
	stri_unlock(inode);
}

STATIC void
spec_umount_begin(struct super_block *sb)
{
	(void) sb;
	return;
}

STATIC struct super_operations spec_s_ops ____cacheline_aligned = {
	.read_inode = spec_read_inode,
#if defined HAVE_KMEMB_STRUCT_SUPER_OPERATIONS_READ_INODE2
	.read_inode2 = spec_read_inode2,
#endif
	.put_inode = spec_put_inode,
	.delete_inode = spec_delete_inode,
	.put_super = spec_put_super,
	.statfs = spec_statfs,		/* like simple_statfs */
	.remount_fs = spec_remount_fs,
	.clear_inode = spec_clear_inode,
	.umount_begin = spec_umount_begin,
};

/* TODO: handle extended attributes */
/**
 *  specfs_fill_super: - create a shadow special filesystem super block
 *  @sb:	superblock for which to read a superblock inode
 *  @data:	options data
 *  @silent:	whether to ignore options errors
 *
 *  Create the super block inode.
 */
STATIC int
specfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct spec_sb_info *sbi;
	int err;

	_ptrace(("filling superblock %p\n", sb));
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
#if defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_S_FS_INFO
	sb->s_fs_info = sbi;
#elif defined HAVE_KMEMB_STRUCT_SUPER_BLOCK_U_GENERIC_SBP
	sb->u.generic_sbp = sbi;
#endif
	return (0);
      iput_error:
	_ptrace(("inode %p no %lu refcount now %d\n", inode, inode->i_ino,
		 atomic_read(&inode->i_count) - 1));
	iput(inode);
      free_error:
	spec_sbi_free(sbi);
      error:
	return (err);
}

#if defined HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_GET_SB
STATIC struct super_block *
specfs_get_sb(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
	_ptrace(("reading superblock\n"));
	return get_sb_single(fs_type, flags, data, specfs_fill_super);
}
STATIC void
specfs_kill_sb(struct super_block *sb)
{
	_ptrace(("killing superblock %p\n", sb));
	return kill_anon_super(sb);
}

struct file_system_type spec_fs_type = {
	.owner = THIS_MODULE,
	.name = "specfs",
	.get_sb = specfs_get_sb,
	.kill_sb = specfs_kill_sb,
};
#elif defined HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_READ_SUPER
/**
 *  specfs_read_super: - create a shadow special filesystem super block
 *  @sb:	superblock for which to read a superblock inode
 *  @data:	options data
 *  @silent:	whether to ignore options errors
 *
 *  Create the super block inode.
 */
STATIC struct super_block *
specfs_read_super(struct super_block *sb, void *data, int silent)
{
	_ptrace(("reading superblock %p\n", sb));
	return (specfs_fill_super(sb, data, silent) ? NULL : sb);
}

STATIC DECLARE_FSTYPE(spec_fs_type, "specfs", specfs_read_super, FS_SINGLE);
#else
#error HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_GET_SB or HAVE_KMEMB_STRUCT_FILE_SYSTEM_TYPE_READ_SUPER must be defined.
#endif

STATIC spinlock_t specfs_lock = SPIN_LOCK_UNLOCKED;

streams_fastcall struct vfsmount *
specfs_mount(void)
{
	spin_lock(&specfs_lock);
	if (!specfs_mnt) {
		specfs_mnt = kern_mount(&spec_fs_type);
		if (IS_ERR(specfs_mnt))
			specfs_mnt = NULL;
	}
	spin_unlock(&specfs_lock);
	return (mntget(specfs_mnt));
}

EXPORT_SYMBOL_NOVERS(specfs_mount);

#if ! defined HAVE_KFUNC_KERN_UMOUNT
#undef kern_umount
#define kern_umount mntput
#endif

streams_fastcall void
specfs_umount(void)
{
	if (specfs_mnt) {
		mntput(specfs_mnt);
		spin_lock(&specfs_lock);
		if (atomic_read(&specfs_mnt->mnt_count) == 1) {
			kern_umount(specfs_mnt);
			specfs_mnt = NULL;
		}
		spin_unlock(&specfs_lock);
	}
}

EXPORT_SYMBOL_NOVERS(specfs_umount);

/**
 *  strspecfs_init: - initialize the shadow special filesystem
 */
#ifdef CONFIG_STREAMS_MODULE
static
#endif
int __init
specfs_init(void)
{
	int result;

#ifdef CONFIG_STREAMS_MODULE
	printk(KERN_INFO SPECFS_BANNER);	/* log message */
#else
	printk(KERN_INFO SPECFS_SPLASH);	/* console splash */
#endif
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
void __exit
specfs_exit(void)
{
	strlookup_exit();
	unregister_filesystem(&spec_fs_type);
}

#ifdef CONFIG_STREAMS_MODULE
module_init(specfs_init);
module_exit(specfs_exit);
#endif
