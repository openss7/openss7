/* dsapwait.c - DSAP: Deal with incoming activity */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/net/RCS/dsapwait.c,v 9.0 1992/06/16 12:14:05 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/net/RCS/dsapwait.c,v 9.0 1992/06/16 12:14:05 isode Rel $
 *
 *
 * $Log: dsapwait.c,v $
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

#include <signal.h>
#include "logger.h"
#include "quipu/util.h"
#include "quipu/dsap.h"
#include "../x500as/DAS-types.h"
#include "../x500as/Quipu-types.h"

extern LLog	* log_dsap;
extern unsigned watchdog_time;

extern char * RoErrString();

#ifdef PDU_DUMP
#define DUMP_ARG 	"arg"
#define DUMP_RES 	"res"
#define DUMP_ERR 	"err"
#endif

static slack_watch_dog ();
extern char dsa_mode;

/* ARGSUSED */

int	  DWaitRequest (ctx, sd, secs, di)
int			  ctx;
int                       sd;
int			  secs;
struct DSAPindication	* di;
{
    int	  result;

    switch (ctx)
    {
        case DS_CTX_X500_DAP:
	    result = DapRespWaitRequest (sd, secs, di);
	    break;

	case DS_CTX_X500_DSP:
	    result = DspWaitRequest (sd, secs, di);
	    break;

	case DS_CTX_QUIPU_DSP:
	    result = QspWaitRequest (sd, secs, di);
	    break;

	case DS_CTX_INTERNET_DSP:
	    result = IspWaitRequest (sd, secs, di);
	    break;

	default:
	    LLOG (log_dsap, LLOG_EXCEPTIONS, 
		  ("DWaitRequest: unknown context id %d", ctx));
	    return (dsaplose (di, DA_APP_CONTEXT, "WAIT REQUEST"));
    }

    return (result);
}

static dsap_wait_err (str,roi,sd) 
char * str;
struct RoSAPindication	* roi;
int sd;
{
int op;
char * err;
int cc = 0;
char * data = NULLCP;
char * xtype;

	switch (roi->roi_type) {
	case ROI_RESULT:
		op   = roi->roi_result.ror_id;
		err  = "received";
		xtype = "result";
		break;
	case ROI_ERROR:
		op   = roi->roi_error.roe_id;
		err  = "received";
		xtype = "error";
		break;
	case ROI_UREJECT:			
		op   = roi->roi_ureject.rou_id;
		err  = RoErrString(roi->roi_ureject.rou_reason);
		xtype = "user reject";
		break;
	case ROI_PREJECT:
		op   = roi->roi_preject.rop_id;
		err  = RoErrString(roi->roi_preject.rop_reason);
		cc   = roi->roi_preject.rop_cc;
		data = roi->roi_preject.rop_data;
		xtype = "provider reject";
		break;
	}

	if (cc)
		LLOG (log_dsap, LLOG_EXCEPTIONS, 
		      ("%s: cn=%d, op=%d %s (%s) %s",
		       str, sd, op, xtype, err, data));
	else
		LLOG (log_dsap, LLOG_EXCEPTIONS, 
		      ("%s: cn=%d, op=%d %s (%s)",
		       str, sd, op, xtype, err));

	switch (roi->roi_type) {
	case ROI_RESULT:
		RORFREE (&roi->roi_result);	break;
	case ROI_ERROR:
		ROEFREE (&roi->roi_error);	break;
	case ROI_UREJECT:			
		/* no op */			break;
	case ROI_PREJECT:
		ROPFREE (&(roi->roi_preject));	break;
	}
	
}

int	  DapRespWaitRequest (sd, secs, di)
int			  sd;
int			  secs;
struct DSAPindication	* di;
{
	int	  result;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);

	DLOG (log_dsap,LLOG_TRACE,( "DapRespWaitRequest()"));

	watch_dog("RoWaitRequest (DAP)");
	result = RoWaitRequest(sd, secs, roi);
	watch_dog_reset();

	if (result == NOTOK)
	{
		if (roi->roi_preject.rop_reason == ROS_TIMER)
		{
			return (DONE);
		}

		if (ROS_FATAL (roi->roi_preject.rop_reason))
		{
			return (ros2dsaplose (di, "DapRespWaitRequest", 
					      &(roi->roi_preject)));
		}
		else
		{
			return (dsapreject (di, DP_ROS, -1, NULLCP, 
			       "DapRespWaitRequest: Non-fatal reject"));
		}
        }

        switch(roi->roi_type)
        {
	case ROI_INVOKE:
		return (DapDecodeInvoke (sd, &(roi->roi_invoke), di));

	case ROI_RESULT:
		dsap_wait_err ("DapRespWaitRequest",roi,sd);
		DRejectRequest (sd, ROS_RRP_UNRECOG, roi->roi_result.ror_id);
		return (dsaplose (di, DI_RESULT, NULLCP, 
				  "DAP responder cannot accept results"));

	case ROI_ERROR:
		dsap_wait_err ("DapRespWaitRequest",roi,sd);
		DRejectRequest (sd, ROS_REP_UNRECOG, roi->roi_error.roe_id);
		return (dsaplose (di, DI_RESULT, NULLCP, 
				  "DAP responder cannot accept errors"));

	case ROI_UREJECT:
		dsap_wait_err ("DapRespWaitRequest",roi,sd);
		return (ros2dsapreject(di, "ROI_UREJECT", &(roi->roi_ureject)));

	case ROI_PREJECT:
		dsap_wait_err ("DapRespWaitRequest",roi,sd);
		result = ros2dsaplose (di, "ROI_PREJECT", &(roi->roi_preject));
		return result;

	case ROI_FINISH:
	    /*
	    * Should be getting an RoBIND structure here.
	    * Currently this is simulated with RoUnBindInit, which
	    * will check that the user data in the release was used
	    * correctly even though no UnbindArgument is present.
	    * This is mapped up into D-UNBIND indication.
	    */
	    return (DDecodeUnbind (sd, &(roi->roi_finish), di));

	default:
	    LLOG (log_dsap,LLOG_EXCEPTIONS,( "Unknown indication type: %d", 
					    roi->roi_type));
	    return (dsaplose (di, DA_NO_REASON, NULLCP, NULLCP));
        }
}

int	  DspWaitRequest (sd, secs, di)
int			  sd;
int			  secs;
struct DSAPindication	* di;
{
	int	  result;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);

	DLOG (log_dsap,LLOG_TRACE,( "DspWaitRequest()"));

	slack_watch_dog("RoWaitRequest (DSP)");
	result = RoWaitRequest(sd, secs, roi);
	watch_dog_reset();

	if (result == NOTOK)
	{
		if (roi->roi_preject.rop_reason == ROS_TIMER)
		{
			return (DONE);
		}

		if (ROS_FATAL (roi->roi_preject.rop_reason))
		{
			return (ros2dsaplose (di, "DspRespWaitRequest", &(roi->roi_preject)));
		}
		else
		{
			return (dsapreject (di, DP_ROS, -1, NULLCP, "DspRespWaitRequest: Non-fatal reject"));
		}
        }

        switch(roi->roi_type)
        {
	case ROI_INVOKE:
		return (DspDecodeInvoke (sd, &(roi->roi_invoke), di));

	case ROI_RESULT:
		return (DspDecodeResult (sd, &(roi->roi_result), di));

	case ROI_ERROR:
		return (DDecodeError (sd, &(roi->roi_error), di));

	case ROI_UREJECT:
		dsap_wait_err ("DspWaitRequest",roi,sd);
		return (ros2dsapreject(di, "ROI_UREJECT", &(roi->roi_ureject)));

	case ROI_PREJECT:
		dsap_wait_err ("DspWaitRequest",roi,sd);
		result = ros2dsaplose (di, "ROI_PREJECT", &(roi->roi_preject));
		return result;

	case ROI_FINISH:
	    /*
	    * Should be getting an RoBIND structure here.
	    * Currently this is simulated with RoUnBindInit, which
	    * will check that the user data in the release was used
	    * correctly even though no UnbindArgument is present.
	    * This is mapped up into D-UNBIND indication.
	    */
	    return (DDecodeUnbind (sd, &(roi->roi_finish), di));

	default:
	    LLOG (log_dsap,LLOG_EXCEPTIONS,
		  ( "Unknown indication type : %d", roi->roi_type));
	    return (dsaplose (di, DA_NO_REASON, NULLCP, NULLCP));
        }
}

int	  QspWaitRequest (sd, secs, di)
int			  sd;
int			  secs;
struct DSAPindication	* di;
{
	int	  result;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);

	DLOG (log_dsap,LLOG_TRACE,( "QspWaitRequest()"));

	slack_watch_dog("RoWaitRequest (QSP)");
	result = RoWaitRequest(sd, secs, roi);
	watch_dog_reset();

	if (result == NOTOK)
	{
		if (roi->roi_preject.rop_reason == ROS_TIMER)
		{
			return (DONE);
		}

		if (ROS_FATAL (roi->roi_preject.rop_reason))
		{
			return (ros2dsaplose (di, "QspRespWaitRequest", 
					      &(roi->roi_preject)));
		}
		else
		{
			return (dsapreject (di, DP_ROS, -1, NULLCP, 
				   "QspRespWaitRequest: Non-fatal reject"));
		}
        }

        switch(roi->roi_type)
        {
	case ROI_INVOKE:
		return (QspDecodeInvoke (sd, &(roi->roi_invoke), di));

	case ROI_RESULT:
		return (QspDecodeResult (sd, &(roi->roi_result), di));

	case ROI_ERROR:
		return (DDecodeError (sd, &(roi->roi_error), di));

	case ROI_UREJECT:
		dsap_wait_err ("QspWaitRequest",roi,sd);
		return (ros2dsapreject(di, "ROI_UREJECT", &(roi->roi_ureject)));

	case ROI_PREJECT:
		dsap_wait_err ("QspWaitRequest",roi,sd);
		result = ros2dsaplose (di, "ROI_PREJECT", &(roi->roi_preject));
		return result;

	case ROI_FINISH:
	    /*
	    * Should be getting an RoBIND structure here.
	    * Currently this is simulated with RoUnBindInit, which
	    * will check that the user data in the release was used
	    * correctly even though no UnbindArgument is present.
	    * This is mapped up into D-UNBIND indication.
	    */
	    return (DDecodeUnbind (sd, &(roi->roi_finish), di));

	default:
	    LLOG (log_dsap,LLOG_EXCEPTIONS,( "Unknown indication type : %d", 
					    roi->roi_type));
	    return (dsaplose (di, DA_NO_REASON, NULLCP, NULLCP));
        }
}

int	  DapDecodeInvoke (sd, rox, di)
int			  sd;
struct RoSAPinvoke	* rox;
struct DSAPindication	* di;
{
    int			  success;
    PE			  pe = rox->rox_args;
    struct ds_op_arg	* dsarg = &(di->di_invoke.dx_arg);
    struct chain_arg	* charg = &(dsarg->dca_charg);
    struct DSArgument	* arg = &(dsarg->dca_dsarg);

    if (rox -> rox_nolinked == 0) {
	LLOG (log_dsap, LLOG_EXCEPTIONS, 
	      ("DapDecodeInvoke: Linked operation (%d) %d",sd,
	       rox -> rox_linkid));
	DRejectRequest (sd, ROS_IP_LINKED, rox->rox_id);
	ROXFREE (rox);
	return (dsapreject (di, DP_INVOKE, -1, NULLCP, "Link op"));
    }

    di->di_type = DI_INVOKE;
    di->di_invoke.dx_id = rox->rox_id;

    switch(arg->arg_type = rox->rox_op)
    {
    case    OP_READ :
    {
        struct ds_read_arg * dr;
	success = decode_DAS_ReadArgument(pe,1,NULLIP,NULLVP,&dr);
	arg->arg_rd = *dr; /* struct copy */
	free ((char *) dr);
    }
	break;
    case    OP_COMPARE :
    {
	struct ds_compare_arg * dr;
	success = decode_DAS_CompareArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_cm = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;
    case    OP_ABANDON :
    {
	struct ds_abandon_arg * dr;
	success = decode_DAS_AbandonArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_ab = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;
    case    OP_LIST :
    {
	struct ds_list_arg * dr;
	success = decode_DAS_ListArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_ls = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;
    case    OP_SEARCH :
    {
	struct ds_search_arg * dr;
	success = decode_DAS_SearchArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_sr = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;
    case    OP_ADDENTRY :
    {
	struct ds_addentry_arg * dr;
	success = decode_DAS_AddEntryArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_ad = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;
    case    OP_REMOVEENTRY :
    {
	struct ds_removeentry_arg * dr;
	success = decode_DAS_RemoveEntryArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_rm = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;
    case    OP_MODIFYENTRY :
    {
	struct ds_modifyentry_arg * dr;
	success = decode_DAS_ModifyEntryArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_me = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;
    case    OP_MODIFYRDN :
    {
	struct ds_modifyrdn_arg * dr;
	success = decode_DAS_ModifyRDNArgument(pe,1,NULLIP,NULLVP,&dr);
        arg->arg_mr = *dr; /* struct copy */
        free ((char *) dr);
    }
	break;

    default:
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("DapDecodeInvoke(%d): op id %d unknown!", sd, rox->rox_op));
	DRejectRequest (sd, ROS_IP_UNRECOG, rox->rox_id);
	ROXFREE (rox);
	return (dsaplose (di, DP_INVOKE, NULLCP, "Unknown operation identifier"));
    }

    if (success == NOTOK)
    {
	LLOG (log_dsap, LLOG_EXCEPTIONS, 
	      ("DapDecodeInvoke(%d): Unable to parse argument",sd));
	DRejectRequest (sd, ROS_IP_MISTYPED, rox->rox_id);
	ROXFREE (rox);
	return (dsapreject (di, DP_INVOKE, -1, NULLCP, "Undecodable argument"));
    }

    charg->cha_originator = NULLDN;
    charg->cha_target = NULLDN;
    charg->cha_domaininfo = NULLPE;
    charg->cha_trace = NULLTRACEINFO;
    charg->cha_timelimit = NULLCP;

    ROXFREE (rox);
    return(success);
}

int	  DspDecodeInvoke (sd, rox, di)
int			  sd;
struct RoSAPinvoke	* rox;
struct DSAPindication	* di;
{
    int			  success;
    PE			  pe = rox->rox_args;

    di->di_type = DI_INVOKE;
    di->di_invoke.dx_id = rox->rox_id;

    if (rox -> rox_nolinked == 0) {
	LLOG (log_dsap, LLOG_EXCEPTIONS, 
	      ("DspDecodeInvoke: Linked operation (%d) %d",
	       sd,rox -> rox_linkid));
	DRejectRequest (sd, ROS_IP_LINKED, rox->rox_id);
	ROXFREE (rox);
	return (dsapreject (di, DP_INVOKE, -1, NULLCP, "Link op"));
    }
    switch(rox->rox_op)
    {
    case    OP_READ :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedReadArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    case    OP_COMPARE :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedCompareArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    case    OP_ABANDON :
    {
	struct ds_abandon_arg * ab;
	success = decode_DAS_AbandonArgument(pe,1,NULLIP,NULLVP,&ab);
	di->di_invoke.dx_arg.dca_dsarg.arg_ab = *ab; /* struct copy */
	free ((char *)ab);
    }
	break;
    case    OP_LIST :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedListArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    case    OP_SEARCH :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedSearchArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    case    OP_ADDENTRY :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedAddEntryArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    case    OP_REMOVEENTRY :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedRemoveEntryArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    case    OP_MODIFYENTRY :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedModifyEntryArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    case    OP_MODIFYRDN :
    {
	struct ds_op_arg * da;
	success = decode_DO_ChainedModifyRDNArgument(pe,1,NULLIP,NULLVP,&da);
	di->di_invoke.dx_arg = *da; /* struct copy */
	free ((char *)da);
    }
	break;
    default:
	LLOG(log_dsap, LLOG_EXCEPTIONS, 
	     ("DspDecodeInvoke(%d): op id %d unknown!", sd, rox->rox_op));
	DRejectRequest (sd, ROS_IP_UNRECOG, rox->rox_id);
    	ROXFREE (rox);
	return (dsaplose (di, DP_INVOKE, NULLCP, "Unknown operation identifier"));
    }

    if (success == NOTOK)
    {
	LLOG (log_dsap, LLOG_EXCEPTIONS, 
	      ("DspDecodeInvoke (%d): Unable to parse argument",sd));
	DRejectRequest (sd, ROS_IP_MISTYPED, rox->rox_id);
    	ROXFREE (rox);
	return (dsaplose (di, DP_INVOKE, NULLCP, "Undecodable argument"));
    }

    di->di_invoke.dx_arg.dca_dsarg.arg_type = rox->rox_op;

    ROXFREE (rox);

    return(success);
}

int	  DspDecodeResult (sd, ror, di)
int			  sd;
struct RoSAPresult	* ror;
struct DSAPindication	* di;
{
    int			  success;
    PE			  pe = ror->ror_result;

    di->di_type = DI_RESULT;
    di->di_result.dr_id = ror->ror_id;

    switch(ror->ror_op)
    {
    case    OP_READ :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedReadResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;
    case    OP_COMPARE :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedCompareResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;
    case    OP_ABANDON :
    {
	struct ds_op_res *op;
	success = decode_DAS_AbandonResult(pe,1,NULLIP,NULLVP,&op);
        /* No result to copy */
	free ((char *)op);
    }
	break;
    case    OP_LIST :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedListResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;
    case    OP_SEARCH :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedSearchResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;
    case    OP_ADDENTRY :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedAddEntryResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;
    case    OP_REMOVEENTRY :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedRemoveEntryResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;
    case    OP_MODIFYENTRY :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedModifyEntryResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;
    case    OP_MODIFYRDN :
    {
	struct ds_op_res *op;
	success = decode_DO_ChainedModifyRDNResult(pe,1,NULLIP,NULLVP,&op);
	di->di_result.dr_res = *op; /* struct copy */
	free ((char *)op);
    }
	break;

    default:
	success = NOTOK;
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("DspDecodeResult (%d): op id %d unknown!", ror->ror_op, sd));
	DRejectRequest (sd, ROS_RRP_UNRECOG, ror->ror_id);
    	RORFREE (ror);
	return (dsaplose (di, DP_RESULT, NULLCP, "Unknown operation identifier"));
    }

    if (success == NOTOK)
    {
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("DspDecodeResult (%d): Unable to parse argument",sd));
	DRejectRequest (sd, ROS_RRP_MISTYPED, ror->ror_id);
    	RORFREE (ror);
	return (dsaplose (di, DP_RESULT, NULLCP, "Undecodable argument"));
    }

    di->di_result.dr_res.dcr_dsres.result_type = ror->ror_op;

    RORFREE (ror);

    return(success);
}

int	  QspDecodeInvoke (sd, rox, di)
int			  sd;
struct RoSAPinvoke	* rox;
struct DSAPindication	* di;
{
    int			  success;
    PE			  pe = rox->rox_args;

    if (rox -> rox_nolinked == 0) {
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("QspDecodeInvoke: Linked operation (%d) %d",sd,rox -> rox_linkid));
	DRejectRequest (sd, ROS_IP_LINKED, rox->rox_id);
	ROXFREE (rox);
	return (dsapreject (di, DP_INVOKE, -1, NULLCP, "Link op"));
    }

    di->di_type = DI_INVOKE;
    di->di_invoke.dx_id = rox->rox_id;

    switch(rox->rox_op)
    {
    case    OP_READ :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedReadArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_COMPARE :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedCompareArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_ABANDON :
    {
	struct ds_abandon_arg *arg;
	success = decode_DAS_AbandonArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg.dca_dsarg.arg_ab = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_LIST :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedListArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_SEARCH :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedSearchArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_ADDENTRY :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedAddEntryArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_REMOVEENTRY :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedRemoveEntryArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_MODIFYENTRY :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedModifyEntryArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_MODIFYRDN :
    {
	struct ds_op_arg *arg;
	success = decode_DO_ChainedModifyRDNArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    case    OP_GETEDB :
    {
	struct getedb_arg *arg;
	success = decode_Quipu_GetEntryDataBlockArgument(pe,1,NULLIP,NULLVP,&arg);
	di->di_invoke.dx_arg.dca_dsarg.arg_ge = *arg; /* struct copy */
	free ((char *)arg);
    }
	break;
    default:
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("QspDecodeInvoke (%d): op id %d unknown!", sd, rox->rox_op));
	DRejectRequest (sd, ROS_IP_UNRECOG, rox->rox_id);
	ROXFREE (rox);
	return (dsaplose (di, DP_INVOKE, NULLCP, "Unknown operation identifier"));
    }

    if (success == NOTOK)
    {
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("QspDecodeInvoke (%d): Unable to parse argument",sd));
	DRejectRequest (sd, ROS_IP_MISTYPED, rox->rox_id);
	ROXFREE (rox);
	return (dsaplose (di, DP_INVOKE, NULLCP, "Undecodable argument"));
    }

    di->di_invoke.dx_arg.dca_dsarg.arg_type = rox->rox_op;

    ROXFREE (rox);

    return (success);
}

int	  QspDecodeResult (sd, ror, di)
int			  sd;
struct RoSAPresult	* ror;
struct DSAPindication	* di;
{
    int			  success;
    PE			  pe = ror->ror_result;

    di->di_type = DI_RESULT;
    di->di_result.dr_id = ror->ror_id;

    switch(ror->ror_op)
    {
    case    OP_READ :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedReadResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_COMPARE :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedCompareResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_ABANDON :
    {
	struct ds_op_res *res;
	success = decode_DAS_AbandonResult(pe,1,NULLIP,NULLVP,&res);
	/* NO result to copy !!! */
	free ((char *)res);
    }
	break;
    case    OP_LIST :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedListResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_SEARCH :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedSearchResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_ADDENTRY :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedAddEntryResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_REMOVEENTRY :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedRemoveEntryResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_MODIFYENTRY :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedModifyEntryResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_MODIFYRDN :
    {
	struct ds_op_res *res;
	success = decode_DO_ChainedModifyRDNResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    case    OP_GETEDB :
    {
	struct getedb_result *res;
	success = decode_Quipu_GetEntryDataBlockResult(pe,1,NULLIP,NULLVP,&res);
	di->di_result.dr_res.dcr_dsres.res_ge = *res; /* sturct copy */
	free ((char *)res);
    }
	break;
    default:
	success = NOTOK;
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("QspDecodeResult (%d): op id %d unknown!", sd, ror->ror_op));
	DRejectRequest (sd, ROS_RRP_UNRECOG, ror->ror_id);
	RORFREE (ror);
	return (dsaplose (di, DP_RESULT, NULLCP, "Unknown operation identifier"));
    }

    if (success == NOTOK)
    {
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("QspDecodeResult (%d): Unable to parse argument",sd));
	DRejectRequest (sd, ROS_RRP_MISTYPED, ror->ror_id);
	RORFREE (ror);
	return (dsaplose (di, DP_RESULT, NULLCP, "Undecodable argument"));
    }

    di->di_result.dr_res.dcr_dsres.result_type = ror->ror_op;

    RORFREE (ror);
    return(success);
}

int	  DDecodeError (sd, roe, di)
int			  sd;
struct RoSAPerror	* roe;
struct DSAPindication	* di;
{
    int			  success;
    PE			  pe = roe->roe_param;
    struct DSError	* err = &(di->di_error.de_err);

#ifdef PDU_DUMP
    pdu_dump (pe,DUMP_ERR,roe->roe_id);
#endif

    di->di_type = DI_ERROR;
    di->di_error.de_id = roe->roe_id;

    switch(err->dse_type = roe->roe_error)
    {
    case    DSE_ABANDON_FAILED :
    {
        struct DSE_abandon_fail * de;
	success = decode_DAS_AbandonFailedParm(pe,1,NULLIP,NULLVP,&de);
	err->dse_un.dse_un_abandon_fail = *de; /* struct copy */
	free ((char *)de);
    }
	break;
    case    DSE_ATTRIBUTEERROR :
    {
        struct DSE_attribute * de;
	success = decode_DAS_AttributeErrorParm(pe,1,NULLIP,NULLVP,&de);
	err->dse_un.dse_un_attribute = *de; /* struct copy */
	free ((char *)de);
    }
	break;
    case    DSE_NAMEERROR :
    {
        struct DSE_name * de;
	success = decode_DAS_NameErrorParm(pe,1,NULLIP,NULLVP,&de);
	err->dse_un.dse_un_name = *de; /* struct copy */
	free ((char *)de);
    }
	break;
    case    DSE_REFERRAL :
    {
        struct DSE_referral * de;
	success = decode_DAS_ReferralParm(pe,1,NULLIP,NULLVP,&de);
	err->dse_un.dse_un_referral = *de; /* struct copy */
	free ((char *)de);
    }
	break;
    case    DSE_SECURITYERROR :
    {
        struct DSE_security * de;
	success = decode_DAS_SecurityErrorParm(pe,1,NULLIP,NULLVP,&de);
	err->dse_un.dse_un_security = *de; /* struct copy */
	free ((char *)de);
    }
	break;
    case    DSE_SERVICEERROR :
    {
        struct DSE_service * de;
	success = decode_DAS_ServiceErrorParm(pe,1,NULLIP,NULLVP,&de);
	err->dse_un.dse_un_service = *de; /* struct copy */
	free ((char *)de);
    }
	break;
    case    DSE_UPDATEERROR :
    {
        struct DSE_update * de;
	success = decode_DAS_UpdateErrorParm(pe,1,NULLIP,NULLVP,&de);
	err->dse_un.dse_un_update = *de; /* struct copy */
	free ((char *)de);
    }
	break;
    case    DSE_ABANDONED :
	success = ((pe == NULLPE) ? OK : NOTOK);
	break;
    case	DSE_DSAREFERRAL :
    {
        struct DSE_referral * de;
	success = decode_DO_DSAReferralParm(pe, 1, NULLIP, NULLVP, &de);
	err->dse_un.dse_un_referral = *de; /* struct copy */
	free ((char *)de);
    }
	break;

    default:
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("DDecodeError (%d): op id %d unknown!",sd, roe->roe_error));
	DRejectRequest (sd, ROS_REP_UNRECOG, roe->roe_id);
	ROEFREE (roe);
	return (dsaplose (di, DP_ERROR, NULLCP, "Unknown operation identifier"));
    }

    if (success == NOTOK)
    {
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("DDecodeError (%d): Unable to parse argument",sd));
	DRejectRequest (sd, ROS_RRP_MISTYPED, roe->roe_id);
	ROEFREE (roe);
	return (dsaplose (di, DP_ERROR, NULLCP, "Undecodable argument"));
    }

    ROEFREE (roe);
    return(success);
}

int	  DDecodeUnbind (sd, acf, di)
int			  sd;
struct AcSAPfinish	* acf;
struct DSAPindication	* di;
{
    struct RoNOTindication	  rni_s;
    struct RoNOTindication	* rni = &(rni_s);

    watch_dog("RoUnBindInit");
    if (RoUnBindInit (sd, acf, rni) != OK) {
        watch_dog_reset();
	return (ronot2dsaplose (di, "RoUnBindInit", rni));
    }
    watch_dog_reset();

    if (acf->acf_ninfo != 0)
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("Unbind has argument present! sd=%d", sd));

    di->di_type = DI_FINISH;
    di->di_finish.df_reason = acf->acf_reason;

    return (OK);
}

/* Isp == Qsp here */

int	  IspWaitRequest (sd, secs, di)
int			  sd;
int			  secs;
struct DSAPindication	* di;
{
	return QspWaitRequest (sd, secs, di);
}

int	  IspDecodeInvoke (sd, rox, di)
int			  sd;
struct RoSAPinvoke	* rox;
struct DSAPindication	* di;
{
	return QspDecodeInvoke (sd, rox, di);
}

int	  IspDecodeResult (sd, ror, di)
int			  sd;
struct RoSAPresult	* ror;
struct DSAPindication	* di;
{
	return QspDecodeResult (sd, ror, di);
}

/* Watchdog stuff */

IFP restart_fn = NULLIFP;

static char * watch_dog_where;
static watchdogfinal = FALSE;

watch_dog_final(fn)
SFP fn;
{
	watchdogfinal = TRUE;

	if (dsa_mode) {
		watch_dog_where = "FINAL";
		(void) signal (SIGALRM, fn);
		(void) alarm (watchdog_time);
	}
}

/* ARGSUSED */
static SFD watch_dog_activate (sd)
int sd;
{
static char called = FALSE;

	if (!called) {
		if (restart_fn == NULLIFP)
			return;
		else {
			called = TRUE;
			LLOG (log_dsap, LLOG_FATAL, ("Watchdog activated in %s", watch_dog_where));
			(*restart_fn)(-1);
		}
	} else 
		LLOG (log_dsap, LLOG_FATAL, ("Repeated lower level blocking in %s", watch_dog_where));
		
	exit(-1);
}

/* ARGSUSED */
static SFD slack_watch_dog_activate (sd)
int sd;
{
	LLOG (log_dsap, LLOG_EXCEPTIONS, 
	     ("Watchdog: blocking in %s, trying again...", 
	       watch_dog_where));

	watch_dog (watch_dog_where);
}

watch_dog (where)
char * where;
{
/*
	A simple timer to stop DSAs holding onto associations, due to
	a lower level failure.
*/
	if (watchdogfinal)
		return;

	if (dsa_mode) {
		watch_dog_where = where;
		(void) signal (SIGALRM, watch_dog_activate);
		(void) alarm (watchdog_time);
	}
}

static slack_watch_dog (where)
char * where;
{
/*
	A simple timer to stop DSAs holding onto associations, due to
	a lower level failure.
	Give two chances: log a warning first time around.
*/
	if (watchdogfinal)
		return;

	if (dsa_mode) {
		watch_dog_where = where;
		(void) signal (SIGALRM, slack_watch_dog_activate);
		(void) alarm (watchdog_time);
	}
}

watch_dog_aux (where,secs)
char * where;
unsigned secs;
{

	if (watchdogfinal)
		return;

	if (dsa_mode) {
		watch_dog_where = where;
		(void) signal (SIGALRM, watch_dog_activate);
		(void) alarm (secs);
	}
}

watch_dog_reset ()
{
	if (watchdogfinal)
		return;

	if (dsa_mode) {
		(void) signal (SIGALRM, SIG_IGN);
		(void) alarm ((unsigned) 0);
	}
}

watch_dog_final_reset ()
{
	if (dsa_mode) {
		(void) signal (SIGALRM, SIG_IGN);
		(void) alarm ((unsigned) 0);
	}
}

