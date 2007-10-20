/* oper_preject.c - deal with preject of an operation */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/oper_preject.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/oper_preject.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: oper_preject.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include "rosap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern	LLog	* log_dsap;

oper_preject(conn, dp)
struct connection	* conn;
struct DSAPpreject	* dp;
{
    struct oper_act *       on;

    DLOG(log_dsap, LLOG_TRACE, ("oper_preject"));

    if (dp->dp_id == -1)
    {
	/* No identified operation to reject! */
	return;
    }

    for(on=conn->cn_operlist; on!=NULLOPER; on=on->on_next_conn)
	if(on->on_id == dp->dp_id)
	    break;

    if(on == NULLOPER)
    {
	if (dp->dp_reason != DA_ROS)	/* Ros will have logged it */
	LLOG(log_dsap,LLOG_EXCEPTIONS,( 
	      "Unlocatable P-REJECT.INDICATION : %d (cn=%d, id=%d)",
	      dp->dp_reason, conn->cn_ad, dp->dp_id));
    }
    else
    {
	oper_fail_wakeup(on);
    }
}

