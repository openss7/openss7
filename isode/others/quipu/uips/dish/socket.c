/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* socket.c - dish -pipe support */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/socket.c,v 9.0 1992/06/16 12:44:21 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/socket.c,v 9.0 1992/06/16 12:44:21 isode Rel
 *
 *
 * Log: socket.c,v
 * Revision 9.0  1992/06/16  12:44:21  isode
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

#include <stdio.h>
#include "quipu/util.h"
#include "tailor.h"
#include "general.h"

#ifdef SOCKETS			/* USE INTERNET SOCKETS */

#include "internet.h"

get_dish_sock(isock, pid, islocal)
	struct sockaddr_in *isock;
	int pid, islocal;
{
	int myppid;
	char *getenv();
	char *ptr, *prnt;
	static char buffer[BUFSIZ];
	static char parent[BUFSIZ];
	int portno;
	char *dp;
	register struct hostent *hp;

	if ((myppid = pid) == 0)
		myppid = getppid();

	if (pid != 0 || (ptr = getenv("DISHPROC")) == NULLCP) {
		char *cp;

		portno = (myppid & 0xffff) | 0x8000;
		if (!islocal) {
			if ((hp = gethostbystring(cp = getlocalhost()))
			    == NULL) {
				(void) fprintf(stderr, "%s: unknown host", cp);
				return (-1);
			}
			(void) sprintf(buffer, "%s %d", inet_ntoa(*(struct in_addr *)
								  hp->h_addr), portno);
		} else
			(void) sprintf(buffer, "127.0.0.1 %d", portno);

		(void) setenv("DISHPROC", ptr = buffer);
	}

	if (pid != 0 || (prnt = getenv("DISHPARENT")) == NULLCP) {
		(void) sprintf(parent, "%d", myppid);
		(void) setenv("DISHPARENT", prnt = parent);
	}

	if (sscanf(prnt, "%d", &pid) != 1) {
		(void) fprintf(stderr, "DISHPARENT malformed");
		return (-1);
	}

	if ((dp = index(ptr, ' ')) == NULLCP || sscanf(dp + 1, "%d", &portno) != 1) {
		(void) fprintf(stderr, "DISHPROC malformed");
		return (-1);
	}
	*dp = NULL;

	if ((hp = gethostbystring(ptr)) == NULL) {
		(void) fprintf(stderr, "%s: unknown host in DISHPROC", ptr);
		return (-1);
	}
	*dp = ' ';

	bzero((char *) isock, sizeof *isock);
	isock->sin_family = hp->h_addrtype;
	isock->sin_port = htons((u_short) portno);
	inaddr_copy(hp, isock);

	return (0);

}

#else				/* USE UNIX NAMED PIPES */

void
dummy()
{
	;
}

#endif
