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

...static int
op_name(sd, ryo, rox, in, roi)
	int sd;
	struct RyOperation *ryo;
	struct RoSAPinvoke *rox;
	caddr_t in;
	struct RoSAPindication *roi;
{
	int result, won;
	register struct type_MODULE_argument *arg = (struct type_MODULE_argument *) in;

/* this check should *not* be made if linking of operations is expected */
	if (rox->rox_nolinked == 0) {
		advise(LLOG_EXCEPTIONS, NULLCP,
		       "RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
		       sd, ryo->ryo_name, rox->rox_linkid);
		return ureject(sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise(LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s", sd, ryo->ryo_name);

/* do something with ``arg'' here and decide if won... */

	if (won) {
		register struct type_MODULE_result *res;

/* allocate and initialize the result ``res'' here... */

		if (RyDsResult(sd, rox->rox_id, (caddr_t) res, ROS_NOPRIO, roi)
		    == NOTOK)
			ros_adios(&roi->roi_preject, "RESULT");
		result = OK;

		free_MODULE_result(res);
	} else {
		struct type_MODULE_parameter *parm;

/* allocate and initialize ``parm'' here... */

		result = error(sd, error_MODULE_error1, (caddr_t) parm, rox, roi);

		free_MODULE_parameter(parm);
	}

/* ``arg'' will be automatically free'd when op_name returns */

	return result;
}

...static int
error(sd, err, param, rox, roi)
	int sd, err;
	caddr_t param;
	struct RoSAPinvoke *rox;
	struct RoSAPindication *roi;
{
	if (RyDsError(sd, rox->rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
		ros_adios(&roi->roi_preject, "ERROR");

	return OK;
}

static int
ureject(sd, reason, rox, roi)
	int sd, reason;
	struct RoSAPinvoke *rox;
	struct RoSAPindication *roi;
{
	if (RyDsUReject(sd, rox->rox_id, reason, ROS_NOPRIO, roi) == NOTOK)
		ros_adios(&roi->roi_preject, "U-REJECT");

	return OK;
}

...
