...

    for (;;) {
	if (TReadRequest (sd, tx, NOTOK, td) == NOTOK) {
	    if (td -> td_reason != DR_NORMAL)
		error ("T-READ.REQUEST: %s", TErrString (td -> td_reason));
	    break;
	}

	if (tx -> tx_expedited)
	    result = TExpdRequest (sd, tx -> tx_base, tx -> tx_cc, td);
	else
	    result = TDataRequest (sd, tx -> tx_base, tx -> tx_cc, td);
	if (tx -> tx_base)
	    free (tx -> tx_base);

	if (result == NOTOK)
	    error ("%s: %s", tx -> tx_expedited ? "T-EXPEDITED-DATA.REQUEST"
		    : "T-DATA.REQUEST", TErrString (td -> td_reason));

    }

    exit (0);
}
