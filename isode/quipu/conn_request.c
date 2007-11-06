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

/* conn_request.c - Generate DSP BIND from connection block */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/conn_request.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/conn_request.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: conn_request.c,v
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

#include "quipu/util.h"
#include "quipu/dsap.h"
#include "tsap.h"
#include "quipu/connection.h"

extern time_t timenow;

struct connection *conn_alloc();
void conn_free();

extern LLog *log_dsap;

#ifndef NO_STATS
extern LLog *log_stat;
#endif

extern PS opt;
extern unsigned watchdog_time;
extern unsigned watchdog_delta;

/*
*  conn_request uses the directory bind argument and context
*  set up in the connection block to generate an asynchronous
*  association request.
*  If OK is returned the connection should be linked onto
*  the global list.
*  If NOTOK is returned the connection block should be freed,
*  alerting all the operations requesting it.
*/
conn_request(cn)
	register struct connection *cn;
{
	struct DSAPconnect *dc = &(cn->cn_connect.cc_dc);
	struct DSAPindication di_s;
	struct DSAPindication *di = &(di_s);
	int inv_ret;
	struct oper_act *on;
	char abandon = TRUE;

	LLOG(log_dsap, LLOG_TRACE,
	     ("conn_request: Calling: %s", paddr2str(&(cn->cn_addr), NULLNA)));

	/* check op has not been abandoned */
	for (on = cn->cn_operlist; on != NULLOPER; on = on->on_next_conn)
		if (on->on_state != ON_ABANDONED) {
			abandon = FALSE;
			break;
		}

	if (abandon && cn->cn_operlist)
		return NOTOK;

	switch (cn->cn_ctx) {
	case DS_CTX_X500_DAP:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Making DAP connections illegal for DSA"));
		return (NOTOK);

	case DS_CTX_X500_DSP:
		LLOG(log_dsap, LLOG_TRACE, ("Making an X500 DSP connection"));
		inv_ret = DspAsynBindRequest(&(cn->cn_addr), &(cn->cn_connect.cc_req),
					     0, dc, di, ROS_ASYNC);
		break;

	case DS_CTX_QUIPU_DSP:
		LLOG(log_dsap, LLOG_TRACE, ("Making a QUIPU DSP connection"));
		inv_ret = QspAsynBindRequest(&(cn->cn_addr),
					     &(cn->cn_connect.cc_req), 0, dc, di, ROS_ASYNC);
		break;

	case DS_CTX_INTERNET_DSP:
		LLOG(log_dsap, LLOG_TRACE, ("Making a Internet DSP connection"));
		inv_ret = IspAsynBindRequest(&(cn->cn_addr),
					     &(cn->cn_connect.cc_req), 0, dc, di, ROS_ASYNC);
		break;

	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Unknown connection context"));
		return (NOTOK);
	}

	cn->cn_last_used = timenow;

	switch (inv_ret) {
	case NOTOK:
#ifndef NO_STATS
		pslog(log_stat, LLOG_NOTICE, "Failed (NOTOK)", (IFP) dn_print, (caddr_t) cn->cn_dn);
#endif
		dsa_reliable(cn, FALSE, timenow);
		return (NOTOK);
	case DONE:
		cn->cn_ad = dc->dc_sd;
		if (cn->cn_ad == NOTOK) {
#ifndef NO_STATS
			pslog(log_stat, LLOG_NOTICE, "Failed (DONE)", (IFP) dn_print,
			      (caddr_t) cn->cn_dn);
#endif
			cn->cn_state = CN_FAILED;
			dsa_reliable(cn, FALSE, timenow);
			return (NOTOK);
		}
		if (conn_req_aux(cn) == NOTOK) {
#ifndef NO_STATS
			pslog(log_stat, LLOG_NOTICE, "Failed (DONE 2)", (IFP) dn_print,
			      (caddr_t) cn->cn_dn);
#endif
			dsa_reliable(cn, FALSE, timenow);
			return NOTOK;
		}
		return OK;
	case CONNECTING_1:
		cn->cn_ad = dc->dc_sd;
		cn->cn_state = CN_CONNECTING1;
#ifndef NO_STATS
		pslog(log_stat, LLOG_NOTICE, "Trying (CONN_1)", (IFP) dn_print,
		      (caddr_t) cn->cn_dn);
#endif
		return (OK);

	case CONNECTING_2:
		cn->cn_ad = dc->dc_sd;
		cn->cn_state = CN_CONNECTING2;
#ifndef NO_STATS
		pslog(log_stat, LLOG_NOTICE, "Trying (CONN_2)", (IFP) dn_print,
		      (caddr_t) cn->cn_dn);
#endif
		return (OK);

	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Unknown return from DAsynBind : %d", inv_ret));
		return (NOTOK);
	}			/* switch inv_ret */
	/* NOTREACHED */
}

/*
* conn_req_aux() is called to complete work started by conn_request().
* Current major complication is how to deal with an undecodable BindResult.
* If OK is returned, the connection is ready for action and any waiting
* operations should be sent.
* If NOTOK is returned the connection needs to be extracted, alerting any
* waiting operations in the process.
*/
conn_req_aux(cn)
	register struct connection *cn;
{

	switch (cn->cn_connect.cc_dc.dc_result) {
	case DS_RESULT:
		DLOG(log_dsap, LLOG_TRACE, ("D-BIND.RETRY(ASYNC) RESULT"));
		cn->cn_state = CN_OPEN;
		break;

	case DS_ERROR:
		/* 
		 * Get the DirectoryBindError
		 */
		DLOG(log_dsap, LLOG_TRACE, ("D-BIND.RETRY(ASYNC) ERROR"));
		cn->cn_state = CN_FAILED;
		cn->cn_ad = 0;
		break;

	default:
		cn->cn_state = CN_FAILED;
		cn->cn_ad = 0;
		LLOG(log_dsap, LLOG_TRACE, ("Association rejected"));
		break;

	}			/* switch acc->acc_result */

	if (cn->cn_state == CN_OPEN) {
		struct TSAPdisconnect td_s;
		struct TSAPdisconnect *td = &td_s;

		if (TSetQueuesOK(cn->cn_ad, 1, td) == NOTOK)
			td_log(td, "TSetQueuesOK (outgoing)");

		cn->cn_last_used = timenow;
		dsa_reliable(cn, TRUE, cn->cn_last_used);
#ifndef NO_STATS
		{
			char buf[LINESIZE];

			(void) sprintf(buf, "Bound using %s DSP context (%d)",
				       cn->cn_ctx == DS_CTX_QUIPU_DSP ? "QUIPU" :
				       cn->cn_ctx == DS_CTX_INTERNET_DSP ? "Internet" : "X500",
				       cn->cn_ad);
			pslog(log_stat, LLOG_NOTICE, buf, (IFP) dn_print, (caddr_t) cn->cn_dn);
		}
#endif
		return (OK);
	}

	return (NOTOK);
}
