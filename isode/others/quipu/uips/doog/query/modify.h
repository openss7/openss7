#ifndef _query_modify_h_
#define _query_modify_h_

#include "types.h"
#include "error.h"
#include "sequence.h"
#include "read.h"
#include "attrt_list.h"

/*
 * 	Structures and data types for `modify entry' request.
 */

typedef enum
{
  new,
  unchanged,
  deleted,
  updated
} modValState;

typedef struct _modify_value
{
  char *value;
  char *new_value;

  modValState status;

  struct _modify_value *next;
} modify_value, *modifyValue;

#define NULLModifyValue (modifyValue) NULL
#define modify_value_alloc() (modifyValue) smalloc(sizeof(modify_value))


typedef struct _modify_attr
{
  char *attr_name;
  AttributeType attr_type;

  QBool to_modify;

  modifyValue values;
  
  struct _modify_attr *next;
} modify_attr, *modifyAttr;

#define NULLModifyAttr (modifyAttr) NULL
#define modify_attr_alloc() (modifyAttr) smalloc(sizeof(modify_attr))


 /* Modify result structure */

typedef struct _modify_result
{
  QBool was_successful;
  errorList errors;
} modify_result, *modifyResult;

#define NULLModifyResult (modifyResult) NULL
#define modify_result_alloc() (modifyResult) smalloc(sizeof(modify_result))

 /* Modify request record */

typedef struct _modify_rec
{
  QCardinal request_id;
  char *base_object;
  int task_id;
  modifyResult result;
} modify_rec, *modifyRec;

#define NULLModifyRec (modifyRec) NULL
#define modify_rec_alloc() (modifyRec) smalloc(sizeof(modify_rec))


/*
 *	Structures and data types for `make entry template' requests.
 */

/*
 *	Make template request result structure
 */

typedef struct _make_template_result
{
  char *base_object;

  modifyAttr must_request;
  modifyAttr may_request;
  
  modifyAttr must_exc;
  modifyAttr may_exc;

  modifyAttr object_class;

  errorList errors;
} make_template_result, *makeTemplateResult;

#define NULLTemplateResult (makeTemplateResult) NULL
#define template_result_alloc() (makeTemplateResult) \
                                smalloc(sizeof(make_template_result))


/*
 *	Make template request record
 */

typedef struct _make_template_rec
{
  char *base_object;
  modifyAttr include_attrs;

  QCardinal request_id;
  int task_id;

  makeTemplateResult result;

  errorList errors;
} make_template_rec, *makeTemplateRec;

#define NULLTemplateRec (makeTemplateRec) NULL
#define template_rec_alloc() (makeTemplateRec) \
  			      smalloc(sizeof(make_template_rec))


/*
 *	Public procedures
 */

QE_error_code do_make_template();
makeTemplateResult get_make_template_result();

errorList do_modify();
modifyResult get_modify_result();

QE_error_code do_change_name();
void get_change_name_result();

QE_error_code do_add_entry();
void get_add_entry_result();

QE_error_code do_delete_entry();
void get_delete_entry_result();

void free_mod_attr_list(), free_mod_val_list(),
     free_make_template_result(), free_modify_result();

void free_make_template_rec(), modify_rec_free();

request_state process_modify_ds_result(), process_modify_ds_error();
request_state process_template_ds_result(), process_template_ds_error();

#endif
