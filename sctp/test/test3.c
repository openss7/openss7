/*****************************************************************************

 @(#) $Id: test3.c,v 0.9.2.3 2002/05/14 09:38:55 brian Exp $

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

 $Log: test3.c,v $
 Revision 0.9.2.3  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.2  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.1.1.1  2001/04/16 04:46:59  brian
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
		return;
	}
	num = len/sizeof(struct sockaddr_in);

	printf("socket %d bound to:",n);
	for ( i=0; i<num; i++ )
	{
		printf(" %s:%d", inet_ntoa(usin[i].sin_addr),
				ntohs(usin[i].sin_port));
	}
	printf("\n");
}

void test_open(int n)
{
//	printf("Opening socket %d\n", n);
	if ( (s[n] = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0 )
		perror(__FUNCTION__);
}

void test_single_bind(int n)
{
	printf("Binding socket %d to ", n);
	printf("%s:%d\n", inet_ntoa(addr[n].sin_addr),
			htons(addr[n].sin_port));
	if ( (bind(s[n], (struct sockaddr *)&addr[n], sizeof(struct sockaddr_in))) < 0 )
		perror(__FUNCTION__);
	else
		show_bind(n);
}

main() {
	int i, j=0;
	int len=0;

	inet_aton("0.0.0.0",	    &addr[0].sin_addr);
	inet_aton("127.0.0.0",	    &addr[1].sin_addr);
	inet_aton("127.0.0.1",	    &addr[2].sin_addr);
	inet_aton("192.168.0.3",    &addr[3].sin_addr);
	inet_aton("172.16.13.17",   &addr[4].sin_addr);
	inet_aton("192.168.0.255",  &addr[5].sin_addr);
	inet_aton("255.255.255.255",&addr[6].sin_addr);

	printf("Addresses:\n");
	printf("addr 0: %s\n", inet_ntoa(addr[0].sin_addr));
	printf("addr 1: %s\n", inet_ntoa(addr[1].sin_addr));
	printf("addr 2: %s\n", inet_ntoa(addr[2].sin_addr));
	printf("addr 3: %s\n", inet_ntoa(addr[3].sin_addr));
	printf("addr 4: %s\n", inet_ntoa(addr[4].sin_addr));
	printf("addr 5: %s\n", inet_ntoa(addr[5].sin_addr));
	printf("addr 6: %s\n", inet_ntoa(addr[6].sin_addr));
	printf("\n");

	do {
	for ( i=0; i<3; i++ )
	{

	printf("\nBind test iteration %d\n", i+1);

//	printf("Opening sockets:\n");
	test_open(0);
	test_open(1);
	test_open(2);
	test_open(3);
	test_open(4);
	test_open(5);
	test_open(6);

//	printf("Testing single bind:\n");
	test_single_bind(6);
	test_single_bind(5);
	test_single_bind(4);
	test_single_bind(3);
	test_single_bind(2);
	test_single_bind(1);
	test_single_bind(0);

	close(s[0]);
	close(s[1]);
	close(s[2]);
	close(s[3]);
	close(s[4]);
	close(s[5]);
	close(s[6]);
	}
#define TEST_PORT 10002

	addr[0].sin_port = htons(TEST_PORT);
	addr[1].sin_port = htons(TEST_PORT);
	addr[2].sin_port = htons(TEST_PORT);
	addr[3].sin_port = htons(TEST_PORT);
	addr[4].sin_port = htons(TEST_PORT);
	addr[5].sin_port = htons(TEST_PORT);
	addr[6].sin_port = htons(TEST_PORT);

	} while(j++<1);
}
