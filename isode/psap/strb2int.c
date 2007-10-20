/* converts a bit string - output of bitstr2strb() - to an integer */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/strb2int.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/strb2int.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: strb2int.c,v $
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


int	strb2int (cp, len)
register char   *cp;
register int	len;
{
    register int    i,
		    j,
		    bit,
		    mask,
		    n;

    n = 0;
    for (bit = (*cp & 0xff), i = 0, mask = 1 << (j = 7); i < len; i++) {
	if (bit & mask)
	    n |= 1 << i;
	if (j-- == 0)
	    bit = *++cp & 0xff, mask = 1 << (j = 7);
	else
	    mask >>= 1;
    }

    return n;
}
