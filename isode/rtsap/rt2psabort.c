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

/* rt2psabort.c - RTPM: user abort */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rtsap/RCS/rt2psabort.c,v 9.0 1992/06/16 12:37:45 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rtsap/RCS/rt2psabort.c,v 9.0 1992/06/16 12:37:45 isode Rel
 *
 *
 * Log: rt2psabort.c,v
 * Revision 9.0  1992/06/16  12:37:45  isode
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
#include "RTS-types.h"
#include "rtpkt.h"
#include "tailor.h"
#include "logger.h"

static int RtUAbortRequestAux();

/*    RT-U-ABORT.REQUEST */

int
RtUAbortRequest(sd, data, rti)
	int sd;
	PE data;
	struct RtSAPindication *rti;
{
	SBV smask;
	int result;
	register struct assocblk *acb;

	missingP(rti);

	smask = sigioblock();

	rtsapPsig(acb, sd);

	result = RtUAbortRequestAux(acb, data, rti);

	(void) sigiomask(smask);

	return result;
}

/*  */

static int
RtUAbortRequestAux(acb, data, rti)
	register struct assocblk *acb;
	PE data;
	register struct RtSAPindication *rti;
{
	int result;
	PE pe, p;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &aci->aci_abort;

	if (!(acb->acb_flags & ACB_ACS))
		return rtsaplose(rti, RTS_OPERATION, NULLCP,
				 "not an association descriptor for RTS");
/* begin RTAB APDU */
	if ((pe = pe_alloc(PE_CLASS_CONT, PE_FORM_CONS, 22)) == NULLPE
	    || set_add(pe, num2prim((integer) ABORT_USER, PE_CLASS_CONT,
				    RTAB_REASON)) == NOTOK
	    || (data
		&& (set_add(pe, p = pe_alloc(PE_CLASS_CONT, PE_FORM_CONS,
					     RTAB_USERDATA)) == NOTOK
		    || set_add(p, data) == NOTOK))) {
		result = rtsaplose(rti, RTS_CONGEST, NULLCP, "out of memory");
		(void) AcUAbortRequest(acb->acb_fd, NULLPEP, 0, aci);
		goto out;
	}
	pe->pe_context = acb->acb_rtsid;
/* end RTAB APDU */

	PLOGP(rtsap_log, RTS_RTSE__apdus, pe, "RTABapdu", 0);

	if ((result = AcUAbortRequest(acb->acb_fd, &pe, 1, aci)) == NOTOK)
		(void) acs2rtslose(acb, rti, "AcUAbortRequest", aca);
	else
		result = OK;

      out:;
	if (pe) {
		if (data)
			(void) pe_extract(pe, data);
		pe_free(pe);
	}

	return result;
}
