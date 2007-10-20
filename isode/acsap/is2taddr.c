/* is2taddr.c - old-style T-ADDR lookup */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/is2taddr.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/is2taddr.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: is2taddr.c,v $
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

struct TSAPaddr *is2taddr (host, service, is)
char   *host,
       *service;
struct isoservent *is;
{
    AEI	    aei;
    struct PSAPaddr *pa;
    struct TSAPaddr *ta;

    if ((aei = str2aei (host, service)) == NULLAEI
	    || (pa = aei2addr (aei)) == NULLPA)
	return NULLTA;

    ta = &pa -> pa_addr.sa_addr;
    if (is && strcmp (is -> is_provider, "tsap") == 0) {
	if (is -> is_selectlen > TSSIZE)	/* XXX */
	    return NULLTA;

	bcopy (is -> is_selector, ta -> ta_selector,
		ta -> ta_selectlen = is -> is_selectlen);
    }
	    
    return ta;
}
