/* na2str.c - pretty-print NSAPaddr */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/na2str.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/na2str.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: na2str.c,v $
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

/*    Network Address to String */

char   *na2str (na)
register struct NSAPaddr *na;
{
    switch (na -> na_stack) {
	case NA_TCP: 
	    return na -> na_domain;

	case NA_X25:
	    return na -> na_dte;

	case NA_NSAP:
	default:
	    return sel2str (na -> na_address, na -> na_addrlen, 0);
    }
}
