/* internet.h - TCP/IP abstractions */

/* 
 * $Header: /f/iso/h/RCS/internet.h,v 5.0 88/07/21 14:39:00 mrose Rel $
 *
 *
 * $Log$
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

/*    SOCKETS */

#include "sys/socket.h"
#include <HULA/solsocket.h>
#include <HULA/ex_errno.h>


#ifndef	SOMAXCONN
#define	SOMAXCONN	5
#endif

#if	!defined(S5R3) || !defined(WIN)
#include "netinet/in.h"
#else
#include "sys/in.h"
#endif


int	start_tcp_client ();
int	start_tcp_server ();

#ifdef	NATIVE
int	join_tcp_client ();
#define	join_tcp_server(s,t) \
	connect ((s), (struct sockaddr *) (t), sizeof *(t))

#define	read_tcp_socket		read
#define	write_tcp_socket	write
#define	close_tcp_socket	close
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

int	read_tcp_socket ();
int	write_tcp_socket ();
int	close_tcp_socket ();

int	select_tcp_socket ();

/*    UDP */

#define	MAXUDP	8192

int	start_udp_server ();
#define	start_udp_client	start_udp_server

#define	join_udp_server(fd,sock)	join_udp_aux ((fd), (sock), 0)
#define	join_udp_client(fd,sock)	join_udp_aux ((fd), (sock), 1)
int	join_udp_aux ();

int	read_udp_socket ();
int	write_udp_socket ();
int	close_udp_socket ();
int	select_udp_socket ();


/*    NETDB */

#ifdef	NATIVE
#include <netdb.h>
#endif


#ifdef	EXOS
struct hostent {
    char   *h_name;		/* official name */
    char  **h_aliases;		/* alias list */
    int     h_addrtype;		/* address type: AF_INET */
    int     h_length;		/* address length: sizeof (u_long) == 4 */
    char   *h_addr;		/* address value: (struct in_addr *) */
};

struct servent {
    char   *s_name;		/* official name */
    char  **s_aliases;		/* alias list */
    int     s_port;		/* port number */
    char   *s_proto;		/* protocol beneath service */
};


struct hostent *gethostbyaddr (), *gethostbyname ();
struct servent *getservbyname ();
#endif


struct hostent *gethostbystring ();

/*    INET */

/* under BSD42, we could simply include <arpa/inet.h> instead.  However,
   the definition of inet_addr contained therein causes problems with some
   compilers. */

char   *inet_ntoa ();
#ifndef	DG
u_long	inet_addr ();
#else
struct in_addr inet_addr ();
#endif
#endif
