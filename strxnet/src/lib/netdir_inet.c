/*****************************************************************************

 @(#) $RCSfile: netdir_inet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/14 04:00:51 $

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

 Last Modified $Date: 2007/08/14 04:00:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netdir_inet.c,v $
 Revision 0.9.2.5  2007/08/14 04:00:51  brian
 - GPLv3 header update

 Revision 0.9.2.4  2006/09/24 21:57:23  brian
 - documentation and library updates

 Revision 0.9.2.3  2006/09/22 20:54:27  brian
 - tweaked source file for use with doxygen

 Revision 0.9.2.2  2006/09/18 13:52:56  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:03:15  brian
 - added libxnsl source files

 *****************************************************************************/

#ident "@(#) $RCSfile: netdir_inet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/14 04:00:51 $"

static char const ident[] =
    "$RCSfile: netdir_inet.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/14 04:00:51 $";

/* This file can be processed with doxygen(1). */

/**
  * @weakgroup inetn2a INET Name-to-Address Translation
  * @{ */

/** @file
  * INET Network Name-to-Address Translation implementation file.
  *
  * This is not only a sample inet name-to-address shared object library, it is
  * also linked into libxnsl to provide default inet name-to-address mappings.  */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#if 0
#define __USE_UNIX98
#define __USE_XOPEN2K
#define __USE_GNU
#endif

#define NEED_T_USCALAR_T 1

#include <stdlib.h>

#include "gettext.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#include <sys/stropts.h>
#include <sys/poll.h>
#include <fcntl.h>

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifndef __EXCEPTIONS
#define __EXCEPTIONS 1
#endif

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <pthread.h>
#include <linux/limits.h>
#include <values.h>

#include <dlfcn.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

#if defined __i386__ || defined __x86_64__ || defined __k8__
#define fastcall __attribute__((__regparm__(3)))
#else
#define fastcall
#endif

#define __hot __attribute__((section(".text.hot")))
#define __unlikely __attribute__((section(".text.unlikely")))

#if __GNUC__ < 3
#define inline static inline fastcall __hot
#define noinline extern fastcall __unlikely
#else
#define inline static inline __attribute__((always_inline)) fastcall __hot
#define noinline static __attribute__((noinline)) fastcall __unlikely
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#undef min
#define min(a, b) (a < b ? a : b)

#include <xti.h>
#include <xti_inet.h>
#include <netconfig.h>
#include <netdir.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#ifndef NETDIR_STRONG_DEFS
#pragma weak __inet_netdir_getbyname
#pragma weak __inet_netdir_getbyaddr
#pragma weak __inet_netdir_options
#pragma weak __inet_taddr2uaddr
#pragma weak __inet_uaddr2taddr
#pragma weak __inet_netdir_mergeaddr
#endif				/* NETDIR_STRONG_DEFS */

/*
 *  Name-to-Address Translation Function defaults for inet:
 *  =======================================================
 */

/** @brief Inet _netdir_getbyname() lookup function.
  * @param nc transport.
  * @param h host and service name.
  * @par Alias:
  * This function is an implementation of _netdir_getbyname().
  *
  * Converts a host and service name into an address list.  This function uses
  * the newer getaddrinfo(3) AF_INET lookup call.  It is fairly simplistic.
  */
struct nd_addrlist *
__inet_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *h)
{
	struct addrinfo hints, *res = NULL, *ai;
	struct nd_addrlist *n = NULL;
	struct netbuf *b;
	int cnt;

	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;

	if (getaddrinfo(h->h_name, h->h_serv, &hints, &res) == -1) {
		nd_error = ND_SYSTEM;
		return (NULL);
	}
	/* count them */
	for (cnt = 0, ai = res; ai; cnt++, ai = ai->ai_next) ;
	if (cnt == 0) {
		nd_error = ND_NOHOST;
		return (NULL);
	}
	if ((n = malloc(sizeof(*n))) == NULL)
		goto nomem;
	if ((n->n_addrs = calloc(cnt, sizeof(struct netbuf))) == NULL)
		goto nomem;
	memset(n->n_addrs, 0, cnt * sizeof(struct netbuf));
	for (b = n->n_addrs, n->n_cnt = 0, ai = res; n->n_cnt < cnt;
	     b++, n->n_cnt++, ai = ai->ai_next) {
		if (ai->ai_addr && ai->ai_addrlen > 0) {
			if ((b->buf = malloc(ai->ai_addrlen)) == NULL)
				goto nomem;
			memcpy(b->buf, ai->ai_addr, ai->ai_addrlen);
			b->len = b->maxlen = ai->ai_addrlen;
		}
	}
	freeaddrinfo(res);
	return (n);
      nomem:
	nd_error = ND_NOMEM;
	goto error;
      error:
	netdir_free(n, ND_ADDRLIST);
	freeaddrinfo(res);
	return (NULL);
}

/*  @fn struct nd_addrlist *_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *h)
  * @param nc transport.
  * @param h host and service name.
  * @par Alias:
  * This symbol is an strong alias of __inet_netdir_getbyname().
  */
struct nd_addrlist *_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *h)
    __attribute__ ((alias("__inet_netdir_getbyname")));

/** @brief Inet _netdir_getbyaddr() lookup function.
  * @param nc transport.
  * @param addr address to lookup.
  * @par Alias:
  * This function is an implementation of _netdir_getbyaddr().
  */
struct nd_hostservlist *
__inet_netdir_getbyaddr(struct netconfig *nc, struct netbuf *addr)
{
	char hbuf[128];
	char sbuf[128];
	char *host = NULL;
	char *serv = NULL;
	char **aliasp, **aliases = NULL;
	struct sockaddr_in *sin;
	struct servent *s;
	struct hostent *h;
	struct nd_hostservlist *hl = NULL;
	struct nd_hostserv *hs;
	int cnt;

	if (addr == NULL || addr->buf == NULL || addr->len == 0) {
		nd_error = ND_BADARG;
		return (NULL);
	}
	if (addr->len < sizeof(*sin)) {
		nd_error = ND_NO_ADDRESS;
		return (NULL);
	}
	sin = (struct sockaddr_in *) addr->buf;
	if (sin->sin_family != AF_INET && sin->sin_family != 0) {
		nd_error = ND_NO_ADDRESS;
		return (NULL);
	}
	if ((s = getservbyport(sin->sin_port, nc->nc_proto)) == NULL) {
		uint16_t in_port = ntohs(sin->sin_port);

		sprintf(sbuf, "%hd", in_port);
		serv = sbuf;
	} else {
		serv = s->s_name;
	}
	if ((h = gethostbyaddr((char *) &sin->sin_addr.s_addr, 4, AF_INET)) == NULL) {
		uint32_t in_addr = ntohl(sin->sin_addr.s_addr);

		sprintf(hbuf, "%d.%d.%d.%d", (in_addr >> 24) & 0xff, (in_addr >> 16) & 0xff,
			(in_addr >> 8) & 0xff, (in_addr >> 0) & 0xff);
		host = hbuf;
	} else {
		host = h->h_name;
		aliases = h->h_aliases;
	}
	/* count 'em all */
	for (cnt = 1, aliasp = h->h_aliases; *aliasp; cnt++, aliasp++) ;
	if ((hl = malloc(sizeof(*hl))) == NULL)
		goto nomem;
	memset(hl, 0, sizeof(*hl));
	if ((hs = hl->h_hostservs = calloc(cnt, sizeof(struct nd_hostserv))) == NULL)
		goto nomem;
	memset(hs, 0, cnt * sizeof(*hs));
	hl->h_cnt = 1;
	if ((hs->h_name = strdup(host)) == NULL)
		goto nomem;
	if ((hs->h_serv = strdup(serv)) == NULL)
		goto nomem;
	hs++;
	hl->h_cnt++;
	aliasp = h->h_aliases;
	for (; *aliasp; aliasp++, hs++, hl->h_cnt++) {
		if ((hs->h_name = strdup(*aliasp)) == NULL)
			goto nomem;
		if ((hs->h_serv = strdup(serv)) == NULL)
			goto nomem;
	}
	return (hl);
      nomem:
	nd_error = ND_NOMEM;
	goto error;
      error:
	netdir_free(hl, ND_HOSTSERVLIST);
	return (NULL);
}

/*  @fn struct nd_hostservlist *_netdir_getbyaddr(struct netconfig *nc, struct netbuf *addr)
  * @param nc transport.
  * @param addr address to lookup.
  * @par Alias:
  * This symbol is a strong alias of __inet_netdir_getbyaddr().
  */
struct nd_hostservlist *_netdir_getbyaddr(struct netconfig *nc, struct netbuf *addr)
    __attribute__ ((alias("__inet_netdir_getbyaddr")));

static int __setoption(int fd, t_scalar_t level, t_scalar_t name, t_scalar_t value);
static int __setresvport(int fd, struct netbuf *addr);
static int __checkresvport(struct netbuf *addr);
static int __joinmulticast(int fd, struct netbuf *addr);
static int __leavemulticast(int fd, struct netbuf *addr);

char *__inet_netdir_mergeaddr(struct netconfig *nc, char *caddr, char *saddr);

/** @brief Inet _netdir_options() lookup function.
  * @param nc transport.
  * @param option option to effect.
  * @param fd a file descriptor to set or RPC_ANYFD.
  * @param pta pointer to arguments for the option.
  * @par Alias:
  * This function is an implementation of _netdir_getbyaddr().
  */
int
__inet_netdir_options(struct netconfig *nc, int option, int fd, char *pta)
{
	switch (option) {
	case ND_SET_BROADCAST:
		return (__setoption(fd, T_INET_IP, T_IP_BROADCAST, T_YES));
#ifdef ND_CLEAR_BROADCAST
	case ND_CLEAR_BROADCAST:
		return (__setoption(fd, T_INET_IP, T_IP_BROADCAST, T_NO));
#endif
#ifdef ND_SET_REUSEADDR
	case ND_SET_REUSEADDR:
		return (__setoption(fd, T_INET_IP, T_IP_REUSEADDR, T_YES));
#endif
#ifdef ND_CLEAR_REUSEADDR
	case ND_CLEAR_REUSEADDR:
		return (__setoption(fd, T_INET_IP, T_IP_REUSEADDR, T_NO));
#endif
#ifdef ND_SET_DONTROUTE
	case ND_SET_DONTROUTE:
		return (__setoption(fd, T_INET_IP, T_IP_DONTROUTE, T_YES));
#endif
#ifdef ND_CLEAR_DONTROUTE
	case ND_CLEAR_DONTROUTE:
		return (__setoption(fd, T_INET_IP, T_IP_DONTROUTE, T_NO));
#endif
#ifdef NS_SET_PRIORITY
	case ND_SET_PRIORITY:
		if (pta == NULL) {
			nd_error = ND_BADARG;
			return (-1);
		}
		return (__setoption(fd, T_INET_IP, T_IP_TOS, *(int *) pta));
#endif
#ifdef ND_SET_KEEPALIVE
	case ND_SET_KEEPALIVE:
		if (nc == NULL) {
			nd_error = ND_BADARG;
			return (-1);
		}
		if (nc->nc_proto) {
			if (strcmp(nc->nc_proto, NC_TCP) == 0)
				return (__setoption(fd, T_INET_TCP, T_TCP_KEEPALIVE, T_YES));
#if defined N_SCTP && defined T_SCTP_KEEPALIVE
			if (strcmp(nc->nc_proto, NC_SCTP) == 0)
				return (__setoption(fd, T_INET_SCTP, T_SCTP_KEEPALIVE, T_YES));
#endif				/* defined N_SCTP && defined T_SCTP_KEEPALIVE */
		}
		nd_error = ND_NOCTRL;
		errno = ENOPROTOOPT;
		return (-1);
#endif
#ifdef ND_CLEAR_KEEPALIVE
	case ND_CLEAR_KEEPALIVE:
		if (nc == NULL) {
			nd_error = ND_BADARG;
			return (-1);
		}
		if (nc->nc_proto) {
			if (strcmp(nc->nc_proto, NC_TCP) == 0)
				return (__setoption(fd, T_INET_TCP, T_TCP_KEEPALIVE, T_NO));
#if defined N_SCTP && defined T_SCTP_KEEPALIVE
			if (strcmp(nc->nc_proto, NC_SCTP) == 0)
				return (__setoption(fd, T_INET_SCTP, T_SCTP_KEEPALIVE, T_NO));
#endif				/* defined N_SCTP && defined T_SCTP_KEEPALIVE */
		}
		nd_error = ND_NOCTRL;
		errno = ENOPROTOOPT;
		return (-1);
#endif
	case ND_SET_RESERVEDPORT:
		return (__setresvport(fd, (struct netbuf *) pta));
	case ND_CHECK_RESERVEDPORT:
		return (__checkresvport((struct netbuf *) pta));
	case ND_MERGEADDR:
	{
		struct nd_mergearg *m = (typeof(m)) (pta);

		if ((m->m_uaddr = __inet_netdir_mergeaddr(nc, m->c_uaddr, m->s_uaddr)))
			return (0);
		return (-1);
	}
#ifdef ND_JOIN_MULTICAST
	case ND_JOIN_MULTICAST:
		return (__joinmulticast(fd, (struct netbuf *) pta));
#endif
#ifdef ND_LEAVE_MULTICAST
	case ND_LEAVE_MULTICAST:
		return (__leavemulticast(fd, (struct netbuf *) pta));
#endif
	default:
		nd_error = ND_NOCTRL;
		errno = ENOPROTOOPT;
		return (-1);
	}
}

/*  @fn int _netdir_options(struct netconfig *nc, int option, int fd, char *pta)
  * @param nc transport.
  * @param option option to effect.
  * @param fd a file descriptor to set or RPC_ANYFD.
  * @param pta pointer to arguments for the option.
  * @par Alias:
  * This symbol is a strong alias of __inet_netdir_options().
  */
int _netdir_options(struct netconfig *nc, int option, int fd, char *pta)
    __attribute__ ((alias("__inet_netdir_options")));

static int
__setoption(int fd, t_scalar_t level, t_scalar_t name, t_scalar_t value)
{
	int state;
	struct t_optmgmt req, ret;
	struct {
		struct t_opthdr hdr;
		t_scalar_t value;
	} optbuf;

	nd_error = ND_SYSTEM;
	if ((state = t_getstate(fd)) != T_UNBND) {
		if (t_errno == TBADF)
			errno = EBADF;
		if (state != -1)
			errno = EISCONN;
		return (-1);
	}
	optbuf.hdr.len = sizeof(optbuf);
	optbuf.hdr.level = level;
	optbuf.hdr.name = name;
	optbuf.hdr.status = 0;
	optbuf.value = value;
	req.opt.buf = (char *) &optbuf;
	req.opt.len = sizeof(optbuf);
	req.opt.maxlen = sizeof(optbuf);
	req.flags = T_NEGOTIATE;
	ret.opt.buf = NULL;
	ret.opt.len = 0;
	ret.opt.maxlen = -1;
	ret.flags = T_FAILURE;
	if (t_optmgmt(fd, &req, &ret) == -1) {
		nd_error = ND_SYSTEM;
		errno = EINVAL;
		if (t_errno == TBADF)
			errno = EBADF;
		if (t_errno == TBADFLAG)
			errno = EINVAL;
		if (t_errno == TBADOPT)
			errno = EINVAL;
		if (t_errno == TBUFOVFLW)
			errno = EINVAL;
		if (t_errno == TNOTSUPPORT)
			errno = EOPNOTSUPP;
		if (t_errno == TOUTSTATE)
			errno = EPROTO;
		if (t_errno == TPROTO)
			errno = EPROTO;
		return (-1);
	}
	if (ret.flags != T_SUCCESS && ret.flags != T_PARTSUCCESS) {
		switch (ret.flags) {
		case T_NOTSUPPORT:
			/* This is normally an unsupported option (which we would not provide) or a
			   privilege failure, which could happen. */
			nd_error = ND_FAILCTRL;
			t_errno = TACCES;
			errno = EPERM;	/* EACCES? */
			return (-1);
		case T_READONLY:
		case T_FAILURE:
			nd_error = ND_FAILCTRL;
			t_errno = TSYSERR;
			errno = EINVAL;
			return (-1);
		default:
			nd_error = ND_FAILCTRL;
			return (-1);
		}
	}
	// nd_error = ND_OK;
	return (0);
}

static int
__setresvport(int fd, struct netbuf *addr)
{
	/* not implemented yet */
	nd_error = ND_NOCTRL;
	return (-1);
}
static int
__checkresvport(struct netbuf *addr)
{
	struct sockaddr_in *sin;
	unsigned short port;

	if (addr == NULL || addr->buf == NULL || addr->len == 0) {
		nd_error = ND_NO_ADDRESS;
		return (-1);
	}
	if (addr->len < sizeof(*sin)) {
		nd_error = ND_BADARG;
		return (-1);
	}
	sin = (typeof(sin)) (addr->buf);
	port = ntohs(sin->sin_port);
	// nd_error = ND_OK;
	if (port < IPPORT_RESERVED)
		return (0);
	return (1);
}
static int
__joinmulticast(int fd, struct netbuf *addr)
{
	/* not implemented yet */
	nd_error = ND_NOCTRL;
	return (-1);
}
static int
__leavemulticast(int fd, struct netbuf *addr)
{
	/* not implemented yet */
	nd_error = ND_NOCTRL;
	return (-1);
}

/** @brief Convert an inet transport address to a universal address.
  * @param nc the transport.
  * @param taddr the transport address.
  * @par Alias:
  * THis function is an implementation of _taddr2uaddr().
  *
  * Convert from a trasnsport address to a universal address.  The universal
  * address is formatted as %d.%d.%d.%d.%d.%d which represents the address and
  * port number from most significant byte to lease significant byte.  Each
  * number between the dots is a byte.
  */
char *
__inet_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
{
	struct sockaddr_in *sin;
	char buf[INET_ADDRSTRLEN + 16], *ret;
	unsigned short port;
	int len;

	if (taddr == NULL || taddr->buf == NULL || taddr->len == 0) {
		nd_error = ND_BADARG;
		return (NULL);
	}
	if (taddr->len < sizeof(*sin)) {
		nd_error = ND_NO_ADDRESS;
		return (NULL);
	}
	sin = (typeof(sin)) (taddr->buf);
	port = ntohs(sin->sin_port);

	if (inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf)) == NULL) {
		nd_error = ND_SYSTEM;
		return (NULL);
	}
	len = strlen(buf);
	(void) snprintf(buf + len, sizeof(buf) - len, ".%d.%d", port >> 8, port & 0xff);
	ret = strdup(buf);
	if (ret == NULL) {
		nd_error = ND_NOMEM;
		return (NULL);
	}
	return (ret);
}

/*  @fn char *_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
  * @param nc transport.
  * @param taddr transport address.
  * @par Alias:
  * This symbol is a strong alias of __inet_taddr2uaddr().
  */
char *_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
    __attribute__ ((alias("__inet_taddr2uaddr")));

/** @brief Convert a universal address to a transport address.
  * @param nc the transport.
  * @param uaddr the universal address.
  * @par Alias:
  * This function is an implementation of _uaddr2taddr().
  *
  * Convert from universal address format to one suitable for use with inet.
  * The universal address is %d.%d.%d.%d.%d.%d which represent the address and
  * port number from most significant byte to least significant byte.  Each
  * number between the dots is a byte.
  */
struct netbuf *
__inet_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr)
{
	struct netbuf *nb = NULL;
	struct sockaddr_in *sin = NULL;
	uint32_t addr = 0;
	unsigned short port = 0;
	char *beg, *end;
	int field;

	if (uaddr == NULL || uaddr->buf == NULL || uaddr->len == 0) {
		nd_error = ND_BADARG;
		return (NULL);
	}
	if (uaddr->len < sizeof(*sin)) {
		nd_error = ND_NO_ADDRESS;
		return (NULL);
	}
	if ((nb = (typeof(nb)) malloc(sizeof(*nb))) == NULL) {
		nd_error = ND_NOMEM;
		return (NULL);
	}
	if ((sin = (typeof(sin)) malloc(sizeof(*sin))) == NULL) {
		free(nb);
		nd_error = ND_NOMEM;
		return (NULL);
	}
	nb->buf = (char *) sin;
	nb->maxlen = sizeof(*sin);
	nb->len = sizeof(*sin);

	for (field = 0, beg = end = uaddr->buf;; field++, beg = end + 1) {
		ulong value;

		if (field > 5)
			break;
		value = strtoul(beg, &end, 10);
		if (beg == end || (end[0] != '.' && end[0] != '\0')) {
			field--;
			break;
		}
		if (field <= 3)
			addr |= (value & 0xff) << (24 - 8 * field);
		if (field >= 4)
			port |= (value & 0xff) << (8 - 8 * (field - 4));
		if (end[0] == '\0')
			break;
	}
	if (field != 5) {
		free(nb);
		free(sin);
		nd_error = ND_NO_ADDRESS;
		return (NULL);
	}
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(addr);
	sin->sin_port = htons(port);
	return (nb);
}

/*  @fn struct netbuf *_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr)
  * @param nc transport.
  * @param uaddr universal address.
  * @par Alias:
  * This symbol is a strong alias of __inet_uaddr2taddr().
  */
struct netbuf *_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr)
    __attribute__ ((alias("__inet_uaddr2taddr")));

/** @brief Merge a server and client address into a merged address.
  * @param nc the transport.
  * @param caddr the client universal address.
  * @param saddr the server universal address.
  * @par Alias:
  * This function is an implementation of _netdir_mergeaddr().
  *
  * When a server address is underspecified (such as 0.0.0.0.1.12) this function
  * takes a client address (e.g. 192.168.0.5.1.12) and makes the merged address
  * into a specific address at which the client can contact the server (possibly
  * 192.168.0.1.1.12 in this case).
  */
char *
__inet_netdir_mergeaddr(struct netconfig *nc, char *caddr, char *saddr)
{
#if 0
	/* working on it */
	struct netbuf cbuf, sbuf, mbuf, *cadd = NULL, *sadd = NULL, *madd = NULL;
	struct sockaddr_in *csin, *ssin, *msin;
	char *maddr = NULL;

	sbuf.buf = saddr;
	sbuf.len = sbuf.maxlen = strlen(saddr);
	if (!(sadd = __inet_uaddr2taddr(nc, &sbuf)))
		goto error;
	ssin = (typeof(ssin)) sadd->buf;
	if (ssin->sin_addr.s_addr != INADDR_ANY) {
		if ((maddr = strdup(saddr)) == NULL)
			goto nomem;
		goto done;
	}

	cbuf.buf = caddr;
	cbuf.len = cbuf.maxlen = strlen(caddr);
	if (!(cadd = __inet_uaddr2taddr(nc, &cbuf)))
		goto error;
	csin = (typeof(csin)) cadd->buf;
	if (ssin->sin_addr.s_addr == INADDR_ANY) {
		if ((maddr = strdup(saddr)) == NULL)
			goto nomem;
		goto done;
	}

	mbuf.buf = saddr;
	mbuf.len = mbuf.maxlen = strlen(saddr);
	if (!(madd = __inet_uaddr2taddr(nc, &mbuf)))
		goto error;
	msin = (typeof(msin)) madd->buf;

	/* Need to find the local interface address that is closest to the client address and fill
	   it out in msin->sin_addr.s_addr. */

	if (!(maddr = __inet_taddr2uaddr(nc, madd)))
		goto error;
	goto done;

      nomem:
	nd_error = ND_NOMEM;
	goto error;
      error:
	if (maddr != NULL) {
		free(maddr);
		maddr == NULL;
	}
      done:
	netdir_free(cadd, ND_ADDR);
	netdir_free(sadd, ND_ADDR);
	netdir_free(madd, ND_ADDR);
	return (maddr);
#endif
	/* not implemented yet */
	nd_error = ND_NOCTRL;
	return (NULL);
}

/*  @fn char *_netdir_mergeaddr(struct netconfig *nc, char *caddr, char *saddr)
  * @param nc transport.
  * @param caddr client address.
  * @param saddr server address.
  * @par Alias:
  * This symbol is a strong alias of __inet_netdir_mergeaddr().
  */
char *_netdir_mergeaddr(struct netconfig *nc, char *caddr, char *saddr)
    __attribute__ ((alias("__inet_netdir_mergeaddr")));

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
