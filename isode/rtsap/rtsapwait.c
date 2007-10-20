/* rtsapwait.c - RTPM: wait for an indication */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rtsap/RCS/rtsapwait.c,v 9.0 1992/06/16 12:37:45 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rtsap/RCS/rtsapwait.c,v 9.0 1992/06/16 12:37:45 isode Rel $
 *
 *
 * $Log: rtsapwait.c,v $
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

/*    RT-WAIT.REQUEST (pseudo) */

int	RtWaitRequest (sd, secs, rti)
int	sd;
int	secs;
struct RtSAPindication *rti;
{
    SBV	    smask;
    int     result;
    register struct assocblk   *acb;

    missingP (rti);

    smask = sigioblock ();

    rtsapPsig (acb, sd);

    result = RtWaitRequestAux (acb, secs, 0, rti);

    (void) sigiomask (smask);

    return result;
}

/*  */

int	RtWaitRequestAux (acb, secs, trans, rti)
register struct assocblk   *acb;
int     secs,
        trans;
register struct RtSAPindication *rti;
{
    if (!trans && (acb -> acb_flags & ACB_PLEASE)) {
	acb -> acb_flags &= ~ACB_PLEASE;

	rti -> rti_type = RTI_TURN;
	{
	    register struct RtSAPturn  *rtu = &rti -> rti_turn;

	    rtu -> rtu_please = 1;
	    rtu -> rtu_priority = acb -> acb_priority;
	}

	return DONE;
    }

    return (*acb -> acb_rtwaitrequest) (acb, secs, trans, rti);
}
