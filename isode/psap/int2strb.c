/* int2strb.c - integer to string of bits */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/int2strb.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/int2strb.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: int2strb.c,v $
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

char   *int2strb (n, len)
register int    n;
int     len;
{
    register int    i;
    static char buffer[sizeof (int) + 1];

    bzero (buffer, sizeof (buffer));
    for (i = 0; i < len; i++)
	if (n & (1 << i))
	    buffer[i / 8] |= (1 << (7 - (i % 8)));

    return buffer;
}
