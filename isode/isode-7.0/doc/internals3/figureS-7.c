static  int     lastq = -1;

static  int     ipforwarding;
#define FORW_GATEWAY	1
#define FORW_HOST	2

static  struct ipstat ipstat;

static int  o_ip (oi, v, offset)
OI      oi;
register struct type_SNMP_VarBind *v;
int     offset;
{
    int     ifvar;
    register struct ipstat *ips = &ipstat;
    register OID    oid = oi -> oi_name;
    register OT     ot = oi -> oi_type;
    OID	    new;

    ifvar = (int) ot -> ot_info;
