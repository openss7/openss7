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

/* ftambulk.c - FPM: bulk data transfer */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftambulk.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftambulk.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftambulk.c,v
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

/*    F-DATA.REQUEST */

int
FDataRequest(sd, fadus, nfadu, fti)
	int sd;
	PE fadus[];
	int nfadu;
	struct FTAMindication *fti;
{
	SBV smask;
	int result;
	register struct ftamblk *fsb;

	missingP(fti);

	smask = sigioblock();

	ftamPsig(fsb, sd);

	result = FDataRequestAux(fsb, fadus, nfadu, fti);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
FDataRequestAux(fsb, fadus, nfadu, fti)
	register struct ftamblk *fsb;
	PE fadus[];
	int nfadu;
	struct FTAMindication *fti;
{
	register int i;
	PE pe, *pep;
	struct PSAPindication pis;
	struct PSAPindication *pi = &pis;
	struct PSAPabort *pa = &pi->pi_abort;

	switch (fsb->fsb_state) {
	case FSB_DATAREAD:
		if (fsb->fsb_flags & FSB_INIT)
			return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "not responder");
		break;

	case FSB_DATAWRITE:
		if (!(fsb->fsb_flags & FSB_INIT))
			return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "not initiator");
		break;

	default:
		return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "wrong state");
	}

	for (pep = fadus, i = nfadu - 1; i >= 0; pep++, i--) {
		if ((pe = *pep) == NULLPE)
			return ftamlose(fti, FS_GEN(fsb), 0, NULLCP,
					"empty FADU at slot %d", nfadu - i - 1);

		if (pe->pe_context == PE_DFLT_CTX)
			pe->pe_context = fsb->fsb_id;

		if (pe->pe_context == fsb->fsb_id)
			switch (PE_ID(pe->pe_class, pe->pe_id)) {
			case PE_ID(PE_CLASS_APPL, FADU_NODESCR):
			case PE_ID(PE_CLASS_APPL, FADU_ENTERTREE):
			case PE_ID(PE_CLASS_APPL, FADU_EXITREE):
				break;

			default:
				return ftamlose(fti, FS_GEN(fsb), 0, NULLCP,
						"bad context at slot %d", nfadu - i - 1);
			}
	}

	if (PDataRequest(fsb->fsb_fd, fadus, nfadu, pi) == NOTOK) {
		(void) ps2ftamlose(fsb, fti, "PDataRequest", pa);
		if (PC_FATAL(pa->pa_reason))
			freefsblk(fsb);

		return NOTOK;
	}

	return OK;
}

/*    F-DATA-END.REQUEST */

int
FDataEndRequest(sd, action, diag, ndiag, fti)
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
	missingP(fti);

	smask = sigioblock();

	ftamPsig(fsb, sd);

	result = FDataEndRequestAux(fsb, action, diag, ndiag, fti);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
FDataEndRequestAux(fsb, action, diag, ndiag, fti)
	register struct ftamblk *fsb;
	int action;
	struct FTAMdiagnostic diag[];
	int ndiag;
	struct FTAMindication *fti;
{
	int result;
	PE pe;
	struct PSAPindication pis;
	struct PSAPindication *pi = &pis;
	struct PSAPabort *pa = &pi->pi_abort;
	register struct type_FTAM_PDU *pdu;
	register struct type_FTAM_F__DATA__END__request *req;

	switch (fsb->fsb_state) {
	case FSB_DATAREAD:
		if (fsb->fsb_flags & FSB_INIT)
			return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "not responder");
		break;

	case FSB_DATAWRITE:
		if (!(fsb->fsb_flags & FSB_INIT))
			return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "not initiator");
		break;

	default:
		return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "wrong state");
	}

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
	pdu->offset = type_FTAM_PDU_f__data__end__request;
	if ((req = (struct type_FTAM_F__DATA__END__request *)
	     calloc(1, sizeof *req)) == NULL)
		goto no_mem;
	pdu->un.f__data__end__request = req;
	if ((req->action__result = (struct type_FTAM_Action__Result *)
	     calloc(1, sizeof *req->action__result)) == NULL)
		goto no_mem;
	req->action__result->parm = action;
	if (ndiag > 0 && (req->diagnostic = diag2fpm(fsb, 0, diag, ndiag, fti))
	    == NULL)
		goto out;

	if (encode_FTAM_PDU(&pe, 1, 0, NULLCP, pdu) == NOTOK) {
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "error encoding PDU: %s", PY_pepy);
		goto out;
	}

	pe->pe_context = fsb->fsb_id;

	fsbtrace(fsb, (fsb->fsb_fd, "P-DATA.REQUEST", "F-DATA-END-request", pe, 0));

	result = PDataRequest(fsb->fsb_fd, &pe, 1, pi);

	pe_free(pe);
	pe = NULLPE;
	free_FTAM_PDU(pdu);
	pdu = NULL;

	if (result == NOTOK) {
		(void) ps2ftamlose(fsb, fti, "PDataRequest", pa);
		goto out;
	}

	fsb->fsb_state = FSB_DATAFIN1;

	return OK;
}

/*    F-CANCEL.REQUEST */

int
FCancelRequest(sd, action, sharedASE, diag, ndiag, fti)
	int sd;
	int action;
	PE sharedASE;
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
	missingP(fti);

	smask = sigioblock();

	ftamPsig(fsb, sd);

	result = FCancelRequestAux(fsb, action, sharedASE, diag, ndiag, fti);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
FCancelRequestAux(fsb, action, sharedASE, diag, ndiag, fti)
	register struct ftamblk *fsb;
	int action;
	PE sharedASE;
	struct FTAMdiagnostic diag[];
	int ndiag;
	struct FTAMindication *fti;
{
	int result, settings;
	char *prequest;
	PE pe;
	struct PSAPindication pis;
	struct PSAPindication *pi = &pis;
	struct PSAPabort *pa = &pi->pi_abort;
	register struct type_FTAM_PDU *pdu;
	register struct type_FTAM_F__CANCEL__request *req;

	switch (fsb->fsb_state) {
	case FSB_DATAREAD:
	case FSB_DATAWRITE:
		break;

	default:
		return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "wrong state");
	}

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
	pdu->offset = type_FTAM_PDU_f__cancel__request;
	if ((req = (struct type_FTAM_F__CANCEL__request *)
	     calloc(1, sizeof *req)) == NULL)
		goto no_mem;
	pdu->un.f__cancel__request = req;
	if ((req->action__result = (struct type_FTAM_Action__Result *)
	     calloc(1, sizeof *req->action__result)) == NULL)
		goto no_mem;
	req->action__result->parm = action;
	if (sharedASE && (req->shared__ASE__information = shared2fpm(fsb, sharedASE, fti)) == NULL)
		goto out;
	if (ndiag > 0 && (req->diagnostic = diag2fpm(fsb, 0, diag, ndiag, fti))
	    == NULL)
		goto out;

	if (encode_FTAM_PDU(&pe, 1, 0, NULLCP, pdu) == NOTOK) {
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "error encoding PDU: %s", PY_pepy);
		goto out;
	}

	pe->pe_context = fsb->fsb_id;

	if (fsb->fsb_srequirements & SR_RESYNC) {
		fsbtrace(fsb, (fsb->fsb_fd, "P-RESYNCHRONIZE.REQUEST", "F-CANCEL-request", pe, 0));

		settings = 0;	/* XXX: more later! */

		result = PReSyncRequest(fsb->fsb_fd, SYNC_ABANDON, SERIAL_NONE,
					settings, &pe, 1, pi);

		prequest = "PReSyncRequest";
	} else {
		fsbtrace(fsb, (fsb->fsb_fd, "P-DATA.REQUEST", "F-CANCEL-request", pe, 0));

		result = PDataRequest(fsb->fsb_fd, &pe, 1, pi);

		prequest = "PDataRequest";
	}

	pe_free(pe);
	pe = NULLPE;
	free_FTAM_PDU(pdu);
	pdu = NULL;

	if (result == NOTOK) {
		(void) ps2ftamlose(fsb, fti, prequest, pa);
		goto out;
	}

	fsb->fsb_flags |= FSB_CANCEL;
	fsb->fsb_state = FSB_DATACANCEL;
	fsb->fsb_cancelaction = action;
	if (fsb->fsb_cancelshared = sharedASE)
		fsb->fsb_cancelshared->pe_refcnt++;
	fsb->fsb_canceldiags = diag;
	fsb->fsb_cancelndiag = ndiag;

	return FWaitRequestAux(fsb, NOTOK, fti);
}

/*    F-CANCEL.RESPONSE */

int
FCancelResponse(sd, action, sharedASE, diag, ndiag, fti)
	int sd;
	int action;
	PE sharedASE;
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
	missingP(fti);

	smask = sigioblock();

	ftamPsig(fsb, sd);

	result = FCancelResponseAux(fsb, action, sharedASE, diag, ndiag, fti);

	(void) sigiomask(smask);

	return result;
}

/*  */

int
FCancelResponseAux(fsb, action, sharedASE, diag, ndiag, fti)
	register struct ftamblk *fsb;
	int action;
	PE sharedASE;
	struct FTAMdiagnostic diag[];
	int ndiag;
	struct FTAMindication *fti;
{
	int result;
	char *prequest;
	PE pe;
	struct PSAPindication pis;
	struct PSAPindication *pi = &pis;
	struct PSAPabort *pa = &pi->pi_abort;
	register struct type_FTAM_PDU *pdu;
	register struct type_FTAM_F__CANCEL__response *rsp;

	if (fsb->fsb_state != FSB_DATACANCEL)
		return ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "wrong state");

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
	pdu->offset = type_FTAM_PDU_f__cancel__response;
	if ((rsp = (struct type_FTAM_F__CANCEL__response *)
	     calloc(1, sizeof *rsp)) == NULL)
		goto no_mem;
	pdu->un.f__cancel__response = rsp;
	if ((rsp->action__result = (struct type_FTAM_Action__Result *)
	     calloc(1, sizeof *rsp->action__result)) == NULL)
		goto no_mem;
	rsp->action__result->parm = action;
	if (sharedASE && (rsp->shared__ASE__information = shared2fpm(fsb, sharedASE, fti)) == NULL)
		goto out;
	if (ndiag > 0 && (rsp->diagnostic = diag2fpm(fsb, 0, diag, ndiag, fti))
	    == NULL)
		goto out;

	if (encode_FTAM_PDU(&pe, 1, 0, NULLCP, pdu) == NOTOK) {
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "error encoding PDU: %s", PY_pepy);
		goto out;
	}

	pe->pe_context = fsb->fsb_id;

	if (fsb->fsb_srequirements & SR_RESYNC) {
		fsbtrace(fsb, (fsb->fsb_fd, "P-RESYNCHRONIZE.RESPONSE",
			       "F-CANCEL-response", pe, 0));

		result = PReSyncResponse(fsb->fsb_fd, SERIAL_NONE, fsb->fsb_settings, &pe, 1, pi);

		prequest = "PReSyncResponse";
	} else {
		fsbtrace(fsb, (fsb->fsb_fd, "P-DATA.REQUEST", "F-CANCEL-response", pe, 0));

		result = PDataRequest(fsb->fsb_fd, &pe, 1, pi);

		prequest = "PDataRequest";
	}

	pe_free(pe);
	pe = NULLPE;
	free_FTAM_PDU(pdu);
	pdu = NULL;

	if (result == NOTOK) {
		(void) ps2ftamlose(fsb, fti, prequest, pa);
		goto out;
	}

	fsb->fsb_state = FSB_DATAIDLE;

	return OK;
}