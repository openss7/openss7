/*****************************************************************************

 @(#) $RCSfile: server.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:14 $

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

 Last Modified $Date: 2008-06-18 16:43:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: server.c,v $
 Revision 0.9.2.1  2008-06-18 16:43:14  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ident "@(#) $RCSfile: server.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:14 $"

static char const ident[] = "$RCSfile: server.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:14 $";

/*
 * From Tru64 UNIX Network Programmer's Guide AR-H9UE-TE.
 *
 * Example B-3 implements a server using the XTI library for network
 * communication.  It is an alternative design for a communication program
 * that makes it transport independent.  Compare this program with the socket
 * server program in Section B.1.1.  This program has the same limitations
 * described at the beginning of the appendix.
 */

/*
 * This file contains the main XTI server code
 * for a connection-oriented mode of communication.
 *
 * Usage: xtiserver
 *
 */
#include "server.h"

char *parse(char *);
struct transaction *verifycustomer(char *, int, char *);

int
main(int argc, char *argv[])
{
	int xtifd;
	int newxtifd;
	struct sockaddr_in serveraddr;
	struct hostent *he;
	int pid;
	struct t_bind *bindreqp;
	struct t_call *call;

	signal(SIGCHLD, SIG_IGN);

	/* ---> [1] <---- */
	if ((xtifd = t_open("/dev/streams/xtiso/tcp+", O_RDWR, NULL)) < 0) {
		xerror("xti_open", xtifd);
		exit(1);
	}
	bzero((char *) &serveraddr, sizeof(struct sockaddr_in));
	/* ---> [2] <---- */
	serveraddr.sin_family = AF_INET;
	/* ---> [3] <---- */
	serveraddr.sin_addr.s_addr = htonl(INARRY_ANY);
	/* ---> [4] <---- */
	serveraddr.sin_port = htons(SERVER_PORT);

	/* allocate structures for the t_bind and t_listen call */
	if (((bindreqp = (struct t_bind *) t_alloc(xtifd, T_BIND, T_ALL)) == NULL) ||
	    ((call = (struct t_call *) t_alloc(xtifd, T_CALL, T_ALL)) == NULL)) {
		xerror("xti_alloc", xtifd);
		exit(3);
	}
	bindreqp->addr.buf = (char *) &serveraddr;
	bindreqp->addr.len = sizeof(serveraddr);

	/* 
	 * Sepcify how many pending connections can be matinained, until
	 * finish accept processing.
	 */

	/* ---> [5] <---- */
	bindreqp->qlen = 8;

	/* ---> [6] <---- */
	if (t_bind(xtifd, bindreqp, (struct t_bind *) NULL) < 0) {
		xerror("xti_bind", xtifd);
		exit(4);
	}

	/* 
	 * Now that the socket is ready to accept connections.  For each
	 * connection, fork a child process in charge of the session, and then
	 * resume accepting connections.
	 */

	while (1) {
		/* ---> [7] <---- */
		if (t_listen(xtifd, &call) < 0) {
			if (errno == EINTR) {
				printf("Bye...\n");
				exit(0);
			} else {
				xerror("t_listen", xtifd);
				exit(4);
			}
		}

		/* 
		 * Create a new transport endpoint on which to accept a
		 * connection.
		 */
		/* ---> [8] <---- */
		if ((newxtifd = t_open("/dev/streams/xtiso/tcp+", O_RDWR, NULL)) < 0) {
			xerror("xti_newopen", xtifd);
			exit(5);
		}
		/* accept connection */
		/* ---> [9] <---- */
		if (t_accept(xtifd, newxtifd, call) < 0) {
			xerror("xti_accept", xtifd);
			exit(7);
		}
		pid = fork();

		switch (pid) {
		case -1:	/* error */
			xerror("dosession_fork", xtifd);
			break;
		default:
			t_close(newxtifd);
			break;
		case 0:	/* child */
			t_close(xtifd);
			transactions(newxtifd);
			if ((t_free((char *) bindreqp, T_BIND) < 0) ||
			    (t_free((char *) call, T_CALL) < 0)) {
				xerror("xti_free", xtifd);
				exit(3);
			}
			t_close(newxtifd);
			return (0);
		}

	}
}

/* ---> [10] <---- */
int
transactions(int fd)
{
	int bytes;
	char *reply;
	int dcount;
	int flags;
	char datapipe[MAXBUFSIZE + 1];

	/* 
	 * Look at the data buffer and parse commands, if more data required
	 * go get it.
	 * Since the protocol is SOCK_STREAM oriented, no data boundaries will
	 * be preserved.
	 *
	 */
	while (1) {
		/* ---> [11] <---- */
		if ((dcount = t_rcv(fd, datapipe, MAXBUFSIZE, &flags)) < 0) {
			/* if disconnected bit a goodbye */
			if (t_errno == TLOOK) {
				int tmp = t_look(fd);

				if (tmp != T_DISCONNECT) {
					t_scope(tmp);
				} else {
					exit(0);
				}
			}
			xerror("transactions_receive", fd);
			break;
		}
		if (dcount == 0) {
			/* consolidate all transactions */
			return (0);
		}
		datapipe[dcount] = ' ';
		if ((reply = parse(datapipe)) != NULL) {
			/* ---> [12] <---- */
			if (t_snd(fd, reply, strlen(reply), 0) < 0) {
				xerror("xti_send", fd);
				break;
			}
		}
	}
}

/*
 * [1]   The t_open() call specifies a device special file name; for example,
 *       /dev/streams/xtiso/tcp+.  This file name provides the necessary
 *       abstraction for the TCP transport protocol over IP.  Unlike the
 *       socket interface, where you specify the address family (for example,
 *       AF_INET), this information is already represented in the choice of
 *       the device special file.  The /dev/streams/xtio/tcp+ file implies
 *       both TCP transport and IP.  See the Chapter 5 for information about
 *       STREAMS devices.
 *
 *       As mentioned in Section B.1.1, if the OSI transport were available
 *       you would use a device such as /dev/streams/xtiso/cots.
 *
 *       Contrast the t_open() call with the socket call in Section B.1.1.
 *
 * [2]   Selection of the address depends on the choice of transport protocol.
 *       Note that in the socket example, the address family was the name as
 *       used in the socket system call.  With XTI, the choice is not obvious
 *       and you must know the appropriate mapping from the transport protocol
 *       to sockaddr.  See Chapter 3 for more information.
 *
 * [3]   INADDR_ANY signifies any attached interface adapter on the system.
 *       All numbers must be converted to the network format using appropriate
 *       macros.  See the following reference pages for more information:
 *       htonl(3), htons(3), ntohl(3), ntohs(3).
 *
 * [4]   SERVER_PORT is defined in the <common.h> header file.  It has a data
 *       type of short integer which helps identify the server process from
 *       other application processes.  Numbers from 0 to 1024 are reserved.
 *
 * [5]   Specify the number of pending connections the server can queue while
 *       it processes the last request.
 *
 * [6]   Bind the server's address with the t_bind() call.  The combination of
 *       the address and port number uniquely identify it on the network.
 *       After the server process' address is bound, the server profcess is
 *       registered on the system and can be identified by the lower level
 *       kernel functions to which to direct any requests.
 *
 * [7]   Listen for connection requests with the t_listen() function.
 *
 * [8]   Create a new transprot endpoint with another call to t_open()
 *       function.
 *
 * [9]   Accept the connection request with the t_accept() function.
 *
 * [10]  Each incoming message packet is accepted and passed to the parse
 *       function, which tracks the information provided (such as the
 *       merchant's login ID, password, and customer's credit card number).
 *       This process is repeated until the parse function identifies a
 *       complete transaction and returns a response packet to be sent to the
 *       client program.
 *
 *       The client program can send information packets in any order (and in
 *       one or more packets), so the parse function is designated to remember
 *       state information sufficient to deal with this unstructured message
 *       stream.
 *
 *       Since the program uses a connection-oriented protocol for data
 *       transfer, this function uses t_snd() and t_rcv() to send and receive
 *       data, respecitvely.
 *
 * [11]  Receive data with the t_rcv() function.
 *
 * [12]  Send data with the t_snd() function.
 *
 */
