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

#ifndef __SNMP_ROUTES_H__
#define __SNMP_ROUTES_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* routes.h - support for MIB support of the routing tables */

/* 
 * Header: /xtel/isode/isode/snmp/RCS/routes.h,v 9.0 1992/06/16 12:38:11 isode Rel
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * Log: routes.h,v
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

#ifdef	BSD44
#include <sys/param.h>
#endif
#include <sys/mbuf.h>
#include <net/route.h>

#define	METRIC_NONE	(-1)	/* ipRouteMetric[1234] */

#define	PROTO_OTHER	1	/* ipRouteProto */
#define	PROTO_ICMP	4
#define	PROTO_ESIS	10

struct rtetab {
#define	RT_SIZE		20		/* object instance */
	unsigned int rt_instance[RT_SIZE + 1];
	int rt_insize;

	int rt_magic;			/* for multiple routes to the same destination */

	struct rtentry rt_rt;		/* from kernel */

	union sockaddr_un rt_dst;	/* key */
	union sockaddr_un rt_gateway;	/* value */

	int rt_type;			/* ipRouteType */
#define	TYPE_OTHER	1
#define	TYPE_INVALID	2
#define	TYPE_DIRECT	3
#define	TYPE_REMOTE	4

	int rt_touched;			/* set request'd */
	union sockaddr_un rt_oldgwy;	/* .. ipRouteNextHop */

	struct rtetab *rt_next;
};

extern int routeNumber;
extern int flush_rt_cache;

extern struct rtetab *rts;
extern struct rtetab *rts_inet;

#ifdef	BSD44
extern struct rtetab *rts_iso;
#endif

int get_routes();
struct rtetab *get_rtent();

#endif				/* __SNMP_ROUTES_H__ */
