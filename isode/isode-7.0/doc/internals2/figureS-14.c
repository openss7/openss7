	default:
	    return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case ipNextHop:
	    return o_ipaddr (oi, v, (struct sockaddr_in *) &rt -> rt_gateway);

...

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}
