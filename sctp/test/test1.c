/*****************************************************************************

 @(#) $Id: test1.c,v 0.9.2.3 2002/05/14 09:38:55 brian Exp $

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

 $Log: test1.c,v $
 Revision 0.9.2.3  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.2  2002/05/14 09:38:55  brian
 Updated test files and includes.

 Revision 0.1.1.1  2001/04/15 21:11:43  brian
 Import of Linux SCTP Prerelease1

 *****************************************************************************/

static char const ident[] =
    "$Name:  $($Revision: 0.9.2.3 $) $Date: 2002/05/14 09:38:55 $";

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>

main()
{
	int s1, s2, s3;

	if ((s1 = socket(PF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0)
		perror("SOCK_STREAM");
	else
		printf("SOCK_STREAM: Success\n");

	if ((s2 = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0)
		perror("SOCK_SEQPACKET");
	else
		printf("SOCK_SEQPACKET: Success\n");

	if ((s3 = socket(PF_INET, SOCK_RDM, IPPROTO_SCTP)) < 0)
		perror("SOCK_RDM");
	else
		printf("SOCK_RDM: Success\n");

}
