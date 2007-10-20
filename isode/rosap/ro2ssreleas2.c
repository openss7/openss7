/* ro2ssreleas2.c - respond to release */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosap/RCS/ro2ssreleas2.c,v 9.0 1992/06/16 12:37:02 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosap/RCS/ro2ssreleas2.c,v 9.0 1992/06/16 12:37:02 isode Rel $
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * $Log: ro2ssreleas2.c,v $
 * Revision 9.0  1992/06/16  12:37:02  isode
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
#include "ropkt.h"

static int  RoEndResponseAux ();

/*    RO-END.RESPONSE */

int	RoEndResponse (sd, roi)
int	sd;
struct RoSAPindication *roi;
{
    SBV	    smask;
    int     result;
    register struct assocblk   *acb;

    missingP (roi);

    smask = sigioblock ();

    rosapFsig (acb, sd);

    result = RoEndResponseAux (acb, roi);

    (void) sigiomask (smask);

    return result;

}

/*  */

static int  RoEndResponseAux (acb, roi)
register struct assocblk   *acb;
struct RoSAPindication *roi;
{
    int     result;
    struct SSAPindication   sis;
    register struct SSAPindication *si = &sis;
    register struct SSAPabort *sa = &si -> si_abort;

    if (SRelResponse (acb -> acb_fd, SC_ACCEPT, NULLCP, 0, si) == NOTOK)
	result = ss2roslose (acb, roi, "SRelResponse", sa);
    else {
	acb -> acb_fd = NOTOK;
	result = OK;
    }

    freeacblk (acb);

    return result;
}
