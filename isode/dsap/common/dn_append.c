#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/dn_append.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/dn_append.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: dn_append.c,v $
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

extern LLog * log_dsap;

dn_append (a,b)
DN a,b;
{
register DN ptr;
register DN eptr;

	if (a == NULLDN)
		DLOG (log_dsap,LLOG_DEBUG,("appending to null dn!"));
	else {
		for (eptr = a; eptr != NULLDN; eptr = eptr->dn_parent)
			ptr = eptr;
		ptr->dn_parent = b;
	}
}

