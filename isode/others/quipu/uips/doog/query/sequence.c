/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/sequence.c,v 9.0 1992/06/16 12:45:27 isode Rel";
#endif

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/sequence.c,v 9.0 1992/06/16 12:45:27 isode Rel */

/*
 * Log: sequence.c,v
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
void
add_string_to_seq(string, sequence)
	char *string;
	stringCell *sequence;
{
	stringCell head = cell_alloc();

	head->string = copy_string(string);
	head->next = *sequence;
	*sequence = head;
}				/* add_string_to_seq */

/*
 * - copy_string_seq() -
 * 
 *
 */
stringCell
copy_string_seq(sequence)
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

	while (sequence != NULLStrCell) {
		curr = curr->next = cell_alloc();

		curr->next = NULLStrCell;
		curr->string = copy_string(sequence->string);

		sequence = sequence->next;
	}

	return head;
}				/* copy_string_seq */

/*
 * - delete_from_seq() -
 * Delete a string from a sequence.
 *
 *
 */
void
delete_from_seq(string, sequence)
	char *string;
	stringCell *sequence;
{
	stringCell curr_cell = *sequence;
	stringCell last_cell = NULLStrCell;

	while (curr_cell != NULLStrCell && strcmp(curr_cell->string, string) != 0) {
		last_cell = curr_cell;
		curr_cell = curr_cell->next;
	}

	if (curr_cell != NULLStrCell) {
		if (last_cell == NULLStrCell)
			*sequence = curr_cell->next;
		else
			last_cell->next = curr_cell->next;

		curr_cell->next = NULLStrCell;
		free_string_seq(&curr_cell);
	}
}				/* delete_from_seq */

/*
 * - free_string_seq() -
 * Free a string sequence.
 *
 */
void
free_string_seq(sequence)
	stringCell *sequence;
{
	register stringCell next_cell, curr_cell = *sequence;

	while (curr_cell != NULLStrCell) {
		free(curr_cell->string);
		next_cell = curr_cell->next;
		free((char *) curr_cell);
		curr_cell = next_cell;
	}

	*sequence = NULLStrCell;
}				/* free_string_seq */
