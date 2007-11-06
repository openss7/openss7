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

/* ronotunbind2.c - RONOT: ABSTRACT-BIND mapping onto A-RELEASE.RESPONSE */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ronot/RCS/ronotunbind2.c,v 9.0 1992/06/16 12:36:36 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ronot/RCS/ronotunbind2.c,v 9.0 1992/06/16 12:36:36 isode Rel
 *
 *
 * Log: ronotunbind2.c,v
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

/* 	RO-UNBIND.INDICATION */

/* ARGSUSED */

int
RoUnBindInit(sd, acf, rni)
	int sd;
	struct AcSAPfinish *acf;
	struct RoNOTindication *rni;
{
	/* 
	 * What is provided here is in-line handling of the
	 * (usually NULL) user data as an unbind argument value.
	 *
	 * A plausible alternative approach is to add a new finish
	 * routine (in place of AcFINISHser) as an element in a
	 * new ros-mapping structure so that RoWaitRequest performed
	 * this automatically for RO-Notation users.
	 */

	PE pe;

	if (acf->acf_ninfo == 0)
		return (OK);

	if (acf->acf_ninfo != 1)
		return (ronotlose(rni, RBI_DEC_NINFO, NULLCP, NULLCP));

	if (acf->acf_info[0] == NULLPE)
		return (ronotlose(rni, RBI_DEC_NINFO, NULLCP, NULLCP));

	pe = acf->acf_info[0];
	acf->acf_info[0] = NULLPE;
	if (decode_RONOT_UnBindArgumentValue(pe, 1, NULLIP, NULLVP, &(acf->acf_info[0])) != OK) {
		LLOG(rosap_log, LLOG_EXCEPTIONS,
		     ("RO-UNBIND.INDICATION: decode_RONOT_UnBindArgumentValue failed"));
		acf->acf_ninfo = 0;
		pe_free(pe);
		return (ronotlose(rni, RBI_DEC_BIND_ARG, NULLCP, NULLCP));
	}
	pe_free(pe);

	return (OK);
}

/*    RO-UNBIND.RESULT */

/* ARGSUSED */

int
RoUnBindResult(sd, unbindrespe, rni)
	int sd;
	PE unbindrespe;
	struct RoNOTindication *rni;
{
	int result;
	PE user_data;
	PE *user_data_p = &(user_data);
	int ndata;
	struct AcSAPindication aci_s;
	struct AcSAPindication *aci = &(aci_s);
	struct AcSAPabort *aca = &(aci->aci_abort);

	if (unbindrespe != NULLPE) {
		if (encode_RONOT_UnBindResultValue(user_data_p, 1, 0, NULLCP, unbindrespe) == NOTOK) {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("RoUnBindResult: encode_RONOT_UnBindResultValue failed"));
			return (ronotlose(rni, RBI_ENC_UNBIND_RES, NULLCP, NULLCP));
		}
		(*user_data_p)->pe_context = unbindrespe->pe_context;
		ndata = 1;
	} else {
		(*user_data_p) = NULLPE;
		ndata = 0;
	}

	result = AcRelResponse(sd, ACS_ACCEPT, ACF_NORMAL, user_data_p, ndata, aci);

	if ((*user_data_p) != NULLPE) {
		pe_free((*user_data_p));
	}

	if (result == NOTOK) {
		LLOG(rosap_log, LLOG_EXCEPTIONS, ("RoUnBindResult: AcRelResponse failed"));
		/* Have an AcSAPindication, need to return RoNOTindication */
		(void) acs2ronotlose(rni, "RO-UNBIND.RESULT", aca);
		ACAFREE(aca);
		return (NOTOK);
	}

	return (result);
}

/*    RO-UNBIND.ERROR */

/* ARGSUSED */

int
RoUnBindError(sd, unbinderrpe, rni)
	int sd;
	PE unbinderrpe;
	struct RoNOTindication *rni;
{
	int result;
	PE user_data;
	PE *user_data_p = &(user_data);
	int ndata;
	struct AcSAPindication aci_s;
	struct AcSAPindication *aci = &(aci_s);
	struct AcSAPabort *aca = &(aci->aci_abort);

	if (unbinderrpe != NULLPE) {
		if (encode_RONOT_UnBindErrorValue(user_data_p, 1, 0, NULLCP, unbinderrpe) == NOTOK) {
			LLOG(rosap_log, LLOG_EXCEPTIONS,
			     ("RoUnBindError: encode_RONOT_UnBindErrorValue failed"));
			return (ronotlose(rni, RBI_ENC_UNBIND_ERR, NULLCP, NULLCP));
		}
		(*user_data_p)->pe_context = unbinderrpe->pe_context;
		ndata = 1;
	} else {
		(*user_data_p) = NULLPE;
		ndata = 0;
	}

	result = AcRelResponse(sd, ACS_REJECT, ACR_NOTFINISHED, user_data_p, ndata, aci);

	if ((*user_data_p) != NULLPE) {
		pe_free((*user_data_p));
	}

	if (result == NOTOK) {
		LLOG(rosap_log, LLOG_EXCEPTIONS, ("RoUnBindError: AcRelResponse failed"));
		(void) acs2ronotlose(rni, "RO-UNBIND.ERROR", aca);
		ACAFREE(aca);
		return (NOTOK);
	}

	return (result);
}

/*    RO-UNBIND.REJECT */

/* ARGSUSED */

int
RoUnBindReject(sd, status, reason, rni)
	int sd;
	int status;
	int reason;
	struct RoNOTindication *rni;
{
	int result;
	struct AcSAPindication aci_s;
	struct AcSAPindication *aci = &(aci_s);
	struct AcSAPabort *aca = &(aci->aci_abort);

	LLOG(rosap_log, LLOG_EXCEPTIONS, ("RoUnBindReject: RO-UNBIND.REJECT called on %d", sd));

	result = AcRelResponse(sd, status, reason, NULLPEP, 0, aci);

	if (result == NOTOK) {
		LLOG(rosap_log, LLOG_EXCEPTIONS, ("RoUnBindReject: AcRelResponse failed"));
		(void) acs2ronotlose(rni, "RO-UNBIND.ERROR", aca);
		ACAFREE(aca);
		return (NOTOK);
	}

	return (result);
}
