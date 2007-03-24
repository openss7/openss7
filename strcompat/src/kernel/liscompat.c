/*****************************************************************************

 @(#) $RCSfile: liscompat.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2007/03/02 10:04:06 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/03/02 10:04:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: liscompat.c,v $
 Revision 0.9.2.42  2007/03/02 10:04:06  brian
 - updates to common build process and versions for all exported symbols

 Revision 0.9.2.41  2006/11/03 10:39:27  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ident "@(#) $RCSfile: liscompat.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2007/03/02 10:04:06 $"

static char const ident[] = "$RCSfile: liscompat.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2007/03/02 10:04:06 $";

/* 
 *  This is my solution for those who don't want to inline GPL'ed functions or
 *  who don't use optimizations when compiling or specifies
 *  -fnoinline-functions or something of the like.  This file implements all
 *  of the extern inlines from the header files by just including the header
 *  files with the functions declared 'inline' instead of 'extern inline'.
 *
 *  There are implemented here in a separate object, out of the way of the
 *  modules that don't use them.
 */

#define __LIS_EXTERN_INLINE __inline__ streamscall __unlikely
#define __LIS_NO_MACROS
#define __depr

#define _LIS_SOURCE

#include "sys/os7/compat.h"

#include <asm/dma.h>		/* for request_dma and friends */

#define LISCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LISCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define LISCOMP_REVISION	"LfS $RCSfile: liscompat.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2007/03/02 10:04:06 $"
#define LISCOMP_DEVICE		"LiS 2.16 and 2.18 Compatibility"
#define LISCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define LISCOMP_LICENSE		"GPL"
#define LISCOMP_BANNER		LISCOMP_DESCRIP		"\n" \
				LISCOMP_COPYRIGHT	"\n" \
				LISCOMP_REVISION	"\n" \
				LISCOMP_DEVICE		"\n" \
				LISCOMP_CONTACT		"\n"
#define LISCOMP_SPLASH		LISCOMP_DEVICE		" - " \
				LISCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
MODULE_AUTHOR(LISCOMP_CONTACT);
MODULE_DESCRIPTION(LISCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(LISCOMP_DEVICE);
MODULE_LICENSE(LISCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-liscompat");
#endif
#endif

#ifndef CONFIG_STREAMS_CLONE_MAJOR
#define CONFIG_STREAMS_CLONE_MAJOR 54
//#error "CONFIG_STREAMS_CLONE_MAJOR must be defined."
#endif

/* 
 *  This file provides some level of binary compatibility for LiS exported
 *  symbols.  It is only linked into the kernel when compiled with
 *  CONFIG_STREAMS_COMPAT_LIS set.
 */
static char *
dont_use_this_function(void)
{
	swerr();
	return "(DON'T USE THIS FUNCTION!)";
}

#ifdef _DEBUG
#ifdef LIS_DEPRECARTED_FUNCTIONS
static void
warn_kern_violation(char *func, char *file, int line)
{
	printk(KERN_WARNING "%s: export violation: %s +%d\n", func, file, line);
}
#endif
static void
warn_kern_wrapper(const char *func, const char *repl)
{
	printk(KERN_DEBUG "%s: don't call this function: use %s instead\n", func, repl);
}
static void
warnf_kern_wrapper(const char *func, const char *repl, const char *f, const int l)
{
	printk(KERN_DEBUG "%s [%s +%d]: don't call this function: use %s instead\n", func, f, l,
	       repl);
}

#define WARN(__exp) \
	({ warn_kern_wrapper(__FUNCTION__,#__exp); __exp; })

#define WARNF(__exp, __f, __l) \
	({ warnf_kern_wrapper(__FUNCTION__,#__exp, __f, __l); __exp; })

#else				/* _DEBUG */

#define WARN(__exp) \
	({ __exp; })

#define WARNF(__exp, __f, __l) \
	({ __exp; })

#endif				/* _DEBUG */

/* 
 *  Here are the lis definitions...
 */
__LIS_EXTERN_INLINE int _RP lis_adjmsg(mblk_t *mp, int length);

EXPORT_SYMBOL(lis_adjmsg);
__LIS_EXTERN_INLINE struct msgb *_RP lis_allocb(int size, unsigned int priority, char *file_name,
					    int line_nr);
EXPORT_SYMBOL(lis_allocb);
__LIS_EXTERN_INLINE struct msgb *_RP lis_allocb_physreq(int size, unsigned int priority,
						    void *physreq_ptr, char *file_name,
						    int line_nr);
EXPORT_SYMBOL(lis_allocb_physreq);
#ifdef LIS_DEPRECARTED_FUNCTIONS
__LIS_EXTERN_INLINE queue_t *_RP lis_allocq(const char *name);

EXPORT_SYMBOL(lis_allocq);
#endif
__LIS_EXTERN_INLINE int _RP lis_appq(queue_t *q, mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL(lis_appq);
__LIS_EXTERN_INLINE queue_t *_RP lis_backq(queue_t *q);

EXPORT_SYMBOL(lis_backq);
__LIS_EXTERN_INLINE queue_t *_RP lis_backq_fcn(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_backq_fcn);
__LIS_EXTERN_INLINE int _RP lis_bcanput(queue_t *q, unsigned char band);

EXPORT_SYMBOL(lis_bcanput);
__LIS_EXTERN_INLINE int _RP lis_bcanputnext(queue_t *q, unsigned char band);

EXPORT_SYMBOL(lis_bcanputnext);
__LIS_EXTERN_INLINE int _RP lis_bcanputnext_anyband(queue_t *q);

EXPORT_SYMBOL(lis_bcanputnext_anyband);
__LIS_EXTERN_INLINE int _RP lis_bufcall(unsigned size, int priority, void streamscall (*function) (long), long arg);

EXPORT_SYMBOL(lis_bufcall);
__LIS_EXTERN_INLINE mblk_t *_RP lis_copyb(mblk_t *mp);

EXPORT_SYMBOL(lis_copyb);
__LIS_EXTERN_INLINE mblk_t *_RP lis_copymsg(mblk_t *mp);

EXPORT_SYMBOL(lis_copymsg);
#ifdef LIS_DEPRECARTED_FUNCTIONS
__LIS_EXTERN_INLINE int _RP lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len);

EXPORT_SYMBOL(lis_copyin);
__LIS_EXTERN_INLINE int _RP lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len);

EXPORT_SYMBOL(lis_copyout);
#endif
__LIS_EXTERN_INLINE mblk_t *_RP lis_dupb(mblk_t *mp);

EXPORT_SYMBOL(lis_dupb);
__LIS_EXTERN_INLINE mblk_t *_RP lis_dupmsg(mblk_t *mp);

EXPORT_SYMBOL(lis_dupmsg);
__LIS_EXTERN_INLINE mblk_t *_RP lis_esballoc(unsigned char *base, int size, int priority,
					 frtn_t *freeinfo, char *file_name, int line_nr);
EXPORT_SYMBOL(lis_esballoc);
__LIS_EXTERN_INLINE int _RP lis_esbbcall(int priority, void streamscall (*function) (long), long arg);

EXPORT_SYMBOL(lis_esbbcall);
__LIS_EXTERN_INLINE void _RP lis_flushband(queue_t *q, unsigned char band, int flag);

EXPORT_SYMBOL(lis_flushband);
__LIS_EXTERN_INLINE void _RP lis_flushq(queue_t *q, int flag);

EXPORT_SYMBOL(lis_flushq);
__LIS_EXTERN_INLINE void _RP lis_freeb(mblk_t *bp);

EXPORT_SYMBOL(lis_freeb);
__LIS_EXTERN_INLINE void _RP lis_freemsg(mblk_t *mp);

EXPORT_SYMBOL(lis_freemsg);
__LIS_EXTERN_INLINE void _RP lis_freezestr(queue_t *q);

EXPORT_SYMBOL(lis_freezestr);
__LIS_EXTERN_INLINE mblk_t *_RP lis_getq(queue_t *q);

EXPORT_SYMBOL(lis_getq);
#ifdef LIS_DEPRECARTED_FUNCTIONS
__LIS_EXTERN_INLINE void _RP lis_freeq(queue_t *q);

EXPORT_SYMBOL(lis_freeq);
#endif
__LIS_EXTERN_INLINE int _RP lis_insq(queue_t *q, mblk_t *emp, mblk_t *mp);

EXPORT_SYMBOL(lis_insq);
__LIS_EXTERN_INLINE void _RP lis_linkb(mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL(lis_linkb);
__LIS_EXTERN_INLINE int _RP lis_max(int a, int b);

EXPORT_SYMBOL(lis_max);
__LIS_EXTERN_INLINE int _RP lis_min(int a, int b);

EXPORT_SYMBOL(lis_min);
__LIS_EXTERN_INLINE int _RP lis_msgdsize(mblk_t *mp);

EXPORT_SYMBOL(lis_msgdsize);
__LIS_EXTERN_INLINE mblk_t *_RP lis_msgpullup(mblk_t *mp, int len);

EXPORT_SYMBOL(lis_msgpullup);
__LIS_EXTERN_INLINE int _RP lis_msgsize(mblk_t *mp);

EXPORT_SYMBOL(lis_msgsize);
__LIS_EXTERN_INLINE int _RP lis_pullupmsg(mblk_t *mp, int length);

EXPORT_SYMBOL(lis_pullupmsg);
__LIS_EXTERN_INLINE int _RP lis_putbq(queue_t *q, mblk_t *mp);

EXPORT_SYMBOL(lis_putbq);
__LIS_EXTERN_INLINE int _RP lis_putctl(queue_t *q, int type, char *file_name, int line_nr);

EXPORT_SYMBOL(lis_putctl);
__LIS_EXTERN_INLINE int _RP lis_putctl1(queue_t *q, int type, int param, char *file_name, int line_nr);

EXPORT_SYMBOL(lis_putctl1);
__LIS_EXTERN_INLINE int _RP lis_putnextctl(queue_t *q, int type, char *file_name, int line_nr);

EXPORT_SYMBOL(lis_putnextctl);
__LIS_EXTERN_INLINE int _RP lis_putnextctl1(queue_t *q, int type, int param, char *file_name,
					int line_nr);
EXPORT_SYMBOL(lis_putnextctl1);
__LIS_EXTERN_INLINE int _RP lis_putq(queue_t *q, mblk_t *mp);

EXPORT_SYMBOL(lis_putq);
__LIS_EXTERN_INLINE int _RP lis_qcountstrm(queue_t *q);

EXPORT_SYMBOL(lis_qcountstrm);
#ifdef LIS_DEPRECARTED_FUNCTIONS
__LIS_EXTERN_INLINE void _RP lis_qdetach(queue_t *q, int do_close, int flag, cred_t *creds);

EXPORT_SYMBOL(lis_qdetach);
#endif
__LIS_EXTERN_INLINE void _RP lis_qenable(queue_t *q);

EXPORT_SYMBOL(lis_qenable);
__LIS_EXTERN_INLINE void _RP lis_qprocsoff(queue_t *rdq);

EXPORT_SYMBOL(lis_qprocsoff);
__LIS_EXTERN_INLINE void _RP lis_qprocson(queue_t *rdq);

EXPORT_SYMBOL(lis_qprocson);
__LIS_EXTERN_INLINE int _RP lis_qsize(queue_t *q);

EXPORT_SYMBOL(lis_qsize);
__LIS_EXTERN_INLINE mblk_t *_RP lis_rmvb(mblk_t *mp, mblk_t *bp);

EXPORT_SYMBOL(lis_rmvb);
__LIS_EXTERN_INLINE void _RP lis_rmvq(queue_t *q, mblk_t *mp);

EXPORT_SYMBOL(lis_rmvq);
__LIS_EXTERN_INLINE queue_t *_RP lis_safe_OTHERQ(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_safe_OTHERQ);
__LIS_EXTERN_INLINE queue_t *_RP lis_safe_RD(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_safe_RD);
__LIS_EXTERN_INLINE int _RP lis_safe_SAMESTR(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_safe_SAMESTR);
__LIS_EXTERN_INLINE queue_t *_RP lis_safe_WR(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_safe_WR);
__LIS_EXTERN_INLINE int _RP lis_safe_canenable(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_safe_canenable);
__LIS_EXTERN_INLINE void _RP lis_safe_enableok(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_safe_enableok);
__LIS_EXTERN_INLINE void _RP lis_safe_noenable(queue_t *q, char *f, int l);

EXPORT_SYMBOL(lis_safe_noenable);
__LIS_EXTERN_INLINE void _RP lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l);

EXPORT_SYMBOL(lis_safe_putmsg);
__LIS_EXTERN_INLINE void _RP lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l);

EXPORT_SYMBOL(lis_safe_putnext);
__LIS_EXTERN_INLINE void _RP lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l);

EXPORT_SYMBOL(lis_safe_qreply);
__LIS_EXTERN_INLINE void _RP lis_setq(queue_t *q, struct qinit *rinit, struct qinit *winit);

EXPORT_SYMBOL(lis_setq);
__LIS_EXTERN_INLINE int _RP lis_strqget(queue_t *q, qfields_t what, unsigned char band, long *val);

EXPORT_SYMBOL(lis_strqget);
__LIS_EXTERN_INLINE int _RP lis_strqset(queue_t *q, qfields_t what, unsigned char band, long val);

EXPORT_SYMBOL(lis_strqset);
__LIS_EXTERN_INLINE int _RP lis_testb(int size, unsigned int priority);

EXPORT_SYMBOL(lis_testb);
__LIS_EXTERN_INLINE toid_t _RP lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks,
					   char *file_name, int line_nr);
EXPORT_SYMBOL(lis_timeout_fcn);
__LIS_EXTERN_INLINE void _RP lis_unbufcall(int bcid);

EXPORT_SYMBOL(lis_unbufcall);
__LIS_EXTERN_INLINE void _RP lis_unfreezestr(queue_t *q);

EXPORT_SYMBOL(lis_unfreezestr);
__LIS_EXTERN_INLINE mblk_t _RP *lis_unlinkb(mblk_t *mp);

EXPORT_SYMBOL(lis_unlinkb);
__LIS_EXTERN_INLINE toid_t _RP lis_untimeout(toid_t id);

EXPORT_SYMBOL(lis_untimeout);
__LIS_EXTERN_INLINE int _RP lis_xmsgsize(mblk_t *mp);

EXPORT_SYMBOL(lis_xmsgsize);

#ifdef LIS_DEPRECARTED_FUNCTIONS
char *lis_poll_file = "<linux/poll.h>";

EXPORT_SYMBOL(lis_poll_file);
#endif
char *lis_stropts_file = "<linux/stropts.h>";

EXPORT_SYMBOL(lis_stropts_file);
char lis_date[] = "2006/10/30 08:00:04";

EXPORT_SYMBOL(lis_date);
char lis_kernel_version[] = UTS_RELEASE;

EXPORT_SYMBOL(lis_kernel_version);
char lis_version[] = "2.18.4";

EXPORT_SYMBOL(lis_version);
#ifdef LIS_DEPRECARTED_FUNCTIONS
char *
lis_poll_events(short events)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL(lis_poll_events);
const char *
lis_maj_min_name(stdata_t *head)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL(lis_maj_min_name);
#endif
_RP const char *
lis_msg_type_name(mblk_t *mp)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL(lis_msg_type_name);
_RP const char *
lis_queue_name(queue_t *q)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL(lis_queue_name);
_RP const char *
lis_strm_name(stdata_t *head)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL(lis_strm_name);
_RP const char *
lis_strm_name_from_queue(queue_t *q)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL(lis_strm_name_from_queue);
_RP int
lis_apush_get(struct lis_strapush *ap)
{
	struct strapush sap;
	int k, err;
	int maxapush = MAXAPUSH > LIS_MAXAPUSH ? LIS_MAXAPUSH : MAXAPUSH;
	int fmnamesz = FMNAMESZ > LIS_FMNAMESZ ? LIS_FMNAMESZ : FMNAMESZ;

	sap.sap_cmd = ap->sap_cmd;
	sap.sap_major = ap->sap_major;
	sap.sap_minor = ap->sap_minor;
	if ((err = apush_get(&sap)) != 0)
		return err;
	if (sap.sap_npush > maxapush)
		return -EIO;
	ap->sap_cmd = sap.sap_cmd;
	ap->sap_major = sap.sap_major;
	ap->sap_minor = sap.sap_minor;
	ap->sap_lastminor = sap.sap_lastminor;
	ap->sap_npush = sap.sap_npush;
	for (k = 0; k < maxapush; k++)
		strncpy(ap->sap_list[k], sap.sap_list[k], fmnamesz);
	return 0;
}

EXPORT_SYMBOL(lis_apush_get);
_RP int
lis_apush_set(struct lis_strapush *ap)
{
	struct strapush sap;
	int k;
	int maxapush = MAXAPUSH > LIS_MAXAPUSH ? LIS_MAXAPUSH : MAXAPUSH;
	int fmnamesz = FMNAMESZ > LIS_FMNAMESZ ? LIS_FMNAMESZ : FMNAMESZ;

	sap.sap_cmd = ap->sap_cmd;
	sap.sap_major = ap->sap_major;
	sap.sap_minor = ap->sap_minor;
	sap.sap_lastminor = ap->sap_lastminor;
	sap.sap_npush = ap->sap_npush;
	if (sap.sap_npush < 1 || sap.sap_npush > maxapush)
		return -EINVAL;
	for (k = 0; k < maxapush; k++)
		strncpy(sap.sap_list[k], ap->sap_list[k], fmnamesz);
	return apush_set(&sap);
}

EXPORT_SYMBOL(lis_apush_set);
_RP int
lis_apush_vml(struct str_list *slp)
{
	return apush_vml(slp);
}

EXPORT_SYMBOL(lis_apush_vml);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP int
lis_apushm(dev_t dev, const char *mods[])
{
	struct strapush sap;
	int err;

	sap.sap_major = getmajor(dev);
	sap.sap_minor = getminor(dev);
	if ((err = apush_get(&sap)) == 0)
		for (; err < MAXAPUSH; err++)
			mods[err] = sap.sap_list[err];
	return err;
}

EXPORT_SYMBOL(lis_apushm);
_RP int
lis_check_guard(void *ptr, char *msg)
{
	return 1;
}

EXPORT_SYMBOL(lis_check_guard);
_RP int
lis_check_mem(void)
{
	return 1;
}

EXPORT_SYMBOL(lis_check_mem);
_RP int
lis_check_q_magic(queue_t *q, char *file, int line)
{
	return 1;
}

EXPORT_SYMBOL(lis_check_q_magic);
#endif
_RP int
lis_clone_major(void)
{
	return CONFIG_STREAMS_CLONE_MAJOR;
}

EXPORT_SYMBOL(lis_clone_major);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP int
lis_doclose(struct inode *i, struct file *f, stdata_t *head, cred_t *creds)
{
	return strm_f_ops.release(i, f);
}

EXPORT_SYMBOL(lis_doclose);
_RP int
lis_fifo_open_sync(struct inode *i, struct file *f)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL(lis_fifo_open_sync);
_RP int
lis_fifo_write_sync(struct inode *i, int written)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL(lis_fifo_write_sync);
_RP int
lis_get_fifo(struct file **f)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL(lis_get_fifo);
_RP int
lis_get_pipe(struct file **f0, struct file **f1)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL(lis_get_pipe);
_RP int
lis_ioc_fattach(struct file *f, char *path)
{
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
	return lis_fattach(f, path);
#else
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL(lis_ioc_fattach);
_RP int
lis_ioc_fdetach(char *path)
{
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
	return lis_fdetach(path);
#else
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL(lis_ioc_fdetach);
_RP int
lis_ioc_pipe(unsigned int *fildes)
{
#ifdef HAVE_KERNEL_PIPE_SUPPORT
	return lis_pipe(fildes);
#else
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL(lis_ioc_pipe);
#endif

int lis_major = 0;

EXPORT_SYMBOL(lis_major);
_RP int
lis_own_spl(void)
{
	return 1;
}

EXPORT_SYMBOL(lis_own_spl);

#if defined CONFIG_STREAMS_NOIRQ || defined _TEST

#define spin_lock_str(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)
#define write_lock_str(__lkp, __flags) \
	do { (void)__flags; write_lock_bh(__lkp); } while (0)
#define write_unlock_str(__lkp, __flags) \
	do { (void)__flags; write_unlock_bh(__lkp); } while (0)
#define read_lock_str(__lkp, __flags) \
	do { (void)__flags; read_lock_bh(__lkp); } while (0)
#define read_unlock_str(__lkp, __flags) \
	do { (void)__flags; read_unlock_bh(__lkp); } while (0)

#else

#define spin_lock_str(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)
#define write_lock_str(__lkp, __flags) \
	write_lock_irqsave(__lkp, __flags)
#define write_unlock_str(__lkp, __flags) \
	write_unlock_irqrestore(__lkp, __flags)
#define read_lock_str(__lkp, __flags) \
	read_lock_irqsave(__lkp, __flags)
#define read_unlock_str(__lkp, __flags) \
	read_unlock_irqrestore(__lkp, __flags)

#endif

_RP int
lis_printk(const char *fmt, ...)
{
	static spinlock_t printk_lock = SPIN_LOCK_UNLOCKED;
	static char printk_buf[1024];
	unsigned long flags;
	va_list args;
	int ret;

	spin_lock_str(&printk_lock, flags);
	va_start(args, fmt);
	vsnprintf(printk_buf, sizeof(printk_buf), fmt, args);
	va_end(args);
	ret = WARN(printk("%s", printk_buf));
	spin_unlock_str(&printk_lock, flags);
	return ret;
}

EXPORT_SYMBOL(lis_printk);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP int
lis_strclose(struct inode *i, struct file *f)
{
	return strm_f_ops.release(i, f);
}

EXPORT_SYMBOL(lis_strclose);
_RP int
lis_strgetpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp, int *bandp,
	       int *flagsp, int doit)
{
	return strgetpmsg(fp, ctlp, datp, bandp, flagsp);
}

EXPORT_SYMBOL(lis_strgetpmsg);
_RP int
lis_strioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
{
	return strm_f_ops.ioctl(i, f, cmd, arg);
}

EXPORT_SYMBOL(lis_strioctl);
_RP int
lis_stropen(struct inode *i, struct file *f)
{
	return strm_f_ops.open(i, f);
}

EXPORT_SYMBOL(lis_stropen);
_RP int
lis_strputpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp, int band, int flags)
{
	return strputpmsg(fp, ctlp, datp, band, flags);
}

EXPORT_SYMBOL(lis_strputpmsg);
_RP int
lis_valid_mod_list(struct str_list ml)
{
	return apush_vml(&ml);
}

EXPORT_SYMBOL(lis_valid_mod_list);

long lis_max_mem = 0;

EXPORT_SYMBOL(lis_max_mem);
#endif
_RP long
lis_milli_to_ticks(long milli_sec)
{
	return WARN(milli_sec / (1000 / HZ));
}

EXPORT_SYMBOL(lis_milli_to_ticks);
_RP pid_t
lis_thread_start(int (*fcn) (void *), void *arg, const char *name)
{
	return 0;
}

EXPORT_SYMBOL(lis_thread_start);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP ssize_t
lis_strread(struct file *fp, char *ubuff, size_t ulen, loff_t *op)
{
	return strm_f_ops.read(fp, ubuff, ulen, op);
}

EXPORT_SYMBOL(lis_strread);
_RP ssize_t
lis_strwrite(struct file *fp, const char *ubuff, size_t ulen, loff_t *op)
{
	return strm_f_ops.write(fp, ubuff, ulen, op);
}

EXPORT_SYMBOL(lis_strwrite);
_RP streamtab_t *
lis_find_strdev(major_t major)
{
	struct streamtab *st = NULL;

#ifdef MAX_CHRDEV
	if (major < MAX_CHRDEV)
#else
	if (major == MAJOR(MKDEV(major, 0)))
#endif
	{
		struct cdevsw *cdev;

		if ((cdev = sdev_get(major)) != NULL) {
			printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
			st = cdev->d_str;
			printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
			sdev_put(cdev);
		}
	}
	return st;
}

EXPORT_SYMBOL(lis_find_strdev);
struct fmodsw *lis_fmod_sw = NULL;

EXPORT_SYMBOL(lis_fmod_sw);
struct fmodsw *lis_fstr_sw = NULL;

EXPORT_SYMBOL(lis_fstr_sw);
_RP struct inode *
lis_old_inode(struct file *f, struct inode *i)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL(lis_old_inode);
_RP unsigned
lis_poll_2_1(struct file *fp, poll_table * wait)
{
	return strm_f_ops.poll(fp, wait);
}

EXPORT_SYMBOL(lis_poll_2_1);
_RP unsigned
lis_poll_bits(stdata_t *hd)
{
	unsigned int mask = 0;
	struct stdata *sd = (typeof(sd)) hd;

	if (test_bit(STRDERR_BIT, &sd->sd_flag) || test_bit(STWRERR_BIT, &sd->sd_flag))
		mask |= POLLERR;
	if (test_bit(STRHUP_BIT, &sd->sd_flag))
		mask |= POLLHUP;
	if (test_bit(STRPRI_BIT, &sd->sd_flag))
		mask |= POLLPRI;
	if (test_bit(STRMSIG_BIT, &sd->sd_flag))
		mask |= POLLMSG;
	if (bcanget(sd->sd_rq, 0))
		mask |= POLLIN | POLLRDNORM;
	if (bcanget(sd->sd_rq, ANYBAND))
		mask |= POLLIN | POLLRDBAND;
	if (bcanput(sd->sd_wq, 0))
		mask |= POLLOUT | POLLWRNORM;
	if (bcanput(sd->sd_wq, ANYBAND))
		mask |= POLLOUT | POLLWRBAND;
	return mask;
}

EXPORT_SYMBOL(lis_poll_bits);
#endif
unsigned long lis_debug_mask2 = 0;

EXPORT_SYMBOL(lis_debug_mask2);
unsigned long lis_debug_mask = 0;

EXPORT_SYMBOL(lis_debug_mask);
#ifdef HAVE_KFUNC_VMALLOC
_RP void *
lis_vmalloc(unsigned long size)
{
	return WARN(vmalloc(size));
}

EXPORT_SYMBOL(lis_vmalloc);
#endif
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_bprintf(char *fmt, ...)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_bprintf);
#endif
_RP void
lis_cmn_err(int err_lvl, char *fmt, ...)
{
	extern void streams_fastcall vcmn_err(int err_lvl, const char *fmt, va_list args);
	va_list args;

	va_start(args, fmt);
	vcmn_err(err_lvl, fmt, args);
	va_end(args);
}

EXPORT_SYMBOL(lis_cmn_err);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_dobufcall(int cpu_id)
{
	/* bufcalls will be processed by the LfS executive when necessary. */
	return;
}

EXPORT_SYMBOL(lis_dobufcall);
void
_RP lis_enable_intr(struct streamtab *strtab, int major, const char *name)
{
	/* don't do this */
	swerr();
	return;
}

EXPORT_SYMBOL(lis_enable_intr);
void
_RP lis_fifo_close_sync(struct inode *i, struct file *f)
{
	/* don't do this */
	swerr();
	return;
}

EXPORT_SYMBOL(lis_fifo_close_sync);
void
_RP lis_flush_print_buffer(void)
{
	/* print buffers will be processed by kernel. */
	return;
}

EXPORT_SYMBOL(lis_flush_print_buffer);
#endif
_RP void
lis_free(void *ptr, char *file_name, int line_nr)
{
	kfree(ptr);
}

EXPORT_SYMBOL(lis_free);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_freedb(mblk_t *bp, int free_hdr)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_freedb);
_RP void
lis_init_bufcall(void)
{
	return;
}

EXPORT_SYMBOL(lis_init_bufcall);
_RP void
lis_mark_mem(void *ptr, const char *file_name, int line_nr)
{
	return;
}

EXPORT_SYMBOL(lis_mark_mem);
#endif
_RP void
lis_print_block(void *ptr)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_block);
_RP void
lis_print_data(mblk_t *mp, int opt, int cont)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_data);
_RP void
lis_print_mem(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_mem);
_RP void
lis_print_msg(mblk_t *mp, const char *prefix, int opt)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_msg);
_RP void
lis_print_queue(queue_t *q)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_queue);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_print_queues(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_queues);
_RP void
lis_print_spl_track(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_spl_track);
_RP void
lis_print_stream(stdata_t *hd)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_print_stream);
_RP void
lis_set_file_str(struct file *f, struct stdata *s)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_set_file_str);
_RP void
lis_setqsched(int can_call)
{
	WARN(setqsched());
}

EXPORT_SYMBOL(lis_setqsched);
#endif
_RP void
lis_spl0_fcn(char *file, int line)
{
	WARNF(local_irq_enable(), file, line);
}

EXPORT_SYMBOL(lis_spl0_fcn);
#ifdef LIS_DEPRECARTED_FUNCTIONS
volatile unsigned long lis_queuerun_cnts[NR_CPUS] = { 0, };

EXPORT_SYMBOL(lis_queuerun_cnts);
volatile unsigned long lis_runq_cnts[NR_CPUS] = { 0, };

EXPORT_SYMBOL(lis_runq_cnts);
volatile unsigned long lis_setqsched_cnts[NR_CPUS] = { 0, };

EXPORT_SYMBOL(lis_setqsched_cnts);
volatile unsigned long lis_setqsched_isr_cnts[NR_CPUS] = { 0, };

EXPORT_SYMBOL(lis_setqsched_isr_cnts);
#endif

#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP mblk_t *
lis_get_passfp(void)
{
	return allocb(sizeof(struct strrecvfd), BPRI_HI);
}

EXPORT_SYMBOL(lis_get_passfp);
_RP void
lis_free_passfp(mblk_t *mp)
{
	freemsg(mp);
}

EXPORT_SYMBOL(lis_free_passfp);
_RP int
lis_recvfd(stdata_t *recvhd, strrecvfd_t * recv, struct file *fp)
{
	return (-EOPNOTSUPP);
}

EXPORT_SYMBOL(lis_recvfd);
_RP int
lis_sendfd(stdata_t *sendhd, unsigned int fd, struct file *fp)
{
	return (-EOPNOTSUPP);
}

EXPORT_SYMBOL(lis_sendfd);
#endif

/* 
 *  This is the absurd list.  If Dave thinks that LiS can change Linux GPL to
 *  LGPL in this fashion, he's nuts...  They are all bugged out so that you
 *  know when a module is using one and can change it.
 */
#ifdef CONFIG_PCI
#ifdef HAVE_KFUNC_PCI_DAC_PAGE_TO_DMA
_RP dma64_addr_t
lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev, struct page *page,
			     unsigned long offset, int direction)
{
	return WARN(pci_dac_page_to_dma(pdev, page, offset, direction));
}

EXPORT_SYMBOL(lis_osif_pci_dac_page_to_dma);
#endif				/* HAVE_KFUNC_PCI_DAC_PAGE_TO_DMA */
_RP dma_addr_t
lis_osif_pci_map_page(struct pci_dev *hwdev, struct page *page, unsigned long offset,
		      size_t size, int direction)
{
	return WARN(pci_map_page(hwdev, page, offset, size, direction));
}

EXPORT_SYMBOL(lis_osif_pci_map_page);
_RP dma_addr_t
lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size, int direction)
{
	return WARN(pci_map_single(hwdev, ptr, size, direction));
}

EXPORT_SYMBOL(lis_osif_pci_map_single);
_RP dma_addr_t
lis_osif_sg_dma_address(struct scatterlist *sg)
{
	return WARN(sg_dma_address(sg));
}

EXPORT_SYMBOL(lis_osif_sg_dma_address);
#endif
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP int
lis_can_unload(void)
{
	return 1;
}

EXPORT_SYMBOL(lis_can_unload);
#ifdef HAVE_KFUNC_CHECK_REGION
_RP int
lis_check_region(unsigned int from, unsigned int extent)
{
	return WARN(check_region(from, extent));
}

EXPORT_SYMBOL(lis_check_region);
#endif
_RP int
lis_check_umem(struct file *fp, int rd_wr_fcn, const void *usr_addr, int lgth)
{
	return WARN(verify_area(rd_wr_fcn, usr_addr, lgth));
}

EXPORT_SYMBOL(lis_check_umem);
_RP int
lis_del_timer(struct timer_list *timer)
{
	return WARN(del_timer(timer));
}

EXPORT_SYMBOL(lis_del_timer);
#endif

/* This one is just plain silly. */
_RP int
lis_getint(unsigned char **p)
{
	int retval = *((int *) (*p));
	p += sizeof(int);
	return retval;
}

EXPORT_SYMBOL(lis_getint);
_RP int
lis_kernel_down(struct semaphore *sem)
{
	return WARN(down_interruptible(sem));
}

EXPORT_SYMBOL(lis_kernel_down);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP int
lis_kill_pg(int pgrp, int sig, int priv)
{
	return WARN(kill_pg(pgrp, sig, priv));
}

EXPORT_SYMBOL(lis_kill_pg);
_RP int
lis_kill_proc(int pid, int sig, int priv)
{
	return WARN(kill_proc(pid, sig, priv));
}

EXPORT_SYMBOL(lis_kill_proc);
_RP int
lis_loadable_load(const char *name)
{
	return WARN(request_module(name));
}

EXPORT_SYMBOL(lis_loadable_load);
#endif

int lis_num_cpus = NR_CPUS;

EXPORT_SYMBOL(lis_num_cpus);
#ifdef CONFIG_PCI
_RP int
lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_dac_dma_supported(hwdev, mask));
}

EXPORT_SYMBOL(lis_osif_pci_dac_dma_supported);
#ifdef HAVE_KFUNC_PCI_DAC_SET_DMA_MASK
_RP int
lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_dac_set_dma_mask(hwdev, mask));
}

EXPORT_SYMBOL(lis_osif_pci_dac_set_dma_mask);
#endif
_RP int
lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_dma_supported(hwdev, mask));
}

EXPORT_SYMBOL(lis_osif_pci_dma_supported);
_RP int
lis_osif_pci_enable_device(struct pci_dev *dev)
{
	return WARN(pci_enable_device(dev));
}

EXPORT_SYMBOL(lis_osif_pci_enable_device);
_RP int
lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{
	return WARN(pci_map_sg(hwdev, sg, nents, direction));
}

EXPORT_SYMBOL(lis_osif_pci_map_sg);
_RP int
lis_osif_pci_module_init(void *p)
{
	return WARN(pci_module_init(p));
}

EXPORT_SYMBOL(lis_osif_pci_module_init);
_RP int
lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val)
{
	return WARN(pci_read_config_byte(dev, where, val));
}

EXPORT_SYMBOL(lis_osif_pci_read_config_byte);
_RP int
lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val)
{
	return WARN(pci_read_config_dword(dev, where, val));
}

EXPORT_SYMBOL(lis_osif_pci_read_config_dword);
_RP int
lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val)
{
	return WARN(pci_read_config_word(dev, where, val));
}

EXPORT_SYMBOL(lis_osif_pci_read_config_word);
_RP int
lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_set_dma_mask(hwdev, mask));
}

EXPORT_SYMBOL(lis_osif_pci_set_dma_mask);
_RP int
lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
	return WARN(pci_write_config_byte(dev, where, val));
}

EXPORT_SYMBOL(lis_osif_pci_write_config_byte);
_RP int
lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
	return WARN(pci_write_config_dword(dev, where, val));
}

EXPORT_SYMBOL(lis_osif_pci_write_config_dword);
_RP int
lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
	return WARN(pci_write_config_word(dev, where, val));
}

EXPORT_SYMBOL(lis_osif_pci_write_config_word);
#endif

#ifdef CONFIG_PCI
_RP const char *
lis_pcibios_strerror(int error)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL(lis_pcibios_strerror);
#ifdef HAVE_KFUNC_PCIBIOS_INIT
_RP void
lis_pcibios_init(void)
{
#ifdef HAVE_PCIBIOS_INIT_ADDR
	typeof(&pcibios_init) _pcibios_init = (typeof(_pcibios_init)) HAVE_PCIBIOS_INIT_ADDR;
	return WARN(_pcibios_init());
#else
	swerr();
#endif
}

EXPORT_SYMBOL(lis_pcibios_init);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_FIND_CLASS
_RP int
lis_pcibios_find_class(unsigned int class_code, unsigned short index, unsigned char *bus,
		       unsigned char *dev_fn)
{
	return WARN(pcibios_find_class(class_code, index, bus, dev_fn));
}

EXPORT_SYMBOL(lis_pcibios_find_class);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_FIND_DEVICE
_RP int
lis_pcibios_find_device(unsigned short vendor, unsigned short dev_id, unsigned short index,
			unsigned char *bus, unsigned char *dev_fn)
{
	return WARN(pcibios_find_device(vendor, dev_id, index, bus, dev_fn));
}

EXPORT_SYMBOL(lis_pcibios_find_device);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_PRESENT
_RP int
lis_pcibios_present(void)
{
	return WARN(pcibios_present());
}

EXPORT_SYMBOL(lis_pcibios_present);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_READ_CONFIG_BYTE
_RP int
lis_pcibios_read_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
			     unsigned char *val)
{
	return WARN(pcibios_read_config_byte(bus, dev_fn, where, val));
}

EXPORT_SYMBOL(lis_pcibios_read_config_byte);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_READ_CONFIG_DWORD
_RP int
lis_pcibios_read_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
			      unsigned int *val)
{
	return WARN(pcibios_read_config_dword(bus, dev_fn, where, val));
}

EXPORT_SYMBOL(lis_pcibios_read_config_dword);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_READ_CONFIG_WORD
_RP int
lis_pcibios_read_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
			     unsigned short *val)
{
	return WARN(pcibios_read_config_word(bus, dev_fn, where, val));
}

EXPORT_SYMBOL(lis_pcibios_read_config_word);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_WRITE_CONFIG_BYTE
_RP int
lis_pcibios_write_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
			      unsigned char val)
{
	return WARN(pcibios_write_config_byte(bus, dev_fn, where, val));
}

EXPORT_SYMBOL(lis_pcibios_write_config_byte);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_WRITE_CONFIG_DWORD
_RP int
lis_pcibios_write_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
			       unsigned int val)
{
	return WARN(pcibios_write_config_dword(bus, dev_fn, where, val));
}

EXPORT_SYMBOL(lis_pcibios_write_config_dword);
#endif
#ifdef HAVE_KFUNC_PCIBIOS_WRITE_CONFIG_WORD
_RP int
lis_pcibios_write_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
			      unsigned short val)
{
	return WARN(pcibios_write_config_word(bus, dev_fn, where, val));
}

EXPORT_SYMBOL(lis_pcibios_write_config_word);
#endif
#endif

_RP int
lis_request_dma(unsigned int dma_nr, const char *device_id)
{
	return WARN(request_dma(dma_nr, device_id));
}

EXPORT_SYMBOL(lis_request_dma);
#ifdef HAVE_KTYPE_IRQRETURN_T
_RP int
lis_request_irq(unsigned int irq, irqreturn_t(*handler) (int, void *, struct pt_regs *),
		unsigned long flags, const char *device, void *dev_id)
{
	return WARN(request_irq(irq, handler, flags, device, dev_id));
}
#else
int
lis_request_irq(unsigned int irq, void (*handler) (int, void *, struct pt_regs *),
		unsigned long flags, const char *device, void *dev_id)
{
	return WARN(request_irq(irq, handler, flags, device, dev_id));
}
#endif

EXPORT_SYMBOL(lis_request_irq);
_RP int
lis_sprintf(char *bfr, const char *fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = WARN(vsprintf(bfr, fmt, args));
	va_end(args);
	return ret;
}

EXPORT_SYMBOL(lis_sprintf);
_RP int
lis_thread_stop(pid_t pid)
{
	return WARN(kill_proc(pid, SIGTERM, 1));
}

EXPORT_SYMBOL(lis_thread_stop);
_RP int
lis_vsprintf(char *bfr, const char *fmt, va_list args)
{
	return WARN(vsprintf(bfr, fmt, args));
}

EXPORT_SYMBOL(lis_vsprintf);

#ifdef CONFIG_PCI
_RP size_t
lis_osif_sg_dma_len(struct scatterlist *sg)
{
	return WARN(sg_dma_len(sg));
}

EXPORT_SYMBOL(lis_osif_sg_dma_len);
#endif
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP struct inode *
lis_file_inode(struct file *f)
{
	return f->f_dentry->d_inode;
}

EXPORT_SYMBOL(lis_file_inode);
#endif

#ifdef CONFIG_PCI
#ifdef HAVE_KFUNC_PCI_DAC_DMA_TO_PAGE
_RP struct page *
lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev, dma64_addr_t dma_addr)
{
	return WARN(pci_dac_dma_to_page(pdev, dma_addr));
}

EXPORT_SYMBOL(lis_osif_pci_dac_dma_to_page);
#endif
#ifdef HAVE_KFUNC_PCI_FIND_CLASS
_RP struct pci_dev *
lis_osif_pci_find_class(unsigned int class, struct pci_dev *from)
{
	return WARN(pci_find_class(class, from));
}

EXPORT_SYMBOL(lis_osif_pci_find_class);
#endif
_RP struct pci_dev *
lis_osif_pci_find_device(unsigned int vendor, unsigned int device, struct pci_dev *from)
{
	return WARN(pci_find_device(vendor, device, from));
}

EXPORT_SYMBOL(lis_osif_pci_find_device);
_RP struct pci_dev *
lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn)
{
	return WARN(pci_find_slot(bus, devfn));
}

EXPORT_SYMBOL(lis_osif_pci_find_slot);
#endif
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP struct stdata *
lis_file_str(struct file *f)
{
	return WARN((struct stdata *) f->f_dentry->d_inode->i_pipe);
}

EXPORT_SYMBOL(lis_file_str);
#endif
_RP unsigned
lis_usectohz(unsigned usec)
{
	return WARN(usec / (1000000 / HZ));
}

EXPORT_SYMBOL(lis_usectohz);
_RP unsigned long
lis_dsecs(void)
{
	struct timeval tv;

	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec * 10L) + (tv.tv_usec / 100000L);
}

EXPORT_SYMBOL(lis_dsecs);
_RP unsigned long
lis_hitime(void)
{
	struct timeval tv;

	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec & 0x3f) * 1000000 + tv.tv_usec;
}

EXPORT_SYMBOL(lis_hitime);
_RP unsigned long
lis_jiffies(void)
{
	return WARN(jiffies);
}

EXPORT_SYMBOL(lis_jiffies);
_RP unsigned long
lis_msecs(void)
{
	struct timeval tv;

	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec * 1000L) + (tv.tv_usec / 1000L);
}

EXPORT_SYMBOL(lis_msecs);

#ifdef CONFIG_PCI
#ifdef HAVE_KFUNC_PCI_DAC_DMA_TO_OFFSET
_RP unsigned long
lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev, dma64_addr_t dma_addr)
{
	return pci_dac_dma_to_offset(pdev, dma_addr);
}

EXPORT_SYMBOL(lis_osif_pci_dac_dma_to_offset);
#endif
#endif
_RP unsigned long
lis_secs(void)
{
	struct timeval tv;

	WARN(do_gettimeofday(&tv));
	return tv.tv_sec;
}

EXPORT_SYMBOL(lis_secs);
_RP unsigned long
lis_usecs(void)
{
	struct timeval tv;

	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec * 1000000L) + tv.tv_usec;
}

EXPORT_SYMBOL(lis_usecs);
_RP unsigned long
lis_virt_to_phys(volatile void *addr)
{
	return WARN(virt_to_phys(addr));
}

EXPORT_SYMBOL(lis_virt_to_phys);
_RP void *
lis__kfree(void *ptr)
{
	WARN(kfree(ptr));
	return NULL;
}

EXPORT_SYMBOL(lis__kfree);
_RP void *
lis__kmalloc(int nbytes, int class, int use_cache)
{
	return WARN(kmalloc(nbytes, class));
}

EXPORT_SYMBOL(lis__kmalloc);
_RP void *
lis_alloc_atomic_fcn(int nbytes, char *file, int line)
{
	return WARNF(kmalloc(nbytes, GFP_ATOMIC), file, line);
}

EXPORT_SYMBOL(lis_alloc_atomic_fcn);
_RP void *
lis_alloc_dma_fcn(int nbytes, char *file, int line)
{
	return WARNF(kmalloc(nbytes, GFP_DMA), file, line);
}

EXPORT_SYMBOL(lis_alloc_dma_fcn);
_RP void *
lis_alloc_kernel_fcn(int nbytes, char *file, int line)
{
	return WARNF(kmalloc(nbytes, GFP_KERNEL), file, line);
}

EXPORT_SYMBOL(lis_alloc_kernel_fcn);
_RP void *
lis_free_mem_fcn(void *mem_area, char *file, int line)
{
	WARNF(kfree(mem_area), file, line);
	return NULL;
}

EXPORT_SYMBOL(lis_free_mem_fcn);
_RP void *
lis_ioremap(unsigned long offset, unsigned long size)
{
	return WARN(ioremap(offset, size));
}

EXPORT_SYMBOL(lis_ioremap);
_RP void *
lis_ioremap_nocache(unsigned long offset, unsigned long size)
{
	return WARN(ioremap_nocache(offset, size));
}

EXPORT_SYMBOL(lis_ioremap_nocache);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void *
lis_kmalloc(size_t nbytes, int type)
{
	return WARN(kmalloc(nbytes, type));
}

EXPORT_SYMBOL(lis_kmalloc);
#endif
_RP void *
lis_malloc(int nbytes, int class, int use_cache, char *file_name, int line_nr)
{
	return WARN(kmalloc(nbytes, class));
}

EXPORT_SYMBOL(lis_malloc);

#ifdef CONFIG_PCI
_RP void *
lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size, dma_addr_t *dma_handle)
{
	return WARN(pci_alloc_consistent(hwdev, size, dma_handle));
}

EXPORT_SYMBOL(lis_osif_pci_alloc_consistent);
#endif
_RP void *
lis_phys_to_virt(unsigned long addr)
{
	return WARN(phys_to_virt(addr));
}

EXPORT_SYMBOL(lis_phys_to_virt);
_RP void *
lis_vremap(unsigned long offset, unsigned long size)
{
	return WARN(ioremap_nocache(offset, size));
}

EXPORT_SYMBOL(lis_vremap);
_RP void *
lis_zmalloc(int nbytes, int class, char *file_name, int line_nr)
{
	return WARN(kmem_zalloc(nbytes, (class & __GFP_WAIT) ? KM_SLEEP : KM_NOSLEEP));
}

EXPORT_SYMBOL(lis_zmalloc);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_add_timer(struct timer_list *timer)
{
	return WARN(add_timer(timer));
}

EXPORT_SYMBOL(lis_add_timer);
#endif
_RP void
lis_assert_fail(const char *expr, const char *objname, const char *file, unsigned int line)
{
	return (void) printk(KERN_CRIT "%s: assert(%s) failed in file %s, line %u\n", objname, expr,
			     file, line);
}

EXPORT_SYMBOL(lis_assert_fail);
#ifdef LIS_DEPRECARTED_FUNCTIONS
#ifdef HAVE_KFUNC_MOD_DEC_USE_COUNT
_RP void
lis_dec_mod_cnt_fcn(const char *file, int line)
{
	WARNF(MOD_DEC_USE_COUNT, file, line);
	return;
}

EXPORT_SYMBOL(lis_dec_mod_cnt_fcn);
#endif
#endif
_RP void
lis_disable_irq(unsigned int irq)
{
	return WARN(disable_irq(irq));
}

EXPORT_SYMBOL(lis_disable_irq);
_RP void
lis_enable_irq(unsigned int irq)
{
	return WARN(enable_irq(irq));
}

EXPORT_SYMBOL(lis_enable_irq);
_RP void
lis_free_dma(unsigned int dma_nr)
{
	return WARN(free_dma(dma_nr));
}

EXPORT_SYMBOL(lis_free_dma);
_RP void
lis_free_irq(unsigned int irq, void *dev_id)
{
	return WARN(free_irq(irq, dev_id));
}

EXPORT_SYMBOL(lis_free_irq);
_RP void
lis_gettimeofday(struct timeval *tv)
{
	return WARN(do_gettimeofday(tv));
}

EXPORT_SYMBOL(lis_gettimeofday);
#ifdef LIS_DEPRECARTED_FUNCTIONS
#ifdef HAVE_KFUNC_MOD_INC_USE_COUNT
_RP void
lis_inc_mod_cnt_fcn(const char *file, int line)
{
	WARNF(MOD_INC_USE_COUNT, file, line);
	return;
}

EXPORT_SYMBOL(lis_inc_mod_cnt_fcn);
#endif
#endif
#ifdef HAVE_KFUNC_INTERRUPTIBLE_SLEEP_ON
_RP void
lis_interruptible_sleep_on(wait_queue_head_t *wq)
{
	return WARN(interruptible_sleep_on(wq));
}

EXPORT_SYMBOL(lis_interruptible_sleep_on);
#endif
_RP void
lis_kernel_up(struct semaphore *sem)
{
	return WARN(up(sem));
}

EXPORT_SYMBOL(lis_kernel_up);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_kfree(const void *ptr)
{
	return WARN(kfree(ptr));
}

EXPORT_SYMBOL(lis_kfree);
#endif
_RP void
lis_iounmap(void *ptr)
{
	return WARN(iounmap(ptr));
}

EXPORT_SYMBOL(lis_iounmap);
_RP void
lis_osif_cli(void)
{
#ifdef HAVE_KFUNC_CLI
	WARN(cli());
#else
	WARN(local_irq_disable());
#endif
	return;
}

EXPORT_SYMBOL(lis_osif_cli);
_RP void
lis_osif_do_gettimeofday(struct timeval *tp)
{
	return WARN(do_gettimeofday(tp));
}

EXPORT_SYMBOL(lis_osif_do_gettimeofday);
#ifdef HAVE_TIMESPEC_DO_SETTIMEOFDAY
_RP int
lis_osif_do_settimeofday(struct timespec *tp)
{
	return WARN(do_settimeofday(tp));
}
#else
_RP void
lis_osif_do_settimeofday(struct timeval *tp)
{
	return WARN(do_settimeofday(tp));
}
#endif

EXPORT_SYMBOL(lis_osif_do_settimeofday);

#ifdef CONFIG_PCI
#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE
_RP void
lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev, dma64_addr_t dma_addr, size_t len,
				 int direction)
{
	return WARN(pci_dac_dma_sync_single(pdev, dma_addr, len, direction));
}

EXPORT_SYMBOL(lis_osif_pci_dac_dma_sync_single);
#endif

#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE_FOR_CPU
_RP void
lis_osif_pci_dac_dma_sync_single_for_cpu(struct pci_dev *pdev, dma64_addr_t dma_addr,
					 size_t len, int direction)
{
	return WARN(pci_dac_dma_sync_single_for_cpu(pdev, dma_addr, len, direction));
}

EXPORT_SYMBOL(lis_osif_pci_dac_dma_sync_single_for_cpu);
#endif

#ifdef HAVE_KFUNC_PCI_DAC_DMA_SYNC_SINGLE_FOR_DEVICE
_RP void
lis_osif_pci_dac_dma_sync_single_for_device(struct pci_dev *pdev, dma64_addr_t dma_addr,
					    size_t len, int direction)
{
	return WARN(pci_dac_dma_sync_single_for_device(pdev, dma_addr, len, direction));
}

EXPORT_SYMBOL(lis_osif_pci_dac_dma_sync_single_for_device);
#endif

_RP void
lis_osif_pci_disable_device(struct pci_dev *dev)
{
	return WARN(pci_disable_device(dev));
}

EXPORT_SYMBOL(lis_osif_pci_disable_device);
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SG
_RP void
lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nelems, int direction)
{
	return WARN(pci_dma_sync_sg(hwdev, sg, nelems, direction));
}

EXPORT_SYMBOL(lis_osif_pci_dma_sync_sg);
#endif
#ifdef HAVE_KFUNC_PCI_DMA_SYNC_SINGLE
_RP void
lis_osif_pci_dma_sync_single(struct pci_dev *hwdev, dma_addr_t dma_handle, size_t size,
			     int direction)
{
	return WARN(pci_dma_sync_single(hwdev, dma_handle, size, direction));
}

EXPORT_SYMBOL(lis_osif_pci_dma_sync_single);
#endif
_RP void
lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr, dma_addr_t dma_handle)
{
	return WARN(pci_free_consistent(hwdev, size, vaddr, dma_handle));
}

EXPORT_SYMBOL(lis_osif_pci_free_consistent);
_RP void
lis_osif_pci_set_master(struct pci_dev *dev)
{
	return WARN(pci_set_master(dev));
}

EXPORT_SYMBOL(lis_osif_pci_set_master);
_RP void
lis_osif_pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address, size_t size, int direction)
{
	return WARN(pci_unmap_page(hwdev, dma_address, size, direction));
}

EXPORT_SYMBOL(lis_osif_pci_unmap_page);
_RP void
lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{
	return WARN(pci_unmap_sg(hwdev, sg, nents, direction));
}

EXPORT_SYMBOL(lis_osif_pci_unmap_sg);
_RP void
lis_osif_pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, size_t size, int direction)
{
	return WARN(pci_unmap_single(hwdev, dma_addr, size, direction));
}

EXPORT_SYMBOL(lis_osif_pci_unmap_single);
_RP void
lis_osif_pci_unregister_driver(struct pci_driver *p)
{
	return WARN(pci_unregister_driver(p));
}

EXPORT_SYMBOL(lis_osif_pci_unregister_driver);
#endif
_RP void
lis_osif_sti(void)
{
#ifdef HAVE_KFUNC_STI
	WARN(sti());
#else
	WARN(local_irq_enable());
#endif
	return;
}

EXPORT_SYMBOL(lis_osif_sti);
_RP void
lis_putbyte(unsigned char **p, unsigned char byte)
{
	*(*p)++ = byte;
	return;
}

EXPORT_SYMBOL(lis_putbyte);
_RP void
lis_release_region(unsigned int from, unsigned int extent)
{
	return WARN(release_region(from, extent));
}

EXPORT_SYMBOL(lis_release_region);
_RP void
lis_request_region(unsigned int from, unsigned int extent, const char *name)
{
	return (void) WARN(request_region(from, extent, name));
}

EXPORT_SYMBOL(lis_request_region);
#ifdef HAVE_KFUNC_SLEEP_ON
_RP void
lis_sleep_on(wait_queue_head_t *wq)
{
	return WARN(sleep_on(wq));
}

EXPORT_SYMBOL(lis_sleep_on);
#endif
_RP void
lis_udelay(long micro_secs)
{
	return WARN(udelay(micro_secs));
}

EXPORT_SYMBOL(lis_udelay);
#ifdef HAVE_KFUNC_VFREE
_RP void
lis_vfree(void *ptr)
{
	return WARN(vfree(ptr));
}

EXPORT_SYMBOL(lis_vfree);
#endif
_RP void
lis_wake_up(wait_queue_head_t *wq)
{
	return WARN(wake_up(wq));
}

EXPORT_SYMBOL(lis_wake_up);
_RP void
lis_wake_up_interruptible(wait_queue_head_t *wq)
{
	return WARN(wake_up_interruptible(wq));
}

EXPORT_SYMBOL(lis_wake_up_interruptible);
_RP int
lis_splstr_fcn(char *file, int line)
{
	unsigned long flags;

	WARNF(local_irq_save(flags), file, line);
	return flags;
}

EXPORT_SYMBOL(lis_splstr_fcn);
_RP void
lis_splx_fcn(int x, char *file, int line)
{
	unsigned long flags = x;

	WARNF(local_irq_restore(flags), file, line);
	return;
}

EXPORT_SYMBOL(lis_splx_fcn);

/* 
 *  No idea what to do with these.  LiS has some strange concepts of PCI
 *  devices.  Use the Linux kernel facilities directly please.
 */
#ifdef CONFIG_PCI
_RP lis_pci_dev_t *
lis_pci_find_class(unsigned class, lis_pci_dev_t *previous_struct)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL(lis_pci_find_class);
_RP lis_pci_dev_t *
lis_pci_find_device(unsigned vendor, unsigned device, lis_pci_dev_t *previous_struct)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL(lis_pci_find_device);
_RP lis_pci_dev_t *
lis_pci_find_slot(unsigned bus, unsigned dev_fcn)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL(lis_pci_find_slot);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_pci_cleanup(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_pci_cleanup);
#endif
_RP void
lis_pci_disable_device(lis_pci_dev_t *dev)
{
	WARN(pci_disable_device(dev->kern_ptr));
	return;
}

EXPORT_SYMBOL(lis_pci_disable_device);
_RP void
lis_pci_set_master(lis_pci_dev_t *dev)
{
	WARN(pci_set_master(dev->kern_ptr));
	return;
}

EXPORT_SYMBOL(lis_pci_set_master);
_RP int
lis_pci_enable_device(lis_pci_dev_t *dev)
{
	return WARN(pci_enable_device(dev->kern_ptr));
}

EXPORT_SYMBOL(lis_pci_enable_device);
_RP int
lis_pci_read_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char *rtn_val)
{
	return WARN(pci_read_config_byte(dev->kern_ptr, index, rtn_val));
}

EXPORT_SYMBOL(lis_pci_read_config_byte);
_RP int
lis_pci_read_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long *rtn_val)
{
	return WARN(pci_read_config_dword(dev->kern_ptr, index, (u32 *) rtn_val));
}

EXPORT_SYMBOL(lis_pci_read_config_dword);
_RP int
lis_pci_read_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short *rtn_val)
{
	return WARN(pci_read_config_word(dev->kern_ptr, index, rtn_val));
}

EXPORT_SYMBOL(lis_pci_read_config_word);
_RP int
lis_pci_write_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char val)
{
	return WARN(pci_write_config_byte(dev->kern_ptr, index, val));
}

EXPORT_SYMBOL(lis_pci_write_config_byte);
_RP int
lis_pci_write_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long val)
{
	return WARN(pci_write_config_dword(dev->kern_ptr, index, val));
}

EXPORT_SYMBOL(lis_pci_write_config_dword);
_RP int
lis_pci_write_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short val)
{
	return WARN(pci_write_config_word(dev->kern_ptr, index, val));
}

EXPORT_SYMBOL(lis_pci_write_config_word);
#endif

/* 
 *  LiS weighty spin locks.  We just use a normal kernel spin lock
 *  embedded in the oversized LiS structure.
 */
_RP int
lis_spin_is_locked_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	return WARNF(spin_is_locked((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_spin_is_locked_fcn);
_RP int
lis_spin_trylock_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	return WARNF(spin_trylock((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_spin_trylock_fcn);
_RP lis_spin_lock_t *
lis_spin_lock_alloc_fcn(const char *name, char *file, int line)
{
	lis_spin_lock_t *lock;

	if ((lock = WARNF(kmem_zalloc(sizeof(*lock), KM_NOSLEEP), file, line)))
		spin_lock_init((spinlock_t *) lock->spin_lock_mem);
	return lock;
}

EXPORT_SYMBOL(lis_spin_lock_alloc_fcn);
_RP lis_spin_lock_t *
lis_spin_lock_free_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	unsigned long flags;

	spin_lock_irqsave((spinlock_t *) lock->spin_lock_mem, flags);
	WARNF(kmem_free((void *) lock, sizeof(*lock)), file, line);
	local_irq_restore(flags);
	return NULL;
}

EXPORT_SYMBOL(lis_spin_lock_free_fcn);
_RP void
lis_spin_lock_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_lock((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_spin_lock_fcn);
_RP void
lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, char *file, int line)
{
	WARNF(spin_lock_init((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_spin_lock_init_fcn);
_RP void
lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_lock_irq((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_spin_lock_irq_fcn);
_RP void
lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = 0;

	WARNF(spin_lock_str((spinlock_t *) lock->spin_lock_mem, flags), file, line);
	*flagp = flags;
}

EXPORT_SYMBOL(lis_spin_lock_irqsave_fcn);
_RP void
lis_spin_unlock_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_unlock((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_spin_unlock_fcn);
_RP void
lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_unlock_irq((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_spin_unlock_irq_fcn);
_RP void
lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = *flagp;

	WARNF(spin_unlock_str((spinlock_t *) lock->spin_lock_mem, flags), file, line);
}

EXPORT_SYMBOL(lis_spin_unlock_irqrestore_fcn);

/* 
 *  LiS weighty read write locks.  We just use a normal kernel read write lock
 *  embedded in the oversized LiS structure.
 */
_RP lis_rw_lock_t *
lis_rw_lock_alloc_fcn(const char *name, char *file, int line)
{
	lis_rw_lock_t *lock;

	if ((lock = WARNF(kmem_zalloc(sizeof(*lock), KM_NOSLEEP), file, line)))
		rwlock_init((rwlock_t *) lock->rw_lock_mem);
	return lock;
}

EXPORT_SYMBOL(lis_rw_lock_alloc_fcn);
_RP lis_rw_lock_t *
lis_rw_lock_free_fcn(lis_rw_lock_t *lock, const char *name, char *file, int line)
{
	unsigned long flags;

	write_lock_irqsave((rwlock_t *) lock->rw_lock_mem, flags);
	WARNF(kmem_free((void *) lock, sizeof(*lock)), file, line);
	local_irq_restore(flags);
	return NULL;
}

EXPORT_SYMBOL(lis_rw_lock_free_fcn);
_RP void
lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, char *file, int line)
{
	WARNF(rwlock_init((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_lock_init_fcn);
_RP void
lis_rw_read_lock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_lock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_read_lock_fcn);
_RP void
lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_lock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_read_lock_irq_fcn);
_RP void
lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = 0;

	WARNF(read_lock_str((rwlock_t *) lock->rw_lock_mem, flags), file, line);
	*flagp = flags;
}

EXPORT_SYMBOL(lis_rw_read_lock_irqsave_fcn);
_RP void
lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_unlock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_read_unlock_fcn);
_RP void
lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_unlock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_read_unlock_irq_fcn);
_RP void
lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = *flagp;

	WARNF(read_unlock_str((rwlock_t *) lock->rw_lock_mem, flags), file, line);
}

EXPORT_SYMBOL(lis_rw_read_unlock_irqrestore_fcn);
_RP void
lis_rw_write_lock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_lock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_write_lock_fcn);
_RP void
lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_lock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_write_lock_irq_fcn);
_RP void
lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = 0;

	WARNF(write_lock_str((rwlock_t *) lock->rw_lock_mem, flags), file, line);
	*flagp = flags;
}

EXPORT_SYMBOL(lis_rw_write_lock_irqsave_fcn);
_RP void
lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_unlock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_write_unlock_fcn);
_RP void
lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_unlock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL(lis_rw_write_unlock_irq_fcn);
_RP void
lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = *flagp;

	WARNF(write_unlock_str((rwlock_t *) lock->rw_lock_mem, flags), file, line);
}

EXPORT_SYMBOL(lis_rw_write_unlock_irqrestore_fcn);

/* 
 *  LiS weighty atomic wrappers.  We just use the normal kernel atomic types
 *  inside the LiS type.
 */
#ifdef LIS_DEPRECARTED_FUNCTIONS
lis_atomic_t lis_in_syscall = 0;

EXPORT_SYMBOL(lis_in_syscall);
lis_atomic_t lis_open_cnt = 0;

EXPORT_SYMBOL(lis_open_cnt);
lis_atomic_t lis_queues_running = 0;

EXPORT_SYMBOL(lis_queues_running);
lis_atomic_t lis_runq_req_cnt = 0;

EXPORT_SYMBOL(lis_runq_req_cnt);
lis_atomic_t lis_stdata_cnt = 0;

EXPORT_SYMBOL(lis_stdata_cnt);
lis_atomic_t lis_strcount = 0;

EXPORT_SYMBOL(lis_strcount);
lis_atomic_t lis_strstats[24][4] = { {0,}, };

EXPORT_SYMBOL(lis_strstats);
#endif
_RP void
lis_atomic_add(lis_atomic_t *atomic_addr, int amt)
{
	return WARN(atomic_add(amt, (atomic_t *) atomic_addr));
}

EXPORT_SYMBOL(lis_atomic_add);
_RP void
lis_atomic_dec(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_dec((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL(lis_atomic_dec);
_RP void
lis_atomic_inc(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_inc((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL(lis_atomic_inc);
_RP void
lis_atomic_set(lis_atomic_t *atomic_addr, int valu)
{
	return (void) WARN(atomic_set((atomic_t *) atomic_addr, valu));
}

EXPORT_SYMBOL(lis_atomic_set);
_RP void
lis_atomic_sub(lis_atomic_t *atomic_addr, int amt)
{
	return WARN(atomic_sub(amt, (atomic_t *) atomic_addr));
}

EXPORT_SYMBOL(lis_atomic_sub);
_RP int
lis_atomic_dec_and_test(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_dec_and_test((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL(lis_atomic_dec_and_test);
_RP int
lis_atomic_read(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_read((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL(lis_atomic_read);

/* 
 *  LiS weighty semaphore wrappers.  We just use a normal kernel semaphore
 *  embedded in the oversized LiS structure (sem_mem area).
 */
_RP int
lis_down_fcn(lis_semaphore_t *lsem, char *file, int line)
{
	return WARNF(down_interruptible((struct semaphore *) lsem->sem_mem), file, line);
}

EXPORT_SYMBOL(lis_down_fcn);
_RP lis_semaphore_t *
lis_sem_alloc(int count)
{
	lis_semaphore_t *lsem;
	lsem = WARN(kmem_zalloc(sizeof(lis_semaphore_t), KM_NOSLEEP));
	if (lsem != NULL)
		init_MUTEX((struct semaphore *) lsem->sem_mem);
	return lsem;
}

EXPORT_SYMBOL(lis_sem_alloc);
_RP lis_semaphore_t *
lis_sem_destroy(lis_semaphore_t *lsem)
{
	WARN(kmem_free((void *) lsem, sizeof(*lsem)));
	return NULL;
}

EXPORT_SYMBOL(lis_sem_destroy);
_RP void
lis_sem_init(lis_semaphore_t *lsem, int count)
{
	return WARN(sema_init((struct semaphore *) lsem->sem_mem, count));
}

EXPORT_SYMBOL(lis_sem_init);
_RP void
lis_up_fcn(lis_semaphore_t *lsem, char *file, int line)
{
	return WARNF(up((struct semaphore *) lsem->sem_mem), file, line);
}

EXPORT_SYMBOL(lis_up_fcn);

/* 
 *  These are bad, don't use them.
 */
_RP void *
lis_free_pages_fcn(void *ptr, char *file, int line)
{
	swerr();
	WARNF(free_pages((unsigned long) ptr, 0), file, line);
	return NULL;
}

EXPORT_SYMBOL(lis_free_pages_fcn);
_RP void *
lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line)
{
	int order;

	for (order = 0, nbytes >>= PAGE_SHIFT; nbytes; order++, nbytes >>= 1) ;
	return (void *) WARNF(__get_free_pages(GFP_ATOMIC, order), file, line);
}

EXPORT_SYMBOL(lis_get_free_pages_atomic_fcn);
_RP void *
lis_get_free_pages_fcn(int nbytes, int class, char *file, int line)
{
	int order;

	for (order = 0, nbytes >>= PAGE_SHIFT; nbytes; order++, nbytes >>= 1) ;
	return (void *) WARNF(__get_free_pages(class, order), file, line);
}

EXPORT_SYMBOL(lis_get_free_pages_fcn);
_RP void *
lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line)
{
	int order;

	for (order = 0, nbytes >>= PAGE_SHIFT; nbytes; order++, nbytes >>= 1) ;
	return (void *) WARNF(__get_free_pages(GFP_KERNEL, order), file, line);
}

EXPORT_SYMBOL(lis_get_free_pages_kernel_fcn);

/* 
 *  These are just wrappered system calls.
 */
_RP int
lis_mknod(char *name, int mode, dev_t dev)
{
#ifdef HAVE_SYS_MKNOD_ADDR
	asmlinkage long (*sys_mknod) (const char *filename, int mode, dev_t dev)
	    = (typeof(sys_mknod)) HAVE_SYS_MKNOD_ADDR;
	mm_segment_t old_fs;
	int ret;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	ret = WARN(sys_mknod(name, mode, dev));
	set_fs(old_fs);
	return ret;
#else
	swerr();
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL(lis_mknod);
_RP int
lis_unlink(char *name)
{
#ifdef HAVE_SYS_UNLINK_ADDR
	asmlinkage long (*sys_unlink) (const char *pathname)
	    = (typeof(sys_unlink)) HAVE_SYS_UNLINK_ADDR;
	mm_segment_t old_fs;
	int ret;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	ret = WARN(sys_unlink(name));
	set_fs(old_fs);
	return ret;
#else
	swerr();
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL(lis_unlink);
_RP int
lis_mount(char *dev_name, char *dir_name, char *fstype, unsigned long rwflag, void *data)
{
#ifdef HAVE_SYS_MOUNT_ADDR
	asmlinkage long (*sys_mount) (char *dev_name, char *dir_name, char *type,
				      unsigned long flags, void *data)
	    = (typeof(sys_mount)) HAVE_SYS_MOUNT_ADDR;
	mm_segment_t old_fs;
	int ret;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	ret = WARN(sys_mount(dev_name, dir_name, fstype, rwflag, data));
	set_fs(old_fs);
	return ret;
#else
	swerr();
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL(lis_mount);
#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP int
lis_umount2(char *path, int flags)
{
#ifdef HAVE_SYS_UMOUNT_ADDR
	asmlinkage long (*sys_umount) (char *name, int flags)
	    = (typeof(sys_umount)) HAVE_SYS_UMOUNT_ADDR;
	mm_segment_t old_fs;
	int ret;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	ret = WARN(sys_umount(path, flags));
	set_fs(old_fs);
	return ret;
#else
	swerr();
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL(lis_umount2);
#endif
#ifdef LIS_DEPRECARTED_FUNCTIONS
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
_RP int
lis_fattach(struct file *f, const char *path)
{
	long do_fattach(const struct file *f, const char *path);

	return WARN(do_fattach(f, path));
}

EXPORT_SYMBOL(lis_fattach);
_RP int
lis_fdetach(const char *path)
{
	long do_fdetach(const char *path);

	return WARN(do_fdetach(path));
}

EXPORT_SYMBOL(lis_fdetach);
#endif				/* HAVE_KERNEL_FATTACH_SUPPORT */
#endif
#ifdef LIS_DEPRECARTED_FUNCTIONS
#ifdef HAVE_KERNEL_PIPE_SUPPORT
_RP int
lis_pipe(unsigned int *fd)
{
	long do_spipe(int *fd);

	return WARN(do_spipe(fd));
}

EXPORT_SYMBOL(lis_pipe);
#endif				/* HAVE_KERNEL_PIPE_SUPPORT */
#endif

#ifdef LIS_DEPRECARTED_FUNCTIONS
_RP void
lis_fdetach_all(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_fdetach_all);
_RP void
lis_fdetach_stream(stdata_t *head)
{
	swerr();
	return;
}

EXPORT_SYMBOL(lis_fdetach_stream);
#endif

/* 
 *  STREAMS Configuration
 */
/**
 *  lis_register_strdev - emulation of LiS STREAMS device registration
 *  @major:major device number to register
 *  @strtab:pointer to struct streamtab structure to register
 *  @nminor:number of minor numbers (ignored)
 *  @name:the name of the device
 */
_RP int
lis_register_strdev(major_t major, struct streamtab *strtab, int nminor, const char *name)
{
	struct cdevsw *cdev;
	int err;

	if (nminor & MINORMASK)
		return (-EINVAL);	/* handle multiple majors later */
	if ((cdev = kmem_zalloc(sizeof(*cdev), KM_NOSLEEP)) == NULL)
		return (-ENOMEM);
	cdev->d_name = name;
	cdev->d_str = strtab;
	cdev->d_flag = D_LIS | D_NEW | D_MTPERQ;	/* mark LiS compatibility for qopen and
							   qclose */
	cdev->d_kmod = NULL;
	atomic_set(&cdev->d_count, 0);
	INIT_LIST_HEAD(&cdev->d_majors);
	INIT_LIST_HEAD(&cdev->d_minors);
	INIT_LIST_HEAD(&cdev->d_apush);
	INIT_LIST_HEAD(&cdev->d_stlist);
	if ((err = WARN(register_strdev(cdev, major))) < 0)
		kmem_free(cdev, sizeof(*cdev));
	return (err);
}

EXPORT_SYMBOL(lis_register_strdev);

/**
 *  lis_unregister_strdev - emulation of LiS STREAMS device deregistration
 *  @major:major number to deregister
 */
_RP int
lis_unregister_strdev(major_t major)
{
	struct cdevsw *cdev;
	int err;

#ifdef MAX_CHRDEV
	if (0 >= major || major >= MAX_CHRDEV)
#else
	if (major != MAJOR(MKDEV(major, 0)))
#endif
		return (-EINVAL);
	if ((cdev = sdev_get(major)) == NULL)
		return (-ENOENT);
	printd(("%s: %s: got device\n", __FUNCTION__, cdev->d_name));
	printd(("%s: %s: putting device\n", __FUNCTION__, cdev->d_name));
	sdev_put(cdev);
	/* we should be able to accept a cdev of NULL so that we don't need to export sdev_get and
	   sdev_put when liscomp is loaded as a module */
	if ((err = unregister_strdev(cdev, major)) == 0)
		kmem_free(cdev, sizeof(*cdev));
	return (err);
}

EXPORT_SYMBOL(lis_unregister_strdev);

/**
 *  lis_register_strmod - emulation of LiS STREAMS module registration
 *  @strtab:pointer to struct streamtab structure to register
 *  @name:the name of the module
 */
_RP modID_t
lis_register_strmod(struct streamtab *strtab, const char *name)
{
	struct fmodsw *fmod;
	int err;

	if ((fmod = kmem_zalloc(sizeof(*fmod), KM_NOSLEEP)) == NULL)
		return (-ENOMEM);
	/* this is ugly, we have to ignore "name" for deregistration */
	fmod->f_name = strtab->st_rdinit->qi_minfo->mi_idname;
	fmod->f_str = strtab;
	fmod->f_flag = D_LIS | D_NEW | D_MTPERQ;	/* mark LiS compatibility for qopen and
							   qclose */
	fmod->f_kmod = NULL;
	atomic_set(&fmod->f_count, 0);
	if ((err = register_strmod(fmod)) < 0)
		kmem_free(fmod, sizeof(*fmod));
	return (err);
}

EXPORT_SYMBOL(lis_register_strmod);

/**
 *  lis_unregister_strmod - emulation of LiS STREAMS module deregistration
 *  @strtab:point to struct streamtab structure to deregister
 */
_RP int
lis_unregister_strmod(struct streamtab *strtab)
{
	struct fmodsw *fmod;
	int err;

	/* this is ugly, we have to ignore "name" for deregistration */
	/* we should be able to accept a cdev of NULL so that we don't need to export fmod_find and
	   fmod_put when liscomp is loaded as a module */
	if ((fmod = fmod_find(strtab->st_rdinit->qi_minfo->mi_idname)) == NULL)
		return (-ENOENT);
	printd(("%s: %s: found module\n", __FUNCTION__, fmod->f_name));
	printd(("%s: %s: putting module\n", __FUNCTION__, fmod->f_name));
	fmod_put(fmod);
	if ((err = WARN(unregister_strmod(fmod))) == 0)
		kmem_free(fmod, sizeof(*fmod));
	return (err);
}

EXPORT_SYMBOL(lis_unregister_strmod);

/**
 *  lis_register_driver_qlock_option - emulation of LiS STREAMS driver synchronization
 *  @major: major device number of driver
 *  @qlock_option: synchronization level
 */
_RP int
lis_register_driver_qlock_option(major_t major, int qlock_option)
{
	struct cdevsw *cdev;
	int err = 0;

	if ((cdev = sdev_get(major)) == NULL)
		return (-EINVAL);
	if (cdev->d_syncq != NULL)
		err = -EINVAL;
	else {
		switch (qlock_option) {
		case LIS_QLOCK_NONE:
			cdev->d_sqlvl = SQLVL_NOP;
			cdev->d_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			cdev->d_flag |= D_MP;
			break;
		default:
		case LIS_QLOCK_QUEUE:
			cdev->d_sqlvl = SQLVL_QUEUE;
			cdev->d_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			cdev->d_flag |= D_MTPERQ;
			break;
		case LIS_QLOCK_QUEUE_PAIR:
			cdev->d_sqlvl = SQLVL_QUEUEPAIR;
			cdev->d_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			cdev->d_flag |= D_MTQPAIR;
			break;
		case LIS_QLOCK_GLOBAL:
			cdev->d_sqlvl = SQLVL_GLOBAL;
			cdev->d_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			cdev->d_flag |= 0;
			err = register_strsync((struct fmodsw *)cdev);
			break;
		}
	}
	sdev_put(cdev);
	return (err);
}

EXPORT_SYMBOL(lis_register_driver_qlock_option);

/**
 *  lis_register_module_qlock_option - emulation of LiS STREAMS module synchronization
 *  @modid: module id
 *  @qlock_option: synchronization level
 */
_RP int
lis_register_module_qlock_option(modID_t modid, int qlock_option)
{
	struct fmodsw *fmod;
	int err = 0;

	if ((fmod = fmod_get(modid)) == NULL)
		return (-EINVAL);
	if (fmod->f_syncq != NULL)
		err = -EINVAL;
	else {
		switch (qlock_option) {
		case LIS_QLOCK_NONE:
			fmod->f_sqlvl = SQLVL_NOP;
			fmod->f_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			fmod->f_flag |= D_MP;
			break;
		default:
		case LIS_QLOCK_QUEUE:
			fmod->f_sqlvl = SQLVL_QUEUE;
			fmod->f_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			fmod->f_flag |= D_MTPERQ;
			break;
		case LIS_QLOCK_QUEUE_PAIR:
			fmod->f_sqlvl = SQLVL_QUEUEPAIR;
			fmod->f_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			fmod->f_flag |= D_MTQPAIR;
			break;
		case LIS_QLOCK_GLOBAL:
			fmod->f_sqlvl = SQLVL_GLOBAL;
			fmod->f_flag &= ~(D_MP|D_UP|D_MTPERQ|D_MTQPAIR|D_MTPERMOD|D_MTOUTPERIM|D_MTOCEXCL|D_MTPUTSHARED);
			fmod->f_flag |= 0;
			err = register_strsync(fmod);
			break;
		}
	}
	fmod_put(fmod);
	return (err);
}

EXPORT_SYMBOL(lis_register_module_qlock_option);

#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
static
#endif
int __init
liscomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
	printk(KERN_INFO LISCOMP_BANNER);
#else
	printk(KERN_INFO LISCOMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
static
#endif
void __exit
liscomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
module_init(liscomp_init);
module_exit(liscomp_exit);
#endif
