/* conn_dispatch.c - deal with an event on an open connection */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/conn_disp.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/conn_disp.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: conn_disp.c,v $
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


#include "quipu/dsap.h"
#include "tsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

#ifdef QUIPU_CONSOLE
#include "quipu/attrvalue.h"
static void running_analyse() ;
#endif /* QUIPU_CONSOLE */

extern LLog * log_dsap;
extern char quipu_shutdown;

/*
* Something has happened on the association with descriptor "ad".
* Check what type of activity it is and dispatch to an appropriate
* routine to handle the activity.
*/
conn_dispatch(cn)
struct connection	* cn;
{
    int				  result;
    struct DSAPindication      di_s;
    struct DSAPindication      *di = &di_s;

    DLOG (log_dsap,LLOG_TRACE,( "conn_dispatch()"));

    bzero ((char *)di, sizeof di_s);

    result = DWaitRequest(cn->cn_ctx, cn->cn_ad, OK, di);

#ifdef QUIPU_CONSOLE
    running_analyse(cn, di) ;
#endif /* QUIPU_CONSOLE */

    if (result == DONE)
    {
	/* TIMER expired */
	return;
    }

    if (result == NOTOK)
    {
        switch(di->di_type)
        {
	case DI_PREJECT:
	    DLOG(log_dsap, LLOG_DEBUG, ("conn_dispatch calling oper_preject"));
	    oper_preject(cn, &(di->di_preject));
	    return;

	case DI_ABORT:
	    LLOG(log_dsap, LLOG_NOTICE, ("DWaitRequest - abort"));
	    do_ds_unbind (cn);
	    conn_extract(cn);
	    return;

	default:
	    LLOG (log_dsap,LLOG_EXCEPTIONS,
		  ("Unknown indication type: %d (%d)", 
		   di->di_type, cn->cn_ad));
	    return;
	}
    }

    switch(di->di_type)
    {
	case DI_INVOKE:
	    if (task_invoke(cn, &(di->di_invoke)) != OK)
	    {
		if (!quipu_shutdown) LLOG (log_dsap,LLOG_EXCEPTIONS,
		      ("task_invoke failed in conn_dispatch (%d)",cn->cn_ad));
	    }
	    break;

	case DI_RESULT:
	    oper_result(cn, di);
	    break;

	case DI_ERROR:
	    oper_error(cn, di);
	    break;

	case DI_FINISH:
	    conn_finish(cn, &(di->di_finish));
	    break;

	default:
	    LLOG (log_dsap,LLOG_EXCEPTIONS,( 
		 "Unknown indication type: %d (%d)", di->di_type,cn->cn_ad));
	    break;
    }
}

#ifdef QUIPU_CONSOLE
static void
running_analyse(cn, di)
struct connection *cn ;
struct DSAPindication *di ;
{
  extern AV_Sequence open_call_avs ;
  extern time_t timenow ;
  AV_Sequence tmp_avs ;
  struct op_list *tmp_op_list ;
  struct DSArgument *arg ;
  UTCtime ut ;
  time_t timenow ;

#ifdef SPT_DEBUG
  fprintf(stderr, "Ping! We have a running_analysis to do...\n") ;
#endif

  /* SPT: Must do something in here for the other cases! */
  if (di->di_type == DI_INVOKE)
  {
    /* Move along the open_call_avs to find a matching address, DN and association ID */

    tmp_avs = open_call_avs ;

    if (tmp_avs == (AV_Sequence) 0)
    {
#ifdef SPT_DEBUG
      fprintf(stderr, "dsa_wait.c: Hey, got a running analyse with no calls present. Error!\n") ;
#endif
      return ;
    }

    while (tmp_avs != (AV_Sequence) 0)
    {
      if ((((struct quipu_call *)tmp_avs->avseq_av.av_struct)->assoc_id == cn->cn_start.cs_ds.ds_sd) &&
	  (((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time == (char *) 0))
      {
	/* Same Assoc Number and the connection has not been finished! Match? */
	break ;
      }	 
      tmp_avs = tmp_avs->avseq_next ;
    }

    if (tmp_avs == (AV_Sequence) 0)
    {
#ifdef SPT_DEBUG
      fprintf(stderr, "We have no match here. Odd. Check...\n") ;
#endif
      return ;
    }

    /* Now we have the appropriate quipu_call structure, we can start filling out the op_list */
    /* Note that we add them to the pending operations. Move them over to invoked operations */
    /* in task_select() once they are chosen... */

    /* Before that, move to the end of a possibly null op_list and add another op onto the end. */
    if (((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops == (struct op_list *) 0)
    {
      ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops = 
	(struct op_list *) calloc (1, sizeof (struct op_list)) ;
      tmp_op_list = ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops ;
      tmp_op_list->operation_list = (struct ops *) calloc (1, sizeof(struct ops)) ;
    }
    else
    {
      tmp_op_list = ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops ;

      while (tmp_op_list->next != (struct op_list *) 0)
	tmp_op_list = tmp_op_list->next ;

      tmp_op_list->next = (struct op_list *) calloc (1, sizeof (struct op_list)) ;
      tmp_op_list = tmp_op_list->next ;
      tmp_op_list->operation_list = (struct ops *) calloc (1, sizeof(struct ops)) ;
    }

    arg = &(di->di_invoke.dx_arg.dca_dsarg) ;

    tmp_op_list->operation_list->operation_id = arg->arg_type ;
    tmp_op_list->operation_list->invoke_id = di->di_invoke.dx_id ;
    switch (arg->arg_type) 
    {
    case OP_READ:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_rd.rda_object) ;
	break;
      }
    case OP_COMPARE:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_cm.cma_object) ;
	break;
      }
    case OP_LIST:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_ls.lsa_object) ;
	break;
      }
    case OP_SEARCH:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_sr.sra_baseobject) ;
	break;
      }
    case OP_ADDENTRY:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_ad.ada_object) ;
	break;
      }
    case OP_REMOVEENTRY:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_rm.rma_object) ;
	break;
      }
    case OP_MODIFYENTRY:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_me.mea_object) ;
	break;
      }
    case OP_MODIFYRDN:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_mr.mra_object) ;
	break;
      }
    case OP_GETEDB:
      {
	tmp_op_list->operation_list->base_object = dn_cpy(arg->arg_ge.ga_entry) ;
	break;
      }
    default:
      {
	tmp_op_list->operation_list->base_object = NULLDN;
	break;
      }
    }
    (void) time(&timenow) ;
    tm2ut(gmtime(&timenow), &ut) ;
    tmp_op_list->operation_list->start_time = strdup(utct2str(&ut)) ;
    tmp_op_list->operation_list->finish_time = (char *) 0 ;
  }
  else
  if (di->di_type == DI_RESULT)
  {
    tmp_avs = open_call_avs ;

    if (tmp_avs == (AV_Sequence) 0)
    {
#ifdef SPT_DEBUG
      fprintf(stderr, "dsa_wait.c: Hey, got a running analyse with no calls present. Error!\n") ;
#endif
      return ;
    }

    while (tmp_avs != (AV_Sequence) 0)
    {
      if ((((struct quipu_call *)tmp_avs->avseq_av.av_struct)->assoc_id == cn->cn_ad) &&
	  (((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time == (char *) 0))
      {
	/* Same Assoc Number and the connection has not been finished! Match? */
	break ;
      }	 
      tmp_avs = tmp_avs->avseq_next ;
    }

    if (tmp_avs == (AV_Sequence) 0)
    {
#ifdef SPT_DEBUG
      fprintf(stderr, "We have no match here. Odd. Check...\n") ;
#endif
      return ;
    }
    
    /* Before that, move to the end of a possibly null op_list and add another op onto the end. */
    if (((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops == (struct op_list *) 0)
    {
      ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops = 
	(struct op_list *) calloc (1, sizeof (struct op_list)) ;
      tmp_op_list = ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops ;
      tmp_op_list->operation_list = (struct ops *) calloc (1, sizeof(struct ops)) ;
    }
    else
    {
      tmp_op_list = ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops ;

      while (tmp_op_list->next != (struct op_list *) 0)
	tmp_op_list = tmp_op_list->next ;

      tmp_op_list->next = (struct op_list *) calloc (1, sizeof (struct op_list)) ;
      tmp_op_list = tmp_op_list->next ;
      tmp_op_list->operation_list = (struct ops *) calloc (1, sizeof(struct ops)) ;
    }
    tmp_op_list->operation_list->operation_id = 0 ;
    tmp_op_list->operation_list->invoke_id = cn->cn_op_id ;
    tmp_op_list->operation_list->base_object = dn_cpy(cn->cn_dn) ;
    (void) time(&timenow) ;
    tm2ut(gmtime(&timenow), &ut) ;
    tmp_op_list->operation_list->start_time = strdup(utct2str(&ut)) ;
    tmp_op_list->operation_list->finish_time = (char *) 0 ;
  }
  else
  if ((di->di_type == DI_FINISH) ||
      (di->di_type == DI_ABORT))
  {
    tmp_avs = open_call_avs ;

    while (tmp_avs != (AV_Sequence) 0)
    {
      if ((((struct quipu_call *)tmp_avs->avseq_av.av_struct)->assoc_id == cn->cn_ad) &&
	  (((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time == (char *) 0))
      {
	/* Same Assoc Number and the connection has not been finished! Match? */
	break ;
      }	 
      tmp_avs = tmp_avs->avseq_next ;
    }

    if (tmp_avs == (AV_Sequence) 0)
    {
#ifdef SPT_DEBUG
      fprintf(stderr, "We have no match here. Odd. Check...\n") ;
#endif
      return ;
    }

    /* Now fill out the finish time */
    (void) time(&timenow) ;
    tm2ut(gmtime(&timenow), &ut) ;
    ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time = strdup(utct2str(&ut)) ;
#ifdef SPT_DEBUG
      fprintf(stderr, "Finished Conn %d in conn_disp.c\n", cn->cn_ad) ;
#endif

  }
#ifdef SPT_DEBUG
  else
  {
    fprintf(stderr, "Hey, neglected to add a task here of type %d.\n", di->di_type) ;
  }
#endif
  return ;
}
#endif /* QUIPU_CONSOLE */
