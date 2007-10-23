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

#ifndef __CL_H_ACUPKT_H__
#define __CL_H_ACUPKT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* HULA */
/* acupkt.h - include file for A-UNIT-DATA ACSE providers (AcS-PROVIDER) */
/* module is subject to ISODE license agreement */

#ifdef   ACSE

#ifndef	_AcuSAP_
#include "acusap.h"		/* definitions for AcS-USERs */
#endif

#ifndef	_PuSAP2__
#include "pusap2.h"		/* definitions for PS-USERs */
#endif

#define	AC_ASN		"acse pci version 1"

#define	toomuchP(b,n,m,p) \
{ \
    if (b == NULL) \
	n = 0; \
    else \
	if (n > m) \
	    return acusaplose (aci, ACS_PARAMETER, NULLCP, \
				"too many %s user data elements", p); \
}

#define	missingP(p) \
{ \
    if (p == NULL) \
	return acusaplose (aci, ACS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}

#endif				/* ifdef ACSE */

#ifndef	lint
#ifndef	__STDC__
#define	copyAcSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/* */_cc = min (i, sizeof d -> d/* */_data)) > 0) \
	bcopy (base, d -> d/* */_data, d -> d/* */_cc); \
}
#else
#define	copyAcSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyAcSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

#define	ACS_USER_BASE	ACS_USER_NULL
#define	ACS_PROV_BASE	ACS_PROV_NULL

int acusaplose();
int ps2aculose();

struct type_ACS_Association__information *info2_apdu();
int apdu2_info();

struct assocblk {
	struct assocblk *acb_forw;	/* doubly-linked list */
	struct assocblk *acb_back;	/* .. */

	int acb_fd;			/* session/presentation descriptor */

	short acb_flags;		/* our state */
/*  state flags match connection oriented control block */
/*  necessary for ROS */
#define	ACB_NULL	0x0000
#define	ACB_CONN	0x0001		/* connected */
#define	ACB_FINN	0x0002		/* other side wants to finish */
#define	ACB_INIT	0x0004		/* this side initiated the association */
#define	ACB_ASYN	0x0008		/* asynchronous */
#define	ACB_TURN	0x0010		/* we have the turn */
#define	ACB_TWA		0x0020		/* two-way alternate */
#define	ACB_ACT		0x0040		/* activity in progress */
#define	ACB_PLEASE	0x0080		/* RTS: RT-TURN-PLEASE received ROS on Session: sent
					   S-PLEASE-TOKENS */
#define	ACB_TIMER	0x0100		/* ACTIVITY discarded due to timing constraint */
#define	ACB_ROS		0x0200		/* ROS started association */
#define	ACB_RTS		0x0400		/* RTS .. */
#define	ACB_ACS		0x0800		/* ACS .. */
#define	ACB_CLOSING	0x1000		/* waiting to close */
#define	ACB_FINISH	0x2000		/* .. */

#define ACB_AUDT	0x8000		/* ROS maps to A.UNIT.DATA */

	int acb_ssdusize;		/* largest atomic SSDU */
	IFP acb_uabort;			/* disconnect underlying service */

/* ACSE */
	int acb_sversion;		/* session service version number */
	int acb_id;			/* ACSE context id */
	OID acb_context;		/* application context name */
	int acb_offset;			/* offset to ACSE PCI */
	/* negative means at END */
	OID acb_audtpci;		/* identifier for AUDT PCI */
	AEI acb_callingtitle;		/* local user title */
	AEI acb_calledtitle;		/* remote user title */
	int acb_binding;		/* static bind vs dynamic reconnect */
#define BIND_STATIC    0		/* fixed called addr */
#define BIND_DYNAMIC   1		/* reconnect called to last calling addr */

/* ROSE */
	int acb_rosid;			/* ROSE (SASE) context id */
	IFP acb_putosdu;		/* osdu2acb */
	IFP acb_rowaitrequest;		/* RO-WAIT.REQUEST */
	IFP acb_ready;			/* get HDX permission */
	IFP acb_rosetindications;	/* define vectors for INDICATION events */
	IFP acb_roselectmask;		/* map association descriptors for select () */
	IFP acb_ropktlose;		/* protocol-level abort */
	 PE(*acb_getosdu) ();		/* for users of THORN... */

	PE acb_apdu;			/* APDU buffered */

	IFP acb_rosindication;		/* ros event handler */
};

#define	NULLACB		((struct assocblk *) 0)
int freeacublk();

#endif				/* __CL_H_ACUPKT_H__ */
