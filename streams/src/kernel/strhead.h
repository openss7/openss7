/*****************************************************************************

 @(#) $RCSfile: strhead.h,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2004/05/04 21:36:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2004/05/04 21:36:59 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_STRHEAD_H__
#define __LOCAL_STRHEAD_H__

//extern loff_t strllseek(struct file *file, loff_t off, int whence);
//extern ssize_t strread(struct file *file, char *buf, size_t len, loff_t *ppos);
//extern ssize_t strwrite(struct file *file, const char *buf, size_t len, loff_t *ppos);
//extern unsigned int strpoll(struct file *file, struct poll_table_struct *poll);
//extern int strioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
//extern int strmmap(struct file *filp, struct vm_area_struct *vma);
//extern int stropen(struct inode *inode, struct file *file);
//extern int strflush(struct file *file);
//extern int strclose(struct inode *inode, struct file *file);
//extern int strfasync(int fd, struct file *file, int on);
//extern ssize_t strreadv(struct file *file, const struct iovec *iov, unsigned long len,
//			loff_t *ppos);
//extern ssize_t strwritev(struct file *file, const struct iovec *iov, unsigned long count,
//			 loff_t *ppos);
//extern ssize_t strsendpage(struct file *file, struct page *page, int offset, size_t size,
//			   loff_t *ppos, int more);
extern int strgetpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int *bandp,
		      int *flagsp);
extern int strputpmsg(struct file *file, struct strbuf *ctlp, struct strbuf *datp, int band,
		      int flags);

extern struct file_operations strm_f_ops;

extern struct smodule_info str_minfo;	/* for strsysctl.c */

extern int autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag, int sflag,
		    cred_t *crp);

/* make SVR4.2 oflag from file flags and mode */
#define make_oflag(__f) \
	(((__f)->f_flags & ~O_ACCMODE) | \
	 ((__f)->f_mode & O_ACCMODE) | \
	 ((__f)->f_flags & FNDELAY ? (O_NONBLOCK | O_NDELAY) : 0))

extern int sth_init(void);
extern void sth_exit(void);

#endif				/* __LOCAL_STRHEAD_H__ */
