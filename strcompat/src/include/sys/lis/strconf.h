/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.10 2005/07/15 23:08:50 brian Exp $

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

 Last Modified $Date: 2005/07/15 23:08:50 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_LIS_STRCONF_H__
#define __SYS_LIS_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/15 23:08:50 $"

#ifndef __SYS_STRCONF_H__
#warning "Do not include sys/aix/strconf.h directly, include sys/strconf.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __depr
#if __GNUC__ >= 3
#define __depr __attribute__ ((__deprecated__))
#else
#define __depr
#endif
#endif

#ifndef __LIS_EXTERN_INLINE
#define __LIS_EXTERN_INLINE __depr extern __inline__
#endif				/* __LIS_EXTERN_INLINE */

#ifndef _LIS_SOURCE
#warning "_LIS_SOURCE not defined but LIS strconf.h included"
#define _LIS_SOURCE 1
#endif

#include <sys/strcompat/config.h>

#if defined(CONFIG_STREAMS_COMPAT_LIS) || defined(CONFIG_STREAMS_COMPAT_LIS_MODULE)

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

typedef struct lis_strrecvfd {
	union {
		struct file *fp;
		int fd;
	} f;
	uid_t uid;
	gid_t gid;
	struct {
		struct file *fp;
		struct stdata *hd;
	} r;
} strrecvfd_t;

#define LIS_MAXAPUSH	8
#define LIS_FMNAMESZ	8
struct lis_strapush {
	unsigned sap_cmd;
	unsigned long sap_major;
	unsigned long sap_minor;
	unsigned long sap_lastminor;
	unsigned int sap_npush;
	char sap_list[LIS_MAXAPUSH][LIS_FMNAMESZ + 1];
};

extern int lis_register_strdev(major_t major, struct streamtab *strtab, int nminor, const char *name);
extern int lis_unregister_strdev(major_t major);
extern modID_t lis_register_strmod(struct streamtab *strtab, const char *name);
extern int lis_unregister_strmod(struct streamtab *strtab);

extern int lis_apush_get(struct lis_strapush *ap) __depr;
extern int lis_apush_set(struct lis_strapush *ap) __depr;
extern int lis_apush_vml(struct str_list *slp) __depr;
#if 0
extern int lis_apushm(dev_t dev, const char *mods[]) __depr;
#endif

#if 0
extern int lis_check_q_magic(queue_t *q, char *file, int line) __depr;
#endif
extern int lis_clone_major(void) __depr;
#if 0
#if HAVE_KERNEL_FATTACH_SUPPORT
extern int lis_fattach(struct file *f, const char *path) __depr;
extern int lis_fdetach(const char *path) __depr;
#endif
extern void lis_fdetach_all(void) __depr;
extern void lis_fdetach_stream(struct stdata *head) __depr;
extern void lis_fifo_close_sync(struct inode *i, struct file *f) __depr;
extern int lis_fifo_open_sync(struct inode *i, struct file *f) __depr;
extern int lis_fifo_write_sync(struct inode *i, int written) __depr;
extern struct inode *lis_file_inode(struct file *f) __depr;
extern struct stdata *lis_file_str(struct file *f) __depr;
extern streamtab_t *lis_find_strdev(major_t major) __depr;
extern void lis_flush_print_buffer(void) __depr;
extern struct fmodsw *lis_fmod_sw __depr;
extern struct fmodsw *lis_fstr_sw __depr;
extern int lis_get_fifo(struct file **f) __depr;
extern void lis_free_passfp(mblk_t *mp) __depr;
extern void lis_freedb(mblk_t *bp, int free_hdr) __depr;
extern mblk_t *lis_get_passfp(void) __depr;
extern int lis_get_pipe(struct file **f0, struct file **f1) __depr;
extern int lis_ioc_fattach(struct file *f, char *path) __depr;
extern int lis_ioc_fdetach(char *path) __depr;
extern int lis_ioc_pipe(unsigned int *fildes) __depr;
extern struct inode *lis_old_inode(struct file *f, struct inode *i) __depr;
extern lis_atomic_t lis_open_cnt __depr;
#if HAVE_KERNEL_PIPE_SUPPORT
extern int lis_pipe(unsigned int *fd) __depr;
#endif
extern unsigned lis_poll_2_1(struct file *fp, poll_table * wait) __depr;
extern unsigned lis_poll_bits(struct stdata *hd) __depr;
extern char *lis_poll_events(short events) __depr;
extern char *lis_poll_file __depr;
extern void lis_print_queues(void) __depr;
extern void lis_print_stream(struct stdata *hd) __depr;
extern volatile unsigned long lis_queuerun_cnts[NR_CPUS] __depr;
extern lis_atomic_t lis_queues_running __depr;
extern int lis_recvfd(struct stdata *recvhd, strrecvfd_t * recv, struct file *fp) __depr;
extern volatile unsigned long lis_runq_cnts[NR_CPUS] __depr;
extern lis_atomic_t lis_runq_req_cnt __depr;
extern int lis_sendfd(struct stdata *sendhd, unsigned int fd, struct file *fp) __depr;
extern void lis_set_file_str(struct file *f, struct stdata *s) __depr;
extern void lis_setqsched(int can_call) __depr;
extern volatile unsigned long lis_setqsched_cnts[NR_CPUS] __depr;
extern volatile unsigned long lis_setqsched_isr_cnts[NR_CPUS] __depr;
extern lis_atomic_t lis_stdata_cnt __depr;
extern int lis_strclose(struct inode *i, struct file *f) __depr;
extern lis_atomic_t lis_strcount __depr;
extern int lis_strgetpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp, int *bandp,
			  int *flagsp, int doit) __depr;
extern int lis_strioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg) __depr;
extern int lis_stropen(struct inode *i, struct file *f) __depr;
extern int lis_strputpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp, int band,
			  int flags) __depr;
extern ssize_t lis_strread(struct file *fp, char *ubuff, size_t ulen, loff_t *op) __depr;
extern lis_atomic_t lis_strstats[24][4] __depr;
extern ssize_t lis_strwrite(struct file *fp, const char *ubuff, size_t ulen, loff_t *op) __depr;
extern int lis_valid_mod_list(struct str_list ml) __depr;
#endif

#elif defined(_LIS_SOURCE)
#warning "_LIS_SOURCE defined but not CONFIG_STREAMS_COMPAT_LIS"
#endif				/* CONFIG_STREAMS_COMPAT_LIS */

#endif				/* __SYS_LIS_STRCONF_H__ */
