    if ((pe = ps2pe (ps)) == NULLPE) {
#ifdef	COTS
	if (rfx == ts_read) {
/* note T-DISCONNECT... */
	}
#endif
	advise (LLOG_EXCEPTIONS, NULLCP, "ps2pe: %s (%s)",
		ps_error (ps -> ps_errno), source);
	snmpstat.s_inpkts++;
	snmpstat.s_asnparseerrs++;
	goto out;
    }
#ifdef	COTS
    if (rfx == ts_read)
	advise (LLOG_XXX, NULLCP, "T-DATA.INDICATION: %d (%s)", fd, source);
    else
#endif
	advise (LLOG_XXX, NULLCP, "packet from %s", source);

    snmpstat.s_inpkts++;
