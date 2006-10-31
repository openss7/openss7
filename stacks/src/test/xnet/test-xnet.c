/*****************************************************************************

 @(#) $RCSfile: test-xnet.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/10/31 21:04:56 $

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

 Last Modified $Date: 2006/10/31 21:04:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-xnet.c,v $
 Revision 0.9.2.10  2006/10/31 21:04:56  brian
 - changes for 32-bit compatibility and remove HZ dependency

 Revision 0.9.2.9  2006/07/08 07:25:46  brian
 - removed trigraphs

 Revision 0.9.2.8  2006/03/03 12:06:14  brian
 - 32/64-bit compatibility

 Revision 0.9.2.7  2005/12/28 09:58:34  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.6  2005/05/14 08:31:40  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-xnet.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/10/31 21:04:56 $"

static char const ident[] = "$RCSfile: test-xnet.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2006/10/31 21:04:56 $";

/*
 *  This is a ferry-clip XTI/TLI conformance test program for testing the
 *  OpenSS7 XTI/TLI Library.
 *
 *  GENERAL OPERATION:
 *
 *  The test program opens STREAMS pipe and pushes the "timod" module onto one
 *  side of the pipe.
 *
 *  The test program exchanges TPI primitives with one side of the open pipe
 *  and performs XTI/TLI Library Calls on a file descriptor at the other end
 *  of the pipe.  The side with the XTI/TLI Library represents the XTI user;
 *  the other side, the test harness, as shown below:
 *
 *                               USER SPACE
 *                              TEST PROGRAM
 *   ___________________________________  _________________________________
 *  |                                   |            \   /  |              
 *  |                                   |             \ /   |              
 *  |                                   |              |    |              
 *  |                                   |              |    |              
 *  |             XTI/TLI               |              |    |              
 *  |             Library               |              |    |              
 *  |                                   |              |    |              
 *  |                                   |              |    |              
 *  |                                   |              |    |              
 *  |___________________________________|              |    |              
 *               \   /  |                              |    |              
 *                \ /   |                              |    |              
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
 *  This test arrangement results in a a ferry-clip around the XTI/TLI Library
 *  and TIMOD module, where TPI primitives are injected and removed beneath
 *  the module as well XTI/TLI Library calls peformed above the module.
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
static int show_data = 1;

static int last_event = 0;
static int last_errno = 0;
static int last_t_errno = 0;
static int last_prim = 0;
static int last_tevent = 0;
static int last_qlen = 2;
static int last_sequence = 1;
static int last_servtype = T_COTS_ORD;
static int last_provflag = T_SENDZERO | T_ORDRELDATA | T_XPG4_1;
static int last_tstate = TS_UNBND;

static int MORE_flag = 0;

static int top_fd = 0;
static int bot_fd = 0;

static int test_sndflags = 0; /* T_MORE | T_EXPEDITED | T_PUSH */
static int test_rcvflags = 0; /* T_MORE | T_EXPEDITED */
static int test_pflags = MSG_BAND; /* MSG_BAND | MSG_HIPRI */
static int test_pband = 0;
static int test_gflags = 0; /* MSG_BAND | MSG_HIPRI */
static int test_gband = 0;
static int test_bufsize = 256;
static int test_tidu = 256;

char test_opt[256], test_udata[256], test_locadd[256], test_remadd[256];
struct t_unitdata test_sndudata = {
	{256, 256, test_remadd},
	{256, 256, test_opt},
	{256, 256, test_udata}
};
struct t_unitdata test_rcvudata = {
	{256, 0, test_remadd},
	{256, 0, test_opt},
	{256, 0, test_udata}
};
struct t_uderr test_uderr = {
	{256, 0, test_remadd},
	{256, 0, test_opt},
	0
};
/* listen call used in t_listen */
struct t_call test_liscall = {
	{256, 0, test_remadd},
	{256, 0, test_opt},
	{256, 0, test_udata},
	1
};
/* accepted call used in t_accept */
struct t_call test_acccall = {
	{256, 256, test_locadd},
	{256, 256, test_opt},
	{256, 256, test_udata},
	1
};
/* send to call used in t_connect */
struct t_call test_sndcall = {
	{256, 256, test_remadd},
	{256, 256, test_opt},
	{256, 256, test_udata},
	0
};
/* receive call used in t_connect an t_rcvconnect */
struct t_call test_rcvcall = {
	{256, 0, test_remadd},
	{256, 0, test_opt},
	{256, 0, test_udata},
	0
};
struct t_call test_discall = {
	{256, 256, test_locadd},
	{256, 256, test_opt},
	{256, 256, test_udata},
	1
};
struct t_optmgmt test_req = {
	{256, 256, test_opt},
	T_NEGOTIATE
};
struct t_optmgmt test_ret = {
	{256, 0, test_opt},
	0
};
struct t_discon test_rcvdis = {
	{256, 0, test_udata},
	0
};
struct t_discon test_snddis = {
	{256, 256, test_udata},
	0, 1
};
struct t_bind test_bindreq = {
	{256, 256, test_locadd},
	2
};
struct t_bind test_bindret = {
	{256, 0, test_locadd},
	2
};
struct t_bind test_addrloc = {
	{256, 0, test_locadd},
	0
};
struct t_bind test_addrrem = {
	{256, 0, test_remadd},
	0
};
struct t_info test_info;
struct iovec test_iov[4] = {
	[0] = {test_udata, 64},
	[1] = {test_udata + 64, 64},
	[2] = {test_udata + 64 + 64, 64},
	[3] = {test_udata + 64 + 64 + 64, 64},
};
struct t_iovec test_t_iov[4] = {
	[0] = {test_udata, 64},
	[1] = {test_udata + 64, 64},
	[2] = {test_udata + 64 + 64, 64},
	[3] = {test_udata + 64 + 64 + 64, 64},
};
unsigned int test_iovcount = 4;

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
	__TEST_O_NONBLOCK, __TEST_O_BLOCK,
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
	__TEST_T_ACCEPT, __TEST_T_BIND, __TEST_T_CLOSE, __TEST_T_CONNECT,
	__TEST_T_GETINFO, __TEST_T_GETPROTADDR, __TEST_T_GETSTATE,
	__TEST_T_LISTEN, __TEST_T_LOOK, __TEST_T_OPTMGMT, __TEST_T_RCV,
	__TEST_T_RCVCONNECT, __TEST_T_RCVDIS, __TEST_T_RCVREL,
	__TEST_T_RCVRELDATA, __TEST_T_RCVUDATA, __TEST_T_RCVUDERR,
	__TEST_T_RCVV, __TEST_T_RCVVUDATA, __TEST_T_SND, __TEST_T_SNDDIS,
	__TEST_T_SNDREL, __TEST_T_SNDRELDATA, __TEST_T_SNDUDATA,
	__TEST_T_SNDV, __TEST_T_SNDVUDATA, __TEST_T_SYNC, __TEST_T_UNBIND,
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

#if 0 
/* *INDENT-OFF* */
static timer_range_t timer[tmax] = {
	{(15 * 1000),		(60 * 1000)},		/* T1 15-60 seconds */
	{(3 * 60 * 1000),	(3 * 60 * 1000)},	/* T2 3 minutes */
	{(2 * 60 * 1000),	(2 * 60 * 1000)},	/* T3 2 minutes */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T4 5-15 minutes */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T5 5-15 minutes */
	{(10 * 1000),		(32 * 1000)},		/* T6 10-32 seconds (specified in Q.118) */
	{(20 * 1000),		(30 * 1000)},		/* T7 20-30 seconds */
	{(10 * 1000),		(15 * 1000)},		/* T8 10-15 seconds */
	{(2 * 60 * 1000),	(4 * 60 * 1000)},	/* T9 2-4 minutes (specified in Q.118) */
	{(4 * 1000),		(6 * 1000)},		/* T10 4-6 seconds */
	{(15 * 1000),		(20 * 1000)},		/* T11 15-20 seconds */
	{(15 * 1000),		(60 * 1000)},		/* T12 15-60 seconds */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T13 5-15 minutes */
	{(15 * 1000),		(60 * 1000)},		/* T14 15-60 seconds */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T15 5-15 minutes */
	{(15 * 1000),		(60 * 1000)},		/* T16 15-60 seconds */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T17 5-15 minutes */
	{(15 * 1000),		(60 * 1000)},		/* T18 15-60 seconds */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T19 5-15 minutes */
	{(15 * 1000),		(60 * 1000)},		/* T20 15-60 seconds */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T21 5-15 minutes */
	{(15 * 1000),		(60 * 1000)},		/* T22 15-60 seconds */
	{(5 * 60 * 1000),	(15 * 60 * 1000)},	/* T23 5-15 minutes */
	{(1 * 1000),		(2 * 1000)},		/* T24 < 2 seconds */
	{(1 * 1000),		(10 * 1000)},		/* T25 1-10 seconds */
	{(1 * 60 * 1000),	(3 * 60 * 1000)},	/* T26 1-3 minutes */
	{(4 * 60 * 1000),	(4 * 60 * 1000)},	/* T27 4 minutes */
	{(10 * 1000),		(10 * 1000)},		/* T28 10 seconds */
	{(300),			(600)},			/* T29 300-600 milliseconds */
	{(5 * 1000),		(10 * 1000)},		/* T30 5-10 seconds */
	{(6 * 60 * 1000),	(7 * 60 * 1000)},	/* T31 > 6 minutes */
	{(3 * 1000),		(5 * 1000)},		/* T32 3-5 seconds */
	{(12 * 1000),		(15 * 1000)},		/* T33 12-15 seconds */
	{(12 * 1000),		(15 * 1000)},		/* T34 12-15 seconds */
	{(15 * 1000),		(20 * 1000)},		/* T35 15-20 seconds */
	{(15 * 1000),		(20 * 1000)},		/* T36 15-20 seconds */
	{(2 * 1000),		(4 * 1000)},		/* T37 2-4 seconds */
	{(15 * 1000),		(20 * 1000)}		/* T38 15-20 seconds */
};
/* *INDENT-ON* */
#endif

long test_start = 0;

static int state;

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
#endif

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
		return FAILURE;
	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
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
	int result;
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
	test_pflags = MSG_BAND;
	test_pband = 0;
	if (fd == top_fd) {
		switch (action) {
		case __TEST_PUSH:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--I_PUSH(timod)---->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (ioctl(top_fd, I_PUSH, "timod") < 0) {
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
				fprintf(stdout, "--I_POP(timod)----->|  |                               |                    [%d]\n", state);
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
		case __TEST_O_NONBLOCK:
		{
			long flags;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--fcntl(2)--------->|  | O_NONBLOCK                    |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((flags = fcntl(top_fd, F_GETFL)) == -1) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: fcntl F_GETFL failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return (FAILURE);
			}
			if (fcntl(top_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: fcntl F_SETFL failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return (FAILURE);
			}
			return (SUCCESS);
		}
		case __TEST_O_BLOCK:
		{
			long flags;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--fcntl(2)--------->|  | ~O_NONBLOCK                   |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((flags = fcntl(top_fd, F_GETFL)) == -1) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: fcntl F_GETFL failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return (FAILURE);
			}
			if (fcntl(top_fd, F_SETFL, flags & ~O_NONBLOCK) == -1) {
				last_errno = errno;
				if (verbose) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "***************ERROR: fcntl F_SETFL failed\n");
					fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				return (FAILURE);
			}
			return (SUCCESS);
		}
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
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--writev(2)-------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			test_iov[0].iov_base = dbuf;
			test_iov[0].iov_len = sprintf(test_iov[0].iov_base, "Writev test datum for vector 0.");
			test_iov[1].iov_base = dbuf + test_iov[0].iov_len;
			test_iov[1].iov_len = sprintf(test_iov[1].iov_base, "Writev test datum for vector 1.");
			test_iov[2].iov_base = dbuf + test_iov[1].iov_len;
			test_iov[2].iov_len = sprintf(test_iov[2].iov_base, "Writev test datum for vector 2.");
			test_iov[3].iov_base = dbuf + test_iov[2].iov_len;
			test_iov[3].iov_len = sprintf(test_iov[3].iov_base, "Writev test datum for vector 3.");
			if (writev(top_fd, test_iov, test_iovcount) < 0) {
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 1;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			p->conn_res.SEQ_number = last_sequence;
			test_pflags = MSG_BAND;
			test_pband = 0;
			break;
		case __TEST_DISCON_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_DISCON_REQ----->|  |  seq = %02d                     |                    [%d]\n", last_sequence, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->discon_req);
			data = NULL;
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = last_sequence;
			test_pflags = MSG_BAND;
			test_pband = 0;
			break;
		case __TEST_DATA_REQ:
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_DATA_REQ------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->data_req);
			p->data_req.PRIM_type = T_DATA_REQ;
			p->data_req.MORE_flag = MORE_flag;
			data->len = sprintf(dbuf, "Normal test message.");
			test_pflags = MSG_BAND;
			test_pband = 0;
			break;
		case __TEST_EXDATA_REQ:
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_EXDATA_REQ----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->exdata_req);
			p->exdata_req.PRIM_type = T_EXDATA_REQ;
			p->exdata_req.MORE_flag = MORE_flag;
			data->len = sprintf(dbuf, "Expedited test message.");
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_HIPRI;
			test_pband = 0;
			break;
		case __TEST_BIND_REQ:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--T_BIND_REQ------->|  |  qlen = %02d                    |                    [%d]\n", last_qlen, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->bind_req);
			p->bind_req.PRIM_type = T_BIND_REQ;
			p->bind_req.ADDR_length = 0;
			p->bind_req.ADDR_offset = 0;
			p->bind_req.CONIND_number = last_qlen;
			data = NULL;
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
			break;
		case __TEST_UNITDATA_REQ:
			if ((verbose && show_data) || verbose > 1) {
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_HIPRI;
			test_pband = 0;
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
			test_pflags = MSG_HIPRI;
			test_pband = 0;
			break;
		case __TEST_T_ACCEPT:
			test_acccall.sequence = last_sequence;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_accept(3)------>|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_accept(top_fd, top_fd, &test_acccall) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_BIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_bind(3)-------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			test_bindreq.qlen = last_qlen;
			if (t_bind(top_fd, &test_bindreq, &test_bindret) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_CLOSE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_close(3)------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_close(top_fd) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_CONNECT:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_connect(3)----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_connect(top_fd, &test_sndcall, &test_rcvcall) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_GETINFO:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_getinfo(3)----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_getinfo(top_fd, &test_info) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_GETPROTADDR:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_getprotaddr(3)->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_getprotaddr(fd, &test_addrloc, &test_addrrem) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_GETSTATE:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_getstate(3)---->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((last_tstate = t_getstate(fd)) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_LISTEN:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_listen(3)------>|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_listen(fd, &test_liscall) == -1)
				goto do_terror;
			last_sequence = test_liscall.sequence;
			return (SUCCESS);
		case __TEST_T_LOOK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_look(3)-------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((last_tevent = t_look(fd)) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_OPTMGMT:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_optmgmt(3)----->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_optmgmt(fd, &test_req, &test_ret) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_RCV:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcv(3)--------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((result = t_rcv(fd, test_udata, test_bufsize, &test_rcvflags)) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", result, test_rcvflags & T_MORE ? '+' : '$', test_rcvflags & T_EXPEDITED ?  '!' : ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_RCVCONNECT:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvconnect(3)-->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_rcvconnect(top_fd, &test_rcvcall) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", test_rcvcall.udata.len, ' ', ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_RCVDIS:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvdis(3)------>|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_rcvdis(fd, &test_rcvdis) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", test_rcvdis.udata.len, ' ', ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_RCVREL:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvrel(3)------>|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_rcvrel(fd) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", 0, ' ', ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_RCVRELDATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvreldata(3)-->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_rcvreldata(fd, &test_rcvdis) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", test_rcvdis.udata.len, ' ', ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_RCVUDATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvudata(3)---->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_rcvudata(fd, &test_rcvudata, &test_rcvflags) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", test_rcvudata.udata.len, ' ', ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_RCVUDERR:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvuderr(3)---->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_rcvuderr(fd, &test_uderr) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_RCVV:
			test_t_iov[0].iov_base = test_udata;
			test_t_iov[0].iov_len = 64;
			test_t_iov[1].iov_base = test_udata + 64;
			test_t_iov[1].iov_len = 64;
			test_t_iov[2].iov_base = test_udata + 64 + 64;
			test_t_iov[2].iov_len = 64;
			test_t_iov[3].iov_base = test_udata + 64 + 64 + 64;
			test_t_iov[3].iov_len = 64;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvv(3)-------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((result = t_rcvv(fd, test_t_iov, test_iovcount, &test_rcvflags)) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", result, test_rcvflags & T_MORE ? '+' : '$', test_rcvflags & T_EXPEDITED ?  '!' : ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_RCVVUDATA:
			test_t_iov[0].iov_base = test_udata;
			test_t_iov[0].iov_len = 64;
			test_t_iov[1].iov_base = test_udata + 64;
			test_t_iov[1].iov_len = 64;
			test_t_iov[2].iov_base = test_udata + 64 + 64;
			test_t_iov[2].iov_len = 64;
			test_t_iov[3].iov_base = test_udata + 64 + 64 + 64;
			test_t_iov[3].iov_len = 64;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_rcvvudata(3)--->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_rcvvudata(fd, &test_rcvudata, test_t_iov, test_iovcount, &test_rcvflags) == -1)
				goto do_terror;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "  %4d bytes%c%c      |  |                               |                    [%d]\n", test_rcvudata.udata.len, ' ', ' ', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			return (SUCCESS);
		case __TEST_T_SND:
		{
			int nbytes = sprintf(test_udata, "Test t_snd data for transmission.");
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_snd(3)--------->|  |  %4d bytes                   |                    [%d]\n", nbytes, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_snd(fd, test_udata, nbytes, test_sndflags) == -1)
				goto do_terror;
			return (SUCCESS);
		}
		case __TEST_T_SNDDIS:
			test_discall.sequence = last_sequence;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_snddis(3)------>|  |  %4d bytes                   |                    [%d]\n", test_discall.udata.len > 0 ?  test_discall.udata.len : 0, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_snddis(top_fd, &test_discall) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_SNDREL:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_sndrel(3)------>|  |  %4d bytes                   |                    [%d]\n", 0, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_sndrel(fd) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_SNDRELDATA:
			test_snddis.udata.len = sprintf(test_udata, "Test t_sndreldata data.");
			test_snddis.sequence = last_sequence;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_sndreldata(3)-->|  |  %4d bytes                   |                    [%d]\n", test_snddis.udata.len > 0 ?  test_snddis.udata.len : 0, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_sndreldata(fd, &test_snddis) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_SNDUDATA:
			test_sndudata.udata.len = sprintf(test_udata, "Test t_sndudata data.");
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_sndudata(3)---->|  |  %4d bytes                   |                    [%d]\n", test_sndudata.udata.len, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_sndudata(fd, &test_sndudata) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_SNDV:
			test_t_iov[0].iov_base = dbuf;
			test_t_iov[0].iov_len = sprintf(test_t_iov[0].iov_base, "Writev test datum for vector 0.");
			test_t_iov[1].iov_base = dbuf + test_t_iov[0].iov_len;
			test_t_iov[1].iov_len = sprintf(test_t_iov[1].iov_base, "Writev test datum for vector 1.");
			test_t_iov[2].iov_base = dbuf + test_t_iov[1].iov_len;
			test_t_iov[2].iov_len = sprintf(test_t_iov[2].iov_base, "Writev test datum for vector 2.");
			test_t_iov[3].iov_base = dbuf + test_t_iov[2].iov_len;
			test_t_iov[3].iov_len = sprintf(test_t_iov[3].iov_base, "Writev test datum for vector 3.");
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_sndv(3)-------->|  |  %2d x %4d bytes              |                    [%d]\n", test_iovcount, test_t_iov[0].iov_len, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_sndv(fd, test_t_iov, test_iovcount, test_sndflags) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_SNDVUDATA:
			test_t_iov[0].iov_base = dbuf;
			test_t_iov[0].iov_len = sprintf(test_t_iov[0].iov_base, "Writev test datum for vector 0.");
			test_t_iov[1].iov_base = dbuf + test_t_iov[0].iov_len;
			test_t_iov[1].iov_len = sprintf(test_t_iov[1].iov_base, "Writev test datum for vector 1.");
			test_t_iov[2].iov_base = dbuf + test_t_iov[1].iov_len;
			test_t_iov[2].iov_len = sprintf(test_t_iov[2].iov_base, "Writev test datum for vector 2.");
			test_t_iov[3].iov_base = dbuf + test_t_iov[2].iov_len;
			test_t_iov[3].iov_len = sprintf(test_t_iov[3].iov_base, "Writev test datum for vector 3.");
			test_sndudata.udata.len = 0;
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_sndvudata(3)--->|  |  %2d x %4d bytes              |                    [%d]\n", test_iovcount, test_t_iov[0].iov_len, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_sndvudata(fd, &test_sndudata, test_t_iov,
						test_iovcount) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_SYNC:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_sync(3)-------->|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_sync(top_fd) == -1)
				goto do_terror;
			return (SUCCESS);
		case __TEST_T_UNBIND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--t_unbind(3)------>|  |                               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (t_unbind(top_fd) == -1)
				goto do_terror;
			return (SUCCESS);
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
			p->bind_req.CONIND_number = last_qlen;
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
			p->bind_req.CONIND_number = last_qlen;
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
			p->bind_req.CONIND_number = last_qlen;
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
			p->conn_res.SEQ_number = last_sequence;
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
		case __TEST_TI_SETMYNAME_DISC:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETMYNAME                 |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->discon_req);
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = last_sequence;
			goto do_ioctl;
		case __TEST_TI_SETPEERNAME_DISC:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETPEERNAME               |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->discon_req);
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = last_sequence;
			goto do_ioctl;
		case __TEST_TI_SETMYNAME_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETMYNAME w/DATA          |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->conn_res) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->conn_res.PRIM_type = T_CONN_RES;
			p->conn_res.ACCEPTOR_id = 0;
			p->conn_res.OPT_length = 0;
			p->conn_res.OPT_offset = 0;
			p->conn_res.SEQ_number = last_sequence;
			goto do_ioctl;
		case __TEST_TI_SETPEERNAME_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETPEERNAME w/DATA        |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->conn_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->conn_req.PRIM_type = T_CONN_REQ;
			p->conn_req.DEST_length = 0;
			p->conn_req.DEST_offset = 0;
			p->conn_req.OPT_length = 0;
			p->conn_req.OPT_offset = 0;
			goto do_ioctl;
		case __TEST_TI_SETMYNAME_DISC_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETMYNAME w/DATA          |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETMYNAME;
			ic.ic_len = sizeof(p->discon_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = last_sequence;
			goto do_ioctl;
		case __TEST_TI_SETPEERNAME_DISC_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "--ioctl(2)--------->|  |  TI_SETPEERNAME w/DATA        |                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ic.ic_cmd = TI_SETPEERNAME;
			ic.ic_len = sizeof(p->discon_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
			p->discon_req.PRIM_type = T_DISCON_REQ;
			p->discon_req.SEQ_number = last_sequence;
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
		if (putpmsg(top_fd, ctrl, data, test_pband, test_pflags) < 0) {
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
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "ioctl from top: cmd=%d, timout=%d, len=%d, dp=%p\n", ic.ic_cmd, ic.ic_timout, ic.ic_len, ic.ic_dp);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = ioctl(top_fd, I_STR, &ic);
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "got ioctl from top: cmd=%d, timout=%d, len=%d, dp=%p\n", ic.ic_cmd, ic.ic_timout, ic.ic_len, ic.ic_dp);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		switch (result) {
		case 0:
			return SUCCESS;
		case -1:
			last_errno = errno;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "***************ERROR: ioctl failed\n");
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
				lockf(fileno(stdout), F_ULOCK, 0);
				fflush(stdout);
			}
			return FAILURE;
		}
		last_t_errno = result & 0x00ff;
		if (last_t_errno == TSYSERR)
			last_errno = (result >> 8) & 0x00ff;
		else
			last_errno = 0;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "***************ERROR: ioctl failed\n");
			if (verbose > 3)
				fprintf(stdout, "                    : %s; result = %d\n", __FUNCTION__, result);
			fprintf(stdout, "                    : %s; TPI error = %d\n", __FUNCTION__, last_t_errno);
			if (last_t_errno == TSYSERR)
				fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return FAILURE;
	      do_terror:
		last_errno = errno;
		last_t_errno = t_errno;
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "***************ERROR: XTI Library Call failed\n");
			if (last_t_errno == TSYSERR)
				fprintf(stdout, "                    : %s; UNIX: [%d] %s\n", __FUNCTION__, last_errno, strerror(last_errno));
			else
				fprintf(stdout, "                    : %s; XTI: [%d] %s\n", __FUNCTION__, last_t_errno, t_strerror(last_t_errno));
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (FAILURE);
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
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-writev(2)---------(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			test_iov[0].iov_base = dbuf;
			test_iov[0].iov_len = sprintf(test_iov[0].iov_base, "Writev test datum for vector 0.");
			test_iov[1].iov_base = dbuf + test_iov[0].iov_len;
			test_iov[1].iov_len = sprintf(test_iov[1].iov_base, "Writev test datum for vector 1.");
			test_iov[2].iov_base = dbuf + test_iov[1].iov_len;
			test_iov[2].iov_len = sprintf(test_iov[2].iov_base, "Writev test datum for vector 2.");
			test_iov[3].iov_base = dbuf + test_iov[2].iov_len;
			test_iov[3].iov_len = sprintf(test_iov[3].iov_base, "Writev test datum for vector 3.");
			if (writev(bot_fd, test_iov, test_iovcount) < 0) {
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
		case __TEST_PUTMSG_DATA:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< - - - - - - - - - - - - - - -|<-putmsg(2)---------(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl = NULL;
			data->len = sprintf(dbuf, "Putmsg test data.");
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 1;
			break;
		case __TEST_CONN_IND:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< seq = %02d- - - - - - - - - - -|<-T_CONN_IND------- (%d)\n", last_sequence, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->conn_ind);
			p->conn_ind.PRIM_type = T_CONN_IND;
			p->conn_ind.SRC_length = 0;
			p->conn_ind.SRC_offset = 0;
			p->conn_ind.OPT_length = 0;
			p->conn_ind.OPT_offset = 0;
			p->conn_ind.SEQ_number = last_sequence;
			data->len = sprintf(dbuf, "Connection indication test data.");
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			p->discon_ind.SEQ_number = last_sequence;
			data->len = sprintf(dbuf, "Disconnection indication test data.");
			test_pflags = MSG_BAND;
			test_pband = 0;
			break;
		case __TEST_DATA_IND:
			ctrl->len = sizeof(p->data_ind);
			p->data_ind.PRIM_type = T_DATA_IND;
			p->data_ind.MORE_flag = MORE_flag;
			data->len = sprintf(dbuf, "Normal test data.");
			test_pflags = MSG_BAND;
			test_pband = 0;
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< %4d bytes- - - - - - - - - -|<-T_DATA_IND%c------ (%d)\n", data->len, MORE_flag ? '+' : '-', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			break;
		case __TEST_EXDATA_IND:
			ctrl->len = sizeof(p->exdata_ind);
			p->data_ind.PRIM_type = T_EXDATA_IND;
			p->data_ind.MORE_flag = MORE_flag;
			data->len = sprintf(dbuf, "Expedited test data.");
			test_pflags = MSG_BAND;
			test_pband = 1;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< %4d bytes- - - - - - - - - -|<-T_EXDATA_IND%c---- (%d)\n", data->len, MORE_flag ? '+' : '-', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
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
			p->info_ack.TSDU_size = test_bufsize;
			p->info_ack.ETSDU_size = test_bufsize;
			p->info_ack.CDATA_size = test_bufsize;
			p->info_ack.DDATA_size = test_bufsize;
			p->info_ack.ADDR_size = test_bufsize;
			p->info_ack.OPT_size = test_bufsize;
			p->info_ack.TIDU_size = test_tidu;
			p->info_ack.SERV_type = last_servtype;
			p->info_ack.CURRENT_state = last_tstate;
			p->info_ack.PROVIDER_flag = last_provflag;
			data = NULL;
			test_pflags = MSG_HIPRI;
			test_pband = 0;
			break;
		case __TEST_BIND_ACK:
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |< qlen = %02d - - - - - - - - - -|<-T_BIND_ACK------- (%d)\n", last_qlen, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ctrl->len = sizeof(p->bind_ack);
			p->bind_ack.PRIM_type = T_BIND_ACK;
			p->bind_ack.ADDR_length = 0;
			p->bind_ack.ADDR_offset = 0;
			p->bind_ack.CONIND_number = last_qlen;
			data = NULL;
			test_pflags = MSG_HIPRI;
			test_pband = 0;
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
			p->error_ack.ERROR_prim = last_prim;
			p->error_ack.TLI_error = last_t_errno;
			p->error_ack.UNIX_error = last_errno;
			data = NULL;
			test_pflags = MSG_HIPRI;
			test_pband = 0;
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
			test_pflags = MSG_HIPRI;
			test_pband = 0;
			break;
		case __TEST_UNITDATA_IND:
			if ((verbose && show_data) || verbose > 1) {
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_HIPRI;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_BAND;
			test_pband = 1;
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
			test_pflags = MSG_BAND;
			test_pband = 0;
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
			test_pflags = MSG_HIPRI;
			test_pband = 0;
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
			p->capability_ack.INFO_ack.TSDU_size = test_bufsize;
			p->capability_ack.INFO_ack.ETSDU_size = test_bufsize;
			p->capability_ack.INFO_ack.CDATA_size = test_bufsize;
			p->capability_ack.INFO_ack.DDATA_size = test_bufsize;
			p->capability_ack.INFO_ack.ADDR_size = test_bufsize;
			p->capability_ack.INFO_ack.OPT_size = test_bufsize;
			p->capability_ack.INFO_ack.TIDU_size = test_tidu;
			p->capability_ack.INFO_ack.SERV_type = last_servtype;
			p->capability_ack.INFO_ack.CURRENT_state = last_tstate;
			p->capability_ack.INFO_ack.PROVIDER_flag = last_provflag;
			p->capability_ack.ACCEPTOR_id = 0;
			data = NULL;
			test_pflags = MSG_HIPRI;
			test_pband = 0;
			break;
		default:
			return SCRIPTERROR;
		}
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putpmsg to bot: [%d,%d]\n", ctrl ? ctrl->len : -1, data ? data->len : -1);
			lockf(fileno(stdout), F_ULOCK, 0);
			fflush(stdout);
		}
		if (putpmsg(bot_fd, ctrl, data, test_pband, test_pflags) < 0) {
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

#if 0 
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
#endif

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
		if ((verbose && show_data) || verbose > 1) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-DATA--------------|  |  %4d bytes                   |                    [%d]\n", data->len, state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (__TEST_DATA);
	}
	if (fd == bot_fd) {
		if ((verbose && show_data) || verbose > 1) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- %4d bytes- - - - - - - - - >|--DATA------------> (%d)\n", data->len, state);
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
		switch ((last_prim = p->type)) {
		case T_CONN_IND:
			last_sequence = p->conn_ind.SEQ_number;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_CONN_IND--------|  |  seq = %02d                     |                    [%d]\n", last_sequence, state);
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
			if ((verbose && show_data) || verbose > 1) {
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
			last_qlen = p->bind_ack.CONIND_number;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "<-T_BIND_ACK--------|  |  qlen = %02d                        |                    [%d]\n", last_qlen, state);
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
			if ((verbose && show_data) || verbose > 1) {
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
				fprintf(stdout, "<-T_????_??? -------|  |                               |                    [%d]\n", state);
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
		switch ((last_prim = p->type)) {
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
			last_sequence = p->discon_req.SEQ_number;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- seq = %02d- - - - - - - - - - >|--T_DISCON_REQ----> (%d)\n", last_sequence, state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_DISCON_REQ;
			break;
		case T_DATA_REQ:
			MORE_flag = p->data_req.MORE_flag;
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_DATA_REQ%c-----> (%d)\n", MORE_flag ? '+' : '-', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_DATA_REQ;
			break;
		case T_EXDATA_REQ:
			MORE_flag = p->data_req.MORE_flag;
			if ((verbose && show_data) || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_EXDATA_REQ%c---> (%d)\n", MORE_flag ? '+' : '-', state);
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
			last_qlen = p->bind_req.CONIND_number;
			if (verbose) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- qlen = %02d - - - - - - - - - >|--T_BIND_REQ------> (%d)\n", last_qlen, state);
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
			if ((verbose && show_data) || verbose > 1) {
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
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_????_??? -----> (%d)\n", state);
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
	int ret = FAILURE;
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
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-readv(2)----------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = readv(top_fd, test_iov, test_iovcount)) < 0) {
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
	case __TEST_GETMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-getmsg(2)---------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getmsg(top_fd, NULL, &data, &test_gflags)) < 0) {
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
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-getpmsg(2)--------|  |                               |                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getpmsg(top_fd, NULL, &data, &test_gband, &test_gflags)) < 0) {
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
	int ret = FAILURE;
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
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--readv(2)--------->(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = readv(bot_fd, test_iov, test_iovcount)) < 0) {
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
	case __TEST_GETMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--getmsg(2)-------->(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getmsg(bot_fd, NULL, &data, &test_gflags)) < 0) {
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
		if (verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--getpmsg(2)------->(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		if ((ret = getpmsg(bot_fd, NULL, &data, &test_gband, &test_gflags)) < 0) {
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
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "polling:\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
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
				if (verbose > 3) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "getmsg from top:\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				if (getmsg(top_fd, &ctrl, &data, &flags) == 0) {
					if (verbose > 3) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from top [%d,%d]:\n", ctrl.len, data.len);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
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
				if (verbose > 3) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "getmsg from bot:\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				if (getmsg(bot_fd, &ctrl, &data, &flags) == 0) {
					if (verbose > 3) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from bot [%d,%d,%d]:\n", ctrl.len, data.len, flags);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
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
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "polling top:\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
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
				if (verbose > 3) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "getmsg from top:\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				if (getmsg(top_fd, &ctrl, &data, &flags) == 0) {
					if (verbose > 3) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from top [%d,%d]:\n", ctrl.len, data.len);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
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
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "polling bot:\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
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
				if (verbose > 3) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "getmsg from bot:\n");
					fflush(stdout);
					lockf(fileno(stdout), F_ULOCK, 0);
				}
				if (getmsg(bot_fd, &ctrl, &data, &flags) == 0) {
					if (verbose > 3) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from bot [%d,%d,%d]:\n", ctrl.len, data.len, flags);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
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
	state = 1;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SYNC) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
postamble_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (do_signal(fd, __TEST_T_CLOSE) != SUCCESS)
		return (FAILURE);
	state = 2;
	stop_tt();
	state = 3;
	return (SUCCESS);
}
static int
preamble_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CAPABILITY_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_CAPABILITY_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
postamble_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) == TIMEOUT) {
		state = 2;
		return (SUCCESS);
	}
	state = 3;
	switch (last_event) {
	case __TEST_DISCON_REQ:
		stop_tt();
		state = 4;
		return SUCCESS;
	case NO_MSG:
		state = 5;
		return SUCCESS;
	}
	state = 6;
	stop_tt();
	state = 7;
	return (FAILURE);
}

static int
preamble_1cl_top(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return preamble_1_top(fd);
}
static int
postamble_1cl_top(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return postamble_1_top(fd);
}
static int
preamble_1cl_bot(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return preamble_1_bot(fd);
}
static int
postamble_1cl_bot(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return postamble_1_bot(fd);
}

static int
preamble_2_top(int fd)
{
	state = 0;
	start_tt(1000);
	state = 1;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SYNC) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_BIND) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}
static int
postamble_2_top(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (do_signal(fd, __TEST_T_UNBIND) != SUCCESS)
		return (FAILURE);
	state = 2;
	start_tt(500);
	state = 3;
	if (do_signal(fd, __TEST_T_CLOSE) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}
static int
preamble_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CAPABILITY_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_CAPABILITY_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_BIND_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}
static int
postamble_2_bot(int fd)
{
	state = 0;
	start_tt(2000);
	state = 1;
	if (get_event(fd) != __TEST_UNBIND_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) == TIMEOUT) {
		state = 5;
		return (SUCCESS);
	}
	state = 6;
	switch (last_event) {
	case __TEST_DISCON_REQ:
		state = 7;
		return SUCCESS;
	case NO_MSG:
		state = 8;
		return SUCCESS;
	}
	state = 9;
	return (FAILURE);
}

static int
preamble_2cl_top(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return preamble_2_top(fd);
}
static int
postamble_2cl_top(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return postamble_2_top(fd);
}
static int
preamble_2cl_bot(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return preamble_2_bot(fd);
}
static int
postamble_2cl_bot(int fd)
{
	last_qlen = 0;
	last_servtype = T_CLTS;
	return postamble_2_bot(fd);
}

static int
preamble_3_top(int fd)
{
	state = 0;
	start_tt(1000);
	state = 1;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SYNC) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_BIND) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_CONNECT) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
postamble_3_top(int fd)
{
	state = 0;
	start_tt(1000);
	state = 1;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_UNBIND) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_CLOSE) != SUCCESS)
		return (FAILURE);
	state = 4;
	stop_tt();
	state = 5;
	return (SUCCESS);
}
static int
preamble_3_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CAPABILITY_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_CAPABILITY_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_BIND_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_CONN_CON) != SUCCESS)
		return (FAILURE);
	state = 10;
	return (SUCCESS);
}
static int
postamble_3_bot(int fd)
{
	state = 0;
	start_tt(2000);
	state = 1;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != __TEST_UNBIND_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) == TIMEOUT) {
		state = 8;
		return (SUCCESS);
	}
	state = 9;
	switch (last_event) {
	case __TEST_DISCON_REQ:
		stop_tt();
		state = 10;
		return SUCCESS;
	case NO_MSG:
		state = 11;
		return SUCCESS;
	}
	state = 12;
	stop_tt();
	state = 13;
	return (FAILURE);
}

static int
preamble_3s_top(int fd)
{
	test_tidu = 13;
	return preamble_3_top(fd);
}
static int
postamble_3s_top(int fd)
{
	test_tidu = test_bufsize;
	return postamble_3_top(fd);
}
static int
preamble_3s_bot(int fd)
{
	test_tidu = 13;
	return preamble_3_bot(fd);
}
static int
postamble_3s_bot(int fd)
{
	test_tidu = test_bufsize;
	return postamble_3_bot(fd);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test Cases
 *
 *  -------------------------------------------------------------------------
 */

#define desc_case_1_1 "\
Pushing and popping the timod module\n\
-- normal push and pop sequence\n\
This test case tests the simple pushing and popping of the timod module.  It\n\
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
	start_tt(200);
	state = 1;
	while (get_event(fd) == NO_MSG) ;
	state = 2;
	switch (last_event) {
	case __TEST_DISCON_REQ:
	case TIMEOUT:
		return (SUCCESS);
	}
	state = 3;
	return (FAILURE);
}

#define desc_case_1_2_1 "\
Syncrhonizing the library to a file descriptor\n\
-- syncrhonization of a file descriptor after \"timod\" has been pushed\n\
-- normal operation\n\
This test case tests syncrhonization of the XTI library to a file descriptor\n\
on the test harness upon which the \"timod\" module has been pushed."
static int
test_case_1_2_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_SYNC) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_CLOSE) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_1_2_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CAPABILITY_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_CAPABILITY_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_1_2_2 "\
Synchronizing the library to a file descriptor\n\
-- syncrhonization of a file descriptor after \"timod\" has been pushed\n\
-- closed file descriptor\n\
This test case tests synchronization of the XTI library to a file descriptor\n\
that is closed."
static int
test_case_1_2_2_top(int fd)
{
	state = 0;
	close(5);
	state = 1;
	top_fd = 5;
	if (do_signal(5, __TEST_T_SYNC) == SUCCESS || last_t_errno != TBADF)
		return (FAILURE);
	top_fd = fd;
	state = 2;
	return (SUCCESS);
}
static int
test_case_1_2_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}

#define desc_case_1_2_3 "\
Synchronizing the library to a file descriptor\n\
-- synchronization of a file descriptor after \"timod\" has been pushed\n\
-- system error\n\
This test case tests synchronization of the XTI library to a file descriptor\n\
on the test harness with a system error (ENOMEM)."
static int
test_case_1_2_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_SYNC) == SUCCESS || last_t_errno != TSYSERR || last_errno != ENOMEM)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_1_2_3_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CAPABILITY_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = TSYSERR;
	last_errno = ENOMEM;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_2_1_1 "\
Binding to an address\n\
-- successful bind operation\n\
This test case tests successful bind of the XTI stream."
static int
test_case_2_1_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_BIND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}
static int
test_case_2_1_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_BIND_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}

#define desc_case_2_1_2 "\
Binding to an address\n\
-- unsuccessful bind operation\n\
-- TACCES error\n\
This test case test unsuccessful bind of the XTI stream."
static int
test_case_2_1_x_top(int fd, int terror)
{
	state = 0;
	if (do_signal(fd, __TEST_T_BIND) == SUCCESS || last_t_errno != terror)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}
static int
test_case_2_1_x_bot(int fd, int terror)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = terror;
	last_errno = 0;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_2_1_2_top(int fd)
{
	return test_case_2_1_x_top(fd, TACCES);
}
static int
test_case_2_1_2_bot(int fd)
{
	return test_case_2_1_x_bot(fd, TACCES);
}

#define desc_case_2_1_3 "\
Binding to an address\n\
-- unsuccessful bind operation\n\
-- TADDRBUSY error\n\
This test case test unsuccessful bind of the XTI stream."
static int
test_case_2_1_3_top(int fd)
{
	return test_case_2_1_x_top(fd, TADDRBUSY);
}
static int
test_case_2_1_3_bot(int fd)
{
	return test_case_2_1_x_bot(fd, TADDRBUSY);
}

#define desc_case_2_1_4 "\
Binding to an address\n\
-- unsuccessful bind operation\n\
-- TBADADDR error\n\
This test case test unsuccessful bind of the XTI stream."
static int
test_case_2_1_4_top(int fd)
{
	return test_case_2_1_x_top(fd, TBADADDR);
}
static int
test_case_2_1_4_bot(int fd)
{
	return test_case_2_1_x_bot(fd, TBADADDR);
}

#define desc_case_2_1_5 "\
Binding to an address\n\
-- unsuccessful bind operation\n\
-- TOUTSTATE error\n\
This test case test unsuccessful bind of the XTI stream."
static int
test_case_2_1_5_top(int fd)
{
	return test_case_2_1_x_top(fd, TOUTSTATE);
}
static int
test_case_2_1_5_bot(int fd)
{
	return test_case_2_1_x_bot(fd, TOUTSTATE);
}

#define desc_case_2_1_6 "\
Binding to an address\n\
-- unsuccessful bind operation\n\
-- TNOADDR error\n\
This test case test unsuccessful bind of the XTI stream."
static int
test_case_2_1_6_top(int fd)
{
	return test_case_2_1_x_top(fd, TNOADDR);
}
static int
test_case_2_1_6_bot(int fd)
{
	return test_case_2_1_x_bot(fd, TNOADDR);
}

#define desc_case_2_1_7 "\
Binding to an address\n\
-- unsuccessful bind operation\n\
-- TOUTSTATE error\n\
This test case test unsuccessful bind of the XTI stream."
static int
test_case_2_1_7_top(int fd)
{
	return test_case_2_1_x_top(fd, TOUTSTATE);
}
static int
test_case_2_1_7_bot(int fd)
{
	return test_case_2_1_x_bot(fd, TOUTSTATE);
}

#define desc_case_2_1_8 "\
Binding to an address\n\
-- unsuccessful bind operation\n\
-- double bind error\n\
This test case tests unsuccessful bind of the XTI stream."
static int
test_case_2_1_8_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_BIND) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_BIND) == SUCCESS || last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_UNBIND) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_2_1_8_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_BIND_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != __TEST_UNBIND_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}

#define desc_case_2_2 "\
Binding to an address\n\
-- successful bind and unbind operation\n\
This test case tests successful bind and unbind of the XTI stream."
static int
test_case_2_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_BIND) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_UNBIND) != SUCCESS)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_2_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_BIND_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != __TEST_UNBIND_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}

#define desc_case_2_3_1 "\
Connecting to an address\n\
-- successful connect and disconnect operation\n\
-- immediate confirmation, synchronous mode\n\
This test case tests successful connect and disconnect of the XTI stream."
static int
test_case_2_3_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_CONNECT) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_2_3_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_CONN_CON) != SUCCESS)
		return (FAILURE);
	state = 4;
	start_tt(200);
	state = 5;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 7;
	return (SUCCESS);
}

#define desc_case_2_3_2 "\
Connecting to an address\n\
-- successful connect and disconnect operation\n\
-- delayed confirmation, synchronous mode\n\
This test case tests successful connect and disconnect of the XTI stream."
static int
test_case_2_3_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_CONNECT) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_2_3_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(500);
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_CONN_CON) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}

#define desc_case_2_3_3 "\
Connecting to an address\n\
-- successful connect and disconnect operation\n\
-- delayed confirmation, asynchronous mode\n\
This test case tests successful connect and disconnect of the XTI stream."
static int
test_case_2_3_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS || last_t_errno != TNODATA)
		return (FAILURE);
	state = 2;
	start_tt(600);
	state = 3;
	pause();
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_RCVCONNECT) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 7;
	return (SUCCESS);
}
static int
test_case_2_3_3_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(500);
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_CONN_CON) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}

#define desc_case_2_3_4 "\
Connecting to an address\n\
-- unsuccessful connect operation\n\
-- TACCES error\n\
This test case test unsuccessful connection of the XTI stream with a\n\
TACCES error."
static int
test_case_2_3_x_top(int fd, int terror)
{
	state = 0;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS || last_t_errno != terror)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}
static int
test_case_2_3_x_bot(int fd, int terror)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = terror;
	last_errno = 0;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_2_3_4_top(int fd)
{
	return test_case_2_3_x_top(fd, TACCES);
}
static int
test_case_2_3_4_bot(int fd)
{
	return test_case_2_3_x_bot(fd, TACCES);
}

#define desc_case_2_3_5 "\
Connecting to an address\n\
-- unsuccessful connect operation\n\
-- TADDRBUSY error\n\
This test case test unsuccessful connection of the XTI stream with a\n\
TADDRBUSY error."
static int
test_case_2_3_5_top(int fd)
{
	return test_case_2_3_x_top(fd, TADDRBUSY);
}
static int
test_case_2_3_5_bot(int fd)
{
	return test_case_2_3_x_bot(fd, TADDRBUSY);
}

#define desc_case_2_3_6 "\
Connecting to an address\n\
-- unsuccessful connect operation\n\
-- TBADADDR error\n\
This test case test unsuccessful connection of the XTI stream with a\n\
TBADADDR error."
static int
test_case_2_3_6_top(int fd)
{
	return test_case_2_3_x_top(fd, TBADADDR);
}
static int
test_case_2_3_6_bot(int fd)
{
	return test_case_2_3_x_bot(fd, TBADADDR);
}

#define desc_case_2_3_7 "\
Connecting to an address\n\
-- unsuccessful connect operation\n\
-- TBADOPT error\n\
This test case test unsuccessful connection of the XTI stream with a\n\
TBADOPT error."
static int
test_case_2_3_7_top(int fd)
{
	return test_case_2_3_x_top(fd, TBADOPT);
}
static int
test_case_2_3_7_bot(int fd)
{
	return test_case_2_3_x_bot(fd, TBADOPT);
}

#define desc_case_2_3_8 "\
Connecting to an address\n\
-- unsuccessful connect operation\n\
-- TNOTSUPPORT error\n\
This test case test unsuccessful connection of the XTI stream with a\n\
TNOTSUPPORT error."
static int
test_case_2_3_8_top(int fd)
{
	return test_case_2_3_x_top(fd, TNOTSUPPORT);
}
static int
test_case_2_3_8_bot(int fd)
{
	return test_case_2_3_x_bot(fd, TNOTSUPPORT);
}

#define desc_case_2_3_9 "\
Connecting to an address\n\
-- unsuccessful connect operation\n\
-- TOUTSTATE error\n\
This test case test unsuccessful connection of the XTI stream with a\n\
TOUTSTATE error."
static int
test_case_2_3_9_top(int fd)
{
	return test_case_2_3_x_top(fd, TOUTSTATE);
}
static int
test_case_2_3_9_bot(int fd)
{
	return test_case_2_3_x_bot(fd, TOUTSTATE);
}

#define desc_case_2_4_1 "\
Connection from an address\n\
-- successful connection and disconnection operation\n\
-- synchronous mode\n\
This test case tests successful incoming connection and disconnection of\n\
the XTI stream."
static int
test_case_2_4_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_2_4_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_2_5_1 "\
Connection collision\n\
-- incoming connection before outgoing\n\
This test case tests unsuccessful outgoing connection due to incoming\n\
connection indication on an XTI stream."
static int
test_case_2_5_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	start_tt(500);
	state = 4;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (FAILURE);
	state = 5;
	switch (last_t_errno) {
	case TLOOK:
	case TOUTSTATE:
		break;
	default:
		state = 6;
		return (FAILURE);
	}
	state = 7;
	start_tt(500);
	state = 8;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 10;
	return (SUCCESS);
}
static int
test_case_2_5_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	for (;;) {
		switch (get_event(fd)) {
		case __TEST_DISCON_REQ:
			state = 3;
			if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
				return (FAILURE);
			break;
		case __TEST_CONN_REQ:
			state = 4;
			last_t_errno = TOUTSTATE;
			last_errno = 0;
			if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
				return (FAILURE);
			continue;
		default:
			state = 5;
			return (FAILURE);
		}
		break;
	}
	state = 6;
	return (SUCCESS);
}

#define desc_case_3_1_1 "\
t_look response to events\n\
-- T_CONN_IND event\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_CONN_IND TPI event."
static int
test_case_3_1_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_LISTEN) == SUCCESS)
		return (INCONCLUSIVE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_LISTEN)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 10;
	return (SUCCESS);
}
static int
test_case_3_1_1_bot(int fd)
{
	state = 0;
	start_tt(50);
	state = 1;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_CONN_IND) != 0)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}

#define desc_case_3_2_1 "\
t_look response to events\n\
-- T_CONN_CON event\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_CONN_CON TPI event."
static int
test_case_3_2_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (INCONCLUSIVE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	start_tt(300);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_CONNECT)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_RCVCONNECT) != SUCCESS)
		return (FAILURE);
	state = 9;
	last_sequence = 0;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 10;
	return (SUCCESS);
}
static int
test_case_3_2_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_CONN_CON) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}

#define desc_case_3_3_1 "\
t_look response to events\n\
-- T_DISCON_IND event\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_DISCON_IND TPI event."
static int
test_case_3_3_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (INCONCLUSIVE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	start_tt(300);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}
static int
test_case_3_3_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	last_sequence = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}

#define desc_case_3_4_1 "\
t_look response to events\n\
-- T_ORDREL_IND event\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_ORDREL_IND TPI event."
static int
test_case_3_4_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_CONNECT) != SUCCESS) {
		state = 2;
		if (last_t_errno != TNODATA)
			return (FAILURE);
		state = 3;
		start_tt(200);
		state = 4;
		pause();
		state = 5;
		if (get_event(fd) != TIMEOUT)
			return (FAILURE);
		state = 6;
		if (do_signal(fd, __TEST_T_RCVCONNECT) != SUCCESS)
			return (FAILURE);
	}
	state = 7;
	start_tt(200);
	state = 8;
	pause();
	state = 9;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 11;
	if (last_tevent != T_ORDREL)
		return (FAILURE);
	state = 12;
	if (do_signal(fd, __TEST_T_RCVREL) != SUCCESS)
		return (FAILURE);
	state = 13;
	if (do_signal(fd, __TEST_T_SNDREL) != SUCCESS)
		return (FAILURE);
	state = 14;
	return (SUCCESS);
}
static int
test_case_3_4_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_CONN_CON) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	last_sequence = 0;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) != __TEST_ORDREL_REQ)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}

#define desc_case_3_4_2 "\
t_look response to events\n\
-- T_ORDREL_IND w/ DATA event\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_ORDREL_IND w/ DATA TPI event."
static int
test_case_3_4_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_CONNECT) != SUCCESS) {
		state = 2;
		if (last_t_errno != TNODATA)
			return (FAILURE);
		state = 3;
		start_tt(200);
		state = 4;
		pause();
		state = 5;
		if (get_event(fd) != TIMEOUT)
			return (FAILURE);
		state = 6;
		if (do_signal(fd, __TEST_T_RCVCONNECT) != SUCCESS)
			return (FAILURE);
	}
	state = 7;
	start_tt(200);
	state = 8;
	pause();
	state = 9;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 11;
	if (last_tevent != T_ORDREL)
		return (FAILURE);
	state = 12;
	if (do_signal(fd, __TEST_T_RCVRELDATA) != SUCCESS)
		return (FAILURE);
	state = 13;
	if (do_signal(fd, __TEST_T_SNDRELDATA) != SUCCESS)
		return (FAILURE);
	state = 14;
	return (SUCCESS);
}
static int
test_case_3_4_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_CONN_CON) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	last_sequence = 0;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	start_tt(200);
	state = 7;
	if (get_event(fd) != __TEST_ORDREL_REQ)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}

#define desc_case_3_5_1 "\
t_look repsonse to events\n\
-- T_DATA_IND event\n\
-- t_rcv used as receive function\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_DATA_IND TPI event."
static int
test_case_3_5_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS)
		return (INCONCLUSIVE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	start_tt(300);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_DATA)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_RCV) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS || last_t_errno != TNODATA)
		return (FAILURE);
	return (SUCCESS);
}
static int
test_case_3_5_1_bot(int fd)
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
	return (SUCCESS);
}

#define desc_case_3_5_2 "\
t_look repsonse to events\n\
-- T_DATA_IND event\n\
-- t_rcvv used as receive function\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_DATA_IND TPI event."
static int
test_case_3_5_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS)
		return (INCONCLUSIVE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	start_tt(300);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_DATA)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_RCVV) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_T_RCVV) == SUCCESS || last_t_errno != TNODATA)
		return (FAILURE);
	return (SUCCESS);
}
static int
test_case_3_5_2_bot(int fd)
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
	return (SUCCESS);
}

#define desc_case_3_6_1 "\
t_look response to events\n\
-- T_UNITDATA_IND event\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_UNITDATA_IND TPI event."
static int
test_case_3_6_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	start_tt(300);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_DATA)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_RCVUDATA) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_T_RCVUDATA) == SUCCESS || last_t_errno != TNODATA)
		return (FAILURE);
	state = 10;
	return (SUCCESS);
}
static int
test_case_3_6_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_UNITDATA_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_3_7_1 "\
t_look response to events\n\
-- T_UDERROR_IND event\n\
This test case tests the response of the t_look function to events.\n\
This test case tests the response to the T_UDERROR_IND TPI event."
static int
test_case_3_7_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVUDERR) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOUDERR)
		return (FAILURE);
	state = 3;
	start_tt(300);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_UDERR)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_RCVUDERR) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (do_signal(fd, __TEST_T_RCVUDERR) == SUCCESS || last_t_errno != TNOUDERR)
		return (FAILURE);
	state = 10;
	return (SUCCESS);
}
static int
test_case_3_7_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_UDERROR_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_4_1_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_accept library call\n\
This test case tests the response of the t_accept library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_1_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 4;
	start_tt(200);
	state = 5;
	pause();
	state = 6;
	if (do_signal(fd, __TEST_T_ACCEPT) == SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 11;
	start_tt(500);
	state = 12;
	pause();
	state = 13;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 14;
	return (SUCCESS);
}
static int
test_case_4_1_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 2;
	start_tt(500);
	state = 3;
	for (;;) {
		switch (get_event(fd)) {
		case TIMEOUT:
			state = 4;
			stop_tt();
			return (SUCCESS);
		case __TEST_CONN_RES:
			if (state != 3)
				return (FAILURE);
			state = 5;
			last_t_errno = TOUTSTATE;
			last_errno = 0;
			if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
				return (FAILURE);
			state = 6;
			stop_tt();
			return (SUCCESS);
		default:
			state = 7;
			return (FAILURE);
		}
	}
}

#define desc_case_4_1_2 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_accept library call\n\
This test case tests the response of the t_accept library call to an\n\
asynchronous T_LISTEN event."
static int
test_case_4_1_2_top(int fd)
{
	int seq;
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	seq = last_sequence;
	state = 4;
	start_tt(200);
	state = 5;
	pause();
	state = 6;
	if (do_signal(fd, __TEST_T_ACCEPT) == SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (last_tevent != T_LISTEN)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 11;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 12;
	last_sequence = seq;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 13;
	return (SUCCESS);
}
static int
test_case_4_1_2_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	last_sequence = 2;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 2;
	start_tt(1000);
	state = 3;
	for (;;) {
		switch (get_event(fd)) {
		case TIMEOUT:
			state = 4;
			return (FAILURE);
		case __TEST_CONN_RES:
			if (state != 3)
				return (FAILURE);
			state = 5;
			last_t_errno = TOUTSTATE;
			last_errno = 0;
			if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
				return (FAILURE);
			state = 6;
			start_tt(200);
			continue;
		case __TEST_DISCON_REQ:
			if (state != 8) {
				state = 7;
				if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
					return (FAILURE);
				state = 8;
				continue;
			}
			state = 9;
			if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
				return (FAILURE);
			state = 10;
			stop_tt();
			return (SUCCESS);
		default:
			state = 9;
			return (FAILURE);
		}
	}
}

#define desc_case_4_2_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_connect library call\n\
This test case tests the response of the t_connect library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_2_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
test_case_4_2_1_bot(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_4_2_2 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_connect library call\n\
This test case tests the response of the t_connect library call to an\n\
asynchronous T_LISTEN event."
static int
test_case_4_2_2_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_LISTEN)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}
static int
test_case_4_2_2_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	for (;;) {
		switch (get_event(fd)) {
		case __TEST_CONN_REQ:
			if (state != 2)
				return (FAILURE);
			state = 3;
			last_t_errno = TOUTSTATE;
			last_errno = 0;
			if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
				return (FAILURE);
			state = 4;
			continue;
		case __TEST_DISCON_REQ:
			state = 5;
			if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
				return (FAILURE);
			state = 6;
			return (SUCCESS);
		case TIMEOUT:
			state = 7;
			return (SUCCESS);
		default:
			state = 8;
			return (FAILURE);
		}
	}
}

#define desc_case_4_3_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_listen library call\n\
This test case tests the response of the t_listen library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_3_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_LISTEN) == SUCCESS)
		return (FAILURE);
	state = 5;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}
static int
test_case_4_3_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}

#define desc_case_4_4_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcv library call\n\
This test case tests the response of the t_rcv library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_4_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
test_case_4_4_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_4_2 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcv library call\n\
This test case tests the response of the t_rcv library call to an\n\
asynchronous T_ORDREL event."
static int
test_case_4_4_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_tevent != T_ORDREL)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_RCVREL) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_SNDREL) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}
static int
test_case_4_4_2_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	if (get_event(fd) != __TEST_ORDREL_REQ)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}

#define desc_case_4_4_3 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcvv library call\n\
This test case tests the response of the t_rcvv library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_4_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVV) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 5;
	return (SUCCESS);
}
static int
test_case_4_4_3_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_4_4 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcvv library call\n\
This test case tests the response of the t_rcvv library call to an\n\
asynchronous T_ORDREL event."
static int
test_case_4_4_4_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVV) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_tevent != T_ORDREL)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_RCVREL) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_SNDREL) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}
static int
test_case_4_4_4_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	if (get_event(fd) != __TEST_ORDREL_REQ)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}

#define desc_case_4_4_5 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_snd library call\n\
This test case tests the response of the t_snd library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_4_5_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SND) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_4_5_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_4_6 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_snd library call\n\
This test case tests the response of the t_snd library call to an\n\
asynchronous T_ORDREL event."
static int
test_case_4_4_6_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SND) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_ORDREL)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVREL) != SUCCESS)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_SNDREL) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}
static int
test_case_4_4_6_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	if (get_event(fd) != __TEST_ORDREL_REQ)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}

#define desc_case_4_4_7 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_sndv library call\n\
This test case tests the response of the t_sndv library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_4_7_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SNDV) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_4_7_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_4_8 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_sndv library call\n\
This test case tests the response of the t_sndv library call to an\n\
asynchronous T_ORDREL event."
static int
test_case_4_4_8_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SNDV) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_ORDREL)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVREL) != SUCCESS)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_SNDREL) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}
static int
test_case_4_4_8_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_ORDREL_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	if (get_event(fd) != __TEST_ORDREL_REQ)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}

#define desc_case_4_5_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcvconnect library call\n\
This test case tests the response of the t_rcvconnect library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_5_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	start_tt(300);
	state = 4;
	pause();
	state = 5;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 6;
	if (do_signal(fd, __TEST_T_RCVCONNECT) == SUCCESS)
		return (FAILURE);
	state = 7;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 9;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 10;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 11;
	return (SUCCESS);
}
static int
test_case_4_5_1_bot(int fd)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	start_tt(200);
	state = 4;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}

#define desc_case_4_6_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcvrel library call\n\
This test case tests the response of the t_rcvrel library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_6_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_RCVREL) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_6_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_6_2 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcvreldata library call\n\
This test case tests the response of the t_rcvreldata library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_6_2_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_RCVRELDATA) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_6_2_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_6_3 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_sndrel library call\n\
This test case tests the response of the t_sndrel library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_6_3_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SNDREL) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_6_3_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_6_4 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_sndreldata library call\n\
This test case tests the response of the t_sndreldata library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_6_4_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SNDRELDATA) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_6_4_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_7_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcvudata library call\n\
This test case tests the response of the t_rcvudata library call to an\n\
asynchronous T_UDERR event."
static int
test_case_4_7_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_RCVUDATA) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_UDERR)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVUDERR) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_7_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_UDERROR_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return(SUCCESS);
}

#define desc_case_4_7_2 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_rcvvudata library call\n\
This test case tests the response of the t_rcvvudata library call to an\n\
asynchronous T_UDERR event."
static int
test_case_4_7_2_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_RCVVUDATA) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_UDERR)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVUDERR) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_7_2_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_UDERROR_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return(SUCCESS);
}

#define desc_case_4_7_3 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_sndudata library call\n\
This test case tests the response of the t_sndudata library call to an\n\
asynchronous T_UDERR event."
static int
test_case_4_7_3_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SNDUDATA) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_UDERR)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVUDERR) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_7_3_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_UDERROR_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return(SUCCESS);
}

#define desc_case_4_7_4 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_sndvudata library call\n\
This test case tests the response of the t_sndvudata library call to an\n\
asynchronous T_UDERR event."
static int
test_case_4_7_4_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SNDVUDATA) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_UDERR)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVUDERR) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_7_4_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_UDERROR_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return(SUCCESS);
}

#define desc_case_4_8_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_unbind library call\n\
This test case tests the response of the t_unbind library call to an\n\
asynchronous T_LISTEN event."
static int
test_case_4_8_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_UNBIND) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_LISTEN)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 8;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 9;
	return (SUCCESS);
}
static int
test_case_4_8_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_4_8_2 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_unbind library call\n\
This test case tests the response of the t_unbind library call to an\n\
asynchronous T_DATA event."
static int
test_case_4_8_2_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_UNBIND) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DATA)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVUDATA) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_8_2_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_UNITDATA_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_8_3 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_unbind library call\n\
This test case tests the response of the t_unbind library call to an\n\
asynchronous T_UDERR event."
static int
test_case_4_8_3_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_UNBIND) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_UDERR)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVUDERR) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_8_3_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_UDERROR_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_4_9_1 "\
Asynchronous operation\n\
-- TLOOK response to various calls\n\
-- t_snddis library call\n\
This test case tests the response of the t_snddis library call to an\n\
asynchronous T_DISCONNECT event."
static int
test_case_4_9_1_top(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_SNDDIS) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TLOOK)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_LOOK) != SUCCESS)
		return (FAILURE);
	state = 6;
	if (last_tevent != T_DISCONNECT)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_RCVDIS) != SUCCESS)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_4_9_1_bot(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_DISCON_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}

#define desc_case_5_1_1 "\
Operations on a closed file descriptor\n\
-- t_accept operation\n\
This test case tests the t_accept operation on a closed file descriptor."
static int
test_case_5_1_x_top(int fd, int function)
{
	state = 0;
	close(5);
	state = 1;
	top_fd = 5;
	if (do_signal(5, function) == SUCCESS || last_t_errno != TBADF)
		return (FAILURE);
	top_fd = fd;
	state = 2;
	return (SUCCESS);
}
static int
test_case_5_1_x_bot(int fd, int function)
{
	state = 0;
	return (SUCCESS);
}
static int
test_case_5_1_1_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_ACCEPT);
}
static int
test_case_5_1_1_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_ACCEPT);
}

#define desc_case_5_1_2 "\
Operations on a closed file descriptor\n\
-- t_bind operation\n\
This test case tests the t_bind operation on a closed file descriptor."
static int
test_case_5_1_2_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_BIND);
}
static int
test_case_5_1_2_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_BIND);
}

#define desc_case_5_1_3 "\
Operations on a closed file descriptor\n\
-- t_close operation\n\
This test case tests the t_close operation on a closed file descriptor."
static int
test_case_5_1_3_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_CLOSE);
}
static int
test_case_5_1_3_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_CLOSE);
}

#define desc_case_5_1_4 "\
Operations on a closed file descriptor\n\
-- t_connect operation\n\
This test case tests the t_connect operation on a closed file descriptor."
static int
test_case_5_1_4_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_CONNECT);
}
static int
test_case_5_1_4_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_CONNECT);
}

#define desc_case_5_1_5 "\
Operations on a closed file descriptor\n\
-- t_getinfo operation\n\
This test case tests the t_getinfo operation on a closed file descriptor."
static int
test_case_5_1_5_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_GETINFO);
}
static int
test_case_5_1_5_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_GETINFO);
}

#define desc_case_5_1_6 "\
Operations on a closed file descriptor\n\
-- t_getprotaddr operation\n\
This test case tests the t_getprotaddr operation on a closed file descriptor."
static int
test_case_5_1_6_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_GETPROTADDR);
}
static int
test_case_5_1_6_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_GETPROTADDR);
}

#define desc_case_5_1_7 "\
Operations on a closed file descriptor\n\
-- t_getstate operation\n\
This test case tests the t_getstate operation on a closed file descriptor."
static int
test_case_5_1_7_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_GETSTATE);
}
static int
test_case_5_1_7_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_GETSTATE);
}

#define desc_case_5_1_8 "\
Operations on a closed file descriptor\n\
-- t_listen operation\n\
This test case tests the t_listen operation on a closed file descriptor."
static int
test_case_5_1_8_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_LISTEN);
}
static int
test_case_5_1_8_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_LISTEN);
}

#define desc_case_5_1_9 "\
Operations on a closed file descriptor\n\
-- t_look operation\n\
This test case tests the t_look operation on a closed file descriptor."
static int
test_case_5_1_9_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_LOOK);
}
static int
test_case_5_1_9_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_LOOK);
}

#define desc_case_5_1_10 "\
Operations on a closed file descriptor\n\
-- t_optmgmt operation\n\
This test case tests the t_optmgmt operation on a closed file descriptor."
static int
test_case_5_1_10_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_OPTMGMT);
}
static int
test_case_5_1_10_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_OPTMGMT);
}

#define desc_case_5_1_11 "\
Operations on a closed file descriptor\n\
-- t_rcv operation\n\
This test case tests the t_rcv operation on a closed file descriptor."
static int
test_case_5_1_11_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCV);
}
static int
test_case_5_1_11_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCV);
}

#define desc_case_5_1_12 "\
Operations on a closed file descriptor\n\
-- t_rcvconnect operation\n\
This test case tests the t_rcvconnect operation on a closed file descriptor."
static int
test_case_5_1_12_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVCONNECT);
}
static int
test_case_5_1_12_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVCONNECT);
}

#define desc_case_5_1_13 "\
Operations on a closed file descriptor\n\
-- t_rcvdis operation\n\
This test case tests the t_rcvdis operation on a closed file descriptor."
static int
test_case_5_1_13_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVDIS);
}
static int
test_case_5_1_13_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVDIS);
}

#define desc_case_5_1_14 "\
Operations on a closed file descriptor\n\
-- t_rcvrel operation\n\
This test case tests the t_rcvrel operation on a closed file descriptor."
static int
test_case_5_1_14_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVREL);
}
static int
test_case_5_1_14_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVREL);
}

#define desc_case_5_1_15 "\
Operations on a closed file descriptor\n\
-- t_rcvreldata operation\n\
This test case tests the t_rcvreldata operation on a closed file descriptor."
static int
test_case_5_1_15_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVRELDATA);
}
static int
test_case_5_1_15_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVRELDATA);
}

#define desc_case_5_1_16 "\
Operations on a closed file descriptor\n\
-- t_rcvudata operation\n\
This test case tests the t_rcvudata operation on a closed file descriptor."
static int
test_case_5_1_16_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVUDATA);
}
static int
test_case_5_1_16_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVUDATA);
}

#define desc_case_5_1_17 "\
Operations on a closed file descriptor\n\
-- t_rcvuderr operation\n\
This test case tests the t_rcvuderr operation on a closed file descriptor."
static int
test_case_5_1_17_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVUDERR);
}
static int
test_case_5_1_17_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVUDERR);
}

#define desc_case_5_1_18 "\
Operations on a closed file descriptor\n\
-- t_rcvv operation\n\
This test case tests the t_rcvv operation on a closed file descriptor."
static int
test_case_5_1_18_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVV);
}
static int
test_case_5_1_18_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVV);
}

#define desc_case_5_1_19 "\
Operations on a closed file descriptor\n\
-- t_rcvvudata operation\n\
This test case tests the t_rcvvudata operation on a closed file descriptor."
static int
test_case_5_1_19_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_RCVVUDATA);
}
static int
test_case_5_1_19_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_RCVVUDATA);
}

#define desc_case_5_1_20 "\
Operations on a closed file descriptor\n\
-- t_snd operation\n\
This test case tests the t_snd operation on a closed file descriptor."
static int
test_case_5_1_20_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SND);
}
static int
test_case_5_1_20_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SND);
}

#define desc_case_5_1_21 "\
Operations on a closed file descriptor\n\
-- t_snddis operation\n\
This test case tests the t_snddis operation on a closed file descriptor."
static int
test_case_5_1_21_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SNDDIS);
}
static int
test_case_5_1_21_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SNDDIS);
}

#define desc_case_5_1_22 "\
Operations on a closed file descriptor\n\
-- t_sndrel operation\n\
This test case tests the t_sndrel operation on a closed file descriptor."
static int
test_case_5_1_22_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SNDREL);
}
static int
test_case_5_1_22_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SNDREL);
}

#define desc_case_5_1_23 "\
Operations on a closed file descriptor\n\
-- t_sndreldata operation\n\
This test case tests the t_sndreldata operation on a closed file descriptor."
static int
test_case_5_1_23_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SNDRELDATA);
}
static int
test_case_5_1_23_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SNDRELDATA);
}

#define desc_case_5_1_24 "\
Operations on a closed file descriptor\n\
-- t_sndudata operation\n\
This test case tests the t_sndudata operation on a closed file descriptor."
static int
test_case_5_1_24_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SNDUDATA);
}
static int
test_case_5_1_24_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SNDUDATA);
}

#define desc_case_5_1_25 "\
Operations on a closed file descriptor\n\
-- t_sndv operation\n\
This test case tests the t_sndv operation on a closed file descriptor."
static int
test_case_5_1_25_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SNDV);
}
static int
test_case_5_1_25_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SNDV);
}

#define desc_case_5_1_26 "\
Operations on a closed file descriptor\n\
-- t_sndvudata operation\n\
This test case tests the t_sndvudata operation on a closed file descriptor."
static int
test_case_5_1_26_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SNDVUDATA);
}
static int
test_case_5_1_26_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SNDVUDATA);
}

#define desc_case_5_1_27 "\
Operations on a closed file descriptor\n\
-- t_sync operation\n\
This test case tests the t_sync operation on a closed file descriptor."
static int
test_case_5_1_27_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_SYNC);
}
static int
test_case_5_1_27_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_SYNC);
}

#define desc_case_5_1_28 "\
Operations on a closed file descriptor\n\
-- t_unbind operation\n\
This test case tests the t_unbind operation on a closed file descriptor."
static int
test_case_5_1_28_top(int fd)
{
	return test_case_5_1_x_top(fd, __TEST_T_UNBIND);
}
static int
test_case_5_1_28_bot(int fd)
{
	return test_case_5_1_x_bot(fd, __TEST_T_UNBIND);
}

#define desc_case_5_2_1 "\
Operations on a non-STREAMS file descriptor\n\
-- t_accept operation\n\
This test case tests the t_accept operation on a non-STREAMS file descriptor."
static int
test_case_5_2_x_top(int fd, int function)
{
	state = 0;
	top_fd = fileno(stderr);
	state = 1;
	if (do_signal(top_fd, function) == SUCCESS || last_t_errno != TBADF)
		return (FAILURE);
	top_fd = fd;
	state = 2;
	return (SUCCESS);
}
static int
test_case_5_2_x_bot(int fd, int function)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_5_2_1_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_ACCEPT);
}
static int
test_case_5_2_1_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_ACCEPT);
}

#define desc_case_5_2_2 "\
Operations on a non-STREAMS file descriptor\n\
-- t_bind operation\n\
This test case tests the t_bind operation on a non-STREAMS file descriptor."
static int
test_case_5_2_2_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_BIND);
}
static int
test_case_5_2_2_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_BIND);
}

#define desc_case_5_2_3 "\
Operations on a non-STREAMS file descriptor\n\
-- t_close operation\n\
This test case tests the t_close operation on a non-STREAMS file descriptor."
static int
test_case_5_2_3_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_CLOSE);
}
static int
test_case_5_2_3_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_CLOSE);
}

#define desc_case_5_2_4 "\
Operations on a non-STREAMS file descriptor\n\
-- t_connect operation\n\
This test case tests the t_connect operation on a non-STREAMS file descriptor."
static int
test_case_5_2_4_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_CONNECT);
}
static int
test_case_5_2_4_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_CONNECT);
}

#define desc_case_5_2_5 "\
Operations on a non-STREAMS file descriptor\n\
-- t_getinfo operation\n\
This test case tests the t_getinfo operation on a non-STREAMS file descriptor."
static int
test_case_5_2_5_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_GETINFO);
}
static int
test_case_5_2_5_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_GETINFO);
}

#define desc_case_5_2_6 "\
Operations on a non-STREAMS file descriptor\n\
-- t_getprotaddr operation\n\
This test case tests the t_getprotaddr operation on a non-STREAMS file descriptor."
static int
test_case_5_2_6_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_GETPROTADDR);
}
static int
test_case_5_2_6_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_GETPROTADDR);
}

#define desc_case_5_2_7 "\
Operations on a non-STREAMS file descriptor\n\
-- t_getstate operation\n\
This test case tests the t_getstate operation on a non-STREAMS file descriptor."
static int
test_case_5_2_7_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_GETSTATE);
}
static int
test_case_5_2_7_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_GETSTATE);
}

#define desc_case_5_2_8 "\
Operations on a non-STREAMS file descriptor\n\
-- t_listen operation\n\
This test case tests the t_listen operation on a non-STREAMS file descriptor."
static int
test_case_5_2_8_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_LISTEN);
}
static int
test_case_5_2_8_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_LISTEN);
}

#define desc_case_5_2_9 "\
Operations on a non-STREAMS file descriptor\n\
-- t_look operation\n\
This test case tests the t_look operation on a non-STREAMS file descriptor."
static int
test_case_5_2_9_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_LOOK);
}
static int
test_case_5_2_9_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_LOOK);
}

#define desc_case_5_2_10 "\
Operations on a non-STREAMS file descriptor\n\
-- t_optmgmt operation\n\
This test case tests the t_optmgmt operation on a non-STREAMS file descriptor."
static int
test_case_5_2_10_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_OPTMGMT);
}
static int
test_case_5_2_10_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_OPTMGMT);
}

#define desc_case_5_2_11 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcv operation\n\
This test case tests the t_rcv operation on a non-STREAMS file descriptor."
static int
test_case_5_2_11_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCV);
}
static int
test_case_5_2_11_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCV);
}

#define desc_case_5_2_12 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvconnect operation\n\
This test case tests the t_rcvconnect operation on a non-STREAMS file descriptor."
static int
test_case_5_2_12_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVCONNECT);
}
static int
test_case_5_2_12_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVCONNECT);
}

#define desc_case_5_2_13 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvdis operation\n\
This test case tests the t_rcvdis operation on a non-STREAMS file descriptor."
static int
test_case_5_2_13_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVDIS);
}
static int
test_case_5_2_13_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVDIS);
}

#define desc_case_5_2_14 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvrel operation\n\
This test case tests the t_rcvrel operation on a non-STREAMS file descriptor."
static int
test_case_5_2_14_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVREL);
}
static int
test_case_5_2_14_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVREL);
}

#define desc_case_5_2_15 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvreldata operation\n\
This test case tests the t_rcvreldata operation on a non-STREAMS file descriptor."
static int
test_case_5_2_15_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVRELDATA);
}
static int
test_case_5_2_15_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVRELDATA);
}

#define desc_case_5_2_16 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvudata operation\n\
This test case tests the t_rcvudata operation on a non-STREAMS file descriptor."
static int
test_case_5_2_16_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVUDATA);
}
static int
test_case_5_2_16_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVUDATA);
}

#define desc_case_5_2_17 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvuderr operation\n\
This test case tests the t_rcvuderr operation on a non-STREAMS file descriptor."
static int
test_case_5_2_17_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVUDERR);
}
static int
test_case_5_2_17_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVUDERR);
}

#define desc_case_5_2_18 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvv operation\n\
This test case tests the t_rcvv operation on a non-STREAMS file descriptor."
static int
test_case_5_2_18_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVV);
}
static int
test_case_5_2_18_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVV);
}

#define desc_case_5_2_19 "\
Operations on a non-STREAMS file descriptor\n\
-- t_rcvvudata operation\n\
This test case tests the t_rcvvudata operation on a non-STREAMS file descriptor."
static int
test_case_5_2_19_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_RCVVUDATA);
}
static int
test_case_5_2_19_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_RCVVUDATA);
}

#define desc_case_5_2_20 "\
Operations on a non-STREAMS file descriptor\n\
-- t_snd operation\n\
This test case tests the t_snd operation on a non-STREAMS file descriptor."
static int
test_case_5_2_20_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SND);
}
static int
test_case_5_2_20_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SND);
}

#define desc_case_5_2_21 "\
Operations on a non-STREAMS file descriptor\n\
-- t_snddis operation\n\
This test case tests the t_snddis operation on a non-STREAMS file descriptor."
static int
test_case_5_2_21_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SNDDIS);
}
static int
test_case_5_2_21_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SNDDIS);
}

#define desc_case_5_2_22 "\
Operations on a non-STREAMS file descriptor\n\
-- t_sndrel operation\n\
This test case tests the t_sndrel operation on a non-STREAMS file descriptor."
static int
test_case_5_2_22_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SNDREL);
}
static int
test_case_5_2_22_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SNDREL);
}

#define desc_case_5_2_23 "\
Operations on a non-STREAMS file descriptor\n\
-- t_sndreldata operation\n\
This test case tests the t_sndreldata operation on a non-STREAMS file descriptor."
static int
test_case_5_2_23_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SNDRELDATA);
}
static int
test_case_5_2_23_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SNDRELDATA);
}

#define desc_case_5_2_24 "\
Operations on a non-STREAMS file descriptor\n\
-- t_sndudata operation\n\
This test case tests the t_sndudata operation on a non-STREAMS file descriptor."
static int
test_case_5_2_24_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SNDUDATA);
}
static int
test_case_5_2_24_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SNDUDATA);
}

#define desc_case_5_2_25 "\
Operations on a non-STREAMS file descriptor\n\
-- t_sndv operation\n\
This test case tests the t_sndv operation on a non-STREAMS file descriptor."
static int
test_case_5_2_25_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SNDV);
}
static int
test_case_5_2_25_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SNDV);
}

#define desc_case_5_2_26 "\
Operations on a non-STREAMS file descriptor\n\
-- t_sndvudata operation\n\
This test case tests the t_sndvudata operation on a non-STREAMS file descriptor."
static int
test_case_5_2_26_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SNDVUDATA);
}
static int
test_case_5_2_26_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SNDVUDATA);
}

#define desc_case_5_2_27 "\
Operations on a non-STREAMS file descriptor\n\
-- t_sync operation\n\
This test case tests the t_sync operation on a non-STREAMS file descriptor."
static int
test_case_5_2_27_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_SYNC);
}
static int
test_case_5_2_27_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_SYNC);
}

#define desc_case_5_2_28 "\
Operations on a non-STREAMS file descriptor\n\
-- t_unbind operation\n\
This test case tests the t_unbind operation on a non-STREAMS file descriptor."
static int
test_case_5_2_28_top(int fd)
{
	return test_case_5_2_x_top(fd, __TEST_T_UNBIND);
}
static int
test_case_5_2_28_bot(int fd)
{
	return test_case_5_2_x_bot(fd, __TEST_T_UNBIND);
}

#define desc_case_5_3_1 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_accept operation\n\
This test case tests the t_accept operation on a STREAMS file descriptor."
static int
test_case_5_3_x_top(int fd, int function)
{
	state = 0;
	if (do_signal(fd, function) == SUCCESS || last_t_errno != TBADF)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}
static int
test_case_5_3_x_bot(int fd, int function)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_5_3_1_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_ACCEPT);
}
static int
test_case_5_3_1_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_ACCEPT);
}

#define desc_case_5_3_2 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_bind operation\n\
This test case tests the t_bind operation on a STREAMS file descriptor."
static int
test_case_5_3_2_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_BIND);
}
static int
test_case_5_3_2_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_BIND);
}

#define desc_case_5_3_3 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_close operation\n\
This test case tests the t_close operation on a STREAMS file descriptor."
static int
test_case_5_3_3_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_CLOSE);
}
static int
test_case_5_3_3_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_CLOSE);
}

#define desc_case_5_3_4 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_connect operation\n\
This test case tests the t_connect operation on a STREAMS file descriptor."
static int
test_case_5_3_4_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_CONNECT);
}
static int
test_case_5_3_4_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_CONNECT);
}

#define desc_case_5_3_5 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_getinfo operation\n\
This test case tests the t_getinfo operation on a STREAMS file descriptor."
static int
test_case_5_3_5_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_GETINFO);
}
static int
test_case_5_3_5_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_GETINFO);
}

#define desc_case_5_3_6 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_getprotaddr operation\n\
This test case tests the t_getprotaddr operation on a STREAMS file descriptor."
static int
test_case_5_3_6_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_GETPROTADDR);
}
static int
test_case_5_3_6_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_GETPROTADDR);
}

#define desc_case_5_3_7 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_getstate operation\n\
This test case tests the t_getstate operation on a STREAMS file descriptor."
static int
test_case_5_3_7_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_GETSTATE);
}
static int
test_case_5_3_7_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_GETSTATE);
}

#define desc_case_5_3_8 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_listen operation\n\
This test case tests the t_listen operation on a STREAMS file descriptor."
static int
test_case_5_3_8_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_LISTEN);
}
static int
test_case_5_3_8_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_LISTEN);
}

#define desc_case_5_3_9 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_look operation\n\
This test case tests the t_look operation on a STREAMS file descriptor."
static int
test_case_5_3_9_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_LOOK);
}
static int
test_case_5_3_9_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_LOOK);
}

#define desc_case_5_3_10 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_optmgmt operation\n\
This test case tests the t_optmgmt operation on a STREAMS file descriptor."
static int
test_case_5_3_10_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_OPTMGMT);
}
static int
test_case_5_3_10_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_OPTMGMT);
}

#define desc_case_5_3_11 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcv operation\n\
This test case tests the t_rcv operation on a STREAMS file descriptor."
static int
test_case_5_3_11_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCV);
}
static int
test_case_5_3_11_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCV);
}

#define desc_case_5_3_12 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvconnect operation\n\
This test case tests the t_rcvconnect operation on a STREAMS file descriptor."
static int
test_case_5_3_12_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVCONNECT);
}
static int
test_case_5_3_12_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVCONNECT);
}

#define desc_case_5_3_13 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvdis operation\n\
This test case tests the t_rcvdis operation on a STREAMS file descriptor."
static int
test_case_5_3_13_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVDIS);
}
static int
test_case_5_3_13_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVDIS);
}

#define desc_case_5_3_14 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvrel operation\n\
This test case tests the t_rcvrel operation on a STREAMS file descriptor."
static int
test_case_5_3_14_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVREL);
}
static int
test_case_5_3_14_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVREL);
}

#define desc_case_5_3_15 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvreldata operation\n\
This test case tests the t_rcvreldata operation on a STREAMS file descriptor."
static int
test_case_5_3_15_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVRELDATA);
}
static int
test_case_5_3_15_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVRELDATA);
}

#define desc_case_5_3_16 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvudata operation\n\
This test case tests the t_rcvudata operation on a STREAMS file descriptor."
static int
test_case_5_3_16_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVUDATA);
}
static int
test_case_5_3_16_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVUDATA);
}

#define desc_case_5_3_17 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvuderr operation\n\
This test case tests the t_rcvuderr operation on a STREAMS file descriptor."
static int
test_case_5_3_17_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVUDERR);
}
static int
test_case_5_3_17_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVUDERR);
}

#define desc_case_5_3_18 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvv operation\n\
This test case tests the t_rcvv operation on a STREAMS file descriptor."
static int
test_case_5_3_18_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVV);
}
static int
test_case_5_3_18_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVV);
}

#define desc_case_5_3_19 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_rcvvudata operation\n\
This test case tests the t_rcvvudata operation on a STREAMS file descriptor."
static int
test_case_5_3_19_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_RCVVUDATA);
}
static int
test_case_5_3_19_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_RCVVUDATA);
}

#define desc_case_5_3_20 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_snd operation\n\
This test case tests the t_snd operation on a STREAMS file descriptor."
static int
test_case_5_3_20_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SND);
}
static int
test_case_5_3_20_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SND);
}

#define desc_case_5_3_21 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_snddis operation\n\
This test case tests the t_snddis operation on a STREAMS file descriptor."
static int
test_case_5_3_21_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SNDDIS);
}
static int
test_case_5_3_21_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SNDDIS);
}

#define desc_case_5_3_22 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_sndrel operation\n\
This test case tests the t_sndrel operation on a STREAMS file descriptor."
static int
test_case_5_3_22_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SNDREL);
}
static int
test_case_5_3_22_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SNDREL);
}

#define desc_case_5_3_23 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_sndreldata operation\n\
This test case tests the t_sndreldata operation on a STREAMS file descriptor."
static int
test_case_5_3_23_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SNDRELDATA);
}
static int
test_case_5_3_23_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SNDRELDATA);
}

#define desc_case_5_3_24 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_sndudata operation\n\
This test case tests the t_sndudata operation on a STREAMS file descriptor."
static int
test_case_5_3_24_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SNDUDATA);
}
static int
test_case_5_3_24_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SNDUDATA);
}

#define desc_case_5_3_25 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_sndv operation\n\
This test case tests the t_sndv operation on a STREAMS file descriptor."
static int
test_case_5_3_25_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SNDV);
}
static int
test_case_5_3_25_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SNDV);
}

#define desc_case_5_3_26 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_sndvudata operation\n\
This test case tests the t_sndvudata operation on a STREAMS file descriptor."
static int
test_case_5_3_26_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SNDVUDATA);
}
static int
test_case_5_3_26_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SNDVUDATA);
}

#define desc_case_5_3_27 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_sync operation\n\
This test case tests the t_sync operation on a STREAMS file descriptor."
static int
test_case_5_3_27_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_SYNC);
}
static int
test_case_5_3_27_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_SYNC);
}

#define desc_case_5_3_28 "\
Operations on a STREAMS file descriptor\n\
-- without \"timod\" pushed\n\
-- t_unbind operation\n\
This test case tests the t_unbind operation on a STREAMS file descriptor."
static int
test_case_5_3_28_top(int fd)
{
	return test_case_5_3_x_top(fd, __TEST_T_UNBIND);
}
static int
test_case_5_3_28_bot(int fd)
{
	return test_case_5_3_x_bot(fd, __TEST_T_UNBIND);
}

#define desc_case_6_1_1 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TACCES error\n\
This test case tests the TACCES error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_x_top(int fd, int terror, int error)
{
	state = 0;
	start_tt(200);
	state = 1;
	pause();
	state = 2;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_ACCEPT) == SUCCESS)
		return (FAILURE);
	state = 5;
	if (last_t_errno != terror)
		return (FAILURE);
	state = 6;
	if (last_t_errno == TSYSERR && last_errno != error)
		return (FAILURE);
	state = 7;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (INCONCLUSIVE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_6_1_x_bot(int fd, int terror, int error)
{
	state = 0;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	start_tt(500);
	state = 2;
	if (get_event(fd) != __TEST_CONN_RES)
		return (FAILURE);
	state = 3;
	last_t_errno = terror;
	last_errno = error;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 4;
	start_tt(500);
	state = 5;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (INCONCLUSIVE);
	state = 6;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 7;
	return (SUCCESS);
}
static int
test_case_6_1_1_top(int fd)
{
	return test_case_6_1_x_top(fd, TACCES, 0);
}
static int
test_case_6_1_1_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TACCES, 0);
}

#define desc_case_6_1_2 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TBADADDR error\n\
This test case tests the TBADADDR error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_2_top(int fd)
{
	return test_case_6_1_x_top(fd, TBADADDR, 0);
}
static int
test_case_6_1_2_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TBADADDR, 0);
}

#define desc_case_6_1_3 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TBADDATA error\n\
This test case tests the TBADDATA error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_3_top(int fd)
{
	return test_case_6_1_x_top(fd, TBADDATA, 0);
}
static int
test_case_6_1_3_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TBADDATA, 0);
}

#define desc_case_6_1_4 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TBADF error\n\
This test case tests the TBADF error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_4_top(int fd)
{
	return test_case_6_1_x_top(fd, TBADF, 0);
}
static int
test_case_6_1_4_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TBADF, 0);
}

#define desc_case_6_1_5 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TBADOPT error\n\
This test case tests the TBADOPT error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_5_top(int fd)
{
	return test_case_6_1_x_top(fd, TBADOPT, 0);
}
static int
test_case_6_1_5_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TBADOPT, 0);
}

#define desc_case_6_1_6 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TBADSEQ error\n\
This test case tests the TBADSEQ error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_6_top(int fd)
{
	return test_case_6_1_x_top(fd, TBADSEQ, 0);
}
static int
test_case_6_1_6_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TBADSEQ, 0);
}

#define desc_case_6_1_7 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_7_top(int fd)
{
	return test_case_6_1_x_top(fd, TNOTSUPPORT, 0);
}
static int
test_case_6_1_7_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TNOTSUPPORT, 0);
}

#define desc_case_6_1_8 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_8_top(int fd)
{
	return test_case_6_1_x_top(fd, TOUTSTATE, 0);
}
static int
test_case_6_1_8_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TOUTSTATE, 0);
}

#define desc_case_6_1_9 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TPROVMISMATCH error\n\
This test case tests the TPROVMISMATCH error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_9_top(int fd)
{
	return test_case_6_1_x_top(fd, TPROVMISMATCH, 0);
}
static int
test_case_6_1_9_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TPROVMISMATCH, 0);
}

#define desc_case_6_1_10 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TRESADDR error\n\
This test case tests the TRESADDR error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_10_top(int fd)
{
	return test_case_6_1_x_top(fd, TRESADDR, 0);
}
static int
test_case_6_1_10_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TRESADDR, 0);
}

#define desc_case_6_1_11 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TRESQLEN error\n\
This test case tests the TRESQLEN error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_11_top(int fd)
{
	return test_case_6_1_x_top(fd, TRESQLEN, 0);
}
static int
test_case_6_1_11_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TRESQLEN, 0);
}

#define desc_case_6_1_12 "\
Valid TPI error acknowledgements for all calls\n\
-- t_accept library call\n\
-- TSYSERR error\n\
This test case tests the TSYSERR error acknowledgement from the TPI provider\n\
in response to the t_accept library call."
static int
test_case_6_1_12_top(int fd)
{
	return test_case_6_1_x_top(fd, TSYSERR, EPROTO);
}
static int
test_case_6_1_12_bot(int fd)
{
	return test_case_6_1_x_bot(fd, TSYSERR, EPROTO);
}

#define desc_case_6_2_1 "\
Valid TPI error acknowledgements for all calls\n\
-- t_bind library call\n\
-- TACCES error\n\
This test case tests the TACCES error acknowledgement from the TPI provider\n\
in response to the t_bind library call."
static int
test_case_6_2_x_top(int fd, int terror, int error)
{
	state = 0;
	if (do_signal(fd, __TEST_T_BIND) == SUCCESS)
		return (FAILURE);
	if (last_t_errno != terror)
		return (FAILURE);
	if (last_t_errno == TSYSERR && last_errno != error)
		return (FAILURE);
	state = 1;
	return (SUCCESS);
}
static int
test_case_6_2_x_bot(int fd, int terror, int error)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = terror;
	last_errno = error;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_6_2_1_top(int fd)
{
	return test_case_6_2_x_top(fd, TACCES, 0);
}
static int
test_case_6_2_1_bot(int fd)
{
	return test_case_6_2_x_bot(fd, TACCES, 0);
}

#define desc_case_6_2_2 "\
Valid TPI error acknowledgements for all calls\n\
-- t_bind library call\n\
-- TADDRBUSY error\n\
This test case tests the TADDRBUSY error acknowledgement from the TPI provider\n\
in response to the t_bind library call."
static int
test_case_6_2_2_top(int fd)
{
	return test_case_6_2_x_top(fd, TADDRBUSY, 0);
}
static int
test_case_6_2_2_bot(int fd)
{
	return test_case_6_2_x_bot(fd, TADDRBUSY, 0);
}

#define desc_case_6_2_3 "\
Valid TPI error acknowledgements for all calls\n\
-- t_bind library call\n\
-- TBADADDR error\n\
This test case tests the TBADADDR error acknowledgement from the TPI provider\n\
in response to the t_bind library call."
static int
test_case_6_2_3_top(int fd)
{
	return test_case_6_2_x_top(fd, TBADADDR, 0);
}
static int
test_case_6_2_3_bot(int fd)
{
	return test_case_6_2_x_bot(fd, TBADADDR, 0);
}

#define desc_case_6_2_4 "\
Valid TPI error acknowledgements for all calls\n\
-- t_bind library call\n\
-- TNOADDR error\n\
This test case tests the TNOADDR error acknowledgement from the TPI provider\n\
in response to the t_bind library call."
static int
test_case_6_2_4_top(int fd)
{
	return test_case_6_2_x_top(fd, TNOADDR, 0);
}
static int
test_case_6_2_4_bot(int fd)
{
	return test_case_6_2_x_bot(fd, TNOADDR, 0);
}

#define desc_case_6_2_5 "\
Valid TPI error acknowledgements for all calls\n\
-- t_bind library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error acknowledgement from the TPI provider\n\
in response to the t_bind library call."
static int
test_case_6_2_5_top(int fd)
{
	return test_case_6_2_x_top(fd, TOUTSTATE, 0);
}
static int
test_case_6_2_5_bot(int fd)
{
	return test_case_6_2_x_bot(fd, TOUTSTATE, 0);
}

#define desc_case_6_2_6 "\
Valid TPI error acknowledgements for all calls\n\
-- t_bind library call\n\
-- TSYSERR error\n\
This test case tests the TSYSERR error acknowledgement from the TPI provider\n\
in response to the t_bind library call."
static int
test_case_6_2_6_top(int fd)
{
	return test_case_6_2_x_top(fd, TSYSERR, EPROTO);
}
static int
test_case_6_2_6_bot(int fd)
{
	return test_case_6_2_x_bot(fd, TSYSERR, EPROTO);
}

#define desc_case_6_3_1 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TACCES error\n\
This test case tests the TACCES error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_x_top(int fd, int terror, int error)
{
	state = 0;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != terror)
		return (FAILURE);
	state = 2;
	if (last_t_errno == TSYSERR && last_errno != error)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_6_3_x_bot(int fd, int terror, int error)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = terror;
	last_errno = error;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_6_3_1_top(int fd)
{
	return test_case_6_3_x_top(fd, TACCES, 0);
}
static int
test_case_6_3_1_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TACCES, 0);
}

#define desc_case_6_3_2 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TADDRBUSY error\n\
This test case tests the TADDRBUSY error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_2_top(int fd)
{
	return test_case_6_3_x_top(fd, TADDRBUSY, 0);
}
static int
test_case_6_3_2_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TADDRBUSY, 0);
}

#define desc_case_6_3_3 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TBADADDR error\n\
This test case tests the TBADADDR error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_3_top(int fd)
{
	return test_case_6_3_x_top(fd, TBADADDR, 0);
}
static int
test_case_6_3_3_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TBADADDR, 0);
}

#define desc_case_6_3_4 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TBADDATA error\n\
This test case tests the TBADDATA error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_4_top(int fd)
{
	return test_case_6_3_x_top(fd, TBADDATA, 0);
}
static int
test_case_6_3_4_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TBADDATA, 0);
}

#define desc_case_6_3_5 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TBADOPT error\n\
This test case tests the TBADOPT error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_5_top(int fd)
{
	return test_case_6_3_x_top(fd, TBADOPT, 0);
}
static int
test_case_6_3_5_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TBADOPT, 0);
}

#define desc_case_6_3_6 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_6_top(int fd)
{
	return test_case_6_3_x_top(fd, TNOTSUPPORT, 0);
}
static int
test_case_6_3_6_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TNOTSUPPORT, 0);
}

#define desc_case_6_3_7 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_7_top(int fd)
{
	return test_case_6_3_x_top(fd, TOUTSTATE, 0);
}
static int
test_case_6_3_7_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TOUTSTATE, 0);
}

#define desc_case_6_3_8 "\
Valid TPI error acknowledgements for all calls\n\
-- t_connect library call\n\
-- TSYSERR error\n\
This test case tests the TSYSERR error acknowledgement from the TPI provider\n\
in response to the t_connect library call."
static int
test_case_6_3_8_top(int fd)
{
	return test_case_6_3_x_top(fd, TSYSERR, EPROTO);
}
static int
test_case_6_3_8_bot(int fd)
{
	return test_case_6_3_x_bot(fd, TSYSERR, EPROTO);
}

#define desc_case_6_4_1 "\
Valid TPI error acknowledgements for all calls\n\
-- t_optmgmt library call\n\
-- TACCES error\n\
This test case tests the TACCES error acknowledgement from the TPI provider\n\
in response to the t_optmgmt library call."
static int
test_case_6_4_x_top(int fd, int terror, int error)
{
	state = 0;
	if (do_signal(fd, __TEST_T_OPTMGMT) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != terror)
		return (FAILURE);
	state = 2;
	if (last_t_errno == TSYSERR && last_errno != error)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_6_4_x_bot(int fd, int terror, int error)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (get_event(fd) != __TEST_OPTMGMT_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = terror;
	last_errno = error;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_6_4_1_top(int fd)
{
	return test_case_6_4_x_top(fd, TACCES, 0);
}
static int
test_case_6_4_1_bot(int fd)
{
	return test_case_6_4_x_bot(fd, TACCES, 0);
}

#define desc_case_6_4_2 "\
Valid TPI error acknowledgements for all calls\n\
-- t_optmgmt library call\n\
-- TBADFLAG error\n\
This test case tests the TBADFLAG error acknowledgement from the TPI provider\n\
in response to the t_optmgmt library call."
static int
test_case_6_4_2_top(int fd)
{
	return test_case_6_4_x_top(fd, TBADFLAG, 0);
}
static int
test_case_6_4_2_bot(int fd)
{
	return test_case_6_4_x_bot(fd, TBADFLAG, 0);
}

#define desc_case_6_4_3 "\
Valid TPI error acknowledgements for all calls\n\
-- t_optmgmt library call\n\
-- TBADOPT error\n\
This test case tests the TBADOPT error acknowledgement from the TPI provider\n\
in response to the t_optmgmt library call."
static int
test_case_6_4_3_top(int fd)
{
	return test_case_6_4_x_top(fd, TBADOPT, 0);
}
static int
test_case_6_4_3_bot(int fd)
{
	return test_case_6_4_x_bot(fd, TBADOPT, 0);
}

#define desc_case_6_4_4 "\
Valid TPI error acknowledgements for all calls\n\
-- t_optmgmt library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error acknowledgement from the TPI provider\n\
in response to the t_optmgmt library call."
static int
test_case_6_4_4_top(int fd)
{
	return test_case_6_4_x_top(fd, TOUTSTATE, 0);
}
static int
test_case_6_4_4_bot(int fd)
{
	return test_case_6_4_x_bot(fd, TOUTSTATE, 0);
}

#define desc_case_6_4_5 "\
Valid TPI error acknowledgements for all calls\n\
-- t_optmgmt library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error acknowledgement from the TPI provider\n\
in response to the t_optmgmt library call."
static int
test_case_6_4_5_top(int fd)
{
	return test_case_6_4_x_top(fd, TNOTSUPPORT, 0);
}
static int
test_case_6_4_5_bot(int fd)
{
	return test_case_6_4_x_bot(fd, TNOTSUPPORT, 0);
}

#define desc_case_6_4_6 "\
Valid TPI error acknowledgements for all calls\n\
-- t_optmgmt library call\n\
-- TSYSERR error\n\
This test case tests the TSYSERR error acknowledgement from the TPI provider\n\
in response to the t_optmgmt library call."
static int
test_case_6_4_6_top(int fd)
{
	return test_case_6_4_x_top(fd, TSYSERR, EPROTO);
}
static int
test_case_6_4_6_bot(int fd)
{
	return test_case_6_4_x_bot(fd, TSYSERR, EPROTO);
}

#define desc_case_6_5_1 "\
Valid TPI error acknowledgements for all calls\n\
-- t_snddis library call\n\
-- TBADDATA error\n\
This test case tests the TBADDATA error acknowledgement from the TPI provider\n\
in response to the t_snddis library call."
static int
test_case_6_5_x_top(int fd, int terror, int error)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SNDDIS) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != terror)
		return (FAILURE);
	state = 2;
	if (last_t_errno == TSYSERR && last_errno != error)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_6_5_x_bot(int fd, int terror, int error)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = terror;
	last_errno = error;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return(SUCCESS);
}
static int
test_case_6_5_1_top(int fd)
{
	return test_case_6_5_x_top(fd, TBADDATA, 0);
}
static int
test_case_6_5_1_bot(int fd)
{
	return test_case_6_5_x_bot(fd, TBADDATA, 0);
}

#define desc_case_6_5_2 "\
Valid TPI error acknowledgements for all calls\n\
-- t_snddis library call\n\
-- TBADSEQ error\n\
This test case tests the TBADSEQ error acknowledgement from the TPI provider\n\
in response to the t_snddis library call."
static int
test_case_6_5_2_top(int fd)
{
	return test_case_6_5_x_top(fd, TBADSEQ, 0);
}
static int
test_case_6_5_2_bot(int fd)
{
	return test_case_6_5_x_bot(fd, TBADSEQ, 0);
}

#define desc_case_6_5_3 "\
Valid TPI error acknowledgements for all calls\n\
-- t_snddis library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error acknowledgement from the TPI provider\n\
in response to the t_snddis library call."
static int
test_case_6_5_3_top(int fd)
{
	return test_case_6_5_x_top(fd, TNOTSUPPORT, 0);
}
static int
test_case_6_5_3_bot(int fd)
{
	return test_case_6_5_x_bot(fd, TNOTSUPPORT, 0);
}

#define desc_case_6_5_4 "\
Valid TPI error acknowledgements for all calls\n\
-- t_snddis library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error acknowledgement from the TPI provider\n\
in response to the t_snddis library call."
static int
test_case_6_5_4_top(int fd)
{
	return test_case_6_5_x_top(fd, TOUTSTATE, 0);
}
static int
test_case_6_5_4_bot(int fd)
{
	return test_case_6_5_x_bot(fd, TOUTSTATE, 0);
}

#define desc_case_6_5_5 "\
Valid TPI error acknowledgements for all calls\n\
-- t_snddis library call\n\
-- TSYSERR error\n\
This test case tests the TSYSERR error acknowledgement from the TPI provider\n\
in response to the t_snddis library call."
static int
test_case_6_5_5_top(int fd)
{
	return test_case_6_5_x_top(fd, TSYSERR, EPROTO);
}
static int
test_case_6_5_5_bot(int fd)
{
	return test_case_6_5_x_bot(fd, TSYSERR, EPROTO);
}

#define desc_case_6_6_1 "\
Valid TPI error acknowledgements for all calls\n\
-- t_unbind library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error acknowledgement from the TPI provider\n\
in response to the t_unbind library call."
static int
test_case_6_6_x_top(int fd, int terror, int error)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (do_signal(fd, __TEST_T_UNBIND) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != terror)
		return (FAILURE);
	state = 3;
	if (last_t_errno == TSYSERR && last_errno != error)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}
static int
test_case_6_6_x_bot(int fd, int terror, int error)
{
	state = 0;
	start_tt(500);
	state = 1;
	if (get_event(fd) != __TEST_UNBIND_REQ)
		return (FAILURE);
	state = 2;
	last_t_errno = terror;
	last_errno = error;
	if (do_signal(fd, __TEST_ERROR_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_6_6_1_top(int fd)
{
	return test_case_6_6_x_top(fd, TOUTSTATE, 0);
}
static int
test_case_6_6_1_bot(int fd)
{
	return test_case_6_6_x_bot(fd, TOUTSTATE, 0);
}

#define desc_case_6_6_2 "\
Valid TPI error acknowledgements for all calls\n\
-- t_unbind library call\n\
-- TSYSERR error\n\
This test case tests the TSYSERR error acknowledgement from the TPI provider\n\
in response to the t_unbind library call."
static int
test_case_6_6_2_top(int fd)
{
	return test_case_6_6_x_top(fd, TSYSERR, EPROTO);
}
static int
test_case_6_6_2_bot(int fd)
{
	return test_case_6_6_x_bot(fd, TSYSERR, EPROTO);
}

#define desc_case_7_1_1 "\
Error codes returned by the XTI library\n\
-- t_accept library call\n\
-- TINDOUT error\n\
This test case tests the TINDOUT error from the XTI library in response to\n\
the t_accept library call."
static int
test_case_7_1_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_ACCEPT) == SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_t_errno != TINDOUT)
		return (FAILURE);
	state = 4;
	last_sequence = 1;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 5;
	last_sequence = 2;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}
static int
test_case_7_1_1_bot(int fd)
{
	state = 0;
	last_sequence = 1;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	last_sequence = 2;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}

#define desc_case_7_1_2 "\
Error codes returned by the XTI library\n\
-- t_accept library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_accept library call."
static int
test_case_7_1_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_ACCEPT) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_7_1_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_1_3 "\
Error codes returned by the XTI library\n\
-- t_accept library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_accept library call."
static int
test_case_7_1_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_ACCEPT) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_7_1_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_2_1 "\
Error codes returned by the XTI library\n\
-- t_connect library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_connect library call."
static int
test_case_7_2_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_7_2_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_3_1 "\
Error codes returned by the XTI library\n\
-- t_listen library call\n\
-- TBADQLEN error\n\
This test case tests the TBADQLEN error from the XTI library in response to\n\
the t_listen library call."
static int
test_case_7_3_1_top(int fd)
{
	state = 0;
	last_qlen = 0;
	if (do_signal(fd, __TEST_T_BIND) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_LISTEN) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TBADQLEN)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_UNBIND) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}
static int
test_case_7_3_1_bot(int fd)
{
	state = 0;
	if (get_event(fd) != __TEST_BIND_REQ)
		return (FAILURE);
	state = 1;
	last_qlen = 0;
	if (do_signal(fd, __TEST_BIND_ACK) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (get_event(fd) != __TEST_UNBIND_REQ)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_7_3_2 "\
Error codes returned by the XTI library\n\
-- t_listen library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_listen library call."
static int
test_case_7_3_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_LISTEN) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_7_3_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_3_3 "\
Error codes returned by the XTI library\n\
-- t_listen library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_listen library call."
static int
test_case_7_3_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_LISTEN) == SUCCESS)
		return (FAILURE);
	state = 1;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 2;
	return (SUCCESS);
}
static int
test_case_7_3_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_3_4 "\
Error codes returned by the XTI library\n\
-- t_listen library call\n\
-- TQFULL error\n\
This test case tests the TQFULL error from the XTI library in response to\n\
the t_listen library call."
static int
test_case_7_3_4_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_LISTEN) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_LISTEN) == SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_t_errno != TQFULL)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}
static int
test_case_7_3_4_bot(int fd)
{
	state = 0;
	last_sequence = 1;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 1;
	last_sequence = 2;
	if (do_signal(fd, __TEST_CONN_IND) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 4;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}

#define desc_case_7_3_5 "\
Error codes returned by the XTI library\n\
-- t_listen library call\n\
-- TNODATA error\n\
This test case tests the TNODATA error from the XTI library in response to\n\
the t_listen library call."
static int
test_case_7_3_5_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_LISTEN) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_3_5_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_4_1 "\
Error codes returned by the XTI library\n\
-- t_rcv library call\n\
-- TNODATA error\n\
This test case tests the TNODATA error from the XTI library in response to\n\
the t_rcv library call."
static int
test_case_7_4_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_4_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_4_2 "\
Error codes returned by the XTI library\n\
-- t_rcv library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcv library call."
static int
test_case_7_4_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_4_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_4_3 "\
Error codes returned by the XTI library\n\
-- t_rcv library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcv library call."
static int
test_case_7_4_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_4_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_5_1 "\
Error codes returned by the XTI library\n\
-- t_rcvv library call\n\
-- TNODATA error\n\
This test case tests the TNODATA error from the XTI library in response to\n\
the t_rcvv library call."
static int
test_case_7_5_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_5_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_5_2 "\
Error codes returned by the XTI library\n\
-- t_rcvv library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvv library call."
static int
test_case_7_5_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_5_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_5_3 "\
Error codes returned by the XTI library\n\
-- t_rcvv library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvv library call."
static int
test_case_7_5_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_5_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_6_1 "\
Error codes returned by the XTI library\n\
-- t_rcvconnect library call\n\
-- TNODATA error\n\
This test case tests the TNODATA error from the XTI library in response to\n\
the t_rcvconnect library call."
static int
test_case_7_6_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_CONNECT) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_T_RCVCONNECT) == SUCCESS)
		return (FAILURE);
	state = 4;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_SNDDIS) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}
static int
test_case_7_6_1_bot(int fd)
{
	state = 0;
	if (get_event(fd) != __TEST_CONN_REQ)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (get_event(fd) != __TEST_DISCON_REQ)
		return (FAILURE);
	state = 3;
	if (do_signal(fd, __TEST_OK_ACK) != SUCCESS)
		return (FAILURE);
	state = 4;
	return (SUCCESS);
}

#define desc_case_7_6_2 "\
Error codes returned by the XTI library\n\
-- t_rcvconnect library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvconnect library call."
static int
test_case_7_6_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVCONNECT) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_6_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_6_3 "\
Error codes returned by the XTI library\n\
-- t_rcvconnect library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvconnect library call."
static int
test_case_7_6_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVCONNECT) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_6_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_7_1 "\
Error codes returned by the XTI library\n\
-- t_rcvdis library call\n\
-- TNODIS error\n\
This test case tests the TNODIS error from the XTI library in response to\n\
the t_rcvdis library call."
static int
test_case_7_7_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVDIS) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODIS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_7_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_7_2 "\
Error codes returned by the XTI library\n\
-- t_rcvdis library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvdis library call."
static int
test_case_7_7_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVDIS) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_7_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_7_3 "\
Error codes returned by the XTI library\n\
-- t_rcvdis library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvdis library call."
static int
test_case_7_7_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVDIS) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_7_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_8_1 "\
Error codes returned by the XTI library\n\
-- t_rcvrel library call\n\
-- TNOREL error\n\
This test case tests the TNOREL error from the XTI library in response to\n\
the t_rcvrel library call."
static int
test_case_7_8_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVREL) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOREL)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_8_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_8_2 "\
Error codes returned by the XTI library\n\
-- t_rcvrel library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvrel library call."
static int
test_case_7_8_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVREL) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_8_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_8_3 "\
Error codes returned by the XTI library\n\
-- t_rcvrel library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvrel library call."
static int
test_case_7_8_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVREL) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_8_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_9_1 "\
Error codes returned by the XTI library\n\
-- t_rcvreldata library call\n\
-- TNOREL error\n\
This test case tests the TNOREL error from the XTI library in response to\n\
the t_rcvreldata library call."
static int
test_case_7_9_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVRELDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOREL)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_9_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_9_2 "\
Error codes returned by the XTI library\n\
-- t_rcvreldata library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvreldata library call."
static int
test_case_7_9_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVRELDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_9_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_9_3 "\
Error codes returned by the XTI library\n\
-- t_rcvreldata library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvreldata library call."
static int
test_case_7_9_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVRELDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_9_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_10_1 "\
Error codes returned by the XTI library\n\
-- t_rcvudata library call\n\
-- TNODATA error\n\
This test case tests the TNODATA error from the XTI library in response to\n\
the t_rcvudata library call."
static int
test_case_7_10_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_10_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_10_2 "\
Error codes returned by the XTI library\n\
-- t_rcvudata library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvudata library call."
static int
test_case_7_10_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_10_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_10_3 "\
Error codes returned by the XTI library\n\
-- t_rcvudata library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvudata library call."
static int
test_case_7_10_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_10_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_11_1 "\
Error codes returned by the XTI library\n\
-- t_rcvvudata library call\n\
-- TNODATA error\n\
This test case tests the TNODATA error from the XTI library in response to\n\
the t_rcvvudata library call."
static int
test_case_7_11_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNODATA)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_11_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_11_2 "\
Error codes returned by the XTI library\n\
-- t_rcvvudata library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvvudata library call."
static int
test_case_7_11_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_11_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_11_3 "\
Error codes returned by the XTI library\n\
-- t_rcvvudata library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvvudata library call."
static int
test_case_7_11_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_11_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_12_1 "\
Error codes returned by the XTI library\n\
-- t_rcvuderr library call\n\
-- TNOUDERR error\n\
This test case tests the TNOUDERR error from the XTI library in response to\n\
the t_rcvuderr library call."
static int
test_case_7_12_1_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 1;
	if (do_signal(fd, __TEST_T_RCVUDERR) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOUDERR)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_12_1_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_12_2 "\
Error codes returned by the XTI library\n\
-- t_rcvuderr library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_rcvuderr library call."
static int
test_case_7_12_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVUDERR) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_12_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_12_3 "\
Error codes returned by the XTI library\n\
-- t_rcvuderr library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_rcvuderr library call."
static int
test_case_7_12_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_RCVUDERR) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_12_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_13_1 "\
Error codes returned by the XTI library\n\
-- t_snd library call\n\
-- TFLOW error\n\
This test case tests the TFLOW error from the XTI library in response to\n\
the t_snd library call."
static int
test_case_7_13_1_top(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SND) != SUCCESS)
		return (FAILURE);
	state = 3;
	show_data = 0;
	while (do_signal(fd, __TEST_T_SND) == SUCCESS) ;
	show_data = 1;
	state = 4;
	if (last_t_errno != TFLOW)
		return (FAILURE);
	state = 5;
	start_tt(6000);
	state = 6;
	pause();
	state = 7;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_7_13_1_bot(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	for (;;) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case NO_MSG:
			show_data = 0;
			state = 2;
			continue;
		case TIMEOUT:
			state = 3;
			show_data = 1;
			break;
		default:
			state = 4;
			show_data = 1;
			return (FAILURE);
		}
		break;
	}
	state = 5;
	return (SUCCESS);
}

#define desc_case_7_13_2 "\
Error codes returned by the XTI library\n\
-- t_snd library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_snd library call."
static int
test_case_7_13_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SND) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_13_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_13_3 "\
Error codes returned by the XTI library\n\
-- t_snd library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_snd library call."
static int
test_case_7_13_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SND) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_13_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_14_1 "\
Error codes returned by the XTI library\n\
-- t_sndv library call\n\
-- TFLOW error\n\
This test case tests the TFLOW error from the XTI library in response to\n\
the t_sndv library call."
static int
test_case_7_14_1_top(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SNDV) != SUCCESS)
		return (FAILURE);
	state = 3;
	show_data = 0;
	while (do_signal(fd, __TEST_T_SNDV) == SUCCESS) ;
	show_data = 1;
	state = 4;
	if (last_t_errno != TFLOW)
		return (FAILURE);
	state = 5;
	start_tt(6000);
	state = 6;
	pause();
	state = 7;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_7_14_1_bot(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	for (;;) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case NO_MSG:
			show_data = 0;
			state = 2;
			continue;
		case TIMEOUT:
			state = 3;
			show_data = 1;
			break;
		default:
			state = 4;
			show_data = 1;
			return (FAILURE);
		}
		break;
	}
	state = 5;
	return (SUCCESS);
}

#define desc_case_7_14_2 "\
Error codes returned by the XTI library\n\
-- t_sndv library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_sndv library call."
static int
test_case_7_14_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SNDV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_14_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_14_3 "\
Error codes returned by the XTI library\n\
-- t_sndv library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_sndv library call."
static int
test_case_7_14_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SNDV) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_14_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_15_1 "\
Error codes returned by the XTI library\n\
-- t_sndudata library call\n\
-- TFLOW error\n\
This test case tests the TFLOW error from the XTI library in response to\n\
the t_sndudata library call."
static int
test_case_7_15_1_top(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SNDUDATA) != SUCCESS)
		return (FAILURE);
	state = 3;
	show_data = 0;
	while (do_signal(fd, __TEST_T_SNDUDATA) == SUCCESS) ;
	show_data = 1;
	state = 4;
	if (last_t_errno != TFLOW)
		return (FAILURE);
	state = 5;
	start_tt(6000);
	state = 6;
	pause();
	state = 7;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_7_15_1_bot(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	for (;;) {
		switch (get_event(fd)) {
		case __TEST_UNITDATA_REQ:
		case NO_MSG:
			show_data = 0;
			state = 2;
			continue;
		case TIMEOUT:
			state = 3;
			show_data = 1;
			break;
		default:
			state = 4;
			show_data = 1;
			return (FAILURE);
		}
		break;
	}
	state = 5;
	return (SUCCESS);
}

#define desc_case_7_15_2 "\
Error codes returned by the XTI library\n\
-- t_sndudata library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_sndudata library call."
static int
test_case_7_15_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SNDUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_15_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_15_3 "\
Error codes returned by the XTI library\n\
-- t_sndudata library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_sndudata library call."
static int
test_case_7_15_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SNDUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_15_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_16_1 "\
Error codes returned by the XTI library\n\
-- t_sndvudata library call\n\
-- TFLOW error\n\
This test case tests the TFLOW error from the XTI library in response to\n\
the t_sndvudata library call."
static int
test_case_7_16_1_top(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	if (do_signal(fd, __TEST_O_NONBLOCK) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SNDVUDATA) != SUCCESS)
		return (FAILURE);
	state = 3;
	show_data = 0;
	while (do_signal(fd, __TEST_T_SNDVUDATA) == SUCCESS) ;
	show_data = 1;
	state = 4;
	if (last_t_errno != TFLOW)
		return (FAILURE);
	state = 5;
	start_tt(6000);
	state = 6;
	pause();
	state = 7;
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state = 8;
	return (SUCCESS);
}
static int
test_case_7_16_1_bot(int fd)
{
	state = 0;
	start_tt(5000);
	state = 1;
	for (;;) {
		switch (get_event(fd)) {
		case __TEST_UNITDATA_REQ:
		case NO_MSG:
			show_data = 0;
			state = 2;
			continue;
		case TIMEOUT:
			state = 3;
			show_data = 1;
			break;
		default:
			state = 4;
			show_data = 1;
			return (FAILURE);
		}
		break;
	}
	state = 5;
	return (SUCCESS);
}

#define desc_case_7_16_2 "\
Error codes returned by the XTI library\n\
-- t_sndvudata library call\n\
-- TNOTSUPPORT error\n\
This test case tests the TNOTSUPPORT error from the XTI library in response to\n\
the t_sndvudata library call."
static int
test_case_7_16_2_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SNDVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TNOTSUPPORT)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_16_2_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_16_3 "\
Error codes returned by the XTI library\n\
-- t_sndvudata library call\n\
-- TOUTSTATE error\n\
This test case tests the TOUTSTATE error from the XTI library in response to\n\
the t_sndvudata library call."
static int
test_case_7_16_3_top(int fd)
{
	state = 0;
	if (do_signal(fd, __TEST_T_SNDVUDATA) == SUCCESS)
		return (FAILURE);
	state = 2;
	if (last_t_errno != TOUTSTATE)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}
static int
test_case_7_16_3_bot(int fd)
{
	state = 0;
	return (SUCCESS);
}

#define desc_case_7_17_1 "\
Error codes returned by the XTI library\n\
-- t_sync library call\n\
-- TSTATECHNG error\n\
This test case tests the TSTATECHNG error from the XTI library in response to\n\
the t_sync library call."
static int
test_case_7_17_1_top(int fd)
{
	state = 0;
	start_tt(1000);
	state = 1;
	if (do_signal(fd, __TEST_PUSH) != SUCCESS)
		return (FAILURE);
	state = 2;
	if (do_signal(fd, __TEST_T_SYNC) == SUCCESS)
		return (FAILURE);
	state = 3;
	if (last_t_errno != TSTATECHNG)
		return (FAILURE);
	state = 4;
	if (do_signal(fd, __TEST_POP) != SUCCESS)
		return (FAILURE);
	state = 5;
	if (do_signal(fd, __TEST_T_CLOSE) != SUCCESS)
		return (FAILURE);
	state = 6;
	return (SUCCESS);
}
static int
test_case_7_17_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	state = 1;
	if (get_event(fd) != __TEST_CAPABILITY_REQ)
		return (FAILURE);
	state = 2;
	last_tstate = TS_WACK_BREQ;
	if (do_signal(fd, __TEST_CAPABILITY_ACK) != SUCCESS)
		return (FAILURE);
	state = 3;
	return (SUCCESS);
}

#define desc_case_8_1_1 "\
Passing data\n\
-- receiving data with t_rcv\n\
-- normal data\n\
This test case tests positive test cases for the t_rcv library call."
static int
test_case_8_1_1_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCV)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_1_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		MORE_flag = (state < 19) ? 1 : 0;
		if (do_signal(fd, __TEST_DATA_IND) != SUCCESS) {
			MORE_flag = 0;
			return (FAILURE);
		}
	}
	MORE_flag = 0;
	state++;
	return (SUCCESS);
}

#define desc_case_8_1_2 "\
Passing data\n\
-- receiving data with t_rcv\n\
-- expedited data\n\
This test case tests positive test cases for the t_rcv library call."
static int
test_case_8_1_2_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCV)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_1_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		MORE_flag = (state < 19) ? 1 : 0;
		if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
			MORE_flag = 0;
			return (FAILURE);
		}
	}
	MORE_flag = 0;
	state++;
	return (SUCCESS);
}

#define desc_case_8_1_3 "\
Passing data\n\
-- receiving data with t_rcv\n\
-- normal data with intermingled expedited data\n\
This test case tests positive test cases for the t_rcv library call."
static int
test_case_8_1_3_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCV)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_1_3_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		MORE_flag = (state < 19) ? 1 : 0;
		if (do_signal(fd, __TEST_DATA_IND) != SUCCESS) {
			MORE_flag = 0;
			return (FAILURE);
		}
		if (state == 10) {
			start_tt(200);
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			MORE_flag = 1;
			if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
				MORE_flag = 0;
				return (FAILURE);
			}
		}
		if (state == 15) {
			start_tt(200);
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			MORE_flag = 0;
			if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
				MORE_flag = 0;
				return (FAILURE);
			}
		}
	}
	MORE_flag = 0;
	state++;
	return (SUCCESS);
}

#define desc_case_8_1_4 "\
Passing data\n\
-- receiving data with t_rcv\n\
-- normal data with intermingled expedited data\n\
-- receive buffer smaller that received data\n\
This test case tests positive test cases for the t_rcv library call."
static int
test_case_8_1_4_top(int fd)
{
	test_bufsize = 12;
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCV)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT) {
				test_bufsize = 256;
				return (FAILURE);
			}
			break;
		}
		break;
	}
	state++;
	test_bufsize = 256;
	return (SUCCESS);
}
static int
test_case_8_1_4_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		MORE_flag = (state < 19) ? 1 : 0;
		if (do_signal(fd, __TEST_DATA_IND) != SUCCESS) {
			MORE_flag = 0;
			return (FAILURE);
		}
		if (state == 10) {
			start_tt(200);
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			MORE_flag = 1;
			if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
				MORE_flag = 0;
				return (FAILURE);
			}
		}
		if (state == 15) {
			start_tt(200);
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			MORE_flag = 0;
			if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
				MORE_flag = 0;
				return (FAILURE);
			}
		}
	}
	MORE_flag = 0;
	state++;
	return (SUCCESS);
}

#define desc_case_8_2_1 "\
Passing data\n\
-- receiving data with t_rcvv\n\
-- normal data\n\
This test case tests positive test cases for the t_rcvv library call."
static int
test_case_8_2_1_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCVV)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_2_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		MORE_flag = (state < 19) ? 1 : 0;
		if (do_signal(fd, __TEST_DATA_IND) != SUCCESS) {
			MORE_flag = 0;
			return (FAILURE);
		}
	}
	MORE_flag = 0;
	state++;
	return (SUCCESS);
}

#define desc_case_8_2_2 "\
Passing data\n\
-- receiving data with t_rcvv\n\
-- expedited data\n\
This test case tests positive test cases for the t_rcvv library call."
static int
test_case_8_2_2_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCVV)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_2_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		MORE_flag = (state < 19) ? 1 : 0;
		if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
			MORE_flag = 0;
			return (FAILURE);
		}
	}
	MORE_flag = 0;
	state++;
	return (SUCCESS);
}

#define desc_case_8_2_3 "\
Passing data\n\
-- receiving data with t_rcvv\n\
-- normal data with intermingled expedited data\n\
This test case tests positive test cases for the t_rcvv library call."
static int
test_case_8_2_3_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCVV)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_2_3_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		MORE_flag = (state < 19) ? 1 : 0;
		if (do_signal(fd, __TEST_DATA_IND) != SUCCESS) {
			MORE_flag = 0;
			return (FAILURE);
		}
		if (state == 10) {
			start_tt(200);
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			MORE_flag = 1;
			if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
				MORE_flag = 0;
				return (FAILURE);
			}
		}
		if (state == 15) {
			start_tt(200);
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			MORE_flag = 0;
			if (do_signal(fd, __TEST_EXDATA_IND) != SUCCESS) {
				MORE_flag = 0;
				return (FAILURE);
			}
		}
	}
	MORE_flag = 0;
	state++;
	return (SUCCESS);
}

#define desc_case_8_3_1 "\
Passing data\n\
-- receiving data with t_rcvudata\n\
-- within TIDU sized data\n\
This test case tests positive test cases for the t_rcvudata library call."
static int
test_case_8_3_1_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCVUDATA)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_3_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		if (do_signal(fd, __TEST_UNITDATA_IND) != SUCCESS)
			return (FAILURE);
	}
	state++;
	return (SUCCESS);
}

#define desc_case_8_3_2 "\
Passing data\n\
-- receiving data with t_rcvudata\n\
-- within TIDU sized data\n\
-- receive buffer smaller than received data size\n\
This test case tests positive test cases for the t_rcvudata library call."
static int
test_case_8_3_2_top(int fd)
{
	test_rcvudata.udata.maxlen = 12;
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCVUDATA)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT) {
				test_rcvudata.udata.maxlen = 256;
				return (FAILURE);
			}
			break;
		}
		break;
	}
	state++;
	test_rcvudata.udata.maxlen = 256;
	return (SUCCESS);
}
static int
test_case_8_3_2_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		if (do_signal(fd, __TEST_UNITDATA_IND) != SUCCESS)
			return (FAILURE);
	}
	state++;
	return (SUCCESS);
}

#define desc_case_8_4_1 "\
Passing data\n\
-- receiving data with t_rcvvudata\n\
-- within TIDU sized data\n\
This test case tests positive test cases for the t_rcvvudata library call."
static int
test_case_8_4_1_top(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (do_signal(fd, __TEST_T_RCVVUDATA)) {
		case SUCCESS:
			continue;
		case FAILURE:
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			break;
		}
		break;
	}
	state++;
	return (SUCCESS);
}
static int
test_case_8_4_1_bot(int fd)
{
	state = 0;
	start_tt(200);
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		if (do_signal(fd, __TEST_UNITDATA_IND) != SUCCESS)
			return (FAILURE);
	}
	state++;
	return (SUCCESS);
}


#define desc_case_8_5_1 "\
Passing data\n\
-- sending data with t_snd\n\
-- normal data\n\
This test case tests positive test cases for the t_snd library call."
static int
test_case_8_5_1_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		test_sndflags = (state < 19) ? T_MORE : 0;
		if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
			test_sndflags = 0;
			return (FAILURE);
		}
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_5_1_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case __TEST_EXDATA_REQ:
			continue;
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

#define desc_case_8_5_2 "\
Passing data\n\
-- sending data with t_snd\n\
-- expedited data\n\
This test case tests positive test cases for the t_snd library call."
static int
test_case_8_5_2_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		test_sndflags = ((state < 19) ? T_MORE : 0) | T_EXPEDITED;
		if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
			test_sndflags = T_EXPEDITED;
			return (FAILURE);
		}
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_5_2_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case __TEST_EXDATA_REQ:
			continue;
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

#define desc_case_8_5_3 "\
Passing data\n\
-- sending data with t_snd\n\
-- expedited data intermingled with normal data\n\
This test case tests positive test cases for the t_snd library call."
static int
test_case_8_5_3_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		test_sndflags = ((state < 19) ? T_MORE : 0);
		if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
			test_sndflags = 0;
			return (FAILURE);
		}
		if (state == 10) {
			start_tt(200);
			pause();
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			test_sndflags = T_MORE|T_EXPEDITED;
			if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
				test_sndflags = 0;
				return (FAILURE);
			}
		}
		if (state == 15) {
			start_tt(200);
			pause();
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			test_sndflags = T_EXPEDITED;
			if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
				test_sndflags = 0;
				return (FAILURE);
			}
		}
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_5_3_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case __TEST_EXDATA_REQ:
			continue;
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

#define desc_case_8_5_4 "\
Passing data\n\
-- sending data with t_snd\n\
-- expedited data intermingled with normal data\n\
-- data larget than TIDU size\n\
This test case tests positive test cases for the t_snd library call."
static int
test_case_8_5_4_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		test_sndflags = ((state < 19) ? T_MORE : 0);
		if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
			test_sndflags = 0;
			return (FAILURE);
		}
		if (state == 10) {
			start_tt(200);
			pause();
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			test_sndflags = T_MORE|T_EXPEDITED;
			if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
				test_sndflags = 0;
				return (FAILURE);
			}
		}
		if (state == 15) {
			start_tt(200);
			pause();
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			test_sndflags = T_EXPEDITED;
			if (do_signal(fd, __TEST_T_SND) != SUCCESS) {
				test_sndflags = 0;
				return (FAILURE);
			}
		}
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_5_4_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case __TEST_EXDATA_REQ:
			continue;
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

#define desc_case_8_6_1 "\
Passing data\n\
-- sending data with t_sndv\n\
-- normal data\n\
This test case tests positive test cases for the t_sndv library call."
static int
test_case_8_6_1_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		test_sndflags = (state < 19) ? T_MORE : 0;
		if (do_signal(fd, __TEST_T_SNDV) != SUCCESS) {
			test_sndflags = 0;
			return (FAILURE);
		}
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_6_1_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case __TEST_EXDATA_REQ:
			continue;
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

#define desc_case_8_6_2 "\
Passing data\n\
-- sending data with t_sndv\n\
-- expedited data\n\
This test case tests positive test cases for the t_sndv library call."
static int
test_case_8_6_2_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		test_sndflags = ((state < 19) ? T_MORE : 0) | T_EXPEDITED;
		if (do_signal(fd, __TEST_T_SNDV) != SUCCESS) {
			test_sndflags = T_EXPEDITED;
			return (FAILURE);
		}
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_6_2_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case __TEST_EXDATA_REQ:
			continue;
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

#define desc_case_8_6_3 "\
Passing data\n\
-- sending data with t_sndv\n\
-- expedited data intermingled with normal data\n\
This test case tests positive test cases for the t_sndv library call."
static int
test_case_8_6_3_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		test_sndflags = ((state < 19) ? T_MORE : 0);
		if (do_signal(fd, __TEST_T_SNDV) != SUCCESS) {
			test_sndflags = 0;
			return (FAILURE);
		}
		if (state == 10) {
			start_tt(200);
			pause();
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			test_sndflags = T_MORE|T_EXPEDITED;
			if (do_signal(fd, __TEST_T_SNDV) != SUCCESS) {
				test_sndflags = 0;
				return (FAILURE);
			}
		}
		if (state == 15) {
			start_tt(200);
			pause();
			if (get_event(fd) != TIMEOUT)
				return (FAILURE);
			test_sndflags = T_EXPEDITED;
			if (do_signal(fd, __TEST_T_SNDV) != SUCCESS) {
				test_sndflags = 0;
				return (FAILURE);
			}
		}
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_6_3_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_DATA_REQ:
		case __TEST_EXDATA_REQ:
			continue;
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

#define desc_case_8_7_1 "\
Passing data\n\
-- sending data with t_sndudata\n\
-- within TIDU sized data\n\
This test case tests positive test cases for the t_sndudata library call."
static int
test_case_8_7_1_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		if (do_signal(fd, __TEST_T_SNDUDATA) != SUCCESS)
			return (FAILURE);
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_7_1_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_UNITDATA_REQ:
			continue;
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

#define desc_case_8_8_1 "\
Passing data\n\
-- sending data with t_sndvudata\n\
-- within TIDU sized data\n\
This test case tests positive test cases for the t_sndvudata library call."
static int
test_case_8_8_1_top(int fd)
{
	state = 0;
	start_tt(200);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	for (state = 0; state < 20; state++) {
		if (do_signal(fd, __TEST_T_SNDVUDATA) != SUCCESS)
			return (FAILURE);
	}
	state++;
	start_tt(1000);
	pause();
	if (get_event(fd) != TIMEOUT)
		return (FAILURE);
	state++;
	return (SUCCESS);
}
static int
test_case_8_8_1_bot(int fd)
{
	for (state = 0, start_tt(500);; start_tt(500), state++) {
		switch (get_event(fd)) {
		case __TEST_UNITDATA_REQ:
			continue;
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
		fprintf(stdout, "--I_SRDOPT--------->|  |                            |  |                    {%d}\n", state);
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
		fprintf(stdout, "--I_PUSH----------->|  |                            |  |                    {%d}\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (ioctl(top_fd, I_PUSH, "timod") < 0) {
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
		fprintf(stdout, "--I_POP------------>|  |                            |  |                    {%d}\n", state);
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
	int got_stat, top_stat = SUCCESS, bot_stat = SUCCESS;
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
	}
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
	}
	for (; children > 0; children--) {
		if ((got_chld = wait(&got_stat)) > 0) {
			if (WIFEXITED(got_stat)) {
				int status = WEXITSTATUS(got_stat);
				if (got_chld == top_chld) {
					top_stat = status;
					top_chld = 0;
					if (status != SUCCESS && bot_chld)
						kill(bot_chld, SIGKILL);
				}
				if (got_chld == bot_chld) {
					bot_stat = status;
					bot_chld = 0;
					if (status != SUCCESS && top_chld)
						kill(top_chld, SIGKILL);
				}
			} else if (WIFSIGNALED(got_stat)) {
				int signal = WTERMSIG(got_stat);
				if (got_chld == top_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "@@@@@@@@@@@@@@@@@@@@|  |@@@@@@@ TERMINATED %02d @@@@@@@@@@|                   {%d}\n", signal, state);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (bot_chld)
						kill(bot_chld, SIGKILL);
					top_stat = signal == SIGKILL ? INCONCLUSIVE : FAILURE;
					top_chld = 0;
				}
				if (got_chld == bot_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |  |@@@@@@@ TERMINATED %02d @@@@@@@@@@|@@@@@@@@@@@@@@@@@@@{%d}\n", signal, state);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (top_chld)
						kill(bot_chld, SIGKILL);
					bot_stat = signal == SIGKILL ? INCONCLUSIVE : FAILURE;
					bot_chld = 0;
				}
			} else if (WIFSTOPPED(got_stat)) {
				int signal = WSTOPSIG(got_stat);
				if (got_chld == top_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "&&&&&&&&&&&&&&&&&&&&|  |&&&&&&& STOPPED %02d &&&&&&&&&&&&&|                   {%d}\n", signal, state);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (bot_chld)
						kill(bot_chld, SIGKILL);
					kill(top_chld, SIGKILL);
				}
				if (got_chld == bot_chld) {
					if (verbose) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |  |&&&&&&& STOPPED %02d &&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&&{%d}\n", signal, state);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (top_chld)
						kill(top_chld, SIGKILL);
					kill(bot_chld, SIGKILL);
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
				top_stat = INCONCLUSIVE;
				top_chld = 0;
			}
			if (bot_chld) {
				kill(bot_chld, SIGKILL);
				bot_stat = INCONCLUSIVE;
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
		"1.2.1", desc_case_1_2_1, {
		&preamble_0, &test_case_1_2_1_top, &postamble_0}, {
	&preamble_0, &test_case_1_2_1_bot, &postamble_0}, 0, 0}, {
		"1.2.2", desc_case_1_2_2, {
		&preamble_0, &test_case_1_2_2_top, &postamble_0}, {
	&preamble_0, &test_case_1_2_2_bot, &postamble_0}, 0, 0}, {
		"1.2.3", desc_case_1_2_3, {
		&preamble_0, &test_case_1_2_3_top, &postamble_0}, {
	&preamble_0, &test_case_1_2_3_bot, &postamble_0}, 0, 0}, {
		"2.1.1", desc_case_2_1_1, {
		&preamble_1_top, &test_case_2_1_1_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_1_bot, &postamble_1_bot}, 0, 0}, {
		"2.1.2", desc_case_2_1_2, {
		&preamble_1_top, &test_case_2_1_2_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_2_bot, &postamble_1_bot}, 0, 0}, {
		"2.1.3", desc_case_2_1_3, {
		&preamble_1_top, &test_case_2_1_3_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_3_bot, &postamble_1_bot}, 0, 0}, {
		"2.1.4", desc_case_2_1_4, {
		&preamble_1_top, &test_case_2_1_4_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_4_bot, &postamble_1_bot}, 0, 0}, {
		"2.1.5", desc_case_2_1_5, {
		&preamble_1_top, &test_case_2_1_5_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_5_bot, &postamble_1_bot}, 0, 0}, {
		"2.1.6", desc_case_2_1_6, {
		&preamble_1_top, &test_case_2_1_6_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_6_bot, &postamble_1_bot}, 0, 0}, {
		"2.1.7", desc_case_2_1_7, {
		&preamble_1_top, &test_case_2_1_7_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_7_bot, &postamble_1_bot}, 0, 0}, {
		"2.1.8", desc_case_2_1_8, {
		&preamble_1_top, &test_case_2_1_8_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_1_8_bot, &postamble_1_bot}, 0, 0}, {
		"2.2", desc_case_2_2, {
		&preamble_1_top, &test_case_2_2_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_2_2_bot, &postamble_1_bot}, 0, 0}, {
		"2.3.1", desc_case_2_3_1, {
		&preamble_2_top, &test_case_2_3_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_1_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.2", desc_case_2_3_2, {
		&preamble_2_top, &test_case_2_3_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_2_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.3", desc_case_2_3_3, {
		&preamble_2_top, &test_case_2_3_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_3_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.4", desc_case_2_3_4, {
		&preamble_2_top, &test_case_2_3_4_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_4_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.5", desc_case_2_3_5, {
		&preamble_2_top, &test_case_2_3_5_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_5_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.6", desc_case_2_3_6, {
		&preamble_2_top, &test_case_2_3_6_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_6_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.7", desc_case_2_3_7, {
		&preamble_2_top, &test_case_2_3_7_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_7_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.8", desc_case_2_3_8, {
		&preamble_2_top, &test_case_2_3_8_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_8_bot, &postamble_2_bot}, 0, 0}, {
		"2.3.9", desc_case_2_3_9, {
		&preamble_2_top, &test_case_2_3_9_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_3_9_bot, &postamble_2_bot}, 0, 0}, {
		"2.4.1", desc_case_2_4_1, {
		&preamble_2_top, &test_case_2_4_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_4_1_bot, &postamble_2_bot}, 0, 0}, {
		"2.5.1", desc_case_2_5_1, {
		&preamble_2_top, &test_case_2_5_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_2_5_1_bot, &postamble_2_bot}, 0, 0}, {
		"3.1.1", desc_case_3_1_1, {
		&preamble_2_top, &test_case_3_1_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_3_1_1_bot, &postamble_2_bot}, 0, 0}, {
		"3.2.1", desc_case_3_2_1, {
		&preamble_2_top, &test_case_3_2_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_3_2_1_bot, &postamble_2_bot}, 0, 0}, {
		"3.3.1", desc_case_3_3_1, {
		&preamble_2_top, &test_case_3_3_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_3_3_1_bot, &postamble_2_bot}, 0, 0}, {
		"3.4.1", desc_case_3_4_1, {
		&preamble_2_top, &test_case_3_4_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_3_4_1_bot, &postamble_2_bot}, 0, 0}, {
		"3.4.2", desc_case_3_4_2, {
		&preamble_2_top, &test_case_3_4_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_3_4_2_bot, &postamble_2_bot}, 0, 0}, {
		"3.5.1", desc_case_3_5_1, {
		&preamble_3_top, &test_case_3_5_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_3_5_1_bot, &postamble_3_bot}, 0, 0}, {
		"3.5.2", desc_case_3_5_2, {
		&preamble_3_top, &test_case_3_5_2_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_3_5_2_bot, &postamble_3_bot}, 0, 0}, {
		"3.6.1", desc_case_3_6_1, {
		&preamble_2cl_top, &test_case_3_6_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_3_6_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"3.7.1", desc_case_3_7_1, {
		&preamble_2cl_top, &test_case_3_7_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_3_7_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"4.1.1", desc_case_4_1_1, {
		&preamble_2_top, &test_case_4_1_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_4_1_1_bot, &postamble_2_bot}, 0, 0}, {
		"4.1.2", desc_case_4_1_2, {
		&preamble_2_top, &test_case_4_1_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_4_1_2_bot, &postamble_2_bot}, 0, 0}, {
		"4.2.1", desc_case_4_2_1, {
		&preamble_2_top, &test_case_4_2_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_4_2_1_bot, &postamble_2_bot}, 0, 0}, {
		"4.2.2", desc_case_4_2_2, {
		&preamble_2_top, &test_case_4_2_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_4_2_2_bot, &postamble_2_bot}, 0, 0}, {
		"4.3.1", desc_case_4_3_1, {
		&preamble_2_top, &test_case_4_3_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_4_3_1_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.1", desc_case_4_4_1, {
		&preamble_3_top, &test_case_4_4_1_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_1_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.2", desc_case_4_4_2, {
		&preamble_3_top, &test_case_4_4_2_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_2_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.3", desc_case_4_4_3, {
		&preamble_3_top, &test_case_4_4_3_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_3_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.4", desc_case_4_4_4, {
		&preamble_3_top, &test_case_4_4_4_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_4_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.5", desc_case_4_4_5, {
		&preamble_3_top, &test_case_4_4_5_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_5_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.6", desc_case_4_4_6, {
		&preamble_3_top, &test_case_4_4_6_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_6_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.7", desc_case_4_4_7, {
		&preamble_3_top, &test_case_4_4_7_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_7_bot, &postamble_2_bot}, 0, 0}, {
		"4.4.8", desc_case_4_4_8, {
		&preamble_3_top, &test_case_4_4_8_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_4_8_bot, &postamble_2_bot}, 0, 0}, {
		"4.5.1", desc_case_4_5_1, {
		&preamble_2_top, &test_case_4_5_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_4_5_1_bot, &postamble_2_bot}, 0, 0}, {
		"4.6.1", desc_case_4_6_1, {
		&preamble_3_top, &test_case_4_6_1_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_6_1_bot, &postamble_2_bot}, 0, 0}, {
		"4.6.2", desc_case_4_6_2, {
		&preamble_3_top, &test_case_4_6_2_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_6_2_bot, &postamble_2_bot}, 0, 0}, {
		"4.6.3", desc_case_4_6_3, {
		&preamble_3_top, &test_case_4_6_3_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_6_3_bot, &postamble_2_bot}, 0, 0}, {
		"4.6.4", desc_case_4_6_4, {
		&preamble_3_top, &test_case_4_6_4_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_6_4_bot, &postamble_2_bot}, 0, 0}, {
		"4.7.1", desc_case_4_7_1, {
		&preamble_2cl_top, &test_case_4_7_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_4_7_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"4.7.2", desc_case_4_7_2, {
		&preamble_2cl_top, &test_case_4_7_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_4_7_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"4.7.3", desc_case_4_7_3, {
		&preamble_2cl_top, &test_case_4_7_3_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_4_7_3_bot, &postamble_2cl_bot}, 0, 0}, {
		"4.7.4", desc_case_4_7_4, {
		&preamble_2cl_top, &test_case_4_7_4_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_4_7_4_bot, &postamble_2cl_bot}, 0, 0}, {
		"4.8.1", desc_case_4_8_1, {
		&preamble_2_top, &test_case_4_8_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_4_8_1_bot, &postamble_2_bot}, 0, 0}, {
		"4.8.2", desc_case_4_8_2, {
		&preamble_2cl_top, &test_case_4_8_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_4_8_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"4.8.3", desc_case_4_8_3, {
		&preamble_2cl_top, &test_case_4_8_3_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_4_8_3_bot, &postamble_2cl_bot}, 0, 0}, {
		"4.9.1", desc_case_4_9_1, {
		&preamble_3_top, &test_case_4_9_1_top, &postamble_2_top}, {
	&preamble_3_bot, &test_case_4_9_1_bot, &postamble_2_bot}, 0, 0}, {
		"5.1.1", desc_case_5_1_1, {
		&preamble_0, &test_case_5_1_1_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_1_bot, &postamble_0}, 0, 0}, {
		"5.1.2", desc_case_5_1_2, {
		&preamble_0, &test_case_5_1_2_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_2_bot, &postamble_0}, 0, 0}, {
		"5.1.3", desc_case_5_1_3, {
		&preamble_0, &test_case_5_1_3_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_3_bot, &postamble_0}, 0, 0}, {
		"5.1.4", desc_case_5_1_4, {
		&preamble_0, &test_case_5_1_4_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_4_bot, &postamble_0}, 0, 0}, {
		"5.1.5", desc_case_5_1_5, {
		&preamble_0, &test_case_5_1_5_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_5_bot, &postamble_0}, 0, 0}, {
		"5.1.6", desc_case_5_1_6, {
		&preamble_0, &test_case_5_1_6_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_6_bot, &postamble_0}, 0, 0}, {
		"5.1.7", desc_case_5_1_7, {
		&preamble_0, &test_case_5_1_7_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_7_bot, &postamble_0}, 0, 0}, {
		"5.1.8", desc_case_5_1_8, {
		&preamble_0, &test_case_5_1_8_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_8_bot, &postamble_0}, 0, 0}, {
		"5.1.9", desc_case_5_1_9, {
		&preamble_0, &test_case_5_1_9_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_9_bot, &postamble_0}, 0, 0}, {
		"5.1.10", desc_case_5_1_10, {
		&preamble_0, &test_case_5_1_10_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_10_bot, &postamble_0}, 0, 0}, {
		"5.1.11", desc_case_5_1_11, {
		&preamble_0, &test_case_5_1_11_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_11_bot, &postamble_0}, 0, 0}, {
		"5.1.12", desc_case_5_1_12, {
		&preamble_0, &test_case_5_1_12_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_12_bot, &postamble_0}, 0, 0}, {
		"5.1.13", desc_case_5_1_13, {
		&preamble_0, &test_case_5_1_13_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_13_bot, &postamble_0}, 0, 0}, {
		"5.1.14", desc_case_5_1_14, {
		&preamble_0, &test_case_5_1_14_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_14_bot, &postamble_0}, 0, 0}, {
		"5.1.15", desc_case_5_1_15, {
		&preamble_0, &test_case_5_1_15_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_15_bot, &postamble_0}, 0, 0}, {
		"5.1.16", desc_case_5_1_16, {
		&preamble_0, &test_case_5_1_16_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_16_bot, &postamble_0}, 0, 0}, {
		"5.1.17", desc_case_5_1_17, {
		&preamble_0, &test_case_5_1_17_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_17_bot, &postamble_0}, 0, 0}, {
		"5.1.18", desc_case_5_1_18, {
		&preamble_0, &test_case_5_1_18_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_18_bot, &postamble_0}, 0, 0}, {
		"5.1.19", desc_case_5_1_19, {
		&preamble_0, &test_case_5_1_19_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_19_bot, &postamble_0}, 0, 0}, {
		"5.1.20", desc_case_5_1_20, {
		&preamble_0, &test_case_5_1_20_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_20_bot, &postamble_0}, 0, 0}, {
		"5.1.21", desc_case_5_1_21, {
		&preamble_0, &test_case_5_1_21_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_21_bot, &postamble_0}, 0, 0}, {
		"5.1.22", desc_case_5_1_22, {
		&preamble_0, &test_case_5_1_22_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_22_bot, &postamble_0}, 0, 0}, {
		"5.1.23", desc_case_5_1_23, {
		&preamble_0, &test_case_5_1_23_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_23_bot, &postamble_0}, 0, 0}, {
		"5.1.24", desc_case_5_1_24, {
		&preamble_0, &test_case_5_1_24_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_24_bot, &postamble_0}, 0, 0}, {
		"5.1.25", desc_case_5_1_25, {
		&preamble_0, &test_case_5_1_25_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_25_bot, &postamble_0}, 0, 0}, {
		"5.1.26", desc_case_5_1_26, {
		&preamble_0, &test_case_5_1_26_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_26_bot, &postamble_0}, 0, 0}, {
		"5.1.27", desc_case_5_1_27, {
		&preamble_0, &test_case_5_1_27_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_27_bot, &postamble_0}, 0, 0}, {
		"5.1.28", desc_case_5_1_28, {
		&preamble_0, &test_case_5_1_28_top, &postamble_0}, {
	&preamble_0, &test_case_5_1_28_bot, &postamble_0}, 0, 0}, {
		"5.2.1", desc_case_5_2_1, {
		&preamble_0, &test_case_5_2_1_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_1_bot, &postamble_0}, 0, 0}, {
		"5.2.2", desc_case_5_2_2, {
		&preamble_0, &test_case_5_2_2_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_2_bot, &postamble_0}, 0, 0}, {
		"5.2.3", desc_case_5_2_3, {
		&preamble_0, &test_case_5_2_3_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_3_bot, &postamble_0}, 0, 0}, {
		"5.2.4", desc_case_5_2_4, {
		&preamble_0, &test_case_5_2_4_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_4_bot, &postamble_0}, 0, 0}, {
		"5.2.5", desc_case_5_2_5, {
		&preamble_0, &test_case_5_2_5_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_5_bot, &postamble_0}, 0, 0}, {
		"5.2.6", desc_case_5_2_6, {
		&preamble_0, &test_case_5_2_6_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_6_bot, &postamble_0}, 0, 0}, {
		"5.2.7", desc_case_5_2_7, {
		&preamble_0, &test_case_5_2_7_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_7_bot, &postamble_0}, 0, 0}, {
		"5.2.8", desc_case_5_2_8, {
		&preamble_0, &test_case_5_2_8_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_8_bot, &postamble_0}, 0, 0}, {
		"5.2.9", desc_case_5_2_9, {
		&preamble_0, &test_case_5_2_9_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_9_bot, &postamble_0}, 0, 0}, {
		"5.2.10", desc_case_5_2_10, {
		&preamble_0, &test_case_5_2_10_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_10_bot, &postamble_0}, 0, 0}, {
		"5.2.11", desc_case_5_2_11, {
		&preamble_0, &test_case_5_2_11_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_11_bot, &postamble_0}, 0, 0}, {
		"5.2.12", desc_case_5_2_12, {
		&preamble_0, &test_case_5_2_12_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_12_bot, &postamble_0}, 0, 0}, {
		"5.2.13", desc_case_5_2_13, {
		&preamble_0, &test_case_5_2_13_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_13_bot, &postamble_0}, 0, 0}, {
		"5.2.14", desc_case_5_2_14, {
		&preamble_0, &test_case_5_2_14_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_14_bot, &postamble_0}, 0, 0}, {
		"5.2.15", desc_case_5_2_15, {
		&preamble_0, &test_case_5_2_15_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_15_bot, &postamble_0}, 0, 0}, {
		"5.2.16", desc_case_5_2_16, {
		&preamble_0, &test_case_5_2_16_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_16_bot, &postamble_0}, 0, 0}, {
		"5.2.17", desc_case_5_2_17, {
		&preamble_0, &test_case_5_2_17_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_17_bot, &postamble_0}, 0, 0}, {
		"5.2.18", desc_case_5_2_18, {
		&preamble_0, &test_case_5_2_18_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_18_bot, &postamble_0}, 0, 0}, {
		"5.2.19", desc_case_5_2_19, {
		&preamble_0, &test_case_5_2_19_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_19_bot, &postamble_0}, 0, 0}, {
		"5.2.20", desc_case_5_2_20, {
		&preamble_0, &test_case_5_2_20_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_20_bot, &postamble_0}, 0, 0}, {
		"5.2.21", desc_case_5_2_21, {
		&preamble_0, &test_case_5_2_21_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_21_bot, &postamble_0}, 0, 0}, {
		"5.2.22", desc_case_5_2_22, {
		&preamble_0, &test_case_5_2_22_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_22_bot, &postamble_0}, 0, 0}, {
		"5.2.23", desc_case_5_2_23, {
		&preamble_0, &test_case_5_2_23_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_23_bot, &postamble_0}, 0, 0}, {
		"5.2.24", desc_case_5_2_24, {
		&preamble_0, &test_case_5_2_24_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_24_bot, &postamble_0}, 0, 0}, {
		"5.2.25", desc_case_5_2_25, {
		&preamble_0, &test_case_5_2_25_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_25_bot, &postamble_0}, 0, 0}, {
		"5.2.26", desc_case_5_2_26, {
		&preamble_0, &test_case_5_2_26_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_26_bot, &postamble_0}, 0, 0}, {
		"5.2.27", desc_case_5_2_27, {
		&preamble_0, &test_case_5_2_27_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_27_bot, &postamble_0}, 0, 0}, {
		"5.2.28", desc_case_5_2_28, {
		&preamble_0, &test_case_5_2_28_top, &postamble_0}, {
	&preamble_0, &test_case_5_2_28_bot, &postamble_0}, 0, 0}, {
		"5.3.1", desc_case_5_3_1, {
		&preamble_0, &test_case_5_3_1_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_1_bot, &postamble_0}, 0, 0}, {
		"5.3.2", desc_case_5_3_2, {
		&preamble_0, &test_case_5_3_2_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_2_bot, &postamble_0}, 0, 0}, {
		"5.3.3", desc_case_5_3_3, {
		&preamble_0, &test_case_5_3_3_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_3_bot, &postamble_0}, 0, 0}, {
		"5.3.4", desc_case_5_3_4, {
		&preamble_0, &test_case_5_3_4_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_4_bot, &postamble_0}, 0, 0}, {
		"5.3.5", desc_case_5_3_5, {
		&preamble_0, &test_case_5_3_5_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_5_bot, &postamble_0}, 0, 0}, {
		"5.3.6", desc_case_5_3_6, {
		&preamble_0, &test_case_5_3_6_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_6_bot, &postamble_0}, 0, 0}, {
		"5.3.7", desc_case_5_3_7, {
		&preamble_0, &test_case_5_3_7_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_7_bot, &postamble_0}, 0, 0}, {
		"5.3.8", desc_case_5_3_8, {
		&preamble_0, &test_case_5_3_8_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_8_bot, &postamble_0}, 0, 0}, {
		"5.3.9", desc_case_5_3_9, {
		&preamble_0, &test_case_5_3_9_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_9_bot, &postamble_0}, 0, 0}, {
		"5.3.10", desc_case_5_3_10, {
		&preamble_0, &test_case_5_3_10_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_10_bot, &postamble_0}, 0, 0}, {
		"5.3.11", desc_case_5_3_11, {
		&preamble_0, &test_case_5_3_11_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_11_bot, &postamble_0}, 0, 0}, {
		"5.3.12", desc_case_5_3_12, {
		&preamble_0, &test_case_5_3_12_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_12_bot, &postamble_0}, 0, 0}, {
		"5.3.13", desc_case_5_3_13, {
		&preamble_0, &test_case_5_3_13_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_13_bot, &postamble_0}, 0, 0}, {
		"5.3.14", desc_case_5_3_14, {
		&preamble_0, &test_case_5_3_14_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_14_bot, &postamble_0}, 0, 0}, {
		"5.3.15", desc_case_5_3_15, {
		&preamble_0, &test_case_5_3_15_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_15_bot, &postamble_0}, 0, 0}, {
		"5.3.16", desc_case_5_3_16, {
		&preamble_0, &test_case_5_3_16_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_16_bot, &postamble_0}, 0, 0}, {
		"5.3.17", desc_case_5_3_17, {
		&preamble_0, &test_case_5_3_17_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_17_bot, &postamble_0}, 0, 0}, {
		"5.3.18", desc_case_5_3_18, {
		&preamble_0, &test_case_5_3_18_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_18_bot, &postamble_0}, 0, 0}, {
		"5.3.19", desc_case_5_3_19, {
		&preamble_0, &test_case_5_3_19_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_19_bot, &postamble_0}, 0, 0}, {
		"5.3.20", desc_case_5_3_20, {
		&preamble_0, &test_case_5_3_20_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_20_bot, &postamble_0}, 0, 0}, {
		"5.3.21", desc_case_5_3_21, {
		&preamble_0, &test_case_5_3_21_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_21_bot, &postamble_0}, 0, 0}, {
		"5.3.22", desc_case_5_3_22, {
		&preamble_0, &test_case_5_3_22_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_22_bot, &postamble_0}, 0, 0}, {
		"5.3.23", desc_case_5_3_23, {
		&preamble_0, &test_case_5_3_23_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_23_bot, &postamble_0}, 0, 0}, {
		"5.3.24", desc_case_5_3_24, {
		&preamble_0, &test_case_5_3_24_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_24_bot, &postamble_0}, 0, 0}, {
		"5.3.25", desc_case_5_3_25, {
		&preamble_0, &test_case_5_3_25_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_25_bot, &postamble_0}, 0, 0}, {
		"5.3.26", desc_case_5_3_26, {
		&preamble_0, &test_case_5_3_26_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_26_bot, &postamble_0}, 0, 0}, {
		"5.3.27", desc_case_5_3_27, {
		&preamble_0, &test_case_5_3_27_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_27_bot, &postamble_0}, 0, 0}, {
		"5.3.28", desc_case_5_3_28, {
		&preamble_0, &test_case_5_3_28_top, &postamble_0}, {
	&preamble_0, &test_case_5_3_28_bot, &postamble_0}, 0, 0}, {
		"6.1.1", desc_case_6_1_1, {
		&preamble_2_top, &test_case_6_1_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_1_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.2", desc_case_6_1_2, {
		&preamble_2_top, &test_case_6_1_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_2_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.3", desc_case_6_1_3, {
		&preamble_2_top, &test_case_6_1_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_3_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.4", desc_case_6_1_4, {
		&preamble_2_top, &test_case_6_1_4_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_4_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.5", desc_case_6_1_5, {
		&preamble_2_top, &test_case_6_1_5_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_5_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.6", desc_case_6_1_6, {
		&preamble_2_top, &test_case_6_1_6_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_6_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.7", desc_case_6_1_7, {
		&preamble_2_top, &test_case_6_1_7_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_7_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.8", desc_case_6_1_8, {
		&preamble_2_top, &test_case_6_1_8_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_8_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.9", desc_case_6_1_9, {
		&preamble_2_top, &test_case_6_1_9_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_9_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.10", desc_case_6_1_10, {
		&preamble_2_top, &test_case_6_1_10_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_10_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.11", desc_case_6_1_11, {
		&preamble_2_top, &test_case_6_1_11_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_11_bot, &postamble_2_bot}, 0, 0}, {
		"6.1.12", desc_case_6_1_12, {
		&preamble_2_top, &test_case_6_1_12_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_1_12_bot, &postamble_2_bot}, 0, 0}, {
		"6.2.1", desc_case_6_2_1, {
		&preamble_1_top, &test_case_6_2_1_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_6_2_1_bot, &postamble_1_bot}, 0, 0}, {
		"6.2.2", desc_case_6_2_2, {
		&preamble_1_top, &test_case_6_2_2_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_6_2_2_bot, &postamble_1_bot}, 0, 0}, {
		"6.2.3", desc_case_6_2_3, {
		&preamble_1_top, &test_case_6_2_3_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_6_2_3_bot, &postamble_1_bot}, 0, 0}, {
		"6.2.4", desc_case_6_2_4, {
		&preamble_1_top, &test_case_6_2_4_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_6_2_4_bot, &postamble_1_bot}, 0, 0}, {
		"6.2.5", desc_case_6_2_5, {
		&preamble_1_top, &test_case_6_2_5_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_6_2_5_bot, &postamble_1_bot}, 0, 0}, {
		"6.2.6", desc_case_6_2_6, {
		&preamble_1_top, &test_case_6_2_6_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_6_2_6_bot, &postamble_1_bot}, 0, 0}, {
		"6.3.1", desc_case_6_3_1, {
		&preamble_2_top, &test_case_6_3_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_1_bot, &postamble_2_bot}, 0, 0}, {
		"6.3.2", desc_case_6_3_2, {
		&preamble_2_top, &test_case_6_3_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_2_bot, &postamble_2_bot}, 0, 0}, {
		"6.3.3", desc_case_6_3_3, {
		&preamble_2_top, &test_case_6_3_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_3_bot, &postamble_2_bot}, 0, 0}, {
		"6.3.4", desc_case_6_3_4, {
		&preamble_2_top, &test_case_6_3_4_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_4_bot, &postamble_2_bot}, 0, 0}, {
		"6.3.5", desc_case_6_3_5, {
		&preamble_2_top, &test_case_6_3_5_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_5_bot, &postamble_2_bot}, 0, 0}, {
		"6.3.6", desc_case_6_3_6, {
		&preamble_2_top, &test_case_6_3_6_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_6_bot, &postamble_2_bot}, 0, 0}, {
		"6.3.7", desc_case_6_3_7, {
		&preamble_2_top, &test_case_6_3_7_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_7_bot, &postamble_2_bot}, 0, 0}, {
		"6.3.8", desc_case_6_3_8, {
		&preamble_2_top, &test_case_6_3_8_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_3_8_bot, &postamble_2_bot}, 0, 0}, {
		"6.4.1", desc_case_6_4_1, {
		&preamble_2_top, &test_case_6_4_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_4_1_bot, &postamble_2_bot}, 0, 0}, {
		"6.4.2", desc_case_6_4_2, {
		&preamble_2_top, &test_case_6_4_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_4_2_bot, &postamble_2_bot}, 0, 0}, {
		"6.4.3", desc_case_6_4_3, {
		&preamble_2_top, &test_case_6_4_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_4_3_bot, &postamble_2_bot}, 0, 0}, {
		"6.4.4", desc_case_6_4_4, {
		&preamble_2_top, &test_case_6_4_4_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_4_4_bot, &postamble_2_bot}, 0, 0}, {
		"6.4.5", desc_case_6_4_5, {
		&preamble_2_top, &test_case_6_4_5_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_4_5_bot, &postamble_2_bot}, 0, 0}, {
		"6.4.6", desc_case_6_4_6, {
		&preamble_2_top, &test_case_6_4_6_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_6_4_6_bot, &postamble_2_bot}, 0, 0}, {
		"6.5.1", desc_case_6_5_1, {
		&preamble_3_top, &test_case_6_5_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_6_5_1_bot, &postamble_3_bot}, 0, 0}, {
		"6.5.2", desc_case_6_5_2, {
		&preamble_3_top, &test_case_6_5_2_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_6_5_2_bot, &postamble_3_bot}, 0, 0}, {
		"6.5.3", desc_case_6_5_3, {
		&preamble_3_top, &test_case_6_5_3_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_6_5_3_bot, &postamble_3_bot}, 0, 0}, {
		"6.5.4", desc_case_6_5_4, {
		&preamble_3_top, &test_case_6_5_4_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_6_5_4_bot, &postamble_3_bot}, 0, 0}, {
		"6.5.5", desc_case_6_5_5, {
		&preamble_3_top, &test_case_6_5_5_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_6_5_5_bot, &postamble_3_bot}, 0, 0}, {
		"6.6.1", desc_case_6_6_1, {
		&preamble_2cl_top, &test_case_6_6_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_6_6_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"6.6.2", desc_case_6_6_2, {
		&preamble_2cl_top, &test_case_6_6_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_6_6_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.1.1", desc_case_7_1_1, {
		&preamble_2_top, &test_case_7_1_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_1_1_bot, &postamble_2_bot}, 0, 0}, {
		"7.1.2", desc_case_7_1_2, {
		&preamble_2cl_top, &test_case_7_1_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_1_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.1.3", desc_case_7_1_3, {
		&preamble_2_top, &test_case_7_1_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_1_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.2.1", desc_case_7_2_1, {
		&preamble_3_top, &test_case_7_2_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_2_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.3.1", desc_case_7_3_1, {
		&preamble_1_top, &test_case_7_3_1_top, &postamble_1_top}, {
	&preamble_1_bot, &test_case_7_3_1_bot, &postamble_1_bot}, 0, 0}, {
		"7.3.2", desc_case_7_3_2, {
		&preamble_2cl_top, &test_case_7_3_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_3_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.3.3", desc_case_7_3_3, {
		&preamble_3_top, &test_case_7_3_3_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_3_3_bot, &postamble_3_bot}, 0, 0}, {
		"7.3.4", desc_case_7_3_4, {
		&preamble_2_top, &test_case_7_3_4_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_3_4_bot, &postamble_2_bot}, 0, 0}, {
		"7.3.5", desc_case_7_3_5, {
		&preamble_2_top, &test_case_7_3_5_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_3_5_bot, &postamble_2_bot}, 0, 0}, {
		"7.4.1", desc_case_7_4_1, {
		&preamble_3_top, &test_case_7_4_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_4_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.4.2", desc_case_7_4_2, {
		&preamble_2cl_top, &test_case_7_4_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_4_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.4.3", desc_case_7_4_3, {
		&preamble_2_top, &test_case_7_4_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_4_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.5.1", desc_case_7_5_1, {
		&preamble_3_top, &test_case_7_5_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_5_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.5.2", desc_case_7_5_2, {
		&preamble_2cl_top, &test_case_7_5_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_5_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.5.3", desc_case_7_5_3, {
		&preamble_2_top, &test_case_7_5_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_5_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.6.1", desc_case_7_6_1, {
		&preamble_2_top, &test_case_7_6_1_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_6_1_bot, &postamble_2_bot}, 0, 0}, {
		"7.6.2", desc_case_7_6_2, {
		&preamble_2cl_top, &test_case_7_6_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_6_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.6.3", desc_case_7_6_3, {
		&preamble_2_top, &test_case_7_6_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_6_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.7.1", desc_case_7_7_1, {
		&preamble_3_top, &test_case_7_7_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_7_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.7.2", desc_case_7_7_2, {
		&preamble_2cl_top, &test_case_7_7_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_7_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.7.3", desc_case_7_7_3, {
		&preamble_2_top, &test_case_7_7_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_7_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.8.1", desc_case_7_8_1, {
		&preamble_3_top, &test_case_7_8_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_8_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.8.2", desc_case_7_8_2, {
		&preamble_2cl_top, &test_case_7_8_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_8_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.8.3", desc_case_7_8_3, {
		&preamble_2_top, &test_case_7_8_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_8_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.9.1", desc_case_7_9_1, {
		&preamble_3_top, &test_case_7_9_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_9_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.9.2", desc_case_7_9_2, {
		&preamble_2cl_top, &test_case_7_9_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_9_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.9.3", desc_case_7_9_3, {
		&preamble_2_top, &test_case_7_9_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_9_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.10.1", desc_case_7_10_1, {
		&preamble_2cl_top, &test_case_7_10_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_10_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.10.2", desc_case_7_10_2, {
		&preamble_2_top, &test_case_7_10_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_10_2_bot, &postamble_2_bot}, 0, 0}, {
		"7.10.3", desc_case_7_10_3, {
		&preamble_1cl_top, &test_case_7_10_3_top, &postamble_1cl_top}, {
	&preamble_1cl_bot, &test_case_7_10_3_bot, &postamble_1cl_bot}, 0, 0}, {
		"7.11.1", desc_case_7_11_1, {
		&preamble_2cl_top, &test_case_7_11_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_11_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.11.2", desc_case_7_11_2, {
		&preamble_2_top, &test_case_7_11_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_11_2_bot, &postamble_2_bot}, 0, 0}, {
		"7.11.3", desc_case_7_11_3, {
		&preamble_1cl_top, &test_case_7_11_3_top, &postamble_1cl_top}, {
	&preamble_1cl_bot, &test_case_7_11_3_bot, &postamble_1cl_bot}, 0, 0}, {
		"7.12.1", desc_case_7_12_1, {
		&preamble_2cl_top, &test_case_7_12_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_12_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.12.2", desc_case_7_12_2, {
		&preamble_2_top, &test_case_7_12_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_12_2_bot, &postamble_2_bot}, 0, 0}, {
		"7.12.3", desc_case_7_12_3, {
		&preamble_1cl_top, &test_case_7_12_3_top, &postamble_1cl_top}, {
	&preamble_1cl_bot, &test_case_7_12_3_bot, &postamble_1cl_bot}, 0, 0}, {
		"7.13.1", desc_case_7_13_1, {
		&preamble_3_top, &test_case_7_13_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_13_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.13.2", desc_case_7_13_2, {
		&preamble_2cl_top, &test_case_7_13_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_13_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.13.3", desc_case_7_13_3, {
		&preamble_2_top, &test_case_7_13_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_13_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.14.1", desc_case_7_14_1, {
		&preamble_3_top, &test_case_7_14_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_7_14_1_bot, &postamble_3_bot}, 0, 0}, {
		"7.14.2", desc_case_7_14_2, {
		&preamble_2cl_top, &test_case_7_14_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_14_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.14.3", desc_case_7_14_3, {
		&preamble_2_top, &test_case_7_14_3_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_14_3_bot, &postamble_2_bot}, 0, 0}, {
		"7.15.1", desc_case_7_15_1, {
		&preamble_2cl_top, &test_case_7_15_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_15_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.15.2", desc_case_7_15_2, {
		&preamble_2_top, &test_case_7_15_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_15_2_bot, &postamble_2_bot}, 0, 0}, {
		"7.15.3", desc_case_7_15_3, {
		&preamble_1cl_top, &test_case_7_15_3_top, &postamble_1cl_top}, {
	&preamble_1cl_bot, &test_case_7_15_3_bot, &postamble_1cl_bot}, 0, 0}, {
		"7.16.1", desc_case_7_16_1, {
		&preamble_2cl_top, &test_case_7_16_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_7_16_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"7.16.2", desc_case_7_16_2, {
		&preamble_2_top, &test_case_7_16_2_top, &postamble_2_top}, {
	&preamble_2_bot, &test_case_7_16_2_bot, &postamble_2_bot}, 0, 0}, {
		"7.16.3", desc_case_7_16_3, {
		&preamble_1cl_top, &test_case_7_16_3_top, &postamble_1cl_top}, {
	&preamble_1cl_bot, &test_case_7_16_3_bot, &postamble_1cl_bot}, 0, 0}, {
		"7.17.1", desc_case_7_17_1, {
		&preamble_0, &test_case_7_17_1_top, &postamble_0}, {
	&preamble_0, &test_case_7_17_1_bot, &postamble_0}, 0, 0}, {
		"8.1.1", desc_case_8_1_1, {
		&preamble_3_top, &test_case_8_1_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_1_1_bot, &postamble_3_bot}, 0, 0}, {
		"8.1.2", desc_case_8_1_2, {
		&preamble_3_top, &test_case_8_1_2_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_1_2_bot, &postamble_3_bot}, 0, 0}, {
		"8.1.3", desc_case_8_1_3, {
		&preamble_3_top, &test_case_8_1_3_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_1_3_bot, &postamble_3_bot}, 0, 0}, {
		"8.1.4", desc_case_8_1_4, {
		&preamble_3_top, &test_case_8_1_4_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_1_4_bot, &postamble_3_bot}, 0, 0}, {
		"8.2.1", desc_case_8_2_1, {
		&preamble_3_top, &test_case_8_2_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_2_1_bot, &postamble_3_bot}, 0, 0}, {
		"8.2.2", desc_case_8_2_2, {
		&preamble_3_top, &test_case_8_2_2_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_2_2_bot, &postamble_3_bot}, 0, 0}, {
		"8.2.3", desc_case_8_2_3, {
		&preamble_3_top, &test_case_8_2_3_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_2_3_bot, &postamble_3_bot}, 0, 0}, {
		"8.3.1", desc_case_8_3_1, {
		&preamble_2cl_top, &test_case_8_3_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_8_3_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"8.3.2", desc_case_8_3_2, {
		&preamble_2cl_top, &test_case_8_3_2_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_8_3_2_bot, &postamble_2cl_bot}, 0, 0}, {
		"8.4.1", desc_case_8_4_1, {
		&preamble_2cl_top, &test_case_8_4_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_8_4_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"8.5.1", desc_case_8_5_1, {
		&preamble_3_top, &test_case_8_5_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_5_1_bot, &postamble_3_bot}, 0, 0}, {
		"8.5.2", desc_case_8_5_2, {
		&preamble_3_top, &test_case_8_5_2_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_5_2_bot, &postamble_3_bot}, 0, 0}, {
		"8.5.3", desc_case_8_5_3, {
		&preamble_3_top, &test_case_8_5_3_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_5_3_bot, &postamble_3_bot}, 0, 0}, {
		"8.5.4", desc_case_8_5_4, {
		&preamble_3s_top, &test_case_8_5_4_top, &postamble_3s_top}, {
	&preamble_3s_bot, &test_case_8_5_4_bot, &postamble_3s_bot}, 0, 0}, {
		"8.6.1", desc_case_8_6_1, {
		&preamble_3_top, &test_case_8_6_1_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_6_1_bot, &postamble_3_bot}, 0, 0}, {
		"8.6.2", desc_case_8_6_2, {
		&preamble_3_top, &test_case_8_6_2_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_6_2_bot, &postamble_3_bot}, 0, 0}, {
		"8.6.3", desc_case_8_6_3, {
		&preamble_3_top, &test_case_8_6_3_top, &postamble_3_top}, {
	&preamble_3_bot, &test_case_8_6_3_bot, &postamble_3_bot}, 0, 0}, {
		"8.7.1", desc_case_8_7_1, {
		&preamble_2cl_top, &test_case_8_7_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_8_7_1_bot, &postamble_2cl_bot}, 0, 0}, {
		"8.8.1", desc_case_8_8_1, {
		&preamble_2cl_top, &test_case_8_8_1_top, &postamble_2cl_top}, {
	&preamble_2cl_bot, &test_case_8_8_1_bot, &postamble_2cl_bot}, 0, 0}, {
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
				fprintf(stdout, "\nTest Case XTI/TLI:%s:\n%s\n", tests[i].numb, tests[i].name);
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
					fprintf(stdout, "Test Case XTI/TLI:%-10s ", tests[i].numb);
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
			fprintf(stdout, "========= %3d successes   \n", successes);
			fprintf(stdout, "========= %3d failures    \n", failures);
			fprintf(stdout, "========= %3d inconclusive\n", inconclusive);
			fprintf(stdout, "==========================\n");
			fprintf(stdout, "========= %3d total       \n", successes + failures + inconclusive);
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
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
XNS 5.2 - OpenSS7 XTI/TLI Library - Conformance Test Suite\n\
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
    Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
\n\
    See `%1$s --copying' for copying permissions.\n\
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
        print the version and exit\n\
    -C, --copying\n\
        print copying permissions and exit\n\
\n\
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
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{ 0, }
		};
		/* *INDENT-ON* */
		c = getopt_long_only(argc, argv, "f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "f::so:t:mqvhVC?");
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
	if (!tests_to_run) {
		if (verbose) {
			fprintf(stderr, "%s: error: no tests to run\n", argv[0]);
			fflush(stderr);
		}
		exit(2);
	}
	copying(argc, argv);
	do_tests();
	exit(0);
}
