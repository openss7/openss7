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
    "Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/dn_list.c,v 9.0 1992/06/16 12:45:27 isode Rel";
#endif

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/dn_list.c,v 9.0 1992/06/16 12:45:27 isode Rel */

/*
 * Log: dn_list.c,v
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  dn_list.c - Implements a list of sorted entry names.

*****************************************************************************/

#include <stdio.h>
#include <ctype.h>

#include "quipu/util.h"

#include "types.h"
#include "util.h"
#include "dn_list.h"

/*
 * - dn_list_insert -
 * Insert a new entry name into an unsorted listed of entry names.
 *
 */
QBool
dn_list_insert(dn, entry_list_ptr, object_type)
	char *dn;
	entryList *entry_list_ptr;
	AttributeType object_type;
{
	entryList element, new_element;
	register char *sort_key;
	char *entry_name;

	element = *entry_list_ptr;

	new_element = entry_list_alloc();
	new_element->string_dn = entry_name = copy_string(dn);
	new_element->object_class = object_type;

	/* Sort key is at present the attr value of the RDN. ####### Need to do more for 'cn'
	   attributes. ####### */
	sort_key = entry_name;

	while (*sort_key != '\0')
		sort_key++;
	while (*sort_key != '=')
		sort_key--;

	sort_key++;

	while (isspace(*sort_key))
		sort_key++;

	new_element->sort_key = sort_key;

	new_element->next = element;
	*entry_list_ptr = new_element;

	return TRUE;
}				/* dn_list_insert */

/*
 * - dn_list_add -
 * Insert a new entry name into a sorted listed of entry names.
 *
 */
QBool
dn_list_add(dn, entry_list_ptr, object_type)
	char *dn;
	entryList *entry_list_ptr;
	AttributeType object_type;
{
	entryList element, new_element;
	register char *sort_key;
	char *entry_name;
	int lexcmp;

	element = *entry_list_ptr;

	new_element = entry_list_alloc();
	new_element->string_dn = entry_name = copy_string(dn);
	new_element->object_class = object_type;

	/* Sort key is at present the attr value of the RDN. ####### Need to do more for 'cn'
	   attributes. ####### */
	sort_key = entry_name;

	while (*sort_key != '\0')
		sort_key++;

	while (*sort_key != '=')
		sort_key--;

	sort_key++;
	while (isspace(*sort_key))
		sort_key++;

	new_element->sort_key = sort_key;

	/* If given list is empty, return new_element. */
	if (element == NULLEntryList) {
		new_element->next = NULLEntryList;
		*entry_list_ptr = new_element;

		return TRUE;
	}

	/* Check if alphabetically lower than first in list. */
	lexcmp = lexequ(sort_key, element->sort_key);

	if (lexcmp < 0) {
		new_element->next = element;
		*entry_list_ptr = new_element;
	} else if (lexcmp == 0) {	/* Don't want the same dn twice */
		(void) free(entry_name);
		(void) free((char *) new_element);

		return FALSE;
	} else {
		/* Insert new entry_list into alphabetical position. */
		while (element->next != NULLEntryList) {
			lexcmp = lexequ(sort_key, element->next->sort_key);

			if (lexcmp < 0)
				break;
			else if (lexcmp == 0) {
				(void) free(entry_name);
				(void) free((char *) new_element);

				return FALSE;
			} else
				element = element->next;
		}

		new_element->next = element->next;
		element->next = new_element;
	}

	return TRUE;
}				/* dn_list_add */

/*
 * - dn_list_free() -
 * Free given entry list.
 *
 */
void
dn_list_free(entry_list_ptr)
	entryList *entry_list_ptr;
{
	entryList next_entry, list = *entry_list_ptr;

	while (list != NULLEntryList) {
		next_entry = list->next;
		(void) free(list->string_dn);

		/* Don't have to free sort key as it is a pointer into above string. */
		(void) free((char *) list);
		list = next_entry;
	}

	*entry_list_ptr = NULLEntryList;
}				/* dn_list_free */

/*
 * - dn_list_copy() -
 *
 *
 */
void
dn_list_copy(original, copy)
	entryList original, *copy;
{
	entryList copylist = NULLEntryList;
	int sort_key_diff = 0;

	for (*copy = NULLEntryList; original != NULLEntryList; original = original->next) {
		if (copylist == NULLEntryList)
			copylist = *copy = entry_list_alloc();
		else
			copylist = copylist->next = entry_list_alloc();

		copylist->next = NULLEntryList;

		/* sort_key is a pointer into string_dn */
		sort_key_diff = (int) (original->sort_key - original->string_dn);

		copylist->sort_key = copylist->string_dn = copy_string(original->string_dn);

		copylist->sort_key = (char *) (sort_key_diff + copylist->sort_key);
		copylist->object_class = original->object_class;
	}
}				/* dn_list_copy */
