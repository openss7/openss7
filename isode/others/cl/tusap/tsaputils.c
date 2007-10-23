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

/* tsaputils.c - common service routines to tsap unit data */

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
#include "tpkt.h"
#include "tsap.h"
#include "tusap.h"
#include "isoservent.h"
#include "tailor.h"

#define	selmask(fd,m,n) \
{ \
    FD_SET (fd, &(m)); \
    if ((fd) >= (n)) \
	(n) = (fd) + 1; \
}

/*    DATA */

#ifdef HULA

static int tu_once_only = 0;
static struct tsapblk tusapque;
static struct tsapblk *TuHead = &tusapque;

#ifndef	SIGPOLL
#ifndef TPid
static int TPid = NOTOK;
#endif
#endif

/*    INTERNAL */

struct tsapblk *
newtublk()
{
	register struct tsapblk *tb;

	tb = (struct tsapblk *) calloc(1, sizeof *tb);
	if (tb == NULL)
		return NULL;

	tb->tb_fd = NOTOK;

	tb->tb_qbuf.qb_forw = tb->tb_qbuf.qb_back = &tb->tb_qbuf;

	if (tu_once_only == 0) {
		TuHead->tb_forw = TuHead->tb_back = TuHead;
		tu_once_only++;
	}

	insque(tb, TuHead->tb_back);

	return tb;
}

freetublk(tb)
	register struct tsapblk *tb;
{
	SBV smask;
	register struct qbuf *qb, *qp;

#ifndef	SIGPOLL
	struct TSAPdisconnect tds;
#endif

	if (tb == NULL)
		return;

	smask = sigioblock();

#ifdef HULA
	if (tb->tb_flags & TB_CLNS) {
		if (*tb->tb_UnitDataClose)
			(*tb->tb_UnitDataClose) (tb->tb_fd);
		tb->tb_fd = NOTOK;
	}
#endif

	if (tb->tb_fd != NOTOK)
		(void) (*tb->tb_closefnx) (tb->tb_fd);

	if (tb->tb_retry)
		freetpkt(tb->tb_retry);

#ifndef	SIGPOLL
	if ((tb->tb_flags & TB_ASYN) && TPid > OK) {
		(void) kill(TPid, SIGTERM);
		TPid = NOTOK;
	}
#endif

	for (qb = tb->tb_qbuf.qb_forw; qb != &tb->tb_qbuf; qb = qp) {
		qp = qb->qb_forw;
		remque(qb);

		free((char *) qb);
	}

	remque(tb);

	free((char *) tb);

#ifndef	SIGPOLL
	for (tb = TuHead->tb_forw; tb != TuHead; tb = tb->tb_forw)
		if (tb->tb_fd != NOTOK && (tb->tb_flags & TB_ASYN)) {
			if (tb->tb_flags & TB_CLNS)
				/* (void) TUnitDataWakeUp (tb); */
				;
			break;
		}
#endif

	(void) sigiomask(smask);
}

/*  */

struct tsapblk *
findtublk(sd)
	register int sd;
{
	register struct tsapblk *tb;

	if (tu_once_only == 0)
		return NULL;

	for (tb = TuHead->tb_forw; tb != TuHead; tb = tb->tb_forw)
		if (tb->tb_fd == sd)
			return tb;

	return NULL;
}

#endif
