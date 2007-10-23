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

/* ds_abandon.c -  */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/ds_abandon.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/ds_abandon.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: ds_abandon.c,v
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

#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "quipu/abandon.h"
#include "quipu/dsp.h"
#include "quipu/ds_error.h"
#include "quipu/connection.h"

extern LLog *log_dsap;

/* ARGSUSED */

do_ds_abandon(arg, error)
	struct ds_abandon_arg *arg;
	struct DSError *error;
{
	DLOG(log_dsap, LLOG_TRACE, ("ds_abandon"));

	error->dse_type = DSE_ABANDON_FAILED;
	error->ERR_ABANDON_FAIL.DSE_ab_problem = DSE_AB_CANNOTABANDON;
	error->ERR_ABANDON_FAIL.DSE_ab_invokeid = 0;
	return (NOTOK);
}

perform_abandon(tk)
	struct task_act *tk;
{
	struct task_act *tk_tmp;
	struct task_act **tk_p;
	int ab_id = tk->tk_dx.dx_arg.dca_dsarg.arg_ab.aba_invokeid;
	struct DSError *err = &(tk->tk_resp.di_error.de_err);

	DLOG(log_dsap, LLOG_TRACE, ("perform_abandon"));

	tk_p = &(tk->tk_conn->cn_tasklist);
	for (tk_tmp = (*tk_p); tk_tmp != NULLTASK; tk_tmp = tk_tmp->tk_next) {
		if (tk_tmp->tk_dx.dx_id == ab_id)
			break;

		tk_p = &(tk_tmp->tk_next);
	}
	if (tk_tmp == NULLTASK) {
		LLOG(log_dsap, LLOG_NOTICE, ("perform_abandon - cannot find task to abandon"));
		err->dse_type = DSE_ABANDON_FAILED;
		err->ERR_ABANDON_FAIL.DSE_ab_problem = DSE_AB_NOSUCHOPERATION;
		err->ERR_ABANDON_FAIL.DSE_ab_invokeid = ab_id;
		return (NOTOK);
	} else {
		DLOG(log_dsap, LLOG_DEBUG, ("perform_abandon - found task to abandon"));

		/* Slice out task to abandon */
		(*tk_p) = tk_tmp->tk_next;

		if (task_abandon(tk_tmp) != OK) {
			DLOG(log_dsap, LLOG_DEBUG, ("perform_abandon - task_abandon NOTOK"));
			err->dse_type = DSE_ABANDON_FAILED;
			err->ERR_ABANDON_FAIL.DSE_ab_problem = DSE_AB_CANNOTABANDON;
			err->ERR_ABANDON_FAIL.DSE_ab_invokeid = ab_id;
			return (NOTOK);
		} else {
			DLOG(log_dsap, LLOG_DEBUG, ("perform_abandon - task_abandon OK"));
			tk->tk_result = &(tk->tk_resp.di_result.dr_res);
			tk->tk_result->dcr_dsres.result_type = OP_ABANDON;
			return (OK);
		}
	}
}

task_abandon(tk)
	struct task_act *tk;
{
	struct oper_act *on;

	DLOG(log_dsap, LLOG_TRACE, ("task_abandon"));

	for (on = tk->tk_operlist; on != NULLOPER; on = on->on_next_task) {
		on->on_state = ON_ABANDONED;
		on->on_task = NULLTASK;
		if (on->on_dsas) {
			di_desist(on->on_dsas);
			on->on_dsas = NULL_DI_BLOCK;
		}
	}

	ds_error_free(&tk->tk_resp.di_error.de_err);
	tk->tk_resp.di_error.de_err.dse_type = DSE_ABANDONED;
	task_error(tk);

	return (OK);
}
