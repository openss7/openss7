/* plog.c - lower-layer logging routines */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/net/RCS/plog.c,v 9.0 1992/06/16 12:14:05 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/net/RCS/plog.c,v 9.0 1992/06/16 12:14:05 isode Rel $
 *
 *
 * $Log: plog.c,v $
 * Revision 9.0  1992/06/16  12:14:05  isode
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

#include "quipu/util.h"
#include "quipu/dsap.h"
#include "tsap.h"

extern	LLog	* log_dsap;

SFP	abort_vector = NULL;


void    ros_log(rop, event)
register struct RoSAPpreject *rop;
char   *event;
{
int level = LLOG_EXCEPTIONS;

    if ((rop->rop_reason == ROS_TIMER) || (rop->rop_reason == ROS_ACS))
	level = LLOG_DEBUG;

    if(rop->rop_cc > 0)
	LLOG(log_dsap, level, ("%s: [%s] %*.*s", event,
	  RoErrString(rop->rop_reason), rop->rop_cc, rop->rop_cc,
	  rop->rop_data));
    else
	LLOG(log_dsap, level, ("%s: [%s]", event, RoErrString(rop->rop_reason)));

    if (rop -> rop_reason == ROS_CONGEST && abort_vector)
	(*abort_vector) (-2);
}

void    acs_log(aca, event)
register struct AcSAPabort *aca;
char   *event;
{
    if(aca->aca_cc > 0)
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s] %*.*s", event,
		AcErrString(aca->aca_reason),
		aca->aca_cc, aca->aca_cc, aca->aca_data));
    else
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s]", event, AcErrString(aca->aca_reason)));

    if (aca -> aca_reason == ACS_CONGEST && abort_vector)
	(*abort_vector) (-2);
}

td_log(td, event)
struct TSAPdisconnect   *td;
char *event;
{
    if(td->td_cc > 0)
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s] %*.*s", event,
	  TErrString(td->td_reason),
	  td->td_cc, td->td_cc, td->td_data));
    }
    else
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s]", event, TErrString(td->td_reason)));
    }
}

int     SetROPS(ad)
int     ad;
{
    struct RoSAPindication      roi_s;
    struct RoSAPindication      *roi = &(roi_s);
    struct RoSAPpreject         *rop = &(roi->roi_preject);

    if(RoSetService(ad, RoPService, roi) != OK)
    {
	(void) ros_log(rop, "RoSetService");
	return NOTOK;
    }
    return(OK);
}
