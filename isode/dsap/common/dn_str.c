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

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/dn_str.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/dn_str.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: dn_str.c,v
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

#include "quipu/util.h"
#include "quipu/name.h"

static char dn_alias;

extern int dn_cmp();
extern int dn_free();
extern char *SkipSpace();

short syntax_dn = 0;

DN
str2dn(str)
	register char *str;
{
	register char *ptr;
	char *save, val, sep = '@';
	char *aliasptr;
	DN dn = NULLDN, newdn, tmpdn;
	RDN rdn;
	int rfcformat = 0;
	char *alias2name();

	if (str == NULLCP)
		return (NULLDN);

	if (*str == '<') {
		/* try 'rfc' string format instead */
		rfcformat = 1;
		str++;		/* remove leading bracket */
		if (isspace(*str))
			str++;
		sep = ',';
	}

	if ((rfcformat == 0) && (*str == '@'))	/* Skip leading '@' signs for dish compat */
		str++;

	while ((ptr = index(str, sep)) != 0) {
		save = ptr++;
		if (isspace(*(save - 1))) {
			save--;
			if (*(save - 1) == '=')
				save++;	/* Let "type= " pass by */
		}
		val = *save;
		*save = 0;

		if ((rfcformat == 0) && (dn == NULLDN)) {
			/* try str as an alias */
			if ((aliasptr = alias2name(SkipSpace(str))) != NULLCP) {
				dn_alias = TRUE;
				if ((newdn = str2dn(aliasptr)) == NULLDN) {
					parse_error("Invalid alias '%s'", aliasptr);
					dn_free(dn);
					return (NULLDN);
				}
				dn = newdn;
				*save = val;
				str = ptr;
				continue;
			}
		}

		if ((rdn = str2rdn(str)) == NULLRDN) {
			dn_free(dn);
			return (NULLDN);
		}
		if (dn == NULLDN)
			dn = dn_comp_new(rdn);
		else {
			if (rfcformat == 0)
				dn_append(dn, dn_comp_new(rdn));
			else {
				tmpdn = dn;
				dn = dn_comp_new(rdn);
				dn->dn_parent = tmpdn;
			}
		}
		*save = val;
		str = ptr;
	}

	/* try str as an alias */
	if ((rfcformat == 0) && (dn == NULLDN)) {
		if ((aliasptr = alias2name(SkipSpace(str))) != NULLCP) {
			dn_alias = TRUE;
			if ((newdn = str2dn(aliasptr)) == NULLDN) {
				parse_error("Invalid alias '%s'", aliasptr);
				dn_free(dn);
				return (NULLDN);
			}
			return (newdn);
		}
	}

	if (rfcformat == 1) {
		register char *p;

		p = str;
		while (*p && *p != '>')
			p++;
		if (*p == '>') {
			*p-- = 0;
			if (isspace(*p))
				*p = 0;
		}
	}

	if ((rdn = str2rdn(str)) == NULLRDN) {
		dn_free(dn);
		return (NULLDN);
	}

	if (dn == NULLDN)
		dn = dn_comp_new(rdn);
	else {
		if (rfcformat == 0)
			dn_append(dn, dn_comp_new(rdn));
		else {
			tmpdn = dn;
			dn = dn_comp_new(rdn);
			dn->dn_parent = tmpdn;
		}
	}
	return (dn);
}

DN
str2dn_aux(str, alias)
	char *str;
	char *alias;
{
	DN dn;

	dn_alias = FALSE;
	dn = str2dn(str);
	*alias = dn_alias;
	return (dn);
}

DN
str2dnX(str)
	register char *str;
{
	register char *ptr;

	if ((ptr = rindex(str, '#')) != 0) {
		/* a bit or reverse compatability... */
		if (*++ptr != 0) {
			parse_error("invalid # in '%s'", str);
			return (NULLDN);
		} else
			*--ptr = 0;
	}
	return (str2dn(str));
}

DN
dn_dec(pe)
	PE pe;
{
	DN adn;

	if (decode_IF_DistinguishedName(pe, 1, NULLIP, NULLVP, &adn) == NOTOK)
		return (NULLDN);

	return (adn);
}

PE
dn_enc(dn)
	DN dn;
{
	PE ret_pe;

	(void) encode_IF_DistinguishedName(&ret_pe, 0, 0, NULLCP, dn);
	return (ret_pe);
}

dn_syntax()
{
	syntax_dn = add_attribute_syntax("dn",
					 (IFP) dn_enc, (IFP) dn_dec,
					 (IFP) str2dnX, (IFP) dn_print,
					 (IFP) dn_cpy, dn_cmp, dn_free, NULLCP, NULLIFP, TRUE);
}
