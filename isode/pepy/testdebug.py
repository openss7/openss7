-- testdebug.py - support routine for pepy generated routines

-- $Header: /xtel/isode/isode/pepy/RCS/testdebug.py,v 9.0 1992/06/16 12:25:01 isode Rel $
--
--
-- $Log: testdebug.py,v $
-- Revision 9.0  1992/06/16  12:25:01  isode
-- Release 8.0
--
--

--
--				  NOTICE
--
--    Acquisition, use, and distribution of this module and related
--    materials are subject to the restrictions of a license agreement.
--    Consult the Preface in the User's Manual for the full terms of
--    this agreement.
--
--


TESTDEBUG DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/testdebug.py,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif

/* LINTLIBRARY */


#include <stdio.h>

%}

BEGIN

END

%{
int	testdebug (pe, s)
register PE	pe;
register char	*s;
{
    char  *cp;
    register PS ps;
    static int debug = OK;
    
    switch (debug) {
	case NOTOK:
	    return;

	case OK:
	    if ((debug = (cp = getenv ("PEPYDEBUG")) && *cp ? atoi (cp)
							    : NOTOK) == NOTOK)
		return;
	    (void) fflush (stdout);
#ifndef lint
	    (void) fprintf (stderr, "testdebug made with %s\n", pepyid);
#endif
	    /* and fall... */

	default:
	    (void) fflush (stdout);
	    (void) fprintf (stderr, "%s\n", s);

	    if ((ps = ps_alloc (std_open)) == NULLPS)
		break;
	    if (std_setup (ps, stderr) != NOTOK)
		(void) pe2pl (ps, pe);
	    (void) fprintf (stderr, "--------\n");
	    ps_free (ps);
	    break;
    }
}

%}
