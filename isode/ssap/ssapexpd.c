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

/* ssapexpd.c - SPM: write expedited data */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapexpd.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapexpd.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapexpd.c,v
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

/*    S-EXPEDITED-DATA.REQUEST */

static int SExpdRequestAux();

int
SExpdRequest(sd, data, cc, si)
	int sd;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	SBV smask;
	int result;
	register struct ssapblk *sb;

	missingP(data);
	if (cc > SX_EXSIZE)
		return ssaplose(si, SC_PARAMETER, NULLCP,
				"too much expedited user data, %d octets", cc);
	missingP(si);

	smask = sigioblock();

	ssapPsig(sb, sd);

	result = SExpdRequestAux(sb, data, cc, si);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
SExpdRequestAux(sb, data, cc, si)
	register struct ssapblk *sb;
	char *data;
	int cc;
	struct SSAPindication *si;
{
	int result;
	register struct ssapkt *s;

	if (!(sb->sb_requirements & SR_EXPEDITED))
		return ssaplose(si, SC_OPERATION, NULLCP, "expedited data service unavailable");

	if ((s = newspkt(SPDU_EX)) == NULL)
		return ssaplose(si, SC_CONGEST, NULLCP, "out of memory");

	s->s_udata = data, s->s_ulen = cc;
	result = spkt2sd(s, sb->sb_fd, 1, si);
	s->s_udata = NULL, s->s_ulen = 0;

	freespkt(s);

	if (result == NOTOK)
		freesblk(sb);

	return result;
}
