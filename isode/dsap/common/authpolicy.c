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

/* authpolicy.c - authentication policy routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/authpolicy.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/authpolicy.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: authpolicy.c,v
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
#include "quipu/entry.h"
#include "quipu/syntaxes.h"

/*
 * These routines implement the AuthenticationPolicySyntax.  The EDB
 * representation of this syntax is as follows:
 *
 *  AuthenticationPolicySyntax ::= <modpolicy> '#' <readandcomparepolicy>
 *					'#' <listandsearchpolicy>
 *
 *  <modpolicy> ::= <authpolicy>
 *
 *  <readandcomparepolicy> ::= <authpolicy>
 *
 *  <listandsearchpolicy> ::= <authpolicy>
 *
 *  <authpolicy> ::= 'TRUST' | 'SIMPLE' | 'STRONG'
 */

short authp_sntx;

authp_cmp(a, b)
	Authpolicy a;
	Authpolicy b;
{
	if (a == NULLAUTHP && b == NULLAUTHP)
		return (0);

	if (a == NULLAUTHP)
		return (1);

	if (b == NULLAUTHP)
		return (-1);

	if (a->ap_modification != b->ap_modification)
		return (a->ap_modification > b->ap_modification ? 1 : -1);

	if (a->ap_readandcompare != b->ap_readandcompare)
		return (a->ap_readandcompare > b->ap_readandcompare ? 1 : -1);

	if (a->ap_listandsearch != b->ap_listandsearch)
		return (a->ap_listandsearch > b->ap_listandsearch ? 1 : -1);

	return (0);
}

static Authpolicy
authp_cpy(ap)
	Authpolicy ap;
{
	Authpolicy new;

	new = authp_alloc();
	*new = *ap;

	return (new);
}

static Authpolicy
authp_decode(pe)
	PE pe;
{
	Authpolicy ap;

	if (decode_Quipu_AuthenticationPolicySyntax(pe, 1, NULLIP, NULLVP, &ap) == NOTOK) {
		return (NULLAUTHP);
	}

	return (ap);
}

static PE
authp_enc(ap)
	Authpolicy ap;
{
	PE ret_pe;

	(void) encode_Quipu_AuthenticationPolicySyntax(&ret_pe, 0, 0, NULLCP, ap);

	return (ret_pe);
}

static
get_policy(str)
	char *str;
{
	/* get modification policy */
	if (lexnequ(str, "trust", 5) == 0) {
		return (AP_TRUST);
	} else if (lexnequ(str, "simple", 6) == 0) {
		return (AP_SIMPLE);
	} else if (lexnequ(str, "strong", 6) == 0) {
		return (AP_STRONG);
	} else {
		parse_error("unknown authentication policy '%s'", str);
		return (NOTOK);
	}
}

static Authpolicy
str2authp(str)
	char *str;
{
	Authpolicy new;
	char save, *s;

	if ((s = index(str, '#')) == NULL) {
		parse_error("# missing in authentication policy '%s'", str);
		return (NULLAUTHP);
	}
	save = *s;
	*s = '\0';
	while (isspace(*str))
		str++;

	new = authp_alloc();

	new->ap_modification = get_policy(str);

	/* skip past next # */
	*s++ = save;
	str = s;
	if ((s = index(str, '#')) == NULL) {
		parse_error("# missing in authentication policy '%s'", str);
		free((char *) new);
		return (NULLAUTHP);
	}
	save = *s;
	*s = '\0';
	while (isspace(*str))
		str++;

	new->ap_readandcompare = get_policy(str);

	/* skip past next # */
	*s++ = save;
	str = s;
	while (isspace(*str))
		str++;

	new->ap_listandsearch = get_policy(str);

	return (new);
}

static char *policy[] = {
	"trust",
	"simple",
	"strong"
};

authp_print(ps, ap, format)
	PS ps;
	Authpolicy ap;
	int format;
{
	if (format == READOUT) {
		ps_printf(ps, "modification policy: %s\n", policy[ap->ap_modification]);
		ps_printf(ps, "\t\t\tread and compare policy: %s\n", policy[ap->ap_readandcompare]);
		ps_printf(ps, "\t\t\tlist and search policy: %s", policy[ap->ap_listandsearch]);
	} else {
		ps_printf(ps, "%s # %s # %s", policy[ap->ap_modification],
			  policy[ap->ap_readandcompare], policy[ap->ap_listandsearch]);
	}
}

authp_syntax()
{
	extern sfree();

	authp_sntx = add_attribute_syntax("AuthenticationPolicySyntax",
					  (IFP) authp_enc, (IFP) authp_decode,
					  (IFP) str2authp, authp_print,
					  (IFP) authp_cpy, authp_cmp, sfree, NULLCP, NULLIFP, TRUE);
}
