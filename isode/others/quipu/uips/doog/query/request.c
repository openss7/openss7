#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/request.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/request.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: request.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  request.c - Manage requests to the query engine.

*****************************************************************************/

#include "types.h"
#include "error.h"
#include "request.h"
#include "ds_tasks.h"
#include "util.h"

#include "quipu/dap2.h"

requestRec live_requests = NULLReqRec;
QCardinal live_req_count = 0;
QCardinal next_req_id = 1;

extern QCardinal live_task_count;
extern DsTask live_task_list;

/* Directory association descriptor. */
extern int dsap_ad;

static request_state process_dap_result(), process_dap_error();
static void request_rec_free();

/*
 * - abort_request() -
 * Abort request of given id.
 *
 */
void abort_request(request_id)
     QCardinal request_id;
{
  DsTask curr_task;

  if (_get_request_of_id(request_id) == NULLReqRec)
    return;
  
  curr_task = live_task_list;
  
  while (curr_task != NULLDsTask)
    {
      if (curr_task->request_id == request_id)
	{
	  abort_task(curr_task->task_id);
	  
	  curr_task = live_task_list;
	}
      else
	curr_task = curr_task->next;
    }

  _request_complete(request_id);
} /* abort_request */

/*
 * - directory_wait() -
 * Process one incoming result/error. 
 * Check if any user requests are complete and processes them. If the 
 * relevant user request has completed, return true and pass back the results.
 *
 */
QCardinal directory_wait(id_ptr)
     QCardinal **id_ptr;
{
  struct DAPindication indication;
  QCardinal aindex, req_id;
  QCardinal result_list[MAXREQUESTS];
  QCardinal rlist_index = 0;

  /*
   *	Poll directory for an incoming result/error
   */

  while (DapInitWaitRequest(dsap_ad, 0, &indication) == OK)
    {
      switch (indication.di_type)
	{
	case DI_RESULT:
	  
	  if (_get_task_of_id(indication.di_result.dr_id) == NULLDsTask)
	    /* Task can be ignored */
	    ;
	  else
	    if (process_dap_result(&indication.di_result, &req_id)
		== RQ_results_returned)
	      result_list[rlist_index++] = req_id;
	  
	  ds_res_free(&indication.di_result.dr_res);

	  break;

	case DI_ERROR:
	  
	  if (_get_task_of_id(indication.di_error.de_id) == NULLDsTask)
	    /* Task can be ignored */
	    ;
	  else
	    if (process_dap_error(&indication.di_error, &req_id)
		== RQ_results_returned)
	      result_list[rlist_index++] = req_id;

	  ds_error_free(&indication.di_error.de_err);

	  break;
	}
    }

  if (rlist_index > 0)
    {
      QCardinal *return_list;
      
      return_list = (QCardinal *) smalloc((int)(sizeof(QCardinal) * rlist_index));
      *id_ptr = return_list;

      for (aindex = 0; aindex < rlist_index; aindex++)
	return_list[aindex] = result_list[aindex];
      
      return rlist_index;
    }
  else
    {
      *id_ptr = (QCardinal *) NULL;
      return 0;
    }
} /* directory_wait */


/*
 * - _request_invoked() -
 * Add invoked user request to request array and pass back identifier.
 *
 */
QE_error_code _request_invoked(type, return_id_ptr)
     request_type type;
     QCardinal *return_id_ptr;
{
  requestRec new_request;
  QCardinal aindex;
  ufnameRec ufnrec;
  ufsearchRec ufsrec;
  readRec readrec;
  readDnAttrRec readdnrec;
  modifyRec modifyrec;
  makeTemplateRec temprec;

  *return_id_ptr = 0;

  if (live_req_count == MAXREQUESTS) return QERR_request_failed;

  new_request = request_rec_alloc();

  new_request->next = live_requests;
  new_request->prev = NULLReqRec;

  new_request->request_id = *return_id_ptr = next_req_id++;

  if (live_requests == NULLReqRec)
    live_requests = new_request;
  else
    {
      live_requests->prev = new_request;
      live_requests = new_request;
    }

  new_request->errors = NULLError;
  new_request->type = type;
  
  switch (type)
    {

    case UFSEARCH:

      ufsrec = new_request->UFSEARCH_REC = ufsearch_rec_alloc();
      ufsrec->request_id = *return_id_ptr;
      
      ufsrec->exact_task_count = ufsrec->approx_task_count = 0;
      ufsrec->tasks_sent = ufsrec->tasks_failed = 0;
      
      ufsrec->tried_target_search = ufsrec->trying_target_search = FALSE;
      
      ufsrec->search_data = ufsrec->target_data = NULLSearchPair;
      ufsrec->followed = ufsrec->to_follow = ufsrec->path = NULLEntryList;
      
      for (aindex = 0; aindex < MaxUfsearchTasks; aindex++)
	{
	  ufsrec->exact_task_ids[aindex] =
	    ufsrec->approx_task_ids[aindex] = NO_TASK;
	}

      ufsrec->result = NULLUfsearchResult;
      
      break;
      
  case UFNAME:

      ufnrec = new_request->UFNAME_REC = ufname_rec_alloc();
      ufnrec->request_id = *return_id_ptr;
      
      ufnrec->exact_task_count = ufnrec->approx_task_count = 0;
      ufnrec->name_parts = NULLNamePart;
      
      for (aindex = 0; aindex < MAX_TASKS_PER_REQ; aindex++)
	{
	  ufnrec->dap_exact_task_ids[aindex] =
	    ufnrec->dap_approx_task_ids[aindex] = NO_TASK;
	}

      break;
      
    case DS_READ:

      readrec = new_request->READ_REC = read_rec_alloc();
      readrec->request_id = *return_id_ptr;
      
      readrec->task_id = NO_TASK;
      readrec->results = NULLReadResults;
      
      break;

    case READ_DN_ATTR:
      
      readdnrec = new_request->READ_DN_ATTR_REC = read_dn_attr_rec_alloc();
      readdnrec->request_id = *return_id_ptr;
      
      readdnrec->task_id = NO_TASK;
      readdnrec->results = NULLReadResults;

      break;

    case MODIFY_ENTRY:

      modifyrec = new_request->MODIFY_REC = modify_rec_alloc();
      modifyrec->request_id = *return_id_ptr;

      modifyrec->task_id = NO_TASK;
      modifyrec->result = NULLModifyResult;

      break;

    case ENTRY_TEMPLATE:
      
      temprec = new_request->TEMPLATE_REC = template_rec_alloc();

      temprec->request_id = *return_id_ptr;
      temprec->task_id = NO_TASK;

      temprec->result = NULLTemplateResult;

      temprec->base_object = NULLCP;
      temprec->include_attrs = NULLModifyAttr;

      break;
    }

  live_req_count++;

  return QERR_ok;
} /* _request_invoked */


/*
 * - _request_complete() -
 *
 * The id'ed request has aborted or returned. Remove from outstanding list.
 *
 */
void _request_complete(request_id)
     QCardinal request_id;
{
  requestRec old_request;

  if ((old_request = _get_request_of_id(request_id)) == NULLReqRec) return;

  live_req_count--;

  if (old_request->prev == NULLReqRec)
    live_requests = old_request->next;
  else
    old_request->prev->next = old_request->next;

  old_request->next = NULLReqRec;

  request_rec_free(old_request);
} /* _request_complete */

/*
 * - request_rec_free() -
 * Free a request_rec structure.
 *
 */
static void request_rec_free(request)
     requestRec request;
{
  if (request == NULLReqRec) return;
  
  switch (request->type)
    {
    case ENTRY_TEMPLATE:
      
      free_make_template_rec(request->TEMPLATE_REC);
      break;

    case UFNAME:

      ufname_rec_free(request->UFNAME_REC);
      break;

    case UFSEARCH:

      ufsearch_rec_free(request->UFSEARCH_REC);
      break;

    case DS_READ:

      read_rec_free(request->READ_REC);
      break;

    case READ_DN_ATTR:
      
      read_dn_attr_rec_free(request->READ_DN_ATTR_REC);
      break;

    case MODIFY_ENTRY:
      
      modify_rec_free(request->MODIFY_REC);
      break;
    }

  free((char *) request);
} /* request_rec_free */
  
/*
 * - process_dap_result() -
 * Decode results and place into appropriate request_rec for later dispatch.
 *
 */
static request_state process_dap_result(dap_result, id_ptr)
     struct DAPresult *dap_result;
     QCardinal *id_ptr;
{
  requestRec request;
  DsTask task_rec;
  int task_id = dap_result->dr_id;
  QCardinal request_id;
  struct DSResult *ds_result = &dap_result->dr_res;
  
  if ((task_rec = _get_task_of_id(task_id)) == NULLDsTask)
    return RQ_processing;

  *id_ptr = request_id = task_rec->request_id;
  request_id = task_rec->request_id;

  if ((request = _get_request_of_id(request_id)) == NULLReqRec)
    /* This shouldn't happen. */
    return RQ_processing;

  switch (request->type)
    {
    case UFNAME:

      return process_ufn_ds_result(request, task_id, ds_result);

    case UFSEARCH:

      return process_ufs_ds_result(request, task_id, ds_result);

    case ENTRY_TEMPLATE:
      
      return process_template_ds_result(request, task_id, ds_result);
      
    case DS_READ:

      return process_read_ds_result(request, task_id, ds_result);

    case READ_DN_ATTR:

      return process_read_dn_attr_result(request, task_id, ds_result);

    case MODIFY_ENTRY:
      
      return process_modify_ds_result(request, task_id, ds_result);
    }

  return RQ_processing;
} /* process_dap_result */

/*
 * - process_dap_error() -
 *
 *
 */
static request_state process_dap_error(error, id_ptr)
     struct DAPerror *error;
     QCardinal *id_ptr;
{
  requestRec request;
  DsTask task_rec;
  int task_id = error->de_id;
  QCardinal request_id;
  struct DSError *ds_error = &error->de_err;
  
  if ((task_rec = _get_task_of_id(task_id)) == NULLDsTask)
    {
      (void) get_log_error_type(ds_error, task_id);
      return RQ_processing;
    }

  *id_ptr = request_id = task_rec->request_id;
  if ((request = _get_request_of_id(request_id)) == NULLReqRec)
    return RQ_processing;

  switch (request->type)
    {
    case UFNAME:

      return process_ufn_ds_error(request, task_id, ds_error);
      
    case UFSEARCH:

      return process_ufs_ds_error(request, task_id, ds_error);

    case ENTRY_TEMPLATE:
      
      return process_template_ds_error(request, task_id, ds_error);

    case DS_READ:

      return process_read_ds_error(request, task_id, ds_error);

    case READ_DN_ATTR:

      return process_read_dn_attr_error(request, task_id, ds_error);

    case MODIFY_ENTRY:
      
      return process_modify_ds_error(request, task_id, ds_error);
    }

  return RQ_processing;
}

/*
 * - _get_request_of_id() -
 * Return a pointer to the identified request record. If not found
 * return null pointer.
 */
requestRec _get_request_of_id(request_id)
     QCardinal request_id;
{
  requestRec request;
  QCardinal count;
  
  for (count = 0, request = live_requests;
       count < live_req_count && request_id != request->request_id;
       count++, request = request->next)
    ;

  if (count == live_req_count)
    return NULLReqRec;
  else
    return request;
} /* _get_request_of_id */
