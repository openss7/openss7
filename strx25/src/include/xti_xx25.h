/*****************************************************************************

 @(#) $Id: xti_xx25.h,v 0.9.2.1 2008-05-03 10:46:38 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-05-03 10:46:38 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_xx25.h,v $
 Revision 0.9.2.1  2008-05-03 10:46:38  brian
 - added package files

 *****************************************************************************/

#ifndef __XTI_XX25_H__
#define __XTI_XX25_H__

#ident "@(#) $RCSfile: xti_xx25.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 * This appendix presents the additional header file information for XX25.
 * Implementations supporting XX25 will provide equivalent definitions in
 * <xti_xx25.h>. XX25 programs should include <xti_xx25.h> as well as <xti.h>.
 *
 * Values specified for some of the symbolic constants in this X25 header
 * definitions are designated as not mandatory for conformance purposes. These
 * are indentified by the comment accompanying the constant definition.
 */

/*
 * New Error Codes to Support X.25 Service
 */
#define TX25NOTOACK		41	/* no data to acknowledge */

/*
 * New Flags, defined to support X.25 Service
 */
#define T_X25_D			0x0800	/* Data with D bit set */
#define T_X25_Q			0x1000	/* Qualified Data */
#define T_X25_RST		0x2000	/* Request or Indication of reset */
#define T_X25_DACK		0x4000	/* Acknowledgement of data sent with D bit. */
#define T_X25_EACK		0x8000	/* Acknowledgement of expedited data */

/*
 * New Macros to Set and Retrieve Cause and Diagnostic
 * of a Connection Release
 */
#define T_X25_SET_CAUSE_DIAG(x,y)	(((x) << 8) + (y))
#define T_X25_GET_CAUSE(x)		(((x) >> 8) & 0xff)
#define T_X25_GET_DIAG(x)		((x) & 0xff)

/*
 * X.25 Level
 */
#define T_X25_NP		0x101	/* X.25 Level; value is recommended only, not mandatory */

/*
 * New Options to Support X.25 Service.  These values are recommended only, not
 * mandatory.
 */
#define T_X25_USER_DACK		0x0001	/* Explicit Acknowledgement of data */
#define T_X25_USER_EACK		0x0002	/* Explicit Acknowledgement of expedited data */
#define T_X25_RST_OPT		0x0003	/* Reset is known to the appli. */
#define T_X25_VERSION		0x0004	/* Version of ITU-T Recommendation X.25 or ISO/IEC X.25 */
#define T_X25_DISCON_REASON	0x0005	/* Reason of a Connection release */
#define T_X25_DISCON_ADD	0x0006	/* Address of the user that released the connection */
#define T_X25_D_OPT		0x0007	/* Support of the D bit */
#define T_X25_CONN_DBIT		0x0008	/* Setting of the D-bit at the connection phase */

/*
 * Options to support X.25 facilities.  These values are recommended only, not
 * mandatory.
 */
#define T_X25_PKTSIZE		0x0009	/* Packet Size */
#define T_X25_WINDOWSIZE	0x000A	/* Window Size */
#define T_X25_TCN		0x000B	/* Througput Class Negotiation (basic format) */
#define T_X25_CUG		0x000C	/* CUG (basic format) */
#define T_X25_CUGOUT		0x000D	/* CUG with Outgoing Access (basic format) */
#define T_X25_BCUG		0x000E	/* Bilateral CUG */
#define T_X25_FASTSELECT	0x000F	/* Fast Select */
#define T_X25_REVCHG		0x0010	/* Reverse Charging */
#define T_X25_NUI		0x0011	/* NUI - Network User Identification */
#define T_X25_CHGINFO_REQ	0x0012	/* Charging Information Req. Service */
#define T_X25_CHGINFO_MU	0x0013	/* Charging Information Monetary Unit */
#define T_X25_CHGINFO_SC	0x0014	/* Charging Information Segment Count */
#define T_X25_CHGINFO_CD	0x0015	/* Charging Information Call Duration */
#define T_X25_RPOA		0x0016	/* RPOA-Recognised Private Operating Agency-(basic format) */
#define T_X25_CALLDEF		0x0017	/* Call Deflection Selection */
#define T_X25_CALLRED		0x0018	/* Call Redirection or Deflection Notification */
#define T_X25_CALLADDMOD	0x0019	/* Called Line Address Modified Notification */
#define T_X25_TDSAI		0x001A	/* Transit Delay Selection and Indication */
#define T_X25_CALLING_ADDEXT	0x001B	/* Calling Address Extension */
#define T_X25_CALLED_ADDEXT	0x001C	/* Called Address Extension */
#define T_X25_MTCN		0x001D	/* Minimum Throughput Class Neg. */
#define T_X25_EETDN		0x001E	/* End-to-End Transit Delay Neg. */
#define T_X25_PRIORITY		0x001F	/* Priority */
#define T_X25_PROTECTION	0x0020	/* Protection */
#define T_X25_EDN		0x0021	/* Expedited Data Negotiation */
#define T_X25_LOC_NONX25	0x0022	/* Non-X25 local facilities */
#define T_X25_REM_NONX25	0x0023	/* Non-X25 remote facilities */

/*
 * New Values for the XX25 Options
 */

/*
 * New Values for the T_X25_VERSION Option "Version of ITU-T Recommendation X.25
 * or ISO/IEC X.25"
 */
#define T_X25_1980		1980	/* X.25 1980 version */
#define T_X25_1984		1984	/* X.25 1984 version */
#define T_X25_1988		1988	/* X.25 1988 version */
#define T_X25_1993		1993	/* X.25 1993 version */

/*
 * New Values for the T_X25_FASTSELECT Option "Fast Select X.25 facility"
 */
#define T_X25_FASTSEL_NOREST	0x0002	/* Fast Select requested with no restriction on response */
#define T_X25_FASTSEL_REST	0x0003	/* Fast Select requested with restriction on response */

/*
 * New Defines for the reason code for the DTE deflecting the call for the
 * Options:
 *
 * T_X25_CALLDEF "Call Deflection Selection facility"
 * T_X25_CALLRED "Call Redirection or Deflection Notification facility"
 * T_X25_CALLADDMOD "Called Line Address Modified Notification facility"
 */
#define T_X25_CLDEF1		0x0001	/* Call-deflection by the originally-called DTE */
#define T_X25_CLDEF2		0x0002	/* Call-deflection by gateway as a result of call
					   redirection due to originally-called DTE busy */
#define T_X25_CLDEF3		0x0003	/* Call-deflection by gateway as a result of call
					   redirection due to originally-called DTE out-of-order */
#define T_X25_CLDEF4		0x0004	/* Call-deflection by gateway as a result of call
					   redirection due to prior request from originally-called
					   DTE systematic call redirection */
#define T_X25_CLRED1		0x0005	/* Call-redirection due to originally-called DTE busy */
#define T_X25_CLRED2		0x0006	/* Call-distribution within a hunt group */
#define T_X25_CLRED3		0x0007	/* Call-redirection due to originally-called DTE
					   out-of-order */
#define T_X25_CLRED4		0x0008	/* Call-redirection due to prior request from
					   originally-called DTE for systematic call redirection */

/*
 * New Defines for the type of the address extension for the Options:
 * T_X25_CALLING_ADDEXT "Calling Address Extension facility"
 * T_X25_CALLED_ADDEXT "Called Address Extension facility"
 */
#define T_X25_NSAPADDR		0x0001	/* address defined according to ISO/IEC 8348 */
#define T_X25_OTHERADDR		0x0002	/* address defined in another format */

/*
 * New Defines for the type of the priority for the T_X25_PRIORITY option
 * "Priority facility"
 */
#define T_X25_PRIDATA		0x0001	/* priority on data on a connection */
#define T_X25_PRIGAIN		0x0002	/* priority to gain a connection */
#define T_X25_PRIKEEP		0x0003	/* priority to keep a connection */

/*
 * New Defines for the Type of the Protection for the T_X25_PROTECTION option
 * "Protection Facility"
 */
#define T_X25_SRCPROTECT	0x0001	/* Source-address specific protection */
#define T_X25_DESTPROTECT	0x0002	/* destination-address specific protection */
#define T_X25_GLBPROTECT	0x0003	/* global protection */

/*
 * New Structures for the XX25 Options (X.25 facilities)
 */
struct t_x25addext {
	unsigned char addr_type;
	unsigned char len;
	unsigned char addr[20];
};

struct t_x25facval {
	unsigned long remote;		/* value for the direction of data transmission from the
					   called DTE */
	unsigned long local;		/* value for the direction of data transmission from the
					   calling DTE */
};

struct t_x25facinfocd {
	unsigned char day;		/* number of days for the call */
	unsigned char hour;		/* number of hours for the call */
	unsigned char min;		/* number of minutes for the call */
	unsigned char sec;		/* number of seconds for the call */
};

struct t_x25facaddr {
	unsigned char code;		/* reason code */
	unsigned char len;		/* length in semi-octets */
	unsigned char addr[8];		/* DTE address */
};

struct t_x25faceetdn {
	unsigned short cumuldel;	/* cumulative transit delay */
	unsigned short targetdel;	/* target end-to-end transit delay */
	unsigned short maxdel;		/* maximum end-to-end transit delay */
};

struct t_x25facpr {
	unsigned char typeval;		/* type of the value */
	unsigned char targetval;	/* target value */
	unsigned char lowval;		/* lowest-acceptable value */
}

#endif				/* __XTI_XX25_H__ */

