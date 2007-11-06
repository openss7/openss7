    switch (offset) {
	case type_SNMP_PDUs_get__request:
	    if (oid -> oid_nelem < ot -> ot_name -> oid_nelem + IFN_SIZE)
		return int_SNMP_error__status_noSuchName;
	    if ((rt = get_rtent (oid -> oid_elements 
				     + ot -> ot_name -> oid_nelem,
				 oid -> oid_nelem
				     - ot -> ot_name -> oid_nelem,
				 rts_inet, 0))
		    == NULL)
		return int_SNMP_error__status_noSuchName;
	    break;

