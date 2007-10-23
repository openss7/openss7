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

/* task_error.c - */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/task_error.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/task_error.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: task_error.c,v
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

#include "rosap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern LLog *log_dsap;

void ros_log();

/*
* The DSA has produced an error for the task, encode the error,
* generate a D-ERROR.REQUEST and update the task block.
*/
task_error(task)
	register struct task_act *task;
{
	int result;
	struct DSAPindication di_s;
	struct DSAPindication *di = &(di_s);
	struct DSError *err;
	struct connection *cn = task->tk_conn;

#ifndef NO_STATS
	extern LLog *log_stat;

	if (log_stat->ll_events & LLOG_DEBUG)
		LLOG(log_stat, LLOG_DEBUG, ("Error sent (%d) [%d]",
					    task->tk_conn->cn_ad, task->tk_dx.dx_id));
	else
		LLOG(log_stat, LLOG_TRACE, ("Error sent (%d)", task->tk_conn->cn_ad));
#endif

	if (task == NULLTASK) {
		LLOG(log_dsap, LLOG_FATAL, ("Task memerr 2"));
		return;
	}

	err = task->tk_error;
	task->tk_resp.di_type = DI_ERROR;

	if (log_dsap->ll_events & LLOG_NOTICE)
		log_ds_error(task->tk_error);

	/* Return the right sort of referral error */
	if (cn->cn_ctx == DS_CTX_X500_DAP) {
		if (err->dse_type == DSE_DSAREFERRAL) {
			DLOG(log_dsap, LLOG_DEBUG, ("Changing DSAREFERRAL to REFERRAL (DAP)"));
			err->dse_type = DSE_REFERRAL;
		}
	} else {
		if (err->dse_type == DSE_REFERRAL) {
			DLOG(log_dsap, LLOG_DEBUG, ("Changing DSAREFERRAL to REFERRAL"));
			err->dse_type = DSE_DSAREFERRAL;
		}
	}

	switch (cn->cn_ctx) {
	case DS_CTX_X500_DAP:
		result = DapErrorRequest(cn->cn_ad, task->tk_dx.dx_id, err, di);
		break;
	case DS_CTX_X500_DSP:
		result = DspErrorRequest(cn->cn_ad, task->tk_dx.dx_id, err, di);
		break;
	case DS_CTX_QUIPU_DSP:
		result = QspErrorRequest(cn->cn_ad, task->tk_dx.dx_id, err, di);
		break;
	case DS_CTX_INTERNET_DSP:
		result = IspErrorRequest(cn->cn_ad, task->tk_dx.dx_id, err, di);
		break;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("task_error(): Unknown context %d", cn->cn_ctx));
		break;
	}

	if (result != OK) {
		if (di->di_type == DI_ABORT) {
			LLOG(log_dsap, LLOG_FATAL,
			     ("D-RESULT.REQUEST: fatal reject - fail the connection"));
			cn->cn_state = CN_FAILED;
		}
	}

	if (cn->cn_state == CN_FAILED) {
		DLOG(log_dsap, LLOG_DEBUG, ("task_error(): extracting conn:"));
		conn_log(cn, LLOG_DEBUG);
		conn_extract(cn);
	}
}
