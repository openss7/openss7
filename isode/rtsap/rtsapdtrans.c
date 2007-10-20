/* rtsapdtrans.c - RTPM: set downtrans upcall */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rtsap/RCS/rtsapdtrans.c,v 9.0 1992/06/16 12:37:45 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rtsap/RCS/rtsapdtrans.c,v 9.0 1992/06/16 12:37:45 isode Rel $
 *
 *
 * $Log: rtsapdtrans.c,v $
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

/*    set downtrans upcall */

int	RtSetDownTrans (sd, fnx, rti)
int	sd;
IFP	fnx;
struct RtSAPindication *rti;
{
    SBV	    smask;
    register struct assocblk   *acb;

    missingP (rti);

    smask = sigioblock ();

    rtsapPsig (acb, sd);

    acb -> acb_downtrans = fnx;

    (void) sigiomask (smask);

    return OK;
}
