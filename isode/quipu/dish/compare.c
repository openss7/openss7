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

/* compare.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/compare.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/compare.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: compare.c,v
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
#include "quipu/compare.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

#include "osisec-stub.h"

extern DN dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS opt, rps;

call_compare(argc, argv)
	int argc;
	char **argv;
{
	struct DSError error;
	struct ds_compare_result result;
	struct ds_compare_arg compare_arg;

	int x;
	int att_present = 0;
	int print = FALSE;
	char *str1_type;
	char *str1_value;
	char *ptr;

	if ((argc = service_control(OPT, argc, argv, &compare_arg.cma_common)) == -1)
		return;

	if (argc == 1) {
		ps_print(OPT, "What do you want to compare ?\n");
		Usage(argv[0]);
		return;
	}

	for (x = 1; x < argc; x++) {
		if (test_arg(argv[x], "-attribute", 1)) {
			str1_type = argv[++x];
			att_present = 1;
		} else if (test_arg(argv[x], "-print", 3)) {
			print = TRUE;
		} else if (test_arg(argv[x], "-noprint", 3)) {
			print = FALSE;
		} else if (move(argv[x]) == OK)
			continue;
		else {
			if (*argv[x] == '-')
				ps_printf(OPT, "Unknown option %s\n", argv[x]);
			else
				ps_printf(OPT, "Invalid attribute assertion syntax %s\n", argv[x]);
			Usage(argv[0]);
			return;
		}
	}

	if (att_present == 0) {
		ps_printf(OPT, "We are missing <attribute_type>=<attribute_value>.\n");
		Usage(argv[0]);
		return;
	}
	ptr = str1_type;
	while (*ptr != '=') {
		ptr++;
	}
	*ptr++ = '\0';
	str1_value = ptr;
	if (get_ava(&compare_arg.cma_purported, str1_type, str1_value) != OK) {
		Usage(argv[0]);
		return;
	}
	compare_arg.cma_object = dn;

	if (rebind() != OK)
		return;

	/* Strong authentication */
	if (compare_arg.cma_common.ca_security != (struct security_parms *) 0) {
		extern struct SecurityServices *dsap_security;

		compare_arg.cma_common.ca_sig =
		    (dsap_security->serv_sign) ((caddr_t) &compare_arg,
						_ZCompareArgumentDataDAS, &_ZDAS_mod);
	}

	while (ds_compare(&compare_arg, &error, &result) != DS_OK) {
		if (dish_error(OPT, &error) == 0)
			return;
		compare_arg.cma_object = error.ERR_REFERRAL.DSE_ref_candidates->cr_name;
	}

	if (result.cmr_common.cr_aliasdereferenced & print) {
		ps_print(RPS, "(Alias dereferenced - ");
		dn_print(RPS, result.cmr_object, EDBOUT);
		dn_free(result.cmr_object);
		ps_print(RPS, ")\n");
	}
	if (print) {
		if (result.cmr_matched == TRUE)
			ps_print(RPS, "TRUE\n");
		else
			ps_print(OPT, "FALSE\n");
	} else {
		if (result.cmr_matched == TRUE)
			ps_print(RPS, "\1");
		else
			ps_print(OPT, "\0");
	}
}
