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
#if defined HAVE_VARARGS_H
#include <varargs.h>
#else				/* defined HAVE_VARARGS_H */
#include <stdarg.h>
#endif				/* defined HAVE_VARARGS_H */
#include "general.h"
#include "manifest.h"

#ifdef __STDC__
#include <errno.h>
#else
extern int errno;
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_VARARGS_H
void
xsprintf(bp, ap)			/* what, fmt, args, ... */
	char *bp;
	va_list ap;
{
	char *what;
	what = va_arg(ap, char *);

	_xsprintf(bp, what, ap);
}

#ifdef X25
unsigned char isode_x25_err[2];
char isode_x25_errflag = 0;
#endif

void
_xsprintf(bp, what, ap)			/* fmt, args, ... */
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
#if !defined HAVE_VPRINTF
		struct _iobuf iob;

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
#else				/* !defined HAVE_VPRINTF */
		(void) vsprintf(bp, fmt, ap);
#endif				/* !defined HAVE_VPRINTF */
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

#else				/* HAVE_VARARGS_H */

void
xsprintf(register char *bp, const char *what, const char *fmt, ...)
{
	va_list ap;

	(void) rcsid;
	va_start(ap, fmt);
	_xsprintf(bp, what, fmt, ap);
	va_end(ap);
}

void
_xsprintf(char *bp, const char *what, const char *fmt, va_list ap)
{
	register int eindex;

	eindex = errno;
	*bp = '\0';
	if (fmt) {
		/* If you have stdarg.h you have vsprintf(). */
		(void) vsprintf(bp, fmt, ap);
		bp += strlen(bp);
	}
	if (what) {
		if (*what) {
			(void) sprintf(bp, " %s: ", what);
			bp += strlen(bp);
		}
		(void) strcpy(bp, sys_errname(eindex));	/* works out to strerror() */
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

#endif				/* HAVE_VARARGS_H */
