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

/*
 *  Socket to Path mapping.
 *  =======================
 *  Here we take two approaches to mapping domain, type, and protocol arguments of the socket(3) or
 *  socketpair(3) call into a path: one uses the older netconfig(3) approach (and we now have a
 *  netconfig(3) library in the strxnet package), and the other uses the newer sock2path(5)
 *  approach, but does not maintain the mapping in the kernel.  We first try the sock2path(5)
 *  approach and then try the netconfig(3) approach.
 */

/* netconfig(3) fallback approach */
static char *
__so_socket_path_netconfig(int domain, int type, int protocol)
{
	const char *semantics1, *semantics2;
	const char *protocol1, *protocol2;

	switch (type) {
#ifdef SOCK_STREAMS_ORD
		/* Some implementations actually defined a SOCK_STREAM_ORD here. */
	case SOCK_STREAMS_ORD:
		semantics1 = NC_TPI_COTS_ORD;
		semantics2 = NULL;
		break;
#endif				/* SOCK_STREAMS_ORD */
	case SOCK_STREAM:
#ifdef SOCK_STREAMS_ORD
		semantics1 = NC_TPI_COTS;
		semantics2 = NULL;
#else				/* SOCK_STREAMS_ORD */
		semantics1 = NC_TPI_COTS_ORD;
		semantics2 = NC_TPI_COTS;
#endif				/* SOCK_STREAMS_ORD */
		break;
	case SOCK_DGRAM:
		semantics1 = NC_TPI_CLTS;
		semantics2 = NULL;
		break;
	case SOCK_RAW:
		semantics1 = NC_TPI_RAW;
		semantics2 = NULL;
		break;
	default:
		/* cannot handle things like SOCK_SEQPACKET */
		return (NULL);
	}
}

void *__so_setsock2path(void);
struct sockent *__so_getsock2path(void *handle);
int __so_endsock2path(void *handle);

void *
__so_setsock2path(void)
{
}

__asm__(".symver __so_setsock2path,setsock2path@@SOCKLIB_1.0");

struct sockent *
__so_getsock2path(void *handle)
{
}

__asm__(".symver __so_getsock2path,getsock2path@@SOCKLIB_1.0");

int
__so_endsock2path(void *handle)
{
}

__asm__(".symver __so_endsock2path,endsock2path@@SOCKLIB_1.0");

static char *
__so_socket_path_sock2path(int domain, int type, int protocol)
{
}

static char *
__so_socket_path(int domain, int type, int protocol)
{
}

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
