#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/sequence.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/sequence.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: sequence.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  sequence.c - Linked list for string storage.

*****************************************************************************/

#include "util.h"
#include "sequence.h"

/*
 * - add_string_to_seq() -
 * Insert a string into a sequence.
 *
 */
void add_string_to_seq(string, sequence)
     char *string;
     stringCell *sequence;
{
  stringCell head = cell_alloc();
  head->string = copy_string(string);
  head->next = *sequence;
  *sequence = head;
} /* add_string_to_seq */

/*
 * - copy_string_seq() -
 * 
 *
 */
stringCell copy_string_seq(sequence)
     register stringCell sequence;
{
  register stringCell curr;
  stringCell head;

  if (sequence == NULLStrCell)
    return NULLStrCell;

  if (sequence != NULLStrCell)
    head = curr = cell_alloc();

  curr->next = NULLStrCell;
  curr->string = copy_string(sequence->string);

  sequence = sequence->next;

  while (sequence != NULLStrCell)
    {
      curr = curr->next = cell_alloc();

      curr->next = NULLStrCell;
      curr->string = copy_string(sequence->string);

      sequence = sequence->next;
    }

  return head;
} /* copy_string_seq */

/*
 * - delete_from_seq() -
 * Delete a string from a sequence.
 *
 *
 */
void delete_from_seq(string, sequence)
     char *string;
     stringCell *sequence;
{
  stringCell curr_cell = *sequence;
  stringCell last_cell = NULLStrCell;

  while (curr_cell != NULLStrCell && strcmp(curr_cell->string, string) != 0)
    {
      last_cell = curr_cell;
      curr_cell = curr_cell->next;
    }

  if (curr_cell != NULLStrCell)
    {
      if (last_cell == NULLStrCell)
	*sequence = curr_cell->next;
      else
	last_cell->next = curr_cell->next;
      
      curr_cell->next = NULLStrCell;
      free_string_seq(&curr_cell);
    }
} /* delete_from_seq */


/*
 * - free_string_seq() -
 * Free a string sequence.
 *
 */
void free_string_seq(sequence)
     stringCell *sequence;
{
  register stringCell next_cell, curr_cell = *sequence;
  
  while (curr_cell != NULLStrCell)
    {
      free(curr_cell->string);
      next_cell = curr_cell->next;
      free((char *) curr_cell);
      curr_cell = next_cell;
    }

  *sequence = NULLStrCell;
} /* free_string_seq */



