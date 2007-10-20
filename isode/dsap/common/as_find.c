#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/as_find.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/as_find.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: as_find.c,v $
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
#include "quipu/attrvalue.h"

Attr_Sequence as_find_type (a,b)
Attr_Sequence a;
AttributeType b;
{
register int i;
register Attr_Sequence ptr;
	/* if Attr_cmp returns <0 no point in continuing due to ordering */

	for(ptr = a; ptr != NULLATTR; ptr=ptr->attr_link) {
		if (  (i = AttrT_cmp (ptr->attr_type,b)) <= 0)
			return (i ? NULLATTR : ptr);
	}
	return (NULLATTR);
}


