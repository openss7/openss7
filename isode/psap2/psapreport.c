/* psapreport.c - PPM: exception reports */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap2/RCS/psapreport.c,v 9.0 1992/06/16 12:29:42 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap2/RCS/psapreport.c,v 9.0 1992/06/16 12:29:42 isode Rel $
 *
 *
 * $Log: psapreport.c,v $
 * Revision 9.0  1992/06/16  12:29:42  isode
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
#include "ppkt.h"

/*    P-U-EXCEPTION-REPORT.REQUEST */

int	PUReportRequest (sd, reason, data, ndata, pi)
int	sd;
int	reason,
	ndata;
PE     *data;
struct PSAPindication *pi;
{
    SBV	    smask;
    int     len,
	    result;
    char   *base,
	   *realbase;
    register struct psapblk *pb;
    struct SSAPindication   sis;
    register struct SSAPabort  *sa = &sis.si_abort;

    toomuchP (data, ndata, NPDATA, "report");
    missingP (pi);

    smask = sigioblock ();

    psapPsig (pb, sd);

    if ((result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
			     "P-U-EXCEPTION-REPORT user-data", PPDU_NONE))
	    != OK)
	goto out2;

    if ((result = SUReportRequest (sd, reason, base, len, &sis)) == NOTOK)
	if (SC_FATAL (sa -> sa_reason))
	    (void) ss2pslose (pb, pi, "SUReportRequest", sa);
	else {
	    (void) ss2pslose (NULLPB, pi, "SUReportRequest", sa);
	    goto out1;
	}

out2: ;
    if (result == NOTOK)
	freepblk (pb);
    else
	if (result == DONE)
	    result = NOTOK;
out1: ;
    if (realbase)
	free (realbase);
    else
	if (base)
	    free (base);

    (void) sigiomask (smask);

    return result;
}
