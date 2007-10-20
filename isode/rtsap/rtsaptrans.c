/* rtsaptrans.c - RTPM: transfer */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rtsap/RCS/rtsaptrans.c,v 9.0 1992/06/16 12:37:45 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rtsap/RCS/rtsaptrans.c,v 9.0 1992/06/16 12:37:45 isode Rel $
 *
 *
 * $Log: rtsaptrans.c,v $
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

/*    RT-TRANSFER.REQUEST */

int	RtTransferRequest (sd, data, secs, rti)
int	sd;
PE	data;
int	secs;
struct RtSAPindication *rti;
{
    SBV	    smask;
    int     result;
    register struct assocblk   *acb;

    missingP (rti);

    smask = sigioblock ();

    rtsapPsig (acb, sd);

    if (data == NULLPE && acb -> acb_downtrans == NULLIFP) {
	(void) sigiomask (smask);
	return rtsaplose (rti, RTS_PARAMETER, NULLCP,
			  "mandatory parameter \"data\" missing");
    }

    result = (*acb -> acb_transferequest)  (acb, data, secs, rti);

    (void) sigiomask (smask);

    return result;
}
