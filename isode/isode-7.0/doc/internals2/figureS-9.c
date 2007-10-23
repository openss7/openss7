    if (quantum != lastq) {
	lastq = quantum;

	if (getkmem (nl + N_IPFORWARDING, (caddr_t) &ipforwarding,
		     sizeof ipforwarding) == NOTOK
		|| getkmem (nl + N_IPSTAT, (caddr_t) ips, sizeof *ips)
			== NOTOK)
	    return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case ipForwarding:
	    return o_integer (oi, v, ipforwarding ? FORW_GATEWAY : FORW_HOST);

...

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}
