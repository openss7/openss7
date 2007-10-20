/* isofiles.c - ISODE files */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/isofiles.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/isofiles.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: isofiles.c,v $
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
#include "general.h"
#include "manifest.h"
#include "tailor.h"

/*  */

char   *_isodefile (path, file)
char   *path,
       *file;
{
    static char buffer[BUFSIZ];

    isodetailor (NULLCP, 0);	/* not really recursive */

    if (*file == '/'
	    || (*file == '.'
		    && (file[1] == '/'
			    || (file[1] == '.' && file[2] == '/'))))
	(void) strcpy (buffer, file);
    else
	(void) sprintf (buffer, "%s%s", path, file);

    return buffer;
}
