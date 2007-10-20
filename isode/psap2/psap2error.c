/* psap2error.c - return PSAP error code in string form */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/psap2/RCS/psap2error.c,v 9.0 1992/06/16 12:29:42 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/psap2/RCS/psap2error.c,v 9.0 1992/06/16 12:29:42 isode Rel $
 *
 *
 * $Log: psap2error.c,v $
 * Revision 9.0  1992/06/16  12:29:42  isode
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
#include "psap2.h"

/*  */

static char *reject_err0[] = {
    "Rejected by peer",
    "Reason not specified",
    "Temporary congestion",
    "Local limit exceeded",
    "Called presentation address unknown",
    "Protocol version not supported",
    "Default context not supported",
    "User-data not readable",
    "No PSAP available",
    "Unrecognized PPDU",
    "Unexpected PPDU",
    "Unexpected session service primitive",
    "Unrecognized PPDU parameter",
    "Unexpected PPDU parameter",
    "Invalid PPDU parameter value",
    "Abstract syntax not supported",
    "Proposed transfer syntaxes not supported",
    "Local limit on DCS exceeded",
    "Connect request refused on this network connection",
    "Session disconnect",
    "Protocol error",
    "Peer aborted connection",
    "Invalid parameter",
    "Invalid operation",
    "Timer expired",
    "Indications waiting"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];

/*  */

char   *PErrString (code)
register int code;
{
    static char buffer[50];

    if (code < reject_err0_cnt)
	return reject_err0[code];

    (void) sprintf (buffer, "unknown error code %d", code);
    return buffer;
}
