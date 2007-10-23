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

/* ssapresync.c - SPM: initiate resyncs */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapresync1.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapresync1.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapresync1.c,v
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

static int SReSyncRequestAux();

/*    S-RESYNCHRONIZE.REQUEST */

#define	dotoken(requires,shift,bit,type) \
{ \
    if (sb -> sb_requirements & requires) \
	switch (settings & (ST_MASK << shift)) { \
	    case ST_INIT_VALUE << shift: \
	    case ST_RESP_VALUE << shift: \
	    case ST_CALL_VALUE << shift: \
		break; \
 \
	    default: \
		return ssaplose (si, SC_PARAMETER, NULLCP, \
			"improper choice of %s token setting",type); \
	} \
}

/*  */

int
SReSyncRequest(sd, type, ssn, settings, data, cc, si)
	int sd;
	int type, settings;
	long ssn;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	switch (type) {
	case SYNC_RESTART:
		break;

	case SYNC_ABANDON:
		if (ssn != SERIAL_NONE)
			return ssaplose(si, SC_PARAMETER, NULLCP, "serial number inappropriate");
		break;

	case SYNC_SET:
		if (SERIAL_MIN > ssn || ssn > SERIAL_MAX + 1)
			return ssaplose(si, SC_PARAMETER, NULLCP, "invalid serial number");
		break;

	default:
		return ssaplose(si, SC_PARAMETER, NULLCP, "improper choice of type setting");
	}
	missingP(si);

	smask = sigioblock();

	ssapRsig(sb, sd);
	toomuchP(sb, data, cc, SN_SIZE, "resync");

	result = SReSyncRequestAux(sb, type, ssn, settings, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SReSyncRequestAux(sb, type, ssn, settings, data, cc, si)
	register struct ssapblk *sb;
	int type, settings;
	long ssn;
	char *data;
	int cc;
	register struct SSAPindication *si;
{
	int result;

	if (!(sb->sb_requirements & SR_RESYNC))
		return ssaplose(si, SC_OPERATION, NULLCP, "resynchronize service unavailable");

	if ((sb->sb_requirements & SR_ACTIVITY)
	    && !(sb->sb_flags & SB_Vact))
		return ssaplose(si, SC_OPERATION, NULLCP, "no activity in progress");

	if ((sb->sb_flags & SB_RA)
	    && SDoCollideAux(sb->sb_flags & SB_INIT ? 1 : 0, type, ssn,
			     sb->sb_rs, sb->sb_rsn) == NOTOK)
		return ssaplose(si, SC_OPERATION, NULLCP, "resync in progress takes precedence");

	switch (type) {
	case SYNC_RESTART:
		if (sb->sb_V_M < ssn || ssn < sb->sb_V_R)
			return ssaplose(si, SC_OPERATION, NULLCP,
					"bad choice for resync ssn, should be in [%d..%d]",
					sb->sb_V_R, sb->sb_V_M);
		break;

	case SYNC_ABANDON:
		ssn = sb->sb_V_M;
		break;

	case SYNC_SET:
		break;
	}

	dotokens();

	if ((result = SWriteRequestAux(sb, SPDU_RS, data, cc, type, ssn,
				       settings, NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk(sb);
	else {
		sb->sb_flags |= SB_RS, sb->sb_flags &= ~(SB_RA | SB_EDACK | SB_ERACK);
		sb->sb_rs = type;
		sb->sb_rsn = ssn;
		sb->sb_rsettings = sb->sb_requirements & SR_TOKENS ? settings : 0;
	}

	return result;
}

#undef	dotoken
