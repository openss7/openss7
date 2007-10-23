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

/* rosapuerror.c - ROPM: error */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/rosapuerror.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/rosapuerror.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: rosapuerror.c,v
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

static int RoErrorRequestAux();

/*    RO-ERROR.REQUEST */

int
RoErrorRequest(sd, invokeID, error, params, priority, roi)
	int sd;
	int invokeID, error, priority;
	PE params;
	struct RoSAPindication *roi;
{
	SBV smask;
	int result;
	register struct assocblk *acb;

	missingP(roi);

	smask = sigioblock();

	rosapPsig(acb, sd);

	result = RoErrorRequestAux(acb, invokeID, error, params, priority, roi);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
RoErrorRequestAux(acb, invokeID, error, params, priority, roi)
	register struct assocblk *acb;
	int invokeID, error, priority;
	PE params;
	struct RoSAPindication *roi;
{
	register PE pe, p;

	if ((acb->acb_flags & ACB_INIT) && (acb->acb_flags & ACB_ROS))
		return rosaplose(roi, ROS_OPERATION, NULLCP, "not responder");
	if (!(acb->acb_flags & ACB_ACS)) {
		missingP(params);
	}

	if (acb->acb_ready && !(acb->acb_flags & ACB_TURN)
	    && (*acb->acb_ready) (acb, priority, roi) == NOTOK)
		return NOTOK;

/* begin Error APDU */
	if ((pe = pe_alloc(PE_CLASS_CONT, PE_FORM_CONS, APDU_ERROR)) == NULLPE
	    || ((acb->acb_flags & ACB_ACS)
		? (p = pe, 0)
		: set_add(pe, p = pe_alloc(PE_CLASS_UNIV, PE_FORM_CONS, PE_CONS_SEQ)) == NOTOK)
	    || seq_add(p, int2prim(invokeID), -1) == NOTOK
	    || seq_add(p, int2prim(error), -1) == NOTOK
	    || (params && seq_add(p, params, -1) == NOTOK)) {
		if (pe) {
			if (params)
				(void) pe_extract(pe, params);
			pe_free(pe);
		}
		freeacblk(acb);
		return rosaplose(roi, ROS_CONGEST, NULLCP, "out of memory");
	}
/* end Error APDU */

	return (*acb->acb_putosdu) (acb, pe, params, priority, roi);
}
