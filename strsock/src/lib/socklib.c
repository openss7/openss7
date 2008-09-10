/*****************************************************************************

 @(#) $RCSfile: socklib.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-09-10 03:49:56 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-09-10 03:49:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: socklib.c,v $
 Revision 0.9.2.6  2008-09-10 03:49:56  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.5  2008-04-28 22:33:33  brian
 - updated headers for release

 Revision 0.9.2.4  2007/08/14 05:17:23  brian
 - GPLv3 header update

 Revision 0.9.2.3  2006/09/25 12:22:38  brian
 - working up library

 Revision 0.9.2.2  2006/09/18 13:52:53  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:10:36  brian
 - added libsocket source files and manuals

 *****************************************************************************/

#ident "@(#) $RCSfile: socklib.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-09-10 03:49:56 $"

static char const ident[] =
    "$RCSfile: socklib.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-09-10 03:49:56 $";

/* This file can be processed with doxygen(1). */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#if 0
#define __USE_UNIX98
#define __USE_XOPEN2K
#define __USE_GNU
#endif

#include <stdlib.h>
#include "gettext.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
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

#include <xti.h>
#include <sys/xti.h>
#include <sys/xti_ip.h>
#include <sys/xti_tcp.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

#include <sockdb.h>
#include "sockpath.h"

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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netconfig.h>
#include <netdir.h>
#include <sockdb.h>
#include <sys/sockmod.h>

struct _so_user {
	pthread_rwlock_t lock;		/* lock for this structure */
	int refs;			/* number of references to this structure */
	int flags;			/* user flags */
	struct netconfig *nc;		/* netconfig pointer */
	struct si_udata info;		/* information structure */
};

#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif

static struct _so_user *_so_fds[OPEN_MAX] = { NULL, };

#if 0
static pthread_rwlock_t __so_fd_lock = PTHREAD_RWLOCK_INITIALIZER;

static void
__so_list_unlock(void *ignore)
{
	pthread_rwlock_unlock(&__so_fd_lock);
}

static void
__so_putuser(void *arg)
{
	int fd = *(int *) arg;
	struct _so_user *user = _so_fds[fd];

	pthread_rwlock_unlock(&user->lock);
	__so_list_unlock(NULL);
}

static __hot struct _so_user *
__so_getuser(int fd)
{
	struct _so_user *user;
	int err;

	if (unlikely((err = pthread_rwlock_rdlock(&__so_fd_lock))))
		goto list_lock_error;
	if (unlikely(0 > fd) || unlikely(fd >= OPEN_MAX))
		goto badf;
	if (unlikely(!(user = _so_fds[fd])))
		goto badf;
	if (unlikely((err = pthread_rwlock_wrlock(&user->lock))))
		goto user_lock_error;
	return (user);
      user_lock_error:
	errno = err;
	__so_list_unlock(NULL);
	goto error;
      badf:
	errno = EBADF;
	goto error;
      list_lock_error:
	errno = err;
	goto error;
      error:
	return (NULL);
}
#endif

/*
 *  Socket to Path mapping.
 *  =======================
 *  Here we take two approaches to mapping domain, type, and protocol arguments of the socket(3) or
 *  socketpair(3) call into a path: one uses the older netconfig(3) approach (and we now have a
 *  netconfig(3) library in the strxnet package), and the other uses the newer sock2path(5)
 *  approach, but does not maintain the mapping in the kernel.  We first try the sock2path(5)
 *  approach and then try the netconfig(3) approach.
 */

/* mapping from protocol family to netconfig(5) protofmly */
static const char *__so_family_protofmly[PF_MAX] = {
	[PF_UNSPEC] = NULL,
	[PF_LOCAL] = NC_LOOPBACK,
	[PF_INET] = NC_INET,
#ifdef NC_AX25
	[PF_AX25] = NC_AX25,
#endif
#ifdef NC_IPX
	[PF_IPX] = NC_IPX,
#endif
	[PF_APPLETALK] = NC_APPLETALK,
#ifdef NC_NETROM
	[PF_NETROM] = NC_NETROM,
#endif
#ifdef NC_BRIDGE
	[PF_BRIDGE] = NC_BRIDGE,
#endif
#ifdef NC_ATMPVC
	[PF_ATMPVC] = NC_ATMPVC,
#endif
	[PF_X25] = NC_X25,
	[PF_INET6] = NC_INET6,
#ifdef NC_ROSE
	[PF_ROSE] = NC_ROSE,
#endif
	[PF_DECnet] = NC_DECNET,
#ifdef NC_NETBEUI
	[PF_NETBEUI] = NC_NETBEUI,
#endif
#if 0
	[PF_SECURITY] = NULL,
	[PF_KEY] = NULL,
	[PF_NETLINK] = NULL,
	[PF_ROUTE] = NULL,
#endif
	[PF_PACKET] = NC_NIT,
#ifdef NC_ASH
	[PF_ASH] = NC_ASH,
#endif
#ifdef NC_ECONET
	[PF_ECONET] = NC_ECONET,
#endif
#ifdef NC_ATMSVC
	[PF_ATMSVC] = NC_ATMSVC,
#endif
	[PF_SNA] = NC_SNA,
#ifdef NC_IRDA
	[PF_IRDA] = NC_IRDA,
#endif
#ifdef NC_PPPOX
	[PF_PPPOX] = NC_PPPOX,
#endif
#ifdef NC_WANPIPE
	[PF_WANPIPE] = NC_WANPIPE,
#endif
#ifdef NC_BLUETOOTH
	[PF_BLUETOOTH] = NC_BLUETOOTH,
#endif
};

static int
__so_map_socket_to_netconfig(int domain, int type, int protocol,
			     int *sem1, int *sem2, const char **family, const char **proto)
{
	switch (type) {
#ifdef SOCK_STREAMS_ORD
		/* Some implementations actually defined a SOCK_STREAM_ORD here. */
	case SOCK_STREAMS_ORD:
		*sem1 = NC_TPI_COTS_ORD;
		break;
#endif				/* SOCK_STREAMS_ORD */
	case SOCK_STREAM:
#ifdef SOCK_STREAMS_ORD
		*sem1 = NC_TPI_COTS;
#else				/* SOCK_STREAMS_ORD */
		*sem1 = NC_TPI_COTS_ORD;
		*sem2 = NC_TPI_COTS;
#endif				/* SOCK_STREAMS_ORD */
		break;
	case SOCK_DGRAM:
		*sem1 = NC_TPI_CLTS;
		break;
	case SOCK_RAW:
	case SOCK_PACKET:
		*sem1 = NC_TPI_RAW;
		break;
#ifdef NC_TPI_COTS_PKT
	case SOCK_SEQPACKET:
		*sem1 = NC_TPI_COTS_PKT;
		break;
#endif
	default:
		/* cannot handle things like SOCK_SEQPACKET */
		return (-1);
	}
	if (domain < PF_MAX)
		*family = __so_family_protofmly[domain];
	if (*family == NULL && domain != PF_UNSPEC)
		return (-1);
	switch (domain) {
	case PF_INET:
	case PF_INET6:
		switch (protocol) {
		case IPPROTO_TCP:
			*proto = NC_TCP;
			break;
		case IPPROTO_UDP:
			*proto = NC_UDP;
			break;
		case IPPROTO_ICMP:
			*proto = NC_ICMP;
			break;
#if defined NC_SCTP && defined IPPROTO_SCTP
		case IPPROTO_SCTP:
			*proto = NC_SCTP;
			break;
#endif
		}
	}
	return (0);
}

/* netconfig(3) fallback approach */
static char *
__so_socket_path_netconfig(int domain, int type, int protocol)
{
	int semantics1 = 0, semantics2 = 0;
	const char *proto = NULL;
	const char *family = NULL;
	struct netconfig *nc;
	char *result = NULL;

	if (__so_map_socket_to_netconfig(domain, type, protocol,
					 &semantics1, &semantics2, &family, &proto) == -1)
		return (NULL);

	if ((nc = getnetconfigent(proto)) != NULL) {
		if ((result = strdup(nc->nc_device)) == NULL)
			nc_error = NC_NOMEM;
		freenetconfigent(nc);
	}
	if (!result) {
		nc_perror("socklib");
	}
	return (result);
}

static char *
__so_socket_path_sock2path(int domain, int type, int protocol)
{
	struct sockpath *sp;
	char *result = NULL;

	if ((sp = getsockpathent(domain, type, protocol)) != NULL) {
		if ((result = strdup(sp->sp_path)) == NULL)
			sp_error = SP_NOMEM;
		freesockpathent(sp);
	}
	if (!result) {
		sp_perror("socklib");
	}
	return (result);
}

static char *
__so_socket_path(int domain, int type, int protocol)
{
	char *result;

	if ((result = __so_socket_path_netconfig(domain, type, protocol)) != NULL)
		return (result);
	if ((result = __so_socket_path_sock2path(domain, type, protocol)) != NULL)
		return (result);
	return (NULL);
}

extern int __libc_socket(int, int, int);

int
__so_socket(int domain, int type, int protocol)
{
	char *device;
	int socket;
	int err;

	if ((device = __so_socket_path(domain, type, protocol)) == NULL)
		return (__libc_socket(domain, type, protocol));
	if ((socket = open(device, O_RDWR)) == -1) {
		perror("socklib");
		return (-1);
	}
	if (ioctl(socket, I_PUSH, "sockmod") == -1) {
		err = errno;
		perror("socklib");
		close(socket);
		errno = err;
		return (-1);
	}
	return (socket);
}

__asm__(".symver __so_socket,socket@@SOCKLIB_1.0");

/*
 *  "Our solution to this discrepancy relies on the ability to unbind an endpoint after it has first
 *   been bound.  When an application calls bind, socklib specifies a backlog of zero to the
 *   transport provider.  (A value of zero is normal for applications that do not wish to receive
 *   connect indications.)  If the application subsequently calls listen with a non-zero backlog
 *   value then the request translates to an ioctl that sockkmod services. [SI_LISTEN]  When sockmod
 *   receives the ioctl message, it first unbinds the endpoint, and then rebinds it to the same
 *   address, specifying the new backlog value.  The fact that in some cases the endpoint ahs to be
 *   unbind and then rebound introduces a window in which another application could request the same
 *   address.  HOwever, this window is small and we consider it acceptable." -- Implementing
 *   Berkeley Sockets in UNIX(R) System V Release 4.
 */
int
_so_listen(int fd, int backlog)
{
	return (0);
}

int
__so_sockatmark(int fd)
{
	return (0);
}

int __libc_setsockopt(int, int, int, const void *, socklen_t);

static int
__so_setoption(int s, t_scalar_t level, t_scalar_t name, const void *optptr, size_t optlen)
{
	struct {
		struct t_opthdr hdr;
		char optval[64];
	} opt;
	struct t_optmgmt opts = {
		{
		 .maxlen = sizeof(opt),
		 .len = optlen,
		 .buf = opt.optval,
		 }
		, T_NEGOTIATE,
	};
	int retval;

	retval = t_optmgmt(s, &opts, &opts);
	if (retval == -1) {
		switch (t_errno) {
		case TBADF:
			errno = EBADF;
			break;
		default:
		case TBADFLAG:
		case TBADOPT:
		case TBUFOVFLW:
		case TNOTSUPPORT:
		case TOUTSTATE:
			errno = EINVAL;
			break;
		case TPROTO:
			errno = EPROTO;
			break;
		case TSYSERR:
			break;
		}
		return (-1);
	}
	return (retval);
}

int
__so_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
	struct _so_user *so;
	int option, val, rval;

	if ((so = _so_fds[s]))
		return (__libc_setsockopt(s, level, optname, optval, optlen));

	switch (level) {
	case SOL_SOCKET:
		switch (optname) {
		case SO_DEBUG:	/* POSIX */
			/* Handle with XTI_GENERIC/XTI_DEBUG. */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_REUSEADDR:	/* POSIX */
			/* The problem here is that this must be converted into a transport
			   specific option, such as T_IP_REUSEADDR for inet protocols.  We might be
			   able to use netdir_options(3) for this.  Another way is to create an XTI
			   generic option, such as XTI_REUSEADDR. A third way is to create a sockio
			   input-output control. All but the first way requires the cooperation of
			   the transport provider.  */
			if (optval == NULL) {
				errno = EINVAL;
				return (-1);
			}
			val = (*(int *) optval);
#if defined ND_SET_REUSEADDR && defined ND_CLEAR_REUSEADDR
			if (so->nc) {
				option = val ? ND_SET_REUSEADDR : ND_CLEAR_REUSEADDR;
				rval = netdir_options(so->nc, option, s, NULL);
				if (rval == ND_OK || nd_error != ND_NOCTRL)
					return (rval);
			}
#endif				/* defined ND_SET_REUSEADDR && defined ND_CLEAR_REUSEADDR */
#if defined XTI_GENERIC && defined XTI_REUSEADDR
			option = val ? T_YES : T_NO;
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_REUSEADDR, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_REUSEADDR */
#if defined SIOCREUSEADDR
			rval = ioctl(s, SIOCREUSEADDR, val);
			if (rval == 0 || errno != EINVAL)
				return (rval);
#endif				/* defined SIOCREUSEADDR */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_TYPE:	/* POSIX *//* getsockopt() only */
		case SO_ERROR:	/* POSIX *//* getsockopt() only */
			/* These are local anyway. */
			errno = EINVAL;
			return (-1);
		case SO_DONTROUTE:	/* POSIX */
			/* The problem here is that this must be converted into a transport
			   specific option, such as T_IP_DONTROUTE for inet protocols.  We might be 
			   able to use netdir_options(3) for this (but, it doe not really have a
			   routing option, but we could add one).  The other way would be to create 
			   a generic XTI option such as XTI_DONTROUTE. A third way is to create a
			   sockio input-output control. All but the first way requires the
			   cooperation of the transport provider.  */
			if (optval == NULL) {
				errno = EINVAL;
				return (-1);
			}
			val = (*(int *) optval);
			/** AF_INET and AF_INET6 is handled directly, which is what the purpose of
			  * the SO_DONTROUTE option was in the first place. */
			if (so->info.sockparams.sp_family == AF_INET
			    || so->info.sockparams.sp_family == AF_INET6) {
				option = val ? T_YES : T_NO;
				optval = &option;
				optlen = sizeof(t_scalar_t);
				rval = __so_setoption(s, T_INET_IP, T_IP_DONTROUTE, optval, optlen);
				if (rval == 0 || errno != EPERM)
					return (rval);
			}
#if defined ND_SET_DONTROUTE && defined ND_CLEAR_DONTROUTE
			if (so->nc) {
				option = val ? ND_SET_DONTROUTE : ND_CLEAR_DONTROUTE;
				rval = netdir_options(so->nc, option, s, NULL);
				if (rval == ND_OK || nd_error != ND_NOCTRL)
					return (rval);
			}
#endif				/* defined ND_SET_DONTROUTE && defined ND_CLEAR_DONTROUTE */
#if defined XTI_GENERIC && defined XTI_DONTROUTE
			option = val ? T_YES : T_NO;
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_DONTROUTE, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_DONTROUTE */
#if defined SIOCDONTROUTE
			rval = ioctl(s, SIOCDONTROUTE, val);
			if (rval == 0 || errno != EINVAL)
				return (rval);
#endif				/* defined SIOCDONTROUTE */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_BROADCAST:	/* POSIX *//* SOCK_DGRAM only */
			/* The problem here is that this must be converted into a transport
			   specific option, such as T_IP_BROADCAST for inet protocols.  We might be
			   able to use netdir_options(3) for this.  Another way is to create an XTI
			   generic option, such as XTI_BROADCAST. A third way is to create a sockio
			   input-output control. All but the first way requires the cooperation of
			   the transport provider.  */
			if (optval == NULL) {
				errno = EINVAL;
				return (-1);
			}
			val = (*(int *) optval);
			/** AF_INET and AF_INET6 is handled directly, which is what the purpose of
			  * the SO_BROADCAST option was in the first place. */
			if (so->info.sockparams.sp_family == AF_INET
			    || so->info.sockparams.sp_family == AF_INET6) {
				option = val ? T_YES : T_NO;
				optval = &option;
				optlen = sizeof(t_scalar_t);
				rval = __so_setoption(s, T_INET_IP, T_IP_BROADCAST, optval, optlen);
				if (rval == 0 || errno != EPERM)
					return (rval);
			}
#if defined ND_SET_BROADCAST && defined ND_CLEAR_BROADCAST
			if (so->nc) {
				option = val ? ND_SET_BROADCAST : ND_CLEAR_BROADCAST;
				rval = netdir_options(so->nc, option, s, NULL);
				if (rval == ND_OK || nd_error != ND_NOCTRL)
					return (rval);
			}
#endif				/* defined ND_SET_BROADCAST && defined ND_CLEAR_BROADCAST */
#if defined XTI_GENERIC && defined XTI_BROADCAST
			option = val ? T_YES : T_NO;
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_BROADCAST, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_BROADCAST */
#if defined SIOCBROADCAST
			rval = ioctl(s, SIOCBROADCAST, val);
			if (rval == 0 || errno != EINVAL)
				return (rval);
#endif				/* defined SIOCBROADCAST */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_SNDBUF:	/* POSIX */
#if defined XTI_GENERIC && defined XTI_SNDBUF
			/** @def SO_SNDBUF
			  * Handle with XTI_GENERIC/XTI_SNDBUF. */
			option = val;
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_SNDBUF, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_SNDBUF */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_RCVBUF:	/* POSIX */
#if defined XTI_GENERIC && defined XTI_RCVBUF
			/** @def SO_RCVBUF
			  * Handle with XTI_GENERIC/XTI_RCVBUF. */
			option = val;
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_RCVBUF, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_RCVBUF */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_KEEPALIVE:	/* POSIX */
			/** @def SO_KEEPALIVE
			  *   The problem here is that this must be converted into a transport
			  *   specific option, such as T_TCP_KEEPALIVE for tcp protocol.  We might
			  *   be able to use netdir_options(3) for this (but, it doe not really have
			  *   a keepalive option, but we could add one).  The other way would be to
			  *   create a generic XTI option such as XTI_KEEPALIVE. A third way is to
			  *   create a sockio input-output control. All but the first way requires
			  *   the cooperation of the transport provider.  */
			if (optval == NULL) {
				errno = EINVAL;
				return (-1);
			}
			val = (*(int *) optval);
#if defined NC_INET && defined NC_INET6
			/** @def SO_KEEPALIVE
			  *   AF_INET and AF_INET6 and handled directly for IPPROTO_TCP or default
			  *   protocol (0) for SOCK_STREAM.  These are converted directly into a
			  *   T_INET_TCP, T_TCP_KEEPALIVE setting. */
			if ((so->info.sockparams.sp_family == AF_INET
			     || so->info.sockparams.sp_family == AF_INET6)
			    && (so->info.sockparams.sp_protocol == IPPROTO_TCP
				|| (so->info.sockparams.sp_protocol == 0
				    && so->info.sockparams.sp_type == SOCK_STREAM))) {
				optval = &option;
				optlen = sizeof(struct t_kpalive);
				rval =
				    __so_setoption(s, T_INET_TCP, T_TCP_KEEPALIVE, optval, optlen);
				if (rval == 0 || errno != EPERM)
					return (rval);
			}
#endif				/* defined NC_INET && defined NC_INET6 */
#if defined ND_SET_KEEPALIVE && defined ND_CLEAR_KEEPALIVE
			if (so->nc) {
				option = val ? ND_SET_KEEPALIVE : ND_CLEAR_KEEPALIVE;
				rval = netdir_options(so->nc, option, s, NULL);
				if (rval == ND_OK || nd_error != ND_NOCTRL)
					return (rval);
			}
#endif				/* defined ND_SET_KEEPALIVE && defined ND_CLEAR_KEEPALIVE */
#if defined XTI_GENERIC && defined XTI_KEEPALIVE
			optval = &option;
			optlen = sizeof(struct t_kpalive);
			rval = __so_setoption(s, XTI_GENERIC, XTI_KEEPALIVE, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_KEEPALIVE */
#if defined SIOCKEEPALIVE
			rval = ioctl(s, SIOCKEEPALIVE, val);
			if (rval == 0 || errno != EINVAL)
				return (rval);
#endif				/* defined SIOCKEEPALIVE */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_OOBINLINE:	/* POSIX */
			/* This option can be hadled locally. */
			errno = ENOPROTOOPT;
			return (-1);
#if 0
		case SO_NO_CHECK:
#endif
		case SO_PRIORITY:	/* Linux Specific? */
			/* The problem here is that this must be converted into a transport
			   specific option, such as T_IP_TOS inet protocol.  We might be able to
			   use netdir_options(3) for this (but, it doe not really have a priority
			   option, but we could add one).  The other way would be to create a
			   generic XTI option such as XTI_PRIORITY. A third way is to create a
			   sockio input-output control. All but the first way requires the
			   cooperation of the transport provider.  */
#if defined ND_SET_PRIORITY
			if (so->nc) {
				rval = netdir_options(so->nc, ND_SET_PRIORITY, s, (char *) &val);
				if (rval == ND_OK || nd_error != ND_NOCTRL)
					return (rval);
			}
#endif				/* defined ND_SET_PRIORITY */
#if defined XTI_GENERIC && defined XTI_PRIORITY
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_PRIORITY, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_PRIORITY */
#if defined SIOCPRIORITY
			rval = ioctl(s, SIOCPRIORITY, val);
			if (rval == 0 || errno != EINVAL)
				return (rval);
#endif				/* defined SIOCPRIORITY */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_LINGER:	/* POSIX */
			/* Handle with XTI_GENERIC/XTI_LINGER. */
			errno = ENOPROTOOPT;
			return (-1);
#if 0
		case SO_BSDCOMPAT:
		case SO_PASSCRED:
		case SO_PEERCRED:
#endif
		case SO_RCVLOWAT:	/* POSIX */
			/** @def SO_RCVLOWAT
			  * Handle with XTI_GENERIC/XTI_RCVLOWAT. */
#if defined XTI_GENERIC && defined XTI_RCVLOWAT
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_RCVLOWAT, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_RCVLOWAT */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_SNDLOWAT:	/* POSIX */
			/** @def SO_SNDLOWAT
			  * Handle with XTI_GENERIC/XTI_SNDLOWAT. */
#if defined XTI_GENERIC && defined XTI_SNDLOWAT
			optval = &option;
			optlen = sizeof(t_scalar_t);
			rval = __so_setoption(s, XTI_GENERIC, XTI_SNDLOWAT, optval, optlen);
			if (rval == 0 || errno != EPERM)
				return (rval);
#endif				/* defined XTI_GENERIC && defined XTI_SNDLOWAT */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_RCVTIMEO:	/* POSIX */
			/* This option can be handled locally. */
			errno = ENOPROTOOPT;
			return (-1);
		case SO_SNDTIMEO:	/* POSIX */
			/* This option can be handled locally. */
			errno = ENOPROTOOPT;
			return (-1);
#if 0
		case SO_SECURITY_AUTHENTICATION:
		case SO_SECURITY_ENCRYPTION_TRANSPORT:
		case SO_SECURITY_ENCRYPTION_NETWORK:
		case SO_BINDTODEVICE:
		case SO_ATTACH_FILTER:
		case SO_DETACH_FILTER:
		case SO_PEERNAME:
		case SO_TIMESTAMP:
		case SO_ACCEPTCONN:
#endif
		default:
			errno = ENOPROTOOPT;
			return (-1);
		}
	}
	errno = ENOPROTOOPT;
	return (-1);
}

__asm__(".symver __so_setsockopt,setsockopt@@SOCKLIB_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
