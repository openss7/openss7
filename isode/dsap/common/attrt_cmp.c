#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/attrt_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/attrt_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: attrt_cmp.c,v $
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
#include "quipu/name.h"

#ifdef AttrT_cmp
#undef AttrT_cmp
#endif

AttrT_cmp_old (x,y)
register AttributeType x,y;
{
	if ( x == NULLAttrT )
		return (y ? -1 : 0);
	if ( y == NULLAttrT )
		return (1);

	if (x == y)
		return (0);

	return (x > y) ? 1 : -1;
}



