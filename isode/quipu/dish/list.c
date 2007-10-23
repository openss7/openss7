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

/* list.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/list.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/list.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: list.c,v
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
#include "quipu/list.h"
#include "quipu/sequence.h"
#include "pepsy.h"
#include "quipu/DAS-types.h"

#include "osisec-stub.h"

extern DN dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS opt, rps;

extern char move_flag;

extern int sizelimit;
char list_show;

call_list(argc, argv)
	int argc;
	char **argv;
{
	struct ds_list_arg list_arg;
	struct ds_list_result result;
	struct list_cache *ptr;
	struct DSError error;
	int x;
	char nocacheflag = FALSE;
	extern int copy_flag;

	list_show = TRUE;
	move_flag = FALSE;

	list_arg.lsa_common.ca_servicecontrol.svc_sizelimit = sizelimit;

	if ((argc = service_control(OPT, argc, argv, &list_arg.lsa_common)) == -1)
		return;

	for (x = 1; x < argc; x++) {

		if (test_arg(argv[x], "-nocache", 4))
			nocacheflag = TRUE;
		else if (test_arg(argv[x], "-noshow", 4))
			list_show = FALSE;
		else if (test_arg(argv[x], "-move", 2))
			move_flag = TRUE;
		else if (test_arg(argv[x], "-nomove", 3))
			move_flag = FALSE;
		else if (move(argv[x]) == OK)
			continue;
		else {
			ps_printf(OPT, "Unknown option %s\n", argv[x]);
			Usage(argv[0]);
			return;
		}

	}

	list_arg.lsa_object = dn;

	if ((!nocacheflag) && copy_flag)
		if ((ptr =
		     find_list_cache(dn,
				     list_arg.lsa_common.ca_servicecontrol.svc_sizelimit)) !=
		    NULLCACHE) {
			print_list_subordinates(ptr->list_subs, ptr->list_problem);
			consolidate_move();
			return;
		}

	if (rebind() != OK)
		return;

	/* Strong authentication */
	if (list_arg.lsa_common.ca_security != (struct security_parms *) 0) {
		extern struct SecurityServices *dsap_security;

		list_arg.lsa_common.ca_sig =
		    (dsap_security->serv_sign) ((caddr_t) &list_arg, _ZListArgumentDataDAS,
						&_ZDAS_mod);
	}

	while (ds_list(&list_arg, &error, &result) != DS_OK) {	/* deal with error */
		if (dish_error(OPT, &error) == 0)
			return;
		list_arg.lsa_object = error.ERR_REFERRAL.DSE_ref_candidates->cr_name;
	}

	if (result.lsr_common.cr_aliasdereferenced) {
		ps_print(RPS, "(Alias dereferenced - ");
		dn_print(RPS, result.lsr_object, EDBOUT);
		dn_free(result.lsr_object);
		ps_print(RPS, ")\n");
	}
	print_list_subordinates(result.lsr_subordinates, result.lsr_limitproblem);

	cache_list(result.lsr_subordinates, result.lsr_limitproblem, dn,
		   list_arg.lsa_common.ca_servicecontrol.svc_sizelimit);

	subords_free(result.lsr_subordinates);

	consolidate_move();
}

print_list_subordinates(ptr, prob)
	struct subordinate *ptr;
	int prob;
{
	DN adn;
	DN newdn;
	int seqno;
	extern char *result_sequence;

	adn = dn_cpy(dn);
	newdn = dn_comp_new(rdn_comp_new(NULLAttrT, NULLAttrV));
	if (adn != NULLDN)
		dn_append(adn, newdn);
	else {
		dn_free(adn);
		adn = newdn;
	}

	if (result_sequence)
		set_sequence(result_sequence);

	if (ptr == NULLSUBORD)
		if (list_show)
			ps_print(RPS, "No children\n");

	for (; ptr != NULLSUBORD; ptr = ptr->sub_next) {
		rdn_free(newdn->dn_rdn);
		dn_comp_fill(newdn, rdn_cpy(ptr->sub_rdn));
		seqno = add_sequence(adn);
		if (seqno != 0)
			ps_printf(RPS, "%-3d ", seqno);
		if (list_show)
			rdn_print(RPS, ptr->sub_rdn, READOUT);
		ps_print(RPS, "\n");
	}

	dn_free(adn);

	if (prob != LSR_NOLIMITPROBLEM)
		if (list_show)
			ps_print(RPS, "(Limit problem)\n");

}
