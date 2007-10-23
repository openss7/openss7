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

/* psapactivity.c - PPM: activities */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap2/RCS/psapactivity.c,v 9.0 1992/06/16 12:29:42 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap2/RCS/psapactivity.c,v 9.0 1992/06/16 12:29:42 isode Rel
 *
 *
 * Log: psapactivity.c,v
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

/*    P-CONTROL-GIVE.REQUEST */

int
PGControlRequest(sd, pi)
	int sd;
	struct PSAPindication *pi;
{
	SBV smask;
	int result;
	register struct psapblk *pb;
	struct SSAPindication sis;
	register struct SSAPabort *sa = &sis.si_abort;

	missingP(pi);

	smask = sigioblock();

	psapPsig(pb, sd);

	if ((result = SGControlRequest(sd, &sis)) == NOTOK)
		if (SC_FATAL(sa->sa_reason))
			(void) ss2pslose(pb, pi, "SGControlRequest", sa);
		else {
			(void) ss2pslose(NULLPB, pi, "SGControlRequest", sa);
			goto out1;
	} else
		pb->pb_owned = 0;

	if (result == NOTOK)
		freepblk(pb);
      out1:;
	(void) sigiomask(smask);

	return result;
}

/*    P-ACTIVITY-START.REQUEST */

int
PActStartRequest(sd, id, data, ndata, pi)
	int sd;
	struct SSAPactid *id;
	int ndata;
	PE *data;
	struct PSAPindication *pi;
{
	SBV smask;
	int len, result;
	char *base, *realbase;
	register struct psapblk *pb;
	struct SSAPindication sis;
	register struct SSAPabort *sa = &sis.si_abort;

	toomuchP(data, ndata, NPDATA, "activity start");
	missingP(pi);

	smask = sigioblock();

	psapPsig(pb, sd);

	if ((result = info2ssdu(pb, pi, data, ndata, &realbase, &base, &len,
				"P-ACTIVITY-START user-data", PPDU_NONE)) != OK)
		goto out2;

	if ((result = SActStartRequest(sd, id, base, len, &sis)) == NOTOK)
		if (SC_FATAL(sa->sa_reason))
			(void) ss2pslose(pb, pi, "SActStartRequest", sa);
		else {
			(void) ss2pslose(NULLPB, pi, "SActStartRequest", sa);
			goto out1;
		}

      out2:;
	if (result == NOTOK)
		freepblk(pb);
	else if (result == DONE)
		result = NOTOK;
      out1:;
	if (realbase)
		free(realbase);
	else if (base)
		free(base);

	(void) sigiomask(smask);

	return result;
}

/*    P-ACTIVITY-RESUME.REQUEST */

int
PActResumeRequest(sd, id, oid, ssn, ref, data, ndata, pi)
	int sd;
	struct SSAPactid *id, *oid;
	int ndata;
	long ssn;
	struct SSAPref *ref;
	PE *data;
	struct PSAPindication *pi;
{
	SBV smask;
	int len, result;
	char *base, *realbase;
	register struct psapblk *pb;
	struct SSAPindication sis;
	register struct SSAPabort *sa = &sis.si_abort;

	toomuchP(data, ndata, NPDATA, "activity resume");
	missingP(pi);

	smask = sigioblock();

	psapPsig(pb, sd);

	if ((result = info2ssdu(pb, pi, data, ndata, &realbase, &base, &len,
				"P-ACTIVITY-RESUME user-data", PPDU_NONE)) != OK)
		goto out2;

	if ((result = SActResumeRequest(sd, id, oid, ssn, ref, base, len, &sis))
	    == NOTOK)
		if (SC_FATAL(sa->sa_reason))
			(void) ss2pslose(pb, pi, "SActResumeRequest", sa);
		else {
			(void) ss2pslose(NULLPB, pi, "SActResumeRequest", sa);
			goto out1;
		}

      out2:;
	if (result == NOTOK)
		freepblk(pb);
	else if (result == DONE)
		result = NOTOK;
      out1:;
	if (realbase)
		free(realbase);
	else if (base)
		free(base);

	(void) sigiomask(smask);

	return result;
}

/*    P-ACTIVITY-{INTERRUPT,DISCARD}.REQUEST */

int
PActIntrRequestAux(sd, reason, pi, sfunc, stype)
	int sd;
	int reason;
	struct PSAPindication *pi;
	char *stype;
	IFP sfunc;
{
	SBV smask;
	int result;
	register struct psapblk *pb;
	struct SSAPindication sis;
	register struct SSAPabort *sa = &sis.si_abort;

	missingP(pi);
	missingP(sfunc);
	missingP(stype);

	smask = sigioblock();

	psapPsig(pb, sd);

	if ((result = (*sfunc) (sd, reason, &sis)) == NOTOK)
		if (SC_FATAL(sa->sa_reason))
			(void) ss2pslose(pb, pi, stype, sa);
		else {
			(void) ss2pslose(NULLPB, pi, stype, sa);
			goto out1;
		}

	if (result == NOTOK)
		freepblk(pb);
      out1:;
	(void) sigiomask(smask);

	return result;
}

/*    P-ACTIVITY-{INTERRUPT,DISCARD}.RESPONSE */

int
PActIntrResponseAux(sd, pi, sfunc, stype)
	int sd;
	struct PSAPindication *pi;
	char *stype;
	IFP sfunc;
{
	SBV smask;
	int result;
	register struct psapblk *pb;
	struct SSAPindication sis;
	register struct SSAPabort *sa = &sis.si_abort;

	missingP(pi);
	missingP(sfunc);
	missingP(stype);

	smask = sigioblock();

	psapPsig(pb, sd);

	if ((result = (*sfunc) (sd, &sis)) == NOTOK)
		if (SC_FATAL(sa->sa_reason))
			(void) ss2pslose(pb, pi, stype, sa);
		else {
			(void) ss2pslose(NULLPB, pi, stype, sa);
			goto out1;
		}

	if (result == NOTOK)
		freepblk(pb);
      out1:;
	(void) sigiomask(smask);

	return result;
}
