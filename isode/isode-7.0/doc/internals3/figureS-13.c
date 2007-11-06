	    if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
		if ((rt = rts_inet) == NULL)
		    return NOTOK;

		if ((new = oid_extend (oid, rt -> rt_insize)) == NULLOID)
		    return int_SNMP_error__status_genErr;
		ip = new -> oid_elements + new -> oid_nelem - rt -> rt_insize;
		jp = rt -> rt_instance;
		for (i = rt -> rt_insize; i > 0; i--)
		    *ip++ = *jp++;

		if (v -> name)
		    free_SNMP_ObjectName (v -> name);
		v -> name = new;
	    }
	    else {
