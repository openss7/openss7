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

/* rydserror.c - ROSY: return error to invocation */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosy/RCS/rydserror.c,v 9.0 1992/06/16 12:37:29 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosy/RCS/rydserror.c,v 9.0 1992/06/16 12:37:29 isode Rel
 *
 *
 * Log: rydserror.c,v
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
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}
#endif
/*    ERROR */

int
RyDsError(sd, id, err, out, priority, roi)
	int sd;
	int id, err, priority;
	caddr_t out;
	struct RoSAPindication *roi;
{
	int result;
	PE pe;
	register struct opsblk *opb;
	register struct RyError **ryep, *rye;
	register struct RyOperation *ryo;

	missingP(roi);

	if ((opb = findopblk(sd, id, OPB_RESPONDER)) == NULLOPB)
		return rosaplose(roi, ROS_PARAMETER, NULLCP,
				 "invocation %d not in progress on association %d", id, sd);

	ryo = opb->opb_ryo;
	if (!(ryep = ryo->ryo_errors))
		return rosaplose(roi, ROS_PARAMETER, NULLCP,
				 "error not permitted with operation %s/%d",
				 ryo->ryo_name, ryo->ryo_op);

	for (; *ryep; ryep++)
		if ((*ryep)->rye_err == err)
			break;

	if (!(rye = *ryep))
		return rosaplose(roi, ROS_PARAMETER, NULLCP,
				 "error %d not permitted with operation %s/%d",
				 err, ryo->ryo_name, ryo->ryo_op);

#ifdef PEPSY_DEFINITIONS
	if (rye->rye_param_mod) {
#else
	if (rye->rye_param_encode) {
#endif
#ifdef	notdef
		missingP(out);
#endif
		PY_pepy[0] = 0;
#ifdef PEPSY_DEFINITIONS
		if (enc_f(rye->rye_param_index, rye->rye_param_mod, &pe, 1, NULL,
			  NULLCP, out) == NOTOK)
#else
		if ((*rye->rye_param_encode) (&pe, 1, NULL, NULLCP, out) == NOTOK)
#endif
			return rosaplose(roi, ROS_CONGEST, NULLCP,
					 "error encoding parameter for invocation %d having %s/%d [%s]",
					 opb->opb_id, rye->rye_name, rye->rye_err, PY_pepy);
	} else {
		if (out)
			return rosaplose(roi, ROS_PARAMETER, NULLCP,
					 "error parameter not permitted with operation %s/%d",
					 rye->rye_name, rye->rye_err);

		pe = NULLPE;
	}

	if ((result = RoErrorRequest(sd, id, err, pe, priority, roi)) != NOTOK)
		freeopblk(opb);

	if (pe)
		pe_free(pe);

	return result;

}
