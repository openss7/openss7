#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/dn_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/dn_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: dn_new.c,v $
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

DN  dn_comp_new (rdn)
RDN rdn;
{
DN ptr;
	ptr = dn_comp_alloc ();
	dn_comp_fill (ptr,rdn);
	ptr->dn_parent = NULLDN;
	return (ptr);
}

