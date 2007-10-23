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

#ifndef __ISODE_ROPKT_H__
#define __ISODE_ROPKT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ropkt.h - include file for remote operation providers (RoS-PROVIDER) */

/* 
 * Header: /xtel/isode/isode/h/RCS/ropkt.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: ropkt.h,v
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

#ifndef	_RoSAP_
#include "rosap.h"		/* definitions for RoS-USERs */
#endif

#include "acpkt.h"		/* definitions for AcS-PROVIDERs */

#ifndef	_RtSAP_
#include "rtsap.h"		/* definitions for RtS-USERs */
#endif

#ifndef _PSAP2_
#include "psap2.h"		/* definitions for PS-USERs */
#endif

#ifndef	_SSAP_
#include "ssap.h"		/* definitions for SS-USERs */
#endif

#define	rosapPsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (acb -> acb_flags & ACB_CLOSING) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_WAITING, NULLCP, NULLCP); \
    } \
    if (acb -> acb_flags & ACB_FINN) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor finishing"); \
    } \
    if (acb -> acb_putosdu == NULLIFP) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not bound"); \
    } \
}

#define	rosapXsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (acb -> acb_flags & ACB_FINN) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor finishing"); \
    } \
    if (acb -> acb_putosdu == NULLIFP) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not bound"); \
    } \
}

#define	rosapFsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_ROS)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "not an association descriptor for ROS"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (!(acb -> acb_flags & ACB_FINN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not finishing"); \
    } \
    if (acb -> acb_putosdu == NULLIFP) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not bound"); \
    } \
}

#ifdef __STDC__
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing",#p); \
}
#else
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}
#endif

#ifndef	lint
#ifndef	__STDC__
#define	copyRoSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/**/_cc = min (i, sizeof d -> d/**/_data)) > 0) \
	bcopy (base, d -> d/**/_data, d -> d/**/_cc); \
}
#else
#define	copyRoSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyRoSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

#define	pylose() \
	ropktlose (acb, roi, ROS_PROTOCOL, NULLCP, "%s", PY_pepy)

int ropktlose(), rosapreject(), rosaplose();

int acb2osdu();

int ro2rtswrite(), ro2rtswait(), ro2rtsready(), ro2rtsasync(), ro2rtsmask();

int ro2pswrite(), ro2pswait(), ro2psasync(), ro2psmask();

int ss2roslose(), ss2rosabort();
int ro2sswrite(), ro2sswait(), ro2ssasync(), ro2ssmask(), ro2sslose(), ro2ssready();

				/* APDU types */
#define	APDU_INVOKE	1	/* Invoke */
#define	APDU_RESULT	2	/* Return result */
#define	APDU_ERROR	3	/* Return error */
#define	APDU_REJECT	4	/* Reject */

#define	APDU_UNKNOWN	(-1)	/* anything other than the above */

				/* Reject APDU types */
#define	REJECT_GENERAL	0	/* General Problem */
#define	  REJECT_GENERAL_BASE	ROS_GP_UNRECOG
#define	REJECT_INVOKE	1	/* Invoke Problem */
#define	  REJECT_INVOKE_BASE	ROS_IP_DUP
#define	REJECT_RESULT	2	/* Return Result Problem */
#define	  REJECT_RESULT_BASE	ROS_RRP_UNRECOG
#define	REJECT_ERROR	3	/* Return Error Problem */
#define	  REJECT_ERROR_BASE	ROS_REP_UNRECOG
#define	REJECT_COMPLETE	4	/* more Invoke Problem codes */

#endif				/* __ISODE_ROPKT_H__ */
