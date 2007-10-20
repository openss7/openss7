/* oper_invoke.c - encode argument and invoke operation */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/oper_invoke.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/oper_invoke.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: oper_invoke.c,v $
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
#ifndef NO_STATS
extern  LLog    * log_stat;
#endif
void ros_log ();
extern char quipu_shutdown;

oper_send_invoke(oper)
register        struct oper_act * oper;
{
    int				  result;
    struct DSAPindication	  di_s;
    struct DSAPindication	* di = &(di_s);

    DLOG(log_dsap, LLOG_TRACE, ("oper_send_invoke"));

    if (quipu_shutdown) {
	    struct connection	* cn;
	    oper->on_conn->cn_state = CN_FAILED;
	    cn = oper->on_conn;
	    oper_extract(oper);
	    conn_extract(cn);
	    return(NOTOK);
    }

    if(oper == NULLOPER)
    {
	LLOG(log_dsap, LLOG_FATAL, ("Task memerr 3"));
	return(NOTOK);
    }

    if(oper->on_state == ON_ABANDONED)
	return NOTOK;

    /*
    * Genrate an id unique over this connection for this operation.
    */
    oper->on_id = ++(oper->on_conn->cn_op_id);

    switch (oper->on_conn->cn_ctx)
    {
	case DS_CTX_X500_DAP:
	    LLOG (log_dsap, LLOG_EXCEPTIONS, ("oper_invoke(): DAP context unexpected"));
	    break;
	case DS_CTX_X500_DSP:
	    result = DspInvokeRequest (oper->on_conn->cn_ad, oper->on_id,
			oper->on_arg, di);
	    break;
	case DS_CTX_QUIPU_DSP:
	    result = QspInvokeRequest (oper->on_conn->cn_ad, oper->on_id,
			oper->on_arg, di);
	    break;
	case DS_CTX_INTERNET_DSP:
	    if (oper->on_type == ON_TYPE_GET_EDB)
		    set_edb_limit (oper);

	    result = IspInvokeRequest (oper->on_conn->cn_ad, oper->on_id,
			oper->on_arg, di);
	    break;
	default:
	    LLOG (log_dsap, LLOG_EXCEPTIONS, ("oper_invoke(): Unknown context %d", oper->on_conn->cn_ctx));
	    break;
    }

    if (result != OK)
    {
	if(di->di_type == DI_ABORT)
	{
	    struct connection	* cn;

	    LLOG(log_dsap, LLOG_FATAL, ("D-INVOKE.REQUEST: fatal reject - fail the connection"));
	    oper->on_conn->cn_state = CN_FAILED;
	    cn = oper->on_conn;
	    oper_extract(oper);
	    conn_extract(cn);
	    return(NOTOK);
	}
	else
	{
	    oper->on_state = ON_COMPLETE;
	    oper->on_resp.di_type = DI_PREJECT;
	    oper_fail_wakeup(oper);
	    return(NOTOK);
	}
    }
    else
    {
	DLOG(log_dsap, LLOG_TRACE, ("D-INVOKE.REQUEST: OK"));
#ifndef NO_STATS
	{ 
	char * op;
	if (oper->on_task)
	   switch (oper->on_task->tk_dx.dx_arg.dca_dsarg.arg_type) {
		case OP_READ:		op = "Read";		break;
		case OP_COMPARE:	op = "Compare";		break;
		case OP_ABANDON:	op = "Abandon";		break;
		case OP_LIST:		op = "List";		break;
		case OP_SEARCH:		op = "Search";		break;
		case OP_ADDENTRY:	op = "Add";		break;
		case OP_REMOVEENTRY:	op = "Remove";		break;
		case OP_MODIFYENTRY:	op = "Modify";		break;
		case OP_MODIFYRDN:	op = "Modifyrdn";	break;
		case OP_GETEDB:		op = "Getedb";		break;
		default:		op = "Unknown op"; 	break;
	} else
	   switch (oper->on_type) {
	       case ON_TYPE_GET_EDB:	op = "Getedb";		break;
	       case ON_TYPE_X500:	op = "X500 op";		break;
	       case ON_TYPE_BIND_COMPARE:op = "Compare - bind";	break;
	       case ON_TYPE_GET_DSA_INFO:op = "Read - getdsa";	break;
	       case ON_TYPE_SUBTASK:	op = "Search - subtask";break;
	       case ON_TYPE_SHADOW:	op = "Read - shadow";	break;
	   }
	
	if (log_stat -> ll_events & LLOG_DEBUG)
		LLOG(log_stat, LLOG_DEBUG, ("Chain (%d): %s [%d]",
				    oper->on_conn->cn_ad,
				    op, oper->on_id));
	else 
		LLOG(log_stat, LLOG_TRACE, ("Chain (%d): %s",
				    oper->on_conn->cn_ad, op));
        }
#endif
	oper->on_state = ON_CHAINED;
	return(OK);
    }
}

