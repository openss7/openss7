/*****************************************************************************

 @(#) $Id: strattach.h,v 0.9 2004/03/01 00:08:40 brian Exp $

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

 Last Modified $Date: 2004/03/01 00:08:40 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRATTACH_H__
#define __LOCAL_STRATTACH_H__

/*
   The following symbols have to be ripped.
 */
#ifndef HAVE_TASK_NAMESPACE_SEM
extern struct semaphore mount_sem;
#endif
struct vfsmount *clone_mnt(struct vfsmount *old, struct dentry *root);
int check_mnt(struct vfsmount *mnt);
int graft_tree(struct vfsmount *mnt, struct nameidata *nd);
int do_umount(struct vfsmount *mnt, int flags);

long do_fattach(const struct file *file, const char *file_name);
long do_fdetach(const char *file_name);

#endif				/* __LOCAL_STRATTACH_H__ */
