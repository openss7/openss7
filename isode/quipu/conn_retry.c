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

/* conn_retry.c - deal with asynchronous A-Associate events */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/conn_retry.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/conn_retry.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: conn_retry.c,v
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

#include "quipu/dsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern LLog *log_dsap;
extern time_t timenow;
extern time_t nsap_timeout;

#ifndef NO_STATS
extern LLog *log_stat;
#endif

struct connection *conn_alloc();

static test_deadlock();

/*
* Deal with an incoming acceptance of association establishment.
* Return value says whether anything has happened or not.
*/
conn_retry(conn, moveon)
	struct connection *conn;
	int moveon;
{
	struct DSAPconnect *dc = &(conn->cn_connect.cc_dc);
	struct DSAPindication di_s;
	struct DSAPindication *di = &di_s;
	struct oper_act *on;
	struct oper_act *onext;
	struct oper_act *ont = NULLOPER;
	int result;
	int pstate;
	int do_next_nsap;

	DLOG(log_dsap, LLOG_TRACE, ("conn_retry()"));

	pstate = conn->cn_state;

	if (moveon && (timenow - conn->cn_last_used >= nsap_timeout)) {
		/* this NSAP has had long enough - try the next one... */
		LLOG(log_dsap, LLOG_NOTICE, ("NSAP hanging (%d)...", conn->cn_ad));
		do_next_nsap = 1;
		conn->cn_last_used = timenow;	/* restart timer */
	} else {
		do_next_nsap = 0;
	}

	switch (conn->cn_ctx) {
	case DS_CTX_X500_DAP:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("DAP context type in conn_retry()"));
		conn_extract(conn);
		return;

	case DS_CTX_X500_DSP:
		result = DspAsynBindRetry(conn->cn_ad, do_next_nsap, dc, di);
		break;

	case DS_CTX_QUIPU_DSP:
		result = QspAsynBindRetry(conn->cn_ad, do_next_nsap, dc, di);
		break;

	case DS_CTX_INTERNET_DSP:
		result = IspAsynBindRetry(conn->cn_ad, do_next_nsap, dc, di);
		break;

	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Unknown context type in conn_retry()"));
		conn_extract(conn);
		return;
	}

	switch (result) {
	case CONNECTING_1:
		DLOG(log_dsap, LLOG_TRACE, ("D-BIND.RETRY CONNECTING_1 (%d)", conn->cn_ad));
		conn->cn_state = CN_CONNECTING1;
		break;

	case CONNECTING_2:
		DLOG(log_dsap, LLOG_TRACE, ("D-BIND.RETRY CONNECTING_2 (%d)", conn->cn_ad));
		conn->cn_state = CN_CONNECTING2;
		break;

	case NOTOK:
		conn->cn_state = CN_FAILED;

#ifndef NO_STATS
		pslog(log_stat, LLOG_TRACE, "Failed (RETRY NOTOK)", (IFP) dn_print,
		      (caddr_t) conn->cn_dn);
#endif
		dsa_reliable(conn, FALSE, timenow);

		for (on = conn->cn_operlist; on != NULLOPER; on = onext) {
			onext = on->on_next_conn;
			/* See if there is another DSA to try... */
			if ((on->on_state != ON_ABANDONED) && (on->on_dsas != NULL_DI_BLOCK)) {
				LLOG(log_dsap, LLOG_TRACE, ("Trying a different DSA (NOTOK)..."));
				if ((test_deadlock(on) == OK) && (oper_chain(on) == OK)) {
					if (ont == NULLOPER)
						conn->cn_operlist = onext;
					else
						ont->on_next_conn = onext;
					continue;
				}
			}
			/* No - we can't do it !!! */
			oper_fail_wakeup(on);
			ont = on;
		}
		conn_extract(conn);
		break;

	case DONE:
		DLOG(log_dsap, LLOG_TRACE, ("D-BIND.RETRY DONE (%d)", conn->cn_ad));
		if ((conn->cn_ad == NOTOK) || (conn_req_aux(conn) != OK)) {
#ifndef NO_STATS
			pslog(log_stat, LLOG_NOTICE, "Failed (RETRY DONE)", (IFP) dn_print,
			      (caddr_t) conn->cn_dn);
#endif
			dsa_reliable(conn, FALSE, timenow);
			conn->cn_state = CN_FAILED;
			for (on = conn->cn_operlist; on != NULLOPER; on = onext) {
				onext = on->on_next_conn;
				/* See if there is another DSA to try... */
				if ((on->on_state != ON_ABANDONED)
				    && (on->on_dsas != NULL_DI_BLOCK)) {
					LLOG(log_dsap, LLOG_TRACE,
					     ("Trying a different DSA (DONE)..."));
					if ((test_deadlock(on) == OK) && (oper_chain(on) == OK)) {
						if (ont == NULLOPER)
							conn->cn_operlist = onext;
						else
							ont->on_next_conn = onext;
						continue;
					}
				}
				oper_fail_wakeup(on);
				ont = on;
			}
			DLOG(log_dsap, LLOG_DEBUG, ("conn_retry calling conn_extract 1"));
			conn_extract(conn);
			return;
		}

		for (on = conn->cn_operlist; on != NULLOPER; on = on->on_next_conn) {
			if (on->on_state == ON_ABANDONED)
				oper_fail_wakeup(on);

			else if (oper_send_invoke(on) != OK) {
				LLOG(log_dsap, LLOG_EXCEPTIONS,
				     ("oper_send_invoke failed in conn_retry"));
				oper_log(on, LLOG_DEBUG);
				oper_fail_wakeup(on);
			}
		}
		break;

	default:
		DLOG(log_dsap, LLOG_DEBUG, ("conn_retry calling conn_extract 2"));
		for (on = conn->cn_operlist; on != NULLOPER; on = on->on_next_conn) {
			oper_fail_wakeup(on);
		}
		conn_extract(conn);
		break;

	}			/* switch retry */

	if (pstate != conn->cn_state)
		conn->cn_last_used = timenow;
}

static
test_deadlock(on)
	struct oper_act *on;
{
	struct di_block *di;
	register int ndi = 0;

	for (di = on->on_dsas; di != NULL_DI_BLOCK; di = di->di_next)
		ndi++;

	/* To proceed, we need to contact on_dsas. */
	/* Check they do not rely on the conn that has just failed */
	/* Possibly a better way of testing this... */

	for (di = on->on_dsas; di != NULL_DI_BLOCK; di = di->di_next)
		if ((di->di_state == DI_DEFERRED) && (di->di_perform) && (di->di_perform->on_conn))
			switch (di->di_perform->on_conn->cn_state) {
			case 0:
			case CN_FAILED:
				ndi--;
			}

	if (ndi != 0)
		return OK;

	if (on->on_task) {
		on->on_task->tk_resp.di_error.de_err.dse_type = DSE_SERVICEERROR;
		on->on_task->
		    tk_resp.di_error.de_err.ERR_SERVICE.DSE_sv_problem = DSE_SV_UNABLETOPROCEED;
	}

	return NOTOK;
}
