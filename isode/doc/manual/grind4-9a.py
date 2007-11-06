...

%{
/*    DEBUG */

#ifdef  DEBUG
testdebug (pe, s)
register PE pe;
register char *s;
{
    static int  debug = OK;
    char   *cp;
    register PS     ps;

    switch (debug) {
	case NOTOK: 
	    break;

	case OK: 
	    if ((debug = (cp = getenv ("PEPYDEBUG")) && *cp ? atoi (cp)
			: NOTOK) == NOTOK)
		break;
	    fprintf (stderr, "%s made with %s\n", myname, pepyid);

	default: 
	    fprintf (stderr, "%s\n", s);

	    if ((ps = ps_alloc (std_open)) == NULLPS)
		return;
	    if (std_setup (ps, stderr) != NOTOK)
		(void) pe2pl (ps, pe);
	    fprintf (stderr, "--------\n");
	    ps_free (ps);
	    break;
    }
}
#endif

...
