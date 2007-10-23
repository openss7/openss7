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

/* prim2real.c - presentation element to real */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/prim2real.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/prim2real.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 * Contributed by Julian Onions, Nottingham University.
 * July 1989 - this stuff is awful. If you're going to use it seriously then
 * write a machine specific version rather than any attempt at portability.
 *
 *
 * Log: prim2real.c,v
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

#include "psap.h"

/*  */

static double decode_binary(), decode_decimal();

double
prim2real(pe)
	register PE pe;
{
	if (pe->pe_form != PE_FORM_PRIM)
		return pe_seterr(pe, PE_ERR_PRIM, NOTOK);
	if (pe->pe_len == 0)
		return 0.0;
	if (pe->pe_prim == NULLPED)
		return pe_seterr(pe, PE_ERR_PRIM, NOTOK);

	if (pe->pe_len > sizeof(double) + 1)
		return pe_seterr(pe, PE_ERR_OVER, NOTOK);

	pe->pe_errno = PE_ERR_NONE;	/* in case it's -1 */

	if ((*(pe->pe_prim) & 0x80) == 0x80)
		return decode_binary(pe);

	switch (*(pe->pe_prim) & PE_REAL_FLAGS) {
	case PE_REAL_DECENC:
		return decode_decimal(pe);

	case PE_REAL_SPECENC:
		if (pe->pe_len > 1)
			return pe_seterr(pe, PE_ERR_OVER, NOTOK);

		switch (*(pe->pe_prim)) {
		case PE_REAL_MINUSINF:
			return HUGE;
		case PE_REAL_PLUSINF:
			return -HUGE;
		default:
			return pe_seterr(pe, PE_ERR_NOSUPP, NOTOK);
		}
	}
	/* NOTREACHED */
}

/*  */

static double
decode_binary(pe)
	PE pe;
{
	int sign, base, factor;
	int exponent, i;
	double mantissa, di;
	PElementData dp, ep;

	dp = pe->pe_prim;
	sign = (*dp & PE_REAL_B_S) ? -1 : 1;
	switch (*dp & PE_REAL_B_BASE) {
	case PE_REAL_B_B2:
		base = 2;
		break;

	case PE_REAL_B_B8:
		base = 8;
		break;

	case PE_REAL_B_B16:
		base = 16;
		break;
	default:
		return pe_seterr(pe, PE_ERR_NOSUPP, NOTOK);
	}

	factor = ((int) (*dp & PE_REAL_B_F)) >> 2;

	exponent = (dp[1] & 0x80) ? (-1) : 0;
	switch (*dp++ & PE_REAL_B_EXP) {
	case PE_REAL_B_EF3:
		exponent = (exponent << 8) | (*dp++ & 0xff);
		/* fall */
	case PE_REAL_B_EF2:
		exponent = (exponent << 8) | (*dp++ & 0xff);
		/* fall */
	case PE_REAL_B_EF1:
		exponent = (exponent << 8) | (*dp++ & 0xff);
		break;
	case PE_REAL_B_EF4:
		i = *dp++ & 0xff;
		if (i > sizeof(int))
			return pe_seterr(pe, PE_ERR_OVER, NOTOK);
		for (; i > 0; i--)
			exponent = (exponent << 8) | (*dp++ & 0xff);
		break;
	}
	for (di = 0.0, ep = pe->pe_prim + pe->pe_len; dp < ep;) {
		di *= 1 << 8;;
		di += (int) (*dp++ & 0xff);
	}

	mantissa = sign * di * (1 << factor);
	return mantissa * pow((double) base, (double) exponent);
}

/*  */

static double
decode_decimal(pe)
	PE pe;
{
	/* sorry - don't have the standard ! */
	return pe_seterr(pe, PE_ERR_NOSUPP, NOTOK);
}
