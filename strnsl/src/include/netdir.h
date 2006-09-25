/*****************************************************************************

 @(#) $Id: netdir.h,v 0.9.2.1 2006/09/25 12:30:55 brian Exp $

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

 Last Modified $Date: 2006/09/25 12:30:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netdir.h,v $
 Revision 0.9.2.1  2006/09/25 12:30:55  brian
 - added files for new strnsl package

 Revision 0.9.2.4  2006/09/24 21:57:21  brian
 - documentation and library updates

 Revision 0.9.2.3  2006/09/22 20:54:24  brian
 - prepared header file for use with doxygen, touching many lines

 Revision 0.9.2.2  2006/09/18 13:52:44  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:03:13  brian
 - added libxnsl source files

 *****************************************************************************/

#ifndef __NETDIR_H__
#define __NETDIR_H__

#ident "@(#) $RCSfile: netdir.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup n2a
  * @{ */

/** @file
  * Network Name-to-Address Translation header file.  */

/*
 *  Network Name-to-Address Translation header file.
 */

#include <netconfig.h>

#ifdef _REENTRANT
extern int *__nderror(void);

#define nd_error (*(__nderror()))
extern char *__nderrbuf(void);

#define nd_errbuf (__nderrbuf())
#else
#error Compiled without _REENTRANT defined!
//extern int nd_error;
//extern char *nd_errbuf;
#endif
/* compatibility */
#define _nderror nd_error

/** Network Address List. */
struct nd_addrlist {
	int n_cnt;			/**< Number of addresses in array. */
	struct netbuf *n_addrs;		/**< The addresses (array). */
};

/** Host/Service Name. */
struct nd_hostserv {
	char *h_name;			/**< Host name. */
	char *h_serv;			/**< Service name. */
};

/** @name Special Hostnames
 *  Special hostname strings for use with nd_hostserv#h_name.
 *  @{ */
#define HOST_SELF		"\\1"	/**< Local host name. */
#define HOST_ANY		"\\2"	/**< Any host name. */
#define HOST_BROADCAST		"\\3"	/**< All addresses accessible by transport. */
#define HOST_SELF_CONNECT	"\\4"	/**< Address for connecting to the local host. */
#define HOST_SELF_BIND		HOST_SELF /**< Bind to one's self. */
#define HOST_ALLNODES		"\\5"	/**< All nodes (rpcbind broadcast). */
#define HOST_RPCNODES		"\\6"	/**< All RPM nodes (rpcbind broadcast). */
/** @} */

/** Host/Service Name List. */
struct nd_hostservlist {
	int h_cnt;			/**< Number of Host/Service names in array. */
	struct nd_hostserv *h_hostservs; /**< Array of host service names. */
};

/** @name Structure Types
 *  struct_type for use with netdir_free().
 *  @{ */
#define ND_ADDR			1	/**< struct netbuf */
#define ND_ADDRLIST		2	/**< struct nd_addrlist */
#define ND_HOSTSERV		3	/**< struct nd_hostserv */
#define ND_HOSTSERVLIST		4	/**< struct nd_hostservlist */
#if 0
/* some other implemenations do this: why???? */
#define ND_HOSTSERV		0
#define ND_HOSTSERVLIST		1
#define ND_ADDR			2
#define ND_ADDRLIST		3
#endif
/** @} */

/** @name Netdir Options
 *  Options for use with netdir_options().
 *  @{ */
/* Following are tirpc2.3 compatible. */
#define ND_SET_BROADCAST	 1	/**< Permit use of broadcast address on tp endpoint (TI-RPC). */
#define ND_SET_RESERVEDPORT	 2	/**< Bind to a reserved port on tp endpoint (TI-RPC). */
#define ND_CHECK_RESERVEDPORT	 3	/**< Check if port in address is reserved (TI-RPC). */
#define ND_MERGEADDR		 4	/**< Merge underspecified server address with client (TI-RPC). */
#define ND_JOIN_MULTICAST	 5	/**< Join a multicast group (TI-RPC). */
#define ND_LEAVE_MULTICAST	 6	/**< Leave a multicast group (TI-RPC). */
/* Following are defined by some other Unices. */
#define ND_CLEAR_BROADCAST	 7	/**< Prohibit use of broadcast address (UnixWare). */
#define ND_SET_REUSEADDR	 8	/**< Permit reuse of local addresses (UnixWare). */
#define ND_CLEAR_REUSEADDR	 9	/**< Prohibit reuse of local addresses (UnixWare). */
/* Following are to support sockets library SOL_SOCKET options. */
#define ND_SET_DONTROUTE	10	/**< Do no route packets from tp endpoint (OpenSS7). */
#define ND_CLEAR_DONTROUTE	11	/**< Route packets from tp endpoint (OpenSS7). */
#define ND_SET_KEEPALIVE	12	/**< Set keepalive on tp endpoint (OpenSS7). */
#define ND_CLEAR_KEEPALIVE	13	/**< Clear keepalive on tp endpoint (OpenSS7). */
#define ND_SET_PRIORITY		14	/**< Set priority of tp endpoint (OpenSS7). */
/** @} */

/** Merge address group. */
struct nd_mergearg {
	char *s_uaddr;			/**< Server universal address. */
	char *c_uaddr;			/**< Client universal address. */
	char *m_uaddr;			/**< Merged universal address. */
};

/** @name Name-to-Address Translation Errors
 *  Error numbers returned by name-to-address translation functions.
 *
 *  Negative errors are fatal, positive errors are warnings.
 *  @{ */
#define ND_ERROR_OFS		-5	/**< Negative offset to place array at zero. */
#define ND_TRY_AGAIN		-5	/**< Try again later. */
#define ND_NO_RECOVERY		-4	/**< Unrecoverable error. */
#define ND_NO_DATA		-3	/**< Insufficient data. */
#define ND_NO_ADDRESS		 ND_NO_DATA /**< Insufficient address. */
#define ND_BADARG		-2	/**< Bad argument to call. */
#define ND_NOMEM		-1	/**< Insufficient memory for operation. */
#define ND_OK			 0	/**< No error. */
#define ND_NOHOST		 1	/**< Host name not found. */
#define ND_NOSERV		 2	/**< Service name not found. */
#define ND_NOSYM		 3	/**< Symbol missing from shared object. */
#define ND_OPEN			 4	/**< Could not open database or shared object. */
#define ND_ACCESS		 5	/**< Access denied. */
#define ND_UKNWN		 6	/**< Unknown error. */
#define ND_NOCTRL		 7	/**< netdir_option() option unrecognized/unsupported. */
#define ND_FAILCTRL		 8	/**< Control operation failed. */
#define ND_SYSTEM		 9	/**< System error, see errno(3) and t_errno(3). */
#define ND_NOCONVERT		10	/**< String conversion failed (AS400 only?). */
#define ND_ERROR_MAX		11	/**< Maximum size of array (plus #ND_ERROR_OFS). */
/** @} */

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/** @name Name-to-Address Translation API Functions
 *  @brief Primary functions exported by the name-to-address translation module.
 *  @{ */
/** Get transport network address by host and service name.
 *  netdir_getbyname() converts the machine neame and service name in the
 *  nd_hostserv structure to a collection of addresses of the type understood
 *  by the transport identified in the netconfig structure.  */
extern int netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service,
			    struct nd_addrlist **addrs);

/** Get host and service name by transport network address.
 *  netdir_getbyaddr() maps addresses to service names.  The function returns
 *  a service, a list of host and service pairs that yeild these addresses.
 *  If more than one tuple of host and service name is returned, the first
 *  type contains the preferred host and service names.  */
extern int netdir_getbyaddr(struct netconfig *nc, struct nd_hostservlist **service,
			    struct netbuf *netaddr);

/** Deep free a name-to-address translation structure.  netdir_free() is used
 *  to free the structures allocated by the name to address translation
 *  functions.  The ptr parameter points to the structure that has to be
 *  freed.  The parameter struct_type identifies the structure.  Universal
 *  addresses are freed with free().  */
extern void netdir_free(void *ptr, int struct_type);

/** Perform transport-specific operations or apply options to a transport
 *  endpoint.  netdir_options() is used to do all transport specific setups
 *  and option management.  fd is the associated file descriptor.  option,
 *  fd, and pta are passed to the netdir_options() function for the
 *  transport specified in nc.  */
extern int netdir_options(struct netconfig *nc, int option, int fd, char *pta);

/** Translate an address from transport specific address to universal address.
 *  taddr2uaddr() and uaddr2taddr() support transaltion between universal
 *  addresses and TLI type struct netbuf.  The taddr2uaddr() function takes a
 *  struct netbuf data structure and returns a pointer to a string that
 *  contains the universal address.  It returns NULL if the conversion is not
 *  possible.  This is not a fatal condition as some transports do not support
 *  a universal address form.  */
extern char *taddr2uaddr(struct netconfig *nc, struct netbuf *taddr);

/** Translate a universal address to a transport specific address.
 *  uaddr2taddr() operates the reverse of the taddr2uaddr() function. It
 *  returns the struct netbuf data structure for the given universal address.  */
extern struct netbuf *uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr);

/** Print a name-to-address translation error message.  netdir_perror() prints
 *  an error message to standard output that states the cause of a
 *  name-to-address mapping failure.  The error message is preceded by the
 *  string given as an argument.  */
extern void netdir_perror(char *msg);

/** Return a name-to-address translation error message string.
 *  netdir_sperror() returns a pointer to a buffer that contains the error
 *  message string.  The buffer is overwritten on each call.  In multhreaded
 *  applications, this buffers is implemented as thread-specific data.  */
extern char *netdir_sperror(void);

/** @} */

/** @name Name-to-Address Translation Internal API Functions
  * @brief These are the internal implementations of the corresponding functions.
  * @{ */
extern int __nsl_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service,
				  struct nd_addrlist **addrs);
extern int __nsl_netdir_getbyaddr(struct netconfig *nc, struct nd_hostservlist **service,
				  struct netbuf *netaddr);
extern void __nsl_netdir_free(void *ptr, int struct_type);
extern int __nsl_netdir_options(struct netconfig *nc, int option, int fd, char *pta);
extern char *__nsl_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr);
extern struct netbuf *__nsl_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr);
extern void __nsl_netdir_perror(char *msg);
extern char *__nsl_netdir_sperror(void);
/** @} */

/** @name Symbol Types
 *  Types of symbols from lookup shared objects.
 *  @{ */
/** Function pointer to _netdir_getbyname() function. */
typedef struct nd_addrlist *(*nd_gbn_t) (struct netconfig *, struct nd_hostserv *);

/** Function pointer to _netdir_getbyaddr() function. */
typedef struct nd_hostservlist *(*nd_gba_t) (struct netconfig *, struct netbuf *);

/** Function pointer to _netdir_options() function. */
typedef int (*nd_opt_t) (struct netconfig *, int, int, char *);

/** Function pointer to _taddr2uaddr() function. */
typedef char *(*nd_t2u_t) (struct netconfig *, struct netbuf *);

/** Function pointer to _uaddr2tadddr() function. */
typedef struct netbuf *(*nd_u2t_t) (struct netconfig *, struct netbuf *);

/** Function pointer to _netdir_mergeaddr() function. */
typedef char *(*nd_mga_t) (struct netconfig *, char *, char *);

/** @} */

/** @name Symbol Names
 *  Symbols from lookup shared objects.
 *  @{ */
/** netdir_getbyname() internal function. */
struct nd_addrlist *_netdir_getbyname(struct netconfig *, struct nd_hostserv *);

/** netdir_getbyaddr() internal function. */
struct nd_hostservlist *_netdir_getbyaddr(struct netconfig *, struct netbuf *);

/** netdir_options() internal function. */
int _netdir_options(struct netconfig *, int, int, char *);

/** taddr2uaddr() internal function. */
char *_taddr2uaddr(struct netconfig *, struct netbuf *);

/** uaddr2taddr() internal function. */
struct netbuf *_uaddr2taddr(struct netconfig *, struct netbuf *);

/** netdir_mergeaddr() internal function. */
char *_netdir_mergeaddr(struct netconfig *, char *, char *);

#pragma weak _netdir_getbyname
#pragma weak _netdir_getbyaddr
#pragma weak _netdir_options
#pragma weak _taddr2uaddr
#pragma weak _uaddr2taddr
#pragma weak _netdir_mergeaddr
/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* __NETDIR_H__ */

/** @} */

// vim: ft=cpp com=sr\:/**,mb\:\ *,eb\:\ */,sr\:/*,mb\:*,eb\:*/,b\:TRANS
