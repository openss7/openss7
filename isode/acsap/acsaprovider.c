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

/* acsaprovider.c - implement the association control protocol */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/acsap/RCS/acsaprovider.c,v 9.0 1992/06/16 12:05:59 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/acsap/RCS/acsaprovider.c,v 9.0 1992/06/16 12:05:59 isode Rel
 *
 *
 * Log: acsaprovider.c,v
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

/*    PSAP interface */

int
ps2acslose(acb, aci, event, pa)
	register struct assocblk *acb;
	register struct AcSAPindication *aci;
	char *event;
	register struct PSAPabort *pa;
{
	int reason;
	char *cp, buffer[BUFSIZ];

	if (event)
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
		     (pa->pa_cc > 0 ? "%s: %s [%*.*s]" : "%s: %s", event,
		      PErrString(pa->pa_reason), pa->pa_cc, pa->pa_cc, pa->pa_data));

	cp = "";
	switch (pa->pa_reason) {
	case PC_ADDRESS:
		reason = ACS_ADDRESS;
		break;

	case PC_REFUSED:
		reason = ACS_REFUSED;
		break;

	case PC_CONGEST:
		reason = ACS_CONGEST;
		break;

	case PC_PARAMETER:
		reason = ACS_PARAMETER;
		break;

	case PC_OPERATION:
		reason = ACS_OPERATION;
		break;

	case PC_TIMER:
		reason = ACS_TIMER;
		break;

	default:
		(void) sprintf(cp = buffer, " (%s at presentation)", PErrString(pa->pa_reason));
	case PC_SESSION:
		reason = ACS_PRESENTATION;
		break;
	}

	if (ACS_FATAL(reason)) {
		if (pa->pa_cc > 0)
			return acpktlose(acb, aci, reason, NULLCP, "%*.*s%s",
					 pa->pa_cc, pa->pa_cc, pa->pa_data, cp);
		else
			return acpktlose(acb, aci, reason, NULLCP, "%s", *cp ? cp + 1 : cp);
	} else {
		if (pa->pa_cc > 0)
			return acsaplose(aci, reason, NULLCP, "%*.*s%s",
					 pa->pa_cc, pa->pa_cc, pa->pa_data, cp);
		else
			return acsaplose(aci, reason, NULLCP, "%s", *cp ? cp + 1 : cp);
	}
}

/*    INTERNAL */

/* 
   Owing to laziness on our part, we use only ASN.1 transfer syntax.
 */

/* ARGSUSED */

struct type_ACS_Association__information *
info2apdu(acb, aci, data, ndata)
	struct assocblk *acb;
	struct AcSAPindication *aci;
	PE *data;
	int ndata;
{
	register PE pe;
	struct type_ACS_Association__information *info;
	register struct type_ACS_Association__information **pp, *p;
	register struct type_UNIV_EXTERNAL *q;

	for (pp = &info; ndata-- > 0; pp = &p->next) {
		if ((*pp = p = (struct type_ACS_Association__information *)
		     calloc(1, sizeof *p)) == NULL || (p->EXTERNAL = (struct type_UNIV_EXTERNAL *)
						       calloc(1, sizeof *q)) == NULL
		    || (p->EXTERNAL->encoding = (struct choice_UNIV_0 *)
			malloc(sizeof(struct choice_UNIV_0))) == NULL)
			goto out;
		q = p->EXTERNAL;

		if (!(acb->acb_flags & ACB_CONN)
		    && (q->direct__reference = oid_cpy(BER_OID))
		    == NULLOID)
			goto out;
		q->indirect__reference = (pe = *data++)->pe_context;
		q->encoding->offset = choice_UNIV_0_single__ASN1__type;
		(q->encoding->un.single__ASN1__type = pe)->pe_refcnt++;
	}
	(*pp) = NULL;

	return info;

      out:;
	free_ACS_Association__information(info);

	(void) acsaplose(aci, ACS_CONGEST, NULLCP, "out of memory");

	return NULL;
}

/*  */

/* ARGSUSED */

int
apdu2info(acb, aci, info, data, ndata)
	struct assocblk *acb;
	struct AcSAPindication *aci;
	struct type_ACS_Association__information *info;
	PE *data;
	int *ndata;
{
	register int i;
	register PE pe;
	register struct type_UNIV_EXTERNAL *q;

	for (i = 0; info; info = info->next, i++) {
		if (i > NACDATA)
			return acpktlose(acb, aci, ACS_CONGEST, NULLCP,
					 "too much user information");

		q = info->EXTERNAL;
		if (q->encoding->offset != choice_UNIV_0_single__ASN1__type)
			return acpktlose(acb, aci, ACS_PROTOCOL, NULLCP,
					 "EXTERNAL data not single-ASN1-type");

		(pe = q->encoding->un.single__ASN1__type)->pe_refcnt++;
		pe->pe_context = q->indirect__reference;

		*data++ = pe;
	}
	*ndata = i;

	return OK;
}
