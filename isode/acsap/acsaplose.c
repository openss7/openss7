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

/* acsaplose.c - ACPM: you lose */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsaplose.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsaplose.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsaplose.c,v
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
#include <varargs.h>
#include "ACS-types.h"
#define	ACSE
#include "acpkt.h"
#include "tailor.h"

/*  */

#ifndef	lint
static int _acsaplose();
#endif

#ifndef	lint
int
acpktlose(va_alist)
	va_dcl
{
	int reason, result;
	PE pe;
	register struct assocblk *acb;
	register struct AcSAPindication *aci;
	struct PSAPindication pis;
	struct type_ACS_ABRT__apdu pdus;
	register struct type_ACS_ABRT__apdu *pdu = &pdus;
	va_list ap;

	va_start(ap);

	acb = va_arg(ap, struct assocblk *);
	aci = va_arg(ap, struct AcSAPindication *);
	reason = va_arg(ap, int);

	result = _acsaplose(aci, reason, ap);

	va_end(ap);

	if (acb == NULLACB || acb->acb_fd == NOTOK)
		return result;

	if (acb->acb_sversion == 1) {
		if (PUAbortRequest(acb->acb_fd, NULLPEP, 0, &pis) != NOTOK)
			acb->acb_fd = NOTOK;

		return result;
	}

	pdu->abort__source = int_ACS_abort__source_acse__service__provider;
	pdu->user__information = NULL;

	pe = NULLPE;
	if (encode_ACS_ABRT__apdu(&pe, 1, 0, NULLCP, pdu) != NOTOK) {
		pe->pe_context = acb->acb_id;

		PLOGP(acsap_log, ACS_ACSE__apdu, pe, "ABRT-apdu", 0);

		if (PUAbortRequest(acb->acb_fd, &pe, 1, &pis) != NOTOK)
			acb->acb_fd = NOTOK;
	}
	if (pe)
		pe_free(pe);

	return result;
}
#else
/* VARARGS5 */

int
acpktlose(acb, aci, reason, what, fmt)
	struct assocblk *acb;
	struct AcSAPindication *aci;
	int reason;
	char *what, *fmt;
{
	return acpktlose(acb, aci, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
int
acsaplose(va_alist)
	va_dcl
{
	int reason, result;
	struct AcSAPindication *aci;
	va_list ap;

	va_start(ap);

	aci = va_arg(ap, struct AcSAPindication *);
	reason = va_arg(ap, int);

	result = _acsaplose(aci, reason, ap);

	va_end(ap);

	return result;
}
#else
/* VARARGS4 */

int
acsaplose(aci, reason, what, fmt)
	struct AcSAPindication *aci;
	int reason;
	char *what, *fmt;
{
	return acsaplose(aci, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_acsaplose(aci, reason, ap)		/* what, fmt, args ... */
	register struct AcSAPindication *aci;
	int reason;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];
	register struct AcSAPabort *aca;

	if (aci) {
		bzero((char *) aci, sizeof *aci);
		aci->aci_type = ACI_ABORT;
		aca = &aci->aci_abort;

		asprintf(bp = buffer, ap);
		bp += strlen(bp);

		aca->aca_source = ACA_LOCAL;
		aca->aca_reason = reason;
		copyAcSAPdata(buffer, bp - buffer, aca);
	}

	return NOTOK;
}
#endif
