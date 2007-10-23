...

    while (fgets (buffer, sizeof buffer, stdin))
	if (SDataRequest (sd, buffer, strlen (buffer) + 1, si) == NOTOK)
	    error ("S-DATA.REQUEST: %s", SErrString (sa -> sa_reason));

    if (SRelRequest (sd, NULLCP, 0, NOTOK, sr, si) == NOTOK)
	error ("S-RELEASE.REQUEST: %s", SErrString (sa -> sa_reason));

    if (!sr -> sr_affirmative) {
	(void) SUAbortRequest (sd, NULLCP, 0, si);
	error ("release rejected by sink");
    }

    exit (0);
}
