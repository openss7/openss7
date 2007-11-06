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

/* oper_result.c - deal with result of an operation */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/oper_result.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/oper_result.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: oper_result.c,v
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

#ifndef NO_STATS
extern LLog *log_stat;
#endif
extern time_t timenow;
extern Entry cache_dsp_entry();

oper_result(cn, di)
	struct connection *cn;
	struct DSAPindication *di;
{
	struct DSAPresult *dr = &(di->di_result);
	struct oper_act *on;

	DLOG(log_dsap, LLOG_TRACE, ("oper_result()"));

	for (on = cn->cn_operlist; on != NULLOPER; on = on->on_next_conn) {
		if (on->on_id == dr->dr_id)
			break;
	}

	if (on == NULLOPER) {
		LLOG(log_dsap, LLOG_FATAL, ("Cannot find operation to match result"));
		ds_res_free(&dr->dr_res.dcr_dsres);
		send_ro_ureject(cn->cn_ad, &(dr->dr_id), ROS_RRP_UNRECOG);
		return;
	}
#ifndef NO_STATS
	LLOG(log_stat, LLOG_DEBUG, ("Result received (%d) [%d]", on->on_conn->cn_ad, on->on_id));
#endif

	if (on->on_state == ON_ABANDONED) {
		LLOG(log_dsap, LLOG_TRACE, ("oper_result - operation had been abandoned"));

		/* If we have the arguments we could do more caching here. */
		if (dr->dr_res.dcr_dsres.result_type == OP_READ)
			(void) cache_dsp_entry(&dr->dr_res.dcr_dsres.res_rd.rdr_entry, TRUE);

		ds_res_free(&dr->dr_res.dcr_dsres);
		oper_extract(on);
		return;
	}

	if (dr->dr_res.dcr_dsres.result_type != on->on_arg->dca_dsarg.arg_type) {
		LLOG(log_dsap, LLOG_TRACE, ("oper_result - operation had been abandoned (2)"));
		send_ro_ureject(on->on_conn->cn_ad, &(dr->dr_id), ROS_RRP_MISTYPED);
		ds_res_free(&dr->dr_res.dcr_dsres);
		oper_extract(on);
		return;
	}

	/* free previous error - if any */
	ds_error_free(&on->on_resp.di_error.de_err);

	on->on_resp = (*di);	/* struct copy */

	cn->cn_last_used = timenow;

	switch (on->on_type) {
	case ON_TYPE_X500:
		task_result_wakeup(on);
		break;
	case ON_TYPE_SUBTASK:
		subtask_result_wakeup(on);
		break;
	case ON_TYPE_BIND_COMPARE:
		bind_compare_result_wakeup(on);
		ds_res_free(&dr->dr_res.dcr_dsres);
		break;
	case ON_TYPE_GET_DSA_INFO:
		dsa_info_result_wakeup(on);
		ds_res_free(&dr->dr_res.dcr_dsres);
		break;
	case ON_TYPE_GET_EDB:
	case ON_TYPE_SHADOW:
		on->on_state = ON_COMPLETE;
		break;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_result: operation of unknown type"));
		oper_extract(on);
		break;
	}
}
