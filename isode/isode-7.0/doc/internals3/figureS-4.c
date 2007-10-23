    PE	    pe;
    PS	    ps;

    pe = NULLPE;
    if ((ps = ps_alloc (dg_open)) == NULLPS
	    || dg_setup (ps, fd, MAXDGRAM, rfx, wfx, cfx) == NOTOK) {
	if (ps == NULLPS)
	    advise (LLOG_EXCEPTIONS, NULLCP, "ps_alloc: out of memory (%s)",
		    source);
	else
	    advise (LLOG_EXCEPTIONS, NULLCP, "dg_setup: %s (%s)",
		    ps_error (ps -> ps_errno), source);

	goto out;
    }
