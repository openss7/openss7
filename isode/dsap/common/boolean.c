#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/boolean.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/boolean.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: boolean.c,v $
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


/*
	SYNTAX:
		boolean ::= "TRUE" | "FALSE"
*/

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

extern int strprint();
extern int sfree();
extern int lexequ();

static PE boolenc (x)
char *x;
{
	return (bool2prim (lexequ (x,"TRUE") ? 0 : 1));
}

static char * booldec (pe)
PE pe;
{
        if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_BOOL))
		return (NULLCP);

	if (prim2flag (pe) == 1)
		return (strdup ("TRUE"));
	else
		return (strdup ("FALSE"));
}

static char * boolget (x)
char * x;
{
	if ((lexequ (x,"TRUE") == 0) || (lexequ (x,"FALSE") == 0))
		return (strdup(x));

	parse_error ("TRUE or FALSE expected (%s)",x);
	return (NULLCP);
}

boolean_syntax ()
{
	(void) add_attribute_syntax ("boolean",
		(IFP) boolenc,	(IFP) booldec,
		(IFP) boolget,	strprint,
		(IFP) strdup,	lexequ,
		sfree,		NULLCP,
		NULLIFP,	FALSE);
}

