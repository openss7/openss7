/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

#ifndef __SYS_NETCONFIG_H__
#define __SYS_NETCONFIG_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2006 OpenSS7 Corporation."

struct netconfig {
	char *nc_netid;
	ulong nc_semantics;
	ulong nc_flag;
	char *nc_protofmly;
	char *nc_proto;
	char *nc_device;
	ulong nc_nlookups;
	char **nc_lookups;
	ulong nc_unused[9];
};

/* for use in nc_semanitcs field */
#define NC_TPI_CLTS		1
#define NC_TPI_COTS		2
#define NC_TPI_COTS_ORD		3

/* for use in nc_flag field */
#define NC_NOFLAG		0
#define NC_VISIBLE		1

extern struct netconfig *getnetconfig(void *handle);
extern void *setnetconfig(void);
extern int endnetconfig(void *handle);
extern struct netconfig *getnetconfigent(const char *netid);
extern void freenetconfigent(struct netconfig *netconfig);
extern void nc_perror(const char *msg);
extern char *nc_sperror(void);

extern struct netconfig *getnetpath(void *handle);
extern void *setnetpath(void);
extern int endnetpath(void *handle);

#endif				/* __SYS_NETCONFIG_H__ */

