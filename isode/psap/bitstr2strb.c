/* bitstr2strb.c - bit string to string of bits */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/bitstr2strb.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/bitstr2strb.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: bitstr2strb.c,v $
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

char   *bitstr2strb (pe, k)
PE	pe;
int    *k;
{
    register int    i,
		    j,
		    len,
		    bit,
		    mask;
    register char   *dp;
    char   *cp;

    if (pe == NULLPE)
	return NULLCP;

    *k = len = pe -> pe_nbits;
    if ((cp = dp = calloc (1, (unsigned) (len / 8 + 2))) == NULLCP)
	return NULLCP;

    for (bit = i = 0, mask = 1 << (j = 7); i < len; i++) {
	if (bit_test (pe, i))
	    bit |= mask;
	if (j-- == 0)
	    *dp++ = bit & 0xff, bit = 0, mask = 1 << (j = 7);
	else
	    mask >>= 1;
    }
    if (j != 7)
	*dp = bit & 0xff;

    return cp;
}
