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

/* ssaprelease2.c - SPM: respond to release */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssaprelease2.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssaprelease2.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssaprelease2.c,v
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

static int SRelResponseAux();

/*    S-RELEASE.RESPONSE */

int
SRelResponse(sd, status, data, cc, si)
	int sd;
	int status, cc;
	char *data;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(si);

	smask = sigioblock();

	ssapFsig(sb, sd);
	toomuchP(sb, data, cc, SR_SIZE, "release");

	result = SRelResponseAux(sb, status, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*    S-RELEASE.RESPONSE */

static int
SRelResponseAux(sb, status, data, cc, si)
	register struct ssapblk *sb;
	int status, cc;
	char *data;
	struct SSAPindication *si;
{
	int code, result;
	register struct ssapkt *s;

	switch (status) {
	case SC_ACCEPT:
		code = SPDU_DN;
		break;

	case SC_REJECTED:
		if (!(sb->sb_requirements & SR_NEGOTIATED))
			return ssaplose(si, SC_OPERATION, NULLCP,
					"negotiated release service unavailable");
		if (!(sb->sb_requirements & SR_RLS_EXISTS))
			return ssaplose(si, SC_OPERATION, NULLCP, "release token unavailable");
		if (sb->sb_owned & ST_RLS_TOKEN)
			return ssaplose(si, SC_OPERATION, NULLCP, "release token owned by you");

		code = SPDU_NF;
		break;

	default:
		return ssaplose(si, SC_PARAMETER, NULLCP, "invalid value for status parameter");
	}

	if ((s = newspkt(code)) == NULL)
		return ssaplose(si, SC_CONGEST, NULLCP, "out of memory");

	if (cc > 0) {
		s->s_mask |= SMASK_UDATA_PGI;
		s->s_udata = data, s->s_ulen = cc;
	} else
		s->s_udata = NULL, s->s_ulen = 0;
	result = spkt2sd(s, sb->sb_fd, 0, si);
	s->s_mask &= ~SMASK_UDATA_PGI;
	s->s_udata = NULL, s->s_ulen = 0;

	freespkt(s);
	if (result == NOTOK || code == SPDU_DN)
		freesblk(sb);
	else
		sb->sb_flags &= ~SB_FINN;

	return result;
}
