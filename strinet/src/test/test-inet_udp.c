/*****************************************************************************

 @(#) $RCSfile: test-inet_udp.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/06/06 12:11:46 $

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

 Last Modified $Date: 2005/06/06 12:11:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-inet_udp.c,v $
 Revision 0.9.2.11  2005/06/06 12:11:46  brian
 - more upgrades to test suites

 Revision 0.9.2.10  2005/06/04 13:38:48  brian
 - final workup of test suites

 Revision 0.9.2.9  2005/06/04 09:13:55  brian
 - test suite corrections

 Revision 0.9.2.8  2005/06/04 05:01:42  brian
 - working up test suite upgrade

 Revision 0.9.2.7  2005/06/04 03:02:18  brian
 - upgraded test suites

 Revision 0.9.2.6  2005/05/14 08:28:53  brian
 - copyright header correction

 Revision 0.9.2.5  2005/04/04 16:43:17  brian
 - removed references to HZ

 Revision 0.9.2.4  2005/01/25 16:09:59  brian
 - Add check for <sys/wait.h>.

 Revision 0.9.2.3  2005/01/22 16:38:22  brian
 - Fixed compiler warnings.

 Revision 0.9.2.2  2004/09/02 10:07:37  brian
 - Updates for LFS compile.

 Revision 0.9.2.1  2004/06/27 10:08:38  brian
 - Built up separate inet release.

 Revision 0.9.2.1  2004/05/16 04:12:36  brian
 - Updating strxnet release.

 Revision 1.1.4.11  2004/04/13 12:12:57  brian
 - Rearranged header files.

 Revision 1.1.4.10  2004/04/13 06:04:05  brian
 - INET driver works pretty good now.

 Revision 1.1.4.9  2004/04/12 20:50:05  brian
 - Added ability to list test cases.

 Revision 1.1.4.8  2004/04/12 20:17:59  brian
 - Test cases pass.

 Revision 1.1.4.7  2004/04/12 11:34:31  brian
 - INET driver working better now for UDP.

 Revision 1.1.4.6  2004/04/09 11:24:57  brian
 - Cleaned up printouts and verbosity.

 Revision 1.1.4.5  2004/04/08 14:53:59  brian
 - Added more test cases.

 Revision 1.1.4.4  2004/04/08 13:14:33  brian
 - Updated INET-UDP test program.

 Revision 1.1.4.3  2004/03/31 09:00:53  brian
 - Working up new inet driver and documentation.

 Revision 1.1.4.2  2004/03/28 17:30:21  brian
 - First clean compile of inet updates.

 Revision 1.1.4.1  2004/01/12 23:33:19  brian
 - Updated LiS-2.16.18 gcom release to autoconf.

 Revision 1.1.2.3  2004/01/07 11:34:54  brian
 - Updated copyright dates.

 Revision 1.1.2.2  2004/01/04 11:31:24  brian
 - Corrected xti include.

 Revision 1.1.2.1  2003/12/23 11:12:24  brian
 - Added INET streams test programs.

 *****************************************************************************/

#ident "@(#) $RCSfile: test-inet_udp.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/06/06 12:11:46 $"

static char const ident[] = "$RCSfile: test-inet_udp.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2005/06/06 12:11:46 $";

/*
 *  Simple test program for INET streams.
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NEED_T_USCALAR_T

#include <xti.h>
#include <tihdr.h>
#include <timod.h>
#include <xti_inet.h>
#include <sys/xti_sctp.h>

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "OpenSS7 INET Driver - UDP";
/* static const char *spkgname = "INET"; */
static const char *lstdname = "XNS 5.2/TPI Rev 2";
static const char *sstdname = "XNS/TPI";
static const char *shortname = "INET/UDP";
static char devname[256] = "/dev/udp";

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
static int last_t_errno = 0;
static int last_qlen = 2;
static int last_sequence = 1;
static int last_servtype = T_COTS_ORD;
static int last_provflag = T_SENDZERO | T_ORDRELDATA | T_XPG4_1;
static int last_tstate = TS_UNBND;

static int MORE_flag = 0;
static int DATA_flag = T_ODF_EX | T_ODF_MORE;

int test_fd[3] = { 0, 0, 0 };

#define BUFSIZE 5*4096

#define FFLUSH(stream)

#define SHORT_WAIT 100		// 10
#define NORMAL_WAIT 500		// 100
#define LONG_WAIT 5000		// 500
#define LONGER_WAIT 10000	// 5000


char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strbuf ctrl = { BUFSIZE, -1, cbuf };
struct strbuf data = { BUFSIZE, -1, dbuf };

static int test_pflags = MSG_BAND;	/* MSG_BAND | MSG_HIPRI */
static int test_pband = 0;
static int test_gflags = 0;		/* MSG_BAND | MSG_HIPRI */
static int test_gband = 0;
static int test_bufsize = 256;
static int test_tidu = 256;
static int test_mgmtflags = T_NEGOTIATE;
static struct sockaddr_in *test_addr = NULL;
static socklen_t test_alen = sizeof(*test_addr);
static const char *test_data = NULL;
static int test_resfd = -1;

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
		fprintf(stdout, "                    | %11.6g                    |  |                   <%d>\n", t, state);
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
 *  Addresses
 */
struct sockaddr_in addrs[4];
unsigned short ports[4] = { 10000, 10001, 10002, 10003 };
const char *addr_strings[4] = { "127.0.0.1", "127.0.0.2", "127.0.0.3", "127.0.0.4" };

/*
 *  Options
 */

/*
 * data options
 */
struct {
	struct t_opthdr tos_hdr __attribute__ ((packed));
	t_scalar_t tos_val __attribute__ ((packed));
	struct t_opthdr ttl_hdr __attribute__ ((packed));
	t_scalar_t ttl_val __attribute__ ((packed));
	struct t_opthdr drt_hdr __attribute__ ((packed));
	t_scalar_t drt_val __attribute__ ((packed));
	struct t_opthdr csm_hdr __attribute__ ((packed));
	t_scalar_t csm_val __attribute__ ((packed));
#if 0
	struct t_opthdr ppi_hdr __attribute__ ((packed));
	t_scalar_t ppi_val __attribute__ ((packed));
	struct t_opthdr sid_hdr __attribute__ ((packed));
	t_scalar_t sid_val __attribute__ ((packed));
#endif
} opt_data = {
	{ sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TOS, T_SUCCESS}, 0x0
	, { sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TTL, T_SUCCESS}, 64
	, { sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_DONTROUTE, T_SUCCESS}, T_NO
	, { sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_UDP, T_UDP_CHECKSUM, T_SUCCESS}, T_NO
#if 0
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS}, 10
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS}, 0
#endif
};

/*
 * connect options
 */
struct {
	struct t_opthdr tos_hdr __attribute__ ((packed));
	t_scalar_t tos_val __attribute__ ((packed));
	struct t_opthdr ttl_hdr __attribute__ ((packed));
	t_scalar_t ttl_val __attribute__ ((packed));
	struct t_opthdr drt_hdr __attribute__ ((packed));
	t_scalar_t drt_val __attribute__ ((packed));
	struct t_opthdr bca_hdr __attribute__ ((packed));
	t_scalar_t bca_val __attribute__ ((packed));
	struct t_opthdr reu_hdr __attribute__ ((packed));
	t_scalar_t reu_val __attribute__ ((packed));
#if 0
	struct t_opthdr ist_hdr __attribute__ ((packed));
	t_scalar_t ist_val __attribute__ ((packed));
	struct t_opthdr ost_hdr __attribute__ ((packed));
	t_scalar_t ost_val __attribute__ ((packed));
#endif
} opt_conn = {
	{ sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TOS, T_SUCCESS}, 0x0
	, { sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TTL, T_SUCCESS}, 64
	, { sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_DONTROUTE, T_SUCCESS}, T_NO
	, { sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_BROADCAST, T_SUCCESS}, T_NO
	, { sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_IP, T_IP_REUSEADDR, T_SUCCESS}, T_NO
#if 0
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS, T_SUCCESS}, 1
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS, T_SUCCESS}, 1
#endif
};

/*
 * management options
 */
struct {
	struct t_opthdr tos_hdr __attribute__ ((packed));
	t_scalar_t tos_val __attribute__ ((packed));
	struct t_opthdr ttl_hdr __attribute__ ((packed));
	t_scalar_t ttl_val __attribute__ ((packed));
	struct t_opthdr drt_hdr __attribute__ ((packed));
	t_scalar_t drt_val __attribute__ ((packed));
	struct t_opthdr bca_hdr __attribute__ ((packed));
	t_scalar_t bca_val __attribute__ ((packed));
	struct t_opthdr reu_hdr __attribute__ ((packed));
	t_scalar_t reu_val __attribute__ ((packed));
#if 0
	struct t_opthdr ndl_hdr __attribute__ ((packed));
	t_scalar_t ndl_val __attribute__ ((packed));
	struct t_opthdr mxs_hdr __attribute__ ((packed));
	t_scalar_t mxs_val __attribute__ ((packed));
	struct t_opthdr kpa_hdr __attribute__ ((packed));
	t_scalar_t kpa_val __attribute__ ((packed));
#endif
	struct t_opthdr csm_hdr __attribute__ ((packed));
	t_scalar_t csm_val __attribute__ ((packed));
#if 0
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
#endif
} opt_optm = {
	{ sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TOS, T_SUCCESS}, 0x0
	, { sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TTL, T_SUCCESS}, 64
	, { sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_DONTROUTE, T_SUCCESS}, T_NO
	, { sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_BROADCAST, T_SUCCESS}, T_NO
	, { sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_REUSEADDR, T_SUCCESS}, T_NO
#if 0
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_TCP, T_TCP_NODELAY, T_SUCCESS}, T_NO
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_TCP, T_TCP_MAXSEG, T_SUCCESS}, 576
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_TCP, T_TCP_KEEPALIVE, T_SUCCESS}, T_NO
#endif
	, { sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_UDP, T_UDP_CHECKSUM, T_SUCCESS}, T_NO
#if 0
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_NODELAY, T_SUCCESS}, T_YES
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_CORK, T_SUCCESS}, T_YES
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS}, 10
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS}, 0
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RECVOPT, T_SUCCESS}, T_NO
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_LIFE, T_SUCCESS}, 60000
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SACK_DELAY, T_SUCCESS}, 200
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PATH_MAX_RETRANS, T_SUCCESS}, 5
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ASSOC_MAX_RETRANS, T_SUCCESS}, 12
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAX_INIT_RETRIES, T_SUCCESS}, 12
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_HEARTBEAT_ITVL, T_SUCCESS}, 1000
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_INITIAL, T_SUCCESS}, 200
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MIN, T_SUCCESS}, 10
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MAX, T_SUCCESS}, 2000
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS, T_SUCCESS}, 1
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS, T_SUCCESS}, 1
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_INC, T_SUCCESS}, 1000
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_THROTTLE_ITVL, T_SUCCESS}, 50
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAC_TYPE, T_SUCCESS}, T_SCTP_HMAC_NONE
	, { sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_DEBUG, T_SUCCESS}, 0
#endif
};

#if 0
typeof(opt_data) * test_opt_data = NULL;
typeof(opt_conn) * test_opt_conn = NULL;
typeof(opt_optm) * test_opt_optm = NULL;
#endif
#if 1
typeof(opt_data) * test_opt_data = &opt_data;
typeof(opt_conn) * test_opt_conn = &opt_conn;
typeof(opt_optm) * test_opt_optm = &opt_optm;
#endif

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
	case __TEST_CONN_REQ:
		return ("T_CONN_REQ");
	case __TEST_CONN_RES:
		return ("T_CONN_RES");
	case __TEST_DISCON_REQ:
		return ("T_DISCON_REQ");
	case __TEST_DATA_REQ:
		return ("T_DATA_REQ");
	case __TEST_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case __TEST_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case __TEST_INFO_REQ:
		return ("T_INFO_REQ");
	case __TEST_BIND_REQ:
		return ("T_BIND_REQ");
	case __TEST_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case __TEST_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case __TEST_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case __TEST_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case __TEST_CONN_IND:
		return ("T_CONN_IND");
	case __TEST_CONN_CON:
		return ("T_CONN_CON");
	case __TEST_DISCON_IND:
		return ("T_DISCON_IND");
	case __TEST_DATA_IND:
		return ("T_DATA_IND");
	case __TEST_EXDATA_IND:
		return ("T_EXDATA_IND");
	case __TEST_NRM_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case __TEST_EXP_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case __TEST_INFO_ACK:
		return ("T_INFO_ACK");
	case __TEST_BIND_ACK:
		return ("T_BIND_ACK");
	case __TEST_ERROR_ACK:
		return ("T_ERROR_ACK");
	case __TEST_OK_ACK:
		return ("T_OK_ACK");
	case __TEST_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case __TEST_UDERROR_IND:
		return ("T_UDERROR_IND");
	case __TEST_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case __TEST_ORDREL_IND:
		return ("T_ORDREL_IND");
	case __TEST_ADDR_REQ:
		return ("T_ADDR_REQ");
	case __TEST_ADDR_ACK:
		return ("T_ADDR_ACK");
	case __TEST_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case __TEST_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
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

#if 0
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
	/* len += snprintf(buf + len, sizeof(buf) - len, "\0"); */
	return buf;
}
void print_addrs(int fd, char *add_ptr, size_t add_len)
{
	fprintf(stdout, "Stupid!\n");
}
#else
void print_addr(char *add_ptr, size_t add_len)
{
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	if (add_len) {
		if (add_len != sizeof(*a))
			printf("Aaarrg! add_len = %d, ", add_len);
		printf("%d.%d.%d.%d:%d", (a->sin_addr.s_addr >> 0) & 0xff, (a->sin_addr.s_addr >> 8) & 0xff, (a->sin_addr.s_addr >> 16) & 0xff, (a->sin_addr.s_addr >> 24) & 0xff, ntohs(a->sin_port));
	} else
		printf("(no address)");
	printf("\n");
}

char *addr_string(char *add_ptr, size_t add_len)
{
	static char buf[128];
	size_t len = 0;
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	if (add_len) {
		if (add_len != sizeof(*a))
			len += snprintf(buf + len, sizeof(buf) - len, "Aaarrg! add_len = %d, ", add_len);
		len += snprintf(buf + len, sizeof(buf) - len, "%d.%d.%d.%d:%d", (a->sin_addr.s_addr >> 0) & 0xff, (a->sin_addr.s_addr >> 8) & 0xff, (a->sin_addr.s_addr >> 16) & 0xff, (a->sin_addr.s_addr >> 24) & 0xff, ntohs(a->sin_port));
	} else
		len += snprintf(buf + len, sizeof(buf) - len, "(no address)");
	/* snprintf(buf + len, sizeof(buf) - len, "\0"); */
	return buf;
}
#endif

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
#if 0
	case T_INET_SCTP:
		return ("T_INET_SCTP");
#endif
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
#if 0
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
#endif
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
	snprintf(buf, 32, "%d", *((t_scalar_t *) T_OPT_DATA(oh)));
	return (buf);
}

char *value_string(struct t_opthdr *oh)
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
#if 0
	case T_INET_SCTP:
		switch (oh->name) {
		case T_SCTP_NODELAY:
			return yesno_string(oh);
		case T_SCTP_CORK:
			return yesno_string(oh);
		case T_SCTP_PPI:
			return number_string(oh);;
		case T_SCTP_SID:
			sid[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
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
#endif
	}
	return ("(unknown value)");
}

#if 0
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
#endif

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

char *t_errno_string(long err, long syserr)
{
	switch (err) {
	case 0:
		return ("ok");
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
		return errno_string(syserr);
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
		snprintf(buf, sizeof(buf), "[%ld]", err);
		return buf;
	}
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

void print_simple_string(int child, const char *msgs[], const char *string)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string);
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
		"  open()      ----->v                                   .                   \n",
		"                    |                                   v<-----     open()  \n",
		"                    |                                v<-+------     open()  \n",
		"                    .                                .  .                   \n",
	};
	if (verbose > 3)
		print_simple(child, msgs);
}

void print_close(int child)
{
	static const char *msgs[] = {
		"  close()     ----->X                                   |                   \n",
		"                    .                                   X<-----    close()  \n",
		"                    .                                X<-+------    close()  \n",
		"                    .                                .  .                   \n",
	};
	if (verbose > 3)
		print_simple(child, msgs);
}

void print_preamble(int child)
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

void print_inconclusive(int child)
{
	static const char *msgs[] = {
		"????????????????????|?????????? INCONCLUSIVE ????????|??|                   [%d]\n",
		"                    |?????????? INCONCLUSIVE ????????|  |???????????????????[%d]\n",
		"                    |?????????? INCONCLUSIVE ????????|??|???????????????????[%d]\n",
		"????????????????????|?????????? INCONCLUSIVE ????????|??|???????????????????[%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
}

void print_test(int child)
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

void print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|                   [%d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXX|  |XXXXXXXXXXXXXXXXXXX[%d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX[%d]\n",
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX[%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
}

void print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########### SCRIPT ERROR #######|##|                   [%d]\n",
		"                    |########### SCRIPT ERROR #######|  |###################[%d]\n",
		"                    |########### SCRIPT ERROR #######|##|###################[%d]\n",
		"####################|########### SCRIPT ERROR #######|##|###################[%d]\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, state);
}

void print_passed(int child)
{
	static const char *msgs[] = {
		"********************|************* PASSED ***********|**|                   [%d]\n",
		"                    |************* PASSED ***********|  |*******************[%d]\n",
		"                    |************* PASSED ***********|**|*******************[%d]\n",
		"********************|************* PASSED ***********|**|*******************[%d]\n",
	};
	if (verbose > 2)
		print_simple_int(child, msgs, state);
}

void print_postamble(int child)
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

void print_test_end(int child)
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

void print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|                   {%d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@|  |@@@@@@@@@@@@@@@@@@@{%d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@{%d}\n",
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@{%d}\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, signal);
}

void print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|                   {%d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&|  |&&&&&&&&&&&&&&&&&&&{%d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&&{%d}\n",
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&&{%d}\n",
	};
	if (verbose > 0)
		print_simple_int(child, msgs, signal);
}

void print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|++++++++++++ TIMEOUT! ++++++++++|++|                   [%d]\n",
		"                    |++++++++++++ TIMEOUT! ++++++++++|  |+++++++++++++++++++[%d]\n",
		"                    |++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++[%d]\n",
		"++++++++++++++++++++|++|+++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++[%d]\n",
	};
	if (show_timeout || verbose > 0) {
		print_simple_int(child, msgs, state);
		show_timeout--;
	}
}

void print_nothing(int child)
{
	static const char *msgs[] = {
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - |  |                   [%d]\n",
		"                    |- - - - - - -nothing! - - - - - |  |- - - - - - - - - -[%d]\n",
		"                    |- - - - - - -nothing! - - - - - | -|- - - - - - - - - -[%d]\n",
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - | -|- - - - - - - - - -[%d]\n",
	};
	if (verbose > 1)
		print_simple_int(child, msgs, state);
}

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
		"%-14s----->|                                |  |                   [%d]\n",
		"                    |                                |  |<---%-14s [%d]\n",
		"                    |                                |<-+----%-14s [%d]\n",
		"                    |                                |  |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_tx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"--%16s->|- - - - - - - - - - - - - - - ->|->|                   [%d]\n",
		"                    |<- - - - - - - - - - - - - - - -|- |<-%16s-[%d]\n",
		"                    |<- - - - - - - - - - - - - - - -|<----%16s-[%d]\n",
		"                    |                                |  |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_rx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s--|<- - - - - - - - - - - - - - - -| -|                   [%d]\n",
		"                    |- - - - - - - - - - - - - - - ->|  |-%16s->[%d]\n",
		"                    |- - - - - - - - - - - - - - - ->|--+-%16s->[%d]\n",
		"                    |         <%16s>     |  |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_ack_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s-/|<- - - - - - - - - - - - - - - -| -|                   [%d]\n",
		"                    |- - - - - - - - - - - - - - - ->|  |\\%16s->[%d]\n",
		"                    |- - - - - - - - - - - - - - - ->|\\-+-%16s->[%d]\n",
		"                    |         <%16s>     |  |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_long_state(int child, const char *msgs[], long value)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], value, state);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_no_prim(int child, long prim)
{
	static const char *msgs[] = {
		"????%4ld????  ?----?|?- - - - - - -?                |  |                    [%d]\n",
		"                    |                               |  |?--? ????%4ld????   [%d]\n",
		"                    |                               |?-+---? ????%4ld????   [%d]\n",
		"                    | ? - - - - - - %4ld  - - - - ? |  |                    [%d]\n",
	};
	if (verbose > 0)
		print_long_state(child, msgs, prim);
}

void print_string_int_state(int child, const char *msgs[], const char *string, int val)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, val, state);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_rx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"<-%1$16s--|<- -%2$4d bytes- - - - - - - - - |- |                   [%3$d]\n",
		"                    |- - %2$4d bytes- - - - - - - - ->|  |--%1$16s>[%3$d]\n",
		"                    |- - %2$4d bytes- - - - - - - - - |->|--%1$16s>[%3$d]\n",
		"                    |- - %2$4d bytes %1$16s |  |                   [%3$d]\n",
	};
	if ((verbose && show_data) || verbose > 1)
		print_string_int_state(child, msgs, command, bytes);
}

void print_errno(int child, long error)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                |  |                   [%d]\n",
		"                    |                                |  |\\-->%14s [%d]\n",
		"                    |                                |\\-+--->%14s [%d]\n",
		"                    |          [%14s]      |  |                   [%d]\n",
	};
	if (verbose > 3)
		print_string_state(child, msgs, errno_string(error));
}

void print_success(int child)
{
	static const char *msgs[] = {
		"  ok          <----/|                                |  |                   [%d]\n",
		"                    |                                |  |\\---->         ok  [%d]\n",
		"                    |                                |\\-+----->         ok  [%d]\n",
		"                    |                 ok             |  |                   [%d]\n",
	};
	if (verbose > 4)
		print_simple_int(child, msgs, state);
}

void print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"  %10d  <----/|                                |  |                   [%d]\n",
		"                    |                                |  |\\---->  %10d [%d]\n",
		"                    |                                |\\-+----->  %10d [%d]\n",
		"                    |            [%10d]        |  |                   [%d]\n",
	};
	if (verbose)
		print_double_int(child, msgs, value, state);
}

void print_ti_ioctl(int child, int cmd, intptr_t arg)
{
	static const char *msgs[] = {
		"--ioctl(2)--------->|       %16s         |  |                   [%d]\n",
		"                    |       %16s         |  |<---ioctl(2)------ [%d]\n",
		"                    |       %16s         |<-+----ioctl(2)------ [%d]\n",
		"                    |       %16s ioctl(2)|  |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, ioctl_string(cmd, arg));
}

void print_ioctl(int child, int cmd, intptr_t arg)
{
	if (verbose > 3)
		print_ti_ioctl(child, cmd, arg);
}

void print_datcall(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"  %1$16s->|- - %2$4d bytes- - - - - - - - ->|->|                   [%3$d]\n",
		"                    |< - %2$4d bytes- - - - - - - - - |- |<-%1$16s [%3$d]\n",
		"                    |< - %2$4d bytes- - - - - - - - - |<-+--%1$16s [%3$d]\n",
		"                    |- - %2$4d bytes %1$16s |  |                   [%3$d]\n",
	};
	if ((verbose && show_data) || verbose > 1)
		print_string_int_state(child, msgs, command, bytes);
}

void print_libcall(int child, const char *command)
{
	static const char *msgs[] = {
		"  %-16s->|                                |  |                   [%d]\n",
		"                    |                                |  |<-%16s [%d]\n",
		"                    |                                |<-+--%16s [%d]\n",
		"                    |        [%16s]      |  |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, command);
}

void print_terror(int child, long error, long terror)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                |  |                   [%d]\n",
		"                    |                                |  |\\-->%14s [%d]\n",
		"                    |                                |\\-+--->%14s [%d]\n",
		"                    |          [%14s]      |  |                   [%d]\n",
	};
	if (verbose > 0)
		print_string_state(child, msgs, t_errno_string(terror, error));
}

void print_expect(int child, int want)
{
	static const char *msgs[] = {
		" (%-16s) |- - - - - -[Expected]- - - - - -|  |                    [%d]\n",
		"                    |- - - - - -[Expected]- - - - - -|  | (%-16s) [%d]\n",
		"                    |- - - - - -[Expected]- - - - - -|- | (%-16s) [%d]\n",
		"                    |- [Expected %-16s ] -|- |                    [%d]\n",
	};
	if (verbose > 1 && show)
		print_string_state(child, msgs, event_string(want));
}

void print_string(int child, const char *string)
{
	static const char *msgs[] = {
		"%-20s|                                |  |                    \n",
		"                    |                                |  |%-20s\n",
		"                    |                                |   %-20s\n",
		"                    |       %-20s     |  |                    \n",
	};
	if (verbose > 1 && show)
		print_simple_string(child, msgs, string);
}

void print_time_state(int child, const char *msgs[], ulong time)
{
	lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], time, state);
	fflush(stdout);
	lockf(fileno(stdout), F_ULOCK, 0);
}

void print_waiting(int child, ulong time)
{
	static const char *msgs[] = {
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / /|  |                   [%d]\n",
		"                    | / / / Waiting %03lu seconds / / /|  | / / / / / / / / / [%d]\n",
		"                    | / / / Waiting %03lu seconds / / /|/ | / / / / / / / / / [%d]\n",
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / /|/ | / / / / / / / / / [%d]\n",
	};
	if (verbose > 0 && show)
		print_time_state(child, msgs, time);
}

void print_mgmtflag(int child, t_uscalar_t flag)
{
	print_string(child, mgmtflag_string(flag));
}

void print_opt_level(int child, struct t_opthdr *oh)
{
	char *level = level_string(oh);
	if (level)
		print_string(child, level);
}
void print_opt_name(int child, struct t_opthdr *oh)
{
	char *name = name_string(oh);
	if (name)
		print_string(child, name);
}
void print_opt_status(int child, struct t_opthdr *oh)
{
	char *status = status_string(oh);
	if (status)
		print_string(child, status);
}
void print_opt_value(int child, struct t_opthdr *oh)
{
	char *value = value_string(oh);
	if (value)
		print_string(child, value);
}
void print_options(int child, char *opt_ptr, size_t opt_len)
{
	struct t_opthdr *oh;
	if (verbose < 4)
		return;
	for (oh = _T_OPT_FIRSTHDR_OFS(opt_ptr, opt_len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(opt_ptr, opt_len, oh, 0)) {
		int len = oh->len - sizeof(*oh);
		if (len < 0)
			break;
		print_opt_level(child, oh);
		print_opt_name(child, oh);
		print_opt_status(child, oh);
		print_opt_value(child, oh);
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Driver actions.
 *
 *  -------------------------------------------------------------------------
 */
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
		if (verbose > 3)
			print_success_value(child, last_retval);
		return (__RESULT_SUCCESS);
	}
}

int test_insertfd(int child, int resfd,  int offset, struct strbuf *ctrl, struct strbuf *data, int flags)
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
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

int test_putpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int band, int flags)
{
	if (flags & MSG_BAND || band) {
		if (verbose > 3) {
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putpmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			lockf(fileno(stdout), F_ULOCK, 0);
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
			lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			lockf(fileno(stdout), F_ULOCK, 0);
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
		if (verbose > 3)
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

int test_nonblock(int child)
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

int test_block(int child)
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
 *  STREAM Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int stream_start(int child, int index)
{
	int offset = 3 * index;
	addrs[3].sin_family = AF_INET;
	addrs[3].sin_port = htons(ports[3] + offset);
	inet_aton(addr_strings[3], &addrs[3].sin_addr);
	switch (child) {
	case 1:
	case 2:
	case 0:
		addrs[child].sin_family = AF_INET;
		addrs[child].sin_port = htons(ports[child] + offset);
		inet_aton(addr_strings[child], &addrs[child].sin_addr);
		if (test_open(child, devname) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_ioctl(child, I_SRDOPT, (intptr_t) RMSGD) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		return __RESULT_SUCCESS;
	default:
		return __RESULT_FAILURE;
	}
}

static int stream_stop(int child)
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

/*
 *  -------------------------------------------------------------------------
 *
 *  Test harness initialization and termination.
 *
 *  -------------------------------------------------------------------------
 */

static int begin_tests(int index)
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

static int end_tests(void)
{
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

static int do_signal(int child, int action)
{
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
	switch (action) {
	case __TEST_PUSH:
		return test_ti_ioctl(child, I_PUSH, (intptr_t) "tirdwr");
	case __TEST_POP:
		return test_ti_ioctl(child, I_POP, (intptr_t) NULL);
	case __TEST_PUTMSG_DATA:
		ctrl = NULL;
		data->len = sprintf(dbuf, "Putmsg test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PUTPMSG_DATA:
		ctrl = NULL;
		data->len = sprintf(dbuf, "Putpmsg band test data.");
		test_pflags = MSG_BAND;
		test_pband = 1;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_REQ:
		ctrl->len = sizeof(p->conn_req)
		    + (test_addr ? test_alen : 0)
		    + (test_opt_conn ? sizeof(*test_opt_conn) : 0);
		p->conn_req.PRIM_type = T_CONN_REQ;
		p->conn_req.DEST_length = test_addr ? test_alen : 0;
		p->conn_req.DEST_offset = test_addr ? sizeof(p->conn_req) : 0;
		p->conn_req.OPT_length = test_opt_conn ? sizeof(*test_opt_conn) : 0;
		p->conn_req.OPT_offset = test_opt_conn ? sizeof(p->conn_req) + p->conn_req.DEST_length : 0;
		if (test_addr)
			bcopy(test_addr, ctrl->buf + p->conn_req.DEST_offset, p->conn_req.DEST_length);
		if (test_opt_conn)
			bcopy(test_opt_conn, ctrl->buf + p->conn_req.OPT_offset, p->conn_req.OPT_length);
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, addr_string(cbuf + p->conn_req.DEST_offset, p->conn_req.DEST_length));
		print_options(child, cbuf + p->conn_req.OPT_offset, p->conn_req.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_IND:
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
		print_tx_prim(child, prim_string(p->type));
		print_options(child, cbuf + p->conn_ind.OPT_offset, p->conn_ind.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_RES:
		ctrl->len = sizeof(p->conn_res);
		p->conn_res.PRIM_type = T_CONN_RES;
		p->conn_res.ACCEPTOR_id = 0;
		p->conn_res.OPT_length = 0;
		p->conn_res.OPT_offset = 0;
		p->conn_res.SEQ_number = last_sequence;
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_options(child, cbuf + p->conn_res.OPT_offset, p->conn_res.OPT_length);
		if (test_resfd == -1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		else
			return test_insertfd(child, test_resfd, 4, ctrl, data, 0);
	case __TEST_CONN_CON:
		ctrl->len = sizeof(p->conn_con);
		p->conn_con.PRIM_type = T_CONN_CON;
		p->conn_con.RES_length = 0;
		p->conn_con.RES_offset = 0;
		p->conn_con.OPT_length = 0;
		p->conn_con.OPT_offset = 0;
		data->len = sprintf(dbuf, "Connection confirmation test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_options(child, cbuf + p->conn_con.OPT_offset, p->conn_con.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_REQ:
		ctrl->len = sizeof(p->discon_req);
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_IND:
		ctrl->len = sizeof(p->discon_ind);
		p->discon_ind.PRIM_type = T_DISCON_IND;
		p->discon_ind.DISCON_reason = 0;
		p->discon_ind.SEQ_number = last_sequence;
		data->len = sprintf(dbuf, "Disconnection indication test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_REQ:
		ctrl->len = sizeof(p->data_req);
		p->data_req.PRIM_type = T_DATA_REQ;
		p->data_req.MORE_flag = MORE_flag;
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_IND:
		ctrl->len = sizeof(p->data_ind);
		p->data_ind.PRIM_type = T_DATA_IND;
		p->data_ind.MORE_flag = MORE_flag;
		data->len = sprintf(dbuf, "Normal test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_REQ:
		ctrl->len = sizeof(p->exdata_req);
		p->exdata_req.PRIM_type = T_EXDATA_REQ;
		p->exdata_req.MORE_flag = MORE_flag;
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 1;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_IND:
		ctrl->len = sizeof(p->exdata_ind);
		p->data_ind.PRIM_type = T_EXDATA_IND;
		p->data_ind.MORE_flag = MORE_flag;
		data->len = sprintf(dbuf, "Expedited test data.");
		test_pflags = MSG_BAND;
		test_pband = 1;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_REQ:
		ctrl->len = sizeof(p->info_req);
		p->info_req.PRIM_type = T_INFO_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_ACK:
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
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_REQ:
		ctrl->len = sizeof(p->bind_req) + (test_addr ? test_alen : 0);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = test_addr ? test_alen : 0;
		p->bind_req.ADDR_offset = test_addr ? sizeof(p->bind_req) : 0;
		p->bind_req.CONIND_number = last_qlen;
		if (test_addr)
			bcopy(test_addr, ctrl->buf + p->bind_req.ADDR_offset, p->bind_req.ADDR_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_ACK:
		ctrl->len = sizeof(p->bind_ack);
		p->bind_ack.PRIM_type = T_BIND_ACK;
		p->bind_ack.ADDR_length = 0;
		p->bind_ack.ADDR_offset = 0;
		p->bind_ack.CONIND_number = last_qlen;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNBIND_REQ:
		ctrl->len = sizeof(p->unbind_req);
		p->unbind_req.PRIM_type = T_UNBIND_REQ;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ERROR_ACK:
		ctrl->len = sizeof(p->error_ack);
		p->error_ack.PRIM_type = T_ERROR_ACK;
		p->error_ack.ERROR_prim = last_prim;
		p->error_ack.TLI_error = last_t_errno;
		p->error_ack.UNIX_error = last_errno;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, terrno_string(p->error_ack.TLI_error, p->error_ack.UNIX_error));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OK_ACK:
		ctrl->len = sizeof(p->ok_ack);
		p->ok_ack.PRIM_type = T_OK_ACK;
		p->ok_ack.CORRECT_prim = 0;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_REQ:
		ctrl->len = sizeof(p->unitdata_req)
		    + (test_addr ? test_alen : 0)
		    + (test_opt_data ? sizeof(*test_opt_data) : 0);
		p->unitdata_req.PRIM_type = T_UNITDATA_REQ;
		p->unitdata_req.DEST_length = test_addr ? test_alen : 0;
		p->unitdata_req.DEST_offset = test_addr ? sizeof(p->unitdata_req) : 0;
		p->unitdata_req.OPT_length = test_opt_data ? sizeof(*test_opt_data) : 0;
		p->unitdata_req.OPT_offset = test_opt_data ? sizeof(p->unitdata_req) + p->unitdata_req.DEST_length : 0;
		if (test_addr)
			bcopy(test_addr, ctrl->buf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length);
		if (test_opt_data)
			bcopy(test_opt_data, ctrl->buf + p->unitdata_req.OPT_offset, p->unitdata_req.OPT_length);
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, addr_string(cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length));
		print_options(child, cbuf + p->unitdata_req.OPT_offset, p->unitdata_req.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_IND:
		ctrl->len = sizeof(p->unitdata_ind);
		p->unitdata_ind.PRIM_type = T_UNITDATA_IND;
		p->unitdata_ind.SRC_length = 0;
		p->unitdata_ind.SRC_offset = 0;
		p->unitdata_ind.OPT_length = 0;
		p->unitdata_ind.OPT_offset = 0;
		data->len = sprintf(dbuf, "Unit test data indication.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, addr_string(cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length));
		print_options(child, cbuf + p->unitdata_ind.OPT_offset, p->unitdata_ind.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UDERROR_IND:
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
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTMGMT_REQ:
		ctrl->len = sizeof(p->optmgmt_req)
		    + (test_opt_optm ? sizeof(*test_opt_optm) : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opt_optm ? sizeof(*test_opt_optm) : 0;
		p->optmgmt_req.OPT_offset = test_opt_optm ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opt_optm)
			bcopy(test_opt_optm, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_mgmtflag(child, p->optmgmt_req.MGMT_flags);
		print_options(child, cbuf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTMGMT_ACK:
		ctrl->len = sizeof(p->optmgmt_ack);
		p->optmgmt_ack.PRIM_type = T_OPTMGMT_ACK;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_mgmtflag(child, p->optmgmt_ack.MGMT_flags);
		print_options(child, cbuf + p->optmgmt_ack.OPT_offset, p->optmgmt_ack.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ORDREL_REQ:
		ctrl->len = sizeof(p->ordrel_req);
		p->ordrel_req.PRIM_type = T_ORDREL_REQ;
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ORDREL_IND:
		ctrl->len = sizeof(p->ordrel_ind);
		p->ordrel_ind.PRIM_type = T_ORDREL_IND;
		data->len = sprintf(dbuf, "Orderly release indication test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTDATA_REQ:
		ctrl->len = sizeof(p->optdata_req)
		    + (test_opt_data ? sizeof(*test_opt_data) : 0);
		p->optdata_req.PRIM_type = T_OPTDATA_REQ;
		p->optdata_req.DATA_flag = DATA_flag;
		p->optdata_req.OPT_length = test_opt_data ? sizeof(*test_opt_data) : 0;
		p->optdata_req.OPT_offset = test_opt_data ? sizeof(p->optdata_req) : 0;
		if (test_data)
			data->len = sprintf(dbuf, test_data);
		else
			data = NULL;
		if (test_opt_data)
			bcopy(test_opt_data, ctrl->buf + p->optdata_req.OPT_offset, p->optdata_req.OPT_length);
		test_pflags = MSG_BAND;
		test_pband = (DATA_flag & T_ODF_EX) ? 1 : 0;
		if (p->optdata_req.DATA_flag & T_ODF_EX) {
			if (p->optdata_req.DATA_flag & T_ODF_MORE)
				print_tx_prim(child, "T_OPTDATA_REQ!+ ");
			else
				print_tx_prim(child, "T_OPTDATA_REQ!  ");
		} else {
			if (p->optdata_req.DATA_flag & T_ODF_MORE)
				print_tx_prim(child, "T_OPTDATA_REQ+  ");
			else
				print_tx_prim(child, "T_OPTDATA_REQ   ");
		}
		print_options(child, cbuf + p->optdata_req.OPT_offset, p->optdata_req.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_NRM_OPTDATA_IND:
		ctrl->len = sizeof(p->optdata_ind);
		p->optdata_ind.PRIM_type = T_OPTDATA_IND;
		p->optdata_ind.DATA_flag = 0;
		p->optdata_ind.OPT_length = 0;
		p->optdata_ind.OPT_offset = 0;
		data->len = sprintf(dbuf, "Option data indication test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		if (p->optdata_ind.DATA_flag & T_ODF_MORE)
			print_tx_prim(child, "T_OPTDATA_IND+  ");
		else
			print_tx_prim(child, "T_OPTDATA_IND   ");
		print_options(child, cbuf + p->optdata_ind.OPT_offset, p->optdata_ind.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXP_OPTDATA_IND:
		ctrl->len = sizeof(p->optdata_ind);
		p->optdata_ind.PRIM_type = T_OPTDATA_IND;
		p->optdata_ind.DATA_flag = T_ODF_EX;
		p->optdata_ind.OPT_length = 0;
		p->optdata_ind.OPT_offset = 0;
		data->len = sprintf(dbuf, "Option data indication test data.");
		test_pflags = MSG_BAND;
		test_pband = 1;
		if (p->optdata_ind.DATA_flag & T_ODF_MORE)
			print_tx_prim(child, "T_OPTDATA_IND!+ ");
		else
			print_tx_prim(child, "T_OPTDATA_IND!  ");
		print_options(child, cbuf + p->optdata_ind.OPT_offset, p->optdata_ind.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ADDR_REQ:
		ctrl->len = sizeof(p->addr_req);
		p->addr_req.PRIM_type = T_ADDR_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ADDR_ACK:
		ctrl->len = sizeof(p->addr_ack);
		p->addr_ack.PRIM_type = T_ADDR_ACK;
		p->addr_ack.LOCADDR_length = 0;
		p->addr_ack.LOCADDR_offset = 0;
		p->addr_ack.REMADDR_length = 0;
		p->addr_ack.REMADDR_offset = 0;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, addr_string(cbuf + p->addr_ack.LOCADDR_offset, p->addr_ack.LOCADDR_length));
		print_string(child, addr_string(cbuf + p->addr_ack.REMADDR_offset, p->addr_ack.REMADDR_length));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CAPABILITY_REQ:
		ctrl->len = sizeof(p->capability_req);
		p->capability_req.PRIM_type = T_CAPABILITY_REQ;
		p->capability_req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CAPABILITY_ACK:
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
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_O_TI_GETINFO:
		ic.ic_cmd = O_TI_GETINFO;
		ic.ic_len = sizeof(p->info_ack);
		p->info_req.PRIM_type = T_INFO_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
	case __TEST_O_TI_OPTMGMT:
		ic.ic_cmd = O_TI_OPTMGMT;
		ic.ic_len = sizeof(p->optmgmt_ack)
		    + (test_opt_optm ? sizeof(*test_opt_optm) : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opt_optm ? sizeof(*test_opt_optm) : 0;
		p->optmgmt_req.OPT_offset = test_opt_optm ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opt_optm)
			bcopy(test_opt_optm, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
	case __TEST_O_TI_BIND:
		ic.ic_cmd = O_TI_BIND;
		ic.ic_len = sizeof(p->bind_ack);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = 0;
		p->bind_req.ADDR_offset = 0;
		p->bind_req.CONIND_number = last_qlen;
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
		ic.ic_len = sizeof(p->optmgmt_ack)
		    + (test_opt_optm ? sizeof(*test_opt_optm) : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opt_optm ? sizeof(*test_opt_optm) : 0;
		p->optmgmt_req.OPT_offset = test_opt_optm ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opt_optm)
			bcopy(test_opt_optm, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
	case __TEST__O_TI_BIND:
		ic.ic_cmd = _O_TI_BIND;
		ic.ic_len = sizeof(p->bind_ack);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = 0;
		p->bind_req.ADDR_offset = 0;
		p->bind_req.CONIND_number = last_qlen;
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
		ic.ic_len = sizeof(p->optmgmt_ack)
		    + (test_opt_optm ? sizeof(*test_opt_optm) : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opt_optm ? sizeof(*test_opt_optm) : 0;
		p->optmgmt_req.OPT_offset = test_opt_optm ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opt_optm)
			bcopy(test_opt_optm, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
	case __TEST_TI_BIND:
		ic.ic_cmd = TI_BIND;
		ic.ic_len = sizeof(p->bind_ack);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = 0;
		p->bind_req.ADDR_offset = 0;
		p->bind_req.CONIND_number = last_qlen;
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
		p->conn_res.SEQ_number = last_sequence;
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
		p->discon_req.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
	case __TEST_TI_SETPEERNAME_DISC:
		ic.ic_cmd = TI_SETPEERNAME;
		ic.ic_len = sizeof(p->discon_req);
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
	case __TEST_TI_SETMYNAME_DATA:
		ic.ic_cmd = TI_SETMYNAME;
		ic.ic_len = sizeof(p->conn_res) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
		p->conn_res.PRIM_type = T_CONN_RES;
		p->conn_res.ACCEPTOR_id = 0;
		p->conn_res.OPT_length = 0;
		p->conn_res.OPT_offset = 0;
		p->conn_res.SEQ_number = last_sequence;
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
		p->discon_req.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) & ic);
	case __TEST_TI_SETPEERNAME_DISC_DATA:
		ic.ic_cmd = TI_SETPEERNAME;
		ic.ic_len = sizeof(p->discon_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
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
	return __RESULT_SCRIPT_ERROR;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Received event decoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

static int do_decode_data(int child, struct strbuf *data)
{
	int event = __RESULT_DECODE_ERROR;
	if (data->len >= 0) {
		event = __TEST_DATA;
		print_rx_prim(child, "DATA------------");
	}
	return ((last_event = event));
}

static int do_decode_ctrl(int child, struct strbuf *ctrl, struct strbuf *data)
{
	int event = __RESULT_DECODE_ERROR;
	union T_primitives *p = (union T_primitives *) ctrl->buf;
	if (ctrl->len >= sizeof(p->type)) {
		switch ((last_prim = p->type)) {
		case T_CONN_REQ:
			event = __TEST_CONN_REQ;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf + p->conn_req.OPT_offset, p->conn_req.OPT_length);
			break;
		case T_CONN_RES:
			event = __TEST_CONN_RES;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf + p->conn_res.OPT_offset, p->conn_res.OPT_length);
			break;
		case T_DISCON_REQ:
			event = __TEST_DISCON_REQ;
			last_sequence = p->discon_req.SEQ_number;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_DATA_REQ:
			event = __TEST_DATA_REQ;
			MORE_flag = p->data_req.MORE_flag;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_EXDATA_REQ:
			event = __TEST_EXDATA_REQ;
			MORE_flag = p->data_req.MORE_flag;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_INFO_REQ:
			event = __TEST_INFO_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_BIND_REQ:
			event = __TEST_BIND_REQ;
			last_qlen = p->bind_req.CONIND_number;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_UNBIND_REQ:
			event = __TEST_UNBIND_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_UNITDATA_REQ:
			event = __TEST_UNITDATA_REQ;
			print_rx_prim(child, prim_string(p->type));
			print_string(child, addr_string(cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length));
			print_options(child, cbuf + p->unitdata_req.OPT_offset, p->unitdata_req.OPT_length);
			break;
		case T_OPTMGMT_REQ:
			event = __TEST_OPTMGMT_REQ;
			print_rx_prim(child, prim_string(p->type));
			print_mgmtflag(child, p->optmgmt_req.MGMT_flags);
			print_options(child, cbuf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
			break;
		case T_ORDREL_REQ:
			event = __TEST_ORDREL_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_OPTDATA_REQ:
			event = __TEST_OPTDATA_REQ;
			if (p->optdata_req.DATA_flag & T_ODF_EX) {
				if (p->optdata_req.DATA_flag & T_ODF_MORE)
					print_rx_prim(child, "T_OPTDATA_REQ!+ ");
				else
					print_rx_prim(child, "T_OPTDATA_REQ!  ");
			} else {
				if (p->optdata_req.DATA_flag & T_ODF_MORE)
					print_rx_prim(child, "T_OPTDATA_REQ+  ");
				else
					print_rx_prim(child, "T_OPTDATA_REQ   ");
			}
			print_options(child, cbuf + p->optdata_req.OPT_offset, p->optdata_req.OPT_length);
			break;
		case T_ADDR_REQ:
			event = __TEST_ADDR_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_CAPABILITY_REQ:
			event = __TEST_CAPABILITY_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_CONN_IND:
			event = __TEST_CONN_IND;
			last_sequence = p->conn_ind.SEQ_number;
			print_rx_prim(child, prim_string(p->type));
			print_string(child, addr_string(cbuf + p->conn_ind.SRC_offset, p->conn_ind.SRC_length));
			print_options(child, cbuf + p->conn_ind.OPT_offset, p->conn_ind.OPT_length);
			break;
		case T_CONN_CON:
			event = __TEST_CONN_CON;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf + p->conn_con.OPT_offset, p->conn_con.OPT_length);
			break;
		case T_DISCON_IND:
			event = __TEST_DISCON_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_DATA_IND:
			event = __TEST_DATA_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_EXDATA_IND:
			event = __TEST_EXDATA_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_INFO_ACK:
			event = __TEST_INFO_ACK;
			print_ack_prim(child, prim_string(p->type));
			break;
		case T_BIND_ACK:
			event = __TEST_BIND_ACK;
			last_qlen = p->bind_ack.CONIND_number;
			print_ack_prim(child, prim_string(p->type));
			print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
			break;
		case T_ERROR_ACK:
			event = __TEST_ERROR_ACK;
			last_t_errno = p->error_ack.TLI_error;
			last_errno = p->error_ack.UNIX_error;
			print_ack_prim(child, prim_string(p->type));
			print_string(child, terrno_string(p->error_ack.TLI_error, p->error_ack.UNIX_error));
			break;
		case T_OK_ACK:
			event = __TEST_OK_ACK;
			print_ack_prim(child, prim_string(p->type));
			break;
		case T_UNITDATA_IND:
			event = __TEST_UNITDATA_IND;
			print_rx_prim(child, prim_string(p->type));
			print_string(child, addr_string(cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length));
			print_options(child, cbuf + p->unitdata_ind.OPT_offset, p->unitdata_ind.OPT_length);
			break;
		case T_UDERROR_IND:
			event = __TEST_UDERROR_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_OPTMGMT_ACK:
			event = __TEST_OPTMGMT_ACK;
			print_ack_prim(child, prim_string(p->type));
			print_mgmtflag(child, p->optmgmt_ack.MGMT_flags);
			print_options(child, cbuf + p->optmgmt_ack.OPT_offset, p->optmgmt_ack.OPT_length);
			break;
		case T_ORDREL_IND:
			event = __TEST_ORDREL_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_OPTDATA_IND:
			if (p->optdata_ind.DATA_flag & T_ODF_EX) {
				event = __TEST_EXP_OPTDATA_IND;
				if (p->optdata_ind.DATA_flag & T_ODF_MORE)
					print_rx_prim(child, "T_OPTDATA_IND!+ ");
				else
					print_rx_prim(child, "T_OPTDATA_IND!  ");
			} else {
				event = __TEST_NRM_OPTDATA_IND;
				if (p->optdata_ind.DATA_flag & T_ODF_MORE)
					print_rx_prim(child, "T_OPTDATA_IND+  ");
				else
					print_rx_prim(child, "T_OPTDATA_IND   ");
			}
			print_options(child, cbuf + p->optdata_ind.OPT_offset, p->optdata_ind.OPT_length);
			break;
		case T_ADDR_ACK:
			event = __TEST_ADDR_ACK;
			print_ack_prim(child, prim_string(p->type));
			print_string(child, addr_string(cbuf + p->addr_ack.LOCADDR_offset, p->addr_ack.LOCADDR_length));
			print_string(child, addr_string(cbuf + p->addr_ack.REMADDR_offset, p->addr_ack.REMADDR_length));
			break;
		case T_CAPABILITY_ACK:
			event = __TEST_CAPABILITY_ACK;
			print_ack_prim(child, prim_string(p->type));
			break;
		default:
			event = __EVENT_UNKNOWN;
			print_no_prim(child, p->type);
			break;
		}
		if (data && data->len >= 0)
			if (do_decode_data(child, data) != __TEST_DATA)
				event = __RESULT_FAILURE;
	}
	return ((last_event = event));
}

static int do_decode_msg(int child, struct strbuf *ctrl, struct strbuf *data)
{
	if (ctrl->len > 0) {
		if ((last_event = do_decode_ctrl(child, ctrl, data)) != __EVENT_UNKNOWN)
			return time_event(last_event);
	} else if (data->len > 0) {
		if ((last_event = do_decode_data(child, data)) != __EVENT_UNKNOWN)
			return time_event(last_event);
	}
	return ((last_event = __EVENT_NO_MSG));
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
					if ((last_event = do_decode_msg(0, &ctrl, &data)) != __EVENT_NO_MSG)
						return time_event(last_event);
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
						fprintf(stdout, "gotmsg from bot [%d,%d,%d]:\n", ctrl.len, data.len, flags);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if ((last_event = do_decode_msg(1, &ctrl, &data)) != __EVENT_NO_MSG)
						return time_event(last_event);
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
		if (verbose > 4)
			print_syscall(child, "poll()");
		pfd[0].fd = test_fd[child];
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			print_errno(child, (last_errno = errno));
			if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
				break;
			return (__RESULT_FAILURE);
		case 0:
			if (verbose > 4)
				print_success(child);
			print_nothing(child);
			last_event = __EVENT_NO_MSG;
			return time_event(__EVENT_NO_MSG);
		case 1:
			if (verbose > 4)
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
					if (verbose > 4)
						print_syscall(child, "getmsg()");
					if ((ret = getmsg(test_fd[child], &ctrl, &data, &flags)) >= 0)
						break;
					print_errno(child, (last_errno = errno));
					if (last_errno == EINTR || last_errno == ERESTART)
						continue;
					if (last_errno == EAGAIN)
						break;
					return __RESULT_FAILURE;
				}
				if (ret < 0)
					break;
				if (ret == 0) {
					if (verbose > 4)
						print_success(child);
					if (verbose > 4) {
						lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from %d [%d,%d]:\n", child, ctrl.len, data.len);
						fflush(stdout);
						lockf(fileno(stdout), F_ULOCK, 0);
					}
					if ((last_event = do_decode_msg(child, &ctrl, &data)) != __EVENT_NO_MSG)
						return time_event(last_event);
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

int expect(int child, int wait, int want)
{
	if ((last_event = wait_event(child, wait)) == want)
		return (__RESULT_SUCCESS);
	print_expect(child, want);
	return (__RESULT_FAILURE);
}

void test_sleep(int child, unsigned long t)
{
	print_waiting(child, t);
	sleep(t);
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
	return (__RESULT_SUCCESS);
}

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
	return (__RESULT_SUCCESS);
}

static int preamble_1(int child)
{
	test_mgmtflags = T_NEGOTIATE;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_addr = &addrs[child];
	test_alen = sizeof(addrs[child]);
	last_qlen = 0;
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int preamble_1s(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int postamble_1(int child)
{
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

static int postamble_1e(int child)
{
	if (do_signal(child, __TEST_UNBIND_REQ) == __RESULT_SUCCESS || last_errno != EPROTO) {
		expect(child, SHORT_WAIT, __TEST_OK_ACK);
		goto failure;
	}
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#if 0
static int preamble_2_conn(int child)
{
	if (preamble_1s(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_addr = &addrs[2];
	test_alen = sizeof(addrs[2]);
	test_data = NULL;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int preamble_2_resp(int child)
{
	if (preamble_1s(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int preamble_2_list(int child)
{
	if (preamble_1s(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_resfd = test_fd[1];
	test_data = NULL;
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_sleep(child, 1);
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int postamble_2_conn(int child)
{
	int failed = -1;
	test_data = NULL;
	last_sequence = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (postamble_1(child) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}

static int postamble_2_resp(int child)
{
	int failed = -1;
	if (expect(child, LONG_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	test_data = NULL;
	last_sequence = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (postamble_1(child) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}

static int postamble_2_list(int child)
{
	int failed = -1;
	if (postamble_1(child) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}

#if 0
static int preamble_2b_conn(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_addr = &addrs[1];
	test_alen = sizeof(addrs[1]);
	test_data = "Hello World";
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int preamble_2b_resp(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_EXDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int preamble_2b_list(int child)
{
	if (preamble_1(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_resfd = test_fd[1];
	test_data = "Hello There!";
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
#endif

static int postamble_3_conn(int child)
{
	int failed = -1;
	test_data = NULL;
	if (do_signal(child, __TEST_ORDREL_REQ) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(child, LONG_WAIT, __TEST_ORDREL_IND) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1) {
		state += 10;
		if (postamble_2_conn(child) != __RESULT_SUCCESS)
			failed = (failed == -1) ? state : failed;
	} else {
		if (postamble_1(child) != __RESULT_SUCCESS)
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

static int postamble_3_resp(int child)
{
	int failed = -1;
	if (expect(child, LONG_WAIT, __TEST_ORDREL_IND) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	test_data = NULL;
	if (do_signal(child, __TEST_ORDREL_REQ) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (expect(child, LONG_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
		failed = (failed == -1) ? state : failed;
	state++;
	if (failed != -1) {
		state += 10;
		if (postamble_2_resp(child) != __RESULT_SUCCESS)
			failed = (failed == -1) ? state : failed;
	} else {
		if (postamble_1(child) != __RESULT_SUCCESS)
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

static int postamble_3_list(int child)
{
	return postamble_2_list(child);
}
#endif

#if 0
static int preamble_3b_conn(int child)
{
	opt_optm.rcv_val = T_YES;
	opt_optm.ist_val = 32;
	opt_optm.ost_val = 32;
	opt_conn.ist_val = 32;
	opt_conn.ost_val = 32;
	return preamble_2_conn(child);
}
static int preamble_3b_resp(int child)
{
	opt_optm.rcv_val = T_YES;
	opt_optm.ist_val = 32;
	opt_optm.ost_val = 32;
	opt_conn.ist_val = 32;
	opt_conn.ost_val = 32;
	return preamble_2_resp(child);
}
static int preamble_3b_list(int child)
{
	opt_optm.rcv_val = T_YES;
	opt_optm.ist_val = 32;
	opt_optm.ost_val = 32;
	opt_conn.ist_val = 32;
	opt_conn.ost_val = 32;
	return preamble_2_list(child);
}

static int preamble_4_conn(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_DROPPING;
	return preamble_2_conn(child);
}
static int preamble_4_list(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_DROPPING;
	return preamble_2_list(child);
}
static int preamble_4_resp(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_DROPPING;
	return preamble_2_resp(child);
}

static int preamble_4b_conn(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_2_conn(child);
}
static int preamble_4b_list(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_2_list(child);
}
static int preamble_4b_resp(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_2_resp(child);
}

static int preamble_5_conn(int child)
{
	// opt_optm.dbg_val = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	opt_optm.dbg_val = SCTP_OPTION_BREAK;
	return preamble_2_conn(child);
}
static int preamble_5_list(int child)
{
	// opt_optm.dbg_val = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	opt_optm.dbg_val = SCTP_OPTION_BREAK;
	return preamble_2_list(child);
}
static int preamble_5_resp(int child)
{
	// opt_optm.dbg_val = SCTP_OPTION_BREAK|SCTP_OPTION_DBREAK|SCTP_OPTION_DROPPING;
	opt_optm.dbg_val = SCTP_OPTION_BREAK;
	return preamble_2_resp(child);
}

static int preamble_6_conn(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_3b_conn(child);
}
static int preamble_6_list(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_3b_list(child);
}
static int preamble_6_resp(int child)
{
	opt_optm.dbg_val = SCTP_OPTION_RANDOM;
	return preamble_3b_resp(child);
}

static int preamble_7_conn(int child)
{
	opt_optm.mac_val = T_SCTP_HMAC_SHA1;
	return preamble_1(child);
}
static int preamble_7_list(int child)
{
	opt_optm.mac_val = T_SCTP_HMAC_SHA1;
	return preamble_1(child);
}
static int preamble_7_resp(int child)
{
	opt_optm.mac_val = T_SCTP_HMAC_SHA1;
	return preamble_1(child);
}

static int preamble_8_conn(int child)
{
	opt_optm.mac_val = T_SCTP_HMAC_MD5;
	return preamble_1(child);
}
static int preamble_8_list(int child)
{
	opt_optm.mac_val = T_SCTP_HMAC_MD5;
	return preamble_1(child);
}
static int preamble_8_resp(int child)
{
	opt_optm.mac_val = T_SCTP_HMAC_MD5;
	return preamble_1(child);
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

/*
 *  Open and Close 3 streams.
 */
#define test_group_1 "Opening and closing streams"
#define tgrp_case_1_1 test_group_1
#define numb_case_1_1 "1.1"
#define name_case_1_1 "Open and close 3 streams"
#define desc_case_1_1 "\
Checks that three streams can be opened and closed."

int test_case_1_1(int child)
{
	return (__RESULT_SUCCESS);
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

struct test_stream test_1_1_conn = { &preamble_1_1_conn, &test_case_1_1_conn, &postamble_1_1_conn };
struct test_stream test_1_1_resp = { &preamble_1_1_resp, &test_case_1_1_resp, &postamble_1_1_resp };
struct test_stream test_1_1_list = { &preamble_1_1_list, &test_case_1_1_list, &postamble_1_1_list };

/*
 *  Request information.
 */
#define tgrp_case_1_2 test_group_1
#define numb_case_1_2 "1.2"
#define name_case_1_2 "Request information."
#define desc_case_1_2 "\
Checks that information can be requested on each of three streasm."

int test_case_1_2(int child)
{
	if (do_signal(child, __TEST_INFO_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_2_conn	test_case_1_2
#define test_case_1_2_resp	test_case_1_2
#define test_case_1_2_list	test_case_1_2

#define preamble_1_2_conn	preamble_0
#define preamble_1_2_resp	preamble_0
#define preamble_1_2_list	preamble_0

#define postamble_1_2_conn	postamble_0
#define postamble_1_2_resp	postamble_0
#define postamble_1_2_list	postamble_0

struct test_stream test_1_2_conn = { &preamble_1_2_conn, &test_case_1_2_conn, &postamble_1_2_conn };
struct test_stream test_1_2_resp = { &preamble_1_2_resp, &test_case_1_2_resp, &postamble_1_2_resp };
struct test_stream test_1_2_list = { &preamble_1_2_list, &test_case_1_2_list, &postamble_1_2_list };

/*
 *  Request capabilities.
 */
#define tgrp_case_1_3 test_group_1
#define numb_case_1_3 "1.3"
#define name_case_1_3 "Request capabilities."
#define desc_case_1_3 "\
Checks that capabilities can be requested on each of three streasm."

int test_case_1_3(int child)
{
	if (do_signal(child, __TEST_CAPABILITY_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_CAPABILITY_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_3_conn	test_case_1_3
#define test_case_1_3_resp	test_case_1_3
#define test_case_1_3_list	test_case_1_3

#define preamble_1_3_conn	preamble_0
#define preamble_1_3_resp	preamble_0
#define preamble_1_3_list	preamble_0

#define postamble_1_3_conn	postamble_0
#define postamble_1_3_resp	postamble_0
#define postamble_1_3_list	postamble_0

struct test_stream test_1_3_conn = { &preamble_1_3_conn, &test_case_1_3_conn, &postamble_1_3_conn };
struct test_stream test_1_3_resp = { &preamble_1_3_resp, &test_case_1_3_resp, &postamble_1_3_resp };
struct test_stream test_1_3_list = { &preamble_1_3_list, &test_case_1_3_list, &postamble_1_3_list };

/*
 *  Request addresses.
 */
#define tgrp_case_1_4 test_group_1
#define numb_case_1_4 "1.4"
#define name_case_1_4 "Request addresses."
#define desc_case_1_4 "\
Checks that addresses can be requested on each of three streasm."

int test_case_1_4(int child)
{
	if (do_signal(child, __TEST_ADDR_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_ADDR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_1_4_conn	test_case_1_4
#define test_case_1_4_resp	test_case_1_4
#define test_case_1_4_list	test_case_1_4

#define preamble_1_4_conn	preamble_0
#define preamble_1_4_resp	preamble_0
#define preamble_1_4_list	preamble_0

#define postamble_1_4_conn	postamble_0
#define postamble_1_4_resp	postamble_0
#define postamble_1_4_list	postamble_0

struct test_stream test_1_4_conn = { &preamble_1_4_conn, &test_case_1_4_conn, &postamble_1_4_conn };
struct test_stream test_1_4_resp = { &preamble_1_4_resp, &test_case_1_4_resp, &postamble_1_4_resp };
struct test_stream test_1_4_list = { &preamble_1_4_list, &test_case_1_4_list, &postamble_1_4_list };

/*
 *  Do options management.
 */
#define test_group_2 "Options management"
#define tgrp_case_2_1 test_group_2
#define numb_case_2_1 "2.1"
#define name_case_2_1 "Perform options management."
#define desc_case_2_1 "\
Checks that options management can be performed on several streams."

int test_case_2_1(int child)
{
	test_mgmtflags = T_DEFAULT;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_mgmtflags = T_CURRENT;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_mgmtflags = T_CHECK;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_mgmtflags = T_NEGOTIATE;
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_2_1_conn	test_case_2_1
#define test_case_2_1_resp	test_case_2_1
#define test_case_2_1_list	test_case_2_1

#define preamble_2_1_conn	preamble_0
#define preamble_2_1_resp	preamble_0
#define preamble_2_1_list	preamble_0

#define postamble_2_1_conn	postamble_0
#define postamble_2_1_resp	postamble_0
#define postamble_2_1_list	postamble_0

struct test_stream test_2_1_conn = { &preamble_2_1_conn, &test_case_2_1_conn, &postamble_2_1_conn };
struct test_stream test_2_1_resp = { &preamble_2_1_resp, &test_case_2_1_resp, &postamble_2_1_resp };
struct test_stream test_2_1_list = { &preamble_2_1_list, &test_case_2_1_list, &postamble_2_1_list };

/*
 *  Bind and unbind three streams.
 */
#define tgrp_case_2_2 test_group_2
#define numb_case_2_2 "2.2"
#define name_case_2_2 "Bind and unbind three streams."
#define desc_case_2_2 "\
Checks that three streams can be bound and unbound.  One is bound to\n\
a normal address, another to a null address, the last to a wildcard\n\
address."

int test_case_2_2(int child, struct sockaddr_in *addr, socklen_t len)
{
	test_addr = addr;
	test_alen = len;
	last_qlen = 0;
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_ADDR_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_ADDR_ACK) != __RESULT_SUCCESS)
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

int test_case_2_2_conn(int child)
{
	return test_case_2_2(child, &addrs[0], sizeof(addrs[0]));
}
int test_case_2_2_resp(int child)
{
	return test_case_2_2(child, NULL, 0);
}
int test_case_2_2_list(int child)
{
	addrs[3].sin_addr.s_addr = INADDR_ANY;
	return test_case_2_2(child, &addrs[3], sizeof(addrs[3]));
}

#define preamble_2_2_conn	preamble_0
#define preamble_2_2_resp	preamble_0
#define preamble_2_2_list	preamble_0

#define postamble_2_2_conn	postamble_0
#define postamble_2_2_resp	postamble_0
#define postamble_2_2_list	postamble_0

struct test_stream test_2_2_conn = { &preamble_2_2_conn, &test_case_2_2_conn, &postamble_2_2_conn };
struct test_stream test_2_2_resp = { &preamble_2_2_resp, &test_case_2_2_resp, &postamble_2_2_resp };
struct test_stream test_2_2_list = { &preamble_2_2_list, &test_case_2_2_list, &postamble_2_2_list };

/*
 *  Transfer connectionless data.
 */
#define test_group_3 "Data transfer"
#define tgrp_case_3_1 test_group_3
#define numb_case_3_1 "3.1"
#define name_case_3_1 "Transfer connectionless data."
#define desc_case_3_1 "\
Attempts to transfer connectionless data."

int test_case_3_1(int child, struct sockaddr_in *addr, socklen_t len)
{
	const char msg[] = "Unit test data.";
	test_addr = addr;
	test_alen = len;
	test_data = msg;
	if (do_signal(child, __TEST_UNITDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, LONG_WAIT, __TEST_UNITDATA_IND) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int test_case_3_1_conn(int child)
{
	return test_case_3_1(child, &addrs[1], sizeof(addrs[1]));
}
int test_case_3_1_resp(int child)
{
	return test_case_3_1(child, &addrs[2], sizeof(addrs[2]));
}
int test_case_3_1_list(int child)
{
	return test_case_3_1(child, &addrs[0], sizeof(addrs[0]));
}

#define preamble_3_1_conn	preamble_1s
#define preamble_3_1_resp	preamble_1s
#define preamble_3_1_list	preamble_1s

#define postamble_3_1_conn	postamble_1
#define postamble_3_1_resp	postamble_1
#define postamble_3_1_list	postamble_1

struct test_stream test_3_1_conn = { &preamble_3_1_conn, &test_case_3_1_conn, &postamble_3_1_conn };
struct test_stream test_3_1_resp = { &preamble_3_1_resp, &test_case_3_1_resp, &postamble_3_1_resp };
struct test_stream test_3_1_list = { &preamble_3_1_list, &test_case_3_1_list, &postamble_3_1_list };

/*
 *  Negative test cases on connection oriented primitives.
 */
#define test_group_4 "Negative test cases"
#define tgrp_case_4_1 test_group_4
#define numb_case_4_1 "4.1"
#define name_case_4_1 "Unsupported T_CONN_REQ."
#define desc_case_4_1 "\
Attempts to invoke connection oriented primitives.\n\
- T_CONN_REQ."

int test_case_4_1(int child)
{
	static char dat[] = "Dummy message.";
	test_addr = &addrs[0];
	test_alen = sizeof(addrs[0]);
	test_data = dat;
	if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_4_1_conn	test_case_4_1
#define test_case_4_1_resp	test_case_4_1
#define test_case_4_1_list	test_case_4_1

#define preamble_4_1_conn	preamble_1
#define preamble_4_1_resp	preamble_1
#define preamble_4_1_list	preamble_1

#define postamble_4_1_conn	postamble_1
#define postamble_4_1_resp	postamble_1
#define postamble_4_1_list	postamble_1

struct test_stream test_4_1_conn = { &preamble_4_1_conn, &test_case_4_1_conn, &postamble_4_1_conn };
struct test_stream test_4_1_resp = { &preamble_4_1_resp, &test_case_4_1_resp, &postamble_4_1_resp };
struct test_stream test_4_1_list = { &preamble_4_1_list, &test_case_4_1_list, &postamble_4_1_list };

/*
 *  Negative test cases on connection oriented primitives.
 */
#define tgrp_case_4_2 test_group_4
#define numb_case_4_2 "4.2"
#define name_case_4_2 "Unsupported T_CONN_RES."
#define desc_case_4_2 "\
Attempts to invoke connection oriented primitives.\n\
- T_CONN_RES."

int test_case_4_2(int child)
{
	static char dat[] = "Dummy message.";
	test_data = dat;
	test_resfd = test_fd[child];
	if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_4_2_conn	test_case_4_2
#define test_case_4_2_resp	test_case_4_2
#define test_case_4_2_list	test_case_4_2

#define preamble_4_2_conn	preamble_1
#define preamble_4_2_resp	preamble_1
#define preamble_4_2_list	preamble_1

#define postamble_4_2_conn	postamble_1
#define postamble_4_2_resp	postamble_1
#define postamble_4_2_list	postamble_1

struct test_stream test_4_2_conn = { &preamble_4_2_conn, &test_case_4_2_conn, &postamble_4_2_conn };
struct test_stream test_4_2_resp = { &preamble_4_2_resp, &test_case_4_2_resp, &postamble_4_2_resp };
struct test_stream test_4_2_list = { &preamble_4_2_list, &test_case_4_2_list, &postamble_4_2_list };

/*
 *  Negative test cases on connection oriented primitives.
 */
#define tgrp_case_4_3 test_group_4
#define numb_case_4_3 "4.3"
#define name_case_4_3 "Unsupported T_DISCON_REQ."
#define desc_case_4_3 "\
Attempts to invoke connection oriented primitives.\n\
- T_DISCON_REQ."

int test_case_4_3(int child)
{
	static char dat[] = "Dummy message.";
	test_data = dat;
	last_sequence = 0;
	if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_ERROR_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_4_3_conn	test_case_4_3
#define test_case_4_3_resp	test_case_4_3
#define test_case_4_3_list	test_case_4_3

#define preamble_4_3_conn	preamble_1
#define preamble_4_3_resp	preamble_1
#define preamble_4_3_list	preamble_1

#define postamble_4_3_conn	postamble_1
#define postamble_4_3_resp	postamble_1
#define postamble_4_3_list	postamble_1

struct test_stream test_4_3_conn = { &preamble_4_3_conn, &test_case_4_3_conn, &postamble_4_3_conn };
struct test_stream test_4_3_resp = { &preamble_4_3_resp, &test_case_4_3_resp, &postamble_4_3_resp };
struct test_stream test_4_3_list = { &preamble_4_3_list, &test_case_4_3_list, &postamble_4_3_list };

/*
 *  Negative test cases on connection oriented primitives.
 */
#define tgrp_case_4_4 test_group_4
#define numb_case_4_4 "4.4"
#define name_case_4_4 "Unsupported T_DATA_REQ."
#define desc_case_4_4 "\
Attempts to invoke connection oriented primitives.\n\
- T_DATA_REQ."

int test_case_4_4(int child)
{
	static char dat[] = "Normal test message.";
	test_data = dat;
	MORE_flag = 0;
	if (do_signal(child, __TEST_DATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS || last_errno != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_4_4_conn	test_case_4_4
#define test_case_4_4_resp	test_case_4_4
#define test_case_4_4_list	test_case_4_4

#define preamble_4_4_conn	preamble_1
#define preamble_4_4_resp	preamble_1
#define preamble_4_4_list	preamble_1

#define postamble_4_4_conn	postamble_1e
#define postamble_4_4_resp	postamble_1e
#define postamble_4_4_list	postamble_1e

struct test_stream test_4_4_conn = { &preamble_4_4_conn, &test_case_4_4_conn, &postamble_4_4_conn };
struct test_stream test_4_4_resp = { &preamble_4_4_resp, &test_case_4_4_resp, &postamble_4_4_resp };
struct test_stream test_4_4_list = { &preamble_4_4_list, &test_case_4_4_list, &postamble_4_4_list };

/*
 *  Negative test cases on connection oriented primitives.
 */
#define tgrp_case_4_5 test_group_4
#define numb_case_4_5 "4.5"
#define name_case_4_5 "Unsupported T_EXDATA_REQ."
#define desc_case_4_5 "\
Attempts to invoke connection oriented primitives.\n\
- T_EXDATA_REQ."

int test_case_4_5(int child)
{
	static char dat[] = "Expedited test message.";
	test_data = dat;
	MORE_flag = 0;
	if (do_signal(child, __TEST_EXDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS || last_errno != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_4_5_conn	test_case_4_5
#define test_case_4_5_resp	test_case_4_5
#define test_case_4_5_list	test_case_4_5

#define preamble_4_5_conn	preamble_1
#define preamble_4_5_resp	preamble_1
#define preamble_4_5_list	preamble_1

#define postamble_4_5_conn	postamble_1e
#define postamble_4_5_resp	postamble_1e
#define postamble_4_5_list	postamble_1e

struct test_stream test_4_5_conn = { &preamble_4_5_conn, &test_case_4_5_conn, &postamble_4_5_conn };
struct test_stream test_4_5_resp = { &preamble_4_5_resp, &test_case_4_5_resp, &postamble_4_5_resp };
struct test_stream test_4_5_list = { &preamble_4_5_list, &test_case_4_5_list, &postamble_4_5_list };

/*
 *  Negative test cases on connection oriented primitives.
 */
#define tgrp_case_4_6 test_group_4
#define numb_case_4_6 "4.6"
#define name_case_4_6 "Unsupported T_OPTDATA_REQ."
#define desc_case_4_6 "\
Attempts to invoke connection oriented primitives.\n\
- T_OPTDATA_REQ."

int test_case_4_6(int child)
{
	static char dat[] = "Option data.";
	test_data = dat;
	DATA_flag = 0;
	if (do_signal(child, __TEST_OPTDATA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS || last_errno != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_4_6_conn	test_case_4_6
#define test_case_4_6_resp	test_case_4_6
#define test_case_4_6_list	test_case_4_6

#define preamble_4_6_conn	preamble_1
#define preamble_4_6_resp	preamble_1
#define preamble_4_6_list	preamble_1

#define postamble_4_6_conn	postamble_1e
#define postamble_4_6_resp	postamble_1e
#define postamble_4_6_list	postamble_1e

struct test_stream test_4_6_conn = { &preamble_4_6_conn, &test_case_4_6_conn, &postamble_4_6_conn };
struct test_stream test_4_6_resp = { &preamble_4_6_resp, &test_case_4_6_resp, &postamble_4_6_resp };
struct test_stream test_4_6_list = { &preamble_4_6_list, &test_case_4_6_list, &postamble_4_6_list };

/*
 *  Negative test cases on connection oriented primitives.
 */
#define tgrp_case_4_7 test_group_4
#define numb_case_4_7 "4.7"
#define name_case_4_7 "Unsupported T_ORDREL_REQ."
#define desc_case_4_7 "\
Attempts to invoke connection oriented primitives.\n\
- T_ORDREL_REQ."

int test_case_4_7(int child)
{
	static char dat[] = "Orderly release data.";
	test_data = dat;
	if (do_signal(child, __TEST_ORDREL_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __RESULT_FAILURE) != __RESULT_SUCCESS || last_errno != EPROTO)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

#define test_case_4_7_conn	test_case_4_7
#define test_case_4_7_resp	test_case_4_7
#define test_case_4_7_list	test_case_4_7

#define preamble_4_7_conn	preamble_1
#define preamble_4_7_resp	preamble_1
#define preamble_4_7_list	preamble_1

#define postamble_4_7_conn	postamble_1e
#define postamble_4_7_resp	postamble_1e
#define postamble_4_7_list	postamble_1e

struct test_stream test_4_7_conn = { &preamble_4_7_conn, &test_case_4_7_conn, &postamble_4_7_conn };
struct test_stream test_4_7_resp = { &preamble_4_7_resp, &test_case_4_7_resp, &postamble_4_7_resp };
struct test_stream test_4_7_list = { &preamble_4_7_list, &test_case_4_7_list, &postamble_4_7_list };

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
 *  The conn child (child[0]) is the connecting process, the resp child
 *  (child[1]) is the responding process.
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
	for (; children > 0; children--) {
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
				last_event = __EVENT_TIMEOUT;
			}
			if (child[0]) {
				kill(child[0], SIGKILL);
				status[0] = __RESULT_INCONCLUSIVE;
				child[0] = 0;
			}
			if (child[1]) {
				kill(child[1], SIGKILL);
				status[1] = __RESULT_INCONCLUSIVE;
				child[1] = 0;
			}
			if (child[2]) {
				kill(child[2], SIGKILL);
				status[2] = __RESULT_INCONCLUSIVE;
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
		numb_case_1_1, tgrp_case_1_1, name_case_1_1, desc_case_1_1, { &test_1_1_conn, &test_1_1_resp, &test_1_1_list }, 0, 0}, {
		numb_case_1_2, tgrp_case_1_2, name_case_1_2, desc_case_1_2, { &test_1_2_conn, &test_1_2_resp, &test_1_2_list }, 0, 0}, {
		numb_case_1_3, tgrp_case_1_3, name_case_1_3, desc_case_1_3, { &test_1_3_conn, &test_1_3_resp, &test_1_3_list }, 0, 0}, {
		numb_case_1_4, tgrp_case_1_4, name_case_1_4, desc_case_1_4, { &test_1_4_conn, &test_1_4_resp, &test_1_4_list }, 0, 0}, {
		numb_case_2_1, tgrp_case_2_1, name_case_2_1, desc_case_2_1, { &test_2_1_conn, &test_2_1_resp, &test_2_1_list }, 0, 0}, {
		numb_case_2_2, tgrp_case_2_2, name_case_2_2, desc_case_2_2, { &test_2_2_conn, &test_2_2_resp, &test_2_2_list }, 0, 0}, {
		numb_case_3_1, tgrp_case_3_1, name_case_3_1, desc_case_3_1, { &test_3_1_conn, &test_3_1_resp, &test_3_1_list }, 0, 0}, {
		numb_case_4_1, tgrp_case_4_1, name_case_4_1, desc_case_4_1, { &test_4_1_conn, &test_4_1_resp, &test_4_1_list }, 0, 0}, {
		numb_case_4_2, tgrp_case_4_2, name_case_4_2, desc_case_4_2, { &test_4_2_conn, &test_4_2_resp, &test_4_2_list }, 0, 0}, {
		numb_case_4_3, tgrp_case_4_3, name_case_4_3, desc_case_4_3, { &test_4_3_conn, &test_4_3_resp, &test_4_3_list }, 0, 0}, {
		numb_case_4_4, tgrp_case_4_4, name_case_4_4, desc_case_4_4, { &test_4_4_conn, &test_4_4_resp, &test_4_4_list }, 0, 0}, {
		numb_case_4_5, tgrp_case_4_5, name_case_4_5, desc_case_4_5, { &test_4_5_conn, &test_4_5_resp, &test_4_5_list }, 0, 0}, {
		numb_case_4_6, tgrp_case_4_6, name_case_4_6, desc_case_4_6, { &test_4_6_conn, &test_4_6_resp, &test_4_6_list }, 0, 0}, {
		numb_case_4_7, tgrp_case_4_7, name_case_4_7, desc_case_4_7, { &test_4_7_conn, &test_4_7_resp, &test_4_7_list }, 0, 0}, {
	NULL,}
};

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
	if (begin_tests(0) == __RESULT_SUCCESS) {
		end_tests();
		show = 1;
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
			if (tests[i].result)
				continue;
			if (!tests[i].run) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				skipped++;
				continue;
			}
			if (aborted) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				inconclusive++;
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
			if ((result = begin_tests(i)) != __RESULT_SUCCESS)
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
			fprintf(stdout, "========= %3d successes   \n", successes);
			fprintf(stdout, "========= %3d failures    \n", failures);
			fprintf(stdout, "========= %3d inconclusive\n", inconclusive);
			fprintf(stdout, "========= %3d skipped     \n", skipped);
			fprintf(stdout, "==========================\n");
			fprintf(stdout, "========= %3d total       \n", successes + failures + inconclusive + skipped);
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
    See `%1$s --copying' for copying permission.\n\
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
        print copying permission and exit\n\
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
