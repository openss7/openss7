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

/* ftamaccess1.c - FPM: initiate file access */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamaccess1.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamaccess1.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamaccess1.c,v
 * Revision 9.0  1992/06/16  12:14:55  isode
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
#include <signal.h>
#include "fpkt.h"

/*    F-{LOCATE,ERASE}.REQUEST */

int
FAccessRequest(sd, operation, identity, lock, fti)
	int sd;
	int operation;
	struct FADUidentity *identity;
	int lock;			/* F-LOCATE.REQUEST only */
	struct FTAMindication *fti;
{
	SBV smask;
	int result, state;
	register struct ftamblk *fsb;

	switch (operation) {
	case FA_OPS_LOCATE:
		state = FSB_LOCATE;
		break;

	case FA_OPS_ERASE:
		state = FSB_ERASE;
		break;

	default:
		return ftamlose(fti, FS_GEN_NOREASON, 0, NULLCP,
				"bad value for operation parameter");
	}
	missingP(identity);
	missingP(fti);

	smask = sigioblock();

	ftamPsig(fsb, sd);

	result = FAccessRequestAux(fsb, state, identity, lock, fti);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
FAccessRequestAux(fsb, state, identity, lock, fti)
	register struct ftamblk *fsb;
	int state;
	struct FADUidentity *identity;
	int lock;
	struct FTAMindication *fti;
{
	int result;
	PE pe;
	struct PSAPindication pis;
	struct PSAPindication *pi = &pis;
	struct PSAPabort *pa = &pi->pi_abort;
	register struct type_FTAM_PDU *pdu;
	register struct type_FTAM_F__LOCATE__request *req;

	if (!(fsb->fsb_flags & FSB_INIT))
		return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "not initiator");
	if (fsb->fsb_state != FSB_DATAIDLE)
		return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "wrong state");
	if (!(fsb->fsb_units & FUNIT_ACCESS))
		return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "file access not allowed");

	pe = NULLPE;
	if ((pdu = (struct type_FTAM_PDU *) calloc(1, sizeof *pdu)) == NULL) {
	      no_mem:;
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
	      out:;
		if (pe)
			pe_free(pe);
		if (pdu)
			free_FTAM_PDU(pdu);
		if (fti->fti_abort.fta_action == FACTION_PERM)
			freefsblk(fsb);
		return NOTOK;
	}
	/* F-ERASE-request is identical... */
	pdu->offset = state != FSB_LOCATE ? type_FTAM_PDU_f__erase__request
	    : type_FTAM_PDU_f__locate__request;
	if ((req = (struct type_FTAM_F__LOCATE__request *)
	     calloc(1, sizeof *req)) == NULL)
		goto no_mem;
	pdu->un.f__locate__request = req;
	if ((req->file__access__data__unit__identity = faduid2fpm(fsb, identity, fti)) == NULL)
		goto out;
	if ((fsb->fsb_units & FUNIT_FADULOCK) && state == FSB_LOCATE) {
		if ((req->fadu__lock = (struct type_FTAM_FADU__Lock *)
		     calloc(1, sizeof *req->fadu__lock))
		    == NULL)
			goto no_mem;
		req->fadu__lock->parm = lock ? int_FTAM_FADU__Lock_on : int_FTAM_FADU__Lock_off;
	}

	if (encode_FTAM_PDU(&pe, 1, 0, NULLCP, pdu) == NOTOK) {
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "error encoding PDU: %s", PY_pepy);
		goto out;
	}

	pe->pe_context = fsb->fsb_id;

	fsbtrace(fsb, (fsb->fsb_fd, "P-DATA.REQUEST",
		       state == FSB_LOCATE ? "F-LOCATE-request" : "F-ERASE-request", pe, 0));

	result = PDataRequest(fsb->fsb_fd, &pe, 1, pi);

	pe_free(pe);
	pe = NULLPE;
	free_FTAM_PDU(pdu);
	pdu = NULL;

	if (result == NOTOK) {
		(void) ps2ftamlose(fsb, fti, "PDataRequest", pa);
		goto out;
	}

	fsb->fsb_state = state;

	return FWaitRequestAux(fsb, NOTOK, fti);
}
