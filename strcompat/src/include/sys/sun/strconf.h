/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.17 2007/08/12 15:51:13 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/12 15:51:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strconf.h,v $
 Revision 0.9.2.17  2007/08/12 15:51:13  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.16  2006/12/08 05:08:18  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.15  2006/11/03 10:39:23  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_SUN_STRCONF_H__
#define __SYS_SUN_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.17 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_STRCONF_H__
#warning "Do not include sys/aix/strconf.h directly, include sys/strconf.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __SUN_EXTERN_INLINE
#define __SUN_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __SUN_EXTERN_INLINE */

#ifndef __SUN_EXTERN
#define __SUN_EXTERN extern streamscall
#endif

#ifndef _SUN_SOURCE
#warning "_SUN_SOURCE not defined but SUN strconf.h included"
#endif

#include <sys/strcompat/config.h>

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
	int (*cb_open) (void);
	int (*cb_close) (void);
	int (*cb_strategy) (void);
	int (*cb_print) (void);
	int (*cb_dump) (void);
	int (*cb_read) (void);
	int (*cb_write) (void);
	int (*cb_ioctl) (void);
	int (*cb_devmap) (void);
	int (*cb_mmap) (void);
	int (*cb_segmap) (void);
	int (*cb_chpoll) (void);
	int (*cb_prop_op) (void);
	struct streamtab *cb_str;	/* streams information */
	int cb_flag;
	int cb_rev;
	int (*cb_aread) (void);
	int (*cb_awrite) (void);
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

//struct dev_info;
typedef struct dev_info dev_info_t;

struct dev_ops {
	int devo_rev;
	int devo_refcnt;
	int (*devo_getinfo) (dev_info_t * dip, ddi_info_cmn_t cmd, void *arg, void **result);
	int (*devo_identify) (dev_info_t * dip);
	int (*devo_probe) (dev_info_t * dip);
	int (*devo_attach) (dev_info_t * dip, ddi_attach_cmn_t cmd);
	int (*devo_detach) (dev_info_t * dip, ddi_detach_cmn_t cmd);
	int (*devo_reset) (dev_info_t * dip, ddi_reset_cmn_t cmd);
	struct cb_ops *devo_cb_ops;
	struct bus_ops *devo_bus_ops;
	int (*devo_power) (dev_info_t * dip, int component, int level);
};

__SUN_EXTERN_INLINE int
nodev(void)
{
	return (ENXIO);
}
__SUN_EXTERN_INLINE int
nulldev(void)
{
	return (0);
}
__SUN_EXTERN_INLINE int
nochpoll(void)
{
	return (ENXIO);
}
__SUN_EXTERN_INLINE int
ddi_prop_op(void)
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
	int ml_rev;			/* revision, must be MODREV_1 */
	void *ml_linkage[4];		/* null terminated array of linkage structures */
};

struct modinfo {
	int __dummy;
};

__SUN_EXTERN int mod_install(struct modlinkage *ml);
__SUN_EXTERN int mod_remove(struct modlinkage *ml);
__SUN_EXTERN int mod_info(struct modlinkage *ml, struct modinfo *mi);

#else
#ifdef _SUN_SOURCE
#warning "_SUN_SOURCE defined but not CONFIG_STREAMS_COMPAT_SUN"
#endif
#endif				/* CONFIG_STREAMS_COMPAT_SUN */

#endif				/* __SYS_SUN_STRCONF_H__ */
