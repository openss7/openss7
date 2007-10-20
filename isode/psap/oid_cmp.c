/* oid_cmp.c - compare two object identifiers */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/oid_cmp.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/oid_cmp.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: oid_cmp.c,v $
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

int	oid_cmp (p, q)
register OID	p,
		q;
{
    if (p == NULLOID)
	return (q ? -1 : 0);

    return elem_cmp (p -> oid_elements, p -> oid_nelem,
		     q -> oid_elements, q -> oid_nelem);
}

/*  */

int	elem_cmp (ip, i, jp, j)
register int   i,
	       j;
register unsigned int *ip,
		      *jp;
{
    while (i > 0) {
	if (j == 0)
	    return 1;
	if (*ip > *jp)
	    return 1;
	else
	    if (*ip < *jp)
		return (-1);

	ip++, i--;
	jp++, j--;
    }
    return (j == 0 ? 0 : -1);
}
