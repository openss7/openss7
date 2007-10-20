/* rtsapasync.c - RTPM: set asynchronous events */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rtsap/RCS/rtsapasync.c,v 9.0 1992/06/16 12:37:45 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rtsap/RCS/rtsapasync.c,v 9.0 1992/06/16 12:37:45 isode Rel $
 *
 *
 * $Log: rtsapasync.c,v $
 * Revision 9.0  1992/06/16  12:37:45  isode
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
#include "rtpkt.h"

/*    define vectors for INDICATION events */

int	RtSetIndications (sd, indication, rti)
int	sd;
IFP	indication;
struct RtSAPindication *rti;
{
    SBV	    smask;
    int     result;
    register struct assocblk   *acb;

    _iosignals_set = 1;

    smask = sigioblock ();

    rtsapPsig (acb, sd);

    if (acb -> acb_flags & ACB_PLEASE) {
	(void) sigiomask (smask);

	return rtsaplose (rti, RTS_WAITING, NULLCP, NULLCP);
    }

    result = (*acb -> acb_rtsetindications) (acb, indication, rti);

    (void) sigiomask (smask);

    return result;
}
