/* set_find.c - find member of a set */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/set_find.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/set_find.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: set_find.c,v $
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

PE	set_find (pe, class, id)
register PE	pe;
register PElementClass class;
register PElementID id;
{
    register int    pe_id;
    register PE	    p;
    PE pfound = NULLPE;

    pe_id = PE_ID (class, id);
    for (p = pe -> pe_cons; p; p = p -> pe_next)
	if (PE_ID (p -> pe_class, p -> pe_id) == pe_id) {
	    if (pfound == NULLPE)
		pfound = p;
	    else 
		return pe_seterr (pe, PE_ERR_DUPLICATE, NULLPE);
	}
    return pfound;
}
