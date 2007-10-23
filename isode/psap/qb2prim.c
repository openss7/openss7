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

/* qb2prim.c - octet string to primitive */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/qb2prim.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/qb2prim.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: qb2prim.c,v
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

/* form: PRIMitive or CONStructor --

	qb2prim  -	octet string (via qbufs) to presentation element

 */

/*  */

PE
qb2prim_aux(qb, class, id, in_line)
	register struct qbuf *qb;
	PElementClass class;
	PElementID id;
	int in_line;
{
	register PE pe, p;
	register struct qbuf *qp;

	if (qb == NULL)
		return NULLPE;

	if ((qp = qb->qb_forw) == qb || qp->qb_forw == qb) {
		if ((pe = pe_alloc(class, PE_FORM_PRIM, id)) == NULLPE)
			return NULLPE;

		if (in_line) {
			if (pe->pe_len = qp->qb_len)
				pe->pe_prim = (PElementData) qp->qb_data;
			pe->pe_inline = 1;
		} else if (pe->pe_len = qp->qb_len) {
			if ((pe->pe_prim = PEDalloc(pe->pe_len)) == NULLPED)
				goto no_mem;
			PEDcpy(qp->qb_data, pe->pe_prim, pe->pe_len);
		}
	} else {
		if ((pe = pe_alloc(class, PE_FORM_CONS, id)) == NULLPE)
			return NULLPE;

		do {
			if (seq_add(pe, p = pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM,
						     PE_PRIM_OCTS), -1) == NOTOK) {
			      no_mem:;
				pe_free(pe);
				return NULLPE;
			}

			p->pe_len = qp->qb_len;
			if (in_line) {
				p->pe_prim = (PElementData) qp->qb_data;
				p->pe_inline = 1;
			} else {
				if ((p->pe_prim = PEDalloc(p->pe_len)) == NULLPED)
					goto no_mem;
				PEDcpy(qp->qb_data, p->pe_prim, p->pe_len);
			}

			qp = qp->qb_forw;
		}
		while (qp != qb);
	}

	return pe;
}
