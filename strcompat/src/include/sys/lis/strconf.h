/*****************************************************************************

 @(#) $Id: strconf.h,v 0.9.2.17 2006/12/08 05:08:11 brian Exp $

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

 Last Modified $Date: 2006/12/08 05:08:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strconf.h,v $
 Revision 0.9.2.17  2006/12/08 05:08:11  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.16  2006/11/03 10:39:19  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ifndef __SYS_LIS_STRCONF_H__
#define __SYS_LIS_STRCONF_H__

#ident "@(#) $RCSfile: strconf.h,v $ $Name:  $($Revision: 0.9.2.17 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

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

#undef _RP
#define _RP streamscall __depr

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __LIS_EXTERN_INLINE
#define __LIS_EXTERN_INLINE __EXTERN_INLINE streamscall
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

extern int streamscall lis_register_strdev(major_t major, struct streamtab *strtab, int nminor,
				   const char *name);
extern int streamscall lis_unregister_strdev(major_t major);
extern modID_t streamscall lis_register_strmod(struct streamtab *strtab, const char *name);
extern int streamscall lis_unregister_strmod(struct streamtab *strtab);

extern int streamscall lis_register_module_qlock_option(modID_t id, int qlock_option);
extern int streamscall lis_register_driver_qlock_option(major_t major, int qlock_option);

extern int _RP lis_apush_get(struct lis_strapush *ap);
extern int _RP lis_apush_set(struct lis_strapush *ap);
extern int _RP lis_apush_vml(struct str_list *slp);

#if 0
extern int _RP lis_apushm(dev_t dev, const char *mods[]);
#endif

#if 0
extern int _RP lis_check_q_magic(queue_t *q, char *file, int line);
#endif
extern int _RP lis_clone_major(void);

#if 0
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
extern int _RP lis_fattach(struct file *f, const char *path);
extern int _RP lis_fdetach(const char *path);
#endif
extern void _RP lis_fdetach_all(void);
extern void _RP lis_fdetach_stream(struct stdata *head);
extern void _RP lis_fifo_close_sync(struct inode *i, struct file *f);
extern int _RP lis_fifo_open_sync(struct inode *i, struct file *f);
extern int _RP lis_fifo_write_sync(struct inode *i, int written);
extern struct inode *_RP lis_file_inode(struct file *f);
extern struct stdata *_RP lis_file_str(struct file *f);
extern streamtab_t *_RP lis_find_strdev(major_t major);
extern void _RP lis_flush_print_buffer(void);
extern struct fmodsw *lis_fmod_sw __depr;
extern struct fmodsw *lis_fstr_sw __depr;
extern int _RP lis_get_fifo(struct file **f);
extern void _RP lis_free_passfp(mblk_t *mp);
extern void _RP lis_freedb(mblk_t *bp, int free_hdr);
extern mblk_t *_RP lis_get_passfp(void);
extern int _RP lis_get_pipe(struct file **f0, struct file **f1);
extern int _RP lis_ioc_fattach(struct file *f, char *path);
extern int _RP lis_ioc_fdetach(char *path);
extern int _RP lis_ioc_pipe(unsigned int *fildes);
extern struct inode *_RP lis_old_inode(struct file *f, struct inode *i);
extern lis_atomic_t lis_open_cnt __depr;

#ifdef HAVE_KERNEL_PIPE_SUPPORT
extern int _RP lis_pipe(unsigned int *fd);
#endif
extern unsigned _RP lis_poll_2_1(struct file *fp, poll_table * wait);
extern unsigned _RP lis_poll_bits(struct stdata *hd);
extern char *_RP lis_poll_events(short events);
extern char *_RP lis_poll_file __depr;
extern void _RP lis_print_queues(void);
extern void _RP lis_print_stream(struct stdata *hd);
extern volatile unsigned long _RP lis_queuerun_cnts[NR_CPUS] __depr;
extern lis_atomic_t _RP lis_queues_running __depr;
extern int _RP lis_recvfd(struct stdata *recvhd, strrecvfd_t * recv, struct file *fp);
extern volatile unsigned long _RP lis_runq_cnts[NR_CPUS] __depr;
extern lis_atomic_t _RP lis_runq_req_cnt __depr;
extern int _RP lis_sendfd(struct stdata *sendhd, unsigned int fd, struct file *fp);
extern void _RP lis_set_file_str(struct file *f, struct stdata *s);
extern void _RP lis_setqsched(int can_call);
extern volatile unsigned long _RP lis_setqsched_cnts[NR_CPUS] __depr;
extern volatile unsigned long _RP lis_setqsched_isr_cnts[NR_CPUS] __depr;
extern lis_atomic_t _RP lis_stdata_cnt __depr;
extern int _RP lis_strclose(struct inode *i, struct file *f);
extern lis_atomic_t _RP lis_strcount __depr;
extern int _RP lis_strgetpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp,
			      int *bandp, int *flagsp, int doit);
extern int _RP lis_strioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg);
extern int _RP lis_stropen(struct inode *i, struct file *f);
extern int _RP lis_strputpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp,
			      int band, int flags);
extern ssize_t _RP lis_strread(struct file *fp, char *ubuff, size_t ulen, loff_t *op);
extern lis_atomic_t _RP lis_strstats[24][4] __depr;
extern ssize_t _RP lis_strwrite(struct file *fp, const char *ubuff, size_t ulen, loff_t *op);
extern int _RP lis_valid_mod_list(struct str_list ml);
#endif

#else
#ifdef _LIS_SOURCE
#warning "_LIS_SOURCE defined but not CONFIG_STREAMS_COMPAT_LIS"
#endif
#endif				/* CONFIG_STREAMS_COMPAT_LIS */

#endif				/* __SYS_LIS_STRCONF_H__ */
