/* dse-c.c - DSE wrapper for pepsy */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/dse-c.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/dse-c.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: dse-c.c,v $
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
#include "tailor.h"
#include "DSE-types.h"


#define advise	PY_advise


static struct type_DSE_PSAPaddr *psap2dse ();

static int  dse2psap ();
static int  gstring ();

/*  */

int	build_DSE_PSAPaddr (pe, explicit, len, buffer, parm)
PE     *pe;
int     explicit;
int     len;
char   *buffer;
char	*parm;
{
    int	    result;
    register struct PSAPaddr *pa = (struct PSAPaddr *) parm;
    struct type_DSE_PSAPaddr *dse;

    if ((dse = psap2dse (pa)) == NULL)
	return NOTOK;

    result = encode_DSE_PSAPaddr (pe, explicit, len, buffer, dse);

    free_DSE_PSAPaddr (dse);

    return result;
}

/*  */

static struct type_DSE_PSAPaddr *psap2dse (pa)
register struct PSAPaddr *pa;
{
    register int    n;
    register struct type_DSE_PSAPaddr *dse;
    register struct SSAPaddr *sa = &pa -> pa_addr;
    register struct TSAPaddr *ta = &sa -> sa_addr;
    register struct NSAPaddr *na;
    register struct member_DSE_0  *nDSE,
				 **oDSE;

    if ((dse = (struct type_DSE_PSAPaddr *) calloc (1, sizeof *dse)) == NULL) {
	advise (NULLCP, "psap2dse: out of memory");
	return NULL;
    }

    if (pa -> pa_selectlen > 0)
	dse -> pSelector = str2qb (pa -> pa_selector, pa -> pa_selectlen, 1);
    
    if (sa -> sa_selectlen > 0)
	dse -> sSelector = str2qb (sa -> sa_selector, sa -> sa_selectlen, 1);
    
    if (ta -> ta_selectlen > 0)
	dse -> tSelector = str2qb (ta -> ta_selector, ta -> ta_selectlen, 1);

    oDSE = &dse -> nAddress;
    for (na = ta -> ta_addrs, n = ta -> ta_naddr; n > 0; na++, n--) {
	register struct NSAPaddr *ca;

	if ((nDSE = (struct member_DSE_0 *) calloc (1, sizeof *nDSE))
	        == NULL) {
	    advise (NULLCP, "psap2dse: out of memory");
losing: ;
	    free_DSE_PSAPaddr (dse);
	    return NULL;
	}
	*oDSE = nDSE;
	oDSE = &nDSE -> next;

	if ((ca = na2norm (na)) == NULLNA) {
	    advise (NULLCP, "unable to normalize address");
	    goto losing;
	}

	nDSE -> member_DSE_1 = str2qb (ca -> na_address, ca -> na_addrlen, 1);
    }
    
    return dse;
}

/*  */

int	parse_DSE_PSAPaddr (pe, explicit, len, buffer, parm)
register PE pe;
int	explicit;
int    *len;
char  **buffer;
char   *parm;
{
    int	    result;
    register struct PSAPaddr *pa = (struct PSAPaddr *) parm;
    struct type_DSE_PSAPaddr *dse;

    if (decode_DSE_PSAPaddr (pe, explicit, len, buffer, &dse) == NOTOK)
	return NOTOK;

    result = dse2psap (dse, pa);
	
    free_DSE_PSAPaddr (dse);

    return result;
}

/*  */

static int  dse2psap (dse, pa)
register struct type_DSE_PSAPaddr *dse;
register struct PSAPaddr *pa;
{
    register struct SSAPaddr *sa = &pa -> pa_addr;
    register struct TSAPaddr *ta = &sa -> sa_addr;
    register struct member_DSE_0 *nDSE;

    bzero ((char *) pa, sizeof *pa);

    pa -> pa_selectlen = gstring (pa -> pa_selector,
				  sizeof pa -> pa_selector,
				  dse -> pSelector, "psap selector");

    sa -> sa_selectlen = gstring (sa -> sa_selector, sizeof sa -> sa_selector,
				  dse -> sSelector, "ssap selector");

    ta -> ta_selectlen = gstring (ta -> ta_selector, sizeof ta -> ta_selector,
				  dse -> tSelector, "tsap selector");

    for (nDSE  = dse -> nAddress; nDSE; nDSE = nDSE -> next) {
	char   *p;

	if (ta -> ta_naddr >= NTADDR) {
	    advise (NULLCP, "too many network addresses");
	    return NOTOK;
	}

	p = qb2str (nDSE->member_DSE_1);
	if (norm2na (p, nDSE -> member_DSE_1 -> qb_len,
		     &ta -> ta_addrs[ta -> ta_naddr++]) == NOTOK)
	    return NOTOK;
	free (p);
    }

    return OK;
}

/*  */

static int  gstring (buf, buflen, qb, w)
char   *buf;
int	buflen;
struct qbuf *qb;
char	*w;
{
    char   *p;

    if (qb == NULL || qb -> qb_len <= 0)
	return 0;
    
    if (qb -> qb_len > buflen) {
	advise (NULLCP, "%s too long", w);
	return 0;
    }

    p = qb2str (qb);
    bcopy (p, buf, qb -> qb_len);
    free (p);

    return qb -> qb_len;
}
