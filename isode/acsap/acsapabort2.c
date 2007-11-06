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

/* acsapabort2.c - ACPM: interpret abort */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsapabort2.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsapabort2.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsapabort2.c,v
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
#ifdef	DEBUG
#include "tailor.h"
#endif

/*    handle P-{U,P}-ABORT.INDICATION */

int
AcABORTser(sd, pa, aci)
	int sd;
	register struct PSAPabort *pa;
	register struct AcSAPindication *aci;
{
	SBV smask;
	int result;
	register struct assocblk *acb;

	missingP(pa);
	missingP(aci);

	smask = sigioblock();

	if ((acb = findacblk(sd)) == NULL) {
		(void) sigiomask(smask);
		return acsaplose(aci, ACS_PARAMETER, NULLCP, "invalid association descriptor");
	}

	result = ps2acsabort(acb, pa, aci);

	(void) sigiomask(smask);

	return result;
}

/*  */

int
ps2acsabort(acb, pa, aci)
	register struct assocblk *acb;
	register struct PSAPabort *pa;
	register struct AcSAPindication *aci;
{
	int result;
	PE pe;
	register struct AcSAPabort *aca = &aci->aci_abort;
	struct type_ACS_ACSE__apdu *pdu;
	register struct type_ACS_ABRT__apdu *abrt;

	result = OK;
	pdu = NULL;

	if (!pa->pa_peer) {
		if (PC_FATAL(pa->pa_reason))
			acb->acb_fd = NOTOK;

		(void) ps2acslose(acb, aci, NULLCP, pa);
		goto out;
	}

	if (pa->pa_ninfo == 0) {
		(void) acsaplose(aci, ACS_ABORTED, NULLCP, NULLCP);
		if (acb->acb_sversion == 1)
			aca->aca_source = ACA_PROVIDER;
		goto out;
	}

	bzero((char *) aci, sizeof *aci);
	aci->aci_type = ACI_ABORT;

	if (acb->acb_sversion == 1) {
		register int i;

		aca->aca_reason = ACS_ABORTED;
		aca->aca_source = ACA_USER;
		if ((i = pa->pa_ninfo) > NACDATA)
			i = NACDATA;
		aca->aca_ninfo = i;
		while (i-- > 0) {
			aca->aca_info[i] = pa->pa_info[i];
			pa->pa_info[i] = NULLPE;
		}
		goto out;
	}

	result = decode_ACS_ACSE__apdu(pe = pa->pa_info[0], 1, NULLIP, NULLVP, &pdu);

#ifdef	DEBUG
	if (result == OK && (acsap_log->ll_events & LLOG_PDUS))
		pvpdu(acsap_log, print_ACS_ACSE__apdu_P, pe, "ACSE-apdu", 1);
#endif

	pe_free(pe);
	pe = pa->pa_info[0] = NULLPE;

	if (result == NOTOK) {
		(void) acsaplose(aci, ACS_PROTOCOL, NULLCP, "%s", PY_pepy);
		goto out;
	}

	if (pdu->offset != type_ACS_ACSE__apdu_abrt) {
		result = acsaplose(aci, ACS_PROTOCOL, NULLCP,
				   "unexpected PDU %d on P-U-ABORT", pdu->offset);
		goto out;
	}

	abrt = pdu->un.abrt;
	aca->aca_reason = ACS_ABORTED;
	aca->aca_source = abrt->abort__source;
	(void) apdu2info(acb, aci, abrt->user__information, aca->aca_info, &aca->aca_ninfo);

      out:;
	acb->acb_fd = NOTOK;
	PAFREE(pa);
	if (pdu)
		free_ACS_ACSE__apdu(pdu);

	return result;
}
