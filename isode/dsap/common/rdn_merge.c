#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/rdn_merge.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/rdn_merge.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: rdn_merge.c,v $
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

RDN rdn_merge (a,b)
RDN a,b;
{
register RDN aptr, bptr, result, trail;

	if ( a == NULLRDN )
		return (b);
	if ( b == NULLRDN )
		return (a);

	/* start sequence off, make sure 'a' is the first */
	switch (rdn_cmp_comp (a,b)) {
		case 0: /* equal */
			result = a;
			aptr = a->rdn_next;
			rdn_comp_free (b);
			bptr = b->rdn_next;
			break;
		case -1:
			result = b;
			aptr = a;
			bptr = b->rdn_next;
			break;
		case 1:
			result = a;
			aptr = a->rdn_next;
			bptr = b;
			break;
		}

	trail = result;
	while (  (aptr != NULLRDN) && (bptr != NULLRDN) ) {

	   switch (rdn_cmp_comp (aptr,bptr)) {
		case 0: /* equal */
			trail->rdn_next = aptr;
			trail = aptr;
			rdn_comp_free (bptr);
			aptr = aptr->rdn_next;
			bptr = bptr->rdn_next;
			break;
		case -1:
			trail->rdn_next = bptr;
			trail = bptr;
			bptr = bptr->rdn_next;
			break;
		case 1:
			trail->rdn_next = aptr;
			trail = aptr;
			aptr = aptr->rdn_next;
			break;
	    }
	}
	if (aptr == NULLRDN)
		trail->rdn_next = bptr;
	else
		trail->rdn_next = aptr;

	return (result);
}

