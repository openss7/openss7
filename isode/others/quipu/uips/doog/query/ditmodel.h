#ifndef _query_dit_model_h_
#define _query_dit_model_h_

#include "types.h"
#include "dn_list.h"
#include "util.h"
#include "sequence.h"

typedef struct _object_type
{
  AttributeType object_type;
  QBool use_subtree;
  struct _object_type *next;
} *objectType, *objectTypeList;

#define NULLObjectType (objectType) NULL
#define object_type_alloc() (objectType) smalloc(sizeof(struct _object_type))

typedef struct _dit_relation
{
  AttributeType parent_type;
  objectTypeList child_types;
  
  struct _dit_relation *next;
} dit_relation, *ditRelation;

#define MaxDitTypes = 128;

#define NULLDitRelation (ditRelation) NULL
#define dit_relation_alloc() (ditRelation) smalloc(sizeof(dit_relation))

typedef struct _search_info
{
  AttributeType object_type;
  char *type_label;

  objectTypeList search_on_list;

  struct _search_info *next;
} search_info, *searchInfo, *searchInfoList;

#define NULLSearchInfo (searchInfo) NULL
#define search_info_alloc() (searchInfo) smalloc(sizeof(search_info))

typedef struct _ufs_path
{
  AttributeType object_type;
  entryList path;

  struct _ufs_path *next;
} ufs_path, *ufsPath;

#define NULLUfsPath (ufsPath) NULL
#define ufs_path_alloc() (ufsPath) smalloc(sizeof(ufs_path))

void set_relation(), set_search_attrs(), set_default_path();
objectTypeList get_child_list(), get_search_attrs();
entryList get_default_path();

QBool is_oc_must(), is_oc_may(), is_search_attr();
char *get_type_label();

#endif
