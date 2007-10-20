/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ds_tasks.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_ds_task_h_
#define _query_ds_task_h_

#include "types.h"
#include "error.h"

#define MAXTASKS 512
#define NO_TASK -1

typedef enum
{
  SEARCH_TASK,
  LIST_TASK,
  READ_TASK,
  MODIFY_TASK,
  MODIFY_RDN_TASK,
  ADD_TASK,
  REMOVE_TASK,
  COMPARE_TASK,
  ABANDON_TASK
} task_type;
  
typedef struct _ds_task
{
  task_type type;
  int task_id;
  char *baseobject;
  QCardinal request_id;
  struct _ds_task *next;
} ds_task, *DsTask;

#define NULLDsTask (DsTask) NULL
#define ds_task_alloc() (DsTask) smalloc(sizeof(ds_task))

QE_error_code _task_invoked();
void _task_complete();
DsTask _get_task_of_id();
void abort_task();

#endif _query_ds_task_h_
