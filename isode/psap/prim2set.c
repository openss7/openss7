/* prim2flag.c - presentation element to set */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/prim2set.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/prim2set.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: prim2set.c,v $
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

PE	prim2set (pe)
register PE	pe;
{
    register int    i;
    register PE	    p;

    if (pe -> pe_form != PE_FORM_CONS)
	return pe_seterr (pe, PE_ERR_CONS, NULLPE);

    for (i = 0, p = pe -> pe_cons; p; p = p -> pe_next)
	p -> pe_offset = i++;

    pe -> pe_cardinal = i;

    return pe;
}
