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

/* logger.c - system logging routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/logger.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/logger.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: logger.c,v
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
#define getdtablesize() (sysconf (_SC_OPEN_MAX))
#include <stdio.h>

#if defined HAVE_VARARGS_H
#include <varargs.h>
#else				/* defined HAVE_VARARGS_H */
#include <stdarg.h>
#endif				/* !defined HAVE_VARARGS_H */

#include "general.h"
#include "manifest.h"
#include "logger.h"
#include "tailor.h"

#ifdef	NULL
#undef	NULL
#endif

#include <sys/param.h>
#ifndef	NULL
#define	NULL	0
#endif

#include "sys.file.h"
#include <sys/stat.h>
#ifndef	SYS5
#include <syslog.h>
extern void closelog();
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif				/* HAVE_STRING_H */
#include <time.h>

#ifndef	lint
static
#endif
int _ll_printf();

struct ll_private {
	int ll_checks;
#define	CHKINT	15			/* call ll_check 1 in every 15 uses... */
};

static struct ll_private *llp = NULL;

static int (*_ll_header_routine)() = ll_defmhdr;

long lseek();

int
ll_open(lp)
	register LLog *lp;
{
	int mask, mode;
	char *bp, buffer[BUFSIZ];

#ifdef linux
	(void) rcsid;
#endif
/* BSD code
    if (llp == NULL
	    && (llp = (struct ll_private *)
			calloc ((unsigned int) getdtablesize (),
				sizeof *llp)) == NULL)
	goto you_lose;
*/
	if (llp == NULL && (llp = (struct ll_private *)
			    calloc((unsigned int) sysconf(_SC_OPEN_MAX), sizeof *llp)) == NULL)
		goto you_lose;
	if (lp->ll_file == NULLCP || *lp->ll_file == '\0') {
	      you_lose:;
		(void) ll_close(lp);
		lp->ll_stat |= LLOGERR;
		return NOTOK;
	}
	lp->ll_stat &= ~LLOGERR;
	if (lp->ll_fd != NOTOK)
		return OK;
	if (strcmp(lp->ll_file, "-") == 0) {
		lp->ll_stat |= LLOGTTY;
		return OK;
	}
	(void) sprintf(bp = buffer, _isodefile(isodelogpath, lp->ll_file), getpid());
	mode = O_WRONLY | O_APPEND;
	if (lp->ll_stat & LLOGCRT)
		mode |= O_CREAT;
	mask = umask(~0666);
	lp->ll_fd = open(bp, mode, 0666);
	(void) umask(mask);
	if (ll_check(lp) == NOTOK)
		return (NOTOK);
	if (lp->ll_fd != NOTOK)
		llp[lp->ll_fd].ll_checks = CHKINT;
	return (lp->ll_fd != NOTOK ? OK : NOTOK);
}

int
ll_close(lp)
	register LLog *lp;
{
	int status;

	if (lp->ll_fd == NOTOK)
		return OK;
	status = close(lp->ll_fd);
	lp->ll_fd = NOTOK;
	return status;
}

#ifdef HAVE_VARARGS_H
#ifndef	lint
int
ll_log(va_alist)
	va_dcl
{
	int event, result;
	LLog *lp;
	va_list ap;

	va_start(ap);
	lp = va_arg(ap, LLog *);
	event = va_arg(ap, int);

	result = _ll_log(lp, event, ap);
	va_end(ap);
	return result;
}
#else
/* VARARGS4 */
int
ll_log(lp, event, what, fmt)
	LLog *lp;
	int event;
	char *what, *fmt;
{
	return ll_log(lp, event, what, fmt);
}
#endif

int
_ll_log(lp, event, ap)			/* what, fmt, args ... */
	register LLog *lp;
	int event;
	va_list ap;
{
	int cc, status;
	register char *bp;
	char *what, buffer[BUFSIZ];

	if (!(lp->ll_events & event))
		return OK;
	bp = buffer;
	/* Create header */
	(*_ll_header_routine) (bp, lp->ll_hdr, lp->ll_dhdr);
	bp += strlen(bp);
	what = va_arg(ap, char *);
	_xsprintf(bp, what, ap);
#ifndef	SYS5
	if (lp->ll_syslog & event) {
		int priority;

		switch (event) {
		case LLOG_FATAL:
			priority = LOG_ERR;
			break;
		case LLOG_EXCEPTIONS:
			priority = LOG_WARNING;
			break;
		case LLOG_NOTICE:
			priority = LOG_INFO;
			break;
		case LLOG_PDUS:
		case LLOG_TRACE:
		case LLOG_DEBUG:
			priority = LOG_DEBUG;
			break;
		default:
			priority = LOG_NOTICE;
			break;
		}
		(void) syslog(priority, "%s", buffer + 13);
		if (lp->ll_stat & LLOGCLS)
			(void) closelog();
	}
#endif
	if (!(lp->ll_stat & LLOGTTY)
	    && lp->ll_fd == NOTOK && strcmp(lp->ll_file, "-") == 0)
		lp->ll_stat |= LLOGTTY;
	if (lp->ll_stat & LLOGTTY) {
		(void) fflush(stdout);
		if (lp->ll_fd != NOTOK)
			(void) fprintf(stderr, "LOGGING: ");
		(void) fputs(bp, stderr);
		(void) fputc('\n', stderr);
		(void) fflush(stderr);
	}
	bp += strlen(bp);
	if (lp->ll_fd == NOTOK) {
		if ((lp->ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
			return OK;
		if (ll_open(lp) == NOTOK)
			return NOTOK;
	} else if ((!llp || llp[lp->ll_fd].ll_checks-- < 0)
		   && ll_check(lp) == NOTOK)
		return NOTOK;
	*bp++ = '\n', *bp = NULL;
	cc = bp - buffer;
	if ((status = write(lp->ll_fd, buffer, cc)) != cc) {
		if (status == NOTOK) {
			(void) ll_close(lp);
		      error:;
			lp->ll_stat |= LLOGERR;
			return NOTOK;
		}
		status = NOTOK;
	} else
		status = OK;
	if ((lp->ll_stat & LLOGCLS) && ll_close(lp) == NOTOK)
		goto error;
	return status;
}
#else				/* HAVE_VARARGS_H */

/* GCC no longer supports the old varargs.h implementation and will #error and
 * beltch if you try to include <varags.h>.  It complains that all varargs
 * implementations need to be updated to use stdarg.h.  This is the
 * reimplementation of the above using stdard.h. */

int
ll_log(LLog * lp, int event, const char *what, const char *fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = _ll_log(lp, event, what, fmt, ap);
	va_end(ap);
	return (result);
}

int
_ll_log(LLog * lp, int event, const char *what, const char *fmt, va_list ap)
{
	int cc, status;
	register char *bp;
	char buffer[BUFSIZ];

	if (!(lp->ll_events & event))
		return OK;
	bp = buffer;
	/* Create header */
	(*_ll_header_routine) (bp, lp->ll_hdr, lp->ll_dhdr);
	bp += strlen(bp);
	_xsprintf(bp, what, fmt, ap);
	if (lp->ll_syslog & event) {
		int priority;

		switch (event) {
		case LLOG_FATAL:
			priority = LOG_ERR;
			break;
		case LLOG_EXCEPTIONS:
			priority = LOG_WARNING;
			break;
		case LLOG_NOTICE:
			priority = LOG_INFO;
			break;
		case LLOG_PDUS:
		case LLOG_TRACE:
		case LLOG_DEBUG:
			priority = LOG_DEBUG;
			break;
		default:
			priority = LOG_NOTICE;
			break;
		}
		(void) syslog(priority, "%s", buffer + 13);
		if (lp->ll_stat & LLOGCLS)
			(void) closelog();
	}
	if (!(lp->ll_stat & LLOGTTY)
	    && lp->ll_fd == NOTOK && strcmp(lp->ll_file, "-") == 0)
		lp->ll_stat |= LLOGTTY;
	if (lp->ll_stat & LLOGTTY) {
		(void) fflush(stdout);
		if (lp->ll_fd != NOTOK)
			(void) fprintf(stderr, "LOGGING: ");
		(void) fputs(bp, stderr);
		(void) fputc('\n', stderr);
		(void) fflush(stderr);
	}
	bp += strlen(bp);
	if (lp->ll_fd == NOTOK) {
		if ((lp->ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
			return OK;
		if (ll_open(lp) == NOTOK)
			return NOTOK;
	} else if ((!llp || llp[lp->ll_fd].ll_checks-- < 0)
		   && ll_check(lp) == NOTOK)
		return NOTOK;
	*bp++ = '\n', *bp = '\0';
	cc = bp - buffer;
	if ((status = write(lp->ll_fd, buffer, cc)) != cc) {
		if (status == NOTOK) {
			(void) ll_close(lp);
		      error:;
			lp->ll_stat |= LLOGERR;
			return NOTOK;
		}
		status = NOTOK;
	} else
		status = OK;
	if ((lp->ll_stat & LLOGCLS) && ll_close(lp) == NOTOK)
		goto error;
	return status;
}

#endif				/* HAVE_VARARGS_H */

void
ll_hdinit(lp, prefix)
	register LLog *lp;
	char *prefix;
{
	register char *cp, *up;
	char buffer[BUFSIZ], user[10];

	if (prefix == NULLCP) {
		if ((lp->ll_stat & LLOGHDR) && strlen(lp->ll_hdr) == 25)
			(cp = lp->ll_hdr)[8] = '\0';
		else
			cp = "unknown";
	} else {
		if ((cp = rindex(prefix, '/')))
			cp++;
		if (cp == NULL || *cp == '\0')
			cp = prefix;
	}
	if ((up = getenv("USER")) == NULLCP && (up = getenv("LOGNAME")) == NULLCP) {
		(void) sprintf(user, "#%d", getuid());
		up = user;
	}
	(void) sprintf(buffer, "%-8.8s %05d (%-8.8s)", cp, getpid() % 100000, up);
	if (lp->ll_stat & LLOGHDR)
		free(lp->ll_hdr);
	lp->ll_stat &= ~LLOGHDR;
	if ((lp->ll_hdr = malloc((unsigned) (strlen(buffer) + 1))) == NULLCP)
		return;
	(void) strcpy(lp->ll_hdr, buffer);
	lp->ll_stat |= LLOGHDR;
}

void
ll_dbinit(lp, prefix)
	register LLog *lp;
	char *prefix;
{
	register char *cp;
	char buffer[BUFSIZ];

	ll_hdinit(lp, prefix);
	if (prefix) {
		if ((cp = rindex(prefix, '/')))
			cp++;
		if (cp == NULL || *cp == '\0')
			cp = prefix;
		(void) sprintf(buffer, "./%s.log", cp);
		if ((lp->ll_file = malloc((unsigned) (strlen(buffer) + 1)))
		    == NULLCP)
			return;
		(void) strcpy(lp->ll_file, buffer);
	}
	lp->ll_events |= LLOG_ALL;
	lp->ll_stat |= LLOGTTY;
}

#if defined HAVE_VARARGS_H
#ifndef	lint
int
ll_printf(va_alist)
	va_dcl
{
	int result;
	LLog *lp;
	va_list ap;

	va_start(ap);
	lp = va_arg(ap, LLog *);
	result = _ll_printf(lp, ap);
	va_end(ap);
	return result;
}
#else
/* VARARGS2 */
int
ll_printf(lp, fmt)
	LLog *lp;
	char *fmt;
{
	return ll_printf(lp, fmt);
}
#endif
#ifndef	lint
static
#endif
    int
_ll_printf(lp, ap)			/* fmt, args ... */
	register LLog *lp;
	va_list ap;
{
	int cc, status;
	register char *bp;
	char buffer[BUFSIZ];
	char *fmt;
	va_list fp;

	fp = ap;
	fmt = va_arg(fp, char *);

	if (strcmp(fmt, "%s") != 0) {
		bp = buffer;
		_xsprintf(bp, NULLCP, ap);
	} else {
		bp = NULL;
		fmt = va_arg(fp, char *);
	}
	if (!(lp->ll_stat & LLOGTTY)
	    && lp->ll_fd == NOTOK && strcmp(lp->ll_file, "-") == 0)
		lp->ll_stat |= LLOGTTY;
	if (lp->ll_stat & LLOGTTY) {
		(void) fflush(stdout);
		if (bp)
			(void) fputs(bp, stderr);
		else
			(void) fputs(fmt, stderr);
		(void) fflush(stderr);
	}
	if (bp)
		bp += strlen(bp);
	if (lp->ll_fd == NOTOK) {
		if ((lp->ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
			return OK;
		if (ll_open(lp) == NOTOK)
			return NOTOK;
	} else if ((!llp || llp[lp->ll_fd].ll_checks-- < 0)
		   && ll_check(lp) == NOTOK)
		return NOTOK;
	if (bp)
		cc = bp - buffer;
	else
		cc = strlen(fmt);
	if ((status = write(lp->ll_fd, bp ? buffer : fmt, cc)) != cc) {
		if (status == NOTOK) {
			(void) ll_close(lp);
			lp->ll_stat |= LLOGERR;
			return NOTOK;
		}
		status = NOTOK;
	} else
		status = OK;
	return status;
}
#else				/* defined HAVE_VARARGS_H */

/* GCC does not support varargs.h. anymore.  This is the converison to
 * stdarg.h. */

int
ll_printf(LLog * lp, const char *fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = _ll_printf(lp, fmt, ap);
	va_end(ap);
	return (result);
}

int
_ll_printf(LLog * lp, const char *fmt, va_list ap)
{
	int cc, status;
	register char *bp;
	char buffer[BUFSIZ];

	if (strcmp(fmt, "%s") != 0) {
		bp = buffer;
		_xsprintf(bp, NULLCP, fmt, ap);
	} else {
		bp = NULL;
	}
	if (!(lp->ll_stat & LLOGTTY)
	    && lp->ll_fd == NOTOK && strcmp(lp->ll_file, "-") == 0)
		lp->ll_stat |= LLOGTTY;
	if (lp->ll_stat & LLOGTTY) {
		(void) fflush(stdout);
		if (bp)
			(void) fputs(bp, stderr);
		else
			(void) fputs(fmt, stderr);
		(void) fflush(stderr);
	}
	if (bp)
		bp += strlen(bp);
	if (lp->ll_fd == NOTOK) {
		if ((lp->ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
			return OK;
		if (ll_open(lp) == NOTOK)
			return NOTOK;
	} else if ((!llp || llp[lp->ll_fd].ll_checks-- < 0)
		   && ll_check(lp) == NOTOK)
		return NOTOK;
	if (bp)
		cc = bp - buffer;
	else
		cc = strlen(fmt);
	if ((status = write(lp->ll_fd, bp ? buffer : fmt, cc)) != cc) {
		if (status == NOTOK) {
			(void) ll_close(lp);
			lp->ll_stat |= LLOGERR;
			return NOTOK;
		}
		status = NOTOK;
	} else
		status = OK;
	return status;
}

#endif				/* defined HAVE_VARARGS_H */

int
ll_sync(lp)
	register LLog *lp;
{
	if (lp->ll_stat & LLOGCLS)
		return ll_close(lp);
	return OK;
}

/*  */

#if defined HAVE_VARARGS_H
#ifndef	lint
char *
ll_preset(va_alist)
	va_dcl
{
	va_list ap;
	static char buffer[BUFSIZ];

	va_start(ap);
	_xsprintf(buffer, NULLCP, ap);
	va_end(ap);
	return buffer;
}
#else
/* VARARGS1 */
char *
ll_preset(fmt)
	char *fmt;
{
	return ll_preset(fmt);
}
#endif
#else				/* defined HAVE_VARARGS_H */
/* GCC does not support varargs.h anymore, so here is the stdarg.h version. */
char *
ll_preset(const char *fmt, ...)
{
	va_list ap;
	static char buffer[BUFSIZ];

	va_start(ap, fmt);
	_xsprintf(buffer, NULLCP, fmt, ap);
	va_end(ap);
	return (buffer);
}
#endif				/* defined HAVE_VARARGS_H */

int
ll_check(lp)
	register LLog *lp;
{
#ifndef	BSD42
	int fd;
	char buffer[BUFSIZ];
#endif
	long size;
	struct stat st;

	if ((size = lp->ll_msize) <= 0)
		return OK;
	if (llp && lp->ll_fd != NOTOK)
		llp[lp->ll_fd].ll_checks = CHKINT;
	if (lp->ll_fd == NOTOK || (fstat(lp->ll_fd, &st) != NOTOK && st.st_size < (size <<= 10)))
		return OK;
	if (!(lp->ll_stat & LLOGZER)) {
		(void) ll_close(lp);
	      error:;
		lp->ll_stat |= LLOGERR;
		return NOTOK;
	}
#ifdef	BSD42
#ifdef	SUNOS4
	if (ftruncate(lp->ll_fd, (off_t) 0) == -1)
		goto error;
#else
	if (ftruncate(lp->ll_fd, 0) == -1)
		goto error;
#endif
	(void) lseek(lp->ll_fd, 0L, 0);
	return OK;
#else
	(void) sprintf(buffer, _isodefile(isodelogpath, lp->ll_file), getpid());
	if ((fd = open(buffer, O_WRONLY | O_APPEND | O_TRUNC)) == NOTOK)
		goto error;
	(void) close(fd);
	return OK;
#endif
}

/*
 * ll_defmhdr - Default "make header" routine.
 */
int
ll_defmhdr(bufferp, headerp, dheaderp)
	char *bufferp;			/* Buffer pointer */
	char *headerp;			/* Static header string */
	char *dheaderp;			/* Dynamic header string */
{
	time_t clock;
	register struct tm *tm;

	(void) time(&clock);
	tm = localtime(&clock);
	(void) sprintf(bufferp, "%2d/%2d %2d:%02d:%02d %s %s ", tm->tm_mon + 1, tm->tm_mday,
		       tm->tm_hour, tm->tm_min, tm->tm_sec, headerp ? headerp : "",
		       dheaderp ? dheaderp : "");
	return OK;
}

/*
 * ll_setmhdr - Set "make header" routine, overriding default.
 */
int (*ll_setmhdr(make_header_routine))()
	int (*make_header_routine)();
{
	int (*result)() = _ll_header_routine;

	_ll_header_routine = make_header_routine;
	return result;
}

#ifdef ULTRIX_X25
#ifdef ULTRIX_X25_DEMSA
char *
CAT(x, y)
	char *x;
	char *y;
{
	if (strlen(x) + strlen(y) - 2 > BUFSIZ - 1)
		return (char *) y;
	else {
		strcpy(our_global_buffer, x);
		strcat(our_global_buffer, y);
		return (char *) our_global_buffer;
	}
}
#endif
#endif
