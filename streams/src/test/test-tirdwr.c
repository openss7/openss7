/*****************************************************************************

 @(#) $RCSfile: test-tirdwr.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/04 23:15:47 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004 OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000 Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written
 authorization of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that
 the recipient will protect this information and keep it confidential, and
 will not disclose the information contained in this document without the
 written permission of its owner.

 The author reserves the right to revise this software and documentation
 for any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of
 the technical arts, or the reflection of changes in the design of any
 techniques, or procedures embodied, described, or referred to herein.
 The author is under no obligation to provide any feature listed herein.

 As an exception to the above, this software may be distributed under the
 GNU General Public License (GPL) Version 2 or later, so long as the
 software is distributed with, and only used for the testing of, OpenSS7
 modules, drivers, and libraries.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"),
 it is classified as "Commercial Computer Software" under paragraph
 252.227-7014 of the DoD Supplement to the Federal Acquisition Regulations
 ("DFARS") (or any successor regulations) and the Government is acquiring
 only the license rights granted herein (the license rights customarily
 provided to non-Government users).  If the Software is supplied to any unit
 or agency of the Government other than DoD, it is classified as "Restricted
 Computer Software" and the Government's rights in the Software are defined
 in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR") (or
 any success regulations) or, in the cases of NASA, in paragraph 18.52.227-86
 of the NASA Supplement to the FAR (or any successor regulations).

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/03/04 23:15:47 $ by <bidulock@openss7.org>

 *****************************************************************************/

#ident "@(#) $RCSfile: test-tirdwr.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/04 23:15:47 $"

static char const ident[] = "$RCSfile: test-tirdwr.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/03/04 23:15:47 $";

/*
 *  These is a ferry-clip TIRDWR conformance test program for testing the
 *  OpenSS7 tirdwr STREAMS module.
 *
 *  GENERAL OPERATION:
 *
 *  The test program opens STREAMS pipe and pushes the "tirdwr" module onto one
 *  side of the pipe.
 *
 *  The test program exchanges TPI primitives with one side of the open pipe
 *  and exchagnes TPI primitives and executes TIRDWR ioctls on the other side
 *  of the pipe.  The side of the pipe with the "tirdwr" STREAMS module pushed
 *  represents the XTI library user of the TIRDWR module.  The other side of
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
 *    |            TIRDWR            |                 |    |
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
 *  This test arrangement results in a a ferry-clip around the TIRDWR module,
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

static int verbose = 1;

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;

static int last_event = 0;
static int last_errno = 0;

static int top_fd = 0;
static int bot_fd = 0;

#define BUFSIZE 4096

/*
 *  -------------------------------------------------------------------------
 *
 *  Events and Actions
 *
 *  -------------------------------------------------------------------------
 */
enum {
	NO_MSG = -6, TIMEOUT = -5, UNKNOWN = -4, DECODEERROR = -3, SCRIPTERROR = -2,
	INCONCLUSIVE = -1, SUCCESS = 0, FAILURE = 1,
};
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

#undef HZ
#define HZ 1000

/* *INDENT-OFF* */
static timer_range_t timer[tmax] = {
	{(15 * HZ),		(60 * HZ)},		/* T1 15-60 seconds */
	{(3 * 60 * HZ),		(3 * 60 * HZ)},		/* T2 3 minutes */
	{(2 * 60 * HZ),		(2 * 60 * HZ)},		/* T3 2 minutes */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T4 5-15 minutes */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T5 5-15 minutes */
	{(10 * HZ),		(32 * HZ)},		/* T6 10-32 seconds (specified in Q.118) */
	{(20 * HZ),		(30 * HZ)},		/* T7 20-30 seconds */
	{(10 * HZ),		(15 * HZ)},		/* T8 10-15 seconds */
	{(2 * 60 * HZ),		(4 * 60 * HZ)},		/* T9 2-4 minutes (specified in Q.118) */
	{(4 * HZ),		(6 * HZ)},		/* T10 4-6 seconds */
	{(15 * HZ),		(20 * HZ)},		/* T11 15-20 seconds */
	{(15 * HZ),		(60 * HZ)},		/* T12 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T13 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T14 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T15 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T16 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T17 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T18 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T19 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T20 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T21 5-15 minutes */
	{(15 * HZ),		(60 * HZ)},		/* T22 15-60 seconds */
	{(5 * 60 * HZ),		(15 * 60 * HZ)},	/* T23 5-15 minutes */
	{(1 * HZ),		(2 * HZ)},		/* T24 < 2 seconds */
	{(1 * HZ),		(10 * HZ)},		/* T25 1-10 seconds */
	{(1 * 60 * HZ),		(3 * 60 * HZ)},		/* T26 1-3 minutes */
	{(4 * 60 * HZ),		(4 * 60 * HZ)},		/* T27 4 minutes */
	{(10 * HZ),		(10 * HZ)},		/* T28 10 seconds */
	{(300 * HZ / 1000),	(600 * HZ / 1000)},	/* T29 300-600 milliseconds */
	{(5 * HZ),		(10 * HZ)},		/* T30 5-10 seconds */
	{(6 * 60 * HZ),		(7 * 60 * HZ)},		/* T31 > 6 minutes */
	{(3 * HZ),		(5 * HZ)},		/* T32 3-5 seconds */
	{(12 * HZ),		(15 * HZ)},		/* T33 12-15 seconds */
	{(12 * HZ),		(15 * HZ)},		/* T34 12-15 seconds */
	{(15 * HZ),		(20 * HZ)},		/* T35 15-20 seconds */
	{(15 * HZ),		(20 * HZ)},		/* T36 15-20 seconds */
	{(2 * HZ),		(4 * HZ)},		/* T37 2-4 seconds */
	{(15 * HZ),		(20 * HZ)}		/* T38 15-20 seconds */
};
/* *INDENT-ON* */

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
		int error = errno;
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "***************ERROR: couldn't get time!\n");
		fprintf(stdout, "                      %s: %s\n", __FUNCTION__, strerror(error));
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
static long
milliseconds(char *t)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "                    .  .              :                .                    \n");
	fprintf(stdout, "                    .  .            %6s             .                    <%d>\n", t, state);
	fprintf(stdout, "                    .  .              :                .                    \n");
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return now();
}
static long
milliseconds_2nd(char *t)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "                    .  .              :   :            .                    \n");
	fprintf(stdout, "                    .  .              : %6s         .                    <%d>\n", t, state);
	fprintf(stdout, "                    .  .              :   :            .                    \n");
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
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
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "                    |  |(%7.3g <= %7.3g <= %7.3g)| %s             <%d>\n", dlo - tol, itv, dhi + tol, t, state);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	if (dlo - tol <= itv && itv <= dhi + tol)
		return SUCCESS;
	else
		return FAILURE;
}

static int
time_event(int event)
{
	if (verbose > 1) {
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

static RETSIGTYPE
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
	act.sa_handler = &timer_handler;
	act.sa_flags = SA_RESTART | SA_ONESHOT;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	return SUCCESS;
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
		return FAILURE;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return FAILURE;
	timer_timeout = 0;
	return SUCCESS;
}
static int
start_st(long duration)
{
	long sdur = (duration + timer_scale - 1) / timer_scale;
	return start_tt(sdur);
}

static int
stop_tt(void)
{
	struct itimerval setting = { {0, 0}, {0, 0} };
	sigset_t mask;
	struct sigaction act;
	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return FAILURE;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return FAILURE;
	timer_timeout = 0;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	return SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Injected event encoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

static int
do_signal(int fd, int action)
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
	if (fd == top_fd) {
		switch (action) {
		case __TEST_PUSH:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "---I_PUSH(tirdwr)-->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (ioctl(top_fd, I_PUSH, "tirdwr") < 0) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: ioctl I_PUSH failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return (FAILURE);
			}
			return (SUCCESS);
		case __TEST_POP:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "---I_POP(tirdwr)--->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (ioctl(top_fd, I_POP, NULL) < 0) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: ioctl I_POP failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return (FAILURE);
			}
			return (SUCCESS);
		case __TEST_WRITE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--write(2)--------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			data->len = sprintf(dbuf, "Write test data.");
			if (write(top_fd, dbuf, data->len) < 0) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: write failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return FAILURE;
			}
			return SUCCESS;
		case __TEST_WRITEV:
		{
			struct iovec vector[4];
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--writev(2)-------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			vector[0].iov_base = dbuf;
			vector[0].iov_len = sprintf(vector[0].iov_base, "Writev test datum for vector 0.");
			vector[1].iov_base = dbuf + vector[0].iov_len;
			vector[1].iov_len = sprintf(vector[1].iov_base, "Writev test datum for vector 1.");
			vector[2].iov_base = dbuf + vector[1].iov_len;
			vector[2].iov_len = sprintf(vector[2].iov_base, "Writev test datum for vector 2.");
			vector[3].iov_base = dbuf + vector[2].iov_len;
			vector[3].iov_len = sprintf(vector[3].iov_base, "Writev test datum for vector 3.");
			if (writev(top_fd, vector, 4) < 0) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: writev failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return FAILURE;
			}
			return SUCCESS;
		}
		case __TEST_PUTMSG_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--putmsg(2)-------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putmsg test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_PUTPMSG_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--putpmsg(2)------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putpmsg band test data.");
			flags = MSG_BAND;
			band = 1;
			break;
		case __TEST_CONN_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_CONN_REQ------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->conn_req);
			data = NULL;
			p->conn_req.PRIM_type = T_CONN_REQ;
			p->conn_req.DEST_length = 0;
			p->conn_req.DEST_offset = 0;
			p->conn_req.OPT_length = 0;
			p->conn_req.OPT_offset = 0;
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_CONN_RES:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_CONN_RES------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->conn_res);
			data = NULL;
			p->conn_res.PRIM_type = T_CONN_RES;
			p->conn_res.ACCEPTOR_id = 0;
			p->conn_res.OPT_length = 0;
			p->conn_res.OPT_offset = 0;
			p->conn_res.SEQ_number = 0;
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_DISCON_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_DISCON_REQ----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->discon_req);
			data = NULL;
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = 0;
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_DATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_DATA_REQ------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->data_req);
			p->data_req.PRIM_type = T_DATA_REQ;
			p->data_req.MORE_flag = 0;
			data->len = sprintf(dbuf, "Normal test message.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_EXDATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_EXDATA_REQ----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->exdata_req);
			p->exdata_req.PRIM_type = T_EXDATA_REQ;
			p->exdata_req.MORE_flag = 0;
			data->len = sprintf(dbuf, "Expedited test message.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_INFO_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_INFO_REQ------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->info_req);
			p->info_req.PRIM_type = T_INFO_REQ;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_BIND_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_BIND_REQ------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->bind_req);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			data = NULL;
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_UNBIND_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_UNBIND_REQ----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->unbind_req);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			data = NULL;
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_UNITDATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_UNITDATA_REQ--->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->unitdata_req);
			p->unitdata_req.PRIM_type = T_UNITDATA_REQ;
			p->unitdata_req.DEST_length = 0;
			p->unitdata_req.DEST_offset = 0;
			p->unitdata_req.OPT_length = 0;
			p->unitdata_req.OPT_offset = 0;
			data->len = sprintf(dbuf, "Unit test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_OPTMGMT_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_OPTMGMT_REQ---->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->optmgmt_req);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			data = NULL;
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_ORDREL_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_ORDREL_REQ----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->ordrel_req);
			p->ordrel_req.PRIM_type = T_ORDREL_REQ;
			data->len = sprintf(dbuf, "Orderly release data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_OPTDATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_OPTDATA_REQ---->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->optdata_req);
			p->optdata_req.PRIM_type = T_OPTDATA_REQ;
			p->optdata_req.DATA_flag = T_ODF_EX | T_ODF_MORE;
			p->optdata_req.OPT_length = 0;
			p->optdata_req.OPT_offset = 0;
			data->len = sprintf(dbuf, "Option data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_ADDR_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_ADDR_REQ------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->addr_req);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_CAPABILITY_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_CAPABILITY_REQ->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->capability_req);
			p->capability_req.PRIM_type = T_CAPABILITY_REQ;
			p->capability_req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_O_TI_GETINFO:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  O_TI_GETINFO                 |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = O_TI_GETINFO;
			ic.ic_len = sizeof(p->info_ack);
			p->info_req.PRIM_type = T_INFO_REQ;
			goto do_ioctl;
		case __TEST_O_TI_OPTMGMT:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  O_TI_OPTMGMT                 |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = O_TI_OPTMGMT;
			ic.ic_len = sizeof(p->optmgmt_ack);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			goto do_ioctl;
		case __TEST_O_TI_BIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  O_TI_BIND                    |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = O_TI_BIND;
			ic.ic_len = sizeof(p->bind_ack);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			goto do_ioctl;
		case __TEST_O_TI_UNBIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  O_TI_UNBIND                  |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = O_TI_UNBIND;
			ic.ic_len = sizeof(p->ok_ack);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			goto do_ioctl;
		case __TEST__O_TI_GETINFO:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_GETINFO                |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_GETINFO;
			ic.ic_len = sizeof(p->info_ack);
			p->info_req.PRIM_type = T_INFO_REQ;
			goto do_ioctl;
		case __TEST__O_TI_OPTMGMT:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_OPTMGMT                |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_OPTMGMT;
			ic.ic_len = sizeof(p->optmgmt_ack);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			goto do_ioctl;
		case __TEST__O_TI_BIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_BIND                   |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_BIND;
			ic.ic_len = sizeof(p->bind_ack);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			goto do_ioctl;
		case __TEST__O_TI_UNBIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_UNBIND                 |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_UNBIND;
			ic.ic_len = sizeof(p->ok_ack);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			goto do_ioctl;
		case __TEST__O_TI_GETMYNAME:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_GETMYNAME              |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_GETMYNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			goto do_ioctl;
		case __TEST__O_TI_GETPEERNAME:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_GETPEERNAME            |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_GETPEERNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			goto do_ioctl;
		case __TEST__O_TI_XTI_HELLO:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_XTI_HELLO              |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_XTI_HELLO;
			ic.ic_len = 0;
			goto do_ioctl;
		case __TEST__O_TI_XTI_GET_STATE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_XTI_GET_STATE          |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_XTI_GET_STATE;
			ic.ic_len = 0;
			goto do_ioctl;
		case __TEST__O_TI_XTI_CLEAR_EVENT:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_XTI_CLEAR_EVENT        |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_XTI_CLEAR_EVENT;
			ic.ic_len = 0;
			goto do_ioctl;
		case __TEST__O_TI_XTI_MODE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_XTI_MODE               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_XTI_MODE;
			ic.ic_len = 0;
			goto do_ioctl;
		case __TEST__O_TI_TLI_MODE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  _O_TI_TLI_MODE               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = _O_TI_TLI_MODE;
			ic.ic_len = 0;
			goto do_ioctl;
		case __TEST_TI_GETINFO:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_GETINFO                   |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_GETINFO;
			ic.ic_len = sizeof(p->info_ack);
			p->info_req.PRIM_type = T_INFO_REQ;
			goto do_ioctl;
		case __TEST_TI_OPTMGMT:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_OPTMGMT                   |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_OPTMGMT;
			ic.ic_len = sizeof(p->optmgmt_ack);
			p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
			p->optmgmt_req.OPT_length = 0;
			p->optmgmt_req.OPT_offset = 0;
			p->optmgmt_req.MGMT_flags = T_NEGOTIATE;
			goto do_ioctl;
		case __TEST_TI_BIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_BIND                      |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_BIND;
			ic.ic_len = sizeof(p->bind_ack);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = 0;
			goto do_ioctl;
		case __TEST_TI_UNBIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_UNBIND                    |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_UNBIND;
			ic.ic_len = sizeof(p->ok_ack);
			p->unbind_req.PRIM_type = T_UNBIND_REQ;
			goto do_ioctl;
		case __TEST_TI_GETMYNAME:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_GETMYNAME                 |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_GETMYNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			goto do_ioctl;
		case __TEST_TI_GETPEERNAME:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_GETPEERNAME               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_GETPEERNAME;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			goto do_ioctl;
		case __TEST_TI_SETMYNAME:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETMYNAME                 |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->conn_res);
			p->conn_res.PRIM_type = T_CONN_RES;
			p->conn_res.ACCEPTOR_id = 0;
			p->conn_res.OPT_length = 0;
			p->conn_res.OPT_offset = 0;
			p->conn_res.SEQ_number = 0;
			goto do_ioctl;
		case __TEST_TI_SETPEERNAME:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETPEERNAME               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->conn_req);
			p->conn_req.PRIM_type = T_CONN_REQ;
			p->conn_req.DEST_length = 0;
			p->conn_req.DEST_offset = 0;
			p->conn_req.OPT_length = 0;
			p->conn_req.OPT_offset = 0;
			goto do_ioctl;
		case __TEST_TI_SYNC:
		{
			union {
				struct ti_sync_req req;
				struct ti_sync_ack ack;
			} *s = (typeof(s)) p;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SYNC                      |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SYNC;
			ic.ic_len = sizeof(*s);
			s->req.tsr_flags = TSRF_INFO_REQ | TSRF_IS_EXP_IN_RCVBUF | TSRF_QLEN_REQ;
			goto do_ioctl;
		}
		case __TEST_TI_GETADDRS:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_GETADDRS                  |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_GETADDRS;
			ic.ic_len = sizeof(p->addr_ack);
			p->addr_req.PRIM_type = T_ADDR_REQ;
			goto do_ioctl;
		case __TEST_TI_CAPABILITY:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_CAPABILITY                |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_CAPABILITY;
			ic.ic_len = sizeof(p->capability_ack);
			p->capability_req.PRIM_type = T_CAPABILITY_REQ;
			p->capability_req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
			goto do_ioctl;
		default:
			return SCRIPTERROR;
		}
		if (putpmsg(top_fd, ctrl, data, band, flags) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "***************ERROR: putpmsg failed\n");
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
				lockf(fileno(stdout), F_ULOCK, 0);
				fflush(stdout);
			}
			return FAILURE;
		}
		return SUCCESS;
	      do_ioctl:
		if (ioctl(top_fd, I_STR, &ic) == -1) {
			last_errno = errno & 0x00ff;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "***************ERROR: ioctl failed\n");
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
				lockf(fileno(stdout), F_ULOCK, 0);
				fflush(stdout);
			}
			return FAILURE;
		}
		return SUCCESS;
	}
	if (fd == bot_fd) {
		switch (action) {
		case __TEST_WRITE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-write(2)----------(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			data->len = sprintf(dbuf, "Write test data.");
			if (write(bot_fd, dbuf, data->len) < 0) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "                                              ****ERROR: write failed\n");
					fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return FAILURE;
			}
			return SUCCESS;
		case __TEST_WRITEV:
		{
			struct iovec vector[4];
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-writev(2)---------(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			vector[0].iov_base = dbuf;
			vector[0].iov_len = sprintf(vector[0].iov_base, "Writev test datum for vector 0.");
			vector[1].iov_base = dbuf + vector[0].iov_len;
			vector[1].iov_len = sprintf(vector[1].iov_base, "Writev test datum for vector 1.");
			vector[2].iov_base = dbuf + vector[1].iov_len;
			vector[2].iov_len = sprintf(vector[2].iov_base, "Writev test datum for vector 2.");
			vector[3].iov_base = dbuf + vector[2].iov_len;
			vector[3].iov_len = sprintf(vector[3].iov_base, "Writev test datum for vector 3.");
			if (writev(bot_fd, vector, 4) < 0) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "                                              ****ERROR: writev failed\n");
					fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return FAILURE;
			}
			return SUCCESS;
		}
		case __TEST_PUTMSG_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-putmsg(2)---------(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putmsg test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_PUTPMSG_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-putpmsg(2)--------(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putpmsg band test data.");
			flags = MSG_BAND;
			band = 1;
			break;
		case __TEST_CONN_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_CONN_IND------- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
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
			break;
		case __TEST_CONN_CON:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_CONN_CON------- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->conn_con);
			p->conn_con.PRIM_type = T_CONN_CON;
			p->conn_con.RES_length = 0;
			p->conn_con.RES_offset = 0;
			p->conn_con.OPT_length = 0;
			p->conn_con.OPT_offset = 0;
			data->len = sprintf(dbuf, "Connection confirmation test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_DISCON_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_DISCON_IND----- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->discon_ind);
			p->discon_ind.PRIM_type = T_DISCON_IND;
			p->discon_ind.DISCON_reason = 0;
			p->discon_ind.SEQ_number = 0;
			data->len = sprintf(dbuf, "Disconnection indication test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_DATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_DATA_IND------- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->data_ind);
			p->data_ind.PRIM_type = T_DATA_IND;
			p->data_ind.MORE_flag = 0;
			data->len = sprintf(dbuf, "Normal test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_EXDATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_EXDATA_IND----- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->exdata_ind);
			p->data_ind.PRIM_type = T_EXDATA_IND;
			p->data_ind.MORE_flag = 0;
			data->len = sprintf(dbuf, "Expedited test data.");
			flags = MSG_BAND;
			band = 1;
			break;
		case __TEST_INFO_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_INFO_ACK------- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
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
			break;
		case __TEST_BIND_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_BIND_ACK------- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->bind_ack);
			p->bind_ack.PRIM_type = T_BIND_ACK;
			p->bind_ack.ADDR_length = 0;
			p->bind_ack.ADDR_offset = 0;
			p->bind_ack.CONIND_number = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_ERROR_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_ERROR_ACK------ (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->error_ack);
			p->error_ack.PRIM_type = T_ERROR_ACK;
			p->error_ack.ERROR_prim = 0;
			p->error_ack.TLI_error = 0;
			p->error_ack.UNIX_error = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_OK_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_OK_ACK--------- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->ok_ack);
			p->ok_ack.PRIM_type = T_OK_ACK;
			p->ok_ack.CORRECT_prim = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_UNITDATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_UNIDATA_IND---- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->unitdata_ind);
			p->unitdata_ind.PRIM_type = T_UNITDATA_IND;
			p->unitdata_ind.SRC_length = 0;
			p->unitdata_ind.SRC_offset = 0;
			p->unitdata_ind.OPT_length = 0;
			p->unitdata_ind.OPT_offset = 0;
			data->len = sprintf(dbuf, "Unit test data indication.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_UDERROR_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_UDERROR_IND---- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
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
			break;
		case __TEST_OPTMGMT_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_OPTMGMT_ACK---- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->optmgmt_ack);
			p->optmgmt_ack.PRIM_type = T_OPTMGMT_ACK;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_ORDREL_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_ORDREL_IND----- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->ordrel_ind);
			p->ordrel_ind.PRIM_type = T_ORDREL_IND;
			data->len = sprintf(dbuf, "Orderly release indication test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_EXP_OPTDATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_OPTDATA_IND---- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->optdata_ind);
			p->optdata_ind.PRIM_type = T_OPTDATA_IND;
			p->optdata_ind.DATA_flag = T_ODF_EX;
			p->optdata_ind.OPT_length = 0;
			p->optdata_ind.OPT_offset = 0;
			data->len = sprintf(dbuf, "Option data indication test data.");
			flags = MSG_BAND;
			band = 1;
			break;
		case __TEST_NRM_OPTDATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_OPTDATA_IND---- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->optdata_ind);
			p->optdata_ind.PRIM_type = T_OPTDATA_IND;
			p->optdata_ind.DATA_flag = 0;
			p->optdata_ind.OPT_length = 0;
			p->optdata_ind.OPT_offset = 0;
			data->len = sprintf(dbuf, "Option data indication test data.");
			flags = MSG_BAND;
			band = 0;
			break;
		case __TEST_ADDR_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_ADDR_ACK------- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->addr_ack);
			p->addr_ack.PRIM_type = T_ADDR_ACK;
			p->addr_ack.LOCADDR_length = 0;
			p->addr_ack.LOCADDR_offset = 0;
			p->addr_ack.REMADDR_length = 0;
			p->addr_ack.REMADDR_offset = 0;
			data = NULL;
			flags = MSG_HIPRI;
			band = 0;
			break;
		case __TEST_CAPABILITY_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-T_CAPABILITY_ACK- (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
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
			break;
		default:
			return SCRIPTERROR;
		}
		if (putpmsg(bot_fd, ctrl, data, band, flags) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                                              ****ERROR: putpmsg failed\n");
				fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return FAILURE;
		}
		return SUCCESS;
	}
	return SCRIPTERROR;
}

static int
top_signal(int action)
{
	return do_signal(top_fd, action);
}

static int
bot_signal(int action)
{
	return do_signal(bot_fd, action);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Received event decoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

static int
do_decode_data(int fd, struct strbuf *data)
{
	if (fd == top_fd) {
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-DATA--------------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (__TEST_DATA);
	}
	if (fd == bot_fd) {
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--DATA------------> (%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (__TEST_DATA);
	}
	return (DECODEERROR);
}
static int
do_decode_ctrl(int fd, struct strbuf *ctrl, struct strbuf *data)
{
	int ret;
	union T_primitives *p = (union T_primitives *) ctrl->buf;
	if (fd == top_fd) {
		switch (p->type) {
		case T_CONN_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_CONN_IND--------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_CONN_IND;
			break;
		case T_CONN_CON:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_CONN_CON--------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_CONN_CON;
			break;
		case T_DISCON_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_DISCON_IND------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_DISCON_IND;
			break;
		case T_DATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_DATA_IND--------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_DATA_IND;
			break;
		case T_EXDATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_EXDATA_IND------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_EXDATA_IND;
			break;
		case T_INFO_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_INFO_ACK--------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_INFO_ACK;
			break;
		case T_BIND_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_BIND_ACK--------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_BIND_ACK;
			break;
		case T_ERROR_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_ERROR_ACK-------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_ERROR_ACK;
			break;
		case T_OK_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_OK_ACK----------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_OK_ACK;
			break;
		case T_UNITDATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_UNITDATA_REQ----|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_UNITDATA_IND;
			break;
		case T_UDERROR_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_UDERROR_IND-----|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_UDERROR_IND;
			break;
		case T_OPTMGMT_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_OPTMGMT_ACK-----|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_OPTMGMT_ACK;
			break;
		case T_ORDREL_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_ORDREL_IND------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_ORDREL_IND;
			break;
		case T_OPTDATA_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_OPTDATA_IND-----|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (p->optdata_ind.DATA_flag & T_ODF_EX)
				ret = __TEST_EXP_OPTDATA_IND;
			else
				ret = __TEST_NRM_OPTDATA_IND;
			break;
		case T_ADDR_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_ADDR_ACK--------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_ADDR_ACK;
			break;
		case T_CAPABILITY_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_CPABILITY_ACK---|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_CAPABILITY_ACK;
			break;
		default:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_????_???--------|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = DECODEERROR;
			break;
		}
		if (data && data->len >= 0)
			if (do_decode_data(fd, data) != __TEST_DATA)
				return (FAILURE);
		return (ret);
	}
	if (fd == bot_fd) {
		switch (p->type) {
		case T_CONN_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_CONN_REQ------> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_CONN_REQ;
			break;
		case T_CONN_RES:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_CONN_RES------> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_CONN_RES;
			break;
		case T_DISCON_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_DISCON_REQ----> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_DISCON_REQ;
			break;
		case T_DATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_DATA_REQ------> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_DATA_REQ;
			break;
		case T_EXDATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_EXDATA_REQ----> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_EXDATA_REQ;
			break;
		case T_INFO_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_INFO_REQ------> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_INFO_REQ;
			break;
		case T_BIND_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_BIND_REQ------> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_BIND_REQ;
			break;
		case T_UNBIND_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_UNBIND_REQ----> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_UNBIND_REQ;
			break;
		case T_UNITDATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_UNITDATA_REQ--> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_UNITDATA_REQ;
			break;
		case T_OPTMGMT_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_OPTMGMT_REQ---> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_OPTMGMT_REQ;
			break;
		case T_ORDREL_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_ORDREL_REQ----> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_ORDREL_REQ;
			break;
		case T_OPTDATA_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_OPTDATA_REQ---> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_OPTDATA_REQ;
			break;
		case T_ADDR_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_ADDR_REQ------> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_ADDR_REQ;
			break;
		case T_CAPABILITY_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_CAPABILITY_REQ> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_CAPABILITY_REQ;
			break;
		default:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_????_???------> (%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = DECODEERROR;
			break;
		}
		if (data && data->len >= 0)
			if (do_decode_data(fd, data) != __TEST_DATA)
				return (FAILURE);
		return (ret);
	}
	return (DECODEERROR);
}

static int
top_decode_data(struct strbuf *data)
{
	return do_decode_data(top_fd, data);
}
static int
top_decode_ctrl(struct strbuf *ctrl, struct strbuf *data)
{
	return do_decode_ctrl(top_fd, ctrl, data);
}
static int
bot_decode_data(struct strbuf *data)
{
	return do_decode_data(bot_fd, data);
}
static int
bot_decode_ctrl(struct strbuf *ctrl, struct strbuf *data)
{
	return do_decode_ctrl(bot_fd, ctrl, data);
}

#define IUT 0x00000001UL
#define PT  0x00000002UL
#define ANY 0x00000003UL

static int
top_get_data(int action)
{
	int ret;
	switch (action) {
	case __TEST_READ:
	{
		char buf[BUFSIZE];
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-read(2)-----------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = read(top_fd, buf, BUFSIZE)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "***************ERROR: read failed\n");
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		}
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
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-readv(2)----------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = readv(top_fd, vector, count)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "***************ERROR: readv failed\n");
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		}
		break;
	}
	case __TEST_GETMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		int flag = 0;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-getmsg(2)---------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getmsg(top_fd, NULL, &data, &flag)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "***************ERROR: getmsg failed\n");
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		} else
			ret = data.len;
		break;
	}
	case __TEST_GETPMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		int band = 0;
		int flag = 0;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-getpmsg(2)--------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getpmsg(top_fd, NULL, &data, &band, &flag)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "***************ERROR: getpmsg failed\n");
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		} else
			ret = data.len;
		break;
	}
	}
	return (ret);
}

static int
bot_get_data(int action)
{
	int ret;
	switch (action) {
	case __TEST_READ:
	{
		char buf[BUFSIZE];
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--read(2)---------->(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = read(bot_fd, buf, BUFSIZE)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                                              ****ERROR: read failed\n");
				fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		}
		break;
	}
	case __TEST_READV:
	{
		char buf[BUFSIZE];
		static const size_t count = 4;
		struct iovec vector[4] = {
			{buf, (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2), (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2) + (BUFSIZE >> 2), (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2) + (BUFSIZE >> 2) + (BUFSIZE >> 2), (BUFSIZE >> 2)}
		};
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--readv(2)--------->(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = readv(bot_fd, vector, count)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                                              ****ERROR: readv failed\n");
				fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		}
		break;
	}
	case __TEST_GETMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		int flag = 0;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--getmsg(2)-------->(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getmsg(bot_fd, NULL, &data, &flag)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                                              ****ERROR: getmsg failed\n");
				fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		} else
			ret = data.len;
		break;
	}
	case __TEST_GETPMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		int band = 0;
		int flag = 0;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--getpmsg(2)------->(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getpmsg(bot_fd, NULL, &data, &band, &flag)) < 0) {
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                                              ****ERROR: getpmsg failed\n");
				fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
		} else
			ret = data.len;
		break;
	}
	}
	return (ret);
}

static int
wait_event(int wait, int source)
{
	while (1) {
		struct pollfd pfd[] = {
			{bot_fd, POLLIN | POLLPRI, 0},
			{top_fd, POLLIN | POLLPRI, 0}
		};
		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "++++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
				show_timeout--;
			}
			last_event = TIMEOUT;
			return time_event(TIMEOUT);
		}
		// fprintf(stdout, "polling:\n");
		// fflush(stdout);
		pfd[0].fd = bot_fd;
		pfd[0].events = (source & PT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[0].revents = 0;
		pfd[1].fd = top_fd;
		pfd[1].events = (source & IUT) ? (POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP) : 0;
		pfd[1].revents = 0;
		switch (poll(pfd, 2, wait)) {
		case -1:
			if ((errno == EAGAIN || errno == EINTR))
				break;
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "X-X-X-X-X-X-X-X-X-X-|-X|X-X-X-X-X- ERROR! -X-X-X-X-X-X-|-X-X-X-X-X-X-X-X-X-X(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			break;
		case 0:
			if (verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "- - - - - - - - - - | -|- - - - - nothing! - - - - - - | - - - - - - - - - -(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			last_event = NO_MSG;
			return time_event(NO_MSG);
		case 1:
		case 2:
			if (pfd[1].revents) {
				int flags = 0;
				char cbuf[BUFSIZE];
				char dbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf };
				struct strbuf data = { BUFSIZE, 0, dbuf };
				// fprintf(stdout, "getmsg from iut:\n");
				// fflush(stdout);
				if (getmsg(top_fd, &ctrl, &data, &flags) == 0) {
					// fprintf(stdout, "gotmsg from iut
					// [%d,%d]:\n",ctrl.len,data.len);
					// fflush(stdout);
					if (ctrl.len > 0) {
						if ((last_event = top_decode_ctrl(&ctrl, &data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = top_decode_data(&data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
			if (pfd[0].revents) {
				int flags = 0;
				char cbuf[BUFSIZE];
				char dbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf };
				struct strbuf data = { BUFSIZE, 0, dbuf };
				// fprintf(stdout, "getmsg from pt:\n");
				// fflush(stdout);
				if (getmsg(bot_fd, &ctrl, &data, &flags) == 0) {
					// fprintf(stdout, "gotmsg from pt
					// [%d,%d]:\n",ctrl.len,data.len);
					// fflush(stdout);
					if (ctrl.len > 0) {
						if ((last_event = bot_decode_ctrl(&ctrl, &data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = bot_decode_data(&data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
		default:
			break;
		}
	}
}

int
any_event(void)
{
	return wait_event(-1, ANY);
}

int
top_wait_event(int wait)
{
	while (1) {
		struct pollfd pfd[] = { {top_fd, POLLIN | POLLPRI, 0} };
		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "++++++++++++++++++++|  |+++++++++ TIMEOUT! ++++++++++++|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
				show_timeout--;
			}
			last_event = TIMEOUT;
			return time_event(TIMEOUT);
		}
		pfd[0].fd = top_fd;
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			if ((errno == EAGAIN || errno == EINTR))
				break;
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "X-X-X-X-X-X-X-X-X-X-|  |X-X-X-X-X- ERROR! -X-X-X-X-X-X-|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			break;
		case 0:
			if (verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "- - - - - - - - - - |  |- - - - - nothing! - - - - - - |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			last_event = NO_MSG;
			return time_event(NO_MSG);
		case 1:
			if (pfd[0].revents) {
				int flags = 0;
				char cbuf[BUFSIZE];
				char dbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf };
				struct strbuf data = { BUFSIZE, 0, dbuf };
				if (getmsg(top_fd, &ctrl, &data, &flags) == 0) {
					if (ctrl.len > 0) {
						if ((last_event = top_decode_ctrl(&ctrl, &data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = top_decode_data(&data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
		default:
			break;
		}
	}
	return UNKNOWN;
}

int
top_event(void)
{
	return top_wait_event(-1);
}

int
bot_wait_event(int wait)
{
	while (1) {
		struct pollfd pfd[] = { {bot_fd, POLLIN | POLLPRI, 0} };
		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
				show_timeout--;
			}
			last_event = TIMEOUT;
			return time_event(TIMEOUT);
		}
		pfd[0].fd = bot_fd;
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			if ((errno == EAGAIN || errno == EINTR))
				break;
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |X-X-X-X-X- ERROR! -X-X-X-X-X-X-|-X-X-X-X-X-X-X-X-X-X(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			break;
		case 0:
			if (verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - nothing! - - - - - - | - - - - - - - - - -(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			last_event = NO_MSG;
			return time_event(NO_MSG);
		case 1:
			if (pfd[0].revents) {
				int flags = 0;
				char cbuf[BUFSIZE];
				char dbuf[BUFSIZE];
				struct strbuf ctrl = { BUFSIZE, 0, cbuf };
				struct strbuf data = { BUFSIZE, 0, dbuf };
				if (getmsg(bot_fd, &ctrl, &data, &flags) == 0) {
					if (ctrl.len > 0) {
						if ((last_event = bot_decode_ctrl(&ctrl, &data)) != UNKNOWN)
							return time_event(last_event);
					} else if (data.len > 0) {
						if ((last_event = bot_decode_data(&data)) != UNKNOWN)
							return time_event(last_event);
					}
				}
			}
		default:
			break;
		}
	}
	return UNKNOWN;
}

int
bot_event(void)
{
	return bot_wait_event(-1);
}

int
do_wait_event(int fd, int wait)
{
	if (fd == top_fd)
		return top_wait_event(wait);
	if (fd == bot_fd)
		return bot_wait_event(wait);
	return (SCRIPTERROR);
}

int
get_event(int fd)
{
	return do_wait_event(fd, -1);
}

int
get_data(int fd, int action)
{
	if (fd == top_fd)
		return top_get_data(action);
	if (fd == bot_fd)
		return bot_get_data(action);
	return (SCRIPTERROR);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */
static int
preamble_0(int fd)
{
	state = 0;
	start_tt(1000);
	return SUCCESS;
};

static int
postamble_0(int fd)
{
	state = 0;
	stop_tt();
	state = 1;
	start_tt(1000);
	for (;;) {
		state++;
		switch (do_wait_event(fd, 0)) {
		case NO_MSG:
			break;
		case TIMEOUT:
			break;
		default:
			continue;
		}
		break;
	}
	state++;
	stop_tt();
	return SUCCESS;
};

static int
preamble_1_top(int fd)
{
	state = 0;
	start_tt(1000);
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	return (SUCCESS);
}

static int
postamble_1_top(int fd)
{
	while (do_wait_event(fd, 0) != NO_MSG) ;
	state = 0;
	stop_tt();
	if (do_signal(fd, __TEST_POP))
		return (FAILURE);
	return (SUCCESS);
}

static int
postamble_1_bot(int fd)
{
	state = 0;
	for (;;)
		switch (get_event(fd)) {
		case __TEST_DISCON_REQ:
			stop_tt();
			return SUCCESS;
		case NO_MSG:
			break;;
		default:
			stop_tt();
			return (FAILURE);
		}
	return SCRIPTERROR;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test Cases
 *
 *  -------------------------------------------------------------------------
 */

#define desc_case_1_1 "\
Pushing and popping the tirdwr module\n\
-- normal push and pop sequence\n\
This test case tests the simple pushing and popping of the tirdwr module.  It\n\
ensures that pushing the module on an empty stream has no effect, while\n\
popping the module from the stream results in a T_DISCON_REQ being sent to the\n\
transport peer."
static int
test_case_1_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_1_1_bot(int fd)
{
	state = 0;
	while (get_event(fd) == NO_MSG) ;
	switch (last_event) {
	case __TEST_DISCON_REQ:
		state = 1;
		return (SUCCESS);
	}
	state = 2;
	return (FAILURE);
}

#define desc_case_1_2 "\
Pushing and popping the tirdwr module\n\
-- pop after orderly release\n\
This test case tests the pushing and popping of the tirdwr module.  It ensures\n\
that the pushing the module on an empty stream has no effect, while popping\n\
the module from the stream after receiving an orderly release indication\n\
results in a T_ORDREL_REQ being sent to the transport peer."
static int
test_case_1_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	while (get_event(fd) == NO_MSG) ;
	switch (last_event) {
	case TIMEOUT:
		start_tt(500);
		break;
	default:
		return (FAILURE);
	}
	state = 2;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_1_2_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	while (get_event(fd) == NO_MSG) ;
	switch (last_event) {
	case __TEST_ORDREL_REQ:
		state = 2;
		return (SUCCESS);
	}
	state = 3;
	return (FAILURE);
}

#define desc_case_1_3 "\
Pushing and popping the tirdwr module\n\
-- push after receiving M_DATA\n\
This test case tests that the module can be successfully pushed after a number\n\
of M_DATA have been received on the stream."
static int
test_case_1_3_top(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	pause();
	switch (get_event(fd)) {
	case TIMEOUT:
		start_tt(500);
		break;
	default:
		return (FAILURE);
	}
	state = 2;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}
static int
test_case_1_3_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 3;
	while (get_event(fd) == NO_MSG) ;
	switch (last_event) {
	case __TEST_DISCON_REQ:
		state = 4;
		return (SUCCESS);
	}
	state = 5;
	return (FAILURE);
}

#define desc_case_1_4 "\
Pushing and popping the tirdwr module\n\
-- push after receiving T_DATA_IND\n\
This test case tests that the module cannot be pushed on a stream that has\n\
already received T_DATA_IND primitives."
static int
test_case_1_4_top(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	pause();
	switch (get_event(fd)) {
	case TIMEOUT:
		start_tt(500);
		break;
	default:
		return (FAILURE);
	}
	state = 2;
	if (do_signal(fd, __TEST_PUSH) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_1_4_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}

#define desc_case_2_1 "\
Receiving data\n\
-- receiving M_DATA\n\
This test case ensures that the application can receive data formatted as\n\
M_DATA messages after pushing the module."
static int
test_case_2_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	start_tt(500);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_2_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	while (get_event(fd) == NO_MSG) ;
	state = 7;
	switch (last_event) {
	case __TEST_ORDREL_REQ:
		break;
	default:
		return (FAILURE);
	}
	state = 8;
	return (SUCCESS);
}

#define desc_case_2_2 "\
Receiving data\n\
-- receiving T_DATA_IND\n\
This test case ensures that the application can receive data formatted as\n\
T_DATA_IND messages after pushing the module."
static int
test_case_2_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	start_tt(500);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_2_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	while (get_event(fd) == NO_MSG) ;
	state = 7;
	switch (last_event) {
	case __TEST_ORDREL_REQ:
		break;
	default:
		return (FAILURE);
	}
	state = 8;
	return (SUCCESS);
}

#define desc_case_2_3 "\
Receiving data\n\
-- receiving T_OPTDATA_IND\n\
This test case ensures that the application can receive data formatted as\n\
T_OPTDATA_IND messages after pushing the module."
static int
test_case_2_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	start_tt(500);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_2_3_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_NRM_OPTDATA_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_NRM_OPTDATA_IND) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_NRM_OPTDATA_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	while (get_event(fd) == NO_MSG) ;
	state = 7;
	switch (last_event) {
	case __TEST_ORDREL_REQ:
		break;
	default:
		return (FAILURE);
	}
	state = 8;
	return (SUCCESS);
}

#define desc_case_2_4 "\
Receiving data\n\
-- receiving after orderly release\n\
This test case ensures that the application can receive data formatted as\n\
T_DATA_IND messages after pushing the module.  It also ensures that the\n\
application will receive zero returns to reads after an orderly release\n\
indication has been received."
static int
test_case_2_4_top(int fd)
{
	int count = 0;
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	start_tt(500);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			count++;
			if (count == 3) {
				stop_tt();
				break;
			}
			continue;
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	start_tt(500);
	state++;
	if (get_data(fd, __TEST_READ) != 0)
		return (FAILURE);
	state++;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_2_4_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	start_tt(800);
	state = 3;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	while (get_event(fd) == NO_MSG) ;
	state = 7;
	switch (last_event) {
	case __TEST_ORDREL_REQ:
		break;
	default:
		return (FAILURE);
	}
	state = 8;
	return (SUCCESS);
}

#define desc_case_2_5 "\
Receiving data\n\
-- receiving after orderly release\n\
This test case ensures that the application can receive data formatted as\n\
T_DATA_IND messages after pushing the module.  It also ensures that the\n\
application will receive zero returns to reads after disconnect indication has\n\
been received."
static int
test_case_2_5_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	start_tt(500);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	if (get_data(fd, __TEST_READ) != 0)
		return (FAILURE);
	state++;
	if (do_signal(fd, __TEST_POP) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_2_5_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	start_tt(800);
	state = 3;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_DATA_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 7;
	while (1) {
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state = 8;
	return (SUCCESS);
}

#define desc_case_3_1 "\
Sending data\n\
-- sending data with write(2)\n\
Tests that data can be sent with write(2) after pushing the module."
static int
test_case_3_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
test_case_3_1_bot(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case __TEST_DISCON_REQ:
			break;
		case TIMEOUT:
			return (FAILURE);
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_3_2 "\
Sending data\n\
-- sending data with writev(2)\n\
Tests that data can be sent with writev(2) after pushing the module."
static int
test_case_3_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_WRITEV) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_WRITEV) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_WRITEV) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
test_case_3_2_bot(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case __TEST_DISCON_REQ:
			break;
		case TIMEOUT:
			return (FAILURE);
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_3_3 "\
Sending data\n\
-- sending data with putmsg(2)\n\
Tests that data can be sent with putmsg(2) after pushing the module."
static int
test_case_3_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_PUTMSG_DATA) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_PUTMSG_DATA) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_PUTMSG_DATA) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
test_case_3_3_bot(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case __TEST_DISCON_REQ:
			break;
		case TIMEOUT:
			return (FAILURE);
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_3_4 "\
Sending data\n\
-- sending data with putpmsg(2)\n\
Tests that expedited data cannot be sent with putpmsg(2) after pushing the module."
static int
test_case_3_4_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_PUTPMSG_DATA) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_PUTPMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_PUTPMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
test_case_3_4_bot(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case __TEST_DISCON_REQ:
			break;
		case TIMEOUT:
			return (FAILURE);
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_3_5 "\
Sending data\n\
-- sending data with write(2) after orderly release\n\
Tests that data can be sent with write(2) after orderly release."
static int
test_case_3_5_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_WRITE) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_3_5_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case __TEST_ORDREL_REQ:
			break;
		case TIMEOUT:
			return (FAILURE);
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_3_6 "\
Sending data\n\
-- sending data with write(2) after abortive disconnect\n\
Tests that data cannot be sent with write(2) after disconnect."
static int
test_case_3_6_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_WRITE) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_WRITE) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_WRITE) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_POP) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_3_6_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	start_tt(200);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			return (FAILURE);
		case __TEST_ORDREL_REQ:
			return (FAILURE);
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_3_7 "\
Sending data\n\
-- sending data with putmsg(2) after orderly release\n\
Tests that data can be sent with putmsg(2) after orderly release."
static int
test_case_3_7_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_PUTMSG_DATA) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_PUTMSG_DATA) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_PUTMSG_DATA) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_3_7_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			continue;
		case __TEST_ORDREL_REQ:
			break;
		case TIMEOUT:
			return (FAILURE);
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_3_8 "\
Sending data\n\
-- sending data with putmsg(2) after abortive disconnect\n\
Tests that data cannot be sent with putmsg(2) after disconnect."
static int
test_case_3_8_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_PUTMSG_DATA) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_PUTMSG_DATA) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_PUTMSG_DATA) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_POP) == SUCCESS || last_errno != ENXIO)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_3_8_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	start_tt(200);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case __TEST_DATA:
			return (FAILURE);
		case __TEST_ORDREL_REQ:
			return (FAILURE);
		case TIMEOUT:
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}

#define desc_case_4_1_1 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_CONN_IND\n\
Tests that receipt of a T_CONN_IND after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	pause();
	state = 3;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	start_tt(500);
	state = 4;
	if (get_data(fd, __TEST_READ) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 5;
	if (get_data(fd, __TEST_READV) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 6;
	if (get_data(fd, __TEST_GETMSG) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 7;
	if (get_data(fd, __TEST_GETPMSG) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_WRITE) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_WRITEV) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_PUTMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 11;
	if (do_signal(fd, __TEST_PUTPMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 12;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 13;
	return (SUCCESS);
}
static int
test_case_4_1_bot(int fd, int signal)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	start_tt(800);
	if (do_signal(fd, signal) != SUCCESS)
		return (FAILURE);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case TIMEOUT:
			break;
		case __TEST_DISCON_REQ:	/* is this ok? */
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_4_1_1_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_CONN_IND);
}

#define desc_case_4_1_2 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_CONN_CON\n\
Tests that receipt of a T_CONN_CON after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_2_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_CONN_CON);
}

#define desc_case_4_1_3 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_EXDATA_IND\n\
Tests that receipt of a T_EXDATA_IND after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_3_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_EXDATA_IND);
}

#define desc_case_4_1_4 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_INFO_ACK\n\
Tests that receipt of a T_INFO_ACK after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_4_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_INFO_ACK);
}

#define desc_case_4_1_5 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_BIND_ACK\n\
Tests that receipt of a T_BIND_ACK after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_5_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_BIND_ACK);
}

#define desc_case_4_1_6 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_ERROR_ACK\n\
Tests that receipt of a T_ERROR_ACK after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_6_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_ERROR_ACK);
}

#define desc_case_4_1_7 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_OK_ACK\n\
Tests that receipt of a T_OK_ACK after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_7_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_OK_ACK);
}

#define desc_case_4_1_8 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_UNITDATA_IND\n\
Tests that receipt of a T_UNITDATA_IND after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_8_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_UNITDATA_IND);
}

#define desc_case_4_1_9 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_UDERROR_IND\n\
Tests that receipt of a T_UDERROR_IND after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_9_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_UDERROR_IND);
}

#define desc_case_4_1_10 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_OPTMGMT_ACK\n\
Tests that receipt of a T_OPTMGMT_ACK after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_10_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_OPTMGMT_ACK);
}

#define desc_case_4_1_11 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_ADDR_ACK\n\
Tests that receipt of a T_ADDR_ACK after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_11_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_ADDR_ACK);
}

#define desc_case_4_1_12 "\
Unexpected messages received from Transport Provider\n\
-- receiving T_CAPABILITY_ACK\n\
Tests that receipt of a T_CAPABILITY_ACK after the module has been pushed results in\n\
an error on the stream."
static int
test_case_4_1_12_bot(int fd)
{
	return test_case_4_1_bot(fd, __TEST_CAPABILITY_ACK);
}

#define desc_case_4_2_1 "\
Unexpected messages sent from the Transport User\n\
-- sending T_CONN_REQ\n\
Tests that sending a T_CONN_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_top(int fd, int signal)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, signal) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (get_data(fd, __TEST_READ) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 3;
	if (get_data(fd, __TEST_READV) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 4;
	if (get_data(fd, __TEST_GETMSG) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 5;
	if (get_data(fd, __TEST_GETPMSG) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_WRITE) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_WRITEV) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_PUTMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_PUTPMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 11;
	return (SUCCESS);
}
static int
test_case_4_2_bot(int fd)
{
	state = 0;
	start_tt(2000);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case TIMEOUT:
			break;
		case __TEST_DISCON_REQ:	/* is this ok? */
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_4_2_1_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_CONN_REQ);
}

#define desc_case_4_2_2 "\
Unexpected messages sent from the Transport User\n\
-- sending T_CONN_RES\n\
Tests that sending a T_CONN_RES after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_2_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_CONN_RES);
}

#define desc_case_4_2_3 "\
Unexpected messages sent from the Transport User\n\
-- sending T_DISCON_REQ\n\
Tests that sending a T_DISCON_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_3_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_DISCON_REQ);
}

#define desc_case_4_2_4 "\
Unexpected messages sent from the Transport User\n\
-- sending T_DATA_REQ\n\
Tests that sending a T_DATA_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_4_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_DATA_REQ);
}

#define desc_case_4_2_5 "\
Unexpected messages sent from the Transport User\n\
-- sending T_EXDATA_REQ\n\
Tests that sending a T_EXDATA_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_5_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_EXDATA_REQ);
}

#define desc_case_4_2_6 "\
Unexpected messages sent from the Transport User\n\
-- sending T_INFO_REQ\n\
Tests that sending a T_INFO_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_6_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_INFO_REQ);
}

#define desc_case_4_2_7 "\
Unexpected messages sent from the Transport User\n\
-- sending T_BIND_REQ\n\
Tests that sending a T_BIND_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_7_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_BIND_REQ);
}

#define desc_case_4_2_8 "\
Unexpected messages sent from the Transport User\n\
-- sending T_UNBIND_REQ\n\
Tests that sending a T_UNBIND_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_8_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_UNBIND_REQ);
}

#define desc_case_4_2_9 "\
Unexpected messages sent from the Transport User\n\
-- sending T_UNITDATA_REQ\n\
Tests that sending a T_UNITDATA_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_9_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_UNITDATA_REQ);
}

#define desc_case_4_2_10 "\
Unexpected messages sent from the Transport User\n\
-- sending T_OPTMGMT_REQ\n\
Tests that sending a T_OPTMGMT_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_10_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_OPTMGMT_REQ);
}

#define desc_case_4_2_11 "\
Unexpected messages sent from the Transport User\n\
-- sending T_ORDREL_REQ\n\
Tests that sending a T_ORDREL_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_11_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_ORDREL_REQ);
}

#define desc_case_4_2_12 "\
Unexpected messages sent from the Transport User\n\
-- sending T_OPTDATA_REQ\n\
Tests that sending a T_OPTDATA_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_12_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_OPTDATA_REQ);
}

#define desc_case_4_2_13 "\
Unexpected messages sent from the Transport User\n\
-- sending T_ADDR_REQ\n\
Tests that sending a T_ADDR_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_13_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_ADDR_REQ);
}

#define desc_case_4_2_14 "\
Unexpected messages sent from the Transport User\n\
-- sending T_CAPABILITY_REQ\n\
Tests that sending a T_CAPABILITY_REQ after the module has been pushed results in an\n\
error on the stream."
static int
test_case_4_2_14_top(int fd)
{
	return test_case_4_2_top(fd, __TEST_CAPABILITY_REQ);
}

#define desc_case_4_3_1 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_GETINFO\n\
Tests that performing a TI_GETINFO IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_top(int fd, int signal)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, signal) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 2;
	if (get_data(fd, __TEST_READ) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 3;
	if (get_data(fd, __TEST_READV) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 4;
	if (get_data(fd, __TEST_GETMSG) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 5;
	if (get_data(fd, __TEST_GETPMSG) >= 0 || last_errno != EPROTO)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_WRITE) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_WRITEV) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_PUTMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_PUTPMSG_DATA) == SUCCESS || last_errno != EPROTO)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 11;
	return (SUCCESS);
}
static int
test_case_4_3_bot(int fd)
{
	state = 0;
	start_tt(2000);
	for (;;) {
		state++;
		switch (get_event(fd)) {
		case NO_MSG:
			continue;
		case TIMEOUT:
			break;
		case __TEST_DISCON_REQ:	/* is this ok? */
			break;
		default:
			return (FAILURE);
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_4_3_1_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_GETINFO);
}

#define desc_case_4_3_2 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_OPTMGMT\n\
Tests that performing a TI_OPTMGMT IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_2_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_OPTMGMT);
}

#define desc_case_4_3_3 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_BIND\n\
Tests that performing a TI_BIND IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_3_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_BIND);
}

#define desc_case_4_3_4 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_UNBIND\n\
Tests that performing a TI_UNBIND IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_4_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_UNBIND);
}

#define desc_case_4_3_5 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_GETMYNAME\n\
Tests that performing a TI_GETMYNAME IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_5_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_GETMYNAME);
}

#define desc_case_4_3_6 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_GETPEERNAME\n\
Tests that performing a TI_GETPEERNAME IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_6_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_GETPEERNAME);
}

#define desc_case_4_3_7 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_SETMYNAME\n\
Tests that performing a TI_SETMYNAME IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_7_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_SETMYNAME);
}

#define desc_case_4_3_8 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_SETPEERNAME\n\
Tests that performing a TI_SETPEERNAME IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_8_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_SETPEERNAME);
}

#define desc_case_4_3_9 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_SYNC\n\
Tests that performing a TI_SYNC IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_9_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_SYNC);
}

#define desc_case_4_3_10 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_GETADDRS\n\
Tests that performing a TI_GETADDRS IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_10_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_GETADDRS);
}

#define desc_case_4_3_11 "\
Unexpected XTI IO controls from the Transport User\n\
-- performing TI_CAPABILITY\n\
Tests that performing a TI_CAPABILITY IO control after the module has been pushed\n\
results in an error on the stream."
static int
test_case_4_3_11_top(int fd)
{
	return test_case_4_3_top(fd, __TEST_TI_CAPABILITY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  PT (Protocol Tester) Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int
bot_open(void)
{
	int pfd[2];
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    X--+----------------------------+--X---pipe()           {%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (pipe(pfd) < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                                              ****ERROR: pipe failed\n");
			fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
	bot_fd = pfd[0];
	top_fd = pfd[1];
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .  .                            |  |<--I_SRDOPT---------{%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (ioctl(bot_fd, I_SRDOPT, RMSGD) < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                                              ****ERROR: ioctl failed\n");
			fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .  .                            |  |<--I_PUSH-----------{%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (ioctl(bot_fd, I_PUSH, "pipemod") < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                                              ****ERROR: push failed\n");
			fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
	return SUCCESS;
}

static int
bot_close(void)
{
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .  .                            |  |<--I_POP------------{%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (ioctl(bot_fd, I_POP, 0) < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                                              ****ERROR: pop failed\n");
			fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    .  .                            X--X---close()          {%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (close(bot_fd) < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                                              ****ERROR: close pipe[0] failed\n");
			fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
	if (close(top_fd) < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                                              ****ERROR: close pipe[1] failed\n");
			fprintf(stdout, "                                              ****ERROR: %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
	return SUCCESS;
}

static int
bot_start(void)
{
	if (bot_open() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

static int
bot_stop(void)
{
	if (bot_close() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IUT (Implementation Under Test) Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int
top_push(void)
{
#if 0
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "---I_SRDOPT-------->|  |                            |  |                    {%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (ioctl(top_fd, I_SRDOPT, RMSGD) < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "***************ERROR: ioctl failed\n");
			fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "---I_PUSH---------->|  |                            |  |                    {%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (ioctl(top_fd, I_PUSH, "tirdwr") < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "***************ERROR: ioctl failed\n");
			fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
#endif
	return SUCCESS;
}

static int
top_pop(void)
{
#if 0
	if (verbose > 1) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "---I_POP----------->|  |                            |  |                    {%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (ioctl(top_fd, I_POP, NULL) < 0) {
		last_errno = errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "***************ERROR: ioctl failed\n");
			fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	}
#endif
	return SUCCESS;
}

static int
top_start(void)
{
	if (top_push() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

static int
top_stop(void)
{
	if (top_pop() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test harness initialization.
 *
 *  -------------------------------------------------------------------------
 */

static int
begin_tests(void)
{
	if (bot_start() != SUCCESS)
		return FAILURE;
	if (top_start() != SUCCESS)
		return FAILURE;
	show_acks = 1;
	return SUCCESS;
}

static int
end_tests(void)
{
	show_acks = 0;
	if (top_stop() != SUCCESS)
		return FAILURE;
	if (bot_stop() != SUCCESS)
		return FAILURE;
	return SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test case child scheduler
 *
 *  -------------------------------------------------------------------------
 */
struct test_side {
	int (*preamble) (int);		/* test preamble */
	int (*testcase) (int);		/* test case */
	int (*postamble) (int);		/* test postamble */
};

int
top_run(struct test_side *side)
{
	int result = SCRIPTERROR;
	if (verbose) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "--------------------+  +---------Preamble--------------+                    \n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (side->preamble && side->preamble(top_fd) != SUCCESS) {
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "????????????????????|  |????\? INCONCLUSIVE ??????????\?|                     [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = INCONCLUSIVE;
	} else {
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "--------------------|  |-----------Test----------------|                    \n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		switch (side->testcase(top_fd)) {
		default:
		case INCONCLUSIVE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "????????????????????|  |????\? INCONCLUSIVE ??????????\?|                     [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = INCONCLUSIVE;
			break;
		case FAILURE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "XXXXXXXXXXXXXXXXXXXX|  |XXXXXXXXX FAILED XXXXXXXXXXXXXX|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = FAILURE;
			break;
		case SCRIPTERROR:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "####################|  |####### SCRIPT ERROR ##########|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = SCRIPTERROR;
			break;
		case SUCCESS:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "********************|  |********* PASSED **************|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = SUCCESS;
			break;
		}
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "--------------------|  |----------Postamble------------|                    \n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if (side->postamble && side->postamble(top_fd) != SUCCESS) {
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "????????????????????|  |????\? INCONCLUSIVE ??????????\?|                     [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == SUCCESS)
				result = INCONCLUSIVE;
		}
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "--------------------+  +-------------------------------+                    \n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
	}
	exit(result);
}

int
bot_run(struct test_side *side)
{
	int result = SCRIPTERROR;
	if (verbose) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +  +---------Preamble--------------+--------------------\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (side->preamble && side->preamble(bot_fd) != SUCCESS) {
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |????\? INCONCLUSIVE ??????????\?|????????????????????\?(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = INCONCLUSIVE;
	} else {
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |-----------Test----------------|--------------------\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		switch (side->testcase(bot_fd)) {
		default:
		case INCONCLUSIVE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |????\? INCONCLUSIVE ??????????\?|????????????????????\?(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = INCONCLUSIVE;
			break;
		case FAILURE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |XXXXXXXXX FAILED XXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = FAILURE;
			break;
		case SCRIPTERROR:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |####### SCRIPT ERROR ###########|###################(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = SCRIPTERROR;
			break;
		case SUCCESS:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |********* PASSED **************|********************(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = SUCCESS;
			break;
		}
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |----------Postamble------------|--------------------\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if (side->postamble && side->postamble(bot_fd) != SUCCESS) {
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |????\? INCONCLUSIVE ??????????\?|????????????????????\?(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == SUCCESS)
				result = INCONCLUSIVE;
		}
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +  +-------------------------------+--------------------\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
	}
	exit(result);
}

/*
 *  Fork multiple children to do the actual testing.  The top child is the
 *  process running above the tested module, the bot child is the process
 *  running below (at other end of pipe) the tested module.
 */

int
test_run(struct test_side *top_side, struct test_side *bot_side)
{
	int children = 0;
	pid_t got_chld, top_chld = 0, bot_chld = 0;
	int got_stat, top_stat, bot_stat;
	start_tt(5000);
	if (top_side) {
		switch ((top_chld = fork())) {
		case 00:	/* we are the child */
			exit(top_run(top_side));	/* execute top state machine */
		case -1:	/* error */
			return FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		top_stat = SUCCESS;
	if (bot_side) {
		switch ((bot_chld = fork())) {
		case 00:	/* we are the child */
			exit(bot_run(bot_side));	/* execute bot state machine */
		case -1:	/* error */
			if (top_chld)
				kill(top_chld, SIGKILL);	/* toast top child */
			return FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		bot_stat = SUCCESS;
	for (; children > 0; children--) {
		if ((got_chld = wait(&got_stat)) > 0) {
			if (WIFEXITED(got_stat)) {
				int status = WEXITSTATUS(got_stat);
				if (got_chld == top_chld) {
					top_stat = status;
					top_chld = 0;
				}
				if (got_chld == bot_chld) {
					bot_stat = status;
					bot_chld = 0;
				}
			} else if (WIFSIGNALED(got_stat)) {
				int signal = WTERMSIG(got_stat);
				if (got_chld == top_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "@@@@@@@@@@@@@@@@@@@@|  |@@@@@@@ TERMINATED @@@@@@@@@@@@@|                   {%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (bot_chld)
						kill(bot_chld, SIGKILL);
					top_stat = FAILURE;
					top_chld = 0;
				}
				if (got_chld == bot_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |  |@@@@@@@ TERMINATED @@@@@@@@@@@@@|@@@@@@@@@@@@@@@@@@@{%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (top_chld)
						kill(bot_chld, SIGKILL);
					bot_stat = FAILURE;
					bot_chld = 0;
				}
			} else if (WIFSTOPPED(got_stat)) {
				int signal = WSTOPSIG(got_stat);
				if (got_chld == top_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "&&&&&&&&&&&&&&&&&&&&|  |&&&&&&& STOPPED &&&&&&&&&&&&&&&&|                   {%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (bot_chld)
						kill(bot_chld, SIGKILL);
					top_stat = FAILURE;
					top_chld = 0;
				}
				if (got_chld == bot_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |  |&&&&&&& STOPPED &&&&&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&&{%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (top_chld)
						kill(bot_chld, SIGKILL);
					bot_stat = FAILURE;
					bot_chld = 0;
				}
			}
		} else {
			if (timer_timeout) {
				timer_timeout = 0;
				if (show_timeout || verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "++++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++++|++++++++++++++++++++{%d}\n", state);
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
					show_timeout--;
				}
				last_event = TIMEOUT;
			}
			if (top_chld) {
				kill(top_chld, SIGKILL);
				top_stat = FAILURE;
				top_chld = 0;
			}
			if (bot_chld) {
				kill(bot_chld, SIGKILL);
				bot_stat = FAILURE;
				bot_chld = 0;
			}
			break;
		}
	}
	stop_tt();
	if (top_stat == FAILURE || bot_stat == FAILURE)
		return (FAILURE);
	if (top_stat == SUCCESS && bot_stat == SUCCESS)
		return (SUCCESS);
	return (INCONCLUSIVE);
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
	struct test_side top;		/* top process (process above the module) */
	struct test_side bot;		/* bot process (process below the module) */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} tests[] = {
	{
		"1.1", desc_case_1_1, {
		&preamble_0, &test_case_1_1_top, &postamble_0}, {
	&preamble_0, &test_case_1_1_bot, &postamble_0}, 0, 0}, {
		"1.2", desc_case_1_2, {
		&preamble_0, &test_case_1_2_top, &postamble_0}, {
	&preamble_0, &test_case_1_2_bot, &postamble_0}, 0, 0}, {
		"1.3", desc_case_1_3, {
		&preamble_0, &test_case_1_3_top, &postamble_0}, {
	&preamble_0, &test_case_1_3_bot, &postamble_0}, 0, 0}, {
		"1.4", desc_case_1_4, {
		&preamble_0, &test_case_1_4_top, &postamble_0}, {
	&preamble_0, &test_case_1_4_bot, &postamble_0}, 0, 0}, {
		"2.1", desc_case_2_1, {
		&preamble_0, &test_case_2_1_top, &postamble_0}, {
	&preamble_0, &test_case_2_1_bot, &postamble_0}, 0, 0}, {
		"2.2", desc_case_2_2, {
		&preamble_0, &test_case_2_2_top, &postamble_0}, {
	&preamble_0, &test_case_2_2_bot, &postamble_0}, 0, 0}, {
		"2.3", desc_case_2_3, {
		&preamble_0, &test_case_2_3_top, &postamble_0}, {
	&preamble_0, &test_case_2_3_bot, &postamble_0}, 0, 0}, {
		"2.4", desc_case_2_4, {
		&preamble_0, &test_case_2_4_top, &postamble_0}, {
	&preamble_0, &test_case_2_4_bot, &postamble_0}, 0, 0}, {
		"2.5", desc_case_2_5, {
		&preamble_0, &test_case_2_5_top, &postamble_0}, {
	&preamble_0, &test_case_2_5_bot, &postamble_0}, 0, 0}, {
		"3.1", desc_case_3_1, {
		&preamble_0, &test_case_3_1_top, &postamble_0}, {
	&preamble_0, &test_case_3_1_bot, &postamble_0}, 0, 0}, {
		"3.2", desc_case_3_2, {
		&preamble_0, &test_case_3_2_top, &postamble_0}, {
	&preamble_0, &test_case_3_2_bot, &postamble_0}, 0, 0}, {
		"3.3", desc_case_3_3, {
		&preamble_0, &test_case_3_3_top, &postamble_0}, {
	&preamble_0, &test_case_3_3_bot, &postamble_0}, 0, 0}, {
		"3.4", desc_case_3_4, {
		&preamble_0, &test_case_3_4_top, &postamble_0}, {
	&preamble_0, &test_case_3_4_bot, &postamble_0}, 0, 0}, {
		"3.5", desc_case_3_5, {
		&preamble_0, &test_case_3_5_top, &postamble_0}, {
	&preamble_0, &test_case_3_5_bot, &postamble_0}, 0, 0}, {
		"3.6", desc_case_3_6, {
		&preamble_0, &test_case_3_6_top, &postamble_0}, {
	&preamble_0, &test_case_3_6_bot, &postamble_0}, 0, 0}, {
		"3.7", desc_case_3_7, {
		&preamble_0, &test_case_3_7_top, &postamble_0}, {
	&preamble_0, &test_case_3_7_bot, &postamble_0}, 0, 0}, {
		"3.8", desc_case_3_8, {
		&preamble_0, &test_case_3_8_top, &postamble_0}, {
	&preamble_0, &test_case_3_8_bot, &postamble_0}, 0, 0}, {
		"4.1.1", desc_case_4_1_1, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_1_bot, &postamble_0}, 0, 0}, {
		"4.1.2", desc_case_4_1_2, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_2_bot, &postamble_0}, 0, 0}, {
		"4.1.3", desc_case_4_1_3, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_3_bot, &postamble_0}, 0, 0}, {
		"4.1.4", desc_case_4_1_4, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_4_bot, &postamble_0}, 0, 0}, {
		"4.1.5", desc_case_4_1_5, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_5_bot, &postamble_0}, 0, 0}, {
		"4.1.6", desc_case_4_1_6, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_6_bot, &postamble_0}, 0, 0}, {
		"4.1.7", desc_case_4_1_7, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_7_bot, &postamble_0}, 0, 0}, {
		"4.1.8", desc_case_4_1_8, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_8_bot, &postamble_0}, 0, 0}, {
		"4.1.9", desc_case_4_1_9, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_9_bot, &postamble_0}, 0, 0}, {
		"4.1.10", desc_case_4_1_10, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_10_bot, &postamble_0}, 0, 0}, {
		"4.1.11", desc_case_4_1_11, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_11_bot, &postamble_0}, 0, 0}, {
		"4.1.12", desc_case_4_1_12, {
		&preamble_0, &test_case_4_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_1_12_bot, &postamble_0}, 0, 0}, {
		"4.2.1", desc_case_4_2_1, {
		&preamble_0, &test_case_4_2_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.2", desc_case_4_2_2, {
		&preamble_0, &test_case_4_2_2_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.3", desc_case_4_2_3, {
		&preamble_0, &test_case_4_2_3_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.4", desc_case_4_2_4, {
		&preamble_0, &test_case_4_2_4_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.5", desc_case_4_2_5, {
		&preamble_0, &test_case_4_2_5_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.6", desc_case_4_2_6, {
		&preamble_0, &test_case_4_2_6_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.7", desc_case_4_2_7, {
		&preamble_0, &test_case_4_2_7_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.8", desc_case_4_2_8, {
		&preamble_0, &test_case_4_2_8_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.9", desc_case_4_2_9, {
		&preamble_0, &test_case_4_2_9_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.10", desc_case_4_2_10, {
		&preamble_0, &test_case_4_2_10_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.11", desc_case_4_2_11, {
		&preamble_0, &test_case_4_2_11_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.12", desc_case_4_2_12, {
		&preamble_0, &test_case_4_2_12_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.13", desc_case_4_2_13, {
		&preamble_0, &test_case_4_2_13_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.2.14", desc_case_4_2_14, {
		&preamble_0, &test_case_4_2_14_top, &postamble_0}, {
	&preamble_0, &test_case_4_2_bot, &postamble_0}, 0, 0}, {
		"4.3.1", desc_case_4_3_1, {
		&preamble_0, &test_case_4_3_1_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.2", desc_case_4_3_2, {
		&preamble_0, &test_case_4_3_2_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.3", desc_case_4_3_3, {
		&preamble_0, &test_case_4_3_3_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.4", desc_case_4_3_4, {
		&preamble_0, &test_case_4_3_4_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.5", desc_case_4_3_5, {
		&preamble_0, &test_case_4_3_5_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.6", desc_case_4_3_6, {
		&preamble_0, &test_case_4_3_6_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.7", desc_case_4_3_7, {
		&preamble_0, &test_case_4_3_7_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.8", desc_case_4_3_8, {
		&preamble_0, &test_case_4_3_8_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.9", desc_case_4_3_9, {
		&preamble_0, &test_case_4_3_9_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.10", desc_case_4_3_10, {
		&preamble_0, &test_case_4_3_10_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
		"4.3.11", desc_case_4_3_11, {
		&preamble_0, &test_case_4_3_11_top, &postamble_0}, {
	&preamble_0, &test_case_4_3_bot, &postamble_0}, 0, 0}, {
NULL,}};

static int summary = 0;

int
do_tests(void)
{
	int i;
	int result = INCONCLUSIVE;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	int num_exit;
	if (verbose) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "\n\nXNS 5.2 - OpenSS7 XTI/TLI Library - Conformance Test Program.\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (begin_tests() == SUCCESS) {
		end_tests();
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (tests[i].result)
				continue;
			if (!tests[i].run) {
				tests[i].result = INCONCLUSIVE;
				continue;
			}
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "\nTest Case TIRDWR:%s:\n%s\n", tests[i].numb, tests[i].name);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((result = begin_tests()) != SUCCESS)
				goto inconclusive;
			switch ((result = test_run(&tests[i].top, &tests[i].bot))) {
			case SUCCESS:
				successes++;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "*********\n");
					fprintf(stdout, "********* Test Case SUCCESSFUL\n");
					fprintf(stdout, "*********\n\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case FAILURE:
				failures++;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case FAILED\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			default:
			case INCONCLUSIVE:
			      inconclusive:
				inconclusive++;
				if (verbose) {
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
			end_tests();
		}
		if (summary && verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
			for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
				if (tests[i].run) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "Test Case TIRDWR:%-10s ", tests[i].numb);
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
					switch (tests[i].result) {
					case SUCCESS:
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "SUCCESS\n");
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case FAILURE:
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "FAILURE\n");
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
						break;
					default:
					case INCONCLUSIVE:
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "INCONCLUSIVE\n");
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
						break;
					}
				}
			}
		}
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "Done.\n\n");
			fprintf(stdout, "========= %2d successes   \n", successes);
			fprintf(stdout, "========= %2d failures    \n", failures);
			fprintf(stdout, "========= %2d inconclusive\n", inconclusive);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (0);
	} else {
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "Test setup failed!\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		end_tests();
		return (2);
	}
}

void
splash(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
XNS 5.2 - OpenSS7 XTI/TLI Library - Conformance Test Suite\n\
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

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2003-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
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
", argv[0]);
}

void
help(int argc, char *argv[])
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
", argv[0]);
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
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "f::so:t:mvhqV?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "f::so:t:mvhqV?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
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
		if (verbose) {
			fprintf(stderr, "%s: error: no tests to run\n", argv[0]);
			fflush(stderr);
		}
		exit(2);
	}
	splash(argc, argv);
	do_tests();
	exit(0);
}
