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

#ifndef __ISODE_MANIFEST_H__
#define __ISODE_MANIFEST_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * Again, most of this one should be replaced or removed by autoconf techiniques.
 * FIXME: do that.  -bb (2007-10-21)
 */

/* manifest.h - manifest constants */

/* 
 * Header: /xtel/isode/isode/h/RCS/manifest.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 *
 * Log: manifest.h,v
 * Revision 9.0  1992/06/16  12:17:57  isode
 * Release 8.0
 *
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

#ifndef	_MANIFEST_
#define	_MANIFEST_

			/* current ISODE distribution: major*10+minor */
#define	ISODE	80

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif

/* target-dependent defines:

	SYS5NLY -	target has SYS5 types only, no BSD types

	BSDSIGS	-	target supports BSD signals

 */

#ifdef	BSD42
#undef	SYS5NLY
#define	BSDSIGS
#endif

#ifdef	ROS
#undef	SYS5NLY
#define	BSDSIGS

#ifndef	BSD42
#define	BSD42
#endif

#endif

#ifdef	SYS5
#define	SYS5NLY
#undef	BSDSIGS

#ifdef	masscomp
#undef	SYS5NLY
#endif

#ifdef	OSX
#undef	SYS5NLY
#endif

#ifdef	XOS_2
#undef	SYS5NLY
#define	SIGEMT	SIGSEGV
#endif

#if	defined(WIN) || defined (WINTLI)
#undef	SYS5NLY
#endif

#ifdef	sun
#undef	SYS5NLY
#endif

#ifdef	sgi
#undef	SYS5NLY
#endif

#ifdef SVR4
#undef  SYS5NLY
#endif

#ifdef	HPUX
#undef	SYS5NLY
#define	BSDSIGS
#undef	SIGPOLL
#endif

#ifdef	AIX
#undef	SYS5NLY
#define	BSDSIGS
#define	SIGEMT	SIGUSR1

int (*_signal()) ();

#define	signal	_signal
#endif

#ifdef AUX
#undef	SYS5NLY
#define BSDSIGS
#endif
#endif

#ifdef  SVR4
#ifndef SVR3
#define SVR3
#endif
#endif

#ifdef	NSIG

#ifndef	sigmask
#define	sigmask(s)	(1 << ((s) - 1))
#endif

#ifndef _SIGIO
#ifdef XOS_2
#define _SIGIO SIGEMT
#else
#ifdef	SIGPOLL
#define	_SIGIO	SIGPOLL
#else
#ifdef	SIGUSR1
#define	_SIGIO	SIGUSR1
#else
#define	_SIGIO	SIGEMT
#endif
#endif
#endif
#endif

#if 0
typedef int SBV;

#ifndef	lint
#define	sigioblock()	(_iosignals_set ? sigblock (sigmask (_SIGIO)) : 0)
#define	sigiomask(s)	(_iosignals_set ? sigsetmask (s) : 0)
#else
#define	sigioblock()	sigblock (sigmask (_SIGIO))
#define	sigiomask(s)	sigsetmask (s)
#endif
extern int _iosignals_set;

#define	siginblock()	sigblock (sigmask (SIGINT))
#define	siginmask(s)	sigsetmask (s)
#else

#ifdef HAVE_SIGPROCMASK
typedef sigset_t SBV;
#else
typedef int SBV;
#endif

extern SBV sigioblock(void);		/* see compat/sigioblock.c */
extern void sigiomask(SBV mask);	/* see compat/sigiomask.c */
extern SBV siginblock(void);		/* see compat/siginblock.c */
extern void siginmask(SBV mask);	/* see compat/siginmask.c */

#endif

#endif

/* TYPES */

#ifndef	NOTOK
#define	NOTOK		(-1)
#define	OK		0
#define	DONE		1
#define CONNECTING_1	OK
#define CONNECTING_2	2
#endif

#ifdef SVR4
#ifdef NULLVP
#undef NULLVP
#endif
#endif

#ifndef	NULLCP
typedef char *CP;

#define	NULLCP		((char *) 0)
#define	NULLVP		((char **) 0)
#endif

#ifndef INTDEF
#define	INTDEF	long
#endif

typedef INTDEF integer;

#define NULLINT		((integer) 0)
#define NULLINTP	((integer *) 0)

#ifndef	makedev
#include <sys/types.h>
#if	defined(WIN) || defined(WINTLI)
#include "sys/inet.h"
#ifndef	NFDBITS
typedef struct fd_set {
	int fds_bits[1];
} fd_set;
#endif
#endif

#if defined (_AIX) && defined (SYS5)
#include <sys/select.h>
#endif
int xselect(int nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, int secs);

#ifdef	SYS5NLY
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
#endif
#endif

#if	!defined(FD_SET) && !defined(LINUX)
#define	FD_SETSIZE	    (sizeof (fd_set) * 8)

#define FD_SET(f,s)	    ((s)->fds_bits[0] |= (1 << (f)))
#define FD_CLR(f,s)	    ((s)->fds_bits[0] &= ~(1 << (f)))
#define FD_ISSET(f,s)	    ((s)->fds_bits[0] & (1 << (f)))
#define FD_ZERO(s)	    ((s)->fds_bits[0] = 0)
#endif

#define	NULLFD		((fd_set *) 0)

#undef	IP
typedef int *IP;

#define	NULLIP		((IP) 0)

#if 0
typedef int (*IFP) ();

#define	NULLIFP		((IFP) 0)

typedef void (*VFP) ();

#define	NULLVFP		((VFP) 0)

#ifndef	SFD
#if !defined(SVR3) && !defined(SUNOS4) && !defined(BSD44) && !defined(ultrix)
#define	SFD	int
#define	SFP	IFP
#else
#define	SFD	void
#define	SFP	VFP
#endif
#endif
#endif

#ifndef HAVE_SIGHANDLER_T
typedef RETSIGTYPE(*sighandler_t) (int);
#endif

struct udvec {				/* looks like a BSD iovec... */
	caddr_t uv_base;
	int uv_len;

	int uv_inline;
};

struct qbuf {
	struct qbuf *qb_forw;		/* doubly-linked list */
	struct qbuf *qb_back;		/* .. */

	int qb_len;			/* length of data */
	char *qb_data;			/* current pointer into data */
	char qb_base[1];		/* extensible... */
};

#define	QBFREE(qb) \
{ \
    register struct qbuf *QB, \
			 *QP; \
 \
    for (QB = (qb) -> qb_forw; QB != (qb); QB = QP) { \
	QP = QB -> qb_forw; \
	remque (QB); \
	free ((char *) QB); \
    } \
}

#ifndef	min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#define	max(a, b)	((a) > (b) ? (a) : (b))
#endif

#ifdef SYS5

#if	!defined(WINTLI) && !defined(WIN)
#ifndef	sun
#define	getdtablesize()	_NFILE
#endif
#else
#define	getdtablesize()	(_NFILE - 1)
#endif

#endif

#if	defined(RT) || defined (HPUX)
#define	ntohs(x)	(x)
#define	htons(x)	(x)
#define	ntohl(x)	(x)
#define	htonl(x)	(x)
#endif

/* POSIX */

#ifndef	SETSID
#if	defined(SUNOS41) || defined(BSD44) || defined (SVR4) || defined(_AIX)
#define	SETSID
#endif
#endif

#endif

#endif				/* __ISODE_MANIFEST_H__ */
