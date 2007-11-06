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

/*
 ****************************************************************
 *                                                              *
 *  HULA project - connectionless ISODE                         *
 *                                             			*
 *  module:  	ro2acu.c                                        *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code implements the mapping of ROSE remote operations  *
 *  onto a connectionless UNIT-DATA protocol stack.             *
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 * 	RoAcuService (acb, roi)                                 *
 * 	ro2acuwait (acb, invokeID, secs, roi)                   *
 * 	ro2acuwrite (acb, pe, fe, priority, roi)                *
 *                                                              *
 *  internal routines:                                          *
 *                                                              *
 *      ValidBinding ( acb, acs )                               *
 *      aculose (acb, roi, event, aca)                          *
 *								*
 *								*
 ****************************************************************
 *								*
 *			     NOTICE		   		*
 *								*
 *    Use of this module is subject to the restrictions of the  *
 *    ISODE license agreement.					*
 *								*    
 ****************************************************************
 */

/* ro2acu.c - modified from ro2ps.c - ROPM: PSAP interface */

#ifndef	lint
static char *rcsid = "Header: /f/iso/rosap/RCS/ro2ps.c,v 5.0 88/07/21 14:55:50 mrose Rel";
#endif

/* LINTLIBRARY */

#include <stdio.h>
#include "ropkt.h"
#include "tailor.h"

/*    DATA */

int aculose();

/*---------------------------------------------------------------------------*/
/*    bind underlying service */
/*---------------------------------------------------------------------------*/
int
RoAcuService(acb, roi)
/*---------------------------------------------------------------------------*/
	register struct assocblk *acb;
	struct RoSAPindication *roi;
{

	if (!(acb->acb_flags & ACB_AUDT))
		return rosaplose(roi, ROS_OPERATION, NULLCP,
				 "not an association descriptor for ROS on A-UNIT-DATA");

	acb->acb_putosdu = ro2acuwrite;
	acb->acb_rowaitrequest = ro2acuwait;
	acb->acb_ready = NULLIFP;
	acb->acb_rosetindications = NULLIFP;	/* until ro2acuasync */
	acb->acb_roselectmask = NULLIFP;	/* until synch intr supported */
	acb->acb_ropktlose = NULLIFP;	/* AcuUnbind ??? */

	return OK;
}

/* leave this here for the future when we add async */
/*
*    define vectors for INDICATION events *
#define	e(i)	(indication ? (i) : NULLIFP)
* ARGSUSED *
int	ro2acuasync (acb, indication, roi)
register struct assocblk   *acb;
IFP	indication;
struct RoSAPindication *roi;
{
#undef	e
*/

/* leave this here until we add ROS_INTR and async */
/*
*    map association descriptors for select() *
* ARGSUSED *
int	ro2acumask (acb, mask, nfds, roi)
register struct assocblk   *acb;
fd_set *mask;
int    *nfds;
struct RoSAPindication *roi;
{
    struct AcSAPindication   acis;
    register struct AcSAPindication *aci = &acis;
    register struct AcSAPabort *aca = &acis.aci_abort;

    if (AcuSelectMask (acb -> acb_fd, mask, nfds, aci) == NOTOK)
	switch (aca -> aca_reason) {
	    case PC_WAITING: 
		return rosaplose (roi, ROS_WAITING, NULLCP, NULLCP);

	    default: 
		(void) aculose (acb, roi, "PSelectMask", aca);
		freeacblk (acb);
		return NOTOK;
	}
    return OK;
}
*/

/*---------------------------------------------------------------------------*/
int
ro2acuwait(acb, invokeID, secs, roi)
/*---------------------------------------------------------------------------*/
	register struct assocblk *acb;
	int *invokeID, secs;
	register struct RoSAPindication *roi;
{
	int fd, result;
	register PE pe;
	struct AcuSAPstart acss;
	struct AcuSAPstart *acs = &acss;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &acis.aci_abort;

	fd = acb->acb_fd;

	for (;;) {
		switch (result = AcUnitDataRead(fd, acs, secs, aci)) {
		case NOTOK:
			return aculose(NULLACB, roi, "AcUnitDataRead", aca);

		default:
			return rosaplose(roi, ROS_PROTOCOL, NULLCP,
					 "unexpected return from AcUnitDataRead=%d", result);

		case OK:
			/* check received AUDT against binding */

			if (ValidBinding(acb, acs) == NOTOK) {
				rosaplose(roi, ROS_ACS, NULLCP,
					  "A-Unit-Data read did not match binding");
				ACSFREE(acs);
				return NOTOK;
			}

			/* a server may need to reset binding to respond to client */

			if (acb->acb_binding == BIND_DYNAMIC)
				if (AcUnitDataRebind(fd, &acs->acs_callingtitle,
						     &acs->acs_start.ps_calling, aci)
				    == NOTOK) {
					aculose(NULLACB, roi, "AcUnitDataRebind", aca);
					ACSFREE(acs);
					return NOTOK;
				}

			pe = acs->acs_info[0], acs->acs_info[0] = NULLPE;
			ACSFREE(acs);

			if ((result = acb2osdu(acb, invokeID, pe, roi)) != OK)
				return (result != DONE ? result : OK);
			continue;
		}
	}
}

/*  */

/* ARGSUSED */

/*---------------------------------------------------------------------------*/
int
ro2acuwrite(acb, pe, fe, priority, roi)
/*---------------------------------------------------------------------------*/
	register struct assocblk *acb;
	PE pe, fe;
	int priority;
	struct RoSAPindication *roi;
{
	int result;
	struct AcSAPindication acis;
	register struct AcSAPindication *aci = &acis;
	register struct AcSAPabort *aca = &acis.aci_abort;

	pe->pe_context = acb->acb_rosid;

#ifdef	DEBUG
	if (rosaplevel & ISODELOG_PDUS) {
		int isopen;
		FILE *fp;

		if (strcmp(rosapfile, "-")) {
			char file[BUFSIZ];

			(void) sprintf(file, rosapfile, getpid());
			fp = fopen(file, "a"), isopen = 1;
		} else
			fp = stderr, isopen = 0, (void) fflush(stdout);

		if (fp) {
			vpushfp(fp, pe, "ROSEapdus", 0);
			(void) print_ROS_ROSEapdus(pe, 1, NULLIP, NULLVP, NULLCP);
			vpopfp();

			if (isopen)
				(void) fclose(fp);
			else
				(void) fflush(fp);
		}
	}
#endif

	if ((result = AcUnitDataWrite(acb->acb_fd, &pe, 1, aci)) == NOTOK) {
		(void) aculose(acb, roi, "AcUnitDataWrite", aca);
	}

	if (fe)
		(void) pe_extract(pe, fe);
	pe_free(pe);

	return result;
}

/*---------------------------------------------------------------------------*/
int
ValidBinding(acb, acs)
/*---------------------------------------------------------------------------*/
	struct assocblk *acb;
	struct AcuSAPstart *acs;
{

#define	AEICMP(aei1,aei2) \
    (pe_cmp ((aei1) -> aei_ap_title, (aei2) -> aei_ap_title) \
  || pe_cmp ((aei1) -> aei_ae_qualifier, (aei2) -> aei_ae_qualifier))

	if (oid_cmp(acb->acb_context, acs->acs_context))
		return NOTOK;

	if (acb->acb_callingtitle && AEICMP(acb->acb_callingtitle, &acs->acs_calledtitle))
		return NOTOK;

	if (acb->acb_binding == BIND_STATIC
	    && acb->acb_calledtitle && AEICMP(acb->acb_calledtitle, &acs->acs_callingtitle))
		return NOTOK;
	return OK;
}

/*---------------------------------------------------------------------------*/
/*    AcSAP interface */
/*---------------------------------------------------------------------------*/
static int
aculose(acb, roi, event, aca)
/*---------------------------------------------------------------------------*/
	register struct assocblk *acb;
	register struct RoSAPindication *roi;
	char *event;
	register struct AcSAPabort *aca;
{
	int reason;
	char *cp, buffer[BUFSIZ];

	if ((rosaplevel & ISODELOG_EXCEPTIONS) && event)
		xsprintf(NULLCP, NULLCP,
			 aca->aca_cc > 0 ? "%s: %s\n\t%*.*s" : "%s: %s", event,
			 AcErrString(aca->aca_reason), aca->aca_cc, aca->aca_cc, aca->aca_data);

	cp = "";
	switch (aca->aca_reason) {
	case ACS_ADDRESS:
		reason = ROS_ADDRESS;
		break;

	case ACS_REFUSED:
		reason = ROS_REFUSED;
		break;

	case ACS_CONGEST:
		reason = ROS_CONGEST;
		break;

	default:
		(void) sprintf(cp = buffer, " (%s at association control)",
			       AcErrString(aca->aca_reason));
	case ACS_PRESENTATION:
		reason = ROS_ACS;
		break;
	}

	if (aca->aca_cc > 0)
		return rosaplose(roi, reason, NULLCP, "%*.*s%s",
				 aca->aca_cc, aca->aca_cc, aca->aca_data, cp);
	else
		return rosaplose(roi, reason, NULLCP, "%s", cp);
}
