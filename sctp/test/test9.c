/*****************************************************************************

 @(#) $Id: test9.c,v 0.9.2.3 2002/05/14 09:38:55 brian Exp $

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

 Last Modified $Date: 2002/05/14 09:38:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test9.c,v $
 Revision 0.9.2.3  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.2  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.1.1.1  2001/04/24 16:28:43  brian
 Import of Linux SCTP Prerelease1

 *****************************************************************************/

static char const ident[] = "$Name:  $($Revision: 0.9.2.3 $) $Date: 2002/05/14 09:38:55 $";

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

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

void show_bind(int n)
{
	struct sockaddr_in usin[7] = { 0, };
	int len = sizeof(usin);
	int i, num;

	if ( (getsockname(s[n], (struct sockaddr *)usin, &len)) < 0 )
	{
		perror(__FUNCTION__);
		fprintf(stderr, "^^^^^^^^\n");
		return;
	}
	num = len/sizeof(struct sockaddr_in);

	fprintf(stderr, "socket %d bound to:",n);
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
	{
		perror(__FUNCTION__);
		fprintf(stderr, "^^^^^^^^\n");
		return -1;
	}
//	if ( fcntl(s[n], F_SETFL, O_NONBLOCK) < 0 )
//	{
//		perror(__FUNCTION__);
//		fprintf(stderr, "^^^^^^^^\n");
//		return -1;
//	}
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
	{
		perror(__FUNCTION__);
		fprintf(stderr, "^^^^^^^^\n");
		return -1;
	}
	else
		show_bind(n);
	return 0;
}

int test_listen(int n)
{
	fprintf(stderr, "Listening on socket %d\n", n);
	if ( (listen(s[n], 1)) < 0 )
	{
		perror(__FUNCTION__);
		fprintf(stderr, "^^^^^^^^\n");
		return -1;
	}
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
	{
		perror(__FUNCTION__);
		fprintf(stderr, "^^^^^^^^\n");
		return -1;
	}
	else
		show_bind(n);
	return 0;

}

main() {
	int keepalive = 1;

	inet_aton("0.0.0.0",	    &addr[0].sin_addr);
	inet_aton("127.0.0.0",	    &addr[1].sin_addr);
	inet_aton("127.0.0.1",	    &addr[2].sin_addr);
	inet_aton("192.168.0.3",    &addr[3].sin_addr);
	inet_aton("172.16.13.17",   &addr[4].sin_addr);
	inet_aton("192.168.0.255",  &addr[5].sin_addr);
	inet_aton("127.0.0.1",      &addr[6].sin_addr);

#define TEST_PORT 10002
	addr[0].sin_port = htons(TEST_PORT);
	addr[1].sin_port = htons(TEST_PORT);
	addr[2].sin_port = htons(TEST_PORT);
	addr[3].sin_port = htons(TEST_PORT);
	addr[4].sin_port = htons(TEST_PORT);
	addr[5].sin_port = htons(TEST_PORT);
	addr[6].sin_port = htons(TEST_PORT+1);

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
	test_open(1);

	if ( !test_multi_bind(1,2,1) ) test_listen(1);
	if ( !test_multi_bind(0,6,1) ) test_connect(0,2,1);

	sleep(40);

//	test_connect(0,1,2);

	printf("Closing connected socket\n");
	sleep(5);
	close(s[0]);

	printf("Closing listening socket\n");
	sleep(5);
	close(s[1]);
}
