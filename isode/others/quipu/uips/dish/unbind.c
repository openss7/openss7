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

/* unbind.c - dish shell unbind and squid commands */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/unbind.c,v 9.0 1992/06/16 12:44:21 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/uips/dish/RCS/unbind.c,v 9.0 1992/06/16 12:44:21 isode Rel
 *
 *
 * Log: unbind.c,v
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

extern int errno;

#ifdef SOCKETS			/* USE INTERNET SOCKETS */

#include "internet.h"

main(argc, argv)
	int argc;
	char *argv[];
{
	int sd, res, status;
	struct sockaddr_in sin_buf;
	struct sockaddr_in *sin = &sin_buf;
	char buffer[BUFSIZ];
	char *ptr;

	if ((sd = start_tcp_client((struct sockaddr_in *) 0, 0)) == NOTOK) {
		perror("start_tcp_client");
		exit(-20);
	}

	if (get_dish_sock(sin, 0, 1) != 0)
		exit(-21);

	if (join_tcp_server(sd, sin) == NOTOK) {
		(void) fprintf(stderr, "No connection and no cache !!!\n");
		(void) close_tcp_socket(sd);
		exit(0);
	}

	if ((ptr = rindex(argv[0], '/')) == NULLCP)
		(void) strcpy(buffer, argv[0]);
	else
		(void) strcpy(buffer, ++ptr);

	argc--, argv++;

	while (argc--) {
		(void) strcat(buffer, " \"");
		(void) strcat(buffer, *argv++);
		(void) strcat(buffer, "\"");
	}
	(void) strcat(buffer, "\n");

	if (send(sd, buffer, strlen(buffer), 0) == -1) {
		perror("send");
		(void) close_tcp_socket(sd);
		exit(-25);
	}

	if ((res = recv(sd, buffer, BUFSIZ - 1, 0)) == -1) {
		perror("recv");
		(void) close_tcp_socket(sd);
		exit(-26);
	}
	*(buffer + res) = 0;

	if (*buffer == '2') {
		status = 1;
		if (res > 1)
			(void) write(2, &buffer[1], --res);
		while ((res = recv(sd, buffer, BUFSIZ, 0)) > 0)
			(void) write(2, buffer, res);
	} else if (*buffer == '1') {
		status = 0;
		if (res > 1)
			(void) write(1, &buffer[1], --res);
		while ((res = recv(sd, buffer, BUFSIZ, 0)) > 0)
			(void) write(1, buffer, res);
	}

	(void) close_tcp_socket(sd);

	exit(status);

	return status;
}

#else				/* USE UNIX NAMED PIPES */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

char retfile[LINESIZE];
int fd;

main(argc, argv)
	int argc;
	char **argv;
{
	int res;
	char buffer[BUFSIZ];
	char sendfile[LINESIZE];
	int i;
	char *ptr;
	void pipe_quit();
	char *getenv(), *sprintf();

	(void) umask(0);
	(void) sprintf(retfile, "/tmp/dish%d", getpid());
	if ((ptr = getenv("DISHPROC")) == NULLCP) {
		(void) sprintf(sendfile, "/tmp/dish-%d", getppid());
		(void) setenv("DISHPROC", sendfile);
	} else
		(void) strcpy(sendfile, ptr);

	setbuf(stdout, NULLCP);
	setbuf(stderr, NULLCP);

	if (mknod(retfile, S_IFIFO | 0660, 0) == -1) {
		(void) fprintf(stderr, "Can't create result file %s\n", retfile);
		exit(-5);
	}

	for (i = 1; i <= 15; i++)
		(void) signal(i, pipe_quit);

	if ((fd = open(sendfile, O_WRONLY | O_NDELAY)) == -1) {
		(void) fprintf(stderr, "No connection and no cache !!!\n");
		(void) unlink(retfile);
		exit(0);
	}

	argc--;
	if ((ptr = rindex(argv[0], '/')) == NULLCP)
		(void) sprintf(buffer, "%s:%s", retfile, argv[0]);
	else
		(void) sprintf(buffer, "%s:%s", retfile, ++ptr);
	*argv++;

	while (argc--) {
		(void) strcat(buffer, " ");
		(void) strcat(buffer, *argv++);
	}

	if ((res = write(fd, buffer, strlen(buffer))) == -1) {
		(void) fprintf(stderr, "Write failed\n");
		(void) close(fd);
		(void) unlink(retfile);
		exit(-2);
	}
	(void) close(fd);

	/* get results */
	if ((fd = open(retfile, O_RDONLY)) < 0) {
		(void) fprintf(stderr, "Can't read results\n");
		(void) unlink(retfile);
		exit(-3);
	}

	if ((res = read(fd, buffer, BUFSIZ)) == -1) {
		(void) fprintf(stderr, "Read failed (%d)\n", errno);
		(void) unlink(retfile);
		(void) close(fd);
		exit(-4);
	}

	*(buffer + res) = 0;

	if (*buffer == '2')
		fputs(&buffer[1], stderr);
	else if (*buffer == '1')
		fputs(&buffer[1], stdout);

	(void) close(fd);
	(void) unlink(retfile);

	if (*buffer == '2')
		exit(-1);
}

void
pipe_quit(sig)
	int sig;
{
	(void) unlink(retfile);
	(void) fprintf(stderr, "(signal %d) exiting...\n", sig);
	exit(0);
}

#endif
