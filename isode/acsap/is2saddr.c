/* is2saddr.c - old-style S-ADDR lookup */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/is2saddr.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/is2saddr.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: is2saddr.c,v $
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

struct SSAPaddr *is2saddr (host, service, is)
char   *host,
       *service;
struct isoservent *is;
{
    AEI	    aei;
    struct PSAPaddr *pa;
    struct SSAPaddr *sa;

    if ((aei = str2aei (host, service)) == NULLAEI
	    || (pa = aei2addr (aei)) == NULLPA)
	return NULLSA;

    sa = &pa -> pa_addr;
    if (is && strcmp (is -> is_provider, "ssap") == 0) {
	if (is -> is_selectlen > SSSIZE)	/* XXX */
	    return NULLSA;

	bcopy (is -> is_selector, sa -> sa_selector,
		sa -> sa_selectlen = is -> is_selectlen);
    }

    return sa;
}
