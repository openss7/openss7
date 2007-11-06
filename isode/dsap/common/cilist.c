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

/* cilist.c - Case Ignore List */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/cilist.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/cilist.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: cilist.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

/*
	SYNTAX:
		list = <list_component> | <list_component> '$' <list>
		list_component = [ '{T61}' ] <string>

	EXAMPLE:
		An example $ of a case ignore list $ syntax attribute
*/

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "quipu/syntaxes.h"

static
cilistfree(cilist)
	struct CIList *cilist;
{
	struct CIList *next;

	for (; cilist != NULLCILIST; cilist = next) {
		next = cilist->l_next;
		free(cilist->l_str);
		free((char *) cilist);
	}
}

static
cilistcmp(a, b)
	struct CIList *a, *b;
{
	int res;

	for (; (a != NULLCILIST) && (b != NULLCILIST); a = a->l_next, b = b->l_next)
		if ((res = lexequ(a->l_str, b->l_str)) != 0)
			return (res);

	if (a != b)
		return (a > b ? 1 : -1);
	else
		return (0);

}

static struct CIList *
cilistcpy(a)
	struct CIList *a;
{
	struct CIList *b, *c, *result = NULLCILIST;

	c = result;		/* to keep lint quiet ! */

	for (; a != NULLCILIST; a = a->l_next) {
		b = (struct CIList *) smalloc(sizeof(struct CIList));
		b->l_type = a->l_type;
		b->l_str = strdup(a->l_str);

		if (result == NULLCILIST)
			result = b;
		else
			c->l_next = b;
		c = b;
	}

	b->l_next = NULLCILIST;
	return (result);
}

static struct CIList *
cilistparse(str)
	char *str;
{
	struct CIList *result = NULLCILIST;
	struct CIList *a, *b;
	char *ptr;
	char *mark = NULLCP;
	char t61_str = FALSE;
	extern char t61_flag;
	char *octparse();
	char *prtparse();

	b = result;		/* to keep lint quiet */

	if (t61_flag) {
		t61_str = TRUE;
		t61_flag = FALSE;	/* indicate recognition */
	}

	ptr = str = SkipSpace(str);

	while (ptr) {
		mark = NULLCP;
		a = (struct CIList *) smalloc(sizeof(struct CIList));

		if ((ptr = index(str, '$')) != NULLCP) {
			*ptr-- = 0;
			if (isspace(*ptr)) {
				*ptr = 0;
				mark = ptr;
			}
			ptr++;
		}

		if (*str == 0) {
			parse_error("Null string not allowed", NULLCP);
			return NULLCILIST;
		}

		if ((t61_str) || (!check_print_string(str))) {
			a->l_type = 1;
			if ((a->l_str = octparse(str)) == NULLCP)
				return (NULLCILIST);
		} else {
			a->l_str = strdup(str);
			a->l_type = 2;
		}

		if (result == NULLCILIST)
			result = a;
		else
			b->l_next = a;
		b = a;

		t61_str = FALSE;

		if (ptr != NULLCP) {
			*ptr++ = '$';
			if (mark != NULLCP)
				*mark = ' ';
			str = (SkipSpace(ptr));
			ptr = str;

			if (*ptr++ == '{') {
				if ((str = index(ptr, '}')) == 0) {
					parse_error("close bracket missing '%s'", --ptr);
					return (NULLCILIST);
				}
				*str = 0;
				if (lexequ("T.61", ptr) != 0) {
					*str = '}';
					parse_error("{T.61} expected '%s'", --ptr);
					return (NULLCILIST);
				}
				*str++ = '}';
				str = (SkipSpace(str));
				t61_str = TRUE;
			}
		}
	}

	a->l_next = NULLCILIST;

	return (result);
}

static
cilistprint(ps, cilist, format)
	PS ps;
	struct CIList *cilist;
	int format;
{
	char *prefix = NULLCP;

	for (; cilist != NULLCILIST; cilist = cilist->l_next) {
		if (prefix != NULLCP)
			ps_print(ps, prefix);

		if (cilist->l_type == 1) {
			if (format != READOUT)
				ps_print(ps, "{T.61}");
			octprint(ps, cilist->l_str, format);
		} else
			ps_print(ps, cilist->l_str);

		if (format == READOUT)
			prefix = "\n\t\t\t";
		else
			prefix = " $\\\n\t";
	}
}

static PE
cilistenc(m)
	struct CIList *m;
{
	PE ret_pe;

	(void) encode_SA_CaseIgnoreList(&ret_pe, 0, 0, NULLCP, m);

	return (ret_pe);
}

static struct CIList *
cilistdec(pe)
	PE pe;
{
	struct CIList *m;

	if (decode_SA_CaseIgnoreList(pe, 1, NULLIP, NULLVP, &m) == NOTOK)
		return (NULLCILIST);
	return (m);
}

cilist_syntax()
{
	(void) add_attribute_syntax("CaseIgnoreList",
				    (IFP) cilistenc, (IFP) cilistdec,
				    (IFP) cilistparse, cilistprint,
				    (IFP) cilistcpy, cilistcmp, cilistfree, NULLCP, NULLIFP, TRUE);

}
