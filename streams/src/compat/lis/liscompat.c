/*****************************************************************************

 @(#) $RCSfile: liscompat.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/04/30 10:42:00 $

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

 Last Modified $Date: 2004/04/30 10:42:00 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: liscompat.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/04/30 10:42:00 $"

static char const ident[] =
    "$RCSfile: liscompat.c,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/04/30 10:42:00 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>	/* for MOD_DEC_USE_COUNT etc */
#include <linux/modversions.h>

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

#define __LIS_EXTERN_INLINE inline
#define __LIS_NO_MACROS

#include <linux/kernel.h>	/* for vsprintf and friends */
#include <linux/vmalloc.h>	/* for vmalloc */
#ifdef CONFIG_PCI
#include <linux/pci.h>		/* for many pci functions */
#include <asm/pci.h>		/* for many pci functions */
#endif
#include <linux/ioport.h>	/* for check_region and friends */
#include <asm/uaccess.h>	/* for verify_area and friends */
#include <linux/timer.h>	/* for del_timer and friends */
#include <asm/semaphore.h>	/* for semaphores */
#include <linux/sched.h>	/* for kill_proc, jiffies and friends */
#include <linux/kmod.h>		/* for request_module and friends */
#include <linux/threads.h>	/* for NR_CPUS */
#include <asm/dma.h>		/* for request_dma and friends */
#include <linux/fs.h>		/* for filesystem related stuff */
#include <linux/time.h>		/* for do_gettimeofday and friends */
#include <asm/io.h>		/* for virt_to_page and friends */
#include <linux/slab.h>		/* for kmalloc and friends */
#include <asm/irq.h>		/* for disable_irq */
#include <asm/system.h>		/* for sti, cli */
#include <asm/delay.h>		/* for udelay */
#include <linux/spinlock.h>	/* for spinlock functions */
#include <asm/atomic.h>		/* for atomic functions */
#include <linux/poll.h>		/* for poll_table */
#include <linux/string.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#define _LIS_SOURCE
#include <sys/kmem.h>		/* for SVR4 style kmalloc functions */
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "strdebug.h"
#include "strhead.h"
#include "strsad.h"

#define LISCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LISCOMP_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define LISCOMP_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.9 $) $Date: 2004/04/30 10:42:00 $"
#define LISCOMP_DEVICE		"LiS 2.16 Compatibility"
#define LISCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define LISCOMP_LICENSE		"GPL"
#define LISCOMP_BANNER		LISCOMP_DESCRIP		"\n" \
				LISCOMP_COPYRIGHT	"\n" \
				LISCOMP_REVISION	"\n" \
				LISCOMP_DEVICE		"\n" \
				LISCOMP_CONTACT		"\n"
#define LISCOMP_SPLASH		LISCOMP_DEVICE		" - " \
				LISCOMP_REVISION	"\n"

MODULE_AUTHOR(LISCOMP_CONTACT);
MODULE_DESCRIPTION(LISCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(LISCOMP_DEVICE);
MODULE_LICENSE(LISCOMP_LICENSE);

#ifndef CONFIG_STREAMS_CLONE_MAJOR
#define CONFIG_STREAMS_CLONE_MAJOR 54
//#error "CONFIG_STREAMS_CLONE_MAJOR must be defined."
#endif

/* 
 *  This file provides some level of binary compatibility for LiS exported
 *  symbols.  It is only linked into the kernel when compiled with
 *  CONFIG_STREAMS_COMPAT_LIS set.
 */
static char *dont_use_this_function(void)
{
	swerr();
	return "(DON'T USE THIS FUNCTION!)";
}

#ifdef CONFIG_STREAMS_DEBUG
#if 0
static void warn_kern_violation(char *func, char *file, int line)
{
	printk(KERN_WARNING "%s: export violation: %s +%d\n", func, file, line);
}
#endif
static void warn_kern_wrapper(const char *func, const char *repl)
{
	printk(KERN_DEBUG "%s: don't call this function: use %s instead\n", func, repl);
}
static void warnf_kern_wrapper(const char *func, const char *repl, const char *f, const int l)
{
	printk(KERN_DEBUG "%s [%s +%d]: don't call this function: use %s instead\n", func, f, l,
	       repl);
}

#define WARN(__exp) \
	({ warn_kern_wrapper(__FUNCTION__,#__exp); __exp; })

#define WARNF(__exp, __f, __l) \
	({ warnf_kern_wrapper(__FUNCTION__,#__exp, __f, __l); __exp; })

#else				/* CONFIG_STREAMS_DEBUG */

#define WARN(__exp) \
	({ __exp; })

#define WARNF(__exp, __f, __l) \
	({ __exp; })

#endif				/* CONFIG_STREAMS_DEBUG */

/* 
 *  Here are the lis definitions...
 */
__LIS_EXTERN_INLINE int lis_adjmsg(mblk_t *mp, int length);
EXPORT_SYMBOL_GPL(lis_adjmsg);
__LIS_EXTERN_INLINE struct msgb *lis_allocb(int size, unsigned int priority, char *file_name,
					    int line_nr);
EXPORT_SYMBOL_GPL(lis_allocb);
__LIS_EXTERN_INLINE struct msgb *lis_allocb_physreq(int size, unsigned int priority,
						    void *physreq_ptr, char *file_name,
						    int line_nr);
EXPORT_SYMBOL_GPL(lis_allocb_physreq);
__LIS_EXTERN_INLINE queue_t *lis_allocq(const char *name);
EXPORT_SYMBOL_GPL(lis_allocq);
__LIS_EXTERN_INLINE void lis_appq(queue_t *q, mblk_t *mp1, mblk_t *mp2);
EXPORT_SYMBOL_GPL(lis_appq);
__LIS_EXTERN_INLINE queue_t *lis_backq(queue_t *q);
EXPORT_SYMBOL_GPL(lis_backq);
__LIS_EXTERN_INLINE queue_t *lis_backq_fcn(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_backq_fcn);
__LIS_EXTERN_INLINE int lis_bcanput(queue_t *q, unsigned char band);
EXPORT_SYMBOL_GPL(lis_bcanput);
__LIS_EXTERN_INLINE int lis_bcanputnext(queue_t *q, unsigned char band);
EXPORT_SYMBOL_GPL(lis_bcanputnext);
__LIS_EXTERN_INLINE int lis_bcanputnext_anyband(queue_t *q);
EXPORT_SYMBOL_GPL(lis_bcanputnext_anyband);
__LIS_EXTERN_INLINE int lis_bufcall(unsigned size, int priority, void (*function) (long), long arg);
EXPORT_SYMBOL_GPL(lis_bufcall);
__LIS_EXTERN_INLINE mblk_t *lis_copyb(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_copyb);
__LIS_EXTERN_INLINE mblk_t *lis_copymsg(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_copymsg);
__LIS_EXTERN_INLINE int lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len);
EXPORT_SYMBOL_GPL(lis_copyin);
__LIS_EXTERN_INLINE int lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len);
EXPORT_SYMBOL_GPL(lis_copyout);
__LIS_EXTERN_INLINE mblk_t *lis_dupb(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_dupb);
__LIS_EXTERN_INLINE mblk_t *lis_dupmsg(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_dupmsg);
__LIS_EXTERN_INLINE mblk_t *lis_esballoc(unsigned char *base, int size, int priority,
					 frtn_t *freeinfo, char *file_name, int line_nr);
EXPORT_SYMBOL_GPL(lis_esballoc);
__LIS_EXTERN_INLINE int lis_esbbcall(int priority, void (*function) (long), long arg);
EXPORT_SYMBOL_GPL(lis_esbbcall);
__LIS_EXTERN_INLINE void lis_flushband(queue_t *q, unsigned char band, int flag);
EXPORT_SYMBOL_GPL(lis_flushband);
__LIS_EXTERN_INLINE void lis_flushq(queue_t *q, int flag);
EXPORT_SYMBOL_GPL(lis_flushq);
__LIS_EXTERN_INLINE void lis_freeb(mblk_t *bp);
EXPORT_SYMBOL_GPL(lis_freeb);
__LIS_EXTERN_INLINE void lis_freemsg(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_freemsg);
__LIS_EXTERN_INLINE mblk_t *lis_getq(queue_t *q);
EXPORT_SYMBOL_GPL(lis_getq);
__LIS_EXTERN_INLINE void lis_freeq(queue_t *q);
EXPORT_SYMBOL_GPL(lis_freeq);
__LIS_EXTERN_INLINE int lis_insq(queue_t *q, mblk_t *emp, mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_insq);
__LIS_EXTERN_INLINE void lis_linkb(mblk_t *mp1, mblk_t *mp2);
EXPORT_SYMBOL_GPL(lis_linkb);
__LIS_EXTERN_INLINE int lis_msgdsize(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_msgdsize);
__LIS_EXTERN_INLINE mblk_t *lis_msgpullup(mblk_t *mp, int len);
EXPORT_SYMBOL_GPL(lis_msgpullup);
__LIS_EXTERN_INLINE int lis_msgsize(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_msgsize);
__LIS_EXTERN_INLINE int lis_pullupmsg(mblk_t *mp, int length);
EXPORT_SYMBOL_GPL(lis_pullupmsg);
__LIS_EXTERN_INLINE int lis_putbq(queue_t *q, mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_putbq);
__LIS_EXTERN_INLINE int lis_putctl(queue_t *q, int type, char *file_name, int line_nr);
EXPORT_SYMBOL_GPL(lis_putctl);
__LIS_EXTERN_INLINE int lis_putctl1(queue_t *q, int type, int param, char *file_name, int line_nr);
EXPORT_SYMBOL_GPL(lis_putctl1);
__LIS_EXTERN_INLINE int lis_putnextctl(queue_t *q, int type, char *file_name, int line_nr);
EXPORT_SYMBOL_GPL(lis_putnextctl);
__LIS_EXTERN_INLINE int lis_putnextctl1(queue_t *q, int type, int param, char *file_name,
					int line_nr);
EXPORT_SYMBOL_GPL(lis_putnextctl1);
__LIS_EXTERN_INLINE int lis_putq(queue_t *q, mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_putq);
__LIS_EXTERN_INLINE int lis_qcountstrm(queue_t *q);
EXPORT_SYMBOL_GPL(lis_qcountstrm);
__LIS_EXTERN_INLINE void lis_qdetach(queue_t *q, int do_close, int flag, cred_t *creds);
EXPORT_SYMBOL_GPL(lis_qdetach);
__LIS_EXTERN_INLINE void lis_qenable(queue_t *q);
EXPORT_SYMBOL_GPL(lis_qenable);
__LIS_EXTERN_INLINE void lis_qprocsoff(queue_t *rdq);
EXPORT_SYMBOL_GPL(lis_qprocsoff);
__LIS_EXTERN_INLINE void lis_qprocson(queue_t *rdq);
EXPORT_SYMBOL_GPL(lis_qprocson);
__LIS_EXTERN_INLINE int lis_qsize(queue_t *q);
EXPORT_SYMBOL_GPL(lis_qsize);
__LIS_EXTERN_INLINE mblk_t *lis_rmvb(mblk_t *mp, mblk_t *bp);
EXPORT_SYMBOL_GPL(lis_rmvb);
__LIS_EXTERN_INLINE void lis_rmvq(queue_t *q, mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_rmvq);
__LIS_EXTERN_INLINE queue_t *lis_safe_OTHERQ(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_OTHERQ);
__LIS_EXTERN_INLINE queue_t *lis_safe_RD(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_RD);
__LIS_EXTERN_INLINE int lis_safe_SAMESTR(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_SAMESTR);
__LIS_EXTERN_INLINE queue_t *lis_safe_WR(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_WR);
__LIS_EXTERN_INLINE int lis_safe_canenable(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_canenable);
__LIS_EXTERN_INLINE void lis_safe_enableok(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_enableok);
__LIS_EXTERN_INLINE void lis_safe_noenable(queue_t *q, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_noenable);
__LIS_EXTERN_INLINE void lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_putmsg);
__LIS_EXTERN_INLINE void lis_safe_putnext(queue_t *q, mblk_t *mp, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_putnext);
__LIS_EXTERN_INLINE void lis_safe_qreply(queue_t *q, mblk_t *mp, char *f, int l);
EXPORT_SYMBOL_GPL(lis_safe_qreply);
__LIS_EXTERN_INLINE void lis_setq(queue_t *q, struct qinit *rinit, struct qinit *winit);
EXPORT_SYMBOL_GPL(lis_setq);
__LIS_EXTERN_INLINE int lis_strqget(queue_t *q, qfields_t what, unsigned char band, long *val);
EXPORT_SYMBOL_GPL(lis_strqget);
__LIS_EXTERN_INLINE int lis_strqset(queue_t *q, qfields_t what, unsigned char band, long val);
EXPORT_SYMBOL_GPL(lis_strqset);
__LIS_EXTERN_INLINE int lis_testb(int size, unsigned int priority);
EXPORT_SYMBOL_GPL(lis_testb);
__LIS_EXTERN_INLINE toid_t lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks,
					   char *file_name, int line_nr);
EXPORT_SYMBOL_GPL(lis_timeout_fcn);
__LIS_EXTERN_INLINE void lis_unbufcall(int bcid);
EXPORT_SYMBOL_GPL(lis_unbufcall);
__LIS_EXTERN_INLINE mblk_t *lis_unlinkb(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_unlinkb);
__LIS_EXTERN_INLINE toid_t lis_untimeout(toid_t id);
EXPORT_SYMBOL_GPL(lis_untimeout);
__LIS_EXTERN_INLINE int lis_xmsgsize(mblk_t *mp);
EXPORT_SYMBOL_GPL(lis_xmsgsize);

char *lis_poll_file = "<linux/poll.h>";
EXPORT_SYMBOL_GPL(lis_poll_file);
char *lis_stropts_file = "<linux/stropts.h>";
EXPORT_SYMBOL_GPL(lis_stropts_file);
char lis_date[] = "2003/10/28 08:00:04";
EXPORT_SYMBOL_GPL(lis_date);
char lis_kernel_version[] = UTS_RELEASE;
EXPORT_SYMBOL_GPL(lis_kernel_version);
char lis_version[] = "0.9.2.17";
EXPORT_SYMBOL_GPL(lis_version);
char *lis_poll_events(short events)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL_GPL(lis_poll_events);
const char *lis_maj_min_name(stdata_t *head)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL_GPL(lis_maj_min_name);
const char *lis_msg_type_name(mblk_t *mp)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL_GPL(lis_msg_type_name);
const char *lis_queue_name(queue_t *q)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL_GPL(lis_queue_name);
const char *lis_strm_name(stdata_t *head)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL_GPL(lis_strm_name);
const char *lis_strm_name_from_queue(queue_t *q)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL_GPL(lis_strm_name_from_queue);
int lis_apush_get(struct lis_strapush *ap)
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

EXPORT_SYMBOL_GPL(lis_apush_get);
int lis_apush_set(struct lis_strapush *ap)
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

EXPORT_SYMBOL_GPL(lis_apush_set);
int lis_apushm(dev_t dev, const char *mods[])
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

EXPORT_SYMBOL_GPL(lis_apushm);
int lis_check_guard(void *ptr, char *msg)
{
	return 1;
}

EXPORT_SYMBOL_GPL(lis_check_guard);
int lis_check_mem(void)
{
	return 1;
}

EXPORT_SYMBOL_GPL(lis_check_mem);
int lis_check_q_magic(queue_t *q, char *file, int line)
{
	return 1;
}

EXPORT_SYMBOL_GPL(lis_check_q_magic);
int lis_clone_major(void)
{
	return CONFIG_STREAMS_CLONE_MAJOR;
}

EXPORT_SYMBOL_GPL(lis_clone_major);
int lis_doclose(struct inode *i, struct file *f, stdata_t *head, cred_t *creds)
{
	return strclose(i, f);
}

EXPORT_SYMBOL_GPL(lis_doclose);
int lis_fifo_open_sync(struct inode *i, struct file *f)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL_GPL(lis_fifo_open_sync);
int lis_fifo_write_sync(struct inode *i, int written)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL_GPL(lis_fifo_write_sync);
int lis_get_fifo(struct file **f)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL_GPL(lis_get_fifo);
int lis_get_pipe(struct file **f0, struct file **f1)
{
	swerr();
	return -ENOSYS;
}

EXPORT_SYMBOL_GPL(lis_get_pipe);
int lis_ioc_fattach(struct file *f, char *path)
{
	return lis_fattach(f, path);
}

EXPORT_SYMBOL_GPL(lis_ioc_fattach);
int lis_ioc_fdetach(char *path)
{
	return lis_fdetach(path);
}

EXPORT_SYMBOL_GPL(lis_ioc_fdetach);
int lis_ioc_pipe(unsigned int *fildes)
{
	return lis_pipe(fildes);
}

EXPORT_SYMBOL_GPL(lis_ioc_pipe);

int lis_major = 0;
EXPORT_SYMBOL_GPL(lis_major);
int lis_own_spl(void)
{
	return 1;
}

EXPORT_SYMBOL_GPL(lis_own_spl);
int lis_printk(const char *fmt, ...)
{
	static spinlock_t printk_lock = SPIN_LOCK_UNLOCKED;
	static char printk_buf[1024];
	unsigned long flags;
	va_list args;
	int ret;
	spin_lock_irqsave(&printk_lock, flags);
	va_start(args, fmt);
	vsnprintf(printk_buf, sizeof(printk_buf), fmt, args);
	va_end(args);
	ret = WARN(printk("%s", printk_buf));
	spin_unlock_irqrestore(&printk_lock, flags);
	return ret;
}

EXPORT_SYMBOL_GPL(lis_printk);
int lis_strclose(struct inode *i, struct file *f)
{
	return strclose(i, f);
}

EXPORT_SYMBOL_GPL(lis_strclose);
int lis_strgetpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp, int *bandp,
		   int *flagsp, int doit)
{
	return strgetpmsg(fp, ctlp, datp, bandp, flagsp);
}

EXPORT_SYMBOL_GPL(lis_strgetpmsg);
int lis_strioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
{
	return strioctl(i, f, cmd, arg);
}

EXPORT_SYMBOL_GPL(lis_strioctl);
int lis_stropen(struct inode *i, struct file *f)
{
	return stropen(i, f);
}

EXPORT_SYMBOL_GPL(lis_stropen);
int lis_strputpmsg(struct inode *i, struct file *fp, void *ctlp, void *datp, int band, int flags)
{
	return strputpmsg(fp, ctlp, datp, band, flags);
}

EXPORT_SYMBOL_GPL(lis_strputpmsg);
int lis_valid_mod_list(struct str_list ml)
{
	return apush_vml(&ml);
}

EXPORT_SYMBOL_GPL(lis_valid_mod_list);

long lis_max_mem = 0;
EXPORT_SYMBOL_GPL(lis_max_mem);
long lis_milli_to_ticks(long milli_sec)
{
	return WARN(milli_sec / (1000 / HZ));
}

EXPORT_SYMBOL_GPL(lis_milli_to_ticks);
pid_t lis_thread_start(int (*fcn) (void *), void *arg, const char *name)
{
	return 0;
}

EXPORT_SYMBOL_GPL(lis_thread_start);
ssize_t lis_strread(struct file *fp, char *ubuff, size_t ulen, loff_t *op)
{
	return strread(fp, ubuff, ulen, op);
}

EXPORT_SYMBOL_GPL(lis_strread);
ssize_t lis_strwrite(struct file *fp, const char *ubuff, size_t ulen, loff_t *op)
{
	return strwrite(fp, ubuff, ulen, op);
}

EXPORT_SYMBOL_GPL(lis_strwrite);
streamtab_t *lis_find_strdev(major_t major)
{
	struct streamtab *st = NULL;
	if (major < MAX_CHRDEV) {
		struct cdevsw *cdev;
		if ((cdev = cdev_get(major)) != NULL) {
			st = cdev->d_str;
			cdev_put(cdev);
		}
	}
	return st;
}

EXPORT_SYMBOL_GPL(lis_find_strdev);
struct fmodsw *lis_fmod_sw = NULL;
EXPORT_SYMBOL_GPL(lis_fmod_sw);
struct fmodsw *lis_fstr_sw = NULL;
EXPORT_SYMBOL_GPL(lis_fstr_sw);
struct inode *lis_old_inode(struct file *f, struct inode *i)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_old_inode);
unsigned lis_poll_2_1(struct file *fp, poll_table * wait)
{
	return strpoll(fp, wait);
}

EXPORT_SYMBOL_GPL(lis_poll_2_1);
unsigned lis_poll_bits(stdata_t *hd)
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

EXPORT_SYMBOL_GPL(lis_poll_bits);
unsigned long lis_debug_mask2 = 0;
EXPORT_SYMBOL_GPL(lis_debug_mask2);
unsigned long lis_debug_mask = 0;
EXPORT_SYMBOL_GPL(lis_debug_mask);
void *lis_vmalloc(unsigned long size)
{
	return WARN(vmalloc(size));
}

EXPORT_SYMBOL_GPL(lis_vmalloc);
void lis_bprintf(char *fmt, ...)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_bprintf);
void lis_cmn_err(int err_lvl, char *fmt, ...)
{
	extern int vcmn_err(int err_lvl, const char *fmt, va_list args);
	va_list args;
	va_start(args, fmt);
	vcmn_err(err_lvl, fmt, args);
	va_end(args);
}

EXPORT_SYMBOL_GPL(lis_cmn_err);
void lis_dobufcall(int cpu_id)
{
	/* bufcalls will be processed by the LfS executive when necessary. */
	return;
}

EXPORT_SYMBOL_GPL(lis_dobufcall);
void lis_enable_intr(struct streamtab *strtab, int major, const char *name)
{
	/* don't do this */
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_enable_intr);
void lis_fifo_close_sync(struct inode *i, struct file *f)
{
	/* don't do this */
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_fifo_close_sync);
void lis_flush_print_buffer(void)
{
	/* print buffers will be processed by kernel. */
	return;
}

EXPORT_SYMBOL_GPL(lis_flush_print_buffer);
void lis_free(void *ptr, char *file_name, int line_nr)
{
	kfree(ptr);
}

EXPORT_SYMBOL_GPL(lis_free);
void lis_freedb(mblk_t *bp, int free_hdr)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_freedb);
void lis_init_bufcall(void)
{
	return;
}

EXPORT_SYMBOL_GPL(lis_init_bufcall);
void lis_mark_mem(void *ptr, const char *file_name, int line_nr)
{
	return;
}

EXPORT_SYMBOL_GPL(lis_mark_mem);
void lis_print_block(void *ptr)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_block);
void lis_print_data(mblk_t *mp, int opt, int cont)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_data);
void lis_print_mem(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_mem);
void lis_print_msg(mblk_t *mp, const char *prefix, int opt)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_msg);
void lis_print_queue(queue_t *q)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_queue);
void lis_print_queues(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_queues);
void lis_print_spl_track(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_spl_track);
void lis_print_stream(stdata_t *hd)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_print_stream);
void lis_set_file_str(struct file *f, struct stdata *s)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_set_file_str);
void lis_setqsched(int can_call)
{
	WARN(setqsched());
}

EXPORT_SYMBOL_GPL(lis_setqsched);
void lis_spl0_fcn(char *file, int line)
{
	WARNF(local_irq_enable(), file, line);
}

EXPORT_SYMBOL_GPL(lis_spl0_fcn);
volatile unsigned long lis_queuerun_cnts[NR_CPUS] = { 0, };
EXPORT_SYMBOL_GPL(lis_queuerun_cnts);
volatile unsigned long lis_runq_cnts[NR_CPUS] = { 0, };
EXPORT_SYMBOL_GPL(lis_runq_cnts);
volatile unsigned long lis_setqsched_cnts[NR_CPUS] = { 0, };
EXPORT_SYMBOL_GPL(lis_setqsched_cnts);
volatile unsigned long lis_setqsched_isr_cnts[NR_CPUS] = { 0, };
EXPORT_SYMBOL_GPL(lis_setqsched_isr_cnts);

mblk_t *lis_get_passfp(void)
{
	return allocb(sizeof(struct strrecvfd), BPRI_HI);
}

EXPORT_SYMBOL_GPL(lis_get_passfp);
void lis_free_passfp(mblk_t *mp)
{
	freemsg(mp);
}

EXPORT_SYMBOL_GPL(lis_free_passfp);
int lis_recvfd(stdata_t *recvhd, strrecvfd_t * recv, struct file *fp)
{
	return (-EOPNOTSUPP);
}

EXPORT_SYMBOL_GPL(lis_recvfd);
int lis_sendfd(stdata_t *sendhd, unsigned int fd, struct file *fp)
{
	return (-EOPNOTSUPP);
}

EXPORT_SYMBOL_GPL(lis_sendfd);

/* 
 *  This is the absurd list.  If Dave thinks that LiS can change Linux GPL to
 *  LGPL in this fashion, he's nuts...  They are all bugged out so that you
 *  know when a module is using one and can change it.
 */
#ifdef CONFIG_PCI
dma64_addr_t lis_osif_pci_dac_page_to_dma(struct pci_dev *pdev, struct page *page,
					  unsigned long offset, int direction)
{
	return WARN(pci_dac_page_to_dma(pdev, page, offset, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dac_page_to_dma);
dma_addr_t lis_osif_pci_map_page(struct pci_dev *hwdev, struct page *page, unsigned long offset,
				 size_t size, int direction)
{
	return WARN(pci_map_page(hwdev, page, offset, size, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_map_page);
dma_addr_t lis_osif_pci_map_single(struct pci_dev *hwdev, void *ptr, size_t size, int direction)
{
	return WARN(pci_map_single(hwdev, ptr, size, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_map_single);
dma_addr_t lis_osif_sg_dma_address(struct scatterlist *sg)
{
	return WARN(sg_dma_address(sg));
}

EXPORT_SYMBOL_GPL(lis_osif_sg_dma_address);
#endif
int lis_can_unload(void)
{
	return 1;
}

EXPORT_SYMBOL_GPL(lis_can_unload);
int lis_check_region(unsigned int from, unsigned int extent)
{
	return WARN(check_region(from, extent));
}

EXPORT_SYMBOL_GPL(lis_check_region);
int lis_check_umem(struct file *fp, int rd_wr_fcn, const void *usr_addr, int lgth)
{
	return WARN(verify_area(rd_wr_fcn, usr_addr, lgth));
}

EXPORT_SYMBOL_GPL(lis_check_umem);
int lis_del_timer(struct timer_list *timer)
{
	return WARN(del_timer(timer));
}

EXPORT_SYMBOL_GPL(lis_del_timer);

/* This one is just plain silly. */
int lis_getint(unsigned char **p)
{
	return *(((int *) (*p))++);
}

EXPORT_SYMBOL_GPL(lis_getint);
int lis_kernel_down(struct semaphore *sem)
{
	return WARN(down_interruptible(sem));
}

EXPORT_SYMBOL_GPL(lis_kernel_down);
int lis_kill_pg(int pgrp, int sig, int priv)
{
	return WARN(kill_pg(pgrp, sig, priv));
}

EXPORT_SYMBOL_GPL(lis_kill_pg);
int lis_kill_proc(int pid, int sig, int priv)
{
	return WARN(kill_proc(pid, sig, priv));
}

EXPORT_SYMBOL_GPL(lis_kill_proc);
int lis_loadable_load(const char *name)
{
	return WARN(request_module(name));
}

EXPORT_SYMBOL_GPL(lis_loadable_load);

int lis_num_cpus = NR_CPUS;
EXPORT_SYMBOL_GPL(lis_num_cpus);
#ifdef CONFIG_PCI
int lis_osif_pci_dac_dma_supported(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_dac_dma_supported(hwdev, mask));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_supported);
int lis_osif_pci_dac_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_dac_set_dma_mask(hwdev, mask));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dac_set_dma_mask);
int lis_osif_pci_dma_supported(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_dma_supported(hwdev, mask));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dma_supported);
int lis_osif_pci_enable_device(struct pci_dev *dev)
{
	return WARN(pci_enable_device(dev));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_enable_device);
int lis_osif_pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{
	return WARN(pci_map_sg(hwdev, sg, nents, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_map_sg);
int lis_osif_pci_module_init(void *p)
{
	return WARN(pci_module_init(p));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_module_init);
int lis_osif_pci_read_config_byte(struct pci_dev *dev, u8 where, u8 * val)
{
	return WARN(pci_read_config_byte(dev, where, val));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_read_config_byte);
int lis_osif_pci_read_config_dword(struct pci_dev *dev, u8 where, u32 * val)
{
	return WARN(pci_read_config_dword(dev, where, val));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_read_config_dword);
int lis_osif_pci_read_config_word(struct pci_dev *dev, u8 where, u16 * val)
{
	return WARN(pci_read_config_word(dev, where, val));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_read_config_word);
int lis_osif_pci_set_dma_mask(struct pci_dev *hwdev, u64 mask)
{
	return WARN(pci_set_dma_mask(hwdev, mask));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_set_dma_mask);
int lis_osif_pci_write_config_byte(struct pci_dev *dev, u8 where, u8 val)
{
	return WARN(pci_write_config_byte(dev, where, val));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_write_config_byte);
int lis_osif_pci_write_config_dword(struct pci_dev *dev, u8 where, u32 val)
{
	return WARN(pci_write_config_dword(dev, where, val));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_write_config_dword);
int lis_osif_pci_write_config_word(struct pci_dev *dev, u8 where, u16 val)
{
	return WARN(pci_write_config_word(dev, where, val));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_write_config_word);
#endif

#ifdef CONFIG_PCI
const char *lis_pcibios_strerror(int error)
{
	return dont_use_this_function();
}

EXPORT_SYMBOL_GPL(lis_pcibios_strerror);
void lis_pcibios_init(void)
{
#ifdef HAVE_PCIBIOS_INIT_ADDR
	typeof(&pcibios_init) _pcibios_init = (typeof(_pcibios_init)) HAVE_PCIBIOS_INIT_ADDR;
	return WARN(_pcibios_init());
#else
	swerr();
#endif
}

EXPORT_SYMBOL_GPL(lis_pcibios_init);
int lis_pcibios_find_class(unsigned int class_code, unsigned short index, unsigned char *bus,
			   unsigned char *dev_fn)
{
	return WARN(pcibios_find_class(class_code, index, bus, dev_fn));
}

EXPORT_SYMBOL_GPL(lis_pcibios_find_class);
int lis_pcibios_find_device(unsigned short vendor, unsigned short dev_id, unsigned short index,
			    unsigned char *bus, unsigned char *dev_fn)
{
	return WARN(pcibios_find_device(vendor, dev_id, index, bus, dev_fn));
}

EXPORT_SYMBOL_GPL(lis_pcibios_find_device);
int lis_pcibios_present(void)
{
	return WARN(pcibios_present());
}

EXPORT_SYMBOL_GPL(lis_pcibios_present);
int lis_pcibios_read_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
				 unsigned char *val)
{
	return WARN(pcibios_read_config_byte(bus, dev_fn, where, val));
}

EXPORT_SYMBOL_GPL(lis_pcibios_read_config_byte);
int lis_pcibios_read_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
				  unsigned int *val)
{
	return WARN(pcibios_read_config_dword(bus, dev_fn, where, val));
}

EXPORT_SYMBOL_GPL(lis_pcibios_read_config_dword);
int lis_pcibios_read_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
				 unsigned short *val)
{
	return WARN(pcibios_read_config_word(bus, dev_fn, where, val));
}

EXPORT_SYMBOL_GPL(lis_pcibios_read_config_word);
int lis_pcibios_write_config_byte(unsigned char bus, unsigned char dev_fn, unsigned char where,
				  unsigned char val)
{
	return WARN(pcibios_write_config_byte(bus, dev_fn, where, val));
}

EXPORT_SYMBOL_GPL(lis_pcibios_write_config_byte);
int lis_pcibios_write_config_dword(unsigned char bus, unsigned char dev_fn, unsigned char where,
				   unsigned int val)
{
	return WARN(pcibios_write_config_dword(bus, dev_fn, where, val));
}

EXPORT_SYMBOL_GPL(lis_pcibios_write_config_dword);
int lis_pcibios_write_config_word(unsigned char bus, unsigned char dev_fn, unsigned char where,
				  unsigned short val)
{
	return WARN(pcibios_write_config_word(bus, dev_fn, where, val));
}

EXPORT_SYMBOL_GPL(lis_pcibios_write_config_word);
#endif

int lis_request_dma(unsigned int dma_nr, const char *device_id)
{
	return WARN(request_dma(dma_nr, device_id));
}

EXPORT_SYMBOL_GPL(lis_request_dma);
int lis_request_irq(unsigned int irq, void (*handler) (int, void *, struct pt_regs *),
		    unsigned long flags, const char *device, void *dev_id)
{
	return WARN(request_irq(irq, handler, flags, device, dev_id));
}

EXPORT_SYMBOL_GPL(lis_request_irq);
int lis_sprintf(char *bfr, const char *fmt, ...)
{
	va_list args;
	int ret;
	va_start(args, fmt);
	ret = WARN(vsprintf(bfr, fmt, args));
	va_end(args);
	return ret;
}

EXPORT_SYMBOL_GPL(lis_sprintf);
int lis_thread_stop(pid_t pid)
{
	return WARN(kill_proc(pid, SIGTERM, 1));
}

EXPORT_SYMBOL_GPL(lis_thread_stop);
int lis_vsprintf(char *bfr, const char *fmt, va_list args)
{
	return WARN(vsprintf(bfr, fmt, args));
}

EXPORT_SYMBOL_GPL(lis_vsprintf);

#ifdef CONFIG_PCI
size_t lis_osif_sg_dma_len(struct scatterlist *sg)
{
	return WARN(sg_dma_len(sg));
}

EXPORT_SYMBOL_GPL(lis_osif_sg_dma_len);
#endif
struct inode *lis_file_inode(struct file *f)
{
	return f->f_dentry->d_inode;
}

EXPORT_SYMBOL_GPL(lis_file_inode);

#ifdef CONFIG_PCI
struct page *lis_osif_pci_dac_dma_to_page(struct pci_dev *pdev, dma64_addr_t dma_addr)
{
	return WARN(pci_dac_dma_to_page(pdev, dma_addr));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_to_page);
struct pci_dev *lis_osif_pci_find_class(unsigned int class, struct pci_dev *from)
{
	return WARN(pci_find_class(class, from));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_find_class);
struct pci_dev *lis_osif_pci_find_device(unsigned int vendor, unsigned int device,
					 struct pci_dev *from)
{
	return WARN(pci_find_device(vendor, device, from));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_find_device);
struct pci_dev *lis_osif_pci_find_slot(unsigned int bus, unsigned int devfn)
{
	return WARN(pci_find_slot(bus, devfn));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_find_slot);
#endif
struct stdata *lis_file_str(struct file *f)
{
	return WARN((struct stdata *) f->f_dentry->d_inode->i_pipe);
}

EXPORT_SYMBOL_GPL(lis_file_str);
unsigned lis_usectohz(unsigned usec)
{
	return WARN(usec / (1000000 / HZ));
}

EXPORT_SYMBOL_GPL(lis_usectohz);
unsigned long lis_dsecs(void)
{
	struct timeval tv;
	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec * 10L) + (tv.tv_usec / 100000L);
}

EXPORT_SYMBOL_GPL(lis_dsecs);
unsigned long lis_hitime(void)
{
	struct timeval tv;
	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec & 0x3f) * 1000000 + tv.tv_usec;
}

EXPORT_SYMBOL_GPL(lis_hitime);
unsigned long lis_jiffies(void)
{
	return WARN(jiffies);
}

EXPORT_SYMBOL_GPL(lis_jiffies);
unsigned long lis_msecs(void)
{
	struct timeval tv;
	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec * 1000L) + (tv.tv_usec / 1000L);
}

EXPORT_SYMBOL_GPL(lis_msecs);

#ifdef CONFIG_PCI
unsigned long lis_osif_pci_dac_dma_to_offset(struct pci_dev *pdev, dma64_addr_t dma_addr)
{
	return pci_dac_dma_to_offset(pdev, dma_addr);
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_to_offset);
#endif
unsigned long lis_secs(void)
{
	struct timeval tv;
	WARN(do_gettimeofday(&tv));
	return tv.tv_sec;
}

EXPORT_SYMBOL_GPL(lis_secs);
unsigned long lis_usecs(void)
{
	struct timeval tv;
	WARN(do_gettimeofday(&tv));
	return (tv.tv_sec * 1000000L) + tv.tv_usec;
}

EXPORT_SYMBOL_GPL(lis_usecs);
unsigned long lis_virt_to_phys(volatile void *addr)
{
	return WARN(virt_to_phys(addr));
}

EXPORT_SYMBOL_GPL(lis_virt_to_phys);
void *lis__kfree(void *ptr)
{
	WARN(kfree(ptr));
	return NULL;
}

EXPORT_SYMBOL_GPL(lis__kfree);
void *lis__kmalloc(int nbytes, int class, int use_cache)
{
	return WARN(kmalloc(nbytes, class));
}

EXPORT_SYMBOL_GPL(lis__kmalloc);
void *lis_alloc_atomic_fcn(int nbytes, char *file, int line)
{
	return WARNF(kmalloc(nbytes, GFP_ATOMIC), file, line);
}

EXPORT_SYMBOL_GPL(lis_alloc_atomic_fcn);
void *lis_alloc_dma_fcn(int nbytes, char *file, int line)
{
	return WARNF(kmalloc(nbytes, GFP_DMA), file, line);
}

EXPORT_SYMBOL_GPL(lis_alloc_dma_fcn);
void *lis_alloc_kernel_fcn(int nbytes, char *file, int line)
{
	return WARNF(kmalloc(nbytes, GFP_KERNEL), file, line);
}

EXPORT_SYMBOL_GPL(lis_alloc_kernel_fcn);
void *lis_free_mem_fcn(void *mem_area, char *file, int line)
{
	WARNF(kfree(mem_area), file, line);
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_free_mem_fcn);
void *lis_ioremap(unsigned long offset, unsigned long size)
{
	return WARN(ioremap(offset, size));
}

EXPORT_SYMBOL_GPL(lis_ioremap);
void *lis_ioremap_nocache(unsigned long offset, unsigned long size)
{
	return WARN(ioremap_nocache(offset, size));
}

EXPORT_SYMBOL_GPL(lis_ioremap_nocache);
void *lis_kmalloc(size_t nbytes, int type)
{
	return WARN(kmalloc(nbytes, type));
}

EXPORT_SYMBOL_GPL(lis_kmalloc);
void *lis_malloc(int nbytes, int class, int use_cache, char *file_name, int line_nr)
{
	return WARN(kmalloc(nbytes, class));
}

EXPORT_SYMBOL_GPL(lis_malloc);

#ifdef CONFIG_PCI
void *lis_osif_pci_alloc_consistent(struct pci_dev *hwdev, size_t size, dma_addr_t *dma_handle)
{
	return WARN(pci_alloc_consistent(hwdev, size, dma_handle));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_alloc_consistent);
#endif
void *lis_phys_to_virt(unsigned long addr)
{
	return WARN(phys_to_virt(addr));
}

EXPORT_SYMBOL_GPL(lis_phys_to_virt);
void *lis_vremap(unsigned long offset, unsigned long size)
{
	return WARN(ioremap_nocache(offset, size));
}

EXPORT_SYMBOL_GPL(lis_vremap);
void *lis_zmalloc(int nbytes, int class, char *file_name, int line_nr)
{
	return WARN(kmem_zalloc(nbytes, (class & __GFP_WAIT) ? KM_SLEEP : KM_NOSLEEP));
}

EXPORT_SYMBOL_GPL(lis_zmalloc);
void lis_add_timer(struct timer_list *timer)
{
	return WARN(add_timer(timer));
}

EXPORT_SYMBOL_GPL(lis_add_timer);
void lis_assert_fail(const char *expr, const char *objname, const char *file, unsigned int line)
{
	return (void) printk(KERN_CRIT "%s: assert(%s) failed in file %s, line %u\n", objname, expr,
			     file, line);
}

EXPORT_SYMBOL_GPL(lis_assert_fail);
void lis_dec_mod_cnt_fcn(const char *file, int line)
{
	WARNF(MOD_DEC_USE_COUNT, file, line);
	return;
}

EXPORT_SYMBOL_GPL(lis_dec_mod_cnt_fcn);
void lis_disable_irq(unsigned int irq)
{
	return WARN(disable_irq(irq));
}

EXPORT_SYMBOL_GPL(lis_disable_irq);
void lis_enable_irq(unsigned int irq)
{
	return WARN(enable_irq(irq));
}

EXPORT_SYMBOL_GPL(lis_enable_irq);
void lis_free_dma(unsigned int dma_nr)
{
	return WARN(free_dma(dma_nr));
}

EXPORT_SYMBOL_GPL(lis_free_dma);
void lis_free_irq(unsigned int irq, void *dev_id)
{
	return WARN(free_irq(irq, dev_id));
}

EXPORT_SYMBOL_GPL(lis_free_irq);
void lis_gettimeofday(struct timeval *tv)
{
	return WARN(do_gettimeofday(tv));
}

EXPORT_SYMBOL_GPL(lis_gettimeofday);
void lis_inc_mod_cnt_fcn(const char *file, int line)
{
	WARNF(MOD_INC_USE_COUNT, file, line);
	return;
}

EXPORT_SYMBOL_GPL(lis_inc_mod_cnt_fcn);
void lis_interruptible_sleep_on(wait_queue_head_t *wq)
{
	return WARN(interruptible_sleep_on(wq));
}

EXPORT_SYMBOL_GPL(lis_interruptible_sleep_on);
void lis_kernel_up(struct semaphore *sem)
{
	return WARN(up(sem));
}

EXPORT_SYMBOL_GPL(lis_kernel_up);
void lis_kfree(const void *ptr)
{
	return WARN(kfree(ptr));
}

EXPORT_SYMBOL_GPL(lis_kfree);
void lis_iounmap(void *ptr)
{
	return WARN(iounmap(ptr));
}

EXPORT_SYMBOL_GPL(lis_iounmap);
void lis_osif_cli(void)
{
	WARN(cli());
	return;
}

EXPORT_SYMBOL_GPL(lis_osif_cli);
void lis_osif_do_gettimeofday(struct timeval *tp)
{
	return WARN(do_gettimeofday(tp));
}

EXPORT_SYMBOL_GPL(lis_osif_do_gettimeofday);
void lis_osif_do_settimeofday(struct timeval *tp)
{
	return WARN(do_settimeofday(tp));
}

EXPORT_SYMBOL_GPL(lis_osif_do_settimeofday);

#ifdef CONFIG_PCI
void lis_osif_pci_dac_dma_sync_single(struct pci_dev *pdev, dma64_addr_t dma_addr, size_t len,
				      int direction)
{
	return WARN(pci_dac_dma_sync_single(pdev, dma_addr, len, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dac_dma_sync_single);
void lis_osif_pci_disable_device(struct pci_dev *dev)
{
	return WARN(pci_disable_device(dev));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_disable_device);
void lis_osif_pci_dma_sync_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nelems,
			      int direction)
{
	return WARN(pci_dma_sync_sg(hwdev, sg, nelems, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dma_sync_sg);
void lis_osif_pci_dma_sync_single(struct pci_dev *hwdev, dma_addr_t dma_handle, size_t size,
				  int direction)
{
	return WARN(pci_dma_sync_single(hwdev, dma_handle, size, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_dma_sync_single);
void lis_osif_pci_free_consistent(struct pci_dev *hwdev, size_t size, void *vaddr,
				  dma_addr_t dma_handle)
{
	return WARN(pci_free_consistent(hwdev, size, vaddr, dma_handle));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_free_consistent);
void lis_osif_pci_set_master(struct pci_dev *dev)
{
	return WARN(pci_set_master(dev));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_set_master);
void lis_osif_pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address, size_t size,
			     int direction)
{
	return WARN(pci_unmap_page(hwdev, dma_address, size, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_unmap_page);
void lis_osif_pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg, int nents, int direction)
{
	return WARN(pci_unmap_sg(hwdev, sg, nents, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_unmap_sg);
void lis_osif_pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, size_t size,
			       int direction)
{
	return WARN(pci_unmap_single(hwdev, dma_addr, size, direction));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_unmap_single);
void lis_osif_pci_unregister_driver(struct pci_driver *p)
{
	return WARN(pci_unregister_driver(p));
}

EXPORT_SYMBOL_GPL(lis_osif_pci_unregister_driver);
#endif
void lis_osif_sti(void)
{
	WARN(sti());
	return;
}

EXPORT_SYMBOL_GPL(lis_osif_sti);
void lis_putbyte(unsigned char **p, unsigned char byte)
{
	*(*p)++ = byte;
	return;
}

EXPORT_SYMBOL_GPL(lis_putbyte);
void lis_release_region(unsigned int from, unsigned int extent)
{
	return WARN(release_region(from, extent));
}

EXPORT_SYMBOL_GPL(lis_release_region);
void lis_request_region(unsigned int from, unsigned int extent, const char *name)
{
	return (void) WARN(request_region(from, extent, name));
}

EXPORT_SYMBOL_GPL(lis_request_region);
void lis_sleep_on(wait_queue_head_t *wq)
{
	return WARN(sleep_on(wq));
}

EXPORT_SYMBOL_GPL(lis_sleep_on);
void lis_udelay(long micro_secs)
{
	return WARN(udelay(micro_secs));
}

EXPORT_SYMBOL_GPL(lis_udelay);
void lis_vfree(void *ptr)
{
	return WARN(vfree(ptr));
}

EXPORT_SYMBOL_GPL(lis_vfree);
void lis_wake_up(wait_queue_head_t *wq)
{
	return WARN(wake_up(wq));
}

EXPORT_SYMBOL_GPL(lis_wake_up);
void lis_wake_up_interruptible(wait_queue_head_t *wq)
{
	return WARN(wake_up_interruptible(wq));
}

EXPORT_SYMBOL_GPL(lis_wake_up_interruptible);
int lis_splstr_fcn(char *file, int line)
{
	unsigned long flags;
	WARNF(local_irq_save(flags), file, line);
	return flags;
}

EXPORT_SYMBOL_GPL(lis_splstr_fcn);
void lis_splx_fcn(int x, char *file, int line)
{
	unsigned long flags = x;
	WARNF(local_irq_restore(flags), file, line);
	return;
}

EXPORT_SYMBOL_GPL(lis_splx_fcn);

/* 
 *  No idea what to do with these.  LiS has some strange concepts of PCI
 *  devices.  Use the Linux kernel facilities directly please.
 */
#ifdef CONFIG_PCI
lis_pci_dev_t *lis_pci_find_class(unsigned class, lis_pci_dev_t *previous_struct)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_pci_find_class);
lis_pci_dev_t *lis_pci_find_device(unsigned vendor, unsigned device, lis_pci_dev_t *previous_struct)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_pci_find_device);
lis_pci_dev_t *lis_pci_find_slot(unsigned bus, unsigned dev_fcn)
{
	swerr();
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_pci_find_slot);
void lis_pci_cleanup(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_pci_cleanup);
void lis_pci_disable_device(lis_pci_dev_t *dev)
{
	WARN(pci_disable_device(dev->kern_ptr));
	return;
}

EXPORT_SYMBOL_GPL(lis_pci_disable_device);
void lis_pci_set_master(lis_pci_dev_t *dev)
{
	WARN(pci_set_master(dev->kern_ptr));
	return;
}

EXPORT_SYMBOL_GPL(lis_pci_set_master);
int lis_pci_enable_device(lis_pci_dev_t *dev)
{
	return WARN(pci_enable_device(dev->kern_ptr));
}

EXPORT_SYMBOL_GPL(lis_pci_enable_device);
int lis_pci_read_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char *rtn_val)
{
	return WARN(pci_read_config_byte(dev->kern_ptr, index, rtn_val));
}

EXPORT_SYMBOL_GPL(lis_pci_read_config_byte);
int lis_pci_read_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long *rtn_val)
{
	return WARN(pci_read_config_dword(dev->kern_ptr, index, (u32 *) rtn_val));
}

EXPORT_SYMBOL_GPL(lis_pci_read_config_dword);
int lis_pci_read_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short *rtn_val)
{
	return WARN(pci_read_config_word(dev->kern_ptr, index, rtn_val));
}

EXPORT_SYMBOL_GPL(lis_pci_read_config_word);
int lis_pci_write_config_byte(lis_pci_dev_t *dev, unsigned index, unsigned char val)
{
	return WARN(pci_write_config_byte(dev->kern_ptr, index, val));
}

EXPORT_SYMBOL_GPL(lis_pci_write_config_byte);
int lis_pci_write_config_dword(lis_pci_dev_t *dev, unsigned index, unsigned long val)
{
	return WARN(pci_write_config_dword(dev->kern_ptr, index, val));
}

EXPORT_SYMBOL_GPL(lis_pci_write_config_dword);
int lis_pci_write_config_word(lis_pci_dev_t *dev, unsigned index, unsigned short val)
{
	return WARN(pci_write_config_word(dev->kern_ptr, index, val));
}

EXPORT_SYMBOL_GPL(lis_pci_write_config_word);
#endif

/* 
 *  LiS weighty spin locks.  We just use a normal kernel spin lock
 *  embedded in the oversized LiS structure.
 */
int lis_spin_is_locked_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	return WARNF(spin_is_locked((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_is_locked_fcn);
int lis_spin_trylock_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	return WARNF(spin_trylock((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_trylock_fcn);
lis_spin_lock_t *lis_spin_lock_alloc_fcn(const char *name, char *file, int line)
{
	lis_spin_lock_t *lock;
	if ((lock = WARNF(kmem_zalloc(sizeof(*lock), KM_NOSLEEP), file, line)))
		spin_lock_init((spinlock_t *) lock->spin_lock_mem);
	return lock;
}

EXPORT_SYMBOL_GPL(lis_spin_lock_alloc_fcn);
lis_spin_lock_t *lis_spin_lock_free_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	unsigned long flags;
	spin_lock_irqsave((spinlock_t *) lock->spin_lock_mem, flags);
	WARNF(kmem_free((void *) lock, sizeof(*lock)), file, line);
	local_irq_restore(flags);
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_spin_lock_free_fcn);
void lis_spin_lock_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_lock((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_lock_fcn);
void lis_spin_lock_init_fcn(lis_spin_lock_t *lock, const char *name, char *file, int line)
{
	WARNF(spin_lock_init((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_lock_init_fcn);
void lis_spin_lock_irq_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_lock_irq((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_lock_irq_fcn);
void lis_spin_lock_irqsave_fcn(lis_spin_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags;
	WARNF(spin_lock_irqsave((spinlock_t *) lock->spin_lock_mem, flags), file, line);
	*flagp = flags;
}

EXPORT_SYMBOL_GPL(lis_spin_lock_irqsave_fcn);
void lis_spin_unlock_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_unlock((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_unlock_fcn);
void lis_spin_unlock_irq_fcn(lis_spin_lock_t *lock, char *file, int line)
{
	WARNF(spin_unlock_irq((spinlock_t *) lock->spin_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_unlock_irq_fcn);
void lis_spin_unlock_irqrestore_fcn(lis_spin_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = *flagp;
	WARNF(spin_unlock_irqrestore((spinlock_t *) lock->spin_lock_mem, flags), file, line);
}

EXPORT_SYMBOL_GPL(lis_spin_unlock_irqrestore_fcn);

/* 
 *  LiS weighty read write locks.  We just use a normal kernel read write lock
 *  embedded in the oversized LiS structure.
 */
lis_rw_lock_t *lis_rw_lock_alloc_fcn(const char *name, char *file, int line)
{
	lis_rw_lock_t *lock;
	if ((lock = WARNF(kmem_zalloc(sizeof(*lock), KM_NOSLEEP), file, line)))
		rwlock_init((rwlock_t *) lock->rw_lock_mem);
	return lock;
}

EXPORT_SYMBOL_GPL(lis_rw_lock_alloc_fcn);
lis_rw_lock_t *lis_rw_lock_free_fcn(lis_rw_lock_t *lock, const char *name, char *file, int line)
{
	unsigned long flags;
	write_lock_irqsave((rwlock_t *) lock->rw_lock_mem, flags);
	WARNF(kmem_free((void *) lock, sizeof(*lock)), file, line);
	local_irq_restore(flags);
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_rw_lock_free_fcn);
void lis_rw_lock_init_fcn(lis_rw_lock_t *lock, const char *name, char *file, int line)
{
	WARNF(rwlock_init((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_lock_init_fcn);
void lis_rw_read_lock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_lock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_read_lock_fcn);
void lis_rw_read_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_lock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_read_lock_irq_fcn);
void lis_rw_read_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags;
	WARNF(read_lock_irqsave((rwlock_t *) lock->rw_lock_mem, flags), file, line);
	*flagp = flags;
}

EXPORT_SYMBOL_GPL(lis_rw_read_lock_irqsave_fcn);
void lis_rw_read_unlock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_unlock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_read_unlock_fcn);
void lis_rw_read_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(read_unlock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_read_unlock_irq_fcn);
void lis_rw_read_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = *flagp;
	WARNF(read_unlock_irqrestore((rwlock_t *) lock->rw_lock_mem, flags), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_read_unlock_irqrestore_fcn);
void lis_rw_write_lock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_lock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_write_lock_fcn);
void lis_rw_write_lock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_lock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_write_lock_irq_fcn);
void lis_rw_write_lock_irqsave_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags;
	WARNF(write_lock_irqsave((rwlock_t *) lock->rw_lock_mem, flags), file, line);
	*flagp = flags;
}

EXPORT_SYMBOL_GPL(lis_rw_write_lock_irqsave_fcn);
void lis_rw_write_unlock_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_unlock((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_write_unlock_fcn);
void lis_rw_write_unlock_irq_fcn(lis_rw_lock_t *lock, char *file, int line)
{
	WARNF(write_unlock_irq((rwlock_t *) lock->rw_lock_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_write_unlock_irq_fcn);
void lis_rw_write_unlock_irqrestore_fcn(lis_rw_lock_t *lock, int *flagp, char *file, int line)
{
	unsigned long flags = *flagp;
	WARNF(write_unlock_irqrestore((rwlock_t *) lock->rw_lock_mem, flags), file, line);
}

EXPORT_SYMBOL_GPL(lis_rw_write_unlock_irqrestore_fcn);

/* 
 *  LiS weighty atomic wrappers.  We just use the normal kernel atomic types
 *  inside the LiS type.
 */
lis_atomic_t lis_in_syscall = 0;
EXPORT_SYMBOL_GPL(lis_in_syscall);
lis_atomic_t lis_open_cnt = 0;
EXPORT_SYMBOL_GPL(lis_open_cnt);
lis_atomic_t lis_queues_running = 0;
EXPORT_SYMBOL_GPL(lis_queues_running);
lis_atomic_t lis_runq_req_cnt = 0;
EXPORT_SYMBOL_GPL(lis_runq_req_cnt);
lis_atomic_t lis_stdata_cnt = 0;
EXPORT_SYMBOL_GPL(lis_stdata_cnt);
lis_atomic_t lis_strcount = 0;
EXPORT_SYMBOL_GPL(lis_strcount);
lis_atomic_t lis_strstats[24][4] = { {0,}, };
EXPORT_SYMBOL_GPL(lis_strstats);
void lis_atomic_add(lis_atomic_t *atomic_addr, int amt)
{
	return WARN(atomic_add(amt, (atomic_t *) atomic_addr));
}

EXPORT_SYMBOL_GPL(lis_atomic_add);
void lis_atomic_dec(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_dec((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL_GPL(lis_atomic_dec);
void lis_atomic_inc(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_inc((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL_GPL(lis_atomic_inc);
void lis_atomic_set(lis_atomic_t *atomic_addr, int valu)
{
	return (void) WARN(atomic_set((atomic_t *) atomic_addr, valu));
}

EXPORT_SYMBOL_GPL(lis_atomic_set);
void lis_atomic_sub(lis_atomic_t *atomic_addr, int amt)
{
	return WARN(atomic_sub(amt, (atomic_t *) atomic_addr));
}

EXPORT_SYMBOL_GPL(lis_atomic_sub);
int lis_atomic_dec_and_test(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_dec_and_test((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL_GPL(lis_atomic_dec_and_test);
int lis_atomic_read(lis_atomic_t *atomic_addr)
{
	return WARN(atomic_read((atomic_t *) atomic_addr));
}

EXPORT_SYMBOL_GPL(lis_atomic_read);

/* 
 *  LiS weighty semaphore wrappers.  We just use a normal kernel semaphore
 *  embedded in the oversized LiS structure (sem_mem area).
 */
int lis_down_fcn(lis_semaphore_t *lsem, char *file, int line)
{
	return WARNF(down_interruptible((struct semaphore *) lsem->sem_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_down_fcn);
lis_semaphore_t *lis_sem_alloc(int count)
{
	lis_semaphore_t *lsem;
	lsem = WARN(kmem_zalloc(sizeof(lis_semaphore_t), KM_NOSLEEP));
	if (lsem != NULL)
		init_MUTEX((struct semaphore *) lsem->sem_mem);
	return lsem;
}

EXPORT_SYMBOL_GPL(lis_sem_alloc);
lis_semaphore_t *lis_sem_destroy(lis_semaphore_t *lsem)
{
	WARN(kmem_free((void *) lsem, sizeof(*lsem)));
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_sem_destroy);
void lis_sem_init(lis_semaphore_t *lsem, int count)
{
	return WARN(sema_init((struct semaphore *) lsem->sem_mem, count));
}

EXPORT_SYMBOL_GPL(lis_sem_init);
void lis_up_fcn(lis_semaphore_t *lsem, char *file, int line)
{
	return WARNF(up((struct semaphore *) lsem->sem_mem), file, line);
}

EXPORT_SYMBOL_GPL(lis_up_fcn);

/* 
 *  These are bad, don't use them.
 */
void *lis_free_pages_fcn(void *ptr, char *file, int line)
{
	swerr();
	WARNF(free_pages((unsigned long) ptr, 0), file, line);
	return NULL;
}

EXPORT_SYMBOL_GPL(lis_free_pages_fcn);
void *lis_get_free_pages_atomic_fcn(int nbytes, char *file, int line)
{
	int order;
	for (order = 0, nbytes >>= PAGE_SHIFT; nbytes; order++, nbytes >>= 1) ;
	return (void *) WARNF(__get_free_pages(GFP_ATOMIC, order), file, line);
}

EXPORT_SYMBOL_GPL(lis_get_free_pages_atomic_fcn);
void *lis_get_free_pages_fcn(int nbytes, int class, char *file, int line)
{
	int order;
	for (order = 0, nbytes >>= PAGE_SHIFT; nbytes; order++, nbytes >>= 1) ;
	return (void *) WARNF(__get_free_pages(class, order), file, line);
}

EXPORT_SYMBOL_GPL(lis_get_free_pages_fcn);
void *lis_get_free_pages_kernel_fcn(int nbytes, char *file, int line)
{
	int order;
	for (order = 0, nbytes >>= PAGE_SHIFT; nbytes; order++, nbytes >>= 1) ;
	return (void *) WARNF(__get_free_pages(GFP_KERNEL, order), file, line);
}

EXPORT_SYMBOL_GPL(lis_get_free_pages_kernel_fcn);

/* 
 *  These are just wrappered system calls.
 */
int lis_mknod(char *name, int mode, dev_t dev)
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

EXPORT_SYMBOL_GPL(lis_mknod);
int lis_unlink(char *name)
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

EXPORT_SYMBOL_GPL(lis_unlink);
int lis_mount(char *dev_name, char *dir_name, char *fstype, unsigned long rwflag, void *data)
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

EXPORT_SYMBOL_GPL(lis_mount);
int lis_umount2(char *path, int flags)
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

EXPORT_SYMBOL_GPL(lis_umount2);
int lis_fattach(struct file *f, const char *path)
{
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
	long do_fattach(const struct file *f, const char *path);
	return WARN(do_fattach(f, path));
#else
	swerr();
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL_GPL(lis_fattach);
int lis_fdetach(const char *path)
{
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
	long do_fdetach(const char *path);
	return WARN(do_fdetach(path));
#else
	swerr();
	return (-ENOSYS);
#endif
}

EXPORT_SYMBOL_GPL(lis_fdetach);
int lis_pipe(unsigned int *fd)
{
	/* FIXME */
	extern int do_pipe(int *fd);
	return WARN(do_pipe(fd));
}

EXPORT_SYMBOL_GPL(lis_pipe);

void lis_fdetach_all(void)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_fdetach_all);
void lis_fdetach_stream(stdata_t *head)
{
	swerr();
	return;
}

EXPORT_SYMBOL_GPL(lis_fdetach_stream);

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
int lis_register_strdev(major_t major, struct streamtab *strtab, int nminor, const char *name)
{
	struct cdevsw *cdev;
	int err;
	if ((cdev = kmem_zalloc(sizeof(*cdev), KM_NOSLEEP)) == NULL)
		return (-ENOMEM);
	cdev->d_name = name;
	cdev->d_str = strtab;
	cdev->d_flag = D_MP | D_MTPERQ;	/* mark LiS compatibility for qopen and qclose */
	cdev->d_kmod = NULL;
	atomic_set(&cdev->d_count, 0);
	INIT_LIST_HEAD(&cdev->d_apush);
	if ((err = WARN(register_strdev(major, cdev))) < 0)
		kmem_free(cdev, sizeof(*cdev));
	return (err);
}

EXPORT_SYMBOL_GPL(lis_register_strdev);

/**
 *  lis_unregister_strdev - emulation of LiS STREAMS device deregistration
 *  @major:major number to deregister
 */
int lis_unregister_strdev(major_t major)
{
	struct cdevsw *cdev;
	int err;
	if (0 >= major || major >= MAX_CHRDEV)
		return (-EINVAL);
	if ((cdev = cdev_get(major)) == NULL)
		return (-ENOENT);
	cdev_put(cdev);
	/* we should be able to accept a cdev of NULL so that we don't need to export cdev_get and
	   cdev_put when liscomp is loaded as a module */
	if ((err = unregister_strdev(major, cdev)) == 0)
		kmem_free(cdev, sizeof(*cdev));
	return (err);
}

EXPORT_SYMBOL_GPL(lis_unregister_strdev);

/**
 *  lis_register_strmod - emulation of LiS STREAMS module registration
 *  @strtab:pointer to struct streamtab structure to register
 *  @name:the name of the module
 */
modID_t lis_register_strmod(struct streamtab *strtab, const char *name)
{
	struct fmodsw *fmod;
	int err;
	if ((fmod = kmem_zalloc(sizeof(*fmod), KM_NOSLEEP)) == NULL)
		return (-ENOMEM);
	/* this is ugly, we have to ignore "name" for deregistration */
	fmod->f_name = strtab->st_rdinit->qi_minfo->mi_idname;
	fmod->f_str = strtab;
	fmod->f_flag = D_MP | D_MTPERQ;	/* mark LiS compatibility for qopen and qclose */
	fmod->f_kmod = NULL;
	atomic_set(&fmod->f_count, 0);
	if ((err = register_strmod(fmod)) < 0)
		kmem_free(fmod, sizeof(*fmod));
	return (err);
}

EXPORT_SYMBOL_GPL(lis_register_strmod);

/**
 *  lis_unregister_strmod - emulation of LiS STREAMS module deregistration
 *  @strtab:point to struct streamtab structure to deregister
 */
int lis_unregister_strmod(struct streamtab *strtab)
{
	struct fmodsw *fmod;
	int err;
	/* this is ugly, we have to ignore "name" for deregistration */
	/* we should be able to accept a cdev of NULL so that we don't need to export fmod_find and
	   fmod_put when liscomp is loaded as a module */
	if ((fmod = fmod_find(strtab->st_rdinit->qi_minfo->mi_idname)) == NULL)
		return (-ENOENT);
	fmod_put(fmod);
	if ((err = WARN(unregister_strmod(fmod))) == 0)
		kmem_free(fmod, sizeof(*fmod));
	return (err);
}

EXPORT_SYMBOL_GPL(lis_unregister_strmod);

static int __init liscomp_init(void)
{
#ifdef MODULE
	printk(KERN_INFO LISCOMP_BANNER);
#else
	printk(KERN_INFO LISCOMP_SPLASH);
#endif
	return (0);
}
static void __exit liscomp_exit(void)
{
	return;
}

module_init(liscomp_init);
module_exit(liscomp_exit);
