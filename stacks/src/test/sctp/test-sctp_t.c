/*****************************************************************************

 @(#) $RCSfile: test-sctp_t.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/03/03 12:06:13 $

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

 Last Modified $Date: 2006/03/03 12:06:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-sctp_t.c,v $
 Revision 0.9.2.6  2006/03/03 12:06:13  brian
 - 32/64-bit compatibility

 Revision 0.9.2.5  2005/05/14 08:31:33  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: test-sctp_t.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/03/03 12:06:13 $"

static char const ident[] = "$RCSfile: test-sctp_t.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/03/03 12:06:13 $";

/* 
 *  This file is for testing the sctp_t driver.  It is provided for the
 *  purpose of testing the OpenSS7 sctp_t driver only.
 */
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#if defined HAVE_TIHDR_H
#   include <tihdr.h>
#else
#   include <sys/tihdr.h>
#endif

#include <sys/xti.h>
#include <sys/xti_ip.h>
#include <sys/xti_udp.h>
#include <sys/xti_tcp.h>
#include <sys/xti_sctp.h>

#ifndef SCTP_VERSION_2
#   ifndef sctp_addr_t
	typedef struct sctp_addr {
		uint16_t port __attribute__ ((packed));
		uint32_t addr[0] __attribute__ ((packed));
	} sctp_addr_t;
#	define sctp_addr_t sctp_addr_t
#   endif				/* sctp_addr_t */
#endif				/* SCTP_VERSION_2 */

static int verbose = 1;
static int summary = 0;

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;

static int last_event = 0;
static int last_errno = 0;
static int last_prim = 0;

static int conn_fd = 0;
static int resp_fd = 0;
static int list_fd = 0;

#define BUFSIZE 5*4096
#define FFLUSH(stream)

#define SHORT_WAIT 100 // 10
#define NORMAL_WAIT 500 // 100
#define LONG_WAIT 5000 // 500
#define LONGER_WAIT 10000 // 5000

ulong seq[10] = { 0, };
ulong tok[10] = { 0, };
ulong tsn[10] = { 0, };
ulong sid[10] = { 0, };
ulong ssn[10] = { 0, };
ulong ppi[10] = { 0, };
ulong exp[10] = { 0, };

union {
	union T_primitives tpi;
	char cbuf[BUFSIZE];
} cmd;

char dbuf[BUFSIZE];

struct strbuf ctrl = { BUFSIZE, 0, cmd.cbuf };
struct strbuf data = { BUFSIZE, 0, dbuf };

struct strfdinsert fdi = {
	{BUFSIZE, 0, cmd.cbuf},
	{BUFSIZE, 0, dbuf},
	0,
	0,
	0
};
int flags = 0;

#ifndef SCTP_VERSION_2
typedef struct addr {
	uint16_t port __attribute__ ((packed));
	struct in_addr addr[3] __attribute__ ((packed));
} addr_t;
#endif				/* SCTP_VERSION_2 */

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
	__RESULT_INCONCLUSIVE = -1, __RESULT_SUCCESS = 0,
	__RESULT_FAILURE = 1, __EVENT_CONN_REQ = 2, __EVENT_CONN_RES,
	__EVENT_DISCON_REQ, __EVENT_DATA_REQ, __EVENT_EXDATA_REQ,
	__EVENT_INFO_REQ, __EVENT_BIND_REQ, __EVENT_UNBIND_REQ,
	__EVENT_UNITDATA_REQ, __EVENT_OPTMGMT_REQ, __EVENT_ORDREL_REQ,
	__EVENT_OPTDATA_REQ, __EVENT_ADDR_REQ, __EVENT_CAPABILITY_REQ,
	__EVENT_CONN_IND, __EVENT_CONN_CON, __EVENT_DISCON_IND,
	__EVENT_DATA_IND, __EVENT_EXDATA_IND, __EVENT_INFO_ACK,
	__EVENT_BIND_ACK, __EVENT_ERROR_ACK, __EVENT_OK_ACK,
	__EVENT_UNITDATA_IND, __EVENT_UDERROR_IND, __EVENT_OPTMGMT_ACK,
	__EVENT_ORDREL_IND, __EVENT_OPTDATA_IND, __EVENT_ADDR_ACK,
	__EVENT_CAPABILITY_ACK, __EVENT_WRITE, __EVENT_WRITEV,
	__EVENT_PUTMSG_DATA, __EVENT_PUTPMSG_DATA, __EVENT_PUSH, __EVENT_POP,
	__EVENT_READ, __EVENT_READV, __EVENT_GETMSG, __EVENT_GETPMSG,
	__EVENT_DATA,
	__EVENT_DATACK_REQ, __EVENT_DATACK_IND,
	__EVENT_RESET_REQ, __EVENT_RESET_IND, __EVENT_RESET_RES, __EVENT_RESET_CON,
};

/* 
 *  -------------------------------------------------------------------------
 */
const char *device = "/dev/sctp_t";
int show = 1;
#ifndef SCTP_VERSION_2
addr_t addr1, addr2, addr3, addr4;
#else				/* SCTP_VERSION_2 */
struct sockaddr_in addr1[3], addr2[3], addr3[3], addr4[3];
#endif				/* SCTP_VERSION_2 */
unsigned short port1 = 10000, port2 = 10001, port3 = 10002, port4 = 10003;

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

#if 1
#undef lockf
#define lockf(x,y,z) 0
#endif

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
 *  Options
 *  
 *  -------------------------------------------------------------------------
 */

/* data options */
struct {
	struct t_opthdr ppi_hdr __attribute__ ((packed));
	t_scalar_t ppi_val __attribute__ ((packed));
	struct t_opthdr sid_hdr __attribute__ ((packed));
	t_scalar_t sid_val __attribute__ ((packed));
} opt_data = {
	{
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS}
	, 10, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS}
, 0};

/* receive data options */
typedef struct rdat_opt {
	struct t_opthdr ppi_hdr __attribute__ ((packed));
	t_scalar_t ppi_val __attribute__ ((packed));
	struct t_opthdr sid_hdr __attribute__ ((packed));
	t_scalar_t sid_val __attribute__ ((packed));
	struct t_opthdr ssn_hdr __attribute__ ((packed));
	t_scalar_t ssn_val __attribute__ ((packed));
	struct t_opthdr tsn_hdr __attribute__ ((packed));
	t_scalar_t tsn_val __attribute__ ((packed));
} rdat_opt_t;
/* connect options */
struct {
	struct t_opthdr ist_hdr __attribute__ ((packed));
	t_scalar_t ist_val __attribute__ ((packed));
	struct t_opthdr ost_hdr __attribute__ ((packed));
	t_scalar_t ost_val __attribute__ ((packed));
} opt_conn = {
	{
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS, T_SUCCESS}
	, 1, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS, T_SUCCESS}
, 1};

/* management options */
struct {
	struct t_opthdr nod_hdr __attribute__ ((packed));
	t_scalar_t nod_val __attribute__ ((packed));
	struct t_opthdr crk_hdr __attribute__ ((packed));
	t_scalar_t crk_val __attribute__ ((packed));
	struct t_opthdr ppi_hdr __attribute__ ((packed));
	t_scalar_t ppi_val __attribute__ ((packed));
	struct t_opthdr sid_hdr __attribute__ ((packed));
	t_scalar_t sid_val __attribute__ ((packed));
	struct t_opthdr rcv_hdr __attribute__ ((packed));
	t_scalar_t rcv_val __attribute__ ((packed));
	struct t_opthdr ckl_hdr __attribute__ ((packed));
	t_scalar_t ckl_val __attribute__ ((packed));
	struct t_opthdr skd_hdr __attribute__ ((packed));
	t_scalar_t skd_val __attribute__ ((packed));
	struct t_opthdr prt_hdr __attribute__ ((packed));
	t_scalar_t prt_val __attribute__ ((packed));
	struct t_opthdr art_hdr __attribute__ ((packed));
	t_scalar_t art_val __attribute__ ((packed));
	struct t_opthdr irt_hdr __attribute__ ((packed));
	t_scalar_t irt_val __attribute__ ((packed));
	struct t_opthdr hbi_hdr __attribute__ ((packed));
	t_scalar_t hbi_val __attribute__ ((packed));
	struct t_opthdr rin_hdr __attribute__ ((packed));
	t_scalar_t rin_val __attribute__ ((packed));
	struct t_opthdr rmn_hdr __attribute__ ((packed));
	t_scalar_t rmn_val __attribute__ ((packed));
	struct t_opthdr rmx_hdr __attribute__ ((packed));
	t_scalar_t rmx_val __attribute__ ((packed));
	struct t_opthdr ist_hdr __attribute__ ((packed));
	t_scalar_t ist_val __attribute__ ((packed));
	struct t_opthdr ost_hdr __attribute__ ((packed));
	t_scalar_t ost_val __attribute__ ((packed));
	struct t_opthdr cin_hdr __attribute__ ((packed));
	t_scalar_t cin_val __attribute__ ((packed));
	struct t_opthdr tin_hdr __attribute__ ((packed));
	t_scalar_t tin_val __attribute__ ((packed));
	struct t_opthdr mac_hdr __attribute__ ((packed));
	t_scalar_t mac_val __attribute__ ((packed));
	struct t_opthdr dbg_hdr __attribute__ ((packed));
	t_scalar_t dbg_val __attribute__ ((packed));
} opt_optm = {
	{
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_NODELAY, T_SUCCESS}
	, T_YES, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_CORK, T_SUCCESS}
	, T_YES, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS}
	, 10, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS}
	, 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RECVOPT, T_SUCCESS}
	, T_NO, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_LIFE, T_SUCCESS}
	, 60000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SACK_DELAY, T_SUCCESS}
	, 200, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PATH_MAX_RETRANS, T_SUCCESS}
	, 5, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ASSOC_MAX_RETRANS, T_SUCCESS}
	, 12, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAX_INIT_RETRIES, T_SUCCESS}
	, 12, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_HEARTBEAT_ITVL, T_SUCCESS}
	, 1000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_INITIAL, T_SUCCESS}
	, 200, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MIN, T_SUCCESS}
	, 10, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MAX, T_SUCCESS}
	, 2000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS, T_SUCCESS}
	, 1, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS, T_SUCCESS}
	, 1, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_INC, T_SUCCESS}
	, 1000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_THROTTLE_ITVL, T_SUCCESS}
	, 50, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAC_TYPE, T_SUCCESS}
	, T_SCTP_HMAC_NONE, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_DEBUG, T_SUCCESS}
, 0};

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

char *terrno_string(ulong terr, long uerr)
{
	switch (terr) {
	case TBADADDR:
		return ("[TBADADDR]");
	case TBADOPT:
		return ("[TBADOPT]");
	case TACCES:
		return ("[TACCES]");
	case TBADF:
		return ("[TBADF]");
	case TNOADDR:
		return ("[TNOADDR]");
	case TOUTSTATE:
		return ("[TOUTSTATE]");
	case TBADSEQ:
		return ("[TBADSEQ]");
	case TSYSERR:
		return errno_string(uerr);
	case TLOOK:
		return ("[TLOOK]");
	case TBADDATA:
		return ("[TBADDATA]");
	case TBUFOVFLW:
		return ("[TBUFOVFLW]");
	case TFLOW:
		return ("[TFLOW]");
	case TNODATA:
		return ("[TNODATA]");
	case TNODIS:
		return ("[TNODIS]");
	case TNOUDERR:
		return ("[TNOUDERR]");
	case TBADFLAG:
		return ("[TBADFLAG]");
	case TNOREL:
		return ("[TNOREL]");
	case TNOTSUPPORT:
		return ("[TNOTSUPPORT]");
	case TSTATECHNG:
		return ("[TSTATECHNG]");
	case TNOSTRUCTYPE:
		return ("[TNOSTRUCTYPE]");
	case TBADNAME:
		return ("[TBADNAME]");
	case TBADQLEN:
		return ("[TBADQLEN]");
	case TADDRBUSY:
		return ("[TADDRBUSY]");
	case TINDOUT:
		return ("[TINDOUT]");
	case TPROVMISMATCH:
		return ("[TPROVMISMATCH]");
	case TRESQLEN:
		return ("[TRESQLEN]");
	case TRESADDR:
		return ("[TRESADDR]");
	case TQFULL:
		return ("[TQFULL]");
	case TPROTO:
		return ("[TPROTO]");
	default:
	{
		static char buf[32];
		snprintf(buf, sizeof(buf), "[%lu]", terr);
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
	case __EVENT_CONN_REQ:
		return ("T_CONN_REQ");
	case __EVENT_CONN_RES:
		return ("T_CONN_RES");
	case __EVENT_DISCON_REQ:
		return ("T_DISCON_REQ");
	case __EVENT_DATA_REQ:
		return ("T_DATA_REQ");
	case __EVENT_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case __EVENT_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case __EVENT_INFO_REQ:
		return ("T_INFO_REQ");
	case __EVENT_BIND_REQ:
		return ("T_BIND_REQ");
	case __EVENT_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case __EVENT_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case __EVENT_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case __EVENT_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case __EVENT_CONN_IND:
		return ("T_CONN_IND");
	case __EVENT_CONN_CON:
		return ("T_CONN_CON");
	case __EVENT_DISCON_IND:
		return ("T_DISCON_IND");
	case __EVENT_DATA_IND:
		return ("T_DATA_IND");
	case __EVENT_EXDATA_IND:
		return ("T_EXDATA_IND");
	case __EVENT_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case __EVENT_INFO_ACK:
		return ("T_INFO_ACK");
	case __EVENT_BIND_ACK:
		return ("T_BIND_ACK");
	case __EVENT_ERROR_ACK:
		return ("T_ERROR_ACK");
	case __EVENT_OK_ACK:
		return ("T_OK_ACK");
	case __EVENT_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case __EVENT_UDERROR_IND:
		return ("T_UDERROR_IND");
	case __EVENT_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case __EVENT_ORDREL_IND:
		return ("T_ORDREL_IND");
	case __EVENT_ADDR_REQ:
		return ("T_ADDR_REQ");
	case __EVENT_ADDR_ACK:
		return ("T_ADDR_ACK");
	case __EVENT_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case __EVENT_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("(unexpected");
	}
}

const char *state_string(ulong state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WRES_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}

#ifndef SCTP_VERSION_2
void print_addr(char *add_ptr, size_t add_len)
{
	sctp_addr_t *a = (sctp_addr_t *) add_ptr;
	size_t anum = add_len >= sizeof(a->port) ? (add_len - sizeof(a->port)) / sizeof(a->addr[0]) : 0;
	lockf(fileno(stdout), F_LOCK, 0);
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
	lockf(fileno(stdout), F_ULOCK, 0);
}
char *addr_string(char *add_ptr, size_t add_len)
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
	len += snprintf(buf + len, sizeof(buf) - len, "\0");
	return buf;
}
void print_addrs(int fd, char *add_ptr, size_t add_len)
{
	fprintf(stdout, "Stupid!\n");
}
#else				/* SCTP_VERSION_2 */
void print_addr(char *add_ptr, size_t add_len)
{
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	size_t anum = add_len / sizeof(*a);
	lockf(fileno(stdout), F_LOCK, 0);
	if (add_len > 0) {
		int i;
		if (add_len != anum * sizeof(*a))
			fprintf(stdout, "Aaarrg! add_len = %d, anum = %d, ", add_len, anum);
		fprintf(stdout, "[%d]", ntohs(a[0].sin_port));
		for (i = 0; i < anum; i++) {
			uint32_t addr = a[i].sin_addr.s_addr;
			fprintf(stdout, "%s%d.%d.%d.%d.", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
		}
	} else
		fprintf(stdout, "(no address)");
	fprintf(stdout, "\n");
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}
char *addr_string(char *add_ptr, size_t add_len)
{
	static char buf[128];
	size_t len = 0;
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	size_t anum = add_len / sizeof(*a);
	if (add_len > 0) {
		int i;
		if (add_len != anum * sizeof(*a))
			len += snprintf(buf + len, sizeof(buf) - len, "Aaarrg! add_len = %d, anum = %d, ", add_len, anum);
		len += snprintf(buf + len, sizeof(buf) - len, "[%d]", ntohs(a[0].sin_port));
		for (i = 0; i < anum; i++) {
			uint32_t addr = a[i].sin_addr.s_addr;
			snprintf(buf + len, sizeof(buf) - len, "%s%d.%d.%d.%d.", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
		}
	} else
		len += snprintf(buf + len, sizeof(buf) - len, "(no address)");
	len += snprintf(buf + len, sizeof(buf) - len, "\0");
	return buf;
}
void print_addrs(int fd, char *add_ptr, size_t add_len)
{
	struct sockaddr_in *sin;
	if (verbose < 3)
		return;
	for (sin = (typeof(sin)) add_ptr; add_len >= sizeof(*sin); sin++, add_len -= sizeof(*sin)) {
		char buf[128];
		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%d", (sin->sin_addr.s_addr >> 0) & 0xff, (sin->sin_addr.s_addr >> 8) & 0xff, (sin->sin_addr.s_addr >> 16) & 0xff, (sin->sin_addr.s_addr >> 24) & 0xff, ntohs(sin->sin_port));
		if (fd == conn_fd) {
			fprintf(stdout, "%-20s|                               |  |                    \n", buf);
		}
		if (fd == resp_fd) {
			fprintf(stdout, "                    |                               |  |     %-15s\n", buf);
		}
		if (fd == list_fd) {
			fprintf(stdout, "                    |                               |  |     %-15s\n", buf);
		}
	}
}
#endif				/* SCTP_VERSION_2 */

char *status_string(struct t_opthdr *oh)
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
		return ("(unknown status)");
	}
}

char *level_string(struct t_opthdr *oh)
{
	switch (oh->level) {
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
		return ("(unknown level)");
	}
}

char *name_string(struct t_opthdr *oh)
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
	return ("(unknown name)");
}

char *yesno_string(struct t_opthdr *oh)
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

char *number_string(struct t_opthdr *oh)
{
	static char buf[32];
	snprintf(buf, 32, "%d\0", *((t_scalar_t *) T_OPT_DATA(oh)));
	return (buf);
}

char *value_string(int fd, struct t_opthdr *oh)
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
			if (oh->len = sizeof(*oh) + sizeof(unsigned char))
				snprintf(buf, sizeof(buf), "0x%02x", *((unsigned char *) T_OPT_DATA(oh)));
			return buf;
		case T_IP_TTL:
			if (oh->len = sizeof(*oh) + sizeof(unsigned char))
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
				snprintf(buf, sizeof(buf), "%lu", *((t_uscalar_t *) T_OPT_DATA(oh)));
			return buf;
		case T_TCP_KEEPALIVE:
			return yesno_string(oh);
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
			sid[fd] = *((t_uscalar_t *)T_OPT_DATA(oh));
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
			return number_string(oh);;
		case T_SCTP_STATUS:
			break;
		case T_SCTP_DEBUG:
			break;
		}
		break;
	}
	return ("(unknown value)");
}

void print_options(int fd, char *opt_ptr, size_t opt_len)
{
	struct t_opthdr *oh;
	for (oh = _T_OPT_FIRSTHDR_OFS(opt_ptr, opt_len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(opt_ptr, opt_len, oh, 0)) {
		char *level = level_string(oh);
		char *name = name_string(oh);
		char *status = status_string(oh);
		char *value = value_string(fd, oh);
		int len = oh->len - sizeof(*oh);
		unsigned char *val = _T_OPT_DATA_OFS(oh, 0);
		if (len < 0)
			break;
		if (verbose < 4)
			continue;
		if (fd == conn_fd) {
#if 0
			fprintf(stdout, "%-20s|                               |  |                    \n", level);
#endif
			fprintf(stdout, "%-20s|                               |  |                    \n", name);
			if (status)
				fprintf(stdout, "%-20s|                               |  |                    \n", status);
			if (value)
				fprintf(stdout, "%-20s|                               |  |                    \n", value);
#if 0
			fprintf(stdout, "len = %03d           |                               |  |                    \n", len);
#endif
		}
		if (fd == resp_fd) {
#if 0
			fprintf(stdout, "                    |                               |  |     %-15s\n", level);
#endif
			fprintf(stdout, "                    |                               |  |     %-15s\n", name);
			if (status)
				fprintf(stdout, "                    |                               |  |     %-15s\n", status);
			if (value)
				fprintf(stdout, "                    |                               |  |     %-15s\n", value);
#if 0
			fprintf(stdout, "                    |                               |  |     len = %03d      \n", len);
#endif
		}
		if (fd == list_fd) {
#if 0
			fprintf(stdout, "                    |                               |  |     %-15s\n", level);
#endif
			fprintf(stdout, "                    |                               |  |     %-15s\n", name);
			if (status)
				fprintf(stdout, "                    |                               |  |     %-15s\n", status);
			if (value)
				fprintf(stdout, "                    |                               |  |     %-15s\n", value);
#if 0
			fprintf(stdout, "                    |                               |  |     len = %03d      \n", len);
#endif
		}
	}
}

void parse_options(int fd, char *opt_ptr, size_t opt_len)
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

char *mgmtflag_string(t_uscalar_t flag)
{
	switch (flag) {
	case T_NEGOTIATE:
		return ("T_NEGOTIATE");
	case T_CHECK:
		return ("T_CHECK");
	case T_DEFAULT:
		return ("T_DEFAULT");
	case T_SUCCESS:
		return ("T_SUCCESS");
	case T_FAILURE:
		return ("T_FAILURE");
	case T_CURRENT:
		return ("T_CURRENT");
	case T_PARTSUCCESS:
		return ("T_PARTSUCCESS");
	case T_READONLY:
		return ("T_READONLY");
	case T_NOTSUPPORT:
		return ("T_NOTSUPPORT");
	}
	return "(unknown flag)";
}

void print_mgmtflag(int fd, t_uscalar_t flag)
{
	if (verbose < 2)
		return;
	if (fd == conn_fd) {
		fprintf(stdout, "%-20s|                               |  |                    \n", mgmtflag_string(flag));
	}
	if (fd == resp_fd) {
		fprintf(stdout, "                    |                               |  |     %-15s\n", mgmtflag_string(flag));
	}
	if (fd == list_fd) {
		fprintf(stdout, "                    |                               |  |     %-15s\n", mgmtflag_string(flag));
	}
}

char *size_string(ulong size)
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
	snprintf(buf, sizeof(buf), "%lu", size);
	return buf;
}

void print_event_conn(int fd, int event)
{
	switch (event) {
	case __EVENT_INFO_REQ:
		fprintf(stdout, "T_INFO_REQ    ----->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_INFO_ACK:
		fprintf(stdout, "T_INFO_ACK    <----/|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_OPTMGMT_REQ:
		fprintf(stdout, "T_OPTMGMT_REQ ----->|                               |  |                    [%d]\n", state);
		print_mgmtflag(fd, cmd.tpi.optmgmt_req.MGMT_flags);
		print_options(fd, cmd.cbuf + cmd.tpi.optmgmt_req.OPT_offset, cmd.tpi.optmgmt_req.OPT_length);
		break;
	case __EVENT_OPTMGMT_ACK:
		fprintf(stdout, "T_OPTMGMT_ACK <----/|                               |  |                    [%d]\n", state);
		print_mgmtflag(fd, cmd.tpi.optmgmt_ack.MGMT_flags);
		print_options(fd, cmd.cbuf + cmd.tpi.optmgmt_ack.OPT_offset, cmd.tpi.optmgmt_ack.OPT_length);
		break;
	case __EVENT_BIND_REQ:
		fprintf(stdout, "T_BIND_REQ    ----->|                               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.bind_req.ADDR_offset, cmd.tpi.bind_req.ADDR_length);
		break;
	case __EVENT_BIND_ACK:
		fprintf(stdout, "T_BIND_ACK    <----/|                               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.bind_ack.ADDR_offset, cmd.tpi.bind_ack.ADDR_length);
		break;
	case __EVENT_ADDR_REQ:
		fprintf(stdout, "T_ADDR_REQ    ----->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_ADDR_ACK:
		fprintf(stdout, "T_ADDR_ACK    <----/|                               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.addr_ack.LOCADDR_offset, cmd.tpi.addr_ack.LOCADDR_length);
		print_addrs(fd, cmd.cbuf + cmd.tpi.addr_ack.REMADDR_offset, cmd.tpi.addr_ack.REMADDR_length);
		break;
	case __EVENT_UNBIND_REQ:
		fprintf(stdout, "T_UNBIND_REQ  ----->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_ERROR_ACK:
		fprintf(stdout, "T_ERROR_ACK   <----/|                               |  |                    [%d]\n", state);
		fprintf(stdout, "%-15s     |                               |  |                    \n", terrno_string(cmd.tpi.error_ack.TLI_error, cmd.tpi.error_ack.UNIX_error));
		break;
	case __EVENT_OK_ACK:
		fprintf(stdout, "T_OK_ACK      <----/|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_UNITDATA_REQ:
		fprintf(stdout, "T_UNITDATA_REQ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.unitdata_req.DEST_offset, cmd.tpi.unitdata_req.DEST_length);
		print_options(fd, cmd.cbuf + cmd.tpi.unitdata_req.OPT_offset, cmd.tpi.unitdata_req.OPT_length);
		break;
	case __EVENT_UNITDATA_IND:
		fprintf(stdout, "T_UNITDATA_IND<-----|<- - - - - - - /               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.unitdata_ind.SRC_offset, cmd.tpi.unitdata_ind.SRC_length);
		print_options(fd, cmd.cbuf + cmd.tpi.unitdata_ind.OPT_offset, cmd.tpi.unitdata_ind.OPT_length);
		break;
	case __EVENT_UDERROR_IND:
		fprintf(stdout, "T_UDERROR_IND <----/|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_CONN_REQ:
		fprintf(stdout, "T_CONN_REQ    ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_req.DEST_offset, cmd.tpi.conn_req.DEST_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_req.OPT_offset, cmd.tpi.conn_req.OPT_length);
		break;
	case __EVENT_CONN_IND:
		fprintf(stdout, "T_CONN_IND    <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_ind.SRC_offset, cmd.tpi.conn_ind.SRC_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_ind.OPT_offset, cmd.tpi.conn_ind.OPT_length);
		seq[fd] = cmd.tpi.conn_ind.SEQ_number;
		break;
	case __EVENT_CONN_RES:
		fprintf(stdout, "T_CONN_RES    ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_res.OPT_offset, cmd.tpi.conn_res.OPT_length);
		break;
	case __EVENT_CONN_CON:
		fprintf(stdout, "T_CONN_CON    <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_con.RES_offset, cmd.tpi.conn_con.RES_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_con.OPT_offset, cmd.tpi.conn_con.OPT_length);
		break;
	case __EVENT_DATA_REQ:
		if (cmd.tpi.data_req.MORE_flag)
			fprintf(stdout, "T_DATA_REQ+   ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		else
			fprintf(stdout, "T_DATA_REQ    ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_DATA_IND:
		if (cmd.tpi.data_ind.MORE_flag)
			fprintf(stdout, "T_DATA_IND+   <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		else
			fprintf(stdout, "T_DATA_IND    <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_EXDATA_REQ:
		if (cmd.tpi.exdata_req.MORE_flag)
			fprintf(stdout, "T_EXDATA_REQ+ ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		else
			fprintf(stdout, "T_EXDATA_REQ  ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_EXDATA_IND:
		if (cmd.tpi.exdata_ind.MORE_flag)
			fprintf(stdout, "T_EXDATA_IND+ <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		else
			fprintf(stdout, "T_EXDATA_IND  <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_OPTDATA_REQ:
		parse_options(fd, cmd.cbuf + cmd.tpi.optdata_req.OPT_offset, cmd.tpi.optdata_req.OPT_length);
		if (cmd.tpi.optdata_req.DATA_flag & T_ODF_EX) {
			if (cmd.tpi.optdata_req.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "T_OPTDATA_REQ+----->| - (%03d:-U-) ->\\               |  |                    [%d]\n", sid[fd], state);
			else
				fprintf(stdout, "T_OPTDATA_REQ ----->| - (%03d:-U-) ->\\               |  |                    [%d]\n", sid[fd], state);
		} else {
			if (cmd.tpi.optdata_req.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "T_OPTDATA_REQ+----->| - (%03d:%03d) ->\\               |  |                    [%d]\n", sid[fd], ssn[fd], state);
			else
				fprintf(stdout, "T_OPTDATA_REQ ----->| - (%03d:%03d) ->\\               |  |                    [%d]\n", sid[fd], ssn[fd], state);
		}
		print_options(fd, cmd.cbuf + cmd.tpi.optdata_req.OPT_offset, cmd.tpi.optdata_req.OPT_length);
		break;
	case __EVENT_OPTDATA_IND:
		parse_options(fd, cmd.cbuf + cmd.tpi.optdata_ind.OPT_offset, cmd.tpi.optdata_ind.OPT_length);
		if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX) {
			exp[fd] = 1;
			if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "T_OPTDATA_IND+<-----|<- -(%03d:-U-)- /               |  |                    [%d]\n", sid[fd], state);
			else
				fprintf(stdout, "T_OPTDATA_IND <-----|<- -(%03d:-U-)- /               |  |                    [%d]\n", sid[fd], state);
		} else {
			exp[fd] = 0;
			if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "T_OPTDATA_IND+<-----|<- -(%03d:%03d)- /               |  |                    [%d]\n", sid[fd], ssn[fd], state);
			else
				fprintf(stdout, "T_OPTDATA_IND <-----|<- -(%03d:%03d)- /               |  |                    [%d]\n", sid[fd], ssn[fd], state);
		}
		print_options(fd, cmd.cbuf + cmd.tpi.optdata_ind.OPT_offset, cmd.tpi.optdata_ind.OPT_length);
		break;
	case __EVENT_DISCON_REQ:
		fprintf(stdout, "T_DISCON_REQ  ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_DISCON_IND:
		fprintf(stdout, "T_DISCON_IND  <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_ORDREL_REQ:
		fprintf(stdout, "T_ORDREL_REQ  ----->| - - - - - - ->\\               |  |                    [%d]\n", state);
		break;
	case __EVENT_ORDREL_IND:
		fprintf(stdout, "T_ORDREL_IND  <-----|<- - - - - - - /               |  |                    [%d]\n", state);
		break;
	case __EVENT_CAPABILITY_REQ:
		fprintf(stdout, "T_CAPABILITY_REQ--->|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_CAPABILITY_ACK:
		fprintf(stdout, "T_CAPABILITY_ACK<--/|                               |  |                    [%d]\n", state);
		break;
	case __EVENT_UNKNOWN:
		fprintf(stdout, "????%4ld????  ?----?|?- - - - - - -?                |  |                    [%d]\n", cmd.tpi.type, state);
		break;
	default:
	case __RESULT_SCRIPT_ERROR:
		break;
	}
}

void print_event_resp(int fd, int event)
{
	switch (event) {
	case __EVENT_INFO_REQ:
		fprintf(stdout, "                    |                               |  |<--- T_INFO_REQ     [%d]\n", state);
		break;
	case __EVENT_INFO_ACK:
		fprintf(stdout, "                    |                               |  |\\--> T_INFO_ACK---> [%d]\n", state);
		break;
	case __EVENT_OPTMGMT_REQ:
		fprintf(stdout, "                    |                               |  |<--- T_OPTMGMT_REQ  [%d]\n", state);
		print_mgmtflag(fd, cmd.tpi.optmgmt_req.MGMT_flags);
		print_options(fd, cmd.cbuf + cmd.tpi.optmgmt_req.OPT_offset, cmd.tpi.optmgmt_req.OPT_length);
		break;
	case __EVENT_OPTMGMT_ACK:
		fprintf(stdout, "                    |                               |  |\\--> T_OPTMGMT_ACK  [%d]\n", state);
		print_mgmtflag(fd, cmd.tpi.optmgmt_ack.MGMT_flags);
		print_options(fd, cmd.cbuf + cmd.tpi.optmgmt_ack.OPT_offset, cmd.tpi.optmgmt_ack.OPT_length);
		break;
	case __EVENT_BIND_REQ:
		fprintf(stdout, "                    |                               |  |<--- T_BIND_REQ     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.bind_req.ADDR_offset, cmd.tpi.bind_req.ADDR_length);
		break;
	case __EVENT_BIND_ACK:
		fprintf(stdout, "                    |                               |  |\\--> T_BIND_ACK     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.bind_ack.ADDR_offset, cmd.tpi.bind_ack.ADDR_length);
		break;
	case __EVENT_ADDR_REQ:
		fprintf(stdout, "                    |                               |  |<--- T_ADDR_REQ     [%d]\n", state);
		break;
	case __EVENT_ADDR_ACK:
		fprintf(stdout, "                    |                               |  |\\--> T_ADDR_ACK     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.addr_ack.LOCADDR_offset, cmd.tpi.addr_ack.LOCADDR_length);
		print_addrs(fd, cmd.cbuf + cmd.tpi.addr_ack.REMADDR_offset, cmd.tpi.addr_ack.REMADDR_length);
		break;
	case __EVENT_UNBIND_REQ:
		fprintf(stdout, "                    |                               |  |<--- T_UNBIND_REQ   [%d]\n", state);
		break;
	case __EVENT_ERROR_ACK:
		fprintf(stdout, "                    |                               |  |\\--> T_ERROR_ACK    [%d]\n", state);
		fprintf(stdout, "                    |                               |  |     %-15s\n", terrno_string(cmd.tpi.error_ack.TLI_error, cmd.tpi.error_ack.UNIX_error));
		break;
	case __EVENT_OK_ACK:
		fprintf(stdout, "                    |                               |  |\\--> T_OK_ACK       [%d]\n", state);
		break;
	case __EVENT_UNITDATA_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_UNITDATA_REQ [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.unitdata_req.DEST_offset, cmd.tpi.unitdata_req.DEST_length);
		print_options(fd, cmd.cbuf + cmd.tpi.unitdata_req.OPT_offset, cmd.tpi.unitdata_req.OPT_length);
		break;
	case __EVENT_UNITDATA_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_UNITDATA_IND [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.unitdata_ind.SRC_offset, cmd.tpi.unitdata_ind.SRC_length);
		print_options(fd, cmd.cbuf + cmd.tpi.unitdata_ind.OPT_offset, cmd.tpi.unitdata_ind.OPT_length);
		break;
	case __EVENT_UDERROR_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|\\--> T_UDERROR_IND  [%d]\n", state);
		break;
	case __EVENT_CONN_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_CONN_REQ     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_req.DEST_offset, cmd.tpi.conn_req.DEST_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_req.OPT_offset, cmd.tpi.conn_req.OPT_length);
		break;
	case __EVENT_CONN_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_CONN_IND     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_ind.SRC_offset, cmd.tpi.conn_ind.SRC_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_ind.OPT_offset, cmd.tpi.conn_ind.OPT_length);
		seq[fd] = cmd.tpi.conn_ind.SEQ_number;
		break;
	case __EVENT_CONN_RES:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_CONN_RES     [%d]\n", state);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_res.OPT_offset, cmd.tpi.conn_res.OPT_length);
		break;
	case __EVENT_CONN_CON:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_CONN_CON     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_con.RES_offset, cmd.tpi.conn_con.RES_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_con.OPT_offset, cmd.tpi.conn_con.OPT_length);
		break;
	case __EVENT_DATA_REQ:
		if (cmd.tpi.data_req.MORE_flag)
			fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_DATA_REQ+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_DATA_REQ     [%d]\n", state);
		break;
	case __EVENT_DATA_IND:
		if (cmd.tpi.data_ind.MORE_flag)
			fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_DATA_IND+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_DATA_IND     [%d]\n", state);
		break;
	case __EVENT_EXDATA_REQ:
		if (cmd.tpi.exdata_req.MORE_flag)
			fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_EXDATA_REQ+  [%d]\n", state);
		else
			fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_EXDATA_REQ   [%d]\n", state);
		break;
	case __EVENT_EXDATA_IND:
		if (cmd.tpi.exdata_ind.MORE_flag)
			fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_EXDATA_IND+  [%d]\n", state);
		else
			fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_EXDATA_IND   [%d]\n", state);
		break;
	case __EVENT_OPTDATA_REQ:
		parse_options(fd, cmd.cbuf + cmd.tpi.optdata_req.OPT_offset, cmd.tpi.optdata_req.OPT_length);
		if (cmd.tpi.optdata_req.DATA_flag & T_ODF_EX) {
			if (cmd.tpi.optdata_req.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               /<- - (%03d:-U-) + -|<--- T_OPTDATA_REQ+ [%d]\n", sid[fd], state);
			else
				fprintf(stdout, "                    |               /<- - (%03d:-U-) + -|<--- T_OPTDATA_REQ  [%d]\n", sid[fd], state);
		} else {
			if (cmd.tpi.optdata_req.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               /<- - (%03d:%03d) + -|<--- T_OPTDATA_REQ+ [%d]\n", sid[fd], ssn[fd], state);
			else
				fprintf(stdout, "                    |               /<- - (%03d:%03d) + -|<--- T_OPTDATA_REQ  [%d]\n", sid[fd], ssn[fd], state);
		}
		print_options(fd, cmd.cbuf + cmd.tpi.optdata_req.OPT_offset, cmd.tpi.optdata_req.OPT_length);
		break;
	case __EVENT_OPTDATA_IND:
		parse_options(fd, cmd.cbuf + cmd.tpi.optdata_ind.OPT_offset, cmd.tpi.optdata_ind.OPT_length);
		if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX) {
			exp[fd] = 1;
			if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               \\ - - (%03d:-U-) +->|---> T_OPTDATA_IND+ [%d]\n", sid[fd], state);
			else
				fprintf(stdout, "                    |               \\ - - (%03d:-U-) +->|---> T_OPTDATA_IND  [%d]\n", sid[fd], state);
		} else {
			exp[fd] = 0;
			if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               \\ - - (%03d:%03d) +->|---> T_OPTDATA_IND+ [%d]\n", sid[fd], ssn[fd], state);
			else
				fprintf(stdout, "                    |               \\ - - (%03d:%03d) +->|---> T_OPTDATA_IND  [%d]\n", sid[fd], ssn[fd], state);
		}
		print_options(fd, cmd.cbuf + cmd.tpi.optdata_ind.OPT_offset, cmd.tpi.optdata_ind.OPT_length);
		break;
	case __EVENT_DISCON_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_DISCON_REQ   [%d]\n", state);
		break;
	case __EVENT_DISCON_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_DISCON_IND   [%d]\n", state);
		break;
	case __EVENT_ORDREL_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - + -|<--- T_ORDREL_REQ   [%d]\n", state);
		break;
	case __EVENT_ORDREL_IND:
		fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_ORDREL_IND   [%d]\n", state);
		break;
	case __EVENT_CAPABILITY_REQ:
		fprintf(stdout, "                    |                               |  |<---T_CAPABILITY_REQ[%d]\n", state);
		break;
	case __EVENT_CAPABILITY_ACK:
		fprintf(stdout, "                    |                               |  |\\-->T_CAPABILITY_ACK[%d]\n", state);
		break;
	case __EVENT_UNKNOWN:
		fprintf(stdout, "                    |                               |  |?--? ????%4ld????   [%d]\n", cmd.tpi.type, state);
		break;
	default:
	case __RESULT_SCRIPT_ERROR:
		break;
	}
}

void print_event_list(int fd, int event)
{
	switch (event) {
	case __EVENT_INFO_REQ:
		fprintf(stdout, "                    |                               |<-+---- T_INFO_REQ     [%d]\n", state);
		break;
	case __EVENT_INFO_ACK:
		fprintf(stdout, "                    |                               |\\-+---> T_INFO_ACK     [%d]\n", state);
		break;
	case __EVENT_OPTMGMT_REQ:
		fprintf(stdout, "                    |                               |<-+---- T_OPTMGMT_REQ  [%d]\n", state);
		print_mgmtflag(fd, cmd.tpi.optmgmt_req.MGMT_flags);
		print_options(fd, cmd.cbuf + cmd.tpi.optmgmt_req.OPT_offset, cmd.tpi.optmgmt_req.OPT_length);
		break;
	case __EVENT_OPTMGMT_ACK:
		fprintf(stdout, "                    |                               |\\-+---- T_OPTMGMT_ACK  [%d]\n", state);
		print_mgmtflag(fd, cmd.tpi.optmgmt_ack.MGMT_flags);
		print_options(fd, cmd.cbuf + cmd.tpi.optmgmt_ack.OPT_offset, cmd.tpi.optmgmt_ack.OPT_length);
		break;
	case __EVENT_BIND_REQ:
		fprintf(stdout, "                    |                               |<-+---- T_BIND_REQ     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.bind_req.ADDR_offset, cmd.tpi.bind_req.ADDR_length);
		break;
	case __EVENT_BIND_ACK:
		fprintf(stdout, "                    |                               |\\-+---> T_BIND_ACK     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.bind_ack.ADDR_offset, cmd.tpi.bind_ack.ADDR_length);
		break;
	case __EVENT_ADDR_REQ:
		fprintf(stdout, "                    |                               |<-+---- T_ADDR_REQ     [%d]\n", state);
		break;
	case __EVENT_ADDR_ACK:
		fprintf(stdout, "                    |                               |\\-+---> T_ADDR_ACK     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.addr_ack.LOCADDR_offset, cmd.tpi.addr_ack.LOCADDR_length);
		print_addrs(fd, cmd.cbuf + cmd.tpi.addr_ack.REMADDR_offset, cmd.tpi.addr_ack.REMADDR_length);
		break;
	case __EVENT_UNBIND_REQ:
		fprintf(stdout, "                    |                               |<-+---- T_UNBIND_REQ   [%d]\n", state);
		break;
	case __EVENT_ERROR_ACK:
		fprintf(stdout, "                    |                               |\\-+---> T_ERROR_ACK    [%d]\n", state);
		fprintf(stdout, "                    |                               |   .    %-15s\n", terrno_string(cmd.tpi.error_ack.TLI_error, cmd.tpi.error_ack.UNIX_error));
		break;
	case __EVENT_OK_ACK:
		fprintf(stdout, "                    |                               |\\-+---> T_OK_ACK       [%d]\n", state);
		break;
	case __EVENT_UNITDATA_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_UNITDATA_REQ [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.unitdata_req.DEST_offset, cmd.tpi.unitdata_req.DEST_length);
		print_options(fd, cmd.cbuf + cmd.tpi.unitdata_req.OPT_offset, cmd.tpi.unitdata_req.OPT_length);
		break;
	case __EVENT_UNITDATA_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_UNITDATA_IND [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.unitdata_ind.SRC_offset, cmd.tpi.unitdata_ind.SRC_length);
		print_options(fd, cmd.cbuf + cmd.tpi.unitdata_ind.OPT_offset, cmd.tpi.unitdata_ind.OPT_length);
		break;
	case __EVENT_UDERROR_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|\\-+---> T_UDERROR_IND  [%d]\n", state);
		break;
	case __EVENT_CONN_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_CONN_REQ     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_req.DEST_offset, cmd.tpi.conn_req.DEST_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_req.OPT_offset, cmd.tpi.conn_req.OPT_length);
		break;
	case __EVENT_CONN_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_CONN_IND     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_ind.SRC_offset, cmd.tpi.conn_ind.SRC_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_ind.OPT_offset, cmd.tpi.conn_ind.OPT_length);
		seq[fd] = cmd.tpi.conn_ind.SEQ_number;
		break;
	case __EVENT_CONN_RES:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_CONN_RES     [%d]\n", state);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_res.OPT_offset, cmd.tpi.conn_res.OPT_length);
		break;
	case __EVENT_CONN_CON:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_CONN_CON     [%d]\n", state);
		print_addrs(fd, cmd.cbuf + cmd.tpi.conn_con.RES_offset, cmd.tpi.conn_con.RES_length);
		print_options(fd, cmd.cbuf + cmd.tpi.conn_con.OPT_offset, cmd.tpi.conn_con.OPT_length);
		break;
	case __EVENT_DATA_REQ:
		if (cmd.tpi.data_req.MORE_flag)
			fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_DATA_REQ+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_DATA_REQ     [%d]\n", state);
		break;
	case __EVENT_DATA_IND:
		if (cmd.tpi.data_ind.MORE_flag)
			fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_DATA_IND+    [%d]\n", state);
		else
			fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_DATA_IND     [%d]\n", state);
		break;
	case __EVENT_EXDATA_REQ:
		if (cmd.tpi.exdata_req.MORE_flag)
			fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_EXDATA_REQ+  [%d]\n", state);
		else
			fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_EXDATA_REQ   [%d]\n", state);
		break;
	case __EVENT_EXDATA_IND:
		if (cmd.tpi.exdata_ind.MORE_flag)
			fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_EXDATA_IND+  [%d]\n", state);
		else
			fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_EXDATA_IND   [%d]\n", state);
		break;
	case __EVENT_OPTDATA_REQ:
		if (cmd.tpi.optdata_req.DATA_flag & T_ODF_EX) {
			if (cmd.tpi.optdata_req.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               /<- -(%03d:-U-)- |<-+---- T_OPTDATA_REQ+ [%d]\n", sid[fd], state);
			else
				fprintf(stdout, "                    |               /<- -(%03d:-U-)- |<-+---- T_OPTDATA_REQ  [%d]\n", sid[fd], state);
		} else {
			if (cmd.tpi.optdata_req.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               /<- -(%03d:%03d)- |<-+---- T_OPTDATA_REQ+ [%d]\n", sid[fd], ssn[fd], state);
			else
				fprintf(stdout, "                    |               /<- -(%03d:%03d)- |<-+---- T_OPTDATA_REQ  [%d]\n", sid[fd], ssn[fd], state);
		}
		print_options(fd, cmd.cbuf + cmd.tpi.optdata_req.OPT_offset, cmd.tpi.optdata_req.OPT_length);
		break;
	case __EVENT_OPTDATA_IND:
		if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX) {
			exp[fd] = 1;
			if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               \\ - -(%03d:-U-)->|--+---> T_OPTDATA_IND+ [%d]\n", sid[fd], state);
			else
				fprintf(stdout, "                    |               \\ - -(%03d:-U-)->|--+---> T_OPTDATA_IND  [%d]\n", sid[fd], state);
		} else {
			exp[fd] = 0;
			if (cmd.tpi.optdata_ind.DATA_flag & T_ODF_MORE)
				fprintf(stdout, "                    |               \\ - -(%03d:%03d)->|--+---> T_OPTDATA_IND+ [%d]\n", sid[fd], ssn[fd], state);
			else
				fprintf(stdout, "                    |               \\ - -(%03d:%03d)->|--+---> T_OPTDATA_IND  [%d]\n", sid[fd], ssn[fd], state);
		}
		print_options(fd, cmd.cbuf + cmd.tpi.optdata_ind.OPT_offset, cmd.tpi.optdata_ind.OPT_length);
		break;
	case __EVENT_DISCON_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_DISCON_REQ   [%d]\n", state);
		break;
	case __EVENT_DISCON_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_DISCON_IND   [%d]\n", state);
		break;
	case __EVENT_ORDREL_REQ:
		fprintf(stdout, "                    |               /<- - - - - - - |<-+---- T_ORDREL_REQ   [%d]\n", state);
		break;
	case __EVENT_ORDREL_IND:
		fprintf(stdout, "                    |               \\ - - - - - - ->|--+---> T_ORDREL_IND   [%d]\n", state);
		break;
	case __EVENT_CAPABILITY_REQ:
		fprintf(stdout, "                    |                               |<-+----T_CAPABILITY_REQ[%d]\n", state);
		break;
	case __EVENT_CAPABILITY_ACK:
		fprintf(stdout, "                    |                               |\\-+--->T_CAPABILITY_ACK[%d]\n", state);
		break;
	case __EVENT_UNKNOWN:
		fprintf(stdout, "                    |                               |?-+---? ????%4ld????   [%d]\n", cmd.tpi.type, state);
		break;
	default:
	case __RESULT_SCRIPT_ERROR:
		break;
	}
}

void print_event(int fd, int event)
{
	if (verbose < 1 || !show)
		return;
	lockf(fileno(stdout), F_LOCK, 0);
	if (fd == conn_fd)
		print_event_conn(fd, event);
	else if (fd == resp_fd)
		print_event_resp(fd, event);
	else if (fd == list_fd)
		print_event_list(fd, event);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return;
}

void print_less(int fd)
{
	if (verbose < 1 || !show)
		return;
	lockf(fileno(stdout), F_LOCK, 0);
	if (fd == conn_fd) {
		fprintf(stdout, " .         .  <---->|               .               :  :                    \n");
		fprintf(stdout, " .  (more) .  <---->|               .               :  :                    [%d]\n", state);
		fprintf(stdout, " .         .  <---->|               .               :  :                    \n");
	} else if (fd == resp_fd) {
		fprintf(stdout, "                    :               .               :  |<-->  .         .   \n");
		fprintf(stdout, "                    :               .               :  |<-->  . (more)  .   [%d]\n", state);
		fprintf(stdout, "                    :               .               :  |<-->  .         .   \n");
	} else if (fd == list_fd) {
		fprintf(stdout, "                    :               .               |<-:--->  .         .   \n");
		fprintf(stdout, "                    :               .               |<-:--->  . (more)  .   [%d]\n", state);
		fprintf(stdout, "                    :               .               |<-:--->  .         .   \n");
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

void print_open(int *fdp)
{
	if (verbose > 3) {
		lockf(fileno(stdout), F_LOCK, 0);
		if (fdp == &conn_fd)
			fprintf(stdout, "open()        ----->v                                                       \n");
		else if (fdp == &resp_fd)
			fprintf(stdout, "                    |                                  v<--- open()         \n");
		else if (fdp == &list_fd)
			fprintf(stdout, "                    |                               v<-|---- open()         \n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void print_close(int *fdp)
{
	if (verbose > 3) {
		lockf(fileno(stdout), F_LOCK, 0);
		if (fdp == &conn_fd)
			fprintf(stdout, "close()       ----->X                               |  |                    \n");
		else if (fdp == &resp_fd)
			fprintf(stdout, "                                                    |  X<--- close()        \n");
		else if (fdp == &list_fd)
			fprintf(stdout, "                                                    X<------ close()        \n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void print_command(int fd, const char *command)
{
	if (verbose > 3) {
		lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "%-14s----->|                               |  |                    [%d]\n", command, state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    |                               |  |<--- %-14s [%d]\n", command, state);
		else if (fd == list_fd)
			fprintf(stdout, "                    |                               |<-+---- %-14s [%d]\n", command, state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void print_errno(int fd, long error)
{
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "%-14s<----/|                               |  |                    [%d]\n", errno_string(error), state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    |                               |  |\\--> %-14s [%d]\n", errno_string(error), state);
		else if (fd == list_fd)
			fprintf(stdout, "                    |                               |\\-+---> %-14s [%d]\n", errno_string(error), state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void print_success(int fd)
{
	if (verbose > 4) {
		lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "ok            <----/|                               |  |                    [%d]\n", state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    |                               |  |\\--> ok             [%d]\n", state);
		else if (fd == list_fd)
			fprintf(stdout, "                    |                               |\\-+---> ok             [%d]\n", state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
}

static int decode_data(int fd)
{
	int event = __RESULT_DECODE_ERROR;
	if (data.len >= 0)
		event = __EVENT_DATA;
	if (verbose > 0 && show)
		print_event(fd, event);
	return ((last_event = event));
}

static int decode_ctrl(int fd)
{
	int event = __RESULT_DECODE_ERROR;
	if (ctrl.len >= sizeof(cmd.tpi.type))
		switch ((last_prim = cmd.tpi.type)) {
		case T_CONN_REQ:
			event = __EVENT_CONN_REQ;
			break;
		case T_CONN_RES:
			event = __EVENT_CONN_RES;
			break;
		case T_DISCON_REQ:
			event = __EVENT_DISCON_REQ;
			break;
		case T_DATA_REQ:
			event = __EVENT_DATA_REQ;
			break;
		case T_EXDATA_REQ:
			event = __EVENT_EXDATA_REQ;
			break;
		case T_INFO_REQ:
			event = __EVENT_INFO_REQ;
			break;
		case T_BIND_REQ:
			event = __EVENT_BIND_REQ;
			break;
		case T_UNBIND_REQ:
			event = __EVENT_UNBIND_REQ;
			break;
		case T_UNITDATA_REQ:
			event = __EVENT_UNITDATA_REQ;
			break;
		case T_OPTMGMT_REQ:
			event = __EVENT_OPTMGMT_REQ;
			break;
		case T_ORDREL_REQ:
			event = __EVENT_ORDREL_REQ;
			break;
		case T_OPTDATA_REQ:
			event = __EVENT_OPTDATA_REQ;
			break;
		case T_ADDR_REQ:
			event = __EVENT_ADDR_REQ;
			break;
		case T_CAPABILITY_REQ:
			event = __EVENT_CAPABILITY_REQ;
			break;
		case T_CONN_IND:
			event = __EVENT_CONN_IND;
			break;
		case T_CONN_CON:
			event = __EVENT_CONN_CON;
			break;
		case T_DISCON_IND:
			event = __EVENT_DISCON_IND;
			break;
		case T_DATA_IND:
			event = __EVENT_DATA_IND;
			break;
		case T_EXDATA_IND:
			event = __EVENT_EXDATA_IND;
			break;
		case T_INFO_ACK:
			event = __EVENT_INFO_ACK;
			break;
		case T_BIND_ACK:
			event = __EVENT_BIND_ACK;
			break;
		case T_ERROR_ACK:
			event = __EVENT_ERROR_ACK;
			break;
		case T_OK_ACK:
			event = __EVENT_OK_ACK;
			break;
		case T_UNITDATA_IND:
			event = __EVENT_UNITDATA_IND;
			break;
		case T_UDERROR_IND:
			event = __EVENT_UDERROR_IND;
			break;
		case T_OPTMGMT_ACK:
			event = __EVENT_OPTMGMT_ACK;
			break;
		case T_ORDREL_IND:
			event = __EVENT_ORDREL_IND;
			break;
		case T_OPTDATA_IND:
			event = __EVENT_OPTDATA_IND;
			break;
		case T_ADDR_ACK:
			event = __EVENT_ADDR_ACK;
			break;
		case T_CAPABILITY_ACK:
			event = __EVENT_CAPABILITY_ACK;
			break;
		default:
			event = __EVENT_UNKNOWN;
			break;
		}
	if (verbose > 0 && show)
		print_event(fd, event);
	return ((last_event = event));
}

static int decode_msg(int fd)
{
	if (ctrl.len >= 0) {
		if ((last_event = decode_ctrl(fd)) != __EVENT_UNKNOWN)
			return time_event(last_event);
	} else if (data.len >= 0) {
		if ((last_event = decode_data(fd)) != __EVENT_UNKNOWN)
			return time_event(last_event);
	}
	return ((last_event = __EVENT_NO_MSG));
}

int wait_event(int fd, int wait)
{
	for (;;) {
		struct pollfd pfd[] = { {fd, POLLIN | POLLPRI, 0} };
		if (timer_timeout) {
			timer_timeout = 0;
			if (show_timeout || verbose > 1) {
				lockf(fileno(stdout), F_LOCK, 0);
				if (fd == conn_fd)
					fprintf(stdout, "++++++++++++++++++++|+++++++++++ TIMEOUT +++++++++++|  |                    [%d]\n", state);
				else if (fd == resp_fd)
					fprintf(stdout, "                    |+++++++++++ TIMEOUT +++++++++++|  |++++++++++++++++++++[%d]\n", state);
				else if (fd == list_fd)
					fprintf(stdout, "                    |+++++++++++ TIMEOUT +++++++++++|++|++++++++++++++++++++[%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
				show_timeout--;
			}
			last_event = __EVENT_TIMEOUT;
			return time_event(__EVENT_TIMEOUT);
		}
		print_command(fd, "poll()");
		pfd[0].fd = fd;
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			print_errno(fd, (last_errno = errno));
			if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
				break;
			return (__RESULT_FAILURE);
		case 0:
			print_success(fd);
			last_event = __EVENT_NO_MSG;
			return time_event(__EVENT_NO_MSG);
		case 1:
			print_success(fd);
			if (pfd[0].revents) {
				int ret;
				ctrl.len = -1;
				data.len = -1;
				flags = 0;
				for (;;) {
					print_command(fd, "getmsg()");
					if ((ret = getmsg(fd, &ctrl, &data, &flags)) >= 0)
						break;
					print_errno(fd, (last_errno = errno));
					if (last_errno == EINTR || last_errno == ERESTART)
						continue;
					if (last_errno == EAGAIN)
						break;
					return __RESULT_FAILURE;
				}
				if (ret < 0)
					break;
				if (ret == 0) {
					print_success(fd);
					if ((last_event = decode_msg(fd)) != __EVENT_NO_MSG)
						return (last_event);
				}
			}
		default:
			break;
		}
	}
	return __EVENT_UNKNOWN;
}

int get_event(int fd)
{
	return wait_event(fd, -1);
}

void test_sleep(int fd, unsigned long t)
{
	if (verbose > 1 && show) {
		lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "/ / / / / / / / / / | / / / Waiting %03lu seconds / / |  |                    [%d]\n", t, state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    | / / / Waiting %03lu seconds / / |  | / / / / / / / / / /[%d]\n", t, state);
		else if (fd == list_fd)
			fprintf(stdout, "                    | / / / Waiting %03lu seconds / / |/ | / / / / / / / / / /[%d]\n", t, state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	sleep(t);
}

int expect(int fd, int wait, int want)
{
	if ((last_event = wait_event(fd, wait)) == want)
		return (__RESULT_SUCCESS);
	if (verbose > 1 && show) {
		lockf(fileno(stdout), F_LOCK, 0);
		if (fd == conn_fd)
			fprintf(stdout, "- (%-15s) | - - - - - [Expected]- - - - - |  |                    [%d]\n", event_string(want), state);
		else if (fd == resp_fd)
			fprintf(stdout, "                    | - - - - - [Expected]- - - - - |  | (%-15s) -[%d]\n", event_string(want), state);
		else if (fd == list_fd)
			fprintf(stdout, "                    | - - - - - [Expected]- - - - - |- | (%-15s) -[%d]\n", event_string(want), state);
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	return (__RESULT_FAILURE);
}

int put_msg(int fd, int band, int flags, int wait)
{
	int ret;
	struct strbuf *myctrl = ctrl.len >= 0 ? &ctrl : NULL;
	struct strbuf *mydata = data.len >= 0 ? &data : NULL;
	decode_msg(fd);
	for (;;) {
		print_command(fd, "putpmsg()");
		if ((ret = putpmsg(fd, myctrl, mydata, band, flags)) >= 0)
			break;
		print_errno(fd, (last_errno = errno));
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		if (last_errno == EAGAIN)
			break;
		return (__RESULT_FAILURE);
	}
	print_success(fd);
	if (!ret)
		return (__RESULT_SUCCESS);
	if (!wait) {
		/* 
		 * printf("Nothing put on putpmsg\n"); 
		 */
		return (__RESULT_FAILURE);
	}
	for (;;) {
		int flag = band ? POLLWRNORM | POLLOUT : POLLWRBAND;
		struct pollfd pfd[] = {
			{fd, flag, 0}
		};
		if (!(ret = poll(pfd, 1, wait))) {
			/* 
			 * printf("Timeout on poll for putpmsg\n"); 
			 */
			return (__RESULT_FAILURE);
		}
		if (ret == 1 || ret == 2) {
			if (pfd[0].revents & (POLLOUT | POLLWRNORM | POLLWRBAND)) {
				if (putpmsg(fd, myctrl, mydata, band, flags) == 0)
					return (__RESULT_SUCCESS);
				printf("%d-ERROR: putpmsg: [%d] %s\n", fd, errno, strerror(errno));
				return (__RESULT_FAILURE);
			}
		}
		if (ret == -1) {
			printf("%d-ERROR: poll: [%d] %s\n", fd, errno, strerror(errno));
			return (__RESULT_FAILURE);
		}
	}
}

int put_fdi(int fd, int resfd, int offset, int flags)
{
	fdi.flags = flags;
	fdi.fildes = resfd;
	fdi.offset = offset;
	decode_msg(fd);
	for (;;) {
		print_command(fd, "ioctl()");
		if (ioctl(fd, I_FDINSERT, &fdi) >= 0) {
			print_success(fd);
			return (__RESULT_SUCCESS);
		}
		print_errno(fd, (last_errno = errno));
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
			continue;
		return (__RESULT_FAILURE);
	}
}

int sctp_t_open(const char *name, int *fdp)
{
	int fd;
	for (;;) {
		print_open(fdp);
		if ((fd = open(name, O_NONBLOCK | O_RDWR)) >= 0) {
			*fdp = fd;
			print_success(fd);
			return (__RESULT_SUCCESS);
		}
		print_errno(fd, (last_errno = errno));
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
			continue;
		return (__RESULT_FAILURE);
	}
}

int sctp_close(int *fdp)
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
		if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
			continue;
		return __RESULT_FAILURE;
	}
}

int sctp_info_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.info_req);
	cmd.tpi.type = T_INFO_REQ;
	return put_msg(fd, 0, MSG_HIPRI, 0);
}

int sctp_optmgmt_req(int fd, ulong flags)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.optmgmt_req) + sizeof(opt_optm);
	cmd.tpi.type = T_OPTMGMT_REQ;
	cmd.tpi.optmgmt_req.MGMT_flags = flags;
	cmd.tpi.optmgmt_req.OPT_length = sizeof(opt_optm);
	cmd.tpi.optmgmt_req.OPT_offset = sizeof(cmd.tpi.optmgmt_req);
	bcopy(&opt_optm, (cmd.cbuf + sizeof(cmd.tpi.optmgmt_req)), sizeof(opt_optm));
	return put_msg(fd, 0, MSG_BAND, 0);
}

int sctp_bind_req(int fd, void * add_ptr, size_t add_len, int coninds)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.bind_req) + add_len;
	cmd.tpi.type = T_BIND_REQ;
	cmd.tpi.bind_req.ADDR_length = add_len;
	cmd.tpi.bind_req.ADDR_offset = sizeof(cmd.tpi.bind_req);
	cmd.tpi.bind_req.CONIND_number = coninds;
	bcopy(add_ptr, (&cmd.tpi.bind_req) + 1, add_len);
	return put_msg(fd, 0, MSG_BAND, 0);
}

int sctp_unbind_req(int fd)
{
	data.len = 0;
	ctrl.len = sizeof(cmd.tpi.unbind_req);
	cmd.tpi.type = T_UNBIND_REQ;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int sctp_conn_req(int fd, void *add_ptr, size_t add_len, const char *dat)
{
	int ret;
	if (!dat)
		data.len = -1;
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.conn_req) + add_len + sizeof(opt_conn);
	cmd.tpi.type = T_CONN_REQ;
	cmd.tpi.conn_req.DEST_length = add_len;
	cmd.tpi.conn_req.DEST_offset = sizeof(cmd.tpi.conn_req);
	cmd.tpi.conn_req.OPT_length = sizeof(opt_conn);
	cmd.tpi.conn_req.OPT_offset = sizeof(cmd.tpi.conn_req) + add_len;
	bcopy(add_ptr, (cmd.cbuf + sizeof(cmd.tpi.conn_req)), add_len);
	bcopy(&opt_conn, (cmd.cbuf + sizeof(cmd.tpi.conn_req) + add_len), sizeof(opt_conn));
	ret = put_msg(fd, 0, MSG_BAND, 0);
	data.maxlen = BUFSIZE;
	data.len = -1;
	data.buf = dbuf;
	return (ret);
}

int sctp_conn_res(int fd, int respfd, const char *dat)
{
	int ret;
	if (!dat)
		fdi.databuf.len = -1;
	else {
		fdi.databuf.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	fdi.ctlbuf.len = sizeof(cmd.tpi.conn_res);
	cmd.tpi.type = T_CONN_RES;
	cmd.tpi.conn_res.ACCEPTOR_id = 0;
	cmd.tpi.conn_res.SEQ_number = seq[fd];
	cmd.tpi.conn_res.OPT_offset = 0;
	cmd.tpi.conn_res.OPT_length = 0;
	ret = put_fdi(fd, respfd, sizeof(cmd.tpi.type), 0);
	data.maxlen = BUFSIZE;
	data.len = -1;
	data.buf = dbuf;
	return (ret);
}

int sctp_discon_req(int fd, ulong seq)
{
	data.len = -1;
	ctrl.len = sizeof(cmd.tpi.discon_req);
	cmd.tpi.type = T_DISCON_REQ;
	cmd.tpi.discon_req.SEQ_number = seq;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int sctp_ordrel_req(int fd)
{
	data.len = -1;
	ctrl.len = sizeof(cmd.tpi.ordrel_req);
	cmd.tpi.type = T_ORDREL_REQ;
	return put_msg(fd, 0, MSG_BAND, 0);
}

int sctp_ndata_req(int fd, ulong flags, const char *dat, size_t len, int wait)
{
	int ret;
	if (!dat)
		return (__RESULT_FAILURE);
	else {
		data.maxlen = len;
		data.len = len;
		data.buf = (char *) dat;
	}
	ctrl.len = sizeof(cmd.tpi.data_req);
	cmd.tpi.type = T_DATA_REQ;
	cmd.tpi.data_req.MORE_flag = flags & T_MORE;
	ret = put_msg(fd, 0, MSG_BAND, wait);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int sctp_data_req(int fd, ulong flags, const char *dat, int wait)
{
	int ret;
	if (!dat)
		return (__RESULT_FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.data_req);
	cmd.tpi.type = T_DATA_REQ;
	cmd.tpi.data_req.MORE_flag = flags & T_MORE;
	ret = put_msg(fd, 0, MSG_BAND, wait);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int sctp_exdata_req(int fd, ulong flags, const char *dat)
{
	int ret;
	if (!dat)
		return (__RESULT_FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.exdata_req);
	cmd.tpi.type = T_EXDATA_REQ;
	cmd.tpi.exdata_req.MORE_flag = flags & T_MORE;
	ret = put_msg(fd, 1, MSG_BAND, 0);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int sctp_optdata_req(int fd, ulong flags, const char *dat, int wait)
{
	int ret;
	if (!dat)
		return (__RESULT_FAILURE);
	else {
		data.len = strlen(dat) + 1;
		strncpy(dbuf, dat, BUFSIZE);
	}
	ctrl.len = sizeof(cmd.tpi.optdata_req) + sizeof(opt_data);
	cmd.tpi.type = T_OPTDATA_REQ;
	cmd.tpi.optdata_req.DATA_flag = flags;
	cmd.tpi.optdata_req.OPT_length = sizeof(opt_data);
	cmd.tpi.optdata_req.OPT_offset = sizeof(cmd.tpi.optdata_req);
	bcopy(&opt_data, cmd.cbuf + sizeof(cmd.tpi.optdata_req), sizeof(opt_data));
	ret = put_msg(fd, flags & T_ODF_EX ? 1 : 0, MSG_BAND, wait);
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	return (ret);
}

int begin_tests(void)
{
#ifndef SCTP_VERSION_2
	addr1.port = htons(port1);
	inet_aton("127.0.0.3", &addr1.addr[0]);
	inet_aton("127.0.0.2", &addr1.addr[1]);
	inet_aton("127.0.0.1", &addr1.addr[2]);
	port1 += 3;
	addr2.port = htons(port2);
	inet_aton("127.0.0.3", &addr2.addr[0]);
	inet_aton("127.0.0.2", &addr2.addr[1]);
	inet_aton("127.0.0.1", &addr2.addr[2]);
	port2 += 3;
	addr3.port = htons(port3);
	inet_aton("127.0.0.3", &addr3.addr[0]);
	inet_aton("127.0.0.2", &addr3.addr[1]);
	inet_aton("127.0.0.1", &addr3.addr[2]);
	port3 += 3;
	addr4.port = htons(port4);
	inet_aton("127.0.0.3", &addr4.addr[0]);
	inet_aton("127.0.0.2", &addr4.addr[1]);
	inet_aton("127.0.0.1", &addr4.addr[2]);
	port4 += 3;
#else				/* SCTP_VERSION_2 */
	addr1[0].sin_family = addr1[1].sin_family = addr1[2].sin_family = AF_INET;
	addr1[0].sin_port = addr1[1].sin_port = addr1[2].sin_port = htons(port1);
	inet_aton("127.0.0.3", &addr1[0].sin_addr);
	inet_aton("127.0.0.2", &addr1[1].sin_addr);
	inet_aton("127.0.0.1", &addr1[2].sin_addr);
	port1 += 3;
	addr2[0].sin_family = addr2[1].sin_family = addr2[2].sin_family = AF_INET;
	addr2[0].sin_port = addr2[1].sin_port = addr2[2].sin_port = htons(port2);
	inet_aton("127.0.0.3", &addr2[0].sin_addr);
	inet_aton("127.0.0.2", &addr2[1].sin_addr);
	inet_aton("127.0.0.1", &addr2[2].sin_addr);
	port2 += 3;
	addr3[0].sin_family = addr3[1].sin_family = addr3[2].sin_family = AF_INET;
	addr3[0].sin_port = addr3[1].sin_port = addr3[2].sin_port = htons(port3);
	inet_aton("127.0.0.3", &addr3[0].sin_addr);
	inet_aton("127.0.0.2", &addr3[1].sin_addr);
	inet_aton("127.0.0.1", &addr3[2].sin_addr);
	port3 += 3;
	addr4[0].sin_family = addr4[1].sin_family = addr4[2].sin_family = AF_INET;
	addr4[0].sin_port = addr4[1].sin_port = addr4[2].sin_port = htons(port4);
	inet_aton("127.0.0.3", &addr4[0].sin_addr);
	inet_aton("127.0.0.2", &addr4[1].sin_addr);
	inet_aton("127.0.0.1", &addr4[2].sin_addr);
	port4 += 3;
#endif				/* SCTP_VERSION_2 */
	if (sctp_t_open(device, &conn_fd) != __RESULT_SUCCESS)
		goto failure;
	if (sctp_t_open(device, &resp_fd) != __RESULT_SUCCESS)
		goto failure;
	if (sctp_t_open(device, &list_fd) != __RESULT_SUCCESS)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	return __RESULT_FAILURE;
}

int end_tests(void)
{
	show_acks = 0;
	if (sctp_close(&conn_fd) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (sctp_close(&resp_fd) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	if (sctp_close(&list_fd) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return (__RESULT_SUCCESS);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postables
 *
 *  -------------------------------------------------------------------------
 */

int preamble_0(int fd)
{
	return (__RESULT_SUCCESS);
}

int postamble_0(int fd)
{
	return (__RESULT_SUCCESS);
}

int preamble_1(int fd, void * add_ptr, size_t add_len, int coninds)
{
	if (sctp_optmgmt_req(fd, T_NEGOTIATE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_bind_req(fd, add_ptr, add_len, coninds) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int preamble_1_conn(int fd)
{
	return preamble_1(fd, &addr1, sizeof(addr1), 0);
}
int preamble_1_list(int fd)
{
	return preamble_1(fd, &addr2, sizeof(addr2), 5);
}
int preamble_1_resp(int fd)
{
	return preamble_1(fd, &addr3, sizeof(addr3), 0);
}

int preamble_2_conn(int fd)
{
	if (preamble_1_conn(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int preamble_2_list(int fd)
{
	if (preamble_1_list(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_res(fd, resp_fd, NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int preamble_2_resp(int fd)
{
	if (preamble_1_resp(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int preamble_2b_conn(int fd)
{
	if (preamble_1_conn(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), "Hello World!") != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int preamble_2b_list(int fd)
{
	if (preamble_1_list(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_res(fd, resp_fd, "Hello There!") != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int preamble_2b_resp(int fd)
{
	if (preamble_1_resp(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int preamble_3b_conn(int fd)
{
	opt_optm.rcv_val = T_YES;
	opt_optm.ist_val = 32;
	opt_optm.ost_val = 32;
	opt_conn.ist_val = 32;
	opt_conn.ost_val = 32;
	return preamble_2_conn(fd);
}
int preamble_3b_resp(int fd)
{
	opt_optm.rcv_val = T_YES;
	opt_optm.ist_val = 32;
	opt_optm.ost_val = 32;
	opt_conn.ist_val = 32;
	opt_conn.ost_val = 32;
	return preamble_2_resp(fd);
}
int preamble_3b_list(int fd)
{
	opt_optm.rcv_val = T_YES;
	opt_optm.ist_val = 32;
	opt_optm.ost_val = 32;
	opt_conn.ist_val = 32;
	opt_conn.ost_val = 32;
	return preamble_2_list(fd);
}

int preamble_4_conn(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_DROPPING;
	return preamble_2_conn(fd);
}
int preamble_4_list(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_DROPPING;
	return preamble_2_list(fd);
}
int preamble_4_resp(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_DROPPING;
	return preamble_2_resp(fd);
}

int preamble_4b_conn(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_2_conn(fd);
}
int preamble_4b_list(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_2_list(fd);
}
int preamble_4b_resp(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_2_resp(fd);
}

int preamble_5_conn(int fd)
{
	// opt_optm.dbg_val = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	opt_optm.dbg_val = SCTP_OPTION_BREAK;
	return preamble_2_conn(fd);
}
int preamble_5_list(int fd)
{
	// opt_optm.dbg_val = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	opt_optm.dbg_val = SCTP_OPTION_BREAK;
	return preamble_2_list(fd);
}
int preamble_5_resp(int fd)
{
	// opt_optm.dbg_val = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	opt_optm.dbg_val = SCTP_OPTION_BREAK;
	return preamble_2_resp(fd);
}

int preamble_6_conn(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_3b_conn(fd);
}
int preamble_6_list(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_3b_list(fd);
}
int preamble_6_resp(int fd)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_3b_resp(fd);
}

int preamble_7_conn(int fd)
{
	opt_optm.mac_val = T_SCTP_HMAC_SHA1;
	return preamble_1_conn(fd);
}
int preamble_7_list(int fd)
{
	opt_optm.mac_val = T_SCTP_HMAC_SHA1;
	return preamble_1_list(fd);
}
int preamble_7_resp(int fd)
{
	opt_optm.mac_val = T_SCTP_HMAC_SHA1;
	return preamble_1_resp(fd);
}

int preamble_8_conn(int fd)
{
	opt_optm.mac_val = T_SCTP_HMAC_MD5;
	return preamble_1_conn(fd);
}
int preamble_8_list(int fd)
{
	opt_optm.mac_val = T_SCTP_HMAC_MD5;
	return preamble_1_list(fd);
}
int preamble_8_resp(int fd)
{
	opt_optm.mac_val = T_SCTP_HMAC_MD5;
	return preamble_1_resp(fd);
}

int postamble_1_conn(int fd)
{
	if (sctp_unbind_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int postamble_1_list(int fd)
{
	if (sctp_unbind_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int postamble_1_resp(int fd)
{
	if (sctp_unbind_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int postamble_2_conn(int fd)
{
	int failed = -1;
	if (sctp_discon_req(fd, 0) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
      unbind:
	if (postamble_1_conn(fd) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}
int postamble_2_list(int fd)
{
	int failed = -1;
	if (sctp_unbind_req(fd) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}
int postamble_2_resp(int fd)
{
	int failed = -1;
	if (expect(fd, LONG_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (sctp_discon_req(fd, 0) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (postamble_1_resp(fd) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}

int postamble_3_conn(int fd)
{
	int failed = -1;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1) {
		state += 10;
		if (postamble_2_conn(fd) != __RESULT_SUCCESS)
			failed = (failed == -1) ? state : failed;
	} else {
		if (postamble_1_conn(fd) != __RESULT_SUCCESS)
			failed = (failed == -1) ? state : failed;
	}
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}
int postamble_3_list(int fd)
{
	return postamble_2_list(fd);
}
int postamble_3_resp(int fd)
{
	int failed = -1;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1) {
		state += 10;
		if (postamble_2_resp(fd) != __RESULT_SUCCESS)
			failed = (failed == -1) ? state : failed;
	} else {
		if (postamble_1_resp(fd) != __RESULT_SUCCESS)
			failed = (failed == -1) ? state : failed;
	}
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}

/* 
 *  Do options management.
 */
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Do options management."
#define desc_case_1_1 "\
Checks that options management can be performed on several streams."
int test_case_1_1(int fd)
{
	if (sctp_optmgmt_req(fd, T_NEGOTIATE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_info_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_optmgmt_req(fd, T_CURRENT) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_1_conn	test_case_1_1
#define test_case_1_1_resp	test_case_1_1
#define test_case_1_1_list	test_case_1_1

#define preamble_1_1_conn	preamble_0
#define preamble_1_1_resp	preamble_0
#define preamble_1_1_list	preamble_0

#define postamble_1_1_conn	postamble_0
#define postamble_1_1_resp	postamble_0
#define postamble_1_1_list	postamble_0

/* 
 *  Bind and unbind three streams.
 */
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Bind and Unbind three streams."
#define desc_case_1_2 "\
Checks that three streams can be bound and unbound with\n\
one stream as listener."
int test_case_1_2(int fd, void * add_ptr, size_t add_len, int coninds)
{
	if (sctp_bind_req(fd, add_ptr, add_len, coninds) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_unbind_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_1_2_conn(int fd)
{
	return test_case_1_2(fd, &addr1, sizeof(addr1), 0);
}
int test_case_1_2_list(int fd)
{
	return test_case_1_2(fd, &addr2, sizeof(addr2), 5);
}
int test_case_1_2_resp(int fd)
{
	return test_case_1_2(fd, &addr3, sizeof(addr3), 0);
}

#define preamble_1_2_conn	preamble_0
#define preamble_1_2_resp	preamble_0
#define preamble_1_2_list	preamble_0

#define postamble_1_2_conn	postamble_0
#define postamble_1_2_resp	postamble_0
#define postamble_1_2_list	postamble_0

/* 
 *  Attempt a connection with no listener.
 */
#define numb_case_2_1 "2.1"
#define name_case_2_1 "Attempt a connection with no listener."
#define desc_case_2_1 "\
Attempts a connection with no listener.  The connection attempt\n\
should time out."
int test_case_2_1_conn(int fd)
{
	int i;
	if (sctp_optmgmt_req(fd, T_NEGOTIATE) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_bind_req(fd, &addr1, sizeof(addr1), 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, (opt_optm.rmx_val + 10) * opt_optm.irt_val, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_unbind_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_2_1_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_2_1_resp(int fd)
{
	return (__RESULT_SUCCESS);
}

#define preamble_2_1_conn	preamble_0
#define preamble_2_1_resp	preamble_0
#define preamble_2_1_list	preamble_0

#define postamble_2_1_conn	postamble_0
#define postamble_2_1_resp	postamble_0
#define postamble_2_1_list	postamble_0

/* 
 *  Attempt and withdraw a connection request.
 */
#define numb_case_2_2 "2.2"
#define name_case_2_2 "Attempt and withdraw a connection request."
#define desc_case_2_2 "\
Attempts and then withdraws a connection request.  The connection\n\
should disconnect at both ends."
int test_case_2_2_conn(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_discon_req(fd, 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_2_2_list(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONGER_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_2_2_resp(int fd)
{
	return (__RESULT_SUCCESS);
}

#define preamble_2_2_conn	preamble_1_conn
#define preamble_2_2_resp	preamble_1_resp
#define preamble_2_2_list	preamble_1_list

#define postamble_2_2_conn	postamble_1_conn
#define postamble_2_2_resp	postamble_1_resp
#define postamble_2_2_list	postamble_1_list

/* 
 *  Attempt and refuse a connection request.
 */
#define numb_case_3_1 "3.1"
#define name_case_3_1 "Attempt and refuse a connection request."
#define desc_case_3_1 "\
Attempts a connection which is refused by the receiving end.\n\
The connection should disconnect at the attempting end."
int test_case_3_1_conn(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_3_1_list(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_discon_req(fd, seq[fd]) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_3_1_resp(int fd)
{
	return (__RESULT_SUCCESS);
}

#define preamble_3_1_conn	preamble_1_conn
#define preamble_3_1_resp	preamble_1_resp
#define preamble_3_1_list	preamble_1_list

#define postamble_3_1_conn	postamble_1_conn
#define postamble_3_1_resp	postamble_1_resp
#define postamble_3_1_list	postamble_1_list

/* 
 *  Attempt and delayed refuse a connection request.
 */
#define numb_case_3_2 "3.2"
#define name_case_3_2 "Attempt and delayed refuse a connection request."
#define desc_case_3_2 "\
Attempts a delayed refusal of a connection requrest.  This delayed\n\
refusal should come after the connector has already timed out."
int test_case_3_2_conn(int fd)
{
	int i;
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, (opt_optm.rmx_val + 10) * opt_optm.irt_val, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_3_2_list(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_discon_req(fd, seq[fd]) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_3_2_resp(int fd)
{
	return (__RESULT_SUCCESS);
}

#define preamble_3_2_conn	preamble_1_conn
#define preamble_3_2_resp	preamble_1_resp
#define preamble_3_2_list	preamble_1_list

#define postamble_3_2_conn	postamble_1_conn
#define postamble_3_2_resp	postamble_1_resp
#define postamble_3_2_list	postamble_1_list

/* 
 *  Accept a connection.
 */
#define numb_case_4_1 "4.1"
#define name_case_4_1 "Accept a connection."
#define desc_case_4_1 "\
Accept a connection and then disconnect.  This connection attempt\n\
should be successful."
int test_case_4_1_conn(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONGER_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_4_1_list(int fd)
{
	if (expect(fd, LONG_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_res(fd, resp_fd, NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_4_1_resp(int fd)
{
	if (expect(fd, LONG_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_discon_req(fd, 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_4_1_conn	preamble_1_conn
#define preamble_4_1_resp	preamble_1_resp
#define preamble_4_1_list	preamble_1_list

#define postamble_4_1_conn	postamble_1_conn
#define postamble_4_1_resp	postamble_1_resp
#define postamble_4_1_list	postamble_1_list

/* 
 *  Attempt and delayed accept a connection request.
 */
#define numb_case_4_2 "4.2"
#define name_case_4_2 "Attempt and delayed accept a connection request."
#define desc_case_4_2 "\
Test Case 4(b):\n\
Attempt a connection and delay the acceptance of the connection request.\n\
This should result in a disconnection indication after the connection is\n\
accepted.  "
int test_case_4_2_conn(int fd)
{
	int i;
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, (opt_optm.rmx_val + 10) * opt_optm.irt_val, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_4_2_list(int fd)
{
	if (expect(fd, LONG_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, (opt_optm.rmx_val + 10) * opt_optm.irt_val + NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_res(fd, resp_fd, NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_4_2_resp(int fd)
{
	switch (wait_event(fd, (opt_optm.rmx_val + 10) * opt_optm.irt_val + LONG_WAIT)) {
	case __EVENT_DISCON_IND:
		goto success;
	case __EVENT_NO_MSG:
		break;
	default:
		goto failure;
	}
	state++;
	if (sctp_data_req(fd, 0, "Hello!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
      success:
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_4_2_conn	preamble_1_conn
#define preamble_4_2_resp	preamble_1_resp
#define preamble_4_2_list	preamble_1_list

#define postamble_4_2_conn	postamble_1_conn
#define postamble_4_2_resp	postamble_1_resp
#define postamble_4_2_list	postamble_1_list

/* 
 *  Accept a connection.
 */
#define numb_case_5_1 "5.1"
#define name_case_5_1 "Accept a connection."
#define desc_case_5_1 "\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the T_SCTP_HMAC_NONE signature on its cookie."
int test_case_5_1_conn(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_discon_req(fd, 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_5_1_list(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_res(fd, resp_fd, NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_5_1_resp(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_5_1_conn	preamble_1_conn
#define preamble_5_1_resp	preamble_1_resp
#define preamble_5_1_list	preamble_1_list

#define postamble_5_1_conn	postamble_1_conn
#define postamble_5_1_resp	postamble_1_resp
#define postamble_5_1_list	postamble_1_list

/* 
 *  Accept a connection (MD5 hashed cookie)
 */
#define numb_case_5_2 "5.2"
#define name_case_5_2 "Accept a connection (MD5 hashed cookie)."
#define desc_case_5_2 "\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the T_SCTP_HMAC_MD5 signature on its cookie."
int test_case_5_2_conn(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_discon_req(fd, 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_5_2_list(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_res(fd, resp_fd, NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_5_2_resp(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_5_2_conn	preamble_8_conn
#define preamble_5_2_resp	preamble_8_resp
#define preamble_5_2_list	preamble_8_list

#define postamble_5_2_conn	postamble_1_conn
#define postamble_5_2_resp	postamble_1_resp
#define postamble_5_2_list	postamble_1_list

/* 
 *  Accept a connection (SHA1 hashed cookie)
 */
#define numb_case_5_3 "5.3"
#define name_case_5_3 "Accept a connection (SHA1 hashed cookie)"
#define desc_case_5_3 "\
Attempt and accept a connection.  This should be successful.  The\n\
accepting stream uses the T_SCTP_HMAC_SHA1 signature on its cookie."
int test_case_5_3_conn(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_req(fd, &addr2, sizeof(addr2), NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_discon_req(fd, 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_5_3_list(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_conn_res(fd, resp_fd, NULL) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, SHORT_WAIT, __EVENT_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_5_3_resp(int fd)
{
	if (expect(fd, LONGER_WAIT, __EVENT_DISCON_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_5_3_conn	preamble_7_conn
#define preamble_5_3_resp	preamble_7_resp
#define preamble_5_3_list	preamble_7_list

#define postamble_5_3_conn	postamble_1_conn
#define postamble_5_3_resp	postamble_1_resp
#define postamble_5_3_list	postamble_1_list

/* 
 *  Connect with data.
 */
#define numb_case_6_1 "6.1"
#define name_case_6_1 "Connect with data."
#define desc_case_6_1 "\
Attempt and accept a connection where data is also passed in the\n\
connection request and the connection response.  This should result\n\
in DATA chunks being bundled with the COOKIE-ECHO and COOKIE-ACK\n\
chunks in the SCTP messages."
int test_case_6_1_conn(int fd)
{
	int i;
	if (sctp_data_req(fd, 0, "Hellos Again!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hellos Again!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hellos Again!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 21; i++)
		if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_6_1_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_6_1_resp(int fd)
{
	int i;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 21; i++)
		if (sctp_data_req(fd, 0, "Hello Too!", 0) != __RESULT_SUCCESS)
			goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_6_1_conn	preamble_2b_conn
#define preamble_6_1_resp	preamble_2b_resp
#define preamble_6_1_list	preamble_2b_list

#define postamble_6_1_conn	postamble_3_conn
#define postamble_6_1_resp	postamble_3_resp
#define postamble_6_1_list	postamble_3_list

/* 
 *  Connect and send partial data.
 */
#define numb_case_6_2 "6.2"
#define name_case_6_2 "Connect and send partial data."
#define desc_case_6_2 "\
Connect and send partial data (i.e., data with more flag set)."
int test_case_6_2_conn(int fd)
{
	if (sctp_data_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_6_2_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_6_2_resp(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_6_2_conn	preamble_2_conn
#define preamble_6_2_resp	preamble_2_resp
#define preamble_6_2_list	preamble_2_list

#define postamble_6_2_conn	postamble_3_conn
#define postamble_6_2_resp	postamble_3_resp
#define postamble_6_2_list	postamble_3_list

/* 
 *  Connect and send partial data.
 */
#define numb_case_6_3 "6.3"
#define name_case_6_3 "Connect and send partial data."
#define desc_case_6_3 "\
Connect and send partial data and expedited data on multiple streams.\n\
Expedited data should be delivered between ordered data fragments on\n\
the same stream and delivered to the user first."
int test_case_6_3_conn(int fd)
{
	opt_data.sid_val = 0;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 1;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 2;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 3;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 0;
	if (sctp_exdata_req(fd, 0, "Hello There.") != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 0;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 1;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 2;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 3;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 0;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 1;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 2;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 3;
	if (sctp_optdata_req(fd, T_MORE, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 0;
	if (sctp_optdata_req(fd, 0, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 1;
	if (sctp_optdata_req(fd, 0, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 2;
	if (sctp_optdata_req(fd, 0, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 3;
	if (sctp_optdata_req(fd, 0, "Hello There.", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_6_3_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_6_3_resp(int fd)
{
	int i;
	if (expect(fd, NORMAL_WAIT, __EVENT_OPTDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (!(cmd.tpi.optdata_ind.DATA_flag & T_ODF_EX))
		goto failure;
	state++;
	for (i = 0; i < 16; i++) {
		if (expect(fd, NORMAL_WAIT, __EVENT_OPTDATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_6_3_conn	preamble_3b_conn
#define preamble_6_3_resp	preamble_3b_resp
#define preamble_6_3_list	preamble_3b_list

#define postamble_6_3_conn	postamble_3_conn
#define postamble_6_3_resp	postamble_3_resp
#define postamble_6_3_list	postamble_3_list

/* 
 *  Test fragmentation by sending very large packets.
 */
#define numb_case_7_1 "7.1"
#define name_case_7_1 "Test fragmentation by sending very large packets."
#define desc_case_7_1 "\
Connect and send very large packets to test fragmentation."
int test_case_7_1_conn(int fd)
{
	unsigned char lbuf[100000];
	const char nrm[] = "Hello.";
	const char urg[] = "Urgent.";
	int i;
	bzero(lbuf, sizeof(lbuf));
	for (i = 0; i < 4; i++) {
		if (sctp_ndata_req(fd, 0, lbuf, sizeof(lbuf), 0) != __RESULT_SUCCESS) {
			state++;
			test_sleep(fd, 1);
			if (sctp_ndata_req(fd, 0, lbuf, sizeof(lbuf), 0) != __RESULT_SUCCESS)
				goto failure;
		}
		state++;
		if (sctp_exdata_req(fd, 0, urg) != __RESULT_SUCCESS) {
			state++;
			test_sleep(fd, 1);
			if (sctp_exdata_req(fd, 0, urg) != __RESULT_SUCCESS)
				goto failure;
		}
		state++;
		if (sctp_data_req(fd, 0, nrm, 0) != __RESULT_SUCCESS) {
			state++;
			test_sleep(fd, 1);
			if (sctp_data_req(fd, 0, nrm, 0) != __RESULT_SUCCESS)
				goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_7_1_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_7_1_resp(int fd)
{
	size_t len = 0;
	while (len < 4 * 100000 + 4 * 8 + 4 * 7) {
		switch (wait_event(fd, LONGER_WAIT)) {
		case __EVENT_EXDATA_IND:
		case __EVENT_DATA_IND:
			len += data.len;
			break;
		default:
			goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "i = %d, j = %d, k = %d\n", 4, 4, 4);
	fprintf(stdout, "Received %u bytes, expecting %u\n", len, 4 * 100000 + 4 * 8 + 4 * 7);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#define preamble_7_1_conn	preamble_2_conn
#define preamble_7_1_resp	preamble_2_resp
#define preamble_7_1_list	preamble_2_list

#define postamble_7_1_conn	postamble_3_conn
#define postamble_7_1_resp	postamble_3_resp
#define postamble_7_1_list	postamble_3_list

/* 
 *  Test coallescing packets by sending many small fragmented pieces.
 */
#define numb_case_7_2 "7.2"
#define name_case_7_2 "Test coallescing by sending small fragmented pieces."
#define desc_case_7_2 "\
Connect and send many small packets to test coallescing of packets."
int test_case_7_2_conn(int fd)
{
	int s = 0;
	size_t snd_bytes = 0;
	while (s < 100000) {
		if (sctp_data_req(fd, T_MORE, "Hi There.", 0) != __RESULT_SUCCESS) {
			state++;
			print_more();
			test_sleep(fd, 1);
			if (sctp_data_req(fd, T_MORE, "Hi There.", 0) != __RESULT_SUCCESS)
				goto failure;
		}
		snd_bytes += 10;
		s++;
		state++;
		if (s > 3)
			print_less(fd);
	}
	state++;
	if (sctp_data_req(fd, 0, "Hi There.", 0) != __RESULT_SUCCESS)
		goto failure;
	snd_bytes += 10;
	s++;
	state++;
	print_more();
	state++;
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "Sent %3d messages making %6u bytes.\n", s, snd_bytes);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	print_more();
	return (__RESULT_FAILURE);
}
int test_case_7_2_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_7_2_resp(int fd)
{
	int r = 0;
	size_t rcv_bytes = 0;
	int joined = 0;
	while (r < 4) {
		if (expect(fd, LONGER_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
		if (data.len > 10)
			joined = 1;
		rcv_bytes += data.len;
		r++;
		state++;
	}
	print_less(fd);
	while (rcv_bytes < 1000010) {
		if (expect(fd, LONGER_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
		if (data.len > 10)
			joined = 1;
		rcv_bytes += data.len;
		r++;
		state++;
	}
	if (!joined)
		goto failure;
	state++;
	print_more();
	state++;
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "Rcvd %3d messages making %6u bytes.\n", r, rcv_bytes);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	print_more();
	return (__RESULT_FAILURE);
}

#define preamble_7_2_conn	preamble_2_conn
#define preamble_7_2_resp	preamble_2_resp
#define preamble_7_2_list	preamble_2_list

#define postamble_7_2_conn	postamble_3_conn
#define postamble_7_2_resp	postamble_3_resp
#define postamble_7_2_list	postamble_3_list

/* 
 *  Connect with transfer data and orderly release.
 */
#define numb_case_8_1 "8.1"
#define name_case_8_1 "Connect with transfer data and orderly release."
#define desc_case_8_1 "\
Connect, transfer data and perform orderly release."
int test_case_8_1_conn(int fd)
{
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_8_1_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_8_1_resp(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_8_1_conn	preamble_2_conn
#define preamble_8_1_resp	preamble_2_resp
#define preamble_8_1_list	preamble_2_list

#define postamble_8_1_conn	postamble_1_conn
#define postamble_8_1_resp	postamble_1_resp
#define postamble_8_1_list	postamble_1_list

/* 
 *  Connect with orderly release and late data transfer.
 */
#define numb_case_8_2 "8.2"
#define name_case_8_2 "Connect with orderly release and late data transfer."
#define desc_case_8_2 "\
Connect, transfer data and perform orderly release but transfer\n\
data after release has been initiated"
int test_case_8_2_conn(int fd)
{
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_8_2_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_8_2_resp(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_8_2_conn	preamble_2_conn
#define preamble_8_2_resp	preamble_2_resp
#define preamble_8_2_list	preamble_2_list

#define postamble_8_2_conn	postamble_1_conn
#define postamble_8_2_resp	postamble_1_resp
#define postamble_8_2_list	postamble_1_list

/* 
 *  Connect with attempted simultaneous orderly release.
 */
#define numb_case_8_3 "8.3"
#define name_case_8_3 "Connect with attempted simultaneous orderly release."
#define desc_case_8_3 "\
Connect, transfer data and perform orderly release but attempt\n\
to perform a simultaneous release from both sides.  (This might\n\
or might not result in a simultaneous release attempt.)"
int test_case_8_3_conn(int fd)
{
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_8_3_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_8_3_resp(int fd)
{
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_8_3_conn	preamble_2_conn
#define preamble_8_3_resp	preamble_2_resp
#define preamble_8_3_list	preamble_2_list

#define postamble_8_3_conn	postamble_1_conn
#define postamble_8_3_resp	postamble_1_resp
#define postamble_8_3_list	postamble_1_list

/* 
 *  Connect with orderly release under noise.
 */
#define numb_case_8_4 "8.4"
#define name_case_8_4 "Connect with orderly release under noise."
#define desc_case_8_4 "\
Connect, transfer data and perform orderly release under noise."
int test_case_8_4_conn(int fd)
{
	int i;
	if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 20; i++)
		if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
			goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 20; i++) {
		if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
	}
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_8_4_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_8_4_resp(int fd)
{
	int i;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 20; i++)
		if (sctp_data_req(fd, 0, "Hello World!", 0) != __RESULT_SUCCESS)
			goto failure;
	state++;
	if (sctp_ordrel_req(fd) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (i = 0; i < 20; i++) {
		if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
	}
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_ORDREL_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_8_4_conn	preamble_4b_conn
#define preamble_8_4_resp	preamble_4b_resp
#define preamble_8_4_list	preamble_4b_list

#define postamble_8_4_conn	postamble_1_conn
#define postamble_8_4_resp	postamble_1_resp
#define postamble_8_4_list	postamble_1_list

/* 
 *  Delivering ordered data under noise.
 */
#define numb_case_9_1 "9.1"
#define name_case_9_1 "Delivering ordered data under noise."
#define desc_case_9_1 "\
Delivery of ordered data under noise with acknowledgement."
#define TEST_PACKETS 300
int test_case_9_1_conn(int fd)
{
	int i = 0, j = 0;
	if (sctp_data_req(fd, 0, "Pattern-1", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(fd, 1);
	state++;
	while (i < TEST_PACKETS) {
		if (sctp_data_req(fd, 0, "Pattern-1", 0) != __RESULT_SUCCESS)
			goto failure;
		i++;
		state++;
	}
	while (j < TEST_PACKETS) {
		switch (wait_event(fd, NORMAL_WAIT)) {
		case __EVENT_DATA_IND:
			j++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d\n", fd, i, j);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}
int test_case_9_1_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_9_1_resp(int fd)
{
	int i = 0, j = 0;
	if (expect(fd, NORMAL_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(fd, 1);
	state++;
	while (i < TEST_PACKETS) {
		if (sctp_data_req(fd, 0, "Pattern-3", 0) != __RESULT_SUCCESS)
			goto failure;
		i++;
		state++;
	}
	while (j < TEST_PACKETS) {
		switch (wait_event(fd, NORMAL_WAIT)) {
		case __EVENT_DATA_IND:
			j++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d\n", fd, i, j);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#define preamble_9_1_conn	preamble_4_conn
#define preamble_9_1_resp	preamble_4_resp
#define preamble_9_1_list	preamble_4_list

#define postamble_9_1_conn	postamble_3_conn
#define postamble_9_1_resp	postamble_3_resp
#define postamble_9_1_list	postamble_3_list

/* 
 *  Delivering out-of-order data under noise.
 */
#define numb_case_9_2 "9.2"
#define name_case_9_2 "Delivering out-of-order data under noise."
#define desc_case_9_2 "\
Delivery of un-ordered data under noise."
int test_case_9_2_conn(int fd)
{
	int i = 0, j = 0;
	if (sctp_exdata_req(fd, 0, "Pattern-1") != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	while (i < TEST_PACKETS) {
		if (sctp_exdata_req(fd, 0, "Pattern-1") != __RESULT_SUCCESS)
			goto failure;
		i++;
		state++;
	}
	while (j < TEST_PACKETS) {
		switch (wait_event(fd, NORMAL_WAIT)) {
		case __EVENT_EXDATA_IND:
			j++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d\n", fd, i, j);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}
int test_case_9_2_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_9_2_resp(int fd)
{
	int i = 0, j = 0;
	if (expect(fd, LONG_WAIT, __EVENT_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_exdata_req(fd, 0, "Pattern-3") != __RESULT_SUCCESS)
		goto failure;
	state++;
	while (i < TEST_PACKETS) {
		if (sctp_exdata_req(fd, 0, "Pattern-3") != __RESULT_SUCCESS)
			goto failure;
		i++;
		state++;
	}
	while (j < TEST_PACKETS) {
		switch (wait_event(fd, NORMAL_WAIT)) {
		case __EVENT_EXDATA_IND:
			j++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "%d sent %d inds %d\n", fd, i, j);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#define preamble_9_2_conn	preamble_4_conn
#define preamble_9_2_resp	preamble_4_resp
#define preamble_9_2_list	preamble_4_list

#define postamble_9_2_conn	postamble_3_conn
#define postamble_9_2_resp	postamble_3_resp
#define postamble_9_2_list	postamble_3_list

#undef TEST_PACKETS

#define TEST_PACKETS  5
#define TEST_STREAMS 32
#define TEST_TOTAL (TEST_PACKETS*TEST_STREAMS)
/* 
 *  Delivering ordered data in multiple streams under noise.
 */
#define numb_case_9_3 "9.3"
#define name_case_9_3 "Delivering ordered data in multiple streams under noise."
#define desc_case_9_3 "\
Delivery of ordered data in multiple streams under noise."
int test_case_9_3_conn(int fd)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int I = 0, J = 0;
	if (sctp_optdata_req(fd, 0, "Pattern-1", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_OPTDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			opt_data.sid_val = s;
			if (sctp_optdata_req(fd, 0, "Pattern-1", 0) != __RESULT_SUCCESS)
				goto failure;
			i[s]++;
			I++;
			state++;
		}
	}
	while (J < TEST_TOTAL) {
		switch (wait_event(fd, NORMAL_WAIT)) {
		case __EVENT_OPTDATA_IND:
			j[sid[fd]]++;
			J++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	for (s = 0; s < TEST_STREAMS; s++) {
		if (j[s] != TEST_PACKETS)
			goto failure;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", fd, i[s], j[s]);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}
int test_case_9_3_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_9_3_resp(int fd)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int I = 0, J = 0;
	if (expect(fd, LONG_WAIT, __EVENT_OPTDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_optdata_req(fd, 0, "Pattern-3", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			opt_data.sid_val = s;
			if (sctp_optdata_req(fd, 0, "Pattern-3", 0) != __RESULT_SUCCESS)
				goto failure;
			i[s]++;
			I++;
			state++;
		}
	}
	while (J < TEST_TOTAL) {
		switch (wait_event(fd, NORMAL_WAIT)) {
		case __EVENT_OPTDATA_IND:
			j[sid[fd]]++;
			J++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	for (s = 0; s < TEST_STREAMS; s++) {
		if (j[s] != TEST_PACKETS)
			goto failure;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", fd, i[s], j[s]);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#define preamble_9_3_conn	preamble_6_conn
#define preamble_9_3_resp	preamble_6_resp
#define preamble_9_3_list	preamble_6_list

#define postamble_9_3_conn	postamble_3_conn
#define postamble_9_3_resp	postamble_3_resp
#define postamble_9_3_list	postamble_3_list

/* 
 *  Delivering ordered and unordered data in multiple streams under noise.
 */
#define numb_case_9_4 "9.4"
#define name_case_9_4 "Delivering data in multiple streams under noise."
#define desc_case_9_4 "\
Delivery of ordered and un-ordered data in multiple streams under noise."
int test_case_9_4_conn(int fd)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int o[TEST_STREAMS] = { 0, };
	int p[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, O = 0, P = 0;
	opt_data.sid_val = 1;
	if (sctp_optdata_req(fd, T_ODF_EX, "Pattern-1", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONG_WAIT, __EVENT_OPTDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			opt_data.sid_val = s;
			if (sctp_optdata_req(fd, 0, "Pattern-1", 0) != __RESULT_SUCCESS)
				continue;
			i[s]++;
			I++;
			state++;
		}
		while (O < TEST_TOTAL && o[s] < TEST_PACKETS) {
			opt_data.sid_val = s;
			if (sctp_optdata_req(fd, T_ODF_EX, "Pattern-1", 0) != __RESULT_SUCCESS)
				continue;
			o[s]++;
			O++;
			state++;
		}
	}
	while (J < TEST_TOTAL || P < TEST_TOTAL) {
		switch (wait_event(fd, LONGER_WAIT)) {
		case __EVENT_OPTDATA_IND:
			if (!exp[fd]) {
				j[sid[fd]]++;
				J++;
			} else {
				p[sid[fd]]++;
				P++;
			}
			break;
		case __EVENT_DATA_IND:
			j[sid[fd]]++;
			J++;
			break;
		case __EVENT_EXDATA_IND:
			p[sid[fd]]++;
			P++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", fd, i[s], j[s]);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", fd, o[s], p[s]);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}
int test_case_9_4_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_9_4_resp(int fd)
{
	int s;
	int i[TEST_STREAMS] = { 0, };
	int j[TEST_STREAMS] = { 0, };
	int o[TEST_STREAMS] = { 0, };
	int p[TEST_STREAMS] = { 0, };
	int I = 0, J = 0, O = 0, P = 0;
	if (expect(fd, LONG_WAIT, __EVENT_OPTDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	opt_data.sid_val = 3;
	if (sctp_optdata_req(fd, T_ODF_EX, "Pattern-3", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (s = 0; s < TEST_STREAMS; s++) {
		while (I < TEST_TOTAL && i[s] < TEST_PACKETS) {
			opt_data.sid_val = s;
			if (sctp_optdata_req(fd, 0, "Pattern-3", 0) != __RESULT_SUCCESS)
				continue;
			i[s]++;
			I++;
			state++;
		}
		while (O < TEST_TOTAL && o[s] < TEST_PACKETS) {
			opt_data.sid_val = s;
			if (sctp_optdata_req(fd, T_ODF_EX, "Pattern-3", 0) != __RESULT_SUCCESS)
				continue;
			o[s]++;
			O++;
			state++;
		}
	}
	while (J < TEST_TOTAL || P < TEST_TOTAL) {
		switch (wait_event(fd, LONGER_WAIT)) {
		case __EVENT_OPTDATA_IND:
			if (!exp[fd]) {
				j[sid[fd]]++;
				J++;
			} else {
				p[sid[fd]]++;
				P++;
			}
			break;
		case __EVENT_DATA_IND:
			j[sid[fd]]++;
			J++;
			break;
		case __EVENT_EXDATA_IND:
			p[sid[fd]]++;
			P++;
			break;
		case __EVENT_NO_MSG:
			continue;
		default:
			goto failure;
		}
		state++;
	}
	for (s = 0; s < TEST_STREAMS; s++) {
		if (j[s] != TEST_PACKETS || p[s] != TEST_PACKETS)
			goto failure;
	}
	return (__RESULT_SUCCESS);
      failure:
	lockf(fileno(stdout), F_LOCK, 0);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", fd, i[s], j[s]);
	for (s = 0; s < TEST_STREAMS; s++)
		fprintf(stdout, "%d send %d inds %d\n", fd, o[s], p[s]);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	return (__RESULT_FAILURE);
}

#define preamble_9_4_conn	preamble_6_conn
#define preamble_9_4_resp	preamble_6_resp
#define preamble_9_4_list	preamble_6_list
#define postamble_9_4_conn	postamble_3_conn
#define postamble_9_4_resp	postamble_3_resp
#define postamble_9_4_list	postamble_3_list

/* 
 *  Data for destination failure testing.
 */
#define numb_case_10_1 "10.1"
#define name_case_10_1 "Data for destination failure testing."
#define desc_case_10_1 "\
Delivery of ordered data with destination failure."
int test_case_10_1_conn(int fd)
{
	int i, j;
	if (sctp_data_req(fd, 0, "Test Pattern-1", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(fd, LONGER_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20;) {
			if (sctp_data_req(fd, 0, "Test Pattern-1", 0) != __RESULT_SUCCESS)
				continue;
			i++;
			state++;
		}
		for (i = 0; i < 20;) {
			switch (wait_event(fd, LONGER_WAIT)) {
				case __EVENT_DATA_IND:
					i++;
					break;
				case __EVENT_NO_MSG:
					continue;
				default:
					goto failure;
			}
			state++;
		}
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}
int test_case_10_1_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_10_1_resp(int fd)
{
	int i, j;
	if (expect(fd, LONGER_WAIT, __EVENT_DATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (sctp_data_req(fd, 0, "Test Pattern-3", 0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (j = 0; j < 20; j++) {
		for (i = 0; i < 20;) {
			if (sctp_data_req(fd, 0, "Test Pattern-3", 0) != __RESULT_SUCCESS)
				continue;
			i++;
			state++;
		}
		for (i = 0; i < 20;) {
			switch (wait_event(fd, LONGER_WAIT)) {
				case __EVENT_DATA_IND:
					i++;
					break;
				case __EVENT_NO_MSG:
					continue;
				default:
					goto failure;
			}
			state++;
		}
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define preamble_10_1_conn	preamble_5_conn
#define preamble_10_1_resp	preamble_5_resp
#define preamble_10_1_list	preamble_5_list

#define postamble_10_1_conn	postamble_3_conn
#define postamble_10_1_resp	postamble_3_resp
#define postamble_10_1_list	postamble_3_list

/* 
 *  Data for destination failure testing.
 */
long time_sub(struct timeval *t1, struct timeval *t2)
{
	return ((t1->tv_sec - t2->tv_sec) * 1000000 + (t1->tv_usec - t2->tv_usec));
}

#define numb_case_10_2 "10.2"
#define name_case_10_2 "Data for destination failure testing."
#define desc_case_10_2 "\
Delivery of ordered data with destination failure."
int test_case_10_2_conn(int fd)
{
	int i, j, n = 0;
	struct result {
		uint req_idx;
		struct timeval req;
	};
#define SETS 1000
#define REPS 1
	struct result times[SETS * REPS];
	bzero(times, sizeof(times));
	show = 0;
	for (j = 0; j < SETS;) {
		for (i = 0; i < REPS; i++, state++) {
			if (sctp_data_req(fd, 0, "Test Pattern-1", 0) != __RESULT_SUCCESS)
				continue;
			times[j * REPS + i].req = when;
			times[j * REPS + i].req_idx = n++;
			j++;
			state++;
		}
	}
	show = 1;
	lockf(fileno(stdout), F_LOCK, 0);
	for (j = 0, n = 0; n < 3 * SETS * REPS; n++) {
		for (i = 0; i < SETS * REPS; i++) {
			if (times[i].req_idx == n) {
				fprintf(stdout, "T_DATA_REQ    ----->| - - - - - - ->\\               |  |                    \n");
				fprintf(stdout, "                    |               \\ - - - - - - - +->|---> T_DATA_IND     \n");
				break;
			}
		}
	}
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
	show = 1;
	return (__RESULT_SUCCESS);
      failure:
	show = 1;
	return (__RESULT_FAILURE);
}
int test_case_10_2_list(int fd)
{
	return (__RESULT_SUCCESS);
}
int test_case_10_2_resp(int fd)
{
	int i, j;
	show = 0;
	for (j = 0; j < SETS; j++)
		for (i = 0; i < REPS;) {
			switch (wait_event(fd, LONG_WAIT)) {
				case __EVENT_DATA_IND:
					i++;
					break;
				case __EVENT_NO_MSG:
					continue;
				default:
					goto failure;
			}
			state++;
		}
	show = 1;
	return (__RESULT_SUCCESS);
      failure:
	show = 1;
	return (__RESULT_FAILURE);
}

#define preamble_10_2_conn	preamble_5_conn
#define preamble_10_2_resp	preamble_5_resp
#define preamble_10_2_list	preamble_5_list

#define postamble_10_2_conn	postamble_3_conn
#define postamble_10_2_resp	postamble_3_resp
#define postamble_10_2_list	postamble_3_list

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

int conn_run(struct test_side *side)
{
	int result = __RESULT_SCRIPT_ERROR;
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "--------------------+------------Preamble-----------+--+                    \n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	state = 0;
	if (side->preamble && side->preamble(conn_fd) != __RESULT_SUCCESS) {
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "???????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = __RESULT_INCONCLUSIVE;
	} else {
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "--------------------+--------------Test-------------+--+                    \n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		switch (side->testcase(conn_fd)) {
		default:
		case __RESULT_INCONCLUSIVE:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "???????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_FAILURE:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "####################|########## SCRIPT ERROR #######|##|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			if (verbose > 2) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "********************|************ PASSED ***********|**|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SUCCESS;
			break;
		}
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "--------------------+------------Postamble----------+--+                    \n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		if (side->postamble && side->postamble(conn_fd) != __RESULT_SUCCESS) {
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "???????????????????\?|???????\? INCONCLUSIVE ????????\?|?\?|                    [%d]\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == __RESULT_SUCCESS)
				result = __RESULT_INCONCLUSIVE;
		}
	}
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "--------------------+-------------------------------+--+                    \n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	exit(result);
}

int resp_run(struct test_side *side)
{
	int result = __RESULT_SCRIPT_ERROR;
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +------------Preamble-----------+  +--------------------\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	state = 0;
	if (side->preamble && side->preamble(resp_fd) != __RESULT_SUCCESS) {
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|  |???????????????????\?(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = __RESULT_INCONCLUSIVE;
	} else {
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +--------------Test-------------+  +--------------------\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		switch (side->testcase(resp_fd)) {
		default:
		case __RESULT_INCONCLUSIVE:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|  |???????????????????\?(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_FAILURE:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |XXXXXXXXXXXX FAILED XXXXXXXXXXX|  |XXXXXXXXXXXXXXXXXXX(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |######### SCRIPT ERROR ########|  |###################(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			if (verbose > 2) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |************ PASSED ***********|  |********************(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SUCCESS;
			break;
		}
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +------------Postamble----------+  +--------------------\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		if (side->postamble && side->postamble(resp_fd) != __RESULT_SUCCESS) {
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|  |???????????????????\?(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == __RESULT_SUCCESS)
				result = __RESULT_INCONCLUSIVE;
		}
	}
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +-------------------------------+  +--------------------\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	exit(result);
}

int list_run(struct test_side *side)
{
	int result = __RESULT_SCRIPT_ERROR;
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +------------Preamble-----------+--+--------------------\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	state = 0;
	if (side->preamble && side->preamble(list_fd) != __RESULT_SUCCESS) {
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|?\?|???????????????????\?(%d)\n", state);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		result = __RESULT_INCONCLUSIVE;
	} else {
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +--------------Test-------------+--+--------------------\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		switch (side->testcase(list_fd)) {
		default:
		case __RESULT_INCONCLUSIVE:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|?\?|???????????????????\?(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_FAILURE:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |XXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |######### SCRIPT ERROR ########|##|###################(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			if (verbose > 2) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |************ PASSED ***********|**|********************(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			result = __RESULT_SUCCESS;
			break;
		}
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "                    +------------Postamble----------+--+--------------------\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		state = 0;
		if (side->postamble && side->postamble(list_fd) != __RESULT_SUCCESS) {
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "                    |???????\? INCONCLUSIVE ????????\?|?\?|???????????????????\?(%d)\n", state);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			if (result == __RESULT_SUCCESS)
				result = __RESULT_INCONCLUSIVE;
		}
	}
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "                    +-------------------------------+--+--------------------\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	exit(result);
}

/* 
 *  Fork multiple children to do the actual testing.  The conn child is the
 *  connecting process, the resp child is the responding process.
 */

int test_run(struct test_side *conn_side, struct test_side *resp_side, struct test_side *list_side)
{
	int children = 0;
	pid_t got_chld, conn_chld = 0, resp_chld = 0, list_chld = 0;
	int got_stat, conn_stat, resp_stat, list_stat;
	start_tt(5000);
	if (conn_side) {
		switch ((conn_chld = fork())) {
		case 00:	/* we are the child */
			exit(conn_run(conn_side));	/* execute conn state machine */
		case -1:	/* error */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		conn_stat = __RESULT_SUCCESS;
	if (resp_side) {
		switch ((resp_chld = fork())) {
		case 00:	/* we are the child */
			exit(resp_run(resp_side));	/* execute resp state machine */
		case -1:	/* error */
			if (conn_chld)
				kill(conn_chld, SIGKILL);	/* toast conn child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		resp_stat = __RESULT_SUCCESS;
	if (list_side) {
		switch ((list_chld = fork())) {
		case 00:	/* we are the child */
			exit(list_run(list_side));	/* execute list state machine */
		case -1:	/* error */
			if (conn_chld)
				kill(conn_chld, SIGKILL);	/* toast conn child */
			if (resp_chld)
				kill(resp_chld, SIGKILL);	/* toast resp child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		list_stat = __RESULT_SUCCESS;
	for (; children > 0; children--) {
		if ((got_chld = wait(&got_stat)) > 0) {
			if (WIFEXITED(got_stat)) {
				int status = WEXITSTATUS(got_stat);
				if (got_chld == conn_chld) {
					conn_stat = status;
					conn_chld = 0;
				}
				if (got_chld == resp_chld) {
					resp_stat = status;
					resp_chld = 0;
				}
				if (got_chld == list_chld) {
					list_stat = status;
					list_chld = 0;
				}
			} else if (WIFSIGNALED(got_stat)) {
				int signal = WTERMSIG(got_stat);
				if (got_chld == conn_chld) {
					if (verbose > 0) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@ TERMINATED @@@@@@@@@|  |                    {%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (resp_chld)
						kill(resp_chld, SIGKILL);
					if (list_chld)
						kill(list_chld, SIGKILL);
					conn_stat = __RESULT_FAILURE;
					conn_chld = 0;
				}
				if (got_chld == resp_chld) {
					if (verbose > 0) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |@@@@@@@@@@ TERMINATED @@@@@@@@@|  |@@@@@@@@@@@@@@@@@@@@{%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (conn_chld)
						kill(conn_chld, SIGKILL);
					if (list_chld)
						kill(list_chld, SIGKILL);
					resp_stat = __RESULT_FAILURE;
					resp_chld = 0;
				}
				if (got_chld == list_chld) {
					if (verbose > 0) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@@{%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (conn_chld)
						kill(conn_chld, SIGKILL);
					if (resp_chld)
						kill(resp_chld, SIGKILL);
					list_stat = __RESULT_FAILURE;
					list_chld = 0;
				}
			} else if (WIFSTOPPED(got_stat)) {
				int signal = WSTOPSIG(got_stat);
				if (got_chld == conn_chld) {
					if (verbose > 0) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |                    {%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (resp_chld)
						kill(resp_chld, SIGKILL);
					if (list_chld)
						kill(list_chld, SIGKILL);
					conn_stat = __RESULT_FAILURE;
					conn_chld = 0;
				}
				if (got_chld == resp_chld) {
					if (verbose > 0) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |&&&&&&&&&&&&&&&&&&&&{%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (conn_chld)
						kill(conn_chld, SIGKILL);
					if (list_chld)
						kill(list_chld, SIGKILL);
					resp_stat = __RESULT_FAILURE;
					resp_chld = 0;
				}
				if (got_chld == list_chld) {
					if (verbose > 0) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "                    |&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&&&{%d}\n", signal);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if (conn_chld)
						kill(conn_chld, SIGKILL);
					if (resp_chld)
						kill(resp_chld, SIGKILL);
					list_stat = __RESULT_FAILURE;
					list_chld = 0;
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
				last_event = __EVENT_TIMEOUT;
			}
			if (conn_chld) {
				kill(conn_chld, SIGKILL);
				conn_stat = __RESULT_FAILURE;
				conn_chld = 0;
			}
			if (resp_chld) {
				kill(resp_chld, SIGKILL);
				resp_stat = __RESULT_FAILURE;
				resp_chld = 0;
			}
			if (list_chld) {
				kill(list_chld, SIGKILL);
				list_stat = __RESULT_FAILURE;
				list_chld = 0;
			}
			break;
		}
	}
	stop_tt();
	if (conn_stat == __RESULT_FAILURE || resp_stat == __RESULT_FAILURE || list_stat == __RESULT_FAILURE)
		return (__RESULT_FAILURE);
	if (conn_stat == __RESULT_SUCCESS && resp_stat == __RESULT_SUCCESS && list_stat == __RESULT_SUCCESS)
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
	struct test_side conn;		/* connecting process */
	struct test_side resp;		/* responding process */
	struct test_side list;		/* listening process */
	int run;			/* whether to run this test */
	int result;			/* results of test */
} tests[] = {
	{
		numb_case_1_1, name_case_1_1, desc_case_1_1, {
		&preamble_1_1_conn, &test_case_1_1_conn, &postamble_1_1_conn}, {
		&preamble_1_1_resp, &test_case_1_1_resp, &postamble_1_1_resp}, {
	&preamble_1_1_list, &test_case_1_1_list, &postamble_1_1_list}, 0, 0}, {
		numb_case_1_2, name_case_1_2, desc_case_1_2, {
		&preamble_1_2_conn, &test_case_1_2_conn, &postamble_1_2_conn}, {
		&preamble_1_2_resp, &test_case_1_2_resp, &postamble_1_2_resp}, {
	&preamble_1_2_list, &test_case_1_2_list, &postamble_1_2_list}, 0, 0}, {
		numb_case_2_1, name_case_2_1, desc_case_2_1, {
		&preamble_2_1_conn, &test_case_2_1_conn, &postamble_2_1_conn}, {
		&preamble_2_1_resp, &test_case_2_1_resp, &postamble_2_1_resp}, {
	&preamble_2_1_list, &test_case_2_1_list, &postamble_2_1_list}, 0, 0}, {
		numb_case_2_2, name_case_2_2, desc_case_2_2, {
		&preamble_2_2_conn, &test_case_2_2_conn, &postamble_2_2_conn}, {
		&preamble_2_2_resp, &test_case_2_2_resp, &postamble_2_2_resp}, {
	&preamble_2_2_list, &test_case_2_2_list, &postamble_2_2_list}, 0, 0}, {
		numb_case_3_1, name_case_3_1, desc_case_3_1, {
		&preamble_3_1_conn, &test_case_3_1_conn, &postamble_3_1_conn}, {
		&preamble_3_1_resp, &test_case_3_1_resp, &postamble_3_1_resp}, {
	&preamble_3_1_list, &test_case_3_1_list, &postamble_3_1_list}, 0, 0}, {
		numb_case_3_2, name_case_3_2, desc_case_3_2, {
		&preamble_3_2_conn, &test_case_3_2_conn, &postamble_3_2_conn}, {
		&preamble_3_2_resp, &test_case_3_2_resp, &postamble_3_2_resp}, {
	&preamble_3_2_list, &test_case_3_2_list, &postamble_3_2_list}, 0, 0}, {
		numb_case_4_1, name_case_4_1, desc_case_4_1, {
		&preamble_4_1_conn, &test_case_4_1_conn, &postamble_4_1_conn}, {
		&preamble_4_1_resp, &test_case_4_1_resp, &postamble_4_1_resp}, {
	&preamble_4_1_list, &test_case_4_1_list, &postamble_4_1_list}, 0, 0}, {
		numb_case_4_2, name_case_4_2, desc_case_4_2, {
		&preamble_4_2_conn, &test_case_4_2_conn, &postamble_4_2_conn}, {
		&preamble_4_2_resp, &test_case_4_2_resp, &postamble_4_2_resp}, {
	&preamble_4_2_list, &test_case_4_2_list, &postamble_4_2_list}, 0, 0}, {
		numb_case_5_1, name_case_5_1, desc_case_5_1, {
		&preamble_5_1_conn, &test_case_5_1_conn, &postamble_5_1_conn}, {
		&preamble_5_1_resp, &test_case_5_1_resp, &postamble_5_1_resp}, {
	&preamble_5_1_list, &test_case_5_1_list, &postamble_5_1_list}, 0, 0}, {
		numb_case_5_2, name_case_5_2, desc_case_5_2, {
		&preamble_5_2_conn, &test_case_5_2_conn, &postamble_5_2_conn}, {
		&preamble_5_2_resp, &test_case_5_2_resp, &postamble_5_2_resp}, {
	&preamble_5_2_list, &test_case_5_2_list, &postamble_5_2_list}, 0, 0}, {
		numb_case_5_3, name_case_5_3, desc_case_5_3, {
		&preamble_5_3_conn, &test_case_5_3_conn, &postamble_5_3_conn}, {
		&preamble_5_3_resp, &test_case_5_3_resp, &postamble_5_3_resp}, {
	&preamble_5_3_list, &test_case_5_3_list, &postamble_5_3_list}, 0, 0}, {
		numb_case_6_1, name_case_6_1, desc_case_6_1, {
		&preamble_6_1_conn, &test_case_6_1_conn, &postamble_6_1_conn}, {
		&preamble_6_1_resp, &test_case_6_1_resp, &postamble_6_1_resp}, {
	&preamble_6_1_list, &test_case_6_1_list, &postamble_6_1_list}, 0, 0}, {
		numb_case_6_2, name_case_6_2, desc_case_6_2, {
		&preamble_6_2_conn, &test_case_6_2_conn, &postamble_6_2_conn}, {
		&preamble_6_2_resp, &test_case_6_2_resp, &postamble_6_2_resp}, {
	&preamble_6_2_list, &test_case_6_2_list, &postamble_6_2_list}, 0, 0}, {
		numb_case_6_3, name_case_6_3, desc_case_6_3, {
		&preamble_6_3_conn, &test_case_6_3_conn, &postamble_6_3_conn}, {
		&preamble_6_3_resp, &test_case_6_3_resp, &postamble_6_3_resp}, {
	&preamble_6_3_list, &test_case_6_3_list, &postamble_6_3_list}, 0, 0}, {
		numb_case_7_1, name_case_7_1, desc_case_7_1, {
		&preamble_7_1_conn, &test_case_7_1_conn, &postamble_7_1_conn}, {
		&preamble_7_1_resp, &test_case_7_1_resp, &postamble_7_1_resp}, {
	&preamble_7_1_list, &test_case_7_1_list, &postamble_7_1_list}, 0, 0}, {
		numb_case_7_2, name_case_7_2, desc_case_7_2, {
		&preamble_7_2_conn, &test_case_7_2_conn, &postamble_7_2_conn}, {
		&preamble_7_2_resp, &test_case_7_2_resp, &postamble_7_2_resp}, {
	&preamble_7_2_list, &test_case_7_2_list, &postamble_7_2_list}, 0, 0}, {
		numb_case_8_1, name_case_8_1, desc_case_8_1, {
		&preamble_8_1_conn, &test_case_8_1_conn, &postamble_8_1_conn}, {
		&preamble_8_1_resp, &test_case_8_1_resp, &postamble_8_1_resp}, {
	&preamble_8_1_list, &test_case_8_1_list, &postamble_8_1_list}, 0, 0}, {
		numb_case_8_2, name_case_8_2, desc_case_8_2, {
		&preamble_8_2_conn, &test_case_8_2_conn, &postamble_8_2_conn}, {
		&preamble_8_2_resp, &test_case_8_2_resp, &postamble_8_2_resp}, {
	&preamble_8_2_list, &test_case_8_2_list, &postamble_8_2_list}, 0, 0}, {
		numb_case_8_3, name_case_8_3, desc_case_8_3, {
		&preamble_8_3_conn, &test_case_8_3_conn, &postamble_8_3_conn}, {
		&preamble_8_3_resp, &test_case_8_3_resp, &postamble_8_3_resp}, {
	&preamble_8_3_list, &test_case_8_3_list, &postamble_8_3_list}, 0, 0}, {
		numb_case_8_4, name_case_8_4, desc_case_8_4, {
		&preamble_8_4_conn, &test_case_8_4_conn, &postamble_8_4_conn}, {
		&preamble_8_4_resp, &test_case_8_4_resp, &postamble_8_4_resp}, {
	&preamble_8_4_list, &test_case_8_4_list, &postamble_8_4_list}, 0, 0}, {
		numb_case_9_1, name_case_9_1, desc_case_9_1, {
		&preamble_9_1_conn, &test_case_9_1_conn, &postamble_9_1_conn}, {
		&preamble_9_1_resp, &test_case_9_1_resp, &postamble_9_1_resp}, {
	&preamble_9_1_list, &test_case_9_1_list, &postamble_9_1_list}, 0, 0}, {
		numb_case_9_2, name_case_9_2, desc_case_9_2, {
		&preamble_9_2_conn, &test_case_9_2_conn, &postamble_9_2_conn}, {
		&preamble_9_2_resp, &test_case_9_2_resp, &postamble_9_2_resp}, {
	&preamble_9_2_list, &test_case_9_2_list, &postamble_9_2_list}, 0, 0}, {
		numb_case_9_3, name_case_9_3, desc_case_9_3, {
		&preamble_9_3_conn, &test_case_9_3_conn, &postamble_9_3_conn}, {
		&preamble_9_3_resp, &test_case_9_3_resp, &postamble_9_3_resp}, {
	&preamble_9_3_list, &test_case_9_3_list, &postamble_9_3_list}, 0, 0}, {
		numb_case_9_4, name_case_9_4, desc_case_9_4, {
		&preamble_9_4_conn, &test_case_9_4_conn, &postamble_9_4_conn}, {
		&preamble_9_4_resp, &test_case_9_4_resp, &postamble_9_4_resp}, {
	&preamble_9_4_list, &test_case_9_4_list, &postamble_9_4_list}, 0, 0}, {
		numb_case_10_1, name_case_10_1, desc_case_10_1, {
		&preamble_10_1_conn, &test_case_10_1_conn, &postamble_10_1_conn}, {
		&preamble_10_1_resp, &test_case_10_1_resp, &postamble_10_1_resp}, {
	&preamble_10_1_list, &test_case_10_1_list, &postamble_10_1_list}, 0, 0}, {
		numb_case_10_2, name_case_10_2, desc_case_10_2, {
		&preamble_10_2_conn, &test_case_10_2_conn, &postamble_10_2_conn}, {
		&preamble_10_2_resp, &test_case_10_2_resp, &postamble_10_2_resp}, {
	&preamble_10_2_list, &test_case_10_2_list, &postamble_10_2_list}, 0, 0}, {
	NULL,}
};

int do_tests(void)
{
	int i;
	int result = __RESULT_INCONCLUSIVE;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	int num_exit;
	if (verbose > 0) {
		lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "\n\nRFC 2960 SCTP - OpenSS7 STREAMS SCTP - Conformance Test Program.\n");
		fflush(stdout);
		lockf(fileno(stdout), F_ULOCK, 0);
	}
	show = 0;
	state = 0;
	if (begin_tests() == __RESULT_SUCCESS) {
		end_tests();
		show = 1;
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (tests[i].result)
				continue;
			if (!tests[i].run) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				continue;
			}
			if (verbose > 0) {
				lockf(fileno(stdout), F_LOCK, 0);
				fprintf(stdout, "\nTest Case SCTP-TPI/%s: %s\n", tests[i].numb, tests[i].name);
				if (verbose > 1)
					fprintf(stdout, "%s\n", tests[i].desc);
				fflush(stdout);
				lockf(fileno(stdout), F_ULOCK, 0);
			}
			state = 0;
			if ((result = begin_tests()) != __RESULT_SUCCESS)
				goto inconclusive;
			result = test_run(&tests[i].conn, &tests[i].resp, &tests[i].list);
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
		}
		if (summary && verbose) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
			for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
				if (tests[i].run) {
					lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "Test Case SCTP-TPI/%-10s ", tests[i].numb);
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
			fprintf(stdout, "Done.\n\n");
			fprintf(stdout, "========= %2d successes   \n", successes);
			fprintf(stdout, "========= %2d failures    \n", failures);
			fprintf(stdout, "========= %2d inconclusive\n", inconclusive);
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (0);
	} else {
		end_tests();
		show = 1;
		if (verbose > 0) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "Test setup failed!\n");
			fflush(stdout);
			lockf(fileno(stdout), F_ULOCK, 0);
		}
		return (2);
	}
}

void copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
RFC 2960 SCTP - OpenSS7 STREAMS SCTP - Conformance Test Suite\n\
\n\
Copyright (c) 2001-2005 OpenSS7 Corporation <http://www.openss7.com/>\n\
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
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
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
    Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
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
    %1$s {-C, --copying}\n\
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
", argv[0]);
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
			{ 0, }
		};
		/* *INDENT-ON* */
		c = getopt_long(argc, argv, "l::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "l::f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'l':
			if (optarg) {
				l = strnlen(optarg, 16);
				fprintf(stdout, "\n");
				for (n = 0, t = tests; t->numb; t++)
					if (!strncmp(t->numb, optarg, l)) {
						fprintf(stdout, "Test Case SCTP-TPI/%s: %s\n", t->numb, t->name);
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
					fprintf(stdout, "Test Case SCTP-TPI/%s: %s\n", t->numb, t->name);
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
