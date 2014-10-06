/*****************************************************************************

 @(#) $RCSfile: strlookup.c,v $ $Name:  $($Revision: 1.1.2.7 $) $Date: 2011-09-20 09:51:39 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-09-20 09:51:39 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strlookup.c,v $
 Revision 1.1.2.7  2011-09-20 09:51:39  brian
 - updates from git

 Revision 1.1.2.6  2011-05-31 09:46:08  brian
 - new distros

 Revision 1.1.2.5  2011-04-05 16:35:14  brian
 - weak module design

 Revision 1.1.2.4  2011-01-12 04:10:32  brian
 - code updates for 2.6.32 kernel and gcc 4.4

 Revision 1.1.2.3  2010-11-28 14:21:56  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.2  2009-09-01 09:09:51  brian
 - added text image files

 Revision 1.1.2.1  2009-06-21 11:37:16  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] =
    "$RCSfile: strlookup.c,v $ $Name:  $($Revision: 1.1.2.7 $) $Date: 2011-09-20 09:51:39 $";

#include <linux/compiler.h>
#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
#include <linux/kmod.h>
#endif
#include <linux/kernel.h>	/* for FASTCALL(), fastcall */
#include <linux/sched.h>	/* for current */
#include <linux/file.h>		/* for fput */
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/mount.h>	/* for vfsmount stuff */
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
#include <asm/hardirq.h>
#if defined HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>	/* avoid ptrace conflict */
#endif
#if defined HAVE_KINC_LINUX_DEVICE_H
#include <linux/device.h>
#endif

#include "sys/strdebug.h"

#include <sys/kmem.h>		/* for kmem_ */
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#ifndef HAVE_KFUNC_TRY_MODULE_GET
#define try_module_get try_inc_mod_count
#endif
#ifndef HAVE_KFUNC_MODULE_PUT
#define module_put(__m) __MOD_DEC_USE_COUNT((__m))
#define module_refcount(__m) atomic_read(&(__m)->uc.usecount)
#endif

#include "sys/config.h"
#include "src/modules/sth.h"	/* for stream operations */
#include "src/kernel/strspecfs.h"	/* for spec_snode */
#include "src/kernel/strlookup.h"	/* extern verification */

#if defined HAVE_KFUNC_IN_ATOMIC || defined in_atomic
#define can_sleep() (!in_interrupt()&&!in_atomic())
#else				/* defined HAVE_KFUNC_IN_ATOMIC || defined in_atomic */
#define can_sleep() (!in_interrupt())
#endif				/* defined HAVE_KFUNC_IN_ATOMIC || defined in_atomic */

/* we want macro versions of these */

#undef getmajor
#define getmajor(__ino) (((__ino)>>16)&0x0000ffff)

#undef getminor
#define getminor(__ino) (((__ino)>>0)&0x0000ffff)

#undef makedevice
#define makedevice(__maj,__min) ((((__maj)<<16)&0xffff0000)|(((__min)<<0)&0x0000ffff))

/* these don't need to be so big */

#define STRMOD_HASH_ORDER 5
#define STRDEV_HASH_ORDER 5
#define STRNOD_HASH_ORDER 5

#define STRMOD_HASH_SIZE (1 << STRMOD_HASH_ORDER)
#define STRDEV_HASH_SIZE (1 << STRDEV_HASH_ORDER)
#define STRNOD_HASH_SIZE (1 << STRNOD_HASH_ORDER)

#define STRMOD_HASH_MASK ((unsigned long)(STRMOD_HASH_SIZE-1))
#define STRDEV_HASH_MASK ((unsigned long)(STRDEV_HASH_SIZE-1))
#define STRNOD_HASH_MASK ((unsigned long)(STRNOD_HASH_SIZE-1))

#define strmod_hash_slot(_modid) \
	(fmodsw_hash + (((_modid) + ((_modid) >> STRMOD_HASH_ORDER) + ((_modid) >> 2 * STRMOD_HASH_ORDER)) & STRMOD_HASH_MASK))

#define strdev_hash_slot(_major) \
	(cdevsw_hash + (((_major) + ((_major) >> STRDEV_HASH_ORDER) + ((_major) >> 2 * STRDEV_HASH_ORDER)) & STRDEV_HASH_MASK))

#define strnod_hash_slot(_minor) \
	(cminsw_hash + (((_minor) + ((_minor) >> STRNOD_HASH_ORDER) + ((_minor) >> 2 * STRNOD_HASH_ORDER)) & STRNOD_HASH_MASK))

#if	defined DEFINE_RWLOCK
DEFINE_RWLOCK(cdevsw_lock);
DEFINE_RWLOCK(fmodsw_lock);
#elif	defined __RW_LOCK_UNLOCKED
rwlock_t cdevsw_lock = __RW_LOCK_UNLOCKED(cdevsw_lock);
rwlock_t fmodsw_lock = __RW_LOCK_UNLOCKED(fmodsw_lock);
#elif	defined RW_LOCK_UNLOCKED
rwlock_t cdevsw_lock = RW_LOCK_UNLOCKED;
rwlock_t fmodsw_lock = RW_LOCK_UNLOCKED;
#else
#error cannot ininitalized read-write locks
#endif


#if 0
rwlock_t cminsw_lock = RW_LOCK_UNLOCKED;
#endif

struct list_head cdevsw_list = LIST_HEAD_INIT(cdevsw_list);	/* Devices go here */
struct list_head fmodsw_list = LIST_HEAD_INIT(fmodsw_list);	/* Modules go here */

#if 0
struct list_head cminsw_list = LIST_HEAD_INIT(cminsw_list);	/* Minors go here */
#endif

EXPORT_SYMBOL_GPL(cdevsw_lock);
EXPORT_SYMBOL_GPL(cdevsw_list);
EXPORT_SYMBOL_GPL(fmodsw_lock);
EXPORT_SYMBOL_GPL(fmodsw_list);
#if 0
EXPORT_SYMBOL_GPL(cminsw_lock);
EXPORT_SYMBOL_GPL(cminsw_list);
#endif

struct list_head fmodsw_hash[STRMOD_HASH_SIZE] __cacheline_aligned = { {NULL,}, };
struct list_head cdevsw_hash[STRDEV_HASH_SIZE] __cacheline_aligned = { {NULL,}, };
struct list_head cminsw_hash[STRNOD_HASH_SIZE] __cacheline_aligned = { {NULL,}, };

int cdev_count = 0;
int fmod_count = 0;
int cmin_count = 0;

EXPORT_SYMBOL_GPL(cdev_count);
EXPORT_SYMBOL_GPL(fmod_count);
EXPORT_SYMBOL_GPL(cmin_count);

/*
 *  -------------------------------------------------------------------------
 *
 *  Hash functions
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  init_fmod_hash: - initialize the list_head structures in the fmod hash
 */
STATIC void
init_fmod_hash(void)
{
	int i;

	for (i = 0; i < STRMOD_HASH_SIZE; i++)
		INIT_LIST_HEAD((fmodsw_hash + i));
}

/**
 *  init_cdev_hash: - initialize the list_head structures in the cdev hash
 */
STATIC void
init_cdev_hash(void)
{
	int i;

	for (i = 0; i < STRDEV_HASH_SIZE; i++)
		INIT_LIST_HEAD((cdevsw_hash + i));
}

/**
 *  init_cmin_hash: - initialize the list_head structures in the cmin hash
 */
STATIC void
init_cmin_hash(void)
{
	int i;

	for (i = 0; i < STRNOD_HASH_SIZE; i++)
		INIT_LIST_HEAD((cminsw_hash + i));
}

/**
 *  __cmaj_lookup: - look up a cmaj by major device number in cmaj hashes
 *  @major: major device number to look up
 *
 *  Look up a STREAMS device information structure by external major device number in the devnode
 *  hashes without locking or acquisition of the result.  This function can be called multiple times
 *  with the same @major with very little performance impact.
 */
streams_fastcall struct devnode *
__cmaj_lookup(major_t major)
{
	struct list_head *pos, *slot = strdev_hash_slot(major);

	list_for_each(pos, slot) {
		struct devnode *cmaj = list_entry(pos, struct devnode, n_hash);

		if (cmaj->n_major == major) {
#if 0
			/* cache to front */
			list_del_init(&cmaj->n_hash);
			list_add(&cmaj->n_hash, slot);
#endif
			return (cmaj);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_GPL(__cmaj_lookup);

/**
 *  __cdev_lookup: - look up a cdev by major device number in cdev hashes
 *  @major: major device number to look up
 *
 *  Look up a STREAMS driver by major device number in the cdevsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @major with
 *  very little performance impact.
 */
streams_fastcall struct cdevsw *
__cdev_lookup(major_t major)
{
	struct devnode *cmaj;

	if ((cmaj = __cmaj_lookup(major)))
		return (cmaj->n_dev);
	return (NULL);
}

EXPORT_SYMBOL_GPL(__cdev_lookup);

/**
 *  __cdrv_lookup: - look up a cdev by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *
 *  Look up a STREAMS module by module identifier in the fmodsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @modid with
 *  very little performance impact.
 */
streams_fastcall struct cdevsw *
__cdrv_lookup(modID_t modid)
{
	struct list_head *pos, *slot = strmod_hash_slot(modid);

	list_for_each(pos, slot) {
		struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_hash);

		if (cdev->d_modid == modid) {
#if 0
			/* cache to front */
			list_del_init(&cdev->d_hash);
			list_add(&cdev->d_hash, slot);
#endif
			return (cdev);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_GPL(__cdrv_lookup);

/**
 *  __cmin_lookup: - look up a node by cdev and minor device number in node hashes
 *  @cdev: pointer to character device switch entry
 *  @minor: minor device number
 */
streams_fastcall struct devnode *
__cmin_lookup(struct cdevsw *cdev, minor_t minor)
{
	struct list_head *pos, *slot = strnod_hash_slot(minor);

	list_for_each(pos, slot) {
		struct devnode *cmin = list_entry(pos, struct devnode, n_hash);

		if (cmin->n_dev == cdev && cmin->n_minor == minor) {
#if 0
			/* pull to head of slot */
			list_del_init(&cmin->n_hash);
			list_add(&cmin->n_hash, slot);
#endif
			return (cmin);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_GPL(__cmin_lookup);

/**
 *  __fmod_lookup: - look up a fmod by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *
 *  Look up a STREAMS module by module identifier in the fmodsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @modid with
 *  very little performance impact.
 */
streams_fastcall struct fmodsw *
__fmod_lookup(modID_t modid)
{
	struct list_head *pos, *slot = strmod_hash_slot(modid);

	list_for_each(pos, slot) {
		struct fmodsw *fmod = list_entry(pos, struct fmodsw, f_hash);

		if (fmod->f_modid == modid) {
#if 0
			/* pull to front of slot */
			list_del_init(&fmod->f_hash);
			list_add(&fmod->f_hash, slot);
#endif
			return (fmod);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_GPL(__fmod_lookup);

streams_fastcall struct cdevsw *
__cdev_search(const char *name)
{
	struct list_head *pos, *slot = &cdevsw_list;

	list_for_each(pos, slot) {
		struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_list);

		if (!strncmp(cdev->d_name, name, FMNAMESZ)) {
#if 0
			/* pull to front of slot */
			list_del_init(&cdev->d_list);
			list_add(&cdev->d_list, slot);
#endif
			return (cdev);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_GPL(__cdev_search);

streams_fastcall struct fmodsw *
__fmod_search(const char *name)
{
	struct list_head *pos, *slot = &fmodsw_list;

	list_for_each(pos, slot) {
		struct fmodsw *fmod = list_entry(pos, struct fmodsw, f_list);

		if (!strncmp(fmod->f_name, name, FMNAMESZ)) {
#if 0
			/* pull to front of slot */
			list_del_init(&fmod->f_list);
			list_add(&fmod->f_list, slot);
#endif
			return (fmod);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_GPL(__fmod_search);

streams_fastcall struct devnode *
__cmin_search(struct cdevsw *cdev, const char *name)
{
	struct list_head *pos, *slot = &cdev->d_minors;

	ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));

	list_for_each(pos, slot) {
		struct devnode *cmin = list_entry(pos, struct devnode, n_list);

		if (!strncmp(cmin->n_name, name, FMNAMESZ)) {
#if 0
			/* pull to head of slot */
			list_del_init(&cmin->n_list);
			list_add(&cmin->n_list, slot);
#endif
			return (cmin);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_GPL(__cmin_search);

streams_fastcall void *
__smod_search(const char *name)
{
	void *fmod = NULL;

	if (!fmod)
		fmod = __cdev_search(name);
	if (!fmod)
		fmod = __fmod_search(name);
	return (fmod);
}

EXPORT_SYMBOL_GPL(__smod_search);

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
STATIC struct cdevsw *
cdev_grab(struct cdevsw *cdev)
{
	if (cdev) {
		if (cdev->d_str) {
			if (try_module_get(cdev->d_kmod)) {
				_ptrace(("%s: %s: incremented mod count\n", __FUNCTION__,
					 cdev->d_name));
				_printd(("%s: %s: [%s] count is now %d\n", __FUNCTION__,
					 cdev->d_name, cdev->d_kmod->name,
					 module_refcount(cdev->d_kmod)));
				return (cdev);
			}
		}
		cdev = NULL;
	}
	return (cdev);
}
#endif

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
STATIC struct fmodsw *
fmod_grab(struct fmodsw *fmod)
{
	if (fmod) {
		if (fmod->f_str) {
			if (try_module_get(fmod->f_kmod)) {
				_ptrace(("%s: %s: incremented mod count\n", __FUNCTION__,
					 fmod->f_name));
				_printd(("%s: %s: [%s] count is now %d\n", __FUNCTION__,
					 fmod->f_name, fmod->f_kmod->name,
					 module_refcount(fmod->f_kmod)));
				return (fmod);
			}
		}
		fmod = NULL;
	}
	return (fmod);
}
#endif

/*
 *  cdev_lookup: - look up a cdev by major device number in cdev hashes
 *  @major: major device number to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct cdevsw *
cdev_lookup(major_t major, int load)
{
	struct cdevsw *cdev = NULL;

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	int reload;

	read_lock(&cdevsw_lock);
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char devname[64];

			if ((cdev = __cdev_lookup(major)))
				break;
			if (!load)
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "char-major-%d", major);
			request_module(devname);
			read_lock(&cdevsw_lock);
			if ((cdev = __cdev_lookup(major)))
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "streams-major-%d", major);
			request_module(devname);
			read_lock(&cdevsw_lock);
#if defined CONFIG_DEVFS || 1
			if ((cdev = __cdev_lookup(major)))
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "/dev/streams/%d", major);
			request_module(devname);
			read_lock(&cdevsw_lock);
#endif				/* CONFIG_DEVFS */
		} while (0);
		/* try to acquire the module */
		if ((cdev = cdev_grab(cdev)))
			break;
	}
	read_unlock(&cdevsw_lock);
#else
	read_lock(&cdevsw_lock);
	cdev = __cdev_lookup(major);
	read_unlock(&cdevsw_lock);
#endif				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	return (cdev);
}

/*
 *  cdrv_lookup: - look up a cdev by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct cdevsw *
cdrv_lookup(modID_t modid, int load)
{
	struct cdevsw *cdev = NULL;

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	int reload;

	read_lock(&fmodsw_lock);
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char modname[64];

			if ((cdev = __cdrv_lookup(modid)))
				break;
			if (!load)
				break;
			read_unlock(&fmodsw_lock);
			snprintf(modname, sizeof(modname), "streams-modid-%d", modid);
			request_module(modname);
			read_lock(&fmodsw_lock);
			if ((cdev = __cdrv_lookup(modid)))
				break;
		} while (0);
		/* try to acquire the module */
		if ((cdev = cdev_grab(cdev)))
			break;
	}
	read_unlock(&fmodsw_lock);
#else
	read_lock(&fmodsw_lock);
	cdev = __cdrv_lookup(modid);
	read_unlock(&fmodsw_lock);
#endif				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	return (cdev);
}

/*
 *  fmod_lookup: - look up a fmod by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct fmodsw *
fmod_lookup(modID_t modid, int load)
{
	struct fmodsw *fmod = NULL;

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	int reload;

	read_lock(&fmodsw_lock);
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char modname[64];

			if ((fmod = __fmod_lookup(modid)))
				break;
			if (!load)
				break;
			read_unlock(&fmodsw_lock);
			snprintf(modname, sizeof(modname), "streams-modid-%d", modid);
			request_module(modname);
			read_lock(&fmodsw_lock);
			if ((fmod = __fmod_lookup(modid)))
				break;
		} while (0);
		/* try to acquire the module */
		if ((fmod = fmod_grab(fmod)))
			break;
	}
	read_unlock(&fmodsw_lock);
#else
	read_lock(&fmodsw_lock);
	fmod = __fmod_lookup(modid);
	read_unlock(&fmodsw_lock);
#endif				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	return (fmod);
}

/**
 *  cmaj_lookup: - look up a devnode by device and major device number
 *  @cdev:	cdevsw structure for the driver
 *  @major:	major device number to look up
 */
STATIC struct devnode *
cmaj_lookup(const struct cdevsw *cdev, major_t major)
{
	struct devnode *cmaj = NULL;

	ensure(cdev->d_majors.next, return (cmaj));

	read_lock(&cdevsw_lock);
	if (cdev) {
		register struct list_head *pos;

		list_for_each(pos, &cdev->d_majors) {
			struct devnode *d = list_entry(pos, struct devnode, n_list);

			if (d->n_major == major) {
				cmaj = d;
				break;
			}
		}
	}
	read_unlock(&cdevsw_lock);
	return (cmaj);
}

/*
 *  cmin_lookup: - look up a devnode by device and minor device number
 *  @cdev:	cdevsw structure for the driver
 *  @minor:	minor device number to look up
 */
STATIC struct devnode *
cmin_lookup(const struct cdevsw *cdev, minor_t minor)
{
	struct devnode *cmin = NULL;

	ensure(cdev->d_minors.next, return (cmin));

	read_lock(&cdevsw_lock);
	if (cdev) {
		register struct list_head *pos;

		list_for_each(pos, &cdev->d_minors) {
			struct devnode *n = list_entry(pos, struct devnode, n_list);

			if (n->n_minor == minor) {
				cmin = n;
				break;
			}
		}
	}
	read_unlock(&cdevsw_lock);
	return (cmin);
}

/**
 *  cdev_search: - look up a cdev by name
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 *
 *  The search is exhaustive.  If this function is called multiple times with the same name, the
 *  peformance impact will be minimal.  If the device is not found by name, an attempt will be made
 *  to demand load the kernel module "streams-%s" and then "/dev/streams/%s".
 */
STATIC struct cdevsw *
cdev_search(const char *name, int load)
{
	struct cdevsw *cdev = NULL;

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	int reload;
#endif
	read_lock(&cdevsw_lock);
#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char devname[64];

			if ((cdev = __cdev_search(name)))
				break;
			if (!load)
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "streams-%s", name);
			request_module(devname);
			read_lock(&cdevsw_lock);
#if defined CONFIG_DEVFS || 1
			if ((cdev = __cdev_search(name)))
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "/dev/streams/%s", name);
			request_module(devname);
			read_lock(&cdevsw_lock);
#endif				/* CONFIG_DEVFS */
		} while (0);
		/* try to acquire the module */
		if (cdev && cdev->d_str)
			if (try_module_get(cdev->d_kmod)) {
				_ptrace(("%s: %s: incremented mod count\n", __FUNCTION__,
					 cdev->d_name));
				_printd(("%s: %s: [%s] count is now %d\n", __FUNCTION__,
					 cdev->d_name, cdev->d_kmod->name,
					 module_refcount(cdev->d_kmod)));
				break;
			}
		cdev = NULL;
	}
#else				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	cdev = __cdev_search(name);
#endif				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	read_unlock(&cdevsw_lock);
	return (cdev);
}

/*
 *  fmod_search: - look up a fmod by name
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 *
 *  The search is exhaustive.  If this function is called multiple times with the same name, the
 *  peformance impact will be minimal.  If the module is not found by name, an attempt will be made
 *  to demand load the kernel module "streams-%s".
 */
STATIC struct fmodsw *
fmod_search(const char *name, int load)
{
	struct fmodsw *fmod = NULL;

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	int reload;
#endif
	read_lock(&fmodsw_lock);
#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char devname[64];

			if ((fmod = __fmod_search(name)))
				break;
			if (!load)
				break;
			read_unlock(&fmodsw_lock);
			snprintf(devname, sizeof(devname), "streams-%s", name);
			request_module(devname);
			read_lock(&fmodsw_lock);
		} while (0);
		/* try to acquire the module */
		if (fmod && fmod->f_str)
			if (try_module_get(fmod->f_kmod)) {
				_ptrace(("%s: %s: incremented mod count\n", __FUNCTION__,
					 fmod->f_name));
				break;
			}
		fmod = NULL;
	}
#else				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	fmod = __fmod_search(name);
#endif				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	read_unlock(&fmodsw_lock);
	return (fmod);
}

/*
 *  cmin_search: - looke up a minor device node by name
 *  @cdev: character device major structure
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 *
 *  The search is exhaustive.  If this function is called multiple times with the same name, the
 *  peformance impact will be minimal.  If the device is not found by name, an attempt will be made
 *  to demand load the kernel module "streams-%s-%s" and then "/dev/streams/%s/%s".
 *
 *  Demand loading modules by minor device names is primarily for minor device names under the clone
 *  device.  There was a problem that this is the first check for unloaded clone minors and the
 *  apporpriate module was not being demand loaded, therefore, the additional check was made that if
 *  the major device is named 'clone' then streams-%s and /dev/streams/%s is also attempted.
 */
STATIC struct devnode *
cmin_search(struct cdevsw *cdev, const char *name, int load)
{
	struct devnode *cmin = NULL;

#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	int reload;
#endif				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	read_lock(&cdevsw_lock);
#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	for (reload = load ? 0 : 1; reload < 2; reload++) {
		do {
			char devname[96];

			if (strcmp(cdev->d_name, "clone") == 0) {
				if ((cmin = __cmin_search(cdev, name)))
					break;
				if (!load)
					break;
				read_unlock(&cdevsw_lock);
				snprintf(devname, sizeof(devname), "streams-%s", name);
				request_module(devname);
				read_lock(&cdevsw_lock);
#if defined CONFIG_DEVFS || 1
				if ((cmin = __cmin_search(cdev, name)))
					break;
				read_unlock(&cdevsw_lock);
				snprintf(devname, sizeof(devname), "/dev/streams/%s", name);
				request_module(devname);
				read_lock(&cdevsw_lock);
#endif				/* CONFIG_DEVFS */
			}
			if ((cmin = __cmin_search(cdev, name)))
				break;
			if (!load)
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "streams-%s-%s", cdev->d_name, name);
			request_module(devname);
			read_lock(&cdevsw_lock);
#if defined CONFIG_DEVFS || 1
			if ((cmin = __cmin_search(cdev, name)))
				break;
			read_unlock(&cdevsw_lock);
			snprintf(devname, sizeof(devname), "/dev/streams/%s/%s", cdev->d_name,
				 name);
			request_module(devname);
			read_lock(&cdevsw_lock);
#endif				/* CONFIG_DEVFS */
		} while (0);
	}
#else				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	cmin = __cmin_search(cdev, name);
#endif				/* defined(CONFIG_KMOD) || defined(CONFIG_MODULES) */
	read_unlock(&cdevsw_lock);
	return (cmin);
}

/**
 *  fmod_str:	- look up a fmod by streamtab
 *  @str:	streamtab to look up
 */
streams_fastcall struct fmodsw *
fmod_str(const struct streamtab *str)
{
	struct fmodsw *fmod = NULL;
	struct list_head *pos;

	read_lock(&fmodsw_lock);
	list_for_each(pos, &fmodsw_list) {
		fmod = list_entry(pos, struct fmodsw, f_list);

		if (fmod->f_str == str)
			break;
		fmod = NULL;
	}
	read_unlock(&fmodsw_lock);
	return (fmod);
}

EXPORT_SYMBOL_GPL(fmod_str);

/**
 *  cdev_str:	- look up a cdev by streamtab
 *  @str:	streamtab to look up
 */
streams_fastcall struct cdevsw *
cdev_str(const struct streamtab *str)
{
	struct cdevsw *cdev = NULL;
	struct list_head *pos;

	read_lock(&cdevsw_lock);
	list_for_each(pos, &cdevsw_list) {
		cdev = list_entry(pos, struct cdevsw, d_list);

		if (cdev->d_str == str)
			break;
		cdev = NULL;
	}
	read_unlock(&cdevsw_lock);
	return (cdev);
}

EXPORT_SYMBOL_GPL(cdev_str);

/* 
 *  -------------------------------------------------------------------------
 *
 *  Get and put devices and modules.
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  sdev_get: - get a reference to a STREAMS device
 *  @major: (internal) major device number of the STREAMS device
 *
 *  Context: When the calling context can block, an attempt will be made to load the driver by major
 *  device number.
 *
 *  Notices: sdev_get() and sdev_put() had to be renamed from cdev_get() and cdev_put() because
 *  later 2.6 kernels use those names for character devices.
 */
streams_fastcall struct cdevsw *
sdev_get(major_t major)
{
	return cdev_lookup(major, can_sleep());
}

EXPORT_SYMBOL_GPL(sdev_get);

/**
 *  sdev_put:	- put a reference to a STREAMS device
 *  @cdev:	STREAMS device structure pointer to put
 *
 *  Notices: sdev_get() and sdev_put() had to be renamed from cdev_get() and cdev_put() because
 *  later 2.6 kernels use those names for character devices.
 */
streams_fastcall void
sdev_put(struct cdevsw *cdev)
{
#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	if (cdev && cdev->d_kmod) {
		_printd(("%s: %s: decrementing use count\n", __FUNCTION__, cdev->d_name));
		__assert(module_refcount(cdev->d_kmod) > 0);
		module_put(cdev->d_kmod);
		_printd(("%s: %s: [%s] count is now %d\n", __FUNCTION__, cdev->d_name,
			 cdev->d_kmod->name, module_refcount(cdev->d_kmod)));
	}
#endif
}

EXPORT_SYMBOL_GPL(sdev_put);

/**
 *  cdrv_get:	- get a reference to a STREAMS driver
 *  @modid:	module id number of the STREAMS driver
 */
streams_fastcall struct cdevsw *
cdrv_get(modID_t modid)
{
	return cdrv_lookup(modid, can_sleep());
}

EXPORT_SYMBOL_GPL(cdrv_get);

/**
 *  cdrv_put:	- put a reference to a STREAMS driver
 *  @cdev:	STREAMS driver structure pointer to put
 */
streams_fastcall void
cdrv_put(struct cdevsw *cdev)
{
	sdev_put(cdev);
}

EXPORT_SYMBOL_GPL(cdrv_put);

/**
 *  fmod_get: - get a reference to a STREAMS module
 *  @modid: module id number of the STREAMS module
 *
 *  Context: When the calling context can block, an attempt will be made to load the module by
 *  module identifier.
 */
streams_fastcall struct fmodsw *
fmod_get(modID_t modid)
{
	return fmod_lookup(modid, can_sleep());
}

EXPORT_SYMBOL_GPL(fmod_get);

/**
 *  fmod_put: - put a reference to a STREAMS module
 *  @fmod: STREAMS module structure pointer to put
 *
 */
streams_fastcall void
fmod_put(struct fmodsw *fmod)
{
#if defined(CONFIG_KMOD) || defined(CONFIG_MODULES)
	if (fmod && fmod->f_kmod) {
		_ptrace(("%s: %s: decrementing use count\n", __FUNCTION__, fmod->f_name));
		__assert(module_refcount(fmod->f_kmod) > 0);
		module_put(fmod->f_kmod);
		_printd(("%s: %s: [%s] count is now %d\n", __FUNCTION__, fmod->f_name,
			 fmod->f_kmod->name, module_refcount(fmod->f_kmod)));
	}
#endif
}

EXPORT_SYMBOL_GPL(fmod_put);

/**
 *  cmaj_get: - get a reference to a major device node (devnode)
 *  @cdev:	cdevsw structure for device
 *  @major:	major device number
 */
streams_fastcall struct devnode *
cmaj_get(const struct cdevsw *cdev, major_t major)
{
	return cmaj_lookup(cdev, major);
}

EXPORT_SYMBOL_GPL(cmaj_get);

/**
 *  cmin_get: - get a reference to a minor device node (devnode)
 *  @cdev:	cdevsw structure for device
 *  @minor:	minor device number
 */
streams_fastcall struct devnode *
cmin_get(const struct cdevsw *cdev, minor_t minor)
{
	return cmin_lookup(cdev, minor);
}

EXPORT_SYMBOL_GPL(cmin_get);

/**
 *  cdev_find: - find a STREAMS device by its name
 *  @name:	the name to find
 *
 *  Attempt to find a STREAMS device by name.  If the device cannot be found by name, attempt to
 *  load the kernel module streams-%s where %s is the name requested and check again.
 *
 *  Return Value: A pointer to the STREAMS device, with use count incremented, or %NULL if not found
 *  and could not be loaded.
 *
 *  Context: When the calling context can block, an attempt will be made to load the driver by name.
 */
streams_fastcall struct cdevsw *
cdev_find(const char *name)
{
	return cdev_search(name, can_sleep());
}

EXPORT_SYMBOL_GPL(cdev_find);

/**
 *  cdev_match: - find a STREAMS device by extended name
 *  @name:	the name to find
 *
 *  Attempt to find a STREAMS device by extended name.  If the device cannot be found by extended
 *  name, attempt to load the kernel module streams-%s where %s is the name requested and check
 *  again.
 *
 *  Return Value: A pointer to the STREAMS device, with use count incremented, or %NULL if not found
 *  and could not be loaded.
 *
 *  Context: When the calling context can block, an attempt will be made to load the driver by name.
 */
streams_fastcall struct cdevsw *
cdev_match(const char *name)
{
	int i;
	char root[FMNAMESZ + 1];

	snprintf(root, sizeof(root), "%s", name);
	for (i = 0; i < FMNAMESZ && root[i] != '.'; i++) ;
	root[i] = '\0';
	return cdev_find(root);
}

EXPORT_SYMBOL_GPL(cdev_match);

/**
 *  fmod_find: - get a reference to a STREAMS module
 *  @name: name of the module
 *
 *  Attempt to find a STREAMS module by name.  If the module cannot be found by name, attempt to
 *  load the kernel module streams-%s where %s is the name requested and check again.
 *
 *  Return Value: A pointer to the STREAMS module, with use count incremented, or %NULL if not found
 *  and could not be loaded.
 *
 *  Context: When the calling context can block, an attempt will be made to load the module by name.
 */
streams_fastcall struct fmodsw *
fmod_find(const char *name)
{
	return fmod_search(name, can_sleep());
}

EXPORT_SYMBOL_GPL(fmod_find);

streams_fastcall struct devnode *
cmin_find(struct cdevsw *cdev, const char *name)
{
	return cmin_search(cdev, name, can_sleep());
}

EXPORT_SYMBOL_GPL(cmin_find);

/**
 *  cdev_minor: - get extended minor number from kernel device type
 *  @cdev: the cdevsw table entry for the device
 *  @major: the external major device number
 *  @minor: the external minor device number
 */
streams_fastcall minor_t
cdev_minor(struct cdevsw *cdev, major_t major, minor_t minor)
{
	struct list_head *pos;

	ensure(cdev->d_majors.next, INIT_LIST_HEAD(&cdev->d_majors));

	list_for_each(pos, &cdev->d_majors) {
		struct devnode *cmaj = list_entry(pos, struct devnode, n_list);

		if (major == cmaj->n_major)
			break;
		minor += (1U << MINORBITS);
	}
	return (minor);
}

EXPORT_SYMBOL_GPL(cdev_minor);

streams_fastcall void
fmod_add(struct fmodsw *fmod, modID_t modid)
{
	/* These are statically allocated and therefore need to be initialized, however,
	   the module must not already be on a list. */

	assert(!fmod->f_list.next || list_empty(&fmod->f_list));
	INIT_LIST_HEAD(&fmod->f_list);
	assert(!fmod->f_hash.next || list_empty(&fmod->f_hash));
	INIT_LIST_HEAD(&fmod->f_hash);

	fmod->f_modid = modid;
	list_add(&fmod->f_list, &fmodsw_list);
	list_add(&fmod->f_hash, strmod_hash_slot(modid));
	fmod_count++;
}

EXPORT_SYMBOL_GPL(fmod_add);

streams_fastcall void
fmod_del(struct fmodsw *fmod)
{
	/* remove from list and hash */
	list_del_init(&fmod->f_list);
	list_del_init(&fmod->f_hash);
}

EXPORT_SYMBOL_GPL(fmod_del);

/**
 *  sdev_ini:	- initialize a STREAMS device for the registration list
 *  @cdev:	STREAMS device structure to initialize
 *  @modid:	assigned module identifier
 *
 *  Locking: spec_snode might sleep, so this needs to be called without any locks held.
 */
streams_fastcall int
sdev_ini(struct cdevsw *cdev, modID_t modid)
{
	struct inode *inode;
	dev_t dev;

	/* These are statically allocated and therefore need to be initialized, however,
	   the module must not already be on a list. */

	assert(!cdev->d_list.next || list_empty(&cdev->d_list));
	INIT_LIST_HEAD(&cdev->d_list);
	assert(!cdev->d_hash.next || list_empty(&cdev->d_hash));
	INIT_LIST_HEAD(&cdev->d_hash);
	assert(!cdev->d_majors.next || list_empty(&cdev->d_majors));
	INIT_LIST_HEAD(&cdev->d_majors);
	assert(!cdev->d_minors.next || list_empty(&cdev->d_minors));
	INIT_LIST_HEAD(&cdev->d_minors);
	assert(!cdev->d_apush.next || list_empty(&cdev->d_apush));
	INIT_LIST_HEAD(&cdev->d_apush);
	assert(!cdev->d_stlist.next || list_empty(&cdev->d_stlist));
	INIT_LIST_HEAD(&cdev->d_stlist);

	dev = makedevice(0, modid);

	if (IS_ERR((inode = spec_snode(dev, cdev)))) {
		_ptrace(("couldn't get specfs inode\n"));
		return PTR_ERR(inode);
	}
	cdev->d_inode = inode;
	cdev->d_modid = modid;
	return (0);
}

EXPORT_SYMBOL_GPL(sdev_ini);

/**
 *  sdev_add:	- add a STREAMS device to the registration list
 *  @cdev:	STREAMS device structure to add
 *
 *  Notices: sdev_add() and sdev_del() had to be renamed from cdev_add() and cdev_put() because
 *  later 2.6 kernels use those names for character devices.
 *
 *  Locking: this function needs to be called with the cdevsw_lock held.
 */
streams_fastcall int
sdev_add(struct cdevsw *cdev)
{
	cdev->d_inode->i_sb->s_root->d_inode->i_nlink++;
	/* add to list and hash */
	list_add(&cdev->d_list, &cdevsw_list);
	list_add(&cdev->d_hash, strmod_hash_slot(cdev->d_modid));
	cdev_count++;
	return (0);
}

EXPORT_SYMBOL_GPL(sdev_add);

/**
 *  sdev_del:	- delete a streams device from the registration list
 *  @cdev:	STREAMS device structure to delete
 *
 *  Notices: sdev_add() and sdev_del() had to be renamed from cdev_add() and cdev_put() because
 *  later 2.6 kernels use those names for character devices.
 */
streams_fastcall void
sdev_del(struct cdevsw *cdev)
{
	cdev->d_inode->i_sb->s_root->d_inode->i_nlink--;
	/* remove from list and hash */
	list_del_init(&cdev->d_list);
	list_del_init(&cdev->d_hash);
	cdev_count--;
}

EXPORT_SYMBOL_GPL(sdev_del);

streams_fastcall void
sdev_rel(struct cdevsw *cdev)
{
	struct inode *inode;

	inode = cdev->d_inode;
	/* put away dentry if necessary */
	_ptrace(("inode %p no %lu refcount now %d\n", inode, inode->i_ino,
		 atomic_read(&inode->i_count) - 1));
	iput(inode);
	cdev->d_inode = NULL;
}

EXPORT_SYMBOL_GPL(sdev_rel);

streams_fastcall void
cmaj_add(struct devnode *cmaj, struct cdevsw *cdev, major_t major)
{
	cmaj->n_major = major;
	cmaj->n_minor = 0;	/* FIXME */

	ensure(cdev->d_majors.next, INIT_LIST_HEAD(&cdev->d_majors));

	/* add to list and hash */
	if (list_empty(&cdev->d_majors))
		cdev->d_major = major;
	list_add_tail(&cmaj->n_list, &cdev->d_majors);
	list_add_tail(&cmaj->n_hash, strdev_hash_slot(major));
}

EXPORT_SYMBOL_GPL(cmaj_add);

streams_fastcall void
cmaj_del(struct devnode *cmaj, struct cdevsw *cdev)
{
	ensure(cdev->d_majors.next, INIT_LIST_HEAD(&cdev->d_majors));

	list_del_init(&cmaj->n_list);
	list_del_init(&cmaj->n_hash);
	if (list_empty(&cdev->d_majors))
		cdev->d_major = 0;
}

EXPORT_SYMBOL_GPL(cmaj_del);

streams_fastcall int
cmin_ini(struct devnode *cmin, struct cdevsw *cdev, minor_t minor)
{
	struct inode *inode;
	dev_t dev;

	dev = makedevice(cdev->d_modid, minor);

	if (IS_ERR(inode = spec_snode(dev, cdev))) {
		_ptrace(("couldn't get specfs inode\n"));
		return PTR_ERR(inode);
	}
	cmin->n_inode = inode;
	cmin->n_dev = cdev;
	cmin->n_modid = cdev->d_modid;
	cmin->n_minor = minor;
	if (!cmin->n_str)
		cmin->n_str = cdev->d_str;
	if (!(cmin->n_mode & S_IFMT))
		cmin->n_mode = cdev->d_mode;
	if (!(cmin->n_mode & S_IFMT))
		cmin->n_mode = S_IFCHR | S_IRUGO | S_IWUGO;
	return (0);
}

EXPORT_SYMBOL_GPL(cmin_ini);

#ifdef HAVE_KFUNC_DEVICE_CREATE
struct class *streams_class;
#endif

streams_fastcall int
cmin_add(struct devnode *cmin, struct cdevsw *cdev)
{
	ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));

	cdev->d_inode->i_nlink++;
	/* add to list and hash */
	list_add(&cmin->n_list, &cdev->d_minors);
	list_add(&cmin->n_hash, strnod_hash_slot(cmin->n_minor));
	cmin_count++;
#if 0
#ifdef HAVE_KFUNC_DEVICE_CREATE
	device_create(streams_class,NULL,MKDEV(cdev->d_major,cmin->n_minor),NULL,"streams/%s/%s",cdev->d_name,cmin->n_name);
#endif
#endif
	return (0);
}

EXPORT_SYMBOL_GPL(cmin_add);

streams_fastcall void
cmin_del(struct devnode *cmin, struct cdevsw *cdev)
{
	ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));

	cdev->d_inode->i_nlink--;
	/* remove from list and hash */
	list_del_init(&cmin->n_list);
	list_del_init(&cmin->n_hash);
	cmin_count--;
#if 0
#ifdef HAVE_KFUNC_DEVICE_DESTROY
	//device_destroy(streams_class,MKDEV(cdev->d_major,cmin->n_minor));
#endif
#endif
}

EXPORT_SYMBOL_GPL(cmin_del);

streams_fastcall void
cmin_rel(struct devnode *cmin)
{
	struct inode *inode;

	inode = cmin->n_inode;
	/* put away inode if required */
	_ptrace(("inode %p no %lu refcount now %d\n", inode, inode->i_ino,
		 atomic_read(&inode->i_count) - 1));
	iput(inode);
	cmin->n_inode = NULL;
	cmin->n_dev = NULL;
	cmin->n_modid = -1;
	cmin->n_minor = -1;
}

EXPORT_SYMBOL_GPL(cmin_rel);

static inline int
is_module_name(const char *name)
{
	int i;
	unsigned char c;

	for (i = 0, c = (unsigned char) name[i];
	     i < MODULE_NAME_LEN && c != '\0';
	     i++, c = (unsigned char) name[i])
	{
		/* valid characters are [a-zA-Z0-9_] */
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')
		    || ('_' == c))
			continue;
		return 0;
	}
	if ((c != '\0') || (i >= MODULE_NAME_LEN) || (i < 2))
		return 0;
	return 1;
}

/* The next 500-odd lines are just for figuring out a usable way of determining which module owns an
 * address (or text address).  Note that streams_module_address is only used when installing IP
 * hooks, which is normally during module initialization time. */

#ifdef HAVE_KMEMB_STRUCT_MODULE_NEXT
/* 2.4.33 kernel approach */
/* The issues here are that module->next is used to link modules together from module_list; however,
 * modules are pushed onto the list, so modules loaded after specfs are "earlier" on the list and we
 * will not be able to find them.  Guess what though, module kernel_module is first to be pushed on
 * the list, which means that it is last on the list from us.  Also, 'module_list' happens to be
 * defined immediately after the kernel module structure, so we can find the head of the list
 * afterall.  Also, we can verify when our module is loaded that this is true. */
static struct module **module_list_pointer = NULL;
static void
find_module_list(void)
{
	struct module *mod = THIS_MODULE, **modp;

	for (; mod->next != NULL; mod = mod->next) ;
	/* mod now points to kernel_module */
	struct {
		struct module;
		struct module *module_list_p;
	} *kmod = (typeof(kmod)) mod;

	modp = &kmod->module_list_p;
	/* we should be first on the list when our init is run */
	if (*modp == THIS_MODULE)
		module_list_pointer = modp;
}

struct module *
__streams_module_address(unsigned long addr)
{
	struct module *mod;

	if (module_list_pointer != NULL) {
		for (mod = *module_list_pointer; mod != NULL; mod = mod->next) {
			if (mod->next == NULL)
				continue;
			if ((unsigned long) mod <= addr && addr <= (unsigned long) mod + mod->size)
				return mod;
		}
	}
	return NULL;
}

#if   defined HAVE_MODLIST_LOCK_SUPPORT
extern spinlock_t modlist_lock;
struct module *
streams_module_address(unsigned long addr)
{
	struct module *mod;
	unsigned long flags;

	spin_lock_irqsave(&modlist_lock, flags);
	mod = __streams_module_address(addr);
	spin_unlock_irqrestore(&modlist_lock, flags);
	return mod;
}

EXPORT_SYMBOL(streams_module_address);
#elif defined HAVE_MODLIST_LOCK_USABLE && CONFIG_KERNEL_WEAK_MODULES
extern spinlock_t modlist_lock __attribute__ ((__weak__));
struct module *
streams_module_address(unsigned long addr)
{
	struct module *mod;

	if (&modlist_lock != 0) {
		unsigned long flags;

		spin_lock_irqsave(&modlist_lock, flags);
		mod = __streams_module_address(addr);
		spin_unlock_irqrestore(&modlist_lock, flags);
	} else {
		/* probably safe without locks because only called during module
		 * init procedures */
		mod = __streams_module_address(addr);
	}
	return mod;
}

EXPORT_SYMBOL(streams_module_address);
struct module *
#else
struct module *
streams_module_address(unsigned long addr)
{
	/* probably safe without locks because only called during module init
	 * procedures */
	return __streams_module_address(addr);
}

EXPORT_SYMBOL(streams_module_address);
struct module *
#endif
#else				/* HAVE_KMEMB_STRUCT_MODULE_NEXT */
/* 2.6.18+ kernel approach */
#ifdef HAVE_MODLIST_LOCK_SYMBOL
/* ------------------------------------------------------------------------- */
/* 2.6.18+ approach -------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* The unforunate problem here is that we cannot get access to the module list
 * lock without ripping kernel symbols.  Therefore, we have to be a little more
 * creative.  Also, when a module list lock was used, none of the search symbols
 * were exported, so its brute force. */
struct module *
__streams_module_address(unsigned long addr)
{
	struct module *mod, *mod_prev = NULL;
	struct list_head *head = &__this_module.list;

	list_for_each_entry(mod, head, list) {
		/* extra list sanity check */
		if (mod->list.prev != head && mod->list.prev != &mod_prev->list)
			return NULL;
		/* extra list sanity check */
		if (mod->list.next == NULL || mod->list.next == &mod->list)
			return NULL;
		mod_prev = mod;
		switch (mod->state) {
		case MODULE_STATE_LIVE:
		case MODULE_STATE_COMING:
		case MODULE_STATE_GOING:
			break;
		default:
			return NULL;
		}
		if (!is_module_name(mod->name))
			return NULL;
		if ((unsigned long) mod->module_init <= addr && addr < (unsigned long) mod->module_init + mod->init_size)
			return mod;
		if ((unsigned long) mod->module_core <= addr && addr < (unsigned long) mod->module_core + mod->core_size)
			return mod;
	}
	return NULL;
}

#if   defined HAVE_MODLIST_LOCK_SUPPORT
extern spinlock_t modlist_lock;
struct module *
streams_module_address(unsigned long addr)
{
	struct module *mod;
	unsigned long flags;

	spin_lock_irqsave(&modlist_lock, flags);
	mod = __streams_module_address(addr);
	spin_unlock_irqrestore(&modlist_lock, flags);
	return mod;
}

EXPORT_SYMBOL(streams_module_address);

#elif defined HAVE_MODLIST_LOCK_SYMBOL && defined CONFIG_KERNEL_WEAK_SYMBOLS
extern spinlock_t modlist_lock __attribute__ ((__weak__));
struct module *
streams_module_address(unsigned long addr)
{
	struct module *mod = NULL;

	if (&modlist_lock != 0) {
		unsigned long flags;

		spin_lock_irqsave(&modlist_lock, flags);
		mod = __streams_module_address(addr);
		spin_unlock_irqrestore(&modlist_lock, flags);
	}
	return mod;
}

EXPORT_SYMBOL(streams_module_address);

#else
struct module *
streams_module_address(unsigned long addr)
{
#if 1
	struct module *mod;

	/* is it too dangerous to walk the list without locks? */
	/* Probably not for this version, because it is only to be called from module init
	 * procedures. */
	preempt_disable();
	mod = __streams_module_address(addr);
	preempt_enable();
	return mod;
#else
	return NULL;
#endif
}

EXPORT_SYMBOL(streams_module_address);

#endif

#else				/* HAVE_MODLIST_LOCK_SYMBOL */
/* ------------------------------------------------------------------------- */
/* 2.6.21+ approach -------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
struct module *
__streams_module_address(unsigned long addr)
{
	struct module *mod;
	struct list_head *head = &__this_module.list;

	list_for_each_entry(mod, head, list) {
		switch (mod->state) {
		case MODULE_STATE_LIVE:
		case MODULE_STATE_COMING:
		case MODULE_STATE_GOING:
			break;
		default:
			return NULL;
		}
		if (!is_module_name(mod->name))
			return NULL;
		if ((unsigned long) mod->module_init <= addr && addr < (unsigned long) mod->module_init + mod->init_size)
			return mod;
		if ((unsigned long) mod->module_core <= addr && addr < (unsigned long) mod->module_core + mod->core_size)
			return mod;
	}
	return NULL;
}

#if   defined HAVE_MODULE_ADDRESS_SUPPORT
extern struct module *module_address(unsigned long addr);
struct module *
streams_module_address(unsigned long addr)
{
	return module_address(addr);
}

EXPORT_SYMBOL(streams_module_address);

#elif defined HAVE___MODULE_ADDRESS_SUPPORT
extern struct module *__module_address(unsigned long addr);
streams_fastcall struct module *
streams_module_address(unsigned long addr)
{
	struct module *mod;

	preempt_disable();
	mod = __module_address(addr);
	preempt_enable();
	return mod;
}

EXPORT_SYMBOL(streams_module_address);

#elif defined HAVE_MODULE_ADDRESS_SYMBOL && defined CONFIG_KERNEL_WEAK_SYMBOLS
extern struct module *module_address(unsigned long addr) __attribute__ ((__weak__));
struct module *
streams_module_address(unsigned long addr)
{
	if (module_address != 0) {
		return module_address(addr);
	} else {
		struct module *mod;

		preempt_disable();
		mod = __streams_module_address(addr);
		preempt_enable();
		return mod;
	}
}

EXPORT_SYMBOL(streams_module_address);

#elif defined HAVE___MODULE_ADDRESS_SYMBOL && defined CONFIG_KERNEL_WEAK_SYMBOLS
extern struct module *__module_address(unsigned long addr) __attribute__ ((__weak__));
struct module *
streams_module_address(unsigned long addr)
{
	if (__module_address != 0) {
		struct module *mod;

		preempt_disable();
		mod = __module_address(addr);
		preempt_enable();
		return mod;
	} else {
		struct module *mod;

		preempt_disable();
		mod = __streams_module_address(addr);
		preempt_enable();
		return mod;
	}
}

EXPORT_SYMBOL(streams_module_address);

#else
struct module *
streams_module_address(unsigned long addr)
{
	struct module *mod;

	preempt_disable();
	mod = __streams_module_address(addr);
	preempt_enable();
	return mod;
}

EXPORT_SYMBOL(streams_module_address);
#endif

#endif				/* HAVE_MODLIST_LOCK_SYMBOL */
#endif				/* HAVE_KMEMB_STRUCT_MODULE_NEXT */

BIG_STATIC int
strlookup_init(void)
{
	init_fmod_hash();
	init_cdev_hash();
	init_cmin_hash();
#if 0
#if defined(HAVE_KFUNC_CLASS_CREATE) || defined(HAVE_KMACRO_CLASS_CREATE)
	streams_class = class_create(THIS_MODULE, "streams");
#endif
#endif
	return (0);
}

BIG_STATIC void
strlookup_exit(void)
{
#if 0
#ifdef HAVE_KFUNC_CLASS_DESTROY
	class_destroy(streams_class);
#endif
#endif
	return;
}
