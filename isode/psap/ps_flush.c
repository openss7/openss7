/* ps_flush.c - flush a presentation stream */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/ps_flush.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/ps_flush.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: ps_flush.c,v $
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

int	ps_flush (ps)
register PS	ps;
{
    if (ps -> ps_flushP)
	return (*ps -> ps_flushP) (ps);

    return OK;
}
