    msg = NULL;
    if (decode_SNMP_Message (pe, 1, NULLIP, NULLVP, &msg) == NOTOK) {
	advise (LLOG_EXCEPTIONS, NULLCP, "decode_SNMP_Message: %s (%s)",
		PY_pepy, source);
	snmpstat.s_asnparseerrs++;
#ifdef	COTS
	if (rfx == ts_read) {
/* clear connection... */
	}
#endif
	goto out;
    }

    PLOG (pgm_log, print_SNMP_Message, pe, "Message", 1);
