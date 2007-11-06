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

/* ssapabort.c - SPM: user abort */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapabort.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapabort.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapabort.c,v
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
#include "tailor.h"

static int SUAbortRequestAux();

/*    S-U-ABORT.REQUEST */

int
SUAbortRequest(sd, data, cc, si)
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

	if ((sb = findsblk(sd)) == NULL) {
		(void) sigiomask(smask);
		return ssaplose(si, SC_PARAMETER, NULLCP, "invalid session descriptor");
	}
	toomuchP(sb, data, cc, SA_SIZE, "abort");

	result = SUAbortRequestAux(sb, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SUAbortRequestAux(sb, data, cc, si)
	register struct ssapblk *sb;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	int result;
	register struct ssapkt *s;
	struct TSAPdata txs;
	register struct TSAPdata *tx = &txs;
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td = &tds;

	sb->sb_flags &= ~(SB_ED | SB_EDACK | SB_ERACK);

	if ((sb->sb_flags & SB_EXPD)
	    && sb->sb_version >= SB_VRSN2 && cc > 9) {
		register struct ssapkt *p;

		if (p = newspkt(SPDU_PR)) {
			p->s_mask |= SMASK_PR_TYPE;
			p->s_pr_type = PR_AB;
			result = spkt2sd(p, sb->sb_fd, 1, si);
			freespkt(p);
			if (result == NOTOK)
				goto out1;
		}
	}

	if ((s = newspkt(SPDU_AB)) == NULL) {
		result = ssaplose(si, SC_CONGEST, NULLCP, "out of memory");
		goto out1;
	}

	s->s_mask |= SMASK_SPDU_AB | SMASK_AB_DISC;
	s->s_ab_disconnect = AB_DISC_RELEASE | AB_DISC_USER;

	if (cc > 0) {
		s->s_mask |= SMASK_UDATA_PGI;
		s->s_udata = data, s->s_ulen = cc;
	} else
		s->s_udata = NULL, s->s_ulen = 0;
	result = spkt2sd(s, sb->sb_fd, sb->sb_flags & SB_EXPD ? 1 : 0, si);
	s->s_mask &= ~SMASK_UDATA_PGI;
	s->s_udata = NULL, s->s_ulen = 0;

	freespkt(s);
	if (result == NOTOK)
		goto out1;

	if (ses_ab_timer >= 0)
		switch (TReadRequest(sb->sb_fd, tx, ses_ab_timer, td)) {
		case OK:
		default:	/* should be an ABORT ACCEPT, but who cares? */
			TXFREE(tx);
			break;

		case NOTOK:
			if (td->td_reason != DR_TIMER)
				sb->sb_fd = NOTOK;
			break;
		}
	result = OK;

      out1:;
	freesblk(sb);

	return result;
}
