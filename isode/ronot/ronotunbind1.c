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

/* ronotunbind1.c - RONOT: Maps ABSTRACT-UNBIND onto A-RELEASE.REQUEST */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ronot/RCS/ronotunbind1.c,v 9.0 1992/06/16 12:36:36 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ronot/RCS/ronotunbind1.c,v 9.0 1992/06/16 12:36:36 isode Rel
 *
 *
 * Log: ronotunbind1.c,v
 * Revision 9.0  1992/06/16  12:36:36  isode
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

#include "tailor.h"
#include "logger.h"
#include "ronot.h"
#include "RONOT-types.h"

/*    RO-UNBIND.REQUEST */

/* ARGSUSED */

int
RoUnBindRequest(sd, unbindargpe, secs, acr, rni)
	int sd;
	PE unbindargpe;
	int secs;
	struct AcSAPrelease *acr;
	struct RoNOTindication *rni;
{
	int result;
	PE user_data;
	PE *user_data_p = &(user_data);
	int ndata;
	struct AcSAPindication aci_s;
	struct AcSAPindication *aci = &aci_s;
	struct AcSAPabort *aca = &(aci->aci_abort);

	if (unbindargpe != NULLPE) {
		if (encode_RONOT_UnBindArgumentValue(user_data_p, 1, 0, NULLCP, unbindargpe) ==
		    NOTOK) {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("RoUnbindRequest: encode_RONOT_UnBindArgumentValue failed"));
			return (ronotlose(rni, RBI_ENC_UNBIND_ARG, NULLCP, NULLCP));
		}
		(*user_data_p)->pe_context = unbindargpe->pe_context;
		ndata = 1;
	} else {
		(*user_data_p) = NULLPE;
		ndata = 0;
	}

	result = AcRelRequest(sd, ACF_NORMAL, user_data_p, ndata, secs, acr, aci);

	if ((*user_data_p) != NULLPE) {
		pe_free((*user_data_p));
	}

	if (result == NOTOK) {
		if (aci->aci_abort.aca_reason == ACS_TIMER) {
			/* ADT: Watch this !! */
			/* more work needed !!! */
			ACAFREE(aca);
			return (DONE);
		} else {
			LLOG(rosap_log, LLOG_EXCEPTIONS, ("RoUnbindRequest: AcRelRequest failed"));
			(void) acs2ronotlose(rni, "RO-UNBIND.REQUEST", aca);
			ACAFREE(aca);
			return (NOTOK);
		}
	} else {
		if (ParseRoUnBindResponse(acr, rni) != OK) {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("RoUnbindRequest: ParseRoUnBindResponse failed"));
			ACRFREE(acr);
			return (NOTOK);
		}
	}

	return (result);
}

/*    RO-UNBIND.RETRY */

/* ARGSUSED */

int
RoUnBindRetry(sd, secs, acr, rni)
	int sd;
	int secs;
	struct AcSAPrelease *acr;
	struct RoNOTindication *rni;
{
	int result;
	struct AcSAPindication aci_s;
	struct AcSAPindication *aci = &(aci_s);
	struct AcSAPabort *aca = &(aci->aci_abort);

	result = AcRelRetryRequest(sd, secs, acr, aci);

	if (result == NOTOK) {
		if (aci->aci_abort.aca_reason == ACS_TIMER) {
			/* ADT: Watch out for this */
			/* more work needed !!! */
			ACAFREE(aca);
			return (DONE);
		} else {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("RoUnbindRetry: AcRelRetryRequest failed"));
			(void) acs2ronotlose(rni, "RO-UNBIND.RETRY", aca);
			ACAFREE(aca);
			return (NOTOK);
		}
	} else {
		if (ParseRoUnBindResponse(acr, rni) != OK) {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("RoUnbindRetry: ParseRoUnBindResponse failed"));
			ACRFREE(acr);
			return (NOTOK);
		}
	}

	return (result);
}

int
ParseRoUnBindResponse(acr, rni)
	struct AcSAPrelease *acr;
	struct RoNOTindication *rni;
{
	PE pe;

	if (acr->acr_ninfo == 0)
		return (OK);

	if (acr->acr_ninfo != 1)
		return (ronotlose(rni, RBI_DEC_NINFO, NULLCP, NULLCP));

	if (acr->acr_info[0] == NULLPE)
		return (ronotlose(rni, RBI_DEC_NINFO, NULLCP, NULLCP));

	/* ADT: Can we get rid of this copy? */
	pe = acr->acr_info[0];
	acr->acr_info[0] = NULLPE;
	if (acr->acr_affirmative == ACS_ACCEPT) {
		if (decode_RONOT_UnBindResultValue(pe, 1, NULLIP, NULLVP, &acr->acr_info[0]) != OK) {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("ParseRoUnBindResponse: decode_RONOT_UnBindResultValue failed"));
			acr->acr_ninfo = 0;
			pe_free(pe);
			return (ronotlose(rni, RBI_DEC_UNBIND_RES, NULLCP, NULLCP));
		}
	} else {
		if (decode_RONOT_UnBindErrorValue(pe, 1, NULLIP, NULLVP, &acr->acr_info[0]) != OK) {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("ParseRoUnBindResponse: decode_RONOT_UnBindErrorValue failed"));
			acr->acr_ninfo = 0;
			pe_free(pe);
			return (ronotlose(rni, RBI_DEC_UNBIND_RES, NULLCP, NULLCP));
		}
	}
	pe_free(pe);

	return (OK);
}
