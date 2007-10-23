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

/* task_act.c - routines to manipulate task activity blocks */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/task_act.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/task_act.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: task_act.c,v
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
#include "quipu/connection.h"

extern LLog *log_dsap;

#ifdef QUIPU_CONSOLE
void end_of_task_analyse();
#endif				/* QUIPU_CONSOLE */

struct task_act *
task_alloc()
{
	struct task_act *tk_ret;

	tk_ret = (struct task_act *) calloc(1, sizeof(struct task_act));

	tk_ret->tk_result = &(tk_ret->tk_resp.di_result.dr_res);
	tk_ret->tk_error = &(tk_ret->tk_resp.di_error.de_err);

	return (tk_ret);
}

task_free(tk)
	struct task_act *tk;
{
	DLOG(log_dsap, LLOG_TRACE, ("task_free()"));

	op_arg_free(&(tk->tk_dx.dx_arg));

	if (tk->tk_resp.di_type == DI_ERROR)
		ds_error_free(&(tk->tk_resp.di_error.de_err));
	else if (tk->tk_resp.di_type == DI_RESULT)
		if (tk->tk_result)
			if (tk->tk_conn->cn_ctx == DS_CTX_X500_DAP)
				ds_res_free(&(tk->tk_result->dcr_dsres));
			else
				op_res_free(tk->tk_result);

	free((char *) tk);
}

/*
*  Extract task from list held by the connection it was received on.
*/
task_conn_extract(tk)
	struct task_act *tk;
{
	struct task_act *tk_tmp;
	struct task_act **tk_p;

	DLOG(log_dsap, LLOG_TRACE, ("task_conn_extract()"));

	tk_p = &(tk->tk_conn->cn_tasklist);
	for (tk_tmp = (*tk_p); tk_tmp != NULLTASK; tk_tmp = tk_tmp->tk_next) {
		if (tk_tmp == tk)
			break;

		tk_p = &(tk_tmp->tk_next);
	}
	if (tk_tmp == NULLTASK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("task_conn_extract: task lost from connections list."));
	} else {
		(*tk_p) = tk->tk_next;
	}
}

task_extract(tk)
	struct task_act *tk;
{
	struct oper_act *on;

#ifdef QUIPU_CONSOLE
	/* SPT: Tidy up and add finish time to this task in */
	/* the mammoth open_call structure. (wherever it is!) */
	end_of_task_analyse(tk);

#endif				/* QUIPU_CONSOLE */

	DLOG(log_dsap, LLOG_TRACE, ("task_extract()"));
	if (tk == NULLTASK) {
		LLOG(log_dsap, LLOG_FATAL, ("Attempted to extract NULLTASK!!"));
		return;
	}

	for (on = tk->tk_operlist; on != NULLOPER; on = on->on_next_task) {
		oper_task_extract(on);
		if (on->on_conn == NULLCONN)
			oper_free(on);
	}

	task_free(tk);
	DLOG(log_dsap, LLOG_TRACE, ("task block extracted"));
}

task_log(tk, level)
	struct task_act *tk;
	int level;
{
	struct oper_act *on;
	char *state;

	switch (tk->tk_state) {
	case TK_ACTIVE:
		state = "ACTIVE";
		break;
	case TK_PASSIVE:
		state = "PASSIVE";
		break;
	case TK_SUSPEND:
		state = "SUSPEND";
		break;
	default:
		state = "?";
		break;
	}

	DLOG(log_dsap, level, ("Task id = %d, state = %s, prio = %d.",
			       tk->tk_dx.dx_id, state, tk->tk_prio));

	if (tk->tk_operlist != NULLOPER)
		DLOG(log_dsap, level, ("Task-Opers:"));
	for (on = tk->tk_operlist; on != NULLOPER; on = on->on_next_task)
		oper_log(on, level);
}

#ifdef QUIPU_CONSOLE
void
end_of_task_analyse(task)
	struct task_act *task;
{
	extern AV_Sequence open_call_avs;
	extern time_t timenow;
	AV_Sequence tmp_avs;
	struct op_list *tmp_op_list;
	struct DSArgument *arg;
	UTCtime ut;
	time_t timenow;

#ifdef SPT_DEBUG
	fprintf(stderr, "Ping! We have an end_of_task_analysis to do...\n");
#endif

	/* Move along the open_call_avs to find a matching address, DN and association ID */

	tmp_avs = open_call_avs;

	if (tmp_avs == (AV_Sequence) 0) {
#ifdef SPT_DEBUG
		fprintf(stderr,
			"dsa_wait.c: Hey, got a running analyse with no calls present. Error!\n");
#endif
		return;
	}

	while (tmp_avs != (AV_Sequence) 0) {
		if ((((struct quipu_call *) tmp_avs->avseq_av.av_struct)->assoc_id ==
		     task->tk_conn->cn_start.cs_ds.ds_sd)
		    && (((struct quipu_call *) tmp_avs->avseq_av.av_struct)->finish_time ==
			(char *) 0)) {
			/* Same Assoc Number and the connection has not been finished! Match? */
			break;
		}
		tmp_avs = tmp_avs->avseq_next;
	}

	if (tmp_avs == (AV_Sequence) 0) {
#ifdef SPT_DEBUG
		fprintf(stderr, "We have no match here. Odd. Check...\n");
#endif
		return;
	}
#ifdef SPT_DEBUG
	fprintf(stderr, "We have a match here. Yahoo...\n");
#endif

	if (((struct quipu_call *) tmp_avs->avseq_av.av_struct)->pending_ops ==
	    (struct op_list *) 0) {
		fprintf(stderr, "What?!! No tasks on this conn, and we must close a task?!!\n");
		return;
	}

	tmp_op_list = ((struct quipu_call *) tmp_avs->avseq_av.av_struct)->pending_ops;

	while (tmp_op_list != (struct op_list *) 0) {
		if ((tmp_op_list->operation_list->invoke_id == task->tk_dx.dx_id) &&
		    (tmp_op_list->operation_list->finish_time == (char *) 0)) {
			break;
		}
		tmp_op_list = tmp_op_list->next;
	}

	if (tmp_op_list == (struct op_list *) 0) {
		fprintf(stderr, "Can't find the task to close it!\n");
		return;
	}

	(void) time(&timenow);
	tm2ut(gmtime(&timenow), &ut);
	tmp_op_list->operation_list->finish_time = strdup(utct2str(&ut));
}
#endif				/* QUIPU_CONSOLE */
