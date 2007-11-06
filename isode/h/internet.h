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

#ifndef __ISODE_INTERNET_H__
#define __ISODE_INTERNET_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * This is another file that should be out and out removed.  Again, autoconf configuration tests
 * should be relied upon to perform the replacement and adaptation between POSIX and whatever.  The
 * code file should only use POSIX facilities where possible. -bb (2007-10-21)
 */

/* internet.h - TCP/IP abstractions */

/* 
 * Header: /xtel/isode/isode/h/RCS/internet.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 *
 * Log: internet.h,v
 * Revision 9.0  1992/06/16  12:17:57  isode
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

#ifndef	_INTERNET_
#define	_INTERNET_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif

/* SOCKETS */

#include <sys/socket.h>

#ifndef	SOMAXCONN
#define	SOMAXCONN	5
#endif

#if	defined(WIN) || defined(WINTLI)
#include "sys/in.h"
#else

#ifdef SVR4			/* Has a different defn for NULLVP */
#ifdef NULLVP
#undef NULLVP
#endif
#endif				/* SVR4 */

#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef SVR4			/* Put back the ISODE defn */
#ifdef NULLVP
#undef NULLVP
#endif
#define NULLVP ((char **) 0)
#endif				/* SVR4 */

#endif				/* defined(WIN) || defined(WINTLI) */

int start_tcp_client();
int start_tcp_server();

#if defined(SOCKETS) && !defined(TLI_TCP)
int join_tcp_client();
int join_tcp_server();

#define	read_tcp_socket		read
#define	write_tcp_socket	write
#define	select_tcp_socket	selsocket
#endif

#ifdef TLI_TCP
int join_tcp_client();
int join_tcp_server();
int read_tcp_socket();
int write_tcp_socket();
int close_tcp_socket();

#define	select_tcp_socket	selsocket
#endif

#ifdef	WINTLI
int join_tcp_client();
int join_tcp_server();

#define	select_tcp_socket	selsocket
#endif

#ifdef	EXOS
#ifdef	SYS5
#define	join_tcp_client(s,f) \
	(accept ((s), (struct sockaddr *) (f)) != NOTOK ? (s) : NOTOK)
#define	join_tcp_server(s,t)	connect ((s), (struct sockaddr *) (t))

#define	read_tcp_socket		read
#define	write_tcp_socket	write
#define	close_tcp_socket	close
#define	select_tcp_socket	selsocket
#endif
#endif

#ifndef read_tcp_socket
int read_tcp_socket();
#endif
#ifndef write_tcp_socket
int write_tcp_socket();
#endif
#ifndef close_tcp_socket
int close_tcp_socket();
#endif

int select_tcp_socket();

/* UDP */

#ifdef	SOCKETS
#ifndef	_DGRAM_
#include "dgram.h"
#endif

int start_udp_server();

#define	start_udp_client	start_udp_server

#define	join_udp_server(fd,sock) \
		join_dgram_aux ((fd), (struct sockaddr *) (sock), 0)
#define	join_udp_client(fd,sock) \
		join_dgram_aux ((fd), (struct sockaddr *) (sock), 1)

#define	read_udp_socket		read_dgram_socket
#define	write_udp_socket	write_dgram_socket
#define	close_udp_socket	close_dgram_socket

#define	select_udp_socket	select_dgram_socket
#define	check_udp_socket	check_dgram_socket
#endif

/* NETDB */

#if	defined(SOCKETS) || defined (WINTLI) || defined (WIN)
#if     defined(_AIX) && defined(n_name)	/* different def in nlist.h clashes with netdb.h */
#undef n_name
#endif
#include <netdb.h>
#if   defined(_AIX) && defined(SYS5)
#define n_name          _n._n_name
#endif
#endif

#if	defined(BIND) && !defined(h_addr)
#define	h_addr	h_addr_list[0]
#endif

#define	inaddr_copy(hp,sin) \
    bcopy ((hp) -> h_addr, (char *) &((sin) -> sin_addr), (hp) -> h_length)

#ifdef	EXOS
struct hostent {
	char *h_name;			/* official name */
	char **h_aliases;		/* alias list */
	int h_addrtype;			/* address type: AF_INET */
	int h_length;			/* address length: sizeof (unsigned long) == 4 */
	char *h_addr;			/* address value: (struct in_addr *) */
};

struct servent {
	char *s_name;			/* official name */
	char **s_aliases;		/* alias list */
	int s_port;			/* port number */
	char *s_proto;			/* protocol beneath service */
};

struct hostent *gethostbyaddr(), *gethostbyname();
struct servent *getservbyname();
#endif

struct hostent *gethostbystring();

/* INET */

/* under BSD42, we could simply include <arpa/inet.h> instead.  However,
   the definition of inet_addr contained therein causes problems with some
   compilers. */

#ifndef linux
char *inet_ntoa();

#ifndef	DG
unsigned long inet_addr();

#ifndef	HPUX
unsigned long inet_network();
#else
int inet_network();
#endif
#else
struct in_addr inet_addr(), inet_network();
#endif
#endif
#endif

#endif				/* __ISODE_INTERNET_H__ */
