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
    "Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/symtab.c,v 9.0 1992/06/16 12:45:08 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/symtab.c,v 9.0 1992/06/16 12:45:08 isode Rel
 *
 * Log: symtab.c,v
 * Revision 9.0  1992/06/16  12:45:08  isode
 * Release 8.0
 *
 */

#include "general.h"
#include "symtab.h"

put_symbol_value(table, name, val)
	table_entry table;
	char *name;
	char *val;
{
	if (!name)
		return;

	while (table && strcmp(name, table->name)) {
		table = table->next;
	}
	if (table) {
		free(table->val);
		if (val) {
			table->val = (char *) malloc((unsigned) strlen(val) + 1);
			(void) strcpy(table->val, val);
		} else
			table->val = (char *) 0;
	} else {
		table = (table_entry) malloc(sizeof(table_entry));
		table->next = NULLSYM;
		table->name = (char *) malloc((unsigned) strlen(name) + 1);
		(void) strcpy(table->name, name);
		if (val) {
			table->val = (char *) malloc((unsigned) strlen(val) + 1);
			(void) strcpy(table->val, val);
		} else
			table->val = 0;
	}
}

char *
get_symbol_value(table, name)
	table_entry table;
	char *name;
{
	while (table && strcmp(name, table->name))
		table = table->next;
	if (table)
		return table->val;
	return (char *) 0;
}

free_table(table)
	table_entry table;
{
	table_entry entry;

	while (table) {
		if (table->val)
			free(table->val);
		free(table->name);
		entry = table;
		table = table->next;
		free((char *) entry);
	}
}
