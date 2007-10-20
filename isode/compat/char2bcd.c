/* char2bcd.c - convert to BCD (for X.25 and others) */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/char2bcd.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/char2bcd.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 * $Log: char2bcd.c,v $
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

#include "general.h"

int  char2bcd (s, n, d)
register char   *s;
int n;
register u_char *d;
{
    register int    c,
		    i;

    for (i = 0; *s && n-- > 0; i++) {
	if ((c = *s++) >= 'a' && c <= 'f')
	    c -= 'a' - 0x0a;
	else
	    if (c >= 'A' && c <= 'F')
		c -= 'A' - 0x0a;
	    else
		if (c >= '0' && c <= '9')
		    c -= '0';
		else
		    c = 0;

	if (i & 1)
	    *d++ |= c & 0xf;
	else
	    *d = (c & 0xf) << 4;
    }

    return i;
}
