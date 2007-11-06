...

/*    MAIN */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    PE	    pe;
    PS	    ps;

    myname = *argv;
    if ((ps = ps_alloc (std_open)) == NULLPS)
	adios (NULLCP, "ps_alloc loses");
    if (std_setup (ps, stdin) == NOTOK)
	ps_adios (ps, "std_setup");

    for (;;) {
	if ((pe = ps2pe (ps)) == NULLPE)
	    if (ps -> ps_errno)
		ps_adios (ps, "ps2pe");
	    else
		break;

	(void) parse_PERSONNEL_PersonnelRecord (pe, 1, NULLIP, NULLVP,
		    NULLCP);

	pe_free (pe);
    }

    ps_free (ps);

    exit (0);
}

%}

...
