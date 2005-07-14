/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.10 2005/07/14 03:40:07 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/07/14 03:40:07 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_LFS_STRCONF_H__
#define __SYS_LFS_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/14 03:40:07 $"

#ifndef __SYS_STRCONF_H__
#warning "Do not include sys/aix/strconf.h directly, include sys/strconf.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __LFS_EXTERN_INLINE
#define __LFS_EXTERN_INLINE extern __inline__
#endif				/* __LFS_EXTERN_INLINE */

#ifndef _LFS_SOURCE
#warning "_LFS_SOURCE not defined but LFS strconf.h included"
#endif

#include <sys/strcompat/config.h>

#if defined(CONFIG_STREAMS_COMPAT_LFS) || defined(CONFIG_STREAMS_COMPAT_LFS_MODULE)

struct _fmodsw {
	struct list_head f_list;	/* list of all structures */
	struct list_head f_hash;	/* list of module hashes in slot */
	const char *f_name;		/* module name */
	struct streamtab *f_str;	/* pointer to streamtab for module */
	uint f_flag;			/* module flags */
	uint f_modid;			/* module id */
	atomic_t f_count;		/* open count */
	int f_sqlvl;			/* q sychronization level */
	struct syncq *f_syncq;		/* synchronization queue */
	struct module *f_kmod;		/* kernel module */
};

struct cdevsw;

struct devnode {
	struct list_head n_list;	/* list of all nodes for this device */
	struct list_head n_hash;	/* list of major hashes in slot */
	const char *n_name;		/* node name */
	struct streamtab *n_str;	/* streamtab for node */
	uint n_flag;			/* node flags */
	uint n_modid;			/* node module id */
	atomic_t n_count;		/* open count */
	int n_sqlvl;			/* q sychronization level */
	struct syncq *n_syncq;		/* synchronization queue */
	struct module *n_kmod;		/* kernel module */
	/* above must match fmodsw */
	int n_major;			/* node major device number */
	struct inode *n_inode;		/* specfs inode */
	mode_t n_mode;			/* inode mode */
	/* above must match cdevsw */
	int n_minor;			/* node minor device number */
	struct cdevsw *n_dev;		/* character device */
};
#define N_MAJOR		0x01	/* major device node */

struct file_operations;

struct cdevsw {
	struct list_head d_list;	/* list of all structures */
	struct list_head d_hash;	/* list of module hashes in slot */
	const char *d_name;		/* driver name */
	struct streamtab *d_str;	/* pointer to streamtab for driver */
	uint d_flag;			/* driver flags */
	uint d_modid;			/* driver moidule id */
	atomic_t d_count;		/* open count */
	int d_sqlvl;			/* q sychronization level */
	struct syncq *d_syncq;		/* synchronization queue */
	struct module *d_kmod;		/* kernel module */
	/* above must match fmodsw */
	int d_major;			/* base major device number */
	struct inode *d_inode;		/* specfs inode */
	mode_t d_mode;			/* inode mode */
	/* above must match devnode */
	struct file_operations *d_fop;	/* file operations */
	struct list_head d_majors;	/* major device nodes for this device */
	struct list_head d_minors;	/* minor device nodes for this device */
	struct list_head d_apush;	/* autopush list */
	struct stdata *d_plinks;	/* permanent links for this device */
	struct list_head d_stlist;	/* stream head list for this device */
};

#undef register_strdev
#undef register_strmod
#undef unregister_strdev
#undef unregister_strmod

__LFS_EXTERN_INLINE int register_strnod(struct cdevsw *cdev, struct devnode *cmin, minor_t minor)
{
	return (-EOPNOTSUPP);
}
__LFS_EXTERN_INLINE int register_strdev(struct cdevsw *cdev, major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, cdev->d_str, 255, cdev->d_name)) < 0)
		return (err);
	return ((cdev->d_major = err));
}
__LFS_EXTERN_INLINE int register_strdrv(struct cdevsw *cdev)
{
	return register_strdev(cdev, cdev->d_major);
}
__LFS_EXTERN_INLINE int register_strmod(struct _fmodsw *fmod)
{
	return lis_register_strmod(fmod->f_str, fmod->f_name);
}
__LFS_EXTERN_INLINE int unregister_strnod(struct cdevsw *cdev, minor_t minor)
{
	return (-EOPNOTSUPP);
}
__LFS_EXTERN_INLINE int unregister_strdev(struct cdevsw *cdev, major_t major)
{
	return lis_unregister_strdev(major);
}
__LFS_EXTERN_INLINE int unregister_strdrv(struct cdevsw *cdev)
{
	return unregister_strdev(cdev, cdev->d_major);
}
__LFS_EXTERN_INLINE int unregister_strmod(struct _fmodsw *fmod)
{
	return lis_unregister_strmod(fmod->f_str);
}

__LFS_EXTERN_INLINE int apush_get(struct strapush *sap)
{
	return lis_apush_get(sap);
}
__LFS_EXTERN_INLINE int apush_set(struct strapush *sap)
{
	return lis_apush_set(sap);
}
__LFS_EXTERN_INLINE int apush_vml(struct str_list *slp)
{
	return lis_apush_vml(slp);
}

__LFS_EXTERN_INLINE int autopush_add(struct strapush *sap)
{
	return apush_set(sap);
}
__LFS_EXTERN_INLINE int autopush_del(struct strapush *sap)
{
	sap->sap_cmd = SAP_CLEAR;
	return apush_set(sap);
}
__LFS_EXTERN_INLINE int autopush_vml(struct str_mlist *ml, int nmods)
{
	struct str_list sl;
	sl.sl_nmods = nmods;
	sl.sl_modlist = ml;
	return apush_vml(&sl);
}
__LFS_EXTERN_INLINE struct strapush *autopush_find(dev_t dev)
{
	struct strapush *sap;
	if ((sap = kmem_alloc(sizeof(*sap), KM_NOSLEEP))) {
		sap->sap_cmd = SAD_GAP;
		sap->sap_major = getmajor(dev);
		sap->sap_minor = getminor(dev);
		apush_get(sap);
	}
	return (sap);
}

#elif defined(_LFS_SOURCE)
#warning "_LFS_SOURCE defined but not CONFIG_STREAMS_COMPAT_LFS"
#endif				/* CONFIG_STREAMS_COMPAT_LFS */

#endif				/* __SYS_LFS_STRCONF_H__ */
