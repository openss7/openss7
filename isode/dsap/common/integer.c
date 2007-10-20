#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/integer.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/integer.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: integer.c,v $
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


/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

static PE intenc (x)
int * x;
{
	return (int2prim(*x));
}
static PE enumenc (x)
int *x;
{
	return enumint2prim ((integer)*x);
}

static int * intdec (pe)
PE pe;
{
int * x;

        if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_INT))
		return (0);

	x = (int *) smalloc (sizeof (int));
	*x = prim2num(pe);

	return x;

}

static int * enumdec (pe)
PE pe;
{
	int *x;

	if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_ENUM))
		return 0;
	x = (int *) smalloc (sizeof (int));
        *x = prim2enum(pe);

        return x;
}

	

/* ARGSUSED */
static intprint (ps,x,format)
PS ps;
int * x,format;
{
	ps_printf (ps,"%d",*x);
}
#define enumprint intprint

static int * intdup (x)
int *x;
{
int *y;

	y = (int *) smalloc (sizeof (int));
	*y = *x;
	
	return (y);
}
#define enumdup intdup

static intcmp (x,y)
int *x,*y;
{
	return ( *x == *y ? 0 : (*x > *y ? 1 : -1) );
}
#define enumcmp intcmp

static intfree (x)
int * x;
{
	free ((char *) x);
}
#define enumfree intfree

static int * intparse (str)
char * str;
{
int atoi();
int * x;

	x = (int *) smalloc (sizeof (int));
	*x = atoi (str);

	return (x);
}
#define enumparse intparse

integer_syntax ()
{
	(void) add_attribute_syntax ("integer",
		(IFP) intenc,	(IFP) intdec,
		(IFP) intparse,	intprint,
		(IFP) intdup,	intcmp,
		intfree,	NULLCP,
		NULLIFP,	FALSE);

	(void) add_attribute_syntax("enumerated",
				    (IFP) enumenc, 	(IFP)enumdec,
				    (IFP) enumparse,	enumprint,
				    (IFP) enumdup,	enumcmp,
				    enumfree,		NULLCP,
				    NULLIFP,		FALSE);
}
