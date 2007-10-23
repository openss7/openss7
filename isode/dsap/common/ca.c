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

/* ca.c - General Directory Name routines */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/ca.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/ca.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: ca.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

#include "quipu/util.h"
#include "quipu/common.h"
#include "quipu/dsargument.h"

DN mydsadn = NULLDN;

struct common_args *
get_ca_ref(dsarg)
	struct ds_op_arg *dsarg;
{
	struct common_args *ca;

	switch (dsarg->dca_dsarg.arg_type) {
	case OP_READ:
		ca = &(dsarg->dca_dsarg.arg_rd.rda_common);
		break;
	case OP_COMPARE:
		ca = &(dsarg->dca_dsarg.arg_cm.cma_common);
		break;
	case OP_ABANDON:
		ca = NULL_COMMONARG;
		break;
	case OP_LIST:
		ca = &(dsarg->dca_dsarg.arg_ls.lsa_common);
		break;
	case OP_SEARCH:
		ca = &(dsarg->dca_dsarg.arg_sr.sra_common);
		break;
	case OP_ADDENTRY:
		ca = &(dsarg->dca_dsarg.arg_ad.ada_common);
		break;
	case OP_REMOVEENTRY:
		ca = &(dsarg->dca_dsarg.arg_rm.rma_common);
		break;
	case OP_MODIFYENTRY:
		ca = &(dsarg->dca_dsarg.arg_me.mea_common);
		break;
	case OP_MODIFYRDN:
		ca = &(dsarg->dca_dsarg.arg_mr.mra_common);
		break;
	case OP_GETEDB:
		ca = NULL_COMMONARG;
		break;
	}
	return (ca);
}

cha_loopdetected(cha)
	struct chain_arg *cha;
{
	struct trace_info ti_elem_s;
	struct trace_info *ti_elem = &(ti_elem_s);

	ti_elem->ti_dsa = mydsadn;
	ti_elem->ti_target = cha->cha_target;
	ti_elem->ti_progress.op_resolution_phase = cha->cha_progress.op_resolution_phase;
	ti_elem->ti_progress.op_nextrdntoberesolved = cha->cha_progress.op_nextrdntoberesolved;

	return (ti_is_elem(ti_elem, cha->cha_trace));
}

ti_is_elem(ti, ti_list)
	struct trace_info *ti;
	struct trace_info *ti_list;
{
	struct trace_info *tip;

	for (tip = ti_list; tip != NULLTRACEINFO; tip = tip->ti_next) {
		if (dn_cmp(ti->ti_dsa, tip->ti_dsa) != 0)
			continue;

		if (dn_cmp(ti->ti_target, tip->ti_target) != 0)
			continue;

		if (ti->ti_progress.op_resolution_phase == tip->ti_progress.op_resolution_phase)
			return (1);

		if (ti->ti_progress.op_nextrdntoberesolved ==
		    tip->ti_progress.op_nextrdntoberesolved)
			return (1);
	}

	return (0);
}

struct trace_info *
ti_cpy(ti)
	struct trace_info *ti;
{
	struct trace_info *ret_ti;

	if (ti == NULLTRACEINFO)
		return (NULLTRACEINFO);

	ret_ti = (struct trace_info *) malloc(sizeof(struct trace_info));

	ret_ti->ti_target = dn_cpy(ti->ti_target);
	ret_ti->ti_dsa = dn_cpy(ti->ti_dsa);
	ret_ti->ti_progress = ti->ti_progress;
	ret_ti->ti_next = ti_cpy(ti->ti_next);

	return (ret_ti);
}
