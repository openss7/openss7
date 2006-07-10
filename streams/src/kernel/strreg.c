/*****************************************************************************

 @(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.66 $) $Date: 2006/07/10 12:22:42 $

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

 Last Modified $Date: 2006/07/10 12:22:42 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.66 $) $Date: 2006/07/10 12:22:42 $"

static char const ident[] =
    "$RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.66 $) $Date: 2006/07/10 12:22:42 $";

#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

#include <linux/kernel.h>	/* for FASTCALL(), fastcall */
#include <linux/sched.h>	/* for current */
#include <linux/file.h>		/* for fput */
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/mount.h>	/* for vfsmount and friends */
#include <asm/hardirq.h>

#include "sys/strdebug.h"

#include <sys/kmem.h>		/* for kmem_ */
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#ifndef HAVE_KFUNC_MODULE_PUT
#define module_refcount(__m) atomic_read(&(__m)->uc.usecount)
#endif

#include "sys/config.h"
#include "src/modules/sth.h"	/* for stream operations */
#include "src/kernel/strsched.h"	/* for sq_alloc */
#include "src/kernel/strutil.h"	/* for global_syncq */
#include "src/kernel/strlookup.h"	/* cdevsw_list, etc. */
#include "src/kernel/strreg.h"	/* extern verification */
/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and Deregistration
 *
 *  -------------------------------------------------------------------------
 */

#if defined CONFIG_STREAMS_SYNCQS
STATIC int
register_strsync(struct fmodsw *fmod)
{
	int sqlvl = SQLVL_DEFAULT;

	/* propagate flags to f_sqlvl */
	if (fmod->f_sqlvl == SQLVL_DEFAULT) {
		if (fmod->f_flag & D_UP)
			sqlvl = SQLVL_PERSTREAM;
		else if (fmod->f_flag & D_MP)
			sqlvl = SQLVL_NOP;
		else if (fmod->f_flag & D_MTPERQ)
			sqlvl = SQLVL_QUEUE;
		else if (fmod->f_flag & D_MTQPAIR)
			sqlvl = SQLVL_QUEUEPAIR;
		else if (fmod->f_flag & D_MTPERMOD)
			sqlvl = SQLVL_MODULE;
	}
	/* allocate synchronization queues */
	if (fmod->f_flag & D_MTOUTPERIM) {
		switch (sqlvl) {
		case SQLVL_NOP:
		case SQLVL_QUEUE:
		case SQLVL_QUEUEPAIR:
			break;
		default:
			return (-EINVAL);
		}
		if (!fmod->f_syncq) {
			struct syncq *sq;

			if (!(sq = sq_alloc()))
				return (-ENOMEM);
			fmod->f_syncq = sq;
			sq->sq_level = sqlvl;
			sq->sq_flag = SQ_OUTER | ((fmod->f_flag & D_MTOCEXCL) ? SQ_EXCLUS : 0);
		}
	} else {
		switch (sqlvl) {
		case SQLVL_DEFAULT:
		case SQLVL_MODULE:	/* default */
		case SQLVL_ELSEWHERE:
			if (!fmod->f_syncq) {
				struct syncq *sq;

				if (!(sq = sq_alloc()))
					return (-ENOMEM);
				fmod->f_syncq = sq;
				sq->sq_level = sqlvl;
				sq->sq_flag = (SQ_OUTER | SQ_EXCLUS);
			}
			break;
		case SQLVL_GLOBAL:	/* for testing */
			if (!fmod->f_syncq) {
				struct syncq *sq;

				sq = sq_get(global_syncq);
				fmod->f_syncq = sq;
				sq->sq_level = sqlvl;
				sq->sq_flag = (SQ_OUTER | SQ_EXCLUS);
			}
			break;
		}
	}
	fmod->f_sqlvl = sqlvl;
	return (0);
}

STATIC void
unregister_strsync(struct fmodsw *fmod)
{
	/* always delete if it exists */
	sq_put(&fmod->f_syncq);
}
#endif

/**
 *  register_strmod: - register STREAMS module
 *  @fmod: STREAMS module structure to register
 */
streams_fastcall int
register_strmod(struct fmodsw *fmod)
{
	struct module_info *mi;
	struct fmodsw *f;
	int err;

	if (!fmod || !fmod->f_name || !fmod->f_name[0]) {
		_ptrace(("Error path taken!\n"));
		return (-EINVAL);
	} else {
		struct streamtab *st;
		struct qinit *qi;

		if (!(st = fmod->f_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo)) {
			_ptrace(("Error path taken!\n"));
			return (-EINVAL);
		}
	}
#if defined CONFIG_STREAMS_SYNCQS
	if ((err = register_strsync(fmod))) {
		_ptrace(("Error path taken!\n"));
		return (err);
	}
#endif
	write_lock(&fmodsw_lock);
	{
		modID_t modid;

		/* check name for another module */
		if ((f = __smod_search(fmod->f_name))) {
			if (f != fmod) {
				_ptrace(("Error path taken!\n"));
				goto eperm;
			}
			_ptrace(("Error path taken!\n"));
			goto ebusy;
		}

		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			_ptrace(("Finding a free module id\n"));
			for (modid = (modID_t) (-1UL); modid && __fmod_lookup(modid); modid--) ;
			if (!modid) {
				_ptrace(("Error path taken!\n"));
				goto enxio;
			}
			mi->mi_idnum = modid;
		} else {
			_ptrace(("Using module id %hu\n", modid));
			/* use specified module id */
			if ((f = __fmod_lookup(modid))) {
				if (f != fmod) {
					_ptrace(("Error path taken!\n"));
					goto eperm;
				}
				_ptrace(("Error path taken!\n"));
				goto ebusy;
			}
		}
		_ptrace(("Assigned module id %hu\n", modid));
		fmod_add(fmod, modid);
		err = modid;
		goto unlock_exit;
	}
      eperm:
	err = -EPERM;
	goto unregister_exit;
      ebusy:
	err = -EBUSY;
	goto unregister_exit;
      enxio:
	err = -ENXIO;
	goto unregister_exit;
      unregister_exit:
#if defined CONFIG_STREAMS_SYNCQS
	unregister_strsync(fmod);
#endif
      unlock_exit:
	write_unlock(&fmodsw_lock);
	return (err);
}

EXPORT_SYMBOL_NOVERS(register_strmod);

/**
 *  unregister_strmod:
 *  @fmod: STREAMS module structure to unregister
 */
streams_fastcall int
unregister_strmod(struct fmodsw *fmod)
{
	int err = 0;

	write_lock(&fmodsw_lock);
	if (!fmod || !fmod->f_name || !fmod->f_name[0])
		goto einval;
	if (!fmod->f_list.next || list_empty(&fmod->f_list))
		goto enxio;
	fmod_del(fmod);
#if defined CONFIG_STREAMS_SYNCQS
	unregister_strsync(fmod);
#endif
      unlock_exit:
	write_unlock(&fmodsw_lock);
	return (err);
      einval:
	err = -EINVAL;
	goto unlock_exit;
      enxio:
	err = -ENXIO;
	goto unlock_exit;
}

EXPORT_SYMBOL_NOVERS(unregister_strmod);

/**
 *  register_strdrv:	- register STREAMS driver to specfs
 *  @cdev:	STREAMS device structure to register
 *
 *  register_strdrv() registers the specified cdevsw(9) structure to the specfs(5).  This results in
 *  the allocation of a specfs(5) directory node for the driver.  This is probably not the function
 *  that you want.  See register_strdev(9).
 *
 *  For full details, see register_strdrv(9).
 */
streams_fastcall int
register_strdrv(struct cdevsw *cdev)
{
	struct module_info *mi;
	struct cdevsw *c;
	int err = 0;

	if (!cdev || !cdev->d_name || !cdev->d_name[0]) {
		_ptrace(("Error path taken!\n"));
		return (-EINVAL);
	} else {
		struct streamtab *st;
		struct qinit *qi;

		if (!(st = cdev->d_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo)) {
			_ptrace(("Error path taken!\n"));
			return (-EINVAL);
		}
	}
#if defined CONFIG_STREAMS_SYNCQS
	if ((err = register_strsync((struct fmodsw *) cdev))) {
		_ptrace(("Error path taken!\n"));
		return (err);
	}
#endif
	write_lock(&cdevsw_lock);
	{
		modID_t modid;

		/* check name for another module */
		if ((c = __smod_search(cdev->d_name))) {
			if (c != cdev) {
				_ptrace(("Error path taken!\n"));
				goto eperm;
			}
			_ptrace(("Error path taken!\n"));
			goto ebusy;
		}
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			_ptrace(("Finding a free module id\n"));
			for (modid = (modID_t) (-1UL); modid && __cdrv_lookup(modid); modid--) ;
			if (!modid) {
				_ptrace(("Error path taken!\n"));
				goto enxio;
			}
			mi->mi_idnum = modid;
		} else {
			_ptrace(("Using module id %hu\n", modid));
			/* use specified module id */
			if ((c = __cdrv_lookup(modid))) {
				if (c != cdev) {
					_ptrace(("Error path taken!\n"));
					goto eperm;
				}
				/* already registered to us */
				_ptrace(("Error path taken!\n"));
				goto ebusy;
			}
		}
		write_unlock(&cdevsw_lock);
		_ptrace(("Assigned module id %hu\n", modid));
		err = sdev_ini(cdev, modid);
		write_lock(&cdevsw_lock);
		if (err) {
			_ptrace(("Error path taken!\n"));
			goto unlock_release_exit;
		}
		if ((err = __cdrv_lookup(modid) ? -EPERM : 0)) {
			/* someone stole our modid while the lock was released */
			_ptrace(("Error path taken!\n"));
			goto unlock_release_exit;
		}
		if ((err = sdev_add(cdev))) {
			_ptrace(("Error path taken!\n"));
			goto unlock_release_exit;
		}
		write_unlock(&cdevsw_lock);
		return (modid);
	}
      eperm:
	err = -EPERM;
	goto unlock_unregister_exit;
      ebusy:
	err = -EBUSY;
	goto unlock_unregister_exit;
      enxio:
	err = -ENXIO;
	goto unlock_unregister_exit;
      unlock_unregister_exit:
	write_unlock(&cdevsw_lock);
	goto unregister_exit;
      unlock_release_exit:
	write_unlock(&cdevsw_lock);
	goto release_exit;
      release_exit:
	sdev_rel(cdev);
      unregister_exit:
#if defined CONFIG_STREAMS_SYNCQS
	unregister_strsync((struct fmodsw *) cdev);
#endif
	return (err);
}

EXPORT_SYMBOL_NOVERS(register_strdrv);

/**
 *  unregister_strdrv:	- unregister STREAMS driver from specfs
 *  @cdev:	STREAMS driver structure to unregister
 *
 *  unregister_strdrv() unregisters the specified cdevsw(9) structure from the specfs(5).  @cdev
 *  must have already been registered with register_strdrv(9).  This results in the deallocation of
 *  the specfs(5) directory node for the driver.  This is probably not the function that you want.
 *  See unregister_strdev(9).
 *
 *  For full details, see unregister_strdrv(9).
 */
streams_fastcall int
unregister_strdrv(struct cdevsw *cdev)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	if (!cdev || !cdev->d_name || !cdev->d_name[0]) {
		_ptrace(("Error path taken!\n"));
		goto einval;
	}
	/* the cdev is not registered as a module */
	if (!cdev->d_list.next || list_empty(&cdev->d_list)) {
		_ptrace(("Error path taken!\n"));
		goto enxio;
	}
	if (cdev->d_majors.next && !list_empty(&cdev->d_majors)) {
		_ptrace(("Error path taken!\n"));
		goto ebusy;
	}
	if (cdev->d_minors.next && !list_empty(&cdev->d_minors)) {
		_ptrace(("Error path taken!\n"));
		goto ebusy;
	}
	if (cdev->d_apush.next && !list_empty(&cdev->d_apush)) {
		_ptrace(("Error path taken!\n"));
		goto ebusy;
	}
	if (cdev->d_stlist.next && !list_empty(&cdev->d_stlist)) {
		_ptrace(("Error path taken!\n"));
		goto ebusy;
	}
#if defined CONFIG_STREAMS_SYNCQS
	unregister_strsync((struct fmodsw *) cdev);
#endif
	sdev_del(cdev);
	write_unlock(&cdevsw_lock);
	sdev_rel(cdev);
	return (0);
      unlock_exit:
	write_unlock(&cdevsw_lock);
	return (err);
      einval:
	err = -EINVAL;
	goto unlock_exit;
      enxio:
	err = -ENXIO;
	goto unlock_exit;
      ebusy:
	err = -EBUSY;
	goto unlock_exit;
}

EXPORT_SYMBOL_NOVERS(unregister_strdrv);

/*
 *  register_xinode:	- register a character device inode
 *  @cdev:	character device switch structure pointer
 *  @major:	the major device number to register
 *  @fops:	file operations to apply to external character major device nodes
 *
 *  Registers and extenal character special device major number with Linux outside the STREAMS
 *  subsystem.  @fops is the file operations that will be used to open the character device.
 *
 *  The major device number can be specified as zero (0), in which case the major device number will
 *  be assigned to a free major device number by register_chrdev() and returned as a positive return
 *  value.  Any valid external major device number can be used for @major.
 *
 *  register_xinode() can be called multiple times for the same registered cdevsw entries to
 *  register additional major device numbers for the same entry.  On each call to register_xinode()
 *  only one character major device number will be allocated.  If @major is zero on each call, a new
 *  available major device number will be allocated on each call.
 */
STATIC int
register_xinode(struct cdevsw *cdev, struct devnode *cmaj, major_t major,
		struct file_operations *fops)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0] || !cmaj) {
			_ptrace(("Error path taken!\n"));
			break;
		}
		err = -EINVAL;
#ifndef MAX_CHRDEV
		if (major != MAJOR(MKDEV(major, 0)))
#else
		if (major >= MAX_CHRDEV)
#endif
		{
			_ptrace(("Error path taken!\n"));
			break;
		}
		err = -EINVAL;
		/* ensure that the device is registered (as a module) */
		if (!cdev->d_list.next || list_empty(&cdev->d_list)) {
			_ptrace(("Error path taken!\n"));
			break;
		}
		err = -EBUSY;
		if (major && __cmaj_lookup(major)) {
			_ptrace(("Error path taken!\n"));
			break;
		}
#ifdef CONFIG_STREAMS_DEBUG
		if (fops->owner)
			_printd(("%s: [%s] count is now %d\n", __FUNCTION__,
				 fops->owner->name, module_refcount(fops->owner)));
		else
			_printd(("%s: new f_ops have no owner!\n", __FUNCTION__));
#endif
		/* register the character device */
		if ((err = register_chrdev(major, cdev->d_name, fops)) < 0) {
			_ptrace(("Error path taken!\n"));
			break;
		}
#ifdef CONFIG_STREAMS_DEBUG
		if (fops->owner)
			_printd(("%s: [%s] count is now %d\n", __FUNCTION__,
				 fops->owner->name, module_refcount(fops->owner)));
		else
			_printd(("%s: new f_ops have no owner!\n", __FUNCTION__));
#endif
		if (err > 0 && major == 0)
			major = err;
		cmaj_add(cmaj, cdev, major);
		err = major;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

/*
 *  unregister_xinode: - unregister a special device inode
 *  @cdev: character device switch structure pointer
 *  @major: major device number (character special devices only)
 *
 *  Deregisters an external character device major number previously registered with
 *  register_xinode().  @major must be the major number returned from a successful call to
 *  register_xinode() or the special value zero (0).  When @major is specified, all major device
 *  numbers associated with the driver will be deregistered.  In this way, one one call to
 *  unregister_xinode() with @major set to zero (0) is necessary after multiple successful calls to
 *  register_xinode().
 *
 *  Notices: The major device number must be the major device number returned from a successful
 *  register_xinode() call.  cdev->d_name must have the same value as on the
 *  call to register_xinode()
 *  or the call will fail.
 */
STATIC int
unregister_xinode(struct cdevsw *cdev, struct devnode *cmaj, major_t major)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		struct devnode *d;

		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		err = -EINVAL;
#ifndef MAX_CHRDEV
		if (major != MAJOR(MKDEV(major, 0)))
#else
		if (major >= MAX_CHRDEV)
#endif
			break;
		if (major) {
			/* deregister one specific major device number */
			err = -ENXIO;
			if (!(d = __cmaj_lookup(major)))
				break;
			err = -EPERM;
			if (d != cmaj)
				break;
			if ((err = unregister_chrdev(major, cdev->d_name)) < 0)
				break;
			cmaj_del(cmaj, cdev);
		} else {
			struct list_head *pos;

			ensure(cdev->d_majors.next, INIT_LIST_HEAD(&cdev->d_majors));

			/* deregister all major device numbers */
			list_for_each(pos, &cdev->d_majors) {
				cmaj = list_entry(pos, struct devnode, n_list);

				unregister_chrdev(cmaj->n_major, cdev->d_name);
				cmaj_del(cmaj, cdev);
			}
		}
		err = 0;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

/**
 *  register_cmajor:	- register a major device node
 *  @cdev:	character device switch structure pointer
 *  @major:	the major device number to register
 *  @fops:	the file operations to use for external character major device nodes
 *
 *  Registers an internal and external character special major device number.  The internal
 *  registration is made with the specfs filesystem, the external registration is made as a normal
 *  Linux character device.  @fops are the file operations to associate with the external character
 *  special major device. @cdev->d_fop are the file operations to associate with the internal
 *  character special major device within the specfs filesystem.
 *
 *  @major, the major device number, can be specified as zero (0), in which case the major device
 *  number will be assigned to a free major device number by the Linux register_chrdev() function
 *  and returned as a positive return value.  Any valid external major device number can be used for
 *  @major.
 *
 *  register_cmajor() can be called multiple times for the same registered @cdev entries to register
 *  additional external major device numbers for the same entry.  On each call to register_cmajor()
 *  only one character major device number will be allocated.  If @major is zero on each call, a new
 *  available external major device number will be allocated on each call.
 */
streams_fastcall int
register_cmajor(struct cdevsw *cdev, major_t major, struct file_operations *fops)
{
	int err;
	struct devnode *cmaj;

	if ((err = register_strdrv(cdev)) < 0 && err != -EBUSY)
		goto no_strdrv;
	err = -ENOMEM;
	if (!(cmaj = kmalloc(sizeof(*cmaj), GFP_ATOMIC)))
		goto no_cmaj;
	memset(cmaj, 0, sizeof(*cmaj));
	INIT_LIST_HEAD(&cmaj->n_list);
	INIT_LIST_HEAD(&cmaj->n_hash);
	cmaj->n_name = cdev->d_name;
	cmaj->n_str = cdev->d_str;
	cmaj->n_flag = cdev->d_flag;
	cmaj->n_modid = cdev->d_modid;
	cmaj->n_count = (atomic_t) ATOMIC_INIT(0);
	cmaj->n_sqlvl = cdev->d_sqlvl;
	cmaj->n_syncq = cdev->d_syncq;
	cmaj->n_kmod = cdev->d_kmod;
	cmaj->n_major = major;
	cmaj->n_mode = cdev->d_mode;
	cmaj->n_minor = 0;
	cmaj->n_dev = cdev;
	if ((err = register_xinode(cdev, cmaj, major, fops)) < 0)
		goto no_xinode;
	return (err);
      no_xinode:
	kfree(cmaj);
      no_cmaj:
	unregister_strdrv(cdev);
      no_strdrv:
	return (err);
}

EXPORT_SYMBOL_NOVERS(register_cmajor);

streams_fastcall int
unregister_cmajor(struct cdevsw *cdev, major_t major)
{
	int err;
	struct devnode *cmaj;

	err = -ENXIO;
	if (!(cmaj = cmaj_get(cdev, major))) {
		_ptrace(("Error path taken!\n"));
		goto error;
	}
	if ((err = unregister_xinode(cdev, cmaj, major)) < 0) {
		_ptrace(("Error path taken!\n"));
		goto error;
	}
	kfree(cmaj);
	if ((err = unregister_strdrv(cdev)) < 0 && err != -EBUSY) {
		_ptrace(("Error path taken!\n"));
		goto error;
	}
	return (0);
      error:
	return (err);
}

EXPORT_SYMBOL_NOVERS(unregister_cmajor);

/**
 *  register_strnod:	- register a minor device node
 *  @cdev:	character device switch structure pointer
 *  @cmin:	minor device node structure pointer
 *  @minor:	minor device number
 */
streams_fastcall int
register_strnod(struct cdevsw *cdev, struct devnode *cmin, minor_t minor)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		struct devnode *n;

		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		err = -EINVAL;
		if (!cmin || !cmin->n_name || !cmin->n_name[0])
			break;
		err = -EINVAL;
		/* ensure that the device is regsitered (as a module) */
		if (!cdev->d_list.next || list_empty(&cdev->d_list))
			break;

		ensure(cdev->d_majors.next, INIT_LIST_HEAD(&cdev->d_majors));
		ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));

		/* check name for another cmin */
		if ((n = __cmin_search(cdev, cmin->n_name))) {
			err = -EPERM;
			if (n != cmin)
				break;
			err = -EBUSY;
			break;
		}
		if ((minor == (minor_t) (-1UL))) {
			/* find a free minor */
			for (minor = 0; (minor != (minor_t) (-1UL)) && __cmin_lookup(cdev, minor);
			     minor++) ;
			err = -ENXIO;
			if ((minor == (minor_t) (-1UL))) {
				break;
			}
		} else {
			/* use specified minor */
			if ((n = __cmin_lookup(cdev, minor))) {
				err = -EPERM;
				if (n != cmin)
					break;
				err = -EBUSY;
				break;
			}
		}
		write_unlock(&cdevsw_lock);
		err = cmin_ini(cmin, cdev, minor);
		write_lock(&cdevsw_lock);
		if (err)
			break;
		if ((err = __cmin_lookup(cdev, minor) ? -EPERM : 0))
			goto unlock_release_exit;
		if ((err = cmin_add(cmin, cdev)))
			goto unlock_release_exit;
		err = minor;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
      unlock_release_exit:
	write_unlock(&cdevsw_lock);
	cmin_rel(cmin);
	return (err);
}

EXPORT_SYMBOL_NOVERS(register_strnod);

/**
 *  unregister_strnod: - unregister a minor device node
 *  @cdev: character device switch structure pointer
 *  @cmin: minor device node structure pointer
 *  @minor: minor device number
 */
streams_fastcall int
unregister_strnod(struct cdevsw *cdev, minor_t minor)
{
	int err = 0;

	write_lock(&cdevsw_lock);
	do {
		struct devnode *cmin;

		err = -EINVAL;
		if (!cdev)
			break;
		if (minor != (minor_t) (-1UL)) {
			/* deregister one specific minor device number */
			err = -ENXIO;
			if (!(cmin = __cmin_lookup(cdev, minor)))
				break;
			cmin_del(cmin, cdev);
			write_unlock(&cdevsw_lock);
			cmin_rel(cmin);
			return (0);
		} else {
			/* deregister all minor devices */
			struct list_head *pos;

			ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));
			/* deregister all minor device numbers */
			list_for_each(pos, &cdev->d_minors) {
				cmin = list_entry(pos, struct devnode, n_list);

				cmin_del(cmin, cdev);
				write_unlock(&cdevsw_lock);
				cmin_rel(cmin);
				write_lock(&cdevsw_lock);
			}
			INIT_LIST_HEAD(&cdev->d_minors);
		}
		err = 0;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

EXPORT_SYMBOL_NOVERS(unregister_strnod);
