#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/as_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/as_new.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: as_new.c,v $
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

Attr_Sequence  as_comp_new (at,as,acl)
AttributeType  at;
AV_Sequence    as;
struct acl_info * acl;
{
Attr_Sequence ptr;
	ptr = as_comp_alloc ();
	bzero ((char *)ptr,sizeof(*ptr));
	ptr->attr_value = as;
	ptr->attr_acl   = acl;
	if (at) 
		ptr->attr_type = at;
	ptr->attr_link = NULLATTR;
	return (ptr);
}

