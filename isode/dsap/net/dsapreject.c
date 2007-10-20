/* dsapreject.c - DSAP: Reject a remote operations event */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/net/RCS/dsapreject.c,v 9.0 1992/06/16 12:14:05 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/net/RCS/dsapreject.c,v 9.0 1992/06/16 12:14:05 isode Rel $
 *
 *
 * $Log: dsapreject.c,v $
 * Revision 9.0  1992/06/16  12:14:05  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include "logger.h"
#include "quipu/util.h"
#include "quipu/dsap.h"


extern LLog	* log_dsap;

/* ARGSUSED */

int	  DRejectRequest (sd, reason, id)
int	  sd;
int	  reason;
int	  id;
{
	int	  result;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);

	DLOG (log_dsap,LLOG_TRACE,( "DRejectRequest()"));

	watch_dog("RoURejectRequest");
	result = RoURejectRequest(sd, &id, reason, ROS_NOPRIO, roi);
	watch_dog_reset();

	if (result == NOTOK)
	{
		LLOG (log_dsap, LLOG_EXCEPTIONS,( "DBindReject: RoURejectRequest failed"));
        }

}

