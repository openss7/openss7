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

/* real2prim.c - real to presentation element */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/real2prim.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/real2prim.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 * Contributed by Julian Onions, Nottingham University.
 * July 1989 - this is awful stuff!
 *
 * Log: real2prim.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
 * Release 8.0
 *
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

PE
real2prim(d, class, id)
	register double d;
	PElementClass class;
	PElementID id;
{
	register PE pe;
	double mant, nm;
	int exponent;
	int expsign;
	int parts[sizeof(double)];
	int sign, i, maxi, mask;
	int n, explen;
	PElementData dp;

	if ((pe = pe_alloc(class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	if (d == 0.0)
		return pe;

	mant = frexp(d, &exponent);

	if (mant < 0.0) {
		sign = -1;
		mant = -mant;
	} else
		sign = 1;

	nm = mant;
	for (i = 0; i < sizeof(double); i++) {
		int intnm;

		nm *= (1 << 8);
		intnm = ((int) nm) & 0xff;
		nm -= intnm;
		if (intnm)
			maxi = i + 1;
		parts[i] = intnm;
	}

	exponent -= 8 * maxi;

	expsign = exponent >= 0 ? exponent : exponent ^ (-1);
	mask = 0x1ff << (((n = sizeof exponent) - 1) * 8 - 1);
	while (n > 1 && (expsign & mask) == 0)
		mask >>= 8, n--;

	explen = n;
	if (n > 3)
		n++;

	if ((pe->pe_prim = PEDalloc(n + maxi + 1)) == NULLPED) {
		pe_free(pe);
		return NULLPE;
	}

	dp = pe->pe_prim + (pe->pe_len = n + maxi + 1);

	for (; maxi > 0; maxi--)
		*--dp = parts[maxi - 1];
	for (n = explen; n-- > 0; exponent >>= 8)
		*--dp = exponent & 0xff;
	if (explen > 3)
		*--dp = explen & 0xff;

	switch (explen) {
	case 1:
		explen = PE_REAL_B_EF1;
		break;
	case 2:
		explen = PE_REAL_B_EF2;
		break;
	case 3:
		explen = PE_REAL_B_EF3;
		break;
	default:
		explen = PE_REAL_B_EF3;
		break;
	}
	*--dp = PE_REAL_BINENC | PE_REAL_B_B2 | (sign == -1 ? PE_REAL_B_S : 0)
	    | explen;
	return pe;
}
