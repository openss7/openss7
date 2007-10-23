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

/* plog.c - lower-layer logging routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/net/RCS/plog.c,v 9.0 1992/06/16 12:14:05 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/net/RCS/plog.c,v 9.0 1992/06/16 12:14:05 isode Rel
 *
 *
 * Log: plog.c,v
 * Revision 9.0  1992/06/16  12:14:05  isode
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

#include "quipu/util.h"
#include "quipu/dsap.h"
#include "tsap.h"

sighandler_t	abort_vector = NULL;

SFP abort_vector = NULL;

void
ros_log(rop, event)
	register struct RoSAPpreject *rop;
	char *event;
{
	int level = LLOG_EXCEPTIONS;

	if ((rop->rop_reason == ROS_TIMER) || (rop->rop_reason == ROS_ACS))
		level = LLOG_DEBUG;

	if (rop->rop_cc > 0)
		LLOG(log_dsap, level, ("%s: [%s] %*.*s", event,
				       RoErrString(rop->rop_reason), rop->rop_cc, rop->rop_cc,
				       rop->rop_data));
	else
		LLOG(log_dsap, level, ("%s: [%s]", event, RoErrString(rop->rop_reason)));

	if (rop->rop_reason == ROS_CONGEST && abort_vector)
		(*abort_vector) (-2);
}

void
acs_log(aca, event)
	register struct AcSAPabort *aca;
	char *event;
{
	if (aca->aca_cc > 0)
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s] %*.*s", event,
						 AcErrString(aca->aca_reason),
						 aca->aca_cc, aca->aca_cc, aca->aca_data));
	else
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s]", event, AcErrString(aca->aca_reason)));

	if (aca->aca_reason == ACS_CONGEST && abort_vector)
		(*abort_vector) (-2);
}

td_log(td, event)
	struct TSAPdisconnect *td;
	char *event;
{
	if (td->td_cc > 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s] %*.*s", event,
						 TErrString(td->td_reason),
						 td->td_cc, td->td_cc, td->td_data));
	} else {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("%s: [%s]", event, TErrString(td->td_reason)));
	}
}

int
SetROPS(ad)
	int ad;
{
	struct RoSAPindication roi_s;
	struct RoSAPindication *roi = &(roi_s);
	struct RoSAPpreject *rop = &(roi->roi_preject);

	if (RoSetService(ad, RoPService, roi) != OK) {
		(void) ros_log(rop, "RoSetService");
		return NOTOK;
	}
	return (OK);
}
