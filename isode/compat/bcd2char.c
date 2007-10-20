/* char2bcd.c - convert to BCD (for X.25 and others) */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/bcd2char.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/bcd2char.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 * $Log: bcd2char.c,v $
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

int     bcd2char (s, d, len)
register    u_char *s;
register char  *d;
int     len;
{
    register int    i,
		    g;

    for (i = 0; i < len; i++) {
	g = s[i >> 1];
	if ((i & 1) == 0)
	    g >>= 4;
	g &= 0xf;

	if (g < 0x0a)
	    *d++ = g + '0';
	else
	    *d++ = g + 'a' - 0x0a;
    }

    *d = 0;

    return len;
}
