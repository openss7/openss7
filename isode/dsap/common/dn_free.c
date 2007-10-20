#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/dn_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/dn_free.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: dn_free.c,v $
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

dn_comp_free (dn)
DN dn;
{
	rdn_free (dn->dn_rdn);
	free ((char *) dn);
}
dn_free (dn)
DN dn;
{
register DN eptr;
register DN next;

	for (eptr = dn; eptr != NULLDN; eptr=next) {
		next = eptr->dn_parent;
		dn_comp_free (eptr);
	}
}

