/* rosapasync.c - ROPM: set asynchronous events */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosap/RCS/rosapasync.c,v 9.0 1992/06/16 12:37:02 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosap/RCS/rosapasync.c,v 9.0 1992/06/16 12:37:02 isode Rel $
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * $Log: rosapasync.c,v $
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

/*    define vectors for INDICATION events */

int	RoSetIndications (sd, indication, roi)
int	sd;
IFP	indication;
struct RoSAPindication *roi;
{
    SBV	    smask;
    int     result;
    register struct assocblk   *acb;

    _iosignals_set = 1;

    smask = sigioblock ();

    rosapPsig (acb, sd);

    if (acb -> acb_apdu || (acb -> acb_flags & ACB_CLOSING)) {
	(void) sigiomask (smask);
	return rosaplose (roi, ROS_WAITING, NULLCP, NULLCP);
    }

    result = (*acb -> acb_rosetindications) (acb, indication, roi);

    (void) sigiomask (smask);

    return result;
}
