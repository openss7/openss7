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

#ifndef __CL_H_SUSAP_H__
#define __CL_H_SUSAP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

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

struct SuSAPstart {			/* S-CONNECT.INDICATION */
	int ss_sd;			/* SESSION descriptor */

	struct SSAPaddr ss_calling;	/* address of peer calling */
	struct SSAPaddr ss_called;	/* address of peer called */

	int ss_ssdusize;		/* largest atomic SSDU */
	int ss_version;			/* session service version number */

	struct QOStype ss_qos;		/* quality of service */

	/* UNITDATA from peer */

	int ss_cc;			/* length */
	char *ss_data;			/* data */

	char *ss_base;			/* base ptr for free */

};

#define	SUSFREE(ss) \
{ \
    if ((ss) -> ss_base) \
	free ((ss) -> ss_base), (ss) -> ss_data = (ss) -> ss_base = NULL; \
}

#endif

#ifdef HULA
int SUnitDataBind();			/* bind local to a remote address */
int SUnitDataUnbind();			/* unbind local from a remote address */
int SUnitDataSetupRead();		/* set up read data for server */
int SUnitDataWrite();			/* UNITDATA.request (with handle) */
int SUnitDataWriteV();			/* UNITDATA.request (for iovec) */
int SUnitDataRead();			/* UNITDATA.indication (with handle) */
int SUnitDataRequest();			/* UNITDATA.request (without handle) */
int SUnitDataSelectMask();		/* set the select mask for async */
int SuSave();				/* save unitdata buffer */
#endif

#endif

#endif				/* __CL_H_SUSAP_H__ */
