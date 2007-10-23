    switch (offset) {
	case type_SNMP_SMUX__PDUs_get__request:
	    if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1
		    || oid -> oid_elements[oid -> oid_nelem - 1] != 0)
		return int_SNMP_error__status_noSuchName;
	    break;

	case type_SNMP_SMUX__PDUs_get__next__request:
	    if (oid -> oid_nelem != ot -> ot_name -> oid_nelem)
		return NOTOK;
	    if ((new = oid_extend (oid, 1)) == NULLOID)
		return int_SNMP_error__status_genErr;
	    new -> oid_elements[new -> oid_nelem - 1] = 0;
	    if (v -> name) free_SNMP_ObjectName (v -> name);
	    v -> name = new;
	    break;

	default:
	    return int_SNMP_error__status_genErr;
    }
