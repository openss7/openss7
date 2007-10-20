#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/rdn_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/rdn_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: rdn_new.c,v $
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

RDN  rdn_comp_new (at,av)
AttributeType  at;
AttributeValue av;
{
register RDN ptr;
	ptr = rdn_comp_alloc ();
	bzero ((char *)ptr, sizeof (*ptr));
	ptr->rdn_at = at;
	if (av)
		AttrV_cpy_aux (av,&ptr->rdn_av);
	ptr->rdn_next = NULLRDN;
	return (ptr);
}

