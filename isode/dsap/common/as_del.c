#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/as_del.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/as_del.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: as_del.c,v $
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

extern LLog * log_dsap;

as_delnext (as)
Attr_Sequence  as;
{
Attr_Sequence ptr;

	if (as == NULLATTR)
		DLOG (log_dsap,LLOG_DEBUG,("delnext of null as!"));
	else    {
		ptr = as->attr_link;
		if (ptr == NULLATTR)
			DLOG (log_dsap,LLOG_DEBUG,("no new as to delete!"));
		else    {
			as->attr_link = ptr->attr_link;
			as_comp_free (ptr);
			}
		}
}

