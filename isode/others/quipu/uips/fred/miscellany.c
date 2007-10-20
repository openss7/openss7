/* miscellany.c - fred miscellaneous functions */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/miscellany.c,v 9.0 1992/06/16 12:44:30 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/fred/RCS/miscellany.c,v 9.0 1992/06/16 12:44:30 isode Rel $
 *
 *
 * $Log: miscellany.c,v $
 * Revision 9.0  1992/06/16  12:44:30  isode
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


#include <ctype.h>
#include "fred.h"

/*    DATA */

int	area_quantum = 0;

struct area_guide areas[] = {
    W_ORGANIZATION,
	"organization",	"-singlelevel",	"organization",		"o",	NULL,

    W_UNIT,
	"unit",		"-subtree",	"organizationalUnit",	"ou",	NULL,

    W_LOCALITY,
	"locality",	"-singlelevel", "locality",		"l",	NULL,

    W_PERSON,
	"person",	"-subtree",	"person",		"cn",	NULL,

    W_DSA,
	"dsa",		"-singlelevel",	"dsa",			"cn",	NULL,

    W_ROLE,
	"role",		"-subtree",	"organizationalRole",	"cn",	NULL,

    NULL
};

/*    ALIAS */

/* ARGSUSED */

int	f_alias (vec)
char  **vec;
{
    char   *cp,
	    buffer[BUFSIZ];

    if ((cp = *++vec) == NULL)
	return dish ("squid -fred -sequence default", 0);

    if (strcmp (cp, "-help") == 0) {
	(void) fprintf (stdfp, "alias [name]\n");
	(void) fprintf (stdfp, "    with no arguments, reports on active aliases\n");
	(void) fprintf (stdfp,
		 "    with one argument, defines an alias for the given name\n");

	return OK;
    }

    (void) sprintf (buffer, "squid -fred -alias \"%s\"", cp);
    return dish (buffer, runcom);
}

/*    AREA */

int	f_area (vec)
char  **vec;
{
    int	    status;
    char   *cp,
	   *dp,
	   *ep,
	    buffer[BUFSIZ];
    register struct area_guide *ag;

    if ((cp = *++vec) == NULL) {
	if (myarea == NULL) {
	    if (dish ("moveto -pwd", 0) == NOTOK)
		return NOTOK;
	}
	else
	    (void) fprintf (stdfp, "                     default area %s\n", myarea);

	for (ag = areas; ag -> ag_record; ag++)
	    if (ag -> ag_area)
		(void) fprintf (stdfp, "area for record-type %-12.12s %s\n",
			 ag -> ag_key, ag -> ag_area);

	return OK;
    }

    if (strcmp (cp, "-help") == 0) {
	(void) fprintf (stdfp, "area [[record] location]\n");
	(void) fprintf (stdfp,
		 "    with no arguments, lists areas current defined for various searches\n");
	(void) fprintf (stdfp,
		 "    with one argument, sets the default area for general searches\n");
	(void) fprintf (stdfp,
		 "    with two arguments, sets the default area for the given record type\n");

	return OK;
    }

    if ((dp = *++vec) == NULL) {
	(void) sprintf (buffer, "moveto -pwd \"%s\"", cp);
	if (dish (buffer, 1) == NOTOK) {
	    advise (NULLCP, "bad area: \"%s\"", cp);
	    return NOTOK;
	}
	if (!runcom)
	    (void) fprintf (stdfp, "%s\n", myarea);

	area_quantum++;
	return OK;
    }

    if ((ep = index (dp, '=')) && *++ep == '@' && runcom) {
	ep = isodefile ("fredrc", 0);
	adios (NULLCP,
	       "%s is not configured!\n\tif this is your first system to run fred, read the Admin. Guide\n\totherwise, get a copy of %s from your first system",
	       ep, ep);
    }

    for (ag = areas; ag -> ag_record; ag++)
	if (strcmp (ag -> ag_key, cp) == 0)
	    break;
    if (!ag -> ag_record) {
	advise (NULLCP, "invalid record-type: \"%s\"", cp);
	return NOTOK;
    }

    if (cp = myarea)
	myarea = NULL;

    (void) sprintf (buffer, "moveto -pwd \"%s\"", dp);
    if ((status = dish (buffer, 1)) == OK) {
	if (ag -> ag_area)
	    free (ag -> ag_area);
	ag -> ag_area = myarea;

	if (!runcom)
	    (void) fprintf (stdfp, "area for record-type %s: %s\n",
		     ag -> ag_key, ag -> ag_area);
    }
    else {
	advise (NULLCP, "bad area: \"%s\"", dp);
	if (myarea)
	    free (myarea), myarea = NULL;
    }

    if (myarea = cp) {
	(void) sprintf (buffer, "moveto -pwd \"%s\"", myarea);
	(void) dish (buffer, 1);
    }

    area_quantum++;
    return status;
}

/*    DISH */

int	f_dish (vec)
char  **vec;
{
    register char *bp,
		  *cp;
    char    buffer[BUFSIZ];

    if ((cp = *++vec) == NULL)
	return dish ("squid -fred", 0);
    if (strcmp (cp, "-help") == 0) {
	(void) fprintf (stdfp, "dish [command [arguments ...]]\n");
	(void) fprintf (stdfp, "    with no arguments, reports on status of dish\n");
	(void) fprintf (stdfp, "    with arguments, passes those directly to dish\n");

	return OK;
    }

    (void) strcpy (bp = buffer, cp);
    bp += strlen (bp);

    while (cp = *++vec) {
	(void) sprintf (bp, " \"%s\"", cp);
	bp += strlen (bp);
    }

    return dish (buffer, runcom);
}

/*    EDIT */

int	f_edit (vec)
char  **vec;
{
    int	    result;
    char    buffer[BUFSIZ];

    if (*++vec != NULL && strcmp (*vec, "-help") == 0) {
	(void) fprintf (stdfp, "edit\n");
	(void) fprintf (stdfp, "    edit entry in the white pages\n");

	return OK;
    }

    if (readonly) {
	advise (NULLCP, "read only system ... edits not allowed ...");
	return NOTOK;
    }

    if (mydn == NULL) {
	advise (NULLCP, "who are you?  use the \"thisis\" command first...");
	return NOTOK;
    }

    (void) sprintf (buffer, "modify -nocache -dontusecopy -newdraft \"%s\"",
		    mydn);
    dontpage = 1;
    result = dish (buffer, 0);
    dontpage = 0;

    if (result != OK)
	return result;

    (void) sprintf (buffer, "showentry \"%s\" -fred -nocache -dontusecopy",
		    mydn);
    (void) dish (buffer, 0);
    return OK;
}

/*    MANUAL */

int	f_manual (vec)
char  **vec;
{
    char   buffer[BUFSIZ];
    FILE  *fp;

    if (*++vec != NULL && strcmp (*vec, "-help") == 0) {
	(void) fprintf (stdfp, "manual\n");
	(void) fprintf (stdfp, "    print detailed information\n");

	return OK;
    }

    (void) strcpy (buffer, isodefile ("fred.0", 0));
    if (fp = fopen (buffer, "r")) {
	while (fgets (buffer, sizeof buffer, fp))
	    paginate (stdfp, buffer, strlen (buffer));
	paginate (stdfp, NULLCP, 0);

	(void) fclose (fp);
    }
    else
	advise (buffer, "unable to open");

    return OK;
}

/*    REPORT */

int	f_report (vec)
char  **vec;
{
    register char   *bp;
    char   *cp,
	    buffer[BUFSIZ];

    if (*++vec != NULL && strcmp (*vec, "-help") == 0) {
	(void) fprintf (stdfp, "report [subject]\n");
	(void) fprintf (stdfp, "    send report to white pages manager\n");

	return OK;
    }

    if (readonly)
	(void) strcpy (buffer, _isodefile (isodebinpath, "mhmail"));
    bp = buffer;
    cp = strcmp (manager, "internal") ? manager : "wpp-manager@psi.com";

    if (!readonly || access (buffer, 0x01) == NOTOK) {
	(void) strcpy (bp, "/usr/ucb/Mail ");
	bp += strlen (bp);

	if (debug) {
	    (void) sprintf (bp, "-v ");
	    bp += strlen (bp);
	}

	if (readonly) {
	    (void) sprintf (bp, "-r \"%s\" ", cp);
	    bp += strlen (bp);
	}

	(void) sprintf (bp, "-s");
    }
    else {
	bp += strlen (bp);

	(void) sprintf (bp, " -subject");
    }
    bp += strlen (bp);

    (void) sprintf (bp, " \"%s\" \"%s\"",
		    *vec ? *vec : "White Pages report", cp);
    bp += strlen (bp);
			

    (void) fprintf (stdfp, "End report with CTRL-D%s.\n",
	readonly ? ", it will then take 30 seconds to post message" : "");
    if (readonly)
	(void) fprintf (stdfp,
		 "In the message please specify your e-mail address, so a reply may be made.\n");
    (void) fflush (stdfp);

    if (watch) {
	(void) fprintf (stderr, "%s\n", buffer);
	(void) fflush (stderr);
    }
    if (system (buffer))
	advise (NULLCP, "problem sending report");

    return OK;
}

/*    THISIS */

int	f_thisis (vec)
char  **vec;
{
    register char   *bp;
    char   *cp,
	    buffer[BUFSIZ];

again: ;
    if ((cp = *++vec) == NULL) {
	if (mydn == NULL) {
	    advise (NULLCP, "who are you?");
	    return NOTOK;
	}

	(void) printf ("you are \"%s\"\n", mydn);
	return OK;
    }

    if (strcmp (cp, "-help") == 0) {
	(void) fprintf (stdfp, "thisis [name]\n");
	(void) fprintf (stdfp,
		 "    with no arguments, lists your name in the white pages\n");
	(void) fprintf (stdfp,
		 "    with one argument, identifies you in the white pages\n");

	return OK;
    }

    if (strcmp (cp, "is") == 0)
	goto again;

    if (*cp == '!')
	cp++;
    for (bp = cp; isdigit (*bp); bp++)
	continue;
    if (*bp && (index (cp, '@') == NULL || index (cp, '=') == NULL)) {
	advise (NULLCP,
	"expecting a distinguished name (if you don't know what this is, punt)"
		);
	return NOTOK;
    }

    bp = buffer;

    (void) sprintf (bp, "bind -simple -user \"%s\"", cp);
    bp += strlen (bp);

    if (*++vec) {
	if (runcom && (rcmode & 077))
	    adios (NULLCP,
		   "incorrect mode for runcom file -- use \"chmod 0600 $HOME/.fredrc\"");

	(void) sprintf (bp, " -password \"%s\"", *vec);
	bp += strlen (bp);
    }

    if (dish (buffer, 0) != OK) {
	(void) f_quit (NULLVP);
	exit (1);	/* NOT REACHED */
    }

    if (runcom)
	didbind = 1;

    (void) dish ("squid -user", 1);

    return OK;
}
