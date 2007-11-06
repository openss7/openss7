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

/* ro2ssreleas1.c - initiate release */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/ro2ssreleas1.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/ro2ssreleas1.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: ro2ssreleas1.c,v
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

static int RoEndRequestAux();

/*    RO-END.REQUEST */

int
RoEndRequest(sd, priority, roi)
	int sd;
	int priority;
	struct RoSAPindication *roi;
{
	SBV smask;
	int result;
	register struct assocblk *acb;

	missingP(roi);

	smask = sigioblock();

	rosapPsig(acb, sd);

	result = RoEndRequestAux(acb, priority, roi);

	(void) sigiomask(smask);

	return result;

}

/*  */

static int
RoEndRequestAux(acb, priority, roi)
	register struct assocblk *acb;
	int priority;
	struct RoSAPindication *roi;
{
	int result;
	struct SSAPindication sis;
	register struct SSAPindication *si = &sis;
	register struct SSAPabort *sa = &si->si_abort;
	struct SSAPrelease srs;
	register struct SSAPrelease *sr = &srs;

	if (acb->acb_apdu)	/* ACB_CLOSING tested earlier in rosapPsig */
		return rosaplose(roi, ROS_WAITING, NULLCP, NULLCP);

	if (!(acb->acb_flags & ACB_ROS))
		return rosaplose(roi, ROS_OPERATION, NULLCP,
				 "not an association descriptor for ROS");

	if (!(acb->acb_flags & ACB_INIT))
		return rosaplose(roi, ROS_OPERATION, NULLCP, "not initiator");

	if (acb->acb_ready && !(acb->acb_flags & ACB_TURN)
	    && (*acb->acb_ready) (acb, priority, roi) == NOTOK)
		return NOTOK;

	if (SRelRequest(acb->acb_fd, NULLCP, 0, NOTOK, sr, si) == NOTOK) {
		if (sa->sa_peer)
			return ss2rosabort(acb, sa, roi);

		result = ss2roslose(acb, roi, "SRelRequest", sa);
	} else if (!sr->sr_affirmative)
		result = ropktlose(acb, roi, ROS_PROTOCOL, NULLCP,
				   "other side refused to release connection");
	else {
		acb->acb_fd = NOTOK;
		result = OK;
	}

	freeacblk(acb);

	return result;
}
