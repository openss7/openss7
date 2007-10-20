/* ryfind.c - ROSY: find operations and errors by numbers and names */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosy/RCS/ryfind.c,v 9.0 1992/06/16 12:37:29 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosy/RCS/ryfind.c,v 9.0 1992/06/16 12:37:29 isode Rel $
 *
 *
 * $Log: ryfind.c,v $
 * Revision 9.0  1992/06/16  12:37:29  isode
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
#include "rosy.h"

/*  */

struct RyOperation *findopbyop (ryo, op)
register struct RyOperation *ryo;
int	op;
{
    if (!ryo)
	return NULL;

    for (; ryo -> ryo_name; ryo++)
	if (ryo -> ryo_op == op)
	    return ryo;

    return NULL;
}


struct RyOperation *findopbyname (ryo, name)
register struct RyOperation *ryo;
char   *name;
{
    if (!ryo)
	return NULL;

    for (; ryo -> ryo_name; ryo++)
	if (strcmp (ryo -> ryo_name, name) == 0)
	    return ryo;

    return NULL;
}


struct RyError *finderrbyerr (rye, err)
register struct RyError *rye;
int	err;
{
    if (!rye)
	return NULL;

    for (; rye -> rye_name; rye++)
	if (rye -> rye_err == err)
	    return rye;

    return NULL;
}


struct RyError *finderrbyname (rye, name)
register struct RyError *rye;
char   *name;
{
    if (!rye)
	return NULL;

    for (; rye -> rye_name; rye++)
	if (strcmp (rye -> rye_name, name) == 0)
	    return rye;

    return NULL;
}
