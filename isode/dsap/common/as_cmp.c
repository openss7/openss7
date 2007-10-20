#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/as_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/as_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: as_cmp.c,v $
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

as_cmp_comp (a,b)
Attr_Sequence  a,b;
{
int i;
	if (( i= AttrT_cmp (a->attr_type,b->attr_type)) != 0)
		return (i);

	return (avs_cmp (a->attr_value,b->attr_value));
}

as_cmp (a,b)
Attr_Sequence  a,b;
{
int i;
	for (; (a != NULLATTR) && (b != NULLATTR) ; a = a->attr_link, b = b->attr_link)
		if ( (i =as_cmp_comp (a,b)) != 0)
			return (i);

	if ( (a == NULLATTR) && (b == NULLATTR) )
		return (0);
	else
		return (a ? 1  : -1);

}

