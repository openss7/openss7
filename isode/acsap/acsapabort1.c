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

/* acsapabort1.c - ACPM: user abort */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsapabort1.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsapabort1.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsapabort1.c,v
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

/*    A-ABORT.REQUEST */

int
AcUAbortRequest(sd, data, ndata, aci)
	int sd;
	PE *data;
	int ndata;
	struct AcSAPindication *aci;
{
	SBV smask;
	int result;
	register struct assocblk *acb;
	PE pe;
	struct PSAPindication pis;
	register struct PSAPindication *pi = &pis;
	register struct PSAPabort *pa = &pi->pi_abort;
	register struct type_ACS_ABRT__apdu *pdu;

	toomuchP(data, ndata, NACDATA, "release");
	missingP(aci);

	smask = sigioblock();

	if ((acb = findacblk(sd)) == NULL) {
		(void) sigiomask(smask);
		return acsaplose(aci, ACS_PARAMETER, NULLCP, "invalid association descriptor");
	}

	pdu = NULL;
	pe = NULLPE;
	if (acb->acb_sversion == 1) {
		if ((result = PUAbortRequest(acb->acb_fd, data, ndata, pi))
		    == NOTOK) {
			(void) ps2acslose(acb, aci, "PUAbortRequest", pa);
			if (PC_FATAL(pa->pa_reason))
				goto out2;
			else
				goto out1;
		}

		result = OK;
		acb->acb_fd = NOTOK;
		goto out2;
	}

	if ((pdu = (struct type_ACS_ABRT__apdu *) calloc(1, sizeof *pdu))
	    == NULL) {
		result = acsaplose(aci, ACS_CONGEST, NULLCP, "out of memory");
		goto out2;
	}
	pdu->abort__source = int_ACS_abort__source_acse__service__user;
	if (data && ndata > 0 && (pdu->user__information = info2apdu(acb, aci, data, ndata))
	    == NULL)
		goto out2;

	result = encode_ACS_ABRT__apdu(&pe, 1, 0, NULLCP, pdu);

	free_ACS_ABRT__apdu(pdu);
	pdu = NULL;

	if (result == NOTOK) {
		(void) acsaplose(aci, ACS_CONGEST, NULLCP, "error encoding PDU: %s", PY_pepy);
		goto out2;
	}
	pe->pe_context = acb->acb_id;

	PLOGP(acsap_log, ACS_ACSE__apdu, pe, "ABRT-apdu", 0);

	if ((result = PUAbortRequest(acb->acb_fd, &pe, 1, pi)) == NOTOK) {
		(void) ps2acslose(acb, aci, "PUAbortRequest", pa);
		if (PC_FATAL(pa->pa_reason))
			goto out2;
		else
			goto out1;
	}

	result = OK;
	acb->acb_fd = NOTOK;

      out2:;
	freeacblk(acb);

      out1:;
	if (pe)
		pe_free(pe);
	if (pdu)
		free_ACS_ABRT__apdu(pdu);

	(void) sigiomask(smask);

	return result;
}
