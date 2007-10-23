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

/* getpassword.c - generic read-the-password-from-the-tty */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/getpassword.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/getpassword.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: getpassword.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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

/* LINTLIBRARY */

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "sys.file.h"
#ifdef linux
#include <termios.h>
#include <sys/ttydefaults.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif				/* HAVE_STRING_H */

#ifdef	BSD44
char *getpass();
#endif

/*  */

/* roll our own since want to get past UNIX's limit of 8 octets... */

char *
getpassword(prompt)
	char *prompt;
{
#ifndef	BSD44
	register int c;
	int flags, isopen;
	register char *bp, *ep;

#if	!defined(SYS5) && !defined(XOS_2) && !defined(LINUX) && !defined(linux)
	struct sgttyb sg;
#else
	struct termio sg;
#endif
	void *istat;
	FILE *fp;
	static char buffer[BUFSIZ];

	(void) rcsid;
#ifdef SUNLINK_7_0
	fp = stdin, isopen = 0;	/* will help greatly to work off a script */
#else
	if ((c = open("/dev/tty", O_RDWR)) != NOTOK && (fp = fdopen(c, "r")))
		setbuf(fp, NULLCP), isopen = 1;
	else {
		if (c != NOTOK)
			(void) close(c);

		fp = stdin, isopen = 0;
	}
#endif

	istat = signal(SIGINT, SIG_IGN);

#if	!defined(SYS5) && !defined(XOS_2) && !defined(LINUX) && !defined(linux)
	(void) gtty(fileno(fp), &sg);
	flags = sg.sg_flags;
	sg.sg_flags &= ~ECHO;
	(void) stty(fileno(fp), &sg);
#else
	(void) ioctl(fileno(fp), TCGETA, (char *) &sg);
	flags = sg.c_lflag;
	sg.c_lflag &= ~ECHO;
	(void) ioctl(fileno(fp), TCSETAW, (char *) &sg);
#endif

#ifdef SUNLINK_7_0
	(void) fprintf(stdout, "%s", prompt);
	(void) fflush(stdout);
#else
	(void) fprintf(stderr, "%s", prompt);
	(void) fflush(stderr);
#endif

	for (ep = (bp = buffer) + sizeof buffer - 1; (c = getc(fp)) != EOF;)
#ifndef	apollo
		if (c == '\n')
#else
		if (c == '\n' || c == '\r')
#endif
			break;
		else if (bp < ep)
			*bp++ = c;
	*bp = '\0';

#ifdef SUNLINK_7_0
	(void) fprintf(stdout, "\n");
	(void) fflush(stdout);
#else
	(void) fprintf(stderr, "\n");
	(void) fflush(stderr);
#endif

#if	!defined(SYS5) && !defined(XOS_2) && !defined(LINUX) && !defined(linux)
	sg.sg_flags = flags;
	(void) stty(fileno(fp), &sg);
#else
	sg.c_lflag = flags;
	(void) ioctl(fileno(fp), TCSETAW, (char *) &sg);
#endif

	(void) signal(SIGINT, istat);

	if (isopen)
		(void) fclose(fp);

	return buffer;
#else
	return getpass(prompt);
#endif
}

static inline void
dummy(void)
{
	(void) rcsid;
}
