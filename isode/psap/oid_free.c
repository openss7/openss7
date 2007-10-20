/* oid_free.c - free an object identifier */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap/RCS/oid_free.c,v 9.0 1992/06/16 12:25:44 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap/RCS/oid_free.c,v 9.0 1992/06/16 12:25:44 isode Rel $
 *
 *
 * $Log: oid_free.c,v $
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

int	oid_free (oid)
register OID oid;
{
    if (oid == NULLOID)
	return;

    if (oid -> oid_elements)
	free ((char *) oid -> oid_elements);

    free ((char *) oid);
}
