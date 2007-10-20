-- pp.py - generic pretty-printer

-- $Header: /xtel/isode/isode/pepy/RCS/pp.py,v 9.0 1992/06/16 12:25:01 isode Rel $
--
--
-- $Log: pp.py,v $
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


PRETTY DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/pp.py,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif

/*  */

%}


BEGIN

PRINTER print

Printer ::=
    ANY

END

%{

/*  */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    exit (PY_pp (argc, argv, envp, print_PRETTY_Printer));
    return 0;
}

/*  */

%}
