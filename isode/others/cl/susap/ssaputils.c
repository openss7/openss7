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

/* ssaputils.c - comon session utilities */

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

/*    DATA */

#ifdef HULA

static int su_once_only = 0;
static struct ssapblk susapque;
static struct ssapblk *SuHead = &susapque;

/*    INTERNAL */

struct ssapblk *
newsublk()
{
	register struct ssapblk *sb;

	sb = (struct ssapblk *) calloc(1, sizeof *sb);
	if (sb == NULL)
		return NULL;

	sb->sb_fd = NOTOK;
	sb->sb_qbuf.qb_forw = sb->sb_qbuf.qb_back = &sb->sb_qbuf;
	sb->sb_pr = SPDU_PR;

	if (su_once_only == 0) {
		SuHead->sb_forw = SuHead->sb_back = SuHead;
		su_once_only++;
	}

	insque(sb, SuHead->sb_back);

	return sb;
}

int
freesublk(sb)
	register struct ssapblk *sb;
{
	register struct qbuf *qb, *qp;

	if (sb == NULL)
		return;

	if (sb->sb_retry) {
		sb->sb_retry->s_mask &= ~SMASK_UDATA_PGI;
		sb->sb_retry->s_udata = NULL, sb->sb_retry->s_ulen = 0;
		freespkt(sb->sb_retry);
	}

	if (sb->sb_xspdu)
		freespkt(sb->sb_xspdu);
	if (sb->sb_spdu)
		freespkt(sb->sb_spdu);
	for (qb = sb->sb_qbuf.qb_forw; qb != &sb->sb_qbuf; qb = qp) {
		qp = qb->qb_forw;
		remque(qb);

		free((char *) qb);
	}

	remque(sb);

	free((char *) sb);
}

/*  */

struct ssapblk *
findsublk(sd)
	register int sd;
{
	register struct ssapblk *sb;

	if (su_once_only == 0)
		return NULL;

	for (sb = SuHead->sb_forw; sb != SuHead; sb = sb->sb_forw)
		if (sb->sb_fd == sd)
			return sb;

	return NULL;
}

#endif
