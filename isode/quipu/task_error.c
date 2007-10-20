/* task_error.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/task_error.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/task_error.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: task_error.c,v $
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
* The DSA has produced an error for the task, encode the error,
* generate a D-ERROR.REQUEST and update the task block.
*/
task_error(task)
register        struct task_act * task;
{
    int				  result;
    struct DSAPindication	  di_s;
    struct DSAPindication	* di = &(di_s);
    struct DSError              * err;
    struct connection		* cn = task->tk_conn;

#ifndef NO_STATS
    extern LLog * log_stat;

    if (log_stat -> ll_events & LLOG_DEBUG) 
    	    LLOG (log_stat, LLOG_DEBUG,("Error sent (%d) [%d]",
					task->tk_conn->cn_ad,
					task->tk_dx.dx_id));
    else
    	    LLOG (log_stat, LLOG_TRACE,("Error sent (%d)",
					task->tk_conn->cn_ad));
#endif

    if(task == NULLTASK)
    {
	LLOG(log_dsap, LLOG_FATAL, ("Task memerr 2"));
	return;
    }

    err = task->tk_error;
    task->tk_resp.di_type = DI_ERROR;    

    if (log_dsap -> ll_events & LLOG_NOTICE)
	    log_ds_error (task->tk_error);

    /* Return the right sort of referral error */
    if(cn->cn_ctx == DS_CTX_X500_DAP)
    {
	if(err->dse_type == DSE_DSAREFERRAL)
	{
	    DLOG(log_dsap, LLOG_DEBUG, ("Changing DSAREFERRAL to REFERRAL (DAP)"));
	    err->dse_type = DSE_REFERRAL;
	}
    }
    else
    {
	if(err->dse_type == DSE_REFERRAL)
	{
	    DLOG(log_dsap, LLOG_DEBUG, ("Changing DSAREFERRAL to REFERRAL"));
	    err->dse_type = DSE_DSAREFERRAL;
	}
    }

    switch (cn->cn_ctx)
    {
	case DS_CTX_X500_DAP:
	    result = DapErrorRequest (cn->cn_ad, task->tk_dx.dx_id, err, di);
	    break;
	case DS_CTX_X500_DSP:
	    result = DspErrorRequest (cn->cn_ad, task->tk_dx.dx_id, err, di);
	    break;
	case DS_CTX_QUIPU_DSP:
	    result = QspErrorRequest (cn->cn_ad, task->tk_dx.dx_id, err, di);
	    break;
	case DS_CTX_INTERNET_DSP:
	    result = IspErrorRequest (cn->cn_ad, task->tk_dx.dx_id, err, di);
	    break;
	default:
	    LLOG (log_dsap, LLOG_EXCEPTIONS, ("task_error(): Unknown context %d", cn->cn_ctx));
	    break;
    }

    if (result != OK)
    {
	if(di->di_type == DI_ABORT)
	{
	    LLOG(log_dsap, LLOG_FATAL, ("D-RESULT.REQUEST: fatal reject - fail the connection"));
	    cn->cn_state = CN_FAILED;
	}
    }

    if(cn->cn_state == CN_FAILED)
    {
	DLOG(log_dsap, LLOG_DEBUG, ("task_error(): extracting conn:"));
	conn_log(cn,LLOG_DEBUG);
	conn_extract(cn);
    }
}
