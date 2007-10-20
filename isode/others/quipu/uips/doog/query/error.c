#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/error.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/error.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: error.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  error.c - Misc. functions for handling error lists.

*****************************************************************************/


#include "types.h"
#include "error.h"
#include "util.h"
#include "request.h"

#include "quipu/util.h"
#include "quipu/ds_error.h"

extern LLog *log_stat;

QE_error error_msgs[] =
{
  {QERR_ok, "OK"},
  {QERR_succeeded, "Successful"},
  {QERR_request_failed, "Request Failed!"},
  {QERR_referral_error, "Could Not Contact Remote Part Of Directory"},
  {QERR_bad_name, "Bad Name Supplied"},
  {QERR_internal_limit_reached, "Internal Limit Reached. Cannot Perform"},
  {QERR_bad_value_syntax, "Bad Value Supplied"},
  {QERR_time_limit_reached, "Time Limit Reached"},
  {QERR_size_limit_reached, "Size Limit Encountered"},
  {QERR_admin_limit_reached, "Administrative Limit Encountered"},
  {QERR_local_error, "Local Directory System Error"},
  {QERR_remote_error, "Remote Directory System Error"},
  {QERR_no_such_attr, "No Such Attribute"},
  {QERR_bad_attr_syntax, "Bad Attribute Syntax"},
  {QERR_no_such_object, "Unknown Directory Object"},
  {QERR_alias_problem, "Alias Problem"},
  {QERR_security_error, "Security Problem"},
  {QERR_service_busy, "Service Busy"},
  {QERR_service_unavailable, "Service Unavailable"},
  {QERR_chaining_required, "Chaining Required"},
  {QERR_unable_to_proceed, "Unable To Proceed"},
  {QERR_loop_detect, "Directory Loop Detected"},
  {QERR_ext_unavailable, "Directory Service Extension Unavailable"},
  {QERR_out_of_scope, "Out Of Scope"},
  {QERR_dit_error, "DIT Error"},
  {QERR_naming_violation, "Naming Violation"},
  {QERR_oc_violation, "Object Class Violation"},
  {QERR_not_on_nonleaf, "Not On Non Leaf Entry"},
  {QERR_not_on_rdn, "Not On RDN"},
  {QERR_already_exists, "Already Exists"},
  {QERR_affects_mult_dsas, "Affects Multiple DSAs"},
  {QERR_no_oc_mods, "No Object Class Modifications Allowed"},
  {QERR_no_such_op, "No Such Operation"},
  {QERR_too_late, "Operation Requested Too Late"},
  {QERR_cannot_abandon, "Operation Cannot Be Abandoned"},
  {QERR_nothing_found, "Nothing Found"},
  {QERR_internal, "Internal Error!"},
  {QERR_no_mods_supplied, "No modifications to make!"},
  {QERR_null, ""}
};

/*
 * - get_message_of_code() -
 *
 *
 */
char *get_message_of_code(code)
     QE_error_code code;
{
  QCardinal count;

  for (count = 0;
       error_msgs[count].error != QERR_null;
       count++)
    if (code == error_msgs[count].error)
      return error_msgs[count].error_message;

  return NULLCP;
}


/*
 * - char *get_message_from_ds_error() -
 *
 *
 */
char *ds_error_message(error)
     struct DSError *error;
{
  PS ps;
  char buffer[LINESIZE];
  char *str, *message;

  if (error == (struct DSError *) NULL)
    return NULLCP;

  if ((ps = ps_alloc(str_open)) == NULLPS)
    return NULLCP;


  if (str_setup(ps, buffer, LINESIZE, 1) == NOTOK)
    return NULLCP;

  ds_error(ps, error);
  *ps->ps_ptr = 0;
  ps_free(ps);

  str = buffer;

  if (*str != '\0')
    message = copy_string(str);
  else
    message = NULLCP;

  return message;
} /* get_message_from_ds_error */
     

/*
 * - add_error_to_request_rec() -
 * A dap error has occured. Record it in the given request record.
 *
 */
void add_error_to_request_rec(request, baseobject, error_type, error)
     requestRec request;
     char *baseobject;
     QE_error_code error_type;
     struct DSError *error;
{
  errorList new_err = error_alloc();

  new_err->baseobject = (baseobject == NULLCP? NULLCP:copy_string(baseobject));
  new_err->error_type = error_type;
  new_err->next = request->errors;

  new_err->ds_message = ds_error_message(error);

  request->errors = new_err;
} /* add_error_to_request_rec */


/* ARGSUSED */
QE_error_code get_log_error_type(error, task_id)
     struct DSError *error;
     int task_id;
{
  log_ds_error(error);

  switch (error->dse_type)
    {
    case DSE_LOCALERROR:
      return QERR_local_error;
      
    case DSE_REMOTEERROR:
      return QERR_remote_error;
      
    case DSE_NOERROR:
      return QERR_ok;
      
    case DSE_ATTRIBUTEERROR:
      return QERR_bad_attr_syntax;
      
    case DSE_NAMEERROR:
      return QERR_bad_name;
      
    case DSE_SERVICEERROR:
      return QERR_service_unavailable;
      
    case DSE_REFERRAL:
    case DSE_DSAREFERRAL:
      return QERR_referral_error;
      
    case DSE_ABANDONED:
      return QERR_ok;
      
    case DSE_ABANDON_FAILED:
      return QERR_ok;
      
    case DSE_SECURITYERROR:
      return QERR_security_error;

    case DSE_UPDATEERROR:
      return QERR_request_failed;
      
      default:
      return QERR_ok;
    }
}


/*
 * - error_list_free() -
 *
 *
 */
void error_list_free(error_list_ptr)
     errorList *error_list_ptr;
{
  errorList next_errors, errors = *error_list_ptr;

  while (errors != NULLError)
    {
      if (errors->baseobject != NULLCP) free(errors->baseobject);
      next_errors = errors->next;

      if (errors->ds_message != NULLCP)
	(void) free(errors->ds_message);

      free((char *) errors);
      errors = next_errors;
    }

  *error_list_ptr = NULLError;
} /* error_list_free */

/*
 * - error_list_copy() -
 *
 *
 */
errorList error_list_copy(list)
     errorList list;
{
  errorList new_list = NULLError, curr_error = NULLError;

  if (list == NULLError)
    return NULLError;

  for (; list != NULLError; list = list->next)
    {
      if (new_list == NULLError)
	new_list = curr_error = error_alloc();
      else
	curr_error = curr_error->next = error_alloc();

      curr_error->error_type = list->error_type;
      
      curr_error->baseobject = (list->baseobject != NULLCP ?
				copy_string(list->baseobject) :
				NULLCP);
      
      curr_error->ds_message = (list->ds_message != NULLCP ?
				copy_string(list->ds_message) :
				NULLCP);

      curr_error->next = NULLError;
    }

  return new_list;
} /* error_list_copy */

