/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.4 2004/03/07 23:53:43 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2004/03/07 23:53:43 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STRCONF_H__
#define __SYS_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/03/07 23:53:43 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#include <linux/fs.h>		/* for file_operations */

#include <sys/sad.h>		/* for strapush */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

#ifndef __SUN_EXTERN_INLINE
#define __SUN_EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

#if defined(CONFIG_STREAMS_COMPAT_AIX) || defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)

typedef struct {
	char *sc_name;
	struct streamtab *sc_str;
	int sc_open_stylesc_flags;
	int sc_major;
	int sc_sqlevel;
	caddr_t sc_sqinfo;
} strconf_t;

extern int str_install_AIX(int cmd, strconf_t * conf);

#define STR_LOAD_DEV	1	/* add a device */
#define STR_UNLOAD_DEV	2	/* remove a device */
#define STR_LOAD_MOD	3	/* load a module */
#define STR_UNLOAD_MOD	4	/* unload a module */

#define STR_OLD_OPEN	0x00000000	/* SVR3 open semantics */
#define STR_NEW_OPEN	0x00000100	/* SVR4 open semantics */

#define STR_QSAFETY     0x00000200	/* Module needs safe callbacks */
#define STR_MPSAFE	0x00000400	/* Module is MP safe */
#define STR_PERSTREAM	0x00000800	/* Module has per-stream sync */
#define STR_Q_NOTTOSPEC	0x00001000	/* Module runs under process context */
#define STR_64BIT	0x00002000	/* 64-bit capable */
#define STR_NEWCLONING	0x00004000	/* Module does cloning without CLONEOPEN */

#if defined(_AIX_SOURCE) && !defined(str_install)
#define str_install(__cmd, __conf) str_install_AIX(__cmd, __conf)
#endif

#elif defined(_AIX_SOURCE)
#warning "_AIX_SOURCE defined but not CONFIG_STREAMS_COMPAT_AIX"
#endif				/* CONFIG_STREAMS_COMPAT_AIX */

#if defined(CONFIG_STREAMS_COMPAT_OSF) || defined(CONFIG_STREAMS_COMPAT_OSF_MODULE)

#if 0				/* we don't support the static configuration */

#define OSF_STREAMS_CONFIG_10   0x04026019
#define OSF_STREAMS_CONFIG_11   0x0503611A

typedef struct str_config {
	uint sc_version;
	uint sc_sa_flags;
	char sc_sa_name[FMNAMESZ + 1];
	dev_t sc_devnum;
} str_config_t;

#endif

#define OSF_STREAMS_10  0x04026019	/* OSF/1.0 */
#define OSF_STREAMS_11  0x0503611B	/* OSF/1.1 */

struct streamadm {
	uint sa_version;
	uint sa_flags;
	char sa_name[FMNAMESZ + 1];
	caddr_t sa_ttys;
	uint sa_sync_level;
	caddr_t sa_sync_info;
};

extern dev_t strmod_add(dev_t dev, struct streamtab *str, struct streamadm *sa);
extern int strmod_del(dev_t dev, struct streamtab *str, struct streamadm *sa);

#elif defined(_OSF_SOURCE)
#warning "_OSF_SOURCE defined but not CONFIG_STREAMS_COMPAT_OSF"
#endif				/* CONFIG_STREAMS_COMPAT_OSF */

#if defined(CONFIG_STREAMS_COMPAT_HPUX) || defined(CONFIG_STREAMS_COMPAT_HPUX_MODULE)

typedef struct stream_inst {
	char *name;			/* name of driver or module */
	int inst_major;			/* major number for driver (-1 dynamic or module) */
	struct streamtab inst_str_tab;	/* current streams tab entry */
	unsigned int inst_flags;	/* flags, e.g. STR_SVR4_OPEN, STR_IS_MODULE */
	int inst_sync_level;		/* sync level defined by stream.h */
	char inst_sync_info[FMNAMESZ + 1];	/* elsewhere sync param described in MP */
} streams_info_t;

extern int str_install_HPUX(struct stream_inst *inst);
extern int str_uninstall(struct stream_inst *inst);

#if defined(_HPUX_SOURCE) && !defined(str_install)
#define str_install(__inst) str_install_HPUX(__inst)
#endif

#elif defined(_HPUX_SOURCE)
#warning "_HPUX_SOURCE defined but not CONFIG_STREAMS_COMPAT_HPUX"
#endif				/* CONFIG_STREAMS_COMPAT_HPUX */

#define STR_IS_DEVICE   0x00000001	/* device */
#define STR_IS_MODULE   0x00000002	/* module */
#define STR_TYPE_MASK   (STR_IS_DEVICE|STR_IS_MODULE)
#define STR_SYSV4_OPEN  0x00000100	/* V.4 open signature/return */
#define STR_QSAFETY     0x00000200	/* Module needs safe callbacks */
#define STR_IS_SECURE   0x00010000	/* Module/device is secure */

typedef enum {
	SQLVL_DEFAULT = 0,		/* default level */
	SQLVL_GLOBAL = 1,		/* STREAMS scheduler level */
	SQLVL_ELSEWHERE = 2,		/* module group level */
	SQLVL_MODULE = 3,		/* module level (default) */
	SQLVL_QUEUEPAIR = 4,		/* queue pair level */
	SQLVL_QUEUE = 5,		/* queue level */
	SQLVL_NOP = 6,			/* no synchronization */
} sqlvl_t;

#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)

#if 0
struct cb_ops {
	int (*cb_open) (dev_t *devp, int flag, int otyp, cred_t *credp);
	int (*cb_close) (dev_t dev, int flag, int otyp, cred_t *credp);
	int (*cb_strategy) (struct buf * bp);
	int (*cb_print) (dev_t dev, char *str);
	int (*cb_dump) (dev_t dev, caddr_t addr, daddr_t blkno, int nblk);
	int (*cb_read) (dev_t dev, struct uio * uiop, cred_t *credp);
	int (*cb_write) (dev_t dev, struct uio * uiop, cred_t *credp);
	int (*cb_ioctl) (dev_t dev, int cmd, intptr_t arg, int mode, cred_t *credp, int *rvalp);
	int (*cb_devmap) (dev_t dev, devmap_cookie_t dhp, offset_t off, size_t len, size_t *maplen,
			  uint_t model);
	int (*cb_mmap) (dev_t dev, off_t off, int prot);
	int (*cb_segmap) (dev_t dev, off_t off, struct as * asp, caddr_t *addrp, off_t len,
			  unsigned int prot, unsigned int maxprot, unsigned int flags,
			  cred_t *credp);
	int (*cb_chpoll) (dev_t dev, short events, int anyyet, short *reventsp,
			  struct pollhead ** phpp);
	int (*cb_prop_op) (dev_t dev, dev_info_t * dip, ddi_prop_op_t prop_op, int mod_flags,
			   char *name, caddr_t valuep, int *length);
	struct streamtab *cb_str;	/* streams information */
	int cb_flag;
	int cb_rev;
	int (*cb_aread) (dev_t dev, struct aio_req * aio, cred_t *credp);
	int (*cb_awrite) (dev_t dev, struct aio_req * aio, cred_t *credp);
};
#else
struct cb_ops {
	int (*cb_open) ();
	int (*cb_close) ();
	int (*cb_strategy) ();
	int (*cb_print) ();
	int (*cb_dump) ();
	int (*cb_read) ();
	int (*cb_write) ();
	int (*cb_ioctl) ();
	int (*cb_devmap) ();
	int (*cb_mmap) ();
	int (*cb_segmap) ();
	int (*cb_chpoll) ();
	int (*cb_prop_op) ();
	struct streamtab *cb_str;	/* streams information */
	int cb_flag;
	int cb_rev;
	int (*cb_aread) ();
	int (*cb_awrite) ();
};
#endif

#if 0
/* not sure about these values */
#define D_MP		(1<<0)	/* MT safe */
#define D_MTPERQ	(1<<1)	/* inner perimeter per queue */
#define D_MTQPAIR	(1<<2)	/* inner perimeter per queue qair */
#define D_MTPERMOD	(1<<3)	/* inner perimeter per queue module */
#define D_MTOUTPERIM	(1<<4)	/* outer perimeter per queue module */
#define D_MTPUTSHARED	(1<<5)	/* enter inner perimeter shared on putp() */
#define D_MTOCEXCL	(1<<6)	/* qopen() qclose() entered exclusive at outer perimeter */
#define D_64BIT		(1<<7)	/* properly handles 64-bit offsets */
#define D_NEW		(1<<8)	/* SVR4 open and close */
#endif

typedef enum {
	DDI_INFO_DEVT2DEVINFO = 1,
	DDI_INFO_DEVT2INSTANCE,
} ddi_info_cmn_t;

typedef enum {
	DDI_ATTACH = 1,
	DDI_PM_RESUME,
	DDI_RESUME,
} ddi_attach_cmn_t;

typedef enum {
	DDI_DETACH = 1,
	DDI_PM_SUSPEND,
	DDI_SUSPEND,
} ddi_detach_cmn_t;

typedef enum {
	DDI_RESET = 1,
} ddi_reset_cmn_t;

struct dev_info;
typedef struct dev_info dev_info_t;

struct dev_ops {
	int devo_rev;
	int devo_refcnt;
	int (*devo_getinfo)(dev_info_t *dip, ddi_info_cmn_t cmd, void *arg, void **result);
	int (*devo_identify)(dev_info_t *dip);
	int (*devo_probe)(dev_info_t *dip);
	int (*devo_attach)(dev_info_t *dip, ddi_attach_cmn_t cmd);
	int (*devo_detach)(dev_info_t *dip, ddi_detach_cmn_t cmd);
	int (*devo_reset)(dev_info_t *dip, ddi_reset_cmn_t cmd);
	struct cb_ops *devo_cb_ops;
	struct bus_ops *devo_bus_ops;
	int (*devo_power)(dev_info_t *dip, int component, int level);
};

__SUN_EXTERN_INLINE int nodev()
{
	return (ENXIO);
}
__SUN_EXTERN_INLINE int nulldev()
{
	return (0);
}
__SUN_EXTERN_INLINE int nochpoll()
{
	return (ENXIO);
}
__SUN_EXTERN_INLINE int ddi_prop_op()
{
	return (0);
}

#define MODMAXNAMELEN	64

/* fake mod_ops structure */
struct mod_ops {
	int modo_rev;
	int modo_refcnt;
	/* blah */
};

extern struct mod_ops mod_strmops;

struct modldrv {
	struct mod_ops *drv_modops;
	char *drv_link_info;
	struct dev_ops *drv_dev_ops;
};

struct sun_fmodsw {
	char f_name[FMNAMESZ + 1];	/* module name */
	struct streamtab *f_str;	/* streams information */
	int f_flag;			/* flags (same as cb_flag above */
};

struct modlstrmod {
	struct mod_ops *strmod_modops;
	char *strmod_linkinfo;
	struct sun_fmodsw *strmod_fmodsw;
};

#define MODREV_1 1

struct modlinkage {
	int ml_rev; /* revision, must be MODREV_1 */
	void *ml_linkage[4]; /* null terminated array of linkage structures */
};

struct modinfo;

extern int mod_install(struct modlinkage *ml);
extern int mod_remove(struct modlinkage *ml);
extern int mod_info(struct modlinkage *ml, struct modinfo *mi);

#elif defined(_SUN_SOURCE)
#warning "_SUN_SOURCE defined but not CONFIG_STREAMS_COMPAT_SUN"
#endif

#if defined(CONFIG_STREAMS_COMPAT_LIS) || defined(CONFIG_STREAMS_COMPAT_LIS_MODULE)
extern int lis_register_strdev(major_t major, struct streamtab *strtab, int nminor,
			       const char *name);
extern int lis_unregister_strdev(major_t major);
extern modID_t lis_register_strmod(struct streamtab *strtab, const char *name);
extern int lis_unregister_strmod(struct streamtab *strtab);
#elif defined(_LIS_SOURCE)
#warning "_LIS_SOURCE defined but not CONFIG_STREAMS_COMPAT_LIS"
#endif				/* CONFIG_STREAMS_COMPAT_LIS */

extern int register_inode(dev_t dev, struct cdevsw *cdev, struct file_operations *fops);
extern int register_inode_major(dev_t dev, struct cdevsw *cdev, struct file_operations *fops);
extern int register_inode_minor(dev_t dev, struct cdevsw *cdev);
extern int register_strdev(dev_t dev, struct cdevsw *cdev);
extern int register_strdev_major(dev_t dev, struct cdevsw *cdev);
extern int register_strdev_minor(dev_t dev, struct cdevsw *cdev);
extern int register_strmod(modID_t modi, struct fmodsw *fmod);
extern int unregister_inode(dev_t dev, struct cdevsw *cdev);
extern int unregister_inode_major(dev_t dev, struct cdevsw *cdev);
extern int unregister_inode_minor(dev_t dev, struct cdevsw *cdev);
extern int unregister_strdev(dev_t dev, struct cdevsw *cdev);
extern int unregister_strdev_major(dev_t dev, struct cdevsw *cdev);
extern int unregister_strdev_minor(dev_t dev, struct cdevsw *cdev);
extern int unregister_strmod(modID_t modi, struct fmodsw *fmod);

extern int autopush_add(struct strapush *sap);
extern int autopush_del(struct strapush *sap);
extern int autopush_vml(struct str_mlist *smp, int nmods);
extern struct strapush *autopush_find(dev_t dev);

extern int apush_get(struct strapush *sap);
extern int apush_set(struct strapush *sap);
extern int apush_vml(struct str_list *slp);

#endif				/* __SYS_STRCONF_H__ */
