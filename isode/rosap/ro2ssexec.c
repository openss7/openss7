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

/* ro2ssexec.c - RTPM: exec */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/ro2ssexec.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/ro2ssexec.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: ro2ssexec.c,v
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
#include "ROS-types.h"
#include "../acsap/OACS-types.h"
#include "ropkt.h"
#include "isoservent.h"
#include "tailor.h"
#include "internet.h"

/*    SERVER only */

int
RoExec(ss, roi, arg1, arg2, hook, setperms)
	struct SSAPstart *ss;
	char *arg1, *arg2;
	struct RoSAPindication *roi;
	IFP hook, setperms;
{
	int result, result2;
	register struct isoservent *is;
	register PE pe;
	struct SSAPref ref;
	struct SSAPindication sis;
	register struct SSAPindication *si = &sis;
	struct type_OACS_PConnect *pcon;

	missingP(ss);
	missingP(roi);
	missingP(arg1);
	missingP(arg2);

	if ((pe = ssdu2pe(ss->ss_data, ss->ss_cc, NULLCP, &result)) == NULLPE
	    || parse_OACS_PConnect(pe, 1, NULLIP, NULLVP, &pcon) == NOTOK) {
		if (pe)
			pe_free(pe);
		if (result == PS_ERR_NMEM) {
		      congest:;
			result = SC_CONGESTION, result2 = ROS_CONGEST;
		} else
			result = SC_REJECTED, result2 = ROS_PROTOCOL;
		goto out;
	}

	PLOGP(rosap_log, OACS_PConnect, pe, "PConnect", 1);

	pe_free(pe);

	if (pcon->pUserData->member_OACS_2->offset != type_OACS_ConnectionData_open) {
		result = SC_REJECTED, result2 = ROS_ADDRESS;
		goto out;
	}
	if (is = getisoserventbyport("rosap",
				     (u_short) htons((u_short) pcon->pUserData->
						     applicationProtocol))) {
		*is->is_tail++ = arg1;
		*is->is_tail++ = arg2;
		*is->is_tail = NULL;
	} else {
		result = SC_REJECTED, result2 = ROS_ADDRESS;
		goto out;
	}

	switch (hook ? (*hook) (is, roi) : OK) {
	case NOTOK:
		return NOTOK;

	case DONE:
		return OK;

	case OK:
		if (setperms)
			(void) (*setperms) (is);
		(void) execv(*is->is_vec, is->is_vec);	/* fall */
		SLOG(rosap_log, LLOG_FATAL, *is->is_vec, ("unable to exec"));
	default:
		goto congest;
	}

      out:;
	SSFREE(ss);

	bzero((char *) &ref, sizeof ref);
	(void) SConnResponse(ss->ss_sd, &ref, NULLSA, result, 0, 0, SERIAL_NONE, NULLCP, 0, si);
	return rosaplose(roi, result2, NULLCP, NULLCP);
}
