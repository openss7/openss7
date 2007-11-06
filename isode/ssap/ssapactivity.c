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

/* ssapactitivity.c - SPM: activities */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapactivity.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapactivity.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapactivity.c,v
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

/*    S-CONTROL-GIVE.REQUEST */

static int SActResumeRequestAux();
static int SActIntrRequestAux();
static int SActIntrResponseAux();
static int SGControlRequestAux();
static int SActStartRequestAux();

int
SGControlRequest(sd, si)
	int sd;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	smask = sigioblock();

	ssapPsig(sb, sd);

	result = SGControlRequestAux(sb, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SGControlRequestAux(sb, si)
	register struct ssapblk *sb;
	register struct SSAPindication *si;
{
	int result;
	register struct ssapkt *s;

	if (SDoActivityAux(sb, si, 1, 1) == NOTOK)
		return NOTOK;

	if (sb->sb_flags & SB_GTC)
		return ssaplose(si, SC_OPERATION, NULLCP, "give control in progress");

	if ((s = newspkt(SPDU_GTC)) == NULL)
		return ssaplose(si, SC_CONGEST, NULLCP, "out of memory");

	if ((result = spkt2sd(s, sb->sb_fd, 0, si)) == NOTOK)
		freesblk(sb);
	else {
		sb->sb_owned = 0;
		sb->sb_flags |= SB_GTC;
	}
	freespkt(s);

	return result;
}

/*    S-ACTIVITY-START.REQUEST */

int
SActStartRequest(sd, id, data, cc, si)
	int sd;
	struct SSAPactid *id;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(id);
	idmuchP(id);
	missingP(si);

	smask = sigioblock();

	ssapPsig(sb, sd);
	toomuchP(sb, data, cc, SV_SIZE, "activity start");

	result = SActStartRequestAux(sb, id, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SActStartRequestAux(sb, id, data, cc, si)
	register struct ssapblk *sb;
	struct SSAPactid *id;
	char *data;
	int cc;
	register struct SSAPindication *si;
{
	int result;

	if (SDoActivityAux(sb, si, 1, 0) == NOTOK)
		return NOTOK;

	if ((result = SWriteRequestAux(sb, SPDU_AS, data, cc, 0, 0L, 0, id,
				       NULLSD, NULLSR, si)) == NOTOK)
		freesblk(sb);
	else {
		sb->sb_V_A = sb->sb_V_M = sb->sb_V_R = 1;
		sb->sb_flags |= SB_Vact;
	}

	return result;
}

/*    S-ACTIVITY-RESUME.REQUEST */

int
SActResumeRequest(sd, id, oid, ssn, ref, data, cc, si)
	int sd;
	struct SSAPactid *id, *oid;
	long ssn;
	struct SSAPref *ref;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(id);
	idmuchP(id);
	missingP(oid);
	idmuchP(oid);
	if (SERIAL_MIN > ssn || ssn > SERIAL_MAX)
		return ssaplose(si, SC_PARAMETER, NULLCP, "invalid serial number");
#ifdef	notdef
	missingP(ref);
#endif
	if (ref)
		refmuchP(ref)
		    missingP(si);

	smask = sigioblock();

	ssapPsig(sb, sd);
	toomuchP(sb, data, cc, SV_SIZE, "activity resume");

	result = SActResumeRequestAux(sb, id, oid, ssn, ref, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SActResumeRequestAux(sb, id, oid, ssn, ref, data, cc, si)
	register struct ssapblk *sb;
	struct SSAPactid *id, *oid;
	long ssn;
	struct SSAPref *ref;
	char *data;
	int cc;
	register struct SSAPindication *si;
{
	int result;

	if (SDoActivityAux(sb, si, 1, 0) == NOTOK)
		return NOTOK;

	if ((result = SWriteRequestAux(sb, SPDU_AR, data, cc, 0, ssn, 0, id,
				       oid, ref, si)) == NOTOK)
		freesblk(sb);
	else {
		sb->sb_V_A = sb->sb_V_M = ssn + 1;
		sb->sb_V_R = 1;
		sb->sb_flags |= SB_Vact;
	}

	return result;
}

/*    S-ACTIVITY-INTERRUPT.REQUEST */

int
SActIntrRequest(sd, reason, si)
	int sd;
	int reason;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	if (!(SP_OK(reason)))
		return ssaplose(si, SC_PARAMETER, NULLCP, "invalid reason");
	missingP(si);

	smask = sigioblock();

	ssapXsig(sb, sd);
	if (sb->sb_flags & SB_MAP) {
		(void) sigsetmask(smask);
		return ssaplose(si, SC_OPERATION, NULLCP, "majorsync in progress");
	}

	result = SActIntrRequestAux(sb, reason, SPDU_AI, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SActIntrRequestAux(sb, reason, type, si)
	register struct ssapblk *sb;
	int reason, type;
	register struct SSAPindication *si;
{
	int result;

	if (!(sb->sb_requirements & SR_ACTIVITY))
		return ssaplose(si, SC_OPERATION, NULLCP,
				"activity management service unavailable");
	if (!(sb->sb_owned & ST_ACT_TOKEN))
		return ssaplose(si, SC_OPERATION, NULLCP, "activity token not owned by you");
	if (!(sb->sb_flags & SB_Vact))
		return ssaplose(si, SC_OPERATION, NULLCP, "no activity in progress");
	if ((sb->sb_flags & SB_RA)
	    && SDoCollideAux(sb->sb_flags & SB_INIT ? 1 : 0,
			     type == SPDU_AI ? SYNC_INTR : SYNC_DISC, 0L,
			     sb->sb_rs, sb->sb_rsn) == NOTOK)
		return ssaplose(si, SC_OPERATION, NULLCP, "resync in progress takes precedence");

	if ((result = SWriteRequestAux(sb, type, NULLCP, 0, reason, 0L, 0,
				       NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk(sb);
	else {
		sb->sb_flags |= SB_AI, sb->sb_flags &= ~(SB_RA | SB_EDACK | SB_ERACK);
		sb->sb_rs = type == SPDU_AI ? SYNC_INTR : SYNC_DISC;
	}

	return result;
}

/*    S-ACTIVITY-INTERRUPT.RESPONSE */

int
SActIntrResponse(sd, si)
	int sd;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(si);

	smask = sigioblock();

	ssapAsig(sb, sd);

	result = SActIntrResponseAux(sb, SPDU_AIA, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SActIntrResponseAux(sb, type, si)
	register struct ssapblk *sb;
	int type;
	register struct SSAPindication *si;
{
	int result;

	if (!(sb->sb_requirements & SR_ACTIVITY))
		return ssaplose(si, SC_OPERATION, NULLCP,
				"activity management service unavailable");
	if (!(sb->sb_flags & SB_Vact))
		return ssaplose(si, SC_OPERATION, NULLCP, "no activity in progress");
	if (!(sb->sb_flags & SB_AIA))
		return ssaplose(si, SC_OPERATION, NULLCP,
				"no activity interrupt/discard in progress");

	if ((result = SWriteRequestAux(sb, type, NULLCP, 0, 0, 0L, 0,
				       NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk(sb);
	else {
		sb->sb_flags &= ~(SB_AIA | SB_Vact);
		sb->sb_owned = 0;
	}

	return result;
}

/*    S-ACTIVITY-DISCARD.REQUEST */

int
SActDiscRequest(sd, reason, si)
	int sd;
	int reason;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	if (!(SP_OK(reason)))
		return ssaplose(si, SC_PARAMETER, NULLCP, "invalid reason");
	missingP(si);

	smask = sigioblock();

	ssapXsig(sb, sd);
	if (sb->sb_flags & SB_MAP) {
		(void) sigsetmask(smask);
		return ssaplose(si, SC_OPERATION, NULLCP, "majorsync in progress");
	}

	result = SActIntrRequestAux(sb, reason, SPDU_AD, si);

	(void) sigiomask(smask);

	return result;
}

/*    S-ACTIVITY-DISCARD.RESPONSE */

int
SActDiscResponse(sd, si)
	int sd;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(si);

	smask = sigioblock();

	ssapAsig(sb, sd);

	result = SActIntrResponseAux(sb, SPDU_ADA, si);

	(void) sigiomask(smask);

	return result;
}

/*    S-ACTIVITY-END.REQUEST */

int
SActEndRequest(sd, ssn, data, cc, si)
	int sd;
	long *ssn;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(ssn);
	missingP(si);

	smask = sigioblock();

	ssapPsig(sb, sd);
	toomuchP(sb, data, cc, SV_SIZE, "activity end");

	result = SMajSyncRequestAux(sb, ssn, data, cc, 0, si);

	(void) sigiomask(smask);

	return result;
}

/*    S-ACTIVITY-END.RESPONSE */

int
SActEndResponse(sd, data, cc, si)
	int sd;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(si);

	smask = sigioblock();

	ssapPsig(sb, sd);
	toomuchP(sb, data, cc, SV_SIZE, "activity end");

	result = SMajSyncResponseAux(sb, data, cc, si);

	(void) sigiomask(smask);

	return result;
}
