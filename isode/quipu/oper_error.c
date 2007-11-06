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

/* oper_error.c - deal with return of error to an operation */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/oper_error.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/oper_error.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: oper_error.c,v
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

#include "rosap.h"
#include "quipu/util.h"
#include "quipu/connection.h"
#include "quipu/ds_error.h"

extern LLog *log_dsap;

#ifndef NO_STATS
extern LLog *log_stat;
#endif
extern time_t timenow;

oper_error(conn, di)
	struct connection *conn;
	struct DSAPindication *di;
{
	struct DSAPerror *de = &(di->di_error);
	struct oper_act *oper;

	DLOG(log_dsap, LLOG_TRACE, ("net_wait_ro_error"));

	for (oper = conn->cn_operlist; oper != NULLOPER; oper = oper->on_next_conn)
		if (oper->on_id == de->de_id)
			break;

	if (oper == NULLOPER) {
		LLOG(log_dsap, LLOG_FATAL, ("oper_error: Cannot locate operation for error"));
		ds_error_free(&di->di_error.de_err);
		send_ro_ureject(conn->cn_ad, &(de->de_id), ROS_REP_UNRECOG);
		return;
	}
#ifndef NO_STATS
	LLOG(log_stat, LLOG_DEBUG, ("Error received (%d) [%d]", oper->on_conn->cn_ad, oper->on_id));
#endif

	if (oper->on_state == ON_ABANDONED) {
		LLOG(log_dsap, LLOG_TRACE, ("oper_error: operation had been abandoned"));
		ds_error_free(&di->di_error.de_err);
		oper_extract(oper);
		return;
	}

	if (!ds_recog_err(de->de_err.dse_type)) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_error - Unrecognised error"));
		ds_error_free(&di->di_error.de_err);
		send_ro_ureject(conn->cn_ad, &(de->de_id), ROS_REP_RECERR);
		oper_fail_wakeup(oper);
	}

	/* free previous error - if any */
	ds_error_free(&oper->on_resp.di_error.de_err);

	oper->on_resp = (*di);	/* struct copy */

	conn->cn_last_used = timenow;

	/* Need to check type of operation here! */
	switch (oper->on_type) {
	case ON_TYPE_X500:
		task_error_wakeup(oper);
		break;
	case ON_TYPE_SUBTASK:
		subtask_error_wakeup(oper);
		break;
	case ON_TYPE_BIND_COMPARE:
		bind_compare_error_wakeup(oper);
		break;
	case ON_TYPE_GET_DSA_INFO:
		dsa_info_error_wakeup(oper);
		break;
	case ON_TYPE_GET_EDB:
		get_edb_fail_wakeup(oper);
		break;
	case ON_TYPE_SHADOW:
		(void) shadow_fail_wakeup(oper);
		break;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("oper_error - on_type invalid"));
		break;
	}

}
