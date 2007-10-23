static int  o_ip_route (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i,
		    j;
    register unsigned int *ip,
			  *jp;
    register struct rtetab *rt;
    register OID    oid = oi -> oi_name;
    OID	    new;
    register OT	    ot = oi -> oi_type;

    if (get_routes (offset) == NOTOK)
	return int_SNMP_error__status_genErr;

    ifvar = (int) ot -> ot_info;
