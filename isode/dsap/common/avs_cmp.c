#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/avs_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/avs_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: avs_cmp.c,v $
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
#include "quipu/dsp.h"        /* for ds_error.h */
#include "quipu/ds_error.h"

extern short acl_sntx;

avs_cmp (a,b)
AV_Sequence  a,b;
{
int i;

	if ( (a == NULLAV) || (b == NULLAV) )   {
		/* @#$%ing access control ! */
		if ( a == b )
			return 0;
		if (a && (a->avseq_av.av_syntax == acl_sntx))
			return acl_cmp ((struct acl *)a->avseq_av.av_struct,
					(struct acl *)NULL);
		if (b && (b->avseq_av.av_syntax == acl_sntx))
			return acl_cmp ((struct acl *)NULL,
					(struct acl *)b->avseq_av.av_struct);
					
		return (a ?  1 : -1);
	}

	for (; (a != NULLAV) && (b != NULLAV) ; a = a->avseq_next, b = b->avseq_next)
		if ( (i = avs_cmp_comp (a,b))  != 0) 
			return (i);

	if ( (a == NULLAV) && (b == NULLAV) )   {
		return 0;
	} else {
		return (a ?  1 : -1);
		}
}
