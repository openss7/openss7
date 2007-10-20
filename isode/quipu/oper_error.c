/* oper_error.c - deal with return of error to an operation */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/oper_error.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/oper_error.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: oper_error.c,v $
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
#include "quipu/ds_error.h"

extern	LLog	* log_dsap;
#ifndef NO_STATS
extern  LLog    * log_stat;
#endif
extern time_t	  timenow;

oper_error(conn, di)
struct connection	* conn;
struct DSAPindication	* di;
{
    struct DSAPerror	* de = &(di->di_error);
    struct oper_act *   oper;

    DLOG(log_dsap, LLOG_TRACE, ("net_wait_ro_error"));

    for(oper=conn->cn_operlist; oper != NULLOPER; oper=oper->on_next_conn)
	if(oper->on_id == de->de_id)
	    break;

    if(oper == NULLOPER)
    {
	LLOG(log_dsap, LLOG_FATAL, ("oper_error: Cannot locate operation for error"));
        ds_error_free (&di->di_error.de_err);
	send_ro_ureject(conn->cn_ad, &(de->de_id), ROS_REP_UNRECOG);
	return;
    }

#ifndef NO_STATS
	LLOG(log_stat, LLOG_DEBUG, ("Error received (%d) [%d]",
				    oper->on_conn->cn_ad,
				    oper->on_id));
#endif

    if(oper->on_state == ON_ABANDONED)
    {
	LLOG(log_dsap, LLOG_TRACE, ("oper_error: operation had been abandoned"));
        ds_error_free (&di->di_error.de_err);
	oper_extract(oper);
	return;
    }

    if(!ds_recog_err(de->de_err.dse_type))
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_error - Unrecognised error"));
        ds_error_free (&di->di_error.de_err);
	send_ro_ureject(conn->cn_ad, &(de->de_id), ROS_REP_RECERR);
	oper_fail_wakeup(oper);
    }

    /* free previous error - if any */
    ds_error_free (&oper->on_resp.di_error.de_err);

    oper->on_resp = (*di); /* struct copy */

    conn->cn_last_used = timenow;

    /* Need to check type of operation here! */
    switch(oper->on_type)
    {
    case ON_TYPE_X500:
	task_error_wakeup(oper);
	break;
    case ON_TYPE_SUBTASK:
	subtask_error_wakeup(oper);
	break;
    case ON_TYPE_BIND_COMPARE:
	bind_compare_error_wakeup(oper);
	break;
    case ON_TYPE_GET_DSA_INFO:
	dsa_info_error_wakeup(oper);
	break;
    case ON_TYPE_GET_EDB:
	get_edb_fail_wakeup(oper);
	break;
    case ON_TYPE_SHADOW:
	(void) shadow_fail_wakeup(oper);
	break;
    default:
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_error - on_type invalid"));
	break;
    }

}

