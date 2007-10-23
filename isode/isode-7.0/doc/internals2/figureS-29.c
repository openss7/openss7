		if ((rt = get_rtent (ip = oid -> oid_elements
					    + ot -> ot_name -> oid_nelem,
				     j = oid -> oid_nelem
					     - ot -> ot_name -> oid_nelem,
				     rts_inet, 1)) == NULL)
		    return NOTOK;

		if ((i = j - rt -> rt_insize) < 0) {
		    if ((new = oid_extend (oid, -i)) == NULLOID)
			return int_SNMP_error__status_genErr;
		    if (v -> name)
			free_SNMP_ObjectName (v -> name);
		    v -> name = new;

		    oid = new;
		}
		else
		    if (i > 0)
			oid -> oid_nelem -= i;

		ip = oid -> oid_elements + ot -> ot_name -> oid_nelem;
		jp = rt -> rt_instance;
		for (i = rt -> rt_insize; i > 0; i--)
		    *ip++ = *jp++;
	    }
	    break;
