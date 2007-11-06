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

/* acsapblock.c - manage association blocks */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsapblock.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsapblock.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsapblock.c,v
 * Revision 9.0  1992/06/16  12:05:59  isode
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
#include "acpkt.h"

/*    DATA */

static int once_only = 0;
static struct assocblk assocque;
static struct assocblk *ACHead = &assocque;

/*    ASSOCIATION BLOCKS */

struct assocblk *
newacblk()
{
	register struct assocblk *acb;

	acb = (struct assocblk *) calloc(1, sizeof *acb);
	if (acb == NULL)
		return NULL;

	acb->acb_fd = NOTOK;
	acb->acb_actno = 1;

	if (once_only == 0) {
		ACHead->acb_forw = ACHead->acb_back = ACHead;
		once_only++;
	}

	insque(acb, ACHead->acb_back);

	return acb;
}

/*  */

freeacblk(acb)
	register struct assocblk *acb;
{
	if (acb == NULL)
		return;

	if (acb->acb_flags & ACB_STICKY) {
		acb->acb_flags &= ~ACB_STICKY;
		return;
	}

	if (acb->acb_fd != NOTOK && acb->acb_uabort)
		if (acb->acb_flags & ACB_ACS) {
			if (acb->acb_flags & ACB_RTS) {	/* recurse */
				struct AcSAPindication acis;

				(void) (*acb->acb_uabort) (acb->acb_fd, NULLPEP, 0, &acis);
				return;
			} else {
				struct PSAPindication pis;

				(void) (*acb->acb_uabort) (acb->acb_fd, NULLPEP, 0, &pis);
			}
		} else {
			struct SSAPindication sis;

			(void) (*acb->acb_uabort) (acb->acb_fd, NULLCP, 0, &sis);
		}

	if (acb->acb_flags & ACB_FINISH)
		ACFFREE(&acb->acb_finish);

	if (acb->acb_context)
		oid_free(acb->acb_context);
	if (acb->acb_retry)
		pe_free(acb->acb_retry);

	FREEACB(acb);

	if (acb->acb_apdu)
		pe_free(acb->acb_apdu);

	remque(acb);

	free((char *) acb);
}

/*  */

struct assocblk *
findacblk(sd)
	register int sd;
{
	register struct assocblk *acb;

	if (once_only == 0)
		return NULL;

	for (acb = ACHead->acb_forw; acb != ACHead; acb = acb->acb_forw)
		if (acb->acb_fd == sd)
			return acb;

	return NULL;
}
