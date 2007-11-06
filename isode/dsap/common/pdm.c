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

/* pdm.c - preferred delivery method handling */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/pdm.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/pdm.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: pdm.c,v
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

/*
	SYNTAX:
		pdm = <pdm_component> | <pdm_component> '$' <pdm>
		pdm_component = "any" | "mhs" | "physical" | "telex"
				| "teletex" | "g3fax" | "g4fax" | "ia5"
				| "videotex" | "telephone"

	EXAMPLE:
		mhs $ physical $ telex $ telephone
*/

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "cmd_srch.h"
#include "quipu/syntaxes.h"

static CMD_TABLE pdm_table[] = {
	"ANY", 0,
	"MHS", 1,
	"PHYSICAL", 2,
	"TELEX", 3,
	"TELETEX", 4,
	"G3FAX", 5,
	"G4FAX", 6,
	"IA5", 7,
	"VIDEOTEX", 8,
	"TELEPHONE", 9,
	"UNKNOWN", -1,
	0, -1
};

static
pdmfree(pdm)
	struct pref_deliv *pdm;
{
	struct pref_deliv *next;

	for (; pdm != (struct pref_deliv *) NULL; pdm = next) {
		next = pdm->pd_next;
		free((char *) pdm);
	}
}

static
pdmcmp(a, b)
	struct pref_deliv *a, *b;
{
	/* matching here is a bit dubious !!! */

	for (; (a != (struct pref_deliv *) NULL) && (b != (struct pref_deliv *) NULL);
	     a = a->pd_next, b = b->pd_next)
		if (a->deliv != b->deliv)
			return (a->deliv > b->deliv ? 1 : -1);

	if (a != b)
		return (a > b ? 1 : -1);
	else
		return (0);

}

static struct pref_deliv *
pdmcpy(a)
	struct pref_deliv *a;
{
	struct pref_deliv *b, *c, *result = (struct pref_deliv *) NULL;

	c = result;		/* to keep lint happy */

	for (; a != (struct pref_deliv *) NULL; a = a->pd_next) {
		b = (struct pref_deliv *) smalloc(sizeof(struct pref_deliv));
		b->deliv = a->deliv;

		if (result == (struct pref_deliv *) NULL)
			result = b;
		else
			c->pd_next = b;
		c = b;
	}

	b->pd_next = (struct pref_deliv *) NULL;
	return (result);
}

static struct pref_deliv *
pdmparse(str)
	char *str;
{
	struct pref_deliv *result = (struct pref_deliv *) NULL;
	struct pref_deliv *a, *b;
	char *ptr;
	char *mark = NULLCP;

	b = result;		/* to keep lint happy */

	for (;;) {
		mark = NULLCP;
		a = (struct pref_deliv *) smalloc(sizeof(struct pref_deliv));

		if ((ptr = index(str, '$')) != NULLCP) {
			*ptr-- = 0;
			if (isspace(*ptr)) {
				*ptr = 0;
				mark = ptr;
			}
			ptr++;
		}

		if ((a->deliv = cmd_srch(str, pdm_table)) == -1) {
			parse_error("Unknown method %s", str);
			return ((struct pref_deliv *) NULL);
		}

		if (result == (struct pref_deliv *) NULL)
			result = a;
		else
			b->pd_next = a;
		b = a;

		if (ptr != NULLCP) {
			*ptr++ = '$';
			if (mark != NULLCP)
				*mark = ' ';
			str = (SkipSpace(ptr));
			ptr = str;
		} else
			break;
	}
	a->pd_next = (struct pref_deliv *) NULL;

	return (result);
}

static
pdmprint(ps, pdm, format)
	PS ps;
	struct pref_deliv *pdm;
	int format;
{
	char *prefix = NULLCP;

	for (; pdm != (struct pref_deliv *) NULL; pdm = pdm->pd_next) {
		if (prefix != NULLCP)
			ps_print(ps, prefix);

		ps_print(ps, rcmd_srch(pdm->deliv, pdm_table));

		if (format == READOUT)
			prefix = " or ";
		else
			prefix = " $ ";
	}
}

static PE
pdmenc(m)
	struct pref_deliv *m;
{
	PE ret_pe;

	(void) encode_SA_PreferredDeliveryMethod(&ret_pe, 0, 0, NULLCP, m);

	return (ret_pe);
}

static struct pref_deliv *
pdmdec(pe)
	PE pe;
{
	struct pref_deliv *m;

	if (decode_SA_PreferredDeliveryMethod(pe, 1, NULLIP, NULLVP, &m) == NOTOK)
		return ((struct pref_deliv *) NULL);
	return (m);
}

pref_deliv_syntax()
{
	(void) add_attribute_syntax("DeliveryMethod",
				    (IFP) pdmenc, (IFP) pdmdec,
				    (IFP) pdmparse, pdmprint,
				    (IFP) pdmcpy, pdmcmp, pdmfree, NULLCP, NULLIFP, TRUE);

}
