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

/* delete.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/delete.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/delete.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: delete.c,v
 * Revision 9.0  1992/06/16  12:35:39  isode
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

#include "quipu/util.h"
#include "quipu/remove.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

#include "osisec-stub.h"

extern DN dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS opt, rps;

call_delete(argc, argv)
	int argc;
	char **argv;
{
	DN dnptr, trail = NULLDN;
	struct ds_removeentry_arg remove_arg;
	struct DSError error;

	if ((argc = service_control(OPT, argc, argv, &remove_arg.rma_common)) == -1)
		return;

	if (argc > 1)
		if (move(argv[1]) == OK)
			argc--;

	if (argc != 1) {
		ps_printf(OPT, "Unknown option %s\n", argv[1]);
		Usage(argv[0]);
		return;
	}
	remove_arg.rma_object = dn;

	if (rebind() != OK)
		return;

	/* Strong authentication */
	if (remove_arg.rma_common.ca_security != (struct security_parms *) 0) {
		extern struct SecurityServices *dsap_security;

		remove_arg.rma_common.ca_sig =
		    (dsap_security->serv_sign) ((caddr_t) &remove_arg,
						_ZRemoveEntryArgumentDataDAS, &_ZDAS_mod);
	}

	while (ds_removeentry(&remove_arg, &error) != DS_OK) {
		if (dish_error(OPT, &error) == 0)
			return;
		remove_arg.rma_object = error.ERR_REFERRAL.DSE_ref_candidates->cr_name;
	}

	ps_print(RPS, "Removed ");
	dn_print(RPS, dn, EDBOUT);
	delete_cache(dn);
	for (dnptr = dn; dnptr->dn_parent != NULLDN; dnptr = dnptr->dn_parent)
		trail = dnptr;

	if (trail != NULLDN)
		trail->dn_parent = NULLDN;
	else
		dn = NULLDN;

	dn_comp_free(dnptr);
	ps_print(RPS, "\n");
}
