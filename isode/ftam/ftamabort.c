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

/* ftamabort.c - FPM: user abort */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamabort.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamabort.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamabort.c,v
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

/*    F-U-ABORT.REQUEST */

int
FUAbortRequest(sd, action, diag, ndiag, fti)
	int sd;
	int action;
	struct FTAMdiagnostic diag[];
	int ndiag;
	struct FTAMindication *fti;
{
	SBV smask;
	int result;
	register struct ftamblk *fsb;

	switch (action) {
	case FACTION_SUCCESS:
	case FACTION_TRANS:
	case FACTION_PERM:
		break;

	default:
		return ftamlose(fti, FS_GEN_NOREASON, 0, NULLCP, "bad value for action parameter");
	}
	toomuchP(diag, ndiag, NFDIAG, "diagnostic");

	smask = sigioblock();

	ftamPsig(fsb, sd);

	result = FAbortRequestAux(fsb, type_FTAM_PDU_f__u__abort__request, action,
				  diag, ndiag, fti);

	(void) sigiomask(smask);

	return result;
}

/*  */

int
FAbortRequestAux(fsb, id, action, diag, ndiag, fti)
	register struct ftamblk *fsb;
	int id, action;
	struct FTAMdiagnostic diag[];
	int ndiag;
	struct FTAMindication *fti;
{
	int result;
	PE pe;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;
	register struct type_FTAM_PDU *pdu;
	register struct type_FTAM_F__U__ABORT__request *req;

	if ((pdu = (struct type_FTAM_PDU *) calloc(1, sizeof *pdu)) == NULL)
		goto carry_on;
	pdu->offset = id;	/* F-P-ABORT-request is identical... */
	if ((req = (struct type_FTAM_F__U__ABORT__request *)
	     calloc(1, sizeof *req)) == NULL)
		goto carry_on;
	pdu->un.f__u__abort__request = req;
	if ((req->action__result = (struct type_FTAM_Action__Result *)
	     calloc(1, sizeof *req->action__result))
	    == NULL)
		goto carry_on;
	req->action__result->parm = action;
	if (ndiag > 0
	    && (req->diagnostic =
		diag2fpm(fsb,
			 id == type_FTAM_PDU_f__p__abort__request, diag, ndiag, fti)) == NULL) {
		free_FTAM_PDU(pdu);
		if (fti->fti_abort.fta_action == FACTION_TRANS)
			return NOTOK;
		pdu = NULL;
	}
      carry_on:;

	pe = NULLPE;
	if (pdu) {
		result = encode_FTAM_PDU(&pe, 1, 0, NULLCP, pdu);

		if (result == NOTOK) {
			if (pe)
				pe_free(pe), pe = NULLPE;
		} else if (pe)
			pe->pe_context = fsb->fsb_id;
	}

	fsbtrace(fsb,
		 (fsb->fsb_fd, "A-ABORT.REQUEST",
		  id != type_FTAM_PDU_f__p__abort__request ? "F-U-ABORT-request"
		  : "F-P-ABORT-request", pe, 0));

	result = AcUAbortRequest(fsb->fsb_fd, pe ? &pe : NULLPEP, pe ? 1 : 0, aci);

	if (pe)
		pe_free(pe);
	if (pdu)
		free_FTAM_PDU(pdu);

	if (result == NOTOK)
		(void) acs2ftamlose(fsb, fti, "AcUAbortRequest", aca);
	else {
		fsb->fsb_fd = NOTOK;
		result = OK;
	}

	if (id != type_FTAM_PDU_f__p__abort__request)
		freefsblk(fsb);
	else if (result == OK)
		fsb->fsb_fd = NOTOK;

	return result;
}
