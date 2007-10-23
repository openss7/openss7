/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* general.c - general utilities for emulation of 4.2BSD */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/general.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/general.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: general.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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

/* LINTLIBRARY */

#ifdef linux
#include <unistd.h>
#endif
#include <stdio.h>
#include "general.h"
#include "manifest.h"

/* Berkeley UNIX: 4.2 */

#ifdef	BSD42

/* Simply including "general.h" is sufficient. */

int
_general_stub()
{
	return (0);
};

#endif

/* non-Berkeley UNIX */

#if !defined HAVE_INSQUE || !defined HAVE_REMQUE
struct qelem {
	struct qelem *q_forw;
	struct qelem *q_back;
	char q_data[1];			/* extensible */
};
#endif				/* !defined HAVE_INSQUE || !defined HAVE_REMQUE */

#ifndef HAVE_INSQUE
void
insque(elem, pred)
	struct qelem *elem, *pred;
{
	if (pred->q_forw)
		pred->q_forw->q_back = elem;
	elem->q_forw = pred->q_forw;
	elem->q_back = pred;
	pred->q_forw = elem;
}
#endif				/* HAVE_INSQUE */

#ifndef HAVE_REMQUE
void
remque(elem)
	struct qelem *elem;
{
	if (elem->q_forw)
		elem->q_forw->q_back = elem->q_back;
	elem->q_back->q_forw = elem->q_forw;
}
#endif				/* HAVE_REMQUE */

/* DUP2 */

#ifndef	BSD42
#ifdef	SYS5
#include <fcntl.h>
#endif

extern int errno;

#ifndef	F_DUPFD
#ifdef linux
int
#endif
dup2_aux(d1, d2)
	int d1, d2;
{
	int fd, result;

#ifdef linux
	(void) rcsid;
#endif
	if ((fd = dup(d1)) == NOTOK || fd == d2)
		return fd;

	result = dup2_aux(d1, d2);

	(void) close(fd);

	return result;
}
#endif

int
dup2(d1, d2)
	register int d1, d2;
{
	int d;

	if (d1 == d2)
		return OK;

	(void) close(d2);
#ifdef	F_DUPFD
	if ((d = fcntl(d1, F_DUPFD, d2)) == NOTOK)
		return NOTOK;
#else
	if ((d = dup2_aux(d1, d2)) == NOTOK)
		return NOTOK;
#endif
	if (d == d2)
		return OK;

	errno = 0;
	return NOTOK;
}

#endif

/* BYTEORDER */

#ifndef linux
#ifndef	SWABLIB

/* ROS and HP-UX don't seem to have these in libc.a */

#undef	ntohs
unsigned short
ntohs(netshort)
	unsigned short netshort;
{
	return netshort;
}

#undef	htons
unsigned short
htons(hostshort)
	unsigned short hostshort;
{
	return hostshort;
}

#undef	ntohl
unsigned long
ntohl(netlong)
	unsigned long netlong;
{
	return netlong;
}

#undef	htonl
unsigned long
htonl(hostlong)
	unsigned long hostlong;
{
	return hostlong;
}

#endif
#endif

static inline void
dummy(void)
{
	(void) rcsid;
}
