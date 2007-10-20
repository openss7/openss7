/* sprintoid.c - object identifier to string */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/sprintoid.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/sprintoid.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: sprintoid.c,v $
 * Revision 9.0  1992/06/16  12:25:44  isode
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
#include "psap.h"

/*  */

char   *sprintoid (oid)
register OID oid;
{
    register int    i;
    register unsigned int  *ip;
    register char  *bp,
		   *cp;
    static char buffer[BUFSIZ];

    if (oid == NULLOID || oid -> oid_nelem < 1)
	return "";

    bp = buffer;

    for (ip = oid -> oid_elements, i = oid -> oid_nelem, cp = "";
	    i-- > 0;
	    ip++, cp = ".") {
	(void) sprintf (bp, "%s%u", cp, *ip);
	bp += strlen (bp);
    }

    return buffer;
}
