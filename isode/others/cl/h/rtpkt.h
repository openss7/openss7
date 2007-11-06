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

#ifndef __CL_H_RTPKT_H__
#define __CL_H_RTPKT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* rtpkt.h - include file for reliable transfer providers (RtS-PROVIDER) */

/* 
 * Header: /f/iso/h/RCS/rtpkt.h,v 5.0 88/07/21 14:39:28 mrose Rel
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

#ifndef	_RtSAP_
#include "rtsap.h"		/* definitions for RtS-USERs */
#endif

#include "acpkt.h"		/* definitions for AcS-PROVIDERs */

#ifndef	_SSAP_
#include "ssap.h"		/* definitions for SS-USERs */
#endif

#define	rtsapPsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_RTS)) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "not an association descriptor for RTS"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (acb -> acb_flags & ACB_FINN) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor finishing"); \
    } \
}

#define	rtsapFsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_RTS)) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "not an association descriptor for RTS"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (!(acb -> acb_flags & ACB_FINN)) { \
	(void) sigsetmask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor not finishing"); \
    } \
}

#define	missingP(p) \
{ \
    if (p == NULL) \
	return rtsaplose (rti, RTS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}

#ifndef	lint
#ifndef	__STDC__
#define	copyRtSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/* */_cc = min (i, sizeof d -> d/* */_data)) > 0) \
	bcopy (base, d -> d/* */_data, d -> d/* */_cc); \
}
#else
#define	copyRtSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyRtSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

#define	pylose() \
	rtpktlose (acb, rti, RTS_PROTOCOL, NULLCP, "%s", PY_pepy)

/* would really prefer to determine DEFAULT_CKPOINT dynamically, but can't
   since need to know it *before* doing the A-ASSOCIATE.REQUEST or
   S-CONNECT.REQUEST... */

#define	DEFAULT_CKPOINT	(65518 >> 10)
#define	DEFAULT_WINDOW	PCONN_WD_DFLT

#define	RTS_MYREQUIRE	(SR_EXCEPTIONS | SR_ACTIVITY | SR_HALFDUPLEX \
			    | SR_MINORSYNC)

#define	RT_ASN		"rtse pci version 1"

int rtpktlose(), rtsaplose();

#define	SetPS2RtService(acb) \
{ \
    (acb) -> acb_pturnrequest = rt2pspturn; \
    (acb) -> acb_gturnrequest = rt2psgturn; \
    (acb) -> acb_transferequest = rt2pstrans; \
    (acb) -> acb_rtwaitrequest = rt2pswait; \
    (acb) -> acb_rtsetindications = rt2psasync; \
    (acb) -> acb_rtselectmask = rt2psmask; \
    (acb) -> acb_rtpktlose = rt2pslose; \
}

int acs2rtslose(), acs2rtsabort(), ps2rtslose();
int rt2pspturn(), rt2psgturn(), rt2pstrans(), rt2pswait(), rt2psasync(), rt2psmask(), rt2pslose();

#define	SetSS2RtService(acb) \
{ \
    (acb) -> acb_pturnrequest = rt2sspturn; \
    (acb) -> acb_gturnrequest = rt2ssgturn; \
    (acb) -> acb_transferequest = rt2sstrans; \
    (acb) -> acb_rtwaitrequest = rt2sswait; \
    (acb) -> acb_rtsetindications = rt2ssasync; \
    (acb) -> acb_rtselectmask = rt2ssmask; \
    (acb) -> acb_rtpktlose = rt2sslose; \
}

int ss2rtslose(), ss2rtsabort();
int rt2sspturn(), rt2ssgturn(), rt2sstrans(), rt2sswait(), rt2ssasync(), rt2ssmask(), rt2sslose();

				/* RTORQ apdu */
#define	RTORQ_CKPOINT	0	/* checkpointSize tag */
#define	  RTORQ_CK_DFLT	0	/* default */
#define	RTORQ_WINDOW	1	/* windowSize tag */
#define	  RTORQ_WD_DFLT	3	/* default */
#define	RTORQ_DIALOGUE	2	/* dialogueMode tag */
#define	  RTORQ_DM_MONO 0	/* monologue */
#define	  RTORQ_DM_TWA	1	/* two-way alternate */
#define	  RTORQ_DM_DFLT	RTORQ_DM_MONO
#define	RTORQ_CONNDATA	3	/* connectionDataRQ tag */
#define	  RTORQ_CD_OPEN 0	/* open tag */
#define	  RTORQ_CD_RCVR	1	/* recover tag */

				/* RTOAC apdu */
#define	RTOAC_CKPOINT	0	/* checkpointSize tag */
#define	  RTOAC_CK_DFLT	0	/* default */
#define	RTOAC_WINDOW	1	/* windowSize tag */
#define	  RTOAC_WD_DFLT	3	/* default */
#define	RTOAC_CONNDATA	2	/* connectionDataAC */
#define	  RTOAC_CD_OPEN	0	/* open tag */
#define	  RTOAC_CD_RCVR	1	/* recover tag */

				/* RTORJ apdu */
#define	RTORJ_REFUSE	0	/* refuseReason tag */
#define	RTORJ_USERDATA	1	/* userDataRJ */

				/* RTAB apdu */
#define	RTAB_REASON	0	/* abortReason tag */
#define	RTAB_REFLECT	1	/* relectedParameter tag */
#define	RTAB_USERDATA	2	/* userDataAB */

extern int rtsap_priority;

#endif				/* __CL_H_RTPKT_H__ */
