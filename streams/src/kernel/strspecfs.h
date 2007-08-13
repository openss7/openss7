/*****************************************************************************

 @(#) $Id: strspecfs.h,v 0.9.2.21 2007/08/13 22:46:19 brian Exp $

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

 Last Modified $Date: 2007/08/13 22:46:19 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strspecfs.h,v $
 Revision 0.9.2.21  2007/08/13 22:46:19  brian
 - GPLv3 header updates

 Revision 0.9.2.20  2006/12/18 10:09:00  brian
 - updated headers for release

 *****************************************************************************/

#ifndef __LOCAL_STRSPECFS_H__
#define __LOCAL_STRSPECFS_H__

#ident "@(#) $RCSfile: strspecfs.h,v $ $Name:  $($Revision: 0.9.2.21 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __EXTERN
#define __EXTERN extern
#endif

#ifndef __STREAMS_EXTERN
#define __STREAMS_EXTERN __EXTERN streams_fastcall
#endif

#define SPEC_SBI_MAGIC 0XFEEDDEAF
struct spec_sb_info {
	u32 sbi_magic;
	int sbi_setuid;
	int sbi_setgid;
	int sbi_setmod;
	uid_t sbi_uid;
	gid_t sbi_gid;
	umode_t sbi_mode;
};

// extern struct file_operations spec_dev_f_ops;

__STREAMS_EXTERN struct vfsmount *specfs_mount(void);
__STREAMS_EXTERN void specfs_umount(void);

__STREAMS_EXTERN int spec_reparent(struct file *file, struct cdevsw *cdev, dev_t dev);
__STREAMS_EXTERN int spec_open(struct file *file, struct cdevsw *cdev, dev_t dev, int sflag);

#ifndef BIG_COMPILE
__STREAMS_EXTERN struct inode *spec_snode(dev_t dev, struct cdevsw *cdev);
#endif

#if 0
/* initialization for main */
__STREAMS_EXTERN int strspecfs_init(void);
__STREAMS_EXTERN void strspecfs_exit(void);
#endif

#endif				/* __LOCAL_STRSPECFS_H__ */
