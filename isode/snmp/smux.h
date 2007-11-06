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

#ifndef __SNMP_SMUX_H__
#define __SNMP_SMUX_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* smux.h - SMUX include file */

/* 
 * Header: /xtel/isode/isode/snmp/RCS/smux.h,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * Log: smux.h,v
 * Revision 9.0  1992/06/16  12:38:11  isode
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

#ifndef	PEPYPATH
#include <isode/pepsy/SNMP-types.h>
#else
#include "SNMP-types.h"
#endif

#define	readOnly	int_SNMP_operation_readOnly
#define	readWrite	int_SNMP_operation_readWrite
#define	delete		int_SNMP_operation_delete

#define	goingDown		int_SNMP_ClosePDU_goingDown
#define	unsupportedVersion	int_SNMP_ClosePDU_unsupportedVersion
#define	packetFormat		int_SNMP_ClosePDU_packetFormat
#define	protocolError		int_SNMP_ClosePDU_protocolError
#define	internalError		int_SNMP_ClosePDU_internalError
#define	authenticationFailure	int_SNMP_ClosePDU_authenticationFailure

#define	invalidOperation	(-1)
#define	parameterMissing	(-2)
#define	systemError		(-3)
#define	youLoseBig		(-4)
#define	congestion		(-5)
#define	inProgress		(-6)

extern integer smux_errno;
extern char smux_info[];

int smux_init();			/* INIT */
int smux_simple_open();			/* (simple) OPEN */
int smux_close();			/* CLOSE */
int smux_register();			/* REGISTER */
int smux_response();			/* RESPONSE */
int smux_wait();			/* WAIT */
int smux_trap();			/* TRAP */

char *smux_error();			/* TEXTUAL ERROR */

struct smuxEntry {
	char *se_name;

	OIDentifier se_identity;
	char *se_password;

	int se_priority;
};

int setsmuxEntry(), endsmuxEntry();

struct smuxEntry *getsmuxEntry();

struct smuxEntry *getsmuxEntrybyname();
struct smuxEntry *getsmuxEntrybyidentity();

#endif				/* __SNMP_SMUX_H__ */
