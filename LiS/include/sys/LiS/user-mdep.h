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
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ifndef _USER_MDEP_H
#define _USER_MDEP_H

#ident "@(#) LiS user-mdep.h 2.10 01/12/04 11:06:59 "

#include <sys/errno.h>	      /* for errno */
#ifndef _MEMORY_H
#include <memory.h>	      /* for memcpy */
#endif
#ifndef _SIGNAL_H
#include <sys/signal.h>	      /* for signal numbers */
#endif
#ifndef _SYS_TYPES_H
#include <sys/types.h>	      /* uid_t and other types */
#endif
#include <sys/time.h>         /* for gettimeofday */
#include <sys/LiS/config.h>

#if 0 /* OSH: Not needed with -D_BSD_SOURCE */
/*
 *  caddr_t is missing when we use glibc and -D_SVID_SOURCE
 */
#ifdef __GLIBC__
/* typedef unsigned char *caddr_t;  BSD gets this */
#endif

/*
 * sync(2) prototype is missing when we use glibc and -D_SVID_SOURCE
 * This is probably a bug in my glibc 2.0.7
 */
#ifdef __GLIBC__
int sync(void);
#endif
#endif

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
#define KDEV_TO_INT(dev_t_var)	(dev_t_var)
#define DEV_SAME(a,b)		((a) == (b))
#define MKDEV(majnum,minnum)	makedevice(majnum,minnum)

#define makedevice(majornum,minornum) \
		((((dev_t) (majornum)) << 16) | ((minornum) & 0xFFFF))

typedef unsigned long	port_dev_t ;		/* device major/minor */

#ifndef LIS_HAVE_MAJOR_T
#define LIS_HAVE_MAJOR_T
typedef unsigned short	major_t ;
typedef unsigned short	minor_t ;
#endif

#define	LIS_FIFO  FIFO__CMAJOR_0
#define LIS_CLONE CLONE__CMAJOR_0

/*
 * Atomic functions
 *
 * Usage is: lis_atomic_t	av ;
 *           lis_atomic_set(&av, 1) ;
 */
typedef	volatile int	lis_atomic_t ;

#define lis_atomic_set(atomic_addr,valu)	*(atomic_addr) = (valu)
#define lis_atomic_read(atomic_addr)		*(atomic_addr)
#define lis_atomic_add(atomic_addr,amt)		(*(atomic_addr)) += (amt)
#define lis_atomic_sub(atomic_addr,amt)		(*(atomic_addr)) -= (amt)
#define lis_atomic_inc(atomic_addr)		(*(atomic_addr))++
#define lis_atomic_dec(atomic_addr)		(*(atomic_addr))--
#define lis_atomic_dec_and_test(atomic_addr)	(*(atomic_addr))--,*(atomic_addr) != 0

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
typedef int     o_uid_t;
typedef int     o_gid_t;
#endif
typedef unsigned   char uchar;
typedef struct cred {
	uid_t	cr_uid;			/* effective user id */
	gid_t	cr_gid;			/* effective group id */
	uid_t	cr_ruid;		/* real user id */
	gid_t	cr_rgid;		/* real group id */
} cred_t;

/*
 * Semaphores are unique for each version
 */

#define	semaphore	u_semaphore
#define	semaphore_t	u_semaphore_t

typedef struct u_semaphore
{
    int			sem_count ;		/* semaphore counter */
    long		sem_xxx[8] ;		/* just a placeholder */

} u_semaphore_t ;


#define	kvtophys(addr)		(addr)		/* user level fakery */

/*
 * bzero and bcopy
 */
#define	bzero(addr,nbytes)	memset(addr, 0, nbytes)
#define	bcopy(src,dst,n)	memcpy(dst,src,n)

/*
 * A dummy to support the select structure in stdata_t.
 */
typedef struct lis_select_struct
{
    int		dummy ;

} lis_select_t ;


#define	LIS_QBAND_FREE		FREE
#define	LIS_QUEUE_FREE		FREE
#define	LIS_QUEUE_ALLOC(nb,s)	ALLOCF_CACHE(nb,s)
#define LIS_QBAND_ALLOC(nb,s)	ALLOCF(nb,s)

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
void   lis_run_queues(int cpu) ;

/*
 * Queue locking/unlocking dummies
 */
#define lis_lockqf(qp,f,l) do { } while(0)
#define lis_lockq(qp)	lis_lockqf(qp,__FILE__,__LINE__)

#define lis_unlockqf(qp,f,l) do { } while(0)
#define lis_unlockq(qp)	lis_unlockqf(qp,__FILE__,__LINE__)

/*
 *  FIFO/pipe support
 */
extern int user_get_fifo(struct file **);
extern int user_get_pipe(struct file **,struct file **);
extern int user_pipe(unsigned int *);

#endif
