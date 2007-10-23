/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* tsaperror.c - print out TPKT error codes */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/tsap/RCS/tsaperror.c,v 9.0 1992/06/16 12:40:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/tsap/RCS/tsaperror.c,v 9.0 1992/06/16 12:40:39 isode Rel
 *
 *
 * Log: tsaperror.c,v
 * Revision 9.0  1992/06/16  12:40:39  isode
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

/* LINTLIBRARY */

#include <stdio.h>
#include "tsap.h"
#include "tp4.h"
#ifdef	SUNLINK_7_0
#include <netosi/osi_layer.h>
#include <netosi/cons_impl.h>
#endif

/*  */

static char *disc_err0[] = {
	"Reason not specified",
	"Congestion at TSAP",
	"Session entity not attached to TSAP",
	"Address unknown"
};

static int disc_err0_cnt = sizeof disc_err0 / sizeof disc_err0[0];

static char *disc_err8[] = {
	"Normal disconnect initiated by session entity",
	"Remote transport entity congestion at connect time",
	"Connection negotiation failed (proposed class(es) not supported)",
	"Duplicate source reference detected for the same pair of NSAPs",
	"Mismatched references",
	"Protocol error",
	"Not used",
	"Reference overflow",
	"Connect request refused on this network connection",
	"Not used",
	"Header or parameter length invalid",
	"Network disconnect",
	"Invalid parameter",
	"Invalid operation",
	"Timer expired",
	"Indications waiting"
};

static int disc_err8_cnt = sizeof disc_err8 / sizeof disc_err8[0];

/*  */

#ifdef	SUNLINK_7_0
static char *cons_err0 = "Unspecified (undefined) CONS error";

/* CONS Error 224 through 255 */
static char *cons_err1[] = {
	"CONS provider: undefined",	/* R_CP_OSI_NETWORK_SERVICE_PROBLEM */
	"CONS provider: disconnection-transient",
	/* R_CP_DIS_TRANS */
	"CONS provider: disconnection-permanent",
	/* R_CP_DIS_PERM */
	"CONS provider: connection rejection - reason unspecified (transient)",
	/* R_CP_CON_REJ_UNSPEC_TRANS */
	"CONS provider: connection rejection - reason unspecified (permanent)",
	/* R_CP_CON_REJ_UNSPEC_PERM */
	"CONS provider: connection rejection - QOS not available (transient)",
	/* R_CP_CON_REJ_NO_QOS_TRANS */
	"CONS provider: connection rejection - QOS not available (permanent)",
	/* R_CP_CON_REJ_NO_QOS_PERM */
	"CONS provider: connection rejection - NSAP unreachable (transient)",
	/* R_CP_CON_REJ_NSAP_UNREACH_TRANS */
	"CONS provider: connection rejection - NSAP unreachable (permanent)",
	/* R_CP_CON_REJ_NSAP_UNREACH_PERM */
	"CONS provider: RESET - reason unspecified",
	/* R_CP_RESET_UNSPEC */
	"CONS provider: RESET - congestion",
	/* R_CP_RESET_CONGESTION */
	"CONS provider: connection rejection - NSAP address unknown (permanent)",
	/* R_CP_CON_REJ_NSAP_UNKNOWN_PERM */
	"CONS provider: 236",
	/* R_CP_X25_236 */
	"CONS provider: 237",
	/* R_CP_X25_237 */
	"CONS provider: 238",
	/* R_CP_X25_238 */
	"CONS provider: 239",
	/* R_CP_X25_239 */
	"CONS user: undefined",
	/* R_CU_HIGHER_LEVEL_INITIATED = 240 */
	"CONS user: disconnection - normal",
	/* R_CU_DIS_NORMAL */
	"CONS user: disconnection - abnormal",
	/* R_CU_DIS_ABNORMAL */
	"CONS user: 243",
	/* R_CU_DIS_INCOMPAT */
	"CONS user: connection rejection - transient",
	/* R_CU_CON_REJ_UNSPEC_TRANS */
	"CONS user: connection rejection - permanent",
	/* R_CU_CON_REJ_UNSPEC_PERM */
	"CONS user: connection rejection - QOS not available (transient)",
	/* R_CU_CON_REJ_NO_QOS_TRANS */
	"CONS user: connection rejection - QOS not available (permanent)",
	/* R_CU_CON_REJ_NO_QOS_PERM */
	"CONS user: connection rejection - incompatible info in NS-user-data",
	/* R_CU_CON_REJ_INCOMPAT */
	"CONS user: 249",
	/* R_CU_CON_UNREC_PROTO */
	"CONS user: RESET - user resynchronization",
	/* R_CU_RESET_USER_RESYNCH */
	"CONS user: 251",
	/* R_CU_X25_251 */
	"CONS user: 252",
	/* R_CU_X25_252 */
	"CONS user: 253",
	/* R_CU_X25_253 */
	"CONS user: 254",
	/* R_CU_X25_254 */
	"CONS user: 255",
	/* R_CU_X25_255 */
};
#endif

/*  */

char *
TErrString(code)
	register int code;
{
	register int fcode;
	static char buffer[60];

#ifdef	SUNLINK_7_0
	if (code > 0xff) {
		/* CONS error code */
		code -= 0x100;
		if (code == R_CONS_UNDEFINED)
			return cons_err0;
		if (code >= R_CP_OSI_NETWORK_SERVICE_PROBLEM && code <= R_CU_X25_255)
			return cons_err1[code - R_CP_OSI_NETWORK_SERVICE_PROBLEM];

		(void) sprintf(buffer, "unknown CONS error code 0x%x", code);
		return buffer;
	}
#endif

	code &= 0xff;
	if (code & DR_BASE) {
		if ((fcode = code & ~DR_BASE) < disc_err8_cnt)
			return disc_err8[fcode];
	} else if (code < disc_err0_cnt)
		return disc_err0[code];

	(void) sprintf(buffer, "unknown error code 0x%x", code);
	return buffer;
}
