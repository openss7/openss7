/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/sequence.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_sequence_h_
#define _query_sequence_h_

#include "types.h"

typedef struct _string_cell
{
  char *string;
  struct _string_cell *next;
} string_cell, *stringCell, *stringList;

#define NULLStrCell (stringCell) NULL
#define cell_alloc() (stringCell) smalloc(sizeof(string_cell))

void add_string_to_seq(),
     delete_from_seq(),
     free_string_seq();

stringCell copy_string_seq();

#endif _query_sequence_h_
