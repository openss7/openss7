/* oper_result.c - deal with result of an operation */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/oper_result.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/oper_result.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: oper_result.c,v $
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

#include "quipu/dsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern	LLog	* log_dsap;
#ifndef NO_STATS
extern  LLog    * log_stat;
#endif
extern time_t	  timenow;
extern Entry cache_dsp_entry ();

oper_result(cn, di)
struct connection	* cn;
struct DSAPindication	* di;
{
    struct DSAPresult	* dr = &(di->di_result);
    struct oper_act *   on;

    DLOG(log_dsap, LLOG_TRACE, ("oper_result()"));

    for(on=cn->cn_operlist; on != NULLOPER; on=on->on_next_conn)
    {
	if(on->on_id == dr->dr_id)
	    break;
    }

    if(on == NULLOPER)
    {
	LLOG(log_dsap, LLOG_FATAL, ("Cannot find operation to match result"));
	ds_res_free (&dr->dr_res.dcr_dsres);
	send_ro_ureject(cn->cn_ad, &(dr->dr_id), ROS_RRP_UNRECOG);
	return;
    }

#ifndef NO_STATS
	LLOG(log_stat, LLOG_DEBUG, ("Result received (%d) [%d]",
				    on->on_conn->cn_ad,
				    on->on_id));
#endif

    if(on->on_state == ON_ABANDONED)
    {
	LLOG(log_dsap, LLOG_TRACE, ("oper_result - operation had been abandoned"));

	/* If we have the arguments we could do more caching here. */
	if (dr->dr_res.dcr_dsres.result_type == OP_READ)
	       (void) cache_dsp_entry (&dr->dr_res.dcr_dsres.res_rd.rdr_entry);

	ds_res_free (&dr->dr_res.dcr_dsres);
	oper_extract(on);
	return;
    }

    if (dr->dr_res.dcr_dsres.result_type != on->on_arg->dca_dsarg.arg_type)
    {
	LLOG(log_dsap, LLOG_TRACE, ("oper_result - operation had been abandoned (2)"));
	send_ro_ureject(on->on_conn->cn_ad, &(dr->dr_id), ROS_RRP_MISTYPED);
	ds_res_free (&dr->dr_res.dcr_dsres);
	oper_extract(on);
	return;
    }

    /* free previous error - if any */
    ds_error_free (&on->on_resp.di_error.de_err);

    on->on_resp = (*di);	/* struct copy */

    cn->cn_last_used = timenow;

    switch(on->on_type)
    {
    case ON_TYPE_X500:
	task_result_wakeup (on);
	break;
    case ON_TYPE_SUBTASK:
	subtask_result_wakeup (on);
	break;
    case ON_TYPE_BIND_COMPARE:
	bind_compare_result_wakeup(on);
	ds_res_free (&dr->dr_res.dcr_dsres);
	break;
    case ON_TYPE_GET_DSA_INFO:
	dsa_info_result_wakeup(on);
	ds_res_free (&dr->dr_res.dcr_dsres);
	break;
    case ON_TYPE_GET_EDB:
    case ON_TYPE_SHADOW:
	on->on_state = ON_COMPLETE;
	break;
    default:
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_result: operation of unknown type"));
	oper_extract(on);
	break;
    }
}

