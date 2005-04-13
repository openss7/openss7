/*****************************************************************************

 @(#) $Id: $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: $ by $Author: $

 *****************************************************************************/

/************************************************************************
*                   SVR 4 User Level Machine Dependencies               *
*************************************************************************
*									*
* This file contains the missing structure definitions required		*
* to run STREAMS at user level in a a testing environment on SVR4 UNIX.	*
*									*
* The file then includes port-mdep.h for the portable version of the	*
* code.									*
*									*
* Author:	David Grothe						*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/

#ifndef	_SYS54_MDEP_H
#define	_SYS54_MDEP_H			1

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"


#include <sys/types.h>
#include <sys/errno.h>	      /* for errno */
#include <memory.h>	      /* for memcpy */
#include <sys/signal.h>	      /* for signal numbers */

/*
 * name changes for these structures.
 */
#define	semaphore	u_semaphore
#define	semaphore_t	u_semaphore_t

/************************************************************************
*                            major/minor                                *
*************************************************************************
*									*
* Macros to extract the major and minor device numbers from a dev_t	*
* variable.								*
*									*
************************************************************************/

#ifndef _INKERNEL		
#define	_INKERNEL		1
#endif

#undef _KERNEL			/* undefine for this include file */
#include <sys/mkdev.h>		/* SVR4 include file */
#define	_KERNEL			1

#define	MAJOR		major
#define	MINOR		minor
#define	makedevice	makedev

#if defined(__USLC__)

    /* compiling with USL C compiler, i.e., compiling on SVR4 */

#else

    /* not compiling ON SVR4, but compiling FOR SVR4 */
    typedef unsigned long	major_t ;
    typedef unsigned long	minor_t ;

#endif

typedef unsigned long	port_dev_t ;		/* device major/minor */

#undef uid 
#undef gid
/* typedef int     o_uid_t; */
/* typedef int     o_gid_t; */
typedef unsigned   char uchar;
typedef struct cred {
	uid_t	cr_uid;			/* effective user id */
	gid_t	cr_gid;			/* effective group id */
} cred_t;

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


#ifndef PORTABLE
#define	PORTABLE	1
#endif

/*
 * Signal that STREAMS uses but SVR4 does not support.
 */
/* #define	SIGURG		31 */




/*
 * Now include the portable stuff
 */
#ifndef _USER_CMN_H
#include <sys/LiS/user-cmn.h>
#endif
#ifndef _PORT_MDEP_H
#include <sys/LiS/port-mdep.h>
#endif


#endif
