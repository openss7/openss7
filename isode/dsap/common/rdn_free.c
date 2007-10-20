#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/rdn_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/rdn_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: rdn_free.c,v $
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

rdn_comp_free (rdn)
RDN rdn;
{
	AttrV_free_aux (&rdn->rdn_av);
	free ((char *) rdn);
}

rdn_free (rdn)
RDN rdn;
{
register RDN eptr;
register RDN next;

	for (eptr=rdn; eptr!=NULLRDN; eptr=next) {
		next = eptr->rdn_next;
		rdn_comp_free (eptr);
	}
}

