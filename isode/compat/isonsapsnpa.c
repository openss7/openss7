/* isonsapsnpa.c - look-up ISODE NSAP->SNP binding */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/isonsapsnpa.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/isonsapsnpa.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: isonsapsnpa.c,v $
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

#include <ctype.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"
#include "tailor.h"
#include "cmd_srch.h"

/*    DATA */

static char *isonsapsnpa = "isonsapsnpa";

static FILE *servf = NULL;
static int  stayopen = 0;

static CMD_TABLE tbl_communities[] = {
    "cons",	NAS_CONS,
    "clns",	NAS_CLNS,
    NULLCP,	NOTOK
    };

/*  */

int	setisonsapsnpa (f)
int	f;
{
    if (servf == NULL)
	servf = fopen (isodefile (isonsapsnpa, 0), "r");
    else
	rewind (servf);
    stayopen |= f;

    return (servf != NULL);
}


int	endisonsapsnpa () {
    if (servf && !stayopen) {
	(void) fclose (servf);
	servf = NULL;
    }

    return 1;
}

/*  */

struct NSAPinfo  *getisonsapsnpa () 
{
    int	    mask,
	    vecp;
    register char  *cp;
    static struct NSAPinfo iss;
    register struct NSAPinfo *is = &iss;
    static char buffer[BUFSIZ + 1];
    static char *vec[NVEC + NSLACK + 1];
    struct TSAPaddr *ta;

    if (servf == NULL
	    && (servf = fopen (isodefile (isonsapsnpa, 0), "r")) == NULL)
	return NULL;

    bzero ((char *) is, sizeof *is);

    while (fgets (buffer, sizeof buffer, servf) != NULL) {
	if (*buffer == '#')
	    continue;
	if (cp = index (buffer, '\n'))
	    *cp = NULL;
	if ((vecp = str2vecX (buffer, vec, 1 + 1, &mask, NULL, 1)) < 1)
	    continue;

	ta = str2taddr (vec[0]);
	if (ta == NULLTA)
		continue;
	if (ta -> ta_naddr != 1 ||
	    ta -> ta_addrs[0].na_stack != NA_NSAP)
		continue;
	bcopy (ta -> ta_addrs[0].na_address, is -> is_prefix,
	       is -> is_plen = ta -> ta_addrs[0].na_addrlen);
	if (strcmp(vec[1], "-") == 0)
	    is -> is_nsap.na_stack = NOTOK;
	else {
	    if ((ta = str2taddr (vec[1])) == NULLTA || ta -> ta_naddr < 1)
		continue;
	    is -> is_nsap = ta -> ta_addrs[0]; /* struct copy */
	}

	if (vecp > 1)
	    switch (mask = cmd_srch (vec[2], tbl_communities)) {
	    case NAS_CONS:
	    case NAS_CLNS:
		is -> is_stack = mask;
		break;
	    default:
		is -> is_stack = nsap_default_stack;
		break;
	    }
	else
	    is -> is_stack = nsap_default_stack;

	if (vecp > 2 && strcmp(vec[2],"-") != 0)
	    (void) strncpy (is -> is_class, vec[2], sizeof (is -> is_class));
	else is -> is_class[0] = '\0';

	return is;
    }

    return NULL;
}

struct NSAPaddr *getisosnpa (nsap)
struct NSAPaddr *nsap;
{
	struct NSAPinfo *is;
	int best = -1;
	static struct NSAPinfo nss;
	struct NSAPinfo *ns = &nss;

	if (nsap -> na_stack != NA_NSAP)
		return NULLNA;

	(void) setisonsapsnpa (0);

	while (is = getisonsapsnpa ()) {
		if (is -> is_nsap.na_stack == NOTOK || is -> is_plen < best)
			continue;
		if (bcmp (nsap -> na_address, is -> is_prefix,
			  is -> is_plen) == 0) {
			best = is -> is_plen;
			*ns = *is; /* struct copy */
		}
	}
	(void) endisonsapsnpa ();

	if (best >= 0)
		return &ns -> is_nsap;
	return NULLNA;
}

struct NSAPinfo *getnsapinfo (nsap)
struct NSAPaddr *nsap;
{
	struct NSAPinfo *is;
	int best = -1;
	static struct NSAPinfo nss;
	struct NSAPinfo *ns = &nss;

	if (nsap -> na_stack != NA_NSAP)
		return NULLNI;

	(void) setisonsapsnpa (0);

	while (is = getisonsapsnpa ()) {
		if (is -> is_plen < best)
			continue;
		if (bcmp (nsap -> na_address, is -> is_prefix,
			  is -> is_plen) == 0) {
			best = is -> is_plen;
			
			*ns = *is; /* struct copy */
		}
	}
	(void) endisonsapsnpa ();

	if (best >= 0)
		return ns;
	return NULLNI;
}
