#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/avs_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/avs_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: avs_free.c,v $
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

avs_comp_free (avs)
AV_Sequence avs;
{
	AttrV_free_aux (&avs->avseq_av);
	free ((char *) avs);
}

avs_free (avs)
AV_Sequence avs;
{
register AV_Sequence eptr;
register AV_Sequence next;

	for(eptr = avs; eptr != NULLAV; eptr = next) {
		next = eptr->avseq_next;
		avs_comp_free (eptr);
	}
}
