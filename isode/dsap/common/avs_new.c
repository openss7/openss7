#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/avs_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/avs_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: avs_new.c,v $
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

avs_comp_fill (ptr,pe)
AV_Sequence ptr;
AttributeValue pe;
{
	ptr->avseq_av.av_struct = pe->av_struct;
	ptr->avseq_av.av_syntax = pe->av_syntax;
}

AV_Sequence  avs_comp_new (pe)
AttributeValue pe;
{
AV_Sequence ptr;
	ptr = avs_comp_alloc ();
	avs_comp_fill (ptr,pe);
	free ((char *)pe);
	ptr->avseq_next = NULLAV;
	return (ptr);
}
