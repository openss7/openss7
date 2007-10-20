/* rygenid.c - ROSY: generate unique invoke ID */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosy/RCS/rygenid.c,v 9.0 1992/06/16 12:37:29 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosy/RCS/rygenid.c,v 9.0 1992/06/16 12:37:29 isode Rel $
 *
 *
 * $Log: rygenid.c,v $
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

/*    generate unique invoke ID */

/* ARGSUSED */

int	RyGenID (sd)
int	sd;
{
    static int	id = 0;

    return (++id);
}
