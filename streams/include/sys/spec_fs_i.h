/*****************************************************************************

 @(#) $Id: spec_fs_i.h,v 0.9.2.7 2006/12/18 10:08:54 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/12/18 10:08:54 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_SPEC_FS_I_H__
#define __SYS_SPEC_FS_I_H__

#ident "@(#) $RCSfile: spec_fs_i.h,v $ $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

 /* Note: this is identical layout to a socket (rearranged for validation).  The minor exception is 
    that the the sock pointer points to an stdata and the proto_ops pointer points to a stream_ops
    and we point to the dentry instead of the inode. The purpose here is the same a sockets: to
    keep asyncrhonous waiters separate from the stream head so that the stream head can be
    destroyed independently.  Also, to keep the stream head separate from the inode so that the
    stream head can linger on for a while after the inode has been deleted. -- brian */

#define specfs_inode_info snode

struct specfs_inode_info {
	int s_state;			/* stream state */
	int s_flags;			/* registration flags */
	struct cdevsw *s_cdevsw;	/* registered minor */
	struct inode *s_inode;		/* the inode we are attached to */
	struct fasync_struct *s_fasync;	/* asynchronous waiter list */
	struct file *s_file;		/* user file pointer (only valid at open) */
	struct stdata *s_stream;	/* active stream head */
	wait_queue_head_t s_wait;	/* wait list */
	atomic_t s_refs;		/* reference count */
};

#endif				/* __SYS_SPEC_FS_I_H__ */
