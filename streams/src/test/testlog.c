/*****************************************************************************

 @(#) $RCSfile: testlog.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/13 22:46:35 $

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

 Last Modified $Date: 2007/08/13 22:46:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testlog.c,v $
 Revision 0.9.2.2  2007/08/13 22:46:35  brian
 - GPLv3 header updates

 Revision 0.9.2.1  2006/11/26 15:27:44  brian
 - testing and corrections to strlog capabilities

 *****************************************************************************/

#ident "@(#) $RCSfile: testlog.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/13 22:46:35 $"

static char const ident[] =
    "$RCSfile: testlog.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/13 22:46:35 $";

/*
 *  This is a little user space program to test the correctness of the formatting of the data blocks
 *  used by the strlog function and the log driver.
 */
#define _XOPEN_SOURCE 600
#define _GNU_SOURCE 1

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
#include <stdarg.h>

#if 0
#define SL_ERROR    0x0001
#define SL_TRACE    0x0002
#define SL_NOTIFY   0x0004
#define SL_CONSOLE  0x0008
#define SL_FATAL    0x0010
#define SL_WARN	    0x0020
#define SL_NOTE	    0x0040
#define SL_NOPUTBUF 0x0080	/* uw7 compatibility */

#define LOGMSGSZ    1024
#define NLOGARGS    20

struct log_ctl {
	int16_t mid;
	int16_t sid;
	char level;
	int16_t flags;
	int32_t ltime;
	int32_t ttime;
	int32_t seq_no;
	int32_t pri;
};
#else
#include <sys/strlog.h>
#endif

#define BUFSIZE 4096

unsigned char dbuf[BUFSIZE];
struct log_ctl cbuf = { 0, };
size_t clen = sizeof(cbuf);

struct strbuf ctrl = {.buf = (char *) &cbuf,.len = sizeof(cbuf),.maxlen = sizeof(cbuf) };
struct strbuf data = {.buf = (char *) dbuf,.len = sizeof(dbuf),.maxlen = sizeof(dbuf) };

#define PROMOTE_TYPE	    int
#define PROMOTE_SIZE	    (sizeof(PROMOTE_TYPE))
#define PROMOTE_ALIGN(__x)  (((__x) + PROMOTE_SIZE - 1) & ~(PROMOTE_SIZE - 1))
#define PROMOTE_SIZEOF(__x) ((sizeof(__x) < PROMOTE_SIZE) ? PROMOTE_SIZE : sizeof(__x))

static int
log_alloc_data(struct strbuf *buf, const char *fmt, va_list args)
{
	unsigned char *bp;
	size_t len = strnlen(fmt, LOGMSGSZ);
	size_t plen = PROMOTE_ALIGN(len + 1);

	bp = (unsigned char *) buf->buf; {
		va_list args2;
		size_t nargs = 0, alen = 0;
		char type;
		unsigned char *dp;

		buf->len = plen;
		buf->maxlen = plen;

		va_copy(args2, args);
		/* two passes, one for sizing the next for argument list preparation */
		for (; *fmt; ++fmt, bp++) {
			if ((*bp = *fmt) != '%')
				continue;
			/* flags */
			for (++fmt, bp++;; ++fmt, bp++) {
				switch ((*bp = *fmt)) {
				case '-':
				case '+':
				case ' ':
				case '#':
				case '0':
					continue;
				default:
					break;
				}
				break;
			}
			/* field width */
			if (isdigit((*bp = *fmt))) {
				++fmt;
				bp++;
				while (isdigit((*bp = *fmt))) {
					++fmt;
					bp++;
				}
			} else if ((*bp = *fmt) == '*') {
				++fmt;
				bp++;
				alen += PROMOTE_SIZEOF(int);
				(void) va_arg(args2, int);

				if (++nargs == NLOGARGS)
					break;
			}
			/* get precision */
			if ((*bp = *fmt) == '.') {
				++fmt;
				bp++;
				if (isdigit((*bp = *fmt))) {
					++fmt;
					bp++;
					while (isdigit((*bp = *fmt))) {
						++fmt;
						bp++;
					}
				} else if ((*bp = *fmt) == '*') {
					++fmt;
					bp++;
					alen += PROMOTE_SIZEOF(int);
					(void) va_arg(args2, int);

					if (++nargs == NLOGARGS)
						break;
				}
			}
			/* get qualifier */
			type = 'i';
			switch ((*bp = *fmt)) {
			case 'h':
				type = *fmt;
				++fmt;
				bp++;
				if ((*bp = *fmt) == 'h') {
					++fmt;
					bp++;
					type = 'c';
				}
				break;
			case 'l':
				type = *fmt;
				++fmt;
				bp++;
				if ((*bp = *fmt) == 'l') {
					++fmt;
					bp++;
					type = 'L';
				}
				break;
			case 'q':
			case 'L':
			case 'Z':
			case 'z':
			case 't':
				type = *fmt;
				++fmt;
				bp++;
				break;
			}
			switch ((*bp = *fmt)) {
			case 'c':
			case 'p':
			case 'o':
			case 'X':
			case 'x':
			case 'd':
			case 'i':
			case 'u':
				switch (*fmt) {
				case 'c':
					type = 'c';
					break;
				case 'p':
					type = 'p';
					break;
				}
				switch (type) {
				case 'p':
					alen += PROMOTE_SIZEOF(void *);
					(void) va_arg(args2, void *);

					break;
				case 't':
					alen += PROMOTE_SIZEOF(ptrdiff_t);
					(void) va_arg(args2, ptrdiff_t);
					break;
				case 'l':
					alen += PROMOTE_SIZEOF(long);
					(void) va_arg(args2, long);

					break;
				case 'q':
				case 'L':
					alen += PROMOTE_SIZEOF(long long);
					(void) va_arg(args2, long long);

					break;
				case 'Z':
				case 'z':
					alen += PROMOTE_SIZEOF(size_t);
					(void) va_arg(args2, size_t);

					break;
				case 'h':
					alen += PROMOTE_SIZEOF(short);
					(void) va_arg(args2, int);

					break;
				case 'c':
					alen += PROMOTE_SIZEOF(char);
					(void) va_arg(args2, int);

					break;
				case 'i':
				default:
					alen += PROMOTE_SIZEOF(int);
					(void) va_arg(args2, int);

					break;
				}
				if (++nargs == NLOGARGS)
					break;
				continue;
			case 's':
			{
				char *s = va_arg(args2, char *);
				size_t slen = strnlen(s, LOGMSGSZ);
				size_t splen = PROMOTE_ALIGN(slen + 1);

				alen += splen;
				if (++nargs == NLOGARGS)
					break;
				continue;
			}
			case '%':
			default:
				continue;
			}
			break;
		}
		*bp++ = '\0';	/* terminate format string */
		va_end(args2);
		/* pass through once more with arguments */
		dp = (unsigned char *) buf->buf + plen; {
			buf->len = plen + alen;
			buf->maxlen = plen + alen;

			fmt = (const char *) buf->buf;

			for (; *fmt; ++fmt) {
				if (*fmt != '%')
					continue;
				/* flags */
				for (++fmt;; ++fmt) {
					switch (*fmt) {
					case '-':
					case '+':
					case ' ':
					case '#':
					case '0':
						continue;
					default:
						break;
					}
					break;
				}
				/* field width */
				if (isdigit(*fmt)) {
					++fmt;
					while (isdigit(*fmt)) {
						++fmt;
					}
				} else if (*fmt == '*') {
					++fmt;
					*(int *) dp = va_arg(args, int);
					dp += PROMOTE_SIZEOF(int);

					if (!--nargs)
						break;
				}
				/* get precision */
				if (*fmt == '.') {
					++fmt;
					if (isdigit(*fmt)) {
						++fmt;
						while (isdigit(*fmt)) {
							++fmt;
						}
					} else if (*fmt == '*') {
						++fmt;
						*(int *) dp = va_arg(args, int);
						dp += PROMOTE_SIZEOF(int);

						if (!--nargs)
							break;
					}
				}
				/* get qualifier */
				type = 'i';
				switch (*fmt) {
				case 'h':
					++fmt;
					if (*fmt == 'h') {
						type = 'c';
						++fmt;
					}
					break;
				case 'l':
					++fmt;
					if (*fmt == 'l') {
						type = 'L';
						++fmt;
					}
					break;
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
				case 'n':
				case 'p':
				case 'o':
				case 'X':
				case 'x':
				case 'd':
				case 'i':
				case 'u':
					switch (*fmt) {
					case 'c':
						type = 'c';
						break;
					case 'p':
						type = 'p';
						break;
					}
					switch (type) {
					case 'p':
						*(void **) dp = va_arg(args, void *);
						dp += PROMOTE_SIZEOF(void *);

						break;
					case 'l':
						*(long *) dp = va_arg(args, long);
						dp += PROMOTE_SIZEOF(long);

						break;
					case 'L':
						*(long long *) dp = va_arg(args, long long);
						dp += PROMOTE_SIZEOF(long long);

						break;
					case 'Z':
					case 'z':
						*(size_t *) dp = va_arg(args, size_t);
						dp += PROMOTE_SIZEOF(size_t);

						break;
					case 't':
						*(ptrdiff_t *) dp = va_arg(args, ptrdiff_t);
						dp += PROMOTE_SIZEOF(ptrdiff_t);
						break;
					case 'h':
						*(int *) dp = va_arg(args, int);
						dp += PROMOTE_SIZEOF(short);

						break;
					case 'c':
						*(int *) dp = va_arg(args, int);
						dp += PROMOTE_SIZEOF(char);

						break;
					default:
						*(int *) dp = va_arg(args, int);
						dp += PROMOTE_SIZEOF(int);

						break;
					}
					if (!--nargs)
						break;
					continue;
				case 's':
				{
					char *s = va_arg(args, char *);
					size_t slen = strnlen(s, LOGMSGSZ);
					size_t splen = PROMOTE_ALIGN(slen + 1);

					strncpy((char *) dp, s, slen);
					dp[slen] = '\0';
					dp += splen;
					if (!--nargs)
						break;
					continue;
				}
				case '%':
				default:
					continue;
				}
				break;
			}
		}
	}
	return (0);
}

static void
my_vstrlog(short mid, short sid, char level, unsigned short flags, char *fmt, va_list args)
{
	static int sequence = 10000;
	struct log_ctl *lp = (typeof(lp)) ctrl.buf;
	struct timeval tv;
	int err, pri, band, flag;

	if (flags & SL_WARN)
		pri = LOG_WARNING;
	else if (flags & SL_FATAL)
		pri = LOG_ERR;
	else if (flags & SL_NOTE)
		pri = LOG_NOTICE;
	else if (flags & SL_TRACE)
		pri = LOG_DEBUG;
	else
		pri = LOG_INFO;

	if (flags & SL_CONSOLE) {
		band = 0;
		flag = MSG_HIPRI;
	} else if (flags & SL_ERROR) {
		band = 0;
		flag = MSG_HIPRI;
	} else if (flags & SL_TRACE) {
		band = pri;
		flag = MSG_BAND;
	} else {
		band = pri;
		flag = MSG_BAND;
	}

	lp->mid = mid;
	lp->sid = sid;
	lp->level = level;
	lp->flags = flags;
	lp->ttime = time(NULL);
	gettimeofday(&tv, NULL);
	lp->ltime = tv.tv_sec;
	lp->seq_no = 0;
	lp->pri = pri | LOG_USER;

	log_alloc_data(&data, fmt, args);
}

static void my_strlog(short mid, short sid, char level, unsigned short flag, char *fmt, ...)
    __attribute__ ((__format__(__printf__, 5, 6)));

static void
my_strlog(short mid, short sid, char level, unsigned short flag, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	my_vstrlog(mid, sid, level, flag, fmt, args);
	va_end(args);
}

int
main()
{
	my_strlog(1, 2, 3, SL_TRACE, "test format");
	pstrlog(stdout, &ctrl, &data);
	my_strlog(2, 2, 3, SL_TRACE, "test format2 %d", 45);
	pstrlog(stdout, &ctrl, &data);
	my_strlog(3, 2, 3, SL_TRACE, "test format2 %d %c", 45, 'q');
	pstrlog(stdout, &ctrl, &data);
	my_strlog(4, 2, 3, SL_TRACE, "test format2 %d %c %p", 45, 'q', data.buf);
	pstrlog(stdout, &ctrl, &data);
	my_strlog(5, 2, 3, SL_TRACE, "test format2 %d %c %s", 45, 'q', "green");
	pstrlog(stdout, &ctrl, &data);
	my_strlog(6, 2, 3, SL_TRACE, "test format2 %d %s %c", 45, "green", 'q');
	pstrlog(stdout, &ctrl, &data);
	my_strlog(7, 2, 3, SL_TRACE, "test format2 %d %s", 45, "green");
	pstrlog(stdout, &ctrl, &data);
	my_strlog(8, 2, 3, SL_TRACE, "test format2 %#x %s", 45, "green");
	pstrlog(stdout, &ctrl, &data);
	my_strlog(9, 2, 3, SL_TRACE, "test format2 %#0*x %s", 16, 45, "green");
	pstrlog(stdout, &ctrl, &data);
	return (0);
}
