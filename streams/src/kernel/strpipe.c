/*****************************************************************************

 @(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2005/07/07 20:29:46 $

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

 Last Modified $Date: 2005/07/07 20:29:46 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2005/07/07 20:29:46 $"

static char const ident[] =
    "$RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2005/07/07 20:29:46 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/quotaops.h>
#include <linux/acct.h>
#include <linux/devfs_fs_kernel.h>

#include <asm/uaccess.h>

#include <linux/seq_file.h>
#if HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/file.h>
#if HAVE_KINC_LINUX_NAMEI_H
#include <linux/namei.h>
#endif

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "strlookup.h"		/* cdevsw_list, etc. */
#include "strspecfs.h"		/* for struct spec_sb_info */
#include "strpipe.h"		/* header verification */

#if defined HAVE_KERNEL_PIPE_SUPPORT

#ifndef O_CLONE
#define O_CLONE (O_CREAT|O_EXCL)
#endif

/*
 *  This is a variation on the theme of spec_open.
 */
STATIC struct file *pipe_file_open(void)
{
	struct file *file;
	struct dentry *dentry;
	struct vfsmount *mnt;
	ptrace(("%s: performing pipe open\n", __FUNCTION__));
	{
		struct qstr name = {.name = "pipe",.len = 4, };
		name.hash = full_name_hash(name.name, name.len);
		{
			file = ERR_PTR(-ENXIO);
			if (!(mnt = specfs_get()))
				goto done;
			printd(("%s: got mount point\n", __FUNCTION__));
			down(&mnt->mnt_root->d_inode->i_sem);
			dentry = lookup_hash(&name, mnt->mnt_root);
			up(&mnt->mnt_root->d_inode->i_sem);
			specfs_put();
		}
	}
	if (IS_ERR((file = (void *) dentry))) {
		ptrace(("%s: parent lookup in error, errno %d\n", __FUNCTION__,
			-(int) PTR_ERR(file)));
		goto done;
	}
	if (!dentry->d_inode) {
		ptrace(("%s: negative dentry on lookup\n", __FUNCTION__));
		goto enoent;
	}
	{
		struct dentry *parent = dentry;
		struct qstr name = {.name = "0",.len = 1, };
		name.hash = full_name_hash(name.name, name.len);
		printd(("%s: looking up minor device %hu by name '%s', len %d\n", __FUNCTION__, 0,
			name.name, name.len));
		down(&parent->d_inode->i_sem);
		dentry = lookup_hash(&name, parent);
		up(&parent->d_inode->i_sem);
		dput(parent);
	}
	if (IS_ERR((file = (void *) dentry))) {
		ptrace(("%s: dentry lookup in error, errno %d\n", __FUNCTION__, -(int)PTR_ERR(file)));
		goto done;
	}
	if (!dentry->d_inode) {
		ptrace(("%s: negative dentry on lookup\n", __FUNCTION__));
		goto enoent;
	}
	printd(("%s: opening dentry %p, inode %p (%ld)\n", __FUNCTION__, dentry, dentry->d_inode,
		dentry->d_inode->i_ino));
	file = dentry_open(dentry, mntget(mnt), O_CLONE | 0x3);
	if (IS_ERR(file)) {
		ptrace(("%s: dentry_open returned error, errno %d\n", __FUNCTION__,
			-(int)PTR_ERR(file)));
		goto done;
	}
      done:
	return (file);
      enoent:
	dput(dentry);
	file = ERR_PTR(-ENOENT);
	goto done;
}

long do_spipe(int *fds)
{
	struct file *file1, *file2;
	int fd1, fd2;
	int err;
#if HAVE_PUT_FILP_ADDR
	static void (*put_filp) (struct file * file) = (typeof(put_filp)) HAVE_PUT_FILP_ADDR;
#endif
	err = -ENFILE;
	if (!(file1 = pipe_file_open()))
		goto no_file1;
	err = PTR_ERR(file1);
	if (IS_ERR(file1))
		goto no_file1;
	if (!(file2 = pipe_file_open()))
		goto no_file2;
	err = PTR_ERR(file2);
	if (IS_ERR(file2))
		goto no_file2;
	if ((fd1 = err = get_unused_fd()) < 0)
		goto no_fd1;
	if ((fd2 = err = get_unused_fd()) < 0)
		goto no_fd2;
	fd_install(fd1, file1);
	fd_install(fd2, file2);
	fds[0] = fd1;
	fds[1] = fd2;
	return (0);
      no_fd2:
	put_unused_fd(fd1);
      no_fd1:
	put_filp(file2);
      no_file2:
	put_filp(file1);
      no_file1:
	return (err);
}

#if defined CONFIG_STREAMS_STH_MODULE
EXPORT_SYMBOL(do_spipe);
#endif
#endif				/* defined HAVE_KERNEL_PIPE_SUPPORT */
