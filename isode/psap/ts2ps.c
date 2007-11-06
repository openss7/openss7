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

/* ts2ps.c - TSDU-backed abstraction for PStreams
 		(really just a refinement of datagram-backed PStreams) */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/ts2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/ts2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: ts2ps.c,v
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
#include "tsap.h"
#include "tailor.h"

/*  */

int
ts_read(fd, q)
	int fd;
	struct qbuf **q;
{
	register struct qbuf *qb;
	struct TSAPdata txs;
	register struct TSAPdata *tx = &txs;
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td = &tds;

	if (TReadRequest(fd, tx, NOTOK, td) == NOTOK) {
		if (td->td_reason == DR_NORMAL) {
			*q = NULL;
			return OK;
		}

		SLOG(psap_log, LLOG_EXCEPTIONS, NULLCP,
		     (td->td_cc > 0 ? "ts_read: [%s] %*.*s" : "ts_read: [%s]",
		      TErrString(td->td_reason), td->td_cc, td->td_cc, td->td_data));

		return NOTOK;
	}

	qb = &tx->tx_qbuf;
	if (qb->qb_forw->qb_forw != qb && qb_pullup(qb) == NOTOK) {
		SLOG(psap_log, LLOG_EXCEPTIONS, NULLCP, ("ts_read: qb_pullup fails"));
		TXFREE(tx);

		return NOTOK;
	}

	remque(qb = tx->tx_qbuf.qb_forw);
	qb->qb_forw = qb->qb_back = qb;

	*q = qb;

	TXFREE(tx);

	return qb->qb_len;
}

int
ts_write(fd, qb)
	int fd;
	register struct qbuf *qb;
{
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td = &tds;

	if (TDataRequest(fd, qb->qb_data, qb->qb_len, td) == NOTOK) {
		SLOG(psap_log, LLOG_EXCEPTIONS, NULLCP,
		     (td->td_cc > 0 ? "ts_write: [%s] %*.*s" : "ts_write: [%s]",
		      TErrString(td->td_reason), td->td_cc, td->td_cc, td->td_data));

		return NOTOK;
	}

	return qb->qb_len;
}
