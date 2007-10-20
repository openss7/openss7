/* rydispatch.c - ROSY: dispatch  */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosy/RCS/rydispatch.c,v 9.0 1992/06/16 12:37:29 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosy/RCS/rydispatch.c,v 9.0 1992/06/16 12:37:29 isode Rel $
 *
 *
 * $Log: rydispatch.c,v $
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
/*    DISPATCH */

int	RyDispatch (sd, ryo, op, fnx, roi)
int	sd;
register struct RyOperation *ryo;
int	op;
IFP	fnx;
struct RoSAPindication *roi;
{
    register struct dspblk *dsb;

    missingP (roi);

    if ((dsb = finddsblk (sd, op)) == NULLDSB) {
	missingP (ryo);
	missingP (fnx);

	for (; ryo -> ryo_name; ryo++)
	    if (ryo -> ryo_op == op)
		break;
	if (!ryo -> ryo_name)
	    return rosaplose (roi, ROS_PARAMETER, NULLCP,
		    "unknown operation code %d", op);

	if ((dsb = newdsblk (sd, ryo)) == NULLDSB)
	    return rosaplose (roi, ROS_CONGEST, NULLCP, NULLCP);
    }
    else
	if (ryo)
	    dsb -> dsb_ryo = ryo;

    if ((dsb -> dsb_vector = fnx) == NULLIFP)
	freedsblk (dsb);

    return OK;
}
