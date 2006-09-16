/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#define _REENTRANT

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stropts.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netconfig.h>

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
#ifdef NC_SCTP
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
	return (NULL);
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
