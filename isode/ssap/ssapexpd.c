/* ssapexpd.c - SPM: write expedited data */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ssap/RCS/ssapexpd.c,v 9.0 1992/06/16 12:39:41 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ssap/RCS/ssapexpd.c,v 9.0 1992/06/16 12:39:41 isode Rel $
 *
 *
 * $Log: ssapexpd.c,v $
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

/*    S-EXPEDITED-DATA.REQUEST */

static int  SExpdRequestAux ();

int	SExpdRequest (sd, data, cc, si)
int	sd;
char   *data;
int	cc;
struct SSAPindication  *si;
{
    SBV	    smask;
    int     result;
    register struct ssapblk *sb;

    missingP (data);
    if (cc > SX_EXSIZE)
	return ssaplose (si, SC_PARAMETER, NULLCP,
			 "too much expedited user data, %d octets", cc);
    missingP (si);

    smask = sigioblock ();

    ssapPsig (sb, sd);

    result = SExpdRequestAux (sb, data, cc, si);

    (void) sigiomask (smask);

    return result;
}

/*  */

static int  SExpdRequestAux (sb, data, cc, si)
register struct ssapblk *sb;
char   *data;
int	cc;
struct SSAPindication  *si;
{
    int     result;
    register struct ssapkt *s;

    if (!(sb -> sb_requirements & SR_EXPEDITED))
	return ssaplose (si, SC_OPERATION, NULLCP,
		    "expedited data service unavailable");

    if ((s = newspkt (SPDU_EX)) == NULL)
	return ssaplose (si, SC_CONGEST, NULLCP, "out of memory");

    s -> s_udata = data, s -> s_ulen = cc;
    result = spkt2sd (s, sb -> sb_fd, 1, si);
    s -> s_udata = NULL, s -> s_ulen = 0;

    freespkt (s);

    if (result == NOTOK)
	freesblk (sb);

    return result;
}
