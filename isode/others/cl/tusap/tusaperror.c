/* tsaperror.c - print out TSAP UNTITDATA  error codes */

/* kad */

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
#include "tsap.h"
#include "uderrors.h"



/*  */

static char *disc_uerr0[] = {
    "Reason not specified or Successful Return",
    "Congestion at TSAP",
    "Session entity not attached to TSAP",
    "Address unknown"
};

static int  disc_uerr0_cnt = sizeof disc_uerr0 / sizeof disc_uerr0[0];


static char *disc_uerr8[] = {
    "Normal disconnect initiated by session entity",
    "Remote transport entity congestion at connect time",
    "Connection negotiation failed (proposed class(es) not supported)",
    "Duplicate source reference detected for the same pair of NSAPs",
    "Mismatched references",
    "Protocol error",
    "Not used",
    "Reference overflow",
    "Connect request refused on this network connection",
    "Not used",
    "Header or parameter length invalid",
    "Network disconnect",
    "Invalid parameter",
    "Invalid operation",
    "Timer expired",
    "Indications waiting"
};

static int  disc_uerr8_cnt = sizeof disc_uerr8 / sizeof disc_uerr8[0];

/*  */

char *TuErrString(code)
register int	code;
{
    register int    fcode;
    static char buffer[BUFSIZ];

    code &= 0xff;
    if ((code & DR_BASE)) {
	if ((fcode = code & ~DR_BASE) < disc_uerr8_cnt)
	    return disc_uerr8[fcode];
    }
    else
    	if ((code & UD_BASE)) {
    	    if ( (fcode = code & ~UD_BASE) > UDERR_MAX )
		return uderror_text[UDERR_UNKNOWN];
	    return uderror_text[fcode];
        }
        else
	    if (code < disc_uerr0_cnt)
	        return disc_uerr0[code];

    (void) sprintf (buffer, "unknown error code 0x%x", code);
    return buffer;
}
