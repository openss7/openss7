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

/* tree_struct.c - Tree Structure utility routines */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/tree_struct.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/tree_struct.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: tree_struct.c,v
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

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"

extern int oidformat;

/* ARGSUSED */
tree_struct_free(ptr)
	struct tree_struct *ptr;
{
	/* don't free objectclass - in static table */
	free((char *) ptr);
}

static struct tree_struct *
tree_struct_cpy(a)
	struct tree_struct *a;
{
	struct tree_struct *result;

	result = tree_struct_alloc();
	result->tree_object = a->tree_object;
	return (result);
}

static
tree_struct_cmp(a, b)
	struct tree_struct *a, *b;
{
	if (a == NULLTREE)
		return (b == NULLTREE ? 0 : -1);

	if (b == NULLTREE)
		return (1);

	return (objclass_cmp(a->tree_object, b->tree_object));
}

/* ARGSUSED */
static
tree_struct_print(ps, tree, format)
	register PS ps;
	struct tree_struct *tree;
	int format;
{
	ps_printf(ps, "%s", oc2name(tree->tree_object, oidformat));
}

static struct tree_struct *
str2schema(str)
	char *str;
{
	struct tree_struct *ts;
	objectclass *str2oc();

	ts = tree_struct_alloc();
	if ((ts->tree_object = str2oc(str)) == NULLOBJECTCLASS) {
		parse_error("invalid oid in schema '%s'", str);
		free((char *) ts);
		return (NULLTREE);
	}
	return (ts);
}

static PE
ts_enc(ts)
	struct tree_struct *ts;
{
	PE ret_pe;

	(void) encode_Quipu_TreeStructureSyntax(&ret_pe, 0, 0, NULLCP, ts);

	return (ret_pe);
}

static struct tree_struct *
ts_dec(pe)
	PE pe;
{
	struct tree_struct *ts;

	if (decode_Quipu_TreeStructureSyntax(pe, 1, NULLIP, NULLVP, &ts) == NOTOK)
		return (struct tree_struct *) NULL;

	return (ts);
}

schema_syntax()
{
	(void) add_attribute_syntax("schema",
				    (IFP) ts_enc, (IFP) ts_dec,
				    (IFP) str2schema, tree_struct_print,
				    (IFP) tree_struct_cpy, tree_struct_cmp,
				    tree_struct_free, NULLCP, NULLIFP, FALSE);
}
