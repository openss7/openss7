/*****************************************************************************

 @(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/06/20 20:34:09 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/06/20 20:34:09 $ by $Author: brian $

 -----------------------------------------------------------------------------
 $Log: test-streams.c,v $
 Revision 0.9.2.5  2004/06/20 20:34:09  brian
 - Further debugging.

 Revision 1.5  2004/06/20 20:34:09  brian
 - Further debugging.

 Revision 1.4  2004/06/12 23:20:34  brian
 - Ioctl debugging.

 Revision 1.3  2004/06/09 08:32:59  brian
 - Open works fine but don't want to hold dentries in cache.

 Revision 1.2  2004/06/08 02:27:36  brian
 - Framework for testing streams.

 Revision 1.1  2004/06/07 22:35:57  brian
 - Starting test suites for everything.

 *****************************************************************************/

#ident "@(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/06/20 20:34:09 $"

static char const ident[] = "$RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/06/20 20:34:09 $";

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/uio.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/types.h>

/* 
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "Linux Fast-STREAMS";
static const char *spkgname = "LfS";
static const char *lstdname = "UNIX 98/SUS Version 2";
static const char *sstdname = "XSI";
static const char *shortname = "STREAMS";
static char devname[256] = "/dev/nuls";

static int exit_on_failure = 0;

static int verbose = 1;

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;

static int last_prim = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;

int test_fd[3] = { 0, 0, 0 };

#define BUFSIZE 5*4096

#define FFLUSH(stream)

#define SHORT_WAIT 10
#define NORMAL_WAIT 100
#define LONG_WAIT 500

char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strbuf ctrl = { BUFSIZE, -1, cbuf };
struct strbuf data = { BUFSIZE, -1, dbuf };

struct strfdinsert fdi = {
	{BUFSIZE, 0, cbuf},
	{BUFSIZE, 0, dbuf},
	0,
	0,
	0
};
int flags = 0;

struct timeval when;

/* 
 *  -------------------------------------------------------------------------
 *
 *  Events and Actions
 *
 *  -------------------------------------------------------------------------
 */
enum {
	__EVENT_NO_MSG = -6, __EVENT_TIMEOUT = -5, __EVENT_UNKNOWN = -4,
	__RESULT_DECODE_ERROR = -3, __RESULT_SCRIPT_ERROR = -2,
	__RESULT_INCONCLUSIVE = -1, __RESULT_SUCCESS = 0, __RESULT_FAILURE = 1,
};

/* 
 *  -------------------------------------------------------------------------
 */
int show = 1;

/* 
 *  -------------------------------------------------------------------------
 *
 *  Timer Functions
 *
 *  -------------------------------------------------------------------------
 */

/* 
 *  Timer values for tests: each timer has a low range (minus error margin)
 *  and a high range (plus error margin).
 */

static long timer_scale = 1;

#define TEST_TIMEOUT 5000

typedef struct timer_range {
	long lo;
	long hi;
} timer_range_t;

enum {
	t1 = 0, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15,
	t16, t17, t18, t19, t20, t21, t22, t23, t24, t25, t26, t27, t28, t29,
	t30, t31, t32, t33, t34, t35, t36, t37, t38, tmax
};

long test_start = 0;

static int state;

/* 
 *  Return the current time in milliseconds.
 */
static long now(void)
{
	long ret;
	struct timeval now;
	if (gettimeofday(&now, NULL)) {
		last_errno = errno;
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "***************ERROR! couldn't get time!            !  !                    \n");
		fprintf(stdout, "%20s! %-54s\n", __FUNCTION__, strerror(last_errno));
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
		return (0);
	}
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000L;
	ret += (now.tv_usec + 999L) / 1000L;
	return ret;
}
static long milliseconds(char *t)
{
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .               :               .  .                    \n");
		fprintf(stdout, "                    .             %6s            .  .                    <%d>\n", t, state);
		fprintf(stdout, "                    .               :               .  .                    \n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	return now();
}
static long milliseconds_2nd(char *t)
{
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .               :   :           .  .                    \n");
		fprintf(stdout, "                    .               : %6s        .  .                    <%d>\n", t, state);
		fprintf(stdout, "                    .               :   :           .  .                    \n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	return now();
}

/* 
 *  Check the current time against the beginning time provided as an argnument
 *  and see if the time inverval falls between the low and high values for the
 *  timer as specified by arguments.  Return SUCCESS if the interval is within
 *  the allowable range and FAILURE otherwise.
 */
static int check_time(const char *t, long i, long lo, long hi)
{
	float tol, dlo, dhi, itv;
	itv = i * timer_scale;
	dlo = lo;
	dhi = hi;
	tol = 100 * timer_scale;
	itv = itv / 1000;
	dlo = dlo / 1000;
	dhi = dhi / 1000;
	tol = tol / 1000;
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    |(%7.3g <= %7.3g <= %7.3g)|  | %6s             <%d>\n", dlo - tol, itv, dhi + tol, t, state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (dlo - tol <= itv && itv <= dhi + tol)
		return __RESULT_SUCCESS;
	else
		return __RESULT_FAILURE;
}

static int time_event(int event)
{
	if (verbose > 4) {
		float t, m;
		struct timeval now;
		gettimeofday(&now, NULL);
		if (!test_start)
			test_start = now.tv_sec;
		t = (now.tv_sec - test_start);
		m = now.tv_usec;
		m = m / 1000000;
		t += m;
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    | %11.6g                   |  |                    <%d>\n", t, state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	return (event);
}

static int timer_timeout = 0;

static void timer_handler(int signum)
{
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

static int timer_sethandler(void)
{
	sigset_t mask;
	struct sigaction act;
	act.sa_handler = timer_handler;
	act.sa_flags = SA_RESTART | SA_ONESHOT;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return __RESULT_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	return __RESULT_SUCCESS;
}

/* 
 *  Start an interval timer as the overall test timer.
 */
static int start_tt(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};
	if (timer_sethandler())
		return __RESULT_FAILURE;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return __RESULT_FAILURE;
	timer_timeout = 0;
	return __RESULT_SUCCESS;
}
static int start_st(long duration)
{
	long sdur = (duration + timer_scale - 1) / timer_scale;
	return start_tt(sdur);
}

static int stop_tt(void)
{
	struct itimerval setting = { {0, 0}, {0, 0} };
	sigset_t mask;
	struct sigaction act;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return __RESULT_FAILURE;
	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return __RESULT_FAILURE;
	timer_timeout = 0;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	return __RESULT_SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Printing things
 *
 *  -------------------------------------------------------------------------
 */

char *errno_string(long err)
{
	switch (err) {
	case 0:
		return ("ok");
	case EPERM:
		return ("[EPERM]");
	case ENOENT:
		return ("[ENOENT]");
	case ESRCH:
		return ("[ESRCH]");
	case EINTR:
		return ("[EINTR]");
	case EIO:
		return ("[EIO]");
	case ENXIO:
		return ("[ENXIO]");
	case E2BIG:
		return ("[E2BIG]");
	case ENOEXEC:
		return ("[ENOEXEC]");
	case EBADF:
		return ("[EBADF]");
	case ECHILD:
		return ("[ECHILD]");
	case EAGAIN:
		return ("[EAGAIN]");
	case ENOMEM:
		return ("[ENOMEM]");
	case EACCES:
		return ("[EACCES]");
	case EFAULT:
		return ("[EFAULT]");
	case ENOTBLK:
		return ("[ENOTBLK]");
	case EBUSY:
		return ("[EBUSY]");
	case EEXIST:
		return ("[EEXIST]");
	case EXDEV:
		return ("[EXDEV]");
	case ENODEV:
		return ("[ENODEV]");
	case ENOTDIR:
		return ("[ENOTDIR]");
	case EISDIR:
		return ("[EISDIR]");
	case EINVAL:
		return ("[EINVAL]");
	case ENFILE:
		return ("[ENFILE]");
	case EMFILE:
		return ("[EMFILE]");
	case ENOTTY:
		return ("[ENOTTY]");
	case ETXTBSY:
		return ("[ETXTBSY]");
	case EFBIG:
		return ("[EFBIG]");
	case ENOSPC:
		return ("[ENOSPC]");
	case ESPIPE:
		return ("[ESPIPE]");
	case EROFS:
		return ("[EROFS]");
	case EMLINK:
		return ("[EMLINK]");
	case EPIPE:
		return ("[EPIPE]");
	case EDOM:
		return ("[EDOM]");
	case ERANGE:
		return ("[ERANGE]");
	case EDEADLK:
		return ("[EDEADLK]");
	case ENAMETOOLONG:
		return ("[ENAMETOOLONG]");
	case ENOLCK:
		return ("[ENOLCK]");
	case ENOSYS:
		return ("[ENOSYS]");
	case ENOTEMPTY:
		return ("[ENOTEMPTY]");
	case ELOOP:
		return ("[ELOOP]");
	case ENOMSG:
		return ("[ENOMSG]");
	case EIDRM:
		return ("[EIDRM]");
	case ECHRNG:
		return ("[ECHRNG]");
	case EL2NSYNC:
		return ("[EL2NSYNC]");
	case EL3HLT:
		return ("[EL3HLT]");
	case EL3RST:
		return ("[EL3RST]");
	case ELNRNG:
		return ("[ELNRNG]");
	case EUNATCH:
		return ("[EUNATCH]");
	case ENOCSI:
		return ("[ENOCSI]");
	case EL2HLT:
		return ("[EL2HLT]");
	case EBADE:
		return ("[EBADE]");
	case EBADR:
		return ("[EBADR]");
	case EXFULL:
		return ("[EXFULL]");
	case ENOANO:
		return ("[ENOANO]");
	case EBADRQC:
		return ("[EBADRQC]");
	case EBADSLT:
		return ("[EBADSLT]");
	case EBFONT:
		return ("[EBFONT]");
	case ENOSTR:
		return ("[ENOSTR]");
	case ENODATA:
		return ("[ENODATA]");
	case ETIME:
		return ("[ETIME]");
	case ENOSR:
		return ("[ENOSR]");
	case ENONET:
		return ("[ENONET]");
	case ENOPKG:
		return ("[ENOPKG]");
	case EREMOTE:
		return ("[EREMOTE]");
	case ENOLINK:
		return ("[ENOLINK]");
	case EADV:
		return ("[EADV]");
	case ESRMNT:
		return ("[ESRMNT]");
	case ECOMM:
		return ("[ECOMM]");
	case EPROTO:
		return ("[EPROTO]");
	case EMULTIHOP:
		return ("[EMULTIHOP]");
	case EDOTDOT:
		return ("[EDOTDOT]");
	case EBADMSG:
		return ("[EBADMSG]");
	case EOVERFLOW:
		return ("[EOVERFLOW]");
	case ENOTUNIQ:
		return ("[ENOTUNIQ]");
	case EBADFD:
		return ("[EBADFD]");
	case EREMCHG:
		return ("[EREMCHG]");
	case ELIBACC:
		return ("[ELIBACC]");
	case ELIBBAD:
		return ("[ELIBBAD]");
	case ELIBSCN:
		return ("[ELIBSCN]");
	case ELIBMAX:
		return ("[ELIBMAX]");
	case ELIBEXEC:
		return ("[ELIBEXEC]");
	case EILSEQ:
		return ("[EILSEQ]");
	case ERESTART:
		return ("[ERESTART]");
	case ESTRPIPE:
		return ("[ESTRPIPE]");
	case EUSERS:
		return ("[EUSERS]");
	case ENOTSOCK:
		return ("[ENOTSOCK]");
	case EDESTADDRREQ:
		return ("[EDESTADDRREQ]");
	case EMSGSIZE:
		return ("[EMSGSIZE]");
	case EPROTOTYPE:
		return ("[EPROTOTYPE]");
	case ENOPROTOOPT:
		return ("[ENOPROTOOPT]");
	case EPROTONOSUPPORT:
		return ("[EPROTONOSUPPORT]");
	case ESOCKTNOSUPPORT:
		return ("[ESOCKTNOSUPPORT]");
	case EOPNOTSUPP:
		return ("[EOPNOTSUPP]");
	case EPFNOSUPPORT:
		return ("[EPFNOSUPPORT]");
	case EAFNOSUPPORT:
		return ("[EAFNOSUPPORT]");
	case EADDRINUSE:
		return ("[EADDRINUSE]");
	case EADDRNOTAVAIL:
		return ("[EADDRNOTAVAIL]");
	case ENETDOWN:
		return ("[ENETDOWN]");
	case ENETUNREACH:
		return ("[ENETUNREACH]");
	case ENETRESET:
		return ("[ENETRESET]");
	case ECONNABORTED:
		return ("[ECONNABORTED]");
	case ECONNRESET:
		return ("[ECONNRESET]");
	case ENOBUFS:
		return ("[ENOBUFS]");
	case EISCONN:
		return ("[EISCONN]");
	case ENOTCONN:
		return ("[ENOTCONN]");
	case ESHUTDOWN:
		return ("[ESHUTDOWN]");
	case ETOOMANYREFS:
		return ("[ETOOMANYREFS]");
	case ETIMEDOUT:
		return ("[ETIMEDOUT]");
	case ECONNREFUSED:
		return ("[ECONNREFUSED]");
	case EHOSTDOWN:
		return ("[EHOSTDOWN]");
	case EHOSTUNREACH:
		return ("[EHOSTUNREACH]");
	case EALREADY:
		return ("[EALREADY]");
	case EINPROGRESS:
		return ("[EINPROGRESS]");
	case ESTALE:
		return ("[ESTALE]");
	case EUCLEAN:
		return ("[EUCLEAN]");
	case ENOTNAM:
		return ("[ENOTNAM]");
	case ENAVAIL:
		return ("[ENAVAIL]");
	case EISNAM:
		return ("[EISNAM]");
	case EREMOTEIO:
		return ("[EREMOTEIO]");
	case EDQUOT:
		return ("[EDQUOT]");
	case ENOMEDIUM:
		return ("[ENOMEDIUM]");
	case EMEDIUMTYPE:
		return ("[EMEDIUMTYPE]");
	default:
	{
		static char buf[32];
		snprintf(buf, sizeof(buf), "[%ld]", err);
		return buf;
	}
	}
}

const char *event_string(int event)
{
	switch (event) {
	case __EVENT_NO_MSG:
		return ("NO MESSAGE");
	case __EVENT_TIMEOUT:
		return ("TIMEOUT");
	case __EVENT_UNKNOWN:
		return ("UNKNOWN");
	case __RESULT_DECODE_ERROR:
		return ("DECODE ERROR");
	case __RESULT_SCRIPT_ERROR:
		return ("SCRIPT ERROR");
	case __RESULT_INCONCLUSIVE:
		return ("INCONCLUSIVE");
	case __RESULT_SUCCESS:
		return ("SUCCESS");
	case __RESULT_FAILURE:
		return ("FAILURE");
	default:
		return ("(unexpected");
	}
}

const char *ioctl_string(int cmd)
{
	switch (cmd) {
	case I_NREAD:
		return ("I_NREAD-------");
	case I_PUSH:
		return ("I_PUSH--------");
	case I_POP:
		return ("I_POP---------");
	case I_LOOK:
		return ("I_LOOK--------");
	case I_FLUSH:
		return ("I_FLUSH-------");
	case I_SRDOPT:
		return ("I_SRDOPT------");
	case I_GRDOPT:
		return ("I_GRDOPT------");
	case I_STR:
		return ("I_STR---------");
	case I_SETSIG:
		return ("I_SETSIG------");
	case I_GETSIG:
		return ("I_GETSIG------");
	case I_FIND:
		return ("I_FIND--------");
	case I_LINK:
		return ("I_LINK--------");
	case I_UNLINK:
		return ("I_UNLINK------");
	case I_RECVFD:
		return ("I_RECVFD------");
	case I_PEEK:
		return ("I_PEEK--------");
	case I_FDINSERT:
		return ("I_FDINSERT----");
	case I_SENDFD:
		return ("I_SENDFD------");
	case I_E_RECVFD:
		return ("I_E_RECVFD----");
	case I_SWROPT:
		return ("I_SWROPT------");
	case I_GWROPT:
		return ("I_GWROPT------");
	case I_LIST:
		return ("I_LIST--------");
	case I_PLINK:
		return ("I_PLINK-------");
	case I_PUNLINK:
		return ("I_PUNLINK-----");
	case I_FLUSHBAND:
		return ("I_FLUSHBAND---");
	case I_CKBAND:
		return ("I_CKBAND------");
	case I_GETBAND:
		return ("I_GETBAND-----");
	case I_ATMARK:
		return ("I_ATMARK------");
	case I_SETCLTIME:
		return ("I_SETCLTIME---");
	case I_GETCLTIME:
		return ("I_GETCLTIME---");
	case I_CANPUT:
		return ("I_CANPUT------");
	case I_SERROPT:
		return ("I_SERROPT-----");
	case I_GERROPT:
		return ("I_GERROPT-----");
	case I_ANCHOR:
		return ("I_ANCHOR------");
#if 0
	case I_S_RECVFD:
		return ("I_S_RECVFD----");
	case I_STATS:
		return ("I_STATS-------");
	case I_BIGPIPE:
		return ("I_BIGPIPE-----");
#endif
	case I_GETTP:
		return ("I_GETTP-------");
	case I_AUTOPUSH:
		return ("I_AUTOPUSH----");
	case I_HEAP_REPORT:
		return ("I_HEAP_REPORT-");
	case I_FIFO:
		return ("I_FIFO--------");
	case I_PUTPMSG:
		return ("I_PUTPMSG-----");
	case I_GETPMSG:
		return ("I_GETPMSG-----");
	case I_FATTACH:
		return ("I_FATTACH-----");
	case I_FDETACH:
		return ("I_FDETACH-----");
	case I_PIPE:
		return ("I_PIPE--------");
	default:
		return ("(unexpected)--");
	}
}

void print_less(int child)
{
	if (verbose < 1 || !show)
		return;
	lockf(fileno(stdout), F_LOCK, 0);
	switch (child) {
	case 0:
		fprintf(stdout, " .         .  <---->|               .               :  :                    \n");
		fprintf(stdout, " .  (more) .  <---->|               .               :  :                    [%d]\n", state);
		fprintf(stdout, " .         .  <---->|               .               :  :                    \n");
		break;
	case 1:
		fprintf(stdout, "                    :               .               :  |<-->  .         .   \n");
		fprintf(stdout, "                    :               .               :  |<-->  . (more)  .   [%d]\n", state);
		fprintf(stdout, "                    :               .               :  |<-->  .         .   \n");
		break;
	case 2:
		fprintf(stdout, "                    :               .               |<-:--->  .         .   \n");
		fprintf(stdout, "                    :               .               |<-:--->  . (more)  .   [%d]\n", state);
		fprintf(stdout, "                    :               .               |<-:--->  .         .   \n");
		break;
	}
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	show = 0;
	return;
}

void print_more(void)
{
	show = 1;
}

void print_simple(int child, const char *msgs[])
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child]);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_simple_int(int child, const char *msgs[], int val)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_double_int(int child, const char *msgs[], int val, int val2)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val, val2);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_open(int child)
{
	static const char *msgs[] = {
		"open()        ----->v                               |  |                    \n",
		"  open()      ----->v                               |  |                    \n",
		"    open()    ----->v                               |  |                    \n",
	};
	if (verbose > 3)
		print_simple(child, msgs);
}

void print_close(int child)
{
	static const char *msgs[] = {
		"close()       ----->X                               |  |                    \n",
		"  close()     ----->X                               |  |                    \n",
		"    close()   ----->X                               |  |                    \n",
	};
	if (verbose > 3)
		print_simple(child, msgs);
}

void print_preamble(int child)
{
	static const char *msgs[] = {
		"--------------------+------------Preamble-----------+--+                    \n",
		"  ------------------+------------Preamble-----------+--+                    \n",
		"    ----------------+------------Preamble-----------+--+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_inconclusive(int child)
{
	static const char *msgs[] = {
		"???????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n",
		"  ?????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n",
		"    ???????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
};

void print_test(int child)
{
	static const char *msgs[] = {
		"--------------------+--------------Test-------------+--+                    \n",
		"  ------------------+--------------Test-------------+--+                    \n",
		"    ----------------+--------------Test-------------+--+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    [%d]\n",
		"  XXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    [%d]\n",
		"    XXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    [%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
};

void print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########## SCRIPT ERROR #######|##|                    [%d]\n",
		"  ##################|########## SCRIPT ERROR #######|##|                    [%d]\n",
		"    ################|########## SCRIPT ERROR #######|##|                    [%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
};

void print_passed(int child)
{
	static const char *msgs[] = {
		"********************|************ PASSED ***********|**|                    [%d]\n",
		"  ******************|************ PASSED ***********|**|                    [%d]\n",
		"    ****************|************ PASSED ***********|**|                    [%d]\n",
	};
	if (verbose > 2)
		print_simple_int(child, msgs, state);
};

void print_postamble(int child)
{
	static const char *msgs[] = {
		"--------------------+------------Postamble----------+--+                    \n",
		"  ------------------+------------Postamble----------+--+                    \n",
		"    ----------------+------------Postamble----------+--+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_test_end(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------------------------+--+                    \n",
		"  ------------------+-------------------------------+--+                    \n",
		"    ----------------+-------------------------------+--+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d}\n",
		"  @@@@@@@@@@@@@@@@@@|@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d}\n",
		"    @@@@@@@@@@@@@@@@|@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d}\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, signal);
};

void print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d}\n",
		"  &&&&&&&&&&&&&&&&&&|&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d}\n",
		"    &&&&&&&&&&&&&&&&|&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d}\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, signal);
};

void print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++{%d}\n",
		"  ++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++{%d}\n",
		"    ++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++{%d}\n",
	};
	if (show_timeout || verbose > 0) {
		print_simple_int(child, msgs, state);
		show_timeout--;
	}
};

void print_string_state(int child, const char *msgs[], const char *string)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, state);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_command(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s<----/|                               |  |                    [%d]\n",
		"  %-14s<--/|                               |  |                    [%d]\n",
		"    %-14s</|                               |  |                    [%d]\n",
	};
	if (verbose > 3)
		print_string_state(child, msgs, command);
}

void print_errno(int child, long error)
{
	static const char *msgs[] = {
		"%-14s<----/|                               |  |                    [%d]\n",
		"  %-14s<--/|                               |  |                    [%d]\n",
		"    %-14s</|                               |  |                    [%d]\n",
	};
	if (verbose > 3)
		print_string_state(child, msgs, errno_string(error));
}

void print_success(int child)
{
	static const char *msgs[] = {
		"ok            <----/|                               |  |                    [%d]\n",
		"  ok          <----/|                               |  |                    [%d]\n",
		"    ok        <----/|                               |  |                    [%d]\n",
	};
	if (verbose > 4)
		print_simple_int(child, msgs, state);
}

void print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"%10d    <----/|                               |  |                    [%d]\n",
		"  %10d  <----/|                               |  |                    [%d]\n",
		"    %10d<----/|                               |  |                    [%d]\n",
	};
	if (verbose)
		print_double_int(child, msgs, value, state);
}

void print_ioctl(int child, int cmd)
{
	static const char *msgs[] = {
		"%-14s----->|                               |  |                    [%d]\n",
		"  %-14s--->|                               |  |                    [%d]\n",
		"    %-14s->|                               |  |                    [%d]\n",
	};
	if (verbose)
		print_string_state(child, msgs, ioctl_string(cmd));
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Driver actions.
 *
 *  -------------------------------------------------------------------------
 */

int test_ioctl(int child, int cmd, intptr_t arg)
{
	print_ioctl(child, cmd);
	for (;;) {
		if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		return (__RESULT_SUCCESS);
	}
}

int test_open(int child, const char *name)
{
	int fd;
	for (;;) {
		print_open(child);
		if ((fd = open(name, O_NONBLOCK | O_RDWR)) >= 0) {
			test_fd[child] = fd;
			print_success(child);
			return (__RESULT_SUCCESS);
		}
		print_errno(child, (last_errno = errno));
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
			continue;
		return (__RESULT_FAILURE);
	}
}

int test_close(int child)
{
	int fd = test_fd[child];
	test_fd[child] = 0;
	for (;;) {
		print_close(child);
		if (close(fd) >= 0) {
			print_success(child);
			return __RESULT_SUCCESS;
		}
		print_errno(child, (last_errno = errno));
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
			continue;
		return __RESULT_FAILURE;
	}
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Test initialization and termination.
 *
 *  -------------------------------------------------------------------------
 */

static int begin_tests(void)
{
	state = 0;
	return (__RESULT_SUCCESS);
}

static int end_tests(void)
{
	show_acks = 0;
	return (__RESULT_SUCCESS);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */

int preamble_0(int child)
{
	if (!test_fd[child] && test_open(child, devname) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}
int postamble_0(int child)
{
	if (test_fd[child] && test_close(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* 
 *  =========================================================================
 *
 *  The Test Cases...
 *
 *  =========================================================================
 */

struct test_stream {
	int (*preamble) (int);		/* test preamble */
	int (*testcase) (int);		/* test case */
	int (*postamble) (int);		/* test postamble */
};

/*
 *  Open and Close 1 stream.
 */
#define name_case_1_1 "Open and close 1 stream."
#define desc_case_1_1 "\
Checks that one stream can be opened and closed."
int test_1_1(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
struct test_stream test_case_1_1 = { NULL, &test_1_1, NULL };
#define test_case_1_1_stream_0 (&test_case_1_1)
#define test_case_1_1_stream_1 (NULL)
#define test_case_1_1_stream_2 (NULL)

/*
 *  Open and Close 3 streams.
 */
#define name_case_1_2 "Open and close 3 streams."
#define desc_case_1_2 "\
Checks that three streams can be opened and closed."
int test_1_2(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
struct test_stream test_case_1_2 = { NULL, &test_1_2, NULL };
#define test_case_1_2_stream_0 (&test_case_1_2)
#define test_case_1_2_stream_1 (&test_case_1_2)
#define test_case_1_2_stream_2 (&test_case_1_2)

/*
 *  Perform IOCTL on one stream - I_NREAD
 */
#define name_case_2_1_1 "Perform streamio I_NREAD."
#define desc_case_2_1_1 "\
Checks that I_NREAD can be performed on a stream.  Because this test is \n\
peformed on a freshly opened stream, it should return zero (0) as a return \n\
value and return zero (0) in the integer pointed to by arg."
int test_2_1_1(int child)
{
	int numb = -1;
	if (test_ioctl(child, I_NREAD, (intptr_t) & numb) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (numb != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_1_1 = { &preamble_0, &test_2_1_1, &postamble_0 };
#define test_case_2_1_1_stream_0 (&test_case_2_1_1)
#define test_case_2_1_1_stream_1 (NULL)
#define test_case_2_1_1_stream_2 (NULL)

#define name_case_2_1_2 "Perform streamio I_NREAD - EFAULT."
#define desc_case_2_1_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."
int test_2_1_2(int child)
{
	if (test_ioctl(child, I_NREAD, (intptr_t) NULL) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_1_2 = { &preamble_0, &test_2_1_2, &postamble_0 };
#define test_case_2_1_2_stream_0 (&test_case_2_1_2)
#define test_case_2_1_2_stream_1 (NULL)
#define test_case_2_1_2_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PUSH
 */
#define name_case_2_2 "Perform streamio I_PUSH."
#define desc_case_2_2 "\
Checks that I_PUSH can be performed on a stream."
int test_2_2(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nomodule") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_2 = { &preamble_0, &test_2_2, &postamble_0 };
#define test_case_2_2_stream_0 (&test_case_2_2)
#define test_case_2_2_stream_1 (NULL)
#define test_case_2_2_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_POP
 */
#define name_case_2_3 "Perform streamio I_POP."
#define desc_case_2_3 "\
Checks that I_POP can be performed on a stream."
int test_2_3(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_3 = { &preamble_0, &test_2_3, &postamble_0 };
#define test_case_2_3_stream_0 (&test_case_2_3)
#define test_case_2_3_stream_1 (NULL)
#define test_case_2_3_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_LOOK
 */
#define name_case_2_4 "Perform streamio I_LOOK."
#define desc_case_2_4 "\
Checks that I_LOOK can be performed on a stream."
int test_2_4(int child)
{
	char buf[FMNAMESZ + 1];
	if (test_ioctl(child, I_LOOK, (intptr_t) buf) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_4 = { &preamble_0, &test_2_4, &postamble_0 };
#define test_case_2_4_stream_0 (&test_case_2_4)
#define test_case_2_4_stream_1 (NULL)
#define test_case_2_4_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FLUSH
 */
#define name_case_2_5 "Perform streamio I_FLUSH."
#define desc_case_2_5 "\
Checks that I_FLUSH can be performed on a stream."
int test_2_5(int child)
{
#if 1
	return __RESULT_INCONCLUSIVE;
#else
	if (test_ioctl(child, I_FLUSH, 0) != __RESULT_SUCCESS && last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_case_2_5 = { &preamble_0, &test_2_5, &postamble_0 };
#define test_case_2_5_stream_0 (&test_case_2_5)
#define test_case_2_5_stream_1 (NULL)
#define test_case_2_5_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SRDOPT
 */
#define name_case_2_6 "Perform streamio I_SRDOPT."
#define desc_case_2_6 "\
Checks that I_SRDOPT can be performed on a stream."
int test_2_6(int child)
{
	if (test_ioctl(child, I_SRDOPT, 0) != __RESULT_SUCCESS && last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_6 = { &preamble_0, &test_2_6, &postamble_0 };
#define test_case_2_6_stream_0 (&test_case_2_6)
#define test_case_2_6_stream_1 (NULL)
#define test_case_2_6_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GRDOPT
 */
#define name_case_2_7 "Perform streamio I_GRDOPT."
#define desc_case_2_7 "\
Checks that I_GRDOPT can be performed on a stream."
int test_2_7(int child)
{
	int opts = 0;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & opts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_7 = { &preamble_0, &test_2_7, &postamble_0 };
#define test_case_2_7_stream_0 (&test_case_2_7)
#define test_case_2_7_stream_1 (NULL)
#define test_case_2_7_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_STR
 */
#define name_case_2_8 "Perform streamio I_STR."
#define desc_case_2_8 "\
Checks that I_STR can be performed on a stream."
int test_2_8(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_8 = { &preamble_0, &test_2_8, &postamble_0 };
#define test_case_2_8_stream_0 (&test_case_2_8)
#define test_case_2_8_stream_1 (NULL)
#define test_case_2_8_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SETSIG
 */
#define name_case_2_9 "Perform streamio I_SETSIG."
#define desc_case_2_9 "\
Checks that I_SETSIG can be performed on a stream."
int test_2_9(int child)
{
	if (test_ioctl(child, I_SETSIG, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_9 = { &preamble_0, &test_2_9, &postamble_0 };
#define test_case_2_9_stream_0 (&test_case_2_9)
#define test_case_2_9_stream_1 (NULL)
#define test_case_2_9_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETSIG
 */
#define name_case_2_10 "Perform streamio I_GETSIG."
#define desc_case_2_10 "\
Checks that I_GETSIG can be performed on a stream."
int test_2_10(int child)
{
	int sigs = 0;
	if (test_ioctl(child, I_GETSIG, (intptr_t) & sigs) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_10 = { &preamble_0, &test_2_10, &postamble_0 };
#define test_case_2_10_stream_0 (&test_case_2_10)
#define test_case_2_10_stream_1 (NULL)
#define test_case_2_10_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FIND
 */
#define name_case_2_11 "Perform streamio I_FIND."
#define desc_case_2_11 "\
Checks that I_FIND can be performed on a stream."
int test_2_11(int child)
{
	char bogus[] = "bogus";
	if (test_ioctl(child, I_FIND, (intptr_t) bogus) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_11 = { &preamble_0, &test_2_11, &postamble_0 };
#define test_case_2_11_stream_0 (&test_case_2_11)
#define test_case_2_11_stream_1 (NULL)
#define test_case_2_11_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_LINK
 */
#define name_case_2_12 "Perform streamio I_LINK."
#define desc_case_2_12 "\
Checks that I_LINK can be performed on a stream."
int test_2_12(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_12 = { &preamble_0, &test_2_12, &postamble_0 };
#define test_case_2_12_stream_0 (&test_case_2_12)
#define test_case_2_12_stream_1 (NULL)
#define test_case_2_12_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_UNLINK
 */
#define name_case_2_13 "Perform streamio I_UNLINK."
#define desc_case_2_13 "\
Checks that I_UNLINK can be performed on a stream."
int test_2_13(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_13 = { &preamble_0, &test_2_13, &postamble_0 };
#define test_case_2_13_stream_0 (&test_case_2_13)
#define test_case_2_13_stream_1 (NULL)
#define test_case_2_13_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_RECVFD
 */
#define name_case_2_14 "Perform streamio I_RECVFD."
#define desc_case_2_14 "\
Checks that I_RECVFD can be performed on a stream."
int test_2_14(int child)
{
	struct strrecvfd recvfd;
	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_14 = { &preamble_0, &test_2_14, &postamble_0 };
#define test_case_2_14_stream_0 (&test_case_2_14)
#define test_case_2_14_stream_1 (NULL)
#define test_case_2_14_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PEEK
 */
#define name_case_2_15 "Perform streamio I_PEEK."
#define desc_case_2_15 "\
Checks that I_PEEK can be performed on a stream."
int test_2_15(int child)
{
	struct strpeek peek;
	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_15 = { &preamble_0, &test_2_15, &postamble_0 };
#define test_case_2_15_stream_0 (&test_case_2_15)
#define test_case_2_15_stream_1 (NULL)
#define test_case_2_15_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FDINSERT
 */
#define name_case_2_16 "Perform streamio I_FDINSERT."
#define desc_case_2_16 "\
Checks that I_FDINSERT can be performed on a stream."
int test_2_16(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_16 = { &preamble_0, &test_2_16, &postamble_0 };
#define test_case_2_16_stream_0 (&test_case_2_16)
#define test_case_2_16_stream_1 (NULL)
#define test_case_2_16_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SENDFD
 */
#define name_case_2_17 "Perform streamio I_SENDFD."
#define desc_case_2_17 "\
Checks that I_SENDFD can be performed on a stream."
int test_2_17(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_17 = { &preamble_0, &test_2_17, &postamble_0 };
#define test_case_2_17_stream_0 (&test_case_2_17)
#define test_case_2_17_stream_1 (NULL)
#define test_case_2_17_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_E_RECVFD
 */
#define name_case_2_18 "Perform streamio I_E_RECVFD."
#define desc_case_2_18 "\
Checks that I_E_RECVFD can be performed on a stream."
int test_2_18(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_18 = { &preamble_0, &test_2_18, &postamble_0 };
#define test_case_2_18_stream_0 (&test_case_2_18)
#define test_case_2_18_stream_1 (NULL)
#define test_case_2_18_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SWROPT
 */
#define name_case_2_19 "Perform streamio I_SWROPT."
#define desc_case_2_19 "\
Checks that I_SWROPT can be performed on a stream."
int test_2_19(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDPIPE | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_19 = { &preamble_0, &test_2_19, &postamble_0 };
#define test_case_2_19_stream_0 (&test_case_2_19)
#define test_case_2_19_stream_1 (NULL)
#define test_case_2_19_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GWROPT
 */
#define name_case_2_20 "Perform streamio I_GWROPT."
#define desc_case_2_20 "\
Checks that I_GWROPT can be performed on a stream."
int test_2_20(int child)
{
	int opts = 0;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & opts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_20 = { &preamble_0, &test_2_20, &postamble_0 };
#define test_case_2_20_stream_0 (&test_case_2_20)
#define test_case_2_20_stream_1 (NULL)
#define test_case_2_20_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_LIST
 */
#define name_case_2_21 "Perform streamio I_LIST."
#define desc_case_2_21 "\
Checks that I_LIST can be performed on a stream."
int test_2_21(int child)
{
	if (test_ioctl(child, I_LIST, 0) != __RESULT_SUCCESS || (last_retval != 0 && last_retval != 1))
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_21 = { &preamble_0, &test_2_21, &postamble_0 };
#define test_case_2_21_stream_0 (&test_case_2_21)
#define test_case_2_21_stream_1 (NULL)
#define test_case_2_21_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PUNLINK
 */
#define name_case_2_22 "Perform streamio I_PUNLINK."
#define desc_case_2_22 "\
Checks that I_PUNLINK can be performed on a stream."
int test_2_22(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_22 = { &preamble_0, &test_2_22, &postamble_0 };
#define test_case_2_22_stream_0 (&test_case_2_22)
#define test_case_2_22_stream_1 (NULL)
#define test_case_2_22_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FLUSHBAND
 */
#define name_case_2_23 "Perform streamio I_FLUSHBAND."
#define desc_case_2_23 "\
Checks that I_FLUSHBAND can be performed on a stream."
int test_2_23(int child)
{
#if 1
	return __RESULT_INCONCLUSIVE;
#else
	struct bandinfo bi = { 0, 0 };
	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_case_2_23 = { &preamble_0, &test_2_23, &postamble_0 };
#define test_case_2_23_stream_0 (&test_case_2_23)
#define test_case_2_23_stream_1 (NULL)
#define test_case_2_23_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_CKBAND
 */
#define name_case_2_24 "Perform streamio I_CKBAND."
#define desc_case_2_24 "\
Checks that I_CKBAND can be performed on a stream."
int test_2_24(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_24 = { &preamble_0, &test_2_24, &postamble_0 };
#define test_case_2_24_stream_0 (&test_case_2_24)
#define test_case_2_24_stream_1 (NULL)
#define test_case_2_24_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETBAND
 */
#define name_case_2_25 "Perform streamio I_GETBAND."
#define desc_case_2_25 "\
Checks that I_GETBAND can be performed on a stream."
int test_2_25(int child)
{
	int band = 0;
	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != ENODATA)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_25 = { &preamble_0, &test_2_25, &postamble_0 };
#define test_case_2_25_stream_0 (&test_case_2_25)
#define test_case_2_25_stream_1 (NULL)
#define test_case_2_25_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_ATMARK
 */
#define name_case_2_26 "Perform streamio I_ATMARK."
#define desc_case_2_26 "\
Checks that I_ATMARK can be performed on a stream."
int test_2_26(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_26 = { &preamble_0, &test_2_26, &postamble_0 };
#define test_case_2_26_stream_0 (&test_case_2_26)
#define test_case_2_26_stream_1 (NULL)
#define test_case_2_26_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SETCLTIME
 */
#define name_case_2_27 "Perform streamio I_SETCLTIME."
#define desc_case_2_27 "\
Checks that I_SETCLTIME can be performed on a stream."
int test_2_27(int child)
{
	if (test_ioctl(child, I_SETCLTIME, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_27 = { &preamble_0, &test_2_27, &postamble_0 };
#define test_case_2_27_stream_0 (&test_case_2_27)
#define test_case_2_27_stream_1 (NULL)
#define test_case_2_27_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETCLTIME
 */
#define name_case_2_28 "Perform streamio I_GETCLTIME."
#define desc_case_2_28 "\
Checks that I_GETCLTIME can be performed on a stream."
int test_2_28(int child)
{
	long cltime;
	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_28 = { &preamble_0, &test_2_28, &postamble_0 };
#define test_case_2_28_stream_0 (&test_case_2_28)
#define test_case_2_28_stream_1 (NULL)
#define test_case_2_28_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_CANPUT
 */
#define name_case_2_29 "Perform streamio I_CANPUT."
#define desc_case_2_29 "\
Checks that I_CANPUT can be performed on a stream."
int test_2_29(int child)
{
	if (test_ioctl(child, I_CANPUT, 2) != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_29 = { &preamble_0, &test_2_29, &postamble_0 };
#define test_case_2_29_stream_0 (&test_case_2_29)
#define test_case_2_29_stream_1 (NULL)
#define test_case_2_29_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SERROPT	(Solaris)
 */
#define name_case_2_30 "Perform streamio I_SERROPT."
#define desc_case_2_30 "\
Checks that I_SERROPT can be performed on a stream."
int test_2_30(int child)
{
	if (test_ioctl(child, I_SERROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_30 = { &preamble_0, &test_2_30, &postamble_0 };
#define test_case_2_30_stream_0 (&test_case_2_30)
#define test_case_2_30_stream_1 (NULL)
#define test_case_2_30_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GERROPT	(Solaris)
 */
#define name_case_2_31 "Perform streamio I_GERROPT."
#define desc_case_2_31 "\
Checks that I_GERROPT can be performed on a stream."
int test_2_31(int child)
{
	int opts = 0;
	if (test_ioctl(child, I_GERROPT, (intptr_t) & opts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_31 = { &preamble_0, &test_2_31, &postamble_0 };
#define test_case_2_31_stream_0 (&test_case_2_31)
#define test_case_2_31_stream_1 (NULL)
#define test_case_2_31_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_ANCHOR	(Solaris)
 */
#define name_case_2_32 "Perform streamio I_ANCHOR."
#define desc_case_2_32 "\
Checks that I_ANCHOR can be performed on a stream."
int test_2_32(int child)
{
	if (test_ioctl(child, I_ANCHOR, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_32 = { &preamble_0, &test_2_32, &postamble_0 };
#define test_case_2_32_stream_0 (&test_case_2_32)
#define test_case_2_32_stream_1 (NULL)
#define test_case_2_32_stream_2 (NULL)

#if 0
/*
 *  Perform IOCTL on one stream - I_S_RECVFD	(UnixWare)
 */
#define name_case_2_33 "Perform streamio I_S_RECVFD."
#define desc_case_2_33 "\
Checks that I_S_RECVFD can be performed on a stream."
int test_2_33(int child)
{
	struct strrecvfd recvfd;
	if (test_ioctl(child, I_S_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_33 = { &preamble_0, &test_2_33, &postamble_0 };
#define test_case_2_33_stream_0 (&test_case_2_33)
#define test_case_2_33_stream_1 (NULL)
#define test_case_2_33_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_STATS	(UnixWare)
 */
#define name_case_2_34 "Perform streamio I_STATS."
#define desc_case_2_34 "\
Checks that I_STATS can be performed on a stream."
int test_2_34(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_34 = { &preamble_0, &test_2_34, &postamble_0 };
#define test_case_2_34_stream_0 (&test_case_2_34)
#define test_case_2_34_stream_1 (NULL)
#define test_case_2_34_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_BIGPIPE	(UnixWare)
 */
#define name_case_2_35 "Perform streamio I_BIGPIPE."
#define desc_case_2_35 "\
Checks that I_BIGPIPE can be performed on a stream."
int test_2_35(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_35 = { &preamble_0, &test_2_35, &postamble_0 };
#define test_case_2_35_stream_0 (&test_case_2_35)
#define test_case_2_35_stream_1 (NULL)
#define test_case_2_35_stream_2 (NULL)
#endif

/*
 *  Perform IOCTL on one stream - I_GETTP	(UnixWare)
 */
#define name_case_2_36 "Perform streamio I_GETTP."
#define desc_case_2_36 "\
Checks that I_GETTP can be performed on a stream."
int test_2_36(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_36 = { &preamble_0, &test_2_36, &postamble_0 };
#define test_case_2_36_stream_0 (&test_case_2_36)
#define test_case_2_36_stream_1 (NULL)
#define test_case_2_36_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_AUTOPUSH	(MacOT)
 */
#define name_case_2_37 "Perform streamio I_AUTOPUSH."
#define desc_case_2_37 "\
Checks that I_AUTOPUSH can be performed on a stream."
int test_2_37(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_37 = { &preamble_0, &test_2_37, &postamble_0 };
#define test_case_2_37_stream_0 (&test_case_2_37)
#define test_case_2_37_stream_1 (NULL)
#define test_case_2_37_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_HEAP_REPORT	(MacOT)
 */
#define name_case_2_38 "Perform streamio I_HEAP_REPORT."
#define desc_case_2_38 "\
Checks that I_HEAP_REPORT can be performed on a stream."
int test_2_38(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_38 = { &preamble_0, &test_2_38, &postamble_0 };
#define test_case_2_38_stream_0 (&test_case_2_38)
#define test_case_2_38_stream_1 (NULL)
#define test_case_2_38_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FIFO	(MacOT)
 */
#define name_case_2_39 "Perform streamio I_FIFO."
#define desc_case_2_39 "\
Checks that I_FIFO can be performed on a stream."
int test_2_39(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_39 = { &preamble_0, &test_2_39, &postamble_0 };
#define test_case_2_39_stream_0 (&test_case_2_39)
#define test_case_2_39_stream_1 (NULL)
#define test_case_2_39_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PUTPMSG	(LiS)
 */
#define name_case_2_40 "Perform streamio I_PUTPMSG."
#define desc_case_2_40 "\
Checks that I_PUTPMSG can be performed on a stream."
int test_2_40(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_40 = { &preamble_0, &test_2_40, &postamble_0 };
#define test_case_2_40_stream_0 (&test_case_2_40)
#define test_case_2_40_stream_1 (NULL)
#define test_case_2_40_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETPMSG	(LiS)
 */
#define name_case_2_41 "Perform streamio I_GETPMSG."
#define desc_case_2_41 "\
Checks that I_GETPMSG can be performed on a stream."
int test_2_41(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_41 = { &preamble_0, &test_2_41, &postamble_0 };
#define test_case_2_41_stream_0 (&test_case_2_41)
#define test_case_2_41_stream_1 (NULL)
#define test_case_2_41_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FATTACH	(LiS)
 */
#define name_case_2_42 "Perform streamio I_FATTACH."
#define desc_case_2_42 "\
Checks that I_FATTACH can be performed on a stream."
int test_2_42(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_42 = { &preamble_0, &test_2_42, &postamble_0 };
#define test_case_2_42_stream_0 (&test_case_2_42)
#define test_case_2_42_stream_1 (NULL)
#define test_case_2_42_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FDETACH	(LiS)
 */
#define name_case_2_43 "Perform streamio I_FDETACH."
#define desc_case_2_43 "\
Checks that I_FDETACH can be performed on a stream."
int test_2_43(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_43 = { &preamble_0, &test_2_43, &postamble_0 };
#define test_case_2_43_stream_0 (&test_case_2_43)
#define test_case_2_43_stream_1 (NULL)
#define test_case_2_43_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PIPE	(LiS)
 */
#define name_case_2_44 "Perform streamio I_PIPE."
#define desc_case_2_44 "\
Checks that I_PIPE can be performed on a stream."
int test_2_44(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_case_2_44 = { &preamble_0, &test_2_44, &postamble_0 };
#define test_case_2_44_stream_0 (&test_case_2_44)
#define test_case_2_44_stream_1 (NULL)
#define test_case_2_44_stream_2 (NULL)

/* 
 *  -------------------------------------------------------------------------
 *
 *  Test case child scheduler
 *
 *  -------------------------------------------------------------------------
 */
int run_stream(int child, struct test_stream *stream)
{
	int result = __RESULT_SCRIPT_ERROR;
	print_preamble(child);
	state = 100;
	if (stream->preamble && stream->preamble(child) != __RESULT_SUCCESS) {
		print_inconclusive(child);
		result = __RESULT_INCONCLUSIVE;
	} else {
		print_test(child);
		state = 200;
		switch (stream->testcase(child)) {
		default:
		case __RESULT_INCONCLUSIVE:
			print_inconclusive(child);
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_FAILURE:
			print_failed(child);
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			print_script_error(child);
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			print_passed(child);
			result = __RESULT_SUCCESS;
			break;
		}
		print_postamble(child);
		state = 300;
		if (stream->postamble && stream->postamble(child) != __RESULT_SUCCESS) {
			print_inconclusive(child);
			if (result == __RESULT_SUCCESS)
				result = __RESULT_INCONCLUSIVE;
		}
	}
	print_test_end(child);
	exit(result);
}

/* 
 *  Fork multiple children to do the actual testing.
 */

int test_run(struct test_stream *stream[])
{
	int children = 0;
	pid_t this_child, child[3] = { 0, };
	int this_status, status[3] = { 0, };
	start_tt(5000);
	if (stream[0]) {
		switch ((child[0] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(0, stream[0]));	/* execute stream[0] state machine */
		case -1:	/* error */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		status[0] = __RESULT_SUCCESS;
	if (stream[1]) {
		switch ((child[1] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(1, stream[1]));	/* execute stream[1] state machine */
		case -1:	/* error */
			if (child[0])
				kill(child[0], SIGKILL);	/* toast stream[0] child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		status[1] = __RESULT_SUCCESS;
	if (stream[2]) {
		switch ((child[2] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(2, stream[2]));	/* execute stream[1] state machine */
		case -1:	/* error */
			if (child[0])
				kill(child[0], SIGKILL);	/* toast stream[0] child */
			if (child[1])
				kill(child[1], SIGKILL);	/* toast stream[1] child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		status[2] = __RESULT_SUCCESS;
	for (; children > 0; children--) {
		if ((this_child = wait(&this_status)) > 0) {
			if (WIFEXITED(this_status)) {
				if (this_child == child[0]) {
					status[0] = WEXITSTATUS(this_status);
					child[0] = 0;
				}
				if (this_child == child[1]) {
					status[1] = WEXITSTATUS(this_status);
					child[1] = 0;
				}
				if (this_child == child[2]) {
					status[2] = WEXITSTATUS(this_status);
					child[2] = 0;
				}
			} else if (WIFSIGNALED(this_status)) {
				int signal = WTERMSIG(this_status);
				if (this_child == child[0]) {
					print_terminated(0, signal);
					if (child[1])
						kill(child[1], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[0] = __RESULT_FAILURE;
					child[0] = 0;
				}
				if (this_child == child[1]) {
					print_terminated(1, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[1] = __RESULT_FAILURE;
					child[1] = 0;
				}
				if (this_child == child[2]) {
					print_terminated(2, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[1])
						kill(child[1], SIGKILL);
					status[2] = __RESULT_FAILURE;
					child[2] = 0;
				}
			} else if (WIFSTOPPED(this_status)) {
				int signal = WSTOPSIG(this_status);
				if (this_child == child[0]) {
					print_stopped(0, signal);
					if (child[1])
						kill(child[1], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[0] = __RESULT_FAILURE;
					child[0] = 0;
				}
				if (this_child == child[1]) {
					print_stopped(1, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[1] = __RESULT_FAILURE;
					child[1] = 0;
				}
				if (this_child == child[2]) {
					print_stopped(2, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[1])
						kill(child[1], SIGKILL);
					status[2] = __RESULT_FAILURE;
					child[2] = 0;
				}
			}
		} else {
			if (timer_timeout) {
				timer_timeout = 0;
				print_timeout(0);
				last_event = __EVENT_TIMEOUT;
			}
			if (child[0]) {
				kill(child[0], SIGKILL);
				status[0] = __RESULT_FAILURE;
				child[0] = 0;
			}
			if (child[1]) {
				kill(child[1], SIGKILL);
				status[1] = __RESULT_FAILURE;
				child[1] = 0;
			}
			if (child[2]) {
				kill(child[2], SIGKILL);
				status[2] = __RESULT_FAILURE;
				child[2] = 0;
			}
			break;
		}
	}
	stop_tt();
	if (status[0] == __RESULT_FAILURE || status[1] == __RESULT_FAILURE || status[2] == __RESULT_FAILURE)
		return (__RESULT_FAILURE);
	if (status[0] == __RESULT_SUCCESS && status[1] == __RESULT_SUCCESS && status[2] == __RESULT_SUCCESS)
		return (__RESULT_SUCCESS);
	return (__RESULT_INCONCLUSIVE);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Test case lists
 *
 *  -------------------------------------------------------------------------
 */

struct test_case {
	const char *numb;		/* test case number */
	const char *name;		/* test case name */
	const char *desc;		/* test case description */
	struct test_stream *stream[3];	/* test streams */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} tests[] = {
	{
		"1.1", name_case_1_1, desc_case_1_1, {
	test_case_1_1_stream_0, test_case_1_1_stream_1, test_case_1_1_stream_2}, 0, 0}
	, {
		"1.2", name_case_1_2, desc_case_1_2, {
	test_case_1_2_stream_0, test_case_1_2_stream_1, test_case_1_2_stream_2}, 0, 0}
	, {
		"2.1.1", name_case_2_1_1, desc_case_2_1_1, {
	test_case_2_1_1_stream_0, test_case_2_1_1_stream_1, test_case_2_1_1_stream_2}, 0, 0}
	, {
		"2.1.2", name_case_2_1_2, desc_case_2_1_2, {
	test_case_2_1_2_stream_0, test_case_2_1_2_stream_1, test_case_2_1_2_stream_2}, 0, 0}
	, {
		"2.2", name_case_2_2, desc_case_2_2, {
	test_case_2_2_stream_0, test_case_2_2_stream_1, test_case_2_2_stream_2}, 0, 0}
	, {
		"2.3", name_case_2_3, desc_case_2_3, {
	test_case_2_3_stream_0, test_case_2_3_stream_1, test_case_2_3_stream_2}, 0, 0}
	, {
		"2.4", name_case_2_4, desc_case_2_4, {
	test_case_2_4_stream_0, test_case_2_4_stream_1, test_case_2_4_stream_2}, 0, 0}
	, {
		"2.5", name_case_2_5, desc_case_2_5, {
	test_case_2_5_stream_0, test_case_2_5_stream_1, test_case_2_5_stream_2}, 0, 0}
	, {
		"2.6", name_case_2_6, desc_case_2_6, {
	test_case_2_6_stream_0, test_case_2_6_stream_1, test_case_2_6_stream_2}, 0, 0}
	, {
		"2.7", name_case_2_7, desc_case_2_7, {
	test_case_2_7_stream_0, test_case_2_7_stream_1, test_case_2_7_stream_2}, 0, 0}
	, {
		"2.8", name_case_2_8, desc_case_2_8, {
	test_case_2_8_stream_0, test_case_2_8_stream_1, test_case_2_8_stream_2}, 0, 0}
	, {
		"2.9", name_case_2_9, desc_case_2_9, {
	test_case_2_9_stream_0, test_case_2_9_stream_1, test_case_2_9_stream_2}, 0, 0}
	, {
		"2.10", name_case_2_10, desc_case_2_10, {
	test_case_2_10_stream_0, test_case_2_10_stream_1, test_case_2_10_stream_2}, 0, 0}
	, {
		"2.11", name_case_2_11, desc_case_2_11, {
	test_case_2_11_stream_0, test_case_2_11_stream_1, test_case_2_11_stream_2}, 0, 0}
	, {
		"2.12", name_case_2_12, desc_case_2_12, {
	test_case_2_12_stream_0, test_case_2_12_stream_1, test_case_2_12_stream_2}, 0, 0}
	, {
		"2.13", name_case_2_13, desc_case_2_13, {
	test_case_2_13_stream_0, test_case_2_13_stream_1, test_case_2_13_stream_2}, 0, 0}
	, {
		"2.14", name_case_2_14, desc_case_2_14, {
	test_case_2_14_stream_0, test_case_2_14_stream_1, test_case_2_14_stream_2}, 0, 0}
	, {
		"2.15", name_case_2_15, desc_case_2_15, {
	test_case_2_15_stream_0, test_case_2_15_stream_1, test_case_2_15_stream_2}, 0, 0}
	, {
		"2.16", name_case_2_16, desc_case_2_16, {
	test_case_2_16_stream_0, test_case_2_16_stream_1, test_case_2_16_stream_2}, 0, 0}
	, {
		"2.17", name_case_2_17, desc_case_2_17, {
	test_case_2_17_stream_0, test_case_2_17_stream_1, test_case_2_17_stream_2}, 0, 0}
	, {
		"2.18", name_case_2_18, desc_case_2_18, {
	test_case_2_18_stream_0, test_case_2_18_stream_1, test_case_2_18_stream_2}, 0, 0}
	, {
		"2.19", name_case_2_19, desc_case_2_19, {
	test_case_2_19_stream_0, test_case_2_19_stream_1, test_case_2_19_stream_2}, 0, 0}
	, {
		"2.20", name_case_2_20, desc_case_2_20, {
	test_case_2_20_stream_0, test_case_2_20_stream_1, test_case_2_20_stream_2}, 0, 0}
	, {
		"2.21", name_case_2_21, desc_case_2_21, {
	test_case_2_21_stream_0, test_case_2_21_stream_1, test_case_2_21_stream_2}, 0, 0}
	, {
		"2.22", name_case_2_22, desc_case_2_22, {
	test_case_2_22_stream_0, test_case_2_22_stream_1, test_case_2_22_stream_2}, 0, 0}
	, {
		"2.23", name_case_2_23, desc_case_2_23, {
	test_case_2_23_stream_0, test_case_2_23_stream_1, test_case_2_23_stream_2}, 0, 0}
	, {
		"2.24", name_case_2_24, desc_case_2_24, {
	test_case_2_24_stream_0, test_case_2_24_stream_1, test_case_2_24_stream_2}, 0, 0}
	, {
		"2.25", name_case_2_25, desc_case_2_25, {
	test_case_2_25_stream_0, test_case_2_25_stream_1, test_case_2_25_stream_2}, 0, 0}
	, {
		"2.26", name_case_2_26, desc_case_2_26, {
	test_case_2_26_stream_0, test_case_2_26_stream_1, test_case_2_26_stream_2}, 0, 0}
	, {
		"2.27", name_case_2_27, desc_case_2_27, {
	test_case_2_27_stream_0, test_case_2_27_stream_1, test_case_2_27_stream_2}, 0, 0}
	, {
		"2.28", name_case_2_28, desc_case_2_28, {
	test_case_2_28_stream_0, test_case_2_28_stream_1, test_case_2_28_stream_2}, 0, 0}
	, {
		"2.29", name_case_2_29, desc_case_2_29, {
	test_case_2_29_stream_0, test_case_2_29_stream_1, test_case_2_29_stream_2}, 0, 0}
	, {
		"2.30", name_case_2_30, desc_case_2_30, {
	test_case_2_30_stream_0, test_case_2_30_stream_1, test_case_2_30_stream_2}, 0, 0}
	, {
		"2.31", name_case_2_31, desc_case_2_31, {
	test_case_2_31_stream_0, test_case_2_31_stream_1, test_case_2_31_stream_2}, 0, 0}
	, {
		"2.32", name_case_2_32, desc_case_2_32, {
	test_case_2_32_stream_0, test_case_2_32_stream_1, test_case_2_32_stream_2}, 0, 0}
	, {
#if 0
		"2.33", name_case_2_33, desc_case_2_33, {
	test_case_2_33_stream_0, test_case_2_33_stream_1, test_case_2_33_stream_2}, 0, 0}
	, {
		"2.34", name_case_2_34, desc_case_2_34, {
	test_case_2_34_stream_0, test_case_2_34_stream_1, test_case_2_34_stream_2}, 0, 0}
	, {
		"2.35", name_case_2_35, desc_case_2_35, {
	test_case_2_35_stream_0, test_case_2_35_stream_1, test_case_2_35_stream_2}, 0, 0}
	, {
#endif
		"2.36", name_case_2_36, desc_case_2_36, {
	test_case_2_36_stream_0, test_case_2_36_stream_1, test_case_2_36_stream_2}, 0, 0}
	, {
		"2.37", name_case_2_37, desc_case_2_37, {
	test_case_2_37_stream_0, test_case_2_37_stream_1, test_case_2_37_stream_2}, 0, 0}
	, {
		"2.38", name_case_2_38, desc_case_2_38, {
	test_case_2_38_stream_0, test_case_2_38_stream_1, test_case_2_38_stream_2}, 0, 0}
	, {
		"2.39", name_case_2_39, desc_case_2_39, {
	test_case_2_39_stream_0, test_case_2_39_stream_1, test_case_2_39_stream_2}, 0, 0}
	, {
		"2.40", name_case_2_40, desc_case_2_40, {
	test_case_2_40_stream_0, test_case_2_40_stream_1, test_case_2_40_stream_2}, 0, 0}
	, {
		"2.41", name_case_2_41, desc_case_2_41, {
	test_case_2_41_stream_0, test_case_2_41_stream_1, test_case_2_41_stream_2}, 0, 0}
	, {
		"2.42", name_case_2_42, desc_case_2_42, {
	test_case_2_42_stream_0, test_case_2_42_stream_1, test_case_2_42_stream_2}, 0, 0}
	, {
		"2.43", name_case_2_43, desc_case_2_43, {
	test_case_2_43_stream_0, test_case_2_43_stream_1, test_case_2_43_stream_2}, 0, 0}
	, {
		"2.44", name_case_2_44, desc_case_2_44, {
	test_case_2_44_stream_0, test_case_2_44_stream_1, test_case_2_44_stream_2}, 0, 0}
	, {
	NULL,}
};

static int summary = 0;

void print_header(void)
{
	if (verbose <= 0)
		return;
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "\n%s - %s - %s - Conformance Test Suite\n", lstdname, lpkgname, shortname);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

int do_tests(void)
{
	int i;
	int result = __RESULT_INCONCLUSIVE;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	int skipped = 0;
	int aborted = 0;
	int num_exit;
	print_header();
	show = 0;
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "\nUsing device %s\n\n", devname);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (begin_tests() == __RESULT_SUCCESS) {
		end_tests();
		show = 1;
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (aborted) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				inconclusive++;
				continue;
			}
			if (tests[i].result)
				continue;
			if (!tests[i].run) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				skipped++;
				continue;
			}
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "\nTest Case %s-%s/%s: %s\n", sstdname, shortname, tests[i].numb, tests[i].name);
				if (verbose > 1)
					fprintf(stdout, "%s\n", tests[i].desc);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((result = begin_tests()) != __RESULT_SUCCESS)
				goto inconclusive;
			result = test_run(tests[i].stream);
			end_tests();
			switch (result) {
			case __RESULT_SUCCESS:
				successes++;
				if (verbose > 0) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "*********\n");
					fprintf(stdout, "********* Test Case SUCCESSFUL\n");
					fprintf(stdout, "*********\n\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case __RESULT_FAILURE:
				failures++;
				if (verbose > 0) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case FAILED\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			default:
			case __RESULT_INCONCLUSIVE:
			      inconclusive:
				inconclusive++;
				if (verbose > 0) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "?????????\n");
					fprintf(stdout, "????????? Test Case INCONCLUSIVE\n");
					fprintf(stdout, "?????????\n\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			}
			tests[i].result = result;
			if (exit_on_failure && result != __RESULT_SUCCESS)
				aborted = 1;
		}
		if (summary && verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
			for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
				if (tests[i].run) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "Test Case %s-%s/%-10s ", sstdname, shortname, tests[i].numb);
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
					switch (tests[i].result) {
					case __RESULT_SUCCESS:
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "SUCCESS\n");
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case __RESULT_FAILURE:
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "FAILURE\n");
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
						break;
					default:
					case __RESULT_INCONCLUSIVE:
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "INCONCLUSIVE\n");
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
						break;
					}
				}
			}
		}
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n");
			fprintf(stdout, "========= %2d successes   \n", successes);
			fprintf(stdout, "========= %2d failures    \n", failures);
			fprintf(stdout, "========= %2d inconclusive\n", inconclusive);
			fprintf(stdout, "========= %2d skipped     \n", skipped);
			if (!(aborted + failures + inconclusive))
				fprintf(stdout, "\nDone.\n\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if (aborted) {
			lockf(fileno(stderr), F_LOCK, 0);
			if (verbose > 0)
				fprintf(stderr, "\n");
			fprintf(stderr, "Test Suite aborted due to failure.\n");
			if (verbose > 0)
				fprintf(stderr, "\n");
			fflush(stderr);
			lockf(fileno(stderr), F_ULOCK, 0);
		} else if (failures + inconclusive) {
			lockf(fileno(stderr), F_LOCK, 0);
			if (verbose > 0)
				fprintf(stderr, "\n");
			fprintf(stderr, "Test Suite failed.\n");
			if (verbose > 0)
				fprintf(stderr, "\n");
			fflush(stderr);
			lockf(fileno(stderr), F_ULOCK, 0);
		}
		return (aborted);
	} else {
		end_tests();
		show = 1;
		lockf(fileno(stderr), F_LOCK, 0);
		fprintf(stderr, "Test Suite setup failed!\n");
		fflush(stderr);
		lockf(fileno(stderr), F_ULOCK, 0);
		return (2);
	}
}

void splash(int argc, char *argv[])
{
	if (!verbose)
		return;
	print_header();
	fprintf(stdout, "\
\n\
Copyright (c) 2001-2004 OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001 Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Unauthorized distribution or duplication is prohibited.\n\
\n\
This software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompilation.\n\
No part of this software or related documentation may  be reproduced in any form\n\
by any means without the prior  written  authorization of the  copyright holder,\n\
and licensors, if any.\n\
\n\
The recipient of this document,  by its retention and use, warrants that the re-\n\
cipient  will protect this  information and  keep it confidential,  and will not\n\
disclose the information contained  in this document without the written permis-\n\
sion of its owner.\n\
\n\
The author reserves the right to revise  this software and documentation for any\n\
reason,  including but not limited to, conformity with standards  promulgated by\n\
various agencies, utilization of advances in the state of the technical arts, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   The author  is under no  obligation to\n\
provide any feature listed herein.\n\
\n\
As an exception to the above,  this software may be  distributed  under the  GNU\n\
General Public License  (GPL)  Version 2  or later,  so long as  the software is\n\
distributed with,  and only used for the testing of,  OpenSS7 modules,  drivers,\n\
and libraries.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the  U.S. Government  (\"Government\"),  the following provisions apply to you.\n\
If the Software is  supplied by the Department of Defense (\"DoD\"), it is classi-\n\
fied as  \"Commercial Computer Software\"  under paragraph 252.227-7014 of the DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-Government  users).  If\n\
the Software is supplied to any unit or agency of the Government other than DoD,\n\
it is classified as  \"Restricted Computer Software\" and the  Government's rights\n\
in the  Software are defined in  paragraph 52.227-19 of the Federal  Acquisition\n\
Regulations  (\"FAR\") (or any success  regulations) or, in the  cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
");
}

void version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 1997-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
", argv[0], ident);
}

void usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
", argv[0]);
}

void help(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    -d, --device DEVICE\n\
        Device name to open [default: %2$s].\n\
    -e, --exit\n\
        Exit on the first failed or inconclusive test case.\n\
    -l, --list [RANGE]\n\
        List test case names within a range [default: all] and exit.\n\
    -f, --fast [SCALE]\n\
        Increase speed of tests by scaling timers [default: 50]\n\
    -s, --summary\n\
        Print a test case summary at end of testing [default: off]\n\
    -o, --onetest [TESTCASE]\n\
        Run a single test case.\n\
    -t, --tests [RANGE]\n\
        Run a range of test cases.\n\
    -m, --messages\n\
        Display messages. [default: off]\n\
    -q, --quiet\n\
        Suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        Increase verbosity or set to LEVEL [default: 1]\n\
        This option may be repeated.\n\
    -h, --help, -?, --?\n\
        Prints this usage message and exists\n\
    -V, --version\n\
        Prints the version and exists\n\
", argv[0], devname);
}

int main(int argc, char *argv[])
{
	size_t l, n;
	int range = 0;
	struct test_case *t;
	int tests_to_run = 0;
	for (t = tests; t->numb; t++) {
		if (!t->result) {
			t->run = 1;
			tests_to_run++;
		}
	}
	for (;;) {
		int c, val;
#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"device",	required_argument,	NULL, 'd'},
			{"exit",	no_argument,		NULL, 'e'},
			{"list",	optional_argument,	NULL, 'l'},
			{"fast",	optional_argument,	NULL, 'f'},
			{"summary",	no_argument,		NULL, 's'},
			{"onetest",	required_argument,	NULL, 'o'},
			{"tests",	required_argument,	NULL, 't'},
			{"messages",	no_argument,		NULL, 'm'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "d:el::f::so:t:mqvhV?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "d:el::f::so:t:mqvhV?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'd':
			if (optarg) {
				snprintf(devname, sizeof(devname), "%s", optarg);
				break;
			}
			goto bad_option;
		case 'e':
			exit_on_failure = 1;
			break;
		case 'l':
			if (optarg) {
				l = strnlen(optarg, 16);
				fprintf(stdout, "\n");
				for (n = 0, t = tests; t->numb; t++)
					if (!strncmp(t->numb, optarg, l)) {
						fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
						if (verbose > 1)
							fprintf(stdout, "%s\n\n", t->desc);
						fflush(stdout);
						n++;
					}
				if (!n) {
					fprintf(stderr, "WARNING: specification `%s' matched no test\n", optarg);
					fflush(stderr);
					goto bad_option;
				}
				if (verbose <= 1)
					fprintf(stdout, "\n");
				fflush(stdout);
				exit(0);
			} else {
				fprintf(stdout, "\n");
				for (t = tests; t->numb; t++) {
					fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
					if (verbose > 1)
						fprintf(stdout, "%s\n\n", t->desc);
					fflush(stdout);
				}
				if (verbose <= 1)
					fprintf(stdout, "\n");
				fflush(stdout);
				exit(0);
			}
			break;
		case 'f':
			if (optarg)
				timer_scale = atoi(optarg);
			else
				timer_scale = 50;
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n", timer_scale);
			break;
		case 's':
			summary = 1;
			break;
		case 'o':
			if (optarg) {
				if (!range) {
					for (t = tests; t->numb; t++)
						t->run = 0;
					tests_to_run = 0;
				}
				range = 1;
				for (n = 0, t = tests; t->numb; t++)
					if (!strncmp(t->numb, optarg, 16)) {
						if (!t->result) {
							t->run = 1;
							n++;
							tests_to_run++;
						}
					}
				if (!n) {
					fprintf(stderr, "WARNING: specification `%s' matched no test\n", optarg);
					fflush(stderr);
					goto bad_option;
				}
				break;
			}
			goto bad_option;
		case 'q':
			verbose = 0;
			break;
		case 'v':
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 't':
			l = strnlen(optarg, 16);
			if (!range) {
				for (t = tests; t->numb; t++)
					t->run = 0;
				tests_to_run = 0;
			}
			range = 1;
			for (n = 0, t = tests; t->numb; t++)
				if (!strncmp(t->numb, optarg, l)) {
					if (!t->result) {
						t->run = 1;
						n++;
						tests_to_run++;
					}
				}
			if (!n) {
				fprintf(stderr, "WARNING: specification `%s' matched no test\n", optarg);
				fflush(stderr);
				goto bad_option;
			}
			break;
		case 'm':
			show_msg = 1;
			break;
		case 'H':	/* -H */
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				while (optind < argc)
					fprintf(stderr, "%s ", argv[optind++]);
				fprintf(stderr, "\n");
				fflush(stderr);
			}
		      bad_usage:
			usage(argc, argv);
			exit(2);
		}
	}
	/* 
	 * dont' ignore non-option arguments
	 */
	if (optind < argc)
		goto bad_nonopt;
	if (!tests_to_run) {
		if (verbose > 0) {
			fprintf(stderr, "%s: error: no tests to run\n", argv[0]);
			fflush(stderr);
		}
		exit(2);
	}
	splash(argc, argv);
	do_tests();
	exit(0);
}
