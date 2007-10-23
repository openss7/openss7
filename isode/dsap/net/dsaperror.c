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

/* dsaperror.c - DSAP : Return operation errors */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/net/RCS/dsaperror.c,v 9.0 1992/06/16 12:14:05 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/net/RCS/dsaperror.c,v 9.0 1992/06/16 12:14:05 isode Rel
 *
 *
 * Log: dsaperror.c,v
 * Revision 9.0  1992/06/16  12:14:05  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include <stdio.h>
#include "logger.h"
#include "quipu/dsap.h"
#include "../x500as/DAS-types.h"

extern LLog *log_dsap;

int
DapErrorRequest(sd, id, err, di)
	int sd;
	int id;
	struct DSError *err;
	struct DSAPindication *di;
{
	int result;
	PE err_pe;
	struct RoSAPindication roi_s;
	struct RoSAPindication *roi = &(roi_s);
	struct RoSAPpreject *rop = &(roi->roi_preject);

	if (DEncodeError(&(err_pe), err) != OK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("DapErrorRequest: Encoding failed"));
		return (dsapreject(di, DP_INVOKE, id, NULLCP, "Failed to encode operation error"));
	}

	watch_dog("RoErrorRequest (DAP)");
	result = RoErrorRequest(sd, id, err->dse_type, err_pe, ROS_NOPRIO, roi);
	watch_dog_reset();

	if (result != OK) {
		if (ROS_FATAL(rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG(log_dsap, LLOG_EXCEPTIONS,
			     ("DapErrorRequest(%d): Fatal rejection", sd));
			return (dsaplose(di, DP_INVOKE, NULLCP, "RoErrorRequest failed"));
		} else {
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("DapErrorRequest(): Non-Fatal rejection"));
			return (dsapreject(di, DP_INVOKE, id, NULLCP, "RoErrorRequest failed"));
		}
	}

	if (err_pe != NULLPE)
		pe_free(err_pe);

	return (OK);
}

int
DspErrorRequest(sd, id, err, di)
	int sd;
	int id;
	struct DSError *err;
	struct DSAPindication *di;
{
	int result;
	PE err_pe;
	struct RoSAPindication roi_s;
	struct RoSAPindication *roi = &(roi_s);
	struct RoSAPpreject *rop = &(roi->roi_preject);

	if (DEncodeError(&(err_pe), err) != OK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("DspErrorRequest: Encoding failed"));
		return (dsapreject(di, DP_INVOKE, id, NULLCP, "Failed to encode operation error"));
	}

	watch_dog("RoErrorRequest (DSP)");
	result = RoErrorRequest(sd, id, err->dse_type, err_pe, ROS_NOPRIO, roi);
	watch_dog_reset();

	if (result != OK) {
		if (ROS_FATAL(rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("DspErrorRequest(): Fatal rejection"));
			return (dsaplose(di, DP_INVOKE, NULLCP, "RoErrorRequest failed"));
		} else {
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("DspErrorRequest(): Non-Fatal rejection"));
			return (dsapreject(di, DP_INVOKE, id, NULLCP, "RoErrorRequest failed"));
		}
	}

	if (err_pe != NULLPE)
		pe_free(err_pe);

	return (OK);
}

int
QspErrorRequest(sd, id, err, di)
	int sd;
	int id;
	struct DSError *err;
	struct DSAPindication *di;
{
	int result;
	PE err_pe;
	struct RoSAPindication roi_s;
	struct RoSAPindication *roi = &(roi_s);
	struct RoSAPpreject *rop = &(roi->roi_preject);

	if (DEncodeError(&(err_pe), err) != OK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("QspErrorRequest: Encoding failed"));
		return (dsapreject(di, DP_INVOKE, id, NULLCP, "Failed to encode operation error"));
	}

	watch_dog("RoErrorRequest (QSP)");
	result = RoErrorRequest(sd, id, err->dse_type, err_pe, ROS_NOPRIO, roi);
	watch_dog_reset();

	if (result != OK) {
		if (ROS_FATAL(rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("QspErrorRequest(): Fatal rejection"));
			return (dsaplose(di, DP_INVOKE, NULLCP, "RoErrorRequest failed"));
		} else {
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("QspErrorRequest(): Non-Fatal rejection"));
			return (dsapreject(di, DP_INVOKE, id, NULLCP, "RoErrorRequest failed"));
		}
	}

	if (err_pe != NULLPE)
		pe_free(err_pe);

	return (OK);
}

int
DEncodeError(pep, err)
	PE *pep;
	struct DSError *err;
{
	int success;

	switch (err->dse_type) {
	case DSE_NOERROR:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("DEncodeError(): no error"));
		(*pep) = NULLPE;
		success = NOTOK;
		break;
	case DSE_ABANDON_FAILED:
		success =
		    encode_DAS_AbandonFailedParm(pep, 1, 0, NULLCP,
						 &(err->dse_un.dse_un_abandon_fail));
		break;
	case DSE_ATTRIBUTEERROR:
		success =
		    encode_DAS_AttributeErrorParm(pep, 1, 0, NULLCP,
						  &(err->dse_un.dse_un_attribute));
		break;
	case DSE_NAMEERROR:
		success = encode_DAS_NameErrorParm(pep, 1, 0, NULLCP, &(err->dse_un.dse_un_name));
		break;
	case DSE_REFERRAL:
		success =
		    encode_DAS_ReferralParm(pep, 1, 0, NULLCP, &(err->dse_un.dse_un_referral));
		break;
	case DSE_SECURITYERROR:
		success =
		    encode_DAS_SecurityErrorParm(pep, 1, 0, NULLCP, &(err->dse_un.dse_un_security));
		break;
	case DSE_SERVICEERROR:
		success =
		    encode_DAS_ServiceErrorParm(pep, 1, 0, NULLCP, &(err->dse_un.dse_un_service));
		break;
	case DSE_UPDATEERROR:
		success =
		    encode_DAS_UpdateErrorParm(pep, 1, 0, NULLCP, &(err->dse_un.dse_un_update));
		break;
	case DSE_ABANDONED:
		(*pep) = NULLPE;
		success = OK;
		break;
	case DSE_DSAREFERRAL:
		success =
		    encode_DO_DSAReferralParm(pep, 1, 0, NULLCP, &(err->dse_un.dse_un_referral));
		break;
	default:
		(*pep) = NULLPE;
		success = NOTOK;
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("DEncodeError(): unknown error %d", err->dse_type));
		break;
	}

	return (success);
}

int
IspErrorRequest(sd, id, err, di)
	int sd;
	int id;
	struct DSError *err;
	struct DSAPindication *di;
{
	return QspErrorRequest(sd, id, err, di);
}
