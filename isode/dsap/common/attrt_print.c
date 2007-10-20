#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/attrt_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/attrt_print.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: attrt_print.c,v $
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

extern int oidformat;
AttributeType last_at;
int avs_count = 1;

AttrT_print (ps,x,format)
register PS ps;
register AttributeType x;
register int format;
{
	if (x == NULLAttrT) {
		ps_print(ps,"Unknown Type");
		return;
	}

	last_at = x;
	avs_count = 1;

	if (format == READOUT)
		ps_printf (ps,"%s",attr2name (x,oidformat));
	else
		ps_printf (ps,"%s",attr2name_aux (x));
}
