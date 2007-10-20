/* rylose.c - ROSY: clean-up after association termination */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosy/RCS/rylose.c,v 9.0 1992/06/16 12:37:29 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosy/RCS/rylose.c,v 9.0 1992/06/16 12:37:29 isode Rel $
 *
 *
 * $Log: rylose.c,v $
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
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", #p); \
}
#else
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", "p"); \
}
#endif

/*    clean-up after association termination */

int	RyLose (sd, roi)
int	sd;
struct RoSAPindication *roi;
{
    missingP (roi);

    loseopblk (sd, ROS_DONE);
    losedsblk (sd);

    return OK;
}
