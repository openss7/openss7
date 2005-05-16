/*****************************************************************************

 @(#) $RCSfile: test-timod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/05/16 10:22:06 $

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

 Last Modified $Date: 2005/05/16 10:22:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-timod.c,v $
 Revision 0.9.2.7  2005/05/16 10:22:06  brian
 - updating tests

 Revision 0.9.2.6  2005/05/14 08:28:31  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-timod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/05/16 10:22:06 $"

static char const ident[] = "$RCSfile: test-timod.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/05/16 10:22:06 $";

/*
 *  These is a ferry-clip TIMOD conformance test program for testing the
 *  OpenSS7 timod STREAMS module.
 *
 *  GENERAL OPERATION:
 *
 *  The test program opens STREAMS pipe and pushes the "timod" module onto one
 *  side of the pipe.
 *
 *  The test program exchanges TPI primitives with one side of the open pipe
 *  and exchagnes TPI primitives and executes TIMOD ioctls on the other side
 *  of the pipe.  The side of the pipe with the "timod" STREAMS module pushed
 *  represents the XTI library user of the TIMOD module.  The other side of
 *  the pipe is the test harness, as shown below:
 *
 *                               USER SPACE
 *                              TEST PROGRAM
 *  _________________________________________________________________________
 *               \   /  |                            \   /  |
 *                \ /   |                             \ /   |
 *                 |   / \                             |    |
 *     ____________|__/___\__________                  |    |
 *    |                              |                 |    |
 *    |                              |                 |    |
 *    |            TIMOD             |                 |    |
 *    |                              |                 |    |
 *    |        STREAMS MODULE        |                 |    |
 *    |                              |                 |    |
 *    |                              |                 |    |
 *    |______________________________|                 |    |
 *               \   /  |                              |    |
 *                \ /   |                              |    |
 *                 |    |                              |    |
 *                 |   / \                             |    |
 *     ____________|__/___\____________________________|____|_____________
 *    |                                                                   |
 *    |                                                                   |
 *    |                               PIPE                                |
 *    |                                                                   |
 *    |___________________________________________________________________|
 *
 *
 *  This test arrangement results in a a ferry-clip around the TIMOD module,
 *  where TPI primitives are injected and removed beneath the module as well
 *  as both TPI primitives and ioctls being performed above the module.
 *
 *  To preserve the environment and ensure that the tests are repeatable in
 *  any order, the entire test harness (pipe) is assembled and disassembled
 *  for each test.  A test preamble is used to place the module in the correct
 *  state for a test case to begin and then a postable is used to ensure that
 *  the module can be removed correctly.
 */

#include <sys/types.h>
#include <stropts.h>
#include <stdlib.h>

#if HAVE_INTTYPES_H
# include <inttypes.h>
#elif HAVE_STDINT_H
# include <stdint.h>
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

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define NEED_T_USCALAR_T

#include <xti.h>
#include <tihdr.h>
#include <timod.h>
#include <xti_inet.h>

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "OpenSS7 XTI/TLI Library";
/* static const char *spkgname = "XNET"; */
static const char *lstdname = "XNS 5.2";
static const char *sstdname = "XNS";
static const char *shortname = "TIMOD";
static char devname[256] = "/dev/echo";

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

enum {
	__TEST_CONN_REQ = 2, __TEST_CONN_RES, __TEST_DISCON_REQ,
	__TEST_DATA_REQ, __TEST_EXDATA_REQ, __TEST_INFO_REQ, __TEST_BIND_REQ,
	__TEST_UNBIND_REQ, __TEST_UNITDATA_REQ, __TEST_OPTMGMT_REQ,
	__TEST_ORDREL_REQ, __TEST_OPTDATA_REQ, __TEST_ADDR_REQ,
	__TEST_CAPABILITY_REQ, __TEST_CONN_IND, __TEST_CONN_CON,
	__TEST_DISCON_IND, __TEST_DATA_IND, __TEST_EXDATA_IND,
	__TEST_INFO_ACK, __TEST_BIND_ACK, __TEST_ERROR_ACK, __TEST_OK_ACK,
	__TEST_UNITDATA_IND, __TEST_UDERROR_IND, __TEST_OPTMGMT_ACK,
	__TEST_ORDREL_IND, __TEST_NRM_OPTDATA_IND, __TEST_EXP_OPTDATA_IND,
	__TEST_ADDR_ACK, __TEST_CAPABILITY_ACK, __TEST_WRITE, __TEST_WRITEV,
	__TEST_PUTMSG_DATA, __TEST_PUTPMSG_DATA, __TEST_PUSH, __TEST_POP,
	__TEST_READ, __TEST_READV, __TEST_GETMSG, __TEST_GETPMSG,
	__TEST_DATA,
	__TEST_O_TI_GETINFO, __TEST_O_TI_OPTMGMT, __TEST_O_TI_BIND,
	__TEST_O_TI_UNBIND,
	__TEST__O_TI_GETINFO, __TEST__O_TI_OPTMGMT, __TEST__O_TI_BIND,
	__TEST__O_TI_UNBIND, __TEST__O_TI_GETMYNAME, __TEST__O_TI_GETPEERNAME,
	__TEST__O_TI_XTI_HELLO, __TEST__O_TI_XTI_GET_STATE,
	__TEST__O_TI_XTI_CLEAR_EVENT, __TEST__O_TI_XTI_MODE,
	__TEST__O_TI_TLI_MODE,
	__TEST_TI_GETINFO, __TEST_TI_OPTMGMT, __TEST_TI_BIND,
	__TEST_TI_UNBIND, __TEST_TI_GETMYNAME, __TEST_TI_GETPEERNAME,
	__TEST_TI_SETMYNAME, __TEST_TI_SETPEERNAME, __TEST_TI_SYNC,
	__TEST_TI_GETADDRS, __TEST_TI_CAPABILITY,
	__TEST_TI_SETMYNAME_DATA, __TEST_TI_SETPEERNAME_DATA,
	__TEST_TI_SETMYNAME_DISC, __TEST_TI_SETPEERNAME_DISC,
	__TEST_TI_SETMYNAME_DISC_DATA, __TEST_TI_SETPEERNAME_DISC_DATA,
};

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

#if 0
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
#endif

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
		fprintf(stdout, "                    |  | %11.6g                   |                    <%d>\n", t, state);
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

#if 0
static int start_st(long duration)
{
	long sdur = (duration + timer_scale - 1) / timer_scale;
	return start_tt(sdur);
}
#endif

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

const char *ioctl_string(int cmd, intptr_t arg)
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
		if (arg) {
			struct strioctl *icp = (struct strioctl *) arg;
			switch (icp->ic_cmd) {
			case _O_TI_BIND:
				return ("_O_TI_BIND");
			case O_TI_BIND:
				return ("O_TI_BIND");
			case _O_TI_GETINFO:
				return ("_O_TI_GETINFO");
			case O_TI_GETINFO:
				return ("O_TI_GETINFO");
			case _O_TI_GETMYNAME:
				return ("_O_TI_GETMYNAME");
			case _O_TI_GETPEERNAME:
				return ("_O_TI_GETPEERNAME");
			case _O_TI_OPTMGMT:
				return ("_O_TI_OPTMGMT");
			case O_TI_OPTMGMT:
				return ("O_TI_OPTMGMT");
			case _O_TI_TLI_MODE:
				return ("_O_TI_TLI_MODE");
			case _O_TI_UNBIND:
				return ("_O_TI_UNBIND");
			case O_TI_UNBIND:
				return ("O_TI_UNBIND");
			case _O_TI_XTI_CLEAR_EVENT:
				return ("_O_TI_XTI_CLEAR_EVENT");
			case _O_TI_XTI_GET_STATE:
				return ("_O_TI_XTI_GET_STATE");
			case _O_TI_XTI_HELLO:
				return ("_O_TI_XTI_HELLO");
			case _O_TI_XTI_MODE:
				return ("_O_TI_XTI_MODE");
			case TI_BIND:
				return ("TI_BIND");
			case TI_CAPABILITY:
				return ("TI_CAPABILITY");
			case TI_GETADDRS:
				return ("TI_GETADDRS");
			case TI_GETINFO:
				return ("TI_GETINFO");
			case TI_GETMYNAME:
				return ("TI_GETMYNAME");
			case TI_GETPEERNAME:
				return ("TI_GETPEERNAME");
			case TI_OPTMGMT:
				return ("TI_OPTMGMT");
			case TI_SETMYNAME:
				return ("TI_SETMYNAME");
			case TI_SETPEERNAME:
				return ("TI_SETPEERNAME");
			case TI_SYNC:
				return ("TI_SYNC");
			case TI_UNBIND:
				return ("TI_UNBIND");
			}
		}
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
	case I_E_RECVFD:
		return ("I_E_RECVFD");
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
	case I_SERROPT:
		return ("I_SERROPT");
	case I_GERROPT:
		return ("I_GERROPT");
	case I_ANCHOR:
		return ("I_ANCHOR");
#if 0
	case I_S_RECVFD:
		return ("I_S_RECVFD");
	case I_STATS:
		return ("I_STATS");
	case I_BIGPIPE:
		return ("I_BIGPIPE");
#endif
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
	default:
		return ("(unexpected)");
	}
}

const char *prim_string(ulong prim)
{
	switch (prim) {
	case T_CONN_REQ:
		return ("T_CONN_REQ------");
	case T_CONN_RES:
		return ("T_CONN_RES------");
	case T_DISCON_REQ:
		return ("T_DISCON_REQ----");
	case T_DATA_REQ:
		return ("T_DATA_REQ------");
	case T_EXDATA_REQ:
		return ("T_EXDATA_REQ----");
	case T_INFO_REQ:
		return ("T_INFO_REQ------");
	case T_BIND_REQ:
		return ("T_BIND_REQ------");
	case T_UNBIND_REQ:
		return ("T_UNBIND_REQ----");
	case T_UNITDATA_REQ:
		return ("T_UNITDATA_REQ--");
	case T_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ---");
	case T_ORDREL_REQ:
		return ("T_ORDREL_REQ----");
	case T_OPTDATA_REQ:
		return ("T_OPTDATA_REQ---");
	case T_ADDR_REQ:
		return ("T_ADDR_REQ------");
	case T_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case T_CONN_IND:
		return ("T_CONN_IND------");
	case T_CONN_CON:
		return ("T_CONN_CON------");
	case T_DISCON_IND:
		return ("T_DISCON_IND----");
	case T_DATA_IND:
		return ("T_DATA_IND------");
	case T_EXDATA_IND:
		return ("T_EXDATA_IND----");
	case T_INFO_ACK:
		return ("T_INFO_ACK------");
	case T_BIND_ACK:
		return ("T_BIND_ACK------");
	case T_ERROR_ACK:
		return ("T_ERROR_ACK-----");
	case T_OK_ACK:
		return ("T_OK_ACK--------");
	case T_UNITDATA_IND:
		return ("T_UNITDATA_IND--");
	case T_UDERROR_IND:
		return ("T_UDERROR_IND---");
	case T_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK---");
	case T_ORDREL_IND:
		return ("T_ORDREL_IND----");
	case T_OPTDATA_IND:
		return ("T_OPTDATA_IND---");
	case T_ADDR_ACK:
		return ("T_ADDR_ACK------");
	case T_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("T_????_???------");
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

void print_pipe(int child)
{
	static const char *msgs[] = {
		"  pipe()      ----->v  v<------------------------------>v                   \n",
		"                    v  v<------------------------------>v<-----     pipe()  \n",
		"                    .  .                                .                   \n",
	};
	if (verbose > 3)
		print_simple(child, msgs);
}

void print_open(int child)
{
	static const char *msgs[] = {
		"  open()      ----->v  .                                .                   \n",
		"                    |  v                                v<-----     open()  \n",
		"                    .  .                                .                   \n",
	};
	if (verbose > 3)
		print_simple(child, msgs);
}

void print_close(int child)
{
	static const char *msgs[] = {
		"  close()     ----->X  |                                |                   \n",
		"                    .  X                                X<-----    close()  \n",
		"                    .  .                                .                   \n",
	};
	if (verbose > 3)
		print_simple(child, msgs);
}

void print_preamble(int child)
{
	static const char *msgs[] = {
		"--------------------+  +---------Preamble--------------+                    \n",
		"                    +  +---------Preamble--------------+--------------------\n",
		"                    +--+---------Preamble--------------+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_inconclusive(int child)
{
	static const char *msgs[] = {
		"???????????????????\?|  |?????\? INCONCLUSIVE ???????\???\?|                    [%d]\n",
		"                    |  |?????\? INCONCLUSIVE ???????\???\?|???????????????????\?[%d]\n",
		"                    |??|?????\? INCONCLUSIVE ???????\?|?\?|                    [%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
};

void print_test(int child)
{
	static const char *msgs[] = {
		"--------------------+  +-----------Test----------------+                    \n",
		"                    +  +-----------Test----------------+--------------------\n",
		"                    +--+-----------Test----------------+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|  |XXXXXXXXX FAILED XXXXXXXXXXXXXX|                    [%d]\n",
		"                    |  |XXXXXXXXX FAILED XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXX[%d]\n",
		"                    |XX|XXXXXXXXX FAILED XXXXXXXXXXXXXX|                    [%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
};

void print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|  |####### SCRIPT ERROR ##########|                    [%d]\n",
		"                    |  |####### SCRIPT ERROR ###########|###################[%d]\n",
		"                    |##|####### SCRIPT ERROR ##########|                    [%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
};

void print_passed(int child)
{
	static const char *msgs[] = {
		"********************|  |********* PASSED **************|                    [%d]\n",
		"                    |  |********* PASSED **************|********************[%d]\n",
		"                    |**|********* PASSED **************|                    [%d]\n",
	};
	if (verbose > 2)
		print_simple_int(child, msgs, state);
};

void print_postamble(int child)
{
	static const char *msgs[] = {
		"--------------------+  +----------Postamble------------+                    \n",
		"                    +  +----------Postamble------------+--------------------\n",
		"                    +--+---------Postamble-------------+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_test_end(int child)
{
	static const char *msgs[] = {
		"--------------------+  +-------------------------------+                    \n",
		"                    +  +-------------------------------+--------------------\n",
		"                    +--+-------------------------------+                    \n",
	};
	if (verbose > 0)
		print_simple(child, msgs);
};

void print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|  |@@@@@@@ TERMINATED @@@@@@@@@@@@@|                   {%d}\n",
		"                    |  |@@@@@@@ TERMINATED @@@@@@@@@@@@@|@@@@@@@@@@@@@@@@@@@{%d}\n",
		"                    |@@@@@@@@@@ TERMINATED @@@@@@@@@@@@@|                   {%d}\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, signal);
};

void print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|  |&&&&&&& STOPPED &&&&&&&&&&&&&&&&|                   {%d}\n",
		"                    |  |&&&&&&& STOPPED &&&&&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&&{%d}\n",
		"                    |&&&&&&&&&& STOPPED &&&&&&&&&&&&&&&&|                   {%d}\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, signal);
};

void print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|  |+++++++++ TIMEOUT! +++++++++++++|                   [%d]\n",
		"                    |  |+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++[%d]\n",
		"                    |++|+++++++++ TIMEOUT! +++++++++++++|                   [%d]\n",
		"++++++++++++++++++++|++|+++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++[%d]\n",
	};
	if (show_timeout || verbose > 0) {
		print_simple_int(child, msgs, state);
		show_timeout--;
	}
};

void print_nothing(int child)
{
	static const char *msgs[] = {
		"- - - - - - - - - - |  |- - - - - nothing! - - - - - - -|                   [%d]\n",
		"                    |  |- - - - - nothing! - - - - - - -|- - - - - - - - - -[%d]\n",
		"                    |- |- - - - - nothing! - - - - - - -|                   [%d]\n",
		"- - - - - - - - - - |- |- - - - - nothing! - - - - - - -|- - - - - - - - - -[%d]\n",
	};
	if (verbose > 1)
		print_simple_int(child, msgs, state);
};

void print_string_state(int child, const char *msgs[], const char *string)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, state);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"  %-14s--->|  |                                |                   [%d]\n",
		"                    |  |                                |<---%14s [%d]\n",
		"                    |  |                                |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_tx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"--%16s->|  |                                |                   [%d]\n",
		"                    |  |<- - - - - - - - - - - - - - - -|<-%16s-[%d]\n",
		"                    |  |                                |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_rx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s--|  |                                |                   [%d]\n",
		"                    |  |- - - - - - - - - - - - - - - ->|-%16s->[%d]\n",
		"                    |  |                                |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_errno(int child, long error)
{
	static const char *msgs[] = {
		"  %-14s<--/|  |                                |                   [%d]\n",
		"                    |  |                                |\\-->%14s [%d]\n",
		"                    |  |                                |                   [%d]\n",
		"                    |  |       [%14s]         |                   [%d]\n",
	};
	if (verbose > 3)
		print_string_state(child, msgs, errno_string(error));
}

void print_success(int child)
{
	static const char *msgs[] = {
		"  ok          <----/|  |                                |                   [%d]\n",
		"                    |  |                                |\\---->         ok  [%d]\n",
		"                    |  |                                |                   [%d]\n",
	};
	if (verbose > 4)
		print_simple_int(child, msgs, state);
}

void print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"  %10d  <----/|  |                                |                   [%d]\n",
		"                    |  |                                |\\---->  %10d [%d]\n",
		"                    |  |                                |                   [%d]\n",
	};
	if (verbose)
		print_double_int(child, msgs, value, state);
}

void print_ti_ioctl(int child, int cmd, intptr_t arg)
{
	static const char *msgs[] = {
		"--ioctl(2)--------->|  |    %16s            |                   [%d]\n",
		"                    |  |    %16s            |<---ioctl(2)------ [%d]\n",
		"                    |  |    %16s            |<---ioctl(2)------ [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, ioctl_string(cmd, arg));
}

void print_ioctl(int child, int cmd, intptr_t arg)
{
	if (verbose > 3)
		print_ti_ioctl(child, cmd, arg);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Driver actions.
 *
 *  -------------------------------------------------------------------------
 */
int test_putpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int band, int flags)
{
	if (ctrl != NULL) {
		union T_primitives *p = (typeof(p)) ctrl->buf;
		print_tx_prim(child, prim_string(p->type));
	}
	if (band) {
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putpmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			lockf(fileno(stdout), F_ULOCK, 0);
			fflush(stdout);
		}
		if (ctrl == NULL)
			print_syscall(child, "putpmsg(2)----");
		for (;;) {
			if ((last_retval = putpmsg(test_fd[child], ctrl, data, band, flags)) == -1) {
				print_errno(child, (last_errno = errno));
				if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
					continue;
				return (__RESULT_FAILURE);
			}
			print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	} else {
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			lockf(fileno(stdout), F_ULOCK, 0);
			fflush(stdout);
		}
		if (ctrl == NULL)
			print_syscall(child, "putmsg(2)-----");
		for (;;) {
			if ((last_retval = putmsg(test_fd[child], ctrl, data, flags)) == -1) {
				print_errno(child, (last_errno = errno));
				if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
					continue;
				return (__RESULT_FAILURE);
			}
			print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	}
}

int test_write(int child, const void *buf, size_t len)
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

int test_writev(int child, const struct iovec *iov, int num)
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

int test_getmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *flagp)
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

int test_getpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *bandp, int *flagp)
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

int test_read(int child, void *buf, size_t count)
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

int test_readv(int child, const struct iovec *iov, int count)
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

int test_ti_ioctl(int child, int cmd, intptr_t arg)
{
	int tpi_error;
	if (cmd == I_STR && verbose > 3) {
		struct strioctl *icp = (struct strioctl *) arg;
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "ioctl from %d: cmd=%d, timout=%d, len=%d, dp=%p\n", child, icp->ic_cmd, icp->ic_timout, icp->ic_len, icp->ic_dp);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	print_ti_ioctl(child, cmd, arg);
	for (;;) {
		if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	if (cmd == I_STR && verbose > 3) {
		struct strioctl *icp = (struct strioctl *) arg;
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "got ioctl from %d: cmd=%d, timout=%d, len=%d, dp=%p\n", child, icp->ic_cmd, icp->ic_timout, icp->ic_len, icp->ic_dp);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (last_retval == 0)
		return __RESULT_SUCCESS;
	tpi_error = last_retval & 0x00ff;
	if (tpi_error == TSYSERR)
		last_errno = (last_retval >> 8) & 0x00ff;
	else
		last_errno = 0;
	if (verbose) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "***************ERROR: ioctl failed\n");
		if (verbose > 3)
			fprintf(stdout, "                    : %s; result = %d\n", __FUNCTION__, last_retval);
		fprintf(stdout, "                    : %s; TPI error = %d\n", __FUNCTION__, tpi_error);
		if (tpi_error == TSYSERR)
			fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
		lockf(fileno(stdout), F_ULOCK, 0);
		fflush(stdout);
	}
	return (__RESULT_FAILURE);
}

int test_ioctl(int child, int cmd, intptr_t arg)
{
	print_ioctl(child, cmd, arg);
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

int test_pipe(int child)
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
 *  IUT (Implementation Under Test) Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int top_push(void)
{
#if 0
	if (test_ioctl(0, I_SRDOPT, (intptr_t) RMSGD) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (test_ioctl(0, I_PUSH, (intptr_t) "timod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
#endif
	return __RESULT_SUCCESS;
}

static int top_pop(void)
{
#if 0
	if (test_ioctl(0, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
#endif
	return __RESULT_SUCCESS;
}

static int top_start(void)
{
	if (top_push() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

static int top_stop(void)
{
	if (top_pop() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  PT (Protocol Tester) Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int bot_open(void)
{
	if (test_pipe(0) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (test_ioctl(1, I_SRDOPT, (intptr_t) RMSGD) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (test_ioctl(1, I_PUSH, (intptr_t) "pipemod") != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

static int bot_close(void)
{
	if (test_ioctl(1, I_POP, (intptr_t) NULL) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (test_close(1) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (test_close(0) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

static int bot_start(void)
{
	if (bot_open() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

static int bot_stop(void)
{
	if (bot_close() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test harness initialization and termination.
 *
 *  -------------------------------------------------------------------------
 */

static int begin_tests(void)
{
	state = 0;
	if (bot_start() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (top_start() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	show_acks = 1;
	return __RESULT_SUCCESS;
}

static int end_tests(void)
{
	show_acks = 0;
	if (top_stop() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	if (bot_stop() != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	state++;
	return __RESULT_SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Injected event encoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

static int do_signal(int child, int action)
{
	int flags = MSG_BAND, band = 0;
	struct strbuf ctrl_buf, data_buf, *ctrl = &ctrl_buf, *data = &data_buf;
	char cbuf[256], dbuf[256];
	union T_primitives *p = (typeof(p)) cbuf;
	struct strioctl ic;
	ic.ic_cmd = 0;
	ic.ic_timout = 200;
	ic.ic_len = sizeof(cbuf);
	ic.ic_dp = cbuf;
	ctrl->maxlen = 0;
	ctrl->buf = cbuf;
	data->maxlen = 0;
	data->buf = dbuf;
	switch (action) {
	case __TEST_WRITE:
		data->len = sprintf(dbuf, "Write test data.");
		return test_write(child, dbuf, data->len);
	case __TEST_WRITEV:
	{
		struct iovec vector[4];
		vector[0].iov_base = dbuf;
		vector[0].iov_len = sprintf(vector[0].iov_base, "Writev test datum for vector 0.");
		vector[1].iov_base = dbuf + vector[0].iov_len;
		vector[1].iov_len = sprintf(vector[1].iov_base, "Writev test datum for vector 1.");
		vector[2].iov_base = dbuf + vector[1].iov_len;
		vector[2].iov_len = sprintf(vector[2].iov_base, "Writev test datum for vector 2.");
		vector[3].iov_base = dbuf + vector[2].iov_len;
		vector[3].iov_len = sprintf(vector[3].iov_base, "Writev test datum for vector 3.");
		return test_writev(child, vector, 4);
	}
	}
	switch (child) {
	case 0:
		switch (action) {
		case __TEST_PUSH:
			return test_ti_ioctl(child, I_PUSH, (intptr_t) "timod");
		case __TEST_POP:
			return test_ti_ioctl(child, I_POP, (intptr_t) NULL);
		case __TEST_PUTMSG_DATA:
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putmsg test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_PUTPMSG_DATA:
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putpmsg band test data.");
			flags = MSG_BAND;
			band = 1;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_CONN_REQ:
			ctrl->len = sizeof(p->conn_req);
			data = NULL;
			p->conn_req.PRIM_type = T_CONN_REQ;
			p->conn_req.DEST_length = 0;
			p->conn_req.DEST_offset = 0;
			p->conn_req.OPT_length = 0;
			p->conn_req.OPT_offset = 0;
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_CONN_RES:
			ctrl->len = sizeof(p->conn_res);
			data = NULL;
			p->conn_res.PRIM_type = T_CONN_RES;
			p->conn_res.ACCEPTOR_id = 0;
			p->conn_res.OPT_length = 0;
			p->conn_res.OPT_offset = 0;
			p->conn_res.SEQ_number = 0;
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_DISCON_REQ:
			ctrl->len = sizeof(p->discon_req);
			data = NULL;
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = 0;
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_DATA_REQ:
			ctrl->len = sizeof(p->data_req);
			p->data_req.PRIM_type = T_DATA_REQ;
			p->data_req.MORE_flag = 0;
			data->len = sprintf(dbuf, "Normal test message.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_EXDATA_REQ:
			ctrl->len = sizeof(p->exdata_req);
			p->exdata_req.PRIM_type = T_EXDATA_REQ;
			p->exdata_req.MORE_flag = 0;
			data->len = sprintf(dbuf, "Expedited test message.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_INFO_REQ:
			ctrl->len = sizeof(p->info_req);
			p->info_req.PRIM_type = T_INFO_REQ;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_BIND_REQ:
			ctrl->len = sizeof(p->bind_req);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			data = NULL;
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_UNBIND_REQ:
			ctrl->len = sizeof(p->unbind_req);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			data = NULL;
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_UNITDATA_REQ:
			ctrl->len = sizeof(p->unitdata_req);
			p->unitdata_req.PRIM_type = T_UNITDATA_REQ;
			p->unitdata_req.DEST_length = 0;
			p->unitdata_req.DEST_offset = 0;
			p->unitdata_req.OPT_length = 0;
			p->unitdata_req.OPT_offset = 0;
			data->len = sprintf(dbuf, "Unit test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_OPTMGMT_REQ:
			ctrl->len = sizeof(p->optmgmt_req);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			data = NULL;
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_ORDREL_REQ:
			ctrl->len = sizeof(p->ordrel_req);
			p->ordrel_req.PRIM_type = T_ORDREL_REQ;
			data->len = sprintf(dbuf, "Orderly release data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_OPTDATA_REQ:
			ctrl->len = sizeof(p->optdata_req);
			p->optdata_req.PRIM_type = T_OPTDATA_REQ;
			p->optdata_req.DATA_flag = T_ODF_EX | T_ODF_MORE;
			p->optdata_req.OPT_length = 0;
			p->optdata_req.OPT_offset = 0;
			data->len = sprintf(dbuf, "Option data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_ADDR_REQ:
			ctrl->len = sizeof(p->addr_req);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_CAPABILITY_REQ:
			ctrl->len = sizeof(p->capability_req);
			p->capability_req.PRIM_type = T_CAPABILITY_REQ;
			p->capability_req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_O_TI_GETINFO:
			ic.ic_cmd = O_TI_GETINFO;
			ic.ic_len = sizeof(p->info_ack);
			p->info_req.PRIM_type = T_INFO_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_O_TI_OPTMGMT:
			ic.ic_cmd = O_TI_OPTMGMT;
			ic.ic_len = sizeof(p->optmgmt_ack);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_O_TI_BIND:
			ic.ic_cmd = O_TI_BIND;
			ic.ic_len = sizeof(p->bind_ack);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_O_TI_UNBIND:
			ic.ic_cmd = O_TI_UNBIND;
			ic.ic_len = sizeof(p->ok_ack);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_GETINFO:
			ic.ic_cmd = _O_TI_GETINFO;
			ic.ic_len = sizeof(p->info_ack);
			p->info_req.PRIM_type = T_INFO_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_OPTMGMT:
			ic.ic_cmd = _O_TI_OPTMGMT;
			ic.ic_len = sizeof(p->optmgmt_ack);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_BIND:
			ic.ic_cmd = _O_TI_BIND;
			ic.ic_len = sizeof(p->bind_ack);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_UNBIND:
			ic.ic_cmd = _O_TI_UNBIND;
			ic.ic_len = sizeof(p->ok_ack);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_GETMYNAME:
			ic.ic_cmd = _O_TI_GETMYNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_GETPEERNAME:
			ic.ic_cmd = _O_TI_GETPEERNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_XTI_HELLO:
			ic.ic_cmd = _O_TI_XTI_HELLO;
			ic.ic_len = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_XTI_GET_STATE:
			ic.ic_cmd = _O_TI_XTI_GET_STATE;
			ic.ic_len = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_XTI_CLEAR_EVENT:
			ic.ic_cmd = _O_TI_XTI_CLEAR_EVENT;
			ic.ic_len = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_XTI_MODE:
			ic.ic_cmd = _O_TI_XTI_MODE;
			ic.ic_len = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST__O_TI_TLI_MODE:
			ic.ic_cmd = _O_TI_TLI_MODE;
			ic.ic_len = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_GETINFO:
			ic.ic_cmd = TI_GETINFO;
			ic.ic_len = sizeof(p->info_ack);
			p->info_req.PRIM_type = T_INFO_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_OPTMGMT:
			ic.ic_cmd = TI_OPTMGMT;
			ic.ic_len = sizeof(p->optmgmt_ack);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_BIND:
			ic.ic_cmd = TI_BIND;
			ic.ic_len = sizeof(p->bind_ack);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_UNBIND:
			ic.ic_cmd = TI_UNBIND;
			ic.ic_len = sizeof(p->ok_ack);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_GETMYNAME:
			ic.ic_cmd = TI_GETMYNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_GETPEERNAME:
			ic.ic_cmd = TI_GETPEERNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETMYNAME:
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->conn_res);
			p->conn_res.PRIM_type = T_CONN_RES;
			p->conn_res.ACCEPTOR_id = 0;
			p->conn_res.OPT_length = 0;
			p->conn_res.OPT_offset = 0;
			p->conn_res.SEQ_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETPEERNAME:
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->conn_req);
			p->conn_req.PRIM_type = T_CONN_REQ;
			p->conn_req.DEST_length = 0;
			p->conn_req.DEST_offset = 0;
			p->conn_req.OPT_length = 0;
			p->conn_req.OPT_offset = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETMYNAME_DISC:
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->discon_req);
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETPEERNAME_DISC:
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->discon_req);
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETMYNAME_DATA:
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->conn_res) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->conn_res.PRIM_type = T_CONN_RES;
			p->conn_res.ACCEPTOR_id = 0;
			p->conn_res.OPT_length = 0;
			p->conn_res.OPT_offset = 0;
			p->conn_res.SEQ_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETPEERNAME_DATA:
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->conn_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->conn_req.PRIM_type = T_CONN_REQ;
			p->conn_req.DEST_length = 0;
			p->conn_req.DEST_offset = 0;
			p->conn_req.OPT_length = 0;
			p->conn_req.OPT_offset = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETMYNAME_DISC_DATA:
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->discon_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SETPEERNAME_DISC_DATA:
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->discon_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = 0;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_SYNC:
		{
			union {
				struct ti_sync_req req;
				struct ti_sync_ack ack;
			} *s = (typeof(s)) p;
			ic.ic_cmd = TI_SYNC;
			ic.ic_len = sizeof(*s);
			s->req.tsr_flags = TSRF_INFO_REQ | TSRF_IS_EXP_IN_RCVBUF | TSRF_QLEN_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		}
		case __TEST_TI_GETADDRS:
			ic.ic_cmd = TI_GETADDRS;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		case __TEST_TI_CAPABILITY:
			ic.ic_cmd = TI_CAPABILITY;
			ic.ic_len = sizeof(p->capability_ack);
			p->capability_req.PRIM_type = T_CAPABILITY_REQ;
			p->capability_req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	case 1:
		switch (action) {
		case __TEST_PUTMSG_DATA:
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putmsg test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_PUTPMSG_DATA:
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putpmsg band test data.");
			flags = MSG_BAND;
			band = 1;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_CONN_IND:
			ctrl->len = sizeof(p->conn_ind);
			p->conn_ind.PRIM_type = T_CONN_IND;
			p->conn_ind.SRC_length = 0;
			p->conn_ind.SRC_offset = 0;
			p->conn_ind.OPT_length = 0;
			p->conn_ind.OPT_offset = 0;
			p->conn_ind.SEQ_number = 0;
			data->len = sprintf(dbuf, "Connection indication test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_CONN_CON:
			ctrl->len = sizeof(p->conn_con);
			p->conn_con.PRIM_type = T_CONN_CON;
			p->conn_con.RES_length = 0;
			p->conn_con.RES_offset = 0;
			p->conn_con.OPT_length = 0;
			p->conn_con.OPT_offset = 0;
			data->len = sprintf(dbuf, "Connection confirmation test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_DISCON_IND:
			ctrl->len = sizeof(p->discon_ind);
			p->discon_ind.PRIM_type = T_DISCON_IND;
			p->discon_ind.DISCON_reason = 0;
			p->discon_ind.SEQ_number = 0;
			data->len = sprintf(dbuf, "Disconnection indication test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_DATA_IND:
			ctrl->len = sizeof(p->data_ind);
			p->data_ind.PRIM_type = T_DATA_IND;
			p->data_ind.MORE_flag = 0;
			data->len = sprintf(dbuf, "Normal test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_EXDATA_IND:
			ctrl->len = sizeof(p->exdata_ind);
			p->data_ind.PRIM_type = T_EXDATA_IND;
			p->data_ind.MORE_flag = 0;
			data->len = sprintf(dbuf, "Expedited test data.");
			flags = MSG_BAND;
			band = 1;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_INFO_ACK:
			ctrl->len = sizeof(p->info_ack);
			p->info_ack.PRIM_type = T_INFO_ACK;
			p->info_ack.TSDU_size = 0;
			p->info_ack.ETSDU_size = 0;
			p->info_ack.CDATA_size = 0;
			p->info_ack.DDATA_size = 0;
			p->info_ack.ADDR_size = 0;
			p->info_ack.OPT_size = 0;
			p->info_ack.TIDU_size = 0;
			p->info_ack.SERV_type = 0;
			p->info_ack.CURRENT_state = 0;
			p->info_ack.PROVIDER_flag = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_BIND_ACK:
			ctrl->len = sizeof(p->bind_ack);
			p->bind_ack.PRIM_type = T_BIND_ACK;
			p->bind_ack.ADDR_length = 0;
			p->bind_ack.ADDR_offset = 0;
			p->bind_ack.CONIND_number = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_ERROR_ACK:
			ctrl->len = sizeof(p->error_ack);
			p->error_ack.PRIM_type = T_ERROR_ACK;
			p->error_ack.ERROR_prim = last_prim;
			p->error_ack.TLI_error = TSYSERR;
			p->error_ack.UNIX_error = EPROTO;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_OK_ACK:
			ctrl->len = sizeof(p->ok_ack);
			p->ok_ack.PRIM_type = T_OK_ACK;
			p->ok_ack.CORRECT_prim = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_UNITDATA_IND:
			ctrl->len = sizeof(p->unitdata_ind);
			p->unitdata_ind.PRIM_type = T_UNITDATA_IND;
			p->unitdata_ind.SRC_length = 0;
			p->unitdata_ind.SRC_offset = 0;
			p->unitdata_ind.OPT_length = 0;
			p->unitdata_ind.OPT_offset = 0;
			data->len = sprintf(dbuf, "Unit test data indication.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_UDERROR_IND:
			ctrl->len = sizeof(p->uderror_ind);
			p->uderror_ind.PRIM_type = T_UDERROR_IND;
			p->uderror_ind.DEST_length = 0;
			p->uderror_ind.DEST_offset = 0;
			p->uderror_ind.OPT_length = 0;
			p->uderror_ind.OPT_offset = 0;
			p->uderror_ind.ERROR_type = 0;
			data = NULL;
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_OPTMGMT_ACK:
			ctrl->len = sizeof(p->optmgmt_ack);
			p->optmgmt_ack.PRIM_type = T_OPTMGMT_ACK;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_ORDREL_IND:
			ctrl->len = sizeof(p->ordrel_ind);
			p->ordrel_ind.PRIM_type = T_ORDREL_IND;
			data->len = sprintf(dbuf, "Orderly release indication test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_EXP_OPTDATA_IND:
			ctrl->len = sizeof(p->optdata_ind);
			p->optdata_ind.PRIM_type = T_OPTDATA_IND;
			p->optdata_ind.DATA_flag = T_ODF_EX;
			p->optdata_ind.OPT_length = 0;
			p->optdata_ind.OPT_offset = 0;
			data->len = sprintf(dbuf, "Option data indication test data.");
			flags = MSG_BAND;
			band = 1;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_NRM_OPTDATA_IND:
			ctrl->len = sizeof(p->optdata_ind);
			p->optdata_ind.PRIM_type = T_OPTDATA_IND;
			p->optdata_ind.DATA_flag = 0;
			p->optdata_ind.OPT_length = 0;
			p->optdata_ind.OPT_offset = 0;
			data->len = sprintf(dbuf, "Option data indication test data.");
			flags = MSG_BAND;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_ADDR_ACK:
			ctrl->len = sizeof(p->addr_ack);
			p->addr_ack.PRIM_type = T_ADDR_ACK;
			p->addr_ack.LOCADDR_length = 0;
			p->addr_ack.LOCADDR_offset = 0;
			p->addr_ack.REMADDR_length = 0;
			p->addr_ack.REMADDR_offset = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		case __TEST_CAPABILITY_ACK:
			ctrl->len = sizeof(p->capability_ack);
			p->capability_ack.PRIM_type = T_CAPABILITY_ACK;
			p->capability_ack.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			p->capability_ack.INFO_ack.TSDU_size = 0;
			p->capability_ack.INFO_ack.ETSDU_size = 0;
			p->capability_ack.INFO_ack.CDATA_size = 0;
			p->capability_ack.INFO_ack.DDATA_size = 0;
			p->capability_ack.INFO_ack.ADDR_size = 0;
			p->capability_ack.INFO_ack.OPT_size = 0;
			p->capability_ack.INFO_ack.TIDU_size = 0;
			p->capability_ack.INFO_ack.SERV_type = 0;
			p->capability_ack.INFO_ack.CURRENT_state = 0;
			p->capability_ack.INFO_ack.PROVIDER_flag = 0;
			p->capability_ack.ACCEPTOR_id = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			return test_putpmsg(child, ctrl, data, band, flags);
		default:
			return __RESULT_SCRIPT_ERROR;
		}
	}
	return __RESULT_SCRIPT_ERROR;
}

#if 0
static int top_signal(int action)
{
	return do_signal(0, action);
}

static int bot_signal(int action)
{
	return do_signal(1, action);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Received event decoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

static int do_decode_data(int child, struct strbuf *data)
{
	print_rx_prim(child, "DATA------------");
	return (__TEST_DATA);
}
static int do_decode_ctrl(int child, struct strbuf *ctrl, struct strbuf *data)
{
	int ret;
	union T_primitives *p = (union T_primitives *) ctrl->buf;
	switch (child) {
	case 0:
		print_rx_prim(child, prim_string(p->type));
		switch ((last_prim = p->type)) {
		case T_CONN_IND:
			ret = __TEST_CONN_IND;
			break;
		case T_CONN_CON:
			ret = __TEST_CONN_CON;
			break;
		case T_DISCON_IND:
			ret = __TEST_DISCON_IND;
			break;
		case T_DATA_IND:
			ret = __TEST_DATA_IND;
			break;
		case T_EXDATA_IND:
			ret = __TEST_EXDATA_IND;
			break;
		case T_INFO_ACK:
			ret = __TEST_INFO_ACK;
			break;
		case T_BIND_ACK:
			ret = __TEST_BIND_ACK;
			break;
		case T_ERROR_ACK:
			ret = __TEST_ERROR_ACK;
			break;
		case T_OK_ACK:
			ret = __TEST_OK_ACK;
			break;
		case T_UNITDATA_IND:
			ret = __TEST_UNITDATA_IND;
			break;
		case T_UDERROR_IND:
			ret = __TEST_UDERROR_IND;
			break;
		case T_OPTMGMT_ACK:
			ret = __TEST_OPTMGMT_ACK;
			break;
		case T_ORDREL_IND:
			ret = __TEST_ORDREL_IND;
			break;
		case T_OPTDATA_IND:
			if (p->optdata_ind.DATA_flag & T_ODF_EX)
				ret = __TEST_EXP_OPTDATA_IND;
			else
				ret = __TEST_NRM_OPTDATA_IND;
			break;
		case T_ADDR_ACK:
			ret = __TEST_ADDR_ACK;
			break;
		case T_CAPABILITY_ACK:
			ret = __TEST_CAPABILITY_ACK;
			break;
		default:
			ret = __RESULT_DECODE_ERROR;
			break;
		}
		if (data && data->len >= 0)
			if (do_decode_data(child, data) != __TEST_DATA)
				ret = __RESULT_FAILURE;
		break;
	case 1:
		print_rx_prim(child, prim_string(p->type));
		switch ((last_prim = p->type)) {
		case T_CONN_REQ:
			ret = __TEST_CONN_REQ;
			break;
		case T_CONN_RES:
			ret = __TEST_CONN_RES;
			break;
		case T_DISCON_REQ:
			ret = __TEST_DISCON_REQ;
			break;
		case T_DATA_REQ:
			ret = __TEST_DATA_REQ;
			break;
		case T_EXDATA_REQ:
			ret = __TEST_EXDATA_REQ;
			break;
		case T_INFO_REQ:
			ret = __TEST_INFO_REQ;
			break;
		case T_BIND_REQ:
			ret = __TEST_BIND_REQ;
			break;
		case T_UNBIND_REQ:
			ret = __TEST_UNBIND_REQ;
			break;
		case T_UNITDATA_REQ:
			ret = __TEST_UNITDATA_REQ;
			break;
		case T_OPTMGMT_REQ:
			ret = __TEST_OPTMGMT_REQ;
			break;
		case T_ORDREL_REQ:
			ret = __TEST_ORDREL_REQ;
			break;
		case T_OPTDATA_REQ:
			ret = __TEST_OPTDATA_REQ;
			break;
		case T_ADDR_REQ:
			ret = __TEST_ADDR_REQ;
			break;
		case T_CAPABILITY_REQ:
			ret = __TEST_CAPABILITY_REQ;
			break;
		default:
			ret = __RESULT_DECODE_ERROR;
			break;
		}
		if (data && data->len >= 0)
			if (do_decode_data(child, data) != __TEST_DATA)
				ret = __RESULT_FAILURE;
		break;
	default:
		ret = __RESULT_FAILURE;
		break;
	}
	return (ret);
}

#if 0
#define IUT 0x00000001UL
#define PT  0x00000002UL
#define ANY 0x00000003UL

int any_wait_event(int source, int wait)
{
	while (1) {
		struct pollfd pfd[] = {
			{test_fd[0], POLLIN | POLLPRI, 0},
			{test_fd[1], POLLIN | POLLPRI, 0}
		};
		if (timer_timeout) {
			timer_timeout = 0;
			print_timeout(3);
			last_event = __EVENT_TIMEOUT;
			return time_event(__EVENT_TIMEOUT);
		}
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "polling:\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		pfd[0].fd = test_fd[0];
		pfd[0].events = (source & IUT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[0].revents = 0;
		pfd[1].fd = test_fd[1];
		pfd[1].events = (source & PT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[1].revents = 0;
		switch (poll(pfd, 2, wait)) {
		case -1:
			if ((errno == EAGAIN || errno == EINTR))
				break;
			print_errno(3, (last_errno = errno));
			break;
		case 0:
			print_nothing(3);
			last_event = __EVENT_NO_MSG;
			return time_event(__EVENT_NO_MSG);
		case 1:
		case 2:
			if (pfd[0].revents) {
				int flags = 0;
				char cbuf[BUFSIZE];
				char dbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf };
				struct strbuf data = { BUFSIZE, 0, dbuf };
				if (verbose > 3) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "getmsg from top:\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				if (getmsg(test_fd[0], &ctrl, &data, &flags) == 0) {
					if (verbose > 3) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from top [%d,%d]:\n", ctrl.len, data.len);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (ctrl.len > 0) {
						if ((last_event = do_decode_ctrl(0, &ctrl, &data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = do_decode_data(0, &data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			if (pfd[1].revents) {
				int flags = 0;
				char cbuf[BUFSIZE];
				char dbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf };
				struct strbuf data = { BUFSIZE, 0, dbuf };
				if (verbose > 3) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "getmsg from bot:\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				if (getmsg(test_fd[1], &ctrl, &data, &flags) == 0) {
					if (verbose > 3) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from bot [%d,%d]:\n", ctrl.len, data.len);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (ctrl.len > 0) {
						if ((last_event = do_decode_ctrl(1, &ctrl, &data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = do_decode_data(1, &data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
		default:
			break;
		}
	}
}
#endif

int wait_event(int child, int wait)
{
	while (1) {
		struct pollfd pfd[] = { {test_fd[child], POLLIN | POLLPRI, 0} };
		if (timer_timeout) {
			timer_timeout = 0;
			print_timeout(child);
			last_event = __EVENT_TIMEOUT;
			return time_event(__EVENT_TIMEOUT);
		}
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "polling %d:\n", child);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		pfd[0].fd = test_fd[child];
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			if ((errno == EAGAIN || errno == EINTR))
				break;
			print_errno(child, (last_errno = errno));
			break;
		case 0:
			print_nothing(child);
			last_event = __EVENT_NO_MSG;
			return time_event(__EVENT_NO_MSG);
		case 1:
			if (pfd[0].revents) {
				int flags = 0;
				char cbuf[BUFSIZE];
				char dbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf };
				struct strbuf data = { BUFSIZE, 0, dbuf };
				if (verbose > 3) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "getmsg from %d:\n", child);
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				if (getmsg(test_fd[child], &ctrl, &data, &flags) == 0) {
					if (verbose > 3) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from %d [%d,%d]:\n", child, ctrl.len, data.len);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (ctrl.len > 0) {
						if ((last_event = do_decode_ctrl(child, &ctrl, &data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = do_decode_data(child, &data)) != __EVENT_UNKNOWN)
							return time_event(last_event);
					}
				}
			}
		default:
			break;
		}
	}
	return __EVENT_UNKNOWN;
}

int get_event(int child)
{
	return wait_event(child, -1);
}

int get_data(int child, int action)
{
	int ret = 0;
	switch (action) {
	case __TEST_READ:
	{
		char buf[BUFSIZE];
		test_read(child, buf, BUFSIZE);
		ret = last_retval;
		break;
	}
	case __TEST_READV:
	{
		char buf[BUFSIZE];
		static const size_t count = 4;
		struct iovec vector[] = {
			{buf, (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2), (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2) + (BUFSIZE >> 2), (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2) + (BUFSIZE >> 2) + (BUFSIZE >> 2), (BUFSIZE >> 2)}
		};
		test_readv(child, vector, count);
		ret = last_retval;
		break;
	}
	case __TEST_GETMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		int flag = 0;
		if (test_getmsg(child, NULL, &data, &flag) == __RESULT_FAILURE) {
			ret = last_retval;
			break;
		}
		ret = data.len;
		break;
	}
	case __TEST_GETPMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		int band = 0;
		int flag = 0;
		if (test_getpmsg(child, NULL, &data, &band, &flag) == __RESULT_FAILURE) {
			ret = last_retval;
			break;
		}
		ret = data.len;
		break;
	}
	}
	return (ret);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */
static int preamble_0(int child)
{
	start_tt(1000);
	return __RESULT_SUCCESS;
};

static int postamble_0(int child)
{
	stop_tt();
	state++;
	start_tt(1000);
	for (;;) {
		state++;
		switch (wait_event(child, 0)) {
		case __EVENT_NO_MSG:
			break;
		case __EVENT_TIMEOUT:
			break;
		default:
			continue;
		}
		break;
	}
	state++;
	stop_tt();
	return __RESULT_SUCCESS;
};

#if 0
static int preamble_1_top(int child)
{
	start_tt(1000);
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

static int postamble_1_top(int child)
{
	while (wait_event(child, 0) != __EVENT_NO_MSG) ;
	state++;
	stop_tt();
	if (do_signal(child, __TEST_POP))
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}

static int postamble_1_bot(int child)
{
	for (;;)
		switch (get_event(child)) {
		case __TEST_DISCON_REQ:
			stop_tt();
			return __RESULT_SUCCESS;
		case __EVENT_NO_MSG:
			break;;
		default:
			stop_tt();
			return (__RESULT_FAILURE);
		}
	state++;
	return __RESULT_SCRIPT_ERROR;
}
#endif

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

#define test_group_1 "Pushing and popping the timod module"

#define tgrp_case_1_1 test_group_1
#define name_case_1_1 "Normal push and pop sequence"
#define desc_case_1_1 "\
This test case tests the simple pushing and popping of the timod module.  It\n\
ensures that pushing the module on an empty stream has no effect, while\n\
popping the module from the stream results in a T_DISCON_REQ being sent to the\n\
transport peer."
static int test_1_1_top(int child)
{
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, __TEST_POP) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
static int test_1_1_bot(int child)
{
	start_tt(200);
	state++;
	while (get_event(child) == __EVENT_NO_MSG) ;
	state++;
	switch (last_event) {
	case __TEST_DISCON_REQ:
	case __EVENT_TIMEOUT:
		state++;
		return (__RESULT_SUCCESS);
	}
	state++;
	return (__RESULT_FAILURE);
}
struct test_stream test_case_1_1_top = { &preamble_0, &test_1_1_top, &postamble_0 };
struct test_stream test_case_1_1_bot = { &preamble_0, &test_1_1_bot, &postamble_0 };
#define test_case_1_1_stream_top (&test_case_1_1_top)
#define test_case_1_1_stream_bot (&test_case_1_1_bot)

#define test_group_2 "Performing IO controls on the timod module"

#define tgrp_case_2_1_1 test_group_2
#define name_case_2_1_1 "TI_GETINFO IO control positive acknowledgement"
#define desc_case_2_1_1 "\
This test case test the execution of the TI_GETINFO IO control with positive\n\
results on using the timod module."
static int test_2_1_top(int child, int command)
{
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, command) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, __TEST_POP) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
static int test_2_1_bot(int child, int signal, int expect)
{
	start_tt(200);
	state++;
	if (get_event(child) != signal)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, expect) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	state++;
	switch (get_event(child)) {
	case __TEST_DISCON_REQ:
	case __EVENT_TIMEOUT:
		break;
	default:
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	return (__RESULT_SUCCESS);
}
static int test_2_1_1_top(int child)
{
	return test_2_1_top(child, __TEST_TI_GETINFO);
}
static int test_2_1_1_bot(int child)
{
	return test_2_1_bot(child, __TEST_INFO_REQ, __TEST_INFO_ACK);
}
struct test_stream test_case_2_1_1_top = { &preamble_0, &test_2_1_1_top, &postamble_0 };
struct test_stream test_case_2_1_1_bot = { &preamble_0, &test_2_1_1_bot, &postamble_0 };
#define test_case_2_1_1_stream_top (&test_case_2_1_1_top)
#define test_case_2_1_1_stream_bot (&test_case_2_1_1_bot)

#define tgrp_case_2_1_2 test_group_2
#define name_case_2_1_2 "TI_OPTMGMT IO control positive acknowledgement"
#define desc_case_2_1_2 "\
This test case test the execution of the TI_OPTMGMT IO control with positive\n\
results on using the timod module."
static int test_2_1_2_top(int child)
{
	return test_2_1_top(child, __TEST_TI_OPTMGMT);
}
static int test_2_1_2_bot(int child)
{
	return test_2_1_bot(child, __TEST_OPTMGMT_REQ, __TEST_OPTMGMT_ACK);
}
struct test_stream test_case_2_1_2_top = { &preamble_0, &test_2_1_2_top, &postamble_0 };
struct test_stream test_case_2_1_2_bot = { &preamble_0, &test_2_1_2_bot, &postamble_0 };
#define test_case_2_1_2_stream_top (&test_case_2_1_2_top)
#define test_case_2_1_2_stream_bot (&test_case_2_1_2_bot)

#define tgrp_case_2_1_3 test_group_2
#define name_case_2_1_3 "TI_BIND IO control positive acknowledgement"
#define desc_case_2_1_3 "\
This test case test the execution of the TI_BIND IO control with positive\n\
results on using the timod module."
static int test_2_1_3_top(int child)
{
	return test_2_1_top(child, __TEST_TI_BIND);
}
static int test_2_1_3_bot(int child)
{
	return test_2_1_bot(child, __TEST_BIND_REQ, __TEST_BIND_ACK);
}
struct test_stream test_case_2_1_3_top = { &preamble_0, &test_2_1_3_top, &postamble_0 };
struct test_stream test_case_2_1_3_bot = { &preamble_0, &test_2_1_3_bot, &postamble_0 };
#define test_case_2_1_3_stream_top (&test_case_2_1_3_top)
#define test_case_2_1_3_stream_bot (&test_case_2_1_3_bot)

#define tgrp_case_2_1_4 test_group_2
#define name_case_2_1_4 "TI_UNBIND IO control positive acknowledgement"
#define desc_case_2_1_4 "\
This test case test the execution of the TI_UNBIND IO control with positive\n\
results on using the timod module."
static int test_2_1_4_top(int child)
{
	return test_2_1_top(child, __TEST_TI_UNBIND);
}
static int test_2_1_4_bot(int child)
{
	return test_2_1_bot(child, __TEST_UNBIND_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_2_1_4_top = { &preamble_0, &test_2_1_4_top, &postamble_0 };
struct test_stream test_case_2_1_4_bot = { &preamble_0, &test_2_1_4_bot, &postamble_0 };
#define test_case_2_1_4_stream_top (&test_case_2_1_4_top)
#define test_case_2_1_4_stream_bot (&test_case_2_1_4_bot)

#define tgrp_case_2_1_5 test_group_2
#define name_case_2_1_5 "TI_GETMYNAME IO control positive acknowledgement"
#define desc_case_2_1_5 "\
This test case test the execution of the TI_GETMYNAME IO control with positive\n\
results on using the timod module."
static int test_2_1_5_top(int child)
{
	return test_2_1_top(child, __TEST_TI_GETMYNAME);
}
static int test_2_1_5_bot(int child)
{
	return test_2_1_bot(child, __TEST_ADDR_REQ, __TEST_ADDR_ACK);
}
struct test_stream test_case_2_1_5_top = { &preamble_0, &test_2_1_5_top, &postamble_0 };
struct test_stream test_case_2_1_5_bot = { &preamble_0, &test_2_1_5_bot, &postamble_0 };
#define test_case_2_1_5_stream_top (&test_case_2_1_5_top)
#define test_case_2_1_5_stream_bot (&test_case_2_1_5_bot)

#define tgrp_case_2_1_6 test_group_2
#define name_case_2_1_6 "TI_GETPEERNAME IO control positive acknowledgement"
#define desc_case_2_1_6 "\
This test case test the execution of the TI_GETPEERNAME IO control with positive\n\
results on using the timod module."
static int test_2_1_6_top(int child)
{
	return test_2_1_top(child, __TEST_TI_GETPEERNAME);
}
static int test_2_1_6_bot(int child)
{
	return test_2_1_bot(child, __TEST_ADDR_REQ, __TEST_ADDR_ACK);
}
struct test_stream test_case_2_1_6_top = { &preamble_0, &test_2_1_6_top, &postamble_0 };
struct test_stream test_case_2_1_6_bot = { &preamble_0, &test_2_1_6_bot, &postamble_0 };
#define test_case_2_1_6_stream_top (&test_case_2_1_6_top)
#define test_case_2_1_6_stream_bot (&test_case_2_1_6_bot)

#define tgrp_case_2_1_7_1 test_group_2
#define name_case_2_1_7_1 "TI_SETMYNAME IO control positive acknowledgement"
#define desc_case_2_1_7_1 "\
This test case test the execution of the TI_SETMYNAME IO control with positive\n\
results on using the timod module."
static int test_2_1_7_1_top(int child)
{
	return test_2_1_top(child, __TEST_TI_SETMYNAME);
}
static int test_2_1_7_1_bot(int child)
{
	return test_2_1_bot(child, __TEST_CONN_RES, __TEST_OK_ACK);
}
struct test_stream test_case_2_1_7_1_top = { &preamble_0, &test_2_1_7_1_top, &postamble_0 };
struct test_stream test_case_2_1_7_1_bot = { &preamble_0, &test_2_1_7_1_bot, &postamble_0 };
#define test_case_2_1_7_1_stream_top (&test_case_2_1_7_1_top)
#define test_case_2_1_7_1_stream_bot (&test_case_2_1_7_1_bot)

#define tgrp_case_2_1_7_2 test_group_2
#define name_case_2_1_7_2 "TI_SETMYNAME (disconnect) IO control positive acknowledgement"
#define desc_case_2_1_7_2 "\
This test case test the execution of the TI_SETMYNAME (disconnect) IO control\n\
with positive results on using the timod module."
static int test_2_1_7_2_top(int child)
{
	return test_2_1_top(child, __TEST_TI_SETMYNAME_DISC);
}
static int test_2_1_7_2_bot(int child)
{
	return test_2_1_bot(child, __TEST_DISCON_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_2_1_7_2_top = { &preamble_0, &test_2_1_7_2_top, &postamble_0 };
struct test_stream test_case_2_1_7_2_bot = { &preamble_0, &test_2_1_7_2_bot, &postamble_0 };
#define test_case_2_1_7_2_stream_top (&test_case_2_1_7_2_top)
#define test_case_2_1_7_2_stream_bot (&test_case_2_1_7_2_bot)

#define tgrp_case_2_1_8_1 test_group_2
#define name_case_2_1_8_1 "TI_SETPEERNAME IO control positive acknowledgement"
#define desc_case_2_1_8_1 "\
This test case test the execution of the TI_SETPEERNAME IO control with positive\n\
results on using the timod module."
static int test_2_1_8_1_top(int child)
{
	return test_2_1_top(child, __TEST_TI_SETPEERNAME);
}
static int test_2_1_8_1_bot(int child)
{
	return test_2_1_bot(child, __TEST_CONN_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_2_1_8_1_top = { &preamble_0, &test_2_1_8_1_top, &postamble_0 };
struct test_stream test_case_2_1_8_1_bot = { &preamble_0, &test_2_1_8_1_bot, &postamble_0 };
#define test_case_2_1_8_1_stream_top (&test_case_2_1_8_1_top)
#define test_case_2_1_8_1_stream_bot (&test_case_2_1_8_1_bot)

#define tgrp_case_2_1_8_2 test_group_2
#define name_case_2_1_8_2 "TI_SETPEERNAME (disconnect) IO control positive acknowledgement"
#define desc_case_2_1_8_2 "\
This test case test the execution of the TI_SETPEERNAME (disconnect) IO\n\
control with positive results on using the timod module."
static int test_2_1_8_2_top(int child)
{
	return test_2_1_top(child, __TEST_TI_SETPEERNAME_DISC);
}
static int test_2_1_8_2_bot(int child)
{
	return test_2_1_bot(child, __TEST_DISCON_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_2_1_8_2_top = { &preamble_0, &test_2_1_8_2_top, &postamble_0 };
struct test_stream test_case_2_1_8_2_bot = { &preamble_0, &test_2_1_8_2_bot, &postamble_0 };
#define test_case_2_1_8_2_stream_top (&test_case_2_1_8_2_top)
#define test_case_2_1_8_2_stream_bot (&test_case_2_1_8_2_bot)

#if 0
#define tgrp_case_2_1_9 test_group_2
#define name_case_2_1_9 "TI_SYNC IO control positive acknowledgement"
#define desc_case_2_1_9 "\
This test case test the execution of the TI_SYNC IO control with positive\n\
results on using the timod module."
static int test_2_1_9_top(int child)
{
	return test_2_1_top(child, __TEST_TI_SYNC);
}
static int test_2_1_9_bot(int child)
{
	return test_2_1_bot(child, __TEST_INFO_REQ, __TEST_INFO_ACK);
}
struct test_stream test_case_2_1_9_top = { &preamble_0, &test_2_1_9_top, &postamble_0 };
struct test_stream test_case_2_1_9_bot = { &preamble_0, &test_2_1_9_bot, &postamble_0 };
#define test_case_2_1_9_stream_top (&test_case_2_1_9_top)
#define test_case_2_1_9_stream_bot (&test_case_2_1_9_bot)
#endif

#define tgrp_case_2_1_10 test_group_2
#define name_case_2_1_10 "TI_GETADDRS IO control positive acknowledgement"
#define desc_case_2_1_10 "\
This test case test the execution of the TI_GETADDRS IO control with positive\n\
results on using the timod module."
static int test_2_1_10_top(int child)
{
	return test_2_1_top(child, __TEST_TI_GETADDRS);
}
static int test_2_1_10_bot(int child)
{
	return test_2_1_bot(child, __TEST_ADDR_REQ, __TEST_ADDR_ACK);
}
struct test_stream test_case_2_1_10_top = { &preamble_0, &test_2_1_10_top, &postamble_0 };
struct test_stream test_case_2_1_10_bot = { &preamble_0, &test_2_1_10_bot, &postamble_0 };
#define test_case_2_1_10_stream_top (&test_case_2_1_10_top)
#define test_case_2_1_10_stream_bot (&test_case_2_1_10_bot)

#define tgrp_case_2_1_11 test_group_2
#define name_case_2_1_11 "TI_CAPABILITY IO control positive acknowledgement"
#define desc_case_2_1_11 "\
This test case test the execution of the TI_CAPABILITY IO control with positive\n\
results on using the timod module."
static int test_2_1_11_top(int child)
{
	return test_2_1_top(child, __TEST_TI_CAPABILITY);
}
static int test_2_1_11_bot(int child)
{
	return test_2_1_bot(child, __TEST_CAPABILITY_REQ, __TEST_CAPABILITY_ACK);
}
struct test_stream test_case_2_1_11_top = { &preamble_0, &test_2_1_11_top, &postamble_0 };
struct test_stream test_case_2_1_11_bot = { &preamble_0, &test_2_1_11_bot, &postamble_0 };
#define test_case_2_1_11_stream_top (&test_case_2_1_11_top)
#define test_case_2_1_11_stream_bot (&test_case_2_1_11_bot)

#define tgrp_case_2_2_1 test_group_2
#define name_case_2_2_1 "TI_GETINFO IO control negative acknowledgement"
#define desc_case_2_2_1 "\
This test case test the execution of the TI_GETINFO IO control with negative\n\
results on using the timod module."
static int test_2_2_top(int child, int command)
{
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, command) == __RESULT_SUCCESS || last_errno != EPROTO)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, __TEST_POP) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
static int test_2_2_bot(int child, int signal, int expect)
{
	start_tt(200);
	state++;
	if (get_event(child) != signal)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, expect) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	state++;
	switch (get_event(child)) {
	case __TEST_DISCON_REQ:
	case __EVENT_TIMEOUT:
		break;
	default:
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	return (__RESULT_SUCCESS);
}
static int test_2_2_1_top(int child)
{
	return test_2_2_top(child, __TEST_TI_GETINFO);
}
static int test_2_2_1_bot(int child)
{
	return test_2_2_bot(child, __TEST_INFO_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_1_top = { &preamble_0, &test_2_2_1_top, &postamble_0 };
struct test_stream test_case_2_2_1_bot = { &preamble_0, &test_2_2_1_bot, &postamble_0 };
#define test_case_2_2_1_stream_top (&test_case_2_2_1_top)
#define test_case_2_2_1_stream_bot (&test_case_2_2_1_bot)

#define tgrp_case_2_2_2 test_group_2
#define name_case_2_2_2 "TI_OPTMGMT IO control negative acknowledgement"
#define desc_case_2_2_2 "\
This test case test the execution of the TI_OPTMGMT IO control with negative\n\
results on using the timod module."
static int test_2_2_2_top(int child)
{
	return test_2_2_top(child, __TEST_TI_OPTMGMT);
}
static int test_2_2_2_bot(int child)
{
	return test_2_2_bot(child, __TEST_OPTMGMT_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_2_top = { &preamble_0, &test_2_2_2_top, &postamble_0 };
struct test_stream test_case_2_2_2_bot = { &preamble_0, &test_2_2_2_bot, &postamble_0 };
#define test_case_2_2_2_stream_top (&test_case_2_2_2_top)
#define test_case_2_2_2_stream_bot (&test_case_2_2_2_bot)

#define tgrp_case_2_2_3 test_group_2
#define name_case_2_2_3 "TI_BIND IO control negative acknowledgement"
#define desc_case_2_2_3 "\
This test case test the execution of the TI_BIND IO control with negative\n\
results on using the timod module."
static int test_2_2_3_top(int child)
{
	return test_2_2_top(child, __TEST_TI_BIND);
}
static int test_2_2_3_bot(int child)
{
	return test_2_2_bot(child, __TEST_BIND_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_3_top = { &preamble_0, &test_2_2_3_top, &postamble_0 };
struct test_stream test_case_2_2_3_bot = { &preamble_0, &test_2_2_3_bot, &postamble_0 };
#define test_case_2_2_3_stream_top (&test_case_2_2_3_top)
#define test_case_2_2_3_stream_bot (&test_case_2_2_3_bot)

#define tgrp_case_2_2_4 test_group_2
#define name_case_2_2_4 "TI_UNBIND IO control negative acknowledgement"
#define desc_case_2_2_4 "\
This test case test the execution of the TI_UNBIND IO control with negative\n\
results on using the timod module."
static int test_2_2_4_top(int child)
{
	return test_2_2_top(child, __TEST_TI_UNBIND);
}
static int test_2_2_4_bot(int child)
{
	return test_2_2_bot(child, __TEST_UNBIND_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_4_top = { &preamble_0, &test_2_2_4_top, &postamble_0 };
struct test_stream test_case_2_2_4_bot = { &preamble_0, &test_2_2_4_bot, &postamble_0 };
#define test_case_2_2_4_stream_top (&test_case_2_2_4_top)
#define test_case_2_2_4_stream_bot (&test_case_2_2_4_bot)

#define tgrp_case_2_2_5 test_group_2
#define name_case_2_2_5 "TI_GETMYNAME IO control negative acknowledgement"
#define desc_case_2_2_5 "\
This test case test the execution of the TI_GETMYNAME IO control with negative\n\
results on using the timod module."
static int test_2_2_5_top(int child)
{
	return test_2_2_top(child, __TEST_TI_GETMYNAME);
}
static int test_2_2_5_bot(int child)
{
	return test_2_2_bot(child, __TEST_ADDR_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_5_top = { &preamble_0, &test_2_2_5_top, &postamble_0 };
struct test_stream test_case_2_2_5_bot = { &preamble_0, &test_2_2_5_bot, &postamble_0 };
#define test_case_2_2_5_stream_top (&test_case_2_2_5_top)
#define test_case_2_2_5_stream_bot (&test_case_2_2_5_bot)

#define tgrp_case_2_2_6 test_group_2
#define name_case_2_2_6 "TI_GETPEERNAME IO control negative acknowledgement"
#define desc_case_2_2_6 "\
This test case test the execution of the TI_GETPEERNAME IO control with negative\n\
results on using the timod module."
static int test_2_2_6_top(int child)
{
	return test_2_2_top(child, __TEST_TI_GETPEERNAME);
}
static int test_2_2_6_bot(int child)
{
	return test_2_2_bot(child, __TEST_ADDR_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_6_top = { &preamble_0, &test_2_2_6_top, &postamble_0 };
struct test_stream test_case_2_2_6_bot = { &preamble_0, &test_2_2_6_bot, &postamble_0 };
#define test_case_2_2_6_stream_top (&test_case_2_2_6_top)
#define test_case_2_2_6_stream_bot (&test_case_2_2_6_bot)

#define tgrp_case_2_2_7_1 test_group_2
#define name_case_2_2_7_1 "TI_SETMYNAME IO control negative acknowledgement"
#define desc_case_2_2_7_1 "\
This test case test the execution of the TI_SETMYNAME IO control with negative\n\
results on using the timod module."
static int test_2_2_7_1_top(int child)
{
	return test_2_2_top(child, __TEST_TI_SETMYNAME);
}
static int test_2_2_7_1_bot(int child)
{
	return test_2_2_bot(child, __TEST_CONN_RES, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_7_1_top = { &preamble_0, &test_2_2_7_1_top, &postamble_0 };
struct test_stream test_case_2_2_7_1_bot = { &preamble_0, &test_2_2_7_1_bot, &postamble_0 };
#define test_case_2_2_7_1_stream_top (&test_case_2_2_7_1_top)
#define test_case_2_2_7_1_stream_bot (&test_case_2_2_7_1_bot)

#define tgrp_case_2_2_7_2 test_group_2
#define name_case_2_2_7_2 "TI_SETMYNAME (disconnect) IO control negative acknowledgement"
#define desc_case_2_2_7_2 "\
This test case test the execution of the TI_SETMYNAME (disconnect) IO control\n\
with negative results on using the timod module."
static int test_2_2_7_2_top(int child)
{
	return test_2_2_top(child, __TEST_TI_SETMYNAME_DISC);
}
static int test_2_2_7_2_bot(int child)
{
	return test_2_2_bot(child, __TEST_DISCON_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_7_2_top = { &preamble_0, &test_2_2_7_2_top, &postamble_0 };
struct test_stream test_case_2_2_7_2_bot = { &preamble_0, &test_2_2_7_2_bot, &postamble_0 };
#define test_case_2_2_7_2_stream_top (&test_case_2_2_7_2_top)
#define test_case_2_2_7_2_stream_bot (&test_case_2_2_7_2_bot)

#define tgrp_case_2_2_8_1 test_group_2
#define name_case_2_2_8_1 "TI_SETPEERNAME IO control negative acknowledgement"
#define desc_case_2_2_8_1 "\
This test case test the execution of the TI_SETPEERNAME IO control with negative\n\
results on using the timod module."
static int test_2_2_8_1_top(int child)
{
	return test_2_2_top(child, __TEST_TI_SETPEERNAME);
}
static int test_2_2_8_1_bot(int child)
{
	return test_2_2_bot(child, __TEST_CONN_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_8_1_top = { &preamble_0, &test_2_2_8_1_top, &postamble_0 };
struct test_stream test_case_2_2_8_1_bot = { &preamble_0, &test_2_2_8_1_bot, &postamble_0 };
#define test_case_2_2_8_1_stream_top (&test_case_2_2_8_1_top)
#define test_case_2_2_8_1_stream_bot (&test_case_2_2_8_1_bot)

#define tgrp_case_2_2_8_2 test_group_2
#define name_case_2_2_8_2 "TI_SETPEERNAME (disconnect) IO control negative acknowledgement"
#define desc_case_2_2_8_2 "\
This test case test the execution of the TI_SETPEERNAME (disconnect) IO\n\
control with negative results on using the timod module."
static int test_2_2_8_2_top(int child)
{
	return test_2_2_top(child, __TEST_TI_SETPEERNAME_DISC);
}
static int test_2_2_8_2_bot(int child)
{
	return test_2_2_bot(child, __TEST_DISCON_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_8_2_top = { &preamble_0, &test_2_2_8_2_top, &postamble_0 };
struct test_stream test_case_2_2_8_2_bot = { &preamble_0, &test_2_2_8_2_bot, &postamble_0 };
#define test_case_2_2_8_2_stream_top (&test_case_2_2_8_2_top)
#define test_case_2_2_8_2_stream_bot (&test_case_2_2_8_2_bot)

#if 0
#define tgrp_case_2_2_9 test_group_2
#define name_case_2_2_9 "TI_SYNC IO control negative acknowledgement"
#define desc_case_2_2_9 "\
This test case test the execution of the TI_SYNC IO control with negative\n\
results on using the timod module."
static int test_2_2_9_top(int child)
{
	return test_2_2_top(child, __TEST_TI_SYNC);
}
static int test_2_2_9_bot(int child)
{
	return test_2_2_bot(child, __TEST_INFO_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_9_top = { &preamble_0, &test_2_2_9_top, &postamble_0 };
struct test_stream test_case_2_2_9_bot = { &preamble_0, &test_2_2_9_bot, &postamble_0 };
#define test_case_2_2_9_stream_top (&test_case_2_2_9_top)
#define test_case_2_2_9_stream_bot (&test_case_2_2_9_bot)
#endif

#define tgrp_case_2_2_10 test_group_2
#define name_case_2_2_10 "TI_GETADDRS IO control negative acknowledgement"
#define desc_case_2_2_10 "\
This test case test the execution of the TI_GETADDRS IO control with negative\n\
results on using the timod module."
static int test_2_2_10_top(int child)
{
	return test_2_2_top(child, __TEST_TI_GETADDRS);
}
static int test_2_2_10_bot(int child)
{
	return test_2_2_bot(child, __TEST_ADDR_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_10_top = { &preamble_0, &test_2_2_10_top, &postamble_0 };
struct test_stream test_case_2_2_10_bot = { &preamble_0, &test_2_2_10_bot, &postamble_0 };
#define test_case_2_2_10_stream_top (&test_case_2_2_10_top)
#define test_case_2_2_10_stream_bot (&test_case_2_2_10_bot)

#define tgrp_case_2_2_11 test_group_2
#define name_case_2_2_11 "TI_CAPABILITY IO control negative acknowledgement"
#define desc_case_2_2_11 "\
This test case test the execution of the TI_CAPABILITY IO control with negative\n\
results on using the timod module."
static int test_2_2_11_top(int child)
{
	return test_2_2_top(child, __TEST_TI_CAPABILITY);
}
static int test_2_2_11_bot(int child)
{
	return test_2_2_bot(child, __TEST_CAPABILITY_REQ, __TEST_ERROR_ACK);
}
struct test_stream test_case_2_2_11_top = { &preamble_0, &test_2_2_11_top, &postamble_0 };
struct test_stream test_case_2_2_11_bot = { &preamble_0, &test_2_2_11_bot, &postamble_0 };
#define test_case_2_2_11_stream_top (&test_case_2_2_11_top)
#define test_case_2_2_11_stream_bot (&test_case_2_2_11_bot)

#define test_group_3 "Multiple process IO controls"

#define tgrp_case_2_3 test_group_3
#define name_case_2_3 "Two child processes attempting same IO control"
#define desc_case_2_3 "\
This test case tests two child processes attempting the same IO control.  One\n\
is given a positive acknowledgement and the other a negative acknowledgement."
static int test_2_3_top(int child)
{
	pid_t fork_child;
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	switch ((fork_child = fork())) {
	case -1:		/* error */
		state++;
		return (__RESULT_FAILURE);
	case 00:		/* the fork_child */
		state++;
		if (do_signal(child, __TEST_TI_GETINFO) == __RESULT_SUCCESS || last_errno == EPROTO)
			exit(__RESULT_SUCCESS);
		state++;
		exit(__RESULT_FAILURE);
	default:		/* the parent */
	{
		int status = 0;
		int result;
		state++;
		if ((result = do_signal(child, __TEST_TI_GETINFO)) != __RESULT_SUCCESS && last_errno != EPROTO)
			return (__RESULT_FAILURE);
		state++;
		if (waitpid(fork_child, &status, WUNTRACED) == -1) {
			state++;
			return (__RESULT_FAILURE);
		}
		state++;
		if (WIFEXITED(status)) {
			int cresult = WEXITSTATUS(status);
			state++;
			if ((result == __RESULT_SUCCESS && cresult == __RESULT_FAILURE) || (cresult == __RESULT_SUCCESS && result == __RESULT_FAILURE))
				break;
			state++;
			return (__RESULT_FAILURE);
		}
		state++;
		return (__RESULT_FAILURE);
	}
	}
	state++;
	if (do_signal(child, __TEST_POP) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
static int test_2_3_bot(int child)
{
	start_tt(200);
	state++;
	switch (get_event(child)) {
	case __TEST_INFO_REQ:
		state++;
		break;
	case __EVENT_TIMEOUT:
		state++;
		return (__RESULT_FAILURE);
	default:
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	start_tt(100);
	switch (get_event(child)) {
	case __EVENT_TIMEOUT:
		state++;
		break;
	case __TEST_INFO_REQ:
		state++;
		return (__RESULT_FAILURE);
	default:
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	if (do_signal(child, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	switch (get_event(child)) {
	case __TEST_INFO_REQ:
		state++;
		break;
	case __EVENT_TIMEOUT:
		state++;
		return (__RESULT_FAILURE);
	default:
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	if (do_signal(child, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	switch (get_event(child)) {
	case __EVENT_TIMEOUT:
		state++;
		break;
	default:
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	return (__RESULT_SUCCESS);
}
struct test_stream test_case_2_3_top = { &preamble_0, &test_2_3_top, &postamble_0 };
struct test_stream test_case_2_3_bot = { &preamble_0, &test_2_3_bot, &postamble_0 };
#define test_case_2_3_stream_top (&test_case_2_3_top)
#define test_case_2_3_stream_bot (&test_case_2_3_bot)

#define test_group_4 "IO controls with data"

#define tgrp_case_2_4_1 test_group_4
#define name_case_2_4_1 "TI_SETMYNAME with data"
#define desc_case_2_4_1 "\
Tests that IO controls with optional data parts can be sent with data.\n\
This test case tests the TI_SETMYNAME IO control."
static int test_2_4_top(int child, int control)
{
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, control) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, __TEST_POP) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
static int test_2_4_bot(int child, int signal, int expect)
{
	start_tt(200);
	state++;
	if (get_event(child) != signal)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, expect) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	state++;
	switch (get_event(child)) {
	case __TEST_DISCON_REQ:
	case __EVENT_TIMEOUT:
		break;
	default:
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	return (__RESULT_SUCCESS);
}
static int test_2_4_1_top(int child)
{
	return test_2_4_top(child, __TEST_TI_SETMYNAME_DATA);
}
static int test_2_4_1_bot(int child)
{
	return test_2_4_bot(child, __TEST_CONN_RES, __TEST_OK_ACK);
}
struct test_stream test_case_2_4_1_top = { &preamble_0, &test_2_4_1_top, &postamble_0 };
struct test_stream test_case_2_4_1_bot = { &preamble_0, &test_2_4_1_bot, &postamble_0 };
#define test_case_2_4_1_stream_top (&test_case_2_4_1_top)
#define test_case_2_4_1_stream_bot (&test_case_2_4_1_bot)

#define tgrp_case_2_4_2 test_group_4
#define name_case_2_4_2 "TI_SETPEERNAME with data"
#define desc_case_2_4_2 "\
Tests that IO controls with optional data parts can be sent with data.\n\
This test case tests the TI_SETPEERNAME IO control."
static int test_2_4_2_top(int child)
{
	return test_2_4_top(child, __TEST_TI_SETPEERNAME_DATA);
}
static int test_2_4_2_bot(int child)
{
	return test_2_4_bot(child, __TEST_CONN_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_2_4_2_top = { &preamble_0, &test_2_4_2_top, &postamble_0 };
struct test_stream test_case_2_4_2_bot = { &preamble_0, &test_2_4_2_bot, &postamble_0 };
#define test_case_2_4_2_stream_top (&test_case_2_4_2_top)
#define test_case_2_4_2_stream_bot (&test_case_2_4_2_bot)

#define tgrp_case_2_4_3 test_group_4
#define name_case_2_4_3 "TI_SETMYNAME (disconnect) with data"
#define desc_case_2_4_3 "\
Tests that IO controls with optional data parts can be sent with data.\n\
This test case tests the TI_SETMYNAME (disconnect) IO control."
static int test_2_4_3_top(int child)
{
	return test_2_4_top(child, __TEST_TI_SETMYNAME_DISC_DATA);
}
static int test_2_4_3_bot(int child)
{
	return test_2_4_bot(child, __TEST_DISCON_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_2_4_3_top = { &preamble_0, &test_2_4_3_top, &postamble_0 };
struct test_stream test_case_2_4_3_bot = { &preamble_0, &test_2_4_3_bot, &postamble_0 };
#define test_case_2_4_3_stream_top (&test_case_2_4_3_top)
#define test_case_2_4_3_stream_bot (&test_case_2_4_3_bot)

#define tgrp_case_2_4_4 test_group_4
#define name_case_2_4_4 "TI_SETPEERNAME (disconnect) with data"
#define desc_case_2_4_4 "\
Tests that IO controls with optional data parts can be sent with data.\n\
This test case tests the TI_SETPEERNAME (disconnect) IO control."
static int test_2_4_4_top(int child)
{
	return test_2_4_top(child, __TEST_TI_SETPEERNAME_DISC_DATA);
}
static int test_2_4_4_bot(int child)
{
	return test_2_4_bot(child, __TEST_DISCON_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_2_4_4_top = { &preamble_0, &test_2_4_4_top, &postamble_0 };
struct test_stream test_case_2_4_4_bot = { &preamble_0, &test_2_4_4_bot, &postamble_0 };
#define test_case_2_4_4_stream_top (&test_case_2_4_4_top)
#define test_case_2_4_4_stream_bot (&test_case_2_4_4_bot)

#define test_group_5 "Non-IO control pass through of TPI messages"

#define tgrp_case_3_1_1 test_group_5
#define name_case_3_1_1 "T_INFO_REQ with positive acknowledgement"
#define desc_case_3_1_1 "\
This test case test the pass through of T_INFO_REQ with positive\n\
acknowledgement."
static int test_3_1_top(int child, int signal, int expect)
{
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, signal) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	if (get_event(child) == expect) {
		state++;
		if (do_signal(child, __TEST_POP) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		return (__RESULT_SUCCESS);
	} else if (last_event == __EVENT_TIMEOUT) {
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	return (__RESULT_FAILURE);
}
static int test_3_1_bot(int child, int signal, int expect)
{
	start_tt(200);
	state++;
	if (get_event(child) == signal) {
		state++;
	} else if (last_event == __EVENT_TIMEOUT) {
		state++;
		return (__RESULT_FAILURE);
	} else {
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	if (do_signal(child, expect) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	state++;
	pause();
	state++;
	if (get_event(child) != __EVENT_TIMEOUT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
static int test_3_1_1_top(int child)
{
	return test_3_1_top(child, __TEST_INFO_REQ, __TEST_INFO_ACK);
}
static int test_3_1_1_bot(int child)
{
	return test_3_1_bot(child, __TEST_INFO_REQ, __TEST_INFO_ACK);
}
struct test_stream test_case_3_1_1_top = { &preamble_0, &test_3_1_1_top, &postamble_0 };
struct test_stream test_case_3_1_1_bot = { &preamble_0, &test_3_1_1_bot, &postamble_0 };
#define test_case_3_1_1_stream_top (&test_case_3_1_1_top)
#define test_case_3_1_1_stream_bot (&test_case_3_1_1_bot)

#define tgrp_case_3_1_2 test_group_5
#define name_case_3_1_2 "T_CONN_REQ with positive acknowledgement"
#define desc_case_3_1_2 "\
This test case test the pass through of T_CONN_REQ with positive\n\
acknowledgement."
static int test_3_1_2_top(int child)
{
	return test_3_1_top(child, __TEST_CONN_REQ, __TEST_OK_ACK);
}
static int test_3_1_2_bot(int child)
{
	return test_3_1_bot(child, __TEST_CONN_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_3_1_2_top = { &preamble_0, &test_3_1_2_top, &postamble_0 };
struct test_stream test_case_3_1_2_bot = { &preamble_0, &test_3_1_2_bot, &postamble_0 };
#define test_case_3_1_2_stream_top (&test_case_3_1_2_top)
#define test_case_3_1_2_stream_bot (&test_case_3_1_2_bot)

#define tgrp_case_3_1_3 test_group_5
#define name_case_3_1_3 "T_CONN_RES with positive acknowledgement"
#define desc_case_3_1_3 "\
This test case test the pass through of T_CONN_RES with positive\n\
acknowledgement."
static int test_3_1_3_top(int child)
{
	return test_3_1_top(child, __TEST_CONN_RES, __TEST_OK_ACK);
}
static int test_3_1_3_bot(int child)
{
	return test_3_1_bot(child, __TEST_CONN_RES, __TEST_OK_ACK);
}
struct test_stream test_case_3_1_3_top = { &preamble_0, &test_3_1_3_top, &postamble_0 };
struct test_stream test_case_3_1_3_bot = { &preamble_0, &test_3_1_3_bot, &postamble_0 };
#define test_case_3_1_3_stream_top (&test_case_3_1_3_top)
#define test_case_3_1_3_stream_bot (&test_case_3_1_3_bot)

#define tgrp_case_3_1_4 test_group_5
#define name_case_3_1_4 "T_DISCON_REQ with positive acknowledgement"
#define desc_case_3_1_4 "\
This test case test the pass through of T_DISCON_REQ with positive\n\
acknowledgement."
static int test_3_1_4_top(int child)
{
	return test_3_1_top(child, __TEST_DISCON_REQ, __TEST_OK_ACK);
}
static int test_3_1_4_bot(int child)
{
	return test_3_1_bot(child, __TEST_DISCON_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_3_1_4_top = { &preamble_0, &test_3_1_4_top, &postamble_0 };
struct test_stream test_case_3_1_4_bot = { &preamble_0, &test_3_1_4_bot, &postamble_0 };
#define test_case_3_1_4_stream_top (&test_case_3_1_4_top)
#define test_case_3_1_4_stream_bot (&test_case_3_1_4_bot)

#define tgrp_case_3_1_5 test_group_5
#define name_case_3_1_5 "T_BIND_REQ with positive acknowledgement"
#define desc_case_3_1_5 "\
This test case test the pass through of T_BIND_REQ with positive\n\
acknowledgement."
static int test_3_1_5_top(int child)
{
	return test_3_1_top(child, __TEST_BIND_REQ, __TEST_BIND_ACK);
}
static int test_3_1_5_bot(int child)
{
	return test_3_1_bot(child, __TEST_BIND_REQ, __TEST_BIND_ACK);
}
struct test_stream test_case_3_1_5_top = { &preamble_0, &test_3_1_5_top, &postamble_0 };
struct test_stream test_case_3_1_5_bot = { &preamble_0, &test_3_1_5_bot, &postamble_0 };
#define test_case_3_1_5_stream_top (&test_case_3_1_5_top)
#define test_case_3_1_5_stream_bot (&test_case_3_1_5_bot)

#define tgrp_case_3_1_6 test_group_5
#define name_case_3_1_6 "T_UNBIND_REQ with positive acknowledgement"
#define desc_case_3_1_6 "\
This test case test the pass through of T_UNBIND_REQ with positive\n\
acknowledgement."
static int test_3_1_6_top(int child)
{
	return test_3_1_top(child, __TEST_UNBIND_REQ, __TEST_OK_ACK);
}
static int test_3_1_6_bot(int child)
{
	return test_3_1_bot(child, __TEST_UNBIND_REQ, __TEST_OK_ACK);
}
struct test_stream test_case_3_1_6_top = { &preamble_0, &test_3_1_6_top, &postamble_0 };
struct test_stream test_case_3_1_6_bot = { &preamble_0, &test_3_1_6_bot, &postamble_0 };
#define test_case_3_1_6_stream_top (&test_case_3_1_6_top)
#define test_case_3_1_6_stream_bot (&test_case_3_1_6_bot)

#define tgrp_case_3_1_7 test_group_5
#define name_case_3_1_7 "T_OPTMGMT_REQ with positive acknowledgement"
#define desc_case_3_1_7 "\
This test case test the pass through of T_OPTMGMT_REQ with positive\n\
acknowledgement."
static int test_3_1_7_top(int child)
{
	return test_3_1_top(child, __TEST_OPTMGMT_REQ, __TEST_OPTMGMT_ACK);
}
static int test_3_1_7_bot(int child)
{
	return test_3_1_bot(child, __TEST_OPTMGMT_REQ, __TEST_OPTMGMT_ACK);
}
struct test_stream test_case_3_1_7_top = { &preamble_0, &test_3_1_7_top, &postamble_0 };
struct test_stream test_case_3_1_7_bot = { &preamble_0, &test_3_1_7_bot, &postamble_0 };
#define test_case_3_1_7_stream_top (&test_case_3_1_7_top)
#define test_case_3_1_7_stream_bot (&test_case_3_1_7_bot)

#define tgrp_case_3_1_8 test_group_5
#define name_case_3_1_8 "T_ADDR_REQ with positive acknowledgement"
#define desc_case_3_1_8 "\
This test case test the pass through of T_ADDR_REQ with positive\n\
acknowledgement."
static int test_3_1_8_top(int child)
{
	return test_3_1_top(child, __TEST_ADDR_REQ, __TEST_ADDR_ACK);
}
static int test_3_1_8_bot(int child)
{
	return test_3_1_bot(child, __TEST_ADDR_REQ, __TEST_ADDR_ACK);
}
struct test_stream test_case_3_1_8_top = { &preamble_0, &test_3_1_8_top, &postamble_0 };
struct test_stream test_case_3_1_8_bot = { &preamble_0, &test_3_1_8_bot, &postamble_0 };
#define test_case_3_1_8_stream_top (&test_case_3_1_8_top)
#define test_case_3_1_8_stream_bot (&test_case_3_1_8_bot)

#define tgrp_case_3_1_9 test_group_5
#define name_case_3_1_9 "T_CAPABILITY_REQ with positive acknowledgement"
#define desc_case_3_1_9 "\
This test case test the pass through of T_CAPABILITY_REQ with positive\n\
acknowledgement."
static int test_3_1_9_top(int child)
{
	return test_3_1_top(child, __TEST_CAPABILITY_REQ, __TEST_CAPABILITY_ACK);
}
static int test_3_1_9_bot(int child)
{
	return test_3_1_bot(child, __TEST_CAPABILITY_REQ, __TEST_CAPABILITY_ACK);
}
struct test_stream test_case_3_1_9_top = { &preamble_0, &test_3_1_9_top, &postamble_0 };
struct test_stream test_case_3_1_9_bot = { &preamble_0, &test_3_1_9_bot, &postamble_0 };
#define test_case_3_1_9_stream_top (&test_case_3_1_9_top)
#define test_case_3_1_9_stream_bot (&test_case_3_1_9_bot)

#define tgrp_case_3_2_1 test_group_5
#define name_case_3_2_1 "T_INFO_REQ with negative acknowledgement"
#define desc_case_3_2_1 "\
This test case test the pass through of T_INFO_REQ with negative\n\
acknowledgement."
static int test_3_2_top(int child, int signal)
{
	if (do_signal(child, __TEST_PUSH) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	if (do_signal(child, signal) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	if (get_event(child) == __TEST_ERROR_ACK) {
		state++;
		if (do_signal(child, __TEST_POP) != __RESULT_SUCCESS)
			return (__RESULT_FAILURE);
		state++;
		return (__RESULT_SUCCESS);
	} else if (last_event == __EVENT_TIMEOUT) {
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	return (__RESULT_FAILURE);
}
static int test_3_2_bot(int child, int signal)
{
	start_tt(200);
	state++;
	if (get_event(child) == signal) {
		state++;
	} else if (last_event == __EVENT_TIMEOUT) {
		state++;
		return (__RESULT_FAILURE);
	} else {
		state++;
		return (__RESULT_FAILURE);
	}
	state++;
	if (do_signal(child, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		return (__RESULT_FAILURE);
	state++;
	start_tt(200);
	state++;
	pause();
	state++;
	if (get_event(child) != __EVENT_TIMEOUT)
		return (__RESULT_FAILURE);
	state++;
	return (__RESULT_SUCCESS);
}
static int test_3_2_1_top(int child)
{
	return test_3_2_top(child, __TEST_INFO_REQ);
}
static int test_3_2_1_bot(int child)
{
	return test_3_2_bot(child, __TEST_INFO_REQ);
}
struct test_stream test_case_3_2_1_top = { &preamble_0, &test_3_2_1_top, &postamble_0 };
struct test_stream test_case_3_2_1_bot = { &preamble_0, &test_3_2_1_bot, &postamble_0 };
#define test_case_3_2_1_stream_top (&test_case_3_2_1_top)
#define test_case_3_2_1_stream_bot (&test_case_3_2_1_bot)

#define tgrp_case_3_2_2 test_group_5
#define name_case_3_2_2 "T_CONN_REQ with negative acknowledgement"
#define desc_case_3_2_2 "\
This test case test the pass through of T_CONN_REQ with negative\n\
acknowledgement."
static int test_3_2_2_top(int child)
{
	return test_3_2_top(child, __TEST_CONN_REQ);
}
static int test_3_2_2_bot(int child)
{
	return test_3_2_bot(child, __TEST_CONN_REQ);
}
struct test_stream test_case_3_2_2_top = { &preamble_0, &test_3_2_2_top, &postamble_0 };
struct test_stream test_case_3_2_2_bot = { &preamble_0, &test_3_2_2_bot, &postamble_0 };
#define test_case_3_2_2_stream_top (&test_case_3_2_2_top)
#define test_case_3_2_2_stream_bot (&test_case_3_2_2_bot)

#define tgrp_case_3_2_3 test_group_5
#define name_case_3_2_3 "T_CONN_RES with negative acknowledgement"
#define desc_case_3_2_3 "\
This test case test the pass through of T_CONN_RES with negative\n\
acknowledgement."
static int test_3_2_3_top(int child)
{
	return test_3_2_top(child, __TEST_CONN_RES);
}
static int test_3_2_3_bot(int child)
{
	return test_3_2_bot(child, __TEST_CONN_RES);
}
struct test_stream test_case_3_2_3_top = { &preamble_0, &test_3_2_3_top, &postamble_0 };
struct test_stream test_case_3_2_3_bot = { &preamble_0, &test_3_2_3_bot, &postamble_0 };
#define test_case_3_2_3_stream_top (&test_case_3_2_3_top)
#define test_case_3_2_3_stream_bot (&test_case_3_2_3_bot)

#define tgrp_case_3_2_4 test_group_5
#define name_case_3_2_4 "T_DISCON_REQ with negative acknowledgement"
#define desc_case_3_2_4 "\
This test case test the pass through of T_DISCON_REQ with negative\n\
acknowledgement."
static int test_3_2_4_top(int child)
{
	return test_3_2_top(child, __TEST_DISCON_REQ);
}
static int test_3_2_4_bot(int child)
{
	return test_3_2_bot(child, __TEST_DISCON_REQ);
}
struct test_stream test_case_3_2_4_top = { &preamble_0, &test_3_2_4_top, &postamble_0 };
struct test_stream test_case_3_2_4_bot = { &preamble_0, &test_3_2_4_bot, &postamble_0 };
#define test_case_3_2_4_stream_top (&test_case_3_2_4_top)
#define test_case_3_2_4_stream_bot (&test_case_3_2_4_bot)

#define tgrp_case_3_2_5 test_group_5
#define name_case_3_2_5 "T_BIND_REQ with negative acknowledgement"
#define desc_case_3_2_5 "\
This test case test the pass through of T_BIND_REQ with negative\n\
acknowledgement."
static int test_3_2_5_top(int child)
{
	return test_3_2_top(child, __TEST_BIND_REQ);
}
static int test_3_2_5_bot(int child)
{
	return test_3_2_bot(child, __TEST_BIND_REQ);
}
struct test_stream test_case_3_2_5_top = { &preamble_0, &test_3_2_5_top, &postamble_0 };
struct test_stream test_case_3_2_5_bot = { &preamble_0, &test_3_2_5_bot, &postamble_0 };
#define test_case_3_2_5_stream_top (&test_case_3_2_5_top)
#define test_case_3_2_5_stream_bot (&test_case_3_2_5_bot)

#define tgrp_case_3_2_6 test_group_5
#define name_case_3_2_6 "T_UNBIND_REQ with negative acknowledgement"
#define desc_case_3_2_6 "\
This test case test the pass through of T_UNBIND_REQ with negative\n\
acknowledgement."
static int test_3_2_6_top(int child)
{
	return test_3_2_top(child, __TEST_UNBIND_REQ);
}
static int test_3_2_6_bot(int child)
{
	return test_3_2_bot(child, __TEST_UNBIND_REQ);
}
struct test_stream test_case_3_2_6_top = { &preamble_0, &test_3_2_6_top, &postamble_0 };
struct test_stream test_case_3_2_6_bot = { &preamble_0, &test_3_2_6_bot, &postamble_0 };
#define test_case_3_2_6_stream_top (&test_case_3_2_6_top)
#define test_case_3_2_6_stream_bot (&test_case_3_2_6_bot)

#define tgrp_case_3_2_7 test_group_5
#define name_case_3_2_7 "T_OPTMGMT_REQ with negative acknowledgement"
#define desc_case_3_2_7 "\
This test case test the pass through of T_OPTMGMT_REQ with negative\n\
acknowledgement."
static int test_3_2_7_top(int child)
{
	return test_3_2_top(child, __TEST_OPTMGMT_REQ);
}
static int test_3_2_7_bot(int child)
{
	return test_3_2_bot(child, __TEST_OPTMGMT_REQ);
}
struct test_stream test_case_3_2_7_top = { &preamble_0, &test_3_2_7_top, &postamble_0 };
struct test_stream test_case_3_2_7_bot = { &preamble_0, &test_3_2_7_bot, &postamble_0 };
#define test_case_3_2_7_stream_top (&test_case_3_2_7_top)
#define test_case_3_2_7_stream_bot (&test_case_3_2_7_bot)

#define tgrp_case_3_2_8 test_group_5
#define name_case_3_2_8 "T_ADDR_REQ with negative acknowledgement"
#define desc_case_3_2_8 "\
This test case test the pass through of T_ADDR_REQ with negative\n\
acknowledgement."
static int test_3_2_8_top(int child)
{
	return test_3_2_top(child, __TEST_ADDR_REQ);
}
static int test_3_2_8_bot(int child)
{
	return test_3_2_bot(child, __TEST_ADDR_REQ);
}
struct test_stream test_case_3_2_8_top = { &preamble_0, &test_3_2_8_top, &postamble_0 };
struct test_stream test_case_3_2_8_bot = { &preamble_0, &test_3_2_8_bot, &postamble_0 };
#define test_case_3_2_8_stream_top (&test_case_3_2_8_top)
#define test_case_3_2_8_stream_bot (&test_case_3_2_8_bot)

#define tgrp_case_3_2_9 test_group_5
#define name_case_3_2_9 "T_CAPABILITY_REQ with negative acknowledgement"
#define desc_case_3_2_9 "\
This test case test the pass through of T_CAPABILITY_REQ with negative\n\
acknowledgement."
static int test_3_2_9_top(int child)
{
	return test_3_2_top(child, __TEST_CAPABILITY_REQ);
}
static int test_3_2_9_bot(int child)
{
	return test_3_2_bot(child, __TEST_CAPABILITY_REQ);
}
struct test_stream test_case_3_2_9_top = { &preamble_0, &test_3_2_9_top, &postamble_0 };
struct test_stream test_case_3_2_9_bot = { &preamble_0, &test_3_2_9_bot, &postamble_0 };
#define test_case_3_2_9_stream_top (&test_case_3_2_9_top)
#define test_case_3_2_9_stream_bot (&test_case_3_2_9_bot)

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
 *  The top child (child[0]) is the process running above the tested module, the
 *  bot child (child[1]) is the process running below (at other end of pipe) the
 *  tested module.
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
				print_timeout(3);
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
	const char *tgrp;		/* test case group */
	const char *name;		/* test case name */
	const char *desc;		/* test case description */
	struct test_stream *stream[3];	/* test streams */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} tests[] = {
	{
		"1.1", tgrp_case_1_1, name_case_1_1, desc_case_1_1, {
	test_case_1_1_stream_top, test_case_1_1_stream_bot, NULL}, 0, 0}, {
		"2.1.1", tgrp_case_2_1_1, name_case_2_1_1, desc_case_2_1_1, {
	test_case_2_1_1_stream_top, test_case_2_1_1_stream_bot, NULL}, 0, 0}, {
		"2.1.2", tgrp_case_2_1_2, name_case_2_1_2, desc_case_2_1_2, {
	test_case_2_1_2_stream_top, test_case_2_1_2_stream_bot, NULL}, 0, 0}, {
		"2.1.3", tgrp_case_2_1_3, name_case_2_1_3, desc_case_2_1_3, {
	test_case_2_1_3_stream_top, test_case_2_1_3_stream_bot, NULL}, 0, 0}, {
		"2.1.4", tgrp_case_2_1_4, name_case_2_1_4, desc_case_2_1_4, {
	test_case_2_1_4_stream_top, test_case_2_1_4_stream_bot, NULL}, 0, 0}, {
		"2.1.5", tgrp_case_2_1_5, name_case_2_1_5, desc_case_2_1_5, {
	test_case_2_1_5_stream_top, test_case_2_1_5_stream_bot, NULL}, 0, 0}, {
		"2.1.6", tgrp_case_2_1_6, name_case_2_1_6, desc_case_2_1_6, {
	test_case_2_1_6_stream_top, test_case_2_1_6_stream_bot, NULL}, 0, 0}, {
		"2.1.7.1", tgrp_case_2_1_7_1, name_case_2_1_7_1, desc_case_2_1_7_1, {
	test_case_2_1_7_1_stream_top, test_case_2_1_7_1_stream_bot, NULL}, 0, 0}, {
		"2.1.7.2", tgrp_case_2_1_7_2, name_case_2_1_7_2, desc_case_2_1_7_2, {
	test_case_2_1_7_2_stream_top, test_case_2_1_7_2_stream_bot, NULL}, 0, 0}, {
		"2.1.8.1", tgrp_case_2_1_8_1, name_case_2_1_8_1, desc_case_2_1_8_1, {
	test_case_2_1_8_1_stream_top, test_case_2_1_8_1_stream_bot, NULL}, 0, 0}, {
		"2.1.8.2", tgrp_case_2_1_8_2, name_case_2_1_8_2, desc_case_2_1_8_2, {
	test_case_2_1_8_2_stream_top, test_case_2_1_8_2_stream_bot, NULL}, 0, 0}, {
#if 0
		"2.1.9", tgrp_case_2_1_9, name_case_2_1_9, desc_case_2_1_9, {
	test_case_2_1_9_stream_top, test_case_2_1_9_stream_bot, NULL}, 0, 0}, {
#endif
		"2.1.10", tgrp_case_2_1_10, name_case_2_1_10, desc_case_2_1_10, {
	test_case_2_1_10_stream_top, test_case_2_1_10_stream_bot, NULL}, 0, 0}, {
		"2.1.11", tgrp_case_2_1_11, name_case_2_1_11, desc_case_2_1_11, {
	test_case_2_1_11_stream_top, test_case_2_1_11_stream_bot, NULL}, 0, 0}, {
		"2.2.1", tgrp_case_2_2_1, name_case_2_2_1, desc_case_2_2_1, {
	test_case_2_2_1_stream_top, test_case_2_2_1_stream_bot, NULL}, 0, 0}, {
		"2.2.2", tgrp_case_2_2_2, name_case_2_2_2, desc_case_2_2_2, {
	test_case_2_2_2_stream_top, test_case_2_2_2_stream_bot, NULL}, 0, 0}, {
		"2.2.3", tgrp_case_2_2_3, name_case_2_2_3, desc_case_2_2_3, {
	test_case_2_2_3_stream_top, test_case_2_2_3_stream_bot, NULL}, 0, 0}, {
		"2.2.4", tgrp_case_2_2_4, name_case_2_2_4, desc_case_2_2_4, {
	test_case_2_2_4_stream_top, test_case_2_2_4_stream_bot, NULL}, 0, 0}, {
		"2.2.5", tgrp_case_2_2_5, name_case_2_2_5, desc_case_2_2_5, {
	test_case_2_2_5_stream_top, test_case_2_2_5_stream_bot, NULL}, 0, 0}, {
		"2.2.6", tgrp_case_2_2_6, name_case_2_2_6, desc_case_2_2_6, {
	test_case_2_2_6_stream_top, test_case_2_2_6_stream_bot, NULL}, 0, 0}, {
		"2.2.7.1", tgrp_case_2_2_7_1, name_case_2_2_7_1, desc_case_2_2_7_1, {
	test_case_2_2_7_1_stream_top, test_case_2_2_7_1_stream_bot, NULL}, 0, 0}, {
		"2.2.7.2", tgrp_case_2_2_7_2, name_case_2_2_7_2, desc_case_2_2_7_2, {
	test_case_2_2_7_2_stream_top, test_case_2_2_7_2_stream_bot, NULL}, 0, 0}, {
		"2.2.8.1", tgrp_case_2_2_8_1, name_case_2_2_8_1, desc_case_2_2_8_1, {
	test_case_2_2_8_1_stream_top, test_case_2_2_8_1_stream_bot, NULL}, 0, 0}, {
		"2.2.8.2", tgrp_case_2_2_8_2, name_case_2_2_8_2, desc_case_2_2_8_2, {
	test_case_2_2_8_2_stream_top, test_case_2_2_8_2_stream_bot, NULL}, 0, 0}, {
#if 0
		"2.2.9", tgrp_case_2_2_9, name_case_2_2_9, desc_case_2_2_9, {
	test_case_2_2_9_stream_top, test_case_2_2_9_stream_bot, NULL}, 0, 0}, {
#endif
		"2.2.10", tgrp_case_2_2_10, name_case_2_2_10, desc_case_2_2_10, {
	test_case_2_2_10_stream_top, test_case_2_2_10_stream_bot, NULL}, 0, 0}, {
		"2.2.11", tgrp_case_2_2_11, name_case_2_2_11, desc_case_2_2_11, {
	test_case_2_2_11_stream_top, test_case_2_2_11_stream_bot, NULL}, 0, 0}, {
		"2.3", tgrp_case_2_3, name_case_2_3, desc_case_2_3, {
	test_case_2_3_stream_top, test_case_2_3_stream_bot, NULL}, 0, 0}, {
		"2.4.1", tgrp_case_2_4_1, name_case_2_4_1, desc_case_2_4_1, {
	test_case_2_4_1_stream_top, test_case_2_4_1_stream_bot, NULL}, 0, 0}, {
		"2.4.2", tgrp_case_2_4_2, name_case_2_4_2, desc_case_2_4_2, {
	test_case_2_4_2_stream_top, test_case_2_4_2_stream_bot, NULL}, 0, 0}, {
		"2.4.3", tgrp_case_2_4_3, name_case_2_4_3, desc_case_2_4_3, {
	test_case_2_4_3_stream_top, test_case_2_4_3_stream_bot, NULL}, 0, 0}, {
		"2.4.4", tgrp_case_2_4_4, name_case_2_4_4, desc_case_2_4_4, {
	test_case_2_4_4_stream_top, test_case_2_4_4_stream_bot, NULL}, 0, 0}, {
		"3.1.1", tgrp_case_3_1_1, name_case_3_1_1, desc_case_3_1_1, {
	test_case_3_1_1_stream_top, test_case_3_1_1_stream_bot, NULL}, 0, 0}, {
		"3.1.2", tgrp_case_3_1_2, name_case_3_1_2, desc_case_3_1_2, {
	test_case_3_1_2_stream_top, test_case_3_1_2_stream_bot, NULL}, 0, 0}, {
		"3.1.3", tgrp_case_3_1_3, name_case_3_1_3, desc_case_3_1_3, {
	test_case_3_1_3_stream_top, test_case_3_1_3_stream_bot, NULL}, 0, 0}, {
		"3.1.4", tgrp_case_3_1_4, name_case_3_1_4, desc_case_3_1_4, {
	test_case_3_1_4_stream_top, test_case_3_1_4_stream_bot, NULL}, 0, 0}, {
		"3.1.5", tgrp_case_3_1_5, name_case_3_1_5, desc_case_3_1_5, {
	test_case_3_1_5_stream_top, test_case_3_1_5_stream_bot, NULL}, 0, 0}, {
		"3.1.6", tgrp_case_3_1_6, name_case_3_1_6, desc_case_3_1_6, {
	test_case_3_1_6_stream_top, test_case_3_1_6_stream_bot, NULL}, 0, 0}, {
		"3.1.7", tgrp_case_3_1_7, name_case_3_1_7, desc_case_3_1_7, {
	test_case_3_1_7_stream_top, test_case_3_1_7_stream_bot, NULL}, 0, 0}, {
		"3.1.8", tgrp_case_3_1_8, name_case_3_1_8, desc_case_3_1_8, {
	test_case_3_1_8_stream_top, test_case_3_1_8_stream_bot, NULL}, 0, 0}, {
		"3.1.9", tgrp_case_3_1_9, name_case_3_1_9, desc_case_3_1_9, {
	test_case_3_1_9_stream_top, test_case_3_1_9_stream_bot, NULL}, 0, 0}, {
		"3.2.1", tgrp_case_3_2_1, name_case_3_2_1, desc_case_3_2_1, {
	test_case_3_2_1_stream_top, test_case_3_2_1_stream_bot, NULL}, 0, 0}, {
		"3.2.2", tgrp_case_3_2_2, name_case_3_2_2, desc_case_3_2_2, {
	test_case_3_2_2_stream_top, test_case_3_2_2_stream_bot, NULL}, 0, 0}, {
		"3.2.3", tgrp_case_3_2_3, name_case_3_2_3, desc_case_3_2_3, {
	test_case_3_2_3_stream_top, test_case_3_2_3_stream_bot, NULL}, 0, 0}, {
		"3.2.4", tgrp_case_3_2_4, name_case_3_2_4, desc_case_3_2_4, {
	test_case_3_2_4_stream_top, test_case_3_2_4_stream_bot, NULL}, 0, 0}, {
		"3.2.5", tgrp_case_3_2_5, name_case_3_2_5, desc_case_3_2_5, {
	test_case_3_2_5_stream_top, test_case_3_2_5_stream_bot, NULL}, 0, 0}, {
		"3.2.6", tgrp_case_3_2_6, name_case_3_2_6, desc_case_3_2_6, {
	test_case_3_2_6_stream_top, test_case_3_2_6_stream_bot, NULL}, 0, 0}, {
		"3.2.7", tgrp_case_3_2_7, name_case_3_2_7, desc_case_3_2_7, {
	test_case_3_2_7_stream_top, test_case_3_2_7_stream_bot, NULL}, 0, 0}, {
		"3.2.8", tgrp_case_3_2_8, name_case_3_2_8, desc_case_3_2_8, {
	test_case_3_2_8_stream_top, test_case_3_2_8_stream_bot, NULL}, 0, 0}, {
		"3.2.9", tgrp_case_3_2_9, name_case_3_2_9, desc_case_3_2_9, {
	test_case_3_2_9_stream_top, test_case_3_2_9_stream_bot, NULL}, 0, 0}, {
NULL,}};

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
				if (verbose > 1)
					fprintf(stdout, "\nTest Group: %s", tests[i].tgrp);
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
			if (!(aborted + failures))
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
		} else if (failures) {
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

void copying(int argc, char *argv[])
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

void version(int argc, char *argv[])
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
    See `%1$s --copying' for copying permissions.\n\
\n\
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
    %1$s {-C, --copying}\n\
", argv[0]);
}

void help(int argc, char *argv[])
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
        print copying permissions and exit\n\
\n\
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
	copying(argc, argv);
	do_tests();
	exit(0);
}
