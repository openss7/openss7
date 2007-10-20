#ifndef _query_ufsearch_h_
#define _query_ufsearch_h_

#include "types.h"
#include "util.h"
#include "dn_list.h"
#include "sequence.h"
#include "error.h"

typedef struct _search_pair {
  AttributeType type;
  stringList values;
  QBool use_subtree;
  QBool checked;
  struct _search_pair *next;
} search_pair, *searchPair, *searchPairList;

#define NULLSearchPair (searchPair) NULL
#define search_pair_alloc() (searchPair) smalloc(sizeof(search_pair))

typedef enum {UFS_Succeeded, UFS_Failed} ufsearchState;
typedef struct _ufsearch_result {
  ufsearchState search_status;

  QCardinal tasks_sent;
  QCardinal tasks_failed;

  entryList hits;
  QCardinal hit_num;

  errorList errors;
} ufsearch_result, *ufsearchResult;

#define NULLUfsearchResult (ufsearchResult) NULL
#define ufsearch_res_alloc() (ufsearchResult) smalloc(sizeof(ufsearch_result))
  
typedef struct _ufsearch_rec {
  searchPairList search_data;
  searchPair target_data;

  QBool tried_target_search;
  QBool trying_target_search;

  QCardinal tasks_sent;
  QCardinal tasks_failed;

#define MaxUfsearchTasks 128
  int exact_task_ids[MaxUfsearchTasks];
  int approx_task_ids[MaxUfsearchTasks];

  QCardinal exact_task_count;
  QCardinal approx_task_count;

  entryList followed;
  entryList to_follow;

  entryList path;

  QCardinal to_follow_num;

  QCardinal request_id;
  ufsearchResult result;
} ufsearch_rec, *ufsearchRec;

#define NULLUfsearchRec (ufsearchRec) NULL
#define ufsearch_rec_alloc() (ufsearchRec) smalloc(sizeof(ufsearch_rec))

/* Public procedures */
QE_error_code do_ufsearch();

request_state process_ufs_ds_result();
request_state process_ufs_ds_error();
  
ufsearchResult get_ufsearch_result();

void ufsearch_rec_free(), ufsearch_result_free();
void search_pair_list_free();

#endif
