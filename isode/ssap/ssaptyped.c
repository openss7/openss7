/* ssaptyped.c - SPM: write typed data */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ssap/RCS/ssaptyped.c,v 9.0 1992/06/16 12:39:41 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ssap/RCS/ssaptyped.c,v 9.0 1992/06/16 12:39:41 isode Rel $
 *
 *
 * $Log: ssaptyped.c,v $
 * Revision 9.0  1992/06/16  12:39:41  isode
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
#include <signal.h>
#include "spkt.h"

/*    S-TYPED-DATA.REQUEST */

int	STypedRequest (sd, data, cc, si)
int	sd;
char   *data;
int	cc;
struct SSAPindication *si;
{
    SBV	    smask;
    int     result;
    struct udvec uvs[2];
    register struct udvec *uv = uvs;
    register struct ssapblk *sb;

    missingP (data);
    if (cc <= 0)
	return ssaplose (si, SC_PARAMETER, NULLCP,
		    "illegal value for TSSDU length (%d)", cc);
    missingP (si);

    smask = sigioblock ();

    ssapPsig (sb, sd);

    uv -> uv_base = data, uv -> uv_len = cc, uv++;
    uv -> uv_base = NULL;

    result = SDataRequestAux (sb, SPDU_TD, uvs, 1, 1, si);

    (void) sigiomask (smask);

    return result;
}
