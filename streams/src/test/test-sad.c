/*****************************************************************************

 @(#) $RCSfile: test-sad.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2006/12/18 07:32:45 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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
 with, and only used for the testing of, OpenSS7 modules, drivers, and libraries.

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

 Last Modified $Date: 2006/12/18 07:32:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sad.c,v $
 Revision 0.9.2.26  2006/12/18 07:32:45  brian
 - lfs device names, autoload clone minors, device numbering, missing manpages

 Revision 0.9.2.25  2006/11/26 15:27:40  brian
 - testing and corrections to strlog capabilities

 Revision 0.9.2.24  2006/03/03 10:57:16  brian
 - 32-bit compatibility support, updates for release

 Revision 0.9.2.23  2006/02/22 19:57:34  brian
 - strap out lockf() that was blocking some test case
   processes on SMP and even on UP

 Revision 0.9.2.22  2005/12/28 09:48:05  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.21  2005/11/07 10:56:21  brian
 - converged SAD tests for LiS and LfS

 Revision 0.9.2.20  2005/10/15 10:19:52  brian
 - working up autopush and SAD driver tests

 Revision 0.9.2.19  2005/10/15 02:12:32  brian
 - continuing SAD testing

 Revision 0.9.2.18  2005/10/13 10:58:53  brian
 - working up testing of sad(4) and sc(4)

 Revision 0.9.2.17  2005/10/10 10:37:22  brian
 - FIFOs working nicely and tested.

 Revision 0.9.2.16  2005/10/07 09:34:31  brian
 - more testing and corrections

 Revision 0.9.2.15  2005/10/02 10:34:52  brian
 - staring full read/write getmsg/putmsg testing

 Revision 0.9.2.14  2005/09/29 23:08:20  brian
 - starting testing of FIFOs

 Revision 0.9.2.13  2005/09/25 22:52:36  brian
 - added test module and continuing with testing

 Revision 0.9.2.12  2005/09/10 18:16:41  brian
 - more test build

 Revision 0.9.2.11  2005/09/08 09:37:11  brian
 - corrected error output

 Revision 0.9.2.10  2005/09/08 05:52:43  brian
 - added nullmod module and loop driver
 - corrections during testing
 - many ioctl(2p) test cases work very well now

 Revision 0.9.2.9  2005/07/18 12:07:05  brian
 - standard indentation

 Revision 0.9.2.7  2005/07/18 12:07:05  brian
 - standard indentation

 Revision 0.9.2.6  2005/06/06 12:03:56  brian
 - upgraded test suites

 Revision 0.9.2.5  2005/05/15 23:57:08  brian
 - update tests

 Revision 0.9.2.4  2005/05/14 08:39:37  brian
 - updated copyright headers

 Revision 0.9.2.3  2005/05/14 08:34:46  brian
 - copyright header correction

 Revision 0.9.2.2  2005/01/16 23:09:06  brian
 - Added --copying options.

 Revision 0.9.2.1  2004/08/22 06:17:56  brian
 - Checkin on new working branch.

 Revision 1.2  2004/06/09 08:32:59  brian
 - Open works fine but don't want to hold dentries in cache.

 Revision 1.2  2004/06/08 02:27:36  brian
 - Framework for testing streams.

 Revision 1.1  2004/06/07 22:35:57  brian
 - Starting test suites for everything.

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sad.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2006/12/18 07:32:45 $"

static char const ident[] = "$RCSfile: test-sad.c,v $ $Name:  $($Revision: 0.9.2.26 $) $Date: 2006/12/18 07:32:45 $";

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

#include <sys/sad.h>

#ifdef LFS
#include "include/sys/config.h"
#endif

#ifdef LIS
#include "sys/LiS/config.h"
#ifndef CONFIG_STREAMS_SAD_NAME
#define CONFIG_STREAMS_SAD_NAME "sad"
#endif
#ifndef CONFIG_STREAMS_SC_NAME
#define CONFIG_STREAMS_SC_NAME "nullmod"
#endif
#endif

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
static const char *shortname = "SAD";
#ifdef LFS
static char devname[256] = "/dev/streams/clone/sad";
static char admname[256] = "/dev/streams/sad/admin";
#endif
#ifdef LIS
static char devname[256] = "/dev/sad";
static char admname[256] = "/dev/sad";
#endif

static int repeat_verbose = 0;
static int repeat_on_success = 0;
static int repeat_on_failure = 0;
static int exit_on_failure = 0;

static int verbose = 1;

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;

//static int show_data = 1;

static int last_prim = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;

int test_fd[3] = { 0, 0, 0 };

#define BUFSIZE 32*4096

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 100	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define LONGEST_WAIT	5000	// 20000 // 10000
#define TEST_DURATION	120000
#define INFINITE_WAIT	-1

static int test_duration = TEST_DURATION;	/* wait on other side */

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

static int state = 0;
static const char *failure_string = NULL;

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
#endif

static int
time_event(int child, int event)
{
	static const char *msgs[] = {
		"                    ! %11.6g                |                    <%d:%03d>\n",
		"                    |                %11.6g !                    <%d:%03d>\n",
		"                    |             %11.6g !  |                    <%d:%03d>\n",
		"                    !        %11.6g         !                    <%d:%03d>\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg)) {
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
		fprintf(stdout, msgs[child], t, child, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	return (event);
}

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

	if (duration == (long) INFINITE_WAIT)
		return __RESULT_SUCCESS;
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

		snprintf(buf, sizeof(buf), "[%ld]", (long) err);
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
	case SAD_VML:
		return ("SAD_VML");
	case SAD_GAP:
		return ("SAD_GAP");
	case SAD_SAP:
		return ("SAD_SAP");
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
print_string_state(int child, const char *msgs[], const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_triple_string(int child, const char *msgs[], const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], "", child, state);
	fprintf(stdout, msgs[child], string, child, state);
	fprintf(stdout, msgs[child], "", child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_more(int child)
{
	show = 1;
}

void
print_less(int child)
{
	static const char *msgs[] = {
		"     . %1$6.6s .     | <------         .         ------> :                    [%2$d:%3$03d]\n",
		"       . %1$6.6s .   | <------         .         ------> :                    [%2$d:%3$03d]\n",
		"         . %1$6.6s . | <------         .         ------> :                    [%2$d:%3$03d]\n",
		"         . %1$6.6s . : <------         .      ------> :> : . %1$-6.6s .         [%2$d:%3$03d]\n",
	};

	if (show && verbose > 0)
		print_triple_string(child, msgs, "(more)");
	show = 0;
}

void
print_pipe(int child)
{
	static const char *msgs[] = {
		"  pipe()      ----->v  v<------------------------------>v                   \n",
		"                    v  v<------------------------------>v<-----     pipe()  \n",
		"                    .  .                                .                   \n",
	};

	if (show && verbose > 3)
		print_simple(child, msgs);
}

void
print_open(int child, const char *name)
{
	static const char *msgs[] = {
		"open()        ----->v %-30.30s |  |                   \n",
		"  open()      ----->v %-30.30s |  |                   \n",
		"    open()    ----->v %-30.30s |  |                   \n",
		"                    . %-30.30s .  .                   \n",
	};

	if (show && verbose > 3)
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

	if (show && verbose > 3)
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
print_failure(int child, const char *string)
{
	static const char *msgs[] = {
		"....................|%-32.32s|..|                    [%d:%03d]\n",
		"  ..................|%-32.32s|..|                    [%d:%03d]\n",
		"    ................|%-32.32s|..|                    [%d:%03d]\n",
		"....................|%-32.32s...|................... [%d:%03d]\n",
	};

	if (string && strnlen(string, 32) > 0 && verbose > 0)
		print_string_state(child, msgs, string);
}

void
print_notapplicable(int child)
{
	static const char *msgs[] = {
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|                    [%d:%03d]\n",
		"  X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|                    [%d:%03d]\n",
		"    X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|                    [%d:%03d]\n",
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_skipped(int child)
{
	static const char *msgs[] = {
		"::::::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|                    [%d:%03d]\n",
		"  ::::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|                    [%d:%03d]\n",
		"    ::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|                    [%d:%03d]\n",
		"::::::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|::::::::::::::::::: [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_inconclusive(int child)
{
	static const char *msgs[] = {
		"????????????????????|?????????? INCONCLUSIVE ???????????|                    [%d:%03d]\n",
		"  ??????????????????|?????????? INCONCLUSIVE ???????????|                    [%d:%03d]\n",
		"    ????????????????|?????????? INCONCLUSIVE ???????????|                    [%d:%03d]\n",
		"????????????????????|?????????? INCONCLUSIVE ???????????|??????????????????? [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_test(int child)
{
	static const char *msgs[] = {
		"--------------------+---------------Test----------------+                   \n",
		"  ------------------+---------------Test----------------+                   \n",
		"    ----------------+---------------Test----------------+                   \n",
		"--------------------+---------------Test----------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXXXXX|                    [%d:%03d]\n",
		"  XXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXXXXX|                    [%d:%03d]\n",
		"    XXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXXXXX|                    [%d:%03d]\n",
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########### SCRIPT ERROR ##########|                    [%d:%03d]\n",
		"  ##################|########### SCRIPT ERROR ##########|                    [%d:%03d]\n",
		"    ################|########### SCRIPT ERROR ##########|                    [%d:%03d]\n",
		"####################|########### SCRIPT ERROR ##########|################### [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_passed(int child)
{
	static const char *msgs[] = {
		"********************|************* PASSED **************|                    [%d:%03d]\n",
		"  ******************|************* PASSED **************|                    [%d:%03d]\n",
		"    ****************|************* PASSED **************|                    [%d:%03d]\n",
		"********************|************* PASSED **************|******************* [%d:%03d]\n",
	};

	if (verbose > 2)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_postamble(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------Postamble-------------+                   \n",
		"  ------------------+-------------Postamble-------------+                   \n",
		"    ----------------+-------------Postamble-------------+                   \n",
		"--------------------+-------------Postamble-------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_test_end(int child)
{
	static const char *msgs[] = {
		"--------------------+-----------------------------------+                   \n",
		"  ------------------+-----------------------------------+                   \n",
		"    ----------------+-----------------------------------+                   \n",
		"--------------------+-----------------------------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|                    {%d:%03d}\n",
		"  @@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|                    {%d:%03d}\n",
		"    @@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|                    {%d:%03d}\n",
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|                    {%d:%03d}\n",
		"  &&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|                    {%d:%03d}\n",
		"    &&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|                    {%d:%03d}\n",
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
		"  ++++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
		"    ++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
		"++++++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
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
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - - -|                    [%d:%03d]\n",
		"  - - - - - - - - - |- - - - - - -nothing! - - - - - - -|                    [%d:%03d]\n",
		"    - - - - - - - - |- - - - - - -nothing! - - - - - - -|                    [%d:%03d]\n",
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - - -|- - - - - - - - - - [%d:%03d]\n",
	};

	if (verbose > 1)
		print_double_int(child, msgs, child, state);
}

void
print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                   |                    [%d:%03d]\n",
		"  %-14s--->|                                   |                    [%d:%03d]\n",
		"    %-14s->|                                   |                    [%d:%03d]\n",
		"                    |          %-14s           |                    [%d:%03d]\n",
	};

	if ((verbose && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, command);
}

void
print_signal(int child, int signum)
{
	static const char *msgs[] = {
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"  >>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"    >>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, signal_string(signum));
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
		"%-14s----->|       %-16.16s            |                    [%d:%03d]\n",
		"  %-14s--->|       %-16.16s            |                    [%d:%03d]\n",
		"    %-14s->|       %-16.16s            |                    [%d:%03d]\n",
		"                    | %-14s %-16.16s   |                    [%d:%03d]\n",
	};

	if (show && verbose > 3)
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
		"%-14s<----/|                                   |                    [%d:%03d]\n",
		"  %-14s<--/|                                   |                    [%d:%03d]\n",
		"    %-14s</|                                   |                    [%d:%03d]\n",
		"                    |          [%14s]         |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, errno_string(error));
}

void
print_success(int child)
{
	static const char *msgs[] = {
		"ok            <----/|                                   |                    [%d:%03d]\n",
		"  ok          <----/|                                   |                    [%d:%03d]\n",
		"    ok        <----/|                                   |                    [%d:%03d]\n",
		"                    |                 ok                |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_double_int(child, msgs, child, state);
}

void
print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"%10d<--------/|                                   |                    [%d:%03d]\n",
		"  %10d<------/|                                   |                    [%d:%03d]\n",
		"    %10d<----/|                                   |                    [%d:%03d]\n",
		"                    |            [%10d]           |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
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
		"%10d<--------/| %-30.30s |  |                    [%d:%03d]\n",
		"  %10d<------/| %-30.30s |  |                    [%d:%03d]\n",
		"    %10d<----/| %-30.30s |  |                    [%d:%03d]\n",
		"          %10d| %-30.30s |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 3)
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

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_expect(int child, int want)
{
	static const char *msgs[] = {
		"(%-14s)    |- - - - - -[Expected]- - - - - - - |                     [%d:%03d]\n",
		"  (%-14s)  |- - - - - -[Expected]- - - - - - - |                     [%d:%03d]\n",
		"    (%-14s)|- - - - - -[Expected]- - - - - - - |                     [%d:%03d]\n",
		"                    |- - -[Expected %-14s] - - |                     [%d:%03d]\n",
	};

	if (verbose > 1 && show)
		print_string_state(child, msgs, event_string(want));
}

void
print_string(int child, const char *string)
{
	static const char *msgs[] = {
		"%-16s    |                                   |                    \n",
		"  %-16s  |                                   |                    \n",
		"    %-16s|                                   |                    \n",
		"                    |  |      %-16s          |                    \n",
	};

	if (show && verbose > 0)
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
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / / / |                    [%d:%03d]\n",
		"  / / / / / / / / / | / / / Waiting %03lu seconds / / / / |                    [%d:%03d]\n",
		"    / / / / / / / / | / / / Waiting %03lu seconds / / / / |                    [%d:%03d]\n",
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / / / | / / / / / / / / /  [%d:%03d]\n",
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
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / |                    [%d:%03d]\n",
		"  / / / / / / / / / | / / Waiting %8.4f seconds/ / / |                    [%d:%03d]\n",
		"    / / / / / / / / | / / Waiting %8.4f seconds/ / / |                    [%d:%03d]\n",
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / | / / / / / / / / /  [%d:%03d]\n",
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
	for (;;) {
		if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		if (show && verbose > 3)
			print_success_value(child, last_retval);
		return (__RESULT_SUCCESS);
	}
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
	if (show && verbose > 4) {
		int i;

		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "fdinsert to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
		fprintf(stdout, "[");
		for (i = 0; i < (ctrl ? ctrl->len : 0); i++)
			fprintf(stdout, "%02X", ctrl->buf[i]);
		fprintf(stdout, "]\n");
		fprintf(stdout, "[");
		for (i = 0; i < (data ? data->len : 0); i++)
			fprintf(stdout, "%02X", data->buf[i]);
		fprintf(stdout, "]\n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) != __RESULT_SUCCESS)
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
	if (flags & MSG_BAND || band) {
		if ((verbose > 3 && show) || (verbose > 5 && show_msg)) {
			int i;

			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putpmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			fprintf(stdout, "[");
			for (i = 0; i < (ctrl ? ctrl->len : 0); i++)
				fprintf(stdout, "%02X", ctrl->buf[i]);
			fprintf(stdout, "]\n");
			fprintf(stdout, "[");
			for (i = 0; i < (data ? data->len : 0); i++)
				fprintf(stdout, "%02X", data->buf[i]);
			fprintf(stdout, "]\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		if (ctrl == NULL || data != NULL)
			print_datcall(child, "M_DATA----------", data ? data->len : 0);
		for (;;) {
			if ((last_retval = putpmsg(test_fd[child], ctrl, data, band, flags)) == -1) {
				if (last_errno == EINTR || last_errno == ERESTART)
					continue;
				print_errno(child, (last_errno = errno));
				return (__RESULT_FAILURE);
			}
			if ((verbose > 3 && show) || (verbose > 5 && show_msg))
				print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	} else {
		if ((verbose > 3 && show) || (verbose > 5 && show_msg)) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
			fflush(stdout);
		}
		if (ctrl == NULL || data != NULL)
			print_datcall(child, "M_DATA----------", data ? data->len : 0);
		for (;;) {
			if ((last_retval = putmsg(test_fd[child], ctrl, data, flags)) == -1) {
				if (last_errno == EINTR || last_errno == ERESTART)
					continue;
				print_errno(child, (last_errno = errno));
				return (__RESULT_FAILURE);
			}
			if ((verbose > 3 && show) || (verbose > 5 && show_msg))
				print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	}
}

int
test_write(int child, const void *buf, size_t len)
{
	print_datcall(child, "write(2)------", len);
	for (;;) {
		if ((last_retval = write(test_fd[child], buf, len)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_writev(int child, const struct iovec *iov, int num)
{
	print_syscall(child, "writev(2)-----");
	for (;;) {
		if ((last_retval = writev(test_fd[child], iov, num)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_getmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *flagp)
{
	print_datcall(child, "getmsg(2)-----", data ? data->maxlen : -1);
	for (;;) {
		if ((last_retval = getmsg(test_fd[child], ctrl, data, flagp)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_getpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *bandp, int *flagp)
{
	print_datcall(child, "getpmsg(2)----", data ? data->maxlen : -1);
	for (;;) {
		if ((last_retval = getpmsg(test_fd[child], ctrl, data, bandp, flagp)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_read(int child, void *buf, size_t count)
{
	print_datcall(child, "read(2)-------", count);
	for (;;) {
		if ((last_retval = read(test_fd[child], buf, count)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_readv(int child, const struct iovec *iov, int count)
{
	print_syscall(child, "readv(2)------");
	for (;;) {
		if ((last_retval = readv(test_fd[child], iov, count)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_nonblock(int child)
{
	long flags;

	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((flags = last_retval = fcntl(test_fd[child], F_GETFL)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags | O_NONBLOCK)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_block(int child)
{
	long flags;

	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((flags = last_retval = fcntl(test_fd[child], F_GETFL)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags & ~O_NONBLOCK)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_isastream(int child)
{
	int result;

	print_syscall(child, "isastream(2)--");
	for (;;) {
		if ((result = last_retval = isastream(test_fd[child])) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_poll(int child, const short events, short *revents, long ms)
{
	struct pollfd pfd = {.fd = test_fd[child],.events = events,.revents = 0 };
	int result;

	print_poll(child, events);
	for (;;) {
		if ((result = last_retval = poll(&pfd, 1, ms)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_poll_value(child, last_retval, pfd.revents);
		if (last_retval == 1 && revents)
			*revents = pfd.revents;
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_pipe(int child)
{
	int fds[2];

	for (;;) {
		print_pipe(child);
		if (pipe(fds) >= 0) {
			test_fd[child + 0] = fds[0];
			test_fd[child + 1] = fds[1];
			print_success(child);
			return (__RESULT_SUCCESS);
		}
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
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

	for (;;) {
		if ((fd = test_fopen(child, name, flags)) >= 0) {
			test_fd[child] = fd;
			return (__RESULT_SUCCESS);
		}
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
}

int
test_close(int child)
{
	int fd = test_fd[child];

	test_fd[child] = 0;
	for (;;) {
		if (test_fclose(child, fd) == __RESULT_SUCCESS)
			return (__RESULT_SUCCESS);
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		print_errno(child, (last_errno = errno));
		return __RESULT_FAILURE;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Stream Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int
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

static int
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

static long old_test_duration = 0;

static int
begin_sanity(int index)
{
	old_test_duration = test_duration;
	test_duration = 5000;
	return begin_tests(index);
}

static int
end_sanity(int index)
{
	test_duration = old_test_duration;
	return end_tests(index);
}

/*
 *  =========================================================================
 *
 *  Preambles and Postambles...
 *
 *  =========================================================================
 */

int
preamble_none(int child)
{
	return __RESULT_SUCCESS;
}

int
postamble_none(int child)
{
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
 *  Check test case guard timer.
 */
#define test_group_0 "0. Sanity checks"
#define test_group_0_1 "0.1. Guard timers"
#define tgrp_case_0_1 test_group_0
#define sgrp_case_0_1 test_group_0_1
#define numb_case_0_1 "0.1"
#define name_case_0_1 "Check test case guard timer."
#define xtra_case_0_1 NULL
#define sref_case_0_1 "(none)"
#define desc_case_0_1 "\
Checks that the test case guard timer will fire and bring down the children."

int
test_case_0_1(int child)
{
	if (test_duration == INFINITE_WAIT)
		return __RESULT_NOTAPPL;
	test_msleep(child, test_duration + 1000);
	state++;
	return (__RESULT_SUCCESS);
}

struct test_stream test_0_1 = { &preamble_none, &test_case_0_1, &postamble_none };

#define test_case_0_1_stream_0 (&test_0_1)
#define test_case_0_1_stream_1 (&test_0_1)
#define test_case_0_1_stream_2 (&test_0_1)

/*
 *  Open and Close 1 Stream.
 */
static const char test_group_1[] = "Open and close Streams";

#define tgrp_case_1_1 test_group_1
#define sgrp_case_1_1 NULL
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Open and close 1 user SAD Stream."
#define xtra_case_1_1 NULL
#define sref_case_1_1 sref_none
#define desc_case_1_1 "\
Checks that one user SAD Stream can be opened and closed."

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
#define sgrp_case_1_2 NULL
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Open and close 3 user SAD Streams."
#define xtra_case_1_2 NULL
#define sref_case_1_2 sref_none
#define desc_case_1_2 "\
Checks that three user SAD Streams can be opened and closed."

int
test_case_1_2(int child)
{
	return __RESULT_NOTAPPL;
#if 0
	/* just happens to work on UP (because it opens and closes so fast) */
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
#endif
}
struct test_stream test_1_2 = { NULL, &test_case_1_2, NULL };

#define test_case_1_2_stream_0 (&test_1_2)
#define test_case_1_2_stream_1 (&test_1_2)
#define test_case_1_2_stream_2 (&test_1_2)

#define tgrp_case_1_3 test_group_1
#define numb_case_1_3 "1.3"
#define name_case_1_3 "Open and close 1 admin SAD Stream."
#define sref_case_1_3 sref_none
#define desc_case_1_3 "\
Checks that one admin SAD Stream can be opened and closed."

int
test_case_1_3(int child)
{
	if (getuid() != 0 && geteuid() != 0)
		return __RESULT_SKIPPED;
	if (test_open(child, admname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}
struct test_stream test_1_3 = { NULL, &test_case_1_3, NULL };

#define test_case_1_3_stream_0 (&test_1_3)
#define test_case_1_3_stream_1 (NULL)
#define test_case_1_3_stream_2 (NULL)

static const char test_group_2[] = "STREAMS Administrative Driver (SAD), ioctl(2).";
static const char sref_group_2[] = "STREAMS Administrative Driver (SAD), sad(4) manual page.";

#define tgrp_case_2_1_1 test_group_2
#define numb_case_2_1_1 "2.1.1"
#define name_case_2_1_1 "Perform SAD_VML - EINVAL."
#define sref_case_2_1_1 sref_group_2
#define desc_case_2_1_1 "\
Check that SAD_VML can be performed.  Checks that EINVAL is returned\n\
when sl_nmods is less than one (1)."

int
test_case_2_1_1(int child)
{
	struct str_mlist sml = { CONFIG_STREAMS_SAD_NAME };
	struct str_list sl = { 0, &sml };

	if (test_ioctl(child, SAD_VML, (intptr_t) &sl) == __RESULT_SUCCESS)
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

#define tgrp_case_2_1_2 test_group_2
#define numb_case_2_1_2 "2.1.2"
#define name_case_2_1_2 "Perform SAD_VML - EFAULT."
#define sref_case_2_1_2 sref_group_2
#define desc_case_2_1_2 "\
Check that SAD_VML can be performed.  Checks that EFAULT is returned\n\
when arg is NULL or points outside the caller's address space."

int
test_case_2_1_2(int child)
{
	if (test_ioctl(child, SAD_VML, (intptr_t) NULL) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_VML, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_2 = { &preamble_0, &test_case_2_1_2, &postamble_0 };

#define test_case_2_1_2_stream_0 (&test_2_1_2)
#define test_case_2_1_2_stream_1 (NULL)
#define test_case_2_1_2_stream_2 (NULL)

#define tgrp_case_2_1_3 test_group_2
#define numb_case_2_1_3 "2.1.3"
#define name_case_2_1_3 "Perform SAD_VML - EFAULT."
#define sref_case_2_1_3 sref_group_2
#define desc_case_2_1_3 "\
Check that SAD_VML can be performed.  Checks that EFAULT is returned\n\
when sl_mlist is NULL or points outside the caller's address space."

int
test_case_2_1_3(int child)
{
	struct str_list sl = { 1, NULL };

	if (test_ioctl(child, SAD_VML, (intptr_t) &sl) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	sl.sl_modlist = INVALID_ADDRESS;
	if (test_ioctl(child, SAD_VML, (intptr_t) &sl) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_3 = { &preamble_0, &test_case_2_1_3, &postamble_0 };

#define test_case_2_1_3_stream_0 (&test_2_1_3)
#define test_case_2_1_3_stream_1 (NULL)
#define test_case_2_1_3_stream_2 (NULL)

#define tgrp_case_2_1_4 test_group_2
#define numb_case_2_1_4 "2.1.4"
#define name_case_2_1_4 "Perform SAD_VML."
#define sref_case_2_1_4 sref_group_2
#define desc_case_2_1_4 "\
Check that SAD_VML can be performed on one module already loaded on the\n\
system."

int
test_case_2_1_4(int child)
{
#ifdef LFS
	struct str_mlist sml = { "sth" };
#endif
#ifdef LIS
	struct str_mlist sml = { "relay" };
#endif
	struct str_list sl = { 1, &sml };

	if (test_ioctl(child, SAD_VML, (intptr_t) &sl) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_4 = { &preamble_0, &test_case_2_1_4, &postamble_0 };

#define test_case_2_1_4_stream_0 (&test_2_1_4)
#define test_case_2_1_4_stream_1 (NULL)
#define test_case_2_1_4_stream_2 (NULL)

#define tgrp_case_2_2_1 test_group_2
#define numb_case_2_2_1 "2.2.1"
#define name_case_2_2_1 "Perform SAD_GAP - EFAULT."
#define sref_case_2_2_1 sref_group_2
#define desc_case_2_2_1 "\
Check that SAD_GAP can be performed.  Checks that EFAULT is returned\n\
when arg is NULL or points outside the callers address space."

int
test_case_2_2_1(int child)
{
	if (test_ioctl(child, SAD_GAP, (intptr_t) NULL) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_GAP, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_1 = { &preamble_0, &test_case_2_2_1, &postamble_0 };

#define test_case_2_2_1_stream_0 (&test_2_2_1)
#define test_case_2_2_1_stream_1 (NULL)
#define test_case_2_2_1_stream_2 (NULL)

#define tgrp_case_2_2_2 test_group_2
#define numb_case_2_2_2 "2.2.2"
#define name_case_2_2_2 "Perform SAD_GAP - EINVAL."
#define sref_case_2_2_2 sref_group_2
#define desc_case_2_2_2 "\
Check that SAD_GAP can be performed.  Checks that EINVAL is returned\n\
the device specified by sap_major and sap_minor is invalid."

int
test_case_2_2_2(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_ALL, 0, -1, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_ALL, 0, -1, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (test_ioctl(child, SAD_GAP, (intptr_t) &sap) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_2 = { &preamble_0, &test_case_2_2_2, &postamble_0 };

#define test_case_2_2_2_stream_0 (&test_2_2_2)
#define test_case_2_2_2_stream_1 (NULL)
#define test_case_2_2_2_stream_2 (NULL)

#define tgrp_case_2_2_3 test_group_2
#define numb_case_2_2_3 "2.2.3"
#define name_case_2_2_3 "Perform SAD_GAP - ENOSTR."
#define sref_case_2_2_3 sref_group_2
#define desc_case_2_2_3 "\
Check that SAD_GAP can be performed.  Checks that ENOSTR is returned\n\
when the device specified by sap_major and sap_minor is not a STREAMS\n\
device."

int
test_case_2_2_3(int child)
{
#ifdef LFS
	struct strapush sap = { 0, 136, 5, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, };
#endif
#ifdef LIS
	struct strapush sap = { { 0, 136, 5, 1, 1, },  { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (test_ioctl(child, SAD_GAP, (intptr_t) &sap) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENOSTR)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_3 = { &preamble_0, &test_case_2_2_3, &postamble_0 };

#define test_case_2_2_3_stream_0 (&test_2_2_3)
#define test_case_2_2_3_stream_1 (NULL)
#define test_case_2_2_3_stream_2 (NULL)

#define tgrp_case_2_2_4 test_group_2
#define numb_case_2_2_4 "2.2.4"
#define name_case_2_2_4 "Perform SAD_GAP - ENODEV."
#define sref_case_2_2_4 sref_group_2
#define desc_case_2_2_4 "\
Check that SAD_GAP can be performed.  Checks that ENODEV is returned\n\
when the device specified by sap_major and sap_minor is not configured\n\
for autopush."

int
test_case_2_2_4(int child)
{
#ifdef LFS
	struct strapush sap = { 0, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { 0, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (test_ioctl(child, SAD_GAP, (intptr_t) &sap) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENODEV)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_4 = { &preamble_0, &test_case_2_2_4, &postamble_0 };

#define test_case_2_2_4_stream_0 (&test_2_2_4)
#define test_case_2_2_4_stream_1 (NULL)
#define test_case_2_2_4_stream_2 (NULL)

#define tgrp_case_2_2_5 test_group_2
#define numb_case_2_2_5 "2.2.5"
#define name_case_2_2_5 "Perform SAD_GAP."
#define sref_case_2_2_5 sref_group_2
#define desc_case_2_2_5 "\
Check that SAD_GAP can be performed."

int
preamble_test_case_2_2_5(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_ALL, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_ALL, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_2_5(int child)
{
#ifdef LFS
	struct strapush sap2 = { 0, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap2 = { { 0, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (test_ioctl(child, SAD_GAP, (intptr_t) &sap2) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
postamble_test_case_2_2_5(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_CLEAR, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_CLEAR, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) != __RESULT_SUCCESS)
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

#define tgrp_case_2_3_1 test_group_2
#define numb_case_2_3_1 "2.3.1"
#define name_case_2_3_1 "Perform SAD_SAP - EFAULT."
#define sref_case_2_3_1 sref_group_2
#define desc_case_2_3_1 "\
Check that SAD_SAP can be performed.  Checks that EFAULT is returned\n\
when arg is NULL or points outside the callers address space."

int
test_case_2_3_1(int child)
{
	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	if (test_ioctl(child, SAD_SAP, (intptr_t) NULL) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_1 = { &preamble_0, &test_case_2_3_1, &postamble_0 };

#define test_case_2_3_1_stream_0 (&test_2_3_1)
#define test_case_2_3_1_stream_1 (NULL)
#define test_case_2_3_1_stream_2 (NULL)

#define tgrp_case_2_3_2 test_group_2
#define numb_case_2_3_2 "2.3.2"
#define name_case_2_3_2 "Perform SAD_SAP - EINVAL."
#define sref_case_2_3_2 sref_group_2
#define desc_case_2_3_2 "\
Check that SAD_SAP can be performed.  Checks that EINVAL is returned\n\
when sap_cmd, sap_major, sap_minor or sap_lastminor was invalid."

int
test_case_2_3_2(int child)
{
#ifdef LFS
	struct strapush sap = { 5, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
	struct strapush sap1 = { SAP_CLEAR, 0, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, };
	struct strapush sap2 = { SAP_CLEAR, -1, -1, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#if 0
	struct strapush sap3 = { SAP_CLEAR, -1, 4, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#endif
#ifdef LIS
	struct strapush sap = { { 5, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
	struct strapush sap1 = { { SAP_CLEAR, 0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
	struct strapush sap2 = { { SAP_CLEAR, SAD__CMAJOR_0, -1, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#if 0
	struct strapush sap3 = { { SAP_CLEAR, SAD__CMAJOR_0, 4, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap2) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
#if 0
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap3) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
#endif
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_2 = { &preamble_0, &test_case_2_3_2, &postamble_0 };

#define test_case_2_3_2_stream_0 (&test_2_3_2)
#define test_case_2_3_2_stream_1 (NULL)
#define test_case_2_3_2_stream_2 (NULL)

#define tgrp_case_2_3_3 test_group_2
#define numb_case_2_3_3 "2.3.3"
#define name_case_2_3_3 "Perform SAD_SAP - EINVAL."
#define sref_case_2_3_3 sref_group_2
#define desc_case_2_3_3 "\
Check that SAD_SAP can be performed.  Checks that EINVAL is returned\n\
when sap_nlist is less than one or greater than MAXAPUSH."

int
test_case_2_3_3(int child)
{
#ifdef LFS
	struct strapush sap1 = { SAP_ALL, -1, 0, 1, 0, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
	struct strapush sap2 = { SAP_ALL, -1, 0, 1, MAXAPUSH + 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap1 = { { SAP_ALL, SAD__CMAJOR_0, 0, 1, 0, }, { CONFIG_STREAMS_SC_NAME, }, };
	struct strapush sap2 = { { SAP_ALL, SAD__CMAJOR_0, 0, 1, MAXAPUSH + 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap1) == __RESULT_SUCCESS) {
		sap1.sap_cmd = SAP_CLEAR;
		test_ioctl(child, SAD_SAP, (intptr_t) &sap1);
		return (__RESULT_FAILURE);
	}
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap2) == __RESULT_SUCCESS) {
		sap2.sap_cmd = SAP_CLEAR;
		test_ioctl(child, SAD_SAP, (intptr_t) &sap2);
		return (__RESULT_FAILURE);
	}
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_3 = { &preamble_0, &test_case_2_3_3, &postamble_0 };

#define test_case_2_3_3_stream_0 (&test_2_3_3)
#define test_case_2_3_3_stream_1 (NULL)
#define test_case_2_3_3_stream_2 (NULL)

#define tgrp_case_2_3_4 test_group_2
#define numb_case_2_3_4 "2.3.4"
#define name_case_2_3_4 "Perform SAD_SAP - EINVAL."
#define sref_case_2_3_4 sref_group_2
#define desc_case_2_3_4 "\
Check that SAD_SAP can be performed.  Checks that EINVAL is returned\n\
when sap_list contained invalid information (names are null or not null\n\
terminated)."

int
test_case_2_3_4(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_ALL, -1, 0, 1, 1, { "", }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_ALL, SAD__CMAJOR_0, 0, 1, 1, }, { "", }, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_4 = { &preamble_0, &test_case_2_3_4, &postamble_0 };

#define test_case_2_3_4_stream_0 (&test_2_3_4)
#define test_case_2_3_4_stream_1 (NULL)
#define test_case_2_3_4_stream_2 (NULL)

#define tgrp_case_2_3_5 test_group_2
#define numb_case_2_3_5 "2.3.5"
#define name_case_2_3_5 "Perform SAD_SAP - ENOSTR."
#define sref_case_2_3_5 sref_group_2
#define desc_case_2_3_5 "\
Check that SAD_SAP can be performed.  Checks that ENOSTR is returned\n\
when sap_major, sap_minor and sap_lastminor specify a character device\n\
that is not a STREAMS device."

int
test_case_2_3_5(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_ALL, 137, 5, 6, 1, { CONFIG_STREAMS_SC_NAME, }, 0, };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_ALL, 137, 5, 6, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENOSTR)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_5 = { &preamble_0, &test_case_2_3_5, &postamble_0 };

#define test_case_2_3_5_stream_0 (&test_2_3_5)
#define test_case_2_3_5_stream_1 (NULL)
#define test_case_2_3_5_stream_2 (NULL)

#define tgrp_case_2_3_6 test_group_2
#define numb_case_2_3_6 "2.3.6"
#define name_case_2_3_6 "Perform SAD_SAP - EEXIST."
#define sref_case_2_3_6 sref_group_2
#define desc_case_2_3_6 "\
Check that SAD_SAP can be performed.  Checks that EEXIST is returned\n\
when sap_module, sap_major, sap_minor and sap_lastminor specify a\n\
STREAMS device that is already configured for autopush and the sap_cmd\n\
was SAP_ONE, SAP_RANGE or SAP_ALL."

int
preamble_test_case_2_3_6(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_ALL, -1, 0, 1, 1, {CONFIG_STREAMS_SC_NAME,}, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_ALL, SAD__CMAJOR_0, 0, 1, 1, }, {CONFIG_STREAMS_SC_NAME,}, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	state++;
	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) & sap) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_3_6(int child)
{
#ifdef LFS
	struct strapush sap1 = { SAP_ONE, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
	struct strapush sap2 = { SAP_RANGE, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
	struct strapush sap3 = { SAP_ALL, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap1 = { { SAP_ONE, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
	struct strapush sap2 = { { SAP_RANGE, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
	struct strapush sap3 = { { SAP_ALL, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EEXIST)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap2) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EEXIST)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap3) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EEXIST)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
postamble_test_case_2_3_6(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_CLEAR, -1, 0, 1, 1, {CONFIG_STREAMS_SC_NAME,}, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_CLEAR, SAD__CMAJOR_0, 0, 1, 1, }, {CONFIG_STREAMS_SC_NAME,}, };
#endif
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, SAD_SAP, (intptr_t) & sap) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
}
struct test_stream test_2_3_6 = { &preamble_test_case_2_3_6, &test_case_2_3_6, &postamble_test_case_2_3_6 };

#define test_case_2_3_6_stream_0 (&test_2_3_6)
#define test_case_2_3_6_stream_1 (NULL)
#define test_case_2_3_6_stream_2 (NULL)

#define tgrp_case_2_3_7 test_group_2
#define numb_case_2_3_7 "2.3.7"
#define name_case_2_3_7 "Perform SAD_SAP - ERANGE."
#define sref_case_2_3_7 sref_group_2
#define desc_case_2_3_7 "\
Check that SAD_SAP can be performed.  Checks that ERANGE is returned\n\
when SAP_CLEAR is attempted on a subrange of a range previously set with\n\
SAP_RANGE."

int
preamble_test_case_2_3_7(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_RANGE, -1, 0, 2, 1, {CONFIG_STREAMS_SC_NAME,}, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_RANGE, SAD__CMAJOR_0, 0, 2, 1, }, {CONFIG_STREAMS_SC_NAME,}, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	state++;
	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_2_3_7(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_CLEAR, -1, 1, 2, 1, {CONFIG_STREAMS_SC_NAME,}, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_CLEAR, SAD__CMAJOR_0, 1, 2, 1, }, {CONFIG_STREAMS_SC_NAME,}, };
#endif

	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

int
postamble_test_case_2_3_7(int child)
{
#ifdef LFS
	struct strapush sap = { SAP_CLEAR, -1, 0, 2, 1, {CONFIG_STREAMS_SC_NAME,}, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap = { { SAP_CLEAR, SAD__CMAJOR_0, 0, 2, 1, }, {CONFIG_STREAMS_SC_NAME,}, };
#endif
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
}
struct test_stream test_2_3_7 = { &preamble_test_case_2_3_7, &test_case_2_3_7, &postamble_test_case_2_3_7 };

#define test_case_2_3_7_stream_0 (&test_2_3_7)
#define test_case_2_3_7_stream_1 (NULL)
#define test_case_2_3_7_stream_2 (NULL)

#define tgrp_case_2_3_8 test_group_2
#define numb_case_2_3_8 "2.3.8"
#define name_case_2_3_8 "Perform SAD_SAP - ERANGE."
#define sref_case_2_3_8 sref_group_2
#define desc_case_2_3_8 "\
Check that SAD_SAP can be performed.  Checks that ERANGE is returned\n\
when sap_lastminor is less than or equal to sap_minor and sap_cmd was\n\
SAP_RANGE."

int
test_case_2_3_8(int child)
{
#ifdef LFS
	struct strapush sap1 = { SAP_RANGE, -1, 1, 0, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
	struct strapush sap2 = { SAP_RANGE, -1, 1, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap1 = { { SAP_RANGE, SAD__CMAJOR_0, 1, 0, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
	struct strapush sap2 = { { SAP_RANGE, SAD__CMAJOR_0, 1, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap2) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_8 = { &preamble_0, &test_case_2_3_8, &postamble_0 };

#define test_case_2_3_8_stream_0 (&test_2_3_8)
#define test_case_2_3_8_stream_1 (NULL)
#define test_case_2_3_8_stream_2 (NULL)

#define tgrp_case_2_3_9 test_group_2
#define numb_case_2_3_9 "2.3.9"
#define name_case_2_3_9 "Perform SAD_SAP - ENODEV."
#define sref_case_2_3_9 sref_group_2
#define desc_case_2_3_9 "\
Check that SAD_SAP can be performed.  Checks that ENODEV is returned\n\
when the device specified by sap_major and sap_minor is not configured\n\
with an autopush list and the sap_cmd was SAP_CLEAR."

int
test_case_2_3_9(int child)
{
#ifdef LFS
	struct strapush sap1 = { SAP_CLEAR, -1, 0, 1, 1, { CONFIG_STREAMS_SC_NAME, }, 0, CONFIG_STREAMS_SAD_NAME };
#endif
#ifdef LIS
	struct strapush sap1 = { { SAP_CLEAR, SAD__CMAJOR_0, 0, 1, 1, }, { CONFIG_STREAMS_SC_NAME, }, };
#endif

	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	if (test_ioctl(child, SAD_SAP, (intptr_t) &sap1) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != ENODEV)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_9 = { &preamble_0, &test_case_2_3_9, &postamble_0 };

#define test_case_2_3_9_stream_0 (&test_2_3_9)
#define test_case_2_3_9_stream_1 (NULL)
#define test_case_2_3_9_stream_2 (NULL)

#define tgrp_case_2_3_10 test_group_2
#define numb_case_2_3_10 "2.3.10"
#define name_case_2_3_10 "Perform SAD_SAP - ENOSR."
#define sref_case_2_3_10 sref_group_2
#define desc_case_2_3_10 "\
Check that SAD_SAP can be performed.  Checks that ENOSR is returned\n\
when resources could not be allocated to complete the command."

int
test_case_2_3_10(int child)
{
	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	return (__RESULT_SKIPPED);
}
struct test_stream test_2_3_10 = { &preamble_0, &test_case_2_3_10, &postamble_0 };

#define test_case_2_3_10_stream_0 (&test_2_3_10)
#define test_case_2_3_10_stream_1 (NULL)
#define test_case_2_3_10_stream_2 (NULL)

#define tgrp_case_2_3_11 test_group_2
#define numb_case_2_3_11 "2.3.11"
#define name_case_2_3_11 "Perform SAD_SAP."
#define sref_case_2_3_11 sref_group_2
#define desc_case_2_3_11 "\
Check that SAD_SAP can be performed."

int
test_case_2_3_11(int child)
{
	if (getuid() != 0 && geteuid() != 0)
		return (__RESULT_SKIPPED);
	return (__RESULT_SKIPPED);
}
struct test_stream test_2_3_11 = { &preamble_0, &test_case_2_3_11, &postamble_0 };

#define test_case_2_3_11_stream_0 (&test_2_3_11)
#define test_case_2_3_11_stream_1 (NULL)
#define test_case_2_3_11_stream_2 (NULL)


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
	failure_string = NULL;
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
		failure_string = NULL;
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
		failure_string = NULL;
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
	pid_t this_child, child[3] = { 0, };
	int this_status, status[3] = { 0, };

	if (start_tt(test_duration) != __RESULT_SUCCESS)
		goto inconclusive;
	if (stream[2]) {
		switch ((child[2] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(2, stream[2]));	/* execute stream[2] state machine */
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
	for (; children > 0; children--) {
	      waitagain:
		if ((this_child = wait(&this_status)) > 0) {
			if (WIFEXITED(this_status)) {
				if (this_child == child[0]) {
					child[0] = 0;
					if ((status[0] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (child[1])
							kill(child[1], SIGKILL);
						if (child[2])
							kill(child[2], SIGKILL);
					}
				}
				if (this_child == child[1]) {
					child[1] = 0;
					if ((status[1] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (child[0])
							kill(child[0], SIGKILL);
						if (child[2])
							kill(child[2], SIGKILL);
					}
				}
				if (this_child == child[2]) {
					child[2] = 0;
					if ((status[2] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (child[0])
							kill(child[0], SIGKILL);
						if (child[1])
							kill(child[1], SIGKILL);
					}
				}
			} else if (WIFSIGNALED(this_status)) {
				int signal = WTERMSIG(this_status);

				if (this_child == child[0]) {
					print_terminated(0, signal);
					if (child[1])
						kill(child[1], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[0] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					child[0] = 0;
				}
				if (this_child == child[1]) {
					print_terminated(1, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[1] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					child[1] = 0;
				}
				if (this_child == child[2]) {
					print_terminated(2, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[1])
						kill(child[1], SIGKILL);
					status[2] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					child[2] = 0;
				}
			} else if (WIFSTOPPED(this_status)) {
				int signal = WSTOPSIG(this_status);

				if (this_child == child[0]) {
					print_stopped(0, signal);
					if (child[0])
						kill(child[0], SIGKILL);
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
					if (child[1])
						kill(child[1], SIGKILL);
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
					if (child[2])
						kill(child[2], SIGKILL);
					status[2] = __RESULT_FAILURE;
					child[2] = 0;
				}
			}
		} else {
			if (timer_timeout) {
				timer_timeout = 0;
				print_timeout(3);
			}
			if (child[0])
				kill(child[0], SIGKILL);
			if (child[1])
				kill(child[1], SIGKILL);
			if (child[2])
				kill(child[2], SIGKILL);
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
	const char *sgrp;		/* test case subgroup */
	const char *name;		/* test case name */
	const char *xtra;		/* test case extra information */
	const char *desc;		/* test case description */
	const char *sref;		/* test case standards section reference */
	struct test_stream *stream[3];	/* test streams */
	int (*start) (int);		/* start function */
	int (*stop) (int);		/* stop function */
	int run;			/* whether to run this test */
	int result;			/* results of test */
	int expect;			/* expected result */
} tests[] = {
	{
		numb_case_0_1, tgrp_case_0_1, sgrp_case_0_1, name_case_0_1, xtra_case_0_1, desc_case_0_1, sref_case_0_1, {
	test_case_0_1_stream_0, test_case_0_1_stream_1, test_case_0_1_stream_2}, &begin_sanity, &end_sanity, 0, 0, __RESULT_INCONCLUSIVE}, {
		numb_case_1_1, tgrp_case_1_1, NULL, name_case_1_1, NULL, desc_case_1_1, sref_case_1_1, {
	test_case_1_1_stream_0, test_case_1_1_stream_1, test_case_1_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_1_2, tgrp_case_1_2, NULL, name_case_1_2, NULL, desc_case_1_2, sref_case_1_2, {
	test_case_1_2_stream_0, test_case_1_2_stream_1, test_case_1_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_1_3, tgrp_case_1_3, NULL, name_case_1_3, NULL, desc_case_1_3, sref_case_1_3, {
	test_case_1_3_stream_0, test_case_1_3_stream_1, test_case_1_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_1, tgrp_case_2_1_1, NULL, name_case_2_1_1, NULL, desc_case_2_1_1, sref_case_2_1_1, {
	test_case_2_1_1_stream_0, test_case_2_1_1_stream_1, test_case_2_1_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_2, tgrp_case_2_1_2, NULL, name_case_2_1_2, NULL, desc_case_2_1_2, sref_case_2_1_2, {
	test_case_2_1_2_stream_0, test_case_2_1_2_stream_1, test_case_2_1_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_3, tgrp_case_2_1_3, NULL, name_case_2_1_3, NULL, desc_case_2_1_3, sref_case_2_1_3, {
	test_case_2_1_3_stream_0, test_case_2_1_3_stream_1, test_case_2_1_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_4, tgrp_case_2_1_4, NULL, name_case_2_1_4, NULL, desc_case_2_1_4, sref_case_2_1_4, {
	test_case_2_1_4_stream_0, test_case_2_1_4_stream_1, test_case_2_1_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_1, tgrp_case_2_2_1, NULL, name_case_2_2_1, NULL, desc_case_2_2_1, sref_case_2_2_1, {
	test_case_2_2_1_stream_0, test_case_2_2_1_stream_1, test_case_2_2_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_2, tgrp_case_2_2_2, NULL, name_case_2_2_2, NULL, desc_case_2_2_2, sref_case_2_2_2, {
	test_case_2_2_2_stream_0, test_case_2_2_2_stream_1, test_case_2_2_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_3, tgrp_case_2_2_3, NULL, name_case_2_2_3, NULL, desc_case_2_2_3, sref_case_2_2_3, {
	test_case_2_2_3_stream_0, test_case_2_2_3_stream_1, test_case_2_2_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_4, tgrp_case_2_2_4, NULL, name_case_2_2_4, NULL, desc_case_2_2_4, sref_case_2_2_4, {
	test_case_2_2_4_stream_0, test_case_2_2_4_stream_1, test_case_2_2_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_5, tgrp_case_2_2_5, NULL, name_case_2_2_5, NULL, desc_case_2_2_5, sref_case_2_2_5, {
	test_case_2_2_5_stream_0, test_case_2_2_5_stream_1, test_case_2_2_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_1, tgrp_case_2_3_1, NULL, name_case_2_3_1, NULL, desc_case_2_3_1, sref_case_2_3_1, {
	test_case_2_3_1_stream_0, test_case_2_3_1_stream_1, test_case_2_3_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_2, tgrp_case_2_3_2, NULL, name_case_2_3_2, NULL, desc_case_2_3_2, sref_case_2_3_2, {
	test_case_2_3_2_stream_0, test_case_2_3_2_stream_1, test_case_2_3_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_3, tgrp_case_2_3_3, NULL, name_case_2_3_3, NULL, desc_case_2_3_3, sref_case_2_3_3, {
	test_case_2_3_3_stream_0, test_case_2_3_3_stream_1, test_case_2_3_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_4, tgrp_case_2_3_4, NULL, name_case_2_3_4, NULL, desc_case_2_3_4, sref_case_2_3_4, {
	test_case_2_3_4_stream_0, test_case_2_3_4_stream_1, test_case_2_3_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_5, tgrp_case_2_3_5, NULL, name_case_2_3_5, NULL, desc_case_2_3_5, sref_case_2_3_5, {
	test_case_2_3_5_stream_0, test_case_2_3_5_stream_1, test_case_2_3_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_6, tgrp_case_2_3_6, NULL, name_case_2_3_6, NULL, desc_case_2_3_6, sref_case_2_3_6, {
	test_case_2_3_6_stream_0, test_case_2_3_6_stream_1, test_case_2_3_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_7, tgrp_case_2_3_7, NULL, name_case_2_3_7, NULL, desc_case_2_3_7, sref_case_2_3_7, {
	test_case_2_3_7_stream_0, test_case_2_3_7_stream_1, test_case_2_3_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_8, tgrp_case_2_3_8, NULL, name_case_2_3_8, NULL, desc_case_2_3_8, sref_case_2_3_8, {
	test_case_2_3_8_stream_0, test_case_2_3_8_stream_1, test_case_2_3_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_9, tgrp_case_2_3_9, NULL, name_case_2_3_9, NULL, desc_case_2_3_9, sref_case_2_3_9, {
	test_case_2_3_9_stream_0, test_case_2_3_9_stream_1, test_case_2_3_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_10, tgrp_case_2_3_10, NULL, name_case_2_3_10, NULL, desc_case_2_3_10, sref_case_2_3_10, {
	test_case_2_3_10_stream_0, test_case_2_3_10_stream_1, test_case_2_3_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_3_11, tgrp_case_2_3_11, NULL, name_case_2_3_11, NULL, desc_case_2_3_11, sref_case_2_3_11, {
	test_case_2_3_11_stream_0, test_case_2_3_11_stream_1, test_case_2_3_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
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
	int repeat = 0;
	int oldverbose = verbose;

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
		      rerun:
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
				if (verbose > 1 && tests[i].tgrp)
					fprintf(stdout, "\nTest Group: %s", tests[i].tgrp);
				if (verbose > 1 && tests[i].sgrp)
					fprintf(stdout, "\nTest Subgroup: %s", tests[i].sgrp);
				if (tests[i].xtra)
					fprintf(stdout, "\nTest Case %s-%s/%s: %s (%s)\n", sstdname, shortname, tests[i].numb, tests[i].name, tests[i].xtra);
				else
					fprintf(stdout, "\nTest Case %s-%s/%s: %s\n", sstdname, shortname, tests[i].numb, tests[i].name);
				if (verbose > 1 && tests[i].sref)
					fprintf(stdout, "Test Reference: %s\n", tests[i].sref);
				if (verbose > 1 && tests[i].desc)
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
				if (result == tests[i].expect) {
					switch (result) {
					case __RESULT_SUCCESS:
					case __RESULT_NOTAPPL:
					case __RESULT_SKIPPED:
						/* autotest can handle these */
						break;
					default:
					case __RESULT_INCONCLUSIVE:
					case __RESULT_FAILURE:
						/* these are expected failures */
						result = __RESULT_SUCCESS;
						break;
					}
				}
			} else {
				if (result == tests[i].expect) {
					switch (result) {
					case __RESULT_SUCCESS:
					case __RESULT_NOTAPPL:
					case __RESULT_SKIPPED:
						/* autotest can handle these */
						break;
					default:
					case __RESULT_INCONCLUSIVE:
					case __RESULT_FAILURE:
						/* these are expected failures */
						result = __RESULT_SUCCESS;
						break;
					}
				}
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
				if (!repeat_verbose || repeat)
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
				if (!repeat_verbose || repeat)
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
			if (repeat_on_failure && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE))
				goto rerun;
			if (repeat_on_success && (result == __RESULT_SUCCESS))
				goto rerun;
			if (repeat) {
				repeat = 0;
				verbose = oldverbose;
			} else if (repeat_verbose && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE)) {
				repeat = 1;
				verbose = 5;
				goto rerun;
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
Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>\n\
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
    Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved.\n\
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
    -a, --again\n\
        repeat failed tests verbose.\n\
    -w, --wait\n\
        have server wait indefinitely.\n\
    -r, --repeat\n\
        repeat test cases on success or failure.\n\
    -R, --repeat-fail\n\
        repeat test cases on failure.\n\
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
			{"again",	no_argument,		NULL, 'a'},
			{"wait",	no_argument,		NULL, 'w'},
			{"repeat",	no_argument,		NULL, 'r'},
			{"repeat-fail",	no_argument,		NULL, 'R'},
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

		c = getopt_long(argc, argv, "awrRd:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "awrRd:el::f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'a':	/* --again */
			repeat_verbose = 1;
			break;
		case 'w':	/* --wait */
			test_duration = INFINITE_WAIT;
			break;
		case 'r':	/* --repeat */
			repeat_on_success = 1;
			repeat_on_failure = 1;
			break;
		case 'R':	/* --repeat-fail */
			repeat_on_failure = 1;
			break;
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
						if (verbose > 2 && t->tgrp)
							fprintf(stdout, "Test Group: %s\n", t->tgrp);
						if (verbose > 2 && t->sgrp)
							fprintf(stdout, "Test Subgroup: %s\n", t->sgrp);
						if (t->xtra)
							fprintf(stdout, "Test Case %s-%s/%s: %s (%s)\n", sstdname, shortname, t->numb, t->name, t->xtra);
						else
							fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
						if (verbose > 2 && t->sref)
							fprintf(stdout, "Test Reference: %s\n", t->sref);
						if (verbose > 1 && t->desc)
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
					if (verbose > 2 && t->tgrp)
						fprintf(stdout, "Test Group: %s\n", t->tgrp);
					if (verbose > 2 && t->sgrp)
						fprintf(stdout, "Test Subgroup: %s\n", t->sgrp);
					if (t->xtra)
						fprintf(stdout, "Test Case %s-%s/%s: %s (%s)\n", sstdname, shortname, t->numb, t->name, t->xtra);
					else
						fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
					if (verbose > 2 && t->sref)
						fprintf(stdout, "Test Reference: %s\n", t->sref);
					if (verbose > 1 && t->desc)
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
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n", (long) timer_scale);
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
