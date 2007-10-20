#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/avs_del.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/avs_del.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: avs_del.c,v $
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

avs_delnext (avs)
AV_Sequence  avs;
{
AV_Sequence ptr;
	if (avs == NULLAV)
		DLOG (log_dsap,LLOG_DEBUG,("delnext of null avs!"));
	else    {
		ptr = avs->avseq_next;
		if (ptr == NULLAV)
			DLOG (log_dsap,LLOG_DEBUG,("no new avs to delete!"));
		else    {
			avs->avseq_next = ptr->avseq_next;
			avs_comp_free (ptr);
			}
		}
}

