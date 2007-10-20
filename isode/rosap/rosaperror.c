/* rosaperror.c - return RoSAP error code in string form */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosap/RCS/rosaperror.c,v 9.0 1992/06/16 12:37:02 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosap/RCS/rosaperror.c,v 9.0 1992/06/16 12:37:02 isode Rel $
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * $Log: rosaperror.c,v $
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
#include "rosap.h"

/*  */

static char *reject_err0[] = {
    "Authentication failure",
    "Busy",
    "Unrecognized APDU",
    "Mistyped APDU",
    "Badly structured APDU",
    "Duplicate invocation",
    "Unrecognized operation",
    "Mistyped argument",
    "Resource limitation",
    "Initiator releasing",
    "Unrecognized linked ID",
    "Linked response unexpected",
    "Unexpected child operation",
    "Unrecognized invocation",
    "Result response unexpected",
    "Mistyped result",
    "Unrecognized invocation",
    "Error response unexpected",
    "Unrecognized error",
    "Unexpected error",
    "Mistyped parameter",
    "Address unknown",
    "Connect request refused on this network connection",
    "Session disconnect",
    "Protocol error",
    "Congestion at RoSAP",
    "Remote system problem",
    "Association done via async handler",
    "Peer aborted association",
    "RTS disconnect",
    "Presentation disconnect",
    "ACS disconnect",
    "Invalid parameter",
    "Invalid operation",
    "Timer expired",
    "Indications waiting",
    "APDU not transferred",
    "Stub interrupted"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];

/*  */

char   *RoErrString (code)
register int code;
{
    static char buffer[50];

    if (code < reject_err0_cnt)
	return reject_err0[code];

    (void) sprintf (buffer, "unknown error code 0x%x", code);
    return buffer;
}
