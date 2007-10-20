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
 * $Header: /xtel/isode/isode/others/rfa/RCS/fileinfo.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: fileinfo.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/fileinfo.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
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
#include "RFA-ops.h"        /* operation definitions */
#include "RFA-types.h"  /* type definitions */
#include "ryresponder.h" 
#include "psap.h" 
#include "rfa.h"
#include "rfainfo.h"

/*--------------------------------------------------------------
 *  op_listDir - get list of fileinfos for directory
 *-------------------------------------------------------------*/
int  op_listDir (sd, ryo, rox, in, roi)
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

    if (rox -> rox_nolinked == 0) {
	advise (LLOG_NOTICE, NULLCP,
	    "RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
	    sd, ryo -> ryo_name, rox -> rox_linkid);
	return ureject (sd, ROS_IP_LINKED, rox, roi);
    }
    advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
	    sd, ryo -> ryo_name);

    s = qb2str(arg);

    /*--- expand symlinks and get relative path ---*/
    if ((dir = expandSymLinks(s)) == NULL) {
	free(s);
	advise(LLOG_EXCEPTIONS,NULLCP,"getFileInfo: %s", rfaErrStr);
	return str_error(sd, error_RFA_fileAccessError, rfaErrStr, rox, roi);
    }
    free(s);

    if ((rc = getRfaInfoList(dir, &rfalist, NULL, 0)) != OK) 
	return error(sd, error_RFA_fileAccessError, rc, rox, roi);

    /*--- convert to FileInfoList ---*/
    if ((fi = rfa2fil(dir, rfalist)) == NULL) 
	return syserror(sd, error_RFA_miscError, rox, roi);

    /*--- return result ----*/
    if (RyDsResult (sd, rox->rox_id, (caddr_t) fi, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "RESULT");
    free_RFA_FileInfoList(fi);
    
    return OK;
}

