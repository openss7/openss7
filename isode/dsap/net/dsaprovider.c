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

/* dsaprovider.c - DSAP: Support for DSAP Actions */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/net/RCS/dsaprovider.c,v 9.0 1992/06/16 12:14:05 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/net/RCS/dsaprovider.c,v 9.0 1992/06/16 12:14:05 isode Rel
 *
 *
 * Log: dsaprovider.c,v
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
#include "quipu/dsap.h"
#include "tailor.h"

/*    BIND interface */

int
ronot2dsaplose(di, event, rni)
	struct DSAPindication *di;
	char *event;
	struct RoNOTindication *rni;
{

	char *cp;
	char buffer[BUFSIZ];

#ifdef DEBUG

	if (event)
		SLOG(addr_log, LLOG_EXCEPTIONS, NULLCP,
		     ((rni->rni_cc > 0) ? "%s: %s [%*.*s]" : "%s: %s",
		      event, AcErrString(rni->rni_reason), rni->rni_cc,
		      rni->rni_cc, rni->rni_data));
#endif

	(void) sprintf(cp = buffer, " (Error in RO-BIND)");

	if (rni->rni_cc > 0)
		return (dsaplose(di, DA_RO_BIND, NULLCP, "%*.*s%s",
				 rni->rni_cc, rni->rni_cc, rni->rni_data, cp));
	else
		return (dsaplose(di, DA_RO_BIND, NULLCP, "%s", cp));
}

/*    ROS interface */

int
ros2dsaplose(di, event, rop)
	struct DSAPindication *di;
	char *event;
	struct RoSAPpreject *rop;
{
	char *cp;
	char buffer[BUFSIZ];

/*
	if (event)
		SLOG (addr_log, LLOG_EXCEPTIONS, NULLCP,
			((rop->rop_cc > 0) ? "%s: %s [%*.*s]" : "%s: %s",
			event, AcErrString (rop->rop_reason), rop->rop_cc,
			rop->rop_cc, rop->rop_data));
*/
	(void) sprintf(cp = buffer, " (Error in ROS)");

	if (rop->rop_cc > 0)
		return (dsaplose(di, DA_ROS, NULLCP, "%*.*s%s",
				 rop->rop_cc, rop->rop_cc, rop->rop_data, cp));
	else
		return (dsaplose(di, DA_ROS, NULLCP, "%s", cp));
}

int
ros2dsapreject(di, event, rou)
	struct DSAPindication *di;
	char *event;
	struct RoSAPureject *rou;
{
	char *cp;
	char buffer[BUFSIZ];

	(void) sprintf(cp = buffer, " (Reject at ROS)");

	if (rou->rou_noid)
		return (dsapreject
			(di, DA_ROS, -1, NULLCP, " no op id, reason: %d%s", rou->rou_reason, cp));
	else
		return (dsapreject
			(di, DA_ROS, rou->rou_id, NULLCP, " op id %d, reason: %d%s", rou->rou_id,
			 rou->rou_reason, cp));
}
