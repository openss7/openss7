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

/* dapcompare.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/net/RCS/dapcompare.c,v 9.0 1992/06/16 12:14:05 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/net/RCS/dapcompare.c,v 9.0 1992/06/16 12:14:05 isode Rel
 *
 *
 * Log: dapcompare.c,v
 * Revision 9.0  1992/06/16  12:14:05  isode
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
#include "quipu/dap2.h"
#include "../x500as/DAS-types.h"

dap_compare(ad, id, arg, error, result)
	int ad;
	int *id;
	struct ds_compare_arg *arg;
	struct ds_compare_result *result;
	struct DSError *error;
{
	struct DAPindication di_s;
	struct DAPindication *di = &(di_s);

	++(*id);

	(void) DapCompare(ad, (*id), arg, di, ROS_INTR);

	error->dse_type = DSE_NOERROR;

	switch (di->di_type) {
	case DI_RESULT:
	{
		struct DAPresult *dr = &(di->di_result);

		/* Nasty struct copy */
		(*result) = dr->dr_res.res_cm;	/* struct copy */
		dr->dr_res.result_type = -1;	/* Prevent freeing */
		DRFREE(dr);
		return (DS_OK);
	}

	case DI_ERROR:
	{
		struct DAPerror *de = &(di->di_error);

		/* Nasty struct copy */
		(*error) = de->de_err;	/* struct copy */
		return (DS_ERROR_REMOTE);
	}

	case DI_PREJECT:
		error->dse_type = DSE_REMOTEERROR;
		return (DS_ERROR_PROVIDER);

	case DI_ABORT:
		error->dse_type = DSE_REMOTEERROR;
		return (DS_ERROR_CONNECT);

	default:
		error->dse_type = DSE_REMOTEERROR;
		return (DS_ERROR_PROVIDER);
	}
}

int
DapCompare(ad, id, arg, di, asyn)
	int ad;
	int id;
	struct ds_compare_arg *arg;
	struct DAPindication *di;
	int asyn;
{
	PE arg_pe;

	if (encode_DAS_CompareArgument(&arg_pe, 1, 0, NULLCP, arg) != OK) {
		return (dapreject(di, DP_INVOKE, id, NULLCP, "Compare argument encoding failed"));
	}

	return (DapInvokeReqAux(ad, id, OP_COMPARE, arg_pe, di, asyn));

}
