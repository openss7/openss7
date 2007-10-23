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

#ifndef __CL_H_PUPKT_H__
#define __CL_H_PUPKT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* HULA */
/* pupkt.h - include file for Unit Data presentation providers (PS-PROVIDER) */
/* subject to ISODE license agreement */

#ifndef	_PUSAP2_
#include "pusap2.h"		/* definitions for PS-USERs */
#endif

#include "ssap.h"		/* definitinos for SS-USERs */
#include "susap.h"		/* definitinos for SS-USERs */

#define toomuchP(b,n,m,p) \
{ \
    if (b == NULL) \
	n = 0; \
    else \
	if (n > m) \
	    return pusaplose (pi, PC_PARAMETER, NULLCP, \
			    "too many %s user data elements", p); \
}

#define	missingP(p) \
{ \
    if (p == NULL) \
	return pusaplose (pi, PC_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}

#ifndef	lint
#ifndef	__STDC__
#define	copyPSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/* */_cc = min (i, sizeof d -> d/* */_data)) > 0) \
	bcopy (base, d -> d/* */_data, d -> d/* */_cc); \
}
#else
#define	copyPSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyPSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

int pusaplose();

#define	DFLT_ASN	"iso asn.1 abstract syntax"
#define	DFLT_ATN	"iso asn.1 abstract transfer"

#define	atn_is_ok(pb,atn)	((atn) ? 1 : 0)	/* it's all ISO8825, right? */
#define	atn_is_asn1(atn)	((atn) ? 1 : 0)	/* .. */

struct psapblk {
	struct psapblk *pb_forw;	/* doubly-linked list */
	struct psapblk *pb_back;	/* .. */

	int pb_fd;			/* session descriptor */

	short pb_flags;			/* our state */
#define	PB_NULL		0x00
#define	PB_CONN		0x01		/* connected */
#define	PB_FINN		0x02		/* other side wants to finish */
#define	PB_ASYN		0x04		/* asynchronous */
#define	PB_DFLT		0x10		/* respond with default context result */

#define PB_PUDT         0x20		/* p unit data service */

	struct PSAPaddr *pb_calling;	/* local user address */
	struct PSAPaddr *pb_called;	/* remote user address */

	char *pb_retry;			/* initial ppkt */

	int pb_ncontext;		/* presentation context set */
	struct PSAPcontext pb_contexts[NPCTX];

	OID pb_asn;			/* default: abstract syntax name */
	OID pb_atn;			/* ..  abstract transfer name */
	int pb_result;			/* response */

	int pb_ssdusize;		/* largest atomic SSDU */

	struct PSAPaddr pb_responding;	/* responder */

	IFP pb_DataIndication;		/* INDICATION handlers */
	IFP pb_TokenIndication;		/* .. */
	IFP pb_SyncIndication;		/* .. */
	IFP pb_ActivityIndication;	/* .. */
	IFP pb_ReportIndication;	/* .. */
	IFP pb_ReleaseIndication;	/* .. */
	IFP pb_AbortIndication;		/* .. */

};

#define	NULLPB		((struct psapblk *) 0)

int freepublk();
struct psapblk *newpublk(), *findpublk();

#define	PC_PROV_BASE		PC_NOTSPECIFIED
#define	PC_ABORT_BASE \
	(PC_UNRECOGNIZED - int_PS_Abort__reason_unrecognized__ppdu)
#define	PC_REASON_BASE \
	(PC_ABSTRACT - int_PS_provider__reason_abstract__syntax__not__supported)

int ppdu2data();
struct qbuf *info2_qb();
int qb2_info();
struct type_PS_User__data *info2_ppdu();

int print_PS_UD__type(), print_PS_User__data(), print_PS_Fully__encoded__data();
int vunknown();

#define	REASON_BASE	PC_NOTSPECIFIED
#define	PPDU_UD        14

#endif				/* __CL_H_PUPKT_H__ */
