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
    "Header: /xtel/isode/isode/dsap/common/RCS/dn_print.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/dn_print.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: dn_print.c,v
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

void
dn_print(ps, dn, format)
	DN dn;
	PS ps;
	int format;
{
	register DN eptr;

	if (dn == NULLDN) {
		if (format == READOUT)
			ps_print(ps, "NULL DN");
		return;
	}

	if (format == UFNOUT) {
		ufn_dn_print(ps, dn, 1);
		return;
	}

	dn_comp_print(ps, dn, format);

	for (eptr = dn->dn_parent; eptr != NULLDN; eptr = eptr->dn_parent) {
		switch (format) {
		case DIROUT:
			ps_print(ps, "/");
			break;
		case FILEOUT:
		case RDNOUT:
		case EDBOUT:
			ps_print(ps, "@");
			break;
		case READOUT:
			ps_print(ps, "\n\t\t\t");
			break;
		}
		dn_comp_print(ps, eptr, format);
	}
}

static void
dn_rprint(ps, dn, sep)
	PS ps;
	DN dn;
	char *sep;
{
	if (dn->dn_parent != NULLDN) {
		dn_rprint(ps, dn->dn_parent, sep);
		ps_printf(ps, ",%s", sep);
	}

	dn_comp_print(ps, dn, READOUT);
}

void
dn_rfc_print(ps, dn, sep)
	PS ps;
	DN dn;
	char *sep;
{
	if (dn == NULLDN) {
		ps_print(ps, "NULL DN");
		return;
	}

	dn_rprint(ps, dn, sep);
}

/*  */

static DN localdn = NULLDN;
extern char *local_dit;

void
ufn_dn_print(ps, dn, multiline)
	PS ps;
	DN dn;
	int multiline;
{
	if (dn == NULLDN)
		return;

	if (localdn == NULLDN)
		localdn = str2dn(local_dit);

	(void) ufn_dn_print_aux(ps, dn, localdn, multiline);
}

int ufn_indent = 16;

int
ufn_dn_print_aux(ps, dn, marker, multiline)
	PS ps;
	DN dn;
	DN marker;
	int multiline;
{
	DN next = NULLDN;
	char res = 0;
	char this_one = TRUE;

	if ((marker != NULLDN) && (dn_comp_cmp(dn, marker) == 0)) {
		next = marker->dn_parent;
		if (dn->dn_parent != NULLDN)
			this_one = FALSE;
	}

	if (dn->dn_parent != NULLDN)
		res = ufn_dn_print_aux(ps, dn->dn_parent, next, multiline);

	if (this_one) {
		if (res) {
			if (multiline < 0 || (multiline && res > 1)) {
				ps_printf(ps, ",\n");
				if (ufn_indent > 0)
					ps_printf(ps, "%*s", ufn_indent, "");
			} else
				ps_print(ps, ", ");
		}
		ufn_rdn_print(ps, dn->dn_rdn);
	}
	return ++res;
}

void
ufn_rdn_print(ps, rdn)
	RDN rdn;
	PS ps;
{
	register RDN eptr;

	if (rdn == NULLRDN)
		return;

	AttrV_print(ps, &rdn->rdn_av, READOUT);

	if (rdn->rdn_next != NULLRDN)
		for (eptr = rdn->rdn_next; eptr != NULLRDN; eptr = eptr->rdn_next) {
			ps_print(ps, " + ");
			AttrV_print(ps, &eptr->rdn_av, READOUT);
		}

}

static PS ps = NULLPS;

char *
dn2str(dn)
	DN dn;
{
	char *cp;

	if (ps == NULL && ((ps = ps_alloc(str_open)) == NULLPS)
	    || str_setup(ps, NULLCP, BUFSIZ, 0) == NOTOK) {
		if (ps)
			ps_free(ps), ps = NULLPS;

		return NULLCP;
	}

	dn_print(ps, dn, EDBOUT);
	ps_print(ps, " ");
	*--ps->ps_ptr = NULL, ps->ps_cnt++;

	cp = ps->ps_base;

	ps->ps_base = NULL, ps->ps_cnt = 0;
	ps->ps_ptr = NULL, ps->ps_bufsiz = 0;

	return cp;
}

char *
dn2ufn(dn, multiline)
	DN dn;
	int multiline;
{
	char *cp;

	if (ps == NULL && ((ps = ps_alloc(str_open)) == NULLPS)
	    || str_setup(ps, NULLCP, BUFSIZ, 0) == NOTOK) {
		if (ps)
			ps_free(ps), ps = NULLPS;

		return NULLCP;
	}

	ufn_dn_print(ps, dn, multiline);
	ps_print(ps, " ");
	*--ps->ps_ptr = NULL, ps->ps_cnt++;

	cp = ps->ps_base;

	ps->ps_base = NULL, ps->ps_cnt = 0;
	ps->ps_ptr = NULL, ps->ps_bufsiz = 0;

	return cp;
}

char *
dn2rfc(dn, sep)
	DN dn;
	char *sep;
{
	char *cp;

	if (ps == NULL && ((ps = ps_alloc(str_open)) == NULLPS)
	    || str_setup(ps, NULLCP, BUFSIZ, 0) == NOTOK) {
		if (ps)
			ps_free(ps), ps = NULLPS;

		return NULLCP;
	}

	ps_print(ps, "<");
	dn_rfc_print(ps, dn, sep);
	ps_print(ps, "> ");
	*--ps->ps_ptr = NULL, ps->ps_cnt++;

	cp = ps->ps_base;

	ps->ps_base = NULL, ps->ps_cnt = 0;
	ps->ps_ptr = NULL, ps->ps_bufsiz = 0;

	return cp;
}
