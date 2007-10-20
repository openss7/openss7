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
 * $Header: /xtel/isode/isode/others/rfa/RCS/error.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: error.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/error.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
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
#include "RFA-ops.h"        /* operation definitions */
#include "RFA-types.h"  /* type definitions */
#include "ryresponder.h"    /* for generic idempotent responders */
#include "psap.h" 
#include "rfa.h"

/*--------------------------------------------------------------*/
/*  aux_error							*/
/*--------------------------------------------------------------*/
int  aux_error (sd, err, param, rox, roi)
int sd,
    err;
caddr_t param;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    if (RyDsError (sd, rox -> rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "ERROR");
    return OK;
}


/*--------------------------------------------------------------*/
/*  str_error							*/
/*--------------------------------------------------------------*/
int str_error (sd, err, str, rox, roi)
int sd, err;
char	*str;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    struct type_RFA_Reason *qb;
    int r;
    
    qb= str2qb(str, strlen(str), 1);

    r = aux_error (sd, err, (caddr_t)qb, rox, roi);

    qb_free(qb);
    return r;
}


/*--------------------------------------------------------------*/
/*  syserror							*/
/*--------------------------------------------------------------*/
int syserror (sd, err, rox, roi)
int sd, err;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    return str_error (sd, err, sys_errname (errno), rox, roi); 
} 


/*--------------------------------------------------------------*/
/*  error							*/
/*--------------------------------------------------------------*/
int error(sd, err, type, rox, roi) 
    int sd, err, type; 
    struct RoSAPinvoke *rox; 
    struct RoSAPindication *roi; 
{ 
    if(type == NOTOK) 
	return str_error(sd, err, rfaErrStr, rox, roi);
    else
	return syserror(sd, err, rox, roi);
}


/*--------------------------------------------------------------*/
/*  errMsg							*/
/*--------------------------------------------------------------*/
char *errMsg(type)
    int type;
{
    if(type == NOTOK) 
	return rfaErrStr;
    else
	return sys_errname(errno);
}

    

/*--------------------------------------------------------------*/
/*  statusError							*/
/*--------------------------------------------------------------*/
int statusError (sd, reason, user, since, rox, roi)
int sd;
int reason;
char *user;
long since;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    struct type_RFA_StatusErrorParm se, *sep = & se;

    if((sep->reason = reason) == int_RFA_reason_locked) {
	sep->user = str2qb(user, strlen(user), 1);
	sep->since = (int)since;
    } else {
	sep->user = NULL;
	sep->since = 0;
    }

    return aux_error (sd, error_RFA_statusError, (caddr_t)sep, rox, roi);
}

