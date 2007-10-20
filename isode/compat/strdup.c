/* strdup.c - create a duplicate copy of the given string */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/strdup.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/strdup.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: strdup.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "tailor.h"

/*  */
#if !defined(SVR4) || defined(_AIX)
                      /* strdup in AIX does not seem to use QUIPUs malloc! */

char   *strdup (str)
register char   *str;
{
	register char *ptr;

	if ((ptr = malloc((unsigned) (strlen (str) + 1))) == NULL){
	    LLOG (compat_log,LLOG_FATAL, ("strdup malloc() failure"));
	    abort ();
	    /* NOTREACHED */
	}

	(void) strcpy (ptr, str);

	return ptr;
}

#else

strdup_stub ()
{;}

#endif
