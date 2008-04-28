/*****************************************************************************

 @(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.78 $) $Date: 2008-04-28 12:54:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-04-28 12:54:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strreg.c,v $
 Revision 0.9.2.78  2008-04-28 12:54:05  brian
 - update file headers for release

 Revision 0.9.2.77  2007/12/15 20:19:56  brian
 - updates

 Revision 0.9.2.76  2007/10/18 06:53:56  brian
 - added streams notification registration

 Revision 0.9.2.75  2007/08/13 22:46:17  brian
 - GPLv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.78 $) $Date: 2008-04-28 12:54:05 $"

static char const ident[] =
    "$RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.78 $) $Date: 2008-04-28 12:54:05 $";

#include <linux/compiler.h>
#include <linux/autoconf.h>
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

#ifdef HAVE_KINC_LINUX_CDEV_H
#include <linux/cdev.h>
#endif
#if defined HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>	/* avoid ptrace conflict */
#endif

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
#include "src/kernel/strutil.h"	/* for global_inner_syncq */
#include "src/kernel/strlookup.h"	/* cdevsw_list, etc. */
#include "src/kernel/strreg.h"	/* extern verification */
/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and Deregistration
 *
 *  -------------------------------------------------------------------------
 */

rwlock_t strreg_lock = RW_LOCK_UNLOCKED;
struct list_head strreg_list = LIST_HEAD_INIT(strreg_list);

/**
 * streams_notify: - notify a notifier call chain of a STREAMS event
 * @event: the STREAMS event which has occurred
 * @type: the type of module: %STR_IS_MODULE or %STR_IS_DEVICE
 * @modid: the Module Id Number for the STREAMS module.
 *
 * STREAMS notifiers are used to notify clients that an event has occurred.  Current notifications
 * are:
 *
 * %STREAMS_NTFY_REG (a STREAMS module or device has registered),
 * %STREAMS_NTFY_DEREG (a STREAMS module or device has deregistered),
 * %STREAMS_NTFY_APUSH_ADD (a STREAMS autopush definition has been added),
 * %STREAMS_NTFY_APUSH_DEL (a STREAMS autopush defintiion has been removed).
 *
 * The major purpose of this notifier is to permit the sc(4) module and the sad(4) driver with the
 * ability to provide indications to control STREAMS that a configuration change has occured.  This
 * mechanism is used by user-space processes holding open sc(4) or sad(4) Streams to know when to
 * reread cached configuration data.  The mechanism is used by the strmib(8) STREAMS SNMP sub-agent.
 *
 * Notifier callbacks are called with the notifier chain read locked.  The callback must not attempt
 * to register or deregister the notifier or single-party deadlock will occur.
 */
streams_fastcall int
streams_notify(int event, int type, modID_t modid)
{
	struct list_head *pos, *slot = &strreg_list;
	int count = 0;

	read_lock(&strreg_lock);
	list_for_each(pos, slot) {
		struct streams_notify *sn;
		
		sn = list_entry(pos, struct streams_notify, sn_list);
		(*sn->sn_notify)(event, type, modid, sn);
		count++;
	}
	read_unlock(&strreg_lock);
	return (count);
}

EXPORT_SYMBOL_GPL(streams_notify);

/**
 * streams_register_notifier: - register a STREAMS notifier
 * @sn: pointer to a struct streams_notify structure
 *
 * Registers a STREAMS notifier. @sn is a pointer to a caller-allocated structure that has the
 * sn_notify member completed.  Returns zero (0) on success and -EINVAL on failure.
 */
streamscall int
streams_register_notifier(struct streams_notify *sn)
{
	if (sn == NULL)
		return (-EINVAL);
	INIT_LIST_HEAD(&sn->sn_list);
	if (sn->sn_notify == NULL)
		return (-EINVAL);
	write_lock(&strreg_lock);
	list_add(&sn->sn_list, &strreg_list);
	write_unlock(&strreg_lock);
	return (0);
}

EXPORT_SYMBOL_GPL(streams_register_notifier);

/**
 * streams_unregister_notifier: - unregister a STREAMS notifier
 * @sn: pointer to a struct streams_notify structure
 *
 * Deregisters a STREAMS notifier.  @sn is a pointer to a caller-allocated structure that was
 * previously registered with streams_register_notifier().  Returns zero (0) on success and -EINVAL
 * on failure.
 */
streamscall int
streams_unregister_notifier(struct streams_notify *sn)
{
	if (sn == NULL)
		return (-EINVAL);
	write_lock(&strreg_lock);
	list_del_init(&sn->sn_list);
	write_unlock(&strreg_lock);
	return (0);
}

EXPORT_SYMBOL_GPL(streams_unregister_notifier);

/**
 * register_strsync: - register syncrhonization queueso
 * @fmod: pointer to module structure (whether module or driver)
 *
 * This function is responsible for registering non-queue specific syncrhonization queues.  Normally
 * there is an inner barrier and an optional outer barrier.  The inner barrier is specified by the
 * syncrhonization level.  An outer barrier only need exist at a sycnchronization level wider than
 * the inner barrier.  The outer barrier depends upon the flavor.  Solaris uses and outer barrier
 * that is shared or exclusive (to open and close) that is always at the module level.  HPUX always
 * uses an outer barrier exclusive to open and close that is global.  MacOT (and presumably AIX) use
 * an outer barrier exclusive to open and close that is no smaller than a queue pair.  That is, when
 * SQLVL_QUEUE is set for the inner barrier, an SQLVL_QUEUEPAIR exclusive to open and close outer
 * barrier exists.  (Note that IRIX requires all STREAMS drivers and modules to be D_MP.)
 */
streams_fastcall int
register_strsync(struct fmodsw *fmod)
{
#if defined CONFIG_STREAMS_SYNCQS
	int sqlvl = SQLVL_DEFAULT;
	struct syncq *sq;

	/* Make sure none is allocated because LiS emulation calls this function repeatedly for the
	   same module or driver. */
	sq_put(&fmod->f_syncq);

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
			if (!(sq = sq_alloc()))
				return (-ENOMEM);
			sq->sq_level = SQLVL_MODULE;
			sq->sq_flag = SQ_OUTER | ((fmod->f_flag & D_MTOCEXCL) ? SQ_EXCLUS : 0);
			fmod->f_syncq = sq;
		}
	} else {
		switch (sqlvl) {
		case SQLVL_NOP:
			break;
		case SQLVL_QUEUE:
		case SQLVL_QUEUEPAIR:
			/* Note that HPUX registration function sets D_MTOCEXCL without setting
			   D_MTOUTPERIM meaning that a global outer perimeter exists. */
			if (fmod->f_flag & D_MTOCEXCL) {
				if (!(sq = sq_get(fmod->f_syncq))) {
					if (!(sq = sq_get(global_outer_syncq))) {
						if (!(sq = sq_alloc()))
							return (-ENOMEM);
						/* Note that the global outer synchronization queue 
						   will exist until STREAMS is unloaded. */
						sq->sq_level = SQLVL_GLOBAL;
						sq->sq_flag = SQ_OUTER | SQ_EXCLUS;
						global_outer_syncq = sq_get(sq);
					}
					fmod->f_syncq = sq;
				}
			}
			break;
		case SQLVL_DEFAULT:
		case SQLVL_MODULE:	/* default */
			if (!(sq = sq_get(fmod->f_syncq))) {
				if (!(sq = sq_alloc()))
					return (-ENOMEM);
				sq->sq_level = sqlvl;
				sq->sq_flag = SQ_INNER |
				    ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
				fmod->f_syncq = sq;
			}
			goto get_outer;
		case SQLVL_ELSEWHERE:
			if (!(sq = sq_get(fmod->f_syncq))) {
				if (!(sq = sq_locate(fmod->f_sqinfo)))
					return (-ENOMEM);
				sq->sq_level = sqlvl;
				sq->sq_flag = SQ_INNER |
				    ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
				fmod->f_syncq = sq;
			}
		      get_outer:
			/* Note that HPUX registration function sets D_MTOCEXCL without setting
			   D_MTOUTPERIM meaning that a global outer perimeter exists. */
			if (fmod->f_flag & D_MTOCEXCL) {
				if (!(sq = fmod->f_syncq->sq_outer)) {
					if (!(sq = sq_get(global_outer_syncq))) {
						if (!(sq = sq_alloc())) {
							sq_release(&fmod->f_syncq);
							return (-ENOMEM);
						}
						/* Note that the global outer synchronization queue 
						   will exist until STREAMS is unloaded. */
						sq->sq_level = SQLVL_GLOBAL;
						sq->sq_flag = SQ_OUTER | SQ_EXCLUS;
						global_outer_syncq = sq_get(sq);
					}
					fmod->f_syncq->sq_outer = sq;
				}
			}
			break;
		case SQLVL_GLOBAL:	/* for testing */
			if (!(sq = sq_get(fmod->f_syncq))) {
				if (!(sq = sq_get(global_inner_syncq))) {
					if (!(sq = sq_alloc()))
						return (-ENOMEM);
					/* Note that the global inner synchronization queue will
					   exist until STREAMS is unloaded. */
					sq->sq_level = sqlvl;
					sq->sq_flag = SQ_INNER |
					    ((fmod->f_flag & D_MTPUTSHARED) ? SQ_SHARED : 0);
					global_inner_syncq = sq_get(sq);
				}
				fmod->f_syncq = sq;
			}
			break;
		default:
			return (-EINVAL);
		}
	}
	fmod->f_sqlvl = sqlvl;
#endif
#if !defined CONFIG_STREAMS_SYNCQS
	/* If there is no synchonization support compiled in, simply refuse to load modules or
	   drivers that are not specified as fully MP-safe.  But for LiS style registration, permit 
	   it.  This is because of the broken registration procedures of LiS. */
	if (fmod->f_sqlvl == SQLVL_DEFAULT) {
		if (fmod->f_flag & D_MP)
			fmod->f_sqlvl = SQLVL_NOP;
	}
	if (!(fmod->f_flag & (D_MP | D_LIS)) && fmod->f_sqlvl != SQLVL_NOP)
		return (-ENOSYS);
#endif
	return (0);
}

EXPORT_SYMBOL_GPL(register_strsync);

streams_fastcall void
unregister_strsync(struct fmodsw *fmod)
{
#if defined CONFIG_STREAMS_SYNCQS
	/* always release if it exists */
	sq_release(&fmod->f_syncq);
#endif
}

EXPORT_SYMBOL_GPL(unregister_strsync);

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
	if ((err = register_strsync(fmod))) {
		_ptrace(("Error path taken!\n"));
		return (err);
	}
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
		write_unlock(&fmodsw_lock);
		/* At this point we notify any STREAMS registration notifier
		 * chain that a STREAMS module has registered. */
		streams_notify(STREAMS_NTFY_REG, STR_IS_MODULE, modid);
		return (modid);
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
	unregister_strsync(fmod);
	write_unlock(&fmodsw_lock);
	return (err);
}

EXPORT_SYMBOL(register_strmod);

/**
 *  unregister_strmod:
 *  @fmod: STREAMS module structure to unregister
 */
streams_fastcall int
unregister_strmod(struct fmodsw *fmod)
{
	int err;
	modID_t modid;

	write_lock(&fmodsw_lock);
	if (!fmod || !fmod->f_name || !fmod->f_name[0])
		goto einval;
	if (!fmod->f_list.next || list_empty(&fmod->f_list))
		goto enxio;
	modid = fmod->f_modid;
	fmod_del(fmod);
	unregister_strsync(fmod);
	write_unlock(&fmodsw_lock);
	/* At this point we notify any STREAMS regsitration notifier chain
	 * that a STREAMS module has deregistered. */
	streams_notify(STREAMS_NTFY_DEREG, STR_IS_MODULE, modid);
	return (0);
      einval:
	err = -EINVAL;
	goto unlock_exit;
      enxio:
	err = -ENXIO;
	goto unlock_exit;
      unlock_exit:
	write_unlock(&fmodsw_lock);
	return (err);
}

EXPORT_SYMBOL(unregister_strmod);

/**
 *  register_strdrv:	- register STREAMS driver to specfs
 *  @cdev:	STREAMS device structure to register
 *  @major:	major device number
 *
 *  register_strdrv() registers the specified cdevsw(9) structure to the specfs(5).  This results in
 *  the allocation of a specfs(5) directory node for the driver.  This is probably not the function
 *  that you want.  See register_strdev(9).
 *
 *  For full details, see register_strdrv(9).
 */
streams_fastcall int
register_strdrv(struct cdevsw *cdev, major_t major)
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
	if ((err = register_strsync((struct fmodsw *) cdev))) {
		_ptrace(("Error path taken!\n"));
		return (err);
	}
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
		if (!(modid = major) && !(modid = mi->mi_idnum)) {
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
		cdev->d_modid = modid;
		if ((err = sdev_add(cdev))) {
			_ptrace(("Error path taken!\n"));
			goto unlock_release_exit;
		}
		write_unlock(&cdevsw_lock);
		/* At this point we notify any STREAMS registration notifier
		 * chain that a STREAMS driver has registered. */
		streams_notify(STREAMS_NTFY_REG, STR_IS_DEVICE, modid);
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
	unregister_strsync((struct fmodsw *) cdev);
	return (err);
}

EXPORT_SYMBOL_GPL(register_strdrv);

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
	modID_t modid;

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
	modid = cdev->d_modid;
	unregister_strsync((struct fmodsw *) cdev);
	sdev_del(cdev);
	write_unlock(&cdevsw_lock);
	sdev_rel(cdev);
	/* At this point we notify any STREAMS registration notifier chain
	 * that a STREAMS driver has de-registered. */
	streams_notify(STREAMS_NTFY_DEREG, STR_IS_DEVICE, modid);
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

EXPORT_SYMBOL_GPL(unregister_strdrv);

#if !defined HAVE_KINC_LINUX_CDEV_H
STATIC int
register_cdev(struct cdevsw *cdev, major_t major, struct file_operations *fops)
{
	return register_chrdev(major, cdev->d_name, fops);
}
STATIC int
unregister_cdev(struct cdevsw *cdev, major_t major)
{
	return unregister_chrdev(major, cdev->d_name);
}
#else				/* defined HAVE_KINC_LINUX_CDEV_H */
STATIC int
register_cdev(struct cdevsw *cdev, major_t major, struct file_operations *fops)
{
	int err;
	__kernel_dev_t dev = 0;

	if (major == 0) {
		err = alloc_chrdev_region(&dev, 0, 1 << 16, (char *) cdev->d_name);
		if (err == 0)
			major = MAJOR(dev);
	} else {
		dev = MKDEV(major, 0);
		err = register_chrdev_region(dev, 1 << 16, (char *) cdev->d_name);
	}
	if (err < 0)
		return (err);

	err = -ENOMEM;
	if ((cdev->d_cdev = cdev_alloc()) == NULL)
		goto unregister_out;

	cdev_init(cdev->d_cdev, fops);
	if ((err = cdev_add(cdev->d_cdev, dev, 1 << 16)))
		goto delete_out;

	return (major);

      delete_out:
	cdev_del(XCHG(&cdev->d_cdev, NULL));

      unregister_out:
	unregister_chrdev_region(dev, 1 << 16);

	return (err);

}
STATIC int
unregister_cdev(struct cdevsw *cdev, major_t major)
{
	dev_t dev = MKDEV(major, 0);

	cdev_del(XCHG(&cdev->d_cdev, NULL));
	unregister_chrdev_region(dev, 1 << 16);
	return (0);
}
#endif				/* defined HAVE_KINC_LINUX_CDEV_H */

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
 *  be assigned to a free major device number by register_cdev() and returned as a positive return
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
#ifdef _DEBUG
		if (fops->owner)
			_printd(("%s: [%s] count is now %d\n", __FUNCTION__,
				 fops->owner->name, module_refcount(fops->owner)));
		else
			_printd(("%s: new f_ops have no owner!\n", __FUNCTION__));
#endif
		/* register the character device */
		if ((err = register_cdev(cdev, major, fops)) < 0) {
			_ptrace(("Error path taken!\n"));
			break;
		}
#ifdef _DEBUG
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
			if ((err = unregister_cdev(cdev, major)) < 0)
				break;
			cmaj_del(cmaj, cdev);
		} else {
			struct list_head *pos;

			ensure(cdev->d_majors.next, INIT_LIST_HEAD(&cdev->d_majors));

			/* deregister all major device numbers */
			list_for_each(pos, &cdev->d_majors) {
				cmaj = list_entry(pos, struct devnode, n_list);

				unregister_cdev(cdev, cmaj->n_major);
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
 *  number will be assigned to a free major device number by the Linux register_cdev() function
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

	if ((err = register_strdrv(cdev, major)) < 0 && err != -EBUSY)
		goto no_strdrv;
	err = -ENOMEM;
	/* If major is zero, use the module id as the major device number. */
	if (major == 0)
		major = cdev->d_modid;
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

EXPORT_SYMBOL_GPL(register_cmajor);

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

EXPORT_SYMBOL_GPL(unregister_cmajor);

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

EXPORT_SYMBOL_GPL(register_strnod);

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

EXPORT_SYMBOL_GPL(unregister_strnod);
