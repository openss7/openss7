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

/* filteritem.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/filteritem.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/filteritem.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: filteritem.c,v
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

/* Regular Expression parser written by P.Sharpe */
/* QUIPU specific tailoring by CJR */

#define debug(a,b)		/* remove debug statements */

Filter get_filter();
char *TidyString();

filteritem(str, fltr)
	char *str;
	Filter fltr;
{

	char *ptr;
	AttributeValue av;
	AttributeType at;

	fltr->flt_type = FILTER_ITEM;

	if ((ptr = index(str, '=')) == NULLCP) {
		/* set default (cn~=) */
		fltr->FUITEM.fi_type = FILTERITEM_APPROX;
		at = AttrT_new(CN_OID);
	} else {
		switch (*--ptr) {
		case '~':
			fltr->FUITEM.fi_type = FILTERITEM_APPROX;
			*ptr = 0;
			break;
		case '>':
			fltr->FUITEM.fi_type = FILTERITEM_GREATEROREQUAL;
			*ptr = 0;
			break;
		case '<':
			fltr->FUITEM.fi_type = FILTERITEM_LESSOREQUAL;
			*ptr = 0;
			break;
		default:
			fltr->FUITEM.fi_type = FILTERITEM_EQUALITY;
			break;
		}
		*++ptr = '\0';
		str = TidyString(str);
		at = AttrT_new(str);
		if (at == NULLAttrT) {
			ps_printf(OPT, "invalid attribute type (%s)\n", str);
			return (NOTOK);
		}
		str = ptr + 1;
	}

	if ((*str == '*') || (*str == 0)) {
		if (*++str == 0) {
			fltr->FUITEM.fi_type = FILTERITEM_PRESENT;
			fltr->FUITEM.UNTYPE = at;
			return (OK);
		} else
			str--;
	}

	/* test for whether there is only the simple 'equality' case */
	if ((ptr = index(str, '*')) == NULLCP) {
		debug(1, ("[EXACT(%s)]", str));

		fltr->FUITEM.UNAVA.ava_type = at;
		str = TidyString(str);
		if ((fltr->FUITEM.UNAVA.ava_value = str2AttrV(str, at->oa_syntax)) == NULLAttrV)
			return (NOTOK);
		return (OK);
	}

	/* 
	 * We have to parse the string for 'initial', 'final' and 'any'
	 * components 
	 */

	fltr->FUITEM.UNSUB.fi_sub_initial = NULLAV;
	fltr->FUITEM.UNSUB.fi_sub_any = NULLAV;
	fltr->FUITEM.UNSUB.fi_sub_final = NULLAV;
	fltr->FUITEM.UNSUB.fi_sub_type = at;
	fltr->FUITEM.fi_type = FILTERITEM_SUBSTRINGS;
	if (!sub_string(at->oa_syntax)) {
		ps_print(OPT, "Can only substring search on strings\n");
		return (NOTOK);
	}

	debug(1, ("[ "));
	/* This is the 'initial' section of the string - maybe NULL */
	*ptr = '\0';
	str = TidyString(str);
	if (*str != 0) {
		debug(1, ("INITIAL(%s) ", str));
		if ((av = str2AttrV(str, at->oa_syntax)) == NULLAttrV)
			return NOTOK;
		fltr->FUITEM.UNSUB.fi_sub_initial = avs_comp_new(av);
	}
	str = ptr + 1;

	/* Test for whether there are going to be any 'any' bits */
	if ((ptr = rindex(str, '*')) == NULLCP) {
		ptr = TidyString(str);
		if (*str != 0) {
			debug(1, ("FINAL(%s) ", str));
			if ((av = str2AttrV(str, at->oa_syntax)) == NULLAttrV)
				return NOTOK;
			fltr->FUITEM.UNSUB.fi_sub_final = avs_comp_new(av);
		}
		debug(1, ("]"));
		return (OK);
	}
	*ptr = '\0';
	ptr = TidyString(ptr + 1);
	if (*ptr != 0) {
		debug(1, ("FINAL(%s) ", ptr));
		if ((av = str2AttrV(ptr, at->oa_syntax)) == NULLAttrV)
			return NOTOK;

		fltr->FUITEM.UNSUB.fi_sub_final = avs_comp_new(av);
	}
	/* There are some internal 'any's to be found */
	do {
		AV_Sequence any_end;

		if ((ptr = index(str, '*')) != NULLCP)
			*ptr = '\0';

		if (*str != 0) {
			str = TidyString(str);
			debug(1, ("ANY(%s) ", str));

			if (*str == 0)
				av = str2AttrV("\\20", at->oa_syntax);
			else
				av = str2AttrV(str, at->oa_syntax);

			if (av == NULLAttrV)
				return NOTOK;

			if (fltr->FUITEM.UNSUB.fi_sub_any == NULLAV) {
				fltr->FUITEM.UNSUB.fi_sub_any = avs_comp_new(av);
				any_end = fltr->FUITEM.UNSUB.fi_sub_any;
			} else {
				any_end->avseq_next = avs_comp_new(av);
				any_end = any_end->avseq_next;
			}
		}
		if (ptr != NULLCP)
			str = ptr + 1;
	}

	while (ptr != NULLCP);
	debug(1, ("]"));
	return (OK);
}
