/*****************************************************************************

 @(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/06/12 23:20:20 $

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

 Last Modified $Date: 2004/06/12 23:20:20 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/06/12 23:20:20 $"

static char const ident[] = "$RCSfile: strpipe.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/06/12 23:20:20 $";

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
#include <sys/strsubr.h>
#include <sys/strconf.h>

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
	int err;
	struct file *file;
	struct cdevsw *cdev;
	struct dentry *parent, *dentry;
	struct inode *inode;
	struct vfsmount *mnt;
	ptrace(("%s: performing pipe open\n", __FUNCTION__));
	file = ERR_PTR(-ENXIO);
	if (!(cdev = cdev_find("pipe")))
		goto exit;
	printd(("%s: %s: got driver\n", __FUNCTION__, cdev->d_name));
	printd(("%s: open of driver %s\n", __FUNCTION__, cdev->d_name));
	file = ERR_PTR(-ENOENT);
	if (!(parent = dget(cdev->d_dentry)))
		goto cput_exit;
	printd(("%s: parent dentry %s\n", __FUNCTION__, parent->d_name.name));
	file = ERR_PTR(-ENOMEM);
	if (!(inode = parent->d_inode))
		goto pput_exit;
	printd(("%s: parent inode %ld\n", __FUNCTION__, inode->i_ino));
	file = ERR_PTR(-ENODEV);
	if (is_bad_inode(inode))
		goto pput_exit;
	/* lock the parent */
	if ((err = down_interruptible(&inode->i_sem)) < 0) {
		file = ERR_PTR(err);
		goto pput_exit;
	}
	{
		char buf[] = "0";
		struct qstr name;
		name.name = buf;
		name.len = 1;
		name.hash = full_name_hash(name.name, name.len);
		printd(("%s: looking up minor device %hu by name '%s', len %d\n", __FUNCTION__,
			0, name.name, name.len));
		dentry = lookup_hash(&name, parent);
	}
	file = ERR_PTR(-ENOENT);		/* XXX */
	if (!dentry) {
		ptrace(("%s: dentry lookup is NULL\n", __FUNCTION__));
		goto up_exit;
	}
	file = (typeof(file))dentry;
	if (IS_ERR(dentry)) {
		ptrace(("%s: dentry lookup in error, errno %d\n", __FUNCTION__, -err));
		goto up_exit;
	}
	/* we only fail to get an inode when memory allocation fails */
	file = ERR_PTR(-ENOMEM);
	if (!dentry->d_inode) {
		ptrace(("%s: negative dentry on lookup\n", __FUNCTION__));
		goto dput_exit;
	}
	/* we only get a bad inode when there is no device entry */
	file = ERR_PTR(-ENODEV);
	if (is_bad_inode(dentry->d_inode)) {
		ptrace(("%s: bad inode on lookup\n", __FUNCTION__));
		goto dput_exit;
	}
	/* unlock the parent */
	up(&inode->i_sem);
	inode = NULL;
	file = ERR_PTR(-ENODEV);
	if (!(mnt = specfs_get())) {
		ptrace(("%s: could not find specfs mount point\n", __FUNCTION__));
		goto dput_exit;
	}
	printd(("%s: opening dentry %p\n", __FUNCTION__, dentry));
	file = dentry_open(dentry, mntget(mnt), O_CLONE | 0x3);
	specfs_put();
	if (IS_ERR(file)) {
		ptrace(("%s: dentry_open returned error, errno %d\n", __FUNCTION__, -err));
		goto pput_exit;
	}
	goto cput_exit;
      dput_exit:
	printd(("%s: putting dentry\n", __FUNCTION__));
	dput(dentry);
      up_exit:
	if (inode) {
		printd(("%s: releasing inode semaphore\n", __FUNCTION__));
		up(&inode->i_sem);
	}
      pput_exit:
	printd(("%s: putting parent dentry\n", __FUNCTION__));
	dput(parent);
      cput_exit:
	printd(("%s: %s: putting driver\n", __FUNCTION__, cdev->d_name));
	cdev_put(cdev);
      exit:
	return (file);
}

long do_spipe(int *fds)
{
	struct file *file1, *file2;
	int fd1, fd2;
	int err;
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
EXPORT_SYMBOL_GPL(do_spipe);
#endif
#endif				/* defined HAVE_KERNEL_PIPE_SUPPORT */

