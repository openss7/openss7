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
    "Header: /xtel/isode/isode/dsap/common/RCS/as_str.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/as_str.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: as_str.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/malloc.h"

extern short acl_sntx;
extern IFP merge_acl;
extern IFP acl_fn;
extern char dsa_mode;

Attr_Sequence
str2as(str)
	register char *str;
{
	register char *ptr;
	char *save, val;
	AttributeType at;
	Attr_Sequence as;

	if (str == NULLCP)
		return (NULLATTR);

	if ((ptr = index(str, '=')) == 0) {
		parse_error("equals missing in '%s'", str);
		return (NULLATTR);
	}

	save = ptr++;
	if (!isspace(*--save))
		save++;

	ptr = SkipSpace(ptr);
	if (*ptr == 0)
		return (NULLATTR);

	val = *save;
	*save = 0;

	if ((at = AttrT_new(str)) == NULLAttrT) {
		parse_error("unknown attribute type '%s'", str);
		*ptr = '=';
		return (NULLATTR);
	}

	*save = val;

	as = as_comp_alloc();
	as->attr_acl = NULLACL_INFO;
	as->attr_type = at;
	as->attr_link = NULLATTR;

	ATTRIBUTE_HEAP;

	if ((as->attr_value = str2avs(ptr, as->attr_type)) == NULLAV) {
		RESTORE_HEAP;
		as_free(as);
		return (NULLATTR);
	}

	RESTORE_HEAP;

	return (as);

}

Attr_Sequence
as_combine(as, str, allownull)
	Attr_Sequence as;
	register char *str;
	char allownull;
{
	register char *ptr = str;
	char *save, val;
	AV_Sequence avs;
	Attr_Sequence as2, nas;
	AttributeType at;
	int i;
	static Attr_Sequence fast_as = NULLATTR;
	static Attr_Sequence fast_tail = NULLATTR;
	extern Attr_Sequence as_fast_merge();
	extern AV_Sequence avs_fast_merge();
	extern AV_Sequence fast_str2avs();
	AV_Sequence fast_avs = NULLAV;
	AV_Sequence fast_avstail = NULLAV;

	if (str == NULLCP)
		return (as);

	while (*ptr != 0)
		if (*ptr == '=')
			break;
		else
			ptr++;

	if (*ptr == 0) {
		if (allownull) {
			nas = as_comp_alloc();
			nas->attr_acl = NULLACL_INFO;
			if ((nas->attr_type = AttrT_new(str)) == NULLAttrT) {
				parse_error("unknown attribute type '%s'", str);
				return as;
			}
			nas->attr_link = NULLATTR;
			nas->attr_value = NULLAV;
			as = as_fast_merge(as, nas, fast_as, fast_tail);
			fast_as = as, fast_tail = nas;
			return as;
		}
		parse_error("equals missing in '%s'", str);
		return (as);
	}

	save = ptr++;
	if (!isspace(*--save))
		save++;

	val = *save;
	*save = 0;

	ptr = SkipSpace(ptr);

	if ((at = AttrT_new(str)) == NULLAttrT) {
		parse_error("unknown attribute type '%s'", str);
		*ptr = '=';
		return (as);
	}

	if (*ptr == 0) {
		if ((at->oa_syntax == acl_sntx) && dsa_mode) {
			/* Add default ACL */
			struct acl *acl = (struct acl *) NULL;

			acl = acl_alloc();
			acl->ac_child = (struct acl_info *) (*acl_fn) ();
			acl->ac_entry = (struct acl_info *) (*acl_fn) ();
			acl->ac_default = (struct acl_info *) (*acl_fn) ();
			acl->ac_attributes = NULLACL_ATTR;

			nas = as_comp_alloc();
			nas->attr_acl = NULLACL_INFO;
			nas->attr_link = NULLATTR;
			nas->attr_value = avs_comp_alloc();
			nas->attr_value->avseq_next = NULLAV;
			nas->attr_value->avseq_av.av_syntax = acl_sntx;
			nas->attr_value->avseq_av.av_struct = (caddr_t) acl;
			nas->attr_type = at;
			as = as_fast_merge(as, nas, fast_as, fast_tail);
			fast_as = as, fast_tail = nas;
		} else if ((allownull) || ((at->oa_syntax == acl_sntx) && !dsa_mode)) {
			nas = as_comp_alloc();
			nas->attr_acl = NULLACL_INFO;
			nas->attr_link = NULLATTR;
			nas->attr_value = NULLAV;
			nas->attr_type = at;
			as = as_fast_merge(as, nas, fast_as, fast_tail);
			fast_as = as, fast_tail = nas;
		}
		return (as);
	}

	for (as2 = as; as2 != NULLATTR; as2 = as2->attr_link) {
		if ((i = AttrT_cmp(at, as2->attr_type)) == 0) {
			*save = val;

			ATTRIBUTE_HEAP;

			if (at->oa_syntax == acl_sntx) {
				(*merge_acl) (as2->attr_value, SkipSpace(ptr));
				RESTORE_HEAP;
				return (as);
			}

			if ((avs = fast_str2avs(ptr, as2->attr_type)) == NULLAV) {
				RESTORE_HEAP;
				return (as);
			}

			as2->attr_value = avs_fast_merge(as2->attr_value, avs,
							 fast_avs, fast_avstail);
			fast_avs = as2->attr_value;
			fast_avstail = avs;

			RESTORE_HEAP;

			return (as);
		} else if (i > 0)
			break;
	}

	*save = val;

	nas = as_comp_alloc();
	nas->attr_acl = NULLACL_INFO;
	nas->attr_type = at;
	nas->attr_link = NULLATTR;

	ATTRIBUTE_HEAP;

	if ((nas->attr_value = fast_str2avs(ptr, nas->attr_type)) == NULLAV) {
		RESTORE_HEAP;
		as_free(nas);
		return (as);
	}

	RESTORE_HEAP;

	as = as_fast_merge(as, nas, fast_as, fast_tail);
	fast_as = as, fast_tail = nas;
	return as;

}
