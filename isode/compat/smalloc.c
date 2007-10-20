/* smalloc.c - error checking malloc */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/smalloc.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/smalloc.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: smalloc.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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

#include "general.h"
#include "manifest.h"

/*  */

static void default_smalloc_handler ()
{
	abort ();
}

static VFP smalloc_handler = default_smalloc_handler;

VFP set_smalloc_handler (fnx)
VFP fnx;
{
	VFP savefnx = smalloc_handler;

	if (fnx)
		smalloc_handler = fnx;
	else	smalloc_handler = default_smalloc_handler;
	return savefnx;
}

char *
smalloc(size)
int	size;
{
	register char *ptr;

	if ((ptr = malloc((unsigned) size)) == (char *)0) {
		(*smalloc_handler) ();
		_exit(1);	/* just in case */
	}

	return(ptr);
}
