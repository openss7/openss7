#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/attrt_dec.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/attrt_dec.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: attrt_dec.c,v $
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
#include "quipu/syntaxes.h"

extern LLog * log_dsap;

#ifdef AttrT_decode
#undef AttrT_decode
#endif

AttributeType AttrT_decode_aux (oid)
OID oid;
{
register AttributeType x;

	if (oid == NULLOID) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("Null oid to decode"));
		return NULLTABLE_ATTR;
	}

	if ((x = oid2attr (oid)) == NULLTABLE_ATTR) {
		if ((x = AttrT_new (sprintoid(oid))) == NULLTABLE_ATTR) {
			LLOG (log_dsap,LLOG_EXCEPTIONS,("Unknown attribute type oid %s",sprintoid(oid)));
			return NULLTABLE_ATTR;
		}
	}

	return x;
}

