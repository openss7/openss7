/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.20 2005/10/15 10:19:47 brian Exp $

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

 Last Modified $Date: 2005/10/15 10:19:47 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STREAMS_STRCONF_H__
#define __SYS_STREAMS_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.20 $) $Date: 2005/10/15 10:19:47 $"

#ifndef __SYS_STRCONF_H__
#warning "Do no include sys/streams/strconf.h directly, include sys/strconf.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#include <linux/fs.h>		/* for file_operations */

#include <sys/sad.h>		/* for strapush */

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

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
	SQLVL_PERSTREAM = 7,		/* per-stream synchronization */
} sqlvl_t;

extern int register_strnod(struct cdevsw *cdev, struct devnode *cmin, minor_t minor);
extern int register_strdev(struct cdevsw *cdev, major_t major);
extern int register_strdrv(struct cdevsw *cdev);
extern int register_strmod(struct fmodsw *fmod);
extern int unregister_strnod(struct cdevsw *cdev, minor_t minor);
extern int unregister_strdev(struct cdevsw *cdev, major_t major);
extern int unregister_strdrv(struct cdevsw *cdev);
extern int unregister_strmod(struct fmodsw *fmod);

extern int autopush_add(struct strapush *sap);
extern int autopush_del(struct strapush *sap);
extern int autopush_vml(struct str_mlist *smp, int nmods);
extern struct strapush *autopush_find(dev_t dev);
extern struct strapush *autopush_search(const char *name, minor_t minor);

extern int apush_get(struct strapush *sap);
extern int apush_set(struct strapush *sap);
extern int apush_vml(struct str_list *slp);

#endif				/* __SYS_STREAMS_STRCONF_H__ */
