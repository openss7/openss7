	case type_SNMP_PDUs_get__next__request:
	    if ((i = oid -> oid_nelem - ot -> ot_name -> oid_nelem) != 0
		    && i < IFN_SIZE) {
		for (jp = (ip = oid -> oid_elements
				+ ot -> ot_name -> oid_nelem - 1) + i;
			 jp > ip; jp--)
		    if (*jp != 0)
			break;
		if (jp == ip)
		    oid -> oid_nelem = ot -> ot_name -> oid_nelem;
		else {
		    if ((new = oid_normalize (oid, IFN_SIZE - i, 255))
			    == NULLOID)
			return int_SNMP_error__status_genErr;
		    if (v -> name)
			free_SNMP_ObjectName (v -> name);
		    v -> name = oid = new;
		}
	    }
