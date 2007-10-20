...

    while (fgets (buffer, sizeof buffer, stdin)) {
	register PE = oct2prim (buffer, strlen (buffer) + 1);

	if (PDataRequest (sd, &pe, 1, pi) == NOTOK)
	    error ("P-DATA.REQUEST: %s", PErrString (pa -> pa_reason));

	pe_free (pe);
    }

    if (PRelRequest (sd, NULLPEP, 0, NOTOK, pr, pi) == NOTOK)
	error ("P-RELEASE.REQUEST: %s", PErrString (pa -> pa_reason));

    if (!pr -> pr_affirmative) {
	(void) PUAbortRequest (sd, NULLPEP, 0, pi);
	error ("release rejected by sink");
    }
    PRFREE (pr);

    exit (0);
}
