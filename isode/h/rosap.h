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

#ifndef __ISODE_ROSAP_H__
#define __ISODE_ROSAP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* rosap.h - include file for remote operation users (RoS-USER) */

/* 
 * Header: /xtel/isode/isode/h/RCS/rosap.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * Log: rosap.h,v
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
#define	_RoSAP_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#ifndef	_AcSAP_
#include "acsap.h"		/* definitions for AcS-USERs */
#endif

struct RoSAPaddr {			/* RoSAP address (old-style) */
	struct SSAPaddr roa_addr;

	u_short roa_port;
};

#define	NULLRoA			((struct RoSAPaddr *) 0)

#define	ROS_MYREQUIRE	SR_DUPLEX	/* for direct users of ACSE */

struct RoSAPstart {			/* RO-BEGIN.INDICATION */
	int ros_sd;			/* ROS descriptor */

	struct RoSAPaddr ros_initiator;	/* unique identifier */

	u_short ros_port;		/* application number */

	PE ros_data;			/* initial DATA from peer */
};

#define	ROSFREE(ros) \
{ \
    if ((ros) -> ros_data) \
	pe_free ((ros) -> ros_data), (ros) -> ros_data = NULLPE; \
}

struct RoSAPconnect {			/* RO-BEGIN.CONFIRMATION */
	int roc_sd;			/* ROS descriptor */

	int roc_result;			/* result */
#define	ROS_ACCEPT	(-1)

	/* RO-BEGIN.REQUEST */
#define	ROS_VALIDATE	0		/* Authentication failure */
#define	ROS_BUSY	1		/* Busy */

	/* General Problem */
#define	ROS_GP_UNRECOG	2		/* Unrecognized APDU */
#define	ROS_GP_MISTYPED	3		/* Mistyped APDU */
#define	ROS_GP_STRUCT	4		/* Badly structured APDU */

	/* Invoke Problem */
#define	ROS_IP_DUP	5		/* Duplicate invocation */
#define	ROS_IP_UNRECOG	6		/* Unrecognized operation */
#define	ROS_IP_MISTYPED	7		/* Mistyped argument */
#define	ROS_IP_LIMIT	8		/* Resource limitation */
#define	ROS_IP_RELEASE	9		/* Initiator releasing */
#define	ROS_IP_UNLINKED	10		/* Unrecognized linked ID */
#define	ROS_IP_LINKED	11		/* Linked response unexpected */
#define	ROS_IP_CHILD	12		/* Unexpected child operation */

	/* Return Result Problem */
#define	ROS_RRP_UNRECOG	13		/* Unrecognized invocation */
#define	ROS_RRP_UNEXP	14		/* Result response unexpected */
#define	ROS_RRP_MISTYPED 15		/* Mistyped result */

	/* Return Error Problem */
#define	ROS_REP_UNRECOG	16		/* Unrecognized invocation */
#define	ROS_REP_UNEXP	17		/* Error response unexpected */
#define	ROS_REP_RECERR	18		/* Unrecognized error */
#define	ROS_REP_UNEXPERR 19		/* Unexpected error */
#define	ROS_REP_MISTYPED 20		/* Mistyped parameter */

	/* begin UNOFFICIAL */
#define	ROS_ADDRESS	21		/* Address unknown */
#define	ROS_REFUSED	22		/* Connect request refused on this network connection */
#define	ROS_SESSION	23		/* Session disconnect */
#define	ROS_PROTOCOL	24		/* Protocol error */
#define	ROS_CONGEST	25		/* Congestion at RoSAP */
#define	ROS_REMOTE	26		/* Remote system problem */
#define	ROS_DONE	27		/* Association done via async handler */
#define	ROS_ABORTED	28		/* Peer aborted association */
#define	ROS_RTS		29		/* RTS disconnect */
#define	ROS_PRESENTATION 30		/* Presentation disconnect */
#define	ROS_ACS		31		/* ACS disconnect */
#define	ROS_PARAMETER	32		/* Invalid parameter */
#define	ROS_OPERATION	33		/* Invalid operation */
#define	ROS_TIMER	34		/* Timer expired */
#define	ROS_WAITING	35		/* Indications waiting */
#define	ROS_APDU	36		/* APDU not transferred */
#define	ROS_INTERRUPTED	37		/* stub interrupted */
	/* end UNOFFICIAL */

#define	ROS_FATAL(r)	(!(ROS_OFFICIAL (r)) && (r) < ROS_PARAMETER)
#define	ROS_OFFICIAL(r)	((r) < ROS_ADDRESS)

	PE roc_data;			/* initial DATA from peer */
};

#define	ROCFREE(roc) \
{ \
    if ((roc) -> roc_data) \
	pe_free ((roc) -> roc_data), (roc) -> roc_data = NULLPE; \
}

				/* Operation Classes */
#define	ROS_SYNC	0	/* synchronous mode with result or error */
#define	ROS_ASYNC	1	/* asynchronous mode with result or error */

				/* APDU priorities */
#define	ROS_NOPRIO	0	/* no priority */

struct RoSAPinvoke {			/* RO-INVOKE.INDICATION */
	int rox_id;			/* invokeID */

	int rox_linkid;			/* linkedID */
	int rox_nolinked;		/* non-zero if no linkedID present */

	int rox_op;			/* operation */
	PE rox_args;			/* arguments */
};

#define	ROXFREE(rox) \
{ \
    if ((rox) -> rox_args) \
	pe_free ((rox) -> rox_args), (rox) -> rox_args = NULLPE; \
}

struct RoSAPresult {			/* RO-RESULT.INDICATION */
	int ror_id;			/* invokeID */

	int ror_op;			/* operation */
	PE ror_result;			/* result */
};

#define	RORFREE(ror) \
{ \
    if ((ror) -> ror_result) \
	pe_free ((ror) -> ror_result), (ror) -> ror_result = NULLPE; \
}

struct RoSAPerror {			/* RO-ERROR.INDICATION */
	int roe_id;			/* invokeID */

	int roe_error;			/* error */
	PE roe_param;			/* parameter */
};

#define	ROEFREE(roe) \
{ \
    if ((roe) -> roe_param) \
	pe_free ((roe) -> roe_param), (roe) -> roe_param = NULLPE; \
}

struct RoSAPureject {			/* RO-U-REJECT.INDICATION */
	int rou_id;			/* invokeID */
	int rou_noid;			/* .. is not present */

	int rou_reason;			/* reason (same as roc_result) */
};

struct RoSAPpreject {			/* RO-P-REJECT.INDICATION */
	int rop_reason;			/* reason (same as roc_result) */

	/* APDU not transferred (reason is ROS_APDU) */
	int rop_id;			/* invokeID */
	PE rop_apdu;			/* APDU */

	/* additional failure data from provider */
#define	ROP_SIZE	512
	int rop_cc;			/* length */
	char rop_data[ROP_SIZE];	/* data */
};

#define	ROPFREE(rop) \
{ \
    if ((rop) -> rop_apdu) \
	pe_free ((rop) -> rop_apdu), (rop) -> rop_apdu = NULLPE; \
}

struct RoSAPend {			/* RO-END.INDICATION */
	int roe_dummy;			/* not used */
};

struct RoSAPindication {
	int roi_type;			/* the union element present */
#define	ROI_INVOKE	0x00
#define	ROI_RESULT	0x01
#define	ROI_ERROR	0x02
#define	ROI_UREJECT	0x03
#define	ROI_PREJECT	0x04
#define	ROI_END		0x05
#define	ROI_FINISH	0x06

	union {
		struct RoSAPinvoke roi_un_invoke;
		struct RoSAPresult roi_un_result;
		struct RoSAPerror roi_un_error;
		struct RoSAPureject roi_un_ureject;
		struct RoSAPpreject roi_un_preject;
		struct RoSAPend roi_un_end;
		struct AcSAPfinish roi_un_finish;
	} roi_un;
#define	roi_invoke	roi_un.roi_un_invoke
#define	roi_result	roi_un.roi_un_result
#define	roi_error	roi_un.roi_un_error
#define	roi_ureject	roi_un.roi_un_ureject
#define	roi_preject	roi_un.roi_un_preject
#define	roi_end		roi_un.roi_un_end
#define	roi_finish	roi_un.roi_un_finish
};

extern char *rosapversion;

int RoExec();				/* SERVER only */
int RoInit();				/* RO-BEGIN.INDICATION */

int RoBeginResponse();			/* RO-BEGIN.RESPONSE */
int RoBeginRequest();			/* RO-BEGIN.REQUEST */
int RoEndRequest();			/* RO-END.REQUEST */
int RoEndResponse();			/* RO-END.RESPONSE */

int RoInvokeRequest();			/* RO-INVOKE.REQUEST */
int RoResultRequest();			/* RO-RESULT.REQUEST */
int RoErrorRequest();			/* RO-ERROR.REQUEST */
int RoURejectRequest();			/* RO-U-REJECT.REQUEST */

#define	RoRejectURequest	RoURejectRequest
int RoIntrRequest();			/* RO-INVOKE.REQUEST (interruptable) */
int RoWaitRequest();			/* RO-WAIT.REQUEST (pseudo) */

int RoSetIndications();			/* define vectors for INDICATION events */
int RoSelectMask();			/* map remote operation descriptors for select() */

int RoSetService();			/* bind underlying service */
int RoRtService(), RoPService(), RoSService();
int RoSetThorn();

char *RoErrString();			/* return RoSAP error code in string form */
#endif

#endif				/* __ISODE_ROSAP_H__ */
