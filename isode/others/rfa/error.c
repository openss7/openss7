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
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * error.c : functions for the various error types
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/error.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: error.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/error.c,v 9.0 1992/06/16 12:47:25 isode Rel";
#endif

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <varargs.h>
#include <errno.h>
#include "RFA-ops.h"		/* operation definitions */
#include "RFA-types.h"		/* type definitions */
#include "ryresponder.h"	/* for generic idempotent responders */
#include "psap.h"
#include "rfa.h"

/*--------------------------------------------------------------*/
/*  aux_error							*/
/*--------------------------------------------------------------*/
int
aux_error(sd, err, param, rox, roi)
	int sd, err;
	caddr_t param;
	struct RoSAPinvoke *rox;
	struct RoSAPindication *roi;
{
	if (RyDsError(sd, rox->rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
		ros_adios(&roi->roi_preject, "ERROR");
	return OK;
}

/*--------------------------------------------------------------*/
/*  str_error							*/
/*--------------------------------------------------------------*/
int
str_error(sd, err, str, rox, roi)
	int sd, err;
	char *str;
	struct RoSAPinvoke *rox;
	struct RoSAPindication *roi;
{
	struct type_RFA_Reason *qb;
	int r;

	qb = str2qb(str, strlen(str), 1);

	r = aux_error(sd, err, (caddr_t) qb, rox, roi);

	qb_free(qb);
	return r;
}

/*--------------------------------------------------------------*/
/*  syserror							*/
/*--------------------------------------------------------------*/
int
syserror(sd, err, rox, roi)
	int sd, err;
	struct RoSAPinvoke *rox;
	struct RoSAPindication *roi;
{
	return str_error(sd, err, sys_errname(errno), rox, roi);
}

/*--------------------------------------------------------------*/
/*  error							*/
/*--------------------------------------------------------------*/
int
error(sd, err, type, rox, roi)
	int sd, err, type;
	struct RoSAPinvoke *rox;
	struct RoSAPindication *roi;
{
	if (type == NOTOK)
		return str_error(sd, err, rfaErrStr, rox, roi);
	else
		return syserror(sd, err, rox, roi);
}

/*--------------------------------------------------------------*/
/*  errMsg							*/
/*--------------------------------------------------------------*/
char *
errMsg(type)
	int type;
{
	if (type == NOTOK)
		return rfaErrStr;
	else
		return sys_errname(errno);
}

/*--------------------------------------------------------------*/
/*  statusError							*/
/*--------------------------------------------------------------*/
int
statusError(sd, reason, user, since, rox, roi)
	int sd;
	int reason;
	char *user;
	long since;
	struct RoSAPinvoke *rox;
	struct RoSAPindication *roi;
{
	struct type_RFA_StatusErrorParm se, *sep = &se;

	if ((sep->reason = reason) == int_RFA_reason_locked) {
		sep->user = str2qb(user, strlen(user), 1);
		sep->since = (int) since;
	} else {
		sep->user = NULL;
		sep->since = 0;
	}

	return aux_error(sd, error_RFA_statusError, (caddr_t) sep, rox, roi);
}
