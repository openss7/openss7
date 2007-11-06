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
 * reqmaster.c : responder operation to transfer mastership for file
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/reqmaster.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: reqmaster.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/reqmaster.c,v 9.0 1992/06/16 12:47:25 isode Rel";
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
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <varargs.h>
#include "sys.file.h"
#include "RFA-ops.h"		/* operation definitions */
#include "RFA-types.h"		/* type definitions */
#include "ryresponder.h"	/* for generic idempotent responders */
#include "psap.h"		/* for generic idempotent responders */
#include "rfa.h"
#include "rfainfo.h"
#ifdef SYS5
#include <unistd.h>
#endif

/*--------------------------------------------------------------
 *  op_reqMaster - get mastership of a file
 *-------------------------------------------------------------*/
op_requestMaster(sd, ryo, rox, in, roi)
	int sd;
	struct RyOperation *ryo;
	struct RoSAPinvoke *rox;
	caddr_t in;
	struct RoSAPindication *roi;
{
	register struct type_RFA_RequestMasterArg *rma = (struct type_RFA_RequestMasterArg *) in;
	struct type_RFA_RequestMasterRes rmr;
	struct RfaInfo *rfalist, *rfa;
	char *s;
	time_t t;
	char buf[BUFSIZ];
	char *fn;
	int rc;

	if (rox->rox_nolinked == 0) {
		advise(LLOG_NOTICE, NULLCP,
		       "RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
		       sd, ryo->ryo_name, rox->rox_linkid);
		return ureject(sd, ROS_IP_LINKED, rox, roi);
	}
	advise(LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s", sd, ryo->ryo_name);

	if ((s = qb2str(rma->filename)) == NULL)
		return NOTOK_SYS;

    /*--- expand symlinks and get relative path ---*/
	if ((fn = expandSymLinks(s)) == NULL) {
		free(s);
		advise(LLOG_EXCEPTIONS, NULLCP, "reqMaster: %s", rfaErrStr);
		return str_error(sd, error_RFA_fileAccessError, rfaErrStr, rox, roi);
	}
	free(s);

    /*--- get rfainfo list for directory (begin of critical section) ---*/
	if ((rc = getRfaInfoList(dirname(fn), &rfalist, basename(fn), 1))
	    != OK) {
		return rc;
	}

    /*--- check if file exists and user is allowed to write it ---*/
	if ((rfa = findRfaInfo(basename(fn), rfalist)) == NULL) {
		releaseRfaInfoList(dirname(fn), rfalist);
		advise(LLOG_NOTICE, NULLCP, "reqMaster: %s not .rfainfo", fn);
		return statusError(sd, int_RFA_reason_notRegistered, NULL, 0L, rox, roi);
	}

	if (access(makeFN(fn), W_OK) == -1) {
		releaseRfaInfoList(dirname(fn), rfalist);
		advise(LLOG_NOTICE, NULLCP, "reqMaster: %s not writable", fn);
		return statusError(sd, int_RFA_reason_notWritable, NULL, 0L, rox, roi);
	}

    /*--- check if we are master of the file ---*/
	switch (RI_STATUS(rfa->ri_status)) {
	case RI_MASTER:
		if (IS_LOCKED(rfa->ri_status)) {
			releaseRfaInfoList(dirname(fn), rfalist);
			return statusError(sd, int_RFA_reason_locked,
					   rfa->ri_lckname, rfa->ri_lcksince, rox, roi);
		}
		break;
	case RI_SLAVE:
		releaseRfaInfoList(dirname(fn), rfalist);
		advise(LLOG_NOTICE, NULLCP, "reqMaster: not master for %s", fn);
		return statusError(sd, int_RFA_reason_notMaster, NULL, 0L, rox, roi);

	case RI_UNREGISTERED:
		releaseRfaInfoList(dirname(fn), rfalist);
		advise(LLOG_NOTICE, NULLCP, "reqMaster: %s not registered", fn);
		return statusError(sd, int_RFA_reason_notRegistered, NULL, 0L, rox, roi);

	default:
		releaseRfaInfoList(dirname(fn), rfalist);
		sprintf(buf, "unknown file status %d for %s", rfa->ri_status, fn);
		return str_error(sd, error_RFA_miscError, buf, rox, roi);

	}

	advise(LLOG_DEBUG, NULLCP, "reqMaster: allowing lock mv=%ld sv=%d",
	       rfa->ri_modTime, rma->slaveVersion);

    /*--- so we are master of the file, check the modify times ---*/
	if (rfa->ri_mode & S_IFMT & S_IFREG) {
		t = rma->slaveVersion;
		if (rfa->ri_modTime != t) {
			releaseRfaInfoList(dirname(fn), rfalist);
			advise(LLOG_NOTICE, NULLCP, "reqMaster: lock %s: wrong version %s",
			       fn, ctime(&t));
			return statusError(sd, int_RFA_reason_wrongVersion, NULL, 0L, rox, roi);
		}
	}

    /*--- grant mastership, update rfalist ---*/
	SET_STATUS(rfa->ri_status, RI_SLAVE);
	time(&(rfa->ri_lastChange));
	rfa->ri_lcksince = NULL;
	if (rfa->ri_lckname)
		free(rfa->ri_lckname);
	rfa->ri_lckname = NULL;
	if (putRfaInfoList(dirname(fn), rfalist) != OK) {
		releaseRfaInfoList(dirname(fn), rfalist);
		advise(LLOG_EXCEPTIONS, NULLCP, "reqMaster: could not write %s/.rfainfo",
		       dirname(fn));
		return syserror(sd, error_RFA_miscError, rox, roi);
	}
	advise(LLOG_NOTICE, NULLCP, "reqMaster: sending result");

    /*--- return result ----*/
	rmr.parm = rfa->ri_modTime;
	if (RyDsResult(sd, rox->rox_id, (caddr_t) &rmr, ROS_NOPRIO, roi) != OK) {

	/*--- result failed, so assume that peer has not become master ---*/
		advise(LLOG_NOTICE, NULLCP, "reqMaster: sending result failed");
		SET_STATUS(rfa->ri_status, RI_MASTER);
		if (putRfaInfoList(dirname(fn), rfalist) != OK)
			advise(LLOG_EXCEPTIONS, NULLCP, "reqMaster: couldn't reset MASTER for %s",
			       fn);
		releaseRfaInfoList(dirname(fn), rfalist);
		ros_adios(&roi->roi_preject, "RESULT");
	}

	if (rfa->ri_mode & S_IFMT & S_IFREG)
		if (makeFileReadOnly(fn, rfa) != OK)
			advise(LLOG_EXCEPTIONS, NULLCP, "reqMaster: %s", rfaErrStr);

	releaseRfaInfoList(dirname(fn), rfalist);
	return OK;
}
