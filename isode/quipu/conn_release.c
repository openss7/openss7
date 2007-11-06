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

/* conn_release.c - normal association release */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/conn_release.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/conn_release.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: conn_release.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include "quipu/dsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern LLog *log_dsap;
extern time_t timenow;

struct connection *conn_alloc();
void conn_free();
void ds_log();

conn_release(conn)
	struct connection *conn;
{
	int result;
	struct DSAPrelease dr_s;
	struct DSAPrelease *dr = &(dr_s);
	struct DSAPindication di_s;
	struct DSAPindication *di = &(di_s);
	struct DSAPabort *da = &(di->di_abort);

	DLOG(log_dsap, LLOG_TRACE, ("conn_release(%d)", conn->cn_ad));

	result = DUnBindRequest(conn->cn_ad, OK, dr, di);

	switch (result) {
	case NOTOK:
		do_ds_unbind(conn);
		ds_log(da, "A-RELEASE.REQUEST", conn->cn_ad);
		break;
	case OK:
		DLOG(log_dsap, LLOG_TRACE, ("conn_release: dr_affirmative = %d",
					    dr->dr_affirmative));
		if (!dr->dr_affirmative) {
			if ((conn->cn_last_release == conn->cn_last_used)
			    && (conn->cn_initiator)) {
				LLOG(log_dsap, LLOG_EXCEPTIONS,
				     ("conn_release rejected again without activity - Aborting %d",
				      conn->cn_ad));
				if (DUAbortRequest(conn->cn_ad, di) != OK)
					force_close(conn->cn_ad, di);
				do_ds_unbind(conn);
			} else {
				LLOG(log_dsap, LLOG_EXCEPTIONS,
				     ("conn_release rejected - continuing with association %d",
				      conn->cn_ad));
				conn->cn_last_release = conn->cn_last_used = timenow;
				return NOTOK;
			}
		} else {
			do_ds_unbind(conn);
			DLOG(log_dsap, LLOG_TRACE, ("conn_release: Conn finished!"));
		}
		break;
	case DONE:
		DLOG(log_dsap, LLOG_TRACE, ("Waiting for release"));
		conn->cn_state = CN_CLOSING;
		conn->cn_last_release = conn->cn_last_used = timenow;
		return NOTOK;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Unexpected return from DUnBindRequest"));
		return NOTOK;
	}

	DLOG(log_dsap, LLOG_DEBUG, ("conn_release calling conn_extract"));
	conn_extract(conn);
	return OK;
}

conn_release_retry(conn)
	struct connection *conn;
{
	int result;
	struct DSAPrelease dr_s;
	struct DSAPrelease *dr = &(dr_s);
	struct DSAPindication di_s;
	struct DSAPindication *di = &(di_s);
	struct DSAPabort *da = &(di->di_abort);

	DLOG(log_dsap, LLOG_TRACE, ("conn_release retry (%d)", conn->cn_ad));

	result = DUnBindRetry(conn->cn_ad, OK, dr, di);

	switch (result) {
	case NOTOK:
		do_ds_unbind(conn);
		ds_log(da, "D-UNBIND.REQUEST", conn->cn_ad);
		break;
	case OK:
		DLOG(log_dsap, LLOG_TRACE,
		     ("conn_release: dr_affirmative = %d", dr->dr_affirmative));
		if (!dr->dr_affirmative) {
			if ((conn->cn_last_release == conn->cn_last_used)
			    && (conn->cn_initiator)) {
				LLOG(log_dsap, LLOG_EXCEPTIONS,
				     ("conn_release rejected again without activity - Aborting %d",
				      conn->cn_ad));
				if (DUAbortRequest(conn->cn_ad, di) != OK)
					force_close(conn->cn_ad, di);
				do_ds_unbind(conn);
			} else {
				LLOG(log_dsap, LLOG_EXCEPTIONS,
				     ("conn_release rejected - continuing with association %d",
				      conn->cn_ad));
				conn->cn_last_release = conn->cn_last_used = timenow;
				return NOTOK;
			}
		} else {
			do_ds_unbind(conn);
			DLOG(log_dsap, LLOG_TRACE, ("conn_release: Conn finished!"));
		}
		break;
	case DONE:
		DLOG(log_dsap, LLOG_TRACE, ("Still Waiting for release"));
		conn->cn_last_release = conn->cn_last_used = timenow;
		return NOTOK;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Unexpected return from DUnBindRetry"));
		return NOTOK;
	}

	DLOG(log_dsap, LLOG_DEBUG, ("conn_release calling conn_extract"));
	conn_extract(conn);
	return OK;
}
