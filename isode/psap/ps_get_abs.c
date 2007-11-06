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

/* ps_get_abs.c - get absolute length */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/ps_get_abs.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/ps_get_abs.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: ps_get_abs.c,v
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

/*    DATA */

int ps_len_strategy = PS_LEN_SPAG;
static int ps_get_id();
static int ps_get_len();

/*  */

int
ps_get_abs(pe)
	register PE pe;
{
	register PElementLen len;
	register PE p;

	switch (pe->pe_form) {
	case PE_FORM_PRIM:
		len = pe->pe_len;
		break;

	case PE_FORM_CONS:
		len = 0;
		for (p = pe->pe_cons; p; p = p->pe_next)
			len += ps_get_abs(p);

		switch (ps_len_strategy) {
		case PS_LEN_SPAG:
		default:
			if (len <= PE_LEN_SMAX) {
				pe->pe_len = len;
				break;
			}
			/* else fall */
		case PS_LEN_INDF:
			pe->pe_len = PE_LEN_INDF;
			len += 2;	/* for EOC */
			break;

		case PS_LEN_LONG:
			pe->pe_len = len;
			break;
		}
		break;

	case PE_FORM_ICONS:
		return pe->pe_len;
	}

	return (ps_get_id(pe) + ps_get_len(pe) + len);
}

/*  */

static int
ps_get_id(pe)
	register PE pe;
{
	register int i;
	register PElementID id;

	if ((id = pe->pe_id) < PE_ID_XTND)
		return 1;

	for (i = 1; id != 0; id >>= PE_ID_SHIFT)
		i++;
	return i;
}

/*  */

static int
ps_get_len(pe)
	register PE pe;
{
	register int i;
	register PElementLen len;

	if ((len = pe->pe_len) == PE_LEN_INDF || len <= PE_LEN_SMAX)
		return 1;

	for (i = 1; len > 0; len >>= 8)
		i++;
	return i;
}
