/* sprintaei.c - manage AE info */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/acsap/RCS/sprintaei.c,v 9.0 1992/06/16 12:05:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/acsap/RCS/sprintaei.c,v 9.0 1992/06/16 12:05:59 isode Rel $
 *
 *
 * $Log: sprintaei.c,v $
 * Revision 9.0  1992/06/16  12:05:59  isode
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


/* LINTLIBRARY */

#include <stdio.h>
#include "psap.h"
#include "isoaddrs.h"

/*  */

char   *sprintaei (aei)
AEI	aei;
{
    register char *cp;
    char   *bp;
    static int    i;
    static char buffer1[BUFSIZ],
		buffer2[BUFSIZ];

    bp = cp = (i++ % 2) ? buffer1 : buffer2;

    *cp++ = '<';

    if (aei -> aei_ap_title) {
	vpushstr (cp);
	vunknown (aei -> aei_ap_title);
	vpopstr ();
	cp += strlen (cp);
    }
    *cp++ = ',';

    if (aei -> aei_ae_qualifier) {
	vpushstr (cp);
	vunknown (aei -> aei_ae_qualifier);
	vpopstr ();
	cp += strlen (cp);
    }
    *cp++ = ',';

    if (aei -> aei_flags & AEI_AE_ID) {
	(void) sprintf (cp, "%d", aei -> aei_ae_id);
	cp += strlen (cp);
    }
    *cp++ = ',';

    if (aei -> aei_flags & AEI_AP_ID) {
	(void) sprintf (cp, "%d", aei -> aei_ap_id);
	cp += strlen (cp);
    }
    (void) strcpy (cp, ">");

    return bp;
}
