/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.14 2006/12/08 05:08:10 brian Exp $

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

 Last Modified $Date: 2006/12/08 05:08:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.14  2006/12/08 05:08:10  brian
 - some rework resulting from testing and inspection

 Revision 0.9.2.13  2006/11/03 10:39:18  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.12  2006/07/24 09:00:48  brian
 - results of udp2 optimizations

 Revision 0.9.2.11  2006/06/22 13:11:22  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.10  2006/05/23 10:44:02  brian
 - mark normal inline functions for unlikely text section

 Revision 0.9.2.9  2005/12/28 09:51:47  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.8  2005/07/18 12:25:39  brian
 - standard indentation

 Revision 0.9.2.7  2005/07/15 23:08:37  brian
 - checking in for sync

 Revision 0.9.2.6  2005/07/14 22:03:46  brian
 - updates for check pass and header splitting

 Revision 0.9.2.5  2005/07/14 03:40:06  brian
 - updates for check pass

 Revision 0.9.2.4  2005/07/13 12:01:48  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 0.9.2.3  2005/07/12 13:54:42  brian
 - changes for os7 compatibility and check pass

 Revision 0.9.2.2  2005/07/05 22:46:05  brian
 - change for strcompat package

 Revision 0.9.2.1  2005/07/04 19:29:12  brian
 - first cut at streams compatibility package

 *****************************************************************************/

#ifndef __SYS_LFS_DDI_H__
#define __SYS_LFS_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.14 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __LFS_EXTERN_INLINE
#define __LFS_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __LFS_EXTERN_INLINE */

#include <linux/delay.h>	/* for udelay */

#ifndef _LFS_SOURCE
#warning "_LFS_SOURCE not defined but LFS ddi.h included"
#endif

#if defined(CONFIG_STREAMS_COMPAT_LFS) || defined(CONFIG_STREAMS_COMPAT_LFS_MODULE)

#include <sys/kmem.h>		/* for kmem_alloc/free */
#include <sys/sad.h>

#ifndef ASSERT
#define ASSERT(__assertion) assert((__assertion))
#endif

/* These functions are missing from LiS, but in the core in LfS */

__LFS_EXTERN_INLINE int
bcmp(const void *s1, const void *s2, size_t len)
{
	return memcmp(s1, s2, len);
}

/* LiS 2.18.0 deprecated these for some reason... */
__LFS_EXTERN_INLINE int
copyin(const void *from, void *to, size_t len)
{
	if (copy_from_user(to, from, len))
		return (-EFAULT);
	return (0);
}
__LFS_EXTERN_INLINE int
copyout(const void *from, void *to, size_t len)
{
	if (copy_to_user(to, from, len))
		return (-EFAULT);
	return (0);
}

#define LBOLT		0x01
#define PPGP		0x02
#define UPROCP		0x03
#define PPID		0x04
#define PSID		0x05
#define TIME		0x06
#define UCRED		0x07
/*
 *  Note: the following are added to UW compatibility:
 */
#define DRV_MAXBIOSIZE	0x08
#define STRMSGSIZE	0x09
#define HW_PROVIDER	0x0a
#define SYSCRED		0x0b

extern int drv_getparm(const unsigned int parm, void *value_p);

__LFS_EXTERN_INLINE int
drv_priv(cred_t *crp)
{
	/* FIXME: also need to check for capabilities */
	if (crp->cr_uid == 0 || crp->cr_ruid == 0)
		return (0);
	return (EPERM);
}

/* FIXME: There are faster ways to do these... */
__LFS_EXTERN_INLINE unsigned long
drv_hztomsec(unsigned long hz)
{
	return ((hz * 1000) / HZ);
}
__LFS_EXTERN_INLINE unsigned long
drv_hztousec(unsigned long hz)
{
	return ((hz * 1000000) / HZ);
}
__LFS_EXTERN_INLINE unsigned long
drv_msectohz(unsigned long msec)
{
	return (((msec + 999) * HZ) / 1000);
}
__LFS_EXTERN_INLINE unsigned long
drv_usectohz(unsigned long usec)
{
	return (((usec + 999999) * HZ) / 1000000);
}

__LFS_EXTERN_INLINE void
drv_usecwait(unsigned long usec)
{
	return (udelay(usec));
}
__LFS_EXTERN_INLINE void
delay(unsigned long ticks)
{
	set_current_state(TASK_INTERRUPTIBLE);
	while ((ticks = schedule_timeout(ticks))) ;
	set_current_state(TASK_RUNNING);
}

#else
#ifdef _LFS_SOURCE
#warning "_LFS_SOURCE defined but not CONFIG_STREAMS_COMPAT_LFS"
#endif
#endif				/* CONFIG_STREAMS_COMPAT_LFS */

#endif				/* __SYS_LFS_DDI_H__ */
