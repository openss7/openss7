/* seq_del.c - delete a member from a sequence */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/seq_del.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/seq_del.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: seq_del.c,v $
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

int	seq_del (pe, i)
register PE	pe;
register int	i;
{
    int	    offset;
    register PE	   *p,
		    q;

    for (p = &pe -> pe_cons, offset = 0;
	    q = *p;
	    p = &q -> pe_next, offset = q -> pe_offset)
	if (q -> pe_offset == i) {
	    if (((*p) = q -> pe_next) == NULLPE)
		pe -> pe_cardinal = offset + 1;
	    pe_free (q);
	    return OK;
	}
	else
	    if (q -> pe_offset > i)
		break;
	
    return pe_seterr (pe, PE_ERR_MBER, NOTOK);
}
