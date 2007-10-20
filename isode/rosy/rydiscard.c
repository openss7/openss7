/* rydiscard.c - ROSY: discard invocation in progress */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosy/RCS/rydiscard.c,v 9.0 1992/06/16 12:37:29 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosy/RCS/rydiscard.c,v 9.0 1992/06/16 12:37:29 isode Rel $
 *
 *
 * $Log: rydiscard.c,v $
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

/*  */

static int	do_response ();

/*    DISCARD */

int	RyDiscard (sd, id, roi)
int	sd,
	id;
struct RoSAPindication *roi;
{
    register struct opsblk *opb;

    missingP (roi);

    if ((opb = findopblk (sd, id, OPB_INITIATOR)) == NULLOPB)
	return rosaplose (roi, ROS_PARAMETER, NULLCP,
			  "invocation %d not in progress on association %d",
			  id, sd);

    opb -> opb_resfnx = opb -> opb_errfnx = do_response;

    return OK;
}

/*  */

/* ARGSUSED */

static int  do_response (sd, id, dummy, value, roi)
int	sd,
	id,
	dummy;
caddr_t value;
struct RoSAPindication *roi;
{
    return OK;
}
