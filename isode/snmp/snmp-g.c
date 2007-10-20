/* snmp-g.c - SNMP group */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/snmp/RCS/snmp-g.c,v 9.0 1992/06/16 12:38:11 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/snmp-g.c,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 *
 * $Log: snmp-g.c,v $
 * Revision 9.0  1992/06/16  12:38:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>
#include "mib.h"
#include "snmp-g.h"

/*    SNMP GROUP */

init_snmp () {
    register OT	    ot;

    bzero ((char *) &snmpstat, sizeof snmpstat);

    if (ot = text2obj ("snmpInPkts"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_inpkts;
    if (ot = text2obj ("snmpOutPkts"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_outpkts;
    if (ot = text2obj ("snmpInBadVersions"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_badversions;
    if (ot = text2obj ("snmpInBadCommunityNames"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_badcommunitynames;
    if (ot = text2obj ("snmpInBadCommunityUses"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_badcommunityuses;
    if (ot = text2obj ("snmpInASNParseErrs"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_asnparseerrs;
    if (ot = text2obj ("snmpInTotalReqVars"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_totalreqvars;
    if (ot = text2obj ("snmpInTotalSetVars"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_totalsetvars;
    if (ot = text2obj ("snmpInGetRequests"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_ingetrequests;
    if (ot = text2obj ("snmpInGetNexts"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_ingetnexts;
    if (ot = text2obj ("snmpInSetRequests"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_insetrequests;
    if (ot = text2obj ("snmpInGetResponses"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_ingetresponses;
    if (ot = text2obj ("snmpInTraps"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_intraps;
    if (ot = text2obj ("snmpOutTooBigs"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_toobigs;
    if (ot = text2obj ("snmpOutNoSuchNames"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_nosuchnames;
    if (ot = text2obj ("snmpOutBadValues"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_badvalues;
    if (ot = text2obj ("snmpOutGenErrs"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_generrs;
    if (ot = text2obj ("snmpOutGetResponses"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_outgetresponses;
    if (ot = text2obj ("snmpOutTraps"))
	ot -> ot_getfnx = o_generic,
	ot -> ot_info = (caddr_t) &snmpstat.s_outtraps;
    if (ot = text2obj ("snmpEnableAuthenTraps")) {
	ot -> ot_getfnx = o_generic;
	ot -> ot_setfnx = s_generic;
	if ((ot -> ot_info = calloc (1, sizeof (integer))) == NULL)
	    adios (NULLCP, "out of memory");
	snmpstat.s_enableauthentraps = &ot -> ot_info;
	*((integer *) *snmpstat.s_enableauthentraps) = TRAPS_ENABLED;
	set_hint (ot, TRAPS_MAXVALUE);
    }
    else
	adios (NULLCP, "unable to resolve snmpEnableAuthenTraps");

    if (ot = text2obj ("agentAction")) {
	ot -> ot_getfnx = o_generic;
	ot -> ot_setfnx = s_generic;
	if ((ot -> ot_info = calloc (1, sizeof (integer))) == NULL)
	    adios (NULLCP, "out of memory");
	agentAction = &ot -> ot_info;
	*((integer *) *agentAction) = AGENT_OTHER;
	set_hint (ot, AGENT_MAXVALUE);
    }
    else
	adios (NULLCP, "unable to resolve agentAction");
}

/*  */

static int  set_hint (ot, hi)
OT	ot;
int	hi;
{
    char   *name;
    register OS	    os;

    if ((os = ot -> ot_syntax) == NULL)
	adios (NULLCP, "no syntax defined for object \"%s\"", ot -> ot_text);
    name = os -> os_name;

    (void) add_syntax (ot -> ot_text, os -> os_encode, os -> os_decode,
		       os -> os_free, os -> os_parse, os -> os_print);
    if ((os = text2syn (ot -> ot_text)) == NULL)
	adios (NULLCP, "lost syntax for object \"%s\"", ot -> ot_text);
    ot -> ot_syntax = os;
    os -> os_name = name;
    os -> os_data2 = hi;
}
