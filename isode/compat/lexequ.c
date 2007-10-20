/* lexequ.c - Compare two strings ignoring case */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/lexequ.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/lexequ.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: lexequ.c,v $
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

/*  */

lexequ (str1, str2)
register char   *str1,
		*str2;
{
    if (str1 == NULL)
	if (str2 == NULL)
		return (0);
	else
		return (-1);

    if (str2 == NULL)
	return (1);

    while (chrcnv[*str1] == chrcnv[*str2]) {
	if (*str1++ == NULL)
	    return (0);
	str2++;
    }

    if (chrcnv[*str1] > chrcnv[*str2])
	return (1);
    else
	return (-1);
}
