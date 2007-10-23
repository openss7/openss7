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

#ifndef __ISODE_PPKT_H__
#define __ISODE_PPKT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ppkt.h - include file for presentation providers (PS-PROVIDER) */

/* 
 * Header: /xtel/isode/isode/h/RCS/ppkt.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 * RFC1085 (LPP) support contributed by the Wollongong Group, Inc.
 *
 *
 * Log: ppkt.h,v
 * Revision 9.0  1992/06/16  12:17:57  isode
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

#ifndef	_PSAP2_
#include "psap2.h"		/* definitions for PS-USERs */
#endif

#include "ssap.h"		/* definitinos for SS-USERs */

#define	psapPsig(pb, sd) \
{ \
    if ((pb = findpblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_PARAMETER, NULLCP, \
			    "invalid presentation descriptor"); \
    } \
    if (!(pb -> pb_flags & PB_CONN)) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor not connected"); \
    } \
    if (pb -> pb_flags & PB_FINN) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor finishing"); \
    } \
    if (pb -> pb_flags & PB_RELEASE) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "release in progress"); \
    } \
}

#define	psapFsig(pb, sd) \
{ \
    if ((pb = findpblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_PARAMETER, NULLCP, \
			    "invalid presentation descriptor"); \
    } \
    if (!(pb -> pb_flags & PB_CONN)) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor not connected"); \
    } \
    if (!(pb -> pb_flags & PB_FINN)) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "presentation descriptor not finishing"); \
    } \
    if (pb -> pb_flags & PB_RELEASE) { \
	(void) sigiomask (smask); \
	return psaplose (pi, PC_OPERATION, NULLCP, \
			    "release in progress"); \
    } \
}

#define toomuchP(b,n,m,p) \
{ \
    if (b == NULL) \
	n = 0; \
    else \
	if (n > m) \
	    return psaplose (pi, PC_PARAMETER, NULLCP, \
			    "too many %s user data elements", p); \
}

#ifdef __STDC__
#define	missingP(p) \
{ \
    if (p == NULL) \
	return psaplose (pi, PC_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", #p); \
}
#else
#define missingP(p) \
{ \
    if (p == NULL) \
        return psaplose (pi, PC_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", "p"); \
}
#endif

#ifndef	lint
#ifndef	__STDC__
#define	copyPSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/**/_cc = min (i, sizeof d -> d/**/_data)) > 0) \
	bcopy (base, d -> d/**/_data, d -> d/**/_cc); \
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

#define	pylose(p) \
	ppktlose (pb, pi, PC_UNRECOGNIZED, (p), NULLCP, "%s", PY_pepy)

int ppktlose(), psaplose();

#define	DFLT_ASN	"iso asn.1 abstract syntax"
#define	DFLT_ATN	BER

#if	USE_BUILTIN_OIDS
#define DFLT_ASN_OID	str2oid ("1.0.8824")
#define DFLT_ATN_OID	str2oid ("2.1.1")
#else
#define DFLT_ASN_OID	ode2oid (DFLT_ASN)
#define DFLT_ATN_OID	ode2oid (DFLT_ATN)
#endif

#define	atn_is_ok(pb,atn)	atn_is_ber ((pb), (atn))
#define	atn_is_ber(pb,atn)	(!oid_cmp (pb -> pb_ber, atn))

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
#define	PB_RELEASE	0x20		/* release in progress */
#define PB_MAGIC	0x40		/* backwards compat hack */

#ifndef	LPP
	char *pb_retry;			/* initial/final ppkt */
	char *pb_realbase;
	int pb_len;
#else
	PE pb_retry;
	PE pb_response;

	struct type_PS_SessionConnectionIdentifier *pb_reference;

	PS pb_stream;

	int pb_reliability;

	int pb_maxtries;
	int pb_tries;
#endif

	int pb_ncontext;		/* presentation context set */
	struct PSAPcontext pb_contexts[NPCTX];

	OID pb_asn;			/* default: abstract syntax name */
	OID pb_atn;			/* ..  abstract transfer name */
	int pb_dctxid;			/* ..  id */
	int pb_result;			/* response */

	OID pb_ber;			/* BER */

	int pb_prequirements;		/* presentation requirements */

	int pb_srequirements;		/* our session requirements */
	int pb_urequirements;		/* user's session requirements */
	int pb_owned;			/* session tokens we own */
	int pb_avail;			/* session tokens available */
	int pb_ssdusize;		/* largest atomic SSDU */

#ifdef	LPP
	struct NSAPaddr pb_initiating;	/* initiator */
#endif
	struct PSAPaddr pb_responding;	/* responder */

	IFP pb_DataIndication;		/* INDICATION handlers */
	IFP pb_TokenIndication;		/* .. */
	IFP pb_SyncIndication;		/* .. */
	IFP pb_ActivityIndication;	/* .. */
	IFP pb_ReportIndication;	/* .. */
	IFP pb_ReleaseIndication;	/* .. */
	IFP pb_AbortIndication;		/* .. */

#ifdef	LPP
	IFP pb_retryfnx;
	IFP pb_closefnx;
	IFP pb_selectfnx;
	IFP pb_checkfnx;
#endif
};

#define	NULLPB		((struct psapblk *) 0)

int freepblk();
struct psapblk *newpblk(), *findpblk();

#ifndef	LPP
#define	PC_PROV_BASE		PC_NOTSPECIFIED
#define	PC_ABORT_BASE \
	(PC_UNRECOGNIZED - int_PS_Abort__reason_unrecognized__ppdu)
#define	PC_REASON_BASE \
	(PC_ABSTRACT - int_PS_provider__reason_abstract__syntax__not__supported)

struct type_PS_User__data *info2ppdu();
int ppdu2info();

int info2ssdu(), ssdu2info(), qbuf2info();

struct qbuf *info2qb();
int qb2info();

struct type_PS_Identifier__list *silly_list();

int ss2pslose(), ss2psabort();

struct pair {
	int p_mask;
	int p_bitno;
};

extern struct pair preq_pairs[], sreq_pairs[];
#endif

#define	REASON_BASE	PC_NOTSPECIFIED

#ifndef	LPP
#define	PPDU_NONE	(-1)
#define	PPDU_CP		0
#define	PPDU_CPA	1
#define	PPDU_CPR	2
#define	PPDU_ARU	3
#define	PPDU_ARP	4
#define	PPDU_TD		7
#define	PPDU_RS	       12
#define	PPDU_RSA       13
#else
#define	PR_KERNEL	0x0000	/* kernel (yuk) */

#define	NPCTX_PS	2	/* maximum number of contexts */
#define	NPDATA_PS	1	/* maximum number of PDVs in a request */
#define	PCI_ROSE	1	/* PCI for SASE using ROSE */
#define	PCI_ACSE	3	/* PCI for ACSE */

#define	PT_TCP		'T'	/* TCP providing backing */
#define	PT_UDP		'U'	/* UDP providing backing */

#define	NULLRF	((struct type_PS_SessionConnectionIdentifier *) 0)

#define	pslose(pi,reason) \
    (reason != PS_ERR_NONE && reason != PS_ERR_IO \
	? psaplose ((pi), PC_CONGEST, NULLCP, "%s", ps_error (reason)) \
	: psaplose ((pi), PC_SESSION, NULLCP, NULLCP))

int pdu2sel(), refcmp();
struct SSAPref *pdu2ref();
#endif

#endif				/* __ISODE_PPKT_H__ */
