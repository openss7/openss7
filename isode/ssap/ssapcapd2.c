/* ssapcapd2.c - SPM: read capability data */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ssap/RCS/ssapcapd2.c,v 9.0 1992/06/16 12:39:41 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ssap/RCS/ssapcapd2.c,v 9.0 1992/06/16 12:39:41 isode Rel $
 *
 *
 * $Log: ssapcapd2.c,v $
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

/*    S-CAPABILITY-DATA.RESPONSE */

static  int  SCapdResponseAux ();

int	SCapdResponse (sd, data, cc, si)
int	sd;
char   *data;
int	cc;
struct SSAPindication *si;
{
    SBV	    smask;
    int     result;
    register struct ssapblk *sb;

    missingP (si);

    smask = sigioblock ();

    ssapPsig (sb, sd);
    toomuchP (sb, data, cc, SX_CDASIZE, "capability");

    result = SCapdResponseAux (sb, data, cc, si);

    (void) sigiomask (smask);

    return result;
}

/*  */

static  int  SCapdResponseAux (sb, data, cc, si)
register struct ssapblk *sb;
char   *data;
int	cc;
struct SSAPindication *si;
{
    int     result;

    if (!(sb -> sb_requirements & SR_CAPABILITY))
	return ssaplose (si, SC_OPERATION, NULLCP,
		"capability data exchange service unavailable");
    if (!(sb -> sb_flags & SB_CDA))
	return ssaplose (si, SC_OPERATION, NULLCP,
		"no capability data response in progress");

    if ((result = SWriteRequestAux (sb, SPDU_CDA, data, cc, 0, 0L, 0, NULLSD,
		NULLSD, NULLSR, si)) == NOTOK)
	freesblk (sb);
    else
	sb -> sb_flags &= ~SB_CDA;

    return result;
}
