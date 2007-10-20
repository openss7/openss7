/* seq_find.c - find an element in a sequence */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/seq_find.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/seq_find.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: seq_find.c,v $
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

PE	seq_find (pe, i)
register PE	pe;
register int	i;
{
    register PE	    p;

    if (i >= pe -> pe_cardinal)
	return pe_seterr (pe, PE_ERR_MBER, NULLPE);

    for (p = pe -> pe_cons; p; p = p -> pe_next)
	if (p -> pe_offset == i)
	    break;

    return p;
}
