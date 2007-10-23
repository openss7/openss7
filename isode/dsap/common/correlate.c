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

/* correlate.c - correlate search results */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/correlate.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/correlate.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: correlate.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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
#include "quipu/list.h"		/* to get LSR # defs */
#include "quipu/ds_search.h"

extern LLog *log_dsap;
int entryinfo_print();

correlate_search_results(sr_res)
	struct ds_search_result *sr_res;
{
	struct ds_search_result *sr_tmp;
	struct ds_search_result *sr_last;

	DLOG(log_dsap, LLOG_DEBUG, ("correlate_search_results() "));

	if (sr_res->srr_correlated) {
		DLOG(log_dsap, LLOG_DEBUG, ("Already correlated! "));
		return;
	}

	DLOG(log_dsap, LLOG_DEBUG, ("Not yet correlated! "));

	for (sr_tmp = sr_res->srr_un.srr_parts; sr_tmp != NULLSRR; sr_tmp = sr_tmp->srr_next) {
		DLOG(log_dsap, LLOG_DEBUG, ("correlate_search_results(A Result Part)"));
		correlate_search_results(sr_tmp);
	}

	sr_tmp = sr_res->srr_un.srr_parts;
	sr_res->srr_correlated = TRUE;
	sr_res->srr_un.srr_unit = (struct ds_search_unit *) malloc(sizeof(struct ds_search_unit));
	sr_res->CSR_object = NULLDN;
	sr_res->CSR_entries = NULLENTRYINFO;
	sr_res->CSR_limitproblem = LSR_NOLIMITPROBLEM;
	sr_res->CSR_cr = NULLCONTINUATIONREF;
	sr_res->CSR_common.cr_requestor = NULLDN;
	sr_res->CSR_common.cr_aliasdereferenced = FALSE;

	while (sr_tmp != NULLSRR) {
		merge_search_results(sr_res, sr_tmp);
		sr_last = sr_tmp;
		sr_tmp = sr_tmp->srr_next;
		sr_last->srr_next = NULLSRR;
		DLOG(log_dsap, LLOG_DEBUG, ("Before search_results_free"));
		search_result_free(sr_last);
		DLOG(log_dsap, LLOG_DEBUG, ("After search_results_free"));
	}
}

merge_search_results(sr_res, sr_tmp)
	struct ds_search_result *sr_res;
	struct ds_search_result *sr_tmp;
{
	ContinuationRef cr_tmp;

	DLOG(log_dsap, LLOG_DEBUG, ("merge_search_results"));

	if (sr_tmp == NULLSRR)
		return;

	if (sr_res->CSR_entries == NULLENTRYINFO) {
		DLOG(log_dsap, LLOG_DEBUG, ("Before inserting entries"));
		sr_res->CSR_entries = sr_tmp->CSR_entries;
	} else {
#ifdef	HEAVYDEBUG
		pslog(log_dsap, LLOG_DEBUG, "Before merging entries", entryinfo_print,
		      (caddr_t) sr_res->CSR_entries);
#endif
		entryinfo_merge(sr_res->CSR_entries, sr_tmp->CSR_entries, 0);
	}
	sr_tmp->CSR_entries = NULLENTRYINFO;

	DLOG(log_dsap, LLOG_DEBUG, ("Before merging limitproblems"));

	if (sr_res->CSR_limitproblem == LSR_NOLIMITPROBLEM)
		sr_res->CSR_limitproblem = sr_tmp->CSR_limitproblem;

	DLOG(log_dsap, LLOG_DEBUG, ("Before merging ContinuationRefs"));

	if (sr_tmp->CSR_cr != NULLCONTINUATIONREF) {
		for (cr_tmp = sr_tmp->CSR_cr; cr_tmp->cr_next != NULLCONTINUATIONREF;
		     cr_tmp = cr_tmp->cr_next) {
#ifdef	DEBUG
			pslog(log_dsap, LLOG_DEBUG, "Another new ref:", (IFP) dn_print,
			      (caddr_t) cr_tmp->cr_name);
#endif
		}
#ifdef	DEBUG
		pslog(log_dsap, LLOG_DEBUG, "Another new ref:", (IFP) dn_print,
		      (caddr_t) cr_tmp->cr_name);
#endif
		cr_tmp->cr_next = sr_res->CSR_cr;
		sr_res->CSR_cr = sr_tmp->CSR_cr;
		sr_tmp->CSR_cr = NULLCONTINUATIONREF;
	} else {
		DLOG(log_dsap, LLOG_DEBUG, ("No new references to merge"));
	}

#ifdef DEBUG
	if (sr_res->CSR_cr != NULLCONTINUATIONREF) {
		for (cr_tmp = sr_res->CSR_cr; cr_tmp != NULLCONTINUATIONREF;
		     cr_tmp = cr_tmp->cr_next) {
			pslog(log_dsap, LLOG_DEBUG, "ref entry:", (IFP) dn_print,
			      (caddr_t) cr_tmp->cr_name);
		}
	} else {
		DLOG(log_dsap, LLOG_DEBUG, ("No references"));
	}
#endif
	DLOG(log_dsap, LLOG_DEBUG, ("After merging results:"));

}

search_result_free(arg)
	struct ds_search_result *arg;
{
	DLOG(log_dsap, LLOG_DEBUG, ("search_result_free"));

	if (arg == NULLSRR) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Lost part of search structure somewhere!"));
		return;
	}

	if (arg->srr_correlated) {
		DLOG(log_dsap, LLOG_DEBUG, ("search_result_free - correlated"));
		dn_free(arg->CSR_common.cr_requestor);
		dn_free(arg->CSR_object);
		entryinfo_free(arg->CSR_entries, 0);
		crefs_free(arg->CSR_cr);
		free((char *) arg->srr_un.srr_unit);
	} else {
		DLOG(log_dsap, LLOG_DEBUG, ("search_result_free - uncorrelated"));
		search_result_free(arg->srr_un.srr_parts);
		free((char *) arg->srr_un.srr_parts);
	}

	DLOG(log_dsap, LLOG_DEBUG, ("After freeing parts"));

	if (arg->srr_next != NULLSRR) {
		search_result_free(arg->srr_next);
		free((char *) arg->srr_next);
	}

	DLOG(log_dsap, LLOG_DEBUG, ("After freeing next"));

	return;
}
