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
 * fileinfo.c : responder part of GetFileInfo operation
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/fileinfo.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: fileinfo.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/fileinfo.c,v 9.0 1992/06/16 12:47:25 isode Rel";
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
#include <dirent.h>
#include <errno.h>
#include "RFA-ops.h"		/* operation definitions */
#include "RFA-types.h"		/* type definitions */
#include "ryresponder.h"
#include "psap.h"
#include "rfa.h"
#include "rfainfo.h"

/*--------------------------------------------------------------
 *  op_listDir - get list of fileinfos for directory
 *-------------------------------------------------------------*/
int
op_listDir(sd, ryo, rox, in, roi)
	int sd;
	struct RyOperation *ryo;
	struct RoSAPinvoke *rox;
	caddr_t in;
	struct RoSAPindication *roi;
{
	register struct type_RFA_FileName *arg = (struct type_RFA_FileName *) in;
	struct type_RFA_FileInfoList *fi;
	char *dir, *s;
	struct RfaInfo *rfalist;
	int rc;

	if (rox->rox_nolinked == 0) {
		advise(LLOG_NOTICE, NULLCP,
		       "RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
		       sd, ryo->ryo_name, rox->rox_linkid);
		return ureject(sd, ROS_IP_LINKED, rox, roi);
	}
	advise(LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s", sd, ryo->ryo_name);

	s = qb2str(arg);

    /*--- expand symlinks and get relative path ---*/
	if ((dir = expandSymLinks(s)) == NULL) {
		free(s);
		advise(LLOG_EXCEPTIONS, NULLCP, "getFileInfo: %s", rfaErrStr);
		return str_error(sd, error_RFA_fileAccessError, rfaErrStr, rox, roi);
	}
	free(s);

	if ((rc = getRfaInfoList(dir, &rfalist, NULL, 0)) != OK)
		return error(sd, error_RFA_fileAccessError, rc, rox, roi);

    /*--- convert to FileInfoList ---*/
	if ((fi = rfa2fil(dir, rfalist)) == NULL)
		return syserror(sd, error_RFA_miscError, rox, roi);

    /*--- return result ----*/
	if (RyDsResult(sd, rox->rox_id, (caddr_t) fi, ROS_NOPRIO, roi) == NOTOK)
		ros_adios(&roi->roi_preject, "RESULT");
	free_RFA_FileInfoList(fi);

	return OK;
}
