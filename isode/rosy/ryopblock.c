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

/* ryopblock.c - manage operation blocks */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosy/RCS/ryopblock.c,v 9.0 1992/06/16 12:37:29 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosy/RCS/ryopblock.c,v 9.0 1992/06/16 12:37:29 isode Rel
 *
 *
 * Log: ryopblock.c,v
 * Revision 9.0  1992/06/16  12:37:29  isode
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
#include "rosy.h"

/*    DATA */

static int once_only = 0;
static struct opsblk opsque;
static struct opsblk *OPHead = &opsque;

/*    OPERATION BLOCKS */

struct opsblk *
newopblk(sd, id)
	int sd, id;
{
	register struct opsblk *opb;

	opb = (struct opsblk *) calloc(1, sizeof *opb);
	if (opb == NULL)
		return NULL;

	opb->opb_flags |= OPB_INITIATOR;

	opb->opb_fd = sd;
	opb->opb_id = id;

	if (once_only == 0) {
		OPHead->opb_forw = OPHead->opb_back = OPHead;
		once_only++;
	}

	insque(opb, OPHead->opb_back);

	return opb;
}

/*  */

freeopblk(opb)
	register struct opsblk *opb;
{
	if (opb == NULL)
		return;

#ifdef PEPSY_DEFINITIONS
	if (opb->opb_out && opb->opb_free_mod)
		(void) fre_obj(opb->opb_out,
			       opb->opb_free_mod->md_dtab[opb->opb_free_index],
			       opb->opb_free_mod, 1);
#else
	if (opb->opb_out && opb->opb_free)
		(void) (*opb->opb_free) (opb->opb_out);
#endif

	if (opb->opb_pe)
		pe_free(opb->opb_pe);

	remque(opb);

	free((char *) opb);
}

/*  */

struct opsblk *
findopblk(sd, id, flags)
	register int sd, id, flags;
{
	register struct opsblk *opb;

	if (once_only == 0)
		return NULL;

	flags &= OPB_INITIATOR | OPB_RESPONDER;
	for (opb = OPHead->opb_forw; opb != OPHead; opb = opb->opb_forw)
		if (opb->opb_fd == sd && opb->opb_id == id && (opb->opb_flags & flags))
			return opb;

	return NULL;
}

/*  */

struct opsblk *
firstopblk(sd)
	register int sd;
{
	register struct opsblk *opb, *op2;

	if (once_only == 0)
		return NULL;

	op2 = NULLOPB;
	for (opb = OPHead->opb_forw; opb != OPHead; opb = opb->opb_forw)
		if (opb->opb_fd == sd && (opb->opb_flags & OPB_INITIATOR)) {
			if (opb->opb_flags & OPB_EVENT)
				return opb;
			if (op2 == NULLOPB)
				op2 = opb;
		}

	return op2;
}

/*  */

loseopblk(sd, reason)
	register int sd;
	int reason;
{
	register struct opsblk *opb, *op2;
	struct RoSAPindication rois;

	if (once_only == 0)
		return;

	for (opb = OPHead->opb_forw; opb != OPHead; opb = op2) {
		op2 = opb->opb_forw;

		if (opb->opb_fd == sd) {
			if (opb->opb_errfnx)
				(*opb->opb_errfnx) (sd, opb->opb_id, RY_REJECT,
						    (caddr_t) reason, &rois);

			freeopblk(opb);
		}
	}
}

/*  */

#ifdef	lint

/* VARARGS */

int
rosaplose(roi, reason, what, fmt)
	struct RoSAPindication *roi;
	int reason;
	char *what, *fmt;
{
	return rosaplose(roi, reason, what, fmt);
}
#endif
