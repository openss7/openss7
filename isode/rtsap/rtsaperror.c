/* rtsaperror.c - return RtSAP error code in string form */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rtsap/RCS/rtsaperror.c,v 9.0 1992/06/16 12:37:45 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rtsap/RCS/rtsaperror.c,v 9.0 1992/06/16 12:37:45 isode Rel $
 *
 *
 * $Log: rtsaperror.c,v $
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
#include "rtsap.h"

/*  */


static char *reject_err0[] = {
    "Busy",
    "Cannot recover",
    "Validation failure",
    "Unacceptable dialogue mode",
    "Rejected by responder",
    "Address unknown",
    "Connect request refused on this network connection", 
    "Session disconnect",
    "Protocol error",
    "Congestion at RtSAP",
    "Remote system problem",
    "Presentation disconnect",
    "ACS disconnect",
    "Peer aborted association",
    "Invalid parameter",
    "Invalid operation",
    "Timer expired",
    "Indications waiting",
    "Transfer failure"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];

/*  */

char   *RtErrString (code)
register int code;
{
    static char buffer[50];

    if (code < reject_err0_cnt)
	return reject_err0[code];

    (void) sprintf (buffer, "unknown error code 0x%x", code);
    return buffer;
}
