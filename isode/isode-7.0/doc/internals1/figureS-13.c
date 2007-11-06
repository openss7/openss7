	    if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
		if ((rt = rts_inet) == NULL)
		    return NOTOK;
		if ((new = oid_extend (oid, IFN_SIZE)) == NULLOID)
		    return int_SNMP_error__status_genErr;
		ip = new -> oid_elements + new -> oid_nelem - IFN_SIZE;
		if (v -> name) free_SNMP_ObjectName (v -> name);
		v -> name = new;
	    }
	    else
		if ((rt = get_rtent (ip = oid -> oid_elements
					    + oid -> oid_nelem - IFN_SIZE,
				     IFN_SIZE, rts_inet, 1)) == NULL)
		    return NOTOK;
	    for (i = IFN_SIZE, jp = rt -> rt_instance; i > 0; i--)
		*ip++ = *jp++;
	    break;
