/* ssaperror.c - return SSAP error code in string form */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ssap/RCS/ssaperror.c,v 9.0 1992/06/16 12:39:41 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/ssap/RCS/ssaperror.c,v 9.0 1992/06/16 12:39:41 isode Rel $
 *
 *
 * $Log: ssaperror.c,v $
 * Revision 9.0  1992/06/16  12:39:41  isode
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
#include "ssap.h"

/*  */

static char *reject_err0[] = {
    "Reason not specified",
    "Temporary congestion",
    "Rejected"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];


static char *reject_err8[] = {
    "unknown error code 0x80",
    "SSAP identifier unknown",
    "SS-user not attached to SSAP",
    "Congestion at SSAP",
    "Proposed protocol versions not supported",
    "Address unknown",
    "Connect request refused on this network connection", 
    "Transport disconnect",
    "Provider-initiated abort",
    "Protocol error",
    "Invalid parameter",
    "Invalid operation",
    "Timer expired",
    "Indications waiting"
};

static int reject_err8_cnt = sizeof reject_err8 / sizeof reject_err8[0];


/*  */

char   *SErrString(code)
register int	code;
{
    register int    fcode;
    static char buffer[50];

    code &= 0xff;
    if (code & SC_BASE) {
	if ((fcode = code & ~SC_BASE) < reject_err8_cnt)
	    return reject_err8[fcode];
    }
    else
	if (code < reject_err0_cnt)
	    return reject_err0[code];

    (void) sprintf (buffer, "unknown error code 0x%x", code);
    return buffer;
}
