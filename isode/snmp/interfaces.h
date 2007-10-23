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

#ifndef __SNMP_INTERFACES_H__
#define __SNMP_INTERFACES_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* interfaces.h - support for MIB realization of the Interfaces group */

/* 
 * Header: /xtel/isode/isode/snmp/RCS/interfaces.h,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * Log: interfaces.h,v
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

#include "internet.h"
#include <net/if.h>
#include <netinet/if_ether.h>	/* to get struct arpcom */
#include "clns.h"

extern int ifNumber;

struct interface {
	int ifn_index;			/* 1..ifNumber */
	int ifn_indexmask;		/* 1 << (index - 1) */

	int ifn_ready;			/* has an address associated with it */

	struct arpcom ifn_interface;	/* ifnet+physaddr */
	unsigned long ifn_offset;	/* where in kmem */

	char ifn_descr[IFNAMSIZ];	/* e.g., "lo0" */

	int ifn_type;			/* ifType */
	u_long ifn_speed;		/* ifSpeed */

	int ifn_admin;			/* ifAdminStatus */
	int ifn_touched;		/* .. new value */

	struct interface *ifn_next;
};

extern struct interface *ifs;

int set_interface(), sort_interface();

union sockaddr_un {			/* 'cause sizeof (struct sockaddr_iso) == 32 */
	struct sockaddr sa;

	struct sockaddr_in un_in;
#ifdef	BSD44
	struct sockaddr_iso un_iso;
#endif
};

struct address {
#define	ADR_SIZE	(20 + 1 + 1)	/* object instance */
	unsigned int adr_instance[ADR_SIZE];
	int adr_insize;

	union sockaddr_un adr_address;	/* address */
	union sockaddr_un adr_broadaddr;	/* broadcast, only if AF_INET */
	union sockaddr_un adr_netmask;	/* network mask */

	int adr_indexmask;		/* mask of interfaces with address */

	struct address *adr_next;
};

extern struct address *afs_inet;

#ifdef	BSD44
extern struct address *afs_iso;
#endif

struct address *find_address(), *get_addrent();

#if	defined(BSD44) || defined(BSD43_Tahoe) || defined(RT) || defined(MIPS) || defined(ultrix) || defined(__NeXT__) || defined(sgi)
#define	NEW_AT
#else
#undef	NEW_AT
#endif

#endif				/* __SNMP_INTERFACES_H__ */
