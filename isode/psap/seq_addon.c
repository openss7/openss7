/* seq_addon.c - add a member to the end of a sequence (efficiency hack) */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/seq_addon.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/seq_addon.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: seq_addon.c,v $
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

int	seq_addon (pe, last, new)
PE	pe,
        last,
        new;
{
    if (pe == NULLPE)
	return NOTOK;
    if (last == NULLPE)
	return seq_add (pe, new, -1);
    new -> pe_offset = pe -> pe_cardinal++;
    last -> pe_next = new;
    return OK;
}
