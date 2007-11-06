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

/* psaprelease1.c - PPM: initiate release */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap2/RCS/psaprelease1.c,v 9.0 1992/06/16 12:29:42 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap2/RCS/psaprelease1.c,v 9.0 1992/06/16 12:29:42 isode Rel
 *
 *
 * Log: psaprelease1.c,v
 * Revision 9.0  1992/06/16  12:29:42  isode
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
#include "ppkt.h"

static int PRelRetryRequestAux();

/*    P-RELEASE.REQUEST */

int
PRelRequest(sd, data, ndata, secs, pr, pi)
	int sd;
	PE *data;
	int ndata;
	int secs;
	struct PSAPrelease *pr;
	struct PSAPindication *pi;
{
	SBV smask;
	int result;
	register struct psapblk *pb;

	toomuchP(data, ndata, NPDATA, "release");
	missingP(pr);
	missingP(pi);

	smask = sigioblock();

	psapPsig(pb, sd);

	switch (result = info2ssdu(pb, pi, data, ndata, &pb->pb_realbase,
				   &pb->pb_retry, &pb->pb_len, "P-RELEASE user-data", PPDU_NONE)) {
	case OK:
	default:
		result = PRelRetryRequestAux(pb, secs, pr, pi);
		goto out;

	case NOTOK:
		freepblk(pb), pb = NULLPB;
		break;

	case DONE:
		result = NOTOK;
		break;
	}

	if (pb) {
		if (pb->pb_realbase)
			free(pb->pb_realbase);
		else if (pb->pb_retry)
			free(pb->pb_retry);
		pb->pb_realbase = pb->pb_retry = NULL;
	}

      out:;
	(void) sigiomask(smask);

	return result;
}

/*    P-RELEASE-RETRY.REQUEST (pseudo) */

int
PRelRetryRequest(sd, secs, pr, pi)
	int sd;
	int secs;
	struct PSAPrelease *pr;
	struct PSAPindication *pi;
{
	SBV smask;
	int result;
	register struct psapblk *pb;

	missingP(pr);
	missingP(pi);

	smask = sigioblock();

	if ((pb = findpblk(sd)) == NULL)
		result = psaplose(pi, PC_PARAMETER, NULLCP, "invalid session descriptor");
	else if (!(pb->pb_flags & PB_RELEASE))
		result = psaplose(pi, PC_OPERATION, "release not in progress");
	else
		result = PRelRetryRequestAux(pb, secs, pr, pi);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
PRelRetryRequestAux(pb, secs, pr, pi)
	struct psapblk *pb;
	int secs;
	struct PSAPrelease *pr;
	struct PSAPindication *pi;
{
	int result;
	char *id = pb->pb_flags & PB_RELEASE ? "SRelRetryRequest" : "SRelRequest";
	struct SSAPrelease srs;
	register struct SSAPrelease *sr = &srs;
	struct SSAPindication sis;
	register struct SSAPabort *sa = &sis.si_abort;

	bzero((char *) sr, sizeof *sr);

	if ((result = (pb->pb_flags & PB_RELEASE)
	     ? SRelRetryRequest(pb->pb_fd, secs, sr, &sis)
	     : SRelRequest(pb->pb_fd, pb->pb_retry, pb->pb_len, secs, sr, &sis))
	    == NOTOK) {
		if (sa->sa_reason == SC_TIMER) {
			pb->pb_flags |= PB_RELEASE;

			return ss2pslose(NULLPB, pi, id, sa);
		}

		if (sa->sa_peer) {
			(void) ss2psabort(pb, sa, pi);
			goto out1;
		}
		if (SC_FATAL(sa->sa_reason)) {
			(void) ss2pslose(pb, pi, id, sa);
			goto out2;
		} else {
			(void) ss2pslose(NULLPB, pi, id, sa);
			goto out1;
		}
	}

	bzero((char *) pr, sizeof *pr);

	if ((result = ssdu2info(pb, pi, sr->sr_data, sr->sr_cc, pr->pr_info,
				&pr->pr_ninfo, "P-RELEASE user-data", PPDU_NONE)) == NOTOK)
		goto out2;

	if (pr->pr_affirmative = sr->sr_affirmative) {
		pb->pb_fd = NOTOK;
		result = OK;
	} else
		result = DONE;

      out2:;
	if (result == DONE)
		result = OK;
	else
		freepblk(pb), pb = NULLPB;
      out1:;
	SRFREE(sr);
	if (pb) {
		if (pb->pb_realbase)
			free(pb->pb_realbase);
		else if (pb->pb_retry)
			free(pb->pb_retry);
		pb->pb_realbase = pb->pb_retry = NULL;
	}

	return result;
}
