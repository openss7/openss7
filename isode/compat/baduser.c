/* baduser.c - check file of bad users */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/baduser.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/baduser.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: baduser.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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
#include "general.h"
#include "manifest.h"
#include "tailor.h"

/*  */

int	baduser (file, user)
char   *file,
       *user;
{
    int     hit,
	    tries;
    register char  *bp;
    char    buffer[BUFSIZ];
    FILE   *fp;

    hit = 0;
    for (tries = 0; tries < 2 && !hit; tries++) {
	switch (tries) {
	    case 0:
	        if (file) {
		    bp = isodefile (file, 0);
		    break;
		}
		tries++;
		/* and fall */
	    default:
		bp = "/etc/ftpusers";
		break;
	}
	if ((fp = fopen (bp, "r")) == NULL)
	    continue;

	while (fgets (buffer, sizeof buffer, fp)) {
	    if (bp = index (buffer, '\n'))
		*bp = NULL;
	    if (strcmp (buffer, user) == 0) {
		hit++;
		break;
	    }
	}

	(void) fclose (fp);
    }


    return hit;
}
