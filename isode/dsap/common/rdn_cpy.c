#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/rdn_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/rdn_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: rdn_cpy.c,v $
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
#include "quipu/malloc.h"

extern LLog * log_dsap;

RDN  rdn_comp_cpy (rdn)
RDN  rdn;
{
register RDN ptr;
unsigned last_heap;

	if (rdn==NULLRDN) {
		return (NULLRDN);
	}

	ptr = rdn_comp_alloc();
	ptr->rdn_at = AttrT_cpy (rdn->rdn_at);

	if ((last_heap = mem_heap) == 1)
		mem_heap = 2 + attr_index;

	AttrV_cpy_aux  (&rdn->rdn_av,&ptr->rdn_av);
	ptr->rdn_next = NULLRDN;

	mem_heap = last_heap;

	return (ptr);
}

RDN  rdn_cpy (rdn)
RDN  rdn;
{
RDN start;
register RDN eptr;
register RDN ptr,ptr2;

	if (rdn == NULLRDN) {
		return (NULLRDN);
	}
	start = rdn_comp_cpy (rdn);
	ptr2 = start;

	for (eptr=rdn->rdn_next; eptr!=NULLRDN; eptr=eptr->rdn_next) {
		ptr = rdn_comp_cpy (eptr);
		ptr2->rdn_next = ptr;
		ptr2 = ptr;
	}
	return (start);
}

