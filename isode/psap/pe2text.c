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

/* pe2text.c - write a PE thru a debug filter */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/pe2text.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/pe2text.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: pe2text.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
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
#include "psap.h"
#include "logger.h"

/*  */

/* logfile-backed abstract for PStreams */

/* ARGSUSED */

static int
ll_pswrite(ps, data, n, in_line)
	PS ps;
	PElementData data;
	PElementLen n;
	int in_line;
{
	register LLog *lp = (LLog *) ps->ps_addr;

	if (lp->ll_stat & LLOGTTY) {
		(void) fflush(stdout);

		(void) fwrite((char *) data, sizeof *data, (int) n, stderr);
		(void) fflush(stderr);
	}

	if (lp->ll_fd == NOTOK) {
		if ((lp->ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
			return ((int) n);
		if (ll_open(lp) == NOTOK)
			return NOTOK;
	} else if (ll_check(lp) == NOTOK)
		return NOTOK;

	return write(lp->ll_fd, (char *) data, (int) n);
}

/*  */

static int
ll_psopen(ps)
	register PS ps;
{
	ps->ps_writeP = ll_pswrite;

	return OK;
}

#define	ll_psetup(ps, lp)	((ps) -> ps_addr = (caddr_t) (lp), OK)

/*  */

void
pe2text(lp, pe, rw, cc)
	register LLog *lp;
	register PE pe;
	int rw, cc;
{
	register char *bp;
	char buffer[BUFSIZ];
	register PS ps;

	bp = buffer;
	(void) sprintf(bp, "%s PE", rw ? "read" : "wrote");
	bp += strlen(bp);
	if (pe->pe_context != PE_DFLT_CTX) {
		(void) sprintf(bp, ", context %d", pe->pe_context);
		bp += strlen(bp);
	}
	if (cc != NOTOK) {
		(void) sprintf(bp, ", length %d", cc);
		bp += strlen(bp);
	}
	LLOG(lp, LLOG_ALL, ("%s", buffer));

	if ((ps = ps_alloc(ll_psopen)) != NULLPS) {
		if (ll_psetup(ps, lp) != NOTOK)
			(void) pe2pl(ps, pe);

		ps_free(ps);
	}

	(void) ll_printf(lp, "-------\n");

	(void) ll_sync(lp);
}
