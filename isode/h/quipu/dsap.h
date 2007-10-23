/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

#ifndef __ISODE_QUIPU_DSAP_H__
#define __ISODE_QUIPU_DSAP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* dsap.h - include file for directory service users (DS-USER) */

/* 
 * Header: /xtel/isode/isode/h/quipu/RCS/dsap.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: dsap.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
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

#ifndef	_DSAP_
#define	_DSAP_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#ifndef	_RoSAP_
#include "rosap.h"		/* definitions for ROSE-USERs */
#endif

#ifndef	_RoNot_
#include "ronot.h"		/* definitions for RoBIND-USERs */
#endif

#include "quipu/common.h"
#include "quipu/dsargument.h"

/*  */

/* An additional value for "asyn" parameters for interruptibility */
#define ROS_INTR	-1

/*  */

/* Values used to convey response type throughout DSAP library */
#define DS_RESULT	1
#define DS_ERROR	2
#define DS_REJECT	3

/*  */

struct DSAPstart {			/* D-BIND.INDICATION */
	int ds_sd;			/* association descriptor */

	int ds_ctx;			/* Directory protocol type */
#define DS_CTX_X500_DAP	1
#define DS_CTX_X500_DSP	2
#define DS_CTX_QUIPU_DSP 3		/* Quipu has its own DSP context */
#define DS_CTX_INTERNET_DSP 4		/* Internet also has its own DSP context */

	int ds_pctx_id;			/* Context identifier of directory AS */

	struct AcSAPstart ds_start;	/* info from A-CONNECT.INDICATION */

	struct ds_bind_arg ds_bind_arg;	/* Decoded bind argument */
};

#define	DSFREE(ds) { \
    ACSFREE (&(ds) -> ds_start); \
    bind_arg_free (&(ds) -> ds_bind_arg); \
}

struct DSAPconnect {
	int dc_sd;			/* association descriptor */

	int dc_ctx;			/* Directory protocol type */

	int ds_pctx_id;			/* Context identifier of directory AS */

	struct AcSAPconnect dc_connect;	/* info from A-CONNECT.CONFIRMATION */

	int dc_result;			/* result */
	/* Uses general DS response type definitiions */

	union {
		struct ds_bind_arg dc_bind_res;
		struct ds_bind_error dc_bind_err;
	} dc_un;
};

#ifndef DCFREE
#define	DCFREE(dc) { \
    ACCFREE (&(dc) -> dc_connect); \
    switch ((dc) -> dc_result) { \
	case DS_RESULT: \
	    bind_arg_free (&(dc) -> dc_un.dc_bind_res); \
	    break; \
	case DS_ERROR: \
	    break; \
	default: \
	    break; \
    } \
}
#endif

struct DSAPrelease {			/* D-UNBIND.CONFIRMATION */
	int dr_affirmative;		/* T = connection released NIL = request refused */

	int dr_reason;			/* reason for result */
	/* Takes values from acr_reason */
};

struct DSAPabort {			/* D-ABORT.INDICATION */
	int da_source;			/* abort source */
#define	DA_USER		0		/* directory-user */
#define	DA_PROVIDER	1		/* directory-provider */
#define	DA_LOCAL	2		/* local DPM */

	int da_reason;			/* reason for failure */
#define DA_NO_REASON	0		/* Ain't no reason to some things */
#define DA_RO_BIND	1		/* Error from RO BIND routine called */
#define DA_ROS		2		/* Error from ROS routine called */
#define DA_ARG_ENC 	3		/* Error encoding argument */
#define DA_RES_ENC 	4		/* Error encoding result */
#define DA_ERR_ENC 	5		/* Error encoding error */
#define DA_ARG_DEC 	6		/* Error decoding argument */
#define DA_RES_DEC	7		/* Error decoding result */
#define DA_ERR_DEC 	8		/* Error decoding error */
#define DA_APP_CONTEXT	9		/* Unexpected application context */
#define DA_PCDL		10		/* Unacceptable presentation context list */

	/* diagnostics from provider */
#define	DA_SIZE	512
	int da_cc;			/* length */
	char da_data[DA_SIZE];		/* data */
};

struct DSAPinvoke {
	int dx_id;			/* Operation identifier */
	struct ds_op_arg dx_arg;	/* Decoded argument */
};

#define DXFREE(dx) \
    op_arg_free (&((dx)->dx_arg))

struct DSAPresult {
	int dr_id;
	struct ds_op_res dr_res;	/* Decoded argument */
};

#ifndef DRFREE
#define DRFREE(dr) \
    op_res_free (&((dr)->dr_res))
#endif

struct DSAPerror {
	int de_id;
	struct DSError de_err;		/* Decoded error */
};

#ifndef DEFREE
#define DEFREE(de) \
    op_res_free (&((de)->de_err))
#endif

struct DSAPpreject {
	int dp_id;			/* Operation id or -1 */

	int dp_source;			/* same values as DSAPabort.da_source */

	int dp_reason;			/* reason for failure */
#define DP_NO_REASON	0		/* Ain't no reason to some things */
#define DP_ROS		1		/* ROSE error */
#define DP_INVOKE	2		/* Failure during invocation */
#define DP_RESULT	3		/* Failure during result */
#define DP_ERROR	4		/* Failure during error */

	/* diagnostics from provider */
#define	DP_SIZE	512
	int dp_cc;			/* length */
	char dp_data[DP_SIZE];		/* data */
};

struct DSAPfinish {
	int df_reason;			/* Reason for unbinding */
};

#define DFFREE(df)

struct DSAPindication {
	int di_type;
#define DI_INVOKE	1		/* DAP operation invocation received */
#define DI_RESULT	2		/* DAP operation result received */
#define DI_ERROR	3		/* DAP operation error received */
#define DI_PREJECT	4		/* DAP operation rejected */
#define	DI_FINISH	5		/* DAP UnBind received */
#define	DI_ABORT	6		/* DAP association lost */
	union {
		struct DSAPinvoke di_un_invoke;
		struct DSAPerror di_un_error;
/* This is used at the same time as the error struct in searches
	struct DSAPresult	di_un_result;
*/
		struct DSAPpreject di_un_preject;
		struct DSAPfinish di_un_finish;
		struct DSAPabort di_un_abort;
	} di_un;
	struct DSAPresult di_result;

#define di_invoke di_un.di_un_invoke
/*
#define di_result di_un.di_un_result
*/
#define di_error di_un.di_un_error
#define di_preject di_un.di_un_preject
#define di_finish di_un.di_un_finish
#define di_abort di_un.di_un_abort
};

#ifndef	lint
#ifndef	__STDC__
#define	copyDSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/**/_cc = min (i, sizeof d -> d/**/_data)) > 0) \
	bcopy (base, d -> d/**/_data, d -> d/**/_cc); \
}
#else
#define	copyDSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyDSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

/*  */

extern char *dsapversion;

int DBindInit();			/* D-BIND.INDICATION */

int DAsynBindRequest();			/* D-BIND.REQUEST (ARGUMENT) */
int DAsynBindRetry();			/* D-BIND-RETRY.REQUEST */

int DBindResult();			/* D-BIND.RESPONSE (RESULT) */
int DBindError();			/* D-BIND.RESPONSE (ERROR) */
int DBindReject();			/* D-BIND.RESPONSE (REJECT) */

int DUnBindRequest();			/* D-UNBIND.REQUEST */
int DUnBindRetry();			/* D-BIND-RETRY.REQUEST (pseudo) */
int DUnBindResponse();			/* D-BIND.RESPONSE (RESULT) */
int DUnBindReject();			/* D-BIND.RESPONSE (REJECT) */

char *DErrString();			/* return DSAP error code in string form */

#endif

#endif				/* __ISODE_QUIPU_DSAP_H__ */
