/*****************************************************************************

 @(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/06/07 22:35:57 $

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

 Last Modified $Date: 2004/06/07 22:35:57 $ by $Author: brian $

 -----------------------------------------------------------------------------
 $Log: test-streams.c,v $
 Revision 0.9.2.2  2004/06/07 22:35:57  brian
 - Starting test suites for everything.

 Revision 1.2  2004/06/07 22:35:57  brian
 - Starting test suites for everything.

 Revision 1.1  2004/06/07 17:34:30  brian
 - Starting test programs.

 *****************************************************************************/

#ident "@(#) $RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/06/07 22:35:57 $"

static char const ident[] = "$RCSfile: test-streams.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/06/07 22:35:57 $";

/* 
 *  Simple test program for STREAMS.
 *
 *  Several test environments are supported:
 *
 *  Simple null stream.
 *  
 *  Clone driver.
 *  
 *  Named STREAMS device driver.
 *  
 *  Simple echo stream.
 *  
 *  STREAMS based fifo.
 *  
 *  STREAMS based pipe.
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
	t1 = 0, tmax
};

#undef HZ
#define HZ 1000

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
	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	act.sa_restorer = NULL;
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
		if (verbose && show_data || verbose > 1) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "<-DATA--------------|  |  %4d bytes                   |                    [%d]\n", data->len, state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (__TEST_DATA);
	}
	if (fd == bot_fd) {
		if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |  |- - - - - - - - - - - - - - - >|--T_DATA_REQ%c-----> (%d)\n", MORE_flag ? '+' : '-', state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			ret = __TEST_DATA_REQ;
			break;
		case T_EXDATA_REQ:
			MORE_flag = p->data_req.MORE_flag;
			if (verbose && show_data || verbose > 1) {
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
			if (verbose && show_data || verbose > 1) {
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

/*
 *  -------------------------------------------------------------------------
 *
 *  Test Cases
 *
 *  -------------------------------------------------------------------------
 */

#if 0
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
#endif

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
#if 0
		"1.1", desc_case_1_1, {
		&preamble_0, &test_case_1_1_top, &postamble_0}, {
	&preamble_0, &test_case_1_1_bot, &postamble_0}, 0, 0}, {
#endif
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
    Copyright (c) 2004  OpenSS7 Corporation.  All Rights Reserved.\n\
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
		c = getopt_long(argc, argv, "f::so:t:mqvhV?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "f::so:t:mqvhV?");
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
