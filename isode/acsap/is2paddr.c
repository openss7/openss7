/* is2paddr.c - old-style P-ADDR lookup */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/is2paddr.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/is2paddr.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: is2paddr.c,v $
 * Revision 9.0  1992/06/16  12:05:59  isode
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
#include "isoaddrs.h"
#include "isoservent.h"

/*  */

struct PSAPaddr *is2paddr (host, service, is)
char   *host,
       *service;
struct isoservent *is;
{
    AEI	    aei;
    struct PSAPaddr *pa;

    if ((aei = str2aei (host, service)) == NULLAEI
	    || (pa = aei2addr (aei)) == NULLPA)
	return NULLPA;

    if (is && strcmp (is -> is_provider, "psap") == 0) {
	if (is -> is_selectlen > PSSIZE)	/* XXX */
	    return NULLPA;

	bcopy (is -> is_selector, pa -> pa_selector,
		pa -> pa_selectlen = is -> is_selectlen);
    }

    return pa;
}
