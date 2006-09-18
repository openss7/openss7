/*****************************************************************************

 @(#) $Id: netconfig.h,v 0.9.2.1 2006/09/18 00:03:14 brian Exp $

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

 Last Modified $Date: 2006/09/18 00:03:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netconfig.h,v $
 Revision 0.9.2.1  2006/09/18 00:03:14  brian
 - added libxnsl source files

 *****************************************************************************/

#ifndef __SYS_NETCONFIG_H__
#define __SYS_NETCONFIG_H__

#ident "@(#) $RCSfile: netconfig.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

#define NETCONFIG		"/etc/netconfig"
#define NETPATH			"NETPATH"

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

#ifdef _REENTRANT
extern int *__ncerror(void);
#define nc_error (*(__ncerror()))
extern char *__ncerrbuf(void);
#define nc_errbuf (*(__ncerrbuf()))
#else
#error Compiled without _REENTRANT defined!
//extern int nc_error;
//extern char *nc_errbuf;
#endif

/* error codes */
#define NC_NOERROR		0
#define NC_NOMEM		1
#define NC_NOSET		2
#define NC_OPENFAIL		3
#define NC_BADLINE		4
#define NC_NOTFOUND		5
#define NC_NOMOREENTRIES	6
#define NC_ERROR_MAX		7

/* for use in nc_semanitcs field */
#define NC_TPI_CLTS		1
#define NC_TPI_COTS		2
#define NC_TPI_COTS_ORD		3
#define NC_TPI_RAW		4
#define NC_TPI_COTS_PKT		5

/* for use in nc_flag field */
#define NC_NOFLAG		0
#define NC_VISIBLE		1
#define NC_BROADCAST		2

/* for use in nc_protofmly field */
#define NC_NOPROTOFMLY		"-"
#define NC_LOOPBACK		"loopback"
#define NC_INET			"inet"
#define NC_INET6		"inet6"
#define NC_IMPLINK		"implink"
#define NC_PUP			"pup"
#define NC_CHAOS		"chaos"
#define NC_NS			"ns"
#define NC_NBS			"nbs"
#define NC_ECMA			"ecma"
#define NC_DATAKIT		"datakit"
#define NC_CCITT		"ccitt"
#define NC_SNA			"sna"
#define NC_DECNET		"decnet"
#define NC_DLI			"dli"
#define NC_LAT			"lat"
#define NC_HYLINK		"hylink"
#define NC_APPLETALK		"appletalk"
#define NC_NIT			"nit"
#define NC_IEEE802		"ieee802"
#define NC_OSI			"osi"
#define NC_X25			"x25"
#define NC_OSINET		"osinet"
#define NC_GOSIP		"gosip"
/* nonstandard extensions */
#define NC_AX25			"ax25"
#define NC_IPX			"ipx"
#define NC_NETROM		"netrom"
#define NC_BRIDGE		"bridge"
#define NC_ATMPVC		"atmpvc"
#define NC_ROSE			"rose"
#define NC_NETBEUI		"netbeui"
#define NC_ASH			"ash"
#define NC_ECONET		"econet"
#define NC_ATMSVC		"atmsvc"
#define NC_IRDA			"irda"
#define NC_PPPOX		"pppox"
#define NC_WANPIPE		"wanpipe"
#define NC_BLUETOOTH		"bluetooth"

/* for use in nc_proto field */
#define NC_NOPROTO		"-"
#define NC_TCP			"tcp"
#define NC_UDP			"udp"
#define NC_ICMP			"icmp"
#define NC_SCTP			"sctp"

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

extern void *setnetconfig(void);
extern struct netconfig *getnetconfig(void *handle);
extern struct netconfig *getnetconfigent(const char *netid);
extern void freenetconfigent(struct netconfig *netconfig);
extern int endnetconfig(void *handle);

extern void *setnetpath(void);
extern struct netconfig *getnetpath(void *handle);
extern int endnetpath(void *handle);

extern void nc_perror(const char *msg);
extern char *nc_sperror(void);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* __SYS_NETCONFIG_H__ */
