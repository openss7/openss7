/*****************************************************************************

 @(#) $Id: dki.h,v 0.9.2.13 2005/08/29 10:36:57 brian Exp $

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

 Last Modified $Date: 2005/08/29 10:36:57 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STREAMS_DKI_H__
#define __SYS_STREAMS_DKI_H__ 1

#ident "@(#) $RCSfile: dki.h,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/08/29 10:36:57 $"

#ifndef __SYS_DKI_H__
#warn "Do no include sys/streams/dki.h directly, include sys/dki.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/kdev_t.h>

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

typedef __kernel_dev_t major_t;
typedef __kernel_dev_t minor_t;

/* same layout as in task_struct */
#define current_creds ((cred_t *)(&current->uid))
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

#if 0
/* for LiS binary compatibility */
typedef struct lis_cred {
	uid_t cr_uid;			/* effective user id */
	gid_t cr_gid;			/* effective group id */
	uid_t cr_ruid;			/* real user id */
	gid_t cr_rgid;			/* real group id */
} lis_cred_t;
#endif

/* make SVR4.2 oflag from file flags and mode */
#define make_oflag(__f) \
	(((__f)->f_flags & ~O_ACCMODE) | \
	 ((__f)->f_mode & O_ACCMODE) | \
	 ((__f)->f_flags & FNDELAY ? (O_NONBLOCK | O_NDELAY) : 0))

typedef struct klock {
	unsigned long kl_isrflags; /* no longer required */
	rwlock_t kl_lock;
	struct task_struct *kl_owner;
	uint kl_nest;
	wait_queue_head_t kl_waitq;
} klock_t;

#endif				/* __SYS_STREAMS_DKI_H__ */
