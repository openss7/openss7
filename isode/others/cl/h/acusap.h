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

#ifndef __CL_H_ACUSAP_H__
#define __CL_H_ACUSAP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* acusap.h - include file for A-UNIT-DATA association control users */
/* header from acsap.h left intact */

/* 
 * Header: /f/iso/h/RCS/acsap.h,v 5.0 88/07/21 14:38:46 mrose Rel
 *
 *
 * Log
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

#ifndef	_AcuSAP_
#define	_AcuSAP_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#include "pusap2.h"		/* definitions for PS-USERs */

#define	NACDATA		3	/* arbitrary */

struct AcuSAPstart {			/* A-UNIT-DATA.INDICATION */
	int acs_sd;			/* association descriptor */
	int acs_result;			/* result */
#define	ACS_ACCEPT	0		/* Accepted */
#define	ACS_REJECT	(-1)		/* Release rejected */
	/* Rejected by responder: */
#define	ACS_PERMANENT	1		/* Permanent */
#define	ACS_TRANSIENT	2		/* Transient */

	int acs_diagnostic;		/* source-diagnostic */
/*  needs updating for strictly connectionsless */
	/* service-user */
#define	ACS_USER_NULL	3		/* null */
#define	ACS_USER_NOREASON 4		/* no reason given */
#define	ACS_CONTEXT	5		/* application context name not supported */
#define	ACS_CALLING_AP_TITLE 6		/* calling AP title not recognized */
#define	ACS_CALLING_AP_ID 7		/* calling AP invocation-ID not recognized */
#define	ACS_CALLING_AE_QUAL 8		/* calling AE qualifier not recognized */
#define	ACS_CALLING_AE_ID 9		/* calling AE invocation-ID not recognized */
#define	ACS_CALLED_AP_TITLE 10		/* called AP title not recognized */
#define	ACS_CALLED_AP_ID 11		/* called AP invocation-ID not recognized */
#define	ACS_CALLED_AE_QUAL 12		/* called AE qualifier not recognized */
#define	ACS_CALLED_AE_ID 13		/* called AE invocation-ID not recognized */
	/* service-provider */
#define	ACS_PROV_NULL	14		/* null */
#define	ACS_PROV_NOREASON 15		/* no reason given */
#define	ACS_VERSION	16		/* no common acse version */

	/* begin UNOFFICIAL */
#define	ACS_ADDRESS	17		/* Address unknown */
#define	ACS_REFUSED	18		/* Connect request refused on this network connection */
#define	ACS_CONGEST	19		/* Local limit exceeded */
#define	ACS_PRESENTATION 20		/* Presentation disconnect */
#define	ACS_PROTOCOL	21		/* Protocol error */
#define	ACS_ABORTED	22		/* Peer aborted association */
#define	ACS_PARAMETER	23		/* Invalid parameter */
#define	ACS_OPERATION	24		/* Invalid operation */
	/* end UNOFFICIAL */

#define	ACS_FATAL(r)	((r) < ACS_PARAMETER)
#define	ACS_OFFICIAL(r)	((r) < ACS_ADDRESS)

	OID acs_context;		/* application context name */

	AEInfo acs_callingtitle;	/* info on calling application-entity */
	AEInfo acs_calledtitle;		/* info on called application-entity */

	struct PuSAPstart acs_start;	/* info from P-CONNECT.INDICATION */

	/* initial information from peer */
	int acs_ninfo;			/* number of elements */
	PE acs_info[NACDATA];		/* data */
};

#define	ACSFREE(acs) { \
    register int ACSI; \
 \
    if ((acs) -> acs_context) \
	oid_free ((acs) -> acs_context), (acs) -> acs_context = NULLOID; \
 \
    AEIFREE (&(acs) -> acs_callingtitle); \
    AEIFREE (&(acs) -> acs_calledtitle); \
 \
    for (ACSI = (acs) -> acs_ninfo - 1; ACSI >= 0; ACSI--) \
	if ((acs) -> acs_info[ACSI]) \
	    pe_free ((acs) -> acs_info[ACSI]), \
		(acs) -> acs_info[ACSI] = NULLPE; \
    (acs) -> acs_ninfo = 0; \
}

struct AcSAPabort {			/* A-{U,P}-ABORT.INDICATION */
	int aca_source;			/* abort source */
#define	ACA_USER	0		/* service-user */
#define	ACA_PROVIDER	1		/* service-provider */
#define	ACA_LOCAL	2		/* local ACPM (UNOFFICIAL) */

	int aca_reason;			/* same codes as acc_result */

	/* abort information from peer */
	int aca_ninfo;			/* number of elements */
	PE aca_info[NACDATA];		/* data */

	/* diagnostics from provider */
#define	ACA_SIZE	512
	int aca_cc;			/* length */
	char aca_data[ACA_SIZE];	/* data */
};

#define	ACAFREE(aca) \
{ \
    register int ACAI; \
 \
    for (ACAI = (aca) -> aca_ninfo - 1; ACAI >= 0; ACAI--) \
	if ((aca) -> aca_info[ACAI]) \
	    pe_free ((aca) -> aca_info[ACAI]), \
		(aca) -> aca_info[ACAI] = NULLPE; \
    (aca) -> aca_ninfo = 0; \
}

struct AcSAPindication {
	int aci_type;			/* union for compatability with ACSE */
#define	ACI_ABORT	0x01
	union {
		struct AcSAPabort aci_un_abort;
	} aci_un;
#define	aci_abort	aci_un.aci_un_abort
};

extern char *acsapversion;
char *AcuErrString();			/* return AcuSAP error in string form */
int AcFindPCI();			/* return PCI used by ACSE */

int AcUnitDataRequest();		/* A-UNIT-DATA.REQUEST */
int AcUnitDataBind();			/* set A-UNIT-DATA binding */
int AcUnitDataRebind();			/* reset A-UNIT-DATA binding remote addr */
int AcUnitDataWrite();			/* A-UNIT-DATA.REQUEST on binding */
int AcUnitDataRead();			/* A-UNIT-DATA.INDICATION on binding */
int AcUnitDataUnbind();			/* release A-UNIT-DATA binding */
int AcuSave();				/* save TPDU for AUDT indication */

#endif

#endif				/* __CL_H_ACUSAP_H__ */
