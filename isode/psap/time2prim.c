/* time2prim.c - time string to presentation element */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/time2prim.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/time2prim.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: time2prim.c,v $
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

PE	time2prim (u, generalized, class, id)
register UTC	u;
int	generalized;
PElementClass	class;
PElementID	id;
{
    register int    len;
    register char  *bp;
    register PE	    pe;

    if ((bp = time2str (u, generalized)) == NULLCP)
	return NULLPE;

    if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
	return NULLPE;

    if ((pe -> pe_prim = PEDalloc (len = strlen (bp))) == NULLPED) {
	pe_free (pe);
	return NULLPE;
    }
    PEDcpy (bp, pe -> pe_prim, pe -> pe_len = len);

    return pe;
}
