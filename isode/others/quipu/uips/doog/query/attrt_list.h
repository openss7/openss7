#ifndef _query_attr_type_list_h_
#define _query_attr_type_list_h_

#include "types.h"
#include "quipu/attr.h"


/*
 *	Storage cell for a list of AttributeTypes
 */
typedef struct _attr_type_cell
{
  AttributeType type;

  struct _attr_type_cell *next;
} attr_type_cell, *attrTypeList;

#define NULLAttrTypeList (attrTypeList) NULL
#define at_cell_alloc() (attrTypeList) smalloc(sizeof(attr_type_cell))


/*
 *	Procs
 */
QBool add_type_to_list(), delete_type_from_list();
void free_at_list();

#endif 
