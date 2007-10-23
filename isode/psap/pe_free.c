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

/* pe_free.c - free a presentation element */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/pe_free.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/pe_free.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: pe_free.c,v
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

/*  */

int
pe_free(pe)
	register PE pe;
{
	register PE p, q;
	register struct qbuf *qb, *qp;

	if (!pe)
		abort();

	if (pe->pe_refcnt < 0) {
		DLOG(psap_log, LLOG_DEBUG, ("WARNING: duplicately free'd pe 0x%x!", pe));
		return;
	}

	if (pe->pe_refcnt-- > 0)
		return;

	switch (pe->pe_form) {
	case PE_FORM_PRIM:
	case PE_FORM_ICONS:
		if (pe->pe_prim && !pe->pe_inline)
			PEDfree(pe->pe_prim);
		else if (pe->pe_inline && Hqb) {
			if (Fqb && (--Qbrefs == 0)) {
				for (qb = Fqb; qb && (qb != Hqb); qb = qp) {
					qp = qb->qb_forw;
					free((char *) qb);
				}
				if (!qb)
					abort();
				Fqb = Hqb = NULL;
			}
		}
		break;

	case PE_FORM_CONS:
		for (p = pe->pe_cons; p; p = q) {
			q = p->pe_next;
			pe_free(p);
		}
		break;

	default:
		abort();
		/* NOTREACHED */
	}

	if (pe->pe_realbase)
		free(pe->pe_realbase);

	pe_frees++;
	pe->pe_next = pe_list;
	pe_list = pe;
#ifdef	DEBUG
	if (psap_log->ll_events & LLOG_DEBUG) {
		PE *pep;

		for (pep = &pe_active; p = *pep; pep = &p->pe_link)
			if (p == pe) {
				*pep = p->pe_link;
				break;
			}
		if (!p)
			DLOG(psap_log, LLOG_DEBUG,
			     ("WARNING: free'd pe (0x%x) not on active list", pe));
	}
#endif
}
