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

/* conn_finish.c - deal with request to finish the association */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/conn_finish.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/conn_finish.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: conn_finish.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include "quipu/dsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern LLog *log_dsap;

/* ARGSUSED */
conn_finish(conn, df)
	struct connection *conn;
	struct DSAPfinish *df;
{
	int result;
	struct oper_act *on;
	extern time_t conn_timeout, timenow;
	struct DSAPindication di_s;
	struct DSAPindication *di = &(di_s);

	DLOG(log_dsap, LLOG_TRACE, ("conn_finish()"));

	/* Can release be negotiated? */
	if (conn->cn_start.cs_ds.ds_start.acs_start.ps_srequirements & SR_NEGOTIATED) {
		/* Should release be rejected? */
		for (on = conn->cn_operlist; on != NULLOPER; on = on->on_next_conn)
			if (on->on_state == ON_CHAINED)
				break;

		if (on != NULLOPER) {
			/* 
			 * See if oper has had time to complete
			 * if so remote DSA has probably lost the operation (never !!!)
			 * else reject the release
			 */

			if (timenow - conn->cn_last_used < conn_timeout) {
				result = DUnBindReject(conn->cn_ad, ACS_REJECT,
						       ACR_NOTFINISHED, di);

				if (result != OK) {
					result = DUAbortRequest(conn->cn_ad, di);
					if (result != OK)
						force_close(conn->cn_ad, di);

					do_ds_unbind(conn);
					conn_extract(conn);
				}
				return;
			}
		}
	}

	result = DUnBindAccept(conn->cn_ad, di);
	if (result != OK) {
		result = DUAbortRequest(conn->cn_ad, di);
		if (result != OK)
			force_close(conn->cn_ad, di);
	}
	do_ds_unbind(conn);
	conn_extract(conn);

}

conn_rel_abort(conn)
	struct connection *conn;
{
	struct DSAPindication di_s;
	struct DSAPindication *di = &di_s;

#ifdef notanymore		/* Just get rid of it! */
	if (!conn->cn_initiator)
		return;
#endif

	LLOG(log_dsap, LLOG_NOTICE, ("conn_rel_abort %d", conn->cn_ad));

	if (DUAbortRequest(conn->cn_ad, di) != OK)
		force_close(conn->cn_ad, di);
}
