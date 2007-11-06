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

/* ssapresync2.c - SPM: respond to resyncs */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapresync2.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapresync2.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapresync2.c,v
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

static int SReSyncResponseAux();

/*    S-RESYNCHRONIZE.RESPONSE */

int
SReSyncResponse(sd, ssn, settings, data, cc, si)
	int sd;
	int settings;
	long ssn;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(si);

	smask = sigioblock();

	ssapRsig(sb, sd);
	toomuchP(sb, data, cc, SN_SIZE, "resync");

	result = SReSyncResponseAux(sb, ssn, settings, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SReSyncResponseAux(sb, ssn, settings, data, cc, si)
	register struct ssapblk *sb;
	long ssn;
	int settings;
	char *data;
	int cc;
	register struct SSAPindication *si;
{
	int result;

	if (!(sb->sb_requirements & SR_RESYNC))
		return ssaplose(si, SC_OPERATION, NULLCP, "resynchronize service unavailable");
	if (!(sb->sb_flags & SB_RA))
		return ssaplose(si, SC_OPERATION, NULLCP, "no resync in progress");

	switch (sb->sb_rs) {
	case SYNC_RESTART:
		ssn = sb->sb_rsn;
		break;

	case SYNC_ABANDON:
		ssn = sb->sb_V_A;
		break;

	case SYNC_SET:
		break;
	}

#define	dotoken(requires,shift,bit,type) \
{ \
    if (sb -> sb_requirements & requires) \
	switch (sb -> sb_rsettings & (ST_MASK << shift)) { \
	    dotoken1 (requires,shift,bit,type); \
 \
	    dotoken2 (requires,shift,bit,type); \
	} \
}
#define	dotoken1(requires,shift,bit,type) \
	    case ST_CALL_VALUE << shift: \
		switch (settings & (ST_MASK << shift)) { \
		    case ST_INIT_VALUE << shift: \
			settings &= ~(ST_MASK << shift); \
			settings |= ST_INIT_VALUE << shift; \
			break; \
 \
		    case ST_RESP_VALUE << shift: \
			settings &= ~(ST_MASK << shift); \
			settings |= ST_RESP_VALUE << shift; \
			break; \
 \
		    default: \
			return ssaplose (si, SC_PARAMETER, NULLCP, \
				"improper choice of %s token setting", type); \
		} \
		break;
#define	dotoken2(requires,shift,bit,type) \
	    default: \
		if ((sb -> sb_rsettings & (ST_MASK << shift)) \
			!= (settings & (ST_MASK << shift))) \
		    return ssaplose (si, SC_OPERATION, NULLCP, \
			    "setting of %s token is not your choice"); \
		break;
	dotokens();
#undef	dotoken
#undef	dotoken1
#undef	dotoken2

	if ((result = SWriteRequestAux(sb, SPDU_RA, data, cc, 0, ssn,
				       settings, NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk(sb);
	else {
		sb->sb_flags &= ~SB_RA;
		sb->sb_V_A = sb->sb_V_M = ssn;
		if (sb->sb_rs != SYNC_RESTART)
			sb->sb_V_R = 0;

#define	dotoken(requires,shift,bit,type) \
{ \
	if (sb -> sb_requirements & requires) \
	    switch (settings & (ST_MASK << shift)) { \
		case ST_INIT_VALUE << shift: \
		    if (sb -> sb_flags & SB_INIT) \
			sb -> sb_owned |= bit; \
		    else \
			sb -> sb_owned &= ~bit; \
		    break; \
 \
		case ST_RESP_VALUE << shift: \
		    if (!(sb -> sb_flags & SB_INIT)) \
			sb -> sb_owned |= bit; \
		    else \
			sb -> sb_owned &= ~bit; \
		    break; \
	    } \
}
		dotokens();
#undef	dotoken
	}

	return result;
}
