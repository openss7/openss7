/*****************************************************************************

 @(#) $Id: etsi9.c,v 0.9.2.2 2001/04/27 13:48:44 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 1997-2001  Brian Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 Last Modified $Date: 2001/04/27 13:48:44 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: etsi9.c,v $
 Revision 0.9.2.2  2001/04/27 13:48:44  brian
 Initial revision

 Revision 0.1  2001/04/27 13:48:44  brian
 Initial revision

 *****************************************************************************/

static char const ident[] = "$Name:  $($Revision: 0.9.2.2 $) $Date: 2001/04/27 13:48:44 $";

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>

#ifndef IPPROTO_SCTP
#define IPPROTO_SCTP 132
#endif

int s[7];

struct sockaddr_in addr[] = {
	{ AF_INET, 0, { INADDR_ANY }, },
	{ AF_INET, 0, { INADDR_ANY }, },
	{ AF_INET, 0, { INADDR_ANY }, },
	{ AF_INET, 0, { INADDR_ANY }, },
	{ AF_INET, 0, { INADDR_ANY }, },
	{ AF_INET, 0, { INADDR_ANY }, },
	{ AF_INET, 0, { INADDR_ANY }, }
};

int show_error(const char* func)
{
	if ( errno == EAGAIN )
		return 0;
	perror(func);
	fprintf(stderr, "^^^^^^^\n");
	return -1;
}

void show_bind(int n)
{
	struct sockaddr_in usin[7] = { 0, };
	int len = 8*sizeof(struct sockaddr_in);
	int i, num;
	if ( (getsockname(s[n], (struct sockaddr *)usin, &len)) < 0 )
		return (void)show_error(__FUNCTION__);
	num = len/sizeof(struct sockaddr_in);
	fprintf(stderr, "socket %d bound to:",n);
	for ( i=0; i<num; i++ )
	{
		fprintf(stderr, " %s:%d", inet_ntoa(usin[i].sin_addr),
				ntohs(usin[i].sin_port));
	}
	fprintf(stderr, "\n");
}

void show_connect(int n)
{
	struct sockaddr_in usin[7] = { 0, };
	int len = 8*sizeof(struct sockaddr_in);
	int i, num;
	if ( (getpeername(s[n], (struct sockaddr *)usin, &len)) < 0 )
		return (void)show_error(__FUNCTION__);
	num = len/sizeof(struct sockaddr_in);
	fprintf(stderr, "socket %d connected to:",n);
	for ( i=0; i<num; i++ )
	{
		fprintf(stderr, " %s:%d", inet_ntoa(usin[i].sin_addr),
				ntohs(usin[i].sin_port));
	}
	fprintf(stderr, "\n");
}

int test_open(int n)
{
//	fprintf(stderr, "Opening socket %d\n", n);
	if ( (s[n] = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0 )
		return show_error(__FUNCTION__);
//	if ( fcntl(s[n], F_SETFL, O_NONBLOCK) < 0 )
//		return show_error(__FUNCTION__);
	return 0;
}

int test_multi_bind(int n, int a, int m)
{
	int i;

	fprintf(stderr, "Binding socket %d to", n);
	for ( i=0; i<m; i++ )
		fprintf(stderr, " %s:%d", inet_ntoa(addr[a+i].sin_addr),
				htons(addr[a+i].sin_port));
	fprintf(stderr, "\n");
	if ( (bind(s[n], (struct sockaddr *)&addr[a], m*sizeof(struct sockaddr_in))) < 0 )
		return show_error(__FUNCTION__);
	show_bind(n);
	return 0;
}

int test_listen(int n)
{
	fprintf(stderr, "Listening on socket %d\n", n);
	if ( (listen(s[n], 1)) < 0 )
		return show_error(__FUNCTION__);
	show_bind(n);
	return 0;
}

int test_connect(int n, int a, int m)
{
	int i;

	fprintf(stderr, "Connecting socket %d to", n);
	for ( i=0; i<m; i++ )
		fprintf(stderr, " %s:%d", inet_ntoa(addr[a+i].sin_addr),
				htons(addr[a+i].sin_port));
	fprintf(stderr, "\n");
	if ( (connect(s[n], (struct sockaddr *)&addr[a], m*sizeof(struct sockaddr_in))) < 0 )
		return show_error(__FUNCTION__);
	show_bind(n);
	show_connect(n);
	return 0;

}

int test_accept(int n, int m)
{
	struct sockaddr_in usin[7] = { 0, };
	int len = sizeof(usin);
	int i, num;

	fprintf(stderr, "Accepting on listening socket %d\n", n);
	if ( (s[m] = accept(s[n], (struct sockaddr *)usin, &len)) < 0 )
		return show_error(__FUNCTION__);
	num = len/sizeof(struct sockaddr_in);

	fprintf(stderr, "socket %d accepted from:",m);
	for ( i=0; i<num; i++ )
	{
		fprintf(stderr, " %s:%d", inet_ntoa(usin[i].sin_addr),
				ntohs(usin[i].sin_port));
	}
	fprintf(stderr, "\n");
	show_bind(m);
	show_connect(m);
	return 0;
}

int test_send(int n)
{
//	fprintf(stderr, "Sending on accepted socket %d\n", n);
//	if ( send(s[n], "This is a test!", 16, MSG_OOB|MSG_NOSIGNAL) < 0 )
	if ( send(s[n], "This is a test!", 16, MSG_NOSIGNAL) < 0 )
		return show_error(__FUNCTION__);
	return 0;
}

int test_read(int n)
{
	int len;
	char *buf[100];
//	fprintf(stderr, "Reading from connected socket %d\n", n);
	if ( (len = recv(s[n], buf, 100, MSG_WAITALL|MSG_NOSIGNAL)) < 0 )
		return show_error(__FUNCTION__);
//	fprintf(stderr, "Message[%d]: \"%16s\"\n", len, buf);
	return 0;
}

main() {
	int i;
	struct pollfd pfd[6] =
	{ { 0, POLLIN|POLLOUT|POLLERR|POLLHUP, 0 },
	  { 0, POLLIN|POLLOUT|POLLERR|POLLHUP, 0 },
	  { 0, POLLIN|POLLOUT|POLLERR|POLLHUP, 0 },
	  { 0, POLLIN|POLLOUT|POLLERR|POLLHUP, 0 },
	  { 0, POLLIN|POLLOUT|POLLERR|POLLHUP, 0 },
	  { 0, POLLIN|POLLOUT|POLLERR|POLLHUP, 0 } };

	inet_aton("192.168.9.1",	    &addr[0].sin_addr);
	inet_aton("192.169.9.1",	    &addr[1].sin_addr);
	inet_aton("192.168.19.1",	    &addr[2].sin_addr);
	inet_aton("192.169.19.1",	    &addr[3].sin_addr);
	inet_aton("192.168.19.1",	    &addr[4].sin_addr);
	inet_aton("192.169.19.1",	    &addr[5].sin_addr);
	inet_aton("127.0.0.1",	    &addr[6].sin_addr);

#define TEST_PORT 10000
	addr[0].sin_port = htons(2960);
	addr[1].sin_port = htons(2960);
	addr[2].sin_port = htons(TEST_PORT);
	addr[3].sin_port = htons(TEST_PORT);
	addr[4].sin_port = htons(0);
	addr[5].sin_port = htons(0);
	addr[6].sin_port = htons(0);

	fprintf(stderr, "Addresses:\n");
	fprintf(stderr, "addr 0: %s\n", inet_ntoa(addr[0].sin_addr));
	fprintf(stderr, "addr 1: %s\n", inet_ntoa(addr[1].sin_addr));
	fprintf(stderr, "addr 2: %s\n", inet_ntoa(addr[2].sin_addr));
	fprintf(stderr, "addr 3: %s\n", inet_ntoa(addr[3].sin_addr));
	fprintf(stderr, "addr 4: %s\n", inet_ntoa(addr[4].sin_addr));
	fprintf(stderr, "addr 5: %s\n", inet_ntoa(addr[5].sin_addr));
	fprintf(stderr, "addr 6: %s\n", inet_ntoa(addr[6].sin_addr));
	fprintf(stderr, "\n");

	test_open(0);

//	if ( test_multi_bind(0,2,1) )
//		goto dead;
//	if ( test_listen(0) )
//		goto dead;

	if ( test_multi_bind(0,4,2) )
		goto dead;
	if ( test_connect(0,0,2) )
		goto dead;

//	if ( test_accept(0,1) )
//		goto dead;

	pfd[0].fd = s[0];

	for (;;) {
		if ( poll(&pfd[0], 1, -1) < 0 )
			return show_error(__FUNCTION__);
		if ( pfd[0].revents & POLLOUT )
			if ( test_send(0) )
				goto dead;
		if ( pfd[0].revents & POLLIN )
			if ( test_read(0) )
				goto dead;
		if ( pfd[0].revents & POLLERR )
			goto dead;
		if ( pfd[0].revents & POLLHUP )
			goto dead;
		pfd[0].revents = 0;
	}

kinda_dead:
//	close(s[1]);
dead:
	close(s[0]);
}
