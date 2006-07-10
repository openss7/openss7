/*****************************************************************************

 @(#) $RCSfile: strlookup.c,v $ $Name:  $($Revision: 0.9.2.44 $) $Date: 2006/07/10 08:51:05 $

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

 Last Modified $Date: 2006/07/10 08:51:05 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strlookup.c,v $ $Name:  $($Revision: 0.9.2.44 $) $Date: 2006/07/10 08:51:05 $"

static char const ident[] =
    "$RCSfile: strlookup.c,v $ $Name:  $($Revision: 0.9.2.44 $) $Date: 2006/07/10 08:51:05 $";

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
#include <linux/mount.h>	/* for vfsmount stuff */
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_irq() and friends */
#endif
#include <asm/hardirq.h>

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

rwlock_t cdevsw_lock = RW_LOCK_UNLOCKED;
rwlock_t fmodsw_lock = RW_LOCK_UNLOCKED;

#if 0
rwlock_t cminsw_lock = RW_LOCK_UNLOCKED;
#endif

struct list_head cdevsw_list = LIST_HEAD_INIT(cdevsw_list);	/* Devices go here */
struct list_head fmodsw_list = LIST_HEAD_INIT(fmodsw_list);	/* Modules go here */

#if 0
struct list_head cminsw_list = LIST_HEAD_INIT(cminsw_list);	/* Minors go here */
#endif

#if	defined CONFIG_STREAMS_SC_MODULE || !defined CONFIG_STREAMS_SC
EXPORT_SYMBOL_NOVERS(cdevsw_lock);
EXPORT_SYMBOL_NOVERS(cdevsw_list);
#endif
#if	defined CONFIG_STREAMS_SC_MODULE || !defined CONFIG_STREAMS_SC
EXPORT_SYMBOL_NOVERS(fmodsw_lock);
EXPORT_SYMBOL_NOVERS(fmodsw_list);
#endif
#if	defined CONFIG_STREAMS_SC_MODULE || !defined CONFIG_STREAMS_SC
#if 0
EXPORT_SYMBOL_NOVERS(cminsw_lock);
EXPORT_SYMBOL_NOVERS(cminsw_list);
#endif
#endif

struct list_head fmodsw_hash[STRMOD_HASH_SIZE] __cacheline_aligned = { {NULL,}, };
struct list_head cdevsw_hash[STRDEV_HASH_SIZE] __cacheline_aligned = { {NULL,}, };
struct list_head cminsw_hash[STRNOD_HASH_SIZE] __cacheline_aligned = { {NULL,}, };

int cdev_count = 0;
int fmod_count = 0;
int cmin_count = 0;

#if defined CONFIG_STREAMS_SC_MODULE || !defined CONFIG_STREAMS_SC
EXPORT_SYMBOL_NOVERS(cdev_count);
EXPORT_SYMBOL_NOVERS(fmod_count);
EXPORT_SYMBOL_NOVERS(cmin_count);
#endif

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
			/* cache to front */
			list_del(&cmaj->n_hash);
			list_add(&cmaj->n_hash, slot);
			return (cmaj);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(__cmaj_lookup);

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

EXPORT_SYMBOL_NOVERS(__cdev_lookup);

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
			/* cache to front */
			list_del(&cdev->d_hash);
			list_add(&cdev->d_hash, slot);
			return (cdev);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(__cdrv_lookup);

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
			/* pull to head of slot */
			list_del(&cmin->n_hash);
			list_add(&cmin->n_hash, slot);
			return (cmin);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(__cmin_lookup);

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
			/* pull to front of slot */
			list_del(&fmod->f_hash);
			list_add(&fmod->f_hash, slot);
			return (fmod);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(__fmod_lookup);

streams_fastcall struct cdevsw *
__cdev_search(const char *name)
{
	struct list_head *pos, *slot = &cdevsw_list;

	list_for_each(pos, slot) {
		struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_list);

		if (!strncmp(cdev->d_name, name, FMNAMESZ)) {
			/* pull to front of slot */
			list_del(&cdev->d_list);
			list_add(&cdev->d_list, slot);
			return (cdev);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(__cdev_search);

streams_fastcall struct fmodsw *
__fmod_search(const char *name)
{
	struct list_head *pos, *slot = &fmodsw_list;

	list_for_each(pos, slot) {
		struct fmodsw *fmod = list_entry(pos, struct fmodsw, f_list);

		if (!strncmp(fmod->f_name, name, FMNAMESZ)) {
			/* pull to front of slot */
			list_del(&fmod->f_list);
			list_add(&fmod->f_list, slot);
			return (fmod);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(__fmod_search);

streams_fastcall struct devnode *
__cmin_search(struct cdevsw *cdev, const char *name)
{
	struct list_head *pos, *slot = &cdev->d_minors;

	ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));

	list_for_each(pos, slot) {
		struct devnode *cmin = list_entry(pos, struct devnode, n_list);

		if (!strncmp(cmin->n_name, name, FMNAMESZ)) {
			/* pull to head of slot */
			list_del(&cmin->n_list);
			list_add(&cmin->n_list, slot);
			return (cmin);
		}
	}
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(__cmin_search);

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

EXPORT_SYMBOL_NOVERS(__smod_search);

#ifdef CONFIG_KMOD
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

#ifdef CONFIG_KMOD
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

#ifdef CONFIG_KMOD
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
#ifdef CONFIG_DEVFS
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
#endif				/* CONFIG_KMOD */
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

#ifdef CONFIG_KMOD
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
#endif				/* CONFIG_KMOD */
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

#ifdef CONFIG_KMOD
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
#endif				/* CONFIG_KMOD */
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

#ifdef CONFIG_KMOD
	int reload;
#endif
	read_lock(&cdevsw_lock);
#ifdef CONFIG_KMOD
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
#ifdef CONFIG_DEVFS
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
#else				/* CONFIG_KMOD */
	cdev = __cdev_search(name);
#endif				/* CONFIG_KMOD */
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

#ifdef CONFIG_KMOD
	int reload;
#endif
	read_lock(&fmodsw_lock);
#ifdef CONFIG_KMOD
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
#else				/* CONFIG_KMOD */
	fmod = __fmod_search(name);
#endif				/* CONFIG_KMOD */
	read_unlock(&fmodsw_lock);
	return (fmod);
}

/*
 *  cmin_search: - looke up a minor device node by name
 *  @cdev: character device major structure
 *  @name: name to look up
 */
STATIC struct devnode *
cmin_search(const struct cdevsw *cdev, const char *name)
{
	struct devnode *cmin = NULL;

	ensure(cdev->d_minors.next, return (cmin));

	read_lock(&cdevsw_lock);
	if (cdev) {
		register struct list_head *pos;

		list_for_each(pos, &cdev->d_minors) {
			struct devnode *n = list_entry(pos, struct devnode, n_list);

			if (!strncmp(n->n_name, name, FMNAMESZ)) {
				cmin = n;
				break;
			}
		}
	}
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

EXPORT_SYMBOL_NOVERS(fmod_str);

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

EXPORT_SYMBOL_NOVERS(cdev_str);

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
	return cdev_lookup(major, !in_interrupt());
}

EXPORT_SYMBOL_NOVERS(sdev_get);

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
	if (cdev && cdev->d_kmod) {
		_printd(("%s: %s: decrementing use count\n", __FUNCTION__, cdev->d_name));
		__assert(module_refcount(cdev->d_kmod) > 0);
		module_put(cdev->d_kmod);
		_printd(("%s: %s: [%s] count is now %d\n", __FUNCTION__, cdev->d_name,
			 cdev->d_kmod->name, module_refcount(cdev->d_kmod)));
	}
}

EXPORT_SYMBOL_NOVERS(sdev_put);

/**
 *  cdrv_get:	- get a reference to a STREAMS driver
 *  @modid:	module id number of the STREAMS driver
 */
streams_fastcall struct cdevsw *
cdrv_get(modID_t modid)
{
	return cdrv_lookup(modid, !in_interrupt());
}

EXPORT_SYMBOL_NOVERS(cdrv_get);

/**
 *  cdrv_put:	- put a reference to a STREAMS driver
 *  @cdev:	STREAMS driver structure pointer to put
 */
streams_fastcall void
cdrv_put(struct cdevsw *cdev)
{
	sdev_put(cdev);
}

EXPORT_SYMBOL_NOVERS(cdrv_put);

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
	return fmod_lookup(modid, !in_interrupt());
}

EXPORT_SYMBOL_NOVERS(fmod_get);

/**
 *  fmod_put: - put a reference to a STREAMS module
 *  @fmod: STREAMS module structure pointer to put
 *
 */
streams_fastcall void
fmod_put(struct fmodsw *fmod)
{
	if (fmod && fmod->f_kmod) {
		_ptrace(("%s: %s: decrementing use count\n", __FUNCTION__, fmod->f_name));
		__assert(module_refcount(fmod->f_kmod) > 0);
		module_put(fmod->f_kmod);
		_printd(("%s: %s: [%s] count is now %d\n", __FUNCTION__, fmod->f_name,
			 fmod->f_kmod->name, module_refcount(fmod->f_kmod)));
	}
}

EXPORT_SYMBOL_NOVERS(fmod_put);

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

EXPORT_SYMBOL_NOVERS(cmaj_get);

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

EXPORT_SYMBOL_NOVERS(cmin_get);

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
	return cdev_search(name, !in_interrupt());
}

EXPORT_SYMBOL_NOVERS(cdev_find);

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

#if defined CONFIG_STREAMS_NSDEV_MODULE || !defined CONFIG_STREAMS_NSDEV
EXPORT_SYMBOL_NOVERS(cdev_match);
#endif

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
	return fmod_search(name, !in_interrupt());
}

EXPORT_SYMBOL_NOVERS(fmod_find);

streams_fastcall struct devnode *
cmin_find(const struct cdevsw *cdev, const char *name)
{
	return cmin_search(cdev, name);
}

EXPORT_SYMBOL_NOVERS(cmin_find);

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

EXPORT_SYMBOL_NOVERS(cdev_minor);

streams_fastcall void
fmod_add(struct fmodsw *fmod, modID_t modid)
{
	/* These are statically allocated and therefore need to be initialized, however, the module
	   must not already be on a list. */

	assert(!fmod->f_list.next || list_empty(&fmod->f_list));
	INIT_LIST_HEAD(&fmod->f_list);
	assert(!fmod->f_hash.next || list_empty(&fmod->f_hash));
	INIT_LIST_HEAD(&fmod->f_hash);

	fmod->f_modid = modid;
	list_add(&fmod->f_list, &fmodsw_list);
	list_add(&fmod->f_hash, strmod_hash_slot(modid));
	fmod_count++;
}

EXPORT_SYMBOL_NOVERS(fmod_add);

streams_fastcall void
fmod_del(struct fmodsw *fmod)
{
	/* remove from list and hash */
	list_del_init(&fmod->f_list);
	list_del_init(&fmod->f_hash);
}

EXPORT_SYMBOL_NOVERS(fmod_del);

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

	/* These are statically allocated and therefore need to be initialized, however, the module
	   must not already be on a list. */

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

EXPORT_SYMBOL_NOVERS(sdev_ini);

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

EXPORT_SYMBOL_NOVERS(sdev_add);

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

EXPORT_SYMBOL_NOVERS(sdev_del);

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

EXPORT_SYMBOL_NOVERS(sdev_rel);

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

EXPORT_SYMBOL_NOVERS(cmaj_add);

streams_fastcall void
cmaj_del(struct devnode *cmaj, struct cdevsw *cdev)
{
	ensure(cdev->d_majors.next, INIT_LIST_HEAD(&cdev->d_majors));

	list_del_init(&cmaj->n_list);
	list_del_init(&cmaj->n_hash);
	if (list_empty(&cdev->d_majors))
		cdev->d_major = 0;
}

EXPORT_SYMBOL_NOVERS(cmaj_del);

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

EXPORT_SYMBOL_NOVERS(cmin_ini);

streams_fastcall int
cmin_add(struct devnode *cmin, struct cdevsw *cdev)
{
	ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));

	cdev->d_inode->i_nlink++;
	/* add to list and hash */
	list_add(&cmin->n_list, &cdev->d_minors);
	list_add(&cmin->n_hash, strnod_hash_slot(cmin->n_minor));
	cmin_count++;
	return (0);
}

EXPORT_SYMBOL_NOVERS(cmin_add);

streams_fastcall void
cmin_del(struct devnode *cmin, struct cdevsw *cdev)
{
	ensure(cdev->d_minors.next, INIT_LIST_HEAD(&cdev->d_minors));

	cdev->d_inode->i_nlink--;
	/* remove from list and hash */
	list_del_init(&cmin->n_list);
	list_del_init(&cmin->n_hash);
	cmin_count--;
}

EXPORT_SYMBOL_NOVERS(cmin_del);

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

EXPORT_SYMBOL_NOVERS(cmin_rel);

BIG_STATIC int
strlookup_init(void)
{
	init_fmod_hash();
	init_cdev_hash();
	init_cmin_hash();
	return (0);
}

BIG_STATIC void
strlookup_exit(void)
{
	return;
}
