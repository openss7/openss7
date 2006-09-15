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

#ifndef __NETDIR_H__
#define __NETDIR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2006 OpenSS7 Corporation."

#include <netconfig.h>

#ifdef _REENTRANT
extern int *__nderror(void);
#define nd_error (*(__nderror()))
extern char *__nderrbuf(void);
#define nd_errbuf (__nderrbuf())
#else
#warn Compiled without _REENTRANT defined!
extern int nd_error;
extern char *nd_errbuf;
#endif
/* compatibility */
#define _nderror nd_error

struct nd_addrlist {
	int n_cnt;			/* number of addresses */
	struct netbuf *n_addrs;		/* the addresses (array) */
};

struct nd_hostserv {
	char *h_name;			/* host name */
	char *h_serv;			/* service name */
};

#define HOST_SELF		"\\1"	/* local host name */
#define HOST_ANY		"\\2"	/* any host name */
#define HOST_BROADCAST		"\\3"	/* all addresses accessible by transport */
#define HOST_SELF_CONNECT	"\\4"	/* address for connecting to the local host */
#define HOST_SELF_BIND		HOST_SELF
#define HOST_ALLNODES		"\\5"
#define HOST_RPCNODES		"\\6"

struct nd_hostservlist {
	int h_cnt;			/* number of hostservs found */
	struct nd_hostserv *h_hostservs;
};

/* for use with netdir_free() */
#define ND_ADDR			1	/* struct netbuf */
#define ND_ADDRLIST		2	/* struct nd_addrlist */
#define ND_HOSTSERV		3	/* struct nd_hostserv */
#define ND_HOSTSERVLIST		4	/* struct nd_hostservlist */
#if 0
/* some other implemenations do this: why???? */
#define ND_HOSTSERV		0
#define ND_HOSTSERVLIST		1
#define ND_ADDR			2
#define ND_ADDRLIST		3
#endif

/* for use with netdir_options() */
#define ND_SET_BROADCAST	1
#define ND_SET_RESERVEDPORT	2
#define ND_CHECK_RESERVEDPORT	3
#define ND_MERGEADDR		4
#define ND_JOIN_MULTICAST	5
#define ND_LEAVE_MULTICAST	6

#define ND_CLEAR_BROADCAST	7	/* UnixWare - not binary compatible */
#define ND_SET_REUSEADDR	8	/* UnixWare - not binary compatible */
#define ND_CLEAR_REUSEADDR	9	/* UnixWare - not binary compatible */

struct nd_mergearg {
	char *s_uaddr;			/* server universal address */
	char *c_uaddr;			/* client universal address */
	char *m_uaddr;			/* merged universal address */
};

/* Negative errors are fatal, positive errors are warnings. */

#define ND_ERROR_OFS		-5
#define ND_TRY_AGAIN		-5
#define ND_NO_RECOVERY		-4
#define ND_NO_DATA		-3
#define ND_NO_ADDRESS		 ND_NO_DATA
#define ND_BADARG		-2
#define ND_NOMEM		-1
#define ND_OK			 0
#define ND_NOHOST		 1
#define ND_NOSERV		 2
#define ND_NOSYM		 3
#define ND_OPEN			 4
#define ND_ACCESS		 5
#define ND_UKNWN		 6
#define ND_NOCTRL		 7
#define ND_FAILCTRL		 8
#define ND_SYSTEM		 9
#define ND_NOCONVERT		10	/* AS400 only? */
#define ND_ERROR_MAX		11

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/*
 *  netdir_getbyname - converts the machine neame and service name in the
 *	nd_hostserv structure to a collection of addresses of the type
 *	understood by the transport identified in the netconfig structure.
 */
extern int netdir_getbyname(struct netconfig *config, struct nd_hostserv *service,
			    struct nd_addrlist **addrs);
/*
 *  netdir_getbyaddr - maps addresses to service names.  The function returns a
 *	service, a list of host and service pairs that yeild these addresses.
 *	If more than one tuple of host and service name is returned, the first
 *	type contains the preferred host and service names.
 */
extern int netdir_getbyaddr(struct netconfig *config, struct nd_hostservlist **service,
			    struct netbuf *netaddr);
/*
 *  netdir_free - used to free the structures allocated by the name to address
 *	translation functions.  The ptr parameter points to the structure that
 *	has to be freed.  The parameter struct_type identifies the structure.
 *	Universal addresses are freeed with free().
 */
extern void netdir_free(void *ptr, int struct_type);

/*
 *  netdir_options - used to do all transport specific setups and option
 *	management.  fd is the associated file descriptor.  option, fd, and
 *	pointer_to_args are passed ot the netdir_options() function for the
 *	transport specified in config.
 */
extern int netdir_options(struct netconfig *config, int option, int fd, char *point_to_args);

/*
 *  taddr2uaddr, uaddr2taddr - support transaltion between universal addresses
 *	and TLI type netbufs.  The taddr2uaddr function takes a struct netbuf
 *	data structure and returns a pointer to a string that contains the
 *	universal address.  It returns NULL if the conversion is not possible.
 *	This is not a fatal condition as some transports do not support a
 *	universal address form.
 */
extern char *taddr2uaddr(struct netconfig *config, struct netbuf *addr);

/*
 *  uaddr2taddr - the revers of the taddr2uaddr() function. It returns the
 *	struct netbuf data structure for the given universal address.
 */
extern struct netbuf *uaddr2taddr(struct netconfig *config, struct netbuf *addr);

/*
 *  netdir_perror - prints an error message to standard output that states the
 *	cause of a name-to-address mapping failure.  The error message is
 *	preceded by the string given as an argument.
 */
extern void netdir_perror(char *s);

/*
 *  netdir_sperror - returns a pointer to a buffer that contains the error
 *	message string.  The buffer is overwritten on each call.  In multhreaded
 *	applications, this buffers is implemented as thread-specific data.
 */
extern char *netdir_sperror(void);

/* types of symbols from lookup shared objects */
typedef struct nd_addrlist *(*nd_gbn_t) (struct netconfig *, struct nd_hostserv *);
typedef struct nd_hostservlist *(*nd_gba_t) (struct netconfig *, struct netbuf *);
typedef int (*nd_opt_t) (struct netconfig *, int, int, char *);
typedef char *(*nd_t2u_t) (struct netconfig *, struct netbuf *);
typedef struct netbuf *(*nd_u2t_t) (struct netconfig *, struct netbuf *);
typedef char *(*nd_mga_t) (struct netconfig *, char *, char *);

/* symbols from lookup shared objects */
struct nd_addrlist *_netdir_getbyname(struct netconfig *, struct nd_hostserv *);
struct nd_hostservlist *_netdir_getbyaddr(struct netconfig *, struct netbuf *);
int _netdir_options(struct netconfig *, int, int, char *);
char *_taddr2uaddr(struct netconfig *, struct netbuf *);
struct netbuf *_uaddr2taddr(struct netconfig *, struct netbuf *);
char *_netdir_mergeaddr(struct netconfig *, char *, char *);

#pragma weak _netdir_getbyname
#pragma weak _netdir_getbyaddr
#pragma weak _netdir_options
#pragma weak _taddr2uaddr
#pragma weak _uaddr2taddr
#pragma weak _netdir_mergeaddr

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* __NETDIR_H__ */
