/* acsaperror.c - return AcSAP error code in string form */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/acsaperror.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/acsaperror.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: acsaperror.c,v $
 * Revision 9.0  1992/06/16  12:05:59  isode
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
#include "acsap.h"

/*  */

static char *reject_err0[] = {
    "Accepted",
    "Permanent",
    "Transient",
    "Rejected by service-user: null",
    "Rejected by service-user: no reason given",
    "Application context name not supported",
    "Calling AP title not recognized",
    "Calling AP invocation-ID not recognized",
    "Calling AE qualifier not recognized",
    "Calling AE invocation-ID not recognized",
    "Called AP title not recognized",
    "Called AP invocation-ID not recognized",
    "Called AE qualifier not recognized",
    "Called AE invocation-ID not recognized",
    "Rejected by service-provider: null",
    "Rejected by service-provider: no reason given",
    "No common acse version",
    "Address unknown",
    "Connect request refused on this network connection",
    "Local limit exceeded",
    "Presentation disconnect",
    "Protocol error",
    "Peer aborted association",
    "Invalid parameter",
    "Invalid operation",
    "Timer expired"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];

/*  */

char   *AcErrString (code)
register int code;
{
    static char buffer[50];

    if (code < reject_err0_cnt)
	return reject_err0[code];

    (void) sprintf (buffer, "unknown error code %d", code);
    return buffer;
}
