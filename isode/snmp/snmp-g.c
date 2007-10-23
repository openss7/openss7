/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* snmp-g.c - SNMP group */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/snmp/RCS/snmp-g.c,v 9.0 1992/06/16 12:38:11 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/snmp/RCS/snmp-g.c,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 *
 * Log: snmp-g.c,v
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

init_snmp()
{
	register OT ot;

	bzero((char *) &snmpstat, sizeof snmpstat);

	if (ot = text2obj("snmpInPkts"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_inpkts;
	if (ot = text2obj("snmpOutPkts"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_outpkts;
	if (ot = text2obj("snmpInBadVersions"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_badversions;
	if (ot = text2obj("snmpInBadCommunityNames"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_badcommunitynames;
	if (ot = text2obj("snmpInBadCommunityUses"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_badcommunityuses;
	if (ot = text2obj("snmpInASNParseErrs"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_asnparseerrs;
	if (ot = text2obj("snmpInTotalReqVars"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_totalreqvars;
	if (ot = text2obj("snmpInTotalSetVars"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_totalsetvars;
	if (ot = text2obj("snmpInGetRequests"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_ingetrequests;
	if (ot = text2obj("snmpInGetNexts"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_ingetnexts;
	if (ot = text2obj("snmpInSetRequests"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_insetrequests;
	if (ot = text2obj("snmpInGetResponses"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_ingetresponses;
	if (ot = text2obj("snmpInTraps"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_intraps;
	if (ot = text2obj("snmpOutTooBigs"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_toobigs;
	if (ot = text2obj("snmpOutNoSuchNames"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_nosuchnames;
	if (ot = text2obj("snmpOutBadValues"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_badvalues;
	if (ot = text2obj("snmpOutGenErrs"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_generrs;
	if (ot = text2obj("snmpOutGetResponses"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_outgetresponses;
	if (ot = text2obj("snmpOutTraps"))
		ot->ot_getfnx = o_generic, ot->ot_info = (caddr_t) &snmpstat.s_outtraps;
	if (ot = text2obj("snmpEnableAuthenTraps")) {
		ot->ot_getfnx = o_generic;
		ot->ot_setfnx = s_generic;
		if ((ot->ot_info = calloc(1, sizeof(integer))) == NULL)
			adios(NULLCP, "out of memory");
		snmpstat.s_enableauthentraps = &ot->ot_info;
		*((integer *) * snmpstat.s_enableauthentraps) = TRAPS_ENABLED;
		set_hint(ot, TRAPS_MAXVALUE);
	} else
		adios(NULLCP, "unable to resolve snmpEnableAuthenTraps");

	if (ot = text2obj("agentAction")) {
		ot->ot_getfnx = o_generic;
		ot->ot_setfnx = s_generic;
		if ((ot->ot_info = calloc(1, sizeof(integer))) == NULL)
			adios(NULLCP, "out of memory");
		agentAction = &ot->ot_info;
		*((integer *) * agentAction) = AGENT_OTHER;
		set_hint(ot, AGENT_MAXVALUE);
	} else
		adios(NULLCP, "unable to resolve agentAction");
}

/*  */

static int
set_hint(ot, hi)
	OT ot;
	int hi;
{
	char *name;
	register OS os;

	if ((os = ot->ot_syntax) == NULL)
		adios(NULLCP, "no syntax defined for object \"%s\"", ot->ot_text);
	name = os->os_name;

	(void) add_syntax(ot->ot_text, os->os_encode, os->os_decode,
			  os->os_free, os->os_parse, os->os_print);
	if ((os = text2syn(ot->ot_text)) == NULL)
		adios(NULLCP, "lost syntax for object \"%s\"", ot->ot_text);
	ot->ot_syntax = os;
	os->os_name = name;
	os->os_data2 = hi;
}
