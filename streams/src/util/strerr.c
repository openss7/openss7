/*****************************************************************************

 @(#) $RCSfile: strerr.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/11/26 15:27:45 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2006/11/26 15:27:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strerr.c,v $
 Revision 0.9.2.22  2006/11/26 15:27:45  brian
 - testing and corrections to strlog capabilities

 Revision 0.9.2.21  2006/03/10 07:23:59  brian
 - rationalized streams and strutil package sources

 Revision 0.9.2.20  2006/02/20 10:59:30  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ident "@(#) $RCSfile: strerr.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/11/26 15:27:45 $"

static char const ident[] =
    "$RCSfile: strerr.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/11/26 15:27:45 $";

/*
 *  SVR 4.2 Daemon: strerr - (Daemon) Receives error log messages from the STREAMS
 *  log driver.
 */

#define _XOPEN_SOURCE 600

#include <stddef.h>
#include <stdint.h>
#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>
#include <sys/strlog.h>

const char *program = "strerr";
const char *loggername = "error";

static int nomead = 1;			/* default daemon mode */
static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */

int strlog_fd = -1;

char outfile[256] = "";
char errfile[256] = "";
char outpath[256] = "";
char errpath[256] = "";
char basname[256] = "";
char cfgfile[256] = "";
char outpdir[256] = "/var/log/streams";
char devname[256] = "";
char mailuid[256] = "";
char pidfile[256] = "";

/* search path for log devices */
static const char *logdevices[] = {
	"/dev/streams/strlog",
	"/dev/streams/log",
	"/dev/strlog",
	"/dev/log",
	NULL
};

#define MY_FACILITY(__pri)	(LOG_DAEMON|(__pri))

#define FLAG_ZEROPAD	(1<<0)	/* pad with zero */
#define FLAG_SIGN	(1<<1)	/* unsigned/signed long */
#define FLAG_PLUS	(1<<2)	/* show plus */
#define FLAG_SPACE	(1<<3)	/* space if plus */
#define FLAG_LEFT	(1<<4)	/* left justified */
#define FLAG_SPECIAL	(1<<5)	/* 0x */
#define FLAG_LARGE	(1<<6)	/* use 'ABCDEF' instead of 'abcdef' */

#define PROMOTE_TYPE		int
#define PROMOTE_SIZE		(sizeof(PROMOTE_TYPE))
#define PROMOTE_ALIGN(__x)	(((__x) + PROMOTE_SIZE - 1) & ~(PROMOTE_SIZE - 1))
#define PROMOTE_SIZEOF(__x)	((sizeof(__x) < PROMOTE_SIZE) ? PROMOTE_SIZE : sizeof(__x))
#define PROMOTE_ARGVAL(__type,__ptr) \
	(long long)({ \
		long long val; \
		if (sizeof(__type) < PROMOTE_SIZEOF(__type)) { \
			val = (long long)*((PROMOTE_TYPE *)(__ptr)); \
			__ptr = (typeof(__ptr))((char *)__ptr + PROMOTE_SIZE); \
		} else { \
			val = (long long)*((__type *)(__ptr)); \
			__ptr = (typeof(__ptr))((char *)__ptr + sizeof(__type)); \
		} \
		val; \
	})

/*
 *  This function prints a formatted number to a buffer.  It is largely
 *  taken from /usr/src/linux/lib/vsprintf.c
 */
static int
number(char *sbuf, const char *end, long long num, int base, int width, int decimal, int flags)
{
	char sign;
	int i, iszero = (num == 0);
	char *str;
	char tmp[66];

	str = sbuf;
	if (flags & FLAG_LEFT)
		flags &= ~FLAG_ZEROPAD;
	if (base < 2 || base > 16)
		goto done;
	sign = '\0';
	if (flags & FLAG_SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			width--;
		} else if (flags & FLAG_PLUS) {
			sign = '+';
			width--;
		} else if (flags & FLAG_SPACE) {
			sign = ' ';
			width--;
		}
	}
	if (flags & FLAG_SPECIAL) {
		if (!iszero) {
			switch (base) {
			case 16:
				width -= 2;	/* for 0x */
				break;
			case 8:
				width--;	/* for 0 */
				break;
			}
		}
	}
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else {
		const char *hexdig = (flags & FLAG_LARGE) ? "0123456789ABCDEF" : "0123456789abcdef";

		while (num != 0) {
			lldiv_t result = lldiv(num, base);

			tmp[i++] = hexdig[result.rem];
			num = result.quot;
		}
	}
	if (i > decimal)
		decimal = i;
	width -= decimal;
	if (!(flags & (FLAG_ZEROPAD | FLAG_LEFT)))
		if (width > 0) {
			while (width-- > 0) {
				*str = ' ';
				if (++str >= end)
					goto done;
			}
		}
	if (sign != '\0') {
		*str = sign;
		if (++str >= end)
			goto done;
	}
	if (flags & FLAG_SPECIAL) {
		if (!iszero) {
			switch (base) {
			case 8:
				*str = '0';
				if (++str >= end)
					goto done;
				break;
			case 16:
				*str = '0';
				if (++str >= end)
					goto done;
				if (flags & FLAG_LARGE)
					*str = 'X';
				else
					*str = 'x';
				if (++str >= end)
					goto done;
				break;
			}
		}
	}
	if (!(flags & FLAG_LEFT)) {
		char pad = (flags & FLAG_ZEROPAD) ? '0' : ' ';

		if (width > 0) {
			while (width-- > 0) {
				*str = pad;
				if (++str >= end)
					goto done;
			}
		}
	}
	if (i < decimal) {
		while (i < decimal--) {
			*str = '0';
			if (++str >= end)
				goto done;
		}
	}
	if (i > 0) {
		while (i-- > 0) {
			*str = tmp[i];
			if (++str >= end)
				goto done;
		}
	}
	if (width > 0) {
		while (width-- > 0) {
			*str = ' ';
			if (++str >= end)
				goto done;
		}
	}
      done:
	return (str - sbuf);
}

/*
 *  This function does an snprintf from the buffer using a slight variation of
 *  the snprintf that is identical to that used inside the kernel.  There are
 *  some constructs that are not supported.  Also, the kernel passes a format
 *  string followed by the arguments on the next word aligned boundary, and this
 *  is what is necessary to format them.
 *
 *  This function is largely adapted from /usr/src/linux/lib/vsprintf.c
 */
static int
snprintf_text(char *sbuf, size_t slen, const char *buf, int len)
{
	const char *fmt, *args, *aend, *end;
	char *str;
	char type;
	size_t flen, plen;
	int flags = 0, width = -1, decimal = -1, base = 10;

	if (slen == 0)
		return (0);
	fmt = buf;
	flen = strnlen(fmt, len);
	plen = PROMOTE_ALIGN(flen + 1);
	args = &buf[plen];
	aend = buf + len;
	str = sbuf;
	end = str + slen - 1;	/* room for null */
	for (; *fmt; ++fmt) {
		const char *pos;
		unsigned long long num = 0;

		if (*fmt != '%') {
			*str = *fmt;
			if (++str >= end)
				goto done;
			continue;
		}
		flags = 0;
		width = -1;
		decimal = -1;
		base = 10;
		pos = fmt;	/* remember position of % */
		/* process flags */
		for (++fmt;; ++fmt) {
			switch (*fmt) {
			case '-':
				flags |= FLAG_LEFT;
				continue;
			case '+':
				flags |= FLAG_PLUS;
				continue;
			case ' ':
				flags |= FLAG_SPACE;
				continue;
			case '#':
				flags |= FLAG_SPECIAL;
				continue;
			case '0':
				flags |= FLAG_ZEROPAD;
				continue;
			default:
				break;
			}
			break;
		}
		/* get field width */
		if (isdigit(*fmt))
			for (width = 0; isdigit(*fmt); width *= 10, width += (*fmt++ - '0')) ;
		else if (*fmt == '*') {
			++fmt;
			if (args + PROMOTE_SIZEOF(int) <= aend) {
				width = PROMOTE_ARGVAL(int, args);

				if (width < 0) {
					width = -width;
					flags |= FLAG_LEFT;
				}
			} else
				args = aend;
		}
		/* get the decimal precision */
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt))
				for (decimal = 0; isdigit(*fmt);
				     decimal *= 10, decimal += (*fmt - '0')) ;
			else if (*fmt == '*') {
				if (args + PROMOTE_SIZEOF(int) <= aend) {
					decimal = PROMOTE_ARGVAL(int, args);

					if (decimal < 0)
						decimal = 0;
				} else
					args = aend;
			}
		}
		/* get conversion type */
		type = 'i';
		switch (*fmt) {
		case 'h':
			type = *fmt;
			++fmt;
			if (*fmt == 'h') {
				type = 'c';
				++fmt;
			}
			break;
		case 'l':
			type = *fmt;
			++fmt;
			if (*fmt == 'l') {
				type = 'L';
				++fmt;
			}
			break;
		case 'q':
		case 'L':
		case 'Z':
		case 'z':
		case 't':
			type = *fmt;
			++fmt;
			break;
		}
		switch (*fmt) {
		case 'c':
		{
			char c = ' ';

			if (!(flags & FLAG_LEFT))
				if (width > 0) {
					while (--width > 0) {
						*str = ' ';
						if (++str >= end)
							goto done;
					}
				}
			if (args + PROMOTE_SIZEOF(char) <= aend)
				 c = PROMOTE_ARGVAL(char, args);

			else
				args = aend;
			*str = c;
			if (++str >= end)
				goto done;
			if (width > 0) {
				while (--width > 0) {
					*str = ' ';
					if (++str >= end)
						goto done;
				}
			}
			continue;
		}
		case 's':
		{
			const char *s;
			int i;
			int slen = 0, splen = 0;

			s = args;
			if (args < aend) {
				slen = strlen(s);
				splen = PROMOTE_ALIGN(slen + 1);
			} else
				args = aend;
			if (args + splen <= aend)
				args += splen;
			else
				args = aend;
			if (slen > (size_t) decimal)
				slen = (size_t) decimal;
			if (!(flags & FLAG_LEFT))
				if (slen < width) {
					while (slen < width--) {
						*str = ' ';
						if (++str >= end)
							goto done;
					}
				}
			for (i = 0; i < slen; ++i, ++s) {
				*str = *s;
				if (++str >= end)
					goto done;
			}
			if (slen < width) {
				while (slen < width--) {
					*str = ' ';
					if (++str >= end)
						goto done;
				}
			}
			continue;
		}
		case '%':
			*str = '%';
			if (++str >= end)
				goto done;
			continue;
		case 'p':
		case 'o':
		case 'X':
		case 'x':
		case 'd':
		case 'i':
		case 'u':
			switch (*fmt) {
			case 'p':
				type = 'p';
				if (width == -1) {
					width = 2 * sizeof(void *);
					flags |= FLAG_ZEROPAD;
				}
				flags |= FLAG_SPECIAL;
				base = 16;
				break;
			case 'o':
				base = 8;
				break;
			case 'X':
				flags |= FLAG_LARGE;
			case 'x':
				base = 16;
				break;
			case 'd':
			case 'i':
				flags |= FLAG_SIGN;
			case 'u':
				break;
			}
			switch (type) {
			case 'c':	/* really 'hh' type */
				if (args + PROMOTE_SIZEOF(unsigned char) <= aend)
					 num = PROMOTE_ARGVAL(unsigned char, args);

				else
					args = aend;
				if (flags & FLAG_SIGN)
					num = (signed char) num;
				break;
			case 'h':
				if (args + PROMOTE_SIZEOF(unsigned short) <= aend)
					 num = PROMOTE_ARGVAL(unsigned short, args);

				else
					args = aend;
				if (flags & FLAG_SIGN)
					num = (signed short) num;
				break;
			case 'p':	/* really 'p' conversion */
				if (args + PROMOTE_SIZEOF(uintptr_t) <= aend)
					num = PROMOTE_ARGVAL(uintptr_t, args);
				else
					args = aend;
				flags &= ~FLAG_SIGN;
				break;
			case 'l':
				if (args + PROMOTE_SIZEOF(unsigned long) <= aend)
					 num = PROMOTE_ARGVAL(unsigned long, args);

				else
					args = aend;
				if (flags & FLAG_SIGN)
					num = (signed long) num;
				break;
			case 'q':
			case 'L':	/* really 'll' type */
				if (args + PROMOTE_SIZEOF(unsigned long long) <= aend)
					 num = PROMOTE_ARGVAL(unsigned long long, args);

				else
					args = aend;
				if (flags & FLAG_SIGN)
					num = (signed long long) num;
				break;
			case 'Z':
			case 'z':
				if (args + PROMOTE_SIZEOF(size_t) <= aend)
					 num = PROMOTE_ARGVAL(size_t, args);

				else
					args = aend;
				if (flags & FLAG_SIGN)
					num = (ssize_t) num;
				break;
			case 't':
				if (args + PROMOTE_SIZEOF(ptrdiff_t) <= aend)
					num = PROMOTE_ARGVAL(ptrdiff_t, args);
				else
					args = aend;
				break;
			default:
				if (args + PROMOTE_SIZEOF(unsigned int) <= aend)
					 num = PROMOTE_ARGVAL(unsigned int, args);

				else
					args = aend;
				if (flags & FLAG_SIGN)
					num = (signed int) num;
				break;
			}
			str += number(str, end, num, base, width, decimal, flags);
			if (str >= end)
				goto done;
			continue;
		case '\0':
		default:
			*str = '%';	/* put the original % back */
			if (++str >= end)
				goto done;
			/* put the bad format characters */
			for (; fmt > pos; pos++) {
				*str = *pos;
				if (++str >= end)
					goto done;
			}
			if (*fmt == '\0')
				break;
			continue;
		}
		break;
	}
      done:
	*str = '\0';
	return (str - sbuf);
}

static void
version(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2006  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 2, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Options:\n\
    -a, --admin MAILID\n\
        specify a mail address for notifications, default: 'root'\n\
    -n, --nodaemon\n\
        do not daemonize, run in the foreground, default: daemon\n\
    -d, --directory DIRECTORY\n\
        specify a directory for log files, default: '/var/log/streams'\n\
    -b, --basename BASENAME\n\
        file basename, default: '%3$s'\n\
    -o, --outfile OUTFILE\n\
        redirect output to OUTFILE, default: ${BASENAME}.mm-dd\n\
    -e, --errfile ERRFILE\n\
        redirect errors to ERRFILE, default: ${BASENAME}.errors\n\
    -p, --pidfile PIDFILE\n\
        when running as daemon, output pid to PIDFILE, default: /var/run/%2$s.pid\n\
    -l, --logdev LOGDEVICE\n\
        log device to open, default: '/dev/strlog'\n\
    -q, --quiet\n\
        suppress output\n\
    -D, --debug [LEVEL]\n\
        increase or set debugging verbosity\n\
    -v, --verbose [LEVEL]\n\
        increase or set output verbosity\n\
    -h, --help, -?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print the version and exit\n\
    -C, --copying\n\
        print copying permissions and exit\n\
", argv[0], program, loggername);
}

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the GNU General Public License  as  published by the Free Software\n\
Foundation; version  2  of  the  License.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received a copy of the GNU  General  Public License  along with\n\
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,\n\
Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the  Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
", ident);
}

/* events */
enum {
	STRLOG_NONE = 0,
	STRLOG_SUCCESS = 0,
	STRLOG_TIMEOUT,
	STRLOG_PCPROTO,
	STRLOG_PROTO,
	STRLOG_DATA,
	STRLOG_FAILURE = -1,
};

int
sig_register(int signum, RETSIGTYPE(*handler) (int))
{
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = handler ? handler : SIG_DFL;
	act.sa_flags = handler ? SA_RESTART : 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(signum, &act, NULL))
		return STRLOG_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, signum);
	sigprocmask(handler ? SIG_UNBLOCK : SIG_BLOCK, &mask, NULL);
	return STRLOG_SUCCESS;
}

int alm_signal = 0;
int hup_signal = 0;
int trm_signal = 0;

RETSIGTYPE
alm_handler(int signum)
{
	alm_signal = 1;
	return (RETSIGTYPE) (0);
}

int
alm_catch(void)
{
	return sig_register(SIGALRM, &alm_handler);
}

int
alm_block(void)
{
	return sig_register(SIGALRM, NULL);
}

int
alm_action(void)
{
	alm_signal = 0;
	return (0);
}

RETSIGTYPE
hup_handler(int signum)
{
	hup_signal = 1;
	return (RETSIGTYPE) (0);
}

int
hup_catch(void)
{
	return sig_register(SIGALRM, &hup_handler);
}

int
hup_block(void)
{
	return sig_register(SIGALRM, NULL);
}

int
hup_action(void)
{
	hup_signal = 0;
	syslog(MY_FACILITY(LOG_WARNING), "Caught SIGHUP, reopening files.");
	if (output > 1)
		syslog(MY_FACILITY(LOG_NOTICE), "Reopening output file %s", outpath);
	if (outpath[0] != '\0') {
		fflush(stdout);
		fclose(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(MY_FACILITY(LOG_ERR), "%m");
			syslog(MY_FACILITY(LOG_ERR), "Could not reopen stdout file %s", outpath);
		}
		// output_header(void);
	}
	if (output > 1)
		syslog(MY_FACILITY(LOG_NOTICE), "Reopening error file %s", errpath);
	if (errpath[0] != '\0') {
		fflush(stderr);
		fclose(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(MY_FACILITY(LOG_ERR), "%m");
			syslog(MY_FACILITY(LOG_ERR), "Could not reopen stderr file %s", errpath);
		}
	}
	return (0);
}

RETSIGTYPE
trm_handler(int signum)
{
	trm_signal = 1;
	return (RETSIGTYPE) (0);
}

int
trm_catch(void)
{
	return sig_register(SIGALRM, &trm_handler);
}

int
trm_block(void)
{
	return sig_register(SIGALRM, NULL);
}

void strlog_exit(int retval);

int
trm_action(void)
{
	trm_signal = 0;
	syslog(MY_FACILITY(LOG_WARNING), "%s: Caught SIGTERM, shutting down", program);
	strlog_exit(0);
	return (0);		/* should be no return */
}

void
sig_catch(void)
{
	alm_catch();
	hup_catch();
	trm_catch();
}

void
sig_block(void)
{
	alm_block();
	hup_block();
	trm_block();
}

int
start_timer(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (alm_catch())
		return STRLOG_FAILURE;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return STRLOG_FAILURE;
	alm_signal = 0;
	return STRLOG_SUCCESS;
}

void
strlog_exit(int retval)
{
	syslog(MY_FACILITY(LOG_NOTICE), "%s: Exiting %d", program, retval);
	fflush(stdout);
	fflush(stderr);
	sig_block();
	closelog();
	exit(retval);
}

void
strlog_enter(int argc, char *argv[])
{
	if (nomead) {
		pid_t pid;

		if ((pid = fork()) < 0) {
			perror(argv[0]);
			exit(2);
		} else if (pid != 0) {
			/* parent exits */
			exit(0);
		}
		setsid();	/* become a session leader */
		/* fork once more for SVR4 */
		if ((pid = fork()) < 0) {
			perror(argv[0]);
			exit(2);
		} else if (pid != 0) {
			if (nomead || pidfile[0] != '\0') {
				FILE *pidf;

				/* initialize default filename */
				if (pidfile[0] == '\0')
					snprintf(pidfile, sizeof(pidfile), "/var/run/%s.pid",
						 program);
				if (output > 1)
					syslog(MY_FACILITY(LOG_NOTICE),
					       "%s: Writing daemon pid to file %s", program,
					       pidfile);
				if ((pidf = fopen(pidfile, "w+"))) {
					fprintf(pidf, "%d", (int) pid);
					fflush(pidf);
					fclose(pidf);
				} else {
					syslog(MY_FACILITY(LOG_ERR), "%s: %m", program);
					syslog(MY_FACILITY(LOG_ERR),
					       "%s: Could not write pid to file %s", program,
					       pidfile);
					strlog_exit(2);
				}
			}
			/* parent exits */
			exit(0);
		}
		/* release current directory */
		if (chdir("/") < 0) {
			perror(argv[0]);
			exit(2);
		}
		umask(0);	/* clear file creation mask */
		/* rearrange file streams */
		fclose(stdin);
	}
	/* continue as foreground or background */
	openlog(NULL, LOG_CONS | LOG_NDELAY | (nomead ? 0 : LOG_PERROR), MY_FACILITY(0));
	if (basname[0] == '\0')
		snprintf(basname, sizeof(basname), loggername);
	if (nomead || outfile[0] != '\0') {
		struct tm tm;
		time_t curtime;

		time(&curtime);
		localtime_r(&curtime, &tm);
		/* initialize default filename */
		if (outfile[0] == '\0')
			snprintf(outpath, sizeof(outpath), "%s/%s.%02d-%02d", outpdir, basname,
				 tm.tm_mon, tm.tm_mday);
		else
			snprintf(outpath, sizeof(outpath), "%s/%s", outpdir, outfile);
		if (output > 1)
			syslog(MY_FACILITY(LOG_NOTICE), "%s: Redirecting stdout to file %s",
			       program, outpath);
		fflush(stdout);
		if (freopen(outpath, "a", stdout) == NULL) {
			syslog(MY_FACILITY(LOG_ERR), "%s: %m", program);
			syslog(MY_FACILITY(LOG_ERR), "%s: Could not redirect stdout to %s", program,
			       outpath);
			strlog_exit(2);
		}
	}
	if (nomead || errfile[0] != '\0') {
		/* initialize default filename */
		if (errfile[0] == '\0')
			snprintf(errpath, sizeof(errpath), "%s/%s.errors", outpdir, basname);
		else
			snprintf(errpath, sizeof(errpath), "%s/%s", outpdir, errfile);
		if (output > 1)
			syslog(MY_FACILITY(LOG_NOTICE), "%s: Redirecting stderr to file %s",
			       program, errpath);
		fflush(stderr);
		if (freopen(errpath, "a", stderr) == NULL) {
			syslog(MY_FACILITY(LOG_ERR), "%s: %m", program);
			syslog(MY_FACILITY(LOG_ERR), "%s: Could not redirect stderr to %s", program,
			       errpath);
			strlog_exit(2);
		}
	}
	sig_catch();
	// output_header();
	syslog(MY_FACILITY(LOG_NOTICE), "%s: Startup complete.", program);
}

void
strlog_open(int argc, char *argv[], struct trace_ids *tids, size_t count)
{
	struct strioctl ioc;

	/* open log device */
	if (devname[0] == '\0') {
		const char **dev;

		/* search if not specified */
		if (debug)
			fprintf(stderr, "%s: searching for log device\n", argv[0]);
		for (dev = logdevices; (*dev); dev++) {
			if (debug)
				fprintf(stderr, "%s: trying device %s\n", argv[0], (*dev));
			if ((strlog_fd = open((*dev), O_RDWR)) == 0)
				break;
		}
		if ((*dev) == NULL) {
			perror(argv[0]);
			strlog_exit(1);
		}
	} else {
		if (debug)
			fprintf(stderr, "%s: opening specified device %s\n", argv[0], devname);
		if ((strlog_fd = open(devname, O_RDWR)) < 0) {
			perror(argv[0]);
			strlog_exit(1);
		}
	}
	if (debug)
		fprintf(stderr, "%s: success\n", argv[0]);
	/* setup log device for logging */
	ioc.ic_cmd = I_ERRLOG;
	ioc.ic_timout = 0;
	ioc.ic_len = count * sizeof(struct trace_ids);
	ioc.ic_dp = (char *) tids;
	if (debug)
		fprintf(stderr, "%s: performing ioctl on log device\n", argv[0]);
	if (ioctl(strlog_fd, I_STR, &ioc) < 0) {
		perror(argv[0]);
		strlog_exit(1);
	}
	if (debug)
		fprintf(stderr, "%s: success\n", argv[0]);
}

void
strlog_close(int argc, char *argv[])
{
	if (close(strlog_fd) < 0)
		perror(argv[0]);
}

int
main(int argc, char *argv[])
{
	while (1) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"admin",	required_argument,	NULL, 'a'},
			{"nodaemon",	no_argument,		NULL, 'n'},
			{"directory",	required_argument,	NULL, 'd'},
			{"basename",	required_argument,	NULL, 'b'},
			{"outfile",	required_argument,	NULL, 'o'},
			{"errfile",	required_argument,	NULL, 'e'},
			{"pidfile",	required_argument,	NULL, 'p'},
			{"logdev",	required_argument,	NULL, 'l'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "a:d:nb:o:e:p:l:qD::v::hVC?W:", long_options,
				     &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "a:d:nb:o:e:p:l:qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'n':	/* -n, --nodaemon */
			if (debug)
				fprintf(stderr, "%s: suppressing deamon mode\n", argv[0]);
			nomead = 0;
			break;
		case 'a':	/* -a, --admin MAILID */
			if (debug)
				fprintf(stderr, "%s: setting mail id to %s\n", argv[0], optarg);
			strncpy(mailuid, optarg, 256);
			break;
		case 'd':	/* -d, --directory DIRECTORY */
			strncpy(outpdir, optarg, 256);
			break;
		case 'b':	/* -b, --basename BASNAME */
			strncpy(basname, optarg, 256);
			break;
		case 'o':	/* -o, --outfile OUTFILE */
			strncpy(outfile, optarg, 256);
			break;
		case 'e':	/* -e, --errfile ERRFILE */
			strncpy(errfile, optarg, 256);
			break;
		case 'p':	/* -p, --pidfile PIDFILE */
			if (debug)
				fprintf(stderr, "%s: setting pid file to %s\n", argv[0], optarg);
			strncpy(pidfile, optarg, 256);
			break;
		case 'l':	/* -l, --logdev DEVNAME */
			if (debug)
				fprintf(stderr, "%s: setting device name to %s\n", argv[0], optarg);
			strncpy(devname, optarg, 256);
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			debug = 0;
			output = 0;
			break;
		case 'D':	/* -D, --debug [level] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
			} else {
				if ((val = strtol(optarg, NULL, 0)) < 0)
					goto bad_option;
				debug = val;
			}
			if (debug)
				nomead = 0;
			break;
		case 'v':	/* -v, --verbose [level] */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
		      bad_nonopt:
			if (output || debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: syntax error near '", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "'\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	{
		int count = 0;
		struct trace_ids *tids = NULL;

		if (optind < argc)
			goto bad_nonopt;
		/* open log device */
		strlog_open(argc, argv, tids, count);
	}
	strlog_enter(argc, argv);
	for (;;) {
		struct pollfd pfd[] = {
			{strlog_fd, POLLIN | POLLPRI | POLLERR | POLLHUP, 0}
		};

		if (trm_signal)
			trm_action();
		if (hup_signal)
			hup_action();
		if (alm_signal)
			alm_action();
		if (output > 2)
			fprintf(stderr, "entering poll loop\n");
		switch (poll(pfd, 1, -1)) {
		case -1:
			if (errno == EAGAIN || errno == EINTR || errno == ERESTART)
				continue;
			syslog(MY_FACILITY(LOG_ERR), "%s: %m", program);
			syslog(MY_FACILITY(LOG_ERR), "%s: poll error", program);
			strlog_exit(1);
			return STRLOG_FAILURE;
		case 0:
			return STRLOG_NONE;
		case 1:
			if (pfd[0].revents & (POLLIN | POLLPRI)) {
				int ret, flags;
				char cbuf[1024];
				char dbuf[2048];
				struct strbuf ctl = { 1024, 1024, cbuf };
				struct strbuf dat = { 2048, 2048, dbuf };
				struct log_ctl *lc;
				char sbuf[1024];
				char fchar[] = "          ";
				char *fstr = fchar;

				if ((ret = getmsg(strlog_fd, &ctl, &dat, &flags)) < 0) {
					perror(argv[0]);
					exit(1);
				}
				if (ret != 0) {
					errno = ERANGE;
					perror(argv[0]);
					exit(1);
				}
				lc = (struct log_ctl *) cbuf;
				if (ctl.len < sizeof(*lc))
					continue;
				if (dat.len <= 0)
					continue;
				if (nomead && outfile[0] != '\0') {
					time_t ltime = lc->ltime;

					snprintf_text(sbuf, sizeof(sbuf), dbuf, dat.len);
					fprintf(stdout, "%d", lc->seq_no);
					fprintf(stdout, " %s", ctime(&ltime));
					fprintf(stdout, "%lu", (unsigned long) lc->ttime);
					fprintf(stdout, "%3d", lc->level);
					if (lc->flags & SL_TRACE)
						*fstr++ = 'T';
					if (lc->flags & SL_FATAL)
						*fstr++ = 'F';
					if (lc->flags & SL_NOTIFY)
						*fstr++ = 'N';
					*fstr++ = '\0';
					fprintf(stdout, "%s", fchar);
					fprintf(stdout, "%d", lc->mid);
					fprintf(stdout, "%d", lc->sid);
					fprintf(stdout, "%s", sbuf);
					fprintf(stdout, "\n");
					fflush(stdout);
				}
			}
			if (pfd[0].revents & POLLNVAL) {
				syslog(MY_FACILITY(LOG_ERR), "%s: device invalid", program);
				strlog_exit(1);
				return STRLOG_FAILURE;
			}
			if (pfd[0].revents & POLLHUP) {
				syslog(MY_FACILITY(LOG_ERR), "%s: device hangup", program);
				strlog_exit(1);
				return STRLOG_FAILURE;
			}
			if (pfd[0].revents & POLLERR) {
				syslog(MY_FACILITY(LOG_ERR), "%s: device error", program);
				strlog_exit(1);
				return STRLOG_FAILURE;
			}
			break;
		default:
			syslog(MY_FACILITY(LOG_ERR), "%s: poll error", program);
			strlog_exit(1);
			return STRLOG_FAILURE;
		}
	}
	strlog_close(argc, argv);
	exit(0);
}
