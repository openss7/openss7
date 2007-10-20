#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/as_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/as_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: as_free.c,v $
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

as_comp_free (as)
Attr_Sequence as;
{
	avs_free (as->attr_value);
	free ((char *) as);
}

as_free (as)
Attr_Sequence as;
{
register Attr_Sequence eptr;
register Attr_Sequence next;

	for(eptr = as; eptr != NULLATTR; eptr = next) {
		next = eptr->attr_link;
		as_comp_free (eptr);
	}
}
