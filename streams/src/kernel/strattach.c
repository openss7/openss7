/*****************************************************************************

 @(#) $RCSfile: strattach.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/05/08 19:21:15 $

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

 Last Modified $Date: 2004/05/08 19:21:15 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strattach.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/05/08 19:21:15 $"

static char const ident[] =
    "$RCSfile: strattach.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2004/05/08 19:21:15 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>

#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/quotaops.h>
#include <linux/acct.h>
#include <linux/devfs_fs_kernel.h>

#include <asm/uaccess.h>

#include <linux/seq_file.h>
#ifdef HAVE_TASK_NAMESPACE_SEM
#include <linux/namespace.h>
#endif
#include <linux/file.h>

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/dki.h>

#include "sys/config.h"
#include "strdebug.h"
#include "strattach.h"
#include "strspecfs.h"

#if defined HAVE_KERNEL_FATTACH_SUPPORT

#if defined HAVE_MOUNT_SEM_ADDR
#define mount_sem (*((struct semaphore *)HAVE_MOUNT_SEM_ADDR))
#endif

#if defined HAVE_CLONE_MNT_ADDR
static struct vfsmount *(*clone_mnt) (struct vfsmount * old, struct dentry * root)
= (typeof(clone_mnt)) HAVE_CLONE_MNT_ADDR;
#endif
#if defined HAVE_CHECK_MNT_ADDR
static int (*check_mnt) (struct vfsmount * mnt)
= (typeof(check_mnt)) HAVE_CHECK_MNT_ADDR;
#endif
#if defined HAVE_GRAFT_TREE_ADDR
static int (*graft_tree) (struct vfsmount * mnt, struct nameidata * nd)
= (typeof(graft_tree)) HAVE_GRAFT_TREE_ADDR;
#endif
#if defined HAVE_DO_UMOUNT_ADDR
static int (*do_umount) (struct vfsmount * mnt, int flags)
= (typeof(do_umount)) HAVE_DO_UMOUNT_ADDR;
#endif

long do_fattach(const struct file *file, const char *file_name)
{
	/* very much like do_add_mount() but with different permissions and clone_mnt() of the file 
	   instead of do_kern_mount() of the filesystem root */
	struct nameidata nd;
	struct vfsmount *mnt;
	int err;

	err = -EINVAL;
	if (!file->f_dentry->d_inode->i_pipe)
		goto out;	/* not a STREAM */
	if (!file_name || !*file_name)
		goto out;

	if (path_init(file_name, LOOKUP_FOLLOW | LOOKUP_POSITIVE, &nd))
		err = path_walk(file_name, &nd);
	if (err)
		goto out;

	err = -EPERM;
	/* the owner of the file is permitted to fattach */
	if (!capable(CAP_SYS_ADMIN) && current->uid != file->f_dentry->d_inode->i_uid)
		goto release;

	err = -ENOMEM;
	mnt = clone_mnt(file->f_vfsmnt, file->f_dentry);
	if (!mnt)
		goto release;

#if defined HAVE_TASK_NAMESPACE_SEM
	down_write(&current->namespace->sem);
#else
	down(&mount_sem);
#endif

	/* Something was mounted here while we slept */
	while (d_mountpoint(nd.dentry) && follow_down(&nd.mnt, &nd.dentry)) ;

	err = -EINVAL;
	if (!check_mnt(nd.mnt))
		goto unlock;

	/* Refuse the same filesystem on the same mount point */
	err = -EBUSY;
	if (nd.mnt->mnt_sb == mnt->mnt_sb && nd.mnt->mnt_root == nd.dentry)
		goto unlock;

	mnt->mnt_flags = 0;
	err = graft_tree(mnt, &nd);

      unlock:
#if defined HAVE_TASK_NAMESPACE_SEM
	up_write(&current->namespace->sem);
#else
	up(&mount_sem);
#endif
	mntput(mnt);
      release:
	path_release(&nd);
      out:
	return err;
}
#if defined CONFIG_STREAMS_STH_MODULE || defined CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL_GPL(do_fattach);
#endif

long do_fdetach(const char *file_name)
{
	/* pretty much the same as sys_umount() with different permissions */
	struct nameidata nd;
	int err;

	err = -EINVAL;
	if (!file_name || !*file_name)
		goto out;

	if (path_init(file_name, LOOKUP_FOLLOW | LOOKUP_POSITIVE, &nd))
		err = path_walk(file_name, &nd);
	if (err)
		goto out;

	err = -EINVAL;
	if (nd.dentry != nd.mnt->mnt_root)
		goto release;	/* not mounted */
	if (!check_mnt(nd.mnt))
		goto release;
	if (!nd.dentry->d_inode->i_pipe)
		goto release;	/* not a STREAM */

	err = -EPERM;
	/* the owner of the (real) file is permitted to fdetach */
	if (!capable(CAP_SYS_ADMIN) && current->uid != nd.mnt->mnt_mountpoint->d_inode->i_uid)
		goto release;

	err = do_umount(nd.mnt, MNT_DETACH);

      release:
	path_release(&nd);
      out:
	return err;
}
#if defined CONFIG_STREAMS_STH_MODULE || defined CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL_GPL(do_fdetach);
#endif

#endif				/* defined HAVE_KERNEL_FATTACH_SUPPORT */

