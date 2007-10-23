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

/* ns_ro_ureject.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/task_ureject.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/task_ureject.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: task_ureject.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include "rosap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern LLog *log_dsap;

send_ro_ureject(ad, id_p, urej)
	int ad;
	int *id_p;
	int urej;
{
	struct RoSAPindication roi_s;
	struct RoSAPindication *roi = &roi_s;
	struct RoSAPpreject *rop = &(roi->roi_preject);

	DLOG(log_dsap, LLOG_TRACE, ("send_ro_ureject()"));

	watch_dog("RoURejectRequest");
	if (RoURejectRequest(ad, id_p, urej, ROS_NOPRIO, roi) == NOTOK) {
		watch_dog_reset();
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("RO-U-REJECT.REQUEST: %s",
						 RoErrString(rop->rop_reason)));
		if (ROS_FATAL(rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG(log_dsap, LLOG_FATAL, ("RoUReject fatal PReject"));
		}
	} else
		watch_dog_reset();

}
