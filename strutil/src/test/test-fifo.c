/*****************************************************************************

 @(#) $RCSfile: test-fifo.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/12/18 08:16:54 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written authorization
 of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that the
 recipient will protect this information and keep it confidential, and will
 not disclose the information contained in this document without the written
 permission of its owner.

 The author reserves the right to revise this software and documentation for
 any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of the
 technical arts, or the reflection of changes in the design of any techniques,
 or procedures embodied, described, or referred to herein.  The author is
 under no obligation to provide any feature listed herein.

 -----------------------------------------------------------------------------

 As an exception to the above, this software may be distributed under the GNU
 General Public License (GPL) Version 2, so long as the software is distributed
 with, and only used for the testing of, OpenSS7 modules, drivers, and
 libraries.

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

 Last Modified $Date: 2006/12/18 08:16:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-fifo.c,v $
 Revision 0.9.2.13  2006/12/18 08:16:54  brian
 - resolve device numbering

 Revision 0.9.2.12  2006/03/10 07:24:16  brian
 - rationalized streams and strutil package sources

 Revision 0.9.2.11  2006/03/03 12:17:36  brian
 - 64-bit and SMP compatibility

 Revision 0.9.2.10  2005/12/28 10:01:22  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.9  2005/07/18 12:38:49  brian
 - standard indentation

 Revision 0.9.2.8  2005/06/06 12:03:56  brian
 - upgraded test suites

 Revision 0.9.2.6  2005/06/06 12:03:56  brian
 - upgraded test suites

 Revision 0.9.2.5  2005/05/15 23:57:08  brian
 - update tests

 Revision 0.9.2.4  2005/05/14 08:39:36  brian
 - updated copyright headers

 Revision 0.9.2.3  2005/05/14 08:34:45  brian
 - copyright header correction

 Revision 0.9.2.2  2005/01/16 23:09:06  brian
 - Added --copying options.

 Revision 0.9.2.1  2004/08/22 06:17:55  brian
 - Checkin on new working branch.

 Revision 1.2  2004/06/09 08:32:58  brian
 - Open works fine but don't want to hold dentries in cache.

 Revision 1.2  2004/06/08 02:27:36  brian
 - Framework for testing streams.

 Revision 1.1  2004/06/07 22:35:57  brian
 - Starting test suites for everything.

 *****************************************************************************/

#ident "@(#) $RCSfile: test-fifo.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/12/18 08:16:54 $"

static char const ident[] = "$RCSfile: test-fifo.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/12/18 08:16:54 $";

#include <sys/types.h>
#include <stropts.h>
#include <stdlib.h>

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

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
#include <time.h>

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/testmod.h>

#include <linux/limits.h>

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "Linux Fast-STREAMS";

/* static const char *spkgname = "LfS"; */
static const char *lstdname = "UNIX 98/SUS Version 2";
static const char *sstdname = "XSI/XSR";
static const char *shortname = "FIFO";
#ifdef LFS
static char devname[256] = "/dev/streams/fifo/0";
#else
static char devname[256] = "/dev/fifo";
#endif

static int exit_on_failure = 0;

static int verbose = 1;

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
static int show_data = 1;

//static int last_prim = 0;
//static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;

int test_fd[3] = { 0, 0, 0 };
pid_t test_pid[3] = { 0, 0, 0 };

#define BUFSIZE 5*4096

#define FFLUSH(stream)

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 200	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define INFINITE_WAIT	-1
#define TEST_DURATION	20000
#define INVALID_ADDRESS ((void *)(long)(-1))

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

int dummy = 0;

struct timeval when;

/*
 *  -------------------------------------------------------------------------
 *
 *  Events and Actions
 *
 *  -------------------------------------------------------------------------
 */
enum {
	__EVENT_EOF = -7, __EVENT_NO_MSG = -6, __EVENT_TIMEOUT = -5, __EVENT_UNKNOWN = -4,
	__RESULT_DECODE_ERROR = -3, __RESULT_SCRIPT_ERROR = -2,
	__RESULT_INCONCLUSIVE = -1, __RESULT_SUCCESS = 0, __RESULT_FAILURE = 1,
	__RESULT_NOTAPPL = 3, __RESULT_SKIPPED = 77,
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

/* lockf does not work well on SMP for some reason */
#if 1
#undef lockf
#define lockf(x,y,z) 0
#endif

#if 0
/*
 *  Return the current time in milliseconds.
 */
static long
now(void)
{
	long ret;
	struct timeval now;

	if (gettimeofday(&now, NULL)) {
		last_errno = errno;
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "***************ERROR! couldn't get time!            !  !                    \n");
		fprintf(stdout, "%20s! %-54s\n", __FUNCTION__, strerror(last_errno));
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
		return (0);
	}
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000L;
	ret += (now.tv_usec + 999L) / 1000L;
	return ret;
}
static long
milliseconds(char *t)
{
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .               :               .  .                    \n");
		fprintf(stdout, "                    .             %6s            .  .                    <%d>\n", t, state);
		fprintf(stdout, "                    .               :               .  .                    \n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	return now();
}
static long
milliseconds_2nd(char *t)
{
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .               :   :           .  .                    \n");
		fprintf(stdout, "                    .               : %6s        .  .                    <%d>\n", t, state);
		fprintf(stdout, "                    .               :   :           .  .                    \n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	return now();
}

/*
 *  Check the current time against the beginning time provided as an argnument
 *  and see if the time inverval falls between the low and high values for the
 *  timer as specified by arguments.  Return SUCCESS if the interval is within
 *  the allowable range and FAILURE otherwise.
 */
static int
check_time(const char *t, long i, long lo, long hi)
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
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    |(%7.3g <= %7.3g <= %7.3g)|  | %6s             <%d>\n", dlo - tol, itv, dhi + tol, t, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (dlo - tol <= itv && itv <= dhi + tol)
		return __RESULT_SUCCESS;
	else
		return __RESULT_FAILURE;
}

static int
time_event(int child, int event)
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
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    | %11.6g                    |  |                    <%d:%03d>\n", t, child, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	return (event);
}
#endif

static int timer_timeout = 0;
static int last_signum = 0;

static void
signal_handler(int signum)
{
	last_signum = signum;
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

static int
start_signals(void)
{
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = signal_handler;
//	act.sa_flags = SA_RESTART | SA_ONESHOT;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGPOLL, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGURG, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGPIPE, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGHUP, &act, NULL))
		return __RESULT_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGPOLL);
	sigaddset(&mask, SIGURG);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	siginterrupt(SIGALRM, 1);
	siginterrupt(SIGPOLL, 1);
	siginterrupt(SIGURG, 1);
	siginterrupt(SIGPIPE, 1);
	siginterrupt(SIGHUP, 1);
	return __RESULT_SUCCESS;
}

/*
 *  Start an interval timer as the overall test timer.
 */
static int
start_tt(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (start_signals())
		return __RESULT_FAILURE;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return __RESULT_FAILURE;
	timer_timeout = 0;
	return __RESULT_SUCCESS;
}

#if 0
static int
start_st(long duration)
{
	long sdur = (duration + timer_scale - 1) / timer_scale;

	return start_tt(sdur);
}
#endif

static int
stop_signals(void)
{
	int result = __RESULT_SUCCESS;
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGPOLL, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGURG, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGPIPE, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGHUP, &act, NULL))
		result = __RESULT_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGPOLL);
	sigaddset(&mask, SIGURG);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	return (result);
}

static int
stop_tt(void)
{
	struct itimerval setting = { {0, 0}, {0, 0} };
	int result = __RESULT_SUCCESS;

	if (setitimer(ITIMER_REAL, &setting, NULL))
		return __RESULT_FAILURE;
	if (stop_signals() != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	timer_timeout = 0;
	return (result);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Printing things
 *
 *  -------------------------------------------------------------------------
 */

char *
errno_string(long err)
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

const char *
event_string(int event)
{
	switch (event) {
	case __EVENT_EOF:
		return ("END OF FILE");
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

const char *
ioctl_string(int cmd, intptr_t arg)
{
	switch (cmd) {
	case I_NREAD:
		return ("I_NREAD");	/* 2.1 */
	case I_PUSH:
		return ("I_PUSH");	/* 2.2 */
	case I_POP:
		return ("I_POP");	/* 2.3 */
	case I_LOOK:
		return ("I_LOOK");	/* 2.4 */
	case I_FLUSH:
		return ("I_FLUSH");	/* 2.5 */
	case I_SRDOPT:
		return ("I_SRDOPT");	/* 2.6 */
	case I_GRDOPT:
		return ("I_GRDOPT");	/* 2.7 */
	case I_STR:
		return ("I_STR");	/* 2.8 */
	case I_SETSIG:
		return ("I_SETSIG");	/* 2.9 */
	case I_GETSIG:
		return ("I_GETSIG");	/* 2.10 */
	case I_FIND:
		return ("I_FIND");	/* 2.11 */
	case I_LINK:
		return ("I_LINK");	/* 2.12 */
	case I_UNLINK:
		return ("I_UNLINK");	/* 2.13 */
	case I_RECVFD:
		return ("I_RECVFD");	/* 2.14 */
	case I_PEEK:
		return ("I_PEEK");	/* 2.15 */
	case I_FDINSERT:
		return ("I_FDINSERT");	/* 2.16 */
	case I_SENDFD:
		return ("I_SENDFD");	/* 2.17 */
#if 0
	case I_E_RECVFD:
		return ("I_E_RECVFD");	/* 2.18 */
#endif
	case I_SWROPT:
		return ("I_SWROPT");	/* 2.19 */
	case I_GWROPT:
		return ("I_GWROPT");	/* 2.20 */
	case I_LIST:
		return ("I_LIST");	/* 2.21 */
	case I_PLINK:
		return ("I_PLINK");	/* 2.22 */
	case I_PUNLINK:
		return ("I_PUNLINK");	/* 2.23 */
	case I_FLUSHBAND:
		return ("I_FLUSHBAND");	/* 2.24 */
	case I_CKBAND:
		return ("I_CKBAND");	/* 2.25 */
	case I_GETBAND:
		return ("I_GETBAND");	/* 2.26 */
	case I_ATMARK:
		return ("I_ATMARK");	/* 2.27 */
	case I_SETCLTIME:
		return ("I_SETCLTIME");	/* 2.28 */
	case I_GETCLTIME:
		return ("I_GETCLTIME");	/* 2.29 */
	case I_CANPUT:
		return ("I_CANPUT");	/* 2.30 */
	case I_SERROPT:
		return ("I_SERROPT");	/* 2.31 */
	case I_GERROPT:
		return ("I_GERROPT");	/* 2.32 */
	case I_ANCHOR:
		return ("I_ANCHOR");	/* 2.33 */
#if 0
	case I_S_RECVFD:
		return ("I_S_RECVFD");	/* 2.34 */
	case I_STATS:
		return ("I_STATS");	/* 2.35 */
	case I_BIGPIPE:
		return ("I_BIGPIPE");	/* 2.36 */
	case I_GETTP:
		return ("I_GETTP");	/* 2.37 */
#endif
	case I_GETMSG:
		return ("I_GETMSG");	/* 2.38 */
	case I_PUTMSG:
		return ("I_PUTMSG");	/* 2.39 */
	case I_PUTPMSG:
		return ("I_PUTPMSG");	/* 2.40 */
	case I_GETPMSG:
		return ("I_GETPMSG");	/* 2.41 */
	case I_PIPE:
		return ("I_PIPE");	/* 2.42 */
	case I_FIFO:
		return ("I_FIFO");	/* 2.43 */
	case I_AUTOPUSH:
		return ("I_AUTOPUSH");	/* 2.44 */
	case I_HEAP_REPORT:
		return ("I_HEAP_REPORT");	/* 2.45 */
	case I_FATTACH:
		return ("I_FATTACH");	/* 2.46 */
	case I_FDETACH:
		return ("I_FDETACH");	/* 2.47 */
	case TM_IOC_HANGUP:
		return ("TM_IOC_HANGUP");
	case TM_IOC_RDERR:
		return ("TM_IOC_RDERR");
	case TM_IOC_WRERR:
		return ("TM_IOC_WRERR");
	case TM_IOC_RWERR:
		return ("TM_IOC_RWERR");
	case TM_IOC_PSIGNAL:
		return ("TM_IOC_PSIGNAL");
	case TM_IOC_NSIGNAL:
		return ("TM_IOC_NSIGNAL");
	default:
		return ("(unexpected)");
	}
}

const char *
signal_string(int signum)
{
	switch (signum) {
	case SIGHUP:
		return ("SIGHUP");
	case SIGINT:
		return ("SIGINT");
	case SIGQUIT:
		return ("SIGQUIT");
	case SIGILL:
		return ("SIGILL");
	case SIGABRT:
		return ("SIGABRT");
	case SIGFPE:
		return ("SIGFPE");
	case SIGKILL:
		return ("SIGKILL");
	case SIGSEGV:
		return ("SIGSEGV");
	case SIGPIPE:
		return ("SIGPIPE");
	case SIGALRM:
		return ("SIGALRM");
	case SIGTERM:
		return ("SIGTERM");
	case SIGUSR1:
		return ("SIGUSR1");
	case SIGUSR2:
		return ("SIGUSR2");
	case SIGCHLD:
		return ("SIGCHLD");
	case SIGCONT:
		return ("SIGCONT");
	case SIGSTOP:
		return ("SIGSTOP");
	case SIGTSTP:
		return ("SIGTSTP");
	case SIGTTIN:
		return ("SIGTTIN");
	case SIGTTOU:
		return ("SIGTTOU");
	case SIGBUS:
		return ("SIGBUS");
	case SIGPOLL:
		return ("SIGPOLL");
	case SIGPROF:
		return ("SIGPROF");
	case SIGSYS:
		return ("SIGSYS");
	case SIGTRAP:
		return ("SIGTRAP");
	case SIGURG:
		return ("SIGURG");
	case SIGVTALRM:
		return ("SIGVTALRM");
	case SIGXCPU:
		return ("SIGXCPU");
	case SIGXFSZ:
		return ("SIGXFSZ");
	default:
		return ("unknown");
	}
}

const char *
poll_string(short events)
{
	if (events & POLLIN)
		return ("POLLIN");
	if (events & POLLPRI)
		return ("POLLPRI");
	if (events & POLLOUT)
		return ("POLLOUT");
	if (events & POLLRDNORM)
		return ("POLLRDNORM");
	if (events & POLLRDBAND)
		return ("POLLRDBAND");
	if (events & POLLWRNORM)
		return ("POLLWRNORM");
	if (events & POLLWRBAND)
		return ("POLLWRBAND");
	if (events & POLLERR)
		return ("POLLERR");
	if (events & POLLHUP)
		return ("POLLHUP");
	if (events & POLLNVAL)
		return ("POLLNVAL");
	if (events & POLLMSG)
		return ("POLLMSG");
	return ("none");
}

const char *
poll_events_string(short events)
{
	static char string[256] = "";
	int offset = 0, size = 256, len = 0;

	if (events & POLLIN) {
		len = snprintf(string + offset, size, "POLLIN|");
		offset += len;
		size -= len;
	}
	if (events & POLLPRI) {
		len = snprintf(string + offset, size, "POLLPRI|");
		offset += len;
		size -= len;
	}
	if (events & POLLOUT) {
		len = snprintf(string + offset, size, "POLLOUT|");
		offset += len;
		size -= len;
	}
	if (events & POLLRDNORM) {
		len = snprintf(string + offset, size, "POLLRDNORM|");
		offset += len;
		size -= len;
	}
	if (events & POLLRDBAND) {
		len = snprintf(string + offset, size, "POLLRDBAND|");
		offset += len;
		size -= len;
	}
	if (events & POLLWRNORM) {
		len = snprintf(string + offset, size, "POLLWRNORM|");
		offset += len;
		size -= len;
	}
	if (events & POLLWRBAND) {
		len = snprintf(string + offset, size, "POLLWRBAND|");
		offset += len;
		size -= len;
	}
	if (events & POLLERR) {
		len = snprintf(string + offset, size, "POLLERR|");
		offset += len;
		size -= len;
	}
	if (events & POLLHUP) {
		len = snprintf(string + offset, size, "POLLHUP|");
		offset += len;
		size -= len;
	}
	if (events & POLLNVAL) {
		len = snprintf(string + offset, size, "POLLNVAL|");
		offset += len;
		size -= len;
	}
	if (events & POLLMSG) {
		len = snprintf(string + offset, size, "POLLMSG|");
		offset += len;
		size -= len;
	}
	return (string);
}

void
print_less(int child)
{
	if (verbose < 1 || !show)
		return;
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	switch (child) {
	case 0:
		fprintf(stdout, " .         .  <---->|               .               :  :                    \n");
		fprintf(stdout, " .  (more) .  <---->|               .               :  :                     [%d:%03d]\n", child, state);
		fprintf(stdout, " .         .  <---->|               .               :  :                    \n");
		break;
	case 1:
		fprintf(stdout, "                    :               .               :  |<-->  .         .   \n");
		fprintf(stdout, "                    :               .               :  |<-->  . (more)  .    [%d:%03d]\n", child, state);
		fprintf(stdout, "                    :               .               :  |<-->  .         .   \n");
		break;
	case 2:
		fprintf(stdout, "                    :               .               |<-:--->  .         .   \n");
		fprintf(stdout, "                    :               .               |<-:--->  . (more)  .    [%d:%03d]\n", child, state);
		fprintf(stdout, "                    :               .               |<-:--->  .         .   \n");
		break;
	}
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	show = 0;
	return;
}

void
print_more(void)
{
	show = 1;
}

void
print_simple(int child, const char *msgs[])
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child]);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_simple_int(int child, const char *msgs[], int val)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_double_int(int child, const char *msgs[], int val, int val2)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val, val2);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_triple_int(int child, const char *msgs[], int val, int val2, int val3)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val, val2, val3);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_simple_string(int child, const char *msgs[], const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_pipe(int child)
{
	static const char *msgs[] = {
		"  pipe()      ----->v  v<------------------------------>v                   \n",
		"                    v  v<------------------------------>v<-----     pipe()  \n",
		"                    .  .                                .                   \n",
	};

	if (verbose > 3)
		print_simple(child, msgs);
}

void
print_open(int child, const char* name)
{
	static const char *msgs[] = {
		"open()        ----->v %-30s |  |                   \n",
		"  open()      ----->v %-30s |  |                   \n",
		"    open()    ----->v %-30s |  |                   \n",
		"                    . %-30s .  .                   \n",
	};

	if (verbose > 3)
		print_simple_string(child, msgs, name);
}

void
print_close(int child)
{
	static const char *msgs[] = {
		"close()       ----->X                                |  |                   \n",
		"  close()     ----->X                                |  |                   \n",
		"    close()   ----->X                                |  |                   \n",
		"                    .                                .  .                   \n",
	};

	if (verbose > 3)
		print_simple(child, msgs);
}

void
print_preamble(int child)
{
	static const char *msgs[] = {
		"--------------------+------------Preamble------------+--+                   \n",
		"  ------------------+------------Preamble------------+--+                   \n",
		"    ----------------+------------Preamble------------+--+                   \n",
		"--------------------+-------------Preamble--------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_notapplicable(int child)
{
	static const char *msgs[] = {
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-|X-|                    [%d:%03d]\n",
		"  X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-|X-|                    [%d:%03d]\n",
		"    X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-|X-|                    [%d:%03d]\n",
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-|X-|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
}

void
print_skipped(int child)
{
	static const char *msgs[] = {
		"::::::::::::::::::::|:::::::::::: SKIPPED :::::::::::|::|                    [%d:%03d]\n",
		"  ::::::::::::::::::|:::::::::::: SKIPPED :::::::::::|::|                    [%d:%03d]\n",
		"    ::::::::::::::::|:::::::::::: SKIPPED :::::::::::|::|                    [%d:%03d]\n",
		"::::::::::::::::::::|:::::::::::: SKIPPED :::::::::::|::|::::::::::::::::::: [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
}

void
print_inconclusive(int child)
{
	static const char *msgs[] = {
		"????????????????????|?????????? INCONCLUSIVE ????????|??|                    [%d:%03d]\n",
		"  ??????????????????|?????????? INCONCLUSIVE ????????|??|                    [%d:%03d]\n",
		"    ????????????????|?????????? INCONCLUSIVE ????????|??|                    [%d:%03d]\n",
		"????????????????????|?????????? INCONCLUSIVE ????????|??|??????????????????? [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
}

void
print_test(int child)
{
	static const char *msgs[] = {
		"--------------------+---------------Test-------------+--+                   \n",
		"  ------------------+---------------Test-------------+--+                   \n",
		"    ----------------+---------------Test-------------+--+                   \n",
		"--------------------+---------------Test-------------+--+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXX|XX|                    [%d:%03d]\n",
		"  XXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXX|XX|                    [%d:%03d]\n",
		"    XXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXX|XX|                    [%d:%03d]\n",
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
}

void
print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########### SCRIPT ERROR #######|##|                    [%d:%03d]\n",
		"  ##################|########### SCRIPT ERROR #######|##|                    [%d:%03d]\n",
		"    ################|########### SCRIPT ERROR #######|##|                    [%d:%03d]\n",
		"####################|########### SCRIPT ERROR #######|##|################### [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
}

void
print_passed(int child)
{
	static const char *msgs[] = {
		"********************|************* PASSED ***********|**|                    [%d:%03d]\n",
		"  ******************|************* PASSED ***********|**|                    [%d:%03d]\n",
		"    ****************|************* PASSED ***********|**|                    [%d:%03d]\n",
		"********************|************* PASSED ***********|**|******************* [%d:%03d]\n",
	};

	if (verbose > 2)
		print_double_int(child, msgs, child, state);
}

void
print_postamble(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------Postamble----------+--+                   \n",
		"  ------------------+-------------Postamble----------+--+                   \n",
		"    ----------------+-------------Postamble----------+--+                   \n",
		"--------------------+-------------Postamble----------+--+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_test_end(int child)
{
	static const char *msgs[] = {
		"--------------------+--------------------------------+--+                   \n",
		"  ------------------+--------------------------------+--+                   \n",
		"    ----------------+--------------------------------+--+                   \n",
		"--------------------+--------------------------------+--+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d:%03d}\n",
		"  @@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d:%03d}\n",
		"    @@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d:%03d}\n",
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d:%03d}\n",
		"  &&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d:%03d}\n",
		"    &&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d:%03d}\n",
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++ [%d:%03d]\n",
		"  ++++++++++++++++++|++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++ [%d:%03d]\n",
		"    ++++++++++++++++|++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++ [%d:%03d]\n",
		"++++++++++++++++++++|++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++ [%d:%03d]\n",
	};

	if (show_timeout || verbose > 0) {
		print_double_int(child, msgs, child, state);
		show_timeout--;
	}
}

void
print_nothing(int child)
{
	static const char *msgs[] = {
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - | -|                    [%d:%03d]\n",
		"  - - - - - - - - - |- - - - - - -nothing! - - - - - | -|                    [%d:%03d]\n",
		"    - - - - - - - - |- - - - - - -nothing! - - - - - | -|                    [%d:%03d]\n",
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - | -|- - - - - - - - - - [%d:%03d]\n",
	};

	if (verbose > 1)
		print_double_int(child, msgs, child, state);
}

void
print_string_state(int child, const char *msgs[], const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_signal(int child, int signum)
{
	static const char *msgs[] = {
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"  >>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"    >>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, signal_string(signum));
}

void
print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                |  |                    [%d:%03d]\n",
		"  %-14s--->|                                |  |                    [%d:%03d]\n",
		"    %-14s->|                                |  |                    [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_command(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                |  |                    [%d:%03d]\n",
		"  %-14s--->|                                |  |                    [%d:%03d]\n",
		"    %-14s->|                                |  |                    [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_string_state(child, msgs, command);
}

void
print_double_string_state(int child, const char *msgs[], const char *string1, const char *string2)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string1, string2, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_command_info(int child, const char *command, const char *info)
{
	static const char *msgs[] = {
		"%-14s----->|       %16s         |  |                    [%d:%03d]\n",
		"  %-14s--->|       %16s         |  |                    [%d:%03d]\n",
		"    %-14s->|       %16s         |  |                    [%d:%03d]\n",
		"                    | %-14s %16s|  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_double_string_state(child, msgs, command, info);
}

void
print_string_int_state(int child, const char *msgs[], const char *string, int val)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, val, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_errno(int child, long error)
{
	static const char *msgs[] = {
		"%-14s<----/|                                |  |                    [%d:%03d]\n",
		"  %-14s<--/|                                |  |                    [%d:%03d]\n",
		"    %-14s</|                                |  |                    [%d:%03d]\n",
		"                    |          [%14s]      |  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_string_state(child, msgs, errno_string(error));
}

void
print_success(int child)
{
	static const char *msgs[] = {
		"ok            <----/|                                |  |                    [%d:%03d]\n",
		"  ok          <----/|                                |  |                    [%d:%03d]\n",
		"    ok        <----/|                                |  |                    [%d:%03d]\n",
		"                    |                 ok             |  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_double_int(child, msgs, child, state);
}

void
print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"%10d<--------/|                                |  |                    [%d:%03d]\n",
		"  %10d<------/|                                |  |                    [%d:%03d]\n",
		"    %10d<----/|                                |  |                    [%d:%03d]\n",
		"                    |            [%10d]        |  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_triple_int(child, msgs, value, child, state);
}

void
print_int_string_state(int child, const char *msgs[], const int value, const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], value, string, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_poll_value(int child, int value, short revents)
{
	static const char *msgs[] = {
		"%10d<--------/|%-32s|  |                    [%d:%03d]\n",
		"  %10d<------/|%-32s|  |                    [%d:%03d]\n",
		"    %10d<----/|%-32s|  |                    [%d:%03d]\n",
		"          %10d|%-32s|  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_int_string_state(child, msgs, value, poll_events_string(revents));
}

void
print_ioctl(int child, int cmd, intptr_t arg)
{
	print_command_info(child, "ioctl(2)------", ioctl_string(cmd, arg));
}

void
print_poll(int child, short events)
{
	print_command_info(child, "poll(2)-------", poll_string(events));
}

void
print_datcall(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"%1$14s----->|- - %2$4d bytes- - - - - - - - ->|- |                    [%3$d:%4$03d]\n",
		"  %1$14s--->|- - %2$4d bytes- - - - - - - - ->|- |                    [%3$d:%4$03d]\n",
		"    %1$14s->|- - %2$4d bytes- - - - - - - - ->|- |                    [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show_data) || verbose > 1)
		print_string_int_state(child, msgs, command, bytes);
}

void
print_expect(int child, int want)
{
	static const char *msgs[] = {
		"(%-14s)    |- - - - - -[Expected]- - - - - -|- |                     [%d:%03d]\n",
		"  (%-14s)  |- - - - - -[Expected]- - - - - -|- |                     [%d:%03d]\n",
		"    (%-14s)|- - - - - -[Expected]- - - - - -|- |                     [%d:%03d]\n",
		"                    |- - -[Expected %-14s] -|- |                     [%d:%03d]\n",
	};

	if (verbose > 0 && show)
		print_string_state(child, msgs, event_string(want));
}

void
print_string(int child, const char *string)
{
	static const char *msgs[] = {
		"%-16s    |                                |  |                    \n",
		"  %-16s  |                                |  |                    \n",
		"    %-16s|                                |  |                    \n",
		"                    |  |      %-16s       |  |                    \n",
	};

	if (verbose > 1 && show)
		print_simple_string(child, msgs, string);
}

void
print_time_state(int child, const char *msgs[], ulong time)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], time, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_waiting(int child, ulong time)
{
	static const char *msgs[] = {
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / /|/ |                    [%d:%03d]\n",
		"  / / / / / / / / / | / / / Waiting %03lu seconds / / /|/ |                    [%d:%03d]\n",
		"    / / / / / / / / | / / / Waiting %03lu seconds / / /|/ |                    [%d:%03d]\n",
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / /|/ | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 0 && show)
		print_time_state(child, msgs, time);
}

void
print_float_state(int child, const char *msgs[], float time)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], time, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_mwaiting(int child, struct timespec *time)
{
	static const char *msgs[] = {
		"/ / / / / / / / / / | / / Waiting %8.4f seconds / |/ |                    [%d:%03d]\n",
		"  / / / / / / / / / | / / Waiting %8.4f seconds / |/ |                    [%d:%03d]\n",
		"    / / / / / / / / | / / Waiting %8.4f seconds / |/ |                    [%d:%03d]\n",
		"/ / / / / / / / / / | / / Waiting %8.4f seconds / |/ | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 0 && show) {
		float delay;

		delay = time->tv_nsec;
		delay = delay / 1000000000;
		delay = delay + time->tv_sec;
		print_float_state(child, msgs, delay);
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Driver actions.
 *
 *  -------------------------------------------------------------------------
 */

int
test_waitsig(int child)
{
	int signum;
	sigset_t set;

	sigemptyset(&set);
	while ((signum = last_signum) == 0)
		sigsuspend(&set);
	print_signal(child, signum);
	return (__RESULT_SUCCESS);

}

int
test_ioctl(int child, int cmd, intptr_t arg)
{
	print_ioctl(child, cmd, arg);
	if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_insertfd(int child, int resfd, int offset, struct strbuf *ctrl, struct strbuf *data, int flags)
{
	struct strfdinsert fdi;

	if (ctrl) {
		fdi.ctlbuf.maxlen = ctrl->maxlen;
		fdi.ctlbuf.len = ctrl->len;
		fdi.ctlbuf.buf = ctrl->buf;
	} else {
		fdi.ctlbuf.maxlen = -1;
		fdi.ctlbuf.len = 0;
		fdi.ctlbuf.buf = NULL;
	}
	if (data) {
		fdi.databuf.maxlen = data->maxlen;
		fdi.databuf.len = data->len;
		fdi.databuf.buf = data->buf;
	} else {
		fdi.databuf.maxlen = -1;
		fdi.databuf.len = 0;
		fdi.databuf.buf = NULL;
	}
	fdi.flags = flags;
	fdi.fildes = resfd;
	fdi.offset = offset;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) & fdi) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

int
test_putmsg(int child, struct strbuf *ctrl, struct strbuf *data, int flags)
{
	print_datcall(child, "putmsg(2)-----", data ? data->len : -1);
	if ((last_retval = putmsg(test_fd[child], ctrl, data, flags)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_putpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int band, int flags)
{
	print_datcall(child, "putpmsg(2)----", data ? data->len : -1);
	if ((last_retval = putpmsg(test_fd[child], ctrl, data, band, flags)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_write(int child, const void *buf, size_t len)
{
	print_datcall(child, "write(2)------", len);
	if ((last_retval = write(test_fd[child], buf, len)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_writev(int child, const struct iovec *iov, int num)
{
	print_syscall(child, "writev(2)-----");
	if ((last_retval = writev(test_fd[child], iov, num)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_getmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *flagp)
{
	print_datcall(child, "getmsg(2)-----", data ? data->maxlen : -1);
	if ((last_retval = getmsg(test_fd[child], ctrl, data, flagp)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_getpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *bandp, int *flagp)
{
	print_datcall(child, "getpmsg(2)----", data ? data->maxlen : -1);
	if ((last_retval = getpmsg(test_fd[child], ctrl, data, bandp, flagp)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_read(int child, void *buf, size_t count)
{
	print_datcall(child, "read(2)-------", count);
	if ((last_retval = read(test_fd[child], buf, count)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_readv(int child, const struct iovec *iov, int count)
{
	print_syscall(child, "readv(2)------");
	if ((last_retval = readv(test_fd[child], iov, count)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_nonblock(int child)
{
	long flags;

	print_syscall(child, "fcntl(2)------");
	if ((flags = last_retval = fcntl(test_fd[child], F_GETFL)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	print_syscall(child, "fcntl(2)------");
	if ((last_retval = fcntl(test_fd[child], F_SETFL, flags | O_NONBLOCK)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_block(int child)
{
	long flags;

	print_syscall(child, "fcntl(2)------");
	if ((flags = last_retval = fcntl(test_fd[child], F_GETFL)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	print_syscall(child, "fcntl(2)------");
	if ((last_retval = fcntl(test_fd[child], F_SETFL, flags & ~O_NONBLOCK)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_isastream(int child)
{
	int result;

	print_syscall(child, "isastream(2)--");
	if ((result = last_retval = isastream(test_fd[child])) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_poll(int child, const short events, short *revents, long ms)
{
	struct pollfd pfd = { .fd = test_fd[child], .events = events, .revents = 0 };
	int result;

	print_poll(child, events);
	if ((result = last_retval = poll(&pfd, 1, ms)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_poll_value(child, last_retval, pfd.revents);
	if (last_retval == 1 && revents)
		*revents = pfd.revents;
	return (__RESULT_SUCCESS);
}

int
test_pipe(int child)
{
	int fds[2];

	print_pipe(child);
	if (pipe(fds) >= 0) {
		test_fd[child + 0] = fds[0];
		test_fd[child + 1] = fds[1];
		print_success(child);
		return (__RESULT_SUCCESS);
	}
	print_errno(child, (last_errno = errno));
	return (__RESULT_FAILURE);
}

int
test_fopen(int child, const char *name, int flags)
{
	int fd;

	print_open(child, name);
	if ((fd = open(name, flags)) >= 0) {
		print_success(child);
		return (fd);
	}
	print_errno(child, (last_errno = errno));
	return (fd);
}

int
test_fclose(int child, int fd)
{
	print_close(child);
	if (close(fd) >= 0) {
		print_success(child);
		return __RESULT_SUCCESS;
	}
	print_errno(child, (last_errno = errno));
	return __RESULT_FAILURE;
}

int
test_open(int child, const char *name, int flags)
{
	int fd;

	if ((fd = test_fopen(child, name, flags)) >= 0) {
		test_fd[child] = fd;
		return (__RESULT_SUCCESS);
	}
	return (__RESULT_FAILURE);
}

int
test_close(int child)
{
	int fd = test_fd[child];

	test_fd[child] = 0;
	return test_fclose(child, fd);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Stream Initialization
 *
 *  -------------------------------------------------------------------------
 */

int
stream_start(int child, int index)
{
	switch (child) {
	case 1:
		if (test_pipe(0) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_ioctl(1, I_SRDOPT, (intptr_t) RMSGD) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_ioctl(1, I_PUSH, (intptr_t) "pipemod") != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		return __RESULT_SUCCESS;
	case 2:
		return __RESULT_SUCCESS;
	case 0:
#if 0
		if (test_ioctl(0, I_SRDOPT, (intptr_t) RMSGD) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_ioctl(0, I_PUSH, (intptr_t) "dummymod") != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
#endif
		return __RESULT_SUCCESS;
	default:
		return __RESULT_FAILURE;
	}
}

int
stream_stop(int child)
{
	switch (child) {
	case 1:
		if (test_ioctl(1, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_close(1) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_close(0) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		return __RESULT_SUCCESS;
	case 2:
		return __RESULT_SUCCESS;
	case 0:
#if 0
		if (test_ioctl(0, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
#endif
		return __RESULT_SUCCESS;
	default:
		return __RESULT_FAILURE;
	}
}

void
test_sleep(int child, unsigned long t)
{
	print_waiting(child, t);
	sleep(t);
}

void
test_msleep(int child, unsigned long m)
{
	struct timespec time;

	time.tv_sec = m / 1000;
	time.tv_nsec = (m % 1000) * 1000000;
	print_mwaiting(child, &time);
	nanosleep(&time, NULL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test harness initialization and termination.
 *
 *  -------------------------------------------------------------------------
 */

static int
begin_tests(int index)
{
	state = 0;
	show_acks = 1;
	return (__RESULT_SUCCESS);
}

static int
end_tests(int index)
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

int
preamble_0(int child)
{
	if (!test_fd[child] && test_open(child, devname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
preamble_0_1(int child)
{
	if (!test_fd[child] && test_open(child, devname, O_NONBLOCK | O_RDONLY) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
preamble_0_2(int child)
{
	if (!test_fd[child] && test_open(child, devname, O_NONBLOCK | O_WRONLY) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_0(int child)
{
	if (test_fd[child] && test_close(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/*
 *  =========================================================================
 *
 *  Preambles and Postambles...
 *
 *  =========================================================================
 */

int
preamble_2(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a hungup dev */
int
preamble_2_1(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_HANGUP, (intptr_t) 0) != __RESULT_SUCCESS && last_errno != ENXIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a dev with rderr */
int
preamble_2_2(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_RDERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a dev with wrerr */
int
preamble_2_3(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a dev with rderr and wrerr */
int
preamble_2_4(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_RWERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a dev with a signal waiting */
int
preamble_2_5(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_PSIGNAL, (intptr_t) SIGHUP) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_2(int child)
{
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS && last_errno != ENXIO && last_errno != EIO)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return result;
}

/* open devname with and push nullmod */
int
preamble_3(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_3(int child)
{
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
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

static const char sref_none[] = "(none)";

/*
 *  Open and Close 1 FIFO.
 */
static const char test_group_1[] = "Open and close a FIFO.";
static const char sref_group_1[] = "POSIX 1003.1 2004/SUSv3 open(2p) reference page, FIFOs.";

#define tgrp_case_1_1 test_group_1
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Open and close 1 FIFO."
#define sref_case_1_1 sref_none
#define desc_case_1_1 "\
Checks that one STREAMS-based FIFO can be opened and closed."

int
test_case_1_1(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
struct test_stream test_1_1 = { NULL, &test_case_1_1, NULL };

#define test_case_1_1_stream_0 (&test_1_1)
#define test_case_1_1_stream_1 (NULL)
#define test_case_1_1_stream_2 (NULL)

/*
 *  Open and Close 3 Streams.
 */
#define tgrp_case_1_2 test_group_1
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Open and close 3 Streams."
#define sref_case_1_2 sref_none
#define desc_case_1_2 "\
Checks that three Streams can be opened and closed."

int
test_case_1_2(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
struct test_stream test_1_2 = { NULL, &test_case_1_2, NULL };

#define test_case_1_2_stream_0 (&test_1_2)
#define test_case_1_2_stream_1 (&test_1_2)
#define test_case_1_2_stream_2 (&test_1_2)

/*
 *  Test open without read or write.  FIFOs must be opened for read,
 *  write or both, otherwise the open will fail.
 */
#define tgrp_case_1_3 test_group_1
#define numb_case_1_3 "1.3"
#define name_case_1_3 "Open a FIFO."
#define sref_case_1_3 sref_group_1
#define desc_case_1_3 "\
Checks that a FIFO can be opened.  Checks that an open() attempt of a\n\
FIFO without read or write flags set will fail and return EINVAL.\n\
(Linux cannot test this because the O_RDONLY flag is zero (and the\n\
default) under Linux.)"

int
test_case_1_3(int child)
{
	return (__RESULT_NOTAPPL);
	if (test_open(child, devname, O_NONBLOCK) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_3 = { NULL, &test_case_1_3, NULL };

#define test_case_1_3_stream_0 (&test_1_3)
#define test_case_1_3_stream_1 (NULL)
#define test_case_1_3_stream_2 (NULL)

#define tgrp_case_1_4 test_group_1
#define numb_case_1_4 "1.4"
#define name_case_1_4 "Open a FIFO."
#define sref_case_1_4 sref_group_1
#define desc_case_1_4 "\
Checks that a FIFO can be opened.  If O_NONBLOCK is set, an open() for\n\
reading only (O_RDONLY) shall return without delay."

int
test_case_1_4(int child)
{
	last_signum = 0;
	start_tt(LONG_WAIT);
	if (test_open(child, devname, O_NONBLOCK | O_RDONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	stop_tt();
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_4 = { NULL, &test_case_1_4, NULL };

#define test_case_1_4_stream_0 (&test_1_4)
#define test_case_1_4_stream_1 (NULL)
#define test_case_1_4_stream_2 (NULL)

#define tgrp_case_1_5 test_group_1
#define numb_case_1_5 "1.5"
#define name_case_1_5 "Open a FIFO."
#define sref_case_1_5 sref_group_1
#define desc_case_1_5 "\
Checks that a FIFO can be opened.  If O_NONBLOCK is set, an open() for\n\
writing only (O_WRONLY) shall return an error (ENXIO) if no process\n\
currently has the file open for writing."

int
test_case_1_5(int child)
{
	last_signum = 0;
	start_tt(LONG_WAIT);
	if (test_open(child, devname, O_NONBLOCK | O_WRONLY) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	stop_tt();
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_5 = { NULL, &test_case_1_5, NULL };

#define test_case_1_5_stream_0 (&test_1_5)
#define test_case_1_5_stream_1 (NULL)
#define test_case_1_5_stream_2 (NULL)

#define tgrp_case_1_6_1 test_group_1
#define numb_case_1_6_1 "1.6.1"
#define name_case_1_6_1 "Open a FIFO - EINTR."
#define sref_case_1_6_1 sref_group_1
#define desc_case_1_6_1 "\
Checks that a FIFO can be opened.  If O_NONBLOCK is clear, an open() for\n\
reading only (O_RDONLY) shall block the calling thread until a thread\n\
opens the file for writing or a signal is received."


int
test_case_1_6_1(int child)
{
	last_signum = 0;
	start_tt(NORMAL_WAIT);
	if (test_open(child, devname, O_RDONLY) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_6_1 = { NULL, &test_case_1_6_1, NULL };

#define test_case_1_6_1_stream_0 (&test_1_6_1)
#define test_case_1_6_1_stream_1 (NULL)
#define test_case_1_6_1_stream_2 (NULL)

#define tgrp_case_1_6_2 test_group_1
#define numb_case_1_6_2 "1.6.2"
#define name_case_1_6_2 "Open a FIFO."
#define sref_case_1_6_2 sref_group_1
#define desc_case_1_6_2 "\
Checks that a FIFO can be opened.  If O_NONBLOCK is clear, an open() for\n\
reading only (O_RDONLY) shall block the calling thread until a thread\n\
opens the file for writing."

int
test_case_1_6_2_rd(int child)
{
	last_signum = 0;
	start_tt(LONG_WAIT + NORMAL_WAIT);
	if (test_open(child, devname, O_RDONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	stop_tt();
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_6_2_rd = { NULL, &test_case_1_6_2_rd, &postamble_0 };
int
test_case_1_6_2_wr(int child)
{
	test_msleep(child, LONG_WAIT);
	if (test_open(child, devname, O_WRONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_6_2_wr = { NULL, &test_case_1_6_2_wr, &postamble_0 };

#define test_case_1_6_2_stream_0 (&test_1_6_2_rd)
#define test_case_1_6_2_stream_1 (&test_1_6_2_wr)
#define test_case_1_6_2_stream_2 (NULL)

#define tgrp_case_1_7_1 test_group_1
#define numb_case_1_7_1 "1.7.1"
#define name_case_1_7_1 "Open a FIFO - EINTR."
#define sref_case_1_7_1 sref_group_1
#define desc_case_1_7_1 "\
Checks that a FIFO can be opened.  If O_NONBLOCK is clear, an open() for\n\
writing only (O_WRONLY) shall block the calling thread until a thread\n\
opens the file for reading or a signal is received."

int
test_case_1_7_1(int child)
{
	last_signum = 0;
	start_tt(NORMAL_WAIT);
	if (test_open(child, devname, O_WRONLY) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_7_1 = { NULL, &test_case_1_7_1, NULL };

#define test_case_1_7_1_stream_0 (&test_1_7_1)
#define test_case_1_7_1_stream_1 (NULL)
#define test_case_1_7_1_stream_2 (NULL)

#define tgrp_case_1_7_2 test_group_1
#define numb_case_1_7_2 "1.7.2"
#define name_case_1_7_2 "Open a FIFO."
#define sref_case_1_7_2 sref_group_1
#define desc_case_1_7_2 "\
Checks that a FIFO can be opened.  If O_NONBLOCK is clear, an open() for\n\
writing only (O_WRONLY) shall block the calling thread until a thread\n\
opens the file for reading."

int
test_case_1_7_2_wr(int child)
{
	last_signum = 0;
	start_tt(LONG_WAIT + NORMAL_WAIT);
	if (test_open(child, devname, O_RDONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	stop_tt();
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_7_2_wr = { NULL, &test_case_1_7_2_wr, NULL };
int
test_case_1_7_2_rd(int child)
{
	test_msleep(child, LONG_WAIT);
	if (test_open(child, devname, O_WRONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_1_7_2_rd = { NULL, &test_case_1_7_2_rd, NULL };

#define test_case_1_7_2_stream_0 (&test_1_7_2_wr)
#define test_case_1_7_2_stream_1 (&test_1_7_2_rd)
#define test_case_1_7_2_stream_2 (NULL)

static const char test_group_2[] = "IOCTL on a FIFO";
static const char sref_group_2[] = "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, FIFOs.";

static const char sref_case_2_1[] = "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_LOOK.";

#define tgrp_case_2_1_1 test_group_2
#define numb_case_2_1_1 "2.1.1"
#define name_case_2_1_1 "I_LOOK ioctl() on a FIFO - EINVAL."
#define sref_case_2_1_1 sref_case_2_1
#define desc_case_2_1_1 "\
Check that I_LOOK can be performed on a FIFO.  Checks that the I_LOOK\n\
command on a FIFO with no modules pushed fails and returns EINVAL."

int
test_case_2_1_1(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_1 = { &preamble_0, &test_case_2_1_1, &postamble_0 };

#define test_case_2_1_1_stream_0 (&test_2_1_1)
#define test_case_2_1_1_stream_1 (NULL)
#define test_case_2_1_1_stream_2 (NULL)


/*  I_SENDFD/I_RECVFD works on FIFOs. */

static const char sref_case_2_2[] = "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_RECVFD.";

#define tgrp_case_2_2_1 test_group_2
#define numb_case_2_2_1 "2.2.1"
#define name_case_2_2_1 "I_RECVFD ioctl() on a FIFO - ENXIO."
#define sref_case_2_2_1 sref_case_2_2
#define desc_case_2_2_1 "\
Check that I_RECVFD can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_RECVFD is attempted on a hung up FIFO with an empty\n\
read queue."

int
test_case_2_2_1(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_1 = { &preamble_2_1, &test_case_2_2_1, &postamble_2 };

#define test_case_2_2_1_stream_0 (&test_2_2_1)
#define test_case_2_2_1_stream_1 (NULL)
#define test_case_2_2_1_stream_2 (NULL)

#define tgrp_case_2_2_2 test_group_2
#define numb_case_2_2_2 "2.2.2"
#define name_case_2_2_2 "I_RECVFD ioctl() on a FIFO - EPROTO."
#define sref_case_2_2_2 sref_case_2_2
#define desc_case_2_2_2 "\
Check that I_RECVFD can be performed on a FIFO.  Checks that EPROTO is\n\
returned when I_RECVFD is attempted on a FIFO that has received an\n\
asychronous EPROTO read error."

int
test_case_2_2_2(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_2 = { &preamble_2_2, &test_case_2_2_2, &postamble_2 };

#define test_case_2_2_2_stream_0 (&test_2_2_2)
#define test_case_2_2_2_stream_1 (NULL)
#define test_case_2_2_2_stream_2 (NULL)

#define tgrp_case_2_2_3 test_group_2
#define numb_case_2_2_3 "2.2.3"
#define name_case_2_2_3 "I_RECVFD ioctl() on a FIFO - EAGAIN."
#define sref_case_2_2_3 sref_case_2_2
#define desc_case_2_2_3 "\
Check that I_RECVFD can be performed on a FIFO.  Checks that EAGAIN is\n\
returned when I_RECVFD is attempted on an empty FIFO that has received\n\
an asyncrhonous EPROTO write error."

int
test_case_2_2_3(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_3 = { &preamble_2_3, &test_case_2_2_3, &postamble_2 };

#define test_case_2_2_3_stream_0 (&test_2_2_3)
#define test_case_2_2_3_stream_1 (NULL)
#define test_case_2_2_3_stream_2 (NULL)

#define tgrp_case_2_2_4 test_group_2
#define numb_case_2_2_4 "2.2.4"
#define name_case_2_2_4 "I_RECVFD ioctl() on a FIFO - EPROTO."
#define sref_case_2_2_4 sref_case_2_2
#define desc_case_2_2_4 "\
Check that I_RECVFD can be performed on a FIFO.  Checks that EPROTO is\n\
returned when I_RECVFD is attempted on an empty FIFO that has received\n\
an asynchronous EPROTO error."

int
test_case_2_2_4(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_4 = { &preamble_2_4, &test_case_2_2_4, &postamble_2 };

#define test_case_2_2_4_stream_0 (&test_2_2_4)
#define test_case_2_2_4_stream_1 (NULL)
#define test_case_2_2_4_stream_2 (NULL)

#define tgrp_case_2_2_5 test_group_2
#define numb_case_2_2_5 "2.2.5"
#define name_case_2_2_5 "I_RECVFD ioctl() on a FIFO."
#define sref_case_2_2_5 sref_case_2_2
#define desc_case_2_2_5 "\
Check that I_RECVFD can be performed on a FIFO."

int
preamble_test_case_2_2_5(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (preamble_0(child + 1) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SENDFD, (intptr_t) 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}
int
test_case_2_2_5(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
postamble_test_case_2_2_5(int child)
{
	int result = __RESULT_SUCCESS;

	if (postamble_0(child + 1) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
}
struct test_stream test_2_2_5 = { &preamble_test_case_2_2_5, &test_case_2_2_5, &postamble_test_case_2_2_5 };

#define test_case_2_2_5_stream_0 (&test_2_2_5)
#define test_case_2_2_5_stream_1 (NULL)
#define test_case_2_2_5_stream_2 (NULL)

#define tgrp_case_2_2_6 test_group_2
#define numb_case_2_2_6 "2.2.6"
#define name_case_2_2_6 "I_RECVFD ioctl() on a FIFO - EINTR."
#define sref_case_2_2_6 sref_case_2_2
#define desc_case_2_2_6 "\
Check that I_RECVFD can be performed on a FIFO.  Checks that when\n\
I_RECVFD is attempted on a FIFO that is set for non-blocking operation,\n\
that I_RECVFD will block awaiting the arrival of an M_PASSFP message and\n\
that the wait can be interrupted by a signal."

int
preamble_test_case_2_2_6(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (start_tt(NORMAL_WAIT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return __RESULT_SUCCESS;
}
int
test_case_2_2_6(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_6 = { &preamble_test_case_2_2_6, &test_case_2_2_6, &postamble_0 };

#define test_case_2_2_6_stream_0 (&test_2_2_6)
#define test_case_2_2_6_stream_1 (NULL)
#define test_case_2_2_6_stream_2 (NULL)

#define tgrp_case_2_2_7 test_group_2
#define numb_case_2_2_7 "2.2.7"
#define name_case_2_2_7 "I_RECVFD ioctl() on a FIFO - EINTR."
#define sref_case_2_2_7 sref_case_2_2
#define desc_case_2_2_7 "\
Check that I_RECVFD can be performed on a FIFO.  Checks that when\n\
I_RECVFD is attempted on a FIFO that is set for non-blocking operation,\n\
that I_RECVFD will block awaiting the arrival of an M_PASSFP message."

int
test_case_2_2_7_rcv(int child)
{
	struct strrecvfd recvfd;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (start_tt(3 * NORMAL_WAIT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	stop_tt();
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_7_rcv = { &preamble_0, &test_case_2_2_7_rcv, &postamble_0 };
int
test_case_2_2_7_snd(int child)
{
	test_msleep(child, NORMAL_WAIT);
	if (test_ioctl(child, I_SENDFD, (intptr_t) 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_7_snd = { &preamble_0, &test_case_2_2_7_snd, &postamble_0 };

#define test_case_2_2_7_stream_0 (&test_2_2_7_rcv)
#define test_case_2_2_7_stream_1 (&test_2_2_7_snd)
#define test_case_2_2_7_stream_2 (NULL)

static const char sref_case_2_3[] = "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_SENDFD.";

#define tgrp_case_2_3_1 test_group_2
#define numb_case_2_3_1 "2.3.1"
#define name_case_2_3_1 "I_SENDFD ioctl() on a FIFO - ENXIO."
#define sref_case_2_3_1 sref_case_2_3
#define desc_case_2_3_1 "\
Check that I_SENDFD can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_SENDFD is attempted on a hung up FIFO."

int
test_case_2_3_1(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) 1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_1 = { &preamble_2_1, &test_case_2_3_1, &postamble_2 };

#define test_case_2_3_1_stream_0 (&test_2_3_1)
#define test_case_2_3_1_stream_1 (NULL)
#define test_case_2_3_1_stream_2 (NULL)

#define tgrp_case_2_3_2 test_group_2
#define numb_case_2_3_2 "2.3.2"
#define name_case_2_3_2 "I_SENDFD ioctl() on a FIFO - EPROTO."
#define sref_case_2_3_2 sref_none
#define desc_case_2_3_2 "\
Check that I_SENDFD can be performed on a FIFO.  Checks that EPROTO is\n\
returned when I_SENDFD is attempted on a FIFO that has received an\n\
asyncrhonous EPROTO read error."

int
test_case_2_3_2(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) 1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_2 = { &preamble_2_2, &test_case_2_3_2, &postamble_2 };

#define test_case_2_3_2_stream_0 (&test_2_3_2)
#define test_case_2_3_2_stream_1 (NULL)
#define test_case_2_3_2_stream_2 (NULL)

#define tgrp_case_2_3_3 test_group_2
#define numb_case_2_3_3 "2.3.3"
#define name_case_2_3_3 "I_SENDFD ioctl() on a FIFO - EPROTO."
#define sref_case_2_3_3 sref_none
#define desc_case_2_3_3 "\
Check that I_SENDFD can be performed on a FIFO.  Checks that EPROTO is\n\
returned when I_SENDFD is attempted on a FIFO that has received an\n\
asynchronous EPROTO write error."

int
test_case_2_3_3(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) 1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_3 = { &preamble_2_3, &test_case_2_3_3, &postamble_2 };

#define test_case_2_3_3_stream_0 (&test_2_3_3)
#define test_case_2_3_3_stream_1 (NULL)
#define test_case_2_3_3_stream_2 (NULL)

#define tgrp_case_2_3_4 test_group_2
#define numb_case_2_3_4 "2.3.4"
#define name_case_2_3_4 "I_SENDFD ioctl() on a FIFO - EPROTO."
#define sref_case_2_3_4 sref_none
#define desc_case_2_3_4 "\
Check that I_SENDFD can be performed on a FIFO.  Checks that EPROTO is\n\
returned when I_SENDFD is attempted on a FIFO that has received an\n\
asyncrhonous EPROTO error."

int
test_case_2_3_4(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) 1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_4 = { &preamble_2_4, &test_case_2_3_4, &postamble_2 };

#define test_case_2_3_4_stream_0 (&test_2_3_4)
#define test_case_2_3_4_stream_1 (NULL)
#define test_case_2_3_4_stream_2 (NULL)

#define tgrp_case_2_4_1 test_group_2
#define numb_case_2_4_1 "2.4.1"
#define name_case_2_4_1 "I_PUSH ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_1 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_PUSH."
#define desc_case_2_4_1 "\
Check that I_PUSH can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_PUSH is attempted on a hung up FIFO."

int
test_case_2_4_1(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_1 = { &preamble_2_1, &test_case_2_4_1, &postamble_2 };

#define test_case_2_4_1_stream_0 (&test_2_4_1)
#define test_case_2_4_1_stream_1 (NULL)
#define test_case_2_4_1_stream_2 (NULL)

#define tgrp_case_2_4_2 test_group_2
#define numb_case_2_4_2 "2.4.2"
#define name_case_2_4_2 "I_POP ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_2 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_POP."
#define desc_case_2_4_2 "\
Check that I_POP can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_POP is attempted on a hung up FIFO."

int
test_case_2_4_2(int child)
{
	if (test_ioctl(child, I_POP, (intptr_t) 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_2 = { &preamble_2_1, &test_case_2_4_2, &postamble_2 };

#define test_case_2_4_2_stream_0 (&test_2_4_2)
#define test_case_2_4_2_stream_1 (NULL)
#define test_case_2_4_2_stream_2 (NULL)

#define tgrp_case_2_4_3 test_group_2
#define numb_case_2_4_3 "2.4.3"
#define name_case_2_4_3 "I_FLUSH ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_3 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_FLUSH."
#define desc_case_2_4_3 "\
Check that I_FLUSH can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_FLUSH is attempted on a hung up FIFO."

int
test_case_2_4_3(int child)
{
	if (test_ioctl(child, I_FLUSH, (intptr_t) FLUSHRW) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_3 = { &preamble_2_1, &test_case_2_4_3, &postamble_2 };

#define test_case_2_4_3_stream_0 (&test_2_4_3)
#define test_case_2_4_3_stream_1 (NULL)
#define test_case_2_4_3_stream_2 (NULL)

#define tgrp_case_2_4_4 test_group_2
#define numb_case_2_4_4 "2.4.4"
#define name_case_2_4_4 "I_STR ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_4 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_STR."
#define desc_case_2_4_4 "\
Check that I_STR can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_STR is attempted on a hung up FIFO."

int
test_case_2_4_4(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 0, .ic_dp = NULL, };

	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_4 = { &preamble_2_1, &test_case_2_4_4, &postamble_2 };

#define test_case_2_4_4_stream_0 (&test_2_4_4)
#define test_case_2_4_4_stream_1 (NULL)
#define test_case_2_4_4_stream_2 (NULL)

#define tgrp_case_2_4_5 test_group_2
#define numb_case_2_4_5 "2.4.5"
#define name_case_2_4_5 "I_FDINSERT ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_5 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_FDINSERT."
#define desc_case_2_4_5 "\
Check that I_FDINSERT can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_FDINSERT is attempted on a hung up FIFO."

#ifdef LIS
typedef ulong t_uscalar_t;
#endif

int
test_case_2_4_5(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;

	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = 0;
	fdi.databuf.buf = NULL;
	fdi.flags = 0;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_5 = { &preamble_2_1, &test_case_2_4_5, &postamble_2 };

#define test_case_2_4_5_stream_0 (&test_2_4_5)
#define test_case_2_4_5_stream_1 (NULL)
#define test_case_2_4_5_stream_2 (NULL)

#define tgrp_case_2_4_6 test_group_2
#define numb_case_2_4_6 "2.4.6"
#define name_case_2_4_6 "I_SWROPT ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_6 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_SWROPT."
#define desc_case_2_4_6 "\
Check that I_SWROPT can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_SWROPT is attempted on a hung up FIFO."

int
test_case_2_4_6(int child)
{
	if (test_ioctl(child, I_SWROPT, SNDPIPE) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_6 = { &preamble_2_1, &test_case_2_4_6, &postamble_2 };

#define test_case_2_4_6_stream_0 (&test_2_4_6)
#define test_case_2_4_6_stream_1 (NULL)
#define test_case_2_4_6_stream_2 (NULL)

#define tgrp_case_2_4_7 test_group_2
#define numb_case_2_4_7 "2.4.7"
#define name_case_2_4_7 "I_FLUSHBAND ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_7 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_FLUSHBAND."
#define desc_case_2_4_7 "\
Check that I_FLUSHBAND can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_FLUSHBAND is attempted on a hung up FIFO."

int
test_case_2_4_7(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_7 = { &preamble_2_1, &test_case_2_4_7, &postamble_2 };

#define test_case_2_4_7_stream_0 (&test_2_4_7)
#define test_case_2_4_7_stream_1 (NULL)
#define test_case_2_4_7_stream_2 (NULL)

#define tgrp_case_2_4_8 test_group_2
#define numb_case_2_4_8 "2.4.8"
#define name_case_2_4_8 "I_CANPUT ioctl() on a FIFO - ENXIO."
#define sref_case_2_4_8 "POSIX 1003.1 2004/SUSv3 ioctl(2p) reference page, I_CANPUT."
#define desc_case_2_4_8 "\
Check that I_CANPUT can be performed on a FIFO.  Checks that ENXIO is\n\
returned when I_CANPUT is attempted on a hung up FIFO."

int
test_case_2_4_8(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_8 = { &preamble_2_1, &test_case_2_4_8, &postamble_2 };

#define test_case_2_4_8_stream_0 (&test_2_4_8)
#define test_case_2_4_8_stream_1 (NULL)
#define test_case_2_4_8_stream_2 (NULL)

static const char test_group_3[] = "System Calls to a FIFO";

static const char sref_case_3_1[] = "POSIX 1003.1 2004/SUSv3 write(2p) reference page, FIFOs.";

/* Write requests of {PIPE_BUF} bytes or less shall not be interleaved
 * with data from other processes doing writes on the same pipe.  Writes
 * of greater than {PIPE_BUF} bytes may have data interleaved, on
 * arbitrary boundaries, with writes by other processes, whether or not
 * the O_NONBLOCK flag of the file status flags is set. */

#define tgrp_case_3_1_1 test_group_3
#define numb_case_3_1_1 "3.1.1"
#define name_case_3_1_1 "write() to a FIFO."
#define sref_case_3_1_1 sref_case_3_1
#define desc_case_3_1_1 "\
Checks that write() can be performed on a FIFO.  Checks that writes of\n\
PIPE_BUF bytes or less are not interleaved despite the setting of\n\
O_NONBLOCK."

int
test_case_3_1_1_w1(int child)
{
	char buf[4096] = { 0, };
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_1_w1 = { &preamble_0, &test_case_3_1_1_w1, &postamble_0 };
int
test_case_3_1_1_w2(int child)
{
	char buf[4096] = { 0, };

	if (test_nonblock(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	do {
		last_errno = 0;
		test_write(child, buf, sizeof(buf));
		state++;
	} while (last_errno == EAGAIN);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_1_w2 = { &preamble_0, &test_case_3_1_1_w2, &postamble_0 };
int
test_case_3_1_1_rd(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int flags = 0;
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 5; i++) {
		if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != 0)
			return (__RESULT_FAILURE);
		state++;
		if (dat.len != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_1_rd = { &preamble_0, &test_case_3_1_1_rd, &postamble_0 };

#define test_case_3_1_1_stream_0 (&test_3_1_1_w1)
#define test_case_3_1_1_stream_1 (&test_3_1_1_w2)
#define test_case_3_1_1_stream_2 (&test_3_1_1_rd)

/* If the O_NONBLOCK flag is clear, a write request may cause the thread
 * to block but on normal completion it shall return nbyte. */

#define tgrp_case_3_1_2 test_group_3
#define numb_case_3_1_2 "3.1.2"
#define name_case_3_1_2 "write() to a FIFO."
#define sref_case_3_1_2 sref_case_3_1
#define desc_case_3_1_2 "\
Checks that write() can be performed on a FIFO.  Checks that when set\n\
for blocking operation, writes block until nbytes are sent."

int
test_case_3_1_2_wr(int child)
{
	char buf[4096<<2] = { 0, };

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != sizeof(buf))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_2_wr = { &preamble_0, &test_case_3_1_2_wr, &postamble_0 };
int
test_case_3_1_2_rd(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int flags = 0;
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != 0)
			return (__RESULT_FAILURE);
		state++;
		if (dat.len != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_2_rd = { &preamble_0, &test_case_3_1_2_rd, &postamble_0 };

#define test_case_3_1_2_stream_0 (&test_3_1_2_wr)
#define test_case_3_1_2_stream_1 (&test_3_1_2_rd)
#define test_case_3_1_2_stream_2 (NULL)

#define tgrp_case_3_1_3 test_group_3
#define numb_case_3_1_3 "3.1.3"
#define name_case_3_1_3 "write() to a FIFO."
#define sref_case_3_1_3 sref_case_3_1
#define desc_case_3_1_3 "\
Checks that write() can be performed on a FIFO.  Checks that when set\n\
for blocking operation, writes block until nbytes are sent or until an\n\
interrupt is received."

int
test_case_3_1_3(int child)
{
	char buf[9000] = { 0, };

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_tt(NORMAL_WAIT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 8192)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_3 = { &preamble_0, &test_case_3_1_3, &postamble_0 };

#define test_case_3_1_3_stream_0 (&test_3_1_3)
#define test_case_3_1_3_stream_1 (NULL)
#define test_case_3_1_3_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, write() requests shall not block the
 * thread. */

#define tgrp_case_3_1_4 test_group_3
#define numb_case_3_1_4 "3.1.4"
#define name_case_3_1_4 "write() to a FIFO."
#define sref_case_3_1_4 sref_case_3_1
#define desc_case_3_1_4 "\
Checks that write() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, write() does not block."

int
test_case_3_1_4(int child)
{
	char buf[9000] = { 0, };

	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 8192)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_4 = { &preamble_0, &test_case_3_1_4, &postamble_0 };

#define test_case_3_1_4_stream_0 (&test_3_1_4)
#define test_case_3_1_4_stream_1 (NULL)
#define test_case_3_1_4_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, a write request for {PIPE_BUF} or
 * fewer bytes shall either transfer all the data and return the number
 * of bytes requested, or shall transfer no data and return [EAGAIN]. */

#define tgrp_case_3_1_5 test_group_3
#define numb_case_3_1_5 "3.1.5"
#define name_case_3_1_5 "write() to a FIFO."
#define sref_case_3_1_5 sref_case_3_1
#define desc_case_3_1_5 "\
Checks that write() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, and less than PIPE_BUF bytes are requested,\n\
either PIPE_BUF bytes will be transferred or error EAGAIN returned."

int
test_case_3_1_5(int child)
{
	char buf[300] = { 0, };

	for (;;) {
		last_errno = 0;
		test_write(child, buf, sizeof(buf));
		state++;
		if (last_errno == EAGAIN)
			return (__RESULT_SUCCESS);
		state++;
		if (last_errno != 0)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
}
struct test_stream test_3_1_5 = { &preamble_0, &test_case_3_1_5, &postamble_0 };

#define test_case_3_1_5_stream_0 (&test_3_1_5)
#define test_case_3_1_5_stream_1 (NULL)
#define test_case_3_1_5_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, a write request for more than
 * {PIPE_BUF} bytes shall either at least transfer {PIPE_BUF} bytes to
 * an empty pipe, otherwise it will transfer what it can and return the
 * number of bytes requested. */

/* If the O_NONBLOCK flag is set, a write request for more than
 * {PIPE_BUF} bytes when no data can be written will fail and return
 * [EAGAIN]. */

/*  FIFOs break writes down into 4096 byte M_DATA messages (atomic constraints).
 *  If a FIFO can write 1 byte it will write up to 4096 bytes.
 *  If a FIFO writes more than 4096 bytes and would block beyond 4096 and is set for non-blocking
 *      operation, it will return 4096, and not write the remainder. */

#define tgrp_case_3_1_6 test_group_3
#define numb_case_3_1_6 "3.1.6"
#define name_case_3_1_6 "write() to a FIFO."
#define sref_case_3_1_6 sref_case_3_1
#define desc_case_3_1_6 "\
Checks that write() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, and more than PIPE_BUF bytes are requested,\n\
either PIPE_BUF or more bytes will be transferred or error EAGAIN\n\
returned."

int
test_case_3_1_6(int child)
{
	char buf[5000] = { 0, };

	for (;;) {
		last_errno = 0;
		test_write(child, buf, sizeof(buf));
		state++;
		if (last_errno == EAGAIN)
			return (__RESULT_SUCCESS);
		state++;
		if (last_errno != 0)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval < 4096)
			return (__RESULT_FAILURE);
		state++;
	}
}
struct test_stream test_3_1_6 = { &preamble_0, &test_case_3_1_6, &postamble_0 };

#define test_case_3_1_6_stream_0 (&test_3_1_6)
#define test_case_3_1_6_stream_1 (NULL)
#define test_case_3_1_6_stream_2 (NULL)

/* Writing a zero-length buffer (nbyte is zero) to a STREAMS-based pipe
 * or FIFO sends no message and 0 is returned.  The process may issue
 * I_SWROPT ioctl() to enable zero-length messages to be sent across the
 * pipe or FIFO.
 *
 *  FIFOs do not send zero-length messages by default (must set SNDZERO).
 */

#define tgrp_case_3_1_7 test_group_3
#define numb_case_3_1_7 "3.1.7"
#define name_case_3_1_7 "write() to a FIFO."
#define sref_case_3_1_7 sref_case_3_1
#define desc_case_3_1_7 "\
Checks that write() can be performed on a FIFO.  Writing a zero-length\n\
buffer (nbyte is zero) to a STREAMS-based pipe or FIFO sends no message\n\
and 0 is returned.  The process may issue I_SWROPT ioctl() to enable\n\
zero-length messages to be sent across the pipe or FIFO."

int
test_case_3_1_7(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, NULL, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SWROPT, (intptr_t) (SNDPIPE | SNDZERO)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, NULL, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_7 = { &preamble_0, &test_case_3_1_7, &postamble_0 };

#define test_case_3_1_7_stream_0 (&test_3_1_7)
#define test_case_3_1_7_stream_1 (NULL)
#define test_case_3_1_7_stream_2 (NULL)

/*  FIFOs return ENXIO on write when hung up.
 *  Also true for streamio ioctls() that check write access. */

#define tgrp_case_3_1_8 test_group_3
#define numb_case_3_1_8 "3.1.8"
#define name_case_3_1_8 "write() to a FIFO - ENXIO."
#define sref_case_3_1_8 sref_case_3_1
#define desc_case_3_1_8 "\
Checks that write() can be performed on a FIFO.  Checks that ENXIO is\n\
returned when attempting to write() to a FIFO that has received a hang\n\
up."

int
test_case_3_1_8(int child)
{
	char buf[16] = { 0, };

	last_signum = 0;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_8 = { &preamble_2_1, &test_case_3_1_8, &postamble_2 };

#define test_case_3_1_8_stream_0 (&test_3_1_8)
#define test_case_3_1_8_stream_1 (NULL)
#define test_case_3_1_8_stream_2 (NULL)

/*  FIFOs send SIGPIPE on write errors by default (must clear SNDPIPE).
 *  Also true for streamio ioctls() that check write access. */

#define tgrp_case_3_1_9 test_group_3
#define numb_case_3_1_9 "3.1.9"
#define name_case_3_1_9 "write() to a FIFO - EPROTO."
#define sref_case_3_1_9 sref_case_3_1
#define desc_case_3_1_9 "\
Checks that write() can be performed on a FIFO.  Checks that EPROTO is\n\
returned and SIGPIPE generated when attempting to write() to a FIFO that\n\
has received an asynchronous EPROTO write error."

int
test_case_3_1_9(int child)
{
	char buf[16] = { 0, };

	last_signum = 0;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGPIPE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_9 = { &preamble_2_3, &test_case_3_1_9, &postamble_2 };

#define test_case_3_1_9_stream_0 (&test_3_1_9)
#define test_case_3_1_9_stream_1 (NULL)
#define test_case_3_1_9_stream_2 (NULL)

#define tgrp_case_3_1_10 test_group_3
#define numb_case_3_1_10 "3.1.10"
#define name_case_3_1_10 "write() to a FIFO - EPROTO."
#define sref_case_3_1_10 sref_case_3_1
#define desc_case_3_1_10 "\
Checks that write() can be performed on a FIFO.  Checks that EPROTO is\n\
returned when attempting to write() to a FIFO that has received an\n\
asynchronous EPROTO error.  Checks that the SIGPIPE signal can be\n\
suppressed with I_SWROPT."

int
preamble_test_case_3_1_10(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_SWROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}
int
test_case_3_1_10(int child)
{
	char buf[16] = { 0, };

	last_signum = 0;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_10 = { &preamble_test_case_3_1_10, &test_case_3_1_10, &postamble_2 };

#define test_case_3_1_10_stream_0 (&test_3_1_10)
#define test_case_3_1_10_stream_1 (NULL)
#define test_case_3_1_10_stream_2 (NULL)

/*  FIFOs return EPIPE and generate SIGPIPE when write and there are no readers.
 *  Also true for streamio ioctls() that check write access. */

#define tgrp_case_3_1_11 test_group_3
#define numb_case_3_1_11 "3.1.11"
#define name_case_3_1_11 "write() to a FIFO - EPIPE."
#define sref_case_3_1_11 sref_case_3_1
#define desc_case_3_1_11 "\
Checks that write() can be performed on a FIFO.  Checks that EPIPE is\n\
returned and SIGPIPE generated when a write() attempt is make on a FIFO\n\
that has no readers."

int
preamble_test_case_3_1_11_wr(int child)
{
	if (test_open(child, devname, O_WRONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	/* wait for reader to close */
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_11_wr(int child)
{
	char buf[16] = { 0, };

	last_signum = 0;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPIPE)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGPIPE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_11_wr = { &preamble_test_case_3_1_11_wr, &test_case_3_1_11_wr, &postamble_0 };

int
preamble_test_case_3_1_11_rd(int child)
{
	if (test_open(child, devname, O_RDONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_11_rd(int child)
{
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_11_rd = { &preamble_test_case_3_1_11_rd, &test_case_3_1_11_rd, &postamble_0 };

#define test_case_3_1_11_stream_0 (&test_3_1_11_wr)
#define test_case_3_1_11_stream_1 (&test_3_1_11_rd)
#define test_case_3_1_11_stream_2 (NULL)

static const char sref_case_3_2[] = "POSIX 1003.1 2004/SUSv3 putmsg(2p) reference page, FIFOs.";

/* Write requests of {PIPE_BUF} bytes or less shall not be interleaved
 * with data from other processes doing writes on the same pipe.  Writes
 * of greater than {PIPE_BUF} bytes may have data interleaved, on
 * arbitrary boundaries, with writes by other processes, whether or not
 * the O_NONBLOCK flag of the file status flags is set. */

#define tgrp_case_3_2_1 test_group_3
#define numb_case_3_2_1 "3.2.1"
#define name_case_3_2_1 "putmsg() to a FIFO."
#define sref_case_3_2_1 sref_case_3_2
#define desc_case_3_2_1 "\
Checks that putmsg() can be performed on a FIFO.  Checks that writes of\n\
PIPE_BUF bytes or less are not interleaved despite the setting of\n\
O_NONBLOCK."

int
test_case_3_2_1_w1(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_putmsg(child, NULL, &dat, 0) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_1_w1 = { &preamble_0, &test_case_3_2_1_w1, &postamble_0 };
int
test_case_3_2_1_w2(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	if (test_nonblock(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	do {
		last_errno = 0;
		test_putmsg(child, NULL, &dat, 0);
		state++;
	} while (last_errno == EAGAIN);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_1_w2 = { &preamble_0, &test_case_3_2_1_w2, &postamble_0 };
int
test_case_3_2_1_rd(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int flags = 0;
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 5; i++) {
		if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != 0)
			return (__RESULT_FAILURE);
		state++;
		if (dat.len != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_1_rd = { &preamble_0, &test_case_3_2_1_rd, &postamble_0 };

#define test_case_3_2_1_stream_0 (&test_3_2_1_w1)
#define test_case_3_2_1_stream_1 (&test_3_2_1_w2)
#define test_case_3_2_1_stream_2 (&test_3_2_1_rd)

/* If the O_NONBLOCK flag is clear, a putmsg request may cause the thread
 * to block but on normal completion it shall return nbyte. */

#define tgrp_case_3_2_2 test_group_3
#define numb_case_3_2_2 "3.2.2"
#define name_case_3_2_2 "putmsg() to a FIFO."
#define sref_case_3_2_2 sref_case_3_2
#define desc_case_3_2_2 "\
Checks that putmsg() can be performed on a FIFO.  Checks that when set\n\
for blocking operation, putmsg() blocks until the number of bytes\n\
requested are sent."

int
test_case_3_2_2_wr(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_putmsg(child, NULL, &dat, 0) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_2_wr = { &preamble_0, &test_case_3_2_2_wr, &postamble_0 };
int
test_case_3_2_2_rd(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int flags = 0;
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != 0)
			return (__RESULT_FAILURE);
		state++;
		if (dat.len != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_2_rd = { &preamble_0, &test_case_3_2_2_rd, &postamble_0 };

#define test_case_3_2_2_stream_0 (&test_3_2_2_wr)
#define test_case_3_2_2_stream_1 (&test_3_2_2_rd)
#define test_case_3_2_2_stream_2 (NULL)

#define tgrp_case_3_2_3 test_group_3
#define numb_case_3_2_3 "3.2.3"
#define name_case_3_2_3 "putmsg() to a FIFO."
#define sref_case_3_2_3 sref_case_3_2
#define desc_case_3_2_3 "\
Checks that putmsg() can be performed on a FIFO.  Checks that when set\n\
for blocking operation, putmsg() blocks until the number of bytes\n\
requested are sent or until an interrupt is received."

int
test_case_3_2_3(int child)
{
	char buf[9000] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_tt(NORMAL_WAIT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_3 = { &preamble_0, &test_case_3_2_3, &postamble_0 };

#define test_case_3_2_3_stream_0 (&test_3_2_3)
#define test_case_3_2_3_stream_1 (NULL)
#define test_case_3_2_3_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, putmsg() requests shall not block the
 * thread. */

#define tgrp_case_3_2_4 test_group_3
#define numb_case_3_2_4 "3.2.4"
#define name_case_3_2_4 "putmsg() to a FIFO."
#define sref_case_3_2_4 sref_case_3_2
#define desc_case_3_2_4 "\
Checks that putmsg() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, putmsg() does not block."

int
test_case_3_2_4(int child)
{
	char buf[9000] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	if (test_putmsg(child, NULL, &dat, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_4 = { &preamble_0, &test_case_3_2_4, &postamble_0 };

#define test_case_3_2_4_stream_0 (&test_3_2_4)
#define test_case_3_2_4_stream_1 (NULL)
#define test_case_3_2_4_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, a putmsg request for {PIPE_BUF} or
 * fewer bytes shall either transfer all the data and return the number
 * of bytes requested, or shall transfer no data and return [EAGAIN]. */

#define tgrp_case_3_2_5 test_group_3
#define numb_case_3_2_5 "3.2.5"
#define name_case_3_2_5 "putmsg() to a FIFO."
#define sref_case_3_2_5 sref_case_3_2
#define desc_case_3_2_5 "\
Checks that putmsg() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, and less than PIPE_BUF bytes are requested,\n\
either PIPE_BUF bytes will be transferred or error EAGAIN returned."

int
test_case_3_2_5(int child)
{
	char buf[300] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	for (;;) {
		last_errno = 0;
		test_putmsg(child, NULL, &dat, 0);
		state++;
		if (last_errno == EAGAIN)
			return (__RESULT_SUCCESS);
		state++;
		if (last_errno != 0)
			return (__RESULT_FAILURE);
		state++;
	}
}
struct test_stream test_3_2_5 = { &preamble_0, &test_case_3_2_5, &postamble_0 };

#define test_case_3_2_5_stream_0 (&test_3_2_5)
#define test_case_3_2_5_stream_1 (NULL)
#define test_case_3_2_5_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, a putmsg request for more than
 * {PIPE_BUF} bytes shall either at least transfer {PIPE_BUF} bytes to
 * an empty pipe, otherwise it will transfer what it can and return the
 * number of bytes requested. */

/* If the O_NONBLOCK flag is set, a putmsg request for more than
 * {PIPE_BUF} bytes when no data can be written will fail and return
 * [EAGAIN]. */

/*  FIFOs break writes down into 4096 byte M_DATA messages (atomic constraints).
 *  If a FIFO can putmsg 1 byte it will putmsg up to 4096 bytes.
 *  If a FIFO writes more than 4096 bytes and would block beyond 4096 and is set for non-blocking
 *      operation, it will return 4096, and not putmsg the remainder. */

#define tgrp_case_3_2_6 test_group_3
#define numb_case_3_2_6 "3.2.6"
#define name_case_3_2_6 "putmsg() to a FIFO."
#define sref_case_3_2_6 sref_case_3_2
#define desc_case_3_2_6 "\
Checks that putmsg() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, and more than PIPE_BUF bytes are requested,\n\
either PIPE_BUF or more bytes will be transferred or error EAGAIN\n\
returned."

int
test_case_3_2_6(int child)
{
	char buf[5000] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	for (;;) {
		last_errno = 0;
		test_putmsg(child, NULL, &dat, 0);
		state++;
		if (last_errno == EAGAIN)
			return (__RESULT_SUCCESS);
		state++;
		if (last_errno != 0)
			return (__RESULT_FAILURE);
		state++;
	}
}
struct test_stream test_3_2_6 = { &preamble_0, &test_case_3_2_6, &postamble_0 };

#define test_case_3_2_6_stream_0 (&test_3_2_6)
#define test_case_3_2_6_stream_1 (NULL)
#define test_case_3_2_6_stream_2 (NULL)

/* Writing a zero-length buffer (nbyte is zero) to a STREAMS-based pipe
 * or FIFO sends no message and 0 is returned.  The process may issue
 * I_SWROPT ioctl() to enable zero-length messages to be sent across the
 * pipe or FIFO.
 *
 *  FIFOs do not send zero-length messages by default (must set SNDZERO).
 */

#define tgrp_case_3_2_7 test_group_3
#define numb_case_3_2_7 "3.2.7"
#define name_case_3_2_7 "putmsg() to a FIFO."
#define sref_case_3_2_7 sref_case_3_2
#define desc_case_3_2_7 "\
Checks that putmsg() can be performed on a FIFO.  Writing a zero-length\n\
buffer (nbyte is zero) to a STREAMS-based pipe or FIFO sends no message\n\
and 0 is returned.  The process may issue I_SWROPT ioctl() to enable\n\
zero-length messages to be sent across the pipe or FIFO."

int
test_case_3_2_7(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, 0, NULL };

	if (test_putmsg(child, NULL, &dat, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SWROPT, (intptr_t) (SNDPIPE | SNDZERO)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_7 = { &preamble_0, &test_case_3_2_7, &postamble_0 };

#define test_case_3_2_7_stream_0 (&test_3_2_7)
#define test_case_3_2_7_stream_1 (NULL)
#define test_case_3_2_7_stream_2 (NULL)

/*  FIFOs return ENXIO on putmsg when hung up.
 *  Also true for streamio ioctls() that check putmsg access. */

#define tgrp_case_3_2_8 test_group_3
#define numb_case_3_2_8 "3.2.8"
#define name_case_3_2_8 "putmsg() to a FIFO - ENXIO."
#define sref_case_3_2_8 sref_case_3_2
#define desc_case_3_2_8 "\
Checks that putmsg() can be performed on a FIFO.  Checks that ENXIO is\n\
returned when attempting to putmsg() to a FIFO that has received a hang\n\
up."

int
test_case_3_2_8(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putmsg(child, NULL, &dat, 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_8 = { &preamble_2_1, &test_case_3_2_8, &postamble_2 };

#define test_case_3_2_8_stream_0 (&test_3_2_8)
#define test_case_3_2_8_stream_1 (NULL)
#define test_case_3_2_8_stream_2 (NULL)

/*  FIFOs send SIGPIPE on putmsg errors by default (must clear SNDPIPE).
 *  Also true for streamio ioctls() that check putmsg access. */

#define tgrp_case_3_2_9 test_group_3
#define numb_case_3_2_9 "3.2.9"
#define name_case_3_2_9 "putmsg() to a FIFO - EPROTO."
#define sref_case_3_2_9 sref_case_3_2
#define desc_case_3_2_9 "\
Checks that putmsg() can be performed on a FIFO.  Checks that EPROTO is\n\
returned and SIGPIPE generated when attempting to putmsg() to a FIFO that\n\
has received an asynchronous EPROTO putmsg error."

int
test_case_3_2_9(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putmsg(child, NULL, &dat, 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGPIPE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_9 = { &preamble_2_3, &test_case_3_2_9, &postamble_2 };

#define test_case_3_2_9_stream_0 (&test_3_2_9)
#define test_case_3_2_9_stream_1 (NULL)
#define test_case_3_2_9_stream_2 (NULL)

#define tgrp_case_3_2_10 test_group_3
#define numb_case_3_2_10 "3.2.10"
#define name_case_3_2_10 "putmsg() to a FIFO - EPROTO."
#define sref_case_3_2_10 sref_case_3_2
#define desc_case_3_2_10 "\
Checks that putmsg() can be performed on a FIFO.  Checks that EPROTO is\n\
returned when attempting to putmsg() to a FIFO that has received an\n\
asynchronous EPROTO error.  Checks that the SIGPIPE signal can be\n\
suppressed with I_SWROPT."

int
preamble_test_case_3_2_10(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_SWROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}
int
test_case_3_2_10(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putmsg(child, NULL, &dat, 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_10 = { &preamble_test_case_3_2_10, &test_case_3_2_10, &postamble_2 };

#define test_case_3_2_10_stream_0 (&test_3_2_10)
#define test_case_3_2_10_stream_1 (NULL)
#define test_case_3_2_10_stream_2 (NULL)

/*  FIFOs return EPIPE and generate SIGPIPE when putmsg and there are no readers.
 *  Also true for streamio ioctls() that check putmsg access. */

#define tgrp_case_3_2_11 test_group_3
#define numb_case_3_2_11 "3.2.11"
#define name_case_3_2_11 "putmsg() to a FIFO - EPIPE."
#define sref_case_3_2_11 sref_case_3_2
#define desc_case_3_2_11 "\
Checks that putmsg() can be performed on a FIFO.  Checks that EPIPE is\n\
returned and SIGPIPE generated when a putmsg() attempt is make on a FIFO\n\
that has no readers."

int
preamble_test_case_3_2_11_wr(int child)
{
	if (test_open(child, devname, O_WRONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	/* wait for reader to close */
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_2_11_wr(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putmsg(child, NULL, &dat, 0) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPIPE)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGPIPE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_11_wr = { &preamble_test_case_3_2_11_wr, &test_case_3_2_11_wr, &postamble_0 };

int
preamble_test_case_3_2_11_rd(int child)
{
	if (test_open(child, devname, O_RDONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_2_11_rd(int child)
{
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_11_rd = { &preamble_test_case_3_2_11_rd, &test_case_3_2_11_rd, &postamble_0 };

#define test_case_3_2_11_stream_0 (&test_3_2_11_wr)
#define test_case_3_2_11_stream_1 (&test_3_2_11_rd)
#define test_case_3_2_11_stream_2 (NULL)

static const char sref_case_3_3[] = "POSIX 1003.1 2004/SUSv3 putpmsg(2p) reference page, FIFOs.";

/* Write requests of {PIPE_BUF} bytes or less shall not be interleaved
 * with data from other processes doing writes on the same pipe.  Writes
 * of greater than {PIPE_BUF} bytes may have data interleaved, on
 * arbitrary boundaries, with writes by other processes, whether or not
 * the O_NONBLOCK flag of the file status flags is set. */

#define tgrp_case_3_3_1 test_group_3
#define numb_case_3_3_1 "3.3.1"
#define name_case_3_3_1 "putpmsg() to a FIFO."
#define sref_case_3_3_1 sref_case_3_3
#define desc_case_3_3_1 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that writes of\n\
PIPE_BUF bytes or less are not interleaved despite the setting of\n\
O_NONBLOCK."

int
test_case_3_3_1_w1(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
	}
	/* wait for reader to close */
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_1_w1 = { &preamble_0, &test_case_3_3_1_w1, &postamble_0 };
int
test_case_3_3_1_w2(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	if (test_nonblock(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	do {
		last_errno = 0;
		test_putpmsg(child, NULL, &dat, 0, MSG_BAND);
		state++;
	} while (last_errno == EAGAIN);
	state++;
	/* wait for reader to close */
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_1_w2 = { &preamble_0, &test_case_3_3_1_w2, &postamble_0 };
int
test_case_3_3_1_rd(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int flags = 0;
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 5; i++) {
		if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != 0)
			return (__RESULT_FAILURE);
		state++;
		if (dat.len != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_1_rd = { &preamble_0, &test_case_3_3_1_rd, &postamble_0 };

#define test_case_3_3_1_stream_0 (&test_3_3_1_w1)
#define test_case_3_3_1_stream_1 (&test_3_3_1_w2)
#define test_case_3_3_1_stream_2 (&test_3_3_1_rd)

/* If the O_NONBLOCK flag is clear, a putpmsg request may cause the thread
 * to block but on normal completion it shall return nbyte. */

#define tgrp_case_3_3_2 test_group_3
#define numb_case_3_3_2 "3.3.2"
#define name_case_3_3_2 "putpmsg() to a FIFO."
#define sref_case_3_3_2 sref_case_3_3
#define desc_case_3_3_2 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that when set\n\
for blocking operation, putpmsg() blocks until the number of bytes\n\
requested are sent."

int
test_case_3_3_2_wr(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_2_wr = { &preamble_0, &test_case_3_3_2_wr, &postamble_0 };
int
test_case_3_3_2_rd(int child)
{
	char buf[4096] = { 0, };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int flags = 0;
	int i;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4; i++) {
		if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval != 0)
			return (__RESULT_FAILURE);
		state++;
		if (dat.len != sizeof(buf))
			return (__RESULT_FAILURE);
		state++;
	}
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_2_rd = { &preamble_0, &test_case_3_3_2_rd, &postamble_0 };

#define test_case_3_3_2_stream_0 (&test_3_3_2_wr)
#define test_case_3_3_2_stream_1 (&test_3_3_2_rd)
#define test_case_3_3_2_stream_2 (NULL)

#define tgrp_case_3_3_3 test_group_3
#define numb_case_3_3_3 "3.3.3"
#define name_case_3_3_3 "putpmsg() to a FIFO."
#define sref_case_3_3_3 sref_case_3_3
#define desc_case_3_3_3 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that when set\n\
for blocking operation, putpmsg() blocks until the number of bytes\n\
requested are sent or until an interrupt is received."

int
test_case_3_3_3(int child)
{
	char buf[9000] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_tt(NORMAL_WAIT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_3 = { &preamble_0, &test_case_3_3_3, &postamble_0 };

#define test_case_3_3_3_stream_0 (&test_3_3_3)
#define test_case_3_3_3_stream_1 (NULL)
#define test_case_3_3_3_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, putpmsg() requests shall not block the
 * thread. */

#define tgrp_case_3_3_4 test_group_3
#define numb_case_3_3_4 "3.3.4"
#define name_case_3_3_4 "putpmsg() to a FIFO."
#define sref_case_3_3_4 sref_case_3_3
#define desc_case_3_3_4 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, putpmsg() does not block."

int
test_case_3_3_4(int child)
{
	char buf[9000] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_4 = { &preamble_0, &test_case_3_3_4, &postamble_0 };

#define test_case_3_3_4_stream_0 (&test_3_3_4)
#define test_case_3_3_4_stream_1 (NULL)
#define test_case_3_3_4_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, a putpmsg request for {PIPE_BUF} or
 * fewer bytes shall either transfer all the data and return the number
 * of bytes requested, or shall transfer no data and return [EAGAIN]. */

#define tgrp_case_3_3_5 test_group_3
#define numb_case_3_3_5 "3.3.5"
#define name_case_3_3_5 "putpmsg() to a FIFO."
#define sref_case_3_3_5 sref_case_3_3
#define desc_case_3_3_5 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, and less than PIPE_BUF bytes are requested,\n\
either PIPE_BUF bytes will be transferred or error EAGAIN returned."

int
test_case_3_3_5(int child)
{
	char buf[300] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	for (;;) {
		last_errno = 0;
		test_putpmsg(child, NULL, &dat, 0, MSG_BAND);
		state++;
		if (last_errno == EAGAIN)
			return (__RESULT_SUCCESS);
		state++;
		if (last_errno != 0)
			return (__RESULT_FAILURE);
		state++;
	}
}
struct test_stream test_3_3_5 = { &preamble_0, &test_case_3_3_5, &postamble_0 };

#define test_case_3_3_5_stream_0 (&test_3_3_5)
#define test_case_3_3_5_stream_1 (NULL)
#define test_case_3_3_5_stream_2 (NULL)

/* If the O_NONBLOCK flag is set, a putpmsg request for more than
 * {PIPE_BUF} bytes shall either at least transfer {PIPE_BUF} bytes to
 * an empty pipe, otherwise it will transfer what it can and return the
 * number of bytes requested. */

/* If the O_NONBLOCK flag is set, a putpmsg request for more than
 * {PIPE_BUF} bytes when no data can be written will fail and return
 * [EAGAIN]. */

/*  FIFOs break writes down into 4096 byte M_DATA messages (atomic constraints).
 *  If a FIFO can putpmsg 1 byte it will putpmsg up to 4096 bytes.
 *  If a FIFO writes more than 4096 bytes and would block beyond 4096 and is set for non-blocking
 *      operation, it will return 4096, and not putpmsg the remainder. */

#define tgrp_case_3_3_6 test_group_3
#define numb_case_3_3_6 "3.3.6"
#define name_case_3_3_6 "putpmsg() to a FIFO."
#define sref_case_3_3_6 sref_case_3_3
#define desc_case_3_3_6 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that when set\n\
for non-blocking operation, and more than PIPE_BUF bytes are requested,\n\
either PIPE_BUF or more bytes will be transferred or error EAGAIN\n\
returned."

int
test_case_3_3_6(int child)
{
	char buf[5000] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	for (;;) {
		last_errno = 0;
		test_putpmsg(child, NULL, &dat, 0, MSG_BAND);
		state++;
		if (last_errno == EAGAIN)
			return (__RESULT_SUCCESS);
		state++;
		if (last_errno != 0)
			return (__RESULT_FAILURE);
		state++;
	}
}
struct test_stream test_3_3_6 = { &preamble_0, &test_case_3_3_6, &postamble_0 };

#define test_case_3_3_6_stream_0 (&test_3_3_6)
#define test_case_3_3_6_stream_1 (NULL)
#define test_case_3_3_6_stream_2 (NULL)

/* Writing a zero-length buffer (nbyte is zero) to a STREAMS-based pipe
 * or FIFO sends no message and 0 is returned.  The process may issue
 * I_SWROPT ioctl() to enable zero-length messages to be sent across the
 * pipe or FIFO.
 *
 *  FIFOs do not send zero-length messages by default (must set SNDZERO).
 */

#define tgrp_case_3_3_7 test_group_3
#define numb_case_3_3_7 "3.3.7"
#define name_case_3_3_7 "putpmsg() to a FIFO."
#define sref_case_3_3_7 sref_case_3_3
#define desc_case_3_3_7 "\
Checks that putpmsg() can be performed on a FIFO.  Writing a zero-length\n\
buffer (nbyte is zero) to a STREAMS-based pipe or FIFO sends no message\n\
and 0 is returned.  The process may issue I_SWROPT ioctl() to enable\n\
zero-length messages to be sent across the pipe or FIFO."

int
test_case_3_3_7(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, 0, NULL };

	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SWROPT, (intptr_t) (SNDPIPE | SNDZERO)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_7 = { &preamble_0, &test_case_3_3_7, &postamble_0 };

#define test_case_3_3_7_stream_0 (&test_3_3_7)
#define test_case_3_3_7_stream_1 (NULL)
#define test_case_3_3_7_stream_2 (NULL)

/*  FIFOs return ENXIO on putpmsg when hung up.
 *  Also true for streamio ioctls() that check putpmsg access. */

#define tgrp_case_3_3_8 test_group_3
#define numb_case_3_3_8 "3.3.8"
#define name_case_3_3_8 "putpmsg() to a FIFO - ENXIO."
#define sref_case_3_3_8 sref_case_3_3
#define desc_case_3_3_8 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that ENXIO is\n\
returned when attempting to putpmsg() to a FIFO that has received a hang\n\
up."

int
test_case_3_3_8(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_8 = { &preamble_2_1, &test_case_3_3_8, &postamble_2 };

#define test_case_3_3_8_stream_0 (&test_3_3_8)
#define test_case_3_3_8_stream_1 (NULL)
#define test_case_3_3_8_stream_2 (NULL)

/*  FIFOs send SIGPIPE on putpmsg errors by default (must clear SNDPIPE).
 *  Also true for streamio ioctls() that check putpmsg access. */

#define tgrp_case_3_3_9 test_group_3
#define numb_case_3_3_9 "3.3.9"
#define name_case_3_3_9 "putpmsg() to a FIFO - EPROTO."
#define sref_case_3_3_9 sref_case_3_3
#define desc_case_3_3_9 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that EPROTO is\n\
returned and SIGPIPE generated when attempting to putpmsg() to a FIFO that\n\
has received an asynchronous EPROTO putpmsg error."

int
test_case_3_3_9(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGPIPE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_9 = { &preamble_2_3, &test_case_3_3_9, &postamble_2 };

#define test_case_3_3_9_stream_0 (&test_3_3_9)
#define test_case_3_3_9_stream_1 (NULL)
#define test_case_3_3_9_stream_2 (NULL)

#define tgrp_case_3_3_10 test_group_3
#define numb_case_3_3_10 "3.3.10"
#define name_case_3_3_10 "putpmsg() to a FIFO - EPROTO."
#define sref_case_3_3_10 sref_case_3_3
#define desc_case_3_3_10 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that EPROTO is\n\
returned when attempting to putpmsg() to a FIFO that has received an\n\
asynchronous EPROTO error.  Checks that the SIGPIPE signal can be\n\
suppressed with I_SWROPT."

int
preamble_test_case_3_3_10(int child)
{
	if (preamble_2(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_SWROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}
int
test_case_3_3_10(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_10 = { &preamble_test_case_3_3_10, &test_case_3_3_10, &postamble_2 };

#define test_case_3_3_10_stream_0 (&test_3_3_10)
#define test_case_3_3_10_stream_1 (NULL)
#define test_case_3_3_10_stream_2 (NULL)

/*  FIFOs return EPIPE and generate SIGPIPE when putpmsg and there are no readers.
 *  Also true for streamio ioctls() that check putpmsg access. */

#define tgrp_case_3_3_11 test_group_3
#define numb_case_3_3_11 "3.3.11"
#define name_case_3_3_11 "putpmsg() to a FIFO - EPIPE."
#define sref_case_3_3_11 sref_case_3_3
#define desc_case_3_3_11 "\
Checks that putpmsg() can be performed on a FIFO.  Checks that EPIPE is\n\
returned and SIGPIPE generated when a putpmsg() attempt is make on a FIFO\n\
that has no readers."

int
preamble_test_case_3_3_11_wr(int child)
{
	if (test_open(child, devname, O_WRONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	/* wait for reader to close */
	test_msleep(child, NORMAL_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_3_11_wr(int child)
{
	char buf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(buf), buf };

	last_signum = 0;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPIPE)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		print_signal(child, last_signum);
	state++;
	if (last_signum != SIGPIPE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_11_wr = { &preamble_test_case_3_3_11_wr, &test_case_3_3_11_wr, &postamble_0 };

int
preamble_test_case_3_3_11_rd(int child)
{
	if (test_open(child, devname, O_RDONLY) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_3_11_rd(int child)
{
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_11_rd = { &preamble_test_case_3_3_11_rd, &test_case_3_3_11_rd, &postamble_0 };

#define test_case_3_3_11_stream_0 (&test_3_3_11_wr)
#define test_case_3_3_11_stream_1 (&test_3_3_11_rd)
#define test_case_3_3_11_stream_2 (NULL)

/*
 *  Some things to test for FIFOs.
 *
 *  FIFOs cannot be clone opened.
 *  I_LINK on a FIFO returns EINVAL.
 *  I_PLINK on a FIFO returns EINVAL.
 *  I_UNLINK on a FIFO returns EINVAL.
 *  I_PUNLINK on a FIFO returns EINVAL.
 *  I_LIST returns zero (0): no driver.
 *  FIFOs (even when full) will not block on close (no driver).
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Test case child scheduler
 *
 *  -------------------------------------------------------------------------
 */
int
run_stream(int child, struct test_stream *stream)
{
	int result = __RESULT_SCRIPT_ERROR;
	int pre_result = __RESULT_SCRIPT_ERROR;
	int post_result = __RESULT_SCRIPT_ERROR;

	print_preamble(child);
	state = 100;
	if (stream->preamble && (pre_result = stream->preamble(child)) != __RESULT_SUCCESS) {
		switch (pre_result) {
		case __RESULT_NOTAPPL:
			print_notapplicable(child);
			result = __RESULT_NOTAPPL;
			break;
		case __RESULT_SKIPPED:
			print_skipped(child);
			result = __RESULT_SKIPPED;
			break;
		default:
			print_inconclusive(child);
			result = __RESULT_INCONCLUSIVE;
			break;
		}
	} else {
		print_test(child);
		state = 200;
		switch (stream->testcase(child)) {
		default:
		case __RESULT_INCONCLUSIVE:
			print_inconclusive(child);
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_NOTAPPL:
			print_notapplicable(child);
			result = __RESULT_NOTAPPL;
			break;
		case __RESULT_SKIPPED:
			print_skipped(child);
			result = __RESULT_SKIPPED;
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
		if (stream->postamble && (post_result = stream->postamble(child)) != __RESULT_SUCCESS) {
			switch (post_result) {
			case __RESULT_NOTAPPL:
				print_notapplicable(child);
				result = __RESULT_NOTAPPL;
				break;
			case __RESULT_SKIPPED:
				print_skipped(child);
				result = __RESULT_SKIPPED;
				break;
			default:
				print_inconclusive(child);
				if (result == __RESULT_SUCCESS)
					result = __RESULT_INCONCLUSIVE;
				break;
			}
		}
	}
	print_test_end(child);
	exit(result);
}

/*
 *  Fork multiple children to do the actual testing.
 */

int
test_run(struct test_stream *stream[])
{
	int children = 0;
	pid_t this_pid;
	int this_status, status[3] = { 0, };

	if (start_tt(TEST_DURATION) != __RESULT_SUCCESS)
		goto inconclusive;
	if (stream[2]) {
		switch ((test_pid[2] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(2, stream[2]));	/* execute stream[2] state machine */
		case -1:	/* error */
			if (test_pid[0])
				kill(test_pid[0], SIGKILL);	/* toast stream[0] child */
			if (test_pid[1])
				kill(test_pid[1], SIGKILL);	/* toast stream[1] child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			// printf("Child 2 pid is %d\n", (int)test_pid[2]);
			break;
		}
	} else
		status[2] = __RESULT_SUCCESS;
	if (stream[1]) {
		switch ((test_pid[1] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(1, stream[1]));	/* execute stream[1] state machine */
		case -1:	/* error */
			if (test_pid[0])
				kill(test_pid[0], SIGKILL);	/* toast stream[0] child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			// printf("Child 1 pid is %d\n", (int)test_pid[1]);
			break;
		}
	} else
		status[1] = __RESULT_SUCCESS;
	if (stream[0]) {
		switch ((test_pid[0] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(0, stream[0]));	/* execute stream[0] state machine */
		case -1:	/* error */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			// printf("Child 0 pid is %d\n", (int)test_pid[0]);
			break;
		}
	} else
		status[0] = __RESULT_SUCCESS;
	for (; children > 0; children--) {
	      waitagain:
		if ((this_pid = wait(&this_status)) > 0) {
			if (WIFEXITED(this_status)) {
				if (this_pid == test_pid[0]) {
					test_pid[0] = 0;
					if ((status[0] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (test_pid[1])
							kill(test_pid[1], SIGKILL);
						if (test_pid[2])
							kill(test_pid[2], SIGKILL);
					}
				}
				if (this_pid == test_pid[1]) {
					test_pid[1] = 0;
					if ((status[1] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (test_pid[0])
							kill(test_pid[0], SIGKILL);
						if (test_pid[2])
							kill(test_pid[2], SIGKILL);
					}
				}
				if (this_pid == test_pid[2]) {
					test_pid[2] = 0;
					if ((status[2] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (test_pid[0])
							kill(test_pid[0], SIGKILL);
						if (test_pid[1])
							kill(test_pid[1], SIGKILL);
					}
				}
			} else if (WIFSIGNALED(this_status)) {
				int signal = WTERMSIG(this_status);

				if (this_pid == test_pid[0]) {
					print_terminated(0, signal);
					if (test_pid[1])
						kill(test_pid[1], SIGKILL);
					if (test_pid[2])
						kill(test_pid[2], SIGKILL);
					status[0] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					test_pid[0] = 0;
				}
				if (this_pid == test_pid[1]) {
					print_terminated(1, signal);
					if (test_pid[0])
						kill(test_pid[0], SIGKILL);
					if (test_pid[2])
						kill(test_pid[2], SIGKILL);
					status[1] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					test_pid[1] = 0;
				}
				if (this_pid == test_pid[2]) {
					print_terminated(2, signal);
					if (test_pid[0])
						kill(test_pid[0], SIGKILL);
					if (test_pid[1])
						kill(test_pid[1], SIGKILL);
					status[2] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					test_pid[2] = 0;
				}
			} else if (WIFSTOPPED(this_status)) {
				int signal = WSTOPSIG(this_status);

				if (this_pid == test_pid[0]) {
					print_stopped(0, signal);
					if (test_pid[0])
						kill(test_pid[0], SIGKILL);
					if (test_pid[1])
						kill(test_pid[1], SIGKILL);
					if (test_pid[2])
						kill(test_pid[2], SIGKILL);
					status[0] = __RESULT_FAILURE;
					test_pid[0] = 0;
				}
				if (this_pid == test_pid[1]) {
					print_stopped(1, signal);
					if (test_pid[0])
						kill(test_pid[0], SIGKILL);
					if (test_pid[1])
						kill(test_pid[1], SIGKILL);
					if (test_pid[2])
						kill(test_pid[2], SIGKILL);
					status[1] = __RESULT_FAILURE;
					test_pid[1] = 0;
				}
				if (this_pid == test_pid[2]) {
					print_stopped(2, signal);
					if (test_pid[0])
						kill(test_pid[0], SIGKILL);
					if (test_pid[1])
						kill(test_pid[1], SIGKILL);
					if (test_pid[2])
						kill(test_pid[2], SIGKILL);
					status[2] = __RESULT_FAILURE;
					test_pid[2] = 0;
				}
			}
		} else {
			if (timer_timeout) {
				timer_timeout = 0;
				print_timeout(3);
			}
			if (test_pid[0])
				kill(test_pid[0], SIGKILL);
			if (test_pid[1])
				kill(test_pid[1], SIGKILL);
			if (test_pid[2])
				kill(test_pid[2], SIGKILL);
			goto waitagain;
		}
	}
	if (stop_tt() != __RESULT_SUCCESS)
		goto inconclusive;
	if (status[0] == __RESULT_NOTAPPL || status[1] == __RESULT_NOTAPPL || status[2] == __RESULT_NOTAPPL)
		return (__RESULT_NOTAPPL);
	if (status[0] == __RESULT_SKIPPED || status[1] == __RESULT_SKIPPED || status[2] == __RESULT_SKIPPED)
		return (__RESULT_SKIPPED);
	if (status[0] == __RESULT_FAILURE || status[1] == __RESULT_FAILURE || status[2] == __RESULT_FAILURE)
		return (__RESULT_FAILURE);
	if (status[0] == __RESULT_SUCCESS && status[1] == __RESULT_SUCCESS && status[2] == __RESULT_SUCCESS)
		return (__RESULT_SUCCESS);
      inconclusive:
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
	const char *tgrp;		/* test case group */
	const char *name;		/* test case name */
	const char *desc;		/* test case description */
	const char *sref;		/* test case standards section reference */
	struct test_stream *stream[3];	/* test streams */
	int (*start) (int);		/* start function */
	int (*stop) (int);		/* stop function */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} tests[] = {
	{
		numb_case_1_1, tgrp_case_1_1, name_case_1_1, desc_case_1_1, sref_case_1_1, {
	test_case_1_1_stream_0, test_case_1_1_stream_1, test_case_1_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_2, tgrp_case_1_2, name_case_1_2, desc_case_1_2, sref_case_1_2, {
	test_case_1_2_stream_0, test_case_1_2_stream_1, test_case_1_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_3, tgrp_case_1_3, name_case_1_3, desc_case_1_3, sref_case_1_3, {
	test_case_1_3_stream_0, test_case_1_3_stream_1, test_case_1_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_4, tgrp_case_1_4, name_case_1_4, desc_case_1_4, sref_case_1_4, {
	test_case_1_4_stream_0, test_case_1_4_stream_1, test_case_1_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_5, tgrp_case_1_5, name_case_1_5, desc_case_1_5, sref_case_1_5, {
	test_case_1_5_stream_0, test_case_1_5_stream_1, test_case_1_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_1, tgrp_case_1_6_1, name_case_1_6_1, desc_case_1_6_1, sref_case_1_6_1, {
	test_case_1_6_1_stream_0, test_case_1_6_1_stream_1, test_case_1_6_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_6_2, tgrp_case_1_6_2, name_case_1_6_2, desc_case_1_6_2, sref_case_1_6_2, {
	test_case_1_6_2_stream_0, test_case_1_6_2_stream_1, test_case_1_6_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_7_1, tgrp_case_1_7_1, name_case_1_7_1, desc_case_1_7_1, sref_case_1_7_1, {
	test_case_1_7_1_stream_0, test_case_1_7_1_stream_1, test_case_1_7_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_1_7_2, tgrp_case_1_7_2, name_case_1_7_2, desc_case_1_7_2, sref_case_1_7_2, {
	test_case_1_7_2_stream_0, test_case_1_7_2_stream_1, test_case_1_7_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_1, tgrp_case_2_1_1, name_case_2_1_1, desc_case_2_1_1, sref_case_2_1_1, {
	test_case_2_1_1_stream_0, test_case_2_1_1_stream_1, test_case_2_1_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_1, tgrp_case_2_2_1, name_case_2_2_1, desc_case_2_2_1, sref_case_2_2_1, {
	test_case_2_2_1_stream_0, test_case_2_2_1_stream_1, test_case_2_2_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_2, tgrp_case_2_2_2, name_case_2_2_2, desc_case_2_2_2, sref_case_2_2_2, {
	test_case_2_2_2_stream_0, test_case_2_2_2_stream_1, test_case_2_2_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_3, tgrp_case_2_2_3, name_case_2_2_3, desc_case_2_2_3, sref_case_2_2_3, {
	test_case_2_2_3_stream_0, test_case_2_2_3_stream_1, test_case_2_2_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_4, tgrp_case_2_2_4, name_case_2_2_4, desc_case_2_2_4, sref_case_2_2_4, {
	test_case_2_2_4_stream_0, test_case_2_2_4_stream_1, test_case_2_2_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_5, tgrp_case_2_2_5, name_case_2_2_5, desc_case_2_2_5, sref_case_2_2_5, {
	test_case_2_2_5_stream_0, test_case_2_2_5_stream_1, test_case_2_2_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_6, tgrp_case_2_2_6, name_case_2_2_6, desc_case_2_2_6, sref_case_2_2_6, {
	test_case_2_2_6_stream_0, test_case_2_2_6_stream_1, test_case_2_2_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_7, tgrp_case_2_2_7, name_case_2_2_7, desc_case_2_2_7, sref_case_2_2_7, {
	test_case_2_2_7_stream_0, test_case_2_2_7_stream_1, test_case_2_2_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_3_1, tgrp_case_2_3_1, name_case_2_3_1, desc_case_2_3_1, sref_case_2_3_1, {
	test_case_2_3_1_stream_0, test_case_2_3_1_stream_1, test_case_2_3_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_3_2, tgrp_case_2_3_2, name_case_2_3_2, desc_case_2_3_2, sref_case_2_3_2, {
	test_case_2_3_2_stream_0, test_case_2_3_2_stream_1, test_case_2_3_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_3_3, tgrp_case_2_3_3, name_case_2_3_3, desc_case_2_3_3, sref_case_2_3_3, {
	test_case_2_3_3_stream_0, test_case_2_3_3_stream_1, test_case_2_3_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_3_4, tgrp_case_2_3_4, name_case_2_3_4, desc_case_2_3_4, sref_case_2_3_4, {
	test_case_2_3_4_stream_0, test_case_2_3_4_stream_1, test_case_2_3_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_1, tgrp_case_2_4_1, name_case_2_4_1, desc_case_2_4_1, sref_case_2_4_1, {
	test_case_2_4_1_stream_0, test_case_2_4_1_stream_1, test_case_2_4_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_2, tgrp_case_2_4_2, name_case_2_4_2, desc_case_2_4_2, sref_case_2_4_2, {
	test_case_2_4_2_stream_0, test_case_2_4_2_stream_1, test_case_2_4_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_3, tgrp_case_2_4_3, name_case_2_4_3, desc_case_2_4_3, sref_case_2_4_3, {
	test_case_2_4_3_stream_0, test_case_2_4_3_stream_1, test_case_2_4_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_4, tgrp_case_2_4_4, name_case_2_4_4, desc_case_2_4_4, sref_case_2_4_4, {
	test_case_2_4_4_stream_0, test_case_2_4_4_stream_1, test_case_2_4_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_5, tgrp_case_2_4_5, name_case_2_4_5, desc_case_2_4_5, sref_case_2_4_5, {
	test_case_2_4_5_stream_0, test_case_2_4_5_stream_1, test_case_2_4_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_6, tgrp_case_2_4_6, name_case_2_4_6, desc_case_2_4_6, sref_case_2_4_6, {
	test_case_2_4_6_stream_0, test_case_2_4_6_stream_1, test_case_2_4_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_7, tgrp_case_2_4_7, name_case_2_4_7, desc_case_2_4_7, sref_case_2_4_7, {
	test_case_2_4_7_stream_0, test_case_2_4_7_stream_1, test_case_2_4_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4_8, tgrp_case_2_4_8, name_case_2_4_8, desc_case_2_4_8, sref_case_2_4_8, {
	test_case_2_4_8_stream_0, test_case_2_4_8_stream_1, test_case_2_4_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_1, tgrp_case_3_1_1, name_case_3_1_1, desc_case_3_1_1, sref_case_3_1_1, {
	test_case_3_1_1_stream_0, test_case_3_1_1_stream_1, test_case_3_1_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_2, tgrp_case_3_1_2, name_case_3_1_2, desc_case_3_1_2, sref_case_3_1_2, {
	test_case_3_1_2_stream_0, test_case_3_1_2_stream_1, test_case_3_1_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_3, tgrp_case_3_1_3, name_case_3_1_3, desc_case_3_1_3, sref_case_3_1_3, {
	test_case_3_1_3_stream_0, test_case_3_1_3_stream_1, test_case_3_1_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_4, tgrp_case_3_1_4, name_case_3_1_4, desc_case_3_1_4, sref_case_3_1_4, {
	test_case_3_1_4_stream_0, test_case_3_1_4_stream_1, test_case_3_1_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_5, tgrp_case_3_1_5, name_case_3_1_5, desc_case_3_1_5, sref_case_3_1_5, {
	test_case_3_1_5_stream_0, test_case_3_1_5_stream_1, test_case_3_1_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_6, tgrp_case_3_1_6, name_case_3_1_6, desc_case_3_1_6, sref_case_3_1_6, {
	test_case_3_1_6_stream_0, test_case_3_1_6_stream_1, test_case_3_1_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_7, tgrp_case_3_1_7, name_case_3_1_7, desc_case_3_1_7, sref_case_3_1_7, {
	test_case_3_1_7_stream_0, test_case_3_1_7_stream_1, test_case_3_1_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_8, tgrp_case_3_1_8, name_case_3_1_8, desc_case_3_1_8, sref_case_3_1_8, {
	test_case_3_1_8_stream_0, test_case_3_1_8_stream_1, test_case_3_1_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_9, tgrp_case_3_1_9, name_case_3_1_9, desc_case_3_1_9, sref_case_3_1_9, {
	test_case_3_1_9_stream_0, test_case_3_1_9_stream_1, test_case_3_1_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_10, tgrp_case_3_1_10, name_case_3_1_10, desc_case_3_1_10, sref_case_3_1_10, {
	test_case_3_1_10_stream_0, test_case_3_1_10_stream_1, test_case_3_1_10_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_1_11, tgrp_case_3_1_11, name_case_3_1_11, desc_case_3_1_11, sref_case_3_1_11, {
	test_case_3_1_11_stream_0, test_case_3_1_11_stream_1, test_case_3_1_11_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_1, tgrp_case_3_2_1, name_case_3_2_1, desc_case_3_2_1, sref_case_3_2_1, {
	test_case_3_2_1_stream_0, test_case_3_2_1_stream_1, test_case_3_2_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_2, tgrp_case_3_2_2, name_case_3_2_2, desc_case_3_2_2, sref_case_3_2_2, {
	test_case_3_2_2_stream_0, test_case_3_2_2_stream_1, test_case_3_2_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_3, tgrp_case_3_2_3, name_case_3_2_3, desc_case_3_2_3, sref_case_3_2_3, {
	test_case_3_2_3_stream_0, test_case_3_2_3_stream_1, test_case_3_2_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_4, tgrp_case_3_2_4, name_case_3_2_4, desc_case_3_2_4, sref_case_3_2_4, {
	test_case_3_2_4_stream_0, test_case_3_2_4_stream_1, test_case_3_2_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_5, tgrp_case_3_2_5, name_case_3_2_5, desc_case_3_2_5, sref_case_3_2_5, {
	test_case_3_2_5_stream_0, test_case_3_2_5_stream_1, test_case_3_2_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_6, tgrp_case_3_2_6, name_case_3_2_6, desc_case_3_2_6, sref_case_3_2_6, {
	test_case_3_2_6_stream_0, test_case_3_2_6_stream_1, test_case_3_2_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_7, tgrp_case_3_2_7, name_case_3_2_7, desc_case_3_2_7, sref_case_3_2_7, {
	test_case_3_2_7_stream_0, test_case_3_2_7_stream_1, test_case_3_2_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_8, tgrp_case_3_2_8, name_case_3_2_8, desc_case_3_2_8, sref_case_3_2_8, {
	test_case_3_2_8_stream_0, test_case_3_2_8_stream_1, test_case_3_2_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_9, tgrp_case_3_2_9, name_case_3_2_9, desc_case_3_2_9, sref_case_3_2_9, {
	test_case_3_2_9_stream_0, test_case_3_2_9_stream_1, test_case_3_2_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_10, tgrp_case_3_2_10, name_case_3_2_10, desc_case_3_2_10, sref_case_3_2_10, {
	test_case_3_2_10_stream_0, test_case_3_2_10_stream_1, test_case_3_2_10_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_2_11, tgrp_case_3_2_11, name_case_3_2_11, desc_case_3_2_11, sref_case_3_2_11, {
	test_case_3_2_11_stream_0, test_case_3_2_11_stream_1, test_case_3_2_11_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_1, tgrp_case_3_3_1, name_case_3_3_1, desc_case_3_3_1, sref_case_3_3_1, {
	test_case_3_3_1_stream_0, test_case_3_3_1_stream_1, test_case_3_3_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_2, tgrp_case_3_3_2, name_case_3_3_2, desc_case_3_3_2, sref_case_3_3_2, {
	test_case_3_3_2_stream_0, test_case_3_3_2_stream_1, test_case_3_3_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_3, tgrp_case_3_3_3, name_case_3_3_3, desc_case_3_3_3, sref_case_3_3_3, {
	test_case_3_3_3_stream_0, test_case_3_3_3_stream_1, test_case_3_3_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_4, tgrp_case_3_3_4, name_case_3_3_4, desc_case_3_3_4, sref_case_3_3_4, {
	test_case_3_3_4_stream_0, test_case_3_3_4_stream_1, test_case_3_3_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_5, tgrp_case_3_3_5, name_case_3_3_5, desc_case_3_3_5, sref_case_3_3_5, {
	test_case_3_3_5_stream_0, test_case_3_3_5_stream_1, test_case_3_3_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_6, tgrp_case_3_3_6, name_case_3_3_6, desc_case_3_3_6, sref_case_3_3_6, {
	test_case_3_3_6_stream_0, test_case_3_3_6_stream_1, test_case_3_3_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_7, tgrp_case_3_3_7, name_case_3_3_7, desc_case_3_3_7, sref_case_3_3_7, {
	test_case_3_3_7_stream_0, test_case_3_3_7_stream_1, test_case_3_3_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_8, tgrp_case_3_3_8, name_case_3_3_8, desc_case_3_3_8, sref_case_3_3_8, {
	test_case_3_3_8_stream_0, test_case_3_3_8_stream_1, test_case_3_3_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_9, tgrp_case_3_3_9, name_case_3_3_9, desc_case_3_3_9, sref_case_3_3_9, {
	test_case_3_3_9_stream_0, test_case_3_3_9_stream_1, test_case_3_3_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_10, tgrp_case_3_3_10, name_case_3_3_10, desc_case_3_3_10, sref_case_3_3_10, {
	test_case_3_3_10_stream_0, test_case_3_3_10_stream_1, test_case_3_3_10_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_3_3_11, tgrp_case_3_3_11, name_case_3_3_11, desc_case_3_3_11, sref_case_3_3_11, {
	test_case_3_3_11_stream_0, test_case_3_3_11_stream_1, test_case_3_3_11_stream_2}, &begin_tests, &end_tests, 0, 0}, {
	NULL,}
};

static int summary = 0;

void
print_header(void)
{
	if (verbose <= 0)
		return;
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "\n%s - %s - %s - Conformance Test Suite\n", lstdname, lpkgname, shortname);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

int
do_tests(int num_tests)
{
	int i;
	int result = __RESULT_INCONCLUSIVE;
	int notapplicable = 0;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	int skipped = 0;
	int notselected = 0;
	int aborted = 0;

	print_header();
	show = 0;
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "\nUsing device %s\n\n", devname);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (num_tests == 1 || begin_tests(0) == __RESULT_SUCCESS) {
		if (num_tests != 1)
			end_tests(0);
		show = 1;
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (!tests[i].run) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				notselected++;
				continue;
			}
			if (aborted) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				inconclusive++;
				continue;
			}
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				if (verbose > 1)
					fprintf(stdout, "\nTest Group: %s", tests[i].tgrp);
				fprintf(stdout, "\nTest Case %s-%s/%s: %s\n", sstdname, shortname, tests[i].numb, tests[i].name);
				if (verbose > 1)
					fprintf(stdout, "Test Reference: %s\n", tests[i].sref);
				if (verbose > 1)
					fprintf(stdout, "%s\n", tests[i].desc);
				fprintf(stdout, "\n");
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((result = tests[i].result) == 0) {
				if ((result = (*tests[i].start) (i)) != __RESULT_SUCCESS)
					goto inconclusive;
				result = test_run(tests[i].stream);
				(*tests[i].stop) (i);
			} else {
				switch (result) {
				case __RESULT_SUCCESS:
					print_passed(3);
					break;
				case __RESULT_FAILURE:
					print_failed(3);
					break;
				case __RESULT_NOTAPPL:
					print_notapplicable(3);
					break;
				case __RESULT_SKIPPED:
					print_skipped(3);
					break;
				default:
				case __RESULT_INCONCLUSIVE:
					print_inconclusive(3);
					break;
				}
			}
			switch (result) {
			case __RESULT_SUCCESS:
				successes++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "*********\n");
					fprintf(stdout, "********* Test Case SUCCESSFUL\n");
					fprintf(stdout, "*********\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case __RESULT_FAILURE:
				failures++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case FAILED\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case __RESULT_NOTAPPL:
				notapplicable++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case NOT APPLICABLE\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case __RESULT_SKIPPED:
				skipped++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case SKIPPED\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			default:
			case __RESULT_INCONCLUSIVE:
			      inconclusive:
				inconclusive++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "?????????\n");
					fprintf(stdout, "????????? Test Case INCONCLUSIVE\n");
					fprintf(stdout, "?????????\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			}
			tests[i].result = result;
			if (exit_on_failure && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE))
				aborted = 1;
		}
		if (summary && verbose) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
			for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
				if (tests[i].run) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "Test Case %s-%s/%-10s ", sstdname, shortname, tests[i].numb);
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
					switch (tests[i].result) {
					case __RESULT_SUCCESS:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "SUCCESS\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case __RESULT_FAILURE:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "FAILURE\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case __RESULT_NOTAPPL:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "NOT APPLICABLE\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case __RESULT_SKIPPED:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "SKIPPED\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					default:
					case __RESULT_INCONCLUSIVE:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "INCONCLUSIVE\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					}
				}
			}
		}
		if (verbose > 0 && num_tests > 1) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n");
			fprintf(stdout, "========= %3d successes     \n", successes);
			fprintf(stdout, "========= %3d failures      \n", failures);
			fprintf(stdout, "========= %3d inconclusive  \n", inconclusive);
			fprintf(stdout, "========= %3d not applicable\n", notapplicable);
			fprintf(stdout, "========= %3d skipped       \n", skipped);
			fprintf(stdout, "========= %3d not selected  \n", notselected);
			fprintf(stdout, "============================\n");
			fprintf(stdout, "========= %3d total         \n", successes + failures + inconclusive + notapplicable + skipped + notselected);
			if (!(aborted + failures))
				fprintf(stdout, "\nDone.\n\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		if (aborted) {
			dummy = lockf(fileno(stderr), F_LOCK, 0);
			if (verbose > 0)
				fprintf(stderr, "\n");
			fprintf(stderr, "Test Suite aborted due to failure.\n");
			if (verbose > 0)
				fprintf(stderr, "\n");
			fflush(stderr);
			dummy = lockf(fileno(stderr), F_ULOCK, 0);
		} else if (failures) {
			dummy = lockf(fileno(stderr), F_LOCK, 0);
			if (verbose > 0)
				fprintf(stderr, "\n");
			fprintf(stderr, "Test Suite failed.\n");
			if (verbose > 0)
				fprintf(stderr, "\n");
			fflush(stderr);
			dummy = lockf(fileno(stderr), F_ULOCK, 0);
		}
		if (num_tests == 1) {
			if (successes)
				return (0);
			if (failures)
				return (1);
			if (inconclusive)
				return (1);
			if (notapplicable)
				return (0);
			if (skipped)
				return (77);
		}
		return (aborted);
	} else {
		end_tests(0);
		show = 1;
		dummy = lockf(fileno(stderr), F_LOCK, 0);
		fprintf(stderr, "Test Suite setup failed!\n");
		fflush(stderr);
		dummy = lockf(fileno(stderr), F_ULOCK, 0);
		return (2);
	}
}

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	print_header();
	fprintf(stdout, "\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
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
General Public License (GPL) Version 2,  so long as the  software is distributed\n\
with, and only used for the testing of, OpenSS7 modules, drivers, and libraries.\n\
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
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
\n\
");
}

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s:\n\
    %2$s\n\
    Copyright (c) 1997-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
\n\
    See `%1$s --copying' for copying permission.\n\
\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    -d, --device DEVICE\n\
        device name to open [default: %2$s].\n\
    -e, --exit\n\
        exit on the first failed or inconclusive test case.\n\
    -l, --list [RANGE]\n\
        list test case names within a range [default: all] and exit.\n\
    -f, --fast [SCALE]\n\
        increase speed of tests by scaling timers [default: 50]\n\
    -s, --summary\n\
        print a test case summary at end of testing [default: off]\n\
    -o, --onetest [TESTCASE]\n\
        run a single test case.\n\
    -t, --tests [RANGE]\n\
        run a range of test cases.\n\
    -m, --messages\n\
        display messages. [default: off]\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        increase verbosity or set to LEVEL [default: 1]\n\
        this option may be repeated.\n\
    -h, --help, -?, --?\n\
        print this usage message and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
\n\
", argv[0], devname);
}

int
main(int argc, char *argv[])
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
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "d:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "d:el::f::so:t:mqvhVC?");
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
						if (verbose > 2)
							fprintf(stdout, "Test Group: %s\n", t->tgrp);
						fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
						if (verbose > 2)
							fprintf(stdout, "Test Reference: %s\n", t->sref);
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
					if (verbose > 2)
						fprintf(stdout, "Test Group: %s\n", t->tgrp);
					fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
					if (verbose > 2)
						fprintf(stdout, "Test Reference: %s\n", t->sref);
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
						// if (!t->result) {
						t->run = 1;
						n++;
						tests_to_run++;
						// }
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
					// if (!t->result) {
					t->run = 1;
					n++;
					tests_to_run++;
					// }
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
		case 'C':
			copying(argc, argv);
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
			goto bad_usage;
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
	switch (tests_to_run) {
	case 0:
		if (verbose > 0) {
			fprintf(stderr, "%s: error: no tests to run\n", argv[0]);
			fflush(stderr);
		}
		exit(2);
	case 1:
		break;
	default:
		copying(argc, argv);
	}
	exit(do_tests(tests_to_run));
}
