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

/* rosapureject.c - ROPM: user reject */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/rosapureject.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/rosapureject.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: rosapureject.c,v
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
#include "ropkt.h"

/*    RO-U-REJECT.REQUEST */

int
RoURejectRequest(sd, invokeID, reason, priority, roi)
	int sd;
	int *invokeID, reason, priority;
	struct RoSAPindication *roi;
{
	SBV smask;
	int result;
	PElementID id;
	register struct assocblk *acb;

	switch (reason) {
	case ROS_IP_DUP:	/* Invoke Problem */
	case ROS_IP_UNRECOG:
	case ROS_IP_MISTYPED:
	case ROS_IP_LIMIT:
		id = REJECT_INVOKE;
		reason -= REJECT_INVOKE_BASE;
		break;

	case ROS_IP_RELEASE:
	case ROS_IP_UNLINKED:
	case ROS_IP_LINKED:
	case ROS_IP_CHILD:
		id = REJECT_COMPLETE;
		reason -= REJECT_INVOKE_BASE;
		break;

	case ROS_RRP_UNRECOG:	/* Return Result Problem */
	case ROS_RRP_UNEXP:
	case ROS_RRP_MISTYPED:
		id = REJECT_RESULT;
		reason -= REJECT_RESULT_BASE;
		break;

	case ROS_REP_UNRECOG:	/* Return Error Problem */
	case ROS_REP_UNEXP:
	case ROS_REP_RECERR:
	case ROS_REP_UNEXPERR:
	case ROS_REP_MISTYPED:
		id = REJECT_ERROR;
		reason -= REJECT_ERROR_BASE;
		break;

	default:
		return rosaplose(roi, ROS_PARAMETER, NULLCP, "bad value for reason parameter");
	}
	missingP(roi);

	smask = sigioblock();

	rosapPsig(acb, sd);

	result = RoURejectRequestAux(acb, invokeID, reason, id, priority, roi);

	(void) sigiomask(smask);

	return result;
}

/*  */

int
RoURejectRequestAux(acb, invokeID, reason, id, priority, roi)
	register struct assocblk *acb;
	int *invokeID, reason, priority;
	PElementID id;
	struct RoSAPindication *roi;
{
	register PE pe, p;

	if (id == REJECT_COMPLETE)
		if (acb->acb_flags & ACB_ACS)
			id = REJECT_INVOKE;
		else
			return rosaplose(roi, ROS_PARAMETER, NULLCP,
					 "bad value for reason parameter");

	if (acb->acb_ready && !(acb->acb_flags & ACB_TURN)
	    && (*acb->acb_ready) (acb, priority, roi) == NOTOK)
		return NOTOK;

/* begin Reject APDU */
	if ((pe = pe_alloc(PE_CLASS_CONT, PE_FORM_CONS, APDU_REJECT)) == NULLPE
	    || ((acb->acb_flags & ACB_ACS)
		? (p = pe, 0)
		: set_add(pe, p = pe_alloc(PE_CLASS_UNIV, PE_FORM_CONS, PE_CONS_SEQ)) == NOTOK)
	    || seq_add(p, invokeID ? int2prim(*invokeID)
		       : pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM,
				  PE_PRIM_NULL), -1) == NOTOK
	    || seq_add(p, num2prim((integer) reason, PE_CLASS_CONT, id), -1)
	    == NOTOK) {
		if (pe)
			pe_free(pe);
		freeacblk(acb);
		return rosaplose(roi, ROS_CONGEST, NULLCP, "out of memory");
	}
/* end Reject APDU */

	return (*acb->acb_putosdu) (acb, pe, NULLPE, priority, roi);
}
