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

/* ftamtrace.c - FPM: tracing */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamtrace.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamtrace.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamtrace.c,v
 * Revision 9.0  1992/06/16  12:14:55  isode
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
#include "fpkt.h"

/*    DATA */

LLog _ftam_log = {
	"ftam.log", NULLCP, NULLCP, LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE,
	LLOG_NONE, -1, LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *ftam_log = &_ftam_log;

static char *ftam_appls[] = {
	"Abstract-Syntax-Name",
	"Access-Context",
	"Access-Passwords",
	"Access-Request",
	"Account",
	"Action-Result",
	"Activity-Identifier",
	"Application-Entity-Title",
	"Change-Attributes",
	"Charging",
	"Concurrency-Control",
	"Constraint-Set-Name",
	"Create-Attributes",
	"Diagnostic",
	"Document-Type-Name",
	"FADU-Identity",
	"FADU-Lock",
	"Password",
	"Read-Attributes",
	"Select-Attributes",
	"Shared-ASE-Information",
	"State-Result",
	"User-Identity"
};

static int ftam_nappl = sizeof ftam_appls / sizeof ftam_appls[0];

/*    set tracing */

int
FHookRequest(sd, tracing, fti)
	int sd;
	IFP tracing;
	struct FTAMindication *fti;
{
	SBV smask;
	register struct ftamblk *fsb;

	missingP(fti);

	smask = sigioblock();

	ftamPsig(fsb, sd);

	if (fsb->fsb_trace = tracing)
		pe_applist = ftam_appls, pe_maxappl = ftam_nappl;

	(void) sigiomask(smask);

	return OK;
}

/*    user-defined tracing */

/* ARGSUSED */

int
FTraceHook(sd, event, fpdu, pe, rw)
	int sd;
	char *event, *fpdu;
	PE pe;
	int rw;
{
	if (event)
		LLOG(ftam_log, LLOG_ALL, ("%s %s", rw > 0 ? "event"
					  : rw == 0 ? "action" : "exception", event));

	if (pe)
		pvpdu(ftam_log, print_FTAM_PDU_P, pe, fpdu ? fpdu : "FPDU", rw);

	(void) ll_sync(ftam_log);
}
