/*****************************************************************************

 @(#) $RCSfile: strlog.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/26 15:27:35 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 2.1 of the License.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

 Last Modified $Date: 2006/11/26 15:27:35 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strlog.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/26 15:27:35 $"

static char const ident[] = "$RCSfile: strlog.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/26 15:27:35 $";

#include "streams.h"

#include <stddef.h>
#include <stdint.h>
#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>

#include <sys/strlog.h>

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

	if ((bp = malloc(plen))) {
		va_list args2;
		size_t nargs = 0, alen = 0;
		char type;
		unsigned char *dp;

		buf->buf = (char *)bp;
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
		if ((bp = realloc(buf->buf, plen + alen))) {
			buf->buf = (char *)bp;
			buf->len = plen + alen;
			buf->maxlen = plen + alen;

			fmt = (const char *)bp;
			dp = bp + plen;

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

					strncpy((char *)dp, s, slen);
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
		} else {
			free(buf->buf);
			return (-1);
		}
	} else {
		return (-1);
	}
	return (0);
}

/** @brief Sumbmit a STREAMS log message with variable argument list.
  * @param mid module identifier.
  * @param sid stream instance identifier.
  * @parma level priority level.
  * @param flags log flags specifying additional options.
  * @param fmt printf style format string.
  * @param args arguments to the printf style format string.
  * @version STREAMS_1.0
  */
int
__streams_vstrlog(short mid, short sid, char level, unsigned short flags, const char *fmt,
		  va_list args)
{
	static int log_fd = 0;
	struct strbuf ctrl = { 0, }, data = { 0, };
	struct log_ctl lc;
	struct timeval tv;
	int err, pri, band, flag;

	if (log_fd == 0) {
		if ((log_fd = open("/dev/strlog", O_RDWR)) < 0) {
			log_fd = 0;
			return (-1);
		}
	}

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

	ctrl.buf = (char *)&lc;
	ctrl.len = sizeof(lc);
	ctrl.maxlen = sizeof(lc);

	lc.mid = mid;
	lc.sid = sid;
	lc.level = level;
	lc.flags = flags;
	lc.ttime = time(NULL);
	gettimeofday(&tv, NULL);
	lc.ltime = tv.tv_sec;
	lc.seq_no  = 0;
	lc.pri = pri | LOG_USER;

	if (log_alloc_data(&data, fmt, args) != 0)
		return (-1);
	if (putpmsg(log_fd, &ctrl, &data, band, flag) == -1) {
		err = errno;
		free(data.buf);
		errno = err;
		return (-1);
	}
	free(data.buf);
	return (0);
}

/** @brief Sumbmit a STREAMS log message with variable arguments.
  * @param mid module identifier.
  * @param sid stream instance identifier.
  * @parma level priority level.
  * @param flags log flags specifying additional options.
  * @param fmt printf style format string.
  * @param ... arguments to the printf style format string.
  * @version STREAMS_1.0
  */
int __streams_strlog(short mid, short sid, char level, unsigned short flags, const char *fmt, ...)
    __attribute__ ((__format__(__printf__, 5, 6)));

int
__streams_strlog(short mid, short sid, char level, unsigned short flags, const char *fmt, ...)
{
	int result = 0;
	va_list args;

	va_start(args, fmt);
	result = __streams_vstrlog(mid, sid, level, flags, fmt, args);
	va_end(args);
	return (result);
}

#define FLAG_ZEROPAD	(1<<0)	/* pad with zero */
#define FLAG_SIGN	(1<<1)	/* unsigned/signed long */
#define FLAG_PLUS	(1<<2)	/* show plus */
#define FLAG_SPACE	(1<<3)	/* space if plus */
#define FLAG_LEFT	(1<<4)	/* left justified */
#define FLAG_SPECIAL	(1<<5)	/* 0x */
#define FLAG_LARGE	(1<<6)	/* use 'ABCDEF' instead of 'abcdef' */

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

int
__streams_pstrlog(FILE *file, struct strbuf *ctrl, struct strbuf *data)
{
	char sbuf[LOGMSGSZ<<2];
	char fchar[] = "          ";
	char *fstr = fchar, *tp;
	struct log_ctl lc;
	time_t ltime;
	char timebuf[26];

	if (!ctrl || !data || !ctrl->buf || !data->buf || ctrl->len < sizeof(lc)) {
		errno = -EINVAL;
		return (-1);
	}
	memcpy(&lc, ctrl->buf, sizeof(lc));

	snprintf_text(sbuf, sizeof(sbuf), (char *) data->buf, data->len);
	fprintf(file, "%d", lc.seq_no);
	ctime_r(&ltime, timebuf);
	for (tp = timebuf;; tp++) {
		if (*tp == '\n') {
			*tp = '\0';
			break;
		}
		if (tp == '\0')
			break;
	}
	fprintf(file, " %s", timebuf);
	fprintf(file, " %lu", (unsigned long) lc.ttime);
	fprintf(file, " %3d", lc.level);
	if (lc.flags & SL_ERROR)
		*fstr++ = 'E';
	if (lc.flags & SL_FATAL)
		*fstr++ = 'F';
	if (lc.flags & SL_NOTIFY)
		*fstr++ = 'N';
	*fstr++ = '\0';
	fprintf(file, " %s", fchar);
	fprintf(file, " %d", lc.mid);
	fprintf(file, " %d", lc.sid);
	fprintf(file, " %s", sbuf);
	fprintf(file, "\n");
	return (0);
}

/** @fn int vstrlog(short mid, short sid, char level, unsigned short flags, const char *fmt, va_list args)
  * @param mid module identifier.
  * @param sid stream instance identifier.
  * @parma level priority level.
  * @param flags log flags specifying additional options.
  * @param fmt printf style format string.
  * @param args arguments to the printf style format string.
  * @version STREAMS_1.0 __streams_vstrlog()
  */
__asm__(".symver __streams_vstrlog,vstrlog@@STREAMS_1.0");

/** @fn int strlog(short mid, short sid, char level, unsigned short flags, const char *fmt, ...)
  * @param mid module identifier.
  * @param sid stream instance identifier.
  * @parma level priority level.
  * @param flags log flags specifying additional options.
  * @param fmt printf style format string.
  * @param ... arguments to the printf style format string.
  * @version STREAMS_1.0 __streams_strlog()
  */
__asm__(".symver __streams_strlog,strlog@@STREAMS_1.0");

__asm__(".symver __streams_pstrlog,pstrlog@@STREAMS_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
