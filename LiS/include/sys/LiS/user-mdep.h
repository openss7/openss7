/*****************************************************************************

 @(#) $RCSfile: user-mdep.h,v $ $Name:  $($Revision: 1.1.1.3.4.5 $) $Date: 2008-04-29 08:33:16 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2008-04-29 08:33:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: user-mdep.h,v $
 Revision 1.1.1.3.4.5  2008-04-29 08:33:16  brian
 - update headers for Affero release

 Revision 1.1.1.3.4.4  2007/08/14 10:47:11  brian
 - GPLv3 header update

 *****************************************************************************/

/************************************************************************
*                   User Level Machine Dependencies                     *
*************************************************************************
*									*
* This file contains the missing structure definitions required		*
* to run STREAMS at user level in a a testing environment.		*
*									*
* The file then includes port-mdep.h for the portable version of the	*
* code.									*
*									*
* Author:	David Grothe						*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/

#ifndef _USER_MDEP_H
#define _USER_MDEP_H

#ident "@(#) $RCSfile: user-mdep.h,v $ $Name:  $($Revision: 1.1.1.3.4.5 $) $Date: 2008-04-29 08:33:16 $"

#include <sys/errno.h>		/* for errno */
#ifndef _MEMORY_H
#include <memory.h>		/* for memcpy */
#endif
#ifndef _SIGNAL_H
#include <sys/signal.h>		/* for signal numbers */
#endif
#ifndef _SYS_TYPES_H
#include <sys/types.h>		/* uid_t and other types */
#endif
#include <sys/time.h>		/* for gettimeofday */
#include <sys/LiS/config.h>
#include <sys/LiS/genconf.h>

/*
 * Used for memory allocation alignment
 */
#define LIS_CACHE_BYTES	16

/*
 * Major/minor representation is unique for each version
 */
#ifdef major
#undef major
#endif
#ifdef minor
#undef minor
#endif
#define	major(dev_t_var)	STR_MAJOR(dev_t_var)
#define	minor(dev_t_var)	STR_MINOR(dev_t_var)
#define	MAJOR(dev)		( ((dev) >> 16) )
#define	MINOR(dev)		( ((dev) & 0xFFFF) )
#define	STR_KMAJOR(dev_t_var)	STR_MAJOR(dev_t_var)
#define	STR_KMINOR(dev_t_var)	STR_MINOR(dev_t_var)
#define DEV_TO_INT(dev)		(dev)
#define DEV_SAME(a,b)		((a) == (b))
#define UMKDEV(majr,minr)	MKDEV(majr, minr)
#define MKDEV(majr,minr)	makedevice(majr,minr)

#ifdef makedevice
#undef makedevice
#endif
#define lis_makedevice(majornum,minornum) \
		((((dev_t) (majornum)) << 16) | ((minornum) & 0xFFFF))
#define lis_getmajor(dev)		((dev) >> 16)
#define lis_getminor(dev)		((dev) & 0xFFFF)
#define makedevice(majr,minr)	lis_makedevice((majr),(minr))
#define getmajor(dev)		lis_getmajor(dev)
#define getminor(dev)		lis_getminor(dev)
#define lis_kern_to_lis_dev(dev) (dev)
#define	DEV_TO_RDEV(dev)	((dev_t)(dev))
#define	RDEV_TO_DEV(rdev)	((dev_t)(rdev))
#define	RDEV_TO_INT(rdev)	((int)(rdev))
#define	GET_I_RDEV(inode)	inode->i_rdev

typedef unsigned long port_dev_t;	/* device major/minor */

#ifndef LIS_HAVE_MAJOR_T
#define LIS_HAVE_MAJOR_T
typedef unsigned short major_t;
typedef unsigned short minor_t;
#endif

#define	LIS_FIFO  FIFO__CMAJOR_0
#define LIS_CLONE CLONE__CMAJOR_0

/*
 * Task identity
 */
#define	lis_is_current_task(taskp)	1	/* just a dummy */
#define	lis_current_task_ptr		((void *)0)

/*
 * Atomic functions
 *
 * Usage is: lis_atomic_t	av ;
 *           lis_atomic_set(&av, 1) ;
 */
typedef volatile int lis_atomic_t;

#define lis_atomic_set(atomic_addr,valu)	*(atomic_addr) = (valu)
#define lis_atomic_read(atomic_addr)		*(atomic_addr)
#define lis_atomic_add(atomic_addr,amt)		(*(atomic_addr)) += (amt)
#define lis_atomic_sub(atomic_addr,amt)		(*(atomic_addr)) -= (amt)
#define lis_atomic_inc(atomic_addr)		(*(atomic_addr))++
#define lis_atomic_dec(atomic_addr)		(*(atomic_addr))--
#define lis_atomic_dec_and_test(atomic_addr)	(*(atomic_addr))--,*(atomic_addr) != 0

#define	K_ATOMIC_INC(lis_atomic_addr) lis_atomic_inc((lis_atomic_addr))
#define	K_ATOMIC_DEC(lis_atomic_addr) lis_atomic_dec((lis_atomic_addr))
#define	K_ATOMIC_READ(lis_atomic_addr) lis_atomic_read((lis_atomic_addr))
#define	K_ATOMIC_SET(lis_atomic_addr,v) lis_atomic_set((lis_atomic_addr),v)
#define	K_ATOMIC_ADD(lis_atomic_addr,v) lis_atomic_add((lis_atomic_addr),v)
#define	K_ATOMIC_SUB(lis_atomic_addr,v) lis_atomic_sub((lis_atomic_addr),v)

/*
 *  lis_gettimeofday -  used by lis_hitime and similar functions
 */
#define lis_gettimeofday(tvp) gettimeofday((tvp),NULL)

/*
 * User ids and group ids are unique for each version
 */
#undef uid
#undef gid
#ifndef LIS_HAVE_O_UID_T
#define LIS_HAVE_O_UID_T
typedef int o_uid_t;
typedef int o_gid_t;
#endif
typedef unsigned char uchar;
typedef struct cred {
	uid_t cr_uid;			/* effective user id */
	gid_t cr_gid;			/* effective group id */
	uid_t cr_ruid;			/* real user id */
	gid_t cr_rgid;			/* real group id */
} cred_t;

/*
 * Semaphores are unique for each version
 */

#define	semaphore	u_semaphore
#define	semaphore_t	u_semaphore_t

typedef struct u_semaphore {
	int sem_count;			/* semaphore counter */
	long sem_xxx[8];		/* just a placeholder */

} u_semaphore_t;

#define	kvtophys(addr)		(addr)	/* user level fakery */

/*
 * bzero and bcopy
 */
#define	bzero(addr,nbytes)	memset(addr, 0, nbytes)
#define	bcopy(src,dst,n)	memcpy(dst,src,n)

/*
 * A dummy to support the select structure in stdata_t.
 */
typedef struct lis_select_struct {
	int dummy;

} lis_select_t;

#define	LIS_QSYNC_FREE		FREE
#define	LIS_HEAD_FREE		FREE
#define	LIS_QBAND_FREE		FREE
#define	LIS_QUEUE_FREE		FREE
#define	LIS_QUEUE_ALLOC(nb,s)	ALLOCF_CACHE(nb,s)
#define LIS_QBAND_ALLOC(nb,s)	ALLOCF(nb,s)
#define LIS_HEAD_ALLOC(nb,s)	ALLOCF(nb,s)
#define LIS_QSYNC_ALLOC(nb,s)	ALLOCF(nb,s)

#ifndef PORTABLE
#define	PORTABLE	1
#endif

/*
 * Now include the portable stuff
 */
#ifndef _USER_CMN_H
#include <sys/LiS/user-cmn.h>	/* common elements for all user-file systs */
#endif
#ifndef _PORT_MDEP_H
#include <sys/LiS/port-mdep.h>
#endif

/*
 * Redefine the runqueue routine for user level testing.
 */
#undef lis_runqueues
#define	lis_runqueues()		lis_run_queues(0)
void lis_run_queues(int cpu);

/*
 * Macros for locking and unlocking a queue structure.
 */
#define lis_lockqf(qp,f,l)   lis_lockq_fcn((qp),f,l)
#define lis_lockq(qp)	     lis_lockqf(qp,__FILE__,__LINE__)
#define lis_unlockqf(qp,f,l) lis_unlockq_fcn((qp),f,l)
#define lis_unlockq(qp)	     lis_unlockqf(qp,__FILE__,__LINE__)

/*
 *  FIFO/pipe support
 */
extern int user_get_fifo(struct file **);
extern int user_get_pipe(struct file **, struct file **);
extern int user_pipe(unsigned int *);

#endif
