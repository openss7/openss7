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

/* pe2ps.c - presentation element to presentation stream */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/pe2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/pe2ps.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: pe2ps.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
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

#include <stdio.h>
#include "psap.h"
#include "tailor.h"

/*    DATA */

static PElement pe_eoc = { PE_CLASS_UNIV, PE_FORM_PRIM, PE_UNIV_EOC, 0 };
static int pe2ps_aux2();
int ps_write_id();
int ps_write_len();

/*  */

int
pe2ps_aux(ps, pe, eval)
	register PS ps;
	register PE pe;
	int eval;
{
	int result;

	if (eval > 0)
		switch (pe->pe_form) {
		case PE_FORM_PRIM:
		case PE_FORM_ICONS:
			break;

		case PE_FORM_CONS:
			(void) ps_get_abs(pe);
			break;
		}

	if ((result = pe2ps_aux2(ps, pe, eval)) != NOTOK)
		result = ps_flush(ps);

	return result;
}

static int
pe2ps_aux2(ps, pe, eval)
	register PS ps;
	register PE pe;
	int eval;
{
	register PE p;

	if (pe->pe_form == PE_FORM_ICONS) {
		if (ps_write_aux(ps, pe->pe_prim, pe->pe_len, 1) == NOTOK)
			return NOTOK;

		return OK;
	}

	if (ps_write_id(ps, pe) == NOTOK || ps_write_len(ps, pe) == NOTOK)
		return NOTOK;

	switch (pe->pe_form) {
	case PE_FORM_PRIM:
		if (ps_write_aux(ps, pe->pe_prim, pe->pe_len, 1) == NOTOK)
			return NOTOK;
		break;

	case PE_FORM_CONS:
		if (eval < 0)
			return OK;
		if (pe->pe_len) {
			for (p = pe->pe_cons; p; p = p->pe_next)
				if (pe2ps_aux2(ps, p, 0) == NOTOK)
					return NOTOK;

			if (pe->pe_len == PE_LEN_INDF && pe2ps_aux2(ps, &pe_eoc, 0) == NOTOK)
				return NOTOK;
		}
		break;
	}

	return OK;
}

/*  */

int
ps_write_id(ps, pe)
	register PS ps;
	register PE pe;
{
	byte buffer[1 + sizeof(PElementID)];
	register byte *bp = buffer;
	PElementForm form;
	register PElementID id;

	if ((form = pe->pe_form) == PE_FORM_ICONS)
		form = PE_FORM_CONS;
	*bp = ((pe->pe_class << PE_CLASS_SHIFT) & PE_CLASS_MASK)
	    | ((form << PE_FORM_SHIFT) & PE_FORM_MASK);

	if ((id = pe->pe_id) < PE_ID_XTND)
		*bp++ |= id;
	else {
		register byte *ep;
		register PElementID jd;

		*bp |= PE_ID_XTND;

		ep = buffer;
		for (jd = id; jd != 0; jd >>= PE_ID_SHIFT)
			ep++;

		for (bp = ep; id != 0; id >>= PE_ID_SHIFT)
			*bp-- = id & PE_ID_MASK;
		for (bp = buffer + 1; bp < ep; bp++)
			*bp |= PE_ID_MORE;

		bp = ++ep;
	}

	if (ps_write(ps, buffer, bp - buffer) == NOTOK)
		return NOTOK;

	return OK;
}

/*  */

/* probably should integrate the non-PE_LEN_SMAX case with the algorithm in
   num2prim() for a single, unified routine */

int
ps_write_len(ps, pe)
	register PS ps;
	register PE pe;
{
	byte buffer[1 + sizeof(PElementLen)];
	register byte *bp = buffer, *ep;
	register PElementLen len;

	if ((len = pe->pe_len) == PE_LEN_INDF)
		*bp++ = PE_LEN_XTND;
	else if (len <= PE_LEN_SMAX)
		*bp++ = len & 0xff;
	else {
		ep = buffer + sizeof buffer - 1;
		for (bp = ep; len != 0 && buffer < bp; len >>= 8)
			*bp-- = len & 0xff;
		*bp = PE_LEN_XTND | ((ep - bp) & 0xff);
		if (ps_write(ps, bp, ep - bp + 1) == NOTOK)
			return NOTOK;

		return OK;
	}

	if (ps_write(ps, buffer, bp - buffer) == NOTOK)
		return NOTOK;

	return OK;
}
