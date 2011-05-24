/*****************************************************************************

 @(#) $RCSfile: strpipe.c,v $ $Name:  $($Revision: 1.1.2.5 $) $Date: 2011-03-26 04:28:48 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2011-03-26 04:28:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strpipe.c,v $
 Revision 1.1.2.5  2011-03-26 04:28:48  brian
 - updates to build process

 Revision 1.1.2.4  2011-01-13 16:19:08  brian
 - changes for SLES 11 support

 Revision 1.1.2.3  2010-11-28 14:21:56  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.2  2009-07-23 16:37:53  brian
 - updates for release

 Revision 1.1.2.1  2009-06-21 11:37:16  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: strpipe.c,v $ $Name:  $($Revision: 1.1.2.5 $) $Date: 2011-03-26 04:28:48 $";

#ifdef NEED_LINUX_AUTOCONF_H
#include <linux/autoconf.h>
#endif
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

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "strlookup.h"		/* cdevsw_list, etc. */
#include "strspecfs.h"		/* for struct spec_sb_info */
#include "strpipe.h"		/* header verification */

#if 0
/* not used anymore anway */
#if defined HAVE_KERNEL_PIPE_SUPPORT && \
    (!defined CONFIG_KERNEL_WEAK_MODULES || \
     (defined HAVE_FILE_KILL_EXPORT && defined HAVE_PUT_FILP_EXPORT))

#ifndef O_CLONE
#define O_CLONE (O_CREAT|O_EXCL)
#endif

/*
 *  Opening of pipes has changed somewhat from the original implementation.  Originally we went to
 *  the extent of creating two inodes, one for each end of the pipe.  This is not necessary.  We now
 *  only create one inode and hang both Stream heads off of the same inode.  This has advantages,
 *  particularly with the assignment of inode numbers in the specfs, as well as allowing the inode
 *  semaphore to protect both Stream heads.
 *
 *  Traditional SVR 4.2 implemenations of pipes use a separate filesystem for pipes and FIFOs
 *  (pipefs).  Linux uses a separate filesystem for sockets as well (sockfs).  Linux Fast-STREAMS
 *  pipe inodes are allocated from the Shadown Special Filesystem (specfs) just as any other Stream.
 *  This permits Streams, STREAMS-based pipes, STREAMS-based FIFOs (and even STREAMS-based sockets)
 *  to be treated in the same fashion with regard to filesystem.
 */

#if 0
/*
 *  This is a variation on the theme of the old spec_open.
 */
STATIC struct file *
pipe_file_open(void)
{
	struct file *file;
	struct dentry *dentry;
	struct vfsmount *mnt;

	_ptrace(("%s: performing pipe open\n", __FUNCTION__));
	{
		struct qstr name = {.name = "pipe",.len = 4, };

		name.hash = full_name_hash(name.name, name.len);
		{
			file = ERR_PTR(-ENXIO);
			if (!(mnt = specfs_mount()))
				goto done;
			_printd(("%s: got mount point\n", __FUNCTION__));
			down(&mnt->mnt_root->d_inode->i_sem);
			dentry = lookup_hash(&name, mnt->mnt_root);
			up(&mnt->mnt_root->d_inode->i_sem);
			specfs_umount();
		}
	}
	if (IS_ERR((file = (void *) dentry))) {
		_ptrace(("%s: parent lookup in error, errno %d\n", __FUNCTION__,
			 -(int) PTR_ERR(file)));
		goto done;
	}
	if (!dentry->d_inode) {
		_ptrace(("%s: negative dentry on lookup\n", __FUNCTION__));
		goto enoent;
	}
	{
		struct dentry *parent = dentry;
		struct qstr name = {.name = "0",.len = 1, };

		name.hash = full_name_hash(name.name, name.len);
		_printd(("%s: looking up minor device %hu by name '%s', len %d\n", __FUNCTION__, 0,
			 name.name, name.len));
		down(&parent->d_inode->i_sem);
		dentry = lookup_hash(&name, parent);
		up(&parent->d_inode->i_sem);
		dput(parent);
	}
	if (IS_ERR((file = (void *) dentry))) {
		_ptrace(("%s: dentry lookup in error, errno %d\n", __FUNCTION__,
			 -(int) PTR_ERR(file)));
		goto done;
	}
	if (!dentry->d_inode) {
		_ptrace(("%s: negative dentry on lookup\n", __FUNCTION__));
		goto enoent;
	}
	_printd(("%s: opening dentry %p, inode %p (%ld)\n", __FUNCTION__, dentry, dentry->d_inode,
		 dentry->d_inode->i_ino));
	file = dentry_open(dentry, mntget(mnt), O_CLONE | 0x3);
	if (IS_ERR(file)) {
		_ptrace(("%s: dentry_open returned error, errno %d\n", __FUNCTION__,
			 -(int) PTR_ERR(file)));
		goto done;
	}
      done:
	return (file);
      enoent:
	dput(dentry);
	file = ERR_PTR(-ENOENT);
	goto done;
}

long
do_spipe(int *fds)
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
#else

/* we want macro versions of these */

#undef getmajor
#define getmajor(__ino) (((__ino)>>16)&0x0000ffff)

#undef getminor
#define getminor(__ino) (((__ino)>>0)&0x0000ffff)

#undef makedevice
#define makedevice(__maj,__min) ((((__maj)<<16)&0xffff0000)|(((__min)<<0)&0x0000ffff))

STATIC spinlock_t pipe_ino_lock = SPIN_LOCK_UNLOCKED;
STATIC int pipe_ino = 0;

void file_kill(struct file *file);

streams_fastcall long
do_spipe(int *fds)
{
	dev_t dev;
	minor_t minor;
	modID_t modid;
	struct vfsmount *mnt;
	struct dentry *dentry;
	struct inode *snode;
	struct cdevsw *cdev;
	int fdr, fdw, err;
	struct file *fr, *fw;
	const struct file_operations *f_op;

	err = -ENFILE;
	if (!(fr = get_empty_filp())) {
		_ptrace(("Error path taken!\n"));
		goto error;
	}
	if (!(fw = get_empty_filp())) {
		_ptrace(("Error path taken!\n"));
		goto fr_put;
	}
	if ((cdev = cdev_find("pipe"))) {
		spin_lock(&pipe_ino_lock);
		minor = ++pipe_ino;
		spin_unlock(&pipe_ino_lock);

		modid = cdev->d_modid;
		dev = makedevice(modid, minor);
		snode = spec_snode(dev, cdev);
		_ctrace(sdev_put(cdev));
	} else {
		_ptrace(("Error path taken!\n"));
		goto fw_put;
	}
	if (!snode) {
		_ptrace(("Error path taken!\n"));
		goto fw_put;
	}
	if (IS_ERR(snode)) {
		err = PTR_ERR(snode);
		_ptrace(("Error path taken! err = %d\n", err));
		goto fw_put;
	}
	if ((fdr = get_unused_fd()) < 0) {
		err = fdr;
		_ptrace(("Error path taken! err = %d\n", err));
		goto snode_put;
	}
	if ((fdw = get_unused_fd()) < 0) {
		err = fdw;
		_ptrace(("Error path taken! err = %d\n", err));
		goto fdr_put;
	}
	err = -ENODEV;
	if (!(mnt = specfs_mount())) {
		_ptrace(("Error path taken!\n"));
		goto fdw_put;
	}

	{
		struct qstr name;
		char buf[25];

		name.name = buf;
		name.len = snprintf(buf, sizeof(buf), "STR pipe/%lu", getminor(dev));
		err = -ENOMEM;
		if (!(dentry = d_alloc(NULL, &name))) {
			_ptrace(("Error path taken!\n"));
			goto mnt_put;
		}
		dentry->d_sb = snode->i_sb;
		dentry->d_parent = dentry;
	}
	d_instantiate(dentry, snode);
	err = -ENXIO;
	if (!(f_op = snode->i_fop) || !f_op->open) {
		_ptrace(("Error path taken!\n"));
		goto dentry_put;
	}
	fops_get(f_op);
	fops_get(f_op);

	fr->f_dentry = fw->f_dentry = dget(dentry);
	fr->f_vfsmnt = fw->f_vfsmnt = mntget(mntget(mnt));
#if 0				/* FIXME: need for 2.6.11 */
	fr->f_mapping = fw->f_mapping = snode->i_mapping;
#endif
	fr->f_pos = fw->f_pos = 0;
	fr->f_flags = O_RDONLY;
	fw->f_flags = O_WRONLY;
	fr->f_op = fw->f_op = f_op;
	fr->f_mode = FMODE_READ;
	fw->f_mode = FMODE_WRITE;
#if 0				/* FIXME: need for 2.6.11 */
	fr->f_version = fw->f_version = 0;
#endif

	specfs_umount();

	file_move(fr, &snode->i_sb->s_files);
	file_move(fw, &snode->i_sb->s_files);

	fr->f_flags |= O_CLONE;
	if ((err = fr->f_op->open(snode, fr))) {
		_ptrace(("Error path taken! err = %d\n", err));
		goto cleanup_both;
	}
	fr->f_flags &= ~O_CLONE;

	fw->f_flags |= O_CLONE;
	if ((err = fw->f_op->open(snode, fw))) {
		_ptrace(("Error path taken! err = %d\n", err));
		goto cleanup_write;
	}
	fw->f_flags &= ~O_CLONE;

	/* link Stream heads together */
	{
		struct stdata *sdr = (struct stdata *) fr->private_data;
		struct stdata *sdw = (struct stdata *) fw->private_data;

		sdr->sd_wq->q_next = sdw->sd_rq;
		sdw->sd_wq->q_next = sdr->sd_rq;

		sdr->sd_wq->q_nfsrv = sdw->sd_rq;
		sdw->sd_wq->q_nfsrv = sdr->sd_rq;

		sdr->sd_rq->q_nbsrv = sdw->sd_wq;
		sdw->sd_rq->q_nbsrv = sdr->sd_wq;

		sdr->sd_other = sdw;
		sdw->sd_other = sdr;
	}

	fd_install(fdr, fr);
	fd_install(fdw, fw);

	fds[0] = fdr;
	fds[1] = fdw;

	return (0);

      dentry_put:
	dput(dentry);
	snode = NULL;
      mnt_put:
	specfs_umount();
      fdw_put:
	put_unused_fd(fdw);
      fdr_put:
	put_unused_fd(fdr);
      snode_put:
	if (snode)
		iput(snode);
      fw_put:
	put_filp(fw);
      fr_put:
	put_filp(fr);
      error:
	return (err);

      cleanup_both:
	file_kill(fr);
	file_kill(fw);
	fops_put(f_op);
	fops_put(f_op);
	fr->f_dentry = fw->f_dentry = NULL;
	dput(dentry);
	dput(dentry);
	fr->f_vfsmnt = fw->f_vfsmnt = NULL;
	mntput(mnt);
	mntput(mnt);
	put_unused_fd(fdr);
	put_unused_fd(fdw);
	put_filp(fr);
	put_filp(fw);
	return (err);

      cleanup_write:
	fput(fr);
	file_kill(fw);
	fops_put(f_op);
	fw->f_dentry = NULL;
	dput(dentry);
	fw->f_vfsmnt = NULL;
	mntput(mnt);
	put_unused_fd(fdr);
	put_unused_fd(fdw);
	put_filp(fw);
	return (err);
}
#endif

EXPORT_SYMBOL_GPL(do_spipe);

#endif				/* defined HAVE_KERNEL_PIPE_SUPPORT */
#endif
