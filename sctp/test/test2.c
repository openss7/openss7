/*****************************************************************************

 @(#) $Id: test2.c,v 0.9.2.3 2002/05/14 09:38:55 brian Exp $

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

 $Log: test2.c,v $
 Revision 0.9.2.3  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.2  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.1.1.1  2001/04/15 22:26:08  brian
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

main() {
	int i;
	int s[7], len=0;
	struct sockaddr_in addr[] = {
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, }
	};
	struct sockaddr_in usin[] = {
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, },
		{ AF_INET, 0, { INADDR_ANY }, }
	};

	inet_aton("0.0.0.0",	    &addr[0].sin_addr);
	inet_aton("127.0.0.0",	    &addr[1].sin_addr);
	inet_aton("127.0.0.1",	    &addr[2].sin_addr);
	inet_aton("192.168.0.3",    &addr[3].sin_addr);
	inet_aton("172.16.13.17",   &addr[4].sin_addr);
	inet_aton("192.168.0.255",  &addr[5].sin_addr);
	inet_aton("255.255.255.255",&addr[6].sin_addr);

	printf("Addresses:\n");
	printf("socket 0: %s\n", inet_ntoa(addr[0].sin_addr));
	printf("socket 1: %s\n", inet_ntoa(addr[1].sin_addr));
	printf("socket 2: %s\n", inet_ntoa(addr[2].sin_addr));
	printf("socket 3: %s\n", inet_ntoa(addr[3].sin_addr));
	printf("socket 4: %s\n", inet_ntoa(addr[4].sin_addr));
	printf("socket 5: %s\n", inet_ntoa(addr[5].sin_addr));
	printf("socket 6: %s\n", inet_ntoa(addr[6].sin_addr));

	printf("Opening sockets:\n");

	if ( (s[0] = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0 )
		perror("socket 0 SOCK_STREAM");
	if ( (s[1] = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0 )
		perror("socket 1 SOCK_SEQPACKET");
	if ( (s[2] = socket(PF_INET, SOCK_RDM, IPPROTO_SCTP)) < 0 )
		perror("socket 2 SOCK_RDM");
	if ( (s[3] = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0 )
		perror("socket 3 SOCK_SEQPACKET");
	if ( (s[4] = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0 )
		perror("socket 4 SOCK_SEQPACKET");
	if ( (s[5] = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0 )
		perror("socket 5 SOCK_SEQPACKET");
	if ( (s[6] = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0 )
		perror("socket 6 SOCK_SEQPACKET");

	printf("Testing single bind:\n");

	if ( (bind(s[0], (struct sockaddr *)&addr[0], sizeof(struct sockaddr_in))) < 0 )
		perror("socket 0 bind SOCK_STREAM");
	if ( (bind(s[1], (struct sockaddr *)&addr[1], sizeof(struct sockaddr_in))) < 0 )
		perror("socket 1 bind SOCK_SEQPACKET");
	if ( (bind(s[2], (struct sockaddr *)&addr[2], sizeof(struct sockaddr_in))) < 0 )
		perror("socket 2 bind SOCK_RDM");
	if ( (bind(s[3], (struct sockaddr *)&addr[3], sizeof(struct sockaddr_in))) < 0 )
		perror("socket 3 bind SOCK_STREAM");
	if ( (bind(s[4], (struct sockaddr *)&addr[4], sizeof(struct sockaddr_in))) < 0 )
		perror("socket 4 bind SOCK_STREAM");
	if ( (bind(s[5], (struct sockaddr *)&addr[5], sizeof(struct sockaddr_in))) < 0 )
		perror("socket 5 bind SOCK_STREAM");
	if ( (bind(s[6], (struct sockaddr *)&addr[6], sizeof(struct sockaddr_in))) < 0 )
		perror("socket 6 bind SOCK_STREAM");

	close(s[0]);
	close(s[1]);
	close(s[2]);
	close(s[3]);
	close(s[4]);
	close(s[5]);
	close(s[6]);
}
