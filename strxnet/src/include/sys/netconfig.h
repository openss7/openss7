/*****************************************************************************

 @(#) $Id: netconfig.h,v 0.9.2.5 2007/08/14 04:00:45 brian Exp $

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

 Last Modified $Date: 2007/08/14 04:00:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netconfig.h,v $
 Revision 0.9.2.5  2007/08/14 04:00:45  brian
 - GPLv3 header update

 Revision 0.9.2.4  2006/09/24 21:57:22  brian
 - documentation and library updates

 Revision 0.9.2.3  2006/09/22 20:54:26  brian
 - prepared header file for use with doxygen, touching many lines

 Revision 0.9.2.2  2006/09/18 13:52:45  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:03:14  brian
 - added libxnsl source files

 *****************************************************************************/

#ifndef __SYS_NETCONFIG_H__
#define __SYS_NETCONFIG_H__

#ident "@(#) $RCSfile: netconfig.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup nsf
  * @{ */

/** @file src/include/sys/netconfig.h sys/netconfig.h
  * Primary network selection facility header file.  */

/*
 *  Primary network selection facility header file.
 */

#define NETCONFIG		"/etc/netconfig"
#define NETPATH			"NETPATH"

/** Network Configuration Structure. */
struct netconfig {
	char *nc_netid;		/**< Network identifier. */
	ulong nc_semantics;	/**< Transport provider semantics. */
	ulong nc_flag;		/**< Flags (see below). */
	char *nc_protofmly;	/**< Socket protocol family (e.g, @c "inet"). */
	char *nc_proto;		/**< Socket protocol (e.g., @c "tcp"). */
	char *nc_device;	/**< Full patn name to tp STREAMS device. */
	ulong nc_nlookups;	/**< Number of shared objecst in nc_lookups. */
	char **nc_lookups;	/**< Shared object name array. */
	ulong nc_unused[9];	/**< Unused. */
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

/** @name Network Selection Error Codes
 *  Error codes for use by nc_perror() and nc_sperror().
 *  @{ */
#define NC_NOERROR		0	/**< Success. */
#define NC_NOMEM		1	/**< Insufficient memory. */
#define NC_NOSET		2	/**< No set*() before get*(). */
#define NC_OPENFAIL		3	/**< Open of shared object failed. */
#define NC_BADLINE		4	/**< Bad line in configuration file. */
#define NC_NOTFOUND		5	/**< Host name not found. */
#define NC_NOMOREENTRIES	6	/**< get*() function exhausted list. */
#define NC_ERROR_MAX		7	/**< For sizing arrays. */
/** @} */

/** @name Transport Semantics Values
  * The transport semantics values for use with the netconfig#nc_semantics
  * field.
  * @{ */
#define NC_TPI_CLTS		1	/**< (RFC 1833). */
#define NC_TPI_COTS		2	/**< (RFC 1833). */
#define NC_TPI_COTS_ORD		3	/**< (RFC 1833). */
#define NC_TPI_RAW		4	/**< (RFC 1833). */
#define NC_TPI_COTS_PKT		5	/**< (SCTP). */
/** @} */

/** @name Transport Semantics Names
  * The transport semantics names for use in the /etc/netconfig file.
  * @{ */
#define NC_TPI_CLTS_S		"tpi_clts"	/**< TPI CLTS String. */
#define NC_TPI_COTS_S		"tpi_cots"	/**< TPI COTS String. */
#define NC_TPI_COTS_ORD_S	"tpi_cots_ord"	/**< TPI COTS ORD String. */
#define NC_TPI_RAW_S		"tpi_raw"	/**< TPI RAW String. */
#define NC_TPI_COTS_PKT_S	"tpi_cots_pkt"	/**< TPI COTS PKT String. */
/** @} */

/** @name Transport Flags
  * The flags for use in the netconfig#nc_flag field of the netconfig
  * structure.
  * @{ */
#define NC_NOFLAG		0	/**< No flag. */
#define NC_VISIBLE		1	/**< The visible flag. */
#define NC_BROADCAST		2	/**< The broadcast flag. */
#define NC_MULTICAST		4	/**< The multicast flag. */
/** @} */

/** @name Transport Flag Characters
  * The characters are for use in the /etc/netconfig file.
  * @{ */
/* for use in the flags field of /etc/netconfig */
#define NC_NOFLAG_C		'-'	/**< No flag character. */
#define NC_VISIBLE_C		'v'	/**< Visible character. */
#define NC_BROADCAST_C		'b'	/**< Broadcast character. */
#define NC_MULTICAST_C		'm'	/**< Multicast character. */
/** @} */

/** @name Protocol Family
  * For use in nc_protofmly field of the netconfig structure. These values
  * are the same as those in RFC 1833, "Binding Protocols for ONC RPC Version
  * 2".
  * @{ */
#define NC_NOPROTOFMLY	"-"		/**< No protocol family specified (RFC 1833). */
#define NC_LOOPBACK	"loopback"	/**< (RFC 1833). */
#define NC_INET		"inet"		/**< (RFC 1833). */
#define NC_INET6	"inet6"		/**< (RFC 1833). */
#define NC_IMPLINK	"implink"	/**< (RFC 1833). */
#define NC_PUP		"pup"		/**< (RFC 1833). */
#define NC_CHAOS	"chaos"		/**< (RFC 1833). */
#define NC_NS		"ns"		/**< (RFC 1833). */
#define NC_NBS		"nbs"		/**< (RFC 1833). */
#define NC_ECMA		"ecma"		/**< (RFC 1833). */
#define NC_DATAKIT	"datakit"	/**< (RFC 1833). */
#define NC_CCITT	"ccitt"		/**< (RFC 1833). */
#define NC_SNA		"sna"		/**< (RFC 1833). */
#define NC_DECNET	"decnet"	/**< (RFC 1833). */
#define NC_DLI		"dli"		/**< (RFC 1833). */
#define NC_LAT		"lat"		/**< (RFC 1833). */
#define NC_HYLINK	"hylink"	/**< (RFC 1833). */
#define NC_APPLETALK	"appletalk"	/**< (RFC 1833). */
#define NC_NIT		"nit"		/**< (RFC 1833). */
#define NC_IEEE802	"ieee802"	/**< (RFC 1833). */
#define NC_OSI		"osi"		/**< (RFC 1833). */
#define NC_X25		"x25"		/**< (RFC 1833). */
#define NC_OSINET	"osinet"	/**< (RFC 1833). */
#define NC_GOSIP	"gosip"		/**< (RFC 1833). */
/** @} */

/** @name Protocol Family Extensions
  * For use in nc_protofmly field of the netconfig structure. These values
  * deviate from RFC 1833, "Binding Protocols for ONC RPC Version 2".
  * @{ */
/* nonstandard extensions */
#define NC_AX25		"ax25"		/**< (Linux). */
#define NC_IPX		"ipx"		/**< (Linux). */
#define NC_NETROM	"netrom"	/**< (Linux). */
#define NC_BRIDGE	"bridge"	/**< (Linux). */
#define NC_ATMPVC	"atmpvc"	/**< (Linux). */
#define NC_ROSE		"rose"		/**< (Linux). */
#define NC_NETBEUI	"netbeui"	/**< (Linux). */
#define NC_ASH		"ash"		/**< (Linux). */
#define NC_ECONET	"econet"	/**< (Linux). */
#define NC_ATMSVC	"atmsvc"	/**< (Linux). */
#define NC_IRDA		"irda"		/**< (Linux). */
#define NC_PPPOX	"pppox"		/**< (Linux). */
#define NC_WANPIPE	"wanpipe"	/**< (Linux). */
#define NC_BLUETOOTH	"bluetooth"	/**< (Linux). */
/** @} */

/** @name Protocol Name
  * Constant strings defined for use in the netconfig#nc_proto field of the
  * #netconfig structure.  These strings are also defined for the rpcbind()
  * service.
  * @{ */
#define NC_NOPROTO	"-"		/**< No protocol specified (RFC 1833). */
#define NC_TCP		"tcp"		/**< Protocol is TCP (RFC 1833). */
#define NC_UDP		"udp"		/**< Protocol is UDP (RFC 1833). */
#define NC_ICMP		"icmp"		/**< Protocol is ICMP (RFC 1833). */
#define NC_SCTP		"sctp"		/**< Protocol is SCTP (OpenSS7). */
/** @} */

#endif				/* __SYS_NETCONFIG_H__ */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
