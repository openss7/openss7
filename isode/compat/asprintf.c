/* asprintf.c - sprintf with errno */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/asprintf.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/asprintf.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: asprintf.c,v $
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

void	asprintf (bp, ap)		/* what, fmt, args, ... */
char *bp;
va_list	ap;
{
    char   *what;

    what = va_arg (ap, char *);

    _asprintf (bp, what, ap);
}

#ifdef X25
unsigned char isode_x25_err[2];
char isode_x25_errflag = 0;
#endif

void	_asprintf (bp, what, ap)	/* fmt, args, ... */
register char *bp;
char   *what;
va_list	ap;
{
    register int    eindex;
    char   *fmt;

    eindex = errno;

    *bp = NULL;
    fmt = va_arg (ap, char *);

    if (fmt) {
#ifndef	VSPRINTF
	struct _iobuf iob;
#endif

#ifndef	VSPRINTF
#ifdef	pyr
	bzero ((char *) &iob, sizeof iob);
	iob._file = _NFILE;
#endif
	iob._flag = _IOWRT | _IOSTRG;
#if	!defined(vax) && !defined(pyr)
	iob._ptr = (unsigned char *) bp;
#else
	iob._ptr = bp;
#endif
	iob._cnt = BUFSIZ;
	_doprnt (fmt, ap, &iob);
	putc (NULL, &iob);
#else
	(void) vsprintf (bp, fmt, ap);
#endif
	bp += strlen (bp);

    }

    if (what) {
	if (*what) {
	    (void) sprintf (bp, " %s: ", what);
	    bp += strlen (bp);
	}
	(void) strcpy (bp, sys_errname (eindex));
	bp += strlen (bp);

#ifdef X25
	if (isode_x25_errflag) {	
		(void) sprintf (bp, " (%02x %02x)",isode_x25_err[0],isode_x25_err[1]);
		bp += strlen (bp);
	}
#endif

    }

    errno = eindex;
}
