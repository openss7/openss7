/* str2oid.c - string to object identifier */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/str2oid.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/str2oid.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: str2oid.c,v $
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

OID	str2oid (s)
char    *s;
{
    int	    i;
    static struct OIDentifier   oids;
    static unsigned int elements[NELEM + 1];

    if ((i = str2elem (s, elements)) < 1
	    || elements[0] > 2
	    || (i > 1 && elements[0] < 2 && elements[1] > 39))
	return NULLOID;

    oids.oid_elements = elements, oids.oid_nelem = i;

    return (&oids);
}
