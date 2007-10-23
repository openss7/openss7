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

/* icmp.c - MIB realization of the ICMP group */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/snmp/RCS/icmp.c,v 9.0 1992/06/16 12:38:11 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/snmp/RCS/icmp.c,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * Log: icmp.c,v
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

#include "internet.h"
#ifdef	BSD44
#include <sys/param.h>
#endif
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp_var.h>

/*  */

static struct icmpstat icmpstat;

/*  */

#define	icmpInMsgs	0
#define	icmpInErrors	1
#define	icmpInDestUnreachs 2
#define	icmpInTimeExcds	3
#define	icmpInParmProbs	4
#define	icmpInSrcQuenchs 5
#define	icmpInRedirects	6
#define	icmpInEchos	7
#define	icmpInEchoReps	8
#define	icmpInTimestamps 9
#define	icmpInTimestampReps 10
#define	icmpInAddrMasks	11
#define	icmpInAddrMaskReps 12
#define	icmpOutMsgs	13
#define	icmpOutErrors	14
#define	icmpOutDestUnreachs 15
#define	icmpOutTimeExcds 16
#define	icmpOutParmProbs 17
#define	icmpOutSrcQuenchs 18
#define	icmpOutRedirects 19
#define	icmpOutEchos	20
#define	icmpOutEchoReps	21
#define	icmpOutTimestamps 22
#define	icmpOutTimestampReps 23
#define	icmpOutAddrMasks 24
#define	icmpOutAddrMaskReps 25

static int
o_icmp(oi, v, offset)
	OI oi;
	register struct type_SNMP_VarBind *v;
	int offset;
{
	int ifvar;
	register struct icmpstat *icps = &icmpstat;
	register OID oid = oi->oi_name;
	register OT ot = oi->oi_type;
	static int lastq = -1;

	ifvar = (int) ot->ot_info;
	switch (offset) {
	case type_SNMP_PDUs_get__request:
		if (oid->oid_nelem != ot->ot_name->oid_nelem + 1
		    || oid->oid_elements[oid->oid_nelem - 1] != 0)
			return int_SNMP_error__status_noSuchName;
		break;

	case type_SNMP_PDUs_get__next__request:
		if (oid->oid_nelem == ot->ot_name->oid_nelem) {
			OID new;

			if ((new = oid_extend(oid, 1)) == NULLOID)
				return NOTOK;
			new->oid_elements[new->oid_nelem - 1] = 0;

			if (v->name)
				free_SNMP_ObjectName(v->name);
			v->name = new;
		} else
			return NOTOK;
		break;

	default:
		return int_SNMP_error__status_genErr;
	}

	if (quantum != lastq) {
		lastq = quantum;

		if (getkmem(nl + N_ICMPSTAT, (caddr_t) icps, sizeof *icps) == NOTOK)
			return generr(offset);
	}

	switch (ifvar) {
	case icmpInMsgs:
		return o_integer(oi, v, icps->icps_badcode
				 + icps->icps_checksum
				 + icps->icps_badlen + icps->icps_inhist[ICMP_UNREACH]
				 + icps->icps_inhist[ICMP_TIMXCEED]
				 + icps->icps_inhist[ICMP_PARAMPROB]
				 + icps->icps_inhist[ICMP_SOURCEQUENCH]
				 + icps->icps_inhist[ICMP_REDIRECT]
				 + icps->icps_inhist[ICMP_ECHO]
				 + icps->icps_inhist[ICMP_ECHOREPLY]
				 + icps->icps_inhist[ICMP_TSTAMP]
				 + icps->icps_inhist[ICMP_TSTAMPREPLY]
				 + icps->icps_inhist[ICMP_MASKREQ]
				 + icps->icps_inhist[ICMP_MASKREPLY]);

	case icmpInErrors:
		return o_integer(oi, v, icps->icps_badcode
				 + icps->icps_checksum + icps->icps_badlen);

	case icmpInDestUnreachs:
		return o_integer(oi, v, icps->icps_inhist[ICMP_UNREACH]);

	case icmpInTimeExcds:
		return o_integer(oi, v, icps->icps_inhist[ICMP_TIMXCEED]);

	case icmpInParmProbs:
		return o_integer(oi, v, icps->icps_inhist[ICMP_PARAMPROB]);

	case icmpInSrcQuenchs:
		return o_integer(oi, v, icps->icps_inhist[ICMP_SOURCEQUENCH]);

	case icmpInRedirects:
		return o_integer(oi, v, icps->icps_inhist[ICMP_REDIRECT]);

	case icmpInEchos:
		return o_integer(oi, v, icps->icps_inhist[ICMP_ECHO]);

	case icmpInEchoReps:
		return o_integer(oi, v, icps->icps_inhist[ICMP_ECHOREPLY]);

	case icmpInTimestamps:
		return o_integer(oi, v, icps->icps_inhist[ICMP_TSTAMP]);

	case icmpInTimestampReps:
		return o_integer(oi, v, icps->icps_inhist[ICMP_TSTAMPREPLY]);

	case icmpInAddrMasks:
		return o_integer(oi, v, icps->icps_inhist[ICMP_MASKREQ]);

	case icmpInAddrMaskReps:
		return o_integer(oi, v, icps->icps_inhist[ICMP_MASKREPLY]);

	case icmpOutMsgs:
		return o_integer(oi, v, icps->icps_error
				 + icps->icps_reflect + icps->icps_outhist[ICMP_UNREACH]
				 + icps->icps_outhist[ICMP_TIMXCEED]
				 + icps->icps_outhist[ICMP_PARAMPROB]
				 + icps->icps_outhist[ICMP_SOURCEQUENCH]
				 + icps->icps_outhist[ICMP_REDIRECT]
				 + icps->icps_outhist[ICMP_ECHO]
				 + icps->icps_outhist[ICMP_ECHOREPLY]
				 + icps->icps_outhist[ICMP_TSTAMP]
				 + icps->icps_outhist[ICMP_TSTAMPREPLY]
				 + icps->icps_outhist[ICMP_MASKREQ]
				 + icps->icps_outhist[ICMP_MASKREPLY]);

	case icmpOutErrors:
		return o_integer(oi, v, icps->icps_error);

	case icmpOutDestUnreachs:
		return o_integer(oi, v, icps->icps_outhist[ICMP_UNREACH]);

	case icmpOutTimeExcds:
		return o_integer(oi, v, icps->icps_outhist[ICMP_TIMXCEED]);

	case icmpOutParmProbs:
		return o_integer(oi, v, icps->icps_outhist[ICMP_PARAMPROB]);

	case icmpOutSrcQuenchs:
		return o_integer(oi, v, icps->icps_outhist[ICMP_SOURCEQUENCH]);

	case icmpOutRedirects:
		return o_integer(oi, v, icps->icps_outhist[ICMP_REDIRECT]);

	case icmpOutEchos:
		return o_integer(oi, v, icps->icps_outhist[ICMP_ECHO]);

	case icmpOutEchoReps:
		return o_integer(oi, v, icps->icps_outhist[ICMP_ECHOREPLY]);

	case icmpOutTimestamps:
		return o_integer(oi, v, icps->icps_outhist[ICMP_TSTAMP]);

	case icmpOutTimestampReps:
		return o_integer(oi, v, icps->icps_outhist[ICMP_TSTAMPREPLY]);

	case icmpOutAddrMasks:
		return o_integer(oi, v, icps->icps_outhist[ICMP_MASKREQ]);

	case icmpOutAddrMaskReps:
		return o_integer(oi, v, icps->icps_outhist[ICMP_MASKREPLY]);

	default:
		return int_SNMP_error__status_noSuchName;
	}
}

/*  */

init_icmp()
{
	register OT ot;

	if (ot = text2obj("icmpInMsgs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInMsgs;
	if (ot = text2obj("icmpInErrors"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInErrors;
	if (ot = text2obj("icmpInDestUnreachs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInDestUnreachs;
	if (ot = text2obj("icmpInTimeExcds"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInTimeExcds;
	if (ot = text2obj("icmpInParmProbs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInParmProbs;
	if (ot = text2obj("icmpInSrcQuenchs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInSrcQuenchs;
	if (ot = text2obj("icmpInRedirects"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInRedirects;
	if (ot = text2obj("icmpInEchos"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInEchos;
	if (ot = text2obj("icmpInEchoReps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInEchoReps;
	if (ot = text2obj("icmpInTimestamps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInTimestamps;
	if (ot = text2obj("icmpInTimestampReps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInTimestampReps;
	if (ot = text2obj("icmpInAddrMasks"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInAddrMasks;
	if (ot = text2obj("icmpInAddrMaskReps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpInAddrMaskReps;
	if (ot = text2obj("icmpOutMsgs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutMsgs;
	if (ot = text2obj("icmpOutErrors"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutErrors;
	if (ot = text2obj("icmpOutDestUnreachs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutDestUnreachs;
	if (ot = text2obj("icmpOutTimeExcds"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutTimeExcds;
	if (ot = text2obj("icmpOutParmProbs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutParmProbs;
	if (ot = text2obj("icmpOutSrcQuenchs"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutSrcQuenchs;
	if (ot = text2obj("icmpOutRedirects"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutRedirects;
	if (ot = text2obj("icmpOutEchos"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutEchos;
	if (ot = text2obj("icmpOutEchoReps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutEchoReps;
	if (ot = text2obj("icmpOutTimestamps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutTimestamps;
	if (ot = text2obj("icmpOutTimestampReps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutTimestampReps;
	if (ot = text2obj("icmpOutAddrMasks"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutAddrMasks;
	if (ot = text2obj("icmpOutAddrMaskReps"))
		ot->ot_getfnx = o_icmp, ot->ot_info = (caddr_t) icmpOutAddrMaskReps;
}
