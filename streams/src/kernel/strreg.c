/*****************************************************************************

 @(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2004/05/05 23:10:10 $

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

 Last Modified $Date: 2004/05/05 23:10:10 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2004/05/05 23:10:10 $"

static char const ident[] =
    "$RCSfile: strreg.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2004/05/05 23:10:10 $";

#define __NO_VERSION__

#include <linux/compiler.h>
#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif
#include <linux/kernel.h>	/* for FASTCALL */
#include <linux/sched.h>	/* for current */
#include <linux/file.h>		/* for fput */
#include <linux/poll.h>
#include <linux/fs.h>
#include <asm/hardirq.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include <sys/kmem.h>		/* for kmem_ */
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "sth.h"		/* for stream operations */
#include "strspecfs.h"		/* for str_args */
#include "strsched.h"		/* for di_get and di_put */
#include "strreg.h"		/* extern verification */

#ifndef CONFIG_STREAMS_FIFO_MAJOR
#define CONFIG_STREAMS_FIFO_MAJOR 211
//#error "CONFIG_STREAMS_FIFO_MAJOR must be defined."
#endif
#ifndef CONFIG_STREAMS_SOCKSYS_MAJOR
#define CONFIG_STREAMS_SOCKSYS_MAJOR 40
//#error "CONFIG_STREAMS_SOCKSYS_MAJOR must be defined."
#endif

/* these don't need to be so big */

#define STRMOD_HASH_ORDER 5
#define STRDEV_HASH_ORDER 5

#define STRMOD_HASH_SIZE (1 << STRMOD_HASH_ORDER)
#define STRDEV_HASH_SIZE (1 << STRDEV_HASH_ORDER)

#define STRMOD_HASH_MASK ((unsigned long)(STRMOD_HASH_SIZE-1))
#define STRDEV_HASH_MASK ((unsigned long)(STRDEV_HASH_SIZE-1))

#define strmod_hash_slot(_modid) \
	(cdevsw_hash + (((_modid) + ((_modid) >> STRMOD_HASH_ORDER) + ((_modid) >> 2 * STRMOD_HASH_ORDER)) & STRMOD_HASH_MASK))

#define strdev_hash_slot(_major) \
	(fmodsw_hash + (((_major) + ((_major) >> STRDEV_HASH_ORDER) + ((_major) >> 2 * STRDEV_HASH_ORDER)) & STRDEV_HASH_MASK))

rwlock_t cdevsw_lock = RW_LOCK_UNLOCKED;
rwlock_t fmodsw_lock = RW_LOCK_UNLOCKED;
rwlock_t minors_lock = RW_LOCK_UNLOCKED;

struct list_head cdevsw_list = LIST_HEAD_INIT(cdevsw_list);	/* Devices go here */
struct list_head fmodsw_list = LIST_HEAD_INIT(fmodsw_list);	/* Modules go here */
struct list_head minors_list = LIST_HEAD_INIT(minors_list);	/* Minors go here */

#if	defined CONFIG_STREAMS_NSDEV_MODULE || \
	defined CONFIG_STREAMS_SC_MODULE
EXPORT_SYMBOL(cdevsw_list);
#endif
#if	defined CONFIG_STREAMS_SC_MODULE
EXPORT_SYMBOL(fmodsw_list);
#endif

struct list_head fmodsw_hash[STRMOD_HASH_SIZE] __cacheline_aligned = { {NULL,}, };
struct list_head cdevsw_hash[STRDEV_HASH_SIZE] __cacheline_aligned = { {NULL,}, };

STATIC struct cdevsw *cdevsw_major_cache = NULL;	/* L1 major cache */
STATIC struct cdevsw *cdevsw_modid_cache = NULL;	/* LI modid cache */
STATIC struct fmodsw *fmodsw_modid_cache = NULL;	/* L1 modid cache */
STATIC struct cdevsw *cdevsw_mname_cache = NULL;	/* L1 mname cache */
STATIC struct fmodsw *fmodsw_mname_cache = NULL;	/* L1 mname cache */

int cdev_count = 0;
int fmod_count = 0;

#if defined CONFIG_STREAMS_SC_MODULE
EXPORT_SYMBOL(cdev_count);
EXPORT_SYMBOL(fmod_count);
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Hash functions
 *
 *  -------------------------------------------------------------------------
 */
/**
 *  init_cdev_hash: - initialize the list_head structures in the cdev hash
 */
STATIC void init_cdev_hash(void)
{
	int i;
	for (i = 0; i < STRDEV_HASH_SIZE; i++)
		INIT_LIST_HEAD((cdevsw_hash + i));
}

/**
 *  init_fmod_hash: - initialize the list_head structures in the fmod hash
 */
STATIC void init_fmod_hash(void)
{
	int i;
	for (i = 0; i < STRMOD_HASH_SIZE; i++)
		INIT_LIST_HEAD((fmodsw_hash + i));
}

/**
 *  __cdev_lookup: - look up a cdev by major device number in cdev hashes
 *  @major: major device number to look up
 *
 *  Look up a STREAMS driver by major device number in the cdevsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @major with
 *  very little performance impact.
 */
STATIC struct cdevsw *__cdev_lookup(major_t major)
{
	if (cdevsw_major_cache && cdevsw_major_cache->d_major == major)
		return (cdevsw_major_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, strdev_hash_slot(major)) {
			struct devinfo *devi = list_entry(pos, struct devinfo, di_list);
			if (devi->major != major)
				continue;
			return ((cdevsw_major_cache = devi->di_dev));
		}
		return (NULL);
	}
}

/**
 *  __cdrv_lookup: - look up a cdev by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *
 *  Look up a STREAMS module by module identifier in the fmodsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @modid with
 *  very little performance impact.
 */
STATIC struct cdevsw *__cdrv_lookup(modID_t modid)
{
	if (cdevsw_modid_cache && cdevsw_modid_cache->d_str->st_rdinit->qi_minfo->mi_idnum == modid)
		return (cdevsw_modid_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, strmod_hash_slot(modid)) {
			struct cdevsw *cdev;
			cdev = list_entry(pos, struct cdevsw, d_hash);
			if (cdev->d_str->st_rdinit->qi_minfo->mi_idnum != modid)
				continue;
			return ((cdevsw_modid_cache = cdev));
		}
		return (NULL);
	}
}

/**
 *  __fmod_lookup: - look up a fmod by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *
 *  Look up a STREAMS module by module identifier in the fmodsw hashes without locking or
 *  acquisition of the result.  This function can be called multiple times with the same @modid with
 *  very little performance impact.
 */
STATIC struct fmodsw *__fmod_lookup(modID_t modid)
{
	if (fmodsw_modid_cache && fmodsw_modid_cache->f_str->st_rdinit->qi_minfo->mi_idnum == modid)
		return (fmodsw_modid_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, strmod_hash_slot(modid)) {
			struct fmodsw *fmod;
			fmod = list_entry(pos, struct fmodsw, f_hash);
			if (fmod->f_str->st_rdinit->qi_minfo->mi_idnum != modid)
				continue;
			return ((fmodsw_modid_cache = fmod));
		}
		return (NULL);
	}
}

STATIC struct cdevsw *__cdev_search(const char *name)
{
	size_t len = strnlen(name, FMNAMESZ + 1);
	if (cdevsw_mname_cache && !strncmp(cdevsw_mname_cache->d_name, name, len))
		return (cdevsw_mname_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, &cdevsw_list) {
			struct cdevsw *cdev = list_entry(pos, struct cdevsw, d_list);
			if (strncmp(cdev->d_name, name, len))
				continue;
			return ((cdevsw_mname_cache = cdev));
		}
		return (NULL);
	}
}

STATIC struct fmodsw *__fmod_search(const char *name)
{
	size_t len = strnlen(name, FMNAMESZ + 1);
	if (fmodsw_mname_cache && !strncmp(fmodsw_mname_cache->f_name, name, len))
		return (fmodsw_mname_cache);
	else {
		struct list_head *pos;
		list_for_each(pos, &fmodsw_list) {
			struct fmodsw *fmod = list_entry(pos, struct fmodsw, f_list);
			if (strncmp(fmod->f_name, name, len))
				continue;
			return ((fmodsw_mname_cache = fmod));
		}
		return (NULL);
	}
}

STATIC struct devnode *__node_search(const struct cdevsw *cdev, const char *name)
{
	size_t len = strnlen(name, FMNAMESZ + 1);
	struct list_head *pos;
	list_for_each(pos, &cdev->d_nodes) {
		struct devnode *node = list_entry(pos, struct devnode, n_list);
		if (strncmp(node->n_name, name, len))
			continue;
		return (node);
	}
	return (NULL);
}

/*
 *  __cdev_lookup_locked: - look up a cdev by major device number in cdev hashes
 *  @major: major device number to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct cdevsw *__cdev_lookup_locked(major_t major, int load)
{
	struct cdevsw *cdev = NULL;
	int reload;
#ifdef CONFIG_KMOD
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
		if (cdev && cdev->d_str && try_inc_mod_count(cdev->d_kmod))
			break;
		cdev = NULL;
	}
#else
	cdev = __cdev_lookup(major);
#endif				/* CONFIG_KMOD */
	return (cdev);
}

/*
 *  __cdrv_lookup_locked: - look up a cdev by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct cdevsw *__cdrv_lookup_locked(modID_t modid, int load)
{
	struct cdevsw *cdev = NULL;
	int reload;
#ifdef CONFIG_KMOD
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
		if (cdev && cdev->d_str && try_inc_mod_count(cdev->d_kmod))
			break;
		cdev = NULL;
	}
#else
	cdev = __cdrv_lookup(modid);
#endif				/* CONFIG_KMOD */
	return (cdev);
}

/*
 *  __fmod_lookup_locked: - look up a fmod by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct fmodsw *__fmod_lookup_locked(modID_t modid, int load)
{
	struct fmodsw *fmod = NULL;
	int reload;
#ifdef CONFIG_KMOD
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
		if (fmod && fmod->f_str && try_inc_mod_count(fmod->f_kmod))
			break;
		fmod = NULL;
	}
#else
	fmod = __fmod_lookup(modid);
#endif				/* CONFIG_KMOD */
	return (fmod);
}

/*
 *  __devi_lookup_locked: - look up a devinfo by device and major device number
 *  @cdev:	cdevsw structure for the driver
 *  @major:	major device number to look up
 */
STATIC struct devinfo *__devi_lookup_locked(struct cdevsw *cdev, major_t major)
{
	struct devinfo *devi = NULL;
	if (cdev && cdev->d_majors.next) {
		struct list_head *pos;
		list_for_each(pos, &cdev->d_majors) {
			struct devinfo *d = list_entry(pos, struct devinfo, di_list);
			if (d->major == major) {
				devi = d;
				break;
			}
		}
	}
	return (devi);
}

/*
 *  __node_lookup_locked: - look up devnode by device a minor device number
 *  @cdev:	cdevsw structure for the driver
 *  @minor:	minor device number to look up
 */
STATIC struct devnode *__node_lookup_locked(const struct cdevsw *cdev, minor_t minor)
{
	struct devnode *node = NULL;
	if (cdev && cdev->d_nodes.next) {
		struct list_head *pos;
		list_for_each(pos, &cdev->d_nodes) {
			struct devnode *n = list_entry(pos, struct devnode, n_list);
			if (n->n_minor == minor) {
				node = n;
				break;
			}
		}
	}
	return (node);
}

#if defined CONFIG_STREAMS_COMPAT_AIX || defined CONFIG_STREAMS_COMPAT_AIX_MODULE
/*
 *  __fmod_str_locked: - lookup up fmodsw by streamtab
 *  @str:	streamtab to look up
 */
STATIC struct fmodsw *__fmod_str_locked(const struct streamtab *str)
{
	struct list_head *pos;
	list_for_each(pos, &fmodsw_list) {
		struct fmodsw *fmod = list_entry(pos, struct fmodsw, f_list);
		if (fmod->f_str != str)
			continue;
		return (fmod);
	}
	return (NULL);
}
#endif

/**
 *  cdev_lookup: - look up a cdev by major device number in cdev hashes
 *  @major: major device number to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct cdevsw *cdev_lookup(major_t major, int load)
{
	struct cdevsw *cdev;
	read_lock(&cdevsw_lock);
	cdev = __cdev_lookup_locked(major, load);
	read_unlock(&cdevsw_lock);
	return (cdev);
}

/**
 *  cdrv_lookup: - look up a cdev by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct cdevsw *cdrv_lookup(modID_t modid, int load)
{
	struct cdevsw *cdev;
	read_lock(&fmodsw_lock);
	cdev = __cdrv_lookup_locked(modid, load);
	read_unlock(&fmodsw_lock);
	return (cdev);
}

/**
 *  fmod_lookup: - look up a fmod by module identifier in fmod hashes
 *  @modid: module identifier to look up
 *  @load: flag indicating whether to attempt to demand load the module
 */
STATIC struct fmodsw *fmod_lookup(modID_t modid, int load)
{
	struct fmodsw *fmod;
	read_lock(&fmodsw_lock);
	fmod = __fmod_lookup_locked(modid, load);
	read_unlock(&fmodsw_lock);
	return (fmod);
}

#if 0
/**
 *  devi_lookup: - look up a devinfo by device and major device number
 *  @cdev:	cdevsw structure for the driver
 *  @major:	major device number to look up
 */
STATIC struct devinfo *devi_lookup(struct cdevsw *cdev, major_t major)
{
	struct devinfo *devi;
	read_lock(&cdevsw_lock);
	devi = __devi_lookup_locked(cdev, major);
	read_unlock(&cdevsw_lock);
	return (devi);
}
#endif

/**
 *  node_lookup: - look up a devnode by device and minor device number
 *  @cdev:	cdevsw structure for the driver
 *  @minor:	minor device number to look up
 */
STATIC struct devnode *node_lookup(const struct cdevsw *cdev, minor_t minor)
{
	struct devnode *node;
	read_lock(&cdevsw_lock);
	node = __node_lookup_locked(cdev, minor);
	read_unlock(&cdevsw_lock);
	return (node);
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
STATIC struct cdevsw *cdev_search(const char *name, int load)
{
	struct cdevsw *cdev = NULL;
	int reload;
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
		if (cdev && cdev->d_str && try_inc_mod_count(cdev->d_kmod))
			break;
		cdev = NULL;
	}
#else				/* CONFIG_KMOD */
	cdev = __cdev_search(name);
#endif				/* CONFIG_KMOD */
	read_unlock(&cdevsw_lock);
	return (cdev);
}

/**
 *  fmod_search: - look up a fmod by name
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 *
 *  The search is exhaustive.  If this function is called multiple times with the same name, the
 *  peformance impact will be minimal.  If the module is not found by name, an attempt will be made
 *  to demand load the kernel module "streams-%s".
 */
STATIC struct fmodsw *fmod_search(const char *name, int load)
{
	struct fmodsw *fmod = NULL;
	int reload;
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
		if (fmod && fmod->f_str && try_inc_mod_count(fmod->f_kmod))
			break;
		fmod = NULL;
	}
#else				/* CONFIG_KMOD */
	fmod = __fmod_search(name);
#endif				/* CONFIG_KMOD */
	read_unlock(&fmodsw_lock);
	return (fmod);
}

/**
 *  node_search: - looke up a minor device node by name
 *  @cdev: character device major structure
 *  @name: name to look up
 */
STATIC struct devnode *node_search(const struct cdevsw *cdev, const char *name)
{
	struct devnode *node = NULL;
	read_lock(&cdevsw_lock);
	do {
		if (!cdev || !cdev->d_nodes.next)
			break;
		if ((node = __node_search(cdev, name)))
			break;
	} while (0);
	read_unlock(&cdevsw_lock);
	return (node);
}

/**
 *  smod_search: - look up a cdev or fmod by name
 *  @name: name to look up
 *  @load: whether to demand load kernel modules
 */
STATIC struct fmodsw *smod_search(const char *name, int load)
{
	void *fmod = NULL;
	if (!fmod)
		fmod = cdev_search(name, load);
	if (!fmod)
		fmod = fmod_search(name, load);
	return (fmod);
}

#if defined CONFIG_STREAMS_COMPAT_AIX || defined CONFIG_STREAMS_COMPAT_AIX_MODULE
/**
 *  fmod_str:	- look up a fmod by streamtab
 *  @str:	streamtab to look up
 */
struct fmodsw *fmod_str(const struct streamtab *str)
{
	struct fmodsw *fmod = NULL;
	read_lock(&fmodsw_lock);
	fmod = __fmod_str_locked(str);
	read_unlock(&fmodsw_lock);
	return (fmod);
}
#endif

/* 
 *  -------------------------------------------------------------------------
 *
 *  Get and put devices and modules.
 *
 *  -------------------------------------------------------------------------
 */

/**
 *  cdev_get: - get a reference to a STREAMS device
 *  @major: (internal) major device number of the STREAMS device
 *
 *  Context: When the calling context can block, an attempt will be made to load the driver by major
 *  device number.
 */
struct cdevsw *cdev_get(major_t major)
{
	return cdev_lookup(major, !in_interrupt());
}

/**
 *  cdev_put:	- put a reference to a STREAMS device
 *  @cdev:	STREAMS device structure pointer to put
 */
void cdev_put(struct cdevsw *cdev)
{
	if (cdev)
		__MOD_DEC_USE_COUNT(cdev->d_kmod);
}
#if defined CONFIG_STREAMS_STH_MODULE || \
    defined CONFIG_STREAMS_COMPAT_AIX_MODULE || \
    defined CONFIG_STREAMS_COMPAT_LIS_MODULE || \
    defined CONFIG_STREAMS_COMPAT_UW7_MODULE || \
    defined CONFIG_STREAMS_CLONE_MODULE
EXPORT_SYMBOL(cdev_get);
EXPORT_SYMBOL(cdev_put);
#endif

/**
 *  cdrv_get:	- get a reference to a STREAMS driver
 *  @modid:	module id number of the STREAMS driver
 */
struct cdevsw *cdrv_get(modID_t modid)
{
	return cdrv_lookup(modid, !in_interrupt());
}
#if defined CONFIG_STREAMS_STH_MODULE || \
    defined CONFIG_STREAMS_COMPAT_UW7_MODULE
EXPORT_SYMBOL(cdrv_get);
#endif

/**
 *  cdrv_put:	- put a reference to a STREAMS driver
 *  @cdev:	STREAMS driver structure pointer to put
 */
void cdrv_put(struct cdevsw *cdev)
{
	cdev_put(cdev);
}
#if defined CONFIG_STREAMS_STH_MODULE
EXPORT_SYMBOL(cdrv_put);
#endif

/**
 *  fmod_get: - get a reference to a STREAMS module
 *  @modid: module id number of the STREAMS module
 *
 *  Context: When the calling context can block, an attempt will be made to load the module by
 *  module identifier.
 */
struct fmodsw *fmod_get(modID_t modid)
{
	return fmod_lookup(modid, !in_interrupt());
}

/**
 *  fmod_put: - put a reference to a STREAMS module
 *  @smod: STREAMS module structure pointer to put
 *
 */
void fmod_put(struct fmodsw *smod)
{
	if (smod)
		__MOD_DEC_USE_COUNT(smod->f_kmod);
}

/**
 *  node_get: - get a reference to a minor device node (devnode)
 *  @cdev:	cdevsw structure for device
 *  @minor:	minor device number
 */
struct devnode *node_get(const struct cdevsw *cdev, minor_t minor)
{
	return node_lookup(cdev, minor);
}

/**
 *  cdev_find: - find a STREAMS device by its name
 *  @name: the name to find
 *
 *  Attempt to find a STREAMS device by name.  If the device cannot be found by name, attempt to
 *  load the kernel module streams-%s where %s is the name requested and check again.
 *
 *  Return Value: A pointer to the STREAMS device, with use count incremented, or %NULL if not found
 *  and could not be loaded.
 *
 *  Context: When the calling context can block, an attempt will be made to load the driver by name.
 */
struct cdevsw *cdev_find(const char *name)
{
	return cdev_search(name, !in_interrupt());
}
#ifdef CONFIG_STREAMS_STH_MODULE
EXPORT_SYMBOL(cdev_find);
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
struct fmodsw *fmod_find(const char *name)
{
	return fmod_search(name, !in_interrupt());
}

struct devnode *node_find(const struct cdevsw *cdev, const char *name)
{
	return node_search(cdev, name);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and Deregistration
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  __register_strmod_locked: - register STREAMS module
 *  @fmod: STREAMS module structure to register
 *  @head: list head in which to include structure
 @  @cntp: pointer to list counter
 */
int __register_strmod_locked(struct fmodsw *fmod, struct list_head *head, int *cntp)
{
	int err = 0;
	do {
		modID_t modid;
		struct fmodsw *f = NULL;
		struct module_info *mi;
		err = -EINVAL;
		if (!fmod || !fmod->f_name || !fmod->f_name[0])
			break;
		{
			struct streamtab *st;
			struct qinit *qi;
			err = -EINVAL;
			if (!(st = fmod->f_str) || !(qi = st->st_rdinit) || !(mi = qi->qi_minfo))
				break;
		}
		err = -EBUSY;
		if ((f = smod_search(fmod->f_name, 0)) && f != fmod)
			break;
		if (!(modid = mi->mi_idnum)) {
			/* find a free module id */
			modid = 0xffff;
			while (modid && (f = __fmod_lookup_locked(modid, 0)) && f != fmod)
				modid--;
			err = -ENXIO;
			if (!modid)	/* table full */
				break;
			mi->mi_idnum = modid;
		} else {
			/* use specified module id */
			err = -EBUSY;
			if ((f = __fmod_lookup_locked(modid, 0)) && f != fmod)
				break;
		}
		if (f != fmod) {
			if (!list_empty(&fmod->f_list)) {
				list_del_init(&fmod->f_list);
				list_del_init(&fmod->f_hash);
				*cntp = *cntp - 1;
			}
			list_add(&fmod->f_list, head);
			list_add(&fmod->f_hash, strmod_hash_slot(modid));
			*cntp = *cntp + 1;
		}
		err = modid;
	} while (0);
	return (err);
}

/*
 *  __unregister_strmod_locked:
 *  @fmod: STREAMS module structure to unregister
 *  @head: list head in which to include structure
 *  @cntp: pointer to list counter
 */
int __unregister_strmod_locked(struct fmodsw *fmod, int *cntp)
{
	int err = 0;
	do {
		err = -EINVAL;
		if (!fmod || !fmod->f_name || !fmod->f_name[0])
			break;
		err = -ENXIO;
		if (!fmod->f_list.next || list_empty(&fmod->f_list))
			break;
		err = -EPERM;
		{
			if (!list_empty(&fmod->f_list)) {
				list_del_init(&fmod->f_list);
				list_del_init(&fmod->f_hash);
				*cntp = *cntp - 1;
			}
		}
	} while (0);
	return (err);
}

/*
 *  __register_major_locked: - register a character device inode
 *  @major: major device number
 *  @cdev: character device switch structure pointer
 *  @fops: file operations to apply to external character device nodes
 */
STATIC int __register_major_locked(major_t major, struct cdevsw *cdev, struct file_operations *fops)
{
	struct devinfo *di = NULL;
	int err = 0;
	do {
		struct cdevsw *c;
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		err = -EINVAL;
		/* ensure that the device is registered */
		if (!cdev->d_list.next || list_empty(&cdev->d_list))
			break;
		err = -EBUSY;
		if ((c = __cdev_lookup_locked(major, 0)) && c != cdev)
			break;
		err = -ENOMEM;
		if (!(di = di_get(cdev)))
			break;
		/* register the character device */
		if ((err = register_chrdev(major, cdev->d_name, fops)) < 0)
			break;
		if (err > 0 && major == 0)
			major = err;
		{
			di->major = major;
			di->minor = 0;	/* FIXME */
			list_add_tail(&di->di_list, &cdev->d_majors);
		}
		err = major;
	} while (0);
	if (err < 0 && di)
		di_put(di);
	return (err);
}

/*
 *  __unregister_major_locked: - unregister a special device inode
 *  @major: major device number (character special devices only)
 *  @cdev: character device switch structure pointer
 */
STATIC int __unregister_major_locked(major_t major, struct cdevsw *cdev)
{
	int err = 0;
	write_lock(&cdevsw_lock);
	do {
		struct devinfo *devi;
		struct cdevsw *c;
		err = -EINVAL;
		if (!cdev || !cdev->d_name || !cdev->d_name[0])
			break;
		if (major) {
			/* deregister one specific major device number */
			err = -ENXIO;
			if (!(c = __cdev_lookup_locked(major, 0)))	/* lookup without demand
									   loading */
				break;
			err = -EPERM;
			if (c != cdev)
				break;
			err = -EIO;
			if (!(devi = __devi_lookup_locked(cdev, major))) {
				swerr();
				break;
			}
			if ((err = unregister_chrdev(major, cdev->d_name)) < 0)
				break;
			list_del_init(&devi->di_list);
			di_put(devi);
		} else {
			struct list_head *pos;
			/* deregister all major device numbers */
			list_for_each(pos, &cdev->d_majors) {
				devi = list_entry(pos, struct devinfo, di_list);
				unregister_chrdev(devi->major, cdev->d_name);
				list_del_init(&devi->di_list);
				di_put(devi);
			}
		}
		err = 0;
	} while (0);
	write_unlock(&cdevsw_lock);
	return (err);
}

/**
 *  register_strmod: - register STREAMS module
 *  @fmod: STREAMS module structure to register
 */
int register_strmod(struct fmodsw *fmod)
{
	int err;
	write_lock(&fmodsw_lock);
	err = __register_strmod_locked((void *) fmod, &fmodsw_list, &fmod_count);
	write_unlock(&fmodsw_lock);
	return (err);
}

/**
 *  unregister_strmod:
 *  @fmod: STREAMS module structure to unregister
 */
int unregister_strmod(struct fmodsw *fmod)
{
	int err;
	write_lock(&fmodsw_lock);
	err = __unregister_strmod_locked((void *) fmod, &fmod_count);
	write_unlock(&fmodsw_lock);
	return (err);
}

/**
 *  register_strdrv: - register STREAMS driver
 *  @cdev: STREAMS device structure to register
 */
int register_strdrv(struct cdevsw *cdev)
{
	int err;
	write_lock(&cdevsw_lock);
	err = __register_strmod_locked((void *) cdev, &cdevsw_list, &cdev_count);
	write_unlock(&cdevsw_lock);
	return (err);
}

/**
 *  unregister_strdrv:
 *  @cdev: STREAMS driver structure to unregister
 */
int unregister_strdrv(struct cdevsw *cdev)
{
	int err;
	write_lock(&cdevsw_lock);
	err = __unregister_strmod_locked((void *) cdev, &cdev_count);
	write_unlock(&cdevsw_lock);
	return (err);
}

/**
 *  register_cmajor: - register a character device inode
 *  @major: major device number
 *  @cdev: character device switch structure pointer
 *  @fops: file operations to apply to external character device nodes
 *
 *  Registers and extenal character special device major number with Linux outside the STREAMS
 *  subsystem.  @fops is the file operations that will be used to open the character device.
 *
 *  The major device number can be specified as zero (0), in which case the major device number will
 *  be assigned to a free major device number by register_chrdev() and returned as a positive return
 *  value.  Any valid external major device number can be used for @major.
 *
 *  register_cmajor() can be called multiple times for the same registered cdevsw entries to register
 *  additional major device numbers for the same entry.  On each call to
 *  register_cmajor() only one
 *  character major device number will be allocated.  If @major is zero on each call, a new
 *  available major device number will be allocated on each call.
 */
int register_cmajor(struct cdevsw *cdev, major_t major, struct file_operations *fops)
{
	int ret;
	write_lock(&cdevsw_lock);
	ret = __register_major_locked(major, cdev, fops);
	write_unlock(&cdevsw_lock);
	return (ret);
}

/**
 *  unregister_cmajor: - unregister a special device inode
 *  @cdev: character device switch structure pointer
 *  @major: major device number (character special devices only)
 *
 *  Deregisters an external character device major number previously registered with
 *  register_cmajor().  @major must be the major number returned from a successful call to
 *  register_cmajor() or the special value zero (0).  When @major is specified, all major device
 *  numbers associated with the driver will be deregistered.  In this way, one one call to
 *  unregister_cmajor() with @major set to zero (0) is necessary after multiple successful calls to
 *  register_cmajor().
 *
 *  Notices: The major device number must be the major device number returned from a successful
 *  register_cmajor() call.  cdev->d_name must have the same value as on the
 *  call to register_cmajor()
 *  or the call will fail.
 */
int unregister_cmajor(struct cdevsw *cdev, major_t major)
{
	int ret;
	write_lock(&cdevsw_lock);
	ret = __unregister_major_locked(major, cdev);
	write_unlock(&cdevsw_lock);
	return (ret);
}

/**
 *  getmid: - get the module identifier associated with a module name
 *  @name: the name of the module
 *
 *  Obtains the module id of the named module.
 *
 *  Return Value: Returns the module identifier associated with the named module or zero (0) if no
 *  module of the specified name can be found on the system.
 *
 *  Context: Can be called from any context.  When called from a blocking context, the function has
 *  the side-effect that the named module may be loaded by module name.  The kernel module demand
 *  loaded will have the module name or alias "streams-%s".
 */
modID_t getmid(const char *name)
{
	modID_t modid = 0;
	struct fmodsw *fmod;
	if ((fmod = smod_search(name, !in_interrupt()))) {
		struct streamtab *st;
		struct qinit *qi;
		struct module_info *mi;
		if ((st = fmod->f_str) && (qi = st->st_rdinit) && (mi = qi->qi_minfo))
			modid = mi->mi_idnum;
		fmod_put(fmod);
	}
	return (modid);
}

/**
 *  getadmin: - get the administrative function associated with a module identifier
 *  @modid: the module identifier
 *
 *  Obtains the qi_qadmin function pointer for the module identifier by the module identifier
 *  @modid.
 *
 *  Return Value: Returns a function pointer to the qi_qadmin() procedure for the module, which may be
 *  %NULL, or returns %NULL on failure.
 *
 *  Context: Can be called from any context.  When called from a blocking context, the function has
 *  the side-effect that the identified module may be loaded by module identifier.  The kernel
 *  module demand loaded will have the module name or alias "streams-modid-%u".
 */
qi_qadmin_t getadmin(modID_t modid)
{
	qi_qadmin_t qadmin = NULL;
	struct fmodsw *fmod;
	if ((fmod = fmod_lookup(modid, !in_interrupt()))) {
		struct streamtab *st;
		struct qinit *qi;
		if ((st = fmod->f_str) && (qi = st->st_rdinit))
			qadmin = qi->qi_qadmin;
		fmod_put(fmod);
	}
	return (qadmin);
}

/**
 *  strreg_init: - initialize the registeration module
 */
int strreg_init(void)
{
	init_cdev_hash();
	init_fmod_hash();
	return (0);
}

/**
 *  strreg_init: - deinitialize the registeration module
 */
void strreg_exit(void)
{
	return;
}
