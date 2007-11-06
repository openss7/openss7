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

/* acsapreleas1.c - ACPM: initiate release */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsapreleas1.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsapreleas1.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsapreleas1.c,v
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
#include <signal.h>
#include "ACS-types.h"
#define	ACSE
#include "acpkt.h"
#include "tailor.h"

static int AcRelRetryRequestAux();

/*    A-RELEASE.REQUEST */

int
AcRelRequest(sd, reason, data, ndata, secs, acr, aci)
	int sd;
	int reason;
	PE *data;
	int ndata;
	int secs;
	struct AcSAPrelease *acr;
	struct AcSAPindication *aci;
{
	SBV smask;
	int result;
	register struct assocblk *acb;
	register struct type_ACS_RLRQ__apdu *rlrq;

	switch (reason) {
	case ACF_NORMAL:
	case ACF_URGENT:
	case ACF_USERDEFINED:
		break;

	default:
		return acsaplose(aci, ACS_PARAMETER, NULLCP, "invalid value for reason parameter");
	}
	toomuchP(data, ndata, NACDATA, "release");
	if (data) {		/* XXX: probably should have a more intensive check... */
		register int i;
		register PE *pep;

		for (pep = data, i = ndata; i > 0; pep++, i--)
			if ((*pep)->pe_context == PE_DFLT_CTX)
				return acsaplose(aci, ACS_PARAMETER, NULLCP,
						 "default context not allowed for user-data at slot %d",
						 pep - data);
	}
	missingP(acr);
	missingP(aci);

	smask = sigioblock();

	acsapPsig(acb, sd);

	if ((rlrq = (struct type_ACS_RLRQ__apdu *) calloc(1, sizeof *rlrq))
	    == NULL) {
		(void) acsaplose(aci, ACS_CONGEST, NULLCP, "out of memory");
		goto out2;
	}
	rlrq->optionals |= opt_ACS_RLRQ__apdu_reason;
	rlrq->reason = reason;
	if (data && ndata > 0 && (rlrq->user__information = info2apdu(acb, aci, data, ndata))
	    == NULL)
		goto out2;

	result = encode_ACS_RLRQ__apdu(&acb->acb_retry, 1, 0, NULLCP, rlrq);

	free_ACS_RLRQ__apdu(rlrq);
	rlrq = NULL;

	if (result == NOTOK) {
		(void) acsaplose(aci, ACS_CONGEST, NULLCP, "error encoding PDU: %s", PY_pepy);
		goto out2;
	}
	acb->acb_retry->pe_context = acb->acb_id;

	PLOGP(acsap_log, ACS_ACSE__apdu, acb->acb_retry, "RLRQ-apdu", 0);

	result = AcRelRetryRequestAux(acb, secs, acr, aci);
	goto out1;

      out2:;
	if (acb->acb_retry) {
		pe_free(acb->acb_retry);
		acb->acb_retry = NULLPE;
	}
	freeacblk(acb);
	if (rlrq)
		free_ACS_RLRQ__apdu(rlrq);
	result = NOTOK;

      out1:;
	(void) sigiomask(smask);

	return result;
}

/*    A-RELEASE-RETRY.REQUEST (pseudo) */

int
AcRelRetryRequest(sd, secs, acr, aci)
	int sd;
	int secs;
	struct AcSAPrelease *acr;
	struct AcSAPindication *aci;
{
	SBV smask;
	int result;
	register struct assocblk *acb;

	missingP(acr);
	missingP(aci);

	smask = sigioblock();

	if ((acb = findacblk(sd)) == NULL)
		result = acsaplose(aci, ACS_PARAMETER, NULLCP, "invalid association descriptor");
	else if (!(acb->acb_flags & ACB_RELEASE))
		result = acsaplose(aci, ACS_OPERATION, "release not in progress");
	else
		result = AcRelRetryRequestAux(acb, secs, acr, aci);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
AcRelRetryRequestAux(acb, secs, acr, aci)
	register struct assocblk *acb;
	int secs;
	struct AcSAPrelease *acr;
	struct AcSAPindication *aci;
{
	int result;
	char *id = acb->acb_flags & ACB_RELEASE ? "PRelRetryRequest" : "PRelRequest";
	PE pe;
	struct PSAPrelease prs;
	register struct PSAPrelease *pr = &prs;
	struct PSAPindication pis;
	register struct PSAPabort *pa = &pis.pi_abort;
	struct type_ACS_ACSE__apdu *pdu = NULL;
	register struct type_ACS_RLRE__apdu *rlre;

	bzero((char *) pr, sizeof *pr);

	if ((result = (acb->acb_flags & ACB_RELEASE)
	     ? PRelRetryRequest(acb->acb_fd, secs, pr, &pis)
	     : PRelRequest(acb->acb_fd, &acb->acb_retry, 1, secs, pr, &pis)) == NOTOK) {
		if (pa->pa_reason == PC_TIMER) {
			acb->acb_flags |= ACB_RELEASE;

			return ps2acslose(NULLACB, aci, id, pa);
		}

		if (pa->pa_peer) {
			(void) AcABORTser(acb->acb_fd, pa, aci);
			goto out1;
		}
		if (PC_FATAL(pa->pa_reason)) {
			(void) ps2acslose(acb, aci, id, pa);
			goto out2;
		} else {
			(void) ps2acslose(NULLACB, aci, id, pa);
			goto out1;
		}
	}

	bzero((char *) acr, sizeof *acr);

	if (pr->pr_ninfo == 0) {
		result = acsaplose(aci, ACS_PROTOCOL, NULLCP, "no user-data on P-RELEASE");
		goto out3;
	}

	result = decode_ACS_ACSE__apdu(pe = pr->pr_info[0], 1, NULLIP, NULLVP, &pdu);

#ifdef	DEBUG
	if (result == OK && (acsap_log->ll_events & LLOG_PDUS))
		pvpdu(acsap_log, print_ACS_ACSE__apdu_P, pe, "ACSE-apdu", 1);
#endif

	pe_free(pe);
	pe = pr->pr_info[0] = NULLPE;

	if (result == NOTOK) {
		(void) acpktlose(acb, aci, ACS_PROTOCOL, NULLCP, "%s", PY_pepy);
		goto out3;
	}

	if (pdu->offset != type_ACS_ACSE__apdu_rlre) {
		(void) acpktlose(acb, aci, ACS_PROTOCOL, NULLCP,
				 "unexpected PDU %d on P-RELEASE", pdu->offset);
		goto out3;
	}

	rlre = pdu->un.rlre;
	if (rlre->optionals & opt_ACS_RLRE__apdu_reason)
		acr->acr_reason = rlre->reason;
	else
		acr->acr_reason = int_ACS_reason_normal;
	if (apdu2info(acb, aci, rlre->user__information, acr->acr_info, &acr->acr_ninfo) == NOTOK)
		goto out3;

	if (acr->acr_affirmative = pr->pr_affirmative) {
		acb->acb_fd = NOTOK;
		result = OK;
	} else
		result = DONE;

      out3:;
	PRFREE(pr);

      out2:;
	if (result == DONE)
		result = OK;
	else
		freeacblk(acb), acb = NULLACB;
      out1:;
	if (acb && acb->acb_retry) {
		pe_free(acb->acb_retry);
		acb->acb_retry = NULLPE;
	}
	if (pdu)
		free_ACS_ACSE__apdu(pdu);

	return result;
}
