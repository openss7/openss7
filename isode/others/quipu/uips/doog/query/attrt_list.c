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
    "Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/attrt_list.c,v 9.0 1992/06/16 12:45:27 isode Rel";
#endif

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/attrt_list.c,v 9.0 1992/06/16 12:45:27 isode Rel */

/*****************************************************************************

  attrt_list.c - Strorage for a list of AttributeTypes

*****************************************************************************/

#include "attrt_list.h"

/*
 * - add_type_to_list() -
 */
QBool
add_type_to_list(type, list_ptr)
	AttributeType type;
	attrTypeList *list_ptr;
{
	attrTypeList curr_cell = *list_ptr;

	while (curr_cell != NULLAttrTypeList && curr_cell->type != type)
		curr_cell = curr_cell->next;

	if (curr_cell == NULLAttrTypeList) {
		curr_cell = at_cell_alloc();

		curr_cell->next = *list_ptr;
		curr_cell->type = type;

		*list_ptr = curr_cell;

		return TRUE;
	}

	return FALSE;
}				/* add_type_to_list */

/*
 * - delete_type_from_list() -
 */
QBool
delete_type_from_list(type, list_ptr)
	AttributeType type;
	attrTypeList *list_ptr;
{
	attrTypeList curr_cell = *list_ptr, last_cell = NULLAttrTypeList;

	while (curr_cell != NULLAttrTypeList && curr_cell->type != type) {
		last_cell = curr_cell;
		curr_cell = curr_cell->next;
	}

	if (curr_cell == NULLAttrTypeList)
		return FALSE;
	else {
		if (last_cell == NULLAttrTypeList)
			*list_ptr = curr_cell->next;
		else {
			last_cell->next = curr_cell->next;
			(void) free((char *) curr_cell);
		}

		return TRUE;
	}
}				/* delete_type_from_list */

/*
 * - free_at_list() -
 */
void
free_at_list(list_ptr)
	attrTypeList *list_ptr;
{
	attrTypeList curr_cell, next_cell;

	for (curr_cell = *list_ptr; curr_cell != NULLAttrTypeList; curr_cell = next_cell) {
		next_cell = curr_cell->next;

		(void) free((char *) curr_cell);
	}

	*list_ptr = NULLAttrTypeList;
}				/* free_at_list */
