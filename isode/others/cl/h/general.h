/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __CL_H_GENERAL_H__
#define __CL_H_GENERAL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* general.h - general compatibility */

/* 
 * Header: /f/iso/h/RCS/general.h,v 5.0 88/07/21 14:38:59 mrose Rel
 *
 *
 * Log
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef	_GENERAL_
#define	_GENERAL_

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif

/* target-dependent defines:

	BSDFORK -	target has BSD vfork
	BSDLIBC -	target has real BSD libc
	BSDSTRS -	target has BSD strings
	SWABLIB -	target has byteorder(3n) routines
 */

#ifdef	NATIVE
#define	SWABLIB
#endif

#ifdef	EXOS
#define	SWABLIB
#endif

#ifdef	BSD42
#define	BSDFORK
#define	BSDLIBC
#define	BSDSTRS
#endif

#ifdef	ROS
#undef	BSDFORK
#undef	BSDLIBC
#define	BSDSTRS
#ifndef	BSD42
#define	BSD42
#endif
#undef	SWABLIB
#endif

#ifdef	SYS5
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	sgi
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	HPUX
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#undef	SWABLIB
#endif

#ifdef	HPUX
#undef	SWABLIB
#endif

#ifdef	pyr
#undef	SWABLIB
#endif

#ifndef	BSDFORK
#define	vfork	fork
#endif

/* STRINGS */

#ifndef	BSDSTRS
#define	index	strchr
#define	rindex	strrchr
#endif

char *index();
char *mktemp();
char *rindex();

#ifdef	BSDSTRS
char *sprintf();
#else
int sprintf();
#endif
char *strcat();
int strcmp();
char *strcpy();
int strlen();
char *strncat();
int strncmp();
char *strncpy();

char *getenv();
char *calloc(), *malloc(), *realloc();

#if	defined(SYS5) && !defined(AIX)
#include <memory.h>

#define	bcopy(b1,b2,length)	(void) memcpy ((b2), (b1), (length))
#define	bcmp(b1,b2,length)	memcmp ((b1), (b2), (length))
#define	bzero(b,length)		(void) memset ((b), 0, (length))
#endif

/* HEXIFY */

int explode(), implode();

/* SPRINTB */

char *sprintb();

/* STR2VEC */

#define	NVEC	100
#define	NSLACK	10

#define	str2vec(s,v)	str2vecX ((s), (v), 0, NULLIP, NULL)

int str2vecX();

/* STR2ELEM */

#define	NELEM	20

int str2elem();

/* STR2SEL */

int str2sel();
char *sel2str();

/* MISC */

char *sys_errname();

#ifdef	lint
#define	insque(e,p)	INSQUE ((char *) (e), (char *) (p))
#define	remque(e)	REMQUE ((char *) (e))
#endif

void asprintf(), _asprintf();

#endif

#endif				/* __CL_H_GENERAL_H__ */
