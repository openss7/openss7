/*****************************************************************************

 @(#) ddi.h,v 0.9.2.8 2003/10/26 17:25:59 brian Exp

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

 Last Modified 2003/10/26 17:25:59 by brian

 *****************************************************************************/

#ifndef __DDI_H__
#define __DDI_H__ 1

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif				/* __EXTERN_INLINE */

#include <asm/uaccess.h>	/* for copy_[to|from]_user */
#if 0
#include <linux/compile.h>	/* for UTS_VERSION */
#else
#define UTS_VERSION ""
#endif
#include <linux/version.h>	/* for UTS_RELEASE */
#include <asm/delay.h>		/* for udelay */
#include <linux/dki.h>

__EXTERN_INLINE major_t getmajor(dev_t dev)
{
	return (major(to_kdev_t(dev)));
}
__EXTERN_INLINE minor_t getminor(dev_t dev)
{
	return (minor(to_kdev_t(dev)));
}
__EXTERN_INLINE dev_t makedevice(unsigned char major, unsigned char minor)
{
	return (MKDEV(major, minor));
}

typedef void timo_fcn_t (caddr_t arg);
typedef int toid_t;			/* SVR4 */
typedef int timeout_id_t;		/* Solaris */

extern toid_t timeout(timo_fcn_t *timo_fcn, caddr_t arg, long ticks);
extern clock_t untimeout(toid_t toid);

#if 0
int mknod(const char *pathname, mode_t mode, dev_t dev);
int mount(char *specialfile, char *dir, char *filesystemtype, unsigned long mountflags, void *data);
int umount2(char *pathname, int flags);
int unlink(char *pathname);
#endif

__EXTERN_INLINE int copyin(const void *from, void *to, size_t len)
{
	return copy_from_user(to, from, len);
}
__EXTERN_INLINE int copyout(const void *from, void *to, size_t len)
{
	return copy_to_user(to, from, len);
}

__EXTERN_INLINE unsigned long drv_hztousec(unsigned long hz)
{
	return ((hz * 1000000) / HZ);
}
__EXTERN_INLINE unsigned long drv_usectohz(unsigned long usec)
{
	return (((usec + 999999) * HZ) / 1000000);
}

__EXTERN_INLINE unsigned long drv_hztomsec(unsigned long hz)
{
	return ((hz * 1000) / HZ);
}
__EXTERN_INLINE unsigned long drv_msectohz(unsigned long msec)
{
	return (((msec + 999) * HZ) / 1000);
}

#undef min
__EXTERN_INLINE int min(int a, int b)
{
	return ((a < b) ? a : b);
}

#undef max
__EXTERN_INLINE int max(int a, int b)
{
	return ((a < b) ? b : a);
}

#define LBOLT		0x01
#define PPGP		0x02
#define UPROCP		0x03
#define PPID		0x04
#define PSID		0x05
#define TIME		0x06
#define UCRED		0x07
/* 
 *  Note: the following are added for UW compatibility:
 */
#define DRV_MAXBIOSIZE	0x08
#define STRMSGSIZE	0x09
#define	HW_PROVIDER	0x0a
#define SYSCRED		0x0b

extern int sysctl_str_strmsgsz;

__EXTERN_INLINE int drv_getparm(const unsigned int parm, void *value_p)
{
	switch (parm) {
	case LBOLT:
		*(unsigned long *) value_p = jiffies;
		return (0);
	case PPGP:
		*(pid_t *) value_p = current->pgrp;
		return (0);
	case UPROCP:
		*(ulong *) value_p = (ulong) current->files;
		return (0);
	case PPID:
		*(pid_t *) value_p = current->pid;
		return (0);
	case PSID:
		*(pid_t *) value_p = current->session;
		return (0);
	case TIME:
	{
		struct timeval tv;
		do_gettimeofday(&tv);
		*(time_t *) value_p = tv.tv_sec;
		return (0);
	}
	case UCRED:
		*(cred_t **) value_p = current_creds;
		return (0);
	case STRMSGSIZE:
		*(int *) value_p = sysctl_str_strmsgsz;
		return (0);
	case HW_PROVIDER:
		*(char **) value_p = "Linux " UTS_RELEASE " " UTS_VERSION;
		return (0);
	case DRV_MAXBIOSIZE:
	case SYSCRED:
		return (-1);
	}
	return (-1);
}
__EXTERN_INLINE int drv_priv(cred_t *crp)
{
	return (crp->cr_uid == 0 || crp->cr_ruid == 0);
}
__EXTERN_INLINE void drv_usecwait(unsigned long usec)
{
	return (udelay(usec));
}
__EXTERN_INLINE void delay(unsigned long ticks)
{
	set_current_state(TASK_INTERRUPTIBLE);
	while ((ticks = schedule_timeout(ticks))) ;
	set_current_state(TASK_RUNNING);
}

static __inline__ void bcopy(const void *from, void *to, size_t len)
{
	memcpy(to, from, len);
}
__EXTERN_INLINE void bzero(void *data, size_t len)
{
	memset(data, 0, len);
}

/* these are SVR 4 D3DK functions that need to be implemented yet */
#if 0

/* the following should be in linux/buf.h anyway */
typedef struct buf {
	uint b_flags;
	struct buf *b_forw;
	struct buf *b_back;
	struct buf *av_forw;
	struct buf *av_back;
	long b_bufsize;
	uint b_bcount;
	daddr_t b_blkno;
	ushort b_blkoff;
	unsigned char b_addrtype;
	union {
		caddr_t b_addr;
		uio_t *b_uio;
	} b_un;
	uint b_resid;
	clock_t b_start;
	void (*b_iodone) ();
	void *b_misc;
	union {
		void *un_ptr;
		int un_int;
	} b_priv;
	union {
		void *un_ptr;
		int un_int;
		long un_long;
		daddr_t un_daddr;
	} b_priv2;
} buf_t;
__EXTERN_INLINE void biodone(buf_t * bp);	/* not implemented */
__EXTERN_INLINE int biowait(buf_t * bp);	/* not implemented */
__EXTERN_INLINE void bp_mapin(struct buf_t *bp);	/* not implemented */
__EXTERN_INLINE void bp_mapout(struct buf_t *bp);	/* not implemented */
__EXTERN_INLINE void brelse(struct buf_t *bp);	/* not implemented */
__EXTERN_INLINE void clrbuf(buf_t * bp);	/* not implemented */
__EXTERN_INLINE void freerbuf(buf_t * bp);	/* not implemented */
__EXTERN_INLINE void geterror(buf_t * bp);	/* not implemented */
__EXTERN_INLINE buf_t getrbuf(int flag);	/* not implemented */

__EXTERN_INLINE ulong btop(ulong numbytes);	/* not implemented */
__EXTERN_INLINE ulong btopr(ulong numbytes);	/* not implemented */

__EXTERN_INLINE void page_numtopp(void);	/* see uw7ddi.h */
__EXTERN_INLINE void page_pptonum(void);	/* see uw7ddi.h */

__EXTERN_INLINE void sleep(void);	/* see svr4ddi.h */
__EXTERN_INLINE void wakeup(void);	/* see svr4ddi.h */
__EXTERN_INLINE void spl(void);		/* see svr4ddi.h */

__EXTERN_INLINE void rmalloc(void);	/* not implemented */
__EXTERN_INLINE void rmfree(void);	/* not implemented */
__EXTERN_INLINE void rminit(void);	/* not implemented */

__EXTERN_INLINE void uiomove(void);	/* see uw7ddi.h */
__EXTERN_INLINE void ureadc(void);	/* see uw7ddi.h */
__EXTERN_INLINE void useracc(void);	/* see uw7ddi.h */
__EXTERN_INLINE void uwritec(void);	/* see uw7ddi.h */
#endif

/* pull in OS specific defines */

#ifdef _SVR4_SOURCE
#if defined(CONFIG_STREAMS_COMPAT_SVR4) || defined(CONFIG_STREAMS_COMPAT_SVR4_MODULE)
#include <linux/svr4ddi.h>
#elif !defined(EXPORT_SYMTAB)
#warning _SVR4_SOURCE defined but not CONFIG_STREAMS_COMPAT_SVR4
#endif
#endif

#ifdef _OSF_SOURCE
#if defined(CONFIG_STREAMS_COMPAT_OSF) || defined(CONFIG_STREAMS_COMPAT_OSF_MODULE)
#include <linux/osfddi.h>
#elif !defined(EXPORT_SYMTAB)
#warning _OSF_SOURCE defined but not CONFIG_STREAMS_COMPAT_OSF
#endif
#endif

#ifdef _AIX_SOURCE
#if defined(CONFIG_STREAMS_COMPAT_AIX) || defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)
#include <linux/aixddi.h>
#elif !defined(EXPORT_SYMTAB)
#warning _AIX_SOURCE defined but not CONFIG_STREAMS_COMPAT_AIX
#endif
#endif

#ifdef _HPUX_SOURCE
#if defined(CONFIG_STREAMS_COMPAT_HPUX) || defined(CONFIG_STREAMS_COMPAT_HPUX_MODULE)
#include <linux/hpuxddi.h>
#elif !defined(EXPORT_SYMTAB)
#warning _HPUX_SOURCE defined but not CONFIG_STREAMS_COMPAT_HPUX
#endif
#endif

#ifdef _UW7_SOURCE
#if defined(CONFIG_STREAMS_COMPAT_UW7) || defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)
#include <linux/uw7ddi.h>
#elif !defined(EXPORT_SYMTAB)
#warning _UW7_SOURCE defined but not CONFIG_STREAMS_COMPAT_UW7
#endif
#endif

#ifdef _SUN_SOURCE
#if defined(CONFIG_STREAMS_COMPAT_SUN) || defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
#include <linux/sunddi.h>
#elif !defined(EXPORT_SYMTAB)
#warning _SUN_SOURCE defined but not CONFIG_STREAMS_COMPAT_SUN
#endif
#endif

#ifdef _LIS_SOURCE
#if defined(CONFIG_STREAMS_COMPAT_LIS) || defined(CONFIG_STREAMS_COMPAT_LIS_MODULE)
#include <linux/lisddi.h>
#elif !defined(EXPORT_SYMTAB)
#warning _LIS_SOURCE defined but not CONFIG_STREAMS_COMPAT_LIS
#endif
#endif

#endif				/* __DDI_H__ */
