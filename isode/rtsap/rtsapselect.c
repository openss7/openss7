/* rtsapselect.c - RTPM: map descriptors */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rtsap/RCS/rtsapselect.c,v 9.0 1992/06/16 12:37:45 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rtsap/RCS/rtsapselect.c,v 9.0 1992/06/16 12:37:45 isode Rel $
 *
 *
 * $Log: rtsapselect.c,v $
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

/*    map association descriptors for select() */

int	RtSelectMask (sd, mask, nfds, rti)
int	sd;
fd_set *mask;
int    *nfds;
struct RtSAPindication *rti;
{
    SBV	    smask;
    int     result;
    register struct assocblk   *acb;

    missingP (mask);
    missingP (nfds);
    missingP (rti);

    smask = sigioblock ();

    rtsapPsig (acb, sd);

    if (acb -> acb_flags & ACB_PLEASE) {
	(void) sigiomask (smask);

	return rtsaplose (rti, RTS_WAITING, NULLCP, NULLCP);
    }

    result = (*acb -> acb_rtselectmask) (acb, mask, nfds, rti);

    (void) sigiomask (smask);

    return result;
}
