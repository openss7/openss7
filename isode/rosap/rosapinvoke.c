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

/* rosapinvoke.c - ROPM: invoke */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/rosapinvoke.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/rosapinvoke.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: rosapinvoke.c,v
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
#include <signal.h>
#include "ROS-types.h"
#include "ropkt.h"

static int RoInvokeRequestAux();

/*    RO-INVOKE.REQUEST */

int
RoInvokeRequest(sd, op, class, args, invokeID, linkedID, priority, roi)
	int sd;
	int op, class, invokeID, *linkedID, priority;
	PE args;
	struct RoSAPindication *roi;
{
	SBV smask;
	int result;
	register struct assocblk *acb;

	switch (class) {
	case ROS_SYNC:
	case ROS_ASYNC:
		break;

	default:
		return rosaplose(roi, ROS_PARAMETER, NULLCP, "bad value for class parameter");
	}
	missingP(roi);

	smask = sigioblock();

	rosapPsig(acb, sd);

	result = RoInvokeRequestAux(acb, op, class, args, invokeID, linkedID, priority, roi);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
RoInvokeRequestAux(acb, op, class, args, invokeID, linkedID, priority, roi)
	register struct assocblk *acb;
	int op, class, invokeID, *linkedID, priority;
	PE args;
	struct RoSAPindication *roi;
{
	PE pe;

	struct type_ROS_ROSEapdus papdu;
	struct type_ROS_ROIVapdu piv;
	struct type_ROS_InvokeIDType pidt;
	struct type_ROS_Operation prop;

	if (!(acb->acb_flags & ACB_INIT) && (acb->acb_flags & ACB_ROS))
		return rosaplose(roi, ROS_OPERATION, NULLCP, "not initiator");
	if (!(acb->acb_flags & ACB_ACS)) {
		missingP(args);
		if (linkedID)
			return rosaplose(roi, ROS_OPERATION, NULLCP,
					 "linked operations not permitted");
	}

	if (acb->acb_ready && !(acb->acb_flags & ACB_TURN)
	    && (*acb->acb_ready) (acb, priority, roi) == NOTOK)
		return NOTOK;

	if ((acb->acb_flags & ACB_ACS) == 0) {	/* want OPDU */
		struct type_ROS_OPDU opdu;
		struct type_ROS_Invoke s_invoke;

		s_invoke.invokeID = invokeID;
		s_invoke.element_ROS_2 = &prop;
		prop.parm = op;
		s_invoke.argument = args;
		opdu.offset = type_ROS_OPDU_1;	/* ROS-Invoke */
		opdu.un.choice_ROS_8 = &s_invoke;
		if (encode_ROS_OPDU(&pe, 1, 0, NULLCP, &opdu) == NOTOK)
			return NOTOK;

	} else {
		if (linkedID) {
			piv.optionals = opt_ROS_ROIVapdu_linked__ID;
			piv.linked__ID = *linkedID;
		} else
			piv.optionals = 0;

		piv.invokeID = &pidt;
		pidt.parm = invokeID;
		piv.operation__value = &prop;
		prop.parm = op;
		piv.argument = args;
		papdu.offset = type_ROS_ROSEapdus_roiv__apdu;
		papdu.un.roiv__apdu = &piv;

		if (encode_ROS_ROSEapdus(&pe, 1, 0, NULLCP, &papdu) == NOTOK)
			return NOTOK;
	}

	if ((*acb->acb_putosdu) (acb, pe, args, priority, roi) == NOTOK)
		return NOTOK;

	return (class == ROS_SYNC ? (*acb->acb_rowaitrequest) (acb, &invokeID, NOTOK, roi)
		: OK);
}
