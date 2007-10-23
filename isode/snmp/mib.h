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

#ifndef __SNMP_MIB_H__
#define __SNMP_MIB_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* mib.h - MIB realization */

/* 
 * Header: /xtel/isode/isode/snmp/RCS/mib.h,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * Log: mib.h,v
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

#include <nlist.h>
#ifdef	__NeXT__
#define	n_name	n_un.n_name
#endif
#include "SNMP-types.h"
#include "objects.h"
#include "logger.h"

#define	generr(offset)	((offset) == type_SNMP_PDUs_get__next__request \
				    ? NOTOK : int_SNMP_error__status_genErr)

#define	sysDescr	"4BSD/ISODE SNMP"
#define	sysObjectID	"fourBSD-isode.5"

extern struct nlist nl[];

#define	N_ARPTAB	0
#define	N_ARPTAB_SIZE	1
#define	N_ICMPSTAT	2
#define	N_IFNET		3
#define	N_IPFORWARDING	4
#define	N_IPSTAT	5
#define	N_RTHASHSIZE	6
#define	N_RTHOST	7
#define	N_RTNET		8
#define	N_TCB		9
#define	N_TCPSTAT	10
#define	N_UDB		11
#define	N_UDPSTAT	12
#define	N_RTSTAT	13
#define	N_MBSTAT	14
#ifdef	BSD44
#define	N_RADIX_NODE_HEAD 15
#define	N_ISO_SYSTYPE	16
#define	N_CLNP_STAT	17
#define	N_ESIS_STAT	18
#endif

int init_mib(), fin_mib(), set_variable();
int getkmem(), setkmem(), chekmem();

extern int nd;
extern int quantum;

extern struct timeval
 my_boottime;

extern OID nullSpecific;

void adios(), advise();

#endif				/* __SNMP_MIB_H__ */
