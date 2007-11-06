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

/* ssapreport.c - SPM: exception reports */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapreport.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapreport.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapreport.c,v
 * Revision 9.0  1992/06/16  12:39:41  isode
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
#include "spkt.h"

static int SUReportRequestAux();

/*    S-U-EXCEPTION-REPORT.REQUEST */

int
SUReportRequest(sd, reason, data, cc, si)
	int sd;
	int reason;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	if (!(SP_OK(reason)))
		return ssaplose(si, SC_PARAMETER, NULLCP, "invalid reason");
	missingP(si);

	smask = sigioblock();

	ssapPsig(sb, sd);
	toomuchP(sb, data, cc, SP_SIZE, "report");

	result = SUReportRequestAux(sb, reason, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SUReportRequestAux(sb, reason, data, cc, si)
	register struct ssapblk *sb;
	int reason;
	char *data;
	int cc;
	register struct SSAPindication *si;
{
	int result;

	if (!(sb->sb_requirements & SR_EXCEPTIONS))
		return ssaplose(si, SC_OPERATION, NULLCP, "exceptions service unavailable");
	if (!(sb->sb_requirements & SR_DAT_EXISTS))
		return ssaplose(si, SC_OPERATION, NULLCP, "data token not available");
	if (sb->sb_owned & ST_DAT_TOKEN)
		return ssaplose(si, SC_OPERATION, NULLCP, "data token owned by you");
	if ((sb->sb_requirements & SR_ACTIVITY)
	    && !(sb->sb_flags & SB_Vact))
		return ssaplose(si, SC_OPERATION, NULLCP, "no activity in progress");

	if ((result = SWriteRequestAux(sb, SPDU_ED, data, cc, reason, 0L, 0,
				       NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk(sb);
	else
		sb->sb_flags |= SB_ED;

	return result;
}
