/*****************************************************************************

 @(#) strreg.h,v 0.9.2.8 2003/10/26 17:25:55 brian Exp

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified 2003/10/26 17:25:55 by brian

 *****************************************************************************/

#ifndef __LOCAL_STRREG_H__
#define __LOCAL_STRREG_H__

/* arguments definition */
typedef struct str_args {
	struct file *file;
	dev_t dev;
	int oflag;
	int sflag;
	cred_t *crp;
	struct qstr name;
	char buf[32];
} str_args_t;

extern rwlock_t cdevsw_lock;
extern rwlock_t fmodsw_lock;

extern struct list_head cdevsw_list;
extern struct list_head fmodsw_list;

extern struct cdevsw *cdevsw[MAX_STRDEV];
extern struct fmodsw *fmodsw[MAX_STRMOD];

extern int strm_open(struct inode *, struct file *);
extern int sdev_open(struct inode *i, struct file *f, struct vfsmount *mnt, struct str_args *argp);

/* initialization for main */
extern int strreg_init(void);
extern void strreg_exit(void);

#endif				/* __LOCAL_STRREG_H__ */
