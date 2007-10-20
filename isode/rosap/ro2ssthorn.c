/* ro2ssthorn.c - ROPM: interface for THORN */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosap/RCS/ro2ssthorn.c,v 9.0 1992/06/16 12:37:02 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/rosap/RCS/ro2ssthorn.c,v 9.0 1992/06/16 12:37:02 isode Rel $
 *
 *
 * $Log: ro2ssthorn.c,v $
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
#include <signal.h>
#include "ropkt.h"

/*  */

static PE	qb2Rpe (qb, len, result)
register struct qbuf *qb;
int	len;
int    *result;
{
    return qb2pe (qb, len, 2, result);
}

/*    modify underling service */

int	RoSetThorn (sd, roi)
int	sd;
struct RoSAPindication *roi;
{
    SBV	    smask;
    int	    result;
    register struct assocblk   *acb;

    missingP (roi);

    smask = sigioblock ();

    if ((acb = findacblk (sd)) == NULL) {
	(void) sigiomask (smask);
	return rosaplose (roi, ROS_PARAMETER, NULLCP,
			  "invalid association descriptor");
    }

    if (acb -> acb_flags & ACB_ROS) {
	acb -> acb_getosdu = qb2Rpe;
	result = OK;
    }
    else
	result = rosaplose (roi, ROS_OPERATION, NULLCP,
			    "not an association descriptor for ROS");


    (void) sigiomask (smask);

    return result;
}
