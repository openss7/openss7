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

/* ryopinvoke.c - ROSY: invoke */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosy/RCS/ryopinvoke.c,v 9.0 1992/06/16 12:37:29 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosy/RCS/ryopinvoke.c,v 9.0 1992/06/16 12:37:29 isode Rel
 *
 *
 * Log: ryopinvoke.c,v
 * Revision 9.0  1992/06/16  12:37:29  isode
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
#include "rosy.h"

#ifdef __STDC__
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", #p); \
}
#else
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", "p"); \
}
#endif

/*    INVOKE */

int
RyOpInvoke(sd, ryo, op, in, out, rfx, efx, class, invokeID, linkedID, priority, roi)
	int sd;
	register struct RyOperation *ryo;
	int op, class, invokeID, *linkedID, priority;
	caddr_t in, *out;
	IFP rfx, efx;
	struct RoSAPindication *roi;
{
	int result;
	PE pe;
	register struct opsblk *opb;

	missingP(ryo);
	missingP(roi);

	if (opb = findopblk(sd, invokeID, OPB_INITIATOR))
		return rosaplose(roi, ROS_IP_DUP, NULLCP, NULLCP);

	for (; ryo->ryo_name; ryo++)
		if (ryo->ryo_op == op)
			break;
	if (!ryo->ryo_name)
		return rosaplose(roi, ROS_PARAMETER, NULLCP, "unknown operation code %d", op);

#ifdef PEPSY_DEFINITIONS
	if (ryo->ryo_arg_mod) {
#else
	if (ryo->ryo_arg_encode) {
#endif
#ifdef	notdef
		missingP(in);
#endif
		PY_pepy[0] = 0;
#ifdef PEPSY_DEFINITIONS
		if (enc_f(ryo->ryo_arg_index, ryo->ryo_arg_mod, &pe, 1, NULL, NULLCP, in) == NOTOK)
#else
		if ((*ryo->ryo_arg_encode) (&pe, 1, NULL, NULLCP, in) == NOTOK)
#endif
			return rosaplose(roi, ROS_CONGEST, NULLCP,
					 "error encoding argument for invocation %d operation %s/%d [%s]",
					 invokeID, ryo->ryo_name, ryo->ryo_op, PY_pepy);
	} else {
		if (in)
			return rosaplose(roi, ROS_PARAMETER, NULLCP,
					 "argument not permitted with operation %s/%d",
					 ryo->ryo_name, ryo->ryo_op);

		pe = NULLPE;
	}

	if (ryo->ryo_result || ryo->ryo_errors) {
		if (out) {
			if (rfx || efx)
				return rosaplose(roi, ROS_PARAMETER, NULLCP,
						 "out is exclusive with rfx/efx parameters");
		} else {
			if (ryo->ryo_result) {
				missingP(rfx);
			}
			missingP(efx);
		}

		if ((opb = newopblk(sd, invokeID)) == NULLOPB) {
			if (pe)
				pe_free(pe);
			return rosaplose(roi, ROS_CONGEST, NULLCP, NULLCP);
		}

		opb->opb_ryo = ryo;
		opb->opb_resfnx = rfx, opb->opb_errfnx = efx;
	} else {
		if (class != ROS_ASYNC) {
			if (pe)
				pe_free(pe);
			return rosaplose(roi, ROS_PARAMETER, NULLCP,
					 "ASYNC class must be used with operation %s/%d",
					 ryo->ryo_name, ryo->ryo_op);
		}

		opb = NULLOPB;
	}

	result = RoInvokeRequest(sd, ryo->ryo_op, class, pe, invokeID, linkedID, priority, roi);

	if (pe)
		pe_free(pe);

	switch (result) {
	case NOTOK:
	case DONE:
		break;

	case OK:
		if (class == ROS_ASYNC)
			return OK;
		return RyWaitAux(sd, opb, out, NOTOK, roi);

	default:
		result = rosaplose(roi, ROS_PROTOCOL, NULLCP,
				   "unknown return from RoInvokeRequest=%d", result);
		break;
	}

	freeopblk(opb);

	return result;
}
