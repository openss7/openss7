#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/attrt_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/attrt_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: attrt_cpy.c,v $
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

/* Could turn this all into a macro ! */

#ifdef AttrT_cpy
#undef AttrT_cpy
#endif

AttributeType AttrT_cpy (x)
register AttributeType x;
{
	return x;
}


