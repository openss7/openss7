/* susap.h - include file for session unitdata users (connectionless service) */

/* 
 *
 * Kurt Dobbins		3/89
 *
 * Added UNITDATA for HULA connectionless service.
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


#ifndef	_SuSAP_
#define	_SuSAP_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif
#ifndef	_ISOADDRS_
#include "isoaddrs.h"
#endif



#ifdef HULA

struct SuSAPstart 
    {				/* S-CONNECT.INDICATION */
    int	    ss_sd;		/* SESSION descriptor */

    struct SSAPaddr ss_calling;	/* address of peer calling */
    struct SSAPaddr ss_called;	/* address of peer called */

    int	    ss_ssdusize;	/* largest atomic SSDU */
    int	    ss_version;	    	/* session service version number */

    struct QOStype ss_qos;	/* quality of service */

				/* UNITDATA from peer */

    int	    ss_cc;		/*   length */
    char   *ss_data;		/*   data */

    char   *ss_base;		/*   base ptr for free */

    };

#define	SUSFREE(ss) \
{ \
    if ((ss) -> ss_base) \
	free ((ss) -> ss_base), (ss) -> ss_data = (ss) -> ss_base = NULL; \
}

#endif 


#ifdef HULA
int	SUnitDataBind();	/* bind local to a remote address */
int	SUnitDataUnbind();	/* unbind local from a remote address */
int	SUnitDataSetupRead();	/* set up read data for server */
int	SUnitDataWrite();	/* UNITDATA.request (with handle) */
int	SUnitDataWriteV();	/* UNITDATA.request (for iovec) */
int	SUnitDataRead();	/* UNITDATA.indication (with handle) */
int	SUnitDataRequest();	/* UNITDATA.request (without handle) */
int	SUnitDataSelectMask();	/* set the select mask for async */
int	SuSave();		/* save unitdata buffer */
#endif


#endif
