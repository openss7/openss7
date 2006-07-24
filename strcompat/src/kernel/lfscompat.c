/*****************************************************************************

 @(#) $RCSfile: lfscompat.c,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2006/07/24 09:01:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2006/07/24 09:01:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lfscompat.c,v $
 Revision 0.9.2.23  2006/07/24 09:01:05  brian
 - results of udp2 optimizations

 Revision 0.9.2.22  2006/07/07 20:49:54  brian
 - change to correct LIS-only compile on FC5

 Revision 0.9.2.21  2006/06/22 13:11:32  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.20  2006/03/03 11:11:14  brian
 - 64-bit compatibility, fixes, updates for release

 Revision 0.9.2.19  2005/12/28 09:51:50  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.18  2005/12/22 10:28:53  brian
 - no symbol mangling for 2.4 kernels

 Revision 0.9.2.17  2005/12/19 12:44:41  brian
 - locking down for release

 Revision 0.9.2.16  2005/11/29 05:46:39  brian
 - straightened out register_strlog

 Revision 0.9.2.15  2005/11/28 18:55:57  brian
 - added register_strlog function

 Revision 0.9.2.14  2005/07/29 07:37:51  brian
 - changes to compile with latest streams package.

 Revision 0.9.2.13  2005/07/21 20:47:18  brian
 - sync with notebook

 Revision 0.9.2.12  2005/07/19 11:21:19  brian
 - updated strlog(9) for compatibility

 Revision 0.9.2.11  2005/07/18 12:25:42  brian
 - standard indentation

 Revision 0.9.2.10  2005/07/14 03:40:11  brian
 - updates for check pass

 Revision 0.9.2.9  2005/07/13 12:01:49  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 0.9.2.8  2005/07/12 13:54:45  brian
 - changes for os7 compatibility and check pass

 Revision 0.9.2.7  2005/07/12 08:42:42  brian
 - changes for check pass

 Revision 0.9.2.6  2005/07/09 21:51:21  brian
 - remove dependency on LFS headers

 Revision 0.9.2.5  2005/07/07 20:29:17  brian
 - changes for release

 Revision 0.9.2.4  2005/07/06 03:47:46  brian
 - minor corrections

 Revision 0.9.2.3  2005/07/05 22:46:05  brian
 - change for strcompat package

 Revision 0.9.2.2  2005/07/04 20:14:29  brian
 - fixed spelling of CVS keyword

 Revision 0.9.2.1  2005/07/04 19:29:16  brian
 - first cut at streams compatibility package

 *****************************************************************************/

#ident "@(#) $RCSfile: lfscompat.c,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2006/07/24 09:01:05 $"

static char const ident[] =
    "$RCSfile: lfscompat.c,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2006/07/24 09:01:05 $";

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

#define __LFS_EXTERN_INLINE __inline__ streamscall __unlikely

#define _LFS_SOURCE

#include "sys/os7/compat.h"

#include <sys/strlog.h>

#define LFSCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LFSCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define LFSCOMP_REVISION	"LfS $RCSfile: lfscompat.c,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2006/07/24 09:01:05 $"
#define LFSCOMP_DEVICE		"Linux Fast-STREAMS (LfS) 0.7a.3 Compatibility"
#define LFSCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define LFSCOMP_LICENSE		"GPL"
#define LFSCOMP_BANNER		LFSCOMP_DESCRIP	"\n" \
				LFSCOMP_COPYRIGHT	"\n" \
				LFSCOMP_REVISION	"\n" \
				LFSCOMP_DEVICE		"\n" \
				LFSCOMP_CONTACT	"\n"
#define LFSCOMP_SPLASH		LFSCOMP_DEVICE		" - " \
				LFSCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_LFS_MODULE
MODULE_AUTHOR(LFSCOMP_CONTACT);
MODULE_DESCRIPTION(LFSCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(LFSCOMP_DEVICE);
MODULE_LICENSE(LFSCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-lfscompat");
#endif
#endif

/*
 *  Functions missing from LiS:
 *
 *  alloclk, allocq, allocstr, autopush, autopush_add, autopush_del,
 *  autopush_find, autopush_vml, freelk, freeq, freestr, kmem_alloc_node,
 *  kmem_zalloc_node, qattach, qclose, qdetach, qopen, qpop, qpush, qready,
 *  sealloc, sefree, setq, setqsched
 *
 *  Of these, these six are still important:
 *
 *  __bufcall, __timeout, __strwrit, __strfunc, unweldq, weldq
 *
 */

/* These functions are missing from LiS, but in the core in LfS */

/* Strangely, LiS 2.18.0 defined lis_appq, but no longer appq */
__LFS_EXTERN_INLINE int appq(queue_t *q, mblk_t *emp, mblk_t *nmp);

EXPORT_SYMBOL_NOVERS(appq);

__LFS_EXTERN_INLINE int bcmp(const void *s1, const void *s2, size_t len);

EXPORT_SYMBOL_NOVERS(bcmp);

int
bcanget(queue_t *q, int band)
{
	int result;
	lis_flags_t flags;

	LIS_RDQISRLOCK(q, &flags);
	switch (band) {
		struct qband *qb;

	case 0:
		result = (q->q_first ? 1 : 0);
		break;
	case ANYBAND:
		for (qb = q->q_bandp; qb && qb->qb_first; qb = qb->qb_next) ;
		result = (qb ? qb->qb_first->b_band : 0);
		break;
	default:
		for (qb = q->q_bandp; qb && (!qb->qb_first || (qb->qb_first->b_band != band));
		     qb = qb->qb_next) ;
		result = (qb ? band : 0);
		break;
	}
	LIS_RDQISRUNLOCK(q, &flags);
	return (result);
}

EXPORT_SYMBOL_NOVERS(bcanget);

int
canget(queue_t *q)
{
	int result;
	lis_flags_t flags;

	LIS_QISRLOCK(q, &flags);
	if (!q->q_first) {
		if (!F_ISSET(q->q_flag, QWANTR)) {
			F_SET(q->q_flag, QWANTR);
			result = 2;
		} else
			result = 0;
	} else
		result = 1;
	LIS_QISRUNLOCK(q, &flags);
	if (result & 2)
		lis_backenable(q);
	return (result & 1);
}

EXPORT_SYMBOL_NOVERS(canget);

/* LiS 2.18.0 deprecated these for some reason... */
__LFS_EXTERN_INLINE int copyin(const void *from, void *to, size_t len);

EXPORT_SYMBOL_NOVERS(copyin);

__LFS_EXTERN_INLINE int copyout(const void *from, void *to, size_t len);

EXPORT_SYMBOL_NOVERS(copyout);

int
drv_getparm(const unsigned int parm, void *value_p)
{
	switch (parm) {
	case LBOLT:
		*(unsigned long *) value_p = jiffies;
		return (0);
	case PPGP:
#ifdef HAVE_KFUNC_PROCESS_GROUP
		*(pid_t *) value_p = process_group(current);
#else				/* HAVE_KFUNC_PROCESS_GROUP */
#ifdef HAVE_KMEMB_STRUCT_TASK_STRUCT_PGRP
		*(pid_t *) value_p = current->pgrp;
#else				/* HAVE_KMEMB_STRUCT_TASK_STRUCT_PGRP */
		*(pid_t *) value_p = current->signal->pgrp;
#endif				/* HAVE_KMEMB_STRUCT_TASK_STRUCT_PGRP */
#endif				/* HAVE_KFUNC_PROCESS_GROUP */
		return (0);
	case UPROCP:
		*(ulong *) value_p = (ulong) current->files;
		return (0);
	case PPID:
		*(pid_t *) value_p = current->pid;
		return (0);
	case PSID:
#ifdef HAVE_KMEMB_STRUCT_TASK_STRUCT_SESSION
		*(pid_t *) value_p = current->session;
#else				/* HAVE_KMEMB_STRUCT_TASK_STRUCT_SESSION */
		*(pid_t *) value_p = current->signal->session;
#endif				/* HAVE_KMEMB_STRUCT_TASK_STRUCT_SESSION */
		return (0);
	case TIME:
	{
		struct timeval tv;

		do_gettimeofday(&tv);
		*(time_t *) value_p = tv.tv_sec;
		return (0);
	}
	case UCRED:
		*(cred_t **) value_p = ((cred_t *)&current->uid);
		return (0);
	case STRMSGSIZE:
#ifdef LFS
		*(int *) value_p = sysctl_str_strmsgsz;
		return (0);
#else
		return (-1);
#endif
	case HW_PROVIDER:
#ifdef UTS_VERSION
		*(char **) value_p = "Linux " UTS_RELEASE " " UTS_VERSION;
#else
		*(char **) value_p = "Linux " UTS_RELEASE;
#endif
		return (0);
	default:
	case DRV_MAXBIOSIZE:
	case SYSCRED:
		/* not implemented */
		return (-1);
	}
}

EXPORT_SYMBOL_NOVERS(drv_getparm);

__LFS_EXTERN_INLINE int drv_priv(cred_t *crp);

EXPORT_SYMBOL_NOVERS(drv_priv);

__LFS_EXTERN_INLINE unsigned long drv_hztomsec(unsigned long hz);

EXPORT_SYMBOL_NOVERS(drv_hztomsec);

__LFS_EXTERN_INLINE unsigned long drv_hztousec(unsigned long hz);

EXPORT_SYMBOL_NOVERS(drv_hztousec);

__LFS_EXTERN_INLINE unsigned long drv_msectohz(unsigned long msec);

EXPORT_SYMBOL_NOVERS(drv_msectohz);

__LFS_EXTERN_INLINE unsigned long drv_usectohz(unsigned long usec);

EXPORT_SYMBOL_NOVERS(drv_usectohz);

__LFS_EXTERN_INLINE void drv_usecwait(unsigned long usec);

EXPORT_SYMBOL_NOVERS(drv_usecwait);

__LFS_EXTERN_INLINE void delay(unsigned long ticks);

EXPORT_SYMBOL_NOVERS(delay);

__LFS_EXTERN_INLINE int enableq(queue_t *q);

EXPORT_SYMBOL_NOVERS(enableq);

__LFS_EXTERN_INLINE qi_qadmin_t getadmin(modID_t modid);

EXPORT_SYMBOL_NOVERS(getadmin);

__LFS_EXTERN_INLINE modID_t getmid(const char *name);

EXPORT_SYMBOL_NOVERS(getmid);

__LFS_EXTERN_INLINE mblk_t *linkmsg(mblk_t *mp1, mblk_t *mp2);

EXPORT_SYMBOL_NOVERS(linkmsg);

__LFS_EXTERN_INLINE int pcmsg(unsigned char type);

EXPORT_SYMBOL_NOVERS(pcmsg);

__LFS_EXTERN_INLINE int datamsg(unsigned char type);

EXPORT_SYMBOL_NOVERS(datamsg);

__LFS_EXTERN_INLINE int ctlmsg(unsigned char type);

EXPORT_SYMBOL_NOVERS(ctlmsg);

__LFS_EXTERN_INLINE int isdatablk(dblk_t * db);

EXPORT_SYMBOL_NOVERS(isdatablk);

__LFS_EXTERN_INLINE int isdatamsg(mblk_t *mp);

EXPORT_SYMBOL_NOVERS(isdatamsg);

__LFS_EXTERN_INLINE int putctl(queue_t *q, int type);

EXPORT_SYMBOL_NOVERS(putctl);

__LFS_EXTERN_INLINE int putctl1(queue_t *q, int type, int param);

EXPORT_SYMBOL_NOVERS(putctl1);

__LFS_EXTERN_INLINE int putctl2(queue_t *q, int type, int param1, int param2);

EXPORT_SYMBOL_NOVERS(putctl2);

__LFS_EXTERN_INLINE int putnextctl(queue_t *q, int type);

EXPORT_SYMBOL_NOVERS(putnextctl);

__LFS_EXTERN_INLINE int putnextctl1(queue_t *q, int type, int param);

EXPORT_SYMBOL_NOVERS(putnextctl1);

__LFS_EXTERN_INLINE int putnextctl2(queue_t *q, int type, int param1, int param2);

EXPORT_SYMBOL_NOVERS(putnextctl2);

__LFS_EXTERN_INLINE void setq(queue_t *q, struct qinit *rinit, struct qinit *wrinit);

EXPORT_SYMBOL_NOVERS(setq);

static spinlock_t str_err_lock = SPIN_LOCK_UNLOCKED;
static char str_err_buf[LOGMSGSZ];

/*
 *  This is a default implementation for strlog(9).  When SL_CONSOLE is set, we print directly to
 *  the console using printk(9).  For SL_ERROR and SL_TRACE, we have no STREAMS error or trace
 *  loggers running, so we mark those messages as unseen by those loggers.  We also provide a hook
 *  here so that the strutil package can hook into this call.  Because we cannot filter, only
 *  SL_CONSOLE messages are printed to the system logs.  This follows the rules for setting the
 *  priority according described in log(4).
 */
static int
vstrlog_default(short mid, short sid, char level, unsigned short flag, char *fmt, va_list args)
{
	int rval = 1;

	if (flag & SL_CONSOLE) {
		unsigned long flags;
		short lev = (short) level;

		/* XXX: are these strict locks necessary? */
		spin_lock_irqsave(&str_err_lock, flags);
		vsnprintf(str_err_buf, sizeof(str_err_buf), fmt, args);
#define STRLOG_PFX "strlog(%hd)[%hd,%hd]: %s\n"
		if (flag & SL_FATAL)
			printk(KERN_CRIT STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_ERROR)
			printk(KERN_ERR STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_WARN)
			printk(KERN_WARNING STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_NOTE)
			printk(KERN_NOTICE STRLOG_PFX, lev, mid, sid, str_err_buf);
		else if (flag & SL_TRACE)
			printk(KERN_DEBUG STRLOG_PFX, lev, mid, sid, str_err_buf);
		else
			printk(KERN_INFO STRLOG_PFX, lev, mid, sid, str_err_buf);
#undef STRLOG_PFX
		spin_unlock_irqrestore(&str_err_lock, flags);
	}
	if (flag & SL_ERROR)
		rval = 0;	/* no error logger */
	if (flag & SL_TRACE)
		rval = 0;	/* no trace logger */
	return (rval);
}

static rwlock_t strlog_reg_lock = RW_LOCK_UNLOCKED;
static vstrlog_t vstrlog_hook = &vstrlog_default;

/**
 *  register_strlog:	- register a new STREAMS logger
 *  @newlog:	new vstrlog function pointer
 *
 *  DESCRIPTION: register_strlog() registers a new STREAMS logger callback function and returns the
 *  previous callback function.  Suitable locks are taken to protect module unloading.
 *
 *  CONTEXT: register_strlog() is intended to be called from a STREAMS module or driver qi_qopen() or
 *  qi_qclose() procedure.  It must be called from process context.
 *
 *  LOCKING: This function holds a write lock on strlog_reg_lock to keep others from calling a
 *  strlog() implementation function that is about to be unloaded for safe log driver unloading.
 */
vstrlog_t
register_strlog(vstrlog_t newlog)
{
	unsigned long flags;
	vstrlog_t oldlog;

	write_lock_irqsave(&strlog_reg_lock, flags);
	oldlog = xchg(&vstrlog_hook, newlog);
	write_unlock_irqrestore(&strlog_reg_lock, flags);
	return (oldlog);
}

EXPORT_SYMBOL_NOVERS(register_strlog);

/**
 *  vstrlog:	- log a STREAMS message
 *  @mid:	module id
 *  @sid:	stream id
 *  @level:	severity level
 *  @flag:	flags controlling distribution
 *  @fmt:	printf(3) format
 *  @args:	format specific arguments
 */
int
vstrlog(short mid, short sid, char level, unsigned short flag, char *fmt, va_list args)
{
	int result = 0;
	read_lock(&strlog_reg_lock);
	if (vstrlog_hook != NULL) {
		result = (*vstrlog_hook) (mid, sid, level, flag, fmt, args);
	}
	read_unlock(&strlog_reg_lock);
	return (result);
}

EXPORT_SYMBOL_NOVERS(vstrlog);

/**
 *  strlog:	- log a STREAMS message
 *  @mid:	module id
 *  @sid:	stream id
 *  @level:	severity level
 *  @flag:	flags controlling distribution
 *  @fmt:	printf(3) format
 *  @...:	format specific arguments
 *
 *  CONTEXT: strlog() can be called from any context, however, the caller should be aware that this
 *  function is complex and should only be called from in_interrupt() context sparingly.
 *
 *  LOCKING: This function holds a read lock on strlog_reg_lock to keep de-registrations from
 *  occurring while the function is being called for safe log driver unloading.
 */
int
strlog(short mid, short sid, char level, unsigned short flag, char *fmt, ...)
{
	int result = 0;

	read_lock(&strlog_reg_lock);
	if (vstrlog_hook != NULL) {
		va_list args;

		va_start(args, fmt);
		result = (*vstrlog_hook) (mid, sid, level, flag, fmt, args);
		va_end(args);
	}
	read_unlock(&strlog_reg_lock);
	return (result);
}

EXPORT_SYMBOL_NOVERS(strlog);

/*
 *  The following are from src/include/sys/lfs/strconf.h
 */

__LFS_EXTERN_INLINE int register_strnod(struct cdevsw *cdev, struct devnode *cmin, minor_t minor);

EXPORT_SYMBOL_NOVERS(register_strnod);

__LFS_EXTERN_INLINE int register_strdev(struct cdevsw *cdev, major_t major);

EXPORT_SYMBOL_NOVERS(register_strdev);

__LFS_EXTERN_INLINE int register_strdrv(struct cdevsw *cdev);

EXPORT_SYMBOL_NOVERS(register_strdrv);

__LFS_EXTERN_INLINE int register_strmod(struct _fmodsw *fmod);

EXPORT_SYMBOL_NOVERS(register_strmod);

__LFS_EXTERN_INLINE int unregister_strnod(struct cdevsw *cdev, minor_t minor);

EXPORT_SYMBOL_NOVERS(unregister_strnod);

__LFS_EXTERN_INLINE int unregister_strdev(struct cdevsw *cdev, major_t major);

EXPORT_SYMBOL_NOVERS(unregister_strdev);

__LFS_EXTERN_INLINE int unregister_strdrv(struct cdevsw *cdev);

EXPORT_SYMBOL_NOVERS(unregister_strdrv);

__LFS_EXTERN_INLINE int unregister_strmod(struct _fmodsw *fmod);

EXPORT_SYMBOL_NOVERS(unregister_strmod);

__LFS_EXTERN_INLINE int apush_get(struct strapush *sap);

EXPORT_SYMBOL_NOVERS(apush_get);

__LFS_EXTERN_INLINE int apush_set(struct strapush *sap);

EXPORT_SYMBOL_NOVERS(apush_set);

__LFS_EXTERN_INLINE int apush_vml(struct str_list *slp);

EXPORT_SYMBOL_NOVERS(apush_vml);

__LFS_EXTERN_INLINE int autopush_del(struct strapush *sap);

EXPORT_SYMBOL_NOVERS(autopush_del);

__LFS_EXTERN_INLINE int autopush_add(struct strapush *sap);

EXPORT_SYMBOL_NOVERS(autopush_add);

__LFS_EXTERN_INLINE int autopush_vml(struct str_mlist *ml, int nmods);

EXPORT_SYMBOL_NOVERS(autopush_vml);

__LFS_EXTERN_INLINE struct strapush *autopush_find(dev_t dev);

EXPORT_SYMBOL_NOVERS(autopush_find);

#ifdef CONFIG_STREAMS_COMPAT_LFS_MODULE
static
#endif
int __init
lfscomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_LFS_MODULE
	printk(KERN_INFO LFSCOMP_BANNER);
#else
	printk(KERN_INFO LFSCOMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_LFS_MODULE
static
#endif
void __exit
lfscomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_LFS_MODULE
module_init(lfscomp_init);
module_exit(lfscomp_exit);
#endif
