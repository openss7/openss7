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

/* ftp.c - FTP client */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam-ftp/RCS/ftp.c,v 9.0 1992/06/16 12:15:29 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam-ftp/RCS/ftp.c,v 9.0 1992/06/16 12:15:29 isode Rel
 *
 *
 * Log: ftp.c,v
 * Revision 9.0  1992/06/16  12:15:29  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *	The MITRE Corporation (hereafter MITRE) makes this software available 
 *	on an "as is" basis.  No guarantees, either explicit or implied, are 
 *	given as to performance or suitability.  
 *
 */

/*
 *	Shamelessly taken from UCB
 */

#include "general.h"
#include <sys/param.h>
#ifdef SVR4_UCB
#include <termio.h>
#else
#include <sys/ioctl.h>
#endif
#include "internet.h"
#include <sys/time.h>

#include <arpa/ftp.h>

#include <stdio.h>
#include <errno.h>
#include <varargs.h>

#include "ftp_var.h"
#include "logger.h"
void advise();

#ifndef NOTOK
#define NOTOK (-1)
#define OK	0
#define DONE	1
#endif				/* NOTOK */

int verbose = 0;

struct sockaddr_in hisctladdr;
struct sockaddr_in data_addr;
int data = -1;
int connected = 0;
struct sockaddr_in myctladdr;

FILE *cin, *cout;
int dataconn();

ftp_init()
{				/* default ftp communication values */

	(void) strcpy(typename, "ascii"), type = TYPE_A;
	(void) strcpy(formname, "non-print"), form = FORM_N;
	(void) strcpy(modename, "stream"), mode = MODE_S;
	(void) strcpy(structname, "file"), stru = STRU_F;
	(void) strcpy(bytename, "8"), bytesize = 8;
	ftp_directory = 0;
	ftp_error = ftp_error_buffer;
	verbose = isatty(fileno(stderr));
}

hookup(host, port)
	char *host;
	int port;
{
	register struct hostent *hp;
	int s, len;

	bzero((char *) &hisctladdr, sizeof(hisctladdr));
	hp = gethostbyname(host);
	if (hp == NULL) {
#ifdef	h_addr
		static char *addrs = NULL;
#endif
		static struct hostent def;
		static struct in_addr defaddr;
		static char namebuf[128];
		u_long inet_addr();

		defaddr.s_addr = inet_addr(host);
		if (defaddr.s_addr == -1) {
			(void) sprintf(ftp_error, "%s: Unknown host.", host);
			return (NOTOK);
		}
		(void) strcpy(namebuf, host);
		def.h_name = namebuf;
		hostname = namebuf;
#ifdef	h_addr
		def.h_addr_list = &addrs;
#endif
		def.h_addr = (char *) &defaddr;
		def.h_length = sizeof(struct in_addr);
		def.h_addrtype = AF_INET;
		def.h_aliases = 0;
		hp = &def;
	}
	hostname = hp->h_name;
	hisctladdr.sin_family = hp->h_addrtype;
	s = socket(hp->h_addrtype, SOCK_STREAM, 0);
	if (s < 0) {
		(void) sprintf(ftp_error, "ftp: socket %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		return (NOTOK);
	}
	if (bind(s, (struct sockaddr *) &hisctladdr, sizeof(hisctladdr)) < 0) {
		(void) sprintf(ftp_error, "ftp: bind %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		goto bad;
	}
	inaddr_copy(hp, &hisctladdr);
	hisctladdr.sin_port = htons((u_short) port);
	if (connect(s, (struct sockaddr *) &hisctladdr, sizeof(hisctladdr)) < 0) {
		(void) sprintf(ftp_error, "ftp: connect %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		goto bad;
	}
	len = sizeof(myctladdr);
	if (getsockname(s, (struct sockaddr *) &myctladdr, &len) < 0) {
		(void) sprintf(ftp_error, "ftp: getsockname %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		goto bad;
	}
	cin = fdopen(s, "r");
	cout = fdopen(s, "w");
	if (cin == NULL || cout == NULL) {
		(void) sprintf(ftp_error, "ftp: fdopen failed.");
		if (cin)
			(void) fclose(cin);
		if (cout)
			(void) fclose(cout);
		goto bad;
	}
	(void) getreply(0);	/* read startup message from server */
	connected = 1;
	return (OK);
      bad:
	(void) close(s);
	return (NOTOK);
}

login(user, pass, acct)
	char *user, *pass, *acct;
{
	int n;

	if (!user) {
		(void) sprintf(ftp_error, "Username required");
		return (NOTOK);
	}
	n = command("USER %s", user);
	if (n == CONTINUE) {
		if (!pass) {
			(void) sprintf(ftp_error, "Password required");
			return (NOTOK);
		}
		n = command("PASS %s", pass);
	}
	if (n == CONTINUE) {
		if (!acct) {
			(void) sprintf(ftp_error, "Account required");
			return (NOTOK);
		}
		n = command("ACCT %s", acct);
	}
	if (n != COMPLETE) {
		(void) sprintf(ftp_error, "Login failed.");
		return (NOTOK);
	}
	return (OK);
}

#ifndef	lint
command(va_alist)
	va_dcl
{
	int val;
	va_list ap;

	va_start(ap);

	val = _command(ap);

	va_end(ap);

	return val;
}

_command(ap)
	va_list ap;
{
	char buffer[BUFSIZ];

	if (cout == NULL) {
		(void) sprintf(ftp_error, "No control connection for command %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		return (NOTOK);
	}

	_xsprintf (buffer, NULLCP, ap);
	fprintf (cout, "%s\r\n", buffer);
	(void) fflush(cout);
	if (verbose)
		advise(LLOG_DEBUG, NULLCP, "<--- %s", buffer);
	return (getreply(!strcmp(buffer, "QUIT")));
}
#else
/* VARARGS1 */

command(fmt)
	char *fmt;
{
	return command(fmt);
}
#endif

#include <ctype.h>

getreply(expecteof)
	int expecteof;
{
	register int c, n;
	register int code, dig;
	int originalcode = 0, continuation = 0;
	char *mesg;

	mesg = ftp_error_buffer;
	for (;;) {
		dig = n = code = 0;
		while ((c = getc(cin)) != '\n') {
			dig++;
			if (c == EOF) {
				if (expecteof)
					return (0);
				lostpeer();
				advise(LLOG_EXCEPTIONS, NULLCP, "getreply: %s", ftp_error_buffer);
				return (1);
				/* exit(1); */
			}
			if (c != '\r')
				*mesg++ = c;
			else
				*mesg = '\0';
			if (dig < 4 && isdigit(c))
				code = code * 10 + (c - '0');
			if (dig == 4 && c == '-')
				continuation++;
			if (n == 0)
				n = c;
		}
		if (continuation && code != originalcode) {
			if (originalcode == 0)
				originalcode = code;
			continue;
		}
		if (verbose)
			advise(LLOG_DEBUG, NULLCP, "---> %s", ftp_error_buffer);
		return (n - '0');
	}
}

/*
 *  sendrequest and recvrequest routines have been modified to send the
 *  appropriate remote command then open and return a file descriptor (socket).
 *  The FTAM code treats this as though it were a local file (which
 *  is about what FTP does)
 */
int
sendrequest(cmd, /* local, */ remote)
	char *cmd, /* *local, */ *remote;
{
	int dout;
	int expectingreply = 0;

	if (initconn())
		goto bad;
	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM)
			goto bad;
	} else if (command("%s", cmd) != PRELIM)
		goto bad;
	expectingreply++;	/* got preliminary reply, expecting final reply */
	dout = dataconn("w");
	if (dout == NOTOK)
		goto bad;
	return (dout);

      bad:
	if (data >= 0)
		(void) close(data), data = -1;
	if (expectingreply) {
		(void) getreply(0);
		expectingreply = 0;
	}
	return (NOTOK);
}

int
recvrequest(cmd, /* local, */ remote)
	char *cmd, /* *local, */ *remote;
{
	int din;
	int expectingreply = 0;

	if (initconn())
		goto bad;
	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM)
			goto bad;
	} else if (command("%s", cmd) != PRELIM)
		goto bad;
	expectingreply++;	/* got preliminary reply, expecting final reply */
	din = dataconn("r");
	if (din == NOTOK)
		goto bad;
	return (din);
      bad:
	if (data >= 0)
		(void) close(data), data = -1;
	if (expectingreply) {
		(void) getreply(0);
		expectingreply = 0;
	}
	return (NOTOK);
}

/*
 * Need to start a listen on the data channel
 * before we send the command, otherwise the
 * server's connect may fail.
 */
int sendport = -1;

initconn()
{
	register char *p, *a;
	int result, len;

#ifdef	BSD43
	int on = 1;
#endif

      noport:
	data_addr = myctladdr;
	if (sendport)
		data_addr.sin_port = 0;	/* let system pick one */
	if (data != -1)
		(void) close(data);
	data = socket(AF_INET, SOCK_STREAM, 0);
	if (data < 0) {
		(void) sprintf(ftp_error, "ftp: socket %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		return (NOTOK);
	}
	if (!sendport)
#ifndef	BSD43
		if (setsockopt(data, SOL_SOCKET, SO_REUSEADDR, (char *) 0, 0) < 0) {
#else
		if (setsockopt(data, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof on) < 0) {
#endif
			(void) sprintf(ftp_error, "ftp: setsockopt (reuse address) %s",
				       (errno <= sys_nerr) ? sys_errlist[errno] : "");
			goto bad;
		}
	if (bind(data, (struct sockaddr *) &data_addr, sizeof(data_addr)) < 0) {
		(void) sprintf(ftp_error, "ftp: bind %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		goto bad;
	}
	if (options & SO_DEBUG &&
#ifndef	BSD43
	    setsockopt(data, SOL_SOCKET, SO_DEBUG, (char *) 0, 0) < 0)
#else
	    setsockopt(data, SOL_SOCKET, SO_DEBUG, (char *) &on, on) < 0)
#endif
	    (void) sprintf(ftp_error, "ftp: setsockopt (ignoreg) %s",
			   (errno <= sys_nerr) ? sys_errlist[errno] : "");
	len = sizeof(data_addr);
	if (getsockname(data, (struct sockaddr *) &data_addr, &len) < 0) {
		(void) sprintf(ftp_error, "ftp: getsockname  %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		goto bad;
	}
	if (listen(data, 1) < 0) {
		(void) sprintf(ftp_error, "ftp: listen  %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		goto bad;
	}
	if (sendport) {
		a = (char *) &data_addr.sin_addr;
		p = (char *) &data_addr.sin_port;
#define	UC(b)	(((int)b)&0xff)
		result =
		    command("PORT %d,%d,%d,%d,%d,%d",
			    UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]), UC(p[0]), UC(p[1]));
		if (result == ERROR && sendport == -1) {
			sendport = 0;
			goto noport;
		}
		return ((result == COMPLETE) ? OK : NOTOK);
	}
	return (OK);
      bad:
	(void) close(data), data = -1;
	return (NOTOK);
}

/*ARGSUSED */
int
dataconn(modeX)
	char *modeX;
{
	struct sockaddr_in from;
	int s, fromlen = sizeof(from);

	s = accept(data, (struct sockaddr *) &from, &fromlen);
	if (s < 0) {
		(void) sprintf(ftp_error, "ftp: accept  %s",
			       (errno <= sys_nerr) ? sys_errlist[errno] : "");
		(void) close(data), data = -1;
		return (NOTOK);
	}
	(void) close(data);
	data = s;
	return (data);
}

lostpeer()
{

	if (connected) {
		if (cout != NULL) {
			(void) shutdown(fileno(cout), 1 + 1);
			(void) fclose(cout);
			cout = NULL;
		}
		if (data >= 0) {
			(void) shutdown(data, 1 + 1);
			(void) close(data);
			data = -1;
		}
		connected = 0;
	}
}
