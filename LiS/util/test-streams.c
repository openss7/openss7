/*****************************************************************************

 @(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/03/15 02:33:22 $

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

 Last Modified $Date: 2007/03/15 02:33:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-streams.c,v $
 Revision 0.9.2.8  2007/03/15 02:33:22  brian
 - report failed expectations on test cases

 Revision 0.9.2.7  2007/03/11 05:15:48  brian
 - rationalized test cases back to Linux Fast-STREAMS

 Revision 0.9.2.6  2006/03/05 04:03:14  brian
 - changes primarily for fc4 x86_64 gcc 4.0.4 2.6.15 SMP
 - updates for new release

 Revision 0.9.2.5  2006/02/20 11:05:59  brian
 - gcc 4.0.2 does not like (-1UL) on 64 bit

 Revision 0.9.2.4  2005/12/28 09:53:35  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.3  2005/12/09 04:49:40  brian
 - new test cases

 Revision 0.9.2.2  2005/11/01 11:20:34  brian
 - updates for testing and documentation

 Revision 0.9.2.1  2005/10/23 05:01:27  brian
 - test programs and modules for POSIX testing

 Revision 0.9.2.54  2005/10/14 12:26:54  brian
 - SC module and scls utility tested

 Revision 0.9.2.53  2005/10/13 10:58:54  brian
 - working up testing of sad(4) and sc(4)

 Revision 0.9.2.52  2005/10/09 20:23:02  brian
 - some simple corrections to message queueing and test cases

 Revision 0.9.2.51  2005/10/08 11:21:44  brian
 - complete tests for write/putmsg/putpmsg

 Revision 0.9.2.50  2005/10/08 04:55:42  brian
 - passing full set of getmsg/getpmsg tests

 Revision 0.9.2.49  2005/10/08 01:51:25  brian
 - getmsg processing

 Revision 0.9.2.48  2005/10/08 01:01:01  brian
 - corrected getmsg behavior to tests

 Revision 0.9.2.47  2005/10/07 09:34:32  brian
 - more testing and corrections

 Revision 0.9.2.46  2005/10/07 04:12:21  brian
 - I_PEEK and I_FDINSERT test cases and debugging

 Revision 0.9.2.45  2005/10/06 10:25:34  brian
 - fixed backenabling, flushing, working up I_PEEK

 Revision 0.9.2.44  2005/10/06 08:05:53  brian
 - fixed poll tests and poll bugs

 Revision 0.9.2.43  2005/10/05 09:25:40  brian
 - poll tests, some noxious problem still with poll

 Revision 0.9.2.42  2005/10/05 05:50:21  brian
 - read/readv/getmsg/getpmsg/I_RECVFD wait queue testing

 Revision 0.9.2.41  2005/10/05 03:27:23  brian
 - more testing

 Revision 0.9.2.40  2005/10/04 11:51:57  brian
 - a couple of quirks

 Revision 0.9.2.39  2005/10/04 11:39:04  brian
 - finished write/putmsg/putpmsg and isastream

 Revision 0.9.2.38  2005/10/03 17:42:09  brian
 - working up read/write getmsg/putmsg tests

 Revision 0.9.2.37  2005/10/03 04:22:00  brian
 - read/readv/getmsg/getpmsg testing

 Revision 0.9.2.36  2005/10/02 10:34:53  brian
 - staring full read/write getmsg/putmsg testing

 Revision 0.9.2.35  2005/10/02 01:51:12  brian
 - getpmsg testing and putpmsg testing, sync

 Revision 0.9.2.34  2005/09/30 09:54:37  brian
 - trying to get I_RECVFD working, sync

 Revision 0.9.2.33  2005/09/30 08:27:06  brian
 - I_SENDFD tests complete

 Revision 0.9.2.32  2005/09/29 23:08:20  brian
 - starting testing of FIFOs

 Revision 0.9.2.31  2005/09/29 08:08:16  brian
 - fixed multiplexor dismantling bugs

 Revision 0.9.2.30  2005/09/29 06:48:21  brian
 - added multiplexing test cases

 Revision 0.9.2.29  2005/09/29 00:11:42  brian
 - added final test cases for HUP, RDERR and WRERR

 Revision 0.9.2.29  2005/09/29 00:08:43  brian
 - added final test cases for HUP, RDERR and WRERR

 Revision 0.9.2.28  2005/09/28 08:27:03  brian
 - more HUP and ERR test cases

 Revision 0.9.2.27  2005/09/28 05:57:01  brian
 - additional STRHUP, STRDERR and STWRERR test cases

 Revision 0.9.2.26  2005/09/27 23:34:25  brian
 - added test cases, tweaked straccess()

 Revision 0.9.2.25  2005/09/27 10:04:20  brian
 - more test cases, runqueues bug

 Revision 0.9.2.24  2005/09/27 03:15:57  brian
 - added ENXIO tests

 Revision 0.9.2.23  2005/09/25 22:52:36  brian
 - added test module and continuing with testing

 Revision 0.9.2.22  2005/09/11 02:40:36  brian
 - preparing for mux tests

 Revision 0.9.2.21  2005/09/10 18:16:42  brian
 - more test build

 Revision 0.9.2.20  2005/09/08 09:37:11  brian
 - corrected error output

 Revision 0.9.2.19  2005/09/08 05:52:43  brian
 - added nullmod module and loop driver
 - corrections during testing
 - many ioctl(2p) test cases work very well now

 Revision 0.9.2.18  2005/09/03 08:12:19  brian
 - updates from testing

 Revision 0.9.2.17  2005/07/18 12:07:06  brian
 - standard indentation

 Revision 0.9.2.12  2005/07/18 12:07:06  brian
 - standard indentation

 Revision 0.9.2.11  2005/07/01 07:29:32  brian
 - updates for LE2005 build

 Revision 0.9.2.10  2005/06/25 07:03:46  brian
 - updated streams tests program

 Revision 0.9.2.9  2005/06/06 12:03:57  brian
 - upgraded test suites

 Revision 0.9.2.8  2005/05/16 10:55:14  brian
 - updated test program

 Revision 0.9.2.7  2005/05/16 10:31:54  brian
 - updates and corrections

 Revision 0.9.2.6  2005/05/14 23:59:16  brian
 - getting autotest up and running

 Revision 0.9.2.5  2005/05/14 08:39:37  brian
 - updated copyright headers

 Revision 0.9.2.4  2005/05/14 08:34:46  brian
 - copyright header correction

 Revision 0.9.2.3  2005/05/12 20:58:48  brian
 - corrections during testing

 Revision 0.9.2.2  2005/01/16 23:09:07  brian
 - Added --copying options.

 Revision 0.9.2.1  2004/08/22 06:17:56  brian
 - Checkin on new working branch.

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

#ident "@(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/03/15 02:33:22 $"

static char const ident[] = "$RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/03/15 02:33:22 $";

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

#ifdef LIS
typedef ulong t_uscalar_t;
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
static const char *shortname = "STREAMS";
#ifdef LFS
static char devname[256] = "/dev/streams/clone/echo";
static char muxname[256] = "/dev/streams/clone/mux";
static char fifoname[256] = "/dev/streams/fifo/0";
#else
static char devname[256] = "/dev/echo";
static char muxname[256] = "/dev/mux";
static char fifoname[256] = "/dev/fifo";
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
	case TM_IOC_IOCTL:
		return ("TM_IOC_IOCTL");
	case TM_IOC_COPYIN:
		return ("TM_IOC_COPYIN");
	case TM_IOC_COPYOUT:
		return ("TM_IOC_COPYOUT");
	case TM_IOC_COPYIO:
		return ("TM_IOC_COPYIO");
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

	if (verbose > 0 && show)
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
			if (last_errno == ERESTART)
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
				if (last_errno == ERESTART)
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
				if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
			if (last_errno == ERESTART)
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
		if (last_errno == ERESTART)
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
		if (last_errno == ERESTART)
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
		if (last_errno == ERESTART)
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

int
preamble_1(int child)
{
	if (!test_fd[child] && test_open(child, muxname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_1(int child)
{
	return postamble_0(child);
}

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

int
preamble_4(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_0(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_4(int child)
{
	int result = __RESULT_SUCCESS;

	if (postamble_0(child + 1) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_1(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
}

/* open a hungup mux */
int
preamble_4_2_1(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_HANGUP, (intptr_t) 0) != __RESULT_SUCCESS && last_errno != ENXIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a mux with rderr */
int
preamble_4_2_2(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_RDERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a mux with wrerr */
int
preamble_4_2_3(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* open a mux with rderr and wrerr */
int
preamble_4_2_4(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_RWERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_4_2(int child)
{
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS && last_errno != ENXIO && last_errno != EIO)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_1(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return result;
}

int
preamble_5(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_1(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child + 1, I_LINK, (intptr_t) test_fd[child]) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_5(int child)
{
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child + 1, I_UNLINK, (intptr_t) MUXID_ALL) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_1(child + 1) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
}

int
preamble_6(int child)
{
	if (!test_fd[child] && test_open(child, fifoname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_6(int child)
{
	return postamble_0(child);
}

int
preamble_6_2_1(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_0(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_HANGUP, (intptr_t) 0) != __RESULT_SUCCESS && last_errno != ENXIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
preamble_6_2_2(int child)
{
	if (preamble_6(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_0(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_RDERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
preamble_6_2_3(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_0(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
preamble_6_2_4(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_0(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_PUSH, (intptr_t) "testmod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, TM_IOC_RWERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_6_2(int child)
{
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS && last_errno != ENXIO && last_errno != EIO)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child + 1) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return result;
}

int
preamble_6_3(int child)
{
	if (preamble_6(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_0(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child, I_SENDFD, (intptr_t) test_fd[child + 1]) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_6_3(int child)
{
	int result = __RESULT_SUCCESS;

	if (postamble_0(child + 1) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_6(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
}

int
preamble_7(int child)
{
	if (preamble_6(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (preamble_1(child + 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_ioctl(child + 1, I_LINK, (intptr_t) test_fd[child]) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

int
postamble_7(int child)
{
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child + 1, I_UNLINK, (intptr_t) MUXID_ALL) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_1(child + 1) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_6(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	return (result);
}

int
preamble_8(int child, int band)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	{
		char buf[1024] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int flags = MSG_BAND;

		last_errno = 0;
		do {
			test_putpmsg(child, &ctl, &dat, band, flags);
			state++;
		}
		while (last_errno != EAGAIN);
	}
	if (test_block(child) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* write queue full of band 0 messages and block */
int
preamble_8_0(int child)
{
	if (preamble_8(child, 0) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* write queue full of band 1 messages and block */
int
preamble_8_1(int child)
{
	if (preamble_8(child, 1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/* flush write queue and close */
int
postamble_8(int child)
{
	int result = __RESULT_SUCCESS;

	if (test_ioctl(child, I_FLUSH, FLUSHW) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	state++;
	if (postamble_0(child) != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
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
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Open and close 1 Stream."
#define sref_case_1_1 sref_none
#define desc_case_1_1 "\
Checks that one Stream can be opened and closed."

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

static const char test_group_2[] = "Perform IOCTL on one Stream";
static const char sref_case_2[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page.";

/*
 *  Perform IOCTL on one Stream - I_NREAD
 */
static const char sref_case_2_1[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_NREAD section.";

#define tgrp_case_2_1_1 test_group_2
#define numb_case_2_1_1 "2.1.1"
#define name_case_2_1_1 "Perform streamio I_NREAD."
#define sref_case_2_1_1 sref_case_2_1
#define desc_case_2_1_1 "\
Checks that I_NREAD can be performed on a Stream.  Because this test is peformed\n\
on a freshly opened Stream, it should return zero (0) as a return value and\n\
return zero (0) in the integer pointed to by arg."

int
test_case_2_1_1(int child)
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
struct test_stream test_2_1_1 = { &preamble_0, &test_case_2_1_1, &postamble_0 };

#define test_case_2_1_1_stream_0 (&test_2_1_1)
#define test_case_2_1_1_stream_1 (NULL)
#define test_case_2_1_1_stream_2 (NULL)

#define tgrp_case_2_1_2 test_group_2
#define numb_case_2_1_2 "2.1.2"
#define name_case_2_1_2 "Perform streamio I_NREAD - EFAULT."
#define sref_case_2_1_2 sref_case_2_1
#define desc_case_2_1_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_1_2(int child)
{
	if (test_ioctl(child, I_NREAD, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
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
#define name_case_2_1_3 "Perform streamio I_NREAD - EINVAL."
#define sref_case_2_1_3 sref_case_2_1
#define desc_case_2_1_3 "\
Checks that EINVAL is returned when the Stream is linked under a Multiplexing Driver."

int
test_case_2_1_3(int child)
{
	int numb = -1;

	if (test_ioctl(child, I_NREAD, (intptr_t) & numb) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_3 = { &preamble_5, &test_case_2_1_3, &postamble_5 };

#define test_case_2_1_3_stream_0 (&test_2_1_3)
#define test_case_2_1_3_stream_1 (NULL)
#define test_case_2_1_3_stream_2 (NULL)

#define tgrp_case_2_1_4 test_group_2
#define numb_case_2_1_4 "2.1.4"
#define name_case_2_1_4 "Perform streamio I_NREAD."
#define sref_case_2_1_4 sref_case_2_1
#define desc_case_2_1_4 "\
Checks that I_NREAD can be performed on a Stream.  This test places a\n\
specfic amount of data on the read queue and then checks that I_NREAD\n\
returns the proper value."

int
test_case_2_1_4(int child)
{
	int numb = -1;
	char buf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(buf), buf };
	struct strbuf dat = { -1, sizeof(buf), buf };

	if (test_putpmsg(child, &ctl, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 0, MSG_HIPRI) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_NREAD, (intptr_t) & numb) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (numb != 16)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 3)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_4 = { &preamble_0, &test_case_2_1_4, &postamble_0 };

#define test_case_2_1_4_stream_0 (&test_2_1_4)
#define test_case_2_1_4_stream_1 (NULL)
#define test_case_2_1_4_stream_2 (NULL)

#define tgrp_case_2_1_5 test_group_2
#define numb_case_2_1_5 "2.1.5"
#define name_case_2_1_5 "Perform streamio I_NREAD."
#define sref_case_2_1_5 sref_case_2_1
#define desc_case_2_1_5 "\
Checks that I_NREAD can be performed on a Stream.  This test places a\n\
specfic amount of data on the read queue and then checks that I_NREAD\n\
returns the proper value."

int
test_case_2_1_5(int child)
{
	int numb = -1;
	struct strbuf dat = { -1, 0, NULL };

	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_NREAD, (intptr_t) & numb) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (numb != 0)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 2)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_5 = { &preamble_0, &test_case_2_1_5, &postamble_0 };

#define test_case_2_1_5_stream_0 (&test_2_1_5)
#define test_case_2_1_5_stream_1 (NULL)
#define test_case_2_1_5_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_PUSH
 */
static const char sref_case_2_2[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_PUSH section.";

#define tgrp_case_2_2_1 test_group_2
#define numb_case_2_2_1 "2.2.1"
#define name_case_2_2_1 "Perform streamio I_PUSH."
#define sref_case_2_2_1 sref_case_2_2
#define desc_case_2_2_1 "\
Checks that I_PUSH can be performed on a Stream."

int
test_case_2_2_1(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") != __RESULT_SUCCESS)
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
#define name_case_2_2_2 "Perform streamio I_PUSH - EINVAL."
#define sref_case_2_2_2 sref_case_2_2
#define desc_case_2_2_2 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EINVAL is\n\
returned when I_PUSH is performed with an invalid module name\n\
\"nomodule\"."

int
test_case_2_2_2(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nomodule") == __RESULT_SUCCESS || last_errno != EINVAL)
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
#define name_case_2_2_3 "Perform streamio I_PUSH - EFAULT."
#define sref_case_2_2_3 "AIX, HP-UX, Solaris, UnixWare Documentation."
#define desc_case_2_2_3 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EFAULT is\n\
returned when arg points outside the caller's address space.  This error\n\
is not documented by POSIX, but it is by AIX, HP-UX, Solaris, and\n\
UnixWare."

int
test_case_2_2_3(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
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
#define name_case_2_2_4 "Perform streamio I_PUSH - EINVAL."
#define sref_case_2_2_4 sref_case_2_2
#define desc_case_2_2_4 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EINVAL is\n\
returned when arg specifies a module name that is too short."

int
test_case_2_2_4(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "") == __RESULT_SUCCESS || last_errno != EINVAL)
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
#define name_case_2_2_5 "Perform streamio I_PUSH - EINVAL."
#define sref_case_2_2_5 sref_case_2_2
#define desc_case_2_2_5 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EINVAL is\n\
returned when arg specifies a module name that is too long."

int
test_case_2_2_5(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "reallylongmodulesname") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_5 = { &preamble_0, &test_case_2_2_5, &postamble_0 };

#define test_case_2_2_5_stream_0 (&test_2_2_5)
#define test_case_2_2_5_stream_1 (NULL)
#define test_case_2_2_5_stream_2 (NULL)

#define tgrp_case_2_2_6 test_group_2
#define numb_case_2_2_6 "2.2.6"
#define name_case_2_2_6 "Perform streamio I_PUSH - EINVAL."
#define sref_case_2_2_6 "SVR 4 STREAMS Programmer's Guide"
#define desc_case_2_2_6 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EINVAL is\n\
returned when a Stream's push limit is reached.  This error case is not\n\
documented by POSIX.  Nor is it documented by AIX.  The SVR 4 SPG says\n\
the number of modules pushed is limited by NSTRPUSH.  OpenSolaris\n\
returns EINVAL here, we follow that lead.  LiS returns ENOSR here, but\n\
does not impose a limit."

int
test_case_2_2_6(int child)
{
	int i;
	for (i = 0; i < 64; i++) {
		if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
	}
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_6 = { &preamble_0, &test_case_2_2_6, &postamble_0 };

#define test_case_2_2_6_stream_0 (&test_2_2_6)
#define test_case_2_2_6_stream_1 (NULL)
#define test_case_2_2_6_stream_2 (NULL)

#define tgrp_case_2_2_7 test_group_2
#define numb_case_2_2_7 "2.2.7"
#define name_case_2_2_7 "Perform streamio I_PUSH - EINVAL."
#define sref_case_2_2_7 sref_case_2_2
#define desc_case_2_2_7 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EINVAL is\n\
returned when a driver is pushed instead of a module."

int
test_case_2_2_7(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "echo") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_7 = { &preamble_0, &test_case_2_2_7, &postamble_0 };

#define test_case_2_2_7_stream_0 (&test_2_2_7)
#define test_case_2_2_7_stream_1 (NULL)
#define test_case_2_2_7_stream_2 (NULL)

#define tgrp_case_2_2_8 test_group_2
#define numb_case_2_2_8 "2.2.8"
#define name_case_2_2_8 "Perform streamio I_PUSH - ENXIO."
#define sref_case_2_2_8 sref_case_2_2
#define desc_case_2_2_8 "\
Checks that I_PUSH can be performed on a Stream.  Checks that ENXIO is\n\
returned when an attempt is made to push a module on a hung up Stream."

int
test_case_2_2_8(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_8 = { &preamble_2_1, &test_case_2_2_8, &postamble_2 };

#define test_case_2_2_8_stream_0 (&test_2_2_8)
#define test_case_2_2_8_stream_1 (NULL)
#define test_case_2_2_8_stream_2 (NULL)

#define tgrp_case_2_2_9 test_group_2
#define numb_case_2_2_9 "2.2.9"
#define name_case_2_2_9 "Perform streamio I_PUSH - EIO RDERR."
#define sref_case_2_2_9 sref_case_2_2
#define desc_case_2_2_9 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EIO is\n\
returned when an attempt is made to push a module on a Stream that has\n\
received a read error."

int
test_case_2_2_9(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") == __RESULT_SUCCESS || last_errno != EIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_9 = { &preamble_2_2, &test_case_2_2_9, &postamble_2 };

#define test_case_2_2_9_stream_0 (&test_2_2_9)
#define test_case_2_2_9_stream_1 (NULL)
#define test_case_2_2_9_stream_2 (NULL)

#define tgrp_case_2_2_10 test_group_2
#define numb_case_2_2_10 "2.2.10"
#define name_case_2_2_10 "Perform streamio I_PUSH - EIO WRERR."
#define sref_case_2_2_10 sref_case_2_2
#define desc_case_2_2_10 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EIO is\n\
returned when an attempt is made to push a module on a Stream that has\n\
received a write error."

int
test_case_2_2_10(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") == __RESULT_SUCCESS || last_errno != EIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_10 = { &preamble_2_3, &test_case_2_2_10, &postamble_2 };

#define test_case_2_2_10_stream_0 (&test_2_2_10)
#define test_case_2_2_10_stream_1 (NULL)
#define test_case_2_2_10_stream_2 (NULL)

#define tgrp_case_2_2_11 test_group_2
#define numb_case_2_2_11 "2.2.11"
#define name_case_2_2_11 "Perform streamio I_PUSH - EIO RDERR WRERR."
#define sref_case_2_2_11 sref_case_2_2
#define desc_case_2_2_11 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EIO is\n\
returned when an attempt is made to push a module on a Stream that has\n\
received a read and write error."

int
test_case_2_2_11(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") == __RESULT_SUCCESS || last_errno != EIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_11 = { &preamble_2_4, &test_case_2_2_11, &postamble_2 };

#define test_case_2_2_11_stream_0 (&test_2_2_11)
#define test_case_2_2_11_stream_1 (NULL)
#define test_case_2_2_11_stream_2 (NULL)

#define tgrp_case_2_2_12 test_group_2
#define numb_case_2_2_12 "2.2.12"
#define name_case_2_2_12 "Perform streamio I_PUSH - EINVAL PLEX."
#define sref_case_2_2_12 sref_case_2_2
#define desc_case_2_2_12 "\
Checks that I_PUSH can be performed on a Stream.  Checks that EINVAL is\n\
returned when an attempt is made to push a module on a Stream is linked\n\
Under a Multiplexing Driver."

int
test_case_2_2_12(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_12 = { &preamble_5, &test_case_2_2_12, &postamble_5 };

#define test_case_2_2_12_stream_0 (&test_2_2_12)
#define test_case_2_2_12_stream_1 (NULL)
#define test_case_2_2_12_stream_2 (NULL)

/* additional negative test cases required:
 *
 * EBADF - fd is not a valid open file descriptor.
 * ENOSTR - fd is not a Stream.
 * EIO - fd refers to a Stream that is closing.
 * EPIPE - fd refers to a Pipe and the other end of the pipe is closed.
 * EINTR - a signal was received before the operation could complete.
 */

/*
 *  Perform IOCTL on one Stream - I_POP
 */
static const char sref_case_2_3[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_POP section.";

#define tgrp_case_2_3_1 test_group_2
#define numb_case_2_3_1 "2.3.1"
#define name_case_2_3_1 "Perform streamio I_POP - EINVAL."
#define sref_case_2_3_1 sref_case_2_3
#define desc_case_2_3_1 "\
Checks that I_POP can be performed on a Stream.  Checks that EINVAL is\n\
when I_POP is attempted on a Stream with no module pushed."

int
test_case_2_3_1(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
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
#define name_case_2_3_2 "Perform streamio I_POP."
#define sref_case_2_3_2 sref_case_2_3
#define desc_case_2_3_2 "\
Checks that I_POP can be performed on a Stream.  Checks that a\n\
module can be popped after being pushed."

int
test_case_2_3_2(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_POP, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_2 = { &preamble_0, &test_case_2_3_2, &postamble_0 };

#define test_case_2_3_2_stream_0 (&test_2_3_2)
#define test_case_2_3_2_stream_1 (NULL)
#define test_case_2_3_2_stream_2 (NULL)

#define tgrp_case_2_3_3 test_group_2
#define numb_case_2_3_3 "2.3.3"
#define name_case_2_3_3 "Perform streamio I_POP - EINVAL."
#define sref_case_2_3_3 sref_case_2_3
#define desc_case_2_3_3 "\
Checks that I_POP can be performed on a Stream.  Checks that EINVAL is\n\
returned when I_POP is peformed twice after a single I_PUSH."

int
test_case_2_3_3(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_POP, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
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
#define name_case_2_3_4 "Perform streamio I_POP - ENXIO."
#define sref_case_2_3_4 sref_case_2_3
#define desc_case_2_3_4 "\
Checks that I_POP can be performed on a Stream.  Checks that ENXIO is\n\
returned when I_POP is peformed on a Stream that is hung up."

int
test_case_2_3_4(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_4 = { &preamble_2_1, &test_case_2_3_4, &postamble_2 };

#define test_case_2_3_4_stream_0 (&test_2_3_4)
#define test_case_2_3_4_stream_1 (NULL)
#define test_case_2_3_4_stream_2 (NULL)

#define tgrp_case_2_3_5 test_group_2
#define numb_case_2_3_5 "2.3.5"
#define name_case_2_3_5 "Perform streamio I_POP - EIO RDERR."
#define sref_case_2_3_5 sref_case_2_3
#define desc_case_2_3_5 "\
Checks that I_POP can be performed on a Stream.  Checks that EIO is\n\
returned when I_POP is peformed on a Stream that has received a read\n\
error."

int
test_case_2_3_5(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_5 = { &preamble_2_2, &test_case_2_3_5, &postamble_2 };

#define test_case_2_3_5_stream_0 (&test_2_3_5)
#define test_case_2_3_5_stream_1 (NULL)
#define test_case_2_3_5_stream_2 (NULL)

#define tgrp_case_2_3_6 test_group_2
#define numb_case_2_3_6 "2.3.6"
#define name_case_2_3_6 "Perform streamio I_POP - EIO WRERR."
#define sref_case_2_3_6 sref_case_2_3
#define desc_case_2_3_6 "\
Checks that I_POP can be performed on a Stream.  Checks that EIO is\n\
returned when I_POP is peformed on a Stream that has received a write\n\
error."

int
test_case_2_3_6(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_6 = { &preamble_2_3, &test_case_2_3_6, &postamble_2 };

#define test_case_2_3_6_stream_0 (&test_2_3_6)
#define test_case_2_3_6_stream_1 (NULL)
#define test_case_2_3_6_stream_2 (NULL)

#define tgrp_case_2_3_7 test_group_2
#define numb_case_2_3_7 "2.3.7"
#define name_case_2_3_7 "Perform streamio I_POP - EIO RDERR WRERR."
#define sref_case_2_3_7 sref_case_2_3
#define desc_case_2_3_7 "\
Checks that I_POP can be performed on a Stream.  Checks that EIO is\n\
returned when I_POP is peformed on a Stream that has receive an error."

int
test_case_2_3_7(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_7 = { &preamble_2_4, &test_case_2_3_7, &postamble_2 };

#define test_case_2_3_7_stream_0 (&test_2_3_7)
#define test_case_2_3_7_stream_1 (NULL)
#define test_case_2_3_7_stream_2 (NULL)

#define tgrp_case_2_3_8 test_group_2
#define numb_case_2_3_8 "2.3.8"
#define name_case_2_3_8 "Perform streamio I_POP - EINVAL PLEX."
#define sref_case_2_3_8 sref_case_2_3
#define desc_case_2_3_8 "\
Checks that I_POP can be performed on a Stream.  Checks that EINVAL\n\
is returned when I_POP is peformed on a Stream is linked under a\n\
Multiplexing Driver."

int
test_case_2_3_8(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3_8 = { &preamble_5, &test_case_2_3_8, &postamble_5 };

#define test_case_2_3_8_stream_0 (&test_2_3_8)
#define test_case_2_3_8_stream_1 (NULL)
#define test_case_2_3_8_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_LOOK
 */
static const char sref_case_2_4[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_LOOK section.";

#define tgrp_case_2_4_1 test_group_2
#define numb_case_2_4_1 "2.4.1"
#define name_case_2_4_1 "Perform streamio I_LOOK."
#define sref_case_2_4_1 sref_case_2_4
#define desc_case_2_4_1 "\
Checks that I_LOOK can be performed on a Stream."

int
test_case_2_4_1(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp("echo", buf, FMNAMESZ))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_1 = { &preamble_0, &test_case_2_4_1, &postamble_0 };

#define test_case_2_4_1_stream_0 (&test_2_4_1)
#define test_case_2_4_1_stream_1 (NULL)
#define test_case_2_4_1_stream_2 (NULL)

#define tgrp_case_2_4_2 test_group_2
#define numb_case_2_4_2 "2.4.2"
#define name_case_2_4_2 "Perform streamio I_LOOK - EINVAL."
#define sref_case_2_4_2 sref_case_2_4
#define desc_case_2_4_2 "\
Checks that I_LOOK can be performed on a Stream.  The command should \n\
fail with EINVAL if there is no module in the Stream.  This is a\n\
behavior only of STREAMS-based pipes and FIFOs.  This test is on a FIFO."

int
test_case_2_4_2(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_2 = { &preamble_6, &test_case_2_4_2, &postamble_6 };

#define test_case_2_4_2_stream_0 (&test_2_4_2)
#define test_case_2_4_2_stream_1 (NULL)
#define test_case_2_4_2_stream_2 (NULL)

#define tgrp_case_2_4_3 test_group_2
#define numb_case_2_4_3 "2.4.3"
#define name_case_2_4_3 "Perform streamio I_LOOK."
#define sref_case_2_4_3 sref_case_2_4
#define desc_case_2_4_3 "\
Checks that I_LOOK can be performed on a Stream.  Pushes a module and\n\
then looks at it.  This test case should succeed."

int
test_case_2_4_3(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp("nullmod", buf, FMNAMESZ))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_3 = { &preamble_3, &test_case_2_4_3, &postamble_3 };

#define test_case_2_4_3_stream_0 (&test_2_4_3)
#define test_case_2_4_3_stream_1 (NULL)
#define test_case_2_4_3_stream_2 (NULL)

#define tgrp_case_2_4_4 test_group_2
#define numb_case_2_4_4 "2.4.4"
#define name_case_2_4_4 "Perform streamio I_LOOK."
#define sref_case_2_4_4 sref_case_2_4
#define desc_case_2_4_4 "\
Checks that I_LOOK can be performed on a Stream even if an hangup has\n\
been received by the Stream."

int
test_case_2_4_4(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp("testmod", buf, FMNAMESZ))
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
#define name_case_2_4_5 "Perform streamio I_LOOK."
#define sref_case_2_4_5 sref_case_2_4
#define desc_case_2_4_5 "\
Checks that I_LOOK can be performed on a Stream even if an read error has\n\
been received by the Stream."

int
test_case_2_4_5(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp("testmod", buf, FMNAMESZ))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_5 = { &preamble_2_2, &test_case_2_4_5, &postamble_2 };

#define test_case_2_4_5_stream_0 (&test_2_4_5)
#define test_case_2_4_5_stream_1 (NULL)
#define test_case_2_4_5_stream_2 (NULL)

#define tgrp_case_2_4_6 test_group_2
#define numb_case_2_4_6 "2.4.6"
#define name_case_2_4_6 "Perform streamio I_LOOK."
#define sref_case_2_4_6 sref_case_2_4
#define desc_case_2_4_6 "\
Checks that I_LOOK can be performed on a Stream even if an write error has\n\
been received by the Stream."

int
test_case_2_4_6(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp("testmod", buf, FMNAMESZ))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_6 = { &preamble_2_3, &test_case_2_4_6, &postamble_2 };

#define test_case_2_4_6_stream_0 (&test_2_4_6)
#define test_case_2_4_6_stream_1 (NULL)
#define test_case_2_4_6_stream_2 (NULL)

#define tgrp_case_2_4_7 test_group_2
#define numb_case_2_4_7 "2.4.7"
#define name_case_2_4_7 "Perform streamio I_LOOK."
#define sref_case_2_4_7 sref_case_2_4
#define desc_case_2_4_7 "\
Checks that I_LOOK can be performed on a Stream even if an error has\n\
been received by the Stream."

int
test_case_2_4_7(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp("testmod", buf, FMNAMESZ))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_7 = { &preamble_2_4, &test_case_2_4_7, &postamble_2 };

#define test_case_2_4_7_stream_0 (&test_2_4_7)
#define test_case_2_4_7_stream_1 (NULL)
#define test_case_2_4_7_stream_2 (NULL)

#define tgrp_case_2_4_8 test_group_2
#define numb_case_2_4_8 "2.4.8"
#define name_case_2_4_8 "Perform streamio I_LOOK - EINVAL."
#define sref_case_2_4_8 sref_case_2_4
#define desc_case_2_4_8 "\
Checks that I_LOOK can be performed on a Stream.  Checks that EINVAL is \n\
returned when I_LOOK is attempted on a Stream that is linked under a \n\
Multiplexing Driver."

int
test_case_2_4_8(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4_8 = { &preamble_5, &test_case_2_4_8, &postamble_5 };

#define test_case_2_4_8_stream_0 (&test_2_4_8)
#define test_case_2_4_8_stream_1 (NULL)
#define test_case_2_4_8_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_FLUSH
 */
static const char sref_case_2_5[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_FLUSH section.";

#define tgrp_case_2_5_1 test_group_2
#define numb_case_2_5_1 "2.5.1"
#define name_case_2_5_1 "Perform streamio I_FLUSH - EINVAL."
#define sref_case_2_5_1 sref_case_2_5
#define desc_case_2_5_1 "\
Checks that I_FLUSH with an invalid argument returns EINVAL."

int
test_case_2_5_1(int child)
{
	if (test_ioctl(child, I_FLUSH, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_1 = { &preamble_0, &test_case_2_5_1, &postamble_0 };

#define test_case_2_5_1_stream_0 (&test_2_5_1)
#define test_case_2_5_1_stream_1 (NULL)
#define test_case_2_5_1_stream_2 (NULL)

#define tgrp_case_2_5_2 test_group_2
#define numb_case_2_5_2 "2.5.2"
#define name_case_2_5_2 "Perform streamio I_FLUSH - EINVAL."
#define sref_case_2_5_2 sref_case_2_5
#define desc_case_2_5_2 "\
Checks that I_FLUSH with an invalid argument returns EINVAL."

int
test_case_2_5_2(int child)
{
	if (test_ioctl(child, I_FLUSH, -1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_2 = { &preamble_0, &test_case_2_5_2, &postamble_0 };

#define test_case_2_5_2_stream_0 (&test_2_5_2)
#define test_case_2_5_2_stream_1 (NULL)
#define test_case_2_5_2_stream_2 (NULL)

#define tgrp_case_2_5_3 test_group_2
#define numb_case_2_5_3 "2.5.3"
#define name_case_2_5_3 "Perform streamio I_FLUSH - FLUSHR."
#define sref_case_2_5_3 sref_case_2_5
#define desc_case_2_5_3 "\
Checks that I_FLUSH can be performed on a Stream with FLUSHR."

int
test_case_2_5_3(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHR) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_3 = { &preamble_0, &test_case_2_5_3, &postamble_0 };

#define test_case_2_5_3_stream_0 (&test_2_5_3)
#define test_case_2_5_3_stream_1 (NULL)
#define test_case_2_5_3_stream_2 (NULL)

#define tgrp_case_2_5_4 test_group_2
#define numb_case_2_5_4 "2.5.4"
#define name_case_2_5_4 "Perform streamio I_FLUSH - FLUSHW."
#define sref_case_2_5_4 sref_case_2_5
#define desc_case_2_5_4 "\
Checks that I_FLUSH can be performed on a Stream with FLUSHW."

int
test_case_2_5_4(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_4 = { &preamble_0, &test_case_2_5_4, &postamble_0 };

#define test_case_2_5_4_stream_0 (&test_2_5_4)
#define test_case_2_5_4_stream_1 (NULL)
#define test_case_2_5_4_stream_2 (NULL)

#define tgrp_case_2_5_5 test_group_2
#define numb_case_2_5_5 "2.5.5"
#define name_case_2_5_5 "Perform streamio I_FLUSH - FLUSHRW."
#define sref_case_2_5_5 sref_case_2_5
#define desc_case_2_5_5 "\
Checks that I_FLUSH can be performed on a Stream with FLUSHRW."

int
test_case_2_5_5(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_5 = { &preamble_0, &test_case_2_5_5, &postamble_0 };

#define test_case_2_5_5_stream_0 (&test_2_5_5)
#define test_case_2_5_5_stream_1 (NULL)
#define test_case_2_5_5_stream_2 (NULL)

#define tgrp_case_2_5_6 test_group_2
#define numb_case_2_5_6 "2.5.6"
#define name_case_2_5_6 "Perform streamio I_FLUSH - ENXIO."
#define sref_case_2_5_6 sref_case_2_5
#define desc_case_2_5_6 "\
Checks that I_FLUSH can be performed on a Stream.\n\
This test is perfomed on a hung up Stream and should return ENXIO."

int
test_case_2_5_6(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHRW) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_6 = { &preamble_2_1, &test_case_2_5_6, &postamble_2 };

#define test_case_2_5_6_stream_0 (&test_2_5_6)
#define test_case_2_5_6_stream_1 (NULL)
#define test_case_2_5_6_stream_2 (NULL)

#define tgrp_case_2_5_7 test_group_2
#define numb_case_2_5_7 "2.5.7"
#define name_case_2_5_7 "Perform streamio I_FLUSH - EPROTO."
#define sref_case_2_5_7 sref_case_2_5
#define desc_case_2_5_7 "\
Checks that I_FLUSH can be performed on a Stream.\n\
This test is perfomed on a read errored Stream and should return EPROTO."

int
test_case_2_5_7(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHRW) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_7 = { &preamble_2_2, &test_case_2_5_7, &postamble_2 };

#define test_case_2_5_7_stream_0 (&test_2_5_7)
#define test_case_2_5_7_stream_1 (NULL)
#define test_case_2_5_7_stream_2 (NULL)

#define tgrp_case_2_5_8 test_group_2
#define numb_case_2_5_8 "2.5.8"
#define name_case_2_5_8 "Perform streamio I_FLUSH - EPROTO."
#define sref_case_2_5_8 sref_case_2_5
#define desc_case_2_5_8 "\
Checks that I_FLUSH can be performed on a Stream.\n\
This test is perfomed on a write errored Stream and should return EPROTO."

int
test_case_2_5_8(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHRW) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_8 = { &preamble_2_3, &test_case_2_5_8, &postamble_2 };

#define test_case_2_5_8_stream_0 (&test_2_5_8)
#define test_case_2_5_8_stream_1 (NULL)
#define test_case_2_5_8_stream_2 (NULL)

#define tgrp_case_2_5_9 test_group_2
#define numb_case_2_5_9 "2.5.9"
#define name_case_2_5_9 "Perform streamio I_FLUSH - EPROTO."
#define sref_case_2_5_9 sref_case_2_5
#define desc_case_2_5_9 "\
Checks that I_FLUSH can be performed on a Stream.\n\
This test is perfomed on a errored Stream and should return EPROTO."

int
test_case_2_5_9(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHRW) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_9 = { &preamble_2_4, &test_case_2_5_9, &postamble_2 };

#define test_case_2_5_9_stream_0 (&test_2_5_9)
#define test_case_2_5_9_stream_1 (NULL)
#define test_case_2_5_9_stream_2 (NULL)

#define tgrp_case_2_5_10 test_group_2
#define numb_case_2_5_10 "2.5.10"
#define name_case_2_5_10 "Perform streamio I_FLUSH - EINVAL."
#define sref_case_2_5_10 sref_case_2_5
#define desc_case_2_5_10 "\
Checks that I_FLUSH can be performed on a Stream.  This test is perfomed\n\
on a Stream linked under a Muliplexing Driver and should return EINVAL."

int
test_case_2_5_10(int child)
{
	if (test_ioctl(child, I_FLUSH, FLUSHRW) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_5_10 = { &preamble_5, &test_case_2_5_10, &postamble_5 };

#define test_case_2_5_10_stream_0 (&test_2_5_10)
#define test_case_2_5_10_stream_1 (NULL)
#define test_case_2_5_10_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_SRDOPT
 */
static const char sref_case_2_6[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_SRDOPT section.";

#define tgrp_case_2_6_1 test_group_2
#define numb_case_2_6_1 "2.6.1"
#define name_case_2_6_1 "Perform streamio I_SRDOPT - EINVAL."
#define sref_case_2_6_1 sref_case_2_6
#define desc_case_2_6_1 "\
Checks that I_SRDOPT can be performed on a Stream.  This case is performed with\n\
and invalid argument (RMSGD|RMSGN) and should return EINVAL.  Note that zero (0)\n\
is valid for compatibility with older versions that did not support protocol\n\
options."

int
test_case_2_6_1(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RMSGD|RMSGN)) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_1 = { &preamble_0, &test_case_2_6_1, &postamble_0 };

#define test_case_2_6_1_stream_0 (&test_2_6_1)
#define test_case_2_6_1_stream_1 (NULL)
#define test_case_2_6_1_stream_2 (NULL)

#define tgrp_case_2_6_2 test_group_2
#define numb_case_2_6_2 "2.6.2"
#define name_case_2_6_2 "Perform streamio I_SRDOPT - RNORM | RPROTNORM."
#define sref_case_2_6_2 sref_case_2_6
#define desc_case_2_6_2 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RNORM | RPROTNORM)."

int
test_case_2_6_2(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_2 = { &preamble_0, &test_case_2_6_2, &postamble_0 };

#define test_case_2_6_2_stream_0 (&test_2_6_2)
#define test_case_2_6_2_stream_1 (NULL)
#define test_case_2_6_2_stream_2 (NULL)

#define tgrp_case_2_6_3 test_group_2
#define numb_case_2_6_3 "2.6.3"
#define name_case_2_6_3 "Perform streamio I_SRDOPT - RNORM | RPROTDAT."
#define sref_case_2_6_3 sref_case_2_6
#define desc_case_2_6_3 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RNORM | RPROTDAT)."

int
test_case_2_6_3(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTDAT)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_3 = { &preamble_0, &test_case_2_6_3, &postamble_0 };

#define test_case_2_6_3_stream_0 (&test_2_6_3)
#define test_case_2_6_3_stream_1 (NULL)
#define test_case_2_6_3_stream_2 (NULL)

#define tgrp_case_2_6_4 test_group_2
#define numb_case_2_6_4 "2.6.4"
#define name_case_2_6_4 "Perform streamio I_SRDOPT - RNORM | RPROTDIS."
#define sref_case_2_6_4 sref_case_2_6
#define desc_case_2_6_4 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RNORM | RPROTDIS)."

int
test_case_2_6_4(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTDIS)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_4 = { &preamble_0, &test_case_2_6_4, &postamble_0 };

#define test_case_2_6_4_stream_0 (&test_2_6_4)
#define test_case_2_6_4_stream_1 (NULL)
#define test_case_2_6_4_stream_2 (NULL)

#define tgrp_case_2_6_5 test_group_2
#define numb_case_2_6_5 "2.6.5"
#define name_case_2_6_5 "Perform streamio I_SRDOPT - RMSGN | RPROTNORM."
#define sref_case_2_6_5 sref_case_2_6
#define desc_case_2_6_5 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RMSGN | RPROTNORM)."

int
test_case_2_6_5(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_5 = { &preamble_0, &test_case_2_6_5, &postamble_0 };

#define test_case_2_6_5_stream_0 (&test_2_6_5)
#define test_case_2_6_5_stream_1 (NULL)
#define test_case_2_6_5_stream_2 (NULL)

#define tgrp_case_2_6_6 test_group_2
#define numb_case_2_6_6 "2.6.6"
#define name_case_2_6_6 "Perform streamio I_SRDOPT - RMSGN | RPROTDAT."
#define sref_case_2_6_6 sref_case_2_6
#define desc_case_2_6_6 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RMSGN | RPROTDAT)."

int
test_case_2_6_6(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTDAT)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_6 = { &preamble_0, &test_case_2_6_6, &postamble_0 };

#define test_case_2_6_6_stream_0 (&test_2_6_6)
#define test_case_2_6_6_stream_1 (NULL)
#define test_case_2_6_6_stream_2 (NULL)

#define tgrp_case_2_6_7 test_group_2
#define numb_case_2_6_7 "2.6.7"
#define name_case_2_6_7 "Perform streamio I_SRDOPT - RMSGN | RPROTDIS."
#define sref_case_2_6_7 sref_case_2_6
#define desc_case_2_6_7 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RMSGN | RPROTDIS)."

int
test_case_2_6_7(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTDIS)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_7 = { &preamble_0, &test_case_2_6_7, &postamble_0 };

#define test_case_2_6_7_stream_0 (&test_2_6_7)
#define test_case_2_6_7_stream_1 (NULL)
#define test_case_2_6_7_stream_2 (NULL)

#define tgrp_case_2_6_8 test_group_2
#define numb_case_2_6_8 "2.6.8"
#define name_case_2_6_8 "Perform streamio I_SRDOPT - RMSGD | RPROTNORM."
#define sref_case_2_6_8 sref_case_2_6
#define desc_case_2_6_8 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RMSGD | RPROTNORM)."

int
test_case_2_6_8(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_8 = { &preamble_0, &test_case_2_6_8, &postamble_0 };

#define test_case_2_6_8_stream_0 (&test_2_6_8)
#define test_case_2_6_8_stream_1 (NULL)
#define test_case_2_6_8_stream_2 (NULL)

#define tgrp_case_2_6_9 test_group_2
#define numb_case_2_6_9 "2.6.9"
#define name_case_2_6_9 "Perform streamio I_SRDOPT - RMSGD | RPROTDAT."
#define sref_case_2_6_9 sref_case_2_6
#define desc_case_2_6_9 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RMSGD | RPROTDAT)."

int
test_case_2_6_9(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTDAT)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_9 = { &preamble_0, &test_case_2_6_9, &postamble_0 };

#define test_case_2_6_9_stream_0 (&test_2_6_9)
#define test_case_2_6_9_stream_1 (NULL)
#define test_case_2_6_9_stream_2 (NULL)

#define tgrp_case_2_6_10 test_group_2
#define numb_case_2_6_10 "2.6.10"
#define name_case_2_6_10 "Perform streamio I_SRDOPT - RMSGD | RPROTDIS."
#define sref_case_2_6_10 sref_case_2_6
#define desc_case_2_6_10 "\
Checks that I_SRDOPT can be performed on a Stream with the values \n\
(RMSGD | RPROTDIS)."

int
test_case_2_6_10(int child)
{
	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTDIS)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_10 = { &preamble_0, &test_case_2_6_10, &postamble_0 };

#define test_case_2_6_10_stream_0 (&test_2_6_10)
#define test_case_2_6_10_stream_1 (NULL)
#define test_case_2_6_10_stream_2 (NULL)

#define tgrp_case_2_6_11 test_group_2
#define numb_case_2_6_11 "2.6.11"
#define name_case_2_6_11 "Perform streamio I_SRDOPT - EINVAL."
#define sref_case_2_6_11 sref_case_2_6
#define desc_case_2_6_11 "\
Checks that EINVAL is returned when I_SRDOPT is called with an invalid\n\
argument (-1)."

int
test_case_2_6_11(int child)
{
	if (test_ioctl(child, I_SRDOPT, -1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_11 = { &preamble_0, &test_case_2_6_11, &postamble_0 };

#define test_case_2_6_11_stream_0 (&test_2_6_11)
#define test_case_2_6_11_stream_1 (NULL)
#define test_case_2_6_11_stream_2 (NULL)

#define tgrp_case_2_6_12 test_group_2
#define numb_case_2_6_12 "2.6.12"
#define name_case_2_6_12 "Perform streamio I_SRDOPT - EPROTO."
#define sref_case_2_6_12 sref_case_2_6
#define desc_case_2_6_12 "\
Checks that EPROTO is returned when I_SRDOPT is called on a Stream with\n\
a read error."

int
test_case_2_6_12(int child)
{
	if (test_ioctl(child, I_SRDOPT, 0) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_12 = { &preamble_2_2, &test_case_2_6_12, &postamble_2 };

#define test_case_2_6_12_stream_0 (&test_2_6_12)
#define test_case_2_6_12_stream_1 (NULL)
#define test_case_2_6_12_stream_2 (NULL)

#define tgrp_case_2_6_13 test_group_2
#define numb_case_2_6_13 "2.6.13"
#define name_case_2_6_13 "Perform streamio I_SRDOPT."
#define sref_case_2_6_13 sref_case_2_6
#define desc_case_2_6_13 "\
Checks that success is returned when I_SRDOPT is called on a Stream with\n\
a write error."

int
test_case_2_6_13(int child)
{
	if (test_ioctl(child, I_SRDOPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_13 = { &preamble_2_3, &test_case_2_6_13, &postamble_2 };

#define test_case_2_6_13_stream_0 (&test_2_6_13)
#define test_case_2_6_13_stream_1 (NULL)
#define test_case_2_6_13_stream_2 (NULL)

#define tgrp_case_2_6_14 test_group_2
#define numb_case_2_6_14 "2.6.14"
#define name_case_2_6_14 "Perform streamio I_SRDOPT - EPROTO."
#define sref_case_2_6_14 sref_case_2_6
#define desc_case_2_6_14 "\
Checks that EPROTO is returned when I_SRDOPT is called on a Stream with\n\
an error."

int
test_case_2_6_14(int child)
{
	if (test_ioctl(child, I_SRDOPT, 0) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_14 = { &preamble_2_4, &test_case_2_6_14, &postamble_2 };

#define test_case_2_6_14_stream_0 (&test_2_6_14)
#define test_case_2_6_14_stream_1 (NULL)
#define test_case_2_6_14_stream_2 (NULL)

#define tgrp_case_2_6_15 test_group_2
#define numb_case_2_6_15 "2.6.15"
#define name_case_2_6_15 "Perform streamio I_SRDOPT - EINVAL."
#define sref_case_2_6_15 sref_case_2_6
#define desc_case_2_6_15 "\
Checks that EINVAL is returned when I_SRDOPT is called on a Stream that\n\
is linked under a Multiplexing Driver."

int
test_case_2_6_15(int child)
{
	if (test_ioctl(child, I_SRDOPT, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_15 = { &preamble_5, &test_case_2_6_15, &postamble_5 };

#define test_case_2_6_15_stream_0 (&test_2_6_15)
#define test_case_2_6_15_stream_1 (NULL)
#define test_case_2_6_15_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GRDOPT
 */
static const char sref_case_2_7[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_GRDOPT section.";

#define tgrp_case_2_7_1 test_group_2
#define numb_case_2_7_1 "2.7.1"
#define name_case_2_7_1 "Perform streamio I_GRDOPT - default."
#define sref_case_2_7_1 sref_case_2_7
#define desc_case_2_7_1 "\
Checks that I_GRDOPT can be performed on a Stream to read the Stream default\n\
read options."

int
test_case_2_7_1(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_1 = { &preamble_0, &test_case_2_7_1, &postamble_0 };

#define test_case_2_7_1_stream_0 (&test_2_7_1)
#define test_case_2_7_1_stream_1 (NULL)
#define test_case_2_7_1_stream_2 (NULL)

#define tgrp_case_2_7_2 test_group_2
#define numb_case_2_7_2 "2.7.2"
#define name_case_2_7_2 "Perform streamio I_GRDOPT - set default."
#define sref_case_2_7_2 sref_case_2_7
#define desc_case_2_7_2 "\
Checks that I_GRDOPT can be performed on a Stream to read the Stream default\n\
options after they have been set with I_SRDOPT."

int
test_case_2_7_2(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_2 = { &preamble_0, &test_case_2_7_2, &postamble_0 };

#define test_case_2_7_2_stream_0 (&test_2_7_2)
#define test_case_2_7_2_stream_1 (NULL)
#define test_case_2_7_2_stream_2 (NULL)

#define tgrp_case_2_7_3 test_group_2
#define numb_case_2_7_3 "2.7.3"
#define name_case_2_7_3 "Perform streamio I_GRDOPT - RNORM | RPROTDAT."
#define sref_case_2_7_3 sref_case_2_7
#define desc_case_2_7_3 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RNORM | RPROTDAT) after they have been set with I_SRDOPT."

int
test_case_2_7_3(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTDAT)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTDAT))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_3 = { &preamble_0, &test_case_2_7_3, &postamble_0 };

#define test_case_2_7_3_stream_0 (&test_2_7_3)
#define test_case_2_7_3_stream_1 (NULL)
#define test_case_2_7_3_stream_2 (NULL)

#define tgrp_case_2_7_4 test_group_2
#define numb_case_2_7_4 "2.7.4"
#define name_case_2_7_4 "Perform streamio I_GRDOPT - RNORM | RPROTDIS."
#define sref_case_2_7_4 sref_case_2_7
#define desc_case_2_7_4 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RNORM | RPROTDIS) after they have been set with I_SRDOPT."

int
test_case_2_7_4(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTDIS)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTDIS))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_4 = { &preamble_0, &test_case_2_7_4, &postamble_0 };

#define test_case_2_7_4_stream_0 (&test_2_7_4)
#define test_case_2_7_4_stream_1 (NULL)
#define test_case_2_7_4_stream_2 (NULL)

#define tgrp_case_2_7_5 test_group_2
#define numb_case_2_7_5 "2.7.5"
#define name_case_2_7_5 "Perform streamio I_GRDOPT - RMSGD | RPROTNORM."
#define sref_case_2_7_5 sref_case_2_7
#define desc_case_2_7_5 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RMSGD | RPROTNORM) after they have been set with I_SRDOPT."

int
test_case_2_7_5(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RMSGD | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_5 = { &preamble_0, &test_case_2_7_5, &postamble_0 };

#define test_case_2_7_5_stream_0 (&test_2_7_5)
#define test_case_2_7_5_stream_1 (NULL)
#define test_case_2_7_5_stream_2 (NULL)

#define tgrp_case_2_7_6 test_group_2
#define numb_case_2_7_6 "2.7.6"
#define name_case_2_7_6 "Perform streamio I_GRDOPT - RMSGD | RPROTDAT."
#define sref_case_2_7_6 sref_case_2_7
#define desc_case_2_7_6 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RMSGD | RPROTDAT) after they have been set with I_SRDOPT."

int
test_case_2_7_6(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTDAT)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RMSGD | RPROTDAT))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_6 = { &preamble_0, &test_case_2_7_6, &postamble_0 };

#define test_case_2_7_6_stream_0 (&test_2_7_6)
#define test_case_2_7_6_stream_1 (NULL)
#define test_case_2_7_6_stream_2 (NULL)

#define tgrp_case_2_7_7 test_group_2
#define numb_case_2_7_7 "2.7.7"
#define name_case_2_7_7 "Perform streamio I_GRDOPT - RMSGD | RPROTDIS."
#define sref_case_2_7_7 sref_case_2_7
#define desc_case_2_7_7 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RMSGD | RPROTDIS) after they have been set with I_SRDOPT."

int
test_case_2_7_7(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTDIS)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RMSGD | RPROTDIS))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_7 = { &preamble_0, &test_case_2_7_7, &postamble_0 };

#define test_case_2_7_7_stream_0 (&test_2_7_7)
#define test_case_2_7_7_stream_1 (NULL)
#define test_case_2_7_7_stream_2 (NULL)

#define tgrp_case_2_7_8 test_group_2
#define numb_case_2_7_8 "2.7.8"
#define name_case_2_7_8 "Perform streamio I_GRDOPT - RMSGN | RPROTNORM."
#define sref_case_2_7_8 sref_case_2_7
#define desc_case_2_7_8 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RMSGN | RPROTNORM) after they have been set with I_SRDOPT."

int
test_case_2_7_8(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RMSGN | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_8 = { &preamble_0, &test_case_2_7_8, &postamble_0 };

#define test_case_2_7_8_stream_0 (&test_2_7_8)
#define test_case_2_7_8_stream_1 (NULL)
#define test_case_2_7_8_stream_2 (NULL)

#define tgrp_case_2_7_9 test_group_2
#define numb_case_2_7_9 "2.7.9"
#define name_case_2_7_9 "Perform streamio I_GRDOPT - RMSGN | RPROTDAT."
#define sref_case_2_7_9 sref_case_2_7
#define desc_case_2_7_9 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RMSGN | RPROTDAT) after they have been set with I_SRDOPT."

int
test_case_2_7_9(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTDAT)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RMSGN | RPROTDAT))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_9 = { &preamble_0, &test_case_2_7_9, &postamble_0 };

#define test_case_2_7_9_stream_0 (&test_2_7_9)
#define test_case_2_7_9_stream_1 (NULL)
#define test_case_2_7_9_stream_2 (NULL)

#define tgrp_case_2_7_10 test_group_2
#define numb_case_2_7_10 "2.7.10"
#define name_case_2_7_10 "Perform streamio I_GRDOPT - RMSGN | RPROTDIS."
#define sref_case_2_7_10 sref_case_2_7
#define desc_case_2_7_10 "\
Checks that I_GRDOPT can be performed on a Stream to read the read options\n\
(RMSGN | RPROTDIS) after they have been set with I_SRDOPT."

int
test_case_2_7_10(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTDIS)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RMSGN | RPROTDIS))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_10 = { &preamble_0, &test_case_2_7_10, &postamble_0 };

#define test_case_2_7_10_stream_0 (&test_2_7_10)
#define test_case_2_7_10_stream_1 (NULL)
#define test_case_2_7_10_stream_2 (NULL)

#define tgrp_case_2_7_11 test_group_2
#define numb_case_2_7_11 "2.7.11"
#define name_case_2_7_11 "Perform streamio I_GRDOPT - EFAULT."
#define sref_case_2_7_11 sref_case_2_7
#define desc_case_2_7_11 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_7_11(int child)
{
	if (test_ioctl(child, I_GRDOPT, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_11 = { &preamble_0, &test_case_2_7_11, &postamble_0 };

#define test_case_2_7_11_stream_0 (&test_2_7_11)
#define test_case_2_7_11_stream_1 (NULL)
#define test_case_2_7_11_stream_2 (NULL)

#define tgrp_case_2_7_12 test_group_2
#define numb_case_2_7_12 "2.7.12"
#define name_case_2_7_12 "Perform streamio I_GRDOPT."
#define sref_case_2_7_12 sref_case_2_7
#define desc_case_2_7_12 "\
Checks that I_GRDOPT can be performed on a Stream.  This test checks\n\
that the call is successful regardless of whether the Stream is hung\n\
up."

int
test_case_2_7_12(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_12 = { &preamble_2_1, &test_case_2_7_12, &postamble_2 };

#define test_case_2_7_12_stream_0 (&test_2_7_12)
#define test_case_2_7_12_stream_1 (NULL)
#define test_case_2_7_12_stream_2 (NULL)

#define tgrp_case_2_7_13 test_group_2
#define numb_case_2_7_13 "2.7.13"
#define name_case_2_7_13 "Perform streamio I_GRDOPT."
#define sref_case_2_7_13 sref_case_2_7
#define desc_case_2_7_13 "\
Checks that I_GRDOPT can be performed on a Stream.  This test checks\n\
that the call is successful regardless of whether the Stream is in read\n\
error."

int
test_case_2_7_13(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_13 = { &preamble_2_2, &test_case_2_7_13, &postamble_2 };

#define test_case_2_7_13_stream_0 (&test_2_7_13)
#define test_case_2_7_13_stream_1 (NULL)
#define test_case_2_7_13_stream_2 (NULL)

#define tgrp_case_2_7_14 test_group_2
#define numb_case_2_7_14 "2.7.14"
#define name_case_2_7_14 "Perform streamio I_GRDOPT."
#define sref_case_2_7_14 sref_case_2_7
#define desc_case_2_7_14 "\
Checks that I_GRDOPT can be performed on a Stream.  This test checks\n\
that the call is successful regardless of whether the Stream is in write\n\
error."

int
test_case_2_7_14(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_14 = { &preamble_2_3, &test_case_2_7_14, &postamble_2 };

#define test_case_2_7_14_stream_0 (&test_2_7_14)
#define test_case_2_7_14_stream_1 (NULL)
#define test_case_2_7_14_stream_2 (NULL)

#define tgrp_case_2_7_15 test_group_2
#define numb_case_2_7_15 "2.7.15"
#define name_case_2_7_15 "Perform streamio I_GRDOPT."
#define sref_case_2_7_15 sref_case_2_7
#define desc_case_2_7_15 "\
Checks that I_GRDOPT can be performed on a Stream.  This test checks\n\
that the call is successful regardless of whether the Stream is in error."

int
test_case_2_7_15(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (rdopts != (RNORM | RPROTNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_15 = { &preamble_2_4, &test_case_2_7_15, &postamble_2 };

#define test_case_2_7_15_stream_0 (&test_2_7_15)
#define test_case_2_7_15_stream_1 (NULL)
#define test_case_2_7_15_stream_2 (NULL)

#define tgrp_case_2_7_16 test_group_2
#define numb_case_2_7_16 "2.7.16"
#define name_case_2_7_16 "Perform streamio I_GRDOPT - EINVAL."
#define sref_case_2_7_16 sref_case_2_7
#define desc_case_2_7_16 "\
Checks that I_GRDOPT can be performed on a Stream.  This test checks\n\
that EINVAL is returned when the Stream is linked under a Multiplexing\n\
Driver."

int
test_case_2_7_16(int child)
{
	int rdopts = -1;

	if (test_ioctl(child, I_GRDOPT, (intptr_t) & rdopts) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_16 = { &preamble_5, &test_case_2_7_16, &postamble_5 };

#define test_case_2_7_16_stream_0 (&test_2_7_16)
#define test_case_2_7_16_stream_1 (NULL)
#define test_case_2_7_16_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_STR
 */
static const char sref_case_2_8[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_STR section.";

#define tgrp_case_2_8_1 test_group_2
#define numb_case_2_8_1 "2.8.1"
#define name_case_2_8_1 "Perform streamio I_STR - EFAULT."
#define sref_case_2_8_1 sref_case_2_8
#define desc_case_2_8_1 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_8_1(int child)
{
	if (test_ioctl(child, I_STR, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_1 = { &preamble_0, &test_case_2_8_1, &postamble_0 };

#define test_case_2_8_1_stream_0 (&test_2_8_1)
#define test_case_2_8_1_stream_1 (NULL)
#define test_case_2_8_1_stream_2 (NULL)

#define tgrp_case_2_8_2 test_group_2
#define numb_case_2_8_2 "2.8.2"
#define name_case_2_8_2 "Perform streamio I_STR - EINVAL."
#define sref_case_2_8_2 sref_case_2_8
#define desc_case_2_8_2 "\
Checks that I_STR can be performed on a Stream.  This case tests \n\
ic_len less than zero, which should return EINVAL."

int
test_case_2_8_2(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = -1, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_2 = { &preamble_0, &test_case_2_8_2, &postamble_0 };

#define test_case_2_8_2_stream_0 (&test_2_8_2)
#define test_case_2_8_2_stream_1 (NULL)
#define test_case_2_8_2_stream_2 (NULL)

#define tgrp_case_2_8_3 test_group_2
#define numb_case_2_8_3 "2.8.3"
#define name_case_2_8_3 "Perform streamio I_STR - EINVAL."
#define sref_case_2_8_3 sref_case_2_8
#define desc_case_2_8_3 "\
Checks that I_STR can be performed on a Stream.  This case tests \n\
ic_len greater than streams.strmsgsz, which should return EINVAL."

int
test_case_2_8_3(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 262145, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_3 = { &preamble_0, &test_case_2_8_3, &postamble_0 };

#define test_case_2_8_3_stream_0 (&test_2_8_3)
#define test_case_2_8_3_stream_1 (NULL)
#define test_case_2_8_3_stream_2 (NULL)

#define tgrp_case_2_8_4 test_group_2
#define numb_case_2_8_4 "2.8.4"
#define name_case_2_8_4 "Perform streamio I_STR - EINVAL."
#define sref_case_2_8_4 sref_case_2_8
#define desc_case_2_8_4 "\
Checks that I_STR can be performed on a Stream.  This case tests \n\
ic_timout less than minus one (-1), which should return EINVAL."

int
test_case_2_8_4(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = -2, .ic_len = 0, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_4 = { &preamble_0, &test_case_2_8_4, &postamble_0 };

#define test_case_2_8_4_stream_0 (&test_2_8_4)
#define test_case_2_8_4_stream_1 (NULL)
#define test_case_2_8_4_stream_2 (NULL)

#define tgrp_case_2_8_5 test_group_2
#define numb_case_2_8_5 "2.8.5"
#define name_case_2_8_5 "Perform streamio I_STR."
#define sref_case_2_8_5 sref_case_2_8
#define desc_case_2_8_5 "\
Checks that I_STR can be performed on a Stream."

int
test_case_2_8_5(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 0, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_5 = { &preamble_0, &test_case_2_8_5, &postamble_0 };

#define test_case_2_8_5_stream_0 (&test_2_8_5)
#define test_case_2_8_5_stream_1 (NULL)
#define test_case_2_8_5_stream_2 (NULL)

#define tgrp_case_2_8_6 test_group_2
#define numb_case_2_8_6 "2.8.6"
#define name_case_2_8_6 "Perform streamio I_STR - ENXIO."
#define sref_case_2_8_6 sref_case_2_8
#define desc_case_2_8_6 "\
Checks that I_STR can be performed on a Stream.  Check that ENXIO is\n\
returned when I_STR is attempted on a Stream that has hung up."

int
test_case_2_8_6(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 0, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_6 = { &preamble_2_1, &test_case_2_8_6, &postamble_2 };

#define test_case_2_8_6_stream_0 (&test_2_8_6)
#define test_case_2_8_6_stream_1 (NULL)
#define test_case_2_8_6_stream_2 (NULL)

#define tgrp_case_2_8_7 test_group_2
#define numb_case_2_8_7 "2.8.7"
#define name_case_2_8_7 "Perform streamio I_STR - EPROTO."
#define sref_case_2_8_7 sref_case_2_8
#define desc_case_2_8_7 "\
Checks that I_STR can be performed on a Stream.  Check that EPROTO is\n\
returned when I_STR is attempted on a Stream that has a read error."

int
test_case_2_8_7(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 0, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_7 = { &preamble_2_2, &test_case_2_8_7, &postamble_2 };

#define test_case_2_8_7_stream_0 (&test_2_8_7)
#define test_case_2_8_7_stream_1 (NULL)
#define test_case_2_8_7_stream_2 (NULL)

#define tgrp_case_2_8_8 test_group_2
#define numb_case_2_8_8 "2.8.8"
#define name_case_2_8_8 "Perform streamio I_STR - EPROTO."
#define sref_case_2_8_8 sref_case_2_8
#define desc_case_2_8_8 "\
Checks that I_STR can be performed on a Stream.  Check that EPROTO is\n\
returned when I_STR is attempted on a Stream that has a write error."

int
test_case_2_8_8(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 0, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_8 = { &preamble_2_3, &test_case_2_8_8, &postamble_2 };

#define test_case_2_8_8_stream_0 (&test_2_8_8)
#define test_case_2_8_8_stream_1 (NULL)
#define test_case_2_8_8_stream_2 (NULL)

#define tgrp_case_2_8_9 test_group_2
#define numb_case_2_8_9 "2.8.9"
#define name_case_2_8_9 "Perform streamio I_STR - EPROTO."
#define sref_case_2_8_9 sref_case_2_8
#define desc_case_2_8_9 "\
Checks that I_STR can be performed on a Stream.  Check that EPROTO is\n\
returned when I_STR is attempted on a Stream that has an error."

int
test_case_2_8_9(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 0, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_9 = { &preamble_2_4, &test_case_2_8_9, &postamble_2 };

#define test_case_2_8_9_stream_0 (&test_2_8_9)
#define test_case_2_8_9_stream_1 (NULL)
#define test_case_2_8_9_stream_2 (NULL)

#define tgrp_case_2_8_10 test_group_2
#define numb_case_2_8_10 "2.8.10"
#define name_case_2_8_10 "Perform streamio I_STR - EINVAL."
#define sref_case_2_8_10 sref_case_2_8
#define desc_case_2_8_10 "\
Checks that I_STR can be performed on a Stream.  Check that EINVAL is\n\
returned when I_STR is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_2_8_10(int child)
{
	struct strioctl ic = { .ic_cmd = -5, .ic_timout = 0, .ic_len = 0, .ic_dp = NULL, };
	if (test_ioctl(child, I_STR, (intptr_t) &ic) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_10 = { &preamble_5, &test_case_2_8_10, &postamble_5 };

#define test_case_2_8_10_stream_0 (&test_2_8_10)
#define test_case_2_8_10_stream_1 (NULL)
#define test_case_2_8_10_stream_2 (NULL)

#define tgrp_case_2_8_11 test_group_2
#define numb_case_2_8_11 "2.8.11"
#define name_case_2_8_11 "Perform streamio I_STR."
#define sref_case_2_8_11 sref_case_2_8
#define desc_case_2_8_11 "\
Checks that I_STR can be performed on a Stream.  Check that an I_STR\n\
that copies data in and out is successful."

int
test_case_2_8_11(int child)
{
	char buf[1024] = { 0, };
	struct strioctl ic = { .ic_cmd = TM_IOC_IOCTL, .ic_timout = 0, .ic_len = sizeof(buf), .ic_dp = buf, };
	int i;

	if (test_ioctl(child, I_STR, (intptr_t) &ic) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(buf); i++)
		if ((unsigned char) buf[i] != (unsigned char) 0xa5)
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_8_11 = { &preamble_2, &test_case_2_8_11, &postamble_2 };

#define test_case_2_8_11_stream_0 (&test_2_8_11)
#define test_case_2_8_11_stream_1 (NULL)
#define test_case_2_8_11_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_SETSIG
 */
static const char sref_case_2_9[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_SETSIG section.";

#define tgrp_case_2_9_1 test_group_2
#define numb_case_2_9_1 "2.9.1"
#define name_case_2_9_1 "Perform streamio I_SETSIG - EINVAL."
#define sref_case_2_9_1 sref_case_2_9
#define desc_case_2_9_1 "\
Checks that I_SETSIG can be performed on a Stream. This test case uses \n\
a signal value of zero (0) that means deregistration and should return \n\
[EINVAL] because we are not registered."

int
test_case_2_9_1(int child)
{
	if (test_ioctl(child, I_SETSIG, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_1 = { &preamble_0, &test_case_2_9_1, &postamble_0 };

#define test_case_2_9_1_stream_0 (&test_2_9_1)
#define test_case_2_9_1_stream_1 (NULL)
#define test_case_2_9_1_stream_2 (NULL)

#define tgrp_case_2_9_2 test_group_2
#define numb_case_2_9_2 "2.9.2"
#define name_case_2_9_2 "Perform streamio I_SETSIG."
#define sref_case_2_9_2 sref_case_2_9
#define desc_case_2_9_2 "\
Checks that I_SETSIG can be performed on a Stream."

int
test_case_2_9_2(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_2 = { &preamble_0, &test_case_2_9_2, &postamble_0 };

#define test_case_2_9_2_stream_0 (&test_2_9_2)
#define test_case_2_9_2_stream_1 (NULL)
#define test_case_2_9_2_stream_2 (NULL)

#define tgrp_case_2_9_3 test_group_2
#define numb_case_2_9_3 "2.9.3"
#define name_case_2_9_3 "Perform streamio I_SETSIG."
#define sref_case_2_9_3 sref_case_2_9
#define desc_case_2_9_3 "\
Checks that I_SETSIG can be performed on a Stream that is hung up."

int
test_case_2_9_3(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_3 = { &preamble_2_1, &test_case_2_9_3, &postamble_2 };

#define test_case_2_9_3_stream_0 (&test_2_9_3)
#define test_case_2_9_3_stream_1 (NULL)
#define test_case_2_9_3_stream_2 (NULL)

#define tgrp_case_2_9_4 test_group_2
#define numb_case_2_9_4 "2.9.4"
#define name_case_2_9_4 "Perform streamio I_SETSIG."
#define sref_case_2_9_4 sref_case_2_9
#define desc_case_2_9_4 "\
Checks that I_SETSIG can be performed on a Stream in read error."

int
test_case_2_9_4(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_4 = { &preamble_2_2, &test_case_2_9_4, &postamble_2 };

#define test_case_2_9_4_stream_0 (&test_2_9_4)
#define test_case_2_9_4_stream_1 (NULL)
#define test_case_2_9_4_stream_2 (NULL)

#define tgrp_case_2_9_5 test_group_2
#define numb_case_2_9_5 "2.9.5"
#define name_case_2_9_5 "Perform streamio I_SETSIG."
#define sref_case_2_9_5 sref_case_2_9
#define desc_case_2_9_5 "\
Checks that I_SETSIG can be performed on a Stream in write error."

int
test_case_2_9_5(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_5 = { &preamble_2_3, &test_case_2_9_5, &postamble_2 };

#define test_case_2_9_5_stream_0 (&test_2_9_5)
#define test_case_2_9_5_stream_1 (NULL)
#define test_case_2_9_5_stream_2 (NULL)

#define tgrp_case_2_9_6 test_group_2
#define numb_case_2_9_6 "2.9.6"
#define name_case_2_9_6 "Perform streamio I_SETSIG."
#define sref_case_2_9_6 sref_case_2_9
#define desc_case_2_9_6 "\
Checks that I_SETSIG can be performed on a Stream in error."

int
test_case_2_9_6(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_6 = { &preamble_2_4, &test_case_2_9_6, &postamble_2 };

#define test_case_2_9_6_stream_0 (&test_2_9_6)
#define test_case_2_9_6_stream_1 (NULL)
#define test_case_2_9_6_stream_2 (NULL)

#define tgrp_case_2_9_7 test_group_2
#define numb_case_2_9_7 "2.9.7"
#define name_case_2_9_7 "Perform streamio I_SETSIG - EINVAL."
#define sref_case_2_9_7 sref_case_2_9
#define desc_case_2_9_7 "\
Checks that I_SETSIG can be performed on a Stream.  Checks that EINVAL\n\
is returned when I_SETSIG is attempted on a Stream linked under a\n\
Multiplexing Driver."

int
test_case_2_9_7(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ALL) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_7 = { &preamble_5, &test_case_2_9_7, &postamble_5 };

#define test_case_2_9_7_stream_0 (&test_2_9_7)
#define test_case_2_9_7_stream_1 (NULL)
#define test_case_2_9_7_stream_2 (NULL)

#define tgrp_case_2_9_8 test_group_2
#define numb_case_2_9_8 "2.9.8"
#define name_case_2_9_8 "Perform streamio I_SETSIG - S_RDNORM."
#define sref_case_2_9_8 sref_case_2_9
#define desc_case_2_9_8 "\
Check that SIGPOLL for S_RDNORM is generated."

int
test_case_2_9_8(int child)
{
	if (test_ioctl(child, I_SETSIG, S_RDNORM) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	{
		char buf[16] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 0;
		int flags = MSG_BAND;

		if (test_putpmsg(child, &ctl, &dat, band, flags) != __RESULT_SUCCESS)
			return (__RESULT_INCONCLUSIVE);
		state++;
	}
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_8 = { &preamble_0, &test_case_2_9_8, &postamble_0 };

#define test_case_2_9_8_stream_0 (&test_2_9_8)
#define test_case_2_9_8_stream_1 (NULL)
#define test_case_2_9_8_stream_2 (NULL)

#define tgrp_case_2_9_9 test_group_2
#define numb_case_2_9_9 "2.9.9"
#define name_case_2_9_9 "Perform streamio I_SETSIG - S_RDBAND."
#define sref_case_2_9_9 sref_case_2_9
#define desc_case_2_9_9 "\
Check that SIGPOLL for S_RDBAND is generated."

int
test_case_2_9_9(int child)
{
	if (test_ioctl(child, I_SETSIG, S_RDBAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	{
		char buf[16] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 1;
		int flags = MSG_BAND;

		if (test_putpmsg(child, &ctl, &dat, band, flags) != __RESULT_SUCCESS)
			return (__RESULT_INCONCLUSIVE);
		state++;
	}
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_9 = { &preamble_0, &test_case_2_9_9, &postamble_0 };

#define test_case_2_9_9_stream_0 (&test_2_9_9)
#define test_case_2_9_9_stream_1 (NULL)
#define test_case_2_9_9_stream_2 (NULL)

#define tgrp_case_2_9_10 test_group_2
#define numb_case_2_9_10 "2.9.10"
#define name_case_2_9_10 "Perform streamio I_SETSIG - S_INPUT."
#define sref_case_2_9_10 sref_case_2_9
#define desc_case_2_9_10 "\
Check that SIGPOLL for S_INPUT is generated."

int
test_case_2_9_10(int child)
{
	if (test_ioctl(child, I_SETSIG, S_INPUT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	{
		char buf[16] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 1;
		int flags = MSG_BAND;

		if (test_putpmsg(child, &ctl, &dat, band, flags) != __RESULT_SUCCESS)
			return (__RESULT_INCONCLUSIVE);
		state++;
	}
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_10 = { &preamble_0, &test_case_2_9_10, &postamble_0 };

#define test_case_2_9_10_stream_0 (&test_2_9_10)
#define test_case_2_9_10_stream_1 (NULL)
#define test_case_2_9_10_stream_2 (NULL)

#define tgrp_case_2_9_11 test_group_2
#define numb_case_2_9_11 "2.9.11"
#define name_case_2_9_11 "Perform streamio I_SETSIG - S_HIPRI."
#define sref_case_2_9_11 sref_case_2_9
#define desc_case_2_9_11 "\
Check that SIGPOLL for S_HIPRI is generated."

int
test_case_2_9_11(int child)
{
	if (test_ioctl(child, I_SETSIG, S_HIPRI) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	{
		char buf[16] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 0;
		int flags = MSG_HIPRI;

		if (test_putpmsg(child, &ctl, &dat, band, flags) != __RESULT_SUCCESS)
			return (__RESULT_INCONCLUSIVE);
		state++;
	}
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_11 = { &preamble_0, &test_case_2_9_11, &postamble_0 };

#define test_case_2_9_11_stream_0 (&test_2_9_11)
#define test_case_2_9_11_stream_1 (NULL)
#define test_case_2_9_11_stream_2 (NULL)

#define tgrp_case_2_9_12_1 test_group_2
#define numb_case_2_9_12_1 "2.9.12.1"
#define name_case_2_9_12_1 "Perform streamio I_SETSIG - S_OUTPUT."
#define sref_case_2_9_12_1 sref_case_2_9
#define desc_case_2_9_12_1 "\
Check that SIGPOLL for S_OUTPUT is generated."

int
test_case_2_9_12_1(int child)
{
	if (test_ioctl(child, I_SETSIG, S_OUTPUT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	{
		char buf[1024];
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int flags = 0;

		last_errno = 0;
		while (last_errno != EAGAIN) {
			state++;
			test_putmsg(child, &ctl, &dat, flags);
		}
	}
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_12_1 = { &preamble_0, &test_case_2_9_12_1, &postamble_0 };

#define test_case_2_9_12_1_stream_0 (&test_2_9_12_1)
#define test_case_2_9_12_1_stream_1 (NULL)
#define test_case_2_9_12_1_stream_2 (NULL)

#define tgrp_case_2_9_12_2 test_group_2
#define numb_case_2_9_12_2 "2.9.12.2"
#define name_case_2_9_12_2 "Perform streamio I_SETSIG - S_OUTPUT."
#define sref_case_2_9_12_2 sref_case_2_9
#define desc_case_2_9_12_2 "\
Check that SIGPOLL for S_OUTPUT is not generated by clearing of band 1\n\
congestion."

int
test_case_2_9_12_2(int child)
{
	if (test_ioctl(child, I_SETSIG, S_OUTPUT) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	{
		char buf[1024];
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 1;
		int flags = MSG_BAND;

		last_errno = 0;
		while (last_errno != EAGAIN) {
			state++;
			test_putpmsg(child, &ctl, &dat, band, flags);
		}
	}
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(100);
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_12_2 = { &preamble_0, &test_case_2_9_12_2, &postamble_0 };

#define test_case_2_9_12_2_stream_0 (&test_2_9_12_2)
#define test_case_2_9_12_2_stream_1 (NULL)
#define test_case_2_9_12_2_stream_2 (NULL)

#define tgrp_case_2_9_13_1 test_group_2
#define numb_case_2_9_13_1 "2.9.13.1"
#define name_case_2_9_13_1 "Perform streamio I_SETSIG - S_WRNORM."
#define sref_case_2_9_13_1 sref_case_2_9
#define desc_case_2_9_13_1 "\
Check that SIGPOLL for S_WRNORM is generated."

int
test_case_2_9_13_1(int child)
{
	if (test_ioctl(child, I_SETSIG, S_WRNORM) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	{
		char buf[1024];
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int flags = 0;

		last_errno = 0;
		while (last_errno != EAGAIN) {
			state++;
			test_putmsg(child, &ctl, &dat, flags);
		}
	}
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_13_1 = { &preamble_0, &test_case_2_9_13_1, &postamble_0 };

#define test_case_2_9_13_1_stream_0 (&test_2_9_13_1)
#define test_case_2_9_13_1_stream_1 (NULL)
#define test_case_2_9_13_1_stream_2 (NULL)

#define tgrp_case_2_9_13_2 test_group_2
#define numb_case_2_9_13_2 "2.9.13.2"
#define name_case_2_9_13_2 "Perform streamio I_SETSIG - S_WRNORM."
#define sref_case_2_9_13_2 sref_case_2_9
#define desc_case_2_9_13_2 "\
Check that SIGPOLL for S_WRNORM is not generated by the clearing of band\n\
1 congestion."

int
test_case_2_9_13_2(int child)
{
	if (test_ioctl(child, I_SETSIG, S_WRNORM) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	{
		char buf[1024];
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 1;
		int flags = MSG_BAND;

		last_errno = 0;
		while (last_errno != EAGAIN) {
			state++;
			test_putpmsg(child, &ctl, &dat, band, flags);
		}
	}
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(100);
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_13_2 = { &preamble_0, &test_case_2_9_13_2, &postamble_0 };

#define test_case_2_9_13_2_stream_0 (&test_2_9_13_2)
#define test_case_2_9_13_2_stream_1 (NULL)
#define test_case_2_9_13_2_stream_2 (NULL)

#define tgrp_case_2_9_14_1 test_group_2
#define numb_case_2_9_14_1 "2.9.14.1"
#define name_case_2_9_14_1 "Perform streamio I_SETSIG - S_WRBAND."
#define sref_case_2_9_14_1 sref_case_2_9
#define desc_case_2_9_14_1 "\
Check that SIGPOLL for S_WRBAND is generated."

int
test_case_2_9_14_1(int child)
{
	if (test_ioctl(child, I_SETSIG, S_WRBAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	{
		char buf[1024];
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 1;
		int flags = MSG_BAND;

		last_errno = 0;
		while (last_errno != EAGAIN) {
			state++;
			test_putpmsg(child, &ctl, &dat, band, flags);
		}
	}
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_14_1 = { &preamble_0, &test_case_2_9_14_1, &postamble_0 };

#define test_case_2_9_14_1_stream_0 (&test_2_9_14_1)
#define test_case_2_9_14_1_stream_1 (NULL)
#define test_case_2_9_14_1_stream_2 (NULL)

#define tgrp_case_2_9_14_2 test_group_2
#define numb_case_2_9_14_2 "2.9.14.2"
#define name_case_2_9_14_2 "Perform streamio I_SETSIG - S_WRBAND."
#define sref_case_2_9_14_2 sref_case_2_9
#define desc_case_2_9_14_2 "\
Check that SIGPOLL for S_WRBAND is not generated by the clearing of band\n\
0 congestion."

int
test_case_2_9_14_2(int child)
{
	if (test_ioctl(child, I_SETSIG, S_WRBAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	{
		char buf[1024];
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int flags = 0;

		last_errno = 0;
		while (last_errno != EAGAIN) {
			state++;
			test_putmsg(child, &ctl, &dat, flags);
		}
	}
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(100);
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_14_2 = { &preamble_0, &test_case_2_9_14_2, &postamble_0 };

#define test_case_2_9_14_2_stream_0 (&test_2_9_14_2)
#define test_case_2_9_14_2_stream_1 (NULL)
#define test_case_2_9_14_2_stream_2 (NULL)

#define tgrp_case_2_9_15 test_group_2
#define numb_case_2_9_15 "2.9.15"
#define name_case_2_9_15 "Perform streamio I_SETSIG - S_MSG."
#define sref_case_2_9_15 sref_case_2_9
#define desc_case_2_9_15 "\
Check that SIGPOLL for S_MSG is generated."

int
test_case_2_9_15(int child)
{
	if (test_ioctl(child, I_SETSIG, S_MSG) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_PSIGNAL, (intptr_t) SIGPOLL) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_15 = { &preamble_2, &test_case_2_9_15, &postamble_2 };

#define test_case_2_9_15_stream_0 (&test_2_9_15)
#define test_case_2_9_15_stream_1 (NULL)
#define test_case_2_9_15_stream_2 (NULL)

#define tgrp_case_2_9_16_1 test_group_2
#define numb_case_2_9_16_1 "2.9.16.1"
#define name_case_2_9_16_1 "Perform streamio I_SETSIG - S_ERROR."
#define sref_case_2_9_16_1 sref_case_2_9
#define desc_case_2_9_16_1 "\
Check that SIGPOLL for S_ERROR is generated when an asyncrhonous read\n\
error reaches the Stream head."

int
test_case_2_9_16_1(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ERROR) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_RDERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_16_1 = { &preamble_2, &test_case_2_9_16_1, &postamble_2 };

#define test_case_2_9_16_1_stream_0 (&test_2_9_16_1)
#define test_case_2_9_16_1_stream_1 (NULL)
#define test_case_2_9_16_1_stream_2 (NULL)

#define tgrp_case_2_9_16_2 test_group_2
#define numb_case_2_9_16_2 "2.9.16.2"
#define name_case_2_9_16_2 "Perform streamio I_SETSIG - S_ERROR."
#define sref_case_2_9_16_2 sref_case_2_9
#define desc_case_2_9_16_2 "\
Check that SIGPOLL for S_ERROR is generated when an asyncrhonous write\n\
error reaches the Stream head."

int
test_case_2_9_16_2(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ERROR) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_16_2 = { &preamble_2, &test_case_2_9_16_2, &postamble_2 };

#define test_case_2_9_16_2_stream_0 (&test_2_9_16_2)
#define test_case_2_9_16_2_stream_1 (NULL)
#define test_case_2_9_16_2_stream_2 (NULL)

#define tgrp_case_2_9_16_3 test_group_2
#define numb_case_2_9_16_3 "2.9.16.3"
#define name_case_2_9_16_3 "Perform streamio I_SETSIG - S_ERROR."
#define sref_case_2_9_16_3 sref_case_2_9
#define desc_case_2_9_16_3 "\
Check that SIGPOLL for S_ERROR is generated when an asyncrhonous error\n\
reaches the Stream head."

int
test_case_2_9_16_3(int child)
{
	if (test_ioctl(child, I_SETSIG, S_ERROR) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_RWERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_16_3 = { &preamble_2, &test_case_2_9_16_3, &postamble_2 };

#define test_case_2_9_16_3_stream_0 (&test_2_9_16_3)
#define test_case_2_9_16_3_stream_1 (NULL)
#define test_case_2_9_16_3_stream_2 (NULL)

#define tgrp_case_2_9_17 test_group_2
#define numb_case_2_9_17 "2.9.17"
#define name_case_2_9_17 "Perform streamio I_SETSIG - S_HANGUP."
#define sref_case_2_9_17 sref_case_2_9
#define desc_case_2_9_17 "\
Check that SIGPOLL for S_HANGUP is generated."

int
test_case_2_9_17(int child)
{
	if (test_ioctl(child, I_SETSIG, S_HANGUP) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_HANGUP, (intptr_t) 0) != __RESULT_SUCCESS && last_errno != ENXIO)
		return __RESULT_FAILURE;
	state++;
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGPOLL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_17 = { &preamble_2, &test_case_2_9_17, &postamble_2 };

#define test_case_2_9_17_stream_0 (&test_2_9_17)
#define test_case_2_9_17_stream_1 (NULL)
#define test_case_2_9_17_stream_2 (NULL)

#define tgrp_case_2_9_18 test_group_2
#define numb_case_2_9_18 "2.9.18"
#define name_case_2_9_18 "Perform streamio I_SETSIG - S_RDBAND/S_BANDURG."
#define sref_case_2_9_18 sref_case_2_9
#define desc_case_2_9_18 "\
Check that SIGURG for S_RDBAND/S_BANDURG is generated."

int
test_case_2_9_18(int child)
{
	if (test_ioctl(child, I_SETSIG, S_RDBAND|S_BANDURG) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_signals() != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	{
		char buf[16] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };
		int band = 1;
		int flags = MSG_BAND;

		if (test_putpmsg(child, &ctl, &dat, band, flags) != __RESULT_SUCCESS)
			return (__RESULT_INCONCLUSIVE);
		state++;
	}
	if (test_waitsig(child) != __RESULT_SUCCESS || last_signum != SIGURG)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9_18 = { &preamble_0, &test_case_2_9_18, &postamble_0 };

#define test_case_2_9_18_stream_0 (&test_2_9_18)
#define test_case_2_9_18_stream_1 (NULL)
#define test_case_2_9_18_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GETSIG
 */
static const char sref_case_2_10[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_GETSIG section.";

#define tgrp_case_2_10_1 test_group_2
#define numb_case_2_10_1 "2.10.1"
#define name_case_2_10_1 "Perform streamio I_GETSIG - EINVAL."
#define sref_case_2_10_1 sref_case_2_10
#define desc_case_2_10_1 "\
Checks that I_GETSIG can be performed on a Stream.  This test attempts\n\
I_GETSIG on a Stream not registered for events.  This test should return\n\
[EINVAL]."

int
test_case_2_10_1(int child)
{
	int sigs = 0;

	if (test_ioctl(child, I_GETSIG, (intptr_t) & sigs) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_1 = { &preamble_0, &test_case_2_10_1, &postamble_0 };

#define test_case_2_10_1_stream_0 (&test_2_10_1)
#define test_case_2_10_1_stream_1 (NULL)
#define test_case_2_10_1_stream_2 (NULL)

#define tgrp_case_2_10_2 test_group_2
#define numb_case_2_10_2 "2.10.2"
#define name_case_2_10_2 "Perform streamio I_GETSIG - EFAULT."
#define sref_case_2_10_2 sref_case_2_10
#define desc_case_2_10_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_10_2(int child)
{
	if (test_ioctl(child, I_GETSIG, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_2 = { &preamble_0, &test_case_2_10_2, &postamble_0 };

#define test_case_2_10_2_stream_0 (&test_2_10_2)
#define test_case_2_10_2_stream_1 (NULL)
#define test_case_2_10_2_stream_2 (NULL)

#define tgrp_case_2_10_3 test_group_2
#define numb_case_2_10_3 "2.10.3"
#define name_case_2_10_3 "Perform streamio I_GETSIG."
#define sref_case_2_10_3 sref_case_2_10
#define desc_case_2_10_3 "\
Checks that signals can be retrieved after they have been set."

int
test_case_2_10_3(int child)
{
	int events;
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_GETSIG, (intptr_t)&events) != __RESULT_SUCCESS || events != S_ALL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_3 = { &preamble_0, &test_case_2_10_3, &postamble_0 };

#define test_case_2_10_3_stream_0 (&test_2_10_3)
#define test_case_2_10_3_stream_1 (NULL)
#define test_case_2_10_3_stream_2 (NULL)

#define tgrp_case_2_10_4 test_group_2
#define numb_case_2_10_4 "2.10.4"
#define name_case_2_10_4 "Perform streamio I_GETSIG."
#define sref_case_2_10_4 sref_case_2_10
#define desc_case_2_10_4 "\
Checks that signals can be retrieved after they have been set \n\
regardless if the Stream is hung up."

int
test_case_2_10_4(int child)
{
	int events;
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_GETSIG, (intptr_t)&events) != __RESULT_SUCCESS || events != S_ALL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_4 = { &preamble_2_1, &test_case_2_10_4, &postamble_2 };

#define test_case_2_10_4_stream_0 (&test_2_10_4)
#define test_case_2_10_4_stream_1 (NULL)
#define test_case_2_10_4_stream_2 (NULL)

#define tgrp_case_2_10_5 test_group_2
#define numb_case_2_10_5 "2.10.5"
#define name_case_2_10_5 "Perform streamio I_GETSIG."
#define sref_case_2_10_5 sref_case_2_10
#define desc_case_2_10_5 "\
Checks that signals can be retrieved after they have been set \n\
regardless if the Stream is read errored."

int
test_case_2_10_5(int child)
{
	int events;
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_GETSIG, (intptr_t)&events) != __RESULT_SUCCESS || events != S_ALL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_5 = { &preamble_2_2, &test_case_2_10_5, &postamble_2 };

#define test_case_2_10_5_stream_0 (&test_2_10_5)
#define test_case_2_10_5_stream_1 (NULL)
#define test_case_2_10_5_stream_2 (NULL)

#define tgrp_case_2_10_6 test_group_2
#define numb_case_2_10_6 "2.10.6"
#define name_case_2_10_6 "Perform streamio I_GETSIG."
#define sref_case_2_10_6 sref_case_2_10
#define desc_case_2_10_6 "\
Checks that signals can be retrieved after they have been set \n\
regardless if the Stream is write errored."

int
test_case_2_10_6(int child)
{
	int events;
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_GETSIG, (intptr_t)&events) != __RESULT_SUCCESS || events != S_ALL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_6 = { &preamble_2_3, &test_case_2_10_6, &postamble_2 };

#define test_case_2_10_6_stream_0 (&test_2_10_6)
#define test_case_2_10_6_stream_1 (NULL)
#define test_case_2_10_6_stream_2 (NULL)

#define tgrp_case_2_10_7 test_group_2
#define numb_case_2_10_7 "2.10.7"
#define name_case_2_10_7 "Perform streamio I_GETSIG."
#define sref_case_2_10_7 sref_case_2_10
#define desc_case_2_10_7 "\
Checks that signals can be retrieved after they have been set \n\
regardless if the Stream is errored."

int
test_case_2_10_7(int child)
{
	int events;
	if (test_ioctl(child, I_SETSIG, S_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_GETSIG, (intptr_t)&events) != __RESULT_SUCCESS || events != S_ALL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_7 = { &preamble_2_4, &test_case_2_10_7, &postamble_2 };

#define test_case_2_10_7_stream_0 (&test_2_10_7)
#define test_case_2_10_7_stream_1 (NULL)
#define test_case_2_10_7_stream_2 (NULL)

#define tgrp_case_2_10_8 test_group_2
#define numb_case_2_10_8 "2.10.8"
#define name_case_2_10_8 "Perform streamio I_GETSIG."
#define sref_case_2_10_8 sref_case_2_10
#define desc_case_2_10_8 "\
Check that I_GETSIG can be performed on a Stream.  Checks that EINVAL is\n\
returned when I_GETSIG is attempted on a Stream that is linked under a\n\
multiplexing driver."

int
test_case_2_10_8(int child)
{
	int events;

	if (test_ioctl(child, I_GETSIG, (intptr_t)&events) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_8 = { &preamble_5, &test_case_2_10_8, &postamble_5 };

#define test_case_2_10_8_stream_0 (&test_2_10_8)
#define test_case_2_10_8_stream_1 (NULL)
#define test_case_2_10_8_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_FIND
 */
static const char sref_case_2_11[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_FIND section.";

#define tgrp_case_2_11_1 test_group_2
#define numb_case_2_11_1 "2.11.1"
#define name_case_2_11_1 "Perform streamio I_FIND - false."
#define sref_case_2_11_1 sref_case_2_11
#define desc_case_2_11_1 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return false (0) because it tests for a bogus module name."

int
test_case_2_11_1(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "bogus") != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_1 = { &preamble_0, &test_case_2_11_1, &postamble_0 };

#define test_case_2_11_1_stream_0 (&test_2_11_1)
#define test_case_2_11_1_stream_1 (NULL)
#define test_case_2_11_1_stream_2 (NULL)

#define tgrp_case_2_11_2 test_group_2
#define numb_case_2_11_2 "2.11.2"
#define name_case_2_11_2 "Perform streamio I_FIND - true."
#define sref_case_2_11_2 sref_case_2_11
#define desc_case_2_11_2 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return true (1) as it tests for the driver name."

int
test_case_2_11_2(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "echo") != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_2 = { &preamble_0, &test_case_2_11_2, &postamble_0 };

#define test_case_2_11_2_stream_0 (&test_2_11_2)
#define test_case_2_11_2_stream_1 (NULL)
#define test_case_2_11_2_stream_2 (NULL)

#define tgrp_case_2_11_3 test_group_2
#define numb_case_2_11_3 "2.11.3"
#define name_case_2_11_3 "Perform streamio I_FIND - EINVAL."
#define sref_case_2_11_3 sref_case_2_11
#define desc_case_2_11_3 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return error [EINVAL] because the module name is too short."

int
test_case_2_11_3(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_3 = { &preamble_0, &test_case_2_11_3, &postamble_0 };

#define test_case_2_11_3_stream_0 (&test_2_11_3)
#define test_case_2_11_3_stream_1 (NULL)
#define test_case_2_11_3_stream_2 (NULL)

#define tgrp_case_2_11_4 test_group_2
#define numb_case_2_11_4 "2.11.4"
#define name_case_2_11_4 "Perform streamio I_FIND - EINVAL."
#define sref_case_2_11_4 sref_case_2_11
#define desc_case_2_11_4 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return error [EINVAL] because the module name is too long."

int
test_case_2_11_4(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "reallylongmodulename") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_4 = { &preamble_0, &test_case_2_11_4, &postamble_0 };

#define test_case_2_11_4_stream_0 (&test_2_11_4)
#define test_case_2_11_4_stream_1 (NULL)
#define test_case_2_11_4_stream_2 (NULL)

#define tgrp_case_2_11_5 test_group_2
#define numb_case_2_11_5 "2.11.5"
#define name_case_2_11_5 "Perform streamio I_FIND - true."
#define sref_case_2_11_5 sref_case_2_11
#define desc_case_2_11_5 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return true (1) as it pushes and tests for the same module name."

int
test_case_2_11_5(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nullmod") != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FIND, (intptr_t) "nullmod") != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_5 = { &preamble_0, &test_case_2_11_5, &postamble_0 };

#define test_case_2_11_5_stream_0 (&test_2_11_5)
#define test_case_2_11_5_stream_1 (NULL)
#define test_case_2_11_5_stream_2 (NULL)

#define tgrp_case_2_11_6 test_group_2
#define numb_case_2_11_6 "2.11.6"
#define name_case_2_11_6 "Perform streamio I_FIND - true."
#define sref_case_2_11_6 sref_case_2_11
#define desc_case_2_11_6 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return true (1) even for a hung up Stream."

int
test_case_2_11_6(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "testmod") != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_6 = { &preamble_2_1, &test_case_2_11_6, &postamble_2 };

#define test_case_2_11_6_stream_0 (&test_2_11_6)
#define test_case_2_11_6_stream_1 (NULL)
#define test_case_2_11_6_stream_2 (NULL)

#define tgrp_case_2_11_7 test_group_2
#define numb_case_2_11_7 "2.11.7"
#define name_case_2_11_7 "Perform streamio I_FIND - true."
#define sref_case_2_11_7 sref_case_2_11
#define desc_case_2_11_7 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return true (1) even for a read errored Stream."

int
test_case_2_11_7(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "testmod") != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_7 = { &preamble_2_2, &test_case_2_11_7, &postamble_2 };

#define test_case_2_11_7_stream_0 (&test_2_11_7)
#define test_case_2_11_7_stream_1 (NULL)
#define test_case_2_11_7_stream_2 (NULL)

#define tgrp_case_2_11_8 test_group_2
#define numb_case_2_11_8 "2.11.8"
#define name_case_2_11_8 "Perform streamio I_FIND - true."
#define sref_case_2_11_8 sref_case_2_11
#define desc_case_2_11_8 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return true (1) even for a write errored Stream."

int
test_case_2_11_8(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "testmod") != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_8 = { &preamble_2_3, &test_case_2_11_8, &postamble_2 };

#define test_case_2_11_8_stream_0 (&test_2_11_8)
#define test_case_2_11_8_stream_1 (NULL)
#define test_case_2_11_8_stream_2 (NULL)

#define tgrp_case_2_11_9 test_group_2
#define numb_case_2_11_9 "2.11.9"
#define name_case_2_11_9 "Perform streamio I_FIND - true."
#define sref_case_2_11_9 sref_case_2_11
#define desc_case_2_11_9 "\
Checks that I_FIND can be performed on a Stream.  This test should\n\
return true (1) even for a errored Stream."

int
test_case_2_11_9(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "testmod") != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_9 = { &preamble_2_4, &test_case_2_11_9, &postamble_2 };

#define test_case_2_11_9_stream_0 (&test_2_11_9)
#define test_case_2_11_9_stream_1 (NULL)
#define test_case_2_11_9_stream_2 (NULL)

#define tgrp_case_2_11_10 test_group_2
#define numb_case_2_11_10 "2.11.10"
#define name_case_2_11_10 "Perform streamio I_FIND - EINVAL."
#define sref_case_2_11_10 sref_case_2_11
#define desc_case_2_11_10 "\
Checks that I_FIND can be performed on a Stream.  Checks that EINVAL is\n\
returned when I_FIND is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_2_11_10(int child)
{
	if (test_ioctl(child, I_FIND, (intptr_t) "echo") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11_10 = { &preamble_5, &test_case_2_11_10, &postamble_5 };

#define test_case_2_11_10_stream_0 (&test_2_11_10)
#define test_case_2_11_10_stream_1 (NULL)
#define test_case_2_11_10_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_LINK
 */
static const char sref_case_2_12[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_LINK section.";

#define tgrp_case_2_12_1 test_group_2
#define numb_case_2_12_1 "2.12.1"
#define name_case_2_12_1 "Perform streamio I_LINK - EINVAL."
#define sref_case_2_12_1 sref_case_2_12
#define desc_case_2_12_1 "\
Checks that I_LINK can be performed on a Stream.\n\
File descriptor does not support multiplexing."

int
test_case_2_12_1(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) 10) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_1 = { &preamble_0, &test_case_2_12_1, &postamble_0 };

#define test_case_2_12_1_stream_0 (&test_2_12_1)
#define test_case_2_12_1_stream_1 (NULL)
#define test_case_2_12_1_stream_2 (NULL)

#define tgrp_case_2_12_2 test_group_2
#define numb_case_2_12_2 "2.12.2"
#define name_case_2_12_2 "Perform streamio I_LINK - EINVAL."
#define sref_case_2_12_2 sref_case_2_12
#define desc_case_2_12_2 "\
Checks that I_LINK can be performed on a Stream.\n\
File descriptor argument is not a Stream."

int
test_case_2_12_2(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) 1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_2 = { &preamble_1, &test_case_2_12_2, &postamble_1 };

#define test_case_2_12_2_stream_0 (&test_2_12_2)
#define test_case_2_12_2_stream_1 (NULL)
#define test_case_2_12_2_stream_2 (NULL)

#define tgrp_case_2_12_3 test_group_2
#define numb_case_2_12_3 "2.12.3"
#define name_case_2_12_3 "Perform streamio I_LINK - EBADF."
#define sref_case_2_12_3 sref_case_2_12
#define desc_case_2_12_3 "\
Checks that I_LINK can be performed on a Stream.\n\
File descriptor argument is not a valid, open file descriptor."

int
test_case_2_12_3(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_3 = { &preamble_1, &test_case_2_12_3, &postamble_1 };

#define test_case_2_12_3_stream_0 (&test_2_12_3)
#define test_case_2_12_3_stream_1 (NULL)
#define test_case_2_12_3_stream_2 (NULL)

#define tgrp_case_2_12_4 test_group_2
#define numb_case_2_12_4 "2.12.4"
#define name_case_2_12_4 "Perform streamio I_LINK - ENXIO."
#define sref_case_2_12_4 sref_case_2_12
#define desc_case_2_12_4 "\
Checks that I_LINK can be performed on a Stream.\n\
Hangup received on file descriptor."

int
test_case_2_12_4(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_4 = { &preamble_4_2_1, &test_case_2_12_4, &postamble_4_2 };

#define test_case_2_12_4_stream_0 (&test_2_12_4)
#define test_case_2_12_4_stream_1 (NULL)
#define test_case_2_12_4_stream_2 (NULL)

#define tgrp_case_2_12_5 test_group_2
#define numb_case_2_12_5 "2.12.5"
#define name_case_2_12_5 "Perform streamio I_LINK - EPROTO."
#define sref_case_2_12_5 sref_case_2_12
#define desc_case_2_12_5 "\
Checks that I_LINK can be performed on a Stream.\n\
Read error received on file descriptor."

int
test_case_2_12_5(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_5 = { &preamble_4_2_2, &test_case_2_12_5, &postamble_4_2 };

#define test_case_2_12_5_stream_0 (&test_2_12_5)
#define test_case_2_12_5_stream_1 (NULL)
#define test_case_2_12_5_stream_2 (NULL)

#define tgrp_case_2_12_6 test_group_2
#define numb_case_2_12_6 "2.12.6"
#define name_case_2_12_6 "Perform streamio I_LINK - EPROTO."
#define sref_case_2_12_6 sref_case_2_12
#define desc_case_2_12_6 "\
Checks that I_LINK can be performed on a Stream.\n\
Write error received on file descriptor."

int
test_case_2_12_6(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_6 = { &preamble_4_2_3, &test_case_2_12_6, &postamble_4_2 };

#define test_case_2_12_6_stream_0 (&test_2_12_6)
#define test_case_2_12_6_stream_1 (NULL)
#define test_case_2_12_6_stream_2 (NULL)

#define tgrp_case_2_12_7 test_group_2
#define numb_case_2_12_7 "2.12.7"
#define name_case_2_12_7 "Perform streamio I_LINK - EPROTO."
#define sref_case_2_12_7 sref_case_2_12
#define desc_case_2_12_7 "\
Checks that I_LINK can be performed on a Stream.\n\
Error received on file descriptor."

int
test_case_2_12_7(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_7 = { &preamble_4_2_4, &test_case_2_12_7, &postamble_4_2 };

#define test_case_2_12_7_stream_0 (&test_2_12_7)
#define test_case_2_12_7_stream_1 (NULL)
#define test_case_2_12_7_stream_2 (NULL)

#define tgrp_case_2_12_8 test_group_2
#define numb_case_2_12_8 "2.12.8"
#define name_case_2_12_8 "Perform streamio I_LINK."
#define sref_case_2_12_8 sref_case_2_12
#define desc_case_2_12_8 "\
Checks that I_LINK can be performed on a Stream."

int
test_case_2_12_8(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) test_fd[child + 1]) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_UNLINK, (intptr_t) last_retval) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_8 = { &preamble_4, &test_case_2_12_8, &postamble_4 };

#define test_case_2_12_8_stream_0 (&test_2_12_8)
#define test_case_2_12_8_stream_1 (NULL)
#define test_case_2_12_8_stream_2 (NULL)

#define tgrp_case_2_12_9 test_group_2
#define numb_case_2_12_9 "2.12.9"
#define name_case_2_12_9 "Perform streamio I_LINK."
#define sref_case_2_12_9 sref_case_2_12
#define desc_case_2_12_9 "\
Checks that I_LINK can be performed on a Stream.  This test checks\n\
implicit unlinking of temporary links on close."

int
test_case_2_12_9(int child)
{
	if (test_ioctl(child, I_LINK, (intptr_t) test_fd[child + 1]) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_9 = { &preamble_4, &test_case_2_12_9, &postamble_4 };

#define test_case_2_12_9_stream_0 (&test_2_12_9)
#define test_case_2_12_9_stream_1 (NULL)
#define test_case_2_12_9_stream_2 (NULL)

#define tgrp_case_2_12_10 test_group_2
#define numb_case_2_12_10 "2.12.10"
#define name_case_2_12_10 "Perform streamio I_LINK - EINVAL."
#define sref_case_2_12_10 sref_case_2_12
#define desc_case_2_12_10 "\
Checks that I_LINK can be performed on a Stream.  Check that EINVAL is\n\
returned when the Stream is already linked underneath a Multiplexing\n\
Driver."

int
test_case_2_12_10(int child)
{
	if (test_ioctl(child + 1, I_LINK, (intptr_t) test_fd[child]) == __RESULT_SUCCESS || last_errno != EINVAL)
		return __RESULT_FAILURE;
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_12_10 = { &preamble_5, &test_case_2_12_10, &postamble_5 };

#define test_case_2_12_10_stream_0 (&test_2_12_10)
#define test_case_2_12_10_stream_1 (NULL)
#define test_case_2_12_10_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_UNLINK
 */
static const char sref_case_2_13[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_UNLINK section.";

#define tgrp_case_2_13_1 test_group_2
#define numb_case_2_13_1 "2.13.1"
#define name_case_2_13_1 "Perform streamio I_UNLINK - EINVAL."
#define sref_case_2_13_1 sref_case_2_13
#define desc_case_2_13_1 "\
Checks that I_UNLINK can be performed on a Stream.  This test uses \n\
an invalid multiplexer identifier and should return EINVAL."

int
test_case_2_13_1(int child)
{
	if (test_ioctl(child, I_UNLINK, (intptr_t) 5) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_13_1 = { &preamble_0, &test_case_2_13_1, &postamble_0 };

#define test_case_2_13_1_stream_0 (&test_2_13_1)
#define test_case_2_13_1_stream_1 (NULL)
#define test_case_2_13_1_stream_2 (NULL)

#define tgrp_case_2_13_2 test_group_2
#define numb_case_2_13_2 "2.13.2"
#define name_case_2_13_2 "Perform streamio I_UNLINK."
#define sref_case_2_13_2 sref_case_2_13
#define desc_case_2_13_2 "\
Checks that I_UNLINK can be performed on a Stream.  This tests uses \n\
MUXID_ALL, but there are no Streams linked.  It should succeed."

int
test_case_2_13_2(int child)
{
	if (test_ioctl(child, I_UNLINK, (intptr_t) MUXID_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_13_2 = { &preamble_0, &test_case_2_13_2, &postamble_0 };

#define test_case_2_13_2_stream_0 (&test_2_13_2)
#define test_case_2_13_2_stream_1 (NULL)
#define test_case_2_13_2_stream_2 (NULL)

#define tgrp_case_2_13_3 test_group_2
#define numb_case_2_13_3 "2.13.3"
#define name_case_2_13_3 "Perform streamio I_UNLINK - ENXIO."
#define sref_case_2_13_3 sref_case_2_13
#define desc_case_2_13_3 "\
Checks that I_UNLINK can be performed on a hung up Stream.\n\
This test should return ENXIO."

int
test_case_2_13_3(int child)
{
	if (test_ioctl(child, I_UNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_13_3 = { &preamble_4_2_1, &test_case_2_13_3, &postamble_4_2 };

#define test_case_2_13_3_stream_0 (&test_2_13_3)
#define test_case_2_13_3_stream_1 (NULL)
#define test_case_2_13_3_stream_2 (NULL)

#define tgrp_case_2_13_4 test_group_2
#define numb_case_2_13_4 "2.13.4"
#define name_case_2_13_4 "Perform streamio I_UNLINK - EPROTO."
#define sref_case_2_13_4 sref_case_2_13
#define desc_case_2_13_4 "\
Checks that I_UNLINK can be performed on a read errored Stream.\n\
This test should return EPROTO."

int
test_case_2_13_4(int child)
{
	if (test_ioctl(child, I_UNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_13_4 = { &preamble_4_2_2, &test_case_2_13_4, &postamble_4_2 };

#define test_case_2_13_4_stream_0 (&test_2_13_4)
#define test_case_2_13_4_stream_1 (NULL)
#define test_case_2_13_4_stream_2 (NULL)

#define tgrp_case_2_13_5 test_group_2
#define numb_case_2_13_5 "2.13.5"
#define name_case_2_13_5 "Perform streamio I_UNLINK - EPROTO."
#define sref_case_2_13_5 sref_case_2_13
#define desc_case_2_13_5 "\
Checks that I_UNLINK can be performed on a write errored Stream.\n\
This test should return EPROTO."

int
test_case_2_13_5(int child)
{
	if (test_ioctl(child, I_UNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_13_5 = { &preamble_4_2_3, &test_case_2_13_5, &postamble_4_2 };

#define test_case_2_13_5_stream_0 (&test_2_13_5)
#define test_case_2_13_5_stream_1 (NULL)
#define test_case_2_13_5_stream_2 (NULL)

#define tgrp_case_2_13_6 test_group_2
#define numb_case_2_13_6 "2.13.6"
#define name_case_2_13_6 "Perform streamio I_UNLINK - EPROTO."
#define sref_case_2_13_6 sref_case_2_13
#define desc_case_2_13_6 "\
Checks that I_UNLINK can be performed on an errored Stream.\n\
This test should return EPROTO."

int
test_case_2_13_6(int child)
{
	if (test_ioctl(child, I_UNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_13_6 = { &preamble_4_2_4, &test_case_2_13_6, &postamble_4_2 };

#define test_case_2_13_6_stream_0 (&test_2_13_6)
#define test_case_2_13_6_stream_1 (NULL)
#define test_case_2_13_6_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_RECVFD
 */
static const char sref_case_2_14[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_RECVFD section.";

#define tgrp_case_2_14_1 test_group_2
#define numb_case_2_14_1 "2.14.1"
#define name_case_2_14_1 "Perform streamio I_RECVFD - EAGAIN."
#define sref_case_2_14_1 sref_case_2_14
#define desc_case_2_14_1 "\
Checks that I_RECVFD can be performed on a Stream.  Check that EAGAIN is\n\
returned when the file descriptor is set for nonblocking operation and\n\
the Stream read queue is empty."

int
test_case_2_14_1(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_1 = { &preamble_0, &test_case_2_14_1, &postamble_0 };

#define test_case_2_14_1_stream_0 (&test_2_14_1)
#define test_case_2_14_1_stream_1 (NULL)
#define test_case_2_14_1_stream_2 (NULL)

#define tgrp_case_2_14_2 test_group_2
#define numb_case_2_14_2 "2.14.2"
#define name_case_2_14_2 "Perform streamio I_RECVFD - ENXIO."
#define sref_case_2_14_2 sref_case_2_14
#define desc_case_2_14_2 "\
Checks that I_RECVFD can be performed on a Stream.  Check that ENXIO is\n\
returned when I_RECVFD is called on a Stream with an empty read queue."

int
test_case_2_14_2(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_2 = { &preamble_6_2_1, &test_case_2_14_2, &postamble_6_2 };

#define test_case_2_14_2_stream_0 (&test_2_14_2)
#define test_case_2_14_2_stream_1 (NULL)
#define test_case_2_14_2_stream_2 (NULL)

#define tgrp_case_2_14_3 test_group_2
#define numb_case_2_14_3 "2.14.3"
#define name_case_2_14_3 "Perform streamio I_RECVFD - EPROTO."
#define sref_case_2_14_3 sref_case_2_14
#define desc_case_2_14_3 "\
Checks that I_RECVFD can be performed on a Stream.  Check that EPROTO is \n\
returned when an M_ERROR message containing EPROTO for the read-side is \n\
received before the I_RECVFD command is issued."

int
test_case_2_14_3(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_3 = { &preamble_6_2_2, &test_case_2_14_3, &postamble_6_2 };

#define test_case_2_14_3_stream_0 (&test_2_14_3)
#define test_case_2_14_3_stream_1 (NULL)
#define test_case_2_14_3_stream_2 (NULL)

#define tgrp_case_2_14_4 test_group_2
#define numb_case_2_14_4 "2.14.4"
#define name_case_2_14_4 "Perform streamio I_RECVFD - EAGAIN."
#define sref_case_2_14_4 sref_case_2_14
#define desc_case_2_14_4 "\
Checks that I_RECVFD can be performed on Stream, in spite of receiving \n\
an M_ERROR message containing an EPROTO for the write-side before the \n\
I_RECVFD command is issued."

int
test_case_2_14_4(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_4 = { &preamble_6_2_3, &test_case_2_14_4, &postamble_6_2 };

#define test_case_2_14_4_stream_0 (&test_2_14_4)
#define test_case_2_14_4_stream_1 (NULL)
#define test_case_2_14_4_stream_2 (NULL)

#define tgrp_case_2_14_5 test_group_2
#define numb_case_2_14_5 "2.14.5"
#define name_case_2_14_5 "Perform streamio I_RECVFD - EPROTO."
#define sref_case_2_14_5 sref_case_2_14
#define desc_case_2_14_5 "\
Checks that I_RECVFD can be performed on a Stream.  Check that EPROTO is \n\
returned when an M_ERROR message containing EPROTO (for both sides) is \n\
received before the I_RECVFD command is issued."

int
test_case_2_14_5(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_5 = { &preamble_6_2_4, &test_case_2_14_5, &postamble_6_2 };

#define test_case_2_14_5_stream_0 (&test_2_14_5)
#define test_case_2_14_5_stream_1 (NULL)
#define test_case_2_14_5_stream_2 (NULL)

#define tgrp_case_2_14_6 test_group_2
#define numb_case_2_14_6 "2.14.6"
#define name_case_2_14_6 "Perform streamio I_RECVFD - EINVAL."
#define sref_case_2_14_6 sref_case_2_14
#define desc_case_2_14_6 "\
Checks that I_RECVFD can be performed on a Stream.  Check that EINVAL is \n\
returned when the Stream is linked under a Multiplexing Driver."

int
test_case_2_14_6(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_6 = { &preamble_5, &test_case_2_14_6, &postamble_5 };

#define test_case_2_14_6_stream_0 (&test_2_14_6)
#define test_case_2_14_6_stream_1 (NULL)
#define test_case_2_14_6_stream_2 (NULL)

#define tgrp_case_2_14_7 test_group_2
#define numb_case_2_14_7 "2.14.7"
#define name_case_2_14_7 "Perform streamio I_RECVFD."
#define sref_case_2_14_7 sref_case_2_14
#define desc_case_2_14_7 "\
Checks that I_RECVFD can be performed on a Stream."

int
test_case_2_14_7(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_7 = { &preamble_6_3, &test_case_2_14_7, &postamble_6_3 };

#define test_case_2_14_7_stream_0 (&test_2_14_7)
#define test_case_2_14_7_stream_1 (NULL)
#define test_case_2_14_7_stream_2 (NULL)

#define tgrp_case_2_14_8 test_group_2
#define numb_case_2_14_8 "2.14.8"
#define name_case_2_14_8 "Perform streamio I_RECVFD - EINTR."
#define sref_case_2_14_8 sref_case_2_14
#define desc_case_2_14_8 "\
Checks that I_RECVFD can be performed on a Stream.  Checks that I_RECVD\n\
will block awaiting arrival of an M_PASSFP message on the Stream head,\n\
and that the wait can be interrupted by a signal and will return EINTR."

int
test_case_2_14_8(int child)
{
	struct strrecvfd recvfd;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14_8 = { &preamble_6, &test_case_2_14_8, &postamble_6 };

#define test_case_2_14_8_stream_0 (&test_2_14_8)
#define test_case_2_14_8_stream_1 (NULL)
#define test_case_2_14_8_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_PEEK
 */
static const char sref_case_2_15[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_PEEK section.";

#define tgrp_case_2_15_1 test_group_2
#define numb_case_2_15_1 "2.15.1"
#define name_case_2_15_1 "Perform streamio I_PEEK - false."
#define sref_case_2_15_1 sref_case_2_15
#define desc_case_2_15_1 "\
Checks that I_PEEK can be performed on a Stream."

int
test_case_2_15_1(int child)
{
	struct strpeek peek = { {0, 0, NULL}, {0, 0, NULL}, 0 };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_1 = { &preamble_0, &test_case_2_15_1, &postamble_0 };

#define test_case_2_15_1_stream_0 (&test_2_15_1)
#define test_case_2_15_1_stream_1 (NULL)
#define test_case_2_15_1_stream_2 (NULL)

#define tgrp_case_2_15_2 test_group_2
#define numb_case_2_15_2 "2.15.2"
#define name_case_2_15_2 "Perform streamio I_PEEK - false."
#define sref_case_2_15_2 sref_case_2_15
#define desc_case_2_15_2 "\
Checks that I_PEEK can be performed on a hung up Stream."

int
test_case_2_15_2(int child)
{
	struct strpeek peek = { {0, 0, NULL}, {0, 0, NULL}, 0 };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_2 = { &preamble_2_1, &test_case_2_15_2, &postamble_2 };

#define test_case_2_15_2_stream_0 (&test_2_15_2)
#define test_case_2_15_2_stream_1 (NULL)
#define test_case_2_15_2_stream_2 (NULL)

#define tgrp_case_2_15_3 test_group_2
#define numb_case_2_15_3 "2.15.3"
#define name_case_2_15_3 "Perform streamio I_PEEK - EPROTO."
#define sref_case_2_15_3 sref_case_2_15
#define desc_case_2_15_3 "\
Checks that I_PEEK returns EPROTO when performed on a read errored\n\
Stream."

int
test_case_2_15_3(int child)
{
	struct strpeek peek = { {0, 0, NULL}, {0, 0, NULL}, 0 };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_3 = { &preamble_2_2, &test_case_2_15_3, &postamble_2 };

#define test_case_2_15_3_stream_0 (&test_2_15_3)
#define test_case_2_15_3_stream_1 (NULL)
#define test_case_2_15_3_stream_2 (NULL)

#define tgrp_case_2_15_4 test_group_2
#define numb_case_2_15_4 "2.15.4"
#define name_case_2_15_4 "Perform streamio I_PEEK - false."
#define sref_case_2_15_4 sref_case_2_15
#define desc_case_2_15_4 "\
Checks that I_PEEK can be performed on a write errored Stream."

int
test_case_2_15_4(int child)
{
	struct strpeek peek = { {0, 0, NULL}, {0, 0, NULL}, 0 };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_4 = { &preamble_2_3, &test_case_2_15_4, &postamble_2 };

#define test_case_2_15_4_stream_0 (&test_2_15_4)
#define test_case_2_15_4_stream_1 (NULL)
#define test_case_2_15_4_stream_2 (NULL)

#define tgrp_case_2_15_5 test_group_2
#define numb_case_2_15_5 "2.15.5"
#define name_case_2_15_5 "Perform streamio I_PEEK - EPROTO."
#define sref_case_2_15_5 sref_case_2_15
#define desc_case_2_15_5 "\
Checks that I_PEEK returns EPROTO when performed on an errored\n\
Stream."

int
test_case_2_15_5(int child)
{
	struct strpeek peek = { {0, 0, NULL}, {0, 0, NULL}, 0 };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_5 = { &preamble_2_4, &test_case_2_15_5, &postamble_2 };

#define test_case_2_15_5_stream_0 (&test_2_15_5)
#define test_case_2_15_5_stream_1 (NULL)
#define test_case_2_15_5_stream_2 (NULL)

#define tgrp_case_2_15_6 test_group_2
#define numb_case_2_15_6 "2.15.6"
#define name_case_2_15_6 "Perform streamio I_PEEK - EINVAL."
#define sref_case_2_15_6 sref_case_2_15
#define desc_case_2_15_6 "\
Checks that I_PEEK can be performed on an errored Stream.  Check that\n\
EINVAL is returned when the Stream is linked under a Multiplexing\n\
Driver."

int
test_case_2_15_6(int child)
{
	struct strpeek peek = { {0, 0, NULL}, {0, 0, NULL}, 0 };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_6 = { &preamble_5, &test_case_2_15_6, &postamble_5 };

#define test_case_2_15_6_stream_0 (&test_2_15_6)
#define test_case_2_15_6_stream_1 (NULL)
#define test_case_2_15_6_stream_2 (NULL)

#define tgrp_case_2_15_7 test_group_2
#define numb_case_2_15_7 "2.15.7"
#define name_case_2_15_7 "Perform streamio I_PEEK - true."
#define sref_case_2_15_7 sref_case_2_15
#define desc_case_2_15_7 "\
Checks that I_PEEK can be performed on a Stream.  Check that I_PEEK can\n\
be used to peek a message on the read queue."

int
test_case_2_15_7(int child)
{
	char scbuf[24] = "";
	char sdbuf[24] = "";
	struct strpeek peek = { {sizeof(scbuf), -1, scbuf}, {sizeof(sdbuf), -1, sdbuf}, 0 };
	char pcbuf[16] = "012345678901234";
	char pdbuf[12] = "09876543210";
	struct strbuf ctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf dat = { -1, sizeof(pdbuf), pdbuf };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, RS_HIPRI) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (peek.flags != RS_HIPRI)
		return (__RESULT_FAILURE);
	state++;
	if (peek.ctlbuf.len != sizeof(pcbuf))
		return (__RESULT_FAILURE);
	state++;
	if (peek.databuf.len != sizeof(pdbuf))
		return (__RESULT_FAILURE);
	state++;
	if (strncmp(scbuf, pcbuf, sizeof(pcbuf)) != 0)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp(sdbuf, pdbuf, sizeof(pdbuf)) != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_7 = { &preamble_0, &test_case_2_15_7, &postamble_0 };

#define test_case_2_15_7_stream_0 (&test_2_15_7)
#define test_case_2_15_7_stream_1 (NULL)
#define test_case_2_15_7_stream_2 (NULL)

#define tgrp_case_2_15_8 test_group_2
#define numb_case_2_15_8 "2.15.8"
#define name_case_2_15_8 "Perform streamio I_PEEK - true."
#define sref_case_2_15_8 sref_case_2_15
#define desc_case_2_15_8 "\
Checks that I_PEEK can be performed on a Stream.  Check that I_PEEK can\n\
be used to peek an RS_HIPRI message on the read queue."

int
test_case_2_15_8(int child)
{
	char scbuf[24] = "";
	char sdbuf[24] = "";
	struct strpeek peek = { {sizeof(scbuf), -1, scbuf}, {sizeof(sdbuf), -1, sdbuf}, RS_HIPRI };
	char pcbuf[16] = "012345678901234";
	char pdbuf[12] = "09876543210";
	struct strbuf ctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf dat = { -1, sizeof(pdbuf), pdbuf };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, RS_HIPRI) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (peek.flags != RS_HIPRI)
		return (__RESULT_FAILURE);
	state++;
	if (peek.ctlbuf.len != sizeof(pcbuf))
		return (__RESULT_FAILURE);
	state++;
	if (peek.databuf.len != sizeof(pdbuf))
		return (__RESULT_FAILURE);
	state++;
	if (strncmp(scbuf, pcbuf, sizeof(pcbuf)) != 0)
		return (__RESULT_FAILURE);
	state++;
	if (strncmp(sdbuf, pdbuf, sizeof(pdbuf)) != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_8 = { &preamble_0, &test_case_2_15_8, &postamble_0 };

#define test_case_2_15_8_stream_0 (&test_2_15_8)
#define test_case_2_15_8_stream_1 (NULL)
#define test_case_2_15_8_stream_2 (NULL)

#define tgrp_case_2_15_9 test_group_2
#define numb_case_2_15_9 "2.15.9"
#define name_case_2_15_9 "Perform streamio I_PEEK - false."
#define sref_case_2_15_9 sref_case_2_15
#define desc_case_2_15_9 "\
Checks that I_PEEK can be performed on a Stream.  Check that I_PEEK will\n\
not peek a normal priority message on the queue when RS_HIPRI is set."

int
test_case_2_15_9(int child)
{
	char scbuf[24] = "";
	char sdbuf[24] = "";
	struct strpeek peek = { {sizeof(scbuf), -1, scbuf}, {sizeof(sdbuf), -1, sdbuf}, RS_HIPRI };
	char pcbuf[16] = "012345678901234";
	char pdbuf[12] = "09876543210";
	struct strbuf ctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf dat = { -1, sizeof(pdbuf), pdbuf };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15_9 = { &preamble_0, &test_case_2_15_9, &postamble_0 };

#define test_case_2_15_9_stream_0 (&test_2_15_9)
#define test_case_2_15_9_stream_1 (NULL)
#define test_case_2_15_9_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_FDINSERT
 */
static const char sref_case_2_16[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_FDINSERT section.";

#define tgrp_case_2_16_1 test_group_2
#define numb_case_2_16_1 "2.16.1"
#define name_case_2_16_1 "Perform streamio I_FDINSERT."
#define sref_case_2_16_1 sref_case_2_16
#define desc_case_2_16_1 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that a\n\
Stream can insert its own file descriptor."

int
test_case_2_16_1(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = -1;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = -1;
	fdi.databuf.len = -1;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_1 = { &preamble_0, &test_case_2_16_1, &postamble_0 };

#define test_case_2_16_1_stream_0 (&test_2_16_1)
#define test_case_2_16_1_stream_1 (NULL)
#define test_case_2_16_1_stream_2 (NULL)

#define tgrp_case_2_16_2_1 test_group_2
#define numb_case_2_16_2_1 "2.16.2.1"
#define name_case_2_16_2_1 "Perform streamio I_FDINSERT - ENXIO."
#define sref_case_2_16_2_1 sref_case_2_16
#define desc_case_2_16_2_1 "\
Checks that I_FDINSERT can be performed on a Stream.  This tests\n\
attempts I_FDINSERT on a hung up Stream (fildes) and should return\n\
ENXIO."

int
test_case_2_16_2_1(int child)
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
	fdi.fildes = test_fd[child + 1];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_2_1 = { &preamble_6_2_1, &test_case_2_16_2_1, &postamble_6_2 };

#define test_case_2_16_2_1_stream_0 (&test_2_16_2_1)
#define test_case_2_16_2_1_stream_1 (NULL)
#define test_case_2_16_2_1_stream_2 (NULL)

#define tgrp_case_2_16_2_2 test_group_2
#define numb_case_2_16_2_2 "2.16.2.2"
#define name_case_2_16_2_2 "Perform streamio I_FDINSERT - ENXIO."
#define sref_case_2_16_2_2 sref_case_2_16
#define desc_case_2_16_2_2 "\
Checks that I_FDINSERT can be performed on a Stream.  This tests\n\
attempts I_FDINSERT on a hung up Stream (fdi.fildes) and should return\n\
ENXIO."

int
test_case_2_16_2_2(int child)
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
	fdi.fildes = test_fd[child + 1];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_2_2 = { &preamble_6_2_1, &test_case_2_16_2_2, &postamble_6_2 };

#define test_case_2_16_2_2_stream_0 (&test_2_16_2_2)
#define test_case_2_16_2_2_stream_1 (NULL)
#define test_case_2_16_2_2_stream_2 (NULL)

#define tgrp_case_2_16_3_1 test_group_2
#define numb_case_2_16_3_1 "2.16.3.1"
#define name_case_2_16_3_1 "Perform streamio I_FDINSERT."
#define sref_case_2_16_3_1 sref_case_2_16
#define desc_case_2_16_3_1 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that\n\
I_FDINSERT on a read errored Stream (fildes) can be successful."

int
test_case_2_16_3_1(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;

	fdi.ctlbuf.maxlen = -1;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = -1;
	fdi.databuf.len = -1;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = test_fd[child + 1];
	fdi.offset = 0;

	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_3_1 = { &preamble_6_2_2, &test_case_2_16_3_1, &postamble_6_2 };

#define test_case_2_16_3_1_stream_0 (&test_2_16_3_1)
#define test_case_2_16_3_1_stream_1 (NULL)
#define test_case_2_16_3_1_stream_2 (NULL)

#define tgrp_case_2_16_3_2 test_group_2
#define numb_case_2_16_3_2 "2.16.3.2"
#define name_case_2_16_3_2 "Perform streamio I_FDINSERT - EPROTO."
#define sref_case_2_16_3_2 sref_case_2_16
#define desc_case_2_16_3_2 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EPROTO\n\
is returned when I_FDINSERT is attempted  on a Stream (fdi.fildes) that\n\
has received an asyncrhonous EPROTO read error."

int
test_case_2_16_3_2(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;

	fdi.ctlbuf.maxlen = -1;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = -1;
	fdi.databuf.len = -1;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;

	if (test_ioctl(child + 1, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_3_2 = { &preamble_6_2_2, &test_case_2_16_3_2, &postamble_6_2 };

#define test_case_2_16_3_2_stream_0 (&test_2_16_3_2)
#define test_case_2_16_3_2_stream_1 (NULL)
#define test_case_2_16_3_2_stream_2 (NULL)

#define tgrp_case_2_16_4_1 test_group_2
#define numb_case_2_16_4_1 "2.16.4.1"
#define name_case_2_16_4_1 "Perform streamio I_FDINSERT - EPROTO."
#define sref_case_2_16_4_1 sref_case_2_16
#define desc_case_2_16_4_1 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EPROTO\n\
is returned when I_FDINSERT is attempted on a Stream (fildes) that has\n\
received an asyncrhonous EPROTO write error."

int
test_case_2_16_4_1(int child)
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
	fdi.fildes = test_fd[child + 1];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_4_1 = { &preamble_6_2_3, &test_case_2_16_4_1, &postamble_6_2 };

#define test_case_2_16_4_1_stream_0 (&test_2_16_4_1)
#define test_case_2_16_4_1_stream_1 (NULL)
#define test_case_2_16_4_1_stream_2 (NULL)

#define tgrp_case_2_16_4_2 test_group_2
#define numb_case_2_16_4_2 "2.16.4.2"
#define name_case_2_16_4_2 "Perform streamio I_FDINSERT - EPROTO."
#define sref_case_2_16_4_2 sref_case_2_16
#define desc_case_2_16_4_2 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EPROTO\n\
is returned when I_FDINSERT is attempted on a Stream (fdi.fildes) that\n\
has received an asyncrhonous EPROTO write error."

int
test_case_2_16_4_2(int child)
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
	if (test_ioctl(child + 1, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_4_2 = { &preamble_6_2_3, &test_case_2_16_4_2, &postamble_6_2 };

#define test_case_2_16_4_2_stream_0 (&test_2_16_4_2)
#define test_case_2_16_4_2_stream_1 (NULL)
#define test_case_2_16_4_2_stream_2 (NULL)

#define tgrp_case_2_16_5_1 test_group_2
#define numb_case_2_16_5_1 "2.16.5.1"
#define name_case_2_16_5_1 "Perform streamio I_FDINSERT - EPROTO."
#define sref_case_2_16_5_1 sref_case_2_16
#define desc_case_2_16_5_1 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EPROTO\n\
is returned when I_FDINSERT is attempted on a Stream (fildes) that has\n\
received an asyncrhonous EPROTO error."

int
test_case_2_16_5_1(int child)
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
	fdi.fildes = test_fd[child + 1];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_5_1 = { &preamble_6_2_4, &test_case_2_16_5_1, &postamble_6_2 };

#define test_case_2_16_5_1_stream_0 (&test_2_16_5_1)
#define test_case_2_16_5_1_stream_1 (NULL)
#define test_case_2_16_5_1_stream_2 (NULL)

#define tgrp_case_2_16_5_2 test_group_2
#define numb_case_2_16_5_2 "2.16.5.2"
#define name_case_2_16_5_2 "Perform streamio I_FDINSERT - EPROTO."
#define sref_case_2_16_5_2 sref_case_2_16
#define desc_case_2_16_5_2 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EPROTO\n\
is returned when I_FDINSERT is attempted on a Stream (fdi.fildes) that\n\
has received an asyncrhonous EPROTO error."

int
test_case_2_16_5_2(int child)
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
	if (test_ioctl(child + 1, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_5_2 = { &preamble_6_2_4, &test_case_2_16_5_2, &postamble_6_2 };

#define test_case_2_16_5_2_stream_0 (&test_2_16_5_2)
#define test_case_2_16_5_2_stream_1 (NULL)
#define test_case_2_16_5_2_stream_2 (NULL)

#define tgrp_case_2_16_6_1 test_group_2
#define numb_case_2_16_6_1 "2.16.6.1"
#define name_case_2_16_6_1 "Perform streamio I_FDINSERT - EINVAL."
#define sref_case_2_16_6_1 sref_case_2_16
#define desc_case_2_16_6_1 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EINVAL\n\
is returned when I_FDINSERT is attempted on a Stream (fildes) that is\n\
linked under a Multiplexing Driver."

int
test_case_2_16_6_1(int child)
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
	fdi.fildes = test_fd[child + 1];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_6_1 = { &preamble_5, &test_case_2_16_6_1, &postamble_5 };

#define test_case_2_16_6_1_stream_0 (&test_2_16_6_1)
#define test_case_2_16_6_1_stream_1 (NULL)
#define test_case_2_16_6_1_stream_2 (NULL)

#define tgrp_case_2_16_6_2 test_group_2
#define numb_case_2_16_6_2 "2.16.6.2"
#define name_case_2_16_6_2 "Perform streamio I_FDINSERT."
#define sref_case_2_16_6_2 sref_case_2_16
#define desc_case_2_16_6_2 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that\n\
I_FDINSERT can be successful even if the Stream (fdi.fildes) is linked\n\
under a Multiplexing Driver."

int
test_case_2_16_6_2(int child)
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
	if (test_ioctl(child + 1, I_FDINSERT, (intptr_t) &fdi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_6_2 = { &preamble_5, &test_case_2_16_6_2, &postamble_5 };

#define test_case_2_16_6_2_stream_0 (&test_2_16_6_2)
#define test_case_2_16_6_2_stream_1 (NULL)
#define test_case_2_16_6_2_stream_2 (NULL)

#define tgrp_case_2_16_7 test_group_2
#define numb_case_2_16_7 "2.16.7"
#define name_case_2_16_7 "Perform streamio I_FDINSERT - EINVAL."
#define sref_case_2_16_7 sref_case_2_16
#define desc_case_2_16_7 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EINVAL\n\
is returned when fdi.offset is less than zero."

int
test_case_2_16_7(int child)
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
	fdi.fildes = 1;
	fdi.offset = -1;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_7 = { &preamble_0, &test_case_2_16_7, &postamble_0 };

#define test_case_2_16_7_stream_0 (&test_2_16_7)
#define test_case_2_16_7_stream_1 (NULL)
#define test_case_2_16_7_stream_2 (NULL)

#define tgrp_case_2_16_8 test_group_2
#define numb_case_2_16_8 "2.16.8"
#define name_case_2_16_8 "Perform streamio I_FDINSERT - EINVAL."
#define sref_case_2_16_8 sref_case_2_16
#define desc_case_2_16_8 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EINVAL\n\
is returned when fdi.ctlbuf.len is too small to hold the token."

int
test_case_2_16_8(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(t_uscalar_t) - 1;
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = 0;
	fdi.databuf.buf = NULL;
	fdi.flags = 0;
	fdi.fildes = 1;
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_8 = { &preamble_0, &test_case_2_16_8, &postamble_0 };

#define test_case_2_16_8_stream_0 (&test_2_16_8)
#define test_case_2_16_8_stream_1 (NULL)
#define test_case_2_16_8_stream_2 (NULL)

#define tgrp_case_2_16_9 test_group_2
#define numb_case_2_16_9 "2.16.9"
#define name_case_2_16_9 "Perform streamio I_FDINSERT - EINVAL."
#define sref_case_2_16_9 sref_case_2_16
#define desc_case_2_16_9 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EINVAL\n\
is returned when fdi.offset is not aligned to a t_uscalar_t boundary."

int
test_case_2_16_9(int child)
{
	char buf[sizeof(t_uscalar_t)<<1] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(buf);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = 0;
	fdi.databuf.buf = NULL;
	fdi.flags = 0;
	fdi.fildes = 1;
	fdi.offset = 3;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_9 = { &preamble_0, &test_case_2_16_9, &postamble_0 };

#define test_case_2_16_9_stream_0 (&test_2_16_9)
#define test_case_2_16_9_stream_1 (NULL)
#define test_case_2_16_9_stream_2 (NULL)

#define tgrp_case_2_16_10 test_group_2
#define numb_case_2_16_10 "2.16.10"
#define name_case_2_16_10 "Perform streamio I_FDINSERT - EINVAL."
#define sref_case_2_16_10 sref_case_2_16
#define desc_case_2_16_10 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EINVAL\n\
is returned when fdi.flags is other than zero or RS_HIPRI."

int
test_case_2_16_10(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = 0;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI + 1;
	fdi.fildes = 1;
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_10 = { &preamble_0, &test_case_2_16_10, &postamble_0 };

#define test_case_2_16_10_stream_0 (&test_2_16_10)
#define test_case_2_16_10_stream_1 (NULL)
#define test_case_2_16_10_stream_2 (NULL)

#define tgrp_case_2_16_11 test_group_2
#define numb_case_2_16_11 "2.16.11"
#define name_case_2_16_11 "Perform streamio I_FDINSERT - EINVAL."
#define sref_case_2_16_11 sref_case_2_16
#define desc_case_2_16_11 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EINVAL\n\
is returned when fdi.fildes is not a valid file descriptor."

int
test_case_2_16_11(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = 0;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = -1;
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_11 = { &preamble_0, &test_case_2_16_11, &postamble_0 };

#define test_case_2_16_11_stream_0 (&test_2_16_11)
#define test_case_2_16_11_stream_1 (NULL)
#define test_case_2_16_11_stream_2 (NULL)

#define tgrp_case_2_16_12 test_group_2
#define numb_case_2_16_12 "2.16.12"
#define name_case_2_16_12 "Perform streamio I_FDINSERT - EINVAL."
#define sref_case_2_16_12 sref_case_2_16
#define desc_case_2_16_12 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EINVAL\n\
is returned when fdi.fildes is not a Stream."

int
test_case_2_16_12(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = 0;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = 2;
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_12 = { &preamble_0, &test_case_2_16_12, &postamble_0 };

#define test_case_2_16_12_stream_0 (&test_2_16_12)
#define test_case_2_16_12_stream_1 (NULL)
#define test_case_2_16_12_stream_2 (NULL)

#define tgrp_case_2_16_13 test_group_2
#define numb_case_2_16_13 "2.16.13"
#define name_case_2_16_13 "Perform streamio I_FDINSERT - EFAULT."
#define sref_case_2_16_13 sref_case_2_16
#define desc_case_2_16_13 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EFAULT\n\
is returned when arg points outside the caller's valid address space."

int
test_case_2_16_13(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = 0;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_13 = { &preamble_0, &test_case_2_16_13, &postamble_0 };

#define test_case_2_16_13_stream_0 (&test_2_16_13)
#define test_case_2_16_13_stream_1 (NULL)
#define test_case_2_16_13_stream_2 (NULL)

#define tgrp_case_2_16_14 test_group_2
#define numb_case_2_16_14 "2.16.14"
#define name_case_2_16_14 "Perform streamio I_FDINSERT - EFAULT."
#define sref_case_2_16_14 sref_case_2_16
#define desc_case_2_16_14 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EFAULT\n\
when a buffer described by arg points outside the caller's valid address\n\
space."

int
test_case_2_16_14(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = 0;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = 0;
	fdi.databuf.len = sizeof(t_uscalar_t);
	fdi.databuf.buf = (char *) INVALID_ADDRESS;
	fdi.flags = RS_HIPRI;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_14 = { &preamble_0, &test_case_2_16_14, &postamble_0 };

#define test_case_2_16_14_stream_0 (&test_2_16_14)
#define test_case_2_16_14_stream_1 (NULL)
#define test_case_2_16_14_stream_2 (NULL)

#define tgrp_case_2_16_15 test_group_2
#define numb_case_2_16_15 "2.16.15"
#define name_case_2_16_15 "Perform streamio I_FDINSERT - EBADF."
#define sref_case_2_16_15 sref_case_2_16
#define desc_case_2_16_15 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that EBADF\n\
is returned when fildes is not a valid file descriptor."

int
test_case_2_16_15(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	int oldfd = test_fd[child];
	test_fd[child] = -1;
	fdi.ctlbuf.maxlen = -1;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = -1;
	fdi.databuf.len = -1;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_15 = { &preamble_0, &test_case_2_16_15, &postamble_0 };

#define test_case_2_16_15_stream_0 (&test_2_16_15)
#define test_case_2_16_15_stream_1 (NULL)
#define test_case_2_16_15_stream_2 (NULL)

#define tgrp_case_2_16_16 test_group_2
#define numb_case_2_16_16 "2.16.16"
#define name_case_2_16_16 "Perform streamio I_FDINSERT - EBADF."
#define sref_case_2_16_16 sref_case_2_16
#define desc_case_2_16_16 "\
Checks that I_FDINSERT can be performed on a Stream.  Check that EBADF\n\
is returned when fildes is a Stream that is not open for writing."

int
test_case_2_16_16(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = -1;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = -1;
	fdi.databuf.len = -1;
	fdi.databuf.buf = NULL;
	fdi.flags = RS_HIPRI;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_16 = { &preamble_0_1, &test_case_2_16_16, &postamble_0 };

#define test_case_2_16_16_stream_0 (&test_2_16_16)
#define test_case_2_16_16_stream_1 (NULL)
#define test_case_2_16_16_stream_2 (NULL)

#define tgrp_case_2_16_17 test_group_2
#define numb_case_2_16_17 "2.16.17"
#define name_case_2_16_17 "Perform streamio I_FDINSERT - EBADF."
#define sref_case_2_16_17 sref_case_2_16
#define desc_case_2_16_17 "\
Checks that I_FDINSERT can be performed on a Stream.  Checks that\n\
ioctl() I_FDINSERT will block awaiting flow control at the Stream head,\n\
and that the wait can be interrupted by a signal and ioctl() will return\n\
EINTR."

int
test_case_2_16_17(int child)
{
	char buf[sizeof(t_uscalar_t)] = { 0, };
	struct strfdinsert fdi;
	fdi.ctlbuf.maxlen = -1;
	fdi.ctlbuf.len = sizeof(t_uscalar_t);
	fdi.ctlbuf.buf = buf;
	fdi.databuf.maxlen = -1;
	fdi.databuf.len = -1;
	fdi.databuf.buf = NULL;
	fdi.flags = 0;
	fdi.fildes = test_fd[child];
	fdi.offset = 0;

	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_16_17 = { &preamble_8_0, &test_case_2_16_17, &postamble_8 };

#define test_case_2_16_17_stream_0 (&test_2_16_17)
#define test_case_2_16_17_stream_1 (NULL)
#define test_case_2_16_17_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_SENDFD
 */
static const char sref_case_2_17[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_SENDFD section.";

#define tgrp_case_2_17_1 test_group_2
#define numb_case_2_17_1 "2.17.1"
#define name_case_2_17_1 "Perform streamio I_SENDFD."
#define sref_case_2_17_1 sref_case_2_17
#define desc_case_2_17_1 "\
Checks that I_SENDFD can be performed on a Stream.  This test is\n\
performed using two FIFOs."

int
test_case_2_17_1(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) test_fd[child + 1]) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_17_1 = { &preamble_6, &test_case_2_17_1, &postamble_6 };

#define test_case_2_17_1_stream_0 (&test_2_17_1)
#define test_case_2_17_1_stream_1 (NULL)
#define test_case_2_17_1_stream_2 (NULL)

#define tgrp_case_2_17_2 test_group_2
#define numb_case_2_17_2 "2.17.2"
#define name_case_2_17_2 "Perform streamio I_SENDFD - ENXIO."
#define sref_case_2_17_2 sref_case_2_17
#define desc_case_2_17_2 "\
Checks that I_SENDFD can be performed on a Stream.  Checks that ENXIO is\n\
returned when the Stream is hung up."

int
test_case_2_17_2(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) test_fd[child + 1]) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_17_2 = { &preamble_6_2_1, &test_case_2_17_2, &postamble_6_2 };

#define test_case_2_17_2_stream_0 (&test_2_17_2)
#define test_case_2_17_2_stream_1 (NULL)
#define test_case_2_17_2_stream_2 (NULL)

#define tgrp_case_2_17_3 test_group_2
#define numb_case_2_17_3 "2.17.3"
#define name_case_2_17_3 "Perform streamio I_SENDFD - EPROTO RDERR."
#define sref_case_2_17_3 sref_case_2_17
#define desc_case_2_17_3 "\
Checks that I_SENDFD can be performed on a Stream.  Checks that EPROTO\n\
is returned when the other Stream head is read errored."

int
test_case_2_17_3(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) test_fd[child + 1]) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_17_3 = { &preamble_6_2_2, &test_case_2_17_3, &postamble_6_2 };

#define test_case_2_17_3_stream_0 (&test_2_17_3)
#define test_case_2_17_3_stream_1 (NULL)
#define test_case_2_17_3_stream_2 (NULL)

#define tgrp_case_2_17_4 test_group_2
#define numb_case_2_17_4 "2.17.4"
#define name_case_2_17_4 "Perform streamio I_SENDFD - EPROTO WRERR."
#define sref_case_2_17_4 sref_case_2_17
#define desc_case_2_17_4 "\
Checks that I_SENDFD can be performed on a Stream.  Checks that EPROTO\n\
is returned when the Stream is write errored."

int
test_case_2_17_4(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) test_fd[child + 1]) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_17_4 = { &preamble_6_2_3, &test_case_2_17_4, &postamble_6_2 };

#define test_case_2_17_4_stream_0 (&test_2_17_4)
#define test_case_2_17_4_stream_1 (NULL)
#define test_case_2_17_4_stream_2 (NULL)

#define tgrp_case_2_17_5 test_group_2
#define numb_case_2_17_5 "2.17.5"
#define name_case_2_17_5 "Perform streamio I_SENDFD - EPROTO RDERR WRERR."
#define sref_case_2_17_5 sref_case_2_17
#define desc_case_2_17_5 "\
Checks that I_SENDFD can be performed on a Stream.  Checks that EPROTO\n\
is returned when the Stream is errored."

int
test_case_2_17_5(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) test_fd[child + 1]) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_17_5 = { &preamble_6_2_4, &test_case_2_17_5, &postamble_6_2 };

#define test_case_2_17_5_stream_0 (&test_2_17_5)
#define test_case_2_17_5_stream_1 (NULL)
#define test_case_2_17_5_stream_2 (NULL)

#define tgrp_case_2_17_6 test_group_2
#define numb_case_2_17_6 "2.17.6"
#define name_case_2_17_6 "Perform streamio I_SENDFD - EINVAL."
#define sref_case_2_17_6 sref_case_2_17
#define desc_case_2_17_6 "\
Checks that I_SENDFD can be performed on a Stream.  Checks that EINVAL\n\
is returned when the Stream is linked under a Multiplexing Driver."

int
test_case_2_17_6(int child)
{
	if (test_ioctl(child, I_SENDFD, (intptr_t) test_fd[child + 1]) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_17_6 = { &preamble_7, &test_case_2_17_6, &postamble_7 };

#define test_case_2_17_6_stream_0 (&test_2_17_6)
#define test_case_2_17_6_stream_1 (NULL)
#define test_case_2_17_6_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_E_RECVFD
 */
static const char sref_case_2_18[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_RECVFD section.";

#define tgrp_case_2_18 test_group_2
#define numb_case_2_18 "2.18"
#define name_case_2_18 "Perform streamio I_E_RECVFD."
#define sref_case_2_18 sref_case_2_18
#define desc_case_2_18 "\
Checks that I_E_RECVFD can be performed on a Stream."

int
test_case_2_18(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_18 = { &preamble_0, &test_case_2_18, &postamble_0 };

#define test_case_2_18_stream_0 (&test_2_18)
#define test_case_2_18_stream_1 (NULL)
#define test_case_2_18_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_SWROPT
 */
static const char sref_case_2_19[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_SWROPT section.";

#define tgrp_case_2_19_1 test_group_2
#define numb_case_2_19_1 "2.19.1"
#define name_case_2_19_1 "Perform streamio I_SWROPT - default."
#define sref_case_2_19_1 sref_case_2_19
#define desc_case_2_19_1 "\
Checks that I_SWROPT can be performed on a Stream."

int
test_case_2_19_1(int child)
{
	if (test_ioctl(child, I_SWROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_1 = { &preamble_0, &test_case_2_19_1, &postamble_0 };

#define test_case_2_19_1_stream_0 (&test_2_19_1)
#define test_case_2_19_1_stream_1 (NULL)
#define test_case_2_19_1_stream_2 (NULL)

#define tgrp_case_2_19_2 test_group_2
#define numb_case_2_19_2 "2.19.2"
#define name_case_2_19_2 "Perform streamio I_SWROPT - SNDZERO."
#define sref_case_2_19_2 sref_case_2_19
#define desc_case_2_19_2 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO)."

int
test_case_2_19_2(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_2 = { &preamble_0, &test_case_2_19_2, &postamble_0 };

#define test_case_2_19_2_stream_0 (&test_2_19_2)
#define test_case_2_19_2_stream_1 (NULL)
#define test_case_2_19_2_stream_2 (NULL)

#define tgrp_case_2_19_3 test_group_2
#define numb_case_2_19_3 "2.19.3"
#define name_case_2_19_3 "Perform streamio I_SWROPT - SNDPIPE."
#define sref_case_2_19_3 sref_case_2_19
#define desc_case_2_19_3 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDPIPE)."

int
test_case_2_19_3(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDPIPE)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_3 = { &preamble_0, &test_case_2_19_3, &postamble_0 };

#define test_case_2_19_3_stream_0 (&test_2_19_3)
#define test_case_2_19_3_stream_1 (NULL)
#define test_case_2_19_3_stream_2 (NULL)

#define tgrp_case_2_19_4 test_group_2
#define numb_case_2_19_4 "2.19.4"
#define name_case_2_19_4 "Perform streamio I_SWROPT - SNDHOLD."
#define sref_case_2_19_4 sref_case_2_19
#define desc_case_2_19_4 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDHOLD)."

int
test_case_2_19_4(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SWROPT, (SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_19_4 = { &preamble_0, &test_case_2_19_4, &postamble_0 };

#define test_case_2_19_4_stream_0 (&test_2_19_4)
#define test_case_2_19_4_stream_1 (NULL)
#define test_case_2_19_4_stream_2 (NULL)

#define tgrp_case_2_19_5 test_group_2
#define numb_case_2_19_5 "2.19.5"
#define name_case_2_19_5 "Perform streamio I_SWROPT - SNDZERO | SNDPIPE."
#define sref_case_2_19_5 sref_case_2_19
#define desc_case_2_19_5 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO | SNDPIPE)."

int
test_case_2_19_5(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDPIPE)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_5 = { &preamble_0, &test_case_2_19_5, &postamble_0 };

#define test_case_2_19_5_stream_0 (&test_2_19_5)
#define test_case_2_19_5_stream_1 (NULL)
#define test_case_2_19_5_stream_2 (NULL)

#define tgrp_case_2_19_6 test_group_2
#define numb_case_2_19_6 "2.19.6"
#define name_case_2_19_6 "Perform streamio I_SWROPT - SNDZERO | SNDHOLD."
#define sref_case_2_19_6 sref_case_2_19
#define desc_case_2_19_6 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO | SNDHOLD)."

int
test_case_2_19_6(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_19_6 = { &preamble_0, &test_case_2_19_6, &postamble_0 };

#define test_case_2_19_6_stream_0 (&test_2_19_6)
#define test_case_2_19_6_stream_1 (NULL)
#define test_case_2_19_6_stream_2 (NULL)

#define tgrp_case_2_19_7 test_group_2
#define numb_case_2_19_7 "2.19.7"
#define name_case_2_19_7 "Perform streamio I_SWROPT - SNDPIPE | SNDHOLD."
#define sref_case_2_19_7 sref_case_2_19
#define desc_case_2_19_7 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDPIPE | SNDHOLD)."

int
test_case_2_19_7(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SWROPT, (SNDPIPE | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_19_7 = { &preamble_0, &test_case_2_19_7, &postamble_0 };

#define test_case_2_19_7_stream_0 (&test_2_19_7)
#define test_case_2_19_7_stream_1 (NULL)
#define test_case_2_19_7_stream_2 (NULL)

#define tgrp_case_2_19_8 test_group_2
#define numb_case_2_19_8 "2.19.8"
#define name_case_2_19_8 "Perform streamio I_SWROPT - SNDZERO | SNDPIPE | SNDHOLD."
#define sref_case_2_19_8 sref_case_2_19
#define desc_case_2_19_8 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO | SNDPIPE | SNDHOLD)."

int
test_case_2_19_8(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDPIPE | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_19_8 = { &preamble_0, &test_case_2_19_8, &postamble_0 };

#define test_case_2_19_8_stream_0 (&test_2_19_8)
#define test_case_2_19_8_stream_1 (NULL)
#define test_case_2_19_8_stream_2 (NULL)

#define tgrp_case_2_19_9 test_group_2
#define numb_case_2_19_9 "2.19.9"
#define name_case_2_19_9 "Perform streamio I_SWROPT - EINVAL."
#define sref_case_2_19_9 sref_case_2_19
#define desc_case_2_19_9 "\
Checks that I_SWROPT can be performed on a Stream with an invalid argument\n\
value, resulting in the return of EINVAL."

int
test_case_2_19_9(int child)
{
	if (test_ioctl(child, I_SWROPT, -1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_9 = { &preamble_0, &test_case_2_19_9, &postamble_0 };

#define test_case_2_19_9_stream_0 (&test_2_19_9)
#define test_case_2_19_9_stream_1 (NULL)
#define test_case_2_19_9_stream_2 (NULL)

#define tgrp_case_2_19_10 test_group_2
#define numb_case_2_19_10 "2.19.10"
#define name_case_2_19_10 "Perform streamio I_SWROPT - ENXIO."
#define sref_case_2_19_10 sref_case_2_19
#define desc_case_2_19_10 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO).  Check that ENXIO is return when the Stream is hung up."

int
test_case_2_19_10(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO)) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_10 = { &preamble_2_1, &test_case_2_19_10, &postamble_2 };

#define test_case_2_19_10_stream_0 (&test_2_19_10)
#define test_case_2_19_10_stream_1 (NULL)
#define test_case_2_19_10_stream_2 (NULL)

#define tgrp_case_2_19_11 test_group_2
#define numb_case_2_19_11 "2.19.11"
#define name_case_2_19_11 "Perform streamio I_SWROPT - SNDZERO."
#define sref_case_2_19_11 sref_case_2_19
#define desc_case_2_19_11 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO), despite the Stream being read errored."

int
test_case_2_19_11(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_11 = { &preamble_2_2, &test_case_2_19_11, &postamble_2 };

#define test_case_2_19_11_stream_0 (&test_2_19_11)
#define test_case_2_19_11_stream_1 (NULL)
#define test_case_2_19_11_stream_2 (NULL)

#define tgrp_case_2_19_12 test_group_2
#define numb_case_2_19_12 "2.19.12"
#define name_case_2_19_12 "Perform streamio I_SWROPT - EPROTO."
#define sref_case_2_19_12 sref_case_2_19
#define desc_case_2_19_12 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO).  Check that EPROTO is return when the Stream is write errored."

int
test_case_2_19_12(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO)) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_12 = { &preamble_2_3, &test_case_2_19_12, &postamble_2 };

#define test_case_2_19_12_stream_0 (&test_2_19_12)
#define test_case_2_19_12_stream_1 (NULL)
#define test_case_2_19_12_stream_2 (NULL)

#define tgrp_case_2_19_13 test_group_2
#define numb_case_2_19_13 "2.19.13"
#define name_case_2_19_13 "Perform streamio I_SWROPT - EPROTO."
#define sref_case_2_19_13 sref_case_2_19
#define desc_case_2_19_13 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO).  Check that EPROTO is return when the Stream is errored."

int
test_case_2_19_13(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO)) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_13 = { &preamble_2_4, &test_case_2_19_13, &postamble_2 };

#define test_case_2_19_13_stream_0 (&test_2_19_13)
#define test_case_2_19_13_stream_1 (NULL)
#define test_case_2_19_13_stream_2 (NULL)

#define tgrp_case_2_19_14 test_group_2
#define numb_case_2_19_14 "2.19.14"
#define name_case_2_19_14 "Perform streamio I_SWROPT - EINVAL."
#define sref_case_2_19_14 sref_case_2_19
#define desc_case_2_19_14 "\
Checks that I_SWROPT can be performed on a Stream with write option values\n\
(SNDZERO).  Check that EINVAL is return when the Stream is linked under\n\
a Multiiplexing Driver."

int
test_case_2_19_14(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO)) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_14 = { &preamble_5, &test_case_2_19_14, &postamble_5 };

#define test_case_2_19_14_stream_0 (&test_2_19_14)
#define test_case_2_19_14_stream_1 (NULL)
#define test_case_2_19_14_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GWROPT
 */
static const char sref_case_2_20[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_GWROPT section.";

#define tgrp_case_2_20_1 test_group_2
#define numb_case_2_20_1 "2.20.1"
#define name_case_2_20_1 "Perform streamio I_GWROPT - default."
#define sref_case_2_20_1 sref_case_2_20
#define desc_case_2_20_1 "\
Checks that I_GWROPT can be performed on a Stream to read the Stream default\n\
options (SNDZERO for a regular Stream)."

int
test_case_2_20_1(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_1 = { &preamble_0, &test_case_2_20_1, &postamble_0 };

#define test_case_2_20_1_stream_0 (&test_2_20_1)
#define test_case_2_20_1_stream_1 (NULL)
#define test_case_2_20_1_stream_2 (NULL)

#define tgrp_case_2_20_2 test_group_2
#define numb_case_2_20_2 "2.20.2"
#define name_case_2_20_2 "Perform streamio I_GWROPT - default."
#define sref_case_2_20_2 sref_case_2_20
#define desc_case_2_20_2 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(0) after they have been set with I_SWROPT."

int
test_case_2_20_2(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_2 = { &preamble_0, &test_case_2_20_2, &postamble_0 };

#define test_case_2_20_2_stream_0 (&test_2_20_2)
#define test_case_2_20_2_stream_1 (NULL)
#define test_case_2_20_2_stream_2 (NULL)

#define tgrp_case_2_20_3 test_group_2
#define numb_case_2_20_3 "2.20.3"
#define name_case_2_20_3 "Perform streamio I_GWROPT - SNDZERO."
#define sref_case_2_20_3 sref_case_2_20
#define desc_case_2_20_3 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(SNDZERO) after they have been set with I_SWROPT."

int
test_case_2_20_3(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, (SNDZERO)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_3 = { &preamble_0, &test_case_2_20_3, &postamble_0 };

#define test_case_2_20_3_stream_0 (&test_2_20_3)
#define test_case_2_20_3_stream_1 (NULL)
#define test_case_2_20_3_stream_2 (NULL)

#define tgrp_case_2_20_4 test_group_2
#define numb_case_2_20_4 "2.20.4"
#define name_case_2_20_4 "Perform streamio I_GWROPT - SNDPIPE."
#define sref_case_2_20_4 sref_case_2_20
#define desc_case_2_20_4 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(SNDPIPE) after they have been set with I_SWROPT."

int
test_case_2_20_4(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, (SNDPIPE)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDPIPE))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_4 = { &preamble_0, &test_case_2_20_4, &postamble_0 };

#define test_case_2_20_4_stream_0 (&test_2_20_4)
#define test_case_2_20_4_stream_1 (NULL)
#define test_case_2_20_4_stream_2 (NULL)

#define tgrp_case_2_20_5 test_group_2
#define numb_case_2_20_5 "2.20.5"
#define name_case_2_20_5 "Perform streamio I_GWROPT - SNDHOLD."
#define sref_case_2_20_5 sref_case_2_20
#define desc_case_2_20_5 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_5(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, (SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDHOLD))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_20_5 = { &preamble_0, &test_case_2_20_5, &postamble_0 };

#define test_case_2_20_5_stream_0 (&test_2_20_5)
#define test_case_2_20_5_stream_1 (NULL)
#define test_case_2_20_5_stream_2 (NULL)

#define tgrp_case_2_20_6 test_group_2
#define numb_case_2_20_6 "2.20.6"
#define name_case_2_20_6 "Perform streamio I_GWROPT - SNDZERO | SNDPIPE."
#define sref_case_2_20_6 sref_case_2_20
#define desc_case_2_20_6 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(SNDZERO | SNDPIPE) after they have been set with I_SWROPT."

int
test_case_2_20_6(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDPIPE)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO | SNDPIPE))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_6 = { &preamble_0, &test_case_2_20_6, &postamble_0 };

#define test_case_2_20_6_stream_0 (&test_2_20_6)
#define test_case_2_20_6_stream_1 (NULL)
#define test_case_2_20_6_stream_2 (NULL)

#define tgrp_case_2_20_7 test_group_2
#define numb_case_2_20_7 "2.20.7"
#define name_case_2_20_7 "Perform streamio I_GWROPT - SNDZERO | SNDHOLD."
#define sref_case_2_20_7 sref_case_2_20
#define desc_case_2_20_7 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(SNDZERO | SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_7(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO | SNDHOLD))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_20_7 = { &preamble_0, &test_case_2_20_7, &postamble_0 };

#define test_case_2_20_7_stream_0 (&test_2_20_7)
#define test_case_2_20_7_stream_1 (NULL)
#define test_case_2_20_7_stream_2 (NULL)

#define tgrp_case_2_20_8 test_group_2
#define numb_case_2_20_8 "2.20.8"
#define name_case_2_20_8 "Perform streamio I_GWROPT - SNDPIPE | SNDHOLD."
#define sref_case_2_20_8 sref_case_2_20
#define desc_case_2_20_8 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(SNDPIPE | SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_8(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, (SNDPIPE | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDPIPE | SNDHOLD))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_20_8 = { &preamble_0, &test_case_2_20_8, &postamble_0 };

#define test_case_2_20_8_stream_0 (&test_2_20_8)
#define test_case_2_20_8_stream_1 (NULL)
#define test_case_2_20_8_stream_2 (NULL)

#define tgrp_case_2_20_9 test_group_2
#define numb_case_2_20_9 "2.20.9"
#define name_case_2_20_9 "Perform streamio I_GWROPT - SNDZERO | SNDPIPE | SNDHOLD."
#define sref_case_2_20_9 sref_case_2_20
#define desc_case_2_20_9 "\
Checks that I_GWROPT can be performed on a Stream to read the write options\n\
(SNDZERO | SNDPIPE | SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_9(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int wropts = -1;

	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDPIPE | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO | SNDPIPE | SNDHOLD))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_20_9 = { &preamble_0, &test_case_2_20_9, &postamble_0 };

#define test_case_2_20_9_stream_0 (&test_2_20_9)
#define test_case_2_20_9_stream_1 (NULL)
#define test_case_2_20_9_stream_2 (NULL)

#define tgrp_case_2_20_10 test_group_2
#define numb_case_2_20_10 "2.20.10"
#define name_case_2_20_10 "Perform streamio I_GWROPT - EFAULT."
#define sref_case_2_20_10 sref_case_2_20
#define desc_case_2_20_10 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_20_10(int child)
{
	if (test_ioctl(child, I_GWROPT, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_10 = { &preamble_0, &test_case_2_20_10, &postamble_0 };

#define test_case_2_20_10_stream_0 (&test_2_20_10)
#define test_case_2_20_10_stream_1 (NULL)
#define test_case_2_20_10_stream_2 (NULL)

#define tgrp_case_2_20_11 test_group_2
#define numb_case_2_20_11 "2.20.11"
#define name_case_2_20_11 "Perform streamio I_GWROPT - default."
#define sref_case_2_20_11 sref_case_2_20
#define desc_case_2_20_11 "\
Checks that I_GWROPT can be performed on a Stream to read the Stream default\n\
options (SNDZERO for a regular Stream), despite the Stream being hung up."

int
test_case_2_20_11(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_11 = { &preamble_2_1, &test_case_2_20_11, &postamble_2 };

#define test_case_2_20_11_stream_0 (&test_2_20_11)
#define test_case_2_20_11_stream_1 (NULL)
#define test_case_2_20_11_stream_2 (NULL)

#define tgrp_case_2_20_12 test_group_2
#define numb_case_2_20_12 "2.20.12"
#define name_case_2_20_12 "Perform streamio I_GWROPT - default."
#define sref_case_2_20_12 sref_case_2_20
#define desc_case_2_20_12 "\
Checks that I_GWROPT can be performed on a Stream to read the Stream default\n\
options (SNDZERO for a regular Stream), despite the Stream being read errored."

int
test_case_2_20_12(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_12 = { &preamble_2_2, &test_case_2_20_12, &postamble_2 };

#define test_case_2_20_12_stream_0 (&test_2_20_12)
#define test_case_2_20_12_stream_1 (NULL)
#define test_case_2_20_12_stream_2 (NULL)

#define tgrp_case_2_20_13 test_group_2
#define numb_case_2_20_13 "2.20.13"
#define name_case_2_20_13 "Perform streamio I_GWROPT - default."
#define sref_case_2_20_13 sref_case_2_20
#define desc_case_2_20_13 "\
Checks that I_GWROPT can be performed on a Stream to read the Stream default\n\
options (SNDZERO for a regular Stream), despite the Stream being write errored."

int
test_case_2_20_13(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_13 = { &preamble_2_3, &test_case_2_20_13, &postamble_2 };

#define test_case_2_20_13_stream_0 (&test_2_20_13)
#define test_case_2_20_13_stream_1 (NULL)
#define test_case_2_20_13_stream_2 (NULL)

#define tgrp_case_2_20_14 test_group_2
#define numb_case_2_20_14 "2.20.14"
#define name_case_2_20_14 "Perform streamio I_GWROPT - default."
#define sref_case_2_20_14 sref_case_2_20
#define desc_case_2_20_14 "\
Checks that I_GWROPT can be performed on a Stream to read the Stream default\n\
options (SNDZERO for a regular Stream), despite the Stream being errored."

int
test_case_2_20_14(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != (SNDZERO))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_14 = { &preamble_2_4, &test_case_2_20_14, &postamble_2 };

#define test_case_2_20_14_stream_0 (&test_2_20_14)
#define test_case_2_20_14_stream_1 (NULL)
#define test_case_2_20_14_stream_2 (NULL)

#define tgrp_case_2_20_15 test_group_2
#define numb_case_2_20_15 "2.20.15"
#define name_case_2_20_15 "Perform streamio I_GWROPT - EINVAL."
#define sref_case_2_20_15 sref_case_2_20
#define desc_case_2_20_15 "\
Checks that I_GWROPT can be performed on a Stream to read the Stream default\n\
options (SNDZERO for a regular Stream).  Checks that EINVAL is returned\n\
when the Stream is linked under a Multiplexing Driver."

int
test_case_2_20_15(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_15 = { &preamble_5, &test_case_2_20_15, &postamble_5 };

#define test_case_2_20_15_stream_0 (&test_2_20_15)
#define test_case_2_20_15_stream_1 (NULL)
#define test_case_2_20_15_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_LIST
 */
static const char sref_case_2_21[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_LIST section.";

#define tgrp_case_2_21_1 test_group_2
#define numb_case_2_21_1 "2.21.1"
#define name_case_2_21_1 "Perform streamio I_LIST."
#define sref_case_2_21_1 sref_case_2_21
#define desc_case_2_21_1 "\
Checks that I_LIST can be performed on a Stream."

int
test_case_2_21_1(int child)
{
	if (test_ioctl(child, I_LIST, 0) != __RESULT_SUCCESS || (last_retval != 0 && last_retval != 1))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_21_1 = { &preamble_0, &test_case_2_21_1, &postamble_0 };

#define test_case_2_21_1_stream_0 (&test_2_21_1)
#define test_case_2_21_1_stream_1 (NULL)
#define test_case_2_21_1_stream_2 (NULL)

#define tgrp_case_2_21_2 test_group_2
#define numb_case_2_21_2 "2.21.2"
#define name_case_2_21_2 "Perform streamio I_LIST."
#define sref_case_2_21_2 sref_case_2_21
#define desc_case_2_21_2 "\
Checks that I_LIST can be performed on a Stream, despite the Stream\n\
being hung up."

int
test_case_2_21_2(int child)
{
	if (test_ioctl(child, I_LIST, 0) != __RESULT_SUCCESS || last_retval != 2)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_21_2 = { &preamble_2_1, &test_case_2_21_2, &postamble_2 };

#define test_case_2_21_2_stream_0 (&test_2_21_2)
#define test_case_2_21_2_stream_1 (NULL)
#define test_case_2_21_2_stream_2 (NULL)

#define tgrp_case_2_21_3 test_group_2
#define numb_case_2_21_3 "2.21.3"
#define name_case_2_21_3 "Perform streamio I_LIST."
#define sref_case_2_21_3 sref_case_2_21
#define desc_case_2_21_3 "\
Checks that I_LIST can be performed on a Stream, despite the Stream\n\
being read errored."

int
test_case_2_21_3(int child)
{
	if (test_ioctl(child, I_LIST, 0) != __RESULT_SUCCESS || last_retval != 2)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_21_3 = { &preamble_2_2, &test_case_2_21_3, &postamble_2 };

#define test_case_2_21_3_stream_0 (&test_2_21_3)
#define test_case_2_21_3_stream_1 (NULL)
#define test_case_2_21_3_stream_2 (NULL)

#define tgrp_case_2_21_4 test_group_2
#define numb_case_2_21_4 "2.21.4"
#define name_case_2_21_4 "Perform streamio I_LIST."
#define sref_case_2_21_4 sref_case_2_21
#define desc_case_2_21_4 "\
Checks that I_LIST can be performed on a Stream, despite the Stream\n\
being write errored."

int
test_case_2_21_4(int child)
{
	if (test_ioctl(child, I_LIST, 0) != __RESULT_SUCCESS || last_retval != 2)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_21_4 = { &preamble_2_3, &test_case_2_21_4, &postamble_2 };

#define test_case_2_21_4_stream_0 (&test_2_21_4)
#define test_case_2_21_4_stream_1 (NULL)
#define test_case_2_21_4_stream_2 (NULL)

#define tgrp_case_2_21_5 test_group_2
#define numb_case_2_21_5 "2.21.5"
#define name_case_2_21_5 "Perform streamio I_LIST."
#define sref_case_2_21_5 sref_case_2_21
#define desc_case_2_21_5 "\
Checks that I_LIST can be performed on a Stream, despite the Stream\n\
being errored."

int
test_case_2_21_5(int child)
{
	if (test_ioctl(child, I_LIST, 0) != __RESULT_SUCCESS || last_retval != 2)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_21_5 = { &preamble_2_4, &test_case_2_21_5, &postamble_2 };

#define test_case_2_21_5_stream_0 (&test_2_21_5)
#define test_case_2_21_5_stream_1 (NULL)
#define test_case_2_21_5_stream_2 (NULL)

#define tgrp_case_2_21_6 test_group_2
#define numb_case_2_21_6 "2.21.6"
#define name_case_2_21_6 "Perform streamio I_LIST - EINVAL."
#define sref_case_2_21_6 sref_case_2_21
#define desc_case_2_21_6 "\
Checks that I_LIST can be performed on a Stream.  Checks that EINVAL is\n\
returned when the Stream is lined under a Multiplexing Driver."

int
test_case_2_21_6(int child)
{
	if (test_ioctl(child, I_LIST, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_21_6 = { &preamble_5, &test_case_2_21_6, &postamble_5 };

#define test_case_2_21_6_stream_0 (&test_2_21_6)
#define test_case_2_21_6_stream_1 (NULL)
#define test_case_2_21_6_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_PLINK
 */
static const char sref_case_2_22[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_PLINK section.";

#define tgrp_case_2_22_1 test_group_2
#define numb_case_2_22_1 "2.22.1"
#define name_case_2_22_1 "Perform streamio I_PLINK - EINVAL."
#define sref_case_2_22_1 sref_case_2_22
#define desc_case_2_22_1 "\
Checks that I_PLINK can be performed on a Stream.\n\
File descriptor does not support multiplexing."

int
test_case_2_22_1(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) 10) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_1 = { &preamble_0, &test_case_2_22_1, &postamble_0 };

#define test_case_2_22_1_stream_0 (&test_2_22_1)
#define test_case_2_22_1_stream_1 (NULL)
#define test_case_2_22_1_stream_2 (NULL)

#define tgrp_case_2_22_2 test_group_2
#define numb_case_2_22_2 "2.22.2"
#define name_case_2_22_2 "Perform streamio I_PLINK - EINVAL."
#define sref_case_2_22_2 sref_case_2_22
#define desc_case_2_22_2 "\
Checks that I_PLINK can be performed on a Stream.\n\
File descriptor argument is not a Stream."

int
test_case_2_22_2(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) 1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_2 = { &preamble_1, &test_case_2_22_2, &postamble_1 };

#define test_case_2_22_2_stream_0 (&test_2_22_2)
#define test_case_2_22_2_stream_1 (NULL)
#define test_case_2_22_2_stream_2 (NULL)

#define tgrp_case_2_22_3 test_group_2
#define numb_case_2_22_3 "2.22.3"
#define name_case_2_22_3 "Perform streamio I_PLINK - EBADF."
#define sref_case_2_22_3 sref_case_2_22
#define desc_case_2_22_3 "\
Checks that I_PLINK can be performed on a Stream.\n\
File descriptor argument is not a valid, open file descriptor."

int
test_case_2_22_3(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_3 = { &preamble_1, &test_case_2_22_3, &postamble_1 };

#define test_case_2_22_3_stream_0 (&test_2_22_3)
#define test_case_2_22_3_stream_1 (NULL)
#define test_case_2_22_3_stream_2 (NULL)

#define tgrp_case_2_22_4 test_group_2
#define numb_case_2_22_4 "2.22.4"
#define name_case_2_22_4 "Perform streamio I_PLINK - ENXIO."
#define sref_case_2_22_4 sref_case_2_22
#define desc_case_2_22_4 "\
Checks that I_PLINK can be performed on a Stream.\n\
Hangup received on Stream."

int
test_case_2_22_4(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_4 = { &preamble_4_2_1, &test_case_2_22_4, &postamble_4_2 };

#define test_case_2_22_4_stream_0 (&test_2_22_4)
#define test_case_2_22_4_stream_1 (NULL)
#define test_case_2_22_4_stream_2 (NULL)

#define tgrp_case_2_22_5 test_group_2
#define numb_case_2_22_5 "2.22.5"
#define name_case_2_22_5 "Perform streamio I_PLINK - EPROTO."
#define sref_case_2_22_5 sref_case_2_22
#define desc_case_2_22_5 "\
Checks that I_PLINK can be performed on a Stream.\n\
Read error received on Stream."

int
test_case_2_22_5(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_5 = { &preamble_4_2_2, &test_case_2_22_5, &postamble_4_2 };

#define test_case_2_22_5_stream_0 (&test_2_22_5)
#define test_case_2_22_5_stream_1 (NULL)
#define test_case_2_22_5_stream_2 (NULL)

#define tgrp_case_2_22_6 test_group_2
#define numb_case_2_22_6 "2.22.6"
#define name_case_2_22_6 "Perform streamio I_PLINK - EPROTO."
#define sref_case_2_22_6 sref_case_2_22
#define desc_case_2_22_6 "\
Checks that I_PLINK can be performed on a Stream.\n\
Write error received on Stream."

int
test_case_2_22_6(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_6 = { &preamble_4_2_3, &test_case_2_22_6, &postamble_4_2 };

#define test_case_2_22_6_stream_0 (&test_2_22_6)
#define test_case_2_22_6_stream_1 (NULL)
#define test_case_2_22_6_stream_2 (NULL)

#define tgrp_case_2_22_7 test_group_2
#define numb_case_2_22_7 "2.22.7"
#define name_case_2_22_7 "Perform streamio I_PLINK - EPROTO."
#define sref_case_2_22_7 sref_case_2_22
#define desc_case_2_22_7 "\
Checks that I_PLINK can be performed on a Stream.\n\
Error received on Stream."

int
test_case_2_22_7(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) -1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_7 = { &preamble_4_2_4, &test_case_2_22_7, &postamble_4_2 };

#define test_case_2_22_7_stream_0 (&test_2_22_7)
#define test_case_2_22_7_stream_1 (NULL)
#define test_case_2_22_7_stream_2 (NULL)

#define tgrp_case_2_22_8 test_group_2
#define numb_case_2_22_8 "2.22.8"
#define name_case_2_22_8 "Perform streamio I_PLINK - EINVAL."
#define sref_case_2_22_8 sref_case_2_22
#define desc_case_2_22_8 "\
Checks that I_PLINK can be performed on a Stream.  Check that EINVAL is\n\
returned if the Stream is already linked under a Multiplexing Driver."

int
test_case_2_22_8(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) 1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_8 = { &preamble_5, &test_case_2_22_8, &postamble_5 };

#define test_case_2_22_8_stream_0 (&test_2_22_8)
#define test_case_2_22_8_stream_1 (NULL)
#define test_case_2_22_8_stream_2 (NULL)

#define tgrp_case_2_22_9 test_group_2
#define numb_case_2_22_9 "2.22.9"
#define name_case_2_22_9 "Perform streamio I_PLINK."
#define sref_case_2_22_9 sref_case_2_22
#define desc_case_2_22_9 "\
Checks that I_PLINK can be performed on a Stream."

int
test_case_2_22_9(int child)
{
	if (test_ioctl(child, I_PLINK, (intptr_t) test_fd[child + 1]) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_PUNLINK, (intptr_t) last_retval) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_22_9 = { &preamble_4, &test_case_2_22_9, &postamble_4 };

#define test_case_2_22_9_stream_0 (&test_2_22_9)
#define test_case_2_22_9_stream_1 (NULL)
#define test_case_2_22_9_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_PUNLINK
 */
static const char sref_case_2_23[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_PUNLINK section.";

#define tgrp_case_2_23_1 test_group_2
#define numb_case_2_23_1 "2.23.1"
#define name_case_2_23_1 "Perform streamio I_PUNLINK - EINVAL."
#define sref_case_2_23_1 sref_case_2_23
#define desc_case_2_23_1 "\
Checks that I_PUNLINK can be performed on a Stream.  This test uses \n\
an invalid multiplexer identifier and should return EINVAL."

int
test_case_2_23_1(int child)
{
	if (test_ioctl(child, I_PUNLINK, (intptr_t) 5) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_23_1 = { &preamble_0, &test_case_2_23_1, &postamble_0 };

#define test_case_2_23_1_stream_0 (&test_2_23_1)
#define test_case_2_23_1_stream_1 (NULL)
#define test_case_2_23_1_stream_2 (NULL)

#define tgrp_case_2_23_2 test_group_2
#define numb_case_2_23_2 "2.23.2"
#define name_case_2_23_2 "Perform streamio I_PUNLINK."
#define sref_case_2_23_2 sref_case_2_23
#define desc_case_2_23_2 "\
Checks that I_PUNLINK can be performed on a Stream.  This tests uses \n\
MUXID_ALL, but there are no Streams linked.  It should succeed."

int
test_case_2_23_2(int child)
{
	if (test_ioctl(child, I_PUNLINK, (intptr_t) MUXID_ALL) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_23_2 = { &preamble_0, &test_case_2_23_2, &postamble_0 };

#define test_case_2_23_2_stream_0 (&test_2_23_2)
#define test_case_2_23_2_stream_1 (NULL)
#define test_case_2_23_2_stream_2 (NULL)

#define tgrp_case_2_23_3 test_group_2
#define numb_case_2_23_3 "2.23.3"
#define name_case_2_23_3 "Perform streamio I_PUNLINK - ENXIO."
#define sref_case_2_23_3 sref_case_2_23
#define desc_case_2_23_3 "\
Checks that I_PUNLINK can be performed on a hung up Stream.\n\
This test should return ENXIO."

int
test_case_2_23_3(int child)
{
	if (test_ioctl(child, I_PUNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_23_3 = { &preamble_4_2_1, &test_case_2_23_3, &postamble_4_2 };

#define test_case_2_23_3_stream_0 (&test_2_23_3)
#define test_case_2_23_3_stream_1 (NULL)
#define test_case_2_23_3_stream_2 (NULL)

#define tgrp_case_2_23_4 test_group_2
#define numb_case_2_23_4 "2.23.4"
#define name_case_2_23_4 "Perform streamio I_PUNLINK - EPROTO."
#define sref_case_2_23_4 sref_case_2_23
#define desc_case_2_23_4 "\
Checks that I_PUNLINK can be performed on a read errored Stream.\n\
This test should return EPROTO."

int
test_case_2_23_4(int child)
{
	if (test_ioctl(child, I_PUNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_23_4 = { &preamble_4_2_2, &test_case_2_23_4, &postamble_4_2 };

#define test_case_2_23_4_stream_0 (&test_2_23_4)
#define test_case_2_23_4_stream_1 (NULL)
#define test_case_2_23_4_stream_2 (NULL)

#define tgrp_case_2_23_5 test_group_2
#define numb_case_2_23_5 "2.23.5"
#define name_case_2_23_5 "Perform streamio I_PUNLINK - EPROTO."
#define sref_case_2_23_5 sref_case_2_23
#define desc_case_2_23_5 "\
Checks that I_PUNLINK can be performed on a write errored Stream.\n\
This test should return EPROTO."

int
test_case_2_23_5(int child)
{
	if (test_ioctl(child, I_PUNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_23_5 = { &preamble_4_2_3, &test_case_2_23_5, &postamble_4_2 };

#define test_case_2_23_5_stream_0 (&test_2_23_5)
#define test_case_2_23_5_stream_1 (NULL)
#define test_case_2_23_5_stream_2 (NULL)

#define tgrp_case_2_23_6 test_group_2
#define numb_case_2_23_6 "2.23.6"
#define name_case_2_23_6 "Perform streamio I_PUNLINK - EPROTO."
#define sref_case_2_23_6 sref_case_2_23
#define desc_case_2_23_6 "\
Checks that I_PUNLINK can be performed on an errored Stream.\n\
This test should return EPROTO."

int
test_case_2_23_6(int child)
{
	if (test_ioctl(child, I_PUNLINK, (intptr_t) MUXID_ALL) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_23_6 = { &preamble_4_2_4, &test_case_2_23_6, &postamble_4_2 };

#define test_case_2_23_6_stream_0 (&test_2_23_6)
#define test_case_2_23_6_stream_1 (NULL)
#define test_case_2_23_6_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_FLUSHBAND
 */
static const char sref_case_2_24[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_FLUSHBAND section.";

#define tgrp_case_2_24_1 test_group_2
#define numb_case_2_24_1 "2.24.1"
#define name_case_2_24_1 "Perform streamio I_FLUSHBAND - EINVAL."
#define sref_case_2_24_1 sref_case_2_24
#define desc_case_2_24_1 "\
Checks that I_FLUSHBAND with an invalid argument returns EINVAL."

int
test_case_2_24_1(int child)
{
	struct bandinfo bi = { 0, 0 };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_1 = { &preamble_0, &test_case_2_24_1, &postamble_0 };

#define test_case_2_24_1_stream_0 (&test_2_24_1)
#define test_case_2_24_1_stream_1 (NULL)
#define test_case_2_24_1_stream_2 (NULL)

#define tgrp_case_2_24_2 test_group_2
#define numb_case_2_24_2 "2.24.2"
#define name_case_2_24_2 "Perform streamio I_FLUSHBAND - EINVAL."
#define sref_case_2_24_2 sref_case_2_24
#define desc_case_2_24_2 "\
Checks that I_FLUSHBAND with an invalid argument returns EINVAL."

int
test_case_2_24_2(int child)
{
	struct bandinfo bi = { 0, -1};

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_2 = { &preamble_0, &test_case_2_24_2, &postamble_0 };

#define test_case_2_24_2_stream_0 (&test_2_24_2)
#define test_case_2_24_2_stream_1 (NULL)
#define test_case_2_24_2_stream_2 (NULL)

#define tgrp_case_2_24_3 test_group_2
#define numb_case_2_24_3 "2.24.3"
#define name_case_2_24_3 "Perform streamio I_FLUSHBAND - EFAULT."
#define sref_case_2_24_3 sref_case_2_24
#define desc_case_2_24_3 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_24_3(int child)
{
	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_3 = { &preamble_0, &test_case_2_24_3, &postamble_0 };

#define test_case_2_24_3_stream_0 (&test_2_24_3)
#define test_case_2_24_3_stream_1 (NULL)
#define test_case_2_24_3_stream_2 (NULL)

#define tgrp_case_2_24_4 test_group_2
#define numb_case_2_24_4 "2.24.4"
#define name_case_2_24_4 "Perform streamio I_FLUSHBAND - band 0 FLUSHR."
#define sref_case_2_24_4 sref_case_2_24
#define desc_case_2_24_4 "\
Checks that I_FLUSHBAND succeeds on band zero with FLUSHR."

int
test_case_2_24_4(int child)
{
	struct bandinfo bi = { 0, FLUSHR };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_4 = { &preamble_0, &test_case_2_24_4, &postamble_0 };

#define test_case_2_24_4_stream_0 (&test_2_24_4)
#define test_case_2_24_4_stream_1 (NULL)
#define test_case_2_24_4_stream_2 (NULL)

#define tgrp_case_2_24_5 test_group_2
#define numb_case_2_24_5 "2.24.5"
#define name_case_2_24_5 "Perform streamio I_FLUSHBAND - band 0 FLUSHW."
#define sref_case_2_24_5 sref_case_2_24
#define desc_case_2_24_5 "\
Checks that I_FLUSHBAND succeeds on band zero with FLUSHW."

int
test_case_2_24_5(int child)
{
	struct bandinfo bi = { 0, FLUSHW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_5 = { &preamble_0, &test_case_2_24_5, &postamble_0 };

#define test_case_2_24_5_stream_0 (&test_2_24_5)
#define test_case_2_24_5_stream_1 (NULL)
#define test_case_2_24_5_stream_2 (NULL)

#define tgrp_case_2_24_6 test_group_2
#define numb_case_2_24_6 "2.24.6"
#define name_case_2_24_6 "Perform streamio I_FLUSHBAND - band 0 FLUSHRW."
#define sref_case_2_24_6 sref_case_2_24
#define desc_case_2_24_6 "\
Checks that I_FLUSHBAND succeeds on band zero with FLUSHRW."

int
test_case_2_24_6(int child)
{
	struct bandinfo bi = { 0, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_6 = { &preamble_0, &test_case_2_24_6, &postamble_0 };

#define test_case_2_24_6_stream_0 (&test_2_24_6)
#define test_case_2_24_6_stream_1 (NULL)
#define test_case_2_24_6_stream_2 (NULL)

#define tgrp_case_2_24_7 test_group_2
#define numb_case_2_24_7 "2.24.7"
#define name_case_2_24_7 "Perform streamio I_FLUSHBAND - banded FLUSHR."
#define sref_case_2_24_7 sref_case_2_24
#define desc_case_2_24_7 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHR."

int
test_case_2_24_7(int child)
{
	struct bandinfo bi = { 1, FLUSHR };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_7 = { &preamble_0, &test_case_2_24_7, &postamble_0 };

#define test_case_2_24_7_stream_0 (&test_2_24_7)
#define test_case_2_24_7_stream_1 (NULL)
#define test_case_2_24_7_stream_2 (NULL)

#define tgrp_case_2_24_8 test_group_2
#define numb_case_2_24_8 "2.24.8"
#define name_case_2_24_8 "Perform streamio I_FLUSHBAND - banded FLUSHW."
#define sref_case_2_24_8 sref_case_2_24
#define desc_case_2_24_8 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHW."

int
test_case_2_24_8(int child)
{
	struct bandinfo bi = { 1, FLUSHW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_8 = { &preamble_0, &test_case_2_24_8, &postamble_0 };

#define test_case_2_24_8_stream_0 (&test_2_24_8)
#define test_case_2_24_8_stream_1 (NULL)
#define test_case_2_24_8_stream_2 (NULL)

#define tgrp_case_2_24_9 test_group_2
#define numb_case_2_24_9 "2.24.9"
#define name_case_2_24_9 "Perform streamio I_FLUSHBAND - banded FLUSHRW."
#define sref_case_2_24_9 sref_case_2_24
#define desc_case_2_24_9 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHRW."

int
test_case_2_24_9(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_9 = { &preamble_0, &test_case_2_24_9, &postamble_0 };

#define test_case_2_24_9_stream_0 (&test_2_24_9)
#define test_case_2_24_9_stream_1 (NULL)
#define test_case_2_24_9_stream_2 (NULL)

#define tgrp_case_2_24_10 test_group_2
#define numb_case_2_24_10 "2.24.10"
#define name_case_2_24_10 "Perform streamio I_FLUSHBAND - ENXIO."
#define sref_case_2_24_10 sref_case_2_24
#define desc_case_2_24_10 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHRW.\n\
This test is perfomed on a hung up Stream and should return ENXIO."

int
test_case_2_24_10(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_10 = { &preamble_2_1, &test_case_2_24_10, &postamble_2 };

#define test_case_2_24_10_stream_0 (&test_2_24_10)
#define test_case_2_24_10_stream_1 (NULL)
#define test_case_2_24_10_stream_2 (NULL)

#define tgrp_case_2_24_11 test_group_2
#define numb_case_2_24_11 "2.24.11"
#define name_case_2_24_11 "Perform streamio I_FLUSHBAND - EPROTO."
#define sref_case_2_24_11 sref_case_2_24
#define desc_case_2_24_11 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHRW.\n\
This test is perfomed on a read errored Stream and should return EPROTO."

int
test_case_2_24_11(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_11 = { &preamble_2_2, &test_case_2_24_11, &postamble_2 };

#define test_case_2_24_11_stream_0 (&test_2_24_11)
#define test_case_2_24_11_stream_1 (NULL)
#define test_case_2_24_11_stream_2 (NULL)

#define tgrp_case_2_24_12 test_group_2
#define numb_case_2_24_12 "2.24.12"
#define name_case_2_24_12 "Perform streamio I_FLUSHBAND - EPROTO."
#define sref_case_2_24_12 sref_case_2_24
#define desc_case_2_24_12 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHRW.\n\
This test is perfomed on a write errored Stream and should return EPROTO."

int
test_case_2_24_12(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_12 = { &preamble_2_3, &test_case_2_24_12, &postamble_2 };

#define test_case_2_24_12_stream_0 (&test_2_24_12)
#define test_case_2_24_12_stream_1 (NULL)
#define test_case_2_24_12_stream_2 (NULL)

#define tgrp_case_2_24_13 test_group_2
#define numb_case_2_24_13 "2.24.13"
#define name_case_2_24_13 "Perform streamio I_FLUSHBAND - EPROTO."
#define sref_case_2_24_13 sref_case_2_24
#define desc_case_2_24_13 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHRW.\n\
This test is perfomed on an errored Stream and should return EPROTO."

int
test_case_2_24_13(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_13 = { &preamble_2_4, &test_case_2_24_13, &postamble_2 };

#define test_case_2_24_13_stream_0 (&test_2_24_13)
#define test_case_2_24_13_stream_1 (NULL)
#define test_case_2_24_13_stream_2 (NULL)

#define tgrp_case_2_24_14 test_group_2
#define numb_case_2_24_14 "2.24.14"
#define name_case_2_24_14 "Perform streamio I_FLUSHBAND - EINVAL."
#define sref_case_2_24_14 sref_case_2_24
#define desc_case_2_24_14 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHRW.\n\
This test is perfomed on a Stream linked under a Multiplexing Driver and\n\
should return EINVAL."

int
test_case_2_24_14(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_14 = { &preamble_5, &test_case_2_24_14, &postamble_5 };

#define test_case_2_24_14_stream_0 (&test_2_24_14)
#define test_case_2_24_14_stream_1 (NULL)
#define test_case_2_24_14_stream_2 (NULL)

#define tgrp_case_2_24_15 test_group_2
#define numb_case_2_24_15 "2.24.15"
#define name_case_2_24_15 "Perform streamio I_FLUSHBAND."
#define sref_case_2_24_15 sref_case_2_24
#define desc_case_2_24_15 "\
Checks that I_FLUSHBAND succeeds on a non-zero band with FLUSHRW.\n\
Checks that only the specified messages are flushed."

int
preamble_test_case_2_24_15(int child)
{
	char buf[32] = { 0, };
	struct strbuf ctl = { sizeof(buf), sizeof(buf), buf };
	struct strbuf dat = { sizeof(buf), sizeof(buf), buf };

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 2, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 2, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_24_15(int child)
{
	struct bandinfo bi = { 1, FLUSHRW };

	char buf[32] = { 0, };
	struct strbuf ctl = { sizeof(buf), sizeof(buf), buf };
	struct strbuf dat = { sizeof(buf), sizeof(buf), buf };
	int band = 0;
	int flags = MSG_ANY;

	if (test_ioctl(child, I_FLUSHBAND, (intptr_t) & bi) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (flags != MSG_BAND || band != 2)
		return (__RESULT_FAILURE);
	state++;
	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (flags != MSG_BAND || band != 2)
		return (__RESULT_FAILURE);
	state++;
	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (flags != MSG_BAND || band != 0)
		return (__RESULT_FAILURE);
	state++;
	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (flags != MSG_BAND || band != 0)
		return (__RESULT_FAILURE);
	state++;
	state++;
	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24_15 = { &preamble_test_case_2_24_15, &test_case_2_24_15, &postamble_0 };

#define test_case_2_24_15_stream_0 (&test_2_24_15)
#define test_case_2_24_15_stream_1 (NULL)
#define test_case_2_24_15_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_CKBAND
 */
static const char sref_case_2_25[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_CKBAND section.";

#define tgrp_case_2_25_1 test_group_2
#define numb_case_2_25_1 "2.25.1"
#define name_case_2_25_1 "Perform streamio I_CKBAND."
#define sref_case_2_25_1 sref_case_2_25
#define desc_case_2_25_1 "\
Checks that I_CKBAND can be performed on a Stream."

int
test_case_2_25_1(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_1 = { &preamble_0, &test_case_2_25_1, &postamble_0 };

#define test_case_2_25_1_stream_0 (&test_2_25_1)
#define test_case_2_25_1_stream_1 (NULL)
#define test_case_2_25_1_stream_2 (NULL)

#define tgrp_case_2_25_2 test_group_2
#define numb_case_2_25_2 "2.25.2"
#define name_case_2_25_2 "Perform streamio I_CKBAND."
#define sref_case_2_25_2 sref_case_2_25
#define desc_case_2_25_2 "\
Checks that I_CKBAND can be performed on a hung up Stream."

int
test_case_2_25_2(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_2 = { &preamble_2_1, &test_case_2_25_2, &postamble_2 };

#define test_case_2_25_2_stream_0 (&test_2_25_2)
#define test_case_2_25_2_stream_1 (NULL)
#define test_case_2_25_2_stream_2 (NULL)

#define tgrp_case_2_25_3 test_group_2
#define numb_case_2_25_3 "2.25.3"
#define name_case_2_25_3 "Perform streamio I_CKBAND - EPROTO."
#define sref_case_2_25_3 sref_case_2_25
#define desc_case_2_25_3 "\
Checks that I_CKBAND can be performed on a read errored Stream.\n\
This test should return EPROTO."

int
test_case_2_25_3(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_3 = { &preamble_2_2, &test_case_2_25_3, &postamble_2 };

#define test_case_2_25_3_stream_0 (&test_2_25_3)
#define test_case_2_25_3_stream_1 (NULL)
#define test_case_2_25_3_stream_2 (NULL)

#define tgrp_case_2_25_4 test_group_2
#define numb_case_2_25_4 "2.25.4"
#define name_case_2_25_4 "Perform streamio I_CKBAND."
#define sref_case_2_25_4 sref_case_2_25
#define desc_case_2_25_4 "\
Checks that I_CKBAND can be performed on a write errored Stream."

int
test_case_2_25_4(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_4 = { &preamble_2_3, &test_case_2_25_4, &postamble_2 };

#define test_case_2_25_4_stream_0 (&test_2_25_4)
#define test_case_2_25_4_stream_1 (NULL)
#define test_case_2_25_4_stream_2 (NULL)

#define tgrp_case_2_25_5 test_group_2
#define numb_case_2_25_5 "2.25.5"
#define name_case_2_25_5 "Perform streamio I_CKBAND - EPROTO."
#define sref_case_2_25_5 sref_case_2_25
#define desc_case_2_25_5 "\
Checks that I_CKBAND can be performed on an errored Stream.\n\
This test should return EPROTO."

int
test_case_2_25_5(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_5 = { &preamble_2_4, &test_case_2_25_5, &postamble_2 };

#define test_case_2_25_5_stream_0 (&test_2_25_5)
#define test_case_2_25_5_stream_1 (NULL)
#define test_case_2_25_5_stream_2 (NULL)

#define tgrp_case_2_25_6 test_group_2
#define numb_case_2_25_6 "2.25.6"
#define name_case_2_25_6 "Perform streamio I_CKBAND - EINVAL."
#define sref_case_2_25_6 sref_case_2_25
#define desc_case_2_25_6 "\
Checks that I_CKBAND can be performed on a Stream linked under a\n\
Multiplexing Driver.  This test should return EINVAL."

int
test_case_2_25_6(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_6 = { &preamble_5, &test_case_2_25_6, &postamble_5 };

#define test_case_2_25_6_stream_0 (&test_2_25_6)
#define test_case_2_25_6_stream_1 (NULL)
#define test_case_2_25_6_stream_2 (NULL)

#define tgrp_case_2_25_7 test_group_2
#define numb_case_2_25_7 "2.25.7"
#define name_case_2_25_7 "Perform streamio I_CKBAND."
#define sref_case_2_25_7 sref_case_2_25
#define desc_case_2_25_7 "\
Checks that I_CKBAND can be performed on a Stream.  Checks that I_CKBAND\n\
returns false for various bands on an empty queue."

int
test_case_2_25_7(int child)
{
	if (test_ioctl(child, I_CKBAND, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 1) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 7) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 255) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_7 = { &preamble_0, &test_case_2_25_7, &postamble_0 };

#define test_case_2_25_7_stream_0 (&test_2_25_7)
#define test_case_2_25_7_stream_1 (NULL)
#define test_case_2_25_7_stream_2 (NULL)

#define tgrp_case_2_25_8 test_group_2
#define numb_case_2_25_8 "2.25.8"
#define name_case_2_25_8 "Perform streamio I_CKBAND."
#define sref_case_2_25_8 sref_case_2_25
#define desc_case_2_25_8 "\
Checks that I_CKBAND can be performed on a Stream.  Checks that I_CKBAND\n\
returns true for various bands on a queue."

int
preamble_test_case_2_25_8(int child)
{
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 7, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_25_8(int child)
{
	if (test_ioctl(child, I_CKBAND, 7) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 1) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 2) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 6) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 8) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CKBAND, 255) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25_8 = { &preamble_test_case_2_25_8, &test_case_2_25_8, &postamble_0 };

#define test_case_2_25_8_stream_0 (&test_2_25_8)
#define test_case_2_25_8_stream_1 (NULL)
#define test_case_2_25_8_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GETBAND
 */
static const char sref_case_2_26[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_GETBAND section.";

#define tgrp_case_2_26_1 test_group_2
#define numb_case_2_26_1 "2.26.1"
#define name_case_2_26_1 "Perform streamio I_GETBAND - ENODATA."
#define sref_case_2_26_1 sref_case_2_26
#define desc_case_2_26_1 "\
Checks that I_GETBAND can be performed on a Stream."

int
test_case_2_26_1(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != ENODATA)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_1 = { &preamble_0, &test_case_2_26_1, &postamble_0 };

#define test_case_2_26_1_stream_0 (&test_2_26_1)
#define test_case_2_26_1_stream_1 (NULL)
#define test_case_2_26_1_stream_2 (NULL)

#define tgrp_case_2_26_2 test_group_2
#define numb_case_2_26_2 "2.26.2"
#define name_case_2_26_2 "Perform streamio I_GETBAND - ENODATA."
#define sref_case_2_26_2 sref_case_2_26
#define desc_case_2_26_2 "\
Checks that I_GETBAND can be performed on a hung up Stream."

int
test_case_2_26_2(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != ENODATA)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_2 = { &preamble_2_1, &test_case_2_26_2, &postamble_2 };

#define test_case_2_26_2_stream_0 (&test_2_26_2)
#define test_case_2_26_2_stream_1 (NULL)
#define test_case_2_26_2_stream_2 (NULL)

#define tgrp_case_2_26_3 test_group_2
#define numb_case_2_26_3 "2.26.3"
#define name_case_2_26_3 "Perform streamio I_GETBAND - EPROTO."
#define sref_case_2_26_3 sref_case_2_26
#define desc_case_2_26_3 "\
Checks that I_GETBAND can be performed on a read errored Stream.\n\
This test should return EPROTO."

int
test_case_2_26_3(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_3 = { &preamble_2_2, &test_case_2_26_3, &postamble_2 };

#define test_case_2_26_3_stream_0 (&test_2_26_3)
#define test_case_2_26_3_stream_1 (NULL)
#define test_case_2_26_3_stream_2 (NULL)

#define tgrp_case_2_26_4 test_group_2
#define numb_case_2_26_4 "2.26.4"
#define name_case_2_26_4 "Perform streamio I_GETBAND - ENODATA."
#define sref_case_2_26_4 sref_case_2_26
#define desc_case_2_26_4 "\
Checks that I_GETBAND can be performed on a write errored Stream."

int
test_case_2_26_4(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != ENODATA)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_4 = { &preamble_2_3, &test_case_2_26_4, &postamble_2 };

#define test_case_2_26_4_stream_0 (&test_2_26_4)
#define test_case_2_26_4_stream_1 (NULL)
#define test_case_2_26_4_stream_2 (NULL)

#define tgrp_case_2_26_5 test_group_2
#define numb_case_2_26_5 "2.26.5"
#define name_case_2_26_5 "Perform streamio I_GETBAND - EPROTO."
#define sref_case_2_26_5 sref_case_2_26
#define desc_case_2_26_5 "\
Checks that I_GETBAND can be performed on an errored Stream.\n\
This test should return EPROTO."

int
test_case_2_26_5(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_5 = { &preamble_2_4, &test_case_2_26_5, &postamble_2 };

#define test_case_2_26_5_stream_0 (&test_2_26_5)
#define test_case_2_26_5_stream_1 (NULL)
#define test_case_2_26_5_stream_2 (NULL)

#define tgrp_case_2_26_6 test_group_2
#define numb_case_2_26_6 "2.26.6"
#define name_case_2_26_6 "Perform streamio I_GETBAND - EINVAL."
#define sref_case_2_26_6 sref_case_2_26
#define desc_case_2_26_6 "\
Checks that I_GETBAND can be performed on a Stream linked under a\n\
Multiplexing Driver.  This test should return EPROTO."

int
test_case_2_26_6(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_6 = { &preamble_5, &test_case_2_26_6, &postamble_5 };

#define test_case_2_26_6_stream_0 (&test_2_26_6)
#define test_case_2_26_6_stream_1 (NULL)
#define test_case_2_26_6_stream_2 (NULL)

#define tgrp_case_2_26_7 test_group_2
#define numb_case_2_26_7 "2.26.7"
#define name_case_2_26_7 "Perform streamio I_GETBAND."
#define sref_case_2_26_7 sref_case_2_26
#define desc_case_2_26_7 "\
Checks that I_GETBAND can be performed on a Stream.  Checks that\n\
I_GETBAND returns the correct band number when an M_DATA message is at\n\
the head of the Stream head read queue."

int
preamble_test_case_2_26_7(int child)
{
	char dbuf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_26_7(int child)
{
	int band = 1;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (band != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_7 = { &preamble_test_case_2_26_7, &test_case_2_26_7, &postamble_0 };

#define test_case_2_26_7_stream_0 (&test_2_26_7)
#define test_case_2_26_7_stream_1 (NULL)
#define test_case_2_26_7_stream_2 (NULL)

#define tgrp_case_2_26_8 test_group_2
#define numb_case_2_26_8 "2.26.8"
#define name_case_2_26_8 "Perform streamio I_GETBAND."
#define sref_case_2_26_8 sref_case_2_26
#define desc_case_2_26_8 "\
Checks that I_GETBAND can be performed on a Stream.  Checks that\n\
I_GETBAND returns the correct band number when a high-priority message\n\
is at the head of the Stream head read queue."

int
preamble_test_case_2_26_8(int child)
{
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 0, MSG_HIPRI) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_26_8(int child)
{
	int band = 1;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (band != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_8 = { &preamble_test_case_2_26_8, &test_case_2_26_8, &postamble_0 };

#define test_case_2_26_8_stream_0 (&test_2_26_8)
#define test_case_2_26_8_stream_1 (NULL)
#define test_case_2_26_8_stream_2 (NULL)

#define tgrp_case_2_26_9 test_group_2
#define numb_case_2_26_9 "2.26.9"
#define name_case_2_26_9 "Perform streamio I_GETBAND."
#define sref_case_2_26_9 sref_case_2_26
#define desc_case_2_26_9 "\
Checks that I_GETBAND can be performed on a Stream.  Checks that\n\
I_GETBAND returns the correct band number when a priority message is at\n\
the head of the Stream head read queue."

int
preamble_test_case_2_26_9(int child)
{
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_26_9(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (band != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26_9 = { &preamble_test_case_2_26_9, &test_case_2_26_9, &postamble_0 };

#define test_case_2_26_9_stream_0 (&test_2_26_9)
#define test_case_2_26_9_stream_1 (NULL)
#define test_case_2_26_9_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_ATMARK
 */
static const char sref_case_2_27[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_ATMARK section.";

#define tgrp_case_2_27_1 test_group_2
#define numb_case_2_27_1 "2.27.1"
#define name_case_2_27_1 "Perform streamio I_ATMARK - false."
#define sref_case_2_27_1 sref_case_2_27
#define desc_case_2_27_1 "\
Checks that I_ATMARK can be performed on a Stream."

int
test_case_2_27_1(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_1 = { &preamble_0, &test_case_2_27_1, &postamble_0 };

#define test_case_2_27_1_stream_0 (&test_2_27_1)
#define test_case_2_27_1_stream_1 (NULL)
#define test_case_2_27_1_stream_2 (NULL)

#define tgrp_case_2_27_2 test_group_2
#define numb_case_2_27_2 "2.27.2"
#define name_case_2_27_2 "Perform streamio I_ATMARK - false."
#define sref_case_2_27_2 sref_case_2_27
#define desc_case_2_27_2 "\
Checks that I_ATMARK can be performed on a hung up Stream."

int
test_case_2_27_2(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_2 = { &preamble_2_1, &test_case_2_27_2, &postamble_2 };

#define test_case_2_27_2_stream_0 (&test_2_27_2)
#define test_case_2_27_2_stream_1 (NULL)
#define test_case_2_27_2_stream_2 (NULL)

#define tgrp_case_2_27_3 test_group_2
#define numb_case_2_27_3 "2.27.3"
#define name_case_2_27_3 "Perform streamio I_ATMARK - EPROTO."
#define sref_case_2_27_3 sref_case_2_27
#define desc_case_2_27_3 "\
Checks that I_ATMARK can be performed on a read errored Stream.\n\
This test should return EPROTO."

int
test_case_2_27_3(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_3 = { &preamble_2_2, &test_case_2_27_3, &postamble_2 };

#define test_case_2_27_3_stream_0 (&test_2_27_3)
#define test_case_2_27_3_stream_1 (NULL)
#define test_case_2_27_3_stream_2 (NULL)

#define tgrp_case_2_27_4 test_group_2
#define numb_case_2_27_4 "2.27.4"
#define name_case_2_27_4 "Perform streamio I_ATMARK - false."
#define sref_case_2_27_4 sref_case_2_27
#define desc_case_2_27_4 "\
Checks that I_ATMARK can be performed on a write errored Stream."

int
test_case_2_27_4(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_4 = { &preamble_2_3, &test_case_2_27_4, &postamble_2 };

#define test_case_2_27_4_stream_0 (&test_2_27_4)
#define test_case_2_27_4_stream_1 (NULL)
#define test_case_2_27_4_stream_2 (NULL)

#define tgrp_case_2_27_5 test_group_2
#define numb_case_2_27_5 "2.27.5"
#define name_case_2_27_5 "Perform streamio I_ATMARK - EROTO."
#define sref_case_2_27_5 sref_case_2_27
#define desc_case_2_27_5 "\
Checks that I_ATMARK can be performed on an errored Stream.\n\
This test should return EPROTO."

int
test_case_2_27_5(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_5 = { &preamble_2_4, &test_case_2_27_5, &postamble_2 };

#define test_case_2_27_5_stream_0 (&test_2_27_5)
#define test_case_2_27_5_stream_1 (NULL)
#define test_case_2_27_5_stream_2 (NULL)

#define tgrp_case_2_27_6 test_group_2
#define numb_case_2_27_6 "2.27.6"
#define name_case_2_27_6 "Perform streamio I_ATMARK - EINVAL."
#define sref_case_2_27_6 sref_case_2_27
#define desc_case_2_27_6 "\
Checks that I_ATMARK can be performed on a Stream linked under a\n\
Multiplexing Driver.  This test should return EINVAL."

int
test_case_2_27_6(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_6 = { &preamble_5, &test_case_2_27_6, &postamble_5 };

#define test_case_2_27_6_stream_0 (&test_2_27_6)
#define test_case_2_27_6_stream_1 (NULL)
#define test_case_2_27_6_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_SETCLTIME
 */
static const char sref_case_2_28[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_SETCLTIME section.";

#define tgrp_case_2_28_1 test_group_2
#define numb_case_2_28_1 "2.28.1"
#define name_case_2_28_1 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_1 sref_case_2_28
#define desc_case_2_28_1 "\
Checks that I_SETCLTIME can be performed on a Stream.\n\
Checks that the close time can be set to zero."

int
test_case_2_28_1(int child)
{
	int cltime = 0;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_1 = { &preamble_0, &test_case_2_28_1, &postamble_0 };

#define test_case_2_28_1_stream_0 (&test_2_28_1)
#define test_case_2_28_1_stream_1 (NULL)
#define test_case_2_28_1_stream_2 (NULL)

#define tgrp_case_2_28_2 test_group_2
#define numb_case_2_28_2 "2.28.2"
#define name_case_2_28_2 "Perform streamio I_SETCLTIME - EFAULT."
#define sref_case_2_28_2 sref_case_2
#define desc_case_2_28_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_28_2(int child)
{
	if (test_ioctl(child, I_SETCLTIME, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_2 = { &preamble_0, &test_case_2_28_2, &postamble_0 };

#define test_case_2_28_2_stream_0 (&test_2_28_2)
#define test_case_2_28_2_stream_1 (NULL)
#define test_case_2_28_2_stream_2 (NULL)

#define tgrp_case_2_28_3 test_group_2
#define numb_case_2_28_3 "2.28.3"
#define name_case_2_28_3 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_3 sref_case_2_28
#define desc_case_2_28_3 "\
Checks that I_SETCLTIME can be performed on a hung up Stream.\n\
Checks that the close time can be set to zero."

int
test_case_2_28_3(int child)
{
	int cltime = 0;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_3 = { &preamble_2_1, &test_case_2_28_3, &postamble_2 };

#define test_case_2_28_3_stream_0 (&test_2_28_3)
#define test_case_2_28_3_stream_1 (NULL)
#define test_case_2_28_3_stream_2 (NULL)

#define tgrp_case_2_28_4 test_group_2
#define numb_case_2_28_4 "2.28.4"
#define name_case_2_28_4 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_4 sref_case_2_28
#define desc_case_2_28_4 "\
Checks that I_SETCLTIME can be performed on a read errored Stream.\n\
Checks that the close time can be set to zero."

int
test_case_2_28_4(int child)
{
	int cltime = 0;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_4 = { &preamble_2_2, &test_case_2_28_4, &postamble_2 };

#define test_case_2_28_4_stream_0 (&test_2_28_4)
#define test_case_2_28_4_stream_1 (NULL)
#define test_case_2_28_4_stream_2 (NULL)

#define tgrp_case_2_28_5 test_group_2
#define numb_case_2_28_5 "2.28.5"
#define name_case_2_28_5 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_5 sref_case_2_28
#define desc_case_2_28_5 "\
Checks that I_SETCLTIME can be performed on a write errored Stream.\n\
Checks that the close time can be set to zero."

int
test_case_2_28_5(int child)
{
	int cltime = 0;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_5 = { &preamble_2_3, &test_case_2_28_5, &postamble_2 };

#define test_case_2_28_5_stream_0 (&test_2_28_5)
#define test_case_2_28_5_stream_1 (NULL)
#define test_case_2_28_5_stream_2 (NULL)

#define tgrp_case_2_28_6 test_group_2
#define numb_case_2_28_6 "2.28.6"
#define name_case_2_28_6 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_6 sref_case_2_28
#define desc_case_2_28_6 "\
Checks that I_SETCLTIME can be performed on an errored Stream.\n\
Checks that the close time can be set to zero."

int
test_case_2_28_6(int child)
{
	int cltime = 0;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_6 = { &preamble_2_4, &test_case_2_28_6, &postamble_2 };

#define test_case_2_28_6_stream_0 (&test_2_28_6)
#define test_case_2_28_6_stream_1 (NULL)
#define test_case_2_28_6_stream_2 (NULL)

#define tgrp_case_2_28_7 test_group_2
#define numb_case_2_28_7 "2.28.7"
#define name_case_2_28_7 "Perform streamio I_SETCLTIME - EINVAL."
#define sref_case_2_28_7 sref_case_2_28
#define desc_case_2_28_7 "\
Checks that I_SETCLTIME can be performed on a Stream linked under a\n\
Multiplexing Driver.  Checks that EINVAL is returned."

int
test_case_2_28_7(int child)
{
	int cltime = 0;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_7 = { &preamble_5, &test_case_2_28_7, &postamble_5 };

#define test_case_2_28_7_stream_0 (&test_2_28_7)
#define test_case_2_28_7_stream_1 (NULL)
#define test_case_2_28_7_stream_2 (NULL)

#define tgrp_case_2_28_8 test_group_2
#define numb_case_2_28_8 "2.28.8"
#define name_case_2_28_8 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_8 sref_case_2_28
#define desc_case_2_28_8 "\
Checks that I_SETCLTIME can be performed on a Stream.  Checks that a\n\
Stream with a full write queue takes the specified amount of time to\n\
close."

int
preamble_test_case_2_28_8(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_errno = 0;
	do {
		test_putmsg(child, &ctl, &dat, flags);
		state++;
	} while (last_errno != EAGAIN);
	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_28_8(int child)
{
	int cltime = 1000;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_signum = 0;
	if (start_tt(cltime - 100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_close(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
int
postamble_test_case_2_28_8(int child)
{
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_8 = { &preamble_test_case_2_28_8, &test_case_2_28_8, &postamble_test_case_2_28_8 };

#define test_case_2_28_8_stream_0 (&test_2_28_8)
#define test_case_2_28_8_stream_1 (NULL)
#define test_case_2_28_8_stream_2 (NULL)

#define tgrp_case_2_28_9 test_group_2
#define numb_case_2_28_9 "2.28.9"
#define name_case_2_28_9 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_9 sref_case_2_28
#define desc_case_2_28_9 "\
Checks that I_SETCLTIME can be performed on a Stream.  Checks that a\n\
Stream with a full write queue takes the default amount of time to\n\
close."

int
preamble_test_case_2_28_9(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_errno = 0;
	do {
		test_putmsg(child, &ctl, &dat, flags);
		state++;
	} while (last_errno != EAGAIN);
	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_28_9(int child)
{
	int cltime = 15000;

	last_signum = 0;
	if (start_tt(cltime - 100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_close(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
int
postamble_test_case_2_28_9(int child)
{
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_9 = { &preamble_test_case_2_28_9, &test_case_2_28_9, &postamble_test_case_2_28_9 };

#define test_case_2_28_9_stream_0 (&test_2_28_9)
#define test_case_2_28_9_stream_1 (NULL)
#define test_case_2_28_9_stream_2 (NULL)

#define tgrp_case_2_28_10 test_group_2
#define numb_case_2_28_10 "2.28.10"
#define name_case_2_28_10 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_10 sref_case_2_28
#define desc_case_2_28_10 "\
Checks that I_SETCLTIME can be performed on a Stream.  Checks that a\n\
Stream with a full write queue wait can be interrupted by a signal."

int
preamble_test_case_2_28_10(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_errno = 0;
	do {
		test_putmsg(child, &ctl, &dat, flags);
		state++;
	} while (last_errno != EAGAIN);
	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_2_28_10(int child)
{
	last_signum = 0;
	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_close(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != SIGALRM)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
int
postamble_test_case_2_28_10(int child)
{
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_10 = { &preamble_test_case_2_28_10, &test_case_2_28_10, &postamble_test_case_2_28_10 };

#define test_case_2_28_10_stream_0 (&test_2_28_10)
#define test_case_2_28_10_stream_1 (NULL)
#define test_case_2_28_10_stream_2 (NULL)

#define tgrp_case_2_28_11 test_group_2
#define numb_case_2_28_11 "2.28.11"
#define name_case_2_28_11 "Perform streamio I_SETCLTIME."
#define sref_case_2_28_11 sref_case_2_28
#define desc_case_2_28_11 "\
Checks that I_SETCLTIME can be performed on a Stream.  Checks that a\n\
Stream with a full write queue will not block on close if non-blocking\n\
operation set on file descriptor."

int
preamble_test_case_2_28_11(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	last_errno = 0;
	do {
		test_putmsg(child, &ctl, &dat, flags);
		state++;
	} while (last_errno != EAGAIN);
	return (__RESULT_SUCCESS);
}
int
test_case_2_28_11(int child)
{
	last_signum = 0;
	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_close(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_signum != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
postamble_test_case_2_28_11(int child)
{
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_11 = { &preamble_test_case_2_28_11, &test_case_2_28_11, &postamble_test_case_2_28_11 };

#define test_case_2_28_11_stream_0 (&test_2_28_11)
#define test_case_2_28_11_stream_1 (NULL)
#define test_case_2_28_11_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GETCLTIME
 */
static const char sref_case_2_29[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_GETCLTIME section.";

#define tgrp_case_2_29_1 test_group_2
#define numb_case_2_29_1 "2.29.1"
#define name_case_2_29_1 "Perform streamio I_GETCLTIME - default."
#define sref_case_2_29_1 sref_case_2_29
#define desc_case_2_29_1 "\
Checks that I_GETCLTIME can be performed on a Stream.\n\
Checks that the default close time is 15000 milliseconds (or 15 seconds)."

int
test_case_2_29_1(int child)
{
	int cltime;

	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS || cltime != 15000)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_1 = { &preamble_0, &test_case_2_29_1, &postamble_0 };

#define test_case_2_29_1_stream_0 (&test_2_29_1)
#define test_case_2_29_1_stream_1 (NULL)
#define test_case_2_29_1_stream_2 (NULL)

#define tgrp_case_2_29_2 test_group_2
#define numb_case_2_29_2 "2.29.2"
#define name_case_2_29_2 "Perform streamio I_GETCLTIME - EFAULT."
#define sref_case_2_29_2 sref_case_2_29
#define desc_case_2_29_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_29_2(int child)
{
	if (test_ioctl(child, I_GETCLTIME, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_2 = { &preamble_0, &test_case_2_29_2, &postamble_0 };

#define test_case_2_29_2_stream_0 (&test_2_29_2)
#define test_case_2_29_2_stream_1 (NULL)
#define test_case_2_29_2_stream_2 (NULL)

#define tgrp_case_2_29_3 test_group_2
#define numb_case_2_29_3 "2.29.3"
#define name_case_2_29_3 "Perform streamio I_GETCLTIME - default."
#define sref_case_2_29_3 sref_case_2_29
#define desc_case_2_29_3 "\
Checks that I_GETCLTIME can be performed on a hung up Stream.\n\
Checks that the default close time is 15000 milliseconds (or 15 seconds)."

int
test_case_2_29_3(int child)
{
	int cltime;

	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS || cltime != 15000)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_3 = { &preamble_2_1, &test_case_2_29_3, &postamble_2 };

#define test_case_2_29_3_stream_0 (&test_2_29_3)
#define test_case_2_29_3_stream_1 (NULL)
#define test_case_2_29_3_stream_2 (NULL)

#define tgrp_case_2_29_4 test_group_2
#define numb_case_2_29_4 "2.29.4"
#define name_case_2_29_4 "Perform streamio I_GETCLTIME - default."
#define sref_case_2_29_4 sref_case_2_29
#define desc_case_2_29_4 "\
Checks that I_GETCLTIME can be performed on a read errored Stream.\n\
Checks that the default close time is 15000 milliseconds (or 15 seconds)."

int
test_case_2_29_4(int child)
{
	int cltime;

	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS || cltime != 15000)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_4 = { &preamble_2_2, &test_case_2_29_4, &postamble_2 };

#define test_case_2_29_4_stream_0 (&test_2_29_4)
#define test_case_2_29_4_stream_1 (NULL)
#define test_case_2_29_4_stream_2 (NULL)

#define tgrp_case_2_29_5 test_group_2
#define numb_case_2_29_5 "2.29.5"
#define name_case_2_29_5 "Perform streamio I_GETCLTIME - default."
#define sref_case_2_29_5 sref_case_2_29
#define desc_case_2_29_5 "\
Checks that I_GETCLTIME can be performed on a write errored Stream.\n\
Checks that the default close time is 15000 milliseconds (or 15 seconds)."

int
test_case_2_29_5(int child)
{
	int cltime;

	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS || cltime != 15000)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_5 = { &preamble_2_3, &test_case_2_29_5, &postamble_2 };

#define test_case_2_29_5_stream_0 (&test_2_29_5)
#define test_case_2_29_5_stream_1 (NULL)
#define test_case_2_29_5_stream_2 (NULL)

#define tgrp_case_2_29_6 test_group_2
#define numb_case_2_29_6 "2.29.6"
#define name_case_2_29_6 "Perform streamio I_GETCLTIME - default."
#define sref_case_2_29_6 sref_case_2_29
#define desc_case_2_29_6 "\
Checks that I_GETCLTIME can be performed on an errored Stream.\n\
Checks that the default close time is 15000 milliseconds (or 15 seconds)."

int
test_case_2_29_6(int child)
{
	int cltime;

	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS || cltime != 15000)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_6 = { &preamble_2_4, &test_case_2_29_6, &postamble_2 };

#define test_case_2_29_6_stream_0 (&test_2_29_6)
#define test_case_2_29_6_stream_1 (NULL)
#define test_case_2_29_6_stream_2 (NULL)

#define tgrp_case_2_29_7 test_group_2
#define numb_case_2_29_7 "2.29.7"
#define name_case_2_29_7 "Perform streamio I_GETCLTIME - EINVAL."
#define sref_case_2_29_7 sref_case_2_29
#define desc_case_2_29_7 "\
Checks that I_GETCLTIME can be performed on an errored Stream.  Checks\n\
that EINVAL is returned when the Stream is linked under a Multiplexing\n\
Driver."

int
test_case_2_29_7(int child)
{
	int cltime;

	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_7 = { &preamble_5, &test_case_2_29_7, &postamble_5 };

#define test_case_2_29_7_stream_0 (&test_2_29_7)
#define test_case_2_29_7_stream_1 (NULL)
#define test_case_2_29_7_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_CANPUT
 */
static const char sref_case_2_30[] = "POSIX 1003.1 2003/SUSv3 ioctl(2p) reference page, I_CANPUT section.";

#define tgrp_case_2_30_1 test_group_2
#define numb_case_2_30_1 "2.30.1"
#define name_case_2_30_1 "Perform streamio I_CANPUT."
#define sref_case_2_30_1 sref_case_2_30
#define desc_case_2_30_1 "\
Checks that I_CANPUT can be performed on a Stream for band 0."

int
test_case_2_30_1(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_1 = { &preamble_0, &test_case_2_30_1, &postamble_0 };

#define test_case_2_30_1_stream_0 (&test_2_30_1)
#define test_case_2_30_1_stream_1 (NULL)
#define test_case_2_30_1_stream_2 (NULL)

#define tgrp_case_2_30_2 test_group_2
#define numb_case_2_30_2 "2.30.2"
#define name_case_2_30_2 "Perform streamio I_CANPUT."
#define sref_case_2_30_2 sref_case_2_30
#define desc_case_2_30_2 "\
Checks that I_CANPUT can be performed on a Stream for band 2."

int
test_case_2_30_2(int child)
{
	if (test_ioctl(child, I_CANPUT, 2) != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_2 = { &preamble_0, &test_case_2_30_2, &postamble_0 };

#define test_case_2_30_2_stream_0 (&test_2_30_2)
#define test_case_2_30_2_stream_1 (NULL)
#define test_case_2_30_2_stream_2 (NULL)

#define tgrp_case_2_30_3 test_group_2
#define numb_case_2_30_3 "2.30.3"
#define name_case_2_30_3 "Perform streamio I_CANPUT EINVAL."
#define sref_case_2_30_3 sref_case_2_30
#define desc_case_2_30_3 "\
Checks that I_CANPUT performed on a Stream for an illegal band (256) will result\n\
in an EINVAL error."

int
test_case_2_30_3(int child)
{
	if (test_ioctl(child, I_CANPUT, 256) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_3 = { &preamble_0, &test_case_2_30_3, &postamble_0 };

#define test_case_2_30_3_stream_0 (&test_2_30_2)
#define test_case_2_30_3_stream_1 (NULL)
#define test_case_2_30_3_stream_2 (NULL)

#define tgrp_case_2_30_4 test_group_2
#define numb_case_2_30_4 "2.30.4"
#define name_case_2_30_4 "Perform streamio I_CANPUT ANYBAND false."
#define sref_case_2_30_4 sref_case_2_30
#define desc_case_2_30_4 "\
Checks that I_CANPUT can be performed on a Stream for the special band ANYBAND.\n\
Because there is not any writable non-zero band (no non-zero band exists on a \n\
newly opened Stream), the return value should be zero (0)."

int
test_case_2_30_4(int child)
{
	if (test_ioctl(child, I_CANPUT, (-1)) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_4 = { &preamble_0, &test_case_2_30_4, &postamble_0 };

#define test_case_2_30_4_stream_0 (&test_2_30_4)
#define test_case_2_30_4_stream_1 (NULL)
#define test_case_2_30_4_stream_2 (NULL)

#define tgrp_case_2_30_5 test_group_2
#define numb_case_2_30_5 "2.30.5"
#define name_case_2_30_5 "Perform streamio I_CANPUT - ENXIO."
#define sref_case_2_30_5 sref_case_2_30
#define desc_case_2_30_5 "\
Checks that I_CANPUT can be performed on a hung up Stream for band 0.\n\
This test should return ENXIO."

int
test_case_2_30_5(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_5 = { &preamble_2_1, &test_case_2_30_5, &postamble_2 };

#define test_case_2_30_5_stream_0 (&test_2_30_5)
#define test_case_2_30_5_stream_1 (NULL)
#define test_case_2_30_5_stream_2 (NULL)

#define tgrp_case_2_30_6 test_group_2
#define numb_case_2_30_6 "2.30.6"
#define name_case_2_30_6 "Perform streamio I_CANPUT."
#define sref_case_2_30_6 sref_case_2_30
#define desc_case_2_30_6 "\
Checks that I_CANPUT can be performed on a read errored Stream for band 0."

int
test_case_2_30_6(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_6 = { &preamble_2_2, &test_case_2_30_6, &postamble_2 };

#define test_case_2_30_6_stream_0 (&test_2_30_6)
#define test_case_2_30_6_stream_1 (NULL)
#define test_case_2_30_6_stream_2 (NULL)

#define tgrp_case_2_30_7 test_group_2
#define numb_case_2_30_7 "2.30.7"
#define name_case_2_30_7 "Perform streamio I_CANPUT - EPROTO."
#define sref_case_2_30_7 sref_case_2_30
#define desc_case_2_30_7 "\
Checks that I_CANPUT can be performed on a write errored Stream for band 0.\n\
This test should return EPROTO."

int
test_case_2_30_7(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_7 = { &preamble_2_3, &test_case_2_30_7, &postamble_2 };

#define test_case_2_30_7_stream_0 (&test_2_30_7)
#define test_case_2_30_7_stream_1 (NULL)
#define test_case_2_30_7_stream_2 (NULL)

#define tgrp_case_2_30_8 test_group_2
#define numb_case_2_30_8 "2.30.8"
#define name_case_2_30_8 "Perform streamio I_CANPUT - EPROTO."
#define sref_case_2_30_8 sref_case_2_30
#define desc_case_2_30_8 "\
Checks that I_CANPUT can be performed on an errored Stream for band 0.\n\
This test should return EPROTO."

int
test_case_2_30_8(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_8 = { &preamble_2_4, &test_case_2_30_8, &postamble_2 };

#define test_case_2_30_8_stream_0 (&test_2_30_8)
#define test_case_2_30_8_stream_1 (NULL)
#define test_case_2_30_8_stream_2 (NULL)

#define tgrp_case_2_30_9 test_group_2
#define numb_case_2_30_9 "2.30.9"
#define name_case_2_30_9 "Perform streamio I_CANPUT - EINVAL."
#define sref_case_2_30_9 sref_case_2_30
#define desc_case_2_30_9 "\
Checks that I_CANPUT can be performed for band 0 on a Stream linked\n\
under a Multiplexing Driver.  This test should return EINVAL."

int
test_case_2_30_9(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_9 = { &preamble_5, &test_case_2_30_9, &postamble_5 };

#define test_case_2_30_9_stream_0 (&test_2_30_9)
#define test_case_2_30_9_stream_1 (NULL)
#define test_case_2_30_9_stream_2 (NULL)

#define tgrp_case_2_30_10 test_group_2
#define numb_case_2_30_10 "2.30.10"
#define name_case_2_30_10 "Perform streamio I_CANPUT - false."
#define sref_case_2_30_10 sref_case_2_30
#define desc_case_2_30_10 "\
Checks that I_CANPUT can be performed on a Stream.  Check that I_CANPUT\n\
returns false when the Stream is full of messages on the write side for\n\
band 0."

int
preamble_test_case_2_30_x(int child, int band)
{
	char buf[1024];
	struct strbuf ctl = { -1, sizeof(buf), buf };
	struct strbuf dat = { -1, sizeof(buf), buf };
	int flags = MSG_BAND;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_CANPUT, band) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	last_errno = 0;
	do {
		test_putpmsg(child, &ctl, &dat, band, flags);
		state++;
	} while (last_errno != EAGAIN);
	return (__RESULT_SUCCESS);
}
int
preamble_test_case_2_30_10(int child)
{
	return preamble_test_case_2_30_x(child, 0);
}
int
test_case_2_30_10(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_10 = { &preamble_test_case_2_30_10, &test_case_2_30_10, &postamble_0 };

#define test_case_2_30_10_stream_0 (&test_2_30_10)
#define test_case_2_30_10_stream_1 (NULL)
#define test_case_2_30_10_stream_2 (NULL)

#define tgrp_case_2_30_11 test_group_2
#define numb_case_2_30_11 "2.30.11"
#define name_case_2_30_11 "Perform streamio I_CANPUT - false."
#define sref_case_2_30_11 sref_case_2_30
#define desc_case_2_30_11 "\
Checks that I_CANPUT can be performed on a Stream.  Check that I_CANPUT\n\
returns false when the Stream is full of messages on the write side for\n\
band 1."

int
preamble_test_case_2_30_11(int child)
{
	return preamble_test_case_2_30_x(child, 1);
}
int
test_case_2_30_11(int child)
{
	if (test_ioctl(child, I_CANPUT, 1) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_11 = { &preamble_test_case_2_30_11, &test_case_2_30_11, &postamble_0 };

#define test_case_2_30_11_stream_0 (&test_2_30_11)
#define test_case_2_30_11_stream_1 (NULL)
#define test_case_2_30_11_stream_2 (NULL)

#define tgrp_case_2_30_12 test_group_2
#define numb_case_2_30_12 "2.30.12"
#define name_case_2_30_12 "Perform streamio I_CANPUT - false."
#define sref_case_2_30_12 sref_case_2_30
#define desc_case_2_30_12 "\
Checks that I_CANPUT can be performed on a Stream.  Check that I_CANPUT\n\
returns false when the Stream is full of messages on the write side for\n\
any band."

int
preamble_test_case_2_30_12(int child)
{
	return preamble_test_case_2_30_x(child, 1);
}
int
test_case_2_30_12(int child)
{
	if (test_ioctl(child, I_CANPUT, (-1)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_12 = { &preamble_test_case_2_30_12, &test_case_2_30_12, &postamble_0 };

#define test_case_2_30_12_stream_0 (&test_2_30_12)
#define test_case_2_30_12_stream_1 (NULL)
#define test_case_2_30_12_stream_2 (NULL)

#define tgrp_case_2_30_13 test_group_2
#define numb_case_2_30_13 "2.30.13"
#define name_case_2_30_13 "Perform streamio I_CANPUT - true."
#define sref_case_2_30_13 sref_case_2_30
#define desc_case_2_30_13 "\
Checks that I_CANPUT can be performed on a Stream.  Check that I_CANPUT\n\
returns true when one band in the Stream is full of messages on the\n\
write side but another band is empty."

int
preamble_test_case_2_30_13(int child)
{
	return preamble_test_case_2_30_x(child, 2);
}
int
test_case_2_30_13(int child)
{
	if (test_ioctl(child, I_CANPUT, (-1)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_13 = { &preamble_test_case_2_30_13, &test_case_2_30_13, &postamble_0 };

#define test_case_2_30_13_stream_0 (&test_2_30_13)
#define test_case_2_30_13_stream_1 (NULL)
#define test_case_2_30_13_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_SERROPT	(Solaris)
 */
#define tgrp_case_2_31_1 test_group_2
#define numb_case_2_31_1 "2.31.1"
#define name_case_2_31_1 "Perform streamio I_SERROPT - default."
#define sref_case_2_31_1 sref_none
#define desc_case_2_31_1 "\
Checks that I_SERROPT can be performed on a Stream with error option values\n\
(RERRNORM | WRERRNORM)."

int
test_case_2_31_1(int child)
{
	if (test_ioctl(child, I_SERROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_31_1 = { &preamble_0, &test_case_2_31_1, &postamble_0 };

#define test_case_2_31_1_stream_0 (&test_2_31_1)
#define test_case_2_31_1_stream_1 (NULL)
#define test_case_2_31_1_stream_2 (NULL)

#define tgrp_case_2_31_2 test_group_2
#define numb_case_2_31_2 "2.31.2"
#define name_case_2_31_2 "Perform streamio I_SERROPT - RERRNONPERSIST."
#define sref_case_2_31_2 sref_none
#define desc_case_2_31_2 "\
Checks that I_SERROPT can be performed on a Stream with error options values\n\
(RERRNONPERSIST | WRERRNORM)."

int
test_case_2_31_2(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SERROPT, RERRNONPERSIST) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_2 = { &preamble_0, &test_case_2_31_2, &postamble_0 };

#define test_case_2_31_2_stream_0 (&test_2_31_2)
#define test_case_2_31_2_stream_1 (NULL)
#define test_case_2_31_2_stream_2 (NULL)

#define tgrp_case_2_31_3 test_group_2
#define numb_case_2_31_3 "2.31.3"
#define name_case_2_31_3 "Perform streamio I_SERROPT - WERRNONPERSIST."
#define sref_case_2_31_3 sref_none
#define desc_case_2_31_3 "\
Checks that I_SERROPT can be performed on a Stream with error options values\n\
(RERRNORM | WERRNONPERSIST)."

int
test_case_2_31_3(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SERROPT, WERRNONPERSIST) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_3 = { &preamble_0, &test_case_2_31_3, &postamble_0 };

#define test_case_2_31_3_stream_0 (&test_2_31_3)
#define test_case_2_31_3_stream_1 (NULL)
#define test_case_2_31_3_stream_2 (NULL)

#define tgrp_case_2_31_4 test_group_2
#define numb_case_2_31_4 "2.31.4"
#define name_case_2_31_4 "Perform streamio I_SERROPT - RERRNONPERSIST | WERRNONPERSIST."
#define sref_case_2_31_4 sref_none
#define desc_case_2_31_4 "\
Checks that I_SERROPT can be performed on a Stream with error options values\n\
(RERRNONPERSIST | WERRNONPERSIST)."

int
test_case_2_31_4(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SERROPT, (RERRNONPERSIST | WERRNONPERSIST)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_4 = { &preamble_0, &test_case_2_31_4, &postamble_0 };

#define test_case_2_31_4_stream_0 (&test_2_31_4)
#define test_case_2_31_4_stream_1 (NULL)
#define test_case_2_31_4_stream_2 (NULL)

#define tgrp_case_2_31_5 test_group_2
#define numb_case_2_31_5 "2.31.5"
#define name_case_2_31_5 "Perform streamio I_SERROPT - EINVAL."
#define sref_case_2_31_5 sref_none
#define desc_case_2_31_5 "\
Checks that I_SERROPT can be performed on a Stream with an invalid argument\n\
value, resulting in the return of EINVAL."

int
test_case_2_31_5(int child)
{
	if (test_ioctl(child, I_SERROPT, -1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_31_5 = { &preamble_0, &test_case_2_31_5, &postamble_0 };

#define test_case_2_31_5_stream_0 (&test_2_31_5)
#define test_case_2_31_5_stream_1 (NULL)
#define test_case_2_31_5_stream_2 (NULL)

#define tgrp_case_2_31_6 test_group_2
#define numb_case_2_31_6 "2.31.6"
#define name_case_2_31_6 "Perform streamio I_SERROPT - EINVAL."
#define sref_case_2_31_6 sref_none
#define desc_case_2_31_6 "\
Checks that I_SERROPT can be performed on a Stream with error options values\n\
(RERRNONPERSIST | WERRNONPERSIST).  Check than EINVAL is returned when\n\
the Stream is linked under a Multiplexing Driver."

int
test_case_2_31_6(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	if (test_ioctl(child, I_SERROPT, (RERRNONPERSIST | WERRNONPERSIST)) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_6 = { &preamble_5, &test_case_2_31_6, &postamble_5 };

#define test_case_2_31_6_stream_0 (&test_2_31_6)
#define test_case_2_31_6_stream_1 (NULL)
#define test_case_2_31_6_stream_2 (NULL)

#define tgrp_case_2_31_7 test_group_2
#define numb_case_2_31_7 "2.31.7"
#define name_case_2_31_7 "Perform streamio I_SERROPT - RERRNORM."
#define sref_case_2_31_7 sref_none
#define desc_case_2_31_7 "\
Checks that I_SERROPT can be performed on a Stream.  Checks that read\n\
errors are persistent when set to RERRNORM."

int
test_case_2_31_7(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	char buf[16] = { 0, };

	if (test_ioctl(child, I_SERROPT, (RERRNORM | WERRNONPERSIST)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_RDERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_7 = { &preamble_2, &test_case_2_31_7, &postamble_0 };

#define test_case_2_31_7_stream_0 (&test_2_31_7)
#define test_case_2_31_7_stream_1 (NULL)
#define test_case_2_31_7_stream_2 (NULL)

#define tgrp_case_2_31_8 test_group_2
#define numb_case_2_31_8 "2.31.8"
#define name_case_2_31_8 "Perform streamio I_SERROPT - WERRNORM."
#define sref_case_2_31_8 sref_none
#define desc_case_2_31_8 "\
Checks that I_SERROPT can be performed on a Stream.  Checks that write\n\
errors are persistent when set to WERRNORM."

int
test_case_2_31_8(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	char buf[16] = { 0, };

	if (test_ioctl(child, I_SERROPT, (RERRNONPERSIST | WERRNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_8 = { &preamble_2, &test_case_2_31_8, &postamble_0 };

#define test_case_2_31_8_stream_0 (&test_2_31_8)
#define test_case_2_31_8_stream_1 (NULL)
#define test_case_2_31_8_stream_2 (NULL)

#define tgrp_case_2_31_9 test_group_2
#define numb_case_2_31_9 "2.31.9"
#define name_case_2_31_9 "Perform streamio I_SERROPT - RERRNONPERSIST."
#define sref_case_2_31_9 sref_none
#define desc_case_2_31_9 "\
Checks that I_SERROPT can be performed on a Stream.  Checks that read\n\
errors are non-persistent when set to RERRNONPERSIST."

int
test_case_2_31_9(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	char buf[16] = { 0, };

	if (test_ioctl(child, I_SERROPT, (RERRNONPERSIST | WERRNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_RDERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_9 = { &preamble_2, &test_case_2_31_9, &postamble_0 };

#define test_case_2_31_9_stream_0 (&test_2_31_9)
#define test_case_2_31_9_stream_1 (NULL)
#define test_case_2_31_9_stream_2 (NULL)

#define tgrp_case_2_31_10 test_group_2
#define numb_case_2_31_10 "2.31.10"
#define name_case_2_31_10 "Perform streamio I_SERROPT - WERRNONPERSIST."
#define sref_case_2_31_10 sref_none
#define desc_case_2_31_10 "\
Checks that I_SERROPT can be performed on a Stream.  Checks that write\n\
errors are non-persistent when set to WERRNONPERSIST."

int
test_case_2_31_10(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	char buf[16] = { 0, };

	if (test_ioctl(child, I_SERROPT, (RERRNORM | WERRNONPERSIST)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, TM_IOC_WRERR, (intptr_t) EPROTO) != __RESULT_SUCCESS && last_errno != EIO)
		return __RESULT_FAILURE;
	state++;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_31_10 = { &preamble_2, &test_case_2_31_10, &postamble_0 };

#define test_case_2_31_10_stream_0 (&test_2_31_10)
#define test_case_2_31_10_stream_1 (NULL)
#define test_case_2_31_10_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GERROPT	(Solaris)
 */
#define tgrp_case_2_32_1 test_group_2
#define numb_case_2_32_1 "2.32.1"
#define name_case_2_32_1 "Perform streamio I_GERROPT - default."
#define sref_case_2_32_1 sref_none
#define desc_case_2_32_1 "\
Checks that I_GERROPT can be performed on a Stream to read the Stream default\n\
error options."

int
test_case_2_32_1(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int erropts = -1;

	if (test_ioctl(child, I_GERROPT, (intptr_t) & erropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (erropts != (RERRNORM | WERRNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_32_1 = { &preamble_0, &test_case_2_32_1, &postamble_0 };

#define test_case_2_32_1_stream_0 (&test_2_32_1)
#define test_case_2_32_1_stream_1 (NULL)
#define test_case_2_32_1_stream_2 (NULL)

#define tgrp_case_2_32_2 test_group_2
#define numb_case_2_32_2 "2.32.2"
#define name_case_2_32_2 "Perform streamio I_GERROPT - set to default."
#define sref_case_2_32_2 sref_none
#define desc_case_2_32_2 "\
Checks that I_GERROPT can be performed on a Stream to read the errror options\n\
(RERRNORM | WERRNORM) after they have been set with I_SERROPT."

int
test_case_2_32_2(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int erropts = -1;

	if (test_ioctl(child, I_SERROPT, (RERRNORM | WERRNORM)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GERROPT, (intptr_t) & erropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (erropts != (RERRNORM | WERRNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_32_2 = { &preamble_0, &test_case_2_32_2, &postamble_0 };

#define test_case_2_32_2_stream_0 (&test_2_32_2)
#define test_case_2_32_2_stream_1 (NULL)
#define test_case_2_32_2_stream_2 (NULL)

#define tgrp_case_2_32_3 test_group_2
#define numb_case_2_32_3 "2.32.3"
#define name_case_2_32_3 "Perform streamio I_GERROPT - RERRNONPERSIST."
#define sref_case_2_32_3 sref_none
#define desc_case_2_32_3 "\
Checks that I_GERROPT can be performed on a Stream to read the errror options\n\
(RERRNONPERSIST | WERRNORM) after they have been set with I_SERROPT."

int
test_case_2_32_3(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int erropts = -1;

	if (test_ioctl(child, I_SERROPT, (RERRNONPERSIST | WERRNORM)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GERROPT, (intptr_t) & erropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (erropts != (RERRNONPERSIST | WERRNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_32_3 = { &preamble_0, &test_case_2_32_3, &postamble_0 };

#define test_case_2_32_3_stream_0 (&test_2_32_3)
#define test_case_2_32_3_stream_1 (NULL)
#define test_case_2_32_3_stream_2 (NULL)

#define tgrp_case_2_32_4 test_group_2
#define numb_case_2_32_4 "2.32.4"
#define name_case_2_32_4 "Perform streamio I_GERROPT - WERRNONPERSIST."
#define sref_case_2_32_4 sref_none
#define desc_case_2_32_4 "\
Checks that I_GERROPT can be performed on a Stream to read the errror options\n\
(RERRNORM | WERRNONPERSIST) after they have been set with I_SERROPT."

int
test_case_2_32_4(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int erropts = -1;

	if (test_ioctl(child, I_SERROPT, (RERRNORM | WERRNONPERSIST)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GERROPT, (intptr_t) & erropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (erropts != (RERRNORM | WERRNONPERSIST))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_32_4 = { &preamble_0, &test_case_2_32_4, &postamble_0 };

#define test_case_2_32_4_stream_0 (&test_2_32_4)
#define test_case_2_32_4_stream_1 (NULL)
#define test_case_2_32_4_stream_2 (NULL)

#define tgrp_case_2_32_5 test_group_2
#define numb_case_2_32_5 "2.32.5"
#define name_case_2_32_5 "Perform streamio I_GERROPT - RERRNONPERSIST | WERRNONPERSIST."
#define sref_case_2_32_5 sref_none
#define desc_case_2_32_5 "\
Checks that I_GERROPT can be performed on a Stream to read the errror options\n\
(RERRNONPERSIST | WERRNONPERSIST) after they have been set with I_SERROPT."

int
test_case_2_32_5(int child)
{
#ifdef LIS
	return (__RESULT_FAILURE);
#else
	int erropts = -1;

	if (test_ioctl(child, I_SERROPT, (RERRNONPERSIST | WERRNONPERSIST)) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_ioctl(child, I_GERROPT, (intptr_t) & erropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (erropts != (RERRNONPERSIST | WERRNONPERSIST))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_32_5 = { &preamble_0, &test_case_2_32_5, &postamble_0 };

#define test_case_2_32_5_stream_0 (&test_2_32_5)
#define test_case_2_32_5_stream_1 (NULL)
#define test_case_2_32_5_stream_2 (NULL)

#define tgrp_case_2_32_6 test_group_2
#define numb_case_2_32_6 "2.32.6"
#define name_case_2_32_6 "Perform streamio I_GERROPT - EFAULT."
#define sref_case_2_32_6 sref_none
#define desc_case_2_32_6 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_32_6(int child)
{
	if (test_ioctl(child, I_GERROPT, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_32_6 = { &preamble_0, &test_case_2_32_6, &postamble_0 };

#define test_case_2_32_6_stream_0 (&test_2_32_6)
#define test_case_2_32_6_stream_1 (NULL)
#define test_case_2_32_6_stream_2 (NULL)

#define tgrp_case_2_32_7 test_group_2
#define numb_case_2_32_7 "2.32.7"
#define name_case_2_32_7 "Perform streamio I_GERROPT - EINVAL."
#define sref_case_2_32_7 sref_none
#define desc_case_2_32_7 "\
Check that I_GERROPT can be performed on a Stream.  Check the EINVAL is\n\
returned when the Stream is linked under a Multiplexing Driver."

int
test_case_2_32_7(int child)
{
	int erropts = -1;

	if (test_ioctl(child, I_GERROPT, (intptr_t) &erropts) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_32_7 = { &preamble_5, &test_case_2_32_7, &postamble_5 };

#define test_case_2_32_7_stream_0 (&test_2_32_7)
#define test_case_2_32_7_stream_1 (NULL)
#define test_case_2_32_7_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_ANCHOR	(Solaris)
 */
#define tgrp_case_2_33 test_group_2
#define numb_case_2_33 "2.33"
#define name_case_2_33 "Perform streamio I_ANCHOR."
#define sref_case_2_33 sref_none
#define desc_case_2_33 "\
Checks that I_ANCHOR can be performed on a Stream."

int
test_case_2_33(int child)
{
	if (test_ioctl(child, I_ANCHOR, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_33 = { &preamble_0, &test_case_2_33, &postamble_0 };

#define test_case_2_33_stream_0 (&test_2_33)
#define test_case_2_33_stream_1 (NULL)
#define test_case_2_33_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_S_RECVFD	(UnixWare)
 */
#define tgrp_case_2_34 test_group_2
#define numb_case_2_34 "2.34"
#define name_case_2_34 "Perform streamio I_S_RECVFD."
#define sref_case_2_34 sref_none
#define desc_case_2_34 "\
Checks that I_S_RECVFD can be performed on a Stream."

int
test_case_2_34(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_34 = { &preamble_0, &test_case_2_34, &postamble_0 };

#define test_case_2_34_stream_0 (&test_2_34)
#define test_case_2_34_stream_1 (NULL)
#define test_case_2_34_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_STATS	(UnixWare)
 */
#define tgrp_case_2_35 test_group_2
#define numb_case_2_35 "2.35"
#define name_case_2_35 "Perform streamio I_STATS."
#define sref_case_2_35 sref_none
#define desc_case_2_35 "\
Checks that I_STATS can be performed on a Stream."

int
test_case_2_35(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_35 = { &preamble_0, &test_case_2_35, &postamble_0 };

#define test_case_2_35_stream_0 (&test_2_35)
#define test_case_2_35_stream_1 (NULL)
#define test_case_2_35_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_BIGPIPE	(UnixWare)
 */
#define tgrp_case_2_36 test_group_2
#define numb_case_2_36 "2.36"
#define name_case_2_36 "Perform streamio I_BIGPIPE."
#define sref_case_2_36 sref_none
#define desc_case_2_36 "\
Checks that I_BIGPIPE can be performed on a Stream."

int
test_case_2_36(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_36 = { &preamble_0, &test_case_2_36, &postamble_0 };

#define test_case_2_36_stream_0 (&test_2_36)
#define test_case_2_36_stream_1 (NULL)
#define test_case_2_36_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GETTP	(UnixWare)
 */
#define tgrp_case_2_37 test_group_2
#define numb_case_2_37 "2.37"
#define name_case_2_37 "Perform streamio I_GETTP."
#define sref_case_2_37 sref_none
#define desc_case_2_37 "\
Checks that I_GETTP can be performed on a Stream."

int
test_case_2_37(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_37 = { &preamble_0, &test_case_2_37, &postamble_0 };

#define test_case_2_37_stream_0 (&test_2_37)
#define test_case_2_37_stream_1 (NULL)
#define test_case_2_37_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GETMSG  (HP-UX, OSF)
 */
#define tgrp_case_2_38 test_group_2
#define numb_case_2_38 "2.38"
#define name_case_2_38 "Perform streamio I_GETMSG."
#define sref_case_2_38 sref_none
#define desc_case_2_38 "\
Checks that I_GETMSG can be performed on a Stream."

int
test_case_2_38(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_38 = { &preamble_0, &test_case_2_38, &postamble_0 };

#define test_case_2_38_stream_0 (&test_2_38)
#define test_case_2_38_stream_1 (NULL)
#define test_case_2_38_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_PUTMSG  (HP-UX, OSF)
 */
#define tgrp_case_2_39 test_group_2
#define numb_case_2_39 "2.39"
#define name_case_2_39 "Perform streamio I_PUTMSG."
#define sref_case_2_39 sref_none
#define desc_case_2_39 "\
Checks that I_PUTMSG can be performed on a Stream."

int
test_case_2_39(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_39 = { &preamble_0, &test_case_2_39, &postamble_0 };

#define test_case_2_39_stream_0 (&test_2_39)
#define test_case_2_39_stream_1 (NULL)
#define test_case_2_39_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_GETPMSG  (LfS)
 */
static const char sref_case_2_40[] = "POSIX 1003.1 2003/SuSv3 getpmsg(2p) reference page.";

#define tgrp_case_2_40_1 test_group_2
#define numb_case_2_40_1 "2.40.1"
#define name_case_2_40_1 "Perform streamio I_GETPMSG."
#define sref_case_2_40_1 sref_case_2_40
#define desc_case_2_40_1 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
zero is returned for both the control part size and the data part size,\n\
indicating a hung up Stream, when I_GETPMSG is called on a hung up Stream."

int
test_case_2_40_1(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 0;
	sg.flags = MSG_ANY;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (sg.ctlbuf.len != 0 || sg.databuf.len != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_1 = { &preamble_2_1, &test_case_2_40_1, &postamble_2 };

#define test_case_2_40_1_stream_0 (&test_2_40_1)
#define test_case_2_40_1_stream_1 (NULL)
#define test_case_2_40_1_stream_2 (NULL)

#define tgrp_case_2_40_2 test_group_2
#define numb_case_2_40_2 "2.40.2"
#define name_case_2_40_2 "Perform streamio I_GETPMSG - EPROTO."
#define sref_case_2_40_2 sref_case_2_40
#define desc_case_2_40_2 "\
Checks that I_GETPMSG can be performed on a read errored Stream.  Checks\n\
that EPROTO is returned when I_GETPMSG is attempted on a Stream that has\n\
received a read error of EPROTO."

int
test_case_2_40_2(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 0;
	sg.flags = MSG_ANY;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_2 = { &preamble_2_2, &test_case_2_40_2, &postamble_2 };

#define test_case_2_40_2_stream_0 (&test_2_40_2)
#define test_case_2_40_2_stream_1 (NULL)
#define test_case_2_40_2_stream_2 (NULL)

#define tgrp_case_2_40_3 test_group_2
#define numb_case_2_40_3 "2.40.3"
#define name_case_2_40_3 "Perform streamio I_GETPMSG."
#define sref_case_2_40_3 sref_case_2_40
#define desc_case_2_40_3 "\
Checks that I_GETPMSG can be performed on a write errored Stream.  Checks\n\
that I_GETPMSG is successful on a Stream that has received a write\n\
error."

int
test_case_2_40_3(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 0;
	sg.flags = MSG_ANY;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_3 = { &preamble_2_3, &test_case_2_40_3, &postamble_2 };

#define test_case_2_40_3_stream_0 (&test_2_40_3)
#define test_case_2_40_3_stream_1 (NULL)
#define test_case_2_40_3_stream_2 (NULL)

#define tgrp_case_2_40_4 test_group_2
#define numb_case_2_40_4 "2.40.4"
#define name_case_2_40_4 "Perform streamio I_GETPMSG - EPROTO."
#define sref_case_2_40_4 sref_case_2_40
#define desc_case_2_40_4 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
EPROTO is returned when I_GETPMSG is attempted on a Stream that has\n\
received an error of EPROTO."

int
test_case_2_40_4(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 0;
	sg.flags = MSG_ANY;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_4 = { &preamble_2_4, &test_case_2_40_4, &postamble_2 };

#define test_case_2_40_4_stream_0 (&test_2_40_4)
#define test_case_2_40_4_stream_1 (NULL)
#define test_case_2_40_4_stream_2 (NULL)

#define tgrp_case_2_40_5 test_group_2
#define numb_case_2_40_5 "2.40.5"
#define name_case_2_40_5 "Perform streamio I_GETPMSG - EPROTO."
#define sref_case_2_40_5 sref_case_2_40
#define desc_case_2_40_5 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
EAGAIN is returned when I_GETPMSG with MSG_HIPRI is attempted on a\n\
Stream that has no data and the Stream is set for non-blocking\n\
operation."

int
test_case_2_40_5(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 0;
	sg.flags = MSG_HIPRI;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_5 = { &preamble_0, &test_case_2_40_5, &postamble_0 };

#define test_case_2_40_5_stream_0 (&test_2_40_5)
#define test_case_2_40_5_stream_1 (NULL)
#define test_case_2_40_5_stream_2 (NULL)

#define tgrp_case_2_40_6 test_group_2
#define numb_case_2_40_6 "2.40.6"
#define name_case_2_40_6 "Perform streamio I_GETPMSG - EPROTO."
#define sref_case_2_40_6 sref_case_2_40
#define desc_case_2_40_6 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
EAGAIN is returned when I_GETPMSG with MSG_BAND is attempted on a\n\
Stream that has no data and the Stream is set for non-blocking\n\
operation."

int
test_case_2_40_6(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 1;
	sg.flags = MSG_BAND;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_6 = { &preamble_0, &test_case_2_40_6, &postamble_0 };

#define test_case_2_40_6_stream_0 (&test_2_40_6)
#define test_case_2_40_6_stream_1 (NULL)
#define test_case_2_40_6_stream_2 (NULL)

#define tgrp_case_2_40_7 test_group_2
#define numb_case_2_40_7 "2.40.7"
#define name_case_2_40_7 "Perform streamio I_GETPMSG - EPROTO."
#define sref_case_2_40_7 sref_case_2_40
#define desc_case_2_40_7 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
EAGAIN is returned when I_GETPMSG with MSG_ANY is attempted on a\n\
Stream that has no data and the Stream is set for non-blocking\n\
operation."

int
test_case_2_40_7(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 0;
	sg.flags = MSG_ANY;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_7 = { &preamble_0, &test_case_2_40_7, &postamble_0 };

#define test_case_2_40_7_stream_0 (&test_2_40_7)
#define test_case_2_40_7_stream_1 (NULL)
#define test_case_2_40_7_stream_2 (NULL)

#define tgrp_case_2_40_8 test_group_2
#define numb_case_2_40_8 "2.40.8"
#define name_case_2_40_8 "Perform streamio I_GETPMSG - EINVAL."
#define sref_case_2_40_8 sref_case_2_40
#define desc_case_2_40_8 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
EINVAL is returned when the Stream is linked under a Multiplexing\n\
Driver."

int
test_case_2_40_8(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = 0;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = buf;

	sg.databuf.maxlen = 0;
	sg.databuf.len = -1;
	sg.databuf.buf = buf;

	sg.band = 0;
	sg.flags = MSG_ANY;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_8 = { &preamble_5, &test_case_2_40_8, &postamble_5 };

#define test_case_2_40_8_stream_0 (&test_2_40_8)
#define test_case_2_40_8_stream_1 (NULL)
#define test_case_2_40_8_stream_2 (NULL)

#define tgrp_case_2_40_9 test_group_2
#define numb_case_2_40_9 "2.40.9"
#define name_case_2_40_9 "Perform streamio I_GETPMSG - EFAULT."
#define sref_case_2_40_9 sref_case_2_40
#define desc_case_2_40_9 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
EFAULT is returned when arg extends outside the callers valid address\n\
space."

int
test_case_2_40_9(int child)
{
	if (test_ioctl(child, I_GETPMSG, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_9 = { &preamble_5, &test_case_2_40_9, &postamble_5 };

#define test_case_2_40_9_stream_0 (&test_2_40_9)
#define test_case_2_40_9_stream_1 (NULL)
#define test_case_2_40_9_stream_2 (NULL)

#define tgrp_case_2_40_10 test_group_2
#define numb_case_2_40_10 "2.40.10"
#define name_case_2_40_10 "Perform streamio I_GETPMSG - EFAULT."
#define sref_case_2_40_10 sref_case_2_40
#define desc_case_2_40_10 "\
Checks that I_GETPMSG can be performed on a hung up Stream.  Checks that\n\
EFAULT is returned when a buffer extends outside the callers valid\n\
address space."

int
preamble_test_case_2_40_10(int child)
{
	char buf[] = "Test message.";

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_2_40_10(int child)
{
	struct strpmsg sg;

	sg.ctlbuf.maxlen = 5;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = (char *) INVALID_ADDRESS;

	sg.databuf.maxlen = 5;
	sg.databuf.len = -1;
	sg.databuf.buf = (char *) INVALID_ADDRESS;

	sg.band = 0;
	sg.flags = MSG_ANY;

	if (test_ioctl(child, I_GETPMSG, (intptr_t) &sg) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_40_10 = { &preamble_test_case_2_40_10, &test_case_2_40_10, &postamble_0 };

#define test_case_2_40_10_stream_0 (&test_2_40_10)
#define test_case_2_40_10_stream_1 (NULL)
#define test_case_2_40_10_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_PUTPMSG  (LfS)
 */
static const char sref_case_2_41[] = "POSIX 1003.1 2003/SuSv3 putpmsg(2p) reference page.";

#define tgrp_case_2_41_1 test_group_2
#define numb_case_2_41_1 "2.41.1"
#define name_case_2_41_1 "Perform streamio I_PUTPMSG - ENXIO."
#define sref_case_2_41_1 sref_case_2_41
#define desc_case_2_41_1 "\
Checks that I_PUTPMSG can be performed on a Stream.  Checks that ENXIO is\n\
returned when the Stream as received a hangup."

int
test_case_2_41_1(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = -1;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = NULL;

	sg.databuf.maxlen = -1;
	sg.databuf.len = 0;
	sg.databuf.buf = buf;

	sg.band = -1;
	sg.flags = 0;

	if (test_ioctl(child, I_PUTPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_41_1 = { &preamble_2_1, &test_case_2_41_1, &postamble_2 };

#define test_case_2_41_1_stream_0 (&test_2_41_1)
#define test_case_2_41_1_stream_1 (NULL)
#define test_case_2_41_1_stream_2 (NULL)

#define tgrp_case_2_41_2 test_group_2
#define numb_case_2_41_2 "2.41.2"
#define name_case_2_41_2 "Perform streamio I_PUTPMSG."
#define sref_case_2_41_2 sref_case_2_41
#define desc_case_2_41_2 "\
Checks that I_PUTPMSG can be performed on a Stream.  Check that I_PUTPMSG\n\
can be performed regardless of a received read error."

int
test_case_2_41_2(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = -1;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = NULL;

	sg.databuf.maxlen = -1;
	sg.databuf.len = 0;
	sg.databuf.buf = buf;

	sg.band = -1;
	sg.flags = 0;

	if (test_ioctl(child, I_PUTPMSG, (intptr_t) &sg) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_41_2 = { &preamble_2_2, &test_case_2_41_2, &postamble_2 };

#define test_case_2_41_2_stream_0 (&test_2_41_2)
#define test_case_2_41_2_stream_1 (NULL)
#define test_case_2_41_2_stream_2 (NULL)

#define tgrp_case_2_41_3 test_group_2
#define numb_case_2_41_3 "2.41.3"
#define name_case_2_41_3 "Perform streamio I_PUTPMSG - EPROTO."
#define sref_case_2_41_3 sref_case_2_41
#define desc_case_2_41_3 "\
Checks that I_PUTPMSG can be performed on a Stream.  Check the EPROTO is\n\
returned when the Stream has received a write error of EPROTO."

int
test_case_2_41_3(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = -1;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = NULL;

	sg.databuf.maxlen = -1;
	sg.databuf.len = 0;
	sg.databuf.buf = buf;

	sg.band = -1;
	sg.flags = 0;

	if (test_ioctl(child, I_PUTPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_41_3 = { &preamble_2_3, &test_case_2_41_3, &postamble_2 };

#define test_case_2_41_3_stream_0 (&test_2_41_3)
#define test_case_2_41_3_stream_1 (NULL)
#define test_case_2_41_3_stream_2 (NULL)

#define tgrp_case_2_41_4 test_group_2
#define numb_case_2_41_4 "2.41.4"
#define name_case_2_41_4 "Perform streamio I_PUTPMSG - EPROTO."
#define sref_case_2_41_4 sref_case_2_41
#define desc_case_2_41_4 "\
Checks that I_PUTPMSG can be performed on a Stream.  Check the EPROTO is\n\
returned when the Stream has received an error of EPROTO."

int
test_case_2_41_4(int child)
{
	struct strpmsg sg;
	char buf[1];

	sg.ctlbuf.maxlen = -1;
	sg.ctlbuf.len = -1;
	sg.ctlbuf.buf = NULL;

	sg.databuf.maxlen = -1;
	sg.databuf.len = 0;
	sg.databuf.buf = buf;

	sg.band = -1;
	sg.flags = 0;

	if (test_ioctl(child, I_PUTPMSG, (intptr_t) &sg) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_41_4 = { &preamble_2_4, &test_case_2_41_4, &postamble_2 };

#define test_case_2_41_4_stream_0 (&test_2_41_4)
#define test_case_2_41_4_stream_1 (NULL)
#define test_case_2_41_4_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_PIPE  (LfS)
 */
#define tgrp_case_2_42 test_group_2
#define numb_case_2_42 "2.42"
#define name_case_2_42 "Perform streamio I_PIPE."
#define sref_case_2_42 sref_none
#define desc_case_2_42 "\
Checks that I_PIPE can be performed on a Stream."

int
test_case_2_42(int child)
{
	return (__RESULT_SKIPPED);
}
struct test_stream test_2_42 = { &preamble_0, &test_case_2_42, &postamble_0 };

#define test_case_2_42_stream_0 (&test_2_42)
#define test_case_2_42_stream_1 (NULL)
#define test_case_2_42_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_FIFO  (LfS)
 */
#define tgrp_case_2_43 test_group_2
#define numb_case_2_43 "2.43"
#define name_case_2_43 "Perform streamio I_FIFO."
#define sref_case_2_43 sref_none
#define desc_case_2_43 "\
Checks that I_FIFO can be performed on a Stream."

int
test_case_2_43(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_43 = { &preamble_0, &test_case_2_43, &postamble_0 };

#define test_case_2_43_stream_0 (&test_2_43)
#define test_case_2_43_stream_1 (NULL)
#define test_case_2_43_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_AUTOPUSH	(MacOT)
 */
#define tgrp_case_2_44 test_group_2
#define numb_case_2_44 "2.44"
#define name_case_2_44 "Perform streamio I_AUTOPUSH."
#define sref_case_2_44 sref_none
#define desc_case_2_44 "\
Checks that I_AUTOPUSH can be performed on a Stream."

int
test_case_2_44(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_44 = { &preamble_0, &test_case_2_44, &postamble_0 };

#define test_case_2_44_stream_0 (&test_2_44)
#define test_case_2_44_stream_1 (NULL)
#define test_case_2_44_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_HEAP_REPORT	(MacOT)
 */
#define tgrp_case_2_45 test_group_2
#define numb_case_2_45 "2.45"
#define name_case_2_45 "Perform streamio I_HEAP_REPORT."
#define sref_case_2_45 sref_none
#define desc_case_2_45 "\
Checks that I_HEAP_REPORT can be performed on a Stream."

int
test_case_2_45(int child)
{
	return (__RESULT_NOTAPPL);
}
struct test_stream test_2_45 = { &preamble_0, &test_case_2_45, &postamble_0 };

#define test_case_2_45_stream_0 (&test_2_45)
#define test_case_2_45_stream_1 (NULL)
#define test_case_2_45_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_FATTACH	(LfS)
 */
#define tgrp_case_2_46 test_group_2
#define numb_case_2_46 "2.46"
#define name_case_2_46 "Perform streamio I_FATTACH."
#define sref_case_2_46 sref_none
#define desc_case_2_46 "\
Checks that I_FATTACH can be performed on a Stream."

int
test_case_2_46(int child)
{
	return (__RESULT_SKIPPED);
}
struct test_stream test_2_46 = { &preamble_0, &test_case_2_46, &postamble_0 };

#define test_case_2_46_stream_0 (&test_2_46)
#define test_case_2_46_stream_1 (NULL)
#define test_case_2_46_stream_2 (NULL)

/*
 *  Perform IOCTL on one Stream - I_FDETACH	(LfS)
 */
#define tgrp_case_2_47 test_group_2
#define numb_case_2_47 "2.47"
#define name_case_2_47 "Perform streamio I_FDETACH."
#define sref_case_2_47 sref_none
#define desc_case_2_47 "\
Checks that I_FDETACH can be performed on a Stream."

int
test_case_2_47(int child)
{
	return (__RESULT_SKIPPED);
}
struct test_stream test_2_47 = { &preamble_0, &test_case_2_47, &postamble_0 };

#define test_case_2_47_stream_0 (&test_2_47)
#define test_case_2_47_stream_1 (NULL)
#define test_case_2_47_stream_2 (NULL)

#define tgrp_case_2_48_1 test_group_2
#define numb_case_2_48_1 "2.48.1"
#define name_case_2_48_1 "Perform streamio TRANSPARENT."
#define sref_case_2_48_1 sref_none
#define desc_case_2_48_1 "\
Checks that a TRANSPARENT ioctl can be performed on a Stream.  Check\n\
that a TRANSPARENT ioctl that that copies data in is successful."

int
test_case_2_48_1(int child)
{
	char buf[1024] = { 0, };

	if (test_ioctl(child, TM_IOC_COPYIN, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_48_1 = { &preamble_2, &test_case_2_48_1, &postamble_2 };

#define test_case_2_48_1_stream_0 (&test_2_48_1)
#define test_case_2_48_1_stream_1 (NULL)
#define test_case_2_48_1_stream_2 (NULL)

#define tgrp_case_2_48_2 test_group_2
#define numb_case_2_48_2 "2.48.2"
#define name_case_2_48_2 "Perform streamio TRANSPARENT."
#define sref_case_2_48_2 sref_none
#define desc_case_2_48_2 "\
Checks that a TRANSPARENT ioctl can be performed on a Stream.  Check\n\
that a TRANSPARENT ioctl that that copies data out is successful."

int
test_case_2_48_2(int child)
{
	char buf[1024] = { 0, };
	int i;

	if (test_ioctl(child, TM_IOC_COPYOUT, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 64; i++)
		if ((unsigned char) buf[i] != (unsigned char) 0xa5)
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_48_2 = { &preamble_2, &test_case_2_48_2, &postamble_2 };

#define test_case_2_48_2_stream_0 (&test_2_48_2)
#define test_case_2_48_2_stream_1 (NULL)
#define test_case_2_48_2_stream_2 (NULL)

#define tgrp_case_2_48_3 test_group_2
#define numb_case_2_48_3 "2.48.3"
#define name_case_2_48_3 "Perform streamio TRANSPARENT."
#define sref_case_2_48_3 sref_none
#define desc_case_2_48_3 "\
Checks that a TRANSPARENT ioctl can be performed on a Stream.  Check\n\
that a TRANSPARENT ioctl that that copies data in and out is successful."

int
test_case_2_48_3(int child)
{
	char buf[1024] = { 0, };
	int i;

	if (test_ioctl(child, TM_IOC_COPYIO, (intptr_t) buf) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 64; i++)
		if ((unsigned char) buf[i] != (unsigned char) 0xa5)
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_48_3 = { &preamble_2, &test_case_2_48_3, &postamble_2 };

#define test_case_2_48_3_stream_0 (&test_2_48_3)
#define test_case_2_48_3_stream_1 (NULL)
#define test_case_2_48_3_stream_2 (NULL)

#define tgrp_case_2_48_4 test_group_2
#define numb_case_2_48_4 "2.48.4"
#define name_case_2_48_4 "Perform streamio TRANSPARENT - EFAULT."
#define sref_case_2_48_4 sref_none
#define desc_case_2_48_4 "\
Checks that a TRANSPARENT ioctl can be performed on a Stream.  Check\n\
that EFAULT is returned when the user address is invalid on a copyin\n\
operation."

int
test_case_2_48_4(int child)
{
	if (test_ioctl(child, TM_IOC_COPYIN, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_48_4 = { &preamble_2, &test_case_2_48_4, &postamble_2 };

#define test_case_2_48_4_stream_0 (&test_2_48_4)
#define test_case_2_48_4_stream_1 (NULL)
#define test_case_2_48_4_stream_2 (NULL)

#define tgrp_case_2_48_5 test_group_2
#define numb_case_2_48_5 "2.48.5"
#define name_case_2_48_5 "Perform streamio TRANSPARENT - EFAULT."
#define sref_case_2_48_5 sref_none
#define desc_case_2_48_5 "\
Checks that a TRANSPARENT ioctl can be performed on a Stream.  Check\n\
that EFAULT is returned when the user address is invalid on a copyout\n\
operation."

int
test_case_2_48_5(int child)
{
	if (test_ioctl(child, TM_IOC_COPYOUT, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_48_5 = { &preamble_2, &test_case_2_48_5, &postamble_2 };

#define test_case_2_48_5_stream_0 (&test_2_48_5)
#define test_case_2_48_5_stream_1 (NULL)
#define test_case_2_48_5_stream_2 (NULL)

#define tgrp_case_2_48_6 test_group_2
#define numb_case_2_48_6 "2.48.6"
#define name_case_2_48_6 "Perform streamio TRANSPARENT - EFAULT."
#define sref_case_2_48_6 sref_none
#define desc_case_2_48_6 "\
Checks that a TRANSPARENT ioctl can be performed on a Stream.  Check\n\
that EFAULT is returned when the user address is invalid on a copyin\n\
and copyout operation."

int
test_case_2_48_6(int child)
{
	if (test_ioctl(child, TM_IOC_COPYIO, (intptr_t) INVALID_ADDRESS) == __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_48_6 = { &preamble_2, &test_case_2_48_6, &postamble_2 };

#define test_case_2_48_6_stream_0 (&test_2_48_6)
#define test_case_2_48_6_stream_1 (NULL)
#define test_case_2_48_6_stream_2 (NULL)

static const char test_group_3[] = "Perform information transfer on one Stream";

static const char test_group_3_1[] = "Perform READ on one Stream";
static const char sref_case_3_1[] = "POSIX 1003.1 2003/SUSv3 read(2p) reference page.";

#define tgrp_case_3_1_1 test_group_3_1
#define numb_case_3_1_1 "3.1.1"
#define name_case_3_1_1 "Perform read - EOF."
#define sref_case_3_1_1 sref_case_3_1
#define desc_case_3_1_1 "\
Check that read() can be performed on a Stream.  Checks that zero (0) is\n\
returned when read() is attempted on a Stream that has received a hang\n\
up."

int
test_case_3_1_1(int child)
{
	char buf[16];

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_1 = { &preamble_2_1, &test_case_3_1_1, &postamble_2 };

#define test_case_3_1_1_stream_0 (&test_3_1_1)
#define test_case_3_1_1_stream_1 (NULL)
#define test_case_3_1_1_stream_2 (NULL)

#define tgrp_case_3_1_2 test_group_3_1
#define numb_case_3_1_2 "3.1.2"
#define name_case_3_1_2 "Perform read - EPROTO."
#define sref_case_3_1_2 sref_case_3_1
#define desc_case_3_1_2 "\
Check that read() can be performed on a Stream.  Checks that EPROTO is\n\
returned when read() is attempted on a Stream that has received an\n\
asynchronous EPROTO read error."

int
test_case_3_1_2(int child)
{
	char buf[16];

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_2 = { &preamble_2_2, &test_case_3_1_2, &postamble_2 };

#define test_case_3_1_2_stream_0 (&test_3_1_2)
#define test_case_3_1_2_stream_1 (NULL)
#define test_case_3_1_2_stream_2 (NULL)

#define tgrp_case_3_1_3 test_group_3_1
#define numb_case_3_1_3 "3.1.3"
#define name_case_3_1_3 "Perform read - EAGAIN."
#define sref_case_3_1_3 sref_case_3_1
#define desc_case_3_1_3 "\
Check that read() can be performed on a Stream.  Checks that EAGAIN is\n\
returned when read() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO write error (and non-blocking operation is set)."

int
test_case_3_1_3(int child)
{
	char buf[16];

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_3 = { &preamble_2_3, &test_case_3_1_3, &postamble_2 };

#define test_case_3_1_3_stream_0 (&test_3_1_3)
#define test_case_3_1_3_stream_1 (NULL)
#define test_case_3_1_3_stream_2 (NULL)

#define tgrp_case_3_1_4 test_group_3_1
#define numb_case_3_1_4 "3.1.4"
#define name_case_3_1_4 "Perform read - EPROTO."
#define sref_case_3_1_4 sref_case_3_1
#define desc_case_3_1_4 "\
Check that read() can be performed on a Stream.  Checks that EPROTO is\n\
returned when read() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO error."

int
test_case_3_1_4(int child)
{
	char buf[16];

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_4 = { &preamble_2_4, &test_case_3_1_4, &postamble_2 };

#define test_case_3_1_4_stream_0 (&test_3_1_4)
#define test_case_3_1_4_stream_1 (NULL)
#define test_case_3_1_4_stream_2 (NULL)

#define tgrp_case_3_1_5 test_group_3_1
#define numb_case_3_1_5 "3.1.5"
#define name_case_3_1_5 "Perform read - EINVAL."
#define sref_case_3_1_5 sref_case_3_1
#define desc_case_3_1_5 "\
Check that read() can be performed on a Stream.  Checks that EINVAL is\n\
returned when read() is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_3_1_5(int child)
{
	char buf[16];

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_5 = { &preamble_5, &test_case_3_1_5, &postamble_5 };

#define test_case_3_1_5_stream_0 (&test_3_1_5)
#define test_case_3_1_5_stream_1 (NULL)
#define test_case_3_1_5_stream_2 (NULL)

#define tgrp_case_3_1_6 test_group_3_1
#define numb_case_3_1_6 "3.1.6"
#define name_case_3_1_6 "Perform read - EAGAIN."
#define sref_case_3_1_6 sref_case_3_1
#define desc_case_3_1_6 "\
Check that read() can be performed on a Stream.  The O_NONBLOCK flag is\n\
set for the file descriptor and the process would be delayed."

int
test_case_3_1_6(int child)
{
	char buf[16];

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_6 = { &preamble_0, &test_case_3_1_6, &postamble_0 };

#define test_case_3_1_6_stream_0 (&test_3_1_6)
#define test_case_3_1_6_stream_1 (NULL)
#define test_case_3_1_6_stream_2 (NULL)

#define tgrp_case_3_1_7 test_group_3_1
#define numb_case_3_1_7 "3.1.7"
#define name_case_3_1_7 "Perform read - EBADF."
#define sref_case_3_1_7 sref_case_3_1
#define desc_case_3_1_7 "\
Check that read() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not a valid file descriptor."

int
test_case_3_1_7(int child)
{
	char buf[16];
	int oldfd = test_fd[child];

	test_fd[child] = -1;

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	test_fd[child] = oldfd;
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_7 = { &preamble_0, &test_case_3_1_7, &postamble_0 };

#define test_case_3_1_7_stream_0 (&test_3_1_7)
#define test_case_3_1_7_stream_1 (NULL)
#define test_case_3_1_7_stream_2 (NULL)

#define tgrp_case_3_1_8 test_group_3_1
#define numb_case_3_1_8 "3.1.8"
#define name_case_3_1_8 "Perform read - EBADF."
#define sref_case_3_1_8 sref_case_3_1
#define desc_case_3_1_8 "\
Check that read() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not open for reading."

int
test_case_3_1_8(int child)
{
	char buf[16];

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_8 = { &preamble_0_2, &test_case_3_1_8, &postamble_0 };

#define test_case_3_1_8_stream_0 (&test_3_1_8)
#define test_case_3_1_8_stream_1 (NULL)
#define test_case_3_1_8_stream_2 (NULL)

#define tgrp_case_3_1_9 test_group_3_1
#define numb_case_3_1_9 "3.1.9"
#define name_case_3_1_9 "Perform read - EFAULT."
#define sref_case_3_1_9 sref_case_3_1
#define desc_case_3_1_9 "\
Check that read() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the buffer extends outside the callers valid address space."

int
preamble_test_case_3_1_9(int child)
{
	char buf[16];

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_3_1_9(int child)
{
	char buf[16];

	if (test_read(child, (char *)INVALID_ADDRESS, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_9 = { &preamble_test_case_3_1_9, &test_case_3_1_9, &postamble_0 };

#define test_case_3_1_9_stream_0 (&test_3_1_9)
#define test_case_3_1_9_stream_1 (NULL)
#define test_case_3_1_9_stream_2 (NULL)

#define tgrp_case_3_1_10 test_group_3_1
#define numb_case_3_1_10 "3.1.10"
#define name_case_3_1_10 "Perform read - EINTR."
#define sref_case_3_1_10 sref_case_3_1
#define desc_case_3_1_10 "\
Check that read() can be performed on a Stream.  Checks that read() will\n\
block awaiting arrival of data at the Stream head, and that the wait can\n\
be interrupted by a signal and will return EINTR."

int
test_case_3_1_10(int child)
{
	char buf[16];

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_10 = { &preamble_0, &test_case_3_1_10, &postamble_0 };

#define test_case_3_1_10_stream_0 (&test_3_1_10)
#define test_case_3_1_10_stream_1 (NULL)
#define test_case_3_1_10_stream_2 (NULL)

#define tgrp_case_3_1_11_1 test_group_3_1
#define numb_case_3_1_11_1 "3.1.11.1"
#define name_case_3_1_11_1 "Perform read - RNORM."
#define sref_case_3_1_11_1 sref_case_3_1
#define desc_case_3_1_11_1 "\
Check that read() can be performed on a Stream.  Check that if a portion\n\
of a message is read in byte-mode (RNORM) that the remainder is placed\n\
back on the read queue."

int
preamble_test_case_3_1_11_1(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_11_1(int child)
{
	char buf[32] = { 0, };
	int i;

	if (test_read(child, buf, 16) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 16)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 16; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf + 16, 16) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 16)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 16; i++)
		if (buf[i + 16] != (char) (i + 16))
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_11_1 = { &preamble_test_case_3_1_11_1, &test_case_3_1_11_1, &postamble_0 };

#define test_case_3_1_11_1_stream_0 (&test_3_1_11_1)
#define test_case_3_1_11_1_stream_1 (NULL)
#define test_case_3_1_11_1_stream_2 (NULL)

#define tgrp_case_3_1_11_2 test_group_3_1
#define numb_case_3_1_11_2 "3.1.11.2"
#define name_case_3_1_11_2 "Perform read - RNORM."
#define sref_case_3_1_11_2 sref_case_3_1
#define desc_case_3_1_11_2 "\
Check that read() can be performed on a Stream.  Check that message\n\
boundaries are ignored in byte-mode (RNORM) and that read() continues\n\
until the byte count is satisfied."

int
preamble_test_case_3_1_11_2(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_11_2(int child)
{
	char buf[64] = { 0, };
	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != sizeof(buf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i + 32] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_11_2 = { &preamble_test_case_3_1_11_2, &test_case_3_1_11_2, &postamble_0 };

#define test_case_3_1_11_2_stream_0 (&test_3_1_11_2)
#define test_case_3_1_11_2_stream_1 (NULL)
#define test_case_3_1_11_2_stream_2 (NULL)

#define tgrp_case_3_1_11_3 test_group_3_1
#define numb_case_3_1_11_3 "3.1.11.3"
#define name_case_3_1_11_3 "Perform read - RNORM."
#define sref_case_3_1_11_3 sref_case_3_1
#define desc_case_3_1_11_3 "\
Check that read() can be performed on a Stream.  Check that in byte-mode\n\
(RNORM), read() terminates on a zero-length message and that the\n\
zero-length message is placed back on the read queue for a subsequent\n\
read() call."

int
preamble_test_case_3_1_11_3(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	dat.len = 0;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_11_3(int child)
{
	char buf[64] = { 0, };
	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_11_3 = { &preamble_test_case_3_1_11_3, &test_case_3_1_11_3, &postamble_0 };

#define test_case_3_1_11_3_stream_0 (&test_3_1_11_3)
#define test_case_3_1_11_3_stream_1 (NULL)
#define test_case_3_1_11_3_stream_2 (NULL)

#define tgrp_case_3_1_12_1 test_group_3_1
#define numb_case_3_1_12_1 "3.1.12.1"
#define name_case_3_1_12_1 "Perform read - RMSGD."
#define sref_case_3_1_12_1 sref_case_3_1
#define desc_case_3_1_12_1 "\
Check that read() can be performed on a Stream.  Check that if a portion\n\
of a message is read in message discard read mode (RMSGD) that the\n\
remainder of the message is discarded."

int
preamble_test_case_3_1_12_1(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_12_1(int child)
{
	char buf[32] = { 0, };
	int i;

	if (test_read(child, buf, 16) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 16)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 16; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf + 16, 16) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_12_1 = { &preamble_test_case_3_1_12_1, &test_case_3_1_12_1, &postamble_0 };

#define test_case_3_1_12_1_stream_0 (&test_3_1_12_1)
#define test_case_3_1_12_1_stream_1 (NULL)
#define test_case_3_1_12_1_stream_2 (NULL)

#define tgrp_case_3_1_12_2 test_group_3_1
#define numb_case_3_1_12_2 "3.1.12.2"
#define name_case_3_1_12_2 "Perform read - RMSGD."
#define sref_case_3_1_12_2 sref_case_3_1
#define desc_case_3_1_12_2 "\
Check that read() can be performed on a Stream.  Check that if an entire\n\
message is read in message discard mode (RMSGD) that read() terminates\n\
and returns the number of bytes read."

int
preamble_test_case_3_1_12_2(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_12_2(int child)
{
	char buf[64] = { 0, };
	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_12_2 = { &preamble_test_case_3_1_12_2, &test_case_3_1_12_2, &postamble_0 };

#define test_case_3_1_12_2_stream_0 (&test_3_1_12_2)
#define test_case_3_1_12_2_stream_1 (NULL)
#define test_case_3_1_12_2_stream_2 (NULL)

#define tgrp_case_3_1_12_3 test_group_3_1
#define numb_case_3_1_12_3 "3.1.12.3"
#define name_case_3_1_12_3 "Perform read - RMSGD."
#define sref_case_3_1_12_3 sref_case_3_1
#define desc_case_3_1_12_3 "\
Check that read() can be performed on a Stream.  Check that a\n\
zero-length message terminates the read() and returns zero in message\n\
discard mode (RMSGD)."

int
preamble_test_case_3_1_12_3(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RMSGD | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	dat.len = 0;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_12_3(int child)
{
	char buf[64] = { 0, };
	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_12_3 = { &preamble_test_case_3_1_12_3, &test_case_3_1_12_3, &postamble_0 };

#define test_case_3_1_12_3_stream_0 (&test_3_1_12_3)
#define test_case_3_1_12_3_stream_1 (NULL)
#define test_case_3_1_12_3_stream_2 (NULL)

#define tgrp_case_3_1_13_1 test_group_3_1
#define numb_case_3_1_13_1 "3.1.13.1"
#define name_case_3_1_13_1 "Perform read - RMSGN."
#define sref_case_3_1_13_1 sref_case_3_1
#define desc_case_3_1_13_1 "\
Check that read() can be performed on a Stream.  Check that if the\n\
entire message is not read, that the remainder is placed back on the\n\
read queue in RMSGN read mode."

int
preamble_test_case_3_1_13_1(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_13_1(int child)
{
	char buf[32] = { 0, };
	int i;

	if (test_read(child, buf, 16) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 16)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 16; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf + 16, 16) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 16)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 16; i++)
		if (buf[i + 16] != (char) (i + 16))
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_13_1 = { &preamble_test_case_3_1_13_1, &test_case_3_1_13_1, &postamble_0 };

#define test_case_3_1_13_1_stream_0 (&test_3_1_13_1)
#define test_case_3_1_13_1_stream_1 (NULL)
#define test_case_3_1_13_1_stream_2 (NULL)

#define tgrp_case_3_1_13_2 test_group_3_1
#define numb_case_3_1_13_2 "3.1.13.2"
#define name_case_3_1_13_2 "Perform read - RMSGN."
#define sref_case_3_1_13_2 sref_case_3_1
#define desc_case_3_1_13_2 "\
Check that read() can be performed on a Stream.  Check that if an entire\n\
message is read in message non-discard mode (RMSGN) that read()\n\
terminates and returns the number of bytes read."

int
preamble_test_case_3_1_13_2(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_13_2(int child)
{
	char buf[64] = { 0, };
	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_13_2 = { &preamble_test_case_3_1_13_2, &test_case_3_1_13_2, &postamble_0 };

#define test_case_3_1_13_2_stream_0 (&test_3_1_13_2)
#define test_case_3_1_13_2_stream_1 (NULL)
#define test_case_3_1_13_2_stream_2 (NULL)

#define tgrp_case_3_1_13_3 test_group_3_1
#define numb_case_3_1_13_3 "3.1.13.3"
#define name_case_3_1_13_3 "Perform read - RMSGN."
#define sref_case_3_1_13_3 sref_case_3_1
#define desc_case_3_1_13_3 "\
Check that read() can be performed on a Stream.  Check that a\n\
zero-length message terminates the read() and returns zero in message\n\
non-discard mode (RMSGN)."

int
preamble_test_case_3_1_13_3(int child)
{
	char dbuf[32] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RMSGN | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	dat.len = 0;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_13_3(int child)
{
	char buf[64] = { 0, };
	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 32; i++)
		if (buf[i] != (char) i)
			return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_13_3 = { &preamble_test_case_3_1_13_3, &test_case_3_1_13_3, &postamble_0 };

#define test_case_3_1_13_3_stream_0 (&test_3_1_13_3)
#define test_case_3_1_13_3_stream_1 (NULL)
#define test_case_3_1_13_3_stream_2 (NULL)

#define tgrp_case_3_1_14_1 test_group_3_1
#define numb_case_3_1_14_1 "3.1.14.1"
#define name_case_3_1_14_1 "Perform read - RPROTNORM EBADMSG."
#define sref_case_3_1_14_1 sref_case_3_1
#define desc_case_3_1_14_1 "\
Check that read() can be performed on a Stream.  The file is a Stream\n\
file that is set to control-normal mode and the message waiting to be\n\
read includes a control part."

int
preamble_test_case_3_1_14_1(int child)
{
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(cbuf); i++)
		cbuf[i] = i;
	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i + sizeof(cbuf);

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_14_1(int child)
{
	char buf[32] = { 0, };

	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EBADMSG)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_14_1 = { &preamble_test_case_3_1_14_1, &test_case_3_1_14_1, &postamble_0 };

#define test_case_3_1_14_1_stream_0 (&test_3_1_14_1)
#define test_case_3_1_14_1_stream_1 (NULL)
#define test_case_3_1_14_1_stream_2 (NULL)

#define tgrp_case_3_1_14_2 test_group_3_1
#define numb_case_3_1_14_2 "3.1.14.2"
#define name_case_3_1_14_2 "Perform read - RPROTNORM."
#define sref_case_3_1_14_2 sref_case_3_1
#define desc_case_3_1_14_2 "\
Check that read() can be performed on a Stream.  Checks that messages\n\
without a control part are read correctly in RPROTNORM mode."

int
preamble_test_case_3_1_14_2(int child)
{
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(cbuf); i++)
		cbuf[i] = i;
	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i + sizeof(cbuf);

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTNORM)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, NULL, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_14_2(int child)
{
	char buf[32] = { 0, };
	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 16)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 16; i++)
		if (buf[i] != (char)(i + 16))
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_14_2 = { &preamble_test_case_3_1_14_2, &test_case_3_1_14_2, &postamble_0 };

#define test_case_3_1_14_2_stream_0 (&test_3_1_14_2)
#define test_case_3_1_14_2_stream_1 (NULL)
#define test_case_3_1_14_2_stream_2 (NULL)

#define tgrp_case_3_1_15 test_group_3_1
#define numb_case_3_1_15 "3.1.15"
#define name_case_3_1_15 "Perform read - RPROTDAT."
#define sref_case_3_1_15 sref_case_3_1
#define desc_case_3_1_15 "\
Check that read() can be performed on a Stream.  Checks that a control\n\
part is read as data in RPROTDAT read control mode."

int
preamble_test_case_3_1_15(int child)
{
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(cbuf); i++)
		cbuf[i] = i;
	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i + sizeof(cbuf);

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTDAT)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_15(int child)
{

	char buf[32] = { 0, };

	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 32)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(buf); i++)
		if (buf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_15 = { &preamble_test_case_3_1_15, &test_case_3_1_15, &postamble_0 };

#define test_case_3_1_15_stream_0 (&test_3_1_15)
#define test_case_3_1_15_stream_1 (NULL)
#define test_case_3_1_15_stream_2 (NULL)

#define tgrp_case_3_1_16 test_group_3_1
#define numb_case_3_1_16 "3.1.16"
#define name_case_3_1_16 "Perform read - RPROTDIS."
#define sref_case_3_1_16 sref_case_3_1
#define desc_case_3_1_16 "\
Check that read() can be performed on a Stream.  Checks that a control\n\
part is discarded in RPROTDIS read control mode."

int
preamble_test_case_3_1_16(int child)
{
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(cbuf); i++)
		cbuf[i] = i;
	for (i = 0; i < sizeof(dbuf); i++)
		dbuf[i] = i + sizeof(cbuf);

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_SRDOPT, (RNORM | RPROTDIS)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_1_16(int child)
{
	char buf[32] = { 0, };

	int i;

	if (test_read(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 16)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 16; i++)
		if (buf[i] != (char)(i + 16))
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_1_16 = { &preamble_test_case_3_1_16, &test_case_3_1_16, &postamble_0 };

#define test_case_3_1_16_stream_0 (&test_3_1_16)
#define test_case_3_1_16_stream_1 (NULL)
#define test_case_3_1_16_stream_2 (NULL)

static const char test_group_3_2[] = "Perform READV on one Stream";
static const char sref_case_3_2[] = "POSIX 1003.1 2003/SUSv3 readv(2p) reference page.";

#define tgrp_case_3_2_1 test_group_3_2
#define numb_case_3_2_1 "3.2.1"
#define name_case_3_2_1 "Perform readv - EOF."
#define sref_case_3_2_1 sref_case_3_2
#define desc_case_3_2_1 "\
Check that readv() can be performed on a Stream.  Checks that zero (0) is\n\
returned when readv() is attempted on a Stream that has received a hang\n\
up."

int
test_case_3_2_1(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_1 = { &preamble_2_1, &test_case_3_2_1, &postamble_2 };

#define test_case_3_2_1_stream_0 (&test_3_2_1)
#define test_case_3_2_1_stream_1 (NULL)
#define test_case_3_2_1_stream_2 (NULL)

#define tgrp_case_3_2_2 test_group_3_2
#define numb_case_3_2_2 "3.2.2"
#define name_case_3_2_2 "Perform readv - EPROTO."
#define sref_case_3_2_2 sref_case_3_2
#define desc_case_3_2_2 "\
Check that readv() can be performed on a Stream.  Checks that EPROTO is\n\
returned when readv() is attempted on a Stream that has received an\n\
asynchronous EPROTO readv error."

int
test_case_3_2_2(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_2 = { &preamble_2_2, &test_case_3_2_2, &postamble_2 };

#define test_case_3_2_2_stream_0 (&test_3_2_2)
#define test_case_3_2_2_stream_1 (NULL)
#define test_case_3_2_2_stream_2 (NULL)

#define tgrp_case_3_2_3 test_group_3_2
#define numb_case_3_2_3 "3.2.3"
#define name_case_3_2_3 "Perform readv - EAGAIN."
#define sref_case_3_2_3 sref_case_3_2
#define desc_case_3_2_3 "\
Check that readv() can be performed on a Stream.  Checks that EAGAIN is\n\
returned when readv() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO write error (and non-blocking operation is set)."

int
test_case_3_2_3(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_3 = { &preamble_2_3, &test_case_3_2_3, &postamble_2 };

#define test_case_3_2_3_stream_0 (&test_3_2_3)
#define test_case_3_2_3_stream_1 (NULL)
#define test_case_3_2_3_stream_2 (NULL)

#define tgrp_case_3_2_4 test_group_3_2
#define numb_case_3_2_4 "3.2.4"
#define name_case_3_2_4 "Perform readv - EPROTO."
#define sref_case_3_2_4 sref_case_3_2
#define desc_case_3_2_4 "\
Check that readv() can be performed on a Stream.  Checks that EPROTO is\n\
returned when readv() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO error."

int
test_case_3_2_4(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_4 = { &preamble_2_4, &test_case_3_2_4, &postamble_2 };

#define test_case_3_2_4_stream_0 (&test_3_2_4)
#define test_case_3_2_4_stream_1 (NULL)
#define test_case_3_2_4_stream_2 (NULL)

#define tgrp_case_3_2_5 test_group_3_2
#define numb_case_3_2_5 "3.2.5"
#define name_case_3_2_5 "Perform readv - EINVAL."
#define sref_case_3_2_5 sref_case_3_2
#define desc_case_3_2_5 "\
Check that readv() can be performed on a Stream.  Checks that EINVAL is\n\
returned when readv() is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_3_2_5(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_5 = { &preamble_5, &test_case_3_2_5, &postamble_5 };

#define test_case_3_2_5_stream_0 (&test_3_2_5)
#define test_case_3_2_5_stream_1 (NULL)
#define test_case_3_2_5_stream_2 (NULL)

#define tgrp_case_3_2_6 test_group_3_2
#define numb_case_3_2_6 "3.2.6"
#define name_case_3_2_6 "Perform readv - EAGAIN."
#define sref_case_3_2_6 sref_case_3_2
#define desc_case_3_2_6 "\
Check that readv() can be performed on a Stream.  The O_NONBLOCK flag is\n\
set for the file descriptor and the process would be delayed."

int
test_case_3_2_6(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_6 = { &preamble_0, &test_case_3_2_6, &postamble_0 };

#define test_case_3_2_6_stream_0 (&test_3_2_6)
#define test_case_3_2_6_stream_1 (NULL)
#define test_case_3_2_6_stream_2 (NULL)

#define tgrp_case_3_2_7 test_group_3_2
#define numb_case_3_2_7 "3.2.7"
#define name_case_3_2_7 "Perform readv - EBADF."
#define sref_case_3_2_7 sref_case_3_2
#define desc_case_3_2_7 "\
Check that readv() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not a valid file descriptor."

int
test_case_3_2_7(int child)
{
	char buf[16];
	int oldfd = test_fd[child];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	test_fd[child] = -1;

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_7 = { &preamble_0, &test_case_3_2_7, &postamble_0 };

#define test_case_3_2_7_stream_0 (&test_3_2_7)
#define test_case_3_2_7_stream_1 (NULL)
#define test_case_3_2_7_stream_2 (NULL)

#define tgrp_case_3_2_8 test_group_3_2
#define numb_case_3_2_8 "3.2.8"
#define name_case_3_2_8 "Perform readv - EBADF."
#define sref_case_3_2_8 sref_case_3_2
#define desc_case_3_2_8 "\
Check that readv() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not open for reading."

int
test_case_3_2_8(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_8 = { &preamble_0_2, &test_case_3_2_8, &postamble_0 };

#define test_case_3_2_8_stream_0 (&test_3_2_8)
#define test_case_3_2_8_stream_1 (NULL)
#define test_case_3_2_8_stream_2 (NULL)

#define tgrp_case_3_2_9 test_group_3_2
#define numb_case_3_2_9 "3.2.9"
#define name_case_3_2_9 "Perform readv - EFAULT."
#define sref_case_3_2_9 sref_case_3_2
#define desc_case_3_2_9 "\
Check that readv() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the iov argument describes an array that extends outside\n\
the caller's valid address space."

int
test_case_3_2_9(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, (struct iovec *)INVALID_ADDRESS, 1) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_9 = { &preamble_0, &test_case_3_2_9, &postamble_0 };

#define test_case_3_2_9_stream_0 (&test_3_2_9)
#define test_case_3_2_9_stream_1 (NULL)
#define test_case_3_2_9_stream_2 (NULL)

#define tgrp_case_3_2_10 test_group_3_2
#define numb_case_3_2_10 "3.2.10"
#define name_case_3_2_10 "Perform readv - EFAULT."
#define sref_case_3_2_10 sref_case_3_2
#define desc_case_3_2_10 "\
Check that readv() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the iov array describes a buffer that extends outside the\n\
caller's valid address space."

int
preamble_test_case_3_2_10(int child)
{
	char buf[16];

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_3_2_10(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = (char *)INVALID_ADDRESS;
	iov[0].iov_len = sizeof(buf);

	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_10 = { &preamble_test_case_3_2_10, &test_case_3_2_10, &postamble_0 };

#define test_case_3_2_10_stream_0 (&test_3_2_10)
#define test_case_3_2_10_stream_1 (NULL)
#define test_case_3_2_10_stream_2 (NULL)

#define tgrp_case_3_2_11 test_group_3_2
#define numb_case_3_2_11 "3.2.11"
#define name_case_3_2_11 "Perform readv - EFAULT."
#define sref_case_3_2_11 sref_case_3_2
#define desc_case_3_2_11 "\
Check that readv() can be performed on a Stream.  Checks readv() will\n\
block awaiting data at the Stream head, and that the wait can be\n\
interrupted by a signal and readv() will return EINTR."

int
test_case_3_2_11(int child)
{
	char buf[16];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_readv(child, iov, 1) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_2_11 = { &preamble_0, &test_case_3_2_11, &postamble_0 };

#define test_case_3_2_11_stream_0 (&test_3_2_11)
#define test_case_3_2_11_stream_1 (NULL)
#define test_case_3_2_11_stream_2 (NULL)

static const char test_group_3_3[] = "Perform WRITE on one Stream";
static const char sref_case_3_3[] = "POSIX 1003.1 2003/SUSv3 write(2p) reference page.";

#define tgrp_case_3_3_1 test_group_3_3
#define numb_case_3_3_1 "3.3.1"
#define name_case_3_3_1 "Perform write - ENXIO."
#define sref_case_3_3_1 sref_case_3_3
#define desc_case_3_3_1 "\
Check that write() can be performed on a Stream.  Checks that ENXIO is\n\
returned when write() is attempted on a Stream that has received a hang\n\
up."

int
test_case_3_3_1(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_1 = { &preamble_2_1, &test_case_3_3_1, &postamble_2 };

#define test_case_3_3_1_stream_0 (&test_3_3_1)
#define test_case_3_3_1_stream_1 (NULL)
#define test_case_3_3_1_stream_2 (NULL)

#define tgrp_case_3_3_2 test_group_3_3
#define numb_case_3_3_2 "3.3.2"
#define name_case_3_3_2 "Perform write."
#define sref_case_3_3_2 sref_case_3_3
#define desc_case_3_3_2 "\
Check that write() can be performed on a Stream.  Checks that write() is\n\
successful on a Stream that has received an asynchronous EPROTO read\n\
error."

int
test_case_3_3_2(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, buf, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_2 = { &preamble_2_2, &test_case_3_3_2, &postamble_2 };

#define test_case_3_3_2_stream_0 (&test_3_3_2)
#define test_case_3_3_2_stream_1 (NULL)
#define test_case_3_3_2_stream_2 (NULL)

#define tgrp_case_3_3_3 test_group_3_3
#define numb_case_3_3_3 "3.3.3"
#define name_case_3_3_3 "Perform write - EPROTO."
#define sref_case_3_3_3 sref_case_3_3
#define desc_case_3_3_3 "\
Check that write() can be performed on a Stream.  Checks that EPROTO is\n\
returned when write() is attempted on a Stream that has received an\n\
asynchronous EPROTO write error."

int
test_case_3_3_3(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_3 = { &preamble_2_3, &test_case_3_3_3, &postamble_2 };

#define test_case_3_3_3_stream_0 (&test_3_3_3)
#define test_case_3_3_3_stream_1 (NULL)
#define test_case_3_3_3_stream_2 (NULL)

#define tgrp_case_3_3_4 test_group_3_3
#define numb_case_3_3_4 "3.3.4"
#define name_case_3_3_4 "Perform write - EPROTO."
#define sref_case_3_3_4 sref_case_3_3
#define desc_case_3_3_4 "\
Check that write() can be performed on a Stream.  Checks that EPROTO is\n\
returned when write() is attempted on a Stream that has received an\n\
asynchronous EPROTO error."

int
test_case_3_3_4(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_4 = { &preamble_2_4, &test_case_3_3_4, &postamble_2 };

#define test_case_3_3_4_stream_0 (&test_3_3_4)
#define test_case_3_3_4_stream_1 (NULL)
#define test_case_3_3_4_stream_2 (NULL)

#define tgrp_case_3_3_5 test_group_3_3
#define numb_case_3_3_5 "3.3.5"
#define name_case_3_3_5 "Perform write - EINVAL."
#define sref_case_3_3_5 sref_case_3_3
#define desc_case_3_3_5 "\
Check that write() can be performed on a Stream.  Checks that EINVAL is\n\
returned when write() is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_3_3_5(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_5 = { &preamble_5, &test_case_3_3_5, &postamble_5 };

#define test_case_3_3_5_stream_0 (&test_3_3_5)
#define test_case_3_3_5_stream_1 (NULL)
#define test_case_3_3_5_stream_2 (NULL)

#define tgrp_case_3_3_6 test_group_3_3
#define numb_case_3_3_6 "3.3.6"
#define name_case_3_3_6 "Perform write."
#define sref_case_3_3_6 sref_case_3_3
#define desc_case_3_3_6 "\
Check that write() can be performed on a Stream.  Checks that a write()\n\
of zero bytes returns zero."

int
test_case_3_3_6(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, buf, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_6 = { &preamble_0, &test_case_3_3_6, &postamble_0 };

#define test_case_3_3_6_stream_0 (&test_3_3_6)
#define test_case_3_3_6_stream_1 (NULL)
#define test_case_3_3_6_stream_2 (NULL)

#define tgrp_case_3_3_7 test_group_3_3
#define numb_case_3_3_7 "3.3.7"
#define name_case_3_3_7 "Perform write - EBADF."
#define sref_case_3_3_7 sref_case_3_3
#define desc_case_3_3_7 "\
Check that write() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is an invalid file descriptor."

int
test_case_3_3_7(int child)
{
	char buf[16] = { 0, };
	int oldfd = test_fd[child];

	test_fd[child] = -1;

	if (test_write(child, buf, 0) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_7 = { &preamble_0, &test_case_3_3_7, &postamble_0 };

#define test_case_3_3_7_stream_0 (&test_3_3_7)
#define test_case_3_3_7_stream_1 (NULL)
#define test_case_3_3_7_stream_2 (NULL)

#define tgrp_case_3_3_8 test_group_3_3
#define numb_case_3_3_8 "3.3.8"
#define name_case_3_3_8 "Perform write - EBADF."
#define sref_case_3_3_8 sref_case_3_3
#define desc_case_3_3_8 "\
Check that write() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is not open for writing."

int
test_case_3_3_8(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, buf, 0) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_8 = { &preamble_0_1, &test_case_3_3_8, &postamble_0 };

#define test_case_3_3_8_stream_0 (&test_3_3_8)
#define test_case_3_3_8_stream_1 (NULL)
#define test_case_3_3_8_stream_2 (NULL)

#define tgrp_case_3_3_9 test_group_3_3
#define numb_case_3_3_9 "3.3.9"
#define name_case_3_3_9 "Perform write - EFAULT."
#define sref_case_3_3_9 sref_case_3_3
#define desc_case_3_3_9 "\
Check that write() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the buffer extends outside the caller's valid address\n\
space."

int
test_case_3_3_9(int child)
{
	char buf[16] = { 0, };

	if (test_write(child, (char *)INVALID_ADDRESS, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_9 = { &preamble_0, &test_case_3_3_9, &postamble_0 };

#define test_case_3_3_9_stream_0 (&test_3_3_9)
#define test_case_3_3_9_stream_1 (NULL)
#define test_case_3_3_9_stream_2 (NULL)

#define tgrp_case_3_3_10 test_group_3_3
#define numb_case_3_3_10 "3.3.10"
#define name_case_3_3_10 "Perform write - EINTR."
#define sref_case_3_3_10 sref_case_3_3
#define desc_case_3_3_10 "\
Check that write() can be performed on a Stream.  Checks write() will\n\
block awaiting flow control at the Stream head, and that the wait can be\n\
interrupted by a signal and write() will return EINTR."

int
test_case_3_3_10(int child)
{
	char buf[16] = { 0, };

	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_10 = { &preamble_8_0, &test_case_3_3_10, &postamble_8 };

#define test_case_3_3_10_stream_0 (&test_3_3_10)
#define test_case_3_3_10_stream_1 (NULL)
#define test_case_3_3_10_stream_2 (NULL)

#define tgrp_case_3_3_11 test_group_3_3
#define numb_case_3_3_11 "3.3.11"
#define name_case_3_3_11 "Perform write."
#define sref_case_3_3_11 sref_case_3_3
#define desc_case_3_3_11 "\
Check that write() can be performed on a Stream."

int
test_case_3_3_11(int child)
{
	char buf[32] = { 0, };
	char dbuf[64] = { 0, };
	struct strbuf ctl = { -1, -1, NULL };
	struct strbuf dat = { sizeof(dbuf), -1, dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < sizeof(buf); i++)
		buf[i] = (char)i;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != sizeof(buf))
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &ctl, &dat, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (dat.len != sizeof(buf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(buf); i++)
		if (dbuf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_11 = { &preamble_0, &test_case_3_3_11, &postamble_0 };

#define test_case_3_3_11_stream_0 (&test_3_3_11)
#define test_case_3_3_11_stream_1 (NULL)
#define test_case_3_3_11_stream_2 (NULL)

#define tgrp_case_3_3_12 test_group_3_3
#define numb_case_3_3_12 "3.3.12"
#define name_case_3_3_12 "Perform write."
#define sref_case_3_3_12 sref_case_3_3
#define desc_case_3_3_12 "\
Check that write() can be performed on a Stream.  Check that if zero\n\
bytes are written to a Stream that a zero-length message is sent and\n\
zero is returned."

int
test_case_3_3_12(int child)
{
	char buf[32] = { 0, };
	
	if (test_write(child, buf, 0) != __RESULT_SUCCESS)
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
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_12 = { &preamble_0, &test_case_3_3_12, &postamble_0 };

#define test_case_3_3_12_stream_0 (&test_3_3_12)
#define test_case_3_3_12_stream_1 (NULL)
#define test_case_3_3_12_stream_2 (NULL)

#define tgrp_case_3_3_13 test_group_3_3
#define numb_case_3_3_13 "3.3.13"
#define name_case_3_3_13 "Perform write."
#define sref_case_3_3_13 sref_case_3_3
#define desc_case_3_3_13 "\
Check that write() can be performed on a Stream.  Check that if zero\n\
bytes are written to a pipe or FIFO that no message is sent and zero is\n\
returned."

int
test_case_3_3_13(int child)
{
	char buf[32] = { 0, };
	
	if (test_write(child, buf, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_13 = { &preamble_6, &test_case_3_3_13, &postamble_6 };

#define test_case_3_3_13_stream_0 (&test_3_3_13)
#define test_case_3_3_13_stream_1 (NULL)
#define test_case_3_3_13_stream_2 (NULL)

#define tgrp_case_3_3_14 test_group_3_3
#define numb_case_3_3_14 "3.3.14"
#define name_case_3_3_14 "Perform write."
#define sref_case_3_3_14 sref_case_3_3
#define desc_case_3_3_14 "\
Check that write() can be performed on a Stream.  Check that if write\n\
option SNDZERO is set on a pipe or FIFO that zero bytes written sends a\n\
zero-length message and returns zero."

int
test_case_3_3_14(int child)
{
	char buf[32] = { 0, };

	if (test_ioctl(child, I_SWROPT, (SNDZERO)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, 0) != __RESULT_SUCCESS)
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
	if (test_read(child, buf, sizeof(buf)) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_14 = { &preamble_0, &test_case_3_3_14, &postamble_0 };

#define test_case_3_3_14_stream_0 (&test_3_3_14)
#define test_case_3_3_14_stream_1 (NULL)
#define test_case_3_3_14_stream_2 (NULL)

#define tgrp_case_3_3_15 test_group_3_3
#define numb_case_3_3_15 "3.3.15"
#define name_case_3_3_15 "Perform write."
#define sref_case_3_3_15 sref_case_3_3
#define desc_case_3_3_15 "\
Check that write() can be performed on a Stream.  Check that write()\n\
breaks large writes down into maximum packet size chunks."

int
test_case_3_3_15(int child)
{
	char buf[4096 + 128] = { 0, };
	char dbuf[4096 + 128] = { 0, };
	struct strbuf dat = { sizeof(dbuf), -1, dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < 4096; i++)
		buf[i] = (char)i;
	for (i = 0; i < 128; i++)
		buf[i+4096] = (char)(128-i);

	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != (4096 + 128))
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (dat.len != 4096)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4096; i++)
		if (dbuf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (dat.len != 128)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 128; i++)
		if (dbuf[i] != (char)(128-i)) {
			printf("Character at position %d is %d\n", i, (int)dbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (test_getmsg(child, NULL, &dat, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_15 = { &preamble_0, &test_case_3_3_15, &postamble_0 };

#define test_case_3_3_15_stream_0 (&test_3_3_15)
#define test_case_3_3_15_stream_1 (NULL)
#define test_case_3_3_15_stream_2 (NULL)

#define tgrp_case_3_3_16 test_group_3_3
#define numb_case_3_3_16 "3.3.16"
#define name_case_3_3_16 "Perform write."
#define sref_case_3_3_16 sref_case_3_3
#define desc_case_3_3_16 "\
Check that write() can be performed on a Stream.  Check that a write()\n\
of exactly the maximum packet size does not also generate a zero length\n\
message."

int
test_case_3_3_16(int child)
{
	char buf[4096] = { 0, };
	char dbuf[4096] = { 0, };
	struct strbuf dat = { sizeof(dbuf), -1, dbuf };
	int flags = 0;
	int i;

	for (i = 0; i < 4096; i++)
		buf[i] = (char)i;

	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != (4096))
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, NULL, &dat, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (dat.len != 4096)
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < 4096; i++)
		if (dbuf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, NULL, &dat, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_3_16 = { &preamble_0, &test_case_3_3_16, &postamble_0 };

#define test_case_3_3_16_stream_0 (&test_3_3_16)
#define test_case_3_3_16_stream_1 (NULL)
#define test_case_3_3_16_stream_2 (NULL)

static const char test_group_3_4[] = "Perform WRITEV on one Stream";
static const char sref_case_3_4[] = "POSIX 1003.1 2003/SUSv3 writev(2p) reference page.";

#define tgrp_case_3_4_1 test_group_3_4
#define numb_case_3_4_1 "3.4.1"
#define name_case_3_4_1 "Perform writev - ENXIO."
#define sref_case_3_4_1 sref_case_3_4
#define desc_case_3_4_1 "\
Check that writev() can be performed on a Stream.  Checks that ENXIO is\n\
returned when writev() is attempted on a Stream that has received a hang\n\
up."

int
test_case_3_4_1(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_1 = { &preamble_2_1, &test_case_3_4_1, &postamble_2 };

#define test_case_3_4_1_stream_0 (&test_3_4_1)
#define test_case_3_4_1_stream_1 (NULL)
#define test_case_3_4_1_stream_2 (NULL)

#define tgrp_case_3_4_2 test_group_3_4
#define numb_case_3_4_2 "3.4.2"
#define name_case_3_4_2 "Perform writev."
#define sref_case_3_4_2 sref_case_3_4
#define desc_case_3_4_2 "\
Check that writev() can be performed on a Stream.  Checks that writev() is\n\
successful on a Stream that has received an asynchronous EPROTO read\n\
error."

int
test_case_3_4_2(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = 0;

	if (test_writev(child, iov, 1) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_2 = { &preamble_2_2, &test_case_3_4_2, &postamble_2 };

#define test_case_3_4_2_stream_0 (&test_3_4_2)
#define test_case_3_4_2_stream_1 (NULL)
#define test_case_3_4_2_stream_2 (NULL)

#define tgrp_case_3_4_3 test_group_3_4
#define numb_case_3_4_3 "3.4.3"
#define name_case_3_4_3 "Perform writev - EPROTO."
#define sref_case_3_4_3 sref_case_3_4
#define desc_case_3_4_3 "\
Check that writev() can be performed on a Stream.  Checks that EPROTO is\n\
returned when writev() is attempted on a Stream that has received an\n\
asynchronous EPROTO writev error."

int
test_case_3_4_3(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_3 = { &preamble_2_3, &test_case_3_4_3, &postamble_2 };

#define test_case_3_4_3_stream_0 (&test_3_4_3)
#define test_case_3_4_3_stream_1 (NULL)
#define test_case_3_4_3_stream_2 (NULL)

#define tgrp_case_3_4_4 test_group_3_4
#define numb_case_3_4_4 "3.4.4"
#define name_case_3_4_4 "Perform writev - EPROTO."
#define sref_case_3_4_4 sref_case_3_4
#define desc_case_3_4_4 "\
Check that writev() can be performed on a Stream.  Checks that EPROTO is\n\
returned when writev() is attempted on a Stream that has received an\n\
asynchronous EPROTO error."

int
test_case_3_4_4(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_4 = { &preamble_2_4, &test_case_3_4_4, &postamble_2 };

#define test_case_3_4_4_stream_0 (&test_3_4_4)
#define test_case_3_4_4_stream_1 (NULL)
#define test_case_3_4_4_stream_2 (NULL)

#define tgrp_case_3_4_5 test_group_3_4
#define numb_case_3_4_5 "3.4.5"
#define name_case_3_4_5 "Perform writev - EINVAL."
#define sref_case_3_4_5 sref_case_3_4
#define desc_case_3_4_5 "\
Check that writev() can be performed on a Stream.  Checks that EINVAL is\n\
returned when writev() is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_3_4_5(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_5 = { &preamble_5, &test_case_3_4_5, &postamble_5 };

#define test_case_3_4_5_stream_0 (&test_3_4_5)
#define test_case_3_4_5_stream_1 (NULL)
#define test_case_3_4_5_stream_2 (NULL)

#define tgrp_case_3_4_6 test_group_3_4
#define numb_case_3_4_6 "3.4.6"
#define name_case_3_4_6 "Perform writev."
#define sref_case_3_4_6 sref_case_3_4
#define desc_case_3_4_6 "\
Check that writev() can be performed on a Stream.  Checks that a writev()\n\
of zero bytes returns zero."

int
test_case_3_4_6(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = 0;

	if (test_writev(child, iov, 1) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_6 = { &preamble_0, &test_case_3_4_6, &postamble_0 };

#define test_case_3_4_6_stream_0 (&test_3_4_6)
#define test_case_3_4_6_stream_1 (NULL)
#define test_case_3_4_6_stream_2 (NULL)

#define tgrp_case_3_4_7 test_group_3_4
#define numb_case_3_4_7 "3.4.7"
#define name_case_3_4_7 "Perform writev - EBADF."
#define sref_case_3_4_7 sref_case_3_4
#define desc_case_3_4_7 "\
Check that writev() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is an invalid file descriptor."

int
test_case_3_4_7(int child)
{
	char buf[16] = { 0, };
	int oldfd = test_fd[child];
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = 0;

	test_fd[child] = -1;

	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_7 = { &preamble_0, &test_case_3_4_7, &postamble_0 };

#define test_case_3_4_7_stream_0 (&test_3_4_7)
#define test_case_3_4_7_stream_1 (NULL)
#define test_case_3_4_7_stream_2 (NULL)

#define tgrp_case_3_4_8 test_group_3_4
#define numb_case_3_4_8 "3.4.8"
#define name_case_3_4_8 "Perform writev - EBADF."
#define sref_case_3_4_8 sref_case_3_4
#define desc_case_3_4_8 "\
Check that writev() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is not open for writing."

int
test_case_3_4_8(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = 0;

	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_8 = { &preamble_0_1, &test_case_3_4_8, &postamble_0 };

#define test_case_3_4_8_stream_0 (&test_3_4_8)
#define test_case_3_4_8_stream_1 (NULL)
#define test_case_3_4_8_stream_2 (NULL)

#define tgrp_case_3_4_9 test_group_3_4
#define numb_case_3_4_9 "3.4.9"
#define name_case_3_4_9 "Perform writev - EFAULT."
#define sref_case_3_4_9 sref_case_3_4
#define desc_case_3_4_9 "\
Check that writev() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the iov argument describes an array that extends outside\n\
the caller's valid address space."

int
test_case_3_4_9(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);;

	if (test_writev(child, (struct iovec *)INVALID_ADDRESS, 1) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_9 = { &preamble_0, &test_case_3_4_9, &postamble_0 };

#define test_case_3_4_9_stream_0 (&test_3_4_9)
#define test_case_3_4_9_stream_1 (NULL)
#define test_case_3_4_9_stream_2 (NULL)

#define tgrp_case_3_4_10 test_group_3_4
#define numb_case_3_4_10 "3.4.10"
#define name_case_3_4_10 "Perform writev - EFAULT."
#define sref_case_3_4_10 sref_case_3_4
#define desc_case_3_4_10 "\
Check that writev() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the iov array describes a buffer that extends outside the\n\
caller's valid address space."

int
test_case_3_4_10(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = (char *)INVALID_ADDRESS;
	iov[0].iov_len = sizeof(buf);;

	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_10 = { &preamble_0, &test_case_3_4_10, &postamble_0 };

#define test_case_3_4_10_stream_0 (&test_3_4_10)
#define test_case_3_4_10_stream_1 (NULL)
#define test_case_3_4_10_stream_2 (NULL)

#define tgrp_case_3_4_11 test_group_3_4
#define numb_case_3_4_11 "3.4.11"
#define name_case_3_4_11 "Perform writev - EINTR."
#define sref_case_3_4_11 sref_case_3_4
#define desc_case_3_4_11 "\
Check that writev() can be performed on a Stream.  Checks writev() will\n\
block awaiting flow control at the Stream head, and that the wait can be\n\
interrupted by a signal and writev() will return EINTR."

int
test_case_3_4_11(int child)
{
	char buf[16] = { 0, };
	struct iovec iov[1];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);;

	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_writev(child, iov, 1) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_4_11 = { &preamble_8_0, &test_case_3_4_11, &postamble_8 };

#define test_case_3_4_11_stream_0 (&test_3_4_11)
#define test_case_3_4_11_stream_1 (NULL)
#define test_case_3_4_11_stream_2 (NULL)

static const char test_group_3_5[] = "Perform GETMSG on one Stream";
static const char sref_case_3_5[] = "POSIX 1003.1 2003/SUSv3 getmsg(2p) reference page.";

#define tgrp_case_3_5_1 test_group_3_5
#define numb_case_3_5_1 "3.5.1"
#define name_case_3_5_1 "Perform getmsg."
#define sref_case_3_5_1 sref_case_3_5
#define desc_case_3_5_1 "\
Check that getmsg() can be performed on a Stream.  Checks that zero is\n\
returned for both the control part size and data part size, indicating a\n\
hung up Stream, when getmsg() is called on a hung up Stream."

int
test_case_3_5_1(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (ctl.len != 0 || dat.len != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_1 = { &preamble_2_1, &test_case_3_5_1, &postamble_2 };

#define test_case_3_5_1_stream_0 (&test_3_5_1)
#define test_case_3_5_1_stream_1 (NULL)
#define test_case_3_5_1_stream_2 (NULL)

#define tgrp_case_3_5_2 test_group_3_5
#define numb_case_3_5_2 "3.5.2"
#define name_case_3_5_2 "Perform getmsg - EPROTO."
#define sref_case_3_5_2 sref_case_3_5
#define desc_case_3_5_2 "\
Check that getmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when getmsg() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO read error."

int
test_case_3_5_2(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_2 = { &preamble_2_2, &test_case_3_5_2, &postamble_2 };

#define test_case_3_5_2_stream_0 (&test_3_5_2)
#define test_case_3_5_2_stream_1 (NULL)
#define test_case_3_5_2_stream_2 (NULL)

#define tgrp_case_3_5_3 test_group_3_5
#define numb_case_3_5_3 "3.5.3"
#define name_case_3_5_3 "Perform getmsg - EAGAIN."
#define sref_case_3_5_3 sref_case_3_5
#define desc_case_3_5_3 "\
Check that getmsg() can be performed on a Stream.  Checks that EAGAIN is\n\
returned when getmsg() is attempted on a Stream that has received an\n\
asynchronous EPROTO write error and the Stream is set for non-blocking\n\
operation."

int
test_case_3_5_3(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_3 = { &preamble_2_3, &test_case_3_5_3, &postamble_2 };

#define test_case_3_5_3_stream_0 (&test_3_5_3)
#define test_case_3_5_3_stream_1 (NULL)
#define test_case_3_5_3_stream_2 (NULL)

#define tgrp_case_3_5_4 test_group_3_5
#define numb_case_3_5_4 "3.5.4"
#define name_case_3_5_4 "Perform getmsg - EPROTO."
#define sref_case_3_5_4 sref_case_3_5
#define desc_case_3_5_4 "\
Check that getmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when getmsg() is attempted on a Stream that has received an\n\
asynchronous EPROTO error."

int
test_case_3_5_4(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_4 = { &preamble_2_4, &test_case_3_5_4, &postamble_2 };

#define test_case_3_5_4_stream_0 (&test_3_5_4)
#define test_case_3_5_4_stream_1 (NULL)
#define test_case_3_5_4_stream_2 (NULL)

#define tgrp_case_3_5_5 test_group_3_5
#define numb_case_3_5_5 "3.5.5"
#define name_case_3_5_5 "Perform getmsg - EINVAL."
#define sref_case_3_5_5 sref_case_3_5
#define desc_case_3_5_5 "\
Check that getmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when getmsg() is attempted on a Stream is linked under a\n\
Multiplexing Driver."

int
test_case_3_5_5(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_5 = { &preamble_5, &test_case_3_5_5, &postamble_5 };

#define test_case_3_5_5_stream_0 (&test_3_5_5)
#define test_case_3_5_5_stream_1 (NULL)
#define test_case_3_5_5_stream_2 (NULL)

#define tgrp_case_3_5_6 test_group_3_5
#define numb_case_3_5_6 "3.5.6"
#define name_case_3_5_6 "Perform getmsg - EAGAIN."
#define sref_case_3_5_6 sref_case_3_5
#define desc_case_3_5_6 "\
Check that getmsg() can be performed on a Stream.  Checks that EAGAIN is\n\
returned when the O_NONBLOCK flag is set for the file descriptor and the\n\
process would be delayed."

int
test_case_3_5_6(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_6 = { &preamble_0, &test_case_3_5_6, &postamble_0 };

#define test_case_3_5_6_stream_0 (&test_3_5_6)
#define test_case_3_5_6_stream_1 (NULL)
#define test_case_3_5_6_stream_2 (NULL)

#define tgrp_case_3_5_7 test_group_3_5
#define numb_case_3_5_7 "3.5.7"
#define name_case_3_5_7 "Perform getmsg - EBADF."
#define sref_case_3_5_7 sref_case_3_5
#define desc_case_3_5_7 "\
Check that getmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not a valid file descriptor."

int
test_case_3_5_7(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];
	int oldfd = test_fd[child];

	test_fd[child] = -1;

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_7 = { &preamble_0, &test_case_3_5_7, &postamble_0 };

#define test_case_3_5_7_stream_0 (&test_3_5_7)
#define test_case_3_5_7_stream_1 (NULL)
#define test_case_3_5_7_stream_2 (NULL)

#define tgrp_case_3_5_8 test_group_3_5
#define numb_case_3_5_8 "3.5.8"
#define name_case_3_5_8 "Perform getmsg - EBADF."
#define sref_case_3_5_8 sref_case_3_5
#define desc_case_3_5_8 "\
Check that getmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not open for reading."

int
test_case_3_5_8(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_8 = { &preamble_0_2, &test_case_3_5_8, &postamble_0 };

#define test_case_3_5_8_stream_0 (&test_3_5_8)
#define test_case_3_5_8_stream_1 (NULL)
#define test_case_3_5_8_stream_2 (NULL)

#define tgrp_case_3_5_9 test_group_3_5
#define numb_case_3_5_9 "3.5.9"
#define name_case_3_5_9 "Perform getmsg - EINVAL."
#define sref_case_3_5_9 sref_case_3_5
#define desc_case_3_5_9 "\
Check that getmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when the flagsp argument points to flags that contain an\n\
invalid value (other than 0 or RS_HIPRI)."

int
test_case_3_5_9(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = RS_HIPRI + 1;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_9 = { &preamble_0, &test_case_3_5_9, &postamble_0 };

#define test_case_3_5_9_stream_0 (&test_3_5_9)
#define test_case_3_5_9_stream_1 (NULL)
#define test_case_3_5_9_stream_2 (NULL)

#define tgrp_case_3_5_10 test_group_3_5
#define numb_case_3_5_10 "3.5.10"
#define name_case_3_5_10 "Perform getmsg - EFAULT."
#define sref_case_3_5_10 sref_case_3_5
#define desc_case_3_5_10 "\
Check that getmsg() can be performed on a Stream.  Checks that EFAULT is\n\
returned when a buffer points outside the callers valid address space."

int
preamble_test_case_3_5_10(int child)
{
	char buf[16];

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_3_5_10(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;

	ctl.maxlen = 5;
	ctl.len = -1;
	ctl.buf = (char *)INVALID_ADDRESS;

	dat.maxlen = 5;
	dat.len = -1;
	dat.buf = (char *)INVALID_ADDRESS;

	flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_10 = { &preamble_test_case_3_5_10, &test_case_3_5_10, &postamble_0 };

#define test_case_3_5_10_stream_0 (&test_3_5_10)
#define test_case_3_5_10_stream_1 (NULL)
#define test_case_3_5_10_stream_2 (NULL)

#define tgrp_case_3_5_11 test_group_3_5
#define numb_case_3_5_11 "3.5.11"
#define name_case_3_5_11 "Perform getmsg - EINTR."
#define sref_case_3_5_11 sref_case_3_5
#define desc_case_3_5_11 "\
Check that getmsg() can be performed on a Stream.  Checks that getmsg()\n\
will block awaiting data at the Stream head, and that the wait can be\n\
intterupted by a signal ad getmsg() will return EINTR."

int
test_case_3_5_11(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	flags = 0;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_11 = { &preamble_0, &test_case_3_5_11, &postamble_0 };

#define test_case_3_5_11_stream_0 (&test_3_5_11)
#define test_case_3_5_11_stream_1 (NULL)
#define test_case_3_5_11_stream_2 (NULL)

int
test_case_3_5_12_x(int child, int flags)
{
	char gcbuf[128] = { 0, };
	char gdbuf[256] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putmsg(child, &pctl, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(pcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	if (gdat.len != sizeof(pdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_5_12_1 test_group_3_5
#define numb_case_3_5_12_1 "3.5.12.1"
#define name_case_3_5_12_1 "Perform getmsg."
#define sref_case_3_5_12_1 sref_case_3_5
#define desc_case_3_5_12_1 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
normal complete control and data part can be retrieved."

int
test_case_3_5_12_1(int child)
{
	return test_case_3_5_12_x(child, 0);
}
struct test_stream test_3_5_12_1 = { &preamble_0, &test_case_3_5_12_1, &postamble_0 };

#define test_case_3_5_12_1_stream_0 (&test_3_5_12_1)
#define test_case_3_5_12_1_stream_1 (NULL)
#define test_case_3_5_12_1_stream_2 (NULL)

#define tgrp_case_3_5_12_2 test_group_3_5
#define numb_case_3_5_12_2 "3.5.12.2"
#define name_case_3_5_12_2 "Perform getmsg."
#define sref_case_3_5_12_2 sref_case_3_5
#define desc_case_3_5_12_2 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
high-priority complete control and data part can be retrieved."

int
test_case_3_5_12_2(int child)
{
	return test_case_3_5_12_x(child, RS_HIPRI);
}
struct test_stream test_3_5_12_2 = { &preamble_0, &test_case_3_5_12_2, &postamble_0 };

#define test_case_3_5_12_2_stream_0 (&test_3_5_12_2)
#define test_case_3_5_12_2_stream_1 (NULL)
#define test_case_3_5_12_2_stream_2 (NULL)

int
test_case_3_5_13_x(int child, int flags)
{
	char gcbuf[64] = { 0, };
	char gdbuf[256] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putmsg(child, &pctl, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != MORECTL)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (test_getmsg(child, &gctl, NULL, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)(i + sizeof(gcbuf))) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_5_13_1 test_group_3_5
#define numb_case_3_5_13_1 "3.5.13.1"
#define name_case_3_5_13_1 "Perform getmsg - MORECTL."
#define sref_case_3_5_13_1 sref_case_3_5
#define desc_case_3_5_13_1 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
normal partial control and complete data part can be retrieved."

int
test_case_3_5_13_1(int child)
{
	return test_case_3_5_13_x(child, 0);
}
struct test_stream test_3_5_13_1 = { &preamble_0, &test_case_3_5_13_1, &postamble_0 };

#define test_case_3_5_13_1_stream_0 (&test_3_5_13_1)
#define test_case_3_5_13_1_stream_1 (NULL)
#define test_case_3_5_13_1_stream_2 (NULL)

#define tgrp_case_3_5_13_2 test_group_3_5
#define numb_case_3_5_13_2 "3.5.13.2"
#define name_case_3_5_13_2 "Perform getmsg - MORECTL."
#define sref_case_3_5_13_2 sref_case_3_5
#define desc_case_3_5_13_2 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
high-priority partial control and complete data part can be retrieved."

int
test_case_3_5_13_2(int child)
{
	return test_case_3_5_13_x(child, RS_HIPRI);
}
struct test_stream test_3_5_13_2 = { &preamble_0, &test_case_3_5_13_2, &postamble_0 };

#define test_case_3_5_13_2_stream_0 (&test_3_5_13_2)
#define test_case_3_5_13_2_stream_1 (NULL)
#define test_case_3_5_13_2_stream_2 (NULL)

int
test_case_3_5_14_x(int child, int flags)
{
	char gcbuf[128] = { 0, };
	char gdbuf[128] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putmsg(child, &pctl, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != MOREDATA)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	gflags = 0;
	if (test_getmsg(child, NULL, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pcbuf) = %zu\n", gdat.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)(i + sizeof(gdbuf))) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_5_14_1 test_group_3_5
#define numb_case_3_5_14_1 "3.5.14.1"
#define name_case_3_5_14_1 "Perform getmsg - MOREDATA."
#define sref_case_3_5_14_1 sref_case_3_5
#define desc_case_3_5_14_1 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
normal complete control and partial data part can be retrieved."

int
test_case_3_5_14_1(int child)
{
	return test_case_3_5_14_x(child, 0);
}
struct test_stream test_3_5_14_1 = { &preamble_0, &test_case_3_5_14_1, &postamble_0 };

#define test_case_3_5_14_1_stream_0 (&test_3_5_14_1)
#define test_case_3_5_14_1_stream_1 (NULL)
#define test_case_3_5_14_1_stream_2 (NULL)

#define tgrp_case_3_5_14_2 test_group_3_5
#define numb_case_3_5_14_2 "3.5.14.2"
#define name_case_3_5_14_2 "Perform getmsg - MOREDATA."
#define sref_case_3_5_14_2 sref_case_3_5
#define desc_case_3_5_14_2 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
high-priority complete control and partial data part can be retrieved."

int
test_case_3_5_14_2(int child)
{
	return test_case_3_5_14_x(child, RS_HIPRI);
}
struct test_stream test_3_5_14_2 = { &preamble_0, &test_case_3_5_14_2, &postamble_0 };

#define test_case_3_5_14_2_stream_0 (&test_3_5_14_2)
#define test_case_3_5_14_2_stream_1 (NULL)
#define test_case_3_5_14_2_stream_2 (NULL)

int
test_case_3_5_15_x(int child, int flags)
{
	char gcbuf[64] = { 0, };
	char gdbuf[128] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putmsg(child, &pctl, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != (MORECTL | MOREDATA))
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)(i + sizeof(gcbuf))) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pcbuf) = %zu\n", gdat.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)(i + sizeof(gdbuf))) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_5_15_1 test_group_3_5
#define numb_case_3_5_15_1 "3.5.15.1"
#define name_case_3_5_15_1 "Perform getmsg - MORECTL MOREDATA."
#define sref_case_3_5_15_1 sref_case_3_5
#define desc_case_3_5_15_1 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
normal partial control and partial data part can be retrieved."

int
test_case_3_5_15_1(int child)
{
	return test_case_3_5_15_x(child, 0);
}
struct test_stream test_3_5_15_1 = { &preamble_0, &test_case_3_5_15_1, &postamble_0 };

#define test_case_3_5_15_1_stream_0 (&test_3_5_15_1)
#define test_case_3_5_15_1_stream_1 (NULL)
#define test_case_3_5_15_1_stream_2 (NULL)

#define tgrp_case_3_5_15_2 test_group_3_5
#define numb_case_3_5_15_2 "3.5.15.2"
#define name_case_3_5_15_2 "Perform getmsg - MORECTL MOREDATA."
#define sref_case_3_5_15_2 sref_case_3_5
#define desc_case_3_5_15_2 "\
Check that getmsg() can be performed on a Stream.  Checks that a\n\
high-priority partial control and partial data part can be retrieved."

int
test_case_3_5_15_2(int child)
{
	return test_case_3_5_15_x(child, RS_HIPRI);
}
struct test_stream test_3_5_15_2 = { &preamble_0, &test_case_3_5_15_2, &postamble_0 };

#define test_case_3_5_15_2_stream_0 (&test_3_5_15_2)
#define test_case_3_5_15_2_stream_1 (NULL)
#define test_case_3_5_15_2_stream_2 (NULL)

#define tgrp_case_3_5_16 test_group_3_5
#define numb_case_3_5_16 "3.5.16"
#define name_case_3_5_16 "Perform getmsg - EBADMSG."
#define sref_case_3_5_16 sref_case_3_5
#define desc_case_3_5_16 "\
Check that getmsg() can be performed on a Stream.  Checks that EBADMSG\n\
is returned when an M_PASSFP message is at the head of the Stream head\n\
read queue."

int
test_case_3_5_16(int child)
{
	char buf[16] = { 0, };
	struct strbuf ctl = { sizeof(buf), -1, buf };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int flags = 0;

	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EBADMSG)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_16 = { &preamble_6_3, &test_case_3_5_16, &postamble_6_3 };

#define test_case_3_5_16_stream_0 (&test_3_5_16)
#define test_case_3_5_16_stream_1 (NULL)
#define test_case_3_5_16_stream_2 (NULL)

#define tgrp_case_3_5_17 test_group_3_5
#define numb_case_3_5_17 "3.5.17"
#define name_case_3_5_17 "Perform getmsg - EBADMSG."
#define sref_case_3_5_17 sref_case_3_5
#define desc_case_3_5_17 "\
Check that getmsg() can be performed on a Stream.  Checks that EBADMSG\n\
is returned when the message at the head of the Stream head read queue\n\
is a normal priority message and the flags argument specifies RS_HIPRI."

int
test_case_3_5_17(int child)
{
	char buf[16] = { 0, };
	struct strbuf pctl = { -1, sizeof(buf), buf };
	struct strbuf pdat = { -1, sizeof(buf), buf };
	int pflags = 0;
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gflags = RS_HIPRI;

	if (test_putmsg(child, &pctl, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) == __RESULT_SUCCESS || last_errno != EBADMSG)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_17 = { &preamble_0, &test_case_3_5_17, &postamble_0 };

#define test_case_3_5_17_stream_0 (&test_3_5_17)
#define test_case_3_5_17_stream_1 (NULL)
#define test_case_3_5_17_stream_2 (NULL)

#define tgrp_case_3_5_18 test_group_3_5
#define numb_case_3_5_18 "3.5.18"
#define name_case_3_5_18 "Perform getmsg - ENOSTR."
#define sref_case_3_5_18 sref_case_3_5
#define desc_case_3_5_18 "\
Check that getmsg() can be performed on a Stream.  Checks that ENOSTR\n\
is returned when getmsg() is performed on a file descriptor that does\n\
not correspond to a Stream."

int
test_case_3_5_18(int child)
{
	char buf[16] = { 0, };
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gflags = RS_HIPRI;
	int oldfd = test_fd[child];

	test_fd[child] = 0;
	start_tt(100);
	if (test_getmsg(child, &gctl, &gdat, &gflags) == __RESULT_SUCCESS || last_errno != ENOSTR) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_18 = { &preamble_0, &test_case_3_5_18, &postamble_0 };

#define test_case_3_5_18_stream_0 (&test_3_5_18)
#define test_case_3_5_18_stream_1 (NULL)
#define test_case_3_5_18_stream_2 (NULL)

#define tgrp_case_3_5_19 test_group_3_5
#define numb_case_3_5_19 "3.5.19"
#define name_case_3_5_19 "Perform getmsg - ENODEV."
#define sref_case_3_5_19 sref_case_3_5
#define desc_case_3_5_19 "\
Check that getmsg() can be performed on a Stream.  Checks that ENODEV\n\
is returned when getmsg() is performed on a file descriptor associated\n\
with a device that does not support the getmsg() system call."

int
test_case_3_5_19(int child)
{
	char buf[16] = { 0, };
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gflags = RS_HIPRI;
	int oldfd = test_fd[child];

	test_fd[child] = 0;
	start_tt(100);
	if (test_getmsg(child, &gctl, &gdat, &gflags) == __RESULT_SUCCESS || last_errno != ENODEV) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_19 = { &preamble_0, &test_case_3_5_19, &postamble_0 };

#define test_case_3_5_19_stream_0 (&test_3_5_19)
#define test_case_3_5_19_stream_1 (NULL)
#define test_case_3_5_19_stream_2 (NULL)

#define tgrp_case_3_5_20 test_group_3_5
#define numb_case_3_5_20 "3.5.20"
#define name_case_3_5_20 "Perform getmsg."
#define sref_case_3_5_20 sref_case_3_5
#define desc_case_3_5_20 "\
Check that getmsg() can be performed on a Stream.  Checks that\n\
zero-length control and data parts will be retrieved under the proper\n\
circumstances."

int
test_case_3_5_20(int child)
{
	struct strbuf pctl = { -1, 0, NULL };
	struct strbuf pdat = { -1, 0, NULL };
	int pflags = 0;

	char buf[16] = { 0, };
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gflags = 0;

	/* can't send zero length control? */
	if (test_putmsg(child, &pctl, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	gctl.maxlen = -1;
	gdat.maxlen = -1;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != (MORECTL | MOREDATA))
		return (__RESULT_FAILURE);
	state++;
	gctl.maxlen = 0;
	gdat.maxlen = -1;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != MOREDATA)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != -1)
		return (__RESULT_FAILURE);
	state++;
	gctl.maxlen = sizeof(buf);
	gdat.maxlen = sizeof(buf);
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != -1)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_20 = { &preamble_0, &test_case_3_5_20, &postamble_0 };

#define test_case_3_5_20_stream_0 (&test_3_5_20)
#define test_case_3_5_20_stream_1 (NULL)
#define test_case_3_5_20_stream_2 (NULL)

#define tgrp_case_3_5_21 test_group_3_5
#define numb_case_3_5_21 "3.5.21"
#define name_case_3_5_21 "Perform getmsg."
#define sref_case_3_5_21 sref_case_3_5
#define desc_case_3_5_21 "\
Check that getmsg() can be performed on a Stream.  Checks that\n\
messages are retrieved in the correct order."

int
preamble_test_case_3_5_21(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int i = 0;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	do {
		last_errno = 0;
		test_putpmsg(child, &ctl, &dat, 0, MSG_BAND);
		if (last_errno != 0 && last_errno != EAGAIN)
			return (__RESULT_FAILURE);
		state++;
		if (last_errno == 0)
			i++;
		last_errno = 0;
		test_putpmsg(child, &ctl, &dat, 1, MSG_BAND);
		if (last_errno != 0 && last_errno != EAGAIN)
			return (__RESULT_FAILURE);
		state++;
		if (last_errno == 0)
			i++;
		last_errno = 0;
		test_putpmsg(child, &ctl, &dat, 2, MSG_BAND);
		if (last_errno != 0 && last_errno != EAGAIN)
			return (__RESULT_FAILURE);
		state++;
		if (last_errno == 0)
			i++;
	} while (last_errno != EAGAIN);
	last_errno = 0;
	test_putpmsg(child, &ctl, &dat, 0, MSG_HIPRI);
	if (last_errno != 0)
		return (__RESULT_FAILURE);
	state++;
	i++;
	if (i != 19)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_5_21(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { sizeof(cbuf), -1, cbuf };
	struct strbuf dat = { sizeof(dbuf), -1, dbuf };
	int flags = 0;
	int i = 0;

	flags = 0;
	if (test_getmsg(child, &ctl, &dat, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (flags != RS_HIPRI)
		return (__RESULT_FAILURE);
	state++;
	i++;
	for (;;) {
		flags = 0;
		if (test_getmsg(child, &ctl, &dat, &flags) != __RESULT_SUCCESS) {
			if (last_errno == EAGAIN)
				break;
			return (__RESULT_FAILURE);
		}
		state++;
		if (flags != 0)
			return (__RESULT_FAILURE);
		state++;
		i++;
	}
	state++;
	if (i != 19)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_5_21 = { &preamble_test_case_3_5_21, &test_case_3_5_21, &postamble_0 };

#define test_case_3_5_21_stream_0 (&test_3_5_21)
#define test_case_3_5_21_stream_1 (NULL)
#define test_case_3_5_21_stream_2 (NULL)

static const char test_group_3_6[] = "Perform GETPMSG on one Stream";
static const char sref_case_3_6[] = "POSIX 1003.1 2003/SUSv3 getpmsg(2p) reference page.";

#define tgrp_case_3_6_1 test_group_3_6
#define numb_case_3_6_1 "3.6.1"
#define name_case_3_6_1 "Perform getpmsg."
#define sref_case_3_6_1 sref_case_3_6
#define desc_case_3_6_1 "\
Check that getpmsg() can be performed on a Stream.  Checks that zero is\n\
returned for both the control part size and data part size, indicating a\n\
hung up Stream, when getpmsg() is called on a hung up Stream."

int
test_case_3_6_1(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (ctl.len != 0 || dat.len != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_1 = { &preamble_2_1, &test_case_3_6_1, &postamble_2 };

#define test_case_3_6_1_stream_0 (&test_3_6_1)
#define test_case_3_6_1_stream_1 (NULL)
#define test_case_3_6_1_stream_2 (NULL)

#define tgrp_case_3_6_2 test_group_3_6
#define numb_case_3_6_2 "3.6.2"
#define name_case_3_6_2 "Perform getpmsg - EPROTO."
#define sref_case_3_6_2 sref_case_3_6
#define desc_case_3_6_2 "\
Check that getpmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when getpmsg() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO read error."

int
test_case_3_6_2(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_2 = { &preamble_2_2, &test_case_3_6_2, &postamble_2 };

#define test_case_3_6_2_stream_0 (&test_3_6_2)
#define test_case_3_6_2_stream_1 (NULL)
#define test_case_3_6_2_stream_2 (NULL)

#define tgrp_case_3_6_3 test_group_3_6
#define numb_case_3_6_3 "3.6.3"
#define name_case_3_6_3 "Perform getpmsg - EAGAIN."
#define sref_case_3_6_3 sref_case_3_6
#define desc_case_3_6_3 "\
Check that getpmsg() can be performed on a Stream.  Checks that EAGAIN is\n\
returned when getpmsg() is attempted on a Stream that has received an\n\
asynchronous EPROTO write error and the Stream is set for non-blocking\n\
operation."

int
test_case_3_6_3(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_3 = { &preamble_2_3, &test_case_3_6_3, &postamble_2 };

#define test_case_3_6_3_stream_0 (&test_3_6_3)
#define test_case_3_6_3_stream_1 (NULL)
#define test_case_3_6_3_stream_2 (NULL)

#define tgrp_case_3_6_4 test_group_3_6
#define numb_case_3_6_4 "3.6.4"
#define name_case_3_6_4 "Perform getpmsg - EPROTO."
#define sref_case_3_6_4 sref_case_3_6
#define desc_case_3_6_4 "\
Check that getpmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when getpmsg() is attempted on a Stream that has received an\n\
asynchronous EPROTO error."

int
test_case_3_6_4(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_4 = { &preamble_2_4, &test_case_3_6_4, &postamble_2 };

#define test_case_3_6_4_stream_0 (&test_3_6_4)
#define test_case_3_6_4_stream_1 (NULL)
#define test_case_3_6_4_stream_2 (NULL)

#define tgrp_case_3_6_5 test_group_3_6
#define numb_case_3_6_5 "3.6.5"
#define name_case_3_6_5 "Perform getpmsg - EINVAL."
#define sref_case_3_6_5 sref_case_3_6
#define desc_case_3_6_5 "\
Check that getpmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when getpmsg() is attempted on a Stream is linked under a\n\
Multiplexing Driver."

int
test_case_3_6_5(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_5 = { &preamble_5, &test_case_3_6_5, &postamble_5 };

#define test_case_3_6_5_stream_0 (&test_3_6_5)
#define test_case_3_6_5_stream_1 (NULL)
#define test_case_3_6_5_stream_2 (NULL)

#define tgrp_case_3_6_6 test_group_3_6
#define numb_case_3_6_6 "3.6.6"
#define name_case_3_6_6 "Perform getpmsg - EAGAIN."
#define sref_case_3_6_6 sref_case_3_6
#define desc_case_3_6_6 "\
Check that getpmsg() can be performed on a Stream.  Checks that EAGAIN is\n\
returned when the O_NONBLOCK flag is set for the file descriptor and the\n\
process would be delayed."

int
test_case_3_6_6(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_6 = { &preamble_0, &test_case_3_6_6, &postamble_0 };

#define test_case_3_6_6_stream_0 (&test_3_6_6)
#define test_case_3_6_6_stream_1 (NULL)
#define test_case_3_6_6_stream_2 (NULL)

#define tgrp_case_3_6_7 test_group_3_6
#define numb_case_3_6_7 "3.6.7"
#define name_case_3_6_7 "Perform getpmsg - EBADF."
#define sref_case_3_6_7 sref_case_3_6
#define desc_case_3_6_7 "\
Check that getpmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not a valid file descriptor."

int
test_case_3_6_7(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];
	int oldfd = test_fd[child];

	test_fd[child] = -1;

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_7 = { &preamble_0, &test_case_3_6_7, &postamble_0 };

#define test_case_3_6_7_stream_0 (&test_3_6_7)
#define test_case_3_6_7_stream_1 (NULL)
#define test_case_3_6_7_stream_2 (NULL)

#define tgrp_case_3_6_8 test_group_3_6
#define numb_case_3_6_8 "3.6.8"
#define name_case_3_6_8 "Perform getpmsg - EBADF."
#define sref_case_3_6_8 sref_case_3_6
#define desc_case_3_6_8 "\
Check that getpmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when the fildes argument is not open for reading."

int
test_case_3_6_8(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_8 = { &preamble_0_2, &test_case_3_6_8, &postamble_0 };

#define test_case_3_6_8_stream_0 (&test_3_6_8)
#define test_case_3_6_8_stream_1 (NULL)
#define test_case_3_6_8_stream_2 (NULL)

#define tgrp_case_3_6_9 test_group_3_6
#define numb_case_3_6_9 "3.6.9"
#define name_case_3_6_9 "Perform getpmsg - EINVAL."
#define sref_case_3_6_9 sref_case_3_6
#define desc_case_3_6_9 "\
Check that getpmsg() can be performed on a Stream.  Checks that EINVAL\n\
is returned when the flagsp argument points to flags that contain an\n\
invalid value (other than MSG_HIPRI, MSG_BAND or MSG_ANY)."

int
test_case_3_6_9(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];


	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY + 1;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_9 = { &preamble_0, &test_case_3_6_9, &postamble_0 };

#define test_case_3_6_9_stream_0 (&test_3_6_9)
#define test_case_3_6_9_stream_1 (NULL)
#define test_case_3_6_9_stream_2 (NULL)

#define tgrp_case_3_6_10 test_group_3_6
#define numb_case_3_6_10 "3.6.10"
#define name_case_3_6_10 "Perform getpmsg - EINVAL."
#define sref_case_3_6_10 sref_case_3_6
#define desc_case_3_6_10 "\
Check that getpmsg() can be performed on a Stream.  Checks that EINVAL\n\
is returned when the bandp argument points to a band that contains an\n\
invalid value (greater than 255 or less than 0) when flags are MSG_BAND."

int
test_case_3_6_10(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];


	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 256;
	flags = MSG_BAND;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_10 = { &preamble_0, &test_case_3_6_10, &postamble_0 };

#define test_case_3_6_10_stream_0 (&test_3_6_10)
#define test_case_3_6_10_stream_1 (NULL)
#define test_case_3_6_10_stream_2 (NULL)

#define tgrp_case_3_6_11 test_group_3_6
#define numb_case_3_6_11 "3.6.11"
#define name_case_3_6_11 "Perform getpmsg - EINVAL."
#define sref_case_3_6_11 sref_case_3_6
#define desc_case_3_6_11 "\
Check that getpmsg() can be performed on a Stream.  Checks that EINVAL\n\
is returned when the bandp argument points to a band that contains an\n\
invalid value (other than 0) when flags are MSG_ANY."

int
test_case_3_6_11(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];


	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 1;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_11 = { &preamble_0, &test_case_3_6_11, &postamble_0 };

#define test_case_3_6_11_stream_0 (&test_3_6_11)
#define test_case_3_6_11_stream_1 (NULL)
#define test_case_3_6_11_stream_2 (NULL)

#define tgrp_case_3_6_12 test_group_3_6
#define numb_case_3_6_12 "3.6.12"
#define name_case_3_6_12 "Perform getpmsg - EINVAL."
#define sref_case_3_6_12 sref_case_3_6
#define desc_case_3_6_12 "\
Check that getpmsg() can be performed on a Stream.  Checks that EINVAL\n\
is returned when the bandp argument points to a band that contains an\n\
invalid value (other than 0) when flags are MSG_HIPRI."

int
test_case_3_6_12(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];


	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 1;
	flags = MSG_HIPRI;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_12 = { &preamble_0, &test_case_3_6_12, &postamble_0 };

#define test_case_3_6_12_stream_0 (&test_3_6_12)
#define test_case_3_6_12_stream_1 (NULL)
#define test_case_3_6_12_stream_2 (NULL)

#define tgrp_case_3_6_13 test_group_3_6
#define numb_case_3_6_13 "3.6.13"
#define name_case_3_6_13 "Perform getpmsg - EINVAL."
#define sref_case_3_6_13 sref_case_3_6
#define desc_case_3_6_13 "\
Check that getpmsg() can be performed on a Stream.  Checks that EINVAL\n\
is returned when the flagsp argument points to flags that contain an\n\
invalid value (other than MSG_HIPRI, MSG_BAND or MSG_ANY), in this case zero."

int
test_case_3_6_13(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];


	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = 0;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_13 = { &preamble_0, &test_case_3_6_13, &postamble_0 };

#define test_case_3_6_13_stream_0 (&test_3_6_13)
#define test_case_3_6_13_stream_1 (NULL)
#define test_case_3_6_13_stream_2 (NULL)

#define tgrp_case_3_6_14 test_group_3_6
#define numb_case_3_6_14 "3.6.14"
#define name_case_3_6_14 "Perform getpmsg - EFAULT."
#define sref_case_3_6_14 sref_case_3_6
#define desc_case_3_6_14 "\
Check that getpmsg() can be performed on a Stream.  Checks that EFAULT\n\
is returned when a buffer points outside the callers valid address space."

int
preamble_test_case_3_6_14(int child)
{
	char buf[16];

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_write(child, buf, sizeof(buf)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_3_6_14(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;


	ctl.maxlen = 5;
	ctl.len = -1;
	ctl.buf = (char *)INVALID_ADDRESS;

	dat.maxlen = 5;
	dat.len = -1;
	dat.buf = (char *)INVALID_ADDRESS;

	band = 0;
	flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_14 = { &preamble_test_case_3_6_14, &test_case_3_6_14, &postamble_0 };

#define test_case_3_6_14_stream_0 (&test_3_6_14)
#define test_case_3_6_14_stream_1 (NULL)
#define test_case_3_6_14_stream_2 (NULL)

#define tgrp_case_3_6_15 test_group_3_6
#define numb_case_3_6_15 "3.6.15"
#define name_case_3_6_15 "Perform getpmsg - EINTR."
#define sref_case_3_6_15 sref_case_3_6
#define desc_case_3_6_15 "\
Check that getpmsg() can be performed on a Stream.  Checks that\n\
getpmsg() will block awaiting data at the Stream head, and that the wait\n\
can be interrupted by a signal and getpmsg() will return EINTR."

int
test_case_3_6_15(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[1];
	char dbuf[1];

	ctl.maxlen = 1;
	ctl.len = -1;
	ctl.buf = cbuf;

	dat.maxlen = 1;
	dat.len = -1;
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_block(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_15 = { &preamble_0, &test_case_3_6_15, &postamble_0 };

#define test_case_3_6_15_stream_0 (&test_3_6_15)
#define test_case_3_6_15_stream_1 (NULL)
#define test_case_3_6_15_stream_2 (NULL)

int
test_case_3_6_16_x(int child, int flags)
{
	char gcbuf[128] = { 0, };
	char gdbuf[256] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gband = (flags == MSG_BAND) ? 1 : 0;
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = (flags == MSG_BAND) ? 1 : 0;
	int pflags = (flags == MSG_ANY) ? MSG_BAND : flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(pcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	if (gdat.len != sizeof(pdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_6_16_1 test_group_3_6
#define numb_case_3_6_16_1 "3.6.16.1"
#define name_case_3_6_16_1 "Perform getpmsg - MSG_ANY."
#define sref_case_3_6_16_1 sref_case_3_6
#define desc_case_3_6_16_1 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_16_1(int child)
{
	return test_case_3_6_16_x(child, MSG_ANY);
}
struct test_stream test_3_6_16_1 = { &preamble_0, &test_case_3_6_16_1, &postamble_0 };

#define test_case_3_6_16_1_stream_0 (&test_3_6_16_1)
#define test_case_3_6_16_1_stream_1 (NULL)
#define test_case_3_6_16_1_stream_2 (NULL)

#define tgrp_case_3_6_16_2 test_group_3_6
#define numb_case_3_6_16_2 "3.6.16.2"
#define name_case_3_6_16_2 "Perform getpmsg - MSG_BAND."
#define sref_case_3_6_16_2 sref_case_3_6
#define desc_case_3_6_16_2 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_16_2(int child)
{
	return test_case_3_6_16_x(child, MSG_BAND);
}
struct test_stream test_3_6_16_2 = { &preamble_0, &test_case_3_6_16_2, &postamble_0 };

#define test_case_3_6_16_2_stream_0 (&test_3_6_16_2)
#define test_case_3_6_16_2_stream_1 (NULL)
#define test_case_3_6_16_2_stream_2 (NULL)

#define tgrp_case_3_6_16_3 test_group_3_6
#define numb_case_3_6_16_3 "3.6.16.3"
#define name_case_3_6_16_3 "Perform getpmsg - MSG_HIPRI."
#define sref_case_3_6_16_3 sref_case_3_6
#define desc_case_3_6_16_3 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_16_3(int child)
{
	return test_case_3_6_16_x(child, MSG_HIPRI);
}
struct test_stream test_3_6_16_3 = { &preamble_0, &test_case_3_6_16_3, &postamble_0 };

#define test_case_3_6_16_3_stream_0 (&test_3_6_16_3)
#define test_case_3_6_16_3_stream_1 (NULL)
#define test_case_3_6_16_3_stream_2 (NULL)

int
test_case_3_6_17_x(int child, int flags)
{
	char gcbuf[64] = { 0, };
	char gdbuf[256] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gband = (flags == MSG_BAND) ? 1 : 0;
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = (flags == MSG_BAND) ? 1 : 0;
	int pflags = (flags == MSG_ANY) ? MSG_BAND : flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != MORECTL)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	gband = (flags == MSG_BAND) ? 1 : 0;
	gflags = flags;
	if (test_getpmsg(child, &gctl, NULL, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)(i + sizeof(gcbuf))) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_6_17_1 test_group_3_6
#define numb_case_3_6_17_1 "3.6.17.1"
#define name_case_3_6_17_1 "Perform getpmsg - MORECTL MSG_ANY."
#define sref_case_3_6_17_1 sref_case_3_6
#define desc_case_3_6_17_1 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_17_1(int child)
{
	return test_case_3_6_17_x(child, MSG_ANY);
}
struct test_stream test_3_6_17_1 = { &preamble_0, &test_case_3_6_17_1, &postamble_0 };

#define test_case_3_6_17_1_stream_0 (&test_3_6_17_1)
#define test_case_3_6_17_1_stream_1 (NULL)
#define test_case_3_6_17_1_stream_2 (NULL)

#define tgrp_case_3_6_17_2 test_group_3_6
#define numb_case_3_6_17_2 "3.6.17.2"
#define name_case_3_6_17_2 "Perform getpmsg - MORECTL MSG_BAND."
#define sref_case_3_6_17_2 sref_case_3_6
#define desc_case_3_6_17_2 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_17_2(int child)
{
	return test_case_3_6_17_x(child, MSG_BAND);
}
struct test_stream test_3_6_17_2 = { &preamble_0, &test_case_3_6_17_2, &postamble_0 };

#define test_case_3_6_17_2_stream_0 (&test_3_6_17_2)
#define test_case_3_6_17_2_stream_1 (NULL)
#define test_case_3_6_17_2_stream_2 (NULL)

#define tgrp_case_3_6_17_3 test_group_3_6
#define numb_case_3_6_17_3 "3.6.17.3"
#define name_case_3_6_17_3 "Perform getpmsg - MORECTL MSG_HIPRI."
#define sref_case_3_6_17_3 sref_case_3_6
#define desc_case_3_6_17_3 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_17_3(int child)
{
	return test_case_3_6_17_x(child, MSG_HIPRI);
}
struct test_stream test_3_6_17_3 = { &preamble_0, &test_case_3_6_17_3, &postamble_0 };

#define test_case_3_6_17_3_stream_0 (&test_3_6_17_3)
#define test_case_3_6_17_3_stream_1 (NULL)
#define test_case_3_6_17_3_stream_2 (NULL)

int
test_case_3_6_18_x(int child, int flags)
{
	char gcbuf[128] = { 0, };
	char gdbuf[128] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gband = (flags == MSG_BAND) ? 1 : 0;
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = (flags == MSG_BAND) ? 1 : 0;
	int pflags = (flags == MSG_ANY) ? MSG_BAND : flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != MOREDATA)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	gband = 0;
	gflags = MSG_ANY;
	if (test_getpmsg(child, NULL, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pcbuf) = %zu\n", gdat.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)(i + sizeof(gdbuf))) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_6_18_1 test_group_3_6
#define numb_case_3_6_18_1 "3.6.18.1"
#define name_case_3_6_18_1 "Perform getpmsg - MOREDATA MSG_ANY."
#define sref_case_3_6_18_1 sref_case_3_6
#define desc_case_3_6_18_1 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_18_1(int child)
{
	return test_case_3_6_18_x(child, MSG_ANY);
}
struct test_stream test_3_6_18_1 = { &preamble_0, &test_case_3_6_18_1, &postamble_0 };

#define test_case_3_6_18_1_stream_0 (&test_3_6_18_1)
#define test_case_3_6_18_1_stream_1 (NULL)
#define test_case_3_6_18_1_stream_2 (NULL)

#define tgrp_case_3_6_18_2 test_group_3_6
#define numb_case_3_6_18_2 "3.6.18.2"
#define name_case_3_6_18_2 "Perform getpmsg - MOREDATA MSG_BAND."
#define sref_case_3_6_18_2 sref_case_3_6
#define desc_case_3_6_18_2 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_18_2(int child)
{
	return test_case_3_6_18_x(child, MSG_BAND);
}
struct test_stream test_3_6_18_2 = { &preamble_0, &test_case_3_6_18_2, &postamble_0 };

#define test_case_3_6_18_2_stream_0 (&test_3_6_18_2)
#define test_case_3_6_18_2_stream_1 (NULL)
#define test_case_3_6_18_2_stream_2 (NULL)

#define tgrp_case_3_6_18_3 test_group_3_6
#define numb_case_3_6_18_3 "3.6.18.3"
#define name_case_3_6_18_3 "Perform getpmsg - MOREDATA MSG_HIPRI."
#define sref_case_3_6_18_3 sref_case_3_6
#define desc_case_3_6_18_3 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_18_3(int child)
{
	return test_case_3_6_18_x(child, MSG_HIPRI);
}
struct test_stream test_3_6_18_3 = { &preamble_0, &test_case_3_6_18_3, &postamble_0 };

#define test_case_3_6_18_3_stream_0 (&test_3_6_18_3)
#define test_case_3_6_18_3_stream_1 (NULL)
#define test_case_3_6_18_3_stream_2 (NULL)

int
test_case_3_6_19_x(int child, int flags)
{
	char gcbuf[64] = { 0, };
	char gdbuf[128] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gband = (flags == MSG_BAND) ? 1 : 0;
	int gflags = flags;

	char pcbuf[128] = { 0, };
	char pdbuf[256] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = (flags == MSG_BAND) ? 1 : 0;
	int pflags = (flags == MSG_ANY) ? MSG_BAND : flags;

	int i;

	/* put known data in buffers */
	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = i;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != (MORECTL | MOREDATA))
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pdbuf) = %zu\n", gdat.len, sizeof(pdbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)i) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)i) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(gcbuf)) {
		printf("gctl.len = %d, sizeof(pcbuf) = %zu\n", gctl.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gcbuf); i++)
		if ((unsigned char)gcbuf[i] != (unsigned char)(i + sizeof(gcbuf))) {
			printf("gcbuf differs at byte %d, value %d\n", i, (int)gcbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	if (gdat.len != sizeof(gdbuf)) {
		printf("gdat.len = %d, sizeof(pcbuf) = %zu\n", gdat.len, sizeof(pcbuf));
		return (__RESULT_FAILURE);
	}
	state++;
	for (i = 0; i < sizeof(gdbuf); i++)
		if ((unsigned char)gdbuf[i] != (unsigned char)(i + sizeof(gdbuf))) {
			printf("gdbuf differs at byte %d, value %d\n", i, (int)gdbuf[i]);
			return (__RESULT_FAILURE);
		}
	state++;
	return (__RESULT_SUCCESS);
}

#define tgrp_case_3_6_19_1 test_group_3_6
#define numb_case_3_6_19_1 "3.6.19.1"
#define name_case_3_6_19_1 "Perform getpmsg - MORECTL MOREDATA MSG_ANY."
#define sref_case_3_6_19_1 sref_case_3_6
#define desc_case_3_6_19_1 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_19_1(int child)
{
	return test_case_3_6_19_x(child, MSG_ANY);
}
struct test_stream test_3_6_19_1 = { &preamble_0, &test_case_3_6_19_1, &postamble_0 };

#define test_case_3_6_19_1_stream_0 (&test_3_6_19_1)
#define test_case_3_6_19_1_stream_1 (NULL)
#define test_case_3_6_19_1_stream_2 (NULL)

#define tgrp_case_3_6_19_2 test_group_3_6
#define numb_case_3_6_19_2 "3.6.19.2"
#define name_case_3_6_19_2 "Perform getpmsg - MORECTL MOREDATA MSG_BAND."
#define sref_case_3_6_19_2 sref_case_3_6
#define desc_case_3_6_19_2 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_19_2(int child)
{
	return test_case_3_6_19_x(child, MSG_BAND);
}
struct test_stream test_3_6_19_2 = { &preamble_0, &test_case_3_6_19_2, &postamble_0 };

#define test_case_3_6_19_2_stream_0 (&test_3_6_19_2)
#define test_case_3_6_19_2_stream_1 (NULL)
#define test_case_3_6_19_2_stream_2 (NULL)

#define tgrp_case_3_6_19_3 test_group_3_6
#define numb_case_3_6_19_3 "3.6.19.3"
#define name_case_3_6_19_3 "Perform getpmsg - MORECTL MOREDATA MSG_HIPRI."
#define sref_case_3_6_19_3 sref_case_3_6
#define desc_case_3_6_19_3 "\
Check that getpmsg() can be performed on a Stream."

int
test_case_3_6_19_3(int child)
{
	return test_case_3_6_19_x(child, MSG_HIPRI);
}
struct test_stream test_3_6_19_3 = { &preamble_0, &test_case_3_6_19_3, &postamble_0 };

#define test_case_3_6_19_3_stream_0 (&test_3_6_19_3)
#define test_case_3_6_19_3_stream_1 (NULL)
#define test_case_3_6_19_3_stream_2 (NULL)

#define tgrp_case_3_6_20 test_group_3_6
#define numb_case_3_6_20 "3.6.20"
#define name_case_3_6_20 "Perform getpmsg - EBADMSG."
#define sref_case_3_6_20 sref_case_3_6
#define desc_case_3_6_20 "\
Check that getpmsg() can be performed on a Stream.  Checks that EBADMSG\n\
is returned when an M_PASSFP message is at the head of the Stream head\n\
read queue."

int
test_case_3_6_20(int child)
{
	char buf[16] = { 0, };
	struct strbuf ctl = { sizeof(buf), -1, buf };
	struct strbuf dat = { sizeof(buf), -1, buf };
	int band = 0;
	int flags = MSG_ANY;

	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EBADMSG)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_20 = { &preamble_6_3, &test_case_3_6_20, &postamble_6_3 };

#define test_case_3_6_20_stream_0 (&test_3_6_20)
#define test_case_3_6_20_stream_1 (NULL)
#define test_case_3_6_20_stream_2 (NULL)

#define tgrp_case_3_6_21 test_group_3_6
#define numb_case_3_6_21 "3.6.21"
#define name_case_3_6_21 "Perform getpmsg - EBADMSG."
#define sref_case_3_6_21 sref_case_3_6
#define desc_case_3_6_21 "\
Check that getpmsg() can be performed on a Stream.  Checks taht EBADMSG\n\
is returned when the message at the head of the Stream head read queue\n\
is a normal priority message and the flags argument specifies MSG_HIPRI."

int
test_case_3_6_21(int child)
{
	char buf[16] = { 0, };
	struct strbuf pctl = { -1, sizeof(buf), buf };
	struct strbuf pdat = { -1, sizeof(buf), buf };
	int pband = 0;
	int pflags = MSG_BAND;
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gband = 0;
	int gflags = MSG_HIPRI;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) == __RESULT_SUCCESS || last_errno != EBADMSG)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_21 = { &preamble_0, &test_case_3_6_21, &postamble_0 };

#define test_case_3_6_21_stream_0 (&test_3_6_21)
#define test_case_3_6_21_stream_1 (NULL)
#define test_case_3_6_21_stream_2 (NULL)

#define tgrp_case_3_6_22 test_group_3_6
#define numb_case_3_6_22 "3.6.22"
#define name_case_3_6_22 "Perform getpmsg - EBADMSG."
#define sref_case_3_6_22 sref_case_3_6
#define desc_case_3_6_22 "\
Check that getpmsg() can be performed on a Stream.  Checks that EBADMSG\n\
is returned when the message at the head of the Stream head read queue\n\
is a lower normal priority message than that requested with the MSG_BAND\n\
and bandp arguments."

int
test_case_3_6_22(int child)
{
	char buf[16] = { 0, };
	struct strbuf pctl = { -1, sizeof(buf), buf };
	struct strbuf pdat = { -1, sizeof(buf), buf };
	int pband = 0;
	int pflags = MSG_BAND;
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gband = 1;
	int gflags = MSG_BAND;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) == __RESULT_SUCCESS || last_errno != EBADMSG)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_22 = { &preamble_0, &test_case_3_6_22, &postamble_0 };

#define test_case_3_6_22_stream_0 (&test_3_6_22)
#define test_case_3_6_22_stream_1 (NULL)
#define test_case_3_6_22_stream_2 (NULL)

#define tgrp_case_3_6_23 test_group_3_6
#define numb_case_3_6_23 "3.6.23"
#define name_case_3_6_23 "Perform getpmsg - ENOSTR."
#define sref_case_3_6_23 sref_case_3_6
#define desc_case_3_6_23 "\
Check that getpmsg() can be performed on a Stream.  Check that ENOSTR is\n\
returned when no Stream is associated with the file descriptor."

int
test_case_3_6_23(int child)
{
	char buf[16] = { 0, };
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gband = 0;
	int gflags = MSG_ANY;
	int oldfd = test_fd[child];

	test_fd[child] = 0;
	start_tt(100);
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) == __RESULT_SUCCESS || last_errno != ENOSTR) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_23 = { &preamble_0, &test_case_3_6_23, &postamble_0 };

#define test_case_3_6_23_stream_0 (&test_3_6_23)
#define test_case_3_6_23_stream_1 (NULL)
#define test_case_3_6_23_stream_2 (NULL)

#define tgrp_case_3_6_24 test_group_3_6
#define numb_case_3_6_24 "3.6.24"
#define name_case_3_6_24 "Perform getpmsg - ENODEV."
#define sref_case_3_6_24 sref_case_3_6
#define desc_case_3_6_24 "\
Check that getpmsg() can be performed on a Stream.  Check that ENODEV is\n\
returned when the file descriptor is a device that does not support the\n\
getpmsg() system call."

int
test_case_3_6_24(int child)
{
	char buf[16] = { 0, };
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gband = 0;
	int gflags = MSG_ANY;
	int oldfd = test_fd[child];

	test_fd[child] = 0;
	start_tt(100);
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) == __RESULT_SUCCESS || last_errno != ENODEV) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_24 = { &preamble_0, &test_case_3_6_24, &postamble_0 };

#define test_case_3_6_24_stream_0 (&test_3_6_24)
#define test_case_3_6_24_stream_1 (NULL)
#define test_case_3_6_24_stream_2 (NULL)

#define tgrp_case_3_6_25 test_group_3_6
#define numb_case_3_6_25 "3.6.25"
#define name_case_3_6_25 "Perform getpmsg."
#define sref_case_3_6_25 sref_case_3_6
#define desc_case_3_6_25 "\
Check that getpmsg() can be performed on a Stream.  Checks that\n\
zero-length control and data parts will be retrieved under the proper\n\
circumstances."

int
test_case_3_6_25(int child)
{
	struct strbuf pctl = { -1, 0, NULL };
	struct strbuf pdat = { -1, 0, NULL };
	int pband = 0;
	int pflags = MSG_BAND;

	char buf[16] = { 0, };
	struct strbuf gctl = { sizeof(buf), -1, buf };
	struct strbuf gdat = { sizeof(buf), -1, buf };
	int gband = 0;
	int gflags = MSG_ANY;

	/* can't send zero length control? */
	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_INCONCLUSIVE);
	state++;
	gctl.maxlen = -1;
	gdat.maxlen = -1;
	gband = 0;
	gflags = MSG_ANY;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != (MORECTL | MOREDATA))
		return (__RESULT_FAILURE);
	state++;
	gctl.maxlen = 0;
	gdat.maxlen = -1;
	gband = 0;
	gflags = MSG_ANY;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != MOREDATA)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != -1)
		return (__RESULT_FAILURE);
	state++;
	gctl.maxlen = sizeof(buf);
	gdat.maxlen = sizeof(buf);
	gband = 0;
	gflags = MSG_ANY;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != -1)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_25 = { &preamble_0, &test_case_3_6_25, &postamble_0 };

#define test_case_3_6_25_stream_0 (&test_3_6_25)
#define test_case_3_6_25_stream_1 (NULL)
#define test_case_3_6_25_stream_2 (NULL)

#define tgrp_case_3_6_26 test_group_3_6
#define numb_case_3_6_26 "3.6.26"
#define name_case_3_6_26 "Perform getpmsg."
#define sref_case_3_6_26 sref_case_3_6
#define desc_case_3_6_26 "\
Check that getpmsg() can be performed on a Stream.  Checks that messages\n\
are retrieved in the correct order."

int
preamble_test_case_3_6_26(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { -1, sizeof(cbuf), cbuf };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int i = 0;

	if (preamble_0(child) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	do {
		last_errno = 0;
		test_putpmsg(child, &ctl, &dat, 0, MSG_BAND);
		if (last_errno != 0 && last_errno != EAGAIN)
			return (__RESULT_FAILURE);
		state++;
		if (last_errno == 0)
			i++;
		last_errno = 0;
		test_putpmsg(child, &ctl, &dat, 1, MSG_BAND);
		if (last_errno != 0 && last_errno != EAGAIN)
			return (__RESULT_FAILURE);
		state++;
		if (last_errno == 0)
			i++;
		last_errno = 0;
		test_putpmsg(child, &ctl, &dat, 2, MSG_BAND);
		if (last_errno != 0 && last_errno != EAGAIN)
			return (__RESULT_FAILURE);
		state++;
		if (last_errno == 0)
			i++;
	} while (last_errno != EAGAIN);
	last_errno = 0;
	test_putpmsg(child, &ctl, &dat, 0, MSG_HIPRI);
	if (last_errno != 0)
		return (__RESULT_FAILURE);
	state++;
	i++;
	if (i != 19)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
int
test_case_3_6_26(int child)
{
	char cbuf[1024] = { 0, };
	char dbuf[1024] = { 0, };
	struct strbuf ctl = { sizeof(cbuf), -1, cbuf };
	struct strbuf dat = { sizeof(dbuf), -1, dbuf };
	int band = 0;
	int flags = MSG_ANY;
	int i, k;

	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (flags != MSG_HIPRI)
		return (__RESULT_FAILURE);
	state++;
	for (k = 0; k < 3; k++) {
		for (i = 0; i < 6; i++) {
			band = 0;
			flags = MSG_ANY;
			if (test_getpmsg(child, &ctl, &dat, &band, &flags) != __RESULT_SUCCESS)
				return (__RESULT_FAILURE);
			state++;
			if (band != (2 - k))
				return (__RESULT_FAILURE);
			state++;
			if (flags != MSG_BAND)
				return (__RESULT_FAILURE);
			state++;
		}
	}
	state++;
	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_6_26 = { &preamble_test_case_3_6_26, &test_case_3_6_26, &postamble_0 };

#define test_case_3_6_26_stream_0 (&test_3_6_26)
#define test_case_3_6_26_stream_1 (NULL)
#define test_case_3_6_26_stream_2 (NULL)

static const char test_group_3_7[] = "Perform PUTMSG on one Stream";
static const char sref_case_3_7[] = "POSIX 1003.1 2003/SUSv3 putmsg(2p) reference page.";

#define tgrp_case_3_7_1 test_group_3_7
#define numb_case_3_7_1 "3.7.1"
#define name_case_3_7_1 "Perform putmsg - ENXIO."
#define sref_case_3_7_1 sref_case_3_7
#define desc_case_3_7_1 "\
Check that putmsg() can be performed on a Stream.  Checks that ENXIO is\n\
returned when putmsg() is attempted on a Stream that has received a hang\n\
up."

int
test_case_3_7_1(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_1 = { &preamble_2_1, &test_case_3_7_1, &postamble_2 };

#define test_case_3_7_1_stream_0 (&test_3_7_1)
#define test_case_3_7_1_stream_1 (NULL)
#define test_case_3_7_1_stream_2 (NULL)

#define tgrp_case_3_7_2 test_group_3_7
#define numb_case_3_7_2 "3.7.2"
#define name_case_3_7_2 "Perform putmsg."
#define sref_case_3_7_2 sref_case_3_7
#define desc_case_3_7_2 "\
Check that putmsg() can be performed on a Stream.  Checks that putmsg()\n\
can be successfully performed on a Stream that has received an\n\
asyncrhonous EPROTO read error."

int
test_case_3_7_2(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_2 = { &preamble_2_2, &test_case_3_7_2, &postamble_2 };

#define test_case_3_7_2_stream_0 (&test_3_7_2)
#define test_case_3_7_2_stream_1 (NULL)
#define test_case_3_7_2_stream_2 (NULL)

#define tgrp_case_3_7_3 test_group_3_7
#define numb_case_3_7_3 "3.7.3"
#define name_case_3_7_3 "Perform putmsg - EPROTO."
#define sref_case_3_7_3 sref_case_3_7
#define desc_case_3_7_3 "\
Check that putmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when putmsg() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO write error."

int
test_case_3_7_3(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_3 = { &preamble_2_3, &test_case_3_7_3, &postamble_2 };

#define test_case_3_7_3_stream_0 (&test_3_7_3)
#define test_case_3_7_3_stream_1 (NULL)
#define test_case_3_7_3_stream_2 (NULL)

#define tgrp_case_3_7_4 test_group_3_7
#define numb_case_3_7_4 "3.7.4"
#define name_case_3_7_4 "Perform putmsg - EPROTO."
#define sref_case_3_7_4 sref_case_3_7
#define desc_case_3_7_4 "\
Check that putmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when putmsg() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO error."

int
test_case_3_7_4(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_4 = { &preamble_2_4, &test_case_3_7_4, &postamble_2 };

#define test_case_3_7_4_stream_0 (&test_3_7_4)
#define test_case_3_7_4_stream_1 (NULL)
#define test_case_3_7_4_stream_2 (NULL)

#define tgrp_case_3_7_5 test_group_3_7
#define numb_case_3_7_5 "3.7.5"
#define name_case_3_7_5 "Perform putmsg - EINVAL."
#define sref_case_3_7_5 sref_case_3_7
#define desc_case_3_7_5 "\
Check that putmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when putmsg() is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_3_7_5(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_5 = { &preamble_5, &test_case_3_7_5, &postamble_5 };

#define test_case_3_7_5_stream_0 (&test_3_7_5)
#define test_case_3_7_5_stream_1 (NULL)
#define test_case_3_7_5_stream_2 (NULL)

#define tgrp_case_3_7_6 test_group_3_7
#define numb_case_3_7_6 "3.7.6"
#define name_case_3_7_6 "Perform putmsg - EBADF."
#define sref_case_3_7_6 sref_case_3_7
#define desc_case_3_7_6 "\
Check that putmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is not a valid file descriptor."

int
test_case_3_7_6(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	int oldfd = test_fd[child];

	test_fd[child] = -1;

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_6 = { &preamble_0, &test_case_3_7_6, &postamble_0 };

#define test_case_3_7_6_stream_0 (&test_3_7_6)
#define test_case_3_7_6_stream_1 (NULL)
#define test_case_3_7_6_stream_2 (NULL)

#define tgrp_case_3_7_7 test_group_3_7
#define numb_case_3_7_7 "3.7.7"
#define name_case_3_7_7 "Perform putmsg - EBADF."
#define sref_case_3_7_7 sref_case_3_7
#define desc_case_3_7_7 "\
Check that putmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is not open for writing."

int
test_case_3_7_7(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_7 = { &preamble_0_1, &test_case_3_7_7, &postamble_0 };

#define test_case_3_7_7_stream_0 (&test_3_7_7)
#define test_case_3_7_7_stream_1 (NULL)
#define test_case_3_7_7_stream_2 (NULL)

#define tgrp_case_3_7_8 test_group_3_7
#define numb_case_3_7_8 "3.7.8"
#define name_case_3_7_8 "Perform putmsg - EFAULT."
#define sref_case_3_7_8 sref_case_3_7
#define desc_case_3_7_8 "\
Check that putmsg() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the control or data part buffers extend beyond the\n\
caller's valid address space."

int
test_case_3_7_8(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = (char *)INVALID_ADDRESS;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = (char *)INVALID_ADDRESS;

	flags = 0;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_8 = { &preamble_0, &test_case_3_7_8, &postamble_0 };

#define test_case_3_7_8_stream_0 (&test_3_7_8)
#define test_case_3_7_8_stream_1 (NULL)
#define test_case_3_7_8_stream_2 (NULL)

#define tgrp_case_3_7_9 test_group_3_7
#define numb_case_3_7_9 "3.7.9"
#define name_case_3_7_9 "Perform putmsg - EINVAL."
#define sref_case_3_7_9 sref_case_3_7
#define desc_case_3_7_9 "\
Check that putmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when the flags argument contains an invalid value (other than 0\n\
or RS_HIPRI)."

int
test_case_3_7_9(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	flags = RS_HIPRI + 1;

	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_9 = { &preamble_0, &test_case_3_7_9, &postamble_0 };

#define test_case_3_7_9_stream_0 (&test_3_7_9)
#define test_case_3_7_9_stream_1 (NULL)
#define test_case_3_7_9_stream_2 (NULL)

#define tgrp_case_3_7_10 test_group_3_7
#define numb_case_3_7_10 "3.7.10"
#define name_case_3_7_10 "Perform putmsg - EINTR."
#define sref_case_3_7_10 sref_case_3_7
#define desc_case_3_7_10 "\
Check that putmsg() can be performed on a Stream.  Checks putmsg() will\n\
block awaiting flow control at the Stream head, and that the wait can be\n\
interrupted by a signal and putmsg() will return EINTR."

int
test_case_3_7_10(int child)
{
	char buf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(buf), buf };
	struct strbuf dat = { -1, sizeof(buf), buf };
	int flags = 0;

	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_10 = { &preamble_8_0, &test_case_3_7_10, &postamble_8 };

#define test_case_3_7_10_stream_0 (&test_3_7_10)
#define test_case_3_7_10_stream_1 (NULL)
#define test_case_3_7_10_stream_2 (NULL)

#define tgrp_case_3_7_11 test_group_3_7
#define numb_case_3_7_11 "3.7.11"
#define name_case_3_7_11 "Perform putmsg - EINVAL."
#define sref_case_3_7_11 sref_case_3_7
#define desc_case_3_7_11 "\
Check that putmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when the flags argument is set to RS_HIPRI and no control part\n\
is specified."

int
test_case_3_7_11(int child)
{
	char dbuf[16] = { 0, };
	struct strbuf ctl = { -1, -1, NULL };
	struct strbuf dat = { -1, sizeof(dbuf), dbuf };
	int flags = RS_HIPRI;

	if (test_putmsg(child, NULL, &dat, flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_11 = { &preamble_0, &test_case_3_7_11, &postamble_0 };

#define test_case_3_7_11_stream_0 (&test_3_7_11)
#define test_case_3_7_11_stream_1 (NULL)
#define test_case_3_7_11_stream_2 (NULL)

#define tgrp_case_3_7_12 test_group_3_7
#define numb_case_3_7_12 "3.7.12"
#define name_case_3_7_12 "Perform putmsg."
#define sref_case_3_7_12 sref_case_3_7
#define desc_case_3_7_12 "\
Check that putmsg() can be performed on a Stream.  Checks that zero is\n\
returned (and no message sent) when no control or data part is specified."

int
test_case_3_7_12(int child)
{
	struct strbuf ctl = { -1, -1, NULL };
	struct strbuf dat = { -1, -1, NULL };
	int flags = 0;

	if (test_putmsg(child, NULL, NULL, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_putmsg(child, &ctl, &dat, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &ctl, &dat, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_12 = { &preamble_0, &test_case_3_7_12, &postamble_0 };

#define test_case_3_7_12_stream_0 (&test_3_7_12)
#define test_case_3_7_12_stream_1 (NULL)
#define test_case_3_7_12_stream_2 (NULL)

#define tgrp_case_3_7_13 test_group_3_7
#define numb_case_3_7_13 "3.7.13"
#define name_case_3_7_13 "Perform putmsg."
#define sref_case_3_7_13 sref_case_3_7
#define desc_case_3_7_13 "\
Check that putmsg() can be performed on a Stream.  Check that a message\n\
with control but no data part can be sent with putmsg()."

int
test_case_3_7_13(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	int pflags = 0;

	char gcbuf[32] = { 0, };
	char gdbuf[32] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gflags = 0;

	int i;

	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = (char)i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = (char)(sizeof(pdbuf)-i);

	if (test_putmsg(child, &pctl, NULL, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != -1)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(pcbuf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pcbuf); i++)
		if (gcbuf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_13 = { &preamble_0, &test_case_3_7_13, &postamble_0 };

#define test_case_3_7_13_stream_0 (&test_3_7_13)
#define test_case_3_7_13_stream_1 (NULL)
#define test_case_3_7_13_stream_2 (NULL)

#define tgrp_case_3_7_14 test_group_3_7
#define numb_case_3_7_14 "3.7.14"
#define name_case_3_7_14 "Perform putmsg."
#define sref_case_3_7_14 sref_case_3_7
#define desc_case_3_7_14 "\
Check that putmsg() can be performed on a Stream.  Check that a message\n\
with data but not control part can be sent with putmsg()."

int
test_case_3_7_14(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = 0;

	char gcbuf[32] = { 0, };
	char gdbuf[32] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gflags = 0;

	int i;

	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = (char)i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = (char)(sizeof(pdbuf)-i);

	if (test_putmsg(child, NULL, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != -1)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != sizeof(pdbuf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pdbuf); i++)
		if (gdbuf[i] != (char)(sizeof(pdbuf) - i))
			return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_14 = { &preamble_0, &test_case_3_7_14, &postamble_0 };

#define test_case_3_7_14_stream_0 (&test_3_7_14)
#define test_case_3_7_14_stream_1 (NULL)
#define test_case_3_7_14_stream_2 (NULL)

#define tgrp_case_3_7_15 test_group_3_7
#define numb_case_3_7_15 "3.7.15"
#define name_case_3_7_15 "Perform putmsg."
#define sref_case_3_7_15 sref_case_3_7
#define desc_case_3_7_15 "\
Check that putmsg() can be performed on a Stream.  Checks that a high\n\
priority message with control and data parts can be sent with putmsg()."

int
test_case_3_7_15(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = RS_HIPRI;

	char gcbuf[32] = { 0, };
	char gdbuf[32] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gflags = 0;

	int i;

	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = (char)i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = (char)(sizeof(pdbuf)-i);

	if (test_putmsg(child, &pctl, &pdat, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(pcbuf))
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != sizeof(pdbuf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pcbuf); i++)
		if (gcbuf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pdbuf); i++)
		if (gdbuf[i] != (char)(sizeof(pdbuf) - i))
			return (__RESULT_FAILURE);
	state++;
	if (test_getmsg(child, &gctl, &gdat, &gflags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_15 = { &preamble_0, &test_case_3_7_15, &postamble_0 };

#define test_case_3_7_15_stream_0 (&test_3_7_15)
#define test_case_3_7_15_stream_1 (NULL)
#define test_case_3_7_15_stream_2 (NULL)

#define tgrp_case_3_7_16 test_group_3_7
#define numb_case_3_7_16 "3.7.16"
#define name_case_3_7_16 "Perform putmsg - ERANGE."
#define sref_case_3_7_16 sref_case_3_7
#define desc_case_3_7_16 "\
Check that putmsg() can be performed on a Stream.  Check that an attempt\n\
to send a control part and a data part where the data part is larger\n\
than the maximum packet size fails with error ERANGE."

int
test_case_3_7_16(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[4097] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = RS_HIPRI;

	if (test_putmsg(child, &pctl, &pdat, pflags) == __RESULT_SUCCESS || last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_16 = { &preamble_0, &test_case_3_7_16, &postamble_0 };

#define test_case_3_7_16_stream_0 (&test_3_7_16)
#define test_case_3_7_16_stream_1 (NULL)
#define test_case_3_7_16_stream_2 (NULL)

#define tgrp_case_3_7_17 test_group_3_7
#define numb_case_3_7_17 "3.7.17"
#define name_case_3_7_17 "Perform putmsg - ERANGE."
#define sref_case_3_7_17 sref_case_3_7
#define desc_case_3_7_17 "\
Check that putmsg() can be performed on a Stream.  Check that ERANGE is\n\
returned when the size of the control part exceeds the maximum\n\
configured size for the control part of a message."

int
test_case_3_7_17(int child)
{
	char pcbuf[(1<<12)+1] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = RS_HIPRI;

	if (test_putmsg(child, &pctl, &pdat, pflags) == __RESULT_SUCCESS || last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_17 = { &preamble_0, &test_case_3_7_17, &postamble_0 };

#define test_case_3_7_17_stream_0 (&test_3_7_17)
#define test_case_3_7_17_stream_1 (NULL)
#define test_case_3_7_17_stream_2 (NULL)

#define tgrp_case_3_7_18 test_group_3_7
#define numb_case_3_7_18 "3.7.18"
#define name_case_3_7_18 "Perform putmsg."
#define sref_case_3_7_18 sref_case_3_7
#define desc_case_3_7_18 "\
Check that putmsg() can be performed on a Stream.  Check that ERANGE is\n\
returned when the size of the data part exceeds the maximum configured\n\
size for the data part of a message."

int
test_case_3_7_18(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[(1<<18)+1] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pflags = RS_HIPRI;

	if (test_putmsg(child, &pctl, &pdat, pflags) == __RESULT_SUCCESS || last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_7_18 = { &preamble_0, &test_case_3_7_18, &postamble_0 };

#define test_case_3_7_18_stream_0 (&test_3_7_18)
#define test_case_3_7_18_stream_1 (NULL)
#define test_case_3_7_18_stream_2 (NULL)

#define tgrp_case_3_7_19 test_group_3_7
#define numb_case_3_7_19 "3.7.19"
#define name_case_3_7_19 "Perform putmsg."
#define sref_case_3_7_19 sref_case_3_7
#define desc_case_3_7_19 "\
Check that putmsg() can be performed on a Stream.  Check that ENOSTR\n\
is returned when putmsg() is performed on a file descriptor that does\n\
not correspond to a Stream."

int
test_case_3_7_19(int child)
{
#ifndef HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
	return (__RESULT_SKIPPED);
#else
	/* unfortunately the below just spits crap to standard out using read/write overloading */
	char buf[16] = { 0, };
	struct strbuf pctl = { -1, sizeof(buf), buf };
	struct strbuf pdat = { -1, sizeof(buf), buf };
	int pflags = RS_HIPRI;
	int oldfd = test_fd[child];

	test_fd[child] = 1;
	start_tt(100);
	if (test_putmsg(child, &pctl, &pdat, pflags) == __RESULT_SUCCESS || last_errno != ENOSTR) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_3_7_19 = { &preamble_0, &test_case_3_7_19, &postamble_0 };

#define test_case_3_7_19_stream_0 (&test_3_7_19)
#define test_case_3_7_19_stream_1 (NULL)
#define test_case_3_7_19_stream_2 (NULL)

static const char test_group_3_8[] = "Perform PUTPMSG on one Stream";
static const char sref_case_3_8[] = "POSIX 1003.1 2003/SUSv3 putpmsg(2p) reference page.";

#define tgrp_case_3_8_1 test_group_3_8
#define numb_case_3_8_1 "3.8.1"
#define name_case_3_8_1 "Perform putpmsg - ENXIO."
#define sref_case_3_8_1 sref_case_3_8
#define desc_case_3_8_1 "\
Check that putpmsg() can be performed on a Stream.  Checks that ENXIO is\n\
returned when putpmsg() is attempted on a Stream that has received a hang\n\
up."

int
test_case_3_8_1(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != ENXIO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_1 = { &preamble_2_1, &test_case_3_8_1, &postamble_2 };

#define test_case_3_8_1_stream_0 (&test_3_8_1)
#define test_case_3_8_1_stream_1 (NULL)
#define test_case_3_8_1_stream_2 (NULL)

#define tgrp_case_3_8_2 test_group_3_8
#define numb_case_3_8_2 "3.8.2"
#define name_case_3_8_2 "Perform putpmsg."
#define sref_case_3_8_2 sref_case_3_8
#define desc_case_3_8_2 "\
Check that putpmsg() can be performed on a Stream.  Checks that putpmsg()\n\
can be successfully performed on a Stream that has received an\n\
asyncrhonous EPROTO read error."

int
test_case_3_8_2(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_2 = { &preamble_2_2, &test_case_3_8_2, &postamble_2 };

#define test_case_3_8_2_stream_0 (&test_3_8_2)
#define test_case_3_8_2_stream_1 (NULL)
#define test_case_3_8_2_stream_2 (NULL)

#define tgrp_case_3_8_3 test_group_3_8
#define numb_case_3_8_3 "3.8.3"
#define name_case_3_8_3 "Perform putpmsg - EPROTO."
#define sref_case_3_8_3 sref_case_3_8
#define desc_case_3_8_3 "\
Check that putpmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when putpmsg() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO write error."

int
test_case_3_8_3(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_3 = { &preamble_2_3, &test_case_3_8_3, &postamble_2 };

#define test_case_3_8_3_stream_0 (&test_3_8_3)
#define test_case_3_8_3_stream_1 (NULL)
#define test_case_3_8_3_stream_2 (NULL)

#define tgrp_case_3_8_4 test_group_3_8
#define numb_case_3_8_4 "3.8.4"
#define name_case_3_8_4 "Perform putpmsg - EPROTO."
#define sref_case_3_8_4 sref_case_3_8
#define desc_case_3_8_4 "\
Check that putpmsg() can be performed on a Stream.  Checks that EPROTO is\n\
returned when putpmsg() is attempted on a Stream that has received an\n\
asyncrhonous EPROTO error."

int
test_case_3_8_4(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_4 = { &preamble_2_4, &test_case_3_8_4, &postamble_2 };

#define test_case_3_8_4_stream_0 (&test_3_8_4)
#define test_case_3_8_4_stream_1 (NULL)
#define test_case_3_8_4_stream_2 (NULL)

#define tgrp_case_3_8_5 test_group_3_8
#define numb_case_3_8_5 "3.8.5"
#define name_case_3_8_5 "Perform putpmsg - EINVAL."
#define sref_case_3_8_5 sref_case_3_8
#define desc_case_3_8_5 "\
Check that putpmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when putpmsg() is attempted on a Stream that is linked under a\n\
Multiplexing Driver."

int
test_case_3_8_5(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_5 = { &preamble_5, &test_case_3_8_5, &postamble_5 };

#define test_case_3_8_5_stream_0 (&test_3_8_5)
#define test_case_3_8_5_stream_1 (NULL)
#define test_case_3_8_5_stream_2 (NULL)

#define tgrp_case_3_8_6 test_group_3_8
#define numb_case_3_8_6 "3.8.6"
#define name_case_3_8_6 "Perform putpmsg - EBADF."
#define sref_case_3_8_6 sref_case_3_8
#define desc_case_3_8_6 "\
Check that putpmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is not a valid file descriptor."

int
test_case_3_8_6(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };
	int oldfd = test_fd[child];

	test_fd[child] = -1;

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_6 = { &preamble_0, &test_case_3_8_6, &postamble_0 };

#define test_case_3_8_6_stream_0 (&test_3_8_6)
#define test_case_3_8_6_stream_1 (NULL)
#define test_case_3_8_6_stream_2 (NULL)

#define tgrp_case_3_8_7 test_group_3_8
#define numb_case_3_8_7 "3.8.7"
#define name_case_3_8_7 "Perform putpmsg - EBADF."
#define sref_case_3_8_7 sref_case_3_8
#define desc_case_3_8_7 "\
Check that putpmsg() can be performed on a Stream.  Checks that EBADF is\n\
returned when fildes is not open for writing."

int
test_case_3_8_7(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EBADF)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_7 = { &preamble_0_1, &test_case_3_8_7, &postamble_0 };

#define test_case_3_8_7_stream_0 (&test_3_8_7)
#define test_case_3_8_7_stream_1 (NULL)
#define test_case_3_8_7_stream_2 (NULL)

#define tgrp_case_3_8_8 test_group_3_8
#define numb_case_3_8_8 "3.8.8"
#define name_case_3_8_8 "Perform putpmsg - EFAULT."
#define sref_case_3_8_8 sref_case_3_8
#define desc_case_3_8_8 "\
Check that putpmsg() can be performed on a Stream.  Checks that EFAULT is\n\
returned when the control or data part buffers extend beyond the\n\
caller's valid address space."

int
test_case_3_8_8(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = (char *)INVALID_ADDRESS;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = (char *)INVALID_ADDRESS;

	band = 0;
	flags = MSG_BAND;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_8 = { &preamble_0, &test_case_3_8_8, &postamble_0 };

#define test_case_3_8_8_stream_0 (&test_3_8_8)
#define test_case_3_8_8_stream_1 (NULL)
#define test_case_3_8_8_stream_2 (NULL)

#define tgrp_case_3_8_9 test_group_3_8
#define numb_case_3_8_9 "3.8.9"
#define name_case_3_8_9 "Perform putpmsg - EINVAL."
#define sref_case_3_8_9 sref_case_3_8
#define desc_case_3_8_9 "\
Check that putpmsg() can be performed on a Stream.  Checks that EINVAL is\n\
returned when the flags argument contains an invalid value (other than MSG_BAND\n\
or MSG_HIPRI)."

int
test_case_3_8_9(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 0;
	flags = MSG_ANY;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_9 = { &preamble_0, &test_case_3_8_9, &postamble_0 };

#define test_case_3_8_9_stream_0 (&test_3_8_9)
#define test_case_3_8_9_stream_1 (NULL)
#define test_case_3_8_9_stream_2 (NULL)

#define tgrp_case_3_8_10 test_group_3_8
#define numb_case_3_8_10 "3.8.10"
#define name_case_3_8_10 "Perform putpmsg - EINVAL."
#define sref_case_3_8_10 sref_case_3_8
#define desc_case_3_8_10 "\
Check that putpmsg() can be performed on a Stream.  Checks that EINVAL\n\
is returned when the band argument is non-zero and the flags argument is\n\
MSG_BAND."

int
test_case_3_8_10(int child)
{
	struct strbuf ctl;
	struct strbuf dat;
	int band;
	int flags;
	char cbuf[16] = { 0, };
	char dbuf[16] = { 0, };

	ctl.maxlen = -1;
	ctl.len = sizeof(cbuf);
	ctl.buf = cbuf;

	dat.maxlen = -1;
	dat.len = sizeof(dbuf);
	dat.buf = dbuf;

	band = 1;
	flags = MSG_HIPRI;

	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_10 = { &preamble_0, &test_case_3_8_10, &postamble_0 };

#define test_case_3_8_10_stream_0 (&test_3_8_10)
#define test_case_3_8_10_stream_1 (NULL)
#define test_case_3_8_10_stream_2 (NULL)

#define tgrp_case_3_8_11 test_group_3_8
#define numb_case_3_8_11 "3.8.11"
#define name_case_3_8_11 "Perform putpmsg - EINTR."
#define sref_case_3_8_11 sref_case_3_8
#define desc_case_3_8_11 "\
Check that putpmsg() can be performed on a Stream.  Check putpmsg() will\n\
block awaiting flow control on band 1 at the Stream head, and that the\n\
wait can be interrupted by a signal and putpmsg() will return EINTR."

int
test_case_3_8_11(int child)
{
	char buf[16] = { 0, };
	struct strbuf ctl = { -1, sizeof(buf), buf };
	struct strbuf dat = { -1, sizeof(buf), buf };
	int band = 1;
	int flags = MSG_BAND;

	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, band, flags) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	print_signal(child, last_signum);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_11 = { &preamble_8_1, &test_case_3_8_11, &postamble_8 };

#define test_case_3_8_11_stream_0 (&test_3_8_11)
#define test_case_3_8_11_stream_1 (NULL)
#define test_case_3_8_11_stream_2 (NULL)

#define tgrp_case_3_8_12 test_group_3_8
#define numb_case_3_8_12 "3.8.12"
#define name_case_3_8_12 "Perform putpmsg."
#define sref_case_3_8_12 sref_case_3_8
#define desc_case_3_8_12 "\
Check that putpmsg() can be performed on a Stream.  Checks that zero is\n\
returned (and no message sent) when no control or data part is specified."

int
test_case_3_8_12(int child)
{
	struct strbuf ctl = { -1, -1, NULL };
	struct strbuf dat = { -1, -1, NULL };
	int band = 1;
	int flags = MSG_BAND;

	if (test_putpmsg(child, NULL, NULL, band, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_putpmsg(child, &ctl, &dat, band, flags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	band = 0;
	flags = MSG_ANY;
	if (test_getpmsg(child, &ctl, &dat, &band, &flags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_12 = { &preamble_0, &test_case_3_8_12, &postamble_0 };

#define test_case_3_8_12_stream_0 (&test_3_8_12)
#define test_case_3_8_12_stream_1 (NULL)
#define test_case_3_8_12_stream_2 (NULL)

#define tgrp_case_3_8_13 test_group_3_8
#define numb_case_3_8_13 "3.8.13"
#define name_case_3_8_13 "Perform putpmsg."
#define sref_case_3_8_13 sref_case_3_8
#define desc_case_3_8_13 "\
Check that putpmsg() can be performed on a Stream.  Check that a message\n\
with control but no data part can be sent with putpmsg()."

int
test_case_3_8_13(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	int pband = 1;
	int pflags = MSG_BAND;

	char gcbuf[32] = { 0, };
	char gdbuf[32] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gband = 0;
	int gflags = MSG_ANY;

	int i;

	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = (char)i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = (char)(sizeof(pdbuf)-i);

	if (test_putpmsg(child, &pctl, NULL, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gband != pband)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != -1)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(pcbuf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pcbuf); i++)
		if (gcbuf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	gband = 0;
	gflags = MSG_ANY;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_13 = { &preamble_0, &test_case_3_8_13, &postamble_0 };

#define test_case_3_8_13_stream_0 (&test_3_8_13)
#define test_case_3_8_13_stream_1 (NULL)
#define test_case_3_8_13_stream_2 (NULL)

#define tgrp_case_3_8_14 test_group_3_8
#define numb_case_3_8_14 "3.8.14"
#define name_case_3_8_14 "Perform putpmsg."
#define sref_case_3_8_14 sref_case_3_8
#define desc_case_3_8_14 "\
Check that putpmsg() can be performed on a Stream.  Check that a message\n\
with data but not control part can be sent with putpmsg()."

int
test_case_3_8_14(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = 1;
	int pflags = MSG_BAND;

	char gcbuf[32] = { 0, };
	char gdbuf[32] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gband = 0;
	int gflags = MSG_ANY;

	int i;

	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = (char)i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = (char)(sizeof(pdbuf)-i);

	if (test_putpmsg(child, NULL, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gband != pband)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != -1)
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != sizeof(pdbuf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pdbuf); i++)
		if (gdbuf[i] != (char)(sizeof(pdbuf) - i))
			return (__RESULT_FAILURE);
	state++;
	gband = 0;
	gflags = MSG_ANY;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_14 = { &preamble_0, &test_case_3_8_14, &postamble_0 };

#define test_case_3_8_14_stream_0 (&test_3_8_14)
#define test_case_3_8_14_stream_1 (NULL)
#define test_case_3_8_14_stream_2 (NULL)

#define tgrp_case_3_8_15 test_group_3_8
#define numb_case_3_8_15 "3.8.15"
#define name_case_3_8_15 "Perform putpmsg."
#define sref_case_3_8_15 sref_case_3_8
#define desc_case_3_8_15 "\
Check that putpmsg() can be performed on a Stream.  Checks that a high\n\
priority message with control and data parts can be sent with putpmsg()."

int
test_case_3_8_15(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = 0;
	int pflags = MSG_HIPRI;

	char gcbuf[32] = { 0, };
	char gdbuf[32] = { 0, };
	struct strbuf gctl = { sizeof(gcbuf), -1, gcbuf };
	struct strbuf gdat = { sizeof(gdbuf), -1, gdbuf };
	int gband = 0;
	int gflags = MSG_ANY;

	int i;

	for (i = 0; i < sizeof(pcbuf); i++)
		pcbuf[i] = (char)i;
	for (i = 0; i < sizeof(pdbuf); i++)
		pdbuf[i] = (char)(sizeof(pdbuf)-i);

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (gband != pband)
		return (__RESULT_FAILURE);
	state++;
	if (gflags != pflags)
		return (__RESULT_FAILURE);
	state++;
	if (gctl.len != sizeof(pcbuf))
		return (__RESULT_FAILURE);
	state++;
	if (gdat.len != sizeof(pdbuf))
		return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pcbuf); i++)
		if (gcbuf[i] != (char)i)
			return (__RESULT_FAILURE);
	state++;
	for (i = 0; i < sizeof(pdbuf); i++)
		if (gdbuf[i] != (char)(sizeof(pdbuf) - i))
			return (__RESULT_FAILURE);
	state++;
	gband = 0;
	gflags = MSG_ANY;
	if (test_getpmsg(child, &gctl, &gdat, &gband, &gflags) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_15 = { &preamble_0, &test_case_3_8_15, &postamble_0 };

#define test_case_3_8_15_stream_0 (&test_3_8_15)
#define test_case_3_8_15_stream_1 (NULL)
#define test_case_3_8_15_stream_2 (NULL)

#define tgrp_case_3_8_16 test_group_3_8
#define numb_case_3_8_16 "3.8.16"
#define name_case_3_8_16 "Perform putpmsg - ERANGE."
#define sref_case_3_8_16 sref_case_3_8
#define desc_case_3_8_16 "\
Check that putpmsg() can be performed on a Stream.  Check that an attempt\n\
to send a control part and a data part where the data part is larger\n\
than the maximum packet size fails with error ERANGE."

int
test_case_3_8_16(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[4097] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = 0;
	int pflags = MSG_HIPRI;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) == __RESULT_SUCCESS || last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_16 = { &preamble_0, &test_case_3_8_16, &postamble_0 };

#define test_case_3_8_16_stream_0 (&test_3_8_16)
#define test_case_3_8_16_stream_1 (NULL)
#define test_case_3_8_16_stream_2 (NULL)

#define tgrp_case_3_8_17 test_group_3_8
#define numb_case_3_8_17 "3.8.17"
#define name_case_3_8_17 "Perform putpmsg - ERANGE."
#define sref_case_3_8_17 sref_case_3_8
#define desc_case_3_8_17 "\
Check that putpmsg() can be performed on a Stream.  Check that ERANGE is\n\
returned when the size of the control part exceeds the maximum\n\
configured size for the control part of a message."

int
test_case_3_8_17(int child)
{
	char pcbuf[(1<<12)+1] = { 0, };
	char pdbuf[32] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = 0;
	int pflags = MSG_HIPRI;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) == __RESULT_SUCCESS || last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_17 = { &preamble_0, &test_case_3_8_17, &postamble_0 };

#define test_case_3_8_17_stream_0 (&test_3_8_17)
#define test_case_3_8_17_stream_1 (NULL)
#define test_case_3_8_17_stream_2 (NULL)

#define tgrp_case_3_8_18 test_group_3_8
#define numb_case_3_8_18 "3.8.18"
#define name_case_3_8_18 "Perform putpmsg."
#define sref_case_3_8_18 sref_case_3_8
#define desc_case_3_8_18 "\
Check that putpmsg() can be performed on a Stream.  Check that ERANGE is\n\
returned when the size of the data part exceeds the maximum configured\n\
size for the data part of a message."

int
test_case_3_8_18(int child)
{
	char pcbuf[32] = { 0, };
	char pdbuf[(1<<18)+1] = { 0, };
	struct strbuf pctl = { -1, sizeof(pcbuf), pcbuf };
	struct strbuf pdat = { -1, sizeof(pdbuf), pdbuf };
	int pband = 0;
	int pflags = MSG_HIPRI;

	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) == __RESULT_SUCCESS || last_errno != ERANGE)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_8_18 = { &preamble_0, &test_case_3_8_18, &postamble_0 };

#define test_case_3_8_18_stream_0 (&test_3_8_18)
#define test_case_3_8_18_stream_1 (NULL)
#define test_case_3_8_18_stream_2 (NULL)

#define tgrp_case_3_8_19 test_group_3_8
#define numb_case_3_8_19 "3.8.19"
#define name_case_3_8_19 "Perform putpmsg."
#define sref_case_3_8_19 sref_case_3_8
#define desc_case_3_8_19 "\
Check that putpmsg() can be performed on a Stream.  Check that ENOSTR\n\
is returned when putmsg() is performed on a file descriptor that does\n\
not correspond to a Stream."

int
test_case_3_8_19(int child)
{
#ifndef HAVE_KMEMB_STRUCT_FILE_OPERATIONS_UNLOCKED_IOCTL
	return (__RESULT_SKIPPED);
#else
	/* unfortunately the below just spits crap to standard out using read/write overloading */
	char buf[16] = { 0, };
	struct strbuf pctl = { -1, sizeof(buf), buf };
	struct strbuf pdat = { -1, sizeof(buf), buf };
	int pband = 0;
	int pflags = MSG_HIPRI;
	int oldfd = test_fd[child];

	test_fd[child] = 0;
	start_tt(100);
	if (test_putpmsg(child, &pctl, &pdat, pband, pflags) == __RESULT_SUCCESS || last_errno != ENOSTR) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_3_8_19 = { &preamble_0, &test_case_3_8_19, &postamble_0 };

#define test_case_3_8_19_stream_0 (&test_3_8_19)
#define test_case_3_8_19_stream_1 (NULL)
#define test_case_3_8_19_stream_2 (NULL)

static const char test_group_3_9[] = "Perform ISASTREAM on one Stream";
static const char sref_case_3_9[] = "POSIX 1003.1 2003/SUSv3 isastream(2p) reference page.";

#define tgrp_case_3_9_1 test_group_3_9
#define numb_case_3_9_1 "3.9.1"
#define name_case_3_9_1 "Perform isastream."
#define sref_case_3_9_1 sref_case_3_9
#define desc_case_3_9_1 "\
Check that isastream() can be peformed on a Stream.  Checks that\n\
isastream() successfully identifies a Stream."

int
test_case_3_9_1(int child)
{
	if (test_isastream(child) != __RESULT_SUCCESS || last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_9_1 = { &preamble_0, &test_case_3_9_1, &postamble_0 };

#define test_case_3_9_1_stream_0 (&test_3_9_1)
#define test_case_3_9_1_stream_1 (NULL)
#define test_case_3_9_1_stream_2 (NULL)

#define tgrp_case_3_9_2 test_group_3_9
#define numb_case_3_9_2 "3.9.2"
#define name_case_3_9_2 "Perform isastream."
#define sref_case_3_9_2 sref_case_3_9
#define desc_case_3_9_2 "\
Check that isastream() can be peformed on a Stream.  Checks that\n\
isastream() successfully identifies a file descriptor that is not a Stream."

int
test_case_3_9_2(int child)
{
	int oldfd = test_fd[child];

	test_fd[child] = 1;

	if (test_isastream(child) != __RESULT_SUCCESS || last_retval != 0) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_9_2 = { &preamble_0, &test_case_3_9_2, &postamble_0 };

#define test_case_3_9_2_stream_0 (&test_3_9_2)
#define test_case_3_9_2_stream_1 (NULL)
#define test_case_3_9_2_stream_2 (NULL)

#define tgrp_case_3_9_3 test_group_3_9
#define numb_case_3_9_3 "3.9.3"
#define name_case_3_9_3 "Perform isastream."
#define sref_case_3_9_3 sref_case_3_9
#define desc_case_3_9_3 "\
Check that isastream() can be peformed on a Stream.  Checks that\n\
EBADF is returned for an invalid file descriptor."

int
test_case_3_9_3(int child)
{
	int oldfd = test_fd[child];

	test_fd[child] = -1;

	if (test_isastream(child) == __RESULT_SUCCESS || last_errno != EBADF) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	state++;
	test_fd[child] = oldfd;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_9_3 = { &preamble_0, &test_case_3_9_3, &postamble_0 };

#define test_case_3_9_3_stream_0 (&test_3_9_3)
#define test_case_3_9_3_stream_1 (NULL)
#define test_case_3_9_3_stream_2 (NULL)

static const char test_group_3_10[] = "Perform POLL on one Stream";
static const char sref_case_3_10[] = "POSIX 1003.1 2003/SUSv3 poll(2p) reference page.";

#define tgrp_case_3_10_1 test_group_3_10
#define numb_case_3_10_1 "3.10.1"
#define name_case_3_10_1 "Perform poll - POLLNVAL."
#define sref_case_3_10_1 sref_case_3_10
#define desc_case_3_10_1 "\
Check that poll() can be peformed on a Stream.  Checks that POLNVAL is\n\
returned when an invalid file descriptor is passed to poll()."

int
test_case_3_10_1(int child)
{
	int oldfd = test_fd[child];
	short revents = 0;

	test_fd[child] = -1;
	if (test_poll(child, POLLNVAL, &revents, 0) != __RESULT_SUCCESS) {
		test_fd[child] = oldfd;
		return (__RESULT_FAILURE);
	}
	test_fd[child] = oldfd;
	state++;
	if (last_retval != 1)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLNVAL))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_1 = { &preamble_0, &test_case_3_10_1, &postamble_0 };

#define test_case_3_10_1_stream_0 (&test_3_10_1)
#define test_case_3_10_1_stream_1 (NULL)
#define test_case_3_10_1_stream_2 (NULL)

#define tgrp_case_3_10_2 test_group_3_10
#define numb_case_3_10_2 "3.10.2"
#define name_case_3_10_2 "Perform poll - EFAULT."
#define sref_case_3_10_2 sref_case_3_10
#define desc_case_3_10_2 "\
Check that poll() can be peformed on a Stream.  Check that EFAULT is\n\
returned if the pollfd pointer extends outside the caller's address\n\
space."

int
test_case_3_10_2(int child)
{
	int result;

	print_poll(child, 0);
	if ((result = last_retval = poll((struct pollfd *)-1, 1, 0)) == -1) {
		print_errno(child, (last_errno = errno));
		if (last_errno != EFAULT)
			return (__RESULT_FAILURE);
		state++;
		return (__RESULT_SUCCESS);
	}
	print_poll_value(child, last_retval, 0);
	return (__RESULT_FAILURE);
}
struct test_stream test_3_10_2 = { &preamble_0, &test_case_3_10_2, &postamble_0 };

#define test_case_3_10_2_stream_0 (&test_3_10_2)
#define test_case_3_10_2_stream_1 (NULL)
#define test_case_3_10_2_stream_2 (NULL)

#define tgrp_case_3_10_3 test_group_3_10
#define numb_case_3_10_3 "3.10.3"
#define name_case_3_10_3 "Perform poll - EINVAL."
#define sref_case_3_10_3 sref_case_3_10
#define desc_case_3_10_3 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_3(int child)
{
	struct pollfd pfd[4097] = { { 0, }, };
	int result;

	print_poll(child, 0);
	if ((result = last_retval = poll(pfd, 4097, 0)) == -1) {
		print_errno(child, (last_errno = errno));
		if (last_errno != EINVAL)
			return (__RESULT_FAILURE);
		state++;
		return (__RESULT_SUCCESS);
	}
	print_poll_value(child, last_retval, 0);
	return (__RESULT_FAILURE);
}
struct test_stream test_3_10_3 = { &preamble_0, &test_case_3_10_3, &postamble_0 };

#define test_case_3_10_3_stream_0 (&test_3_10_3)
#define test_case_3_10_3_stream_1 (NULL)
#define test_case_3_10_3_stream_2 (NULL)

#define tgrp_case_3_10_4 test_group_3_10
#define numb_case_3_10_4 "3.10.4"
#define name_case_3_10_4 "Perform poll - EINTR."
#define sref_case_3_10_4 sref_case_3_10
#define desc_case_3_10_4 "\
Check that poll() can be peformed on a Stream.  Check that poll() will\n\
block even (if the Stream is set for non-blocking operation) and can be\n\
interrupted by a signal and will return EINTR."

int
test_case_3_10_4(int child)
{
	short revents = 0;

	if (start_tt(100) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLIN, &revents, 1000) == __RESULT_SUCCESS || last_errno != EINTR)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_4 = { &preamble_0, &test_case_3_10_4, &postamble_0 };

#define test_case_3_10_4_stream_0 (&test_3_10_4)
#define test_case_3_10_4_stream_1 (NULL)
#define test_case_3_10_4_stream_2 (NULL)

#define tgrp_case_3_10_5 test_group_3_10
#define numb_case_3_10_5 "3.10.5"
#define name_case_3_10_5 "Perform poll POLLIN."
#define sref_case_3_10_5 sref_case_3_10
#define desc_case_3_10_5 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_5(int child)
{
	short revents = 0;

	if (test_poll(child, POLLIN, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	{
		char buf[16] = { 0, };
		struct strbuf dat = { -1, sizeof(buf), buf };

		state++;
		if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
	}
	state++;
	if (test_poll(child, POLLIN, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLIN))
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLIN, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_5 = { &preamble_0, &test_case_3_10_5, &postamble_0 };

#define test_case_3_10_5_stream_0 (&test_3_10_5)
#define test_case_3_10_5_stream_1 (NULL)
#define test_case_3_10_5_stream_2 (NULL)

#define tgrp_case_3_10_6 test_group_3_10
#define numb_case_3_10_6 "3.10.6"
#define name_case_3_10_6 "Perform poll POLLPRI."
#define sref_case_3_10_6 sref_case_3_10
#define desc_case_3_10_6 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_6(int child)
{
	short revents = 0;

	if (test_poll(child, POLLPRI, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	{
		char buf[16] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };

		state++;
		if (test_putpmsg(child, &ctl, &dat, 0, MSG_HIPRI) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
	}
	state++;
	if (test_poll(child, POLLPRI, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLPRI))
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLPRI, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_6 = { &preamble_0, &test_case_3_10_6, &postamble_0 };

#define test_case_3_10_6_stream_0 (&test_3_10_6)
#define test_case_3_10_6_stream_1 (NULL)
#define test_case_3_10_6_stream_2 (NULL)

#define tgrp_case_3_10_7 test_group_3_10
#define numb_case_3_10_7 "3.10.7"
#define name_case_3_10_7 "Perform poll POLLOUT."
#define sref_case_3_10_7 sref_case_3_10
#define desc_case_3_10_7 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_7(int child)
{
	short revents = 0;

	if (test_poll(child, POLLOUT, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLOUT))
		return (__RESULT_FAILURE);
	state++;
	{
		char buf[1024] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };

		if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval == 0)
			return (__RESULT_FAILURE);
		while (last_retval != 0) {
			state++;
			if (test_putpmsg(child, &ctl, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
				return (__RESULT_FAILURE);
			state++;
			if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS)
				return (__RESULT_FAILURE);
		}
	}
	state++;
	if (test_poll(child, POLLOUT, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLOUT, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLOUT))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_7 = { &preamble_0, &test_case_3_10_7, &postamble_0 };

#define test_case_3_10_7_stream_0 (&test_3_10_7)
#define test_case_3_10_7_stream_1 (NULL)
#define test_case_3_10_7_stream_2 (NULL)

#define tgrp_case_3_10_8 test_group_3_10
#define numb_case_3_10_8 "3.10.8"
#define name_case_3_10_8 "Perform poll POLLRDNORM."
#define sref_case_3_10_8 sref_case_3_10
#define desc_case_3_10_8 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_8(int child)
{
	short revents = 0;

	if (test_poll(child, POLLRDNORM, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	{
		char buf[16] = { 0, };
		struct strbuf dat = { -1, sizeof(buf), buf };

		state++;
		if (test_putpmsg(child, NULL, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
	}
	state++;
	if (test_poll(child, POLLRDNORM, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLRDNORM))
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLRDNORM, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_8 = { &preamble_0, &test_case_3_10_8, &postamble_0 };

#define test_case_3_10_8_stream_0 (&test_3_10_8)
#define test_case_3_10_8_stream_1 (NULL)
#define test_case_3_10_8_stream_2 (NULL)

#define tgrp_case_3_10_9 test_group_3_10
#define numb_case_3_10_9 "3.10.9"
#define name_case_3_10_9 "Perform poll POLLRDBAND."
#define sref_case_3_10_9 sref_case_3_10
#define desc_case_3_10_9 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_9(int child)
{
	short revents = 0;

	if (test_poll(child, POLLRDBAND, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	{
		char buf[16] = { 0, };
		struct strbuf dat = { -1, sizeof(buf), buf };

		state++;
		if (test_putpmsg(child, NULL, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
	}
	state++;
	if (test_poll(child, POLLRDBAND, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLRDBAND))
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLRDBAND, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_9 = { &preamble_0, &test_case_3_10_9, &postamble_0 };

#define test_case_3_10_9_stream_0 (&test_3_10_9)
#define test_case_3_10_9_stream_1 (NULL)
#define test_case_3_10_9_stream_2 (NULL)

#define tgrp_case_3_10_10 test_group_3_10
#define numb_case_3_10_10 "3.10.10"
#define name_case_3_10_10 "Perform poll POLLWRNORM."
#define sref_case_3_10_10 sref_case_3_10
#define desc_case_3_10_10 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_10(int child)
{
	short revents = 0;

	if (test_poll(child, POLLWRNORM, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLWRNORM))
		return (__RESULT_FAILURE);
	state++;
	{
		char buf[1024] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };

		if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval == 0)
			return (__RESULT_FAILURE);
		while (last_retval != 0) {
			state++;
			if (test_putpmsg(child, &ctl, &dat, 0, MSG_BAND) != __RESULT_SUCCESS)
				return (__RESULT_FAILURE);
			state++;
			if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS)
				return (__RESULT_FAILURE);
		}
	}
	state++;
	if (test_poll(child, POLLWRNORM, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLWRNORM, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLWRNORM))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_10 = { &preamble_0, &test_case_3_10_10, &postamble_0 };

#define test_case_3_10_10_stream_0 (&test_3_10_10)
#define test_case_3_10_10_stream_1 (NULL)
#define test_case_3_10_10_stream_2 (NULL)

#define tgrp_case_3_10_11 test_group_3_10
#define numb_case_3_10_11 "3.10.11"
#define name_case_3_10_11 "Perform poll POLLWRBAND."
#define sref_case_3_10_11 sref_case_3_10
#define desc_case_3_10_11 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_11(int child)
{
	short revents = 0;

#if 0
	if (test_poll(child, POLLWRBAND, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLWRBAND))
		return (__RESULT_FAILURE);
	state++;
#endif
	{
		char buf[1024] = { 0, };
		struct strbuf ctl = { -1, sizeof(buf), buf };
		struct strbuf dat = { -1, sizeof(buf), buf };

		if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		if (last_retval == 0)
			return (__RESULT_FAILURE);
		while (last_retval != 0) {
			state++;
			if (test_putpmsg(child, &ctl, &dat, 1, MSG_BAND) != __RESULT_SUCCESS)
				return (__RESULT_FAILURE);
			state++;
			if (test_ioctl(child, I_CANPUT, 1) != __RESULT_SUCCESS)
				return (__RESULT_FAILURE);
		}
	}
	state++;
	if (test_poll(child, POLLWRBAND, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval != 0)
		return (__RESULT_FAILURE);
	state++;
	if (test_ioctl(child, I_FLUSH, FLUSHRW) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (test_poll(child, POLLWRBAND, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLWRBAND))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_11 = { &preamble_0, &test_case_3_10_11, &postamble_0 };

#define test_case_3_10_11_stream_0 (&test_3_10_11)
#define test_case_3_10_11_stream_1 (NULL)
#define test_case_3_10_11_stream_2 (NULL)

#define tgrp_case_3_10_12_1 test_group_3_10
#define numb_case_3_10_12_1 "3.10.12.1"
#define name_case_3_10_12_1 "Perform poll POLLERR."
#define sref_case_3_10_12_1 sref_case_3_10
#define desc_case_3_10_12_1 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_12_1(int child)
{
	short revents = 0;

	if (test_poll(child, POLLERR, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLERR))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_12_1 = { &preamble_2_2, &test_case_3_10_12_1, &postamble_2 };

#define test_case_3_10_12_1_stream_0 (&test_3_10_12_1)
#define test_case_3_10_12_1_stream_1 (NULL)
#define test_case_3_10_12_1_stream_2 (NULL)

#define tgrp_case_3_10_12_2 test_group_3_10
#define numb_case_3_10_12_2 "3.10.12.2"
#define name_case_3_10_12_2 "Perform poll POLLERR."
#define sref_case_3_10_12_2 sref_case_3_10
#define desc_case_3_10_12_2 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_12_2(int child)
{
	short revents = 0;

	if (test_poll(child, POLLERR, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLERR))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_12_2 = { &preamble_2_3, &test_case_3_10_12_2, &postamble_2 };

#define test_case_3_10_12_2_stream_0 (&test_3_10_12_2)
#define test_case_3_10_12_2_stream_1 (NULL)
#define test_case_3_10_12_2_stream_2 (NULL)

#define tgrp_case_3_10_12_3 test_group_3_10
#define numb_case_3_10_12_3 "3.10.12.3"
#define name_case_3_10_12_3 "Perform poll POLLERR."
#define sref_case_3_10_12_3 sref_case_3_10
#define desc_case_3_10_12_3 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_12_3(int child)
{
	short revents = 0;

	if (test_poll(child, POLLERR, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLERR))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_12_3 = { &preamble_2_4, &test_case_3_10_12_3, &postamble_2 };

#define test_case_3_10_12_3_stream_0 (&test_3_10_12_3)
#define test_case_3_10_12_3_stream_1 (NULL)
#define test_case_3_10_12_3_stream_2 (NULL)

#define tgrp_case_3_10_13 test_group_3_10
#define numb_case_3_10_13 "3.10.13"
#define name_case_3_10_13 "Perform poll POLLHUP."
#define sref_case_3_10_13 sref_case_3_10
#define desc_case_3_10_13 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_13(int child)
{
	short revents = 0;

	if (test_poll(child, POLLHUP, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLHUP))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_13 = { &preamble_2_1, &test_case_3_10_13, &postamble_2 };

#define test_case_3_10_13_stream_0 (&test_3_10_13)
#define test_case_3_10_13_stream_1 (NULL)
#define test_case_3_10_13_stream_2 (NULL)

#define tgrp_case_3_10_14 test_group_3_10
#define numb_case_3_10_14 "3.10.14"
#define name_case_3_10_14 "Perform poll POLLNVAL."
#define sref_case_3_10_14 sref_case_3_10
#define desc_case_3_10_14 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_14(int child)
{
	short revents = 0;

	/* Linux bug. */
	if (test_poll(child, POLLNVAL, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLNVAL))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_3_10_14 = { &preamble_5, &test_case_3_10_14, &postamble_5 };

#define test_case_3_10_14_stream_0 (&test_3_10_14)
#define test_case_3_10_14_stream_1 (NULL)
#define test_case_3_10_14_stream_2 (NULL)

#define tgrp_case_3_10_15 test_group_3_10
#define numb_case_3_10_15 "3.10.15"
#define name_case_3_10_15 "Perform poll POLLMSG."
#define sref_case_3_10_15 sref_case_3_10
#define desc_case_3_10_15 "\
Check that poll() can be peformed on a Stream."

int
test_case_3_10_15(int child)
{
	return (__RESULT_SKIPPED);
#if 0
	short revents = 0;

	if (test_poll(child, POLLMSG, &revents, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (last_retval == 0)
		return (__RESULT_FAILURE);
	state++;
	if (!(revents & POLLMSG))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_3_10_15 = { &preamble_2_5, &test_case_3_10_15, &postamble_2 };

#define test_case_3_10_15_stream_0 (&test_3_10_15)
#define test_case_3_10_15_stream_1 (NULL)
#define test_case_3_10_15_stream_2 (NULL)

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
		numb_case_2_1_1, tgrp_case_2_1_1, NULL, name_case_2_1_1, NULL, desc_case_2_1_1, sref_case_2_1_1, {
	test_case_2_1_1_stream_0, test_case_2_1_1_stream_1, test_case_2_1_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_2, tgrp_case_2_1_2, NULL, name_case_2_1_2, NULL, desc_case_2_1_2, sref_case_2_1_2, {
	test_case_2_1_2_stream_0, test_case_2_1_2_stream_1, test_case_2_1_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_3, tgrp_case_2_1_3, NULL, name_case_2_1_3, NULL, desc_case_2_1_3, sref_case_2_1_3, {
	test_case_2_1_3_stream_0, test_case_2_1_3_stream_1, test_case_2_1_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_4, tgrp_case_2_1_4, NULL, name_case_2_1_4, NULL, desc_case_2_1_4, sref_case_2_1_4, {
	test_case_2_1_4_stream_0, test_case_2_1_4_stream_1, test_case_2_1_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1_5, tgrp_case_2_1_5, NULL, name_case_2_1_5, NULL, desc_case_2_1_5, sref_case_2_1_5, {
	test_case_2_1_5_stream_0, test_case_2_1_5_stream_1, test_case_2_1_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
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
		numb_case_2_2_6, tgrp_case_2_2_6, NULL, name_case_2_2_6, NULL, desc_case_2_2_6, sref_case_2_2_6, {
	test_case_2_2_6_stream_0, test_case_2_2_6_stream_1, test_case_2_2_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_7, tgrp_case_2_2_7, NULL, name_case_2_2_7, NULL, desc_case_2_2_7, sref_case_2_2_7, {
	test_case_2_2_7_stream_0, test_case_2_2_7_stream_1, test_case_2_2_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_8, tgrp_case_2_2_8, NULL, name_case_2_2_8, NULL, desc_case_2_2_8, sref_case_2_2_8, {
	test_case_2_2_8_stream_0, test_case_2_2_8_stream_1, test_case_2_2_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_9, tgrp_case_2_2_9, NULL, name_case_2_2_9, NULL, desc_case_2_2_9, sref_case_2_2_9, {
	test_case_2_2_9_stream_0, test_case_2_2_9_stream_1, test_case_2_2_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_10, tgrp_case_2_2_10, NULL, name_case_2_2_10, NULL, desc_case_2_2_10, sref_case_2_2_10, {
	test_case_2_2_10_stream_0, test_case_2_2_10_stream_1, test_case_2_2_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_11, tgrp_case_2_2_11, NULL, name_case_2_2_11, NULL, desc_case_2_2_11, sref_case_2_2_11, {
	test_case_2_2_11_stream_0, test_case_2_2_11_stream_1, test_case_2_2_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2_12, tgrp_case_2_2_12, NULL, name_case_2_2_12, NULL, desc_case_2_2_12, sref_case_2_2_12, {
	test_case_2_2_12_stream_0, test_case_2_2_12_stream_1, test_case_2_2_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
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
		numb_case_2_4_1, tgrp_case_2_4_1, NULL, name_case_2_4_1, NULL, desc_case_2_4_1, sref_case_2_4_1, {
	test_case_2_4_1_stream_0, test_case_2_4_1_stream_1, test_case_2_4_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_4_2, tgrp_case_2_4_2, NULL, name_case_2_4_2, NULL, desc_case_2_4_2, sref_case_2_4_2, {
	test_case_2_4_2_stream_0, test_case_2_4_2_stream_1, test_case_2_4_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_4_3, tgrp_case_2_4_3, NULL, name_case_2_4_3, NULL, desc_case_2_4_3, sref_case_2_4_3, {
	test_case_2_4_3_stream_0, test_case_2_4_3_stream_1, test_case_2_4_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_4_4, tgrp_case_2_4_4, NULL, name_case_2_4_4, NULL, desc_case_2_4_4, sref_case_2_4_4, {
	test_case_2_4_4_stream_0, test_case_2_4_4_stream_1, test_case_2_4_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_4_5, tgrp_case_2_4_5, NULL, name_case_2_4_5, NULL, desc_case_2_4_5, sref_case_2_4_5, {
	test_case_2_4_5_stream_0, test_case_2_4_5_stream_1, test_case_2_4_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_4_6, tgrp_case_2_4_6, NULL, name_case_2_4_6, NULL, desc_case_2_4_6, sref_case_2_4_6, {
	test_case_2_4_6_stream_0, test_case_2_4_6_stream_1, test_case_2_4_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_4_7, tgrp_case_2_4_7, NULL, name_case_2_4_7, NULL, desc_case_2_4_7, sref_case_2_4_7, {
	test_case_2_4_7_stream_0, test_case_2_4_7_stream_1, test_case_2_4_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_4_8, tgrp_case_2_4_8, NULL, name_case_2_4_8, NULL, desc_case_2_4_8, sref_case_2_4_8, {
	test_case_2_4_8_stream_0, test_case_2_4_8_stream_1, test_case_2_4_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_1, tgrp_case_2_5_1, NULL, name_case_2_5_1, NULL, desc_case_2_5_1, sref_case_2_5_1, {
	test_case_2_5_1_stream_0, test_case_2_5_1_stream_1, test_case_2_5_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_2, tgrp_case_2_5_2, NULL, name_case_2_5_2, NULL, desc_case_2_5_2, sref_case_2_5_2, {
	test_case_2_5_2_stream_0, test_case_2_5_2_stream_1, test_case_2_5_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_3, tgrp_case_2_5_3, NULL, name_case_2_5_3, NULL, desc_case_2_5_3, sref_case_2_5_3, {
	test_case_2_5_3_stream_0, test_case_2_5_3_stream_1, test_case_2_5_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_4, tgrp_case_2_5_4, NULL, name_case_2_5_4, NULL, desc_case_2_5_4, sref_case_2_5_4, {
	test_case_2_5_4_stream_0, test_case_2_5_4_stream_1, test_case_2_5_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_5, tgrp_case_2_5_5, NULL, name_case_2_5_5, NULL, desc_case_2_5_5, sref_case_2_5_5, {
	test_case_2_5_5_stream_0, test_case_2_5_5_stream_1, test_case_2_5_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_6, tgrp_case_2_5_6, NULL, name_case_2_5_6, NULL, desc_case_2_5_6, sref_case_2_5_6, {
	test_case_2_5_6_stream_0, test_case_2_5_6_stream_1, test_case_2_5_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_7, tgrp_case_2_5_7, NULL, name_case_2_5_7, NULL, desc_case_2_5_7, sref_case_2_5_7, {
	test_case_2_5_7_stream_0, test_case_2_5_7_stream_1, test_case_2_5_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_8, tgrp_case_2_5_8, NULL, name_case_2_5_8, NULL, desc_case_2_5_8, sref_case_2_5_8, {
	test_case_2_5_8_stream_0, test_case_2_5_8_stream_1, test_case_2_5_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_9, tgrp_case_2_5_9, NULL, name_case_2_5_9, NULL, desc_case_2_5_9, sref_case_2_5_9, {
	test_case_2_5_9_stream_0, test_case_2_5_9_stream_1, test_case_2_5_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_5_10, tgrp_case_2_5_10, NULL, name_case_2_5_10, NULL, desc_case_2_5_10, sref_case_2_5_10, {
	test_case_2_5_10_stream_0, test_case_2_5_10_stream_1, test_case_2_5_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_1, tgrp_case_2_6_1, NULL, name_case_2_6_1, NULL, desc_case_2_6_1, sref_case_2_6_1, {
	test_case_2_6_1_stream_0, test_case_2_6_1_stream_1, test_case_2_6_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_2, tgrp_case_2_6_2, NULL, name_case_2_6_2, NULL, desc_case_2_6_2, sref_case_2_6_2, {
	test_case_2_6_2_stream_0, test_case_2_6_2_stream_1, test_case_2_6_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_3, tgrp_case_2_6_3, NULL, name_case_2_6_3, NULL, desc_case_2_6_3, sref_case_2_6_3, {
	test_case_2_6_3_stream_0, test_case_2_6_3_stream_1, test_case_2_6_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_4, tgrp_case_2_6_4, NULL, name_case_2_6_4, NULL, desc_case_2_6_4, sref_case_2_6_4, {
	test_case_2_6_4_stream_0, test_case_2_6_4_stream_1, test_case_2_6_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_5, tgrp_case_2_6_5, NULL, name_case_2_6_5, NULL, desc_case_2_6_5, sref_case_2_6_5, {
	test_case_2_6_5_stream_0, test_case_2_6_5_stream_1, test_case_2_6_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_6, tgrp_case_2_6_6, NULL, name_case_2_6_6, NULL, desc_case_2_6_6, sref_case_2_6_6, {
	test_case_2_6_6_stream_0, test_case_2_6_6_stream_1, test_case_2_6_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_7, tgrp_case_2_6_7, NULL, name_case_2_6_7, NULL, desc_case_2_6_7, sref_case_2_6_7, {
	test_case_2_6_7_stream_0, test_case_2_6_7_stream_1, test_case_2_6_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_8, tgrp_case_2_6_8, NULL, name_case_2_6_8, NULL, desc_case_2_6_8, sref_case_2_6_8, {
	test_case_2_6_8_stream_0, test_case_2_6_8_stream_1, test_case_2_6_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_9, tgrp_case_2_6_9, NULL, name_case_2_6_9, NULL, desc_case_2_6_9, sref_case_2_6_9, {
	test_case_2_6_9_stream_0, test_case_2_6_9_stream_1, test_case_2_6_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_10, tgrp_case_2_6_10, NULL, name_case_2_6_10, NULL, desc_case_2_6_10, sref_case_2_6_10, {
	test_case_2_6_10_stream_0, test_case_2_6_10_stream_1, test_case_2_6_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_11, tgrp_case_2_6_11, NULL, name_case_2_6_11, NULL, desc_case_2_6_11, sref_case_2_6_11, {
	test_case_2_6_11_stream_0, test_case_2_6_11_stream_1, test_case_2_6_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_12, tgrp_case_2_6_12, NULL, name_case_2_6_12, NULL, desc_case_2_6_12, sref_case_2_6_12, {
	test_case_2_6_12_stream_0, test_case_2_6_12_stream_1, test_case_2_6_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_13, tgrp_case_2_6_13, NULL, name_case_2_6_13, NULL, desc_case_2_6_13, sref_case_2_6_13, {
	test_case_2_6_13_stream_0, test_case_2_6_13_stream_1, test_case_2_6_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_14, tgrp_case_2_6_14, NULL, name_case_2_6_14, NULL, desc_case_2_6_14, sref_case_2_6_14, {
	test_case_2_6_14_stream_0, test_case_2_6_14_stream_1, test_case_2_6_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_6_15, tgrp_case_2_6_15, NULL, name_case_2_6_15, NULL, desc_case_2_6_15, sref_case_2_6_15, {
	test_case_2_6_15_stream_0, test_case_2_6_15_stream_1, test_case_2_6_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_1, tgrp_case_2_7_1, NULL, name_case_2_7_1, NULL, desc_case_2_7_1, sref_case_2_7_1, {
	test_case_2_7_1_stream_0, test_case_2_7_1_stream_1, test_case_2_7_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_2, tgrp_case_2_7_2, NULL, name_case_2_7_2, NULL, desc_case_2_7_2, sref_case_2_7_2, {
	test_case_2_7_2_stream_0, test_case_2_7_2_stream_1, test_case_2_7_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_3, tgrp_case_2_7_3, NULL, name_case_2_7_3, NULL, desc_case_2_7_3, sref_case_2_7_3, {
	test_case_2_7_3_stream_0, test_case_2_7_3_stream_1, test_case_2_7_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_4, tgrp_case_2_7_4, NULL, name_case_2_7_4, NULL, desc_case_2_7_4, sref_case_2_7_4, {
	test_case_2_7_4_stream_0, test_case_2_7_4_stream_1, test_case_2_7_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_5, tgrp_case_2_7_5, NULL, name_case_2_7_5, NULL, desc_case_2_7_5, sref_case_2_7_5, {
	test_case_2_7_5_stream_0, test_case_2_7_5_stream_1, test_case_2_7_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_6, tgrp_case_2_7_6, NULL, name_case_2_7_6, NULL, desc_case_2_7_6, sref_case_2_7_6, {
	test_case_2_7_6_stream_0, test_case_2_7_6_stream_1, test_case_2_7_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_7, tgrp_case_2_7_7, NULL, name_case_2_7_7, NULL, desc_case_2_7_7, sref_case_2_7_7, {
	test_case_2_7_7_stream_0, test_case_2_7_7_stream_1, test_case_2_7_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_8, tgrp_case_2_7_8, NULL, name_case_2_7_8, NULL, desc_case_2_7_8, sref_case_2_7_8, {
	test_case_2_7_8_stream_0, test_case_2_7_8_stream_1, test_case_2_7_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_9, tgrp_case_2_7_9, NULL, name_case_2_7_9, NULL, desc_case_2_7_9, sref_case_2_7_9, {
	test_case_2_7_9_stream_0, test_case_2_7_9_stream_1, test_case_2_7_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_10, tgrp_case_2_7_10, NULL, name_case_2_7_10, NULL, desc_case_2_7_10, sref_case_2_7_10, {
	test_case_2_7_10_stream_0, test_case_2_7_10_stream_1, test_case_2_7_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_11, tgrp_case_2_7_11, NULL, name_case_2_7_11, NULL, desc_case_2_7_11, sref_case_2_7_11, {
	test_case_2_7_11_stream_0, test_case_2_7_11_stream_1, test_case_2_7_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_12, tgrp_case_2_7_12, NULL, name_case_2_7_12, NULL, desc_case_2_7_12, sref_case_2_7_12, {
	test_case_2_7_12_stream_0, test_case_2_7_12_stream_1, test_case_2_7_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_13, tgrp_case_2_7_13, NULL, name_case_2_7_13, NULL, desc_case_2_7_13, sref_case_2_7_13, {
	test_case_2_7_13_stream_0, test_case_2_7_13_stream_1, test_case_2_7_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_14, tgrp_case_2_7_14, NULL, name_case_2_7_14, NULL, desc_case_2_7_14, sref_case_2_7_14, {
	test_case_2_7_14_stream_0, test_case_2_7_14_stream_1, test_case_2_7_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_15, tgrp_case_2_7_15, NULL, name_case_2_7_15, NULL, desc_case_2_7_15, sref_case_2_7_15, {
	test_case_2_7_15_stream_0, test_case_2_7_15_stream_1, test_case_2_7_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_7_16, tgrp_case_2_7_16, NULL, name_case_2_7_16, NULL, desc_case_2_7_16, sref_case_2_7_16, {
	test_case_2_7_16_stream_0, test_case_2_7_16_stream_1, test_case_2_7_16_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_1, tgrp_case_2_8_1, NULL, name_case_2_8_1, NULL, desc_case_2_8_1, sref_case_2_8_1, {
	test_case_2_8_1_stream_0, test_case_2_8_1_stream_1, test_case_2_8_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_2, tgrp_case_2_8_2, NULL, name_case_2_8_2, NULL, desc_case_2_8_2, sref_case_2_8_2, {
	test_case_2_8_2_stream_0, test_case_2_8_2_stream_1, test_case_2_8_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_3, tgrp_case_2_8_3, NULL, name_case_2_8_3, NULL, desc_case_2_8_3, sref_case_2_8_3, {
	test_case_2_8_3_stream_0, test_case_2_8_3_stream_1, test_case_2_8_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_4, tgrp_case_2_8_4, NULL, name_case_2_8_4, NULL, desc_case_2_8_4, sref_case_2_8_4, {
	test_case_2_8_4_stream_0, test_case_2_8_4_stream_1, test_case_2_8_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_5, tgrp_case_2_8_5, NULL, name_case_2_8_5, NULL, desc_case_2_8_5, sref_case_2_8_5, {
	test_case_2_8_5_stream_0, test_case_2_8_5_stream_1, test_case_2_8_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_6, tgrp_case_2_8_6, NULL, name_case_2_8_6, NULL, desc_case_2_8_6, sref_case_2_8_6, {
	test_case_2_8_6_stream_0, test_case_2_8_6_stream_1, test_case_2_8_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_7, tgrp_case_2_8_7, NULL, name_case_2_8_7, NULL, desc_case_2_8_7, sref_case_2_8_7, {
	test_case_2_8_7_stream_0, test_case_2_8_7_stream_1, test_case_2_8_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_8, tgrp_case_2_8_8, NULL, name_case_2_8_8, NULL, desc_case_2_8_8, sref_case_2_8_8, {
	test_case_2_8_8_stream_0, test_case_2_8_8_stream_1, test_case_2_8_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_9, tgrp_case_2_8_9, NULL, name_case_2_8_9, NULL, desc_case_2_8_9, sref_case_2_8_9, {
	test_case_2_8_9_stream_0, test_case_2_8_9_stream_1, test_case_2_8_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_10, tgrp_case_2_8_10, NULL, name_case_2_8_10, NULL, desc_case_2_8_10, sref_case_2_8_10, {
	test_case_2_8_10_stream_0, test_case_2_8_10_stream_1, test_case_2_8_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_8_11, tgrp_case_2_8_11, NULL, name_case_2_8_11, NULL, desc_case_2_8_11, sref_case_2_8_11, {
	test_case_2_8_11_stream_0, test_case_2_8_11_stream_1, test_case_2_8_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_1, tgrp_case_2_9_1, NULL, name_case_2_9_1, NULL, desc_case_2_9_1, sref_case_2_9_1, {
	test_case_2_9_1_stream_0, test_case_2_9_1_stream_1, test_case_2_9_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_2, tgrp_case_2_9_2, NULL, name_case_2_9_2, NULL, desc_case_2_9_2, sref_case_2_9_2, {
	test_case_2_9_2_stream_0, test_case_2_9_2_stream_1, test_case_2_9_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_3, tgrp_case_2_9_3, NULL, name_case_2_9_3, NULL, desc_case_2_9_3, sref_case_2_9_3, {
	test_case_2_9_3_stream_0, test_case_2_9_3_stream_1, test_case_2_9_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_4, tgrp_case_2_9_4, NULL, name_case_2_9_4, NULL, desc_case_2_9_4, sref_case_2_9_4, {
	test_case_2_9_4_stream_0, test_case_2_9_4_stream_1, test_case_2_9_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_5, tgrp_case_2_9_5, NULL, name_case_2_9_5, NULL, desc_case_2_9_5, sref_case_2_9_5, {
	test_case_2_9_5_stream_0, test_case_2_9_5_stream_1, test_case_2_9_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_6, tgrp_case_2_9_6, NULL, name_case_2_9_6, NULL, desc_case_2_9_6, sref_case_2_9_6, {
	test_case_2_9_6_stream_0, test_case_2_9_6_stream_1, test_case_2_9_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_7, tgrp_case_2_9_7, NULL, name_case_2_9_7, NULL, desc_case_2_9_7, sref_case_2_9_7, {
	test_case_2_9_7_stream_0, test_case_2_9_7_stream_1, test_case_2_9_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_8, tgrp_case_2_9_8, NULL, name_case_2_9_8, NULL, desc_case_2_9_8, sref_case_2_9_8, {
	test_case_2_9_8_stream_0, test_case_2_9_8_stream_1, test_case_2_9_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_9, tgrp_case_2_9_9, NULL, name_case_2_9_9, NULL, desc_case_2_9_9, sref_case_2_9_9, {
	test_case_2_9_9_stream_0, test_case_2_9_9_stream_1, test_case_2_9_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_10, tgrp_case_2_9_10, NULL, name_case_2_9_10, NULL, desc_case_2_9_10, sref_case_2_9_10, {
	test_case_2_9_10_stream_0, test_case_2_9_10_stream_1, test_case_2_9_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_11, tgrp_case_2_9_11, NULL, name_case_2_9_11, NULL, desc_case_2_9_11, sref_case_2_9_11, {
	test_case_2_9_11_stream_0, test_case_2_9_11_stream_1, test_case_2_9_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_12_1, tgrp_case_2_9_12_1, NULL, name_case_2_9_12_1, NULL, desc_case_2_9_12_1, sref_case_2_9_12_1, {
	test_case_2_9_12_1_stream_0, test_case_2_9_12_1_stream_1, test_case_2_9_12_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_12_2, tgrp_case_2_9_12_2, NULL, name_case_2_9_12_2, NULL, desc_case_2_9_12_2, sref_case_2_9_12_2, {
	test_case_2_9_12_2_stream_0, test_case_2_9_12_2_stream_1, test_case_2_9_12_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_13_1, tgrp_case_2_9_13_1, NULL, name_case_2_9_13_1, NULL, desc_case_2_9_13_1, sref_case_2_9_13_1, {
	test_case_2_9_13_1_stream_0, test_case_2_9_13_1_stream_1, test_case_2_9_13_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_13_2, tgrp_case_2_9_13_2, NULL, name_case_2_9_13_2, NULL, desc_case_2_9_13_2, sref_case_2_9_13_2, {
	test_case_2_9_13_2_stream_0, test_case_2_9_13_2_stream_1, test_case_2_9_13_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_14_1, tgrp_case_2_9_14_1, NULL, name_case_2_9_14_1, NULL, desc_case_2_9_14_1, sref_case_2_9_14_1, {
	test_case_2_9_14_1_stream_0, test_case_2_9_14_1_stream_1, test_case_2_9_14_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_14_2, tgrp_case_2_9_14_2, NULL, name_case_2_9_14_2, NULL, desc_case_2_9_14_2, sref_case_2_9_14_2, {
	test_case_2_9_14_2_stream_0, test_case_2_9_14_2_stream_1, test_case_2_9_14_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_15, tgrp_case_2_9_15, NULL, name_case_2_9_15, NULL, desc_case_2_9_15, sref_case_2_9_15, {
	test_case_2_9_15_stream_0, test_case_2_9_15_stream_1, test_case_2_9_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_16_1, tgrp_case_2_9_16_1, NULL, name_case_2_9_16_1, NULL, desc_case_2_9_16_1, sref_case_2_9_16_1, {
	test_case_2_9_16_1_stream_0, test_case_2_9_16_1_stream_1, test_case_2_9_16_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_16_2, tgrp_case_2_9_16_2, NULL, name_case_2_9_16_2, NULL, desc_case_2_9_16_2, sref_case_2_9_16_2, {
	test_case_2_9_16_2_stream_0, test_case_2_9_16_2_stream_1, test_case_2_9_16_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_16_3, tgrp_case_2_9_16_3, NULL, name_case_2_9_16_3, NULL, desc_case_2_9_16_3, sref_case_2_9_16_3, {
	test_case_2_9_16_3_stream_0, test_case_2_9_16_3_stream_1, test_case_2_9_16_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_17, tgrp_case_2_9_17, NULL, name_case_2_9_17, NULL, desc_case_2_9_17, sref_case_2_9_17, {
	test_case_2_9_17_stream_0, test_case_2_9_17_stream_1, test_case_2_9_17_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_9_18, tgrp_case_2_9_18, NULL, name_case_2_9_18, NULL, desc_case_2_9_18, sref_case_2_9_18, {
	test_case_2_9_18_stream_0, test_case_2_9_18_stream_1, test_case_2_9_18_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_1, tgrp_case_2_10_1, NULL, name_case_2_10_1, NULL, desc_case_2_10_1, sref_case_2_10_1, {
	test_case_2_10_1_stream_0, test_case_2_10_1_stream_1, test_case_2_10_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_2, tgrp_case_2_10_2, NULL, name_case_2_10_2, NULL, desc_case_2_10_2, sref_case_2_10_2, {
	test_case_2_10_2_stream_0, test_case_2_10_2_stream_1, test_case_2_10_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_3, tgrp_case_2_10_3, NULL, name_case_2_10_3, NULL, desc_case_2_10_3, sref_case_2_10_3, {
	test_case_2_10_3_stream_0, test_case_2_10_3_stream_1, test_case_2_10_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_4, tgrp_case_2_10_4, NULL, name_case_2_10_4, NULL, desc_case_2_10_4, sref_case_2_10_4, {
	test_case_2_10_4_stream_0, test_case_2_10_4_stream_1, test_case_2_10_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_5, tgrp_case_2_10_5, NULL, name_case_2_10_5, NULL, desc_case_2_10_5, sref_case_2_10_5, {
	test_case_2_10_5_stream_0, test_case_2_10_5_stream_1, test_case_2_10_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_6, tgrp_case_2_10_6, NULL, name_case_2_10_6, NULL, desc_case_2_10_6, sref_case_2_10_6, {
	test_case_2_10_6_stream_0, test_case_2_10_6_stream_1, test_case_2_10_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_7, tgrp_case_2_10_7, NULL, name_case_2_10_7, NULL, desc_case_2_10_7, sref_case_2_10_7, {
	test_case_2_10_7_stream_0, test_case_2_10_7_stream_1, test_case_2_10_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_10_8, tgrp_case_2_10_8, NULL, name_case_2_10_8, NULL, desc_case_2_10_8, sref_case_2_10_8, {
	test_case_2_10_8_stream_0, test_case_2_10_8_stream_1, test_case_2_10_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_1, tgrp_case_2_11_1, NULL, name_case_2_11_1, NULL, desc_case_2_11_1, sref_case_2_11_1, {
	test_case_2_11_1_stream_0, test_case_2_11_1_stream_1, test_case_2_11_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_2, tgrp_case_2_11_2, NULL, name_case_2_11_2, NULL, desc_case_2_11_2, sref_case_2_11_2, {
	test_case_2_11_2_stream_0, test_case_2_11_2_stream_1, test_case_2_11_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_3, tgrp_case_2_11_3, NULL, name_case_2_11_3, NULL, desc_case_2_11_3, sref_case_2_11_3, {
	test_case_2_11_3_stream_0, test_case_2_11_3_stream_1, test_case_2_11_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_4, tgrp_case_2_11_4, NULL, name_case_2_11_4, NULL, desc_case_2_11_4, sref_case_2_11_4, {
	test_case_2_11_4_stream_0, test_case_2_11_4_stream_1, test_case_2_11_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_5, tgrp_case_2_11_5, NULL, name_case_2_11_5, NULL, desc_case_2_11_5, sref_case_2_11_5, {
	test_case_2_11_5_stream_0, test_case_2_11_5_stream_1, test_case_2_11_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_6, tgrp_case_2_11_6, NULL, name_case_2_11_6, NULL, desc_case_2_11_6, sref_case_2_11_6, {
	test_case_2_11_6_stream_0, test_case_2_11_6_stream_1, test_case_2_11_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_7, tgrp_case_2_11_7, NULL, name_case_2_11_7, NULL, desc_case_2_11_7, sref_case_2_11_7, {
	test_case_2_11_7_stream_0, test_case_2_11_7_stream_1, test_case_2_11_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_8, tgrp_case_2_11_8, NULL, name_case_2_11_8, NULL, desc_case_2_11_8, sref_case_2_11_8, {
	test_case_2_11_8_stream_0, test_case_2_11_8_stream_1, test_case_2_11_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_9, tgrp_case_2_11_9, NULL, name_case_2_11_9, NULL, desc_case_2_11_9, sref_case_2_11_9, {
	test_case_2_11_9_stream_0, test_case_2_11_9_stream_1, test_case_2_11_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_11_10, tgrp_case_2_11_10, NULL, name_case_2_11_10, NULL, desc_case_2_11_10, sref_case_2_11_10, {
	test_case_2_11_10_stream_0, test_case_2_11_10_stream_1, test_case_2_11_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_1, tgrp_case_2_12_1, NULL, name_case_2_12_1, NULL, desc_case_2_12_1, sref_case_2_12_1, {
	test_case_2_12_1_stream_0, test_case_2_12_1_stream_1, test_case_2_12_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_2, tgrp_case_2_12_2, NULL, name_case_2_12_2, NULL, desc_case_2_12_2, sref_case_2_12_2, {
	test_case_2_12_2_stream_0, test_case_2_12_2_stream_1, test_case_2_12_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_3, tgrp_case_2_12_3, NULL, name_case_2_12_3, NULL, desc_case_2_12_3, sref_case_2_12_3, {
	test_case_2_12_3_stream_0, test_case_2_12_3_stream_1, test_case_2_12_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_4, tgrp_case_2_12_4, NULL, name_case_2_12_4, NULL, desc_case_2_12_4, sref_case_2_12_4, {
	test_case_2_12_4_stream_0, test_case_2_12_4_stream_1, test_case_2_12_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_5, tgrp_case_2_12_5, NULL, name_case_2_12_5, NULL, desc_case_2_12_5, sref_case_2_12_5, {
	test_case_2_12_5_stream_0, test_case_2_12_5_stream_1, test_case_2_12_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_6, tgrp_case_2_12_6, NULL, name_case_2_12_6, NULL, desc_case_2_12_6, sref_case_2_12_6, {
	test_case_2_12_6_stream_0, test_case_2_12_6_stream_1, test_case_2_12_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_7, tgrp_case_2_12_7, NULL, name_case_2_12_7, NULL, desc_case_2_12_7, sref_case_2_12_7, {
	test_case_2_12_7_stream_0, test_case_2_12_7_stream_1, test_case_2_12_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_8, tgrp_case_2_12_8, NULL, name_case_2_12_8, NULL, desc_case_2_12_8, sref_case_2_12_8, {
	test_case_2_12_8_stream_0, test_case_2_12_8_stream_1, test_case_2_12_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_9, tgrp_case_2_12_9, NULL, name_case_2_12_9, NULL, desc_case_2_12_9, sref_case_2_12_9, {
	test_case_2_12_9_stream_0, test_case_2_12_9_stream_1, test_case_2_12_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_12_10, tgrp_case_2_12_10, NULL, name_case_2_12_10, NULL, desc_case_2_12_10, sref_case_2_12_10, {
	test_case_2_12_10_stream_0, test_case_2_12_10_stream_1, test_case_2_12_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_13_1, tgrp_case_2_13_1, NULL, name_case_2_13_1, NULL, desc_case_2_13_1, sref_case_2_13_1, {
	test_case_2_13_1_stream_0, test_case_2_13_1_stream_1, test_case_2_13_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_13_2, tgrp_case_2_13_2, NULL, name_case_2_13_2, NULL, desc_case_2_13_2, sref_case_2_13_2, {
	test_case_2_13_2_stream_0, test_case_2_13_2_stream_1, test_case_2_13_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_13_3, tgrp_case_2_13_3, NULL, name_case_2_13_3, NULL, desc_case_2_13_3, sref_case_2_13_3, {
	test_case_2_13_3_stream_0, test_case_2_13_3_stream_1, test_case_2_13_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_13_4, tgrp_case_2_13_4, NULL, name_case_2_13_4, NULL, desc_case_2_13_4, sref_case_2_13_4, {
	test_case_2_13_4_stream_0, test_case_2_13_4_stream_1, test_case_2_13_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_13_5, tgrp_case_2_13_5, NULL, name_case_2_13_5, NULL, desc_case_2_13_5, sref_case_2_13_5, {
	test_case_2_13_5_stream_0, test_case_2_13_5_stream_1, test_case_2_13_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_13_6, tgrp_case_2_13_6, NULL, name_case_2_13_6, NULL, desc_case_2_13_6, sref_case_2_13_6, {
	test_case_2_13_6_stream_0, test_case_2_13_6_stream_1, test_case_2_13_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_1, tgrp_case_2_14_1, NULL, name_case_2_14_1, NULL, desc_case_2_14_1, sref_case_2_14_1, {
	test_case_2_14_1_stream_0, test_case_2_14_1_stream_1, test_case_2_14_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_2, tgrp_case_2_14_2, NULL, name_case_2_14_2, NULL, desc_case_2_14_2, sref_case_2_14_2, {
	test_case_2_14_2_stream_0, test_case_2_14_2_stream_1, test_case_2_14_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_3, tgrp_case_2_14_3, NULL, name_case_2_14_3, NULL, desc_case_2_14_3, sref_case_2_14_3, {
	test_case_2_14_3_stream_0, test_case_2_14_3_stream_1, test_case_2_14_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_4, tgrp_case_2_14_4, NULL, name_case_2_14_4, NULL, desc_case_2_14_4, sref_case_2_14_4, {
	test_case_2_14_4_stream_0, test_case_2_14_4_stream_1, test_case_2_14_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_5, tgrp_case_2_14_5, NULL, name_case_2_14_5, NULL, desc_case_2_14_5, sref_case_2_14_5, {
	test_case_2_14_5_stream_0, test_case_2_14_5_stream_1, test_case_2_14_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_6, tgrp_case_2_14_6, NULL, name_case_2_14_6, NULL, desc_case_2_14_6, sref_case_2_14_6, {
	test_case_2_14_6_stream_0, test_case_2_14_6_stream_1, test_case_2_14_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_7, tgrp_case_2_14_7, NULL, name_case_2_14_7, NULL, desc_case_2_14_7, sref_case_2_14_7, {
	test_case_2_14_7_stream_0, test_case_2_14_7_stream_1, test_case_2_14_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_14_8, tgrp_case_2_14_8, NULL, name_case_2_14_8, NULL, desc_case_2_14_8, sref_case_2_14_8, {
	test_case_2_14_8_stream_0, test_case_2_14_8_stream_1, test_case_2_14_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_1, tgrp_case_2_15_1, NULL, name_case_2_15_1, NULL, desc_case_2_15_1, sref_case_2_15_1, {
	test_case_2_15_1_stream_0, test_case_2_15_1_stream_1, test_case_2_15_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_2, tgrp_case_2_15_2, NULL, name_case_2_15_2, NULL, desc_case_2_15_2, sref_case_2_15_2, {
	test_case_2_15_2_stream_0, test_case_2_15_2_stream_1, test_case_2_15_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_3, tgrp_case_2_15_3, NULL, name_case_2_15_3, NULL, desc_case_2_15_3, sref_case_2_15_3, {
	test_case_2_15_3_stream_0, test_case_2_15_3_stream_1, test_case_2_15_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_4, tgrp_case_2_15_4, NULL, name_case_2_15_4, NULL, desc_case_2_15_4, sref_case_2_15_4, {
	test_case_2_15_4_stream_0, test_case_2_15_4_stream_1, test_case_2_15_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_5, tgrp_case_2_15_5, NULL, name_case_2_15_5, NULL, desc_case_2_15_5, sref_case_2_15_5, {
	test_case_2_15_5_stream_0, test_case_2_15_5_stream_1, test_case_2_15_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_6, tgrp_case_2_15_6, NULL, name_case_2_15_6, NULL, desc_case_2_15_6, sref_case_2_15_6, {
	test_case_2_15_6_stream_0, test_case_2_15_6_stream_1, test_case_2_15_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_7, tgrp_case_2_15_7, NULL, name_case_2_15_7, NULL, desc_case_2_15_7, sref_case_2_15_7, {
	test_case_2_15_7_stream_0, test_case_2_15_7_stream_1, test_case_2_15_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_8, tgrp_case_2_15_8, NULL, name_case_2_15_8, NULL, desc_case_2_15_8, sref_case_2_15_8, {
	test_case_2_15_8_stream_0, test_case_2_15_8_stream_1, test_case_2_15_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_15_9, tgrp_case_2_15_9, NULL, name_case_2_15_9, NULL, desc_case_2_15_9, sref_case_2_15_9, {
	test_case_2_15_9_stream_0, test_case_2_15_9_stream_1, test_case_2_15_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_1, tgrp_case_2_16_1, NULL, name_case_2_16_1, NULL, desc_case_2_16_1, sref_case_2_16_1, {
	test_case_2_16_1_stream_0, test_case_2_16_1_stream_1, test_case_2_16_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_2_1, tgrp_case_2_16_2_1, NULL, name_case_2_16_2_1, NULL, desc_case_2_16_2_1, sref_case_2_16_2_1, {
	test_case_2_16_2_1_stream_0, test_case_2_16_2_1_stream_1, test_case_2_16_2_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_2_2, tgrp_case_2_16_2_2, NULL, name_case_2_16_2_2, NULL, desc_case_2_16_2_2, sref_case_2_16_2_2, {
	test_case_2_16_2_2_stream_0, test_case_2_16_2_2_stream_1, test_case_2_16_2_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_3_1, tgrp_case_2_16_3_1, NULL, name_case_2_16_3_1, NULL, desc_case_2_16_3_1, sref_case_2_16_3_1, {
	test_case_2_16_3_1_stream_0, test_case_2_16_3_1_stream_1, test_case_2_16_3_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_3_2, tgrp_case_2_16_3_2, NULL, name_case_2_16_3_2, NULL, desc_case_2_16_3_2, sref_case_2_16_3_2, {
	test_case_2_16_3_2_stream_0, test_case_2_16_3_2_stream_1, test_case_2_16_3_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_4_1, tgrp_case_2_16_4_1, NULL, name_case_2_16_4_1, NULL, desc_case_2_16_4_1, sref_case_2_16_4_1, {
	test_case_2_16_4_1_stream_0, test_case_2_16_4_1_stream_1, test_case_2_16_4_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_4_2, tgrp_case_2_16_4_2, NULL, name_case_2_16_4_2, NULL, desc_case_2_16_4_2, sref_case_2_16_4_2, {
	test_case_2_16_4_2_stream_0, test_case_2_16_4_2_stream_1, test_case_2_16_4_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_5_1, tgrp_case_2_16_5_1, NULL, name_case_2_16_5_1, NULL, desc_case_2_16_5_1, sref_case_2_16_5_1, {
	test_case_2_16_5_1_stream_0, test_case_2_16_5_1_stream_1, test_case_2_16_5_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_5_2, tgrp_case_2_16_5_2, NULL, name_case_2_16_5_2, NULL, desc_case_2_16_5_2, sref_case_2_16_5_2, {
	test_case_2_16_5_2_stream_0, test_case_2_16_5_2_stream_1, test_case_2_16_5_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_6_1, tgrp_case_2_16_6_1, NULL, name_case_2_16_6_1, NULL, desc_case_2_16_6_1, sref_case_2_16_6_1, {
	test_case_2_16_6_1_stream_0, test_case_2_16_6_1_stream_1, test_case_2_16_6_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_6_2, tgrp_case_2_16_6_2, NULL, name_case_2_16_6_2, NULL, desc_case_2_16_6_2, sref_case_2_16_6_2, {
	test_case_2_16_6_2_stream_0, test_case_2_16_6_2_stream_1, test_case_2_16_6_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_7, tgrp_case_2_16_7, NULL, name_case_2_16_7, NULL, desc_case_2_16_7, sref_case_2_16_7, {
	test_case_2_16_7_stream_0, test_case_2_16_7_stream_1, test_case_2_16_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_8, tgrp_case_2_16_8, NULL, name_case_2_16_8, NULL, desc_case_2_16_8, sref_case_2_16_8, {
	test_case_2_16_8_stream_0, test_case_2_16_8_stream_1, test_case_2_16_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_9, tgrp_case_2_16_9, NULL, name_case_2_16_9, NULL, desc_case_2_16_9, sref_case_2_16_9, {
	test_case_2_16_9_stream_0, test_case_2_16_9_stream_1, test_case_2_16_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_10, tgrp_case_2_16_10, NULL, name_case_2_16_10, NULL, desc_case_2_16_10, sref_case_2_16_10, {
	test_case_2_16_10_stream_0, test_case_2_16_10_stream_1, test_case_2_16_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_11, tgrp_case_2_16_11, NULL, name_case_2_16_11, NULL, desc_case_2_16_11, sref_case_2_16_11, {
	test_case_2_16_11_stream_0, test_case_2_16_11_stream_1, test_case_2_16_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_12, tgrp_case_2_16_12, NULL, name_case_2_16_12, NULL, desc_case_2_16_12, sref_case_2_16_12, {
	test_case_2_16_12_stream_0, test_case_2_16_12_stream_1, test_case_2_16_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_13, tgrp_case_2_16_13, NULL, name_case_2_16_13, NULL, desc_case_2_16_13, sref_case_2_16_13, {
	test_case_2_16_13_stream_0, test_case_2_16_13_stream_1, test_case_2_16_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_14, tgrp_case_2_16_14, NULL, name_case_2_16_14, NULL, desc_case_2_16_14, sref_case_2_16_14, {
	test_case_2_16_14_stream_0, test_case_2_16_14_stream_1, test_case_2_16_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_15, tgrp_case_2_16_15, NULL, name_case_2_16_15, NULL, desc_case_2_16_15, sref_case_2_16_15, {
	test_case_2_16_15_stream_0, test_case_2_16_15_stream_1, test_case_2_16_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_16, tgrp_case_2_16_16, NULL, name_case_2_16_16, NULL, desc_case_2_16_16, sref_case_2_16_16, {
	test_case_2_16_16_stream_0, test_case_2_16_16_stream_1, test_case_2_16_16_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_16_17, tgrp_case_2_16_17, NULL, name_case_2_16_17, NULL, desc_case_2_16_17, sref_case_2_16_17, {
	test_case_2_16_17_stream_0, test_case_2_16_17_stream_1, test_case_2_16_17_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_17_1, tgrp_case_2_17_1, NULL, name_case_2_17_1, NULL, desc_case_2_17_1, sref_case_2_17_1, {
	test_case_2_17_1_stream_0, test_case_2_17_1_stream_1, test_case_2_17_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_17_2, tgrp_case_2_17_2, NULL, name_case_2_17_2, NULL, desc_case_2_17_2, sref_case_2_17_2, {
	test_case_2_17_2_stream_0, test_case_2_17_2_stream_1, test_case_2_17_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_17_3, tgrp_case_2_17_3, NULL, name_case_2_17_3, NULL, desc_case_2_17_3, sref_case_2_17_3, {
	test_case_2_17_3_stream_0, test_case_2_17_3_stream_1, test_case_2_17_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_17_4, tgrp_case_2_17_4, NULL, name_case_2_17_4, NULL, desc_case_2_17_4, sref_case_2_17_4, {
	test_case_2_17_4_stream_0, test_case_2_17_4_stream_1, test_case_2_17_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_17_5, tgrp_case_2_17_5, NULL, name_case_2_17_5, NULL, desc_case_2_17_5, sref_case_2_17_5, {
	test_case_2_17_5_stream_0, test_case_2_17_5_stream_1, test_case_2_17_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_17_6, tgrp_case_2_17_6, NULL, name_case_2_17_6, NULL, desc_case_2_17_6, sref_case_2_17_6, {
	test_case_2_17_6_stream_0, test_case_2_17_6_stream_1, test_case_2_17_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_18, tgrp_case_2_18, NULL, name_case_2_18, NULL, desc_case_2_18, sref_case_2_18, {
	test_case_2_18_stream_0, test_case_2_18_stream_1, test_case_2_18_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_1, tgrp_case_2_19_1, NULL, name_case_2_19_1, NULL, desc_case_2_19_1, sref_case_2_19_1, {
	test_case_2_19_1_stream_0, test_case_2_19_1_stream_1, test_case_2_19_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_2, tgrp_case_2_19_2, NULL, name_case_2_19_2, NULL, desc_case_2_19_2, sref_case_2_19_2, {
	test_case_2_19_2_stream_0, test_case_2_19_2_stream_1, test_case_2_19_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_3, tgrp_case_2_19_3, NULL, name_case_2_19_3, NULL, desc_case_2_19_3, sref_case_2_19_3, {
	test_case_2_19_3_stream_0, test_case_2_19_3_stream_1, test_case_2_19_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_4, tgrp_case_2_19_4, NULL, name_case_2_19_4, NULL, desc_case_2_19_4, sref_case_2_19_4, {
	test_case_2_19_4_stream_0, test_case_2_19_4_stream_1, test_case_2_19_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_5, tgrp_case_2_19_5, NULL, name_case_2_19_5, NULL, desc_case_2_19_5, sref_case_2_19_5, {
	test_case_2_19_5_stream_0, test_case_2_19_5_stream_1, test_case_2_19_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_6, tgrp_case_2_19_6, NULL, name_case_2_19_6, NULL, desc_case_2_19_6, sref_case_2_19_6, {
	test_case_2_19_6_stream_0, test_case_2_19_6_stream_1, test_case_2_19_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_7, tgrp_case_2_19_7, NULL, name_case_2_19_7, NULL, desc_case_2_19_7, sref_case_2_19_7, {
	test_case_2_19_7_stream_0, test_case_2_19_7_stream_1, test_case_2_19_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_8, tgrp_case_2_19_8, NULL, name_case_2_19_8, NULL, desc_case_2_19_8, sref_case_2_19_8, {
	test_case_2_19_8_stream_0, test_case_2_19_8_stream_1, test_case_2_19_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_9, tgrp_case_2_19_9, NULL, name_case_2_19_9, NULL, desc_case_2_19_9, sref_case_2_19_9, {
	test_case_2_19_9_stream_0, test_case_2_19_9_stream_1, test_case_2_19_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_10, tgrp_case_2_19_10, NULL, name_case_2_19_10, NULL, desc_case_2_19_10, sref_case_2_19_10, {
	test_case_2_19_10_stream_0, test_case_2_19_10_stream_1, test_case_2_19_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_11, tgrp_case_2_19_11, NULL, name_case_2_19_11, NULL, desc_case_2_19_11, sref_case_2_19_11, {
	test_case_2_19_11_stream_0, test_case_2_19_11_stream_1, test_case_2_19_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_12, tgrp_case_2_19_12, NULL, name_case_2_19_12, NULL, desc_case_2_19_12, sref_case_2_19_12, {
	test_case_2_19_12_stream_0, test_case_2_19_12_stream_1, test_case_2_19_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_13, tgrp_case_2_19_13, NULL, name_case_2_19_13, NULL, desc_case_2_19_13, sref_case_2_19_13, {
	test_case_2_19_13_stream_0, test_case_2_19_13_stream_1, test_case_2_19_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_19_14, tgrp_case_2_19_14, NULL, name_case_2_19_14, NULL, desc_case_2_19_14, sref_case_2_19_14, {
	test_case_2_19_14_stream_0, test_case_2_19_14_stream_1, test_case_2_19_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_1, tgrp_case_2_20_1, NULL, name_case_2_20_1, NULL, desc_case_2_20_1, sref_case_2_20_1, {
	test_case_2_20_1_stream_0, test_case_2_20_1_stream_1, test_case_2_20_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_2, tgrp_case_2_20_2, NULL, name_case_2_20_2, NULL, desc_case_2_20_2, sref_case_2_20_2, {
	test_case_2_20_2_stream_0, test_case_2_20_2_stream_1, test_case_2_20_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_3, tgrp_case_2_20_3, NULL, name_case_2_20_3, NULL, desc_case_2_20_3, sref_case_2_20_3, {
	test_case_2_20_3_stream_0, test_case_2_20_3_stream_1, test_case_2_20_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_4, tgrp_case_2_20_4, NULL, name_case_2_20_4, NULL, desc_case_2_20_4, sref_case_2_20_4, {
	test_case_2_20_4_stream_0, test_case_2_20_4_stream_1, test_case_2_20_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_5, tgrp_case_2_20_5, NULL, name_case_2_20_5, NULL, desc_case_2_20_5, sref_case_2_20_5, {
	test_case_2_20_5_stream_0, test_case_2_20_5_stream_1, test_case_2_20_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_6, tgrp_case_2_20_6, NULL, name_case_2_20_6, NULL, desc_case_2_20_6, sref_case_2_20_6, {
	test_case_2_20_6_stream_0, test_case_2_20_6_stream_1, test_case_2_20_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_7, tgrp_case_2_20_7, NULL, name_case_2_20_7, NULL, desc_case_2_20_7, sref_case_2_20_7, {
	test_case_2_20_7_stream_0, test_case_2_20_7_stream_1, test_case_2_20_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_8, tgrp_case_2_20_8, NULL, name_case_2_20_8, NULL, desc_case_2_20_8, sref_case_2_20_8, {
	test_case_2_20_8_stream_0, test_case_2_20_8_stream_1, test_case_2_20_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_9, tgrp_case_2_20_9, NULL, name_case_2_20_9, NULL, desc_case_2_20_9, sref_case_2_20_9, {
	test_case_2_20_9_stream_0, test_case_2_20_9_stream_1, test_case_2_20_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_10, tgrp_case_2_20_10, NULL, name_case_2_20_10, NULL, desc_case_2_20_10, sref_case_2_20_10, {
	test_case_2_20_10_stream_0, test_case_2_20_10_stream_1, test_case_2_20_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_11, tgrp_case_2_20_11, NULL, name_case_2_20_11, NULL, desc_case_2_20_11, sref_case_2_20_11, {
	test_case_2_20_11_stream_0, test_case_2_20_11_stream_1, test_case_2_20_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_12, tgrp_case_2_20_12, NULL, name_case_2_20_12, NULL, desc_case_2_20_12, sref_case_2_20_12, {
	test_case_2_20_12_stream_0, test_case_2_20_12_stream_1, test_case_2_20_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_13, tgrp_case_2_20_13, NULL, name_case_2_20_13, NULL, desc_case_2_20_13, sref_case_2_20_13, {
	test_case_2_20_13_stream_0, test_case_2_20_13_stream_1, test_case_2_20_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_14, tgrp_case_2_20_14, NULL, name_case_2_20_14, NULL, desc_case_2_20_14, sref_case_2_20_14, {
	test_case_2_20_14_stream_0, test_case_2_20_14_stream_1, test_case_2_20_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_20_15, tgrp_case_2_20_15, NULL, name_case_2_20_15, NULL, desc_case_2_20_15, sref_case_2_20_15, {
	test_case_2_20_15_stream_0, test_case_2_20_15_stream_1, test_case_2_20_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_21_1, tgrp_case_2_21_1, NULL, name_case_2_21_1, NULL, desc_case_2_21_1, sref_case_2_21_1, {
	test_case_2_21_1_stream_0, test_case_2_21_1_stream_1, test_case_2_21_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_21_2, tgrp_case_2_21_2, NULL, name_case_2_21_2, NULL, desc_case_2_21_2, sref_case_2_21_2, {
	test_case_2_21_2_stream_0, test_case_2_21_2_stream_1, test_case_2_21_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_21_3, tgrp_case_2_21_3, NULL, name_case_2_21_3, NULL, desc_case_2_21_3, sref_case_2_21_3, {
	test_case_2_21_3_stream_0, test_case_2_21_3_stream_1, test_case_2_21_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_21_4, tgrp_case_2_21_4, NULL, name_case_2_21_4, NULL, desc_case_2_21_4, sref_case_2_21_4, {
	test_case_2_21_4_stream_0, test_case_2_21_4_stream_1, test_case_2_21_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_21_5, tgrp_case_2_21_5, NULL, name_case_2_21_5, NULL, desc_case_2_21_5, sref_case_2_21_5, {
	test_case_2_21_5_stream_0, test_case_2_21_5_stream_1, test_case_2_21_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_21_6, tgrp_case_2_21_6, NULL, name_case_2_21_6, NULL, desc_case_2_21_6, sref_case_2_21_6, {
	test_case_2_21_6_stream_0, test_case_2_21_6_stream_1, test_case_2_21_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_1, tgrp_case_2_22_1, NULL, name_case_2_22_1, NULL, desc_case_2_22_1, sref_case_2_22_1, {
	test_case_2_22_1_stream_0, test_case_2_22_1_stream_1, test_case_2_22_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_2, tgrp_case_2_22_2, NULL, name_case_2_22_2, NULL, desc_case_2_22_2, sref_case_2_22_2, {
	test_case_2_22_2_stream_0, test_case_2_22_2_stream_1, test_case_2_22_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_3, tgrp_case_2_22_3, NULL, name_case_2_22_3, NULL, desc_case_2_22_3, sref_case_2_22_3, {
	test_case_2_22_3_stream_0, test_case_2_22_3_stream_1, test_case_2_22_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_4, tgrp_case_2_22_4, NULL, name_case_2_22_4, NULL, desc_case_2_22_4, sref_case_2_22_4, {
	test_case_2_22_4_stream_0, test_case_2_22_4_stream_1, test_case_2_22_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_5, tgrp_case_2_22_5, NULL, name_case_2_22_5, NULL, desc_case_2_22_5, sref_case_2_22_5, {
	test_case_2_22_5_stream_0, test_case_2_22_5_stream_1, test_case_2_22_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_6, tgrp_case_2_22_6, NULL, name_case_2_22_6, NULL, desc_case_2_22_6, sref_case_2_22_6, {
	test_case_2_22_6_stream_0, test_case_2_22_6_stream_1, test_case_2_22_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_7, tgrp_case_2_22_7, NULL, name_case_2_22_7, NULL, desc_case_2_22_7, sref_case_2_22_7, {
	test_case_2_22_7_stream_0, test_case_2_22_7_stream_1, test_case_2_22_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_8, tgrp_case_2_22_8, NULL, name_case_2_22_8, NULL, desc_case_2_22_8, sref_case_2_22_8, {
	test_case_2_22_8_stream_0, test_case_2_22_8_stream_1, test_case_2_22_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_22_9, tgrp_case_2_22_9, NULL, name_case_2_22_9, NULL, desc_case_2_22_9, sref_case_2_22_9, {
	test_case_2_22_9_stream_0, test_case_2_22_9_stream_1, test_case_2_22_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_23_1, tgrp_case_2_23_1, NULL, name_case_2_23_1, NULL, desc_case_2_23_1, sref_case_2_23_1, {
	test_case_2_23_1_stream_0, test_case_2_23_1_stream_1, test_case_2_23_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_23_2, tgrp_case_2_23_2, NULL, name_case_2_23_2, NULL, desc_case_2_23_2, sref_case_2_23_2, {
	test_case_2_23_2_stream_0, test_case_2_23_2_stream_1, test_case_2_23_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_23_3, tgrp_case_2_23_3, NULL, name_case_2_23_3, NULL, desc_case_2_23_3, sref_case_2_23_3, {
	test_case_2_23_3_stream_0, test_case_2_23_3_stream_1, test_case_2_23_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_23_4, tgrp_case_2_23_4, NULL, name_case_2_23_4, NULL, desc_case_2_23_4, sref_case_2_23_4, {
	test_case_2_23_4_stream_0, test_case_2_23_4_stream_1, test_case_2_23_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_23_5, tgrp_case_2_23_5, NULL, name_case_2_23_5, NULL, desc_case_2_23_5, sref_case_2_23_5, {
	test_case_2_23_5_stream_0, test_case_2_23_5_stream_1, test_case_2_23_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_23_6, tgrp_case_2_23_6, NULL, name_case_2_23_6, NULL, desc_case_2_23_6, sref_case_2_23_6, {
	test_case_2_23_6_stream_0, test_case_2_23_6_stream_1, test_case_2_23_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_1, tgrp_case_2_24_1, NULL, name_case_2_24_1, NULL, desc_case_2_24_1, sref_case_2_24_1, {
	test_case_2_24_1_stream_0, test_case_2_24_1_stream_1, test_case_2_24_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_2, tgrp_case_2_24_2, NULL, name_case_2_24_2, NULL, desc_case_2_24_2, sref_case_2_24_2, {
	test_case_2_24_2_stream_0, test_case_2_24_2_stream_1, test_case_2_24_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_3, tgrp_case_2_24_3, NULL, name_case_2_24_3, NULL, desc_case_2_24_3, sref_case_2_24_3, {
	test_case_2_24_3_stream_0, test_case_2_24_3_stream_1, test_case_2_24_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_4, tgrp_case_2_24_4, NULL, name_case_2_24_4, NULL, desc_case_2_24_4, sref_case_2_24_4, {
	test_case_2_24_4_stream_0, test_case_2_24_4_stream_1, test_case_2_24_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_5, tgrp_case_2_24_5, NULL, name_case_2_24_5, NULL, desc_case_2_24_5, sref_case_2_24_5, {
	test_case_2_24_5_stream_0, test_case_2_24_5_stream_1, test_case_2_24_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_6, tgrp_case_2_24_6, NULL, name_case_2_24_6, NULL, desc_case_2_24_6, sref_case_2_24_6, {
	test_case_2_24_6_stream_0, test_case_2_24_6_stream_1, test_case_2_24_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_7, tgrp_case_2_24_7, NULL, name_case_2_24_7, NULL, desc_case_2_24_7, sref_case_2_24_7, {
	test_case_2_24_7_stream_0, test_case_2_24_7_stream_1, test_case_2_24_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_8, tgrp_case_2_24_8, NULL, name_case_2_24_8, NULL, desc_case_2_24_8, sref_case_2_24_8, {
	test_case_2_24_8_stream_0, test_case_2_24_8_stream_1, test_case_2_24_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_9, tgrp_case_2_24_9, NULL, name_case_2_24_9, NULL, desc_case_2_24_9, sref_case_2_24_9, {
	test_case_2_24_9_stream_0, test_case_2_24_9_stream_1, test_case_2_24_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_10, tgrp_case_2_24_10, NULL, name_case_2_24_10, NULL, desc_case_2_24_10, sref_case_2_24_10, {
	test_case_2_24_10_stream_0, test_case_2_24_10_stream_1, test_case_2_24_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_11, tgrp_case_2_24_11, NULL, name_case_2_24_11, NULL, desc_case_2_24_11, sref_case_2_24_11, {
	test_case_2_24_11_stream_0, test_case_2_24_11_stream_1, test_case_2_24_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_12, tgrp_case_2_24_12, NULL, name_case_2_24_12, NULL, desc_case_2_24_12, sref_case_2_24_12, {
	test_case_2_24_12_stream_0, test_case_2_24_12_stream_1, test_case_2_24_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_13, tgrp_case_2_24_13, NULL, name_case_2_24_13, NULL, desc_case_2_24_13, sref_case_2_24_13, {
	test_case_2_24_13_stream_0, test_case_2_24_13_stream_1, test_case_2_24_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_14, tgrp_case_2_24_14, NULL, name_case_2_24_14, NULL, desc_case_2_24_14, sref_case_2_24_14, {
	test_case_2_24_14_stream_0, test_case_2_24_14_stream_1, test_case_2_24_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_24_15, tgrp_case_2_24_15, NULL, name_case_2_24_15, NULL, desc_case_2_24_15, sref_case_2_24_15, {
	test_case_2_24_15_stream_0, test_case_2_24_15_stream_1, test_case_2_24_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_1, tgrp_case_2_25_1, NULL, name_case_2_25_1, NULL, desc_case_2_25_1, sref_case_2_25_1, {
	test_case_2_25_1_stream_0, test_case_2_25_1_stream_1, test_case_2_25_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_2, tgrp_case_2_25_2, NULL, name_case_2_25_2, NULL, desc_case_2_25_2, sref_case_2_25_2, {
	test_case_2_25_2_stream_0, test_case_2_25_2_stream_1, test_case_2_25_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_3, tgrp_case_2_25_3, NULL, name_case_2_25_3, NULL, desc_case_2_25_3, sref_case_2_25_3, {
	test_case_2_25_3_stream_0, test_case_2_25_3_stream_1, test_case_2_25_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_4, tgrp_case_2_25_4, NULL, name_case_2_25_4, NULL, desc_case_2_25_4, sref_case_2_25_4, {
	test_case_2_25_4_stream_0, test_case_2_25_4_stream_1, test_case_2_25_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_5, tgrp_case_2_25_5, NULL, name_case_2_25_5, NULL, desc_case_2_25_5, sref_case_2_25_5, {
	test_case_2_25_5_stream_0, test_case_2_25_5_stream_1, test_case_2_25_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_6, tgrp_case_2_25_6, NULL, name_case_2_25_6, NULL, desc_case_2_25_6, sref_case_2_25_6, {
	test_case_2_25_6_stream_0, test_case_2_25_6_stream_1, test_case_2_25_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_7, tgrp_case_2_25_7, NULL, name_case_2_25_7, NULL, desc_case_2_25_7, sref_case_2_25_7, {
	test_case_2_25_7_stream_0, test_case_2_25_7_stream_1, test_case_2_25_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_25_8, tgrp_case_2_25_8, NULL, name_case_2_25_8, NULL, desc_case_2_25_8, sref_case_2_25_8, {
	test_case_2_25_8_stream_0, test_case_2_25_8_stream_1, test_case_2_25_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_1, tgrp_case_2_26_1, NULL, name_case_2_26_1, NULL, desc_case_2_26_1, sref_case_2_26_1, {
	test_case_2_26_1_stream_0, test_case_2_26_1_stream_1, test_case_2_26_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_2, tgrp_case_2_26_2, NULL, name_case_2_26_2, NULL, desc_case_2_26_2, sref_case_2_26_2, {
	test_case_2_26_2_stream_0, test_case_2_26_2_stream_1, test_case_2_26_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_3, tgrp_case_2_26_3, NULL, name_case_2_26_3, NULL, desc_case_2_26_3, sref_case_2_26_3, {
	test_case_2_26_3_stream_0, test_case_2_26_3_stream_1, test_case_2_26_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_4, tgrp_case_2_26_4, NULL, name_case_2_26_4, NULL, desc_case_2_26_4, sref_case_2_26_4, {
	test_case_2_26_4_stream_0, test_case_2_26_4_stream_1, test_case_2_26_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_5, tgrp_case_2_26_5, NULL, name_case_2_26_5, NULL, desc_case_2_26_5, sref_case_2_26_5, {
	test_case_2_26_5_stream_0, test_case_2_26_5_stream_1, test_case_2_26_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_6, tgrp_case_2_26_6, NULL, name_case_2_26_6, NULL, desc_case_2_26_6, sref_case_2_26_6, {
	test_case_2_26_6_stream_0, test_case_2_26_6_stream_1, test_case_2_26_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_7, tgrp_case_2_26_7, NULL, name_case_2_26_7, NULL, desc_case_2_26_7, sref_case_2_26_7, {
	test_case_2_26_7_stream_0, test_case_2_26_7_stream_1, test_case_2_26_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_8, tgrp_case_2_26_8, NULL, name_case_2_26_8, NULL, desc_case_2_26_8, sref_case_2_26_8, {
	test_case_2_26_8_stream_0, test_case_2_26_8_stream_1, test_case_2_26_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_26_9, tgrp_case_2_26_9, NULL, name_case_2_26_9, NULL, desc_case_2_26_9, sref_case_2_26_9, {
	test_case_2_26_9_stream_0, test_case_2_26_9_stream_1, test_case_2_26_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_27_1, tgrp_case_2_27_1, NULL, name_case_2_27_1, NULL, desc_case_2_27_1, sref_case_2_27_1, {
	test_case_2_27_1_stream_0, test_case_2_27_1_stream_1, test_case_2_27_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_27_2, tgrp_case_2_27_2, NULL, name_case_2_27_2, NULL, desc_case_2_27_2, sref_case_2_27_2, {
	test_case_2_27_2_stream_0, test_case_2_27_2_stream_1, test_case_2_27_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_27_3, tgrp_case_2_27_3, NULL, name_case_2_27_3, NULL, desc_case_2_27_3, sref_case_2_27_3, {
	test_case_2_27_3_stream_0, test_case_2_27_3_stream_1, test_case_2_27_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_27_4, tgrp_case_2_27_4, NULL, name_case_2_27_4, NULL, desc_case_2_27_4, sref_case_2_27_4, {
	test_case_2_27_4_stream_0, test_case_2_27_4_stream_1, test_case_2_27_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_27_5, tgrp_case_2_27_5, NULL, name_case_2_27_5, NULL, desc_case_2_27_5, sref_case_2_27_5, {
	test_case_2_27_5_stream_0, test_case_2_27_5_stream_1, test_case_2_27_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_27_6, tgrp_case_2_27_6, NULL, name_case_2_27_6, NULL, desc_case_2_27_6, sref_case_2_27_6, {
	test_case_2_27_6_stream_0, test_case_2_27_6_stream_1, test_case_2_27_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_1, tgrp_case_2_28_1, NULL, name_case_2_28_1, NULL, desc_case_2_28_1, sref_case_2_28_1, {
	test_case_2_28_1_stream_0, test_case_2_28_1_stream_1, test_case_2_28_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_2, tgrp_case_2_28_2, NULL, name_case_2_28_2, NULL, desc_case_2_28_2, sref_case_2_28_2, {
	test_case_2_28_2_stream_0, test_case_2_28_2_stream_1, test_case_2_28_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_3, tgrp_case_2_28_3, NULL, name_case_2_28_3, NULL, desc_case_2_28_3, sref_case_2_28_3, {
	test_case_2_28_3_stream_0, test_case_2_28_3_stream_1, test_case_2_28_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_4, tgrp_case_2_28_4, NULL, name_case_2_28_4, NULL, desc_case_2_28_4, sref_case_2_28_4, {
	test_case_2_28_4_stream_0, test_case_2_28_4_stream_1, test_case_2_28_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_5, tgrp_case_2_28_5, NULL, name_case_2_28_5, NULL, desc_case_2_28_5, sref_case_2_28_5, {
	test_case_2_28_5_stream_0, test_case_2_28_5_stream_1, test_case_2_28_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_6, tgrp_case_2_28_6, NULL, name_case_2_28_6, NULL, desc_case_2_28_6, sref_case_2_28_6, {
	test_case_2_28_6_stream_0, test_case_2_28_6_stream_1, test_case_2_28_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_7, tgrp_case_2_28_7, NULL, name_case_2_28_7, NULL, desc_case_2_28_7, sref_case_2_28_7, {
	test_case_2_28_7_stream_0, test_case_2_28_7_stream_1, test_case_2_28_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_8, tgrp_case_2_28_8, NULL, name_case_2_28_8, NULL, desc_case_2_28_8, sref_case_2_28_8, {
	test_case_2_28_8_stream_0, test_case_2_28_8_stream_1, test_case_2_28_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_9, tgrp_case_2_28_9, NULL, name_case_2_28_9, NULL, desc_case_2_28_9, sref_case_2_28_9, {
	test_case_2_28_9_stream_0, test_case_2_28_9_stream_1, test_case_2_28_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_10, tgrp_case_2_28_10, NULL, name_case_2_28_10, NULL, desc_case_2_28_10, sref_case_2_28_10, {
	test_case_2_28_10_stream_0, test_case_2_28_10_stream_1, test_case_2_28_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_28_11, tgrp_case_2_28_11, NULL, name_case_2_28_11, NULL, desc_case_2_28_11, sref_case_2_28_11, {
	test_case_2_28_11_stream_0, test_case_2_28_11_stream_1, test_case_2_28_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_29_1, tgrp_case_2_29_1, NULL, name_case_2_29_1, NULL, desc_case_2_29_1, sref_case_2_29_1, {
	test_case_2_29_1_stream_0, test_case_2_29_1_stream_1, test_case_2_29_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_29_2, tgrp_case_2_29_2, NULL, name_case_2_29_2, NULL, desc_case_2_29_2, sref_case_2_29_2, {
	test_case_2_29_2_stream_0, test_case_2_29_2_stream_1, test_case_2_29_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_29_3, tgrp_case_2_29_3, NULL, name_case_2_29_3, NULL, desc_case_2_29_3, sref_case_2_29_3, {
	test_case_2_29_3_stream_0, test_case_2_29_3_stream_1, test_case_2_29_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_29_4, tgrp_case_2_29_4, NULL, name_case_2_29_4, NULL, desc_case_2_29_4, sref_case_2_29_4, {
	test_case_2_29_4_stream_0, test_case_2_29_4_stream_1, test_case_2_29_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_29_5, tgrp_case_2_29_5, NULL, name_case_2_29_5, NULL, desc_case_2_29_5, sref_case_2_29_5, {
	test_case_2_29_5_stream_0, test_case_2_29_5_stream_1, test_case_2_29_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_29_6, tgrp_case_2_29_6, NULL, name_case_2_29_6, NULL, desc_case_2_29_6, sref_case_2_29_6, {
	test_case_2_29_6_stream_0, test_case_2_29_6_stream_1, test_case_2_29_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_29_7, tgrp_case_2_29_7, NULL, name_case_2_29_7, NULL, desc_case_2_29_7, sref_case_2_29_7, {
	test_case_2_29_7_stream_0, test_case_2_29_7_stream_1, test_case_2_29_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_1, tgrp_case_2_30_1, NULL, name_case_2_30_1, NULL, desc_case_2_30_1, sref_case_2_30_1, {
	test_case_2_30_1_stream_0, test_case_2_30_1_stream_1, test_case_2_30_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_2, tgrp_case_2_30_2, NULL, name_case_2_30_2, NULL, desc_case_2_30_2, sref_case_2_30_2, {
	test_case_2_30_2_stream_0, test_case_2_30_2_stream_1, test_case_2_30_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_3, tgrp_case_2_30_3, NULL, name_case_2_30_3, NULL, desc_case_2_30_3, sref_case_2_30_3, {
	test_case_2_30_3_stream_0, test_case_2_30_3_stream_1, test_case_2_30_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_4, tgrp_case_2_30_4, NULL, name_case_2_30_4, NULL, desc_case_2_30_4, sref_case_2_30_4, {
	test_case_2_30_4_stream_0, test_case_2_30_4_stream_1, test_case_2_30_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_5, tgrp_case_2_30_5, NULL, name_case_2_30_5, NULL, desc_case_2_30_5, sref_case_2_30_5, {
	test_case_2_30_5_stream_0, test_case_2_30_5_stream_1, test_case_2_30_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_6, tgrp_case_2_30_6, NULL, name_case_2_30_6, NULL, desc_case_2_30_6, sref_case_2_30_6, {
	test_case_2_30_6_stream_0, test_case_2_30_6_stream_1, test_case_2_30_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_7, tgrp_case_2_30_7, NULL, name_case_2_30_7, NULL, desc_case_2_30_7, sref_case_2_30_7, {
	test_case_2_30_7_stream_0, test_case_2_30_7_stream_1, test_case_2_30_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_8, tgrp_case_2_30_8, NULL, name_case_2_30_8, NULL, desc_case_2_30_8, sref_case_2_30_8, {
	test_case_2_30_8_stream_0, test_case_2_30_8_stream_1, test_case_2_30_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_9, tgrp_case_2_30_9, NULL, name_case_2_30_9, NULL, desc_case_2_30_9, sref_case_2_30_9, {
	test_case_2_30_9_stream_0, test_case_2_30_9_stream_1, test_case_2_30_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_10, tgrp_case_2_30_10, NULL, name_case_2_30_10, NULL, desc_case_2_30_10, sref_case_2_30_10, {
	test_case_2_30_10_stream_0, test_case_2_30_10_stream_1, test_case_2_30_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_11, tgrp_case_2_30_11, NULL, name_case_2_30_11, NULL, desc_case_2_30_11, sref_case_2_30_11, {
	test_case_2_30_11_stream_0, test_case_2_30_11_stream_1, test_case_2_30_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_12, tgrp_case_2_30_12, NULL, name_case_2_30_12, NULL, desc_case_2_30_12, sref_case_2_30_12, {
	test_case_2_30_12_stream_0, test_case_2_30_12_stream_1, test_case_2_30_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_30_13, tgrp_case_2_30_13, NULL, name_case_2_30_13, NULL, desc_case_2_30_13, sref_case_2_30_13, {
	test_case_2_30_13_stream_0, test_case_2_30_13_stream_1, test_case_2_30_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_1, tgrp_case_2_31_1, NULL, name_case_2_31_1, NULL, desc_case_2_31_1, sref_case_2_31_1, {
	test_case_2_31_1_stream_0, test_case_2_31_1_stream_1, test_case_2_31_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_2, tgrp_case_2_31_2, NULL, name_case_2_31_2, NULL, desc_case_2_31_2, sref_case_2_31_2, {
	test_case_2_31_2_stream_0, test_case_2_31_2_stream_1, test_case_2_31_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_3, tgrp_case_2_31_3, NULL, name_case_2_31_3, NULL, desc_case_2_31_3, sref_case_2_31_3, {
	test_case_2_31_3_stream_0, test_case_2_31_3_stream_1, test_case_2_31_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_4, tgrp_case_2_31_4, NULL, name_case_2_31_4, NULL, desc_case_2_31_4, sref_case_2_31_4, {
	test_case_2_31_4_stream_0, test_case_2_31_4_stream_1, test_case_2_31_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_5, tgrp_case_2_31_5, NULL, name_case_2_31_5, NULL, desc_case_2_31_5, sref_case_2_31_5, {
	test_case_2_31_5_stream_0, test_case_2_31_5_stream_1, test_case_2_31_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_6, tgrp_case_2_31_6, NULL, name_case_2_31_6, NULL, desc_case_2_31_6, sref_case_2_31_6, {
	test_case_2_31_6_stream_0, test_case_2_31_6_stream_1, test_case_2_31_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_7, tgrp_case_2_31_7, NULL, name_case_2_31_7, NULL, desc_case_2_31_7, sref_case_2_31_7, {
	test_case_2_31_7_stream_0, test_case_2_31_7_stream_1, test_case_2_31_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_8, tgrp_case_2_31_8, NULL, name_case_2_31_8, NULL, desc_case_2_31_8, sref_case_2_31_8, {
	test_case_2_31_8_stream_0, test_case_2_31_8_stream_1, test_case_2_31_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_9, tgrp_case_2_31_9, NULL, name_case_2_31_9, NULL, desc_case_2_31_9, sref_case_2_31_9, {
	test_case_2_31_9_stream_0, test_case_2_31_9_stream_1, test_case_2_31_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_31_10, tgrp_case_2_31_10, NULL, name_case_2_31_10, NULL, desc_case_2_31_10, sref_case_2_31_10, {
	test_case_2_31_10_stream_0, test_case_2_31_10_stream_1, test_case_2_31_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_32_1, tgrp_case_2_32_1, NULL, name_case_2_32_1, NULL, desc_case_2_32_1, sref_case_2_32_1, {
	test_case_2_32_1_stream_0, test_case_2_32_1_stream_1, test_case_2_32_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_32_2, tgrp_case_2_32_2, NULL, name_case_2_32_2, NULL, desc_case_2_32_2, sref_case_2_32_2, {
	test_case_2_32_2_stream_0, test_case_2_32_2_stream_1, test_case_2_32_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_32_3, tgrp_case_2_32_3, NULL, name_case_2_32_3, NULL, desc_case_2_32_3, sref_case_2_32_3, {
	test_case_2_32_3_stream_0, test_case_2_32_3_stream_1, test_case_2_32_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_32_4, tgrp_case_2_32_4, NULL, name_case_2_32_4, NULL, desc_case_2_32_4, sref_case_2_32_4, {
	test_case_2_32_4_stream_0, test_case_2_32_4_stream_1, test_case_2_32_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_32_5, tgrp_case_2_32_5, NULL, name_case_2_32_5, NULL, desc_case_2_32_5, sref_case_2_32_5, {
	test_case_2_32_5_stream_0, test_case_2_32_5_stream_1, test_case_2_32_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_32_6, tgrp_case_2_32_6, NULL, name_case_2_32_6, NULL, desc_case_2_32_6, sref_case_2_32_6, {
	test_case_2_32_6_stream_0, test_case_2_32_6_stream_1, test_case_2_32_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_32_7, tgrp_case_2_32_7, NULL, name_case_2_32_7, NULL, desc_case_2_32_7, sref_case_2_32_7, {
	test_case_2_32_7_stream_0, test_case_2_32_7_stream_1, test_case_2_32_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_33, tgrp_case_2_33, NULL, name_case_2_33, NULL, desc_case_2_33, sref_case_2_33, {
	test_case_2_33_stream_0, test_case_2_33_stream_1, test_case_2_33_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_34, tgrp_case_2_34, NULL, name_case_2_34, NULL, desc_case_2_34, sref_case_2_34, {
	test_case_2_34_stream_0, test_case_2_34_stream_1, test_case_2_34_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_35, tgrp_case_2_35, NULL, name_case_2_35, NULL, desc_case_2_35, sref_case_2_35, {
	test_case_2_35_stream_0, test_case_2_35_stream_1, test_case_2_35_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_36, tgrp_case_2_36, NULL, name_case_2_36, NULL, desc_case_2_36, sref_case_2_36, {
	test_case_2_36_stream_0, test_case_2_36_stream_1, test_case_2_36_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_37, tgrp_case_2_37, NULL, name_case_2_37, NULL, desc_case_2_37, sref_case_2_37, {
	test_case_2_37_stream_0, test_case_2_37_stream_1, test_case_2_37_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_38, tgrp_case_2_38, NULL, name_case_2_38, NULL, desc_case_2_38, sref_case_2_38, {
	test_case_2_38_stream_0, test_case_2_38_stream_1, test_case_2_38_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_39, tgrp_case_2_39, NULL, name_case_2_39, NULL, desc_case_2_39, sref_case_2_39, {
	test_case_2_39_stream_0, test_case_2_39_stream_1, test_case_2_39_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_1, tgrp_case_2_40_1, NULL, name_case_2_40_1, NULL, desc_case_2_40_1, sref_case_2_40_1, {
	test_case_2_40_1_stream_0, test_case_2_40_1_stream_1, test_case_2_40_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_2, tgrp_case_2_40_2, NULL, name_case_2_40_2, NULL, desc_case_2_40_2, sref_case_2_40_2, {
	test_case_2_40_2_stream_0, test_case_2_40_2_stream_1, test_case_2_40_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_3, tgrp_case_2_40_3, NULL, name_case_2_40_3, NULL, desc_case_2_40_3, sref_case_2_40_3, {
	test_case_2_40_3_stream_0, test_case_2_40_3_stream_1, test_case_2_40_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_4, tgrp_case_2_40_4, NULL, name_case_2_40_4, NULL, desc_case_2_40_4, sref_case_2_40_4, {
	test_case_2_40_4_stream_0, test_case_2_40_4_stream_1, test_case_2_40_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_5, tgrp_case_2_40_5, NULL, name_case_2_40_5, NULL, desc_case_2_40_5, sref_case_2_40_5, {
	test_case_2_40_5_stream_0, test_case_2_40_5_stream_1, test_case_2_40_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_6, tgrp_case_2_40_6, NULL, name_case_2_40_6, NULL, desc_case_2_40_6, sref_case_2_40_6, {
	test_case_2_40_6_stream_0, test_case_2_40_6_stream_1, test_case_2_40_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_7, tgrp_case_2_40_7, NULL, name_case_2_40_7, NULL, desc_case_2_40_7, sref_case_2_40_7, {
	test_case_2_40_7_stream_0, test_case_2_40_7_stream_1, test_case_2_40_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_8, tgrp_case_2_40_8, NULL, name_case_2_40_8, NULL, desc_case_2_40_8, sref_case_2_40_8, {
	test_case_2_40_8_stream_0, test_case_2_40_8_stream_1, test_case_2_40_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_9, tgrp_case_2_40_9, NULL, name_case_2_40_9, NULL, desc_case_2_40_9, sref_case_2_40_9, {
	test_case_2_40_9_stream_0, test_case_2_40_9_stream_1, test_case_2_40_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_40_10, tgrp_case_2_40_10, NULL, name_case_2_40_10, NULL, desc_case_2_40_10, sref_case_2_40_10, {
	test_case_2_40_10_stream_0, test_case_2_40_10_stream_1, test_case_2_40_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_41_1, tgrp_case_2_41_1, NULL, name_case_2_41_1, NULL, desc_case_2_41_1, sref_case_2_41_1, {
	test_case_2_41_1_stream_0, test_case_2_41_1_stream_1, test_case_2_41_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_41_2, tgrp_case_2_41_2, NULL, name_case_2_41_2, NULL, desc_case_2_41_2, sref_case_2_41_2, {
	test_case_2_41_2_stream_0, test_case_2_41_2_stream_1, test_case_2_41_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_41_3, tgrp_case_2_41_3, NULL, name_case_2_41_3, NULL, desc_case_2_41_3, sref_case_2_41_3, {
	test_case_2_41_3_stream_0, test_case_2_41_3_stream_1, test_case_2_41_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_41_4, tgrp_case_2_41_4, NULL, name_case_2_41_4, NULL, desc_case_2_41_4, sref_case_2_41_4, {
	test_case_2_41_4_stream_0, test_case_2_41_4_stream_1, test_case_2_41_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_42, tgrp_case_2_42, NULL, name_case_2_42, NULL, desc_case_2_42, sref_case_2_42, {
	test_case_2_42_stream_0, test_case_2_42_stream_1, test_case_2_42_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_43, tgrp_case_2_43, NULL, name_case_2_43, NULL, desc_case_2_43, sref_case_2_43, {
	test_case_2_43_stream_0, test_case_2_43_stream_1, test_case_2_43_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_44, tgrp_case_2_44, NULL, name_case_2_44, NULL, desc_case_2_44, sref_case_2_44, {
	test_case_2_44_stream_0, test_case_2_44_stream_1, test_case_2_44_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_45, tgrp_case_2_45, NULL, name_case_2_45, NULL, desc_case_2_45, sref_case_2_45, {
	test_case_2_45_stream_0, test_case_2_45_stream_1, test_case_2_45_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_46, tgrp_case_2_46, NULL, name_case_2_46, NULL, desc_case_2_46, sref_case_2_46, {
	test_case_2_46_stream_0, test_case_2_46_stream_1, test_case_2_46_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_47, tgrp_case_2_47, NULL, name_case_2_47, NULL, desc_case_2_47, sref_case_2_47, {
	test_case_2_47_stream_0, test_case_2_47_stream_1, test_case_2_47_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_48_1, tgrp_case_2_48_1, NULL, name_case_2_48_1, NULL, desc_case_2_48_1, sref_case_2_48_1, {
	test_case_2_48_1_stream_0, test_case_2_48_1_stream_1, test_case_2_48_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_48_2, tgrp_case_2_48_2, NULL, name_case_2_48_2, NULL, desc_case_2_48_2, sref_case_2_48_2, {
	test_case_2_48_2_stream_0, test_case_2_48_2_stream_1, test_case_2_48_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_48_3, tgrp_case_2_48_3, NULL, name_case_2_48_3, NULL, desc_case_2_48_3, sref_case_2_48_3, {
	test_case_2_48_3_stream_0, test_case_2_48_3_stream_1, test_case_2_48_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_48_4, tgrp_case_2_48_4, NULL, name_case_2_48_4, NULL, desc_case_2_48_4, sref_case_2_48_4, {
	test_case_2_48_4_stream_0, test_case_2_48_4_stream_1, test_case_2_48_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_48_5, tgrp_case_2_48_5, NULL, name_case_2_48_5, NULL, desc_case_2_48_5, sref_case_2_48_5, {
	test_case_2_48_5_stream_0, test_case_2_48_5_stream_1, test_case_2_48_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_48_6, tgrp_case_2_48_6, NULL, name_case_2_48_6, NULL, desc_case_2_48_6, sref_case_2_48_6, {
	test_case_2_48_6_stream_0, test_case_2_48_6_stream_1, test_case_2_48_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_1, tgrp_case_3_1_1, NULL, name_case_3_1_1, NULL, desc_case_3_1_1, sref_case_3_1_1, {
	test_case_3_1_1_stream_0, test_case_3_1_1_stream_1, test_case_3_1_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_2, tgrp_case_3_1_2, NULL, name_case_3_1_2, NULL, desc_case_3_1_2, sref_case_3_1_2, {
	test_case_3_1_2_stream_0, test_case_3_1_2_stream_1, test_case_3_1_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_3, tgrp_case_3_1_3, NULL, name_case_3_1_3, NULL, desc_case_3_1_3, sref_case_3_1_3, {
	test_case_3_1_3_stream_0, test_case_3_1_3_stream_1, test_case_3_1_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_4, tgrp_case_3_1_4, NULL, name_case_3_1_4, NULL, desc_case_3_1_4, sref_case_3_1_4, {
	test_case_3_1_4_stream_0, test_case_3_1_4_stream_1, test_case_3_1_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_5, tgrp_case_3_1_5, NULL, name_case_3_1_5, NULL, desc_case_3_1_5, sref_case_3_1_5, {
	test_case_3_1_5_stream_0, test_case_3_1_5_stream_1, test_case_3_1_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_6, tgrp_case_3_1_6, NULL, name_case_3_1_6, NULL, desc_case_3_1_6, sref_case_3_1_6, {
	test_case_3_1_6_stream_0, test_case_3_1_6_stream_1, test_case_3_1_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_7, tgrp_case_3_1_7, NULL, name_case_3_1_7, NULL, desc_case_3_1_7, sref_case_3_1_7, {
	test_case_3_1_7_stream_0, test_case_3_1_7_stream_1, test_case_3_1_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_8, tgrp_case_3_1_8, NULL, name_case_3_1_8, NULL, desc_case_3_1_8, sref_case_3_1_8, {
	test_case_3_1_8_stream_0, test_case_3_1_8_stream_1, test_case_3_1_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_9, tgrp_case_3_1_9, NULL, name_case_3_1_9, NULL, desc_case_3_1_9, sref_case_3_1_9, {
	test_case_3_1_9_stream_0, test_case_3_1_9_stream_1, test_case_3_1_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_10, tgrp_case_3_1_10, NULL, name_case_3_1_10, NULL, desc_case_3_1_10, sref_case_3_1_10, {
	test_case_3_1_10_stream_0, test_case_3_1_10_stream_1, test_case_3_1_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_11_1, tgrp_case_3_1_11_1, NULL, name_case_3_1_11_1, NULL, desc_case_3_1_11_1, sref_case_3_1_11_1, {
	test_case_3_1_11_1_stream_0, test_case_3_1_11_1_stream_1, test_case_3_1_11_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_11_2, tgrp_case_3_1_11_2, NULL, name_case_3_1_11_2, NULL, desc_case_3_1_11_2, sref_case_3_1_11_2, {
	test_case_3_1_11_2_stream_0, test_case_3_1_11_2_stream_1, test_case_3_1_11_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_11_3, tgrp_case_3_1_11_3, NULL, name_case_3_1_11_3, NULL, desc_case_3_1_11_3, sref_case_3_1_11_3, {
	test_case_3_1_11_3_stream_0, test_case_3_1_11_3_stream_1, test_case_3_1_11_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_12_1, tgrp_case_3_1_12_1, NULL, name_case_3_1_12_1, NULL, desc_case_3_1_12_1, sref_case_3_1_12_1, {
	test_case_3_1_12_1_stream_0, test_case_3_1_12_1_stream_1, test_case_3_1_12_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_12_2, tgrp_case_3_1_12_2, NULL, name_case_3_1_12_2, NULL, desc_case_3_1_12_2, sref_case_3_1_12_2, {
	test_case_3_1_12_2_stream_0, test_case_3_1_12_2_stream_1, test_case_3_1_12_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_12_3, tgrp_case_3_1_12_3, NULL, name_case_3_1_12_3, NULL, desc_case_3_1_12_3, sref_case_3_1_12_3, {
	test_case_3_1_12_3_stream_0, test_case_3_1_12_3_stream_1, test_case_3_1_12_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_13_1, tgrp_case_3_1_13_1, NULL, name_case_3_1_13_1, NULL, desc_case_3_1_13_1, sref_case_3_1_13_1, {
	test_case_3_1_13_1_stream_0, test_case_3_1_13_1_stream_1, test_case_3_1_13_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_13_2, tgrp_case_3_1_13_2, NULL, name_case_3_1_13_2, NULL, desc_case_3_1_13_2, sref_case_3_1_13_2, {
	test_case_3_1_13_2_stream_0, test_case_3_1_13_2_stream_1, test_case_3_1_13_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_13_3, tgrp_case_3_1_13_3, NULL, name_case_3_1_13_3, NULL, desc_case_3_1_13_3, sref_case_3_1_13_3, {
	test_case_3_1_13_3_stream_0, test_case_3_1_13_3_stream_1, test_case_3_1_13_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_14_1, tgrp_case_3_1_14_1, NULL, name_case_3_1_14_1, NULL, desc_case_3_1_14_1, sref_case_3_1_14_1, {
	test_case_3_1_14_1_stream_0, test_case_3_1_14_1_stream_1, test_case_3_1_14_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_14_2, tgrp_case_3_1_14_2, NULL, name_case_3_1_14_2, NULL, desc_case_3_1_14_2, sref_case_3_1_14_2, {
	test_case_3_1_14_2_stream_0, test_case_3_1_14_2_stream_1, test_case_3_1_14_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_15, tgrp_case_3_1_15, NULL, name_case_3_1_15, NULL, desc_case_3_1_15, sref_case_3_1_15, {
	test_case_3_1_15_stream_0, test_case_3_1_15_stream_1, test_case_3_1_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1_16, tgrp_case_3_1_16, NULL, name_case_3_1_16, NULL, desc_case_3_1_16, sref_case_3_1_16, {
	test_case_3_1_16_stream_0, test_case_3_1_16_stream_1, test_case_3_1_16_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_1, tgrp_case_3_2_1, NULL, name_case_3_2_1, NULL, desc_case_3_2_1, sref_case_3_2_1, {
	test_case_3_2_1_stream_0, test_case_3_2_1_stream_1, test_case_3_2_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_2, tgrp_case_3_2_2, NULL, name_case_3_2_2, NULL, desc_case_3_2_2, sref_case_3_2_2, {
	test_case_3_2_2_stream_0, test_case_3_2_2_stream_1, test_case_3_2_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_3, tgrp_case_3_2_3, NULL, name_case_3_2_3, NULL, desc_case_3_2_3, sref_case_3_2_3, {
	test_case_3_2_3_stream_0, test_case_3_2_3_stream_1, test_case_3_2_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_4, tgrp_case_3_2_4, NULL, name_case_3_2_4, NULL, desc_case_3_2_4, sref_case_3_2_4, {
	test_case_3_2_4_stream_0, test_case_3_2_4_stream_1, test_case_3_2_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_5, tgrp_case_3_2_5, NULL, name_case_3_2_5, NULL, desc_case_3_2_5, sref_case_3_2_5, {
	test_case_3_2_5_stream_0, test_case_3_2_5_stream_1, test_case_3_2_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_6, tgrp_case_3_2_6, NULL, name_case_3_2_6, NULL, desc_case_3_2_6, sref_case_3_2_6, {
	test_case_3_2_6_stream_0, test_case_3_2_6_stream_1, test_case_3_2_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_7, tgrp_case_3_2_7, NULL, name_case_3_2_7, NULL, desc_case_3_2_7, sref_case_3_2_7, {
	test_case_3_2_7_stream_0, test_case_3_2_7_stream_1, test_case_3_2_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_8, tgrp_case_3_2_8, NULL, name_case_3_2_8, NULL, desc_case_3_2_8, sref_case_3_2_8, {
	test_case_3_2_8_stream_0, test_case_3_2_8_stream_1, test_case_3_2_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_9, tgrp_case_3_2_9, NULL, name_case_3_2_9, NULL, desc_case_3_2_9, sref_case_3_2_9, {
	test_case_3_2_9_stream_0, test_case_3_2_9_stream_1, test_case_3_2_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_10, tgrp_case_3_2_10, NULL, name_case_3_2_10, NULL, desc_case_3_2_10, sref_case_3_2_10, {
	test_case_3_2_10_stream_0, test_case_3_2_10_stream_1, test_case_3_2_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2_11, tgrp_case_3_2_11, NULL, name_case_3_2_11, NULL, desc_case_3_2_11, sref_case_3_2_11, {
	test_case_3_2_11_stream_0, test_case_3_2_11_stream_1, test_case_3_2_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_1, tgrp_case_3_3_1, NULL, name_case_3_3_1, NULL, desc_case_3_3_1, sref_case_3_3_1, {
	test_case_3_3_1_stream_0, test_case_3_3_1_stream_1, test_case_3_3_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_2, tgrp_case_3_3_2, NULL, name_case_3_3_2, NULL, desc_case_3_3_2, sref_case_3_3_2, {
	test_case_3_3_2_stream_0, test_case_3_3_2_stream_1, test_case_3_3_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_3, tgrp_case_3_3_3, NULL, name_case_3_3_3, NULL, desc_case_3_3_3, sref_case_3_3_3, {
	test_case_3_3_3_stream_0, test_case_3_3_3_stream_1, test_case_3_3_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_4, tgrp_case_3_3_4, NULL, name_case_3_3_4, NULL, desc_case_3_3_4, sref_case_3_3_4, {
	test_case_3_3_4_stream_0, test_case_3_3_4_stream_1, test_case_3_3_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_5, tgrp_case_3_3_5, NULL, name_case_3_3_5, NULL, desc_case_3_3_5, sref_case_3_3_5, {
	test_case_3_3_5_stream_0, test_case_3_3_5_stream_1, test_case_3_3_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_6, tgrp_case_3_3_6, NULL, name_case_3_3_6, NULL, desc_case_3_3_6, sref_case_3_3_6, {
	test_case_3_3_6_stream_0, test_case_3_3_6_stream_1, test_case_3_3_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_7, tgrp_case_3_3_7, NULL, name_case_3_3_7, NULL, desc_case_3_3_7, sref_case_3_3_7, {
	test_case_3_3_7_stream_0, test_case_3_3_7_stream_1, test_case_3_3_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_8, tgrp_case_3_3_8, NULL, name_case_3_3_8, NULL, desc_case_3_3_8, sref_case_3_3_8, {
	test_case_3_3_8_stream_0, test_case_3_3_8_stream_1, test_case_3_3_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_9, tgrp_case_3_3_9, NULL, name_case_3_3_9, NULL, desc_case_3_3_9, sref_case_3_3_9, {
	test_case_3_3_9_stream_0, test_case_3_3_9_stream_1, test_case_3_3_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_10, tgrp_case_3_3_10, NULL, name_case_3_3_10, NULL, desc_case_3_3_10, sref_case_3_3_10, {
	test_case_3_3_10_stream_0, test_case_3_3_10_stream_1, test_case_3_3_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_11, tgrp_case_3_3_11, NULL, name_case_3_3_11, NULL, desc_case_3_3_11, sref_case_3_3_11, {
	test_case_3_3_11_stream_0, test_case_3_3_11_stream_1, test_case_3_3_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_12, tgrp_case_3_3_12, NULL, name_case_3_3_12, NULL, desc_case_3_3_12, sref_case_3_3_12, {
	test_case_3_3_12_stream_0, test_case_3_3_12_stream_1, test_case_3_3_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_13, tgrp_case_3_3_13, NULL, name_case_3_3_13, NULL, desc_case_3_3_13, sref_case_3_3_13, {
	test_case_3_3_13_stream_0, test_case_3_3_13_stream_1, test_case_3_3_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_14, tgrp_case_3_3_14, NULL, name_case_3_3_14, NULL, desc_case_3_3_14, sref_case_3_3_14, {
	test_case_3_3_14_stream_0, test_case_3_3_14_stream_1, test_case_3_3_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_15, tgrp_case_3_3_15, NULL, name_case_3_3_15, NULL, desc_case_3_3_15, sref_case_3_3_15, {
	test_case_3_3_15_stream_0, test_case_3_3_15_stream_1, test_case_3_3_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_3_16, tgrp_case_3_3_16, NULL, name_case_3_3_16, NULL, desc_case_3_3_16, sref_case_3_3_16, {
	test_case_3_3_16_stream_0, test_case_3_3_16_stream_1, test_case_3_3_16_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_1, tgrp_case_3_4_1, NULL, name_case_3_4_1, NULL, desc_case_3_4_1, sref_case_3_4_1, {
	test_case_3_4_1_stream_0, test_case_3_4_1_stream_1, test_case_3_4_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_2, tgrp_case_3_4_2, NULL, name_case_3_4_2, NULL, desc_case_3_4_2, sref_case_3_4_2, {
	test_case_3_4_2_stream_0, test_case_3_4_2_stream_1, test_case_3_4_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_3, tgrp_case_3_4_3, NULL, name_case_3_4_3, NULL, desc_case_3_4_3, sref_case_3_4_3, {
	test_case_3_4_3_stream_0, test_case_3_4_3_stream_1, test_case_3_4_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_4, tgrp_case_3_4_4, NULL, name_case_3_4_4, NULL, desc_case_3_4_4, sref_case_3_4_4, {
	test_case_3_4_4_stream_0, test_case_3_4_4_stream_1, test_case_3_4_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_5, tgrp_case_3_4_5, NULL, name_case_3_4_5, NULL, desc_case_3_4_5, sref_case_3_4_5, {
	test_case_3_4_5_stream_0, test_case_3_4_5_stream_1, test_case_3_4_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_6, tgrp_case_3_4_6, NULL, name_case_3_4_6, NULL, desc_case_3_4_6, sref_case_3_4_6, {
	test_case_3_4_6_stream_0, test_case_3_4_6_stream_1, test_case_3_4_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_7, tgrp_case_3_4_7, NULL, name_case_3_4_7, NULL, desc_case_3_4_7, sref_case_3_4_7, {
	test_case_3_4_7_stream_0, test_case_3_4_7_stream_1, test_case_3_4_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_8, tgrp_case_3_4_8, NULL, name_case_3_4_8, NULL, desc_case_3_4_8, sref_case_3_4_8, {
	test_case_3_4_8_stream_0, test_case_3_4_8_stream_1, test_case_3_4_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_9, tgrp_case_3_4_9, NULL, name_case_3_4_9, NULL, desc_case_3_4_9, sref_case_3_4_9, {
	test_case_3_4_9_stream_0, test_case_3_4_9_stream_1, test_case_3_4_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_10, tgrp_case_3_4_10, NULL, name_case_3_4_10, NULL, desc_case_3_4_10, sref_case_3_4_10, {
	test_case_3_4_10_stream_0, test_case_3_4_10_stream_1, test_case_3_4_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_4_11, tgrp_case_3_4_11, NULL, name_case_3_4_11, NULL, desc_case_3_4_11, sref_case_3_4_11, {
	test_case_3_4_11_stream_0, test_case_3_4_11_stream_1, test_case_3_4_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_1, tgrp_case_3_5_1, NULL, name_case_3_5_1, NULL, desc_case_3_5_1, sref_case_3_5_1, {
	test_case_3_5_1_stream_0, test_case_3_5_1_stream_1, test_case_3_5_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_2, tgrp_case_3_5_2, NULL, name_case_3_5_2, NULL, desc_case_3_5_2, sref_case_3_5_2, {
	test_case_3_5_2_stream_0, test_case_3_5_2_stream_1, test_case_3_5_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_3, tgrp_case_3_5_3, NULL, name_case_3_5_3, NULL, desc_case_3_5_3, sref_case_3_5_3, {
	test_case_3_5_3_stream_0, test_case_3_5_3_stream_1, test_case_3_5_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_4, tgrp_case_3_5_4, NULL, name_case_3_5_4, NULL, desc_case_3_5_4, sref_case_3_5_4, {
	test_case_3_5_4_stream_0, test_case_3_5_4_stream_1, test_case_3_5_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_5, tgrp_case_3_5_5, NULL, name_case_3_5_5, NULL, desc_case_3_5_5, sref_case_3_5_5, {
	test_case_3_5_5_stream_0, test_case_3_5_5_stream_1, test_case_3_5_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_6, tgrp_case_3_5_6, NULL, name_case_3_5_6, NULL, desc_case_3_5_6, sref_case_3_5_6, {
	test_case_3_5_6_stream_0, test_case_3_5_6_stream_1, test_case_3_5_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_7, tgrp_case_3_5_7, NULL, name_case_3_5_7, NULL, desc_case_3_5_7, sref_case_3_5_7, {
	test_case_3_5_7_stream_0, test_case_3_5_7_stream_1, test_case_3_5_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_8, tgrp_case_3_5_8, NULL, name_case_3_5_8, NULL, desc_case_3_5_8, sref_case_3_5_8, {
	test_case_3_5_8_stream_0, test_case_3_5_8_stream_1, test_case_3_5_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_9, tgrp_case_3_5_9, NULL, name_case_3_5_9, NULL, desc_case_3_5_9, sref_case_3_5_9, {
	test_case_3_5_9_stream_0, test_case_3_5_9_stream_1, test_case_3_5_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_10, tgrp_case_3_5_10, NULL, name_case_3_5_10, NULL, desc_case_3_5_10, sref_case_3_5_10, {
	test_case_3_5_10_stream_0, test_case_3_5_10_stream_1, test_case_3_5_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_11, tgrp_case_3_5_11, NULL, name_case_3_5_11, NULL, desc_case_3_5_11, sref_case_3_5_11, {
	test_case_3_5_11_stream_0, test_case_3_5_11_stream_1, test_case_3_5_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_12_1, tgrp_case_3_5_12_1, NULL, name_case_3_5_12_1, NULL, desc_case_3_5_12_1, sref_case_3_5_12_1, {
	test_case_3_5_12_1_stream_0, test_case_3_5_12_1_stream_1, test_case_3_5_12_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_12_2, tgrp_case_3_5_12_2, NULL, name_case_3_5_12_2, NULL, desc_case_3_5_12_2, sref_case_3_5_12_2, {
	test_case_3_5_12_2_stream_0, test_case_3_5_12_2_stream_1, test_case_3_5_12_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_13_1, tgrp_case_3_5_13_1, NULL, name_case_3_5_13_1, NULL, desc_case_3_5_13_1, sref_case_3_5_13_1, {
	test_case_3_5_13_1_stream_0, test_case_3_5_13_1_stream_1, test_case_3_5_13_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_13_2, tgrp_case_3_5_13_2, NULL, name_case_3_5_13_2, NULL, desc_case_3_5_13_2, sref_case_3_5_13_2, {
	test_case_3_5_13_2_stream_0, test_case_3_5_13_2_stream_1, test_case_3_5_13_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_14_1, tgrp_case_3_5_14_1, NULL, name_case_3_5_14_1, NULL, desc_case_3_5_14_1, sref_case_3_5_14_1, {
	test_case_3_5_14_1_stream_0, test_case_3_5_14_1_stream_1, test_case_3_5_14_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_14_2, tgrp_case_3_5_14_2, NULL, name_case_3_5_14_2, NULL, desc_case_3_5_14_2, sref_case_3_5_14_2, {
	test_case_3_5_14_2_stream_0, test_case_3_5_14_2_stream_1, test_case_3_5_14_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_15_1, tgrp_case_3_5_15_1, NULL, name_case_3_5_15_1, NULL, desc_case_3_5_15_1, sref_case_3_5_15_1, {
	test_case_3_5_15_1_stream_0, test_case_3_5_15_1_stream_1, test_case_3_5_15_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_15_2, tgrp_case_3_5_15_2, NULL, name_case_3_5_15_2, NULL, desc_case_3_5_15_2, sref_case_3_5_15_2, {
	test_case_3_5_15_2_stream_0, test_case_3_5_15_2_stream_1, test_case_3_5_15_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_16, tgrp_case_3_5_16, NULL, name_case_3_5_16, NULL, desc_case_3_5_16, sref_case_3_5_16, {
	test_case_3_5_16_stream_0, test_case_3_5_16_stream_1, test_case_3_5_16_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_17, tgrp_case_3_5_17, NULL, name_case_3_5_17, NULL, desc_case_3_5_17, sref_case_3_5_17, {
	test_case_3_5_17_stream_0, test_case_3_5_17_stream_1, test_case_3_5_17_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_18, tgrp_case_3_5_18, NULL, name_case_3_5_18, NULL, desc_case_3_5_18, sref_case_3_5_18, {
	test_case_3_5_18_stream_0, test_case_3_5_18_stream_1, test_case_3_5_18_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_19, tgrp_case_3_5_19, NULL, name_case_3_5_19, NULL, desc_case_3_5_19, sref_case_3_5_19, {
	test_case_3_5_19_stream_0, test_case_3_5_19_stream_1, test_case_3_5_19_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_20, tgrp_case_3_5_20, NULL, name_case_3_5_20, NULL, desc_case_3_5_20, sref_case_3_5_20, {
	test_case_3_5_20_stream_0, test_case_3_5_20_stream_1, test_case_3_5_20_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_5_21, tgrp_case_3_5_21, NULL, name_case_3_5_21, NULL, desc_case_3_5_21, sref_case_3_5_21, {
	test_case_3_5_21_stream_0, test_case_3_5_21_stream_1, test_case_3_5_21_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_1, tgrp_case_3_6_1, NULL, name_case_3_6_1, NULL, desc_case_3_6_1, sref_case_3_6_1, {
	test_case_3_6_1_stream_0, test_case_3_6_1_stream_1, test_case_3_6_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_2, tgrp_case_3_6_2, NULL, name_case_3_6_2, NULL, desc_case_3_6_2, sref_case_3_6_2, {
	test_case_3_6_2_stream_0, test_case_3_6_2_stream_1, test_case_3_6_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_3, tgrp_case_3_6_3, NULL, name_case_3_6_3, NULL, desc_case_3_6_3, sref_case_3_6_3, {
	test_case_3_6_3_stream_0, test_case_3_6_3_stream_1, test_case_3_6_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_4, tgrp_case_3_6_4, NULL, name_case_3_6_4, NULL, desc_case_3_6_4, sref_case_3_6_4, {
	test_case_3_6_4_stream_0, test_case_3_6_4_stream_1, test_case_3_6_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_5, tgrp_case_3_6_5, NULL, name_case_3_6_5, NULL, desc_case_3_6_5, sref_case_3_6_5, {
	test_case_3_6_5_stream_0, test_case_3_6_5_stream_1, test_case_3_6_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_6, tgrp_case_3_6_6, NULL, name_case_3_6_6, NULL, desc_case_3_6_6, sref_case_3_6_6, {
	test_case_3_6_6_stream_0, test_case_3_6_6_stream_1, test_case_3_6_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_7, tgrp_case_3_6_7, NULL, name_case_3_6_7, NULL, desc_case_3_6_7, sref_case_3_6_7, {
	test_case_3_6_7_stream_0, test_case_3_6_7_stream_1, test_case_3_6_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_8, tgrp_case_3_6_8, NULL, name_case_3_6_8, NULL, desc_case_3_6_8, sref_case_3_6_8, {
	test_case_3_6_8_stream_0, test_case_3_6_8_stream_1, test_case_3_6_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_9, tgrp_case_3_6_9, NULL, name_case_3_6_9, NULL, desc_case_3_6_9, sref_case_3_6_9, {
	test_case_3_6_9_stream_0, test_case_3_6_9_stream_1, test_case_3_6_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_10, tgrp_case_3_6_10, NULL, name_case_3_6_10, NULL, desc_case_3_6_10, sref_case_3_6_10, {
	test_case_3_6_10_stream_0, test_case_3_6_10_stream_1, test_case_3_6_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_11, tgrp_case_3_6_11, NULL, name_case_3_6_11, NULL, desc_case_3_6_11, sref_case_3_6_11, {
	test_case_3_6_11_stream_0, test_case_3_6_11_stream_1, test_case_3_6_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_12, tgrp_case_3_6_12, NULL, name_case_3_6_12, NULL, desc_case_3_6_12, sref_case_3_6_12, {
	test_case_3_6_12_stream_0, test_case_3_6_12_stream_1, test_case_3_6_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_13, tgrp_case_3_6_13, NULL, name_case_3_6_13, NULL, desc_case_3_6_13, sref_case_3_6_13, {
	test_case_3_6_13_stream_0, test_case_3_6_13_stream_1, test_case_3_6_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_14, tgrp_case_3_6_14, NULL, name_case_3_6_14, NULL, desc_case_3_6_14, sref_case_3_6_14, {
	test_case_3_6_14_stream_0, test_case_3_6_14_stream_1, test_case_3_6_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_15, tgrp_case_3_6_15, NULL, name_case_3_6_15, NULL, desc_case_3_6_15, sref_case_3_6_15, {
	test_case_3_6_15_stream_0, test_case_3_6_15_stream_1, test_case_3_6_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_16_1, tgrp_case_3_6_16_1, NULL, name_case_3_6_16_1, NULL, desc_case_3_6_16_1, sref_case_3_6_16_1, {
	test_case_3_6_16_1_stream_0, test_case_3_6_16_1_stream_1, test_case_3_6_16_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_16_2, tgrp_case_3_6_16_2, NULL, name_case_3_6_16_2, NULL, desc_case_3_6_16_2, sref_case_3_6_16_2, {
	test_case_3_6_16_2_stream_0, test_case_3_6_16_2_stream_1, test_case_3_6_16_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_16_3, tgrp_case_3_6_16_3, NULL, name_case_3_6_16_3, NULL, desc_case_3_6_16_3, sref_case_3_6_16_3, {
	test_case_3_6_16_3_stream_0, test_case_3_6_16_3_stream_1, test_case_3_6_16_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_17_1, tgrp_case_3_6_17_1, NULL, name_case_3_6_17_1, NULL, desc_case_3_6_17_1, sref_case_3_6_17_1, {
	test_case_3_6_17_1_stream_0, test_case_3_6_17_1_stream_1, test_case_3_6_17_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_17_2, tgrp_case_3_6_17_2, NULL, name_case_3_6_17_2, NULL, desc_case_3_6_17_2, sref_case_3_6_17_2, {
	test_case_3_6_17_2_stream_0, test_case_3_6_17_2_stream_1, test_case_3_6_17_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_17_3, tgrp_case_3_6_17_3, NULL, name_case_3_6_17_3, NULL, desc_case_3_6_17_3, sref_case_3_6_17_3, {
	test_case_3_6_17_3_stream_0, test_case_3_6_17_3_stream_1, test_case_3_6_17_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_18_1, tgrp_case_3_6_18_1, NULL, name_case_3_6_18_1, NULL, desc_case_3_6_18_1, sref_case_3_6_18_1, {
	test_case_3_6_18_1_stream_0, test_case_3_6_18_1_stream_1, test_case_3_6_18_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_18_2, tgrp_case_3_6_18_2, NULL, name_case_3_6_18_2, NULL, desc_case_3_6_18_2, sref_case_3_6_18_2, {
	test_case_3_6_18_2_stream_0, test_case_3_6_18_2_stream_1, test_case_3_6_18_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_18_3, tgrp_case_3_6_18_3, NULL, name_case_3_6_18_3, NULL, desc_case_3_6_18_3, sref_case_3_6_18_3, {
	test_case_3_6_18_3_stream_0, test_case_3_6_18_3_stream_1, test_case_3_6_18_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_19_1, tgrp_case_3_6_19_1, NULL, name_case_3_6_19_1, NULL, desc_case_3_6_19_1, sref_case_3_6_19_1, {
	test_case_3_6_19_1_stream_0, test_case_3_6_19_1_stream_1, test_case_3_6_19_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_19_2, tgrp_case_3_6_19_2, NULL, name_case_3_6_19_2, NULL, desc_case_3_6_19_2, sref_case_3_6_19_2, {
	test_case_3_6_19_2_stream_0, test_case_3_6_19_2_stream_1, test_case_3_6_19_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_19_3, tgrp_case_3_6_19_3, NULL, name_case_3_6_19_3, NULL, desc_case_3_6_19_3, sref_case_3_6_19_3, {
	test_case_3_6_19_3_stream_0, test_case_3_6_19_3_stream_1, test_case_3_6_19_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_20, tgrp_case_3_6_20, NULL, name_case_3_6_20, NULL, desc_case_3_6_20, sref_case_3_6_20, {
	test_case_3_6_20_stream_0, test_case_3_6_20_stream_1, test_case_3_6_20_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_21, tgrp_case_3_6_21, NULL, name_case_3_6_21, NULL, desc_case_3_6_21, sref_case_3_6_21, {
	test_case_3_6_21_stream_0, test_case_3_6_21_stream_1, test_case_3_6_21_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_22, tgrp_case_3_6_22, NULL, name_case_3_6_22, NULL, desc_case_3_6_22, sref_case_3_6_22, {
	test_case_3_6_22_stream_0, test_case_3_6_22_stream_1, test_case_3_6_22_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_23, tgrp_case_3_6_23, NULL, name_case_3_6_23, NULL, desc_case_3_6_23, sref_case_3_6_23, {
	test_case_3_6_23_stream_0, test_case_3_6_23_stream_1, test_case_3_6_23_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_24, tgrp_case_3_6_24, NULL, name_case_3_6_24, NULL, desc_case_3_6_24, sref_case_3_6_24, {
	test_case_3_6_24_stream_0, test_case_3_6_24_stream_1, test_case_3_6_24_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_25, tgrp_case_3_6_25, NULL, name_case_3_6_25, NULL, desc_case_3_6_25, sref_case_3_6_25, {
	test_case_3_6_25_stream_0, test_case_3_6_25_stream_1, test_case_3_6_25_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_6_26, tgrp_case_3_6_26, NULL, name_case_3_6_26, NULL, desc_case_3_6_26, sref_case_3_6_26, {
	test_case_3_6_26_stream_0, test_case_3_6_26_stream_1, test_case_3_6_26_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_1, tgrp_case_3_7_1, NULL, name_case_3_7_1, NULL, desc_case_3_7_1, sref_case_3_7_1, {
	test_case_3_7_1_stream_0, test_case_3_7_1_stream_1, test_case_3_7_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_2, tgrp_case_3_7_2, NULL, name_case_3_7_2, NULL, desc_case_3_7_2, sref_case_3_7_2, {
	test_case_3_7_2_stream_0, test_case_3_7_2_stream_1, test_case_3_7_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_3, tgrp_case_3_7_3, NULL, name_case_3_7_3, NULL, desc_case_3_7_3, sref_case_3_7_3, {
	test_case_3_7_3_stream_0, test_case_3_7_3_stream_1, test_case_3_7_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_4, tgrp_case_3_7_4, NULL, name_case_3_7_4, NULL, desc_case_3_7_4, sref_case_3_7_4, {
	test_case_3_7_4_stream_0, test_case_3_7_4_stream_1, test_case_3_7_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_5, tgrp_case_3_7_5, NULL, name_case_3_7_5, NULL, desc_case_3_7_5, sref_case_3_7_5, {
	test_case_3_7_5_stream_0, test_case_3_7_5_stream_1, test_case_3_7_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_6, tgrp_case_3_7_6, NULL, name_case_3_7_6, NULL, desc_case_3_7_6, sref_case_3_7_6, {
	test_case_3_7_6_stream_0, test_case_3_7_6_stream_1, test_case_3_7_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_7, tgrp_case_3_7_7, NULL, name_case_3_7_7, NULL, desc_case_3_7_7, sref_case_3_7_7, {
	test_case_3_7_7_stream_0, test_case_3_7_7_stream_1, test_case_3_7_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_8, tgrp_case_3_7_8, NULL, name_case_3_7_8, NULL, desc_case_3_7_8, sref_case_3_7_8, {
	test_case_3_7_8_stream_0, test_case_3_7_8_stream_1, test_case_3_7_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_9, tgrp_case_3_7_9, NULL, name_case_3_7_9, NULL, desc_case_3_7_9, sref_case_3_7_9, {
	test_case_3_7_9_stream_0, test_case_3_7_9_stream_1, test_case_3_7_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_10, tgrp_case_3_7_10, NULL, name_case_3_7_10, NULL, desc_case_3_7_10, sref_case_3_7_10, {
	test_case_3_7_10_stream_0, test_case_3_7_10_stream_1, test_case_3_7_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_11, tgrp_case_3_7_11, NULL, name_case_3_7_11, NULL, desc_case_3_7_11, sref_case_3_7_11, {
	test_case_3_7_11_stream_0, test_case_3_7_11_stream_1, test_case_3_7_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_12, tgrp_case_3_7_12, NULL, name_case_3_7_12, NULL, desc_case_3_7_12, sref_case_3_7_12, {
	test_case_3_7_12_stream_0, test_case_3_7_12_stream_1, test_case_3_7_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_13, tgrp_case_3_7_13, NULL, name_case_3_7_13, NULL, desc_case_3_7_13, sref_case_3_7_13, {
	test_case_3_7_13_stream_0, test_case_3_7_13_stream_1, test_case_3_7_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_14, tgrp_case_3_7_14, NULL, name_case_3_7_14, NULL, desc_case_3_7_14, sref_case_3_7_14, {
	test_case_3_7_14_stream_0, test_case_3_7_14_stream_1, test_case_3_7_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_15, tgrp_case_3_7_15, NULL, name_case_3_7_15, NULL, desc_case_3_7_15, sref_case_3_7_15, {
	test_case_3_7_15_stream_0, test_case_3_7_15_stream_1, test_case_3_7_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_16, tgrp_case_3_7_16, NULL, name_case_3_7_16, NULL, desc_case_3_7_16, sref_case_3_7_16, {
	test_case_3_7_16_stream_0, test_case_3_7_16_stream_1, test_case_3_7_16_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_17, tgrp_case_3_7_17, NULL, name_case_3_7_17, NULL, desc_case_3_7_17, sref_case_3_7_17, {
	test_case_3_7_17_stream_0, test_case_3_7_17_stream_1, test_case_3_7_17_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_18, tgrp_case_3_7_18, NULL, name_case_3_7_18, NULL, desc_case_3_7_18, sref_case_3_7_18, {
	test_case_3_7_18_stream_0, test_case_3_7_18_stream_1, test_case_3_7_18_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_7_19, tgrp_case_3_7_19, NULL, name_case_3_7_19, NULL, desc_case_3_7_19, sref_case_3_7_19, {
	test_case_3_7_19_stream_0, test_case_3_7_19_stream_1, test_case_3_7_19_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_1, tgrp_case_3_8_1, NULL, name_case_3_8_1, NULL, desc_case_3_8_1, sref_case_3_8_1, {
	test_case_3_8_1_stream_0, test_case_3_8_1_stream_1, test_case_3_8_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_2, tgrp_case_3_8_2, NULL, name_case_3_8_2, NULL, desc_case_3_8_2, sref_case_3_8_2, {
	test_case_3_8_2_stream_0, test_case_3_8_2_stream_1, test_case_3_8_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_3, tgrp_case_3_8_3, NULL, name_case_3_8_3, NULL, desc_case_3_8_3, sref_case_3_8_3, {
	test_case_3_8_3_stream_0, test_case_3_8_3_stream_1, test_case_3_8_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_4, tgrp_case_3_8_4, NULL, name_case_3_8_4, NULL, desc_case_3_8_4, sref_case_3_8_4, {
	test_case_3_8_4_stream_0, test_case_3_8_4_stream_1, test_case_3_8_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_5, tgrp_case_3_8_5, NULL, name_case_3_8_5, NULL, desc_case_3_8_5, sref_case_3_8_5, {
	test_case_3_8_5_stream_0, test_case_3_8_5_stream_1, test_case_3_8_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_6, tgrp_case_3_8_6, NULL, name_case_3_8_6, NULL, desc_case_3_8_6, sref_case_3_8_6, {
	test_case_3_8_6_stream_0, test_case_3_8_6_stream_1, test_case_3_8_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_7, tgrp_case_3_8_7, NULL, name_case_3_8_7, NULL, desc_case_3_8_7, sref_case_3_8_7, {
	test_case_3_8_7_stream_0, test_case_3_8_7_stream_1, test_case_3_8_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_8, tgrp_case_3_8_8, NULL, name_case_3_8_8, NULL, desc_case_3_8_8, sref_case_3_8_8, {
	test_case_3_8_8_stream_0, test_case_3_8_8_stream_1, test_case_3_8_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_9, tgrp_case_3_8_9, NULL, name_case_3_8_9, NULL, desc_case_3_8_9, sref_case_3_8_9, {
	test_case_3_8_9_stream_0, test_case_3_8_9_stream_1, test_case_3_8_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_10, tgrp_case_3_8_10, NULL, name_case_3_8_10, NULL, desc_case_3_8_10, sref_case_3_8_10, {
	test_case_3_8_10_stream_0, test_case_3_8_10_stream_1, test_case_3_8_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_11, tgrp_case_3_8_11, NULL, name_case_3_8_11, NULL, desc_case_3_8_11, sref_case_3_8_11, {
	test_case_3_8_11_stream_0, test_case_3_8_11_stream_1, test_case_3_8_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_12, tgrp_case_3_8_12, NULL, name_case_3_8_12, NULL, desc_case_3_8_12, sref_case_3_8_12, {
	test_case_3_8_12_stream_0, test_case_3_8_12_stream_1, test_case_3_8_12_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_13, tgrp_case_3_8_13, NULL, name_case_3_8_13, NULL, desc_case_3_8_13, sref_case_3_8_13, {
	test_case_3_8_13_stream_0, test_case_3_8_13_stream_1, test_case_3_8_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_14, tgrp_case_3_8_14, NULL, name_case_3_8_14, NULL, desc_case_3_8_14, sref_case_3_8_14, {
	test_case_3_8_14_stream_0, test_case_3_8_14_stream_1, test_case_3_8_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_15, tgrp_case_3_8_15, NULL, name_case_3_8_15, NULL, desc_case_3_8_15, sref_case_3_8_15, {
	test_case_3_8_15_stream_0, test_case_3_8_15_stream_1, test_case_3_8_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_16, tgrp_case_3_8_16, NULL, name_case_3_8_16, NULL, desc_case_3_8_16, sref_case_3_8_16, {
	test_case_3_8_16_stream_0, test_case_3_8_16_stream_1, test_case_3_8_16_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_17, tgrp_case_3_8_17, NULL, name_case_3_8_17, NULL, desc_case_3_8_17, sref_case_3_8_17, {
	test_case_3_8_17_stream_0, test_case_3_8_17_stream_1, test_case_3_8_17_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_18, tgrp_case_3_8_18, NULL, name_case_3_8_18, NULL, desc_case_3_8_18, sref_case_3_8_18, {
	test_case_3_8_18_stream_0, test_case_3_8_18_stream_1, test_case_3_8_18_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_8_19, tgrp_case_3_8_19, NULL, name_case_3_8_19, NULL, desc_case_3_8_19, sref_case_3_8_19, {
	test_case_3_8_19_stream_0, test_case_3_8_19_stream_1, test_case_3_8_19_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_9_1, tgrp_case_3_9_1, NULL, name_case_3_9_1, NULL, desc_case_3_9_1, sref_case_3_9_1, {
	test_case_3_9_1_stream_0, test_case_3_9_1_stream_1, test_case_3_9_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_9_2, tgrp_case_3_9_2, NULL, name_case_3_9_2, NULL, desc_case_3_9_2, sref_case_3_9_2, {
	test_case_3_9_2_stream_0, test_case_3_9_2_stream_1, test_case_3_9_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_9_3, tgrp_case_3_9_3, NULL, name_case_3_9_3, NULL, desc_case_3_9_3, sref_case_3_9_3, {
	test_case_3_9_3_stream_0, test_case_3_9_3_stream_1, test_case_3_9_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_1, tgrp_case_3_10_1, NULL, name_case_3_10_1, NULL, desc_case_3_10_1, sref_case_3_10_1, {
	test_case_3_10_1_stream_0, test_case_3_10_1_stream_1, test_case_3_10_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_2, tgrp_case_3_10_2, NULL, name_case_3_10_2, NULL, desc_case_3_10_2, sref_case_3_10_2, {
	test_case_3_10_2_stream_0, test_case_3_10_2_stream_1, test_case_3_10_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_3, tgrp_case_3_10_3, NULL, name_case_3_10_3, NULL, desc_case_3_10_3, sref_case_3_10_3, {
	test_case_3_10_3_stream_0, test_case_3_10_3_stream_1, test_case_3_10_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_4, tgrp_case_3_10_4, NULL, name_case_3_10_4, NULL, desc_case_3_10_4, sref_case_3_10_4, {
	test_case_3_10_4_stream_0, test_case_3_10_4_stream_1, test_case_3_10_4_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_5, tgrp_case_3_10_5, NULL, name_case_3_10_5, NULL, desc_case_3_10_5, sref_case_3_10_5, {
	test_case_3_10_5_stream_0, test_case_3_10_5_stream_1, test_case_3_10_5_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_6, tgrp_case_3_10_6, NULL, name_case_3_10_6, NULL, desc_case_3_10_6, sref_case_3_10_6, {
	test_case_3_10_6_stream_0, test_case_3_10_6_stream_1, test_case_3_10_6_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_7, tgrp_case_3_10_7, NULL, name_case_3_10_7, NULL, desc_case_3_10_7, sref_case_3_10_7, {
	test_case_3_10_7_stream_0, test_case_3_10_7_stream_1, test_case_3_10_7_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_8, tgrp_case_3_10_8, NULL, name_case_3_10_8, NULL, desc_case_3_10_8, sref_case_3_10_8, {
	test_case_3_10_8_stream_0, test_case_3_10_8_stream_1, test_case_3_10_8_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_9, tgrp_case_3_10_9, NULL, name_case_3_10_9, NULL, desc_case_3_10_9, sref_case_3_10_9, {
	test_case_3_10_9_stream_0, test_case_3_10_9_stream_1, test_case_3_10_9_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_10, tgrp_case_3_10_10, NULL, name_case_3_10_10, NULL, desc_case_3_10_10, sref_case_3_10_10, {
	test_case_3_10_10_stream_0, test_case_3_10_10_stream_1, test_case_3_10_10_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_11, tgrp_case_3_10_11, NULL, name_case_3_10_11, NULL, desc_case_3_10_11, sref_case_3_10_11, {
	test_case_3_10_11_stream_0, test_case_3_10_11_stream_1, test_case_3_10_11_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_12_1, tgrp_case_3_10_12_1, NULL, name_case_3_10_12_1, NULL, desc_case_3_10_12_1, sref_case_3_10_12_1, {
	test_case_3_10_12_1_stream_0, test_case_3_10_12_1_stream_1, test_case_3_10_12_1_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_12_2, tgrp_case_3_10_12_2, NULL, name_case_3_10_12_2, NULL, desc_case_3_10_12_2, sref_case_3_10_12_2, {
	test_case_3_10_12_2_stream_0, test_case_3_10_12_2_stream_1, test_case_3_10_12_2_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_12_3, tgrp_case_3_10_12_3, NULL, name_case_3_10_12_3, NULL, desc_case_3_10_12_3, sref_case_3_10_12_3, {
	test_case_3_10_12_3_stream_0, test_case_3_10_12_3_stream_1, test_case_3_10_12_3_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_13, tgrp_case_3_10_13, NULL, name_case_3_10_13, NULL, desc_case_3_10_13, sref_case_3_10_13, {
	test_case_3_10_13_stream_0, test_case_3_10_13_stream_1, test_case_3_10_13_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_14, tgrp_case_3_10_14, NULL, name_case_3_10_14, NULL, desc_case_3_10_14, sref_case_3_10_14, {
	test_case_3_10_14_stream_0, test_case_3_10_14_stream_1, test_case_3_10_14_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_10_15, tgrp_case_3_10_15, NULL, name_case_3_10_15, NULL, desc_case_3_10_15, sref_case_3_10_15, {
	test_case_3_10_15_stream_0, test_case_3_10_15_stream_1, test_case_3_10_15_stream_2}, &begin_tests, &end_tests, 0, 0, __RESULT_SUCCESS}, {
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
		fprintf(stdout, "\nUsing device %s, %s, %s\n\n", devname, muxname, fifoname);
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
