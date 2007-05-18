/*****************************************************************************

 @(#) $RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9.2.29 $) $Date: 2007/05/18 12:05:42 $

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

 Last Modified $Date: 2007/05/18 12:05:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sctp_n.c,v $
 Revision 0.9.2.29  2007/05/18 12:05:42  brian
 - wrap up of sctp testing

 Revision 0.9.2.28  2007/05/18 05:02:28  brian
 - final sctp performance rework

 Revision 0.9.2.27  2007/03/15 10:23:32  brian
 - test case reporting and pushed release date one day

 Revision 0.9.2.26  2007/03/15 02:02:06  brian
 - last known bug fixes, report failed expectations

 Revision 0.9.2.25  2007/03/12 09:35:03  brian
 - boosted default test port numbers from 10000 to 18000

 Revision 0.9.2.24  2007/03/08 08:27:27  brian
 - print primitives at default verbosity

 Revision 0.9.2.23  2006/12/18 07:57:45  brian
 - resolve device numbering

 Revision 0.9.2.22  2006/12/15 00:22:15  brian
 - bufq locking changes and test suite upgrade

 Revision 0.9.2.21  2006/10/30 20:26:32  brian
 - bug hunting

 Revision 0.9.2.20  2006/10/27 23:10:13  brian
 - rationalized to new test suites

 Revision 0.9.2.19  2006/10/21 19:55:39  brian
 - a couple more test case corrections

 Revision 0.9.2.18  2006/10/21 17:00:40  brian
 - fixed test cases, added split client/server operation

 Revision 0.9.2.17  2006/10/21 12:00:36  brian
 - missing checkins

 Revision 0.9.2.16  2006/10/16 00:14:52  brian
 - updates for release and test case passes on UP

 Revision 0.9.2.15  2006/07/29 07:44:24  brian
 - CVS checkin of changes before leaving for SCTP interop

 Revision 0.9.2.14  2006/05/06 10:36:10  brian
 - minor changes

 Revision 0.9.2.13  2006/03/03 11:47:02  brian
 - 32/64-bit compatibility

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9.2.29 $) $Date: 2007/05/18 12:05:42 $"

static char const ident[] = "$RCSfile: test-sctp_n.c,v $ $Name:  $($Revision: 0.9.2.29 $) $Date: 2007/05/18 12:05:42 $";

/*
 *  This file is for testing the sctp_n driver.  It is provided for the
 *  purpose of testing the OpenSS7 sctp_n driver only.
 */

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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "OpenSS7 SCTP Driver - NPI-SCTP";

/* static const char *spkgname = "SCTP"; */
static const char *lstdname = "XNS 5.2/NPI Rev 2";
static const char *sstdname = "XNS/NPI";
static const char *shortname = "SCTP";
#ifdef LFS
static char devname[256] = "/dev/streams/clone/sctp_n";
#else
static char devname[256] = "/dev/sctp_n";
#endif

static int repeat_verbose = 0;
static int repeat_on_success = 0;
static int repeat_on_failure = 0;
static int exit_on_failure = 0;

static int client_port_specified = 0;
static int server_port_specified = 0;
static int client_host_specified = 0;
static int server_host_specified = 0;

static int verbose = 1;

static int client_exec = 0; /* execute client side */
static int server_exec = 0; /* execute server side */

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;
//static int show_data = 1;

static int PRIM_type = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;
static int last_prio = 0;

static int NPI_error = 0;
static int CONIND_number = 2;
static int TOKEN_value = 0;
static int SEQ_number = 1;
static int SERV_type = N_CLNS;
static int CURRENT_state = NS_UNBND;
N_info_ack_t last_info = { 0, };

static int DATA_xfer_flags = 0;
static int BIND_flags = 0;
static int RESET_orig = N_UNDEFINED;
static int RESET_reason = 0;
static int DISCON_reason = 0;
static int CONN_flags = 0;
static int ERROR_type = 0;
static int RESERVED_field[2] = { 0, 0 };

#define TEST_PROTOCOL 132

int test_fd[3] = { 0, 0, 0 };

#define BUFSIZE 32*4096

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 100	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define LONGEST_WAIT	5000	// 20000 // 10000
#define TEST_DURATION	20000
#define INFINITE_WAIT	-1

static ulong test_duration = TEST_DURATION;	/* wait on other side */

ulong seq[10] = { 0, };
ulong tok[10] = { 0, };
ulong tsn[10] = { 0, };
ulong sid[10] = { 0, };
ulong ssn[10] = { 0, };
ulong ppi[10] = { 0, };
ulong exc[10] = { 0, };

char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strbuf ctrl = { BUFSIZE, -1, cbuf };
struct strbuf data = { BUFSIZE, -1, dbuf };

static int test_pflags = MSG_BAND;	/* MSG_BAND | MSG_HIPRI */
static int test_pband = 0;
static int test_gflags = 0;		/* MSG_BAND | MSG_HIPRI */
static int test_gband = 0;
static int test_timout = 200;

static int test_bufsize = 256;
static int test_nidu = 256;
static int OPTMGMT_flags = 0;
static struct sockaddr_in *ADDR_buffer = NULL;
static socklen_t ADDR_length = sizeof(*ADDR_buffer);
static struct sockaddr_in *DEST_buffer = NULL;
static socklen_t DEST_length = 0;
static struct sockaddr_in *SRC_buffer = NULL;
static socklen_t SRC_length = 0;
static unsigned char *PROTOID_buffer = NULL;
static size_t PROTOID_length = 0;
static char *DATA_buffer = NULL;
static size_t DATA_length = 0;
static int test_resfd = -1;
static void *QOS_buffer = NULL;
static int QOS_length = 0;

struct strfdinsert fdi = {
	{BUFSIZE, 0, cbuf},
	{BUFSIZE, 0, dbuf},
	0,
	0,
	0
};
int flags = 0;

int dummy = 0;

#if 1
#ifndef SCTP_VERSION_2
typedef struct addr {
	uint16_t port __attribute__ ((packed));
	struct in_addr addr[3] __attribute__ ((packed));
} addr_t;
#endif				/* SCTP_VERSION_2 */
#endif

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

enum {
	__TEST_CONN_REQ = 100, __TEST_CONN_RES, __TEST_DISCON_REQ,
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
	__TEST_DATACK_REQ, __TEST_DATACK_IND, __TEST_RESET_REQ,
	__TEST_RESET_IND, __TEST_RESET_RES, __TEST_RESET_CON,
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
	__TEST_PRIM_TOO_SHORT, __TEST_PRIM_WAY_TOO_SHORT,
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

static int state = 0;
static const char *failure_string = NULL;

#define __stringify_1(x) #x
#define __stringify(x) __stringify_1(x)
#define FAILURE_STRING(string) "[" __stringify(__LINE__) "] " string

#if 1
#undef lockf
#define lockf(x,y,z) 0
#endif

#if 0
/*
 *  Return the current time in milliseconds.
 */
static long
dual_milliseconds(int child, int t1, int t2)
{
	long ret;
	struct timeval now;
	static const char *msgs[] = {
		"             %1$-6.6s !      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     :                    [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     ! %1$-6.6s             [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !  : %1$-6.6s             [%7$d:%8$03d]\n",
		"                    !  %1$-6.6s %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !                    [%7$d:%8$03d]\n",
	};
	static const char *blank[] = {
		"                    !                                   :                    \n",
		"                    :                                   !                    \n",
		"                    :                                !  :                    \n",
		"                    !                                   !                    \n",
	};
	static const char *plus[] = {
		"               +    !                                   :                    \n",
		"                    :                                   !    +               \n",
		"                    :                                !  :    +               \n",
		"                    !      +                            !                    \n",
	};

	gettimeofday(&now, NULL);
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000;
	ret += (now.tv_usec + 500) / 1000;

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, blank[child]);
		fprintf(stdout, msgs[child], timer[t1].name, timer[t1].lo / 1000, timer[t1].lo - ((timer[t1].lo / 1000) * 1000), timer[t1].name, timer[t1].hi / 1000, timer[t1].hi - ((timer[t1].hi / 1000) * 1000), child, state);
		fprintf(stdout, plus[child]);
		fprintf(stdout, msgs[child], timer[t2].name, timer[t2].lo / 1000, timer[t2].lo - ((timer[t2].lo / 1000) * 1000), timer[t2].name, timer[t2].hi / 1000, timer[t2].hi - ((timer[t2].hi / 1000) * 1000), child, state);
		fprintf(stdout, blank[child]);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}

	return ret;
}

/*
 *  Return the current time in milliseconds.
 */
static long
milliseconds(int child, int t)
{
	long ret;
	struct timeval now;
	static const char *msgs[] = {
		"             %1$-6.6s !      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     :                    [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     ! %1$-6.6s             [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !  : %1$-6.6s             [%7$d:%8$03d]\n",
		"                    !  %1$-6.6s %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !                    [%7$d:%8$03d]\n",
	};
	static const char *blank[] = {
		"                    !                                   :                    \n",
		"                    :                                   !                    \n",
		"                    :                                !  :                    \n",
		"                    !                                   !                    \n",
	};

	gettimeofday(&now, NULL);
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000;
	ret += (now.tv_usec + 500) / 1000;

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, blank[child]);
		fprintf(stdout, msgs[child], timer[t].name, timer[t].lo / 1000, timer[t].lo - ((timer[t].lo / 1000) * 1000), timer[t].name, timer[t].hi / 1000, timer[t].hi - ((timer[t].hi / 1000) * 1000), child, state);
		fprintf(stdout, blank[child]);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}

	return ret;
}

/*
 *  Check the current time against the beginning time provided as an argnument
 *  and see if the time inverval falls between the low and high values for the
 *  timer as specified by arguments.  Return SUCCESS if the interval is within
 *  the allowable range and FAILURE otherwise.
 */
static int
check_time(int child, const char *t, long beg, long lo, long hi)
{
	long i;
	struct timeval now;
	static const char *msgs[] = {
		"       check %1$-6.6s ? [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]   |                    [%8$d:%9$03d]\n",
		"                    | [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]   ? %1$-6.6s check       [%8$d:%9$03d]\n",
		"                    | [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]?  | %1$-6.6s check       [%8$d:%9$03d]\n",
		"       check %1$-6.6s ? [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]   ?                    [%8$d:%9$03d]\n",
	};

	if (gettimeofday(&now, NULL)) {
		printf("****ERROR: gettimeofday\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}

	i = (now.tv_sec - test_start) * 1000;
	i += (now.tv_usec + 500) / 1000;
	i -= beg;

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, msgs[child], t, (lo - 100) / 1000, (lo - 100) - (((lo - 100) / 1000) * 1000), i / 1000, i - ((i / 1000) * 1000), (hi + 100) / 1000, (hi + 100) - (((hi + 100) / 1000) * 1000), child, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}

	if (lo - 100 <= i && i <= hi + 100)
		return __RESULT_SUCCESS;
	else
		return __RESULT_FAILURE;
}
#endif

static int
time_event(int child, int event)
{
	static const char *msgs[] = {
		"                    ! %11.6g                       |                    <%d:%03d>\n",
		"                    |                       %11.6g !                    <%d:%03d>\n",
		"                    |                    %11.6g !  |                    <%d:%03d>\n",
		"                    !            %11.6g            !                    <%d:%03d>\n",
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
//      act.sa_flags = SA_RESTART | SA_ONESHOT;
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
 *  Addresses
 */
#if 1
#ifndef SCTP_VERSION_2
addr_t addrs[4];
#else				/* SCTP_VERSION_2 */
struct sockaddr_in addrs[4][3];
#endif				/* SCTP_VERSION_2 */
#else
struct sockaddr_in addrs[4];
#endif
int anums[4] = { 3, 3, 3, 3 };
#define TEST_PORT_NUMBER 18000
unsigned short ports[4] = { TEST_PORT_NUMBER+0, TEST_PORT_NUMBER+1, TEST_PORT_NUMBER+2, TEST_PORT_NUMBER+3 };
const char *addr_strings[4] = { "127.0.0.1", "127.0.0.2", "127.0.0.3", "127.0.0.4" };

/*
 *  Options
 */

N_qos_sel_data_sctp_t qos_data = {
	.n_qos_type = N_QOS_SEL_DATA_SCTP,
	.ppi = 0,
	.sid = 0,
	.ssn = 0,
	.tsn = 0,
	.more = 0,
};

N_qos_sel_conn_sctp_t qos_conn = {
	.n_qos_type = N_QOS_SEL_CONN_SCTP,
	.i_streams = 1,
	.o_streams = 1,
};

N_qos_sel_info_sctp_t qos_info = {
	.n_qos_type = N_QOS_SEL_INFO_SCTP,
	.i_streams = 1,
	.o_streams = 1,
	.ppi = 0,
	.sid = 0,
	.max_inits = 12,
	.max_retrans = 12,
	.ck_life = -1,
	.ck_inc = -1,
	.hmac = -1,
	.throttle = -1,
	.max_sack = 0,
	.rto_ini = 0,
	.rto_min = 0,
	.rto_max = 0,
	.rtx_path = 0,
	.hb_itvl = 200,
	.options = 0,
};

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

char *
nerrno_string(ulong nerr, long uerr)
{
	switch (nerr) {
	case NBADADDR:
		return ("[NBADADDR]");
	case NBADOPT:
		return ("[NBADOPT]");
	case NACCESS:
		return ("[NACCESS]");
	case NNOADDR:
		return ("[NNOADDR]");
	case NOUTSTATE:
		return ("[NOUTSTATE]");
	case NBADSEQ:
		return ("[NBADSEQ]");
	case NSYSERR:
		return errno_string(uerr);
	case NBADDATA:
		return ("[NBADDATA]");
	case NBADFLAG:
		return ("[NBADFLAG]");
	case NNOTSUPPORT:
		return ("[NNOTSUPPORT]");
	case NBOUND:
		return ("[NBOUND]");
	case NBADQOSPARAM:
		return ("[NBADQOSPARAM]");
	case NBADQOSTYPE:
		return ("[NBADQOSTYPE]");
	case NBADTOKEN:
		return ("[NBADTOKEN]");
	case NNOPROTOID:
		return ("[NNOPROTOID]");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "[%lu]", (ulong) nerr);
		return buf;
	}
	}
}

const char *
event_string(int child, int event)
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
	case __TEST_CONN_REQ:
		return ("N_CONN_REQ");
	case __TEST_CONN_RES:
		return ("N_CONN_RES");
	case __TEST_DISCON_REQ:
		return ("N_DISCON_REQ");
	case __TEST_DATA_REQ:
		return ("N_DATA_REQ");
	case __TEST_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case __TEST_INFO_REQ:
		return ("N_INFO_REQ");
	case __TEST_BIND_REQ:
		return ("N_BIND_REQ");
	case __TEST_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case __TEST_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case __TEST_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case __TEST_CONN_IND:
		return ("N_CONN_IND");
	case __TEST_CONN_CON:
		return ("N_CONN_CON");
	case __TEST_DISCON_IND:
		return ("N_DISCON_IND");
	case __TEST_DATA_IND:
		return ("N_DATA_IND");
	case __TEST_EXDATA_IND:
		return ("N_EXDATA_IND");
	case __TEST_INFO_ACK:
		return ("N_INFO_ACK");
	case __TEST_BIND_ACK:
		return ("N_BIND_ACK");
	case __TEST_ERROR_ACK:
		return ("N_ERROR_ACK");
	case __TEST_OK_ACK:
		return ("N_OK_ACK");
	case __TEST_UNITDATA_IND:
		return ("N_UNITDATA_IND");
	case __TEST_UDERROR_IND:
		return ("N_UDERROR_IND");
	case __TEST_DATACK_REQ:
		return ("N_DATACK_REQ");
	case __TEST_DATACK_IND:
		return ("N_DATACK_IND");
	case __TEST_RESET_REQ:
		return ("N_RESET_REQ");
	case __TEST_RESET_IND:
		return ("N_RESET_IND");
	case __TEST_RESET_RES:
		return ("N_RESET_RES");
	case __TEST_RESET_CON:
		return ("N_RESET_CON");
	}
	return ("(unexpected)");
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
		if (arg) {
			struct strioctl *icp = (struct strioctl *) arg;

			switch (icp->ic_cmd) {
#if 0
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
#endif
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

const char *
service_type(np_ulong type)
{
	switch (type) {
	case 0:
		return ("(none)");
	case N_CLNS:
		return ("N_CLNS");
	case N_CONS:
		return ("N_CONS");
	case N_CLNS | N_CONS:
		return ("N_CLNS|N_CONS");
	default:
		return ("(unknown)");
	}
}

const char *
provider_type(np_ulong type)
{
	switch (type) {
	case N_SUBNET:
		return ("N_SUBNET");
	case N_SNICFP:
		return ("N_SNICFP");
	default:
		return ("(unknown)");
	}
}

const char *
state_string(np_ulong state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}

#ifndef SCTP_VERSION_2
void
print_addr(char *add_ptr, size_t add_len)
{
	sctp_addr_t *a = (sctp_addr_t *) add_ptr;
	size_t anum = add_len >= sizeof(a->port) ? (add_len - sizeof(a->port)) / sizeof(a->addr[0]) : 0;

	dummy = lockf(fileno(stdout), F_LOCK, 0);
	if (add_len) {
		int i;

		if (add_len != sizeof(a->port) + anum * sizeof(a->addr[0]))
			fprintf(stdout, "Aaarrg! add_len = %d, anum = %d, ", add_len, anum);
		fprintf(stdout, "[%d]", ntohs(a->port));
		for (i = 0; i < anum; i++) {
			fprintf(stdout, "%s%d.%d.%d.%d", i ? "," : "", (a->addr[i] >> 0) & 0xff, (a->addr[i] >> 8) & 0xff, (a->addr[i] >> 16) & 0xff, (a->addr[i] >> 24) & 0xff);
		}
	} else
		fprintf(stdout, "(no address)");
	fprintf(stdout, "\n");
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

char *
addr_string(char *add_ptr, size_t add_len)
{
	static char buf[128];
	size_t len = 0;
	sctp_addr_t *a = (sctp_addr_t *) add_ptr;
	size_t anum = add_len >= sizeof(a->port) ? (add_len - sizeof(a->port)) / sizeof(a->addr[0]) : 0;

	if (add_len) {
		int i;

		if (add_len != sizeof(a->port) + anum * sizeof(a->addr[0]))
			len += snprintf(buf + len, sizeof(buf) - len, "Aaarrg! add_len = %d, anum = %d, ", add_len, anum);
		len += snprintf(buf + len, sizeof(buf) - len, "[%d]", ntohs(a->port));
		for (i = 0; i < anum; i++) {
			len += snprintf(buf + len, sizeof(buf) - len, "%s%d.%d.%d.%d", i ? "," : "", (a->addr[i] >> 0) & 0xff, (a->addr[i] >> 8) & 0xff, (a->addr[i] >> 16) & 0xff, (a->addr[i] >> 24) & 0xff);
		}
	} else
		len += snprintf(buf + len, sizeof(buf) - len, "(no address)");
	/* len += snprintf(buf + len, sizeof(buf) - len, "\0"); */
	return buf;
}

void
print_addrs(int fd, char *add_ptr, size_t add_len)
{
	fprintf(stdout, "Stupid!\n");
}
#else				/* SCTP_VERSION_2 */
void
print_addr(char *add_ptr, size_t add_len)
{
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	size_t anum = add_len / sizeof(*a);

	dummy = lockf(fileno(stdout), F_LOCK, 0);
	if (add_len > 0) {
		int i;

		if (add_len != anum * sizeof(*a))
			fprintf(stdout, "Aaarrg! add_len = %lu, anum = %lu, ", (ulong) add_len, (ulong) anum);
		fprintf(stdout, "[%d]", ntohs(a[0].sin_port));
		for (i = 0; i < anum; i++) {
			uint32_t addr = a[i].sin_addr.s_addr;

			fprintf(stdout, "%s%d.%d.%d.%d", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
		}
	} else
		fprintf(stdout, "(no address)");
	fprintf(stdout, "\n");
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

char *
addr_string(char *add_ptr, size_t add_len)
{
	static char buf[128];
	size_t len = 0;
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	size_t anum = add_len / sizeof(*a);

	if (add_len > 0) {
		int i;

		if (add_len != anum * sizeof(*a))
			len += snprintf(buf + len, sizeof(buf) - len, "Aaarrg! add_len = %lu, anum = %lu, ", (ulong) add_len, (ulong) anum);
		len += snprintf(buf + len, sizeof(buf) - len, "[%d]", ntohs(a[0].sin_port));
		for (i = 0; i < anum; i++) {
			uint32_t addr = a[i].sin_addr.s_addr;

			len += snprintf(buf + len, sizeof(buf) - len, "%s%d.%d.%d.%d", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
		}
	} else
		len += snprintf(buf + len, sizeof(buf) - len, "(no address)");
	/* len += snprintf(buf + len, sizeof(buf) - len, "\0"); */
	return buf;
}
void print_string(int child, const char *string);
void
print_addrs(int child, char *add_ptr, size_t add_len)
{
	struct sockaddr_in *sin;

	if (verbose < 3 || !show)
		return;
	if (add_len == 0)
		print_string(child, "(no address)");
	for (sin = (typeof(sin)) add_ptr; add_len >= sizeof(*sin); sin++, add_len -= sizeof(*sin)) {
		char buf[128];

		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%d", (sin->sin_addr.s_addr >> 0) & 0xff, (sin->sin_addr.s_addr >> 8) & 0xff, (sin->sin_addr.s_addr >> 16) & 0xff, (sin->sin_addr.s_addr >> 24) & 0xff, ntohs(sin->sin_port));
		print_string(child, buf);
	}
}
#endif				/* SCTP_VERSION_2 */

char *
prot_string(char *pro_ptr, size_t pro_len)
{
	static char buf[128];
	size_t len = 0;
	int i;

	buf[0] = 0;
	for (i = 0; i < pro_len; i++) {
		len += snprintf(buf + len, sizeof(buf) - len, "%u ", (uint) (unsigned char) pro_ptr[i]);
	}
	return (buf);
}

void print_string(int child, const char *string);
void
print_prots(int child, char *pro_ptr, size_t pro_len)
{
	unsigned char *prot;

	if (verbose < 3 || !show)
		return;
	for (prot = (typeof(prot)) pro_ptr; pro_len > 0; prot++, pro_len--) {
		char buf[32];

		snprintf(buf, sizeof(buf), "<%u>", (unsigned int) *prot);
		print_string(child, buf);
	}
}

#if 0
char *
status_string(struct t_opthdr *oh)
{
	switch (oh->status) {
	case 0:
		return (NULL);
	case T_SUCCESS:
		return ("T_SUCCESS");
	case T_FAILURE:
		return ("T_FAILURE");
	case T_PARTSUCCESS:
		return ("T_PARTSUCCESS");
	case T_READONLY:
		return ("T_READONLY");
	case T_NOTSUPPORT:
		return ("T_NOTSUPPORT");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "(unknown status %ld)", (long) oh->status);
		return buf;
	}
	}
}

#ifndef T_ALLLEVELS
#define T_ALLLEVELS -1
#endif

char *
level_string(struct t_opthdr *oh)
{
	switch (oh->level) {
	case T_ALLLEVELS:
		return ("T_ALLLEVELS");
	case XTI_GENERIC:
		return ("XTI_GENERIC");
	case T_INET_IP:
		return ("T_INET_IP");
	case T_INET_UDP:
		return ("T_INET_UDP");
	case T_INET_TCP:
		return ("T_INET_TCP");
	case T_INET_SCTP:
		return ("T_INET_SCTP");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "(unknown level %ld)", (long) oh->level);
		return buf;
	}
	}
}

char *
name_string(struct t_opthdr *oh)
{
	if (oh->name == T_ALLOPT)
		return ("T_ALLOPT");
	switch (oh->level) {
	case XTI_GENERIC:
		switch (oh->name) {
		case XTI_DEBUG:
			return ("XTI_DEBUG");
		case XTI_LINGER:
			return ("XTI_LINGER");
		case XTI_RCVBUF:
			return ("XTI_RCVBUF");
		case XTI_RCVLOWAT:
			return ("XTI_RCVLOWAT");
		case XTI_SNDBUF:
			return ("XTI_SNDBUF");
		case XTI_SNDLOWAT:
			return ("XTI_SNDLOWAT");
		}
		break;
	case T_INET_IP:
		switch (oh->name) {
		case T_IP_OPTIONS:
			return ("T_IP_OPTIONS");
		case T_IP_TOS:
			return ("T_IP_TOS");
		case T_IP_TTL:
			return ("T_IP_TTL");
		case T_IP_REUSEADDR:
			return ("T_IP_REUSEADDR");
		case T_IP_DONTROUTE:
			return ("T_IP_DONTROUTE");
		case T_IP_BROADCAST:
			return ("T_IP_BROADCAST");
		case T_IP_ADDR:
			return ("T_IP_ADDR");
		}
		break;
	case T_INET_UDP:
		switch (oh->name) {
		case T_UDP_CHECKSUM:
			return ("T_UDP_CHECKSUM");
		}
		break;
	case T_INET_TCP:
		switch (oh->name) {
		case T_TCP_NODELAY:
			return ("T_TCP_NODELAY");
		case T_TCP_MAXSEG:
			return ("T_TCP_MAXSEG");
		case T_TCP_KEEPALIVE:
			return ("T_TCP_KEEPALIVE");
		case T_TCP_CORK:
			return ("T_TCP_CORK");
		case T_TCP_KEEPIDLE:
			return ("T_TCP_KEEPIDLE");
		case T_TCP_KEEPINTVL:
			return ("T_TCP_KEEPINTVL");
		case T_TCP_KEEPCNT:
			return ("T_TCP_KEEPCNT");
		case T_TCP_SYNCNT:
			return ("T_TCP_SYNCNT");
		case T_TCP_LINGER2:
			return ("T_TCP_LINGER2");
		case T_TCP_DEFER_ACCEPT:
			return ("T_TCP_DEFER_ACCEPT");
		case T_TCP_WINDOW_CLAMP:
			return ("T_TCP_WINDOW_CLAMP");
		case T_TCP_INFO:
			return ("T_TCP_INFO");
		case T_TCP_QUICKACK:
			return ("T_TCP_QUICKACK");
		}
		break;
	case T_INET_SCTP:
		switch (oh->name) {
		case T_SCTP_NODELAY:
			return ("T_SCTP_NODELAY");
		case T_SCTP_CORK:
			return ("T_SCTP_CORK");
		case T_SCTP_PPI:
			return ("T_SCTP_PPI");
		case T_SCTP_SID:
			return ("T_SCTP_SID");
		case T_SCTP_SSN:
			return ("T_SCTP_SSN");
		case T_SCTP_TSN:
			return ("T_SCTP_TSN");
		case T_SCTP_RECVOPT:
			return ("T_SCTP_RECVOPT");
		case T_SCTP_COOKIE_LIFE:
			return ("T_SCTP_COOKIE_LIFE");
		case T_SCTP_SACK_DELAY:
			return ("T_SCTP_SACK_DELAY");
		case T_SCTP_PATH_MAX_RETRANS:
			return ("T_SCTP_PATH_MAX_RETRANS");
		case T_SCTP_ASSOC_MAX_RETRANS:
			return ("T_SCTP_ASSOC_MAX_RETRANS");
		case T_SCTP_MAX_INIT_RETRIES:
			return ("T_SCTP_MAX_INIT_RETRIES");
		case T_SCTP_HEARTBEAT_ITVL:
			return ("T_SCTP_HEARTBEAT_ITVL");
		case T_SCTP_RTO_INITIAL:
			return ("T_SCTP_RTO_INITIAL");
		case T_SCTP_RTO_MIN:
			return ("T_SCTP_RTO_MIN");
		case T_SCTP_RTO_MAX:
			return ("T_SCTP_RTO_MAX");
		case T_SCTP_OSTREAMS:
			return ("T_SCTP_OSTREAMS");
		case T_SCTP_ISTREAMS:
			return ("T_SCTP_ISTREAMS");
		case T_SCTP_COOKIE_INC:
			return ("T_SCTP_COOKIE_INC");
		case T_SCTP_THROTTLE_ITVL:
			return ("T_SCTP_THROTTLE_ITVL");
		case T_SCTP_MAC_TYPE:
			return ("T_SCTP_MAC_TYPE");
		case T_SCTP_CKSUM_TYPE:
			return ("T_SCTP_CKSUM_TYPE");
		case T_SCTP_ECN:
			return ("T_SCTP_ECN");
		case T_SCTP_ALI:
			return ("T_SCTP_ALI");
		case T_SCTP_ADD:
			return ("T_SCTP_ADD");
		case T_SCTP_SET:
			return ("T_SCTP_SET");
		case T_SCTP_ADD_IP:
			return ("T_SCTP_ADD_IP");
		case T_SCTP_DEL_IP:
			return ("T_SCTP_DEL_IP");
		case T_SCTP_SET_IP:
			return ("T_SCTP_SET_IP");
		case T_SCTP_PR:
			return ("T_SCTP_PR");
		case T_SCTP_LIFETIME:
			return ("T_SCTP_LIFETIME");
		case T_SCTP_DISPOSITION:
			return ("T_SCTP_DISPOSITION");
		case T_SCTP_MAX_BURST:
			return ("T_SCTP_MAX_BURST");
		case T_SCTP_HB:
			return ("T_SCTP_HB");
		case T_SCTP_RTO:
			return ("T_SCTP_RTO");
		case T_SCTP_MAXSEG:
			return ("T_SCTP_MAXSEG");
		case T_SCTP_STATUS:
			return ("T_SCTP_STATUS");
		case T_SCTP_DEBUG:
			return ("T_SCTP_DEBUG");
		}
		break;
	}
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "(unknown name %ld)", (long) oh->name);
		return buf;
	}
}

char *
yesno_string(struct t_opthdr *oh)
{
	switch (*((t_uscalar_t *) T_OPT_DATA(oh))) {
	case T_YES:
		return ("T_YES");
	case T_NO:
		return ("T_NO");
	default:
		return ("(invalid)");
	}
}

char *
number_string(struct t_opthdr *oh)
{
	static char buf[32];

	snprintf(buf, 32, "%d", *((t_scalar_t *) T_OPT_DATA(oh)));
	return (buf);
}

char *
value_string(int child, struct t_opthdr *oh)
{
	static char buf[64] = "(invalid)";

	if (oh->len == sizeof(*oh))
		return (NULL);
	switch (oh->level) {
	case XTI_GENERIC:
		switch (oh->name) {
		case XTI_DEBUG:
			break;
		case XTI_LINGER:
			break;
		case XTI_RCVBUF:
			break;
		case XTI_RCVLOWAT:
			break;
		case XTI_SNDBUF:
			break;
		case XTI_SNDLOWAT:
			break;
		}
		break;
	case T_INET_IP:
		switch (oh->name) {
		case T_IP_OPTIONS:
			break;
		case T_IP_TOS:
			if (oh->len == sizeof(*oh) + sizeof(unsigned char))
				snprintf(buf, sizeof(buf), "0x%02x", *((unsigned char *) T_OPT_DATA(oh)));
			return buf;
		case T_IP_TTL:
			if (oh->len == sizeof(*oh) + sizeof(unsigned char))
				snprintf(buf, sizeof(buf), "0x%02x", *((unsigned char *) T_OPT_DATA(oh)));
			return buf;
		case T_IP_REUSEADDR:
			return yesno_string(oh);
		case T_IP_DONTROUTE:
			return yesno_string(oh);
		case T_IP_BROADCAST:
			return yesno_string(oh);
		case T_IP_ADDR:
			if (oh->len == sizeof(*oh) + sizeof(uint32_t)) {
				uint32_t addr = *((uint32_t *) T_OPT_DATA(oh));

				snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (addr >> 0) & 0x00ff, (addr >> 8) & 0x00ff, (addr >> 16) & 0x00ff, (addr >> 24) & 0x00ff);
			}
			return buf;
		}
		break;
	case T_INET_UDP:
		switch (oh->name) {
		case T_UDP_CHECKSUM:
			return yesno_string(oh);
		}
		break;
	case T_INET_TCP:
		switch (oh->name) {
		case T_TCP_NODELAY:
			return yesno_string(oh);
		case T_TCP_MAXSEG:
			if (oh->len == sizeof(*oh) + sizeof(t_uscalar_t))
				snprintf(buf, sizeof(buf), "%lu", (ulong) *((t_uscalar_t *) T_OPT_DATA(oh)));
			return buf;
		case T_TCP_KEEPALIVE:
			break;
		case T_TCP_CORK:
			return yesno_string(oh);
		case T_TCP_KEEPIDLE:
			break;
		case T_TCP_KEEPINTVL:
			break;
		case T_TCP_KEEPCNT:
			break;
		case T_TCP_SYNCNT:
			break;
		case T_TCP_LINGER2:
			break;
		case T_TCP_DEFER_ACCEPT:
			break;
		case T_TCP_WINDOW_CLAMP:
			break;
		case T_TCP_INFO:
			break;
		case T_TCP_QUICKACK:
			break;
		}
		break;
	case T_INET_SCTP:
		switch (oh->name) {
		case T_SCTP_NODELAY:
			return yesno_string(oh);
		case T_SCTP_CORK:
			return yesno_string(oh);
		case T_SCTP_PPI:
			return number_string(oh);;
		case T_SCTP_SID:
#if 1
			sid[child] = *((t_uscalar_t *) T_OPT_DATA(oh));
#endif
			return number_string(oh);;
		case T_SCTP_SSN:
		case T_SCTP_TSN:
			return number_string(oh);;
		case T_SCTP_RECVOPT:
			return yesno_string(oh);
		case T_SCTP_COOKIE_LIFE:
		case T_SCTP_SACK_DELAY:
		case T_SCTP_PATH_MAX_RETRANS:
		case T_SCTP_ASSOC_MAX_RETRANS:
		case T_SCTP_MAX_INIT_RETRIES:
		case T_SCTP_HEARTBEAT_ITVL:
		case T_SCTP_RTO_INITIAL:
		case T_SCTP_RTO_MIN:
		case T_SCTP_RTO_MAX:
		case T_SCTP_OSTREAMS:
		case T_SCTP_ISTREAMS:
		case T_SCTP_COOKIE_INC:
		case T_SCTP_THROTTLE_ITVL:
			return number_string(oh);;
		case T_SCTP_MAC_TYPE:
			break;
		case T_SCTP_CKSUM_TYPE:
			break;
		case T_SCTP_ECN:
			break;
		case T_SCTP_ALI:
			break;
		case T_SCTP_ADD:
			break;
		case T_SCTP_SET:
			break;
		case T_SCTP_ADD_IP:
			break;
		case T_SCTP_DEL_IP:
			break;
		case T_SCTP_SET_IP:
			break;
		case T_SCTP_PR:
			break;
		case T_SCTP_LIFETIME:
		case T_SCTP_DISPOSITION:
		case T_SCTP_MAX_BURST:
		case T_SCTP_HB:
		case T_SCTP_RTO:
		case T_SCTP_MAXSEG:
			return number_string(oh);
		case T_SCTP_STATUS:
			break;
		case T_SCTP_DEBUG:
			break;
		}
		break;
	}
	return ("(unknown value)");
}
#endif

#if 0
void
parse_options(int fd, char *opt_ptr, size_t opt_len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(opt_ptr, opt_len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(opt_ptr, opt_len, oh, 0)) {
		if (oh->len == sizeof(*oh))
			continue;
		switch (oh->level) {
		case T_INET_SCTP:
			switch (oh->name) {
			case T_SCTP_PPI:
				ppi[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			case T_SCTP_SID:
				sid[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			case T_SCTP_SSN:
				ssn[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			case T_SCTP_TSN:
				tsn[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			}
		}
	}
}
#endif

const char *
mgmtflag_string(np_ulong flag)
{
	switch (flag) {
	case 0:
		return ("(none)");
	case DEFAULT_RC_SEL:
		return ("DEFAULT_RC_SEL");
	default:
		return ("(unknown flags)");
	}
}

#if 0
char *
size_string(t_uscalar_t size)
{
	static char buf[128];

	switch (size) {
	case T_INFINITE:
		return ("T_INFINITE");
	case T_INVALID:
		return ("T_INVALID");
	case T_UNSPEC:
		return ("T_UNSPEC");
	}
	snprintf(buf, sizeof(buf), "%lu", (ulong) size);
	return buf;
}
#endif

const char *
reset_reason_string(np_ulong RESET_reason)
{
	switch (RESET_reason) {
	case N_CONGESTION:
		return ("N_CONGESTION");
	case N_RESET_UNSPECIFIED:
		return ("N_RESET_UNSPECIFIED");
	case N_USER_RESYNC:
		return ("N_USER_RESYNC");
	case N_REASON_UNDEFINED:
		return ("N_REASON_UNDEFINED");
	case N_UD_UNDEFINED:
		return ("N_UD_UNDEFINED");
	case N_UD_TD_EXCEEDED:
		return ("N_UD_TD_EXCEEDED");
	case N_UD_CONGESTION:
		return ("N_UD_CONGESTION");
	case N_UD_QOS_UNAVAIL:
		return ("N_UD_QOS_UNAVAIL");
	case N_UD_LIFE_EXCEEDED:
		return ("N_UD_LIFE_EXCEEDED");
	case N_UD_ROUTE_UNAVAIL:
		return ("N_UD_ROUTE_UNAVAIL");
	case N_UD_SEG_REQUIRED:
		return ("N_UD_SEG_REQUIRED");
	default:
		return ("(unknown)");
	}
}

char *
reset_orig_string(np_ulong RESET_orig)
{
	switch (RESET_orig) {
	case N_USER:
		return ("N_USER");
	case N_PROVIDER:
		return ("N_PROVIDER");
	case N_UNDEFINED:
		return ("N_UNDEFINED");
	default:
		return ("(unknown)");
	}
}

void
print_proto(char *pro_ptr, size_t pro_len)
{
	uint32_t *p = (uint32_t *) pro_ptr;
	size_t pnum = pro_len / sizeof(p[0]);

	if (pro_len) {
		int i;

		if (!pnum)
			printf("(PROTOID_length = %lu)", (ulong) pro_len);
		for (i = 0; i < pnum; i++) {
			printf("%s%d", i ? "," : "", p[i]);
		}
	} else
		printf("(no protoids)");
	printf("\n");
}

void
print_options(int child, const char *cmd_buf, size_t qos_ofs, size_t qos_len)
{
	unsigned char *qos_ptr = (unsigned char *) (cmd_buf + qos_ofs);
	N_qos_sctp_t *qos = (N_qos_sctp_t *) qos_ptr;
	char buf[64];

	if (verbose < 3 || !show)
		return;
	if (qos_len) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_CONN_SCTP:
			snprintf(buf, sizeof(buf), "N_QOS_SEL_CONN_SCTP:");
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " i_streams = %ld,", (long) qos->n_qos_conn.i_streams);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " o_streams = %ld ", (long) qos->n_qos_conn.o_streams);
			print_string(child, buf);
			break;

		case N_QOS_SEL_DATA_SCTP:
			snprintf(buf, sizeof(buf), "DATA: ");
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ppi = %ld,", (long) qos->n_qos_data.ppi);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " sid = %ld,", (long) qos->n_qos_data.sid);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ssn = %ld,", (long) qos->n_qos_data.ssn);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " tsn = %lu,", (ulong) qos->n_qos_data.tsn);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " more = %ld", (long) qos->n_qos_data.more);
			print_string(child, buf);
			break;

		case N_QOS_SEL_INFO_SCTP:
			snprintf(buf, sizeof(buf), "N_QOS_SEL_INFO_SCTP: ");
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " i_streams = %ld,", (long) qos->n_qos_info.i_streams);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " o_streams = %ld,", (long) qos->n_qos_info.o_streams);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ppi = %ld,", (long) qos->n_qos_info.ppi);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " sid = %ld,", (long) qos->n_qos_info.sid);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " max_inits = %ld,", (long) qos->n_qos_info.max_inits);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " max_retrans = %ld,", (long) qos->n_qos_info.max_retrans);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " max_sack = %ld,", (long) qos->n_qos_info.max_sack);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ck_life = %ld,", (long) qos->n_qos_info.ck_life);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ck_inc = %ld,", (long) qos->n_qos_info.ck_inc);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " hmac = %ld,", (long) qos->n_qos_info.hmac);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " throttle = %ld,", (long) qos->n_qos_info.throttle);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " rto_ini = %ld,", (long) qos->n_qos_info.rto_ini);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " rto_min = %ld,", (long) qos->n_qos_info.rto_min);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " rto_max = %ld,", (long) qos->n_qos_info.rto_max);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " rtx_path = %ld,", (long) qos->n_qos_info.rtx_path);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " hb_itvl = %ld", (long) qos->n_qos_info.hb_itvl);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " options = ");
			print_string(child, buf);
			if (!qos->n_qos_info.options) {
				snprintf(buf, sizeof(buf), "(none)");
				print_string(child, buf);
			}
			if (qos->n_qos_info.options & SCTP_OPTION_DROPPING) {
				snprintf(buf, sizeof(buf), " DEBUG-DROPPING");
				print_string(child, buf);
			}
			if (qos->n_qos_info.options & SCTP_OPTION_BREAK) {
				snprintf(buf, sizeof(buf), " DEBUG-BREAK");
				print_string(child, buf);
			}
			if (qos->n_qos_info.options & SCTP_OPTION_DBREAK) {
				snprintf(buf, sizeof(buf), " DEBUG-DBREAK");
				print_string(child, buf);
			}
			if (qos->n_qos_info.options & SCTP_OPTION_RANDOM) {
				snprintf(buf, sizeof(buf), " DEBUG-RANDOM");
				print_string(child, buf);
			}
			break;

		case N_QOS_RANGE_INFO_SCTP:
			snprintf(buf, sizeof(buf), "N_QOS_RANGE_INFO_SCTP: ");
			print_string(child, buf);
			break;

		default:
			snprintf(buf, sizeof(buf), "(unknown qos structure %lu)\n", (ulong) qos->n_qos_type);
			print_string(child, buf);
			break;
		}
	} else {
		snprintf(buf, sizeof(buf), "(no qos)");
		print_string(child, buf);
	}
}

void
print_size(ulong size)
{
	switch (size) {
	case -1:
		printf("UNLIMITED\n");
		break;
	case -2:
		printf("UNDEFINED\n");
		break;
	default:
		printf("%lu\n", size);
		break;
	}
}

const char *
prim_string(int prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return ("N_CONN_REQ------");
	case N_CONN_RES:
		return ("N_CONN_RES------");
	case N_DISCON_REQ:
		return ("N_DISCON_REQ----");
	case N_DATA_REQ:
		return ("N_DATA_REQ------");
	case N_EXDATA_REQ:
		return ("N_EXDATA_REQ----");
	case N_INFO_REQ:
		return ("N_INFO_REQ------");
	case N_BIND_REQ:
		return ("N_BIND_REQ------");
	case N_UNBIND_REQ:
		return ("N_UNBIND_REQ----");
	case N_UNITDATA_REQ:
		return ("N_UNITDATA_REQ--");
	case N_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ---");
	case N_RESET_REQ:
		return ("N_RESET_REQ-----");
	case N_RESET_RES:
		return ("N_RESET_RES-----");
	case N_CONN_IND:
		return ("N_CONN_IND------");
	case N_CONN_CON:
		return ("N_CONN_CON------");
	case N_DISCON_IND:
		return ("N_DISCON_IND----");
	case N_DATA_IND:
		return ("N_DATA_IND------");
	case N_EXDATA_IND:
		return ("N_EXDATA_IND----");
	case N_INFO_ACK:
		return ("N_INFO_ACK------");
	case N_BIND_ACK:
		return ("N_BIND_ACK------");
	case N_ERROR_ACK:
		return ("N_ERROR_ACK-----");
	case N_OK_ACK:
		return ("N_OK_ACK--------");
	case N_UNITDATA_IND:
		return ("N_UNITDATA_IND--");
	case N_UDERROR_IND:
		return ("N_UDERROR_IND---");
	case N_RESET_IND:
		return ("N_RESET_IND-----");
	case N_RESET_CON:
		return ("N_RESET_CON-----");
	case N_DATACK_REQ:
		return ("N_DATACK_REQ----");
	case N_DATACK_IND:
		return ("N_DATACK_IND----");
	default:
		return ("N_????_??? -----");
	}
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
		"         . %1$6.6s . | <------         .         ------> :                    [%2$d:%3$03d]\n",
		"                    : <------         .         ------> | . %1$-6.6s .         [%2$d:%3$03d]\n",
		"                    : <------         .      ------> |  : . %1$-6.6s .         [%2$d:%3$03d]\n",
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
		"  open()      ----->v %-30.30s    .                   \n",
		"                    | %-30.30s    v<-----     open()  \n",
		"                    | %-30.30s v<-+------     open()  \n",
		"                    . %-30.30s .  .                   \n",
	};

	if (show && verbose > 3)
		print_simple_string(child, msgs, name);
}

void
print_close(int child)
{
	static const char *msgs[] = {
		"  close()     ----->X                                   |                   \n",
		"                    .                                   X<-----    close()  \n",
		"                    .                                X<-+------    close()  \n",
		"                    .                                .  .                   \n",
	};

	if (show && verbose > 3)
		print_simple(child, msgs);
}

void
print_preamble(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------Preamble-----------+--+                   \n",
		"                    +-------------Preamble-----------+  +-------------------\n",
		"                    +-------------Preamble-----------+--+-------------------\n",
		"--------------------+-------------Preamble-----------+--+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failure(int child, const char *string)
{
	static const char *msgs[] = {
		"....................|%-32.32s|..|                    [%d:%03d]\n",
		"                    |%-32.32s|  |................... [%d:%03d]\n",
		"                    |%-32.32s|...................... [%d:%03d]\n",
		"....................|%-32.32s|..|................... [%d:%03d]\n",
	};

	if (string && strnlen(string, 32) > 0 && verbose > 0)
		print_string_state(child, msgs, string);
}

void
print_notapplicable(int child)
{
	static const char *msgs[] = {
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-|X-|                    [%d:%03d]\n",
		"                    |X-X-X-X-X NOT APPLICABLE -X-X-X-|  |X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
		"                    |X-X-X-X-X NOT APPLICABLE -X-X-X-|X-|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-|X-|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_skipped(int child)
{
	static const char *msgs[] = {
		"::::::::::::::::::::|:::::::::::: SKIPPED :::::::::::|::|                    [%d:%03d]\n",
		"                    |:::::::::::: SKIPPED :::::::::::|  |::::::::::::::::::: [%d:%03d]\n",
		"                    |:::::::::::: SKIPPED :::::::::::|::|::::::::::::::::::: [%d:%03d]\n",
		"::::::::::::::::::::|:::::::::::: SKIPPED :::::::::::|::|::::::::::::::::::: [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_inconclusive(int child)
{
	static const char *msgs[] = {
		"????????????????????|?????????? INCONCLUSIVE ????????|??|                    [%d:%03d]\n",
		"                    |?????????? INCONCLUSIVE ????????|  |??????????????????? [%d:%03d]\n",
		"                    |?????????? INCONCLUSIVE ????????|??|??????????????????? [%d:%03d]\n",
		"????????????????????|?????????? INCONCLUSIVE ????????|??|??????????????????? [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_test(int child)
{
	static const char *msgs[] = {
		"--------------------+---------------Test-------------+--+                   \n",
		"                    +---------------Test-------------+  +-------------------\n",
		"                    +---------------Test-------------+--+-------------------\n",
		"--------------------+---------------Test-------------+--+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    [%d:%03d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXX|  |XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########### SCRIPT ERROR #######|##|                    [%d:%03d]\n",
		"                    |########### SCRIPT ERROR #######|  |################### [%d:%03d]\n",
		"                    |########### SCRIPT ERROR #######|##|################### [%d:%03d]\n",
		"####################|########### SCRIPT ERROR #######|##|################### [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_passed(int child)
{
	static const char *msgs[] = {
		"********************|************* PASSED ***********|**|                    [%d:%03d]\n",
		"                    |************* PASSED ***********|  |******************* [%d:%03d]\n",
		"                    |************* PASSED ***********|**|******************* [%d:%03d]\n",
		"********************|************* PASSED ***********|**|******************* [%d:%03d]\n",
	};

	if (verbose > 2)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_postamble(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------Postamble----------+--+                   \n",
		"                    +-------------Postamble----------+  +-------------------\n",
		"                    +-------------Postamble----------+--+-------------------\n",
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
		"                    +--------------------------------+  +-------------------\n",
		"                    +--------------------------------+--+-------------------\n",
		"--------------------+--------------------------------+--+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|                    {%d:%03d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@|  |@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|                    {%d:%03d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|++++++++++++ TIMEOUT! ++++++++++|++|                    [%d:%03d]\n",
		"                    |++++++++++++ TIMEOUT! ++++++++++|  |+++++++++++++++++++ [%d:%03d]\n",
		"                    |++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++ [%d:%03d]\n",
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
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - |  |                    [%d:%03d]\n",
		"                    |- - - - - - -nothing! - - - - - |  |- - - - - - - - - - [%d:%03d]\n",
		"                    |- - - - - - -nothing! - - - - - | -|- - - - - - - - - - [%d:%03d]\n",
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - | -|- - - - - - - - - - [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_double_int(child, msgs, child, state);
}

void
print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |<---%-14s  [%d:%03d]\n",
		"                    |                                |<-+----%-14s  [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if ((verbose && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, command);
}

void
print_tx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"--%16s->|- - - - - - - - - - - - - - - ->|->|                    [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - -|- |<-%16s- [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - -|<----%16s- [%d:%03d]\n",
		"                    |         %-16s       |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_rx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s--|<- - - - - - - - - - - - - - - -| -|                    [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - ->|  |--%16s> [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - ->|--+--%16s> [%d:%03d]\n",
		"                    |         <%16s>     |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_ack_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s-/|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |\\-%16s> [%d:%03d]\n",
		"                    |                                |\\-+--%16s> [%d:%03d]\n",
		"                    |         <%16s>     |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 0)
		print_string_state(child, msgs, command);
}

void
print_long_state(int child, const char *msgs[], long value)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], value, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_no_prim(int child, long prim)
{
	static const char *msgs[] = {
		"????%4ld????  ?----?|?- - - - - - -?                |  |                     [%d:%03d]\n",
		"                    |                               |  |?--? ????%4ld????    [%d:%03d]\n",
		"                    |                               |?-+---? ????%4ld????    [%d:%03d]\n",
		"                    | ? - - - - - - %4ld  - - - - ? |  |                     [%d:%03d]\n",
	};

	if (verbose > 1)
		print_long_state(child, msgs, prim);
}

void
print_signal(int child, int signum)
{
	static const char *msgs[] = {
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|                    [%d:%03d]\n",
		"                    |>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"                    |>>>>>>>>>>>> %-8.8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
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
		"%1$-14s----->|         %2$-16.16s       |  |                    [%3$d:%4$03d]\n",
		"                    |         %2$-16.16s       |  |<---%1$-14s  [%3$d:%4$03d]\n",
		"                    |         %2$-16.16s       |<-+----%1$-14s  [%3$d:%4$03d]\n",
		"                    | %1$-14s %2$-16.16s|  |                    [%3$d:%4$03d]\n",
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
print_tx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"--%1$16s->| - %2$5d bytes - - - - - - - - >|->|                    [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|- |<-%1$16s- [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|<-+ -%1$16s- [%3$d:%4$03d]\n",
		"                    | - %2$5d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show) || verbose > 4)
		print_string_int_state(child, msgs, command, bytes);
}

void
print_rx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"<-%1$16s--|<- -%2$4d bytes- - - - - - - - - |- |                    [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - ->|  |--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - - |->|--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose && show) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_errno(int child, long error)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-+--->%14s  [%d:%03d]\n",
		"                    |          [%14s]      |  |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, errno_string(error));
}

void
print_success(int child)
{
	static const char *msgs[] = {
		"  ok          <----/|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |\\---->         ok   [%d:%03d]\n",
		"                    |                                |\\-+----->         ok   [%d:%03d]\n",
		"                    |                 ok             |  |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_double_int(child, msgs, child, state);
}

void
print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"  %10d  <----/|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |\\---->  %10d  [%d:%03d]\n",
		"                    |                                |\\-+----->  %10d  [%d:%03d]\n",
		"                    |            [%10d]        |  |                    [%d:%03d]\n",
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
		"  %1$10d  <----/| %2$-30.30s |  |                    [%3$d:%4$03d]\n",
		"                    | %2$-30.30s |  |\\---->  %1$10d  [%3$d:%4$03d]\n",
		"                    | %2$-30.30s |\\-+----->  %1$10d  [%3$d:%4$03d]\n",
		"                    | %2$-17.17s [%1$10d] |  |                    [%3$d:%4$03d]\n",
	};

	if (show && verbose > 3)
		print_int_string_state(child, msgs, value, poll_events_string(revents));
}

void
print_ti_ioctl(int child, int cmd, intptr_t arg)
{
	static const char *msgs[] = {
		"--ioctl(2)--------->|       %16s         |  |                    [%d:%03d]\n",
		"                    |       %16s         |  |<---ioctl(2)------  [%d:%03d]\n",
		"                    |       %16s         |<-+----ioctl(2)------  [%d:%03d]\n",
		"                    |       %16s ioctl(2)|  |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, ioctl_string(cmd, arg));
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
		"  %1$16s->| - %2$5d bytes - - - - - - - - >|->|                    [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|- |<-%1$16s  [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|<-+--%1$16s  [%3$d:%4$03d]\n",
		"                    | - %2$5d bytes %1$16s |  |                    [%3$d:%4$03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_libcall(int child, const char *command)
{
	static const char *msgs[] = {
		"  %-16s->|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |<-%16s  [%d:%03d]\n",
		"                    |                                |<-+--%16s  [%d:%03d]\n",
		"                    |        [%16s]      |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, command);
}

#if 0
void
print_terror(int child, long error, long terror)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-+--->%14s  [%d:%03d]\n",
		"                    |          [%14s]      |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, t_errno_string(terror, error));
}
#endif

#if 0
void
print_tlook(int child, int tlook)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-|--->%14s  [%d:%03d]\n",
		"                    |          [%14s]      |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, t_look_string(tlook));
}
#endif

void
print_expect(int child, int want)
{
	static const char *msgs[] = {
		" (%-16s) |- - - - - -[Expected]- - - - - -|  |                    [%d:%03d]\n",
		"                    |- - - - - -[Expected]- - - - - -|  | (%-16s) [%d:%03d]\n",
		"                    |- - - - - -[Expected]- - - - - -|- | (%-16s) [%d:%03d]\n",
		"                    |- [Expected %-16s ] -|- |                    [%d:%03d]\n",
	};

	if (verbose > 0 && show)
		print_string_state(child, msgs, event_string(child, want));
}

void
print_string(int child, const char *string)
{
	static const char *msgs[] = {
		"%-20s|                                |  |                    \n",
		"                    |                                |  |%-20s\n",
		"                    |                                |   %-20s\n",
		"                    |       %-20s     |  |                    \n",
	};

	if (show && verbose > 1)
		print_simple_string(child, msgs, string);
}

void
print_string_val(int child, const char *string, ulong val)
{
	static const char *msgs[] = {
		"%1$20.20s|          %2$15u          |                    \n",
		"                    |          %2$15u          |%1$-20.20s\n",
		"                    |          %2$15u       |   %1$-20.20s\n",
		"                    |%1$-20.20s%2$15u|                    \n",
	};

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, msgs[child], string, val);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void
print_command_state(int child, const char *string)
{
	static const char *msgs[] = {
		"%20s|                                |  |                    [%d:%03d]\n",
		"                    |                                |  |%-20s[%d:%03d]\n",
		"                    |                                |  .%-20s[%d:%03d]\n",
		"                    |       %-20s     |  |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, string);
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
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / /|  |                    [%d:%03d]\n",
		"                    | / / / Waiting %03lu seconds / / /|  | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / / Waiting %03lu seconds / / /|/ | / / / / / / / / /  [%d:%03d]\n",
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
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ /|/ |                    [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ /|/ | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ /|/ | / / / / / / / / /  [%d:%03d]\n",
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ /|/ | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 0 && show) {
		float delay;

		delay = time->tv_nsec;
		delay = delay / 1000000000;
		delay = delay + time->tv_sec;
		print_float_state(child, msgs, delay);
	}
}

void
print_mgmtflag(int child, np_ulong flag)
{
	print_string(child, mgmtflag_string(flag));
}

#if 0
void
print_opt_level(int child, struct t_opthdr *oh)
{
	char *level = level_string(oh);

	if (level)
		print_string(child, level);
}

void
print_opt_name(int child, struct t_opthdr *oh)
{
	char *name = name_string(oh);

	if (name)
		print_string(child, name);
}

void
print_opt_status(int child, struct t_opthdr *oh)
{
	char *status = status_string(oh);

	if (status)
		print_string(child, status);
}

void
print_opt_length(int child, struct t_opthdr *oh)
{
	int len = oh->len - sizeof(*oh);

	if (len) {
		char buf[32];

		snprintf(buf, sizeof(buf), "(len=%d)", len);
		print_string(child, buf);
	}
}
void
print_opt_value(int child, struct t_opthdr *oh)
{
	char *value = value_string(child, oh);

	if (value)
		print_string(child, value);
}
#endif

#if 0
void
print_options(int child, const char *cmd_buf, size_t qos_ofs, size_t qos_len)
{
	unsigned char *qos_ptr = (unsigned char *) (cmd_buf + qos_ofs);
	union N_qos_ip_types *qos = (union N_qos_ip_types *) qos_ptr;
	char buf[64];

	if (verbose < 3 || !show)
		return;
	if (qos_len) {
		switch (qos->n_qos_type) {
		case N_QOS_SEL_CONN_IP:
			snprintf(buf, sizeof(buf), "N_QOS_SEL_CONN_IP:");
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " protocol = %ld,", (long) qos->n_qos_sel_conn.protocol);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " priority = %ld,", (long) qos->n_qos_sel_conn.priority);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ttl = %ld,", (long) qos->n_qos_sel_conn.ttl);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " tos = %ld,", (long) qos->n_qos_sel_conn.tos);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " mtu = %ld,", (long) qos->n_qos_sel_conn.mtu);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " saddr = 0x%x,", (unsigned int) ntohl(qos->n_qos_sel_conn.saddr));
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " daddr = 0x%x,", (unsigned int) ntohl(qos->n_qos_sel_conn.daddr));
			print_string(child, buf);
			break;

		case N_QOS_SEL_UD_IP:
			snprintf(buf, sizeof(buf), "N_QOS_SEL_UD_IP: ");
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " protocol = %ld,", (long) qos->n_qos_sel_ud.protocol);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " priority = %ld,", (long) qos->n_qos_sel_ud.priority);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ttl = %ld,", (long) qos->n_qos_sel_ud.ttl);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " tos = %ld,", (long) qos->n_qos_sel_ud.tos);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " saddr = 0x%x,", (unsigned int) ntohl(qos->n_qos_sel_ud.saddr));
			print_string(child, buf);
			break;

		case N_QOS_SEL_INFO_IP:
			snprintf(buf, sizeof(buf), "N_QOS_SEL_INFO_IP: ");
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " protocol = %ld,", (long) qos->n_qos_sel_info.protocol);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " priority = %ld,", (long) qos->n_qos_sel_info.priority);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " ttl = %ld,", (long) qos->n_qos_sel_info.ttl);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " tos = %ld,", (long) qos->n_qos_sel_info.tos);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " mtu = %ld,", (long) qos->n_qos_sel_info.mtu);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " saddr = 0x%x,", (unsigned int) ntohl(qos->n_qos_sel_info.saddr));
			print_string(child, buf);
			snprintf(buf, sizeof(buf), " daddr = 0x%x,", (unsigned int) ntohl(qos->n_qos_sel_info.daddr));
			print_string(child, buf);
			break;

		case N_QOS_RANGE_INFO_IP:
			snprintf(buf, sizeof(buf), "N_QOS_RANGE_INFO_IP: ");
			print_string(child, buf);
			break;

		default:
			snprintf(buf, sizeof(buf), "(unknown qos structure %lu)\n", (ulong) qos->n_qos_type);
			print_string(child, buf);
			break;
		}
	} else {
		snprintf(buf, sizeof(buf), "(no qos)");
		print_string(child, buf);
	}
}
#endif

void
print_info(int child, N_info_ack_t *info)
{
	char buf[64];

	if (verbose < 4 || !show)
		return;
	snprintf(buf, sizeof(buf), "NSDU_size = %d", (int) info->NSDU_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "ENSDU_size = %d", (int) info->ENSDU_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "CDATA_size = %d", (int) info->CDATA_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "DDATA_size = %d", (int) info->DDATA_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "ADDR_size = %d", (int) info->ADDR_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "OPTIONS_flags = %x", (int) info->OPTIONS_flags);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "NIDU_size = %d", (int) info->NIDU_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "<%s>", service_type(info->SERV_type));
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "<%s>", state_string(info->CURRENT_state));
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "<%s>", provider_type(info->PROVIDER_type));
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "NODU_size = %d", (int) info->NODU_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "NPI_version = %d", (int) info->NPI_version);
	print_string(child, buf);
}

void
print_reset_reason(int child, np_ulong RESET_reason)
{
	print_string(child, reset_reason_string(RESET_reason));
}

void
print_reset_orig(int child, np_ulong RESET_orig)
{
	print_string(child, reset_orig_string(RESET_orig));
}

#if 0
int
ip_n_open(const char *name, int *fdp)
{
	int fd;

	for (;;) {
		print_open(fdp, name);
		if ((fd = open(name, O_NONBLOCK | O_RDWR)) >= 0) {
			*fdp = fd;
			print_success(fd);
			return (__RESULT_SUCCESS);
		}
		print_errno(fd, (last_errno = errno));
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		return (__RESULT_FAILURE);
	}
}

int
ip_close(int *fdp)
{
	int fd = *fdp;

	*fdp = 0;
	for (;;) {
		print_close(fdp);
		if (close(fd) >= 0) {
			print_success(fd);
			return __RESULT_SUCCESS;
		}
		print_errno(fd, (last_errno = errno));
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		return __RESULT_FAILURE;
	}
}

int
ip_datack_req(int fd)
{
	int ret;

	data.len = -1;
	ctrl.len = sizeof(cmd.npi.datack_req) + sizeof(qos_data);
	cmd.prim = N_DATACK_REQ;
	bcopy(&qos_data, cbuf + sizeof(cmd.npi.datack_req), sizeof(qos_data));
	ret = put_msg(fd, 0, MSG_BAND, 0);
	return (ret);
}
#endif

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
			fprintf(stdout, "%02X", (uint8_t) ctrl->buf[i]);
		fprintf(stdout, "]\n");
		fprintf(stdout, "[");
		for (i = 0; i < (data ? data->len : 0); i++)
			fprintf(stdout, "%02X", (uint8_t) data->buf[i]);
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
				fprintf(stdout, "%02X", (uint8_t) ctrl->buf[i]);
			fprintf(stdout, "]\n");
			fprintf(stdout, "[");
			for (i = 0; i < (data ? data->len : 0); i++)
				fprintf(stdout, "%02X", (uint8_t) data->buf[i]);
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
	print_syscall(child, "write(2)------");
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
	print_syscall(child, "getmsg(2)-----");
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
	print_syscall(child, "getpmsg(2)----");
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
	print_syscall(child, "read(2)-------");
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
test_ti_ioctl(int child, int cmd, intptr_t arg)
{
	if (cmd == I_STR && verbose > 3) {
		struct strioctl *icp = (struct strioctl *) arg;

		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "ioctl from %d: cmd=%d, timout=%d, len=%d, dp=%p\n", child, icp->ic_cmd, icp->ic_timout, icp->ic_len, icp->ic_dp);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	print_ti_ioctl(child, cmd, arg);
	for (;;) {
		if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		if (show && verbose > 3)
			print_success_value(child, last_retval);
		break;
	}
	if (cmd == I_STR && show && verbose > 3) {
		struct strioctl *icp = (struct strioctl *) arg;

		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "got ioctl from %d: cmd=%d, timout=%d, len=%d, dp=%p\n", child, icp->ic_cmd, icp->ic_timout, icp->ic_len, icp->ic_dp);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (last_retval == 0)
		return __RESULT_SUCCESS;
	if (verbose) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "***************ERROR: ioctl failed\n");
		if (show && verbose > 3)
			fprintf(stdout, "                    : %s; result = %d\n", __FUNCTION__, last_retval);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
		fflush(stdout);
	}
	return (__RESULT_FAILURE);
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
		print_open(child, name);
		if ((fd = open(name, flags)) >= 0) {
			test_fd[child] = fd;
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
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		print_errno(child, (last_errno = errno));
		return __RESULT_FAILURE;
	}
}

int
test_push(int child, const char *name)
{
	if (show && verbose > 1)
		print_command_state(child, ":push");
	if (test_ioctl(child, I_PUSH, (intptr_t) name))
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

int
test_pop(int child)
{
	if (show && verbose > 1)
		print_command_state(child, ":pop");
	if (test_ioctl(child, I_POP, (intptr_t) 0))
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
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
	int offset = 3 * index;
	int i;

	for (i = 0; i < anums[3]; i++) {
#ifndef SCTP_VERSION_2
		addrs[3].port = htons(ports[3] + offset);
		inet_aton(addr_strings[i], &addrs[child].addr[i]);
#else				/* SCTP_VERSION_2 */
		addrs[3][i].sin_family = AF_INET;
		addrs[3][i].sin_port = htons(ports[3] + offset);
		inet_aton(addr_strings[i], &addrs[3][i].sin_addr);
#endif				/* SCTP_VERSION_2 */
	}
	switch (child) {
	case 1:
	case 2:
	case 0:
		for (i = 0; i < anums[child]; i++) {
#ifndef SCTP_VERSION_2
			addrs[child].port = htons(ports[child] + offset);
			inet_aton(addr_strings[i], &addrs[child].addr[i]);
#else				/* SCTP_VERSION_2 */
			addrs[child][i].sin_family = AF_INET;
			if ((child == 0 && !client_port_specified) || ((child == 1 || child == 2) && !server_port_specified))
				addrs[child][i].sin_port = htons(ports[child] + offset);
			else
				addrs[child][i].sin_port = htons(ports[child]);
			if ((child == 0 && !client_host_specified) || ((child == 1 || child == 2) && !server_host_specified))
				inet_aton(addr_strings[i], &addrs[child][i].sin_addr);
#endif				/* SCTP_VERSION_2 */
		}
		if (test_open(child, devname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_ioctl(child, I_SRDOPT, (intptr_t) RMSGD) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
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
	case 2:
	case 0:
		if (test_close(child) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
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
	if (stream_start(0, index) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_start(1, index) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_start(2, index) != __RESULT_SUCCESS)
		goto failure;
	state++;
	show_acks = 1;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
end_tests(int index)
{
	qos_data.sid = 0;
	qos_info.hmac = SCTP_HMAC_NONE;
	qos_info.options = 0;
	qos_info.i_streams = 1;
	qos_info.o_streams = 1;
	qos_conn.i_streams = 1;
	qos_conn.o_streams = 1;
	show_acks = 0;
	if (stream_stop(2) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_stop(1) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_stop(0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Injected event encoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

static int
do_signal(int child, int action)
{
	struct strbuf ctrl_buf, data_buf, *ctrl = &ctrl_buf, *data = &data_buf;
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	union N_primitives *p = (typeof(p)) cbuf;
	struct strioctl ic;

	ic.ic_cmd = 0;
	ic.ic_timout = test_timout;
	ic.ic_len = sizeof(cbuf);
	ic.ic_dp = cbuf;
	ctrl->maxlen = 0;
	ctrl->buf = cbuf;
	data->maxlen = 0;
	data->buf = dbuf;
	test_pflags = MSG_BAND;
	test_pband = 0;
	switch (action) {
	case __TEST_WRITE:
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Write test data.");
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
	switch (action) {
	case __TEST_PUSH:
		return test_push(child, "tirdwr");
	case __TEST_POP:
		return test_pop(child);
	case __TEST_PUTMSG_DATA:
		ctrl = NULL;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Putmsg test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PUTPMSG_DATA:
		ctrl = NULL;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Putpmsg band test data.");
		test_pflags = MSG_BAND;
		test_pband = 1;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_REQ:
		ctrl->len = sizeof(p->conn_req)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (QOS_buffer ? QOS_length : 0);
		p->conn_req.PRIM_type = N_CONN_REQ;
		p->conn_req.DEST_length = ADDR_buffer ? ADDR_length : 0;
		p->conn_req.DEST_offset = ADDR_buffer ? sizeof(p->conn_req) : 0;
		p->conn_req.CONN_flags = CONN_flags;
		p->conn_req.QOS_length = QOS_buffer ? QOS_length : 0;
		p->conn_req.QOS_offset = QOS_buffer ? sizeof(p->conn_req) + p->conn_req.DEST_length : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->conn_req.DEST_offset, p->conn_req.DEST_length);
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->conn_req.QOS_offset, p->conn_req.QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->conn_req.DEST_offset, p->conn_req.DEST_length));
#else
		print_addrs(child, cbuf + p->conn_req.DEST_offset, p->conn_req.DEST_length);
#endif
		print_options(child, cbuf, p->conn_req.QOS_offset, p->conn_req.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_IND:
		ctrl->len = sizeof(p->conn_ind)
		    + (DEST_buffer ? DEST_length : 0)
		    + (SRC_buffer ? SRC_length : 0)
		    + (QOS_buffer ? QOS_length : 0);
		p->conn_ind.PRIM_type = N_CONN_IND;
		p->conn_ind.DEST_length = DEST_buffer ? DEST_length : 0;
		p->conn_ind.DEST_offset = DEST_buffer ? sizeof(p->conn_ind) : 0;
		p->conn_ind.SRC_length = SRC_buffer ? SRC_length : 0;
		p->conn_ind.SRC_offset = SRC_buffer ? sizeof(p->conn_ind) + p->conn_ind.DEST_length : 0;
		p->conn_ind.SEQ_number = SEQ_number;
		p->conn_ind.CONN_flags = CONN_flags;
		p->conn_ind.QOS_length = QOS_buffer ? QOS_length : 0;
		p->conn_ind.QOS_offset = QOS_buffer ? sizeof(p->conn_ind) + p->conn_ind.DEST_length + p->conn_ind.SRC_length : 0;
		if (DEST_buffer)
			bcopy(DEST_buffer, ctrl->buf + p->conn_ind.DEST_offset, p->conn_ind.DEST_length);
		if (SRC_buffer)
			bcopy(SRC_buffer, ctrl->buf + p->conn_ind.SRC_offset, p->conn_ind.SRC_length);
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->conn_ind.QOS_offset, p->conn_ind.QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_addrs(child, cbuf + p->conn_ind.DEST_offset, p->conn_ind.DEST_length);
		print_addrs(child, cbuf + p->conn_ind.SRC_offset, p->conn_ind.SRC_length);
		print_options(child, cbuf, p->conn_ind.QOS_offset, p->conn_ind.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_RES:
		ctrl->len = sizeof(p->conn_res)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (QOS_buffer ? QOS_length : 0);
		p->conn_res.PRIM_type = N_CONN_RES;
		p->conn_res.TOKEN_value = TOKEN_value;
		p->conn_res.RES_length = ADDR_buffer ? ADDR_length : 0;
		p->conn_res.RES_offset = ADDR_buffer ? sizeof(p->conn_res) : 0;
		p->conn_res.CONN_flags = CONN_flags;
		p->conn_res.QOS_length = QOS_buffer ? QOS_length : 0;
		p->conn_res.QOS_offset = QOS_buffer ? sizeof(p->conn_res) + p->conn_res.RES_length : 0;
		p->conn_res.SEQ_number = SEQ_number;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->conn_res.RES_offset, p->conn_res.RES_length);
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->conn_res.QOS_offset, p->conn_res.QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_options(child, cbuf, p->conn_res.QOS_offset, p->conn_res.QOS_length);
		if (test_resfd == -1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		else
			return test_insertfd(child, test_resfd, 4, ctrl, data, 0);
	case __TEST_CONN_CON:
		ctrl->len = sizeof(p->conn_con);
		p->conn_con.PRIM_type = N_CONN_CON;
		p->conn_con.RES_length = 0;
		p->conn_con.RES_offset = 0;
		p->conn_con.CONN_flags = 0;
		p->conn_con.QOS_length = 0;
		p->conn_con.QOS_offset = 0;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Connection confirmation test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_addrs(child, cbuf + p->conn_con.RES_offset, p->conn_con.RES_length);
		print_options(child, cbuf, p->conn_con.QOS_offset, p->conn_con.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_REQ:
		ctrl->len = sizeof(p->discon_req)
		    + (ADDR_buffer ? ADDR_length : 0);
		p->discon_req.PRIM_type = N_DISCON_REQ;
		p->discon_req.DISCON_reason = DISCON_reason;
		p->discon_req.RES_length = ADDR_buffer ? ADDR_length : 0;
		p->discon_req.RES_offset = ADDR_buffer ? sizeof(p->discon_req) : 0;
		p->discon_req.SEQ_number = SEQ_number;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->discon_req.RES_offset, p->discon_req.RES_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_IND:
		ctrl->len = sizeof(p->discon_ind);
		p->discon_ind.PRIM_type = N_DISCON_IND;
		p->discon_ind.DISCON_orig = 0;
		p->discon_ind.DISCON_reason = 0;
		p->discon_ind.RES_length = 0;
		p->discon_ind.RES_offset = 0;
		p->discon_ind.SEQ_number = SEQ_number;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Disconnection indication test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATACK_REQ:
		ctrl->len = sizeof(p->datack_req) + QOS_length;
		p->datack_req.PRIM_type = N_DATACK_REQ;
		if (QOS_length)
			bcopy(QOS_buffer, ctrl->buf + sizeof(p->datack_req), QOS_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATACK_IND:
		ctrl->len = sizeof(p->datack_ind);
		p->datack_ind.PRIM_type = N_DATACK_IND;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_REQ:
		ctrl->len = sizeof(p->data_req) + QOS_length;
		p->data_req.PRIM_type = N_DATA_REQ;
		p->data_req.DATA_xfer_flags = DATA_xfer_flags;
		if (QOS_length)
			bcopy(QOS_buffer, ctrl->buf + sizeof(p->data_req), QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_IND:
		ctrl->len = sizeof(p->data_ind);
		p->data_ind.PRIM_type = N_DATA_IND;
		p->data_ind.DATA_xfer_flags = DATA_xfer_flags;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Normal test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_REQ:
		ctrl->len = sizeof(p->exdata_req) + QOS_length;
		p->exdata_req.PRIM_type = N_EXDATA_REQ;
		if (QOS_length)
			bcopy(QOS_buffer, ctrl->buf + sizeof(p->exdata_req), QOS_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 1;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_IND:
		ctrl->len = sizeof(p->exdata_ind);
		p->data_ind.PRIM_type = N_EXDATA_IND;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Expedited test data.");
		test_pflags = MSG_BAND;
		test_pband = 1;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_REQ:
		ctrl->len = sizeof(p->info_req);
		p->info_req.PRIM_type = N_INFO_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_ACK:
		ctrl->len = sizeof(p->info_ack);
		p->info_ack.PRIM_type = N_INFO_ACK;
		p->info_ack.NSDU_size = test_bufsize;
		p->info_ack.ENSDU_size = test_bufsize;
		p->info_ack.CDATA_size = test_bufsize;
		p->info_ack.DDATA_size = test_bufsize;
		p->info_ack.ADDR_size = test_bufsize;
		p->info_ack.NIDU_size = test_nidu;
		p->info_ack.SERV_type = SERV_type;
		p->info_ack.CURRENT_state = CURRENT_state;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_info(child, &p->info_ack);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_REQ:
		ctrl->len = sizeof(p->bind_req)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (PROTOID_buffer ? PROTOID_length : 0);
		p->bind_req.PRIM_type = N_BIND_REQ;
		p->bind_req.ADDR_length = ADDR_buffer ? ADDR_length : 0;
		p->bind_req.ADDR_offset = ADDR_buffer ? sizeof(p->bind_req) : 0;
		p->bind_req.CONIND_number = CONIND_number;
		p->bind_req.BIND_flags = BIND_flags;
		p->bind_req.PROTOID_length = PROTOID_buffer ? PROTOID_length : 0;
		p->bind_req.PROTOID_offset = PROTOID_buffer ? sizeof(p->bind_req) + p->bind_req.ADDR_length : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->bind_req.ADDR_offset, p->bind_req.ADDR_length);
		if (PROTOID_buffer)
			bcopy(PROTOID_buffer, ctrl->buf + p->bind_req.PROTOID_offset, p->bind_req.PROTOID_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_prots(child, cbuf + p->bind_ack.PROTOID_offset, p->bind_ack.PROTOID_length);
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
#else
		print_addrs(child, cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length);
#endif
		if (verbose > 3) {
			char buf[64];

			snprintf(buf, sizeof(buf), "CONIND_number = %d", CONIND_number);
			print_string(child, buf);
			snprintf(buf, sizeof(buf), "BIND_flags = %x", BIND_flags);
			print_string(child, buf);
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_ACK:
		ctrl->len = sizeof(p->bind_ack)
		    + (ADDR_buffer ? ADDR_length : 0)
		    + (PROTOID_buffer ? PROTOID_length : 0);
		p->bind_ack.PRIM_type = N_BIND_ACK;
		p->bind_ack.ADDR_length = ADDR_buffer ? ADDR_length : 0;
		p->bind_ack.ADDR_offset = ADDR_buffer ? sizeof(p->bind_ack) : 0;
		p->bind_ack.CONIND_number = CONIND_number;
		p->bind_ack.TOKEN_value = TOKEN_value;
		p->bind_ack.PROTOID_length = PROTOID_buffer ? PROTOID_length : 0;
		p->bind_ack.PROTOID_offset = PROTOID_buffer ? sizeof(p->bind_ack) + p->bind_ack.ADDR_length : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length);
		if (PROTOID_buffer)
			bcopy(PROTOID_buffer, ctrl->buf + p->bind_ack.PROTOID_offset, p->bind_ack.PROTOID_length);
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_prots(child, cbuf + p->bind_ack.PROTOID_offset, p->bind_ack.PROTOID_length);
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
#else
		print_addrs(child, cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length);
#endif
		if (verbose > 3) {
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNBIND_REQ:
		ctrl->len = sizeof(p->unbind_req);
		p->unbind_req.PRIM_type = N_UNBIND_REQ;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ERROR_ACK:
		ctrl->len = sizeof(p->error_ack);
		p->error_ack.PRIM_type = N_ERROR_ACK;
		p->error_ack.ERROR_prim = PRIM_type;
		p->error_ack.NPI_error = NPI_error;
		p->error_ack.UNIX_error = last_errno;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, nerrno_string(p->error_ack.NPI_error, p->error_ack.UNIX_error));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OK_ACK:
		ctrl->len = sizeof(p->ok_ack);
		p->ok_ack.PRIM_type = N_OK_ACK;
		p->ok_ack.CORRECT_prim = 0;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_REQ:
		ctrl->len = sizeof(p->unitdata_req)
		    + (ADDR_buffer ? ADDR_length : 0);
		p->unitdata_req.PRIM_type = N_UNITDATA_REQ;
		p->unitdata_req.DEST_length = ADDR_buffer ? ADDR_length : 0;
		p->unitdata_req.DEST_offset = ADDR_buffer ? sizeof(p->unitdata_req) : 0;
		if (ADDR_buffer)
			bcopy(ADDR_buffer, ctrl->buf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length);
		if (DATA_buffer) {
			data->buf = DATA_buffer;
			data->len = DATA_length;
		} else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length));
#else
		print_addrs(child, cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length);
#endif
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_IND:
		ctrl->len = sizeof(p->unitdata_ind);
		p->unitdata_ind.PRIM_type = N_UNITDATA_IND;
		p->unitdata_ind.SRC_length = 0;
		p->unitdata_ind.SRC_offset = 0;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Unit test data indication.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length));
#else
		print_addrs(child, cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length);
#endif
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UDERROR_IND:
		ctrl->len = sizeof(p->uderror_ind);
		p->uderror_ind.PRIM_type = N_UDERROR_IND;
		p->uderror_ind.DEST_length = 0;
		p->uderror_ind.DEST_offset = 0;
		p->uderror_ind.ERROR_type = 0;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTMGMT_REQ:
		ctrl->len = sizeof(p->optmgmt_req)
		    + (QOS_buffer ? QOS_length : 0);
		p->optmgmt_req.PRIM_type = N_OPTMGMT_REQ;
		p->optmgmt_req.QOS_length = QOS_buffer ? QOS_length : 0;
		p->optmgmt_req.QOS_offset = QOS_buffer ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.OPTMGMT_flags = OPTMGMT_flags;
		if (QOS_buffer)
			bcopy(QOS_buffer, ctrl->buf + p->optmgmt_req.QOS_offset, p->optmgmt_req.QOS_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_mgmtflag(child, p->optmgmt_req.OPTMGMT_flags);
		print_options(child, cbuf, p->optmgmt_req.QOS_offset, p->optmgmt_req.QOS_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_REQ:
		ctrl->len = sizeof(p->reset_req);
		p->reset_req.PRIM_type = N_RESET_REQ;
		p->reset_req.RESET_reason = RESET_reason;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_reset_reason(child, p->reset_req.RESET_reason);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_IND:
		ctrl->len = sizeof(p->reset_ind);
		p->reset_ind.PRIM_type = N_RESET_IND;
		p->reset_ind.RESET_orig = RESET_orig;
		p->reset_ind.RESET_reason = RESET_reason;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_reset_orig(child, p->reset_ind.RESET_orig);
		print_reset_reason(child, p->reset_ind.RESET_reason);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_RES:
		ctrl->len = sizeof(p->reset_res);
		p->reset_res.PRIM_type = N_RESET_RES;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_RESET_CON:
		ctrl->len = sizeof(p->reset_con);
		p->reset_con.PRIM_type = N_RESET_CON;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PRIM_TOO_SHORT:
		ctrl->len = sizeof(p->type);
		p->type = PRIM_type;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PRIM_WAY_TOO_SHORT:
		ctrl->len = sizeof(p->type) >> 1;
		p->type = PRIM_type;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	default:
		if (show && verbose > 1)
			print_command_state(child, ":????????");
		return __RESULT_SCRIPT_ERROR;
	}
	return __RESULT_SCRIPT_ERROR;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Received event decoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

static int
do_decode_data(int child, struct strbuf *ctrl, struct strbuf *data)
{
	int event = __RESULT_DECODE_ERROR;

	if (data->len >= 0) {
		event = __TEST_DATA;
		print_rx_data(child, "M_DATA----------", data->len);
	}
	return ((last_event = event));
}

static int
do_decode_ctrl(int child, struct strbuf *ctrl, struct strbuf *data)
{
	int event = __RESULT_DECODE_ERROR;
	union N_primitives *p = (union N_primitives *) ctrl->buf;

	if (ctrl->len >= sizeof(p->type)) {
		switch ((PRIM_type = p->type)) {
		case N_CONN_REQ:
			event = __TEST_CONN_REQ;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf, p->conn_req.QOS_offset, p->conn_req.QOS_length);
			break;
		case N_CONN_RES:
			event = __TEST_CONN_RES;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf, p->conn_res.QOS_offset, p->conn_res.QOS_length);
			break;
		case N_DISCON_REQ:
			event = __TEST_DISCON_REQ;
			SEQ_number = p->discon_req.SEQ_number;
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_DATA_REQ:
			event = __TEST_DATA_REQ;
			DATA_xfer_flags = p->data_req.DATA_xfer_flags;
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_EXDATA_REQ:
			event = __TEST_EXDATA_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_INFO_REQ:
			event = __TEST_INFO_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_BIND_REQ:
			event = __TEST_BIND_REQ;
			CONIND_number = p->bind_req.CONIND_number;
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_UNBIND_REQ:
			event = __TEST_UNBIND_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_UNITDATA_REQ:
			event = __TEST_UNITDATA_REQ;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length));
#else
			print_addrs(child, cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length);
#endif
			break;
		case N_OPTMGMT_REQ:
			event = __TEST_OPTMGMT_REQ;
			print_rx_prim(child, prim_string(p->type));
			print_mgmtflag(child, p->optmgmt_req.OPTMGMT_flags);
			print_options(child, cbuf, p->optmgmt_req.QOS_offset, p->optmgmt_req.QOS_length);
			break;
		case N_CONN_IND:
			event = __TEST_CONN_IND;
			SEQ_number = p->conn_ind.SEQ_number;
			CONN_flags = p->conn_ind.CONN_flags;
			DEST_buffer = (typeof(DEST_buffer)) (ctrl->buf + p->conn_ind.DEST_offset);
			DEST_length = p->conn_ind.DEST_length;
			SRC_buffer = (typeof(SRC_buffer)) (ctrl->buf + p->conn_ind.SRC_offset);
			SRC_length = p->conn_ind.SRC_length;
			QOS_buffer = (typeof(QOS_buffer)) (ctrl->buf + p->conn_ind.QOS_offset);
			QOS_length = p->conn_ind.QOS_length;
			DATA_buffer = data->buf;
			DATA_length = data->len;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->conn_ind.DEST_offset, p->conn_ind.DEST_length));
#else
			print_addrs(child, cbuf + p->conn_ind.DEST_offset, p->conn_ind.DEST_length);
#endif
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->conn_ind.SRC_offset, p->conn_ind.SRC_length));
#else
			print_addrs(child, cbuf + p->conn_ind.SRC_offset, p->conn_ind.SRC_length);
#endif
			print_options(child, cbuf, p->conn_ind.QOS_offset, p->conn_ind.QOS_length);
			break;
		case N_CONN_CON:
			event = __TEST_CONN_CON;
			print_rx_prim(child, prim_string(p->type));
			print_addrs(child, cbuf + p->conn_con.RES_offset, p->conn_con.RES_length);
			print_options(child, cbuf, p->conn_con.QOS_offset, p->conn_con.QOS_length);
			break;
		case N_DISCON_IND:
			event = __TEST_DISCON_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case N_DATA_IND:
			event = __TEST_DATA_IND;
			DATA_xfer_flags = p->data_ind.DATA_xfer_flags;
			print_rx_prim(child, prim_string(p->type));
			sid[child] = ((N_qos_sel_data_sctp_t *)(cbuf + sizeof(p->data_ind)))->sid;
			//print_options(child, cbuf, sizeof(p->data_ind), sizeof(N_qos_sel_data_sctp_t));
			break;
		case N_EXDATA_IND:
			event = __TEST_EXDATA_IND;
			print_rx_prim(child, prim_string(p->type));
			sid[child] = ((N_qos_sel_data_sctp_t *)(cbuf + sizeof(p->exdata_ind)))->sid;
			//print_options(child, cbuf, sizeof(p->exdata_ind), sizeof(N_qos_sel_data_sctp_t));
			break;
		case N_DATACK_IND:
			event = __TEST_DATACK_IND;
			print_rx_prim(child, prim_string(p->type));
			sid[child] = ((N_qos_sel_data_sctp_t *)(cbuf + sizeof(p->datack_ind)))->sid;
			//print_options(child, cbuf, sizeof(p->datack_ind), sizeof(N_qos_sel_data_sctp_t));
			break;
		case N_INFO_ACK:
			event = __TEST_INFO_ACK;
			last_info = p->info_ack;
			print_ack_prim(child, prim_string(p->type));
			print_info(child, &p->info_ack);
			break;
		case N_BIND_ACK:
			event = __TEST_BIND_ACK;
			CONIND_number = p->bind_ack.CONIND_number;
			TOKEN_value = p->bind_ack.TOKEN_value;
			tok[child] = TOKEN_value;
			ADDR_buffer = (typeof(ADDR_buffer)) (ctrl->buf + p->bind_ack.ADDR_offset);
			ADDR_length = p->bind_ack.ADDR_length;
			PROTOID_buffer = (typeof(PROTOID_buffer)) (ctrl->buf + p->bind_ack.PROTOID_offset);
			PROTOID_length = p->bind_ack.PROTOID_length;
			print_ack_prim(child, prim_string(p->type));
			print_prots(child, cbuf + p->bind_ack.PROTOID_offset, p->bind_ack.PROTOID_length);
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
#else
			print_addrs(child, cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length);
#endif
			if (show && verbose > 3) {
				char buf[64];

				snprintf(buf, sizeof(buf), "CONIND_number = %d", CONIND_number);
				print_string(child, buf);
				snprintf(buf, sizeof(buf), "TOKEN_value = %x", TOKEN_value);
				print_string(child, buf);
			}
			break;
		case N_ERROR_ACK:
			event = __TEST_ERROR_ACK;
			NPI_error = p->error_ack.NPI_error;
			last_errno = p->error_ack.UNIX_error;
			print_ack_prim(child, prim_string(p->type));
			print_string(child, nerrno_string(p->error_ack.NPI_error, p->error_ack.UNIX_error));
			break;
		case N_OK_ACK:
			event = __TEST_OK_ACK;
			print_ack_prim(child, prim_string(p->type));
			break;
		case N_UNITDATA_IND:
			event = __TEST_UNITDATA_IND;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length));
#else
			print_addrs(child, cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length);
#endif
			break;
		case N_UDERROR_IND:
			event = __TEST_UDERROR_IND;
			ADDR_buffer = (typeof(ADDR_buffer)) (ctrl->buf + p->uderror_ind.DEST_offset);
			ADDR_length = p->uderror_ind.DEST_length;
			RESERVED_field[0] = p->uderror_ind.RESERVED_field;
			ERROR_type = p->uderror_ind.ERROR_type;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->uderror_ind.DEST_offset, p->uderror_ind.DEST_length));
#else
			print_addrs(child, cbuf + p->uderror_ind.DEST_offset, p->uderror_ind.DEST_length);
#endif
			break;
		default:
			event = __EVENT_UNKNOWN;
			print_no_prim(child, p->type);
			break;
		}
		if (data && data->len >= 0)
			if ((last_event = do_decode_data(child, ctrl, data)) != __TEST_DATA)
				event = last_event;
	}
	return ((last_event = event));
}

static int
do_decode_msg(int child, struct strbuf *ctrl, struct strbuf *data)
{
	if (ctrl->len > 0) {
		if ((last_event = do_decode_ctrl(child, ctrl, data)) != __EVENT_UNKNOWN)
			return time_event(child, last_event);
	} else if (data->len > 0) {
		if ((last_event = do_decode_data(child, ctrl, data)) != __TEST_DATA)
			return time_event(child, last_event);
	}
	return ((last_event = __EVENT_NO_MSG));
}

int
wait_event(int child, int wait)
{
	while (1) {
		struct pollfd pfd[] = { {test_fd[child], POLLIN | POLLPRI, 0} };

		if (timer_timeout) {
			timer_timeout = 0;
			print_timeout(child);
			last_event = __EVENT_TIMEOUT;
			return time_event(child, __EVENT_TIMEOUT);
		}
		if (show && verbose > 4)
			print_syscall(child, "poll()");
		pfd[0].fd = test_fd[child];
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			if (errno == EINTR || errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		case 0:
			if (show && verbose > 4)
				print_success(child);
			print_nothing(child);
			last_event = __EVENT_NO_MSG;
			return time_event(child, __EVENT_NO_MSG);
		case 1:
			if (show && verbose > 4)
				print_success(child);
			if (pfd[0].revents) {
				int ret;

				ctrl.maxlen = BUFSIZE;
				ctrl.len = -1;
				ctrl.buf = cbuf;
				data.maxlen = BUFSIZE;
				data.len = -1;
				data.buf = dbuf;
				flags = 0;
				for (;;) {
					if ((verbose > 3 && show) || (verbose > 5 && show_msg))
						print_syscall(child, "getmsg()");
					if ((ret = getmsg(test_fd[child], &ctrl, &data, &flags)) >= 0)
						break;
					if (errno == EINTR || errno == ERESTART)
						continue;
					print_errno(child, (last_errno = errno));
					if (errno == EAGAIN)
						break;
					return __RESULT_FAILURE;
				}
				if (ret < 0)
					break;
				if (ret == 0) {
					if ((verbose > 3 && show) || (verbose > 5 && show_msg))
						print_success(child);
					if ((verbose > 3 && show) || (verbose > 5 && show_msg)) {
						int i;

						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from %d [%d,%d]:\n", child, ctrl.len, data.len);
						fprintf(stdout, "[");
						for (i = 0; i < ctrl.len; i++)
							fprintf(stdout, "%02X", (uint8_t) ctrl.buf[i]);
						fprintf(stdout, "]\n");
						fprintf(stdout, "[");
						for (i = 0; i < data.len; i++)
							fprintf(stdout, "%02X", (uint8_t) data.buf[i]);
						fprintf(stdout, "]\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
					}
					last_prio = (flags == RS_HIPRI);
					if ((last_event = do_decode_msg(child, &ctrl, &data)) != __EVENT_NO_MSG)
						return time_event(child, last_event);
				}
			}
		default:
			break;
		}
	}
	return __EVENT_UNKNOWN;
}

int
get_event(int child)
{
	return wait_event(child, -1);
}

int
get_data(int child, int action)
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

		if (test_getmsg(child, NULL, &data, &test_gflags) == __RESULT_FAILURE) {
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

		if (test_getpmsg(child, NULL, &data, &test_gband, &test_gflags) == __RESULT_FAILURE) {
			ret = last_retval;
			break;
		}
		ret = data.len;
		break;
	}
	}
	return (ret);
}

int
expect(int child, int wait, int want)
{
	if ((last_event = wait_event(child, wait)) == want)
		return (__RESULT_SUCCESS);
	print_expect(child, want);
	return (__RESULT_FAILURE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */
static int
preamble_0(int child)
{
	return (__RESULT_SUCCESS);
}

static int
postamble_0(int child)
{
	int failed = -1;

	while (1) {
		expect(child, SHORT_WAIT, __EVENT_NO_MSG);
		switch (last_event) {
		case __EVENT_NO_MSG:
		case __EVENT_TIMEOUT:
			break;
		case __RESULT_FAILURE:
			break;
		default:
			failed = (failed == -1) ? state : failed;
			state++;
			continue;
		}
		break;
	}
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}

/* premable and postamble for the NS_UNBND state. */
static int
preamble_1_unbnd(int child)
{
	if (preamble_0(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_UNBND)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
postamble_1_unbnd(int child)
{
	return postamble_0(child);
}

/* preamble and postamble for the NS_IDLE state. */
int
preamble_1_idle_clns(int child)
{
	unsigned short port = htons(TEST_PORT_NUMBER + child);
	unsigned char proto = TEST_PROTOCOL;
	struct sockaddr_in sin = { AF_INET, port, { INADDR_ANY } };
	unsigned char prot[] = { proto };

	return (__RESULT_NOTAPPL);
	if (preamble_1_unbnd(child) != __RESULT_SUCCESS)
		goto failure;
	ADDR_buffer = &sin;
	ADDR_length = sizeof(sin);
	CONIND_number = 0;
	BIND_flags = 0;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (CONIND_number != 0)
		goto failure;
	state++;
	if (TOKEN_value != 0)
		goto failure;
	state++;
	if (PROTOID_length != sizeof(prot))
		goto failure;
	state++;
	if (PROTOID_buffer[0] != proto)
		goto failure;
	state++;
	if (ADDR_length != sizeof(struct sockaddr_in))
		goto failure;
	state++;
	if (ADDR_buffer->sin_family != AF_INET)
		goto failure;
	state++;
	if (ADDR_buffer->sin_port != port)
		goto failure;
	state++;
	if (ADDR_buffer->sin_addr.s_addr != INADDR_ANY)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.SERV_type != N_CLNS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_IDLE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
preamble_1_idle_cons(int child)
{
	unsigned char proto = TEST_PROTOCOL;
	unsigned char prot[] = { proto };
	int coninds = (child == 2) ? 1 : 0;

	if (preamble_1_unbnd(child) != __RESULT_SUCCESS)
		goto failure;
	ADDR_buffer = addrs[child];
	ADDR_length = anums[child]*sizeof(addrs[child][0]);
	CONIND_number = coninds;
	BIND_flags = TOKEN_REQUEST;
	PROTOID_buffer = prot;
	PROTOID_length = sizeof(prot);
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (CONIND_number != coninds) {
		failure_string = FAILURE_STRING("bad CONIND_number");
		goto failure;
	}
	state++;
	if (TOKEN_value == 0) {
		failure_string = FAILURE_STRING("bad TOKEN_value");
		goto failure;
	}
	state++;
	if (PROTOID_length != 0 && PROTOID_length != sizeof(prot)) {
		failure_string = FAILURE_STRING("bad PROTOID_length");
		goto failure;
	}
	state++;
	if (PROTOID_length != 0 && PROTOID_buffer[0] != proto) {
		failure_string = FAILURE_STRING("bad PROTOID_buffer");
		goto failure;
	}
	state++;
	if (ADDR_length == 0) {
		failure_string = FAILURE_STRING("zero ADDR_length");
		goto failure;
	}
	state++;
	if (ADDR_length != anums[child] * sizeof(addrs[child][0])) {
		failure_string = FAILURE_STRING("bad ADDR_length");
		goto failure;
	}
	state++;
	if (ADDR_buffer->sin_family != AF_INET) {
		failure_string = FAILURE_STRING("bad sin_family");
		goto failure;
	}
	state++;
	if (ADDR_buffer->sin_addr.s_addr != addrs[child][0].sin_addr.s_addr) {
		failure_string = FAILURE_STRING("bad sin_addr");
		goto failure;
	}
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.SERV_type != N_CONS) {
		failure_string = FAILURE_STRING("bad SERV_type");
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}
#if 0
#define preamble_1_idle preamble_1_idle_clns
#else
#define preamble_1_idle preamble_1_idle_cons
#endif
static int
postamble_1_idle(int child)
{
	int failed = -1;

	if (do_signal(child, __TEST_UNBIND_REQ) != __RESULT_SUCCESS)
		goto cannot_unbind;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
      cannot_unbind:
	state++;
	if (postamble_1_unbnd(child) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed == -1)
		return (__RESULT_SUCCESS);
	state = failed;
	return (__RESULT_FAILURE);
}

/* preamble and postamble for the NS_DATA_XFER state. */
static int
postamble_1_data_xfer(int child)
{
	int failed = -1;

	ADDR_buffer = NULL;
	ADDR_length = 0;
	DISCON_reason = N_REASON_UNDEFINED;
	SEQ_number = 0;
	DATA_buffer = NULL;
	DATA_length = 0;

	if (expect(child, SHORT_WAIT * (1 + child), __TEST_DISCON_IND) == __RESULT_SUCCESS)
		goto cannot_disconnect;
	state++;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto cannot_disconnect;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS) {
		/* Note: disconnect indication could have flushed our
		 * disconnect request before acknowledgement */
		if (last_event != __TEST_DISCON_IND)
			if (last_event != __TEST_ERROR_ACK || NPI_error != NOUTSTATE)
				failed = (failed == -1) ? state : failed;
	}
      cannot_disconnect:
	state++;
	if (postamble_1_idle(child) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed == -1)
		return (__RESULT_SUCCESS);
	state = failed;
	return (__RESULT_FAILURE);
}
int
postamble_1_refuse(int child)
{
	int failed = -1;

	if (child != 2)
		goto cannot_refuse;
	if (expect(child, SHORT_WAIT * (1 + child), __TEST_DISCON_IND) == __RESULT_SUCCESS)
		goto cannot_refuse;
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	DISCON_reason = N_REASON_UNDEFINED;
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto cannot_refuse;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS) {
		/* Note: disconnect indication could have flushed our
		 * disconnect request before acknowledgement */
		if (last_event != __TEST_DISCON_IND)
			if (last_event != __TEST_ERROR_ACK || NPI_error != NOUTSTATE)
				failed = (failed == -1) ? state : failed;
	}
      cannot_refuse:
	state++;
	if (postamble_1_idle(child) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed == -1)
		return (__RESULT_SUCCESS);
	state = failed;
	return (__RESULT_FAILURE);
}

static int
preamble_1_wres_cind_conn(int child)
{
	char buf[] = "xxxxTest Data";

	if (preamble_1_idle_cons(child) != __RESULT_SUCCESS)
		goto failure;
	state++;

	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
#if 0
	DATA_buffer = buf;
	DATA_length = 14;
#else
	DATA_buffer = NULL;
	DATA_length = 0;
#endif

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&addrs[child][0].sin_port, &buf[0], sizeof(addrs[child][0].sin_port));
	bcopy(&addrs[child][0].sin_port, &buf[2], sizeof(addrs[child][0].sin_port));

	/* wait for listener to bind */
	test_msleep(child, NORMAL_WAIT);
	state++;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
#if 0
	if (expect(child, NORMAL_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
#endif
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
preamble_2_wres_cind_conn(int child)
{
	char buf[] = "xxxxTest Data";

	if (preamble_1_idle_cons(child) != __RESULT_SUCCESS)
		goto failure;
	state++;

	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
#if 1
	DATA_buffer = buf;
	DATA_length = 14;
#else
	DATA_buffer = NULL;
	DATA_length = 0;
#endif

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&addrs[child][0].sin_port, &buf[0], sizeof(addrs[child][0].sin_port));
	bcopy(&addrs[child][0].sin_port, &buf[2], sizeof(addrs[child][0].sin_port));

	/* wait for listener to bind */
	test_msleep(child, NORMAL_WAIT);
	state++;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
#if 0
	if (expect(child, NORMAL_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
#endif
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
preamble_1_wres_cind_resp(int child)
{
	if (preamble_1_idle_cons(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
preamble_1_wres_cind_list(int child)
{
	if (preamble_1_idle_cons(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int
preamble_1_data_xfer_conn(int child)
{
	if (preamble_1_wres_cind_conn(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_DATA_XFER)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
preamble_2_data_xfer_conn(int child)
{
	if (preamble_2_wres_cind_conn(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_DATA_XFER)
		goto failure;
	state++;
	if (expect(child, LONGER_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
#if 0
static int
preamble_1_data_xfer_resp(int child)
{
	if (preamble_1_wres_cind_resp(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
#endif
int
preamble_2_data_xfer_resp(int child)
{
	if (preamble_1_wres_cind_resp(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONGER_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
preamble_1_data_xfer_list(int child)
{
	char buf[] = "xxxxTest Data";
#if 0
	N_qos_sel_conn_ip_t test_qos = {
		.n_qos_type = N_QOS_SEL_CONN_IP,
		.protocol = QOS_UNKNOWN,
		.priority = 0,
		.ttl = 64,
		.tos = N_ROUTINE | N_NOTOS,
		.mtu = 536,
		.saddr = QOS_UNKNOWN,
		.daddr = QOS_UNKNOWN,
	};
#else
	N_qos_sel_conn_sctp_t test_qos = {
		.n_qos_type = N_QOS_SEL_CONN_SCTP,
		.i_streams = 1,
		.o_streams = 1,
	};
#endif

	if (preamble_1_wres_cind_list(child) != __RESULT_SUCCESS)
		goto failure;
	state++;

	// test_resfd = test_fd[child];
	TOKEN_value = tok[child];
	ADDR_buffer = addrs[child];
	ADDR_length = anums[child]*sizeof(addrs[child][0]);
	CONN_flags = 0;
	QOS_buffer = &test_qos;
	QOS_length = sizeof(test_qos);
#if 0
	DATA_buffer = buf;
	DATA_length = 14;
#else
	DATA_buffer = NULL;
	DATA_length = 0;
#endif

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&addrs[child][0].sin_port, &buf[0], sizeof(addrs[child][0].sin_port));
	bcopy(&addrs[child][0].sin_port, &buf[2], sizeof(addrs[child][0].sin_port));

	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	SEQ_number = 0;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_DATA_XFER)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
static int
preamble_2_data_xfer_list(int child)
{
	char buf[] = "xxxxTest Data";
#if 0
	N_qos_sel_conn_ip_t test_qos = {
		.n_qos_type = N_QOS_SEL_CONN_IP,
		.protocol = QOS_UNKNOWN,
		.priority = 0,
		.ttl = 64,
		.tos = N_ROUTINE | N_NOTOS,
		.mtu = 536,
		.saddr = QOS_UNKNOWN,
		.daddr = QOS_UNKNOWN,
	};
#else
	N_qos_sel_conn_sctp_t test_qos = {
		.n_qos_type = N_QOS_SEL_CONN_SCTP,
		.i_streams = 1,
		.o_streams = 1,
	};
#endif

	if (preamble_1_wres_cind_list(child) != __RESULT_SUCCESS)
		goto failure;
	state++;

	// test_resfd = test_fd[child];
	TOKEN_value = tok[child];
	ADDR_buffer = addrs[child];
	ADDR_length = anums[child]*sizeof(addrs[child][0]);
	CONN_flags = 0;
	QOS_buffer = &test_qos;
	QOS_length = sizeof(test_qos);
#if 1
	DATA_buffer = buf;
	DATA_length = 14;
#else
	DATA_buffer = NULL;
	DATA_length = 0;
#endif

	/* source and destination port, port number in the destination address is meaningless. */
	bcopy(&addrs[child][0].sin_port, &buf[0], sizeof(addrs[child][0].sin_port));
	bcopy(&addrs[child][0].sin_port, &buf[2], sizeof(addrs[child][0].sin_port));

	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	SEQ_number = 0;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != NS_DATA_XFER)
		goto failure;
	state++;
	if (expect(child, LONGER_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
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
 *  Check test case guard timer.
 */
#define test_group_0 "0. Sanity checks"
#define test_group_0_1 "0.1. Guard timers"
#define tgrp_case_0_1 test_group_0
#define sgrp_case_0_1 test_group_0_1
#define numb_case_0_1 "0.1"
#define name_case_0_1 "Check test case guard timer."
#define sref_case_0_1 "(none)"
#define desc_case_0_1 "\
Checks that the test case guard timer will fire and bring down the children."

int
test_case_0_1(int child)
{
	test_sleep(child, 40);
	return (__RESULT_SUCCESS);
}

#define preamble_0_1		preamble_0
#define postamble_0_1		postamble_0

struct test_stream test_0_1_conn = { &preamble_0_1, &test_case_0_1, &postamble_0_1 };
struct test_stream test_0_1_resp = { &preamble_0_1, &test_case_0_1, &postamble_0_1 };
struct test_stream test_0_1_list = { &preamble_0_1, &test_case_0_1, &postamble_0_1 };

#define tgrp_case_1 "Options Management"
#define sgrp_case_1 ""
/*
 *  Do options management.
 */
#define tgrp_case_1_1 tgrp_case_1
#define sgrp_case_1_1 sgrp_case_1
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Do options management."
#define sref_case_1_1 "NPI Rev 2.0.0"
#define desc_case_1_1 "\
Checks that options management can be performed on several streams\n\
and that one stream can be bound and unbound."
int
test_case_1_1(int child, void *add_ptr, size_t add_len, int coninds)
{
	QOS_buffer = &qos_info;
	QOS_length = sizeof(qos_info);
	OPTMGMT_flags = 0;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (add_len) {
		ADDR_buffer = add_ptr;
		ADDR_length = add_len;
		CONIND_number = coninds;
		PROTOID_buffer = NULL;
		PROTOID_length = 0;
		BIND_flags = TOKEN_REQUEST;
		if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		QOS_buffer = &qos_info;
		QOS_length = sizeof(qos_info);
		OPTMGMT_flags = 0;
		if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (do_signal(child, __TEST_UNBIND_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_1_1_conn(int child)
{
	return test_case_1_1(child, addrs[child], sizeof(addrs[child]), 0);
}

int
test_case_1_1_list(int child)
{
	return test_case_1_1(child, NULL, 0, 0);
}

int
test_case_1_1_resp(int child)
{
	return test_case_1_1(child, NULL, 0, 0);
}

#define preamble_1_1_conn preamble_0
#define preamble_1_1_resp preamble_0
#define preamble_1_1_list preamble_0

#define postamble_1_1_conn postamble_0
#define postamble_1_1_resp postamble_0
#define postamble_1_1_list postamble_0

struct test_stream test_1_1_conn = { &preamble_1_1_conn, &test_case_1_1_conn, &postamble_1_1_conn };
struct test_stream test_1_1_resp = { &preamble_1_1_resp, &test_case_1_1_resp, &postamble_1_1_resp };
struct test_stream test_1_1_list = { &preamble_1_1_list, &test_case_1_1_list, &postamble_1_1_list };

/*
 *  Bind and unbind three streams.
 */
#define tgrp_case_1_2 tgrp_case_1
#define sgrp_case_1_2 sgrp_case_1
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Bind and unbind three streams."
#define sref_case_1_2 "NPI Rev 2.0.0"
#define desc_case_1_2 "\
Checks that three streams can be bound and unbound with\n\
one stream as listener."
int
test_case_1_2(int child)
{
	ADDR_buffer = addrs[child];
	ADDR_length = anums[child]*sizeof(addrs[child][0]);
	CONIND_number = (child == 1) ? 5 : 0;
	PROTOID_buffer = NULL;
	PROTOID_length = 0;
	BIND_flags = TOKEN_REQUEST;
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_UNBIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_2_conn test_case_1_2
#define test_case_1_2_list test_case_1_2
#define test_case_1_2_resp test_case_1_2

#define preamble_1_2_conn preamble_0
#define preamble_1_2_resp preamble_0
#define preamble_1_2_list preamble_0

#define postamble_1_2_conn postamble_0
#define postamble_1_2_resp postamble_0
#define postamble_1_2_list postamble_0

struct test_stream test_1_2_conn = { &preamble_1_2_conn, &test_case_1_2_conn, &postamble_1_2_conn };
struct test_stream test_1_2_resp = { &preamble_1_2_resp, &test_case_1_2_resp, &postamble_1_2_resp };
struct test_stream test_1_2_list = { &preamble_1_2_list, &test_case_1_2_list, &postamble_1_2_list };

#define tgrp_case_2 "Failed connections."
#define sgrp_case_2 ""
/*
 *  Attempt a connection with no listener.
 */
#define tgrp_case_2_1 tgrp_case_2
#define sgrp_case_2_1 sgrp_case_2
#define numb_case_2_1 "2.1"
#define name_case_2_1 "Attempt a connection with no listener."
#define sref_case_2_1 "NPI Rev 2.0.0"
#define desc_case_2_1 "\
Attempts a connection with no listener.  The connection attempt\n\
should time out."
int
test_case_2_1_conn(int child)
{
	int i;

	QOS_buffer = &qos_info;
	QOS_length = sizeof(qos_info);
	OPTMGMT_flags = 0;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = addrs[child];
	ADDR_length = anums[child]*sizeof(addrs[child][0]);
	CONIND_number = 0;
	PROTOID_buffer = NULL;
	PROTOID_length = 0;
	BIND_flags = TOKEN_REQUEST;
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	for (i = 0; i < 25; i++) {
		state++;
		if (expect(child, LONG_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
			test_sleep(child, qos_info.rto_max * (qos_info.max_inits + 1) / 100);
		else
			break;
	}
	if (i == 25)
		goto failure;
	state++;
	if (do_signal(child, __TEST_UNBIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) == __RESULT_FAILURE)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_2_1_list(int child)
{
	return (__RESULT_SUCCESS);
}

int
test_case_2_1_resp(int child)
{
	return (__RESULT_SUCCESS);
}

#define preamble_2_1_conn preamble_0
#define preamble_2_1_resp preamble_0
#define preamble_2_1_list preamble_0

#define postamble_2_1_conn postamble_0
#define postamble_2_1_resp postamble_0
#define postamble_2_1_list postamble_0

struct test_stream test_2_1_conn = { &preamble_2_1_conn, &test_case_2_1_conn, &postamble_2_1_conn };
struct test_stream test_2_1_resp = { &preamble_2_1_resp, &test_case_2_1_resp, &postamble_2_1_resp };
struct test_stream test_2_1_list = { &preamble_2_1_list, &test_case_2_1_list, &postamble_2_1_list };

/*
 *  Attempt and withdraw a connection request.
 */
#define tgrp_case_2_2 tgrp_case_2
#define sgrp_case_2_2 sgrp_case_2
#define numb_case_2_2 "2.2"
#define name_case_2_2 "Attempt and withdraw a connection request."
#define sref_case_2_2 "NPI Rev 2.0.0"
#define desc_case_2_2 "\
Attempts and then withdraws a connection request.  The connection\n\
should disconnect at both ends."
int
test_case_2_2_conn(int child)
{
	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	SEQ_number = 0;
	DISCON_reason = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_2_2_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONGER_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_2_2_resp(int child)
{
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
}

#define preamble_2_2_conn preamble_1_idle_cons
#define preamble_2_2_resp preamble_1_idle_cons
#define preamble_2_2_list preamble_1_idle_cons

#define postamble_2_2_conn postamble_1_idle
#define postamble_2_2_resp postamble_1_idle
#define postamble_2_2_list postamble_1_idle

struct test_stream test_2_2_conn = { &preamble_2_2_conn, &test_case_2_2_conn, &postamble_2_2_conn };
struct test_stream test_2_2_resp = { &preamble_2_2_resp, &test_case_2_2_resp, &postamble_2_2_resp };
struct test_stream test_2_2_list = { &preamble_2_2_list, &test_case_2_2_list, &postamble_2_2_list };

#define tgrp_case_3 "Refused connections."
#define sgrp_case_3 ""
/*
 *  Attempt and refuse a connection request.
 */
#define tgrp_case_3_1 tgrp_case_3
#define sgrp_case_3_1 sgrp_case_3
#define numb_case_3_1 "3.1"
#define name_case_3_1 "Attempt and refuse a connection request."
#define sref_case_3_1 "NPI Rev 2.0.0"
#define desc_case_3_1 "\
Attempts a connection which is refused by the receiving end.\n\
The connection should disconnect at the attempting end."
int
test_case_3_1_conn(int child)
{
	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	goto success;
      success:
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_3_1_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	// SEQ_number = seq[child];
	DISCON_reason = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_3_1_resp(int child)
{
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
}

#define preamble_3_1_conn preamble_1_idle_cons
#define preamble_3_1_resp preamble_1_idle_cons
#define preamble_3_1_list preamble_1_idle_cons

#define postamble_3_1_conn postamble_1_idle
#define postamble_3_1_resp postamble_1_idle
#define postamble_3_1_list postamble_1_idle

struct test_stream test_3_1_conn = { &preamble_3_1_conn, &test_case_3_1_conn, &postamble_3_1_conn };
struct test_stream test_3_1_resp = { &preamble_3_1_resp, &test_case_3_1_resp, &postamble_3_1_resp };
struct test_stream test_3_1_list = { &preamble_3_1_list, &test_case_3_1_list, &postamble_3_1_list };

/*
 *  Attempt and delayed refuse a connection request.
 */
#define tgrp_case_3_2 tgrp_case_3
#define sgrp_case_3_2 sgrp_case_3
#define numb_case_3_2 "3.2"
#define name_case_3_2 "Attempt and delayed refuse a connection request."
#define sref_case_3_2 "NPI Rev 2.0.0"
#define desc_case_3_2 "\
Attempts a delayed refusal of a connection requrest.  This delayed\n\
refusal should come after the connector has already timed out."
int
test_case_3_2_conn(int child)
{
	int i;

	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 25; i++)
		if (expect(child, LONG_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
			test_sleep(child, qos_info.rto_max * (qos_info.max_inits + 1) / 100);
		else
			break;
	if (i == 25)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_3_2_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	// SEQ_number = seq[child];
	DISCON_reason = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_3_2_resp(int child)
{
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
}

#define preamble_3_2_conn preamble_1_idle_cons
#define preamble_3_2_resp preamble_1_idle_cons
#define preamble_3_2_list preamble_1_idle_cons

#define postamble_3_2_conn postamble_1_idle
#define postamble_3_2_resp postamble_1_idle
#define postamble_3_2_list postamble_1_idle

struct test_stream test_3_2_conn = { &preamble_3_2_conn, &test_case_3_2_conn, &postamble_3_2_conn };
struct test_stream test_3_2_resp = { &preamble_3_2_resp, &test_case_3_2_resp, &postamble_3_2_resp };
struct test_stream test_3_2_list = { &preamble_3_2_list, &test_case_3_2_list, &postamble_3_2_list };

#define tgrp_case_4 "Accepted connections."
#define sgrp_case_4 ""
/*
 *  Accept a connection.
 */
#define tgrp_case_4_1 tgrp_case_4
#define sgrp_case_4_1 sgrp_case_4
#define numb_case_4_1 "4.1"
#define name_case_4_1 "Accept a connection."
#define sref_case_4_1 "NPI Rev 2.0.0"
#define desc_case_4_1 "\
Accept a connection and then disconnect.  This connection attempt\n\
should be successful."
int
test_case_4_1_conn(int child)
{
	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	if (expect(child, LONGER_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_4_1_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_resfd = test_fd[1];
	TOKEN_value = tok[1];
	ADDR_buffer = NULL;
	ADDR_length = 0;
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = NULL;
	QOS_length = 0;
	// SEQ_number = seq[child];
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_4_1_resp(int child)
{
	test_sleep(child, 1);
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	SEQ_number = 0;
	DISCON_reason = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_4_1_conn preamble_1_idle_cons
#define preamble_4_1_resp preamble_1_idle_cons
#define preamble_4_1_list preamble_1_idle_cons

#define postamble_4_1_conn postamble_1_idle
#define postamble_4_1_resp postamble_1_idle
#define postamble_4_1_list postamble_1_idle

struct test_stream test_4_1_conn = { &preamble_4_1_conn, &test_case_4_1_conn, &postamble_4_1_conn };
struct test_stream test_4_1_resp = { &preamble_4_1_resp, &test_case_4_1_resp, &postamble_4_1_resp };
struct test_stream test_4_1_list = { &preamble_4_1_list, &test_case_4_1_list, &postamble_4_1_list };

/*
 *  Attempt and delayed accept a connection request.
 */
#define tgrp_case_4_2 tgrp_case_4
#define sgrp_case_4_2 sgrp_case_4
#define numb_case_4_2 "4.2"
#define name_case_4_2 "Attempt and delayed accept a connection request."
#define sref_case_4_2 "NPI Rev 2.0.0"
#define desc_case_4_2 "\
Attempt a connection and delay the acceptance of the connection request.\n\
This should result in a disconnection indication after the connection is\n\
accepted.  "
int
test_case_4_2_conn(int child)
{
	int i;
	N_qos_sel_info_sctp_t test_qos_info;

	test_qos_info = qos_info;
	test_qos_info.rto_ini = 10;
	test_qos_info.rto_max = 20;
	test_qos_info.rto_min = 10;
	test_qos_info.max_inits = 1;
	QOS_buffer = &test_qos_info;
	QOS_length = sizeof(test_qos_info);
	OPTMGMT_flags = 0;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;

	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;

	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	for (i = 0; i < 25; i++)
		if (expect(child, LONG_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
			test_sleep(child, test_qos_info.rto_max * (test_qos_info.max_inits + 1) / 100 + 1);
		else
			break;
	if (i == 25)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_4_2_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 2);
	state++;
	test_resfd = test_fd[1];
	TOKEN_value = tok[1];
	ADDR_buffer = NULL;
	ADDR_length = 0;
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = NULL;
	QOS_length = 0;
	// SEQ_number = seq[child];
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_4_2_resp(int child)
{
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);

	DATA_xfer_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = "Hello!";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_4_2_conn preamble_1_idle_cons
#define preamble_4_2_resp preamble_1_idle_cons
#define preamble_4_2_list preamble_1_idle_cons

#define postamble_4_2_conn postamble_1_idle
#define postamble_4_2_resp postamble_1_idle
#define postamble_4_2_list postamble_1_idle

struct test_stream test_4_2_conn = { &preamble_4_2_conn, &test_case_4_2_conn, &postamble_4_2_conn };
struct test_stream test_4_2_resp = { &preamble_4_2_resp, &test_case_4_2_resp, &postamble_4_2_resp };
struct test_stream test_4_2_list = { &preamble_4_2_list, &test_case_4_2_list, &postamble_4_2_list };

#define tgrp_case_5 "Accepted connections with HMAC algorithm."
#define sgrp_case_5 ""
/*
 *  Accept a connection.
 */
#define tgrp_case_5_1 tgrp_case_5
#define sgrp_case_5_1 sgrp_case_5
#define numb_case_5_1 "5.1"
#define name_case_5_1 "Accept a connection."
#define sref_case_5_1 "NPI Rev 2.0.0"
#define desc_case_5_1 "\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the SCTP_HMAC_NONE signature on its cookie."
int
test_case_5_1_conn(int child)
{
	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	SEQ_number = 0;
	DISCON_reason = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_5_1_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_resfd = test_fd[1];
	TOKEN_value = tok[1];
	ADDR_buffer = NULL;
	ADDR_length = 0;
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = NULL;
	QOS_length = 0;
	// SEQ_number = seq[child];
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_5_1_resp(int child)
{
	test_sleep(child, 1);
	state++;
	if (expect(child, LONGER_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_5_1_conn preamble_1_idle_cons
#define preamble_5_1_resp preamble_1_idle_cons
#define preamble_5_1_list preamble_1_idle_cons

#define postamble_5_1_conn postamble_1_idle
#define postamble_5_1_resp postamble_1_idle
#define postamble_5_1_list postamble_1_idle

struct test_stream test_5_1_conn = { &preamble_5_1_conn, &test_case_5_1_conn, &postamble_5_1_conn };
struct test_stream test_5_1_resp = { &preamble_5_1_resp, &test_case_5_1_resp, &postamble_5_1_resp };
struct test_stream test_5_1_list = { &preamble_5_1_list, &test_case_5_1_list, &postamble_5_1_list };

/*
 *  Accept a connection (MD5 hashed cookie)
 */
#define tgrp_case_5_2 tgrp_case_5
#define sgrp_case_5_2 sgrp_case_5
#define numb_case_5_2 "5.2"
#define name_case_5_2 "Accept a connection (MD5 hashed cookie)"
#define sref_case_5_2 "NPI Rev 2.0.0"
#define desc_case_5_2 "\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the SCTP_HMAC_MD5 signature on its cookie."
int
test_case_5_2_conn(int child)
{
	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	SEQ_number = 0;
	DISCON_reason = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_5_2_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_resfd = test_fd[1];
	TOKEN_value = tok[1];
	ADDR_buffer = NULL;
	ADDR_length = 0;
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = NULL;
	QOS_length = 0;
	// SEQ_number = seq[child];
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_5_2_resp(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_5_2_conn preamble_1_idle_cons
#define preamble_5_2_resp preamble_1_idle_cons
#define preamble_5_2_list preamble_1_idle_cons

#define postamble_5_2_conn postamble_1_idle
#define postamble_5_2_resp postamble_1_idle
#define postamble_5_2_list postamble_1_idle

struct test_stream test_5_2_conn = { &preamble_5_2_conn, &test_case_5_2_conn, &postamble_5_2_conn };
struct test_stream test_5_2_resp = { &preamble_5_2_resp, &test_case_5_2_resp, &postamble_5_2_resp };
struct test_stream test_5_2_list = { &preamble_5_2_list, &test_case_5_2_list, &postamble_5_2_list };

/*
 *  Accept a connection (SHA1 hashed cookie)
 */
#define tgrp_case_5_3 tgrp_case_5
#define sgrp_case_5_3 sgrp_case_5
#define numb_case_5_3 "5.3"
#define name_case_5_3 "Accept a connection (SHA1 hashed cookie)"
#define sref_case_5_3 "NPI Rev 2.0.0"
#define desc_case_5_3 "\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the SCTP_HMAC_SHA_1 signature on its cookie."
int
test_case_5_3_conn(int child)
{
	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	ADDR_buffer = NULL;
	ADDR_length = 0;
	SEQ_number = 0;
	DISCON_reason = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_5_3_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_resfd = test_fd[1];
	TOKEN_value = tok[1];
	ADDR_buffer = NULL;
	ADDR_length = 0;
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = NULL;
	QOS_length = 0;
	// SEQ_number = seq[child];
	DATA_buffer = NULL;
	DATA_length = 0;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_5_3_resp(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_5_3_conn preamble_1_idle_cons
#define preamble_5_3_resp preamble_1_idle_cons
#define preamble_5_3_list preamble_1_idle_cons

#define postamble_5_3_conn postamble_1_idle
#define postamble_5_3_resp postamble_1_idle
#define postamble_5_3_list postamble_1_idle

struct test_stream test_5_3_conn = { &preamble_5_3_conn, &test_case_5_3_conn, &postamble_5_3_conn };
struct test_stream test_5_3_resp = { &preamble_5_3_resp, &test_case_5_3_resp, &postamble_5_3_resp };
struct test_stream test_5_3_list = { &preamble_5_3_list, &test_case_5_3_list, &postamble_5_3_list };

#define tgrp_case_6 "Connect with data."
#define sgrp_case_6 ""
/*
 *  Connect with data.
 */
#define tgrp_case_6_1 tgrp_case_6
#define sgrp_case_6_1 sgrp_case_6
#define numb_case_6_1 "6.1"
#define name_case_6_1 "Connect with data."
#define sref_case_6_1 "NPI Rev 2.0.0"
#define desc_case_6_1 "\
Attempt and accept a connection where data is also passed in the\n\
connection request and the connection response.  This should result\n\
in DATA chunks being bundled with the COOKIE-ECHO and COOKIE-ACK\n\
chunks in the SCTP messages."
int
test_case_6_1_conn(int child)
{
	int i;

	DATA_xfer_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = "Hellos Again!";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 21; i++)
		if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_6_1_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_6_1_list(int child)
{
	int i;

	if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	DATA_xfer_flags = 0;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = "Hello Too!";
	DATA_length = strlen(DATA_buffer);
	for (i = 0; i < 21; i++)
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
			goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_6_1_conn preamble_2_data_xfer_conn
#define preamble_6_1_resp preamble_1_idle_cons
#define preamble_6_1_list preamble_2_data_xfer_list

#define postamble_6_1_conn postamble_1_data_xfer
#define postamble_6_1_resp postamble_1_idle
#define postamble_6_1_list postamble_1_data_xfer

struct test_stream test_6_1_conn = { &preamble_6_1_conn, &test_case_6_1_conn, &postamble_6_1_conn };
struct test_stream test_6_1_resp = { &preamble_6_1_resp, &test_case_6_1_resp, &postamble_6_1_resp };
struct test_stream test_6_1_list = { &preamble_6_1_list, &test_case_6_1_list, &postamble_6_1_list };

/*
 *  Connect and send partial data.
 */
#define tgrp_case_6_2 tgrp_case_6
#define sgrp_case_6_2 sgrp_case_6
#define numb_case_6_2 "6.2"
#define name_case_6_2 "Connect and send partial data."
#define sref_case_6_2 "NPI Rev 2.0.0"
#define desc_case_6_2 "\
Connect and send partial data (i.e., data with more flag set)."
int
test_case_6_2_conn(int child)
{
	DATA_xfer_flags = N_MORE_DATA_FLAG;
	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = "Hello There.";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	DATA_xfer_flags = 0;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	DATA_xfer_flags = N_RC_FLAG | N_MORE_DATA_FLAG;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	DATA_xfer_flags = N_RC_FLAG;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
#if 1
	if (expect(child, LONGER_WAIT, __TEST_DATACK_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
#endif
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_6_2_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_6_2_list(int child)
{
	/* Expect 2 message sequences, that can be coallesced. */
	do {
		if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
	} while ((DATA_xfer_flags & N_MORE_DATA_FLAG) != 0);
	do {
		if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
	} while ((DATA_xfer_flags & N_MORE_DATA_FLAG) != 0);
	test_msleep(child, LONG_WAIT); /* must be longer than sack timeout */
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_6_2_conn preamble_1_data_xfer_conn
#define preamble_6_2_resp preamble_1_idle_cons
#define preamble_6_2_list preamble_1_data_xfer_list

#define postamble_6_2_conn postamble_1_data_xfer
#define postamble_6_2_resp postamble_1_idle
#define postamble_6_2_list postamble_1_data_xfer

struct test_stream test_6_2_conn = { &preamble_6_2_conn, &test_case_6_2_conn, &postamble_6_2_conn };
struct test_stream test_6_2_resp = { &preamble_6_2_resp, &test_case_6_2_resp, &postamble_6_2_resp };
struct test_stream test_6_2_list = { &preamble_6_2_list, &test_case_6_2_list, &postamble_6_2_list };

/*
 *  Connect and send partial data.
 */
#define tgrp_case_6_3 tgrp_case_6
#define sgrp_case_6_3 sgrp_case_6
#define numb_case_6_3 "6.3"
#define name_case_6_3 "Connect and send partial data."
#define sref_case_6_3 "NPI Rev 2.0.0"
#define desc_case_6_3 "\
Connect and send partial data and expedited data on multiple streams.\n\
Expedited data should be delivered between ordered data fragments on\n\
the same stream and delivered to the user first."
int
test_case_6_3_conn(int child)
{
#if 1
	int i;
#endif

	qos_data.sid = 0;
	DATA_xfer_flags = N_RC_FLAG | N_MORE_DATA_FLAG;
	QOS_buffer = &qos_data;
	QOS_length = sizeof(qos_data);
	DATA_buffer = "Hello There.";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 1;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 2;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 3;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 0;
	QOS_buffer = &qos_data;
	QOS_length = sizeof(qos_data);
	DATA_buffer = "Hi There.";
	DATA_length = strlen(DATA_buffer);
	do_signal(child, __TEST_EXDATA_REQ);
	qos_data.sid = 0;
	DATA_xfer_flags = N_RC_FLAG | N_MORE_DATA_FLAG;
	QOS_buffer = &qos_data;
	QOS_length = sizeof(qos_data);
	DATA_buffer = "Hello There.";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 1;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 2;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 3;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 0;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 1;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 2;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 3;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 0;
	DATA_xfer_flags = N_RC_FLAG;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 1;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 2;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	qos_data.sid = 3;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
#if 1
	for (i = 0; i < 4; i++) {
		state++;
		/* wait must be longer than sack timeout */
		if (expect(child, LONGER_WAIT, __TEST_DATACK_IND) != __RESULT_SUCCESS)
			break;
	}
	if (i < 4)
		goto failure;
	state++;
#endif
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_6_3_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_6_3_list(int child)
{
	int i;

	test_msleep(child, NORMAL_WAIT);
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 4; i++) {
		do {
			if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
				goto failure;
			state++;
		} while ((DATA_xfer_flags & N_MORE_DATA_FLAG) != 0);
	}
	test_msleep(child, LONG_WAIT); /* must be longer than sack timeout */
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_6_3_conn preamble_1_data_xfer_conn
#define preamble_6_3_resp preamble_1_idle_cons
#define preamble_6_3_list preamble_1_data_xfer_list

#define postamble_6_3_conn postamble_1_data_xfer
#define postamble_6_3_resp postamble_1_idle
#define postamble_6_3_list postamble_1_data_xfer

struct test_stream test_6_3_conn = { &preamble_6_3_conn, &test_case_6_3_conn, &postamble_6_3_conn };
struct test_stream test_6_3_resp = { &preamble_6_3_resp, &test_case_6_3_resp, &postamble_6_3_resp };
struct test_stream test_6_3_list = { &preamble_6_3_list, &test_case_6_3_list, &postamble_6_3_list };

#define tgrp_case_7 "Fragmented data."
#define sgrp_case_7 ""
/*
 *  Test fragmentation by sending very large packets.
 */
#define tgrp_case_7_1 tgrp_case_7
#define sgrp_case_7_1 sgrp_case_7
#define numb_case_7_1 "7.1"
#define name_case_7_1 "Test fragmentation by sending very large packets."
#define sref_case_7_1 "NPI Rev 2.0.0"
#define desc_case_7_1 "\
Connect and send very large packets to test fragmentation.  We send 4 50000\n\
byte packets, 4 expedited packets with the message \"Urgent.\" and 4 normal\n\
packets with the message \"Hello\".  The urgent packets should deliver between\n\
the fragments and but the normal packets should wait until the large packets\n\
are complete."
int
test_case_7_1_conn(int child)
{
	int i;
	char lbuf[50000];
	const char nrm[] = "Hello.";
	const char urg[] = "Urgent.";

	bzero(lbuf, sizeof(lbuf));
	for (i = 0; i < 4; i++) {
		if (expect(child, SHORT_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
			goto failure;
		state++;
		DATA_xfer_flags = 0;
		QOS_buffer = &qos_data;
		QOS_length = sizeof(qos_data);
		DATA_buffer = lbuf;
		DATA_length = sizeof(lbuf);
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
			state++;
			test_sleep(child, 1);
			DATA_xfer_flags = 0;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = lbuf;
			DATA_length = sizeof(lbuf);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				failure_string = "Could not send normal data!";
				goto failure;
			}
		}
		state++;
		if (expect(child, SHORT_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
			goto failure;
		state++;
		QOS_buffer = &qos_data;
		QOS_length = sizeof(qos_data);
		DATA_buffer = (char *)urg;
		DATA_length = sizeof(urg);
		if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS) {
			state++;
			test_sleep(child, 1);
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = (char *)urg;
			DATA_length = sizeof(urg);
			if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS) {
				failure_string = "Could not send expedited data!";
				goto failure;
			}
		}
		state++;
		if (expect(child, SHORT_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
			goto failure;
		state++;
		DATA_xfer_flags = 0;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = (char *)nrm;
		DATA_length = sizeof(nrm);
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
			state++;
			test_sleep(child, 1);
			DATA_xfer_flags = 0;
			QOS_buffer = NULL;
			QOS_length = 0;
			DATA_buffer = (char *)nrm;
			DATA_length = sizeof(nrm);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				failure_string = "Could not send normal data!";
				goto failure;
			}
		}
		state++;
	}
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_7_1_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_7_1_list(int child)
{
	size_t len = 0;

	while (len < 4 * 50000 + 4 * 8 + 4 * 7) {
		switch (wait_event(child, LONGER_WAIT)) {
		case __TEST_EXDATA_IND:
		case __TEST_DATA_IND:
			len += data.len;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			failure_string = event_string(child, last_event);
			goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "i = %d, j = %d, k = %d\n", 4, 4, 4);
	fprintf(stdout, "Received %lu bytes, expecting %u\n", (ulong) len, 4 * 50000 + 4 * 8 + 4 * 7);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#define preamble_7_1_conn preamble_1_data_xfer_conn
#define preamble_7_1_resp preamble_1_idle_cons
#define preamble_7_1_list preamble_1_data_xfer_list

#define postamble_7_1_conn postamble_1_data_xfer
#define postamble_7_1_resp postamble_1_idle
#define postamble_7_1_list postamble_1_data_xfer

struct test_stream test_7_1_conn = { &preamble_7_1_conn, &test_case_7_1_conn, &postamble_7_1_conn };
struct test_stream test_7_1_resp = { &preamble_7_1_resp, &test_case_7_1_resp, &postamble_7_1_resp };
struct test_stream test_7_1_list = { &preamble_7_1_list, &test_case_7_1_list, &postamble_7_1_list };

/*
 *  Test coallescing packets by sending many small fragmented pieces.
 */
#define tgrp_case_7_2 tgrp_case_7
#define sgrp_case_7_2 sgrp_case_7
#define numb_case_7_2 "7.2"
#define name_case_7_2 "Test coallescing packets by sending many small fragmented pieces."
#define sref_case_7_2 "NPI Rev 2.0.0"
#define desc_case_7_2 "\
Connect and send many small packets to test coallescing of packets."
int
test_case_7_2_conn(int child)
{
	int s = 0;
	size_t snd_bytes = 0;

	QOS_buffer = NULL;
	QOS_length = 0;
	DATA_buffer = "Hi There.";
	DATA_length = 10;

	DATA_xfer_flags = N_MORE_DATA_FLAG;

	while (s < 5000) {
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
			state++;
			print_more(child);
			test_sleep(child, 1);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				failure_string = event_string(child, last_event);
				goto failure;
			}
		}
		snd_bytes += DATA_length;
		s++;
		state++;
		if (s > 3) {
			if (s > 9997)
				print_more(child);
			else
				print_less(child);
		}
	}
	state++;

	DATA_xfer_flags = 0;
	do_signal(child, __TEST_DATA_REQ);
	snd_bytes += DATA_length;
	state++;
	print_more(child);
	state++;
	test_sleep(child, 3);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "Sent %3d messages making %6lu bytes.\n", s, (ulong) snd_bytes);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	print_more(child);
	return (__RESULT_FAILURE);
}

int
test_case_7_2_resp(int child)
{
	test_sleep(child, 3);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_7_2_list(int child)
{
	int r = 0;
	size_t rcv_bytes = 0;
	int joined = 0;

	test_sleep(child, 2);
	state++;
	while (r < 4) {
		if (expect(child, LONGER_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS) {
			failure_string = event_string(child, last_event);
			goto failure;
		}
		if (data.len > 10)
			joined = 1;
		rcv_bytes += data.len;
		r++;
		state++;
	}
	print_less(child);
	while (rcv_bytes < 50000 + 10) {
		if (expect(child, LONGER_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS) {
			failure_string = event_string(child, last_event);
			goto failure;
		}
		if (data.len > 10)
			joined = 1;
		rcv_bytes += data.len;
		r++;
		state++;
	}
	if (!joined) {
		failure_string = "No joined message.";
		goto failure;
	}
	state++;
	print_more(child);
	state++;
	test_sleep(child, 2);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "Rcvd %3d messages making %6lu bytes.\n", r, (ulong) rcv_bytes);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	print_more(child);
	return (__RESULT_FAILURE);
}

#define preamble_7_2_conn preamble_1_data_xfer_conn
#define preamble_7_2_resp preamble_1_idle_cons
#define preamble_7_2_list preamble_1_data_xfer_list

#define postamble_7_2_conn postamble_1_data_xfer
#define postamble_7_2_resp postamble_1_idle
#define postamble_7_2_list postamble_1_data_xfer

struct test_stream test_7_2_conn = { &preamble_7_2_conn, &test_case_7_2_conn, &postamble_7_2_conn };
struct test_stream test_7_2_resp = { &preamble_7_2_resp, &test_case_7_2_resp, &postamble_7_2_resp };
struct test_stream test_7_2_list = { &preamble_7_2_list, &test_case_7_2_list, &postamble_7_2_list };

#define tgrp_case_8 "Connections with data and receipt confirmation."
#define sgrp_case_8 ""
/*
 *  Connect with data.
 */
#define tgrp_case_8_1 tgrp_case_8
#define sgrp_case_8_1 sgrp_case_8
#define numb_case_8_1 "8.1"
#define name_case_8_1 "Connect with data."
#define sref_case_8_1 "NPI Rev 2.0.0"
#define desc_case_8_1 "\
Connect with data in the connection request and response, and\n\
send data with receipt confirmation set.  Check that the data\n\
returns an acknowledgement when it is SACK'ed by the other end."
int
test_case_8_1_conn(int child)
{
	int i = 0, j = 0, k, l;

	for (k = 0; k < 2; k++) {
		for (l = 0; l < 6; l++) {
			DATA_xfer_flags = N_RC_FLAG;
			QOS_buffer = NULL;
			QOS_length = 0;
			DATA_buffer = "Hellos Again!";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
				goto failure;
			state++;
		}
		for (i += 6, j += 21; i > 0 || j > 0; state++) {
			/* wait must be longer than sack timeout */
			switch (wait_event(child, LONGER_WAIT)) {
			case __TEST_DATACK_IND:
				i--;
				continue;
			case __TEST_DATA_IND:
				j--;
				continue;
			case __EVENT_NO_MSG:
				continue;
			default:
				fprintf(stdout, "i=%d, j=%d\n", i, j);
				goto failure;
			}
		}
		test_msleep(child, LONG_WAIT);
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_8_1_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_8_1_list(int child)
{
	int i = 0, j = 0, k, l;

	for (k = 0; k < 2; k++) {
		for (l = 0; l < 21; l++) {
			DATA_xfer_flags = N_RC_FLAG;
			QOS_buffer = NULL;
			QOS_length = 0;
			DATA_buffer = "Hellos Too!";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
				goto failure;
			state++;
		}
		for (i += 6, j += 21; i > 0 || j > 0; state++) {
			/* wait must be longer than sack timeout */
			switch (wait_event(child, LONGER_WAIT)) {
			case __TEST_DATACK_IND:
				j--;
				continue;
			case __TEST_DATA_IND:
				i--;
				continue;
			case __EVENT_NO_MSG:
				continue;
			default:
				fprintf(stdout, "i=%d, j=%d\n", i, j);
				goto failure;
			}
		}
		test_msleep(child, LONG_WAIT);
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_8_1_conn preamble_2_data_xfer_conn
#define preamble_8_1_resp preamble_1_idle_cons
#define preamble_8_1_list preamble_2_data_xfer_list

#define postamble_8_1_conn postamble_1_data_xfer
#define postamble_8_1_resp postamble_1_idle
#define postamble_8_1_list postamble_1_data_xfer

struct test_stream test_8_1_conn = { &preamble_8_1_conn, &test_case_8_1_conn, &postamble_8_1_conn };
struct test_stream test_8_1_resp = { &preamble_8_1_resp, &test_case_8_1_resp, &postamble_8_1_resp };
struct test_stream test_8_1_list = { &preamble_8_1_list, &test_case_8_1_list, &postamble_8_1_list };

/*
 *  Data with default receipt confirmation.
 */
#define tgrp_case_8_2 tgrp_case_8
#define sgrp_case_8_2 sgrp_case_8
#define numb_case_8_2 "8.2"
#define name_case_8_2 "Data with default receipt confirmation."
#define sref_case_8_2 "NPI Rev 2.0.0"
#define desc_case_8_2 "\
Connect with data and transfer data with default receipt confirmation.\n\
This would require that an explicit acknowledgement is requested for\n\
each data (but we don't support this as SCTP cannot handle it)."
int
test_case_8_2_conn(int child)
{
	int i;
	uint32_t tsn1[10];

	ADDR_buffer = addrs[2];
	ADDR_length = anums[2]*sizeof(addrs[2][0]);
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = &qos_conn;
	QOS_length = sizeof(qos_conn);
	DATA_buffer = "Hello World!";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	tsn1[0] = tsn[child];
	for (i = 1; i < 5; i++) {
		DATA_xfer_flags = N_RC_FLAG;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = "Hellos Again!";
		DATA_length = strlen(DATA_buffer);
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
			break;
	}
	if (i < 5)
		goto failure;
	state++;
	qos_data.tsn = tsn1[0];
	QOS_buffer = &qos_data;
	QOS_length = sizeof(qos_data);
#if 0
	if (do_signal(child, __TEST_DATACK_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
#endif
	test_sleep(child, qos_info.rto_max / 100 + 1);
#if 1
	for (i = 1; i < 5; i++)
		/* wait must be longer than sack timeout */
		if (expect(child, LONGER_WAIT, __TEST_DATACK_IND) != __RESULT_SUCCESS)
			break;
	if (i != 5)
		goto failure;
	state++;
#endif
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_8_2_list(int child)
{
	if (expect(child, LONGER_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_resfd = test_fd[1];
	TOKEN_value = tok[1];
	ADDR_buffer = NULL;
	ADDR_length = 0;
	CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
	QOS_buffer = NULL;
	QOS_length = 0;
	// SEQ_number = seq[child];
	DATA_buffer = "Hello There!";
	DATA_length = strlen(DATA_buffer);
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_8_2_resp(int child)
{
	int i;
	uint32_t tsn3[10];

	if (expect(child, LONGER_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	tsn3[0] = tsn[child];
	test_sleep(child, qos_info.rto_max / 100 + 1);
	for (i = 1; i < 5; i++)
		if (expect(child, NORMAL_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
			break;
		else
			tsn3[i] = tsn[child];
	if (i != 5)
		goto failure;
	state++;
#if 0
	for (i = 0; i < 5; i++) {
		qos_data.tsn = tsn3[i];
		QOS_buffer = &qos_data;
		QOS_length = sizeof(qos_data);
		do_signal(child, __TEST_DATACK_REQ);
	}
	state++;
#endif
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_8_2_conn preamble_1_idle_cons
#define preamble_8_2_resp preamble_1_idle_cons
#define preamble_8_2_list preamble_1_idle_cons

#define postamble_8_2_conn postamble_1_data_xfer
#define postamble_8_2_resp postamble_1_data_xfer
#define postamble_8_2_list postamble_1_data_xfer

struct test_stream test_8_2_conn = { &preamble_8_2_conn, &test_case_8_2_conn, &postamble_8_2_conn };
struct test_stream test_8_2_resp = { &preamble_8_2_resp, &test_case_8_2_resp, &postamble_8_2_resp };
struct test_stream test_8_2_list = { &preamble_8_2_list, &test_case_8_2_list, &postamble_8_2_list };

#define tgrp_case_9 "Delivery of data under noise."
#define sgrp_case_9 ""
/*
 *  Delivering ordered data under noise.
 */
#define tgrp_case_9_1 tgrp_case_9
#define sgrp_case_9_1 sgrp_case_9
#define numb_case_9_1 "9.1"
#define name_case_9_1 "Delivering ordered data under noise."
#define sref_case_9_1 "NPI Rev 2.0.0"
#define desc_case_9_1 "\
Delivery of ordered data under noise with acknowledgement."
#if 1
#define TEST_PACKETS 300
#else
#define TEST_PACKETS 30
#endif
int
test_case_9_1_conn(int child)
{
	int i = TEST_PACKETS, j = TEST_PACKETS, k = TEST_PACKETS;

	test_msleep(child, NORMAL_WAIT);
	state++;
	while (i > 0) {
		DATA_xfer_flags = N_RC_FLAG;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = "Pattern-1";
		DATA_length = strlen(DATA_buffer);
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		i--;
	}
	while (j > 0 || k > 0) {
		/* wait must be longer than sack timeout */
		switch (wait_event(child, LONGER_WAIT)) {
		case __TEST_DATA_IND:
			j--;
			break;
		case __TEST_DATACK_IND:
			k--;
			break;
		default:
			goto failure;
		}
		state++;
	}
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d acks %d\n", child, TEST_PACKETS - i, TEST_PACKETS - j, TEST_PACKETS - k);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

int
test_case_9_1_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_9_1_list(int child)
{
	int i = TEST_PACKETS, j = TEST_PACKETS, k = TEST_PACKETS;

	test_msleep(child, NORMAL_WAIT);
	state++;
	while (i > 0) {
		DATA_xfer_flags = N_RC_FLAG;
		QOS_buffer = NULL;
		QOS_length = 0;
		DATA_buffer = "Pattern-3";
		DATA_length = strlen(DATA_buffer);
		if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
			goto failure;
		i--;
	}
	while (j > 0 || k > 0) {
		/* wait must be longer than sack timeout */
		switch (wait_event(child, LONGER_WAIT)) {
		case __TEST_DATA_IND:
			j--;
			break;
		case __TEST_DATACK_IND:
			k--;
			break;
		default:
			goto failure;
		}
		state++;
	}
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d acks %d\n", child, TEST_PACKETS - i, TEST_PACKETS - j, TEST_PACKETS - k);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#undef TEST_PACKETS

#define preamble_9_1_conn preamble_1_data_xfer_conn
#define preamble_9_1_resp preamble_1_idle_cons
#define preamble_9_1_list preamble_1_data_xfer_list

#define postamble_9_1_conn postamble_1_data_xfer
#define postamble_9_1_resp postamble_1_idle
#define postamble_9_1_list postamble_1_data_xfer

struct test_stream test_9_1_conn = { &preamble_9_1_conn, &test_case_9_1_conn, &postamble_9_1_conn };
struct test_stream test_9_1_resp = { &preamble_9_1_resp, &test_case_9_1_resp, &postamble_9_1_resp };
struct test_stream test_9_1_list = { &preamble_9_1_list, &test_case_9_1_list, &postamble_9_1_list };

/*
 *  Delivering out-of-order data under noise.
 */
#define tgrp_case_9_2 tgrp_case_9
#define sgrp_case_9_2 sgrp_case_9
#define numb_case_9_2 "9.2"
#define name_case_9_2 "Delivering out-of-order data under noise."
#define sref_case_9_2 "NPI Rev 2.0.0"
#define desc_case_9_2 "\
Delivery of un-ordered data under noise."
#if 1
#define TEST_PACKETS 300
#else
#define TEST_PACKETS 30
#endif
int
test_case_9_2_conn(int child)
{
	int i = 0, j = 0;

	test_msleep(child, NORMAL_WAIT);
	state++;
	while (i < TEST_PACKETS) {
		QOS_buffer = &qos_data;
		QOS_length = sizeof(qos_data);
		DATA_buffer = "Pattern-1";
		DATA_length = strlen(DATA_buffer);
		if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS) {
			state++;
			test_sleep(child, 1);
			state++;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-1";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
				goto failure;
		}
		i++;
	}
	while (j < TEST_PACKETS) {
		if (expect(child, LONGER_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
		j++;
	}
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d\n", child, i, j);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

int
test_case_9_2_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_9_2_list(int child)
{
	int i = 0, j = 0;

	test_msleep(child, NORMAL_WAIT);
	state++;
	while (i < TEST_PACKETS) {
		QOS_buffer = &qos_data;
		QOS_length = sizeof(qos_data);
		DATA_buffer = "Pattern-3";
		DATA_length = strlen(DATA_buffer);
		if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS) {
			state++;
			test_sleep(child, 1);
			state++;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-3";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
				goto failure;
		}
		i++;
	}
	while (j < TEST_PACKETS) {
		if (expect(child, LONGER_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
		j++;
	}
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d\n", child, i, j);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#undef TEST_PACKETS

#define preamble_9_2_conn preamble_1_data_xfer_conn
#define preamble_9_2_resp preamble_1_idle_cons
#define preamble_9_2_list preamble_1_data_xfer_list

#define postamble_9_2_conn postamble_1_data_xfer
#define postamble_9_2_resp postamble_1_idle
#define postamble_9_2_list postamble_1_data_xfer

struct test_stream test_9_2_conn = { &preamble_9_2_conn, &test_case_9_2_conn, &postamble_9_2_conn };
struct test_stream test_9_2_resp = { &preamble_9_2_resp, &test_case_9_2_resp, &postamble_9_2_resp };
struct test_stream test_9_2_list = { &preamble_9_2_list, &test_case_9_2_list, &postamble_9_2_list };

/*
 *  Delivering ordered data in multiple streams under noise.
 */
#define tgrp_case_9_3 tgrp_case_9
#define sgrp_case_9_3 sgrp_case_9
#define numb_case_9_3 "9.3"
#define name_case_9_3 "Delivering ordered data in multiple streams under noise."
#define sref_case_9_3 "NPI Rev 2.0.0"
#define desc_case_9_3 "\
Delivery of ordered data in multiple streams under noise."
#define TEST_PACKETS 32
#define TEST_STREAMS 32
#define TEST_TOTAL (TEST_PACKETS*TEST_STREAMS)
int
test_case_9_3_conn(int child)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int k[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, K = 0;

	test_msleep(child, NORMAL_WAIT);
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			qos_data.sid = s;
			DATA_xfer_flags = N_RC_FLAG;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-1";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				state++;
				test_sleep(child, 1);
				if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
					goto failure;
			}
			i[s]++;
			I++;
		}
	}
	while (J < TEST_TOTAL || K < TEST_TOTAL) {
		/* wait must be longer than sack timeout */
		switch (wait_event(child, LONGER_WAIT)) {
		case __TEST_DATA_IND:
			j[sid[child]]++;
			J++;
			break;
		case __TEST_DATACK_IND:
			k[sid[child]]++;
			K++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	for (s = 0; s < TEST_STREAMS; s++) {
		if (j[s] != TEST_PACKETS || k[s] != TEST_PACKETS)
			goto failure;
	}
	test_sleep(child, 2);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d acks %d\n", child, i[s], j[s], k[s]);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

int
test_case_9_3_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_9_3_list(int child)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int k[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, K = 0;

	test_msleep(child, NORMAL_WAIT);
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			qos_data.sid = s;
			DATA_xfer_flags = N_RC_FLAG;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-3";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				state++;
				test_sleep(child, 1);
				if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
					goto failure;
			}
			i[s]++;
			I++;
		}
	}
	while (J < TEST_TOTAL || K < TEST_TOTAL) {
		/* wait must be longer than sack timeout */
		switch (wait_event(child, LONGER_WAIT)) {
		case __TEST_DATA_IND:
			j[sid[child]]++;
			J++;
			break;
		case __TEST_DATACK_IND:
			k[sid[child]]++;
			K++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	for (s = 0; s < TEST_STREAMS; s++) {
		if (j[s] != TEST_PACKETS || k[s] != TEST_PACKETS)
			goto failure;
	}
	test_sleep(child, 2);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d acks %d\n", child, i[s], j[s], k[s]);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#undef TEST_PACKETS
#undef TEST_STREAMS
#undef TEST_TOTAL

#define preamble_9_3_conn preamble_2_data_xfer_conn
#define preamble_9_3_resp preamble_1_idle_cons
#define preamble_9_3_list preamble_2_data_xfer_list

#define postamble_9_3_conn postamble_1_data_xfer
#define postamble_9_3_resp postamble_1_idle
#define postamble_9_3_list postamble_1_data_xfer

struct test_stream test_9_3_conn = { &preamble_9_3_conn, &test_case_9_3_conn, &postamble_9_3_conn };
struct test_stream test_9_3_resp = { &preamble_9_3_resp, &test_case_9_3_resp, &postamble_9_3_resp };
struct test_stream test_9_3_list = { &preamble_9_3_list, &test_case_9_3_list, &postamble_9_3_list };

/*
 *  Delivering ordered and unordered data in multiple streams under noise.
 */
#define tgrp_case_9_4 tgrp_case_9
#define sgrp_case_9_4 sgrp_case_9
#define numb_case_9_4 "9.4"
#define name_case_9_4 "Delivering ordered and unordered data in multiple streams under noise."
#define sref_case_9_4 "NPI Rev 2.0.0"
#define desc_case_9_4 "\
Delivery of ordered and un-ordered data in multiple streams under noise."
#define TEST_PACKETS 32
#define TEST_STREAMS 32
#define TEST_TOTAL (TEST_PACKETS*TEST_STREAMS)
int
test_case_9_4_conn(int child)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int k[TEST_STREAMS] = { 0, };
	int o[TEST_STREAMS] = { 0, };
	int p[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, K = 0, O = 0, P = 0;

	test_msleep(child, NORMAL_WAIT);
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			qos_data.sid = s;
			DATA_xfer_flags = N_RC_FLAG;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-1";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				state++;
				test_sleep(child, 1);
				state++;
				if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
					goto failure;
			}
			i[s]++;
			I++;
			state++;
		}
		while (O < TEST_TOTAL && o[s] < TEST_PACKETS) {
			qos_data.sid = s;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-1";
			DATA_length = strlen(DATA_buffer);
			do_signal(child, __TEST_EXDATA_REQ);
			o[s]++;
			O++;
			state++;
		}
	}
	while (J < TEST_TOTAL || K < TEST_TOTAL || P < TEST_TOTAL) {
		/* wait must be longer than sack timeout */
		switch (wait_event(child, LONGER_WAIT)) {
		case __TEST_DATA_IND:
			j[sid[child]]++;
			J++;
			break;
		case __TEST_DATACK_IND:
			k[sid[child]]++;
			K++;
			break;
		case __TEST_EXDATA_IND:
			p[sid[child]]++;
			P++;
			break;
		default:
			goto failure;
		}
		state++;
	}
	test_sleep(child, 3);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d acks %d\n", child, i[s], j[s], k[s]);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", child, o[s], p[s]);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

int
test_case_9_4_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_9_4_list(int child)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int k[TEST_STREAMS] = { 0, };
	int o[TEST_STREAMS] = { 0, };
	int p[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, K = 0, O = 0, P = 0;

	test_msleep(child, NORMAL_WAIT);
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			qos_data.sid = s;
			DATA_xfer_flags = N_RC_FLAG;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-3";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				state++;
				test_sleep(child, 1);
				state++;
				if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
					goto failure;
			}
			i[s]++;
			I++;
			state++;
		}
		while (O < TEST_TOTAL && o[s] < TEST_PACKETS) {
			qos_data.sid = s;
			QOS_buffer = &qos_data;
			QOS_length = sizeof(qos_data);
			DATA_buffer = "Pattern-3";
			DATA_length = strlen(DATA_buffer);
			do_signal(child, __TEST_EXDATA_REQ);
			o[s]++;
			O++;
			state++;
		}
	}
	while (J < TEST_TOTAL || K < TEST_TOTAL || P < TEST_TOTAL) {
		/* wait must be longer than sack timeout */
		switch (wait_event(child, LONGER_WAIT)) {
		case __TEST_DATA_IND:
			j[sid[child]]++;
			J++;
			break;
		case __TEST_DATACK_IND:
			k[sid[child]]++;
			K++;
			break;
		case __TEST_EXDATA_IND:
			p[sid[child]]++;
			P++;
			break;
		default:
			goto failure;
		}
		state++;
	}
	for (s = 0; s < TEST_STREAMS; s++) {
		if (j[s] != TEST_PACKETS || k[s] != TEST_PACKETS || p[s] != TEST_PACKETS)
			goto failure;
	}
	test_sleep(child, 3);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d acks %d\n", child, i[s], j[s], k[s]);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", child, o[s], p[s]);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}
#undef TEST_PACKETS
#undef TEST_STREAMS
#undef TEST_TOTAL

#define preamble_9_4_conn preamble_2_data_xfer_conn
#define preamble_9_4_resp preamble_1_idle_cons
#define preamble_9_4_list preamble_2_data_xfer_list

#define postamble_9_4_conn postamble_1_data_xfer
#define postamble_9_4_resp postamble_1_idle
#define postamble_9_4_list postamble_1_data_xfer

struct test_stream test_9_4_conn = { &preamble_9_4_conn, &test_case_9_4_conn, &postamble_9_4_conn };
struct test_stream test_9_4_resp = { &preamble_9_4_resp, &test_case_9_4_resp, &postamble_9_4_resp };
struct test_stream test_9_4_list = { &preamble_9_4_list, &test_case_9_4_list, &postamble_9_4_list };

#define tgrp_case_10 "Data transfer with destination failure."
#define sgrp_case_10 ""
/*
 *  Data for destination failure testing.
 */
#define tgrp_case_10_1 tgrp_case_10
#define sgrp_case_10_1 sgrp_case_10
#define numb_case_10_1 "10.1"
#define name_case_10_1 "Data for destination failure testing."
#define sref_case_10_1 "NPI Rev 2.0.0"
#define desc_case_10_1 "\
Delivery of ordered data with destination failure."
int
test_case_10_1_conn(int child)
{
	int i, j;

	test_msleep(child, NORMAL_WAIT);
	state++;
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			DATA_xfer_flags = 0;
			QOS_buffer = NULL;
			QOS_length = 0;
			DATA_buffer = "Test Pattern-1";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				state++;
				test_sleep(child, 1);
				state++;
				if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
					goto failure;
			}
			state++;
		}
		for (i = 0; i < 20; i++, state++) {
			if (expect(child, LONGER_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
				goto failure;
		}
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
test_case_10_1_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_10_1_list(int child)
{
	int i, j;

	test_msleep(child, NORMAL_WAIT);
	state++;
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20; i++) {
			DATA_xfer_flags = 0;
			QOS_buffer = NULL;
			QOS_length = 0;
			DATA_buffer = "Test Pattern-3";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				state++;
				test_sleep(child, 1);
				state++;
				if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
					goto failure;
			}
			state++;
		}
		for (i = 0; i < 20; i++, state++) {
			if (expect(child, LONGER_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
				goto failure;
		}
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_10_1_conn preamble_1_data_xfer_conn
#define preamble_10_1_resp preamble_1_idle_cons
#define preamble_10_1_list preamble_1_data_xfer_list

#define postamble_10_1_conn postamble_1_data_xfer
#define postamble_10_1_resp postamble_1_idle
#define postamble_10_1_list postamble_1_data_xfer

struct test_stream test_10_1_conn = { &preamble_10_1_conn, &test_case_10_1_conn, &postamble_10_1_conn };
struct test_stream test_10_1_resp = { &preamble_10_1_resp, &test_case_10_1_resp, &postamble_10_1_resp };
struct test_stream test_10_1_list = { &preamble_10_1_list, &test_case_10_1_list, &postamble_10_1_list };

/*
 *  Data for destination failure testing.
 */
long
time_sub(struct timeval *t1, struct timeval *t2)
{
	return ((t1->tv_sec - t2->tv_sec) * 1000000 + (t1->tv_usec - t2->tv_usec));
}

#define tgrp_case_10_2 tgrp_case_10
#define sgrp_case_10_2 sgrp_case_10
#define numb_case_10_2 "10.2"
#define name_case_10_2 "Data for destination failure testing."
#define sref_case_10_2 "NPI Rev 2.0.0"
#define desc_case_10_2 "\
Delivery of ordered data with destination failure."
int
test_case_10_2_conn(int child)
{
	int i, j, n = 0;
	struct result {
		uint req_idx;
		struct timeval req;
		uint ack_idx;
		struct timeval ack;
	};

#define SETS 1000
#define REPS 1
	struct result times[SETS * REPS];
	ulong atotal = 0;

	test_msleep(child, NORMAL_WAIT);
	state++;
	bzero(times, sizeof(times));
	show = 0;
	for (j = 0; j < SETS; j++) {
		for (i = 0; i < REPS; i++, state++) {
			DATA_xfer_flags = N_RC_FLAG;
			QOS_buffer = NULL;
			QOS_length = 0;
			DATA_buffer = "Test Pattern-1";
			DATA_length = strlen(DATA_buffer);
			if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS) {
				state++;
				test_sleep(child, 1);
				state++;
				if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
					goto failure;
			}
			times[j * REPS + i].req = when;
			times[j * REPS + i].req_idx = n++;
		}
		for (i = 0; i < REPS; i++, state++) {
			/* wait must be longer than sack timeout */
			if (expect(child, LONGER_WAIT, __TEST_DATACK_IND) != __RESULT_SUCCESS)
				goto failure;
			times[j * REPS + i].ack = when;
			times[j * REPS + i].ack_idx = n++;
		}
	}
	show = 1;
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	for (j = 0, n = 0; n < 3 * SETS * REPS; n++) {
		for (i = 0; i < SETS * REPS; i++) {
			if (times[i].req_idx == n) {
				fprintf(stdout, "N_DATA_REQ    ----->| - - - - - - ->\\               |  |                    \n");
				break;
			}
			if (times[i].ack_idx == n) {
				fprintf(stdout, "                    |               \\ - - - - - - - +->|---> N_DATA_IND     \n");
				fprintf(stdout, "N_DATACK_IND  <-----|<%9ld usec/               |  |                    \n", time_sub(&times[i].ack, &times[i].req));
				j++;
				atotal += time_sub(&times[i].ack, &times[i].req);
				break;
			}
		}
	}
	atotal /= j;
	fprintf(stdout, "                    |               |               |  |                    \n");
	fprintf(stdout, "                    |  ack average = %9ld usec |  |                    \n", atotal);
	fprintf(stdout, "                    |               |               |  |                    \n");
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
	show = 1;
	return (__RESULT_SUCCESS);
      failure:
	show = 1;
	return (__RESULT_FAILURE);
}

int
test_case_10_2_resp(int child)
{
	test_msleep(child, LONGER_WAIT);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_case_10_2_list(int child)
{
	int i, j;

	test_msleep(child, NORMAL_WAIT);
	state++;
	show = 0;
	for (j = 0; j < SETS; j++)
		for (i = 0; i < REPS; i++, state++)
			if (expect(child, LONGER_WAIT, __TEST_DATA_IND) != __RESULT_SUCCESS)
				goto failure;
	show = 1;
	return (__RESULT_SUCCESS);
      failure:
	show = 1;
	return (__RESULT_FAILURE);
}

#define preamble_10_2_conn preamble_1_data_xfer_conn
#define preamble_10_2_resp preamble_1_idle_cons
#define preamble_10_2_list preamble_1_data_xfer_list

#define postamble_10_2_conn postamble_1_data_xfer
#define postamble_10_2_resp postamble_1_idle
#define postamble_10_2_list postamble_1_data_xfer

struct test_stream test_10_2_conn = { &preamble_10_2_conn, &test_case_10_2_conn, &postamble_10_2_conn };
struct test_stream test_10_2_resp = { &preamble_10_2_resp, &test_case_10_2_resp, &postamble_10_2_resp };
struct test_stream test_10_2_list = { &preamble_10_2_list, &test_case_10_2_list, &postamble_10_2_list };

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
 *  The conn child (child[0]) is the connecting process, the resp child
 *  (child[1]) is the responding process.
 */

int
test_run(struct test_stream *stream[], ulong duration)
{
	int children = 0;
	pid_t this_child, child[3] = { 0, };
	int this_status, status[3] = { 0, };

	if (start_tt(duration) != __RESULT_SUCCESS)
		goto inconclusive;
	if (server_exec && stream[2]) {
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
	if (server_exec && stream[1]) {
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
	if (client_exec && stream[0]) {
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
	ulong duration;			/* maximum duration */
	int run;			/* whether to run this test */
	int result;			/* results of test */
	int expect;			/* expected result */
} tests[] = {
	{
		numb_case_0_1, tgrp_case_0_1, sgrp_case_0_1, name_case_0_1, NULL, desc_case_0_1, sref_case_0_1, {
	&test_0_1_conn, &test_0_1_resp, &test_0_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_1_1, tgrp_case_1_1, sgrp_case_1_1, name_case_1_1, NULL, desc_case_1_1, sref_case_1_1, {
	&test_1_1_conn, &test_1_1_resp, &test_1_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_1_2, tgrp_case_1_2, sgrp_case_1_2, name_case_1_2, NULL, desc_case_1_2, sref_case_1_2, {
	&test_1_2_conn, &test_1_2_resp, &test_1_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_1, tgrp_case_2_1, sgrp_case_2_1, name_case_2_1, NULL, desc_case_2_1, sref_case_2_1, {
	&test_2_1_conn, &test_2_1_resp, &test_2_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_2_2, tgrp_case_2_2, sgrp_case_2_2, name_case_2_2, NULL, desc_case_2_2, sref_case_2_2, {
	&test_2_2_conn, &test_2_2_resp, &test_2_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_1, tgrp_case_3_1, sgrp_case_3_1, name_case_3_1, NULL, desc_case_3_1, sref_case_3_1, {
	&test_3_1_conn, &test_3_1_resp, &test_3_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_3_2, tgrp_case_3_2, sgrp_case_3_2, name_case_3_2, NULL, desc_case_3_2, sref_case_3_2, {
	&test_3_2_conn, &test_3_2_resp, &test_3_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_4_1, tgrp_case_4_1, sgrp_case_4_1, name_case_4_1, NULL, desc_case_4_1, sref_case_4_1, {
	&test_4_1_conn, &test_4_1_resp, &test_4_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_4_2, tgrp_case_4_2, sgrp_case_4_2, name_case_4_2, NULL, desc_case_4_2, sref_case_4_2, {
	&test_4_2_conn, &test_4_2_resp, &test_4_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_5_1, tgrp_case_5_1, sgrp_case_5_1, name_case_5_1, NULL, desc_case_5_1, sref_case_5_1, {
	&test_5_1_conn, &test_5_1_resp, &test_5_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_5_2, tgrp_case_5_2, sgrp_case_5_2, name_case_5_2, NULL, desc_case_5_2, sref_case_5_2, {
	&test_5_2_conn, &test_5_2_resp, &test_5_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_5_3, tgrp_case_5_3, sgrp_case_5_3, name_case_5_3, NULL, desc_case_5_3, sref_case_5_3, {
	&test_5_3_conn, &test_5_3_resp, &test_5_3_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_6_1, tgrp_case_6_1, sgrp_case_6_1, name_case_6_1, NULL, desc_case_6_1, sref_case_6_1, {
	&test_6_1_conn, &test_6_1_resp, &test_6_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_6_2, tgrp_case_6_2, sgrp_case_6_2, name_case_6_2, NULL, desc_case_6_2, sref_case_6_2, {
	&test_6_2_conn, &test_6_2_resp, &test_6_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_6_3, tgrp_case_6_3, sgrp_case_6_3, name_case_6_3, NULL, desc_case_6_3, sref_case_6_3, {
	&test_6_3_conn, &test_6_3_resp, &test_6_3_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_7_1, tgrp_case_7_1, sgrp_case_7_1, name_case_7_1, NULL, desc_case_7_1, sref_case_7_1, {
	&test_7_1_conn, &test_7_1_resp, &test_7_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_7_2, tgrp_case_7_2, sgrp_case_7_2, name_case_7_2, NULL, desc_case_7_2, sref_case_7_2, {
	&test_7_2_conn, &test_7_2_resp, &test_7_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_8_1, tgrp_case_8_1, sgrp_case_8_1, name_case_8_1, NULL, desc_case_8_1, sref_case_8_1, {
	&test_8_1_conn, &test_8_1_resp, &test_8_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_8_2, tgrp_case_8_2, sgrp_case_8_2, name_case_8_2, NULL, desc_case_8_2, sref_case_8_2, {
	&test_8_2_conn, &test_8_2_resp, &test_8_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_9_1, tgrp_case_9_1, sgrp_case_9_1, name_case_9_1, NULL, desc_case_9_1, sref_case_9_1, {
	&test_9_1_conn, &test_9_1_resp, &test_9_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_9_2, tgrp_case_9_2, sgrp_case_9_2, name_case_9_2, NULL, desc_case_9_2, sref_case_9_2, {
	&test_9_2_conn, &test_9_2_resp, &test_9_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_9_3, tgrp_case_9_3, sgrp_case_9_3, name_case_9_3, NULL, desc_case_9_3, sref_case_9_3, {
	&test_9_3_conn, &test_9_3_resp, &test_9_3_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_9_4, tgrp_case_9_4, sgrp_case_9_4, name_case_9_4, NULL, desc_case_9_4, sref_case_9_4, {
	&test_9_4_conn, &test_9_4_resp, &test_9_4_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_10_1, tgrp_case_10_1, sgrp_case_10_1, name_case_10_1, NULL, desc_case_10_1, sref_case_10_1, {
	&test_10_1_conn, &test_10_1_resp, &test_10_1_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
		numb_case_10_2, tgrp_case_10_2, sgrp_case_10_2, name_case_10_2, NULL, desc_case_10_2, sref_case_10_2, {
	&test_10_2_conn, &test_10_2_resp, &test_10_2_list}, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS}, {
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
				ulong duration = test_duration;

				if (duration > tests[i].duration) {
					if (tests[i].duration && duration > tests[i].duration)
						duration = tests[i].duration;
					if ((result = (*tests[i].start) (i)) != __RESULT_SUCCESS)
						goto inconclusive;
					result = test_run(tests[i].stream, duration);
					(*tests[i].stop) (i);
				} else
					result = __RESULT_SKIPPED;
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
    -c, --client\n\
        execute client side (PTU) of test case only.\n\
    -S, --server\n\
        execute server side (IUT) of test case only.\n\
    -a, --again\n\
        repeat failed tests verbose.\n\
    -w, --wait\n\
        have server wait indefinitely.\n\
    -r, --repeat\n\
        repeat test cases on success or failure.\n\
    -R, --repeat-fail\n\
        repeat test cases on failure.\n\
    -p, --client-port [PORT]\n\
        port number from which to connect [default: %3$d+index*3]\n\
    -P, --server-port [PORT]\n\
        port number to which to connect or upon which to listen\n\
        [default: %3$d+index*3+2]\n\
    -i, --client-host [HOSTNAME[,HOSTNAME]*]\n\
        client host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
    -I, --server-host [HOSTNAME[,HOSTNAME]*]\n\
        server host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
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
", argv[0], devname, TEST_PORT_NUMBER);
}

#define HOST_BUF_LEN 128

int
main(int argc, char *argv[])
{
	size_t l, n;
	int range = 0;
	struct test_case *t;
	int tests_to_run = 0;
	char *hostc = "127.0.0.1,127.0.0.2,127.0.0.3";
	char *hosts = "127.0.0.1,127.0.0.2,127.0.0.3";
	char hostbufc[HOST_BUF_LEN];
	char hostbufs[HOST_BUF_LEN];
	struct hostent *haddr;

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
			{"client",	no_argument,		NULL, 'c'},
			{"server",	no_argument,		NULL, 'S'},
			{"again",	no_argument,		NULL, 'a'},
			{"wait",	no_argument,		NULL, 'w'},
			{"client-port",	required_argument,	NULL, 'p'},
			{"server-port",	required_argument,	NULL, 'P'},
			{"client-host",	required_argument,	NULL, 'i'},
			{"server-host",	required_argument,	NULL, 'I'},
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

		c = getopt_long(argc, argv, "cSawp:P:i:I:rRd:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "cSawp:P:i:I:rRd:el::f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'c':	/* --client */
			client_exec = 1;
			break;
		case 'S':	/* --server */
			server_exec = 1;
			break;
		case 'a':	/* --again */
			repeat_verbose = 1;
			break;
		case 'w':	/* --wait */
			test_duration = INFINITE_WAIT;
			break;
		case 'p':	/* --client-port */
			client_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[0] = ports[3];
			break;
		case 'P':	/* --server-port */
			server_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[1] = ports[3];
			ports[2] = ports[3] + 1;
			break;
		case 'i':	/* --client-host *//* client host */
			client_host_specified = 1;
			strncpy(hostbufc, optarg, HOST_BUF_LEN);
			hostc = hostbufc;
			break;
		case 'I':	/* --server-host *//* server host */
			server_host_specified = 1;
			strncpy(hostbufs, optarg, HOST_BUF_LEN);
			hosts = hostbufs;
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
	if (client_exec == 0 && server_exec == 0) {
		client_exec = 1;
		server_exec = 1;
	}
	if (client_host_specified) {
		int count = 0;
		char *token = hostc, *next_token, *delim = NULL;

		fprintf(stdout, "Specified client address => %s\n", token);
		do {
			if ((delim = index(token, ','))) {
				delim[0] = '\0';
				next_token = delim + 1;
			} else
				next_token = NULL;
			haddr = gethostbyname(token);
			addrs[0][count].sin_family = AF_INET;
			addrs[3][count].sin_family = AF_INET;
			if (client_port_specified) {
				addrs[0][count].sin_port = htons(ports[0]);
				addrs[3][count].sin_port = htons(ports[3]);
			} else {
				addrs[0][count].sin_port = 0;
				addrs[3][count].sin_port = 0;
			}
			addrs[0][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			addrs[3][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			count++;
		} while ((token = next_token) && count < 4);
		anums[0] = count;
		anums[3] = count;
		fprintf(stdout, "%d client addresses assigned\n", count);
	}
	if (server_host_specified) {
		int count = 0;
		char *token = hosts, *next_token, *delim = NULL;

		fprintf(stdout, "Specified server address => %s\n", token);
		do {
			if ((delim = index(token, ','))) {
				delim[0] = '\0';
				next_token = delim + 1;
			} else
				next_token = NULL;
			haddr = gethostbyname(token);
			addrs[1][count].sin_family = AF_INET;
			addrs[2][count].sin_family = AF_INET;
			addrs[3][count].sin_family = AF_INET;
			if (server_port_specified) {
				addrs[1][count].sin_port = htons(ports[1]);
				addrs[2][count].sin_port = htons(ports[2]);
				addrs[3][count].sin_port = htons(ports[3]);
			} else {
				addrs[1][count].sin_port = 0;
				addrs[2][count].sin_port = 0;
				addrs[3][count].sin_port = 0;
			}
			addrs[1][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			addrs[2][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			addrs[3][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			count++;
		} while ((token = next_token) && count < 4);
		anums[1] = count;
		anums[2] = count;
		anums[3] = count;
		fprintf(stdout, "%d server addresses assigned\n", count);
	}
	exit(do_tests(tests_to_run));
}
