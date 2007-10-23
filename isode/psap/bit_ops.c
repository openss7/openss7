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

/* bit_opts.c - operations on bit strings */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/bit_ops.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/bit_ops.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: bit_ops.c,v
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

/* the first octet indicates how many unused bits are in the last octet --

	prim2bit  -	presentation element to bit string
	bit2prim  -	bit string to presentation element
	bit_on	  -	turn a bit ON
	bit_off	  -	turn a bit OFF
	bit_test  -	test a bit
 */

static PElementData ffb(), ffb_aux();
static PE ffb_pe();

/*  */

int
bit_on(pe, i)
	register PE pe;
	register int i;
{
	int mask;
	register PElementData bp;

	if ((bp = ffb(pe, i, &mask, 1)) == NULLPED)
		return pe_seterr(pe, PE_ERR_NMEM, NOTOK);

	*bp |= mask;

	return OK;
}

/*  */

int
bit_off(pe, i)
	register PE pe;
	register int i;
{
	int mask;
	register PElementData bp;

	if ((bp = ffb(pe, i, &mask, 1)) == NULLPED)
		return pe_seterr(pe, PE_ERR_NMEM, NOTOK);

	*bp &= ~mask;

	return OK;
}

/*  */

int
bit_test(pe, i)
	register PE pe;
	register int i;
{
	int mask;
	register PElementData bp;

	if ((bp = ffb(pe, i, &mask, 0)) == NULLPED)
		return pe_seterr(pe, PE_ERR_BIT, NOTOK);

	return (*bp & mask ? 1 : 0);
}

/*  */

static PElementData
ffb(pe, n, mask, xtnd)
	register PE pe;
	register int n, *mask, xtnd;
{
	register int len, i;
	int j;
	register PElementData bp;
	register PE *p, q, r;

	i = (j = n) / 8 + 1;
	if ((bp = ffb_aux(pe, &j, mask)) != NULLPED || !xtnd)
		return bp;

	if (pe->pe_form == PE_FORM_CONS)
		pe = ffb_pe(pe);

	switch (pe->pe_form) {
	case PE_FORM_PRIM:
		if (pe->pe_len < (PElementLen) (len = i + 1)) {
			if ((bp = PEDalloc(len)) == NULLPED)
				return NULLPED;
			bzero((char *) bp, len);
			if (pe->pe_prim) {
				PEDcpy(pe->pe_prim, bp, pe->pe_len);
				if (pe->pe_inline)
					pe->pe_inline = 0;
				else
					PEDfree(pe->pe_prim);
			}
			pe->pe_prim = bp, pe->pe_len = len;
		}
		pe->pe_nbits = n + 1;
		*mask = 1 << (7 - (n % 8));
		return (pe->pe_prim + i);

	case PE_FORM_CONS:
		if ((r = pe_alloc(pe->pe_class, PE_FORM_PRIM, pe->pe_id))
		    == NULLPE)
			return NULLPED;
		if ((r->pe_prim = PEDalloc(len = r->pe_len = j / 8 + 2))
		    == NULLPED) {
			pe_free(r);
			return NULLPED;
		}
		bzero((char *) r->pe_prim, len);
		r->pe_nbits = j + 1;
		*mask = 1 << (7 - (j % 8));
		for (p = &pe->pe_cons; q = *p; p = &q->pe_next)
			continue;
		*p = r;
		return (r->pe_prim + len - 1);

	default:
		return NULLPED;
	}
}

/*  */

static PElementData
ffb_aux(pe, n, mask)
	register PE pe;
	register int *n, *mask;
{
	register int i, nbits;
	register PElementData bp;
	register PE p;

	switch (pe->pe_form) {
	case PE_FORM_PRIM:
		if ((nbits = pe->pe_nbits) > (i = *n)) {
			*mask = 1 << (7 - (i % 8));
			return (pe->pe_prim + i / 8 + 1);
		}
		*n -= nbits;
		break;

	case PE_FORM_CONS:
		for (p = pe->pe_cons; p; p = p->pe_next)
			if ((bp = ffb_aux(p, n, mask)) != NULLPED)
				return bp;
		break;
	}

	return NULLPED;
}

/*  */

static PE
ffb_pe(pe)
	register PE pe;
{
	register PE p, q;

	for (p = pe->pe_cons, q = NULLPE; p; q = p, p = p->pe_next)
		continue;

	if (q != NULLPE)
		switch (q->pe_form) {
		case PE_FORM_PRIM:
			return q;

		case PE_FORM_CONS:
			return ffb_pe(q);
		}

	return pe;
}
