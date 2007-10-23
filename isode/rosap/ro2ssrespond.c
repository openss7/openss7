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

/* ro2ssrespond.c - responder */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/ro2ssrespond.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/ro2ssrespond.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: ro2ssrespond.c,v
 * Revision 9.0  1992/06/16  12:37:02  isode
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

/* LINTLIBRARY */

#include <stdio.h>
#include "../acsap/OACS-types.h"
#include "ropkt.h"
#include "tailor.h"
#include "internet.h"

/*    RO-BEGIN.INDICATION */

int
RoInit(vecp, vec, ros, roi)
	int vecp;
	char **vec;
	struct RoSAPstart *ros;
	struct RoSAPindication *roi;
{
	int result;
	register struct assocblk *acb;
	register PE pe;
	struct SSAPref ref;
	struct SSAPstart sss;
	register struct SSAPstart *ss = &sss;
	struct SSAPindication sis;
	register struct SSAPindication *si = &sis;
	register struct SSAPabort *sa = &si->si_abort;
	struct type_OACS_PConnect *pconn;

	isodetailor(NULLCP, 0);

	missingP(vec);
	missingP(ros);
	missingP(roi);

	if ((acb = newacblk()) == NULL)
		return rosaplose(roi, ROS_CONGEST, NULLCP, "out of memory");
	acb->acb_flags |= ACB_ROS;
	(void) RoSService(acb, roi);

	if (SInit(vecp, vec, ss, si) == NOTOK) {
		(void) ss2roslose(acb, roi, "SInit", sa);
		goto out1;
	}

	acb->acb_fd = ss->ss_sd;
	acb->acb_uabort = SUAbortRequest;

	acb->acb_requirements = ss->ss_requirements & SR_BCSUBSET;
	if (acb->acb_requirements & SR_DUPLEX)
		acb->acb_requirements = SR_DUPLEX;
	else if (acb->acb_requirements & SR_HALFDUPLEX)
		acb->acb_requirements = SR_HALFDUPLEX;
	else {
		(void) rosaplose(roi, ROS_PROTOCOL, NULLCP,
				 "desired session requirements unavailable");
		goto out2;
	}

	if (acb->acb_requirements & SR_HALFDUPLEX) {
		if (((ss->ss_settings >> ST_DAT_SHIFT) & ST_MASK) == ST_RESP_VALUE) {
			acb->acb_settings = ss->ss_settings;
			acb->acb_flags |= ACB_TURN;
		} else {
			acb->acb_settings = ST_INIT_VALUE << ST_DAT_SHIFT;
			acb->acb_flags &= ~ACB_TURN;
		}
	}

	if ((pe = ssdu2pe(ss->ss_data, ss->ss_cc, NULLCP, &result))
	    == NULLPE) {
		(void) rosaplose(roi, result != PS_ERR_NMEM ? ROS_PROTOCOL
				 : ROS_CONGEST, NULLCP, "%s", ps_error(result));
		goto out2;
	}

	SSFREE(ss);

	if (parse_OACS_PConnect(pe, 1, NULLIP, NULLVP, &pconn) == NOTOK) {
		(void) pylose();
		pe_free(pe);
		goto out2;
	}

	PLOGP(rosap_log, OACS_PConnect, pe, "PConnect", 1);

	bzero((char *) ros, sizeof *ros);
	ros->ros_sd = acb->acb_fd;
	ros->ros_initiator.roa_addr = ss->ss_calling;	/* struct copy */
	ros->ros_port = htons((u_short) pconn->pUserData->applicationProtocol);
	if (pconn->pUserData->member_OACS_2->offset == type_OACS_ConnectionData_open)
		ros->ros_data = pe_expunge(pe, pconn->pUserData->member_OACS_2->un.open);
	else
		ros->ros_data = NULLPE;

	free_OACS_PConnect(pconn);
	return OK;

      out2:;
	bzero((char *) &ref, sizeof ref);
	(void) SConnResponse(acb->acb_fd, &ref, NULLSA, SC_CONGEST, 0, 0,
			     SERIAL_NONE, NULLCP, 0, si);
	acb->acb_fd = NOTOK;

      out1:;
	SSFREE(ss);
	freeacblk(acb);

	return NOTOK;
}

/*    RO-BEGIN.RESPONSE */

int
RoBeginResponse(sd, status, data, roi)
	int sd;
	int status;
	PE data;
	struct RoSAPindication *roi;
{
	int len, result;
	char *base;
	PE pe;
	register struct assocblk *acb;
	struct SSAPref ref;
	struct SSAPindication sis;
	register struct SSAPindication *si = &sis;
	register struct SSAPabort *sa = &si->si_abort;
	struct type_OACS_PAccept paccpt;
	struct type_OACS_DataTransferSyntax dts;
	struct type_OACS_ConnectionData condata;
	struct member_OACS_4 udata;

	if ((acb = findacblk(sd)) == NULL || (acb->acb_flags & ACB_CONN))
		return rosaplose(roi, ROS_PARAMETER, NULLCP, "invalid association descriptor");
	if (!(acb->acb_flags & ACB_ROS))
		return rosaplose(roi, ROS_PARAMETER, NULLCP,
				 "not an association descriptor for ROS");
	switch (status) {
	case ROS_ACCEPT:
		break;

	case ROS_VALIDATE:
	case ROS_BUSY:
		if (data)
			return rosaplose(roi, ROS_PARAMETER, NULLCP,
					 "user data not permitted when refusing association");
		break;

	default:
		return rosaplose(roi, ROS_PARAMETER, NULLCP, "bad value for status parameter");
	}
	missingP(roi);

	bzero((char *) &ref, sizeof ref);	/* ECMA says don't encode this yet */

	base = NULLCP;
	switch (status) {
	case ROS_ACCEPT:
		paccpt.member_OACS_3 = &dts;
		dts.parm = int_OACS_DataTransferSyntax_x409;
		paccpt.pUserData = &udata;
		udata.checkpointSize = 0;
		udata.windowsize = 3;
		udata.member_OACS_5 = &condata;
		condata.offset = type_OACS_ConnectionData_open;
		if (data == NULLPE)
			condata.un.open = pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_NULL);
		else
			condata.un.open = data;

		if (encode_OACS_PAccept(&pe, 1, 0, NULLCP, &paccpt) == NOTOK) {
		      no_mem:;
			(void) rosaplose(roi, ROS_CONGEST, NULLCP, "out of memory");
			goto out1;
		}
		status = SC_ACCEPT;
		break;

	default:
	{
		struct type_OACS_PRefuse prefuse;

		prefuse.parm = status;
		if (encode_OACS_PRefuse(&pe, 1, 0, NULLCP, &prefuse) == NOTOK) {
			goto no_mem;
		}
	}
		status = SC_REJECTED;
		break;
	}

#ifdef	DEBUG
	if (rosap_log->ll_events & LLOG_PDUS)
		if (status == SC_ACCEPT)
			pvpdu(rosap_log, print_OACS_PAccept_P, pe, "PAccept", 0);
		else
			pvpdu(rosap_log, print_OACS_PRefuse_P, pe, "PRefuse", 0);
#endif

	if (pe2ssdu(pe, &base, &len) == NOTOK)
		goto no_mem;

	if (SConnResponse(acb->acb_fd, &ref, NULLSA, status,
			  acb->acb_requirements, acb->acb_settings, SERIAL_NONE,
			  base, len, si) == NOTOK) {
		acb->acb_fd = NOTOK;
		(void) ss2roslose(acb, roi, "SConnResponse", sa);
		goto out3;
	}

	if (status == SC_ACCEPT)
		acb->acb_flags |= ACB_CONN;
	else {
		acb->acb_fd = NOTOK;
		freeacblk(acb);
	}
	result = OK;

      out2:;
	if (pe) {
		if (data)
			(void) pe_extract(pe, data);
		pe_free(pe);
	}
	if (base)
		free(base);

	return result;

      out1:;
	(void) SConnResponse(acb->acb_fd, &ref, NULLSA, SC_CONGEST, 0, 0,
			     SERIAL_NONE, NULLCP, 0, si);
	acb->acb_fd = NOTOK;
      out3:;
	freeacblk(acb);
	result = NOTOK;
	goto out2;
}
