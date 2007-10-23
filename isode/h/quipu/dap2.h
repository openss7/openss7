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

#ifndef __ISODE_QUIPU_DAP2_H__
#define __ISODE_QUIPU_DAP2_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* dap.h - */

/* 
 * Header: /xtel/isode/isode/h/quipu/RCS/dap2.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: dap2.h,v
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

#ifndef QUIPUDAP2
#define QUIPUDAP2

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

struct DAPconnect {
	int dc_sd;			/* association descriptor */

	int ds_pctx_id;			/* Context identifier of directory access AS */

	struct AcSAPconnect dc_connect;	/* info from A-CONNECT.CONFIRMATION */

	int dc_result;			/* result */
#define DC_RESULT	1
#define DC_ERROR	2
#define DC_REJECT	3

	union {
		struct ds_bind_arg dc_bind_res;	/* DC_RESULT */
		struct ds_bind_error dc_bind_err;	/* DC_ERROR */
	} dc_un;
};

#ifndef DCFREE
#define	DCFREE(dc) { \
    ACCFREE (&((dc) -> dc_connect)); \
    switch ((dc) -> dc_result) { \
	case DC_RESULT: \
	    bind_arg_free (&(dc) -> dc_un.dc_bind_res); \
	    break; \
	case DC_ERROR: \
	    break; \
	default: \
	    break; \
    } \
}
#endif

struct DAPrelease {			/* DAP-UNBIND.CONFIRMATION */
	int dr_affirmative;		/* T = connection released NIL = request refused */

	int dr_reason;			/* reason for result */
	/* Takes values from acr_reason */
};

struct DAPabort {			/* DAP-ABORT.INDICATION */
	int da_source;			/* abort source */
#define	DA_USER		0		/* DAP-user */
#define	DA_PROVIDER	1		/* DAP-provider */
#define	DA_LOCAL	2		/* DAP interface internal error */

	int da_reason;			/* reason for failure */
#define DA_NO_REASON	0		/* Ain't no reason to some things */
#define DA_RONOT	1		/* Error from RONOT provider */
#define DA_ROS		2		/* Error from ROS provider called */
#define DA_ARG_ENC 	3		/* Error encoding argument */
#define DA_RES_ENC 	4		/* Error encoding result */
#define DA_ERR_ENC 	5		/* Error encoding error */
#define DA_ARG_DEC 	6		/* Error decoding argument */
#define DA_RES_DEC	7		/* Error decoding result */
#define DA_ERR_DEC 	8		/* Error decoding error */

	/* diagnostics from provider */
#define	DA_SIZE	512
	int da_cc;			/* length */
	char da_data[DA_SIZE];		/* data */
};

struct DAPresult {
	int dr_id;
	struct DSResult dr_res;		/* Decoded result and op type */
};

#ifndef DRFREE
#define DRFREE(dr) \
    ds_res_free (&((dr)->dr_res))
#endif

struct DAPerror {
	int de_id;
	struct DSError de_err;		/* Decoded error and error type */
};

#ifndef DEFREE
#define DEFREE(de) \
    ds_error_free (&((de)->de_err))
#endif

struct DAPpreject {
	int dp_id;			/* Operation id or -1 */

	int dp_source;			/* same values as DAPabort.da_source */

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

struct DAPindication {
	int di_type;
#define DI_RESULT	2		/* DAP operation result received */
#define DI_ERROR	3		/* DAP operation error received */
#define DI_PREJECT	4		/* DAP operation rejected */
#define	DI_ABORT	6		/* DAP association lost */
	union {
		struct DAPresult di_un_result;
		struct DAPerror di_un_error;
		struct DAPpreject di_un_preject;
		struct DAPabort di_un_abort;
	} di_un;
#define di_result di_un.di_un_result
#define di_error di_un.di_un_error
#define di_preject di_un.di_un_preject
#define di_abort di_un.di_un_abort
};

#ifndef	lint
#ifndef	__STDC__
#define	copyDAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/**/_cc = min (i, sizeof d -> d/**/_data)) > 0) \
	bcopy (base, d -> d/**/_data, d -> d/**/_cc); \
}
#else
#define	copyDAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyDAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

/*  */

int DapAsynBindReqAux();		/* DAP-BIND.REQUEST (ARGUMENT) */
int DapAsynBindRequest();		/* DAP-BIND.REQUEST (ARGUMENT) */
int DapAsynBindRetry();			/* DAP-BIND-RETRY.REQUEST */

int DapUnBindRequest();			/* DAP-UNBIND.REQUEST */
int DapUnBindRetry();			/* DAP-BIND-RETRY.REQUEST (pseudo) */

char *DapErrString();			/* return DAP error code in string form */

#endif

#endif				/* __ISODE_QUIPU_DAP2_H__ */
