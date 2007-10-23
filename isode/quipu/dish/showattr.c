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

/* showattr.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/showattr.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/showattr.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: showattr.c,v
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
#include "quipu/attrvalue.h"
#include "quipu/entry.h"

extern Entry current_entry;
extern DN current_dn;
extern DN dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS rps, opt;

extern char value_flag;
extern char key_flag;
extern char print_format;
extern char show_all_flag;

static Attr_Sequence ignore_attr = NULLATTR;
static char ignore_unknown = FALSE;

showattribute(at)
	AttributeType at;
{
	Attr_Sequence eptr;

	if (!show_all_flag) {
		if (!check_want_attr(at))
			return;
	} else if (!check_want_tmp_attr(at))
		return;

	for (eptr = current_entry->e_attributes; eptr != NULLATTR; eptr = eptr->attr_link) {
		/* Tiptoe through the list of types until one matches, and then print value. */
		if (AttrT_cmp(eptr->attr_type, at) == 0) {
			if (value_flag)
				if (key_flag)
					as_comp_print(RPS, eptr, print_format);
				else
					avs_print(RPS, eptr->attr_value, print_format);
			else {
				AttrT_print(RPS, at, print_format);
				ps_print(RPS, "\n");
			}
			break;
		}
	}

	if (eptr == NULLATTR)
		if (key_flag)
			ps_printf(OPT, "%-21s - (No such attribute in this entry)\n",
				  at->oa_ot.ot_name);
		else
			ps_printf(OPT, "No value\n");

}

show_unknown()
{
	ignore_unknown = TRUE;
}

check_want_attr(at)
	AttributeType at;
{
	Attr_Sequence as;

	if (at == NULLTABLE_ATTR)
		return (ignore_unknown);

	if (at->oa_syntax == 0)
		return (ignore_unknown);

	for (as = ignore_attr; as != NULLATTR; as = as->attr_link)
		if (AttrT_cmp(at, as->attr_type) == 0)
			return (FALSE);

	return (check_want_tmp_attr(at));
}

check_want_tmp_attr(at)
	AttributeType at;
{
	Attr_Sequence as;
	Attr_Sequence as2;
	extern Attr_Sequence tmp_ignore;
	extern Attr_Sequence as_flag;

	if (at->oa_syntax == 0)
		return (ignore_unknown || show_all_flag);

	for (as = tmp_ignore; as != NULLATTR; as = as->attr_link)
		if (AttrT_cmp(at, as->attr_type) == 0) {
			/* may be explicitly wanted... */
			for (as2 = as_flag; as2 != NULLATTR; as2 = as2->attr_link)
				if (AttrT_cmp(as2->attr_type, as->attr_type) == 0)
					return (TRUE);
			return (FALSE);
		}

	return (TRUE);
}

new_ignore(ptr)
	char *ptr;
{
	AttributeType at;
	Attr_Sequence newas;

	if ((at = str2AttrT(ptr)) == NULLAttrT)
		return;
	newas = as_comp_new(at, NULLAV, NULLACL_INFO);
	ignore_attr = as_merge(ignore_attr, newas);
}
