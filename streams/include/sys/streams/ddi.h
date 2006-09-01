/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.37 2006/07/24 09:01:12 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2006/07/24 09:01:12 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.37  2006/07/24 09:01:12  brian
 - results of udp2 optimizations

 Revision 0.9.2.36  2006/05/23 10:39:40  brian
 - marked normally inline functions for unlikely text section

 Revision 0.9.2.35  2006/02/20 10:59:20  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __SYS_STREAMS_DDI_H__
#define __SYS_STREAMS_DDI_H__ 1

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.37 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_DDI_H__
#warning "Do no include sys/streams/ddi.h directly, include sys/ddi.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN
#define __EXTERN extern
#endif

#ifndef __STREAMS_EXTERN
#define __STREAMS_EXTERN __EXTERN streams_fastcall
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __STREAMS_EXTERN_INLINE
#define __STREAMS_EXTERN_INLINE __EXTERN_INLINE streams_fastcall
#endif				/* __STREAMS_EXTERN_INLINE */
#ifndef __STRSCHD_EXTERN_INLINE
#define __STRSCHD_EXTERN_INLINE __EXTERN_INLINE streams_fastcall
#endif				/* __STRSCHD_EXTERN_INLINE */
#ifndef __STRUTIL_EXTERN_INLINE
#define __STRUTIL_EXTERN_INLINE __EXTERN_INLINE streams_fastcall
#endif				/* __STRUTIL_EXTERN_INLINE */

#include <asm/uaccess.h>	/* for copy_[to|from]_user */
#if 0
#include <linux/compile.h>	/* for UTS_VERSION */
#else
#define UTS_VERSION ""
#endif
#include <linux/version.h>	/* for UTS_RELEASE */
#include <asm/delay.h>		/* for udelay */
#include <sys/dki.h>
#if 0
#include <sys/map.h>		/* for rm map definitions */
#endif

__STRUTIL_EXTERN_INLINE major_t
getmajor(dev_t dev)
{
	ulong major = ((dev >> 16) & 0x0000ffff);

#if 0
	if (!major)
		major = MAJOR(dev);
#endif
	return (major);
}
__STRUTIL_EXTERN_INLINE minor_t
getminor(dev_t dev)
{
	ulong minor = (dev & 0x0000ffff);

#if 0
	ulong major = ((dev >> 16) & 0x0000ffff);

	if (!major)
		minor = MINOR(dev);
#endif
	return (minor);
}
__STRUTIL_EXTERN_INLINE dev_t
makedevice(major_t major, minor_t minor)
{
	ulong maj = major & 0x0000ffff;
	ulong min = minor & 0x0000ffff;

	return ((maj << 16) | min);
}

#if 0
int mknod(const char *pathname, mode_t mode, dev_t dev);
int mount(char *specialfile, char *dir, char *filesystemtype, unsigned long mountflags, void *data);
int umount2(char *pathname, int flags);
int unlink(char *pathname);
#endif

__STRUTIL_EXTERN_INLINE int
copyin(const void *from, void *to, size_t len)
{
	if (!copy_from_user(to, from, len))
		return (0);
	return (-EFAULT);
}
__STRUTIL_EXTERN_INLINE int
copyout(const void *from, void *to, size_t len)
{
	if (!copy_to_user(to, from, len))
		return (0);
	return (-EFAULT);
}

__STRUTIL_EXTERN_INLINE unsigned long
drv_hztousec(unsigned long hz)
{
	if (((1000000 / HZ) * HZ) == 1000000)
		return (hz * (1000000 / HZ));
	return ((hz * 1000000) / HZ);
}
__STRUTIL_EXTERN_INLINE unsigned long
drv_usectohz(unsigned long usec)
{
	if (((1000000 / HZ) * HZ) == 1000000)
		return (usec / (1000000 / HZ));
	return ((usec * HZ) / 1000000);
}

__STRUTIL_EXTERN_INLINE unsigned long
drv_hztomsec(unsigned long hz)
{
	if (((1000 / HZ) * HZ) == 1000)
		return (hz * (1000 / HZ));
	return ((hz * 1000) / HZ);
}
__STRUTIL_EXTERN_INLINE unsigned long
drv_msectohz(unsigned long msec)
{
	if (((1000 / HZ) * HZ) == 1000)
		return (msec / (1000 / HZ));
	return ((msec * HZ) / 1000);
}

#undef min
__STREAMS_EXTERN_INLINE int
min(int a, int b)
{
	return ((a < b) ? a : b);
}

#undef max
__STREAMS_EXTERN_INLINE int
max(int a, int b)
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

extern ulong sysctl_str_strmsgsz;

__STREAMS_EXTERN int drv_getparm(const unsigned int parm, void *value_p);

__STRUTIL_EXTERN_INLINE int
drv_priv(cred_t *crp)
{
	/* FIXME: also need to check for capabilities */
	if (crp->cr_uid == 0 || crp->cr_ruid == 0)
		return (0);
	return (EPERM);
}
__STRUTIL_EXTERN_INLINE void
drv_usecwait(unsigned long usec)
{
	return (udelay(usec));
}
__STRUTIL_EXTERN_INLINE void
delay(unsigned long ticks)
{
	set_current_state(TASK_UNINTERRUPTIBLE);
	while ((ticks = schedule_timeout(ticks))) ;
	set_current_state(TASK_RUNNING);
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
__STREAMS_EXTERN_INLINE void biodone(buf_t * bp);	/* not implemented */
__STREAMS_EXTERN_INLINE int biowait(buf_t * bp);	/* not implemented */
__STREAMS_EXTERN_INLINE void bp_mapin(struct buf_t *bp);	/* not implemented */
__STREAMS_EXTERN_INLINE void bp_mapout(struct buf_t *bp);	/* not implemented */
__STREAMS_EXTERN_INLINE void brelse(struct buf_t *bp);	/* not implemented */
__STREAMS_EXTERN_INLINE void clrbuf(buf_t * bp);	/* not implemented */
__STREAMS_EXTERN_INLINE void freerbuf(buf_t * bp);	/* not implemented */
__STREAMS_EXTERN_INLINE void geterror(buf_t * bp);	/* not implemented */
__STREAMS_EXTERN_INLINE buf_t getrbuf(int flag);	/* not implemented */

__STREAMS_EXTERN_INLINE ulong btop(ulong numbytes);	/* not implemented */
__STREAMS_EXTERN_INLINE ulong btopr(ulong numbytes);	/* not implemented */

__STREAMS_EXTERN_INLINE void page_numtopp(void);	/* see uw7ddi.h */
__STREAMS_EXTERN_INLINE void page_pptonum(void);	/* see uw7ddi.h */

__STREAMS_EXTERN_INLINE void sleep(void);	/* see svr4ddi.h */
__STREAMS_EXTERN_INLINE void wakeup(void);	/* see svr4ddi.h */
__STREAMS_EXTERN_INLINE void spl(void);		/* see svr4ddi.h */

__STREAMS_EXTERN_INLINE void rmalloc(void);	/* not implemented */
__STREAMS_EXTERN_INLINE void rmfree(void);	/* not implemented */
__STREAMS_EXTERN_INLINE void rminit(void);	/* not implemented */

__STREAMS_EXTERN_INLINE void uiomove(void);	/* see uw7ddi.h */
__STREAMS_EXTERN_INLINE void ureadc(void);	/* see uw7ddi.h */
__STREAMS_EXTERN_INLINE void useracc(void);	/* see uw7ddi.h */
__STREAMS_EXTERN_INLINE void uwritec(void);	/* see uw7ddi.h */
#endif

#endif				/* __SYS_STREAMS_DDI_H__ */
