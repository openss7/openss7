/*****************************************************************************

 @(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/06/03 10:12:16 $

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

 Last Modified $Date: 2004/06/03 10:12:16 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/06/03 10:12:16 $"

static char const ident[] = "$RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/06/03 10:12:16 $";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

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

#include <sys/stream.h>
#include <sys/strconf.h>

#include "sys/config.h"
#include "strdebug.h"
#include "strspecfs.h"		/* for struct spec_sb_info */
#include "strpipe.h"		/* header verification */

#if defined HAVE_KERNEL_PIPE_SUPPORT
STATIC struct inode *get_spipe_inode(struct vfsmount *mnt)
{
	struct inode *inode;
	if ((inode = new_inode(mnt->mnt_sb))) {
		struct spec_sb_info *sbi = inode->i_sb->u.generic_sbp;
		inode->i_uid = sbi->sbi_setuid ? sbi->sbi_uid : current->fsuid;
		inode->i_gid = sbi->sbi_setgid ? sbi->sbi_gid : current->fsgid;
		inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
		inode->u.generic_ip = NULL;
		inode->i_mode = S_IFIFO | sbi->sbi_mode;
		inode->i_fop = &spec_dev_f_ops;
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
	struct vfsmount *mnt;
	char name[32];
	int fd1, fd2;
	int err;
	err = -ENODEV;
	if (!(mnt = specfs_get()))
		goto no_mnt;
	err = -ENFILE;
	if (!(file1 = get_empty_filp()))
		goto no_file1;
	if (!(file2 = get_empty_filp()))
		goto no_file2;
	if (!(inode1 = get_spipe_inode(mnt)))
		goto no_inode1;
	if (!(inode2 = get_spipe_inode(mnt)))
		goto no_inode2;
	if ((fd1 = err = get_unused_fd()) < 0)
		goto no_fd1;
	if ((fd2 = err = get_unused_fd()) < 0)
		goto no_fd2;
	snprintf(name, sizeof(name), "[%lu]", inode1->i_ino);
	str1.name = name;
	str1.len = strlen(name);
	str1.hash = inode1->i_ino;
	if (!(dentry1 = d_alloc(mnt->mnt_sb->s_root, &str1)))
		goto no_dentry1;
	d_add(dentry1, inode1);
	snprintf(name, sizeof(name), "[%lu]", inode2->i_ino);
	str2.name = name;
	str2.len = strlen(name);
	str2.hash = inode1->i_ino;
	if (!(dentry2 = d_alloc(mnt->mnt_sb->s_root, &str2)))
		goto no_dentry2;
	d_add(dentry2, inode2);
	/* FIXME: need to call open for each dentry */
	/* file 1 */
	file1->f_vfsmnt = mntget(mnt);
	file1->f_dentry = dget(dentry1);
	file1->f_pos = 0;
	file1->f_flags = O_RDWR;
	file1->f_op = &spec_dev_f_ops;
	file1->f_mode = 3;
	file1->f_version = 0;
	/* file 2 */
	file2->f_vfsmnt = mntget(mnt);
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
      no_mnt:
	specfs_put();
      error:
	return (err);
}

#if defined CONFIG_STREAMS_STH_MODULE
EXPORT_SYMBOL_GPL(do_spipe);
#endif
#endif				/* defined HAVE_KERNEL_PIPE_SUPPORT */

