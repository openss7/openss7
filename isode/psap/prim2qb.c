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

/* prim2qb.c - presentation element to qbuf */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/prim2qb.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/prim2qb.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: prim2qb.c,v
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

/*  */

struct qbuf *
prim2qb(pe)
	register PE pe;
{
	register struct qbuf *qb, *qp;
	register PE p;
	register PElementClass class;
	register PElementID id;

	if ((qb = (struct qbuf *) malloc(sizeof *qb)) == NULL)
		return pe_seterr(pe, PE_ERR_NMEM, (struct qbuf *) NULL);
	qb->qb_forw = qb->qb_back = qb;
	qb->qb_data = NULL, qb->qb_len = 0;

	switch (pe->pe_form) {
	case PE_FORM_PRIM:
		if ((qp = str2qb((char *) pe->pe_prim, (int) pe->pe_len, 0))
		    == NULL) {
			pe->pe_errno = PE_ERR_NMEM;
			goto out;
		}
		qb->qb_len = qp->qb_len;
		insque(qp, qb);
		break;

	case PE_FORM_CONS:
		if ((p = pe->pe_cons) == NULLPE)
			break;
		class = p->pe_class, id = p->pe_id;
		for (p = pe->pe_cons; p; p = p->pe_next) {
			register struct qbuf *qpp, *qbp;

			if ((p->pe_class != class || p->pe_id != id)
			    && (p->pe_class != PE_CLASS_UNIV || p->pe_id != PE_PRIM_OCTS)) {
				pe->pe_errno = PE_ERR_TYPE;
				goto out;
			}
			if ((qp = prim2qb(p)) == NULL) {
				pe->pe_errno = p->pe_errno;
				goto out;
			}

			for (qpp = qp->qb_forw; qpp != qp; qpp = qbp) {
				qbp = qpp->qb_forw;

				remque(qpp);
				insque(qpp, qb->qb_back);

				qb->qb_len += qpp->qb_len;
			}
			free((char *) qp);
		}
		break;
	}

	return qb;

      out:;
	qb_free(qb);

	return NULL;
}
