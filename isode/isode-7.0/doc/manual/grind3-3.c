#include <stdio.h>
#include <isode/ppkt.h>

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    int     len;
    char   *cp;
    register PS	    ps;
    register PE	    pe;

    if ((ps = ps_alloc (std_open)) == NULLPS)
	error ("ps_alloc");
    if (std_setup (ps, stdin) == NOTOK)
	error ("std_setup: %s", ps_errno (ps -> ps_errno));

    for (;;) {
	if ((pe = ps2pe (ps)) == NULLPE)
	    if (ps -> ps_errno)
		error ("ps2pe: %s", ps_errno (ps -> ps_errno));
	    else
		break;		/* end-of-file */

/* process ``pe'' here... */

	    pe_free (pe);
    }

    ps_free (ps);

    exit (0);
}
