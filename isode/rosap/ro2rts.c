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

/* ro2rts.c - ROPM: RtSAP interface */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/ro2rts.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/ro2rts.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: ro2rts.c,v
 * Revision 9.0  1992/06/16  12:37:02  isode
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
#include "ROS-types.h"
#include "ropkt.h"
#include "tailor.h"

/*    DATA */

static int rtslose();
static int rtsINDICATIONser();

static int doRTSturn();
static int doRTSclose();
static int doRTSfinish();
static int doRTSabort();

/*    bind underlying service */

int
RoRtService(acb, roi)
	register struct assocblk *acb;
	struct RoSAPindication *roi;
{
	if (!(acb->acb_flags & ACB_RTS))
		return rosaplose(roi, ROS_OPERATION, NULLCP,
				 "not an association descriptor for ROS on reliable transfer");

	acb->acb_putosdu = ro2rtswrite;
	acb->acb_rowaitrequest = ro2rtswait;
	acb->acb_ready = ro2rtsready;
	acb->acb_rosetindications = ro2rtsasync;
	acb->acb_roselectmask = ro2rtsmask;
	acb->acb_ropktlose = NULLIFP;

	acb->acb_flags |= ACB_STICKY;

	return OK;
}

/*    define vectors for INDICATION events */

#define	e(i)	(indication ? (i) : NULLIFP)

/* ARGSUSED */

int
ro2rtsasync(acb, indication, roi)
	register struct assocblk *acb;
	IFP indication;
	struct RoSAPindication *roi;
{
	struct RtSAPindication rtis;
	register struct RtSAPabort *rta = &rtis.rti_abort;

	if (acb->acb_rosindication = indication)
		acb->acb_flags |= ACB_ASYN;
	else
		acb->acb_flags &= ~ACB_ASYN;

	if (RtSetIndications(acb->acb_fd, e(rtsINDICATIONser), &rtis)
	    == NOTOK) {
		acb->acb_flags &= ~ACB_ASYN;
		switch (rta->rta_reason) {
		case RTS_WAITING:
			return rosaplose(roi, ROS_WAITING, NULLCP, NULLCP);

		default:
			(void) rtslose(acb, roi, "RtSetIndications", rta);
			freeacblk(acb);
			return NOTOK;
		}
	}

	return OK;
}

#undef	e

/*    map association descriptors for select() */

/* ARGSUSED */

int
ro2rtsmask(acb, mask, nfds, roi)
	register struct assocblk *acb;
	fd_set *mask;
	int *nfds;
	struct RoSAPindication *roi;
{
	struct RtSAPindication rtis;
	register struct RtSAPabort *rta = &rtis.rti_abort;

	if (RtSelectMask(acb->acb_fd, mask, nfds, &rtis) == NOTOK)
		switch (rta->rta_reason) {
		case RTS_WAITING:
			return rosaplose(roi, ROS_WAITING, NULLCP, NULLCP);

		default:
			(void) rtslose(acb, roi, "RtSelectMask", rta);
			freeacblk(acb);
			return NOTOK;
		}

	return OK;
}

/*    RtSAP interface */

#define	doRTSdata(a,i,t,r)	acb2osdu ((a), (i), (t) -> rtt_data, (r))

int
ro2rtswait(acb, invokeID, secs, roi)
	register struct assocblk *acb;
	int *invokeID, secs;
	register struct RoSAPindication *roi;
{
	int result;
	struct RtSAPindication rtis;
	register struct RtSAPindication *rti = &rtis;

	if (acb->acb_apdu) {
		result = acb2osdu(acb, NULLIP, acb->acb_apdu, roi);
		acb->acb_apdu = NULLPE;

		return result;
	}
	if (acb->acb_flags & ACB_CLOSING) {
		acb->acb_flags |= ACB_FINN;
		acb->acb_flags &= ~ACB_CLOSING;
		if (acb->acb_flags & ACB_FINISH) {
			acb->acb_flags &= ~ACB_FINISH;

			roi->roi_type = ROI_FINISH;
			{
				register struct AcSAPfinish *acf = &roi->roi_finish;

				*acf = acb->acb_finish;	/* struct copy */
			}
		} else {
			roi->roi_type = ROI_END;
			{
				register struct RoSAPend *roe = &roi->roi_end;

				bzero((char *) roe, sizeof *roe);
			}
		}

		return DONE;
	}

	for (;;) {
		switch (result = RtWaitRequest(acb->acb_fd, secs, rti)) {
		case NOTOK:
			return doRTSabort(acb, &rti->rti_abort, roi);

		case OK:
			if ((result = doRTSdata(acb, invokeID, &rti->rti_transfer, roi)) != OK)
				return (result != DONE ? result : OK);
			continue;

		case DONE:
			switch (rti->rti_type) {
			case RTI_TURN:
				if (doRTSturn(acb, &rti->rti_turn, roi) != OK)
					return result;
				continue;

			case RTI_CLOSE:
				if (doRTSclose(acb, &rti->rti_close, roi) != OK)
					return result;
				continue;

			case RTI_FINISH:
				if (doRTSfinish(acb, &rti->rti_finish, roi) != OK)
					return result;
				continue;

			default:
				(void) ropktlose(acb, roi, ROS_PROTOCOL, NULLCP,
						 "unknown indication (0x%x) from rts",
						 rti->rti_type);
				break;
			}
			break;

		default:
			(void) ropktlose(acb, roi, ROS_PROTOCOL, NULLCP,
					 "unexpected return from RtWaitRequest=%d", result);
			break;
		}
		break;
	}

	freeacblk(acb);
	return NOTOK;
}

/*  */

/* ARGSUSED */

int
ro2rtswrite(acb, pe, fe, priority, roi)
	register struct assocblk *acb;
	register PE pe;
	PE fe;
	int priority;
	struct RoSAPindication *roi;
{
	int result;
	struct RtSAPindication rtis;
	register struct RtSAPabort *rta = &rtis.rti_abort;

#ifdef	DEBUG
	if (rosap_log->ll_events & LLOG_PDUS)
		if (acb->acb_flags & ACB_ACS)
			pvpdu(rosap_log, print_ROS_ROSEapdus_P, pe, "ROSEapdus", 0);
		else
			pvpdu(rosap_log, print_ROS_OPDU_P, pe, "OPDU", 0);
#endif

	result = RtTransferRequest(acb->acb_fd, pe, NOTOK, &rtis);

	if (fe)
		(void) pe_extract(pe, fe);
	pe_free(pe);

	if (result == OK)
		return OK;

	if (rta->rta_reason == RTS_TRANSFER)
		return rosaplose(roi, ROS_APDU, NULLCP, NULLCP);

	(void) rtslose(acb, roi, "RtTransferRequest", rta);

	freeacblk(acb);
	return NOTOK;
}

/*  */

static int
doRTSturn(acb, rtu, roi)
	register struct assocblk *acb;
	register struct RtSAPturn *rtu;
	register struct RoSAPindication *roi;
{
	struct RtSAPindication rtis;
	register struct RtSAPindication *rti = &rtis;
	register struct RtSAPabort *rta = &rti->rti_abort;

	if (rtu->rtu_please) {
		if (RtGTurnRequest(acb->acb_fd, rti) == NOTOK)
			return rtslose(acb, roi, "RtGTurnRequest", rta);
	}

	return OK;
}

/*  */

/* ARGSUSED */

static int
doRTSclose(acb, rtc, roi)
	register struct assocblk *acb;
	struct RtSAPclose *rtc;
	register struct RoSAPindication *roi;
{
	if (acb->acb_flags & ACB_INIT) {
		(void) ropktlose(acb, roi, ROS_PROTOCOL, NULLCP, "association management botched");
		freeacblk(acb);
		return NOTOK;
	}

	roi->roi_type = ROI_END;
	{
		register struct RoSAPend *roe = &roi->roi_end;

		bzero((char *) roe, sizeof *roe);
	}

	return DONE;
}

/*  */

static int
doRTSfinish(acb, acf, roi)
	register struct assocblk *acb;
	struct AcSAPfinish *acf;
	register struct RoSAPindication *roi;
{
	if (acb->acb_flags & ACB_INIT) {
		(void) ropktlose(acb, roi, ROS_PROTOCOL, NULLCP, "association management botched");
		ACFFREE(acf);
		freeacblk(acb);
		return NOTOK;
	}

	roi->roi_type = ROI_FINISH;
	roi->roi_finish = *acf;	/* struct copy */

	return DONE;
}

/*  */

static int
doRTSabort(acb, rta, roi)
	register struct assocblk *acb;
	register struct RtSAPabort *rta;
	register struct RoSAPindication *roi;
{
	if (!rta->rta_peer) {
		if (rta->rta_reason == RTS_TIMER)
			return rosaplose(roi, ROS_TIMER, NULLCP, NULLCP);

		(void) rtslose(acb, roi, NULLCP, rta);
	} else
		(void) rosaplose(roi, ROS_ABORTED, NULLCP, NULLCP);

	RTAFREE(rta);
	acb->acb_fd = NOTOK;
	freeacblk(acb);

	return NOTOK;
}

/*  */

static int
rtsINDICATIONser(sd, rti)
	int sd;
	register struct RtSAPindication *rti;
{
	int result;
	IFP handler;
	register struct assocblk *acb;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi = &rois;

	if ((acb = findacblk(sd)) == NULL)
		return;

	handler = acb->acb_rosindication;

	switch (rti->rti_type) {
	case RTI_TURN:
		result = doRTSturn(acb, &rti->rti_turn, roi);
		break;

	case RTI_TRANSFER:
		result = doRTSdata(acb, NULLIP, &rti->rti_transfer, roi);
		break;

	case RTI_ABORT:
		result = doRTSabort(acb, &rti->rti_abort, roi);
		break;

	case RTI_CLOSE:
		result = doRTSclose(acb, &rti->rti_close, roi);
		break;

	case RTI_FINISH:
		result = doRTSfinish(acb, &rti->rti_finish, roi);
		break;

	default:
		result = ropktlose(acb, roi, ROS_PROTOCOL, NULLCP,
				   "unknown indication (0x%x) from rts", rti->rti_type);
		freeacblk(acb);
		break;
	}

	if (result != OK)
		(*handler) (sd, roi);
}

/*  */

int
ro2rtsready(acb, priority, roi)
	register struct assocblk *acb;
	int priority;
	struct RoSAPindication *roi;
{
	int result;
	struct RtSAPindication rtis;
	register struct RtSAPindication *rti = &rtis;
	register struct RtSAPabort *rta = &rti->rti_abort;

	if (acb->acb_apdu || (acb->acb_flags & ACB_CLOSING))
		return rosaplose(roi, ROS_WAITING, NULLCP, NULLCP);

	if (RtPTurnRequest(acb->acb_fd, priority, rti) == NOTOK) {
		(void) rtslose(acb, roi, "RtPTurnRequest", rta);
		goto out;
	}

	do {
		switch (result = RtWaitRequest(acb->acb_fd, NOTOK, rti)) {
		case NOTOK:
			return doRTSabort(acb, &rti->rti_abort, roi);

		case OK:
			acb->acb_apdu = rti->rti_transfer.rtt_data;
			return rosaplose(roi, ROS_WAITING, NULLCP, NULLCP);

		case DONE:
			switch (rti->rti_type) {
			case RTI_TURN:
				if (doRTSturn(acb, &rti->rti_turn, roi) != OK)
					return result;
				continue;

			case RTI_CLOSE:
				switch (doRTSclose(acb, &rti->rti_close, roi)) {
				case NOTOK:
					return NOTOK;

				case OK:
					break;

				case DONE:
					acb->acb_flags |= ACB_CLOSING;
					acb->acb_flags &= ~ACB_FINN;
					return rosaplose(roi, ROS_WAITING, NULLCP, NULLCP);
				}
				continue;

			case RTI_FINISH:
				switch (doRTSfinish(acb, &rti->rti_finish, roi)) {
				case NOTOK:
					return NOTOK;

				case OK:
					break;

				case DONE:
					acb->acb_flags |= ACB_FINISH | ACB_CLOSING;
					acb->acb_flags &= ~ACB_FINN;
					/* struct copy */
					acb->acb_finish = roi->roi_finish;
					return rosaplose(roi, ROS_WAITING, NULLCP, NULLCP);
				}
				continue;

			default:
				(void) ropktlose(acb, roi, ROS_PROTOCOL, NULLCP,
						 "unknown indication (0x%x) from rts",
						 rti->rti_type);
				break;
			}
			goto out;

		default:
			(void) ropktlose(acb, roi, ROS_PROTOCOL, NULLCP,
					 "unexpected return from RtWaitRequest=%d", result);
			goto out;
		}
	}
	while (!(acb->acb_flags & ACB_TURN));

	return OK;

      out:;
	freeacblk(acb);
	return NOTOK;
}

/*  */

static int
rtslose(acb, roi, event, rta)
	register struct assocblk *acb;
	register struct RoSAPindication *roi;
	char *event;
	register struct RtSAPabort *rta;
{
	int reason;
	char *cp, buffer[BUFSIZ];

	if (event)
		SLOG(rosap_log, LLOG_EXCEPTIONS, NULLCP,
		     (rta->rta_cc > 0 ? "%s: %s [%*.*s]" : "%s: %s", event,
		      RtErrString(rta->rta_reason), rta->rta_cc, rta->rta_cc, rta->rta_data));

	cp = "";
	switch (rta->rta_reason) {
	case RTS_ADDRESS:
		reason = ROS_ADDRESS;
		break;

	case RTS_REFUSED:
		reason = ROS_REFUSED;
		break;

	case RTS_CONGEST:
		reason = ROS_CONGEST;
		break;

	default:
		(void) sprintf(cp = buffer, " (%s at rts)", RtErrString(rta->rta_reason));
	case RTS_SESSION:
		reason = ROS_RTS;
		break;
	}

	if (rta->rta_cc > 0)
		return ropktlose(acb, roi, reason, NULLCP, "%*.*s%s",
				 rta->rta_cc, rta->rta_cc, rta->rta_data, cp);
	else
		return ropktlose(acb, roi, reason, NULLCP, "%s", *cp ? cp + 1 : cp);
}
