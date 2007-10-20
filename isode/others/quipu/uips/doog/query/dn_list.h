/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/dn_list.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_entry_list_h_
#define _query_entry_list_h_

#include "types.h"
#include "quipu/attr.h"

typedef struct ent_list {
  char *string_dn;
  char *sort_key;
  AttributeType object_class;
  struct ent_list *next;
} entry_list, *entryList;

#define NULLEntryList (entryList) NULL
#define entry_list_alloc() (entryList) smalloc(sizeof(entry_list))

void dn_list_free(), dn_list_copy();
QBool dn_list_add(), dn_list_merge(), dn_list_insert();

#endif _query_entry_list_h_




