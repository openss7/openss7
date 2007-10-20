/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/error.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_error_h_
#define _query_error_h_

typedef enum
{
  QERR_ok,
  QERR_succeeded,
  QERR_request_failed,
  QERR_referral_error,
  QERR_bad_name,
  QERR_internal_limit_reached,
  QERR_bad_value_syntax,
  QERR_time_limit_reached,
  QERR_size_limit_reached,
  QERR_admin_limit_reached,
  QERR_local_error,
  QERR_remote_error,
  QERR_no_such_attr,
  QERR_bad_attr_syntax,
  QERR_no_such_object,
  QERR_alias_problem,
  QERR_security_error,
  QERR_service_busy,
  QERR_service_unavailable,
  QERR_chaining_required,
  QERR_unable_to_proceed,
  QERR_loop_detect,
  QERR_ext_unavailable,
  QERR_out_of_scope,
  QERR_dit_error,
  QERR_naming_violation,
  QERR_oc_violation,
  QERR_not_on_nonleaf,
  QERR_not_on_rdn,
  QERR_already_exists,
  QERR_affects_mult_dsas,
  QERR_no_oc_mods,
  QERR_no_such_op,
  QERR_too_late,
  QERR_cannot_abandon,
  QERR_nothing_found,
  QERR_internal,
  QERR_no_mods_supplied,
  QERR_null
} QE_error_code;

typedef struct _QE_error
{
  QE_error_code error;
  char *error_message;
} QE_error;


typedef struct _error_list
{
  char *baseobject;
  QE_error_code error_type;
  char *ds_message;
  struct _error_list *next;
} error_list, *errorList;

#define NULLError (errorList) NULL
#define error_alloc() (errorList) smalloc(sizeof(error_list))

void error_list_free();
errorList error_list_copy();

void add_error_to_request_rec();
char *ds_error_message();
QE_error_code get_log_error_type();

#endif _query_error_h_
