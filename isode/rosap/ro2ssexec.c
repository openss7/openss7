/* ro2ssexec.c - RTPM: exec */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosap/RCS/ro2ssexec.c,v 9.0 1992/06/16 12:37:02 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosap/RCS/ro2ssexec.c,v 9.0 1992/06/16 12:37:02 isode Rel $
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * $Log: ro2ssexec.c,v $
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
#include "ROS-types.h"
#include "../acsap/OACS-types.h"
#include "ropkt.h"
#include "isoservent.h"
#include "tailor.h"
#include "internet.h"

/*    SERVER only */

int	RoExec (ss, roi, arg1, arg2, hook, setperms)
struct SSAPstart *ss;
char   *arg1,
       *arg2;
struct RoSAPindication *roi;
IFP	hook,
	setperms;
{
    int     result,
            result2;
    register struct isoservent *is;
    register PE	    pe;
    struct SSAPref ref;
    struct SSAPindication   sis;
    register struct SSAPindication *si = &sis;
    struct type_OACS_PConnect	*pcon;

    missingP (ss);
    missingP (roi);
    missingP (arg1);
    missingP (arg2);

    if ((pe = ssdu2pe (ss -> ss_data, ss -> ss_cc, NULLCP, &result)) == NULLPE
	    || parse_OACS_PConnect (pe, 1, NULLIP, NULLVP, &pcon) == NOTOK) {
	if (pe)
	    pe_free (pe);
	if (result == PS_ERR_NMEM) {
    congest: ;
	    result = SC_CONGESTION, result2 = ROS_CONGEST;
	}
	else
	    result = SC_REJECTED, result2 = ROS_PROTOCOL;
	goto out;
    }

    PLOGP (rosap_log,OACS_PConnect, pe, "PConnect", 1);

    pe_free (pe);

    if (pcon -> pUserData -> member_OACS_2 -> offset
	    != type_OACS_ConnectionData_open) {
	result = SC_REJECTED, result2 = ROS_ADDRESS;
	goto out;
    }	
    if (is = getisoserventbyport ("rosap", 
	    (u_short) htons ((u_short) pcon -> pUserData -> applicationProtocol))) {
	*is -> is_tail++ = arg1;
	*is -> is_tail++ = arg2;
	*is -> is_tail = NULL;
    }
    else {
	result = SC_REJECTED, result2 = ROS_ADDRESS;
	goto out;
    }

    switch (hook ? (*hook) (is, roi) : OK) {
	case NOTOK: 
	    return NOTOK;

	case DONE: 
	    return OK;

	case OK: 
	    if (setperms)
		(void) (*setperms) (is);
	    (void) execv (*is -> is_vec, is -> is_vec);/* fall */
	    SLOG (rosap_log, LLOG_FATAL, *is -> is_vec, ("unable to exec"));
	default: 
	    goto congest;
    }

out: ;
    SSFREE (ss);

    bzero ((char *) &ref, sizeof ref);
    (void) SConnResponse (ss -> ss_sd, &ref, NULLSA,
		result, 0, 0, SERIAL_NONE, NULLCP, 0, si);
    return rosaplose (roi, result2, NULLCP, NULLCP);
}
