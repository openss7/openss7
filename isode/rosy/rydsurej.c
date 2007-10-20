/* rydsureject.c - ROSY: reject invocation */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosy/RCS/rydsurej.c,v 9.0 1992/06/16 12:37:29 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosy/RCS/rydsurej.c,v 9.0 1992/06/16 12:37:29 isode Rel $
 *
 *
 * $Log: rydsurej.c,v $
 * Revision 9.0  1992/06/16  12:37:29  isode
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
#include "rosy.h"


#ifdef __STDC__
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", #p); \
}
#else
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}
#endif

/*    U-REJECT */

int	RyDsUReject (sd, id, reason, priority, roi)
int	sd;
int	id,
	reason,
	priority;
struct RoSAPindication *roi;
{
    int     result;
    register struct opsblk *opb;

    missingP (roi);

    if ((opb = findopblk (sd, id, OPB_RESPONDER)) == NULLOPB)
	return rosaplose (roi, ROS_PARAMETER, NULLCP,
		"invocation %d not in progress on association %d",
		id, sd);

    if ((result = RoURejectRequest (sd, &id, reason, priority, roi)) != NOTOK)
	freeopblk (opb);

    return result;
}
