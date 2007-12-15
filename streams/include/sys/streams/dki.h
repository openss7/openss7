/*****************************************************************************

 @(#) $Id: dki.h,v 0.9.2.19 2007/12/15 20:19:49 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2007/12/15 20:19:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dki.h,v $
 Revision 0.9.2.19  2007/12/15 20:19:49  brian
 - updates

 Revision 0.9.2.18  2007/08/13 22:46:08  brian
 - GPLv3 header updates

 *****************************************************************************/

#ifndef __SYS_STREAMS_DKI_H__
#define __SYS_STREAMS_DKI_H__ 1

#ident "@(#) $RCSfile: dki.h,v $ $Name:  $($Revision: 0.9.2.19 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_DKI_H__
#warning "Do no include sys/streams/dki.h directly, include sys/dki.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/kdev_t.h>

typedef __kernel_dev_t major_t;
typedef __kernel_dev_t minor_t;

#ifdef CONFIG_STREAMS_LIS_BCM

/* for LiS binary compatibility */
typedef struct cred {
	uid_t cr_uid;			/* effective user id */
	gid_t cr_gid;			/* effective group id */
	uid_t cr_ruid;			/* real user id */
	gid_t cr_rgid;			/* real group id */
} cred_t;

#else				/* CONFIG_STREAMS_LIS_BCM */

/* same layout as in task_struct */
typedef struct cred {
	uid_t cr_ruid, cr_uid, cr_suid, cr_fsuid;
	gid_t cr_rgid, cr_gid, cr_sgid, cr_fsgid;
#ifdef NGROUPS
	int cr_ngroups;
	gid_t cr_groups[NGROUPS];
#endif
#ifdef NGROUPS_SMALL
	struct group_info *cr_group_info;
#endif
} cred_t;

#endif				/* CONFIG_STREAMS_LIS_BCM */

/* doesn't work for LIS BCM. */
#define current_creds ((cred_t *)(&current->uid))

/* make SVR4.2 oflag from file flags and mode */
#define make_oflag(__f) \
	(((__f)->f_flags & ~O_ACCMODE) | \
	 ((__f)->f_mode & O_ACCMODE) | \
	 ((__f)->f_flags & FNDELAY ? (O_NONBLOCK | O_NDELAY) : 0))

typedef struct klock {
	unsigned long kl_isrflags;	/* no longer required */
	rwlock_t kl_lock;
	struct task_struct *kl_owner;
	uint kl_nest;
	wait_queue_head_t kl_waitq;
} klock_t;

#endif				/* __SYS_STREAMS_DKI_H__ */
