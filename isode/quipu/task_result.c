/* task_result.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/task_result.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/task_result.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: task_result.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include "rosap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern  LLog    * log_dsap;
void ros_log ();

/*
* The DSA has produced a result for the task, encode the result,
* generate an RO-RESULT.REQUEST and update the task block.
*/
task_result(task)
register        struct task_act * task;
{
    int				  result;
    struct DSAPindication	  di_s;
    struct DSAPindication	* di = &(di_s);
    struct ds_op_res            * res;
    struct connection		* cn = task->tk_conn;

#ifndef NO_STATS
    extern LLog * log_stat;

    if (log_stat -> ll_events & LLOG_DEBUG) 
    	    LLOG (log_stat, LLOG_DEBUG,("Result sent (%d) [%d]",
					task->tk_conn->cn_ad,
					task->tk_dx.dx_id));
    else
    	    LLOG (log_stat, LLOG_TRACE,("Result sent (%d)",
					task->tk_conn->cn_ad));

#endif

    DLOG(log_dsap, LLOG_TRACE, ("task_result"));

    if(task == NULLTASK)
    {
	LLOG(log_dsap, LLOG_FATAL, ("Task memerr 5"));
	return;
    }

    res = task->tk_result;
    task->tk_resp.di_type = DI_RESULT;

    switch (cn->cn_ctx)
    {
	case DS_CTX_X500_DAP:
	    result = DapResultRequest (cn->cn_ad, task->tk_dx.dx_id,
					&(res->dcr_dsres), di);
	    break;
	case DS_CTX_X500_DSP:
	    result = DspResultRequest (cn->cn_ad, task->tk_dx.dx_id, res, di);
	    break;
	case DS_CTX_QUIPU_DSP:
	    result = QspResultRequest (cn->cn_ad, task->tk_dx.dx_id, res, di);
	    break;
	case DS_CTX_INTERNET_DSP:
	    result = IspResultRequest (cn->cn_ad, task->tk_dx.dx_id, res, di);
	    break;
	default:
	    LLOG (log_dsap, LLOG_EXCEPTIONS, ("task_result(): Unknown context %d", cn->cn_ctx));
	    break;
    }

    if (result != OK)
    {
	if(di->di_type == DI_ABORT)
	{
	    LLOG(log_dsap, LLOG_FATAL, ("D-RESULT.REQUEST: fatal reject - fail the connection"));
	    cn->cn_state = CN_FAILED;
	}
	else
	{
	    send_ro_ureject(cn->cn_ad, &(task->tk_dx.dx_id), ROS_IP_RELEASE);
	}
    }

    if(cn->cn_state == CN_FAILED)
    {
	DLOG(log_dsap, LLOG_DEBUG, ("task_result(): extracting conn:"));
	conn_log(cn,LLOG_DEBUG);
	conn_extract(cn);
    }
}

