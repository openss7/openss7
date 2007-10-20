/* nsapcmp.c - compare NSAPs */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/nsapcmp.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/nsapcmp.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: nsapcmp.c,v $
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

int nsap_addr_cmp (na1, na2)
struct NSAPaddr *na1, *na2;
{
    if (na1 -> na_stack != na2 -> na_stack)
	return NOTOK;

    switch (na1 -> na_stack) {
    case NA_NSAP:
	if (na1 -> na_addrlen == 0 ||
	    (na1 -> na_addrlen == na2 -> na_addrlen &&
	     bcmp (na1 -> na_address, na2 -> na_address,
		   na1 -> na_addrlen) == 0))
	    return OK;
	break;

    case NA_TCP:
	if (na1 -> na_port == na2 -> na_port &&
	    strcmp (na1 -> na_domain, na2 -> na_domain) == 0)
	    return OK;
	break;

    case NA_X25:
	if (na1 -> na_dtelen == na2 -> na_dtelen &&
	    bcmp (na1 -> na_dte, na2 -> na_dte,
		  na1 -> na_dtelen) == 0 &&
	    na1 -> na_pidlen == na2 -> na_pidlen &&
	    bcmp (na1 -> na_pid, na2 -> na_pid,
		  na1 -> na_pidlen) == 0)
	    return OK;
	break;
    }
    return NOTOK;
}

int tsap_addr_cmp (ta, tb)
struct TSAPaddr *ta, *tb;
{
    struct NSAPaddr *na1, *na2;
    int n1, n2;

    if (ta -> ta_selectlen != tb -> ta_selectlen ||
	bcmp (ta -> ta_selector, tb -> ta_selector, ta -> ta_selectlen) != 0)
	return NOTOK;
    if (ta -> ta_naddr == 0 && tb -> ta_naddr == 0)
	return OK;
    for (na1 = ta -> ta_addrs, n1 = ta -> ta_naddr; n1-- > 0; na1++) {
	for (na2 = tb -> ta_addrs, n2 = tb -> ta_naddr; n2 -- > 0; na2 ++)
	    if (nsap_addr_cmp (na1, na2) == OK)
		return OK;
    }
    return NOTOK;
}

int ssap_addr_cmp (sa, sb)
struct SSAPaddr *sa, *sb;
{
    if (sa -> sa_selectlen != sb -> sa_selectlen ||
	bcmp (sa -> sa_selector, sb -> sa_selector, sa -> sa_selectlen) != 0)
	return NOTOK;
    return tsap_addr_cmp (&sa -> sa_addr, &sb -> sa_addr);
}

int psap_addr_cmp (pa, pb)
struct PSAPaddr *pa, *pb;
{
    if (pa -> pa_selectlen != pb -> pa_selectlen ||
	bcmp (pa -> pa_selector, pb -> pa_selector, pa -> pa_selectlen) != 0)
	return NOTOK;
    return ssap_addr_cmp (&pa -> pa_addr, &pb -> pa_addr);
}
