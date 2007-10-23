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

/* get_filter.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/get_filter.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/get_filter.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: get_filter.c,v
 * Revision 9.0  1992/06/16  12:35:39  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "quipu/util.h"
#include "quipu/ds_search.h"

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS opt, rps;

char *TidyString();

Filter
get_filter_aux(str)
	char *str;
{
	int gotit, bracketed;
	char ch, och = '\0';
	Filter result, next, ptr = NULLFILTER;

	result = filter_alloc();
	result->FUFILT = NULLFILTER;
	result->flt_next = NULLFILTER;

	str = TidyString(str);

	/* Got a multiple-component string for parsing */

	do {
		str = SkipSpace(str);
		bracketed = FALSE;
		if ((gotit = getop(str, &ch)) == -2)
			return (NULLFILTER);

		if (gotit < 0) {	/* Match an open bracket. */
			if (*str == '(')
				if (str[strlen(str) - 1] == ')') {
					str[strlen(str) - 1] = '\0';
					++str;
					bracketed = TRUE;
				} else {
					ps_print(OPT, "Too many open brackets\n");
					return (NULLFILTER);
				}

			if (och == '\0') {
				if (bracketed == TRUE) {
					gotit = 0;	/* Stop 'while' loop falling */
					continue;	/* Parse the internals */
				} else
					break;	/* Single item only */
			} else
				ch = och;	/* Use last operation */
		}
		if (och == '\0')	/* Remember last operation */
			och = ch;
		else if (och != ch) {
			ps_print(OPT, "Can't mix operations without using brackets!\n");
			return (NULLFILTER);
		}
		if (gotit >= 0)	/* If got an op, make it null */
			str[gotit] = '\0';
		/* Recurse on the 'first' string */
		if ((gotit != 0) && (*str != NULL)) {
			if ((next = get_filter_aux(str)) == NULLFILTER)
				return (NULLFILTER);
			if (ptr == NULLFILTER)
				ptr = next;
			else
				filter_append(ptr, next);
		}
		str += gotit + 1;

		if (gotit >= 0) {	/* Match an and symbol */
			if (och == '&') {
				result->flt_type = FILTER_AND;
			} else {	/* Match an or symbol */
				result->flt_type = FILTER_OR;
			}
			result->FUFILT = ptr;
		}
	}
	while (gotit >= 0);
	if (och == '\0') {
		if (*str == '!') {	/* Match a not symbol */
			result->flt_type = FILTER_NOT;
			if ((result->FUFILT = get_filter_aux(str + 1)) == NULLFILTER)
				return (NULLFILTER);
		} else if (*str != 0)
			if (filteritem(str, result) == NOTOK)
				return (NULLFILTER);
	}
	return (result);
}

Filter
get_filter(str)
	char *str;
{
	char *ptr;
	Filter f;

	if (*str == 0)
		return (NULLFILTER);

	ptr = strdup(str);
	f = get_filter_aux(ptr);
	free(ptr);
	return (f);

}

getop(str, ch)
	char *str, *ch;
{
	int i, bracket = 0;

	for (i = 0; i < (int) strlen(str); i++) {
		if (bracket == 0 && (str[i] == '&' || str[i] == '|')) {
			*ch = str[i];
			return (i);
		}
		if (str[i] == '(')
			++bracket;
		if (str[i] == ')')
			--bracket;
		if (bracket < 0) {
			ps_print(OPT, "Too many close brackets\n");
			return (-2);
		}
	}
	return (-1);
}
