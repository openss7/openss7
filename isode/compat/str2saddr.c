/* str2saddr.c - string value to SSAPaddr */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/str2saddr.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/str2saddr.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: str2saddr.c,v $
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

struct SSAPaddr *str2saddr (str)
char   *str;
{
    register struct PSAPaddr *pa;

    if (pa = str2paddr (str))
	return (&pa -> pa_addr);

    return NULLSA;
}
