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

/* rosapapdu.c - ROPM: interpret APDU */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosap/RCS/rosapapdu.c,v 9.0 1992/06/16 12:37:02 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosap/RCS/rosapapdu.c,v 9.0 1992/06/16 12:37:02 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: rosapapdu.c,v
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
#include <signal.h>
#include "ROS-types.h"
#include "ropkt.h"
#ifdef	DEBUG
#include "tailor.h"
#endif

static int prob2num();
static int apdu_proc();
static int opdu_proc();

/*  */

int
acb2osdu(acb, invokeID, pe, roi)
	register struct assocblk *acb;
	int *invokeID;
	PE pe;
	register struct RoSAPindication *roi;
{
	struct type_ROS_ROSEapdus *papdu;
	struct type_ROS_OPDU *popdu;
	int rosap_type;
	int result;

	rosap_type = APDU_UNKNOWN;
	if (((acb->acb_flags & ACB_ACS)
	     ? decode_ROS_ROSEapdus(pe, 1, NULLIP, NULLVP, &papdu)
	     : decode_ROS_OPDU(pe, 1, NULLIP, NULLVP, &popdu))
	    == NOTOK) {
		(void) rosapreject(acb, roi, rosap_type != APDU_UNKNOWN
				   ? ROS_GP_MISTYPED : ROS_GP_UNRECOG, NULLCP, "%s", PY_pepy);
		pe_free(pe);

		return NOTOK;
	}
#ifdef	DEBUG
	if (rosap_log->ll_events & LLOG_PDUS)
		if (acb->acb_flags & ACB_ACS)
			pvpdu(rosap_log, print_ROS_ROSEapdus_P, pe, "ROSEapdus", 1);
		else
			pvpdu(rosap_log, print_ROS_OPDU_P, pe, "OPDU", 1);
#endif

	if ((acb->acb_flags & ACB_ACS)) {
		result = apdu_proc(acb->acb_fd, papdu, &pe, roi, acb, invokeID);
		if (papdu)
			free_ROS_ROSEapdus(papdu);
	} else {
		result = opdu_proc(acb->acb_fd, popdu, &pe, roi, acb, invokeID);
		if (popdu)
			free_ROS_OPDU(popdu);
	}

	if (pe)
		pe_free(pe);

	return result;
}

/*  */

/*
 * Process an APDU. This separates out all the differences between
 * the two cases. Copy all the fields of the structure into the appropriate
 * RoSAPindication structure
 */

static int
apdu_proc(sd, papdu, pe, roi, acb, invokeID)
	int sd;
	struct type_ROS_ROSEapdus *papdu;
	PE *pe;
	register struct RoSAPindication *roi;
	register struct assocblk *acb;
	int *invokeID;
{
	int rosap_id;

	bzero((char *) roi, sizeof *roi);
	switch (papdu->offset) {
	case type_ROS_ROSEapdus_roiv__apdu:
		roi->roi_type = ROI_INVOKE;
		{
			register struct RoSAPinvoke *rox = &roi->roi_invoke;
			register struct type_ROS_ROIVapdu *piv = papdu->un.roiv__apdu;

			rosap_id = rox->rox_id = piv->invokeID->parm;
			if (!(rox->rox_nolinked = !(piv->optionals & opt_ROS_ROIVapdu_linked__ID)))
				rox->rox_linkid = piv->linked__ID;
			rox->rox_op = piv->operation__value->parm;
			rox->rox_args = pe_expunge(*pe, piv->argument);
			*pe = NULLPE;
		}
		break;

	case type_ROS_ROSEapdus_rors__apdu:
		roi->roi_type = ROI_RESULT;
		{
			register struct RoSAPresult *ror = &roi->roi_result;
			register struct type_ROS_RORSapdu *pres = papdu->un.rors__apdu;

			rosap_id = ror->ror_id = pres->invokeID->parm;
			if (pres->element_ROS_0) {
				ror->ror_op = pres->element_ROS_0->operation__value->parm;
				ror->ror_result = pe_expunge(*pe, pres->element_ROS_0->result);
				*pe = NULLPE;
			}
		}
		break;

	case type_ROS_ROSEapdus_roer__apdu:
		roi->roi_type = ROI_ERROR;
		{
			register struct RoSAPerror *roe = &roi->roi_error;
			register struct type_ROS_ROERapdu *perr = papdu->un.roer__apdu;

			rosap_id = roe->roe_id = perr->invokeID->parm;
			roe->roe_error = perr->error__value->parm;
			roe->roe_param = pe_expunge(*pe, perr->parameter);
			*pe = NULLPE;
		}
		break;

	case type_ROS_ROSEapdus_rorj__apdu:
		roi->roi_type = ROI_UREJECT;
		{
			register struct RoSAPureject *rou = &roi->roi_ureject;
			register struct type_ROS_RORJapdu *prej = papdu->un.rorj__apdu;

			if (prej->invokeID->offset == choice_ROS_0_2)
				rou->rou_noid = 1;
			else {
				rosap_id = rou->rou_id = prej->invokeID->un.choice_ROS_1->parm;
				rou->rou_noid = 0;
			}

			rou->rou_reason = prob2num(prej->problem);
		}
		break;
	}

	if (invokeID
	    && (papdu->offset == type_ROS_ROSEapdus_roiv__apdu
		|| (papdu->offset == type_ROS_ROSEapdus_rorj__apdu
		    && papdu->un.rorj__apdu->invokeID->offset == choice_ROS_0_2)
		|| *invokeID != rosap_id)
	    && acb->acb_rosindication) {
		(*acb->acb_rosindication) (sd = acb->acb_fd, roi);

		if (findacblk(sd) != acb)	/* still not perfect! */
			return rosaplose(roi, ROS_DONE, NULLCP, NULLCP);
		return OK;
	}

	return DONE;
}

/*  */

/*
 * Process an OPDU. A separate function is used for this type of PDU to
 * simpilfy matters. What is an OPDU - an Old PDU ??
 */

static int
opdu_proc(sd, popdu, pe, roi, acb, invokeID)
	int sd;
	struct type_ROS_OPDU *popdu;
	PE *pe;
	register struct RoSAPindication *roi;
	register struct assocblk *acb;
	int *invokeID;
{
	int rosap_id;

	bzero((char *) roi, sizeof *roi);
	switch (popdu->offset) {
	case type_ROS_OPDU_1:
		roi->roi_type = ROI_INVOKE;
		{
			register struct RoSAPinvoke *rox = &roi->roi_invoke;
			register struct type_ROS_Invoke *piv = popdu->un.choice_ROS_8;

			rosap_id = rox->rox_id = piv->invokeID;
			rox->rox_op = piv->element_ROS_2->parm;
			rox->rox_args = pe_expunge(*pe, piv->argument);
			*pe = NULLPE;
		}
		break;

	case type_ROS_OPDU_2:
		roi->roi_type = ROI_RESULT;
		{
			register struct RoSAPresult *ror = &roi->roi_result;
			register struct type_ROS_ReturnResult *piv = popdu->un.choice_ROS_9;

			rosap_id = ror->ror_id = piv->invokeID->parm;
			ror->ror_result = pe_expunge(*pe, piv->result);
			*pe = NULLPE;
		}
		break;

	case type_ROS_OPDU_3:
		roi->roi_type = ROI_ERROR;
		{
			register struct RoSAPerror *roe = &roi->roi_error;
			register struct type_ROS_ReturnError *per = popdu->un.choice_ROS_10;

			rosap_id = roe->roe_id = per->invokeID;;
			roe->roe_error = per->element_ROS_3->parm;
			roe->roe_param = pe_expunge(*pe, per->parameter);
			*pe = NULLPE;
		}
		break;

	case type_ROS_OPDU_4:
		roi->roi_type = ROI_UREJECT;
		{
			register struct RoSAPureject *rou = &roi->roi_ureject;
			register struct type_ROS_Reject *prj = popdu->un.choice_ROS_11;

			if (prj->invokeID->offset == choice_ROS_0_2)
				rou->rou_noid = 1;
			else {
				rosap_id = rou->rou_id = prj->invokeID->un.choice_ROS_1->parm;
				rou->rou_noid = 0;
			}

			rou->rou_reason = prob2num(prj->problem);
		}
		break;
	}

	if (invokeID
	    && (popdu->offset == type_ROS_OPDU_1
		|| (popdu->offset == type_ROS_OPDU_4
		    && popdu->un.choice_ROS_11->invokeID->offset == choice_ROS_0_2)
		|| *invokeID != rosap_id)
	    && acb->acb_rosindication) {
		(*acb->acb_rosindication) (sd = acb->acb_fd, roi);

		if (findacblk(sd) != acb)	/* still not perfect! */
			return rosaplose(roi, ROS_DONE, NULLCP, NULLCP);
		return OK;
	}

	return DONE;
}

/*  */

static int Gprob[] = { ROS_GP_UNRECOG, ROS_GP_MISTYPED, ROS_GP_STRUCT };
static int Iprob[] = { ROS_IP_DUP, ROS_IP_UNRECOG, ROS_IP_MISTYPED,
	ROS_IP_LIMIT, ROS_IP_RELEASE, ROS_IP_UNLINKED,
	ROS_IP_LINKED, ROS_IP_CHILD
};
static int RRprob[] = { ROS_RRP_UNRECOG, ROS_RRP_UNEXP, ROS_RRP_MISTYPED };
static int REprob[] = { ROS_REP_UNRECOG, ROS_REP_UNEXP, ROS_REP_RECERR,
	ROS_REP_UNEXPERR, ROS_REP_MISTYPED
};

/* computes the Number of entries in an array a */
#define NENTRIES(a) (sizeof (a)/sizeof (a[0]))

/* return the ISODE code from the numbers passed in the data or NOTOK if 
 * it finds an illegal value
 */
static int
prob2num(prob)
	register struct choice_ROS_3 *prob;
{
	register int num;

	switch (prob->offset) {
	case choice_ROS_3_1:
		if ((num = prob->un.choice_ROS_4->parm) < 0 || num >= NENTRIES(Gprob))
			goto out;
		num = Gprob[num];
		break;

	case choice_ROS_3_2:
		if ((num = prob->un.choice_ROS_5->parm) < 0 || num >= NENTRIES(Iprob))
			goto out;
		num = Iprob[num];
		break;

	case choice_ROS_3_3:
		if ((num = prob->un.choice_ROS_6->parm) < 0 || num >= NENTRIES(RRprob))
			goto out;
		num = RRprob[num];
		break;

	case choice_ROS_3_4:
		if ((num = prob->un.choice_ROS_7->parm) < 0 || num >= NENTRIES(REprob))
			goto out;
		num = REprob[num];
		break;

	default:
	      out:;
		return ROS_PROTOCOL;	/* What else can we say ? */
	}

	return (num);

}
