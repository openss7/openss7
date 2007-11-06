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

/* psaprelease2.c - PPM: respond to release */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap2/RCS/psaprelease2.c,v 9.0 1992/06/16 12:29:42 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap2/RCS/psaprelease2.c,v 9.0 1992/06/16 12:29:42 isode Rel
 *
 *
 * Log: psaprelease2.c,v
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

/*    P-RELEASE.RESPONSE */

int
PRelResponse(sd, status, data, ndata, pi)
	int sd;
	int status;
	PE *data;
	int ndata;
	struct PSAPindication *pi;
{
	SBV smask;
	int len, result;
	char *base, *realbase;
	register struct psapblk *pb;
	struct SSAPindication sis;
	register struct SSAPabort *sa = &sis.si_abort;

	toomuchP(data, ndata, NPDATA, "release");
	missingP(pi);

	smask = sigioblock();

	psapFsig(pb, sd);

	switch (result = info2ssdu(pb, pi, data, ndata, &realbase, &base, &len,
				   "P-RELEASE user-data", PPDU_NONE)) {
	case NOTOK:
		goto out2;

	case OK:
	default:
		break;

	case DONE:
		result = NOTOK;
		goto out1;
	}

	if ((result = SRelResponse(pb->pb_fd, status, base, len, &sis))
	    == NOTOK)
		if (SC_FATAL(sa->sa_reason)) {
			(void) ss2pslose(pb, pi, "SRelResponse", sa);
			goto out2;
		} else {
			(void) ss2pslose(NULLPB, pi, "SRelResponse", sa);
			goto out1;
		}

	if (status == SC_ACCEPT)
		pb->pb_fd = NOTOK;
	else
		pb->pb_flags &= ~PB_FINN;

	result = OK;

      out2:;
	if (result == NOTOK || status == SC_ACCEPT)
		freepblk(pb);
      out1:;
	if (base)
		free(base);

	(void) sigiomask(smask);

	return result;
}
