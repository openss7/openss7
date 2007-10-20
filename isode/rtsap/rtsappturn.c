/* rtsappturn.c - RTPM: turn please */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rtsap/RCS/rtsappturn.c,v 9.0 1992/06/16 12:37:45 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rtsap/RCS/rtsappturn.c,v 9.0 1992/06/16 12:37:45 isode Rel $
 *
 *
 * $Log: rtsappturn.c,v $
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

/*    RT-TURN-PLEASE.REQUEST */

int	RtPTurnRequest (sd, priority, rti)
int	sd;
int	priority;
struct RtSAPindication *rti;
{
    SBV	    smask;
    int     result;
    register struct assocblk   *acb;

    missingP (rti);

    smask = sigioblock ();

    rtsapPsig (acb, sd);

    result = (*acb -> acb_pturnrequest) (acb, priority, rti);

    (void) sigiomask (smask);

    return result;
}
