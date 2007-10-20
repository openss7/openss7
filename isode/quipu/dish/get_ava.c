/* get_ava.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/dish/RCS/get_ava.c,v 9.0 1992/06/16 12:35:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/dish/RCS/get_ava.c,v 9.0 1992/06/16 12:35:39 isode Rel $
 *
 *
 * $Log: get_ava.c,v $
 * Revision 9.0  1992/06/16  12:35:39  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include "quipu/util.h"
#include "quipu/commonarg.h"

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern	char	frompipe;
extern	PS	opt, rps;

get_ava (avasert, type, value)
AVA            *avasert;
char           *type;
char           *value;
{
	char           *TidyString ();

	if (type == NULLCP || *type == 0)
		return (NOTOK);

	avasert->ava_type = AttrT_new (TidyString (type));
	if (avasert->ava_type == NULLAttrT) {
		ps_printf (OPT, "Invalid at %s\n", type);
		return (NOTOK);
	}

	if ((avasert->ava_value = str_at2AttrV (TidyString (value), avasert->ava_type)) == NULLAttrV) {
		ps_print (OPT, "Invalid attribute value value\n");
		AttrT_free (avasert->ava_type);
		return (NOTOK);
	}

	return (OK);
}
