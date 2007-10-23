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

/* oper_act.c - routines to handle operation activity blocks */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/oper_act.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/oper_act.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: oper_act.c,v
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

struct oper_act *
oper_alloc()
{
	struct oper_act *on_ret;

	on_ret = (struct oper_act *) calloc(1, sizeof(struct oper_act));

	on_ret->on_arg = &(on_ret->on_req);

	on_ret->on_relay = TRUE;	/* Relay unless reason not to. */

	return (on_ret);
}

oper_free(on)
	struct oper_act *on;
{
	extern struct oper_act *pending_ops;

	DLOG(log_dsap, LLOG_TRACE, ("oper_free(%x)", on));

	if (on->on_state == -1) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("duplicate oper_free()"));
		return;
	}

	on->on_state = -1;

/*
	if (on->on_req.dca_charg.cha_trace != (struct trace_info *)NULL)
*/

	ds_error_free(&on->on_resp.di_error.de_err);

	if (on->on_type == ON_TYPE_SUBTASK)
		ch_arg_free(&on->on_req.dca_charg);
	else if (on->on_req.dca_dsarg.arg_type == 0)
		ch_arg_free(&on->on_req.dca_charg);
	else
		op_arg_free(&on->on_req);

	/* Not the best place to do this - but it will catch everything */
	if (on->on_next_task && ((on->on_type == ON_TYPE_GET_EDB) ||
				 (on->on_type == ON_TYPE_SHADOW))) {
		pending_ops = on->on_next_task;
		get_edb_ops = NULLOPER;
	}

	free((char *) on);
}

oper_extract(on)
	struct oper_act *on;
{
	DLOG(log_dsap, LLOG_TRACE, ("oper_extract()"));

	if (on->on_conn != NULLCONN)
		oper_conn_extract(on);

	if (on->on_task != NULLTASK)
		oper_task_extract(on);

	if (on->on_subtask) {
		st_comp_free(on->on_subtask);
		on->on_subtask = NULL_ST;
	}

	oper_free(on);
}

oper_conn_extract(on)
	struct oper_act *on;
{
	/* 
	 * Extract the operation activity block from the list held by its
	 * connection.
	 */
	struct oper_act *on_tmp;
	struct oper_act **on_p;

	DLOG(log_dsap, LLOG_TRACE, ("oper_conn_extract()"));

	if (on == NULLOPER) {
		LLOG(log_dsap, LLOG_FATAL, ("oper_conn_extract: Cannot extract NULLOPER"));
		return;
		/* This is an implementation error */
	}

	if (on->on_conn == NULLCONN) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_conn_extract: already extracted"));
		/* This operation must have already been extracted for some reason. */
		return;
	}

	on_p = &(on->on_conn->cn_operlist);
	for (on_tmp = (*on_p); on_tmp != NULLOPER; on_tmp = on_tmp->on_next_conn) {
		if (on_tmp == on)
			break;

		on_p = &(on_tmp->on_next_conn);
	}
	if (on_tmp == NULLOPER) {
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("oper_conn_extract: oper not on connections list!"));
	} else {
		(*on_p) = on_tmp->on_next_conn;
	}

	on->on_conn = NULLCONN;	/* Shows that this has been conn_extracted */
}

oper_task_extract(on)
	struct oper_act *on;
{
	/* 
	 * Extract this operation from the list held by its task.
	 */
	struct oper_act *on_tmp;
	struct oper_act **on_p;

	DLOG(log_dsap, LLOG_TRACE, ("oper_task_extract()"));

	if (on == NULLOPER) {
		LLOG(log_dsap, LLOG_FATAL, ("oper_task_extract: Cannot extract NULLOPER"));
		return;
		/* This is an implementation error */
	}

	if (on->on_task == NULLTASK) {
		/* Must have been extracted previously. */
		if (on->on_state != ON_ABANDONED)
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_task_extract: oper has no task"));
		return;
	}

	on_p = &(on->on_task->tk_operlist);
	for (on_tmp = (*on_p); on_tmp != NULLOPER; on_tmp = on_tmp->on_next_task) {
		if (on_tmp == on)
			break;

		on_p = &(on_tmp->on_next_task);
	}
	if (on_tmp == NULLOPER) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Oper not on tasks list"));
	} else {
		(*on_p) = on_tmp->on_next_task;
	}

	if (on->on_dsas != NULL_DI_BLOCK)
		di_desist(on->on_dsas);

	on->on_dsas = NULL_DI_BLOCK;

	if (on->on_task->tk_result == &(on->on_resp.di_result.dr_res))
		on->on_task->tk_result = (struct ds_op_res *) NULL;

	on->on_task = NULLTASK;	/* Shows that this has been task_extracted */
}

oper_log(on, level)
	struct oper_act *on;
	int level;
{
	char *state;
	char *xtype;

	switch (on->on_state) {
	case ON_DEFERRED:
		state = "DEFERRED";
		break;
	case ON_CHAINED:
		state = "CHAINED";
		break;
	case ON_COMPLETE:
		state = "COMPLETE";
		break;
	case ON_ABANDONED:
		state = "ABANDONED";
		break;
	default:
		state = "?";
		break;
	}

	switch (on->on_type) {
	case ON_TYPE_X500:
		xtype = "X500";
		break;
	case ON_TYPE_BIND_COMPARE:
		xtype = "BIND_COMPARE";
		break;
	case ON_TYPE_GET_DSA_INFO:
		xtype = "GET_DSA_INFO";
		break;
	case ON_TYPE_GET_EDB:
		xtype = "GET_EDB";
		break;
	case ON_TYPE_SUBTASK:
		xtype = "SUBTASK";
		break;
	case ON_TYPE_SHADOW:
		xtype = "SHADOW";
		break;
	default:
		xtype = "?";
		break;
	}

	DLOG(log_dsap, level, ("Oper id = %d, state = %s, type = %s", on->on_id, state, xtype));
}
