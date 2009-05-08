/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __SYS_LFS_DDI_H__
#define __SYS_LFS_DDI_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2008-2009 Monavacon Limited."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __LFS_EXTERN_INLINE
#define __LFS_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __LFS_EXTERN_INLINE */

#ifndef __LFS_EXTERN
#define __LFS_EXTERN extern streamscall
#endif				/* __AIX_EXTERN_INLINE */

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

__LFS_EXTERN int drv_getparm(const unsigned int parm, void *value_p);

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
	return (((msec * HZ) + 999) / 1000);
}
__LFS_EXTERN_INLINE unsigned long
drv_usectohz(unsigned long usec)
{
	return (((usec * HZ) + 999999) / 1000000);
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
