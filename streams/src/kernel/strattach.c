/*****************************************************************************

 @(#) $RCSfile: strattach.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2006/12/18 10:08:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/12/18 10:08:59 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strattach.c,v $
 Revision 0.9.2.43  2006/12/18 10:08:59  brian
 - updated headers for release

 *****************************************************************************/

#ident "@(#) $RCSfile: strattach.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2006/12/18 10:08:59 $"

static char const ident[] = "$RCSfile: strattach.c,v $ $Name:  $($Revision: 0.9.2.43 $) $Date: 2006/12/18 10:08:59 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/quotaops.h>
#include <linux/acct.h>
#ifdef HAVE_KINC_LINUX_DEVFS_FS_KERNEL_H
#include <linux/devfs_fs_kernel.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>

#include <asm/uaccess.h>

#include <linux/seq_file.h>
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/file.h>
#ifdef HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif
#if defined HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>	/* avoid ptrace conflict */
#endif

#include "sys/strdebug.h"

#include "sys/config.h"
#include "strattach.h"		/* header verification */

#if defined HAVE_KERNEL_FATTACH_SUPPORT

#if defined HAVE_MOUNT_SEM_ADDR
#define mount_sem (*((struct semaphore *)HAVE_MOUNT_SEM_ADDR))
#endif
#if defined HAVE_NAMESPACE_SEM_ADDR
#define namespace_sem (*((struct rw_semaphore *)HAVE_NAMESPACE_SEM_ADDR))
#endif

#if defined HAVE_CLONE_MNT_ADDR
STATIC struct vfsmount *(*clone_mnt) (struct vfsmount * old, struct dentry * root)
= (typeof(clone_mnt)) HAVE_CLONE_MNT_ADDR;
#endif
#if defined HAVE_CHECK_MNT_ADDR
STATIC int (*check_mnt) (struct vfsmount * mnt)
= (typeof(check_mnt)) HAVE_CHECK_MNT_ADDR;
#else
STATIC int
check_mnt(struct vfsmount *mnt)
{
	return mnt->mnt_namespace == current->namespace;
}
#endif
#if defined HAVE_GRAFT_TREE_ADDR
STATIC int (*graft_tree) (struct vfsmount * mnt, struct nameidata * nd)
= (typeof(graft_tree)) HAVE_GRAFT_TREE_ADDR;
#endif
#if defined HAVE_DO_UMOUNT_ADDR
STATIC int (*do_umount) (struct vfsmount * mnt, int flags)
= (typeof(do_umount)) HAVE_DO_UMOUNT_ADDR;
#endif

#ifndef HAVE_PATH_LOOKUP_EXPORT
int
path_lookup(const char *path, unsigned flags, struct nameidata *nd)
{
	int error = 0;

	if (path_init(path, flags, nd))
		error = path_walk(path, nd);
	return error;
}
#endif

streams_fastcall long
do_fattach(const struct file *file, const char *file_name)
{
	/* very much like do_add_mount() but with different permissions and clone_mnt() of the file 
	   instead of do_kern_mount() of the filesystem root */
	struct nameidata nd;
	struct vfsmount *mnt;
	int err;

	err = -EINVAL;
	if (file->f_dentry->d_sb->s_magic != SPECFS_MAGIC)
		goto out;	/* not a STREAM */
	err = -ENOENT;
	if (!file_name || !*file_name)
		goto out;

#ifdef LOOKUP_POSITIVE
	err = path_lookup(file_name, LOOKUP_FOLLOW | LOOKUP_POSITIVE, &nd);
#else
	err = path_lookup(file_name, LOOKUP_FOLLOW, &nd);
#endif
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

#ifdef HAVE_NAMESPACE_SEM_ADDR
	down_write(&namespace_sem);
#else
#ifdef HAVE_MOUNT_SEM_ADDR
	down(&mount_sem);
#else
	down_write(&current->namespace->sem);
#endif
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
#ifdef HAVE_NAMESPACE_SEM_ADDR
	up_write(&namespace_sem);
#else
#ifdef HAVE_MOUNT_SEM_ADDR
	up(&mount_sem);
#else
	up_write(&current->namespace->sem);
#endif
#endif
	mntput(mnt);
      release:
	path_release(&nd);
      out:
	return err;
}

EXPORT_SYMBOL_GPL(do_fattach);

streams_fastcall long
do_fdetach(const char *file_name)
{
	/* pretty much the same as sys_umount() with different permissions */
	struct nameidata nd;
	int err;

	err = -ENOENT;
	if (!file_name || !*file_name)
		goto out;

#ifdef LOOKUP_POSITIVE
	err = path_lookup(file_name, LOOKUP_FOLLOW | LOOKUP_POSITIVE, &nd);
#else
	err = path_lookup(file_name, LOOKUP_FOLLOW, &nd);
#endif
	if (err)
		goto out;

	err = -EINVAL;
	if (nd.dentry != nd.mnt->mnt_root)
		goto release;	/* not mounted */
	if (!check_mnt(nd.mnt))
		goto release;
	if (nd.dentry->d_sb->s_magic != SPECFS_MAGIC)
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

EXPORT_SYMBOL_GPL(do_fdetach);

#endif				/* defined HAVE_KERNEL_FATTACH_SUPPORT */
