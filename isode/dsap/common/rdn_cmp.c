#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/rdn_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/rdn_cmp.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: rdn_cmp.c,v $
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
#include "quipu/dsp.h"
#include "quipu/ds_error.h"

rdn_cmp_comp (a,b)
RDN  a,b;
{
register int j;

	if (a->rdn_at != b->rdn_at) 
		return ((a->rdn_at > b->rdn_at) ? 1 : -1);

	j = AttrV_cmp (&a->rdn_av,&b->rdn_av);

	return (j);

}

