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

/* template.c - your comments here */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/de/RCS/filter.c,v 9.0 1992/06/16 12:45:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/de/RCS/filter.c,v 9.0 1992/06/16 12:45:59 isode Rel
 *
 *
 * Log: filter.c,v
 * Revision 9.0  1992/06/16  12:45:59  isode
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
#include "util.h"
#include "filter.h"

struct ds_search_arg sarg;
struct ds_search_result sresult;
struct DSError serror;

struct s_filter *
andfilter()
{
	struct s_filter *fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_AND;
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
orfilter()
{
	struct s_filter *fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_OR;
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
eqfilter(matchtype, type, value)
	int matchtype;
	char *type, *value;
{
	struct s_filter *fp;
	AttributeType at;

	fp = filter_alloc();
	fp->flt_type = FILTER_ITEM;
	fp->FUITEM.fi_type = matchtype;
	at = fp->FUITEM.fi_un.fi_un_ava.ava_type = str2AttrT(type);
	fp->FUITEM.fi_un.fi_un_ava.ava_value = str2AttrV(value, at->oa_syntax);
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
subsfilter(substrtype, type, value)
	int substrtype;
	char *type, *value;
{
	struct s_filter *fp;
	AttributeType at;

	fp = filter_alloc();
	fp->flt_type = FILTER_ITEM;
	fp->FUITEM.fi_type = FILTERITEM_SUBSTRINGS;
	at = fp->FUITEM.fi_un.fi_un_ava.ava_type = str2AttrT(type);
	switch (substrtype) {
	case LEADSUBSTR:
		fp->FUITEM.UNSUB.fi_sub_initial = avs_comp_new(str2AttrV(value, at->oa_syntax));
		fp->FUITEM.UNSUB.fi_sub_any = NULLAV;
		fp->FUITEM.UNSUB.fi_sub_final = NULLAV;
		break;
	case TRAILSUBSTR:
		fp->FUITEM.UNSUB.fi_sub_final = avs_comp_new(str2AttrV(value, at->oa_syntax));
		fp->FUITEM.UNSUB.fi_sub_any = NULLAV;
		fp->FUITEM.UNSUB.fi_sub_initial = NULLAV;
		break;
	case ANYSUBSTR:
		fp->FUITEM.UNSUB.fi_sub_any = avs_comp_new(str2AttrV(value, at->oa_syntax));
		fp->FUITEM.UNSUB.fi_sub_initial = NULLAV;
		fp->FUITEM.UNSUB.fi_sub_final = NULLAV;
		break;
	}
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
presfilter(type)
	char *type;
{
	struct s_filter *fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_ITEM;
	fp->FUITEM.fi_type = FILTERITEM_PRESENT;
	fp->FUITEM.UNTYPE = str2AttrT(type);
	fp->flt_next = NULLFILTER;
	return fp;
}
