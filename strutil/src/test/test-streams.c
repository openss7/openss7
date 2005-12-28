/*****************************************************************************

 @(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2005/12/28 10:01:24 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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
 General Public License (GPL) Version 2 or later, so long as the software is
 distributed with, and only used for the testing of, OpenSS7 modules, drivers,
 and libraries.

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

 Last Modified $Date: 2005/12/28 10:01:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-streams.c,v $
 Revision 0.9.2.19  2005/12/28 10:01:24  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.18  2005/07/18 12:38:50  brian
 - standard indentation

 Revision 0.9.2.17  2005/07/17 08:06:43  brian
 - changes for first build

 Revision 0.9.2.16  2005/07/01 07:29:32  brian
 - updates for LE2005 build

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

#ident "@(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2005/12/28 10:01:24 $"

static char const ident[] = "$RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.19 $) $Date: 2005/12/28 10:01:24 $";

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

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
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
static const char *sstdname = "XSI";
static const char *shortname = "STREAMS";
static char devname[256] = "/dev/echo";

static int exit_on_failure = 0;

static int verbose = 1;

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
static int show_data = 1;

static int last_prim = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;

int test_fd[3] = { 0, 0, 0 };

#define BUFSIZE 5*4096

#define FFLUSH(stream)

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 100	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define INFINITE_WAIT	-1UL
#define TEST_DURATION	20000

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

static int timer_timeout = 0;

static void
timer_handler(int signum)
{
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

static int
timer_sethandler(void)
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
	siginterrupt(SIGALRM, 1);
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

	if (timer_sethandler())
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
stop_tt(void)
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
		return ("I_NREAD");
	case I_PUSH:
		return ("I_PUSH");
	case I_POP:
		return ("I_POP");
	case I_LOOK:
		return ("I_LOOK");
	case I_FLUSH:
		return ("I_FLUSH");
	case I_SRDOPT:
		return ("I_SRDOPT");
	case I_GRDOPT:
		return ("I_GRDOPT");
	case I_STR:
		return ("I_STR");
	case I_SETSIG:
		return ("I_SETSIG");
	case I_GETSIG:
		return ("I_GETSIG");
	case I_FIND:
		return ("I_FIND");
	case I_LINK:
		return ("I_LINK");
	case I_UNLINK:
		return ("I_UNLINK");
	case I_RECVFD:
		return ("I_RECVFD");
	case I_PEEK:
		return ("I_PEEK");
	case I_FDINSERT:
		return ("I_FDINSERT");
	case I_SENDFD:
		return ("I_SENDFD");
#if 0
	case I_E_RECVFD:
		return ("I_E_RECVFD");
#endif
	case I_SWROPT:
		return ("I_SWROPT");
	case I_GWROPT:
		return ("I_GWROPT");
	case I_LIST:
		return ("I_LIST");
	case I_PLINK:
		return ("I_PLINK");
	case I_PUNLINK:
		return ("I_PUNLINK");
	case I_FLUSHBAND:
		return ("I_FLUSHBAND");
	case I_CKBAND:
		return ("I_CKBAND");
	case I_GETBAND:
		return ("I_GETBAND");
	case I_ATMARK:
		return ("I_ATMARK");
	case I_SETCLTIME:
		return ("I_SETCLTIME");
	case I_GETCLTIME:
		return ("I_GETCLTIME");
	case I_CANPUT:
		return ("I_CANPUT");
#if 0
	case I_SERROPT:
		return ("I_SERROPT");
	case I_GERROPT:
		return ("I_GERROPT");
	case I_ANCHOR:
		return ("I_ANCHOR");
#endif
#if 0
	case I_S_RECVFD:
		return ("I_S_RECVFD");
	case I_STATS:
		return ("I_STATS");
	case I_BIGPIPE:
		return ("I_BIGPIPE");
#endif
#if 0
	case I_GETTP:
		return ("I_GETTP");
	case I_AUTOPUSH:
		return ("I_AUTOPUSH");
	case I_HEAP_REPORT:
		return ("I_HEAP_REPORT");
	case I_FIFO:
		return ("I_FIFO");
	case I_PUTPMSG:
		return ("I_PUTPMSG");
	case I_GETPMSG:
		return ("I_GETPMSG");
	case I_FATTACH:
		return ("I_FATTACH");
	case I_FDETACH:
		return ("I_FDETACH");
	case I_PIPE:
		return ("I_PIPE");
#endif
	default:
		return ("(unexpected)");
	}
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
print_open(int child)
{
	static const char *msgs[] = {
		"open()        ----->v                                |  |                   \n",
		"  open()      ----->v                                |  |                   \n",
		"    open()    ----->v                                |  |                   \n",
		"                    .                                .  .                   \n",
	};

	if (verbose > 3)
		print_simple(child, msgs);
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
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                     [%d:%03d]\n",
		"  XXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                     [%d:%03d]\n",
		"    XXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                     [%d:%03d]\n",
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
	};

	if (verbose > 0)
		print_simple_int(child, msgs, state);
}

void
print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########### SCRIPT ERROR ######|##|                     [%d:%03d]\n",
		"  ##################|########### SCRIPT ERROR ######|##|                     [%d:%03d]\n",
		"    ################|########### SCRIPT ERROR ######|##|                     [%d:%03d]\n",
		"####################|########### SCRIPT ERROR ######|##|#################### [%d:%03d]\n",
	};

	if (verbose > 0)
		print_simple_int(child, msgs, state);
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
		print_simple_int(child, msgs, state);
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
print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s<----/|                                |  |                    [%d:%03d]\n",
		"  %-14s<--/|                                |  |                    [%d:%03d]\n",
		"    %-14s</|                                |  |                    [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_command(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s<----/|                                |  |                    [%d:%03d]\n",
		"  %-14s<--/|                                |  |                    [%d:%03d]\n",
		"    %-14s</|                                |  |                    [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_string_state(child, msgs, command);
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

	if (verbose > 4)
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

	if (verbose)
		print_triple_int(child, msgs, value, child, state);
}

void
print_ioctl(int child, int cmd, intptr_t arg)
{
	static const char *msgs[] = {
		"ioctl(2)----------->|       %16s         |  |                    [%d:%03d]\n",
		"  ioctl(2)--------->|       %16s         |  |                    [%d:%03d]\n",
		"    ioctl(2)------->|       %16s         |  |                    [%d:%03d]\n",
		"                    |       %16s ioctl(2)|  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_string_state(child, msgs, ioctl_string(cmd, arg));
}

void
print_datcall(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"%1$14s- - ->|- - %2$4d bytes- - - - - - - - ->|- |                    [%3$d:%4$03d]\n",
		"  %1$14s- ->|- - %2$4d bytes- - - - - - - - ->|- |                    [%3$d:%4$03d]\n",
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

	if (verbose > 1 && show)
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
		if (verbose > 3)
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
	if (test_ioctl(child, I_FDINSERT, (intptr_t) & fdi) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

int
test_putpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int band, int flags)
{
	if (flags & MSG_BAND || band) {
		if (verbose > 3) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putpmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
			fflush(stdout);
		}
		if (ctrl == NULL || data != NULL)
			print_datcall(child, "putpmsg(2)----", data ? data->len : 0);
		for (;;) {
			if ((last_retval = putpmsg(test_fd[child], ctrl, data, band, flags)) == -1) {
				print_errno(child, (last_errno = errno));
				if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
					continue;
				return (__RESULT_FAILURE);
			}
			if (verbose > 3)
				print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	} else {
		if (verbose > 3) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
			fflush(stdout);
		}
		if (ctrl == NULL || data != NULL)
			print_datcall(child, "putmsg(2)-----", data ? data->len : 0);
		for (;;) {
			if ((last_retval = putmsg(test_fd[child], ctrl, data, flags)) == -1) {
				print_errno(child, (last_errno = errno));
				if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
					continue;
				return (__RESULT_FAILURE);
			}
			if (verbose > 3)
				print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	}
}

int
test_write(int child, const void *buf, size_t len)
{
	print_syscall(child, "write(2)------");
	for (;;) {
		if ((last_retval = write(test_fd[child], buf, len)) == -1) {
			print_errno(child, (last_errno = errno));
			if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
				continue;
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
			print_errno(child, (last_errno = errno));
			if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
				continue;
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
	print_syscall(child, "getmsg(2)-----");
	for (;;) {
		if ((last_retval = getmsg(test_fd[child], ctrl, data, flagp)) == -1) {
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
	print_syscall(child, "getpmsg(2)----");
	for (;;) {
		if ((last_retval = getpmsg(test_fd[child], ctrl, data, bandp, flagp)) == -1) {
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
	print_syscall(child, "read(2)-------");
	for (;;) {
		if ((last_retval = read(test_fd[child], buf, count)) == -1) {
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
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags | O_NONBLOCK)) == -1) {
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
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
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags & ~O_NONBLOCK)) == -1) {
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
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
		print_errno(child, (last_errno = errno));
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
			continue;
		return (__RESULT_FAILURE);
	}
}

int
test_open(int child, const char *name)
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

int
test_close(int child)
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
 *  STREAM Initialization
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
	if (!test_fd[child] && test_open(child, devname) != __RESULT_SUCCESS)
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

/*
 *  Open and Close 1 stream.
 */
#define test_group_1 "Open and close streams"
#define tgrp_case_1_1 test_group_1
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Open and close 1 stream."
#define sref_case_1_1 "(none)"
#define desc_case_1_1 "\
Checks that one stream can be opened and closed."

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
 *  Open and Close 3 streams.
 */
#define tgrp_case_1_2 test_group_1
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Open and close 3 streams."
#define sref_case_1_2 "(none)"
#define desc_case_1_2 "\
Checks that three streams can be opened and closed."

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

#define test_group_2 "Perform IOCTL on one stream"

/*
 *  Perform IOCTL on one stream - I_NREAD
 */
#define tgrp_case_2_1_1 test_group_2
#define numb_case_2_1_1 "2.1.1"
#define name_case_2_1_1 "Perform streamio I_NREAD."
#define sref_case_2_1_1 "(none)"
#define desc_case_2_1_1 "\
Checks that I_NREAD can be performed on a stream.  Because this test is peformed\n\
on a freshly opened stream, it should return zero (0) as a return value and\n\
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
#define sref_case_2_1_2 "(none)"
#define desc_case_2_1_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_1_2(int child)
{
	if (test_ioctl(child, I_NREAD, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_1_2 = { &preamble_0, &test_case_2_1_2, &postamble_0 };

#define test_case_2_1_2_stream_0 (&test_2_1_2)
#define test_case_2_1_2_stream_1 (NULL)
#define test_case_2_1_2_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PUSH
 */
#define tgrp_case_2_2_1 test_group_2
#define numb_case_2_2_1 "2.2.1"
#define name_case_2_2_1 "Perform streamio I_PUSH."
#define sref_case_2_2_1 "(none)"
#define desc_case_2_2_1 "\
Checks that I_PUSH can be performed on a stream."

int
test_case_2_2_1(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nomodule") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_1 = { &preamble_0, &test_case_2_2_1, &postamble_0 };

#define test_case_2_2_1_stream_0 (&test_2_2_1)
#define test_case_2_2_1_stream_1 (NULL)
#define test_case_2_2_1_stream_2 (NULL)

#define tgrp_case_2_2_2 test_group_2
#define numb_case_2_2_2 "2.2.2"
#define name_case_2_2_2 "Perform streamio I_PUSH - EINVAL."
#define sref_case_2_2_2 "(none)"
#define desc_case_2_2_2 "\
Checks that EINVAL is returned when I_PUSH is performed with an invalid module\n\
name \"nomodule\"."

int
test_case_2_2_2(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) "nomodule") == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_2 = { &preamble_0, &test_case_2_2_2, &postamble_0 };

#define test_case_2_2_2_stream_0 (&test_2_2_2)
#define test_case_2_2_2_stream_1 (NULL)
#define test_case_2_2_2_stream_2 (NULL)

#define tgrp_case_2_2_3 test_group_2
#define numb_case_2_2_3 "2.2.3"
#define name_case_2_2_3 "Perform streamio I_PUSH - EFAULT."
#define sref_case_2_2_3 "(none)"
#define desc_case_2_2_3 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_2_3(int child)
{
	if (test_ioctl(child, I_PUSH, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_2_3 = { &preamble_0, &test_case_2_2_3, &postamble_0 };

#define test_case_2_2_3_stream_0 (&test_2_2_3)
#define test_case_2_2_3_stream_1 (NULL)
#define test_case_2_2_3_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_POP
 */
#define tgrp_case_2_3 test_group_2
#define numb_case_2_3 "2.3"
#define name_case_2_3 "Perform streamio I_POP."
#define sref_case_2_3 "(none)"
#define desc_case_2_3 "\
Checks that I_POP can be performed on a stream."

int
test_case_2_3(int child)
{
	if (test_ioctl(child, I_POP, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_3 = { &preamble_0, &test_case_2_3, &postamble_0 };

#define test_case_2_3_stream_0 (&test_2_3)
#define test_case_2_3_stream_1 (NULL)
#define test_case_2_3_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_LOOK
 */
#define tgrp_case_2_4 test_group_2
#define numb_case_2_4 "2.4"
#define name_case_2_4 "Perform streamio I_LOOK."
#define sref_case_2_4 "(none)"
#define desc_case_2_4 "\
Checks that I_LOOK can be performed on a stream."

int
test_case_2_4(int child)
{
	char buf[FMNAMESZ + 1];

	if (test_ioctl(child, I_LOOK, (intptr_t) buf) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_4 = { &preamble_0, &test_case_2_4, &postamble_0 };

#define test_case_2_4_stream_0 (&test_2_4)
#define test_case_2_4_stream_1 (NULL)
#define test_case_2_4_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FLUSH
 */
#define tgrp_case_2_5 test_group_2
#define numb_case_2_5 "2.5"
#define name_case_2_5 "Perform streamio I_FLUSH."
#define sref_case_2_5 "(none)"
#define desc_case_2_5 "\
Checks that I_FLUSH can be performed on a stream."

int
test_case_2_5(int child)
{
#if 1
	return __RESULT_INCONCLUSIVE;
#else
	if (test_ioctl(child, I_FLUSH, 0) != __RESULT_SUCCESS && last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
#endif
}
struct test_stream test_2_5 = { &preamble_0, &test_case_2_5, &postamble_0 };

#define test_case_2_5_stream_0 (&test_2_5)
#define test_case_2_5_stream_1 (NULL)
#define test_case_2_5_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SRDOPT
 */
#define tgrp_case_2_6_1 test_group_2
#define numb_case_2_6_1 "2.6.1"
#define name_case_2_6_1 "Perform streamio I_SRDOPT - EINVAL."
#define sref_case_2_6_1 "(none)"
#define desc_case_2_6_1 "\
Checks that I_SRDOPT can be performed on a stream.  This case is performed with\n\
a zero argument and should return EINVAL."

int
test_case_2_6_1(int child)
{
	if (test_ioctl(child, I_SRDOPT, 0) == __RESULT_SUCCESS || last_errno != EINVAL)
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
#define sref_case_2_6_2 "(none)"
#define desc_case_2_6_2 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_3 "(none)"
#define desc_case_2_6_3 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_4 "(none)"
#define desc_case_2_6_4 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define numb_case_2_6_5 "2.6.4"
#define name_case_2_6_5 "Perform streamio I_SRDOPT - RMSGN | RPROTNORM."
#define sref_case_2_6_5 "(none)"
#define desc_case_2_6_5 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_6 "(none)"
#define desc_case_2_6_6 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_7 "(none)"
#define desc_case_2_6_7 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_8 "(none)"
#define desc_case_2_6_8 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_9 "(none)"
#define desc_case_2_6_9 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_10 "(none)"
#define desc_case_2_6_10 "\
Checks that I_SRDOPT can be performed on a stream with the values \n\
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
#define sref_case_2_6_11 "(none)"
#define desc_case_2_6_11 "\
Checks that EINVAL is returned when I_SRDOPT is called with an invalid\n\
argument (-1UL)."

int
test_case_2_6_11(int child)
{
	if (test_ioctl(child, I_SRDOPT, -1UL) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_6_11 = { &preamble_0, &test_case_2_6_11, &postamble_0 };

#define test_case_2_6_11_stream_0 (&test_2_6_11)
#define test_case_2_6_11_stream_1 (NULL)
#define test_case_2_6_11_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GRDOPT
 */
#define tgrp_case_2_7_1 test_group_2
#define numb_case_2_7_1 "2.7.1"
#define name_case_2_7_1 "Perform streamio I_GRDOPT - default."
#define sref_case_2_7_1 "(none)"
#define desc_case_2_7_1 "\
Checks that I_GRDOPT can be performed on a stream to read the stream default\n\
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
#define sref_case_2_7_2 "(none)"
#define desc_case_2_7_2 "\
Checks that I_GRDOPT can be performed on a stream to read the stream default\n\
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
#define sref_case_2_7_3 "(none)"
#define desc_case_2_7_3 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_4 "(none)"
#define desc_case_2_7_4 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_5 "(none)"
#define desc_case_2_7_5 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_6 "(none)"
#define desc_case_2_7_6 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_7 "(none)"
#define desc_case_2_7_7 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_8 "(none)"
#define desc_case_2_7_8 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_9 "(none)"
#define desc_case_2_7_9 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_10 "(none)"
#define desc_case_2_7_10 "\
Checks that I_GRDOPT can be performed on a stream to read the read options\n\
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
#define sref_case_2_7_11 "(none)"
#define desc_case_2_7_11 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_7_11(int child)
{
	if (test_ioctl(child, I_GRDOPT, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_7_11 = { &preamble_0, &test_case_2_7_11, &postamble_0 };

#define test_case_2_7_11_stream_0 (&test_2_7_11)
#define test_case_2_7_11_stream_1 (NULL)
#define test_case_2_7_11_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_STR
 */
#define tgrp_case_2_8 test_group_2
#define numb_case_2_8 "2.8"
#define name_case_2_8 "Perform streamio I_STR."
#define sref_case_2_8 "(none)"
#define desc_case_2_8 "\
Checks that I_STR can be performed on a stream."

int
test_case_2_8(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_8 = { &preamble_0, &test_case_2_8, &postamble_0 };

#define test_case_2_8_stream_0 (&test_2_8)
#define test_case_2_8_stream_1 (NULL)
#define test_case_2_8_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SETSIG
 */
#define tgrp_case_2_9 test_group_2
#define numb_case_2_9 "2.9"
#define name_case_2_9 "Perform streamio I_SETSIG."
#define sref_case_2_9 "(none)"
#define desc_case_2_9 "\
Checks that I_SETSIG can be performed on a stream."

int
test_case_2_9(int child)
{
	if (test_ioctl(child, I_SETSIG, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_9 = { &preamble_0, &test_case_2_9, &postamble_0 };

#define test_case_2_9_stream_0 (&test_2_9)
#define test_case_2_9_stream_1 (NULL)
#define test_case_2_9_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETSIG
 */
#define tgrp_case_2_10_1 test_group_2
#define numb_case_2_10_1 "2.10.1"
#define name_case_2_10_1 "Perform streamio I_GETSIG."
#define sref_case_2_10_1 "(none)"
#define desc_case_2_10_1 "\
Checks that I_GETSIG can be performed on a stream."

int
test_case_2_10_1(int child)
{
	int sigs = 0;

	if (test_ioctl(child, I_GETSIG, (intptr_t) & sigs) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_1 = { &preamble_0, &test_case_2_10_1, &postamble_0 };

#define test_case_2_10_1_stream_0 (&test_2_10_1)
#define test_case_2_10_1_stream_1 (NULL)
#define test_case_2_10_1_stream_2 (NULL)

#define tgrp_case_2_10_2 test_group_2
#define numb_case_2_10_2 "2.10.2"
#define name_case_2_10_2 "Perform streamio I_GETSIG."
#define sref_case_2_10_2 "(none)"
#define desc_case_2_10_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_10_2(int child)
{
	if (test_ioctl(child, I_GETSIG, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_10_2 = { &preamble_0, &test_case_2_10_2, &postamble_0 };

#define test_case_2_10_2_stream_0 (&test_2_10_2)
#define test_case_2_10_2_stream_1 (NULL)
#define test_case_2_10_2_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FIND
 */
#define tgrp_case_2_11 test_group_2
#define numb_case_2_11 "2.11"
#define name_case_2_11 "Perform streamio I_FIND."
#define sref_case_2_11 "(none)"
#define desc_case_2_11 "\
Checks that I_FIND can be performed on a stream."

int
test_case_2_11(int child)
{
	char bogus[] = "bogus";

	if (test_ioctl(child, I_FIND, (intptr_t) bogus) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_11 = { &preamble_0, &test_case_2_11, &postamble_0 };

#define test_case_2_11_stream_0 (&test_2_11)
#define test_case_2_11_stream_1 (NULL)
#define test_case_2_11_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_LINK
 */
#define tgrp_case_2_12 test_group_2
#define numb_case_2_12 "2.12"
#define name_case_2_12 "Perform streamio I_LINK."
#define sref_case_2_12 "(none)"
#define desc_case_2_12 "\
Checks that I_LINK can be performed on a stream."

int
test_case_2_12(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_12 = { &preamble_0, &test_case_2_12, &postamble_0 };

#define test_case_2_12_stream_0 (&test_2_12)
#define test_case_2_12_stream_1 (NULL)
#define test_case_2_12_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_UNLINK
 */
#define tgrp_case_2_13 test_group_2
#define numb_case_2_13 "2.13"
#define name_case_2_13 "Perform streamio I_UNLINK."
#define sref_case_2_13 "(none)"
#define desc_case_2_13 "\
Checks that I_UNLINK can be performed on a stream."

int
test_case_2_13(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_13 = { &preamble_0, &test_case_2_13, &postamble_0 };

#define test_case_2_13_stream_0 (&test_2_13)
#define test_case_2_13_stream_1 (NULL)
#define test_case_2_13_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_RECVFD
 */
#define tgrp_case_2_14 test_group_2
#define numb_case_2_14 "2.14"
#define name_case_2_14 "Perform streamio I_RECVFD."
#define sref_case_2_14 "(none)"
#define desc_case_2_14 "\
Checks that I_RECVFD can be performed on a stream."

int
test_case_2_14(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_14 = { &preamble_0, &test_case_2_14, &postamble_0 };

#define test_case_2_14_stream_0 (&test_2_14)
#define test_case_2_14_stream_1 (NULL)
#define test_case_2_14_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PEEK
 */
#define tgrp_case_2_15 test_group_2
#define numb_case_2_15 "2.15"
#define name_case_2_15 "Perform streamio I_PEEK."
#define sref_case_2_15 "(none)"
#define desc_case_2_15 "\
Checks that I_PEEK can be performed on a stream."

int
test_case_2_15(int child)
{
	struct strpeek peek = { {0, 0, NULL}, {0, 0, NULL}, 0 };

	if (test_ioctl(child, I_PEEK, (intptr_t) & peek) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_15 = { &preamble_0, &test_case_2_15, &postamble_0 };

#define test_case_2_15_stream_0 (&test_2_15)
#define test_case_2_15_stream_1 (NULL)
#define test_case_2_15_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FDINSERT
 */
#define tgrp_case_2_16 test_group_2
#define numb_case_2_16 "2.16"
#define name_case_2_16 "Perform streamio I_FDINSERT."
#define sref_case_2_16 "(none)"
#define desc_case_2_16 "\
Checks that I_FDINSERT can be performed on a stream."

int
test_case_2_16(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_16 = { &preamble_0, &test_case_2_16, &postamble_0 };

#define test_case_2_16_stream_0 (&test_2_16)
#define test_case_2_16_stream_1 (NULL)
#define test_case_2_16_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SENDFD
 */
#define tgrp_case_2_17 test_group_2
#define numb_case_2_17 "2.17"
#define name_case_2_17 "Perform streamio I_SENDFD."
#define sref_case_2_17 "(none)"
#define desc_case_2_17 "\
Checks that I_SENDFD can be performed on a stream."

int
test_case_2_17(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_17 = { &preamble_0, &test_case_2_17, &postamble_0 };

#define test_case_2_17_stream_0 (&test_2_17)
#define test_case_2_17_stream_1 (NULL)
#define test_case_2_17_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_E_RECVFD
 */
#define tgrp_case_2_18 test_group_2
#define numb_case_2_18 "2.18"
#define name_case_2_18 "Perform streamio I_E_RECVFD."
#define sref_case_2_18 "(none)"
#define desc_case_2_18 "\
Checks that I_E_RECVFD can be performed on a stream."

int
test_case_2_18(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_18 = { &preamble_0, &test_case_2_18, &postamble_0 };

#define test_case_2_18_stream_0 (&test_2_18)
#define test_case_2_18_stream_1 (NULL)
#define test_case_2_18_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SWROPT
 */
#define tgrp_case_2_19_1 test_group_2
#define numb_case_2_19_1 "2.19.1"
#define name_case_2_19_1 "Perform streamio I_SWROPT - default."
#define sref_case_2_19_1 "(none)"
#define desc_case_2_19_1 "\
Checks that I_SWROPT can be performed on a stream."

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
#define sref_case_2_19_2 "(none)"
#define desc_case_2_19_2 "\
Checks that I_SWROPT can be performed on a stream with write option values\n\
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
#define sref_case_2_19_3 "(none)"
#define desc_case_2_19_3 "\
Checks that I_SWROPT can be performed on a stream with write option values\n\
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

#ifdef SNDHOLD
#define tgrp_case_2_19_4 test_group_2
#define numb_case_2_19_4 "2.19.4"
#define name_case_2_19_4 "Perform streamio I_SWROPT - SNDHOLD."
#define sref_case_2_19_4 "(none)"
#define desc_case_2_19_4 "\
Checks that I_SWROPT can be performed on a stream with write option values\n\
(SNDHOLD)."

int
test_case_2_19_4(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_4 = { &preamble_0, &test_case_2_19_4, &postamble_0 };

#define test_case_2_19_4_stream_0 (&test_2_19_4)
#define test_case_2_19_4_stream_1 (NULL)
#define test_case_2_19_4_stream_2 (NULL)
#endif

#define tgrp_case_2_19_5 test_group_2
#define numb_case_2_19_5 "2.19.5"
#define name_case_2_19_5 "Perform streamio I_SWROPT - SNDZERO | SNDPIPE."
#define sref_case_2_19_5 "(none)"
#define desc_case_2_19_5 "\
Checks that I_SWROPT can be performed on a stream with write option values\n\
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

#ifdef SNDHOLD
#define tgrp_case_2_19_6 test_group_2
#define numb_case_2_19_6 "2.19.6"
#define name_case_2_19_6 "Perform streamio I_SWROPT - SNDZERO | SNDHOLD."
#define sref_case_2_19_6 "(none)"
#define desc_case_2_19_6 "\
Checks that I_SWROPT can be performed on a stream with write option values\n\
(SNDZERO | SNDHOLD)."

int
test_case_2_19_6(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_6 = { &preamble_0, &test_case_2_19_6, &postamble_0 };

#define test_case_2_19_6_stream_0 (&test_2_19_6)
#define test_case_2_19_6_stream_1 (NULL)
#define test_case_2_19_6_stream_2 (NULL)

#define tgrp_case_2_19_7 test_group_2
#define numb_case_2_19_7 "2.19.7"
#define name_case_2_19_7 "Perform streamio I_SWROPT - SNDPIPE | SNDHOLD."
#define sref_case_2_19_7 "(none)"
#define desc_case_2_19_7 "\
Checks that I_SWROPT can be performed on a stream with write option values\n\
(SNDPIPE | SNDHOLD)."

int
test_case_2_19_7(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDPIPE | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_7 = { &preamble_0, &test_case_2_19_7, &postamble_0 };

#define test_case_2_19_7_stream_0 (&test_2_19_7)
#define test_case_2_19_7_stream_1 (NULL)
#define test_case_2_19_7_stream_2 (NULL)

#define tgrp_case_2_19_8 test_group_2
#define numb_case_2_19_8 "2.19.8"
#define name_case_2_19_8 "Perform streamio I_SWROPT - SNDZERO | SNDPIPE | SNDHOLD."
#define sref_case_2_19_8 "(none)"
#define desc_case_2_19_8 "\
Checks that I_SWROPT can be performed on a stream with write option values\n\
(SNDZERO | SNDPIPE | SNDHOLD)."

int
test_case_2_19_8(int child)
{
	if (test_ioctl(child, I_SWROPT, (SNDZERO | SNDPIPE | SNDHOLD)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_8 = { &preamble_0, &test_case_2_19_8, &postamble_0 };

#define test_case_2_19_8_stream_0 (&test_2_19_8)
#define test_case_2_19_8_stream_1 (NULL)
#define test_case_2_19_8_stream_2 (NULL)
#endif

#define tgrp_case_2_19_9 test_group_2
#define numb_case_2_19_9 "2.19.9"
#define name_case_2_19_9 "Perform streamio I_SWROPT - EINVAL."
#define sref_case_2_19_9 "(none)"
#define desc_case_2_19_9 "\
Checks that I_SWROPT can be performed on a stream with an invalid argument\n\
value, resulting in the return of EINVAL."

int
test_case_2_19_9(int child)
{
	if (test_ioctl(child, I_SWROPT, -1UL) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_19_9 = { &preamble_0, &test_case_2_19_9, &postamble_0 };

#define test_case_2_19_9_stream_0 (&test_2_19_9)
#define test_case_2_19_9_stream_1 (NULL)
#define test_case_2_19_9_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GWROPT
 */
#define tgrp_case_2_20_1 test_group_2
#define numb_case_2_20_1 "2.20.1"
#define name_case_2_20_1 "Perform streamio I_GWROPT."
#define sref_case_2_20_1 "(none)"
#define desc_case_2_20_1 "\
Checks that I_GWROPT can be performed on a stream to read the stream default\n\
options after they have been set with I_SWROPT."

int
test_case_2_20_1(int child)
{
	int wropts = -1;

	if (test_ioctl(child, I_GWROPT, (intptr_t) & wropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (wropts != 0)
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
#define sref_case_2_20_2 "(none)"
#define desc_case_2_20_2 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
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
#define sref_case_2_20_3 "(none)"
#define desc_case_2_20_3 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
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
#define sref_case_2_20_4 "(none)"
#define desc_case_2_20_4 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
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

#ifdef SNDHOLD
#define tgrp_case_2_20_5 test_group_2
#define numb_case_2_20_5 "2.20.5"
#define name_case_2_20_5 "Perform streamio I_GWROPT - SNDHOLD."
#define sref_case_2_20_5 "(none)"
#define desc_case_2_20_5 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
(SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_5(int child)
{
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
}
struct test_stream test_2_20_5 = { &preamble_0, &test_case_2_20_5, &postamble_0 };

#define test_case_2_20_5_stream_0 (&test_2_20_5)
#define test_case_2_20_5_stream_1 (NULL)
#define test_case_2_20_5_stream_2 (NULL)
#endif

#define tgrp_case_2_20_6 test_group_2
#define numb_case_2_20_6 "2.20.6"
#define name_case_2_20_6 "Perform streamio I_GWROPT - SNDZERO | SNDPIPE."
#define sref_case_2_20_6 "(none)"
#define desc_case_2_20_6 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
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

#ifdef SNDHOLD
#define tgrp_case_2_20_7 test_group_2
#define numb_case_2_20_7 "2.20.7"
#define name_case_2_20_7 "Perform streamio I_GWROPT - SNDZERO | SNDHOLD."
#define sref_case_2_20_7 "(none)"
#define desc_case_2_20_7 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
(SNDZERO | SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_7(int child)
{
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
}
struct test_stream test_2_20_7 = { &preamble_0, &test_case_2_20_7, &postamble_0 };

#define test_case_2_20_7_stream_0 (&test_2_20_7)
#define test_case_2_20_7_stream_1 (NULL)
#define test_case_2_20_7_stream_2 (NULL)

#define tgrp_case_2_20_8 test_group_2
#define numb_case_2_20_8 "2.20.8"
#define name_case_2_20_8 "Perform streamio I_GWROPT - SNDPIPE | SNDHOLD."
#define sref_case_2_20_8 "(none)"
#define desc_case_2_20_8 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
(SNDPIPE | SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_8(int child)
{
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
}
struct test_stream test_2_20_8 = { &preamble_0, &test_case_2_20_8, &postamble_0 };

#define test_case_2_20_8_stream_0 (&test_2_20_8)
#define test_case_2_20_8_stream_1 (NULL)
#define test_case_2_20_8_stream_2 (NULL)

#define tgrp_case_2_20_9 test_group_2
#define numb_case_2_20_9 "2.20.9"
#define name_case_2_20_9 "Perform streamio I_GWROPT - SNDZERO | SNDPIPE | SNDHOLD."
#define sref_case_2_20_9 "(none)"
#define desc_case_2_20_9 "\
Checks that I_GWROPT can be performed on a stream to read the write options\n\
(SNDZERO | SNDPIPE | SNDHOLD) after they have been set with I_SWROPT."

int
test_case_2_20_9(int child)
{
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
}
struct test_stream test_2_20_9 = { &preamble_0, &test_case_2_20_9, &postamble_0 };

#define test_case_2_20_9_stream_0 (&test_2_20_9)
#define test_case_2_20_9_stream_1 (NULL)
#define test_case_2_20_9_stream_2 (NULL)
#endif

#define tgrp_case_2_20_10 test_group_2
#define numb_case_2_20_10 "2.20.10"
#define name_case_2_20_10 "Perform streamio I_GWROPT - EFAULT."
#define sref_case_2_20_10 "(none)"
#define desc_case_2_20_10 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_20_10(int child)
{
	if (test_ioctl(child, I_GWROPT, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_20_10 = { &preamble_0, &test_case_2_20_10, &postamble_0 };

#define test_case_2_20_10_stream_0 (&test_2_20_10)
#define test_case_2_20_10_stream_1 (NULL)
#define test_case_2_20_10_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_LIST
 */
#define tgrp_case_2_21 test_group_2
#define numb_case_2_21 "2.21"
#define name_case_2_21 "Perform streamio I_LIST."
#define sref_case_2_21 "(none)"
#define desc_case_2_21 "\
Checks that I_LIST can be performed on a stream."

int
test_case_2_21(int child)
{
	if (test_ioctl(child, I_LIST, 0) != __RESULT_SUCCESS || (last_retval != 0 && last_retval != 1))
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_21 = { &preamble_0, &test_case_2_21, &postamble_0 };

#define test_case_2_21_stream_0 (&test_2_21)
#define test_case_2_21_stream_1 (NULL)
#define test_case_2_21_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PUNLINK
 */
#define tgrp_case_2_22 test_group_2
#define numb_case_2_22 "2.22"
#define name_case_2_22 "Perform streamio I_PUNLINK."
#define sref_case_2_22 "(none)"
#define desc_case_2_22 "\
Checks that I_PUNLINK can be performed on a stream."

int
test_case_2_22(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_22 = { &preamble_0, &test_case_2_22, &postamble_0 };

#define test_case_2_22_stream_0 (&test_2_22)
#define test_case_2_22_stream_1 (NULL)
#define test_case_2_22_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FLUSHBAND
 */
#define tgrp_case_2_23 test_group_2
#define numb_case_2_23 "2.23"
#define name_case_2_23 "Perform streamio I_FLUSHBAND."
#define sref_case_2_23 "(none)"
#define desc_case_2_23 "\
Checks that I_FLUSHBAND can be performed on a stream."

int
test_case_2_23(int child)
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
struct test_stream test_2_23 = { &preamble_0, &test_case_2_23, &postamble_0 };

#define test_case_2_23_stream_0 (&test_2_23)
#define test_case_2_23_stream_1 (NULL)
#define test_case_2_23_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_CKBAND
 */
#define tgrp_case_2_24 test_group_2
#define numb_case_2_24 "2.24"
#define name_case_2_24 "Perform streamio I_CKBAND."
#define sref_case_2_24 "(none)"
#define desc_case_2_24 "\
Checks that I_CKBAND can be performed on a stream."

int
test_case_2_24(int child)
{
	if (test_ioctl(child, I_CKBAND, 2) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_24 = { &preamble_0, &test_case_2_24, &postamble_0 };

#define test_case_2_24_stream_0 (&test_2_24)
#define test_case_2_24_stream_1 (NULL)
#define test_case_2_24_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETBAND
 */
#define tgrp_case_2_25 test_group_2
#define numb_case_2_25 "2.25"
#define name_case_2_25 "Perform streamio I_GETBAND."
#define sref_case_2_25 "(none)"
#define desc_case_2_25 "\
Checks that I_GETBAND can be performed on a stream."

int
test_case_2_25(int child)
{
	int band = 0;

	if (test_ioctl(child, I_GETBAND, (intptr_t) & band) == __RESULT_SUCCESS || last_errno != ENODATA)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_25 = { &preamble_0, &test_case_2_25, &postamble_0 };

#define test_case_2_25_stream_0 (&test_2_25)
#define test_case_2_25_stream_1 (NULL)
#define test_case_2_25_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_ATMARK
 */
#define tgrp_case_2_26 test_group_2
#define numb_case_2_26 "2.26"
#define name_case_2_26 "Perform streamio I_ATMARK."
#define sref_case_2_26 "(none)"
#define desc_case_2_26 "\
Checks that I_ATMARK can be performed on a stream."

int
test_case_2_26(int child)
{
	if (test_ioctl(child, I_ATMARK, ANYMARK) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_26 = { &preamble_0, &test_case_2_26, &postamble_0 };

#define test_case_2_26_stream_0 (&test_2_26)
#define test_case_2_26_stream_1 (NULL)
#define test_case_2_26_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SETCLTIME
 */
#define tgrp_case_2_27_1 test_group_2
#define numb_case_2_27_1 "2.27.1"
#define name_case_2_27_1 "Perform streamio I_SETCLTIME."
#define sref_case_2_27_1 "(none)"
#define desc_case_2_27_1 "\
Checks that I_SETCLTIME can be performed on a stream.\n\
Checks that the close time can be set to zero."

int
test_case_2_27_1(int child)
{
	int cltime = 0;

	if (test_ioctl(child, I_SETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_1 = { &preamble_0, &test_case_2_27_1, &postamble_0 };

#define test_case_2_27_1_stream_0 (&test_2_27_1)
#define test_case_2_27_1_stream_1 (NULL)
#define test_case_2_27_1_stream_2 (NULL)

#define tgrp_case_2_27_2 test_group_2
#define numb_case_2_27_2 "2.27.2"
#define name_case_2_27_2 "Perform streamio I_SETCLTIME."
#define sref_case_2_27_2 "(none)"
#define desc_case_2_27_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_27_2(int child)
{
	if (test_ioctl(child, I_SETCLTIME, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_27_2 = { &preamble_0, &test_case_2_27_2, &postamble_0 };

#define test_case_2_27_2_stream_0 (&test_2_27_2)
#define test_case_2_27_2_stream_1 (NULL)
#define test_case_2_27_2_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETCLTIME
 */
#define tgrp_case_2_28_1 test_group_2
#define numb_case_2_28_1 "2.28.1"
#define name_case_2_28_1 "Perform streamio I_GETCLTIME - default."
#define sref_case_2_28_1 "(none)"
#define desc_case_2_28_1 "\
Checks that I_GETCLTIME can be performed on a stream.\n\
Checks that the default close time is 15000 milliseconds (or 15 seconds)."

int
test_case_2_28_1(int child)
{
	int cltime;

	if (test_ioctl(child, I_GETCLTIME, (intptr_t) & cltime) != __RESULT_SUCCESS || cltime != 15000)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_1 = { &preamble_0, &test_case_2_28_1, &postamble_0 };

#define test_case_2_28_1_stream_0 (&test_2_28_1)
#define test_case_2_28_1_stream_1 (NULL)
#define test_case_2_28_1_stream_2 (NULL)

#define tgrp_case_2_28_2 test_group_2
#define numb_case_2_28_2 "2.28.2"
#define name_case_2_28_2 "Perform streamio I_GETCLTIME - EFAULT."
#define sref_case_2_28_2 "(none)"
#define desc_case_2_28_2 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_28_2(int child)
{
	if (test_ioctl(child, I_GETCLTIME, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_28_2 = { &preamble_0, &test_case_2_28_2, &postamble_0 };

#define test_case_2_28_2_stream_0 (&test_2_28_2)
#define test_case_2_28_2_stream_1 (NULL)
#define test_case_2_28_2_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_CANPUT
 */
#define tgrp_case_2_29_1 test_group_2
#define numb_case_2_29_1 "2.29.1"
#define name_case_2_29_1 "Perform streamio I_CANPUT."
#define sref_case_2_29_1 "(none)"
#define desc_case_2_29_1 "\
Checks that I_CANPUT can be performed on a stream for band 0."

int
test_case_2_29_1(int child)
{
	if (test_ioctl(child, I_CANPUT, 0) != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_1 = { &preamble_0, &test_case_2_29_1, &postamble_0 };

#define test_case_2_29_1_stream_0 (&test_2_29_1)
#define test_case_2_29_1_stream_1 (NULL)
#define test_case_2_29_1_stream_2 (NULL)

#define tgrp_case_2_29_2 test_group_2
#define numb_case_2_29_2 "2.29.2"
#define name_case_2_29_2 "Perform streamio I_CANPUT."
#define sref_case_2_29_2 "(none)"
#define desc_case_2_29_2 "\
Checks that I_CANPUT can be performed on a stream for band 2."

int
test_case_2_29_2(int child)
{
	if (test_ioctl(child, I_CANPUT, 2) != __RESULT_SUCCESS || last_retval != 1)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_2 = { &preamble_0, &test_case_2_29_2, &postamble_0 };

#define test_case_2_29_2_stream_0 (&test_2_29_2)
#define test_case_2_29_2_stream_1 (NULL)
#define test_case_2_29_2_stream_2 (NULL)

#define tgrp_case_2_29_3 test_group_2
#define numb_case_2_29_3 "2.29.3"
#define name_case_2_29_3 "Perform streamio I_CANPUT."
#define sref_case_2_29_3 "(none)"
#define desc_case_2_29_3 "\
Checks that I_CANPUT performed on a stream for an illegal band (256) will result\n\
in an EINVAL error."

int
test_case_2_29_3(int child)
{
	if (test_ioctl(child, I_CANPUT, 256) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_3 = { &preamble_0, &test_case_2_29_3, &postamble_0 };

#define test_case_2_29_3_stream_0 (&test_2_29_2)
#define test_case_2_29_3_stream_1 (NULL)
#define test_case_2_29_3_stream_2 (NULL)

#define tgrp_case_2_29_4 test_group_2
#define numb_case_2_29_4 "2.29.4"
#define name_case_2_29_4 "Perform streamio I_CANPUT."
#define sref_case_2_29_4 "(none)"
#define desc_case_2_29_4 "\
Checks that I_CANPUT can be performed on a stream for the special band ANYBAND.\n\
Because there is not any writable non-zero band (no non-zero band exists on a \n\
newly opened stream), the return value should be zero (0)."

int
test_case_2_29_4(int child)
{
	if (test_ioctl(child, I_CANPUT, (-1UL)) != __RESULT_SUCCESS || last_retval != 0)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_29_4 = { &preamble_0, &test_case_2_29_4, &postamble_0 };

#define test_case_2_29_4_stream_0 (&test_2_29_4)
#define test_case_2_29_4_stream_1 (NULL)
#define test_case_2_29_4_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_SERROPT	(Solaris)
 */
#define tgrp_case_2_30_1 test_group_2
#define numb_case_2_30_1 "2.30.1"
#define name_case_2_30_1 "Perform streamio I_SERROPT - default."
#define sref_case_2_30_1 "(none)"
#define desc_case_2_30_1 "\
Checks that I_SERROPT can be performed on a stream with error option values\n\
(RERRNORM | WRERRNORM)."

int
test_case_2_30_1(int child)
{
	if (test_ioctl(child, I_SERROPT, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_1 = { &preamble_0, &test_case_2_30_1, &postamble_0 };

#define test_case_2_30_1_stream_0 (&test_2_30_1)
#define test_case_2_30_1_stream_1 (NULL)
#define test_case_2_30_1_stream_2 (NULL)

#ifdef RERRNONPERSIST
#define tgrp_case_2_30_2 test_group_2
#define numb_case_2_30_2 "2.30.2"
#define name_case_2_30_2 "Perform streamio I_SERROPT - RERRNONPERSIST."
#define sref_case_2_30_2 "(none)"
#define desc_case_2_30_2 "\
Checks that I_SERROPT can be performed on a stream with error options values\n\
(RERRNONPERSIST | WRERRNORM)."

int
test_case_2_30_2(int child)
{
	if (test_ioctl(child, I_SERROPT, RERRNONPERSIST) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_2 = { &preamble_0, &test_case_2_30_2, &postamble_0 };

#define test_case_2_30_2_stream_0 (&test_2_30_2)
#define test_case_2_30_2_stream_1 (NULL)
#define test_case_2_30_2_stream_2 (NULL)
#endif

#ifdef WERRNONPERSIST
#define tgrp_case_2_30_3 test_group_2
#define numb_case_2_30_3 "2.30.3"
#define name_case_2_30_3 "Perform streamio I_SERROPT - WERRNONPERSIST."
#define sref_case_2_30_3 "(none)"
#define desc_case_2_30_3 "\
Checks that I_SERROPT can be performed on a stream with error options values\n\
(RERRNORM | WERRNONPERSIST)."

int
test_case_2_30_3(int child)
{
	if (test_ioctl(child, I_SERROPT, WERRNONPERSIST) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_3 = { &preamble_0, &test_case_2_30_3, &postamble_0 };

#define test_case_2_30_3_stream_0 (&test_2_30_3)
#define test_case_2_30_3_stream_1 (NULL)
#define test_case_2_30_3_stream_2 (NULL)
#endif

#ifdef RERRNONPERSIST
#define tgrp_case_2_30_4 test_group_2
#define numb_case_2_30_4 "2.30.4"
#define name_case_2_30_4 "Perform streamio I_SERROPT - RERRNONPERSIST | WERRNONPERSIST."
#define sref_case_2_30_4 "(none)"
#define desc_case_2_30_4 "\
Checks that I_SERROPT can be performed on a stream with error options values\n\
(RERRNONPERSIST | WERRNONPERSIST)."

int
test_case_2_30_4(int child)
{
	if (test_ioctl(child, I_SERROPT, (RERRNONPERSIST | WERRNONPERSIST)) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_4 = { &preamble_0, &test_case_2_30_4, &postamble_0 };

#define test_case_2_30_4_stream_0 (&test_2_30_4)
#define test_case_2_30_4_stream_1 (NULL)
#define test_case_2_30_4_stream_2 (NULL)
#endif

#define tgrp_case_2_30_5 test_group_2
#define numb_case_2_30_5 "2.30.5"
#define name_case_2_30_5 "Perform streamio I_SERROPT - EINVAL."
#define sref_case_2_30_5 "(none)"
#define desc_case_2_30_5 "\
Checks that I_SERROPT can be performed on a stream with an invalid argument\n\
value, resulting in the return of EINVAL."

int
test_case_2_30_5(int child)
{
	if (test_ioctl(child, I_SERROPT, -1UL) == __RESULT_SUCCESS || last_errno != EINVAL)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_30_5 = { &preamble_0, &test_case_2_30_5, &postamble_0 };

#define test_case_2_30_5_stream_0 (&test_2_30_5)
#define test_case_2_30_5_stream_1 (NULL)
#define test_case_2_30_5_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GERROPT	(Solaris)
 */
#if defined RERRNORM && defined WERRNORM
#define tgrp_case_2_31_1 test_group_2
#define numb_case_2_31_1 "2.31.1"
#define name_case_2_31_1 "Perform streamio I_GERROPT - default."
#define sref_case_2_31_1 "(none)"
#define desc_case_2_31_1 "\
Checks that I_GERROPT can be performed on a stream to read the stream default\n\
error options."

int
test_case_2_31_1(int child)
{
	int erropts = -1;

	if (test_ioctl(child, I_GERROPT, (intptr_t) & erropts) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (erropts != (RERRNORM | WERRNORM))
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
#define name_case_2_31_2 "Perform streamio I_GERROPT - set to default."
#define sref_case_2_31_2 "(none)"
#define desc_case_2_31_2 "\
Checks that I_GERROPT can be performed on a stream to read the errror options\n\
(RERRNORM | WERRNORM) after they have been set with I_SERROPT."

int
test_case_2_31_2(int child)
{
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
}
struct test_stream test_2_31_2 = { &preamble_0, &test_case_2_31_2, &postamble_0 };

#define test_case_2_31_2_stream_0 (&test_2_31_2)
#define test_case_2_31_2_stream_1 (NULL)
#define test_case_2_31_2_stream_2 (NULL)
#endif

#if defined RERRNONPERSIST && defined WERRNORM
#define tgrp_case_2_31_3 test_group_2
#define numb_case_2_31_3 "2.31.3"
#define name_case_2_31_3 "Perform streamio I_GERROPT - RERRNONPERSIST."
#define sref_case_2_31_3 "(none)"
#define desc_case_2_31_3 "\
Checks that I_GERROPT can be performed on a stream to read the errror options\n\
(RERRNONPERSIST | WERRNORM) after they have been set with I_SERROPT."

int
test_case_2_31_3(int child)
{
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
}
struct test_stream test_2_31_3 = { &preamble_0, &test_case_2_31_3, &postamble_0 };

#define test_case_2_31_3_stream_0 (&test_2_31_3)
#define test_case_2_31_3_stream_1 (NULL)
#define test_case_2_31_3_stream_2 (NULL)
#endif

#ifdef WERRNONPERSIST
#define tgrp_case_2_31_4 test_group_2
#define numb_case_2_31_4 "2.31.4"
#define name_case_2_31_4 "Perform streamio I_GERROPT - WERRNONPERSIST."
#define sref_case_2_31_4 "(none)"
#define desc_case_2_31_4 "\
Checks that I_GERROPT can be performed on a stream to read the errror options\n\
(RERRNORM | WERRNONPERSIST) after they have been set with I_SERROPT."

int
test_case_2_31_4(int child)
{
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
}
struct test_stream test_2_31_4 = { &preamble_0, &test_case_2_31_4, &postamble_0 };

#define test_case_2_31_4_stream_0 (&test_2_31_4)
#define test_case_2_31_4_stream_1 (NULL)
#define test_case_2_31_4_stream_2 (NULL)
#endif

#ifdef RERRNONPERSIST
#define tgrp_case_2_31_5 test_group_2
#define numb_case_2_31_5 "2.31.5"
#define name_case_2_31_5 "Perform streamio I_GERROPT - RERRNONPERSIST | WERRNONPERSIST."
#define sref_case_2_31_5 "(none)"
#define desc_case_2_31_5 "\
Checks that I_GERROPT can be performed on a stream to read the errror options\n\
(RERRNONPERSIST | WERRNONPERSIST) after they have been set with I_SERROPT."

int
test_case_2_31_5(int child)
{
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
}
struct test_stream test_2_31_5 = { &preamble_0, &test_case_2_31_5, &postamble_0 };

#define test_case_2_31_5_stream_0 (&test_2_31_5)
#define test_case_2_31_5_stream_1 (NULL)
#define test_case_2_31_5_stream_2 (NULL)
#endif

#define tgrp_case_2_31_6 test_group_2
#define numb_case_2_31_6 "2.31.6"
#define name_case_2_31_6 "Perform streamio I_GERROPT - EFAULT."
#define sref_case_2_31_6 "(none)"
#define desc_case_2_31_6 "\
Checks that EFAULT is returned when arg points outside the caller's address \n\
space."

int
test_case_2_31_6(int child)
{
	if (test_ioctl(child, I_GERROPT, (intptr_t) NULL) == __RESULT_SUCCESS || last_errno != EFAULT)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_31_6 = { &preamble_0, &test_case_2_31_6, &postamble_0 };

#define test_case_2_31_6_stream_0 (&test_2_31_6)
#define test_case_2_31_6_stream_1 (NULL)
#define test_case_2_31_6_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_ANCHOR	(Solaris)
 */
#define tgrp_case_2_32 test_group_2
#define numb_case_2_32 "2.32"
#define name_case_2_32 "Perform streamio I_ANCHOR."
#define sref_case_2_32 "(none)"
#define desc_case_2_32 "\
Checks that I_ANCHOR can be performed on a stream."

int
test_case_2_32(int child)
{
	if (test_ioctl(child, I_ANCHOR, 0) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_32 = { &preamble_0, &test_case_2_32, &postamble_0 };

#define test_case_2_32_stream_0 (&test_2_32)
#define test_case_2_32_stream_1 (NULL)
#define test_case_2_32_stream_2 (NULL)

#if 0
/*
 *  Perform IOCTL on one stream - I_S_RECVFD	(UnixWare)
 */
#define tgrp_case_2_33 test_group_2
#define numb_case_2_33 "2.33"
#define name_case_2_33 "Perform streamio I_S_RECVFD."
#define sref_case_2_33 "(none)"
#define desc_case_2_33 "\
Checks that I_S_RECVFD can be performed on a stream."

int
test_case_2_33(int child)
{
	struct strrecvfd recvfd;

	if (test_ioctl(child, I_S_RECVFD, (intptr_t) & recvfd) == __RESULT_SUCCESS || last_errno != EAGAIN)
		return (__RESULT_FAILURE);
	return (__RESULT_SUCCESS);
}
struct test_stream test_2_33 = { &preamble_0, &test_case_2_33, &postamble_0 };

#define test_case_2_33_stream_0 (&test_2_33)
#define test_case_2_33_stream_1 (NULL)
#define test_case_2_33_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_STATS	(UnixWare)
 */
#define tgrp_case_2_34 test_group_2
#define numb_case_2_34 "2.34"
#define name_case_2_34 "Perform streamio I_STATS."
#define sref_case_2_34 "(none)"
#define desc_case_2_34 "\
Checks that I_STATS can be performed on a stream."

int
test_case_2_34(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_34 = { &preamble_0, &test_case_2_34, &postamble_0 };

#define test_case_2_34_stream_0 (&test_2_34)
#define test_case_2_34_stream_1 (NULL)
#define test_case_2_34_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_BIGPIPE	(UnixWare)
 */
#define tgrp_case_2_35 test_group_2
#define numb_case_2_35 "2.35"
#define name_case_2_35 "Perform streamio I_BIGPIPE."
#define sref_case_2_35 "(none)"
#define desc_case_2_35 "\
Checks that I_BIGPIPE can be performed on a stream."

int
test_case_2_35(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_35 = { &preamble_0, &test_case_2_35, &postamble_0 };

#define test_case_2_35_stream_0 (&test_2_35)
#define test_case_2_35_stream_1 (NULL)
#define test_case_2_35_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETTP	(UnixWare)
 */
#define tgrp_case_2_36 test_group_2
#define numb_case_2_36 "2.36"
#define name_case_2_36 "Perform streamio I_GETTP."
#define sref_case_2_36 "(none)"
#define desc_case_2_36 "\
Checks that I_GETTP can be performed on a stream."

int
test_case_2_36(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_36 = { &preamble_0, &test_case_2_36, &postamble_0 };

#define test_case_2_36_stream_0 (&test_2_36)
#define test_case_2_36_stream_1 (NULL)
#define test_case_2_36_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_AUTOPUSH	(MacOT)
 */
#define tgrp_case_2_37 test_group_2
#define numb_case_2_37 "2.37"
#define name_case_2_37 "Perform streamio I_AUTOPUSH."
#define sref_case_2_37 "(none)"
#define desc_case_2_37 "\
Checks that I_AUTOPUSH can be performed on a stream."

int
test_case_2_37(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_37 = { &preamble_0, &test_case_2_37, &postamble_0 };

#define test_case_2_37_stream_0 (&test_2_37)
#define test_case_2_37_stream_1 (NULL)
#define test_case_2_37_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_HEAP_REPORT	(MacOT)
 */
#define tgrp_case_2_38 test_group_2
#define numb_case_2_38 "2.38"
#define name_case_2_38 "Perform streamio I_HEAP_REPORT."
#define sref_case_2_38 "(none)"
#define desc_case_2_38 "\
Checks that I_HEAP_REPORT can be performed on a stream."

int
test_case_2_38(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_38 = { &preamble_0, &test_case_2_38, &postamble_0 };

#define test_case_2_38_stream_0 (&test_2_38)
#define test_case_2_38_stream_1 (NULL)
#define test_case_2_38_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FIFO	(MacOT)
 */
#define tgrp_case_2_39 test_group_2
#define numb_case_2_39 "2.39"
#define name_case_2_39 "Perform streamio I_FIFO."
#define sref_case_2_39 "(none)"
#define desc_case_2_39 "\
Checks that I_FIFO can be performed on a stream."

int
test_case_2_39(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_39 = { &preamble_0, &test_case_2_39, &postamble_0 };

#define test_case_2_39_stream_0 (&test_2_39)
#define test_case_2_39_stream_1 (NULL)
#define test_case_2_39_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PUTPMSG	(LiS)
 */
#define tgrp_case_2_40 test_group_2
#define numb_case_2_40 "2.40"
#define name_case_2_40 "Perform streamio I_PUTPMSG."
#define sref_case_2_40 "(none)"
#define desc_case_2_40 "\
Checks that I_PUTPMSG can be performed on a stream."

int
test_case_2_40(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_40 = { &preamble_0, &test_case_2_40, &postamble_0 };

#define test_case_2_40_stream_0 (&test_2_40)
#define test_case_2_40_stream_1 (NULL)
#define test_case_2_40_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_GETPMSG	(LiS)
 */
#define tgrp_case_2_41 test_group_2
#define numb_case_2_41 "2.41"
#define name_case_2_41 "Perform streamio I_GETPMSG."
#define sref_case_2_41 "(none)"
#define desc_case_2_41 "\
Checks that I_GETPMSG can be performed on a stream."

int
test_case_2_41(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_41 = { &preamble_0, &test_case_2_41, &postamble_0 };

#define test_case_2_41_stream_0 (&test_2_41)
#define test_case_2_41_stream_1 (NULL)
#define test_case_2_41_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FATTACH	(LiS)
 */
#define tgrp_case_2_42 test_group_2
#define numb_case_2_42 "2.42"
#define name_case_2_42 "Perform streamio I_FATTACH."
#define sref_case_2_42 "(none)"
#define desc_case_2_42 "\
Checks that I_FATTACH can be performed on a stream."

int
test_case_2_42(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_42 = { &preamble_0, &test_case_2_42, &postamble_0 };

#define test_case_2_42_stream_0 (&test_2_42)
#define test_case_2_42_stream_1 (NULL)
#define test_case_2_42_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_FDETACH	(LiS)
 */
#define tgrp_case_2_43 test_group_2
#define numb_case_2_43 "2.43"
#define name_case_2_43 "Perform streamio I_FDETACH."
#define sref_case_2_43 "(none)"
#define desc_case_2_43 "\
Checks that I_FDETACH can be performed on a stream."

int
test_case_2_43(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_43 = { &preamble_0, &test_case_2_43, &postamble_0 };

#define test_case_2_43_stream_0 (&test_2_43)
#define test_case_2_43_stream_1 (NULL)
#define test_case_2_43_stream_2 (NULL)

/*
 *  Perform IOCTL on one stream - I_PIPE	(LiS)
 */
#define tgrp_case_2_44 test_group_2
#define numb_case_2_44 "2.44"
#define name_case_2_44 "Perform streamio I_PIPE."
#define sref_case_2_44 "(none)"
#define desc_case_2_44 "\
Checks that I_PIPE can be performed on a stream."

int
test_case_2_44(int child)
{
	return __RESULT_INCONCLUSIVE;
}
struct test_stream test_2_44 = { &preamble_0, &test_case_2_44, &postamble_0 };

#define test_case_2_44_stream_0 (&test_2_44)
#define test_case_2_44_stream_1 (NULL)
#define test_case_2_44_stream_2 (NULL)
#endif

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
	pid_t this_child, child[3] = { 0, };
	int this_status, status[3] = { 0, };

	if (start_tt(TEST_DURATION) != __RESULT_SUCCESS)
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
		numb_case_2_1_1, tgrp_case_2_1_1, name_case_2_1_1, desc_case_2_1_1, sref_case_2_1_1, {
	test_case_2_1_1_stream_0, test_case_2_1_1_stream_1, test_case_2_1_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_1_2, tgrp_case_2_1_2, name_case_2_1_2, desc_case_2_1_2, sref_case_2_1_2, {
	test_case_2_1_2_stream_0, test_case_2_1_2_stream_1, test_case_2_1_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_1, tgrp_case_2_2_1, name_case_2_2_1, desc_case_2_2_1, sref_case_2_2_1, {
	test_case_2_2_1_stream_0, test_case_2_2_1_stream_1, test_case_2_2_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_2, tgrp_case_2_2_2, name_case_2_2_2, desc_case_2_2_2, sref_case_2_2_2, {
	test_case_2_2_2_stream_0, test_case_2_2_2_stream_1, test_case_2_2_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_2_3, tgrp_case_2_2_3, name_case_2_2_3, desc_case_2_2_3, sref_case_2_2_3, {
	test_case_2_2_3_stream_0, test_case_2_2_3_stream_1, test_case_2_2_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_3, tgrp_case_2_3, name_case_2_3, desc_case_2_3, sref_case_2_3, {
	test_case_2_3_stream_0, test_case_2_3_stream_1, test_case_2_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_4, tgrp_case_2_4, name_case_2_4, desc_case_2_4, sref_case_2_4, {
	test_case_2_4_stream_0, test_case_2_4_stream_1, test_case_2_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_5, tgrp_case_2_5, name_case_2_5, desc_case_2_5, sref_case_2_5, {
	test_case_2_5_stream_0, test_case_2_5_stream_1, test_case_2_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_1, tgrp_case_2_6_1, name_case_2_6_1, desc_case_2_6_1, sref_case_2_6_1, {
	test_case_2_6_1_stream_0, test_case_2_6_1_stream_1, test_case_2_6_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_2, tgrp_case_2_6_2, name_case_2_6_2, desc_case_2_6_2, sref_case_2_6_2, {
	test_case_2_6_2_stream_0, test_case_2_6_2_stream_1, test_case_2_6_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_3, tgrp_case_2_6_3, name_case_2_6_3, desc_case_2_6_3, sref_case_2_6_3, {
	test_case_2_6_3_stream_0, test_case_2_6_3_stream_1, test_case_2_6_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_4, tgrp_case_2_6_4, name_case_2_6_4, desc_case_2_6_4, sref_case_2_6_4, {
	test_case_2_6_4_stream_0, test_case_2_6_4_stream_1, test_case_2_6_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_5, tgrp_case_2_6_5, name_case_2_6_5, desc_case_2_6_5, sref_case_2_6_5, {
	test_case_2_6_5_stream_0, test_case_2_6_5_stream_1, test_case_2_6_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_6, tgrp_case_2_6_6, name_case_2_6_6, desc_case_2_6_6, sref_case_2_6_6, {
	test_case_2_6_6_stream_0, test_case_2_6_6_stream_1, test_case_2_6_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_7, tgrp_case_2_6_7, name_case_2_6_7, desc_case_2_6_7, sref_case_2_6_7, {
	test_case_2_6_7_stream_0, test_case_2_6_7_stream_1, test_case_2_6_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_8, tgrp_case_2_6_8, name_case_2_6_8, desc_case_2_6_8, sref_case_2_6_8, {
	test_case_2_6_8_stream_0, test_case_2_6_8_stream_1, test_case_2_6_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_9, tgrp_case_2_6_9, name_case_2_6_9, desc_case_2_6_9, sref_case_2_6_9, {
	test_case_2_6_9_stream_0, test_case_2_6_9_stream_1, test_case_2_6_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_10, tgrp_case_2_6_10, name_case_2_6_10, desc_case_2_6_10, sref_case_2_6_10, {
	test_case_2_6_10_stream_0, test_case_2_6_10_stream_1, test_case_2_6_10_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_6_11, tgrp_case_2_6_11, name_case_2_6_11, desc_case_2_6_11, sref_case_2_6_11, {
	test_case_2_6_11_stream_0, test_case_2_6_11_stream_1, test_case_2_6_11_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_1, tgrp_case_2_7_1, name_case_2_7_1, desc_case_2_7_1, sref_case_2_7_1, {
	test_case_2_7_1_stream_0, test_case_2_7_1_stream_1, test_case_2_7_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_2, tgrp_case_2_7_2, name_case_2_7_2, desc_case_2_7_2, sref_case_2_7_2, {
	test_case_2_7_2_stream_0, test_case_2_7_2_stream_1, test_case_2_7_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_3, tgrp_case_2_7_3, name_case_2_7_3, desc_case_2_7_3, sref_case_2_7_3, {
	test_case_2_7_3_stream_0, test_case_2_7_3_stream_1, test_case_2_7_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_4, tgrp_case_2_7_4, name_case_2_7_4, desc_case_2_7_4, sref_case_2_7_4, {
	test_case_2_7_4_stream_0, test_case_2_7_4_stream_1, test_case_2_7_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_5, tgrp_case_2_7_5, name_case_2_7_5, desc_case_2_7_5, sref_case_2_7_5, {
	test_case_2_7_5_stream_0, test_case_2_7_5_stream_1, test_case_2_7_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_6, tgrp_case_2_7_6, name_case_2_7_6, desc_case_2_7_6, sref_case_2_7_6, {
	test_case_2_7_6_stream_0, test_case_2_7_6_stream_1, test_case_2_7_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_7, tgrp_case_2_7_7, name_case_2_7_7, desc_case_2_7_7, sref_case_2_7_7, {
	test_case_2_7_7_stream_0, test_case_2_7_7_stream_1, test_case_2_7_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_8, tgrp_case_2_7_8, name_case_2_7_8, desc_case_2_7_8, sref_case_2_7_8, {
	test_case_2_7_8_stream_0, test_case_2_7_8_stream_1, test_case_2_7_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_9, tgrp_case_2_7_9, name_case_2_7_9, desc_case_2_7_9, sref_case_2_7_9, {
	test_case_2_7_9_stream_0, test_case_2_7_9_stream_1, test_case_2_7_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_10, tgrp_case_2_7_10, name_case_2_7_10, desc_case_2_7_10, sref_case_2_7_10, {
	test_case_2_7_10_stream_0, test_case_2_7_10_stream_1, test_case_2_7_10_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_7_11, tgrp_case_2_7_11, name_case_2_7_11, desc_case_2_7_11, sref_case_2_7_11, {
	test_case_2_7_11_stream_0, test_case_2_7_11_stream_1, test_case_2_7_11_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_8, tgrp_case_2_8, name_case_2_8, desc_case_2_8, sref_case_2_8, {
	test_case_2_8_stream_0, test_case_2_8_stream_1, test_case_2_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_9, tgrp_case_2_9, name_case_2_9, desc_case_2_9, sref_case_2_9, {
	test_case_2_9_stream_0, test_case_2_9_stream_1, test_case_2_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_10_1, tgrp_case_2_10_1, name_case_2_10_1, desc_case_2_10_1, sref_case_2_10_1, {
	test_case_2_10_1_stream_0, test_case_2_10_1_stream_1, test_case_2_10_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_10_2, tgrp_case_2_10_2, name_case_2_10_2, desc_case_2_10_2, sref_case_2_10_2, {
	test_case_2_10_2_stream_0, test_case_2_10_2_stream_1, test_case_2_10_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_11, tgrp_case_2_11, name_case_2_11, desc_case_2_11, sref_case_2_11, {
	test_case_2_11_stream_0, test_case_2_11_stream_1, test_case_2_11_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_12, tgrp_case_2_12, name_case_2_12, desc_case_2_12, sref_case_2_12, {
	test_case_2_12_stream_0, test_case_2_12_stream_1, test_case_2_12_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_13, tgrp_case_2_13, name_case_2_13, desc_case_2_13, sref_case_2_13, {
	test_case_2_13_stream_0, test_case_2_13_stream_1, test_case_2_13_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_14, tgrp_case_2_14, name_case_2_14, desc_case_2_14, sref_case_2_14, {
	test_case_2_14_stream_0, test_case_2_14_stream_1, test_case_2_14_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_15, tgrp_case_2_15, name_case_2_15, desc_case_2_15, sref_case_2_15, {
	test_case_2_15_stream_0, test_case_2_15_stream_1, test_case_2_15_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_16, tgrp_case_2_16, name_case_2_16, desc_case_2_16, sref_case_2_16, {
	test_case_2_16_stream_0, test_case_2_16_stream_1, test_case_2_16_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_17, tgrp_case_2_17, name_case_2_17, desc_case_2_17, sref_case_2_17, {
	test_case_2_17_stream_0, test_case_2_17_stream_1, test_case_2_17_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_18, tgrp_case_2_18, name_case_2_18, desc_case_2_18, sref_case_2_18, {
	test_case_2_18_stream_0, test_case_2_18_stream_1, test_case_2_18_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_19_1, tgrp_case_2_19_1, name_case_2_19_1, desc_case_2_19_1, sref_case_2_19_1, {
	test_case_2_19_1_stream_0, test_case_2_19_1_stream_1, test_case_2_19_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_19_2, tgrp_case_2_19_2, name_case_2_19_2, desc_case_2_19_2, sref_case_2_19_2, {
	test_case_2_19_2_stream_0, test_case_2_19_2_stream_1, test_case_2_19_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_19_3, tgrp_case_2_19_3, name_case_2_19_3, desc_case_2_19_3, sref_case_2_19_3, {
	test_case_2_19_3_stream_0, test_case_2_19_3_stream_1, test_case_2_19_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#ifdef SNDHOLD
		numb_case_2_19_4, tgrp_case_2_19_4, name_case_2_19_4, desc_case_2_19_4, sref_case_2_19_4, {
	test_case_2_19_4_stream_0, test_case_2_19_4_stream_1, test_case_2_19_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
		numb_case_2_19_5, tgrp_case_2_19_5, name_case_2_19_5, desc_case_2_19_5, sref_case_2_19_5, {
	test_case_2_19_5_stream_0, test_case_2_19_5_stream_1, test_case_2_19_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#ifdef SNDHOLD
		numb_case_2_19_6, tgrp_case_2_19_6, name_case_2_19_6, desc_case_2_19_6, sref_case_2_19_6, {
	test_case_2_19_6_stream_0, test_case_2_19_6_stream_1, test_case_2_19_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_19_7, tgrp_case_2_19_7, name_case_2_19_7, desc_case_2_19_7, sref_case_2_19_7, {
	test_case_2_19_7_stream_0, test_case_2_19_7_stream_1, test_case_2_19_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_19_8, tgrp_case_2_19_8, name_case_2_19_8, desc_case_2_19_8, sref_case_2_19_8, {
	test_case_2_19_8_stream_0, test_case_2_19_8_stream_1, test_case_2_19_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
		numb_case_2_19_9, tgrp_case_2_19_9, name_case_2_19_9, desc_case_2_19_9, sref_case_2_19_9, {
	test_case_2_19_9_stream_0, test_case_2_19_9_stream_1, test_case_2_19_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_20_1, tgrp_case_2_20_1, name_case_2_20_1, desc_case_2_20_1, sref_case_2_20_1, {
	test_case_2_20_1_stream_0, test_case_2_20_1_stream_1, test_case_2_20_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_20_2, tgrp_case_2_20_2, name_case_2_20_2, desc_case_2_20_2, sref_case_2_20_2, {
	test_case_2_20_2_stream_0, test_case_2_20_2_stream_1, test_case_2_20_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_20_3, tgrp_case_2_20_3, name_case_2_20_3, desc_case_2_20_3, sref_case_2_20_3, {
	test_case_2_20_3_stream_0, test_case_2_20_3_stream_1, test_case_2_20_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_20_4, tgrp_case_2_20_4, name_case_2_20_4, desc_case_2_20_4, sref_case_2_20_4, {
	test_case_2_20_4_stream_0, test_case_2_20_4_stream_1, test_case_2_20_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#ifdef SNDHOLD
		numb_case_2_20_5, tgrp_case_2_20_5, name_case_2_20_5, desc_case_2_20_5, sref_case_2_20_5, {
	test_case_2_20_5_stream_0, test_case_2_20_5_stream_1, test_case_2_20_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
		numb_case_2_20_6, tgrp_case_2_20_6, name_case_2_20_6, desc_case_2_20_6, sref_case_2_20_6, {
	test_case_2_20_6_stream_0, test_case_2_20_6_stream_1, test_case_2_20_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#ifdef SNDHOLD
		numb_case_2_20_7, tgrp_case_2_20_7, name_case_2_20_7, desc_case_2_20_7, sref_case_2_20_7, {
	test_case_2_20_7_stream_0, test_case_2_20_7_stream_1, test_case_2_20_7_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_20_8, tgrp_case_2_20_8, name_case_2_20_8, desc_case_2_20_8, sref_case_2_20_8, {
	test_case_2_20_8_stream_0, test_case_2_20_8_stream_1, test_case_2_20_8_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_20_9, tgrp_case_2_20_9, name_case_2_20_9, desc_case_2_20_9, sref_case_2_20_9, {
	test_case_2_20_9_stream_0, test_case_2_20_9_stream_1, test_case_2_20_9_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
		numb_case_2_20_10, tgrp_case_2_20_10, name_case_2_20_10, desc_case_2_20_10, sref_case_2_20_10, {
	test_case_2_20_10_stream_0, test_case_2_20_10_stream_1, test_case_2_20_10_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_21, tgrp_case_2_21, name_case_2_21, desc_case_2_21, sref_case_2_21, {
	test_case_2_21_stream_0, test_case_2_21_stream_1, test_case_2_21_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_22, tgrp_case_2_22, name_case_2_22, desc_case_2_22, sref_case_2_22, {
	test_case_2_22_stream_0, test_case_2_22_stream_1, test_case_2_22_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_23, tgrp_case_2_23, name_case_2_23, desc_case_2_23, sref_case_2_23, {
	test_case_2_23_stream_0, test_case_2_23_stream_1, test_case_2_23_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_24, tgrp_case_2_24, name_case_2_24, desc_case_2_24, sref_case_2_24, {
	test_case_2_24_stream_0, test_case_2_24_stream_1, test_case_2_24_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_25, tgrp_case_2_25, name_case_2_25, desc_case_2_25, sref_case_2_25, {
	test_case_2_25_stream_0, test_case_2_25_stream_1, test_case_2_25_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_26, tgrp_case_2_26, name_case_2_26, desc_case_2_26, sref_case_2_26, {
	test_case_2_26_stream_0, test_case_2_26_stream_1, test_case_2_26_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_27_1, tgrp_case_2_27_1, name_case_2_27_1, desc_case_2_27_1, sref_case_2_27_1, {
	test_case_2_27_1_stream_0, test_case_2_27_1_stream_1, test_case_2_27_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_27_2, tgrp_case_2_27_2, name_case_2_27_2, desc_case_2_27_2, sref_case_2_27_2, {
	test_case_2_27_2_stream_0, test_case_2_27_2_stream_1, test_case_2_27_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_28_1, tgrp_case_2_28_1, name_case_2_28_1, desc_case_2_28_1, sref_case_2_28_1, {
	test_case_2_28_1_stream_0, test_case_2_28_1_stream_1, test_case_2_28_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_28_2, tgrp_case_2_28_2, name_case_2_28_2, desc_case_2_28_2, sref_case_2_28_2, {
	test_case_2_28_2_stream_0, test_case_2_28_2_stream_1, test_case_2_28_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_29_1, tgrp_case_2_29_1, name_case_2_29_1, desc_case_2_29_1, sref_case_2_29_1, {
	test_case_2_29_1_stream_0, test_case_2_29_1_stream_1, test_case_2_29_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_29_2, tgrp_case_2_29_2, name_case_2_29_2, desc_case_2_29_2, sref_case_2_29_2, {
	test_case_2_29_2_stream_0, test_case_2_29_2_stream_1, test_case_2_29_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_29_3, tgrp_case_2_29_3, name_case_2_29_3, desc_case_2_29_3, sref_case_2_29_3, {
	test_case_2_29_3_stream_0, test_case_2_29_3_stream_1, test_case_2_29_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_29_4, tgrp_case_2_29_4, name_case_2_29_4, desc_case_2_29_4, sref_case_2_29_4, {
	test_case_2_29_4_stream_0, test_case_2_29_4_stream_1, test_case_2_29_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_30_1, tgrp_case_2_30_1, name_case_2_30_1, desc_case_2_30_1, sref_case_2_30_1, {
	test_case_2_30_1_stream_0, test_case_2_30_1_stream_1, test_case_2_30_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#ifdef RERRNONPERSIST
		numb_case_2_30_2, tgrp_case_2_30_2, name_case_2_30_2, desc_case_2_30_2, sref_case_2_30_2, {
	test_case_2_30_2_stream_0, test_case_2_30_2_stream_1, test_case_2_30_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
#ifdef WERRNONPERSIST
		numb_case_2_30_3, tgrp_case_2_30_3, name_case_2_30_3, desc_case_2_30_3, sref_case_2_30_3, {
	test_case_2_30_3_stream_0, test_case_2_30_3_stream_1, test_case_2_30_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
#ifdef RERRNONPERSIST
		numb_case_2_30_4, tgrp_case_2_30_4, name_case_2_30_4, desc_case_2_30_4, sref_case_2_30_4, {
	test_case_2_30_4_stream_0, test_case_2_30_4_stream_1, test_case_2_30_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
		numb_case_2_30_5, tgrp_case_2_30_5, name_case_2_30_5, desc_case_2_30_5, sref_case_2_30_5, {
	test_case_2_30_5_stream_0, test_case_2_30_5_stream_1, test_case_2_30_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#if defined RERRNORM && defined WERRNORM
		numb_case_2_31_1, tgrp_case_2_31_1, name_case_2_31_1, desc_case_2_31_1, sref_case_2_31_1, {
	test_case_2_31_1_stream_0, test_case_2_31_1_stream_1, test_case_2_31_1_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_31_2, tgrp_case_2_31_2, name_case_2_31_2, desc_case_2_31_2, sref_case_2_31_2, {
	test_case_2_31_2_stream_0, test_case_2_31_2_stream_1, test_case_2_31_2_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
#if defined RERRNONPERSIST && defined WERRNORM
		numb_case_2_31_3, tgrp_case_2_31_3, name_case_2_31_3, desc_case_2_31_3, sref_case_2_31_3, {
	test_case_2_31_3_stream_0, test_case_2_31_3_stream_1, test_case_2_31_3_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
#ifdef WERRNONPERSIST
		numb_case_2_31_4, tgrp_case_2_31_4, name_case_2_31_4, desc_case_2_31_4, sref_case_2_31_4, {
	test_case_2_31_4_stream_0, test_case_2_31_4_stream_1, test_case_2_31_4_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
#ifdef RERRNONPERSIST
		numb_case_2_31_5, tgrp_case_2_31_5, name_case_2_31_5, desc_case_2_31_5, sref_case_2_31_5, {
	test_case_2_31_5_stream_0, test_case_2_31_5_stream_1, test_case_2_31_5_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
		numb_case_2_31_6, tgrp_case_2_31_6, name_case_2_31_6, desc_case_2_31_6, sref_case_2_31_6, {
	test_case_2_31_6_stream_0, test_case_2_31_6_stream_1, test_case_2_31_6_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_32, tgrp_case_2_32, name_case_2_32, desc_case_2_32, sref_case_2_32, {
	test_case_2_32_stream_0, test_case_2_32_stream_1, test_case_2_32_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#if 0
		numb_case_2_33, tgrp_case_2_33, name_case_2_33, desc_case_2_33, sref_case_2_33, {
	test_case_2_33_stream_0, test_case_2_33_stream_1, test_case_2_33_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_34, tgrp_case_2_34, name_case_2_34, desc_case_2_34, sref_case_2_34, {
	test_case_2_34_stream_0, test_case_2_34_stream_1, test_case_2_34_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_35, tgrp_case_2_35, name_case_2_35, desc_case_2_35, sref_case_2_35, {
	test_case_2_35_stream_0, test_case_2_35_stream_1, test_case_2_35_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_36, tgrp_case_2_36, name_case_2_36, desc_case_2_36, sref_case_2_36, {
	test_case_2_36_stream_0, test_case_2_36_stream_1, test_case_2_36_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_37, tgrp_case_2_37, name_case_2_37, desc_case_2_37, sref_case_2_37, {
	test_case_2_37_stream_0, test_case_2_37_stream_1, test_case_2_37_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_38, tgrp_case_2_38, name_case_2_38, desc_case_2_38, sref_case_2_38, {
	test_case_2_38_stream_0, test_case_2_38_stream_1, test_case_2_38_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_39, tgrp_case_2_39, name_case_2_39, desc_case_2_39, sref_case_2_39, {
	test_case_2_39_stream_0, test_case_2_39_stream_1, test_case_2_39_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_40, tgrp_case_2_40, name_case_2_40, desc_case_2_40, sref_case_2_40, {
	test_case_2_40_stream_0, test_case_2_40_stream_1, test_case_2_40_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_41, tgrp_case_2_41, name_case_2_41, desc_case_2_41, sref_case_2_41, {
	test_case_2_41_stream_0, test_case_2_41_stream_1, test_case_2_41_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_42, tgrp_case_2_42, name_case_2_42, desc_case_2_42, sref_case_2_42, {
	test_case_2_42_stream_0, test_case_2_42_stream_1, test_case_2_42_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_43, tgrp_case_2_43, name_case_2_43, desc_case_2_43, sref_case_2_43, {
	test_case_2_43_stream_0, test_case_2_43_stream_1, test_case_2_43_stream_2}, &begin_tests, &end_tests, 0, 0}, {
		numb_case_2_44, tgrp_case_2_44, name_case_2_44, desc_case_2_44, sref_case_2_44, {
	test_case_2_44_stream_0, test_case_2_44_stream_1, test_case_2_44_stream_2}, &begin_tests, &end_tests, 0, 0}, {
#endif
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
Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>\n\
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
    Copyright (c) 1997-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
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
