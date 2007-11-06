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

/* ftamrelease2.c - FPM: respond to release */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamrelease2.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamrelease2.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamrelease2.c,v
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

/*    F-TERMINATE.RESPONSE */

int
FTerminateResponse(sd, sharedASE, charging, fti)
	int sd;
	PE sharedASE;
	struct FTAMcharging *charging;
	struct FTAMindication *fti;
{
	SBV smask;
	int result;
	register struct ftamblk *fsb;

	if (charging && charging->fc_ncharge > NFCHRG)
		return ftamlose(fti, FS_GEN_NOREASON, 0, NULLCP, "too many charges");
	missingP(fti);

	smask = sigioblock();

	ftamFsig(fsb, sd);

	result = FTerminateResponseAux(fsb, sharedASE, charging, fti);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
FTerminateResponseAux(fsb, sharedASE, charging, fti)
	register struct ftamblk *fsb;
	PE sharedASE;
	struct FTAMcharging *charging;
	struct FTAMindication *fti;
{
	int result;
	PE pe;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;
	register struct type_FTAM_PDU *pdu;
	register struct type_FTAM_F__TERMINATE__response *rsp;

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
	pdu->offset = type_FTAM_PDU_f__terminate__response;
	if ((rsp = (struct type_FTAM_F__TERMINATE__response *)
	     calloc(1, sizeof *rsp)) == NULL)
		goto no_mem;
	pdu->un.f__terminate__response = rsp;
	if (sharedASE && (rsp->shared__ASE__information = shared2fpm(fsb, sharedASE, fti)) == NULL)
		goto out;
	if (charging
	    && charging->fc_ncharge > 0 && (rsp->charging = chrg2fpm(fsb, charging, fti)) == NULL)
		goto out;

	if (encode_FTAM_PDU(&pe, 1, 0, NULLCP, pdu) == NOTOK) {
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "error encoding PDU: %s", PY_pepy);
		goto out;
	}

	pe->pe_context = fsb->fsb_id;

	fsbtrace(fsb, (fsb->fsb_fd, "A-RELEASE.RESPONSE", "F-TERMINATE-response", pe, 0));

	result = AcRelResponse(fsb->fsb_fd, ACS_ACCEPT, ACR_NORMAL, &pe, 1, aci);

	pe_free(pe);
	pe = NULLPE;
	free_FTAM_PDU(pdu);
	pdu = NULL;

	if (result == NOTOK) {
		(void) acs2ftamlose(fsb, fti, "AcRelResponse", aca);
		goto out;
	}

	fsb->fsb_fd = NOTOK;
	freefsblk(fsb);

	return OK;
}
