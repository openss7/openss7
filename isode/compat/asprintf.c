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

/* asprintf.c - sprintf with errno */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/asprintf.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/asprintf.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: asprintf.c,v
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

#include <stdio.h>
#include <varargs.h>
#include "general.h"
#include "manifest.h"

/*    DATA */

extern int errno;

/*  */

void
asprintf(bp, ap)			/* what, fmt, args, ... */
	char *bp;
	va_list ap;
{
	char *what;

	what = va_arg(ap, char *);

	_asprintf(bp, what, ap);
}

#ifdef X25
unsigned char isode_x25_err[2];
char isode_x25_errflag = 0;
#endif

void
_asprintf(bp, what, ap)			/* fmt, args, ... */
	register char *bp;
	char *what;
	va_list ap;
{
	register int eindex;
	char *fmt;

	eindex = errno;

	*bp = NULL;
	fmt = va_arg(ap, char *);

	if (fmt) {
#ifndef	VSPRINTF
		struct _iobuf iob;
#endif

#ifndef	VSPRINTF
#ifdef	pyr
		bzero((char *) &iob, sizeof iob);
		iob._file = _NFILE;
#endif
		iob._flag = _IOWRT | _IOSTRG;
#if	!defined(vax) && !defined(pyr)
		iob._ptr = (unsigned char *) bp;
#else
		iob._ptr = bp;
#endif
		iob._cnt = BUFSIZ;
		_doprnt(fmt, ap, &iob);
		putc(NULL, &iob);
#else
		(void) vsprintf(bp, fmt, ap);
#endif
		bp += strlen(bp);

	}

	if (what) {
		if (*what) {
			(void) sprintf(bp, " %s: ", what);
			bp += strlen(bp);
		}
		(void) strcpy(bp, sys_errname(eindex));
		bp += strlen(bp);

#ifdef X25
		if (isode_x25_errflag) {
			(void) sprintf(bp, " (%02x %02x)", isode_x25_err[0], isode_x25_err[1]);
			bp += strlen(bp);
		}
#endif

	}

	errno = eindex;
}
