/* susaperror.c - return UNITDATA SSAP error code in string form */

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
#include "ssap.h"
#include "uderrors.h"


/*  
 *  Cause the message array to get allocated
 */

#define	UDERR_ALLOCATE  1




/*  */

static char *reject_uerr0[] = {
    "Reason not specified",
    "Temporary congestion",
    "Rejected"
};

static int reject_uerr0_cnt = sizeof reject_uerr0 / sizeof reject_uerr0[0];


static char *reject_uerr8[] = {
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

static int reject_uerr8_cnt = sizeof reject_uerr8 / sizeof reject_uerr8[0];


/*  */

char   *SuErrString(code)
register int	code;
{
    register int    fcode;
    static char buffer[BUFSIZ];

    code &= 0xff;
    if ((code & SC_BASE)) {
	if ((fcode = code & ~SC_BASE) < reject_uerr8_cnt)
	    return reject_uerr8[fcode];
    }
    else
    	if ((code & UD_BASE)) {
    	    if ( (fcode = code & ~UD_BASE) > UDERR_MAX )
		return uderror_text[UDERR_UNKNOWN];
	    return uderror_text[fcode];
        }
    	else
	    if (code < reject_uerr0_cnt)
	        return reject_uerr0[code];

    (void) sprintf (buffer, "unknown error code 0x%x", code);
    return buffer;
}
