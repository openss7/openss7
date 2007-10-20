/* saddr2str.c - SSAPaddr to string value */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/saddr2str.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/saddr2str.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: saddr2str.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

/*  */

char   *saddr2str (sa)
register struct SSAPaddr *sa;
{
    struct PSAPaddr pas;
    register struct PSAPaddr *pa = &pas;

    if (!sa)
	return NULL;
    bzero ((char *) pa, sizeof *pa);
    pa -> pa_addr = *sa;		/* struct copy */

    return paddr2str (pa, NULLNA);
}
